/* 
 * tkCursor.c --
 *
 *	This file maintains a database of read-only cursors for the Tk
 *	toolkit.  This allows cursors to be shared between widgets and
 *	also avoids round-trips to the X server.
 *
 * Copyright 1990 Regents of the University of California
 * Permission to use, copy, modify, and distribute this
 * software and its documentation for any purpose and without
 * fee is hereby granted, provided that the above copyright
 * notice appear in all copies.  The University of California
 * makes no representations about the suitability of this
 * software for any purpose.  It is provided "as is" without
 * express or implied warranty.
 */

#ifndef lint
static char rcsid[] = "$Header: /user6/ouster/wish/RCS/tkCursor.c,v 1.12 91/10/31 11:40:41 ouster Exp $ SPRITE (Berkeley)";
#endif /* not lint */

#include "tkconfig.h"
#include "tkint.h"

/*
 * One of the following data structures exists for each cursor that is
 * currently active.  Each structure is indexed with two hash tables
 * defined below.  One of the tables is idTable, and the other is either
 * nameTable or dataTable, also defined below.
 * .
 */

typedef struct {
    Cursor cursor;		/* X identifier for cursor. */
    Display *display;		/* Display for which cursor is valid. */
    int refCount;		/* Number of active uses of cursor. */
    Tcl_HashTable *otherTable;	/* Second table (other than idTable) used
				 * to index this entry. */
    Tcl_HashEntry *hashPtr;	/* Entry in otherTable for this structure
				 * (needed when deleting). */
} TkCursor;

/*
 * Hash table to map from a textual description of a cursor to the
 * TkCursor record for the cursor, and key structure used in that
 * hash table:
 */

static Tcl_HashTable nameTable;
typedef struct {
    Tk_Uid name;		/* Textual name for desired cursor. */
    Display *display;		/* Display for which cursor will be used. */
} NameKey;

/*
 * Hash table to map from a collection of in-core data about a
 * cursor (bitmap contents, etc.) to a TkCursor structure:
 */

static Tcl_HashTable dataTable;
typedef struct {
    char *source;		/* Cursor bits. */
    char *mask;			/* Mask bits. */
    unsigned int width, height;	/* Dimensions of cursor (and data
				 * and mask). */
    int xHot, yHot;		/* Location of cursor hot-spot. */
    Tk_Uid fg, bg;		/* Colors for cursor. */
    Display *display;		/* Display on which cursor will be used. */
} DataKey;

/*
 * Hash table that maps from Cursor identifiers to the TkCursor structure
 * for the cursor.  This table is indexed by Cursor ids, and is used by
 * Tk_FreeCursor.
 */

static Tcl_HashTable idTable;

static int initialized = 0;	/* 0 means static structures haven't been
				 * initialized yet. */

/*
 * The table below is used to map from the name of a cursor to its
 * index in the official cursor font:
 */

