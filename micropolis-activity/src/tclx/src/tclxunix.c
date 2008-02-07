/*
 * tclXunixcmds.c --
 *
 * Tcl commands to access unix library calls.
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
 * $Id: tclXunixcmds.c,v 2.0 1992/10/16 04:51:18 markd Rel $
 *-----------------------------------------------------------------------------
 */

#include "tclxint.h"

/*
 * A million microsecondss per seconds.
 */
#define TCL_USECS_PER_SEC (1000L * 1000L)

extern
double floor ();

extern
double ceil ();

/*
 *-----------------------------------------------------------------------------
 *
 * Tcl_AlarmCmd --
 *     Implements the TCL Alarm command:
 *         alarm seconds
 *
 * Results:
 *      Standard TCL results, may return the UNIX system error message.
 *
 *-----------------------------------------------------------------------------
 */
int
Tcl_AlarmCmd (clientData, interp, argc, argv)
    ClientData  clientData;
    Tcl_Interp *interp;
    int         argc;
    char      **argv;
{
#ifdef TCL_NO_ITIMER
    double            seconds;
    unsigned          useconds;

    if (argc != 2) {
        Tcl_AppendResult (interp, tclXWrongArgs, argv [0], " seconds", 
                          (char *) NULL);
        return TCL_ERROR;
    }

    if (Tcl_GetDouble (interp, argv[1], &seconds) != TCL_OK)
        return TCL_ERROR;

    useconds = ceil (seconds);
#ifdef MSDOS
    sprintf (interp->result, "%d", sleep (useconds));
#else
    sprintf (interp->result, "%d", alarm (useconds));
#endif

    return TCL_OK;
#else

    double            seconds, secFloor;
    struct itimerval  timer, oldTimer;

    if (argc != 2) {
        Tcl_AppendResult (interp, tclXWrongArgs, argv [0], " seconds", 
                          (char *) NULL);
        return TCL_ERROR;
    }

    if (Tcl_GetDouble (interp, argv[1], &seconds) != TCL_OK)
        return TCL_ERROR;

    secFloor =  floor (seconds);

    timer.it_value.tv_sec     = secFloor;
    timer.it_value.tv_usec    = (long) ((seconds - secFloor) *
                                        (double) TCL_USECS_PER_SEC);
    timer.it_interval.tv_sec  = 0;
    timer.it_interval.tv_usec = 0;  


    if (setitimer (ITIMER_REAL, &timer, &oldTimer) < 0) {
        interp->result = Tcl_UnixError (interp);
        return TCL_ERROR;
    }
    seconds  = oldTimer.it_value.tv_sec;
    seconds += ((double) oldTimer.it_value.tv_usec) /
               ((double) TCL_USECS_PER_SEC);
    sprintf (interp->result, "%g", seconds);

    return TCL_OK;
#endif

}

/*
 *-----------------------------------------------------------------------------
 *
 * Tcl_SleepCmd --
 *     Implements the TCL sleep command:
 *         sleep seconds
 *
 * Results:
 *      Standard TCL results, may return the UNIX system error message.
 *
 *-----------------------------------------------------------------------------
 */
int
Tcl_SleepCmd (clientData, interp, argc, argv)
    ClientData  clientData;
    Tcl_Interp *interp;
    int         argc;
    char      **argv;
{
    unsigned time;

    if (argc != 2) {
        Tcl_AppendResult (interp, tclXWrongArgs, argv [0], " seconds", 
                          (char *) NULL);
        return TCL_ERROR;
    }

    if (Tcl_GetUnsigned (interp, argv[1], &time) != TCL_OK)
        return TCL_ERROR;

    sleep (time);
    return TCL_OK;

}

/*
 *-----------------------------------------------------------------------------
 *
 * Tcl_SystemCmd --
 *     Implements the TCL system command:
 *     system command
 *
 * Results:
 *  Standard TCL results, may return the UNIX system error message.
 *
 *-----------------------------------------------------------------------------
 */
int
Tcl_SystemCmd (clientData, interp, argc, argv)
    ClientData  clientData;
    Tcl_Interp *interp;
    int         argc;
    char      **argv;
{
    int exitCode;

    if (argc != 2) {
        Tcl_AppendResult (interp, tclXWrongArgs, argv [0], " command",
                          (char *) NULL);
        return TCL_ERROR;
    }

    exitCode = Tcl_System (interp, argv[1]);
    if (exitCode == -1)
        return TCL_ERROR;
    sprintf (interp->result, "%d", exitCode);
    return TCL_OK;
}

/*
 *-----------------------------------------------------------------------------
 *
 * Tcl_TimesCmd --
 *     Implements the TCL times command:
 *     times
 *
 * Results:
 *  Standard TCL results.
 *
 *-----------------------------------------------------------------------------
 */
