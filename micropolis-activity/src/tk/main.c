/* 
 * main.c --
 *
 *	A simple program to test the toolkit facilities.
 *
 * Copyright 1990-1992 Regents of the University of California.
 * Permission to use, copy, modify, and distribute this
 * software and its documentation for any purpose and without
 * fee is hereby granted, provided that the above copyright
 * notice appear in all copies.  The University of California
 * makes no representations about the suitability of this
 * software for any purpose.  It is provided "as is" without
 * express or implied warranty.
 */

#ifndef lint
static char rcsid[] = "$Header: /user6/ouster/wish/RCS/main.c,v 1.68 92/05/07 08:52:02 ouster Exp $ SPRITE (Berkeley)";
#endif

#include "tkconfig.h"
#include "tkint.h"

/*
 * Declarations for library procedures:
 */

extern int isatty();

/*
 * Command used to initialize wish:
 */

char initCmd[] = "source $tk_library/wish.tcl";

Tk_Window w;			/* NULL means window has been deleted. */
Tk_TimerToken timeToken = 0;
int idleHandler = 0;
Tcl_Interp *interp;
int x, y;
Tcl_CmdBuf buffer;
int tty;
extern int Tk_SquareCmd _ANSI_ARGS_((ClientData clientData,
	Tcl_Interp *interp, int argc, char **argv));

/*
 * Information for testing out command-line options:
 */

int synchronize = 0;
char *fileName = NULL;
char *name = NULL;
char *display = NULL;
char *geometry = NULL;

Tk_ArgvInfo argTable[] = {
    {"-file", TK_ARGV_STRING, (char *) NULL, (char *) &fileName,
	"File from which to read commands"},
    {"-geometry", TK_ARGV_STRING, (char *) NULL, (char *) &geometry,
	"Initial geometry for window"},
    {"-display", TK_ARGV_STRING, (char *) NULL, (char *) &display,
	"Display to use"},
    {"-name", TK_ARGV_STRING, (char *) NULL, (char *) &name,
	"Name to use for application"},
    {"-sync", TK_ARGV_CONSTANT, (char *) 1, (char *) &synchronize,
	"Use synchronous mode for display server"},
    {(char *) NULL, TK_ARGV_END, (char *) NULL, (char *) NULL,
	(char *) NULL}
};

    /* ARGSUSED */
void
StdinProc(clientData, mask)
    ClientData clientData;		/* Not used. */
    int mask;
{
    char line[200];
    static int gotPartial = 0;
    char *cmd;
    int result;

    if (mask & TK_READABLE) {
	if (fgets(line, 200, stdin) == NULL) {
	    if (!gotPartial) {
		if (tty) {
		    Tcl_Eval(interp, "destroy .", 0, (char **) NULL);
		    exit(0);
		} else {
		    Tk_DeleteFileHandler(0);
		}
		return;
	    } else {
		line[0] = 0;
	    }
	}
	cmd = Tcl_AssembleCmd(buffer, line);
	if (cmd == NULL) {
	    gotPartial = 1;
	    return;
	}
	gotPartial = 0;
	result = Tcl_RecordAndEval(interp, cmd, 0);
	if (*interp->result != 0) {
	    if ((result != TCL_OK) || (tty)) {
		printf("%s\n", interp->result);
	    }
	}
	if (tty) {
	    printf("wish: ");
	    fflush(stdout);
	}
    }
}

	/* ARGSUSED */
static void
StructureProc(clientData, eventPtr)
    ClientData clientData;	/* Information about window. */
    XEvent *eventPtr;		/* Information about event. */
{
    if (eventPtr->type == DestroyNotify) {
	w = NULL;
    }
}

/*
 * Procedure to map initial window.  This is invoked as a do-when-idle
 * handler.  Wait for all other when-idle handlers to be processed
 * before mapping the window, so that the window's correct geometry
 * has been determined.
 */

	/* ARGSUSED */
static void
DelayedMap(clientData)
    ClientData clientData;	/* Not used. */
{

    while (Tk_DoOneEvent(TK_IDLE_EVENTS) != 0) {
	/* Empty loop body. */
    }
    if (w == NULL) {
	return;
    }
    Tk_MapWindow(w);
}

	/* ARGSUSED */
int
DotCmd(dummy, interp, argc, argv)
    ClientData dummy;			/* Not used. */
    Tcl_Interp *interp;			/* Current interpreter. */
    int argc;				/* Number of arguments. */
    char **argv;			/* Argument strings. */
{
    int x, y;

    if (argc != 3) {
	Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
		" x y\"", (char *) NULL);
	return TCL_ERROR;
    }
    x = strtol(argv[1], (char **) NULL, 0);
    y = strtol(argv[2], (char **) NULL, 0);
    Tk_MakeWindowExist(w);
    XDrawPoint(Tk_Display(w), Tk_WindowId(w),
	    DefaultGCOfScreen(Tk_Screen(w)), x, y);
    return TCL_OK;
}

	/* ARGSUSED */
int
MovetoCmd(dummy, interp, argc, argv)
    ClientData dummy;			/* Not used. */
    Tcl_Interp *interp;			/* Current interpreter. */
    int argc;				/* Number of arguments. */
    char **argv;			/* Argument strings. */
{
    if (argc != 3) {
	Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
		" x y\"", (char *) NULL);
	return TCL_ERROR;
    }
    x = strtol(argv[1], (char **) NULL, 0);
    y = strtol(argv[2], (char **) NULL, 0);
    return TCL_OK;
}
	/* ARGSUSED */
