/* 
 * tkCanvBmap.c --
 *
 *	This file implements bitmap items for canvas widgets.
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
static char rcsid[] = "$Header: /user6/ouster/wish/RCS/tkCanvBmap.c,v 1.4 92/08/24 09:24:11 ouster Exp $ SPRITE (Berkeley)";
#endif

#include <stdio.h>
#include <math.h>
#include "tkint.h"
#include "tkcanvas.h"

/*
 * The structure below defines the record for each rectangle/oval item.
 */

typedef struct BitmapItem  {
    Tk_Item header;		/* Generic stuff that's the same for all
				 * types.  MUST BE FIRST IN STRUCTURE. */
    double x, y;		/* Coordinates of positioning point for
				 * bitmap. */
    Tk_Anchor anchor;		/* Where to anchor bitmap relative to
				 * (x,y). */
    Pixmap bitmap;		/* Bitmap to display in window. */
    XColor *fgColor;		/* Foreground color to use for bitmap. */
    XColor *bgColor;		/* Background color to use for bitmap. */
    GC gc;			/* Graphics context to use for drawing
				 * bitmap on screen. */
} BitmapItem;

/*
 * Information used for parsing configuration specs:
 */

static Tk_ConfigSpec configSpecs[] = {
    {TK_CONFIG_ANCHOR, "-anchor", (char *) NULL, (char *) NULL,
	"center", Tk_Offset(BitmapItem, anchor), TK_CONFIG_DONT_SET_DEFAULT},
    {TK_CONFIG_COLOR, "-background", (char *) NULL, (char *) NULL,
	(char *) NULL, Tk_Offset(BitmapItem, bgColor), TK_CONFIG_NULL_OK},
#if defined(USE_XPM3)
    {TK_CONFIG_PIXMAP, "-bitmap", (char *) NULL, (char *) NULL,
	(char *) NULL, Tk_Offset(BitmapItem, bitmap), TK_CONFIG_NULL_OK},
#else
    {TK_CONFIG_BITMAP, "-bitmap", (char *) NULL, (char *) NULL,
	(char *) NULL, Tk_Offset(BitmapItem, bitmap), TK_CONFIG_NULL_OK},
#endif
    {TK_CONFIG_COLOR, "-foreground", (char *) NULL, (char *) NULL,
	"black", Tk_Offset(BitmapItem, fgColor), 0},
    {TK_CONFIG_CUSTOM, "-tags", (char *) NULL, (char *) NULL,
	(char *) NULL, 0, TK_CONFIG_NULL_OK, &tkCanvasTagsOption},
    {TK_CONFIG_END, (char *) NULL, (char *) NULL, (char *) NULL,
	(char *) NULL, 0, 0}
};

/*
 * Prototypes for procedures defined in this file:
 */

static int		BitmapCoords _ANSI_ARGS_((Tk_Canvas *canvasPtr,
			    Tk_Item *itemPtr, int argc, char **argv));
static int		BitmapToArea _ANSI_ARGS_((Tk_Canvas *canvasPtr,
			    Tk_Item *itemPtr, double *rectPtr));
static double		BitmapToPoint _ANSI_ARGS_((Tk_Canvas *canvasPtr,
			    Tk_Item *itemPtr, double *coordPtr));
static void		ComputeBitmapBbox _ANSI_ARGS_((Tk_Canvas *canvasPtr,
			    BitmapItem *bmapPtr));
static int		ConfigureBitmap _ANSI_ARGS_((
			    Tk_Canvas *canvasPtr, Tk_Item *itemPtr, int argc,
			    char **argv, int flags));
static int		CreateBitmap _ANSI_ARGS_((Tk_Canvas *canvasPtr,
			    struct Tk_Item *itemPtr, int argc, char **argv));
static void		DeleteBitmap _ANSI_ARGS_((Tk_Item *itemPtr));
static void		DisplayBitmap _ANSI_ARGS_((Tk_Canvas *canvasPtr,
			    Tk_Item *itemPtr, Drawable dst));
static void		ScaleBitmap _ANSI_ARGS_((Tk_Canvas *canvasPtr,
			    Tk_Item *itemPtr, double originX, double originY,
			    double scaleX, double scaleY));
static void		TranslateBitmap _ANSI_ARGS_((Tk_Canvas *canvasPtr,
			    Tk_Item *itemPtr, double deltaX, double deltaY));

/*
 * The structures below defines the rectangle and oval item types
 * by means of procedures that can be invoked by generic item code.
 */

