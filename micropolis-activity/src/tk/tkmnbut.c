/* 
 * tkMenubutton.c --
 *
 *	This module implements button-like widgets that are used
 *	to invoke pull-down menus.
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
static char rcsid[] = "$Header: /user6/ouster/wish/RCS/tkMenubutton.c,v 1.33 92/08/21 16:21:47 ouster Exp $ SPRITE (Berkeley)";
#endif

#include "tkconfig.h"
#include "default.h"
#include "tkint.h"

/*
 * A data structure of the following type is kept for each
 * widget managed by this file:
 */

typedef struct {
    Tk_Window tkwin;		/* Window that embodies the widget.  NULL
				 * means that the window has been destroyed
				 * but the data structures haven't yet been
				 * cleaned up.*/
    Tcl_Interp *interp;		/* Interpreter associated with menu button. */
    char *menuName;		/* Name of menu associated with widget (used
				 * to generate post and unpost commands).
				 * Malloc-ed. */
    Tk_Uid varName;		/* Name of variable associated with collection
				 * of menu bars:  used to allow scanning of
				 * menus.   Also used as identifier for
				 * menu group. */

    /*
     * Information about what's displayed in the menu button:
     */

    char *text;			/* Text to display in button (malloc'ed)
				 * or NULL. */
    int textLength;		/* # of characters in text. */
    int underline;		/* Index of character to underline. */
    char *textVarName;		/* Name of variable (malloc'ed) or NULL.
				 * If non-NULL, button displays the contents
				 * of this variable. */
    Pixmap bitmap;		/* Bitmap to display or None.  If not None
				 * then text and textVar and underline
				 * are ignored. */

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
    GC normalTextGC;		/* GC for drawing text in normal mode. */
    GC activeTextGC;		/* GC for drawing text in active mode (NULL
				 * means use normalTextGC). */
    Pixmap gray;		/* Pixmap for displaying disabled text/icon if
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
				 * inside window region. */

    /*
     * Miscellaneous information:
     */

    Cursor cursor;		/* Current cursor for window, or None. */
    int flags;			/* Various flags;  see below for
				 * definitions. */
} MenuButton;

/*
 * Flag bits for buttons:
 *
 * REDRAW_PENDING:		Non-zero means a DoWhenIdle handler
 *				has already been queued to redraw
 *				this window.
 * POSTED:			Non-zero means that the menu associated
 *				with this button has been posted (typically
 *				because of an active button press).
 */

#define REDRAW_PENDING		1
#define POSTED			2

/*
 * Information used for parsing configuration specs:
 */

