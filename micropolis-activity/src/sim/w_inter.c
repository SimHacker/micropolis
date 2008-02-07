/* 
 * tkInterval.c --
 *
 *	This module implements a interval widgets for the Tk toolkit.
 *	A interval displays a slider that can be adjusted to change a
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

/* Improvements in the version used for Micropolis are copyrighted and
 * licensed under these copyright terms.
 *
 * Micropolis, Unix Version.  This game was released for the Unix platform
 * in or about 1990 and has been modified for inclusion in the One Laptop
 * Per Child program.  Copyright (C) 1989 - 2007 Electronic Arts Inc.  If
 * you need assistance with this program, you may contact:
 *   http://wiki.laptop.org/go/Micropolis  or email  micropolis@laptop.org.
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or (at
 * your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.  You should have received a
 * copy of the GNU General Public License along with this program.  If
 * not, see <http://www.gnu.org/licenses/>.
 * 
 *             ADDITIONAL TERMS per GNU GPL Section 7
 * 
 * No trademark or publicity rights are granted.  This license does NOT
 * give you any right, title or interest in the trademark SimCity or any
 * other Electronic Arts trademark.  You may not distribute any
 * modification of this program using the trademark SimCity or claim any
 * affliation or association with Electronic Arts Inc. or its employees.
 * 
 * Any propagation or conveyance of this program must include this
 * copyright notice and these terms.
 * 
 * If you convey this program (or any modifications of it) and assume
 * contractual liability for the program to recipients of it, you agree
 * to indemnify Electronic Arts for any liability that those contractual
 * assumptions impose on Electronic Arts.
 * 
 * You may not misrepresent the origins of this program; modified
 * versions of the program must be marked as such and not identified as
 * the original program.
 * 
 * This disclaimer supplements the one included in the General Public
 * License.  TO THE FULLEST EXTENT PERMISSIBLE UNDER APPLICABLE LAW, THIS
 * PROGRAM IS PROVIDED TO YOU "AS IS," WITH ALL FAULTS, WITHOUT WARRANTY
 * OF ANY KIND, AND YOUR USE IS AT YOUR SOLE RISK.  THE ENTIRE RISK OF
 * SATISFACTORY QUALITY AND PERFORMANCE RESIDES WITH YOU.  ELECTRONIC ARTS
 * DISCLAIMS ANY AND ALL EXPRESS, IMPLIED OR STATUTORY WARRANTIES,
 * INCLUDING IMPLIED WARRANTIES OF MERCHANTABILITY, SATISFACTORY QUALITY,
 * FITNESS FOR A PARTICULAR PURPOSE, NONINFRINGEMENT OF THIRD PARTY
 * RIGHTS, AND WARRANTIES (IF ANY) ARISING FROM A COURSE OF DEALING,
 * USAGE, OR TRADE PRACTICE.  ELECTRONIC ARTS DOES NOT WARRANT AGAINST
 * INTERFERENCE WITH YOUR ENJOYMENT OF THE PROGRAM; THAT THE PROGRAM WILL
 * MEET YOUR REQUIREMENTS; THAT OPERATION OF THE PROGRAM WILL BE
 * UNINTERRUPTED OR ERROR-FREE, OR THAT THE PROGRAM WILL BE COMPATIBLE
 * WITH THIRD PARTY SOFTWARE OR THAT ANY ERRORS IN THE PROGRAM WILL BE
 * CORRECTED.  NO ORAL OR WRITTEN ADVICE PROVIDED BY ELECTRONIC ARTS OR
 * ANY AUTHORIZED REPRESENTATIVE SHALL CREATE A WARRANTY.  SOME
 * JURISDICTIONS DO NOT ALLOW THE EXCLUSION OF OR LIMITATIONS ON IMPLIED
 * WARRANTIES OR THE LIMITATIONS ON THE APPLICABLE STATUTORY RIGHTS OF A
 * CONSUMER, SO SOME OR ALL OF THE ABOVE EXCLUSIONS AND LIMITATIONS MAY
 * NOT APPLY TO YOU.
 */

#include "tkconfig.h"
#include "default.h"
#include "tkint.h"

/*
 * A data structure of the following type is kept for each interval
 * widget managed by this file:
 */

