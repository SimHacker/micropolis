/* 
 * tkCanvLine.c --
 *
 *	This file implements line items for canvas widgets.
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
static char rcsid[] = "$Header: /user6/ouster/wish/RCS/tkCanvLine.c,v 1.7 92/07/28 15:40:08 ouster Exp $ SPRITE (Berkeley)";
#endif

#include <stdio.h>
#include <math.h>
#include "tkint.h"
#include "tkcanvas.h"
#include "tkconfig.h"

/*
 * The structure below defines the record for each line item.
 */

typedef struct LineItem  {
    Tk_Item header;		/* Generic stuff that's the same for all
				 * types.  MUST BE FIRST IN STRUCTURE. */
    Tk_Canvas *canvasPtr;	/* Canvas containing item.  Needed for
				 * parsing arrow shapes. */
    int numPoints;		/* Number of points in line (always >= 2). */
    double *coordPtr;		/* Pointer to malloc-ed array containing
				 * x- and y-coords of all points in line.
				 * X-coords are even-valued indices, y-coords
				 * are corresponding odd-valued indices. */
    int width;			/* Width of line. */
    XColor *fg;			/* Foreground color for line. */
    Pixmap fillStipple;		/* Stipple bitmap for filling line. */
    int capStyle;		/* Cap style for line. */
    int joinStyle;		/* Join style for line. */
    GC gc;			/* Graphics context for filling line. */
    Tk_Uid arrow;		/* Indicates whether or not to draw arrowheads:
				 * "none", "first", "last", or "both". */
    float arrowShapeA;		/* Distance from tip of arrowhead to center. */
    float arrowShapeB;		/* Distance from tip of arrowhead to trailing
				 * point, measured along shaft. */
    float arrowShapeC;		/* Distance of trailing points from outside
				 * edge of shaft. */
    double *firstArrowPtr;	/* Points to array of 5 points describing
				 * polygon for arrowhead at first point in
				 * line.  First point of arrowhead is tip.
				 * Malloc'ed.  NULL means no arrowhead at
				 * first point. */
    double *lastArrowPtr;	/* Points to polygon for arrowhead at last
				 * point in line (5 points, first of which
				 * is tip).  Malloc'ed.  NULL means no
				 * arrowhead at last point. */
    int smooth;			/* Non-zero means draw line smoothed (i.e.
				 * with Bezier splines). */
    int splineSteps;		/* Number of steps in each spline segment. */
} LineItem;

/*
 * Number of points in an arrowHead:
 */

#define PTS_IN_ARROW 6

/*
 * Prototypes for procedures defined in this file:
 */

static void		ComputeLineBbox _ANSI_ARGS_((Tk_Canvas *canvasPtr,
			    LineItem *linePtr));
static int		ConfigureLine _ANSI_ARGS_((
			    Tk_Canvas *canvasPtr, Tk_Item *itemPtr, int argc,
			    char **argv, int flags));
static int		ConfigureArrows _ANSI_ARGS_((Tk_Canvas *canvasPtr,
			    LineItem *linePtr));
static int		CreateLine _ANSI_ARGS_((Tk_Canvas *canvasPtr,
			    struct Tk_Item *itemPtr, int argc, char **argv));
static void		DeleteLine _ANSI_ARGS_((Tk_Item *itemPtr));
static void		DisplayLine _ANSI_ARGS_((Tk_Canvas *canvasPtr,
			    Tk_Item *itemPtr, Drawable dst));
static int		LineCoords _ANSI_ARGS_((Tk_Canvas *canvasPtr,
			    Tk_Item *itemPtr, int argc, char **argv));
static int		LineToArea _ANSI_ARGS_((Tk_Canvas *canvasPtr,
			    Tk_Item *itemPtr, double *rectPtr));
static double		LineToPoint _ANSI_ARGS_((Tk_Canvas *canvasPtr,
			    Tk_Item *itemPtr, double *coordPtr));
static int		ParseArrowShape _ANSI_ARGS_((ClientData clientData,
			    Tcl_Interp *interp, Tk_Window tkwin, char *value,
			    char *recordPtr, int offset));
static char *		PrintArrowShape _ANSI_ARGS_((ClientData clientData,
			    Tk_Window tkwin, char *recordPtr, int offset,
			    Tcl_FreeProc **freeProcPtr));
static void		ScaleLine _ANSI_ARGS_((Tk_Canvas *canvasPtr,
			    Tk_Item *itemPtr, double originX, double originY,
			    double scaleX, double scaleY));
static void		TranslateLine _ANSI_ARGS_((Tk_Canvas *canvasPtr,
			    Tk_Item *itemPtr, double deltaX, double deltaY));

/*
 * Information used for parsing configuration specs.  If you change any
 * of the default strings, be sure to change the corresponding default
 * values in CreateLine.
 */

static Tk_CustomOption arrowShapeOption = {ParseArrowShape,
	PrintArrowShape, (ClientData) NULL};

static Tk_ConfigSpec configSpecs[] = {
    {TK_CONFIG_UID, "-arrow", (char *) NULL, (char *) NULL,
	"none", Tk_Offset(LineItem, arrow), TK_CONFIG_DONT_SET_DEFAULT},
    {TK_CONFIG_CUSTOM, "-arrowshape", (char *) NULL, (char *) NULL,
	"8 10 3", Tk_Offset(LineItem, arrowShapeA),
	TK_CONFIG_DONT_SET_DEFAULT, &arrowShapeOption},
    {TK_CONFIG_CAP_STYLE, "-capstyle", (char *) NULL, (char *) NULL,
	"butt", Tk_Offset(LineItem, capStyle), TK_CONFIG_DONT_SET_DEFAULT},
    {TK_CONFIG_COLOR, "-fill", (char *) NULL, (char *) NULL,
	"black", Tk_Offset(LineItem, fg), 0},
    {TK_CONFIG_JOIN_STYLE, "-joinstyle", (char *) NULL, (char *) NULL,
	"round", Tk_Offset(LineItem, joinStyle), TK_CONFIG_DONT_SET_DEFAULT},
    {TK_CONFIG_BOOLEAN, "-smooth", (char *) NULL, (char *) NULL,
	"no", Tk_Offset(LineItem, smooth), TK_CONFIG_DONT_SET_DEFAULT},
    {TK_CONFIG_INT, "-splinesteps", (char *) NULL, (char *) NULL,
	"12", Tk_Offset(LineItem, splineSteps), TK_CONFIG_DONT_SET_DEFAULT},
    {TK_CONFIG_BITMAP, "-stipple", (char *) NULL, (char *) NULL,
	(char *) NULL, Tk_Offset(LineItem, fillStipple), TK_CONFIG_NULL_OK},
    {TK_CONFIG_CUSTOM, "-tags", (char *) NULL, (char *) NULL,
	(char *) NULL, 0, TK_CONFIG_NULL_OK, &tkCanvasTagsOption},
    {TK_CONFIG_PIXELS, "-width", (char *) NULL, (char *) NULL,
	"1", Tk_Offset(LineItem, width), TK_CONFIG_DONT_SET_DEFAULT},
    {TK_CONFIG_END, (char *) NULL, (char *) NULL, (char *) NULL,
	(char *) NULL, 0, 0}
};

