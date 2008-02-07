/* 
 * tkScale.c --
 *
 *	This module implements a scale widgets for the Tk toolkit.
 *	A scale displays a slider that can be adjusted to change a
 *	value;  it also displays numeric labels and a textual label,
 *	if desired.
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
static char rcsid[] = "$Header: /user6/ouster/wish/RCS/tkScale.c,v 1.28 92/08/21 11:45:25 ouster Exp $ SPRITE (Berkeley)";
#endif

#include "tkconfig.h"
#include "default.h"
#include "tkint.h"

/*
 * A data structure of the following type is kept for each scale
 * widget managed by this file:
 */

typedef struct {
    Tk_Window tkwin;		/* Window that embodies the scale.  NULL
				 * means that the window has been destroyed
				 * but the data structures haven't yet been
				 * cleaned up.*/
    Tcl_Interp *interp;		/* Interpreter associated with scale. */
    Tk_Uid orientUid;		/* Orientation for window ("vertical" or
				 * "horizontal"). */
    int vertical;		/* Non-zero means vertical orientation,
				 * zero means horizontal. */
    int value;			/* Current value of scale. */
    int fromValue;		/* Value corresponding to left or top of
				 * scale. */
    int toValue;		/* Value corresponding to right or bottom
				 * of scale. */
    int tickInterval;		/* Distance between tick marks;  0 means
				 * don't display any tick marks. */
    char *command;		/* Command prefix to use when invoking Tcl
				 * commands because the scale value changed.
				 * NULL means don't invoke commands.
				 * Malloc'ed. */
    int commandLength;		/* Number of non-NULL bytes in command. */
    char *label;		/* Label to display above or to right of
				 * scale;  NULL means don't display a
				 * label.  Malloc'ed. */
    int labelLength;		/* Number of non-NULL chars. in label. */
    Tk_Uid state;		/* Normal or disabled.  Value cannot be
				 * changed when scale is disabled. */

    /*
     * Information used when displaying widget:
     */

    int borderWidth;		/* Width of 3-D border around window. */
    Tk_3DBorder bgBorder;	/* Used for drawing background. */
    Tk_3DBorder sliderBorder;	/* Used for drawing slider in normal mode. */
    Tk_3DBorder activeBorder;	/* Used for drawing slider when active (i.e.
				 * when mouse is in window). */
    XFontStruct *fontPtr;	/* Information about text font, or NULL. */
    XColor *textColorPtr;	/* Color for drawing text. */
    GC textGC;			/* GC for drawing text in normal mode. */
    int width;			/* Desired narrow dimension of scale,
				 * in pixels. */
    int length;			/* Desired long dimension of scale,
				 * in pixels. */
    int relief;			/* Indicates whether window as a whole is
				 * raised, sunken, or flat. */
    int offset;			/* Zero if relief is TK_RELIEF_FLAT,
				 * borderWidth otherwise.   Indicates how
				 * much interior stuff must be offset from
				 * outside edges to leave room for border. */
    int sliderLength;		/* Length of slider, measured in pixels along
				 * long dimension of scale. */
    int showValue;		/* Non-zero means to display the scale value
				 * below or to the left of the slider;  zero
				 * means don't display the value. */
    int tickPixels;		/* Number of pixels required for widest tick
				 * mark.  0 means don't display ticks.*/
    int valuePixels;		/* Number of pixels required for value text. */
    int labelPixels;		/* Number of pixels required for label.   0
				 * means don't display label. */

    /*
     * Miscellaneous information:
     */

    Cursor cursor;		/* Current cursor for window, or None. */
    int flags;			/* Various flags;  see below for
				 * definitions. */
} Scale;

/*
 * Flag bits for scales:
 *
 * REDRAW_SLIDER -		1 means slider (and numerical readout) need
 *				to be redrawn.
 * REDRAW_OTHER -		1 means other stuff besides slider and value
 *				need to be redrawn.
 * REDRAW_ALL -			1 means the entire widget needs to be redrawn.
 * ACTIVE -			1 means the widget is active (the mouse is
 *				in its window).
 * BUTTON_PRESSED -		1 means a button press is in progress, so
 *				slider should appear depressed and should be
 *				draggable.
 */

#define REDRAW_SLIDER		1
#define REDRAW_OTHER		2
#define REDRAW_ALL		3
#define ACTIVE			4
#define BUTTON_PRESSED		8

/*
 * Space to leave between scale area and text.
 */

#define SPACING 2

/*
 * Information used for argv parsing.
 */


