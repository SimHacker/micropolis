/* 
 * tkScrollbar.c --
 *
 *	This module implements a scrollbar widgets for the Tk
 *	toolkit.  A scrollbar displays a slider and two arrows;
 *	mouse clicks on features within the scrollbar cause
 *	scrolling commands to be invoked.
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
static char rcsid[] = "$Header: /user6/ouster/wish/RCS/tkScrollbar.c,v 1.35 92/05/22 16:57:27 ouster Exp $ SPRITE (Berkeley)";
#endif

#include "tkconfig.h"
#include "default.h"
#include "tkint.h"

/*
 * A data structure of the following type is kept for each scrollbar
 * widget managed by this file:
 */

typedef struct {
    Tk_Window tkwin;		/* Window that embodies the scrollbar.  NULL
				 * means that the window has been destroyed
				 * but the data structures haven't yet been
				 * cleaned up.*/
    Tcl_Interp *interp;		/* Interpreter associated with scrollbar. */
    Tk_Uid orientUid;		/* Orientation for window ("vertical" or
				 * "horizontal"). */
    int vertical;		/* Non-zero means vertical orientation
				 * requested, zero means horizontal. */
    int width;			/* Desired narrow dimension of scrollbar,
				 * in pixels. */
    char *command;		/* Command prefix to use when invoking
				 * scrolling commands.  NULL means don't
				 * invoke commands.  Malloc'ed. */
    int commandSize;		/* Number of non-NULL bytes in command. */
    int repeatDelay;		/* How long to wait before auto-repeating
				 * on scrolling actions (in ms). */
    int repeatInterval;		/* Interval between autorepeats (in ms). */

    /*
     * Information used when displaying widget:
     */

    int borderWidth;		/* Width of 3-D borders. */
    Tk_3DBorder bgBorder;	/* Used for drawing background. */
    Tk_3DBorder fgBorder;	/* For drawing foreground shapes. */
    Tk_3DBorder activeBorder;	/* For drawing foreground shapes when
				 * active (i.e. when mouse is positioned
				 * over element).  NULL means use fgBorder. */
    GC copyGC;			/* Used for copying from pixmap onto screen. */
    int relief;			/* Indicates whether window as a whole is
				 * raised, sunken, or flat. */
    int offset;			/* Zero if relief is TK_RELIEF_FLAT,
				 * borderWidth otherwise.   Indicates how
				 * much interior stuff must be offset from
				 * outside edges to leave room for border. */
    int arrowLength;		/* Length of arrows along long dimension of
				 * scrollbar.  Recomputed on window size
				 * changes. */
    int sliderFirst;		/* Pixel coordinate of top or left edge
				 * of slider area, including border. */
    int sliderLast;		/* Coordinate of pixel just after bottom
				 * or right edge of slider area, including
				 * border. */
    int mouseField;		/* Indicates which scrollbar element is
				 * under mouse (e.g. TOP_ARROW;  see below
				 * for possible values). */
    int pressField;		/* Field in which button was pressed, or -1
				 * if no button is down. */
    int pressPos;		/* Position of mouse when button was
				 * pressed (y for vertical scrollbar, x
				 * for horizontal). */
    int pressFirstUnit;		/* Value of "firstUnit" when mouse button
				 * was pressed. */

    /*
     * Information describing the application related to the scrollbar.
     * This information is provided by the application by invoking the
     * "set" widget command.
     */

    int totalUnits;		/* Total dimension of application, in
				 * units. */
    int windowUnits;		/* Maximum number of units that can
				 * be displayed in the window at
				 * once. */
    int firstUnit;		/* Number of last unit visible in
				 * application's window. */
    int lastUnit;		/* Index of last unit visible in window. */

    /*
     * Miscellaneous information:
     */

    Cursor cursor;		/* Current cursor for window, or None. */
    Tk_TimerToken autoRepeat;	/* Token for auto-repeat that's
				 * currently in progress.  NULL means no
				 * auto-repeat in progress. */
    int flags;			/* Various flags;  see below for
				 * definitions. */
} Scrollbar;

/*
 * Legal values for "mouseField" field of Scrollbar structures.  These
 * are also the return values from the ScrollbarPosition procedure.
 */

#define TOP_ARROW	1
#define TOP_GAP		2
#define SLIDER		3
#define BOTTOM_GAP	4
#define BOTTOM_ARROW	5
#define OUTSIDE		6

/*
 * Flag bits for scrollbars:
 * 
 * REDRAW_PENDING:		Non-zero means a DoWhenIdle handler
 *				has already been queued to redraw
 *				this window.
 */

#define REDRAW_PENDING		1

/*
 * Information used for argv parsing.
 */


