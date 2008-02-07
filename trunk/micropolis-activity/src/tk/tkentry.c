/* 
 * tkEntry.c --
 *
 *	This module implements entry widgets for the Tk
 *	toolkit.  An entry displays a string and allows
 *	the string to be edited.
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
static char rcsid[] = "$Header: /user6/ouster/wish/RCS/tkEntry.c,v 1.37 92/08/21 16:09:15 ouster Exp $ SPRITE (Berkeley)";
#endif

#include "default.h"
#include "tkconfig.h"
#include "tkint.h"

/*
 * A data structure of the following type is kept for each entry
 * widget managed by this file:
 */

typedef struct {
    Tk_Window tkwin;		/* Window that embodies the entry. NULL
				 * means that the window has been destroyed
				 * but the data structures haven't yet been
				 * cleaned up.*/
    Tcl_Interp *interp;		/* Interpreter associated with entry. */
    int numChars;		/* Number of non-NULL characters in
				 * string (may be 0). */
    char *string;		/* Pointer to storage for string;
				 * NULL-terminated;  malloc-ed. */
    char *textVarName;		/* Name of variable (malloc'ed) or NULL.
				 * If non-NULL, entry's string tracks the
				 * contents of this variable and vice versa. */
    Tk_Uid state;		/* Normal or disabled.  Entry is read-only
				 * when disabled. */

    /*
     * Information used when displaying widget:
     */

    Tk_3DBorder normalBorder;	/* Used for drawing border around whole
				 * window, plus used for background. */
    int borderWidth;		/* Width of 3-D border around window. */
    int relief;			/* 3-D effect: TK_RELIEF_RAISED, etc. */
    XFontStruct *fontPtr;	/* Information about text font, or NULL. */
    XColor *fgColorPtr;		/* Text color in normal mode. */
    GC textGC;			/* For drawing normal text. */
    Tk_3DBorder selBorder;	/* Border and background for selected
				 * characters. */
    int selBorderWidth;		/* Width of border around selection. */
    XColor *selFgColorPtr;	/* Foreground color for selected text. */
    GC selTextGC;		/* For drawing selected text. */
    Tk_3DBorder cursorBorder;	/* Used to draw vertical bar for insertion
				 * cursor. */
    int cursorWidth;		/* Total width of insert cursor. */
    int cursorBorderWidth;	/* Width of 3-D border around insert cursor. */
    int cursorOnTime;		/* Number of milliseconds cursor should spend
				 * in "on" state for each blink. */
    int cursorOffTime;		/* Number of milliseconds cursor should spend
				 * in "off" state for each blink. */
    Tk_TimerToken cursorBlinkHandler;
				/* Timer handler used to blink cursor on and
				 * off. */
    int avgWidth;		/* Width of average character. */
    int prefWidth;		/* Desired width of window, measured in
				 * average characters. */
    int offset;			/* 0 if window is flat, or borderWidth if
				 * raised or sunken. */
    int leftIndex;		/* Index of left-most character visible in
				 * window. */
    int cursorPos;		/* Index of character before which next
				 * typed character will be inserted. */

    /*
     * Information about what's selected, if any.
     */

    int selectFirst;		/* Index of first selected character (-1 means
				 * nothing selected. */
    int selectLast;		/* Index of last selected character (-1 means
				 * nothing selected. */
    int selectAnchor;		/* Fixed end of selection (i.e. "select to"
				 * operation will use this as one end of the
				 * selection). */
    int exportSelection;	/* Non-zero means tie internal entry selection
				 * to X selection. */

    /*
     * Information for scanning:
     */

    int scanMarkX;		/* X-position at which scan started (e.g.
				 * button was pressed here). */
    int scanMarkIndex;		/* Index of character that was at left of
				 * window when scan started. */

    /*
     * Miscellaneous information:
     */

    Cursor cursor;		/* Current cursor for window, or None. */
    char *scrollCmd;		/* Command prefix for communicating with
				 * scrollbar(s).  Malloc'ed.  NULL means
				 * no command to issue. */
    int flags;			/* Miscellaneous flags;  see below for
				 * definitions. */
} Entry;

/*
 * Assigned bits of "flags" fields of Entry structures, and what those
 * bits mean:
 *
 * REDRAW_PENDING:		Non-zero means a DoWhenIdle handler has
 *				already been queued to redisplay the entry.
 * BORDER_NEEDED:		Non-zero means 3-D border must be redrawn
 *				around window during redisplay.  Normally
 *				only text portion needs to be redrawn.
 * CURSOR_ON:			Non-zero means cursor is displayed at
 *				present.  0 means it isn't displayed.
 * GOT_FOCUS:			Non-zero means this window has the input
 *				focus.
 */

#define REDRAW_PENDING		1
#define BORDER_NEEDED		2
#define CURSOR_ON		4
#define GOT_FOCUS		8

/*
 * Information used for argv parsing.
 */

static Tk_ConfigSpec configSpecs[] = {
    {TK_CONFIG_BORDER, "-background", "background", "Background",
	DEF_ENTRY_BG_COLOR, Tk_Offset(Entry, normalBorder),
	TK_CONFIG_COLOR_ONLY},
    {TK_CONFIG_BORDER, "-background", "background", "Background",
	DEF_ENTRY_BG_MONO, Tk_Offset(Entry, normalBorder),
	TK_CONFIG_MONO_ONLY},
    {TK_CONFIG_SYNONYM, "-bd", "borderWidth", (char *) NULL,
	(char *) NULL, 0, 0},
    {TK_CONFIG_SYNONYM, "-bg", "background", (char *) NULL,
	(char *) NULL, 0, 0},
    {TK_CONFIG_PIXELS, "-borderwidth", "borderWidth", "BorderWidth",
	DEF_ENTRY_BORDER_WIDTH, Tk_Offset(Entry, borderWidth), 0},
    {TK_CONFIG_ACTIVE_CURSOR, "-cursor", "cursor", "Cursor",
	DEF_ENTRY_CURSOR, Tk_Offset(Entry, cursor), TK_CONFIG_NULL_OK},
    {TK_CONFIG_BORDER, "-cursorbackground", "cursorBackground", "Foreground",
	DEF_ENTRY_CURSOR_BG, Tk_Offset(Entry, cursorBorder), 0},
    {TK_CONFIG_PIXELS, "-cursorborderwidth", "cursorBorderWidth", "BorderWidth",
	DEF_ENTRY_CURSOR_BD_COLOR, Tk_Offset(Entry, cursorBorderWidth),
	TK_CONFIG_COLOR_ONLY},
    {TK_CONFIG_PIXELS, "-cursorborderwidth", "cursorBorderWidth", "BorderWidth",
	DEF_ENTRY_CURSOR_BD_MONO, Tk_Offset(Entry, cursorBorderWidth),
	TK_CONFIG_MONO_ONLY},
    {TK_CONFIG_INT, "-cursorofftime", "cursorOffTime", "OffTime",
	DEF_ENTRY_CURSOR_OFF_TIME, Tk_Offset(Entry, cursorOffTime), 0},
    {TK_CONFIG_INT, "-cursorontime", "cursorOnTime", "OnTime",
	DEF_ENTRY_CURSOR_ON_TIME, Tk_Offset(Entry, cursorOnTime), 0},
    {TK_CONFIG_PIXELS, "-cursorwidth", "cursorWidth", "CursorWidth",
	DEF_ENTRY_CURSOR_WIDTH, Tk_Offset(Entry, cursorWidth), 0},
    {TK_CONFIG_BOOLEAN, "-exportselection", "exportSelection",
	"ExportSelection", DEF_ENTRY_EXPORT_SELECTION,
	Tk_Offset(Entry, exportSelection), 0},
    {TK_CONFIG_SYNONYM, "-fg", "foreground", (char *) NULL,
	(char *) NULL, 0, 0},
    {TK_CONFIG_FONT, "-font", "font", "Font",
	DEF_ENTRY_FONT, Tk_Offset(Entry, fontPtr), 0},
    {TK_CONFIG_COLOR, "-foreground", "foreground", "Foreground",
	DEF_ENTRY_FG, Tk_Offset(Entry, fgColorPtr), 0},
    {TK_CONFIG_RELIEF, "-relief", "relief", "Relief",
	DEF_ENTRY_RELIEF, Tk_Offset(Entry, relief), 0},
    {TK_CONFIG_STRING, "-scrollcommand", "scrollCommand", "ScrollCommand",
	DEF_ENTRY_SCROLL_COMMAND, Tk_Offset(Entry, scrollCmd), 0},
    {TK_CONFIG_BORDER, "-selectbackground", "selectBackground", "Foreground",
	DEF_ENTRY_SELECT_COLOR, Tk_Offset(Entry, selBorder),
	TK_CONFIG_COLOR_ONLY},
    {TK_CONFIG_BORDER, "-selectbackground", "selectBackground", "Foreground",
	DEF_ENTRY_SELECT_MONO, Tk_Offset(Entry, selBorder),
	TK_CONFIG_MONO_ONLY},
    {TK_CONFIG_PIXELS, "-selectborderwidth", "selectBorderWidth", "BorderWidth",
	DEF_ENTRY_SELECT_BD_COLOR, Tk_Offset(Entry, selBorderWidth),
	TK_CONFIG_COLOR_ONLY},
    {TK_CONFIG_PIXELS, "-selectborderwidth", "selectBorderWidth", "BorderWidth",
	DEF_ENTRY_SELECT_BD_MONO, Tk_Offset(Entry, selBorderWidth),
	TK_CONFIG_MONO_ONLY},
    {TK_CONFIG_COLOR, "-selectforeground", "selectForeground", "Background",
	DEF_ENTRY_SELECT_FG_COLOR, Tk_Offset(Entry, selFgColorPtr),
	TK_CONFIG_COLOR_ONLY},
    {TK_CONFIG_COLOR, "-selectforeground", "selectForeground", "Background",
	DEF_ENTRY_SELECT_FG_MONO, Tk_Offset(Entry, selFgColorPtr),
	TK_CONFIG_MONO_ONLY},
    {TK_CONFIG_UID, "-state", "state", "State",
	DEF_ENTRY_STATE, Tk_Offset(Entry, state), 0},
    {TK_CONFIG_STRING, "-textvariable", "textVariable", "Variable",
	DEF_ENTRY_TEXT_VARIABLE, Tk_Offset(Entry, textVarName),
	TK_CONFIG_NULL_OK},
    {TK_CONFIG_INT, "-width", "width", "Width",
	DEF_ENTRY_WIDTH, Tk_Offset(Entry, prefWidth), 0},
    {TK_CONFIG_END, (char *) NULL, (char *) NULL, (char *) NULL,
	(char *) NULL, 0, 0}
};