/*
 * The structures below defines the line item type by means
 * of procedures that can be invoked by generic item code.
 */

Tk_ItemType TkLineType = {
    "line",				/* name */
    sizeof(LineItem),			/* itemSize */
    CreateLine,				/* createProc */
    configSpecs,			/* configSpecs */
    ConfigureLine,			/* configureProc */
    LineCoords,				/* coordProc */
    DeleteLine,				/* deleteProc */
    DisplayLine,			/* displayProc */
    0,					/* alwaysRedraw */
    LineToPoint,			/* pointProc */
    LineToArea,				/* areaProc */
    (Tk_ItemPostscriptProc *) NULL,	/* postscriptProc */
    ScaleLine,				/* scaleProc */
    TranslateLine,			/* translateProc */
    (Tk_ItemIndexProc *) NULL,		/* indexProc */
    (Tk_ItemCursorProc *) NULL,		/* cursorProc */
    (Tk_ItemSelectionProc *) NULL,	/* selectionProc */
    (Tk_ItemInsertProc *) NULL,		/* insertProc */
    (Tk_ItemDCharsProc *) NULL,		/* dTextProc */
    (Tk_ItemType *) NULL		/* nextPtr */
};

/*
 * The Tk_Uid's below refer to uids for the various arrow types:
 */

static Tk_Uid noneUid = NULL;
static Tk_Uid firstUid = NULL;
static Tk_Uid lastUid = NULL;
static Tk_Uid bothUid = NULL;

/*
 * The definition below determines how large are static arrays
 * used to hold spline points (splines larger than this have to
 * have their arrays malloc-ed).
 */

#define MAX_STATIC_POINTS 200

/*
 *--------------------------------------------------------------
 *
 * CreateLine --
 *
 *	This procedure is invoked to create a new line item in
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
 *	A new line item is created.
 *
 *--------------------------------------------------------------
 */

static int
CreateLine(canvasPtr, itemPtr, argc, argv)
    register Tk_Canvas *canvasPtr;	/* Canvas to hold new item. */
    Tk_Item *itemPtr;			/* Record to hold new item;  header
					 * has been initialized by caller. */
    int argc;				/* Number of arguments in argv. */
    char **argv;			/* Arguments describing line. */
{
    register LineItem *linePtr = (LineItem *) itemPtr;
    int i;

    if (argc < 4) {
	Tcl_AppendResult(canvasPtr->interp, "wrong # args:  should be \"",
		Tk_PathName(canvasPtr->tkwin),
		"\" create x1 y1 x2 y2 ?x3 y3 ...? ?options?",
		(char *) NULL);
	return TCL_ERROR;
    }

    /*
     * Carry out initialization that is needed to set defaults and to
     * allow proper cleanup after errors during the the remainder of
     * this procedure.
     */

    linePtr->canvasPtr = canvasPtr;
    linePtr->numPoints = 0;
    linePtr->coordPtr = NULL;
    linePtr->width = 1;
    linePtr->fg = None;
    linePtr->fillStipple = None;
    linePtr->capStyle = CapButt;
    linePtr->joinStyle = JoinRound;
    linePtr->gc = None;
    if (noneUid == NULL) {
	noneUid = Tk_GetUid("none");
	firstUid = Tk_GetUid("first");
	lastUid = Tk_GetUid("last");
	bothUid = Tk_GetUid("both");
    }
    linePtr->arrow = noneUid;
    linePtr->arrowShapeA = 8.0;
    linePtr->arrowShapeB = 10.0;
    linePtr->arrowShapeC = 3.0;
    linePtr->firstArrowPtr = NULL;
    linePtr->lastArrowPtr = NULL;
    linePtr->smooth = 0;
    linePtr->splineSteps = 12;

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
    if (LineCoords(canvasPtr, itemPtr, i, argv) != TCL_OK) {
	goto error;
    }
    if (ConfigureLine(canvasPtr, itemPtr, argc-i, argv+i, 0) == TCL_OK) {
	return TCL_OK;
    }

    error:
    DeleteLine(itemPtr);
    return TCL_ERROR;
}

/*
 *--------------------------------------------------------------
 *
 * LineCoords --
 *
 *	This procedure is invoked to process the "coords" widget
 *	command on lines.  See the user documentation for details
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
LineCoords(canvasPtr, itemPtr, argc, argv)
    register Tk_Canvas *canvasPtr;	/* Canvas containing item. */
    Tk_Item *itemPtr;			/* Item whose coordinates are to be
					 * read or modified. */
    int argc;				/* Number of coordinates supplied in
					 * argv. */
    char **argv;			/* Array of coordinates: x1, y1,
					 * x2, y2, ... */
{
    register LineItem *linePtr = (LineItem *) itemPtr;
    char buffer[300];
    int i, numPoints;

    if (argc == 0) {
	for (i = 0; i < 2*linePtr->numPoints; i++) {
	    sprintf(buffer, "%g", linePtr->coordPtr[i]);
	    Tcl_AppendElement(canvasPtr->interp, buffer, 0);
	}
    } else if (argc < 4) {
	Tcl_AppendResult(canvasPtr->interp,
		"too few coordinates for line:  must have at least 4",
		(char *) NULL);
	return TCL_ERROR;
    } else if (argc & 1) {
	Tcl_AppendResult(canvasPtr->interp,
		"odd number of coordinates specified for line",
		(char *) NULL);
	return TCL_ERROR;
    } else {
	numPoints = argc/2;
	if (linePtr->numPoints != numPoints) {
	    if (linePtr->coordPtr != NULL) {
		ckfree((char *) linePtr->coordPtr);
	    }
	    linePtr->coordPtr = (double *) ckalloc((unsigned)
		    (sizeof(double) * argc));
	    linePtr->numPoints = numPoints;
	}
	for (i = argc-1; i >= 0; i--) {
	    if (TkGetCanvasCoord(canvasPtr, argv[i], &linePtr->coordPtr[i])
		    != TCL_OK) {
		return TCL_ERROR;
	    }
	}
	ComputeLineBbox(canvasPtr, linePtr);
    }
    return TCL_OK;
}

