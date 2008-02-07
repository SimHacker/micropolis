/* 
 * teststrtoul.c --
 *
 *	This file contains a simple program to detect broken versions
 *	of strtoul, like those on AIX.  The broken versions return an
 *	incorrect terminator pointer for the string "0".  This program
 *	exits with a normal status if strtoul does the right thing, and
 *	it exits with a non-zero status if strtoul is bogus.  Unlike
 *	the other test programs, this one must actually be executed to
 *	be sure that it works.
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
static char rcsid[] = "$Header: /user6/ouster/tcl/compat/RCS/teststrtoul.c,v 1.2 92/01/07 10:02:56 ouster Exp $ SPRITE (Berkeley)";
#endif /* not lint */

extern int strtoul();

int main()
{
    char *string = "0";
    char *term;
    int value;

    value = strtoul(string, &term, 0);
    if ((value != 0) || (term != (string+1))) {
	exit(1);
    }
    exit(0);
}
