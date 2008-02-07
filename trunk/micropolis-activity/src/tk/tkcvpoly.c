/* 
 * tkCanvPoly.c --
 *
 *	This file implements polygon items for canvas widgets.
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
static char rcsid[] = "$Header: /user6/ouster/wish/RCS/tkCanvPoly.c,v 1.6 92/07/28 15:40:10 ouster Exp $ SPRITE (Berkeley)";
#endif

#include <stdio.h>
#include <math.h>
#include "tkint.h"
#include "tkcanvas.h"
#include "tkconfig.h"

/*
 * The structure below defines the record for each polygon item.
 */

typedef struct PolygonItem  {
    Tk_Item header;		/* Generic stuff that's the same for all
				 * types.  MUST BE FIRST IN STRUCTURE. */
    int numPoints;		/* Number of points in polygon (always >= 3).
				 * Polygon is always closed. */
    double *coordPtr;		/* Pointer to malloc-ed array containing
				 * x- and y-coords of all points in polygon.
				 * X-coords are even-valued indices, y-coords
				 * are corresponding odd-valued indices. */
    XColor *fg;			/* Foreground color for polygon. */
    Pixmap fillStipple;		/* Stipple bitmap for filling polygon. */
    GC gc;			/* Graphics context for filling polygon. */
    int smooth;			/* Non-zero means draw shape smoothed (i.e.
				 * with Bezier splines). */
    int splineSteps;		/* Number of steps in each spline segment. */
} PolygonItem;

/*
 * Information used for parsing configuration specs:
 */

static Tk_ConfigSpec configSpecs[] = {
    {TK_CONFIG_COLOR, "-fill", (char *) NULL, (char *) NULL,
	"black", Tk_Offset(PolygonItem, fg), TK_CONFIG_NULL_OK},
    {TK_CONFIG_BOOLEAN, "-smooth", (char *) NULL, (char *) NULL,
	"no", Tk_Offset(PolygonItem, smooth), TK_CONFIG_DONT_SET_DEFAULT},
    {TK_CONFIG_INT, "-splinesteps", (char *) NULL, (char *) NULL,
	"12", Tk_Offset(PolygonItem, splineSteps), TK_CONFIG_DONT_SET_DEFAULT},
    {TK_CONFIG_BITMAP, "-stipple", (char *) NULL, (char *) NULL,
	(char *) NULL, Tk_Offset(PolygonItem, fillStipple), TK_CONFIG_NULL_OK},
    {TK_CONFIG_CUSTOM, "-tags", (char *) NULL, (char *) NULL,
	(char *) NULL, 0, TK_CONFIG_NULL_OK, &tkCanvasTagsOption},
    {TK_CONFIG_END, (char *) NULL, (char *) NULL, (char *) NULL,
	(char *) NULL, 0, 0}
};

/*
 * Prototypes for procedures defined in this file:
 */

static void		ComputePolygonBbox _ANSI_ARGS_((Tk_Canvas *canvasPtr,
			    PolygonItem *polyPtr));
static int		ConfigurePolygon _ANSI_ARGS_((
			    Tk_Canvas *canvasPtr, Tk_Item *itemPtr, int argc,
			    char **argv, int flags));
static int		CreatePolygon _ANSI_ARGS_((Tk_Canvas *canvasPtr,
			    struct Tk_Item *itemPtr, int argc, char **argv));
static void		DeletePolygon _ANSI_ARGS_((Tk_Item *itemPtr));
static void		DisplayPolygon _ANSI_ARGS_((Tk_Canvas *canvasPtr,
			    Tk_Item *itemPtr, Drawable dst));
static int		PolygonCoords _ANSI_ARGS_((Tk_Canvas *canvasPtr,
			    Tk_Item *itemPtr, int argc, char **argv));
static int		PolygonToArea _ANSI_ARGS_((Tk_Canvas *canvasPtr,
			    Tk_Item *itemPtr, double *rectPtr));
static double		PolygonToPoint _ANSI_ARGS_((Tk_Canvas *canvasPtr,
			    Tk_Item *itemPtr, double *pointPtr));
static void		ScalePolygon _ANSI_ARGS_((Tk_Canvas *canvasPtr,
			    Tk_Item *itemPtr, double originX, double originY,
			    double scaleX, double scaleY));