/*
 *--------------------------------------------------------------
 *
 * ConfigureLine --
 *
 *	This procedure is invoked to configure various aspects
 *	of a line item such as its background color.
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
ConfigureLine(canvasPtr, itemPtr, argc, argv, flags)
    Tk_Canvas *canvasPtr;	/* Canvas containing itemPtr. */
    Tk_Item *itemPtr;		/* Line item to reconfigure. */
    int argc;			/* Number of elements in argv.  */
    char **argv;		/* Arguments describing things to configure. */
    int flags;			/* Flags to pass to Tk_ConfigureWidget. */
{
    register LineItem *linePtr = (LineItem *) itemPtr;
    XGCValues gcValues;
    GC newGC;
    unsigned long mask;

    if (Tk_ConfigureWidget(canvasPtr->interp, canvasPtr->tkwin,
	    configSpecs, argc, argv, (char *) linePtr, flags) != TCL_OK) {
	return TCL_ERROR;
    }

    /*
     * A few of the options require additional processing, such as
     * graphics contexts.
     */

    if (linePtr->fg == NULL) {
	newGC = None;
    } else {
	gcValues.foreground = linePtr->fg->pixel;
	gcValues.join_style = linePtr->joinStyle;
	if (linePtr->width < 0) {
	    linePtr->width = 1;
	}
	gcValues.line_width = linePtr->width;
	mask = GCForeground|GCJoinStyle|GCLineWidth;
	if (linePtr->fillStipple != None) {
	    gcValues.stipple = linePtr->fillStipple;
	    gcValues.fill_style = FillStippled;
	    mask |= GCStipple|GCFillStyle;
	}
	if (linePtr->arrow == noneUid) {
	    gcValues.cap_style = linePtr->capStyle;
	    mask |= GCCapStyle;
	}
	newGC = Tk_GetGC(canvasPtr->tkwin, mask, &gcValues);
    }
    if (linePtr->gc != None) {
	Tk_FreeGC(linePtr->gc);
    }
    linePtr->gc = newGC;

    /*
     * Keep spline parameters within reasonable limits.
     */

    if (linePtr->splineSteps < 1) {
	linePtr->splineSteps = 1;
    } else if (linePtr->splineSteps > 100) {
	linePtr->splineSteps = 100;
    }

    /*
     * Setup arrowheads, if needed.  If arrowheads are turned off,
     * restore the line's endpoints (they were shortened when the
     * arrowheads were added).
     */

    if ((linePtr->firstArrowPtr != NULL) && (linePtr->arrow != firstUid)
	    && (linePtr->arrow != bothUid)) {
	linePtr->coordPtr[0] = linePtr->firstArrowPtr[0];
	linePtr->coordPtr[1] = linePtr->firstArrowPtr[1];
	ckfree((char *) linePtr->firstArrowPtr);
	linePtr->firstArrowPtr = NULL;
    }
    if ((linePtr->lastArrowPtr != NULL) && (linePtr->arrow != lastUid)
	    && (linePtr->arrow != bothUid)) {
	int index;

	index = 2*(linePtr->numPoints-1);
	linePtr->coordPtr[index] = linePtr->lastArrowPtr[0];
	linePtr->coordPtr[index+1] = linePtr->lastArrowPtr[1];
	ckfree((char *) linePtr->lastArrowPtr);
	linePtr->lastArrowPtr = NULL;
    }
    if (linePtr->arrow != noneUid) {
	if ((linePtr->arrow != firstUid) && (linePtr->arrow != lastUid)
		&& (linePtr->arrow != bothUid)) {
	    Tcl_AppendResult(canvasPtr->interp, "bad arrow spec \"",
		    linePtr->arrow, "\": must be none, first, last, or both",
		    (char *) NULL);
	    linePtr->arrow = noneUid;
	    return TCL_ERROR;
	}
	ConfigureArrows(canvasPtr, linePtr);
    }

    /*
     * Recompute bounding box for line.
     */

    ComputeLineBbox(canvasPtr, linePtr);

    return TCL_OK;
}

/*
 *--------------------------------------------------------------
 *
 * DeleteLine --
 *
 *	This procedure is called to clean up the data structure
 *	associated with a line item.
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
DeleteLine(itemPtr)
    Tk_Item *itemPtr;			/* Item that is being deleted. */
{
    register LineItem *linePtr = (LineItem *) itemPtr;

    if (linePtr->coordPtr != NULL) {
	ckfree((char *) linePtr->coordPtr);
    }
    if (linePtr->fg != NULL) {
	Tk_FreeColor(linePtr->fg);
    }
    if (linePtr->fillStipple != None) {
	Tk_FreeBitmap(linePtr->fillStipple);
    }
    if (linePtr->gc != None) {
	Tk_FreeGC(linePtr->gc);
    }
    if (linePtr->firstArrowPtr != NULL) {
	ckfree((char *) linePtr->firstArrowPtr);
    }
    if (linePtr->lastArrowPtr != NULL) {
	ckfree((char *) linePtr->lastArrowPtr);
    }
}

