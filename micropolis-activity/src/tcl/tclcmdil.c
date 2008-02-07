/* 
 * tclCmdIL.c --
 *
 *	This file contains the top-level command routines for most of
 *	the Tcl built-in commands whose names begin with the letters
 *	I through L.  It contains only commands in the generic core
 *	(i.e. those that don't depend much upon UNIX facilities).
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
static char rcsid[] = "$Header: /user6/ouster/tcl/RCS/tclCmdIL.c,v 1.84 91/12/06 10:42:36 ouster Exp $ SPRITE (Berkeley)";
#endif

#include "tclint.h"

/*
 * Global absolute file name:
 */

char *TCL_Library = TCL_LIBRARY;

/*
 * Forward declarations for procedures defined in this file:
 */

static int		SortCompareProc _ANSI_ARGS_((CONST VOID *first,
			    CONST VOID *second));

/*
 *----------------------------------------------------------------------
 *
 * Tcl_IfCmd --
 *
 *	This procedure is invoked to process the "if" Tcl command.
 *	See the user documentation for details on what it does.
 *
 * Results:
 *	A standard Tcl result.
 *
 * Side effects:
 *	See the user documentation.
 *
 *----------------------------------------------------------------------
 */

	/* ARGSUSED */
int
Tcl_IfCmd(dummy, interp, argc, argv)
    ClientData dummy;			/* Not used. */
    Tcl_Interp *interp;			/* Current interpreter. */
    int argc;				/* Number of arguments. */
    char **argv;			/* Argument strings. */
{
    char *condition, *ifPart, *elsePart, *cmd, *name;
    char *clause;
    int result, value;

    name = argv[0];
    if (argc < 3) {
	ifSyntax:
	Tcl_AppendResult(interp, "wrong # args: should be \"", name,
		" bool ?then? command ?else? ?command?\"", (char *) NULL);
	return TCL_ERROR;
    }
    condition = argv[1];
    argc -= 2;
    argv += 2;
    if ((**argv == 't') && (strncmp(*argv, "then", strlen(*argv)) == 0)) {
	argc--;
	argv++;
    }
    if (argc < 1) {
	goto ifSyntax;
    }
    ifPart = *argv;
    argv++;
    argc--;
    if (argc == 0) {
	elsePart = "";
    } else {
	if ((**argv == 'e') && (strncmp(*argv, "else", strlen(*argv)) == 0)) {
	    argc--;
	    argv++;
	}
	if (argc != 1) {
	    goto ifSyntax;
	}
	elsePart = *argv;
    }

    cmd = ifPart;
    clause = "\"then\" clause";
    result = Tcl_ExprBoolean(interp, condition, &value);
    if (result != TCL_OK) {
	if (result == TCL_ERROR) {
	    char msg[60];
	    sprintf(msg, "\n    (\"if\" test line %d)", interp->errorLine);
	    Tcl_AddErrorInfo(interp, msg);
	}
	return result;
    }
    if (value == 0) {
	cmd = elsePart;
	clause = "\"else\" clause";
    }
    if (*cmd == 0) {
	return TCL_OK;
    }
    result = Tcl_Eval(interp, cmd, 0, (char **) NULL);
    if (result == TCL_ERROR) {
	char msg[60];
	sprintf(msg, "\n    (%s line %d)", clause, interp->errorLine);
	Tcl_AddErrorInfo(interp, msg);
    }
    return result;
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_IncrCmd --
 *
 *	This procedure is invoked to process the "incr" Tcl command.
 *	See the user documentation for details on what it does.
 *
 * Results:
 *	A standard Tcl result.
 *
 * Side effects:
 *	See the user documentation.
 *
 *----------------------------------------------------------------------
 */

    /* ARGSUSED */
int
Tcl_IncrCmd(dummy, interp, argc, argv)
    ClientData dummy;			/* Not used. */
    Tcl_Interp *interp;			/* Current interpreter. */
    int argc;				/* Number of arguments. */
    char **argv;			/* Argument strings. */
{
    int value;
    char *oldString, *result;
    char newString[30];

    if ((argc != 2) && (argc != 3)) {
	Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
		" varName ?increment?\"", (char *) NULL);
	return TCL_ERROR;
    }

    oldString = Tcl_GetVar(interp, argv[1], TCL_LEAVE_ERR_MSG);
    if (oldString == NULL) {
	return TCL_ERROR;
    }
    if (Tcl_GetInt(interp, oldString, &value) != TCL_OK) {
	Tcl_AddErrorInfo(interp,
		"\n    (reading value of variable to increment)");
	return TCL_ERROR;
    }
    if (argc == 2) {
	value += 1;
    } else {
	int increment;

	if (Tcl_GetInt(interp, argv[2], &increment) != TCL_OK) {
	    Tcl_AddErrorInfo(interp,
		    "\n    (reading increment)");
	    return TCL_ERROR;
	}
	value += increment;
    }
    sprintf(newString, "%d", value);
    result = Tcl_SetVar(interp, argv[1], newString, TCL_LEAVE_ERR_MSG);
    if (result == NULL) {
	return TCL_ERROR;
    }
    interp->result = result;
    return TCL_OK; 
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_InfoCmd --
 *
 *	This procedure is invoked to process the "info" Tcl command.
 *	See the user documentation for details on what it does.
 *
 * Results:
 *	A standard Tcl result.
 *
 * Side effects:
 *	See the user documentation.
 *
 *----------------------------------------------------------------------
 */

	/* ARGSUSED */
int
Tcl_InfoCmd(dummy, interp, argc, argv)
    ClientData dummy;			/* Not used. */
    Tcl_Interp *interp;			/* Current interpreter. */
    int argc;				/* Number of arguments. */
    char **argv;			/* Argument strings. */
{
    register Interp *iPtr = (Interp *) interp;
    int length;
    char c;
    Arg *argPtr;
    Proc *procPtr;
    Var *varPtr;
    Command *cmdPtr;
    Tcl_HashEntry *hPtr;
    Tcl_HashSearch search;

    if (argc < 2) {
	Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
		" option ?arg arg ...?\"", (char *) NULL);
	return TCL_ERROR;
    }
    c = argv[1][0];
    length = strlen(argv[1]);
    if ((c == 'a') && (strncmp(argv[1], "args", length)) == 0) {
	if (argc != 3) {
	    Tcl_AppendResult(interp, "wrong # args: should be \"",
		    argv[0], " args procname\"", (char *) NULL);
	    return TCL_ERROR;
	}
	procPtr = TclFindProc(iPtr, argv[2]);
	if (procPtr == NULL) {
	    infoNoSuchProc:
	    Tcl_AppendResult(interp, "\"", argv[2],
		    "\" isn't a procedure", (char *) NULL);
	    return TCL_ERROR;
	}
	for (argPtr = procPtr->argPtr; argPtr != NULL;
		argPtr = argPtr->nextPtr) {
	    Tcl_AppendElement(interp, argPtr->name, 0);
	}
	return TCL_OK;
    } else if ((c == 'b') && (strncmp(argv[1], "body", length)) == 0) {
	if (argc != 3) {
	    Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
		    " body procname\"", (char *) NULL);
	    return TCL_ERROR;
	}
	procPtr = TclFindProc(iPtr, argv[2]);
	if (procPtr == NULL) {
	    goto infoNoSuchProc;
	}
	iPtr->result = procPtr->command;
	return TCL_OK;
    } else if ((c == 'c') && (strncmp(argv[1], "cmdcount", length) == 0)
	    && (length >= 2)) {
	if (argc != 2) {
	    Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
		    " cmdcount\"", (char *) NULL);
	    return TCL_ERROR;
	}
	sprintf(iPtr->result, "%d", iPtr->cmdCount);
	return TCL_OK;
    } else if ((c == 'c') && (strncmp(argv[1], "commands", length) == 0)
	    && (length >= 2)){
	if (argc > 3) {
	    Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
		    " commands [pattern]\"", (char *) NULL);
	    return TCL_ERROR;
	}
	for (hPtr = Tcl_FirstHashEntry(&iPtr->commandTable, &search);
		hPtr != NULL; hPtr = Tcl_NextHashEntry(&search)) {
	    char *name = Tcl_GetHashKey(&iPtr->commandTable, hPtr);
	    if ((argc == 3) && !Tcl_StringMatch(name, argv[2])) {
		continue;
	    }
	    Tcl_AppendElement(interp, name, 0);
	}
	return TCL_OK;
    } else if ((c == 'd') && (strncmp(argv[1], "default", length)) == 0) {
	if (argc != 5) {
	    Tcl_AppendResult(interp, "wrong # args: should be \"",
		    argv[0], " default procname arg varname\"",
		    (char *) NULL);
	    return TCL_ERROR;
	}
	procPtr = TclFindProc(iPtr, argv[2]);
	if (procPtr == NULL) {
	    goto infoNoSuchProc;
	}
	for (argPtr = procPtr->argPtr; ; argPtr = argPtr->nextPtr) {
	    if (argPtr == NULL) {
		Tcl_AppendResult(interp, "procedure \"", argv[2],
			"\" doesn't have an argument \"", argv[3],
			"\"", (char *) NULL);
		return TCL_ERROR;
	    }
	    if (strcmp(argv[3], argPtr->name) == 0) {
		if (argPtr->defValue != NULL) {
		    if (Tcl_SetVar((Tcl_Interp *) iPtr, argv[4],
			    argPtr->defValue, 0) == NULL) {
			defStoreError:
			Tcl_AppendResult(interp,
				"couldn't store default value in variable \"",
				argv[4], "\"", (char *) NULL);
			return TCL_ERROR;
		    }
		    iPtr->result = "1";
		} else {
		    if (Tcl_SetVar((Tcl_Interp *) iPtr, argv[4], "", 0)
			    == NULL) {
			goto defStoreError;
		    }
		    iPtr->result = "0";
		}
		return TCL_OK;
	    }
	}
    } else if ((c == 'e') && (strncmp(argv[1], "exists", length) == 0)) {
	char *p;
	if (argc != 3) {
	    Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
		    " exists varName\"", (char *) NULL);
	    return TCL_ERROR;
	}
	p = Tcl_GetVar((Tcl_Interp *) iPtr, argv[2], 0);

	/*
	 * The code below handles the special case where the name is for
	 * an array:  Tcl_GetVar will reject this since you can't read
	 * an array variable without an index.
	 */

	if (p == NULL) {
	    Tcl_HashEntry *hPtr;
	    Var *varPtr;

	    if (strchr(argv[2], '(') != NULL) {
		noVar:
		iPtr->result = "0";
		return TCL_OK;
	    }
	    if (iPtr->varFramePtr == NULL) {
		hPtr = Tcl_FindHashEntry(&iPtr->globalTable, argv[2]);
	    } else {
		hPtr = Tcl_FindHashEntry(&iPtr->varFramePtr->varTable, argv[2]);
	    }
	    if (hPtr == NULL) {
		goto noVar;
	    }
	    varPtr = (Var *) Tcl_GetHashValue(hPtr);
	    if (varPtr->flags & VAR_UPVAR) {
		varPtr = (Var *) Tcl_GetHashValue(varPtr->value.upvarPtr);
	    }
	    if (!(varPtr->flags & VAR_ARRAY)) {
		goto noVar;
	    }
	}
	iPtr->result = "1";
	return TCL_OK;
    } else if ((c == 'g') && (strncmp(argv[1], "globals", length) == 0)) {
	char *name;

	if (argc > 3) {
	    Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
		    " globals [pattern]\"", (char *) NULL);
	    return TCL_ERROR;
	}
	for (hPtr = Tcl_FirstHashEntry(&iPtr->globalTable, &search);
		hPtr != NULL; hPtr = Tcl_NextHashEntry(&search)) {
	    varPtr = (Var *) Tcl_GetHashValue(hPtr);
	    if (varPtr->flags & VAR_UNDEFINED) {
		continue;
	    }
	    name = Tcl_GetHashKey(&iPtr->globalTable, hPtr);
	    if ((argc == 3) && !Tcl_StringMatch(name, argv[2])) {
		continue;
	    }
	    Tcl_AppendElement(interp, name, 0);
	}
	return TCL_OK;
    } else if ((c == 'l') && (strncmp(argv[1], "level", length) == 0)
	    && (length >= 2)) {
	if (argc == 2) {
	    if (iPtr->varFramePtr == NULL) {
		iPtr->result = "0";
	    } else {
		sprintf(iPtr->result, "%d", iPtr->varFramePtr->level);
	    }
	    return TCL_OK;
	} else if (argc == 3) {
	    int level;
	    CallFrame *framePtr;

	    if (Tcl_GetInt(interp, argv[2], &level) != TCL_OK) {
		return TCL_ERROR;
	    }
	    if (level <= 0) {
		if (iPtr->varFramePtr == NULL) {
		    levelError:
		    Tcl_AppendResult(interp, "bad level \"", argv[2],
			    "\"", (char *) NULL);
		    return TCL_ERROR;
		}
		level += iPtr->varFramePtr->level;
	    }
	    for (framePtr = iPtr->varFramePtr; framePtr != NULL;
		    framePtr = framePtr->callerVarPtr) {
		if (framePtr->level == level) {
		    break;
		}
	    }
	    if (framePtr == NULL) {
		goto levelError;
	    }
	    iPtr->result = Tcl_Merge(framePtr->argc, framePtr->argv);
	    iPtr->freeProc = (Tcl_FreeProc *) free;
	    return TCL_OK;
	}
	Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
		" level [number]\"", (char *) NULL);
	return TCL_ERROR;
    } else if ((c == 'l') && (strncmp(argv[1], "library", length) == 0)
	    && (length >= 2)) {
	if (argc != 2) {
	    Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
		    " library\"", (char *) NULL);
	    return TCL_ERROR;
	}
