/* 
 * tclXchmod.c --
 *
 *    Chmod, chown and chgrp Tcl commands.
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
 * $Id: tclXchmod.c,v 2.0 1992/10/16 04:50:26 markd Rel $
 *-----------------------------------------------------------------------------
 */

#include "tclxint.h"

/*
 * Prototypes of internal functions.
 */
static int
ConvSymMode _ANSI_ARGS_((Tcl_Interp  *interp,
                         char        *symMode,
                         int          modeVal));


/*
 *-----------------------------------------------------------------------------
 *
 * ConvSymMode --
 *      Parse and convert symbolic file permissions as specified by chmod(C).
 *
 * Parameters:
 *   o interp - Pointer to the current interpreter, error messages will be
 *     returned in the result.
 *   o symMode - The symbolic permissions to parse.
 *   o modeVal - The existing permissions value on a file.
 *
 * Results:
 *      The new permissions, or -1 if invalid permissions where supplied.
 *
 *-----------------------------------------------------------------------------
 */
static int
ConvSymMode (interp, symMode, modeVal)
    Tcl_Interp  *interp;
    char        *symMode;
    int          modeVal;

{
    int  user, group, other;
    char operator, *scanPtr;
    int  rwxMask, ugoMask, setUID, sticky, locking;
    int  newMode;

    scanPtr = symMode;

    while (*scanPtr != '\0') {
        user = group = other = FALSE;

        /* 
         * Scan who field.
         */
        while (! ((*scanPtr == '+') || 
                  (*scanPtr == '-') || 
                  (*scanPtr == '='))) {
            switch (*scanPtr) {
                case 'a':
                    user = group = other = TRUE;
                    break;
                case 'u':
                    user = TRUE;
                    break;
                case 'g':
                    group = TRUE;
                    break;
                case 'o':
                    other = TRUE;
                    break;
                default:
                    goto invalidMode;
            }
            scanPtr++;
        }

        /*
         * If none where specified, that means all.
         */

        if (! (user || group || other))
            user = group = other = TRUE;

        operator = *scanPtr++;

        /* 
         * Decode the permissions
         */

        rwxMask = 0;
        setUID = sticky = locking = FALSE;

        /* 
         * Scan permissions field
         */
        while (! ((*scanPtr == ',') || (*scanPtr == 0))) {
            switch (*scanPtr) {
                case 'r':
                    rwxMask |= 4;
                    break;
                case 'w':
                    rwxMask |= 2;
                    break;
                case 'x':
                    rwxMask |= 1;
                    break;
                case 's':
                    setUID = TRUE;
                    break;
                case 't':
                    sticky = TRUE;
                    break;
                case 'l':
                    locking = TRUE;
                    break;
                default:
                    goto invalidMode;
            }
            scanPtr++;
        }

        /*
         * Build mode map of specified values.
         */

        newMode = 0;
        ugoMask = 0;
        if (user) {
            newMode |= rwxMask << 6;
            ugoMask |= 0700;
        }
        if (group) {
            newMode |= rwxMask << 3;
            ugoMask |= 0070;
        }
        if (other) {
            newMode |= rwxMask;
            ugoMask |= 0007;
        }
        if (setUID && user)
            newMode |= 04000;
        if ((setUID || locking) && group)
            newMode |= 02000;
        if (sticky)
            newMode |= 01000;

        /* 
         * Add to cumulative mode based on operator.
         */

        if (operator == '+')
            modeVal |= newMode;
        else if (operator == '-')
            modeVal &= ~newMode;
        else if (operator == '=')
            modeVal |= (modeVal & ugoMask) | newMode;
        if (*scanPtr == ',')
            scanPtr++;
    }

    return modeVal;

  invalidMode:
    Tcl_AppendResult (interp, "invalid file mode \"", symMode, "\"",
                      (char *) NULL);
    return -1;
}

/*
 *-----------------------------------------------------------------------------
 *
 * Tcl_ChmodCmd --
 *     Implements the TCL chmod command:
 *     chmod mode filelist
 *
 * Results:
 *  Standard TCL results, may return the UNIX system error message.
 *
 *-----------------------------------------------------------------------------
 */
int
Tcl_ChmodCmd (clientData, interp, argc, argv)
    ClientData   clientData;
    Tcl_Interp  *interp;
    int          argc;
    char       **argv;
{
    int           idx, modeVal, fileArgc, absMode;
    char        **fileArgv;
    struct stat   fileStat;

    if (argc != 3) {
        Tcl_AppendResult (interp, tclXWrongArgs, argv [0], 
                          " mode filelist", (char *) NULL);
        return TCL_ERROR;
    }

    if (isdigit (argv [1][0])) {
        if (Tcl_GetInt (interp, argv [1], &modeVal) != TCL_OK)
            return TCL_ERROR;
        absMode = TRUE;
    } else
        absMode = FALSE;

    if (Tcl_SplitList (interp, argv [2], &fileArgc, &fileArgv) != TCL_OK)
        return TCL_ERROR;

    for (idx = 0; idx < fileArgc; idx++) {
        if (!absMode) {
            if (stat (fileArgv [idx], &fileStat) != 0)
                goto fileError;
            modeVal = ConvSymMode (interp, argv [1], fileStat.st_mode & 07777);
            if (modeVal < 0)
                goto errorExit;
        }
        if (chmod (fileArgv [idx], (unsigned short) modeVal) < 0)
            goto fileError;
    }

  exitPoint:
    ckfree ((char *) fileArgv);
    return TCL_OK;

  fileError:
    /*
     * Error accessing file, assumes file name is fileArgv [idx].
     */
    Tcl_AppendResult (interp, fileArgv [idx], ": ", Tcl_UnixError (interp),
                      (char *) NULL);

  errorExit:
    ckfree ((char *) fileArgv);
    return TCL_ERROR;
}