static void		TranslatePolygon _ANSI_ARGS_((Tk_Canvas *canvasPtr,
			    Tk_Item *itemPtr, double deltaX, double deltaY));

/*
 * The structures below defines the polygon item type by means
 * of procedures that can be invoked by generic item code.
 */

Tk_ItemType TkPolygonType = {
    "polygon",				/* name */
    sizeof(PolygonItem),		/* itemSize */
    CreatePolygon,			/* createProc */
    configSpecs,			/* configSpecs */
    ConfigurePolygon,			/* configureProc */
    PolygonCoords,			/* coordProc */
    DeletePolygon,			/* deleteProc */
    DisplayPolygon,			/* displayProc */
    0,					/* alwaysRedraw */
    PolygonToPoint,			/* pointProc */
    PolygonToArea,			/* areaProc */
    (Tk_ItemPostscriptProc *) NULL,	/* postscriptProc */
    ScalePolygon,			/* scaleProc */
    TranslatePolygon,			/* translateProc */
    (Tk_ItemIndexProc *) NULL,		/* indexProc */
    (Tk_ItemCursorProc *) NULL,		/* cursorProc */
    (Tk_ItemSelectionProc *) NULL,	/* selectionProc */
    (Tk_ItemInsertProc *) NULL,		/* insertProc */
    (Tk_ItemDCharsProc *) NULL,		/* dTextProc */
    (Tk_ItemType *) NULL		/* nextPtr */
};

/*
 * The definition below determines how large are static arrays
 * used to hold spline points (splines larger than this have to
 * have their arrays malloc-ed).
 */

#define MAX_STATIC_POINTS 200

/*
 *--------------------------------------------------------------
 *
 * CreatePolygon --
 *
 *	This procedure is invoked to create a new polygon item in
 *	a canvas.
 *
 * Results:
 *	A standard Tcl return value.  If an error occurred in
 *	creating the item, then an error message is left in
 *	canvasPtr->interp->result;  in this case itemPtr is
 *	left uninitialized, so it can be safely freed by the
 *	caller.
 *
 * Side effects:
 *	A new polygon item is created.
 *
 *--------------------------------------------------------------
 */

static int
CreatePolygon(canvasPtr, itemPtr, argc, argv)
    register Tk_Canvas *canvasPtr;	/* Canvas to hold new item. */
    Tk_Item *itemPtr;			/* Record to hold new item;  header
					 * has been initialized by caller. */
    int argc;				/* Number of arguments in argv. */
    char **argv;			/* Arguments describing polygon. */
{
    register PolygonItem *polyPtr = (PolygonItem *) itemPtr;
    int i;

    if (argc < 6) {
	Tcl_AppendResult(canvasPtr->interp, "wrong # args:  should be \"",
		Tk_PathName(canvasPtr->tkwin),
		"\" create x1 y1 x2 y2 x3 y3 ?x4 y4 ...? ?options?",
		(char *) NULL);
	return TCL_ERROR;
    }

    /*
     * Carry out initialization that is needed in order to clean
     * up after errors during the the remainder of this procedure.
     */

    polyPtr->numPoints = 0;
    polyPtr->coordPtr = NULL;
    polyPtr->fg = None;
    polyPtr->fillStipple = None;
    polyPtr->gc = None;
    polyPtr->smooth = 0;
    polyPtr->splineSteps = 12;

    /*
     * Count the number of points and then parse them into a point
     * array.  Leading arguments are assumed to be points if they
     * start with a digit or a minus sign followed by a digit.
     */

    for (i = 4; i < (argc-1); i+=2) {
	if ((!isdigit(argv[i][0])) &&
		((argv[i][0] != '-') || (!isdigit(argv[i][1])))) {
	    break;
	}
    }
    if (PolygonCoords(canvasPtr, itemPtr, i, argv) != TCL_OK) {
	goto error;
    }

    if (ConfigurePolygon(canvasPtr, itemPtr, argc-i, argv+i, 0) == TCL_OK) {
	return TCL_OK;
    }

    error:
    DeletePolygon(itemPtr);
    return TCL_ERROR;
}

