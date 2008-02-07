/* 
 * tkText.c --
 *
 *	This module provides a big chunk of the implementation of
 *	multi-line editable text widgets for Tk.  Among other things,
 *	it provides the Tcl command interfaces to text widgets and
 *	the display code.  The B-tree representation of text is
 *	implemented elsewhere.
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
static char rcsid[] = "$Header: /user6/ouster/wish/RCS/tkText.c,v 1.23 92/08/14 14:45:44 ouster Exp $ SPRITE (Berkeley)";
#endif

#include "default.h"
#include "tkconfig.h"
#include "tk.h"
#include "tktext.h"

/*
 * Information used to parse text configuration options:
 */

static Tk_ConfigSpec configSpecs[] = {
    {TK_CONFIG_BORDER, "-background", "background", "Background",
	DEF_TEXT_BG_COLOR, Tk_Offset(TkText, border), TK_CONFIG_COLOR_ONLY},
    {TK_CONFIG_BORDER, "-background", "background", "Background",
	DEF_TEXT_BG_MONO, Tk_Offset(TkText, border), TK_CONFIG_MONO_ONLY},
    {TK_CONFIG_SYNONYM, "-bd", "borderWidth", (char *) NULL,
	(char *) NULL, 0, 0},
    {TK_CONFIG_SYNONYM, "-bg", "background", (char *) NULL,
	(char *) NULL, 0, 0},
    {TK_CONFIG_PIXELS, "-borderwidth", "borderWidth", "BorderWidth",
	DEF_TEXT_BORDER_WIDTH, Tk_Offset(TkText, borderWidth), 0},
    {TK_CONFIG_ACTIVE_CURSOR, "-cursor", "cursor", "Cursor",
	DEF_TEXT_CURSOR, Tk_Offset(TkText, cursor), TK_CONFIG_NULL_OK},
    {TK_CONFIG_BOOLEAN, "-exportselection", "exportSelection",
	"ExportSelection", DEF_TEXT_EXPORT_SELECTION,
	Tk_Offset(TkText, exportSelection), 0},
    {TK_CONFIG_SYNONYM, "-fg", "foreground", (char *) NULL,
	(char *) NULL, 0, 0},
    {TK_CONFIG_FONT, "-font", "font", "Font",
	DEF_TEXT_FONT, Tk_Offset(TkText, fontPtr), 0},
    {TK_CONFIG_COLOR, "-foreground", "foreground", "Foreground",
	DEF_TEXT_FG, Tk_Offset(TkText, fgColor), 0},
    {TK_CONFIG_INT, "-height", "height", "Height",
	DEF_TEXT_HEIGHT, Tk_Offset(TkText, height), 0},
    {TK_CONFIG_BORDER, "-insertbackground", "insertBackground", "Foreground",
	DEF_TEXT_INSERT_BG, Tk_Offset(TkText, insertBorder), 0},
    {TK_CONFIG_PIXELS, "-insertborderwidth", "insertBorderWidth", "BorderWidth",
	DEF_TEXT_INSERT_BD_COLOR, Tk_Offset(TkText, insertBorderWidth),
	TK_CONFIG_COLOR_ONLY},
    {TK_CONFIG_PIXELS, "-insertborderwidth", "insertBorderWidth", "BorderWidth",
	DEF_TEXT_INSERT_BD_MONO, Tk_Offset(TkText, insertBorderWidth),
	TK_CONFIG_MONO_ONLY},
    {TK_CONFIG_INT, "-insertofftime", "insertOffTime", "OffTime",
	DEF_TEXT_INSERT_OFF_TIME, Tk_Offset(TkText, insertOffTime), 0},
    {TK_CONFIG_INT, "-insertontime", "insertOnTime", "OnTime",
	DEF_TEXT_INSERT_ON_TIME, Tk_Offset(TkText, insertOnTime), 0},
    {TK_CONFIG_PIXELS, "-insertwidth", "insertWidth", "InsertWidth",
	DEF_TEXT_INSERT_WIDTH, Tk_Offset(TkText, insertWidth), 0},
    {TK_CONFIG_PIXELS, "-padx", "padX", "Pad",
	DEF_TEXT_PADX, Tk_Offset(TkText, padX), 0},
    {TK_CONFIG_PIXELS, "-pady", "padY", "Pad",
	DEF_TEXT_PADY, Tk_Offset(TkText, padY), 0},
    {TK_CONFIG_RELIEF, "-relief", "relief", "Relief",
	DEF_TEXT_RELIEF, Tk_Offset(TkText, relief), 0},
    {TK_CONFIG_BORDER, "-selectbackground", "selectBackground", "Foreground",
	DEF_ENTRY_SELECT_COLOR, Tk_Offset(TkText, selBorder),
	TK_CONFIG_COLOR_ONLY},
    {TK_CONFIG_BORDER, "-selectbackground", "selectBackground", "Foreground",
	DEF_TEXT_SELECT_MONO, Tk_Offset(TkText, selBorder),
	TK_CONFIG_MONO_ONLY},
    {TK_CONFIG_PIXELS, "-selectborderwidth", "selectBorderWidth", "BorderWidth",
	DEF_TEXT_SELECT_BD_COLOR, Tk_Offset(TkText, selBorderWidth),
	TK_CONFIG_COLOR_ONLY},
    {TK_CONFIG_PIXELS, "-selectborderwidth", "selectBorderWidth", "BorderWidth",
	DEF_TEXT_SELECT_BD_MONO, Tk_Offset(TkText, selBorderWidth),
	TK_CONFIG_MONO_ONLY},
    {TK_CONFIG_COLOR, "-selectforeground", "selectForeground", "Background",
	DEF_TEXT_SELECT_FG_COLOR, Tk_Offset(TkText, selFgColorPtr),
	TK_CONFIG_COLOR_ONLY},
    {TK_CONFIG_COLOR, "-selectforeground", "selectForeground", "Background",
	DEF_TEXT_SELECT_FG_MONO, Tk_Offset(TkText, selFgColorPtr),
	TK_CONFIG_MONO_ONLY},
    {TK_CONFIG_BOOLEAN, "-setgrid", "setGrid", "SetGrid",
	DEF_TEXT_SET_GRID, Tk_Offset(TkText, setGrid), 0},
    {TK_CONFIG_UID, "-state", "state", "State",
	DEF_TEXT_STATE, Tk_Offset(TkText, state), 0},
    {TK_CONFIG_INT, "-width", "width", "Width",
	DEF_TEXT_WIDTH, Tk_Offset(TkText, width), 0},
    {TK_CONFIG_UID, "-wrap", "wrap", "Wrap",
	DEF_TEXT_WRAP, Tk_Offset(TkText, wrapMode), 0},
    {TK_CONFIG_STRING, "-yscrollcommand", "yScrollCommand", "ScrollCommand",
	DEF_TEXT_YSCROLL_COMMAND, Tk_Offset(TkText, yScrollCmd),
	TK_CONFIG_NULL_OK},
    {TK_CONFIG_END, (char *) NULL, (char *) NULL, (char *) NULL,
	(char *) NULL, 0, 0}
};

