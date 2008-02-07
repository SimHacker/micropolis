/* 
 * tkMessage.c --
 *
 *	This module implements a message widgets for the Tk
 *	toolkit.  A message widget displays a multi-line string
 *	in a window according to a particular aspect ratio.
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
static char rcsid[] = "$Header: /user6/ouster/wish/RCS/tkMessage.c,v 1.36 92/06/08 11:06:05 ouster Exp $ SPRITE (Berkeley)";
#endif

#include "tkconfig.h"
#include "default.h"
#include "tkint.h"

/*
 * A data structure of the following type is kept for each message
 * widget managed by this file:
 */

typedef struct {
    Tk_Window tkwin;		/* Window that embodies the message.  NULL
				 * means that the window has been destroyed
				 * but the data structures haven't yet been
				 * cleaned up.*/
    Tcl_Interp *interp;		/* Interpreter associated with message. */
    Tk_Uid string;		/* String displayed in message. */
    int numChars;		/* Number of characters in string, not
				 * including terminating NULL character. */
    char *textVarName;		/* Name of variable (malloc'ed) or NULL.
				 * If non-NULL, message displays the contents
				 * of this variable. */

    /*
     * Information used when displaying widget:
     */

    Tk_3DBorder border;		/* Structure used to draw 3-D border and
				 * background.  NULL means a border hasn't
				 * been created yet. */
    int borderWidth;		/* Width of border. */
    int relief;			/* 3-D effect: TK_RELIEF_RAISED, etc. */
    XFontStruct *fontPtr;	/* Information about text font, or NULL. */
    XColor *fgColorPtr;		/* Foreground color in normal mode. */
    GC textGC;			/* GC for drawing text in normal mode. */
    int padX, padY;		/* User-requested extra space around text. */
    Tk_Anchor anchor;		/* Where to position text within window region
				 * if window is larger or smaller than
				 * needed. */
    int width;			/* User-requested width, in pixels.  0 means
				 * compute width using aspect ratio below. */
    int aspect;			/* Desired aspect ratio for window
				 * (100*width/height). */
    int lineLength;		/* Length of each line, in pixels.  Computed
				 * from width and/or aspect. */
    int msgHeight;		/* Total number of pixels in vertical direction
				 * needed to display message. */
    Tk_Justify justify;		/* Justification for text. */

    /*
     * Miscellaneous information:
     */

    Cursor cursor;		/* Current cursor for window, or None. */
    int flags;			/* Various flags;  see below for
				 * definitions. */
} Message;

/*
 * Flag bits for messages:
 *
 * REDRAW_PENDING:		Non-zero means a DoWhenIdle handler
 *				has already been queued to redraw
 *				this window.
 * CLEAR_NEEDED;		Need to clear the window when redrawing.
 */

#define REDRAW_PENDING		1
#define CLEAR_NEEDED		2

/*
 * Information used for argv parsing.
 */


