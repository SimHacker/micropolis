/* 
 * tkListbox.c --
 *
 *	This module implements listbox widgets for the Tk
 *	toolkit.  A listbox displays a collection of strings,
 *	one per line, and provides scrolling and selection.
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
static char rcsid[] = "$Header: /user6/ouster/wish/RCS/tkListbox.c,v 1.56 92/05/13 09:05:20 ouster Exp $ SPRITE (Berkeley)";
#endif

#include "tkconfig.h"
#include "default.h"
#include "tkint.h"

/*
 * One record of the following type is kept for each element
 * associated with a listbox widget:
 */

typedef struct Element {
    int textLength;		/* # non-NULL characters in text. */
    int lBearing;		/* Distance from first character's
				 * origin to left edge of character. */
    int pixelWidth;		/* Total width of element in pixels (including
				 * left bearing and right bearing). */
    struct Element *nextPtr;	/* Next in list of all elements of this
				 * listbox, or NULL for last element. */
    char text[4];		/* Characters of this element, NULL-
				 * terminated.  The actual space allocated
				 * here will be as large as needed (> 4,
				 * most likely).  Must be the last field
				 * of the record. */
} Element;

#define ElementSize(stringLength) \
	((unsigned) (sizeof(Element) - 3 + stringLength))

/*
 * A data structure of the following type is kept for each listbox
 * widget managed by this file:
 */

typedef struct {
    Tk_Window tkwin;		/* Window that embodies the listbox.  NULL
				 * means that the window has been destroyed
				 * but the data structures haven't yet been
				 * cleaned up.*/
    Tcl_Interp *interp;		/* Interpreter associated with listbox. */
    int numElements;		/* Total number of elements in this listbox. */
    Element *elementPtr;	/* First in list of elements (NULL if no
				 * elements. */

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
    Tk_3DBorder selBorder;	/* Borders and backgrounds for selected
				 * elements. */
    int selBorderWidth;		/* Width of border around selection. */
    XColor *selFgColorPtr;	/* Foreground color for selected elements. */
    GC selTextGC;		/* For drawing selected text. */
    char *geometry;		/* Desired geometry for window.  Malloc'ed. */
    int lineHeight;		/* Number of pixels allocated for each line
				 * in display. */
    int topIndex;		/* Index of top-most element visible in
				 * window. */
    int numLines;		/* Number of lines (elements) that fit
				 * in window at one time. */

    /*
     * Information to support horizontal scrolling:
     */

    int maxWidth;		/* Width (in pixels) of widest string in
				 * listbox. */
    int xScrollUnit;		/* Number of pixels in one "unit" for
				 * horizontal scrolling (window scrolls
				 * horizontally in increments of this size).
				 * This is an average character size. */
    int xOffset;		/* The left edge of each string in the
				 * listbox is offset to the left by this
				 * many pixels (0 means no offset, positive
				 * means there is an offset). */

    /*
     * Information about what's selected, if any.
     */

    int selectFirst;		/* Index of first selected element (-1 means
				 * nothing selected. */
    int selectLast;		/* Index of last selected element. */
    int selectAnchor;		/* Fixed end of selection (i.e. element
				 * at which selection was started.) */
    int exportSelection;	/* Non-zero means tie internal listbox
				 * to X selection. */

    /*
     * Information for scanning:
     */

    int scanMarkX;		/* X-position at which scan started (e.g.
				 * button was pressed here). */
    int scanMarkY;		/* Y-position at which scan started (e.g.
				 * button was pressed here). */
    int scanMarkXOffset;	/* Value of "xOffset" field when scan
				 * started. */
    int scanMarkYIndex;		/* Index of line that was at top of window
				 * when scan started. */

    /*
     * Miscellaneous information:
     */

    Cursor cursor;		/* Current cursor for window, or None. */
    char *yScrollCmd;		/* Command prefix for communicating with
				 * vertical scrollbar.  NULL means no command
				 * to issue.  Malloc'ed. */
    char *xScrollCmd;		/* Command prefix for communicating with
				 * horizontal scrollbar.  NULL means no command
				 * to issue.  Malloc'ed. */
    int flags;			/* Various flag bits:  see below for
				 * definitions. */
} Listbox;

/*
 * Flag bits for buttons:
 *
 * REDRAW_PENDING:		Non-zero means a DoWhenIdle handler
 *				has already been queued to redraw
 *				this window.
 * UPDATE_V_SCROLLBAR:		Non-zero means vertical scrollbar needs
 *				to be updated.
 * UPDATE_H_SCROLLBAR:		Non-zero means horizontal scrollbar needs
 *				to be updated.
 */

#define REDRAW_PENDING		1
#define UPDATE_V_SCROLLBAR	2
#define UPDATE_H_SCROLLBAR	4

/*
 * Information used for argv parsing:
 */

static Tk_ConfigSpec configSpecs[] = {
    {TK_CONFIG_BORDER, "-background", "background", "Background",
	DEF_LISTBOX_BG_COLOR, Tk_Offset(Listbox, normalBorder),
	TK_CONFIG_COLOR_ONLY},
    {TK_CONFIG_BORDER, "-background", "background", "Background",
	DEF_LISTBOX_BG_MONO, Tk_Offset(Listbox, normalBorder),
	TK_CONFIG_MONO_ONLY},
    {TK_CONFIG_SYNONYM, "-bd", "borderWidth", (char *) NULL,
	(char *) NULL, 0, 0},
    {TK_CONFIG_SYNONYM, "-bg", "background", (char *) NULL,
	(char *) NULL, 0, 0},
    {TK_CONFIG_PIXELS, "-borderwidth", "borderWidth", "BorderWidth",
	DEF_LISTBOX_BORDER_WIDTH, Tk_Offset(Listbox, borderWidth), 0},
    {TK_CONFIG_ACTIVE_CURSOR, "-cursor", "cursor", "Cursor",
	DEF_LISTBOX_CURSOR, Tk_Offset(Listbox, cursor), TK_CONFIG_NULL_OK},
    {TK_CONFIG_BOOLEAN, "-exportselection", "exportSelection",
	"ExportSelection", DEF_LISTBOX_EXPORT_SELECTION,
	Tk_Offset(Listbox, exportSelection), 0},
    {TK_CONFIG_SYNONYM, "-fg", "foreground", (char *) NULL,
	(char *) NULL, 0, 0},
    {TK_CONFIG_FONT, "-font", "font", "Font",
	DEF_LISTBOX_FONT, Tk_Offset(Listbox, fontPtr), 0},
    {TK_CONFIG_COLOR, "-foreground", "foreground", "Foreground",
	DEF_LISTBOX_FG, Tk_Offset(Listbox, fgColorPtr), 0},
    {TK_CONFIG_STRING, "-geometry", "geometry", "Geometry",
	DEF_LISTBOX_GEOMETRY, Tk_Offset(Listbox, geometry), 0},
    {TK_CONFIG_RELIEF, "-relief", "relief", "Relief",
	DEF_LISTBOX_RELIEF, Tk_Offset(Listbox, relief), 0},
    {TK_CONFIG_BORDER, "-selectbackground", "selectBackground", "Foreground",
	DEF_LISTBOX_SELECT_COLOR, Tk_Offset(Listbox, selBorder),
	TK_CONFIG_COLOR_ONLY},
    {TK_CONFIG_BORDER, "-selectbackground", "selectBackground", "Foreground",
	DEF_LISTBOX_SELECT_MONO, Tk_Offset(Listbox, selBorder),
	TK_CONFIG_MONO_ONLY},
    {TK_CONFIG_PIXELS, "-selectborderwidth", "selectBorderWidth", "BorderWidth",
	DEF_LISTBOX_SELECT_BD, Tk_Offset(Listbox, selBorderWidth), 0},
    {TK_CONFIG_COLOR, "-selectforeground", "selectForeground", "Background",
	DEF_LISTBOX_SELECT_FG_COLOR, Tk_Offset(Listbox, selFgColorPtr),
	TK_CONFIG_COLOR_ONLY},
    {TK_CONFIG_COLOR, "-selectforeground", "selectForeground", "Background",
	DEF_LISTBOX_SELECT_FG_MONO, Tk_Offset(Listbox, selFgColorPtr),
	TK_CONFIG_MONO_ONLY},
    {TK_CONFIG_STRING, "-xscrollcommand", "xScrollCommand", "ScrollCommand",
	DEF_LISTBOX_SCROLL_COMMAND, Tk_Offset(Listbox, xScrollCmd), 0},
    {TK_CONFIG_STRING, "-yscrollcommand", "yScrollCommand", "ScrollCommand",
	DEF_LISTBOX_SCROLL_COMMAND, Tk_Offset(Listbox, yScrollCmd), 0},
    {TK_CONFIG_END, (char *) NULL, (char *) NULL, (char *) NULL,
	(char *) NULL, 0, 0}
};