/*
 * The following definition specifies the maximum number of characters
 * needed in a string to hold a position specifier.
 */

#define POS_CHARS 30

/*
 * Tk_Uid's used to represent text states:
 */

Tk_Uid tkTextCharUid = NULL;
Tk_Uid tkTextDisabledUid = NULL;
Tk_Uid tkTextNoneUid = NULL;
Tk_Uid tkTextNormalUid = NULL;
Tk_Uid tkTextWordUid = NULL;

/*
 * Forward declarations for procedures defined later in this file:
 */

static int		ConfigureText _ANSI_ARGS_((Tcl_Interp *interp,
			    TkText *textPtr, int argc, char **argv, int flags));
static void		DeleteChars _ANSI_ARGS_((TkText *textPtr, int line1,
			    int ch1, int line2, int ch2));
static void		DestroyText _ANSI_ARGS_((ClientData clientData));
static void		InsertChars _ANSI_ARGS_((TkText *textPtr, int line,
			    int ch, char *string));
static void		TextBlinkProc _ANSI_ARGS_((ClientData clientData));
static void		TextEventProc _ANSI_ARGS_((ClientData clientData,
			    XEvent *eventPtr));
static int		TextFetchSelection _ANSI_ARGS_((ClientData clientData,
			    int offset, char *buffer, int maxBytes));
static void		TextFocusProc _ANSI_ARGS_((ClientData clientData,
			    int gotFocus));
static int		TextMarkCmd _ANSI_ARGS_((TkText *textPtr,
			    Tcl_Interp *interp, int argc, char **argv));
static int		TextScanCmd _ANSI_ARGS_((TkText *textPtr,
			    Tcl_Interp *interp, int argc, char **argv));
static int		TextWidgetCmd _ANSI_ARGS_((ClientData clientData,
			    Tcl_Interp *interp, int argc, char **argv));

/*
 *--------------------------------------------------------------
 *
 * Tk_TextCmd --
 *
 *	This procedure is invoked to process the "text" Tcl command.
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

int
Tk_TextCmd(clientData, interp, argc, argv)
    ClientData clientData;	/* Main window associated with
				 * interpreter. */
    Tcl_Interp *interp;		/* Current interpreter. */
    int argc;			/* Number of arguments. */
    char **argv;		/* Argument strings. */
{
    Tk_Window tkwin = (Tk_Window) clientData;
    Tk_Window new;
    register TkText *textPtr;

    if (argc < 2) {
	Tcl_AppendResult(interp, "wrong # args: should be \"",
		argv[0], " pathName ?options?\"", (char *) NULL);
	return TCL_ERROR;
    }

    /*
     * Perform once-only initialization:
     */

    if (tkTextNormalUid == NULL) {
	tkTextCharUid = Tk_GetUid("char");
	tkTextDisabledUid = Tk_GetUid("disabled");
	tkTextNoneUid = Tk_GetUid("none");
	tkTextNormalUid = Tk_GetUid("normal");
	tkTextWordUid = Tk_GetUid("word");
    }

    /*
     * Create the window.
     */

    new = Tk_CreateWindowFromPath(interp, tkwin, argv[1], (char *) NULL);
    if (new == NULL) {
	return TCL_ERROR;
    }

    textPtr = (TkText *) ckalloc(sizeof(TkText));
    textPtr->tkwin = new;
    textPtr->interp = interp;
    textPtr->tree = TkBTreeCreate();
    Tcl_InitHashTable(&textPtr->tagTable, TCL_STRING_KEYS);
    textPtr->numTags = 0;
    Tcl_InitHashTable(&textPtr->markTable, TCL_STRING_KEYS);
    textPtr->state = tkTextNormalUid;
    textPtr->border = NULL;
    textPtr->cursor = None;
    textPtr->fgColor = NULL;
    textPtr->fontPtr = NULL;
    textPtr->prevWidth = Tk_Width(new);
    textPtr->prevHeight = Tk_Height(new);
    textPtr->topLinePtr = NULL;
// Moved down so flags were set right.
//    TkTextCreateDInfo(textPtr);
//    TkTextSetView(textPtr, 0, 0);
    textPtr->selBorder = NULL;
    textPtr->selFgColorPtr = NULL;
    textPtr->exportSelection = 1;
    textPtr->selOffset = -1;
    textPtr->insertAnnotPtr = NULL;
    textPtr->insertBorder = NULL;
    textPtr->insertBlinkHandler = (Tk_TimerToken) NULL;
    textPtr->bindingTable = NULL;
    textPtr->pickEvent.type = LeaveNotify;
    textPtr->yScrollCmd = NULL;
    textPtr->scanMarkLine = 0;
    textPtr->scanMarkY = 0;
    textPtr->flags = 0;
    textPtr->updateTimerToken = 0; // Added by Don to optimize rapid updates.
    TkTextCreateDInfo(textPtr);
    TkTextSetView(textPtr, 0, 0);

    /*
     * Create the "sel" tag and the "current" and "insert" marks.
     */

    textPtr->selTagPtr = TkTextCreateTag(textPtr, "sel");
    textPtr->selTagPtr->relief = TK_RELIEF_RAISED;
    textPtr->currentAnnotPtr = TkTextSetMark(textPtr, "current", 0, 0);
    textPtr->insertAnnotPtr = TkTextSetMark(textPtr, "insert", 0, 0);

    Tk_SetClass(new, "Text");
    Tk_CreateEventHandler(textPtr->tkwin, ExposureMask|StructureNotifyMask,
	    TextEventProc, (ClientData) textPtr);
    Tk_CreateEventHandler(textPtr->tkwin, KeyPressMask|KeyReleaseMask
	    |ButtonPressMask|ButtonReleaseMask|EnterWindowMask
	    |LeaveWindowMask|PointerMotionMask, TkTextBindProc,
	    (ClientData) textPtr);
    Tk_CreateSelHandler(textPtr->tkwin, XA_STRING, TextFetchSelection,
	    (ClientData) textPtr, XA_STRING);
    Tcl_CreateCommand(interp, Tk_PathName(textPtr->tkwin),
	    TextWidgetCmd, (ClientData) textPtr, (void (*)()) NULL);
    if (ConfigureText(interp, textPtr, argc-2, argv+2, 0) != TCL_OK) {
	Tk_DestroyWindow(textPtr->tkwin);
	return TCL_ERROR;
    }
    Tk_CreateFocusHandler(textPtr->tkwin, TextFocusProc, (ClientData) textPtr);
    interp->result = Tk_PathName(textPtr->tkwin);

    return TCL_OK;
}

