/*
 * tclXsignal.c --
 *
 * Tcl Unix signal support routines and the signal and commands.
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
 * $Id: tclXsignal.c,v 2.0 1992/10/16 04:51:12 markd Rel $
 *-----------------------------------------------------------------------------
 */

#include "tclxint.h"


#ifndef SIGCLD
#   define SIGCLD SIGCHLD
#endif
#ifndef SIGCHLD
#   define SIGCHLD SIGCLD
#endif

#ifndef MAXSIG
#    define MAXSIG 32
#endif

/*
 * Signal name table maps name to number.
 */

#define SIG_NAME_MAX 7

static struct {char *name;
        short num;
       } sigNameTable [] = {
    "HUP",     SIGHUP,
    "INT",     SIGINT,
    "QUIT",    SIGQUIT,
    "ILL",     SIGILL,
    "TRAP",    SIGTRAP,
    "IOT",     SIGIOT,
#ifdef SIGABRT
    "ABRT",    SIGABRT,
#endif
#ifdef SIGEMT
    "EMT",     SIGEMT,
#endif
#ifdef SIGSYS
    "SYS",     SIGSYS,
#endif
    "FPE",     SIGFPE,
    "KILL",    SIGKILL,
    "BUS",     SIGBUS,
    "SEGV",    SIGSEGV,
    "PIPE",    SIGPIPE,
    "ALRM",    SIGALRM,
    "TERM",    SIGTERM,
    "USR1",    SIGUSR1,
    "USR2",    SIGUSR2,
    "CLD",     SIGCLD,
    "CHLD",    SIGCHLD,
#ifdef SIGPWR
    "PWR",     SIGPWR,
#endif
#ifdef SIGPOLL
    "POLL",    SIGPOLL,
#endif
#ifdef SIGSTOP
    "STOP",    SIGSTOP,
#endif
#ifdef SIGTSTP
    "TSTP",    SIGTSTP,
#endif
#ifdef SIGCONT
    "CONT",    SIGCONT,
#endif
#ifdef SIGTTIN
    "TTIN",    SIGTTIN,
#endif
#ifdef SIGTTOU
    "TTOU",    SIGTTOU,
#endif
    NULL,         -1};

#ifdef TCL_SIG_PROC_INT
#   define SIG_PROC_RET_TYPE int
#else
#   define SIG_PROC_RET_TYPE void
#endif

typedef SIG_PROC_RET_TYPE (*signalProcPtr_t) _ANSI_ARGS_((int));

/*
 * Class of actions that can be set by the signal command.
 */
#define SIGACT_SET     1   /* Set the signal     */
#define SIGACT_GET     2   /* Get the signal     */
#define SIGACT_BLOCK   3   /* Block the signal   */
#define SIGACT_UNBLOCK 4   /* Unblock the signal */

/*
 * Defines if this is not Posix.
 */
#ifndef SIG_BLOCK
#   define SIG_BLOCK       1
#   define SIG_UNBLOCK     2
#endif

/*
 * Messages.
 */
static char *noPosix = "Posix signals are not available on this system";

/*
 * Globals that indicate that some signal was received and how many of each
 * signal type has not yet been processed.
 */
int             tclReceivedSignal = FALSE;    /* A signal was received */ 
static unsigned signalsReceived [MAXSIG];     /* Counters of signals   */

/*
 * Table of commands to evaluate when a signal occurs.  If the command is
 * NULL and the signal is received, an error is returned.
 */
static char *signalTrapCmds [MAXSIG];

/*
 * Prototypes of internal functions.
 */

static int
SigNameToNum _ANSI_ARGS_((char *sigName));

static signalProcPtr_t
GetSignalState _ANSI_ARGS_((int signalNum));

static int
SetSignalAction _ANSI_ARGS_((int             signalNum,
                             signalProcPtr_t sigFunc));

static SIG_PROC_RET_TYPE
TclSignalTrap _ANSI_ARGS_((int signalNum));