/*
 *--------------------------------------------------------------
 *
 * ComputeLineBbox --
 *
 *	This procedure is invoked to compute the bounding box of
 *	all the pixels that may be drawn as part of a line.
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
ComputeLineBbox(canvasPtr, linePtr)
    register Tk_Canvas *canvasPtr;	/* Canvas that contains item. */
    LineItem *linePtr;			/* Item whose bbos is to be
					 * recomputed. */
{
    register double *coordPtr;
    int i;

    coordPtr = linePtr->coordPtr;
    linePtr->header.x1 = linePtr->header.x2 = *coordPtr;
    linePtr->header.y1 = linePtr->header.y2 = coordPtr[1];

    /*
     * Compute the bounding box of all the points in the line,
     * then expand in all directions by the line's width to take
     * care of butting or rounded corners and projecting or
     * rounded caps.  This expansion is an overestimate (worst-case
     * is square root of two over two) but it's simple.  Don't do
     * anything special for curves.  This causes an additional
     * overestimate in the bounding box, but is faster.
     */

    for (i = 1, coordPtr = linePtr->coordPtr+2; i < linePtr->numPoints;
	    i++, coordPtr += 2) {
	TkIncludePoint(canvasPtr, (Tk_Item *) linePtr, coordPtr);
    }
    linePtr->header.x1 -= linePtr->width;
    linePtr->header.x2 += linePtr->width;
    linePtr->header.y1 -= linePtr->width;
    linePtr->header.y2 += linePtr->width;

    /*
     * For mitered lines, make a second pass through all the points.
     * Compute the locations of the two miter vertex points and add
     * those into the bounding box.
     */

    if (linePtr->joinStyle == JoinMiter) {
	for (i = linePtr->numPoints, coordPtr = linePtr->coordPtr; i >= 3;
		i--, coordPtr += 2) {
	    double miter[4];
	    int j;
    
	    if (TkGetMiterPoints(coordPtr, coordPtr+2, coordPtr+4,
		    (double) linePtr->width, miter, miter+2)) {
		for (j = 0; j < 4; j += 2) {
		    TkIncludePoint(canvasPtr, (Tk_Item *) linePtr, miter+j);
		}
	    }
	}
    }

    /*
     * Add in the sizes of arrowheads, if any.
     */

    if (linePtr->arrow != noneUid) {
	if (linePtr->arrow != lastUid) {
	    for (i = 0, coordPtr = linePtr->firstArrowPtr; i < PTS_IN_ARROW;
		    i++, coordPtr += 2) {
		TkIncludePoint(canvasPtr, (Tk_Item *) linePtr, coordPtr);
	    }
	}
	if (linePtr->arrow != firstUid) {
	    for (i = 0, coordPtr = linePtr->lastArrowPtr; i < PTS_IN_ARROW;
		    i++, coordPtr += 2) {
		TkIncludePoint(canvasPtr, (Tk_Item *) linePtr, coordPtr);
	    }
	}
    }

    /*
     * Add one more pixel of fudge factor just to be safe (e.g.
     * X may round differently than we do).
     */

    linePtr->header.x1 -= 1;
    linePtr->header.x2 += 1;
    linePtr->header.y1 -= 1;
    linePtr->header.y2 += 1;
}

/*
 *--------------------------------------------------------------
 *
 * DisplayLine --
 *
 *	This procedure is invoked to draw a line item in a given
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
DisplayLine(canvasPtr, itemPtr, drawable)
    register Tk_Canvas *canvasPtr;	/* Canvas that contains item. */
    Tk_Item *itemPtr;			/* Item to be displayed. */
    Drawable drawable;			/* Pixmap or window in which to draw
					 * item. */
{
    register LineItem *linePtr = (LineItem *) itemPtr;
    XPoint staticPoints[MAX_STATIC_POINTS];
    XPoint *pointPtr;
    register XPoint *pPtr;
    register double *coordPtr;
    int i, numPoints;

    if (linePtr->gc == None) {
	return;
    }

    /*
     * Build up an array of points in screen coordinates.  Use a
     * static array unless the line has an enormous number of points;
     * in this case, dynamically allocate an array.  For smoothed lines,
     * generate the curve points on each redisplay.
     */

    if ((linePtr->smooth) && (linePtr->numPoints > 2)) {
	numPoints = 1 + linePtr->numPoints*linePtr->splineSteps;
    } else {
	numPoints = linePtr->numPoints;
    }

    if (numPoints <= MAX_STATIC_POINTS) {
	pointPtr = staticPoints;
    } else {
	pointPtr = (XPoint *) ckalloc((unsigned) (numPoints * sizeof(XPoint)));
    }

    if (linePtr->smooth) {
	numPoints = TkMakeBezierCurve(canvasPtr, linePtr->coordPtr,
		linePtr->numPoints, linePtr->splineSteps, pointPtr,
		(double *) NULL);
    } else {
	for (i = 0, coordPtr = linePtr->coordPtr, pPtr = pointPtr;
		i < linePtr->numPoints;  i += 1, coordPtr += 2, pPtr++) {
	    pPtr->x = SCREEN_X(canvasPtr, *coordPtr);
	    pPtr->y = SCREEN_Y(canvasPtr, coordPtr[1]);
	}
    }

    /*
     * Display line, the free up line storage if it was dynamically
     * allocated.
     */

    XDrawLines(Tk_Display(canvasPtr->tkwin), drawable, linePtr->gc,
	    pointPtr, numPoints, CoordModeOrigin);
    if (pointPtr != staticPoints) {
	ckfree((char *) pointPtr);
    }

    /*
     * Display arrowheads, if they are wanted.
     */

    if (linePtr->arrow != noneUid) {
	if (linePtr->arrow != lastUid) {
	    TkFillPolygon(canvasPtr, linePtr->firstArrowPtr, PTS_IN_ARROW,
		    drawable, linePtr->gc);
	}
	if (linePtr->arrow != firstUid) {
	    TkFillPolygon(canvasPtr, linePtr->lastArrowPtr, PTS_IN_ARROW,
		    drawable, linePtr->gc);
	}
    }
}

/*
 *--------------------------------------------------------------
 *
 * LineToPoint --
 *
 *	Computes the distance from a given point to a given
 *	line, in canvas units.
 *
 * Results:
 *	The return value is 0 if the point whose x and y coordinates
 *	are pointPtr[0] and pointPtr[1] is inside the line.  If the
 *	point isn't inside the line then the return value is the
 *	distance from the point to the line.
 *
 * Side effects:
 *	None.
 *
 *--------------------------------------------------------------
 */

	/* ARGSUSED */
