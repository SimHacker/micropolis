/*
 * tclXfilescan.c --
 *
 * Tcl file scanning: regular expression matching on lines of a file.  
 * Implements awk.
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
 * $Id: tclXfilescan.c,v 2.0 1992/10/16 04:50:43 markd Rel $
 *-----------------------------------------------------------------------------
 */

#include "tclxint.h"
#include "regexp.h"

/*
 * A scan context describes a collection of match patterns and commands,
 * along with a match default command to apply to a file on a scan.
 */
 
#define CONTEXT_A_CASE_INSENSITIVE_FLAG 2
#define MATCH_CASE_INSENSITIVE_FLAG 4

typedef struct matchDef_t {
    regexp_t            regExpInfo;
    char               *command;
    struct matchDef_t  *nextMatchDefPtr;
    short               matchflags;
    } matchDef_t;
typedef struct matchDef_t *matchDef_pt;

typedef struct scanContext_t {
    matchDef_pt  matchListHead;
    matchDef_pt  matchListTail;
    char        *defaultAction;
    short        flags;
    } scanContext_t;
typedef struct scanContext_t *scanContext_pt;

/*
 * Global data structure, pointer to by clientData.
 */

typedef struct {
    int             useCount;      /* Commands that current share globals */
    void_pt         tblHdrPtr;     /* Scan context handle table           */
    char            curName [16];  /* Current context name.               */ 
    } scanGlob_t;
typedef scanGlob_t *scanGlob_pt;

/*
 * Prototypes of internal functions.
 */
static int
CleanUpContext _ANSI_ARGS_((scanGlob_pt    scanGlobPtr,
                            scanContext_pt contextPtr));

static int
CreateScanContext _ANSI_ARGS_((Tcl_Interp  *interp,
                               scanGlob_pt  scanGlobPtr));

static int
SelectScanContext _ANSI_ARGS_((Tcl_Interp  *interp,
                               scanGlob_pt  scanGlobPtr,
                               char        *contextHandle));

static int
Tcl_Delete_scancontextCmd _ANSI_ARGS_((Tcl_Interp  *interp,
                                       scanGlob_pt  scanGlobPtr,
                                       char        *contextHandle));

static int
SetMatchVar _ANSI_ARGS_((Tcl_Interp *interp,
                         char       *fileLine,
                         long        fileOffset,
                         long        scanLineNum,
                         char       *fileHandle));

static void
FileScanCleanUp _ANSI_ARGS_((ClientData clientData));


/*
 *-----------------------------------------------------------------------------
 *
 * CleanUpContext
 *     Release all resources allocated to the specified scan context
 *     entry.  The entry itself is not released.
 *-----------------------------------------------------------------------------
 */
static int
CleanUpContext (scanGlobPtr, contextPtr)
    scanGlob_pt    scanGlobPtr;
    scanContext_pt contextPtr;
{
    matchDef_pt  matchPtr, oldMatchPtr;

    for (matchPtr = contextPtr->matchListHead; matchPtr != NULL;) {
        Tcl_RegExpClean (&matchPtr->regExpInfo);
        if (matchPtr->command != NULL)
            ckfree(matchPtr->command);
        oldMatchPtr = matchPtr;
        matchPtr = matchPtr->nextMatchDefPtr;
        ckfree ((char *) oldMatchPtr);
        }
    contextPtr->matchListHead = NULL;
    contextPtr->matchListTail = NULL;

    if (contextPtr->defaultAction != NULL) {
        ckfree(contextPtr->defaultAction);
        contextPtr->defaultAction = NULL;
    }
}

/*
 *-----------------------------------------------------------------------------
 *
 * CreateScanContext --
 *     Create a new scan context, implements the subcommand:
 *         scancontext create
 *
 *-----------------------------------------------------------------------------
 */
static int
CreateScanContext (interp, scanGlobPtr)
    Tcl_Interp  *interp;
    scanGlob_pt  scanGlobPtr;
{
    scanContext_pt contextPtr;

    contextPtr = (scanContext_pt)Tcl_HandleAlloc (scanGlobPtr->tblHdrPtr, 
                                                  scanGlobPtr->curName);
    contextPtr->flags = 0;
    contextPtr->matchListHead = NULL;
    contextPtr->matchListTail = NULL;
    contextPtr->defaultAction = NULL;

    Tcl_SetResult (interp, scanGlobPtr->curName, TCL_STATIC);
    return TCL_OK;
}

/*
 *-----------------------------------------------------------------------------
 *
 * DeleteScanContext --
 *     Deletes the specified scan context, implements the subcommand:
 *         scancontext delete contexthandle
 *
 *-----------------------------------------------------------------------------
 */