static Tk_ConfigSpec configSpecs[] = {
    {TK_CONFIG_BORDER, "-activebackground", "activeBackground", "Foreground",
	DEF_MENUBUTTON_ACTIVE_BG_COLOR, Tk_Offset(MenuButton, activeBorder),
	TK_CONFIG_COLOR_ONLY},
    {TK_CONFIG_BORDER, "-activebackground", "activeBackground", "Foreground",
	DEF_MENUBUTTON_ACTIVE_BG_MONO, Tk_Offset(MenuButton, activeBorder),
	TK_CONFIG_MONO_ONLY},
    {TK_CONFIG_COLOR, "-activeforeground", "activeForeground", "Background",
	DEF_MENUBUTTON_ACTIVE_FG_COLOR, Tk_Offset(MenuButton, activeFg),
	TK_CONFIG_COLOR_ONLY},
    {TK_CONFIG_COLOR, "-activeforeground", "activeForeground", "Background",
	DEF_MENUBUTTON_ACTIVE_FG_MONO, Tk_Offset(MenuButton, activeFg),
	TK_CONFIG_MONO_ONLY},
    {TK_CONFIG_ANCHOR, "-anchor", "anchor", "Anchor",
	DEF_MENUBUTTON_ANCHOR, Tk_Offset(MenuButton, anchor), 0},
    {TK_CONFIG_BORDER, "-background", "background", "Background",
	DEF_MENUBUTTON_BG_COLOR, Tk_Offset(MenuButton, normalBorder),
	TK_CONFIG_COLOR_ONLY},
    {TK_CONFIG_BORDER, "-background", "background", "Background",
	DEF_MENUBUTTON_BG_MONO, Tk_Offset(MenuButton, normalBorder),
	TK_CONFIG_MONO_ONLY},
    {TK_CONFIG_SYNONYM, "-bd", "borderWidth", (char *) NULL,
	(char *) NULL, 0, 0},
    {TK_CONFIG_SYNONYM, "-bg", "background", (char *) NULL,
	(char *) NULL, 0, 0},
#if defined(USE_XPM3)
    {TK_CONFIG_PIXMAP, "-bitmap", "bitmap", "Bitmap",
	DEF_MENUBUTTON_BITMAP, Tk_Offset(MenuButton, bitmap),
	TK_CONFIG_NULL_OK},
#else
    {TK_CONFIG_BITMAP, "-bitmap", "bitmap", "Bitmap",
	DEF_MENUBUTTON_BITMAP, Tk_Offset(MenuButton, bitmap),
	TK_CONFIG_NULL_OK},
#endif
    {TK_CONFIG_PIXELS, "-borderwidth", "borderWidth", "BorderWidth",
	DEF_MENUBUTTON_BORDER_WIDTH, Tk_Offset(MenuButton, borderWidth), 0},
    {TK_CONFIG_ACTIVE_CURSOR, "-cursor", "cursor", "Cursor",
	DEF_MENUBUTTON_CURSOR, Tk_Offset(MenuButton, cursor),
	TK_CONFIG_NULL_OK},
    {TK_CONFIG_COLOR, "-disabledforeground", "disabledForeground",
	"DisabledForeground", DEF_MENUBUTTON_DISABLED_FG_COLOR,
	Tk_Offset(MenuButton, disabledFg),
	TK_CONFIG_COLOR_ONLY|TK_CONFIG_NULL_OK},
    {TK_CONFIG_COLOR, "-disabledforeground", "disabledForeground",
	"DisabledForeground", DEF_MENUBUTTON_DISABLED_FG_MONO,
	Tk_Offset(MenuButton, disabledFg),
	TK_CONFIG_MONO_ONLY|TK_CONFIG_NULL_OK},
    {TK_CONFIG_SYNONYM, "-fg", "foreground", (char *) NULL,
	(char *) NULL, 0, 0},
    {TK_CONFIG_FONT, "-font", "font", "Font",
	DEF_MENUBUTTON_FONT, Tk_Offset(MenuButton, fontPtr), 0},
    {TK_CONFIG_COLOR, "-foreground", "foreground", "Foreground",
	DEF_MENUBUTTON_FG, Tk_Offset(MenuButton, normalFg), 0},
    {TK_CONFIG_INT, "-height", "height", "Height",
	DEF_MENUBUTTON_HEIGHT, Tk_Offset(MenuButton, height), 0},
    {TK_CONFIG_STRING, "-menu", "menu", "Menu",
	DEF_MENUBUTTON_MENU, Tk_Offset(MenuButton, menuName), 0},
    {TK_CONFIG_PIXELS, "-padx", "padX", "Pad",
	DEF_MENUBUTTON_PADX, Tk_Offset(MenuButton, padX), 0},
    {TK_CONFIG_PIXELS, "-pady", "padY", "Pad",
	DEF_MENUBUTTON_PADY, Tk_Offset(MenuButton, padY), 0},
    {TK_CONFIG_RELIEF, "-relief", "relief", "Relief",
	DEF_MENUBUTTON_RELIEF, Tk_Offset(MenuButton, relief), 0},
    {TK_CONFIG_UID, "-state", "state", "State",
	DEF_MENUBUTTON_STATE, Tk_Offset(MenuButton, state), 0},
    {TK_CONFIG_STRING, "-text", "text", "Text",
	DEF_MENUBUTTON_TEXT, Tk_Offset(MenuButton, text), 0},
    {TK_CONFIG_STRING, "-textvariable", "textVariable", "Variable",
	DEF_MENUBUTTON_TEXT_VARIABLE, Tk_Offset(MenuButton, textVarName),
	TK_CONFIG_NULL_OK},
    {TK_CONFIG_INT, "-underline", "underline", "Underline",
	DEF_MENUBUTTON_UNDERLINE, Tk_Offset(MenuButton, underline), 0},
    {TK_CONFIG_UID, "-variable", "variable", "Variable",
	DEF_MENUBUTTON_VARIABLE, Tk_Offset(MenuButton, varName), 0},
    {TK_CONFIG_INT, "-width", "width", "Width",
	DEF_MENUBUTTON_WIDTH, Tk_Offset(MenuButton, width), 0},
    {TK_CONFIG_END, (char *) NULL, (char *) NULL, (char *) NULL,
	(char *) NULL, 0, 0}
};

/*
 * Forward declarations for procedures defined later in this file:
 */

static void		ComputeMenuButtonGeometry _ANSI_ARGS_((
			    MenuButton *mbPtr));
static void		MenuButtonEventProc _ANSI_ARGS_((ClientData clientData,
			    XEvent *eventPtr));
static char *		MenuButtonTextVarProc _ANSI_ARGS_((
			    ClientData clientData, Tcl_Interp *interp,
			    char *name1, char *name2, int flags));
static char *		MenuButtonVarProc _ANSI_ARGS_((ClientData clientData,
			    Tcl_Interp *interp, char *name1, char *name2,
			    int flags));
static int		MenuButtonWidgetCmd _ANSI_ARGS_((ClientData clientData,
			    Tcl_Interp *interp, int argc, char **argv));
static int		ConfigureMenuButton _ANSI_ARGS_((Tcl_Interp *interp,
			    MenuButton *mbPtr, int argc, char **argv,
			    int flags));
static void		DestroyMenuButton _ANSI_ARGS_((ClientData clientData));
static void		DisplayMenuButton _ANSI_ARGS_((ClientData clientData));

