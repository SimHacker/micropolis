/*
 * times.c
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
 * $Id: times.c,v 2.0 1992/10/16 04:52:16 markd Rel $
 *-----------------------------------------------------------------------------
 */

/*
 * To avoid Unix version problems, this code has been simplified to avoid
 * const and size_t, however this can cause an incompatible definition on
 * ansi-C systems, so a game is played with defines to ignore a strftime
 * declaration in time.h
 */

#include <sys/types.h>
#include <time.h>
#include <sys/times.h>


/* times --- produce process times */

clock_t 
times(buffer)
    struct tms      *buffer;
{
    clock_t clk_time;

    clk_time = clock();
    buffer->tms_utime  = clk_time;
    buffer->tms_utime2 = clk_time;
    buffer->tms_stime  = clk_time;
    buffer->tms_stime2 = clk_time;

    return clk_time;
}
