/* 
 * tclProc.c --
 *
 *	This file contains routines that implement Tcl procedures,
 *	including the "proc" and "uplevel" commands.
 *
 * Copyright 1987-1991 Regents of the University of California
 * Permission to use, copy, modify, and distribute this
 * software and its documentation for any purpose and without
 * fee is hereby granted, provided that the above copyright
 * notice appear in all copies.  The University of California
 * makes no representations about the suitability of this
 * software for any purpose.  It is provided "as is" without
 * express or implied warranty.
 */

#ifndef lint
static char rcsid[] = "$Header: /sprite/src/lib/tcl/RCS/tclProc.c,v 1.59 91/09/30 16:59:54 ouster Exp $ SPRITE (Berkeley)";
#endif

#include "tclint.h"

/*
 * Forward references to procedures defined later in this file:
 */

static  int	InterpProc _ANSI_ARGS_((ClientData clientData,
		    Tcl_Interp *interp, int argc, char **argv));
static  void	ProcDeleteProc _ANSI_ARGS_((ClientData clientData));

/*
 *----------------------------------------------------------------------
 *
 * Tcl_ProcCmd --
 *
 *	This procedure is invoked to process the "proc" Tcl command.
 *	See the user documentation for details on what it does.
 *
 * Results:
 *	A standard Tcl result value.
 *
 * Side effects:
 *	A new procedure gets created.
 *
 *----------------------------------------------------------------------
 */

	/* ARGSUSED */
int
Tcl_ProcCmd(dummy, interp, argc, argv)
    ClientData dummy;			/* Not used. */
    Tcl_Interp *interp;			/* Current interpreter. */
    int argc;				/* Number of arguments. */
    char **argv;			/* Argument strings. */
{
    register Interp *iPtr = (Interp *) interp;
    register Proc *procPtr;
    int result, argCount, i;
    char **argArray = NULL;
    Arg *lastArgPtr;
    register Arg *argPtr = NULL;	/* Initialization not needed, but
					 * prevents compiler warning. */

    if (argc != 4) {
	Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
		" name args body\"", (char *) NULL);
	return TCL_ERROR;
    }

    procPtr = (Proc *) ckalloc(sizeof(Proc));
    procPtr->iPtr = iPtr;
    procPtr->command = (char *) ckalloc((unsigned) strlen(argv[3]) + 1);
    strcpy(procPtr->command, argv[3]);
    procPtr->argPtr = NULL;

    /*
     * Break up the argument list into argument specifiers, then process
     * each argument specifier.
     */

    result = Tcl_SplitList(interp, argv[2], &argCount, &argArray);
    if (result != TCL_OK) {
	goto procError;
    }
    lastArgPtr = NULL;
    for (i = 0; i < argCount; i++) {
	int fieldCount, nameLength, valueLength;
	char **fieldValues;

	/*
	 * Now divide the specifier up into name and default.
	 */

	result = Tcl_SplitList(interp, argArray[i], &fieldCount,
		&fieldValues);
	if (result != TCL_OK) {
	    goto procError;
	}
	if (fieldCount > 2) {
	    ckfree((char *) fieldValues);
	    Tcl_AppendResult(interp,
		    "too many fields in argument specifier \"",
		    argArray[i], "\"", (char *) NULL);
	    result = TCL_ERROR;
	    goto procError;
	}
	if ((fieldCount == 0) || (*fieldValues[0] == 0)) {
	    ckfree((char *) fieldValues);
	    Tcl_AppendResult(interp, "procedure \"", argv[1],
		    "\" has argument with no name", (char *) NULL);
	    result = TCL_ERROR;
	    goto procError;
	}
	nameLength = strlen(fieldValues[0]) + 1;
	if (fieldCount == 2) {
	    valueLength = strlen(fieldValues[1]) + 1;
	} else {
	    valueLength = 0;
	}
	argPtr = (Arg *) ckalloc((unsigned)
		(sizeof(Arg) - sizeof(argPtr->name) + nameLength
		+ valueLength));
	if (lastArgPtr == NULL) {
	    procPtr->argPtr = argPtr;
	} else {
	    lastArgPtr->nextPtr = argPtr;
	}
	lastArgPtr = argPtr;
	argPtr->nextPtr = NULL;
	strcpy(argPtr->name, fieldValues[0]);
	if (fieldCount == 2) {
	    argPtr->defValue = argPtr->name + nameLength;
	    strcpy(argPtr->defValue, fieldValues[1]);
	} else {
	    argPtr->defValue = NULL;
	}
	ckfree((char *) fieldValues);
    }

    Tcl_CreateCommand(interp, argv[1], InterpProc, (ClientData) procPtr,
	    ProcDeleteProc);
    ckfree((char *) argArray);
    return TCL_OK;

    procError:
    ckfree(procPtr->command);
    while (procPtr->argPtr != NULL) {
	argPtr = procPtr->argPtr;
	procPtr->argPtr = argPtr->nextPtr;
	ckfree((char *) argPtr);
    }
    ckfree((char *) procPtr);
    if (argArray != NULL) {
	ckfree((char *) argArray);
    }
    return result;
}

