/* 
 * tkFont.c --
 *
 *	This file maintains a database of looked-up fonts for the Tk
 *	toolkit, in order to avoid round-trips to the server to map
 *	font names to XFontStructs.
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
static char rcsid[] = "$Header: /user6/ouster/wish/RCS/tkFont.c,v 1.21 92/06/15 14:00:19 ouster Exp $ SPRITE (Berkeley)";
#endif

#include "tkconfig.h"
#include "tkint.h"

/*
 * This module caches extra information about fonts in addition to
 * what X already provides.  The extra information is used by the
 * TkMeasureChars procedure, and consists of two parts:  a type and
 * a width.  The type is one of the following:
 *
 * NORMAL:		Standard character.
 * TAB:			Tab character:  output enough space to
 *			get to next tab stop.
 * NEWLINE:		Newline character:  don't output anything more
 *			on this line (character has infinite width).
 * REPLACE:		This character doesn't print:  instead of
 *			displaying character, display a replacement
 *			sequence of the form "\xdd" where dd is the
 *			hex equivalent of the character.
 * SKIP:		Don't display anything for this character.  This
 *			is only used where the font doesn't contain
 *			all the characters needed to generate
 *			replacement sequences.
 * The width gives the total width of the displayed character or
 * sequence:  for replacement sequences, it gives the width of the
 * sequence.
 */

#define NORMAL		1
#define TAB		2
#define NEWLINE		3
#define REPLACE		4
#define SKIP		5

/*
 * One of the following data structures exists for each font that is
 * currently active.  The structure is indexed with two hash tables,
 * one based on font name and one based on XFontStruct address.
 */

typedef struct {
    XFontStruct *fontStructPtr;	/* X information about font. */
    Display *display;		/* Display to which font belongs. */
    int refCount;		/* Number of active uses of this font. */
    char *types;		/* Malloc'ed array giving types of all
				 * chars in the font (may be NULL). */
    unsigned char *widths;	/* Malloc'ed array giving widths of all
				 * chars in the font (may be NULL). */
    int tabWidth;		/* Width of tabs in this font. */
    Tcl_HashEntry *nameHashPtr;	/* Entry in name-based hash table (needed
				 * when deleting this structure). */
} TkFont;

/*
 * Hash table for name -> TkFont mapping, and key structure used to
 * index into that table:
 */

static Tcl_HashTable nameTable;
typedef struct {
    Tk_Uid name;		/* Name of font. */
    Display *display;		/* Display for which font is valid. */
} NameKey;

/*
 * Hash table for font struct -> TkFont mapping. This table is
 * indexed by the XFontStruct address.
 */

static Tcl_HashTable fontTable;

static int initialized = 0;	/* 0 means static structures haven't been
				 * initialized yet. */

/*
 * To speed up TkMeasureChars, the variables below keep the last
 * mapping from (XFontStruct *) to (TkFont *).
 */

static TkFont *lastFontPtr = NULL;
static XFontStruct *lastFontStructPtr = NULL;

/*
 * Characters used when displaying control sequences as their
 * hex equivalents.
 */

static char hexChars[] = "0123456789abcdefx\\";

/*
 * Forward declarations for procedures defined in this file:
 */

static void		FontInit _ANSI_ARGS_((void));
static void		SetFontMetrics _ANSI_ARGS_((TkFont *fontPtr));

/*
 *----------------------------------------------------------------------
 *
 * Tk_GetFontStruct --
 *
 *	Given a string name for a font, map the name to an XFontStruct
 *	describing the font.
 *
 * Results:
 *	The return value is normally a pointer to the font description
 *	for the desired font.  If an error occurs in mapping the string
 *	to a font, then an error message will be left in interp->result
 *	and NULL will be returned.
 *
 * Side effects:
 *	The font is added to an internal database with a reference count.
 *	For each call to this procedure, there should eventually be a call
 *	to Tk_FreeFontStruct, so that the database is cleaned up when fonts
 *	aren't in use anymore.
 *
 *----------------------------------------------------------------------
 */