static Tk_ConfigSpec configSpecs[] = {
    {TK_CONFIG_BORDER, "-activeforeground", "activeForeground", "Background",
	DEF_SCALE_ACTIVE_FG_COLOR, Tk_Offset(Scale, activeBorder),
	TK_CONFIG_COLOR_ONLY},
    {TK_CONFIG_BORDER, "-activeforeground", "activeForeground", "Background",
	DEF_SCALE_ACTIVE_FG_MONO, Tk_Offset(Scale, activeBorder),
	TK_CONFIG_MONO_ONLY},
    {TK_CONFIG_BORDER, "-background", "background", "Background",
	DEF_SCALE_BG_COLOR, Tk_Offset(Scale, bgBorder),
	TK_CONFIG_COLOR_ONLY},
    {TK_CONFIG_BORDER, "-background", "background", "Background",
	DEF_SCALE_BG_MONO, Tk_Offset(Scale, bgBorder),
	TK_CONFIG_MONO_ONLY},
    {TK_CONFIG_SYNONYM, "-bd", "borderWidth", (char *) NULL,
	(char *) NULL, 0, 0},
    {TK_CONFIG_SYNONYM, "-bg", "background", (char *) NULL,
	(char *) NULL, 0, 0},
    {TK_CONFIG_PIXELS, "-borderwidth", "borderWidth", "BorderWidth",
	DEF_SCALE_BORDER_WIDTH, Tk_Offset(Scale, borderWidth), 0},
    {TK_CONFIG_STRING, "-command", "command", "Command",
	(char *) NULL, Tk_Offset(Scale, command), 0},
    {TK_CONFIG_ACTIVE_CURSOR, "-cursor", "cursor", "Cursor",
	DEF_SCALE_CURSOR, Tk_Offset(Scale, cursor), TK_CONFIG_NULL_OK},
    {TK_CONFIG_SYNONYM, "-fg", "foreground", (char *) NULL,
	(char *) NULL, 0, 0},
    {TK_CONFIG_FONT, "-font", "font", "Font",
	DEF_SCALE_FONT, Tk_Offset(Scale, fontPtr),
	0},
    {TK_CONFIG_COLOR, "-foreground", "foreground", "Foreground",
	DEF_SCALE_FG_COLOR, Tk_Offset(Scale, textColorPtr),
	TK_CONFIG_COLOR_ONLY},
    {TK_CONFIG_COLOR, "-foreground", "foreground", "Foreground",
	DEF_SCALE_FG_MONO, Tk_Offset(Scale, textColorPtr),
	TK_CONFIG_MONO_ONLY},
    {TK_CONFIG_INT, "-from", "from", "From",
	DEF_SCALE_FROM, Tk_Offset(Scale, fromValue), 0},
    {TK_CONFIG_STRING, "-label", "label", "Label",
	DEF_SCALE_LABEL, Tk_Offset(Scale, label), 0},
    {TK_CONFIG_PIXELS, "-length", "length", "Length",
	DEF_SCALE_LENGTH, Tk_Offset(Scale, length), 0},
    {TK_CONFIG_UID, "-orient", "orient", "Orient",
	DEF_SCALE_ORIENT, Tk_Offset(Scale, orientUid), 0},
    {TK_CONFIG_RELIEF, "-relief", "relief", "Relief",
	DEF_SCALE_RELIEF, Tk_Offset(Scale, relief), 0},
    {TK_CONFIG_BOOLEAN, "-showvalue", "showValue", "ShowValue",
	DEF_SCALE_SHOW_VALUE, Tk_Offset(Scale, showValue), 0},
    {TK_CONFIG_BORDER, "-sliderforeground", "sliderForeground", "Background",
	DEF_SCALE_SLIDER_FG_COLOR, Tk_Offset(Scale, sliderBorder),
	TK_CONFIG_COLOR_ONLY},
    {TK_CONFIG_BORDER, "-sliderforeground", "sliderForeground", "Background",
	DEF_SCALE_SLIDER_FG_MONO, Tk_Offset(Scale, sliderBorder), 
	TK_CONFIG_MONO_ONLY},
    {TK_CONFIG_PIXELS, "-sliderlength", "sliderLength", "SliderLength",
	DEF_SCALE_SLIDER_LENGTH, Tk_Offset(Scale, sliderLength), 0},
    {TK_CONFIG_UID, "-state", "state", "State",
	DEF_SCALE_STATE, Tk_Offset(Scale, state), 0},
    {TK_CONFIG_INT, "-tickinterval", "tickInterval", "TickInterval",
	DEF_SCALE_TICK_INTERVAL, Tk_Offset(Scale, tickInterval), 0},
    {TK_CONFIG_INT, "-to", "to", "To",
	DEF_SCALE_TO, Tk_Offset(Scale, toValue), 0},
    {TK_CONFIG_PIXELS, "-width", "width", "Width",
	DEF_SCALE_WIDTH, Tk_Offset(Scale, width), 0},
    {TK_CONFIG_END, (char *) NULL, (char *) NULL, (char *) NULL,
	(char *) NULL, 0, 0}
};

/*
 * Forward declarations for procedures defined later in this file:
 */

static void		ComputeScaleGeometry _ANSI_ARGS_((Scale *scalePtr));
static int		ConfigureScale _ANSI_ARGS_((Tcl_Interp *interp,
			    Scale *scalePtr, int argc, char **argv,
			    int flags));
static void		DestroyScale _ANSI_ARGS_((ClientData clientData));
static void		DisplayHorizontalScale _ANSI_ARGS_((
			    ClientData clientData));
static void		DisplayHorizontalValue _ANSI_ARGS_((Scale *scalePtr,
			    int value, int bottom));
static void		DisplayVerticalScale _ANSI_ARGS_((
			    ClientData clientData));
static void		DisplayVerticalValue _ANSI_ARGS_((Scale *scalePtr,
			    int value, int rightEdge));
static void		EventuallyRedrawScale _ANSI_ARGS_((Scale *scalePtr,
			    int what));
static int		PixelToValue _ANSI_ARGS_((Scale *scalePtr, int x,
			    int y));
static void		ScaleEventProc _ANSI_ARGS_((ClientData clientData,
			    XEvent *eventPtr));
static void		ScaleMouseProc _ANSI_ARGS_((ClientData clientData,
			    XEvent *eventPtr));
static int		ScaleWidgetCmd _ANSI_ARGS_((ClientData clientData,
			    Tcl_Interp *interp, int argc, char **argv));
static void		SetScaleValue _ANSI_ARGS_((Scale *scalePtr,
			    int value));
static int		ValueToPixel _ANSI_ARGS_((Scale *scalePtr, int value));

/*
 *--------------------------------------------------------------
 *
 * Tk_ScaleCmd --
 *
 *	This procedure is invoked to process the "scale" Tcl
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
Tk_ScaleCmd(clientData, interp, argc, argv)
    ClientData clientData;		/* Main window associated with
				 * interpreter. */
    Tcl_Interp *interp;		/* Current interpreter. */
    int argc;			/* Number of arguments. */
    char **argv;		/* Argument strings. */
{
    Tk_Window tkwin = (Tk_Window) clientData;
    register Scale *scalePtr;
    Tk_Window new;

    if (argc < 2) {
	Tcl_AppendResult(interp, "wrong # args: should be \"",
		argv[0], " pathName ?options?\"", (char *) NULL);
	return TCL_ERROR;
    }

    new = Tk_CreateWindowFromPath(interp, tkwin, argv[1], (char *) NULL);
    if (new == NULL) {
	return TCL_ERROR;
    }

    /*
     * Initialize fields that won't be initialized by ConfigureScale,
     * or which ConfigureScale expects to have reasonable values
     * (e.g. resource pointers).
     */

    scalePtr = (Scale *) ckalloc(sizeof(Scale));
    scalePtr->tkwin = new;
    scalePtr->interp = interp;
    scalePtr->value = 0;
    scalePtr->command = NULL;
    scalePtr->label = NULL;
    scalePtr->state = tkNormalUid;
    scalePtr->bgBorder = NULL;
    scalePtr->sliderBorder = NULL;
    scalePtr->activeBorder = NULL;
    scalePtr->fontPtr = NULL;
    scalePtr->textColorPtr = NULL;
    scalePtr->textGC = None;
    scalePtr->cursor = None;
    scalePtr->flags = 0;

    Tk_SetClass(scalePtr->tkwin, "Scale");
    Tk_CreateEventHandler(scalePtr->tkwin, ExposureMask|StructureNotifyMask,
	    ScaleEventProc, (ClientData) scalePtr);
    Tk_CreateEventHandler(scalePtr->tkwin, EnterWindowMask|LeaveWindowMask
	    |PointerMotionMask|ButtonPressMask|ButtonReleaseMask,
	    ScaleMouseProc, (ClientData) scalePtr);
    Tcl_CreateCommand(interp, Tk_PathName(scalePtr->tkwin), ScaleWidgetCmd,
	    (ClientData) scalePtr, (void (*)()) NULL);
    if (ConfigureScale(interp, scalePtr, argc-2, argv+2, 0) != TCL_OK) {
	goto error;
    }

    interp->result = Tk_PathName(scalePtr->tkwin);
    return TCL_OK;

    error:
    Tk_DestroyWindow(scalePtr->tkwin);
    return TCL_ERROR;
}