static double
LineToPoint(canvasPtr, itemPtr, pointPtr)
    Tk_Canvas *canvasPtr;	/* Canvas containing item. */
    Tk_Item *itemPtr;		/* Item to check against point. */
    double *pointPtr;		/* Pointer to x and y coordinates. */
{
    register LineItem *linePtr = (LineItem *) itemPtr;
    register double *coordPtr, *linePoints;
    double staticSpace[2*MAX_STATIC_POINTS];
    double poly[10];
    double bestDist, dist;
    int numPoints, count;
    int changedMiterToBevel;	/* Non-zero means that a mitered corner
				 * had to be treated as beveled after all
				 * because the angle was < 11 degrees. */

    bestDist = 1.0e40;

    /*
     * Handle smoothed lines by generating an expanded set of points
     * against which to do the check.
     */

    if ((linePtr->smooth) && (linePtr->numPoints > 2)) {
	numPoints = 1 + linePtr->numPoints*linePtr->splineSteps;
	if (numPoints <= MAX_STATIC_POINTS) {
	    linePoints = staticSpace;
	} else {
	    linePoints = (double *) ckalloc((unsigned)
		    (2*numPoints*sizeof(double)));
	}
	numPoints = TkMakeBezierCurve(canvasPtr, linePtr->coordPtr,
		linePtr->numPoints, linePtr->splineSteps, (XPoint *) NULL,
		linePoints);
    } else {
	numPoints = linePtr->numPoints;
	linePoints = linePtr->coordPtr;
    }

    /*
     * The overall idea is to iterate through all of the edges of
     * the line, computing a polygon for each edge and testing the
     * point against that polygon.  In addition, there are additional
     * tests to deal with rounded joints and caps.
     */

    changedMiterToBevel = 0;
    for (count = numPoints, coordPtr = linePoints; count >= 2;
	    count--, coordPtr += 2) {

	/*
	 * If rounding is done around the first point then compute
	 * the distance between the point and the point.
	 */

	if (((linePtr->capStyle == CapRound) && (count == numPoints))
		|| ((linePtr->joinStyle == JoinRound)
			&& (count != numPoints))) {
	    dist = hypot(coordPtr[0] - pointPtr[0], coordPtr[1] - pointPtr[1])
		    - linePtr->width/2.0;
	    if (dist <= 0.0) {
		bestDist = 0.0;
		goto done;
	    } else if (dist < bestDist) {
		bestDist = dist;
	    }
	}

	/*
	 * Compute the polygonal shape corresponding to this edge,
	 * consisting of two points for the first point of the edge
	 * and two points for the last point of the edge.
	 */

	if (count == numPoints) {
	    TkGetButtPoints(coordPtr+2, coordPtr, (double) linePtr->width,
		    linePtr->capStyle == CapProjecting, poly, poly+2);
	} else if ((linePtr->joinStyle == JoinMiter) && !changedMiterToBevel) {
	    poly[0] = poly[6];
	    poly[1] = poly[7];
	    poly[2] = poly[4];
	    poly[3] = poly[5];
	} else {
	    TkGetButtPoints(coordPtr+2, coordPtr, (double) linePtr->width, 0,
		    poly, poly+2);

	    /*
	     * If this line uses beveled joints, then check the distance
	     * to a polygon comprising the last two points of the previous
	     * polygon and the first two from this polygon;  this checks
	     * the wedges that fill the mitered joint.
	     */

	    if ((linePtr->joinStyle == JoinBevel) || changedMiterToBevel) {
		poly[8] = poly[0];
		poly[9] = poly[1];
		dist = TkPolygonToPoint(poly, 5, pointPtr);
		if (dist <= 0.0) {
		    bestDist = 0.0;
		    goto done;
		} else if (dist < bestDist) {
		    bestDist = dist;
		}
		changedMiterToBevel = 0;
	    }
	}
	if (count == 2) {
	    TkGetButtPoints(coordPtr, coordPtr+2, (double) linePtr->width,
		    linePtr->capStyle == CapProjecting, poly+4, poly+6);
	} else if (linePtr->joinStyle == JoinMiter) {
	    if (TkGetMiterPoints(coordPtr, coordPtr+2, coordPtr+4,
		    (double) linePtr->width, poly+4, poly+6) == 0) {
		changedMiterToBevel = 1;
		TkGetButtPoints(coordPtr, coordPtr+2, (double) linePtr->width,
			0, poly+4, poly+6);
	    }
	} else {
	    TkGetButtPoints(coordPtr, coordPtr+2, (double) linePtr->width, 0,
		    poly+4, poly+6);
	}
	poly[8] = poly[0];
	poly[9] = poly[1];
	dist = TkPolygonToPoint(poly, 5, pointPtr);
	if (dist <= 0.0) {
	    bestDist = 0.0;
	    goto done;
	} else if (dist < bestDist) {
	    bestDist = dist;
	}
    }

    /*
     * If caps are rounded, check the distance to the cap around the
     * final end point of the line.
     */

    if (linePtr->capStyle == CapRound) {
	dist = hypot(coordPtr[0] - pointPtr[0], coordPtr[1] - pointPtr[1])
		- linePtr->width/2.0;
	if (dist <= 0.0) {
	    bestDist = 0.0;
	    goto done;
	} else if (dist < bestDist) {
	    bestDist = dist;
	}
    }

    /*
     * If there are arrowheads, check the distance to the arrowheads.
     */

    if (linePtr->arrow != noneUid) {
	if (linePtr->arrow != lastUid) {
	    dist = TkPolygonToPoint(linePtr->firstArrowPtr, PTS_IN_ARROW,
		    pointPtr);
	    if (dist <= 0.0) {
		bestDist = 0.0;
		goto done;
	    } else if (dist < bestDist) {
		bestDist = dist;
	    }
	}
	if (linePtr->arrow != firstUid) {
	    dist = TkPolygonToPoint(linePtr->lastArrowPtr, PTS_IN_ARROW,
		    pointPtr);
	    if (dist <= 0.0) {
		bestDist = 0.0;
		goto done;
	    } else if (dist < bestDist) {
		bestDist = dist;
	    }
	}
    }

    done:
    if ((linePoints != staticSpace) && (linePoints != linePtr->coordPtr)) {
	ckfree((char *) linePoints);
    }
    return bestDist;
}

/*
 *--------------------------------------------------------------
 *
 * LineToArea --
 *
 *	This procedure is called to determine whether an item
 *	lies entirely inside, entirely outside, or overlapping
 *	a given rectangular area.
 *
 * Results:
 *	-1 is returned if the item is entirely outside the
 *	area, 0 if it overlaps, and 1 if it is entirely
 *	inside the given area.
 *
 * Side effects:
 *	None.
 *
 *--------------------------------------------------------------
 */

	/* ARGSUSED */
