/* 
 * tkButton.c --
 *
 *	This module implements a collection of button-like
 *	widgets for the Tk toolkit.  The widgets implemented
 *	include labels, buttons, check buttons, and radio
 *	buttons.
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
static char rcsid[] = "$Header: /user6/ouster/wish/RCS/tkButton.c,v 1.69 92/08/21 11:42:47 ouster Exp $ SPRITE (Berkeley)";
#endif

#include "default.h"
#include "tkconfig.h"
#include "tkint.h"

#include <assert.h>

/*
 * A data structure of the following type is kept for each
 * widget managed by this file:
 */

typedef struct {
    Tk_Window tkwin;		/* Window that embodies the button.  NULL
				 * means that the window has been destroyed. */
    Tcl_Interp *interp;		/* Interpreter associated with button. */
    int type;			/* Type of widget:  restricts operations
				 * that may be performed on widget.  See
				 * below for possible values. */

    /*
     * Information about what's in the button.
     */

    char *text;			/* Text to display in button (malloc'ed)
				 * or NULL. */
    int textLength;		/* # of characters in text. */
    char *textVarName;		/* Name of variable (malloc'ed) or NULL.
				 * If non-NULL, button displays the contents
				 * of this variable. */
    Pixmap bitmap;		/* Bitmap to display or None.  If not None
				 * then text and textVar are ignored. */

    /*
     * Information used when displaying widget:
     */

    Tk_Uid state;		/* State of button for display purposes:
				 * normal, active, or disabled. */
    Tk_3DBorder normalBorder;	/* Structure used to draw 3-D
				 * border and background when window
				 * isn't active.  NULL means no such
				 * border exists. */
    Tk_3DBorder activeBorder;	/* Structure used to draw 3-D
				 * border and background when window
				 * is active.  NULL means no such
				 * border exists. */
    int borderWidth;		/* Width of border. */
    int relief;			/* 3-d effect: TK_RELIEF_RAISED, etc. */
    XFontStruct *fontPtr;	/* Information about text font, or NULL. */
    XColor *normalFg;		/* Foreground color in normal mode. */
    XColor *activeFg;		/* Foreground color in active mode.  NULL
				 * means use normalFg instead. */
    XColor *disabledFg;		/* Foreground color when disabled.  NULL
				 * means use normalFg with a 50% stipple
				 * instead. */
    GC normalTextGC;		/* GC for drawing text in normal mode.  Also
				 * used to copy from off-screen pixmap onto
				 * screen. */
    GC activeTextGC;		/* GC for drawing text in active mode (NULL
				 * means use normalTextGC). */
    Pixmap gray;		/* Pixmap for displaying disabled text if
				 * disabledFg is NULL. */
    GC disabledGC;		/* Used to produce disabled effect.  If
				 * disabledFg isn't NULL, this GC is used to
				 * draw button text or icon.  Otherwise
				 * text or icon is drawn with normalGC and
				 * this GC is used to stipple background
				 * across it. */
    int leftBearing;		/* Amount text sticks left from its origin,
				 * in pixels. */
    int rightBearing;		/* Amount text sticks right from its origin. */
    int width, height;		/* If > 0, these specify dimensions to request
				 * for window, in characters for text and in
				 * pixels for bitmaps.  In this case the actual
				 * size of the text string or bitmap is
				 * ignored in computing desired window size. */
    int padX, padY;		/* Extra space around text or bitmap (pixels
				 * on each side). */
    Tk_Anchor anchor;		/* Where text/bitmap should be displayed
				 * inside button region. */
    XColor *selectorFg;		/* Color for selector. */
    GC selectorGC;		/* For drawing highlight when this button
				 * is in selected state. */
    int selectorSpace;		/* Horizontal space (in pixels) allocated for
				 * display of selector. */
    int selectorDiameter;	/* Diameter of selector, in pixels. */

    /*
     * For check and radio buttons, the fields below are used
     * to manage the variable indicating the button's state.
     */

    char *selVarName;		/* Name of variable used to control selected
				 * state of button.  Malloc'ed (if
				 * not NULL). */
    char *onValue;		/* Value to store in variable when
				 * this button is selected.  Malloc'ed (if
				 * not NULL). */
    char *offValue;		/* Value to store in variable when this
				 * button isn't selected.  Malloc'ed
				 * (if not NULL).  Valid only for check
				 * buttons. */

    /*
     * Miscellaneous information:
     */

    Cursor cursor;		/* Current cursor for window, or None. */
    char *command;		/* Command to execute when button is
				 * invoked; valid for buttons only.
				 * If not NULL, it's malloc-ed. */
    int flags;			/* Various flags;  see below for
				 * definitions. */
    Tk_TimerToken updateTimerToken; /* Added by Don to optimize rapid 
				     * updates. */
} Button;

/*
 * Possible "type" values for buttons.  These are the kinds of
 * widgets supported by this file.  The ordering of the type
 * numbers is significant:  greater means more features and is
 * used in the code.
 */

#define TYPE_LABEL		0
#define TYPE_BUTTON		1
#define TYPE_CHECK_BUTTON	2
#define TYPE_RADIO_BUTTON	3

/*
 * Class names for buttons, indexed by one of the type values above.
 */

static char *classNames[] = {"Label", "Button", "CheckButton", "RadioButton"};

/*
 * Flag bits for buttons:
 *
 * REDRAW_PENDING:		Non-zero means a DoWhenIdle handler
 *				has already been queued to redraw
 *				this window.
 * SELECTED:			Non-zero means this button is selected,
 *				so special highlight should be drawn.
 */

#define REDRAW_PENDING		1
#define SELECTED		2

/*
 * Mask values used to selectively enable entries in the
 * configuration specs:
 */

#define LABEL_MASK		TK_CONFIG_USER_BIT
#define BUTTON_MASK		TK_CONFIG_USER_BIT << 1
#define CHECK_BUTTON_MASK	TK_CONFIG_USER_BIT << 2
#define RADIO_BUTTON_MASK	TK_CONFIG_USER_BIT << 3
#define ALL_MASK		(LABEL_MASK | BUTTON_MASK \
	| CHECK_BUTTON_MASK | RADIO_BUTTON_MASK)

static int configFlags[] = {LABEL_MASK, BUTTON_MASK,
	CHECK_BUTTON_MASK, RADIO_BUTTON_MASK};
/*
 * Information used for parsing configuration specs:
 */

