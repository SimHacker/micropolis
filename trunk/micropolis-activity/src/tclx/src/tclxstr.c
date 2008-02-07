/* 
 * tclXstring.c --
 *
 *      Extended TCL string and character manipulation commands.
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
 * $Id: tclXstring.c,v 2.0 1992/10/16 04:51:16 markd Rel $
 *-----------------------------------------------------------------------------
 */

#include "tclxint.h"

/*
 * Prototypes of internal functions.
 */
static unsigned int
ExpandString _ANSI_ARGS_((unsigned char *s,
                          unsigned char  buf[]));


/*
 *-----------------------------------------------------------------------------
 *
 * Tcl_CindexCmd --
 *     Implements the cindex TCL command:
 *         cindex string indexExpr
 *
 * Results:
 *      Returns the character indexed by  index  (zero  based)  from
 *      string. 
 *
 *-----------------------------------------------------------------------------
 */
int
Tcl_CindexCmd (clientData, interp, argc, argv)
    ClientData   clientData;
    Tcl_Interp  *interp;
    int          argc;
    char       **argv;
{
    long index;

    if (argc != 3) {
        Tcl_AppendResult (interp, tclXWrongArgs, argv [0],
                          " string indexExpr", (char *) NULL);
        return TCL_ERROR;
    }

    if (Tcl_ExprLong (interp, argv[2], &index) != TCL_OK)
        return TCL_ERROR;
    if (index >= strlen (argv [1]))
        return TCL_OK;

    interp->result [0] = argv[1][index];
    interp->result [1] = 0;
    return TCL_OK;

} /* Tcl_CindexCmd */

/*
 *-----------------------------------------------------------------------------
 *
 * Tcl_ClengthCmd --
 *     Implements the clength TCL command:
 *         clength string
 *
 * Results:
 *      Returns the length of string in characters. 
 *
 *-----------------------------------------------------------------------------
 */
int
Tcl_ClengthCmd (clientData, interp, argc, argv)
    ClientData   clientData;
    Tcl_Interp  *interp;
    int          argc;
    char       **argv;
{

    if (argc != 2) {
        Tcl_AppendResult (interp, tclXWrongArgs, argv [0], " string", 
                          (char *) NULL);
        return TCL_ERROR;
    }

    sprintf (interp->result, "%d", strlen (argv[1]));
    return TCL_OK;

} /* Tcl_ClengthCmd */

/*
 *-----------------------------------------------------------------------------
 *
 * Tcl_CrangeCmd --
 *     Implements the crange and csubstr TCL commands:
 *         crange string firstExpr lastExpr
 *         csubstr string firstExpr lengthExpr
 *
 * Results:
 *      Standard Tcl result.
 *-----------------------------------------------------------------------------
 */
int
Tcl_CrangeCmd (clientData, interp, argc, argv)
    ClientData   clientData;
    Tcl_Interp  *interp;
    int          argc;
    char       **argv;
{
    long      fullLen, first;
    long      subLen;
    char     *strPtr;
    char      holdChar;
    int       isRange = (argv [0][1] == 'r');  /* csubstr or crange */

    if (argc != 4) {
        Tcl_AppendResult (interp, tclXWrongArgs, argv [0], 
                          " string firstExpr ", 
                          (isRange) ? "lastExpr" : "lengthExpr",
                          (char *) NULL);
        return TCL_ERROR;
    }

    if (Tcl_ExprLong (interp, argv[2], &first) != TCL_OK)
        return TCL_ERROR;

    fullLen = strlen (argv [1]);
    if (first >= fullLen)
        return TCL_OK;

    if (STREQU (argv[3], "end"))
        subLen = fullLen - first;
    else {
        if (Tcl_ExprLong (interp, argv[3], &subLen) != TCL_OK)
            return TCL_ERROR;
        
        if (isRange) {
            if (subLen < first) {
                Tcl_AppendResult (interp, "last is before first",
                                  (char *) NULL);
                return TCL_ERROR;
            }
            subLen = subLen - first +1;
        }

        if (first + subLen > fullLen)
            subLen = fullLen - first;
    }

    strPtr = argv [1] + first;

    holdChar = strPtr [subLen];
    strPtr [subLen] = '\0';
    Tcl_SetResult (interp, strPtr, TCL_VOLATILE);
    strPtr [subLen] = holdChar;

    return TCL_OK;

} /* Tcl_CrangeCmd */

/*
 *-----------------------------------------------------------------------------
 *
 * Tcl_ReplicateCmd --
 *     Implements the replicate TCL command:
 *         replicate string countExpr
 *     See the string(TCL) manual page.
 *
 * Results:
 *      Returns string replicated count times.
 *
 *-----------------------------------------------------------------------------
 */