static struct CursorName {
    char		*name;
    unsigned int	shape;
} cursorNames[] = {
    {"X_cursor",		XC_X_cursor},
    {"arrow",			XC_arrow},
    {"based_arrow_down",	XC_based_arrow_down},
    {"based_arrow_up",		XC_based_arrow_up},
    {"boat",			XC_boat},
    {"bogosity",		XC_bogosity},
    {"bottom_left_corner",	XC_bottom_left_corner},
    {"bottom_right_corner",	XC_bottom_right_corner},
    {"bottom_side",		XC_bottom_side},
    {"bottom_tee",		XC_bottom_tee},
    {"box_spiral",		XC_box_spiral},
    {"center_ptr",		XC_center_ptr},
    {"circle",			XC_circle},
    {"clock",			XC_clock},
    {"coffee_mug",		XC_coffee_mug},
    {"cross",			XC_cross},
    {"cross_reverse",		XC_cross_reverse},
    {"crosshair",		XC_crosshair},
    {"diamond_cross",		XC_diamond_cross},
    {"dot",			XC_dot},
    {"dotbox",			XC_dotbox},
    {"double_arrow",		XC_double_arrow},
    {"draft_large",		XC_draft_large},
    {"draft_small",		XC_draft_small},
    {"draped_box",		XC_draped_box},
    {"exchange",		XC_exchange},
    {"fleur",			XC_fleur},
    {"gobbler",			XC_gobbler},
    {"gumby",			XC_gumby},
    {"hand1",			XC_hand1},
    {"hand2",			XC_hand2},
    {"heart",			XC_heart},
    {"icon",			XC_icon},
    {"iron_cross",		XC_iron_cross},
    {"left_ptr",		XC_left_ptr},
    {"left_side",		XC_left_side},
    {"left_tee",		XC_left_tee},
    {"leftbutton",		XC_leftbutton},
    {"ll_angle",		XC_ll_angle},
    {"lr_angle",		XC_lr_angle},
    {"man",			XC_man},
    {"middlebutton",		XC_middlebutton},
    {"mouse",			XC_mouse},
    {"pencil",			XC_pencil},
    {"pirate",			XC_pirate},
    {"plus",			XC_plus},
    {"question_arrow",		XC_question_arrow},
    {"right_ptr",		XC_right_ptr},
    {"right_side",		XC_right_side},
    {"right_tee",		XC_right_tee},
    {"rightbutton",		XC_rightbutton},
    {"rtl_logo",		XC_rtl_logo},
    {"sailboat",		XC_sailboat},
    {"sb_down_arrow",		XC_sb_down_arrow},
    {"sb_h_double_arrow",	XC_sb_h_double_arrow},
    {"sb_left_arrow",		XC_sb_left_arrow},
    {"sb_right_arrow",		XC_sb_right_arrow},
    {"sb_up_arrow",		XC_sb_up_arrow},
    {"sb_v_double_arrow",	XC_sb_v_double_arrow},
    {"shuttle",			XC_shuttle},
    {"sizing",			XC_sizing},
    {"spider",			XC_spider},
    {"spraycan",		XC_spraycan},
    {"star",			XC_star},
    {"target",			XC_target},
    {"tcross",			XC_tcross},
    {"top_left_arrow",		XC_top_left_arrow},
    {"top_left_corner",		XC_top_left_corner},
    {"top_right_corner",	XC_top_right_corner},
    {"top_side",		XC_top_side},
    {"top_tee",			XC_top_tee},
    {"trek",			XC_trek},
    {"ul_angle",		XC_ul_angle},
    {"umbrella",		XC_umbrella},
    {"ur_angle",		XC_ur_angle},
    {"watch",			XC_watch},
    {"xterm",			XC_xterm},
    {NULL,			0}
};

/*
 * Font to use for cursors:
 */

#ifndef CURSORFONT
#define CURSORFONT "cursor"
#endif

/*
 * Forward declarations for procedures defined in this file:
 */

static void		CursorInit _ANSI_ARGS_((void));

/*
 *----------------------------------------------------------------------
 *
 * Tk_GetCursor --
 *
 *	Given a string describing a cursor, locate (or create if necessary)
 *	a cursor that fits the description.
 *
 * Results:
 *	The return value is the X identifer for the desired cursor,
 *	unless string couldn't be parsed correctly.  In this case,
 *	None is returned and an error message is left in interp->result.
 *	The caller should never modify the cursor that is returned, and
 *	should eventually call Tk_FreeCursor when the cursor is no longer
 *	needed.
 *
 * Side effects:
 *	The cursor is added to an internal database with a reference count.
 *	For each call to this procedure, there should eventually be a call
 *	to Tk_FreeCursor, so that the database can be cleaned up when cursors
 *	aren't needed anymore.
 *
 *----------------------------------------------------------------------
 */

