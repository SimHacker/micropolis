/* 
 * tclParse.c --
 *
 *	This file contains a collection of procedures that are used
 *	to parse Tcl commands or parts of commands (like quoted
 *	strings or nested sub-commands).
 *
 * Copyright 1991 Regents of the University of California.
 * Permission to use, copy, modify, and distribute this
 * software and its documentation for any purpose and without
 * fee is hereby granted, provided that the above copyright
 * notice appear in all copies.  The University of California
 * makes no representations about the suitability of this
 * software for any purpose.  It is provided "as is" without
 * express or implied warranty.
 */

#ifndef lint
static char rcsid[] = "$Header: /user6/ouster/tcl/RCS/tclParse.c,v 1.21 92/06/08 09:32:37 ouster Exp $ SPRITE (Berkeley)";
#endif

#include "tclint.h"

/*
 * The following table assigns a type to each character.  Only types
 * meaningful to Tcl parsing are represented here.  The table indexes
 * all 256 characters, with the negative ones first, then the positive
 * ones.
 */

char tclTypeTable[] = {
    TCL_NORMAL,        TCL_NORMAL,        TCL_NORMAL,        TCL_NORMAL,
    TCL_NORMAL,        TCL_NORMAL,        TCL_NORMAL,        TCL_NORMAL,
    TCL_NORMAL,        TCL_NORMAL,        TCL_NORMAL,        TCL_NORMAL,
    TCL_NORMAL,        TCL_NORMAL,        TCL_NORMAL,        TCL_NORMAL,
    TCL_NORMAL,        TCL_NORMAL,        TCL_NORMAL,        TCL_NORMAL,
    TCL_NORMAL,        TCL_NORMAL,        TCL_NORMAL,        TCL_NORMAL,
    TCL_NORMAL,        TCL_NORMAL,        TCL_NORMAL,        TCL_NORMAL,
    TCL_NORMAL,        TCL_NORMAL,        TCL_NORMAL,        TCL_NORMAL,
    TCL_NORMAL,        TCL_NORMAL,        TCL_NORMAL,        TCL_NORMAL,
    TCL_NORMAL,        TCL_NORMAL,        TCL_NORMAL,        TCL_NORMAL,
    TCL_NORMAL,        TCL_NORMAL,        TCL_NORMAL,        TCL_NORMAL,
    TCL_NORMAL,        TCL_NORMAL,        TCL_NORMAL,        TCL_NORMAL,
    TCL_NORMAL,        TCL_NORMAL,        TCL_NORMAL,        TCL_NORMAL,
    TCL_NORMAL,        TCL_NORMAL,        TCL_NORMAL,        TCL_NORMAL,
    TCL_NORMAL,        TCL_NORMAL,        TCL_NORMAL,        TCL_NORMAL,
    TCL_NORMAL,        TCL_NORMAL,        TCL_NORMAL,        TCL_NORMAL,
    TCL_NORMAL,        TCL_NORMAL,        TCL_NORMAL,        TCL_NORMAL,
    TCL_NORMAL,        TCL_NORMAL,        TCL_NORMAL,        TCL_NORMAL,
    TCL_NORMAL,        TCL_NORMAL,        TCL_NORMAL,        TCL_NORMAL,
    TCL_NORMAL,        TCL_NORMAL,        TCL_NORMAL,        TCL_NORMAL,
    TCL_NORMAL,        TCL_NORMAL,        TCL_NORMAL,        TCL_NORMAL,
    TCL_NORMAL,        TCL_NORMAL,        TCL_NORMAL,        TCL_NORMAL,
    TCL_NORMAL,        TCL_NORMAL,        TCL_NORMAL,        TCL_NORMAL,
    TCL_NORMAL,        TCL_NORMAL,        TCL_NORMAL,        TCL_NORMAL,
    TCL_NORMAL,        TCL_NORMAL,        TCL_NORMAL,        TCL_NORMAL,
    TCL_NORMAL,        TCL_NORMAL,        TCL_NORMAL,        TCL_NORMAL,
    TCL_NORMAL,        TCL_NORMAL,        TCL_NORMAL,        TCL_NORMAL,
    TCL_NORMAL,        TCL_NORMAL,        TCL_NORMAL,        TCL_NORMAL,
    TCL_NORMAL,        TCL_NORMAL,        TCL_NORMAL,        TCL_NORMAL,
    TCL_NORMAL,        TCL_NORMAL,        TCL_NORMAL,        TCL_NORMAL,
    TCL_NORMAL,        TCL_NORMAL,        TCL_NORMAL,        TCL_NORMAL,
    TCL_NORMAL,        TCL_NORMAL,        TCL_NORMAL,        TCL_NORMAL,
    TCL_COMMAND_END,   TCL_NORMAL,        TCL_NORMAL,        TCL_NORMAL,
    TCL_NORMAL,        TCL_NORMAL,        TCL_NORMAL,        TCL_NORMAL,
    TCL_NORMAL,        TCL_SPACE,         TCL_COMMAND_END,   TCL_SPACE,
    TCL_SPACE,         TCL_SPACE,         TCL_NORMAL,        TCL_NORMAL,
    TCL_NORMAL,        TCL_NORMAL,        TCL_NORMAL,        TCL_NORMAL,
    TCL_NORMAL,        TCL_NORMAL,        TCL_NORMAL,        TCL_NORMAL,
    TCL_NORMAL,        TCL_NORMAL,        TCL_NORMAL,        TCL_NORMAL,
    TCL_NORMAL,        TCL_NORMAL,        TCL_NORMAL,        TCL_NORMAL,
    TCL_SPACE,         TCL_NORMAL,        TCL_QUOTE,         TCL_NORMAL,
    TCL_DOLLAR,        TCL_NORMAL,        TCL_NORMAL,        TCL_NORMAL,
    TCL_NORMAL,        TCL_NORMAL,        TCL_NORMAL,        TCL_NORMAL,
    TCL_NORMAL,        TCL_NORMAL,        TCL_NORMAL,        TCL_NORMAL,
    TCL_NORMAL,        TCL_NORMAL,        TCL_NORMAL,        TCL_NORMAL,
    TCL_NORMAL,        TCL_NORMAL,        TCL_NORMAL,        TCL_NORMAL,
    TCL_NORMAL,        TCL_NORMAL,        TCL_NORMAL,        TCL_COMMAND_END,
    TCL_NORMAL,        TCL_NORMAL,        TCL_NORMAL,        TCL_NORMAL,
    TCL_NORMAL,        TCL_NORMAL,        TCL_NORMAL,        TCL_NORMAL,
    TCL_NORMAL,        TCL_NORMAL,        TCL_NORMAL,        TCL_NORMAL,
    TCL_NORMAL,        TCL_NORMAL,        TCL_NORMAL,        TCL_NORMAL,
    TCL_NORMAL,        TCL_NORMAL,        TCL_NORMAL,        TCL_NORMAL,
    TCL_NORMAL,        TCL_NORMAL,        TCL_NORMAL,        TCL_NORMAL,
    TCL_NORMAL,        TCL_NORMAL,        TCL_NORMAL,        TCL_NORMAL,
    TCL_NORMAL,        TCL_NORMAL,        TCL_NORMAL,        TCL_OPEN_BRACKET,
    TCL_BACKSLASH,     TCL_COMMAND_END,   TCL_NORMAL,        TCL_NORMAL,
    TCL_NORMAL,        TCL_NORMAL,        TCL_NORMAL,        TCL_NORMAL,
    TCL_NORMAL,        TCL_NORMAL,        TCL_NORMAL,        TCL_NORMAL,
    TCL_NORMAL,        TCL_NORMAL,        TCL_NORMAL,        TCL_NORMAL,
    TCL_NORMAL,        TCL_NORMAL,        TCL_NORMAL,        TCL_NORMAL,
    TCL_NORMAL,        TCL_NORMAL,        TCL_NORMAL,        TCL_NORMAL,
    TCL_NORMAL,        TCL_NORMAL,        TCL_NORMAL,        TCL_NORMAL,
    TCL_NORMAL,        TCL_NORMAL,        TCL_NORMAL,        TCL_OPEN_BRACE,
    TCL_NORMAL,        TCL_CLOSE_BRACE,   TCL_NORMAL,        TCL_NORMAL,
};