/*
 *--------------------------------------------------------------
 *
 * TextWidgetCmd --
 *
 *	This procedure is invoked to process the Tcl command
 *	that corresponds to a text widget.  See the user
 *	documentation for details on what it does.
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
TextWidgetCmd(clientData, interp, argc, argv)
    ClientData clientData;	/* Information about text widget. */
    Tcl_Interp *interp;		/* Current interpreter. */
    int argc;			/* Number of arguments. */
    char **argv;		/* Argument strings. */
{
    register TkText *textPtr = (TkText *) clientData;
    int result = TCL_OK;
    int length;
    char c;
    int line1, line2, ch1, ch2;

    if (argc < 2) {
	Tcl_AppendResult(interp, "wrong # args: should be \"",
		argv[0], " option ?arg arg ...?\"", (char *) NULL);
	return TCL_ERROR;
    }
    Tk_Preserve((ClientData) textPtr);
    c = argv[1][0];
    length = strlen(argv[1]);
    if ((c == 'c') && (strncmp(argv[1], "compare", length) == 0)
	    && (length >= 3)) {
	int less, equal, greater, value;
	char *p;

	if (argc != 5) {
	    Tcl_AppendResult(interp, "wrong # args: should be \"",
		    argv[0], " compare index1 op index2\"", (char *) NULL);
	    result = TCL_ERROR;
	    goto done;
	}
	if ((TkTextGetIndex(interp, textPtr, argv[2], &line1, &ch1) != TCL_OK)
		|| (TkTextGetIndex(interp, textPtr, argv[4], &line2, &ch2)
		!= TCL_OK)) {
	    result = TCL_ERROR;
	    goto done;
	}
	less = equal = greater = 0;
	if (line1 < line2) {
	    less = 1;
	} else if (line1 > line2) {
	    greater = 1;
	} else {
	    if (ch1 < ch2) {
		less = 1;
	    } else if (ch1 > ch2) {
		greater = 1;
	    } else {
		equal = 1;
	    }
	}
	p = argv[3];
	if (p[0] == '<') {
		value = less;
	    if ((p[1] == '=') && (p[2] == 0)) {
		value = less || equal;
	    } else if (p[1] != 0) {
		compareError:
		Tcl_AppendResult(interp, "bad comparison operator \"",
			argv[3], "\": must be <, <=, ==, >=, >, or !=",
			(char *) NULL);
		result = TCL_ERROR;
		goto done;
	    }
	} else if (p[0] == '>') {
		value = greater;
	    if ((p[1] == '=') && (p[2] == 0)) {
		value = greater || equal;
	    } else if (p[1] != 0) {
		goto compareError;
	    }
	} else if ((p[0] == '=') && (p[1] == '=') && (p[2] == 0)) {
	    value = equal;
	} else if ((p[0] == '!') && (p[1] == '=') && (p[2] == 0)) {
	    value = !equal;
	} else {
	    goto compareError;
	}
	interp->result = (value) ? "1" : "0";
    } else if ((c == 'c') && (strncmp(argv[1], "configure", length) == 0)
	    && (length >= 3)) {
	if (argc == 2) {
	    result = Tk_ConfigureInfo(interp, textPtr->tkwin, configSpecs,
		    (char *) textPtr, (char *) NULL, 0);
	} else if (argc == 3) {
	    result = Tk_ConfigureInfo(interp, textPtr->tkwin, configSpecs,
		    (char *) textPtr, argv[2], 0);
	} else {
	    result = ConfigureText(interp, textPtr, argc-2, argv+2,
		    TK_CONFIG_ARGV_ONLY);
	}
    } else if ((c == 'd') && (strncmp(argv[1], "debug", length) == 0)
	    && (length >= 3)) {
	if (argc > 3) {
	    Tcl_AppendResult(interp, "wrong # args: should be \"",
		    argv[0], " debug ?on|off?\"", (char *) NULL);
	    result = TCL_ERROR;
	    goto done;
	}
	if (argc == 2) {
	    interp->result = (tkBTreeDebug) ? "on" : "off";
	} else {
	    if (Tcl_GetBoolean(interp, argv[2], &tkBTreeDebug) != TCL_OK) {
		result = TCL_ERROR;
		goto done;
	    }
	}
    } else if ((c == 'd') && (strncmp(argv[1], "delete", length) == 0)
	    && (length >= 3)) {
	if ((argc != 3) && (argc != 4)) {
	    Tcl_AppendResult(interp, "wrong # args: should be \"",
		    argv[0], " delete index1 ?index2?\"", (char *) NULL);
	    result = TCL_ERROR;
	    goto done;
	}
	if (TkTextGetIndex(interp, textPtr, argv[2], &line1, &ch1) != TCL_OK) {
	    result = TCL_ERROR;
	    goto done;
	}
	if (argc == 3) {
	    line2 = line1;
	    ch2 = ch1+1;
	} else if (TkTextGetIndex(interp, textPtr, argv[3], &line2, &ch2)
		!= TCL_OK) {
	    result = TCL_ERROR;
	    goto done;
	}
	if (textPtr->state == tkTextNormalUid) {
	    DeleteChars(textPtr, line1, ch1, line2, ch2);
	}
    } else if ((c == 'g') && (strncmp(argv[1], "get", length) == 0)) {
	register TkTextLine *linePtr;

	if ((argc != 3) && (argc != 4)) {
	    Tcl_AppendResult(interp, "wrong # args: should be \"",
		    argv[0], " get index1 ?index2?\"", (char *) NULL);
	    result = TCL_ERROR;
	    goto done;
	}
	if (TkTextGetIndex(interp, textPtr, argv[2], &line1, &ch1) != TCL_OK) {
	    result = TCL_ERROR;
	    goto done;
	}
	if (argc == 3) {
	    line2 = line1;
	    ch2 = ch1+1;
	} else if (TkTextGetIndex(interp, textPtr, argv[3], &line2, &ch2)
		!= TCL_OK) {
	    result = TCL_ERROR;
	    goto done;
	}
	if (line1 < 0) {
	    line1 = 0;
	    ch1 = 0;
	}
	for (linePtr = TkBTreeFindLine(textPtr->tree, line1);
		(linePtr != NULL) && (line1 <= line2);
		linePtr = TkBTreeNextLine(linePtr), line1++, ch1 = 0) {
	    int savedChar, last;

	    if (line1 == line2) {
		last = ch2;
		if (last > linePtr->numBytes) {
		    last = linePtr->numBytes;
		}
	    } else {
		last = linePtr->numBytes;
	    }
	    if (ch1 >= last) {
		continue;
	    }
	    savedChar = linePtr->bytes[last];
	    linePtr->bytes[last] = 0;
	    Tcl_AppendResult(interp, linePtr->bytes+ch1, (char *) NULL);
	    linePtr->bytes[last] = savedChar;
	}
    } else if ((c == 'i') && (strncmp(argv[1], "index", length) == 0)
	    && (length >= 3)) {
	if (argc != 3) {
	    Tcl_AppendResult(interp, "wrong # args: should be \"",
		    argv[0], " index index\"",
		    (char *) NULL);
	    result = TCL_ERROR;
	    goto done;
	}
	if (TkTextGetIndex(interp, textPtr, argv[2], &line1, &ch1) != TCL_OK) {
	    result = TCL_ERROR;
	    goto done;
	}
	TkTextPrintIndex(line1, ch1, interp->result);
    } else if ((c == 'i') && (strncmp(argv[1], "insert", length) == 0)
	    && (length >= 3)) {
	if (argc != 4) {
	    Tcl_AppendResult(interp, "wrong # args: should be \"",
		    argv[0], " insert index chars ?chars ...?\"",
		    (char *) NULL);
	    result = TCL_ERROR;
	    goto done;
	}
	if (TkTextGetIndex(interp, textPtr, argv[2], &line1, &ch1) != TCL_OK) {
	    result = TCL_ERROR;
	    goto done;
	}
	if (textPtr->state == tkTextNormalUid) {
	    InsertChars(textPtr, line1, ch1, argv[3]);
	}
    } else if ((c == 'm') && (strncmp(argv[1], "mark", length) == 0)) {
	result = TextMarkCmd(textPtr, interp, argc, argv);
    } else if ((c == 's') && (strcmp(argv[1], "scan") == 0)) {
	result = TextScanCmd(textPtr, interp, argc, argv);
    } else if ((c == 't') && (strcmp(argv[1], "tag") == 0)) {
	result = TkTextTagCmd(textPtr, interp, argc, argv);
    } else if ((c == 'y') && (strncmp(argv[1], "yview", length) == 0)) {
	int numLines, pickPlace;

	if (argc < 3) {
	    yviewSyntax:
	    Tcl_AppendResult(interp, "wrong # args: should be \"",
		    argv[0], " yview ?-pickplace? lineNum|index\"",
		    (char *) NULL);
	    result = TCL_ERROR;
	    goto done;
	}
	pickPlace = 0;
	if (argv[2][0] == '-') {
	    int switchLength;

	    switchLength = strlen(argv[2]);
	    if ((switchLength >= 2)
		    && (strncmp(argv[2], "-pickplace", switchLength) == 0)) {
		pickPlace = 1;
	    }
	}
	if ((pickPlace+3) != argc) {
	    goto yviewSyntax;
	}
	if (Tcl_GetInt(interp, argv[2+pickPlace], &line1) != TCL_OK) {
	    Tcl_ResetResult(interp);
	    if (TkTextGetIndex(interp, textPtr, argv[2+pickPlace],
		    &line1, &ch1) != TCL_OK) {
		result = TCL_ERROR;
		goto done;
	    }
	}
	numLines = TkBTreeNumLines(textPtr->tree);
	if (line1 >= numLines) {
	    line1 = numLines-1;
	}
	if (line1 < 0) {
	    line1 = 0;
	}
	TkTextSetView(textPtr, line1, pickPlace);
    } else {
	Tcl_AppendResult(interp, "bad option \"", argv[1],
		"\":  must be compare, configure, debug, delete, get, ",
		"index, insert, mark, scan, tag, or yview",
		(char *) NULL);
	result = TCL_ERROR;
    }

    done:
    Tk_Release((ClientData) textPtr);
    return result;
}