static Tk_ConfigSpec configSpecs[] = {
    {TK_CONFIG_BORDER, "-activeforeground", "activeForeground", "Background",
	DEF_SCROLLBAR_ACTIVE_FG_COLOR, Tk_Offset(Scrollbar, activeBorder),
	TK_CONFIG_COLOR_ONLY},
    {TK_CONFIG_BORDER, "-activeforeground", "activeForeground", "Background",
	DEF_SCROLLBAR_ACTIVE_FG_MONO, Tk_Offset(Scrollbar, activeBorder),
	TK_CONFIG_MONO_ONLY},
    {TK_CONFIG_BORDER, "-background", "background", "Background",
	DEF_SCROLLBAR_BG_COLOR, Tk_Offset(Scrollbar, bgBorder),
	TK_CONFIG_COLOR_ONLY},
    {TK_CONFIG_BORDER, "-background", "background", "Background",
	DEF_SCROLLBAR_BG_MONO, Tk_Offset(Scrollbar, bgBorder),
	TK_CONFIG_MONO_ONLY},
    {TK_CONFIG_SYNONYM, "-bd", "borderWidth", (char *) NULL,
	(char *) NULL, 0, 0},
    {TK_CONFIG_SYNONYM, "-bg", "background", (char *) NULL,
	(char *) NULL, 0, 0},
    {TK_CONFIG_PIXELS, "-borderwidth", "borderWidth", "BorderWidth",
	DEF_SCROLLBAR_BORDER_WIDTH, Tk_Offset(Scrollbar, borderWidth), 0},
    {TK_CONFIG_STRING, "-command", "command", "Command",
	DEF_SCROLLBAR_COMMAND, Tk_Offset(Scrollbar, command), 0},
    {TK_CONFIG_ACTIVE_CURSOR, "-cursor", "cursor", "Cursor",
	DEF_SCROLLBAR_CURSOR, Tk_Offset(Scrollbar, cursor), TK_CONFIG_NULL_OK},
    {TK_CONFIG_SYNONYM, "-fg", "foreground", (char *) NULL,
	(char *) NULL, 0, 0},
    {TK_CONFIG_BORDER, "-foreground", "foreground", "Foreground",
	DEF_SCROLLBAR_FG_COLOR, Tk_Offset(Scrollbar, fgBorder),
	TK_CONFIG_COLOR_ONLY},
    {TK_CONFIG_BORDER, "-foreground", "foreground", "Foreground",
	DEF_SCROLLBAR_FG_MONO, Tk_Offset(Scrollbar, fgBorder),
	TK_CONFIG_MONO_ONLY},
    {TK_CONFIG_UID, "-orient", "orient", "Orient",
	DEF_SCROLLBAR_ORIENT, Tk_Offset(Scrollbar, orientUid), 0},
    {TK_CONFIG_RELIEF, "-relief", "relief", "Relief",
	DEF_SCROLLBAR_RELIEF, Tk_Offset(Scrollbar, relief), 0},
    {TK_CONFIG_INT, "-repeatdelay", "repeatDelay", "RepeatDelay",
	DEF_SCROLLBAR_REPEAT_DELAY, Tk_Offset(Scrollbar, repeatDelay), 0},
    {TK_CONFIG_INT, "-repeatinterval", "repeatInterval", "RepeatInterval",
	DEF_SCROLLBAR_REPEAT_INTERVAL, Tk_Offset(Scrollbar, repeatInterval), 0},
    {TK_CONFIG_PIXELS, "-width", "width", "Width",
	DEF_SCROLLBAR_WIDTH, Tk_Offset(Scrollbar, width), 0},
    {TK_CONFIG_END, (char *) NULL, (char *) NULL, (char *) NULL,
	(char *) NULL, 0, 0}
};

/*
 * Forward declarations for procedures defined later in this file:
 */

static void		ComputeScrollbarGeometry _ANSI_ARGS_((
			    Scrollbar *scrollPtr));
static int		ConfigureScrollbar _ANSI_ARGS_((Tcl_Interp *interp,
			    Scrollbar *scrollPtr, int argc, char **argv,
			    int flags));
static void		DestroyScrollbar _ANSI_ARGS_((ClientData clientData));
static void		DisplayScrollbar _ANSI_ARGS_((ClientData clientData));
static void		EventuallyRedraw _ANSI_ARGS_((Scrollbar *scrollPtr));
static void		ScrollbarEventProc _ANSI_ARGS_((ClientData clientData,
			    XEvent *eventPtr));
static void		ScrollbarMouseProc _ANSI_ARGS_((ClientData clientData,
			    XEvent *eventPtr));
static void		ScrollbarNewField _ANSI_ARGS_((Scrollbar *scrollPtr,
			    int field));
static int		ScrollbarPosition _ANSI_ARGS_((Scrollbar *scrollPtr,
			    int x, int y));
static void		ScrollbarTimerProc _ANSI_ARGS_((
			    ClientData clientData));
static int		ScrollbarWidgetCmd _ANSI_ARGS_((ClientData clientData,
			    Tcl_Interp *, int argc, char **argv));
static void		ScrollCmd _ANSI_ARGS_((Scrollbar *scrollPtr,
			    int unit));