/*
 * Flags for GetEntryIndex procedure:
 */

#define ZERO_OK			1
#define LAST_PLUS_ONE_OK	2

/*
 * Forward declarations for procedures defined later in this file:
 */

static int		ConfigureEntry _ANSI_ARGS_((Tcl_Interp *interp,
			    Entry *entryPtr, int argc, char **argv,
			    int flags));
static void		DeleteChars _ANSI_ARGS_((Entry *entryPtr, int index,
			    int count));
static void		DestroyEntry _ANSI_ARGS_((ClientData clientData));
static void		DisplayEntry _ANSI_ARGS_((ClientData clientData));
static int		GetEntryIndex _ANSI_ARGS_((Tcl_Interp *interp,
			    Entry *entryPtr, char *string, int *indexPtr));
static void		InsertChars _ANSI_ARGS_((Entry *entryPtr, int index,
			    char *string));
static void		EntryBlinkProc _ANSI_ARGS_((ClientData clientData));
static void		EntryEventProc _ANSI_ARGS_((ClientData clientData,
			    XEvent *eventPtr));
static void		EntryFocusProc _ANSI_ARGS_ ((ClientData clientData,
			    int gotFocus));
static int		EntryFetchSelection _ANSI_ARGS_((ClientData clientData,
			    int offset, char *buffer, int maxBytes));
static void		EntryLostSelection _ANSI_ARGS_((
			    ClientData clientData));
static void		EventuallyRedraw _ANSI_ARGS_((Entry *entryPtr));
static void		EntryScanTo _ANSI_ARGS_((Entry *entryPtr, int y));
static void		EntrySetValue _ANSI_ARGS_((Entry *entryPtr,
			    char *value));
static void		EntrySelectTo _ANSI_ARGS_((
			    Entry *entryPtr, int index));
static char *		EntryTextVarProc _ANSI_ARGS_((ClientData clientData,
			    Tcl_Interp *interp, char *name1, char *name2,
			    int flags));
static void		EntryUpdateScrollbar _ANSI_ARGS_((Entry *entryPtr));
static int		EntryWidgetCmd _ANSI_ARGS_((ClientData clientData,
			    Tcl_Interp *interp, int argc, char **argv));

/*
 *--------------------------------------------------------------
 *
 * Tk_EntryCmd --
 *
 *	This procedure is invoked to process the "entry" Tcl
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
Tk_EntryCmd(clientData, interp, argc, argv)
    ClientData clientData;	/* Main window associated with
				 * interpreter. */
    Tcl_Interp *interp;		/* Current interpreter. */
    int argc;			/* Number of arguments. */
    char **argv;		/* Argument strings. */
{
    Tk_Window tkwin = (Tk_Window) clientData;
    register Entry *entryPtr;
    Tk_Window new;

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
     * Initialize the fields of the structure that won't be initialized
     * by ConfigureEntry, or that ConfigureEntry requires to be
     * initialized already (e.g. resource pointers).
     */

    entryPtr = (Entry *) ckalloc(sizeof(Entry));
    entryPtr->tkwin = new;
    entryPtr->interp = interp;
    entryPtr->numChars = 0;
    entryPtr->string = (char *) ckalloc(1);
    entryPtr->string[0] = '\0';
    entryPtr->textVarName = NULL;
    entryPtr->state = tkNormalUid;
    entryPtr->normalBorder = NULL;
    entryPtr->fontPtr = NULL;
    entryPtr->fgColorPtr = NULL;
    entryPtr->textGC = None;
    entryPtr->selBorder = NULL;
    entryPtr->selFgColorPtr = NULL;
    entryPtr->selTextGC = NULL;
    entryPtr->cursorBorder = NULL;
    entryPtr->cursorBlinkHandler = (Tk_TimerToken) NULL;
    entryPtr->leftIndex = 0;
    entryPtr->cursorPos = 0;
    entryPtr->selectFirst = -1;
    entryPtr->selectLast = -1;
    entryPtr->selectAnchor = 0;
    entryPtr->exportSelection = 1;
    entryPtr->scanMarkX = 0;
    entryPtr->cursor = None;
    entryPtr->scrollCmd = NULL;
    entryPtr->flags = 0;

    Tk_SetClass(entryPtr->tkwin, "Entry");
    Tk_CreateEventHandler(entryPtr->tkwin, ExposureMask|StructureNotifyMask,
	    EntryEventProc, (ClientData) entryPtr);
    Tk_CreateSelHandler(entryPtr->tkwin, XA_STRING, EntryFetchSelection,
	    (ClientData) entryPtr, XA_STRING);
    Tcl_CreateCommand(interp, Tk_PathName(entryPtr->tkwin), EntryWidgetCmd,
	    (ClientData) entryPtr, (void (*)()) NULL);
    if (ConfigureEntry(interp, entryPtr, argc-2, argv+2, 0) != TCL_OK) {
	goto error;
    }
    Tk_CreateFocusHandler(entryPtr->tkwin, EntryFocusProc,
	    (ClientData) entryPtr);

    interp->result = Tk_PathName(entryPtr->tkwin);
    return TCL_OK;

    error:
    Tk_DestroyWindow(entryPtr->tkwin);
    return TCL_ERROR;
}

