/*
 * tclXid.c --
 *
 * Tcl commands to access getuid, setuid, getgid, setgid and friends.
 *---------------------------------------------------------------------------
 * Copyright 1992 Karl Lehenbauer and Mark Diekhans.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies.  Karl Lehenbauer and
 * Mark Diekhans make no representations about the suitability of this
 * software for any purpose.  It is provided "as is" without express or
 * implied warranty.
 *-----------------------------------------------------------------------------
 * $Id: tclXid.c,v 2.0 1992/10/16 04:50:51 markd Rel $
 *-----------------------------------------------------------------------------
 */

#include "tclxint.h"

/*
 * Prototypes of internal functions.
 */
static int
UseridToUsernameResult _ANSI_ARGS_((Tcl_Interp *interp,
                                    int         userId));

static int
UsernameToUseridResult _ANSI_ARGS_((Tcl_Interp *interp,
                                    char       *userName));

static int
GroupidToGroupnameResult _ANSI_ARGS_((Tcl_Interp *interp,
                                      int         groupId));

static int
GroupnameToGroupidResult _ANSI_ARGS_((Tcl_Interp *interp,
                                      char       *groupName));


/*
 *-----------------------------------------------------------------------------
 *
 * Tcl_IdCmd --
 *     Implements the TCL id command:
 *
 *        id user [name]
 *        id convert user <name>
 *
 *        id userid [uid]
 *        id convert userid <uid>
 *
 *        id group [name]
 *        id convert group <name>
 *
 *        id groupid [gid]
 *        id convert groupid <gid>
 *
 *        id process
 *        id process parent
 *        id process group
 *        id process group set
 *
 *        id effective user
 *        id effective userid
 *
 *        id effective group
 *        id effective groupid
 *
 * Results:
 *  Standard TCL results, may return the UNIX system error message.
 *
 *-----------------------------------------------------------------------------
 */

static int
UseridToUsernameResult (interp, userId)
    Tcl_Interp *interp;
    int         userId;
{
    struct passwd *pw = getpwuid (userId);
    if (pw == NULL) {
        char numBuf [32];

        sprintf (numBuf, "%d", userId);
        Tcl_AppendResult (interp, "unknown user id: ", numBuf, (char *) NULL);
        return TCL_ERROR;
    }
    strcpy (interp->result, pw->pw_name);
    return TCL_OK;
}

static int
UsernameToUseridResult (interp, userName)
    Tcl_Interp *interp;
    char       *userName;
{
    struct passwd *pw = getpwnam (userName);
    if (pw == NULL) {
        Tcl_AppendResult (interp, "unknown user id: ", userName, 
                          (char *) NULL);
        return TCL_ERROR;
    }
    sprintf (interp->result, "%d", pw->pw_uid);
    return TCL_OK;
}

static int
GroupidToGroupnameResult (interp, groupId)
    Tcl_Interp *interp;
    int         groupId;
{
    struct group *grp = getgrgid (groupId);
    if (grp == NULL) {
        char numBuf [32];

        sprintf (numBuf, "%d", groupId);
        Tcl_AppendResult (interp, "unknown group id: ", numBuf, (char *) NULL);
        return TCL_ERROR;
    }
    strcpy (interp->result, grp->gr_name);
    return TCL_OK;
}

static int
GroupnameToGroupidResult (interp, groupName)
    Tcl_Interp *interp;
    char       *groupName;
{
    struct group *grp = getgrnam (groupName);
    if (grp == NULL) {
        Tcl_AppendResult (interp, "unknown group id: ", groupName,
                          (char *) NULL);
        return TCL_ERROR;
    }
    sprintf (interp->result, "%d", grp->gr_gid);
    return TCL_OK;
}