/*
 *--------------------------------------------------------------
 *
 * Tk_ScrollbarCmd --
 *
 *	This procedure is invoked to process the "scrollbar" Tcl
 *	command.  See the user documentation for details on what
 *	it does.
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
Tk_ScrollbarCmd(clientData, interp, argc, argv)
    ClientData clientData;	/* Main window associated with
				 * interpreter. */
    Tcl_Interp *interp;		/* Current interpreter. */
    int argc;			/* Number of arguments. */
    char **argv;		/* Argument strings. */
{
    Tk_Window tkwin = (Tk_Window) clientData;
    register Scrollbar *scrollPtr;
    Tk_Window new;

    if (argc < 2) {
	Tcl_AppendResult(interp, "wrong # args:  should be \"",
		argv[0], " pathName ?options?\"", (char *) NULL);
	return TCL_ERROR;
    }

    new = Tk_CreateWindowFromPath(interp, tkwin, argv[1], (char *) NULL);
    if (new == NULL) {
	return TCL_ERROR;
    }

    /*
     * Initialize fields that won't be initialized by ConfigureScrollbar,
     * or which ConfigureScrollbar expects to have reasonable values
     * (e.g. resource pointers).
     */

    scrollPtr = (Scrollbar *) ckalloc(sizeof(Scrollbar));
    scrollPtr->tkwin = new;
    scrollPtr->interp = interp;
    scrollPtr->command = NULL;
    scrollPtr->bgBorder = NULL;
    scrollPtr->fgBorder = NULL;
    scrollPtr->activeBorder = NULL;
    scrollPtr->copyGC = None;
    scrollPtr->mouseField = OUTSIDE;
    scrollPtr->pressField = -1;
    scrollPtr->totalUnits = 0;
    scrollPtr->windowUnits = 0;
    scrollPtr->firstUnit = 0;
    scrollPtr->lastUnit = 0;
    scrollPtr->cursor = None;
    scrollPtr->autoRepeat = NULL;
    scrollPtr->flags = 0;

    Tk_SetClass(scrollPtr->tkwin, "Scrollbar");
    Tk_CreateEventHandler(scrollPtr->tkwin, ExposureMask|StructureNotifyMask,
	    ScrollbarEventProc, (ClientData) scrollPtr);
    Tk_CreateEventHandler(scrollPtr->tkwin, EnterWindowMask|LeaveWindowMask
	    |PointerMotionMask|ButtonPressMask|ButtonReleaseMask,
	    ScrollbarMouseProc, (ClientData) scrollPtr);
    Tcl_CreateCommand(interp, Tk_PathName(scrollPtr->tkwin), ScrollbarWidgetCmd,
	    (ClientData) scrollPtr, (void (*)()) NULL);
    if (ConfigureScrollbar(interp, scrollPtr, argc-2, argv+2, 0) != TCL_OK) {
	goto error;
    }

    interp->result = Tk_PathName(scrollPtr->tkwin);
    return TCL_OK;

    error:
    Tk_DestroyWindow(scrollPtr->tkwin);
    return TCL_ERROR;
}

/*
 *--------------------------------------------------------------
 *
 * ScrollbarWidgetCmd --
 *
 *	This procedure is invoked to process the Tcl command
 *	that corresponds to a widget managed by this module.
 *	See the user documentation for details on what it does.
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
ScrollbarWidgetCmd(clientData, interp, argc, argv)
    ClientData clientData;	/* Information about scrollbar
					 * widget. */
    Tcl_Interp *interp;			/* Current interpreter. */
    int argc;				/* Number of arguments. */
    char **argv;			/* Argument strings. */
{
    register Scrollbar *scrollPtr = (Scrollbar *) clientData;
    int result = TCL_OK;
    int length;
    char c;

    if (argc < 2) {
	Tcl_AppendResult(interp, "wrong # args: should be \"",
		argv[0], " option ?arg arg ...?\"", (char *) NULL);
	return TCL_ERROR;
    }
    Tk_Preserve((ClientData) scrollPtr);
    c = argv[1][0];
    length = strlen(argv[1]);
    if ((c == 'c') && (strncmp(argv[1], "configure", length) == 0)) {
	if (argc == 2) {
	    result = Tk_ConfigureInfo(interp, scrollPtr->tkwin, configSpecs,
		    (char *) scrollPtr, (char *) NULL, 0);
	} else if (argc == 3) {
	    result = Tk_ConfigureInfo(interp, scrollPtr->tkwin, configSpecs,
		    (char *) scrollPtr, argv[2], 0);
	} else {
	    result = ConfigureScrollbar(interp, scrollPtr, argc-2, argv+2,
		    TK_CONFIG_ARGV_ONLY);
	}
    } else if ((c == 'g') && (strncmp(argv[1], "get", length) == 0)) {
	if (argc != 2) {
	    Tcl_AppendResult(interp, "wrong # args: should be \"",
		    argv[0], " get\"", (char *) NULL);
	    goto error;
	}
	sprintf(interp->result, "%d %d %d %d", scrollPtr->totalUnits,
		scrollPtr->windowUnits, scrollPtr->firstUnit,
		scrollPtr->lastUnit);
    } else if ((c == 's') && (strncmp(argv[1], "set", length) == 0)) {
	int totalUnits, windowUnits, firstUnit, lastUnit;

	if (argc != 6) {
	    Tcl_AppendResult(interp, "wrong # args: should be \"",
		    argv[0],
		    " set totalUnits windowUnits firstUnit lastUnit\"",
		    (char *) NULL);
	    goto error;
	}
	if (Tcl_GetInt(interp, argv[2], &totalUnits) != TCL_OK) {
	    goto error;
	}
	if (totalUnits < 0) {
	    sprintf(interp->result, "illegal totalUnits %d", totalUnits);
	    goto error;
	}
	if (Tcl_GetInt(interp, argv[3], &windowUnits) != TCL_OK) {
	    goto error;
	}
	if (windowUnits < 0) {
	    sprintf(interp->result, "illegal windowUnits %d", windowUnits);
	    goto error;
	}
	if (Tcl_GetInt(interp, argv[4], &firstUnit) != TCL_OK) {
	    goto error;
	}
	if (Tcl_GetInt(interp, argv[5], &lastUnit) != TCL_OK) {
	    goto error;
	}
	if (totalUnits > 0) {
	    if (lastUnit < firstUnit) {
		sprintf(interp->result, "illegal lastUnit %d", lastUnit);
		goto error;
	    }
	} else {
	    firstUnit = lastUnit = 0;
	}
	scrollPtr->totalUnits = totalUnits;
	scrollPtr->windowUnits = windowUnits;
	scrollPtr->firstUnit = firstUnit;
	scrollPtr->lastUnit = lastUnit;
	ComputeScrollbarGeometry(scrollPtr);
	EventuallyRedraw(scrollPtr);
    } else {
	Tcl_AppendResult(interp, "bad option \"", argv[1],
		"\":  must be configure, get, or set", (char *) NULL);
	goto error;
    }
    Tk_Release((ClientData) scrollPtr);
    return result;

    error:
    Tk_Release((ClientData) scrollPtr);
    return TCL_ERROR;
}

