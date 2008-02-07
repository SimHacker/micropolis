/* 
 * tkCmds.c --
 *
 *	This file contains a collection of Tk-related Tcl commands
 *	that didn't fit in any particular file of the toolkit.
 *
 * Copyright 1990-1992 Regents of the University of California
 * Permission to use, copy, modify, and distribute this
 * software and its documentation for any purpose and without
 * fee is hereby granted, provided that the above copyright
 * notice appear in all copies.  The University of California
 * makes no representations about the suitability of this
 * software for any purpose.  It is provided "as is" without
 * express or implied warranty.
 */

#ifndef lint
static char rcsid[] = "$Header: /user6/ouster/wish/RCS/tkCmds.c,v 1.32 92/06/03 14:21:14 ouster Exp $ SPRITE (Berkeley)";
#endif /* not lint */

#include "tkconfig.h"
#include "tkint.h"

/*
 * The data structure below is used by the "after" command to remember
 * the command to be executed later.
 */

typedef struct {
    Tcl_Interp *interp;		/* Interpreter in which to execute command. */
    char *command;		/* Command to execute.  Malloc'ed, so must
				 * be freed when structure is deallocated. 
				 * NULL means nothing to execute. */
    int *donePtr;		/* If non-NULL indicates address of word to
				 * set to 1 when command has finally been
				 * executed. */
} AfterInfo;

/*
 * Forward declarations for procedures defined later in this file:
 */

static void		AfterProc _ANSI_ARGS_((ClientData clientData));
static char *		WaitVariableProc _ANSI_ARGS_((ClientData clientData,
			    Tcl_Interp *interp, char *name1, char *name2,
			    int flags));
static void		WaitWindowProc _ANSI_ARGS_((ClientData clientData,
			    XEvent *eventPtr));

/*
 *----------------------------------------------------------------------
 *
 * Tk_AfterCmd --
 *
 *	This procedure is invoked to process the "after" Tcl command.
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
Tk_AfterCmd(clientData, interp, argc, argv)
    ClientData clientData;	/* Main window associated with
				 * interpreter.  Not used.*/
    Tcl_Interp *interp;		/* Current interpreter. */
    int argc;			/* Number of arguments. */
    char **argv;		/* Argument strings. */
{
    int ms;
    AfterInfo *afterPtr;
    int done;

    if (argc < 2) {
	Tcl_AppendResult(interp, "wrong # args: should be \"",
		argv[0], " milliseconds ?command? ?arg arg ...?\"",
		(char *) NULL);
	return TCL_ERROR;
    }

    if ((Tcl_GetInt(interp, argv[1], &ms) != TCL_OK) || (ms <= 0)) {
	Tcl_AppendResult(interp, "bad milliseconds value \"",
		argv[1], "\"", (char *) NULL);
	return TCL_ERROR;
    }
    afterPtr = (AfterInfo *) ckalloc((unsigned) (sizeof(AfterInfo)));
    afterPtr->interp = interp;
    if (argc == 2) {
	afterPtr->command = (char *) NULL;
	done = 0;
	afterPtr->donePtr = &done;
    } else if (argc == 3) {
	afterPtr->command = (char *) ckalloc((unsigned) (strlen(argv[2]) + 1));
	strcpy(afterPtr->command, argv[2]);
	afterPtr->donePtr = (int *) NULL;
    } else {
	afterPtr->command = Tcl_Concat(argc-2, argv+2);
	afterPtr->donePtr = (int *) NULL;
    }
    Tk_CreateTimerHandler(ms, AfterProc, (ClientData) afterPtr);
    if (argc == 2) {
	while (!done) {
	    Tk_DoOneEvent(0);
	}
    }

    /*
     * Must reset interpreter result because it could have changed as
     * part of events processed by Tk_DoOneEvent.
     */

    Tcl_ResetResult(interp);
    return TCL_OK;
}