int
Tcl_IdCmd (clientData, interp, argc, argv)
    ClientData  clientData;
    Tcl_Interp *interp;
    int         argc;
    char      **argv;
{
    struct passwd *pw;
    struct group *grp;
    int uid, gid;

    if (argc < 2) 
        goto bad_args;

    /*
     * If the first argument is "convert", handle the conversion.
     */
    if (STREQU (argv[1], "convert")) {
        if (argc != 4) {
            Tcl_AppendResult (interp, tclXWrongArgs, argv [0], 
                              " convert arg arg", (char *) NULL);
            return TCL_ERROR;
        }

        if (STREQU (argv[2], "user"))
            return UsernameToUseridResult (interp, argv[3]);

        if (STREQU (argv[2], "userid")) {
            if (Tcl_GetInt (interp, argv[3], &uid) != TCL_OK) 
                return TCL_ERROR;
            return UseridToUsernameResult (interp, uid);
        }

        if (STREQU (argv[2], "group"))
            return GroupnameToGroupidResult (interp, argv[3]);

        if (STREQU (argv[2], "groupid")) {
            if (Tcl_GetInt (interp, argv[3], &gid) != TCL_OK) return TCL_ERROR;
            return GroupidToGroupnameResult (interp, gid);

        }
        goto bad_three_arg;
    }

    /*
     * If the first argument is "effective", return the effective user ID,
     * name, group ID or name.
     */
    if (STREQU (argv[1], "effective")) {
        if (argc != 3) {
            Tcl_AppendResult (interp, tclXWrongArgs, argv [0], 
                              " effective arg", (char *) NULL);
            return TCL_ERROR;
        }

        if (STREQU (argv[2], "user"))
            return UseridToUsernameResult (interp, geteuid ());

        if (STREQU (argv[2], "userid")) {
            sprintf (interp->result, "%d", geteuid ());
            return TCL_OK;
        }

        if (STREQU (argv[2], "group"))
            return GroupidToGroupnameResult (interp, getegid ());

        if (STREQU (argv[2], "groupid")) {
            sprintf (interp->result, "%d", getegid ());
            return TCL_OK;
        }
        goto bad_three_arg;
    }

    /*
     * If the first argument is "process", return the process ID, parent's
     * process ID, process group or set the process group depending on args.
     */
    if (STREQU (argv[1], "process")) {
        if (argc == 2) {
            sprintf (interp->result, "%d", getpid ());
            return TCL_OK;
        }

        if (STREQU (argv[2], "parent")) {
            if (argc != 3) {
                Tcl_AppendResult (interp, tclXWrongArgs, argv [0], 
                                  " process parent", (char *) NULL);
                return TCL_ERROR;
            }
            sprintf (interp->result, "%d", getppid ());
            return TCL_OK;
        }
        if (STREQU (argv[2], "group")) {
            if (argc == 3) {
                sprintf (interp->result, "%d", getpgrp ());
                return TCL_OK;
            }
            if ((argc != 4) || !STREQU (argv[3], "set")) {
                Tcl_AppendResult (interp, tclXWrongArgs, argv [0], 
                                  " process group [set]", (char *) NULL);
                return TCL_ERROR;
            }
            setpgrp ();
            return TCL_OK;
        }
        Tcl_AppendResult (interp, tclXWrongArgs, argv [0], 
                          " process [parent|group|group set]", (char *) NULL);
        return TCL_ERROR;
    }

    /*
     * Handle setting or returning the user ID or group ID (by name or number).
     */
    if (argc > 3)
        goto bad_args;

    if (STREQU (argv[1], "user")) {
        if (argc == 2) {
            return UseridToUsernameResult (interp, getuid ());
        } else {
            pw = getpwnam (argv[2]);
            if (pw == NULL)
                goto name_doesnt_exist;
            if (setuid (pw->pw_uid) < 0)
                goto cannot_set_name;
            return TCL_OK;
        }
    }

    if (STREQU (argv[1], "userid")) {
        if (argc == 2) {
            sprintf (interp->result, "%d", getuid ());
            return TCL_OK;
        } else {
            if (Tcl_GetInt (interp, argv[2], &uid) != TCL_OK)
                return TCL_ERROR;
            if (setuid (uid) < 0) 
                goto cannot_set_name;
            return TCL_OK;
        }
    }

    if (STREQU (argv[1], "group")) {
        if (argc == 2) {
            return GroupidToGroupnameResult (interp, getgid ());
        } else {
            grp = getgrnam (argv[2]);
            if (grp == NULL) 
                goto name_doesnt_exist;
            if (setgid (grp->gr_gid) < 0)
                goto cannot_set_name;
            return TCL_OK;
        }
    }

    if (STREQU (argv[1], "groupid")) {
        if (argc == 2) {
            sprintf (interp->result, "%d", getgid ());
            return TCL_OK;
        } else {
            if (Tcl_GetInt (interp, argv[2], &gid) != TCL_OK)
                return TCL_ERROR;
            if (setgid (gid) < 0)
                goto cannot_set_name;
            return TCL_OK;
        }
    }
    Tcl_AppendResult (interp, "bad arg: ", argv [0], 
                      " second arg must be convert, effective, process, ",
                      "user, userid, group or groupid", (char *) NULL);
    return TCL_ERROR;


  bad_three_arg:
    Tcl_AppendResult (interp, "bad arg: ", argv [0], ": ", argv[1],
                      ": third arg must be user, userid, group or groupid",
                      (char *) NULL);
    return TCL_ERROR;
  bad_args:
    Tcl_AppendResult (interp, tclXWrongArgs, argv [0], " arg [arg..]",
                      (char *) NULL);
    return TCL_ERROR;

  name_doesnt_exist:
    Tcl_AppendResult (interp, " \"", argv[2], "\" does not exists",
                      (char *) NULL);
    return TCL_ERROR;

  cannot_set_name:
    interp->result = Tcl_UnixError (interp);
    return TCL_ERROR;
}