int
Tcl_TimesCmd (clientData, interp, argc, argv)
    ClientData  clientData;
    Tcl_Interp *interp;
    int         argc;
    char      **argv;
{
    struct tms tm;

    if (argc != 1) {
        Tcl_AppendResult (interp, tclXWrongArgs, argv[0], (char *) NULL);
        return TCL_ERROR;
    }

    times(&tm);

    sprintf(interp->result, "%ld %ld %ld %ld", 
#ifdef MSDOS
            tm.tms_utime  * MS_PER_TICK, 
            tm.tms_utime2 * MS_PER_TICK, 
            tm.tms_stime  * MS_PER_TICK, 
            tm.tms_stime2 * MS_PER_TICK);
#else
            tm.tms_utime  * MS_PER_TICK, 
            tm.tms_stime  * MS_PER_TICK, 
            tm.tms_cutime * MS_PER_TICK, 
            tm.tms_cstime * MS_PER_TICK);
#endif
    return TCL_OK;
}

/*
 *-----------------------------------------------------------------------------
 *
 * Tcl_UmaskCmd --
 *     Implements the TCL umask command:
 *     umask [octalmask]
 *
 * Results:
 *  Standard TCL results, may return the UNIX system error message.
 *
 *-----------------------------------------------------------------------------
 */
int
Tcl_UmaskCmd (clientData, interp, argc, argv)
    ClientData  clientData;
    Tcl_Interp *interp;
    int         argc;
    char      **argv;
{
    int mask;

    if ((argc < 1) || (argc > 2)) {
        Tcl_AppendResult (interp, tclXWrongArgs, argv [0], " octalmask",
                          (char *) NULL);
        return TCL_ERROR;
    }

    if (argc == 1) {
        mask = umask (0);
        umask ((unsigned short) mask);
        sprintf (interp->result, "%o", mask);
    } else {
        if (!Tcl_StrToInt (argv [1], 8, &mask)) {
            Tcl_AppendResult (interp, "Expected octal number got: ", argv [1],
                              (char *) NULL);
            return TCL_ERROR;
        }

        umask ((unsigned short) mask);
    }

    return TCL_OK;
}

/*
 *-----------------------------------------------------------------------------
 *
 * Tcl_LinkCmd --
 *     Implements the TCL link command:
 *         link [-sym] srcpath destpath
 *
 * Results:
 *  Standard TCL results, may return the UNIX system error message.
 *
 *-----------------------------------------------------------------------------
 */
int
Tcl_LinkCmd (clientData, interp, argc, argv)
    ClientData  clientData;
    Tcl_Interp *interp;
    int         argc;
    char      **argv;
{
    char *tmppath, *srcpath, *destpath;

    if ((argc < 3) || (argc > 4)) {
        Tcl_AppendResult (interp, tclXWrongArgs, argv [0], 
                          " [-sym] srcpath destpath", (char *) NULL);
        return TCL_ERROR;
    }
    if (argc == 4) {
        if (!STREQU (argv [1], "-sym")) {
            Tcl_AppendResult (interp, "invalid option, expected: \"-sym\", ",
                              "got: ", argv [1], (char *) NULL);
            return TCL_ERROR;
        }
#ifndef S_IFLNK
        Tcl_AppendResult (interp, "symbolic links are not supported on this",
                          " system", (char *) NULL);
        return TCL_ERROR;
#endif
    }

    tmppath = Tcl_TildeSubst (interp, argv [argc - 2]);
    if (tmppath == NULL)
        return TCL_ERROR;
    srcpath = ckalloc (strlen (tmppath) + 1);
    strcpy (srcpath, tmppath);

    destpath = Tcl_TildeSubst (interp, argv [argc - 1]);
    if (destpath == NULL)
        goto errorExit;

    if (argc == 4) {
#ifdef S_IFLNK
        if (symlink (srcpath, destpath) != 0)
           goto unixError;
#endif
    } else {
        if (link (srcpath, destpath) != 0)
           goto unixError;
    }
    ckfree (srcpath);
    return TCL_OK;

unixError:
    interp->result = Tcl_UnixError (interp);

errorExit:
    ckfree (srcpath);
    return TCL_ERROR;
}

/*
 *-----------------------------------------------------------------------------
 *
 * Tcl_UnlinkCmd --
 *     Implements the TCL unlink command:
 *         unlink [-nocomplain] fileList
 *
 * Results:
 *  Standard TCL results, may return the UNIX system error message.
 *
 *-----------------------------------------------------------------------------
 */