/*
 *--------------------------------------------------------------
 *
 * EntryWidgetCmd --
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
EntryWidgetCmd(clientData, interp, argc, argv)
    ClientData clientData;		/* Information about entry widget. */
    Tcl_Interp *interp;			/* Current interpreter. */
    int argc;				/* Number of arguments. */
    char **argv;			/* Argument strings. */
{
    register Entry *entryPtr = (Entry *) clientData;
    int result = TCL_OK;
    int length;
    char c;

    if (argc < 2) {
	Tcl_AppendResult(interp, "wrong # args: should be \"",
		argv[0], " option ?arg arg ...?\"", (char *) NULL);
	return TCL_ERROR;
    }
    Tk_Preserve((ClientData) entryPtr);
    c = argv[1][0];
    length = strlen(argv[1]);
    if ((c == 'c') && (strncmp(argv[1], "configure", length) == 0)
	    && (length >= 2)) {
	if (argc == 2) {
	    result = Tk_ConfigureInfo(interp, entryPtr->tkwin, configSpecs,
		    (char *) entryPtr, (char *) NULL, 0);
	} else if (argc == 3) {
	    result = Tk_ConfigureInfo(interp, entryPtr->tkwin, configSpecs,
		    (char *) entryPtr, argv[2], 0);
	} else {
	    result = ConfigureEntry(interp, entryPtr, argc-2, argv+2,
		    TK_CONFIG_ARGV_ONLY);
	}
    } else if ((c == 'c') && (strncmp(argv[1], "cursor", length) == 0)
	    && (length >= 2)) {
	if (argc != 3) {
	    Tcl_AppendResult(interp, "wrong # args: should be \"",
		    argv[0], " cursor pos\"",
		    (char *) NULL);
	    goto error;
	}
	if (GetEntryIndex(interp, entryPtr, argv[2], &entryPtr->cursorPos)
		!= TCL_OK) {
	    goto error;
	}
	EventuallyRedraw(entryPtr);
    } else if ((c == 'd') && (strncmp(argv[1], "delete", length) == 0)) {
	int first, last;

	if ((argc < 3) || (argc > 4)) {
	    Tcl_AppendResult(interp, "wrong # args: should be \"",
		    argv[0], " delete firstIndex ?lastIndex?\"",
		    (char *) NULL);
	    goto error;
	}
	if (GetEntryIndex(interp, entryPtr, argv[2], &first) != TCL_OK) {
	    goto error;
	}
	if (argc == 3) {
	    last = first;
	} else {
	    if (GetEntryIndex(interp, entryPtr, argv[3], &last) != TCL_OK) {
		goto error;
	    }
	}
	if ((last >= first) && (entryPtr->state == tkNormalUid)) {
	    DeleteChars(entryPtr, first, last+1-first);
	}
    } else if ((c == 'g') && (strncmp(argv[1], "get", length) == 0)) {
	if (argc != 2) {
	    Tcl_AppendResult(interp, "wrong # args: should be \"",
		    argv[0], " get\"", (char *) NULL);
	    goto error;
	}
	interp->result = entryPtr->string;
    } else if ((c == 'i') && (strncmp(argv[1], "index", length) == 0)
	    && (length >= 2)) {
	int index;

	if (argc != 3) {
	    Tcl_AppendResult(interp, "wrong # args: should be \"",
		    argv[0], " index string\"", (char *) NULL);
	    goto error;
	}
	if (GetEntryIndex(interp, entryPtr, argv[2], &index) != TCL_OK) {
	    goto error;
	}
	sprintf(interp->result, "%d", index);
    } else if ((c == 'i') && (strncmp(argv[1], "insert", length) == 0)
	    && (length >= 2)) {
	int index;

	if (argc != 4) {
	    Tcl_AppendResult(interp, "wrong # args: should be \"",
		    argv[0], " insert index text\"",
		    (char *) NULL);
	    goto error;
	}
	if (GetEntryIndex(interp, entryPtr, argv[2], &index) != TCL_OK) {
	    goto error;
	}
	if (entryPtr->state == tkNormalUid) {
	    InsertChars(entryPtr, index, argv[3]);
	}
    } else if ((c == 's') && (length >= 2)
	    && (strncmp(argv[1], "scan", length) == 0)) {
	int x;

	if (argc != 4) {
	    Tcl_AppendResult(interp, "wrong # args: should be \"",
		    argv[0], " scan mark|dragto x\"", (char *) NULL);
	    goto error;
	}
	if (Tcl_GetInt(interp, argv[3], &x) != TCL_OK) {
	    goto error;
	}
	if ((argv[2][0] == 'm')
		&& (strncmp(argv[2], "mark", strlen(argv[2])) == 0)) {
	    entryPtr->scanMarkX = x;
	    entryPtr->scanMarkIndex = entryPtr->leftIndex;
	} else if ((argv[2][0] == 'd')
		&& (strncmp(argv[2], "dragto", strlen(argv[2])) == 0)) {
	    EntryScanTo(entryPtr, x);
	} else {
	    Tcl_AppendResult(interp, "bad scan option \"", argv[2],
		    "\":  must be mark or dragto", (char *) NULL);
	    goto error;
	}
    } else if ((c == 's') && (length >= 2)
	    && (strncmp(argv[1], "select", length) == 0)) {
	int index;

	if (argc < 3) {
	    Tcl_AppendResult(interp, "too few args: should be \"",
		    argv[0], " select option ?index?\"", (char *) NULL);
	    goto error;
	}
	length = strlen(argv[2]);
	c = argv[2][0];
	if ((c == 'c') && (argv[2] != NULL)
		&& (strncmp(argv[2], "clear", length) == 0)) {
	    if (argc != 3) {
		Tcl_AppendResult(interp, "wrong # args: should be \"",
			argv[0], " select clear\"", (char *) NULL);
		goto error;
	    }
	    if (entryPtr->selectFirst != -1) {
		entryPtr->selectFirst = entryPtr->selectLast = -1;
		EventuallyRedraw(entryPtr);
	    }
	    goto done;
	}
	if (argc >= 4) {
	    if (GetEntryIndex(interp, entryPtr, argv[3], &index) != TCL_OK) {
		goto error;
	    }
	}
	if ((c == 'a') && (strncmp(argv[2], "adjust", length) == 0)) {
	    if (argc != 4) {
		Tcl_AppendResult(interp, "wrong # args: should be \"",
			argv[0], " select adjust index\"",
			(char *) NULL);
		goto error;
	    }
	    if (entryPtr->selectFirst >= 0) {
		if (index < (entryPtr->selectFirst + entryPtr->selectLast)/2) {
		    entryPtr->selectAnchor = entryPtr->selectLast + 1;
		} else {
		    entryPtr->selectAnchor = entryPtr->selectFirst;
		}
	    }
	    EntrySelectTo(entryPtr, index);
	} else if ((c == 'f') && (strncmp(argv[2], "from", length) == 0)) {
	    if (argc != 4) {
		Tcl_AppendResult(interp, "wrong # args: should be \"",
			argv[0], " select from index\"",
			(char *) NULL);
		goto error;
	    }
	    entryPtr->selectAnchor = index;
	} else if ((c == 't') && (strncmp(argv[2], "to", length) == 0)) {
	    if (argc != 4) {
		Tcl_AppendResult(interp, "wrong # args: should be \"",
			argv[0], " select to index\"",
			(char *) NULL);
		goto error;
	    }
	    EntrySelectTo(entryPtr, index);
	} else {
	    Tcl_AppendResult(interp, "bad select option \"", argv[2],
		    "\": must be adjust, clear, from, or to", (char *) NULL);
	    goto error;
	}
    } else if ((c == 'v') && (strncmp(argv[1], "view", length) == 0)) {
	int index;

	if (argc != 3) {
	    Tcl_AppendResult(interp, "wrong # args: should be \"",
		    argv[0], " view index\"", (char *) NULL);
	    goto error;
	}
	if (GetEntryIndex(interp, entryPtr, argv[2], &index) != TCL_OK) {
	    goto error;
	}
	if ((index >= entryPtr->numChars) && (index > 0)) {
	    index = entryPtr->numChars-1;
	}
	entryPtr->leftIndex = index;
	EventuallyRedraw(entryPtr);
	EntryUpdateScrollbar(entryPtr);
    } else {
	Tcl_AppendResult(interp, "bad option \"", argv[1],
		"\": must be configure, cursor, delete, get, index, ",
		"insert, scan, select, or view", (char *) NULL);
	goto error;
    }
    done:
    Tk_Release((ClientData) entryPtr);
    return result;

    error:
    Tk_Release((ClientData) entryPtr);
    return TCL_ERROR;
}

