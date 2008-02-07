/*
 * tclXregexp.c --
 *
 * Tcl regular expression pattern matching utilities.
 *-----------------------------------------------------------------------------
 * Copyright 1992 Karl Lehenbauer and Mark Diekhans.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies.  Karl Lehenbauer and
 * Mark Diekhans make no representations about the suitability of this
 * software for any purpose.  It is provided "as is" without express or
 * implied warranty.
 *-----------------------------------------------------------------------------
 * Boyer-Moore code from: 
 *     torek-boyer-moore/27-Aug-90 by
 *     chris@mimsy.umd.edu (Chris Torek)
 *-----------------------------------------------------------------------------
 * $Id: tclXregexp.c,v 2.0 1992/10/16 04:51:08 markd Rel $
 *-----------------------------------------------------------------------------
 */

#include "tclxint.h"
#include "regexp.h"

/*
 * This is declared in tclUtil.c.  Must be set to NULL before compiling
 * a regular expressions.
 */
extern char *tclRegexpError;

/*
 * Meta-characters for regular expression
 */
#define REXP_META                   "^$.[()|?+*\\"
#define REXP_META_NO_BRACKET_NO_OR  "^$.()?+*\\"

#ifndef CHAR_MAX
#    define CHAR_MAX 255
#endif

/*
 * Prototypes of internal functions.
 */

static char *
BoyerMooreCompile _ANSI_ARGS_((char *pat,
                                  int patlen));

static char *
BoyerMooreExecute _ANSI_ARGS_((char     *text,
                               unsigned  textlen,
                               char     *compPtr,
                               unsigned *patLenP));

static int
FindNonRegExpSubStr _ANSI_ARGS_((char  *expression,
                                 char **subStrPtrPtr));


/*
 * Boyer-Moore search: input is `text' (a string) and its length,
 * and a `pattern' (another string) and its length.
 *
 * The linear setup cost of this function is approximately 256 + patlen.
 * Afterwards, however, the average cost is O(textlen/patlen), and the
 * worst case is O(textlen+patlen).
 *
 * The Boyer-Moore algorithm works by observing that, for each position
 * in the text, if the character there does *not* occur somewhere in the
 * search pattern, no comparisons including that character will match.
 * That is, given the text "hello world..." and the pattern "goodbye", the
 * `w' in `world' means that none of `hello w', `ello wo', `llo wor',
 * `lo worl', `o world', ` world.', and `world..' can match.  In fact,
 * exactly patlen strings are certain not to match.  We can discover this
 * simply by looking at the patlen'th character.  Furthermore, even if
 * the text character does occur, it may be that it rules out some number
 * of other matches.  Again, we can discover this by doing the match
 * `backwards'.
 *
 * We set up a table of deltas for each possible character, with
 * delta[character] being patlen for characters not in the pattern,
 * less for characters in the pattern, growing progressively smaller
 * as we near the end of the pattern.  Matching then works as follows:
 *
 *       0         1         2         3
 *       01234567890123456789012345678901234567
 *      "Here is the string being searched into"        (text)
 *       ------                                         (pos = [0..5])
 *      "string"                                        (pat)
 *      654321-                                         (deltas)
 *
 * (the delta for `-' will be derived below).
 *
 * Positions 0..5 end with `i', which is not the `g' we want.  `i' does
 * appear in `string', but two characters before the end.  We skip
 * forward so as to make the `i's match up:
 *
 *      "Here is the string being searched into"        (text)
 *        "string"                                      (pos = [2..7])
 *
 * Next we find that ` ' and `g' do not match.  Since ` ' does not appear
 * in the pattern at all, we can skip forward 6:
 *
 *      "Here is the string being searched into"        (text)
 *              "string"                                (pos = [8..13])
 *
 * Comparing `t' vs `g', we again find no match, and so we obtain the
 * delta for `t', which is 4.  We skip to position 17:
 *
 *      "Here is the string being searched into"        (text)
 *                  "string"                            (pos = [12..17])
 *
 * It thus takes only four steps to move the search point forward to the
 * match, in this case.
 *
 * If the pattern has a recurring character, we must set the delta for
 * that character to the distance of the one closest to the end:
 *
 *      "befuddle the cat"      (text)
 *      "fuddle"                (pos = [0..5])
 *      654321-                 (delta)
 *
 * We want the next search to line the `d's up like this:
 *
 *      "befuddle the cat"      (text)
 *        "fuddle"              (pos = [2..7])
 *
 * and not like this:
 *
 *      "befuddle the cat"      (text)
 *         "fuddle"             (pos = [3..8])
 *
 * so we take the smaller delta for d, i.e., 2.
 *
 * The last task is computing the delta we have noted above as `-':
 *
 *      "candlesticks"          (text)
 *      "hand"                  (pos = [0..3])
 *      4321-                   (delta)
 *
 * Here the `d' in `hand' matches the `d' in `candlesticks', but the
 * strings differ.  Since there are no other `d's in `hand', we know
 * that none of (cand,andl,ndle,dles) can match, and thus we want this
 * delta to be 4 (the length of the pattern).  But if we had, e.g.:
 *
 *      "candlesticks"          (text)
 *      "deed"                  (pos = [0..3])
 *      4321-                   (delta)
 *
 * then we should advance to line up the other `d':
 *
 *      "candlesticks"          (text)
 *         "deed"               (pos = [3..6])
 *
 * As this suggests, the delta should be that for the `d' nearest the
 * end, but not including the end.  This is easily managed by setting up
 * a delta table as follows:
 *
 *      for int:c in [0..255] { delta[c] = patlen; };
 *      for int:x in [0..patlen-1) { delta[pat[x]] = patlen - (x + 1); };
 *
 * delta[pat[patlen-1]] is never written, so the last letter inherits the
 * delta from an earlier iteration or from the previous loop.
 *
 * NB: the nonsense with `deltaspace' below exists merely because gcc
 * does a horrible job of common subexpression elimination (it does not
 * notice that the array is at a constant stack address).
 */