/*
 *----------------------------------------------------------------------
 *
 * DestroyText --
 *
 *	This procedure is invoked by Tk_EventuallyFree or Tk_Release
 *	to clean up the internal structure of a text at a safe time
 *	(when no-one is using it anymore).
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Everything associated with the text is freed up.
 *
 *----------------------------------------------------------------------
 */

static void
DestroyText(clientData)
    ClientData clientData;	/* Info about text widget. */
{
    register TkText *textPtr = (TkText *) clientData;
    Tcl_HashSearch search;
    Tcl_HashEntry *hPtr;
    TkTextTag *tagPtr;

    TkBTreeDestroy(textPtr->tree);
    for (hPtr = Tcl_FirstHashEntry(&textPtr->tagTable, &search);
	    hPtr != NULL; hPtr = Tcl_NextHashEntry(&search)) {
	tagPtr = (TkTextTag *) Tcl_GetHashValue(hPtr);
	TkTextFreeTag(tagPtr);
    }
    Tcl_DeleteHashTable(&textPtr->tagTable);
    for (hPtr = Tcl_FirstHashEntry(&textPtr->markTable, &search);
	    hPtr != NULL; hPtr = Tcl_NextHashEntry(&search)) {
	ckfree((char *) Tcl_GetHashValue(hPtr));
    }
    Tcl_DeleteHashTable(&textPtr->markTable);
    if (textPtr->border != NULL) {
	Tk_Free3DBorder(textPtr->border);
    }
    if (textPtr->cursor != None) {
	Tk_FreeCursor(textPtr->cursor);
    }
    if (textPtr->fgColor != NULL) {
	Tk_FreeColor(textPtr->fgColor);
    }
    if (textPtr->fontPtr != NULL) {
	Tk_FreeFontStruct(textPtr->fontPtr);
    }
    TkTextFreeDInfo(textPtr);

    /*
     * NOTE: do NOT free up selBorder or selFgColorPtr:  they are
     * duplicates of information in the "sel" tag, which was freed
     * up as part of deleting the tags above.
     */

    if (textPtr->insertBorder != NULL) {
	Tk_Free3DBorder(textPtr->insertBorder);
    }
    if (textPtr->insertBlinkHandler != NULL) {
	Tk_DeleteTimerHandler(textPtr->insertBlinkHandler);
    }
    if (textPtr->updateTimerToken != NULL) {
	Tk_DeleteTimerHandler(textPtr->updateTimerToken);
	textPtr->updateTimerToken = 0;
    }
    if (textPtr->bindingTable != NULL) {
	Tk_DeleteBindingTable(textPtr->bindingTable);
    }
    if (textPtr->yScrollCmd != NULL) {
	ckfree(textPtr->yScrollCmd);
    }
    ckfree((char *) textPtr);
}