/*
 *----------------------------------------------------------------------
 *
 * DestroyEntry --
 *
 *	This procedure is invoked by Tk_EventuallyFree or Tk_Release
 *	to clean up the internal structure of an entry at a safe time
 *	(when no-one is using it anymore).
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Everything associated with the entry is freed up.
 *
 *----------------------------------------------------------------------
 */

static void
DestroyEntry(clientData)
    ClientData clientData;			/* Info about entry widget. */
{
    register Entry *entryPtr = (Entry *) clientData;

    ckfree(entryPtr->string);
    if (entryPtr->normalBorder != NULL) {
	Tk_Free3DBorder(entryPtr->normalBorder);
    }
    if (entryPtr->textVarName != NULL) {
	Tcl_UntraceVar(entryPtr->interp, entryPtr->textVarName,
		TCL_GLOBAL_ONLY|TCL_TRACE_WRITES|TCL_TRACE_UNSETS,
		EntryTextVarProc, (ClientData) entryPtr);
	ckfree(entryPtr->textVarName);
    }
    if (entryPtr->fontPtr != NULL) {
	Tk_FreeFontStruct(entryPtr->fontPtr);
    }
    if (entryPtr->fgColorPtr != NULL) {
	Tk_FreeColor(entryPtr->fgColorPtr);
    }
    if (entryPtr->textGC != None) {
	Tk_FreeGC(entryPtr->textGC);
    }
    if (entryPtr->selBorder != NULL) {
	Tk_Free3DBorder(entryPtr->selBorder);
    }
    if (entryPtr->selFgColorPtr != NULL) {
	Tk_FreeColor(entryPtr->selFgColorPtr);
    }
    if (entryPtr->selTextGC != None) {
	Tk_FreeGC(entryPtr->selTextGC);
    }
    if (entryPtr->cursorBorder != NULL) {
	Tk_Free3DBorder(entryPtr->cursorBorder);
    }
    if (entryPtr->cursorBlinkHandler != NULL) {
        Tk_DeleteTimerHandler(entryPtr->cursorBlinkHandler);
	entryPtr->cursorBlinkHandler = NULL;
    }
    if (entryPtr->cursor != None) {
	Tk_FreeCursor(entryPtr->cursor);
    }
    if (entryPtr->scrollCmd != NULL) {
        ckfree(entryPtr->scrollCmd);
    }
    ckfree((char *) entryPtr);
}

/*
 *----------------------------------------------------------------------
 *
 * ConfigureEntry --
 *
 *	This procedure is called to process an argv/argc list, plus
 *	the Tk option database, in order to configure (or reconfigure)
 *	an entry widget.
 *
 * Results:
 *	The return value is a standard Tcl result.  If TCL_ERROR is
 *	returned, then interp->result contains an error message.
 *
 * Side effects:
 *	Configuration information, such as colors, border width,
 *	etc. get set for entryPtr;  old resources get freed,
 *	if there were any.
 *
 *----------------------------------------------------------------------
 */

static int
ConfigureEntry(interp, entryPtr, argc, argv, flags)
    Tcl_Interp *interp;		/* Used for error reporting. */
    register Entry *entryPtr;	/* Information about widget;  may or may
				 * not already have values for some fields. */
    int argc;			/* Number of valid entries in argv. */
    char **argv;		/* Arguments. */
    int flags;			/* Flags to pass to Tk_ConfigureWidget. */
{
    XGCValues gcValues;
    GC new;
    int width, height, fontHeight, oldExport;

    /*
     * Eliminate any existing trace on a variable monitored by the entry.
     */

    if (entryPtr->textVarName != NULL) {
	Tcl_UntraceVar(interp, entryPtr->textVarName, 
		TCL_GLOBAL_ONLY|TCL_TRACE_WRITES|TCL_TRACE_UNSETS,
		EntryTextVarProc, (ClientData) entryPtr);
    }

    oldExport = entryPtr->exportSelection;
    if (Tk_ConfigureWidget(interp, entryPtr->tkwin, configSpecs,
	    argc, argv, (char *) entryPtr, flags) != TCL_OK) {
	return TCL_ERROR;
    }

    /*
     * If the entry is tied to the value of a variable, then set up
     * a trace on the variable's value, create the variable if it doesn't
     * exist, and set the entry's value from the variable's value.
     */

    if (entryPtr->textVarName != NULL) {
	char *value;

	value = Tcl_GetVar(interp, entryPtr->textVarName, TCL_GLOBAL_ONLY);
	if (value == NULL) {
	    Tcl_SetVar(interp, entryPtr->textVarName, entryPtr->string,
		    TCL_GLOBAL_ONLY);
	} else {
	    EntrySetValue(entryPtr, value);
	}
	Tcl_TraceVar(interp, entryPtr->textVarName,
		TCL_GLOBAL_ONLY|TCL_TRACE_WRITES|TCL_TRACE_UNSETS,
		EntryTextVarProc, (ClientData) entryPtr);
    }

    /*
     * A few other options also need special processing, such as parsing
     * the geometry and setting the background from a 3-D border.
     */

    if ((entryPtr->state != tkNormalUid)
	    && (entryPtr->state != tkDisabledUid)) {
	Tcl_AppendResult(interp, "bad state value \"", entryPtr->state,
		"\":  must be normal or disabled", (char *) NULL);
	entryPtr->state = tkNormalUid;
	return TCL_ERROR;
    }

    Tk_SetBackgroundFromBorder(entryPtr->tkwin, entryPtr->normalBorder);

    gcValues.foreground = entryPtr->fgColorPtr->pixel;
    gcValues.font = entryPtr->fontPtr->fid;
    gcValues.graphics_exposures = False;
    new = Tk_GetGC(entryPtr->tkwin, GCForeground|GCFont|GCGraphicsExposures,
	    &gcValues);
    if (entryPtr->textGC != None) {
	Tk_FreeGC(entryPtr->textGC);
    }
    entryPtr->textGC = new;

    gcValues.foreground = entryPtr->selFgColorPtr->pixel;
    gcValues.font = entryPtr->fontPtr->fid;
    new = Tk_GetGC(entryPtr->tkwin, GCForeground|GCFont, &gcValues);
    if (entryPtr->selTextGC != None) {
	Tk_FreeGC(entryPtr->selTextGC);
    }
    entryPtr->selTextGC = new;

    if (entryPtr->cursorWidth > 2*entryPtr->fontPtr->min_bounds.width) {
	entryPtr->cursorWidth = 2*entryPtr->fontPtr->min_bounds.width;
	if (entryPtr->cursorWidth == 0) {
	    entryPtr->cursorWidth = 2;
	}
    }
    if (entryPtr->cursorBorderWidth > entryPtr->cursorWidth/2) {
	entryPtr->cursorBorderWidth = entryPtr->cursorWidth/2;
    }

    /*
     * Restart the cursor timing sequence in case the on-time or off-time
     * just changed.
     */

    if (entryPtr->flags & GOT_FOCUS) {
	EntryFocusProc((ClientData) entryPtr, 1);
    }

    /*
     * Claim the selection if we've suddenly started exporting it.
     */

    if (entryPtr->exportSelection && (!oldExport)
	    && (entryPtr->selectFirst != -1)) {
	Tk_OwnSelection(entryPtr->tkwin, EntryLostSelection,
		(ClientData) entryPtr);
    }

    /*
     * Register the desired geometry for the window, and arrange for
     * the window to be redisplayed.
     */

    fontHeight = entryPtr->fontPtr->ascent + entryPtr->fontPtr->descent;
    entryPtr->avgWidth = XTextWidth(entryPtr->fontPtr, "0", 1);
    width = entryPtr->prefWidth*entryPtr->avgWidth + (15*fontHeight)/10;
    height = fontHeight + 2*entryPtr->borderWidth + 2;
    Tk_GeometryRequest(entryPtr->tkwin, width, height);
    Tk_SetInternalBorder(entryPtr->tkwin, entryPtr->borderWidth);
    if (entryPtr->relief != TK_RELIEF_FLAT) {
	entryPtr->offset = entryPtr->borderWidth;
    } else {
	entryPtr->offset = 0;
    }
    EventuallyRedraw(entryPtr);
    EntryUpdateScrollbar(entryPtr);
    return TCL_OK;
}

