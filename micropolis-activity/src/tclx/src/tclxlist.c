/* 
 * tclXlist.c --
 *
 *  Extended Tcl list commands.
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
 * $Id: tclXlist.c,v 2.0 1992/10/16 04:50:57 markd Rel $
 *-----------------------------------------------------------------------------
 */

#include "tclxint.h"


/*
 *-----------------------------------------------------------------------------
 *
 * Tcl_LvarcatCmd --
 *     Implements the TCL lvarpop command:
 *         lvarcat var string string string
 *
 * Results:
 *      Standard TCL results.
 *
 *-----------------------------------------------------------------------------
 */
int
Tcl_LvarcatCmd (clientData, interp, argc, argv)
    ClientData  clientData;
    Tcl_Interp *interp;
    int         argc;
    char      **argv;
{
    int        listArgc, idx, listIdx;
    char     **listArgv;
    char      *staticArgv [12];
    char      *varContents, *newStr, *result;

    if (argc < 3) {
        Tcl_AppendResult (interp, tclXWrongArgs, argv [0], 
                          " var string [string...]", (char *) NULL);
        return TCL_ERROR;
    }

    varContents = Tcl_GetVar (interp, argv[1], 0);

    if (varContents != NULL)
        listArgc = argc - 1;
    else
        listArgc = argc - 2;

    if (listArgc < (sizeof (staticArgv) / sizeof (char *))) {
        listArgv = staticArgv;
    } else {
        listArgv = (char **) ckalloc (listArgc * sizeof (char *));
    }
    
    if (varContents != NULL) {
        listArgv [0] = varContents;
        listIdx = 1;
    } else {
        listIdx = 0;
    }
    for (idx = 2; idx < argc; idx++, listIdx++)
        listArgv [listIdx] = argv [idx];

    newStr = Tcl_Concat (listArgc, listArgv);
    result = Tcl_SetVar (interp, argv [1], newStr, TCL_LEAVE_ERR_MSG);

    ckfree (newStr);
    if (listArgv != staticArgv)
        ckfree ((char *) listArgv);

    /*
     * If all is ok, return the variable contents as a "static" result.
     */
    if (result != NULL) {
        interp->result = result;
        return TCL_OK;
    } else {
        return TCL_ERROR;
    }
}

/*
 *-----------------------------------------------------------------------------
 *
 * Tcl_LvarpopCmd --
 *     Implements the TCL lvarpop command:
 *         lvarpop var [index [string]]
 *
 * Results:
 *      Standard TCL results.
 *
 *-----------------------------------------------------------------------------
 */
int
Tcl_LvarpopCmd (clientData, interp, argc, argv)
    ClientData  clientData;
    Tcl_Interp *interp;
    int         argc;
    char      **argv;
{
    int        listArgc, listIdx, idx;
    char     **listArgv;
    char      *varContents, *resultList, *returnElement;

    if ((argc < 2) || (argc > 4)) {
        Tcl_AppendResult (interp, tclXWrongArgs, argv [0], 
                          " var [index [string]]", (char *) NULL);
        return TCL_ERROR;
    }

    varContents = Tcl_GetVar (interp, argv[1], TCL_LEAVE_ERR_MSG);
    if (varContents == NULL)
        return TCL_ERROR;

    if (Tcl_SplitList (interp, varContents, &listArgc, &listArgv) == TCL_ERROR)
        return TCL_ERROR;

    if (argc == 2) 
        listIdx = 0;
    else {
        if (Tcl_GetInt (interp, argv[2], &listIdx) != TCL_OK)
            goto errorExit;
    }

    /*
     * Just ignore out-of bounds requests, like standard Tcl.
     */
    if ((listIdx < 0) || (listIdx >= listArgc)) {
        goto okExit;
    }
    returnElement = listArgv [listIdx];

    if (argc == 4)
        listArgv [listIdx] = argv [3];
    else {
        listArgc--;
        for (idx = listIdx; idx < listArgc; idx++)
            listArgv [idx] = listArgv [idx+1];
    }

    resultList = Tcl_Merge (listArgc, listArgv);
    if (Tcl_SetVar (interp, argv [1], resultList, TCL_LEAVE_ERR_MSG) == NULL) {
        ckfree (resultList);
        goto errorExit;
    }
    ckfree (resultList);

    Tcl_SetResult (interp, returnElement, TCL_VOLATILE);
  okExit:
    ckfree((char *) listArgv);
    return TCL_OK;

  errorExit:
    ckfree((char *) listArgv);
    return TCL_ERROR;;
}

/*
 *-----------------------------------------------------------------------------
 *
 * Tcl_LvarpushCmd --
 *     Implements the TCL lvarpush command:
 *         lvarpush var string [index]
 *
 * Results:
 *      Standard TCL results.
 *
 *-----------------------------------------------------------------------------
 */
int
Tcl_LvarpushCmd (clientData, interp, argc, argv)
    ClientData  clientData;
    Tcl_Interp *interp;
    int         argc;
    char      **argv;
{
    int        listArgc, listIdx, idx;
    char     **listArgv;
    char      *varContents, *resultList;

    if ((argc < 3) || (argc > 4)) {
        Tcl_AppendResult (interp, tclXWrongArgs, argv [0], 
                          " var string [index]", (char *) NULL);
        return TCL_ERROR;
    }

    varContents = Tcl_GetVar (interp, argv[1], TCL_LEAVE_ERR_MSG);
    if (varContents == NULL)
        varContents = "";

    if (Tcl_SplitList (interp, varContents, &listArgc, &listArgv) == TCL_ERROR)
        return TCL_ERROR;

    if (argc == 3) 
        listIdx = 0;
    else {
        if (Tcl_GetInt (interp, argv[3], &listIdx) != TCL_OK)
            goto errorExit;
    }

    /*
     * Out-of-bounds request go to the start or end, as with most of Tcl.
     */
    if (listIdx < 0)
        listIdx = 0;
    else
        if (listIdx > listArgc)
            listIdx = listArgc;

    /*
     * This code takes advantage of the fact that a NULL entry is always
     * returned by Tcl_SplitList, but not required by Tcl_Merge.
     */
    for (idx = listArgc; idx > listIdx; idx--)
        listArgv [idx] = listArgv [idx - 1];

    listArgv [listIdx] = argv [2];

    resultList = Tcl_Merge (listArgc + 1, listArgv);

    if (Tcl_SetVar (interp, argv [1], resultList, TCL_LEAVE_ERR_MSG) == NULL) {
        ckfree (resultList);
        goto errorExit;
    }

    ckfree (resultList);
    ckfree((char *) listArgv);
    return TCL_OK;

  errorExit:
    ckfree((char *) listArgv);
    return TCL_ERROR;;
}

/*
 *-----------------------------------------------------------------------------
 *
 * Tcl_LemptyCmd --
 *     Implements the strcat TCL command:
 *         lempty list
 *
 * Results:
 *     Standard TCL result.
 *
 *-----------------------------------------------------------------------------
 */
int
Tcl_LemptyCmd (clientData, interp, argc, argv)
    ClientData   clientData;
    Tcl_Interp  *interp;
    int          argc;
    char       **argv;
{
    char *scanPtr;

    if (argc != 2) {
        Tcl_AppendResult (interp, tclXWrongArgs, argv [0], " list",
                          (char *) NULL);
        return TCL_ERROR;
    }

    scanPtr = argv [1];
    while ((*scanPtr != '\0') && (isspace (*scanPtr)))
        scanPtr++;
    sprintf (interp->result, "%d", (*scanPtr == '\0'));
    return TCL_OK;

} /* Tcl_LemptyCmd */
