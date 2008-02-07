/*
 * tclXprofile.c --
 *
 * Tcl performance profile monitor.
 *-----------------------------------------------------------------------------
 * Copyright 1992 Karl Lehenbauer and Mark Diekhans.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies.  Karl Lehenbauer and
 * Mark Diekhans make no representations about the suitability of this
 * software for any purpose.  It is provided "as is" without express or
 * implied warranty.
 *-----------------------------------------------------------------------------
 * $Id: tclXprofile.c,v 2.0 1992/10/16 04:51:05 markd Rel $
 *-----------------------------------------------------------------------------
 */

#include "tclhash.h"
#include "tclxint.h"

/*
 * Stack entry used to keep track of an profiling information for active
 * procedure.  Handling uplevels is tricky.  The eval level and procedure call
 * level are kept track of.  These are used to distinguish between an uplevel
 * and exiting a procedure.  During an uplevel, the invisible part of the
 * profile stack is saved on another stack until the uplevel completes.
 */

typedef struct profStackEntry_t {
    long                     realTime;      /* Real time at procedure entry. */
    long                     cpuTime;       /* CPU time at procedure entry.  */
    int                      procLevel;     /* Call level of this procedure  */
    int                      evalLevel;     /* Eval level of this prodecure  */
    struct profStackEntry_t *prevEntryPtr;  /* Previous stack entry.         */
    char                     procName [1];  /* Procedure name. MUST BE LAST! */
} profStackEntry_t;


/*
 * Save stack entry used to hold profile stack entries during an uplevel.
 */

typedef struct saveStackEntry_t {
    profStackEntry_t         *topPtr;        /* Top of saved stack section   */
    profStackEntry_t         *bottomPtr;     /* Bottom of saved stack        */
    struct saveStackEntry_t  *prevEntryPtr;  /* Previous saved stack section */
} saveStackEntry_t;

/*
 * Data keeped on a stack snapshot.
 */

typedef struct profDataEntry_t {
    long count;
    long realTime;
    long cpuTime;
} profDataEntry_t;

/*
 * Client data structure for profile command.  A count of real and CPU time
 * spent outside of the profiling routines is kept to factor out the variable
 * overhead.
 */

typedef struct profInfo_t { 
    Tcl_Interp       *interp;            /* Interpreter this is for.         */
    Tcl_Trace         traceHolder;       /* Handle to current trace.         */
    int               allCommands;       /* Prof all commands, not just procs*/
    long              realTime;          /* Real and CPU time counter.       */
    long              cpuTime;
    long              lastRealTime;      /* Real and CPU time of last exit   */
    long              lastCpuTime;       /* from profiling routines.         */
    profStackEntry_t *stackPtr;          /* Pointer to the top of prof stack */
    saveStackEntry_t *saveStackPtr;      /* Frames saved during an uplevel   */
    Tcl_HashTable     profDataTable;     /* Cumulative time table, Keyed by  */
                                         /* call stack list.                 */
} profInfo_t;

/*
 * Prototypes of internal functions.
 */

static void
ProcEntry _ANSI_ARGS_((profInfo_t *infoPtr,
                       char       *procName,
                       int         procLevel,
                       int         evalLevel));

static void
ProcPopEntry _ANSI_ARGS_((profInfo_t *infoPtr));

static void
StackSync _ANSI_ARGS_((profInfo_t *infoPtr,
                       int         procLevel,
                       int         evalLevel));

static void
DoUplevel _ANSI_ARGS_((profInfo_t *infoPtr,
                       int         procLevel));

static void
ProfTraceRoutine _ANSI_ARGS_((ClientData    clientData,
                              Tcl_Interp   *interp,
                              int           evalLevel,
                              char         *command,
                              int           (*cmdProc)(),
                              ClientData    cmdClientData,
                              int           argc,
                              char        **argv));

static void
CleanDataTable _ANSI_ARGS_((profInfo_t *infoPtr));