/*
 *--------------------------------------------------------------
 *
 * PolygonCoords --
 *
 *	This procedure is invoked to process the "coords" widget
 *	command on polygons.  See the user documentation for details
 *	on what it does.
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
PolygonCoords(canvasPtr, itemPtr, argc, argv)
    register Tk_Canvas *canvasPtr;	/* Canvas containing item. */
    Tk_Item *itemPtr;			/* Item whose coordinates are to be
					 * read or modified. */
    int argc;				/* Number of coordinates supplied in
					 * argv. */
    char **argv;			/* Array of coordinates: x1, y1,
					 * x2, y2, ... */
{
    register PolygonItem *polyPtr = (PolygonItem *) itemPtr;
    char buffer[300];
    int i, numPoints;

    if (argc == 0) {
	for (i = 0; i < 2*polyPtr->numPoints; i++) {
	    sprintf(buffer, "%g", polyPtr->coordPtr[i]);
	    Tcl_AppendElement(canvasPtr->interp, buffer, 0);
	}
    } else if (argc < 6) {
	Tcl_AppendResult(canvasPtr->interp,
		"too few coordinates for polygon:  must have at least 6",
		(char *) NULL);
	return TCL_ERROR;
    } else if (argc & 1) {
	Tcl_AppendResult(canvasPtr->interp,
		"odd number of coordinates specified for polygon",
		(char *) NULL);
	return TCL_ERROR;
    } else {
	numPoints = argc/2;
	if (polyPtr->numPoints != numPoints) {
	    if (polyPtr->coordPtr != NULL) {
		ckfree((char *) polyPtr->coordPtr);
	    }

	    /*
	     * One extra point gets allocated here, just in case we have
	     * to add another point to close the polygon.
	     */

	    polyPtr->coordPtr = (double *) ckalloc((unsigned)
		    (sizeof(double) * (argc+2)));
	    polyPtr->numPoints = numPoints;
	}
	for (i = argc-1; i >= 0; i--) {
	    if (TkGetCanvasCoord(canvasPtr, argv[i], &polyPtr->coordPtr[i])
		    != TCL_OK) {
		return TCL_ERROR;
	    }
	}
    
	/*
	 * Close the polygon if it isn't already closed.
	 */
    
	if ((polyPtr->coordPtr[argc-2] != polyPtr->coordPtr[0])
		|| (polyPtr->coordPtr[argc-1] != polyPtr->coordPtr[1])) {
	    polyPtr->numPoints++;
	    polyPtr->coordPtr[argc] = polyPtr->coordPtr[0];
	    polyPtr->coordPtr[argc+1] = polyPtr->coordPtr[1];
	}
	ComputePolygonBbox(canvasPtr, polyPtr);
    }
    return TCL_OK;
}

/*
 *--------------------------------------------------------------
 *
 * ConfigurePolygon --
 *
 *	This procedure is invoked to configure various aspects
 *	of a polygon item such as its background color.
 *
 * Results:
 *	A standard Tcl result code.  If an error occurs, then
 *	an error message is left in canvasPtr->interp->result.
 *
 * Side effects:
 *	Configuration information, such as colors and stipple
 *	patterns, may be set for itemPtr.
 *
 *--------------------------------------------------------------
 */

static int
ConfigurePolygon(canvasPtr, itemPtr, argc, argv, flags)
    Tk_Canvas *canvasPtr;	/* Canvas containing itemPtr. */
    Tk_Item *itemPtr;		/* Polygon item to reconfigure. */
    int argc;			/* Number of elements in argv.  */
    char **argv;		/* Arguments describing things to configure. */
    int flags;			/* Flags to pass to Tk_ConfigureWidget. */
{
    register PolygonItem *polyPtr = (PolygonItem *) itemPtr;
    XGCValues gcValues;
    GC newGC;
    unsigned long mask;

    if (Tk_ConfigureWidget(canvasPtr->interp, canvasPtr->tkwin,
	    configSpecs, argc, argv, (char *) polyPtr, flags) != TCL_OK) {
	return TCL_ERROR;
    }

    /*
     * A few of the options require additional processing, such as
     * graphics contexts.
     */

    if (polyPtr->fg == NULL) {
	newGC = None;
    } else {
	gcValues.foreground = polyPtr->fg->pixel;
	mask = GCForeground;
	if (polyPtr->fillStipple != None) {
	    gcValues.stipple = polyPtr->fillStipple;
	    gcValues.fill_style = FillStippled;
	    mask |= GCStipple|GCFillStyle;
	}
	newGC = Tk_GetGC(canvasPtr->tkwin, mask, &gcValues);
    }
    if (polyPtr->gc != None) {
	Tk_FreeGC(polyPtr->gc);
    }
    polyPtr->gc = newGC;

    /*
     * Keep spline parameters within reasonable limits.
     */

    if (polyPtr->splineSteps < 1) {
	polyPtr->splineSteps = 1;
    } else if (polyPtr->splineSteps > 100) {
	polyPtr->splineSteps = 100;
    }

    ComputePolygonBbox(canvasPtr, polyPtr);
    return TCL_OK;
}

