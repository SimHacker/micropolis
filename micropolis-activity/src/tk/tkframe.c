/* 
 * tkFrame.c --
 *
 *	This module implements "frame" widgets for the Tk
 *	toolkit.  Frames are windows with a background color
 *	and possibly a 3-D effect, but no other attributes.
 *
 * Copyright 1990 Regents of the University of California.
 * Permission to use, copy, modify, and distribute this
 * software and its documentation for any purpose and without
 * fee is hereby granted, provided that the above copyright
 * notice appear in all copies.  The University of California
 * makes no representations about the suitability of this
 * software for any purpose.  It is provided "as is" without
 * express or implied warranty.
 */

#ifndef lint
static char rcsid[] = "$Header: /user6/ouster/wish/RCS/tkFrame.c,v 1.27 92/08/21 16:17:24 ouster Exp $ SPRITE (Berkeley)";
#endif

#include "default.h"
#include "tkconfig.h"
#include "tk.h"

/*
 * A data structure of the following type is kept for each
 * frame that currently exists for this process:
 */

typedef struct {
    Tk_Window tkwin;		/* Window that embodies the frame.  NULL
				 * means that the window has been destroyed
				 * but the data structures haven't yet been
				 * cleaned up.*/
    Tcl_Interp *interp;		/* Interpreter associated with
				 * widget.  Used to delete widget
				 * command.  */
    Tk_Uid screenName;		/* If this window isn't a toplevel window
				 * then this is NULL;  otherwise it gives
				 * the name of the screen on which window
				 * is displayed. */
    Tk_3DBorder border;		/* Structure used to draw 3-D border and
				 * background. */
    int borderWidth;		/* Width of 3-D border (if any). */
    int relief;			/* 3-d effect: TK_RELIEF_RAISED etc. */
    int width;			/* Width to request for window.  <= 0 means
				 * don't request any size. */
    int height;			/* Height to request for window.  <= 0 means
				 * don't request any size. */
    char *geometry;		/* Geometry that user requested.  NULL
				 * means use width and height instead. 
				 * Malloc'ed. */
    Cursor cursor;		/* Current cursor for window, or None. */
    int flags;			/* Various flags;  see below for
				 * definitions. */
} Frame;

/*
 * Flag bits for frames:
 *
 * REDRAW_PENDING:		Non-zero means a DoWhenIdle handler
 *				has already been queued to redraw
 *				this window.
 * CLEAR_NEEDED;		Need to clear the window when redrawing.
 */

#define REDRAW_PENDING		1
#define CLEAR_NEEDED		2

static Tk_ConfigSpec configSpecs[] = {
    {TK_CONFIG_BORDER, "-background", "background", "Background",
	DEF_FRAME_BG_COLOR, Tk_Offset(Frame, border), TK_CONFIG_COLOR_ONLY},
    {TK_CONFIG_BORDER, "-background", "background", "Background",
	DEF_FRAME_BG_MONO, Tk_Offset(Frame, border), TK_CONFIG_MONO_ONLY},
    {TK_CONFIG_SYNONYM, "-bd", "borderWidth", (char *) NULL,
	(char *) NULL, 0, 0},
    {TK_CONFIG_SYNONYM, "-bg", "background", (char *) NULL,
	(char *) NULL, 0, 0},
    {TK_CONFIG_PIXELS, "-borderwidth", "borderWidth", "BorderWidth",
	DEF_FRAME_BORDER_WIDTH, Tk_Offset(Frame, borderWidth), 0},
    {TK_CONFIG_ACTIVE_CURSOR, "-cursor", "cursor", "Cursor",
	DEF_FRAME_CURSOR, Tk_Offset(Frame, cursor), TK_CONFIG_NULL_OK},
    {TK_CONFIG_STRING, "-geometry", "geometry", "Geometry",
	DEF_FRAME_GEOMETRY, Tk_Offset(Frame, geometry), TK_CONFIG_NULL_OK},
    {TK_CONFIG_PIXELS, "-height", "height", "Height",
	DEF_FRAME_HEIGHT, Tk_Offset(Frame, height), 0},
    {TK_CONFIG_RELIEF, "-relief", "relief", "Relief",
	DEF_FRAME_RELIEF, Tk_Offset(Frame, relief), 0},
    {TK_CONFIG_PIXELS, "-width", "width", "Width",
	DEF_FRAME_WIDTH, Tk_Offset(Frame, width), 0},
    {TK_CONFIG_END, (char *) NULL, (char *) NULL, (char *) NULL,
	(char *) NULL, 0, 0}
};