XFontStruct *
Tk_GetFontStruct(interp, tkwin, name)
    Tcl_Interp *interp;		/* Place to leave error message if
				 * font can't be found. */
    Tk_Window tkwin;		/* Window in which font will be used. */
    Tk_Uid name;		/* Name of font (in form suitable for
				 * passing to XLoadQueryFont). */
{
    NameKey nameKey;
    Tcl_HashEntry *nameHashPtr, *fontHashPtr;
    int new;
    register TkFont *fontPtr;
    XFontStruct *fontStructPtr;

    if (!initialized) {
	FontInit();
    }

    /*
     * First, check to see if there's already a mapping for this font
     * name.
     */

    nameKey.name = name;
    nameKey.display = Tk_Display(tkwin);
    nameHashPtr = Tcl_CreateHashEntry(&nameTable, (char *) &nameKey, &new);
    if (!new) {
	fontPtr = (TkFont *) Tcl_GetHashValue(nameHashPtr);
	fontPtr->refCount++;
	return fontPtr->fontStructPtr;
    }

    /*
     * The name isn't currently known.  Map from the name to a font, and
     * add a new structure to the database.
     */

    fontStructPtr = XLoadQueryFont(nameKey.display, name);
    if (fontStructPtr == NULL) {
	Tcl_DeleteHashEntry(nameHashPtr);
	Tcl_AppendResult(interp, "font \"", name, "\" doesn't exist",
		(char *) NULL);
	return NULL;
    }
    fontPtr = (TkFont *) ckalloc(sizeof(TkFont));
    fontPtr->display = nameKey.display;
    fontPtr->fontStructPtr = fontStructPtr;
    fontPtr->refCount = 1;
    fontPtr->types = NULL;
    fontPtr->widths = NULL;
    fontPtr->nameHashPtr = nameHashPtr;
    fontHashPtr = Tcl_CreateHashEntry(&fontTable, (char *) fontStructPtr, &new);
    if (!new) {
	panic("XFontStruct already registered in Tk_GetFontStruct");
    }
    Tcl_SetHashValue(nameHashPtr, fontPtr);
    Tcl_SetHashValue(fontHashPtr, fontPtr);
    return fontPtr->fontStructPtr;
}

/*
 *--------------------------------------------------------------
 *
 * Tk_NameOfFontStruct --
 *
 *	Given a font, return a textual string identifying it.
 *
 * Results:
 *	If font was created by Tk_GetFontStruct, then the return
 *	value is the "string" that was used to create it.
 *	Otherwise the return value is a string giving the X
 *	identifier for the font.  The storage for the returned
 *	string is only guaranteed to persist up until the next
 *	call to this procedure.
 *
 * Side effects:
 *	None.
 *
 *--------------------------------------------------------------
 */

char *
Tk_NameOfFontStruct(fontStructPtr)
    XFontStruct *fontStructPtr;		/* Font whose name is desired. */
{
    Tcl_HashEntry *fontHashPtr;
    TkFont *fontPtr;
    static char string[20];

    if (!initialized) {
	printid:
	sprintf(string, "font id 0x%x", fontStructPtr->fid);
	return string;
    }
    fontHashPtr = Tcl_FindHashEntry(&fontTable, (char *) fontStructPtr);
    if (fontHashPtr == NULL) {
	goto printid;
    }
    fontPtr = (TkFont *) Tcl_GetHashValue(fontHashPtr);
    return ((NameKey *) fontPtr->nameHashPtr->key.words)->name;
}

/*
 *----------------------------------------------------------------------
 *
 * Tk_FreeFontStruct --
 *
 *	This procedure is called to release a font allocated by
 *	Tk_GetFontStruct.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The reference count associated with font is decremented, and
 *	the font is officially deallocated if no-one is using it
 *	anymore.
 *
 *----------------------------------------------------------------------
 */

