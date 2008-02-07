/* 
 * tkCanvWind.c --
 *
 *	This file implements window items for canvas widgets.
 *
 * Copyright 1992 Regents of the University of California.
 * Permission to use, copy, modify, and distribute this
 * software and its documentation for any purpose and without
 * fee is hereby granted, provided that the above copyright
 * notice appear in all copies.  The University of California
 * makes no representations about the suitability of this
 * software for any purpose.  It is provided "as is" without
 * express or implied warranty.
 */

#ifndef lint
static char rcsid[] = "$Header: /user6/ouster/wish/RCS/tkCanvWind.c,v 1.4 92/08/24 09:24:22 ouster Exp $ SPRITE (Berkeley)";
#endif

#include <stdio.h>
#include <math.h>
#include "tkint.h"
#include "tkcanvas.h"

/*
 * The structure below defines the record for each window item.
 */

typedef struct WindowItem  {
    Tk_Item header;		/* Generic stuff that's the same for all
				 * types.  MUST BE FIRST IN STRUCTURE. */
    double x, y;		/* Coordinates of positioning point for
				 * window. */
    Tk_Window tkwin;		/* Window associated with item.  NULL means
				 * window has been destroyed. */
    int width;			/* Width to use for window (<= 0 means use
				 * window's requested width). */
    int height;			/* Width to use for window (<= 0 means use
				 * window's requested width). */
    Tk_Anchor anchor;		/* Where to anchor window relative to
				 * (x,y). */
    Tk_Canvas *canvasPtr;	/* Canvas containing this item. */
} WindowItem;

/*
 * Information used for parsing configuration specs:
 */

static Tk_ConfigSpec configSpecs[] = {
    {TK_CONFIG_ANCHOR, "-anchor", (char *) NULL, (char *) NULL,
	"center", Tk_Offset(WindowItem, anchor), TK_CONFIG_DONT_SET_DEFAULT},
    {TK_CONFIG_PIXELS, "-height", (char *) NULL, (char *) NULL,
	"0", Tk_Offset(WindowItem, height), TK_CONFIG_DONT_SET_DEFAULT},
    {TK_CONFIG_CUSTOM, "-tags", (char *) NULL, (char *) NULL,
	(char *) NULL, 0, TK_CONFIG_NULL_OK, &tkCanvasTagsOption},
    {TK_CONFIG_PIXELS, "-width", (char *) NULL, (char *) NULL,
	"0", Tk_Offset(WindowItem, width), TK_CONFIG_DONT_SET_DEFAULT},
    {TK_CONFIG_WINDOW, "-window", (char *) NULL, (char *) NULL,
	(char *) NULL, Tk_Offset(WindowItem, tkwin), TK_CONFIG_NULL_OK},
    {TK_CONFIG_END, (char *) NULL, (char *) NULL, (char *) NULL,
	(char *) NULL, 0, 0}
};

/*
 * Prototypes for procedures defined in this file:
 */

static void		ComputeWindowBbox _ANSI_ARGS_((Tk_Canvas *canvasPtr,
			    WindowItem *winItemPtr));
static int		ConfigureWinItem _ANSI_ARGS_((
			    Tk_Canvas *canvasPtr, Tk_Item *itemPtr, int argc,
			    char **argv, int flags));
static int		CreateWinItem _ANSI_ARGS_((Tk_Canvas *canvasPtr,
			    struct Tk_Item *itemPtr, int argc, char **argv));
static void		DeleteWinItem _ANSI_ARGS_((Tk_Item *itemPtr));
static void		DisplayWinItem _ANSI_ARGS_((Tk_Canvas *canvasPtr,
			    Tk_Item *itemPtr, Drawable dst));
static void		ScaleWinItem _ANSI_ARGS_((Tk_Canvas *canvasPtr,
			    Tk_Item *itemPtr, double originX, double originY,
			    double scaleX, double scaleY));
static void		TranslateWinItem _ANSI_ARGS_((Tk_Canvas *canvasPtr,
			    Tk_Item *itemPtr, double deltaX, double deltaY));
static int		WinItemCoords _ANSI_ARGS_((Tk_Canvas *canvasPtr,
			    Tk_Item *itemPtr, int argc, char **argv));
static void		WinItemRequestProc _ANSI_ARGS_((ClientData clientData,
			    Tk_Window tkwin));