Tk_ItemType TkBitmapType = {
    "bitmap",				/* name */
    sizeof(BitmapItem),			/* itemSize */
    CreateBitmap,			/* createProc */
    configSpecs,			/* configSpecs */
    ConfigureBitmap,			/* configureProc */
    BitmapCoords,			/* coordProc */
    DeleteBitmap,			/* deleteProc */
    DisplayBitmap,			/* displayProc */
    0,					/* alwaysRedraw */
    BitmapToPoint,			/* pointProc */
    BitmapToArea,			/* areaProc */
    (Tk_ItemPostscriptProc *) NULL,	/* postscriptProc */
    ScaleBitmap,			/* scaleProc */
    TranslateBitmap,			/* translateProc */
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
 * CreateBitmap --
 *
 *	This procedure is invoked to create a new bitmap
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
 *	A new bitmap item is created.
 *
 *--------------------------------------------------------------
 */

static int
CreateBitmap(canvasPtr, itemPtr, argc, argv)
    register Tk_Canvas *canvasPtr;	/* Canvas to hold new item. */
    Tk_Item *itemPtr;			/* Record to hold new item;  header
					 * has been initialized by caller. */
    int argc;				/* Number of arguments in argv. */
    char **argv;			/* Arguments describing rectangle. */
{
    register BitmapItem *bmapPtr = (BitmapItem *) itemPtr;

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

    bmapPtr->anchor = TK_ANCHOR_CENTER;
    bmapPtr->bitmap = None;
    bmapPtr->fgColor = NULL;
    bmapPtr->bgColor = NULL;
    bmapPtr->gc = None;

    /*
     * Process the arguments to fill in the item record.
     */

    if ((TkGetCanvasCoord(canvasPtr, argv[0], &bmapPtr->x) != TCL_OK)
	    || (TkGetCanvasCoord(canvasPtr, argv[1],
		&bmapPtr->y) != TCL_OK)) {
	return TCL_ERROR;
    }

    if (ConfigureBitmap(canvasPtr, itemPtr, argc-2, argv+2, 0) != TCL_OK) {
	DeleteBitmap(itemPtr);
	return TCL_ERROR;
    }
    return TCL_OK;
}

/*
 *--------------------------------------------------------------
 *
 * BitmapCoords --
 *
 *	This procedure is invoked to process the "coords" widget
 *	command on bitmap items.  See the user documentation for
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
BitmapCoords(canvasPtr, itemPtr, argc, argv)
    register Tk_Canvas *canvasPtr;	/* Canvas containing item. */
    Tk_Item *itemPtr;			/* Item whose coordinates are to be
					 * read or modified. */
    int argc;				/* Number of coordinates supplied in
					 * argv. */
    char **argv;			/* Array of coordinates: x1, y1,
					 * x2, y2, ... */
{
    register BitmapItem *bmapPtr = (BitmapItem *) itemPtr;

    if (argc == 0) {
	sprintf(canvasPtr->interp->result, "%g %g", bmapPtr->x, bmapPtr->y);
    } else if (argc == 2) {
	if ((TkGetCanvasCoord(canvasPtr, argv[0], &bmapPtr->x) != TCL_OK)
		|| (TkGetCanvasCoord(canvasPtr, argv[1],
		    &bmapPtr->y) != TCL_OK)) {
	    return TCL_ERROR;
	}
	ComputeBitmapBbox(canvasPtr, bmapPtr);
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
 * ConfigureBitmap --
 *
 *	This procedure is invoked to configure various aspects
 *	of a bitmap item, such as its anchor position.
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
ConfigureBitmap(canvasPtr, itemPtr, argc, argv, flags)
    Tk_Canvas *canvasPtr;	/* Canvas containing itemPtr. */
    Tk_Item *itemPtr;		/* Bitmap item to reconfigure. */
    int argc;			/* Number of elements in argv.  */
    char **argv;		/* Arguments describing things to configure. */
    int flags;			/* Flags to pass to Tk_ConfigureWidget. */
{
    register BitmapItem *bmapPtr = (BitmapItem *) itemPtr;
    XGCValues gcValues;
    GC newGC;

    if (Tk_ConfigureWidget(canvasPtr->interp, canvasPtr->tkwin,
	    configSpecs, argc, argv, (char *) bmapPtr, flags) != TCL_OK) {
	return TCL_ERROR;
    }

    /*
     * A few of the options require additional processing, such as those
     * that determine the graphics context.
     */

    gcValues.foreground = bmapPtr->fgColor->pixel;
    if (bmapPtr->bgColor != NULL) {
	gcValues.background = bmapPtr->bgColor->pixel;
    } else {
	gcValues.background = canvasPtr->bgColor->pixel;
    }
    newGC = Tk_GetGC(canvasPtr->tkwin, GCForeground|GCBackground, &gcValues);
    if (bmapPtr->gc != None) {
	Tk_FreeGC(bmapPtr->gc);
    }
    bmapPtr->gc = newGC;

    ComputeBitmapBbox(canvasPtr, bmapPtr);

    return TCL_OK;
}

/*
 *--------------------------------------------------------------
 *
 * DeleteBitmap --
 *
 *	This procedure is called to clean up the data structure
 *	associated with a bitmap item.
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
DeleteBitmap(itemPtr)
    Tk_Item *itemPtr;			/* Item that is being deleted. */
{
    register BitmapItem *bmapPtr = (BitmapItem *) itemPtr;

    if (bmapPtr->bitmap != None) {
#if defined(USE_XPM3)
        Tk_FreePixmap(bmapPtr->bitmap);
#else
        Tk_FreeBitmap(bmapPtr->bitmap);
#endif
    }
    if (bmapPtr->fgColor != NULL) {
	Tk_FreeColor(bmapPtr->fgColor);
    }
    if (bmapPtr->bgColor != NULL) {
	Tk_FreeColor(bmapPtr->bgColor);
    }
    if (bmapPtr->gc != NULL) {
	Tk_FreeGC(bmapPtr->gc);
    }
}

/*
 *--------------------------------------------------------------
 *
 * ComputeBitmapBbox --
 *
 *	This procedure is invoked to compute the bounding box of
 *	all the pixels that may be drawn as part of a bitmap item.
 *	This procedure is where the child bitmap's placement is
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
ComputeBitmapBbox(canvasPtr, bmapPtr)
    Tk_Canvas *canvasPtr;		/* Canvas that contains item. */
    register BitmapItem *bmapPtr;	/* Item whose bbox is to be
					 * recomputed. */
{
    unsigned int width, height;
    int x, y;

    x = bmapPtr->x + 0.5;
    y = bmapPtr->y + 0.5;

    if (bmapPtr->bitmap == None) {
	bmapPtr->header.x1 = bmapPtr->header.x2 = x;
	bmapPtr->header.y1 = bmapPtr->header.y2 = y;
	return;
    }

    /*
     * Compute location and size of bitmap, using anchor information.
     */

#if defined(USE_XPM3)
    Tk_SizeOfPixmap(bmapPtr->bitmap, &width, &height);
#else
    Tk_SizeOfBitmap(bmapPtr->bitmap, &width, &height);
#endif
    switch (bmapPtr->anchor) {
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

    bmapPtr->header.x1 = x;
    bmapPtr->header.y1 = y;
    bmapPtr->header.x2 = x + width;
    bmapPtr->header.y2 = y + height;
}

/*
 *--------------------------------------------------------------
 *
 * DisplayBitmap --
 *
 *	This procedure is invoked to draw a bitmap item in a given
 *	drawable.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	ItemPtr is drawn in drawable using the transformation
 *	information in canvasPtr.
 *
 *--------------------------------------------------------------
 */

static void
DisplayBitmap(canvasPtr, itemPtr, drawable)
    register Tk_Canvas *canvasPtr;	/* Canvas that contains item. */
    Tk_Item *itemPtr;			/* Item to be displayed. */
    Drawable drawable;			/* Pixmap or window in which to draw
					 * item. */
{
    register BitmapItem *bmapPtr = (BitmapItem *) itemPtr;

    if (bmapPtr->bitmap != None) {
#if defined(USE_XPM3)        
	XCopyArea(Tk_Display(canvasPtr->tkwin), bmapPtr->bitmap, drawable,
		bmapPtr->gc, 0, 0,
		(unsigned int) bmapPtr->header.x2 - bmapPtr->header.x1,
		(unsigned int) bmapPtr->header.y2 - bmapPtr->header.y1,
		bmapPtr->header.x1 - canvasPtr->drawableXOrigin,
		bmapPtr->header.y1 - canvasPtr->drawableYOrigin);
#else
	XCopyPlane(Tk_Display(canvasPtr->tkwin), bmapPtr->bitmap, drawable,
		bmapPtr->gc, 0, 0,
		(unsigned int) bmapPtr->header.x2 - bmapPtr->header.x1,
		(unsigned int) bmapPtr->header.y2 - bmapPtr->header.y1,
		bmapPtr->header.x1 - canvasPtr->drawableXOrigin,
		bmapPtr->header.y1 - canvasPtr->drawableYOrigin, 1);
#endif
    }
}

/*
 *--------------------------------------------------------------
 *
 * BitmapToPoint --
 *
 *	Computes the distance from a given point to a given
 *	rectangle, in canvas units.
 *
 * Results:
 *	The return value is 0 if the point whose x and y coordinates
 *	are coordPtr[0] and coordPtr[1] is inside the bitmap.  If the
 *	point isn't inside the bitmap then the return value is the
 *	distance from the point to the bitmap.
 *
 * Side effects:
 *	None.
 *
 *--------------------------------------------------------------
 */

	/* ARGSUSED */
static double
BitmapToPoint(canvasPtr, itemPtr, coordPtr)
    Tk_Canvas *canvasPtr;	/* Canvas containing item. */
    Tk_Item *itemPtr;		/* Item to check against point. */
    double *coordPtr;		/* Pointer to x and y coordinates. */
{
    register BitmapItem *bmapPtr = (BitmapItem *) itemPtr;
    double x1, x2, y1, y2, xDiff, yDiff;

    x1 = bmapPtr->header.x1;
    y1 = bmapPtr->header.y1;
    x2 = bmapPtr->header.x2;
    y2 = bmapPtr->header.y2;

    /*
     * Point is outside rectangle.
     */

    if (coordPtr[0] < x1) {
	xDiff = x1 - coordPtr[0];
    } else if (coordPtr[0] > x2)  {
	xDiff = coordPtr[0] - x2;
    } else {
	xDiff = 0;
    }

    if (coordPtr[1] < y1) {
	yDiff = y1 - coordPtr[1];
    } else if (coordPtr[1] > y2)  {
	yDiff = coordPtr[1] - y2;
    } else {
	yDiff = 0;
    }

    return hypot(xDiff, yDiff);
}

/*
 *--------------------------------------------------------------
 *
 * BitmapToArea --
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
BitmapToArea(canvasPtr, itemPtr, rectPtr)
    Tk_Canvas *canvasPtr;	/* Canvas containing item. */
    Tk_Item *itemPtr;		/* Item to check against rectangle. */
    double *rectPtr;		/* Pointer to array of four coordinates
				 * (x1, y1, x2, y2) describing rectangular
				 * area.  */
{
    register BitmapItem *bmapPtr = (BitmapItem *) itemPtr;

    if ((rectPtr[2] <= bmapPtr->header.x1)
	    || (rectPtr[0] >= bmapPtr->header.x2)
	    || (rectPtr[3] <= bmapPtr->header.y1)
	    || (rectPtr[1] >= bmapPtr->header.y2)) {
	return -1;
    }
    if ((rectPtr[0] <= bmapPtr->header.x1)
	    && (rectPtr[1] <= bmapPtr->header.y1)
	    && (rectPtr[2] >= bmapPtr->header.x2)
	    && (rectPtr[3] >= bmapPtr->header.y2)) {
	return 1;
    }
    return 0;
}

/*
 *--------------------------------------------------------------
 *
 * ScaleBitmap --
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
ScaleBitmap(canvasPtr, itemPtr, originX, originY, scaleX, scaleY)
    Tk_Canvas *canvasPtr;		/* Canvas containing rectangle. */
    Tk_Item *itemPtr;			/* Rectangle to be scaled. */
    double originX, originY;		/* Origin about which to scale rect. */
    double scaleX;			/* Amount to scale in X direction. */
    double scaleY;			/* Amount to scale in Y direction. */
{
    register BitmapItem *bmapPtr = (BitmapItem *) itemPtr;

    bmapPtr->x = originX + scaleX*(bmapPtr->x - originX);
    bmapPtr->y = originY + scaleY*(bmapPtr->y - originY);
    ComputeBitmapBbox(canvasPtr, bmapPtr);
}

/*
 *--------------------------------------------------------------
 *
 * TranslateBitmap --
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
TranslateBitmap(canvasPtr, itemPtr, deltaX, deltaY)
    Tk_Canvas *canvasPtr;		/* Canvas containing item. */
    Tk_Item *itemPtr;			/* Item that is being moved. */
    double deltaX, deltaY;		/* Amount by which item is to be
					 * moved. */
{
    register BitmapItem *bmapPtr = (BitmapItem *) itemPtr;

    bmapPtr->x += deltaX;
    bmapPtr->y += deltaY;
    ComputeBitmapBbox(canvasPtr, bmapPtr);
}