/*
 *-----------------------------------------------------------------------------
 *
 * Tcl_ChownCmd --
 *     Implements the TCL chown command:
 *     chown owner filelist
 *     chown {owner group} filelist
 *
 * Results:
 *  Standard TCL results, may return the UNIX system error message.
 *
 *-----------------------------------------------------------------------------
 */
int
Tcl_ChownCmd (clientData, interp, argc, argv)
    ClientData   clientData;
    Tcl_Interp  *interp;
    int          argc;
    char       **argv;
{
    int            idx, ownArgc, fileArgc;
    char         **ownArgv, **fileArgv = NULL;
    struct stat    fileStat;
    int            useOwnerGrp, chGroup, ownerId, groupId;
    struct passwd *passwdPtr;
    struct group  *groupPtr;
    int            result = TCL_ERROR;

    if (argc != 3) {
        Tcl_AppendResult (interp, tclXWrongArgs, argv [0], 
                          " owner|{owner group} filelist", (char *) NULL);
        return TCL_ERROR;
    }

    if (Tcl_SplitList (interp, argv[1], &ownArgc, &ownArgv) != TCL_OK)
        return TCL_ERROR;
    if ((ownArgc < 1) || (ownArgc > 2)) {
        interp->result = "owner arg should be: owner or {owner group}";
        goto exitPoint;
    }
    if (ownArgc == 2) {
        useOwnerGrp = (ownArgv [1][0] == '\0');
        chGroup = TRUE;
    } else
        chGroup = FALSE;

    /*
     * Get the owner id, either convert the name or use it as an integer.
     */
    passwdPtr = getpwnam (ownArgv [0]);
    if (passwdPtr != NULL)
        ownerId = passwdPtr->pw_uid;
    else {
        if (!Tcl_StrToInt (ownArgv [0], 10, &ownerId)) {
            Tcl_AppendResult (interp, "unknown user id: ", ownArgv [0],
                              (char *) NULL);
            goto exitPoint;
        }
    }
    /*
     * Get the group id, this is either the specified id or name, or the
     * if associated with the specified user.
     */
    if (chGroup) {
        if (useOwnerGrp) {
            if (passwdPtr == NULL) {
                passwdPtr = getpwuid (ownerId);
                if (passwdPtr != NULL) {
                    Tcl_AppendResult (interp, "unknown user id: ", 
                                      ownArgv [0], (char *) NULL);
                    goto exitPoint;
                }
            }
            groupId = passwdPtr->pw_gid;                        
        } else {
            groupPtr = getgrnam (ownArgv [1]);
            if (groupPtr != NULL)
                groupId = groupPtr->gr_gid;
            else {
                if (!Tcl_StrToInt (ownArgv [1], 10, &groupId)) {
                    Tcl_AppendResult (interp, "unknown group id: ", 
                                      ownArgv [1], (char *) NULL);
                    goto exitPoint;
                }
            }
        }
    }
    if (Tcl_SplitList (interp, argv [2], &fileArgc, &fileArgv) != TCL_OK)
        goto exitPoint;

    for (idx = 0; idx < fileArgc; idx++) {
        if (!chGroup) {
            if (stat (fileArgv [idx], &fileStat) != 0) {
                Tcl_AppendResult (interp, fileArgv [idx], ": ",
                                  Tcl_UnixError (interp), (char *) NULL);
                goto exitPoint;
            }
            groupId = fileStat.st_gid;
        }

        if (chown (fileArgv[idx], ownerId, groupId) < 0) {
            Tcl_AppendResult (interp, fileArgv [idx], ": ",
                              Tcl_UnixError (interp), (char *) NULL);
            goto exitPoint;
        }

    } /* Modify each file */

    result = TCL_OK;
exitPoint:
    ckfree ((char *) ownArgv);
    if (fileArgv != NULL)
        ckfree ((char *) fileArgv);
    return result;
}

/*
 *-----------------------------------------------------------------------------
 *
 * Tcl_ChgrpCmd --
 *     Implements the TCL chgrp command:
 *     chgrp group filelist
 *
 * Results:
 *  Standard TCL results, may return the UNIX system error message.
 *
 *-----------------------------------------------------------------------------
 */
int
Tcl_ChgrpCmd (clientData, interp, argc, argv)
    ClientData   clientData;
    Tcl_Interp  *interp;
    int          argc;
    char       **argv;
{
    int            idx, fileArgc, groupId, result = TCL_ERROR;
    char         **fileArgv;
    struct stat    fileStat;
    struct group  *groupPtr;

    if (argc < 3) {
        Tcl_AppendResult (interp, tclXWrongArgs, argv [0], 
                          " group filelist", (char *) NULL);
        return TCL_ERROR;
    }

    groupPtr = getgrnam (argv [1]);
    if (groupPtr != NULL)
        groupId = groupPtr->gr_gid;
    else {
        if (!Tcl_StrToInt (argv [1], 10, &groupId)) {
            Tcl_AppendResult (interp, "unknown group id: ", argv [1],
                              (char *) NULL);
            return TCL_ERROR;
        }
    }
    if (Tcl_SplitList (interp, argv [2], &fileArgc, &fileArgv) != TCL_OK)
        return TCL_ERROR;

    for (idx = 0; idx < fileArgc; idx++) {
        if ((stat (fileArgv [idx], &fileStat) != 0) ||
                (chown (fileArgv[idx], fileStat.st_uid, groupId) < 0)) {
            Tcl_AppendResult (interp, fileArgv [idx], ": ",
                              Tcl_UnixError (interp), (char *) NULL);
            goto exitPoint;
        }
    } /* Modify each file */

    result = TCL_OK;
exitPoint:
    ckfree ((char *) fileArgv);
    return result;
}