/*
 * Forward declarations for procedures defined later in this file:
 */

static void		ChangeListboxOffset _ANSI_ARGS_((Listbox *listPtr,
			    int offset));
static void		ChangeListboxView _ANSI_ARGS_((Listbox *listPtr,
			    int index));
static int		ConfigureListbox _ANSI_ARGS_((Tcl_Interp *interp,
			    Listbox *listPtr, int argc, char **argv,
			    int flags));
static void		DeleteEls _ANSI_ARGS_((Listbox *listPtr, int first,
			    int last));
static void		DestroyListbox _ANSI_ARGS_((ClientData clientData));
static void		DisplayListbox _ANSI_ARGS_((ClientData clientData));
static int		GetListboxIndex _ANSI_ARGS_((Tcl_Interp *interp,
			    Listbox *listPtr, char *string, int *indexPtr));
static void		InsertEls _ANSI_ARGS_((Listbox *listPtr, int index,
			    int argc, char **argv));
static void		ListboxComputeWidths _ANSI_ARGS_((Listbox *listPtr,
			    int fontChanged));
static void		ListboxEventProc _ANSI_ARGS_((ClientData clientData,
			    XEvent *eventPtr));
static int		ListboxFetchSelection _ANSI_ARGS_((
			    ClientData clientData, int offset, char *buffer,
			    int maxBytes));
static void		ListboxLostSelection _ANSI_ARGS_((
			    ClientData clientData));
static void		ListboxRedrawRange _ANSI_ARGS_((Listbox *listPtr,
			    int first, int last));
static void		ListboxScanTo _ANSI_ARGS_((Listbox *listPtr,
			    int x, int y));
static void		ListboxSelectFrom _ANSI_ARGS_((Listbox *listPtr,
			    int index));
static void		ListboxSelectTo _ANSI_ARGS_((Listbox *listPtr,
			    int index));
static void		ListboxUpdateHScrollbar _ANSI_ARGS_((Listbox *listPtr));
static void		ListboxUpdateVScrollbar _ANSI_ARGS_((Listbox *listPtr));
static int		ListboxWidgetCmd _ANSI_ARGS_((ClientData clientData,
			    Tcl_Interp *interp, int argc, char **argv));
static int		NearestListboxElement _ANSI_ARGS_((Listbox *listPtr,
			    int y));

/*
 *--------------------------------------------------------------
 *
 * Tk_ListboxCmd --
 *
 *	This procedure is invoked to process the "listbox" Tcl
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
Tk_ListboxCmd(clientData, interp, argc, argv)
    ClientData clientData;	/* Main window associated with
				 * interpreter. */
    Tcl_Interp *interp;		/* Current interpreter. */
    int argc;			/* Number of arguments. */
    char **argv;		/* Argument strings. */
{
    register Listbox *listPtr;
    Tk_Window new;
    Tk_Window tkwin = (Tk_Window) clientData;

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
     * Initialize the fields of the structure that won't be initialized
     * by ConfigureListbox, or that ConfigureListbox requires to be
     * initialized already (e.g. resource pointers).
     */

    listPtr = (Listbox *) ckalloc(sizeof(Listbox));
    listPtr->tkwin = new;
    listPtr->interp = interp;
    listPtr->numElements = 0;
    listPtr->elementPtr = NULL;
    listPtr->normalBorder = NULL;
    listPtr->fontPtr = NULL;
    listPtr->fgColorPtr = NULL;
    listPtr->textGC = None;
    listPtr->selBorder = NULL;
    listPtr->selFgColorPtr = NULL;
    listPtr->selTextGC = NULL;
    listPtr->geometry = NULL;
    listPtr->topIndex = 0;
    listPtr->xOffset = 0;
    listPtr->selectFirst = -1;
    listPtr->selectLast = -1;
    listPtr->exportSelection = 1;
    listPtr->cursor = None;
    listPtr->yScrollCmd = NULL;
    listPtr->xScrollCmd = NULL;
    listPtr->flags = 0;

    Tk_SetClass(listPtr->tkwin, "Listbox");
    Tk_CreateEventHandler(listPtr->tkwin, ExposureMask|StructureNotifyMask,
	    ListboxEventProc, (ClientData) listPtr);
    Tk_CreateSelHandler(listPtr->tkwin, XA_STRING, ListboxFetchSelection,
	    (ClientData) listPtr, XA_STRING);
    Tcl_CreateCommand(interp, Tk_PathName(listPtr->tkwin), ListboxWidgetCmd,
	    (ClientData) listPtr, (void (*)()) NULL);
    if (ConfigureListbox(interp, listPtr, argc-2, argv+2, 0) != TCL_OK) {
	goto error;
    }

    interp->result = Tk_PathName(listPtr->tkwin);
    return TCL_OK;

    error:
    Tk_DestroyWindow(listPtr->tkwin);
    return TCL_ERROR;
}