/*
 *----------------------------------------------------------------------
 *
 * AfterProc --
 *
 *	Timer callback to execute commands registered with the
 *	"after" command.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Executes whatever command was specified.  If the command
 *	returns an error, then the command "tkerror" is invoked
 *	to process the error;  if tkerror fails then information
 *	about the error is output on stderr.
 *
 *----------------------------------------------------------------------
 */

static void
AfterProc(clientData)
    ClientData clientData;	/* Describes command to execute. */
{
    AfterInfo *afterPtr = (AfterInfo *) clientData;
    int result;

    if (afterPtr->command != NULL) {
	result = Tcl_GlobalEval(afterPtr->interp, afterPtr->command);
	if (result != TCL_OK) {
	    TkBindError(afterPtr->interp);
	}
	ckfree(afterPtr->command);
    }
    if (afterPtr->donePtr != NULL) {
	*afterPtr->donePtr = 1;
    }
    ckfree((char *) afterPtr);
}

/*
 *----------------------------------------------------------------------
 *
 * Tk_BindCmd --
 *
 *	This procedure is invoked to process the "bind" Tcl command.
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

int
Tk_BindCmd(clientData, interp, argc, argv)
    ClientData clientData;		/* Main window associated with
				 * interpreter. */
    Tcl_Interp *interp;		/* Current interpreter. */
    int argc;			/* Number of arguments. */
    char **argv;		/* Argument strings. */
{
    Tk_Window tkwin = (Tk_Window) clientData;
    TkWindow *winPtr;
    ClientData object;

    if ((argc < 2) || (argc > 4)) {
	Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
		" window ?pattern? ?command?\"", (char *) NULL);
	return TCL_ERROR;
    }
    if (argv[1][0] == '.') {
	winPtr = (TkWindow *) Tk_NameToWindow(interp, argv[1], tkwin);
	if (winPtr == NULL) {
	    return TCL_ERROR;
	}
	object = (ClientData) winPtr->pathName;
    } else {
	winPtr = (TkWindow *) clientData;
	object = (ClientData) Tk_GetUid(argv[1]);
    }

    if (argc == 4) {
	int append = 0;
	unsigned long mask;

	if (argv[3][0] == 0) {
	    return Tk_DeleteBinding(interp, winPtr->mainPtr->bindingTable,
		    object, argv[2]);
	}
	if (argv[3][0] == '+') {
	    argv[3]++;
	    append = 1;
	}
	mask = Tk_CreateBinding(interp, winPtr->mainPtr->bindingTable,
		object, argv[2], argv[3], append);
	if (mask == 0) {
	    return TCL_ERROR;
	}
    } else if (argc == 3) {
	char *command;

	command = Tk_GetBinding(interp, winPtr->mainPtr->bindingTable,
		object, argv[2]);
	if (command == NULL) {
	    Tcl_ResetResult(interp);
	    return TCL_OK;
	}
	interp->result = command;
    } else {
	Tk_GetAllBindings(interp, winPtr->mainPtr->bindingTable, object);
    }
    return TCL_OK;
}

/*
 *----------------------------------------------------------------------
 *
 * TkBindEventProc --
 *
 *	This procedure is invoked by Tk_HandleEvent for each event;  it
 *	causes any appropriate bindings for that event to be invoked.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Depends on what bindings have been established with the "bind"
 *	command.
 *
 *----------------------------------------------------------------------
 */

void
TkBindEventProc(winPtr, eventPtr)
    TkWindow *winPtr;			/* Pointer to info about window. */
    XEvent *eventPtr;			/* Information about event. */
{
    ClientData objects[3];
    static Tk_Uid allUid = NULL;

    if ((winPtr->mainPtr == NULL) || (winPtr->mainPtr->bindingTable == NULL)) {
	return;
    }
    objects[0] = (ClientData) winPtr->pathName;
    objects[1] = (ClientData) winPtr->classUid;
    if (allUid == NULL) {
	allUid = Tk_GetUid("all");
    }
    objects[2] = (ClientData) allUid;
    Tk_BindEvent(winPtr->mainPtr->bindingTable, eventPtr,
	    (Tk_Window) winPtr, 3, objects);
}