Cursor
Tk_GetCursor(interp, tkwin, string)
    Tcl_Interp *interp;		/* Interpreter to use for error reporting. */
    Tk_Window tkwin;		/* Window in which cursor will be used. */
    Tk_Uid string;		/* Description of cursor.  See manual entry
				 * for details on legal syntax. */
{
    NameKey key;
    Tcl_HashEntry *nameHashPtr, *idHashPtr;
    register TkCursor *cursorPtr;
    int new;
    Cursor cursor;
    int argc;
    char **argv = NULL;
    Pixmap source = None;
    Pixmap mask = None;

    if (!initialized) {
	CursorInit();
    }

    key.name = string;
    key.display = Tk_Display(tkwin);
    nameHashPtr = Tcl_CreateHashEntry(&nameTable, (char *) &key, &new);
    if (!new) {
	cursorPtr = (TkCursor *) Tcl_GetHashValue(nameHashPtr);
	cursorPtr->refCount++;
	return cursorPtr->cursor;
    }

    /*
     * No suitable cursor exists.  Parse the cursor name into fields
     * and create a cursor, either from the standard cursor font or
     * from bitmap files.
     */

    if (Tcl_SplitList(interp, string, &argc, &argv) != TCL_OK) {
	goto error;
    }
    if (argc == 0) {
	badString:
	Tcl_AppendResult(interp, "bad cursor spec \"", string, "\"",
		(char *) NULL);
	goto error;
    }
    if (argv[0][0] != '@') {
	XColor fg, bg;
	int maskIndex;
	register struct CursorName *namePtr;
	TkDisplay *dispPtr;

	/*
	 * The cursor is to come from the standard cursor font.  If one
	 * arg, it is cursor name (use black and white for fg and bg).
	 * If two args, they are name and fg color (ignore mask).  If
	 * three args, they are name, fg, bg.  Some of the code below
	 * is stolen from the XCreateFontCursor Xlib procedure.
	 */

	if (argc > 3) {
	    goto badString;
	}
	for (namePtr = cursorNames; ; namePtr++) {
	    if (namePtr->name == NULL) {
		goto badString;
	    }
	    if ((namePtr->name[0] == argv[0][0])
		    && (strcmp(namePtr->name, argv[0]) == 0)) {
		break;
	    }
	}
	maskIndex = namePtr->shape + 1;
	if (argc == 1) {
	    fg.red = fg.green = fg.blue = 0;
	    bg.red = bg.green = bg.blue = 65535;
	} else {
	    if (XParseColor(key.display,
		    Tk_DefaultColormap(Tk_Screen(tkwin)),
		    argv[1], &fg) == 0) {
		Tcl_AppendResult(interp, "invalid color name \"", argv[1],
			"\"", (char *) NULL);
		goto error;
	    }
	    if (argc == 2) {
		bg.red = bg.green = bg.blue = 0;
		maskIndex = namePtr->shape;
	    } else {
		if (XParseColor(key.display,
			Tk_DefaultColormap(Tk_Screen(tkwin)),
			argv[2], &bg) == 0) {
		    Tcl_AppendResult(interp, "invalid color name \"", argv[2],
			    "\"", (char *) NULL);
		    goto error;
		}
	    }
	}
	dispPtr = ((TkWindow *) tkwin)->dispPtr;
	if (dispPtr->cursorFont == None) {
	    dispPtr->cursorFont = XLoadFont(key.display, CURSORFONT);
	    if (dispPtr->cursorFont == None) {
		interp->result = "couldn't load cursor font";
		goto error;
	    }
	}
	cursor = XCreateGlyphCursor(key.display, dispPtr->cursorFont,
		dispPtr->cursorFont, namePtr->shape, maskIndex,
		&fg, &bg);
    } else {
	unsigned int width, height, maskWidth, maskHeight;
	int xHot, yHot, dummy1, dummy2;
	XColor fg, bg;

	/*
	 * The cursor is to be created by reading bitmap files.  There
	 * should be either two elements in the list (source, color) or
	 * four (source mask fg bg).
	 */

	if ((argc != 2) && (argc != 4)) {
	    goto badString;
	}
	if (XReadBitmapFile(key.display, RootWindowOfScreen(Tk_Screen(tkwin)),
		&argv[0][1], &width, &height, &source, &xHot, &yHot)
		!= BitmapSuccess) {
	    Tcl_AppendResult(interp, "error reading bitmap file \"",
		    &argv[0][1], "\"", (char *) NULL);
	    goto error;
	}
	if ((xHot < 0) || (yHot < 0) || (xHot >= width) || (yHot >= height)) {
	    Tcl_AppendResult(interp, "bad hot spot in bitmap file \"",
		    &argv[0][1], "\"", (char *) NULL);
	    goto error;
	}
	if (argc == 2) {
	    if (XParseColor(key.display,
		    Tk_DefaultColormap(Tk_Screen(tkwin)),
		    argv[1], &fg) == 0) {
		Tcl_AppendResult(interp, "invalid color name \"",
			argv[1], "\"", (char *) NULL);
		goto error;
	    }
	    cursor = XCreatePixmapCursor(key.display, source, source,
		    &fg, &fg, xHot, yHot);
	} else {
	    if (XReadBitmapFile(key.display,
		    RootWindowOfScreen(Tk_Screen(tkwin)), argv[1],
		    &maskWidth, &maskHeight, &mask, &dummy1,
		    &dummy2) != BitmapSuccess) {
		Tcl_AppendResult(interp, "error reading bitmap file \"",
			argv[1], "\"", (char *) NULL);
		goto error;
	    }
	    if ((maskWidth != width) && (maskHeight != height)) {
		interp->result =
			"source and mask bitmaps have different sizes";
		goto error;
	    }
	    if (XParseColor(key.display,
		    Tk_DefaultColormap(Tk_Screen(tkwin)),
		    argv[2], &fg) == 0) {
		Tcl_AppendResult(interp, "invalid color name \"", argv[2],
			"\"", (char *) NULL);
		goto error;
	    }
	    if (XParseColor(key.display,
		    Tk_DefaultColormap(Tk_Screen(tkwin)),
		    argv[3], &bg) == 0) {
		Tcl_AppendResult(interp, "invalid color name \"", argv[3],
			"\"", (char *) NULL);
		goto error;
	    }
	    cursor = XCreatePixmapCursor(key.display, source, mask,
		    &fg, &bg, xHot, yHot);
	}
    }
    ckfree((char *) argv);

    /*
     * Add information about this cursor to our database.
     */

    cursorPtr = (TkCursor *) ckalloc(sizeof(TkCursor));
    cursorPtr->cursor = cursor;
    cursorPtr->display = key.display;
    cursorPtr->refCount = 1;
    cursorPtr->otherTable = &nameTable;
    cursorPtr->hashPtr = nameHashPtr;
    idHashPtr = Tcl_CreateHashEntry(&idTable, (char *) cursorPtr->cursor,
	    &new);
    if (!new) {
/* deh patched to support multiple displays */
/*	panic("cursor already registered in Tk_GetCursor"); */
        cursorPtr->refCount = 1000;
    }
    Tcl_SetHashValue(nameHashPtr, cursorPtr);
    Tcl_SetHashValue(idHashPtr, cursorPtr);
    return cursorPtr->cursor;

    error:
    Tcl_DeleteHashEntry(nameHashPtr);
    if (argv != NULL) {
	ckfree((char *) argv);
    }
    if (source != None) {
	XFreePixmap(key.display, source);
    }
    if (mask != None) {
	XFreePixmap(key.display, mask);
    }
    return None;
}