void
Tk_FreeFontStruct(fontStructPtr)
    XFontStruct *fontStructPtr;	/* Font to be released. */
{
    Tcl_HashEntry *fontHashPtr;
    register TkFont *fontPtr;

    if (!initialized) {
	panic("Tk_FreeFontStruct called before Tk_GetFontStruct");
    }

    fontHashPtr = Tcl_FindHashEntry(&fontTable, (char *) fontStructPtr);
    if (fontHashPtr == NULL) {
	panic("Tk_FreeFontStruct received unknown font argument");
    }
    fontPtr = (TkFont *) Tcl_GetHashValue(fontHashPtr);
    fontPtr->refCount--;
    if (fontPtr->refCount == 0) {
	XFreeFont(fontPtr->display, fontPtr->fontStructPtr);
	Tcl_DeleteHashEntry(fontPtr->nameHashPtr);
	Tcl_DeleteHashEntry(fontHashPtr);
	if (fontPtr->types != NULL) {
	    ckfree(fontPtr->types);
	}
	if (fontPtr->widths != NULL) {
	    ckfree((char *) fontPtr->widths);
	}
	ckfree((char *) fontPtr);
	lastFontStructPtr = NULL;
    }
}

/*
 *----------------------------------------------------------------------
 *
 * FontInit --
 *
 *	Initialize the structure used for font management.
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
FontInit()
{
    initialized = 1;
    Tcl_InitHashTable(&nameTable, sizeof(NameKey)/sizeof(int));
    Tcl_InitHashTable(&fontTable, TCL_ONE_WORD_KEYS);
}

/*
 *--------------------------------------------------------------
 *
 * SetFontMetrics --
 *
 *	This procedure is called to fill in the "widths" and "types"
 *	arrays for a font.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	FontPtr gets modified to hold font metric information.
 *
 *--------------------------------------------------------------
 */

static void
SetFontMetrics(fontPtr)
    register TkFont *fontPtr;		/* Font structure in which to
					 * set metrics. */
{
    int i, replaceOK, baseWidth;
    register XFontStruct *fontStructPtr = fontPtr->fontStructPtr;
    char *p;

    /*
     * Pass 1: initialize the arrays.
     */

    fontPtr->types = (char *) ckalloc(256);
    fontPtr->widths = (unsigned char *) ckalloc(256);
    for (i = 0; i < 256; i++) {
	fontPtr->types[i] = REPLACE;
    }

    /*
     * Pass 2:  for all characters that exist in the font and are
     * not control characters, fill in the type and width
     * information.
     */

    for (i = ' '; i < 256;  i++) {
	if ((i == 0177) || (i < fontStructPtr->min_char_or_byte2)
		|| (i > fontStructPtr->max_char_or_byte2)) {
	    continue;
	}
	fontPtr->types[i] = NORMAL;
	if (fontStructPtr->per_char == NULL) {
	    fontPtr->widths[i] = fontStructPtr->min_bounds.width;
	} else {
	    fontPtr->widths[i] = fontStructPtr->per_char[i
		    - fontStructPtr->min_char_or_byte2].width;
	}
    }

    /*
     * Pass 3: fill in information for characters that have to
     * be replaced with  "\xhh" strings.  If the font doesn't
     * have the characters needed for this, then just use the
     * font's default character.
     */

    replaceOK = 1;
    baseWidth = fontPtr->widths['\\'] + fontPtr->widths['x'];
    for (p = hexChars; *p != 0; p++) {
	if (fontPtr->types[*p] != NORMAL) {
	    replaceOK = 0;
	    break;
	}
    }
    for (i = 0; i < 256; i++) {
	if (fontPtr->types[i] != REPLACE) {
	    continue;
	}
	if (replaceOK) {
	    fontPtr->widths[i] = baseWidth
		    + fontPtr->widths[hexChars[i & 0xf]]
		    + fontPtr->widths[hexChars[(i>>4) & 0xf]];
	} else {
	    fontPtr->types[i] = SKIP;
	    fontPtr->widths[i] = 0;
	}
    }

    /*
     * Lastly, fill in special information for newline and tab.
     */

    fontPtr->types['\n'] = NEWLINE;
    fontPtr->widths['\n'] = 0;
    fontPtr->types['\t'] = TAB;
    fontPtr->widths['\t'] = 0;
    if (fontPtr->types['0'] == NORMAL) {
	fontPtr->tabWidth = 8*fontPtr->widths['0'];
    } else {
	fontPtr->tabWidth = 8*fontStructPtr->max_bounds.width;
    }

    /*
     * Make sure the tab width isn't zero (some fonts may not have enough
     * information to set a reasonable tab width).
     */

    if (fontPtr->tabWidth == 0) {
	fontPtr->tabWidth = 1;
    }
}