int
LinetoCmd(dummy, interp, argc, argv)
    ClientData dummy;			/* Not used. */
    Tcl_Interp *interp;			/* Current interpreter. */
    int argc;				/* Number of arguments. */
    char **argv;			/* Argument strings. */
{
    int newX, newY;

    if (argc != 3) {
	Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
		" x y\"", (char *) NULL);
	return TCL_ERROR;
    }
    newX = strtol(argv[1], (char **) NULL, 0);
    newY = strtol(argv[2], (char **) NULL, 0);
    Tk_MakeWindowExist(w);
    XDrawLine(Tk_Display(w), Tk_WindowId(w),
	    DefaultGCOfScreen(Tk_Screen(w)), x, y, newX, newY);
    x = newX;
    y = newY;
    return TCL_OK;
}

int
main(argc, argv)
    int argc;
    char **argv;
{
    char *args, *p, *msg;
    char buf[20];
    int result;
    Tk_3DBorder border;

    { extern char *TCL_Library, *TK_Library;
      extern int TK_CreateColormap;
      char *tcllib = getenv("TCL_LIBRARY");
      char *tklib = getenv("TK_LIBRARY");
      char *create = getenv("CREATE_COLORMAP");
      if (tklib != NULL)
	TK_Library = tklib;
      if (tcllib != NULL)
	TCL_Library = tcllib;
      if (create != NULL)
	TK_CreateColormap = 1;
    }

    interp = Tcl_CreateInterp();
#ifdef TCL_MEM_DEBUG
    Tcl_InitMemory(interp);
#endif
    if (Tk_ParseArgv(interp, (Tk_Window) NULL, &argc, argv, argTable, 0)
	    != TCL_OK) {
	fprintf(stderr, "%s\n", interp->result);
	exit(1);
    }
    if (name == NULL) {
	if (fileName != NULL) {
	    p = fileName;
	} else {
	    p = argv[0];
	}
	name = strrchr(p, '/');
	if (name != NULL) {
	    name++;
	} else {
	    name = p;
	}
    }
    w = Tk_CreateMainWindow(interp, display, name);
    if (w == NULL) {
	fprintf(stderr, "%s\n", interp->result);
	exit(1);
    }
    Tk_SetClass(w, "Tk");
    Tk_CreateEventHandler(w, StructureNotifyMask, StructureProc,
	    (ClientData) NULL);
    Tk_DoWhenIdle(DelayedMap, (ClientData) NULL);
    tty = isatty(0);

    args = Tcl_Merge(argc-1, argv+1);
    Tcl_SetVar(interp, "argv", args, TCL_GLOBAL_ONLY);
    ckfree(args);
    sprintf(buf, "%d", argc-1);
    Tcl_SetVar(interp, "argc", buf, TCL_GLOBAL_ONLY);

    if (synchronize) {
	XSynchronize(Tk_Display(w), True);
    }
    Tk_GeometryRequest(w, 200, 200);
    border = Tk_Get3DBorder(interp, w, None, "#4eee94");
    if (border == NULL) {
	Tcl_SetResult(interp, (char *) NULL, TCL_STATIC);
	Tk_SetWindowBackground(w, WhitePixelOfScreen(Tk_Screen(w)));
    } else {
	Tk_SetBackgroundFromBorder(w, border);
    }
    XSetForeground(Tk_Display(w), DefaultGCOfScreen(Tk_Screen(w)),
	    BlackPixelOfScreen(Tk_Screen(w)));
    Tcl_CreateCommand(interp, "dot", DotCmd, (ClientData) w,
	    (void (*)()) NULL);
    Tcl_CreateCommand(interp, "lineto", LinetoCmd, (ClientData) w,
	    (void (*)()) NULL);
    Tcl_CreateCommand(interp, "moveto", MovetoCmd, (ClientData) w,
	    (void (*)()) NULL);
#ifdef SQUARE_DEMO
    Tcl_CreateCommand(interp, "square", Tk_SquareCmd, (ClientData) w,
	    (void (*)()) NULL);
#endif
    if (geometry != NULL) {
	Tcl_SetVar(interp, "geometry", geometry, TCL_GLOBAL_ONLY);
    }
    result = Tcl_Eval(interp, initCmd, 0, (char **) NULL);
    if (result != TCL_OK) {
	goto error;
    }
    if (fileName != NULL) {
	result = Tcl_VarEval(interp, "source ", fileName, (char *) NULL);
	if (result != TCL_OK) {
	    goto error;
	}
	tty = 0;
    } else {
	tty = isatty(0);
	Tk_CreateFileHandler(0, TK_READABLE, StdinProc, (ClientData) 0);
	if (tty) {
	    printf("wish: ");
	}
    }
    fflush(stdout);
    buffer = Tcl_CreateCmdBuf();
    (void) Tcl_Eval(interp, "update", 0, (char **) NULL);

    Tk_MainLoop();
    Tcl_DeleteInterp(interp);
    Tcl_DeleteCmdBuf(buffer);
    exit(0);

error:
    msg = Tcl_GetVar(interp, "errorInfo", TCL_GLOBAL_ONLY);
    if (msg == NULL) {
	msg = interp->result;
    }
    fprintf(stderr, "%s\n", msg);
    Tcl_Eval(interp, "destroy .", 0, (char **) NULL);
    exit(1);
}