static Tk_ConfigSpec configSpecs[] = {
    {TK_CONFIG_ANCHOR, "-anchor", "anchor", "Anchor",
	DEF_MESSAGE_ANCHOR, Tk_Offset(Message, anchor), 0},
    {TK_CONFIG_INT, "-aspect", "aspect", "Aspect",
	DEF_MESSAGE_ASPECT, Tk_Offset(Message, aspect), 0},
    {TK_CONFIG_BORDER, "-background", "background", "Background",
	DEF_MESSAGE_BG_COLOR, Tk_Offset(Message, border),
	TK_CONFIG_COLOR_ONLY},
    {TK_CONFIG_BORDER, "-background", "background", "Background",
	DEF_MESSAGE_BG_MONO, Tk_Offset(Message, border),
	TK_CONFIG_MONO_ONLY},
    {TK_CONFIG_SYNONYM, "-bd", "borderWidth", (char *) NULL,
	(char *) NULL, 0, 0},
    {TK_CONFIG_SYNONYM, "-bg", "background", (char *) NULL,
	(char *) NULL, 0, 0},
    {TK_CONFIG_PIXELS, "-borderwidth", "borderWidth", "BorderWidth",
	DEF_MESSAGE_BORDER_WIDTH, Tk_Offset(Message, borderWidth), 0},
    {TK_CONFIG_ACTIVE_CURSOR, "-cursor", "cursor", "Cursor",
	DEF_MESSAGE_CURSOR, Tk_Offset(Message, cursor), TK_CONFIG_NULL_OK},
    {TK_CONFIG_SYNONYM, "-fg", "foreground", (char *) NULL,
	(char *) NULL, 0, 0},
    {TK_CONFIG_FONT, "-font", "font", "Font",
	DEF_MESSAGE_FONT, Tk_Offset(Message, fontPtr), 0},
    {TK_CONFIG_COLOR, "-foreground", "foreground", "Foreground",
	DEF_MESSAGE_FG, Tk_Offset(Message, fgColorPtr), 0},
    {TK_CONFIG_JUSTIFY, "-justify", "justify", "Justify",
	DEF_MESSAGE_JUSTIFY, Tk_Offset(Message, justify), 0},
    {TK_CONFIG_PIXELS, "-padx", "padX", "Pad",
	DEF_MESSAGE_PADX, Tk_Offset(Message, padX), 0},
    {TK_CONFIG_PIXELS, "-pady", "padY", "Pad",
	DEF_MESSAGE_PADY, Tk_Offset(Message, padY), 0},
    {TK_CONFIG_RELIEF, "-relief", "relief", "Relief",
	DEF_MESSAGE_RELIEF, Tk_Offset(Message, relief), 0},
    {TK_CONFIG_STRING, "-text", "text", "Text",
	DEF_MESSAGE_TEXT, Tk_Offset(Message, string), 0},
    {TK_CONFIG_STRING, "-textvariable", "textVariable", "Variable",
	DEF_MESSAGE_TEXT_VARIABLE, Tk_Offset(Message, textVarName),
	TK_CONFIG_NULL_OK},
    {TK_CONFIG_PIXELS, "-width", "width", "Width",
	DEF_MESSAGE_WIDTH, Tk_Offset(Message, width), 0},
    {TK_CONFIG_END, (char *) NULL, (char *) NULL, (char *) NULL,
	(char *) NULL, 0, 0}
};

/*
 * Forward declarations for procedures defined later in this file:
 */

static void		MessageEventProc _ANSI_ARGS_((ClientData clientData,
			    XEvent *eventPtr));
static char *		MessageTextVarProc _ANSI_ARGS_((ClientData clientData,
			    Tcl_Interp *interp, char *name1, char *name2,
			    int flags));
static int		MessageWidgetCmd _ANSI_ARGS_((ClientData clientData,
			    Tcl_Interp *interp, int argc, char **argv));
static void		ComputeMessageGeometry _ANSI_ARGS_((Message *msgPtr));
static int		ConfigureMessage _ANSI_ARGS_((Tcl_Interp *interp,
			    Message *msgPtr, int argc, char **argv,
			    int flags));
static void		DestroyMessage _ANSI_ARGS_((ClientData clientData));
static void		DisplayMessage _ANSI_ARGS_((ClientData clientData));