static void
DeleteProfTrace _ANSI_ARGS_((profInfo_t *infoPtr));

static int
DumpTableData  _ANSI_ARGS_((Tcl_Interp *interp,
                            profInfo_t *infoPtr,
                            char       *varName));

static int
Tcl_ProfileCmd _ANSI_ARGS_((ClientData    clientData,
                            Tcl_Interp   *interp,
                            int           argc,
                            char        **argv));

static void
CleanUpProfMon _ANSI_ARGS_((ClientData clientData));


/*
 *-----------------------------------------------------------------------------
 *
 * ProcEntry --
 *   Push a procedure entry onto the stack.
 *
 * Parameters:
 *   o infoPtr (I/O) - The global profiling info.
 *   o procName (I)  The procedure name.
 *   o procLevel (I) - The procedure call level that the procedure will
 *     execute at.
 *   o evalLevel (I) - The eval level that the procedure will start
 *     executing at.
 *-----------------------------------------------------------------------------
 */
static void
ProcEntry (infoPtr, procName, procLevel, evalLevel)
    profInfo_t *infoPtr;
    char       *procName;
    int         procLevel;
    int         evalLevel;
{
    profStackEntry_t *entryPtr;

    /*
     * Calculate the size of an entry.  One byte for name is in the entry.
     */
    entryPtr = (profStackEntry_t *) ckalloc (sizeof (profStackEntry_t) +
                                             strlen (procName));
    
    /*
     * Fill it in and push onto the stack.  Note that the procedures frame has
     * not yet been layed down or the procedure body eval execute, so the value
     * they will be in the procedure is recorded.
     */
    entryPtr->realTime     = infoPtr->realTime;
    entryPtr->cpuTime      = infoPtr->cpuTime;
    entryPtr->procLevel    = procLevel;
    entryPtr->evalLevel    = evalLevel;
    strcpy (entryPtr->procName, procName);

    entryPtr->prevEntryPtr  = infoPtr->stackPtr;
    infoPtr->stackPtr       = entryPtr;
}

/*
 *-----------------------------------------------------------------------------
 *
 * ProcPopEntry --
 *   Pop the procedure entry from the top of the stack and record its
 * times in the data table.
 *
 * Parameters:
 *   o infoPtr (I/O) - The global profiling info.
 *-----------------------------------------------------------------------------
 */
static void
ProcPopEntry (infoPtr)
    profInfo_t *infoPtr;
{
    profStackEntry_t *entryPtr = infoPtr->stackPtr;
    profStackEntry_t *scanPtr;
    int               idx, newEntry;
    char             *stackListPtr;
    Tcl_HashEntry    *hashEntryPtr;
    profDataEntry_t  *dataEntryPtr;
    char             *stackArgv [MAX_NESTING_DEPTH];

    /*
     * Build up a stack list.  Entry [0] is the top of the stack.
     */
    idx= 0;
    scanPtr = entryPtr;
    while (scanPtr != NULL) {
        stackArgv [idx] = scanPtr->procName;
        idx++;
        scanPtr = scanPtr->prevEntryPtr;
    }
    stackListPtr = Tcl_Merge (idx, stackArgv);

    /*
     * Check the hash table for this entry, either finding an existing or
     * creating a new hash entry.
     */

    hashEntryPtr = Tcl_CreateHashEntry (&infoPtr->profDataTable,
                                        stackListPtr,
                                        &newEntry);
    ckfree (stackListPtr);

    /*
     * Fill in or increment the entry.
     */
    if (newEntry) {
        dataEntryPtr = (profDataEntry_t *) ckalloc (sizeof (profDataEntry_t));
        Tcl_SetHashValue (hashEntryPtr, dataEntryPtr);
        dataEntryPtr->count    = 0;
        dataEntryPtr->realTime = 0;
        dataEntryPtr->cpuTime  = 0;;
    } else
        dataEntryPtr = (profDataEntry_t *) Tcl_GetHashValue (hashEntryPtr);

    dataEntryPtr->count++;
    dataEntryPtr->realTime += (infoPtr->realTime - entryPtr->realTime);
    dataEntryPtr->cpuTime  += (infoPtr->cpuTime  - entryPtr->cpuTime);


    infoPtr->stackPtr = entryPtr->prevEntryPtr;
    ckfree ((char *) entryPtr);
}