/*
 *----------------------------------------------------------------------
 *
 * TclGetFrame --
 *
 *	Given a description of a procedure frame, such as the first
 *	argument to an "uplevel" or "upvar" command, locate the
 *	call frame for the appropriate level of procedure.
 *
 * Results:
 *	The return value is -1 if an error occurred in finding the
 *	frame (in this case an error message is left in interp->result).
 *	1 is returned if string was either a number or a number preceded
 *	by "#" and it specified a valid frame.  0 is returned if string
 *	isn't one of the two things above (in this case, the lookup
 *	acts as if string were "1").  The variable pointed to by
 *	framePtrPtr is filled in with the address of the desired frame
 *	(unless an error occurs, in which case it isn't modified).
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

int
TclGetFrame(interp, string, framePtrPtr)
    Tcl_Interp *interp;		/* Interpreter in which to find frame. */
    char *string;		/* String describing frame. */
    CallFrame **framePtrPtr;	/* Store pointer to frame here (or NULL
				 * if global frame indicated). */
{
    register Interp *iPtr = (Interp *) interp;
    int level, result;
    CallFrame *framePtr;

    if (iPtr->varFramePtr == NULL) {
	iPtr->result = "already at top level";
	return -1;
    }

    /*
     * Parse string to figure out which level number to go to.
     */

    result = 1;
    if (*string == '#') {
	if (Tcl_GetInt(interp, string+1, &level) != TCL_OK) {
	    return -1;
	}
	if (level < 0) {
	    levelError:
	    Tcl_AppendResult(interp, "bad level \"", string, "\"",
		    (char *) NULL);
	    return -1;
	}
    } else if (isdigit(*string)) {
	if (Tcl_GetInt(interp, string, &level) != TCL_OK) {
	    return -1;
	}
	level = iPtr->varFramePtr->level - level;
    } else {
	level = iPtr->varFramePtr->level - 1;
	result = 0;
    }

    /*
     * Figure out which frame to use, and modify the interpreter so
     * its variables come from that frame.
     */

    if (level == 0) {
	framePtr = NULL;
    } else {
	for (framePtr = iPtr->varFramePtr; framePtr != NULL;
		framePtr = framePtr->callerVarPtr) {
	    if (framePtr->level == level) {
		break;
	    }
	}
	if (framePtr == NULL) {
	    goto levelError;
	}
    }
    *framePtrPtr = framePtr;
    return result;
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_UplevelCmd --
 *
 *	This procedure is invoked to process the "uplevel" Tcl command.
 *	See the user documentation for details on what it does.
 *
 * Results:
 *	A standard Tcl result value.
 *
 * Side effects:
 *	See the user documentation.
 *
 *----------------------------------------------------------------------
 */

	/* ARGSUSED */
int
Tcl_UplevelCmd(dummy, interp, argc, argv)
    ClientData dummy;			/* Not used. */
    Tcl_Interp *interp;			/* Current interpreter. */
    int argc;				/* Number of arguments. */
    char **argv;			/* Argument strings. */
{
    register Interp *iPtr = (Interp *) interp;
    int result;
    CallFrame *savedVarFramePtr, *framePtr;

    if (argc < 2) {
	uplevelSyntax:
	Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
		" ?level? command ?command ...?\"", (char *) NULL);
	return TCL_ERROR;
    }

    /*
     * Find the level to use for executing the command.
     */

    result = TclGetFrame(interp, argv[1], &framePtr);
    if (result == -1) {
	return TCL_ERROR;
    }
    argc -= (result+1);
    argv += (result+1);

    /*
     * Modify the interpreter state to execute in the given frame.
     */

    savedVarFramePtr = iPtr->varFramePtr;
    iPtr->varFramePtr = framePtr;

    /*
     * Execute the residual arguments as a command.
     */

    if (argc == 0) {
	goto uplevelSyntax;
    }
    if (argc == 1) {
	result = Tcl_Eval(interp, argv[0], 0, (char **) NULL);
    } else {
	char *cmd;

	cmd = Tcl_Concat(argc, argv);
	result = Tcl_Eval(interp, cmd, 0, (char **) NULL);
	ckfree(cmd);
    }
    if (result == TCL_ERROR) {
	char msg[60];
	sprintf(msg, "\n    (\"uplevel\" body line %d)", interp->errorLine);
	Tcl_AddErrorInfo(interp, msg);
    }

    /*
     * Restore the variable frame, and return.
     */

    iPtr->varFramePtr = savedVarFramePtr;
    return result;
}