/*
 *--------------------------------------------------------------
 *
 * ListboxWidgetCmd --
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
ListboxWidgetCmd(clientData, interp, argc, argv)
    ClientData clientData;		/* Information about listbox widget. */
    Tcl_Interp *interp;			/* Current interpreter. */
    int argc;				/* Number of arguments. */
    char **argv;			/* Argument strings. */
{
    register Listbox *listPtr = (Listbox *) clientData;
    int result = TCL_OK;
    int length;
    char c;

    if (argc < 2) {
	Tcl_AppendResult(interp, "wrong # args: should be \"",
		argv[0], " option ?arg arg ...?\"", (char *) NULL);
	return TCL_ERROR;
    }
    Tk_Preserve((ClientData) listPtr);
    c = argv[1][0];
    length = strlen(argv[1]);
    if ((c == 'c') && (strncmp(argv[1], "configure", length) == 0)
	    && (length >= 2)) {
	if (argc == 2) {
	    result = Tk_ConfigureInfo(interp, listPtr->tkwin, configSpecs,
		    (char *) listPtr, (char *) NULL, 0);
	} else if (argc == 3) {
	    result = Tk_ConfigureInfo(interp, listPtr->tkwin, configSpecs,
		    (char *) listPtr, argv[2], 0);
	} else {
	    result = ConfigureListbox(interp, listPtr, argc-2, argv+2,
		    TK_CONFIG_ARGV_ONLY);
	}
    } else if ((c == 'c') && (strncmp(argv[1], "curselection", length) == 0)
	    && (length >= 2)) {
	int i;
	char index[20];

	if (argc != 2) {
	    Tcl_AppendResult(interp, "wrong # args: should be \"",
		    argv[0], " curselection\"",
		    (char *) NULL);
	    goto error;
	}
	if (listPtr->selectFirst != -1) {
	    for (i = listPtr->selectFirst; i <= listPtr->selectLast; i++) {
		sprintf(index, "%d", i);
		Tcl_AppendElement(interp, index, 0);
	    }
	}
    } else if ((c == 'd') && (strncmp(argv[1], "delete", length) == 0)) {
	int first, last;

	if ((argc < 3) || (argc > 4)) {
	    Tcl_AppendResult(interp, "wrong # args: should be \"",
		    argv[0], " delete firstIndex ?lastIndex?\"",
		    (char *) NULL);
	    goto error;
	}
	if (GetListboxIndex(interp, listPtr, argv[2], &first) != TCL_OK) {
	    goto error;
	}
	if (argc == 3) {
	    last = first;
	} else {
	    if (GetListboxIndex(interp, listPtr, argv[3], &last) != TCL_OK) {
		goto error;
	    }
	}
	DeleteEls(listPtr, first, last);
    } else if ((c == 'g') && (strncmp(argv[1], "get", length) == 0)) {
	int index;
	register Element *elPtr;

	if (argc != 3) {
	    Tcl_AppendResult(interp, "wrong # args: should be \"",
		    argv[0], " get index\"", (char *) NULL);
	    goto error;
	}
	if (GetListboxIndex(interp, listPtr, argv[2], &index) != TCL_OK) {
	    goto error;
	}
	if (index < 0) {
	    index = 0;
	}
	if (index >= listPtr->numElements) {
	    index = listPtr->numElements-1;
	}
	for (elPtr = listPtr->elementPtr; index > 0;
		index--, elPtr = elPtr->nextPtr) {
	    /* Empty loop body. */
	}
	if (elPtr != NULL) {
	    interp->result = elPtr->text;
	}
    } else if ((c == 'i') && (strncmp(argv[1], "insert", length) == 0)) {
	int index;

	if (argc < 3) {
	    Tcl_AppendResult(interp, "wrong # args: should be \"",
		    argv[0], " insert index ?element? ?element ...?\"",
		    (char *) NULL);
	    goto error;
	}
	if (argc > 3) {
	    if (GetListboxIndex(interp, listPtr, argv[2], &index) != TCL_OK) {
		goto error;
	    }
	    InsertEls(listPtr, index, argc-3, argv+3);
	}
    } else if ((c == 'n') && (strncmp(argv[1], "nearest", length) == 0)) {
	int index, y;

	if (argc != 3) {
	    Tcl_AppendResult(interp, "wrong # args: should be \"",
		    argv[0], " nearest y\"", (char *) NULL);
	    goto error;
	}
	if (Tcl_GetInt(interp, argv[2], &y) != TCL_OK) {
	    goto error;
	}
	index = NearestListboxElement(listPtr, y);
	sprintf(interp->result, "%d", index);
    } else if ((c == 's') && (length >= 2)
	    && (strncmp(argv[1], "scan", length) == 0)) {
	int x, y;

	if (argc != 5) {
	    Tcl_AppendResult(interp, "wrong # args: should be \"",
		    argv[0], " scan mark|dragto x y\"", (char *) NULL);
	    goto error;
	}
	if ((Tcl_GetInt(interp, argv[3], &x) != TCL_OK)
		|| (Tcl_GetInt(interp, argv[4], &y) != TCL_OK)) {
	    goto error;
	}
	if ((argv[2][0] == 'm')
		&& (strncmp(argv[2], "mark", strlen(argv[2])) == 0)) {
	    listPtr->scanMarkX = x;
	    listPtr->scanMarkY = y;
	    listPtr->scanMarkXOffset = listPtr->xOffset;
	    listPtr->scanMarkYIndex = listPtr->topIndex;
	} else if ((argv[2][0] == 'd')
		&& (strncmp(argv[2], "dragto", strlen(argv[2])) == 0)) {
	    ListboxScanTo(listPtr, x, y);
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
	    if (listPtr->selectFirst != -1) {
		ListboxRedrawRange(listPtr, listPtr->selectFirst,
			listPtr->selectLast);
		listPtr->selectFirst = -1;
	    }
	    goto done;
	}
	if (argc != 4) {
	    Tcl_AppendResult(interp, "wrong # args: should be \"",
		    argv[0], " select option index\"", (char *) NULL);
	    goto error;
	}
	if (GetListboxIndex(interp, listPtr, argv[3], &index) != TCL_OK) {
	    goto error;
	}
	if ((c == 'a') && (strncmp(argv[2], "adjust", length) == 0)) {
	    if (index < (listPtr->selectFirst + listPtr->selectLast)/2) {
		listPtr->selectAnchor = listPtr->selectLast;
	    } else {
		listPtr->selectAnchor = listPtr->selectFirst;
	    }
	    ListboxSelectTo(listPtr, index);
	} else if ((c == 'f') && (strncmp(argv[2], "from", length) == 0)) {
	    ListboxSelectFrom(listPtr, index);
	} else if ((c == 't') && (strncmp(argv[2], "to", length) == 0)) {
	    ListboxSelectTo(listPtr, index);
	} else {
	    Tcl_AppendResult(interp, "bad select option \"", argv[2],
		    "\": must be adjust, clear, from, or to", (char *) NULL);
	    goto error;
	}
    } else if ((c == 's') && (length >= 2)
	    && (strncmp(argv[1], "size", length) == 0)) {
	sprintf(interp->result, "%d", listPtr->numElements);
    } else if ((c == 'x') && (strncmp(argv[1], "xview", length) == 0)) {
	int index;

	if (argc != 3) {
	    Tcl_AppendResult(interp, "wrong # args: should be \"",
		    argv[0], " xview index\"", (char *) NULL);
	    goto error;
	}
	if (Tcl_GetInt(interp, argv[2], &index) != TCL_OK) {
	    goto error;
	}
	ChangeListboxOffset(listPtr, index*listPtr->xScrollUnit);
    } else if ((c == 'y') && (strncmp(argv[1], "yview", length) == 0)) {
	int index;

	if (argc != 3) {
	    Tcl_AppendResult(interp, "wrong # args: should be \"",
		    argv[0], " yview index\"", (char *) NULL);
	    goto error;
	}
	if (GetListboxIndex(interp, listPtr, argv[2], &index) != TCL_OK) {
	    goto error;
	}
	ChangeListboxView(listPtr, index);
    } else {
	Tcl_AppendResult(interp, "bad option \"", argv[1],
		"\": must be configure, curselection, delete, get, ",
		"insert, nearest, scan, select, size, ",
		"xview, or yview", (char *) NULL);
	goto error;
    }
    done:
    Tk_Release((ClientData) listPtr);
    return result;

    error:
    Tk_Release((ClientData) listPtr);
    return TCL_ERROR;
}