/*
 *--------------------------------------------------------------
 *
 * Tk_MenubuttonCmd --
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
Tk_MenubuttonCmd(clientData, interp, argc, argv)
    ClientData clientData;	/* Main window associated with
				 * interpreter. */
    Tcl_Interp *interp;		/* Current interpreter. */
    int argc;			/* Number of arguments. */
    char **argv;		/* Argument strings. */
{
    register MenuButton *mbPtr;
    Tk_Window tkwin = (Tk_Window) clientData;
    Tk_Window new;

    if (argc < 2) {
	Tcl_AppendResult(interp, "wrong # args:  should be \"",
		argv[0], " pathName ?options?\"", (char *) NULL);
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

    mbPtr = (MenuButton *) ckalloc(sizeof(MenuButton));
    mbPtr->tkwin = new;
    mbPtr->interp = interp;
    mbPtr->menuName = NULL;
    mbPtr->varName = NULL;
    mbPtr->text = NULL;
    mbPtr->underline = -1;
    mbPtr->textVarName = NULL;
    mbPtr->bitmap = None;
    mbPtr->state = tkNormalUid;
    mbPtr->normalBorder = NULL;
    mbPtr->activeBorder = NULL;
    mbPtr->borderWidth = 0;
    mbPtr->relief = TK_RELIEF_FLAT;
    mbPtr->fontPtr = NULL;
    mbPtr->normalFg = NULL;
    mbPtr->activeFg = NULL;
    mbPtr->disabledFg = NULL;
    mbPtr->normalTextGC = NULL;
    mbPtr->activeTextGC = NULL;
    mbPtr->gray = None;
    mbPtr->disabledGC = NULL;
    mbPtr->cursor = None;
    mbPtr->flags = 0;

    Tk_SetClass(mbPtr->tkwin, "Menubutton");
    Tk_CreateEventHandler(mbPtr->tkwin, ExposureMask|StructureNotifyMask,
	    MenuButtonEventProc, (ClientData) mbPtr);
    Tcl_CreateCommand(interp, Tk_PathName(mbPtr->tkwin), MenuButtonWidgetCmd,
	    (ClientData) mbPtr, (void (*)()) NULL);
    if (ConfigureMenuButton(interp, mbPtr, argc-2, argv+2, 0) != TCL_OK) {
	Tk_DestroyWindow(mbPtr->tkwin);
	return TCL_ERROR;
    }

    interp->result = Tk_PathName(mbPtr->tkwin);
    return TCL_OK;
}

/*
 *--------------------------------------------------------------
 *
 * MenuButtonWidgetCmd --
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
MenuButtonWidgetCmd(clientData, interp, argc, argv)
    ClientData clientData;	/* Information about button widget. */
    Tcl_Interp *interp;		/* Current interpreter. */
    int argc;			/* Number of arguments. */
    char **argv;		/* Argument strings. */
{
    register MenuButton *mbPtr = (MenuButton *) clientData;
    int result = TCL_OK;
    int length;
    char c;

    if (argc < 2) {
	Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
		" option ?arg arg ...?\"", (char *) NULL);
	return TCL_ERROR;
    }
    Tk_Preserve((ClientData) mbPtr);
    c = argv[1][0];
    length = strlen(argv[1]);
    if ((c == 'a') && (strncmp(argv[1], "activate", length) == 0)) {
	if (argc > 2) {
	    Tcl_AppendResult(interp, "wrong # args: should be \"",
		    argv[0], " activate\"", (char *) NULL);
	    goto error;
	}
	if (mbPtr->state != tkDisabledUid) {
	    mbPtr->state = tkActiveUid;
	    Tk_SetBackgroundFromBorder(mbPtr->tkwin, mbPtr->activeBorder);
	    goto redisplay;
	}
    } else if ((c == 'c') && (strncmp(argv[1], "configure", length) == 0)) {
	if (argc == 2) {
	    result = Tk_ConfigureInfo(interp, mbPtr->tkwin, configSpecs,
		    (char *) mbPtr, (char *) NULL, 0);
	} else if (argc == 3) {
	    result = Tk_ConfigureInfo(interp, mbPtr->tkwin, configSpecs,
		    (char *) mbPtr, argv[2], 0);
	} else {
	    result = ConfigureMenuButton(interp, mbPtr, argc-2, argv+2,
		    TK_CONFIG_ARGV_ONLY);
	}
    } else if ((c == 'd') && (strncmp(argv[1], "deactivate", length) == 0)) {
	if (argc > 2) {
	    Tcl_AppendResult(interp, "wrong # args: should be \"",
		    argv[0], " deactivate\"", (char *) NULL);
	    goto error;
	}
	if (mbPtr->state != tkDisabledUid) {
	    mbPtr->state = tkNormalUid;
	    Tk_SetBackgroundFromBorder(mbPtr->tkwin, mbPtr->normalBorder);
	    goto redisplay;
	}
    } else if ((c == 'p') && (strncmp(argv[1], "post", length) == 0)) {
	if (argc > 2) {
	    Tcl_AppendResult(interp, "wrong # args: should be \"",
		    argv[0], " post\"", (char *) NULL);
	    goto error;
	}
	if ((mbPtr->flags & POSTED) || (mbPtr->menuName == NULL)
		|| (mbPtr->state == tkDisabledUid)) {
	    goto done;
	}

	/*
	 * Store the name of the posted menu into the associated variable.
	 * This will cause any other menu posted via that variable to
	 * unpost itself and will cause this menu to post itself.
	 */

	Tcl_SetVar(interp, mbPtr->varName, Tk_PathName(mbPtr->tkwin),
		TCL_GLOBAL_ONLY);
    } else if ((c == 'u') && (strncmp(argv[1], "unpost", length) == 0)) {
	if (argc > 2) {
	    Tcl_AppendResult(interp, "wrong # args: should be \"",
		    argv[0], " unpost\"", (char *) NULL);
	    goto error;
	}

	/*
	 * The one-liner below looks simple, but it isn't.  This code
	 * does the right thing even if this menu isn't posted anymore,
	 * but some other variable associated with the same variable
	 * is posted instead:  it unposts whatever is posted.  This
	 * approach is necessary because at present ButtonRelease
	 * events go to the menu button where the mouse button was
	 * first pressed;  this may not be the same menu button that's
	 * currently active.
	 */
	Tcl_SetVar(interp, mbPtr->varName, "", TCL_GLOBAL_ONLY);
    } else {
	Tcl_AppendResult(interp, "bad option \"", argv[1],
		"\":  must be activate, configure, deactivate, ",
		"post, or unpost", (char *) NULL);
	goto error;
    }
    done:
    Tk_Release((ClientData) mbPtr);
    return result;

    redisplay:
    if (Tk_IsMapped(mbPtr->tkwin) && !(mbPtr->flags & REDRAW_PENDING)) {
	Tk_DoWhenIdle(DisplayMenuButton, (ClientData) mbPtr);
	mbPtr->flags |= REDRAW_PENDING;
    }
    goto done;

    error:
    Tk_Release((ClientData) mbPtr);
    return TCL_ERROR;
}