/*
 *----------------------------------------------------------------------
 *
 * DestroyScrollbar --
 *
 *	This procedure is invoked by Tk_EventuallyFree or Tk_Release
 *	to clean up the internal structure of a scrollbar at a safe time
 *	(when no-one is using it anymore).
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Everything associated with the scrollbar is freed up.
 *
 *----------------------------------------------------------------------
 */

static void
DestroyScrollbar(clientData)
    ClientData clientData;	/* Info about scrollbar widget. */
{
    register Scrollbar *scrollPtr = (Scrollbar *) clientData;

    if (scrollPtr->command != NULL) {
	ckfree(scrollPtr->command);
    }
    if (scrollPtr->bgBorder != NULL) {
	Tk_Free3DBorder(scrollPtr->bgBorder);
    }
    if (scrollPtr->fgBorder != NULL) {
	Tk_Free3DBorder(scrollPtr->fgBorder);
    }
    if (scrollPtr->activeBorder != NULL) {
	Tk_Free3DBorder(scrollPtr->activeBorder);
    }
    if (scrollPtr->copyGC != None) {
	Tk_FreeGC(scrollPtr->copyGC);
    }
    if (scrollPtr->cursor != None) {
	Tk_FreeCursor(scrollPtr->cursor);
    }
    ckfree((char *) scrollPtr);
}

/*
 *----------------------------------------------------------------------
 *
 * ConfigureScrollbar --
 *
 *	This procedure is called to process an argv/argc list, plus
 *	the Tk option database, in order to configure (or
 *	reconfigure) a scrollbar widget.
 *
 * Results:
 *	The return value is a standard Tcl result.  If TCL_ERROR is
 *	returned, then interp->result contains an error message.
 *
 * Side effects:
 *	Configuration information, such as colors, border width,
 *	etc. get set for scrollPtr;  old resources get freed,
 *	if there were any.
 *
 *----------------------------------------------------------------------
 */

static int
ConfigureScrollbar(interp, scrollPtr, argc, argv, flags)
    Tcl_Interp *interp;			/* Used for error reporting. */
    register Scrollbar *scrollPtr;	/* Information about widget;  may or
					 * may not already have values for
					 * some fields. */
    int argc;				/* Number of valid entries in argv. */
    char **argv;			/* Arguments. */
    int flags;				/* Flags to pass to
					 * Tk_ConfigureWidget. */
{
    int length;
    XGCValues gcValues;

    if (Tk_ConfigureWidget(interp, scrollPtr->tkwin, configSpecs,
	    argc, argv, (char *) scrollPtr, flags) != TCL_OK) {
	return TCL_ERROR;
    }

    /*
     * A few options need special processing, such as parsing the
     * orientation or setting the background from a 3-D border.
     */

    length = strlen(scrollPtr->orientUid);
    if (strncmp(scrollPtr->orientUid, "vertical", length) == 0) {
	scrollPtr->vertical = 1;
    } else if (strncmp(scrollPtr->orientUid, "horizontal", length) == 0) {
	scrollPtr->vertical = 0;
    } else {
	Tcl_AppendResult(interp, "bad orientation \"", scrollPtr->orientUid,
		"\": must be vertical or horizontal", (char *) NULL);
	return TCL_ERROR;
    }

    if (scrollPtr->command != NULL) {
	scrollPtr->commandSize = strlen(scrollPtr->command);
    } else {
	scrollPtr->commandSize = 0;
    }

    Tk_SetBackgroundFromBorder(scrollPtr->tkwin, scrollPtr->bgBorder);

    if (scrollPtr->copyGC == None) {
	gcValues.graphics_exposures = False;
	scrollPtr->copyGC = Tk_GetGC(scrollPtr->tkwin, GCGraphicsExposures,
	    &gcValues);
    }

    /*
     * Register the desired geometry for the window (leave enough space
     * for the two arrows plus a minimum-size slider, plus border around
     * the whole window, if any).  Then arrange for the window to be
     * redisplayed.
     */

    ComputeScrollbarGeometry(scrollPtr);
    EventuallyRedraw(scrollPtr);
    return TCL_OK;
}

/*
 *--------------------------------------------------------------
 *
 * DisplayScrollbar --
 *
 *	This procedure redraws the contents of a scrollbar window.
 *	It is invoked as a do-when-idle handler, so it only runs
 *	when there's nothing else for the application to do.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Information appears on the screen.
 *
 *--------------------------------------------------------------
 */

