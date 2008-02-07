/* 
 * testwait.c --
 *
 *	This file contains a simple program that will compile
 *	correctly if and only if <sys/wait.h> defines the
 *	type "union wait".  It is used during configuration
 *	to determine whether or not to use this type.
 *
 * Copyright 1991 Regents of the University of California
 * Permission to use, copy, modify, and distribute this
 * software and its documentation for any purpose and without
 * fee is hereby granted, provided that this copyright
 * notice appears in all copies.  The University of California
 * makes no representations about the suitability of this
 * software for any purpose.  It is provided "as is" without
 * express or implied warranty.
 */

#ifndef lint
static char rcsid[] = "$Header: /user6/ouster/tcl/compat/RCS/testwait.c,v 1.3 91/12/18 13:42:09 ouster Exp $ SPRITE (Berkeley)";
#endif /* not lint */

#include <sys/types.h>
#include <sys/wait.h>

/*
 * This code is very tricky, mostly because of weirdness in HP-UX
 * where "union wait" is defined in both the BSD and SYS-V
 * environments.  Supposedly the WIFEXITED business will do the
 * right thing...
 */

union wait x;

int main()
{
    WIFEXITED(x);		/* Generates compiler error if WIFEXITED
				 * uses an int. */
    return 0;
}
