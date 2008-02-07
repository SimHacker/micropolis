/*
 * sigs.c
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
 * $Id: sigs.c,v 2.0 1992/10/16 04:52:16 markd Rel $
 *-----------------------------------------------------------------------------
 */

/*
 * To avoid Unix version problems, this code has been simplified to avoid
 * const and size_t, however this can cause an incompatible definition on
 * ansi-C systems, so a game is played with defines to ignore a strftime
 * declaration in time.h
 */

#include <signal.h>


/* sigprocmask --- examines or changes the calling processes' signal mask */

int
sigprocmask(how, set, oset)
    int    how;
    sigset_t *set;
    sigset_t *oset;
{
    int rc = 0;

    return rc;
}