static void
DisplayScrollbar(clientData)
    ClientData clientData;	/* Information about window. */
{
    register Scrollbar *scrollPtr = (Scrollbar *) clientData;
    register Tk_Window tkwin = scrollPtr->tkwin;
    XPoint points[7];
    Tk_3DBorder border;
    int relief, width, fieldLength;
    Pixmap pixmap;

    if ((scrollPtr->tkwin == NULL) || !Tk_IsMapped(tkwin)) {
	goto done;
    }

    if (scrollPtr->vertical) {
	width = Tk_Width(tkwin) - 2*scrollPtr->offset;
    } else {
	width = Tk_Height(tkwin) - 2*scrollPtr->offset;
    }

    /*
     * In order to avoid screen flashes, this procedure redraws
     * the scrollbar in a pixmap, then copies the pixmap to the
     * screen in a single operation.  This means that there's no
     * point in time where the on-sreen image has been cleared.
     */

    pixmap = XCreatePixmap(Tk_Display(tkwin), Tk_WindowId(tkwin),
	    Tk_Width(tkwin), Tk_Height(tkwin),
	    Tk_DefaultDepth(Tk_Screen(tkwin)));
    Tk_Fill3DRectangle(Tk_Display(tkwin), pixmap, scrollPtr->bgBorder,
	    0, 0, Tk_Width(tkwin), Tk_Height(tkwin),
	    scrollPtr->borderWidth, scrollPtr->relief);

    /*
     * Draw the top or left arrow.  The coordinates of the polygon
     * points probably seem odd, but they were carefully chosen with
     * respect to X's rules for filling polygons.  These point choices
     * cause the arrows to just fill the narrow dimension of the
     * scrollbar and be properly centered.
     */

    if (scrollPtr->mouseField == TOP_ARROW) {
	border = scrollPtr->activeBorder;
	relief = scrollPtr->pressField == TOP_ARROW ? TK_RELIEF_SUNKEN
		: TK_RELIEF_RAISED;
    } else {
	border = scrollPtr->fgBorder;
	relief = TK_RELIEF_RAISED;
    }
    if (scrollPtr->vertical) {
	points[0].x = scrollPtr->offset - 1;
	points[0].y = scrollPtr->arrowLength + scrollPtr->offset;
	points[1].x = width + scrollPtr->offset;
	points[1].y = points[0].y;
	points[2].x = width/2 + scrollPtr->offset;
	points[2].y = scrollPtr->offset - 1;
	Tk_Fill3DPolygon(Tk_Display(tkwin), pixmap, border,
		points, 3, scrollPtr->borderWidth, relief);
    } else {
	points[0].x = scrollPtr->arrowLength + scrollPtr->offset;
	points[0].y = scrollPtr->offset - 1;
	points[1].x = scrollPtr->offset;
	points[1].y = width/2 + scrollPtr->offset;
	points[2].x = points[0].x;
	points[2].y = width + scrollPtr->offset;
	Tk_Fill3DPolygon(Tk_Display(tkwin), pixmap, border,
		points, 3, scrollPtr->borderWidth, relief);
    }

    /*
     * Display the bottom or right arrow.
     */

    if (scrollPtr->mouseField == BOTTOM_ARROW) {
	border = scrollPtr->activeBorder;
	relief = scrollPtr->pressField == BOTTOM_ARROW ? TK_RELIEF_SUNKEN
		: TK_RELIEF_RAISED;
    } else {
	border = scrollPtr->fgBorder;
	relief = TK_RELIEF_RAISED;
    }
    if (scrollPtr->vertical) {
	points[0].x = scrollPtr->offset;
	points[0].y = Tk_Height(tkwin) - scrollPtr->arrowLength
		- scrollPtr->offset;
	points[1].x = width/2 + scrollPtr->offset;
	points[1].y = Tk_Height(tkwin) - scrollPtr->offset;
	points[2].x = width + scrollPtr->offset;
	points[2].y = points[0].y;
	Tk_Fill3DPolygon(Tk_Display(tkwin), pixmap, border,
		points, 3, scrollPtr->borderWidth, relief);
    } else {
	points[0].x = Tk_Width(tkwin) - scrollPtr->arrowLength
		- scrollPtr->offset;
	points[0].y = scrollPtr->offset - 1;
	points[1].x = points[0].x;
	points[1].y = width + scrollPtr->offset;
	points[2].x = Tk_Width(tkwin) - scrollPtr->offset;
	points[2].y = width/2 + scrollPtr->offset;
	Tk_Fill3DPolygon(Tk_Display(tkwin), pixmap, border,
		points, 3, scrollPtr->borderWidth, relief);
    }

    /*
     * Display the slider.
     */

    if (scrollPtr->mouseField == SLIDER) {
	border = scrollPtr->activeBorder;
	relief = scrollPtr->pressField == SLIDER ? TK_RELIEF_SUNKEN
		: TK_RELIEF_RAISED;
    } else {
	border = scrollPtr->fgBorder;
	relief = TK_RELIEF_RAISED;
    }
    fieldLength = (scrollPtr->vertical ? Tk_Height(tkwin) : Tk_Width(tkwin))
	    - 2*(scrollPtr->arrowLength + scrollPtr->offset);
    if (fieldLength < 0) {
	fieldLength = 0;
    }
    if (scrollPtr->vertical) {
	Tk_Fill3DRectangle(Tk_Display(tkwin), pixmap, border,
		1 + scrollPtr->offset, scrollPtr->sliderFirst,
		width-2, scrollPtr->sliderLast - scrollPtr->sliderFirst,
		scrollPtr->borderWidth, relief);
    } else {
	Tk_Fill3DRectangle(Tk_Display(tkwin), pixmap, border,
		scrollPtr->sliderFirst, 1 + scrollPtr->offset,
		scrollPtr->sliderLast - scrollPtr->sliderFirst, width-2,
		scrollPtr->borderWidth, relief);
    }

    /*
     * Copy the information from the off-screen pixmap onto the screen,
     * then delete the pixmap.
     */

    XCopyArea(Tk_Display(tkwin), pixmap, Tk_WindowId(tkwin),
	scrollPtr->copyGC, 0, 0, Tk_Width(tkwin), Tk_Height(tkwin), 0, 0);
    XFreePixmap(Tk_Display(tkwin), pixmap);

    done:
    scrollPtr->flags &= ~REDRAW_PENDING;
}