/*
 *--------------------------------------------------------------
 *
 * Tk_MessageCmd --
 *
 *	This procedure is invoked to process the "message" Tcl
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
Tk_MessageCmd(clientData, interp, argc, argv)
    ClientData clientData;	/* Main window associated with
				 * interpreter. */
    Tcl_Interp *interp;		/* Current interpreter. */
    int argc;			/* Number of arguments. */
    char **argv;		/* Argument strings. */
{
    register Message *msgPtr;
    Tk_Window new;
    Tk_Window tkwin = (Tk_Window) clientData;

    if (argc < 2) {
	Tcl_AppendResult(interp, "wrong # args:  should be \"",
		argv[0], " pathName ?options?\"", (char *) NULL);
	return TCL_ERROR;
    }

    new = Tk_CreateWindowFromPath(interp, tkwin, argv[1], (char *) NULL);
    if (new == NULL) {
	return TCL_ERROR;
    }

    msgPtr = (Message *) ckalloc(sizeof(Message));
    msgPtr->tkwin = new;
    msgPtr->interp = interp;
    msgPtr->string = NULL;
    msgPtr->textVarName = NULL;
    msgPtr->border = NULL;
    msgPtr->borderWidth = 0;
    msgPtr->relief = TK_RELIEF_FLAT;
    msgPtr->fontPtr = NULL;
    msgPtr->fgColorPtr = NULL;
    msgPtr->textGC = NULL;
    msgPtr->padX = 0;
    msgPtr->padY = 0;
    msgPtr->width = 0;
    msgPtr->aspect = 150;
    msgPtr->justify = TK_JUSTIFY_LEFT;
    msgPtr->cursor = None;
    msgPtr->flags = 0;

    Tk_SetClass(msgPtr->tkwin, "Message");
    Tk_CreateEventHandler(msgPtr->tkwin, ExposureMask|StructureNotifyMask,
	    MessageEventProc, (ClientData) msgPtr);
    Tcl_CreateCommand(interp, Tk_PathName(msgPtr->tkwin), MessageWidgetCmd,
	    (ClientData) msgPtr, (void (*)()) NULL);
    if (ConfigureMessage(interp, msgPtr, argc-2, argv+2, 0) != TCL_OK) {
	goto error;
    }

    interp->result = Tk_PathName(msgPtr->tkwin);
    return TCL_OK;

    error:
    Tk_DestroyWindow(msgPtr->tkwin);
    return TCL_ERROR;
}

/*
 *--------------------------------------------------------------
 *
 * MessageWidgetCmd --
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
MessageWidgetCmd(clientData, interp, argc, argv)
    ClientData clientData;	/* Information about message widget. */
    Tcl_Interp *interp;		/* Current interpreter. */
    int argc;			/* Number of arguments. */
    char **argv;		/* Argument strings. */
{
    register Message *msgPtr = (Message *) clientData;
    int length;
    char c;

    if (argc < 2) {
	Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
		" option ?arg arg ...?\"", (char *) NULL);
	return TCL_ERROR;
    }
    c = argv[1][0];
    length = strlen(argv[1]);
    if ((c == 'c') && (strncmp(argv[1], "configure", length) == 0)) {
	if (argc == 2) {
	    return Tk_ConfigureInfo(interp, msgPtr->tkwin, configSpecs,
		    (char *) msgPtr, (char *) NULL, 0);
	} else if (argc == 3) {
	    return Tk_ConfigureInfo(interp, msgPtr->tkwin, configSpecs,
		    (char *) msgPtr, argv[2], 0);
	} else {
	    return ConfigureMessage(interp, msgPtr, argc-2, argv+2,
		    TK_CONFIG_ARGV_ONLY);
	}
    } else {
	Tcl_AppendResult(interp, "bad option \"", argv[1],
		"\":  must be configure", (char *) NULL);
	return TCL_ERROR;
    }
}

/*
 *----------------------------------------------------------------------
 *
 * DestroyMessage --
 *
 *	This procedure is invoked by Tk_EventuallyFree or Tk_Release
 *	to clean up the internal structure of a message at a safe time
 *	(when no-one is using it anymore).
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Everything associated with the message is freed up.
 *
 *----------------------------------------------------------------------
 */