/*
 *-----------------------------------------------------------------------------
 *
 * StackSync --
 *   Synchronize the profile stack with the interpreter procedure stack.
 * This is done once return from uplevels, exits and error unwinds are
 * detected (the command after).  Saved profile stack entries may be
 * restored and procedure entries popped from the stack.  When entries
 * are popped, their statistics is saved in stack.
 *
 * Parameters:
 *   o infoPtr (I/O) - The global profiling info.
 *   o procLevel (I) - Procedure call level to return to (zero to clear stack).
 *   o evalLevel (I) - Eval call level to return to (zero to clear stack).
 *-----------------------------------------------------------------------------
 */
static void
StackSync (infoPtr, procLevel, evalLevel)
    profInfo_t *infoPtr;
    int         procLevel;
    int         evalLevel;
{
    saveStackEntry_t *saveEntryPtr;
    
    while (TRUE) {
        /*
         * Move top of saved stack to standard stack if stack is empty or
         * saved eval level is greater than the top of the standard stack.
         */
        saveEntryPtr = infoPtr->saveStackPtr;

        if ((saveEntryPtr != NULL) && 
            ((infoPtr->stackPtr == NULL) || 
             (saveEntryPtr->topPtr->evalLevel >
              infoPtr->stackPtr->evalLevel))) {

            infoPtr->stackPtr = saveEntryPtr->topPtr;
            infoPtr->saveStackPtr = saveEntryPtr->prevEntryPtr;
            ckfree ((char *) saveEntryPtr);

        } else {

            if ((infoPtr->stackPtr == NULL) ||
                ((procLevel >= infoPtr->stackPtr->procLevel) &&
                 (evalLevel >= infoPtr->stackPtr->evalLevel)))
                break;  /* Done */
            ProcPopEntry (infoPtr);

        }
    }
}

/*
 *-----------------------------------------------------------------------------
 *
 * DoUplevel --
 *
 *   Do processing required when an uplevel is detected.  Builds and
 * pushes a save stack containing all of the save entrys that have been
 * hiden by the uplevel.  
 *
 * Parameters:
 *   o infoPtr (I/O) - The global profiling info.
 *   o procLevel (I) - The upleveled procedure call level.
 *-----------------------------------------------------------------------------
 */
static void
DoUplevel (infoPtr, procLevel)
    profInfo_t *infoPtr;
    int         procLevel;
{
    profStackEntry_t *scanPtr, *bottomPtr;
    saveStackEntry_t *saveEntryPtr;

    /*
     * Find the stack area to save.
     */
    bottomPtr = NULL;
    scanPtr = infoPtr->stackPtr;
    while ((scanPtr != NULL) && (scanPtr->procLevel > procLevel)) {
        bottomPtr = scanPtr;
        scanPtr = scanPtr->prevEntryPtr;
    }
    if (bottomPtr == NULL)
        panic ("uplevel stack confusion");

    /*
     * Save the stack entries in the save stack.
     */
    saveEntryPtr = (saveStackEntry_t *) ckalloc (sizeof (saveStackEntry_t));
    saveEntryPtr->topPtr       = infoPtr->stackPtr;
    saveEntryPtr->bottomPtr    = bottomPtr;
    saveEntryPtr->prevEntryPtr = infoPtr->saveStackPtr;;

    infoPtr->saveStackPtr = saveEntryPtr;

    /*
     * Hide the stack entries.
     */
    infoPtr->stackPtr = bottomPtr->prevEntryPtr;

}