static Tk_ConfigSpec configSpecs[] = {
    {TK_CONFIG_BORDER, "-activebackground", "activeBackground", "Foreground",
	DEF_BUTTON_ACTIVE_BG_COLOR, Tk_Offset(Button, activeBorder),
	BUTTON_MASK|CHECK_BUTTON_MASK|RADIO_BUTTON_MASK
	|TK_CONFIG_COLOR_ONLY},
    {TK_CONFIG_BORDER, "-activebackground", "activeBackground", "Foreground",
	DEF_BUTTON_ACTIVE_BG_MONO, Tk_Offset(Button, activeBorder),
	BUTTON_MASK|CHECK_BUTTON_MASK|RADIO_BUTTON_MASK
	|TK_CONFIG_MONO_ONLY},
    {TK_CONFIG_COLOR, "-activeforeground", "activeForeground", "Background",
	DEF_BUTTON_ACTIVE_FG_COLOR, Tk_Offset(Button, activeFg), 
	BUTTON_MASK|CHECK_BUTTON_MASK|RADIO_BUTTON_MASK
	|TK_CONFIG_COLOR_ONLY},
    {TK_CONFIG_COLOR, "-activeforeground", "activeForeground", "Background",
	DEF_BUTTON_ACTIVE_FG_MONO, Tk_Offset(Button, activeFg), 
	BUTTON_MASK|CHECK_BUTTON_MASK|RADIO_BUTTON_MASK
	|TK_CONFIG_MONO_ONLY},
    {TK_CONFIG_ANCHOR, "-anchor", "anchor", "Anchor",
	DEF_BUTTON_ANCHOR, Tk_Offset(Button, anchor), ALL_MASK},
    {TK_CONFIG_BORDER, "-background", "background", "Background",
	DEF_BUTTON_BG_COLOR, Tk_Offset(Button, normalBorder),
	ALL_MASK | TK_CONFIG_COLOR_ONLY},
    {TK_CONFIG_BORDER, "-background", "background", "Background",
	DEF_BUTTON_BG_MONO, Tk_Offset(Button, normalBorder),
	ALL_MASK | TK_CONFIG_MONO_ONLY},
    {TK_CONFIG_SYNONYM, "-bd", "borderWidth", (char *) NULL,
	(char *) NULL, 0, ALL_MASK},
    {TK_CONFIG_SYNONYM, "-bg", "background", (char *) NULL,
	(char *) NULL, 0, ALL_MASK},
#if defined(USE_XPM3)
    {TK_CONFIG_PIXMAP, "-bitmap", "bitmap", "Bitmap",
	DEF_BUTTON_BITMAP, Tk_Offset(Button, bitmap),
	ALL_MASK|TK_CONFIG_NULL_OK},
#else
    {TK_CONFIG_BITMAP, "-bitmap", "bitmap", "Bitmap",
	DEF_BUTTON_BITMAP, Tk_Offset(Button, bitmap),
	ALL_MASK|TK_CONFIG_NULL_OK},
#endif
    {TK_CONFIG_PIXELS, "-borderwidth", "borderWidth", "BorderWidth",
	DEF_BUTTON_BORDER_WIDTH, Tk_Offset(Button, borderWidth), ALL_MASK},
    {TK_CONFIG_STRING, "-command", "command", "Command",
	DEF_BUTTON_COMMAND, Tk_Offset(Button, command),
	BUTTON_MASK|CHECK_BUTTON_MASK|RADIO_BUTTON_MASK},
    {TK_CONFIG_ACTIVE_CURSOR, "-cursor", "cursor", "Cursor",
	DEF_BUTTON_CURSOR, Tk_Offset(Button, cursor),
	ALL_MASK|TK_CONFIG_NULL_OK},
    {TK_CONFIG_COLOR, "-disabledforeground", "disabledForeground",
	"DisabledForeground", DEF_BUTTON_DISABLED_FG_COLOR,
	Tk_Offset(Button, disabledFg), BUTTON_MASK|CHECK_BUTTON_MASK
	|RADIO_BUTTON_MASK|TK_CONFIG_COLOR_ONLY|TK_CONFIG_NULL_OK},
    {TK_CONFIG_COLOR, "-disabledforeground", "disabledForeground",
	"DisabledForeground", DEF_BUTTON_DISABLED_FG_MONO,
	Tk_Offset(Button, disabledFg), BUTTON_MASK|CHECK_BUTTON_MASK
	|RADIO_BUTTON_MASK|TK_CONFIG_MONO_ONLY|TK_CONFIG_NULL_OK},
    {TK_CONFIG_SYNONYM, "-fg", "foreground", (char *) NULL,
	(char *) NULL, 0, ALL_MASK},
    {TK_CONFIG_FONT, "-font", "font", "Font",
	DEF_BUTTON_FONT, Tk_Offset(Button, fontPtr),
	ALL_MASK},
    {TK_CONFIG_COLOR, "-foreground", "foreground", "Foreground",
	DEF_BUTTON_FG, Tk_Offset(Button, normalFg), ALL_MASK},
    {TK_CONFIG_INT, "-height", "height", "Height",
	DEF_BUTTON_HEIGHT, Tk_Offset(Button, height), ALL_MASK},
    {TK_CONFIG_STRING, "-offvalue", "offValue", "Value",
	DEF_BUTTON_OFF_VALUE, Tk_Offset(Button, offValue),
	CHECK_BUTTON_MASK},
    {TK_CONFIG_STRING, "-onvalue", "onValue", "Value",
	DEF_BUTTON_ON_VALUE, Tk_Offset(Button, onValue),
	CHECK_BUTTON_MASK},
    {TK_CONFIG_PIXELS, "-padx", "padX", "Pad",
	DEF_BUTTON_PADX, Tk_Offset(Button, padX), ALL_MASK},
    {TK_CONFIG_PIXELS, "-pady", "padY", "Pad",
	DEF_BUTTON_PADY, Tk_Offset(Button, padY), ALL_MASK},
    {TK_CONFIG_RELIEF, "-relief", "relief", "Relief",
	DEF_BUTTON_RELIEF, Tk_Offset(Button, relief), 
	BUTTON_MASK|CHECK_BUTTON_MASK|RADIO_BUTTON_MASK},
    {TK_CONFIG_RELIEF, "-relief", "relief", "Relief",
	DEF_LABEL_RELIEF, Tk_Offset(Button, relief), LABEL_MASK},
    {TK_CONFIG_COLOR, "-selector", "selector", "Foreground",
	DEF_BUTTON_SELECTOR_COLOR, Tk_Offset(Button, selectorFg),
	CHECK_BUTTON_MASK|RADIO_BUTTON_MASK|TK_CONFIG_COLOR_ONLY
	|TK_CONFIG_NULL_OK},
    {TK_CONFIG_COLOR, "-selector", "selector", "Foreground",
	DEF_BUTTON_SELECTOR_MONO, Tk_Offset(Button, selectorFg),
	CHECK_BUTTON_MASK|RADIO_BUTTON_MASK|TK_CONFIG_MONO_ONLY
	|TK_CONFIG_NULL_OK},
    {TK_CONFIG_UID, "-state", "state", "State",
	DEF_BUTTON_STATE, Tk_Offset(Button, state),
	BUTTON_MASK|CHECK_BUTTON_MASK|RADIO_BUTTON_MASK},
    {TK_CONFIG_STRING, "-text", "text", "Text",
	DEF_BUTTON_TEXT, Tk_Offset(Button, text), ALL_MASK},
    {TK_CONFIG_STRING, "-textvariable", "textVariable", "Variable",
	DEF_BUTTON_TEXT_VARIABLE, Tk_Offset(Button, textVarName),
	ALL_MASK|TK_CONFIG_NULL_OK},
    {TK_CONFIG_STRING, "-value", "value", "Value",
	DEF_BUTTON_VALUE, Tk_Offset(Button, onValue),
	RADIO_BUTTON_MASK},
    {TK_CONFIG_STRING, "-variable", "variable", "Variable",
	DEF_RADIOBUTTON_VARIABLE, Tk_Offset(Button, selVarName),
	RADIO_BUTTON_MASK},
    {TK_CONFIG_STRING, "-variable", "variable", "Variable",
	DEF_CHECKBUTTON_VARIABLE, Tk_Offset(Button, selVarName),
	CHECK_BUTTON_MASK},
    {TK_CONFIG_INT, "-width", "width", "Width",
	DEF_BUTTON_WIDTH, Tk_Offset(Button, width), ALL_MASK},
    {TK_CONFIG_END, (char *) NULL, (char *) NULL, (char *) NULL,
	(char *) NULL, 0, 0}
};

/*
 * String to print out in error messages, identifying options for
 * widget commands for different types of labels or buttons:
 */

static char *optionStrings[] = {
    "configure",
    "activate, configure, deactivate, flash, or invoke",
    "activate, configure, deactivate, deselect, flash, invoke, select, or toggle",
    "activate, configure, deactivate, deselect, flash, invoke, or select"
};

static int ButtonUpdateTime = 200; // Added by Don.

/*
 * Forward declarations for procedures defined later in this file:
 */

static void		ButtonEventProc _ANSI_ARGS_((ClientData clientData,
			    XEvent *eventPtr));