/*
 *----------------------------------------------------------------------
 *
 * DestroyMenuButton --
 *
 *	This procedure is invoked to recycle all of the resources
 *	associated with a button widget.  It is invoked as a
 *	when-idle handler in order to make sure that there is no
 *	other use of the button pending at the time of the deletion.
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
DestroyMenuButton(clientData)
    ClientData clientData;	/* Info about button widget. */
{
    register MenuButton *mbPtr = (MenuButton *) clientData;
    if (mbPtr->menuName != NULL) {
        ckfree(mbPtr->menuName);
    }
    if (mbPtr->varName != NULL) {
	Tcl_UntraceVar(mbPtr->interp, mbPtr->varName,
		TCL_GLOBAL_ONLY|TCL_TRACE_WRITES|TCL_TRACE_UNSETS,
		MenuButtonVarProc, (ClientData) mbPtr);
    }
    if (mbPtr->text != NULL) {
	ckfree(mbPtr->text);
    }
    if (mbPtr->textVarName != NULL) {
	Tcl_UntraceVar(mbPtr->interp, mbPtr->textVarName,
		TCL_GLOBAL_ONLY|TCL_TRACE_WRITES|TCL_TRACE_UNSETS,
		MenuButtonTextVarProc, (ClientData) mbPtr);
	ckfree(mbPtr->textVarName);
    }
    if (mbPtr->bitmap != None) {
#if defined(USE_XPM3)
        Tk_FreePixmap(mbPtr->bitmap);
#else
        Tk_FreeBitmap(mbPtr->bitmap);
#endif
    }
    if (mbPtr->normalBorder != NULL) {
	Tk_Free3DBorder(mbPtr->normalBorder);
    }
    if (mbPtr->activeBorder != NULL) {
	Tk_Free3DBorder(mbPtr->activeBorder);
    }
    if (mbPtr->fontPtr != NULL) {
	Tk_FreeFontStruct(mbPtr->fontPtr);
    }
    if (mbPtr->normalFg != NULL) {
	Tk_FreeColor(mbPtr->normalFg);
    }
    if (mbPtr->activeFg != NULL) {
	Tk_FreeColor(mbPtr->activeFg);
    }
    if (mbPtr->disabledFg != NULL) {
	Tk_FreeColor(mbPtr->disabledFg);
    }
    if (mbPtr->normalTextGC != None) {
	Tk_FreeGC(mbPtr->normalTextGC);
    }
    if (mbPtr->activeTextGC != None) {
	Tk_FreeGC(mbPtr->activeTextGC);
    }
    if (mbPtr->gray != None) {
	Tk_FreeBitmap(mbPtr->gray);
    }
    if (mbPtr->disabledGC != None) {
	Tk_FreeGC(mbPtr->disabledGC);
    }
    if (mbPtr->cursor != None) {
	Tk_FreeCursor(mbPtr->cursor);
    }
    ckfree((char *) mbPtr);
}

/*
 *----------------------------------------------------------------------
 *
 * ConfigureMenuButton --
 *
 *	This procedure is called to process an argv/argc list, plus
 *	the Tk option database, in order to configure (or
 *	reconfigure) a menubutton widget.
 *
 * Results:
 *	The return value is a standard Tcl result.  If TCL_ERROR is
 *	returned, then interp->result contains an error message.
 *
 * Side effects:
 *	Configuration information, such as text string, colors, font,
 *	etc. get set for mbPtr;  old resources get freed, if there
 *	were any.  The menubutton is redisplayed.
 *
 *----------------------------------------------------------------------
 */