/*
 *--------------------------------------------------------------
 *
 * ScrollbarEventProc --
 *
 *	This procedure is invoked by the Tk dispatcher for various
 *	events on scrollbars.
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
ScrollbarEventProc(clientData, eventPtr)
    ClientData clientData;	/* Information about window. */
    XEvent *eventPtr;		/* Information about event. */
{
    Scrollbar *scrollPtr = (Scrollbar *) clientData;

    if ((eventPtr->type == Expose) && (eventPtr->xexpose.count == 0)) {
	EventuallyRedraw(scrollPtr);
    } else if (eventPtr->type == DestroyNotify) {
	Tcl_DeleteCommand(scrollPtr->interp, Tk_PathName(scrollPtr->tkwin));
	scrollPtr->tkwin = NULL;
	if (scrollPtr->flags & REDRAW_PENDING) {
	    Tk_CancelIdleCall(DisplayScrollbar, (ClientData) scrollPtr);
	}
	Tk_EventuallyFree((ClientData) scrollPtr, DestroyScrollbar);
    } else if (eventPtr->type == ConfigureNotify) {
	ComputeScrollbarGeometry(scrollPtr);
    }
}

/*
 *----------------------------------------------------------------------
 *
 * ComputeScrollbarGeometry --
 *
 *	After changes in a scrollbar's size or configuration, this
 *	procedure recomputes various geometry information used in
 *	displaying the scrollbar.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The scrollbar will be displayed differently.
 *
 *----------------------------------------------------------------------
 */

static void
ComputeScrollbarGeometry(scrollPtr)
    register Scrollbar *scrollPtr;	/* Scrollbar whose geometry may
					 * have changed. */
{
    int width, fieldLength;

    if (scrollPtr->relief == TK_RELIEF_FLAT) {
	scrollPtr->offset = 0;
    } else {
	scrollPtr->offset = scrollPtr->borderWidth;
    }
    width = (scrollPtr->vertical) ? Tk_Width(scrollPtr->tkwin)
	    : Tk_Height(scrollPtr->tkwin);
    scrollPtr->arrowLength =
		(((width - 2*scrollPtr->offset)*173) + 100) / 200;
    fieldLength = (scrollPtr->vertical ? Tk_Height(scrollPtr->tkwin)
	    : Tk_Width(scrollPtr->tkwin))
	    - 2*(scrollPtr->arrowLength + scrollPtr->offset);
    if (fieldLength < 0) {
	fieldLength = 0;
    }
    if (scrollPtr->totalUnits <= 0) {
	scrollPtr->sliderFirst = 0;
	scrollPtr->sliderLast = fieldLength;
    } else {
	scrollPtr->sliderFirst = (fieldLength*scrollPtr->firstUnit
		+ scrollPtr->totalUnits/2)/scrollPtr->totalUnits;
	scrollPtr->sliderLast = (fieldLength*(scrollPtr->lastUnit+1)
		+ scrollPtr->totalUnits/2)/scrollPtr->totalUnits;

	/*
	 * Adjust the slider so that some piece of it is always
	 * displayed in the scrollbar and so that it has at least
	 * a minimal width (so it can be grabbed with the mouse).
	 */

	if (scrollPtr->sliderFirst > (fieldLength - 2*scrollPtr->borderWidth)) {
	    scrollPtr->sliderFirst = fieldLength - 2*scrollPtr->borderWidth;
	}
	if (scrollPtr->sliderFirst < 0) {
	    scrollPtr->sliderFirst = 0;
	}
	if (scrollPtr->sliderLast < (scrollPtr->sliderFirst
		+ 2*scrollPtr->borderWidth)) {
	    scrollPtr->sliderLast = scrollPtr->sliderFirst
		    + 2*scrollPtr->borderWidth;
	}
	if (scrollPtr->sliderLast > fieldLength) {
	    scrollPtr->sliderLast = fieldLength;
	}
    }
    scrollPtr->sliderFirst += scrollPtr->arrowLength + scrollPtr->offset;
    scrollPtr->sliderLast += scrollPtr->arrowLength + scrollPtr->offset;

    /*
     * Register the desired geometry for the window (leave enough space
     * for the two arrows plus a minimum-size slider, plus border around
     * the whole window, if any).  Then arrange for the window to be
     * redisplayed.
     */

    if (scrollPtr->vertical) {
	Tk_GeometryRequest(scrollPtr->tkwin,
		scrollPtr->width + 2*scrollPtr->offset,
		2*(scrollPtr->arrowLength + scrollPtr->borderWidth
		+ scrollPtr->offset));
    } else {
	Tk_GeometryRequest(scrollPtr->tkwin,
		2*(scrollPtr->arrowLength + scrollPtr->borderWidth
		+ scrollPtr->offset), scrollPtr->width + 2*scrollPtr->offset);
    }
    Tk_SetInternalBorder(scrollPtr->tkwin, scrollPtr->borderWidth);

}