static char *		ButtonTextVarProc _ANSI_ARGS_((ClientData clientData,
			    Tcl_Interp *interp, char *name1, char *name2,
			    int flags));
static char *		ButtonVarProc _ANSI_ARGS_((ClientData clientData,
			    Tcl_Interp *interp, char *name1, char *name2,
			    int flags));
static int		ButtonWidgetCmd _ANSI_ARGS_((ClientData clientData,
			    Tcl_Interp *interp, int argc, char **argv));
static void		ComputeButtonGeometry _ANSI_ARGS_((Button *butPtr));
static int		ConfigureButton _ANSI_ARGS_((Tcl_Interp *interp,
			    Button *butPtr, int argc, char **argv,
			    int flags));
static void		DestroyButton _ANSI_ARGS_((ClientData clientData));
static void		DisplayButton _ANSI_ARGS_((ClientData clientData));
static int		InvokeButton  _ANSI_ARGS_((Button *butPtr));

/*
 *--------------------------------------------------------------
 *
 * Tk_ButtonCmd --
 *
 *	This procedure is invoked to process the "button", "label",
 *	"radiobutton", and "checkbutton" Tcl commands.  See the
 *	user documentation for details on what it does.
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
Tk_ButtonCmd(clientData, interp, argc, argv)
    ClientData clientData;	/* Main window associated with
				 * interpreter. */
    Tcl_Interp *interp;		/* Current interpreter. */
    int argc;			/* Number of arguments. */
    char **argv;		/* Argument strings. */
{
    register Button *butPtr;
    int type;
    Tk_Window tkwin = (Tk_Window) clientData;
    Tk_Window new;

    if (argc < 2) {
	Tcl_AppendResult(interp, "wrong # args:  should be \"",
		argv[0], " pathName ?options?\"", (char *) NULL);
	return TCL_ERROR;
    }

    switch (argv[0][0]) {
	case 'l':
	    type = TYPE_LABEL;
	    break;
	case 'b':
	    type = TYPE_BUTTON;
	    break;
	case 'c':
	    type = TYPE_CHECK_BUTTON;
	    break;
	case 'r':
	    type = TYPE_RADIO_BUTTON;
	    break;
	default:
	    sprintf(interp->result,
		    "unknown button-creation command \"%.50s\"");
	    return TCL_ERROR;
    }

    /*
     * Create the new window.
     */

    new = Tk_CreateWindowFromPath(interp, tkwin, argv[1], (char *) NULL);
    if (new == NULL) {
	return TCL_ERROR;
    }

    /*
     * Initialize the data structure for the button.
     */

    butPtr = (Button *) ckalloc(sizeof(Button));
    butPtr->tkwin = new;
    butPtr->interp = interp;
    butPtr->type = type;
    butPtr->text = NULL;
    butPtr->textVarName = NULL;
    butPtr->bitmap = None;
    butPtr->state = tkNormalUid;
    butPtr->normalBorder = NULL;
    butPtr->activeBorder = NULL;
    butPtr->borderWidth = 0;
    butPtr->relief = TK_RELIEF_FLAT;
    butPtr->fontPtr = NULL;
    butPtr->normalFg = NULL;
    butPtr->activeFg = NULL;
    butPtr->disabledFg = NULL;
    butPtr->normalTextGC = None;
    butPtr->activeTextGC = None;
    butPtr->gray = None;
    butPtr->disabledGC = None;
    butPtr->selectorFg = NULL;
    butPtr->selectorGC = None;
    butPtr->selVarName = NULL;
    butPtr->onValue = NULL;
    butPtr->offValue = NULL;
    butPtr->cursor = None;
    butPtr->command = NULL;
    butPtr->flags = 0;
    butPtr->updateTimerToken = 0;

    Tk_SetClass(new, classNames[type]);
//fprintf(stderr, "ButtonWidgetCmd Made %s %s\n", Tk_Class(butPtr->tkwin), Tk_PathName(butPtr->tkwin));
    Tk_CreateEventHandler(butPtr->tkwin, ExposureMask|StructureNotifyMask,
	    ButtonEventProc, (ClientData) butPtr);
    Tcl_CreateCommand(interp, Tk_PathName(butPtr->tkwin), ButtonWidgetCmd,
	    (ClientData) butPtr, (void (*)()) NULL);
    if (ConfigureButton(interp, butPtr, argc-2, argv+2,
	    configFlags[type]) != TCL_OK) {
	Tk_DestroyWindow(butPtr->tkwin);
	return TCL_ERROR;
    }

    interp->result = Tk_PathName(butPtr->tkwin);
    return TCL_OK;
}

