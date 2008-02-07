/* 
 * tkCanvas.c --
 *
 *	This module implements canvas widgets for the Tk toolkit.
 *	A canvas displays a background and a collection of graphical
 *	objects such as rectangles, lines, and texts.
 *
 * Copyright 1991-1992 Regents of the University of California.
 * Permission to use, copy, modify, and distribute this
 * software and its documentation for any purpose and without
 * fee is hereby granted, provided that the above copyright
 * notice appear in all copies.  The University of California
 * makes no representations about the suitability of this
 * software for any purpose.  It is provided "as is" without
 * express or implied warranty.
 */

#ifndef lint
static char rcsid[] = "$Header: /user6/ouster/wish/RCS/tkCanvas.c,v 1.28 92/08/19 08:47:57 ouster Exp $ SPRITE (Berkeley)";
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "default.h"
#include "tkint.h"
#include "tkconfig.h"
#include "tkcanvas.h"

/*
 * See tkCanvas.h for key data structures used to implement canvases.
 */

/*
 * The structure defined below is used to keep track of a tag search
 * in progress.  Only the "prevPtr" field should be accessed by anyone
 * other than StartTagSearch and NextItem.
 */

typedef struct TagSearch {
    Tk_Canvas *canvasPtr;	/* Canvas widget being searched. */
    Tk_Uid tag;			/* Tag to search for.   0 means return
				 * all items. */
    Tk_Item *prevPtr;		/* Item just before last one found (or NULL
				 * if last one found was first in the item
				 * list of canvasPtr). */
    Tk_Item *currentPtr;	/* Pointer to last item returned. */
    int searchOver;		/* Non-zero means NextItem should always
				 * return NULL. */
} TagSearch;

/*
 * Information used for argv parsing.
 */


static Tk_ConfigSpec configSpecs[] = {
    {TK_CONFIG_BORDER, "-background", "background", "Background",
	DEF_CANVAS_BG_COLOR, Tk_Offset(Tk_Canvas, bgBorder),
	TK_CONFIG_COLOR_ONLY},
    {TK_CONFIG_COLOR, (char *) NULL, (char *) NULL, (char *) NULL,
	(char *) NULL, Tk_Offset(Tk_Canvas, bgColor),
	TK_CONFIG_COLOR_ONLY},
    {TK_CONFIG_BORDER, "-background", "background", "Background",
	DEF_CANVAS_BG_MONO, Tk_Offset(Tk_Canvas, bgBorder),
	TK_CONFIG_MONO_ONLY},
    {TK_CONFIG_COLOR, (char *) NULL, (char *) NULL, (char *) NULL,
	(char *) NULL, Tk_Offset(Tk_Canvas, bgColor),
	TK_CONFIG_MONO_ONLY},
    {TK_CONFIG_SYNONYM, "-bd", "borderWidth", (char *) NULL,
	(char *) NULL, 0, 0},
    {TK_CONFIG_SYNONYM, "-bg", "background", (char *) NULL,
	(char *) NULL, 0, 0},
    {TK_CONFIG_PIXELS, "-borderwidth", "borderWidth", "BorderWidth",
	DEF_CANVAS_BORDER_WIDTH, Tk_Offset(Tk_Canvas, borderWidth), 0},
    {TK_CONFIG_DOUBLE, "-closeenough", "closeEnough", "CloseEnough",
	DEF_CANVAS_CLOSE_ENOUGH, Tk_Offset(Tk_Canvas, closeEnough), 0},
    {TK_CONFIG_BOOLEAN, "-confine", "confine", "Confine",
	DEF_CANVAS_CONFINE, Tk_Offset(Tk_Canvas, confine), 0},
    {TK_CONFIG_ACTIVE_CURSOR, "-cursor", "cursor", "Cursor",
	DEF_CANVAS_CURSOR, Tk_Offset(Tk_Canvas, cursor), TK_CONFIG_NULL_OK},
    {TK_CONFIG_BORDER, "-cursorbackground", "cursorBackground", "Foreground",
	DEF_CANVAS_CURSOR_BG, Tk_Offset(Tk_Canvas, cursorBorder), 0},
    {TK_CONFIG_PIXELS, "-cursorborderwidth", "cursorBorderWidth", "BorderWidth",
	DEF_CANVAS_CURSOR_BD_COLOR, Tk_Offset(Tk_Canvas, cursorBorderWidth),
	TK_CONFIG_COLOR_ONLY},
    {TK_CONFIG_PIXELS, "-cursorborderwidth", "cursorBorderWidth", "BorderWidth",
	DEF_CANVAS_CURSOR_BD_MONO, Tk_Offset(Tk_Canvas, cursorBorderWidth),
	TK_CONFIG_MONO_ONLY},
    {TK_CONFIG_INT, "-cursorofftime", "cursorOffTime", "OffTime",
	DEF_CANVAS_CURSOR_OFF_TIME, Tk_Offset(Tk_Canvas, cursorOffTime), 0},
    {TK_CONFIG_INT, "-cursorontime", "cursorOnTime", "OnTime",
	DEF_CANVAS_CURSOR_ON_TIME, Tk_Offset(Tk_Canvas, cursorOnTime), 0},
    {TK_CONFIG_PIXELS, "-cursorwidth", "cursorWidth", "CursorWidth",
	DEF_CANVAS_CURSOR_WIDTH, Tk_Offset(Tk_Canvas, cursorWidth), 0},
    {TK_CONFIG_PIXELS, "-height", "height", "Height",
	DEF_CANVAS_HEIGHT, Tk_Offset(Tk_Canvas, height), 0},
    {TK_CONFIG_RELIEF, "-relief", "relief", "Relief",
	DEF_CANVAS_RELIEF, Tk_Offset(Tk_Canvas, relief), 0},
    {TK_CONFIG_PIXELS, "-scrollincrement", "scrollIncrement", "ScrollIncrement",
	DEF_CANVAS_SCROLL_INCREMENT, Tk_Offset(Tk_Canvas, scrollIncrement), 0},
    {TK_CONFIG_STRING, "-scrollregion", "scrollRegion", "ScrollRegion",
	DEF_CANVAS_SCROLL_REGION, Tk_Offset(Tk_Canvas, regionString), 0},
    {TK_CONFIG_BORDER, "-selectbackground", "selectBackground", "Foreground",
	DEF_CANVAS_SELECT_COLOR, Tk_Offset(Tk_Canvas, selBorder),
	TK_CONFIG_COLOR_ONLY},
    {TK_CONFIG_BORDER, "-selectbackground", "selectBackground", "Foreground",
	DEF_CANVAS_SELECT_MONO, Tk_Offset(Tk_Canvas, selBorder),
	TK_CONFIG_MONO_ONLY},
    {TK_CONFIG_PIXELS, "-selectborderwidth", "selectBorderWidth", "BorderWidth",
	DEF_CANVAS_SELECT_BD_COLOR, Tk_Offset(Tk_Canvas, selBorderWidth),
	TK_CONFIG_COLOR_ONLY},
    {TK_CONFIG_PIXELS, "-selectborderwidth", "selectBorderWidth", "BorderWidth",
	DEF_CANVAS_SELECT_BD_MONO, Tk_Offset(Tk_Canvas, selBorderWidth),
	TK_CONFIG_MONO_ONLY},
    {TK_CONFIG_COLOR, "-selectforeground", "selectForeground", "Background",
	DEF_CANVAS_SELECT_FG_COLOR, Tk_Offset(Tk_Canvas, selFgColorPtr),
	TK_CONFIG_COLOR_ONLY},
    {TK_CONFIG_COLOR, "-selectforeground", "selectForeground", "Background",
	DEF_CANVAS_SELECT_FG_MONO, Tk_Offset(Tk_Canvas, selFgColorPtr),
	TK_CONFIG_MONO_ONLY},
    {TK_CONFIG_PIXELS, "-width", "width", "Width",
	DEF_CANVAS_WIDTH, Tk_Offset(Tk_Canvas, width), 0},
    {TK_CONFIG_STRING, "-xscrollcommand", "xScrollCommand", "ScrollCommand",
	DEF_CANVAS_X_SCROLL_CMD, Tk_Offset(Tk_Canvas, xScrollCmd), 0},
    {TK_CONFIG_STRING, "-yscrollcommand", "yScrollCommand", "ScrollCommand",
	DEF_CANVAS_Y_SCROLL_CMD, Tk_Offset(Tk_Canvas, yScrollCmd), 0},
    {TK_CONFIG_END, (char *) NULL, (char *) NULL, (char *) NULL,
	(char *) NULL, 0, 0}
};

/*
 * List of all the item types known at present:
 */

static Tk_ItemType *typeList = NULL;	/* NULL means initialization hasn't
					 * been done yet. */

/*
 * Standard item types provided by Tk:
 */

extern Tk_ItemType TkArcType, TkBitmapType, TkLineType;
extern Tk_ItemType TkOvalType, TkPolygonType;
extern Tk_ItemType TkRectangleType, TkTextType, TkWindowType;

/*
 * Various Tk_Uid's used by this module (set up during initialization):
 */

static Tk_Uid allUid = NULL;
static Tk_Uid currentUid = NULL;

/*
 * Statistics counters:
 */

static int numIdSearches;
static int numSlowSearches;

static int CanvasUpdateTime = 200; // Added by Don.

/*
 * Prototypes for procedures defined later in this file:
 */

static void		CanvasBindProc _ANSI_ARGS_((ClientData clientData,
			    XEvent *eventPtr));
static void		CanvasBlinkProc _ANSI_ARGS_((ClientData clientData));
static void		CanvasDoEvent _ANSI_ARGS_((Tk_Canvas *canvasPtr,
			    XEvent *eventPtr));
static void		CanvasEventProc _ANSI_ARGS_((ClientData clientData,
			    XEvent *eventPtr));
static int		CanvasFetchSelection _ANSI_ARGS_((
			    ClientData clientData, int offset,
			    char *buffer, int maxBytes));
static void		CanvasFocusProc _ANSI_ARGS_((ClientData clientData,
			    int gotFocus));
static void		CanvasLostSelection _ANSI_ARGS_((
			    ClientData clientData));
static void		CanvasSelectTo _ANSI_ARGS_((Tk_Canvas *canvasPtr,
			    Tk_Item *itemPtr, int index));
static void		CanvasSetOrigin _ANSI_ARGS_((Tk_Canvas *canvasPtr,
			    int xOrigin, int yOrigin));
static int		CanvasTagsParseProc _ANSI_ARGS_((ClientData clientData,
			    Tcl_Interp *interp, Tk_Window tkwin, char *value,
			    char *widgRec, int offset));
static char *		CanvasTagsPrintProc _ANSI_ARGS_((ClientData clientData,
			    Tk_Window tkwin, char *widgRec, int offset,
			    Tcl_FreeProc **freeProcPtr));
static void		CanvasUpdateScrollbars _ANSI_ARGS_((
			    Tk_Canvas *canvasPtr));
static int		CanvasWidgetCmd _ANSI_ARGS_((ClientData clientData,
			    Tcl_Interp *interp, int argc, char **argv));
static int		ConfigureCanvas _ANSI_ARGS_((Tcl_Interp *interp,
			    Tk_Canvas *canvasPtr, int argc, char **argv,
			    int flags));
static void		DestroyCanvas _ANSI_ARGS_((ClientData clientData));
static void		DisplayCanvas _ANSI_ARGS_((ClientData clientData));
static void		DoItem _ANSI_ARGS_((Tcl_Interp *interp,
			    Tk_Item *itemPtr, Tk_Uid tag));
static void		EventuallyRedrawArea _ANSI_ARGS_((Tk_Canvas *canvasPtr,
			    int x1, int y1, int x2, int y2));
static int		FindItems _ANSI_ARGS_((Tcl_Interp *interp,
			    Tk_Canvas *canvasPtr, int argc, char **argv,
			    char *newTag, char *cmdName, char *option));
static int		FindArea _ANSI_ARGS_((Tcl_Interp *interp,
			    Tk_Canvas *canvasPtr, char **argv, Tk_Uid uid,
			    int enclosed));
static double		GridAlign _ANSI_ARGS_((double coord, double spacing));
static void		InitCanvas _ANSI_ARGS_((void));
static Tk_Item *	NextItem _ANSI_ARGS_((TagSearch *searchPtr));
static void		PickCurrentItem _ANSI_ARGS_((Tk_Canvas *canvasPtr,
			    XEvent *eventPtr));
static void		RelinkItems _ANSI_ARGS_((Tk_Canvas *canvasPtr,
			    char *tag, Tk_Item *prevPtr));
#if defined(USE_XPM3)
static int		SaveCanvas _ANSI_ARGS_((Tcl_Interp *interp,
			    Tk_Canvas *canvasPtr, char *fileName, int x,
			    int y, unsigned int width, unsigned int height));
#endif
static Tk_Item *	StartTagSearch _ANSI_ARGS_((Tk_Canvas *canvasPtr,
			    char *tag, TagSearch *searchPtr));

/*
 * Custom option for handling "-tags" options for canvas items:
 */

Tk_CustomOption tkCanvasTagsOption = {
    CanvasTagsParseProc,
    CanvasTagsPrintProc,
    (ClientData) NULL
};

/*
 *--------------------------------------------------------------
 *
 * Tk_CanvasCmd --
 *
 *	This procedure is invoked to process the "canvas" Tcl
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
Tk_CanvasCmd(clientData, interp, argc, argv)
    ClientData clientData;		/* Main window associated with
				 * interpreter. */
    Tcl_Interp *interp;		/* Current interpreter. */
    int argc;			/* Number of arguments. */
    char **argv;		/* Argument strings. */
{
    Tk_Window tkwin = (Tk_Window) clientData;
    register Tk_Canvas *canvasPtr;
    Tk_Window new;

    if (typeList == NULL) {
	InitCanvas();
    }

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
     * Initialize fields that won't be initialized by ConfigureCanvas,
     * or which ConfigureCanvas expects to have reasonable values
     * (e.g. resource pointers).
     */

    canvasPtr = (Tk_Canvas *) ckalloc(sizeof(Tk_Canvas));
    canvasPtr->tkwin = new;
    canvasPtr->interp = interp;
    canvasPtr->firstItemPtr = NULL;
    canvasPtr->lastItemPtr = NULL;
    canvasPtr->bgBorder = NULL;
    canvasPtr->bgColor = NULL;
    canvasPtr->pixmapGC = None;
    canvasPtr->selBorder = NULL;
    canvasPtr->selFgColorPtr = NULL;
    canvasPtr->selItemPtr = NULL;
    canvasPtr->selectFirst = -1;
    canvasPtr->selectLast = -1;
    canvasPtr->cursorBorder = NULL;
    canvasPtr->cursorBlinkHandler = (Tk_TimerToken) NULL;
    canvasPtr->focusItemPtr = NULL;
    canvasPtr->xOrigin = canvasPtr->yOrigin = 0;
    canvasPtr->drawableXOrigin = canvasPtr->drawableYOrigin = 0;
    canvasPtr->bindingTable = NULL;
    canvasPtr->currentItemPtr = NULL;
    canvasPtr->pickEvent.type = LeaveNotify;
    canvasPtr->xScrollCmd = NULL;
    canvasPtr->yScrollCmd = NULL;
    canvasPtr->regionString = NULL;
    canvasPtr->hotPtr = NULL;
    canvasPtr->cursor = None;
    canvasPtr->pixelsPerMM = WidthOfScreen(Tk_Screen(new));
    canvasPtr->pixelsPerMM /= WidthMMOfScreen(Tk_Screen(new));
    canvasPtr->flags = 0;
    canvasPtr->nextId = 1;
    canvasPtr->updateTimerToken = NULL;

    Tk_SetClass(canvasPtr->tkwin, "Canvas");
    Tk_CreateEventHandler(canvasPtr->tkwin, ExposureMask|StructureNotifyMask,
	    CanvasEventProc, (ClientData) canvasPtr);
    Tk_CreateEventHandler(canvasPtr->tkwin, KeyPressMask|KeyReleaseMask
	    |ButtonPressMask|ButtonReleaseMask|EnterWindowMask
	    |LeaveWindowMask|PointerMotionMask, CanvasBindProc,
	    (ClientData) canvasPtr);
    Tk_CreateSelHandler(canvasPtr->tkwin, XA_STRING, CanvasFetchSelection,
	    (ClientData) canvasPtr, XA_STRING);
    Tcl_CreateCommand(interp, Tk_PathName(canvasPtr->tkwin), CanvasWidgetCmd,
	    (ClientData) canvasPtr, (void (*)()) NULL);
    if (ConfigureCanvas(interp, canvasPtr, argc-2, argv+2, 0) != TCL_OK) {
	goto error;
    }
    Tk_CreateFocusHandler(canvasPtr->tkwin, CanvasFocusProc,
	    (ClientData) canvasPtr);

    interp->result = Tk_PathName(canvasPtr->tkwin);
    return TCL_OK;

    error:
    Tk_DestroyWindow(canvasPtr->tkwin);
    return TCL_ERROR;
}