#ifdef TCL_LIBRARY
	interp->result = TCL_Library;
	return TCL_OK;
#else
	interp->result = "there is no Tcl library at this installation";
	return TCL_ERROR;
#endif
    } else if ((c == 'l') && (strncmp(argv[1], "locals", length) == 0)
	    && (length >= 2)) {
	char *name;

	if (argc > 3) {
	    Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
		    " locals [pattern]\"", (char *) NULL);
	    return TCL_ERROR;
	}
	if (iPtr->varFramePtr == NULL) {
	    return TCL_OK;
	}
	for (hPtr = Tcl_FirstHashEntry(&iPtr->varFramePtr->varTable, &search);
		hPtr != NULL; hPtr = Tcl_NextHashEntry(&search)) {
	    varPtr = (Var *) Tcl_GetHashValue(hPtr);
	    if (varPtr->flags & (VAR_UNDEFINED|VAR_UPVAR)) {
		continue;
	    }
	    name = Tcl_GetHashKey(&iPtr->varFramePtr->varTable, hPtr);
	    if ((argc == 3) && !Tcl_StringMatch(name, argv[2])) {
		continue;
	    }
	    Tcl_AppendElement(interp, name, 0);
	}
	return TCL_OK;
    } else if ((c == 'p') && (strncmp(argv[1], "procs", length)) == 0) {
	if (argc > 3) {
	    Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
		    " procs [pattern]\"", (char *) NULL);
	    return TCL_ERROR;
	}
	for (hPtr = Tcl_FirstHashEntry(&iPtr->commandTable, &search);
		hPtr != NULL; hPtr = Tcl_NextHashEntry(&search)) {
	    char *name = Tcl_GetHashKey(&iPtr->commandTable, hPtr);

	    cmdPtr = (Command *) Tcl_GetHashValue(hPtr);
	    if (!TclIsProc(cmdPtr)) {
		continue;
	    }
	    if ((argc == 3) && !Tcl_StringMatch(name, argv[2])) {
		continue;
	    }
	    Tcl_AppendElement(interp, name, 0);
	}
	return TCL_OK;
    } else if ((c == 's') && (strncmp(argv[1], "script", length) == 0)) {
	if (argc != 2) {
	    Tcl_AppendResult(interp, "wrong # args: should be \"",
		    argv[0], " script\"", (char *) NULL);
	    return TCL_ERROR;
	}
	if (iPtr->scriptFile != NULL) {
	    interp->result = iPtr->scriptFile;
	}
	return TCL_OK;
    } else if ((c == 't') && (strncmp(argv[1], "tclversion", length) == 0)) {
	if (argc != 2) {
	    Tcl_AppendResult(interp, "wrong # args: should be \"",
		    argv[0], " tclversion\"", (char *) NULL);
	    return TCL_ERROR;
	}

	/*
	 * Note:  TCL_VERSION below is expected to be set with a "-D"
	 * switch in the Makefile.
	 */

	strcpy(iPtr->result, TCL_VERSION);
	return TCL_OK;
    } else if ((c == 'v') && (strncmp(argv[1], "vars", length)) == 0) {
	Tcl_HashTable *tablePtr;
	char *name;

	if (argc > 3) {
	    Tcl_AppendResult(interp, "wrong # args: should be \"",
		    argv[0], " vars [pattern]\"", (char *) NULL);
	    return TCL_ERROR;
	}
	if (iPtr->varFramePtr == NULL) {
	    tablePtr = &iPtr->globalTable;
	} else {
	    tablePtr = &iPtr->varFramePtr->varTable;
	}
	for (hPtr = Tcl_FirstHashEntry(tablePtr, &search);
		hPtr != NULL; hPtr = Tcl_NextHashEntry(&search)) {
	    varPtr = (Var *) Tcl_GetHashValue(hPtr);
	    if (varPtr->flags & VAR_UNDEFINED) {
		continue;
	    }
	    name = Tcl_GetHashKey(tablePtr, hPtr);
	    if ((argc == 3) && !Tcl_StringMatch(name, argv[2])) {
		continue;
	    }
	    Tcl_AppendElement(interp, name, 0);
	}
	return TCL_OK;
    } else {
	Tcl_AppendResult(interp, "bad option \"", argv[1],
		"\": should be args, body, commands, cmdcount, default, ",
		"exists, globals, level, library, locals, procs, ",
		"script, tclversion, or vars",
		(char *) NULL);
	return TCL_ERROR;
    }
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_JoinCmd --
 *
 *	This procedure is invoked to process the "join" Tcl command.
 *	See the user documentation for details on what it does.
 *
 * Results:
 *	A standard Tcl result.
 *
 * Side effects:
 *	See the user documentation.
 *
 *----------------------------------------------------------------------
 */

	/* ARGSUSED */
