/*
 * stdlib.h --
 *
 *	Declares facilities exported by the "stdlib" portion of
 *	the C library.  This file isn't complete in the ANSI-C
 *	sense;  it only declares things that are needed by Tcl.
 *	This file is needed even on many systems with their own
 *	stdlib.h (e.g. SunOS) because not all stdlib.h files
 *	declare all the procedures needed here (such as strtod).
 *
 * Copyright 1991 Regents of the University of California
 * Permission to use, copy, modify, and distribute this
 * software and its documentation for any purpose and without
 * fee is hereby granted, provided that the above copyright
 * notice appears in all copies.  The University of California
 * makes no representations about the suitability of this
 * software for any purpose.  It is provided "as is" without
 * express or implied warranty.
 *
 * $Header: /user6/ouster/tcl/compat/RCS/stdlib.h,v 1.3 91/12/06 10:42:05 ouster Exp $ SPRITE (Berkeley)
 */

#ifndef _STDLIB
#define _STDLIB

#include <tclInt.h>

extern void		abort _ANSI_ARGS_((void));
extern double		atof _ANSI_ARGS_((char *string));
extern int		atoi _ANSI_ARGS_((char *string));
extern long		atol _ANSI_ARGS_((char *string));
extern char *		calloc _ANSI_ARGS_((unsigned int numElements,
			    unsigned int size));
extern int		exit _ANSI_ARGS_((int status));
extern int		free _ANSI_ARGS_((char *blockPtr));
extern char *		getenv _ANSI_ARGS_((char *name));
extern char *		malloc _ANSI_ARGS_((unsigned int numBytes));
extern void		qsort _ANSI_ARGS_((VOID *base, int n, int size,
			    int (*compar)(CONST VOID *element1, CONST VOID
			    *element2)));
extern char *		realloc _ANSI_ARGS_((char *ptr, unsigned int numBytes));
extern double		strtod _ANSI_ARGS_((char *string, char **endPtr));
extern long		strtol _ANSI_ARGS_((char *string, char **endPtr,
			    int base));
extern unsigned long	strtoul _ANSI_ARGS_((CONST char *string,
			    char **endPtr, int base));

#endif /* _STDLIB */