/*
 *--------------------------------------------------------------
 *
 * CanvasWidgetCmd --
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
CanvasWidgetCmd(clientData, interp, argc, argv)
    ClientData clientData;		/* Information about canvas
					 * widget. */
    Tcl_Interp *interp;			/* Current interpreter. */
    int argc;				/* Number of arguments. */
    char **argv;			/* Argument strings. */
{
    register Tk_Canvas *canvasPtr = (Tk_Canvas *) clientData;
    int length, result;
    char c;
    Tk_Item *itemPtr = NULL;		/* Initialization needed only to
					 * prevent compiler warning. */
    TagSearch search;

    if (argc < 2) {
	Tcl_AppendResult(interp, "wrong # args: should be \"",
		argv[0], " option ?arg arg ...?\"", (char *) NULL);
	return TCL_ERROR;
    }
    Tk_Preserve((ClientData) canvasPtr);
    result = TCL_OK;
    c = argv[1][0];
    length = strlen(argv[1]);
    if ((c == 'a') && (strncmp(argv[1], "addtag", length) == 0)) {
	if (argc < 4) {
	    Tcl_AppendResult(interp, "wrong # args: should be \"",
		    argv[0], " addtags tag searchCommand ?arg arg ...?\"",
		    (char *) NULL);
	    goto error;
	}
	result = FindItems(interp, canvasPtr, argc-3, argv+3, argv[2], argv[0],
		" addtag tag");
    } else if ((c == 'b') && (strncmp(argv[1], "bbox", length) == 0)
	    && (length >= 2)) {
	int i, gotAny;
	int x1 = 0, y1 = 0, x2 = 0, y2 = 0;	/* Initializations needed
						 * only to prevent compiler
						 * warnings. */

	if (argc < 3) {
	    Tcl_AppendResult(interp, "wrong # args: should be \"",
		    argv[0], " bbox tagOrId ?tagOrId ...?\"",
		    (char *) NULL);
	    goto error;
	}
	gotAny = 0;
	for (i = 2; i < argc; i++) {
	    for (itemPtr = StartTagSearch(canvasPtr, argv[i], &search);
		    itemPtr != NULL; itemPtr = NextItem(&search)) {
		if (!gotAny) {
		    x1 = itemPtr->x1;
		    y1 = itemPtr->y1;
		    x2 = itemPtr->x2;
		    y2 = itemPtr->y2;
		    gotAny = 1;
		} else {
		    if (itemPtr->x1 < x1) {
			x1 = itemPtr->x1;
		    }
		    if (itemPtr->y1 < y1) {
			y1 = itemPtr->y1;
		    }
		    if (itemPtr->x2 > x2) {
			x2 = itemPtr->x2;
		    }
		    if (itemPtr->y2 > y2) {
			y2 = itemPtr->y2;
		    }
		}
	    }
	}
	if (gotAny) {
	    sprintf(interp->result, "%d %d %d %d", x1, y1, x2, y2);
	}
    } else if ((c == 'b') && (strncmp(argv[1], "bind", length) == 0)
	    && (length >= 2)) {
	ClientData object;

	if ((argc < 3) || (argc > 5)) {
	    Tcl_AppendResult(interp, "wrong # args: should be \"",
		    argv[0], " bind tagOrId ?sequence? ?command?\"",
		    (char *) NULL);
	    goto error;
	}

	/*
	 * Figure out what object to use for the binding (individual
	 * item vs. tag).
	 */

	object = 0;
	if (isdigit(argv[2][0])) {
	    int id;
	    char *end;

	    id = strtoul(argv[2], &end, 0);
	    if (*end != 0) {
		goto bindByTag;
	    }
	    for (itemPtr = canvasPtr->firstItemPtr; itemPtr != NULL;
		    itemPtr = itemPtr->nextPtr) {
		if (itemPtr->id == id) {
		    object = (ClientData) itemPtr;
		    break;
		}
	    }
	    if (object == 0) {
		Tcl_AppendResult(interp, "item \"", argv[2],
			"\" doesn't exist", (char *) NULL);
		goto error;
	    }
	} else {
	    bindByTag:
	    object = (ClientData) Tk_GetUid(argv[2]);
	}

	/*
	 * Make a binding table if the canvas doesn't already have
	 * one.
	 */

	if (canvasPtr->bindingTable == NULL) {
	    canvasPtr->bindingTable = Tk_CreateBindingTable(interp);
	}

	if (argc == 5) {
	    int append = 0;
	    unsigned long mask;

	    if (argv[4][0] == 0) {
		result = Tk_DeleteBinding(interp, canvasPtr->bindingTable,
			object, argv[3]);
		goto done;
	    }
	    if (argv[4][0] == '+') {
		argv[4]++;
		append = 1;
	    }
	    mask = Tk_CreateBinding(interp, canvasPtr->bindingTable,
		    object, argv[3], argv[4], append);
	    if (mask == 0) {
		goto error;
	    }
	    if (mask & ~(ButtonMotionMask|Button1MotionMask|Button2MotionMask
		    |Button3MotionMask|Button4MotionMask|Button5MotionMask
		    |ButtonPressMask|ButtonReleaseMask|EnterWindowMask
		    |LeaveWindowMask|KeyPressMask|KeyReleaseMask
		    |PointerMotionMask)) {
		Tk_DeleteBinding(interp, canvasPtr->bindingTable,
			object, argv[3]);
		Tcl_ResetResult(interp);
		Tcl_AppendResult(interp, "requested illegal events; ",
			"only key, button, motion, and enter/leave ",
			"events may be used", (char *) NULL);
		goto error;
	    }
	} else if (argc == 4) {
	    char *command;
    
	    command = Tk_GetBinding(interp, canvasPtr->bindingTable,
		    object, argv[3]);
	    if (command == NULL) {
		goto error;
	    }
	    interp->result = command;
	} else {
	    Tk_GetAllBindings(interp, canvasPtr->bindingTable, object);
	}
    } else if ((c == 'c') && (strcmp(argv[1], "canvasx") == 0)) {
	int x;
	double grid;

	if ((argc < 3) || (argc > 4)) {
	    Tcl_AppendResult(interp, "wrong # args: should be \"",
		    argv[0], " canvasx screenx ?gridspacing?\"",
		    (char *) NULL);
	    goto error;
	}
	if (Tk_GetPixels(interp, canvasPtr->tkwin, argv[2], &x) != TCL_OK) {
	    goto error;
	}
	if (argc == 4) {
	    if (TkGetCanvasCoord(canvasPtr, argv[3], &grid) != TCL_OK) {
		goto error;
	    }
	} else {
	    grid = 0.0;
	}
	x += canvasPtr->xOrigin;
	sprintf(interp->result, "%g", GridAlign((double) x, grid));
    } else if ((c == 'c') && (strcmp(argv[1], "canvasy") == 0)) {
	int y;
	double grid;

	if ((argc < 3) || (argc > 4)) {
	    Tcl_AppendResult(interp, "wrong # args: should be \"",
		    argv[0], " canvasy screeny ?gridspacing?\"",
		    (char *) NULL);
	    goto error;
	}
	if (Tk_GetPixels(interp, canvasPtr->tkwin, argv[2], &y) != TCL_OK) {
	    goto error;
	}
	if (argc == 4) {
	    if (TkGetCanvasCoord(canvasPtr, argv[3], &grid) != TCL_OK) {
		goto error;
	    }
	} else {
	    grid = 0.0;
	}
	y += canvasPtr->yOrigin;
	sprintf(interp->result, "%g", GridAlign((double) y, grid));
    } else if ((c == 'c') && (strncmp(argv[1], "configure", length) == 0)
	    && (length >= 3)) {
	if (argc == 2) {
	    result = Tk_ConfigureInfo(interp, canvasPtr->tkwin, configSpecs,
		    (char *) canvasPtr, (char *) NULL, 0);
	} else if (argc == 3) {
	    result = Tk_ConfigureInfo(interp, canvasPtr->tkwin, configSpecs,
		    (char *) canvasPtr, argv[2], 0);
	} else {
	    result = ConfigureCanvas(interp, canvasPtr, argc-2, argv+2,
		    TK_CONFIG_ARGV_ONLY);
	}
    } else if ((c == 'c') && (strncmp(argv[1], "coords", length) == 0)
	    && (length >= 3)) {
	if (argc < 3) {
	    Tcl_AppendResult(interp, "wrong # args: should be \"",
		    argv[0], " coords tagOrId ?x y x y ...?\"",
		    (char *) NULL);
	    goto error;
	}
	itemPtr = StartTagSearch(canvasPtr, argv[2], &search);
	if (itemPtr != NULL) {
	    if (argc != 3) {
		EventuallyRedrawArea(canvasPtr, itemPtr->x1, itemPtr->y1,
		    itemPtr->x2, itemPtr->y2);
	    }
	    if (itemPtr->typePtr->coordProc != NULL) {
		result = (*itemPtr->typePtr->coordProc)(canvasPtr, itemPtr,
			argc-3, argv+3);
	    }
	    if (argc != 3) {
		EventuallyRedrawArea(canvasPtr, itemPtr->x1, itemPtr->y1,
		    itemPtr->x2, itemPtr->y2);
	    }
	}
    } else if ((c == 'c') && (strncmp(argv[1], "create", length) == 0)
	    && (length >= 2)) {
	register Tk_ItemType *typePtr;
	Tk_ItemType *matchPtr = NULL;
	register Tk_Item *itemPtr;

	if (argc < 3) {
	    Tcl_AppendResult(interp, "wrong # args: should be \"",
		    argv[0], " create type ?arg arg ...?\"", (char *) NULL);
	    goto error;
	}
	c = argv[2][0];
	length = strlen(argv[2]);
	for (typePtr = typeList; typePtr != NULL; typePtr = typePtr->nextPtr) {
	    if ((c == typePtr->name[0])
		    && (strncmp(argv[2], typePtr->name, length) == 0)) {
		if (matchPtr != NULL) {
		    badType:
		    Tcl_AppendResult(interp,
			    "unknown or ambiguous item type \"",
			    argv[2], "\"", (char *) NULL);
		    goto error;
		}
		matchPtr = typePtr;
	    }
	}
	if (matchPtr == NULL) {
	    goto badType;
	}
	typePtr = matchPtr;
	itemPtr = (Tk_Item *) ckalloc((unsigned) typePtr->itemSize);
	itemPtr->id = canvasPtr->nextId;
	canvasPtr->nextId++;
	itemPtr->tagPtr = itemPtr->staticTagSpace;
	itemPtr->tagSpace = TK_TAG_SPACE;
	itemPtr->numTags = 0;
	itemPtr->typePtr = typePtr;
	if ((*typePtr->createProc)(canvasPtr, itemPtr, argc-3, argv+3)
		!= TCL_OK) {
	    ckfree((char *) itemPtr);
	    goto error;
	}
	itemPtr->nextPtr = NULL;
	canvasPtr->hotPtr = itemPtr;
	canvasPtr->hotPrevPtr = canvasPtr->lastItemPtr;
	if (canvasPtr->lastItemPtr == NULL) {
	    canvasPtr->firstItemPtr = itemPtr;
	} else {
	    canvasPtr->lastItemPtr->nextPtr = itemPtr;
	}
	canvasPtr->lastItemPtr = itemPtr;
	EventuallyRedrawArea(canvasPtr, itemPtr->x1, itemPtr->y1,
		itemPtr->x2, itemPtr->y2);
	canvasPtr->flags |= REPICK_NEEDED;
	sprintf(interp->result, "%d", itemPtr->id);
    } else if ((c == 'c') && (strncmp(argv[1], "cursor", length) == 0)
	    && (length >= 2)) {
	int index;

	if (argc != 4) {
	    Tcl_AppendResult(interp, "wrong # args: should be \"",
		    argv[0], " cursor tagOrId index\"",
		    (char *) NULL);
	    goto error;
	}
	for (itemPtr = StartTagSearch(canvasPtr, argv[2], &search);
		itemPtr != NULL; itemPtr = NextItem(&search)) {
	    if ((itemPtr->typePtr->indexProc == NULL)
		    || (itemPtr->typePtr->cursorProc == NULL)) {
		goto done;
	    }
	    if ((*itemPtr->typePtr->indexProc)(canvasPtr, itemPtr,
		    argv[3], &index) != TCL_OK) {
		goto error;
	    }
	    (*itemPtr->typePtr->cursorProc)(canvasPtr, itemPtr, index);
	    if ((itemPtr == canvasPtr->focusItemPtr)
		    && (canvasPtr->flags & CURSOR_ON)) {
		EventuallyRedrawArea(canvasPtr, itemPtr->x1, itemPtr->y1,
			itemPtr->x2, itemPtr->y2);
	    }
	}
    } else if ((c == 'd') && (strncmp(argv[1], "dchars", length) == 0)
	    && (length >= 2)) {
	int first, last;

	if ((argc != 4) && (argc != 5)) {
	    Tcl_AppendResult(interp, "wrong # args: should be \"",
		    argv[0], " dchars tagOrId first ?last?\"",
		    (char *) NULL);
	    goto error;
	}
	for (itemPtr = StartTagSearch(canvasPtr, argv[2], &search);
		itemPtr != NULL; itemPtr = NextItem(&search)) {
	    if ((itemPtr->typePtr->indexProc == NULL)
		    || (itemPtr->typePtr->dCharsProc == NULL)) {
		continue;
	    }
	    if ((*itemPtr->typePtr->indexProc)(canvasPtr, itemPtr,
		    argv[3], &first) != TCL_OK) {
		goto error;
	    }
	    if (argc == 5) {
		if ((*itemPtr->typePtr->indexProc)(canvasPtr, itemPtr,
			argv[4], &last) != TCL_OK) {
		    goto error;
		}
	    } else {
		last = first;
	    }

	    /*
	     * Redraw both item's old and new areas:  it's possible
	     * that a delete could result in a new area larger than
	     * the old area.
	     */

	    EventuallyRedrawArea(canvasPtr, itemPtr->x1, itemPtr->y1,
		    itemPtr->x2, itemPtr->y2);
	    result = (*itemPtr->typePtr->dCharsProc)(canvasPtr, itemPtr,
		    first, last);
	    EventuallyRedrawArea(canvasPtr, itemPtr->x1, itemPtr->y1,
		    itemPtr->x2, itemPtr->y2);
	    if (result != TCL_OK) {
		goto error;
	    }
	}
    } else if ((c == 'd') && (strncmp(argv[1], "delete", length) == 0)
	    && (length >= 2)) {
	if (argc != 3) {
	    Tcl_AppendResult(interp, "wrong # args: should be \"",
		    argv[0], " delete tagOrId\"",
		    (char *) NULL);
	    goto error;
	}
	for (itemPtr = StartTagSearch(canvasPtr, argv[2], &search);
		itemPtr != NULL; itemPtr = NextItem(&search)) {
	    EventuallyRedrawArea(canvasPtr, itemPtr->x1, itemPtr->y1,
		itemPtr->x2, itemPtr->y2);
	    (*itemPtr->typePtr->deleteProc)(itemPtr);
	    if (itemPtr->tagPtr != itemPtr->staticTagSpace) {
		ckfree((char *) itemPtr->tagPtr);
	    }
	    if (search.prevPtr == NULL) {
		canvasPtr->firstItemPtr = itemPtr->nextPtr;
		if (canvasPtr->firstItemPtr == NULL) {
		    canvasPtr->lastItemPtr = NULL;
		}
	    } else {
		search.prevPtr->nextPtr = itemPtr->nextPtr;
	    }
	    if (canvasPtr->lastItemPtr == itemPtr) {
		canvasPtr->lastItemPtr = search.prevPtr;
	    }
	    ckfree((char *) itemPtr);
	    if (itemPtr == canvasPtr->currentItemPtr) {
		canvasPtr->currentItemPtr = NULL;
		canvasPtr->flags |= REPICK_NEEDED;
	    }
	    if (itemPtr == canvasPtr->focusItemPtr) {
		canvasPtr->focusItemPtr = NULL;
	    }
	    if (itemPtr == canvasPtr->selItemPtr) {
		canvasPtr->selItemPtr = NULL;
	    }
	    if ((itemPtr == canvasPtr->hotPtr)
		    || (itemPtr = canvasPtr->hotPrevPtr)) {
		canvasPtr->hotPtr = NULL;
	    }
	}
    } else if ((c == 'd') && (strncmp(argv[1], "dtag", length) == 0)
	    && (length >= 2)) {
	Tk_Uid tag;
	int i;

	if ((argc != 3) && (argc != 4)) {
	    Tcl_AppendResult(interp, "wrong # args: should be \"",
		    argv[0], " dtag tagOrId ?tagToDelete?\"",
		    (char *) NULL);
	    goto error;
	}
	if (argc == 4) {
	    tag = Tk_GetUid(argv[3]);
	} else {
	    tag = Tk_GetUid(argv[2]);
	}
	for (itemPtr = StartTagSearch(canvasPtr, argv[2], &search);
		itemPtr != NULL; itemPtr = NextItem(&search)) {
	    for (i = itemPtr->numTags-1; i >= 0; i--) {
		if (itemPtr->tagPtr[i] == tag) {
		    itemPtr->tagPtr[i] = itemPtr->tagPtr[itemPtr->numTags-1];
		    itemPtr->numTags--;
		}
	    }
	}
    } else if ((c == 'f') && (strncmp(argv[1], "find", length) == 0)
	    && (length >= 2)) {
	if (argc < 3) {
	    Tcl_AppendResult(interp, "wrong # args: should be \"",
		    argv[0], " find searchCommand ?arg arg ...?\"",
		    (char *) NULL);
	    goto error;
	}
	result = FindItems(interp, canvasPtr, argc-2, argv+2, (char *) NULL,
		argv[0]," find");
    } else if ((c == 'f') && (strncmp(argv[1], "focus", length) == 0)
	    && (length >= 2)) {
	if (argc > 3) {
	    Tcl_AppendResult(interp, "wrong # args: should be \"",
		    argv[0], " focus ?tagOrId?\"",
		    (char *) NULL);
	    goto error;
	}
	itemPtr = canvasPtr->focusItemPtr;
	if (argc == 2) {
	    if (itemPtr != NULL) {
		sprintf(interp->result, "%d", itemPtr->id);
	    }
	    goto done;
	}
	if ((itemPtr != NULL) && (canvasPtr->flags & GOT_FOCUS)) {
	    EventuallyRedrawArea(canvasPtr, itemPtr->x1, itemPtr->y1,
		    itemPtr->x2, itemPtr->y2);
	}
	if (argv[2][0] == 0) {
	    canvasPtr->focusItemPtr = NULL;
	    goto done;
	}
	for (itemPtr = StartTagSearch(canvasPtr, argv[2], &search);
		itemPtr != NULL; itemPtr = NextItem(&search)) {
	    if (itemPtr->typePtr->cursorProc != NULL) {
		break;
	    }
	}
	if (itemPtr == NULL) {
	    goto done;
	}
	canvasPtr->focusItemPtr = itemPtr;
	if (canvasPtr->flags & GOT_FOCUS) {
	    EventuallyRedrawArea(canvasPtr, itemPtr->x1, itemPtr->y1,
		    itemPtr->x2, itemPtr->y2);
	}
    } else if ((c == 'g') && (strncmp(argv[1], "gettags", length) == 0)) {
	if (argc != 3) {
	    Tcl_AppendResult(interp, "wrong # args: should be \"",
		    argv[0], " gettags tagOrId\"", (char *) NULL);
	    goto error;
	}
	itemPtr = StartTagSearch(canvasPtr, argv[2], &search);
	if (itemPtr != NULL) {
	    int i;
	    for (i = 0; i < itemPtr->numTags; i++) {
		Tcl_AppendElement(interp, (char *) itemPtr->tagPtr[i], 0);
	    }
	}
    } else if ((c == 'i') && (strncmp(argv[1], "index", length) == 0)
	    && (length >= 3)) {
	int index;

	if (argc != 4) {
	    Tcl_AppendResult(interp, "wrong # args: should be \"",
		    argv[0], " index tagOrId string\"",
		    (char *) NULL);
	    goto error;
	}
	for (itemPtr = StartTagSearch(canvasPtr, argv[2], &search);
		itemPtr != NULL; itemPtr = NextItem(&search)) {
	    if (itemPtr->typePtr->indexProc != NULL) {
		break;
	    }
	}
	if (itemPtr == NULL) {
	    Tcl_AppendResult(interp, "can't find an indexable item \"",
		    argv[2], "\"", (char *) NULL);
	    goto error;
	}
	if ((*itemPtr->typePtr->indexProc)(canvasPtr, itemPtr,
		argv[3], &index) != TCL_OK) {
	    goto error;
	}
	sprintf(interp->result, "%d", index);
    } else if ((c == 'i') && (strncmp(argv[1], "insert", length) == 0)
	    && (length >= 3)) {
	int beforeThis;

	if (argc != 5) {
	    Tcl_AppendResult(interp, "wrong # args: should be \"",
		    argv[0], " insert tagOrId beforeThis string\"",
		    (char *) NULL);
	    goto error;
	}
	for (itemPtr = StartTagSearch(canvasPtr, argv[2], &search);
		itemPtr != NULL; itemPtr = NextItem(&search)) {
	    if ((itemPtr->typePtr->indexProc == NULL)
		    || (itemPtr->typePtr->insertProc == NULL)) {
		continue;
	    }
	    if ((*itemPtr->typePtr->indexProc)(canvasPtr, itemPtr,
		    argv[3], &beforeThis) != TCL_OK) {
		goto error;
	    }

	    /*
	     * Redraw both item's old and new areas:  it's possible
	     * that an insertion could result in a new area either
	     * larger or smaller than the old area.
	     */

	    EventuallyRedrawArea(canvasPtr, itemPtr->x1, itemPtr->y1,
		    itemPtr->x2, itemPtr->y2);
	    result = (*itemPtr->typePtr->insertProc)(canvasPtr, itemPtr,
		    beforeThis, argv[4]);
	    EventuallyRedrawArea(canvasPtr, itemPtr->x1, itemPtr->y1,
		    itemPtr->x2, itemPtr->y2);
	    if (result != TCL_OK) {
		goto error;
	    }
	}
    } else if ((c == 'i') && (strncmp(argv[1], "itemconfigure", length) == 0)
	    && (length >= 2)) {
	if (argc < 3) {
	    Tcl_AppendResult(interp, "wrong # args: should be \"",
		    argv[0], " itemconfigure tagOrId ?option value ...?\"",
		    (char *) NULL);
	    goto error;
	}
	for (itemPtr = StartTagSearch(canvasPtr, argv[2], &search);
		itemPtr != NULL; itemPtr = NextItem(&search)) {
	    if (argc == 3) {
		result = Tk_ConfigureInfo(canvasPtr->interp, canvasPtr->tkwin,
			itemPtr->typePtr->configSpecs, (char *) itemPtr,
			(char *) NULL, 0);
	    } else if (argc == 4) {
		result = Tk_ConfigureInfo(canvasPtr->interp, canvasPtr->tkwin,
			itemPtr->typePtr->configSpecs, (char *) itemPtr,
			argv[3], 0);
	    } else {
		EventuallyRedrawArea(canvasPtr, itemPtr->x1, itemPtr->y1,
			itemPtr->x2, itemPtr->y2);
		result = (*itemPtr->typePtr->configProc)(canvasPtr, itemPtr,
			argc-3, argv+3, TK_CONFIG_ARGV_ONLY);
		EventuallyRedrawArea(canvasPtr, itemPtr->x1, itemPtr->y1,
			itemPtr->x2, itemPtr->y2);
		canvasPtr->flags |= REPICK_NEEDED;
	    }
	    if ((result != TCL_OK) || (argc < 5)) {
		break;
	    }
	}
    } else if ((c == 'l') && (strncmp(argv[1], "lower", length) == 0)) {
	Tk_Item *prevPtr;

	if ((argc != 3) && (argc != 4)) {
	    Tcl_AppendResult(interp, "wrong # args: should be \"",
		    argv[0], " lower tagOrId ?belowThis?\"",
		    (char *) NULL);
	    goto error;
	}

	/*
	 * First find the item just after which we'll insert the
	 * named items.
	 */

	if (argc == 3) {
	    prevPtr = NULL;
	} else {
	    prevPtr = StartTagSearch(canvasPtr, argv[3], &search);
	    if (prevPtr != NULL) {
		prevPtr = search.prevPtr;
	    } else {
		Tcl_AppendResult(interp, "tag \"", argv[3],
			"\" doesn't match any items", (char *) NULL);
		goto error;
	    }
	}
	RelinkItems(canvasPtr, argv[2], prevPtr);
    } else if ((c == 'm') && (strncmp(argv[1], "move", length) == 0)) {
	double xAmount, yAmount;

	if (argc != 5) {
	    Tcl_AppendResult(interp, "wrong # args: should be \"",
		    argv[0], " move tagOrId xAmount yAmount\"",
		    (char *) NULL);
	    goto error;
	}
	if ((TkGetCanvasCoord(canvasPtr, argv[3], &xAmount) != TCL_OK)
		|| (TkGetCanvasCoord(canvasPtr, argv[4], &yAmount) != TCL_OK)) {
	    goto error;
	}
	for (itemPtr = StartTagSearch(canvasPtr, argv[2], &search);
		itemPtr != NULL; itemPtr = NextItem(&search)) {
	    EventuallyRedrawArea(canvasPtr, itemPtr->x1, itemPtr->y1,
		itemPtr->x2, itemPtr->y2);
	    (void) (*itemPtr->typePtr->translateProc)(canvasPtr, itemPtr,
		    xAmount, yAmount);
	    EventuallyRedrawArea(canvasPtr, itemPtr->x1, itemPtr->y1,
		itemPtr->x2, itemPtr->y2);
	    canvasPtr->flags |= REPICK_NEEDED;
	}
    } else if ((c == 'r') && (strncmp(argv[1], "raise", length) == 0)) {
	Tk_Item *prevPtr;

	if ((argc != 3) && (argc != 4)) {
	    Tcl_AppendResult(interp, "wrong # args: should be \"",
		    argv[0], " raise tagOrId ?aboveThis?\"",
		    (char *) NULL);
	    goto error;
	}

	/*
	 * First find the item just after which we'll insert the
	 * named items.
	 */

	if (argc == 3) {
	    prevPtr = canvasPtr->lastItemPtr;
	} else {
	    prevPtr = NULL;
	    for (itemPtr = StartTagSearch(canvasPtr, argv[3], &search);
		    itemPtr != NULL; itemPtr = NextItem(&search)) {
		prevPtr = itemPtr;
	    }
	    if (prevPtr == NULL) {
		Tcl_AppendResult(interp, "tagOrId \"", argv[3],
			"\" doesn't match any items", (char *) NULL);
		goto error;
	    }
	}
	RelinkItems(canvasPtr, argv[2], prevPtr);
#if defined(USE_XPM3)
    } else if ((c == 's') && (strncmp(argv[1], "save", length) == 0)
	    && (length >= 3)) {
	if (argc != 3 && argc != 7) {
	    Tcl_AppendResult(interp, "wrong # args: should be \"",
		    argv[0], " save fileName ?x y width height?\"",
		    (char *) NULL);
	    goto error;
	}
        if (argc == 3) {
	    if (SaveCanvas(interp, canvasPtr, argv[2], 0, 0, 0, 0) != TCL_OK) {
		goto error;
	    }
        } else {
	    if (SaveCanvas(interp, canvasPtr, argv[2], atol(argv[3]),
		    atol(argv[4]), atol(argv[5]), atol(argv[6]))) {
		goto error;
	    }
        }
#endif
    } else if ((c == 's') && (strncmp(argv[1], "scale", length) == 0)
	    && (length >= 3)) {
	double xOrigin, yOrigin, xScale, yScale;

	if (argc != 7) {
	    Tcl_AppendResult(interp, "wrong # args: should be \"",
		    argv[0], " scale tagOrId xOrigin yOrigin xScale yScale\"",
		    (char *) NULL);
	    goto error;
	}
	if ((TkGetCanvasCoord(canvasPtr, argv[3], &xOrigin) != TCL_OK)
		|| (TkGetCanvasCoord(canvasPtr, argv[4], &yOrigin) != TCL_OK)
		|| (Tcl_GetDouble(interp, argv[5], &xScale) != TCL_OK)
		|| (Tcl_GetDouble(interp, argv[6], &yScale) != TCL_OK)) {
	    goto error;
	}
	if ((xScale <= 0.0) || (yScale <= 0.0)) {
	    interp->result = "scale factors must be greater than zero";
	    goto error;
	}
	for (itemPtr = StartTagSearch(canvasPtr, argv[2], &search);
		itemPtr != NULL; itemPtr = NextItem(&search)) {
	    EventuallyRedrawArea(canvasPtr, itemPtr->x1, itemPtr->y1,
		itemPtr->x2, itemPtr->y2);
	    (void) (*itemPtr->typePtr->scaleProc)(canvasPtr, itemPtr,
		    xOrigin, yOrigin, xScale, yScale);
	    EventuallyRedrawArea(canvasPtr, itemPtr->x1, itemPtr->y1,
		itemPtr->x2, itemPtr->y2);
	    canvasPtr->flags |= REPICK_NEEDED;
	}
    } else if ((c == 's') && (strncmp(argv[1], "scan", length) == 0)
	    && (length >= 3)) {
	int x, y;

	if (argc != 5) {
	    Tcl_AppendResult(interp, "wrong # args: should be \"",
		    argv[0], " scan mark|dragto x y\"", (char *) NULL);
	    goto error;
	}
	if ((Tcl_GetInt(interp, argv[3], &x) != TCL_OK)
		|| (Tcl_GetInt(interp, argv[4], &y) != TCL_OK)){
	    goto error;
	}
	if ((argv[2][0] == 'm')
		&& (strncmp(argv[2], "mark", strlen(argv[2])) == 0)) {
	    canvasPtr->scanX = x;
	    canvasPtr->scanXOrigin = canvasPtr->xOrigin;
	    canvasPtr->scanY = y;
	    canvasPtr->scanYOrigin = canvasPtr->yOrigin;
	} else if ((argv[2][0] == 'd')
		&& (strncmp(argv[2], "dragto", strlen(argv[2])) == 0)) {
	    int newXOrigin, newYOrigin, tmp;

	    /*
	     * Compute a new view origin for the canvas, amplifying the
	     * mouse motion and rounding to the nearest multiple of the
	     * scroll increment.
	     */

	    tmp = canvasPtr->scanXOrigin - 10*(x - canvasPtr->scanX)
		    - canvasPtr->scrollX1;
	    if (tmp >= 0) {
		tmp = (tmp + canvasPtr->scrollIncrement/2)
			/canvasPtr->scrollIncrement;
	    } else {
		tmp = -(((-tmp) + canvasPtr->scrollIncrement/2)
			/canvasPtr->scrollIncrement);
	    }
	    newXOrigin = canvasPtr->scrollX1 + tmp*canvasPtr->scrollIncrement;
	    tmp = canvasPtr->scanYOrigin - 10*(y - canvasPtr->scanY)
		    - canvasPtr->scrollY1;
	    if (tmp >= 0) {
		tmp = (tmp + canvasPtr->scrollIncrement/2)
			/canvasPtr->scrollIncrement;
	    } else {
		tmp = -(((-tmp) + canvasPtr->scrollIncrement/2)
			/canvasPtr->scrollIncrement);
	    }
	    newYOrigin = canvasPtr->scrollY1 + tmp*canvasPtr->scrollIncrement;
	    CanvasSetOrigin(canvasPtr, newXOrigin, newYOrigin);
	} else {
	    Tcl_AppendResult(interp, "bad scan option \"", argv[2],
		    "\":  must be mark or dragto", (char *) NULL);
	    goto error;
	}
    } else if ((c == 's') && (strncmp(argv[1], "select", length) == 0)
	    && (length >= 2)) {
	int index;

	if (argc < 3) {
	    Tcl_AppendResult(interp, "wrong # args: should be \"",
		    argv[0], " select option ?tagOrId? ?arg?\"", (char *) NULL);
	    goto error;
	}
	if (argc >= 4) {
	    for (itemPtr = StartTagSearch(canvasPtr, argv[3], &search);
		    itemPtr != NULL; itemPtr = NextItem(&search)) {
		if ((itemPtr->typePtr->indexProc != NULL)
			&& (itemPtr->typePtr->selectionProc != NULL)){
		    break;
		}
	    }
	    if (itemPtr == NULL) {
		Tcl_AppendResult(interp,
			"can't find an indexable and selectable item \"",
			argv[3], "\"", (char *) NULL);
		goto error;
	    }
	}
	if (argc == 5) {
	    if ((*itemPtr->typePtr->indexProc)(canvasPtr, itemPtr,
		    argv[4], &index) != TCL_OK) {
		goto error;
	    }
	}
	length = strlen(argv[2]);
	c = argv[2][0];
	if ((c == 'a') && (strncmp(argv[2], "adjust", length) == 0)) {
	    if (argc != 5) {
		Tcl_AppendResult(interp, "wrong # args: should be \"",
			argv[0], " select adjust tagOrId index\"",
			(char *) NULL);
		goto error;
	    }
	    if (canvasPtr->selItemPtr == itemPtr) {
		if (index < (canvasPtr->selectFirst
			+ canvasPtr->selectLast)/2) {
		    canvasPtr->selectAnchor = canvasPtr->selectLast + 1;
		} else {
		    canvasPtr->selectAnchor = canvasPtr->selectFirst;
		}
	    }
	    CanvasSelectTo(canvasPtr, itemPtr, index);
	} else if ((c == 'c') && (argv[2] != NULL)
		&& (strncmp(argv[2], "clear", length) == 0)) {
	    if (argc != 3) {
		Tcl_AppendResult(interp, "wrong # args: should be \"",
			argv[0], " select clear\"", (char *) NULL);
		goto error;
	    }
	    if (canvasPtr->selItemPtr != NULL) {
		EventuallyRedrawArea(canvasPtr, canvasPtr->selItemPtr->x1,
		    canvasPtr->selItemPtr->y1, canvasPtr->selItemPtr->x2,
		    canvasPtr->selItemPtr->y2);
		canvasPtr->selItemPtr = NULL;
	    }
	    goto done;
	} else if ((c == 'f') && (strncmp(argv[2], "from", length) == 0)) {
	    if (argc != 5) {
		Tcl_AppendResult(interp, "wrong # args: should be \"",
			argv[0], " select from tagOrId index\"",
			(char *) NULL);
		goto error;
	    }
	    canvasPtr->anchorItemPtr = itemPtr;
	    canvasPtr->selectAnchor = index;
	} else if ((c == 'i') && (strncmp(argv[2], "item", length) == 0)) {
	    if (argc != 3) {
		Tcl_AppendResult(interp, "wrong # args: should be \"",
			argv[0], " select item\"", (char *) NULL);
		goto error;
	    }
	    if (canvasPtr->selItemPtr != NULL) {
		sprintf(interp->result, "%d", canvasPtr->selItemPtr->id);
	    }
	} else if ((c == 't') && (strncmp(argv[2], "to", length) == 0)) {
	    if (argc != 5) {
		Tcl_AppendResult(interp, "wrong # args: should be \"",
			argv[0], " select to tagOrId index\"",
			(char *) NULL);
		goto error;
	    }
	    CanvasSelectTo(canvasPtr, itemPtr, index);
	} else {
	    Tcl_AppendResult(interp, "bad select option \"", argv[2],
		    "\": must be adjust, clear, from, item, or to",
		    (char *) NULL);
	    goto error;
	}
    } else if ((c == 't') && (strncmp(argv[1], "type", length) == 0)) {
	if (argc != 3) {
	    Tcl_AppendResult(interp, "wrong # args: should be \"",
		    argv[0], " type tag\"", (char *) NULL);
	    goto error;
	}
	itemPtr = StartTagSearch(canvasPtr, argv[2], &search);
	if (itemPtr != NULL) {
	    interp->result = itemPtr->typePtr->name;
	}
    } else if ((c == 'x') && (strncmp(argv[1], "xview", length) == 0)) {
	int index;

	if (argc != 3) {
	    Tcl_AppendResult(interp, "wrong # args: should be \"",
		    argv[0], " xview index\"", (char *) NULL);
	    goto error;
	}
	if (Tcl_GetInt(canvasPtr->interp, argv[2], &index) != TCL_OK) {
	    goto error;
	}
	CanvasSetOrigin(canvasPtr,
		(canvasPtr->scrollX1 + index*canvasPtr->scrollIncrement),
		canvasPtr->yOrigin);
    } else if ((c == 'y') && (strncmp(argv[1], "yview", length) == 0)) {
	int index;

	if (argc != 3) {
	    Tcl_AppendResult(interp, "wrong # args: should be \"",
		    argv[0], " yview index\"", (char *) NULL);
	    goto error;
	}
	if (Tcl_GetInt(canvasPtr->interp, argv[2], &index) != TCL_OK) {
	    goto error;
	}
	CanvasSetOrigin(canvasPtr, canvasPtr->xOrigin,
		(canvasPtr->scrollY1 + index*canvasPtr->scrollIncrement));
    } else {
	Tcl_AppendResult(interp, "bad option \"", argv[1],
		"\":  must be addtag, bbox, bind, ",
		"canvasx, canvasy, configure, coords, create, ",
		"cursor, dchars, delete, dtag, find, focus, ",
		"gettags, index, insert, itemconfigure, lower, ",
		"move, raise, scale, scan, select, type, xview, or yview",
		(char *) NULL);  
	goto error;
    }
    done:
    Tk_Release((ClientData) canvasPtr);
    return result;

    error:
    Tk_Release((ClientData) canvasPtr);
    return TCL_ERROR;
}