int
Tcl_JoinCmd(dummy, interp, argc, argv)
    ClientData dummy;			/* Not used. */
    Tcl_Interp *interp;			/* Current interpreter. */
    int argc;				/* Number of arguments. */
    char **argv;			/* Argument strings. */
{
    char *joinString;
    char **listArgv;
    int listArgc, i;

    if (argc == 2) {
	joinString = " ";
    } else if (argc == 3) {
	joinString = argv[2];
    } else {
	Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
		" list ?joinString?\"", (char *) NULL);
	return TCL_ERROR;
    }

    if (Tcl_SplitList(interp, argv[1], &listArgc, &listArgv) != TCL_OK) {
	return TCL_ERROR;
    }
    for (i = 0; i < listArgc; i++) {
	if (i == 0) {
	    Tcl_AppendResult(interp, listArgv[0], (char *) NULL);
	} else  {
	    Tcl_AppendResult(interp, joinString, listArgv[i], (char *) NULL);
	}
    }
    ckfree((char *) listArgv);
    return TCL_OK;
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_LindexCmd --
 *
 *	This procedure is invoked to process the "lindex" Tcl command.
 *	See the user documentation for details on what it does.
 *
 * Results:
 *	A standard Tcl result.
 *
 * Side effects:
 *	See the user documentation.
 *
 *----------------------------------------------------------------------
 */

    /* ARGSUSED */