/*
 *----------------------------------------------------------------------
 *
 * ConfigureText --
 *
 *	This procedure is called to process an argv/argc list, plus
 *	the Tk option database, in order to configure (or
 *	reconfigure) a text widget.
 *
 * Results:
 *	The return value is a standard Tcl result.  If TCL_ERROR is
 *	returned, then interp->result contains an error message.
 *
 * Side effects:
 *	Configuration information, such as text string, colors, font,
 *	etc. get set for textPtr;  old resources get freed, if there
 *	were any.
 *
 *----------------------------------------------------------------------
 */

static int
ConfigureText(interp, textPtr, argc, argv, flags)
    Tcl_Interp *interp;		/* Used for error reporting. */
    register TkText *textPtr;	/* Information about widget;  may or may
				 * not already have values for some fields. */
    int argc;			/* Number of valid entries in argv. */
    char **argv;		/* Arguments. */
    int flags;			/* Flags to pass to Tk_ConfigureWidget. */
{
    int oldExport = textPtr->exportSelection;
    int charWidth, charHeight;

    if (Tk_ConfigureWidget(interp, textPtr->tkwin, configSpecs,
	    argc, argv, (char *) textPtr, flags) != TCL_OK) {
	return TCL_ERROR;
    }

    /*
     * A few other options also need special processing, such as parsing
     * the geometry and setting the background from a 3-D border.
     */

    if ((textPtr->state != tkTextNormalUid)
	    && (textPtr->state != tkTextDisabledUid)) {
	Tcl_AppendResult(interp, "bad state value \"", textPtr->state,
		"\":  must be normal or disabled", (char *) NULL);
	textPtr->state = tkTextNormalUid;
	return TCL_ERROR;
    }

    if ((textPtr->wrapMode != tkTextCharUid)
	    && (textPtr->wrapMode != tkTextNoneUid)
	    && (textPtr->wrapMode != tkTextWordUid)) {
	Tcl_AppendResult(interp, "bad wrap mode \"", textPtr->state,
		"\":  must be char, none, or word", (char *) NULL);
	textPtr->wrapMode = tkTextCharUid;
	return TCL_ERROR;
    }

    Tk_SetBackgroundFromBorder(textPtr->tkwin, textPtr->border);
    Tk_SetInternalBorder(textPtr->tkwin, textPtr->borderWidth);
    Tk_GeometryRequest(textPtr->tkwin, 200, 100);

    /*
     * Make sure that configuration options are properly mirrored
     * between the widget record and the "sel" tags.  NOTE: we don't
     * have to free up information during the mirroring;  old
     * information was freed when it was replaced in the widget
     * record.
     */

    textPtr->selTagPtr->border = textPtr->selBorder;
    textPtr->selTagPtr->borderWidth = textPtr->selBorderWidth;
    textPtr->selTagPtr->fgColor = textPtr->selFgColorPtr;

    /*
     * Claim the selection if we've suddenly started exporting it and there
     * are tagged characters.
     */

    if (textPtr->exportSelection && (!oldExport)) {
	TkTextSearch search;

	TkBTreeStartSearch(textPtr->tree, 0, 0, TkBTreeNumLines(textPtr->tree),
		0, textPtr->selTagPtr, &search);
	if (TkBTreeNextTag(&search)) {
	    Tk_OwnSelection(textPtr->tkwin, TkTextLostSelection,
		    (ClientData) textPtr);
	    textPtr->flags |= GOT_SELECTION;
	}
    }

    /*
     * Register the desired geometry for the window, and arrange for
     * the window to be redisplayed.
     */

    if (textPtr->width <= 0) {
	textPtr->width = 1;
    }
    if (textPtr->height <= 0) {
	textPtr->height = 1;
    }
    charWidth = XTextWidth(textPtr->fontPtr, "0", 1);
    charHeight = (textPtr->fontPtr->ascent + textPtr->fontPtr->descent);
    Tk_GeometryRequest(textPtr->tkwin,
	    textPtr->width * charWidth + 2*textPtr->borderWidth
		    + 2*textPtr->padX,
	    textPtr->height * charHeight + 2*textPtr->borderWidth
		    + 2*textPtr->padX);
    Tk_SetInternalBorder(textPtr->tkwin, textPtr->borderWidth);
    if (textPtr->setGrid) {
	Tk_SetGrid(textPtr->tkwin, textPtr->width, textPtr->height,
		charWidth, charHeight);
    }

    TkTextRelayoutWindow(textPtr);
    return TCL_OK;
}

/*
 *--------------------------------------------------------------
 *
 * TextEventProc --
 *
 *	This procedure is invoked by the Tk dispatcher on
 *	structure changes to a text.  For texts with 3D
 *	borders, this procedure is also invoked for exposures.
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
TextEventProc(clientData, eventPtr)
    ClientData clientData;	/* Information about window. */
    register XEvent *eventPtr;	/* Information about event. */
{
    register TkText *textPtr = (TkText *) clientData;

    if (eventPtr->type == Expose) {
	TkTextRedrawRegion(textPtr, eventPtr->xexpose.x,
		eventPtr->xexpose.y, eventPtr->xexpose.width,
		eventPtr->xexpose.height);
    } else if (eventPtr->type == ConfigureNotify) {
	if ((textPtr->prevWidth != Tk_Width(textPtr->tkwin))
		|| (textPtr->prevHeight != Tk_Height(textPtr->tkwin))) {
	    TkTextRelayoutWindow(textPtr);
	}
    } else if (eventPtr->type == DestroyNotify) {
	Tcl_DeleteCommand(textPtr->interp, Tk_PathName(textPtr->tkwin));
	textPtr->tkwin = NULL;
	Tk_EventuallyFree((ClientData) textPtr, DestroyText);
    }
}

/*
 *----------------------------------------------------------------------
 *
 * InsertChars --
 *
 *	This procedure implements most of the functionality of the
 *	"insert" widget command.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The characters in "string" get added to the text just before
 *	the character indicated by "line" and "ch".
 *
 *----------------------------------------------------------------------
 */

static void
InsertChars(textPtr, line, ch, string)
    TkText *textPtr;		/* Overall information about text widget. */
    int line, ch;		/* Identifies character just before which
				 * new information is to be inserted. */
    char *string;		/* Null-terminated string containing new
				 * information to add to text. */
{
    register TkTextLine *linePtr;

    /*
     * Locate the line where the insertion will occur.
     */

    linePtr = TkTextRoundIndex(textPtr, &line, &ch);

    /*
     * Notify the display module that lines are about to change, then do
     * the insertion.
     */

    TkTextLinesChanged(textPtr, line, line);
    TkBTreeInsertChars(textPtr->tree, linePtr, ch, string);

    /*
     * If the line containing the insertion point was textPtr->topLinePtr,
     * we must reset this pointer since the line structure was re-allocated.
     */

    if (linePtr == textPtr->topLinePtr) {
	TkTextSetView(textPtr, line, 0);
    }

    /*
     * Invalidate any selection retrievals in progress.
     */

    textPtr->selOffset = -1;
}

