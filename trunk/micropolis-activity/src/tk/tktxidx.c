/* 
 * tkTextIndex.c --
 *
 *	This module provides procedures that manipulate indices for
 *	text widgets.
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
static char rcsid[] = "$Header: /user6/ouster/wish/RCS/tkTextIndex.c,v 1.2 92/07/16 16:32:26 ouster Exp $ SPRITE (Berkeley)";
#endif

#include "default.h"
#include "tkconfig.h"
#include "tk.h"
#include "tktext.h"

/*
 * Forward declarations for procedures defined later in this file:
 */

static void		BackwardChars _ANSI_ARGS_((TkText *textPtr,
			    TkTextLine *linePtr, int *lineIndexPtr,
			    int *chPtr, int count));
static char *		ForwBack _ANSI_ARGS_((TkText *textPtr,
			    char *string, int *lineIndexPtr, int *chPtr));
static void		ForwardChars _ANSI_ARGS_((TkText *textPtr,
			    TkTextLine *linePtr, int *lineIndexPtr,
			    int *chPtr, int count));
static char *		StartEnd _ANSI_ARGS_((TkText *textPtr,
			    char *string, int *lineIndexPtr, int *chPtr));

/*
 *----------------------------------------------------------------------
 *
 * TkTextGetIndex --
 *
 *	Given a string, return the line and character indices that
 *	it describes.
 *
 * Results:
 *	The return value is a standard Tcl return result.  If
 *	TCL_OK is returned, then everything went well and information
 *	is stored at *lineIndexPtr and *chPtr;  otherwise TCL_ERROR
 *	is returned and an error message is left in interp->result.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

int
TkTextGetIndex(interp, textPtr, string, lineIndexPtr, chPtr)
    Tcl_Interp *interp;		/* Use this for error reporting. */
    TkText *textPtr;		/* Information about text widget. */
    char *string;		/* Textual description of position. */
    int *lineIndexPtr;		/* Store line number here. */
    int *chPtr;			/* Store character position here. */
{
    register char *p;
    char *end, *endOfBase;
    TkTextLine *linePtr;
    Tcl_HashEntry *hPtr;
    TkAnnotation *markPtr;
    TkTextTag *tagPtr;
    TkTextSearch search;
    int first;
    char c;

    /*
     *------------------------------------------------
     * Stage 1: parse the base index.
     *------------------------------------------------
     */

    if (string[0] == '@') {
	/*
	 * Find character at a given x,y location in the window.
	 */

	int x, y;

	p = string+1;
	x = strtol(p, &end, 0);
	if ((end == p) || (*end != ',')) {
	    goto error;
	}
	p = end+1;
	y = strtol(p, &end, 0);
	if (end == p) {
	    goto error;
	}
	*lineIndexPtr = TkBTreeLineIndex(TkTextCharAtLoc(textPtr, x,
		y, chPtr));
	endOfBase = end;
	goto gotBase; 
    } else if (isdigit(string[0]) || (string[0] == '-')) {
	/*
	 * Base is identified with line and character indices.
	 */

	*lineIndexPtr = strtol(string, &end, 0) - 1;
	if ((end == string) || (*end != '.')) {
	    goto error;
	}
	p = end+1;
	if ((*p == 'e') && (strncmp(p, "end", 3) == 0)) {
	    linePtr = TkBTreeFindLine(textPtr->tree, *lineIndexPtr);
	    if (linePtr == NULL) {
		Tcl_AppendResult(interp, "bad text index \"", string,
			"\": no such line in text", (char *) NULL);
		return TCL_ERROR;
	    }
	    *chPtr = linePtr->numBytes - 1;
	    endOfBase = p+3;
	    goto gotBase;
	} else {
	    *chPtr = strtol(p, &end, 0);
	    if (end == p) {
		goto error;
	    }
	    endOfBase = end;
	    goto gotBase;
	}
    }

    for (p = string; *p != 0; p++) {
	if (isspace(*p) || (*p == '+') || (*p == '-')) {
	    break;
	}
    }
    endOfBase = p;
    if ((string[0] == 'e')
	    && (strncmp(string, "end", endOfBase-string) == 0)) {
	/*
	 * Base position is end of text.
	 */

	*lineIndexPtr = TkBTreeNumLines(textPtr->tree) - 1;
	linePtr = TkBTreeFindLine(textPtr->tree, *lineIndexPtr);
	*chPtr = linePtr->numBytes - 1;
	goto gotBase;
    } else {
	/*
	 * See if the base position is the name of a mark.
	 */

	c = *endOfBase;
	*endOfBase = 0;
	hPtr = Tcl_FindHashEntry(&textPtr->markTable, string);
	*endOfBase = c;
	if (hPtr != NULL) {
	    markPtr = (TkAnnotation *) Tcl_GetHashValue(hPtr);
	    *lineIndexPtr = TkBTreeLineIndex(markPtr->linePtr);
	    *chPtr = markPtr->ch;
	    goto gotBase;
	}
    }

    /*
     * Nothing has worked so far.  See if the base has the form
     * "tag.first" or "tag.last" where "tag" is the name of a valid
     * tag.
     */

    p = strchr(string, '.');
    if (p == NULL) {
	goto error;
    }
    if ((p[1] == 'f') && (endOfBase == (p+6))
	    && (strncmp(p+1, "first", endOfBase - (p+1)) == 0)) {
	first = 1;
    } else if ((p[1] == 'l') && (endOfBase == (p+5))
	    && (strncmp(p+1, "last", endOfBase - (p+1)) == 0)) {
	first = 0;
    } else {
	goto error;
    }
    *p = 0;
    hPtr = Tcl_FindHashEntry(&textPtr->tagTable, string);
    *p = '.';
    if (hPtr == NULL) {
	goto error;
    }
    tagPtr = (TkTextTag *) Tcl_GetHashValue(hPtr);
    TkBTreeStartSearch(textPtr->tree, 0, 0, TkBTreeNumLines(textPtr->tree),
	    0, tagPtr, &search);
    if (!TkBTreeNextTag(&search)) {
	Tcl_AppendResult(interp,
		"text doesn't contain any characters tagged with \"",
		Tcl_GetHashKey(&textPtr->tagTable, hPtr), "\"", (char *) NULL);
	return TCL_ERROR;
    }
    if (first) {
	*lineIndexPtr = search.line1;
	*chPtr = search.ch1;
    } else {
	while (TkBTreeNextTag(&search)) {
	    *lineIndexPtr = search.line1;
	    *chPtr = search.ch1;
	}
    }

    /*
     *-------------------------------------------------------------------
     * Stage 2: process zero or more modifiers.  Each modifier is either
     * a keyword like "wordend" or "linestart", or it has the form
     * "op count units" where op is + or -, count is a number, and units
     * is "chars" or "lines".
     *-------------------------------------------------------------------
     */

    gotBase:
    p = endOfBase;
    while (1) {
	while (isspace(*p)) {
	    p++;
	}
	if (*p == 0) {
	    return TCL_OK;
	}
    
	if ((*p == '+') || (*p == '-')) {
	    p = ForwBack(textPtr, p, lineIndexPtr, chPtr);
	} else {
	    p = StartEnd(textPtr, p, lineIndexPtr, chPtr);
	}
	if (p == NULL) {
	    goto error;
	}
    }

    error:
    Tcl_AppendResult(interp, "bad text index \"", string, "\"",
	    (char *) NULL);
    return TCL_ERROR;
}