static int
ConfigureMenuButton(interp, mbPtr, argc, argv, flags)
    Tcl_Interp *interp;		/* Used for error reporting. */
    register MenuButton *mbPtr;	/* Information about widget;  may or may
				 * not already have values for some fields. */
    int argc;			/* Number of valid entries in argv. */
    char **argv;		/* Arguments. */
    int flags;			/* Flags to pass to Tk_ConfigureWidget. */
{
    XGCValues gcValues;
    GC newGC;
    unsigned long mask;
    int result;
    Tk_Uid oldGroup;
    char *value;

    /*
     * Eliminate any existing traces on variables monitored by the button.
     */

    if (mbPtr->varName != NULL) {
	Tcl_UntraceVar(interp, mbPtr->varName,
		TCL_GLOBAL_ONLY|TCL_TRACE_WRITES|TCL_TRACE_UNSETS,
		MenuButtonVarProc, (ClientData) mbPtr);
    }
    if (mbPtr->textVarName != NULL) {
	Tcl_UntraceVar(interp, mbPtr->textVarName,
		TCL_GLOBAL_ONLY|TCL_TRACE_WRITES|TCL_TRACE_UNSETS,
		MenuButtonTextVarProc, (ClientData) mbPtr);
    }

    oldGroup = mbPtr->varName;
    result = Tk_ConfigureWidget(interp, mbPtr->tkwin, configSpecs,
	    argc, argv, (char *) mbPtr, flags);
    if (oldGroup != mbPtr->varName) {
	Tk_UnshareEvents(mbPtr->tkwin, oldGroup);
	Tk_ShareEvents(mbPtr->tkwin, mbPtr->varName);
    }
    if (result != TCL_OK) {
	return TCL_ERROR;
    }

    /*
     * A few options need special processing, such as setting the
     * background from a 3-D border, or filling in complicated
     * defaults that couldn't be specified to Tk_ConfigureWidget.
     */

    if (mbPtr->state == tkActiveUid) {
	Tk_SetBackgroundFromBorder(mbPtr->tkwin, mbPtr->activeBorder);
    } else {
	Tk_SetBackgroundFromBorder(mbPtr->tkwin, mbPtr->normalBorder);
	if ((mbPtr->state != tkNormalUid) && (mbPtr->state != tkDisabledUid)) {
	    Tcl_AppendResult(interp, "bad state value \"", mbPtr->state,
		    "\":  must be normal, active, or disabled", (char *) NULL);
	    mbPtr->state = tkNormalUid;
	    return TCL_ERROR;
	}
    }

    gcValues.font = mbPtr->fontPtr->fid;
    gcValues.foreground = mbPtr->normalFg->pixel;
    gcValues.background = Tk_3DBorderColor(mbPtr->normalBorder)->pixel;

    /*
     * Note: GraphicsExpose events are disabled in GC's because they're
     * used to copy stuff from an off-screen pixmap onto the screen (we know
     * that there's no problem with obscured areas).
     */

    gcValues.graphics_exposures = False;
    newGC = Tk_GetGC(mbPtr->tkwin,
	    GCForeground|GCBackground|GCFont|GCGraphicsExposures, &gcValues);
    if (mbPtr->normalTextGC != None) {
	Tk_FreeGC(mbPtr->normalTextGC);
    }
    mbPtr->normalTextGC = newGC;

    gcValues.font = mbPtr->fontPtr->fid;
    gcValues.foreground = mbPtr->activeFg->pixel;
    gcValues.background = Tk_3DBorderColor(mbPtr->activeBorder)->pixel;
    newGC = Tk_GetGC(mbPtr->tkwin, GCForeground|GCBackground|GCFont,
	    &gcValues);
    if (mbPtr->activeTextGC != None) {
	Tk_FreeGC(mbPtr->activeTextGC);
    }
    mbPtr->activeTextGC = newGC;

    gcValues.font = mbPtr->fontPtr->fid;
    gcValues.background = Tk_3DBorderColor(mbPtr->normalBorder)->pixel;
    if (mbPtr->disabledFg != NULL) {
	gcValues.foreground = mbPtr->disabledFg->pixel;
	mask = GCForeground|GCBackground|GCFont;
    } else {
	gcValues.foreground = gcValues.background;
	if (mbPtr->gray == None) {
	    mbPtr->gray = Tk_GetBitmap(interp, mbPtr->tkwin,
		    Tk_GetUid("gray50"));
	    if (mbPtr->gray == None) {
		return TCL_ERROR;
	    }
	}
	gcValues.fill_style = FillStippled;
	gcValues.stipple = mbPtr->gray;
	mask = GCForeground|GCFillStyle|GCStipple;
    }
    newGC = Tk_GetGC(mbPtr->tkwin, mask, &gcValues);
    if (mbPtr->disabledGC != None) {
	Tk_FreeGC(mbPtr->disabledGC);
    }
    mbPtr->disabledGC = newGC;

    if (mbPtr->padX < 0) {
	mbPtr->padX = 0;
    }
    if (mbPtr->padY < 0) {
	mbPtr->padY = 0;
    }

    /*
     * Set up a trace on the menu button's variable, then initialize
     * the variable if it doesn't already exist, so that it can be
     * accessed immediately from Tcl code without fear of
     * "nonexistent variable" errors.
     */

    value = Tcl_GetVar(interp, mbPtr->varName, TCL_GLOBAL_ONLY);
    if (value == NULL) {
	Tcl_SetVar(interp, mbPtr->varName, "", TCL_GLOBAL_ONLY);
    }
    Tcl_TraceVar(interp, mbPtr->varName,
	    TCL_GLOBAL_ONLY|TCL_TRACE_WRITES|TCL_TRACE_UNSETS,
	    MenuButtonVarProc, (ClientData) mbPtr);

    /*
     * Set up a trace on the variable that determines what's displayed
     * in the menu button, if such a trace has been requested.
     */

    if ((mbPtr->bitmap == None) && (mbPtr->textVarName != NULL)) {
	char *value;

	value = Tcl_GetVar(interp, mbPtr->textVarName, TCL_GLOBAL_ONLY);
	if (value == NULL) {
	    Tcl_SetVar(interp, mbPtr->textVarName, mbPtr->text,
		    TCL_GLOBAL_ONLY);
	} else {
	    if (mbPtr->text != NULL) {
		ckfree(mbPtr->text);
	    }
	    mbPtr->text = ckalloc((unsigned) (strlen(value) + 1));
	    strcpy(mbPtr->text, value);
	}
	Tcl_TraceVar(interp, mbPtr->textVarName,
		TCL_GLOBAL_ONLY|TCL_TRACE_WRITES|TCL_TRACE_UNSETS,
		MenuButtonTextVarProc, (ClientData) mbPtr);
    }

    /*
     * Recompute the geometry for the button.
     */

    ComputeMenuButtonGeometry(mbPtr);

    /*
     * Lastly, arrange for the button to be redisplayed.
     */

    if (Tk_IsMapped(mbPtr->tkwin) && !(mbPtr->flags & REDRAW_PENDING)) {
	Tk_DoWhenIdle(DisplayMenuButton, (ClientData) mbPtr);
	mbPtr->flags |= REDRAW_PENDING;
    }

    return TCL_OK;
}