/*
 *-----------------------------------------------------------------------------
 *
 * ProfTraceRoutine --
 *  Routine called by Tcl_Eval to do profiling.
 *
 *-----------------------------------------------------------------------------
 */
static void
ProfTraceRoutine (clientData, interp, evalLevel, command, cmdProc,
                  cmdClientData, argc, argv)
    ClientData    clientData;
    Tcl_Interp   *interp;
    int           evalLevel;
    char         *command;
    int           (*cmdProc)();
    ClientData    cmdClientData;
    int           argc;
    char        **argv;
{
    Interp      *iPtr      = (Interp *) interp;
    struct tms   cpuTimes;
    profInfo_t  *infoPtr   = (profInfo_t *) clientData;
    int          procLevel = (iPtr->varFramePtr == NULL) ? 0 : 
                             iPtr->varFramePtr->level;

    /*
     * Calculate the time spent since the last trace.
     */
    infoPtr->realTime += times (&cpuTimes) - infoPtr->lastRealTime;
    infoPtr->cpuTime  += (cpuTimes.tms_utime + cpuTimes.tms_stime) -
                         infoPtr->lastCpuTime;

    
    /*
     * If the procedure level has changed, then something is up.  Its not a
     * procedure call, as we head them off before they happen.  Its one of
     * four events.
     *
     *   o A uplevel command was executed.
     *   o Returned from an uplevel.
     *   o A procedure exit has occured.
     *   o An error unwind has occured.
     *     
     * Eval level must be tested as well as proc level to cover upleveled
     * proc calls that don't execute any commands.
     */
     
    if ((infoPtr->stackPtr != NULL) && 
        ((procLevel != infoPtr->stackPtr->procLevel) ||
         (evalLevel <  infoPtr->stackPtr->evalLevel))) {

        if ((procLevel < infoPtr->stackPtr->procLevel) &&
            (evalLevel > infoPtr->stackPtr->evalLevel))
            DoUplevel (infoPtr, procLevel);
        else
            StackSync (infoPtr, procLevel, evalLevel);
    }

    /*
     * If this is level zero and the stack is empty, add an entry for the
     * global level.  This takes care of the first command at the global level
     * after profiling has been enabled or the case where profiling was
     * enabled in a proc and we have returned to the global level.
     */
     if ((infoPtr->stackPtr == NULL) && (procLevel == 0))
         ProcEntry (infoPtr, "<global>", 0, evalLevel);

    /*
     * If this command is a procedure or if all commands are being traced,
     * handle the entry.
     */

    if (infoPtr->allCommands || (TclFindProc (iPtr, argv [0]) != NULL))
        ProcEntry (infoPtr, argv [0], procLevel + 1, evalLevel + 1);

    /*
     * Save the exit time of the profiling trace handler.
     */
    infoPtr->lastRealTime = times (&cpuTimes);
    infoPtr->lastCpuTime  = cpuTimes.tms_utime + cpuTimes.tms_stime;

}

/*
 *-----------------------------------------------------------------------------
 *
 * CleanDataTable --
 *
 *  Clean up the hash data table, releasing all resources and setting it
 *  to the empty state.
 *
 * Parameters:
 *   o infoPtr (I/O) - The global profiling info.
 *-----------------------------------------------------------------------------
 */
static void
CleanDataTable (infoPtr)
    profInfo_t *infoPtr;
{
    Tcl_HashEntry    *hashEntryPtr;
    Tcl_HashSearch   searchCookie;

    hashEntryPtr = Tcl_FirstHashEntry (&infoPtr->profDataTable,
                                       &searchCookie);
    while (hashEntryPtr != NULL) {
        ckfree ((char *) Tcl_GetHashValue (hashEntryPtr));
        Tcl_DeleteHashEntry (hashEntryPtr);
        hashEntryPtr = Tcl_NextHashEntry (&searchCookie);
    }
}