/*
 * Function prototypes for procedures local to this file:
 */

static char *	QuoteEnd _ANSI_ARGS_((char *string, int term));
static char *	VarNameEnd _ANSI_ARGS_((char *string));

/*
 *----------------------------------------------------------------------
 *
 * Tcl_Backslash --
 *
 *	Figure out how to handle a backslash sequence.
 *
 * Results:
 *	The return value is the character that should be substituted
 *	in place of the backslash sequence that starts at src, or 0
 *	if the backslash sequence should be replace by nothing (e.g.
 *	backslash followed by newline).  If readPtr isn't NULL then
 *	it is filled in with a count of the number of characters in
 *	the backslash sequence.  Note:  if the backslash isn't followed
 *	by characters that are understood here, then the backslash
 *	sequence is only considered to be one character long, and it
 *	is replaced by a backslash char.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

char
Tcl_Backslash(src, readPtr)
    char *src;			/* Points to the backslash character of
				 * a backslash sequence. */
    int *readPtr;		/* Fill in with number of characters read
				 * from src, unless NULL. */
{
    register char *p = src+1;
    char result;
    int count;

    count = 2;

    switch (*p) {
	case 'b':
	    result = '\b';
	    break;
	case 'e':
	    result = 033;
	    break;
	case 'f':
	    result = '\f';
	    break;
	case 'n':
	    result = '\n';
	    break;
	case 'r':
	    result = '\r';
	    break;
	case 't':
	    result = '\t';
	    break;
	case 'v':
	    result = '\v';
	    break;
	case 'C':
	    p++;
	    if (isspace(*p) || (*p == 0)) {
		result = 'C';
		count = 1;
		break;
	    }
	    count = 3;
	    if (*p == 'M') {
		p++;
		if (isspace(*p) || (*p == 0)) {
		    result = 'M' & 037;
		    break;
		}
		count = 4;
		result = (*p & 037) | 0200;
		break;
	    }
	    count = 3;
	    result = *p & 037;
	    break;
	case 'M':
	    p++;
	    if (isspace(*p) || (*p == 0)) {
		result = 'M';
		count = 1;
		break;
	    }
	    count = 3;
	    result = *p + 0200;
	    break;
	case '}':
	case '{':
	case ']':
	case '[':
	case '$':
	case ' ':
	case ';':
	case '"':
	case '\\':
	    result = *p;
	    break;
	case '\n':
	    result = 0;
	    break;
	default:
	    if (isdigit(*p)) {
		result = *p - '0';
		p++;
		if (!isdigit(*p)) {
		    break;
		}
		count = 3;
		result = (result << 3) + (*p - '0');
		p++;
		if (!isdigit(*p)) {
		    break;
		}
		count = 4;
		result = (result << 3) + (*p - '0');
		break;
	    }
	    result = '\\';
	    count = 1;
	    break;
    }

    if (readPtr != NULL) {
	*readPtr = count;
    }
    return result;
}