static int
EvalTrapCode _ANSI_ARGS_((Tcl_Interp *interp,
                          int         signalNum,
                          char       *command));

static int
ParseSignalList _ANSI_ARGS_((Tcl_Interp *interp,
                             char       *signalListStr,
                             int         signalList []));

static char *
SignalBlocked _ANSI_ARGS_((Tcl_Interp  *interp,
                           int          signalNum));

static int
GetSignalStates  _ANSI_ARGS_((Tcl_Interp *interp,
                              int         signalListSize,
                              int         signalList [MAXSIG]));

static int
SetSignalStates  _ANSI_ARGS_((Tcl_Interp      *interp,
                              int              signalListSize,
                              int              signalList [MAXSIG],
                              signalProcPtr_t  actionFunc,
                              char            *command));

static int
BlockSignals _ANSI_ARGS_((Tcl_Interp  *interp,
                          int          action,
                          int          signalListSize,
                          int          signalList [MAXSIG]));

static void
SignalCmdCleanUp _ANSI_ARGS_((ClientData clientData));


/*
 *-----------------------------------------------------------------------------
 *
 * SigNameToNum --
 *     Converts a UNIX signal name to its number, returns -1 if not found.
 *     the name may be upper or lower case and may optionally have the 
 *     leading "SIG" omitted.
 *
 *-----------------------------------------------------------------------------
 */
static int
SigNameToNum (sigName)
    char *sigName;
{
    char  sigNameUp [SIG_NAME_MAX+1];  /* Upshifted signal name */
    char *sigNamePtr; 
    int   idx;

    /*
     * Copy and upshift requested name.
     */

    if (strlen (sigName) > SIG_NAME_MAX)
        return -1;   /* Name too long */

    Tcl_UpShift (sigNameUp, sigName);

    if (STRNEQU (sigNameUp, "SIG", 3))
        sigNamePtr = &sigNameUp [3];
    else
        sigNamePtr = sigNameUp;

    for (idx = 0; sigNameTable [idx].num != -1; idx++)
        if (STREQU (sigNamePtr, sigNameTable [idx].name))
            break;

    return sigNameTable [idx].num;
}

/*
 *-----------------------------------------------------------------------------
 *
 * Tcl_KillCmd --
 *     Implements the TCL kill command:
 *        kill [signal] proclist
 *
 * Results:
 *  Standard TCL results, may return the UNIX system error message.
 *
 *-----------------------------------------------------------------------------
 */
int
Tcl_KillCmd (clientData, interp, argc, argv)
    ClientData  clientData;
    Tcl_Interp *interp;
    int     argc;
    char      **argv;
{
    int    signalNum, idx, procId, procArgc, result = TCL_ERROR;
    char **procArgv;

    if ((argc < 2) || (argc > 3)) {
        Tcl_AppendResult (interp, tclXWrongArgs, argv [0], 
                          " [signal] processlist", (char *) NULL);
        return TCL_ERROR;
    }

    if (argc == 2)
        signalNum = SIGTERM;
    else {
        if (!Tcl_StrToInt (argv[1], 0, &signalNum)) {
            signalNum = SigNameToNum (argv[1]);
        }
        if ((signalNum < 0) || (signalNum > NSIG)) {
            Tcl_AppendResult (interp, "invalid signal", (char *) NULL);
            return TCL_ERROR;
        }
    }

    if (Tcl_SplitList (interp, argv [argc - 1], &procArgc, 
                       &procArgv) != TCL_OK)
        return TCL_ERROR;

    for (idx = 0; idx < procArgc; idx++) {

        if (Tcl_GetInt (interp, procArgv [idx], &procId) != TCL_OK)
            goto exitPoint;

        if (kill ((pid_t) procId, signalNum) < 0) {
            Tcl_AppendResult (interp, "pid ", procArgv [idx],
                              ": ", Tcl_UnixError (interp), (char *) NULL);
            goto exitPoint;
        }
     }

    result = TCL_OK;
exitPoint:
    ckfree ((char *) procArgv);
    return result;
}