/*
 *--------------------------------------------------------------
 *
 * ButtonWidgetCmd --
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
ButtonWidgetCmd(clientData, interp, argc, argv)
    ClientData clientData;	/* Information about button widget. */
    Tcl_Interp *interp;		/* Current interpreter. */
    int argc;			/* Number of arguments. */
    char **argv;		/* Argument strings. */
{
    register Button *butPtr = (Button *) clientData;
    int result = TCL_OK;
    int length;
    char c;

    if (argc < 2) {
	sprintf(interp->result,
		"wrong # args: should be \"%.50s option [arg arg ...]\"",
		argv[0]);
	return TCL_ERROR;
    }
    Tk_Preserve((ClientData) butPtr);
    c = argv[1][0];
    length = strlen(argv[1]);
    if ((c == 'a') && (strncmp(argv[1], "activate", length) == 0)
	    && (butPtr->type != TYPE_LABEL)) {
	if (argc > 2) {
	    sprintf(interp->result,
		    "wrong # args: should be \"%.50s activate\"",
		    argv[0]);
	    goto error;
	}
	if (butPtr->state != tkDisabledUid) {
	    butPtr->state = tkActiveUid;
	    Tk_SetBackgroundFromBorder(butPtr->tkwin, butPtr->activeBorder);
	    goto redisplay;
	}
    } else if ((c == 'c') && (strncmp(argv[1], "configure", length) == 0)) {
	if (argc == 2) {
	    result = Tk_ConfigureInfo(interp, butPtr->tkwin, configSpecs,
		    (char *) butPtr, (char *) NULL, configFlags[butPtr->type]);
	} else if (argc == 3) {
	    result = Tk_ConfigureInfo(interp, butPtr->tkwin, configSpecs,
		    (char *) butPtr, argv[2],
		    configFlags[butPtr->type]);
	} else {
	    result = ConfigureButton(interp, butPtr, argc-2, argv+2,
		    configFlags[butPtr->type] | TK_CONFIG_ARGV_ONLY);
	}
    } else if ((c == 'd') && (strncmp(argv[1], "deactivate", length) == 0)
	    && (length > 2) && (butPtr->type != TYPE_LABEL)) {
	if (argc > 2) {
	    sprintf(interp->result,
		    "wrong # args: should be \"%.50s deactivate\"",
		    argv[0]);
	    goto error;
	}
	if (butPtr->state != tkDisabledUid) {
	    butPtr->state = tkNormalUid;
	    Tk_SetBackgroundFromBorder(butPtr->tkwin, butPtr->normalBorder);
	    goto redisplay;
	}
    } else if ((c == 'd') && (strncmp(argv[1], "deselect", length) == 0)
	    && (length > 2) && (butPtr->type >= TYPE_CHECK_BUTTON)) {
	if (argc > 2) {
	    sprintf(interp->result,
		    "wrong # args: should be \"%.50s deselect\"",
		    argv[0]);
	    goto error;
	}
	if (butPtr->type == TYPE_CHECK_BUTTON) {
	    Tcl_SetVar(interp, butPtr->selVarName, butPtr->offValue,
		    TCL_GLOBAL_ONLY);
	} else if (butPtr->flags & SELECTED) {
	    Tcl_SetVar(interp, butPtr->selVarName, "", TCL_GLOBAL_ONLY);
	}
    } else if ((c == 'f') && (strncmp(argv[1], "flash", length) == 0)
	&& (butPtr->type != TYPE_LABEL)) {
	int i;

	if (argc > 2) {
	    sprintf(interp->result,
		    "wrong # args: should be \"%.50s flash\"",
		    argv[0]);
	    goto error;
	}
	if (butPtr->state != tkDisabledUid) {
	    for (i = 0; i < 4; i++) {
		butPtr->state = (butPtr->state == tkNormalUid)
			? tkActiveUid : tkNormalUid;
		Tk_SetBackgroundFromBorder(butPtr->tkwin,
			(butPtr->state == tkActiveUid) ? butPtr->activeBorder
			: butPtr->normalBorder);
		DisplayButton((ClientData) butPtr);
		XFlush(Tk_Display(butPtr->tkwin));
		Tk_Sleep(50);
	    }
	}
    } else if ((c == 'i') && (strncmp(argv[1], "invoke", length) == 0)
	    && (butPtr->type > TYPE_LABEL)) {
	if (argc > 2) {
	    sprintf(interp->result,
		    "wrong # args: should be \"%.50s invoke\"",
		    argv[0]);
	    goto error;
	}
	if (butPtr->state != tkDisabledUid) {
	    result = InvokeButton(butPtr);
	}
    } else if ((c == 's') && (strncmp(argv[1], "select", length) == 0)
	    && (butPtr->type >= TYPE_CHECK_BUTTON)) {
	if (argc > 2) {
	    sprintf(interp->result,
		    "wrong # args: should be \"%.50s select\"",
		    argv[0]);
	    goto error;
	}
	Tcl_SetVar(interp, butPtr->selVarName, butPtr->onValue, TCL_GLOBAL_ONLY);
    } else if ((c == 't') && (strncmp(argv[1], "toggle", length) == 0)
	    && (length >= 2) && (butPtr->type == TYPE_CHECK_BUTTON)) {
	if (argc > 2) {
	    sprintf(interp->result,
		    "wrong # args: should be \"%.50s select\"",
		    argv[0]);
	    goto error;
	}
	if (butPtr->flags & SELECTED) {
	    Tcl_SetVar(interp, butPtr->selVarName, butPtr->offValue, TCL_GLOBAL_ONLY);
	} else {
	    Tcl_SetVar(interp, butPtr->selVarName, butPtr->onValue, TCL_GLOBAL_ONLY);
	}
    } else {
	sprintf(interp->result,
		"bad option \"%.50s\":  must be %s", argv[1],
		optionStrings[butPtr->type]);
	goto error;
    }
    Tk_Release((ClientData) butPtr);
    return result;

    redisplay:
    if (Tk_IsMapped(butPtr->tkwin) && !(butPtr->flags & REDRAW_PENDING)) {
//Tk_TimerToken last = butPtr->updateTimerToken;
	butPtr->flags |= REDRAW_PENDING;
//	Tk_DoWhenIdle(DisplayButton, (ClientData) butPtr);
	assert(butPtr->updateTimerToken == 0);
	if (butPtr->updateTimerToken == 0) {
	  butPtr->updateTimerToken =
	    Tk_CreateTimerHandler(
	      ButtonUpdateTime,
	      DisplayButton,
	      (ClientData) butPtr);
	}
//fprintf(stderr, "ButtonWidgetCmd Set Timer %s %s was %d now %d\n", Tk_Class(butPtr->tkwin), Tk_PathName(butPtr->tkwin), last, butPtr->updateTimerToken);
    }
    Tk_Release((ClientData) butPtr);
    return TCL_OK;

    error:
    Tk_Release((ClientData) butPtr);
    return TCL_ERROR;
}

/*
 *----------------------------------------------------------------------
 *
 * DestroyButton --
 *
 *	This procedure is invoked by Tk_EventuallyFree or Tk_Release
 *	to clean up the internal structure of a button at a safe time
 *	(when no-one is using it anymore).
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Everything associated with the widget is freed up.
 *
 *----------------------------------------------------------------------
 */

static void
DestroyButton(clientData)
    ClientData clientData;		/* Info about entry widget. */
{
    register Button *butPtr = (Button *) clientData;

    if (butPtr->text != NULL) {
	ckfree(butPtr->text);
    }
    if (butPtr->textVarName != NULL) {
	Tcl_UntraceVar(butPtr->interp, butPtr->textVarName,
		TCL_GLOBAL_ONLY|TCL_TRACE_WRITES|TCL_TRACE_UNSETS,
		ButtonTextVarProc, (ClientData) butPtr);
	ckfree(butPtr->textVarName);
    }
    if (butPtr->bitmap != None) {
#if defined(USE_XPM3)
      Tk_FreePixmap(butPtr->bitmap);
#else
      Tk_FreeBitmap(butPtr->bitmap);
#endif
    }
    if (butPtr->normalBorder != NULL) {
	Tk_Free3DBorder(butPtr->normalBorder);
    }
    if (butPtr->activeBorder != NULL) {
	Tk_Free3DBorder(butPtr->activeBorder);
    }
    if (butPtr->fontPtr != NULL) {
	Tk_FreeFontStruct(butPtr->fontPtr);
    }
    if (butPtr->normalFg != NULL) {
	Tk_FreeColor(butPtr->normalFg);
    }
    if (butPtr->disabledFg != NULL) {
	Tk_FreeColor(butPtr->disabledFg);
    }
    if (butPtr->activeFg != NULL) {
	Tk_FreeColor(butPtr->activeFg);
    }
    if (butPtr->normalTextGC != None) {
	Tk_FreeGC(butPtr->normalTextGC);
    }
    if (butPtr->activeTextGC != None) {
	Tk_FreeGC(butPtr->activeTextGC);
    }
    if (butPtr->gray != None) {
	Tk_FreeBitmap(butPtr->gray);
    }
    if (butPtr->disabledGC != None) {
	Tk_FreeGC(butPtr->disabledGC);
    }
    if (butPtr->selectorFg != NULL) {
	Tk_FreeColor(butPtr->selectorFg);
    }
    if (butPtr->selectorGC != None) {
	Tk_FreeGC(butPtr->selectorGC);
    }
    if (butPtr->selVarName != NULL) {
	Tcl_UntraceVar(butPtr->interp, butPtr->selVarName,
		TCL_GLOBAL_ONLY|TCL_TRACE_WRITES|TCL_TRACE_UNSETS,
		ButtonVarProc, (ClientData) butPtr);
	ckfree(butPtr->selVarName);
    }
    if (butPtr->onValue != NULL) {
	ckfree(butPtr->onValue);
    }
    if (butPtr->offValue != NULL) {
	ckfree(butPtr->offValue);
    }
    if (butPtr->cursor != None) {
	Tk_FreeCursor(butPtr->cursor);
    }
    if (butPtr->command != NULL) {
	ckfree(butPtr->command);
    }
    if (butPtr->updateTimerToken != NULL) {
	Tk_DeleteTimerHandler(butPtr->updateTimerToken);
//fprintf(stderr, "DestroyButton Delete Timer was %d now 0\n", butPtr->updateTimerToken);
	butPtr->updateTimerToken = 0;

    }
    ckfree((char *) butPtr);
}

/*
 *----------------------------------------------------------------------
 *
 * ConfigureButton --
 *
 *	This procedure is called to process an argv/argc list, plus
 *	the Tk option database, in order to configure (or
 *	reconfigure) a button widget.
 *
 * Results:
 *	The return value is a standard Tcl result.  If TCL_ERROR is
 *	returned, then interp->result contains an error message.
 *
 * Side effects:
 *	Configuration information, such as text string, colors, font,
 *	etc. get set for butPtr;  old resources get freed, if there
 *	were any.  The button is redisplayed.
 *
 *----------------------------------------------------------------------
 */