/*
 *----------------------------------------------------------------------
 *
 * DisplayMenuButton --
 *
 *	This procedure is invoked to display a menubutton widget.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Commands are output to X to display the menubutton in its
 *	current mode.
 *
 *----------------------------------------------------------------------
 */

static void
DisplayMenuButton(clientData)
    ClientData clientData;	/* Information about widget. */
{
    register MenuButton *mbPtr = (MenuButton *) clientData;
    GC gc;
    Tk_3DBorder border;
    Pixmap pixmap;
    int x = 0;			/* Initialization needed only to stop
				 * compiler warning. */
    int y;
    register Tk_Window tkwin = mbPtr->tkwin;

    mbPtr->flags &= ~REDRAW_PENDING;
    if ((mbPtr->tkwin == NULL) || !Tk_IsMapped(tkwin)) {
	return;
    }

    if ((mbPtr->state == tkDisabledUid) && (mbPtr->disabledFg != NULL)) {
	gc = mbPtr->disabledGC;
	border = mbPtr->normalBorder;
    } else if (mbPtr->state == tkActiveUid) {
	gc = mbPtr->activeTextGC;
	border = mbPtr->activeBorder;
    } else {
	gc = mbPtr->normalTextGC;
	border = mbPtr->normalBorder;
    }

    /*
     * In order to avoid screen flashes, this procedure redraws
     * the menu button in a pixmap, then copies the pixmap to the
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

    if (mbPtr->bitmap != None) {
	unsigned int width, height;

#if defined(USE_XPM3)
	Tk_SizeOfPixmap(mbPtr->bitmap, &width, &height);
#else
	Tk_SizeOfBitmap(mbPtr->bitmap, &width, &height);
#endif
	switch (mbPtr->anchor) {
	    case TK_ANCHOR_NW: case TK_ANCHOR_W: case TK_ANCHOR_SW:
		x += mbPtr->borderWidth + mbPtr->padX;
		break;
	    case TK_ANCHOR_N: case TK_ANCHOR_CENTER: case TK_ANCHOR_S:
		x += (Tk_Width(tkwin) - width)/2;
		break;
	    default:
		x += Tk_Width(tkwin) - mbPtr->borderWidth - mbPtr->padX
			- width;
		break;
	}
	switch (mbPtr->anchor) {
	    case TK_ANCHOR_NW: case TK_ANCHOR_N: case TK_ANCHOR_NE:
		y = mbPtr->borderWidth + mbPtr->padY;
		break;
	    case TK_ANCHOR_W: case TK_ANCHOR_CENTER: case TK_ANCHOR_E:
		y = (Tk_Height(tkwin) - height)/2;
		break;
	    default:
		y = Tk_Height(tkwin) - mbPtr->borderWidth - mbPtr->padY
			- height;
		break;
	}
#if defined(USE_XPM3)        
	XCopyArea(Tk_Display(tkwin), mbPtr->bitmap, pixmap,
		gc, 0, 0, width, height, x, y);
#else
	XCopyPlane(Tk_Display(tkwin), mbPtr->bitmap, pixmap,
		gc, 0, 0, width, height, x, y, 1);
#endif
    } else {
	switch (mbPtr->anchor) {
	    case TK_ANCHOR_NW: case TK_ANCHOR_W: case TK_ANCHOR_SW:
		x = mbPtr->borderWidth + mbPtr->padX + mbPtr->leftBearing;
		break;
	    case TK_ANCHOR_N: case TK_ANCHOR_CENTER: case TK_ANCHOR_S:
		x = (Tk_Width(tkwin) + mbPtr->leftBearing
			- mbPtr->rightBearing)/2;
		break;
	    default:
		x = Tk_Width(tkwin) - mbPtr->borderWidth - mbPtr->padX
			- mbPtr->rightBearing;
		break;
	}
	switch (mbPtr->anchor) {
	    case TK_ANCHOR_NW: case TK_ANCHOR_N: case TK_ANCHOR_NE:
		y = mbPtr->borderWidth + mbPtr->fontPtr->ascent
			+ mbPtr->padY;
		break;
	    case TK_ANCHOR_W: case TK_ANCHOR_CENTER: case TK_ANCHOR_E:
		y = (Tk_Height(tkwin) + mbPtr->fontPtr->ascent
			- mbPtr->fontPtr->descent)/2;
		break;
	    default:
		y = Tk_Height(tkwin) - mbPtr->borderWidth - mbPtr->padY
			- mbPtr->fontPtr->descent;
		break;
	}
	XDrawString(Tk_Display(tkwin), pixmap, gc, x, y, mbPtr->text,
		mbPtr->textLength);
	if (mbPtr->underline >= 0) {
	    TkUnderlineChars(Tk_Display(tkwin), pixmap, gc, mbPtr->fontPtr,
		    mbPtr->text, x, y, TK_NEWLINES_NOT_SPECIAL,
		    mbPtr->underline, mbPtr->underline);
	}
    }

    /*
     * If the menu button is disabled with a stipple rather than a special
     * foreground color, generate the stippled effect.
     */

    if ((mbPtr->state == tkDisabledUid) && (mbPtr->disabledFg == NULL)) {
	XFillRectangle(Tk_Display(tkwin), pixmap, mbPtr->disabledGC,
		mbPtr->borderWidth, mbPtr->borderWidth,
		(unsigned) (Tk_Width(tkwin) - 2*mbPtr->borderWidth),
		(unsigned) (Tk_Height(tkwin) - 2*mbPtr->borderWidth));
    }

    /*
     * Draw the border last.  This way, if the menu button's contents
     * overflow onto the border they'll be covered up by the border.
     */

    if (mbPtr->relief != TK_RELIEF_FLAT) {
	Tk_Draw3DRectangle(Tk_Display(tkwin), pixmap, border,
		0, 0, Tk_Width(tkwin), Tk_Height(tkwin),
		mbPtr->borderWidth, mbPtr->relief);
    }

    /*
     * Copy the information from the off-screen pixmap onto the screen,
     * then delete the pixmap.
     */

    XCopyArea(Tk_Display(tkwin), pixmap, Tk_WindowId(tkwin),
	mbPtr->normalTextGC, 0, 0, Tk_Width(tkwin), Tk_Height(tkwin), 0, 0);
    XFreePixmap(Tk_Display(tkwin), pixmap);
}

