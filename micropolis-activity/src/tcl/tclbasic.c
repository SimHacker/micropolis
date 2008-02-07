/* 
 * tclBasic.c --
 *
 *	Contains the basic facilities for TCL command interpretation,
 *	including interpreter creation and deletion, command creation
 *	and deletion, and command parsing and execution.
 *
 * Copyright 1987-1992 Regents of the University of California
 * Permission to use, copy, modify, and distribute this
 * software and its documentation for any purpose and without
 * fee is hereby granted, provided that the above copyright
 * notice appear in all copies.  The University of California
 * makes no representations about the suitability of this
 * software for any purpose.  It is provided "as is" without
 * express or implied warranty.
 */

#ifndef lint
static char rcsid[] = "$Header: /user6/ouster/tcl/RCS/tclBasic.c,v 1.131 92/06/21 14:09:41 ouster Exp $ SPRITE (Berkeley)";
#endif

#include "tclint.h"

/*
 * The following structure defines all of the commands in the Tcl core,
 * and the C procedures that execute them.
 */

typedef struct {
    char *name;			/* Name of command. */
    Tcl_CmdProc *proc;		/* Procedure that executes command. */
} CmdInfo;

/*
 * Built-in commands, and the procedures associated with them:
 */

static CmdInfo builtInCmds[] = {
    /*
     * Commands in the generic core:
     */

    {"append",		Tcl_AppendCmd},
    {"array",		Tcl_ArrayCmd},
    {"break",		Tcl_BreakCmd},
    {"case",		Tcl_CaseCmd},
    {"catch",		Tcl_CatchCmd},
    {"concat",		Tcl_ConcatCmd},
    {"continue",	Tcl_ContinueCmd},
    {"error",		Tcl_ErrorCmd},
    {"eval",		Tcl_EvalCmd},
    {"expr",		Tcl_ExprCmd},
    {"for",		Tcl_ForCmd},
    {"foreach",		Tcl_ForeachCmd},
    {"format",		Tcl_FormatCmd},
    {"global",		Tcl_GlobalCmd},
    {"if",		Tcl_IfCmd},
    {"incr",		Tcl_IncrCmd},
    {"info",		Tcl_InfoCmd},
    {"join",		Tcl_JoinCmd},
    {"lappend",		Tcl_LappendCmd},
    {"lindex",		Tcl_LindexCmd},
    {"linsert",		Tcl_LinsertCmd},
    {"list",		Tcl_ListCmd},
    {"llength",		Tcl_LlengthCmd},
    {"lrange",		Tcl_LrangeCmd},
    {"lreplace",	Tcl_LreplaceCmd},
    {"lsearch",		Tcl_LsearchCmd},
    {"lsort",		Tcl_LsortCmd},
    {"proc",		Tcl_ProcCmd},
    {"regexp",		Tcl_RegexpCmd},
    {"regsub",		Tcl_RegsubCmd},
    {"rename",		Tcl_RenameCmd},
    {"return",		Tcl_ReturnCmd},
    {"scan",		Tcl_ScanCmd},
    {"set",		Tcl_SetCmd},
    {"split",		Tcl_SplitCmd},
    {"string",		Tcl_StringCmd},
    {"trace",		Tcl_TraceCmd},
    {"unset",		Tcl_UnsetCmd},
    {"uplevel",		Tcl_UplevelCmd},
    {"upvar",		Tcl_UpvarCmd},
    {"while",		Tcl_WhileCmd},

    /*
     * Commands in the UNIX core:
     */

#ifndef TCL_GENERIC_ONLY
    {"cd",		Tcl_CdCmd},
    {"close",		Tcl_CloseCmd},
    {"eof",		Tcl_EofCmd},
    {"exec",		Tcl_ExecCmd},
    {"exit",		Tcl_ExitCmd},
    {"file",		Tcl_FileCmd},
    {"flush",		Tcl_FlushCmd},
    {"gets",		Tcl_GetsCmd},
    {"glob",		Tcl_GlobCmd},
    {"open",		Tcl_OpenCmd},
    {"puts",		Tcl_PutsCmd},
    {"pwd",		Tcl_PwdCmd},
    {"read",		Tcl_ReadCmd},
    {"seek",		Tcl_SeekCmd},
    {"source",		Tcl_SourceCmd},
    {"tell",		Tcl_TellCmd},
    {"time",		Tcl_TimeCmd},
#endif /* TCL_GENERIC_ONLY */
    {NULL,		(Tcl_CmdProc *) NULL}
};