static void
DestroyMessage(clientData)
    ClientData clientData;	/* Info about message widget. */
{
    register Message *msgPtr = (Message *) clientData;

    if (msgPtr->string != NULL) {
	ckfree(msgPtr->string);
    }
    if (msgPtr->textVarName != NULL) {
	Tcl_UntraceVar(msgPtr->interp, msgPtr->textVarName,
		TCL_GLOBAL_ONLY|TCL_TRACE_WRITES|TCL_TRACE_UNSETS,
		MessageTextVarProc, (ClientData) msgPtr);
	ckfree(msgPtr->textVarName);
    }
    if (msgPtr->border != NULL) {
	Tk_Free3DBorder(msgPtr->border);
    }
    if (msgPtr->fontPtr != NULL) {
	Tk_FreeFontStruct(msgPtr->fontPtr);
    }
    if (msgPtr->fgColorPtr != NULL) {
	Tk_FreeColor(msgPtr->fgColorPtr);
    }
    if (msgPtr->textGC != None) {
	Tk_FreeGC(msgPtr->textGC);
    }
    if (msgPtr->cursor != None) {
	Tk_FreeCursor(msgPtr->cursor);
    }
    ckfree((char *) msgPtr);
}

/*
 *----------------------------------------------------------------------
 *
 * ConfigureMessage --
 *
 *	This procedure is called to process an argv/argc list, plus
 *	the Tk option database, in order to configure (or
 *	reconfigure) a message widget.
 *
 * Results:
 *	The return value is a standard Tcl result.  If TCL_ERROR is
 *	returned, then interp->result contains an error message.
 *
 * Side effects:
 *	Configuration information, such as text string, colors, font,
 *	etc. get set for msgPtr;  old resources get freed, if there
 *	were any.
 *
 *----------------------------------------------------------------------
 */

static int
ConfigureMessage(interp, msgPtr, argc, argv, flags)
    Tcl_Interp *interp;		/* Used for error reporting. */
    register Message *msgPtr;	/* Information about widget;  may or may
				 * not already have values for some fields. */
    int argc;			/* Number of valid entries in argv. */
    char **argv;		/* Arguments. */
    int flags;			/* Flags to pass to Tk_ConfigureWidget. */
{
    XGCValues gcValues;
    GC newGC;

    /*
     * Eliminate any existing trace on a variable monitored by the message.
     */

    if (msgPtr->textVarName != NULL) {
	Tcl_UntraceVar(interp, msgPtr->textVarName, 
		TCL_GLOBAL_ONLY|TCL_TRACE_WRITES|TCL_TRACE_UNSETS,
		MessageTextVarProc, (ClientData) msgPtr);
    }

    if (Tk_ConfigureWidget(interp, msgPtr->tkwin, configSpecs,
	    argc, argv, (char *) msgPtr, flags) != TCL_OK) {
	return TCL_ERROR;
    }

    /*
     * If the message is to display the value of a variable, then set up
     * a trace on the variable's value, create the variable if it doesn't
     * exist, and fetch its current value.
     */

    if (msgPtr->textVarName != NULL) {
	char *value;

	value = Tcl_GetVar(interp, msgPtr->textVarName, TCL_GLOBAL_ONLY);
	if (value == NULL) {
	    Tcl_SetVar(interp, msgPtr->textVarName, msgPtr->string,
		    TCL_GLOBAL_ONLY);
	} else {
	    if (msgPtr->string != NULL) {
		ckfree(msgPtr->string);
	    }
	    msgPtr->string = ckalloc((unsigned) (strlen(value) + 1));
	    strcpy(msgPtr->string, value);
	}
	Tcl_TraceVar(interp, msgPtr->textVarName,
		TCL_GLOBAL_ONLY|TCL_TRACE_WRITES|TCL_TRACE_UNSETS,
		MessageTextVarProc, (ClientData) msgPtr);
    }

    /*
     * A few other options need special processing, such as setting
     * the background from a 3-D border or handling special defaults
     * that couldn't be specified to Tk_ConfigureWidget.
     */

    msgPtr->numChars = strlen(msgPtr->string);

    Tk_SetBackgroundFromBorder(msgPtr->tkwin, msgPtr->border);

    gcValues.font = msgPtr->fontPtr->fid;
    gcValues.foreground = msgPtr->fgColorPtr->pixel;
    newGC = Tk_GetGC(msgPtr->tkwin, GCForeground|GCFont,
	    &gcValues);
    if (msgPtr->textGC != None) {
	Tk_FreeGC(msgPtr->textGC);
    }
    msgPtr->textGC = newGC;

    if (msgPtr->padX == -1) {
	msgPtr->padX = msgPtr->fontPtr->ascent/2;
    }

    if (msgPtr->padY == -1) {
	msgPtr->padY = msgPtr->fontPtr->ascent/4;
    }

    if (msgPtr->justify == TK_JUSTIFY_FILL) {
	interp->result = "can't use \"fill\" justify style in messages";
	return TCL_ERROR;
    }

    /*
     * Recompute the desired geometry for the window, and arrange for
     * the window to be redisplayed.
     */

    ComputeMessageGeometry(msgPtr);
    if ((msgPtr->tkwin != NULL) && Tk_IsMapped(msgPtr->tkwin)
	    && !(msgPtr->flags & REDRAW_PENDING)) {
	Tk_DoWhenIdle(DisplayMessage, (ClientData) msgPtr);
	msgPtr->flags |= REDRAW_PENDING|CLEAR_NEEDED;
    }

    return TCL_OK;
}

