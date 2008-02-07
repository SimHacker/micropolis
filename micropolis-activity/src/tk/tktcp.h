/* tkTCP.h --
 *
 *	This file provides declarations needed by the TCP server
 *	implementation for Tcl interpreters.
 *
 * $Header: /cluster21/kennykb/src/tclTCP.1.0beta/RCS/tclTCP.h,v 1.1 1992/02/14 19:57:51 kennykb Exp $
 * $Source: /cluster21/kennykb/src/tclTCP.1.0beta/RCS/tclTCP.h,v $
 * $Log: tclTCP.h,v $
 * Revision 1.1  1992/02/14  19:57:51  kennykb
 * Initial revision
 *
 *
 *
 * 	Copyright (C) 1992 General Electric.
 *	All rights reserved.
 *	
 *
 *   Permission to use, copy, modify, and distribute this
 *   software and its documentation for any purpose and without
 *   fee is hereby granted, provided that the above copyright
 *   notice appear in all copies and that both that copyright
 *   notice and this permission notice appear in supporting
 *   documentation, and that the name of General Electric not be used in
 *   advertising or publicity pertaining to distribution of the
 *   software without specific, written prior permission.
 *   General Electric makes no representations about the suitability of
 *   this software for any purpose.  It is provided "as is"
 *   without express or implied warranty.
 *
 *   This work was supported by the DARPA Initiative in Concurrent
 *   Engineering (DICE) through DARPA Contract MDA972-88-C-0047.
 *
 */

#ifndef _TCLTCP
#define _TCLTCP 1

#ifndef _TCL
#include "tcl.h"
#endif

/* Definitions of the functions */

EXTERN int tcpCommand _ANSI_ARGS_((ClientData clientData,
				   Tcl_Interp * interp,
				   int argc,
				   char * * argv));

extern void simpleCreateFileHandler _ANSI_ARGS_((int fd,
						 int mask,
						 Tk_FileProc * proc,
						 ClientData clientData));

extern void simpleDeleteFileHandler _ANSI_ARGS_((int fd));

extern void simpleReportBackgroundError _ANSI_ARGS_((Tcl_Interp * interp));

#endif /* _TCLTCP */