/*
 *--------------------------------------------------------------
 *
 * ScaleWidgetCmd --
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
ScaleWidgetCmd(clientData, interp, argc, argv)
    ClientData clientData;		/* Information about scale
					 * widget. */
    Tcl_Interp *interp;			/* Current interpreter. */
    int argc;				/* Number of arguments. */
    char **argv;			/* Argument strings. */
{
    register Scale *scalePtr = (Scale *) clientData;
    int result = TCL_OK;
    int length;
    char c;

    if (argc < 2) {
	Tcl_AppendResult(interp, "wrong # args: should be \"",
		argv[0], " option ?arg arg ...?\"", (char *) NULL);
	return TCL_ERROR;
    }
    Tk_Preserve((ClientData) scalePtr);
    c = argv[1][0];
    length = strlen(argv[1]);
    if ((c == 'c') && (strncmp(argv[1], "configure", length) == 0)) {
	if (argc == 2) {
	    result = Tk_ConfigureInfo(interp, scalePtr->tkwin, configSpecs,
		    (char *) scalePtr, (char *) NULL, 0);
	} else if (argc == 3) {
	    result = Tk_ConfigureInfo(interp, scalePtr->tkwin, configSpecs,
		    (char *) scalePtr, argv[2], 0);
	} else {
	    result = ConfigureScale(interp, scalePtr, argc-2, argv+2,
		    TK_CONFIG_ARGV_ONLY);
	}
    } else if ((c == 'g') && (strncmp(argv[1], "get", length) == 0)) {
	if (argc != 2) {
	    Tcl_AppendResult(interp, "wrong # args: should be \"",
		    argv[0], " get\"", (char *) NULL);
	    goto error;
	}
	sprintf(interp->result, "%d", scalePtr->value);
    } else if ((c == 's') && (strncmp(argv[1], "set", length) == 0)) {
	int value;

	if (argc != 3) {
	    Tcl_AppendResult(interp, "wrong # args: should be \"",
		    argv[0], " set value\"", (char *) NULL);
	    goto error;
	}
	if (Tcl_GetInt(interp, argv[2], &value) != TCL_OK) {
	    goto error;
	}
	if (scalePtr->state == tkNormalUid) {
	    if ((value < scalePtr->fromValue)
		    ^ (scalePtr->toValue < scalePtr->fromValue)) {
		value = scalePtr->fromValue;
	    }
	    if ((value > scalePtr->toValue)
		    ^ (scalePtr->toValue < scalePtr->fromValue)) {
		value = scalePtr->toValue;
	    }
	    SetScaleValue(scalePtr, value);
	}
    } else {
	Tcl_AppendResult(interp, "bad option \"", argv[1],
		"\":  must be configure, get, or set", (char *) NULL);
	goto error;
    }
    Tk_Release((ClientData) scalePtr);
    return result;

    error:
    Tk_Release((ClientData) scalePtr);
    return TCL_ERROR;
}

/*
 *----------------------------------------------------------------------
 *
 * DestroyScale --
 *
 *	This procedure is invoked by Tk_EventuallyFree or Tk_Release
 *	to clean up the internal structure of a button at a safe time
 *	(when no-one is using it anymore).
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Everything associated with the scale is freed up.
 *
 *----------------------------------------------------------------------
 */

static void
DestroyScale(clientData)
    ClientData clientData;	/* Info about scale widget. */
{
    register Scale *scalePtr = (Scale *) clientData;

    if (scalePtr->command != NULL) {
	ckfree(scalePtr->command);
    }
    if (scalePtr->label != NULL) {
	ckfree(scalePtr->label);
    }
    if (scalePtr->bgBorder != NULL) {
	Tk_Free3DBorder(scalePtr->bgBorder);
    }
    if (scalePtr->sliderBorder != NULL) {
	Tk_Free3DBorder(scalePtr->sliderBorder);
    }
    if (scalePtr->activeBorder != NULL) {
	Tk_Free3DBorder(scalePtr->activeBorder);
    }
    if (scalePtr->fontPtr != NULL) {
	Tk_FreeFontStruct(scalePtr->fontPtr);
    }
    if (scalePtr->textColorPtr != NULL) {
	Tk_FreeColor(scalePtr->textColorPtr);
    }
    if (scalePtr->textGC != None) {
	Tk_FreeGC(scalePtr->textGC);
    }
    if (scalePtr->cursor != None) {
	Tk_FreeCursor(scalePtr->cursor);
    }
    ckfree((char *) scalePtr);
}

/*
 *----------------------------------------------------------------------
 *
 * ConfigureScale --
 *
 *	This procedure is called to process an argv/argc list, plus
 *	the Tk option database, in order to configure (or
 *	reconfigure) a scale widget.
 *
 * Results:
 *	The return value is a standard Tcl result.  If TCL_ERROR is
 *	returned, then interp->result contains an error message.
 *
 * Side effects:
 *	Configuration information, such as colors, border width,
 *	etc. get set for scalePtr;  old resources get freed,
 *	if there were any.
 *
 *----------------------------------------------------------------------
 */