/*
 *----------------------------------------------------------------------
 *
 * DestroyCanvas --
 *
 *	This procedure is invoked by Tk_EventuallyFree or Tk_Release
 *	to clean up the internal structure of a canvas at a safe time
 *	(when no-one is using it anymore).
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Everything associated with the canvas is freed up.
 *
 *----------------------------------------------------------------------
 */

static void
DestroyCanvas(clientData)
    ClientData clientData;	/* Info about canvas widget. */
{
    register Tk_Canvas *canvasPtr = (Tk_Canvas *) clientData;
    register Tk_Item *itemPtr;

    for (itemPtr = canvasPtr->firstItemPtr; itemPtr != NULL;
	    itemPtr = canvasPtr->firstItemPtr) {
	canvasPtr->firstItemPtr = itemPtr->nextPtr;
	(*itemPtr->typePtr->deleteProc)(itemPtr);
	if (itemPtr->tagPtr != itemPtr->staticTagSpace) {
	    ckfree((char *) itemPtr->tagPtr);
	}
	ckfree((char *) itemPtr);
    }

    if (canvasPtr->bgBorder != NULL) {
	Tk_Free3DBorder(canvasPtr->bgBorder);
    }
    if (canvasPtr->bgColor != NULL) {
	Tk_FreeColor(canvasPtr->bgColor);
    }
    if (canvasPtr->pixmapGC != None) {
	Tk_FreeGC(canvasPtr->pixmapGC);
    }
    if (canvasPtr->selBorder != NULL) {
	Tk_Free3DBorder(canvasPtr->selBorder);
    }
    if (canvasPtr->selFgColorPtr != NULL) {
	Tk_FreeColor(canvasPtr->selFgColorPtr);
    }
    if (canvasPtr->cursorBorder != NULL) {
	Tk_Free3DBorder(canvasPtr->cursorBorder);
    }
    Tk_DeleteTimerHandler(canvasPtr->cursorBlinkHandler);
    if (canvasPtr->bindingTable != NULL) {
	Tk_DeleteBindingTable(canvasPtr->bindingTable);
    }
    if (canvasPtr->xScrollCmd != NULL) {
	ckfree(canvasPtr->xScrollCmd);
    }
    if (canvasPtr->yScrollCmd != NULL) {
	ckfree(canvasPtr->yScrollCmd);
    }
    if (canvasPtr->regionString != NULL) {
	ckfree(canvasPtr->regionString);
    }
    if (canvasPtr->cursor != None) {
	Tk_FreeCursor(canvasPtr->cursor);
    }
    ckfree((char *) canvasPtr);
}