static int
DeleteScanContext (interp, scanGlobPtr, contextHandle)
    Tcl_Interp  *interp;
    scanGlob_pt  scanGlobPtr;
    char        *contextHandle;
{
    scanContext_pt contextPtr;

    if ((contextPtr = Tcl_HandleXlate (interp, scanGlobPtr->tblHdrPtr, 
                                       contextHandle)) == NULL)
        return TCL_ERROR;

    CleanUpContext (scanGlobPtr, contextPtr);
    Tcl_HandleFree (scanGlobPtr->tblHdrPtr, contextPtr);

    return TCL_OK;
}

/*
 *-----------------------------------------------------------------------------
 *
 * Tcl_ScancontextCmd --
 *     Implements the TCL scancontext Tcl command, which has the 
 *     following forms.
 *         scancontext create
 *         scancontext delete
 *
 * Results:
 *    Standard TCL results.
 *
 *-----------------------------------------------------------------------------
 */
static int
Tcl_ScancontextCmd (clientData, interp, argc, argv)
    char       *clientData;
    Tcl_Interp *interp;
    int         argc;
    char      **argv;
{
    scanGlob_pt  scanGlobPtr = (scanGlob_pt) clientData;

    if (argc < 2) {
        Tcl_AppendResult (interp, tclXWrongArgs, argv [0], " option",
                          (char *) NULL);
        return TCL_ERROR;
    }
    /*
     * Create a new scan context.
     */
    if (STREQU (argv [1], "create")) {
        if (argc != 2) {
            Tcl_AppendResult (interp, tclXWrongArgs, argv [0], " create",
                              (char *) NULL);
            return TCL_ERROR;
        }
        return CreateScanContext (interp, scanGlobPtr);        
    }
    
    /*
     * Delete a scan context.
     */
    if (STREQU (argv [1], "delete")) {
        if (argc != 3) {
            Tcl_AppendResult (interp, tclXWrongArgs, argv [0],
                              "delete contexthandle", (char *) NULL);
            return TCL_ERROR;
        }
        return DeleteScanContext (interp, scanGlobPtr, argv [2]);
    }
    
    Tcl_AppendResult (interp, "invalid argument, expected one of: ",
                      "create or delete", (char *) NULL);
    return TCL_ERROR;
}

/*
 *-----------------------------------------------------------------------------
 *
 * Tcl_ScanmatchCmd --
 *     Implements the TCL command:
 *         scanmatch [-nocase] contexthandle [regexp] commands
 *     This uses both Boyer_Moore and regular expressions matching.
 *
 * Results:
 *    Standard TCL results.
 *
 *-----------------------------------------------------------------------------
 */
static int
Tcl_ScanmatchCmd (clientData, interp, argc, argv)
    char       *clientData;
    Tcl_Interp *interp;
    int         argc;
    char      **argv;
{
    scanGlob_pt     scanGlobPtr = (scanGlob_pt) clientData;
    scanContext_pt  contextPtr;
    char           *result;
    matchDef_pt     newmatch;
    int             compFlags = REXP_BOTH_ALGORITHMS;
    int             firstArg = 1;

    if (argc < 3)
        goto argError;
    if (STREQU (argv[1], "-nocase")) {
        compFlags |= REXP_NO_CASE;
        firstArg = 2;
    }
      
    /*
     * If firstArg == 2 (-nocase), the both a regular expression and a command
     * string must be specified, otherwise the regular expression is optional.
     */
    if (((firstArg == 2) && (argc != 5)) || ((firstArg == 1) && (argc > 4)))
        goto argError;

    if ((contextPtr = Tcl_HandleXlate (interp, scanGlobPtr->tblHdrPtr, 
                                       argv [firstArg])) == NULL)
        return TCL_ERROR;

    /*
     * Handle the default case (no regular expression).
     */
    if (argc == 3) {
        if (contextPtr->defaultAction) {
            Tcl_AppendResult (interp, argv [0], ": default match already ",
                              "specified in this scan context", (char *) NULL);
            return TCL_ERROR;
        }
        contextPtr->defaultAction = ckalloc (strlen (argv [2]) + 1);
        strcpy (contextPtr->defaultAction, argv [2]);

        return TCL_OK;
    }

    /*
     * Add a regular expression to the context.
     */

    newmatch = (matchDef_pt) ckalloc(sizeof (matchDef_t));
    newmatch->matchflags = 0;

    if (compFlags & REXP_NO_CASE) {
        newmatch->matchflags |= MATCH_CASE_INSENSITIVE_FLAG;
        contextPtr->flags |= CONTEXT_A_CASE_INSENSITIVE_FLAG;
    }

    if (Tcl_RegExpCompile (interp, &newmatch->regExpInfo, argv [firstArg + 1], 
                           compFlags) != TCL_OK) {
        ckfree ((char *) newmatch);
        return (TCL_ERROR);
    }

    newmatch->command = ckalloc (strlen (argv[firstArg + 2]) + 1);
    strcpy(newmatch->command, argv [firstArg + 2]);

    /*
     * Link in the new match.
     */
    newmatch->nextMatchDefPtr = NULL;
    if (contextPtr->matchListHead == NULL)
        contextPtr->matchListHead = newmatch;
    else
        contextPtr->matchListTail->nextMatchDefPtr = newmatch;
    contextPtr->matchListTail = newmatch;

    return TCL_OK;

argError:
    Tcl_AppendResult (interp, tclXWrongArgs, argv [0],
                      " [-nocase] contexthandle [regexp] command",
                      (char *) NULL);
    return TCL_ERROR;
}