/*
 *--------------------------------------------------------------
 *
 * ScrollbarPosition --
 *
 *	Determine the scrollbar element corresponding to a
 *	given position.
 *
 * Results:
 *	One of TOP_ARROW, TOP_GAP, etc., indicating which element
 *	of the scrollbar covers the position given by (x, y).  If
 *	(x,y) is outside the scrollbar entirely, then OUTSIDE is
 *	returned.
 *
 * Side effects:
 *	None.
 *
 *--------------------------------------------------------------
 */

static int
ScrollbarPosition(scrollPtr, x, y)
    register Scrollbar *scrollPtr;	/* Scrollbar widget record. */
    int x, y;				/* Coordinates within scrollPtr's
					 * window. */
{
    int length, width, tmp;

    if (scrollPtr->vertical) {
	length = Tk_Height(scrollPtr->tkwin);
	width = Tk_Width(scrollPtr->tkwin);
    } else {
	tmp = x;
	x = y;
	y = tmp;
	length = Tk_Width(scrollPtr->tkwin);
	width = Tk_Height(scrollPtr->tkwin);
    }

    if ((x < 0) || (x > width) || (y < 0)) {
	return OUTSIDE;
    }

    /*
     * All of the calculations in this procedure mirror those in
     * DisplayScrollbar.  Be sure to keep the two consistent.
     */

    if (y < (scrollPtr->offset + scrollPtr->arrowLength)) {
	return TOP_ARROW;
    }
    if (y < scrollPtr->sliderFirst) {
	return TOP_GAP;
    }
    if (y < scrollPtr->sliderLast) {
	return SLIDER;
    }
    if (y >= (length - (scrollPtr->arrowLength + scrollPtr->offset))) {
	return BOTTOM_ARROW;
    }
    return BOTTOM_GAP;
}

/*
 *--------------------------------------------------------------
 *
 * ScrollbarMouseProc --
 *
 *	This procedure is called back by Tk in response to
 *	mouse events such as window entry, window exit, mouse
 *	motion, and button presses.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	This procedure implements the "feel" of the scrollbar
 *	by issuing scrolling commands in response to button presses
 *	and mouse motion.
 *
 *--------------------------------------------------------------
 */

static void
ScrollbarMouseProc(clientData, eventPtr)
    ClientData clientData;		/* Information about window. */
    register XEvent *eventPtr;		/* Information about event. */
{
    register Scrollbar *scrollPtr = (Scrollbar *) clientData;

    Tk_Preserve((ClientData) scrollPtr);
    if (eventPtr->type == EnterNotify) {
	if (scrollPtr->pressField == -1) {
	    ScrollbarNewField(scrollPtr,
		    ScrollbarPosition(scrollPtr, eventPtr->xcrossing.x,
		    eventPtr->xcrossing.y));
	}
    } else if (eventPtr->type == LeaveNotify) {
	if (scrollPtr->pressField == -1) {
	    ScrollbarNewField(scrollPtr, OUTSIDE);
	}
    } else if (eventPtr->type == MotionNotify) {
	if (scrollPtr->pressField == SLIDER) {
	    int delta, length, newFirst;

	    if (scrollPtr->vertical) {
		delta = eventPtr->xmotion.y - scrollPtr->pressPos;
		length = Tk_Height(scrollPtr->tkwin)
			- 2*(scrollPtr->arrowLength + scrollPtr->offset);
	    } else {
		delta = eventPtr->xmotion.x - scrollPtr->pressPos;
		length = Tk_Width(scrollPtr->tkwin)
			- 2*(scrollPtr->arrowLength + scrollPtr->offset);
	    }

	    /*
	     * Do the division with positive numbers to avoid
	     * differences in negative-number truncation on different
	     * machines.
	     */

	    if (delta >= 0) {
		newFirst = scrollPtr->pressFirstUnit
			+ ((delta * scrollPtr->totalUnits) + (length/2))
			/ length;
	    } else {
		newFirst = scrollPtr->pressFirstUnit
			- (((-delta) * scrollPtr->totalUnits) + (length/2))
			/ length;
	    }
	    ScrollCmd(scrollPtr, newFirst);
	} else if (scrollPtr->pressField == -1) {
	    ScrollbarNewField(scrollPtr,
		    ScrollbarPosition(scrollPtr, eventPtr->xmotion.x,
		    eventPtr->xmotion.y));
	}
    } else if ((eventPtr->type == ButtonPress)
	    && (eventPtr->xbutton.state == 0)) {
	scrollPtr->pressField = scrollPtr->mouseField;
	if (scrollPtr->pressField != SLIDER) {
	    scrollPtr->autoRepeat = Tk_CreateTimerHandler(
		    scrollPtr->repeatDelay,
		    ScrollbarTimerProc, (ClientData) scrollPtr);
	}
	if (scrollPtr->vertical) {
	    scrollPtr->pressPos = eventPtr->xbutton.y;
	} else {
	    scrollPtr->pressPos = eventPtr->xbutton.x;
	}
	scrollPtr->pressFirstUnit = scrollPtr->firstUnit;
	if (scrollPtr->pressFirstUnit <= -scrollPtr->windowUnits) {
	    scrollPtr->pressFirstUnit = 1-scrollPtr->windowUnits;
	}
	if (scrollPtr->pressFirstUnit >= scrollPtr->totalUnits) {
	    scrollPtr->pressFirstUnit = scrollPtr->totalUnits-1;
	}
	EventuallyRedraw(scrollPtr);
    } else if (eventPtr->type == ButtonRelease) {
	if (scrollPtr->pressField == scrollPtr->mouseField) {
	    switch (scrollPtr->pressField) {
		case TOP_ARROW:
		    ScrollCmd(scrollPtr, scrollPtr->firstUnit-1);
		    break;
		case TOP_GAP:
		    if (scrollPtr->windowUnits <= 1) {
			ScrollCmd(scrollPtr, scrollPtr->firstUnit - 1);
		    } else {
			ScrollCmd(scrollPtr, scrollPtr->firstUnit
				- (scrollPtr->windowUnits-1));
		    }
		    break;
		case BOTTOM_GAP: {
		    if (scrollPtr->windowUnits <= 1) {
			ScrollCmd(scrollPtr, scrollPtr->firstUnit + 1);
		    } else {
			ScrollCmd(scrollPtr, scrollPtr->firstUnit
				+ (scrollPtr->windowUnits-1));
		    }
		    break;
		}
		case BOTTOM_ARROW:
		    ScrollCmd(scrollPtr, scrollPtr->firstUnit+1);
		    break;
	    }
	}
	if (scrollPtr->autoRepeat != NULL) {
	    Tk_DeleteTimerHandler(scrollPtr->autoRepeat);
	    scrollPtr->autoRepeat = NULL;
	}
	EventuallyRedraw(scrollPtr);
	scrollPtr->pressField = -1;
	ScrollbarNewField(scrollPtr,
		ScrollbarPosition(scrollPtr, eventPtr->xbutton.x,
		eventPtr->xbutton.y));
    }
    Tk_Release((ClientData) scrollPtr);
}

