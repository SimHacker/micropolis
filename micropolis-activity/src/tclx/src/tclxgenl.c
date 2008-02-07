/* 
 * tclXgeneral.c --
 *
 *      Contains general extensions to the basic TCL command set.
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
 * $Id: tclXgeneral.c,v 2.0 1992/10/16 04:50:47 markd Rel $
 *-----------------------------------------------------------------------------
 */

#include "tclxint.h"

/*
 * These globals must be set by main for the information to be defined.
 */

char *tclxVersion       = "?";   /* Extended Tcl version number.            */
int   tclxPatchlevel    = 0;     /* Extended Tcl patch level.               */

char *tclAppName        = NULL;  /* Application name                        */
char *tclAppLongname    = NULL;  /* Long, natural language application name */
char *tclAppVersion     = NULL;  /* Version number of the application       */


/*
 *-----------------------------------------------------------------------------
 *
 * Tcl_EchoCmd --
 *    Implements the TCL echo command:
 *        echo str1 [str2..]
 *
 * Results:
 *      Always returns TCL_OK.
 *
 *-----------------------------------------------------------------------------
 */
int
Tcl_EchoCmd(clientData, interp, argc, argv)
    ClientData  clientData;
    Tcl_Interp *interp;
    int         argc;
    char      **argv;
{
    int idx;

    for (idx = 1; idx < argc; idx++) {
        fputs (argv [idx], stdout);
        if (idx < (argc - 1))
            printf(" ");
    }
    printf("\n");
    return TCL_OK;
}

/*
 *-----------------------------------------------------------------------------
 *
 * Tcl_InfoxCmd --
 *    Implements the TCL infox command:
 *        infox option
 *
 *-----------------------------------------------------------------------------
 */
int
Tcl_InfoxCmd (clientData, interp, argc, argv)
    ClientData  clientData;
    Tcl_Interp *interp;
    int         argc;
    char      **argv;
{
    if (argc != 2) {
        Tcl_AppendResult (interp, tclXWrongArgs, argv [0], 
                          " option", (char *) NULL);
        return TCL_ERROR;
    }

    if (STREQU ("version", argv [1])) {
        Tcl_SetResult (interp, tclxVersion, TCL_STATIC);
    } else if (STREQU ("patchlevel", argv [1])) {
        char numBuf [32];
        sprintf (numBuf, "%d", tclxPatchlevel);
        Tcl_SetResult (interp, numBuf, TCL_VOLATILE);
    } else if (STREQU ("appname", argv [1])) {
        if (tclAppName != NULL)
            Tcl_SetResult (interp, tclAppName, TCL_STATIC);
    } else if (STREQU ("applongname", argv [1])) {
        if (tclAppLongname != NULL)
            Tcl_SetResult (interp, tclAppLongname, TCL_STATIC);
    } else if (STREQU ("appversion", argv [1])) {
        if (tclAppVersion != NULL)
            Tcl_SetResult (interp, tclAppVersion, TCL_STATIC);
    } else {
        Tcl_AppendResult (interp, "illegal option \"", argv [1], 
                          "\" expect one of: version, patchlevel, appname, ",
                          "applongname, or appversion", (char *) NULL);
        return TCL_ERROR;
    }
    return TCL_OK;
}

/*
 *-----------------------------------------------------------------------------
 *
 * Tcl_LoopCmd --
 *     Implements the TCL loop command:
 *         loop var start end [increment] command
 *
 * Results:
 *      Standard TCL results.
 *
 *-----------------------------------------------------------------------------
 */
int
Tcl_LoopCmd (dummy, interp, argc, argv)
    ClientData  dummy;
    Tcl_Interp *interp;
    int         argc;
    char      **argv;
{
    int   result = TCL_OK;
    long  i, first, limit, incr = 1;
    char *command;
    char  itxt [12];

    if ((argc < 5) || (argc > 6)) {
        Tcl_AppendResult (interp, tclXWrongArgs, argv [0], 
                          " var first limit [incr] command", (char *) NULL);
        return TCL_ERROR;
    }

    if (Tcl_GetLong (interp, argv[2], &first) != TCL_OK)
        return TCL_ERROR;
    if (Tcl_GetLong (interp, argv[3], &limit) != TCL_OK)
        return TCL_ERROR;
    if (argc == 5)
        command = argv[4];
    else {
        if (Tcl_GetLong (interp, argv[4], &incr) != TCL_OK)
            return TCL_ERROR;
        command = argv[5];
    }

    for (i = first;
             (((i < limit) && (incr > 0)) || ((i > limit) && (incr < 0)));
             i += incr) {

        sprintf (itxt,"%ld",i);
        if (Tcl_SetVar (interp, argv [1], itxt, TCL_LEAVE_ERR_MSG) == NULL)
            return TCL_ERROR;

        result = Tcl_Eval(interp, command, 0, (char **) NULL);
        if (result != TCL_OK) {
            if (result == TCL_CONTINUE) {
                result = TCL_OK;
            } else if (result == TCL_BREAK) {
                result = TCL_OK;
                break;
            } else if (result == TCL_ERROR) {
                char buf [64];

                sprintf (buf, "\n    (\"loop\" body line %d)", 
                         interp->errorLine);
                Tcl_AddErrorInfo (interp, buf);
                break;
            } else {
                break;
            }
        }
    }
    /*
     * Set variable to its final value.
     */
    sprintf (itxt,"%ld",i);
    if (Tcl_SetVar (interp, argv [1], itxt, TCL_LEAVE_ERR_MSG) == NULL)
        return TCL_ERROR;

    return result;
}