static void		WinItemStructureProc _ANSI_ARGS_((
			    ClientData clientData, XEvent *eventPtr));
static int		WinItemToArea _ANSI_ARGS_((Tk_Canvas *canvasPtr,
			    Tk_Item *itemPtr, double *rectPtr));
static double		WinItemToPoint _ANSI_ARGS_((Tk_Canvas *canvasPtr,
			    Tk_Item *itemPtr, double *pointPtr));

/*
 * The structures below defines the rectangle and oval item types
 * by means of procedures that can be invoked by generic item code.
 */

Tk_ItemType TkWindowType = {
    "window",				/* name */
    sizeof(WindowItem),			/* itemSize */
    CreateWinItem,			/* createProc */
    configSpecs,			/* configSpecs */
    ConfigureWinItem,			/* configureProc */
    WinItemCoords,			/* coordProc */
    DeleteWinItem,			/* deleteProc */
    DisplayWinItem,			/* displayProc */
    1,					/* alwaysRedraw */
    WinItemToPoint,			/* pointProc */
    WinItemToArea,			/* areaProc */
    (Tk_ItemPostscriptProc *) NULL,	/* postscriptProc */
    ScaleWinItem,			/* scaleProc */
    TranslateWinItem,			/* translateProc */
    (Tk_ItemIndexProc *) NULL,		/* indexProc */
    (Tk_ItemCursorProc *) NULL,		/* cursorProc */
    (Tk_ItemSelectionProc *) NULL,	/* selectionProc */
    (Tk_ItemInsertProc *) NULL,		/* insertProc */
    (Tk_ItemDCharsProc *) NULL,		/* dTextProc */
    (Tk_ItemType *) NULL		/* nextPtr */
};

/*
 *--------------------------------------------------------------
 *
 * CreateWinItem --
 *
 *	This procedure is invoked to create a new window
 *	item in a canvas.
 *
 * Results:
 *	A standard Tcl return value.  If an error occurred in
 *	creating the item, then an error message is left in
 *	canvasPtr->interp->result;  in this case itemPtr is
 *	left uninitialized, so it can be safely freed by the
 *	caller.
 *
 * Side effects:
 *	A new window item is created.
 *
 *--------------------------------------------------------------
 */

static int
CreateWinItem(canvasPtr, itemPtr, argc, argv)
    register Tk_Canvas *canvasPtr;	/* Canvas to hold new item. */
    Tk_Item *itemPtr;			/* Record to hold new item;  header
					 * has been initialized by caller. */
    int argc;				/* Number of arguments in argv. */
    char **argv;			/* Arguments describing rectangle. */
{
    register WindowItem *winItemPtr = (WindowItem *) itemPtr;

    if (argc < 2) {
	Tcl_AppendResult(canvasPtr->interp, "wrong # args:  should be \"",
		Tk_PathName(canvasPtr->tkwin), "\" create ",
		itemPtr->typePtr->name, " x y ?options?",
		(char *) NULL);
	return TCL_ERROR;
    }

    /*
     * Initialize item's record.
     */

    winItemPtr->tkwin = NULL;
    winItemPtr->width = 0;
    winItemPtr->height = 0;
    winItemPtr->anchor = TK_ANCHOR_CENTER;
    winItemPtr->canvasPtr = canvasPtr;

    /*
     * Process the arguments to fill in the item record.
     */

    if ((TkGetCanvasCoord(canvasPtr, argv[0], &winItemPtr->x) != TCL_OK)
	    || (TkGetCanvasCoord(canvasPtr, argv[1],
		&winItemPtr->y) != TCL_OK)) {
	return TCL_ERROR;
    }

    if (ConfigureWinItem(canvasPtr, itemPtr, argc-2, argv+2, 0) != TCL_OK) {
	DeleteWinItem(itemPtr);
	return TCL_ERROR;
    }
    return TCL_OK;
}

/*
 *--------------------------------------------------------------
 *
 * WinItemCoords --
 *
 *	This procedure is invoked to process the "coords" widget
 *	command on window items.  See the user documentation for
 *	details on what it does.
 *
 * Results:
 *	Returns TCL_OK or TCL_ERROR, and sets canvasPtr->interp->result.
 *
 * Side effects:
 *	The coordinates for the given item may be changed.
 *
 *--------------------------------------------------------------
 */