/*
 *----------------------------------------------------------------------
 *
 * TkTextPrintIndex --
 *
 *	Given a line number and a character index, this procedure
 *	generates a string description of the position, which is
 *	suitable for reading in again later.
 *
 * Results:
 *	The characters pointed to by string are modified.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

void
TkTextPrintIndex(line, ch, string)
    int line;			/* Line number. */
    int ch;			/* Character position within line. */
    char *string;		/* Place to store the position.  Must have
				 * at least POS_CHARS characters. */
{
    sprintf(string, "%d.%d", line+1, ch);
}

/*
 *----------------------------------------------------------------------
 *
 * TkTextRoundIndex --
 *
 *	Given a line index and a character index, this procedure
 *	adjusts those positions if necessary to correspond to the
 *	nearest actual character within the text.
 *
 * Results:
 *	The return value is a pointer to the line structure for
 *	the line of the text's B-tree that contains the indicated
 *	character.  In addition, *lineIndexPtr and *chPtr are
 *	modified if necessary to refer to an existing character
 *	in the file.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */


TkTextLine *
TkTextRoundIndex(textPtr, lineIndexPtr, chPtr)
    TkText *textPtr;			/* Information about text widget. */
    int *lineIndexPtr;			/* Points to initial line index,
					 * which is overwritten with actual
					 * line index. */
    int *chPtr;				/* Points to initial character index,
					 * which is overwritten with actual
					 * character index. */
{
    int line, ch, lastLine;
    TkTextLine *linePtr;

    line = *lineIndexPtr;
    ch = *chPtr;
    if (line < 0) {
	line = 0;
	ch = 0;
    }
    lastLine = TkBTreeNumLines(textPtr->tree) - 1;
    if (line > lastLine) {
	line = lastLine;
	linePtr = TkBTreeFindLine(textPtr->tree, line);
	ch = linePtr->numBytes - 1;
    } else {
	linePtr = TkBTreeFindLine(textPtr->tree, line);
	if (ch < 0) {
	    ch = 0;
	}
	if (ch >= linePtr->numBytes) {
	    if (line == lastLine) {
		ch = linePtr->numBytes - 1;
	    } else {
		line++;
		linePtr = TkBTreeNextLine(linePtr);
		ch = 0;
	    }
	}
    }
    *lineIndexPtr = line;
    *chPtr = ch;
    return linePtr;
}