/*
 * Forward declarations for procedures defined later in this file:
 */

static int	ConfigureFrame _ANSI_ARGS_((Tcl_Interp *interp,
		    Frame *framePtr, int argc, char **argv, int flags));
static void	DestroyFrame _ANSI_ARGS_((ClientData clientData));
static void	DisplayFrame _ANSI_ARGS_((ClientData clientData));
static void	FrameEventProc _ANSI_ARGS_((ClientData clientData,
		    XEvent *eventPtr));
static int	FrameWidgetCmd _ANSI_ARGS_((ClientData clientData,
		    Tcl_Interp *interp, int argc, char **argv));
static void	MapFrame _ANSI_ARGS_((ClientData clientData));

/*
 *--------------------------------------------------------------
 *
 * Tk_FrameCmd --
 *
 *	This procedure is invoked to process the "frame" and
 *	"toplevel" Tcl commands.  See the user documentation for
 *	details on what it does.
 *
 * Results:
 *	A standard Tcl result.
 *
 * Side effects:
 *	See the user documentation.
 *
 *--------------------------------------------------------------
 */

int
Tk_FrameCmd(clientData, interp, argc, argv)
    ClientData clientData;	/* Main window associated with
				 * interpreter. */
    Tcl_Interp *interp;		/* Current interpreter. */
    int argc;			/* Number of arguments. */
    char **argv;		/* Argument strings. */
{
    Tk_Window tkwin = (Tk_Window) clientData;
    Tk_Window new;
    register Frame *framePtr;
    Tk_Uid screenUid;
    char *className, *screen;
    int src, dst;

    if (argc < 2) {
	Tcl_AppendResult(interp, "wrong # args: should be \"",
		argv[0], " pathName ?options?\"", (char *) NULL);
	return TCL_ERROR;
    }

    /*
     * The code below is a special workaround that extracts a few key
     * options from the argument list now, rather than letting
     * ConfigureFrame do it.  This is necessary because we have
     * to know the window's screen (if it's top-level) and its
     * class before creating the window.
     */

    screen = NULL;
    className = (argv[0][0] == 't') ? "Toplevel" : "Frame";
    for (src = 2, dst = 2; src < argc;  src += 2) {
	char c;

	c = argv[src][1];
	if ((c == 'c')
		&& (strncmp(argv[src], "-class", strlen(argv[src])) == 0)) {
	    className = argv[src+1];
	} else if ((argv[0][0] == 't') && (c == 's')
		&& (strncmp(argv[src], "-screen", strlen(argv[src])) == 0)) {
	    screen = argv[src+1];
	} else {
	    argv[dst] = argv[src];
	    argv[dst+1] = argv[src+1];
	    dst += 2;
	}
    }
    argc -= src-dst;

    /*
     * Provide a default screen for top-level windows (same as screen
     * of parent window).
     */

    if ((argv[0][0] == 't') && (screen == NULL)) {
	screen = "";
    }
    if (screen != NULL) {
	screenUid = Tk_GetUid(screen);
    } else {
	screenUid = NULL;
    }

    /*
     * Create the window.
     */

    new = Tk_CreateWindowFromPath(interp, tkwin, argv[1], screenUid);

    if (new == NULL) {
	return TCL_ERROR;
    }

    Tk_SetClass(new, className);
    framePtr = (Frame *) ckalloc(sizeof(Frame));
    framePtr->tkwin = new;
    framePtr->interp = interp;
    framePtr->screenName = screenUid;
    framePtr->border = NULL;
    framePtr->geometry = NULL;
    framePtr->cursor = None;
    framePtr->flags = 0;
    Tk_CreateEventHandler(framePtr->tkwin, ExposureMask|StructureNotifyMask,
	    FrameEventProc, (ClientData) framePtr);
    Tcl_CreateCommand(interp, Tk_PathName(framePtr->tkwin),
	    FrameWidgetCmd, (ClientData) framePtr, (void (*)()) NULL);

    if (ConfigureFrame(interp, framePtr, argc-2, argv+2, 0) != TCL_OK) {
	Tk_DestroyWindow(framePtr->tkwin);
	return TCL_ERROR;
    }
    if (screenUid != NULL) {
	Tk_DoWhenIdle(MapFrame, (ClientData) framePtr);
    }
    interp->result = Tk_PathName(framePtr->tkwin);
    return TCL_OK;
}