/*
 *--------------------------------------------------------------
 *
 * ComputeMessageGeometry --
 *
 *	Compute the desired geometry for a message window,
 *	taking into account the desired aspect ratio for the
 *	window.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Tk_GeometryRequest is called to inform the geometry
 *	manager of the desired geometry for this window.
 *
 *--------------------------------------------------------------
 */

static void
ComputeMessageGeometry(msgPtr)
    register Message *msgPtr;	/* Information about window. */
{
    char *p;
    int width, inc, height, numLines;
    int thisWidth, maxWidth;
    int aspect, lowerBound, upperBound;

    /*
     * Compute acceptable bounds for the final aspect ratio.
     */
    aspect = msgPtr->aspect/10;
    if (aspect < 5) {
	aspect = 5;
    }
    lowerBound = msgPtr->aspect - aspect;
    upperBound = msgPtr->aspect + aspect;

    /*
     * Do the computation in multiple passes:  start off with
     * a very wide window, and compute its height.  Then change
     * the width and try again.  Reduce the size of the change
     * and iterate until dimensions are found that approximate
     * the desired aspect ratio.  Or, if the user gave an explicit
     * width then just use that.
     */

    if (msgPtr->width > 0) {
	width = msgPtr->width;
	inc = 0;
    } else {
	width = WidthOfScreen(Tk_Screen(msgPtr->tkwin))/2;
	inc = width/2;
    }
    for ( ; ; inc /= 2) {
	maxWidth = 0;
	for (numLines = 1, p = msgPtr->string; ; numLines++)  {
	    if (*p == '\n') {
		p++;
		continue;
	    }
	    p += TkMeasureChars(msgPtr->fontPtr, p,
		    msgPtr->numChars - (p - msgPtr->string), 0, width,
		    TK_WHOLE_WORDS|TK_AT_LEAST_ONE, &thisWidth);
	    if (thisWidth > maxWidth) {
		maxWidth = thisWidth;
	    }
	    if (*p == 0) {
		break;
	    }

	    /*
	     * Skip spaces and tabs at the beginning of a line, unless
	     * they follow a user-requested newline.
	     */

	    while (isspace(*p)) {
		if (*p == '\n') {
		    p++;
		    break;
		}
		p++;
	    }
	}

	height = numLines * (msgPtr->fontPtr->ascent
		+ msgPtr->fontPtr->descent) + 2*msgPtr->borderWidth
		+ 2*msgPtr->padY;
	if (inc <= 2) {
	    break;
	}
	aspect = (100*(maxWidth + 2*msgPtr->borderWidth
		+ 2*msgPtr->padX))/height;
	if (aspect < lowerBound) {
	    width += inc;
	} else if (aspect > upperBound) {
	    width -= inc;
	} else {
	    break;
	}
    }
    msgPtr->lineLength = maxWidth;
    msgPtr->msgHeight = numLines * (msgPtr->fontPtr->ascent
		+ msgPtr->fontPtr->descent);
    Tk_GeometryRequest(msgPtr->tkwin,
	    maxWidth + 2*msgPtr->borderWidth + 2*msgPtr->padX, height);
    Tk_SetInternalBorder(msgPtr->tkwin, msgPtr->borderWidth);
}