/*
 *----------------------------------------------------------------------
 *
 * ConfigureCanvas --
 *
 *	This procedure is called to process an argv/argc list, plus
 *	the Tk option database, in order to configure (or
 *	reconfigure) a canvas widget.
 *
 * Results:
 *	The return value is a standard Tcl result.  If TCL_ERROR is
 *	returned, then interp->result contains an error message.
 *
 * Side effects:
 *	Configuration information, such as colors, border width,
 *	etc. get set for canvasPtr;  old resources get freed,
 *	if there were any.
 *
 *----------------------------------------------------------------------
 */

static int
ConfigureCanvas(interp, canvasPtr, argc, argv, flags)
    Tcl_Interp *interp;		/* Used for error reporting. */
    register Tk_Canvas *canvasPtr;	/* Information about widget;  may or may
				 * not already have values for some fields. */
    int argc;			/* Number of valid entries in argv. */
    char **argv;		/* Arguments. */
    int flags;			/* Flags to pass to Tk_ConfigureWidget. */
{
    XGCValues gcValues;
    GC new;

    if (Tk_ConfigureWidget(interp, canvasPtr->tkwin, configSpecs,
	    argc, argv, (char *) canvasPtr, flags) != TCL_OK) {
	return TCL_ERROR;
    }

    /*
     * A few options need special processing, such as setting the
     * background from a 3-D border and creating a GC for copying
     * bits to the screen.
     */

    Tk_SetBackgroundFromBorder(canvasPtr->tkwin, canvasPtr->bgBorder);

    gcValues.function = GXcopy;
    gcValues.foreground = canvasPtr->bgColor->pixel;
    gcValues.graphics_exposures = False;
    new = Tk_GetGC(canvasPtr->tkwin,
	    GCFunction|GCForeground|GCGraphicsExposures, &gcValues);
    if (canvasPtr->pixmapGC != None) {
	Tk_FreeGC(canvasPtr->pixmapGC);
    }
    canvasPtr->pixmapGC = new;

    /*
     * Reset the desired dimensions for the window.
     */

    Tk_GeometryRequest(canvasPtr->tkwin, canvasPtr->width, canvasPtr->height);

    /*
     * Restart the cursor timing sequence in case the on-time or off-time
     * just changed.
     */

    if (canvasPtr->flags & GOT_FOCUS) {
	CanvasFocusProc((ClientData) canvasPtr, 1);
    }

    /*
     * Recompute the scroll region.
     */

    canvasPtr->scrollX1 = 0;
    canvasPtr->scrollY1 = 0;
    canvasPtr->scrollX2 = 0;
    canvasPtr->scrollY2 = 0;
    if (canvasPtr->regionString != NULL) {
	int argc2;
	char **argv2;

	if (Tcl_SplitList(canvasPtr->interp, canvasPtr->regionString,
		&argc2, &argv2) != TCL_OK) {
	    return TCL_ERROR;
	}
	if (argc2 != 4) {
	    badRegion:
	    Tcl_AppendResult(interp, "bad scrollRegion \"",
		    canvasPtr->regionString, "\"", (char *) NULL);
	    ckfree(canvasPtr->regionString);
	    ckfree((char *) argv2);
	    canvasPtr->regionString = NULL;
	    return TCL_ERROR;
	}
	if ((Tk_GetPixels(canvasPtr->interp, canvasPtr->tkwin,
		    argv2[0], &canvasPtr->scrollX1) != TCL_OK)
		|| (Tk_GetPixels(canvasPtr->interp, canvasPtr->tkwin,
		    argv2[1], &canvasPtr->scrollY1) != TCL_OK)
		|| (Tk_GetPixels(canvasPtr->interp, canvasPtr->tkwin,
		    argv2[2], &canvasPtr->scrollX2) != TCL_OK)
		|| (Tk_GetPixels(canvasPtr->interp, canvasPtr->tkwin,
		    argv2[3], &canvasPtr->scrollY2) != TCL_OK)) {
	    goto badRegion;
	}
	ckfree((char *) argv2);
    }

    /*
     * Reset the canvases origin (this is a no-op unless confine
     * mode has just been turned on or the scroll region has changed).
     */

    CanvasSetOrigin(canvasPtr, canvasPtr->xOrigin, canvasPtr->yOrigin);
    canvasPtr->flags |= UPDATE_SCROLLBARS;
    EventuallyRedrawArea(canvasPtr, canvasPtr->xOrigin, canvasPtr->yOrigin,
	    canvasPtr->xOrigin + Tk_Width(canvasPtr->tkwin),
	    canvasPtr->yOrigin + Tk_Height(canvasPtr->tkwin));
    return TCL_OK;
}

#if defined(USE_XPM3)
//#include "xpmtk.h"
#include <xpm.h>
/*
 *--------------------------------------------------------------
 *
 * SaveCanvas --
 *
 *	This procedure saves the contents of a canvas window.
 *
 * Results:
 *	The return value is a standard Tcl result.  If TCL_ERROR is
 *	returned, then interp->result contains an error message.
 *
 * Side effects:
 *	A pixmap is written to a file.
 *
 *--------------------------------------------------------------
 */

static int
SaveCanvas(interp, canvasPtr, fileName, x, y, width, height)
    Tcl_Interp *interp;		/* Used for error reporting. */
    register Tk_Canvas *canvasPtr;	/* Information about widget */
    char *fileName;             /* the output file name. */
    int x;                      /* upper left x coordinate. */
    int y;                      /* upper left y coordinate. */
    unsigned int width;         /* width of pixmap area to save. */
    unsigned int height;        /* height of pixmap area to save. */
{
    register Tk_Window tkwin = canvasPtr->tkwin;
    register Tk_Item *itemPtr;
    Pixmap pixmap;
    Pixmap savePixmap;
    int screenX1, screenX2, screenY1, screenY2;
    XpmAttributes xpm_attributes;

    if (canvasPtr->tkwin == NULL) {
	return TCL_OK;
    }
    if (!Tk_IsMapped(tkwin)) {
	return TCL_OK;
    }
    if (!(fileName && *fileName)) {
	Tcl_ResetResult(interp);
	Tcl_AppendResult(interp, "no filename specified for canvas saving",
		(char *) NULL);
	return TCL_ERROR;
    }

    /*
     * Choose a new current item if that is needed (this could cause
     * event handlers to be invoked).
     */

    while (canvasPtr->flags & REPICK_NEEDED) {
	Tk_Preserve((ClientData) canvasPtr);
	canvasPtr->flags &= ~REPICK_NEEDED;
	PickCurrentItem(canvasPtr, &canvasPtr->pickEvent);
	tkwin = canvasPtr->tkwin;
	Tk_Release((ClientData) canvasPtr);
	if (tkwin == NULL) {
	    return TCL_OK;
	}
    }

    if(x == 0 && y == 0 && width == 0 && height == 0) {
      screenX1 = 0;
      screenY1 = 0;
      screenX2 = Tk_Width(tkwin);
      screenY2 = Tk_Height(tkwin);
      width = Tk_Width(tkwin);
      height = Tk_Height(tkwin);
    } else {
      if(width != 0 && height != 0) {
        screenX1 = x;
        screenY1 = y;
        screenX2 = x + width;
        screenY2 = y + height;
      } else {
	Tcl_ResetResult(interp);
	Tcl_AppendResult(interp, "no correct size specified for canvas saving",
		(char *) NULL);
        return TCL_ERROR;
      }
    }

    /*
     * Saving is done in a temporary pixmap that is allocated
     * here and freed at the end of the procedure.  All drawing
     * is done to the pixmap, and the pixmap is saved to the
     * file at the end of the procedure.
     *
     * Some tricky points about the pixmap:
     *
     * 1. We only allocate a large enough pixmap to hold the
     *    area that has to be saved.  This saves time in
     *    in the X server for large objects that cover much
     *    more than the area being saved:  only the area
     *    of the pixmap will actually have to be saved.
     * 2. The origin of the pixmap is adjusted to an even multiple
     *    of 32 bits.  This is so that stipple patterns with a size
     *    of 8 or 16 or 32 bits will always line up when information
     *    is copied back to the screen.
     * 3. Some X servers (e.g. the one for DECstations) have troubles
     *    with characters that overlap an edge of the pixmap (on the
     *    DEC servers, as of 8/18/92, such characters are drawn one
     *    pixel too far to the right).  To handle this problem,
     *    make the pixmap a bit larger than is absolutely needed
     *    so that for normal-sized fonts the characters that ovelap
     *    the edge of the pixmap will be outside the area we care
     *    about.
     */

    canvasPtr->drawableXOrigin = (screenX1 - 30) & ~0x1f;
    canvasPtr->drawableYOrigin = (screenY1 - 30) & ~0x1f;
    pixmap = XCreatePixmap(Tk_Display(tkwin), Tk_WindowId(tkwin),
	screenX2 + 30 - canvasPtr->drawableXOrigin,
	screenY2 + 30 - canvasPtr->drawableYOrigin,
	Tk_DefaultDepth(Tk_Screen(tkwin)));
    savePixmap = XCreatePixmap(Tk_Display(tkwin), Tk_WindowId(tkwin),
	width, height, Tk_DefaultDepth(Tk_Screen(tkwin)));

    /*
     * Clear the area to be redrawn.
     */

    XFillRectangle(Tk_Display(tkwin), pixmap, canvasPtr->pixmapGC,
	    screenX1 - canvasPtr->drawableXOrigin,
	    screenY1 - canvasPtr->drawableYOrigin,
	    (unsigned int) (screenX2 - screenX1),
	    (unsigned int) (screenY2 - screenY1));
    XFillRectangle(Tk_Display(tkwin), savePixmap, canvasPtr->pixmapGC,
	    0, 0, width, height);

    /*
     * Scan through the item list, redrawing those items that need it.
     * An item must be redraw if either (a) it intersects the smaller
     * on-screen area or (b) it intersects the full canvas area and its
     * type requests that it be redrawn always (e.g. so subwindows can
     * be unmapped when they move off-screen).
     */

    for (itemPtr = canvasPtr->firstItemPtr; itemPtr != NULL;
	    itemPtr = itemPtr->nextPtr) {
	if ((itemPtr->x1 >= screenX2)
		|| (itemPtr->y1 >= screenY2)
		|| (itemPtr->x2 < screenX1)
		|| (itemPtr->y2 < screenY1)) {
	    if (!itemPtr->typePtr->alwaysRedraw
		    || (itemPtr->x1 >= canvasPtr->redrawX2)
		    || (itemPtr->y1 >= canvasPtr->redrawY2)
		    || (itemPtr->x2 < canvasPtr->redrawX1)
		    || (itemPtr->y2 < canvasPtr->redrawY1)) {
		continue;
	    }
	}
	(*itemPtr->typePtr->displayProc)(canvasPtr, itemPtr, pixmap);
    }

    /*
     * Copy from the temporary pixmap to the save pixmap.
     */

    XCopyArea(Tk_Display(tkwin), pixmap, savePixmap,
	    canvasPtr->pixmapGC,
	    screenX1 - canvasPtr->drawableXOrigin,
	    screenY1 - canvasPtr->drawableYOrigin,
	    screenX2 - screenX1, screenY2 - screenY1, 0, 0);

    /*
     * Save temporary pixmap.
     */

    xpm_attributes.width = width;
    xpm_attributes.height = height;
    xpm_attributes.visual = Tk_DefaultVisual(Tk_Screen(tkwin));
    xpm_attributes.colormap = Tk_DefaultColormap(Tk_Screen(tkwin));
    xpm_attributes.valuemask = XpmSize | XpmVisual | XpmColormap;
    if(XpmWriteFileFromPixmap(Tk_Display(tkwin), fileName,
			      savePixmap, (Pixmap) NULL,
			      &xpm_attributes) != XpmSuccess) {
      XFreePixmap(Tk_Display(tkwin), pixmap);
      XFreePixmap(Tk_Display(tkwin), savePixmap);
      Tcl_ResetResult(interp);
      Tcl_AppendResult(interp, "could not save pixmap for canvas",
	    (char *) NULL);
      return TCL_ERROR;
    }
    XFreePixmap(Tk_Display(tkwin), pixmap);
    XFreePixmap(Tk_Display(tkwin), savePixmap);

    return TCL_OK;
}
#endif