/*
 *----------------------------------------------------------------------
 *
 * DestroyListbox --
 *
 *	This procedure is invoked by Tk_EventuallyFree or Tk_Release
 *	to clean up the internal structure of a listbox at a safe time
 *	(when no-one is using it anymore).
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Everything associated with the listbox is freed up.
 *
 *----------------------------------------------------------------------
 */

static void
DestroyListbox(clientData)
    ClientData clientData;	/* Info about listbox widget. */
{
    register Listbox *listPtr = (Listbox *) clientData;
    register Element *elPtr, *nextPtr;

    for (elPtr = listPtr->elementPtr; elPtr != NULL; ) {
	nextPtr = elPtr->nextPtr;
	ckfree((char *) elPtr);
	elPtr = nextPtr;
    }
    if (listPtr->normalBorder != NULL) {
	Tk_Free3DBorder(listPtr->normalBorder);
    }
    if (listPtr->fontPtr != NULL) {
	Tk_FreeFontStruct(listPtr->fontPtr);
    }
    if (listPtr->fgColorPtr != NULL) {
	Tk_FreeColor(listPtr->fgColorPtr);
    }
    if (listPtr->textGC != None) {
	Tk_FreeGC(listPtr->textGC);
    }
    if (listPtr->selBorder != NULL) {
	Tk_Free3DBorder(listPtr->selBorder);
    }
    if (listPtr->selFgColorPtr != NULL) {
	Tk_FreeColor(listPtr->selFgColorPtr);
    }
    if (listPtr->selTextGC != None) {
	Tk_FreeGC(listPtr->selTextGC);
    }
    if (listPtr->geometry != NULL) {
	ckfree(listPtr->geometry);
    }
    if (listPtr->cursor != None) {
	Tk_FreeCursor(listPtr->cursor);
    }
    if (listPtr->yScrollCmd != NULL) {
	ckfree(listPtr->yScrollCmd);
    }
    if (listPtr->xScrollCmd != NULL) {
	ckfree(listPtr->xScrollCmd);
    }
    ckfree((char *) listPtr);
}

/*
 *----------------------------------------------------------------------
 *
 * ConfigureListbox --
 *
 *	This procedure is called to process an argv/argc list, plus
 *	the Tk option database, in order to configure (or reconfigure)
 *	a listbox widget.
 *
 * Results:
 *	The return value is a standard Tcl result.  If TCL_ERROR is
 *	returned, then interp->result contains an error message.
 *
 * Side effects:
 *	Configuration information, such as colors, border width,
 *	etc. get set for listPtr;  old resources get freed,
 *	if there were any.
 *
 *----------------------------------------------------------------------
 */

static int
ConfigureListbox(interp, listPtr, argc, argv, flags)
    Tcl_Interp *interp;		/* Used for error reporting. */
    register Listbox *listPtr;	/* Information about widget;  may or may
				 * not already have values for some fields. */
    int argc;			/* Number of valid entries in argv. */
    char **argv;		/* Arguments. */
    int flags;			/* Flags to pass to Tk_ConfigureWidget. */
{
    XGCValues gcValues;
    GC new;
    int width, height, fontHeight, oldExport;

    oldExport = listPtr->exportSelection;
    if (Tk_ConfigureWidget(interp, listPtr->tkwin, configSpecs,
	    argc, argv, (char *) listPtr, flags) != TCL_OK) {
	return TCL_ERROR;
    }

    /*
     * A few options need special processing, such as parsing the
     * geometry and setting the background from a 3-D border.
     */

    Tk_SetBackgroundFromBorder(listPtr->tkwin, listPtr->normalBorder);

    gcValues.foreground = listPtr->fgColorPtr->pixel;
    gcValues.font = listPtr->fontPtr->fid;
    gcValues.graphics_exposures = False;
    new = Tk_GetGC(listPtr->tkwin, GCForeground|GCFont|GCGraphicsExposures,
	    &gcValues);
    if (listPtr->textGC != None) {
	Tk_FreeGC(listPtr->textGC);
    }
    listPtr->textGC = new;

    gcValues.foreground = listPtr->selFgColorPtr->pixel;
    gcValues.font = listPtr->fontPtr->fid;
    new = Tk_GetGC(listPtr->tkwin, GCForeground|GCFont, &gcValues);
    if (listPtr->selTextGC != None) {
	Tk_FreeGC(listPtr->selTextGC);
    }
    listPtr->selTextGC = new;

    /*
     * Claim the selection if we've suddenly started exporting it.
     */

    if (listPtr->exportSelection && (!oldExport)
	    && (listPtr->selectFirst !=-1)) {
	Tk_OwnSelection(listPtr->tkwin, ListboxLostSelection,
		(ClientData) listPtr);
    }

    /*
     * Register the desired geometry for the window, and arrange for
     * the window to be redisplayed.
     */

    if ((sscanf(listPtr->geometry, "%dx%d", &width, &height) != 2)
	    || (width <= 0) || (height <= 0)) {
	Tcl_AppendResult(interp, "bad geometry \"",
		listPtr->geometry, "\"", (char *) NULL);
	return TCL_ERROR;
    }
    fontHeight = listPtr->fontPtr->ascent + listPtr->fontPtr->descent;
    listPtr->lineHeight = fontHeight + 1 + 2*listPtr->selBorderWidth;
    listPtr->numLines = (Tk_Height(listPtr->tkwin) - 2*listPtr->borderWidth)
	    / listPtr->lineHeight;
    if (listPtr->numLines < 0) {
	listPtr->numLines = 0;
    }
    ListboxComputeWidths(listPtr, 1);
    width = (width+1)*listPtr->xScrollUnit + 2*listPtr->borderWidth
	    + 2*listPtr->selBorderWidth;
    height = height*listPtr->lineHeight + 2*listPtr->borderWidth;
    Tk_GeometryRequest(listPtr->tkwin, width, height);
    Tk_SetInternalBorder(listPtr->tkwin, listPtr->borderWidth);
    listPtr->flags |= UPDATE_V_SCROLLBAR|UPDATE_H_SCROLLBAR;
    ListboxRedrawRange(listPtr, 0, listPtr->numElements-1);
    return TCL_OK;
}