int
Tcl_LindexCmd(dummy, interp, argc, argv)
    ClientData dummy;			/* Not used. */
    Tcl_Interp *interp;			/* Current interpreter. */
    int argc;				/* Number of arguments. */
    char **argv;			/* Argument strings. */
{
    char *p, *element;
    int index, size, parenthesized, result;

    if (argc != 3) {
	Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
		" list index\"", (char *) NULL);
	return TCL_ERROR;
    }
    if (Tcl_GetInt(interp, argv[2], &index) != TCL_OK) {
	return TCL_ERROR;
    }
    if (index < 0) {
	return TCL_OK;
    }
    for (p = argv[1] ; index >= 0; index--) {
	result = TclFindElement(interp, p, &element, &p, &size,
		&parenthesized);
	if (result != TCL_OK) {
	    return result;
	}
    }
    if (size == 0) {
	return TCL_OK;
    }
    if (size >= TCL_RESULT_SIZE) {
	interp->result = (char *) ckalloc((unsigned) size+1);
	interp->freeProc = (Tcl_FreeProc *) free;
    }
    if (parenthesized) {
	memcpy((VOID *) interp->result, (VOID *) element, size);
	interp->result[size] = 0;
    } else {
	TclCopyAndCollapse(size, element, interp->result);
    }
    return TCL_OK;
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_LinsertCmd --
 *
 *	This procedure is invoked to process the "linsert" Tcl command.
 *	See the user documentation for details on what it does.
 *
 * Results:
 *	A standard Tcl result.
 *
 * Side effects:
 *	See the user documentation.
 *
 *----------------------------------------------------------------------
 */

	/* ARGSUSED */