/*
 *--------------------------------------------------------------
 *
 * DeletePolygon --
 *
 *	This procedure is called to clean up the data structure
 *	associated with a polygon item.
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
DeletePolygon(itemPtr)
    Tk_Item *itemPtr;			/* Item that is being deleted. */
{
    register PolygonItem *polyPtr = (PolygonItem *) itemPtr;

    if (polyPtr->coordPtr != NULL) {
	ckfree((char *) polyPtr->coordPtr);
    }
    if (polyPtr->fg != NULL) {
	Tk_FreeColor(polyPtr->fg);
    }
    if (polyPtr->fillStipple != None) {
	Tk_FreeBitmap(polyPtr->fillStipple);
    }
    if (polyPtr->gc != None) {
	Tk_FreeGC(polyPtr->gc);
    }
}

/*
 *--------------------------------------------------------------
 *
 * ComputePolygonBbox --
 *
 *	This procedure is invoked to compute the bounding box of
 *	all the pixels that may be drawn as part of a polygon.
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

static void
ComputePolygonBbox(canvasPtr, polyPtr)
    register Tk_Canvas *canvasPtr;	/* Canvas that contains item. */
    PolygonItem *polyPtr;		/* Item whose bbox is to be
					 * recomputed. */
{
    register double *coordPtr;
    int i;

    coordPtr = polyPtr->coordPtr;
    polyPtr->header.x1 = polyPtr->header.x2 = *coordPtr;
    polyPtr->header.y1 = polyPtr->header.y2 = coordPtr[1];

    for (i = 1, coordPtr = polyPtr->coordPtr+2; i < polyPtr->numPoints;
	    i++, coordPtr += 2) {
	TkIncludePoint(canvasPtr, (Tk_Item *) polyPtr, coordPtr);
    }

    /*
     * Add one more pixel of fudge factor just to be safe (e.g.
     * X may round differently than we do).
     */

    polyPtr->header.x1 -= 1;
    polyPtr->header.x2 += 1;
    polyPtr->header.y1 -= 1;
    polyPtr->header.y2 += 1;
}

/*
 *--------------------------------------------------------------
 *
 * TkFillPolygon --
 *
 *	This procedure is invoked to convert a polygon to screen
 *	coordinates and display it using a particular GC.
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

void
TkFillPolygon(canvasPtr, coordPtr, numPoints, drawable, gc)
    register Tk_Canvas *canvasPtr;	/* Canvas whose coordinate system
					 * is to be used for drawing. */
    double *coordPtr;			/* Array of coordinates for polygon:
					 * x1, y1, x2, y2, .... */
    int numPoints;			/* Twice this many coordinates are
					 * present at *coordPtr. */
    Drawable drawable;			/* Pixmap or window in which to draw
					 * polygon. */
    GC gc;				/* Graphics context for drawing. */
{
    XPoint staticPoints[MAX_STATIC_POINTS];
    XPoint *pointPtr;
    register XPoint *pPtr;
    int i;

    /*
     * Build up an array of points in screen coordinates.  Use a
     * static array unless the polygon has an enormous number of points;
     * in this case, dynamically allocate an array.
     */

    if (numPoints <= MAX_STATIC_POINTS) {
	pointPtr = staticPoints;
    } else {
	pointPtr = (XPoint *) ckalloc((unsigned) (numPoints * sizeof(XPoint)));
    }

    for (i = 0, pPtr = pointPtr; i < numPoints; i += 1, coordPtr += 2, pPtr++) {
	pPtr->x = SCREEN_X(canvasPtr, coordPtr[0]);
	pPtr->y = SCREEN_Y(canvasPtr, coordPtr[1]);
    }

    /*
     * Display polygon, then free up polygon storage if it was dynamically
     * allocated.
     */

    XFillPolygon(Tk_Display(canvasPtr->tkwin), drawable, gc, pointPtr,
	    numPoints, Complex, CoordModeOrigin);
    if (pointPtr != staticPoints) {
	ckfree((char *) pointPtr);
    }

}