static int
ConfigureButton(interp, butPtr, argc, argv, flags)
    Tcl_Interp *interp;		/* Used for error reporting. */
    register Button *butPtr;	/* Information about widget;  may or may
				 * not already have values for some fields. */
    int argc;			/* Number of valid entries in argv. */
    char **argv;		/* Arguments. */
    int flags;			/* Flags to pass to Tk_ConfigureWidget. */
{
    XGCValues gcValues;
    GC newGC;
    unsigned long mask;

    /*
     * Eliminate any existing trace on variables monitored by the button.
     */

    if (butPtr->textVarName != NULL) {
	Tcl_UntraceVar(interp, butPtr->textVarName, 
		TCL_GLOBAL_ONLY|TCL_TRACE_WRITES|TCL_TRACE_UNSETS,
		ButtonTextVarProc, (ClientData) butPtr);
    }
    if (butPtr->selVarName != NULL) {
	Tcl_UntraceVar(interp, butPtr->selVarName, 
		TCL_GLOBAL_ONLY|TCL_TRACE_WRITES|TCL_TRACE_UNSETS,
		ButtonVarProc, (ClientData) butPtr);
    }

    if (Tk_ConfigureWidget(interp, butPtr->tkwin, configSpecs,
	    argc, argv, (char *) butPtr, flags) != TCL_OK) {
	return TCL_ERROR;
    }

    /*
     * A few options need special processing, such as setting the
     * background from a 3-D border, or filling in complicated
     * defaults that couldn't be specified to Tk_ConfigureWidget.
     */

    if (butPtr->state == tkActiveUid) {
	Tk_SetBackgroundFromBorder(butPtr->tkwin, butPtr->activeBorder);
    } else {
	Tk_SetBackgroundFromBorder(butPtr->tkwin, butPtr->normalBorder);
	if ((butPtr->state != tkNormalUid)
		&& (butPtr->state != tkDisabledUid)) {
	    Tcl_AppendResult(interp, "bad state value \"", butPtr->state,
		    "\":  must be normal, active, or disabled", (char *) NULL);
	    butPtr->state = tkNormalUid;
	    return TCL_ERROR;
	}
    }

    gcValues.font = butPtr->fontPtr->fid;
    gcValues.foreground = butPtr->normalFg->pixel;
    gcValues.background = Tk_3DBorderColor(butPtr->normalBorder)->pixel;

    /*
     * Note: GraphicsExpose events are disabled in normalTextGC because it's
     * used to copy stuff from an off-screen pixmap onto the screen (we know
     * that there's no problem with obscured areas).
     */

    gcValues.graphics_exposures = False;
    newGC = Tk_GetGC(butPtr->tkwin,
	    GCForeground|GCBackground|GCFont|GCGraphicsExposures, &gcValues);
    if (butPtr->normalTextGC != None) {
	Tk_FreeGC(butPtr->normalTextGC);
    }
    butPtr->normalTextGC = newGC;

    if (butPtr->activeFg != NULL) {
	gcValues.font = butPtr->fontPtr->fid;
	gcValues.foreground = butPtr->activeFg->pixel;
	gcValues.background = Tk_3DBorderColor(butPtr->activeBorder)->pixel;
	newGC = Tk_GetGC(butPtr->tkwin, GCForeground|GCBackground|GCFont,
		&gcValues);
	if (butPtr->activeTextGC != None) {
	    Tk_FreeGC(butPtr->activeTextGC);
	}
	butPtr->activeTextGC = newGC;
    }

    gcValues.font = butPtr->fontPtr->fid;
    gcValues.background = Tk_3DBorderColor(butPtr->normalBorder)->pixel;
    if (butPtr->disabledFg != NULL) {
	gcValues.foreground = butPtr->disabledFg->pixel;
	mask = GCForeground|GCBackground|GCFont;
    } else {
	gcValues.foreground = gcValues.background;
	if (butPtr->gray == None) {
	    butPtr->gray = Tk_GetBitmap(interp, butPtr->tkwin,
		    Tk_GetUid("gray50"));
	    if (butPtr->gray == None) {
		return TCL_ERROR;
	    }
	}
	gcValues.fill_style = FillStippled;
	gcValues.stipple = butPtr->gray;
	mask = GCForeground|GCFillStyle|GCStipple;
    }
    newGC = Tk_GetGC(butPtr->tkwin, mask, &gcValues);
    if (butPtr->disabledGC != None) {
	Tk_FreeGC(butPtr->disabledGC);
    }
    butPtr->disabledGC = newGC;

    if (butPtr->padX < 0) {
	butPtr->padX = 0;
    }
    if (butPtr->padY < 0) {
	butPtr->padY = 0;
    }

    if (butPtr->type >= TYPE_CHECK_BUTTON) {
	char *value;

	if (butPtr->selectorFg != NULL) {
	    gcValues.foreground = butPtr->selectorFg->pixel;
	    newGC = Tk_GetGC(butPtr->tkwin, GCForeground, &gcValues);
	} else {
	    newGC = None;
	}
	if (butPtr->selectorGC != None) {
	    Tk_FreeGC(butPtr->selectorGC);
	}
	butPtr->selectorGC = newGC;

	if (butPtr->selVarName == NULL) {
	    butPtr->selVarName = (char *) ckalloc((unsigned)
		    (strlen(Tk_Name(butPtr->tkwin)) + 1));
	    strcpy(butPtr->selVarName, Tk_Name(butPtr->tkwin));
	}
	if (butPtr->onValue == NULL) {
	    butPtr->onValue = (char *) ckalloc((unsigned)
		    (strlen(Tk_Name(butPtr->tkwin)) + 1));
	    strcpy(butPtr->onValue, Tk_Name(butPtr->tkwin));
	}

	/*
	 * Select the button if the associated variable has the
	 * appropriate value, initialize the variable if it doesn't
	 * exist, then set a trace on the variable to monitor future
	 * changes to its value.
	 */

	value = Tcl_GetVar(interp, butPtr->selVarName, TCL_GLOBAL_ONLY);
	butPtr->flags &= ~SELECTED;
	if (value != NULL) {
	    if (strcmp(value, butPtr->onValue) == 0) {
		butPtr->flags |= SELECTED;
	    }
	} else {
	    Tcl_SetVar(interp, butPtr->selVarName,
		    (butPtr->type == TYPE_CHECK_BUTTON) ? butPtr->offValue : "",
		    TCL_GLOBAL_ONLY);
	}
	Tcl_TraceVar(interp, butPtr->selVarName,
		TCL_GLOBAL_ONLY|TCL_TRACE_WRITES|TCL_TRACE_UNSETS,
		ButtonVarProc, (ClientData) butPtr);
    }

    /*
     * If the button is to display the value of a variable, then set up
     * a trace on the variable's value, create the variable if it doesn't
     * exist, and fetch its current value.
     */

    if ((butPtr->bitmap == None) && (butPtr->textVarName != NULL)) {
	char *value;

	value = Tcl_GetVar(interp, butPtr->textVarName, TCL_GLOBAL_ONLY);
	if (value == NULL) {
	    Tcl_SetVar(interp, butPtr->textVarName, butPtr->text,
		    TCL_GLOBAL_ONLY);
	} else {
	    if (butPtr->text != NULL) {
		ckfree(butPtr->text);
	    }
	    butPtr->text = ckalloc((unsigned) (strlen(value) + 1));
	    strcpy(butPtr->text, value);
	}
	Tcl_TraceVar(interp, butPtr->textVarName,
		TCL_GLOBAL_ONLY|TCL_TRACE_WRITES|TCL_TRACE_UNSETS,
		ButtonTextVarProc, (ClientData) butPtr);
    }

    ComputeButtonGeometry(butPtr);

    /*
     * Lastly, arrange for the button to be redisplayed.
     */

    if (Tk_IsMapped(butPtr->tkwin) && !(butPtr->flags & REDRAW_PENDING)) {
//Tk_TimerToken last = butPtr->updateTimerToken;
	butPtr->flags |= REDRAW_PENDING;
//	Tk_DoWhenIdle(DisplayButton, (ClientData) butPtr);
	assert(butPtr->updateTimerToken == 0);
	if (butPtr->updateTimerToken == 0) {
	  butPtr->updateTimerToken =
	    Tk_CreateTimerHandler(
	      ButtonUpdateTime,
	      DisplayButton,
	      (ClientData) butPtr);
	}
//fprintf(stderr, "ConfigureButton Set Timer %s %s was %d now %d\n", Tk_Class(butPtr->tkwin), Tk_PathName(butPtr->tkwin), last, butPtr->updateTimerToken);
    }

    return TCL_OK;
}

