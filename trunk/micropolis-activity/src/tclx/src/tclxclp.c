/* 
 * tclXcmdloop --
 *
 *   Interactive command loop, C and Tcl callable.
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
 * $Id: tclXcmdloop.c,v 2.0 1992/10/16 04:50:29 markd Rel $
 *-----------------------------------------------------------------------------
 */

#include "tclxint.h"


/*
 * Pointer to eval procedure to use.  This way bring in the history module
 * from a library can be made optional.  This only works because the calling
 * sequence of Tcl_Eval is a superset of Tcl_RecordAndEval.  This defaults
 * to no history, set this variable to Tcl_RecordAndEval to use history.
 */

int (*tclShellCmdEvalProc) () = Tcl_Eval;

/*
 * Prototypes of internal functions.
 */
static int
IsSetVarCmd _ANSI_ARGS_((Tcl_Interp *interp,
                         char       *command));

static void
OutFlush _ANSI_ARGS_((FILE *filePtr));

static void
Tcl_PrintResult _ANSI_ARGS_((FILE   *fp,
                             int     returnval,
                             char   *resultText));

static void
OutputPrompt _ANSI_ARGS_((Tcl_Interp *interp,
                          FILE       *outFP,
                          int         topLevel));

static int
SetPromptVar _ANSI_ARGS_((Tcl_Interp  *interp,
                          char        *hookVarName,
                          char        *newHookValue,
                          char       **oldHookValuePtr));


/*
 *-----------------------------------------------------------------------------
 *
 * IsSetVarCmd --
 *
 *      Determine if the current command is a `set' command that set
 *      a variable (i.e. two arguments).  This routine should only be
 *      called if the command returned TCL_OK.
 *
 *-----------------------------------------------------------------------------
 */
static int
IsSetVarCmd (interp, command)
    Tcl_Interp *interp;
    char       *command;
{
    char  *nextPtr;

    if ((!STRNEQU (command, "set", 3)) || (!isspace (command [3])))
        return FALSE;  /* Quick check */

    nextPtr = TclWordEnd (command, FALSE);
    if (*nextPtr == '\0')
        return FALSE;
    nextPtr = TclWordEnd (nextPtr, FALSE);
    if (*nextPtr == '\0')
        return FALSE;

    while (*nextPtr != '\0') {
        if (!isspace (*nextPtr))
            return TRUE;
        nextPtr++;
    }
    return FALSE;
}

/*
 *-----------------------------------------------------------------------------
 *
 * OutFlush --
 *
 *   Flush a stdio file and check for errors.
 *
 *-----------------------------------------------------------------------------
 */
static void
OutFlush (filePtr)
    FILE *filePtr;
{
    int stat;

    stat = fflush (filePtr);
    if (ferror (filePtr)) {
        if (errno != EINTR)
            panic ("command loop: error writing to output file: %s\n",
                   strerror (errno));
        clearerr (filePtr);
    }
}

/*
 *-----------------------------------------------------------------------------
 *
 * Tcl_PrintResult --
 *
 *      Print a Tcl result
 *
 * Results:
 *
 *      Takes an open file pointer, a return value and some result
 *      text.  Prints the result text if the return value is TCL_OK,
 *      prints "Error:" and the result text if it's TCL_ERROR,
 *      else prints "Bad return code:" and the result text.
 *
 *-----------------------------------------------------------------------------
 */
static void
Tcl_PrintResult (fp, returnval, resultText)
    FILE   *fp;
    int     returnval;
    char   *resultText;
{

    if (returnval == TCL_OK) {
        if (resultText [0] != '\0') {
            fputs (resultText, fp);
            fputs ("\n", fp);
        }
    } else {
        OutFlush (fp);
        fputs ((returnval == TCL_ERROR) ? "Error" : "Bad return code", stderr);
        fputs (": ", stderr);
        fputs (resultText, stderr);
        fputs ("\n", stderr);
        OutFlush (stderr);
    }
}