/*
 *-----------------------------------------------------------------------------
 *
 * GetSignalState --
 *     Get the current state of the specified signal.
 * Parameters:
 *   o signalNum (I) - Signal number to query.
 * Results
 *   The signal function or SIG_DFL or SIG_IGN.  If an error occures,
 *   SIG_ERR is returned (check errno);
 *-----------------------------------------------------------------------------
 */
static signalProcPtr_t
GetSignalState (signalNum)
    int signalNum;
{
#ifdef TCL_POSIX_SIG
    struct sigaction currentState;

    if (sigaction (signalNum, NULL, &currentState) < 0)
        return SIG_ERR;
    return currentState.sa_handler;
#else
    signalProcPtr_t  actionFunc;

    if (signalNum == SIGKILL)
        return SIG_DFL;

    actionFunc = signal (signalNum, SIG_DFL);
    if (actionFunc == SIG_ERR)
        return SIG_ERR;
    if (actionFunc != SIG_DFL)
        signal (signalNum, actionFunc);  /* reset */
    return actionFunc;
#endif
}

/*
 *-----------------------------------------------------------------------------
 *
 * SetSignalAction --
 *     Set the action to occur when a signal is received.
 * Parameters:
 *   o signalNum (I) - Signal number to query.
 *   o sigFunc (O) - The signal function or SIG_DFL or SIG_IGN.
 * Results
 *   TRUE if ok,  FALSE if an error (check errno).
 *-----------------------------------------------------------------------------
 */
static int
SetSignalAction (signalNum, sigFunc)
    int             signalNum;
    signalProcPtr_t sigFunc;
{
#ifdef TCL_POSIX_SIG
    struct sigaction newState;
    
    newState.sa_handler = sigFunc;
    sigfillset (&newState.sa_mask);
    newState.sa_flags = 0;

    if (sigaction (signalNum, &newState, NULL) < 0)
        return FALSE;

    return TRUE;
#else
    if (signal (signalNum, sigFunc) == SIG_ERR)
        return FALSE;
    else
        return TRUE;
#endif
}

/*
 *-----------------------------------------------------------------------------
 *
 * TclSignalTrap --
 *     Trap handler for UNIX signals.  Sets a flag indicating that the
 *     trap has occured, saves the name and rearms the trap.  The flag
 *     will be seen by the interpreter when its safe to trap.
 * Globals:
 *   o tclReceivedSignal (O) - Set to TRUE, to indicate a signal was received.
 *   o signalsReceived (O) - The count of each signal that was received.
 *-----------------------------------------------------------------------------
 */
static SIG_PROC_RET_TYPE
TclSignalTrap (signalNum)
    int signalNum;
{
    /*
     * Set flags that are checked by the eval loop.
     */
    signalsReceived [signalNum]++;
    tclReceivedSignal = TRUE;

#ifndef TCL_POSIX_SIG
    /*
     * For old-style Unix signals, the signal must be explictly re-enabled.
     * Not done for SIGCHLD, as we would continue to the signal until the
     * wait is done.  This is fixed by Posix signals and is not necessary under
     * BSD, but it done this way for consistency.
     */
    if (signalNum != SIGCHLD) {
        if (SetSignalAction (signalNum, TclSignalTrap) < 0)
            panic ("TclSignalTrap bug");
    }
#endif
}

/*
 *-----------------------------------------------------------------------------
 *
 * EvalTrapCode --
 *     Run code as the result of a signal.  The code will be run in the
 *     global context, with the symbolic signal name in a global variable.
 *     signalReceived.  If an error occured, then the result will be
 *     left in the interp, if no error occured, the result will be reset.
 * Parameters:
 *   o interp (I/O) - The interpreter to run the signal in.
 *   o signalNum (I) - The signal number of the signal that occured.
 *   o command (I) - The command string to execute.
 * Return:
 *   TCL_OK or TCL_ERROR.
 *-----------------------------------------------------------------------------
 */
