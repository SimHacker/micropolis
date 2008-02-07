/* 
 * tclXclock.c --
 *
 *      Contains the TCL time and date related commands.
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
 * $Id: tclXclock.c,v 2.0 1992/10/16 04:50:28 markd Rel $
 *-----------------------------------------------------------------------------
 */

#include <time.h>
#include "tclxint.h"


/*
 *-----------------------------------------------------------------------------
 *
 * Tcl_GetclockCmd --
 *     Implements the TCL getclock command:
 *         getclock
 *
 * Results:
 *     Standard TCL results.
 *
 *-----------------------------------------------------------------------------
 */
int
Tcl_GetclockCmd (clientData, interp, argc, argv)
    ClientData  clientData;
    Tcl_Interp *interp;
    int         argc;
    char      **argv;
{
    if (argc != 1) {
        Tcl_AppendResult (interp, tclXWrongArgs, argv[0], (char *) NULL);
        return TCL_ERROR;
    }
    sprintf (interp->result, "%ld", time ((long *) NULL));
    return TCL_OK;
}

/*
 *-----------------------------------------------------------------------------
 *
 * Tcl_FmtclockCmd --
 *     Implements the TCL fmtclock command:
 *         fmtclock clockval [format] [GMT|{}]
 *
 * Results:
 *     Standard TCL results.
 *
 *-----------------------------------------------------------------------------
 */
int
Tcl_FmtclockCmd (clientData, interp, argc, argv)
    ClientData  clientData;
    Tcl_Interp *interp;
    int         argc;
    char      **argv;
{
    int              useGMT = FALSE;
    long             clockVal;
    char            *format;
    struct tm       *timeDataPtr;
    int              fmtError;

    if ((argc < 2) || (argc > 4)) {
        Tcl_AppendResult (interp, tclXWrongArgs, argv [0], 
                          " clockval [format] [GMT|{}]", (char *) NULL);
        return TCL_ERROR;
    }

    if (Tcl_GetLong (interp, argv[1], &clockVal) != TCL_OK)
        return TCL_ERROR;
    if ((argc == 4) && (argv [3][0] != '\0')) {
        if (!STREQU (argv [3], "GMT")) {
            Tcl_AppendResult (interp, "expected \"GMT\" or {} got \"",
                              argv [3], "\"", (char *) NULL);
            return TCL_ERROR;
        }
        useGMT = TRUE;
    }

    if ((argc >= 3) && (argv [2][0] != '\0'))
        format = argv[2];
    else
        format = "%a %b %d %X %Z %Y";

    if (useGMT)
        timeDataPtr = gmtime (&clockVal);
    else    
        timeDataPtr = localtime (&clockVal);

    fmtError = strftime (interp->result, TCL_RESULT_SIZE, format, 
                         timeDataPtr) < 0;
    if (fmtError) {
        Tcl_AppendResult (interp, "error formating time", (char *) NULL);
        return TCL_ERROR;
    }
    return TCL_OK;
}