/*
 *--------------------------------------------------------------
 *
 * DisplayListbox --
 *
 *	This procedure redraws the contents of a listbox window.
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
DisplayListbox(clientData)
    ClientData clientData;	/* Information about window. */
{
    register Listbox *listPtr = (Listbox *) clientData;
    register Tk_Window tkwin = listPtr->tkwin;
    register Element *elPtr;
    GC gc;
    int i, limit, x, y, margin;
    Pixmap pixmap;

    listPtr->flags &= ~REDRAW_PENDING;
    if (listPtr->flags & UPDATE_V_SCROLLBAR) {
	ListboxUpdateVScrollbar(listPtr);
    }
    if (listPtr->flags & UPDATE_H_SCROLLBAR) {
	ListboxUpdateHScrollbar(listPtr);
    }
    listPtr->flags &= ~(REDRAW_PENDING|UPDATE_V_SCROLLBAR|UPDATE_H_SCROLLBAR);
    if ((listPtr->tkwin == NULL) || !Tk_IsMapped(tkwin)) {
	return;
    }

    /*
     * Redrawing is done in a temporary pixmap that is allocated
     * here and freed at the end of the procedure.  All drawing is
     * done to the pixmap, and the pixmap is copied to the screen
     * at the end of the procedure.  This provides the smoothest
     * possible visual effects (no flashing on the screen).
     */

    pixmap = XCreatePixmap(Tk_Display(tkwin), Tk_WindowId(tkwin),
	    Tk_Width(tkwin), Tk_Height(tkwin),
	    Tk_DefaultDepth(Tk_Screen(tkwin)));
    Tk_Fill3DRectangle(Tk_Display(tkwin), pixmap, listPtr->normalBorder,
	    0, 0, Tk_Width(tkwin), Tk_Height(tkwin), listPtr->borderWidth,
	    listPtr->relief);

    /*
     * Iterate through all of the elements of the listbox, displaying each
     * in turn.  Selected elements use a different GC and have a raised
     * background.
     */

    limit = listPtr->topIndex + listPtr->numLines - 1;
    if (limit >= listPtr->numElements) {
	limit = listPtr->numElements-1;
    }
    margin = listPtr->selBorderWidth + listPtr->xScrollUnit/2;
    for (elPtr = listPtr->elementPtr, i = 0; (elPtr != NULL) && (i <= limit);
	    elPtr = elPtr->nextPtr, i++) {
	if (i < listPtr->topIndex) {
	    continue;
	}
	x = listPtr->borderWidth;
	y = ((i - listPtr->topIndex) * listPtr->lineHeight) 
		+ listPtr->borderWidth;
	gc = listPtr->textGC;
	if ((listPtr->selectFirst >= 0) && (i >= listPtr->selectFirst)
		&& (i <= listPtr->selectLast)) {
	    gc = listPtr->selTextGC;
	    Tk_Fill3DRectangle(Tk_Display(tkwin), pixmap,
		    listPtr->selBorder, x, y,
		    Tk_Width(tkwin) - 2*listPtr->borderWidth,
		    listPtr->lineHeight, listPtr->selBorderWidth,
		    TK_RELIEF_RAISED);
	}
	y += listPtr->fontPtr->ascent + listPtr->selBorderWidth;
	x += margin - elPtr->lBearing - listPtr->xOffset;
	XDrawString(Tk_Display(tkwin), pixmap, gc, x, y,
		elPtr->text, elPtr->textLength);
    }

    /*
     * Redraw the border for the listbox to make sure that it's on top
     * of any of the text of the listbox entries.
     */

    Tk_Draw3DRectangle(Tk_Display(tkwin), pixmap,
	    listPtr->normalBorder, 0, 0, Tk_Width(tkwin),
	    Tk_Height(tkwin), listPtr->borderWidth,
	    listPtr->relief);
    XCopyArea(Tk_Display(tkwin), pixmap, Tk_WindowId(tkwin),
	    listPtr->textGC, 0, 0, Tk_Width(tkwin), Tk_Height(tkwin),
	    0, 0);
    XFreePixmap(Tk_Display(tkwin), pixmap);
}

/*
 *----------------------------------------------------------------------
 *
 * InsertEls --
 *
 *	Add new elements to a listbox widget.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	New information gets added to listPtr;  it will be redisplayed
 *	soon, but not immediately.
 *
 *----------------------------------------------------------------------
 */

static void
InsertEls(listPtr, index, argc, argv)
    register Listbox *listPtr;	/* Listbox that is to get the new
				 * elements. */
    int index;			/* Add the new elements before this
				 * element. */
    int argc;			/* Number of new elements to add. */
    char **argv;		/* New elements (one per entry). */
{
    register Element *prevPtr, *newPtr;
    int length, dummy, i, oldMaxWidth;
    XCharStruct bbox;

    /*
     * Find the element before which the new ones will be inserted.
     */

    if (index <= 0) {
	index = 0;
    }
    if (index > listPtr->numElements) {
	index = listPtr->numElements;
    }
    if (index == 0) {
	prevPtr = NULL;
    } else {
	for (prevPtr = listPtr->elementPtr, i = index - 1; i > 0; i--) {
	    prevPtr = prevPtr->nextPtr;
	}
    }

    /*
     * For each new element, create a record, initialize it, and link
     * it into the list of elements.
     */

    oldMaxWidth = listPtr->maxWidth;
    for (i = argc ; i > 0; i--, argv++, prevPtr = newPtr) {
	length = strlen(*argv);
	newPtr = (Element *) ckalloc(ElementSize(length));
	newPtr->textLength = length;
	strcpy(newPtr->text, *argv);
	XTextExtents(listPtr->fontPtr, newPtr->text, newPtr->textLength,
		&dummy, &dummy, &dummy, &bbox);
	newPtr->lBearing = bbox.lbearing;
	newPtr->pixelWidth = bbox.lbearing + bbox.rbearing;
	if (newPtr->pixelWidth > listPtr->maxWidth) {
	    listPtr->maxWidth = newPtr->pixelWidth;
	}
	if (prevPtr == NULL) {
	    newPtr->nextPtr = listPtr->elementPtr;
	    listPtr->elementPtr = newPtr;
	} else {
	    newPtr->nextPtr = prevPtr->nextPtr;
	    prevPtr->nextPtr = newPtr;
	}
    }
    listPtr->numElements += argc;

    /*
     * Update the selection to account for the  renumbering that has just
     * occurred.  Then arrange for the new information to be displayed.
     */

    if (index <= listPtr->selectFirst) {
	listPtr->selectFirst += argc;
    }
    if (index <= listPtr->selectLast) {
	listPtr->selectLast += argc;
    }
    listPtr->flags |= UPDATE_V_SCROLLBAR;
    if (listPtr->maxWidth != oldMaxWidth) {
	listPtr->flags |= UPDATE_H_SCROLLBAR;
    }
    ListboxRedrawRange(listPtr, index, listPtr->numElements-1);
}

/*
 *----------------------------------------------------------------------
 *
 * DeleteEls --
 *
 *	Remove one or more elements from a listbox widget.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Memory gets freed, the listbox gets modified and (eventually)
 *	redisplayed.
 *
 *----------------------------------------------------------------------
 */