/*
 *--------------------------------------------------------------
 *
 * MenuButtonEventProc --
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
MenuButtonEventProc(clientData, eventPtr)
    ClientData clientData;	/* Information about window. */
    XEvent *eventPtr;		/* Information about event. */
{
    MenuButton *mbPtr = (MenuButton *) clientData;
    if ((eventPtr->type == Expose) && (eventPtr->xexpose.count == 0)) {
	if ((mbPtr->tkwin != NULL) && !(mbPtr->flags & REDRAW_PENDING)) {
	    Tk_DoWhenIdle(DisplayMenuButton, (ClientData) mbPtr);
	    mbPtr->flags |= REDRAW_PENDING;
	}
    } else if (eventPtr->type == DestroyNotify) {
	Tcl_DeleteCommand(mbPtr->interp, Tk_PathName(mbPtr->tkwin));

	/*
	 * Careful!  Must delete the event-sharing information here
	 * rather than in DestroyMenuButton.  By the time that procedure
	 * is called the tkwin may have been reused, resulting in some
	 * other window accidentally being cut off from shared events.
	 */

	Tk_UnshareEvents(mbPtr->tkwin, mbPtr->varName);
	mbPtr->tkwin = NULL;
	if (mbPtr->flags & REDRAW_PENDING) {
	    Tk_CancelIdleCall(DisplayMenuButton, (ClientData) mbPtr);
	}
	Tk_EventuallyFree((ClientData) mbPtr, DestroyMenuButton);
    }
}

/*
 *----------------------------------------------------------------------
 *
 * ComputeMenuButtonGeometry --
 *
 *	After changes in a menu button's text or bitmap, this procedure
 *	recomputes the menu button's geometry and passes this information
 *	along to the geometry manager for the window.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The menu button's window may change size.
 *
 *----------------------------------------------------------------------
 */

static void
ComputeMenuButtonGeometry(mbPtr)
    register MenuButton *mbPtr;		/* Widget record for menu button. */
{
    XCharStruct bbox;
    int dummy;
    unsigned int width, height;

    if (mbPtr->bitmap != None) {
#if defined(USE_XPM3)
        Tk_SizeOfPixmap(mbPtr->bitmap, &width, &height);
#else
        Tk_SizeOfBitmap(mbPtr->bitmap, &width, &height);
#endif
	if (mbPtr->width > 0) {
	    width = mbPtr->width;
	}
	if (mbPtr->height > 0) {
	    height = mbPtr->height;
	}
    } else {
	mbPtr->textLength = strlen(mbPtr->text);
	XTextExtents(mbPtr->fontPtr, mbPtr->text, mbPtr->textLength,
		&dummy, &dummy, &dummy, &bbox);
	mbPtr->leftBearing = bbox.lbearing;
	mbPtr->rightBearing = bbox.rbearing;
	width = bbox.lbearing + bbox.rbearing;
	height = mbPtr->fontPtr->ascent + mbPtr->fontPtr->descent;
	if (mbPtr->width > 0) {
	    width = mbPtr->width * XTextWidth(mbPtr->fontPtr, "0", 1);
	}
	if (mbPtr->height > 0) {
	    height *= mbPtr->height;
	}
    }

    width += 2*mbPtr->padX;
    height += 2*mbPtr->padY;
    Tk_GeometryRequest(mbPtr->tkwin, (int) (width + 2*mbPtr->borderWidth),
	    (int) (height + 2*mbPtr->borderWidth));
    Tk_SetInternalBorder(mbPtr->tkwin, mbPtr->borderWidth);
}