/*
 *----------------------------------------------------------------------
 *
 * Tk_DestroyCmd --
 *
 *	This procedure is invoked to process the "destroy" Tcl command.
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

int
Tk_DestroyCmd(clientData, interp, argc, argv)
    ClientData clientData;		/* Main window associated with
				 * interpreter. */
    Tcl_Interp *interp;		/* Current interpreter. */
    int argc;			/* Number of arguments. */
    char **argv;		/* Argument strings. */
{
    Tk_Window window;
    Tk_Window tkwin = (Tk_Window) clientData;

    if (argc != 2) {
	Tcl_AppendResult(interp, "wrong # args: should be \"",
		argv[0], " pathName\"", (char *) NULL);
	return TCL_ERROR;
    }

    window = Tk_NameToWindow(interp, argv[1], tkwin);
    if (window == NULL) {
	return TCL_ERROR;
    }
    Tk_DestroyWindow(window);
    return TCL_OK;
}

/*
 *----------------------------------------------------------------------
 *
 * Tk_UpdateCmd --
 *
 *	This procedure is invoked to process the "update" Tcl command.
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
Tk_UpdateCmd(clientData, interp, argc, argv)
    ClientData clientData;	/* Main window associated with
				 * interpreter. */
    Tcl_Interp *interp;		/* Current interpreter. */
    int argc;			/* Number of arguments. */
    char **argv;		/* Argument strings. */
{
    Tk_Window tkwin = (Tk_Window) clientData;
    int flags;

    if (argc == 1) {
	flags = TK_DONT_WAIT;
    } else if (argc == 2) {
	if (strncmp(argv[1], "idletasks", strlen(argv[1])) != 0) {
	    Tcl_AppendResult(interp, "bad argument \"", argv[1],
		    "\": must be idletasks", (char *) NULL);
	    return TCL_ERROR;
	}
	flags = TK_IDLE_EVENTS;
    } else {
	Tcl_AppendResult(interp, "wrong # args: should be \"",
		argv[0], " ?idletasks?\"", (char *) NULL);
	return TCL_ERROR;
    }

    /*
     * Handle all pending events, sync the display, and repeat over
     * and over again until all pending events have been handled.
     */

    while (1) {
	while (Tk_DoOneEvent(flags) != 0) {
	    /* Empty loop body */
	}
	XSync(Tk_Display(tkwin), False);
	if (Tk_DoOneEvent(flags) == 0) {
	    break;
	}
    }

    /*
     * Must clear the interpreter's result because event handlers could
     * have executed commands.
     */

    Tcl_ResetResult(interp);
    return TCL_OK;
}

/*
 *----------------------------------------------------------------------
 *
 * Tk_TkwaitCmd --
 *
 *	This procedure is invoked to process the "wait" Tcl command.
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
Tk_TkwaitCmd(clientData, interp, argc, argv)
    ClientData clientData;	/* Main window associated with
				 * interpreter. */
    Tcl_Interp *interp;		/* Current interpreter. */
    int argc;			/* Number of arguments. */
    char **argv;		/* Argument strings. */
{
    Tk_Window tkwin = (Tk_Window) clientData;
    int c, length;
    int done;

    if (argc != 3) {
	Tcl_AppendResult(interp, "wrong # args: should be \"",
		argv[0], " variable|window name\"", (char *) NULL);
	return TCL_ERROR;
    }
    c = argv[1][0];
    length = strlen(argv[1]);
    if ((c == 'v') && (strncmp(argv[1], "variable", length) == 0)) {
	Tcl_TraceVar(interp, argv[2],
		TCL_GLOBAL_ONLY|TCL_TRACE_WRITES|TCL_TRACE_UNSETS,
		WaitVariableProc, (ClientData) &done);
	done = 0;
	while (!done) {
	    Tk_DoOneEvent(0);
	}
	Tcl_UntraceVar(interp, argv[2],
		TCL_GLOBAL_ONLY|TCL_TRACE_WRITES|TCL_TRACE_UNSETS,
		WaitVariableProc, (ClientData) &done);
    } else if ((c == 'w') && (strncmp(argv[1], "window", length) == 0)) {
	Tk_Window window;

	window = Tk_NameToWindow(interp, argv[2], tkwin);
	if (window == NULL) {
	    return TCL_ERROR;
	}
	Tk_CreateEventHandler(window, StructureNotifyMask,
	    WaitWindowProc, (ClientData) &done);
	done = 0;
	while (!done) {
	    Tk_DoOneEvent(0);
	}
	Tk_DeleteEventHandler(window, StructureNotifyMask,
	    WaitWindowProc, (ClientData) &done);
    } else {
	Tcl_AppendResult(interp, "bad option \"", argv[1],
		"\": must be variable or window", (char *) NULL);
	return TCL_ERROR;
    }

    /*
     * Clear out the interpreter's result, since it may have been set
     * by event handlers.
     */

    Tcl_ResetResult(interp);
    return TCL_OK;
}

	/* ARGSUSED */