static void
DeleteEls(listPtr, first, last)
    register Listbox *listPtr;	/* Listbox widget to modify. */
    int first;			/* Index of first element to delete. */
    int last;			/* Index of last element to delete. */
{
    register Element *prevPtr, *elPtr;
    int count, i, widthChanged;

    /*
     * Adjust the range to fit within the existing elements of the
     * listbox, and make sure there's something to delete.
     */

    if (first < 0) {
	first = 0;
    }
    if (last >= listPtr->numElements) {
	last = listPtr->numElements-1;
    }
    count = last + 1 - first;
    if (count <= 0) {
	return;
    }

    /*
     * Find the element just before the ones to delete.
     */

    if (first == 0) {
	prevPtr = NULL;
    } else {
	for (i = first-1, prevPtr = listPtr->elementPtr; i > 0; i--) {
	    prevPtr = prevPtr->nextPtr;
	}
    }

    /*
     * Delete the requested number of elements.
     */

    widthChanged = 0;
    for (i = count; i > 0; i--) {
	if (prevPtr == NULL) {
	    elPtr = listPtr->elementPtr;
	    listPtr->elementPtr = elPtr->nextPtr;
	} else {
	    elPtr = prevPtr->nextPtr;
	    prevPtr->nextPtr = elPtr->nextPtr;
	}
	if (elPtr->pixelWidth == listPtr->maxWidth) {
	    widthChanged = 1;
	}
	ckfree((char *) elPtr);
    }
    listPtr->numElements -= count;

    /*
     * Update the selection and viewing information to reflect the change
     * in the element numbering, and redisplay to slide information up over
     * the elements that were deleted.
     */

    if (first <= listPtr->selectFirst) {
	listPtr->selectFirst -= count;
	if (listPtr->selectFirst < first) {
	    listPtr->selectFirst = first;
	}
    }
    if (first <= listPtr->selectLast) {
	listPtr->selectLast -= count;
	if (listPtr->selectLast < first) {
	    listPtr->selectLast = first-1;
	}
    }
    if (listPtr->selectLast < listPtr->selectFirst) {
	listPtr->selectFirst = -1;
    }
    if (first <= listPtr->topIndex) {
	listPtr->topIndex -= count;
	if (listPtr->topIndex < first) {
	    listPtr->topIndex = first;
	}
    }
    listPtr->flags |= UPDATE_V_SCROLLBAR;
    if (widthChanged) {
	ListboxComputeWidths(listPtr, 0);
	listPtr->flags |= UPDATE_H_SCROLLBAR;
    }
    ListboxRedrawRange(listPtr, first, listPtr->numElements-1);
}

/*
 *--------------------------------------------------------------
 *
 * ListboxEventProc --
 *
 *	This procedure is invoked by the Tk dispatcher for various
 *	events on listboxes.
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
ListboxEventProc(clientData, eventPtr)
    ClientData clientData;	/* Information about window. */
    XEvent *eventPtr;		/* Information about event. */
{
    Listbox *listPtr = (Listbox *) clientData;

    if (eventPtr->type == Expose) {
	ListboxRedrawRange(listPtr,
		NearestListboxElement(listPtr, eventPtr->xexpose.y),
		NearestListboxElement(listPtr, eventPtr->xexpose.y
		+ eventPtr->xexpose.height));
    } else if (eventPtr->type == DestroyNotify) {
	Tcl_DeleteCommand(listPtr->interp, Tk_PathName(listPtr->tkwin));
	listPtr->tkwin = NULL;
	if (listPtr->flags & REDRAW_PENDING) {
	    Tk_CancelIdleCall(DisplayListbox, (ClientData) listPtr);
	}
	Tk_EventuallyFree((ClientData) listPtr, DestroyListbox);
    } else if (eventPtr->type == ConfigureNotify) {
	Tk_Preserve((ClientData) listPtr);
	listPtr->numLines = (Tk_Height(listPtr->tkwin)
		- 2*listPtr->borderWidth) / listPtr->lineHeight;
	listPtr->flags |= UPDATE_V_SCROLLBAR|UPDATE_H_SCROLLBAR;
	ListboxRedrawRange(listPtr, 0, listPtr->numElements-1);
	Tk_Release((ClientData) listPtr);
    }
}

/*
 *--------------------------------------------------------------
 *
 * GetListboxIndex --
 *
 *	Parse an index into a listbox and return either its value
 *	or an error.
 *
 * Results:
 *	A standard Tcl result.  If all went well, then *indexPtr is
 *	filled in with the index (into listPtr) corresponding to
 *	string.  Otherwise an error message is left in interp->result.
 *
 * Side effects:
 *	None.
 *
 *--------------------------------------------------------------
 */

static int
GetListboxIndex(interp, listPtr, string, indexPtr)
    Tcl_Interp *interp;		/* For error messages. */
    Listbox *listPtr;		/* Listbox for which the index is being
				 * specified. */
    char *string;		/* Numerical index into listPtr's element
				 * list, or "end" to refer to last element. */
    int *indexPtr;		/* Where to store converted index. */
{
    if (string[0] == 'e') {
	if (strncmp(string, "end", strlen(string)) != 0) {
	    badIndex:
	    Tcl_AppendResult(interp, "bad listbox index \"", string,
		    "\"", (char *) NULL);
	    return TCL_ERROR;
	}
	*indexPtr = listPtr->numElements;
	if (listPtr->numElements <= 0) {
	    *indexPtr = 0;
	}
    } else {
	if (Tcl_GetInt(interp, string, indexPtr) != TCL_OK) {
	    Tcl_ResetResult(interp);
	    goto badIndex;
	}
    }
    return TCL_OK;
}

/*
 *----------------------------------------------------------------------
 *
 * ChangeListboxView --
 *
 *	Change the view on a listbox widget.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	What's displayed on the screen is changed.  If there is a
 *	scrollbar associated with this widget, then the scrollbar
 *	is instructed to change its display too.
 *
 *----------------------------------------------------------------------
 */

static void
ChangeListboxView(listPtr, index)
    register Listbox *listPtr;		/* Information about widget. */
    int index;				/* Index of element in listPtr. */
{
    if (listPtr->tkwin == NULL) {
	return;
    }

    if (index >= listPtr->numElements) {
	index = listPtr->numElements-1;
    }
    if (index < 0) {
	index = 0;
    }
    if (listPtr->topIndex != index) {
	if (!(listPtr->flags & REDRAW_PENDING)) {
	    Tk_DoWhenIdle(DisplayListbox, (ClientData) listPtr);
	    listPtr->flags |= REDRAW_PENDING;
	}
	listPtr->topIndex = index;
	ListboxUpdateVScrollbar(listPtr);
    }
}

/*
 *----------------------------------------------------------------------
 *
 * ChangListboxOffset --
 *
 *	Change the horizontal offset for a listbox.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The listbox may be redrawn to reflect its new horizontal
 *	offset.
 *
 *----------------------------------------------------------------------
 */