/*
 *----------------------------------------------------------------------
 *
 * DisplayButton --
 *
 *	This procedure is invoked to display a button widget.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Commands are output to X to display the button in its
 *	current mode.
 *
 *----------------------------------------------------------------------
 */

static void
DisplayButton(clientData)
    ClientData clientData;	/* Information about widget. */
{
    register Button *butPtr = (Button *) clientData;
    GC gc;
    Tk_3DBorder border;
    Pixmap pixmap;
    int x = 0;			/* Initialization only needed to stop
				 * compiler warning. */
    int y;
    register Tk_Window tkwin = butPtr->tkwin;

//fprintf(stderr, "DisplayButton Handled Timer %s %s was %d now 0\n", Tk_Class(butPtr->tkwin), Tk_PathName(butPtr->tkwin), butPtr->updateTimerToken);

    assert(butPtr->updateTimerToken != 0);
    butPtr->updateTimerToken = 0;

    butPtr->flags &= ~REDRAW_PENDING;
    if ((butPtr->tkwin == NULL) || !Tk_IsMapped(tkwin)) {
	return;
    }

    if ((butPtr->state == tkDisabledUid) && (butPtr->disabledFg != NULL)) {
	gc = butPtr->disabledGC;
	border = butPtr->normalBorder;
    } else if (butPtr->state == tkActiveUid) {
	gc = butPtr->activeTextGC;
	border = butPtr->activeBorder;
    } else {
	gc = butPtr->normalTextGC;
	border = butPtr->normalBorder;
    }

    /*
     * In order to avoid screen flashes, this procedure redraws
     * the button in a pixmap, then copies the pixmap to the
     * screen in a single operation.  This means that there's no
     * point in time where the on-sreen image has been cleared.
     */

    pixmap = XCreatePixmap(Tk_Display(tkwin), Tk_WindowId(tkwin),
	    Tk_Width(tkwin), Tk_Height(tkwin),
	    Tk_DefaultDepth(Tk_Screen(tkwin)));
    Tk_Fill3DRectangle(Tk_Display(tkwin), pixmap, border,
	    0, 0, Tk_Width(tkwin), Tk_Height(tkwin), 0, TK_RELIEF_FLAT);

    /*
     * Display bitmap or text for button.
     */

    if (butPtr->bitmap != None) {
	unsigned int width, height;

#if defined(USE_XPM3)
	Tk_SizeOfPixmap(butPtr->bitmap, &width, &height);
#else
	Tk_SizeOfBitmap(butPtr->bitmap, &width, &height);
#endif
	switch (butPtr->anchor) {
	    case TK_ANCHOR_NW: case TK_ANCHOR_W: case TK_ANCHOR_SW:
		x = butPtr->borderWidth + butPtr->selectorSpace
			+ butPtr->padX + 1;
		break;
	    case TK_ANCHOR_N: case TK_ANCHOR_CENTER: case TK_ANCHOR_S:
		x = (Tk_Width(tkwin) + butPtr->selectorSpace - width)/2;
		break;
	    default:
		x = Tk_Width(tkwin) - butPtr->borderWidth - butPtr->padX
			- width - 1;
		break;
	}
	switch (butPtr->anchor) {
	    case TK_ANCHOR_NW: case TK_ANCHOR_N: case TK_ANCHOR_NE:
		y = butPtr->borderWidth + butPtr->padY + 1;
		break;
	    case TK_ANCHOR_W: case TK_ANCHOR_CENTER: case TK_ANCHOR_E:
		y = (Tk_Height(tkwin) - height)/2;
		break;
	    default:
		y = Tk_Height(tkwin) - butPtr->borderWidth - butPtr->padY
			- height - 1;
		break;
	}
	if (butPtr->relief == TK_RELIEF_RAISED) {
	    x -= 1;
	    y -= 1;
	} else if (butPtr->relief == TK_RELIEF_SUNKEN) {
	    x += 1;
	    y += 1;
	}
#if defined(USE_XPM3)        
        XCopyArea(Tk_Display(tkwin), butPtr->bitmap, pixmap,
                gc, 0, 0, width, height, x, y);
#else
	XCopyPlane(Tk_Display(tkwin), butPtr->bitmap, pixmap,
		gc, 0, 0, width, height, x, y, 1);
#endif
	y += height/2;
    } else {
	switch (butPtr->anchor) {
	    case TK_ANCHOR_NW: case TK_ANCHOR_W: case TK_ANCHOR_SW:
		x = butPtr->borderWidth + butPtr->padX + butPtr->selectorSpace
			+ butPtr->leftBearing + 1;
		break;
	    case TK_ANCHOR_N: case TK_ANCHOR_CENTER: case TK_ANCHOR_S:
		x = (Tk_Width(tkwin) + butPtr->selectorSpace
			+ butPtr->leftBearing - butPtr->rightBearing)/2;
		break;
	    default:
		x = Tk_Width(tkwin) - butPtr->borderWidth - butPtr->padX
			- butPtr->rightBearing - 1;
		break;
	}
	switch (butPtr->anchor) {
	    case TK_ANCHOR_NW: case TK_ANCHOR_N: case TK_ANCHOR_NE:
		y = butPtr->borderWidth + butPtr->fontPtr->ascent
			+ butPtr->padY + 1;
		break;
	    case TK_ANCHOR_W: case TK_ANCHOR_CENTER: case TK_ANCHOR_E:
		y = (Tk_Height(tkwin) + butPtr->fontPtr->ascent
			- butPtr->fontPtr->descent)/2;
		break;
	    default:
		y = Tk_Height(tkwin) - butPtr->borderWidth - butPtr->padY
			- butPtr->fontPtr->descent - 1;
		break;
	}
	if (butPtr->relief == TK_RELIEF_RAISED) {
	    x -= 1;
	    y -= 1;
	} else if (butPtr->relief == TK_RELIEF_SUNKEN) {
	    x += 1;
	    y += 1;
	}
	XDrawString(Tk_Display(tkwin), pixmap, gc, x, y,
		butPtr->text, butPtr->textLength);
	y -= (butPtr->fontPtr->ascent - butPtr->fontPtr->descent)/2;
	x -= butPtr->leftBearing;
    }

    /*
     * Draw the selector for check buttons and radio buttons.  At this
     * point x and y refer to the top-left corner of the text or bitmap.
     */

    if ((butPtr->type == TYPE_CHECK_BUTTON) && (butPtr->selectorGC != None)) {
	int dim;

	dim = butPtr->selectorDiameter;
	x -= (butPtr->selectorSpace + butPtr->padX + dim)/2;
	y -= dim/2;
	Tk_Draw3DRectangle(Tk_Display(tkwin), pixmap, border, x, y,
		dim, dim, butPtr->borderWidth, TK_RELIEF_SUNKEN);
	x += butPtr->borderWidth;
	y += butPtr->borderWidth;
	dim -= 2*butPtr->borderWidth;
	if (dim > 0) {
	    if (butPtr->flags & SELECTED) {
		XFillRectangle(Tk_Display(tkwin), pixmap, butPtr->selectorGC,
			x, y, (unsigned int) dim, (unsigned int) dim);
	    } else {
		Tk_Fill3DRectangle(Tk_Display(tkwin), pixmap,
			butPtr->normalBorder, x, y, dim, dim,
			butPtr->borderWidth, TK_RELIEF_FLAT);
	    }
	}
    } else if ((butPtr->type == TYPE_RADIO_BUTTON)
	    && (butPtr->selectorGC != None)) {
	XPoint points[4];
	int radius;

	radius = butPtr->selectorDiameter/2;
	points[0].x = x - (butPtr->selectorSpace + butPtr->padX
		+ butPtr->selectorDiameter)/2;
	points[0].y = y;
	points[1].x = points[0].x + radius;
	points[1].y = points[0].y + radius;
	points[2].x = points[1].x + radius;
	points[2].y = points[0].y;
	points[3].x = points[1].x;
	points[3].y = points[0].y - radius;
	if (butPtr->flags & SELECTED) {
	    XFillPolygon(Tk_Display(tkwin), pixmap, butPtr->selectorGC,
		    points, 4, Convex, CoordModeOrigin);
	} else {
	    Tk_Fill3DPolygon(Tk_Display(tkwin), pixmap, butPtr->normalBorder,
		    points, 4, butPtr->borderWidth, TK_RELIEF_FLAT);
	}
	Tk_Draw3DPolygon(Tk_Display(tkwin), pixmap, border,
		points, 4, butPtr->borderWidth, TK_RELIEF_RAISED);
    }

    /*
     * If the button is disabled with a stipple rather than a special
     * foreground color, generate the stippled effect.
     */

    if ((butPtr->state == tkDisabledUid) && (butPtr->disabledFg == NULL)) {
	XFillRectangle(Tk_Display(tkwin), pixmap, butPtr->disabledGC,
		butPtr->borderWidth, butPtr->borderWidth,
		(unsigned) (Tk_Width(tkwin) - 2*butPtr->borderWidth),
		(unsigned) (Tk_Height(tkwin) - 2*butPtr->borderWidth));
    }

    /*
     * Draw the border last.  This way, if the button's contents
     * overflow onto the border they'll be covered up by the border.
     */

    if (butPtr->relief != TK_RELIEF_FLAT) {
	Tk_Draw3DRectangle(Tk_Display(tkwin), pixmap, border,0, 0,
		Tk_Width(tkwin), Tk_Height(tkwin), butPtr->borderWidth,
		butPtr->relief);
    }

    /*
     * Copy the information from the off-screen pixmap onto the screen,
     * then delete the pixmap.
     */

    XCopyArea(Tk_Display(tkwin), pixmap, Tk_WindowId(tkwin),
	butPtr->normalTextGC, 0, 0, Tk_Width(tkwin), Tk_Height(tkwin), 0, 0);
    XFreePixmap(Tk_Display(tkwin), pixmap);
}