static char *
WaitVariableProc(clientData, interp, name1, name2, flags)
    ClientData clientData;	/* Pointer to integer to set to 1. */
    Tcl_Interp *interp;		/* Interpreter containing variable. */
    char *name1;		/* Name of variable. */
    char *name2;		/* Second part of variable name. */
    int flags;			/* Information about what happened. */
{
    int *donePtr = (int *) clientData;

    *donePtr = 1;
    return (char *) NULL;
}

static void
WaitWindowProc(clientData, eventPtr)
    ClientData clientData;	/* Pointer to integer to set to 1. */
    XEvent *eventPtr;		/* Information about event. */
{
    int *donePtr = (int *) clientData;

    if (eventPtr->type == DestroyNotify) {
	*donePtr = 1;
    }
}

/*
 *----------------------------------------------------------------------
 *
 * Tk_WinfoCmd --
 *
 *	This procedure is invoked to process the "winfo" Tcl command.
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

int
Tk_WinfoCmd(clientData, interp, argc, argv)
    ClientData clientData;	/* Main window associated with
				 * interpreter. */
    Tcl_Interp *interp;		/* Current interpreter. */
    int argc;			/* Number of arguments. */
    char **argv;		/* Argument strings. */
{
    Tk_Window tkwin = (Tk_Window) clientData;
    int length;
    char c, *argName;
    Tk_Window window;
    register TkWindow *winPtr;

#define SETUP(name) \
    if (argc != 3) {\
	argName = name; \
	goto wrongArgs; \
    } \
    window = Tk_NameToWindow(interp, argv[2], tkwin); \
    if (window == NULL) { \
	return TCL_ERROR; \
    }

    if (argc < 2) {
	Tcl_AppendResult(interp, "wrong # args: should be \"",
		argv[0], " option ?arg?\"", (char *) NULL);
	return TCL_ERROR;
    }
    c = argv[1][0];
    length = strlen(argv[1]);
    if ((c == 'a') && (strcmp(argv[1], "atom") == 0)) {
	if (argc != 3) {
	    Tcl_AppendResult(interp, "wrong # args: should be \"",
		    argv[0], " atom name\"", (char *) NULL);
	    return TCL_ERROR;
	}
	sprintf(interp->result, "%d", Tk_InternAtom(tkwin, argv[2]));
    } else if ((c == 'a') && (strncmp(argv[1], "atomname", length) == 0)
	    && (length >= 5)) {
	Atom atom;
	char *name;

	if (argc != 3) {
	    Tcl_AppendResult(interp, "wrong # args: should be \"",
		    argv[0], " atomname id\"", (char *) NULL);
	    return TCL_ERROR;
	}
	if (Tcl_GetInt(interp, argv[2], (int *) &atom) != TCL_OK) {
	    return TCL_ERROR;
	}
	name = Tk_GetAtomName(tkwin, atom);
	if (strcmp(name, "?bad atom?") == 0) {
	    Tcl_AppendResult(interp, "no atom exists with id \"",
		    argv[2], "\"", (char *) NULL);
	    return TCL_ERROR;
	}
	interp->result = name;
    } else if ((c == 'c') && (strncmp(argv[1], "children", length) == 0)
	    && (length >= 2)) {
	char *separator, *childName;

	SETUP("children");
	separator = "";
	for (winPtr = ((TkWindow *) window)->childList; winPtr != NULL;
		winPtr = winPtr->nextPtr) {
	    childName = Tcl_Merge(1, &winPtr->pathName);
	    Tcl_AppendResult(interp, separator, childName, (char *) NULL);
	    ckfree(childName);
	    separator = " ";
	}
    } else if ((c == 'c') && (strncmp(argv[1], "class", length) == 0)
	    && (length >= 2)) {
	SETUP("class");
	interp->result = Tk_Class(window);
    } else if ((c == 'c') && (strncmp(argv[1], "containing", length) == 0)
	    && (length >= 2)) {
	int rootX, rootY;

	if (argc != 4) {
	    Tcl_AppendResult(interp, "wrong # args: should be \"",
		    argv[0], " containing rootX rootY\"", (char *) NULL);
	    return TCL_ERROR;
	}
	if ((Tk_GetPixels(interp, tkwin, argv[2], &rootX) != TCL_OK)
		|| (Tk_GetPixels(interp, tkwin, argv[3], &rootY) != TCL_OK)) {
	    return TCL_ERROR;
	}
	window = Tk_CoordsToWindow(rootX, rootY, tkwin);
	if (window != NULL) {
	    interp->result = Tk_PathName(window);
	}
    } else if ((c == 'f') && (strncmp(argv[1], "fpixels", length) == 0)
	    && (length >= 2)) {
	double mm, pixels;

	if (argc != 4) {
	    Tcl_AppendResult(interp, "wrong # args: should be \"",
		    argv[0], " fpixels window number\"", (char *) NULL);
	    return TCL_ERROR;
	}
	window = Tk_NameToWindow(interp, argv[2], tkwin);
	if (window == NULL) {
	    return TCL_ERROR;
	}
	if (Tk_GetScreenMM(interp, window, argv[3], &mm) != TCL_OK) {
	    return TCL_ERROR;
	}
	pixels = mm * WidthOfScreen(Tk_Screen(window))
		/ WidthMMOfScreen(Tk_Screen(window));
	sprintf(interp->result, "%g", pixels);
    } else if ((c == 'g') && (strncmp(argv[1], "geometry", length) == 0)) {
	SETUP("geometry");
	sprintf(interp->result, "%dx%d+%d+%d", Tk_Width(window),
		Tk_Height(window), Tk_X(window), Tk_Y(window));
    } else if ((c == 'h') && (strncmp(argv[1], "height", length) == 0)) {
	SETUP("height");
	sprintf(interp->result, "%d", Tk_Height(window));
    } else if ((c == 'i') && (strcmp(argv[1], "id") == 0)) {
	SETUP("id");
	sprintf(interp->result, "0x%x", Tk_WindowId(window));
    } else if ((c == 'i') && (strncmp(argv[1], "interps", length) == 0)
	    && (length >= 2)) {
	if (argc != 2) {
	    Tcl_AppendResult(interp, "wrong # args:  should be \"",
		    argv[1], " interps\"", (char *) NULL);
	    return TCL_ERROR;
	}
	return TkGetInterpNames(interp, tkwin);
    } else if ((c == 'i') && (strncmp(argv[1], "ismapped", length) == 0)
	    && (length >= 2)) {
	SETUP("ismapped");
	interp->result = Tk_IsMapped(window) ? "1" : "0";
    } else if ((c == 'n') && (strncmp(argv[1], "name", length) == 0)) {
	SETUP("geometry");
	interp->result = Tk_Name(window);
    } else if ((c == 'p') && (strncmp(argv[1], "parent", length) == 0)) {
	SETUP("geometry");
	winPtr = (TkWindow *) window;
	if (winPtr->parentPtr != NULL) {
	    interp->result = winPtr->parentPtr->pathName;
	}
    } else if ((c == 'p') && (strncmp(argv[1], "pathname", length) == 0)
	    && (length >= 2)) {
	Window id;

	if (argc != 3) {
	    argName = "pathname";
	    goto wrongArgs;
	}
	if (Tcl_GetInt(interp, argv[2], (int *) &id) != TCL_OK) {
	    return TCL_ERROR;
	}
	if ((XFindContext(Tk_Display(tkwin), id, tkWindowContext,
		(void *) &window) != 0) || (((TkWindow *) window)->mainPtr
		!= ((TkWindow *) tkwin)->mainPtr)) {
	    Tcl_AppendResult(interp, "window id \"", argv[2],
		    "\" doesn't exist in this application", (char *) NULL);
	    return TCL_ERROR;
	}
	interp->result = Tk_PathName(window);
    } else if ((c == 'p') && (strncmp(argv[1], "pixels", length) == 0)
	    && (length >= 2)) {
	int pixels;

	if (argc != 4) {
	    Tcl_AppendResult(interp, "wrong # args: should be \"",
		    argv[0], " pixels window number\"", (char *) NULL);
	    return TCL_ERROR;
	}
	window = Tk_NameToWindow(interp, argv[2], tkwin);
	if (window == NULL) {
	    return TCL_ERROR;
	}
	if (Tk_GetPixels(interp, window, argv[3], &pixels) != TCL_OK) {
	    return TCL_ERROR;
	}
	sprintf(interp->result, "%d", pixels);
    } else if ((c == 'r') && (strncmp(argv[1], "reqheight", length) == 0)
	    && (length >= 4)) {
	SETUP("reqheight");
	sprintf(interp->result, "%d", Tk_ReqHeight(window));
    } else if ((c == 'r') && (strncmp(argv[1], "reqwidth", length) == 0)
	    && (length >= 4)) {
	SETUP("reqwidth");
	sprintf(interp->result, "%d", Tk_ReqWidth(window));
    } else if ((c == 'r') && (strcmp(argv[1], "rootx") == 0)) {
	int x, y;

	SETUP("rootx");
	Tk_GetRootCoords(window, &x, &y);
	sprintf(interp->result, "%d", x);
    } else if ((c == 'r') && (strcmp(argv[1], "rooty") == 0)) {
	int x, y;

	SETUP("rooty");
	Tk_GetRootCoords(window, &x, &y);
	sprintf(interp->result, "%d", y);
    } else if ((c == 's') && (strcmp(argv[1], "screen") == 0)) {
	char string[20];

	SETUP("screen");
	sprintf(string, "%d", Tk_ScreenNumber(window));
	Tcl_AppendResult(interp, Tk_DisplayName(window), ".", string,
		(char *) NULL);
    } else if ((c == 's') && (strncmp(argv[1], "screencells", length) == 0)
	    && (length >= 7)) {
	SETUP("screencells");
	sprintf(interp->result, "%d", Tk_DefaultVisual(Tk_Screen(window))->map_entries);
    } else if ((c == 's') && (strncmp(argv[1], "screendepth", length) == 0)
	    && (length >= 7)) {
	SETUP("screendepth");
	sprintf(interp->result, "%d", Tk_DefaultDepth(Tk_Screen(window)));
    } else if ((c == 's') && (strncmp(argv[1], "screenheight", length) == 0)
	    && (length >= 7)) {
	SETUP("screenheight");
	sprintf(interp->result, "%d",  HeightOfScreen(Tk_Screen(window)));
    } else if ((c == 's') && (strncmp(argv[1], "screenmmheight", length) == 0)
	    && (length >= 9)) {
	SETUP("screenmmheight");
	sprintf(interp->result, "%d",  HeightMMOfScreen(Tk_Screen(window)));
    } else if ((c == 's') && (strncmp(argv[1], "screenmmwidth", length) == 0)
	    && (length >= 9)) {
	SETUP("screenmmwidth");
	sprintf(interp->result, "%d",  WidthMMOfScreen(Tk_Screen(window)));
    } else if ((c == 's') && (strncmp(argv[1], "screenvisual", length) == 0)
	    && (length >= 7)) {
	SETUP("screenvisual");
	switch (Tk_DefaultVisual(Tk_Screen(window))->class) {
	    case PseudoColor:	interp->result = "pseudocolor"; break;
	    case GrayScale:	interp->result = "grayscale"; break;
	    case DirectColor:	interp->result = "directcolor"; break;
	    case TrueColor:	interp->result = "truecolor"; break;
	    case StaticColor:	interp->result = "staticcolor"; break;
	    case StaticGray:	interp->result = "staticgray"; break;
	    default:		interp->result = "unknown"; break;
	}
    } else if ((c == 's') && (strncmp(argv[1], "screenwidth", length) == 0)
	    && (length >= 7)) {
	SETUP("screenwidth");
	sprintf(interp->result, "%d",  WidthOfScreen(Tk_Screen(window)));
    } else if ((c == 's') && (strcmp(argv[1], "server") == 0)) {
	SETUP("server");
	Tcl_AppendResult(interp, Tk_DisplayName(window), (char *) NULL);
    } else if ((c == 't') && (strncmp(argv[1], "toplevel", length) == 0)) {
	SETUP("toplevel");
	for (winPtr = (TkWindow *) window; !(winPtr->flags & TK_TOP_LEVEL);
		winPtr = winPtr->parentPtr) {
	    /* Empty loop body. */
	}
	interp->result = winPtr->pathName;
    } else if ((c == 'w') && (strncmp(argv[1], "width", length) == 0)) {
	SETUP("width");
	sprintf(interp->result, "%d", Tk_Width(window));
    } else if ((c == 'x') && (argv[1][1] == '\0')) {
	SETUP("x");
	sprintf(interp->result, "%d", Tk_X(window));
    } else if ((c == 'y') && (argv[1][1] == '\0')) {
	SETUP("y");
	sprintf(interp->result, "%d", Tk_Y(window));
    } else {
	Tcl_AppendResult(interp, "bad option \"", argv[1],
		"\": must be atom, atomname, children, class, fpixels, geometry, height, ",
		"id, interps, ismapped, name, parent, pathname, ",
		"pixels, reqheight, reqwidth, rootx, rooty, ",
		"screen, screencells, screendepth, screenheight, ",
		"screenmmheight, screenmmwidth, screenvisual, ",
		"screenwidth, toplevel, width, x, or y", (char *) NULL);
	return TCL_ERROR;
    }
    return TCL_OK;

    wrongArgs:
    Tcl_AppendResult(interp, "wrong # arguments: must be \"",
	    argv[0], " ", argName, " window\"", (char *) NULL);
    return TCL_ERROR;
}

/*
 *----------------------------------------------------------------------
 *
 * TkDeadAppCmd --
 *
 *	If an application has been deleted then all Tk commands will be
 *	re-bound to this procedure.
 *
 * Results:
 *	A standard Tcl error is reported to let the user know that
 *	the application is dead.
 *
 * Side effects:
 *	See the user documentation.
 *
 *----------------------------------------------------------------------
 */

	/* ARGSUSED */
int
TkDeadAppCmd(clientData, interp, argc, argv)
    ClientData clientData;	/* Dummy. */
    Tcl_Interp *interp;		/* Current interpreter. */
    int argc;			/* Number of arguments. */
    char **argv;		/* Argument strings. */
{
    Tcl_AppendResult(interp, "can't invoke \"", argv[0],
	    "\" command:  application has been destroyed", (char *) NULL);
    return TCL_ERROR;
}