static int
LineToArea(canvasPtr, itemPtr, rectPtr)
    Tk_Canvas *canvasPtr;	/* Canvas containing item. */
    Tk_Item *itemPtr;		/* Item to check against line. */
    double *rectPtr;
{
    register LineItem *linePtr = (LineItem *) itemPtr;
    register double *coordPtr;
    double staticSpace[2*MAX_STATIC_POINTS];
    double *linePoints, poly[10];
    double radius;
    int numPoints, count;
    int changedMiterToBevel;	/* Non-zero means that a mitered corner
				 * had to be treated as beveled after all
				 * because the angle was < 11 degrees. */
    int inside;			/* Tentative guess about what to return,
				 * based on all points seen so far:  one
				 * means everything seen so far was
				 * inside the area;  -1 means everything
				 * was outside the area.  0 means overlap
				 * has been found. */ 

    radius = linePtr->width/2.0;
    inside = -1;

    /*
     * Handle smoothed lines by generating an expanded set of points
     * against which to do the check.
     */

    if ((linePtr->smooth) && (linePtr->numPoints > 2)) {
	numPoints = 1 + linePtr->numPoints*linePtr->splineSteps;
	if (numPoints <= MAX_STATIC_POINTS) {
	    linePoints = staticSpace;
	} else {
	    linePoints = (double *) ckalloc((unsigned)
		    (2*numPoints*sizeof(double)));
	}
	numPoints = TkMakeBezierCurve(canvasPtr, linePtr->coordPtr,
		linePtr->numPoints, linePtr->splineSteps, (XPoint *) NULL,
		linePoints);
    } else {
	numPoints = linePtr->numPoints;
	linePoints = linePtr->coordPtr;
    }

    coordPtr = linePoints;
    if ((coordPtr[0] >= rectPtr[0]) && (coordPtr[0] <= rectPtr[2])
	    && (coordPtr[1] >= rectPtr[1]) && (coordPtr[1] <= rectPtr[3])) {
	inside = 1;
    }

    /*
     * Iterate through all of the edges of the line, computing a polygon
     * for each edge and testing the area against that polygon.  In
     * addition, there are additional tests to deal with rounded joints
     * and caps.
     */

    changedMiterToBevel = 0;
    for (count = numPoints; count >= 2; count--, coordPtr += 2) {

	/*
	 * If rounding is done around the first point of the edge
	 * then test a circular region around the point with the
	 * area.
	 */

	if (((linePtr->capStyle == CapRound) && (count == numPoints))
		|| ((linePtr->joinStyle == JoinRound)
		&& (count != numPoints))) {
	    poly[0] = coordPtr[0] - radius;
	    poly[1] = coordPtr[1] - radius;
	    poly[2] = coordPtr[0] + radius;
	    poly[3] = coordPtr[1] + radius;
	    if (TkOvalToArea(poly, rectPtr) != inside) {
		inside = 0;
		goto done;
	    }
	}

	/*
	 * Compute the polygonal shape corresponding to this edge,
	 * consisting of two points for the first point of the edge
	 * and two points for the last point of the edge.
	 */

	if (count == numPoints) {
	    TkGetButtPoints(coordPtr+2, coordPtr, (double) linePtr->width,
		    linePtr->capStyle == CapProjecting, poly, poly+2);
	} else if ((linePtr->joinStyle == JoinMiter) && !changedMiterToBevel) {
	    poly[0] = poly[6];
	    poly[1] = poly[7];
	    poly[2] = poly[4];
	    poly[3] = poly[5];
	} else {
	    TkGetButtPoints(coordPtr+2, coordPtr, (double) linePtr->width, 0,
		    poly, poly+2);

	    /*
	     * If the last joint was beveled, then also check a
	     * polygon comprising the last two points of the previous
	     * polygon and the first two from this polygon;  this checks
	     * the wedges that fill the beveled joint.
	     */

	    if ((linePtr->joinStyle == JoinBevel) || changedMiterToBevel) {
		poly[8] = poly[0];
		poly[9] = poly[1];
		if (TkPolygonToArea(poly, 5, rectPtr) != inside) {
		    inside = 0;
		    goto done;
		}
		changedMiterToBevel = 0;
	    }
	}
	if (count == 2) {
	    TkGetButtPoints(coordPtr, coordPtr+2, (double) linePtr->width,
		    linePtr->capStyle == CapProjecting, poly+4, poly+6);
	} else if (linePtr->joinStyle == JoinMiter) {
	    if (TkGetMiterPoints(coordPtr, coordPtr+2, coordPtr+4,
		    (double) linePtr->width, poly+4, poly+6) == 0) {
		changedMiterToBevel = 1;
		TkGetButtPoints(coordPtr, coordPtr+2, (double) linePtr->width,
			0, poly+4, poly+6);
	    }
	} else {
	    TkGetButtPoints(coordPtr, coordPtr+2, (double) linePtr->width, 0,
		    poly+4, poly+6);
	}
	poly[8] = poly[0];
	poly[9] = poly[1];
	if (TkPolygonToArea(poly, 5, rectPtr) != inside) {
	    inside = 0;
	    goto done;
	}
    }

    /*
     * If caps are rounded, check the cap around the final point
     * of the line.
     */

    if (linePtr->capStyle == CapRound) {
	poly[0] = coordPtr[0] - radius;
	poly[1] = coordPtr[1] - radius;
	poly[2] = coordPtr[0] + radius;
	poly[3] = coordPtr[1] + radius;
	if (TkOvalToArea(poly, rectPtr) != inside) {
	    inside = 0;
	    goto done;
	}
    }

    /*
     * Check arrowheads, if any.
     */

    if (linePtr->arrow != noneUid) {
	if (linePtr->arrow != lastUid) {
	    if (TkPolygonToArea(linePtr->firstArrowPtr, PTS_IN_ARROW,
		    rectPtr) != inside) {
		inside = 0;
		goto done;
	    }
	}
	if (linePtr->arrow != firstUid) {
	    if (TkPolygonToArea(linePtr->lastArrowPtr, PTS_IN_ARROW,
		    rectPtr) != inside) {
		inside = 0;
		goto done;
	    }
	}
    }

    done:
    if ((linePoints != staticSpace) && (linePoints != linePtr->coordPtr)) {
	ckfree((char *) linePoints);
    }
    return inside;
}