/*
 *--------------------------------------------------------------
 *
 * DisplayCanvas --
 *
 *	This procedure redraws the contents of a canvas window.
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
DisplayCanvas(clientData)
    ClientData clientData;	/* Information about widget. */
{
    register Tk_Canvas *canvasPtr = (Tk_Canvas *) clientData;
    register Tk_Window tkwin = canvasPtr->tkwin;
    register Tk_Item *itemPtr;
    Pixmap pixmap;
    int screenX1, screenX2, screenY1, screenY2;

    if (canvasPtr->tkwin == NULL) {
	return;
    }
    if (!Tk_IsMapped(tkwin)) {
	goto done;
    }

    /*
     * Choose a new current item if that is needed (this could cause
     * event handlers to be invoked).
     */

    while (canvasPtr->flags & REPICK_NEEDED) {
	Tk_Preserve((ClientData) canvasPtr);
	canvasPtr->flags &= ~REPICK_NEEDED;
	PickCurrentItem(canvasPtr, &canvasPtr->pickEvent);
	tkwin = canvasPtr->tkwin;
	Tk_Release((ClientData) canvasPtr);
	if (tkwin == NULL) {
	    return;
	}
    }

    /*
     * Compute the intersection between the area that needs redrawing
     * and the area that's visible on the screen.
     */

    screenX1 = canvasPtr->xOrigin;
    screenY1 = canvasPtr->yOrigin;
    screenX2 = screenX1 + Tk_Width(tkwin);
    screenY2 = screenY1 + Tk_Height(tkwin);
    if (canvasPtr->redrawX1 > screenX1) {
	screenX1 = canvasPtr->redrawX1;
    }
    if (canvasPtr->redrawY1 > screenY1) {
	screenY1 = canvasPtr->redrawY1;
    }
    if (canvasPtr->redrawX2 < screenX2) {
	screenX2 = canvasPtr->redrawX2;
    }
    if (canvasPtr->redrawY2 < screenY2) {
	screenY2 = canvasPtr->redrawY2;
    }
    if ((screenX1 >= screenX2) || (screenY1 >= screenY2)) {
	goto done;
    }

    /*
     * Redrawing is done in a temporary pixmap that is allocated
     * here and freed at the end of the procedure.  All drawing
     * is done to the pixmap, and the pixmap is copied to the
     * screen at the end of the procedure. The temporary pixmap
     * serves two purposes:
     *
     * 1. It provides a smoother visual effect (no clearing and
     *    gradual redraw will be visible to users).
     * 2. It allows us to redraw only the objects that overlap
     *    the redraw area.  Otherwise incorrect results could
     *	  occur from redrawing things that stick outside of
     *	  the redraw area (we'd have to redraw everything in
     *    order to make the overlaps look right).
     *
     * Some tricky points about the pixmap:
     *
     * 1. We only allocate a large enough pixmap to hold the
     *    area that has to be redisplayed.  This saves time in
     *    in the X server for large objects that cover much
     *    more than the area being redisplayed:  only the area
     *    of the pixmap will actually have to be redrawn.
     * 2. The origin of the pixmap is adjusted to an even multiple
     *    of 32 bits.  This is so that stipple patterns with a size
     *    of 8 or 16 or 32 bits will always line up when information
     *    is copied back to the screen.
     * 3. Some X servers (e.g. the one for DECstations) have troubles
     *    with characters that overlap an edge of the pixmap (on the
     *    DEC servers, as of 8/18/92, such characters are drawn one
     *    pixel too far to the right).  To handle this problem,
     *    make the pixmap a bit larger than is absolutely needed
     *    so that for normal-sized fonts the characters that ovelap
     *    the edge of the pixmap will be outside the area we care
     *    about.
     */

    canvasPtr->drawableXOrigin = (screenX1 - 30) & ~0x1f;
    canvasPtr->drawableYOrigin = (screenY1 - 30) & ~0x1f;
    pixmap = XCreatePixmap(Tk_Display(tkwin), Tk_WindowId(tkwin),
	screenX2 + 30 - canvasPtr->drawableXOrigin,
	screenY2 + 30 - canvasPtr->drawableYOrigin,
	Tk_DefaultDepth(Tk_Screen(tkwin)));

    /*
     * Clear the area to be redrawn.
     */

    XFillRectangle(Tk_Display(tkwin), pixmap, canvasPtr->pixmapGC,
	    screenX1 - canvasPtr->drawableXOrigin,
	    screenY1 - canvasPtr->drawableYOrigin,
	    (unsigned int) (screenX2 - screenX1),
	    (unsigned int) (screenY2 - screenY1));

    /*
     * Scan through the item list, redrawing those items that need it.
     * An item must be redraw if either (a) it intersects the smaller
     * on-screen area or (b) it intersects the full canvas area and its
     * type requests that it be redrawn always (e.g. so subwindows can
     * be unmapped when they move off-screen).
     */

    for (itemPtr = canvasPtr->firstItemPtr; itemPtr != NULL;
	    itemPtr = itemPtr->nextPtr) {
	if ((itemPtr->x1 >= screenX2)
		|| (itemPtr->y1 >= screenY2)
		|| (itemPtr->x2 < screenX1)
		|| (itemPtr->y2 < screenY1)) {
	    if (!itemPtr->typePtr->alwaysRedraw
		    || (itemPtr->x1 >= canvasPtr->redrawX2)
		    || (itemPtr->y1 >= canvasPtr->redrawY2)
		    || (itemPtr->x2 < canvasPtr->redrawX1)
		    || (itemPtr->y2 < canvasPtr->redrawY1)) {
		continue;
	    }
	}
	(*itemPtr->typePtr->displayProc)(canvasPtr, itemPtr, pixmap);
    }

    /*
     * Draw the window border.
     */

    if (canvasPtr->relief != TK_RELIEF_FLAT) {
	Tk_Draw3DRectangle(Tk_Display(tkwin), pixmap,
		canvasPtr->bgBorder,
		canvasPtr->xOrigin - canvasPtr->drawableXOrigin,
		canvasPtr->yOrigin - canvasPtr->drawableYOrigin,
		Tk_Width(tkwin), Tk_Height(tkwin),
		canvasPtr->borderWidth, canvasPtr->relief);
    }

    /*
     * Copy from the temporary pixmap to the screen, then free up
     * the temporary pixmap.
     */

    XCopyArea(Tk_Display(tkwin), pixmap, Tk_WindowId(tkwin),
	    canvasPtr->pixmapGC,
	    screenX1 - canvasPtr->drawableXOrigin,
	    screenY1 - canvasPtr->drawableYOrigin,
	    screenX2 - screenX1, screenY2 - screenY1,
	    screenX1 - canvasPtr->xOrigin, screenY1 - canvasPtr->yOrigin);
    XFreePixmap(Tk_Display(tkwin), pixmap);

    done:
    canvasPtr->flags &= ~REDRAW_PENDING;
    assert(canvasPtr->updateTimerToken != NULL);
    canvasPtr->updateTimerToken = NULL;
    if (canvasPtr->flags & UPDATE_SCROLLBARS) {
	CanvasUpdateScrollbars(canvasPtr);
    }
}

/*
 *--------------------------------------------------------------
 *
 * CanvasEventProc --
 *
 *	This procedure is invoked by the Tk dispatcher for various
 *	events on canvases.
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
CanvasEventProc(clientData, eventPtr)
    ClientData clientData;	/* Information about window. */
    XEvent *eventPtr;		/* Information about event. */
{
    Tk_Canvas *canvasPtr = (Tk_Canvas *) clientData;

    if (eventPtr->type == Expose) {
	int x, y;

	x = eventPtr->xexpose.x + canvasPtr->xOrigin;
	y = eventPtr->xexpose.y + canvasPtr->yOrigin;
	EventuallyRedrawArea(canvasPtr, x, y, x + eventPtr->xexpose.width,
		y + eventPtr->xexpose.height);
    } else if (eventPtr->type == DestroyNotify) {
	Tcl_DeleteCommand(canvasPtr->interp, Tk_PathName(canvasPtr->tkwin));
	canvasPtr->tkwin = NULL;
	if (canvasPtr->flags & REDRAW_PENDING) {
	    canvasPtr->flags &= ~REDRAW_PENDING;
//	    Tk_CancelIdleCall(DisplayCanvas, (ClientData) canvasPtr);
	    assert(canvasPtr->updateTimerToken != NULL);
	    if (canvasPtr->updateTimerToken != NULL) {
	        Tk_DeleteTimerHandler(canvasPtr->updateTimerToken);
	        canvasPtr->updateTimerToken = 0;
	    }
	}
	Tk_EventuallyFree((ClientData) canvasPtr, DestroyCanvas);
    } else if (eventPtr->type == ConfigureNotify) {
	canvasPtr->flags |= UPDATE_SCROLLBARS;

	/*
	 * The call below is needed in order to recenter the canvas if
	 * it's confined and its scroll region is smaller than the window.
	 */

	CanvasSetOrigin(canvasPtr, canvasPtr->xOrigin, canvasPtr->yOrigin);
	EventuallyRedrawArea(canvasPtr, 0, 0, Tk_Width(canvasPtr->tkwin),
		Tk_Height(canvasPtr->tkwin));
    }
}

/*
 *--------------------------------------------------------------
 *
 * EventuallyRedrawArea --
 *
 *	Arrange for part or all of a canvas widget to redrawn at
 *	the next convenient time in the future.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The screen will eventually be refreshed.
 *
 *--------------------------------------------------------------
 */

static void
EventuallyRedrawArea(canvasPtr, x1, y1, x2, y2)
    register Tk_Canvas *canvasPtr;	/* Information about widget. */
    int x1, y1;				/* Upper left corner of area to
					 * redraw.  Pixels on edge are
					 * redrawn. */
    int x2, y2;				/* Lower right corner of area to
					 * redraw.  Pixels on edge are
					 * not redrawn. */
{
    if ((canvasPtr->tkwin == NULL) || !Tk_IsMapped(canvasPtr->tkwin)) {
	return;
    }
    if (canvasPtr->flags & REDRAW_PENDING) {
	if (x1 <= canvasPtr->redrawX1) {
	    canvasPtr->redrawX1 = x1;
	}
	if (y1 <= canvasPtr->redrawY1) {
	    canvasPtr->redrawY1 = y1;
	}
	if (x2 >= canvasPtr->redrawX2) {
	    canvasPtr->redrawX2 = x2;
	}
	if (y2 >= canvasPtr->redrawY2) {
	    canvasPtr->redrawY2 = y2;
	}
    } else {
	canvasPtr->redrawX1 = x1;
	canvasPtr->redrawY1 = y1;
	canvasPtr->redrawX2 = x2;
	canvasPtr->redrawY2 = y2;
//	Tk_DoWhenIdle(DisplayCanvas, (ClientData) canvasPtr);
	canvasPtr->flags |= REDRAW_PENDING;
	assert(canvasPtr->updateTimerToken == NULL);
	if (canvasPtr->updateTimerToken == 0) {
	  canvasPtr->updateTimerToken = 
	    Tk_CreateTimerHandler(
	      CanvasUpdateTime,
	      DisplayCanvas,
	      (ClientData) canvasPtr);
	}
    }
}

/*
 *--------------------------------------------------------------
 *
 * Tk_CreateItemType --
 *
 *	This procedure may be invoked to add a new kind of canvas
 *	element to the core item types supported by Tk.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	From now on, the new item type will be useable in canvas
 *	widgets (e.g. typePtr->name can be used as the item type
 *	in "create" widget commands).  If there was already a
 *	type with the same name as in typePtr, it is replaced with
 *	the new type.
 *
 *--------------------------------------------------------------
 */

void
Tk_CreateItemType(typePtr)
    Tk_ItemType *typePtr;		/* Information about item type;
					 * storage must be statically
					 * allocated (must live forever). */
{
    if (typeList == NULL) {
	InitCanvas();
    }
    typePtr->nextPtr = typeList;
    typeList = typePtr;
}

/*
 *--------------------------------------------------------------
 *
 * InitCanvas --
 *
 *	This procedure is invoked to perform once-only-ever
 *	initialization for the module, such as setting up
 *	the type table.
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
InitCanvas()
{
    if (typeList != NULL) {
	return;
    }
    typeList = &TkRectangleType;
    TkRectangleType.nextPtr = &TkTextType;
    TkTextType.nextPtr = &TkPolygonType;
    TkPolygonType.nextPtr = &TkOvalType;
    TkOvalType.nextPtr = &TkLineType;
    TkLineType.nextPtr = &TkWindowType;
    TkWindowType.nextPtr = &TkBitmapType;
    TkBitmapType.nextPtr = &TkArcType;
    TkArcType.nextPtr = NULL;
    allUid = Tk_GetUid("all");
    currentUid = Tk_GetUid("current");
}

/*
 *--------------------------------------------------------------
 *
 * StartTagSearch --
 *
 *	This procedure is called to initiate an enumeration of
 *	all items in a given canvas that contain a given tag.
 *
 * Results:
 *	The return value is a pointer to the first item in
 *	canvasPtr that matches tag, or NULL if there is no
 *	such item.  The information at *searchPtr is initialized
 *	such that successive calls to NextItem will return
 *	successive items that match tag.
 *
 * Side effects:
 *	SearchPtr is linked into a list of searches in progress
 *	on canvasPtr, so that elements can safely be deleted
 *	while the search is in progress.  EndTagSearch must be
 *	called at the end of the search to unlink searchPtr from
 *	this list.
 *
 *--------------------------------------------------------------
 */