/*
 *--------------------------------------------------------------
 *
 * DisplayEntry --
 *
 *	This procedure redraws the contents of an entry window.
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
DisplayEntry(clientData)
    ClientData clientData;	/* Information about window. */
{
    register Entry *entryPtr = (Entry *) clientData;
    register Tk_Window tkwin = entryPtr->tkwin;
    int startX, baseY, selStartX, selEndX, index, cursorX;
    int xBound, count;
    Pixmap pixmap;

    entryPtr->flags &= ~REDRAW_PENDING;
    if ((entryPtr->tkwin == NULL) || !Tk_IsMapped(tkwin)) {
	return;
    }

    /*
     * In order to avoid screen flashes, this procedure redraws the
     * textual area of the entry into off-screen memory, then copies
     * it back on-screen in a single operation.  This means there's
     * no point in time where the on-screen image has been cleared.
     */

    pixmap = XCreatePixmap(Tk_Display(tkwin), Tk_WindowId(tkwin),
	    Tk_Width(tkwin), Tk_Height(tkwin),
	    Tk_DefaultDepth(Tk_Screen(tkwin)));

    /*
     * Compute x-coordinate of the "leftIndex" character, plus limit
     * of visible x-coordinates (actually, pixel just after last visible
     * one), plus vertical position of baseline of text.
     */

    startX = entryPtr->offset;
    xBound = Tk_Width(tkwin) - entryPtr->offset;
    baseY = (Tk_Height(tkwin) + entryPtr->fontPtr->ascent
	    - entryPtr->fontPtr->descent)/2;

    /*
     * Draw the background in three layers.  From bottom to top the
     * layers are:  normal background, selection background, and
     * insertion cursor background.
     */

    Tk_Fill3DRectangle(Tk_Display(tkwin), pixmap, entryPtr->normalBorder,
	    0, 0, Tk_Width(tkwin), Tk_Height(tkwin), 0, TK_RELIEF_FLAT);

    if (entryPtr->selectLast >= entryPtr->leftIndex) {
	if (entryPtr->selectFirst <= entryPtr->leftIndex) {
	    selStartX = startX;
	    index = entryPtr->leftIndex;
	} else {
	    (void) TkMeasureChars(entryPtr->fontPtr,
		    entryPtr->string+entryPtr->leftIndex,
		    entryPtr->selectFirst - entryPtr->leftIndex, startX,
		    xBound, TK_PARTIAL_OK|TK_NEWLINES_NOT_SPECIAL, &selStartX);
	    index = entryPtr->selectFirst;
	}
	if (selStartX < xBound) {
	    (void) TkMeasureChars(entryPtr->fontPtr,
		    entryPtr->string + index, entryPtr->selectLast +1 - index,
		    selStartX, xBound, TK_PARTIAL_OK|TK_NEWLINES_NOT_SPECIAL,
		    &selEndX);
	    Tk_Fill3DRectangle(Tk_Display(tkwin), pixmap, entryPtr->selBorder,
		    selStartX - entryPtr->selBorderWidth,
		    baseY - entryPtr->fontPtr->ascent
			    - entryPtr->selBorderWidth,
		    (selEndX - selStartX) + 2*entryPtr->selBorderWidth,
		    entryPtr->fontPtr->ascent + entryPtr->fontPtr->descent
			    + 2*entryPtr->selBorderWidth,
		    entryPtr->selBorderWidth, TK_RELIEF_RAISED);
	} else {
	    selEndX = xBound;
	}
    }

    /*
     * Draw a special background for the insertion cursor, overriding
     * even the selection background.  As a special workaround to keep the
     * cursor visible on mono displays, write background in the cursor
     * area (instead of nothing) when the cursor isn't on.  Otherwise
     * the selection would hide the cursor.
     */

    if ((entryPtr->cursorPos >= entryPtr->leftIndex)
	    && (entryPtr->state == tkNormalUid)
	    && (entryPtr->flags & GOT_FOCUS)) {
	(void) TkMeasureChars(entryPtr->fontPtr,
		entryPtr->string + entryPtr->leftIndex,
		entryPtr->cursorPos - entryPtr->leftIndex, startX,
		xBound, TK_PARTIAL_OK|TK_NEWLINES_NOT_SPECIAL, &cursorX);
	if (cursorX < xBound) {
	    if (entryPtr->flags & CURSOR_ON) {
		Tk_Fill3DRectangle(Tk_Display(tkwin), pixmap,
			entryPtr->cursorBorder,
			cursorX - (entryPtr->cursorWidth)/2,
			baseY - entryPtr->fontPtr->ascent,
			entryPtr->cursorWidth,
			entryPtr->fontPtr->ascent + entryPtr->fontPtr->descent,
			entryPtr->cursorBorderWidth, TK_RELIEF_RAISED);
	    } else if (Tk_DefaultDepth(Tk_Screen(tkwin)) == 1) {
		Tk_Fill3DRectangle(Tk_Display(tkwin), pixmap,
			entryPtr->normalBorder,
			cursorX - (entryPtr->cursorWidth)/2,
			baseY - entryPtr->fontPtr->ascent,
			entryPtr->cursorWidth,
			entryPtr->fontPtr->ascent + entryPtr->fontPtr->descent,
			0, TK_RELIEF_FLAT);
	    }
	}
    }

    /*
     * Draw the text in three pieces:  first the piece to the left of
     * the selection, then the selection, then the piece to the right
     * of the selection.
     */

    if (entryPtr->selectLast < entryPtr->leftIndex) {
	TkDisplayChars(Tk_Display(tkwin), pixmap, entryPtr->textGC,
		entryPtr->fontPtr, entryPtr->string + entryPtr->leftIndex,
		entryPtr->numChars - entryPtr->leftIndex, startX, baseY,
		TK_NEWLINES_NOT_SPECIAL);
    } else {
	count = entryPtr->selectFirst - entryPtr->leftIndex;
	if (count > 0) {
	    TkDisplayChars(Tk_Display(tkwin), pixmap, entryPtr->textGC,
		    entryPtr->fontPtr, entryPtr->string + entryPtr->leftIndex,
		    count, startX, baseY, TK_NEWLINES_NOT_SPECIAL);
	    index = entryPtr->selectFirst;
	} else {
	    index = entryPtr->leftIndex;
	}
	count = entryPtr->selectLast + 1 - index;
	if ((selStartX < xBound) && (count > 0)) {
	    TkDisplayChars(Tk_Display(tkwin), pixmap, entryPtr->selTextGC,
		    entryPtr->fontPtr, entryPtr->string + index, count,
		    selStartX, baseY, TK_NEWLINES_NOT_SPECIAL);
	}
	count = entryPtr->numChars - entryPtr->selectLast - 1;
	if ((selEndX < xBound) && (count > 0)) {
	    TkDisplayChars(Tk_Display(tkwin), pixmap, entryPtr->textGC,
		    entryPtr->fontPtr,
		    entryPtr->string + entryPtr->selectLast + 1,
		    count, selEndX, baseY, TK_NEWLINES_NOT_SPECIAL);
	}
    }

    /*
     * Draw the border last, so it will overwrite any text that extends
     * past the viewable part of the window.
     */

    if (entryPtr->relief != TK_RELIEF_FLAT) {
	Tk_Draw3DRectangle(Tk_Display(tkwin), pixmap,
		entryPtr->normalBorder, 0, 0, Tk_Width(tkwin),
		Tk_Height(tkwin), entryPtr->borderWidth,
		entryPtr->relief);
    }

    /*
     * Everything's been redisplayed;  now copy the pixmap onto the screen
     * and free up the pixmap.
     */

    XCopyArea(Tk_Display(tkwin), pixmap, Tk_WindowId(tkwin), entryPtr->textGC,
	    0, 0, Tk_Width(tkwin), Tk_Height(tkwin), 0, 0);
    XFreePixmap(Tk_Display(tkwin), pixmap);
    entryPtr->flags &= ~BORDER_NEEDED;
}