/*
 *----------------------------------------------------------------------
 *
 * Tcl_CreateInterp --
 *
 *	Create a new TCL command interpreter.
 *
 * Results:
 *	The return value is a token for the interpreter, which may be
 *	used in calls to procedures like Tcl_CreateCmd, Tcl_Eval, or
 *	Tcl_DeleteInterp.
 *
 * Side effects:
 *	The command interpreter is initialized with an empty variable
 *	table and the built-in commands.
 *
 *----------------------------------------------------------------------
 */

Tcl_Interp *
Tcl_CreateInterp()
{
    register Interp *iPtr;
    register Command *cmdPtr;
    register CmdInfo *cmdInfoPtr;
    int i;

    iPtr = (Interp *) ckalloc(sizeof(Interp));
    iPtr->result = iPtr->resultSpace;
    iPtr->freeProc = 0;
    iPtr->errorLine = 0;
    Tcl_InitHashTable(&iPtr->commandTable, TCL_STRING_KEYS);
    Tcl_InitHashTable(&iPtr->globalTable, TCL_STRING_KEYS);
    iPtr->numLevels = 0;
    iPtr->framePtr = NULL;
    iPtr->varFramePtr = NULL;
    iPtr->activeTracePtr = NULL;
    iPtr->numEvents = 0;
    iPtr->events = NULL;
    iPtr->curEvent = 0;
    iPtr->curEventNum = 0;
    iPtr->revPtr = NULL;
    iPtr->historyFirst = NULL;
    iPtr->revDisables = 1;
    iPtr->evalFirst = iPtr->evalLast = NULL;
    iPtr->appendResult = NULL;
    iPtr->appendAvl = 0;
    iPtr->appendUsed = 0;
    iPtr->numFiles = 0;
    iPtr->filePtrArray = NULL;
    for (i = 0; i < NUM_REGEXPS; i++) {
	iPtr->patterns[i] = NULL;
	iPtr->patLengths[i] = -1;
	iPtr->regexps[i] = NULL;
    }
    iPtr->cmdCount = 0;
    iPtr->noEval = 0;
    iPtr->scriptFile = NULL;
    iPtr->flags = 0;
    iPtr->tracePtr = NULL;
    iPtr->resultSpace[0] = 0;

    /*
     * Create the built-in commands.  Do it here, rather than calling
     * Tcl_CreateCommand, because it's faster (there's no need to
     * check for a pre-existing command by the same name).
     */

    for (cmdInfoPtr = builtInCmds; cmdInfoPtr->name != NULL; cmdInfoPtr++) {
	int new;
	Tcl_HashEntry *hPtr;

	hPtr = Tcl_CreateHashEntry(&iPtr->commandTable,
		cmdInfoPtr->name, &new);
	if (new) {
	    cmdPtr = (Command *) ckalloc(sizeof(Command));
	    cmdPtr->proc = cmdInfoPtr->proc;
	    cmdPtr->clientData = (ClientData) NULL;
	    cmdPtr->deleteProc = NULL;
	    Tcl_SetHashValue(hPtr, cmdPtr);
	}
    }

#ifndef TCL_GENERIC_ONLY
    TclSetupEnv((Tcl_Interp *) iPtr);
#endif

    return (Tcl_Interp *) iPtr;
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_DeleteInterp --
 *
 *	Delete an interpreter and free up all of the resources associated
 *	with it.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The interpreter is destroyed.  The caller should never again
 *	use the interp token.
 *
 *----------------------------------------------------------------------
 */

void
Tcl_DeleteInterp(interp)
    Tcl_Interp *interp;		/* Token for command interpreter (returned
				 * by a previous call to Tcl_CreateInterp). */
{
    Interp *iPtr = (Interp *) interp;
    Tcl_HashEntry *hPtr;
    Tcl_HashSearch search;
    register Command *cmdPtr;
    int i;

    /*
     * If the interpreter is in use, delay the deletion until later.
     */

    iPtr->flags |= DELETED;
    if (iPtr->numLevels != 0) {
	return;
    }

    /*
     * Free up any remaining resources associated with the
     * interpreter.
     */

    for (hPtr = Tcl_FirstHashEntry(&iPtr->commandTable, &search);
	    hPtr != NULL; hPtr = Tcl_NextHashEntry(&search)) {
	cmdPtr = (Command *) Tcl_GetHashValue(hPtr);
	if (cmdPtr->deleteProc != NULL) { 
	    (*cmdPtr->deleteProc)(cmdPtr->clientData);
	}
	ckfree((char *) cmdPtr);
    }
    Tcl_DeleteHashTable(&iPtr->commandTable);
    TclDeleteVars(iPtr, &iPtr->globalTable);
    if (iPtr->events != NULL) {
	int i;

	for (i = 0; i < iPtr->numEvents; i++) {
	    ckfree(iPtr->events[i].command);
	}
	ckfree((char *) iPtr->events);
    }
    while (iPtr->revPtr != NULL) {
	HistoryRev *nextPtr = iPtr->revPtr->nextPtr;

	ckfree((char *) iPtr->revPtr);
	iPtr->revPtr = nextPtr;
    }
    if (iPtr->appendResult != NULL) {
	ckfree(iPtr->appendResult);
    }
#ifndef TCL_GENERIC_ONLY
    if (iPtr->numFiles > 0) {
	for (i = 0; i < iPtr->numFiles; i++) {
	    OpenFile *filePtr;
    
	    filePtr = iPtr->filePtrArray[i];
	    if (filePtr == NULL) {
		continue;
	    }
	    if (i >= 3) {
		fclose(filePtr->f);
		if (filePtr->f2 != NULL) {
		    fclose(filePtr->f2);
		}
		if (filePtr->numPids > 0) {
		    Tcl_DetachPids(filePtr->numPids, filePtr->pidPtr);
		    ckfree((char *) filePtr->pidPtr);
		}
	    }
	    ckfree((char *) filePtr);
	}
	ckfree((char *) iPtr->filePtrArray);
    }
#endif
    for (i = 0; i < NUM_REGEXPS; i++) {
	if (iPtr->patterns[i] == NULL) {
	    break;
	}
	ckfree(iPtr->patterns[i]);
	ckfree((char *) iPtr->regexps[i]);
    }
    while (iPtr->tracePtr != NULL) {
	Trace *nextPtr = iPtr->tracePtr->nextPtr;

	ckfree((char *) iPtr->tracePtr);
	iPtr->tracePtr = nextPtr;
    }
    ckfree((char *) iPtr);
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_CreateCommand --
 *
 *	Define a new command in a command table.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	If a command named cmdName already exists for interp, it is
 *	deleted.  In the future, when cmdName is seen as the name of
 *	a command by Tcl_Eval, proc will be called.  When the command
 *	is deleted from the table, deleteProc will be called.  See the
 *	manual entry for details on the calling sequence.
 *
 *----------------------------------------------------------------------
 */

void
Tcl_CreateCommand(interp, cmdName, proc, clientData, deleteProc)
    Tcl_Interp *interp;		/* Token for command interpreter (returned
				 * by a previous call to Tcl_CreateInterp). */
    char *cmdName;		/* Name of command. */
    Tcl_CmdProc *proc;		/* Command procedure to associate with
				 * cmdName. */
    ClientData clientData;	/* Arbitrary one-word value to pass to proc. */
    Tcl_CmdDeleteProc *deleteProc;
				/* If not NULL, gives a procedure to call when
				 * this command is deleted. */
{
    Interp *iPtr = (Interp *) interp;
    register Command *cmdPtr;
    Tcl_HashEntry *hPtr;
    int new;

    hPtr = Tcl_CreateHashEntry(&iPtr->commandTable, cmdName, &new);
    if (!new) {
	/*
	 * Command already exists:  delete the old one.
	 */

	cmdPtr = (Command *) Tcl_GetHashValue(hPtr);
	if (cmdPtr->deleteProc != NULL) {
	    (*cmdPtr->deleteProc)(cmdPtr->clientData);
	}
    } else {
	cmdPtr = (Command *) ckalloc(sizeof(Command));
	Tcl_SetHashValue(hPtr, cmdPtr);
    }
    cmdPtr->proc = proc;
    cmdPtr->clientData = clientData;
    cmdPtr->deleteProc = deleteProc;
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_DeleteCommand --
 *
 *	Remove the given command from the given interpreter.
 *
 * Results:
 *	0 is returned if the command was deleted successfully.
 *	-1 is returned if there didn't exist a command by that
 *	name.
 *
 * Side effects:
 *	CmdName will no longer be recognized as a valid command for
 *	interp.
 *
 *----------------------------------------------------------------------
 */

int
Tcl_DeleteCommand(interp, cmdName)
    Tcl_Interp *interp;		/* Token for command interpreter (returned
				 * by a previous call to Tcl_CreateInterp). */
    char *cmdName;		/* Name of command to remove. */
{
    Interp *iPtr = (Interp *) interp;
    Tcl_HashEntry *hPtr;
    Command *cmdPtr;

    hPtr = Tcl_FindHashEntry(&iPtr->commandTable, cmdName);
    if (hPtr == NULL) {
	return -1;
    }
    cmdPtr = (Command *) Tcl_GetHashValue(hPtr);
    if (cmdPtr->deleteProc != NULL) {
	(*cmdPtr->deleteProc)(cmdPtr->clientData);
    }
    ckfree((char *) cmdPtr);
    Tcl_DeleteHashEntry(hPtr);
    return 0;
}

/*
 *-----------------------------------------------------------------
 *
 * Tcl_Eval --
 *
 *	Parse and execute a command in the Tcl language.
 *
 * Results:
 *	The return value is one of the return codes defined in tcl.hd
 *	(such as TCL_OK), and interp->result contains a string value
 *	to supplement the return code.  The value of interp->result
 *	will persist only until the next call to Tcl_Eval:  copy it or
 *	lose it! *TermPtr is filled in with the character just after
 *	the last one that was part of the command (usually a NULL
 *	character or a closing bracket).
 *
 * Side effects:
 *	Almost certainly;  depends on the command.
 *
 *-----------------------------------------------------------------
 */

int
Tcl_Eval(interp, cmd, flags, termPtr)
    Tcl_Interp *interp;		/* Token for command interpreter (returned
				 * by a previous call to Tcl_CreateInterp). */
    char *cmd;			/* Pointer to TCL command to interpret. */
    int flags;			/* OR-ed combination of flags like
				 * TCL_BRACKET_TERM and TCL_RECORD_BOUNDS. */
    char **termPtr;		/* If non-NULL, fill in the address it points
				 * to with the address of the char. just after
				 * the last one that was part of cmd.  See
				 * the man page for details on this. */
{
    /*
     * The storage immediately below is used to generate a copy
     * of the command, after all argument substitutions.  Pv will
     * contain the argv values passed to the command procedure.
     */

#   define NUM_CHARS 200
    char copyStorage[NUM_CHARS];
    ParseValue pv;
    char *oldBuffer;

    /*
     * This procedure generates an (argv, argc) array for the command,
     * It starts out with stack-allocated space but uses dynamically-
     * allocated storage to increase it if needed.
     */

#   define NUM_ARGS 10
    char *(argStorage[NUM_ARGS]);
    char **argv = argStorage;
    int argc;
    int argSize = NUM_ARGS;

    register char *src;			/* Points to current character
					 * in cmd. */
    char termChar;			/* Return when this character is found
					 * (either ']' or '\0').  Zero means
					 * that newlines terminate commands. */
    int result;				/* Return value. */
    register Interp *iPtr = (Interp *) interp;
    Tcl_HashEntry *hPtr;
    Command *cmdPtr;
    char *dummy;			/* Make termPtr point here if it was
					 * originally NULL. */
    char *cmdStart;			/* Points to first non-blank char. in
					 * command (used in calling trace
					 * procedures). */
    char *ellipsis = "";		/* Used in setting errorInfo variable;
					 * set to "..." to indicate that not
					 * all of offending command is included
					 * in errorInfo.  "" means that the
					 * command is all there. */
    register Trace *tracePtr;

    /*
     * Initialize the result to an empty string and clear out any
     * error information.  This makes sure that we return an empty
     * result if there are no commands in the command string.
     */

    Tcl_FreeResult((Tcl_Interp *) iPtr);
    iPtr->result = iPtr->resultSpace;
    iPtr->resultSpace[0] = 0;
    result = TCL_OK;

    /*
     * Check depth of nested calls to Tcl_Eval:  if this gets too large,
     * it's probably because of an infinite loop somewhere.
     */

    iPtr->numLevels++;
    if (iPtr->numLevels > MAX_NESTING_DEPTH) {
	iPtr->numLevels--;
	iPtr->result =  "too many nested calls to Tcl_Eval (infinite loop?)";
	return TCL_ERROR;
    }

    /*
     * Initialize the area in which command copies will be assembled.
     */

    pv.buffer = copyStorage;
    pv.end = copyStorage + NUM_CHARS - 1;
    pv.expandProc = TclExpandParseValue;
    pv.clientData = (ClientData) NULL;

    src = cmd;
    if (flags & TCL_BRACKET_TERM) {
	termChar = ']';
    } else {
	termChar = 0;
    }
    if (termPtr == NULL) {
	termPtr = &dummy;
    }
    *termPtr = src;
    cmdStart = src;

    /*
     * There can be many sub-commands (separated by semi-colons or
     * newlines) in one command string.  This outer loop iterates over
     * individual commands.
     */

    while (*src != termChar) {
	iPtr->flags &= ~(ERR_IN_PROGRESS | ERROR_CODE_SET);

	/*
	 * Skim off leading white space and semi-colons, and skip
	 * comments.
	 */

	while (1) {
	    register char c = *src;

	    if ((CHAR_TYPE(c) != TCL_SPACE) && (c != ';') && (c != '\n')) {
		break;
	    }
	    src += 1;
	}
	if (*src == '#') {
	    for (src++; *src != 0; src++) {
		if (*src == '\n') {
		    src++;
		    break;
		}
	    }
	    continue;
	}
	cmdStart = src;

	/*
	 * Parse the words of the command, generating the argc and
	 * argv for the command procedure.  May have to call
	 * TclParseWords several times, expanding the argv array
	 * between calls.
	 */

	pv.next = oldBuffer = pv.buffer;
	argc = 0;
	while (1) {
	    int newArgs, maxArgs;
	    char **newArgv;
	    int i;

	    /*
	     * Note:  the "- 2" below guarantees that we won't use the
	     * last two argv slots here.  One is for a NULL pointer to
	     * mark the end of the list, and the other is to leave room
	     * for inserting the command name "unknown" as the first
	     * argument (see below).
	     */

	    maxArgs = argSize - argc - 2;
	    result = TclParseWords((Tcl_Interp *) iPtr, src, flags,
		    maxArgs, termPtr, &newArgs, &argv[argc], &pv);
	    src = *termPtr;
	    if (result != TCL_OK) {
		ellipsis = "...";
		goto done;
	    }

	    /*
	     * Careful!  Buffer space may have gotten reallocated while
	     * parsing words.  If this happened, be sure to update all
	     * of the older argv pointers to refer to the new space.
	     */

	    if (oldBuffer != pv.buffer) {
		int i;

		for (i = 0; i < argc; i++) {
		    argv[i] = pv.buffer + (argv[i] - oldBuffer);
		}
		oldBuffer = pv.buffer;
	    }
	    argc += newArgs;
	    if (newArgs < maxArgs) {
		argv[argc] = (char *) NULL;
		break;
	    }

	    /*
	     * Args didn't all fit in the current array.  Make it bigger.
	     */

	    argSize *= 2;
	    newArgv = (char **)
		    ckalloc((unsigned) argSize * sizeof(char *));
	    for (i = 0; i < argc; i++) {
		newArgv[i] = argv[i];
	    }
	    if (argv != argStorage) {
		ckfree((char *) argv);
	    }
	    argv = newArgv;
	}

	/*
	 * If this is an empty command (or if we're just parsing
	 * commands without evaluating them), then just skip to the
	 * next command.
	 */

	if ((argc == 0) || iPtr->noEval) {
	    continue;
	}
	argv[argc] = NULL;

	/*
	 * Save information for the history module, if needed.
	 */

	if (flags & TCL_RECORD_BOUNDS) {
	    iPtr->evalFirst = cmdStart;
	    iPtr->evalLast = src-1;
	}

	/*
	 * Find the procedure to execute this command.  If there isn't
	 * one, then see if there is a command "unknown".  If so,
	 * invoke it instead, passing it the words of the original
	 * command as arguments.
	 */

	hPtr = Tcl_FindHashEntry(&iPtr->commandTable, argv[0]);
	if (hPtr == NULL) {
	    int i;

	    hPtr = Tcl_FindHashEntry(&iPtr->commandTable, "unknown");
	    if (hPtr == NULL) {
		Tcl_ResetResult(interp);
		Tcl_AppendResult(interp, "invalid command name: \"",
			argv[0], "\"", (char *) NULL);
		result = TCL_ERROR;
		goto done;
	    }
	    for (i = argc; i >= 0; i--) {
		argv[i+1] = argv[i];
	    }
	    argv[0] = "unknown";
	    argc++;
	}
	cmdPtr = (Command *) Tcl_GetHashValue(hPtr);

	/*
	 * Call trace procedures, if any.
	 */

	for (tracePtr = iPtr->tracePtr; tracePtr != NULL;
		tracePtr = tracePtr->nextPtr) {
	    char saved;

	    if (tracePtr->level < iPtr->numLevels) {
		continue;
	    }
	    saved = *src;
	    *src = 0;
	    (*tracePtr->proc)(tracePtr->clientData, interp, iPtr->numLevels,
		    cmdStart, cmdPtr->proc, cmdPtr->clientData, argc, argv);
	    *src = saved;
	}

	/*
	 * At long last, invoke the command procedure.  Reset the
	 * result to its default empty value first (it could have
	 * gotten changed by earlier commands in the same command
	 * string).
	 */

	iPtr->cmdCount++;
	Tcl_FreeResult(iPtr);
	iPtr->result = iPtr->resultSpace;
	iPtr->resultSpace[0] = 0;
	result = (*cmdPtr->proc)(cmdPtr->clientData, interp, argc, argv);
	if (result != TCL_OK) {
	    break;
	}
    }

    /*
     * Free up any extra resources that were allocated.
     */

    done:
    if (pv.buffer != copyStorage) {
	ckfree((char *) pv.buffer);
    }
    if (argv != argStorage) {
	ckfree((char *) argv);
    }
    iPtr->numLevels--;
    if (iPtr->numLevels == 0) {
	if (result == TCL_RETURN) {
	    result = TCL_OK;
	}
	if ((result != TCL_OK) && (result != TCL_ERROR)) {
	    Tcl_ResetResult(interp);
	    if (result == TCL_BREAK) {
		iPtr->result = "invoked \"break\" outside of a loop";
	    } else if (result == TCL_CONTINUE) {
		iPtr->result = "invoked \"continue\" outside of a loop";
	    } else {
		iPtr->result = iPtr->resultSpace;
		sprintf(iPtr->resultSpace, "command returned bad code: %d",
			result);
	    }
	    result = TCL_ERROR;
	}
	if (iPtr->flags & DELETED) {
	    Tcl_DeleteInterp(interp);
	}
    }

    /*
     * If an error occurred, record information about what was being
     * executed when the error occurred.
     */

    if ((result == TCL_ERROR) && !(iPtr->flags & ERR_ALREADY_LOGGED)) {
	int numChars;
	register char *p;

	/*
	 * Compute the line number where the error occurred.
	 */

	iPtr->errorLine = 1;
	for (p = cmd; p != cmdStart; p++) {
	    if (*p == '\n') {
		iPtr->errorLine++;
	    }
	}
	for ( ; isspace(*p) || (*p == ';'); p++) {
	    if (*p == '\n') {
		iPtr->errorLine++;
	    }
	}

	/*
	 * Figure out how much of the command to print in the error
	 * message (up to a certain number of characters, or up to
	 * the first new-line).
	 */

	numChars = src - cmdStart;
	if (numChars > (NUM_CHARS-50)) {
	    numChars = NUM_CHARS-50;
	    ellipsis = " ...";
	}

	if (!(iPtr->flags & ERR_IN_PROGRESS)) {
	    sprintf(copyStorage, "\n    while executing\n\"%.*s%s\"",
		    numChars, cmdStart, ellipsis);
	} else {
	    sprintf(copyStorage, "\n    invoked from within\n\"%.*s%s\"",
		    numChars, cmdStart, ellipsis);
	}
	Tcl_AddErrorInfo(interp, copyStorage);
	iPtr->flags &= ~ERR_ALREADY_LOGGED;
    } else {
	iPtr->flags &= ~ERR_ALREADY_LOGGED;
    }
    return result;
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_CreateTrace --
 *
 *	Arrange for a procedure to be called to trace command execution.
 *
 * Results:
 *	The return value is a token for the trace, which may be passed
 *	to Tcl_DeleteTrace to eliminate the trace.
 *
 * Side effects:
 *	From now on, proc will be called just before a command procedure
 *	is called to execute a Tcl command.  Calls to proc will have the
 *	following form:
 *
 *	void
 *	proc(clientData, interp, level, command, cmdProc, cmdClientData,
 *		argc, argv)
 *	    ClientData clientData;
 *	    Tcl_Interp *interp;
 *	    int level;
 *	    char *command;
 *	    int (*cmdProc)();
 *	    ClientData cmdClientData;
 *	    int argc;
 *	    char **argv;
 *	{
 *	}
 *
 *	The clientData and interp arguments to proc will be the same
 *	as the corresponding arguments to this procedure.  Level gives
 *	the nesting level of command interpretation for this interpreter
 *	(0 corresponds to top level).  Command gives the ASCII text of
 *	the raw command, cmdProc and cmdClientData give the procedure that
 *	will be called to process the command and the ClientData value it
 *	will receive, and argc and argv give the arguments to the
 *	command, after any argument parsing and substitution.  Proc
 *	does not return a value.
 *
 *----------------------------------------------------------------------
 */

Tcl_Trace
Tcl_CreateTrace(interp, level, proc, clientData)
    Tcl_Interp *interp;		/* Interpreter in which to create the trace. */
    int level;			/* Only call proc for commands at nesting level
				 * <= level (1 => top level). */
    Tcl_CmdTraceProc *proc;	/* Procedure to call before executing each
				 * command. */
    ClientData clientData;	/* Arbitrary one-word value to pass to proc. */
{
    register Trace *tracePtr;
    register Interp *iPtr = (Interp *) interp;

    tracePtr = (Trace *) ckalloc(sizeof(Trace));
    tracePtr->level = level;
    tracePtr->proc = proc;
    tracePtr->clientData = clientData;
    tracePtr->nextPtr = iPtr->tracePtr;
    iPtr->tracePtr = tracePtr;

    return (Tcl_Trace) tracePtr;
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_DeleteTrace --
 *
 *	Remove a trace.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	From now on there will be no more calls to the procedure given
 *	in trace.
 *
 *----------------------------------------------------------------------
 */

void
Tcl_DeleteTrace(interp, trace)
    Tcl_Interp *interp;		/* Interpreter that contains trace. */
    Tcl_Trace trace;		/* Token for trace (returned previously by
				 * Tcl_CreateTrace). */
{
    register Interp *iPtr = (Interp *) interp;
    register Trace *tracePtr = (Trace *) trace;
    register Trace *tracePtr2;

    if (iPtr->tracePtr == tracePtr) {
	iPtr->tracePtr = tracePtr->nextPtr;
	ckfree((char *) tracePtr);
    } else {
	for (tracePtr2 = iPtr->tracePtr; tracePtr2 != NULL;
		tracePtr2 = tracePtr2->nextPtr) {
	    if (tracePtr2->nextPtr == tracePtr) {
		tracePtr2->nextPtr = tracePtr->nextPtr;
		ckfree((char *) tracePtr);
		return;
	    }
	}
    }
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_AddErrorInfo --
 *
 *	Add information to a message being accumulated that describes
 *	the current error.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The contents of message are added to the "errorInfo" variable.
 *	If Tcl_Eval has been called since the current value of errorInfo
 *	was set, errorInfo is cleared before adding the new message.
 *
 *----------------------------------------------------------------------
 */

void
Tcl_AddErrorInfo(interp, message)
    Tcl_Interp *interp;		/* Interpreter to which error information
				 * pertains. */
    char *message;		/* Message to record. */
{
    register Interp *iPtr = (Interp *) interp;

    /*
     * If an error is already being logged, then the new errorInfo
     * is the concatenation of the old info and the new message.
     * If this is the first piece of info for the error, then the
     * new errorInfo is the concatenation of the message in
     * interp->result and the new message.
     */

    if (!(iPtr->flags & ERR_IN_PROGRESS)) {
	Tcl_SetVar2(interp, "errorInfo", (char *) NULL, interp->result,
		TCL_GLOBAL_ONLY);
	iPtr->flags |= ERR_IN_PROGRESS;

	/*
	 * If the errorCode variable wasn't set by the code that generated
	 * the error, set it to "NONE".
	 */

	if (!(iPtr->flags & ERROR_CODE_SET)) {
	    (void) Tcl_SetVar2(interp, "errorCode", (char *) NULL, "NONE",
		    TCL_GLOBAL_ONLY);
	}
    }
    Tcl_SetVar2(interp, "errorInfo", (char *) NULL, message,
	    TCL_GLOBAL_ONLY|TCL_APPEND_VALUE);
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_VarEval --
 *
 *	Given a variable number of string arguments, concatenate them
 *	all together and execute the result as a Tcl command.
 *
 * Results:
 *	A standard Tcl return result.  An error message or other
 *	result may be left in interp->result.
 *
 * Side effects:
 *	Depends on what was done by the command.
 *
 *----------------------------------------------------------------------
 */
int
Tcl_VarEval(Tcl_Interp *interp, ...)
{
    va_list argList;
#define FIXED_SIZE 200
    char fixedSpace[FIXED_SIZE+1];
    int spaceAvl, spaceUsed, length;
    char *string, *cmd;
    int result;

    /*
     * Copy the strings one after the other into a single larger
     * string.  Use stack-allocated space for small commands, but if
     * the commands gets too large than call ckalloc to create the
     * space.
     */

    va_start(argList, interp);
    spaceAvl = FIXED_SIZE;
    spaceUsed = 0;
    cmd = fixedSpace;
    while (1) {
	string = va_arg(argList, char *);
	if (string == NULL) {
	    break;
	}
	length = strlen(string);
	if ((spaceUsed + length) > spaceAvl) {
	    char *new;

	    spaceAvl = spaceUsed + length;
	    spaceAvl += spaceAvl/2;
	    new = ckalloc((unsigned) spaceAvl);
	    memcpy((VOID *) new, (VOID *) cmd, spaceUsed);
	    if (cmd != fixedSpace) {
		ckfree(cmd);
	    }
	    cmd = new;
	}
	strcpy(cmd + spaceUsed, string);
	spaceUsed += length;
    }
    va_end(argList);
    cmd[spaceUsed] = '\0';

    result = Tcl_Eval(interp, cmd, 0, (char **) NULL);
    if (cmd != fixedSpace) {
	ckfree(cmd);
    }
    return result;
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_GlobalEval --
 *
 *	Evaluate a command at global level in an interpreter.
 *
 * Results:
 *	A standard Tcl result is returned, and interp->result is
 *	modified accordingly.
 *
 * Side effects:
 *	The command string is executed in interp, and the execution
 *	is carried out in the variable context of global level (no
 *	procedures active), just as if an "uplevel #0" command were
 *	being executed.
 *
 *----------------------------------------------------------------------
 */

int
Tcl_GlobalEval(interp, command)
    Tcl_Interp *interp;		/* Interpreter in which to evaluate command. */
    char *command;		/* Command to evaluate. */
{
    register Interp *iPtr = (Interp *) interp;
    int result;
    CallFrame *savedVarFramePtr;

    savedVarFramePtr = iPtr->varFramePtr;
    iPtr->varFramePtr = NULL;
    result = Tcl_Eval(interp, command, 0, (char **) NULL);
    iPtr->varFramePtr = savedVarFramePtr;
    return result;
}
