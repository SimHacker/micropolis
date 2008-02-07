/* 
 * strerror.c --
 *
 *	Source code for the "strerror" library routine.
 *
 * Copyright 1988-1991 Regents of the University of California
 * Permission to use, copy, modify, and distribute this
 * software and its documentation for any purpose and without
 * fee is hereby granted, provided that the above copyright
 * notice appears in all copies.  The University of California
 * makes no representations about the suitability of this
 * software for any purpose.  It is provided "as is" without
 * express or implied warranty.
 */

#ifndef lint
static char rcsid[] = "$Header: /user6/ouster/tcl/compat/RCS/strerror.c,v 1.2 91/12/16 09:26:48 ouster Exp $ SPRITE (Berkeley)";
#endif /* not lint */

#include <tclint.h>
#include <tclunix.h>

/*
 *----------------------------------------------------------------------
 *
 * strerror --
 *
 *	Map an integer error number into a printable string.
 *
 * Results:
 *	The return value is a pointer to a string describing
 *	error.  The first character of string isn't capitalized.
 *
 * Side effects:
 *	Each call to this procedure may overwrite the value returned
 *	by the previous call.
 *
 *----------------------------------------------------------------------
 */

char *
strerror(error)
    int error;			/* Integer identifying error (must be
				 * one of the officially-defined Sprite
				 * errors, as defined in errno.h). */
{
    static char msg[50];

#if TCL_SYS_ERRLIST
    if ((error <= sys_nerr) && (error > 0)) {
	return sys_errlist[error];
    }
#else
    switch (error) {
#ifdef E2BIG
	case E2BIG: return "argument list too long";
#endif
#ifdef EACCES
	case EACCES: return "permission denied";
#endif
#ifdef EADDRINUSE
	case EADDRINUSE: return "address already in use";
#endif
#ifdef EADDRNOTAVAIL
	case EADDRNOTAVAIL: return "can't assign requested address";
#endif
#ifdef EADV
	case EADV: return "advertise error";
#endif
#ifdef EAFNOSUPPORT
	case EAFNOSUPPORT: return "address family not supported by protocol family";
#endif
#ifdef EAGAIN
	case EAGAIN: return "no more processes";
#endif
#ifdef EALIGN
	case EALIGN: return "EALIGN";
#endif
#ifdef EALREADY
	case EALREADY: return "operation already in progress";
#endif
#ifdef EBADE
	case EBADE: return "bad exchange descriptor";
#endif
#ifdef EBADF
	case EBADF: return "bad file number";
#endif
#ifdef EBADFD
	case EBADFD: return "file descriptor in bad state";
#endif
#ifdef EBADMSG
	case EBADMSG: return "not a data message";
#endif
#ifdef EBADR
	case EBADR: return "bad request descriptor";
#endif
#ifdef EBADRPC
	case EBADRPC: return "RPC structure is bad";
#endif
#ifdef EBADRQC
	case EBADRQC: return "bad request code";
#endif
#ifdef EBADSLT
	case EBADSLT: return "invalid slot";
#endif
#ifdef EBFONT
	case EBFONT: return "bad font file format";
#endif
#ifdef EBUSY
	case EBUSY: return "mount device busy";
#endif
#ifdef ECHILD
	case ECHILD: return "no children";
#endif
#ifdef ECHRNG
	case ECHRNG: return "channel number out of range";
#endif
#ifdef ECOMM
	case ECOMM: return "communication error on send";
#endif
#ifdef ECONNABORTED
	case ECONNABORTED: return "software caused connection abort";
#endif
#ifdef ECONNREFUSED
	case ECONNREFUSED: return "connection refused";
#endif
#ifdef ECONNRESET
	case ECONNRESET: return "connection reset by peer";
#endif
#if defined(EDEADLK) && (!defined(EWOULDBLOCK) || (EDEADLK != EWOULDBLOCK))
	case EDEADLK: return "resource deadlock avoided";
#endif
#ifdef EDEADLOCK
	case EDEADLOCK: return "resource deadlock avoided";
#endif
#ifdef EDESTADDRREQ
	case EDESTADDRREQ: return "destination address required";
#endif
#ifdef EDIRTY
	case EDIRTY: return "mounting a dirty fs w/o force";
#endif
#ifdef EDOM
	case EDOM: return "math argument out of range";
#endif
#ifdef EDOTDOT
	case EDOTDOT: return "cross mount point";
#endif
#ifdef EDQUOT
	case EDQUOT: return "disk quota exceeded";
#endif
#ifdef EDUPPKG
	case EDUPPKG: return "duplicate package name";
#endif
#ifdef EEXIST
	case EEXIST: return "file already exists";
#endif
#ifdef EFAULT
	case EFAULT: return "bad address in system call argument";
#endif
#ifdef EFBIG
	case EFBIG: return "file too large";
#endif
#ifdef EHOSTDOWN
	case EHOSTDOWN: return "host is down";
#endif
#ifdef EHOSTUNREACH
	case EHOSTUNREACH: return "host is unreachable";
#endif
#ifdef EIDRM
	case EIDRM: return "identifier removed";
#endif
#ifdef EINIT
	case EINIT: return "initialization error";
#endif
#ifdef EINPROGRESS
	case EINPROGRESS: return "operation now in progress";
#endif
#ifdef EINTR
	case EINTR: return "interrupted system call";
#endif
#ifdef EINVAL
	case EINVAL: return "invalid argument";
#endif
#ifdef EIO
	case EIO: return "I/O error";
#endif
#ifdef EISCONN
	case EISCONN: return "socket is already connected";
#endif
#ifdef EISDIR
	case EISDIR: return "illegal operation on a directory";
#endif
#ifdef EISNAME
	case EISNAM: return "is a name file";
#endif
#ifdef ELBIN
	case ELBIN: return "ELBIN";
#endif
#ifdef EL2HLT
	case EL2HLT: return "level 2 halted";
#endif
#ifdef EL2NSYNC
	case EL2NSYNC: return "level 2 not synchronized";
#endif
#ifdef EL3HLT
	case EL3HLT: return "level 3 halted";
#endif
#ifdef EL3RST
	case EL3RST: return "level 3 reset";
#endif
#ifdef ELIBACC
	case ELIBACC: return "can not access a needed shared library";
#endif
#ifdef ELIBBAD
	case ELIBBAD: return "accessing a corrupted shared library";
#endif
#ifdef ELIBEXEC
	case ELIBEXEC: return "can not exec a shared library directly";
#endif
#ifdef ELIBMAX
	case ELIBMAX: return
		"attempting to link in more shared libraries than system limit";
#endif
#ifdef ELIBSCN
	case ELIBSCN: return ".lib section in a.out corrupted";
#endif
#ifdef ELNRNG
	case ELNRNG: return "link number out of range";
#endif
#ifdef ELOOP
	case ELOOP: return "too many levels of symbolic links";
#endif
#ifdef EMFILE
	case EMFILE: return "too many open files";
#endif
#ifdef EMLINK
	case EMLINK: return "too many links";
#endif
#ifdef EMSGSIZE
	case EMSGSIZE: return "message too long";
#endif
#ifdef EMULTIHOP
	case EMULTIHOP: return "multihop attempted";
#endif
#ifdef ENAMETOOLONG
	case ENAMETOOLONG: return "file name too long";
#endif
#ifdef ENAVAIL
	case ENAVAIL: return "not available";
#endif
#ifdef ENET
	case ENET: return "ENET";
#endif
#ifdef ENETDOWN
	case ENETDOWN: return "network is down";
#endif
#ifdef ENETRESET
	case ENETRESET: return "network dropped connection on reset";
#endif
#ifdef ENETUNREACH
	case ENETUNREACH: return "network is unreachable";
#endif
#ifdef ENFILE
	case ENFILE: return "file table overflow";
#endif
#ifdef ENOANO
	case ENOANO: return "anode table overflow";
#endif
#if defined(ENOBUFS) && (!defined(ENOSR) || (ENOBUFS != ENOSR))
	case ENOBUFS: return "no buffer space available";
#endif
#ifdef ENOCSI
	case ENOCSI: return "no CSI structure available";
#endif
#ifdef ENODATA
	case ENODATA: return "no data available";
#endif
#ifdef ENODEV
	case ENODEV: return "no such device";
#endif
#ifdef ENOENT
	case ENOENT: return "no such file or directory";
#endif
#ifdef ENOEXEC
	case ENOEXEC: return "exec format error";
#endif
#ifdef ENOLCK
	case ENOLCK: return "no locks available";
#endif
#ifdef ENOLINK
	case ENOLINK: return "link has be severed";
#endif
#ifdef ENOMEM
	case ENOMEM: return "not enough memory";
#endif
#ifdef ENOMSG
	case ENOMSG: return "no message of desired type";
#endif
#ifdef ENONET
	case ENONET: return "machine is not on the network";
#endif
#ifdef ENOPKG
	case ENOPKG: return "package not installed";
#endif
#ifdef ENOPROTOOPT
	case ENOPROTOOPT: return "bad proocol option";
#endif
#ifdef ENOSPC
	case ENOSPC: return "no space left on device";
#endif
#ifdef ENOSR
	case ENOSR: return "out of stream resources";
#endif
#ifdef ENOSTR
	case ENOSTR: return "not a stream device";
#endif
#ifdef ENOSYM
	case ENOSYM: return "unresolved symbol name";
#endif
#ifdef ENOSYS
	case ENOSYS: return "function not implemented";
#endif
#ifdef ENOTBLK
	case ENOTBLK: return "block device required";
#endif
#ifdef ENOTCONN
	case ENOTCONN: return "socket is not connected";
#endif
#ifdef ENOTDIR
	case ENOTDIR: return "not a directory";
#endif
#ifdef ENOTEMPTY
	case ENOTEMPTY: return "directory not empty";
#endif
#ifdef ENOTNAM
	case ENOTNAM: return "not a name file";
#endif
#ifdef ENOTSOCK
	case ENOTSOCK: return "socket operation on non-socket";
#endif
#ifdef ENOTTY
	case ENOTTY: return "inappropriate device for ioctl";
#endif
#ifdef ENOTUNIQ
	case ENOTUNIQ: return "name not unique on network";
#endif
#ifdef ENXIO
	case ENXIO: return "no such device or address";
#endif
#ifdef EOPNOTSUPP
	case EOPNOTSUPP: return "operation not supported on socket";
#endif
#ifdef EPERM
	case EPERM: return "not owner";
#endif
#ifdef EPFNOSUPPORT
	case EPFNOSUPPORT: return "protocol family not supported";
#endif
#ifdef EPIPE
	case EPIPE: return "broken pipe";
#endif
#ifdef EPROCLIM
	case EPROCLIM: return "too many processes";
#endif
#ifdef EPROCUNAVAIL
	case EPROCUNAVAIL: return "bad procedure for program";
#endif
#ifdef EPROGMISMATCH
	case EPROGMISMATCH: return "program version wrong";
#endif
#ifdef EPROGUNAVAIL
	case EPROGUNAVAIL: return "RPC program not available";
#endif
#ifdef EPROTO
	case EPROTO: return "protocol error";
#endif
#ifdef EPROTONOSUPPORT
	case EPROTONOSUPPORT: return "protocol not suppored";
#endif
#ifdef EPROTOTYPE
	case EPROTOTYPE: return "protocol wrong type for socket";
#endif
#ifdef ERANGE
	case ERANGE: return "math result unrepresentable";
#endif
#if defined(EREFUSED) && (!defined(ECONNREFUSED) || (EREFUSED != ECONNREFUSED))
	case EREFUSED: return "EREFUSED";
#endif
#ifdef EREMCHG
	case EREMCHG: return "remote address changed";
#endif
#ifdef EREMDEV
	case EREMDEV: return "remote device";
#endif
#ifdef EREMOTE
	case EREMOTE: return "pathname hit remote file system";
#endif
#ifdef EREMOTEIO
	case EREMOTEIO: return "remote i/o error";
#endif
#ifdef EREMOTERELEASE
	case EREMOTERELEASE: return "EREMOTERELEASE";
#endif
#ifdef EROFS
	case EROFS: return "read-only file system";
#endif
#ifdef ERPCMISMATCH
	case ERPCMISMATCH: return "RPC version is wrong";
#endif
#ifdef ERREMOTE
	case ERREMOTE: return "object is remote";
#endif
#ifdef ESHUTDOWN
	case ESHUTDOWN: return "can't send afer socket shutdown";
#endif
#ifdef ESOCKTNOSUPPORT
	case ESOCKTNOSUPPORT: return "socket type not supported";
#endif
#ifdef ESPIPE
	case ESPIPE: return "invalid seek";
#endif
#ifdef ESRCH
	case ESRCH: return "no such process";
#endif
#ifdef ESRMNT
	case ESRMNT: return "srmount error";
#endif
#ifdef ESTALE
	case ESTALE: return "stale remote file handle";
#endif
#ifdef ESUCCESS
	case ESUCCESS: return "Error 0";
#endif
#ifdef ETIME
	case ETIME: return "timer expired";
#endif
#ifdef ETIMEDOUT
	case ETIMEDOUT: return "connection timed out";
#endif
#ifdef ETOOMANYREFS
	case ETOOMANYREFS: return "too many references: can't splice";
#endif
#ifdef ETXTBSY
	case ETXTBSY: return "text file or pseudo-device busy";
#endif
#ifdef EUCLEAN
	case EUCLEAN: return "structure needs cleaning";
#endif
#ifdef EUNATCH
	case EUNATCH: return "protocol driver not attached";
#endif
#ifdef EUSERS
	case EUSERS: return "too many users";
#endif
#ifdef EVERSION
	case EVERSION: return "version mismatch";
#endif
#if defined(EWOULDBLOCK) && (!defined(EAGAIN) || (EWOULDBLOCK != EAGAIN))
	case EWOULDBLOCK: return "operation would block";
#endif
#ifdef EXDEV
	case EXDEV: return "cross-domain link";
#endif
#ifdef EXFULL
	case EXFULL: return "message tables full";
#endif
    }
#endif /* ! TCL_SYS_ERRLIST */
    sprintf(msg, "unknown error (%d)", error);
    return msg;
}