static int
EvalTrapCode (interp, signalNum, command)
    Tcl_Interp *interp;
    int         signalNum;
    char       *command;
{
    Interp        *iPtr = (Interp *) interp;
    char          *signalName;
    int            result;
    CallFrame     *savedVarFramePtr;

    Tcl_ResetResult (interp);

    /*
     * Modify the interpreter state to execute in the global frame.
     */
    savedVarFramePtr = iPtr->varFramePtr;
    iPtr->varFramePtr = NULL;

    /*
     * Force name to always be SIGCHLD, even if system defines only SIGCLD.
     */
    if (signalNum == SIGCHLD)
        signalName = "SIGCHLD";
    else
        signalName = Tcl_SignalId (signalNum);

    if (Tcl_SetVar (interp, "signalReceived", signalName,
                    TCL_GLOBAL_ONLY | TCL_LEAVE_ERR_MSG) == NULL)
        result = TCL_ERROR;
    else
        result = TCL_OK;
    if (result == TCL_OK);
        result = Tcl_Eval (interp, signalTrapCmds [signalNum], 0, NULL);

    /*
     * Restore the frame pointer and return the result (only OK or ERROR).
     */
    iPtr->varFramePtr = savedVarFramePtr;

    if (result == TCL_ERROR) {
        char errorInfo [TCL_RESULT_SIZE];

        sprintf (errorInfo, "\n    while executing signal trap code for %s%s",
                 signalName, " signal");
        Tcl_AddErrorInfo (interp, errorInfo);

        return TCL_ERROR;
    } else {
        Tcl_ResetResult (interp);
        return TCL_OK;
    }
}

/*
 *-----------------------------------------------------------------------------
 *
 * Tcl_ResetSignals --
 *  
 *   Reset all of the signal flags to indicate that no signals have 
 * occured.  This is used by the shell at the beginning of each interactive
 * command
 *
 * Globals:
 *   o tclReceivedSignal (O) - Will be cleared.
 *   o signalsReceived (O) - The count of each signal that was received.
 *-----------------------------------------------------------------------------
 */
void
Tcl_ResetSignals ()
{
    int  signalNum;

    tclReceivedSignal = 0;
    for (signalNum = 0; signalNum < MAXSIG; signalNum++) 
        signalsReceived [signalNum] = 0;

}

/*
 *-----------------------------------------------------------------------------
 *
 * Tcl_CheckForSignal --
 *  
 *   Called by Tcl_Eval to check if a signal was received when Tcl_Eval is in
 * a safe state.  If the signal was received, this handles processing the
 * signal prehaps recursively eval-ing some code.  This is called just after a
 * command completes.  The results of the command are passed to this procedure
 * and may be altered by it.  If trap code is specified for the signal that
 * was received, then the trap will be executed, otherwise an error result
 * will be returned indicating that the signal occured.  If an error is
 * returned, clear the errorInfo variable.  This makes sure it exists and
 * that it is empty, otherwise bogus or non-existant information will be
 * returned if this routine was called somewhere besides Tcl_Eval.  If a
 * signal was received multiple times and a trap is set on it, then that
 * trap will be executed for each time the signal was received.
 * 
 * Parameters:
 *   o interp (I/O) - interp->result should contain the result for
 *     the command that just executed.  This will either be restored or
 *     replaced with a new result.
 *   o cmdResultCode (I) - The integer result returned by the command that
 *     Tcl_Eval just completed.  Should be TCL_OK if not called from
 *     Tcl_Eval.
 * Globals:
 *   o tclReceivedSignal (I/O) - Will be cleared.
 *   o signalsReceived (I/O) - The count of each signal that was received.
 * Returns:
 *   Either the original result code, an error result if one of the
 *   trap commands returned an error, or an error indicating the
 *   a signal occured.
 *-----------------------------------------------------------------------------
 */