/*
 *----------------------------------------------------------------------
 *
 * TclFindProc --
 *
 *	Given the name of a procedure, return a pointer to the
 *	record describing the procedure.
 *
 * Results:
 *	NULL is returned if the name doesn't correspond to any
 *	procedure.  Otherwise the return value is a pointer to
 *	the procedure's record.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

Proc *
TclFindProc(iPtr, procName)
    Interp *iPtr;		/* Interpreter in which to look. */
    char *procName;		/* Name of desired procedure. */
{
    Tcl_HashEntry *hPtr;
    Command *cmdPtr;

    hPtr = Tcl_FindHashEntry(&iPtr->commandTable, procName);
    if (hPtr == NULL) {
	return NULL;
    }
    cmdPtr = (Command *) Tcl_GetHashValue(hPtr);
    if (cmdPtr->proc != InterpProc) {
	return NULL;
    }
    return (Proc *) cmdPtr->clientData;
}

/*
 *----------------------------------------------------------------------
 *
 * TclIsProc --
 *
 *	Tells whether a command is a Tcl procedure or not.
 *
 * Results:
 *	If the given command is actuall a Tcl procedure, the
 *	return value is the address of the record describing
 *	the procedure.  Otherwise the return value is 0.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

Proc *
TclIsProc(cmdPtr)
    Command *cmdPtr;		/* Command to test. */
{
    if (cmdPtr->proc == InterpProc) {
	return (Proc *) cmdPtr->clientData;
    }
    return (Proc *) 0;
}

/*
 *----------------------------------------------------------------------
 *
 * InterpProc --
 *
 *	When a Tcl procedure gets invoked, this routine gets invoked
 *	to interpret the procedure.
 *
 * Results:
 *	A standard Tcl result value, usually TCL_OK.
 *
 * Side effects:
 *	Depends on the commands in the procedure.
 *
 *----------------------------------------------------------------------
 */