/*
 *--------------------------------------------------------------
 *
 * ButtonEventProc --
 *
 *	This procedure is invoked by the Tk dispatcher for various
 *	events on buttons.
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
ButtonEventProc(clientData, eventPtr)
    ClientData clientData;	/* Information about window. */
    XEvent *eventPtr;		/* Information about event. */
{
    Button *butPtr = (Button *) clientData;
    if ((eventPtr->type == Expose) && (eventPtr->xexpose.count == 0)) {
	if ((butPtr->tkwin != NULL) && !(butPtr->flags & REDRAW_PENDING)) {
//Tk_TimerToken last = butPtr->updateTimerToken;
	    butPtr->flags |= REDRAW_PENDING;
//	    Tk_DoWhenIdle(DisplayButton, (ClientData) butPtr);
	    assert(butPtr->updateTimerToken == NULL);
	    if (butPtr->updateTimerToken == NULL) {
	      butPtr->updateTimerToken =
		Tk_CreateTimerHandler(
		  ButtonUpdateTime,
		  DisplayButton,
		  (ClientData) butPtr);
	    } // if
//fprintf(stderr, "ButtonEventProc Expose Set Timer %s %s was %d now %d\n", Tk_Class(butPtr->tkwin), Tk_PathName(butPtr->tkwin), last, butPtr->updateTimerToken);
	}
    } else if (eventPtr->type == DestroyNotify) {
	Tcl_DeleteCommand(butPtr->interp, Tk_PathName(butPtr->tkwin));
	butPtr->tkwin = NULL;
	if (butPtr->flags & REDRAW_PENDING) {
//fprintf(stderr, "ButtonEventProc Destroy Timer was %d now 0\n", butPtr->updateTimerToken);
//	    Tk_CancelIdleCall(DisplayButton, (ClientData) butPtr);
	    butPtr->flags &= ~REDRAW_PENDING;
	    assert(butPtr->updateTimerToken != NULL);
	    if (butPtr->updateTimerToken != NULL) {
	        Tk_DeleteTimerHandler(butPtr->updateTimerToken);
	        butPtr->updateTimerToken = 0;
	    }
	}
	Tk_EventuallyFree((ClientData) butPtr, DestroyButton);
    }
}

/*
 *----------------------------------------------------------------------
 *
 * ComputeButtonGeometry --
 *
 *	After changes in a button's text or bitmap, this procedure
 *	recomputes the button's geometry and passes this information
 *	along to the geometry manager for the window.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The button's window may change size.
 *
 *----------------------------------------------------------------------
 */

static void
ComputeButtonGeometry(butPtr)
    register Button *butPtr;	/* Button whose geometry may have changed. */
{
    XCharStruct bbox;
    int dummy;
    unsigned int width, height;

    butPtr->selectorSpace = 0;
    if (butPtr->bitmap != None) {
#if defined(USE_XPM3)
        Tk_SizeOfPixmap(butPtr->bitmap, &width, &height);
#else
        Tk_SizeOfBitmap(butPtr->bitmap, &width, &height);
#endif
	if (butPtr->width > 0) {
	    width = butPtr->width;
	}
	if (butPtr->height > 0) {
	    height = butPtr->height;
	}
	if ((butPtr->type >= TYPE_CHECK_BUTTON)
		&& (butPtr->selectorGC != None)) {
	    butPtr->selectorSpace = (14*height)/10;
	    if (butPtr->type == TYPE_CHECK_BUTTON) {
		butPtr->selectorDiameter = (65*height)/100;
	    } else {
		butPtr->selectorDiameter = (75*height)/100;
	    }
	}
    } else {
	butPtr->textLength = strlen(butPtr->text);
	XTextExtents(butPtr->fontPtr, butPtr->text, butPtr->textLength,
		&dummy, &dummy, &dummy, &bbox);
	butPtr->leftBearing = bbox.lbearing;
	butPtr->rightBearing = bbox.rbearing;
	width = bbox.lbearing + bbox.rbearing;
	height = butPtr->fontPtr->ascent + butPtr->fontPtr->descent;
	if (butPtr->width > 0) {
	    width = butPtr->width * XTextWidth(butPtr->fontPtr, "0", 1);
	}
	if (butPtr->height > 0) {
	    height *= butPtr->height;
	}
	if ((butPtr->type >= TYPE_CHECK_BUTTON)
		&& (butPtr->selectorGC != None)) {
	    butPtr->selectorDiameter = butPtr->fontPtr->ascent
		    + butPtr->fontPtr->descent;
	    if (butPtr->type == TYPE_CHECK_BUTTON) {
		butPtr->selectorDiameter = (80*butPtr->selectorDiameter)/100;
	    }
	    butPtr->selectorSpace = butPtr->selectorDiameter + butPtr->padX;
	}
    }

    /*
     * When issuing the geometry request, add extra space for the selector,
     * if any, and for the border and padding, plus two extra pixels so the
     * display can be offset by 1 pixel in either direction for the raised
     * or lowered effect.
     */

    width += 2*butPtr->padX;
    height += 2*butPtr->padY;
    Tk_GeometryRequest(butPtr->tkwin, (int) (width + butPtr->selectorSpace
	    + 2*butPtr->borderWidth + 2),
	    (int) (height + 2*butPtr->borderWidth + 2));
    Tk_SetInternalBorder(butPtr->tkwin, butPtr->borderWidth);
}

