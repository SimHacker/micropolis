/* 
 * tclXcnvclock.c --
 *
 *      Contains the TCL convertclock command.  This is in a module seperate
 * from clock so that it can be excluded, along with the yacc generated code,
 * since its rather large.
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
 * $Id: tclXcnvclock.c,v 2.1 1992/11/07 22:23:03 markd Exp $
 *-----------------------------------------------------------------------------
 */

#include "tclxint.h"
#include <time.h>


/*
 *-----------------------------------------------------------------------------
 *
 * Tcl_ConvertclockCmd --
 *     Implements the TCL convertclock command:
 *         convertclock dateString [GMT|{}]
 *
 * Results:
 *     Standard TCL results.
 *
 *-----------------------------------------------------------------------------
 */
int
Tcl_ConvertclockCmd (clientData, interp, argc, argv)
    ClientData  clientData;
    Tcl_Interp *interp;
    int         argc;
    char      **argv;
{
    long        clockVal;
    time_t      baseClock;
    struct tm  *timeDataPtr;
    long        zone;

    if ((argc < 2) || (argc > 4)) {
        Tcl_AppendResult (interp, tclXWrongArgs, argv [0], 
                          " dateString [GMT|{}] [baseclock]", (char *) NULL);
	return TCL_ERROR;
    }
    if (argc == 4) {
        if (Tcl_GetLong (interp, argv [3], &baseClock) != TCL_OK)
            return TCL_ERROR;
    } else
        time (&baseClock);

    if ((argc > 2) && (argv [2][0] != '\0')) {
        if (!STREQU (argv [2], "GMT")) {
            Tcl_AppendResult (interp, "invalid argument: expected `GMT', ",
                              "got : `", argv [2], "'", (char *) NULL);
            return TCL_ERROR;
        }
        zone = 0; /* Zero minutes from GMT */
    } else {
        timeDataPtr = localtime (&baseClock);
        /*
         * Get the minutes east of GMT.
         */
#ifdef TCL_TM_GMTOFF
        zone = -(timeDataPtr->tm_gmtoff / 60);
#endif
#ifdef TCL_TIMEZONE_VAR 
        zone = timezone / 60;
#endif
#if  !defined(TCL_TM_GMTOFF) && !defined(TCL_TIMEZONE_VAR)
        zone = timeDataPtr->tm_tzadj  / 60;
#endif
    }

    clockVal = Tcl_GetDate (argv [1], baseClock, zone);
    if (clockVal == -1) {
        Tcl_AppendResult (interp, "Unable to convert date-time string \"",
                          argv [1], "\"", (char *) NULL);
	return TCL_ERROR;
    }
    sprintf (interp->result, "%ld", clockVal);
    return TCL_OK;
}