static int
InterpProc(clientData, interp, argc, argv)
    ClientData clientData;	/* Record describing procedure to be
				 * interpreted. */
    Tcl_Interp *interp;		/* Interpreter in which procedure was
				 * invoked. */
    int argc;			/* Count of number of arguments to this
				 * procedure. */
    char **argv;		/* Argument values. */
{
    register Proc *procPtr = (Proc *) clientData;
    register Arg *argPtr;
    register Interp *iPtr = (Interp *) interp;
    char **args;
    CallFrame frame;
    char *value, *end;
    int result;

    /*
     * Set up a call frame for the new procedure invocation.
     */

    iPtr = procPtr->iPtr;
    Tcl_InitHashTable(&frame.varTable, TCL_STRING_KEYS);
    if (iPtr->varFramePtr != NULL) {
	frame.level = iPtr->varFramePtr->level + 1;
    } else {
	frame.level = 1;
    }
    frame.argc = argc;
    frame.argv = argv;
    frame.callerPtr = iPtr->framePtr;
    frame.callerVarPtr = iPtr->varFramePtr;
    iPtr->framePtr = &frame;
    iPtr->varFramePtr = &frame;

    /*
     * Match the actual arguments against the procedure's formal
     * parameters to compute local variables.
     */

    for (argPtr = procPtr->argPtr, args = argv+1, argc -= 1;
	    argPtr != NULL;
	    argPtr = argPtr->nextPtr, args++, argc--) {

	/*
	 * Handle the special case of the last formal being "args".  When
	 * it occurs, assign it a list consisting of all the remaining
	 * actual arguments.
	 */

	if ((argPtr->nextPtr == NULL)
		&& (strcmp(argPtr->name, "args") == 0)) {
	    if (argc < 0) {
		argc = 0;
	    }
	    value = Tcl_Merge(argc, args);
	    Tcl_SetVar(interp, argPtr->name, value, 0);
	    ckfree(value);
	    argc = 0;
	    break;
	} else if (argc > 0) {
	    value = *args;
	} else if (argPtr->defValue != NULL) {
	    value = argPtr->defValue;
	} else {
	    Tcl_AppendResult(interp, "no value given for parameter \"",
		    argPtr->name, "\" to \"", argv[0], "\"",
		    (char *) NULL);
	    result = TCL_ERROR;
	    goto procDone;
	}
	Tcl_SetVar(interp, argPtr->name, value, 0);
    }
    if (argc > 0) {
	Tcl_AppendResult(interp, "called \"", argv[0],
		"\" with too many arguments", (char *) NULL);
	result = TCL_ERROR;
	goto procDone;
    }

    /*
     * Invoke the commands in the procedure's body.
     */

    result = Tcl_Eval(interp, procPtr->command, 0, &end);
    if (result == TCL_RETURN) {
	result = TCL_OK;
    } else if (result == TCL_ERROR) {
	char msg[100];

	/*
	 * Record information telling where the error occurred.
	 */

	sprintf(msg, "\n    (procedure \"%.50s\" line %d)", argv[0],
		iPtr->errorLine);
	Tcl_AddErrorInfo(interp, msg);
    } else if (result == TCL_BREAK) {
	iPtr->result = "invoked \"break\" outside of a loop";
	result = TCL_ERROR;
    } else if (result == TCL_CONTINUE) {
	iPtr->result = "invoked \"continue\" outside of a loop";
	result = TCL_ERROR;
    }

    /*
     * Delete the call frame for this procedure invocation (it's
     * important to remove the call frame from the interpreter
     * before deleting it, so that traces invoked during the
     * deletion don't see the partially-deleted frame).
     */

    procDone:
    iPtr->framePtr = frame.callerPtr;
    iPtr->varFramePtr = frame.callerVarPtr;
    TclDeleteVars(iPtr, &frame.varTable);
    return result;
}

/*
 *----------------------------------------------------------------------
 *
 * ProcDeleteProc --
 *
 *	This procedure is invoked just before a command procedure is
 *	removed from an interpreter.  Its job is to release all the
 *	resources allocated to the procedure.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Memory gets freed.
 *
 *----------------------------------------------------------------------
 */

static void
ProcDeleteProc(clientData)
    ClientData clientData;		/* Procedure to be deleted. */
{
    register Proc *procPtr = (Proc *) clientData;
    register Arg *argPtr;

    ckfree((char *) procPtr->command);
    for (argPtr = procPtr->argPtr; argPtr != NULL; ) {
	Arg *nextPtr = argPtr->nextPtr;

	ckfree((char *) argPtr);
	argPtr = nextPtr;
    }
    ckfree((char *) procPtr);
}
