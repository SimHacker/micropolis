/*
 * tclXmath.c --
 *
 * Mathematical Tcl commands.
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
 * $Id: tclXmath.c,v 2.0 1992/10/16 04:50:59 markd Rel $
 *-----------------------------------------------------------------------------
 */

#include "tclxint.h"

extern int rand();

/*
 * Prototypes of internal functions.
 */
int 
really_random _ANSI_ARGS_((int my_range));


/*
 *-----------------------------------------------------------------------------
 *
 * Tcl_MaxCmd --
 *      Implements the TCL max command:
 *        max num1 num2 [..numN]
 *
 * Results:
 *      Standard TCL results.
 *
 *-----------------------------------------------------------------------------
 */
int
Tcl_MaxCmd (clientData, interp, argc, argv)
    ClientData  clientData;
    Tcl_Interp *interp;
    int         argc;
    char      **argv;
{
    double value, maxValue = -MAXDOUBLE;
    int    idx,   maxIdx   =  1;


    if (argc < 3) {
        Tcl_AppendResult (interp, tclXWrongArgs, argv [0], 
                          " num1 num2 [..numN]", (char *) NULL);
        return TCL_ERROR;
    }

    for (idx = 1; idx < argc; idx++) {
        if (Tcl_GetDouble (interp, argv [idx], &value) != TCL_OK)
            return TCL_ERROR;
        if (value > maxValue) {
            maxValue = value;
            maxIdx = idx;
        }
    }
    strcpy (interp->result, argv [maxIdx]);
    return TCL_OK;
}

/*
 *-----------------------------------------------------------------------------
 *
 * Tcl_MinCmd --
 *     Implements the TCL min command:
 *         min num1 num2 [..numN]
 *
 * Results:
 *      Standard TCL results.
 *
 *-----------------------------------------------------------------------------
 */
int
Tcl_MinCmd (clientData, interp, argc, argv)
    ClientData  clientData;
    Tcl_Interp *interp;
    int     argc;
    char      **argv;
{
    double value, minValue = MAXDOUBLE;
    int    idx,   minIdx   = 1;

    if (argc < 3) {
        Tcl_AppendResult (interp, tclXWrongArgs, argv [0], 
                          " num1 num2 [..numN]", (char *) NULL);
        return TCL_ERROR;
    }

    for (idx = 1; idx < argc; idx++) {
        if (Tcl_GetDouble (interp, argv [idx], &value) != TCL_OK)
            return TCL_ERROR;
        if (value < minValue) {
            minValue = value;
            minIdx = idx;
            }
        }
    strcpy (interp->result, argv [minIdx]);
    return TCL_OK;
}

/*
 *-----------------------------------------------------------------------------
 *
 * ReallyRandom --
 *     Insure a good random return for a range, unlike an arbitrary
 *     random() % n, thanks to Ken Arnold, Unix Review, October 1987.
 *
 *-----------------------------------------------------------------------------
 */
#ifdef TCL_32_BIT_RANDOM
#    define RANDOM_RANGE 0x7FFFFFFF
#else
#    define RANDOM_RANGE 0x7FFF
#endif

static int 

ReallyRandom (myRange)
    int myRange;
{
    int maxMultiple, rnum;

    maxMultiple = 
        (int)(
	    RANDOM_RANGE / 
	    myRange);
    maxMultiple *= 
        myRange;

    while ((rnum = rand()) >= maxMultiple) {
        continue;
    }

    return (rnum % myRange);
}

/*
 *-----------------------------------------------------------------------------
 *
 * Tcl_RandomCmd  --
 *     Implements the TCL random command:
 *     random limit
 *
 * Results:
 *  Standard TCL results.
 *
 *-----------------------------------------------------------------------------
 */
int
Tcl_RandomCmd (clientData, interp, argc, argv)
    ClientData  clientData;
    Tcl_Interp *interp;
    int         argc;
    char      **argv;
{
    unsigned range;

    if ((argc < 2) || (argc > 3))
        goto invalidArgs;

    if (STREQU (argv [1], "seed")) {
        long seed;

        if (argc == 3) {
            if (Tcl_GetLong (interp, argv[2], &seed) != TCL_OK)
                return TCL_ERROR;
        } else
            seed = (unsigned) (getpid() + time((time_t *)NULL));

        srand(seed);

    } else {
        if (argc != 2)
            goto invalidArgs;
        if (Tcl_GetUnsigned (interp, argv[1], &range) != TCL_OK)
            return TCL_ERROR;
        if ((range == 0) || (range > (int)RANDOM_RANGE))
            goto outOfRange;

        sprintf (interp->result, "%d", ReallyRandom (range));
    }
    return TCL_OK;

invalidArgs:
    Tcl_AppendResult (interp, tclXWrongArgs, argv [0], 
                      " limit | seed [seedval]", (char *) NULL);
    return TCL_ERROR;
outOfRange:
    {
        char buf [18];

        sprintf (buf, "%d", (int)RANDOM_RANGE);
        Tcl_AppendResult (interp, "range must be > 0 and <= ",
                          buf, (char *) NULL);
        return TCL_ERROR;
    }
}