/*
 *--------------------------------------------------------------
 *
 * ScrollCmd --
 *
 *	This procedure takes care of invoking a scrolling Tcl
 *	command and reporting any error that occurs in it.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	A Tcl command is invoked, and an additional error-processing
 *	command may also be invoked.
 *
 *--------------------------------------------------------------
 */

static void
ScrollCmd(scrollPtr, unit)
    register Scrollbar *scrollPtr;	/* Scrollbar from which to issue
					 * command. */
    int unit;				/* Unit position within thing being
					 * being displayed that should appear
					 * at top or right of screen. */
{
    char string[20];
    int result;

    if ((unit == scrollPtr->firstUnit) || (scrollPtr->command == NULL)) {
	return;
    }
    sprintf(string, " %d", unit);
    result = Tcl_VarEval(scrollPtr->interp, scrollPtr->command, string,
	    (char *) NULL);
    if (result != TCL_OK) {
	TkBindError(scrollPtr->interp);
    }
}

/*
 *--------------------------------------------------------------
 *
 * EventuallyRedraw --
 *
 *	Arrange for one or more of the fields of a scrollbar
 *	to be redrawn.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	None.
 *
 *--------------------------------------------------------------
 */

static void
EventuallyRedraw(scrollPtr)
    register Scrollbar *scrollPtr;	/* Information about widget. */
{
    if ((scrollPtr->tkwin == NULL) || (!Tk_IsMapped(scrollPtr->tkwin))) {
	return;
    }
    if ((scrollPtr->flags & REDRAW_PENDING) == 0) {
	Tk_DoWhenIdle(DisplayScrollbar, (ClientData) scrollPtr);
	scrollPtr->flags |= REDRAW_PENDING;
    }
}

/*
 *--------------------------------------------------------------
 *
 * ScrollbarNewField --
 *
 *	This procedure is called to declare that the mouse is in
 *	a particular field of the scrollbar (e.g. top arrow), so
 *	that the field can be highlighed and the previous field
 *	can be returned to normal display.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Fields may be redisplayed.
 *
 *--------------------------------------------------------------
 */

static void
ScrollbarNewField(scrollPtr, field)
    register Scrollbar *scrollPtr;	/* Information about widget. */
    int field;				/* Identifies field under mouse,
					 * e.g. TOP_ARROW. */
{
    if (field == scrollPtr->mouseField) {
	return;
    }
    EventuallyRedraw(scrollPtr);
    scrollPtr->mouseField = field;
}

/*
 *--------------------------------------------------------------
 *
 * ScrollbarTimerProc --
 *
 *	This procedure is invoked as a Tk timer handler for actions
 *	that auto-repeat (mouse presses in an arrow or gap).  It
 *	performs the auto-repeat action.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Whatever action corresponds to the current mouse button
 *	is repeated, and this procedure is rescheduled to execute
 *	again later.
 *
 *--------------------------------------------------------------
 */

static void
ScrollbarTimerProc(clientData)
    ClientData clientData;	/* Information about widget. */
{
    register Scrollbar *scrollPtr = (Scrollbar *) clientData;

    Tk_Preserve((ClientData) scrollPtr);
    switch(scrollPtr->pressField) {
	case TOP_ARROW:
	    ScrollCmd(scrollPtr, scrollPtr->firstUnit-1);
	    break;
	case TOP_GAP:
	    ScrollCmd(scrollPtr, scrollPtr->firstUnit
		    - (scrollPtr->windowUnits-1));
	    break;
	case BOTTOM_GAP: {
	    ScrollCmd(scrollPtr, scrollPtr->firstUnit
		    + (scrollPtr->windowUnits-1));
	    break;
	}
	case BOTTOM_ARROW:
	    ScrollCmd(scrollPtr, scrollPtr->firstUnit+1);
	    break;
    }
    if (scrollPtr->tkwin != NULL) {
	scrollPtr->autoRepeat = Tk_CreateTimerHandler(
		scrollPtr->repeatInterval, ScrollbarTimerProc,
		(ClientData) scrollPtr);
    }
    Tk_Release((ClientData) scrollPtr);
}