/*
 *--------------------------------------------------------------
 *
 * MenuButtonVarProc --
 *
 *	This procedure is invoked when someone changes the
 *	state variable associated with a menubutton.  This causes
 *	the posted/unposted state of the menu to change if needed
 *	to match the variable's new value.
 *
 * Results:
 *	NULL is always returned.
 *
 * Side effects:
 *	The menu may be posted or unposted.
 *
 *--------------------------------------------------------------
 */

	/* ARGSUSED */
static char *
MenuButtonVarProc(clientData, interp, name1, name2, flags)
    ClientData clientData;	/* Information about button. */
    Tcl_Interp *interp;		/* Interpreter containing variable. */
    char *name1;		/* First part of variable's name. */
    char *name2;		/* Second part of variable's name. */
    int flags;			/* Describes what's happening to variable. */
{
    register MenuButton *mbPtr = (MenuButton *) clientData;
    char *value;
    int newFlags;

    /*
     * If the variable is being unset, then just re-establish the
     * trace unless the whole interpreter is going away.  Also unpost
     * the menu.
     */

    newFlags = mbPtr->flags;
    if (flags & TCL_TRACE_UNSETS) {
	newFlags &= ~POSTED;
	if ((flags & TCL_TRACE_DESTROYED) && !(flags & TCL_INTERP_DESTROYED)) {
	    Tcl_TraceVar2(interp, name1, name2,
		    TCL_GLOBAL_ONLY|TCL_TRACE_WRITES|TCL_TRACE_UNSETS,
		    MenuButtonVarProc, clientData);
	}
    } else {

	/*
	 * Use the value of the variable to update the posted status of
	 * the menu.
	 */
    
	value = Tcl_GetVar2(interp, name1, name2, flags & TCL_GLOBAL_ONLY);
	if (strcmp(value, Tk_PathName(mbPtr->tkwin)) == 0) {
	    newFlags |= POSTED;
	} else {
	    newFlags &= ~POSTED;
	}
    }

    if ((mbPtr->menuName != NULL) && (newFlags != mbPtr->flags)) {
	mbPtr->flags = newFlags;
	if (newFlags & POSTED) {
	    int x, y;
	    char string[50];

	    /*
	     * Post the menu just below the menu button.
	     */

	    Tk_GetRootCoords(mbPtr->tkwin, &x, &y);
	    y += Tk_Height(mbPtr->tkwin);
	    sprintf(string, "%d %d ", x, y);
	    if (Tcl_VarEval(interp, mbPtr->menuName, " post ", string,
		    mbPtr->varName, (char *) NULL) != TCL_OK) {
		TkBindError(interp);
	    }
	} else {
	    if (Tcl_VarEval(interp, mbPtr->menuName, " unpost",
		    (char *) NULL) != TCL_OK) {
		TkBindError(interp);
	    }
	}
    }
    return (char *) NULL;
}

/*
 *--------------------------------------------------------------
 *
 * MenuButtonTextVarProc --
 *
 *	This procedure is invoked when someone changes the variable
 *	whose contents are to be displayed in a menu button.
 *
 * Results:
 *	NULL is always returned.
 *
 * Side effects:
 *	The text displayed in the menu button will change to match the
 *	variable.
 *
 *--------------------------------------------------------------
 */

	/* ARGSUSED */
static char *
MenuButtonTextVarProc(clientData, interp, name1, name2, flags)
    ClientData clientData;	/* Information about button. */
    Tcl_Interp *interp;		/* Interpreter containing variable. */
    char *name1;		/* Name of variable. */
    char *name2;		/* Second part of variable name. */
    int flags;			/* Information about what happened. */
{
    register MenuButton *mbPtr = (MenuButton *) clientData;
    char *value;

    /*
     * If the variable is unset, then immediately recreate it unless
     * the whole interpreter is going away.
     */

    if (flags & TCL_TRACE_UNSETS) {
	if ((flags & TCL_TRACE_DESTROYED) && !(flags & TCL_INTERP_DESTROYED)) {
	    Tcl_SetVar2(interp, name1, name2, mbPtr->text,
		    flags & TCL_GLOBAL_ONLY);
	    Tcl_TraceVar2(interp, name1, name2,
		    TCL_GLOBAL_ONLY|TCL_TRACE_WRITES|TCL_TRACE_UNSETS,
		    MenuButtonTextVarProc, clientData);
	}
	return (char *) NULL;
    }

    value = Tcl_GetVar2(interp, name1, name2, flags & TCL_GLOBAL_ONLY);
    if (value == NULL) {
	value = "";
    }
    if (mbPtr->text != NULL) {
	ckfree(mbPtr->text);
    }
    mbPtr->text = ckalloc((unsigned) (strlen(value) + 1));
    strcpy(mbPtr->text, value);
    ComputeMenuButtonGeometry(mbPtr);

    if ((mbPtr->tkwin != NULL) && Tk_IsMapped(mbPtr->tkwin)
	    && !(mbPtr->flags & REDRAW_PENDING)) {
	Tk_DoWhenIdle(DisplayMenuButton, (ClientData) mbPtr);
	mbPtr->flags |= REDRAW_PENDING;
    }
    return (char *) NULL;
}