/*
 *--------------------------------------------------------------
 *
 * TclParseQuotes --
 *
 *	This procedure parses a double-quoted string such as a
 *	quoted Tcl command argument or a quoted value in a Tcl
 *	expression.  This procedure is also used to parse array
 *	element names within parentheses, or anything else that
 *	needs all the substitutions that happen in quotes.
 *
 * Results:
 *	The return value is a standard Tcl result, which is
 *	TCL_OK unless there was an error while parsing the
 *	quoted string.  If an error occurs then interp->result
 *	contains a standard error message.  *TermPtr is filled
 *	in with the address of the character just after the
 *	last one successfully processed;  this is usually the
 *	character just after the matching close-quote.  The
 *	fully-substituted contents of the quotes are stored in
 *	standard fashion in *pvPtr, null-terminated with
 *	pvPtr->next pointing to the terminating null character.
 *
 * Side effects:
 *	The buffer space in pvPtr may be enlarged by calling its
 *	expandProc.
 *
 *--------------------------------------------------------------
 */

int
TclParseQuotes(interp, string, termChar, flags, termPtr, pvPtr)
    Tcl_Interp *interp;		/* Interpreter to use for nested command
				 * evaluations and error messages. */
    char *string;		/* Character just after opening double-
				 * quote. */
    int termChar;		/* Character that terminates "quoted" string
				 * (usually double-quote, but sometimes
				 * right-paren or something else). */
    int flags;			/* Flags to pass to nested Tcl_Eval calls. */
    char **termPtr;		/* Store address of terminating character
				 * here. */
    ParseValue *pvPtr;		/* Information about where to place
				 * fully-substituted result of parse. */
{
    register char *src, *dst, c;

    src = string;
    dst = pvPtr->next;

    while (1) {
	if (dst == pvPtr->end) {
	    /*
	     * Target buffer space is about to run out.  Make more space.
	     */

	    pvPtr->next = dst;
	    (*pvPtr->expandProc)(pvPtr, 1);
	    dst = pvPtr->next;
	}

	c = *src;
	src++;
	if (c == termChar) {
	    *dst = '\0';
	    pvPtr->next = dst;
	    *termPtr = src;
	    return TCL_OK;
	} else if (CHAR_TYPE(c) == TCL_NORMAL) {
	    copy:
	    *dst = c;
	    dst++;
	    continue;
	} else if (c == '$') {
	    int length;
	    char *value;

	    value = Tcl_ParseVar(interp, src-1, termPtr);
	    if (value == NULL) {
		return TCL_ERROR;
	    }
	    src = *termPtr;
	    length = strlen(value);
	    if ((pvPtr->end - dst) <= length) {
		pvPtr->next = dst;
		(*pvPtr->expandProc)(pvPtr, length);
		dst = pvPtr->next;
	    }
	    strcpy(dst, value);
	    dst += length;
	    continue;
	} else if (c == '[') {
	    int result;

	    pvPtr->next = dst;
	    result = TclParseNestedCmd(interp, src, flags, termPtr, pvPtr);
	    if (result != TCL_OK) {
		return result;
	    }
	    src = *termPtr;
	    dst = pvPtr->next;
	    continue;
	} else if (c == '\\') {
	    int numRead;

	    src--;
	    *dst = Tcl_Backslash(src, &numRead);
	    if (*dst != 0) {
		dst++;
	    }
	    src += numRead;
	    continue;
	} else if (c == '\0') {
	    Tcl_ResetResult(interp);
	    sprintf(interp->result, "missing %c", termChar);
	    *termPtr = string-1;
	    return TCL_ERROR;
	} else {
	    goto copy;
	}
    }
}