int
Tcl_LinsertCmd(dummy, interp, argc, argv)
    ClientData dummy;			/* Not used. */
    Tcl_Interp *interp;			/* Current interpreter. */
    int argc;				/* Number of arguments. */
    char **argv;			/* Argument strings. */
{
    char *p, *element, savedChar;
    int i, index, count, result, size;

    if (argc < 4) {
	Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
		" list index element ?element ...?\"", (char *) NULL);
	return TCL_ERROR;
    }
    if (Tcl_GetInt(interp, argv[2], &index) != TCL_OK) {
	return TCL_ERROR;
    }

    /*
     * Skip over the first "index" elements of the list, then add
     * all of those elements to the result.
     */

    size = 0;
    element = argv[1];
    for (count = 0, p = argv[1]; (count < index) && (*p != 0); count++) {
	result = TclFindElement(interp, p, &element, &p, &size, (int *) NULL);
	if (result != TCL_OK) {
	    return result;
	}
    }
    if (*p == 0) {
	Tcl_AppendResult(interp, argv[1], (char *) NULL);
    } else {
	char *end;

	end = element+size;
	if (element != argv[1]) {
	    while ((*end != 0) && !isspace(*end)) {
		end++;
	    }
	}
	savedChar = *end;
	*end = 0;
	Tcl_AppendResult(interp, argv[1], (char *) NULL);
	*end = savedChar;
    }

    /*
     * Add the new list elements.
     */

    for (i = 3; i < argc; i++) {
	Tcl_AppendElement(interp, argv[i], 0);
    }

    /*
     * Append the remainder of the original list.
     */

    if (*p != 0) {
	Tcl_AppendResult(interp, " ", p, (char *) NULL);
    }
    return TCL_OK;
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_ListCmd --
 *
 *	This procedure is invoked to process the "list" Tcl command.
 *	See the user documentation for details on what it does.
 *
 * Results:
 *	A standard Tcl result.
 *
 * Side effects:
 *	See the user documentation.
 *
 *----------------------------------------------------------------------
 */

	/* ARGSUSED */
