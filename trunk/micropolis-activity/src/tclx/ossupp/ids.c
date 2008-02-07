/*
 * ids.c
 *
 * Public-domain relatively quick-and-dirty implemenation of
 * ANSI library routine for System V Unix systems.
 *
 * It's written in old-style C for maximal portability.
 *
 * Arnold Robbins
 * January, February, 1991
 *
 * Fixes from ado@elsie.nci.nih.gov
 * February 1991
 *-----------------------------------------------------------------------------
 * $Id: ids.c,v 2.0 1992/10/16 04:52:16 markd Rel $
 *-----------------------------------------------------------------------------
 */

/*
 * To avoid Unix version problems, this code has been simplified to avoid
 * const and size_t, however this can cause an incompatible definition on
 * ansi-C systems, so a game is played with defines to ignore a strftime
 * declaration in time.h
 */

#include <sys/types.h>

static uid_t usrID  = 100;
static gid_t grpID  = 10;

static int   procID = 12345;
static int   pgrpID = 10;


/* getppid --- returns the process ID of the parent process */

int
getppid()
{
    return procID;
}

/* getpgrp --- returns the process group of the process indicated by pid */

int
getpgrp(pid)
    int pid;
{
    return pgrpID;
}

/* setpgrp --- sets the process group of the process indicated by pid */

int
setpgrp(pid, pgrp)
    int pid;
    int pgrp;
{
    pgrpID = pgrp;
    return pgrp;
}

/* setuid --- sets the real and effective user ID of the current process */

int setuid(uid)
    uid_t  uid;
{
    usrID = uid;
    return 0;
}

/* getgid --- gets the effective group ID of the current process */

uid_t setgid(gid)
    gid_t  gid;
{
    grpID = gid;
    return 0;
}