/*
 *-----------------------------------------------------------------------------
 *
 * SetMatchVar --
 *     Sets the TCL array variable matchInfo to contain information 
 *     about the line that is matched.
 * Results:
 *     TCL_OK if all is ok, TCL_ERROR if an error occures setting the
 *     variables.
 * Side effects:
 *     A TCL array variable is created or altered.
 * 
 *-----------------------------------------------------------------------------
 */
static int
SetMatchVar (interp, fileLine, fileOffset, scanLineNum, fileHandle)
    Tcl_Interp *interp;
    char       *fileLine;
    long        fileOffset;
    long        scanLineNum;
    char       *fileHandle;
{
    char numBuf [20];

    if (Tcl_SetVar2 (interp, "matchInfo", "line", fileLine, 
                     TCL_LEAVE_ERR_MSG) == NULL)
        return TCL_ERROR;

    sprintf (numBuf, "%ld", fileOffset);
    if (Tcl_SetVar2 (interp, "matchInfo", "offset", numBuf,
                     TCL_LEAVE_ERR_MSG) == NULL)
        return TCL_ERROR;

    sprintf (numBuf, "%ld", scanLineNum);
    if (Tcl_SetVar2 (interp, "matchInfo", "linenum", numBuf,
                     TCL_LEAVE_ERR_MSG) == NULL)
        return TCL_ERROR;

    if (Tcl_SetVar2 (interp, "matchInfo", "handle", fileHandle, 
                     TCL_LEAVE_ERR_MSG) == NULL)
        return TCL_ERROR;
    return TCL_OK;
}

/*
 *-----------------------------------------------------------------------------
 *
 * Tcl_ScanfileCmd --
 *     Implements the TCL command:
 *         scanfile contexthandle filehandle
 *
 * Results:
 *    Standard TCL results.
 *
 *-----------------------------------------------------------------------------
 */