struct compiled_search_struct {
        unsigned patlen;
        unsigned deltaspace[CHAR_MAX + 1];
};


static char *
BoyerMooreCompile (pat, patlen)
    char *pat;
    int   patlen;
{
        register unsigned char *p, *t;
        register unsigned i, p1, j, *delta;
        struct compiled_search_struct *cp;
        int alloc_len;

        /*
         * Algorithm fails if pattern is empty.
         */
        if ((p1 = patlen) == 0)
                return (NULL);

        alloc_len = sizeof(struct compiled_search_struct) + patlen + 1;
        cp = (struct compiled_search_struct *) ckalloc (alloc_len);

        strncpy((char *)cp+sizeof(struct compiled_search_struct), pat, patlen);
        *((char *)cp+alloc_len-1) = '\0';

        /* set up deltas */
        delta = cp->deltaspace;

        for (i = 0; i <= CHAR_MAX; i++)
                delta[i] = p1;

        for (p = (unsigned char *)pat, i = p1; --i > 0;)
                delta[*p++] = i;

        cp->patlen = patlen;
        return((char*) cp);
}

static char *
BoyerMooreExecute (text, textlen, compPtr, patLenP)
        char     *text;
        unsigned  textlen;
        char     *compPtr;
        unsigned *patLenP;
{
        register unsigned char *p, *t;
        struct compiled_search_struct *csp = 
        	(struct compiled_search_struct*) compPtr;
        register unsigned i, p1, j, *delta = csp->deltaspace;
        char *pat;
        unsigned patlen;

        *patLenP = p1 = patlen = csp->patlen;
        /* code below fails (whenever i is unsigned) if pattern too long */
        if (p1 > textlen)
                return (NULL);

        pat = (char *)csp + sizeof(struct compiled_search_struct);
        /*
         * From now on, we want patlen - 1.
         * In the loop below, p points to the end of the pattern,
         * t points to the end of the text to be tested against the
         * pattern, and i counts the amount of text remaining, not
         * including the part to be tested.
         */
        p1--;
        p = (unsigned char *)pat + p1;
        t = (unsigned char *)text + p1;
        i = textlen - patlen;
        for (;;) {
                if (*p == *t && 
                    memcmp((p - p1), (t - p1), p1) == 0)
                        return ((char *)t - p1);
                j = delta[*t];
                if (i < j)
                        break;
                i -= j;
                t += j;
        }
        return (NULL);
}