/*
 *--------------------------------------------------------------
 *
 * TclParseNestedCmd --
 *
 *	This procedure parses a nested Tcl command between
 *	brackets, returning the result of the command.
 *
 * Results:
 *	The return value is a standard Tcl result, which is
 *	TCL_OK unless there was an error while executing the
 *	nested command.  If an error occurs then interp->result
 *	contains a standard error message.  *TermPtr is filled
 *	in with the address of the character just after the
 *	last one processed;  this is usually the character just
 *	after the matching close-bracket, or the null character
 *	at the end of the string if the close-bracket was missing
 *	(a missing close bracket is an error).  The result returned
 *	by the command is stored in standard fashion in *pvPtr,
 *	null-terminated, with pvPtr->next pointing to the null
 *	character.
 *
 * Side effects:
 *	The storage space at *pvPtr may be expanded.
 *
 *--------------------------------------------------------------
 */

int
TclParseNestedCmd(interp, string, flags, termPtr, pvPtr)
    Tcl_Interp *interp;		/* Interpreter to use for nested command
				 * evaluations and error messages. */
    char *string;		/* Character just after opening bracket. */
    int flags;			/* Flags to pass to nested Tcl_Eval. */
    char **termPtr;		/* Store address of terminating character
				 * here. */
    register ParseValue *pvPtr;	/* Information about where to place
				 * result of command. */
{
    int result, length, shortfall;
    Interp *iPtr = (Interp *) interp;

    result = Tcl_Eval(interp, string, flags | TCL_BRACKET_TERM, termPtr);
    if (result != TCL_OK) {
	/*
	 * The increment below results in slightly cleaner message in
	 * the errorInfo variable (the close-bracket will appear).
	 */

	if (**termPtr == ']') {
	    *termPtr += 1;
	}
	return result;
    }
    (*termPtr) += 1;
    length = strlen(iPtr->result);
    shortfall = length + 1 - (pvPtr->end - pvPtr->next);
    if (shortfall > 0) {
	(*pvPtr->expandProc)(pvPtr, shortfall);
    }
    strcpy(pvPtr->next, iPtr->result);
    pvPtr->next += length;
    Tcl_FreeResult(iPtr);
    iPtr->result = iPtr->resultSpace;
    iPtr->resultSpace[0] = '\0';
    return TCL_OK;
}

/*
 *--------------------------------------------------------------
 *
 * TclParseBraces --
 *
 *	This procedure scans the information between matching
 *	curly braces.
 *
 * Results:
 *	The return value is a standard Tcl result, which is
 *	TCL_OK unless there was an error while parsing string.
 *	If an error occurs then interp->result contains a
 *	standard error message.  *TermPtr is filled
 *	in with the address of the character just after the
 *	last one successfully processed;  this is usually the
 *	character just after the matching close-brace.  The
 *	information between curly braces is stored in standard
 *	fashion in *pvPtr, null-terminated with pvPtr->next
 *	pointing to the terminating null character.
 *
 * Side effects:
 *	The storage space at *pvPtr may be expanded.
 *
 *--------------------------------------------------------------
 */

