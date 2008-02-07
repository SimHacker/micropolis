/*
 * dirent.h --
 *
 *	Declarations of a library of directory-reading procedures
 *	in the POSIX style ("struct dirent").
 *
 * Copyright 1991 Regents of the University of California
 * Permission to use, copy, modify, and distribute this
 * software and its documentation for any purpose and without
 * fee is hereby granted, provided that this copyright
 * notice appears in all copies.  The University of California
 * makes no representations about the suitability of this
 * software for any purpose.  It is provided "as is" without
 * express or implied warranty.
 *
 * $Header: /sprite/src/lib/tcl/compat/RCS/dirent2.h,v 1.1 91/09/19 16:22:08 ouster Exp $ SPRITE (Berkeley)
 */

#ifndef _DIRENT
#define _DIRENT

#ifndef _TCL
#include <tcl.h>
#endif

/*
 * Dirent structure, which holds information about a single
 * directory entry.
 */

#define MAXNAMLEN 255
#define DIRBLKSIZ 512

struct dirent {
    long d_ino;			/* Inode number of entry */
    short d_reclen;		/* Length of this record */
    short d_namlen;		/* Length of string in d_name */
    char d_name[MAXNAMLEN + 1];	/* Name must be no longer than this */
};

/*
 * State that keeps track of the reading of a directory (clients
 * should never look inside this structure;  the fields should
 * only be accessed by the library procedures).
 */

typedef struct _dirdesc {
    int dd_fd;
    long dd_loc;
    long dd_size;
    char dd_buf[DIRBLKSIZ];
} DIR;

/*
 * Procedures defined for reading directories:
 */

extern void		closedir _ANSI_ARGS_((DIR *dirp));
extern DIR *		opendir _ANSI_ARGS_((char *name));
extern struct dirent *	readdir _ANSI_ARGS_((DIR *dirp));

#endif /* _DIRENT */