/*
 *----------------------------------------------------------------------
 *
 * InvokeButton --
 *
 *	This procedure is called to carry out the actions associated
 *	with a button, such as invoking a Tcl command or setting a
 *	variable.  This procedure is invoked, for example, when the
 *	button is invoked via the mouse.
 *
 * Results:
 *	A standard Tcl return value.  Information is also left in
 *	interp->result.
 *
 * Side effects:
 *	Depends on the button and its associated command.
 *
 *----------------------------------------------------------------------
 */

static int
InvokeButton(butPtr)
    register Button *butPtr;		/* Information about button. */
{
    if (butPtr->type == TYPE_CHECK_BUTTON) {
	if (butPtr->flags & SELECTED) {
	    Tcl_SetVar(butPtr->interp, butPtr->selVarName, butPtr->offValue,
		    TCL_GLOBAL_ONLY);
	} else {
	    Tcl_SetVar(butPtr->interp, butPtr->selVarName, butPtr->onValue,
		    TCL_GLOBAL_ONLY);
	}
    } else if (butPtr->type == TYPE_RADIO_BUTTON) {
	Tcl_SetVar(butPtr->interp, butPtr->selVarName, butPtr->onValue,
		TCL_GLOBAL_ONLY);
    }
    if ((butPtr->type != TYPE_LABEL) && (butPtr->command != NULL)) {
	return Tcl_GlobalEval(butPtr->interp, butPtr->command);
    }
    return TCL_OK;
}

/*
 *--------------------------------------------------------------
 *
 * ButtonVarProc --
 *
 *	This procedure is invoked when someone changes the
 *	state variable associated with a radio button.  Depending
 *	on the new value of the button's variable, the button
 *	may be selected or deselected.
 *
 * Results:
 *	NULL is always returned.
 *
 * Side effects:
 *	The button may become selected or deselected.
 *
 *--------------------------------------------------------------
 */

	/* ARGSUSED */
static char *
ButtonVarProc(clientData, interp, name1, name2, flags)
    ClientData clientData;	/* Information about button. */
    Tcl_Interp *interp;		/* Interpreter containing variable. */
    char *name1;		/* Name of variable. */
    char *name2;		/* Second part of variable name. */
    int flags;			/* Information about what happened. */
{
    register Button *butPtr = (Button *) clientData;
    char *value;

    /*
     * If the variable is being unset, then just re-establish the
     * trace unless the whole interpreter is going away.
     */

    if (flags & TCL_TRACE_UNSETS) {
	butPtr->flags &= ~SELECTED;
	if ((flags & TCL_TRACE_DESTROYED) && !(flags & TCL_INTERP_DESTROYED)) {
	    Tcl_TraceVar2(interp, name1, name2,
		    TCL_GLOBAL_ONLY|TCL_TRACE_WRITES|TCL_TRACE_UNSETS,
		    ButtonVarProc, clientData);
	}
	goto redisplay;
    }

    /*
     * Use the value of the variable to update the selected status of
     * the button.
     */

    value = Tcl_GetVar2(interp, name1, name2, flags & TCL_GLOBAL_ONLY);
    if (strcmp(value, butPtr->onValue) == 0) {
	if (butPtr->flags & SELECTED) {
	    return (char *) NULL;
	}
	butPtr->flags |= SELECTED;
    } else if (butPtr->flags & SELECTED) {
	butPtr->flags &= ~SELECTED;
    } else {
	return (char *) NULL;
    }

    redisplay:
    if ((butPtr->tkwin != NULL) && Tk_IsMapped(butPtr->tkwin)
	    && !(butPtr->flags & REDRAW_PENDING)) {
//Tk_TimerToken last = butPtr->updateTimerToken;
	butPtr->flags |= REDRAW_PENDING;
//	Tk_DoWhenIdle(DisplayButton, (ClientData) butPtr);
	assert(butPtr->updateTimerToken == NULL);
	if (butPtr->updateTimerToken == NULL) {
	    butPtr->updateTimerToken =
	      Tk_CreateTimerHandler(
		ButtonUpdateTime,
		DisplayButton,
		(ClientData) butPtr);
	}
//fprintf(stderr, "ButtonVarProc Set Timer %s %s was %d now %d\n", Tk_Class(butPtr->tkwin), Tk_PathName(butPtr->tkwin), last, butPtr->updateTimerToken);
    }
    return (char *) NULL;
}

/*
 *--------------------------------------------------------------
 *
 * ButtonTextVarProc --
 *
 *	This procedure is invoked when someone changes the variable
 *	whose contents are to be displayed in a button.
 *
 * Results:
 *	NULL is always returned.
 *
 * Side effects:
 *	The text displayed in the button will change to match the
 *	variable.
 *
 *--------------------------------------------------------------
 */

	/* ARGSUSED */
static char *
ButtonTextVarProc(clientData, interp, name1, name2, flags)
    ClientData clientData;	/* Information about button. */
    Tcl_Interp *interp;		/* Interpreter containing variable. */
    char *name1;		/* Name of variable. */
    char *name2;		/* Second part of variable name. */
    int flags;			/* Information about what happened. */
{
    register Button *butPtr = (Button *) clientData;
    char *value;

    /*
     * If the variable is unset, then immediately recreate it unless
     * the whole interpreter is going away.
     */

    if (flags & TCL_TRACE_UNSETS) {
	if ((flags & TCL_TRACE_DESTROYED) && !(flags & TCL_INTERP_DESTROYED)) {
	    Tcl_SetVar2(interp, name1, name2, butPtr->text,
		    flags & TCL_GLOBAL_ONLY);
	    Tcl_TraceVar2(interp, name1, name2,
		    TCL_GLOBAL_ONLY|TCL_TRACE_WRITES|TCL_TRACE_UNSETS,
		    ButtonTextVarProc, clientData);
	}
	return (char *) NULL;
    }

    value = Tcl_GetVar2(interp, name1, name2, flags & TCL_GLOBAL_ONLY);
    if (value == NULL) {
	value = "";
    }
    if (butPtr->text != NULL) {
	ckfree(butPtr->text);
    }
    butPtr->text = ckalloc((unsigned) (strlen(value) + 1));
    strcpy(butPtr->text, value);
    ComputeButtonGeometry(butPtr);

    if ((butPtr->tkwin != NULL) && Tk_IsMapped(butPtr->tkwin)
	    && !(butPtr->flags & REDRAW_PENDING)) {
//Tk_TimerToken last = butPtr->updateTimerToken;
	butPtr->flags |= REDRAW_PENDING;
//	Tk_DoWhenIdle(DisplayButton, (ClientData) butPtr);
	assert(butPtr->updateTimerToken == NULL);
	if (butPtr->updateTimerToken == NULL) {
	    butPtr->updateTimerToken =
	      Tk_CreateTimerHandler(
		ButtonUpdateTime,
		DisplayButton,
		(ClientData) butPtr);
	}
//fprintf(stderr, "ButtonTextVarProc Set Timer %s %s was %d now %d\n", Tk_Class(butPtr->tkwin), Tk_PathName(butPtr->tkwin), last, butPtr->updateTimerToken);
    }
    return (char *) NULL;
}