/*
 *-----------------------------------------------------------------------------
 *
 * OutputPromp --
 *     Outputs a prompt by executing either the command string in
 *     TCLENV(topLevelPromptHook) or TCLENV(downLevelPromptHook).
 *
 *-----------------------------------------------------------------------------
 */
static void
OutputPrompt (interp, outFP, topLevel)
    Tcl_Interp *interp;
    FILE       *outFP;
    int         topLevel;
{
    char *hookName;
    char *promptHook;
    int   result;
    int   promptDone = FALSE;

    hookName = topLevel ? "topLevelPromptHook"
                        : "downLevelPromptHook";

    promptHook = Tcl_GetVar2 (interp, "TCLENV", hookName, 1);
    if ((promptHook != NULL) && (promptHook [0] != '\0')) {
        result = Tcl_Eval (interp, promptHook, 0, (char **)NULL);
        if (!((result == TCL_OK) || (result == TCL_RETURN))) {
            fputs ("Error in prompt hook: ", stderr);
            fputs (interp->result, stderr);
            fputs ("\n", stderr);
            Tcl_PrintResult (outFP, result, interp->result);
        } else {
            fputs (interp->result, outFP);
            promptDone = TRUE;
        }
    } 
    if (!promptDone) {
        if (topLevel)
            fputs ("%", outFP);
        else
            fputs (">", outFP);
    }
    OutFlush (outFP);

}

/*
 *-----------------------------------------------------------------------------
 *
 * Tcl_CommandLoop --
 *
 *   Run a Tcl command loop.  The command loop interactively prompts for,
 * reads and executes commands. Two entries in the global array TCLENV
 * contain prompt hooks.  A prompt hook is Tcl code that is executed and
 * its result is used as the prompt string.  The element `topLevelPromptHook'
 * is the hook that generates the main prompt.  The element
 * `downLevelPromptHook' is the hook to generate the prompt for reading
 * continuation lines for incomplete commands.  If a signal occurs while
 * in the command loop, it is reset and ignored.  EOF terminates the loop.
 *
 * Parameters:
 *   o interp (I) - A pointer to the interpreter
 *   o inFile (I) - The file to read commands from.
 *   o outFile (I) - The file to write the prompts to. 
 *   o evalProc (I) - The function to call to evaluate a command.
 *     Should be either Tcl_Eval or Tcl_RecordAndEval if history is desired.
 *   o options (I) - Currently unused.
 *-----------------------------------------------------------------------------
 */
void
Tcl_CommandLoop (interp, inFile, outFile, evalProc, options)
    Tcl_Interp *interp;
    FILE       *inFile;
    FILE       *outFile;
    int         (*evalProc) ();
    unsigned    options;
{
    Tcl_CmdBuf cmdBuf;
    char       inputBuf[256];
    int        topLevel = TRUE;
    int        result;
    char      *cmd;

    cmdBuf = Tcl_CreateCmdBuf();

    while (TRUE) {
        /*
         * If a signal came in, process it and drop any pending command.
         */
        if (tclReceivedSignal) {
            Tcl_CheckForSignal (interp, TCL_OK);
            Tcl_DeleteCmdBuf(cmdBuf);
            cmdBuf = Tcl_CreateCmdBuf();
            topLevel = TRUE;
        }
        /*
         * Output a prompt and input a command.
         */
        clearerr (inFile);
        clearerr (outFile);
        OutputPrompt (interp, outFile, topLevel);
        errno = 0;
        if (fgets (inputBuf, sizeof (inputBuf), inFile) == NULL) {
            if (!feof(inFile) && (errno == EINTR)) {
                putchar('\n');
                continue;  /* Next command */
            }
            if (ferror (inFile))
                panic ("command loop: error on input file: %s\n",
                       strerror (errno));
            goto endOfFile;
        }
        cmd = Tcl_AssembleCmd(cmdBuf, inputBuf);

        if (cmd == NULL) {
            topLevel = FALSE;
            continue;  /* Next line */
        }
        /*
         * Finally have a complete command, go eval it and maybe output the
         * result.
         */
        result = (*evalProc) (interp, cmd, 0, (char **)NULL);
        if (result != TCL_OK || !IsSetVarCmd (interp, cmd))
            Tcl_PrintResult (outFile, result, interp->result);
        topLevel = TRUE;
    }
endOfFile:
    Tcl_DeleteCmdBuf(cmdBuf);
}