static Tk_Item *
StartTagSearch(canvasPtr, tag, searchPtr)
    Tk_Canvas *canvasPtr;		/* Canvas whose items are to be
					 * searched. */
    char *tag;				/* String giving tag value. */
    TagSearch *searchPtr;		/* Record describing tag search;
					 * will be initialized here. */
{
    int id;
    register Tk_Item *itemPtr, *prevPtr;
    register Tk_Uid *tagPtr;
    register Tk_Uid uid;
    register int count;

    /*
     * Initialize the search.
     */

    searchPtr->canvasPtr = canvasPtr;
    searchPtr->searchOver = 0;

    /*
     * Find the first matching item in one of several ways. If the tag
     * is a number then it selects the single item with the matching
     * identifier.  In this case see if the item being requested is the
     * hot item, in which case the search can be skipped.
     */

    if (isdigit(*tag)) {
	char *end;

	numIdSearches++;
	id = strtoul(tag, &end, 0);
	if (*end == 0) {
	    itemPtr = canvasPtr->hotPtr;
	    prevPtr = canvasPtr->hotPrevPtr;
	    if ((itemPtr == NULL) || (itemPtr->id != id) || (prevPtr == NULL)
		    || (prevPtr->nextPtr != itemPtr)) {
		numSlowSearches++;
		for (prevPtr = NULL, itemPtr = canvasPtr->firstItemPtr;
			itemPtr != NULL;
			prevPtr = itemPtr, itemPtr = itemPtr->nextPtr) {
		    if (itemPtr->id == id) {
			break;
		    }
		}
	    }
	    searchPtr->prevPtr = prevPtr;
	    searchPtr->searchOver = 1;
	    canvasPtr->hotPtr = itemPtr;
	    canvasPtr->hotPrevPtr = prevPtr;
	    return itemPtr;
	}
    }

    searchPtr->tag = uid = Tk_GetUid(tag);
    if (uid == allUid) {

	/*
	 * All items match.
	 */

	searchPtr->tag = NULL;
	searchPtr->prevPtr = NULL;
	searchPtr->currentPtr = canvasPtr->firstItemPtr;
	return canvasPtr->firstItemPtr;
    }

    /*
     * None of the above.  Search for an item with a matching tag.
     */

    for (prevPtr = NULL, itemPtr = canvasPtr->firstItemPtr; itemPtr != NULL;
	    prevPtr = itemPtr, itemPtr = itemPtr->nextPtr) {
	for (tagPtr = itemPtr->tagPtr, count = itemPtr->numTags;
		count > 0; tagPtr++, count--) {
	    if (*tagPtr == uid) {
		searchPtr->prevPtr = prevPtr;
		searchPtr->currentPtr = itemPtr;
		return itemPtr;
	    }
	}
    }
    searchPtr->prevPtr = prevPtr;
    searchPtr->searchOver = 1;
    return NULL;
}

/*
 *--------------------------------------------------------------
 *
 * NextItem --
 *
 *	This procedure returns successive items that match a given
 *	tag;  it should be called only after StartTagSearch has been
 *	used to begin a search.
 *
 * Results:
 *	The return value is a pointer to the next item that matches
 *	the tag specified to StartTagSearch, or NULL if no such
 *	item exists.  *SearchPtr is updated so that the next call
 *	to this procedure will return the next item.
 *
 * Side effects:
 *	None.
 *
 *--------------------------------------------------------------
 */

static Tk_Item *
NextItem(searchPtr)
    TagSearch *searchPtr;		/* Record describing search in
					 * progress. */
{
    register Tk_Item *itemPtr, *prevPtr;
    register int count;
    register Tk_Uid uid;
    register Tk_Uid *tagPtr;

    /*
     * Find next item in list (this may not actually be a suitable
     * one to return), and return if there are no items left.
     */

    prevPtr = searchPtr->prevPtr;
    if (prevPtr == NULL) {
	itemPtr = searchPtr->canvasPtr->firstItemPtr;
    } else {
	itemPtr = prevPtr->nextPtr;
    }
    if ((itemPtr == NULL) || (searchPtr->searchOver)) {
	searchPtr->searchOver = 1;
	return NULL;
    }
    if (itemPtr != searchPtr->currentPtr) {
	/*
	 * The structure of the list has changed.  Probably the
	 * previously-returned item was removed from the list.
	 * In this case, don't advance prevPtr;  just return
	 * its new successor (i.e. do nothing here).
	 */
    } else {
	prevPtr = itemPtr;
	itemPtr = prevPtr->nextPtr;
    }

    /*
     * Handle special case of "all" search by returning next item.
     */

    uid = searchPtr->tag;
    if (uid == NULL) {
	searchPtr->prevPtr = prevPtr;
	searchPtr->currentPtr = itemPtr;
	return itemPtr;
    }

    /*
     * Look for an item with a particular tag.
     */

    for ( ; itemPtr != NULL; prevPtr = itemPtr, itemPtr = itemPtr->nextPtr) {
	for (tagPtr = itemPtr->tagPtr, count = itemPtr->numTags;
		count > 0; tagPtr++, count--) {
	    if (*tagPtr == uid) {
		searchPtr->prevPtr = prevPtr;
		searchPtr->currentPtr = itemPtr;
		return itemPtr;
	    }
	}
    }
    searchPtr->prevPtr = prevPtr;
    searchPtr->searchOver = 1;
    return NULL;
}

/*
 *--------------------------------------------------------------
 *
 * DoItem --
 *
 *	This is a utility procedure called by FindItems.  It
 *	either adds itemPtr's id to the result forming in interp,
 *	or it adds a new tag to itemPtr, depending on the value
 *	of tag.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	If tag is NULL then itemPtr's id is added as a list element
 *	to interp->result;  otherwise tag is added to itemPtr's
 *	list of tags.
 *
 *--------------------------------------------------------------
 */

static void
DoItem(interp, itemPtr, tag)
    Tcl_Interp *interp;			/* Interpreter in which to (possibly)
					 * record item id. */
    register Tk_Item *itemPtr;		/* Item to (possibly) modify. */
    Tk_Uid tag;				/* Tag to add to those already
					 * present for item, or NULL. */
{
    register Tk_Uid *tagPtr;
    register int count;

    /*
     * Handle the "add-to-result" case and return, if appropriate.
     */

    if (tag == NULL) {
	char msg[30];
	sprintf(msg, "%d", itemPtr->id);
	Tcl_AppendElement(interp, msg, 0);
	return;
    }

    for (tagPtr = itemPtr->tagPtr, count = itemPtr->numTags;
	    count > 0; tagPtr++, count--) {
	if (tag == *tagPtr) {
	    return;
	}
    }

    /*
     * Grow the tag space if there's no more room left in the current
     * block.
     */

    if (itemPtr->tagSpace == itemPtr->numTags) {
	Tk_Uid *newTagPtr;

	itemPtr->tagSpace += 5;
	newTagPtr = (Tk_Uid *) ckalloc((unsigned)
		(itemPtr->tagSpace * sizeof(Tk_Uid)));
	memcpy((VOID *) newTagPtr, (VOID *) itemPtr->tagPtr,
		(itemPtr->numTags * sizeof(Tk_Uid)));
	if (itemPtr->tagPtr != itemPtr->staticTagSpace) {
	    ckfree((char *) itemPtr->tagPtr);
	}
	itemPtr->tagPtr = newTagPtr;
	tagPtr = &itemPtr->tagPtr[itemPtr->numTags];
    }

    /*
     * Add in the new tag.
     */

    *tagPtr = tag;
    itemPtr->numTags++;
}

/*
 *--------------------------------------------------------------
 *
 * FindItems --
 *
 *	This procedure does all the work of implementing the
 *	"find" and "addtag" options of the canvas widget command,
 *	which locate items that have certain features (location,
 *	tags, position in display list, etc.).
 *
 * Results:
 *	A standard Tcl return value.  If newTag is NULL, then a
 *	list of ids from all the items that match argc/argv is
 *	returned in interp->result.  If newTag is NULL, then
 *	the normal interp->result is an empty string.  If an error
 *	occurs, then interp->result will hold an error message.
 *
 * Side effects:
 *	If newTag is non-NULL, then all the items that match the
 *	information in argc/argv have that tag added to their
 *	lists of tags.
 *
 *--------------------------------------------------------------
 */

static int
FindItems(interp, canvasPtr, argc, argv, newTag, cmdName, option)
    Tcl_Interp *interp;			/* Interpreter for error reporting. */
    Tk_Canvas *canvasPtr;		/* Canvas whose items are to be
					 * searched. */
    int argc;				/* Number of entries in argv.  Must be
					 * greater than zero. */
    char **argv;			/* Arguments that describe what items
					 * to search for (see user doc on
					 * "find" and "addtag" options). */
    char *newTag;			/* If non-NULL, gives new tag to set
					 * on all found items;  if NULL, then
					 * ids of found items are returned
					 * in interp->result. */
    char *cmdName;			/* Name of original Tcl command, for
					 * use in error messages. */
    char *option;			/* For error messages:  gives option
					 * from Tcl command and other stuff
					 * up to what's in argc/argv. */
{
    char c;
    int length;
    TagSearch search;
    register Tk_Item *itemPtr;
    Tk_Uid uid;

    if (newTag != NULL) {
	uid = Tk_GetUid(newTag);
    } else {
	uid = NULL;
    }
    c = argv[0][0];
    length = strlen(argv[0]);
    if ((c == 'a') && (strncmp(argv[0], "above", length) == 0)
	    && (length >= 2)) {
	Tk_Item *lastPtr = NULL;
	if (argc != 2) {
	    Tcl_AppendResult(interp, "wrong # args:  must be \"",
		    cmdName, option, " above tagOrId", (char *) NULL);
	    return TCL_ERROR;
	}
	for (itemPtr = StartTagSearch(canvasPtr, argv[1], &search);
		itemPtr != NULL; itemPtr = NextItem(&search)) {
	    lastPtr = itemPtr;
	}
	if ((lastPtr != NULL) && (lastPtr->nextPtr != NULL)) {
	    DoItem(interp, lastPtr->nextPtr, uid);
	}
    } else if ((c == 'a') && (strncmp(argv[0], "all", length) == 0)
	    && (length >= 2)) {
	if (argc != 1) {
	    Tcl_AppendResult(interp, "wrong # args:  must be \"",
		    cmdName, option, " all", (char *) NULL);
	    return TCL_ERROR;
	}

	for (itemPtr = canvasPtr->firstItemPtr; itemPtr != NULL;
		itemPtr = itemPtr->nextPtr) {
	    DoItem(interp, itemPtr, uid);
	}
    } else if ((c == 'b') && (strncmp(argv[0], "below", length) == 0)) {
	if (argc != 2) {
	    Tcl_AppendResult(interp, "wrong # args:  must be \"",
		    cmdName, option, " below tagOrId", (char *) NULL);
	    return TCL_ERROR;
	}
	itemPtr = StartTagSearch(canvasPtr, argv[1], &search);
	if (search.prevPtr != NULL) {
	    DoItem(interp, search.prevPtr, uid);
	}
    } else if ((c == 'c') && (strncmp(argv[0], "closest", length) == 0)) {
	double closestDist;
	Tk_Item *startPtr, *closestPtr;
	double coords[2], halo;
	int x1, y1, x2, y2;

	if ((argc < 3) || (argc > 5)) {
	    Tcl_AppendResult(interp, "wrong # args:  must be \"",
		    cmdName, option, " closest x y ?halo? ?start?",
		    (char *) NULL);
	    return TCL_ERROR;
	}
	if ((TkGetCanvasCoord(canvasPtr, argv[1], &coords[0]) != TCL_OK)
		|| (TkGetCanvasCoord(canvasPtr, argv[2], &coords[1])
		!= TCL_OK)) {
	    return TCL_ERROR;
	}
	if (argc > 3) {
	    if (TkGetCanvasCoord(canvasPtr, argv[3], &halo) != TCL_OK) {
		return TCL_ERROR;
	    }
	    if (halo < 0.0) {
		Tcl_AppendResult(interp, "can't have negative halo value \"",
			argv[3], "\"", (char *) NULL);
		return TCL_ERROR;
	    }
	} else {
	    halo = 0.0;
	}

	/*
	 * Find the item at which to start the search.
	 */

	startPtr = canvasPtr->firstItemPtr;
	if (argc == 5) {
	    itemPtr = StartTagSearch(canvasPtr, argv[4], &search);
	    if (itemPtr != NULL) {
		startPtr = itemPtr;
	    }
	}

	/*
	 * The code below is optimized so that it can eliminate most
	 * items without having to call their item-specific procedures.
	 * This is done by keeping a bounding box (x1, y1, x2, y2) that
	 * an item's bbox must overlap if the item is to have any
	 * chance of being closer than the closest so far.
	 */

	itemPtr = startPtr;
	if (itemPtr == NULL) {
	    return TCL_OK;
	}
	closestDist = (*itemPtr->typePtr->pointProc)(canvasPtr,
		itemPtr, coords) - halo;
	if (closestDist < 0.0) {
	    closestDist = 0.0;
	}
	while (1) {
	    double newDist;

	    /*
	     * Update the bounding box using itemPtr, which is the
	     * new closest item.
	     */

	    x1 = (coords[0] - closestDist - halo - 1);
	    y1 = (coords[1] - closestDist - halo - 1);
	    x2 = (coords[0] + closestDist + halo + 1);
	    y2 = (coords[1] + closestDist + halo + 1);
	    closestPtr = itemPtr;

	    /*
	     * Search for an item that beats the current closest one.
	     * Work circularly through the canvas's item list until
	     * getting back to the starting item.
	     */

	    while (1) {
		itemPtr = itemPtr->nextPtr;
		if (itemPtr == NULL) {
		    itemPtr = canvasPtr->firstItemPtr;
		}
		if (itemPtr == startPtr) {
		    DoItem(interp, closestPtr, uid);
		    return TCL_OK;
		}
		if ((itemPtr->x1 >= x2) || (itemPtr->x2 <= x1)
			|| (itemPtr->y1 >= y2) || (itemPtr->y2 <= y1)) {
		    continue;
		}
		newDist = (*itemPtr->typePtr->pointProc)(canvasPtr,
			itemPtr, coords) - halo;
		if (newDist < 0.0) {
		    newDist = 0.0;
		}
		if (newDist <= closestDist) {
		    closestDist = newDist;
		    break;
		}
	    }
	}
    } else if ((c == 'e') && (strncmp(argv[0], "enclosed", length) == 0)) {
	if (argc != 5) {
	    Tcl_AppendResult(interp, "wrong # args:  must be \"",
		    cmdName, option, " enclosed x1 y1 x2 y2", (char *) NULL);
	    return TCL_ERROR;
	}
	return FindArea(interp, canvasPtr, argv+1, uid, 1);
    } else if ((c == 'o') && (strncmp(argv[0], "overlapping", length) == 0)) {
	if (argc != 5) {
	    Tcl_AppendResult(interp, "wrong # args:  must be \"",
		    cmdName, option, " overlapping x1 y1 x2 y2",
		    (char *) NULL);
	    return TCL_ERROR;
	}
	return FindArea(interp, canvasPtr, argv+1, uid, 0);
    } else if ((c == 'w') && (strncmp(argv[0], "withtag", length) == 0)) {
	if (argc != 2) {
	    Tcl_AppendResult(interp, "wrong # args:  must be \"",
		    cmdName, option, " withtag tagOrId", (char *) NULL);
	    return TCL_ERROR;
	}
	for (itemPtr = StartTagSearch(canvasPtr, argv[1], &search);
		itemPtr != NULL; itemPtr = NextItem(&search)) {
	    DoItem(interp, itemPtr, uid);
	}
    } else  {
	Tcl_AppendResult(interp, "bad search command \"", argv[0],
		"\": must be above, all, below, closest, enclosed, ",
		"overlapping, or withtag", (char *) NULL);
	return TCL_ERROR;
    }
    return TCL_OK;
}

/*
 *--------------------------------------------------------------
 *
 * FindArea --
 *
 *	This procedure implements area searches for the "find"
 *	and "addtag" options.
 *
 * Results:
 *	A standard Tcl return value.  If newTag is NULL, then a
 *	list of ids from all the items overlapping or enclosed
 *	by the rectangle given by argc is returned in interp->result.
 *	If newTag is NULL, then the normal interp->result is an
 *	empty string.  If an error occurs, then interp->result will
 *	hold an error message.
 *
 * Side effects:
 *	If uid is non-NULL, then all the items overlapping
 *	or enclosed by the area in argv have that tag added to
 *	their lists of tags.
 *
 *--------------------------------------------------------------
 */

static int
FindArea(interp, canvasPtr, argv, uid, enclosed)
    Tcl_Interp *interp;			/* Interpreter for error reporting
					 * and result storing. */
    Tk_Canvas *canvasPtr;		/* Canvas whose items are to be
					 * searched. */
    char **argv;			/* Array of four arguments that
					 * give the coordinates of the
					 * rectangular area to search. */
    Tk_Uid uid;				/* If non-NULL, gives new tag to set
					 * on all found items;  if NULL, then
					 * ids of found items are returned
					 * in interp->result. */
    int enclosed;			/* 0 means overlapping or enclosed
					 * items are OK, 1 means only enclosed
					 * items are OK. */
{
    double rect[4], tmp;
    int x1, y1, x2, y2;
    register Tk_Item *itemPtr;

    if ((TkGetCanvasCoord(canvasPtr, argv[0], &rect[0]) != TCL_OK)
	    || (TkGetCanvasCoord(canvasPtr, argv[1], &rect[1]) != TCL_OK)
	    || (TkGetCanvasCoord(canvasPtr, argv[2], &rect[2]) != TCL_OK)
	    || (TkGetCanvasCoord(canvasPtr, argv[3], &rect[3]) != TCL_OK)) {
	return TCL_ERROR;
    }
    if (rect[0] > rect[2]) {
	tmp = rect[0]; rect[0] = rect[2]; rect[2] = tmp;
    }
    if (rect[1] > rect[3]) {
	tmp = rect[1]; rect[1] = rect[3]; rect[3] = tmp;
    }

    /*
     * Use an integer bounding box for a quick test, to avoid
     * calling item-specific code except for items that are close.
     */

    x1 = (rect[0]-1.0);
    y1 = (rect[1]-1.0);
    x2 = (rect[2]+1.0);
    y2 = (rect[3]+1.0);
    for (itemPtr = canvasPtr->firstItemPtr; itemPtr != NULL;
	    itemPtr = itemPtr->nextPtr) {
	if ((itemPtr->x1 >= x2) || (itemPtr->x2 <= x1)
		|| (itemPtr->y1 >= y2) || (itemPtr->y2 <= y1)) {
	    continue;
	}
	if ((*itemPtr->typePtr->areaProc)(canvasPtr, itemPtr, rect)
		>= enclosed) {
	    DoItem(interp, itemPtr, uid);
	}
    }
    return TCL_OK;
}