int
TclParseBraces(interp, string, termPtr, pvPtr)
    Tcl_Interp *interp;		/* Interpreter to use for nested command
				 * evaluations and error messages. */
    char *string;		/* Character just after opening bracket. */
    char **termPtr;		/* Store address of terminating character
				 * here. */
    register ParseValue *pvPtr;	/* Information about where to place
				 * result of command. */
{
    int level;
    register char *src, *dst, *end;
    register char c;

    src = string;
    dst = pvPtr->next;
    end = pvPtr->end;
    level = 1;

    /*
     * Copy the characters one at a time to the result area, stopping
     * when the matching close-brace is found.
     */

    while (1) {
	c = *src;
	src++;
	if (dst == end) {
	    pvPtr->next = dst;
	    (*pvPtr->expandProc)(pvPtr, 20);
	    dst = pvPtr->next;
	    end = pvPtr->end;
	}
	*dst = c;
	dst++;
	if (CHAR_TYPE(c) == TCL_NORMAL) {
	    continue;
	} else if (c == '{') {
	    level++;
	} else if (c == '}') {
	    level--;
	    if (level == 0) {
		dst--;			/* Don't copy the last close brace. */
		break;
	    }
	} else if (c == '\\') {
	    int count;

	    /*
	     * Must always squish out backslash-newlines, even when in
	     * braces.  This is needed so that this sequence can appear
	     * anywhere in a command, such as the middle of an expression.
	     */

	    if (*src == '\n') {
		dst--;
		src++;
	    } else {
		(void) Tcl_Backslash(src-1, &count);
		while (count > 1) {
                    if (dst == end) {
                        pvPtr->next = dst;
                        (*pvPtr->expandProc)(pvPtr, 20);
                        dst = pvPtr->next;
                        end = pvPtr->end;
                    }
		    *dst = *src;
		    dst++;
		    src++;
		    count--;
		}
	    }
	} else if (c == '\0') {
	    Tcl_SetResult(interp, "missing close-brace", TCL_STATIC);
	    *termPtr = string-1;
	    return TCL_ERROR;
	}
    }

    *dst = '\0';
    pvPtr->next = dst;
    *termPtr = src;
    return TCL_OK;
}

/*
 *--------------------------------------------------------------
 *
 * TclParseWords --
 *
 *	This procedure parses one or more words from a command
 *	string and creates argv-style pointers to fully-substituted
 *	copies of those words.
 *
 * Results:
 *	The return value is a standard Tcl result.
 *	
 *	*argcPtr is modified to hold a count of the number of words
 *	successfully parsed, which may be 0.  At most maxWords words
 *	will be parsed.  If 0 <= *argcPtr < maxWords then it
 *	means that a command separator was seen.  If *argcPtr
 *	is maxWords then it means that a command separator was
 *	not seen yet.
 *
 *	*TermPtr is filled in with the address of the character
 *	just after the last one successfully processed in the
 *	last word.  This is either the command terminator (if
 *	*argcPtr < maxWords), the character just after the last
 *	one in a word (if *argcPtr is maxWords), or the vicinity
 *	of an error (if the result is not TCL_OK).
 *	
 *	The pointers at *argv are filled in with pointers to the
 *	fully-substituted words, and the actual contents of the
 *	words are copied to the buffer at pvPtr.
 *
 *	If an error occurrs then an error message is left in
 *	interp->result and the information at *argv, *argcPtr,
 *	and *pvPtr may be incomplete.
 *
 * Side effects:
 *	The buffer space in pvPtr may be enlarged by calling its
 *	expandProc.
 *
 *--------------------------------------------------------------
 */