int
Tcl_ReplicateCmd (clientData, interp, argc, argv)
    ClientData   clientData;
    Tcl_Interp  *interp;
    int          argc;
    char       **argv;
{
    long           repCount;
    register char *srcPtr, *scanPtr, *newPtr;
    register long  newLen, cnt;

    if (argc != 3) {
        Tcl_AppendResult (interp, tclXWrongArgs, argv [0], 
                          " string countExpr", (char *) NULL);
        return TCL_ERROR;
    }

    if (Tcl_ExprLong (interp, argv[2], &repCount) != TCL_OK)
        return TCL_ERROR;

    srcPtr = argv [1];
    newLen = strlen (srcPtr) * repCount;
    if (newLen >= TCL_RESULT_SIZE)
        Tcl_SetResult (interp, ckalloc ((unsigned) newLen + 1), TCL_DYNAMIC);

    newPtr = interp->result;
    for (cnt = 0; cnt < repCount; cnt++) {
        for (scanPtr = srcPtr; *scanPtr != 0; scanPtr++)
            *newPtr++ = *scanPtr;
    }
    *newPtr = 0;

    return TCL_OK;

} /* Tcl_ReplicateCmd */

/*
 *-----------------------------------------------------------------------------
 *
 * ExpandString --
 *  Build an expand version of a translit range specification.
 *
 * Results:
 *  TRUE it the expansion is ok, FALSE it its too long.
 *
 *-----------------------------------------------------------------------------
 */
#define MAX_EXPANSION 255

static unsigned int
ExpandString (s, buf)
    unsigned char *s;
    unsigned char  buf[];
{
    int i, j;

    i = 0;
    while((*s !=0) && i < MAX_EXPANSION) {
        if(s[1] == '-' && s[2] > s[0]) {
            for(j = s[0]; j <= s[2]; j++)
                buf[i++] = j;
            s += 3;
        } else
            buf[i++] = *s++;
    }
    buf[i] = 0;
    return (i < MAX_EXPANSION);
}

/*
 *-----------------------------------------------------------------------------
 *
 * Tcl_TranslitCmd --
 *     Implements the TCL translit command:
 *     translit inrange outrange string
 *
 * Results:
 *  Standard TCL results.
 *
 *-----------------------------------------------------------------------------
 */
int
Tcl_TranslitCmd (clientData, interp, argc, argv)
    ClientData   clientData;
    Tcl_Interp  *interp;
    int          argc;
    char       **argv;
{
    unsigned char from [MAX_EXPANSION+1];
    unsigned char to   [MAX_EXPANSION+1];
    unsigned char map  [MAX_EXPANSION+1];
    unsigned char *s, *t;
    int idx;

    if (argc != 4) {
        Tcl_AppendResult (interp, tclXWrongArgs, argv [0], 
                          " from to string", (char *) NULL);
        return TCL_ERROR;
    }

    if (!ExpandString ((unsigned char *) argv[1], from)) {
        interp->result = "inrange expansion too long";
        return TCL_ERROR;
    }

    if (!ExpandString ((unsigned char *) argv[2], to)) {
        interp->result = "outrange expansion too long";
        return TCL_ERROR;
    }

    for (idx = 0; idx <= MAX_EXPANSION; idx++)
        map [idx] = idx;

    for (idx = 0; to [idx] != '\0'; idx++) {
        if (from [idx] != '\0')
            map [from [idx]] = to [idx];
        else
            break;
    }
    if (to [idx] != '\0') {
        interp->result = "inrange longer than outrange";
        return TCL_ERROR;
    }

    for (; from [idx] != '\0'; idx++)
        map [from [idx]] = 0;

    for (s = t = (unsigned char *) argv[3]; *s != '\0'; s++) {
        if (map[*s] != '\0')
            *t++ = map [*s];
    }
    *t = '\0';

    Tcl_SetResult (interp, argv[3], TCL_VOLATILE);

    return TCL_OK;
}

/*
 *-----------------------------------------------------------------------------
 *
 * Tcl_CtypeCmd --
 *
 *      This function implements the 'ctype' command:
 *      ctype class string
 *
 *      Where class is one of the following:
 *        digit, xdigit, lower, upper, alpha, alnum,
 *        space, cntrl,  punct, print, graph, ascii, char or ord.
 *
 * Results:
 *       One or zero: Depending if all the characters in the string are of
 *       the desired class.  Char and ord provide conversions and return the
 *       converted value.
 *
 *-----------------------------------------------------------------------------
 */