/*
 *-----------------------------------------------------------------------------
 *
 * DeleteProfTrace --
 *
 *   Delete the profile trace and clean up the stack, logging all procs
 * as if they had exited.  Data table must still be available.
 *
 * Parameters:
 *   o infoPtr (I/O) - The global profiling info.
 *-----------------------------------------------------------------------------
 */
static void
DeleteProfTrace (infoPtr)
    profInfo_t *infoPtr;
{
    Tcl_DeleteTrace (infoPtr->interp, infoPtr->traceHolder);
    infoPtr->traceHolder = NULL;

    StackSync (infoPtr, 0, 0);

}

/*
 *-----------------------------------------------------------------------------
 *
 * DumpTableData --
 *
 *   Dump the table data to an array variable.  Entries will be deleted
 * as they are dumped to limit memory utilization.
 *
 * Parameters:
 *   o interp (I) - Pointer to the interprer.
 *   o infoPtr (I/O) - The global profiling info.
 *   o varName (I) - The name of the variable to save the data in.
 * Returns:
 *   Standard Tcl command results
 *-----------------------------------------------------------------------------
 */
static int
DumpTableData (interp, infoPtr, varName)
    Tcl_Interp *interp;
    profInfo_t *infoPtr;
    char       *varName;
{
    Tcl_HashEntry    *hashEntryPtr;
    Tcl_HashSearch    searchCookie;
    profDataEntry_t  *dataEntryPtr;
    char             *dataArgv [3], *dataListPtr;
    char              countBuf [32], realTimeBuf [32], cpuTimeBuf [32];

    dataArgv [0] = countBuf;
    dataArgv [1] = realTimeBuf;
    dataArgv [2] = cpuTimeBuf;

    Tcl_UnsetVar (interp, varName, 0);
    hashEntryPtr = Tcl_FirstHashEntry (&infoPtr->profDataTable,
                                       &searchCookie);
    while (hashEntryPtr != NULL) {
        dataEntryPtr = 
            (profDataEntry_t *) Tcl_GetHashValue (hashEntryPtr);

        sprintf (countBuf,    "%ld", dataEntryPtr->count);
        sprintf (realTimeBuf, "%ld", dataEntryPtr->realTime * MS_PER_TICK);
        sprintf (cpuTimeBuf,  "%ld", dataEntryPtr->cpuTime  * MS_PER_TICK);

        dataListPtr = Tcl_Merge (3, dataArgv);

        if (Tcl_SetVar2 (interp, varName,
                         Tcl_GetHashKey (&infoPtr->profDataTable,
                                         hashEntryPtr),
                         dataListPtr, TCL_LEAVE_ERR_MSG) == NULL) {
            ckfree (dataListPtr);
            return TCL_ERROR;
        }
        ckfree (dataListPtr);
        ckfree ((char *) dataEntryPtr);
        Tcl_DeleteHashEntry (hashEntryPtr);

        hashEntryPtr = Tcl_NextHashEntry (&searchCookie);
    }

    return TCL_OK;
}

/*
 *-----------------------------------------------------------------------------
 *
 * Tcl_ProfileCmd --
 *     Implements the TCL profile command:
 *     profile on
 *     profile off arrayvar
 *
 * Results:
 *  Standard TCL results.
 *
 *-----------------------------------------------------------------------------
 */