static int
WinItemCoords(canvasPtr, itemPtr, argc, argv)
    register Tk_Canvas *canvasPtr;	/* Canvas containing item. */
    Tk_Item *itemPtr;			/* Item whose coordinates are to be
					 * read or modified. */
    int argc;				/* Number of coordinates supplied in
					 * argv. */
    char **argv;			/* Array of coordinates: x1, y1,
					 * x2, y2, ... */
{
    register WindowItem *winItemPtr = (WindowItem *) itemPtr;

    if (argc == 0) {
	sprintf(canvasPtr->interp->result, "%g %g", winItemPtr->x,
		winItemPtr->y);
    } else if (argc == 2) {
	if ((TkGetCanvasCoord(canvasPtr, argv[0], &winItemPtr->x) != TCL_OK)
		|| (TkGetCanvasCoord(canvasPtr, argv[1],
		    &winItemPtr->y) != TCL_OK)) {
	    return TCL_ERROR;
	}
	ComputeWindowBbox(canvasPtr, winItemPtr);
    } else {
	sprintf(canvasPtr->interp->result,
		"wrong # coordinates:  expected 0 or 2, got %d",
		argc);
	return TCL_ERROR;
    }
    return TCL_OK;
}

/*
 *--------------------------------------------------------------
 *
 * ConfigureWinItem --
 *
 *	This procedure is invoked to configure various aspects
 *	of a window item, such as its anchor position.
 *
 * Results:
 *	A standard Tcl result code.  If an error occurs, then
 *	an error message is left in canvasPtr->interp->result.
 *
 * Side effects:
 *	Configuration information may be set for itemPtr.
 *
 *--------------------------------------------------------------
 */

static int
ConfigureWinItem(canvasPtr, itemPtr, argc, argv, flags)
    Tk_Canvas *canvasPtr;	/* Canvas containing itemPtr. */
    Tk_Item *itemPtr;		/* Window item to reconfigure. */
    int argc;			/* Number of elements in argv.  */
    char **argv;		/* Arguments describing things to configure. */
    int flags;			/* Flags to pass to Tk_ConfigureWidget. */
{
    register WindowItem *winItemPtr = (WindowItem *) itemPtr;
    Tk_Window oldWindow;

    oldWindow = winItemPtr->tkwin;
    if (Tk_ConfigureWidget(canvasPtr->interp, canvasPtr->tkwin,
	    configSpecs, argc, argv, (char *) winItemPtr, flags) != TCL_OK) {
	return TCL_ERROR;
    }

    /*
     * A few of the options require additional processing.
     */

    if (oldWindow != winItemPtr->tkwin) {
	if (oldWindow != NULL) {
	    Tk_DeleteEventHandler(oldWindow, StructureNotifyMask,
		    WinItemStructureProc, (ClientData) winItemPtr);
	    Tk_ManageGeometry(oldWindow, (Tk_GeometryProc *) NULL,
		    (ClientData) NULL);
	    Tk_UnmapWindow(oldWindow);
	}
	if (winItemPtr->tkwin != NULL) {
	    Tk_Window ancestor, parent;

	    /*
	     * Make sure that the canvas is either the parent of the
	     * window associated with the item or a descendant of that
	     * parent.  Also, don't allow a top-level window to be
	     * managed inside a canvas.
	     */

	    parent = Tk_Parent(winItemPtr->tkwin);
	    for (ancestor = canvasPtr->tkwin; ;
		    ancestor = Tk_Parent(ancestor)) {
		if (ancestor == parent) {
		    break;
		}
		if (((Tk_FakeWin *) (ancestor))->flags & TK_TOP_LEVEL) {
		    badWindow:
		    Tcl_AppendResult(canvasPtr->interp, "can't use ",
			    Tk_PathName(winItemPtr->tkwin),
			    " in a window item of this canvas", (char *) NULL);
		    winItemPtr->tkwin = NULL;
		    return TCL_ERROR;
		}
	    }
	    if (((Tk_FakeWin *) (winItemPtr->tkwin))->flags & TK_TOP_LEVEL) {
		goto badWindow;
	    }
	    if (winItemPtr->tkwin == canvasPtr->tkwin) {
		goto badWindow;
	    }
	    Tk_CreateEventHandler(winItemPtr->tkwin, StructureNotifyMask,
		    WinItemStructureProc, (ClientData) winItemPtr);
	    Tk_ManageGeometry(winItemPtr->tkwin, WinItemRequestProc,
		    (ClientData) winItemPtr);
	}
    }

    ComputeWindowBbox(canvasPtr, winItemPtr);

    return TCL_OK;
}