int
Tcl_ListCmd(dummy, interp, argc, argv)
    ClientData dummy;			/* Not used. */
    Tcl_Interp *interp;			/* Current interpreter. */
    int argc;				/* Number of arguments. */
    char **argv;			/* Argument strings. */
{
    if (argc < 2) {
	Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
		" arg ?arg ...?\"", (char *) NULL);
	return TCL_ERROR;
    }
    interp->result = Tcl_Merge(argc-1, argv+1);
    interp->freeProc = (Tcl_FreeProc *) free;
    return TCL_OK;
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_LlengthCmd --
 *
 *	This procedure is invoked to process the "llength" Tcl command.
 *	See the user documentation for details on what it does.
 *
 * Results:
 *	A standard Tcl result.
 *
 * Side effects:
 *	See the user documentation.
 *
 *----------------------------------------------------------------------
 */

	/* ARGSUSED */
int
Tcl_LlengthCmd(dummy, interp, argc, argv)
    ClientData dummy;			/* Not used. */
    Tcl_Interp *interp;			/* Current interpreter. */
    int argc;				/* Number of arguments. */
    char **argv;			/* Argument strings. */
{
    int count, result;
    char *element, *p;

    if (argc != 2) {
	Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
		" list\"", (char *) NULL);
	return TCL_ERROR;
    }
    for (count = 0, p = argv[1]; *p != 0 ; count++) {
	result = TclFindElement(interp, p, &element, &p, (int *) NULL,
		(int *) NULL);
	if (result != TCL_OK) {
	    return result;
	}
	if (*element == 0) {
	    break;
	}
    }
    sprintf(interp->result, "%d", count);
    return TCL_OK;
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_LrangeCmd --
 *
 *	This procedure is invoked to process the "lrange" Tcl command.
 *	See the user documentation for details on what it does.
 *
 * Results:
 *	A standard Tcl result.
 *
 * Side effects:
 *	See the user documentation.
 *
 *----------------------------------------------------------------------
 */

	/* ARGSUSED */
