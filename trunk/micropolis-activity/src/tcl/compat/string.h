/*
 * string.h --
 *
 *	Declarations of ANSI C library procedures for string handling.
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
 * $Header: /sprite/src/lib/tcl/compat/RCS/string.h,v 1.1 91/09/19 16:22:11 ouster Exp $ SPRITE (Berkeley)
 */

#ifndef _STRING
#define _STRING

#include <tcl.h>

extern char *		memchr _ANSI_ARGS_((char *s, int c, int n));
#if 0
extern int		memcmp _ANSI_ARGS_((char *s1, char *s2, int n));
extern char *		memcpy _ANSI_ARGS_((char *t, char *f, int n));
#endif
extern char *		memmove _ANSI_ARGS_((char *t, char *f, int n));
extern char *		memset _ANSI_ARGS_((char *s, int c, int n));

extern int		strcasecmp _ANSI_ARGS_((char *s1, char *s2));
extern char *		strcat _ANSI_ARGS_((char *dst, char *src));
extern char *		strchr _ANSI_ARGS_((char *string, int c));
#if 0
extern int		strcmp _ANSI_ARGS_((char *s1, char *s2));
extern char *		strcpy _ANSI_ARGS_((char *dst, char *src));
#endif
extern int		strcspn _ANSI_ARGS_((char *string, char *chars));
extern char *		strdup _ANSI_ARGS_((char *string));
extern char *		strerror _ANSI_ARGS_((int error));
#if 0
extern int		strlen _ANSI_ARGS_((char *string));
#endif
extern int		strncasecmp _ANSI_ARGS_((char *s1, char *s2, int n));
extern char *		strncat _ANSI_ARGS_((char *dst, char *src,
			    int numChars));
extern int		strncmp _ANSI_ARGS_((char *s1, char *s2, int nChars));
extern char *		strncpy _ANSI_ARGS_((char *dst, char *src,
			    int numChars));
extern char *		strpbrk _ANSI_ARGS_((char *string, char *chars));
extern char *		strrchr _ANSI_ARGS_((char *string, int c));
extern int		strspn _ANSI_ARGS_((char *string, char *chars));
extern char *		strstr _ANSI_ARGS_((char *string, char *substring));
extern char *		strtok _ANSI_ARGS_((char *s, char *delim));

#endif /* _STRING */