/*
 *--------------------------------------------------------------
 *
 * ScaleLine --
 *
 *	This procedure is invoked to rescale a line item.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The line referred to by itemPtr is rescaled so that the
 *	following transformation is applied to all point
 *	coordinates:
 *		x' = originX + scaleX*(x-originX)
 *		y' = originY + scaleY*(y-originY)
 *
 *--------------------------------------------------------------
 */

static void
ScaleLine(canvasPtr, itemPtr, originX, originY, scaleX, scaleY)
    Tk_Canvas *canvasPtr;		/* Canvas containing line. */
    Tk_Item *itemPtr;			/* Line to be scaled. */
    double originX, originY;		/* Origin about which to scale rect. */
    double scaleX;			/* Amount to scale in X direction. */
    double scaleY;			/* Amount to scale in Y direction. */
{
    LineItem *linePtr = (LineItem *) itemPtr;
    register double *coordPtr;
    int i;

    for (i = 0, coordPtr = linePtr->coordPtr; i < linePtr->numPoints;
	    i++, coordPtr += 2) {
	coordPtr[0] = originX + scaleX*(*coordPtr - originX);
	coordPtr[1] = originY + scaleY*(coordPtr[1] - originY);
    }
    if (linePtr->firstArrowPtr != NULL) {
	for (i = 0, coordPtr = linePtr->firstArrowPtr; i < PTS_IN_ARROW;
		i++, coordPtr += 2) {
	    coordPtr[0] = originX + scaleX*(coordPtr[0] - originX);
	    coordPtr[1] = originY + scaleY*(coordPtr[1] - originY);
	}
    }
    if (linePtr->lastArrowPtr != NULL) {
	for (i = 0, coordPtr = linePtr->lastArrowPtr; i < PTS_IN_ARROW;
		i++, coordPtr += 2) {
	    coordPtr[0] = originX + scaleX*(coordPtr[0] - originX);
	    coordPtr[1] = originY + scaleY*(coordPtr[1] - originY);
	}
    }
    ComputeLineBbox(canvasPtr, linePtr);
}

/*
 *--------------------------------------------------------------
 *
 * TranslateLine --
 *
 *	This procedure is called to move a line by a given amount.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The position of the line is offset by (xDelta, yDelta), and
 *	the bounding box is updated in the generic part of the item
 *	structure.
 *
 *--------------------------------------------------------------
 */

static void
TranslateLine(canvasPtr, itemPtr, deltaX, deltaY)
    Tk_Canvas *canvasPtr;		/* Canvas containing item. */
    Tk_Item *itemPtr;			/* Item that is being moved. */
    double deltaX, deltaY;		/* Amount by which item is to be
					 * moved. */
{
    LineItem *linePtr = (LineItem *) itemPtr;
    register double *coordPtr;
    int i;

    for (i = 0, coordPtr = linePtr->coordPtr; i < linePtr->numPoints;
	    i++, coordPtr += 2) {
	coordPtr[0] += deltaX;
	coordPtr[1] += deltaY;
    }
    if (linePtr->firstArrowPtr != NULL) {
	for (i = 0, coordPtr = linePtr->firstArrowPtr; i < PTS_IN_ARROW;
		i++, coordPtr += 2) {
	    coordPtr[0] += deltaX;
	    coordPtr[1] += deltaY;
	}
    }
    if (linePtr->lastArrowPtr != NULL) {
	for (i = 0, coordPtr = linePtr->lastArrowPtr; i < PTS_IN_ARROW;
		i++, coordPtr += 2) {
	    coordPtr[0] += deltaX;
	    coordPtr[1] += deltaY;
	}
    }
    ComputeLineBbox(canvasPtr, linePtr);
}

/*
 *--------------------------------------------------------------
 *
 * ParseArrowShape --
 *
 *	This procedure is called back during option parsing to
 *	parse arrow shape information.
 *
 * Results:
 *	The return value is a standard Tcl result:  TCL_OK means
 *	that the arrow shape information was parsed ok, and
 *	TCL_ERROR means it couldn't be parsed.
 *
 * Side effects:
 *	Arrow information in recordPtr is updated.
 *
 *--------------------------------------------------------------
 */

	/* ARGSUSED */
static int
ParseArrowShape(clientData, interp, tkwin, value, recordPtr, offset)
    ClientData clientData;	/* Not used. */
    Tcl_Interp *interp;		/* Used for error reporting. */
    Tk_Window tkwin;		/* Not used. */
    char *value;		/* Textual specification of arrow shape. */
    char *recordPtr;		/* Pointer to item record in which to
				 * store arrow information. */
    int offset;			/* Offset of shape information in widget
				 * record. */
{
    LineItem *linePtr = (LineItem *) recordPtr;
    double a, b, c;
    int argc;
    char **argv = NULL;

    if (offset != Tk_Offset(LineItem, arrowShapeA)) {
	panic("ParseArrowShape received bogus offset");
    }

    if (Tcl_SplitList(interp, value, &argc, &argv) != TCL_OK) {
	syntaxError:
	Tcl_ResetResult(interp);
	Tcl_AppendResult(interp, "bad arrow shape \"", value,
		"\": must be list with three numbers", (char *) NULL);
	if (argv != NULL) {
	    ckfree((char *) argv);
	}
	return TCL_ERROR;
    }
    if (argc != 3) {
	goto syntaxError;
    }
    if ((TkGetCanvasCoord(linePtr->canvasPtr, argv[0], &a) != TCL_OK)
	    || (TkGetCanvasCoord(linePtr->canvasPtr, argv[1], &b) != TCL_OK)
	    || (TkGetCanvasCoord(linePtr->canvasPtr, argv[2], &c) != TCL_OK)) {
	goto syntaxError;
    }
    linePtr->arrowShapeA = a;
    linePtr->arrowShapeB = b;
    linePtr->arrowShapeC = c;
    ckfree((char *) argv);
    return TCL_OK;
}

/*
 *--------------------------------------------------------------
 *
 * PrintArrowShape --
 *
 *	This procedure is a callback invoked by the configuration
 *	code to return a printable value describing an arrow shape.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	None.
 *
 *--------------------------------------------------------------
 */

    /* ARGSUSED */