/*
 *----------------------------------------------------------------------
 *
 * DeleteChars --
 *
 *	This procedure implements most of the functionality of the
 *	"delete" widget command.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

static void
DeleteChars(textPtr, line1, ch1, line2, ch2)
    TkText *textPtr;		/* Overall information about text widget. */
    int line1, ch1;		/* Position of first character to delete. */
    int line2, ch2;		/* Position of character just after last
				 * one to delete. */
{
    register TkTextLine *line1Ptr, *line2Ptr;
    int numLines, topLine;

    /*
     * The loop below is needed because a LeaveNotify event may be
     * generated on the current charcter if it's about to be deleted.
     * If this happens, then the bindings that trigger could modify
     * the text, invalidating the range information computed here.
     * So, go back and recompute all the range information after
     * synthesizing a leave event.
     */

    while (1) {

	/*
	 * Locate the starting and ending lines for the deletion and adjust
	 * the endpoints if necessary to ensure that they are within valid
	 * ranges.  Adjust the deletion range if necessary to ensure that the
	 * text (and each invidiual line) always ends in a newline.
	 */
    
	numLines = TkBTreeNumLines(textPtr->tree);
	line1Ptr = TkTextRoundIndex(textPtr, &line1, &ch1);
	if (line2 < 0) {
	    return;
	} else if (line2 >= numLines) {
	    line2 = numLines-1;
	    line2Ptr = TkBTreeFindLine(textPtr->tree, line2);
	    ch2 = line2Ptr->numBytes;
	} else {
	    line2Ptr = TkBTreeFindLine(textPtr->tree, line2);
	    if (ch2 < 0) {
		ch2 = 0;
	    }
	}
    
	/*
	 * If the deletion range ends after the last character of a line,
	 * do one of three things:
	 *
	 * (a) if line2Ptr isn't the last line of the text, just adjust the
	 *     ending point to be just before the 0th character of the next
	 *     line.
	 * (b) if ch1 is at the beginning of a line, then adjust line1Ptr and
	 *     ch1 to point just after the last character of the previous line.
	 * (c) otherwise, adjust ch2 so the final newline isn't deleted.
	 */
    
	if (ch2 >= line2Ptr->numBytes) {
	    if (line2 < (numLines-1)) {
		line2++;
		line2Ptr = TkBTreeNextLine(line2Ptr);
		ch2 = 0;
	    } else {
		ch2 = line2Ptr->numBytes-1;
		if ((ch1 == 0) && (line1 > 0)) {
		    line1--;
		    line1Ptr = TkBTreeFindLine(textPtr->tree, line1);
		    ch1 = line1Ptr->numBytes;
		    ch2 = line2Ptr->numBytes;
		} else {
		    ch2 = line2Ptr->numBytes-1;
		}
	    }
	}

	if ((line1 > line2) || ((line1 == line2) && (ch1 >= ch2))) {
	    return;
	}

	/*
	 * If the current character is within the range being deleted,
	 * unpick it and synthesize a leave event for its tags, then
	 * go back and recompute the range ends.
	 */

	if (!(textPtr->flags & IN_CURRENT)) {
	    break;
	}
	if ((textPtr->currentAnnotPtr->linePtr == line1Ptr)
		&& (textPtr->currentAnnotPtr->ch < ch1)) {
	    break;
	}
	if ((textPtr->currentAnnotPtr->linePtr == line2Ptr)
		&& (textPtr->currentAnnotPtr->ch >= ch2)) {
	    break;
	}
	if (line2 > (line1+1)) {
	    int currentLine;

	    currentLine = TkBTreeLineIndex(textPtr->currentAnnotPtr->linePtr);
	    if ((currentLine <= line1) || (currentLine >= line2)) {
		break;
	    }
	}
	TkTextUnpickCurrent(textPtr);
    }

    /*
     * Tell the display what's about to happen so it can discard
     * obsolete display information, then do the deletion.  Also,
     * check to see if textPtr->topLinePtr is in the range of
     * characters deleted.  If so, call the display module to reset
     * it after doing the deletion.
     */

    topLine = TkBTreeLineIndex(textPtr->topLinePtr);
    TkTextLinesChanged(textPtr, line1, line2);
    TkBTreeDeleteChars(textPtr->tree, line1Ptr, ch1, line2Ptr, ch2);
    if ((topLine >= line1) && (topLine <= line2)) {
	numLines = TkBTreeNumLines(textPtr->tree);
	TkTextSetView(textPtr, (line1 > (numLines-1)) ? (numLines-1) : line1,
		0);
    }

    /*
     * Invalidate any selection retrievals in progress.
     */

    textPtr->selOffset = -1;
}

/*
 *----------------------------------------------------------------------
 *
 * TextFetchSelection --
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
TextFetchSelection(clientData, offset, buffer, maxBytes)
    ClientData clientData;		/* Information about text widget. */
    int offset;				/* Offset within selection of first
					 * character to be returned. */
    char *buffer;			/* Location in which to place
					 * selection. */
    int maxBytes;			/* Maximum number of bytes to place
					 * at buffer, not including terminating
					 * NULL character. */
{
    register TkText *textPtr = (TkText *) clientData;
    register TkTextLine *linePtr;
    int count, chunkSize;
    TkTextSearch search;

    if (!textPtr->exportSelection) {
	return -1;
    }

    /*
     * Find the beginning of the next range of selected text.  Note:  if
     * the selection is being retrieved in multiple pieces (offset != 0)
     * and some modification has been made to the text that affects the
     * selection (textPtr->selOffset != offset) then reject the selection
     * request (make 'em start over again).
     */

    if (offset == 0) {
	textPtr->selLine = 0;
	textPtr->selCh = 0;
	textPtr->selOffset = 0;
    } else if (textPtr->selOffset != offset) {
	return 0;
    }
    TkBTreeStartSearch(textPtr->tree, textPtr->selLine, textPtr->selCh+1,
	    TkBTreeNumLines(textPtr->tree), 0, textPtr->selTagPtr, &search);
    if (!TkBTreeCharTagged(search.linePtr, textPtr->selCh,
	    textPtr->selTagPtr)) {
	if (!TkBTreeNextTag(&search)) {
	    if (offset == 0) {
		return -1;
	    } else {
		return 0;
	    }
	}
	textPtr->selLine = search.line1;
	textPtr->selCh = search.ch1;
    }

    /*
     * Each iteration through the outer loop below scans one selected range.
     * Each iteration through the nested loop scans one line in the
     * selected range.
     */

    count = 0;
    while (1) {
	linePtr = search.linePtr;

	/*
	 * Find the end of the current range of selected text.
	 */

	if (!TkBTreeNextTag(&search)) {
	    panic("TextFetchSelection couldn't find end of range");
	}

	/*
	 * Copy information from text lines into the buffer until
	 * either we run out of space in the buffer or we get to
	 * the end of this range of text.
	 */

	while (1) {
	    chunkSize = ((linePtr == search.linePtr) ? search.ch1
		    : linePtr->numBytes) - textPtr->selCh;
	    if (chunkSize > maxBytes) {
		chunkSize = maxBytes;
	    }
	    memcpy((VOID *) buffer, (VOID *) (linePtr->bytes + textPtr->selCh),
		    chunkSize);
	    buffer += chunkSize;
	    maxBytes -= chunkSize;
	    count += chunkSize;
	    textPtr->selOffset += chunkSize;
	    if (maxBytes == 0) {
		textPtr->selCh += chunkSize;
		goto done;
	    }
	    if (linePtr == search.linePtr) {
		break;
	    }
	    textPtr->selCh = 0;
	    textPtr->selLine++;
	    linePtr = TkBTreeNextLine(linePtr);
	}

	/*
	 * Find the beginning of the next range of selected text.
	 */

	if (!TkBTreeNextTag(&search)) {
	    break;
	}
	textPtr->selLine = search.line1;
	textPtr->selCh = search.ch1;
    }

    done:
    *buffer = 0;
    return count;
}