/*
 *--------------------------------------------------------------
 *
 * DeleteWinItem --
 *
 *	This procedure is called to clean up the data structure
 *	associated with a window item.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Resources associated with itemPtr are released.
 *
 *--------------------------------------------------------------
 */

static void
DeleteWinItem(itemPtr)
    Tk_Item *itemPtr;			/* Item that is being deleted. */
{
    register WindowItem *winItemPtr = (WindowItem *) itemPtr;

    if (winItemPtr->tkwin != NULL) {
	Tk_DeleteEventHandler(winItemPtr->tkwin, StructureNotifyMask,
		WinItemStructureProc, (ClientData) winItemPtr);
	Tk_ManageGeometry(winItemPtr->tkwin, (Tk_GeometryProc *) NULL,
		(ClientData) NULL);
	Tk_UnmapWindow(winItemPtr->tkwin);
    }
}

/*
 *--------------------------------------------------------------
 *
 * ComputeWindowBbox --
 *
 *	This procedure is invoked to compute the bounding box of
 *	all the pixels that may be drawn as part of a window item.
 *	This procedure is where the child window's placement is
 *	computed.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The fields x1, y1, x2, and y2 are updated in the header
 *	for itemPtr.
 *
 *--------------------------------------------------------------
 */

	/* ARGSUSED */
static void
ComputeWindowBbox(canvasPtr, winItemPtr)
    Tk_Canvas *canvasPtr;		/* Canvas that contains item. */
    register WindowItem *winItemPtr;	/* Item whose bbox is to be
					 * recomputed. */
{
    int width, height, x, y;

    x = winItemPtr->x + 0.5;
    y = winItemPtr->y + 0.5;

    if (winItemPtr->tkwin == NULL) {
	winItemPtr->header.x1 = winItemPtr->header.x2 = x;
	winItemPtr->header.y1 = winItemPtr->header.y2 = y;
	return;
    }

    /*
     * Compute dimensions of window.
     */

    width = winItemPtr->width;
    if (width <= 0) {
	width = Tk_ReqWidth(winItemPtr->tkwin);
	if (width <= 0) {
	    width = 1;
	}
    }
    height = winItemPtr->height;
    if (height <= 0) {
	height = Tk_ReqHeight(winItemPtr->tkwin);
	if (height <= 0) {
	    height = 1;
	}
    }

    /*
     * Compute location of window, using anchor information.
     */

    switch (winItemPtr->anchor) {
	case TK_ANCHOR_N:
	    x -= width/2;
	    break;
	case TK_ANCHOR_NE:
	    x -= width;
	    break;
	case TK_ANCHOR_E:
	    x -= width;
	    y -= height/2;
	    break;
	case TK_ANCHOR_SE:
	    x -= width;
	    y -= height;
	    break;
	case TK_ANCHOR_S:
	    x -= width/2;
	    y -= height;
	    break;
	case TK_ANCHOR_SW:
	    y -= height;
	    break;
	case TK_ANCHOR_W:
	    y -= height/2;
	    break;
	case TK_ANCHOR_NW:
	    break;
	case TK_ANCHOR_CENTER:
	    x -= width/2;
	    y -= height/2;
	    break;
    }

    /*
     * Store the information in the item header.
     */

    winItemPtr->header.x1 = x;
    winItemPtr->header.y1 = y;
    winItemPtr->header.x2 = x + width;
    winItemPtr->header.y2 = y + height;
}

/*
 *--------------------------------------------------------------
 *
 * DisplayWinItem --
 *
 *	This procedure is invoked to "draw" a window item in a given
 *	drawable.  Since the window draws itself, we needn't do any
 *	actual redisplay here.  However, this procedure takes care
 *	of actually repositioning the child window so that it occupies
 *	the correct screen position.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The child window's position may get changed.
 *
 *--------------------------------------------------------------
 */

	/* ARGSUSED */