/*
 *-----------------------------------------------------------------------------
 *
 * Tcl_RegExpClean --
 *     Free all resources associated with a regular expression info 
 *     structure..
 *
 *-----------------------------------------------------------------------------
 */
void
Tcl_RegExpClean (regExpPtr)
    regexp_pt regExpPtr;
{
    if (regExpPtr->progPtr != NULL)
    	ckfree ((char *) regExpPtr->progPtr);
    if (regExpPtr->boyerMoorePtr != NULL)
    	ckfree ((char *) regExpPtr->boyerMoorePtr);
}

/*
 *-----------------------------------------------------------------------------
 *
 * FindNonRegExpSubStr
 *     Find the largest substring that does not have any regular 
 *     expression meta-characters and is not located within `[...]'.
 *     If the regexp contains an or (|), zero is returned, as the 
 *     Boyer-Moore optimization does not work, since there are actually
 *     multiple patterns.  The real solution is to build the Boyer-Moore
 *     into the regular expression code.
 *-----------------------------------------------------------------------------
 */
static int
FindNonRegExpSubStr (expression, subStrPtrPtr)
    char  *expression;
    char **subStrPtrPtr;
{
    register char *subStrPtr = NULL;
    register char  subStrLen = 0;
    register char *scanPtr   = expression;
    register int   len;

    while (*scanPtr != '\0') {
        len = strcspn (scanPtr, REXP_META);
        /*
         * If we are at a meta-character, by-pass till non-meta.  If we hit
         * a `[' then by-pass the entire `[...]' range, but be careful, could
         * have omitted `]'.  In a `|' is encountered (except in brackets),'
         * we are through.
         */
        if (len == 0) {
            scanPtr += strspn (scanPtr, REXP_META_NO_BRACKET_NO_OR);
            if (*scanPtr == '|')
                return 0;
            if (*scanPtr == '[') {
                scanPtr += strcspn (scanPtr, "]");
                if (*scanPtr == ']')
                    scanPtr++;
            }          
        } else {
            if (len > subStrLen) {
                subStrPtr = scanPtr;
                subStrLen = len;
            }
            scanPtr += len;
        }
    }
    *subStrPtrPtr = subStrPtr;
    return subStrLen;
}

/*
 *-----------------------------------------------------------------------------
 *
 * Tcl_RegExpCompile --
 *     Compile a regular expression.
 *
 * Parameters:
 *     o regExpPtr - Used to hold info on this regular expression.  If the
 *       structure is being reused, it Tcl_RegExpClean should be called first.
 *     o expression - Regular expression to compile.
 *     o flags - The following flags are recognized:
 *         o REXP_NO_CASE - Comparison will be regardless of case.
 *         o REXP_BOTH_ALGORITHMS - If specified, a Boyer-Moore expression is 
 *           compiled for the largest substring of the expression that does
 *           not contain any meta-characters.  This is slows compiling, but
 *           speeds up large searches.
 *
 * Results:
 *     Standard TCL results.
 *-----------------------------------------------------------------------------
 */