int
Tcl_LrangeCmd(notUsed, interp, argc, argv)
    ClientData notUsed;			/* Not used. */
    Tcl_Interp *interp;			/* Current interpreter. */
    int argc;				/* Number of arguments. */
    char **argv;			/* Argument strings. */
{
    int first, last, result;
    char *begin, *end, c, *dummy;
    int count;

    if (argc != 4) {
	Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
		" list first last\"", (char *) NULL);
	return TCL_ERROR;
    }
    if (Tcl_GetInt(interp, argv[2], &first) != TCL_OK) {
	return TCL_ERROR;
    }
    if (first < 0) {
	first = 0;
    }
    if ((*argv[3] == 'e') && (strncmp(argv[3], "end", strlen(argv[3])) == 0)) {
	last = 1000000;
    } else {
	if (Tcl_GetInt(interp, argv[3], &last) != TCL_OK) {
	    Tcl_ResetResult(interp);
	    Tcl_AppendResult(interp,
		    "expected integer or \"end\" but got \"",
		    argv[3], "\"", (char *) NULL);
	    return TCL_ERROR;
	}
    }
    if (first > last) {
	return TCL_OK;
    }

    /*
     * Extract a range of fields.
     */

    for (count = 0, begin = argv[1]; count < first; count++) {
	result = TclFindElement(interp, begin, &dummy, &begin, (int *) NULL,
		(int *) NULL);
	if (result != TCL_OK) {
	    return result;
	}
	if (*begin == 0) {
	    break;
	}
    }
    for (count = first, end = begin; (count <= last) && (*end != 0);
	    count++) {
	result = TclFindElement(interp, end, &dummy, &end, (int *) NULL,
		(int *) NULL);
	if (result != TCL_OK) {
	    return result;
	}
    }

    /*
     * Chop off trailing spaces.
     */

    while (isspace(end[-1])) {
	end--;
    }
    c = *end;
    *end = 0;
    Tcl_SetResult(interp, begin, TCL_VOLATILE);
    *end = c;
    return TCL_OK;
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_LreplaceCmd --
 *
 *	This procedure is invoked to process the "lreplace" Tcl command.
 *	See the user documentation for details on what it does.
 *
 * Results:
 *	A standard Tcl result.
 *
 * Side effects:
 *	See the user documentation.
 *
 *----------------------------------------------------------------------
 */

	/* ARGSUSED */