/*
 *----------------------------------------------------------------------
 *
 * InsertChars --
 *
 *	Add new characters to an entry widget.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	New information gets added to entryPtr;  it will be redisplayed
 *	soon, but not necessarily immediately.
 *
 *----------------------------------------------------------------------
 */

static void
InsertChars(entryPtr, index, string)
    register Entry *entryPtr;	/* Entry that is to get the new
				 * elements. */
    int index;			/* Add the new elements before this
				 * element. */
    char *string;		/* New characters to add (NULL-terminated
				 * string). */
{
    int length;
    char *new;

    length = strlen(string);
    if (length == 0) {
	return;
    }
    new = (char *) ckalloc((unsigned) (entryPtr->numChars + length + 1));
    strncpy(new, entryPtr->string, index);
    strcpy(new+index, string);
    strcpy(new+index+length, entryPtr->string+index);
    ckfree(entryPtr->string);
    entryPtr->string = new;
    entryPtr->numChars += length;

    /*
     * Inserting characters invalidates all indexes into the string.
     * Touch up the indexes so that they still refer to the same
     * characters (at new positions).
     */

    if (entryPtr->selectFirst >= index) {
	entryPtr->selectFirst += length;
    }
    if (entryPtr->selectLast >= index) {
	entryPtr->selectLast += length;
    }
    if (entryPtr->selectAnchor >= index) {
	entryPtr->selectAnchor += length;
    }
    if (entryPtr->leftIndex > index) {
	entryPtr->leftIndex += length;
    }
    if (entryPtr->cursorPos >= index) {
	entryPtr->cursorPos += length;
    }

    if (entryPtr->textVarName != NULL) {
	Tcl_SetVar(entryPtr->interp, entryPtr->textVarName, entryPtr->string,
		TCL_GLOBAL_ONLY);
    }
    EventuallyRedraw(entryPtr);
    EntryUpdateScrollbar(entryPtr);
}

/*
 *----------------------------------------------------------------------
 *
 * DeleteChars --
 *
 *	Remove one or more characters from an entry widget.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Memory gets freed, the entry gets modified and (eventually)
 *	redisplayed.
 *
 *----------------------------------------------------------------------
 */

static void
DeleteChars(entryPtr, index, count)
    register Entry *entryPtr;	/* Entry widget to modify. */
    int index;			/* Index of first character to delete. */
    int count;			/* How many characters to delete. */
{
    char *new;

    if ((index + count) > entryPtr->numChars) {
	count = entryPtr->numChars - index;
    }
    if (count <= 0) {
	return;
    }

    new = (char *) ckalloc((unsigned) (entryPtr->numChars + 1 - count));
    strncpy(new, entryPtr->string, index);
    strcpy(new+index, entryPtr->string+index+count);
    ckfree(entryPtr->string);
    entryPtr->string = new;
    entryPtr->numChars -= count;

    /*
     * Deleting characters results in the remaining characters being
     * renumbered.  Update the various indexes into the string to reflect
     * this change.
     */
    if (entryPtr->selectFirst >= index) {
	if (entryPtr->selectFirst >= (index+count)) {
	    entryPtr->selectFirst -= count;
	} else {
	    entryPtr->selectFirst = index;
	}
    }
    if (entryPtr->selectLast >= index) {
	if (entryPtr->selectLast >= (index+count)) {
	    entryPtr->selectLast -= count;
	} else {
	    entryPtr->selectLast = index-1;
	}
    }
    if (entryPtr->selectLast < entryPtr->selectFirst) {
	entryPtr->selectFirst = entryPtr->selectLast = -1;
    }
    if (entryPtr->selectAnchor >= index) {
	if (entryPtr->selectAnchor >= (index+count)) {
	    entryPtr->selectAnchor -= count;
	} else {
	    entryPtr->selectAnchor = index;
	}
    }
    if (entryPtr->leftIndex > index) {
	if (entryPtr->leftIndex >= (index+count)) {
	    entryPtr->leftIndex -= count;
	} else {
	    entryPtr->leftIndex = index;
	}
    }
    if (entryPtr->cursorPos >= index) {
	if (entryPtr->cursorPos >= (index+count)) {
	    entryPtr->cursorPos -= count;
	} else {
	    entryPtr->cursorPos = index;
	}
    }

    if (entryPtr->textVarName != NULL) {
	Tcl_SetVar(entryPtr->interp, entryPtr->textVarName, entryPtr->string,
		TCL_GLOBAL_ONLY);
    }
    EventuallyRedraw(entryPtr);
    EntryUpdateScrollbar(entryPtr);
}

/*
 *----------------------------------------------------------------------
 *
 * EntrySetValue --
 *
 *	Replace the contents of a text entry with a given value.  This
 *	procedure is invoked when updating the entry from the entry's
 *	associated variable.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The string displayed in the entry will change.  Any selection
 *	in the entry is lost and the insertion point gets set to the
 *	end of the entry.  Note: this procedure does *not* update the
 *	entry's associated variable, since that could result in an
 *	infinite loop.
 *
 *----------------------------------------------------------------------
 */

static void
EntrySetValue(entryPtr, value)
    register Entry *entryPtr;		/* Entry whose value is to be
					 * changed. */
    char *value;			/* New text to display in entry. */
{
    ckfree(entryPtr->string);
    entryPtr->numChars = strlen(value);
    entryPtr->string = (char *) ckalloc((unsigned) (entryPtr->numChars + 1));
    strcpy(entryPtr->string, value);
    entryPtr->selectFirst = entryPtr->selectLast = -1;
    entryPtr->leftIndex = 0;
    entryPtr->cursorPos = entryPtr->numChars;

    EventuallyRedraw(entryPtr);
    EntryUpdateScrollbar(entryPtr);
}

/*
 *--------------------------------------------------------------
 *
 * EntryEventProc --
 *
 *	This procedure is invoked by the Tk dispatcher for various
 *	events on entryes.
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
EntryEventProc(clientData, eventPtr)
    ClientData clientData;	/* Information about window. */
    XEvent *eventPtr;		/* Information about event. */
{
    Entry *entryPtr = (Entry *) clientData;
    if (eventPtr->type == Expose) {
	EventuallyRedraw(entryPtr);
	entryPtr->flags |= BORDER_NEEDED;
    } else if (eventPtr->type == DestroyNotify) {
	Tcl_DeleteCommand(entryPtr->interp, Tk_PathName(entryPtr->tkwin));
	entryPtr->tkwin = NULL;
	if (entryPtr->flags & REDRAW_PENDING) {
	    Tk_CancelIdleCall(DisplayEntry, (ClientData) entryPtr);
	}
	Tk_EventuallyFree((ClientData) entryPtr, DestroyEntry);
    } else if (eventPtr->type == ConfigureNotify) {
	Tk_Preserve((ClientData) entryPtr);
	EventuallyRedraw(entryPtr);
	EntryUpdateScrollbar(entryPtr);
	Tk_Release((ClientData) entryPtr);
    }
}