/*
 *--------------------------------------------------------------
 *
 * RelinkItems --
 *
 *	Move one or more items to a different place in the
 *	display order for a canvas.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The items identified by "tag" are moved so that they
 *	are all together in the display list and immediately
 *	after prevPtr.  The order of the moved items relative
 *	to each other is not changed.
 *
 *--------------------------------------------------------------
 */

static void
RelinkItems(canvasPtr, tag, prevPtr)
    Tk_Canvas *canvasPtr;	/* Canvas to be modified. */
    char *tag;			/* Tag identifying items to be moved
				 * in the redisplay list. */
    Tk_Item *prevPtr;		/* Reposition the items so that they
				 * go just after this item (NULL means
				 * put at beginning of list). */
{
    register Tk_Item *itemPtr;
    TagSearch search;
    Tk_Item *firstMovePtr, *lastMovePtr;

    /*
     * Find all of the items to be moved and remove them from
     * the list, making an auxiliary list running from firstMovePtr
     * to lastMovePtr.  Record their areas for redisplay.
     */

    firstMovePtr = lastMovePtr = NULL;
    for (itemPtr = StartTagSearch(canvasPtr, tag, &search);
	    itemPtr != NULL; itemPtr = NextItem(&search)) {
	if (itemPtr == prevPtr) {
	    /*
	     * Item after which insertion is to occur is being
	     * moved!  Switch to insert after its predecessor.
	     */

	    prevPtr = search.prevPtr;
	}
	if (search.prevPtr == NULL) {
	    canvasPtr->firstItemPtr = itemPtr->nextPtr;
	} else {
	    search.prevPtr->nextPtr = itemPtr->nextPtr;
	}
	if (canvasPtr->lastItemPtr == itemPtr) {
	    canvasPtr->lastItemPtr = search.prevPtr;
	}
	if (firstMovePtr == NULL) {
	    firstMovePtr = itemPtr;
	} else {
	    lastMovePtr->nextPtr = itemPtr;
	}
	lastMovePtr = itemPtr;
	EventuallyRedrawArea(canvasPtr, itemPtr->x1, itemPtr->y1,
		itemPtr->x2, itemPtr->y2);
	canvasPtr->flags |= REPICK_NEEDED;
    }

    /*
     * Insert the list of to-be-moved items back into the canvas's
     * at the desired position.
     */

    if (firstMovePtr == NULL) {
	return;
    }
    if (prevPtr == NULL) {
	lastMovePtr->nextPtr = canvasPtr->firstItemPtr;
	canvasPtr->firstItemPtr = firstMovePtr;
    } else {
	lastMovePtr->nextPtr = prevPtr->nextPtr;
	prevPtr->nextPtr = firstMovePtr;
    }
    if (canvasPtr->lastItemPtr == prevPtr) {
	canvasPtr->lastItemPtr = lastMovePtr;
    }
}

/*
 *--------------------------------------------------------------
 *
 * CanvasBindProc --
 *
 *	This procedure is invoked by the Tk dispatcher to handle
 *	events associated with bindings on items.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Depends on the command invoked as part of the binding
 *	(if there was any).
 *
 *--------------------------------------------------------------
 */

static void
CanvasBindProc(clientData, eventPtr)
    ClientData clientData;		/* Pointer to canvas structure. */
    XEvent *eventPtr;			/* Pointer to X event that just
					 * happened. */
{
    Tk_Canvas *canvasPtr = (Tk_Canvas *) clientData;
    int repick  = 0;

    Tk_Preserve((ClientData) canvasPtr);

    /*
     * This code simulates grabs for mouse buttons by refusing to
     * pick a new current item between the time a mouse button goes
     * down and the time when the last mouse button is released is
     * released again.
     */

    if (eventPtr->type == ButtonPress) {
	canvasPtr->flags |= BUTTON_DOWN;
    } else if (eventPtr->type == ButtonRelease) {
	int mask;

	switch (eventPtr->xbutton.button) {
	    case Button1:
		mask = Button1Mask;
		break;
	    case Button2:
		mask = Button2Mask;
		break;
	    case Button3:
		mask = Button3Mask;
		break;
	    case Button4:
		mask = Button4Mask;
		break;
	    case Button5:
		mask = Button5Mask;
		break;
	    default:
		mask = 0;
		break;
	}
	if ((eventPtr->xbutton.state & (Button1Mask|Button2Mask
		|Button3Mask|Button4Mask|Button5Mask)) == mask) {
	    canvasPtr->flags &= ~BUTTON_DOWN;
	    repick = 1;
	}
    } else if ((eventPtr->type == EnterNotify)
	    || (eventPtr->type == LeaveNotify)) {
	PickCurrentItem(canvasPtr, eventPtr);
	goto done;
    } else if (eventPtr->type == MotionNotify) {
	PickCurrentItem(canvasPtr, eventPtr);
    }
    CanvasDoEvent(canvasPtr, eventPtr);
    if (repick) {
	unsigned int oldState;

	oldState = eventPtr->xbutton.state;
	eventPtr->xbutton.state &= ~(Button1Mask|Button2Mask
		|Button3Mask|Button4Mask|Button5Mask);
	PickCurrentItem(canvasPtr, eventPtr);
	eventPtr->xbutton.state = oldState;
    }

    done:
    Tk_Release((ClientData) canvasPtr);
}

/*
 *--------------------------------------------------------------
 *
 * PickCurrentItem --
 *
 *	Find the topmost item in a canvas that contains a given
 *	location and mark the the current item.  If the current
 *	item has changed, generate a fake exit event on the old
 *	current item and a fake enter event on the new current
 *	item.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The current item for canvasPtr may change.  If it does,
 *	then the commands associated with item entry and exit
 *	could do just about anything.
 *
 *--------------------------------------------------------------
 */

static void
PickCurrentItem(canvasPtr, eventPtr)
    register Tk_Canvas *canvasPtr;	/* Canvas pointer in which to select
					 * current item. */
    XEvent *eventPtr;			/* Event describing location of
					 * mouse cursor.  Must be EnterWindow,
					 * LeaveWindow, ButtonRelease, or
					 * MotionNotify. */
{
    Tk_Item *closestPtr = NULL;

    /*
     * If a button is down, then don't do anything at all;  we'll be
     * called again when all buttons are up, and we can repick then.
     * This implements a form of mouse grabbing for canvases.
     */

    if (canvasPtr->flags & BUTTON_DOWN) {
	return;
    }

    /*
     * Save information about this event in the canvas.  The event in
     * the canvas is used for two purposes:
     *
     * 1. Event bindings: if the current item changes, fake events are
     *    generated to allow item-enter and item-leave bindings to trigger.
     * 2. Reselection: if the current item gets deleted, can use the
     *    saved event to find a new current item.
     * Translate MotionNotify events into EnterNotify events, since that's
     * what gets reported to item handlers.
     */

    if (eventPtr != &canvasPtr->pickEvent) {
	if ((eventPtr->type == MotionNotify)
		|| (eventPtr->type == ButtonRelease)) {
	    canvasPtr->pickEvent.xcrossing.type = EnterNotify;
	    canvasPtr->pickEvent.xcrossing.serial = eventPtr->xmotion.serial;
	    canvasPtr->pickEvent.xcrossing.send_event
		    = eventPtr->xmotion.send_event;
	    canvasPtr->pickEvent.xcrossing.display = eventPtr->xmotion.display;
	    canvasPtr->pickEvent.xcrossing.window = eventPtr->xmotion.window;
	    canvasPtr->pickEvent.xcrossing.root = eventPtr->xmotion.root;
	    canvasPtr->pickEvent.xcrossing.subwindow = None;
	    canvasPtr->pickEvent.xcrossing.time = eventPtr->xmotion.time;
	    canvasPtr->pickEvent.xcrossing.x = eventPtr->xmotion.x;
	    canvasPtr->pickEvent.xcrossing.y = eventPtr->xmotion.y;
	    canvasPtr->pickEvent.xcrossing.x_root = eventPtr->xmotion.x_root;
	    canvasPtr->pickEvent.xcrossing.y_root = eventPtr->xmotion.y_root;
	    canvasPtr->pickEvent.xcrossing.mode = NotifyNormal;
	    canvasPtr->pickEvent.xcrossing.detail = NotifyNonlinear;
	    canvasPtr->pickEvent.xcrossing.same_screen
		    = eventPtr->xmotion.same_screen;
	    canvasPtr->pickEvent.xcrossing.focus = False;
	    canvasPtr->pickEvent.xcrossing.state = eventPtr->xmotion.state;
	} else  {
	    canvasPtr->pickEvent = *eventPtr;
	}
    }

    /*
     * A LeaveNotify event automatically means that there's no current
     * object, so the rest of the code below can be skipped.
     */

    if (canvasPtr->pickEvent.type != LeaveNotify) {
	int x1, y1, x2, y2;
	double coords[2];
	register Tk_Item *itemPtr;

	coords[0] = canvasPtr->pickEvent.xcrossing.x + canvasPtr->xOrigin;
	coords[1] = canvasPtr->pickEvent.xcrossing.y + canvasPtr->yOrigin;
	x1 = coords[0] - canvasPtr->closeEnough;
	y1 = coords[1] - canvasPtr->closeEnough;
	x2 = coords[0] + canvasPtr->closeEnough;
	y2 = coords[1] + canvasPtr->closeEnough;
    
	for (itemPtr = canvasPtr->firstItemPtr; itemPtr != NULL;
		itemPtr = itemPtr->nextPtr) {
	    if ((itemPtr->x1 >= x2) || (itemPtr->x2 < x1)
		    || (itemPtr->y1 >= y2) || (itemPtr->y2 < y1)) {
		continue;
	    }
	    if ((*itemPtr->typePtr->pointProc)(canvasPtr,
		    itemPtr, coords) <= canvasPtr->closeEnough) {
		closestPtr = itemPtr;
	    }
	}
    }

    /*
     * Simulate a LeaveNotify event on the previous current item and
     * an EnterNotify event on the new current item.  Remove the "current"
     * tag from the previous current item and place it on the new current
     * item.
     */

    if (closestPtr == canvasPtr->currentItemPtr) {
	return;
    }
    if (canvasPtr->currentItemPtr != NULL) {
	XEvent event;
	Tk_Item *itemPtr = canvasPtr->currentItemPtr;
	int i;

	event = canvasPtr->pickEvent;
	event.type = LeaveNotify;
	CanvasDoEvent(canvasPtr, &event);
	for (i = itemPtr->numTags-1; i >= 0; i--) {
	    if (itemPtr->tagPtr[i] == currentUid) {
		itemPtr->tagPtr[i] = itemPtr->tagPtr[itemPtr->numTags-1];
		itemPtr->numTags--;
		break;
	    }
	}
    }
    canvasPtr->currentItemPtr = closestPtr;
    if (canvasPtr->currentItemPtr != NULL) {
	XEvent event;

	DoItem((Tcl_Interp *) NULL, closestPtr, currentUid);
	event = canvasPtr->pickEvent;
	event.type = EnterNotify;
	CanvasDoEvent(canvasPtr, &event);
    }
}

/*
 *--------------------------------------------------------------
 *
 * CanvasDoEvent --
 *
 *	This procedure is called to invoke binding processing
 *	for a new event that is associated with the current item
 *	for a canvas.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Depends on the bindings for the canvas.
 *
 *--------------------------------------------------------------
 */

static void
CanvasDoEvent(canvasPtr, eventPtr)
    Tk_Canvas *canvasPtr;		/* Canvas widget in which event
					 * occurred. */
    XEvent *eventPtr;			/* Real or simulated X event that
					 * is to be processed. */
{
#define NUM_STATIC 3
    ClientData staticObjects[NUM_STATIC];
    ClientData *objectPtr;
    int numObjects, i;
    register Tk_Item *itemPtr;

    if (canvasPtr->bindingTable == NULL) {
	return;
    }

    itemPtr = canvasPtr->currentItemPtr;
    if ((eventPtr->type == KeyPress) || (eventPtr->type == KeyRelease)) {
	itemPtr = canvasPtr->focusItemPtr;
    }
    if (itemPtr == NULL) {
	return;
    }

    /*
     * Set up an array with all the relevant objects for processing
     * this event.  The relevant objects are (a) the event's item,
     * (b) the tags associated with the event's item, and (c) the
     * tag "all".  If there are a lot of tags then malloc an array
     * to hold all of the objects.
     */

    numObjects = itemPtr->numTags + 2;
    if (numObjects <= NUM_STATIC) {
	objectPtr = staticObjects;
    } else {
	objectPtr = (ClientData *) ckalloc((unsigned)
		(numObjects * sizeof(ClientData)));
    }
    objectPtr[0] = (ClientData) itemPtr;
    for (i = itemPtr->numTags-1; i >= 0; i--) {
	objectPtr[i+1] = (ClientData) itemPtr->tagPtr[i];
    }
    objectPtr[itemPtr->numTags+1] = (ClientData) allUid;

    /*
     * Invoke the binding system, then free up the object array if
     * it was malloc-ed.
     */

    Tk_BindEvent(canvasPtr->bindingTable, eventPtr, canvasPtr->tkwin,
	    numObjects, objectPtr);
    if (objectPtr != staticObjects) {
	ckfree((char *) objectPtr);
    }
}

/*
 *----------------------------------------------------------------------
 *
 * CanvasBlinkProc --
 *
 *	This procedure is called as a timer handler to blink the
 *	insertion cursor off and on.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The cursor gets turned on or off, redisplay gets invoked,
 *	and this procedure reschedules itself.
 *
 *----------------------------------------------------------------------
 */

static void
CanvasBlinkProc(clientData)
    ClientData clientData;	/* Pointer to record describing entry. */
{
    register Tk_Canvas *canvasPtr = (Tk_Canvas *) clientData;

    if (!(canvasPtr->flags & GOT_FOCUS) || (canvasPtr->cursorOffTime == 0)) {
	return;
    }
    if (canvasPtr->flags & CURSOR_ON) {
	canvasPtr->flags &= ~CURSOR_ON;
	canvasPtr->cursorBlinkHandler = Tk_CreateTimerHandler(
		canvasPtr->cursorOffTime, CanvasBlinkProc,
		(ClientData) canvasPtr);
    } else {
	canvasPtr->flags |= CURSOR_ON;
	canvasPtr->cursorBlinkHandler = Tk_CreateTimerHandler(
		canvasPtr->cursorOnTime, CanvasBlinkProc,
		(ClientData) canvasPtr);
    }
    if (canvasPtr->focusItemPtr != NULL) {
	EventuallyRedrawArea(canvasPtr, canvasPtr->focusItemPtr->x1,
		canvasPtr->focusItemPtr->y1, canvasPtr->focusItemPtr->x2,
		canvasPtr->focusItemPtr->y2);
    }
}

/*
 *----------------------------------------------------------------------
 *
 * CanvasFocusProc --
 *
 *	This procedure is called whenever a canvas gets or loses the
 *	input focus.  It's also called whenever the window is
 *	reconfigured while it has the focus.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The cursor gets turned on or off.
 *
 *----------------------------------------------------------------------
 */

static void
CanvasFocusProc(clientData, gotFocus)
    ClientData clientData;	/* Pointer to structure describing entry. */
    int gotFocus;		/* 1 means window is getting focus, 0 means
				 * it's losing it. */
{
    register Tk_Canvas *canvasPtr = (Tk_Canvas *) clientData;

    Tk_DeleteTimerHandler(canvasPtr->cursorBlinkHandler);
    if (gotFocus) {
	canvasPtr->flags |= GOT_FOCUS | CURSOR_ON;
	if (canvasPtr->cursorOffTime != 0) {
	    canvasPtr->cursorBlinkHandler = Tk_CreateTimerHandler(
		    canvasPtr->cursorOnTime, CanvasBlinkProc,
		    (ClientData) canvasPtr);
	}
    } else {
	canvasPtr->flags &= ~(GOT_FOCUS | CURSOR_ON);
	canvasPtr->cursorBlinkHandler = (Tk_TimerToken) NULL;
    }
    if (canvasPtr->focusItemPtr != NULL) {
	EventuallyRedrawArea(canvasPtr, canvasPtr->focusItemPtr->x1,
		canvasPtr->focusItemPtr->y1, canvasPtr->focusItemPtr->x2,
		canvasPtr->focusItemPtr->y2);
    }
}