/*
 *----------------------------------------------------------------------
 *
 * TkTextLostSelection --
 *
 *	This procedure is called back by Tk when the selection is
 *	grabbed away from a text widget.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The "sel" tag is cleared from the window.
 *
 *----------------------------------------------------------------------
 */

void
TkTextLostSelection(clientData)
    ClientData clientData;		/* Information about text widget. */
{
    register TkText *textPtr = (TkText *) clientData;

    if (!textPtr->exportSelection) {
	return;
    }

    /*
     * Just remove the "sel" tag from everything in the widget.
     */

    TkTextRedrawTag(textPtr, 0, 0, TkBTreeNumLines(textPtr->tree),
	    0, textPtr->selTagPtr, 1);
    TkBTreeTag(textPtr->tree, 0, 0, TkBTreeNumLines(textPtr->tree),
	    0, textPtr->selTagPtr, 0);
    textPtr->flags &= ~GOT_SELECTION;
}

/*
 *--------------------------------------------------------------
 *
 * TextMarkCmd --
 *
 *	This procedure is invoked to process the "mark" options of
 *	the widget command for text widgets. See the user documentation
 *	for details on what it does.
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
TextMarkCmd(textPtr, interp, argc, argv)
    register TkText *textPtr;	/* Information about text widget. */
    Tcl_Interp *interp;		/* Current interpreter. */
    int argc;			/* Number of arguments. */
    char **argv;		/* Argument strings.  Someone else has already
				 * parsed this command enough to know that
				 * argv[1] is "mark". */
{
    int length, line, ch, i;
    char c;
    Tcl_HashEntry *hPtr;
    TkAnnotation *markPtr;
    Tcl_HashSearch search;

    if (argc < 3) {
	Tcl_AppendResult(interp, "wrong # args: should be \"",
		argv[0], " mark option ?arg arg ...?\"", (char *) NULL);
	return TCL_ERROR;
    }
    c = argv[2][0];
    length = strlen(argv[2]);
    if ((c == 'n') && (strncmp(argv[2], "names", length) == 0)) {
	if (argc != 3) {
	    Tcl_AppendResult(interp, "wrong # args: should be \"",
		    argv[0], " mark names\"", (char *) NULL);
	    return TCL_ERROR;
	}
	for (hPtr = Tcl_FirstHashEntry(&textPtr->markTable, &search);
		hPtr != NULL; hPtr = Tcl_NextHashEntry(&search)) {
	    Tcl_AppendElement(interp,
		    Tcl_GetHashKey(&textPtr->markTable, hPtr), 0);
	}
    } else if ((c == 's') && (strncmp(argv[2], "set", length) == 0)) {
	if (argc != 5) {
	    Tcl_AppendResult(interp, "wrong # args: should be \"",
		    argv[0], " mark set markName index\"", (char *) NULL);
	    return TCL_ERROR;
	}
	if (TkTextGetIndex(interp, textPtr, argv[4], &line, &ch) != TCL_OK) {
	    return TCL_ERROR;
	}
	TkTextSetMark(textPtr, argv[3], line, ch);
    } else if ((c == 'u') && (strncmp(argv[2], "unset", length) == 0)) {
	if (argc < 4) {
	    Tcl_AppendResult(interp, "wrong # args: should be \"",
		    argv[0], " mark unset markName ?markName ...?\"",
		    (char *) NULL);
	    return TCL_ERROR;
	}
	for (i = 3; i < argc; i++) {
	    hPtr = Tcl_FindHashEntry(&textPtr->markTable, argv[i]);
	    if (hPtr != NULL) {
		markPtr = (TkAnnotation *) Tcl_GetHashValue(hPtr);
		if (markPtr == textPtr->insertAnnotPtr) {
		    interp->result = "can't delete \"insert\" mark";
		    return TCL_ERROR;
		}
		if (markPtr == textPtr->currentAnnotPtr) {
		    interp->result = "can't delete \"current\" mark";
		    return TCL_ERROR;
		}
		TkBTreeRemoveAnnotation(markPtr);
		Tcl_DeleteHashEntry(hPtr);
		ckfree((char *) markPtr);
	    }
	}
    } else {
	Tcl_AppendResult(interp, "bad mark option \"", argv[2],
		"\":  must be names, set, or unset",
		(char *) NULL);
	return TCL_ERROR;
    }
    return TCL_OK;
}

/*
 *----------------------------------------------------------------------
 *
 * TkTextSetMark --
 *
 *	Set a mark to a particular position, creating a new mark if
 *	one doesn't already exist.
 *
 * Results:
 *	The return value is a pointer to the mark that was just set.
 *
 * Side effects:
 *	A new mark is created, or an existing mark is moved.
 *
 *----------------------------------------------------------------------
 */