static int
ConfigureScale(interp, scalePtr, argc, argv, flags)
    Tcl_Interp *interp;		/* Used for error reporting. */
    register Scale *scalePtr;	/* Information about widget;  may or may
				 * not already have values for some fields. */
    int argc;			/* Number of valid entries in argv. */
    char **argv;		/* Arguments. */
    int flags;			/* Flags to pass to Tk_ConfigureWidget. */
{
    XGCValues gcValues;
    GC newGC;
    int length;

    if (Tk_ConfigureWidget(interp, scalePtr->tkwin, configSpecs,
	    argc, argv, (char *) scalePtr, flags) != TCL_OK) {
	return TCL_ERROR;
    }

    /*
     * A few options need special processing, such as parsing the
     * orientation or setting the background from a 3-D border.
     */

    length = strlen(scalePtr->orientUid);
    if (strncmp(scalePtr->orientUid, "vertical", length) == 0) {
	scalePtr->vertical = 1;
    } else if (strncmp(scalePtr->orientUid, "horizontal", length) == 0) {
	scalePtr->vertical = 0;
    } else {
	Tcl_AppendResult(interp, "bad orientation \"", scalePtr->orientUid,
		"\": must be vertical or horizontal", (char *) NULL);
	return TCL_ERROR;
    }

    if ((scalePtr->state != tkNormalUid)
	    && (scalePtr->state != tkDisabledUid)) {
	Tcl_AppendResult(interp, "bad state value \"", scalePtr->state,
		"\":  must be normal or disabled", (char *) NULL);
	scalePtr->state = tkNormalUid;
	return TCL_ERROR;
    }

    /*
     * Make sure that the tick interval has the right sign so that
     * addition moves from fromValue to toValue.
     */

    if ((scalePtr->tickInterval < 0)
	    ^ ((scalePtr->toValue - scalePtr->fromValue) <  0)) {
	scalePtr->tickInterval = -scalePtr->tickInterval;
    }

    /*
     * Set the scale value to itself;  all this does is to make sure
     * that the scale's value is within the new acceptable range for
     * the scale.
     */

    SetScaleValue(scalePtr, scalePtr->value);

    if (scalePtr->command != NULL) {
	scalePtr->commandLength = strlen(scalePtr->command);
    } else {
	scalePtr->commandLength = 0;
    }

    if (scalePtr->label != NULL) {
	scalePtr->labelLength = strlen(scalePtr->label);
    } else {
	scalePtr->labelLength = 0;
    }

    Tk_SetBackgroundFromBorder(scalePtr->tkwin, scalePtr->bgBorder);

    gcValues.font = scalePtr->fontPtr->fid;
    gcValues.foreground = scalePtr->textColorPtr->pixel;
    newGC = Tk_GetGC(scalePtr->tkwin, GCForeground|GCFont, &gcValues);
    if (scalePtr->textGC != None) {
	Tk_FreeGC(scalePtr->textGC);
    }
    scalePtr->textGC = newGC;

    if (scalePtr->relief != TK_RELIEF_FLAT) {
	scalePtr->offset = scalePtr->borderWidth;
    } else {
	scalePtr->offset = 0;
    }

    /*
     * Recompute display-related information, and let the geometry
     * manager know how much space is needed now.
     */

    ComputeScaleGeometry(scalePtr);

    EventuallyRedrawScale(scalePtr, REDRAW_ALL);
    return TCL_OK;
}

/*
 *----------------------------------------------------------------------
 *
 * ComputeScaleGeometry --
 *
 *	This procedure is called to compute various geometrical
 *	information for a scale, such as where various things get
 *	displayed.  It's called when the window is reconfigured.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Display-related numbers get changed in *scrollPtr.  The
 *	geometry manager gets told about the window's preferred size.
 *
 *----------------------------------------------------------------------
 */

static void
ComputeScaleGeometry(scalePtr)
    register Scale *scalePtr;		/* Information about widget. */
{
    XCharStruct bbox;
    char valueString[30];
    int dummy, lineHeight;

    /*
     * Horizontal scales are simpler than vertical ones because
     * all sizes are the same (the height of a line of text);
     * handle them first and then quit.
     */

    if (!scalePtr->vertical) {
	lineHeight = scalePtr->fontPtr->ascent + scalePtr->fontPtr->descent;
	if (scalePtr->tickInterval != 0) {
	    scalePtr->tickPixels = lineHeight;
	} else {
	    scalePtr->tickPixels = 0;
	}
	if (scalePtr->showValue) {
	    scalePtr->valuePixels = lineHeight + SPACING;
	} else {
	    scalePtr->valuePixels = 0;
	}
	if (scalePtr->labelLength != 0) {
	    scalePtr->labelPixels = lineHeight;
	} else {
	    scalePtr->labelPixels = 0;
	}

	Tk_GeometryRequest(scalePtr->tkwin,
		scalePtr->length + 2*scalePtr->offset,
		scalePtr->tickPixels + scalePtr->valuePixels
		+ scalePtr->width + 2*scalePtr->borderWidth
		+ scalePtr->labelPixels + 2*scalePtr->offset);
	Tk_SetInternalBorder(scalePtr->tkwin, scalePtr->borderWidth);
	return;
    }

    /*
     * Vertical scale:  compute the amount of space needed for tick marks
     * and current value by formatting strings for the two end points;
     * use whichever length is longer.
     */

    sprintf(valueString, "%d", scalePtr->fromValue);
    XTextExtents(scalePtr->fontPtr, valueString, strlen(valueString),
	    &dummy, &dummy, &dummy, &bbox);
    scalePtr->tickPixels = bbox.rbearing + bbox.lbearing;
    sprintf(valueString, "%d", scalePtr->toValue);
    XTextExtents(scalePtr->fontPtr, valueString, strlen(valueString),
	    &dummy, &dummy, &dummy, &bbox);
    if (scalePtr->tickPixels < bbox.rbearing + bbox.lbearing) {
	scalePtr->tickPixels = bbox.rbearing + bbox.lbearing;
    }

    /*
     * Pad the value with a bit of extra space for prettier printing.
     */

    scalePtr->tickPixels += scalePtr->fontPtr->ascent/2;
    scalePtr->valuePixels = scalePtr->tickPixels;
    if (scalePtr->tickInterval == 0) {
	scalePtr->tickPixels = 0;
    }
    if (!scalePtr->showValue) {
	scalePtr->valuePixels = 0;
    }

    if (scalePtr->labelLength == 0) {
	scalePtr->labelPixels = 0;
    } else {
	XTextExtents(scalePtr->fontPtr, scalePtr->label,
		scalePtr->labelLength, &dummy, &dummy, &dummy, &bbox);
	scalePtr->labelPixels = bbox.rbearing + bbox.lbearing
		+ scalePtr->fontPtr->ascent;
    }
    Tk_GeometryRequest(scalePtr->tkwin, 4*scalePtr->borderWidth
	    + scalePtr->tickPixels + scalePtr->valuePixels + SPACING
	    + scalePtr->width + scalePtr->labelPixels,
	    scalePtr->length);
    Tk_SetInternalBorder(scalePtr->tkwin, scalePtr->borderWidth);
}