/*
 *----------------------------------------------------------------------
 *
 * CanvasSelectTo --
 *
 *	Modify the selection by moving its un-anchored end.  This could
 *	make the selection either larger or smaller.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The selection changes.
 *
 *----------------------------------------------------------------------
 */

static void
CanvasSelectTo(canvasPtr, itemPtr, index)
    register Tk_Canvas *canvasPtr;		/* Information about widget. */
    register Tk_Item *itemPtr;		/* Item that is to hold selection. */
    int index;				/* Index of element that is to
					 * become the "other" end of the
					 * selection. */
{
    int oldFirst, oldLast;
    Tk_Item *oldSelPtr;

    oldFirst = canvasPtr->selectFirst;
    oldLast = canvasPtr->selectLast;
    oldSelPtr = canvasPtr->selItemPtr;

    /*
     * Grab the selection if we don't own it already.
     */

    if (canvasPtr->selItemPtr == NULL) {
	Tk_OwnSelection(canvasPtr->tkwin, CanvasLostSelection,
		(ClientData) canvasPtr);
    } else if (canvasPtr->selItemPtr != itemPtr) {
	EventuallyRedrawArea(canvasPtr, canvasPtr->selItemPtr->x1,
		canvasPtr->selItemPtr->y1, canvasPtr->selItemPtr->x2,
		canvasPtr->selItemPtr->y2);
    }
    canvasPtr->selItemPtr = itemPtr;

    if (canvasPtr->anchorItemPtr != itemPtr) {
	canvasPtr->anchorItemPtr = itemPtr;
	canvasPtr->selectAnchor = index;
    }
    if (canvasPtr->selectAnchor <= index) {
	canvasPtr->selectFirst = canvasPtr->selectAnchor;
	canvasPtr->selectLast = index;
    } else {
	canvasPtr->selectFirst = index;
	canvasPtr->selectLast = canvasPtr->selectAnchor - 1;
    }
    if ((canvasPtr->selectFirst != oldFirst)
	    || (canvasPtr->selectLast != oldLast)
	    || (itemPtr != oldSelPtr)) {
	EventuallyRedrawArea(canvasPtr, itemPtr->x1, itemPtr->y1,
		itemPtr->x2, itemPtr->y2);
    }
}

/*
 *--------------------------------------------------------------
 *
 * CanvasFetchSelection --
 *
 *	This procedure is invoked by Tk to return part or all of
 *	the selection, when the selection is in a canvas widget.
 *	This procedure always returns the selection as a STRING.
 *
 * Results:
 *	The return value is the number of non-NULL bytes stored
 *	at buffer.  Buffer is filled (or partially filled) with a
 *	NULL-terminated string containing part or all of the selection,
 *	as given by offset and maxBytes.
 *
 * Side effects:
 *	None.
 *
 *--------------------------------------------------------------
 */

static int
CanvasFetchSelection(clientData, offset, buffer, maxBytes)
    ClientData clientData;		/* Information about canvas widget. */
    int offset;				/* Offset within selection of first
					 * character to be returned. */
    char *buffer;			/* Location in which to place
					 * selection. */
    int maxBytes;			/* Maximum number of bytes to place
					 * at buffer, not including terminating
					 * NULL character. */
{
    register Tk_Canvas *canvasPtr = (Tk_Canvas *) clientData;

    if (canvasPtr->selItemPtr == NULL) {
	return -1;
    }
    if (canvasPtr->selItemPtr->typePtr->selectionProc == NULL) {
	return -1;
    }
    return (*canvasPtr->selItemPtr->typePtr->selectionProc)(
	    canvasPtr, canvasPtr->selItemPtr, offset, buffer, maxBytes);
}

/*
 *----------------------------------------------------------------------
 *
 * CanvasLostSelection --
 *
 *	This procedure is called back by Tk when the selection is
 *	grabbed away from a canvas widget.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The existing selection is unhighlighted, and the window is
 *	marked as not containing a selection.
 *
 *----------------------------------------------------------------------
 */

static void
CanvasLostSelection(clientData)
    ClientData clientData;		/* Information about entry widget. */
{
    Tk_Canvas *canvasPtr = (Tk_Canvas *) clientData;

    if (canvasPtr->selItemPtr != NULL) {
	EventuallyRedrawArea(canvasPtr, canvasPtr->selItemPtr->x1,
		canvasPtr->selItemPtr->y1, canvasPtr->selItemPtr->x2,
		canvasPtr->selItemPtr->y2);
    }
    canvasPtr->selItemPtr = NULL;
}

/*
 *--------------------------------------------------------------
 *
 * TkGetCanvasCoord --
 *
 *	Given a string, returns a floating-point canvas coordinate
 *	corresponding to that string.
 *
 * Results:
 *	The return value is a standard Tcl return result.  If
 *	TCL_OK is returned, then everything went well and the
 *	canvas coordinate is stored at *doublePtr;  otherwise
 *	TCL_ERROR is returned and an error message is left in
 *	canvasPtr->interp->result.
 *
 * Side effects:
 *	None.
 *
 *--------------------------------------------------------------
 */

int
TkGetCanvasCoord(canvasPtr, string, doublePtr)
    Tk_Canvas *canvasPtr;	/* Canvas to which coordinate applies. */
    char *string;		/* Describes coordinate (any screen
				 * coordinate form may be used here). */
    double *doublePtr;		/* Place to store converted coordinate. */
{
    if (Tk_GetScreenMM(canvasPtr->interp, canvasPtr->tkwin, string,
	    doublePtr) != TCL_OK) {
	return TCL_ERROR;
    }
    *doublePtr *= canvasPtr->pixelsPerMM;
    return TCL_OK;
}

/*
 *--------------------------------------------------------------
 *
 * GridAlign --
 *
 *	Given a coordinate and a grid spacing, this procedure
 *	computes the location of the nearest grid line to the
 *	coordinate.
 *
 * Results:
 *	The return value is the location of the grid line nearest
 *	to coord.
 *
 * Side effects:
 *	None.
 *
 *--------------------------------------------------------------
 */

static double
GridAlign(coord, spacing)
    double coord;		/* Coordinate to grid-align. */
    double spacing;		/* Spacing between grid lines.   If <= 0
				 * then no alignment is done. */
{
    if (spacing <= 0.0) {
	return coord;
    }
    if (coord < 0) {
	return -((int) ((-coord)/spacing + 0.5)) * spacing;
    }
    return ((int) (coord/spacing + 0.5)) * spacing;
}

/*
 *--------------------------------------------------------------
 *
 * CanvasUpdateScrollbars --
 *
 *	This procedure is invoked whenever a canvas has changed in
 *	a way that requires scrollbars to be redisplayed (e.g. the
 *	view in the canvas has changed).
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	If there are scrollbars associated with the canvas, then
 *	their scrolling commands are invoked to cause them to
 *	redisplay.  If errors occur, additional Tcl commands may
 *	be invoked to process the errors.
 *
 *--------------------------------------------------------------
 */

static void
CanvasUpdateScrollbars(canvasPtr)
    register Tk_Canvas *canvasPtr;		/* Information about canvas. */
{
    int result, size, first, last, page;
    char args[200];

#define ROUND(number)						\
    if (number >= 0) {						\
	number = (number + canvasPtr->scrollIncrement/2)	\
		/canvasPtr->scrollIncrement;			\
    } else {							\
	number = -(((-number) + canvasPtr->scrollIncrement/2)	\
		/canvasPtr->scrollIncrement);			\
    }

    canvasPtr->flags &= ~UPDATE_SCROLLBARS;
    if (canvasPtr->xScrollCmd != NULL) {
	size = ((canvasPtr->scrollX2 - canvasPtr->scrollX1)
		/canvasPtr->scrollIncrement) + 1;
	first = canvasPtr->xOrigin - canvasPtr->scrollX1;
	ROUND(first);
	last = canvasPtr->xOrigin + Tk_Width(canvasPtr->tkwin)
		- 1 - canvasPtr->scrollX1;
	ROUND(last);
	page = last - first - 1;
	if (page <= 0) {
	    page = 1;
	}
	sprintf(args, " %d %d %d %d", size, page, first, last);
	result = Tcl_VarEval(canvasPtr->interp, canvasPtr->xScrollCmd, args,
	    (char *) NULL);
	if (result != TCL_OK) {
	    TkBindError(canvasPtr->interp);
	}
	Tcl_ResetResult(canvasPtr->interp);
    }

    if (canvasPtr->yScrollCmd != NULL) {
	size = ((canvasPtr->scrollY2 - canvasPtr->scrollY1)
		/canvasPtr->scrollIncrement) + 1;
	first = canvasPtr->yOrigin - canvasPtr->scrollY1;
	ROUND(first);
	last = canvasPtr->yOrigin + Tk_Height(canvasPtr->tkwin)
		- 1 - canvasPtr->scrollY1;
	ROUND(last);
	page = last - first - 1;
	if (page <= 0) {
	    page = 1;
	}
	sprintf(args, " %d %d %d %d", size, page, first, last);
	result = Tcl_VarEval(canvasPtr->interp, canvasPtr->yScrollCmd, args,
	    (char *) NULL);
	if (result != TCL_OK) {
	    TkBindError(canvasPtr->interp);
	}
	Tcl_ResetResult(canvasPtr->interp);
    }
}

/*
 *--------------------------------------------------------------
 *
 * CanvasSetOrigin --
 *
 *	This procedure is invoked to change the mapping between
 *	canvas coordinates and screen coordinates in the canvas
 *	window.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The canvas will be redisplayed to reflect the change in
 *	view.  In addition, scrollbars will be updated if there
 *	are any.
 *
 *--------------------------------------------------------------
 */

static void
CanvasSetOrigin(canvasPtr, xOrigin, yOrigin)
    register Tk_Canvas *canvasPtr;	/* Information about canvas. */
    int xOrigin;			/* New X origin for canvas (canvas
					 * x-coord corresponding to left edge
					 * of canvas window). */
    int yOrigin;			/* New Y origin for canvas (canvas
					 * y-coord corresponding to top edge
					 * of canvas window). */
{
    /*
     * Adjust the origin if necessary to keep as much as possible of the
     * canvas in the view.
     */

    if ((canvasPtr->confine) && (canvasPtr->regionString != NULL)) {
	int windowWidth, windowHeight, canvasWidth, canvasHeight;

	windowWidth = Tk_Width(canvasPtr->tkwin);
	windowHeight = Tk_Height(canvasPtr->tkwin);
	canvasWidth = canvasPtr->scrollX2 - canvasPtr->scrollX1;
	canvasHeight = canvasPtr->scrollY2 - canvasPtr->scrollY1;
	if (canvasWidth < windowWidth) {
	    xOrigin = (canvasPtr->scrollX1) - (windowWidth-canvasWidth)/2;
	} else if (xOrigin < canvasPtr->scrollX1) {
	    xOrigin = canvasPtr->scrollX1;
	} else if (xOrigin > (canvasPtr->scrollX2 - windowWidth)) {
	    xOrigin = canvasPtr->scrollX2 - windowWidth;
	}
	if (canvasHeight < windowHeight) {
	    yOrigin = (canvasPtr->scrollY1) - (windowHeight-canvasHeight)/2;
	} else if (yOrigin < canvasPtr->scrollY1) {
	    yOrigin = canvasPtr->scrollY1;
	} else if (yOrigin > (canvasPtr->scrollY2 - windowHeight)) {
	    yOrigin = canvasPtr->scrollY2 - windowHeight;
	}
    }

    if ((xOrigin == canvasPtr->xOrigin) && (yOrigin == canvasPtr->yOrigin)) {
	return;
    }

    /*
     * Tricky point:  must redisplay not only everything that's visible
     * in the window's final configuration, but also everything that was
     * visible in the initial configuration.  This is needed because some
     * item types, like windows, need to know when they move off-screen
     * so they can explicitly undisplay themselves.
     */

    EventuallyRedrawArea(canvasPtr, canvasPtr->xOrigin, canvasPtr->yOrigin,
	    canvasPtr->xOrigin + Tk_Width(canvasPtr->tkwin),
	    canvasPtr->yOrigin + Tk_Height(canvasPtr->tkwin));
    canvasPtr->xOrigin = xOrigin;
    canvasPtr->yOrigin = yOrigin;
    canvasPtr->flags |= UPDATE_SCROLLBARS;
    EventuallyRedrawArea(canvasPtr, canvasPtr->xOrigin, canvasPtr->yOrigin,
	    canvasPtr->xOrigin + Tk_Width(canvasPtr->tkwin),
	    canvasPtr->yOrigin + Tk_Height(canvasPtr->tkwin));
}

/*
 *--------------------------------------------------------------
 *
 * CanvasTagsParseProc --
 *
 *	This procedure is invoked during option processing to handle
 *	"-tags" options for canvas items.
 *
 * Results:
 *	A standard Tcl return value.
 *
 * Side effects:
 *	The tags for a given item get replaced by those indicated
 *	in the value argument.
 *
 *--------------------------------------------------------------
 */

	/* ARGSUSED */
static int
CanvasTagsParseProc(clientData, interp, tkwin, value, widgRec, offset)
    ClientData clientData;		/* Not used.*/
    Tcl_Interp *interp;			/* Used for reporting errors. */
    Tk_Window tkwin;			/* Window containing canvas widget. */
    char *value;			/* Value of option (list of tag
					 * names). */
    char *widgRec;			/* Pointer to record for item. */
    int offset;				/* Offset into item (ignored). */
{
    register Tk_Item *itemPtr = (Tk_Item *) widgRec;
    int argc, i;
    char **argv;
    Tk_Uid *newPtr;

    /*
     * Break the value up into the individual tag names.
     */

    if (Tcl_SplitList(interp, value, &argc, &argv) != TCL_OK) {
	return TCL_ERROR;
    }

    /*
     * Make sure that there's enough space in the item to hold the
     * tag names.
     */

    if (itemPtr->tagSpace < argc) {
	newPtr = (Tk_Uid *) ckalloc((unsigned) (argc * sizeof(Tk_Uid)));
	for (i = itemPtr->numTags-1; i >= 0; i--) {
	    newPtr[i] = itemPtr->tagPtr[i];
	}
	if (itemPtr->tagPtr != itemPtr->staticTagSpace) {
	    ckfree((char *) itemPtr->tagPtr);
	}
	itemPtr->tagPtr = newPtr;
	itemPtr->tagSpace = argc;
    }
    itemPtr->numTags = argc;
    for (i = 0; i < argc; i++) {
	itemPtr->tagPtr[i] = Tk_GetUid(argv[i]);
    }
    ckfree((char *) argv);
    return TCL_OK;
}

/*
 *--------------------------------------------------------------
 *
 * CanvasTagsPrintProc --
 *
 *	This procedure is invoked by the Tk configuration code
 *	to produce a printable string for the "-tags" configuration
 *	option for canvas items.
 *
 * Results:
 *	The return value is a string describing all the tags for
 *	the item referred to by "widgRec".  In addition, *freeProcPtr
 *	is filled in with the address of a procedure to call to free
 *	the result string when it's no longer needed (or NULL to
 *	indicate that the string doesn't need to be freed).
 *
 * Side effects:
 *	None.
 *
 *--------------------------------------------------------------
 */

	/* ARGSUSED */
static char *
CanvasTagsPrintProc(clientData, tkwin, widgRec, offset, freeProcPtr)
    ClientData clientData;		/* Ignored. */
    Tk_Window tkwin;			/* Window containing canvas widget. */
    char *widgRec;			/* Pointer to record for item. */
    int offset;				/* Ignored. */
    Tcl_FreeProc **freeProcPtr;		/* Pointer to variable to fill in with
					 * information about how to reclaim
					 * storage for return string. */
{
    register Tk_Item *itemPtr = (Tk_Item *) widgRec;

    if (itemPtr->numTags == 0) {
	*freeProcPtr = (Tcl_FreeProc *) NULL;
	return "";
    }
    if (itemPtr->numTags == 1) {
	*freeProcPtr = (Tcl_FreeProc *) NULL;
	return (char *) itemPtr->tagPtr[0];
    }
    *freeProcPtr = (Tcl_FreeProc *) free;
    return Tcl_Merge(itemPtr->numTags, (char **) itemPtr->tagPtr);
}