typedef struct {
    Tk_Window tkwin;		/* Window that embodies the interval.  NULL
				 * means that the window has been destroyed
				 * but the data structures haven't yet been
				 * cleaned up.*/
    Tcl_Interp *interp;		/* Interpreter associated with interval. */
    Tk_Uid orientUid;		/* Orientation for window ("vertical" or
				 * "horizontal"). */
    int vertical;		/* Non-zero means vertical orientation,
				 * zero means horizontal. */
    int minValue;		/* Value corresponding to minimum of interval. */
    int maxValue;		/* Value corresponding to maximum of interval. */
    int fromValue;		/* Value corresponding to left or top of
				 * interval. */
    int toValue;		/* Value corresponding to right or bottom
				 * of interval. */
    int tickInterval;		/* Distance between tick marks;  0 means
				 * don't display any tick marks. */
    int trackValue;		/* Value of mouse at start of tracking. */
    int trackWidth;		/* Value of max-min at start of tracking. */
    int trackState;		/* Tracking state. */
    char *command;		/* Command prefix to use when invoking Tcl
				 * commands because the interval value changed.
				 * NULL means don't invoke commands.
				 * Malloc'ed. */
    int commandLength;		/* Number of non-NULL bytes in command. */
    char *label;		/* Label to display above or to right of
				 * interval;  NULL means don't display a
				 * label.  Malloc'ed. */
    int labelLength;		/* Number of non-NULL chars. in label. */
    Tk_Uid state;		/* Normal or disabled.  Value cannot be
				 * changed when interval is disabled. */

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
    int width;			/* Desired narrow dimension of interval,
				 * in pixels. */
    int length;			/* Desired long dimension of interval,
				 * in pixels. */
    int relief;			/* Indicates whether window as a whole is
				 * raised, sunken, or flat. */
    int offset;			/* Zero if relief is TK_RELIEF_FLAT,
				 * borderWidth otherwise.   Indicates how
				 * much interior stuff must be offset from
				 * outside edges to leave room for border. */
    int showValue;		/* Non-zero means to display the interval value
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
} Interval;

/*
 * Flag bits for intervals:
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
 * Space to leave between interval area and text.
 */

#define SPACING 2

/*
 * Information used for argv parsing.
 */


static Tk_ConfigSpec configSpecs[] = {
    {TK_CONFIG_BORDER, "-activeforeground", "activeForeground", "Background",
	DEF_SCALE_ACTIVE_FG_COLOR, Tk_Offset(Interval, activeBorder),
	TK_CONFIG_COLOR_ONLY},
    {TK_CONFIG_BORDER, "-activeforeground", "activeForeground", "Background",
	DEF_SCALE_ACTIVE_FG_MONO, Tk_Offset(Interval, activeBorder),
	TK_CONFIG_MONO_ONLY},
    {TK_CONFIG_BORDER, "-background", "background", "Background",
	DEF_SCALE_BG_COLOR, Tk_Offset(Interval, bgBorder),
	TK_CONFIG_COLOR_ONLY},
    {TK_CONFIG_BORDER, "-background", "background", "Background",
	DEF_SCALE_BG_MONO, Tk_Offset(Interval, bgBorder),
	TK_CONFIG_MONO_ONLY},
    {TK_CONFIG_SYNONYM, "-bd", "borderWidth", (char *) NULL,
	(char *) NULL, 0, 0},
    {TK_CONFIG_SYNONYM, "-bg", "background", (char *) NULL,
	(char *) NULL, 0, 0},
    {TK_CONFIG_PIXELS, "-borderwidth", "borderWidth", "BorderWidth",
	DEF_SCALE_BORDER_WIDTH, Tk_Offset(Interval, borderWidth), 0},
    {TK_CONFIG_STRING, "-command", "command", "Command",
	(char *) NULL, Tk_Offset(Interval, command), 0},
    {TK_CONFIG_ACTIVE_CURSOR, "-cursor", "cursor", "Cursor",
	DEF_SCALE_CURSOR, Tk_Offset(Interval, cursor), TK_CONFIG_NULL_OK},
    {TK_CONFIG_SYNONYM, "-fg", "foreground", (char *) NULL,
	(char *) NULL, 0, 0},
    {TK_CONFIG_FONT, "-font", "font", "Font",
	DEF_SCALE_FONT, Tk_Offset(Interval, fontPtr),
	0},
    {TK_CONFIG_COLOR, "-foreground", "foreground", "Foreground",
	DEF_SCALE_FG_COLOR, Tk_Offset(Interval, textColorPtr),
	TK_CONFIG_COLOR_ONLY},
    {TK_CONFIG_COLOR, "-foreground", "foreground", "Foreground",
	DEF_SCALE_FG_MONO, Tk_Offset(Interval, textColorPtr),
	TK_CONFIG_MONO_ONLY},
    {TK_CONFIG_INT, "-from", "from", "From",
	DEF_SCALE_FROM, Tk_Offset(Interval, fromValue), 0},
    {TK_CONFIG_STRING, "-label", "label", "Label",
	DEF_SCALE_LABEL, Tk_Offset(Interval, label), 0},
    {TK_CONFIG_PIXELS, "-length", "length", "Length",
	DEF_SCALE_LENGTH, Tk_Offset(Interval, length), 0},
    {TK_CONFIG_UID, "-orient", "orient", "Orient",
	DEF_SCALE_ORIENT, Tk_Offset(Interval, orientUid), 0},
    {TK_CONFIG_RELIEF, "-relief", "relief", "Relief",
	DEF_SCALE_RELIEF, Tk_Offset(Interval, relief), 0},
    {TK_CONFIG_BOOLEAN, "-showvalue", "showValue", "ShowValue",
	DEF_SCALE_SHOW_VALUE, Tk_Offset(Interval, showValue), 0},
    {TK_CONFIG_BORDER, "-sliderforeground", "sliderForeground", "Background",
	DEF_SCALE_SLIDER_FG_COLOR, Tk_Offset(Interval, sliderBorder),
	TK_CONFIG_COLOR_ONLY},
    {TK_CONFIG_BORDER, "-sliderforeground", "sliderForeground", "Background",
	DEF_SCALE_SLIDER_FG_MONO, Tk_Offset(Interval, sliderBorder), 
	TK_CONFIG_MONO_ONLY},
    {TK_CONFIG_PIXELS, "-min", "min", "Min",
	"0", Tk_Offset(Interval, minValue), 0},
    {TK_CONFIG_PIXELS, "-max", "max", "Max",
	"9999", Tk_Offset(Interval, maxValue), 0},
    {TK_CONFIG_UID, "-state", "state", "State",
	DEF_SCALE_STATE, Tk_Offset(Interval, state), 0},
    {TK_CONFIG_INT, "-tickinterval", "tickInterval", "TickInterval",
	DEF_SCALE_TICK_INTERVAL, Tk_Offset(Interval, tickInterval), 0},
    {TK_CONFIG_INT, "-to", "to", "To",
	DEF_SCALE_TO, Tk_Offset(Interval, toValue), 0},
    {TK_CONFIG_PIXELS, "-width", "width", "Width",
	DEF_SCALE_WIDTH, Tk_Offset(Interval, width), 0},
    {TK_CONFIG_END, (char *) NULL, (char *) NULL, (char *) NULL,
	(char *) NULL, 0, 0}
};

/*
 * Forward declarations for procedures defined later in this file:
 */

static void		ComputeIntervalGeometry _ANSI_ARGS_((Interval *intervalPtr));
static int		ConfigureInterval _ANSI_ARGS_((Tcl_Interp *interp,
			    Interval *intervalPtr, int argc, char **argv,
			    int flags));
static void		DestroyInterval _ANSI_ARGS_((ClientData clientData));
static void		DisplayHorizontalInterval _ANSI_ARGS_((
			    ClientData clientData));
static void		DisplayHorizontalValue _ANSI_ARGS_((Interval *intervalPtr,
			    int value, int bottom));
static void		DisplayVerticalInterval _ANSI_ARGS_((
			    ClientData clientData));
static void		DisplayVerticalValue _ANSI_ARGS_((Interval *intervalPtr,
			    int value, int rightEdge));
static void		EventuallyRedrawInterval _ANSI_ARGS_((Interval *intervalPtr,
			    int what));
static int		PixelToValue _ANSI_ARGS_((Interval *intervalPtr, int x,
			    int y));
static void		IntervalEventProc _ANSI_ARGS_((ClientData clientData,
			    XEvent *eventPtr));
static void		IntervalMouseProc _ANSI_ARGS_((ClientData clientData,
			    XEvent *eventPtr));
static int		IntervalWidgetCmd _ANSI_ARGS_((ClientData clientData,
			    Tcl_Interp *interp, int argc, char **argv));
static void		SetInterval _ANSI_ARGS_((Interval *intervalPtr,
			    int minValue, int maxValue, int notify));
static void		TrackInterval _ANSI_ARGS_((Interval *intervalPtr,
			    int value));
static void		StartTrackInterval _ANSI_ARGS_((Interval *intervalPtr,
			    int value));
static int		ValueToPixel _ANSI_ARGS_((Interval *intervalPtr, int value));

/*
 *--------------------------------------------------------------
 *
 * Tk_IntervalCmd --
 *
 *	This procedure is invoked to process the "interval" Tcl
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
Tk_IntervalCmd(clientData, interp, argc, argv)
    ClientData clientData;		/* Main window associated with
				 * interpreter. */
    Tcl_Interp *interp;		/* Current interpreter. */
    int argc;			/* Number of arguments. */
    char **argv;		/* Argument strings. */
{
    Tk_Window tkwin = (Tk_Window) clientData;
    register Interval *intervalPtr;
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
     * Initialize fields that won't be initialized by ConfigureInterval,
     * or which ConfigureInterval expects to have reasonable values
     * (e.g. resource pointers).
     */

    intervalPtr = (Interval *) ckalloc(sizeof(Interval));
    intervalPtr->tkwin = new;
    intervalPtr->interp = interp;
    intervalPtr->minValue = 0;
    intervalPtr->maxValue = 0;
    intervalPtr->command = NULL;
    intervalPtr->label = NULL;
    intervalPtr->state = tkNormalUid;
    intervalPtr->bgBorder = NULL;
    intervalPtr->sliderBorder = NULL;
    intervalPtr->activeBorder = NULL;
    intervalPtr->fontPtr = NULL;
    intervalPtr->textColorPtr = NULL;
    intervalPtr->textGC = None;
    intervalPtr->cursor = None;
    intervalPtr->flags = 0;

    Tk_SetClass(intervalPtr->tkwin, "Interval");
    Tk_CreateEventHandler(intervalPtr->tkwin, ExposureMask|StructureNotifyMask,
	    IntervalEventProc, (ClientData) intervalPtr);
    Tk_CreateEventHandler(intervalPtr->tkwin, EnterWindowMask|LeaveWindowMask
	    |PointerMotionMask|ButtonPressMask|ButtonReleaseMask,
	    IntervalMouseProc, (ClientData) intervalPtr);
    Tcl_CreateCommand(interp, Tk_PathName(intervalPtr->tkwin), IntervalWidgetCmd,
	    (ClientData) intervalPtr, (void (*)()) NULL);
    if (ConfigureInterval(interp, intervalPtr, argc-2, argv+2, 0) != TCL_OK) {
	goto error;
    }

    interp->result = Tk_PathName(intervalPtr->tkwin);
    return TCL_OK;

    error:
    Tk_DestroyWindow(intervalPtr->tkwin);
    return TCL_ERROR;
}

/*
 *--------------------------------------------------------------
 *
 * IntervalWidgetCmd --
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
IntervalWidgetCmd(clientData, interp, argc, argv)
    ClientData clientData;		/* Information about interval
					 * widget. */
    Tcl_Interp *interp;			/* Current interpreter. */
    int argc;				/* Number of arguments. */
    char **argv;			/* Argument strings. */
{
    register Interval *intervalPtr = (Interval *) clientData;
    int result = TCL_OK;
    int length;
    char c;

    if (argc < 2) {
	Tcl_AppendResult(interp, "wrong # args: should be \"",
		argv[0], " option ?arg arg ...?\"", (char *) NULL);
	return TCL_ERROR;
    }
    Tk_Preserve((ClientData) intervalPtr);
    c = argv[1][0];
    length = strlen(argv[1]);
    if ((c == 'c') && (strncmp(argv[1], "configure", length) == 0)) {
	if (argc == 2) {
	    result = Tk_ConfigureInfo(interp, intervalPtr->tkwin, configSpecs,
		    (char *) intervalPtr, (char *) NULL, 0);
	} else if (argc == 3) {
	    result = Tk_ConfigureInfo(interp, intervalPtr->tkwin, configSpecs,
		    (char *) intervalPtr, argv[2], 0);
	} else {
	    result = ConfigureInterval(interp, intervalPtr, argc-2, argv+2,
		    TK_CONFIG_ARGV_ONLY);
	}
    } else if ((c == 'g') && (strncmp(argv[1], "get", length) == 0)) {
	if (argc != 2) {
	    Tcl_AppendResult(interp, "wrong # args: should be \"",
		    argv[0], " get\"", (char *) NULL);
	    goto error;
	}
	sprintf(interp->result, "%d %d", intervalPtr->minValue, intervalPtr->maxValue);
    } else if ((c == 's') && (strncmp(argv[1], "set", length) == 0)) {
	int minValue, maxValue;

	if (argc != 4) {
	    Tcl_AppendResult(interp, "wrong # args: should be \"",
		    argv[0], " set minValue maxValue\"", (char *) NULL);
	    goto error;
	}
	if (Tcl_GetInt(interp, argv[2], &minValue) != TCL_OK) {
	    goto error;
	}
	if (Tcl_GetInt(interp, argv[3], &maxValue) != TCL_OK) {
	    goto error;
	}
	if (minValue > maxValue) {
	  int temp = minValue;
	  minValue = maxValue; maxValue = temp;
	}
	if (intervalPtr->state == tkNormalUid) {
	    if ((minValue < intervalPtr->fromValue)
		    ^ (intervalPtr->toValue < intervalPtr->fromValue)) {
		minValue = intervalPtr->fromValue;
	    }
	    if ((minValue > intervalPtr->toValue)
		    ^ (intervalPtr->toValue < intervalPtr->fromValue)) {
		minValue = intervalPtr->toValue;
	    }
	    if ((maxValue < intervalPtr->fromValue)
		    ^ (intervalPtr->toValue < intervalPtr->fromValue)) {
		maxValue = intervalPtr->fromValue;
	    }
	    if ((maxValue > intervalPtr->toValue)
		    ^ (intervalPtr->toValue < intervalPtr->fromValue)) {
		maxValue = intervalPtr->toValue;
	    }
	    SetInterval(intervalPtr, minValue, maxValue, 1);
	}
    } else if ((c == 'r') && (strncmp(argv[1], "reset", length) == 0)) {
	if (argc != 2) {
	    Tcl_AppendResult(interp, "wrong # args: should be \"",
		    argv[0], " reset\"", (char *) NULL);
	    goto error;
	}
	SetInterval(intervalPtr,
		    intervalPtr->fromValue, intervalPtr->toValue, 0);
    } else {
	Tcl_AppendResult(interp, "bad option \"", argv[1],
		"\":  must be configure, get, or set", (char *) NULL);
	goto error;
    }
    Tk_Release((ClientData) intervalPtr);
    return result;

    error:
    Tk_Release((ClientData) intervalPtr);
    return TCL_ERROR;
}

/*
 *----------------------------------------------------------------------
 *
 * DestroyInterval --
 *
 *	This procedure is invoked by Tk_EventuallyFree or Tk_Release
 *	to clean up the internal structure of a button at a safe time
 *	(when no-one is using it anymore).
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Everything associated with the interval is freed up.
 *
 *----------------------------------------------------------------------
 */

static void
DestroyInterval(clientData)
    ClientData clientData;	/* Info about interval widget. */
{
    register Interval *intervalPtr = (Interval *) clientData;

    if (intervalPtr->command != NULL) {
	ckfree(intervalPtr->command);
    }
    if (intervalPtr->label != NULL) {
	ckfree(intervalPtr->label);
    }
    if (intervalPtr->bgBorder != NULL) {
	Tk_Free3DBorder(intervalPtr->bgBorder);
    }
    if (intervalPtr->sliderBorder != NULL) {
	Tk_Free3DBorder(intervalPtr->sliderBorder);
    }
    if (intervalPtr->activeBorder != NULL) {
	Tk_Free3DBorder(intervalPtr->activeBorder);
    }
    if (intervalPtr->fontPtr != NULL) {
	Tk_FreeFontStruct(intervalPtr->fontPtr);
    }
    if (intervalPtr->textColorPtr != NULL) {
	Tk_FreeColor(intervalPtr->textColorPtr);
    }
    if (intervalPtr->textGC != None) {
	Tk_FreeGC(intervalPtr->textGC);
    }
    if (intervalPtr->cursor != None) {
	Tk_FreeCursor(intervalPtr->cursor);
    }
    ckfree((char *) intervalPtr);
}

/*
 *----------------------------------------------------------------------
 *
 * ConfigureInterval --
 *
 *	This procedure is called to process an argv/argc list, plus
 *	the Tk option database, in order to configure (or
 *	reconfigure) a interval widget.
 *
 * Results:
 *	The return value is a standard Tcl result.  If TCL_ERROR is
 *	returned, then interp->result contains an error message.
 *
 * Side effects:
 *	Configuration information, such as colors, border width,
 *	etc. get set for intervalPtr;  old resources get freed,
 *	if there were any.
 *
 *----------------------------------------------------------------------
 */

static int
ConfigureInterval(interp, intervalPtr, argc, argv, flags)
    Tcl_Interp *interp;		/* Used for error reporting. */
    register Interval *intervalPtr;	/* Information about widget;  may or may
				 * not already have values for some fields. */
    int argc;			/* Number of valid entries in argv. */
    char **argv;		/* Arguments. */
    int flags;			/* Flags to pass to Tk_ConfigureWidget. */
{
    XGCValues gcValues;
    GC newGC;
    int length;

    if (Tk_ConfigureWidget(interp, intervalPtr->tkwin, configSpecs,
	    argc, argv, (char *) intervalPtr, flags) != TCL_OK) {
	return TCL_ERROR;
    }

    /*
     * A few options need special processing, such as parsing the
     * orientation or setting the background from a 3-D border.
     */

    length = strlen(intervalPtr->orientUid);
    if (strncmp(intervalPtr->orientUid, "vertical", length) == 0) {
	intervalPtr->vertical = 1;
    } else if (strncmp(intervalPtr->orientUid, "horizontal", length) == 0) {
	intervalPtr->vertical = 0;
    } else {
	Tcl_AppendResult(interp, "bad orientation \"", intervalPtr->orientUid,
		"\": must be vertical or horizontal", (char *) NULL);
	return TCL_ERROR;
    }

    if ((intervalPtr->state != tkNormalUid)
	    && (intervalPtr->state != tkDisabledUid)) {
	Tcl_AppendResult(interp, "bad state value \"", intervalPtr->state,
		"\":  must be normal or disabled", (char *) NULL);
	intervalPtr->state = tkNormalUid;
	return TCL_ERROR;
    }

    /*
     * Make sure that the tick interval has the right sign so that
     * addition moves from fromValue to toValue.
     */

    if ((intervalPtr->tickInterval < 0)
	    ^ ((intervalPtr->toValue - intervalPtr->fromValue) <  0)) {
	intervalPtr->tickInterval = -intervalPtr->tickInterval;
    }

    /*
     * Set the interval mix and max values to themselves;  all this does is
     * to make sure that the interval's value is within the new acceptable 
     * range for the interval.
     */

    SetInterval(intervalPtr, intervalPtr->minValue, intervalPtr->maxValue, 0);

    if (intervalPtr->command != NULL) {
	intervalPtr->commandLength = strlen(intervalPtr->command);
    } else {
	intervalPtr->commandLength = 0;
    }

    if (intervalPtr->label != NULL) {
	intervalPtr->labelLength = strlen(intervalPtr->label);
    } else {
	intervalPtr->labelLength = 0;
    }

    Tk_SetBackgroundFromBorder(intervalPtr->tkwin, intervalPtr->bgBorder);

    gcValues.font = intervalPtr->fontPtr->fid;
    gcValues.foreground = intervalPtr->textColorPtr->pixel;
    newGC = Tk_GetGC(intervalPtr->tkwin, GCForeground|GCFont, &gcValues);
    if (intervalPtr->textGC != None) {
	Tk_FreeGC(intervalPtr->textGC);
    }
    intervalPtr->textGC = newGC;

    if (intervalPtr->relief != TK_RELIEF_FLAT) {
	intervalPtr->offset = intervalPtr->borderWidth;
    } else {
	intervalPtr->offset = 0;
    }

    /*
     * Recompute display-related information, and let the geometry
     * manager know how much space is needed now.
     */

    ComputeIntervalGeometry(intervalPtr);

    EventuallyRedrawInterval(intervalPtr, REDRAW_ALL);
    return TCL_OK;
}

/*
 *----------------------------------------------------------------------
 *
 * ComputeIntervalGeometry --
 *
 *	This procedure is called to compute various geometrical
 *	information for a interval, such as where various things get
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
ComputeIntervalGeometry(intervalPtr)
    register Interval *intervalPtr;		/* Information about widget. */
{
    XCharStruct bbox;
    char valueString[30];
    int dummy, lineHeight;

    /*
     * Horizontal intervals are simpler than vertical ones because
     * all sizes are the same (the height of a line of text);
     * handle them first and then quit.
     */

    if (!intervalPtr->vertical) {
	lineHeight = intervalPtr->fontPtr->ascent + intervalPtr->fontPtr->descent;
	if (intervalPtr->tickInterval != 0) {
	    intervalPtr->tickPixels = lineHeight;
	} else {
	    intervalPtr->tickPixels = 0;
	}
	if (intervalPtr->showValue) {
	    intervalPtr->valuePixels = lineHeight + SPACING;
	} else {
	    intervalPtr->valuePixels = 0;
	}
	if (intervalPtr->labelLength != 0) {
	    intervalPtr->labelPixels = lineHeight;
	} else {
	    intervalPtr->labelPixels = 0;
	}

	Tk_GeometryRequest(intervalPtr->tkwin,
		intervalPtr->length + 2*intervalPtr->offset,
		intervalPtr->tickPixels + intervalPtr->valuePixels
		+ intervalPtr->width + 2*intervalPtr->borderWidth
		+ intervalPtr->labelPixels + 2*intervalPtr->offset);
	Tk_SetInternalBorder(intervalPtr->tkwin, intervalPtr->borderWidth);
	return;
    }

    /*
     * Vertical interval:  compute the amount of space needed for tick marks
     * and current value by formatting strings for the two end points;
     * use whichever length is longer.
     */

    sprintf(valueString, "%d", intervalPtr->fromValue);
    XTextExtents(intervalPtr->fontPtr, valueString, strlen(valueString),
	    &dummy, &dummy, &dummy, &bbox);
    intervalPtr->tickPixels = bbox.rbearing + bbox.lbearing;
    sprintf(valueString, "%d", intervalPtr->toValue);
    XTextExtents(intervalPtr->fontPtr, valueString, strlen(valueString),
	    &dummy, &dummy, &dummy, &bbox);
    if (intervalPtr->tickPixels < bbox.rbearing + bbox.lbearing) {
	intervalPtr->tickPixels = bbox.rbearing + bbox.lbearing;
    }

    /*
     * Pad the value with a bit of extra space for prettier printing.
     */

    intervalPtr->tickPixels += intervalPtr->fontPtr->ascent/2;
    intervalPtr->valuePixels = intervalPtr->tickPixels;
    if (intervalPtr->tickInterval == 0) {
	intervalPtr->tickPixels = 0;
    }
    if (!intervalPtr->showValue) {
	intervalPtr->valuePixels = 0;
    }

    if (intervalPtr->labelLength == 0) {
	intervalPtr->labelPixels = 0;
    } else {
	XTextExtents(intervalPtr->fontPtr, intervalPtr->label,
		intervalPtr->labelLength, &dummy, &dummy, &dummy, &bbox);
	intervalPtr->labelPixels = bbox.rbearing + bbox.lbearing
		+ intervalPtr->fontPtr->ascent;
    }
    Tk_GeometryRequest(intervalPtr->tkwin, 2*intervalPtr->borderWidth
	    + intervalPtr->tickPixels + intervalPtr->valuePixels + SPACING
	    + intervalPtr->width + intervalPtr->labelPixels,
	    intervalPtr->length);
    Tk_SetInternalBorder(intervalPtr->tkwin, intervalPtr->borderWidth);
}

/*
 *--------------------------------------------------------------
 *
 * DisplayVerticalInterval --
 *
 *	This procedure redraws the contents of a vertical interval
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
DisplayVerticalInterval(clientData)
    ClientData clientData;	/* Information about widget. */
{
    register Interval *intervalPtr = (Interval *) clientData;
    register Tk_Window tkwin = intervalPtr->tkwin;
    int tickRightEdge, valueRightEdge, labelLeftEdge, intervalLeftEdge;
    int totalPixels, x, y, width, height, tickValue, min, max;
    int relief;
    Tk_3DBorder sliderBorder;

    if ((intervalPtr->tkwin == NULL) || !Tk_IsMapped(tkwin)) {
	goto done;
    }

    /*
     * Scanning from left to right across the window, the window
     * will contain four columns:  ticks, value, interval, and label.
     * Compute the x-coordinate for each of the columns.
     */

    totalPixels = intervalPtr->tickPixels + intervalPtr->valuePixels
	    + 2*intervalPtr->borderWidth + intervalPtr->width
	    + 2*SPACING + intervalPtr->labelPixels;
    tickRightEdge = (Tk_Width(tkwin) - totalPixels)/2 + intervalPtr->tickPixels;
    valueRightEdge = tickRightEdge + intervalPtr->valuePixels;
    intervalLeftEdge = valueRightEdge + SPACING;
    labelLeftEdge = intervalLeftEdge + 2*intervalPtr->borderWidth
	    + intervalPtr->width + intervalPtr->fontPtr->ascent/2;

    /*
     * Display the information from left to right across the window.
     */

    if (intervalPtr->flags & REDRAW_OTHER) {
	XClearWindow(Tk_Display(tkwin), Tk_WindowId(tkwin));

	/*
	 * Display the tick marks.
	 */

	if (intervalPtr->tickPixels != 0) {
	    for (tickValue = intervalPtr->fromValue; ;
		    tickValue += intervalPtr->tickInterval) {
		if (intervalPtr->toValue > intervalPtr->fromValue) {
		    if (tickValue > intervalPtr->toValue) {
			break;
		    }
		} else {
		    if (tickValue < intervalPtr->toValue) {
			break;
		    }
		}
		DisplayVerticalValue(intervalPtr, tickValue, tickRightEdge);
	    }
	}
    }

    /*
     * Display the values, if they are desired.  If not redisplaying the
     * entire window, clear the area of the value to get rid of the
     * old value displayed there.
     */

    if (intervalPtr->showValue) {
	if (!(intervalPtr->flags & REDRAW_OTHER)) {
	    XClearArea(Tk_Display(tkwin), Tk_WindowId(tkwin),
		    valueRightEdge-intervalPtr->valuePixels, intervalPtr->offset,
		    intervalPtr->valuePixels,
		    Tk_Height(tkwin) - 2*intervalPtr->offset, False);
	}
	DisplayVerticalValue(intervalPtr, intervalPtr->minValue, valueRightEdge);
	DisplayVerticalValue(intervalPtr, intervalPtr->maxValue, valueRightEdge);
    }

    /*
     * Display the interval and the slider.  If not redisplaying the
     * entire window, must clear the trench area to erase the old
     * slider, but don't need to redraw the border.
     */

    if (intervalPtr->flags & REDRAW_OTHER) {
	Tk_Draw3DRectangle(Tk_Display(tkwin), Tk_WindowId(tkwin),
		intervalPtr->bgBorder, intervalLeftEdge, intervalPtr->offset,
		intervalPtr->width + 2*intervalPtr->borderWidth,
		Tk_Height(tkwin) - 2*intervalPtr->offset, intervalPtr->borderWidth,
		TK_RELIEF_SUNKEN);
    } else {
	XClearArea(Tk_Display(tkwin), Tk_WindowId(tkwin),
		intervalLeftEdge + intervalPtr->borderWidth,
		intervalPtr->offset + intervalPtr->borderWidth,
		intervalPtr->width,
		Tk_Height(tkwin) - 2*intervalPtr->offset
		- 2*intervalPtr->borderWidth, False);
    }
    if (intervalPtr->flags & ACTIVE) {
	sliderBorder = intervalPtr->activeBorder;
    } else {
	sliderBorder = intervalPtr->sliderBorder;
    }
    width = intervalPtr->width;

    min = ValueToPixel(intervalPtr, intervalPtr->minValue);
    max = ValueToPixel(intervalPtr, intervalPtr->maxValue);

    height = (max - min) + (2 * intervalPtr->borderWidth);

    x = intervalLeftEdge + intervalPtr->borderWidth;

    relief = (intervalPtr->flags & BUTTON_PRESSED) ? TK_RELIEF_SUNKEN
	    : TK_RELIEF_RAISED;
    Tk_Fill3DRectangle(Tk_Display(tkwin), Tk_WindowId(tkwin), sliderBorder,
	    x, min, width, height, intervalPtr->borderWidth, relief);

    /*
     * Draw the label to the right of the interval.
     */

    if ((intervalPtr->flags & REDRAW_OTHER) && (intervalPtr->labelPixels != 0)) {
	XDrawString(Tk_Display(intervalPtr->tkwin), Tk_WindowId(intervalPtr->tkwin),
	    intervalPtr->textGC, labelLeftEdge,
	    intervalPtr->offset + (3*intervalPtr->fontPtr->ascent)/2,
	    intervalPtr->label, intervalPtr->labelLength);
    }

    /*
     * Draw the window border.
     */

    if ((intervalPtr->flags & REDRAW_OTHER)
	    && (intervalPtr->relief != TK_RELIEF_FLAT)) {
	Tk_Draw3DRectangle(Tk_Display(tkwin), Tk_WindowId(tkwin),
		intervalPtr->bgBorder, 0, 0, Tk_Width(tkwin), Tk_Height(tkwin),
		intervalPtr->borderWidth, intervalPtr->relief);
    }

    done:
    intervalPtr->flags &= ~REDRAW_ALL;
}

/*
 *----------------------------------------------------------------------
 *
 * DisplayVerticalValue --
 *
 *	This procedure is called to display values (interval readings)
 *	for vertically-oriented intervals.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The numerical value corresponding to value is displayed with
 *	its right edge at "rightEdge", and at a vertical position in
 *	the interval that corresponds to "value".
 *
 *----------------------------------------------------------------------
 */

static void
DisplayVerticalValue(intervalPtr, value, rightEdge)
    register Interval *intervalPtr;	/* Information about widget in which to
				 * display value. */
    int value;			/* Y-coordinate of number to display,
				 * specified in application coords, not
				 * in pixels (we'll compute pixels). */
    int rightEdge;		/* X-coordinate of right edge of text,
				 * specified in pixels. */
{
    register Tk_Window tkwin = intervalPtr->tkwin;
    int y, dummy, length;
    char valueString[30];
    XCharStruct bbox;

    y = ValueToPixel(intervalPtr, value) + intervalPtr->fontPtr->ascent/2;
    sprintf(valueString, "%d", value);
    length = strlen(valueString);
    XTextExtents(intervalPtr->fontPtr, valueString, length,
	    &dummy, &dummy, &dummy, &bbox);

    /*
     * Adjust the y-coordinate if necessary to keep the text entirely
     * inside the window.
     */

    if ((y - bbox.ascent) < intervalPtr->offset) {
	y = intervalPtr->offset + bbox.ascent;
    }
    if ((y + bbox.descent) > (Tk_Height(tkwin) - intervalPtr->offset)) {
	y = Tk_Height(tkwin) - intervalPtr->offset - bbox.descent;
    }
    XDrawString(Tk_Display(tkwin), Tk_WindowId(tkwin),
	    intervalPtr->textGC, rightEdge - bbox.rbearing,
	    y, valueString, length);
}

/*
 *--------------------------------------------------------------
 *
 * DisplayHorizontalInterval --
 *
 *	This procedure redraws the contents of a horizontal interval
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
DisplayHorizontalInterval(clientData)
    ClientData clientData;	/* Information about widget. */
{
    register Interval *intervalPtr = (Interval *) clientData;
    register Tk_Window tkwin = intervalPtr->tkwin;
    int tickBottom, valueBottom, labelBottom, intervalBottom;
    int totalPixels, x, y, width, height, tickValue, min, max;
    int relief;
    Tk_3DBorder sliderBorder;

    if ((intervalPtr->tkwin == NULL) || !Tk_IsMapped(tkwin)) {
	goto done;
    }

    /*
     * Scanning from bottom to top across the window, the window
     * will contain four rows:  ticks, value, interval, and label.
     * Compute the y-coordinate for each of the rows.
     */

    totalPixels = intervalPtr->tickPixels + intervalPtr->valuePixels
	    + 2*intervalPtr->borderWidth + intervalPtr->width
	    + intervalPtr->labelPixels;
    tickBottom = (Tk_Height(tkwin) + totalPixels)/2 - 1;
    valueBottom = tickBottom - intervalPtr->tickPixels;
    intervalBottom = valueBottom - intervalPtr->valuePixels;
    labelBottom = intervalBottom - 2*intervalPtr->borderWidth - intervalPtr->width;

    /*
     * Display the information from bottom to top across the window.
     */

    if (intervalPtr->flags & REDRAW_OTHER) {
	XClearWindow(Tk_Display(tkwin), Tk_WindowId(tkwin));

	/*
	 * Display the tick marks.
	 */

	if (intervalPtr->tickPixels != 0) {
	    for (tickValue = intervalPtr->fromValue; ;
		    tickValue += intervalPtr->tickInterval) {
		if (intervalPtr->toValue > intervalPtr->fromValue) {
		    if (tickValue > intervalPtr->toValue) {
			break;
		    }
		} else {
		    if (tickValue < intervalPtr->toValue) {
			break;
		    }
		}
		DisplayHorizontalValue(intervalPtr, tickValue, tickBottom);
	    }
	}
    }

    /*
     * Display the values, if they are desired.  If not redisplaying the
     * entire window, clear the area of the value to get rid of the
     * old value displayed there.
     */

    if (intervalPtr->showValue) {
	if (!(intervalPtr->flags & REDRAW_OTHER)) {
	    XClearArea(Tk_Display(tkwin), Tk_WindowId(tkwin),
		    intervalPtr->offset, intervalBottom + 1,
		    Tk_Width(tkwin) - 2*intervalPtr->offset,
		    valueBottom - intervalBottom, False);
	}
	DisplayHorizontalValue(intervalPtr, intervalPtr->minValue, valueBottom);
	DisplayHorizontalValue(intervalPtr, intervalPtr->maxValue, valueBottom);
    }

    /*
     * Display the interval and the slider.  If not redisplaying the
     * entire window, must clear the trench area to erase the old
     * slider, but don't need to redraw the border.
     */

    y = intervalBottom - 2*intervalPtr->borderWidth - intervalPtr->width + 1;
    if (intervalPtr->flags & REDRAW_OTHER) {
	Tk_Draw3DRectangle(Tk_Display(tkwin), Tk_WindowId(tkwin),
		intervalPtr->bgBorder, intervalPtr->offset, y,
		Tk_Width(tkwin) - 2*intervalPtr->offset,
		intervalPtr->width + 2*intervalPtr->borderWidth,
		intervalPtr->borderWidth, TK_RELIEF_SUNKEN);
    } else {
	XClearArea(Tk_Display(tkwin), Tk_WindowId(tkwin),
		intervalPtr->offset + intervalPtr->borderWidth,
		y + intervalPtr->borderWidth,
		Tk_Width(tkwin) - 2*intervalPtr->offset - 2*intervalPtr->borderWidth,
		intervalPtr->width, False);
    }
    if (intervalPtr->flags & ACTIVE) {
	sliderBorder = intervalPtr->activeBorder;
    } else {
	sliderBorder = intervalPtr->sliderBorder;
    }
    height = intervalPtr->width;

    min = ValueToPixel(intervalPtr, intervalPtr->minValue);
    max = ValueToPixel(intervalPtr, intervalPtr->maxValue);

    width = (max - min) + (2 * intervalPtr->borderWidth);

    y += intervalPtr->borderWidth;
    relief = (intervalPtr->flags & BUTTON_PRESSED) ? TK_RELIEF_SUNKEN
	    : TK_RELIEF_RAISED;
    Tk_Fill3DRectangle(Tk_Display(tkwin), Tk_WindowId(tkwin), sliderBorder,
	    min, y, width, height, intervalPtr->borderWidth, relief);

    /*
     * Draw the label to the top of the interval.
     */

    if ((intervalPtr->flags & REDRAW_OTHER) && (intervalPtr->labelPixels != 0)) {
	XDrawString(Tk_Display(intervalPtr->tkwin), Tk_WindowId(intervalPtr->tkwin),
	    intervalPtr->textGC, intervalPtr->offset + intervalPtr->fontPtr->ascent/2,
	    labelBottom - intervalPtr->fontPtr->descent,
	    intervalPtr->label, intervalPtr->labelLength);
    }

    /*
     * Draw the window border.
     */

    if ((intervalPtr->flags & REDRAW_OTHER)
	    && (intervalPtr->relief != TK_RELIEF_FLAT)) {
	Tk_Draw3DRectangle(Tk_Display(tkwin), Tk_WindowId(tkwin),
		intervalPtr->bgBorder, 0, 0, Tk_Width(tkwin), Tk_Height(tkwin),
		intervalPtr->borderWidth, intervalPtr->relief);
    }

    done:
    intervalPtr->flags &= ~REDRAW_ALL;
}

/*
 *----------------------------------------------------------------------
 *
 * DisplayHorizontalValue --
 *
 *	This procedure is called to display values (interval readings)
 *	for horizontally-oriented intervals.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The numerical value corresponding to value is displayed with
 *	its bottom edge at "bottom", and at a horizontal position in
 *	the interval that corresponds to "value".
 *
 *----------------------------------------------------------------------
 */

static void
DisplayHorizontalValue(intervalPtr, value, bottom)
    register Interval *intervalPtr;	/* Information about widget in which to
				 * display value. */
    int value;			/* Y-coordinate of number to display,
				 * specified in application coords, not
				 * in pixels (we'll compute pixels). */
    int bottom;			/* Y-coordinate of bottom edge of text,
				 * specified in pixels. */
{
    register Tk_Window tkwin = intervalPtr->tkwin;
    int x, y, dummy, length;
    char valueString[30];
    XCharStruct bbox;

    x = ValueToPixel(intervalPtr, value);
    y = bottom - intervalPtr->fontPtr->descent;
    sprintf(valueString, "%d", value);
    length = strlen(valueString);
    XTextExtents(intervalPtr->fontPtr, valueString, length,
	    &dummy, &dummy, &dummy, &bbox);

    /*
     * Adjust the x-coordinate if necessary to keep the text entirely
     * inside the window.
     */

    x -= (bbox.lbearing + bbox.rbearing)/2;
    if ((x - bbox.lbearing) < intervalPtr->offset) {
	x = intervalPtr->offset + bbox.lbearing;
    }
    if ((x + bbox.rbearing) > (Tk_Width(tkwin) - intervalPtr->offset)) {
	x = Tk_Width(tkwin) - intervalPtr->offset - bbox.rbearing;
    }
    XDrawString(Tk_Display(tkwin), Tk_WindowId(tkwin),
	    intervalPtr->textGC, x, y, valueString, length);
}

/*
 *----------------------------------------------------------------------
 *
 * PixelToValue --
 *
 *	Given a pixel within a interval window, return the interval
 *	reading corresponding to that pixel.
 *
 * Results:
 *	An integer interval reading.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

static int
PixelToValue(intervalPtr, x, y)
    register Interval *intervalPtr;		/* Information about widget. */
    int x, y;				/* Coordinates of point within
					 * window. */
{
    int value, pixelRange;

    if (intervalPtr->vertical) {
	pixelRange = Tk_Height(intervalPtr->tkwin)
		- 2*intervalPtr->offset - 4*intervalPtr->borderWidth;
	value = y;
    } else {
	pixelRange = Tk_Width(intervalPtr->tkwin)
		- 2*intervalPtr->offset - 4*intervalPtr->borderWidth;
	value = x;
    }

    if (pixelRange <= 0) {
	/*
	 * Not enough room for the slider to actually slide:  just return
	 * a constant.
	 */

	return (0);
    }
    value -= intervalPtr->offset + intervalPtr->borderWidth;
#if 0
    if (value < 0) {
	value = 0;
    }
    if (value > pixelRange) {
	value = pixelRange;
    }
#endif
    if (intervalPtr->toValue > intervalPtr->fromValue) {
	value = intervalPtr->fromValue +
		((value * (intervalPtr->toValue - intervalPtr->fromValue))
		+ pixelRange/2)/pixelRange;
    } else {
	value = intervalPtr->toValue +
		(((pixelRange - value)
		* (intervalPtr->fromValue - intervalPtr->toValue))
		+ pixelRange/2)/pixelRange;
    }
    return value;
}

/*
 *----------------------------------------------------------------------
 *
 * ValueToPixel --
 *
 *	Given a reading of the interval, return the x-coordinate or
 *	y-coordinate corresponding to that reading, depending on
 *	whether the interval is vertical or horizontal, respectively.
 *
 * Results:
 *	An integer value giving the pixel location corresponding
 *	to reading.  The value is restricted to lie within the
 *	defined range for the interval.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

static int
ValueToPixel(intervalPtr, value)
    register Interval *intervalPtr;		/* Information about widget. */
    int value;				/* Reading of the widget. */
{
    int y, pixelRange, valueRange;

    valueRange = intervalPtr->toValue - intervalPtr->fromValue;
    pixelRange = (intervalPtr->vertical ? Tk_Height(intervalPtr->tkwin)
					: Tk_Width(intervalPtr->tkwin))
	    - 2*intervalPtr->offset - 4*intervalPtr->borderWidth;
    y = ((value - intervalPtr->fromValue) * pixelRange
	    + valueRange/2) / valueRange;
    if (y < 0) {
	y = 0;
    } else if (y > pixelRange) {
	y = pixelRange;
    }
    y += intervalPtr->offset + intervalPtr->borderWidth;
    return y;
}

/*
 *--------------------------------------------------------------
 *
 * IntervalEventProc --
 *
 *	This procedure is invoked by the Tk dispatcher for various
 *	events on intervals.
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
IntervalEventProc(clientData, eventPtr)
    ClientData clientData;	/* Information about window. */
    XEvent *eventPtr;		/* Information about event. */
{
    Interval *intervalPtr = (Interval *) clientData;

    if ((eventPtr->type == Expose) && (eventPtr->xexpose.count == 0)) {
	EventuallyRedrawInterval(intervalPtr, REDRAW_ALL);
    } else if (eventPtr->type == DestroyNotify) {
	Tcl_DeleteCommand(intervalPtr->interp, Tk_PathName(intervalPtr->tkwin));
	intervalPtr->tkwin = NULL;
	if (intervalPtr->flags & REDRAW_ALL) {
	    if (intervalPtr->vertical) {
		Tk_CancelIdleCall(DisplayVerticalInterval, (ClientData) intervalPtr);
	    } else {
		Tk_CancelIdleCall(DisplayHorizontalInterval,
			(ClientData) intervalPtr);
	    }
	}
	Tk_EventuallyFree((ClientData) intervalPtr, DestroyInterval);
    } else if (eventPtr->type == ConfigureNotify) {
	ComputeIntervalGeometry(intervalPtr);
    }
}

/*
 *--------------------------------------------------------------
 *
 * IntervalMouseProc --
 *
 *	This procedure is called back by Tk in response to
 *	mouse events such as window entry, window exit, mouse
 *	motion, and button presses.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	This procedure implements the "feel" of the interval by
 *	issuing commands in response to button presses and mouse
 *	motion.
 *
 *--------------------------------------------------------------
 */

static void
IntervalMouseProc(clientData, eventPtr)
    ClientData clientData;		/* Information about window. */
    register XEvent *eventPtr;		/* Information about event. */
{
    register Interval *intervalPtr = (Interval *) clientData;

    if (intervalPtr->state != tkNormalUid) {
	return;
    }

    Tk_Preserve((ClientData) intervalPtr);
    if (eventPtr->type == EnterNotify) {
	intervalPtr->flags |= ACTIVE;
	EventuallyRedrawInterval(intervalPtr, REDRAW_SLIDER);
    } else if (eventPtr->type == LeaveNotify) {
	intervalPtr->flags &= ~ACTIVE;
	EventuallyRedrawInterval(intervalPtr, REDRAW_SLIDER);
    } else if ((eventPtr->type == MotionNotify)
	    && (intervalPtr->flags & BUTTON_PRESSED)) {
	TrackInterval(intervalPtr,  PixelToValue(intervalPtr,
		eventPtr->xmotion.x, eventPtr->xmotion.y));
    } else if ((eventPtr->type == ButtonPress)
/*	    && (eventPtr->xbutton.button == Button1) */
	    && (eventPtr->xbutton.state == 0)) {
	intervalPtr->flags |= BUTTON_PRESSED;
	StartTrackInterval(intervalPtr, PixelToValue(intervalPtr,
		eventPtr->xbutton.x, eventPtr->xbutton.y));
/*	NotifyInterval(intervalPtr); */
	EventuallyRedrawInterval(intervalPtr, REDRAW_SLIDER);
    } else if ((eventPtr->type == ButtonRelease)
/*	    && (eventPtr->xbutton.button == Button1) */
	    && (intervalPtr->flags & BUTTON_PRESSED)) {
	intervalPtr->flags &= ~BUTTON_PRESSED;
	TrackInterval(intervalPtr,  PixelToValue(intervalPtr,
		eventPtr->xmotion.x, eventPtr->xmotion.y));
/*	NotifyInterval(intervalPtr); */
	EventuallyRedrawInterval(intervalPtr, REDRAW_SLIDER);
    }
    Tk_Release((ClientData) intervalPtr);
}

/*
 *--------------------------------------------------------------
 *
 * TrackInterval --
 *
 *	This procedure changes the value of a interval and invokes
 *	a Tcl command to reflect the current position of a interval
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	A Tcl command is invoked, and an additional error-processing
 *	command may also be invoked.  The interval's slider is redrawn.
 *
 *--------------------------------------------------------------
 */

static void
StartTrackInterval(intervalPtr, value)
    register Interval *intervalPtr;	/* Info about widget. */
    int value;			/* New value for interval.  Gets
				 * adjusted if it's off the interval. */
{
    int result;
    char string[20];

    if ((value < intervalPtr->fromValue)
	    ^ (intervalPtr->toValue < intervalPtr->fromValue)) {
	value = intervalPtr->fromValue;
    }
    if ((value > intervalPtr->toValue)
	    ^ (intervalPtr->toValue < intervalPtr->fromValue)) {
	value = intervalPtr->toValue;
    }
    intervalPtr->trackValue = value;
    intervalPtr->trackWidth = intervalPtr->maxValue - intervalPtr->minValue;
    if (value <= intervalPtr->minValue) {
      intervalPtr->trackState = -1;
    } else if (value >= intervalPtr->maxValue) {
      intervalPtr->trackState = 1;
    } else {
      intervalPtr->trackState = 0;
    }
    SetInterval(intervalPtr, intervalPtr->minValue, intervalPtr->maxValue, 1);
}


static void
TrackInterval(intervalPtr, value)
    register Interval *intervalPtr;	/* Info about widget. */
    int value;
{
    int result;
    char string[20];
    int min, max, delta, lastmin, lastmax;


    delta = value - intervalPtr->trackValue;
    if (delta == 0) return;

    intervalPtr->trackValue = value;

    min = intervalPtr->minValue;
    max = intervalPtr->maxValue;

    switch (intervalPtr->trackState) {
    case -1: /* left trench */
      min += delta;
      if (min > max) max = min;
      break;
    case 1: /* right trench */
      max += delta;
      if (min > max) min = max;
      break;
    case 0: /* center slider */
      lastmin = min; lastmax = max;
      min += delta; max += delta;
      if ((max - min) != intervalPtr->trackWidth) { /* squished */
	if (lastmin == intervalPtr->fromValue) {
	  min = max - intervalPtr->trackWidth;
	} else if (lastmax == intervalPtr->toValue) {
	  max = min + intervalPtr->trackWidth;
	}
      }
      break;
    }

    SetInterval(intervalPtr, min, max, 1);
}


static void
SetInterval(intervalPtr, min, max, notify)
    register Interval *intervalPtr;	/* Info about widget. */
    int min, max, notify;
{
    if (min > max) {
      int temp = min;
      min = max;
      max = temp;
    }

    if ((min < intervalPtr->fromValue)
	    ^ (intervalPtr->toValue < intervalPtr->fromValue)) {
	min = intervalPtr->fromValue;
    }
    if ((min > intervalPtr->toValue)
	    ^ (intervalPtr->toValue < intervalPtr->fromValue)) {
	min = intervalPtr->toValue;
    }
    if ((max < intervalPtr->fromValue)
	    ^ (intervalPtr->toValue < intervalPtr->fromValue)) {
	max = intervalPtr->fromValue;
    }
    if ((max > intervalPtr->toValue)
	    ^ (intervalPtr->toValue < intervalPtr->fromValue)) {
	max = intervalPtr->toValue;
    }

    if ((min == intervalPtr->minValue) &&
	(max == intervalPtr->maxValue)) {
      return;
    }

    intervalPtr->minValue = min;
    intervalPtr->maxValue = max;
    EventuallyRedrawInterval(intervalPtr, REDRAW_SLIDER);

    if (notify)
      NotifyInterval(intervalPtr);
}


NotifyInterval(intervalPtr)
    register Interval *intervalPtr;	/* Info about widget. */
{
    int result;
    char string[256];

    sprintf(string, " %d %d", intervalPtr->minValue, intervalPtr->maxValue);
    result = Tcl_VarEval(intervalPtr->interp, intervalPtr->command, string,
	    (char *) NULL);
    if (result != TCL_OK) {
	TkBindError(intervalPtr->interp);
    }
}



/*
 *--------------------------------------------------------------
 *
 * EventuallyRedrawInterval --
 *
 *	Arrange for part or all of a interval widget to redrawn at
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
EventuallyRedrawInterval(intervalPtr, what)
    register Interval *intervalPtr;	/* Information about widget. */
    int what;			/* What to redraw:  REDRAW_SLIDER
				 * or REDRAW_ALL. */
{
    if ((what == 0) || (intervalPtr->tkwin == NULL)
	    || !Tk_IsMapped(intervalPtr->tkwin)) {
	return;
    }
    if ((intervalPtr->flags & REDRAW_ALL) == 0) {
	if (intervalPtr->vertical) {
	    Tk_DoWhenIdle(DisplayVerticalInterval, (ClientData) intervalPtr);
	} else {
	    Tk_DoWhenIdle(DisplayHorizontalInterval, (ClientData) intervalPtr);
	}
    }
    intervalPtr->flags |= what;
}