int
Tcl_CheckForSignal (interp, cmdResultCode)
    Tcl_Interp *interp;
    int         cmdResultCode;
{
    char   *savedResult;
    int     signalNum, result, sigCnt, retErrorForSignal = -1;

    if (!tclReceivedSignal)
        return cmdResultCode;  /* No signal received */

    savedResult = ckalloc (strlen (interp->result) + 1);
    strcpy (savedResult, interp->result);
    Tcl_ResetResult (interp);

    for (signalNum = 1; signalNum < MAXSIG; signalNum++) {
        if (signalsReceived [signalNum] == 0)
            continue;
        
        if (signalTrapCmds [signalNum] == NULL) {
            retErrorForSignal = signalNum;
            signalsReceived [signalNum] = 0;
        } else {
            sigCnt = signalsReceived [signalNum];
            signalsReceived [signalNum] = 0;
            
            while (sigCnt-- > 0) {
                result = EvalTrapCode (interp, signalNum,
                                       signalTrapCmds [signalNum]);
                if (result == TCL_ERROR)
                    goto exitPoint;
            }
        }
    }

    if (retErrorForSignal >= 0) {
        char *signalName;

        /*
         * Force name to always be SIGCHLD, even if system defines only SIGCLD.
         */
        if (retErrorForSignal == SIGCHLD)
            signalName = "SIGCHLD";
        else
            signalName = Tcl_SignalId (retErrorForSignal);

        Tcl_SetErrorCode (interp, "UNIX", "SIG", signalName, (char*) NULL);
        Tcl_AppendResult (interp, signalName, " signal received", 
                          (char *)NULL);
        Tcl_SetVar (interp, "errorInfo", "", TCL_GLOBAL_ONLY);
        result = TCL_ERROR;
    } else {
        Tcl_SetResult (interp, savedResult, TCL_DYNAMIC);
        savedResult = NULL;
        result = cmdResultCode;
    }

exitPoint:
    if (savedResult != NULL)
        ckfree (savedResult);
    /*
     * An error might have caused clearing of some signal flags to be missed.
     */
    Tcl_ResetSignals ();
    return result;
}

/*
 *-----------------------------------------------------------------------------
 *
 * ParseSignalList --
 *  
 *   Parse a list of signal names or numbers.
 * 
 * Parameters:
 *   o interp (O) - Interpreter for returning errors.
 *   o signalListStr (I) - The Tcl list of signals to convert.
 *   o signalList (O) - The list of converted signal numbers, must be
 *     big enough to hold MAXSIG signals.
 *     Tcl_Eval just completed.
 * Returns:
 *   The number of signals converted, or -1 if an error occures.
 *-----------------------------------------------------------------------------
 */
static int
ParseSignalList (interp, signalListStr, signalList)
    Tcl_Interp *interp;
    char       *signalListStr;
    int         signalList [];
{
    char         **signalListArgv;
    int            signalListSize, signalNum, idx;
    int            result = -1;
    char          *signalName;

    if (Tcl_SplitList (interp, signalListStr, &signalListSize, 
                       &signalListArgv) != TCL_OK)
        return -1;

    if (signalListSize > MAXSIG) {
        Tcl_AppendResult (interp, "too many signals supplied in list",
                          (char *) NULL);
        goto exitPoint;
    }

    if (signalListSize == 0) {
        Tcl_AppendResult (interp, "signal list may not be empty",
                          (char *) NULL);
        goto exitPoint;
    }

    for (idx = 0; idx < signalListSize; idx++) {
        signalName = signalListArgv [idx];

        if (Tcl_StrToInt (signalName, 0, &signalNum))
            signalName = Tcl_SignalId (signalNum);
        else
            signalNum = SigNameToNum (signalName);

        if (signalName == NULL) {
            char numBuf [20];

            sprintf (numBuf, "%d", signalNum);
            Tcl_AppendResult (interp, "invalid signal number: ",
                              numBuf, (char *) NULL);
            goto exitPoint;
        }

        if ((signalNum < 1) || (signalNum > NSIG)) {
            Tcl_AppendResult (interp, "invalid signal name: ",
                              signalName, (char *) NULL);
            goto exitPoint;
        }
        signalList [idx] = signalNum;
    }

    result = signalListSize;
exitPoint:
    ckfree ((char *) signalListArgv);
    return result;

}