/*
 *----------------------------------------------------------------------
 *
 * ForwBack --
 *
 *	This procedure handles +/- modifiers for indices to adjust
 *	the index forwards or backwards.
 *
 * Results:
 *	If the modifier is successfully parsed then the return value
 *	is the address of the first character after the modifier, and
 *	*lineIndexPtr and *chPtr are updated to reflect the modifier.
 *	If there is a syntax error in the modifier then NULL is returned.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

static char *
ForwBack(textPtr, string, lineIndexPtr, chPtr)
    TkText *textPtr;		/* Information about widget that index
				 * refers to. */
    char *string;		/* String to parse for additional info
				 * about modifier (count and units). 
				 * Points to "+" or "-" that starts
				 * modifier. */
    int *lineIndexPtr;		/* Points to current line index, which will
				 * be updated to reflect modifier. */
    int *chPtr;			/* Points to current character index, which
				 * will be updated to reflect modifier. */
{
    register char *p;
    char *end, *units;
    int count, length, lastLine;
    TkTextLine *linePtr;

    /*
     * Get the count (how many units forward or backward).
     */

    p = string+1;
    while (isspace(*p)) {
	p++;
    }
    count = strtoul(p, &end, 0);
    if (end == p) {
	return NULL;
    }
    p = end;
    while (isspace(*p)) {
	p++;
    }

    /*
     * Find the end of this modifier (next space or + or - character),
     * then parse the unit specifier and update the position
     * accordingly.
     */

    units = p; 
    while ((*p != 0) && !isspace(*p) && (*p != '+') && (*p != '-')) {
	p++;
    }
    length = p - units;
    if ((*units == 'c') && (strncmp(units, "chars", length) == 0)) {
	linePtr = TkTextRoundIndex(textPtr, lineIndexPtr, chPtr);
	if (*string == '+') {
	    ForwardChars(textPtr, linePtr, lineIndexPtr, chPtr, count);
	} else {
	    BackwardChars(textPtr, linePtr, lineIndexPtr, chPtr, count);
	}
    } else if ((*units == 'l') && (strncmp(units, "lines", length) == 0)) {
	if (*string == '+') {
	    *lineIndexPtr += count;
	    lastLine = TkBTreeNumLines(textPtr->tree) - 1;
	    if (*lineIndexPtr > lastLine) {
		*lineIndexPtr = lastLine;
	    }
	} else {
	    *lineIndexPtr -= count;
	    if (*lineIndexPtr < 0) {
		*lineIndexPtr = 0;
	    }
	}
	linePtr = TkBTreeFindLine(textPtr->tree, *lineIndexPtr);
	if (*chPtr >= linePtr->numBytes) {
	    *chPtr = linePtr->numBytes - 1;
	}
	if (*chPtr < 0) {
	    *chPtr = 0;
	}
    } else {
	return NULL;
    }
    return p;
}

/*
 *----------------------------------------------------------------------
 *
 * ForwardChars --
 *
 *	Given a position in a text widget, this procedure computes
 *	a new position that is "count" characters ahead of the given
 *	position.
 *
 * Results:
 *	*LineIndexPtr and *chPtr are overwritten with new values
 *	corresponding to the new position.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

	/* ARGSUSED */
static void
ForwardChars(textPtr, linePtr, lineIndexPtr, chPtr, count)
    TkText *textPtr;			/* Information about text widget. */
    register TkTextLine *linePtr;	/* Text line corresponding to
					 * *lineIndexPtr. */
    int *lineIndexPtr;			/* Points to initial line index,
					 * which is overwritten with final
					 * line index. */
    int *chPtr;				/* Points to initial character index,
					 * which is overwritten with final
					 * character index. */
    int count;				/* How many characters forward to
					 * move.  Must not be negative. */
{
    TkTextLine *nextPtr;
    int bytesInLine;

    while (count > 0) {
	bytesInLine = linePtr->numBytes - *chPtr;
	if (bytesInLine > count) {
	    *chPtr += count;
	    return;
	}
	nextPtr = TkBTreeNextLine(linePtr);
	if (nextPtr == NULL) {
	    *chPtr = linePtr->numBytes - 1;
	    return;
	}
	*chPtr = 0;
	*lineIndexPtr += 1;
	linePtr = nextPtr;
	count -= bytesInLine;
    }
}