int
TclParseWords(interp, string, flags, maxWords, termPtr, argcPtr, argv, pvPtr)
    Tcl_Interp *interp;		/* Interpreter to use for nested command
				 * evaluations and error messages. */
    char *string;		/* First character of word. */
    int flags;			/* Flags to control parsing (same values as
				 * passed to Tcl_Eval). */
    int maxWords;		/* Maximum number of words to parse. */
    char **termPtr;		/* Store address of terminating character
				 * here. */
    int *argcPtr;		/* Filled in with actual number of words
				 * parsed. */
    char **argv;		/* Store addresses of individual words here. */
    register ParseValue *pvPtr;	/* Information about where to place
				 * fully-substituted word. */
{
    register char *src, *dst;
    register char c;
    int type, result, argc;
    char *oldBuffer;		/* Used to detect when pvPtr's buffer gets
				 * reallocated, so we can adjust all of the
				 * argv pointers. */

    src = string;
    oldBuffer = pvPtr->buffer;
    dst = pvPtr->next;
    for (argc = 0; argc < maxWords; argc++) {
	argv[argc] = dst;

	/*
	 * Skip leading space.
	 */
    
	skipSpace:
	c = *src;
	type = CHAR_TYPE(c);
	while (type == TCL_SPACE) {
	    src++;
	    c = *src;
	    type = CHAR_TYPE(c);
	}
    
	/*
	 * Handle the normal case (i.e. no leading double-quote or brace).
	 */

	if (type == TCL_NORMAL) {
	    normalArg:
	    while (1) {
		if (dst == pvPtr->end) {
		    /*
		     * Target buffer space is about to run out.  Make
		     * more space.
		     */
	
		    pvPtr->next = dst;
		    (*pvPtr->expandProc)(pvPtr, 1);
		    dst = pvPtr->next;
		}
	
		if (type == TCL_NORMAL) {
		    copy:
		    *dst = c;
		    dst++;
		    src++;
		} else if (type == TCL_SPACE) {
		    goto wordEnd;
		} else if (type == TCL_DOLLAR) {
		    int length;
		    char *value;
	
		    value = Tcl_ParseVar(interp, src, termPtr);
		    if (value == NULL) {
			return TCL_ERROR;
		    }
		    src = *termPtr;
		    length = strlen(value);
		    if ((pvPtr->end - dst) <= length) {
			pvPtr->next = dst;
			(*pvPtr->expandProc)(pvPtr, length);
			dst = pvPtr->next;
		    }
		    strcpy(dst, value);
		    dst += length;
		} else if (type == TCL_COMMAND_END) {
		    if ((c == ']') && !(flags & TCL_BRACKET_TERM)) {
			goto copy;
		    }

		    /*
		     * End of command;  simulate a word-end first, so
		     * that the end-of-command can be processed as the
		     * first thing in a new word.
		     */

		    goto wordEnd;
		} else if (type == TCL_OPEN_BRACKET) {
		    pvPtr->next = dst;
		    result = TclParseNestedCmd(interp, src+1, flags, termPtr,
			    pvPtr);
		    if (result != TCL_OK) {
			return result;
		    }
		    src = *termPtr;
		    dst = pvPtr->next;
		} else if (type == TCL_BACKSLASH) {
		    int numRead;
    
		    *dst = Tcl_Backslash(src, &numRead);
		    if (*dst != 0) {
			dst++;
		    }
		    src += numRead;
		} else {
		    goto copy;
		}
		c = *src;
		type = CHAR_TYPE(c);
	    }
	} else {
    
	    /*
	     * Check for the end of the command.
	     */
	
	    if (type == TCL_COMMAND_END) {
		if (flags & TCL_BRACKET_TERM) {
		    if (c == '\0') {
			Tcl_SetResult(interp, "missing close-bracket",
				TCL_STATIC);
			return TCL_ERROR;
		    }
		} else {
		    if (c == ']') {
			goto normalArg;
		    }
		}
		goto done;
	    }
	
	    /*
	     * Now handle the special cases: open braces, double-quotes,
	     * and backslash-newline.
	     */

	    pvPtr->next = dst;
	    if (type == TCL_QUOTE) {
		result = TclParseQuotes(interp, src+1, '"', flags,
			termPtr, pvPtr);
	    } else if (type == TCL_OPEN_BRACE) {
		result = TclParseBraces(interp, src+1, termPtr, pvPtr);
	    } else if ((type == TCL_BACKSLASH) && (src[1] == '\n')) {
		src += 2;
		goto skipSpace;
	    } else {
		goto normalArg;
	    }
	    if (result != TCL_OK) {
		return result;
	    }
	
	    /*
	     * Back from quotes or braces;  make sure that the terminating
	     * character was the end of the word.  Have to be careful here
	     * to handle continuation lines (i.e. lines ending in backslash).
	     */
	
	    c = **termPtr;
	    if ((c == '\\') && ((*termPtr)[1] == '\n')) {
		c = (*termPtr)[2];
	    }
	    type = CHAR_TYPE(c);
	    if ((type != TCL_SPACE) && (type != TCL_COMMAND_END)) {
		if (*src == '"') {
		    Tcl_SetResult(interp, "extra characters after close-quote",
			    TCL_STATIC);
		} else {
		    Tcl_SetResult(interp, "extra characters after close-brace",
			    TCL_STATIC);
		}
		return TCL_ERROR;
	    }
	    src = *termPtr;
	    dst = pvPtr->next;

	}

	/*
	 * We're at the end of a word, so add a null terminator.  Then
	 * see if the buffer was re-allocated during this word.  If so,
	 * update all of the argv pointers.
	 */

	wordEnd:
	*dst = '\0';
	dst++;
	if (oldBuffer != pvPtr->buffer) {
	    int i;

	    for (i = 0; i <= argc; i++) {
		argv[i] = pvPtr->buffer + (argv[i] - oldBuffer);
	    }
	    oldBuffer = pvPtr->buffer;
	}
    }

    done:
    pvPtr->next = dst;
    *termPtr = src;
    *argcPtr = argc;
    return TCL_OK;
}