/*
 *----------------------------------------------------------------------
 *
 * Tk_GetCursorFromData --
 *
 *	Given a description of the bits and colors for a cursor,
 *	make a cursor that has the given properties.
 *
 * Results:
 *	The return value is the X identifer for the desired cursor,
 *	unless it couldn't be created properly.  In this case, None is
 *	returned and an error message is left in interp->result.  The
 *	caller should never modify the cursor that is returned, and
 *	should eventually call Tk_FreeCursor when the cursor is no
 *	longer needed.
 *
 * Side effects:
 *	The cursor is added to an internal database with a reference count.
 *	For each call to this procedure, there should eventually be a call
 *	to Tk_FreeCursor, so that the database can be cleaned up when cursors
 *	aren't needed anymore.
 *
 *----------------------------------------------------------------------
 */

Cursor
Tk_GetCursorFromData(interp, tkwin, source, mask, width, height,
	xHot, yHot, fg, bg)
    Tcl_Interp *interp;		/* Interpreter to use for error reporting. */
    Tk_Window tkwin;		/* Window in which cursor will be used. */
    char *source;		/* Bitmap data for cursor shape. */
    char *mask;			/* Bitmap data for cursor mask. */
    unsigned int width, height;	/* Dimensions of cursor. */
    int xHot, yHot;		/* Location of hot-spot in cursor. */
    Tk_Uid fg;			/* Foreground color for cursor. */
    Tk_Uid bg;			/* Background color for cursor. */
{
    DataKey key;
    Tcl_HashEntry *dataHashPtr, *idHashPtr;
    register TkCursor *cursorPtr;
    int new;
    XColor fgColor, bgColor;
    Pixmap sourcePixmap, maskPixmap;

    if (!initialized) {
	CursorInit();
    }

    key.source = source;
    key.mask = mask;
    key.width = width;
    key.height = height;
    key.xHot = xHot;
    key.yHot = yHot;
    key.fg = fg;
    key.bg = bg;
    key.display = Tk_Display(tkwin);
    dataHashPtr = Tcl_CreateHashEntry(&dataTable, (char *) &key, &new);
    if (!new) {
	cursorPtr = (TkCursor *) Tcl_GetHashValue(dataHashPtr);
	cursorPtr->refCount++;
	return cursorPtr->cursor;
    }

    /*
     * No suitable cursor exists yet.  Make one using the data
     * available and add it to the database.
     */

    if (XParseColor(key.display, Tk_DefaultColormap(Tk_Screen(tkwin)),
	    fg, &fgColor) == 0) {
	Tcl_AppendResult(interp, "invalid color name \"", fg, "\"",
		(char *) NULL);
	goto error;
    }
    if (XParseColor(key.display, Tk_DefaultColormap(Tk_Screen(tkwin)),
	    bg, &bgColor) == 0) {
	Tcl_AppendResult(interp, "invalid color name \"", bg, "\"",
		(char *) NULL);
	goto error;
    }

    cursorPtr = (TkCursor *) ckalloc(sizeof(TkCursor));
    sourcePixmap = XCreateBitmapFromData(key.display,
	    RootWindowOfScreen(Tk_Screen(tkwin)), source, width, height);
    maskPixmap = XCreateBitmapFromData(key.display, 
	    RootWindowOfScreen(Tk_Screen(tkwin)), mask, width, height);
    cursorPtr->cursor = XCreatePixmapCursor(key.display, sourcePixmap,
	    maskPixmap, &fgColor, &bgColor, xHot, yHot);
    XFreePixmap(key.display, sourcePixmap);
    XFreePixmap(key.display, maskPixmap);
    cursorPtr->display = key.display;
    cursorPtr->refCount = 1;
    cursorPtr->otherTable = &dataTable;
    cursorPtr->hashPtr = dataHashPtr;
    idHashPtr = Tcl_CreateHashEntry(&idTable, (char *) cursorPtr->cursor, &new);
    if (!new) {
/* deh patched to support multiple displays */
/*	panic("cursor already registered in Tk_GetCursorFromData"); */
        cursorPtr->refCount = 1000;
    }
    Tcl_SetHashValue(dataHashPtr, cursorPtr);
    Tcl_SetHashValue(idHashPtr, cursorPtr);
    return cursorPtr->cursor;

    error:
    Tcl_DeleteHashEntry(dataHashPtr);
    return None;
}