static char *
PrintArrowShape(clientData, tkwin, recordPtr, offset, freeProcPtr)
    ClientData clientData;	/* Not used. */
    Tk_Window tkwin;		/* Window associated with linePtr's widget. */
    char *recordPtr;		/* Pointer to item record containing current
				 * shape information. */
    int offset;			/* Offset of arrow information in record. */
    Tcl_FreeProc **freeProcPtr;	/* Store address of procedure to call to
				 * free string here. */
{
    LineItem *linePtr = (LineItem *) recordPtr;
    char *buffer;

    buffer = ckalloc(120);
    sprintf(buffer, "%.5g %.5g %.5g", linePtr->arrowShapeA,
	    linePtr->arrowShapeB, linePtr->arrowShapeC);
    *freeProcPtr = (Tcl_FreeProc *) free;
    return buffer;
}

/*
 *--------------------------------------------------------------
 *
 * ConfigureArrows --
 *
 *	If arrowheads have been requested for a line, this
 *	procedure makes arrangements for the arrowheads.
 *
 * Results:
 *	A standard Tcl return value.  If an error occurs, then
 *	an error message is left in canvasPtr->interp->result.
 *
 * Side effects:
 *	Information in linePtr is set up for one or two arrowheads.
 *	the firstArrowPtr and lastArrowPtr polygons are allocated
 *	and initialized, if need be, and the end points of the line
 *	are adjusted so that a thick line doesn't stick out past
 *	the arrowheads.
 *
 *--------------------------------------------------------------
 */

	/* ARGSUSED */
static int
ConfigureArrows(canvasPtr, linePtr)
    Tk_Canvas *canvasPtr;		/* Canvas in which arrows will be
					 * displayed (interp and tkwin
					 * fields are needed). */
    register LineItem *linePtr;		/* Item to configure for arrows. */
{
    double *poly, *coordPtr;
    double dx, dy, length, sinTheta, cosTheta, temp, shapeC;
    double fracHeight;			/* Line width as fraction of
					 * arrowhead width. */
    double backup;			/* Distance to backup end points
					 * so the line ends in the middle
					 * of the arrowhead. */
    double vertX, vertY;		/* Position of arrowhead vertex. */

    /*
     * If there's an arrowhead on the first point of the line, compute
     * its polygon and adjust the first point of the line so that the
     * line doesn't stick out past the leading edge of the arrowhead.
     */

    shapeC = linePtr->arrowShapeC + linePtr->width/2.0;
    fracHeight = (linePtr->width/2.0)/shapeC;
    backup = fracHeight*linePtr->arrowShapeB
	    + linePtr->arrowShapeA*(1.0 - fracHeight)/2.0;
    if (linePtr->arrow != lastUid) {
	poly = linePtr->firstArrowPtr;
	if (poly == NULL) {
	    poly = (double *) ckalloc((unsigned)
		    (2*PTS_IN_ARROW*sizeof(double)));
	    poly[0] = poly[10] = linePtr->coordPtr[0];
	    poly[1] = poly[11] = linePtr->coordPtr[1];
	    linePtr->firstArrowPtr = poly;
	}
	dx = poly[0] - linePtr->coordPtr[2];
	dy = poly[1] - linePtr->coordPtr[3];
	length = hypot(dx, dy);
	if (length == 0) {
	    sinTheta = cosTheta = 0.0;
	} else {
	    sinTheta = dy/length;
	    cosTheta = dx/length;
	}
	vertX = poly[0] - linePtr->arrowShapeA*cosTheta;
	vertY = poly[1] - linePtr->arrowShapeA*sinTheta;
	temp = shapeC*sinTheta;
	poly[2] = poly[0] - linePtr->arrowShapeB*cosTheta + temp;
	poly[8] = poly[2] - 2*temp;
	temp = shapeC*cosTheta;
	poly[3] = poly[1] - linePtr->arrowShapeB*sinTheta - temp;
	poly[9] = poly[3] + 2*temp;
	poly[4] = poly[2]*fracHeight + vertX*(1.0-fracHeight);
	poly[5] = poly[3]*fracHeight + vertY*(1.0-fracHeight);
	poly[6] = poly[8]*fracHeight + vertX*(1.0-fracHeight);
	poly[7] = poly[9]*fracHeight + vertY*(1.0-fracHeight);

	/*
	 * Polygon done.  Now move the first point towards the second so
	 * that the corners at the end of the line are inside the
	 * arrowhead.
	 */

	linePtr->coordPtr[0] = poly[0] - backup*cosTheta;
	linePtr->coordPtr[1] = poly[1] - backup*sinTheta;
    }

    /*
     * Similar arrowhead calculation for the last point of the line.
     */

    if (linePtr->arrow != firstUid) {
	coordPtr = linePtr->coordPtr + 2*(linePtr->numPoints-2);
	poly = linePtr->lastArrowPtr;
	if (poly == NULL) {
	    poly = (double *) ckalloc((unsigned)
		    (2*PTS_IN_ARROW*sizeof(double)));
	    poly[0] = poly[10] = coordPtr[2];
	    poly[1] = poly[11] = coordPtr[3];
	    linePtr->lastArrowPtr = poly;
	}
	dx = poly[0] - coordPtr[0];
	dy = poly[1] - coordPtr[1];
	length = hypot(dx, dy);
	if (length == 0) {
	    sinTheta = cosTheta = 0.0;
	} else {
	    sinTheta = dy/length;
	    cosTheta = dx/length;
	}
	vertX = poly[0] - linePtr->arrowShapeA*cosTheta;
	vertY = poly[1] - linePtr->arrowShapeA*sinTheta;
	temp = shapeC*sinTheta;
	poly[2] = poly[0] - linePtr->arrowShapeB*cosTheta + temp;
	poly[8] = poly[2] - 2*temp;
	temp = shapeC*cosTheta;
	poly[3] = poly[1] - linePtr->arrowShapeB*sinTheta - temp;
	poly[9] = poly[3] + 2*temp;
	poly[4] = poly[2]*fracHeight + vertX*(1.0-fracHeight);
	poly[5] = poly[3]*fracHeight + vertY*(1.0-fracHeight);
	poly[6] = poly[8]*fracHeight + vertX*(1.0-fracHeight);
	poly[7] = poly[9]*fracHeight + vertY*(1.0-fracHeight);
	coordPtr[2] = poly[0] - backup*cosTheta;
	coordPtr[3] = poly[1] - backup*sinTheta;
    }

    return TCL_OK;
}