static int
Tcl_ScanfileCmd (clientData, interp, argc, argv)
    char       *clientData;
    Tcl_Interp *interp;
    int         argc;
    char      **argv;
{
    scanGlob_pt     scanGlobPtr = (scanGlob_pt) clientData;
    scanContext_pt  contextPtr;
    dynamicBuf_t    dynBuf, lowerDynBuf;
    OpenFile       *filePtr;
    matchDef_pt     matchPtr;
    int             result;
    int             matchedAtLeastOne;
    long            fileOffset;
    long            matchOffset;
    long            scanLineNum = 0;
    char           *fileHandle;

    if ((argc < 2) || (argc > 3)) {
        Tcl_AppendResult (interp, tclXWrongArgs, argv [0], 
                          " contexthandle filehandle", (char *) NULL);
        return TCL_ERROR;
    }
    if ((contextPtr = Tcl_HandleXlate (interp, scanGlobPtr->tblHdrPtr, 
                                       argv [1])) == NULL)
        return TCL_ERROR;

    if (TclGetOpenFile (interp, argv [2], &filePtr) != TCL_OK)
            return TCL_ERROR;

    if (contextPtr->matchListHead == NULL) {
        Tcl_AppendResult (interp, "no patterns in current scan context",
                          (char *) NULL);
        return TCL_ERROR;
    }

    Tcl_DynBufInit (&dynBuf);
    Tcl_DynBufInit (&lowerDynBuf);

    result = TCL_OK;  /* Assume the best */

    fileOffset = ftell (filePtr->f);  /* Get starting offset */

    while ((result == TCL_OK)) {
        int storedThisLine = FALSE;

        switch (Tcl_DynamicFgets (&dynBuf, filePtr->f, FALSE)) {
          case -1:  /* Error */
            interp->result = Tcl_UnixError (interp);
            goto scanExit;
            
          case 0:  /* EOF */
            goto scanExit;
        }
        scanLineNum++;
        matchOffset = fileOffset;
        fileOffset += strlen(dynBuf.ptr) + 1;
        storedThisLine = 0;
        matchedAtLeastOne = 0;
        if (contextPtr->flags & CONTEXT_A_CASE_INSENSITIVE_FLAG) {
            lowerDynBuf.len = 0;
            Tcl_DynBufAppend (&lowerDynBuf, dynBuf.ptr);
            Tcl_DownShift (lowerDynBuf.ptr, lowerDynBuf.ptr);
        }
        for (matchPtr = contextPtr->matchListHead; matchPtr != NULL; 
                 matchPtr = matchPtr->nextMatchDefPtr) {

            if (!Tcl_RegExpExecute (interp, &matchPtr->regExpInfo, dynBuf.ptr, 
                                    lowerDynBuf.ptr))
                continue;  /* Try next match pattern */

            matchedAtLeastOne = TRUE;
            if (!storedThisLine) {
                result = SetMatchVar (interp, dynBuf.ptr, matchOffset, 
                                      scanLineNum, argv[2]);
                if (result != TCL_OK)
                    goto scanExit;
                storedThisLine = TRUE;
            }

            result = Tcl_Eval(interp, matchPtr->command, 0, (char **)NULL);
            if (result == TCL_ERROR) {
                Tcl_AddErrorInfo (interp, 
                    "\n    while executing a match command");
                goto scanExit;
            }
            if (result == TCL_CONTINUE) {
                /* 
                 * Don't process any more matches for this line.
                 */
                result = TCL_OK;
                goto matchLineExit;
            }
            if (result == TCL_BREAK) {
                /*
                 * Terminate scan.
                 */
                result = TCL_OK;
                goto scanExit;
            }
        }

        matchLineExit:
        /*
         * Process default action if required.
         */
        if ((contextPtr->defaultAction != NULL) && (!matchedAtLeastOne)) {

            result = SetMatchVar (interp, dynBuf.ptr, matchOffset, 
                                  scanLineNum, argv[2]);
            if (result != TCL_OK)
                goto scanExit;

            result = Tcl_Eval (interp, contextPtr->defaultAction, 0, 
                               (char **)NULL);
            if (result == TCL_CONTINUE)
                result = TCL_OK;    /* This doesn't mean anything, but  */
                                    /* don't break the user.            */
            if (result == TCL_ERROR)
                Tcl_AddErrorInfo (interp, 
                    "\n    while executing a match default command");
        }
    }
scanExit:
    Tcl_DynBufFree (&dynBuf);
    Tcl_DynBufFree (&lowerDynBuf);
    if (result == TCL_RETURN)
        result = TCL_OK;
    return result;
}

/*
 *-----------------------------------------------------------------------------
 *
 *  FileScanCleanUp --
 *      Decrements the use count on the globals when a command is deleted.
 *      If it goes to zero, all resources are released.      
 *
 *-----------------------------------------------------------------------------
 */
static void
FileScanCleanUp (clientData)
    ClientData clientData;
{
    scanGlob_pt    scanGlobPtr = (scanGlob_pt) clientData;
    scanContext_pt contextPtr;
    int            walkKey;
    
    scanGlobPtr->useCount--;
    if (scanGlobPtr->useCount > 0)
        return;

    walkKey = -1;
    while ((contextPtr = Tcl_HandleWalk (scanGlobPtr->tblHdrPtr, 
            &walkKey)) != NULL)
        CleanUpContext (scanGlobPtr, contextPtr);

    Tcl_HandleTblRelease (scanGlobPtr->tblHdrPtr);
    ckfree ((char *) scanGlobPtr);
}

/*
 *-----------------------------------------------------------------------------
 *
 *  Tcl_InitFilescan --
 *      Initialize the TCL file scanning facility..
 *
 *-----------------------------------------------------------------------------
 */
void
Tcl_InitFilescan (interp)
Tcl_Interp *interp;
{
    scanGlob_pt    scanGlobPtr;
    void_pt        fileCbTblPtr;

    scanGlobPtr = (scanGlob_pt) ckalloc (sizeof (scanGlob_t));
    scanGlobPtr->tblHdrPtr = 
        Tcl_HandleTblInit ("context", sizeof (scanContext_t), 5);

    /*
     * Initialize the commands.
     */
    scanGlobPtr->useCount = 3;  /* Number of commands */

    Tcl_CreateCommand (interp, "scanfile", Tcl_ScanfileCmd, 
                       (ClientData)scanGlobPtr, FileScanCleanUp);
    Tcl_CreateCommand (interp, "scanmatch", Tcl_ScanmatchCmd, 
                       (ClientData)scanGlobPtr, FileScanCleanUp);
    Tcl_CreateCommand (interp, "scancontext", Tcl_ScancontextCmd,
                       (ClientData)scanGlobPtr, FileScanCleanUp);
}