int
Tcl_RegExpCompile (interp, regExpPtr, expression, flags)
    Tcl_Interp  *interp;
    regexp_pt    regExpPtr;
    char        *expression;
    int          flags;
{
    char *expBuf;
    int   anyMeta;

    if (*expression == '\0') {
        Tcl_AppendResult (interp, "Null regular expression", (char *) NULL);
        return TCL_ERROR;
    }

    regExpPtr->progPtr = NULL;
    regExpPtr->boyerMoorePtr = NULL;
    regExpPtr->noCase = flags & REXP_NO_CASE;

    if (flags & REXP_NO_CASE) {
        expBuf = ckalloc (strlen (expression) + 1);
        Tcl_DownShift (expBuf, expression);
    } else
        expBuf = expression;

    anyMeta = strpbrk (expBuf, REXP_META) != NULL;

    /*
     * If no meta-characters, use Boyer-Moore string matching only.
     */
    if (!anyMeta) {
        regExpPtr->boyerMoorePtr = BoyerMooreCompile (expBuf, strlen (expBuf));
        goto okExitPoint;
    }
 
    /*
     * Build a Boyer-Moore on the largest non-meta substring, if requested,
     * and the reg-exp does not contain a `|' (or).  If less that three
     * characters in the string, don't use B-M, as it seems not optimal at
     * this point.
     */
    if (flags & REXP_BOTH_ALGORITHMS) {
        char *subStrPtr;
        int   subStrLen;
        
        subStrLen = FindNonRegExpSubStr (expBuf, &subStrPtr);
        if (subStrLen > 2)
            regExpPtr->boyerMoorePtr = 
                BoyerMooreCompile (subStrPtr, subStrLen);
    }
    
    /*
     * Compile meta-character containing regular expression.
     */
    tclRegexpError = NULL;
    regExpPtr->progPtr = regcomp (expBuf);
    if (tclRegexpError != NULL) {
        if (flags & REXP_NO_CASE)
            ckfree (expBuf);
        Tcl_AppendResult (interp, "error in regular expression: ", 
                          tclRegexpError, (char *) NULL);
        if (flags & REXP_NO_CASE)
            ckfree (expBuf);
        Tcl_RegExpClean (regExpPtr);
    }
  
okExitPoint: 
    if (flags & REXP_NO_CASE)
        ckfree (expBuf);
    return TCL_OK;

}

/*
 *-----------------------------------------------------------------------------
 *
 * Tcl_RegExpExecute --
 *     Execute a regular expression compiled with Boyer-Moore and/or 
 *     regexp.
 *
 * Parameters:
 *     o regExpPtr - Used to hold info on this regular expression.
 *     o matchStrIn - String to match against the regular expression.
 *     o matchStrLower - Optional lower case version of the string.  If
 *       multiple no case matches are being done, time can be saved by
 *       down shifting the string in advance.  NULL if not a no-case 
 *       match or this procedure is to do the down shifting.
 *
 * Results:
 *     TRUE if a match, FALSE if it does not match.
 *
 *-----------------------------------------------------------------------------
 */
int
Tcl_RegExpExecute (interp, regExpPtr, matchStrIn, matchStrLower)
    Tcl_Interp  *interp;
    regexp_pt    regExpPtr;
    char        *matchStrIn;
    char        *matchStrLower;
{
    char *matchStr;
    int   result;

    if (regExpPtr->noCase) {
        if (matchStrLower == NULL) {
            matchStr = ckalloc (strlen (matchStrIn) + 1);
            Tcl_DownShift (matchStr, matchStrIn);
        } else
            matchStr = matchStrLower;
    } else
        matchStr = matchStrIn;

    /*
     * If a Boyer-Moore pattern has been compiled, use that algorithm to test
     * against the text.  If that passes, then test with the regexp if we have
     * it.
     */
    if (regExpPtr->boyerMoorePtr != NULL) {
        char     *startPtr;
        unsigned  matchLen;

        startPtr = BoyerMooreExecute (matchStr, strlen (matchStr), 
                                      regExpPtr->boyerMoorePtr, &matchLen);
        if (startPtr == NULL) {
            result = FALSE;
            goto exitPoint;
        }
        if (regExpPtr->progPtr == NULL) {
            result = TRUE;  /* No regexp, its a match! */
            goto exitPoint;
        }
    }
    
    /*
     * Give it a go with full regular expressions
     */
    result = regexec (regExpPtr->progPtr, matchStr);

    /*
     * Clean up and return status here.
     */
exitPoint:
    if ((regExpPtr->noCase) && (matchStrLower == NULL))
        ckfree (matchStr);
    return result;
}