static void
ChangeListboxOffset(listPtr, offset)
    register Listbox *listPtr;		/* Information about widget. */
    int offset;				/* Desired new "xOffset" for
					 * listbox. */
{
    int maxOffset;

    if (listPtr->tkwin == NULL) {
	return;
    }

    /*
     * Make sure that the new offset is within the allowable range, and
     * round it off to an even multiple of xScrollUnit.
     */

    maxOffset = listPtr->maxWidth + (listPtr->xScrollUnit-1)
	    - (Tk_Width(listPtr->tkwin) - 2*listPtr->borderWidth
	    - 2*listPtr->selBorderWidth - listPtr->xScrollUnit);
    if (offset > maxOffset) {
	offset = maxOffset;
    }
    if (offset < 0) {
	offset = 0;
    }
    offset -= offset%listPtr->xScrollUnit;
    if (offset != listPtr->xOffset) {
	listPtr->xOffset = offset;
	listPtr->flags |= UPDATE_H_SCROLLBAR;
	ListboxRedrawRange(listPtr, 0, listPtr->numElements);
    }
}

/*
 *----------------------------------------------------------------------
 *
 * ListboxScanTo --
 *
 *	Given a point (presumably of the curent mouse location)
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
ListboxScanTo(listPtr, x, y)
    register Listbox *listPtr;		/* Information about widget. */
    int x;				/* X-coordinate to use for scan
					 * operation. */
    int y;				/* Y-coordinate to use for scan
					 * operation. */
{
    int newTopIndex, newOffset;

    /*
     * Compute new top line for screen by amplifying the difference
     * between the current position and the place where the scan
     * started (the "mark" position).  If we run off the top or bottom
     * of the list, then reset the mark point so that the current
     * position continues to correspond to the edge of the window.
     * This means that the picture will start dragging as soon as the
     * mouse reverses direction (without this reset, might have to slide
     * mouse a long ways back before the picture starts moving again).
     */

    newTopIndex = listPtr->scanMarkYIndex
	    - (10*(y - listPtr->scanMarkY))/listPtr->lineHeight;
    if (newTopIndex >= listPtr->numElements) {
	newTopIndex = listPtr->scanMarkYIndex = listPtr->numElements-1;
	listPtr->scanMarkY = y;
    } else if (newTopIndex < 0) {
	newTopIndex = listPtr->scanMarkYIndex = 0;
	listPtr->scanMarkY = y;
    }
    ChangeListboxView(listPtr, newTopIndex);

    /*
     * Compute new left edge for display in a similar fashion by amplifying
     * the difference between the current position and the place where the
     * scan started.
     */

    newOffset = listPtr->scanMarkXOffset - (10*(x - listPtr->scanMarkX));
    if (newOffset >= listPtr->maxWidth) {
	newOffset = listPtr->scanMarkXOffset = listPtr->maxWidth;
	listPtr->scanMarkX = x;
    } else if (newOffset < 0) {
	newOffset = listPtr->scanMarkXOffset = 0;
	listPtr->scanMarkX = x;
    }
    ChangeListboxOffset(listPtr, newOffset);
}

/*
 *----------------------------------------------------------------------
 *
 * NearestListboxElement --
 *
 *	Given a y-coordinate inside a listbox, compute the index of
 *	the element under that y-coordinate (or closest to that
 *	y-coordinate).
 *
 * Results:
 *	The return value is an index of an element of listPtr.  If
 *	listPtr has no elements, then 0 is always returned.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

static int
NearestListboxElement(listPtr, y)
    register Listbox *listPtr;		/* Information about widget. */
    int y;				/* Y-coordinate in listPtr's window. */
{
    int index;

    index = (y - listPtr->borderWidth)/listPtr->lineHeight;
    if (index >= listPtr->numLines) {
	index = listPtr->numLines-1;
    }
    if (index < 0) {
	index = 0;
    }
    index += listPtr->topIndex;
    if (index >= listPtr->numElements) {
	index = listPtr->numElements-1;
    }
    return index;
}

/*
 *----------------------------------------------------------------------
 *
 * ListboxSelectFrom --
 *
 *	Start a new selection in a listbox.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	ListPtr claims the selection, and the selection becomes the
 *	single element given by index.
 *
 *----------------------------------------------------------------------
 */

static void
ListboxSelectFrom(listPtr, index)
    register Listbox *listPtr;		/* Information about widget. */
    int index;				/* Index of element that is to
					 * become the new selection. */
{
    /*
     * Make sure the index is within the proper range for the listbox.
     */

    if (index <= 0) {
	index = 0;
    }
    if (index >= listPtr->numElements) {
	index = listPtr->numElements-1;
    }

    if (listPtr->selectFirst != -1) {
	ListboxRedrawRange(listPtr, listPtr->selectFirst, listPtr->selectLast);
    } else if (listPtr->exportSelection) {
	Tk_OwnSelection(listPtr->tkwin, ListboxLostSelection,
		(ClientData) listPtr);
    }

    listPtr->selectFirst = listPtr->selectLast = index;
    listPtr->selectAnchor = index;
    ListboxRedrawRange(listPtr, index, index);
}

/*
 *----------------------------------------------------------------------
 *
 * ListboxSelectTo --
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
ListboxSelectTo(listPtr, index)
    register Listbox *listPtr;		/* Information about widget. */
    int index;				/* Index of element that is to
					 * become the "other" end of the
					 * selection. */
{
    int newFirst, newLast;

    /*
     * Make sure the index is within the proper range for the listbox.
     */

    if (index <= 0) {
	index = 0;
    }
    if (index >= listPtr->numElements) {
	index = listPtr->numElements-1;
    }

    /*
     * We should already own the selection, but grab it if we don't.
     */

    if (listPtr->selectFirst == -1) {
	ListboxSelectFrom(listPtr, index);
    }

    if (listPtr->selectAnchor < index) {
	newFirst = listPtr->selectAnchor;
	newLast = index;
    } else {
	newFirst = index;
	newLast = listPtr->selectAnchor;
    }
    if ((listPtr->selectFirst == newFirst)
	    && (listPtr->selectLast == newLast)) {
	return;
    }
    if (listPtr->selectFirst != newFirst) {
	if (listPtr->selectFirst < newFirst) {
	    ListboxRedrawRange(listPtr, listPtr->selectFirst, newFirst-1);
	} else {
	    ListboxRedrawRange(listPtr, newFirst, listPtr->selectFirst-1);
	}
	listPtr->selectFirst = newFirst;
    }
    if (listPtr->selectLast != newLast) {
	if (listPtr->selectLast < newLast) {
	    ListboxRedrawRange(listPtr, listPtr->selectLast+1, newLast);
	} else {
	    ListboxRedrawRange(listPtr, newLast+1, listPtr->selectLast);
	}
	listPtr->selectLast = newLast;
    }
}