/*
 *--------------------------------------------------------------
 *
 * FrameWidgetCmd --
 *
 *	This procedure is invoked to process the Tcl command
 *	that corresponds to a frame widget.  See the user
 *	documentation for details on what it does.
 *
 * Results:
 *	A standard Tcl result.
 *
 * Side effects:
 *	See the user documentation.
 *
 *--------------------------------------------------------------
 */

static int
FrameWidgetCmd(clientData, interp, argc, argv)
    ClientData clientData;	/* Information about frame widget. */
    Tcl_Interp *interp;		/* Current interpreter. */
    int argc;			/* Number of arguments. */
    char **argv;		/* Argument strings. */
{
    register Frame *framePtr = (Frame *) clientData;
    int result = TCL_OK;
    int length;
    char c;

    if (argc < 2) {
	Tcl_AppendResult(interp, "wrong # args: should be \"",
		argv[0], " option ?arg arg ...?\"", (char *) NULL);
	return TCL_ERROR;
    }
    Tk_Preserve((ClientData) framePtr);
    c = argv[1][0];
    length = strlen(argv[1]);
    if ((c == 'c') && (strncmp(argv[1], "configure", length) == 0)) {
	if (argc == 2) {
	    result = Tk_ConfigureInfo(interp, framePtr->tkwin, configSpecs,
		    (char *) framePtr, (char *) NULL, 0);
	} else if (argc == 3) {
	    result = Tk_ConfigureInfo(interp, framePtr->tkwin, configSpecs,
		    (char *) framePtr, argv[2], 0);
	} else {
	    result = ConfigureFrame(interp, framePtr, argc-2, argv+2,
		    TK_CONFIG_ARGV_ONLY);
	}
    } else {
	Tcl_AppendResult(interp, "bad option \"", argv[1],
		"\":  must be configure", (char *) NULL);
	result = TCL_ERROR;
    }
    Tk_Release((ClientData) framePtr);
    return result;
}

/*
 *----------------------------------------------------------------------
 *
 * DestroyFrame --
 *
 *	This procedure is invoked by Tk_EventuallyFree or Tk_Release
 *	to clean up the internal structure of a frame at a safe time
 *	(when no-one is using it anymore).
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Everything associated with the frame is freed up.
 *
 *----------------------------------------------------------------------
 */

static void
DestroyFrame(clientData)
    ClientData clientData;	/* Info about frame widget. */
{
    register Frame *framePtr = (Frame *) clientData;

    if (framePtr->border != NULL) {
	Tk_Free3DBorder(framePtr->border);
    }
    if (framePtr->geometry != NULL) {
	ckfree(framePtr->geometry);
    }
    if (framePtr->cursor != None) {
	Tk_FreeCursor(framePtr->cursor);
    }
    ckfree((char *) framePtr);
}

/*
 *----------------------------------------------------------------------
 *
 * ConfigureFrame --
 *
 *	This procedure is called to process an argv/argc list, plus
 *	the Tk option database, in order to configure (or
 *	reconfigure) a frame widget.
 *
 * Results:
 *	The return value is a standard Tcl result.  If TCL_ERROR is
 *	returned, then interp->result contains an error message.
 *
 * Side effects:
 *	Configuration information, such as text string, colors, font,
 *	etc. get set for framePtr;  old resources get freed, if there
 *	were any.
 *
 *----------------------------------------------------------------------
 */

static int
ConfigureFrame(interp, framePtr, argc, argv, flags)
    Tcl_Interp *interp;		/* Used for error reporting. */
    register Frame *framePtr;	/* Information about widget;  may or may
				 * not already have values for some fields. */
    int argc;			/* Number of valid entries in argv. */
    char **argv;		/* Arguments. */
    int flags;			/* Flags to pass to Tk_ConfigureWidget. */
{
    if (Tk_ConfigureWidget(interp, framePtr->tkwin, configSpecs,
	    argc, argv, (char *) framePtr, flags) != TCL_OK) {
	return TCL_ERROR;
    }

    Tk_SetBackgroundFromBorder(framePtr->tkwin, framePtr->border);
    Tk_SetInternalBorder(framePtr->tkwin, framePtr->borderWidth);
    if (framePtr->geometry != NULL) {
	int height, width;
	if (sscanf(framePtr->geometry, "%dx%d", &width, &height) != 2) {
	    Tcl_AppendResult(interp, "bad geometry \"", framePtr->geometry,
		    "\": expected widthxheight", (char *) NULL);
	    return TCL_ERROR;
	}
	Tk_GeometryRequest(framePtr->tkwin, width, height);
    } else if ((framePtr->width > 0) && (framePtr->height > 0)) {
	Tk_GeometryRequest(framePtr->tkwin, framePtr->width,
		framePtr->height);
    }

    if (Tk_IsMapped(framePtr->tkwin)
	    && !(framePtr->flags & REDRAW_PENDING)) {
	Tk_DoWhenIdle(DisplayFrame, (ClientData) framePtr);
	framePtr->flags |= REDRAW_PENDING|CLEAR_NEEDED;
    }
    return TCL_OK;
}