/*
 *--------------------------------------------------------------
 *
 * DisplayMessage --
 *
 *	This procedure redraws the contents of a message window.
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
DisplayMessage(clientData)
    ClientData clientData;	/* Information about window. */
{
    register Message *msgPtr = (Message *) clientData;
    register Tk_Window tkwin = msgPtr->tkwin;
    char *p;
    int x, y, lineLength, numChars, charsLeft;

    msgPtr->flags &= ~REDRAW_PENDING;
    if ((msgPtr->tkwin == NULL) || !Tk_IsMapped(tkwin)) {
	return;
    }
    if (msgPtr->flags & CLEAR_NEEDED) {
	XClearWindow(Tk_Display(tkwin), Tk_WindowId(tkwin));
	msgPtr->flags &= ~CLEAR_NEEDED;
    }

    /*
     * Compute starting y-location for message based on message size
     * and anchor option.
     */

    switch (msgPtr->anchor) {
	case TK_ANCHOR_NW: case TK_ANCHOR_N: case TK_ANCHOR_NE:
	    y = msgPtr->borderWidth + msgPtr->padY;
	    break;
	case TK_ANCHOR_W: case TK_ANCHOR_CENTER: case TK_ANCHOR_E:
	    y = (Tk_Height(tkwin) - msgPtr->msgHeight)/2;
	    break;
	default:
	    y = Tk_Height(tkwin) - msgPtr->borderWidth - msgPtr->padY
		    - msgPtr->msgHeight;
	    break;
    }
    y += msgPtr->fontPtr->ascent;

    /*
     * Work through the string to display one line at a time.
     * Display each line in three steps.  First compute the
     * line's width, then figure out where to display the
     * line to justify it properly, then display the line.
     */

    for (p = msgPtr->string, charsLeft = msgPtr->numChars; *p != 0;
	    y += msgPtr->fontPtr->ascent + msgPtr->fontPtr->descent) {
	if (*p == '\n') {
	    p++;
	    charsLeft--;
	    continue;
	}
	numChars = TkMeasureChars(msgPtr->fontPtr, p, charsLeft, 0,
		msgPtr->lineLength, TK_WHOLE_WORDS|TK_AT_LEAST_ONE,
		&lineLength);
	switch (msgPtr->anchor) {
	    case TK_ANCHOR_NW: case TK_ANCHOR_W: case TK_ANCHOR_SW:
		x = msgPtr->borderWidth + msgPtr->padX;
		break;
	    case TK_ANCHOR_N: case TK_ANCHOR_CENTER: case TK_ANCHOR_S:
		x = (Tk_Width(tkwin) - msgPtr->lineLength)/2;
		break;
	    default:
		x = Tk_Width(tkwin) - msgPtr->borderWidth - msgPtr->padX
			- msgPtr->lineLength;
		break;
	}
	if (msgPtr->justify == TK_JUSTIFY_CENTER) {
	    x += (msgPtr->lineLength - lineLength)/2;
	} else if (msgPtr->justify == TK_JUSTIFY_RIGHT) {
	    x += msgPtr->lineLength - lineLength;
	}
	TkDisplayChars(Tk_Display(tkwin), Tk_WindowId(tkwin),
		msgPtr->textGC, msgPtr->fontPtr, p, numChars, x, y, 0);
	p += numChars;
	charsLeft -= numChars;

	/*
	 * Skip blanks at the beginning of a line, unless they follow
	 * a user-requested newline.
	 */

	while (isspace(*p)) {
	    charsLeft--;
	    if (*p == '\n') {
		p++;
		break;
	    }
	    p++;
	}
    }

    if (msgPtr->relief != TK_RELIEF_FLAT) {
	Tk_Draw3DRectangle(Tk_Display(tkwin), Tk_WindowId(tkwin),
		msgPtr->border, 0, 0, Tk_Width(tkwin), Tk_Height(tkwin),
		msgPtr->borderWidth, msgPtr->relief);
    }
}