/*
 *--------------------------------------------------------------
 *
 * DisplayPolygon --
 *
 *	This procedure is invoked to draw a polygon item in a given
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
DisplayPolygon(canvasPtr, itemPtr, drawable)
    register Tk_Canvas *canvasPtr;	/* Canvas that contains item. */
    Tk_Item *itemPtr;			/* Item to be displayed. */
    Drawable drawable;			/* Pixmap or window in which to draw
					 * item. */
{
    register PolygonItem *polyPtr = (PolygonItem *) itemPtr;

    if (polyPtr->gc == None) {
	return;
    }

    if (!polyPtr->smooth) {
	TkFillPolygon(canvasPtr, polyPtr->coordPtr, polyPtr->numPoints,
		drawable, polyPtr->gc);
    } else {
	int numPoints;
	XPoint staticPoints[MAX_STATIC_POINTS];
	XPoint *pointPtr;

	/*
	 * This is a smoothed polygon.  Display using a set of generated
	 * spline points rather than the original points.
	 */

	numPoints = 1 + polyPtr->numPoints*polyPtr->splineSteps;
	if (numPoints <= MAX_STATIC_POINTS) {
	    pointPtr = staticPoints;
	} else {
	    pointPtr = (XPoint *) ckalloc((unsigned)
		    (numPoints * sizeof(XPoint)));
	}
	numPoints = TkMakeBezierCurve(canvasPtr, polyPtr->coordPtr,
		polyPtr->numPoints, polyPtr->splineSteps, pointPtr,
		(double *) NULL);
	XFillPolygon(Tk_Display(canvasPtr->tkwin), drawable, polyPtr->gc,
		pointPtr, numPoints, Complex, CoordModeOrigin);
	if (pointPtr != staticPoints) {
	    ckfree((char *) pointPtr);
	}
    }
}

/*
 *--------------------------------------------------------------
 *
 * PolygonToPoint --
 *
 *	Computes the distance from a given point to a given
 *	polygon, in canvas units.
 *
 * Results:
 *	The return value is 0 if the point whose x and y coordinates
 *	are pointPtr[0] and pointPtr[1] is inside the polygon.  If the
 *	point isn't inside the polygon then the return value is the
 *	distance from the point to the polygon.
 *
 * Side effects:
 *	None.
 *
 *--------------------------------------------------------------
 */

	/* ARGSUSED */
static double
PolygonToPoint(canvasPtr, itemPtr, pointPtr)
    Tk_Canvas *canvasPtr;	/* Canvas containing item. */
    Tk_Item *itemPtr;		/* Item to check against point. */
    double *pointPtr;		/* Pointer to x and y coordinates. */
{
    PolygonItem *polyPtr = (PolygonItem *) itemPtr;
    double *coordPtr, distance;
    double staticSpace[2*MAX_STATIC_POINTS];
    int numPoints;

    if (!polyPtr->smooth) {
	return TkPolygonToPoint(polyPtr->coordPtr, polyPtr->numPoints,
		pointPtr);
    }

    /*
     * Smoothed polygon.  Generate a new set of points and use them
     * for comparison.
     */

    numPoints = 1 + polyPtr->numPoints*polyPtr->splineSteps;
    if (numPoints <= MAX_STATIC_POINTS) {
	coordPtr = staticSpace;
    } else {
	coordPtr = (double *) ckalloc((unsigned)
		(2*numPoints*sizeof(double)));
    }
    numPoints = TkMakeBezierCurve(canvasPtr, polyPtr->coordPtr,
	    polyPtr->numPoints, polyPtr->splineSteps, (XPoint *) NULL,
	    coordPtr);
    distance = TkPolygonToPoint(coordPtr, numPoints, pointPtr);
    if (coordPtr != staticSpace) {
	ckfree((char *) coordPtr);
    }
    return distance;
}