/*
 *--------------------------------------------------------------
 *
 * DisplayVerticalScale --
 *
 *	This procedure redraws the contents of a vertical scale
 *	window.  It is invoked as a do-when-idle handler, so it only
 *	runs when there's nothing else for the application to do.
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
DisplayVerticalScale(clientData)
    ClientData clientData;	/* Information about widget. */
{
    register Scale *scalePtr = (Scale *) clientData;
    register Tk_Window tkwin = scalePtr->tkwin;
    int tickRightEdge, valueRightEdge, labelLeftEdge, scaleLeftEdge;
    int totalPixels, x, y, width, height, shadowWidth, tickValue;
    int relief;
    Tk_3DBorder sliderBorder;

    if ((scalePtr->tkwin == NULL) || !Tk_IsMapped(tkwin)) {
	goto done;
    }

    /*
     * Scanning from left to right across the window, the window
     * will contain four columns:  ticks, value, scale, and label.
     * Compute the x-coordinate for each of the columns.
     */

    totalPixels = scalePtr->tickPixels + scalePtr->valuePixels
	    + 2*scalePtr->borderWidth + scalePtr->width
	    + 2*SPACING + scalePtr->labelPixels;
    tickRightEdge = (Tk_Width(tkwin) - totalPixels)/2 + scalePtr->tickPixels;
    valueRightEdge = tickRightEdge + scalePtr->valuePixels;
    scaleLeftEdge = valueRightEdge + SPACING;
    labelLeftEdge = scaleLeftEdge + 2*scalePtr->borderWidth
	    + scalePtr->width + scalePtr->fontPtr->ascent/2;

    /*
     * Display the information from left to right across the window.
     */

    if (scalePtr->flags & REDRAW_OTHER) {
	XClearWindow(Tk_Display(tkwin), Tk_WindowId(tkwin));

	/*
	 * Display the tick marks.
	 */

	if (scalePtr->tickPixels != 0) {
	    for (tickValue = scalePtr->fromValue; ;
		    tickValue += scalePtr->tickInterval) {
		if (scalePtr->toValue > scalePtr->fromValue) {
		    if (tickValue > scalePtr->toValue) {
			break;
		    }
		} else {
		    if (tickValue < scalePtr->toValue) {
			break;
		    }
		}
		DisplayVerticalValue(scalePtr, tickValue, tickRightEdge);
	    }
	}
    }

    /*
     * Display the value, if it is desired.  If not redisplaying the
     * entire window, clear the area of the value to get rid of the
     * old value displayed there.
     */

    if (scalePtr->showValue) {
	if (!(scalePtr->flags & REDRAW_OTHER)) {
	    XClearArea(Tk_Display(tkwin), Tk_WindowId(tkwin),
		    valueRightEdge-scalePtr->valuePixels, scalePtr->offset,
		    scalePtr->valuePixels,
		    Tk_Height(tkwin) - 2*scalePtr->offset, False);
	}
	DisplayVerticalValue(scalePtr, scalePtr->value, valueRightEdge);
    }

    /*
     * Display the scale and the slider.  If not redisplaying the
     * entire window, must clear the trench area to erase the old
     * slider, but don't need to redraw the border.
     */

    if (scalePtr->flags & REDRAW_OTHER) {
	Tk_Draw3DRectangle(Tk_Display(tkwin), Tk_WindowId(tkwin),
		scalePtr->bgBorder, scaleLeftEdge, scalePtr->offset,
		scalePtr->width + 2*scalePtr->borderWidth,
		Tk_Height(tkwin) - 2*scalePtr->offset, scalePtr->borderWidth,
		TK_RELIEF_SUNKEN);
    } else {
	XClearArea(Tk_Display(tkwin), Tk_WindowId(tkwin),
		scaleLeftEdge + scalePtr->borderWidth,
		scalePtr->offset + scalePtr->borderWidth,
		scalePtr->width,
		Tk_Height(tkwin) - 2*scalePtr->offset
		- 2*scalePtr->borderWidth, False);
    }
    if (scalePtr->flags & ACTIVE) {
	sliderBorder = scalePtr->activeBorder;
    } else {
	sliderBorder = scalePtr->sliderBorder;
    }
    width = scalePtr->width;
    height = scalePtr->sliderLength/2;
    x = scaleLeftEdge + scalePtr->borderWidth;
    y = ValueToPixel(scalePtr, scalePtr->value) - height;
    shadowWidth = scalePtr->borderWidth/2;
    if (shadowWidth == 0) {
	shadowWidth = 1;
    }
    relief = (scalePtr->flags & BUTTON_PRESSED) ? TK_RELIEF_SUNKEN
	    : TK_RELIEF_RAISED;
    Tk_Draw3DRectangle(Tk_Display(tkwin), Tk_WindowId(tkwin), sliderBorder,
	    x, y, width, 2*height, shadowWidth, relief);
    x += shadowWidth;
    y += shadowWidth;
    width -= 2*shadowWidth;
    height -= shadowWidth;
    Tk_Fill3DRectangle(Tk_Display(tkwin), Tk_WindowId(tkwin), sliderBorder,
	    x, y, width, height, shadowWidth, relief);
    Tk_Fill3DRectangle(Tk_Display(tkwin), Tk_WindowId(tkwin), sliderBorder,
	    x, y+height, width, height, shadowWidth, relief);

    /*
     * Draw the label to the right of the scale.
     */

    if ((scalePtr->flags & REDRAW_OTHER) && (scalePtr->labelPixels != 0)) {
	XDrawString(Tk_Display(scalePtr->tkwin), Tk_WindowId(scalePtr->tkwin),
	    scalePtr->textGC, labelLeftEdge,
	    scalePtr->offset + (3*scalePtr->fontPtr->ascent)/2,
	    scalePtr->label, scalePtr->labelLength);
    }

    /*
     * Draw the window border.
     */

    if ((scalePtr->flags & REDRAW_OTHER)
	    && (scalePtr->relief != TK_RELIEF_FLAT)) {
	Tk_Draw3DRectangle(Tk_Display(tkwin), Tk_WindowId(tkwin),
		scalePtr->bgBorder, 0, 0, Tk_Width(tkwin), Tk_Height(tkwin),
		scalePtr->borderWidth, scalePtr->relief);
    }

    done:
    scalePtr->flags &= ~REDRAW_ALL;
}

