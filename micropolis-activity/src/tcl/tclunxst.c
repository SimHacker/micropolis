/* 
 * tclUnixStr.c --
 *
 *	This file contains procedures that generate strings
 *	corresponding to various UNIX-related codes, such
 *	as errno and signals.
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
static char rcsid[] = "$Header: /user6/ouster/tcl/RCS/tclUnixStr.c,v 1.12 92/04/30 15:51:15 ouster Exp $ SPRITE (Berkeley)";
#endif /* not lint */

#include "tclint.h"
#include "tclunix.h"

/*
 *----------------------------------------------------------------------
 *
 * Tcl_ErrnoId --
 *
 *	Return a textual identifier for the current errno value.
 *
 * Results:
 *	This procedure returns a machine-readable textual identifier
 *	that corresponds to the current errno value (e.g. "EPERM").
 *	The identifier is the same as the #define name in errno.h.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

char *
Tcl_ErrnoId()
{
    switch (errno) {
#ifdef E2BIG
	case E2BIG: return "E2BIG";
#endif
#ifdef EACCES
	case EACCES: return "EACCES";
#endif
#ifdef EADDRINUSE
	case EADDRINUSE: return "EADDRINUSE";
#endif
#ifdef EADDRNOTAVAIL
	case EADDRNOTAVAIL: return "EADDRNOTAVAIL";
#endif
#ifdef EADV
	case EADV: return "EADV";
#endif
#ifdef EAFNOSUPPORT
	case EAFNOSUPPORT: return "EAFNOSUPPORT";
#endif
#ifdef EAGAIN
	case EAGAIN: return "EAGAIN";
#endif
#ifdef EALIGN
	case EALIGN: return "EALIGN";
#endif
#ifdef EALREADY
	case EALREADY: return "EALREADY";
#endif
#ifdef EBADE
	case EBADE: return "EBADE";
#endif
#ifdef EBADF
	case EBADF: return "EBADF";
#endif
#ifdef EBADFD
	case EBADFD: return "EBADFD";
#endif
#ifdef EBADMSG
	case EBADMSG: return "EBADMSG";
#endif
#ifdef EBADR
	case EBADR: return "EBADR";
#endif
#ifdef EBADRPC
	case EBADRPC: return "EBADRPC";
#endif
#ifdef EBADRQC
	case EBADRQC: return "EBADRQC";
#endif
#ifdef EBADSLT
	case EBADSLT: return "EBADSLT";
#endif
#ifdef EBFONT
	case EBFONT: return "EBFONT";
#endif
#ifdef EBUSY
	case EBUSY: return "EBUSY";
#endif
#ifdef ECHILD
	case ECHILD: return "ECHILD";
#endif
#ifdef ECHRNG
	case ECHRNG: return "ECHRNG";
#endif
#ifdef ECOMM
	case ECOMM: return "ECOMM";
#endif
#ifdef ECONNABORTED
	case ECONNABORTED: return "ECONNABORTED";
#endif
#ifdef ECONNREFUSED
	case ECONNREFUSED: return "ECONNREFUSED";
#endif
#ifdef ECONNRESET
	case ECONNRESET: return "ECONNRESET";
#endif
#if defined(EDEADLK) && (!defined(EWOULDBLOCK) || (EDEADLK != EWOULDBLOCK))
	case EDEADLK: return "EDEADLK";
#endif
#ifndef IS_LINUX
#ifdef EDEADLOCK
	case EDEADLOCK: return "EDEADLOCK";
#endif
#endif
#ifdef EDESTADDRREQ
	case EDESTADDRREQ: return "EDESTADDRREQ";
#endif
#ifdef EDIRTY
	case EDIRTY: return "EDIRTY";
#endif
#ifdef EDOM
	case EDOM: return "EDOM";
#endif
#ifdef EDOTDOT
	case EDOTDOT: return "EDOTDOT";
#endif
#ifdef EDQUOT
	case EDQUOT: return "EDQUOT";
#endif
#ifdef EDUPPKG
	case EDUPPKG: return "EDUPPKG";
#endif
#ifdef EEXIST
	case EEXIST: return "EEXIST";
#endif
#ifdef EFAULT
	case EFAULT: return "EFAULT";
#endif
#ifdef EFBIG
	case EFBIG: return "EFBIG";
#endif
#ifdef EHOSTDOWN
	case EHOSTDOWN: return "EHOSTDOWN";
#endif
#ifdef EHOSTUNREACH
	case EHOSTUNREACH: return "EHOSTUNREACH";
#endif
#ifdef EIDRM
	case EIDRM: return "EIDRM";
#endif
#ifdef EINIT
	case EINIT: return "EINIT";
#endif
#ifdef EINPROGRESS
	case EINPROGRESS: return "EINPROGRESS";
#endif
#ifdef EINTR
	case EINTR: return "EINTR";
#endif
#ifdef EINVAL
	case EINVAL: return "EINVAL";
#endif
#ifdef EIO
	case EIO: return "EIO";
#endif
#ifdef EISCONN
	case EISCONN: return "EISCONN";
#endif
#ifdef EISDIR
	case EISDIR: return "EISDIR";
#endif
#ifdef EISNAME
	case EISNAM: return "EISNAM";
#endif
#ifdef ELBIN
	case ELBIN: return "ELBIN";
#endif
#ifdef EL2HLT
	case EL2HLT: return "EL2HLT";
#endif
#ifdef EL2NSYNC
	case EL2NSYNC: return "EL2NSYNC";
#endif
#ifdef EL3HLT
	case EL3HLT: return "EL3HLT";
#endif
#ifdef EL3RST
	case EL3RST: return "EL3RST";
#endif
#ifdef ELIBACC
	case ELIBACC: return "ELIBACC";
#endif
#ifdef ELIBBAD
	case ELIBBAD: return "ELIBBAD";
#endif
#ifdef ELIBEXEC
	case ELIBEXEC: return "ELIBEXEC";
#endif
#ifdef ELIBMAX
	case ELIBMAX: return "ELIBMAX";
#endif
#ifdef ELIBSCN
	case ELIBSCN: return "ELIBSCN";
#endif
#ifdef ELNRNG
	case ELNRNG: return "ELNRNG";
#endif
#ifdef ELOOP
	case ELOOP: return "ELOOP";
#endif
#ifdef EMFILE
	case EMFILE: return "EMFILE";
#endif
#ifdef EMLINK
	case EMLINK: return "EMLINK";
#endif
#ifdef EMSGSIZE
	case EMSGSIZE: return "EMSGSIZE";
#endif
#ifdef EMULTIHOP
	case EMULTIHOP: return "EMULTIHOP";
#endif
#ifdef ENAMETOOLONG
	case ENAMETOOLONG: return "ENAMETOOLONG";
#endif
#ifdef ENAVAIL
	case ENAVAIL: return "ENAVAIL";
#endif
#ifdef ENET
	case ENET: return "ENET";
#endif
#ifdef ENETDOWN
	case ENETDOWN: return "ENETDOWN";
#endif
#ifdef ENETRESET
	case ENETRESET: return "ENETRESET";
#endif
#ifdef ENETUNREACH
	case ENETUNREACH: return "ENETUNREACH";
#endif
#ifdef ENFILE
	case ENFILE: return "ENFILE";
#endif
#ifdef ENOANO
	case ENOANO: return "ENOANO";
#endif
#if defined(ENOBUFS) && (!defined(ENOSR) || (ENOBUFS != ENOSR))
	case ENOBUFS: return "ENOBUFS";
#endif
#ifdef ENOCSI
	case ENOCSI: return "ENOCSI";
#endif
#ifdef ENODATA
	case ENODATA: return "ENODATA";
#endif
#ifdef ENODEV
	case ENODEV: return "ENODEV";
#endif
#ifdef ENOENT
	case ENOENT: return "ENOENT";
#endif
#ifdef ENOEXEC
	case ENOEXEC: return "ENOEXEC";
#endif
#ifdef ENOLCK
	case ENOLCK: return "ENOLCK";
#endif
#ifdef ENOLINK
	case ENOLINK: return "ENOLINK";
#endif
#ifdef ENOMEM
	case ENOMEM: return "ENOMEM";
#endif
#ifdef ENOMSG
	case ENOMSG: return "ENOMSG";
#endif
#ifdef ENONET
	case ENONET: return "ENONET";
#endif
#ifdef ENOPKG
	case ENOPKG: return "ENOPKG";
#endif
#ifdef ENOPROTOOPT
	case ENOPROTOOPT: return "ENOPROTOOPT";
#endif
#ifdef ENOSPC
	case ENOSPC: return "ENOSPC";
#endif
#ifdef ENOSR
	case ENOSR: return "ENOSR";
#endif
#ifdef ENOSTR
	case ENOSTR: return "ENOSTR";
#endif
#ifdef ENOSYM
	case ENOSYM: return "ENOSYM";
#endif
#ifdef ENOSYS
	case ENOSYS: return "ENOSYS";
#endif
#ifdef ENOTBLK
	case ENOTBLK: return "ENOTBLK";
#endif
#ifdef ENOTCONN
	case ENOTCONN: return "ENOTCONN";
#endif
#ifdef ENOTDIR
	case ENOTDIR: return "ENOTDIR";
#endif
#if defined(ENOTEMPTY) && (!defined(EEXIST) || (ENOTEMPTY != EEXIST))
	case ENOTEMPTY: return "ENOTEMPTY";
#endif
#ifdef ENOTNAM
	case ENOTNAM: return "ENOTNAM";
#endif
#ifdef ENOTSOCK
	case ENOTSOCK: return "ENOTSOCK";
#endif
#ifdef ENOTTY
	case ENOTTY: return "ENOTTY";
#endif
#ifdef ENOTUNIQ
	case ENOTUNIQ: return "ENOTUNIQ";
#endif
#ifndef __GNUC__
#ifdef ENXIO
	case ENXIO: return "ENXIO";
#endif
#endif
#ifdef EOPNOTSUPP
	case EOPNOTSUPP: return "EOPNOTSUPP";
#endif
#ifndef __GNUC__
#ifdef EPERM
	case EPERM: return "EPERM";
#endif
#endif
#ifdef EPFNOSUPPORT
	case EPFNOSUPPORT: return "EPFNOSUPPORT";
#endif
#ifdef EPIPE
	case EPIPE: return "EPIPE";
#endif
#ifdef EPROCLIM
	case EPROCLIM: return "EPROCLIM";
#endif
#ifdef EPROCUNAVAIL
	case EPROCUNAVAIL: return "EPROCUNAVAIL";
#endif
#ifdef EPROGMISMATCH
	case EPROGMISMATCH: return "EPROGMISMATCH";
#endif
#ifdef EPROGUNAVAIL
	case EPROGUNAVAIL: return "EPROGUNAVAIL";
#endif
#ifdef EPROTO
	case EPROTO: return "EPROTO";
#endif
#ifdef EPROTONOSUPPORT
	case EPROTONOSUPPORT: return "EPROTONOSUPPORT";
#endif
#ifdef EPROTOTYPE
	case EPROTOTYPE: return "EPROTOTYPE";
#endif
#ifdef ERANGE
	case ERANGE: return "ERANGE";
#endif
#if defined(EREFUSED) && (!defined(ECONNREFUSED) || (EREFUSED != ECONNREFUSED))
	case EREFUSED: return "EREFUSED";
#endif
#ifdef EREMCHG
	case EREMCHG: return "EREMCHG";
#endif
#ifdef EREMDEV
	case EREMDEV: return "EREMDEV";
#endif
#ifdef EREMOTE
	case EREMOTE: return "EREMOTE";
#endif
#ifdef EREMOTEIO
	case EREMOTEIO: return "EREMOTEIO";
#endif
#ifdef EREMOTERELEASE
	case EREMOTERELEASE: return "EREMOTERELEASE";
#endif
#ifdef EROFS
	case EROFS: return "EROFS";
#endif
#ifdef ERPCMISMATCH
	case ERPCMISMATCH: return "ERPCMISMATCH";
#endif
#ifdef ERREMOTE
	case ERREMOTE: return "ERREMOTE";
#endif
#ifdef ESHUTDOWN
	case ESHUTDOWN: return "ESHUTDOWN";
#endif
#ifdef ESOCKTNOSUPPORT
	case ESOCKTNOSUPPORT: return "ESOCKTNOSUPPORT";
#endif
#ifdef ESPIPE
	case ESPIPE: return "ESPIPE";
#endif
#ifdef ESRCH
	case ESRCH: return "ESRCH";
#endif
#ifdef ESRMNT
	case ESRMNT: return "ESRMNT";
#endif
#ifdef ESTALE
	case ESTALE: return "ESTALE";
#endif
#ifdef ESUCCESS
	case ESUCCESS: return "ESUCCESS";
#endif
#ifdef ETIME
	case ETIME: return "ETIME";
#endif
#ifdef ETIMEDOUT
	case ETIMEDOUT: return "ETIMEDOUT";
#endif
#ifdef ETOOMANYREFS
	case ETOOMANYREFS: return "ETOOMANYREFS";
#endif
#ifdef ETXTBSY
	case ETXTBSY: return "ETXTBSY";
#endif
#ifdef EUCLEAN
	case EUCLEAN: return "EUCLEAN";
#endif
#ifdef EUNATCH
	case EUNATCH: return "EUNATCH";
#endif
#ifdef EUSERS
	case EUSERS: return "EUSERS";
#endif
#ifdef EVERSION
	case EVERSION: return "EVERSION";
#endif
#if defined(EWOULDBLOCK) && (!defined(EAGAIN) || (EWOULDBLOCK != EAGAIN))
	case EWOULDBLOCK: return "EWOULDBLOCK";
#endif
#ifdef EXDEV
	case EXDEV: return "EXDEV";
#endif
#ifdef EXFULL
	case EXFULL: return "EXFULL";
#endif
    }
    return "unknown error";
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_SignalId --
 *
 *	Return a textual identifier for a signal number.
 *
 * Results:
 *	This procedure returns a machine-readable textual identifier
 *	that corresponds to sig.  The identifier is the same as the
 *	#define name in signal.h.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

char *
Tcl_SignalId(sig)
    int sig;			/* Number of signal. */
{
    switch (sig) {
#ifdef SIGABRT
	case SIGABRT: return "SIGABRT";
#endif
#ifdef SIGALRM
	case SIGALRM: return "SIGALRM";
#endif
#ifdef SIGBUS
	case SIGBUS: return "SIGBUS";
#endif
#ifdef SIGCHLD
	case SIGCHLD: return "SIGCHLD";
#endif
#if defined(SIGCLD) && (!defined(SIGCHLD) || (SIGCLD != SIGCHLD))
	case SIGCLD: return "SIGCLD";
#endif
#ifdef SIGCONT
	case SIGCONT: return "SIGCONT";
#endif
#if defined(SIGEMT) && (!defined(SIGXCPU) || (SIGEMT != SIGXCPU))
	case SIGEMT: return "SIGEMT";
#endif
#ifdef SIGFPE
	case SIGFPE: return "SIGFPE";
#endif
#ifdef SIGHUP
	case SIGHUP: return "SIGHUP";
#endif
#ifdef SIGILL
	case SIGILL: return "SIGILL";
#endif
#ifdef SIGINT
	case SIGINT: return "SIGINT";
#endif
#ifdef SIGIO
	case SIGIO: return "SIGIO";
#endif
#if defined(SIGIOT) && (!defined(SIGABRT) || (SIGIOT != SIGABRT))
	case SIGIOT: return "SIGIOT";
#endif
#ifdef SIGKILL
	case SIGKILL: return "SIGKILL";
#endif
#if defined(SIGLOST) && (!defined(SIGIOT) || (SIGLOST != SIGIOT))
	case SIGLOST: return "SIGLOST";
#endif
#ifdef SIGPIPE
	case SIGPIPE: return "SIGPIPE";
#endif
#if defined(SIGPOLL) && (!defined(SIGIO) || (SIGPOLL != SIGIO))
	case SIGPOLL: return "SIGPOLL";
#endif
#ifdef SIGPROF
	case SIGPROF: return "SIGPROF";
#endif
#if defined(SIGPWR) && (!defined(SIGXFSZ) || (SIGPWR != SIGXFSZ))
	case SIGPWR: return "SIGPWR";
#endif
#ifdef SIGQUIT
	case SIGQUIT: return "SIGQUIT";
#endif
#ifdef SIGSEGV
	case SIGSEGV: return "SIGSEGV";
#endif
#ifdef SIGSTOP
	case SIGSTOP: return "SIGSTOP";
#endif
#ifdef SIGSYS
	case SIGSYS: return "SIGSYS";
#endif
#ifdef SIGTERM
	case SIGTERM: return "SIGTERM";
#endif
#ifdef SIGTRAP
	case SIGTRAP: return "SIGTRAP";
#endif
#ifdef SIGTSTP
	case SIGTSTP: return "SIGTSTP";
#endif
#ifdef SIGTTIN
	case SIGTTIN: return "SIGTTIN";
#endif
#ifdef SIGTTOU
	case SIGTTOU: return "SIGTTOU";
#endif
#ifdef SIGURG
	case SIGURG: return "SIGURG";
#endif
#ifdef SIGUSR1
	case SIGUSR1: return "SIGUSR1";
#endif
#ifdef SIGUSR2
	case SIGUSR2: return "SIGUSR2";
#endif
#ifdef SIGVTALRM
	case SIGVTALRM: return "SIGVTALRM";
#endif
#ifdef SIGWINCH
	case SIGWINCH: return "SIGWINCH";
#endif
#ifdef SIGXCPU
	case SIGXCPU: return "SIGXCPU";
#endif
#ifdef SIGXFSZ
	case SIGXFSZ: return "SIGXFSZ";
#endif
    }
    return "unknown signal";
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_SignalMsg --
 *
 *	Return a human-readable message describing a signal.
 *
 * Results:
 *	This procedure returns a string describing sig that should
 *	make sense to a human.  It may not be easy for a machine
 *	to parse.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

char *
Tcl_SignalMsg(sig)
    int sig;			/* Number of signal. */
{
    switch (sig) {
#ifdef SIGABRT
	case SIGABRT: return "SIGABRT";
#endif
#ifdef SIGALRM
	case SIGALRM: return "alarm clock";
#endif
#ifdef SIGBUS
	case SIGBUS: return "bus error";
#endif
#ifdef SIGCHLD
	case SIGCHLD: return "child status changed";
#endif
#if defined(SIGCLD) && (!defined(SIGCHLD) || (SIGCLD != SIGCHLD))
	case SIGCLD: return "child status changed";
#endif
#ifdef SIGCONT
	case SIGCONT: return "continue after stop";
#endif
#if defined(SIGEMT) && (!defined(SIGXCPU) || (SIGEMT != SIGXCPU))
	case SIGEMT: return "EMT instruction";
#endif
#ifdef SIGFPE
	case SIGFPE: return "floating-point exception";
#endif
#ifdef SIGHUP
	case SIGHUP: return "hangup";
#endif
#ifdef SIGILL
	case SIGILL: return "illegal instruction";
#endif
#ifdef SIGINT
	case SIGINT: return "interrupt";
#endif
#ifdef SIGIO
	case SIGIO: return "input/output possible on file";
#endif
#if defined(SIGIOT) && (!defined(SIGABRT) || (SIGABRT != SIGIOT))
	case SIGIOT: return "IOT instruction";
#endif
#ifdef SIGKILL
	case SIGKILL: return "kill signal";
#endif
#if defined(SIGLOST) && (!defined(SIGIOT) || (SIGLOST != SIGIOT))
	case SIGLOST: return "resource lost";
#endif
#ifdef SIGPIPE
	case SIGPIPE: return "write on pipe with no readers";
#endif
#if defined(SIGPOLL) && (!defined(SIGIO) || (SIGPOLL != SIGIO))
	case SIGPOLL: return "input/output possible on file";
#endif
#ifdef SIGPROF
	case SIGPROF: return "profiling alarm";
#endif
#if defined(SIGPWR) && (!defined(SIGXFSZ) || (SIGPWR != SIGXFSZ))
	case SIGPWR: return "power-fail restart";
#endif
#ifdef SIGQUIT
	case SIGQUIT: return "quit signal";
#endif
#ifdef SIGSEGV
	case SIGSEGV: return "segmentation violation";
#endif
#ifdef SIGSTOP
	case SIGSTOP: return "stop";
#endif
#ifdef SIGSYS
	case SIGSYS: return "bad argument to system call";
#endif
#ifdef SIGTERM
	case SIGTERM: return "software termination signal";
#endif
#ifdef SIGTRAP
	case SIGTRAP: return "trace trap";
#endif
#ifdef SIGTSTP
	case SIGTSTP: return "stop signal from tty";
#endif
#ifdef SIGTTIN
	case SIGTTIN: return "background tty read";
#endif
#ifdef SIGTTOU
	case SIGTTOU: return "background tty write";
#endif
#ifdef SIGURG
	case SIGURG: return "urgent I/O condition";
#endif
#ifdef SIGUSR1
	case SIGUSR1: return "user-defined signal 1";
#endif
#ifdef SIGUSR2
	case SIGUSR2: return "user-defined signal 2";
#endif
#ifdef SIGVTALRM
	case SIGVTALRM: return "virtual time alarm";
#endif
#ifdef SIGWINCH
	case SIGWINCH: return "window changed";
#endif
#ifdef SIGXCPU
	case SIGXCPU: return "exceeded CPU time limit";
#endif
#ifdef SIGXFSZ
	case SIGXFSZ: return "exceeded file size limit";
#endif
    }
    return "unknown signal";
}