int
Tcl_LreplaceCmd(notUsed, interp, argc, argv)
    ClientData notUsed;			/* Not used. */
    Tcl_Interp *interp;			/* Current interpreter. */
    int argc;				/* Number of arguments. */
    char **argv;			/* Argument strings. */
{
    char *p1, *p2, *element, savedChar, *dummy;
    int i, first, last, count, result, size;

    if (argc < 4) {
	Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
		" list first last ?element element ...?\"", (char *) NULL);
	return TCL_ERROR;
    }
    if (Tcl_GetInt(interp, argv[2], &first) != TCL_OK) {
	return TCL_ERROR;
    }
    if (TclGetListIndex(interp, argv[3], &last) != TCL_OK) {
	return TCL_ERROR;
    }
    if (first < 0) {
	first = 0;
    }
    if (last < 0) {
	last = 0;
    }
    if (first > last) {
	Tcl_AppendResult(interp, "first index must not be greater than second",
		(char *) NULL);
	return TCL_ERROR;
    }

    /*
     * Skip over the elements of the list before "first".
     */

    size = 0;
    element = argv[1];
    for (count = 0, p1 = argv[1]; (count < first) && (*p1 != 0); count++) {
	result = TclFindElement(interp, p1, &element, &p1, &size,
				(int *) NULL);
	if (result != TCL_OK) {
	    return result;
	}
    }
    if (*p1 == 0) {
	Tcl_AppendResult(interp, "list doesn't contain element ",
		argv[2], (char *) NULL);
	return TCL_ERROR;
    }

    /*
     * Skip over the elements of the list up through "last".
     */

    for (p2 = p1 ; (count <= last) && (*p2 != 0); count++) {
	result = TclFindElement(interp, p2, &dummy, &p2, (int *) NULL,
		(int *) NULL);
	if (result != TCL_OK) {
	    return result;
	}
    }

    /*
     * Add the elements before "first" to the result.  Be sure to
     * include quote or brace characters that might terminate the
     * last of these elements.
     */

    p1 = element+size;
    if (element != argv[1]) {
	while ((*p1 != 0) && !isspace(*p1)) {
	    p1++;
	}
    }
    savedChar = *p1;
    *p1 = 0;
    Tcl_AppendResult(interp, argv[1], (char *) NULL);
    *p1 = savedChar;

    /*
     * Add the new list elements.
     */

    for (i = 4; i < argc; i++) {
	Tcl_AppendElement(interp, argv[i], 0);
    }

    /*
     * Append the remainder of the original list.
     */

    if (*p2 != 0) {
	if (*interp->result == 0) {
	    Tcl_SetResult(interp, p2, TCL_VOLATILE);
	} else {
	    Tcl_AppendResult(interp, " ", p2, (char *) NULL);
	}
    }
    return TCL_OK;
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_LsearchCmd --
 *
 *	This procedure is invoked to process the "lsearch" Tcl command.
 *	See the user documentation for details on what it does.
 *
 * Results:
 *	A standard Tcl result.
 *
 * Side effects:
 *	See the user documentation.
 *
 *----------------------------------------------------------------------
 */

	/* ARGSUSED */
int
Tcl_LsearchCmd(notUsed, interp, argc, argv)
    ClientData notUsed;			/* Not used. */
    Tcl_Interp *interp;			/* Current interpreter. */
    int argc;				/* Number of arguments. */
    char **argv;			/* Argument strings. */
{
    int listArgc;
    char **listArgv;
    int i, match;

    if (argc != 3) {
	Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
		" list pattern\"", (char *) NULL);
	return TCL_ERROR;
    }
    if (Tcl_SplitList(interp, argv[1], &listArgc, &listArgv) != TCL_OK) {
	return TCL_ERROR;
    }
    match = -1;
    for (i = 0; i < listArgc; i++) {
	if (Tcl_StringMatch(listArgv[i], argv[2])) {
	    match = i;
	    break;
	}
    }
    sprintf(interp->result, "%d", match);
    ckfree((char *) listArgv);
    return TCL_OK;
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_LsortCmd --
 *
 *	This procedure is invoked to process the "lsort" Tcl command.
 *	See the user documentation for details on what it does.
 *
 * Results:
 *	A standard Tcl result.
 *
 * Side effects:
 *	See the user documentation.
 *
 *----------------------------------------------------------------------
 */

	/* ARGSUSED */
int
Tcl_LsortCmd(notUsed, interp, argc, argv)
    ClientData notUsed;			/* Not used. */
    Tcl_Interp *interp;			/* Current interpreter. */
    int argc;				/* Number of arguments. */
    char **argv;			/* Argument strings. */
{
    int listArgc;
    char **listArgv;

    if (argc != 2) {
	Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
		" list\"", (char *) NULL);
	return TCL_ERROR;
    }
    if (Tcl_SplitList(interp, argv[1], &listArgc, &listArgv) != TCL_OK) {
	return TCL_ERROR;
    }
    qsort((VOID *) listArgv, listArgc, sizeof (char *), SortCompareProc);
    interp->result = Tcl_Merge(listArgc, listArgv);
    interp->freeProc = (Tcl_FreeProc *) free;
    ckfree((char *) listArgv);
    return TCL_OK;
}

/*
 * The procedure below is called back by qsort to determine
 * the proper ordering between two elements.
 */

static int
SortCompareProc(first, second)
    CONST VOID *first, *second;		/* Elements to be compared. */
{
    return strcmp(*((char **) first), *((char **) second));
}