/*
 *-----------------------------------------------------------------------------
 *
 * SignalBlocked --
 *     
 *    Determine if a signal is blocked.  On non-Posix systems, always returns
 * "0".
 *
 * Parameters::
 *   o interp (O) - Error messages are returned in result.
 *   o signalNum (I) - The signal to determine the state for.
 * Returns:
 *   NULL if an error occured, or a pointer to a static string of "1" if the
 * signal is block, and a static string of "0" if it is not blocked.
 *-----------------------------------------------------------------------------
 */
static char *
SignalBlocked (interp, signalNum)
    Tcl_Interp  *interp;
    int          signalNum;
{
#ifdef TCL_POSIX_SIG
    int      idx;
    sigset_t sigBlockSet;

    if (sigprocmask (SIG_BLOCK, NULL, &sigBlockSet)) {
        interp->result = Tcl_UnixError (interp);
        return NULL;
    }
    return sigismember (&sigBlockSet, signalNum) ? "1" : "0";
#else
    return "0";
#endif
}

/*
 *-----------------------------------------------------------------------------
 *
 * GetSignalStates --
 *     
 *    Return a keyed list containing the signal states for the specified
 * signals.
 *
 * Parameters::
 *   o interp (O) - The list is returned in the result.
 *   o signalListSize (I) - Number of signals in the signal list.
 *   o signalList (I) - List of signals of requested signals.
 * Returns:
 *   TCL_OK or TCL_ERROR, with error message in interp.
 *-----------------------------------------------------------------------------
 */
static int
GetSignalStates (interp, signalListSize, signalList)
    Tcl_Interp *interp;
    int         signalListSize;
    int         signalList [MAXSIG];
{
    int              idx, signalNum, actuallyDone = -1;
    char            *stateKeyedList [MAXSIG];
    char            *sigState [3], *sigEntry [2];
    signalProcPtr_t  actionFunc;

    for (idx = 0; idx < signalListSize; idx ++) {
        signalNum = signalList [idx];

        actionFunc = GetSignalState (signalNum);
        if (actionFunc == SIG_ERR)
            goto unixSigError;
        
        sigState [2] = NULL;
        if (actionFunc == SIG_DFL)
            sigState [0]  = "default";
        else if (actionFunc == SIG_IGN)
            sigState [0] = "ignore";
        else if (actionFunc == TclSignalTrap) {
            if (signalTrapCmds [signalNum] == NULL)
                sigState [0] = "error";
            else {
                sigState [0] = "trap";
                sigState [2] = signalTrapCmds [signalNum];
            }
        }

        sigState [1] = SignalBlocked (interp, signalNum);
        if (sigState [1] == NULL)
            goto unixSigError;

        sigEntry [0] = Tcl_SignalId (signalNum);
        sigEntry [1] = Tcl_Merge ((sigState [2] == NULL) ? 2 : 3,
                                  sigState);

        stateKeyedList [idx] = Tcl_Merge (2, sigEntry);
        ckfree (sigEntry [1]);

        actuallyDone = idx;

    }
    Tcl_SetResult (interp, Tcl_Merge (signalListSize, stateKeyedList),
                   TCL_DYNAMIC);

    for (idx = 0; idx <= actuallyDone; idx++)
        ckfree (stateKeyedList [idx]);

    return TCL_OK;

unixSigError:
    for (idx = 0; idx <= actuallyDone; idx++)
        ckfree (stateKeyedList [idx]);

    interp->result = Tcl_UnixError (interp);
    return TCL_ERROR;
}