TkAnnotation *
TkTextSetMark(textPtr, name, line, ch)
    TkText *textPtr;		/* Text widget in which to create mark. */
    char *name;			/* Name of mark to set. */
    int line;			/* Index of line at which to place mark. */
    int ch;			/* Index of character within line at which
				 * to place mark. */
{
    Tcl_HashEntry *hPtr;
    TkAnnotation *markPtr;
    int new;

    hPtr = Tcl_CreateHashEntry(&textPtr->markTable, name, &new);
    markPtr = (TkAnnotation *) Tcl_GetHashValue(hPtr);
    if (!new) {
	/*
	 * If this is the insertion point that's being moved, be sure
	 * to force a display update at the old position.
	 */

	if (markPtr == textPtr->insertAnnotPtr) {
	    int oldLine;

	    oldLine = TkBTreeLineIndex(markPtr->linePtr);
	    TkTextLinesChanged(textPtr, oldLine, oldLine);
	}
	TkBTreeRemoveAnnotation(markPtr);
    } else {
	markPtr = (TkAnnotation *) ckalloc(sizeof(TkAnnotation));
	markPtr->type = TK_ANNOT_MARK;
	markPtr->info.hPtr = hPtr;
	Tcl_SetHashValue(hPtr, markPtr);
    }
    if (line < 0) {
	line = 0;
	markPtr->ch = 0;
    } else if (ch < 0) {
	markPtr->ch = 0;
    } else {
	markPtr->ch = ch;
    }
    markPtr->linePtr = TkBTreeFindLine(textPtr->tree, line);
    if (markPtr->linePtr == NULL) {
	line = TkBTreeNumLines(textPtr->tree)-1;
	markPtr->linePtr = TkBTreeFindLine(textPtr->tree, line);
	markPtr->ch = markPtr->linePtr->numBytes-1;
    } else {
	if (markPtr->ch >= markPtr->linePtr->numBytes) {
	    TkTextLine *nextLinePtr;

	    nextLinePtr = TkBTreeNextLine(markPtr->linePtr);
	    if (nextLinePtr == NULL) {
		markPtr->ch = markPtr->linePtr->numBytes-1;
	    } else {
		markPtr->linePtr = nextLinePtr;
		line++;
		markPtr->ch = 0;
	    }
	}
    }
    TkBTreeAddAnnotation(markPtr);

    /*
     * If the mark is the insertion cursor, then update the screen at the
     * mark's new location.
     */

    if (markPtr == textPtr->insertAnnotPtr) {
	TkTextLinesChanged(textPtr, line, line);
    }
    return markPtr;
}

/*
 *----------------------------------------------------------------------
 *
 * TextBlinkProc --
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
TextBlinkProc(clientData)
    ClientData clientData;	/* Pointer to record describing text. */
{
    register TkText *textPtr = (TkText *) clientData;
    int lineNum;

    if (!(textPtr->flags & GOT_FOCUS) || (textPtr->insertOffTime == 0)) {
	return;
    }
    if (textPtr->flags & INSERT_ON) {
	textPtr->flags &= ~INSERT_ON;
	textPtr->insertBlinkHandler = Tk_CreateTimerHandler(
		textPtr->insertOffTime, TextBlinkProc, (ClientData) textPtr);
    } else {
	textPtr->flags |= INSERT_ON;
	textPtr->insertBlinkHandler = Tk_CreateTimerHandler(
		textPtr->insertOnTime, TextBlinkProc, (ClientData) textPtr);
    }
    lineNum = TkBTreeLineIndex(textPtr->insertAnnotPtr->linePtr);
    TkTextLinesChanged(textPtr, lineNum, lineNum);
}

/*
 *----------------------------------------------------------------------
 *
 * TextFocusProc --
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
TextFocusProc(clientData, gotFocus)
    ClientData clientData;	/* Pointer to structure describing text. */
    int gotFocus;		/* 1 means window is getting focus, 0 means
				 * it's losing it. */
{
    register TkText *textPtr = (TkText *) clientData;
    int lineNum;

    Tk_DeleteTimerHandler(textPtr->insertBlinkHandler);
    if (gotFocus) {
	textPtr->flags |= GOT_FOCUS | INSERT_ON;
	if (textPtr->insertOffTime != 0) {
	    textPtr->insertBlinkHandler = Tk_CreateTimerHandler(
		    textPtr->insertOnTime, TextBlinkProc,
		    (ClientData) textPtr);
	}
    } else {
	textPtr->flags &= ~(GOT_FOCUS | INSERT_ON);
	textPtr->insertBlinkHandler = (Tk_TimerToken) NULL;
    }
    lineNum = TkBTreeLineIndex(textPtr->insertAnnotPtr->linePtr);
    TkTextLinesChanged(textPtr, lineNum, lineNum);
}

/*
 *--------------------------------------------------------------
 *
 * TextScanCmd --
 *
 *	This procedure is invoked to process the "scan" options of
 *	the widget command for text widgets. See the user documentation
 *	for details on what it does.
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
TextScanCmd(textPtr, interp, argc, argv)
    register TkText *textPtr;	/* Information about text widget. */
    Tcl_Interp *interp;		/* Current interpreter. */
    int argc;			/* Number of arguments. */
    char **argv;		/* Argument strings.  Someone else has already
				 * parsed this command enough to know that
				 * argv[1] is "tag". */
{
    int length, y, line, lastLine;
    char c;

    if (argc != 4) {
	Tcl_AppendResult(interp, "wrong # args: should be \"",
		argv[0], " scan mark|dragto y\"", (char *) NULL);
	return TCL_ERROR;
    }
    if (Tcl_GetInt(interp, argv[3], &y) != TCL_OK) {
	return TCL_ERROR;
    }
    c = argv[2][0];
    length = strlen(argv[2]);
    if ((c == 'd') && (strncmp(argv[2], "dragto", length) == 0)) {
	/*
	 * Amplify the difference between the current y position and the
	 * mark position to compute how many lines up or down the view
	 * should shift, then update the mark position to correspond to
	 * the new view.  If we run off the top or bottom of the text,
	 * reset the mark point so that the current position continues
	 * to correspond to the edge of the window.  This means that the
	 * picture will start dragging as soon as the mouse reverses
	 * direction (without this reset, might have to slide mouse a
	 * long ways back before the picture starts moving again).
	 */

	line = textPtr->scanMarkLine + (10*(textPtr->scanMarkY - y))
		/ (textPtr->fontPtr->ascent + textPtr->fontPtr->descent);
	lastLine = TkBTreeNumLines(textPtr->tree) - 1;
	if (line < 0) {
	    textPtr->scanMarkLine = line = 0;
	    textPtr->scanMarkY = y;
	} else if (line > lastLine) {
	    textPtr->scanMarkLine = line = lastLine;
	    textPtr->scanMarkY = y;
	}
	TkTextSetView(textPtr, line, 0);
    } else if ((c == 'm') && (strncmp(argv[2], "mark", length) == 0)) {
	textPtr->scanMarkLine = TkBTreeLineIndex(textPtr->topLinePtr);
	textPtr->scanMarkY = y;
    } else {
	Tcl_AppendResult(interp, "bad scan option \"", argv[2],
		"\":  must be mark or dragto", (char *) NULL);
	return TCL_ERROR;
    }
    return TCL_OK;
}