/*
 *----------------------------------------------------------------------
 *
 * DisplayFrame --
 *
 *	This procedure is invoked to display a frame widget.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Commands are output to X to display the frame in its
 *	current mode.
 *
 *----------------------------------------------------------------------
 */

static void
DisplayFrame(clientData)
    ClientData clientData;	/* Information about widget. */
{
    register Frame *framePtr = (Frame *) clientData;
    register Tk_Window tkwin = framePtr->tkwin;

    framePtr->flags &= ~REDRAW_PENDING;
    if ((framePtr->tkwin == NULL) || !Tk_IsMapped(tkwin)) {
	return;
    }

    if (framePtr->flags & CLEAR_NEEDED) {
	XClearWindow(Tk_Display(tkwin), Tk_WindowId(tkwin));
	framePtr->flags &= ~CLEAR_NEEDED;
    }
    if ((framePtr->border != NULL)
	    && (framePtr->relief != TK_RELIEF_FLAT)) {
	Tk_Draw3DRectangle(Tk_Display(tkwin), Tk_WindowId(tkwin),
		framePtr->border, 0, 0, Tk_Width(tkwin), Tk_Height(tkwin),
		framePtr->borderWidth, framePtr->relief);
    }
}

/*
 *--------------------------------------------------------------
 *
 * FrameEventProc --
 *
 *	This procedure is invoked by the Tk dispatcher on
 *	structure changes to a frame.  For frames with 3D
 *	borders, this procedure is also invoked for exposures.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	When the window gets deleted, internal structures get
 *	cleaned up.  When it gets exposed, it is redisplayed.
 *
 *--------------------------------------------------------------
 */

static void
FrameEventProc(clientData, eventPtr)
    ClientData clientData;	/* Information about window. */
    register XEvent *eventPtr;	/* Information about event. */
{
    register Frame *framePtr = (Frame *) clientData;

    if ((eventPtr->type == Expose) && (eventPtr->xexpose.count == 0)) {
	if ((framePtr->relief != TK_RELIEF_FLAT) && (framePtr->tkwin != NULL)
		&& !(framePtr->flags & REDRAW_PENDING)) {
	    Tk_DoWhenIdle(DisplayFrame, (ClientData) framePtr);
	    framePtr->flags |= REDRAW_PENDING;
	}
    } else if (eventPtr->type == DestroyNotify) {
	Tcl_DeleteCommand(framePtr->interp, Tk_PathName(framePtr->tkwin));
	framePtr->tkwin = NULL;
	if (framePtr->flags & REDRAW_PENDING) {
	    Tk_CancelIdleCall(DisplayFrame, (ClientData) framePtr);
	}
	Tk_CancelIdleCall(MapFrame, (ClientData) framePtr);
	Tk_EventuallyFree((ClientData) framePtr, DestroyFrame);
    }
}

/*
 *----------------------------------------------------------------------
 *
 * MapFrame --
 *
 *	This procedure is invoked as a when-idle handler to map a
 *	newly-created top-level frame.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The frame given by the clientData argument is mapped.
 *
 *----------------------------------------------------------------------
 */

static void
MapFrame(clientData)
    ClientData clientData;		/* Pointer to frame structure. */
{
    Frame *framePtr = (Frame *) clientData;

    /*
     * Wait for all other background events to be processed before
     * mapping window.  This ensures that the window's correct geometry
     * will have been determined before it is first mapped, so that the
     * window manager doesn't get a false idea of its desired geometry.
     */

    do {
	if (Tk_DoOneEvent(TK_IDLE_EVENTS) == 0) {
	    break;
	}

	/*
	 * After each event, make sure that the window still exists,
	 * and quit if the window has been destroyed.
	 */

	if (framePtr->tkwin == NULL) {
	    return;
	}
    } while (1);
    Tk_MapWindow(framePtr->tkwin);
}
