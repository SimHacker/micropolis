/* 
 * testuid.c --
 *
 *	This file contains a simple program that will compile
 *	correctly if and only if <sys/types.h> defines the
 *	type uid_t.  It is used to determine whether this type
 *	is defined on a given system.
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
static char rcsid[] = "$Header: /user6/ouster/tcl/compat/RCS/testuid.c,v 1.1 91/12/06 15:31:28 ouster Exp $ SPRITE (Berkeley)";
#endif /* not lint */

#include <sys/types.h>

uid_t uid;

int main()
{
    return 0;
}