/*
 *-----------------------------------------------------------------------------
 *
 * SetSignalStates --
 *     
 *    Set the signal state for the specified signals.  
 *
 * Parameters::
 *   o interp (O) - The list is returned in the result.
 *   o signalListSize (I) - Number of signals in the signal list.
 *   o signalList (I) - List of signals of requested signals.
 *   o actionFunc (I) - The function to run when the signal is received.
 *   o command (I) - If the function is the "trap" function, this is the
 *     Tcl command to run when the trap occurs.  Otherwise, NULL.
 * Returns:
 *   TCL_OK or TCL_ERROR, with error message in interp.
 *-----------------------------------------------------------------------------
 */
static int
SetSignalStates (interp, signalListSize, signalList, actionFunc, command)
    Tcl_Interp      *interp;
    int              signalListSize;
    int              signalList [MAXSIG];
    signalProcPtr_t  actionFunc;
    char            *command;

{
    int idx, signalNum, commandLen;

    if (command != NULL)
        commandLen = strlen (command);

    for (idx = 0; idx < signalListSize; idx ++) {
        signalNum = signalList [idx];

        if (signalTrapCmds [signalNum] != NULL) {
            ckfree (signalTrapCmds [signalNum]);
            signalTrapCmds [signalNum] = NULL;
        }
        if (!SetSignalAction (signalNum, actionFunc))
            goto unixSigError;

        if (command != NULL) {
            signalTrapCmds [signalNum] = ckalloc (commandLen + 1);
            strcpy (signalTrapCmds [signalNum], command);
        }
    }

    return TCL_OK;

unixSigError:
    interp->result = Tcl_UnixError (interp);
    return TCL_ERROR;
}

/*
 *-----------------------------------------------------------------------------
 *
 * BlockSignals --
 *     
 *    Block or unblock the specified signals.  Returns an error if not a Posix
 * system.
 *
 * Parameters::
 *   o interp (O) - Error messages are returned in result.
 *   o action (I) - SIG_BLOCK or SIG_UNBLOCK.
 *   o signalListSize (I) - Number of signals in the signal list.
 *   o signalList (I) - List of signals of requested signals.
 * Returns:
 *   TCL_OK or TCL_ERROR, with error message in interp.
 *-----------------------------------------------------------------------------
 */
static int
BlockSignals (interp, action, signalListSize, signalList)
    Tcl_Interp  *interp;
    int          action;
    int          signalListSize;
    int          signalList [MAXSIG];
{
#ifdef TCL_POSIX_SIG
    int      idx;
    sigset_t sigBlockSet;

    sigemptyset (&sigBlockSet);

    for (idx = 0; idx < signalListSize; idx ++)
        sigaddset (&sigBlockSet, signalList [idx]);

    if (sigprocmask (action, &sigBlockSet, NULL)) {
        interp->result = Tcl_UnixError (interp);
        return TCL_ERROR;
    }

    return TCL_OK;
#else
    interp->result = noPosix;
    return TCL_ERROR;
#endif
}

/*
 *-----------------------------------------------------------------------------
 *
 * Tcl_SignalCmd --
 *     Implements the TCL signal command:
 *         signal action siglist [command]
 *
 * Results:
 *      Standard TCL results, may return the UNIX system error message.
 *
 * Side effects:
 *	Signal handling states may be changed.
 *-----------------------------------------------------------------------------
 */