/*
 *--------------------------------------------------------------
 *
 * TkMeasureChars --
 *
 *	Measure the number of characters from a string that
 *	will fit in a given horizontal span.  The measurement
 *	is done under the assumption that TkDisplayChars will
 *	be used to actually display the characters.
 *
 * Results:
 *	The return value is the number of characters from source
 *	that fit in the span given by startX and maxX.  *nextXPtr
 *	is filled in with the x-coordinate at which the first
 *	character that didn't fit would be drawn, if it were to
 *	be drawn.
 *
 * Side effects:
 *	None.
 *
 *--------------------------------------------------------------
 */

int
TkMeasureChars(fontStructPtr, source, maxChars, startX, maxX, flags, nextXPtr)
    XFontStruct *fontStructPtr;	/* Font in which to draw characters. */
    char *source;		/* Characters to be displayed.  Need not
				 * be NULL-terminated. */
    int maxChars;		/* Maximum # of characters to consider from
				 * source. */
    int startX;			/* X-position at which first character will
				 * be drawn. */
    int maxX;			/* Don't consider any character that would
				 * cross this x-position. */
    int flags;			/* Various flag bits OR-ed together.
				 * TK_WHOLE_WORDS means stop on a word boundary
				 * (just before a space character) if
				 * possible.  TK_AT_LEAST_ONE means always
				 * return a value of at least one, even
				 * if the character doesn't fit. 
				 * TK_PARTIAL_OK means it's OK to display only
				 * a part of the last character in the line.
				 * TK_NEWLINES_NOT_SPECIAL means that newlines
				 * are treated just like other control chars:
				 * they don't terminate the line,*/
    int *nextXPtr;		/* Return x-position of terminating
				 * character here. */
{
    register TkFont *fontPtr;
    register char *p;		/* Current character. */
    register int c;
    char *term;			/* Pointer to most recent character that
				 * may legally be a terminating character. */
    int termX;			/* X-position just after term. */
    int curX;			/* X-position corresponding to p. */
    int newX;			/* X-position corresponding to p+1. */
    int type;

    /*
     * Find the TkFont structure for this font, and make sure its
     * font metrics exist.
     */

    if (lastFontStructPtr == fontStructPtr) {
	fontPtr = lastFontPtr;
    } else {
	Tcl_HashEntry *fontHashPtr;

	if (!initialized) {
	    badArg:
	    panic("TkMeasureChars received unknown font argument");
	}
    
	fontHashPtr = Tcl_FindHashEntry(&fontTable, (char *) fontStructPtr);
	if (fontHashPtr == NULL) {
	    goto badArg;
	}
	fontPtr = (TkFont *) Tcl_GetHashValue(fontHashPtr);
	lastFontStructPtr = fontPtr->fontStructPtr;
	lastFontPtr = fontPtr;
    }
    if (fontPtr->types == NULL) {
	SetFontMetrics(fontPtr);
    }

    /*
     * Scan the input string one character at a time, until a character
     * is found that crosses maxX.
     */

    newX = curX = startX;
    termX = 0;		/* Not needed, but eliminates compiler warning. */
    term = source;
    for (p = source, c = *p & 0xff; maxChars > 0; p++, maxChars--) {
	type = fontPtr->types[c];
	if (type == NORMAL) {
	    newX += fontPtr->widths[c];
	} else if (type == TAB) {
	    newX += fontPtr->tabWidth;
	    newX -= newX % fontPtr->tabWidth;
	} else if (type == REPLACE) {
	    replaceType:
	    newX += fontPtr->widths['\\'] + fontPtr->widths['x']
		    + fontPtr->widths[hexChars[(c >> 4) & 0xf]]
		    + fontPtr->widths[hexChars[c & 0xf]];
	} else if (type == NEWLINE) {
	    if (flags & TK_NEWLINES_NOT_SPECIAL) {
		goto replaceType;
	    }
	    break;
	} else if (type != SKIP) {
	    panic("Unknown type %d in TkMeasureChars", type);
	}
	if (newX > maxX) {
	    break;
	}
	c = p[1] & 0xff;
	if (isspace(c) || (c == 0)) {
	    term = p+1;
	    termX = newX;
	}
	curX = newX;
    }

    /*
     * P points to the first character that doesn't fit in the desired
     * span.  Use the flags to figure out what to return.
     */

    if ((flags & TK_PARTIAL_OK) && (curX < maxX)) {
	curX = newX;
	p++;
    }
    if ((flags & TK_AT_LEAST_ONE) && (term == source) && (maxChars > 0)
	     & !isspace(*term)) {
	term = p;
	termX = curX;
	if (term == source) {
	    term++;
	    termX = newX;
	}
    } else if ((maxChars == 0) || !(flags & TK_WHOLE_WORDS)) {
	term = p;
	termX = curX;
    }
    *nextXPtr = termX;
    return term-source;
}