/*
 *-----------------------------------------------------------------------------
 *
 * SetPromptVar --
 *     Set one of the prompt hook variables, saving a copy of the old
 *     value, if it exists.
 *
 * Parameters:
 *   o hookVarName (I) - The name of the prompt hook, which is an element
 *     of the TCLENV array.  One of topLevelPromptHook or downLevelPromptHook.
 *   o newHookValue (I) - The new value for the prompt hook.
 *   o oldHookValuePtr (O) - If not NULL, then a pointer to a copy of the
 *     old prompt value is returned here.  NULL is returned if there was not
 *     old value.  This is a pointer to a malloc-ed string that must be
 *     freed when no longer needed.
 * Result:
 *   TCL_OK if the hook variable was set ok, TCL_ERROR if an error occured.
 *-----------------------------------------------------------------------------
 */
static int
SetPromptVar (interp, hookVarName, newHookValue, oldHookValuePtr)
    Tcl_Interp *interp;
    char       *hookVarName;
    char       *newHookValue;
    char      **oldHookValuePtr;
{
    char *hookValue;    
    char *oldHookPtr = NULL;

    if (oldHookValuePtr != NULL) {
        hookValue = Tcl_GetVar2 (interp, "TCLENV", hookVarName, 
                                 TCL_GLOBAL_ONLY);
        if (hookValue != NULL) {
            oldHookPtr = ckalloc (strlen (hookValue) + 1);
            strcpy (oldHookPtr, hookValue);
        }
    }
    if (Tcl_SetVar2 (interp, "TCLENV", hookVarName, newHookValue, 
                     TCL_GLOBAL_ONLY | TCL_LEAVE_ERR_MSG) == NULL) {
        if (oldHookPtr != NULL)
            ckfree (oldHookPtr);
        return TCL_ERROR;
    }    
    if (oldHookValuePtr != NULL)
        *oldHookValuePtr = oldHookPtr;
    return TCL_OK;
}

/*
 *-----------------------------------------------------------------------------
 *
 * Tcl_CommandloopCmd --
 *     Implements the TCL commandloop command:
 *       commandloop prompt prompt2
 *
 * Results:
 *     Standard TCL results.
 *
 *-----------------------------------------------------------------------------
 */
int
Tcl_CommandloopCmd(clientData, interp, argc, argv)
    ClientData  clientData;
    Tcl_Interp *interp;
    int         argc;
    char      **argv;
{
    char *oldTopLevelHook  = NULL;
    char *oldDownLevelHook = NULL;
    int   result = TCL_ERROR;

    if (argc > 3) {
        Tcl_AppendResult (interp, tclXWrongArgs, argv[0],
                          " [prompt] [prompt2]", (char *) NULL);
        return TCL_ERROR;
    }
    if (argc > 1) {
        if (SetPromptVar (interp, "topLevelPromptHook", argv[1],
                          &oldTopLevelHook) != TCL_OK)
            goto exitPoint;
    }
    if (argc > 2) {
        if (SetPromptVar (interp, "downLevelPromptHook", argv[2], 
                          &oldDownLevelHook) != TCL_OK)
            goto exitPoint;
    }

    Tcl_CommandLoop (interp, stdin, stdout, tclShellCmdEvalProc, 0);

    if (oldTopLevelHook != NULL)
        SetPromptVar (interp, "topLevelPromptHook", oldTopLevelHook, NULL);
    if (oldDownLevelHook != NULL)
        SetPromptVar (interp, "downLevelPromptHook", oldDownLevelHook, NULL);
        
    result = TCL_OK;
exitPoint:
    if (oldTopLevelHook != NULL)
        ckfree (oldTopLevelHook);
    if (oldDownLevelHook != NULL)
        ckfree (oldDownLevelHook);
    return result;
}