/*
 *--------------------------------------------------------------
 *
 * PolygonToArea --
 *
 *	This procedure is called to determine whether an item
 *	lies entirely inside, entirely outside, or overlapping
 *	a given rectangular area.
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
PolygonToArea(canvasPtr, itemPtr, rectPtr)
    Tk_Canvas *canvasPtr;	/* Canvas containing item. */
    Tk_Item *itemPtr;		/* Item to check against polygon. */
    double *rectPtr;		/* Pointer to array of four coordinates
				 * (x1, y1, x2, y2) describing rectangular
				 * area.  */
{
    PolygonItem *polyPtr = (PolygonItem *) itemPtr;
    double *coordPtr;
    double staticSpace[2*MAX_STATIC_POINTS];
    int numPoints, result;

    if (!polyPtr->smooth) {
	return TkPolygonToArea(polyPtr->coordPtr, polyPtr->numPoints, rectPtr);
    }

    /*
     * Smoothed polygon.  Generate a new set of points and use them
     * for comparison.
     */

    numPoints = 1 + polyPtr->numPoints*polyPtr->splineSteps;
    if (numPoints <= MAX_STATIC_POINTS) {
	coordPtr = staticSpace;
    } else {
	coordPtr = (double *) ckalloc((unsigned)
		(2*numPoints*sizeof(double)));
    }
    numPoints = TkMakeBezierCurve(canvasPtr, polyPtr->coordPtr,
	    polyPtr->numPoints, polyPtr->splineSteps, (XPoint *) NULL,
	    coordPtr);
    result = TkPolygonToArea(coordPtr, numPoints, rectPtr);
    if (coordPtr != staticSpace) {
	ckfree((char *) coordPtr);
    }
    return result;
}

/*
 *--------------------------------------------------------------
 *
 * ScalePolygon --
 *
 *	This procedure is invoked to rescale a polygon item.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The polygon referred to by itemPtr is rescaled so that the
 *	following transformation is applied to all point
 *	coordinates:
 *		x' = originX + scaleX*(x-originX)
 *		y' = originY + scaleY*(y-originY)
 *
 *--------------------------------------------------------------
 */

static void
ScalePolygon(canvasPtr, itemPtr, originX, originY, scaleX, scaleY)
    Tk_Canvas *canvasPtr;		/* Canvas containing polygon. */
    Tk_Item *itemPtr;			/* Polygon to be scaled. */
    double originX, originY;		/* Origin about which to scale rect. */
    double scaleX;			/* Amount to scale in X direction. */
    double scaleY;			/* Amount to scale in Y direction. */
{
    PolygonItem *polyPtr = (PolygonItem *) itemPtr;
    register double *coordPtr;
    int i;

    for (i = 0, coordPtr = polyPtr->coordPtr; i < polyPtr->numPoints;
	    i++, coordPtr += 2) {
	*coordPtr = originX + scaleX*(*coordPtr - originX);
	coordPtr[1] = originY + scaleY*(coordPtr[1] - originY);
    }
    ComputePolygonBbox(canvasPtr, polyPtr);
}

/*
 *--------------------------------------------------------------
 *
 * TranslatePolygon --
 *
 *	This procedure is called to move a polygon by a given
 *	amount.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The position of the polygon is offset by (xDelta, yDelta),
 *	and the bounding box is updated in the generic part of the
 *	item structure.
 *
 *--------------------------------------------------------------
 */

static void
TranslatePolygon(canvasPtr, itemPtr, deltaX, deltaY)
    Tk_Canvas *canvasPtr;		/* Canvas containing item. */
    Tk_Item *itemPtr;			/* Item that is being moved. */
    double deltaX, deltaY;		/* Amount by which item is to be
					 * moved. */
{
    PolygonItem *polyPtr = (PolygonItem *) itemPtr;
    register double *coordPtr;
    int i;

    for (i = 0, coordPtr = polyPtr->coordPtr; i < polyPtr->numPoints;
	    i++, coordPtr += 2) {
	*coordPtr += deltaX;
	coordPtr[1] += deltaY;
    }
    ComputePolygonBbox(canvasPtr, polyPtr);
}