/*
 *--------------------------------------------------------------
 *
 * TkDisplayChars --
 *
 *	Draw a string of characters on the screen, converting
 *	tabs to the right number of spaces and control characters
 *	to sequences of the form "\xhh" where hh are two hex
 *	digits.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Information gets drawn on the screen.
 *
 *--------------------------------------------------------------
 */

void
TkDisplayChars(display, drawable, gc, fontStructPtr, string, numChars,
	x, y, flags)
    Display *display;		/* Display on which to draw. */
    Drawable drawable;		/* Window or pixmap in which to draw. */
    GC gc;			/* Graphics context for actually drawing
				 * characters. */
    XFontStruct *fontStructPtr;	/* Font used in GC;  must have been allocated
				 * by Tk_GetFontStruct.  Used to compute sizes
				 * of tabs, etc. */
    char *string;		/* Characters to be displayed. */
    int numChars;		/* Number of characters to display from
				 * string. */
    int x, y;			/* Coordinates at which to draw string. */
    int flags;			/* Flags to control display.  Only
				 * TK_NEWLINES_NOT_SPECIAL is supported right
				 * now.  See TkMeasureChars for information
				 * about it. */
{
    register TkFont *fontPtr;
    register char *p;		/* Current character being scanned. */
    register int c;
    int type;
    char *start;		/* First character waiting to be displayed. */
    int startX;			/* X-coordinate corresponding to start. */
    int curX;			/* X-coordinate corresponding to p. */
    char replace[10];

    /*
     * Find the TkFont structure for this font, and make sure its
     * font metrics exist.
     */

    if (lastFontStructPtr == fontStructPtr) {
	fontPtr = lastFontPtr;
    } else {
	Tcl_HashEntry *fontHashPtr;

	if (!initialized) {
	    badArg:
	    panic("TkDisplayChars received unknown font argument");
	}
    
	fontHashPtr = Tcl_FindHashEntry(&fontTable, (char *) fontStructPtr);
	if (fontHashPtr == NULL) {
	    goto badArg;
	}
	fontPtr = (TkFont *) Tcl_GetHashValue(fontHashPtr);
	lastFontStructPtr = fontPtr->fontStructPtr;
	lastFontPtr = fontPtr;
    }
    if (fontPtr->types == NULL) {
	SetFontMetrics(fontPtr);
    }

    /*
     * Scan the string one character at a time.  Display control
     * characters immediately, but delay displaying normal characters
     * in order to pass many characters to the server all together.
     */

    startX = curX = x;
    start = string;
    for (p = string; numChars > 0; numChars--, p++) {
	c = *p & 0xff;
	type = fontPtr->types[c];
	if (type == NORMAL) {
	    curX += fontPtr->widths[c];
	    continue;
	}
	if (p != start) {
	    XDrawString(display, drawable, gc, startX, y, start, p - start);
	    startX = curX;
	}
	if (type == TAB) {
	    curX += fontPtr->tabWidth;
	    curX -= curX % fontPtr->tabWidth;
	} else if (type == REPLACE) {
	    doReplace:
	    replace[0] = '\\';
	    replace[1] = 'x';
	    replace[2] = hexChars[(c >> 4) & 0xf];
	    replace[3] = hexChars[c & 0xf];
	    XDrawString(display, drawable, gc, startX, y, replace, 4);
	    curX += fontPtr->widths[replace[0]]
		    + fontPtr->widths[replace[1]]
		    + fontPtr->widths[replace[2]]
		    + fontPtr->widths[replace[3]];
	} else if (type == NEWLINE) {
	    if (flags & TK_NEWLINES_NOT_SPECIAL) {
		goto doReplace;
	    }
	    y += fontStructPtr->ascent + fontStructPtr->descent;
	    curX = x;
	} else if (type != SKIP) {
	    panic("Unknown type %d in TkDisplayChars", type);
	}
	startX = curX;
	start = p+1;
    }

    /*
     * At the very end, there may be one last batch of normal characters
     * to display.
     */

    if (p != start) {
	XDrawString(display, drawable, gc, startX, y, start, p - start);
    }
}