/*
 *----------------------------------------------------------------------
 *
 * DisplayVerticalValue --
 *
 *	This procedure is called to display values (scale readings)
 *	for vertically-oriented scales.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The numerical value corresponding to value is displayed with
 *	its right edge at "rightEdge", and at a vertical position in
 *	the scale that corresponds to "value".
 *
 *----------------------------------------------------------------------
 */

static void
DisplayVerticalValue(scalePtr, value, rightEdge)
    register Scale *scalePtr;	/* Information about widget in which to
				 * display value. */
    int value;			/* Y-coordinate of number to display,
				 * specified in application coords, not
				 * in pixels (we'll compute pixels). */
    int rightEdge;		/* X-coordinate of right edge of text,
				 * specified in pixels. */
{
    register Tk_Window tkwin = scalePtr->tkwin;
    int y, dummy, length;
    char valueString[30];
    XCharStruct bbox;

    y = ValueToPixel(scalePtr, value) + scalePtr->fontPtr->ascent/2;
    sprintf(valueString, "%d", value);
    length = strlen(valueString);
    XTextExtents(scalePtr->fontPtr, valueString, length,
	    &dummy, &dummy, &dummy, &bbox);

    /*
     * Adjust the y-coordinate if necessary to keep the text entirely
     * inside the window.
     */

    if ((y - bbox.ascent) < scalePtr->offset) {
	y = scalePtr->offset + bbox.ascent;
    }
    if ((y + bbox.descent) > (Tk_Height(tkwin) - scalePtr->offset)) {
	y = Tk_Height(tkwin) - scalePtr->offset - bbox.descent;
    }
    XDrawString(Tk_Display(tkwin), Tk_WindowId(tkwin),
	    scalePtr->textGC, rightEdge - bbox.rbearing,
	    y, valueString, length);
}

/*
 *--------------------------------------------------------------
 *
 * DisplayHorizontalScale --
 *
 *	This procedure redraws the contents of a horizontal scale
 *	window.  It is invoked as a do-when-idle handler, so it only
 *	runs when there's nothing else for the application to do.
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
DisplayHorizontalScale(clientData)
    ClientData clientData;	/* Information about widget. */
{
    register Scale *scalePtr = (Scale *) clientData;
    register Tk_Window tkwin = scalePtr->tkwin;
    int tickBottom, valueBottom, labelBottom, scaleBottom;
    int totalPixels, x, y, width, height, shadowWidth, tickValue;
    int relief;
    Tk_3DBorder sliderBorder;

    if ((scalePtr->tkwin == NULL) || !Tk_IsMapped(tkwin)) {
	goto done;
    }

    /*
     * Scanning from bottom to top across the window, the window
     * will contain four rows:  ticks, value, scale, and label.
     * Compute the y-coordinate for each of the rows.
     */

    totalPixels = scalePtr->tickPixels + scalePtr->valuePixels
	    + 2*scalePtr->borderWidth + scalePtr->width
	    + scalePtr->labelPixels;
    tickBottom = (Tk_Height(tkwin) + totalPixels)/2 - 1;
    valueBottom = tickBottom - scalePtr->tickPixels;
    scaleBottom = valueBottom - scalePtr->valuePixels;
    labelBottom = scaleBottom - 2*scalePtr->borderWidth - scalePtr->width;

    /*
     * Display the information from bottom to top across the window.
     */

    if (scalePtr->flags & REDRAW_OTHER) {
	XClearWindow(Tk_Display(tkwin), Tk_WindowId(tkwin));

	/*
	 * Display the tick marks.
	 */

	if (scalePtr->tickPixels != 0) {
	    for (tickValue = scalePtr->fromValue; ;
		    tickValue += scalePtr->tickInterval) {
		if (scalePtr->toValue > scalePtr->fromValue) {
		    if (tickValue > scalePtr->toValue) {
			break;
		    }
		} else {
		    if (tickValue < scalePtr->toValue) {
			break;
		    }
		}
		DisplayHorizontalValue(scalePtr, tickValue, tickBottom);
	    }
	}
    }

    /*
     * Display the value, if it is desired.  If not redisplaying the
     * entire window, clear the area of the value to get rid of the
     * old value displayed there.
     */

    if (scalePtr->showValue) {
	if (!(scalePtr->flags & REDRAW_OTHER)) {
	    XClearArea(Tk_Display(tkwin), Tk_WindowId(tkwin),
		    scalePtr->offset, scaleBottom + 1,
		    Tk_Width(tkwin) - 2*scalePtr->offset,
		    valueBottom - scaleBottom, False);
	}
	DisplayHorizontalValue(scalePtr, scalePtr->value, valueBottom);
    }

    /*
     * Display the scale and the slider.  If not redisplaying the
     * entire window, must clear the trench area to erase the old
     * slider, but don't need to redraw the border.
     */

    y = scaleBottom - 2*scalePtr->borderWidth - scalePtr->width + 1;
    if (scalePtr->flags & REDRAW_OTHER) {
	Tk_Draw3DRectangle(Tk_Display(tkwin), Tk_WindowId(tkwin),
		scalePtr->bgBorder, scalePtr->offset, y,
		Tk_Width(tkwin) - 2*scalePtr->offset,
		scalePtr->width + 2*scalePtr->borderWidth,
		scalePtr->borderWidth, TK_RELIEF_SUNKEN);
    } else {
	XClearArea(Tk_Display(tkwin), Tk_WindowId(tkwin),
		scalePtr->offset + scalePtr->borderWidth,
		y + scalePtr->borderWidth,
		Tk_Width(tkwin) - 2*scalePtr->offset - 2*scalePtr->borderWidth,
		scalePtr->width, False);
    }
    if (scalePtr->flags & ACTIVE) {
	sliderBorder = scalePtr->activeBorder;
    } else {
	sliderBorder = scalePtr->sliderBorder;
    }
    width = scalePtr->sliderLength/2;
    height = scalePtr->width;
    x = ValueToPixel(scalePtr, scalePtr->value) - width;
    y += scalePtr->borderWidth;
    shadowWidth = scalePtr->borderWidth/2;
    if (shadowWidth == 0) {
	shadowWidth = 1;
    }
    relief = (scalePtr->flags & BUTTON_PRESSED) ? TK_RELIEF_SUNKEN
	    : TK_RELIEF_RAISED;
    Tk_Draw3DRectangle(Tk_Display(tkwin), Tk_WindowId(tkwin), sliderBorder,
	    x, y, 2*width, height, shadowWidth, relief);
    x += shadowWidth;
    y += shadowWidth;
    width -= shadowWidth;
    height -= 2*shadowWidth;
    Tk_Fill3DRectangle(Tk_Display(tkwin), Tk_WindowId(tkwin), sliderBorder,
	    x, y, width, height, shadowWidth, relief);
    Tk_Fill3DRectangle(Tk_Display(tkwin), Tk_WindowId(tkwin), sliderBorder,
	    x+width, y, width, height, shadowWidth, relief);

    /*
     * Draw the label to the top of the scale.
     */

    if ((scalePtr->flags & REDRAW_OTHER) && (scalePtr->labelPixels != 0)) {
	XDrawString(Tk_Display(scalePtr->tkwin), Tk_WindowId(scalePtr->tkwin),
	    scalePtr->textGC, scalePtr->offset + scalePtr->fontPtr->ascent/2,
	    labelBottom - scalePtr->fontPtr->descent,
	    scalePtr->label, scalePtr->labelLength);
    }

    /*
     * Draw the window border.
     */

    if ((scalePtr->flags & REDRAW_OTHER)
	    && (scalePtr->relief != TK_RELIEF_FLAT)) {
	Tk_Draw3DRectangle(Tk_Display(tkwin), Tk_WindowId(tkwin),
		scalePtr->bgBorder, 0, 0, Tk_Width(tkwin), Tk_Height(tkwin),
		scalePtr->borderWidth, scalePtr->relief);
    }

    done:
    scalePtr->flags &= ~REDRAW_ALL;
}