/*
 *--------------------------------------------------------------
 *
 * GetEntryIndex --
 *
 *	Parse an index into an entry and return either its value
 *	or an error.
 *
 * Results:
 *	A standard Tcl result.  If all went well, then *indexPtr is
 *	filled in with the index (into entryPtr) corresponding to
 *	string.  The index value is guaranteed to lie between 0 and
 *	the number of characters in the string, inclusive.  If an
 *	error occurs then an error message is left in interp->result.
 *
 * Side effects:
 *	None.
 *
 *--------------------------------------------------------------
 */

static int
GetEntryIndex(interp, entryPtr, string, indexPtr)
    Tcl_Interp *interp;		/* For error messages. */
    Entry *entryPtr;		/* Entry for which the index is being
				 * specified. */
    char *string;		/* Specifies character in entryPtr. */
    int *indexPtr;		/* Where to store converted index. */
{
    int length;

    length = strlen(string);

    if (string[0] == 'e') {
	if (strncmp(string, "end", length) == 0) {
	    *indexPtr = entryPtr->numChars;
	} else {
	    badIndex:

	    /*
	     * Some of the paths here leave messages in interp->result,
	     * so we have to clear it out before storing our own message.
	     */

	    Tcl_SetResult(interp, (char *) NULL, TCL_STATIC);
	    Tcl_AppendResult(interp, "bad entry index \"", string,
		    "\"", (char *) NULL);
	    return TCL_ERROR;
	}
    } else if (string[0] == 'c') {
	if (strncmp(string, "cursor", length) == 0) {
	    *indexPtr = entryPtr->cursorPos;
	} else {
	    goto badIndex;
	}
    } else if (string[0] == 's') {
	if (entryPtr->selectFirst == -1) {
	    interp->result = "selection isn't in entry";
	    return TCL_ERROR;
	}
	if (length < 5) {
	    goto badIndex;
	}
	if (strncmp(string, "sel.first", length) == 0) {
	    *indexPtr = entryPtr->selectFirst;
	} else if (strncmp(string, "sel.last", length) == 0) {
	    *indexPtr = entryPtr->selectLast;
	} else {
	    goto badIndex;
	}
    } else if (string[0] == '@') {
	int x, dummy;

	if (Tcl_GetInt(interp, string+1, &x) != TCL_OK) {
	    goto badIndex;
	}
	if (entryPtr->numChars == 0) {
	    *indexPtr = 0;
	} else {
	    *indexPtr = entryPtr->leftIndex + TkMeasureChars(entryPtr->fontPtr,
		    entryPtr->string + entryPtr->leftIndex,
		    entryPtr->numChars - entryPtr->leftIndex,
		    entryPtr->offset, x, TK_NEWLINES_NOT_SPECIAL, &dummy);
	}
    } else {
	if (Tcl_GetInt(interp, string, indexPtr) != TCL_OK) {
	    goto badIndex;
	}
	if (*indexPtr < 0){
	    *indexPtr = 0;
	} else if (*indexPtr > entryPtr->numChars) {
	    *indexPtr = entryPtr->numChars;
	}
    }
    return TCL_OK;
}

/*
 *----------------------------------------------------------------------
 *
 * EntryScanTo --
 *
 *	Given a y-coordinate (presumably of the curent mouse location)
 *	drag the view in the window to implement the scan operation.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The view in the window may change.
 *
 *----------------------------------------------------------------------
 */

static void
EntryScanTo(entryPtr, x)
    register Entry *entryPtr;		/* Information about widget. */
    int x;				/* X-coordinate to use for scan
					 * operation. */
{
    int newLeftIndex;

    /*
     * Compute new leftIndex for entry by amplifying the difference
     * between the current position and the place where the scan
     * started (the "mark" position).  If we run off the left or right
     * side of the entry, then reset the mark point so that the current
     * position continues to correspond to the edge of the window.
     * This means that the picture will start dragging as soon as the
     * mouse reverses direction (without this reset, might have to slide
     * mouse a long ways back before the picture starts moving again).
     */

    newLeftIndex = entryPtr->scanMarkIndex
	    - (10*(x - entryPtr->scanMarkX))/entryPtr->avgWidth;
    if (newLeftIndex >= entryPtr->numChars) {
	newLeftIndex = entryPtr->scanMarkIndex = entryPtr->numChars-1;
	entryPtr->scanMarkX = x;
    }
    if (newLeftIndex < 0) {
	newLeftIndex = entryPtr->scanMarkIndex = 0;
	entryPtr->scanMarkX = x;
    } 
    if (newLeftIndex != entryPtr->leftIndex) {
	entryPtr->leftIndex = newLeftIndex;
	EventuallyRedraw(entryPtr);
	EntryUpdateScrollbar(entryPtr);
    }
}

/*
 *----------------------------------------------------------------------
 *
 * EntrySelectTo --
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
EntrySelectTo(entryPtr, index)
    register Entry *entryPtr;		/* Information about widget. */
    int index;				/* Index of element that is to
					 * become the "other" end of the
					 * selection. */
{
    int newFirst, newLast;

    /*
     * Grab the selection if we don't own it already.
     */

    if ((entryPtr->selectFirst == -1) && (entryPtr->exportSelection)) {
	Tk_OwnSelection(entryPtr->tkwin, EntryLostSelection,
		(ClientData) entryPtr);
    }

    if (index < 0) {
	index = 0;
    }
    if (index >= entryPtr->numChars) {
	index = entryPtr->numChars-1;
    }
    if (entryPtr->selectAnchor > entryPtr->numChars) {
	entryPtr->selectAnchor = entryPtr->numChars;
    }
    if (entryPtr->selectAnchor <= index) {
	newFirst = entryPtr->selectAnchor;
	newLast = index;
    } else {
	newFirst = index;
	newLast = entryPtr->selectAnchor - 1;
	if (newLast < 0) {
	    newFirst = newLast = -1;
	}
    }
    if ((entryPtr->selectFirst == newFirst)
	    && (entryPtr->selectLast == newLast)) {
	return;
    }
    entryPtr->selectFirst = newFirst;
    entryPtr->selectLast = newLast;
    EventuallyRedraw(entryPtr);
}

/*
 *----------------------------------------------------------------------
 *
 * EntryFetchSelection --
 *
 *	This procedure is called back by Tk when the selection is
 *	requested by someone.  It returns part or all of the selection
 *	in a buffer provided by the caller.
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
 *----------------------------------------------------------------------
 */

static int
EntryFetchSelection(clientData, offset, buffer, maxBytes)
    ClientData clientData;		/* Information about entry widget. */
    int offset;				/* Offset within selection of first
					 * character to be returned. */
    char *buffer;			/* Location in which to place
					 * selection. */
    int maxBytes;			/* Maximum number of bytes to place
					 * at buffer, not including terminating
					 * NULL character. */
{
    Entry *entryPtr = (Entry *) clientData;
    int count;

    if ((entryPtr->selectFirst < 0) || !(entryPtr->exportSelection)) {
	return -1;
    }
    count = entryPtr->selectLast + 1 - entryPtr->selectFirst - offset;
    if (count > maxBytes) {
	count = maxBytes;
    }
    if (count <= 0) {
	return 0;
    }
    strncpy(buffer, entryPtr->string + entryPtr->selectFirst + offset, count);
    buffer[count] = '\0';
    return count;
}