int
Tcl_UnlinkCmd (clientData, interp, argc, argv)
    ClientData  clientData;
    Tcl_Interp *interp;
    int         argc;
    char      **argv;
{
    int    idx, fileArgc;
    char **fileArgv, *fileName;
    int    noComplain;
    
    if ((argc < 2) || (argc > 3))
        goto badArgs;

    if (argc == 3) {
        if (!STREQU (argv [1], "-nocomplain"))
            goto badArgs;
        noComplain = TRUE;
    } else {
        noComplain = FALSE;
    }

    if (Tcl_SplitList (interp, argv [argc - 1], &fileArgc,
                       &fileArgv) != TCL_OK)
        return TCL_ERROR;

    for (idx = 0; idx < fileArgc; idx++) {
        fileName = Tcl_TildeSubst (interp, fileArgv [idx]);
        if (fileName == NULL) {
            if (!noComplain)
                goto errorExit;
            continue;
        }
        if ((unlink (fileName) != 0) && !noComplain) {
            Tcl_AppendResult (interp, fileArgv [idx], ": ",
                              Tcl_UnixError (interp), (char *) NULL);
            goto errorExit;
        }
    }

    ckfree ((char *) fileArgv);
    return TCL_OK;

errorExit:
    ckfree ((char *) fileArgv);
    return TCL_ERROR;

badArgs:
    Tcl_AppendResult (interp, tclXWrongArgs, argv [0], 
                      " [-nocomplain] filelist", (char *) NULL);
    return TCL_ERROR;
}

/*
 *-----------------------------------------------------------------------------
 *
 * Tcl_MkdirCmd --
 *     Implements the TCL Mkdir command:
 *         mkdir [-path] dirList
 *
 * Results:
 *  Standard TCL results, may return the UNIX system error message.
 *
 *-----------------------------------------------------------------------------
 */
int
Tcl_MkdirCmd (clientData, interp, argc, argv)
    ClientData  clientData;
    Tcl_Interp *interp;
    int         argc;
    char      **argv;
{
    int           idx, dirArgc, result;
    char        **dirArgv, *scanPtr;
    struct stat   statBuf;

    if ((argc < 2) || (argc > 3))
        goto usageError;
    if ((argc == 3) && !STREQU (argv [1], "-path"))
        goto usageError;

    if (Tcl_SplitList (interp, argv [argc - 1], &dirArgc, &dirArgv) != TCL_OK)
        return TCL_ERROR;
    /*
     * Make all the directories, optionally making directories along the path.
     */

    for (idx = 0; idx < dirArgc; idx++) {
        /*
         * Make leading directories, if requested.
         */
        if (argc == 3) {
            scanPtr = dirArgv [idx];
            result = 0;  /* Start out ok, for dirs that are skipped */

            while (*scanPtr != '\0') {
                scanPtr = strchr (scanPtr+1, '/');
                if ((scanPtr == NULL) || (*(scanPtr+1) == '\0'))
                    break;
                *scanPtr = '\0';
                if (stat (dirArgv [idx], &statBuf) < 0)
                    result = mkdir (dirArgv [idx], S_IFDIR | 0777);
                *scanPtr = '/';
                if (result < 0)
                   goto mkdirError;
            }
        }
        /*
         * Make final directory in the path.
         */
        if (mkdir (dirArgv [idx], S_IFDIR | 0777) != 0)
           goto mkdirError;
    }

    ckfree ((char *) dirArgv);
    return TCL_OK;

mkdirError:
    Tcl_AppendResult (interp, dirArgv [idx], ": ", Tcl_UnixError (interp),
                      (char *) NULL);
    ckfree ((char *) dirArgv);
    return TCL_ERROR;

usageError:
    Tcl_AppendResult (interp, tclXWrongArgs, argv [0], 
                      " [-path] dirlist", (char *) NULL);
    return TCL_ERROR;
}

/*
 *-----------------------------------------------------------------------------
 *
 * Tcl_RmdirCmd --
 *     Implements the TCL Rmdir command:
 *         rmdir [-nocomplain]  dirList
 *
 * Results:
 *  Standard TCL results, may return the UNIX system error message.
 *
 *-----------------------------------------------------------------------------
 */
int
Tcl_RmdirCmd (clientData, interp, argc, argv)
    ClientData  clientData;
    Tcl_Interp *interp;
    int         argc;
    char      **argv;
{
    int    idx, dirArgc;
    char **dirArgv, *dirName;
    int    noComplain;
    
    if ((argc < 2) || (argc > 3))
        goto badArgs;

    if (argc == 3) {
        if (!STREQU (argv [1], "-nocomplain"))
            goto badArgs;
        noComplain = TRUE;
    } else {
        noComplain = FALSE;
    }

    if (Tcl_SplitList (interp, argv [argc - 1], &dirArgc, &dirArgv) != TCL_OK)
        return TCL_ERROR;

    for (idx = 0; idx < dirArgc; idx++) {
        dirName = Tcl_TildeSubst (interp, dirArgv [idx]);
        if (dirName == NULL) {
            if (!noComplain)
                goto errorExit;
            continue;
        }
        if ((rmdir (dirName) != 0) && !noComplain) {
           Tcl_AppendResult (interp, dirArgv [idx], ": ",
                             Tcl_UnixError (interp), (char *) NULL);
           goto errorExit;
        }
    }

    ckfree ((char *) dirArgv);
    return TCL_OK;

errorExit:
    ckfree ((char *) dirArgv);
    return TCL_ERROR;;

badArgs:
    Tcl_AppendResult (interp, tclXWrongArgs, argv [0], 
                      " [-nocomplain] dirlist", (char *) NULL);
    return TCL_ERROR;
}