/*
 *----------------------------------------------------------------------
 *
 * DisplayHorizontalValue --
 *
 *	This procedure is called to display values (scale readings)
 *	for horizontally-oriented scales.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The numerical value corresponding to value is displayed with
 *	its bottom edge at "bottom", and at a horizontal position in
 *	the scale that corresponds to "value".
 *
 *----------------------------------------------------------------------
 */

static void
DisplayHorizontalValue(scalePtr, value, bottom)
    register Scale *scalePtr;	/* Information about widget in which to
				 * display value. */
    int value;			/* Y-coordinate of number to display,
				 * specified in application coords, not
				 * in pixels (we'll compute pixels). */
    int bottom;			/* Y-coordinate of bottom edge of text,
				 * specified in pixels. */
{
    register Tk_Window tkwin = scalePtr->tkwin;
    int x, y, dummy, length;
    char valueString[30];
    XCharStruct bbox;

    x = ValueToPixel(scalePtr, value);
    y = bottom - scalePtr->fontPtr->descent;
    sprintf(valueString, "%d", value);
    length = strlen(valueString);
    XTextExtents(scalePtr->fontPtr, valueString, length,
	    &dummy, &dummy, &dummy, &bbox);

    /*
     * Adjust the x-coordinate if necessary to keep the text entirely
     * inside the window.
     */

    x -= (bbox.lbearing + bbox.rbearing)/2;
    if ((x - bbox.lbearing) < scalePtr->offset) {
	x = scalePtr->offset + bbox.lbearing;
    }
    if ((y + bbox.rbearing) > (Tk_Width(tkwin) - scalePtr->offset)) {
	x = Tk_Width(tkwin) - scalePtr->offset - bbox.rbearing;
    }
    XDrawString(Tk_Display(tkwin), Tk_WindowId(tkwin),
	    scalePtr->textGC, x, y, valueString, length);
}

/*
 *----------------------------------------------------------------------
 *
 * PixelToValue --
 *
 *	Given a pixel within a scale window, return the scale
 *	reading corresponding to that pixel.
 *
 * Results:
 *	An integer scale reading.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

static int
PixelToValue(scalePtr, x, y)
    register Scale *scalePtr;		/* Information about widget. */
    int x, y;				/* Coordinates of point within
					 * window. */
{
    int value, pixelRange;

    if (scalePtr->vertical) {
	pixelRange = Tk_Height(scalePtr->tkwin) - scalePtr->sliderLength
		- 2*scalePtr->offset - 2*scalePtr->borderWidth;
	value = y;
    } else {
	pixelRange = Tk_Width(scalePtr->tkwin) - scalePtr->sliderLength
		- 2*scalePtr->offset - 2*scalePtr->borderWidth;
	value = x;
    }

    if (pixelRange <= 0) {
	/*
	 * Not enough room for the slider to actually slide:  just return
	 * the scale's current value.
	 */

	return scalePtr->value;
    }
    value -= scalePtr->sliderLength/2 + scalePtr->offset
		+ scalePtr->borderWidth;
    if (value < 0) {
	value = 0;
    }
    if (value > pixelRange) {
	value = pixelRange;
    }
    if (scalePtr->toValue > scalePtr->fromValue) {
	value = scalePtr->fromValue +
		((value * (scalePtr->toValue - scalePtr->fromValue))
		+ pixelRange/2)/pixelRange;
    } else {
	value = scalePtr->toValue +
		(((pixelRange - value)
		* (scalePtr->fromValue - scalePtr->toValue))
		+ pixelRange/2)/pixelRange;
    }
    return value;
}

/*
 *----------------------------------------------------------------------
 *
 * ValueToPixel --
 *
 *	Given a reading of the scale, return the x-coordinate or
 *	y-coordinate corresponding to that reading, depending on
 *	whether the scale is vertical or horizontal, respectively.
 *
 * Results:
 *	An integer value giving the pixel location corresponding
 *	to reading.  The value is restricted to lie within the
 *	defined range for the scale.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

static int
ValueToPixel(scalePtr, value)
    register Scale *scalePtr;		/* Information about widget. */
    int value;				/* Reading of the widget. */
{
    int y, pixelRange, valueRange;

    valueRange = scalePtr->toValue - scalePtr->fromValue;
    pixelRange = (scalePtr->vertical ? Tk_Height(scalePtr->tkwin)
	    : Tk_Width(scalePtr->tkwin)) - scalePtr->sliderLength
	    - 2*scalePtr->offset - 2*scalePtr->borderWidth;
    y = ((value - scalePtr->fromValue) * pixelRange
	    + valueRange/2) / valueRange;
    if (y < 0) {
	y = 0;
    } else if (y > pixelRange) {
	y = pixelRange;
    }
    y += scalePtr->sliderLength/2 + scalePtr->offset + scalePtr->borderWidth;
    return y;
}