/*
 *--------------------------------------------------------------
 *
 * TclExpandParseValue --
 *
 *	This procedure is commonly used as the value of the
 *	expandProc in a ParseValue.  It uses malloc to allocate
 *	more space for the result of a parse.
 *
 * Results:
 *	The buffer space in *pvPtr is reallocated to something
 *	larger, and if pvPtr->clientData is non-zero the old
 *	buffer is freed.  Information is copied from the old
 *	buffer to the new one.
 *
 * Side effects:
 *	None.
 *
 *--------------------------------------------------------------
 */

void
TclExpandParseValue(pvPtr, needed)
    register ParseValue *pvPtr;		/* Information about buffer that
					 * must be expanded.  If the clientData
					 * in the structure is non-zero, it
					 * means that the current buffer is
					 * dynamically allocated. */
    int needed;				/* Minimum amount of additional space
					 * to allocate. */
{
    int newSpace;
    char *new;

    /*
     * Either double the size of the buffer or add enough new space
     * to meet the demand, whichever produces a larger new buffer.
     */

    newSpace = (pvPtr->end - pvPtr->buffer) + 1;
    if (newSpace < needed) {
	newSpace += needed;
    } else {
	newSpace += newSpace;
    }
    new = (char *) ckalloc((unsigned) newSpace);

    /*
     * Copy from old buffer to new, free old buffer if needed, and
     * mark new buffer as malloc-ed.
     */

    memcpy((VOID *) new, (VOID *) pvPtr->buffer, pvPtr->next - pvPtr->buffer);
    pvPtr->next = new + (pvPtr->next - pvPtr->buffer);
    if (pvPtr->clientData != 0) {
	ckfree(pvPtr->buffer);
    }
    pvPtr->buffer = new;
    pvPtr->end = new + newSpace - 1;
    pvPtr->clientData = (ClientData) 1;
}

/*
 *----------------------------------------------------------------------
 *
 * TclWordEnd --
 *
 *	Given a pointer into a Tcl command, find the end of the next
 *	word of the command.
 *
 * Results:
 *	The return value is a pointer to the character just after the
 *	last one that's part of the word pointed to by "start".  This
 *	may be the address of the NULL character at the end of the
 *	string.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

char *
TclWordEnd(start, nested)
    char *start;		/* Beginning of a word of a Tcl command. */
    int nested;			/* Zero means this is a top-level command.
				 * One means this is a nested command (close
				 * brace is a word terminator). */
{
    register char *p;
    int count;

    p = start;
    while (isspace(*p)) {
	p++;
    }

    /*
     * Handle words beginning with a double-quote or a brace.
     */

    if (*p == '"') {
	p = QuoteEnd(p+1, '"');
    } else if (*p == '{') {
	int braces = 1;
	while (braces != 0) {
	    p++;
	    while (*p == '\\') {
		(void) Tcl_Backslash(p, &count);
		p += count;
	    }
	    if (*p == '}') {
		braces--;
	    } else if (*p == '{') {
		braces++;
	    } else if (*p == 0) {
		return p;
	    }
	}
    }

    /*
     * Handle words that don't start with a brace or double-quote.
     * This code is also invoked if the word starts with a brace or
     * double-quote and there is garbage after the closing brace or
     * quote.  This is an error as far as Tcl_Eval is concerned, but
     * for here the garbage is treated as part of the word.
     */

    while (*p != 0) {
	if (*p == '[') {
	    p++;
	    while ((*p != ']') && (*p != 0)) {
		p = TclWordEnd(p, 1);
	    }
	    if (*p == ']') {
		p++;
	    }
	} else if (*p == '\\') {
	    (void) Tcl_Backslash(p, &count);
	    p += count;
	} else if (*p == '$') {
	    p = VarNameEnd(p);
	} else if (*p == ';') {
	    /*
	     * Note:  semi-colon terminates a word
	     * and also counts as a word by itself.
	     */

	    if (p == start) {
		p++;
	    }
	    break;
	} else if (isspace(*p)) {
	    break;
	} else if ((*p == ']') && nested) {
	    break;
	} else {
	    p++;
	}
    }
    return p;
}

/*
 *----------------------------------------------------------------------
 *
 * QuoteEnd --
 *
 *	Given a pointer to a string that obeys the parsing conventions
 *	for quoted things in Tcl, find the end of that quoted thing.
 *	The actual thing may be a quoted argument or a parenthesized
 *	index name.
 *
 * Results:
 *	The return value is a pointer to the character just after the
 *	last one that is part of the quoted string.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

static char *
QuoteEnd(string, term)
    char *string;		/* Pointer to character just after opening
				 * "quote". */
    int term;			/* This character will terminate the
				 * quoted string (e.g. '"' or ')'). */
{
    register char *p = string;
    int count;

    while ((*p != 0) && (*p != term)) {
	if (*p == '\\') {
	    (void) Tcl_Backslash(p, &count);
	    p += count;
	} else if (*p == '[') {
	    p++;
	    while ((*p != ']') && (*p != 0)) {
		p = TclWordEnd(p, 1);
	    }
	    if (*p == ']') {
		p++;
	    }
	} else if (*p == '$') {
	    p = VarNameEnd(p);
	} else {
	    p++;
	}
    }
    return p;
}