static void
DisplayWinItem(canvasPtr, itemPtr, drawable)
    register Tk_Canvas *canvasPtr;	/* Canvas that contains item. */
    Tk_Item *itemPtr;			/* Item to be displayed. */
    Drawable drawable;			/* Pixmap or window in which to draw
					 * item. */
{
    register WindowItem *winItemPtr = (WindowItem *) itemPtr;
    int x,y, width, height;
    Tk_Window ancestor, parent;

    if (winItemPtr->tkwin == NULL) {
	return;
    }
    x = winItemPtr->header.x1 - canvasPtr->xOrigin;
    y = winItemPtr->header.y1 - canvasPtr->yOrigin;
    width = winItemPtr->header.x2 - winItemPtr->header.x1;
    height = winItemPtr->header.y2 - winItemPtr->header.y1;

    /*
     * If the canvas isn't the parent of the window, then translate the
     * coordinates from those of the canvas to those of the window's
     * parent.
     */

    parent = Tk_Parent(winItemPtr->tkwin);
    for (ancestor = canvasPtr->tkwin; ancestor != parent;
	    ancestor = Tk_Parent(ancestor)) {
	x += Tk_X(ancestor) + Tk_Changes(ancestor)->border_width;
	y += Tk_Y(ancestor) + Tk_Changes(ancestor)->border_width;
    }

    /*
     * Reconfigure the window if it isn't already in the correct place.
     */

    if ((x != Tk_X(winItemPtr->tkwin)) || (y != Tk_Y(winItemPtr->tkwin))
	    || (width != Tk_Width(winItemPtr->tkwin))
	    || (height != Tk_Height(winItemPtr->tkwin))) {
	Tk_MoveResizeWindow(winItemPtr->tkwin, x, y, (unsigned int) width,
		(unsigned int) height);
    }
    if (!Tk_IsMapped(winItemPtr->tkwin)) {
	Tk_MapWindow(winItemPtr->tkwin);
    }
}

/*
 *--------------------------------------------------------------
 *
 * WinItemToPoint --
 *
 *	Computes the distance from a given point to a given
 *	rectangle, in canvas units.
 *
 * Results:
 *	The return value is 0 if the point whose x and y coordinates
 *	are coordPtr[0] and coordPtr[1] is inside the window.  If the
 *	point isn't inside the window then the return value is the
 *	distance from the point to the window.
 *
 * Side effects:
 *	None.
 *
 *--------------------------------------------------------------
 */

	/* ARGSUSED */
static double
WinItemToPoint(canvasPtr, itemPtr, pointPtr)
    Tk_Canvas *canvasPtr;	/* Canvas containing item. */
    Tk_Item *itemPtr;		/* Item to check against point. */
    double *pointPtr;		/* Pointer to x and y coordinates. */
{
    register WindowItem *winItemPtr = (WindowItem *) itemPtr;
    double x1, x2, y1, y2, xDiff, yDiff;

    x1 = winItemPtr->header.x1;
    y1 = winItemPtr->header.y1;
    x2 = winItemPtr->header.x2;
    y2 = winItemPtr->header.y2;

    /*
     * Point is outside rectangle.
     */

    if (pointPtr[0] < x1) {
	xDiff = x1 - pointPtr[0];
    } else if (pointPtr[0] > x2)  {
	xDiff = pointPtr[0] - x2;
    } else {
	xDiff = 0;
    }

    if (pointPtr[1] < y1) {
	yDiff = y1 - pointPtr[1];
    } else if (pointPtr[1] > y2)  {
	yDiff = pointPtr[1] - y2;
    } else {
	yDiff = 0;
    }

    return hypot(xDiff, yDiff);
}

/*
 *--------------------------------------------------------------
 *
 * WinItemToArea --
 *
 *	This procedure is called to determine whether an item
 *	lies entirely inside, entirely outside, or overlapping
 *	a given rectangle.
 *
 * Results:
 *	-1 is returned if the item is entirely outside the area
 *	given by rectPtr, 0 if it overlaps, and 1 if it is entirely
 *	inside the given area.
 *
 * Side effects:
 *	None.
 *
 *--------------------------------------------------------------
 */

	/* ARGSUSED */
static int
WinItemToArea(canvasPtr, itemPtr, rectPtr)
    Tk_Canvas *canvasPtr;	/* Canvas containing item. */
    Tk_Item *itemPtr;		/* Item to check against rectangle. */
    double *rectPtr;		/* Pointer to array of four coordinates
				 * (x1, y1, x2, y2) describing rectangular
				 * area.  */
{
    register WindowItem *winItemPtr = (WindowItem *) itemPtr;

    if ((rectPtr[2] <= winItemPtr->header.x1)
	    || (rectPtr[0] >= winItemPtr->header.x2)
	    || (rectPtr[3] <= winItemPtr->header.y1)
	    || (rectPtr[1] >= winItemPtr->header.y2)) {
	return -1;
    }
    if ((rectPtr[0] <= winItemPtr->header.x1)
	    && (rectPtr[1] <= winItemPtr->header.y1)
	    && (rectPtr[2] >= winItemPtr->header.x2)
	    && (rectPtr[3] >= winItemPtr->header.y2)) {
	return 1;
    }
    return 0;
}