static int
Tcl_SignalCmd (clientData, interp, argc, argv)
    char       *clientData;
    Tcl_Interp *interp;
    int         argc;
    char      **argv;
{
    int                  signalListSize, signalNum, idx;
    int                  signalList [MAXSIG], actionClass;
    char                *signalName;
    signalProcPtr_t      actionFunc;
    char                *command = NULL;

    if ((argc < 3) || (argc > 4)) {
        Tcl_AppendResult (interp, tclXWrongArgs, argv [0], 
                          " action signalList [commands]", (char *) NULL);
        return TCL_ERROR;
    }

    signalListSize = ParseSignalList (interp, argv [2], signalList);
    if (signalListSize < 0)    
        return TCL_ERROR;

    /*
     * Determine the action to take on all of the signals.
     */
    if (STREQU (argv [1], "trap")) {
        actionFunc = TclSignalTrap;
        actionClass = SIGACT_SET;
        if (argc != 4) {
            Tcl_AppendResult (interp, "command required for ",
                             "trapping signals", (char *) NULL);
            return TCL_ERROR;
        }
        command = argv [3];
    } else {
        if (STREQU (argv [1], "default")) {
            actionFunc  = SIG_DFL;
            actionClass = SIGACT_SET;
        } else if (STREQU (argv [1], "ignore")) {
            actionFunc = SIG_IGN;
            actionClass = SIGACT_SET;
        } else if (STREQU (argv [1], "error")) {
            actionFunc = TclSignalTrap;
            actionClass = SIGACT_SET;
        } else if (STREQU (argv [1], "get")) {
            actionClass = SIGACT_GET;
        } else if (STREQU (argv [1], "block")) {
            actionClass = SIGACT_BLOCK;
        } else if (STREQU (argv [1], "unblock")) {
            actionClass = SIGACT_UNBLOCK;
        } else {
            Tcl_AppendResult (interp, "invalid signal action specified: ", 
                              argv [1], ": expected one of \"default\", ",
                              "\"ignore\", \"error\", \"trap\", or \"get\", ",
                              "\"block\", \"unblock\"", (char *) NULL);
            return TCL_ERROR;
        }
        if (argc != 3) {
            Tcl_AppendResult (interp, "command may not be ",
                              "specified for \"", argv [1], "\" action",
                              (char *) NULL);
            return TCL_ERROR;
        }
    }

    /*
     * Process the specified action class.
     */
    switch (actionClass) {
      case SIGACT_SET:
        return SetSignalStates (interp, signalListSize, signalList,
                                actionFunc, command);
      case SIGACT_GET:
        return GetSignalStates (interp, signalListSize, signalList);
      case SIGACT_BLOCK:
        return BlockSignals (interp, SIG_BLOCK, signalListSize, signalList);
      case SIGACT_UNBLOCK:
        return BlockSignals (interp, SIG_UNBLOCK, signalListSize, signalList);
    }

}

/*
 *-----------------------------------------------------------------------------
 *
 *  SignalCmdCleanUp --
 *      Clean up the signal table when the interpreter is deleted.  This
 *      is actually when the signal command is deleted.  It releases the
 *      all signal commands that have been allocated.
 *
 *-----------------------------------------------------------------------------
 */
static void
SignalCmdCleanUp (clientData)
    ClientData clientData;
{
    int idx;

    for (idx = 0; idx < MAXSIG; idx++)
        if (signalTrapCmds [idx] != NULL) {
            ckfree (signalTrapCmds [idx]);
            signalTrapCmds [idx] = NULL;
        }

}

/*
 *-----------------------------------------------------------------------------
 *
 * Tcl_InitSignalHandling --
 *      Initializes the TCL unix commands.
 *
 * Side effects:
 *    A catch trap is armed for the SIGINT signal.
 *
 *-----------------------------------------------------------------------------
 */
void
Tcl_InitSignalHandling (interp)
    Tcl_Interp *interp;
{
    int idx;

    for (idx = 0; idx < MAXSIG; idx++) {
        signalsReceived [idx] = 0;
        signalTrapCmds [idx] = NULL;
    }
    Tcl_CreateCommand (interp, "kill", Tcl_KillCmd, (ClientData)NULL,
                      (void (*)())NULL);
    Tcl_CreateCommand (interp, "signal", Tcl_SignalCmd, (ClientData)NULL,
                      SignalCmdCleanUp);
    /*
     * If interrupt is currently  being trapped, enabled it.  Other wise
     * leave it off, or if this process is running as a background job it will
     * get its parent's (shell's) signals.
     */
    if (GetSignalState (SIGINT) != SIG_IGN)
        SetSignalAction (SIGINT, TclSignalTrap);
}