/*
 *----------------------------------------------------------------------
 *
 * VarNameEnd --
 *
 *	Given a pointer to a variable reference using $-notation, find
 *	the end of the variable name spec.
 *
 * Results:
 *	The return value is a pointer to the character just after the
 *	last one that is part of the variable name.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

static char *
VarNameEnd(string)
    char *string;		/* Pointer to dollar-sign character. */
{
    register char *p = string+1;

    if (*p == '{') {
	do {
	    p++;
	} while ((*p != '}') && (*p != 0));
    } else {
	while (isalnum(*p) || (*p == '_')) {
	    p++;
	}
	if ((*p == '(') && (p != string+1)) {
	    p = QuoteEnd(p+1, ')');
	}
    }
    return p;
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_ParseVar --
 *
 *	Given a string starting with a $ sign, parse off a variable
 *	name and return its value.
 *
 * Results:
 *	The return value is the contents of the variable given by
 *	the leading characters of string.  If termPtr isn't NULL,
 *	*termPtr gets filled in with the address of the character
 *	just after the last one in the variable specifier.  If the
 *	variable doesn't exist, then the return value is NULL and
 *	an error message will be left in interp->result.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

char *
Tcl_ParseVar(interp, string, termPtr)
    Tcl_Interp *interp;			/* Context for looking up variable. */
    register char *string;		/* String containing variable name.
					 * First character must be "$". */
    char **termPtr;			/* If non-NULL, points to word to fill
					 * in with character just after last
					 * one in the variable specifier. */

{
    char *name1, *name1End, c, *result;
    register char *name2;
#define NUM_CHARS 200
    char copyStorage[NUM_CHARS];
    ParseValue pv;

    /*
     * There are three cases:
     * 1. The $ sign is followed by an open curly brace.  Then the variable
     *    name is everything up to the next close curly brace, and the
     *    variable is a scalar variable.
     * 2. The $ sign is not followed by an open curly brace.  Then the
     *    variable name is everything up to the next character that isn't
     *    a letter, digit, or underscore.  If the following character is an
     *    open parenthesis, then the information between parentheses is
     *    the array element name, which can include any of the substitutions
     *    permissible between quotes.
     * 3. The $ sign is followed by something that isn't a letter, digit,
     *    or underscore:  in this case, there is no variable name, and "$"
     *    is returned.
     */

    name2 = NULL;
    string++;
    if (*string == '{') {
	string++;
	name1 = string;
	while (*string != '}') {
	    if (*string == 0) {
		Tcl_SetResult(interp, "missing close-brace for variable name",
			TCL_STATIC);
		if (termPtr != 0) {
		    *termPtr = string;
		}
		return NULL;
	    }
	    string++;
	}
	name1End = string;
	string++;
    } else {
	name1 = string;
	while (isalnum(*string) || (*string == '_')) {
	    string++;
	}
	if (string == name1) {
	    if (termPtr != 0) {
		*termPtr = string;
	    }
	    return "$";
	}
	name1End = string;
	if (*string == '(') {
	    char *end;

	    /*
	     * Perform substitutions on the array element name, just as
	     * is done for quotes.
	     */

	    pv.buffer = pv.next = copyStorage;
	    pv.end = copyStorage + NUM_CHARS - 1;
	    pv.expandProc = TclExpandParseValue;
	    pv.clientData = (ClientData) NULL;
	    if (TclParseQuotes(interp, string+1, ')', 0, &end, &pv)
		    != TCL_OK) {
		char msg[100];
		sprintf(msg, "\n    (parsing index for array \"%.*s\")",
			string-name1, name1);
		Tcl_AddErrorInfo(interp, msg);
		result = NULL;
		name2 = pv.buffer;
		if (termPtr != 0) {
		    *termPtr = end;
		}
		goto done;
	    }
	    string = end;
	    name2 = pv.buffer;
	}
    }
    if (termPtr != 0) {
	*termPtr = string;
    }

    c = *name1End;
    *name1End = 0;
    result = Tcl_GetVar2(interp, name1, name2, TCL_LEAVE_ERR_MSG);
    *name1End = c;

    done:
    if ((name2 != NULL) && (pv.buffer != copyStorage)) {
	ckfree(pv.buffer);
    }
    return result;
}