static int
Tcl_ProfileCmd (clientData, interp, argc, argv)
    ClientData    clientData;
    Tcl_Interp   *interp;
    int           argc;
    char        **argv;
{
    Interp      *iPtr = (Interp *) interp;
    profInfo_t  *infoPtr = (profInfo_t *) clientData;
    int          idx;
    int          cmdArgc,   optionsArgc = 0;
    char       **cmdArgv, **optionsArgv = &(argv [1]);

    /*
     * Scan for options (currently only one is supported).  Set cmdArgv to
     * contain the rest of the command following the options.
     */
    for (idx = 1; (idx < argc) && (argv [idx][0] == '-'); idx++)
        optionsArgc++;
    cmdArgc = argc - idx;
    cmdArgv = &(argv [idx]);

    if (cmdArgc < 1)
        goto wrongArgs;

    /*
     * Handle the on command.
     */
    if (STREQU (cmdArgv [0], "on")) {
        int        allCommands = FALSE;
        struct tms cpuTimes;

        if ((cmdArgc != 1) || (optionsArgc > 1))
            goto wrongArgs;

        if (optionsArgc == 1) {
            if (!STREQU (optionsArgv [0], "-commands")) {
                Tcl_AppendResult (interp, "expected option of \"-commands\", ",
                                  "got \"", optionsArgv [0], "\"",
                                  (char *) NULL);
                return TCL_ERROR;
            }
            allCommands = TRUE;
        }

        if (infoPtr->traceHolder != NULL) {
            Tcl_AppendResult (interp, "profiling is already enabled",
                              (char *) NULL);
            return TCL_ERROR;
        }
            
        CleanDataTable (infoPtr);
        infoPtr->traceHolder = Tcl_CreateTrace (interp, MAXINT,
                                                ProfTraceRoutine,
                                                (ClientData) infoPtr);
        infoPtr->realTime = 0;
        infoPtr->cpuTime  = 0;
        infoPtr->lastRealTime = times (&cpuTimes);
        infoPtr->lastCpuTime  = cpuTimes.tms_utime + cpuTimes.tms_stime;
        infoPtr->allCommands = allCommands;
        return TCL_OK;
    }

    /*
     * Handle the off command.  Dump the hash table to a variable.
     */
    if (STREQU (cmdArgv [0], "off")) {

        if ((cmdArgc != 2) || (optionsArgc > 0))
            goto wrongArgs;

        if (infoPtr->traceHolder == NULL) {
            Tcl_AppendResult (interp, "profiling is not currently enabled",
                              (char *) NULL);
            return TCL_ERROR;
        }
            
        DeleteProfTrace (infoPtr);

        if (DumpTableData (interp, infoPtr, argv [2]) != TCL_OK)
            return TCL_ERROR;
        return TCL_OK;
    }

    /*
     * Not a valid subcommand.
     */
    Tcl_AppendResult (interp, "expected one of \"on\" or \"off\", got \"",
                      argv [1], "\"", (char *) NULL);
    return TCL_ERROR;

  wrongArgs:
    Tcl_AppendResult (interp, tclXWrongArgs, argv [0],
                      " [-commands] on|off arrayVar", (char *) NULL);
    return TCL_ERROR;
}

/*
 *-----------------------------------------------------------------------------
 *
 *  CleanUpProfMon --
 *
 *  Release the client data area when the profile command is deleted.
 *
 *-----------------------------------------------------------------------------
 */
static void
CleanUpProfMon (clientData)
    ClientData clientData;
{
    profInfo_t *infoPtr = (profInfo_t *) clientData;

    if (infoPtr->traceHolder != NULL)
        DeleteProfTrace (infoPtr);
    CleanDataTable (infoPtr);
    Tcl_DeleteHashTable (&infoPtr->profDataTable);
    ckfree ((char *) infoPtr);
}

/*
 *-----------------------------------------------------------------------------
 *
 *  Tcl_InitProfile --
 *
 *  Initialize the Tcl profiling command.
 *
 *-----------------------------------------------------------------------------
 */
void
Tcl_InitProfile (interp)
    Tcl_Interp *interp;
{
    profInfo_t *infoPtr;

    infoPtr = (profInfo_t *) ckalloc (sizeof (profInfo_t));

    infoPtr->interp       = interp;
    infoPtr->traceHolder  = NULL;
    infoPtr->stackPtr     = NULL;
    infoPtr->saveStackPtr = NULL;
    Tcl_InitHashTable (&infoPtr->profDataTable, TCL_STRING_KEYS);

    Tcl_CreateCommand (interp, "profile", Tcl_ProfileCmd, 
                       (ClientData)infoPtr, CleanUpProfMon);
}