/*
 *--------------------------------------------------------------
 *
 * ScaleWinItem --
 *
 *	This procedure is invoked to rescale a rectangle or oval
 *	item.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The rectangle or oval referred to by itemPtr is rescaled
 *	so that the following transformation is applied to all
 *	point coordinates:
 *		x' = originX + scaleX*(x-originX)
 *		y' = originY + scaleY*(y-originY)
 *
 *--------------------------------------------------------------
 */

static void
ScaleWinItem(canvasPtr, itemPtr, originX, originY, scaleX, scaleY)
    Tk_Canvas *canvasPtr;		/* Canvas containing rectangle. */
    Tk_Item *itemPtr;			/* Rectangle to be scaled. */
    double originX, originY;		/* Origin about which to scale rect. */
    double scaleX;			/* Amount to scale in X direction. */
    double scaleY;			/* Amount to scale in Y direction. */
{
    register WindowItem *winItemPtr = (WindowItem *) itemPtr;

    winItemPtr->x = originX + scaleX*(winItemPtr->x - originX);
    winItemPtr->y = originY + scaleY*(winItemPtr->y - originY);
    if (winItemPtr->width > 0) {
	winItemPtr->width = scaleY*winItemPtr->width;
    }
    if (winItemPtr->height > 0) {
	winItemPtr->height = scaleY*winItemPtr->height;
    }
    ComputeWindowBbox(canvasPtr, winItemPtr);
}

/*
 *--------------------------------------------------------------
 *
 * TranslateWinItem --
 *
 *	This procedure is called to move a rectangle or oval by a
 *	given amount.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The position of the rectangle or oval is offset by
 *	(xDelta, yDelta), and the bounding box is updated in the
 *	generic part of the item structure.
 *
 *--------------------------------------------------------------
 */

static void
TranslateWinItem(canvasPtr, itemPtr, deltaX, deltaY)
    Tk_Canvas *canvasPtr;		/* Canvas containing item. */
    Tk_Item *itemPtr;			/* Item that is being moved. */
    double deltaX, deltaY;		/* Amount by which item is to be
					 * moved. */
{
    register WindowItem *winItemPtr = (WindowItem *) itemPtr;

    winItemPtr->x += deltaX;
    winItemPtr->y += deltaY;
    ComputeWindowBbox(canvasPtr, winItemPtr);
}

/*
 *--------------------------------------------------------------
 *
 * WinItemStructureProc --
 *
 *	This procedure is invoked whenever StructureNotify events
 *	occur for a window that's managed as part of a canvas window
 *	item.  This procudure's only purpose is to clean up when
 *	windows are deleted.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The window is disassociated from the window item when it is
 *	deleted.
 *
 *--------------------------------------------------------------
 */

static void
WinItemStructureProc(clientData, eventPtr)
    ClientData clientData;	/* Pointer to record describing window item. */
    XEvent *eventPtr;		/* Describes what just happened. */
{
    register WindowItem *winItemPtr = (WindowItem *) clientData;

    if (eventPtr->type == DestroyNotify) {
	winItemPtr->tkwin = NULL;
    }
}

/*
 *--------------------------------------------------------------
 *
 * WinItemRequestProc --
 *
 *	This procedure is invoked whenever a window that's associated
 *	with a window canvas item changes its requested dimensions.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The size and location on the screen of the window may change,
 *	depending on the options specified for the window item.
 *
 *--------------------------------------------------------------
 */

	/* ARGSUSED */
static void
WinItemRequestProc(clientData, tkwin)
    ClientData clientData;		/* Pointer to record for window item. */
    Tk_Window tkwin;			/* Window that changed its desired
					 * size. */
{
    WindowItem *winItemPtr = (WindowItem *) clientData;

    ComputeWindowBbox(winItemPtr->canvasPtr, winItemPtr);
    DisplayWinItem(winItemPtr->canvasPtr, (Tk_Item *) winItemPtr,
	    (Drawable) None);
}