/*
 *----------------------------------------------------------------------
 *
 * EntryLostSelection --
 *
 *	This procedure is called back by Tk when the selection is
 *	grabbed away from an entry widget.
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
EntryLostSelection(clientData)
    ClientData clientData;		/* Information about entry widget. */
{
    Entry *entryPtr = (Entry *) clientData;

    if ((entryPtr->selectFirst != -1) && entryPtr->exportSelection) {
	entryPtr->selectFirst = -1;
	entryPtr->selectLast = -1;
	EventuallyRedraw(entryPtr);
    }
}

/*
 *----------------------------------------------------------------------
 *
 * EventuallyRedraw --
 *
 *	Ensure that an entry is eventually redrawn on the display.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Information gets redisplayed.  Right now we don't do selective
 *	redisplays:  the whole window will be redrawn.  This doesn't
 *	seem to hurt performance noticeably, but if it does then this
 *	could be changed.
 *
 *----------------------------------------------------------------------
 */

static void
EventuallyRedraw(entryPtr)
    register Entry *entryPtr;		/* Information about widget. */
{
    if ((entryPtr->tkwin == NULL) || !Tk_IsMapped(entryPtr->tkwin)) {
	return;
    }

    /*
     * Right now we don't do selective redisplays:  the whole window
     * will be redrawn.  This doesn't seem to hurt performance noticeably,
     * but if it does then this could be changed.
     */

    if (!(entryPtr->flags & REDRAW_PENDING)) {
	entryPtr->flags |= REDRAW_PENDING;
	Tk_DoWhenIdle(DisplayEntry, (ClientData) entryPtr);
    }
}

/*
 *----------------------------------------------------------------------
 *
 * EntryUpdateScrollbar --
 *
 *	This procedure is invoked whenever information has changed in
 *	an entry in a way that would invalidate a scrollbar display.
 *	If there is an associated scrollbar, then this command updates
 *	it by invoking a Tcl command.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	A Tcl command is invoked, and an additional command may be
 *	invoked to process errors in the command.
 *
 *----------------------------------------------------------------------
 */

static void
EntryUpdateScrollbar(entryPtr)
    register Entry *entryPtr;		/* Information about widget. */
{
    char args[100];
    int result, last, charsInWindow, endX;

    if (entryPtr->scrollCmd == NULL) {
	return;
    }

    /*
     * The most painful part here is guessing how many characters
     * actually fit in the window.  This is only an estimate in the
     * case where the window isn't completely filled with characters.
     */

    charsInWindow = TkMeasureChars(entryPtr->fontPtr,
	    entryPtr->string + entryPtr->leftIndex,
	    entryPtr->numChars - entryPtr->leftIndex, entryPtr->offset,
	    Tk_Width(entryPtr->tkwin),
	    TK_AT_LEAST_ONE|TK_NEWLINES_NOT_SPECIAL, &endX);
    if (charsInWindow == 0) {
	last = entryPtr->leftIndex;
    } else {
	last = entryPtr->leftIndex + charsInWindow - 1;
    }
    if (endX < Tk_Width(entryPtr->tkwin)) {
	charsInWindow += (Tk_Width(entryPtr->tkwin) - endX)/entryPtr->avgWidth;
    }
    sprintf(args, " %d %d %d %d", entryPtr->numChars, charsInWindow,
	    entryPtr->leftIndex, last);
    result = Tcl_VarEval(entryPtr->interp, entryPtr->scrollCmd, args,
	    (char *) NULL);
    if (result != TCL_OK) {
	TkBindError(entryPtr->interp);
    }
    Tcl_SetResult(entryPtr->interp, (char *) NULL, TCL_STATIC);
}

/*
 *----------------------------------------------------------------------
 *
 * EntryBlinkProc --
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
EntryBlinkProc(clientData)
    ClientData clientData;	/* Pointer to record describing entry. */
{
    register Entry *entryPtr = (Entry *) clientData;

    if (!(entryPtr->flags & GOT_FOCUS) || (entryPtr->cursorOffTime == 0)) {
	return;
    }
    if (entryPtr->flags & CURSOR_ON) {
	entryPtr->flags &= ~CURSOR_ON;
	entryPtr->cursorBlinkHandler = Tk_CreateTimerHandler(
		entryPtr->cursorOffTime, EntryBlinkProc, (ClientData) entryPtr);
    } else {
	entryPtr->flags |= CURSOR_ON;
	entryPtr->cursorBlinkHandler = Tk_CreateTimerHandler(
		entryPtr->cursorOnTime, EntryBlinkProc, (ClientData) entryPtr);
    }
    EventuallyRedraw(entryPtr);
}

/*
 *----------------------------------------------------------------------
 *
 * EntryFocusProc --
 *
 *	This procedure is called whenever the entry gets or loses the
 *	input focus.  It's also called whenever the window is reconfigured
 *	while it has the focus.
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
EntryFocusProc(clientData, gotFocus)
    ClientData clientData;	/* Pointer to structure describing entry. */
    int gotFocus;		/* 1 means window is getting focus, 0 means
				 * it's losing it. */
{
    register Entry *entryPtr = (Entry *) clientData;

    if (entryPtr->cursorBlinkHandler != NULL) {
      Tk_DeleteTimerHandler(entryPtr->cursorBlinkHandler);
      entryPtr->cursorBlinkHandler = NULL;
    }
    if (gotFocus) {
	entryPtr->flags |= GOT_FOCUS | CURSOR_ON;
	if (entryPtr->cursorOffTime != 0) {
	    entryPtr->cursorBlinkHandler = Tk_CreateTimerHandler(
		    entryPtr->cursorOnTime, EntryBlinkProc,
		    (ClientData) entryPtr);
	}
    } else {
	entryPtr->flags &= ~(GOT_FOCUS | CURSOR_ON);
	entryPtr->cursorBlinkHandler = (Tk_TimerToken) NULL;
    }
    EventuallyRedraw(entryPtr);
}

/*
 *--------------------------------------------------------------
 *
 * EntryTextVarProc --
 *
 *	This procedure is invoked when someone changes the variable
 *	whose contents are to be displayed in an entry.
 *
 * Results:
 *	NULL is always returned.
 *
 * Side effects:
 *	The text displayed in the entry will change to match the
 *	variable.
 *
 *--------------------------------------------------------------
 */

	/* ARGSUSED */
static char *
EntryTextVarProc(clientData, interp, name1, name2, flags)
    ClientData clientData;	/* Information about button. */
    Tcl_Interp *interp;		/* Interpreter containing variable. */
    char *name1;		/* Name of variable. */
    char *name2;		/* Second part of variable name. */
    int flags;			/* Information about what happened. */
{
    register Entry *entryPtr = (Entry *) clientData;
    char *value;

    /*
     * If the variable is unset, then immediately recreate it unless
     * the whole interpreter is going away.
     */

    if (flags & TCL_TRACE_UNSETS) {
	if ((flags & TCL_TRACE_DESTROYED) && !(flags & TCL_INTERP_DESTROYED)) {
	    Tcl_SetVar2(interp, name1, name2, entryPtr->string,
		    flags & TCL_GLOBAL_ONLY);
	    Tcl_TraceVar2(interp, name1, name2,
		    TCL_GLOBAL_ONLY|TCL_TRACE_WRITES|TCL_TRACE_UNSETS,
		    EntryTextVarProc, clientData);
	}
	return (char *) NULL;
    }

    /*
     * Update the entry's text with the value of the variable, unless
     * the entry already has that value (this happens when the variable
     * changes value because we changed it because someone typed in
     * the entry).
     */

    value = Tcl_GetVar2(interp, name1, name2, flags & TCL_GLOBAL_ONLY);
    if (value == NULL) {
	value = "";
    }
    if (strcmp(value, entryPtr->string) != 0) {
	EntrySetValue(entryPtr, value);
    }
    return (char *) NULL;
}