/*
 *--------------------------------------------------------------
 *
 * Tk_NameOfCursor --
 *
 *	Given a cursor, return a textual string identifying it.
 *
 * Results:
 *	If cursor was created by Tk_GetCursor, then the return
 *	value is the "string" that was used to create it.
 *	Otherwise the return value is a string giving the X
 *	identifier for the cursor.  The storage for the returned
 *	string is only guaranteed to persist up until the next
 *	call to this procedure.
 *
 * Side effects:
 *	None.
 *
 *--------------------------------------------------------------
 */

char *
Tk_NameOfCursor(cursor)
    Cursor cursor;			/* Cursor to be released. */
{
    Tcl_HashEntry *idHashPtr;
    TkCursor *cursorPtr;
    static char string[20];

    if (!initialized) {
	printid:
	sprintf(string, "cursor id 0x%x", cursor);
	return string;
    }
    idHashPtr = Tcl_FindHashEntry(&idTable, (char *) cursor);
    if (idHashPtr == NULL) {
	goto printid;
    }
    cursorPtr = (TkCursor *) Tcl_GetHashValue(idHashPtr);
    if (cursorPtr->otherTable != &nameTable) {
	goto printid;
    }
    return ((NameKey *) cursorPtr->hashPtr->key.words)->name;
}

/*
 *----------------------------------------------------------------------
 *
 * Tk_FreeCursor --
 *
 *	This procedure is called to release a cursor allocated by
 *	Tk_GetCursor or TkGetCursorFromData.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The reference count associated with cursor is decremented, and
 *	it is officially deallocated if no-one is using it anymore.
 *
 *----------------------------------------------------------------------
 */

void
Tk_FreeCursor(cursor)
    Cursor cursor;			/* Cursor to be released. */
{
    Tcl_HashEntry *idHashPtr;
    register TkCursor *cursorPtr;

    if (!initialized) {
	panic("Tk_FreeCursor called before Tk_GetCursor");
    }

    idHashPtr = Tcl_FindHashEntry(&idTable, (char *) cursor);
    if (idHashPtr == NULL) {
	panic("Tk_FreeCursor received unknown cursor argument");
    }
    cursorPtr = (TkCursor *) Tcl_GetHashValue(idHashPtr);
    cursorPtr->refCount--;
    if (cursorPtr->refCount == 0) {
	XFreeCursor(cursorPtr->display, cursorPtr->cursor);
	Tcl_DeleteHashEntry(cursorPtr->hashPtr);
	Tcl_DeleteHashEntry(idHashPtr);
	ckfree((char *) cursorPtr);
    }
}

/*
 *----------------------------------------------------------------------
 *
 * CursorInit --
 *
 *	Initialize the structures used for cursor management.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Read the code.
 *
 *----------------------------------------------------------------------
 */

static void
CursorInit()
{
    initialized = 1;
    Tcl_InitHashTable(&nameTable, sizeof(NameKey)/sizeof(long));
    Tcl_InitHashTable(&dataTable, sizeof(DataKey)/sizeof(long));
    Tcl_InitHashTable(&idTable, TCL_ONE_WORD_KEYS);
}