/*
 *--------------------------------------------------------------
 *
 * ScaleEventProc --
 *
 *	This procedure is invoked by the Tk dispatcher for various
 *	events on scales.
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
ScaleEventProc(clientData, eventPtr)
    ClientData clientData;	/* Information about window. */
    XEvent *eventPtr;		/* Information about event. */
{
    Scale *scalePtr = (Scale *) clientData;

    if ((eventPtr->type == Expose) && (eventPtr->xexpose.count == 0)) {
	EventuallyRedrawScale(scalePtr, REDRAW_ALL);
    } else if (eventPtr->type == DestroyNotify) {
	Tcl_DeleteCommand(scalePtr->interp, Tk_PathName(scalePtr->tkwin));
	scalePtr->tkwin = NULL;
	if (scalePtr->flags & REDRAW_ALL) {
	    if (scalePtr->vertical) {
		Tk_CancelIdleCall(DisplayVerticalScale, (ClientData) scalePtr);
	    } else {
		Tk_CancelIdleCall(DisplayHorizontalScale,
			(ClientData) scalePtr);
	    }
	}
	Tk_EventuallyFree((ClientData) scalePtr, DestroyScale);
    } else if (eventPtr->type == ConfigureNotify) {
	ComputeScaleGeometry(scalePtr);
    }
}

/*
 *--------------------------------------------------------------
 *
 * ScaleMouseProc --
 *
 *	This procedure is called back by Tk in response to
 *	mouse events such as window entry, window exit, mouse
 *	motion, and button presses.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	This procedure implements the "feel" of the scale by
 *	issuing commands in response to button presses and mouse
 *	motion.
 *
 *--------------------------------------------------------------
 */

static void
ScaleMouseProc(clientData, eventPtr)
    ClientData clientData;		/* Information about window. */
    register XEvent *eventPtr;		/* Information about event. */
{
    register Scale *scalePtr = (Scale *) clientData;

    if (scalePtr->state != tkNormalUid) {
	return;
    }

    Tk_Preserve((ClientData) scalePtr);
    if (eventPtr->type == EnterNotify) {
	scalePtr->flags |= ACTIVE;
	EventuallyRedrawScale(scalePtr, REDRAW_SLIDER);
    } else if (eventPtr->type == LeaveNotify) {
	scalePtr->flags &= ~ACTIVE;
	EventuallyRedrawScale(scalePtr, REDRAW_SLIDER);
    } else if ((eventPtr->type == MotionNotify)
	    && (scalePtr->flags & BUTTON_PRESSED)) {
	SetScaleValue(scalePtr,  PixelToValue(scalePtr,
		eventPtr->xmotion.x, eventPtr->xmotion.y));
    } else if ((eventPtr->type == ButtonPress)
/*	    && (eventPtr->xbutton.button == Button1) */
	    && (eventPtr->xbutton.state == 0)) {
	scalePtr->flags |= BUTTON_PRESSED;
	SetScaleValue(scalePtr, PixelToValue(scalePtr,
		eventPtr->xbutton.x, eventPtr->xbutton.y));
	EventuallyRedrawScale(scalePtr, REDRAW_SLIDER);
    } else if ((eventPtr->type == ButtonRelease)
/*	    && (eventPtr->xbutton.button == Button1) */
	    && (scalePtr->flags & BUTTON_PRESSED)) {
	scalePtr->flags &= ~BUTTON_PRESSED;
	EventuallyRedrawScale(scalePtr, REDRAW_SLIDER);
    }
    Tk_Release((ClientData) scalePtr);
}

/*
 *--------------------------------------------------------------
 *
 * SetScaleValue --
 *
 *	This procedure changes the value of a scale and invokes
 *	a Tcl command to reflect the current position of a scale
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	A Tcl command is invoked, and an additional error-processing
 *	command may also be invoked.  The scale's slider is redrawn.
 *
 *--------------------------------------------------------------
 */

static void
SetScaleValue(scalePtr, value)
    register Scale *scalePtr;	/* Info about widget. */
    int value;			/* New value for scale.  Gets
				 * adjusted if it's off the scale. */
{
    int result;
    char string[20];

    if ((value < scalePtr->fromValue)
	    ^ (scalePtr->toValue < scalePtr->fromValue)) {
	value = scalePtr->fromValue;
    }
    if ((value > scalePtr->toValue)
	    ^ (scalePtr->toValue < scalePtr->fromValue)) {
	value = scalePtr->toValue;
    }
    if (value == scalePtr->value) {
	return;
    }
    scalePtr->value = value;
    EventuallyRedrawScale(scalePtr, REDRAW_SLIDER);

    sprintf(string, " %d", scalePtr->value);
    result = Tcl_VarEval(scalePtr->interp, scalePtr->command, string,
	    (char *) NULL);
    if (result != TCL_OK) {
	TkBindError(scalePtr->interp);
    }
}

/*
 *--------------------------------------------------------------
 *
 * EventuallyRedrawScale --
 *
 *	Arrange for part or all of a scale widget to redrawn at
 *	the next convenient time in the future.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	If "what" is REDRAW_SLIDER then just the slider and the
 *	value readout will be redrawn;  if "what" is REDRAW_ALL
 *	then the entire widget will be redrawn.
 *
 *--------------------------------------------------------------
 */

static void
EventuallyRedrawScale(scalePtr, what)
    register Scale *scalePtr;	/* Information about widget. */
    int what;			/* What to redraw:  REDRAW_SLIDER
				 * or REDRAW_ALL. */
{
    if ((what == 0) || (scalePtr->tkwin == NULL)
	    || !Tk_IsMapped(scalePtr->tkwin)) {
	return;
    }
    if ((scalePtr->flags & REDRAW_ALL) == 0) {
	if (scalePtr->vertical) {
	    Tk_DoWhenIdle(DisplayVerticalScale, (ClientData) scalePtr);
	} else {
	    Tk_DoWhenIdle(DisplayHorizontalScale, (ClientData) scalePtr);
	}
    }
    scalePtr->flags |= what;
}