int
Tcl_CtypeCmd (clientData, interp, argc, argv)
    ClientData   clientData;
    Tcl_Interp  *interp;
    int          argc;
    char       **argv;
{
    register char *class;
    register char *scanPtr = argv [2];

    if (argc != 3) {
        Tcl_AppendResult (interp, tclXWrongArgs, argv [0], " class string",
                          (char *) NULL);
        return TCL_ERROR;
    }

    class = argv [1];

    /*
     * Handle conversion requests.
     */
    if (STREQU (class, "char")) {
        int number;

        if (Tcl_GetInt (interp, argv [2], &number) != TCL_OK)
            return TCL_ERROR;
        if ((number < 0) || (number > 255)) {
            Tcl_AppendResult (interp, "number must be in the range 0..255",
                              (char *) NULL);
            return TCL_ERROR;
        }

        interp->result [0] = number;
        interp->result [1] = 0;
        return TCL_OK;
    }

    if (STREQU (class, "ord")) {
        if (strlen (argv [2]) != 1) {
            Tcl_AppendResult (interp, "string to convert must be only one",
                              " character", (char *) NULL);
            return TCL_ERROR;
        }

        sprintf(interp->result, "%d", (int)(*argv[2]));
        return TCL_OK;
    }

    /*
     * Select based on the first letter of the 'class' argument to chose the 
     * macro to test characters with.  In some cases another character must be
     * switched on to determine which macro to use.  This is gross, but better
     * we only have to do a string compare once to test if class is correct.
     */
    if ((class [2] == 'n') && STREQU (class, "alnum")) {
        for (; *scanPtr != 0; scanPtr++) {
            if (!isalnum (*scanPtr))
                break;
        }
        goto returnResult;
    }
    if ((class [2] == 'p') && STREQU (class, "alpha")) {
        for (; *scanPtr != 0; scanPtr++) {
            if (! isalpha (*scanPtr))
                break;
        }
        goto returnResult;
    }
    if ((class [1] == 's') && STREQU (class, "ascii")) {
        for (; *scanPtr != 0; scanPtr++) {
            if (!isascii (*scanPtr))
                break;
        }
        goto returnResult;
    }
    if (STREQU (class, "cntrl")) {
        for (; *scanPtr != 0; scanPtr++) {
            if (!iscntrl (*scanPtr))
                break;
        }
        goto returnResult;
    }
    if (STREQU (class, "digit")) {
        for (; *scanPtr != 0; scanPtr++) {
            if (!isdigit (*scanPtr))
                break;
        }
        goto returnResult;
    }
    if (STREQU (class, "graph")) {
        for (; *scanPtr != 0; scanPtr++) {
            if (!isgraph (*scanPtr))
                break;
        }
        goto returnResult;
    }
    if (STREQU (class, "lower")) {
        for (; *scanPtr != 0; scanPtr++) {
            if (!islower (*scanPtr))
                break;
        }
        goto returnResult;
    }
    if ((class [1] == 'r') && STREQU (class, "print")) {
        for (; *scanPtr != 0; scanPtr++) {
            if (!isprint (*scanPtr))
                break;
        }
        goto returnResult;
    }
    if ((class [1] == 'u') && STREQU (class, "punct")) {
        for (; *scanPtr != 0; scanPtr++) {
            if (!ispunct (*scanPtr))
                break;
        }
        goto returnResult;
    }
    if (STREQU (class, "space")) {
        for (; *scanPtr != 0; scanPtr++) {
            if (!isspace (*scanPtr))
                break;
        }
        goto returnResult;
    }
    if (STREQU (class, "upper")) {
        for (; *scanPtr != 0; scanPtr++) {
            if (!isupper (*scanPtr))
                break;
        }
        goto returnResult;
    }
    if (STREQU (class, "xdigit")) {
        for (; *scanPtr != 0; scanPtr++) {
            if (!isxdigit (*scanPtr))
                break;
        }
        goto returnResult;
    }
    /*
     * No match on subcommand.
     */
    Tcl_AppendResult (interp, "unrecognized class specification: \"", class,
                      "\", expected one of: alnum, alpha, ascii, char, ",
                      "cntrl, digit, graph, lower, ord, print, punct, space, ",
                      "upper or xdigit", (char *) NULL);
    return TCL_ERROR;

    /*
     * Return true or false, depending if the end was reached.  Always return 
     * false for a null string.
     */
returnResult:
    if ((*scanPtr == 0) && (scanPtr != argv [2]))
        interp->result = "1";
    else
        interp->result = "0";
    return TCL_OK;

}