/*
 *----------------------------------------------------------------------
 *
 * TkUnderlineChars --
 *
 *	This procedure draws an underline for a given range of characters
 *	in a given string, using appropriate information for the string's
 *	font.  It doesn't draw the characters (which are assumed to have
 *	been displayed previously);  it just draws the underline.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Information gets displayed in "drawable".
 *
 *----------------------------------------------------------------------
 */

void
TkUnderlineChars(display, drawable, gc, fontStructPtr, string, x, y,
	flags, firstChar, lastChar)
    Display *display;		/* Display on which to draw. */
    Drawable drawable;		/* Window or pixmap in which to draw. */
    GC gc;			/* Graphics context for actually drawing
				 * underline. */
    XFontStruct *fontStructPtr;	/* Font used in GC;  must have been allocated
				 * by Tk_GetFontStruct.  Used to character
				 * dimensions, etc. */
    char *string;		/* String containing characters to be
				 * underlined. */
    int x, y;			/* Coordinates at which first character of
				 * string is drawn. */
    int flags;			/* Flags that were passed to TkDisplayChars. */
    int firstChar;		/* Index of first character to underline. */
    int lastChar;		/* Index of last character to underline. */
{
    int xUnder, yUnder, width, height;
    unsigned long value;

    /*
     * First compute the vertical span of the underline, using font
     * properties if they exist.
     */

    if (XGetFontProperty(fontStructPtr, XA_UNDERLINE_POSITION, &value)) {
	yUnder = y + value;
    } else {
	yUnder = y + fontStructPtr->max_bounds.descent/2;
    }
    if (XGetFontProperty(fontStructPtr, XA_UNDERLINE_THICKNESS, &value)) {
	height = value;
    } else {
	height = 2;
    }

    /*
     * Now compute the horizontal span of the underline.
     */

    TkMeasureChars(fontStructPtr, string, firstChar, x, (int) 1000000, flags,
	    &xUnder);
    TkMeasureChars(fontStructPtr, string+firstChar, lastChar+1-firstChar,
	    xUnder, (int) 1000000, flags, &width);
    width -= xUnder;

    XFillRectangle(display, drawable, gc, xUnder, yUnder,
	    (unsigned int) width, (unsigned int) height);
}