/*
 *----------------------------------------------------------------------
 *
 * BackwardChars --
 *
 *	Given a position in a text widget, this procedure computes
 *	a new position that is "count" characters earlier than the given
 *	position.
 *
 * Results:
 *	*LineIndexPtr and *chPtr are overwritten with new values
 *	corresponding to the new position.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

static void
BackwardChars(textPtr, linePtr, lineIndexPtr, chPtr, count)
    TkText *textPtr;			/* Information about text widget. */
    register TkTextLine *linePtr;	/* Text line corresponding to
					 * *lineIndexPtr. */
    int *lineIndexPtr;			/* Points to initial line index,
					 * which is overwritten with final
					 * line index. */
    int *chPtr;				/* Points to initial character index,
					 * which is overwritten with final
					 * character index. */
    int count;				/* How many characters backward to
					 * move.  Must not be negative. */
{
    int bytesInLine;

    while (count > 0) {
	bytesInLine = *chPtr;
	if (bytesInLine >= count) {
	    *chPtr -= count;
	    return;
	}
	if (*lineIndexPtr <= 0) {
	    *chPtr = 0;
	    return;
	}
	*lineIndexPtr -= 1;
	linePtr = TkBTreeFindLine(textPtr->tree, *lineIndexPtr);
	count -= bytesInLine;
	*chPtr = linePtr->numBytes;
    }
}

/*
 *----------------------------------------------------------------------
 *
 * StartEnd --
 *
 *	This procedure handles modifiers like "wordstart" and "lineend"
 *	to adjust indices forwards or backwards.
 *
 * Results:
 *	If the modifier is successfully parsed then the return value
 *	is the address of the first character after the modifier, and
 *	*lineIndexPtr and *chPtr are updated to reflect the modifier.
 *	If there is a syntax error in the modifier then NULL is returned.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

static char *
StartEnd(textPtr, string, lineIndexPtr, chPtr)
    TkText *textPtr;		/* Information about widget that index
				 * refers to. */
    char *string;		/* String to parse for additional info
				 * about modifier (count and units). 
				 * Points to first character of modifer
				 * word. */
    int *lineIndexPtr;		/* Points to current line index, which will
				 * be updated to reflect modifier. */
    int *chPtr;			/* Points to current character index, which
				 * will be updated to reflect modifier. */
{
    char *p, c;
    int length;
    register TkTextLine *linePtr;

    /*
     * Find the end of the modifier word.
     */

    for (p = string; isalnum(*p); p++) {
	/* Empty loop body. */
    }
    length = p-string;
    linePtr = TkTextRoundIndex(textPtr, lineIndexPtr, chPtr);
    if ((*string == 'l') && (strncmp(string, "lineend", length) == 0)
	    && (length >= 5)) {
	*chPtr = linePtr->numBytes - 1;
    } else if ((*string == 'l') && (strncmp(string, "linestart", length) == 0)
	    && (length >= 5)) {
	*chPtr = 0;
    } else if ((*string == 'w') && (strncmp(string, "wordend", length) == 0)
	    && (length >= 5)) {
	c = linePtr->bytes[*chPtr];
	if (!isalnum(c) && (c != '_')) {
	    if (*chPtr >= (linePtr->numBytes - 1)) {
		/*
		 * End of line:  go to start of next line unless this is the
		 * last line in the text.
		 */

		if (TkBTreeNextLine(linePtr) != NULL) {
		    *lineIndexPtr += 1;
		    *chPtr = 0;
		}
	    } else {
		*chPtr += 1;
	    }
	} else {
	    do {
		*chPtr += 1;
		c = linePtr->bytes[*chPtr];
	    } while (isalnum(c) || (c == '_'));
	}
    } else if ((*string == 'w') && (strncmp(string, "wordstart", length) == 0)
	    && (length >= 5)) {
	c = linePtr->bytes[*chPtr];
	if (isalnum(c) || (c == '_')) {
	    while (*chPtr > 0) {
		c = linePtr->bytes[(*chPtr) - 1];
		if (!isalnum(c) && (c != '_')) {
		    break;
		}
		*chPtr -= 1;
	    }
	}
    } else {
	return NULL;
    }
    return p;
}