/*
 *----------------------------------------------------------------------
 *
 * ListboxFetchSelection --
 *
 *	This procedure is called back by Tk when the selection is
 *	requested by someone.  It returns part or all of the selection
 *	in a buffer provided by the caller.
 *
 * Results:
 *	The return value is the number of non-NULL bytes stored
 *	at buffer.  Buffer is filled (or partially filled) with a
 *	NULL-terminated string containing part or all of the selection,
 *	as given by offset and maxBytes.  The selection is returned
 *	as a Tcl list with one list element for each element in the
 *	listbox.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

static int
ListboxFetchSelection(clientData, offset, buffer, maxBytes)
    ClientData clientData;		/* Information about listbox widget. */
    int offset;				/* Offset within selection of first
					 * byte to be returned. */
    char *buffer;			/* Location in which to place
					 * selection. */
    int maxBytes;			/* Maximum number of bytes to place
					 * at buffer, not including terminating
					 * NULL character. */
{
    register Listbox *listPtr = (Listbox *) clientData;
    register Element *elPtr;
    char **argv, *selection;
    int src, dst, length, count, argc;

    if ((listPtr->selectFirst == -1) || !listPtr->exportSelection) {
	return -1;
    }

    /*
     * Use Tcl_Merge to format the listbox elements into a suitable
     * Tcl list.
     */

    argc = listPtr->selectLast - listPtr->selectFirst + 1;
    argv = (char **) ckalloc((unsigned) (argc*sizeof(char *)));
    for (src = 0, dst = 0, elPtr = listPtr->elementPtr; ;
	    src++, elPtr = elPtr->nextPtr) {
	if (src < listPtr->selectFirst) {
	    continue;
	}
	if (src > listPtr->selectLast) {
	    break;
	}
	argv[dst] = elPtr->text;
	dst++;
    }
    selection = Tcl_Merge(argc, argv);

    /*
     * Copy the requested portion of the selection to the buffer.
     */

    length = strlen(selection);
    count = length - offset;
    if (count <= 0) {
	count = 0;
	goto done;
    }
    if (count > maxBytes) {
	count = maxBytes;
    }
    memcpy((VOID *) buffer, (VOID *) (selection + offset), count);

    done:
    buffer[count] = '\0';
    ckfree(selection);
    ckfree((char *) argv);
    return count;
}

/*
 *----------------------------------------------------------------------
 *
 * ListboxLostSelection --
 *
 *	This procedure is called back by Tk when the selection is
 *	grabbed away from a listbox widget.
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
ListboxLostSelection(clientData)
    ClientData clientData;		/* Information about listbox widget. */
{
    register Listbox *listPtr = (Listbox *) clientData;

    if ((listPtr->selectFirst >= 0) && listPtr->exportSelection) {
	ListboxRedrawRange(listPtr, listPtr->selectFirst, listPtr->selectLast);
	listPtr->selectFirst = -1;
    }
}

/*
 *----------------------------------------------------------------------
 *
 * ListboxRedrawRange --
 *
 *	Ensure that a given range of elements is eventually redrawn on
 *	the display (if those elements in fact appear on the display).
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Information gets redisplayed.
 *
 *----------------------------------------------------------------------
 */

	/* ARGSUSED */
static void
ListboxRedrawRange(listPtr, first, last)
    register Listbox *listPtr;		/* Information about widget. */
    int first;				/* Index of first element in list
					 * that needs to be redrawn. */
    int last;				/* Index of last element in list
					 * that needs to be redrawn.  May
					 * be less than first;
					 * these just bracket a range. */
{
    if ((listPtr->tkwin == NULL) || !Tk_IsMapped(listPtr->tkwin)
	    || (listPtr->flags & REDRAW_PENDING)) {
	return;
    }
    Tk_DoWhenIdle(DisplayListbox, (ClientData) listPtr);
    listPtr->flags |= REDRAW_PENDING;
}

/*
 *----------------------------------------------------------------------
 *
 * ListboxUpdateVScrollbar --
 *
 *	This procedure is invoked whenever information has changed in
 *	a listbox in a way that would invalidate a vertical scrollbar
 *	display.  If there is an associated scrollbar, then this command
 *	updates it by invoking a Tcl command.
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
ListboxUpdateVScrollbar(listPtr)
    register Listbox *listPtr;		/* Information about widget. */
{
    char string[60];
    int result, last;

    if (listPtr->yScrollCmd == NULL) {
	return;
    }
    last = listPtr->topIndex + listPtr->numLines - 1;
    if (last >= listPtr->numElements) {
	last = listPtr->numElements-1;
    }
    if (last < listPtr->topIndex) {
	last = listPtr->topIndex;
    }
    sprintf(string, " %d %d %d %d", listPtr->numElements, listPtr->numLines,
	    listPtr->topIndex, last);
    result = Tcl_VarEval(listPtr->interp, listPtr->yScrollCmd, string,
	    (char *) NULL);
    if (result != TCL_OK) {
	TkBindError(listPtr->interp);
    }
}

/*
 *----------------------------------------------------------------------
 *
 * ListboxUpdateHScrollbar --
 *
 *	This procedure is invoked whenever information has changed in
 *	a listbox in a way that would invalidate a horizontal scrollbar
 *	display.  If there is an associated horizontal scrollbar, then
 *	this command updates it by invoking a Tcl command.
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
ListboxUpdateHScrollbar(listPtr)
    register Listbox *listPtr;		/* Information about widget. */
{
    char string[60];
    int result, totalUnits, windowUnits, first, last;

    if (listPtr->xScrollCmd == NULL) {
	return;
    }
    totalUnits = 1 + (listPtr->maxWidth-1)/listPtr->xScrollUnit;
    windowUnits = 1 + (Tk_Width(listPtr->tkwin)
	    - 2*(listPtr->borderWidth + listPtr->selBorderWidth)-1)
	    /listPtr->xScrollUnit;
    first = listPtr->xOffset/listPtr->xScrollUnit;
    last = first + windowUnits - 1;
    if (last < first) {
	last = first;
    }
    sprintf(string, " %d %d %d %d", totalUnits, windowUnits, first, last);
    result = Tcl_VarEval(listPtr->interp, listPtr->xScrollCmd, string,
	    (char *) NULL);
    if (result != TCL_OK) {
	TkBindError(listPtr->interp);
    }
}

/*
 *----------------------------------------------------------------------
 *
 * ListboxComputeWidths --
 *
 *	This procedure is invoked to completely recompute width
 *	information used for displaying listboxes and for horizontal
 *	scrolling.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	If "fontChanged" is non-zero then the widths of the individual
 *	elements are all recomputed.  In addition, listPtr->maxWidth is
 *	recomputed.
 *
 *----------------------------------------------------------------------
 */

static void
ListboxComputeWidths(listPtr, fontChanged)
    Listbox *listPtr;		/* Listbox whose geometry is to be
				 * recomputed. */
    int fontChanged;		/* Non-zero means the font may have changed
				 * so per-element width information also
				 * has to be computed. */
{
    register Element *elPtr;
    int dummy;
    XCharStruct bbox;

    listPtr->xScrollUnit = XTextWidth(listPtr->fontPtr, "0", 1);
    listPtr->maxWidth = 0;
    for (elPtr = listPtr->elementPtr; elPtr != NULL; elPtr = elPtr->nextPtr) {
	if (fontChanged) {
	    XTextExtents(listPtr->fontPtr, elPtr->text, elPtr->textLength,
		    &dummy, &dummy, &dummy, &bbox);
	    elPtr->lBearing = bbox.lbearing;
	    elPtr->pixelWidth = bbox.lbearing + bbox.rbearing;
	}
	if (elPtr->pixelWidth > listPtr->maxWidth) {
	    listPtr->maxWidth = elPtr->pixelWidth;
	}
    }
}