/*
 *--------------------------------------------------------------
 *
 * MessageEventProc --
 *
 *	This procedure is invoked by the Tk dispatcher for various
 *	events on messages.
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
MessageEventProc(clientData, eventPtr)
    ClientData clientData;	/* Information about window. */
    XEvent *eventPtr;		/* Information about event. */
{
    Message *msgPtr = (Message *) clientData;

    if ((eventPtr->type == Expose) && (eventPtr->xexpose.count == 0)) {
	if ((msgPtr->tkwin != NULL) && !(msgPtr->flags & REDRAW_PENDING)) {
	    Tk_DoWhenIdle(DisplayMessage, (ClientData) msgPtr);
	    msgPtr->flags |= REDRAW_PENDING;
	}
    } else if (eventPtr->type == DestroyNotify) {
	Tcl_DeleteCommand(msgPtr->interp, Tk_PathName(msgPtr->tkwin));
	msgPtr->tkwin = NULL;
	if (msgPtr->flags & REDRAW_PENDING) {
	    Tk_CancelIdleCall(DisplayMessage, (ClientData) msgPtr);
	}
	Tk_EventuallyFree((ClientData) msgPtr, DestroyMessage);
    }
}

/*
 *--------------------------------------------------------------
 *
 * MessageTextVarProc --
 *
 *	This procedure is invoked when someone changes the variable
 *	whose contents are to be displayed in a message.
 *
 * Results:
 *	NULL is always returned.
 *
 * Side effects:
 *	The text displayed in the message will change to match the
 *	variable.
 *
 *--------------------------------------------------------------
 */

	/* ARGSUSED */
static char *
MessageTextVarProc(clientData, interp, name1, name2, flags)
    ClientData clientData;	/* Information about message. */
    Tcl_Interp *interp;		/* Interpreter containing variable. */
    char *name1;		/* Name of variable. */
    char *name2;		/* Second part of variable name. */
    int flags;			/* Information about what happened. */
{
    register Message *msgPtr = (Message *) clientData;
    char *value;

    /*
     * If the variable is unset, then immediately recreate it unless
     * the whole interpreter is going away.
     */

    if (flags & TCL_TRACE_UNSETS) {
	if ((flags & TCL_TRACE_DESTROYED) && !(flags & TCL_INTERP_DESTROYED)) {
	    Tcl_SetVar2(interp, name1, name2, msgPtr->string,
		    flags & TCL_GLOBAL_ONLY);
	    Tcl_TraceVar2(interp, name1, name2,
		    TCL_GLOBAL_ONLY|TCL_TRACE_WRITES|TCL_TRACE_UNSETS,
		    MessageTextVarProc, clientData);
	}
	return (char *) NULL;
    }

    value = Tcl_GetVar2(interp, name1, name2, flags & TCL_GLOBAL_ONLY);
    if (value == NULL) {
	value = "";
    }
    if (msgPtr->string != NULL) {
	ckfree(msgPtr->string);
    }
    msgPtr->numChars = strlen(value);
    msgPtr->string = ckalloc((unsigned) (msgPtr->numChars + 1));
    strcpy(msgPtr->string, value);
    ComputeMessageGeometry(msgPtr);

    msgPtr->flags |= CLEAR_NEEDED;
    if ((msgPtr->tkwin != NULL) && Tk_IsMapped(msgPtr->tkwin)
	    && !(msgPtr->flags & REDRAW_PENDING)) {
	Tk_DoWhenIdle(DisplayMessage, (ClientData) msgPtr);
	msgPtr->flags |= REDRAW_PENDING;
    }
    return (char *) NULL;
}
