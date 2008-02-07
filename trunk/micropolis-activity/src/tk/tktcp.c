/* tkTCP.c --
 *
 * 	This file provides basic capabilities to establish a server,
 * attached to a TCP/IP port, that attaches to a Tcl interpreter.
 * Such servers provide a remote-procedure-call mechanism for Tcl
 * without needing to resort to Tk's X-window-based complexities, and
 * also allow access to services that are not bound to any particular
 * display.
 */

static char RCSid [] =
  "$Header: /cluster21/kennykb/src/tclTCP.1.0beta/RCS/tclTCP.c,v 1.7 1992/05/05 18:31:13 kennykb Exp kennykb $";
/* $Source: /cluster21/kennykb/src/tclTCP.1.0beta/RCS/tclTCP.c,v $
 * $Log: tclTCP.c,v $
 * Revision 1.7  1992/05/05  18:31:13  kennykb
 * Changed the flow through the `delete server' code to make it work even
 * if a server is deleted while a client is active.
 * The change avoids aborts at termination time if the server delete code
 * is reached before the application exits.
 *
 * Revision 1.6  1992/03/04  20:04:00  kennykb
 * Modified source code to use the Tcl configurator and corresponding include
 * files.
 *
 * Revision 1.5  1992/02/25  15:21:30  kennykb
 * Modifications to quiet warnings from gcc
 * ,
 *
 * Revision 1.4  1992/02/24  19:30:30  kennykb
 * Merged branches (a) updated tcpTrustedHost and (b) broken-out event mgr.
 *
 * Revision 1.3  1992/02/20  16:22:53  kennykb
 * Event management code removed and broken out into a separate file,
 * simpleEvent.c
 *
 * Revision 1.2.1.1  1992/02/24  19:12:30  kennykb
 * Rewrote tcpTrustedHost to be more forgiving of unusual configurations.
 * It now looks through all aliases for the local host and the loopback
 * pseudo-host.
 *
 * Revision 1.2  1992/02/18  14:43:21  kennykb
 * Fix for bug 920218.1 in `History' file.
 *
 * Revision 1.1  1992/02/14  19:57:51  kennykb
 * Initial revision
 *
 */

static char copyright [] = 
  "Copyright (C) 1992 General Electric. All rights reserved."		;

/*
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
 */

#include <errno.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

/* Only some copies of netinet/in.h have the following defined. */

#ifndef INADDR_LOOPBACK
#ifdef __STDC__
#define INADDR_LOOPBACK 0x7f000001UL
#else
#define INADDR_LOOPBACK (unsigned long) 0x7f000001L
#endif /* __STDC__ */
#endif /* INADDR_LOOPBACK */

#include <signal.h>

#include <tclInt.h>
#include <tclUnix.h>
#include <tk.h>
#include "tkTCP.h"

/* Kernel calls */

/* There doesn't seem to be any place to get these....
 * certainly not a portable one.
 */

extern int accept _ANSI_ARGS_((int, struct sockaddr *, int *));
extern int bind _ANSI_ARGS_((int, const struct sockaddr *, int));
extern int close _ANSI_ARGS_((int));
extern int connect _ANSI_ARGS_((int, const struct sockaddr *, int));
extern int gethostname _ANSI_ARGS_((char *, int));
extern int getsockname _ANSI_ARGS_((int, struct sockaddr *, int *));
extern int ioctl _ANSI_ARGS_((int, int, char *));
extern int listen _ANSI_ARGS_((int, int));
extern int read _ANSI_ARGS_((int, char *, int));
extern int select _ANSI_ARGS_((int, fd_set *, fd_set *, fd_set *,
			       struct timeval *));
extern int socket _ANSI_ARGS_((int, int, int));
extern int getdtablesize _ANSI_ARGS_((void));


/* Configuration parameters */

/*
 * TCP_LISTEN_BACKLOG gives the maximum backlog of connection requests
 * that may be queued for any server
 */

#define TCP_LISTEN_BACKLOG 3

/* Internal data structures */

/*
 * For each server that is established in any interpreter, there's a
 * record of the following type.  Note that only one server may be
 * running at a time in any interpreter, unless the Tk services are
 * available for event management.
 */

typedef struct tcp_ServerData {
  Tcl_Interp *		interp;	/* Interpreter in which connections */
				/* are processed. */
  char			name[ 16 ];
				/* Name of the server object. */
  int			socketfd;
				/* Filedescriptor of the socket at */
				/* which the server listens for connections */
  char *		command;
				/* Command to be executed (using */
				/* Tcl_Eval) when a connection request */
				/* arrives. */
  Tcl_FreeProc *	freeCommand;
				/* Procedure to free the command when */
				/* it's no longer needed. */
  int			stopFlag;
				/* Flag == TRUE if the server is trying */
  				/* to shut down. */
  int			raw;	/* Flag == TRUE if for raw socket mode. */
  struct tcp_ClientData * firstClient;
				/* First in the list of clients at this */
				/* server */
  struct tcp_ServerData * next, * prev;
				/* Linkage in the list of all active servers */
} Tcp_ServerData;

/*
 * Each client of a server will have a record of the following type.
 */

typedef struct tcp_ClientData {
  struct tcp_ServerData * server;
				/* Server to which the client belongs */
  char			name [16];
				/* Name of the client */
  int			socketfd;
				/* Filedescriptor of the socket of the */
				/* the client's connection. */
  struct sockaddr_in	peeraddr;
				/* IP address from which the client */
				/* established the connection. */
  char *		command;
				/* Command to execute when the client */
				/* sends a message */
  Tcl_FreeProc *	freeCommand;
				/* Procedure to free the command when it's */
				/* no longer needed */
  Tcl_CmdBuf		inputBuffer;
				/* Buffer where client commands are stored */
  char *		resultString;
				/* Result of executing a command on the */
				/* client */
  char *		resultPointer;
				/* Pointer to the portion of resultString */
				/* that remains to be transmitted back */
				/* to the client */
  Tcl_FreeProc *	freeResultString;
				/* Procedure to free the result string when */
				/* it's no longer needed. */
  int			activeFlag;
				/* Flag == 1 iff a command is pending on */
				/* this client. */
  int			closeFlag;
				/* Flag == 1 if the client should be closed */
				/* once its result has been returned. */
  struct tcp_ClientData *next, *prev;
				/* Next and previous entries in the list of */
				/* clients at this server */
} Tcp_ClientData;

/* Static variables in this file */

static char * tcpCurrentClient = NULL;
				/* The name of the client for which a */
				/* command is being processed. */
static Tcp_ServerData * tcpFirstServer = NULL;
				/* Pointer to the first in a list of */
				/* servers active in the current process. */

/* Declarations for static functions within this file. */

/* Static procedures in this file */

static void simpleDeleteFileHandler1 _ANSI_ARGS_((ClientData, int));

static void simpleDeleteFileHandler2 _ANSI_ARGS_((ClientData));

static int
tcpClientCommand _ANSI_ARGS_((ClientData clientData, Tcl_Interp * interp,
			      int argc, char * * argv));

static int
tcpConnectCommand _ANSI_ARGS_((ClientData clientData, Tcl_Interp * interp,
			      int argc, char * * argv));

static int
tcpEvalCommand _ANSI_ARGS_((ClientData clientData, Tcl_Interp * interp,
			    int argc, char * * argv));

static int
tcpLoginCommand _ANSI_ARGS_((ClientData clientData, Tcl_Interp * interp,
			     int argc, char * * argv));

static int
tcpMainLoopCommand _ANSI_ARGS_((ClientData clientData, Tcl_Interp * interp,
				int argc, char * * argv));

static int
tcpPollCommand _ANSI_ARGS_((ClientData clientData, Tcl_Interp * interp,
			    int argc, char * * argv));

static int
tcpServerCommand _ANSI_ARGS_((ClientData clientData, Tcl_Interp * interp,
			      int argc, char * * argv));

static int
tcpServersCommand _ANSI_ARGS_((ClientData clientData, Tcl_Interp * interp,
			      int argc, char * * argv));
static int
tcpWaitCommand _ANSI_ARGS_((ClientData clientData, Tcl_Interp * interp,
			     int argc, char * * argv));

static int
tcpServerObjectCmd _ANSI_ARGS_((ClientData clientData, Tcl_Interp * interp,
				int argc, char * * argv));
static void
deleteTcpServerObjectCmd _ANSI_ARGS_((ClientData clientData));

static int
tcpServerObjectAcceptCmd _ANSI_ARGS_((ClientData clientData,
				      Tcl_Interp * interp, int argc,
				      char * * argv));

static int
tcpServerObjectClientsCmd _ANSI_ARGS_((ClientData clientData,
				       Tcl_Interp * interp, int argc,
				       char * * argv));

static int
tcpServerObjectConfigCmd _ANSI_ARGS_((ClientData clientData,
				      Tcl_Interp * interp, int argc,
				      char * * argv));

static int
tcpServerObjectStartCmd _ANSI_ARGS_((ClientData clientData,
				     Tcl_Interp * interp, int argc,
				     char * * argv));

static int
tcpServerObjectStopCmd _ANSI_ARGS_((ClientData clientData,
				    Tcl_Interp * interp, int argc,
				    char * * argv));

static void
tcpDeleteServer _ANSI_ARGS_((Tcp_ServerData * server));

static int
tcpServerObjectConfig _ANSI_ARGS_((ClientData clientData, Tcl_Interp * interp,
				   int argc, char * * argv));

static int
tcpClientObjectCmd _ANSI_ARGS_((ClientData clientData, Tcl_Interp * interp,
				int argc, char * * argv));

static int
tcpClientObjectCloseCmd _ANSI_ARGS_((ClientData clientData,
				     Tcl_Interp * interp,
				     int argc, char * * argv));

static int
tcpClientObjectCommandCmd _ANSI_ARGS_((ClientData clientData,
				       Tcl_Interp * interp,
				       int argc, char * * argv));

static int
tcpClientObjectDoCmd _ANSI_ARGS_((ClientData clientData, Tcl_Interp * interp,
				  int argc, char * * argv));

static int
tcpClientObjectHostnameCmd _ANSI_ARGS_((ClientData clientData,
					Tcl_Interp * interp,
					int argc, char * * argv));

static int
tcpClientObjectServerCmd _ANSI_ARGS_((ClientData clientData,
				      Tcl_Interp * interp,
				      int argc, char * * argv));

static void
deleteTcpClientObjectCmd _ANSI_ARGS_((ClientData clientData));

static int
tcpConnectionObjectCmd _ANSI_ARGS_((ClientData clientData,
				    Tcl_Interp * interp,
				    int argc, char * * argv));

static int
tcpConnectionObjectCloseCmd _ANSI_ARGS_((ClientData clientData,
					 Tcl_Interp * interp,
					 int argc, char * * argv));

static int
tcpConnectionObjectSendCmd _ANSI_ARGS_((ClientData clientData,
					Tcl_Interp * interp,
					int argc, char * * argv));

static void
deleteTcpConnectionObjectCmd _ANSI_ARGS_((ClientData clientData));

static void
tcpServerAcceptConnection _ANSI_ARGS_((ClientData clientData, int mask));

static void
tcpReturnResultToClient _ANSI_ARGS_((Tcp_ClientData * client,
				     Tcl_Interp * interp,
				     int status, int closeflag));

static void
tcpWriteResultToClient _ANSI_ARGS_((ClientData clientData, int mask));

static void
tcpClientReadError _ANSI_ARGS_((Tcp_ClientData * client));

static void
tcpClientWriteError _ANSI_ARGS_((Tcp_ClientData * client));

static void
tcpPrepareClientForInput _ANSI_ARGS_((Tcp_ClientData * client));

static void
tcpReceiveClientInput _ANSI_ARGS_((ClientData clientData, int mask));

static void
tcpCloseClient _ANSI_ARGS_((Tcp_ClientData * client));

static int
tcpTrustedHost _ANSI_ARGS_((char * hostname));

static int
tcpSendCmdToServer _ANSI_ARGS_((Tcl_Interp * interp, int s, char * message));

static int
tcpReceiveResultFromServer _ANSI_ARGS_((Tcl_Interp * interp, int s));

/*
 * simpleReportBackgroundError --
 *
 *	This procedure is invoked to report a Tcl error in the background,
 * when TCL_ERROR has been passed out to the outermost level.
 *
 *	It tries to run `bgerror' giving it  the error message.  If this
 * fails, it reports the problem on stderr.
 */

void
simpleReportBackgroundError (interp)
     Tcl_Interp * interp;
{

  char *argv[2];
  char *command;
  char *error;
  char *errorInfo, *tmp;
  int status;
  int unixStatus;

  /* Get the error message out of the interpreter. */

  error = (char *) ckalloc (strlen (interp -> result) + 1);
  strcpy (error, interp -> result);

  /* Get errorInfo, too */

  tmp = Tcl_GetVar (interp, "errorInfo", TCL_GLOBAL_ONLY);
  if (tmp == NULL) {
    errorInfo = error;
  } else {
    errorInfo = (char *) ckalloc (strlen (tmp) + 1);
    strcpy (errorInfo, tmp);
  }

  /* Build a `bgerror' command to report the error */

  argv[0] = "bgerror";
  argv[1] = error;
  command = Tcl_Merge (2, argv);

  /* Try to run the command */

  status = Tcl_Eval (interp, command, 0, (char **) NULL);

  if (status != TCL_OK) {

    /* Command failed.  Report the problem to stderr. */

    tmp = Tcl_GetVar (interp, "errorInfo", TCL_GLOBAL_ONLY);
    if (tmp == NULL) {
      tmp = interp -> result;
    }
    unixStatus = fprintf (stderr, "\n\
------------------------------------------------------------------------\n\
Tcl interpreter detected a background error.\n\
Original error:\n\
%s\n\
\n\
User \"bgerror\" procedure failed to handle the background error.\n\
Error in bgerror:\n\
%s\n",
	     errorInfo, tmp);
    if (unixStatus < 0) {
      abort ();
    }
  }

  Tcl_ResetResult (interp);

  free (command);

  ckfree (error);

  if (errorInfo != error) {
    ckfree (errorInfo);
  }
}

/*
 * simpleCreateFileHandler --
 *
 *	This procedure is invoked to create a handle to cause a callback
 *	whenever a condition (readable, writable, exception) is
 *	present on a given file.
 *
 *	In the Tk environment, the file handler is created using Tk's
 *	Tk_CreateFileHandler procedure, and the callback takes place
 *	from the Tk main loop.  In a non-Tk environment, a
 *	Tcp_FileHandler structure is created to describe the file, and
 *	this structure is linked to a chain of such structures
 *	processed by the server main loop.
 */

void
simpleCreateFileHandler (fd, mask, proc, clientData)
     int fd;
     int mask;
     Tk_FileProc * proc;
     ClientData clientData;
{
  Tk_CreateFileHandler (fd, mask, (Tk_FileProc *) proc, clientData);

  /* It is possible that we have a file handler scheduled for deletion.
   * This deletion has to be cancelled if we've requested creation of
   * another one.
   */

  Tk_CancelIdleCall ((Tk_IdleProc *) simpleDeleteFileHandler2,
		     (ClientData) fd);
}

/*
 * simpleDeleteFileHandler --
 *
 *	This function is invoked when the program is no longer interested in
 * handling events on a file.  It removes any outstanding handler on the file.
 *
 *	The function is a little tricky because a file handler on the file may
 * be active.  In a non-Tk environment, this is simple; the SIMPLE_DELETE flag
 * is set in the handler's mask, and the main loop deletes the handler once
 * it is quiescent.  In Tk, the event loop won't do that, so what we do
 * is set a DoWhenIdle to delete the handler and return.  The DoWhenIdle
 * gets called back from the Tk event loop at a time that the handler is
 * quiescent, and deletes the handler.
 */

void
simpleDeleteFileHandler (fd)
     int fd;
{
  /* First of all, we have to zero the file's mask to avoid calling the same
     handler over again if the file is still ready. */
  Tk_CreateFileHandler (fd, 0, (Tk_FileProc *) simpleDeleteFileHandler1,
			(ClientData) NULL);
  Tk_DoWhenIdle ((Tk_IdleProc *) simpleDeleteFileHandler2,
		 (ClientData) fd);
}


/* ARGSUSED */
static void
simpleDeleteFileHandler1 (clientData, mask)
     ClientData clientData;
     int mask;
{
  (void) fprintf (stderr, "in simpleDeleteFileHandler1: bug in tkEvent.c");
  abort ();
}

static void
simpleDeleteFileHandler2 (clientData)
     ClientData clientData;
{
  int fd = (int) clientData;

  Tk_DeleteFileHandler (fd);
}

/*
 *----------------------------------------------------------------------
 * Tk_TcpCmd:
 *
 * This procedure implements a `tcp' command for Tcl.  It provides the
 * top-level actions for TCP/IP connections.
 *
 * This command is divided into variants, each with its own procedure:
 *
 *	tcp client
 *		Returns the current active client, or an error if there is
 *		none.
 *	tcp connect host port
 *		Establish a connection to a server running at `port' on
 *		`host.'
 *	tcp eval client command
 *		Do default command processing for command "$command",
 *		originating at client "$client".
 *	tcp login client
 *		Do default login processing for $client.
 *	tcp mainloop
 *		Start the main loop for a server or group of servers.
 *	tcp poll
 *		Poll for whether servers have work to do.
 *	tcp servers
 *		Returns a list of the currently active servers.
 *	tcp server ?args?
 *		Set up a server to run in the current interpreter.
 *	tcp wait
 *		Wait for a server to have work to do.
 *----------------------------------------------------------------------
 */

int
Tk_TcpCmd (clientData, interp, argc, argv)
     ClientData clientData;
     Tcl_Interp * interp;
     int argc;
     char * * argv;
{
  char c;
  unsigned length;

  if (argc < 2) {
    Tcl_AppendResult (interp, "wrong # args: should be \"", argv [0],
		      " command ?args?\"", (char *) NULL);
    return TCL_ERROR;
  }

  c = argv [1] [0];
  length = strlen (argv [1]);

  if ((c == 'c') && (length >= 2) &&
      (strncmp (argv [1], "client", length) == 0)) {
    return tcpClientCommand (clientData, interp, argc-1, argv+1);
  }
  if ((c == 'c') && (length >= 2) &&
      (strncmp (argv [1], "connect", length) == 0)) {
    return tcpConnectCommand (clientData, interp, argc-1, argv+1);
  }
  if ((c == 'e') && (strncmp (argv [1], "eval", length) == 0)) {
    return tcpEvalCommand (clientData, interp, argc-1, argv+1);
  }
  if ((c == 'l') && (strncmp (argv [1], "login", length) == 0)) {
    return tcpLoginCommand (clientData, interp, argc-1, argv+1);
  }
  if ((c == 'm') && (strncmp (argv [1], "mainloop", length) == 0)) {
    return tcpMainLoopCommand (clientData, interp, argc-1, argv+1);
  }
  if ((c == 'p') && (strncmp (argv [1], "poll", length) == 0)) {
    return tcpPollCommand (clientData, interp, argc-1, argv+1);
  }
  if ((c == 's') && (length >= 7)
      && (strncmp (argv [1], "servers", length) == 0)) {
    return tcpServersCommand (clientData, interp, argc-1, argv+1);
  }
  if ((c == 's') && (strncmp (argv [1], "server", length) == 0)) {
    return tcpServerCommand (clientData, interp, argc-1, argv+1);
  }
  if ((c == 'w') && (strncmp (argv [1], "wait", length) == 0)) {
    return tcpWaitCommand (clientData, interp, argc-1, argv+1);
  }
  Tcl_AppendResult (interp, "bad option \"", argv [1],
		    "\": should be client, eval, login,",
		    " mainloop, poll, servers, server or wait",
		    (char *) NULL);
  return TCL_ERROR;

}

/*
 * tcpClientCommand --
 *
 *	This procedure is invoked to process the "tcp client" Tcl command.
 * It returns the name of the currently-active client, or an error if there
 * is none.
 */

/* ARGSUSED */
static int
tcpClientCommand (clientData, interp, argc, argv)
     ClientData clientData;
     Tcl_Interp * interp;
     int argc;
     char * * argv;
{
  /* Check syntax */

  if (argc != 1) {
    Tcl_AppendResult (interp, "wrong # args: should be \"", argv [-1], " ",
		      argv [0], "\"", (char *) NULL);
    return TCL_ERROR;
  }

  /* Make sure there is a current client */

  if (tcpCurrentClient == NULL) {
    Tcl_SetResult (interp, "no current client", TCL_STATIC);
    return TCL_ERROR;
  }

  Tcl_SetResult (interp, tcpCurrentClient, TCL_VOLATILE);
  return TCL_OK;
}

/* tcpConnectCommand --
 *
 *	This procedure is invoked to process the "tcp connect" Tcl command.
 * It takes two arguments: a host name and a port.  It tries to establish a
 * connection to the specified port and host.
 */

/* ARGSUSED */
static int
tcpConnectCommand (clientData, interp, argc, argv)
     ClientData clientData;
     Tcl_Interp * interp;
     int argc;
     char * * argv;
{
  struct hostent * host;
  struct sockaddr_in sockaddr;
  int haddr;
  int port;
  int status;
  int f;
  char name [20];

  /* Check syntax */

  if (argc != 3) {
    Tcl_AppendResult (interp, "wrong # args, should be \"", argv [-1], " ",
		      argv [0], " hostname port#\"", (char *) NULL);
    return TCL_ERROR;
  }

  /* Decode the host name */

  sockaddr.sin_family = AF_INET;
  host = gethostbyname (argv [1]);
  if (host != NULL) {
    memcpy ((char *) &(sockaddr.sin_addr.s_addr),
	    (char *) (host -> h_addr_list [0]),
	    (size_t) (host -> h_length));
  } else {
    haddr = inet_addr (argv [1]);
    if (haddr == -1) {
      Tcl_AppendResult (interp, argv[1], ": host unknown", (char *) NULL);
      return TCL_ERROR;
    }
    sockaddr.sin_addr.s_addr = haddr;
  }

  /* Decode the port number */

  status = Tcl_GetInt (interp, argv [2], &port);
  if (status) return status;
  sockaddr.sin_port = htons (port);

  /* Make a socket to talk to the server */

  f = socket (AF_INET, SOCK_STREAM, 0);
  if (f < 0) {
    Tcl_AppendResult (interp, "can't create socket: ",
		      Tcl_UnixError (interp), (char *) NULL);
    return TCL_ERROR;
  }

  /* Connect to the server */

  status = connect (f, (struct sockaddr *) &sockaddr, sizeof sockaddr);
  if (status < 0) {
    Tcl_AppendResult (interp, "can't connect to server: ",
		      Tcl_UnixError (interp), (char *) NULL);
    (void) close (f);
    return TCL_ERROR;
  }

  /* Get the server's greeting message */

  status = tcpReceiveResultFromServer (interp, f);

  if (status == TCL_OK) {

    /* Stash the greeting, make the connection object and return it. */

    sprintf (name, "tcp_connection_%d", f);
    (void) Tcl_SetVar2 (interp, "tcp_greeting", name, interp -> result,
			TCL_GLOBAL_ONLY);
    Tcl_CreateCommand (interp, name, (Tcl_CmdProc *) tcpConnectionObjectCmd,
		       (ClientData) f,
		       (Tcl_CmdDeleteProc *) deleteTcpConnectionObjectCmd);
    Tcl_SetResult (interp, name, TCL_VOLATILE);
    return TCL_OK;
  } else {

    /* Error reading greeting, quit */

    (void) close (f);
    return TCL_ERROR;
  }
}    

/*
 * tcpEvalCommand --
 *
 *	This procedure is invoked to process the "tcp eval" Tcl command.
 * "tcp eval" is the default command invoked to process connections once
 * a connection has been accepted by "tcp login".
 */

/* ARGSUSED */
static int
tcpEvalCommand (clientData, interp, argc, argv)
     ClientData clientData;
     Tcl_Interp * interp;
     int argc;
     char * * argv;
{
  int status;

  /* Argc == 2 means that we're logging out a client.  Default is to ignore
   * the logout.
   */

  if (argc == 2) {
    return TCL_OK;
  }

  /* Three-argument form is a command from a client.  Default is to eval
   * the command */

  if (argc != 3) {
    Tcl_AppendResult (interp, "wrong # args: should be \"", argv [-1],
		      " ", argv [0], " client command\"", (char *) NULL);
    return TCL_ERROR;
  }

  status = Tcl_Eval (interp, argv [2], 0, (char * *) NULL);
  return status;
}  

/*
 * tcpLoginCommand --
 *
 *	This procedure is invoked to process the "tcp login" Tcl command.
 * It is the default command procedure at initial connection to a server.
 * It is invoked with the name of a client.  It returns TCL_OK, together
 * with a greeting message, if the login succeeds, and TCL_ERROR, together
 * with a denial message, if it fails.
 *
 *	The authentication procedure is as follows:
 *
 * - If the client is on the local host, the connection is accepted.
 * - If the client's IP address is the same as the local host's IP address,
 *   the connection is accepted.
 * - Otherwise, the connection is refused.
 *
 * 	Obviously, there are other authentication techniques.  The use can
 * replace this command with an arbitrary Tcl script.
 */

/*ARGSUSED*/
static int
tcpLoginCommand (clientData, interp, argc, argv)
     ClientData clientData;
     Tcl_Interp * interp;
     int argc;
     char * * argv;
{
  char * hostName;		/* Name of the client's host */
  int status;

  /* Check command syntax */

  if (argc != 2) {
    Tcl_AppendResult (interp, "wrong # args; should be \"", argv [-1], " ",
		      argv [0], " clientName\"", (char *) NULL);
    return TCL_ERROR;
  }

  /* Get the hostname by doing $client hostname */

  status = Tcl_VarEval (interp, argv [1], " hostname", (char *) NULL);
  if (status == TCL_OK) {
    hostName = (char *) ckalloc (strlen (interp -> result) + 1);
    strcpy (hostName, interp -> result);

    /* Check that the host is trusted */
    
    if (tcpTrustedHost (hostName)) {

      /* Change the command to `tcp eval' for next time */

      status = Tcl_VarEval (interp, argv [1], " command {tcp eval}",
			    (char *) NULL);


      if (status == TCL_OK) {

	/* Return a greeting message */

	Tcl_ResetResult (interp);
	Tcl_AppendResult (interp, "GE DICE TCP-based Tcl server\n", RCSid,
			  "\n", copyright, (char *) NULL);

	return TCL_OK;

      }

    }

    ckfree ((char *) hostName);
  }

  /* Host isn't trusted or one of the commands failed. */

  Tcl_SetResult (interp, "Permission denied", TCL_STATIC);
  return TCL_ERROR;
}

/*
 * tcpMainLoopCommand:
 *
 *	This procedure is invoked in a non-Tk environment when the server
 * implementor wishes to use a main loop built into the library.  It
 * repeatedly polls ofr work to be done, returning only when the last server
 * is closed.
 *
 *	In a Tk environment, the procedure returns immediately.
 */

/*ARGSUSED*/
static int
tcpMainLoopCommand (clientData, interp, argc, argv)
     ClientData clientData;
     Tcl_Interp * interp;
     int argc;
     char * * argv;
{

  int status;

  if (argc != 1) {
    Tcl_AppendResult (interp, "wrong # args: should be \"", argv [-1],
		      " ", argv [0], "\"", (char *) NULL);
    return TCL_ERROR;
  }

  errno = 0; status = -1;

  /* In a non-Tk environment, errno has a Unix error or 0 for no clients
   * or servers.  In a Tk environment, errno is zero at this point.
   */

  if (errno != 0) {
    Tcl_AppendResult (interp, "select: ", Tcl_UnixError (interp),
		      (char *) NULL);
    return TCL_ERROR;
  }

  return TCL_OK;
}


/*
 * tcpPollCommand:
 *
 *	This procedure is invoked to process the "tcp poll" Tcl
 * command.  It requests that pending events for the servers be processed.
 * It returns a count of events that were processed successfully.
 *
 *	In a Tk environment, the procedure reports that no servers are known
 * to the event handler.  This is correct -- servers register with Tk, not
 * with the simple event handler.
 */

/*ARGSUSED*/
static int
tcpPollCommand (clientData, interp, argc, argv)
     ClientData clientData;
     Tcl_Interp * interp;
     int argc;
     char * * argv;
{
  int status;

  if (argc != 1) {
    Tcl_AppendResult (interp, "wrong # args: should be \"", argv [-1],
		      " ", argv [0], "\"", (char *) NULL);
    return TCL_ERROR;
  }

  /* Do the poll */

  errno = 0; status = -1;

  /* Check for trouble */

  if (status < 0) {
    if (errno == 0) {
      Tcl_SetResult (interp, "no servers known to event handler", TCL_STATIC);
    } else {
      Tcl_AppendResult (interp, "select: ", Tcl_UnixError (interp),
			(char *) NULL);
    }
    return TCL_ERROR;
  }

  /* Return the number of events processed. */

  sprintf (interp -> result, "%d", status);
  return TCL_OK;
}

/* tcpServerCommand:
 *
 *	This procedure is invoked to process the "tcp server" Tcl
 * command.  It requests that a server be created to listen at a
 * TCP/IP port, whose number may be assigned by the system or
 * specified by the user with the "-port" option.
 *
 *	A command string is supplied for use when the server begins to
 * accept connections.  See the documentation of tcpServerObjectCmd
 * for a description of the command string.
 *
 *	If the server is created successfully, the return value will
 * be the name of a "server object" that can be used for future
 * actions upon the server.  This object will be usable as a Tcl
 * command; the command is processed by the tcpServerObjectCmd function.
 *
 * Syntax:
 *	tcp server ?-port #? ?-command string?
 * 
 * Results:
 *	A standard Tcl result.  Return value is the name of the server
 *	object, which may be invoked as a Tcl command (see
 *	tcpServerObjectCmd for details).
 */

/* ARGSUSED */
static int
tcpServerCommand (clientData, interp, argc, argv)
     ClientData clientData;
     Tcl_Interp * interp;
     int argc;
     char * * argv;
{
  int unixStatus;
  int one;
  char * message;
  char * nargv [3];
  int nargc;

  /* Create a structure to hold the tcp server's description. */

  Tcp_ServerData * server =
    (Tcp_ServerData *) ckalloc (sizeof (Tcp_ServerData));

  /* Set up the interpreter and the default command. Clear the list of
   * clients. */

  server -> interp = interp;
  server -> command = "tcp login";
  server -> freeCommand = TCL_STATIC;
  server -> stopFlag = 0;
  server -> raw = 0;
  server -> firstClient = (Tcp_ClientData *) NULL;

  /* Create the socket at which the server will listen. */

  server -> socketfd = socket (AF_INET, SOCK_STREAM, 0);
  if (server -> socketfd < 0) {
    Tcl_AppendResult (interp, "can't create socket: ",
		      Tcl_UnixError (interp), (char *) NULL);
  } else {

    /* Set up the socket for non-blocking I/O. */

    one = 1;
    unixStatus = ioctl (server -> socketfd, FIONBIO, (char *) &one);
    if (unixStatus < 0) {
      Tcl_AppendResult (interp, "can't set non-blocking I/O on socket: ",
			Tcl_UnixError (interp), (char *) NULL);
    } else {

      /* Server structure has been created and socket has been opened.
       * Now configure the server.
       */

      if (tcpServerObjectConfig ((ClientData) server, interp, argc, argv)
	  == TCL_OK)
	{

	  /* Link the server on the list of active servers */

	  if (tcpFirstServer)
	    tcpFirstServer -> prev = server;
	  server -> next = tcpFirstServer;
	  tcpFirstServer = server;
	  server -> prev = NULL;

	  /* Add the server object command */
	  
	  sprintf (server -> name, "tcp_server_%d", server -> socketfd);

	  Tcl_CreateCommand (interp, server -> name,
			     (Tcl_CmdProc *) tcpServerObjectCmd,
			     (ClientData) server,
			     (Tcl_CmdDeleteProc *) deleteTcpServerObjectCmd);

	  Tcl_SetResult (interp, server -> name, TCL_STATIC);
	  
	  return TCL_OK;

	}
    }

    /* Error in configuring the server.  Trash the socket. */
    
    unixStatus = close (server -> socketfd);
    if (unixStatus < 0) {
      nargc = 3;
      nargv [0] = "(also failed to close socket: ";
      nargv [1] = Tcl_UnixError (interp);
      nargv [2] = ")";
      message = Tcl_Concat (nargc, nargv);
      Tcl_AddErrorInfo (interp, message);
      free (message);
    }
  }
      
  /* Error in creating the server -- get rid of the data structure */

  if (server -> freeCommand != NULL) {
    (*(server -> freeCommand)) (server -> command);
  }
  ckfree ((char *) server);
  return TCL_ERROR;
}

/*
 * tcpServersCommand:
 *
 *	The following procedure is invoked to process the `tcp servers' Tcl
 * command.  It returns a list of the servers that are currently known.
 */

/* ARGSUSED */
static int
tcpServersCommand (clientData, interp, argc, argv)
     ClientData clientData;
     Tcl_Interp * interp;
     int argc;
     char * * argv;
{
  Tcp_ServerData * server;

  /* Check syntax */

  if (argc != 1) {
    Tcl_AppendResult (interp, "wrong # args: should be \"", argv [-1], " ",
		      argv [0], "\"", (char *) NULL);
    return TCL_ERROR;
  }

  for (server = tcpFirstServer; server != NULL; server = server -> next) {
    Tcl_AppendElement (interp, server -> name, 0);
  }

  return TCL_OK;
}

/*
 * tcpWaitCommand:
 *
 *	This procedure is invoked to process the "tcp wait" Tcl
 * command.  It requests that the process delay until an event is
 * pending for a TCP server.
 *
 * It returns a count of pending events.
 *
 *	In a Tk environment, the procedure returns an error message stating
 * that no servers are known to the event handler.  This is correct.  The
 * servers register with Tk's event handler, and are not known to the simple
 * event handler.
 */

/*ARGSUSED*/
static int
tcpWaitCommand (clientData, interp, argc, argv)
     ClientData clientData;
     Tcl_Interp * interp;
     int argc;
     char * * argv;
{
  int status;

  if (argc != 1) {
    Tcl_AppendResult (interp, "wrong # args: should be \"", argv [-1],
		      " ", argv [0], "\"", (char *) NULL);
    return TCL_ERROR;
  }

  /* Do the poll */

  errno = 0; status = -1;

  /* Check for trouble */

  if (status < 0) {
    if (errno == 0) {
      Tcl_SetResult (interp, "no servers known to event handler", TCL_STATIC);
    } else {
      Tcl_AppendResult (interp, "select: ", Tcl_UnixError (interp),
			(char *) NULL);
    }
    return TCL_ERROR;
  }

  /* Return the number of events pending. */

  sprintf (interp -> result, "%d", status);
  return TCL_OK;
}

/*
 * tcpServerObjectCmd --
 *
 *	This procedure is invoked when a command is called on a server
 *	object directly.  It dispatches to the appropriate command processing
 *	procedure to handle the command.
 *
 * $server accept
 *	[Internal call] - Accept a connection.
 * $server clients
 *	Return a list of all clients connected to a server.
 * $server configure ?args?
 *	Revise or query a server's configuration.
 * $server start
 *	Start a server running.
 * $server stop
 *	Terminate a server.
 */

static int
tcpServerObjectCmd (clientData, interp, argc, argv)
     ClientData clientData;
     Tcl_Interp * interp;
     int argc;
     char * * argv;
{
  int c;
  unsigned length;

  if (argc < 2) {
    Tcl_AppendResult (interp, "wrong # args: should be \"", argv [0],
		      " command ?args?\"", (char *) NULL);
    return TCL_ERROR;
  }

  c = argv [1] [0];
  length = strlen (argv [1]);
  
  if (c == 'a' && strncmp (argv [1], "accept", length) == 0) {
    return tcpServerObjectAcceptCmd (clientData, interp, argc-1, argv+1);
  }
  if (c == 'c' && length >= 2 && strncmp (argv [1], "clients", length) == 0) {
    return tcpServerObjectClientsCmd (clientData, interp, argc-1, argv+1);
  }
  if (c == 'c' && length >= 2
      && strncmp (argv [1], "configure", length) == 0) {
    return tcpServerObjectConfigCmd (clientData, interp, argc-1, argv+1);
  }
  if (c == 's' && length >= 3 && strncmp (argv [1], "start", length) == 0) {
    return tcpServerObjectStartCmd (clientData, interp, argc-1, argv+1);
  }
  if (c == 's' && length >= 3 && strncmp (argv [1], "stop", length) == 0) {
    return tcpServerObjectStopCmd (clientData, interp, argc-1, argv+1);
  }
  Tcl_AppendResult (interp, argv [0], ": ", "bad option \"", argv [1],
		    "\": should be clients, configure, start, or stop",
		    (char *) NULL);
  return TCL_ERROR;
}

/*
 * tcpServerObjectAcceptCmd --
 *
 *	The following procedure handles the `accept' command on a
 *	server object.  It is called in the background by
 *	tcpServerAcceptConnection when a connection request appears on
 *	a server.  It is responsible for creating the client and
 *	accepting the connection request.
 *
 * Results:
 *	Returns a standard TCL result.  The return value is the name
 *	of the client if the call is successful.
 *
 * Side effects:
 *	A Tcl command named after the client object is created.
 */

static int
tcpServerObjectAcceptCmd (clientData, interp, argc, argv)
     ClientData clientData;
     Tcl_Interp * interp;
     int argc;
     char * * argv;
{
  register Tcp_ServerData * server = (Tcp_ServerData *) clientData;
  register Tcp_ClientData * client;
  int rubbish;
  int unixStatus;
  int nargc;
  char * nargv [3];
  char * message;

  /* Check command syntax */

  if (argc != 1) {
    Tcl_AppendResult (interp, "wrong # args, should be \"", argv [-1], " ",
		      argv [0], "\"", (char *) NULL);
    return TCL_ERROR;
  }

  /* Create the client data structure */

  client = (Tcp_ClientData *) ckalloc (sizeof (Tcp_ClientData));

  /* Accept the client's connection request */

  rubbish = sizeof (struct sockaddr_in);
  client -> socketfd = accept (server -> socketfd,
			       (struct sockaddr *) &(client -> peeraddr),
			       &rubbish);
  if (client -> socketfd < 0) {
    Tcl_AppendResult (interp, "can't accept connection: ",
		      Tcl_UnixError (interp), (char *) NULL);
    ckfree ((char *) client);
    return TCL_ERROR;
  }

  /* Set up the socket for non-blocking I/O */

  rubbish = 1;
  unixStatus = ioctl (client -> socketfd, FIONBIO, (char *) &rubbish);
  if (unixStatus < 0) {
    Tcl_AppendResult (interp,
		      "can't set non-blocking I/O on client's socket: ",
		      Tcl_UnixError (interp), (char *) NULL);
    unixStatus = close (client -> socketfd);
    if (unixStatus < 0) {
      nargc = 3;
      nargv [0] = "(also failed to close socket: ";
      nargv [1] = Tcl_UnixError (interp);
      nargv [2] = ")";
      message = Tcl_Concat (nargc, nargv);
      Tcl_AddErrorInfo (interp, message);
      free (message);
    }
    ckfree ((char *) client);
    return TCL_ERROR;
  }

  /* Set up the client's description */

  client -> server = server;
  sprintf (client -> name, "tcp_client_%d", client -> socketfd);
  client -> command = malloc (strlen (server -> command) + 1);
  client -> freeCommand = (Tcl_FreeProc *) free;
  strcpy (client -> command, server -> command);
  client -> inputBuffer = Tcl_CreateCmdBuf ();
  client -> resultString = client -> resultPointer = (char *) NULL;
  client -> freeResultString = (Tcl_FreeProc *) NULL;
  client -> activeFlag = 0;
  client -> closeFlag = 0;
  client -> next = server -> firstClient;
  if (client -> next != NULL) {
    client -> next -> prev = client;
  }
  client -> prev = NULL;
  server -> firstClient = client;

  /* Create the Tcl command for the client */
  
  Tcl_CreateCommand (interp, client -> name,
		     (Tcl_CmdProc *) tcpClientObjectCmd,
		     (ClientData) client,
		     (Tcl_CmdDeleteProc *) deleteTcpClientObjectCmd);

  /* Return the client's name */

  Tcl_SetResult (interp, client -> name, TCL_STATIC);
  return TCL_OK;
}

/*
 * tcpServerObjectClientsCmd --
 *
 *	This procedure in invoked in response to the `clients' command
 * on a TCP server object.  It returns a list of clients for the server.
 */

static int
tcpServerObjectClientsCmd (clientData, interp, argc, argv)
     ClientData clientData;
     Tcl_Interp * interp;
     int argc;
     char * * argv;
{
  Tcp_ServerData * server = (Tcp_ServerData *) clientData;
  Tcp_ClientData * client;

  /* Check syntax */

  if (argc != 1) {
    Tcl_AppendResult (interp, "wrong # args, should be\"", argv [-1], " ",
		      argv [0], "\"", (char *) NULL);
    return TCL_ERROR;
  }

  for (client = server -> firstClient; client != NULL;
       client = client -> next) {
    Tcl_AppendElement (interp, client -> name, 0);
  }

  return TCL_OK;
}

/*
 * tcpServerObjectConfigCmd --
 *
 *	This procedure is invoked in response to the `config' command
 *	on a TCP server object.  With no arguments, it returns a list
 *	of valid arguments.  With one argument, it returns the current
 *	value of that option.  With multiple arguments, it attempts to
 *	configure the server according to that argument list.
 * Results:
 *	Returns a standard Tcl result.
 */

static int
tcpServerObjectConfigCmd (clientData, interp, argc, argv)
     ClientData clientData;
     Tcl_Interp * interp;
     int argc;
     char * * argv;
{
  int unixStatus;
  int c;
  unsigned length;

  /* No arguments -- return a list of valid options. */

  if (argc <= 1) {
    Tcl_SetResult (interp, "-command -port", TCL_STATIC);
    return TCL_OK;
  }

  /* One argument -- query a particular option */

  if (argc == 2) {
    register Tcp_ServerData * server = (Tcp_ServerData *) clientData;
    char * arg = argv [1];

    if (arg [0] != '-') {
      Tcl_AppendResult (interp, argv [-1], " ", argv [0],
			": bad option \"", arg,
			"\" -- each option must begin with a hyphen.",
			(char *) NULL);
      return TCL_ERROR;

    }

    length = strlen (++arg);
    c = arg [0];

    if (c == 'c' && strncmp (arg, "command", length) == 0) {

      /* Command option -- Get the command name */

      Tcl_SetResult (interp, server->name, TCL_STATIC);
      return TCL_OK;

    }

    if (c == 'p' && strncmp (arg, "port", length) == 0) {

      /* Port option -- Get the port number */

      struct sockaddr_in portaddr;
      int rubbish = sizeof (struct sockaddr_in);

      unixStatus = getsockname (server -> socketfd,
				(struct sockaddr *) &portaddr, &rubbish);
      if (unixStatus < 0) {
	Tcl_AppendResult (interp, argv [-1], ": can't read port #: ",
			  Tcl_UnixError (interp), (char *) NULL);
	return TCL_ERROR;
      }
      Tcl_ResetResult (interp);
      sprintf (interp -> result, "%d", (int) ntohs (portaddr.sin_port));
      return TCL_OK;
    }
	
    /* Unknown option */

    Tcl_AppendResult (interp, argv [-1], ": unknown option \"", arg,
		      "\" -- must be -command or -port", (char *) NULL);
    return TCL_ERROR;
  }

  return tcpServerObjectConfig (clientData, interp, argc, argv);
}  

/*
 * tcpServerObjectStartCmd --
 *
 *	This procedure is invoked to process the "start" command on a
 *	TCP server object.  It sets the server up so that new
 *	connection requests will create "server-client" objects and
 *	invoke the server's command with them.
 *
 *	If Tk is available, the "start" command returns to the caller.
 *	If Tk is not available, the "start" command immediately enters
 *	a loop that attempts to process the connection events (and
 *	other file events as well).  The loop may be exited by
 *	executing a `stop' command on the server object.  (The `stop'
 *	command also exists in the Tk environment, since there is more
 *	to stopping a server than just breaking out of its event
 *	loop.)
*/

static int
tcpServerObjectStartCmd (clientData, interp, argc, argv)
     ClientData clientData;
     Tcl_Interp * interp;
     int argc;
     char * * argv;
{
  register Tcp_ServerData * server = (Tcp_ServerData *) clientData;
  int unixStatus;

  /* Check command syntax */

  if (argc != 1) {
    Tcl_AppendResult (interp, "wrong # args, should be \"", argv [-1], " ",
		      argv [0], "\"", (char *) NULL);
    return TCL_ERROR;
  }

  /* Listen at the server's socket */

  unixStatus = listen (server -> socketfd, TCP_LISTEN_BACKLOG);
  if (unixStatus < 0) {
    Tcl_AppendResult (interp, argv [-1], ": can't listen at socket: ",
		      Tcl_UnixError (interp), (char *) NULL);
    return TCL_ERROR;
  }

  /* Add a file handler to gain control at tcpServerAcceptConnection
   * whenever a client attempts to connect.
   */

  simpleCreateFileHandler (server -> socketfd, TK_READABLE,
			   (Tk_FileProc *) tcpServerAcceptConnection,
			   clientData);
  return TCL_OK;
}

/*
 * tcpServerObjectStopCmd
 *
 *	This procedure is invoked in response to the `$server stop' Tcl
 * command.  It destroys the server's object command.  Destroying the object
 * command, in turn, attempts to shut down the server in question.  It closes
 * the listen socket, closes all the clients, and sets the `stop' flag for
 * the server itself.  It then calls `tcpServerClose' to try to get rid of
 * the server.
 *
 *	If one or more clients are active, the server does not shut down
 * until they can be closed properly.
 */

static int
tcpServerObjectStopCmd (clientData, interp, argc, argv)
     ClientData clientData;
     Tcl_Interp * interp;
     int argc;
     char * * argv;
{
  register Tcp_ServerData * server = (Tcp_ServerData *) clientData;

  if (argc != 1) {
    Tcl_AppendResult (interp, "wrong # args: should be \"", argv [-1],
		      " ", argv [0], "\"", (char *) NULL);
    return TCL_ERROR;
  }

  /* Delete the server command */

  Tcl_DeleteCommand (interp, server -> name);

  return TCL_OK;
}

/*
 * deleteTcpServerObjectCmd --
 *
 *	This procedure is called when a server's object command is deleted.
 *
 *	It is the first procedure called when a server is shut down.  It
 * closes the listen socket and deletes its file handler.  It also attempts
 * to close all the clients.
 *
 *	It may be that a client needs to be able to complete a data transfer
 * before it can be closed.  In this case, the `close flag' for the client is
 * set.  The client will be deleted when it reaches a quiescent point.
 *
 *	Once all the clients are gone, tcpDeleteServer removes the server's
 * client data structure.
 */

static void
deleteTcpServerObjectCmd (clientData)
     ClientData clientData;
{
  register Tcp_ServerData * server = (Tcp_ServerData *) clientData;
  register Tcp_ClientData * client, * nextClient;

  /* Close the listen socket and delete its handler */

  simpleDeleteFileHandler (server -> socketfd);
  (void) close (server -> socketfd);
  server -> socketfd = -1;

  /* Close all clients */

  for (client = server -> firstClient; client != NULL; client = nextClient) {
    nextClient = client -> next;
    if (client -> activeFlag)
      client -> closeFlag = 1;
    else
      tcpCloseClient (client);
  }

  /* Remove the server from the list of servers. */

  if (server -> next != NULL)
    server -> next -> prev = server -> prev;
  if (server -> prev != NULL)
    server -> prev -> next = server -> next;
  else
    tcpFirstServer = server -> next;

  /* If all clients are closed, get to tcpDeleteServer now.  Otherwise, set
   * the server's stop flag and return.
   */

  if (server -> firstClient == NULL) {
    tcpDeleteServer (server);
  } else {
    server -> stopFlag = 1;
  }
}

/*
 * tcpDeleteServer --
 *
 *	This procedure is invoked as the final phase of deleting a TCP server.
 * When execution gets here, the server's listen socket has been closed and
 * the handler has been removed.  The server's object command has been deleted.
 * The server has been removed from the list of active servers.  All the
 * server's clients have been closed.  The server's login command has been
 * deleted.  All that remains is to deallocate the server's data structures.
 */

static void
tcpDeleteServer (server)
     Tcp_ServerData * server;
{
  /* Get rid of the server's initial command */

  if (server -> command != NULL && server -> freeCommand != NULL) {
    (*(server -> freeCommand)) (server -> command);
  }

  /* Get rid of the server's own data structure */

  (void) ckfree ((char *) server);
}

/*
 * tcpServerObjectConfig --
 *
 *	This procedure is invoked to configure a TCP server object.
 *	It may be called from tcpServerCommand when the server is
 *	first being created, or else from tcpServerObjectCmd if the
 *	server object is called with the "config" option.
 *
 *	In any case, the arguments are expected to contain zero or
 *	more of the following:
 *
 *	-port <number>
 *		Requests that the server listen at a specific port.
 *		Default is whatever the system assigns.
 *
 *	-command <string>
 *		Specifies the initial command used when a client
 *		first connects to the server.  The command is
 *		concatenated with the name of a "server-client" object
 *		that identifies the client, and then called:
 *			command client
 *		Default is "tcp login"
 *
 *	-raw
 *		Puts the server in raw socket mode.
 *
 * Result:
 *	A standard TCL result.
 */

static int
tcpServerObjectConfig (clientData, interp, argc, argv)
     ClientData clientData;
     Tcl_Interp * interp;
     int argc;
     char * * argv;
{

  register Tcp_ServerData * server = (Tcp_ServerData *) clientData;

  int status;
  int unixStatus;

  /* On entry, argc shows one plus the number of parameters.  Argv[-1] */
  /* and argv[0] give the command that got us here: either "tcp */
  /* server" or else "serverName config" */

  int a;
  unsigned length;
  int c;

  /* Step through the parameters */

  for (a = 1; a < argc; ++a) {
    char * arg = argv [a];

    if (arg [0] != '-') {
      Tcl_AppendResult (interp, argv [-1], ": bad option \"", arg,
			"\" -- each option must begin with a hyphen.",
			(char *) NULL);
      return TCL_ERROR;
    } else {

      length = strlen (++arg);
      c = arg [0];

      if (c == 'c' && strncmp (arg, "command", length) == 0) {

	/* Command option -- Get the command name */

	++a;
	if (a >= argc) {
	  Tcl_AppendResult (interp, argv [-1],
			    ": \"-command\" must be followed by a string.",
			    (char *) NULL);
	  return TCL_ERROR;
	}

	/* Free the old command name */

	if (server -> freeCommand != NULL) {
	  (*(server -> freeCommand)) (server -> command);
	}

	/* Put in the new command name */

	server -> command = (char *) malloc (strlen (argv [a]) + 1);
	strcpy (server -> command, argv [a]);
	server -> freeCommand = (Tcl_FreeProc *) free;

      } else if (c == 'p' && strncmp (arg, "port", length) == 0) {

	/* Port option -- get the port number */

	char * portstr;
	int portno;
	struct sockaddr_in portaddr;

	++a;
	if (a >= argc) {
	  Tcl_AppendResult (interp, argv [-1],
			    ": \"-port\" must be followed by a number.",
			    (char *) NULL);
	  return TCL_ERROR;
	}
	portstr = argv [a];
	status = Tcl_GetInt (interp, portstr, &portno);
	if (status) return status;

	/* Set the port number */

	memset ((void *) & portaddr, 0, sizeof (struct sockaddr_in));
	portaddr.sin_port = htons (portno);
	unixStatus = bind (server -> socketfd,
			   (struct sockaddr *) &portaddr,
			   sizeof (struct sockaddr_in));
	if (unixStatus < 0) {
	  Tcl_AppendResult (interp, argv [-1],
			    ": can't set port number: ",
			    Tcl_UnixError (interp), (char *) NULL);
	  return TCL_ERROR;
	}

      } else if (c == 'r' && strncmp (arg, "raw", length) == 0) {

	/* raw option -- set raw socket mode */

	server -> raw = 1;

      } else {

	/* Unknown option */

	Tcl_AppendResult (interp, argv [-1],
			  ": unknown option \"", arg - 1,
			  "\" -- must be -command or -port", (char *) NULL);
	return TCL_ERROR;
      }
    }
  }

  Tcl_SetResult (interp, server -> name, TCL_STATIC);
  return TCL_OK;
}

/*
 * tcpClientObjectCmd --
 *
 *	This procedure handles the object command for a Tcp client (on
 *	the server side).  It takes several forms:
 *		$client command ?command?
 *			With no arguments, returns the client's
 *			current command.  With arguments, replaces the
 *			client's command with the arguments
 *		$client close
 *			Deletes the client.  If a command is being
 *			processed on the client's behalf, the client
 *			will not be deleted until the command's result
 *			is returned.
 *		$client do ?args?
 *			Concatenate the client's command with ?args?,
 *			and execute the result.  Called in background
 *			when a command arrives and on initial
 *			connection.
 *		$client hostname
 *			Returns the name of the host where the client
 *			is running.
 *		$client server
 *			Returns the name of the server to which the client
 *			is connected.
 */

static int
tcpClientObjectCmd (clientData, interp, argc, argv)
     ClientData clientData;
     Tcl_Interp * interp;
     int argc;
     char * * argv;
{
  int c;
  unsigned length;

  if (argc < 2) {
    Tcl_AppendResult (interp, "wrong # args: should be \"", argv [0],
		      " command ?args?\"", (char *) NULL);
    return TCL_ERROR;
  }

  c = argv [1] [0];
  length = strlen (argv [1]);
  
  if (c == 'c' && length >= 2 && strncmp (argv [1], "close", length) == 0) {
    return tcpClientObjectCloseCmd (clientData, interp, argc-1, argv+1);
  }
  if (c == 'c' && length >= 2 && strncmp (argv [1], "command", length) == 0) {
    return tcpClientObjectCommandCmd (clientData, interp, argc-1, argv+1);
  }
  if (c == 'd' && strncmp (argv [1], "do", length) == 0) {
    return tcpClientObjectDoCmd (clientData, interp, argc-1, argv+1);
  }
  if (c == 'h' && strncmp (argv [1], "hostname", length) == 0) {
    return tcpClientObjectHostnameCmd (clientData, interp, argc-1, argv+1);
  }
  if (c == 's' && strncmp (argv [1], "server", length) == 0) {
    return tcpClientObjectServerCmd (clientData, interp, argc-1, argv+1);
  }

  Tcl_AppendResult (interp, "bad option \"", argv [1],
		    "\": should be close, command, do, hostname or server",
		    (char *) NULL);
  return TCL_ERROR;
}

/* 
 * tcpClientObjectCloseCmd --
 *
 * 	This procedure is called when the Tcl program wants to close a client.
 * If the client is active, it sets a flag to close the client when it
 * becomes quiescent.  Otherwise, it closes the client immediately.
 */

static int
tcpClientObjectCloseCmd (clientData, interp, argc, argv)
     ClientData clientData;
     Tcl_Interp * interp;
     int argc;
     char * * argv;
{
  register Tcp_ClientData * client = (Tcp_ClientData *) clientData;

  if (argc != 1) {
    Tcl_AppendResult (interp, "wrong # args: should be \"", argv [-1], " ",
		      argv [0], "\"", (char *) NULL);
    return TCL_ERROR;
  }

  if (client -> activeFlag)
    client -> closeFlag = 1;
  else
    tcpCloseClient (client);

  return TCL_OK;
}

/* 
 * tcpClientObjectCommandCmd --
 *
 * 	Query/change the command associated with a client object
 *
 * Syntax:
 *	$client command ?newcommand?
 *
 * Return:
 *	A standard Tcl result containing the client's command.
 */

static int
tcpClientObjectCommandCmd (clientData, interp, argc, argv)
     ClientData clientData;
     Tcl_Interp * interp;
     int argc;
     char * * argv;
{
  register Tcp_ClientData * client = (Tcp_ClientData *) clientData;

  /* Check syntax */

  if (argc > 2) {
    Tcl_AppendResult (interp, "wrong # args: should be \"", argv [-1], " ",
		      argv [0], " ?command?\"", (char *) NULL);
    return TCL_ERROR;
  }

  /* Set command if necessary */

  if (argc == 2) {
    if (client -> freeCommand != (Tcl_FreeProc *) NULL) {
      (*client -> freeCommand) (client -> command);
    }
    client -> command = malloc (strlen (argv [1]) + 1);
    strcpy (client -> command, argv [1]);
    client -> freeCommand = (Tcl_FreeProc *) free;
  }

  /* Return command in any case */

  Tcl_SetResult (interp, client -> command, TCL_STATIC);

  return TCL_OK;
}

/*
 * tcpClientObjectDoCmd --
 *
 *	The following procedure handles the `do' command on a client
 *	object.  It is called
 *		(a) as "$client do", at login.
 *		(b) as "$client do <command>", when the client sends a
 *			command. 
 *		(c) as "$client do", with no further arguments, when
 *			the connection is closed.
 *	It concatenates the client's saved command string with the
 *	client's name, and then with the passed command, resulting in
 *	a command:
 *		saved_command client passed_command
 *	which is then passed to Tcl_Eval for processing.
 *      During the processing of the command, the `active' flag is set for
 *	the client, to avoid having the client closed prematurely.
 */
static int
tcpClientObjectDoCmd (clientData, interp, argc, argv)
     ClientData clientData;
     Tcl_Interp * interp;
     int argc;
     char * * argv;
{

  register Tcp_ClientData * client = (Tcp_ClientData *) clientData;
  int status;
  int closeflag;
  char * prevClient;
  char * excmd;
  unsigned excmdl;
  int scanflags;

  /* Check command syntax */

  if (argc > 2) {
    Tcl_AppendResult (interp, "wrong # args: should be \"", argv [-1],
		      " ", argv [0], " ?command?\"", (char *) NULL);
    return TCL_ERROR;
  }

  prevClient = tcpCurrentClient;
  tcpCurrentClient = client -> name;

  /* Evaluate the client's command, passing the client name and message */

  closeflag = 0;
  client -> activeFlag = 1;

  if (argc == 2) {
    excmdl = Tcl_ScanElement (argv [1], &scanflags) + 1;
    excmd = (char *) ckalloc (excmdl);
    excmdl = Tcl_ConvertElement (argv [1], excmd, scanflags);
    excmd [excmdl] = '\0';
  } else {
    excmd = (char *) NULL;
  }

  status = Tcl_VarEval (interp, client -> command, " ", client -> name, " ",
			excmd, (char *) NULL);

  if (excmd)
    ckfree (excmd);
  
  if (status != TCL_OK && argc < 2) {
    closeflag = 1;
  }

  client -> activeFlag = 0;
  tcpCurrentClient = prevClient;

  /* If the client command throws an error on login or logout,
   * the client should be disconnected.
   * In any case, the result should be reported back to the client.
   */

  if (! (client -> server -> raw)) {
    tcpReturnResultToClient (client, interp, status, closeflag);
  } else {
    tcpPrepareClientForInput (client);
  }

  /* The client may have been closed by the ReturnResult operation. DON'T
   * USE IT AFTER THIS POINT.
   */

  return TCL_OK;
}

/*
 * tcpClientObjectHostnameCmd --
 *
 *	This procedure is invoked in response to the `$client hostname'
 * Tcl command.  It returns the name of the peer host on which the client
 * runs.
 */

static int
tcpClientObjectHostnameCmd (clientData, interp, argc, argv)
     ClientData clientData;
     Tcl_Interp * interp;
     int argc;
     char * * argv;
{
  register Tcp_ClientData * client = (Tcp_ClientData *) clientData;

  struct hostent * hostdesc;

  if (argc != 1) {
    Tcl_AppendResult (interp, "wrong # args: should be \"", argv [-1], " ",
		      argv [0], "\"", (char *) NULL);
    return TCL_ERROR;
  }

  hostdesc = gethostbyaddr ((char *) &(client -> peeraddr.sin_addr.s_addr),
			    sizeof (client -> peeraddr.sin_addr.s_addr),
			    AF_INET);
  
  if (hostdesc != (struct hostent *) NULL) {
    Tcl_SetResult (interp, hostdesc -> h_name, TCL_VOLATILE);
  } else {
    Tcl_SetResult (interp, inet_ntoa (client -> peeraddr.sin_addr),
		   TCL_VOLATILE);
  }

  return TCL_OK;
}

/*
 * tcpClientObjectServerCmd --
 *
 *	This procedure is invoked in response to the `$client server'
 * Tcl command.  It returns the name of the server to which the client
 * is connected.
 */

static int
tcpClientObjectServerCmd (clientData, interp, argc, argv)
     ClientData clientData;
     Tcl_Interp * interp;
     int argc;
     char * * argv;
{
  register Tcp_ClientData * client = (Tcp_ClientData *) clientData;

  if (argc != 1) {
    Tcl_AppendResult (interp, "wrong # args: should be \"", argv [-1], " ",
		      argv [0], "\"", (char *) NULL);
    return TCL_ERROR;
  }

  Tcl_SetResult (interp, client -> server -> name, TCL_STATIC);

  return TCL_OK;
}

/*
 * deleteTcpClientObjectCmd --
 *
 *	This procedure is invoked when a client object's command has
 * been deleted.  WARNING -- deleting a client object command when the
 * client is active is a FATAL error that cannot be reported through the
 * Tcl interpreter.
 *
 *	This procedure does all the cleanup necessary to getting rid of the
 * client.
 */

static void
deleteTcpClientObjectCmd (clientData)
     ClientData clientData;
{
  register Tcp_ClientData * client = (Tcp_ClientData *) clientData;
  register Tcp_ServerData * server = client -> server;

  /* Make sure the client is really dead. */

  if (client -> activeFlag) {
    fprintf (stderr, "attempt to delete an active TCP client!\n\n");
    abort ();
  }

  /* Remove any handler for data on the client's socket. */

  simpleDeleteFileHandler (client -> socketfd);

  /* Now it's safe to close the socket */

  (void) close (client -> socketfd);

  /* Get rid of the command */

  if (client -> command != NULL && client -> freeCommand != NULL) {
    (*(client -> freeCommand)) (client -> command);
  }

  /* Get rid of the input buffer */

  Tcl_DeleteCmdBuf (client -> inputBuffer);

  /* Get rid of any pending result */

  if (client -> resultString != NULL && client -> freeResultString != NULL) {
    (*(client -> freeResultString)) (client -> resultString);
  }

  /* Unlink the client from the list of active clients */

  if (client -> prev == NULL)
    client -> server -> firstClient = client -> next;
  else
    client -> prev -> next = client -> next;

  if (client -> next != NULL)
    client -> next -> prev = client -> prev;

  /* Now it's ok to destroy the client's data structure */

  ckfree ((char *) client);

  /* Handle a deferred close on the server if necessary */

  if (server -> stopFlag && server -> firstClient == NULL)
    tcpDeleteServer (server);
}

/*
 * tcpConnectionObjectCmd --
 *
 *	This procedure is invoked to process the object command for a client-
 * side connection object.  It takes a couple of diferent forms:
 *
 *	$connection close
 *		Closes the connection.
 *	$connection send arg ?arg....?
 *		Catenates the arguments into a Tcl command, and sends them
 *		to the server.
 */

static int
tcpConnectionObjectCmd (clientData, interp, argc, argv)
     ClientData clientData;
     Tcl_Interp * interp;
     int argc;
     char * * argv;
{
  unsigned length;
  int c;
  char * arg;

  if (argc < 2) {
    Tcl_AppendResult (interp, "wrong # args, should be \"", argv [0], " ",
		      "command ?args?\"", (char *) NULL);
    return TCL_ERROR;
  }

  arg = argv [1];
  c = arg [0];
  length = strlen (arg);

  if (c == 'c' && strncmp (arg, "close", length) == 0) {
    return tcpConnectionObjectCloseCmd (clientData, interp, argc-1, argv+1);
  }
  if (c == 's' && strncmp (arg, "send", length) == 0) {
    return tcpConnectionObjectSendCmd (clientData, interp, argc-1, argv+1);
  }

  Tcl_AppendResult (interp, "unknown command \"", arg,
		    "\": must be close or send", (char *) NULL);
  return TCL_ERROR;
}

/*
 * tcpConnectionObjectCloseCmd --
 *
 *	This procedure is invoked in response to a `close' command on a
 * client-side connection object.  It closes the socket and deletes the
 * object command.
 */

/* ARGSUSED */
static int
tcpConnectionObjectCloseCmd (clientData, interp, argc, argv)
     ClientData clientData;
     Tcl_Interp * interp;
     int argc;
     char * * argv;
{
  if (argc != 1) {
    Tcl_AppendResult (interp, "wrong # args, should be \"", argv [-1], " ",
		      argv [0], "\"", (char *) NULL);
    return TCL_ERROR;
  }

  Tcl_DeleteCommand (interp, argv [-1]);
  return TCL_OK;
}

/*
 * tcpConnectionObjectSendCmd --
 *
 *	This procedure is invoked in response to a `send' command on a client-
 * side connection object.  It catenates the `send' arguments into a single
 * string, presents that string to the server as a command, and returns the
 * server's reply.
 */

static int
tcpConnectionObjectSendCmd (clientData, interp, argc, argv)
     ClientData clientData;
     Tcl_Interp * interp;
     int argc;
     char * * argv;
{
  char * message;
  int f = (int) clientData;
  int status;

  if (argc < 2) {
    Tcl_AppendResult (interp, "wrong # args, should be \"", argv [-1], " ",
		      argv [0], " command\"", (char *) NULL);
    return TCL_ERROR;
  }

  /* Paste together the message */

  message = Tcl_Merge (argc-1, argv+1);

  /* Send the command to the server */

  status = tcpSendCmdToServer (interp, f, message);
  if (status != TCL_OK)
    return status;

  /* Get the server's reply */

  return tcpReceiveResultFromServer (interp, f);
}

/*
 * deleteTcpConnectionObjectCmd --
 *
 *	This procedure is called when a connection object is to be
 * deleted.  It just has to close the socket that the object uses.
 */

static void
deleteTcpConnectionObjectCmd (clientData)
     ClientData clientData;
{
  int f = (int) clientData;
  (void) close (f);
}

/*
 * tcpCloseClient --
 *
 *	This procedure is called when the program is completely done with
 * a client object.  If the `active' flag is set, there is still a reference
 * to the dead client, but we shouldn't have come here in that case.
 */

static void
tcpCloseClient (client)
     Tcp_ClientData * client;
{
  if (client -> activeFlag) 
    abort ();

  /* Deleting the client command is all we need to do -- the delete
   * procedure does everything else.
   */

  Tcl_DeleteCommand (client -> server -> interp, client -> name);
}

/*
 * tcpServerAcceptConnection --
 *
 *	This procedure is invoked as a file handler whenever a server's
 *	socket is ready for `reading' -- i.e., has a connection request
 *	outstanding.
 *
 *	It calls the `accept' command on the server to create a client.
 *	If the `accept' is successful, it then calls the `do'
 *	command on the client.  If either call fails, a background error
 *	is reported.
 */

/* ARGSUSED */
static void
tcpServerAcceptConnection (clientData, mask)
     ClientData clientData;
     int mask;
{
  register Tcp_ServerData * server = (Tcp_ServerData *) clientData;

  int status;

  char * clientName;

  /* Accept the connection with `$server accept' */

  status = Tcl_VarEval (server -> interp, server -> name, " accept",
		     (char *) NULL);

  /* On success, try to execute the client's command with `$client do' */

  if (status == TCL_OK) {
    clientName = (char *) ckalloc (strlen (server -> interp -> result) + 1);
    strcpy (clientName, server -> interp -> result);
    status = Tcl_VarEval (server -> interp, clientName, " do", (char *) NULL);

    /* Client may have been closed at this point.  Don't refer to it again. */
  }

  if (status != TCL_OK) {
    simpleReportBackgroundError (server -> interp);
  }
  Tcl_ResetResult (server -> interp);
}

/*
 * tcpTrustedHost --
 *
 *	This procedure is invoked whenever the code must determine whether
 * a host is trusted.  A host is considered trusted only if it is the local
 * host.
 *
 * Results:
 *	Returns a Boolean value that is TRUE iff the host is trusted.
 */

/* The HOSTCMP macro is just strcmp, but puts its args on stderr if
 * the DEBUG_TRUSTED_HOST flag is #define'd.  It's used because this
 * code can be a little flaky; if `hostname' returns a name that is
 * completely unknown in the database, this macro will trace what happened.
 */

#ifdef DEBUG_TRUSTED_HOST
#define HOSTCMP( name1, name2 )					\
  (fprintf (stderr, "tcpTrustedHost: comparing %s with %s\n",	\
	    (name1), (name2)), 					\
   strcmp ((name1), (name2)))
#else
#define HOSTCMP( name1, name2 )					\
  strcmp ((name1), (name2))
#endif

static int
tcpTrustedHost (hostName)
     char * hostName;
{
  char localName [128];
  struct hostent * hostEnt;
  struct in_addr hostAddr;
  int unixStatus;
  int i;

  /* This procedure really has to do things the hard way.  The problem is
   * that the hostname() kernel call returns the host name set by the system
   * administrator, which may not be the host's primary name as known to
   * the domain name system.  Furthermore, the host presented may be one
   * of the names for the loopback port, 127.0.0.1, and this must be checked,
   * too.
   */

  /* Start assembling a list of possibilities for the host name.  First
   * possibility is the name that the kernel returns as hostname ().
   */

  unixStatus = gethostname (localName, 127);
  if (unixStatus >= 0) {

    if (!HOSTCMP( hostName, localName )) return 1;

    /* Next possibility is a.b.c.d notation for all of the local addresses,
     * plus all the nicknames for the host. 
     */

    hostEnt = gethostbyname (localName);
    if (hostEnt != (struct hostent *) NULL) {
      if (!HOSTCMP( hostName, hostEnt -> h_name )) return 1;
      if (hostEnt -> h_aliases != (char * *) NULL) {
	for (i = 0; hostEnt -> h_aliases [i] != (char *) NULL; ++i) {
	  if (!HOSTCMP( hostName, hostEnt -> h_aliases [i] )) return 1;
	}
      }
      if (hostEnt -> h_addr_list != (char * *) NULL) {
	for (i = 0; hostEnt -> h_addr_list [i] != (char *) NULL; ++i) {
	  /* note that the address doesn't have to be word-aligned (!) */
	  memcpy ((char *) &hostAddr,
		  hostEnt -> h_addr_list [i],
		  hostEnt -> h_length);
	  if (!HOSTCMP( hostName, inet_ntoa (hostAddr) )) return 1;
	}
      }
    }
  }

  /* Finally, there's the possibility of the loopback address, and all of 
   * its aliases.*/

  if (!HOSTCMP( hostName, "0.0.0.0" )) return 1;
  if (!HOSTCMP( hostName, "127.0.0.1" )) return 1;
  hostAddr.s_addr = htonl (INADDR_LOOPBACK);
  hostEnt = gethostbyaddr ((char *) &hostAddr, sizeof hostAddr, AF_INET);
  if (hostEnt != (struct hostent *) NULL) {
    if (!HOSTCMP( hostName, hostEnt -> h_name )) return 1;
    if (hostEnt -> h_aliases != (char * *) NULL) {
      for (i = 0; hostEnt -> h_aliases [i] != (char *) NULL; ++i) {
	if (!HOSTCMP( hostName, hostEnt -> h_aliases [i] )) return 1;
      }
    }
    if (hostEnt -> h_addr_list != (char * *) NULL) {
      for (i = 0; hostEnt -> h_addr_list [i] != (char *) NULL; ++i) {
	/* note that the address doesn't have to be word-aligned (!) */
	memcpy ((char *) &hostAddr,
		hostEnt -> h_addr_list [i],
		hostEnt -> h_length);
	if (!HOSTCMP( hostName, inet_ntoa (hostAddr) )) return 1;
      }
    }
  }

  return 0;
}

/*
 * tcpReturnResultToClient --
 *
 *	This procedure is invoked to return a result to a client.  It
 * extracts the interpreter's result string, bundles it with the return
 * status, and stores it in the client's `resultString' area.
 *
 *	It then calls tcpWriteResultToClient to try to start sending the
 * result.
 */

static void
tcpReturnResultToClient (client, interp, status, closeflag)
     Tcp_ClientData * client;
     Tcl_Interp * interp;
     int status;
     int closeflag;
{
  char * argv [2];
  char rint [16];
  unsigned length;
  char * result;
  
  /* Put together a message comprising the return status and the interpreter
   * result */

  sprintf (rint, "%d", status);
  argv [0] = rint;
  argv [1] = interp -> result;
  result = Tcl_Merge (2, argv);
  length = strlen (result);
  client -> resultString = (char *) malloc (length + 2);
  strcpy (client -> resultString, result);
  strcpy (client -> resultString + length, "\n");
  free (result);
  client -> resultPointer = client -> resultString;
  client -> freeResultString = (Tcl_FreeProc *) free;

  Tcl_ResetResult (interp);
  client -> closeFlag |= closeflag;
  
  /* Now try to send the reply. */

  tcpWriteResultToClient ((ClientData) client, TK_WRITABLE);

  /* tcpWriteResultToClient closes the client if it fails; don't depend on
   * having the client still be usable. */
}

/*
 * tcpWriteResultToClient --
 *
 *	This procedure is invoked to issue a write on a client socket.
 * It can be called directly by tcpReturnResultToClient, to attempt the
 * initial write of results.  It can also be called as a file handler,
 * to retry a write that was previously blocked.
 */

/* ARGSUSED */
static void
tcpWriteResultToClient (clientData, mask)
     ClientData clientData;
     int mask;
{
  register Tcp_ClientData * client = (Tcp_ClientData *) clientData;

  int unixStatus;
  int length;

  length = strlen (client -> resultPointer);

  /* Issue the write */

  unixStatus = write (client -> socketfd, client -> resultPointer,
		      length);
  
  /* Test for a total failure */

  if (unixStatus < 0) {
    if (errno != EWOULDBLOCK) {
      tcpClientWriteError (client);
      /* tcpClientWriteError closes the client as a side effect.  Don't depend
       * on the client still being there.
       */
      return;
    } else {
      unixStatus = 0;		/* Pretend that EWOULDBLOCK succeeded at
				 * writing zero characters. */
    }
  }

  /* Test for a partial success */

  if (unixStatus < length) {
    client -> resultPointer += unixStatus;
    simpleCreateFileHandler (client -> socketfd, TK_WRITABLE,
			     (Tk_FileProc *) tcpWriteResultToClient,
			     clientData);
  }

  /* Total success -- prepare the client for the next input */

  else {
    if (client -> freeResultString != NULL) {
      (*(client -> freeResultString)) (client -> resultString);
    }
    client -> resultString = client -> resultPointer = (char *) NULL;
    client -> freeResultString = (Tcl_FreeProc *) NULL;
    simpleDeleteFileHandler (client -> socketfd);
    if (client -> closeFlag) {
      tcpCloseClient (client);

      /* After tcpCloseClient executes, the client goes away.  Don't depend
	 on it's still being there. */

    } else {
      tcpPrepareClientForInput (client);
    }
  }
}   

/*
 * tcpPrepareClientForInput --
 *
 *	This procedure is invoked to prepare a client to accept command
 * input.  It establishes a handler, tcpReceiveClientInput, that does the
 * actual command buffering.
 */

static void
tcpPrepareClientForInput (client)
     Tcp_ClientData * client;
{
  simpleCreateFileHandler (client -> socketfd, TK_READABLE,
			   (Tk_FileProc *) tcpReceiveClientInput,
			   (ClientData) client);
}

/*
 * tcpReceiveClientInput --
 *
 *	This procedure is called when a server is awaiting input from a client
 * and the client socket tests to be `ready to read'.  It reads a bufferload
 * of data from the client, and places it in the client's command buffer.  If
 * the command is complete, it then tries to invoke the command.
 */

/* ARGSUSED */
static void
tcpReceiveClientInput (clientData, mask)
     ClientData clientData;
     int mask;
{
  register Tcp_ClientData * client = (Tcp_ClientData *) clientData;
  register Tcp_ServerData * server = client -> server;
  register Tcl_Interp * interp = server -> interp;

  static char buffer [BUFSIZ+1];
  int unixStatus;
  char * command;
  int status;
  char * docmd;
  char * argv [3];
  int argc;
  int i;

  /* Try to read from the client */

  errno = 0;
  unixStatus = read (client -> socketfd, buffer, BUFSIZ);
  if (unixStatus <= 0 && errno != EWOULDBLOCK)
    tcpClientReadError (client);

  /* tcpClientReadError closes the client and reports the error.
     In any case, if the read failed, we want to return. */

  if (unixStatus <= 0)
    return;

  if (server -> raw) {
    char buf[512];

    sprintf(buf, "RawInput %s %d %d", client -> name, buffer, unixStatus);
printf("TCP executing: %s\n", buf);
    status = Tcl_Eval (interp, buf, 0, (char * *) NULL);

    tcpPrepareClientForInput (client);

  } else {

    /* Assemble the received data into the buffer */

    buffer [unixStatus] = '\0';
    command = Tcl_AssembleCmd (client -> inputBuffer, buffer);
    if (command != (char *) NULL) {

      /* Process the received command. */

      simpleDeleteFileHandler (client -> socketfd);
      argc = 3;
      argv [0] = client -> name;
      argv [1] = "do";
      argv [2] = command;
      docmd = Tcl_Merge (argc, argv);
      status = Tcl_Eval (interp, docmd, 0, (char * *) NULL);
      free (docmd);

      /* At this point, the client may have been closed.  Don't try to
	 refer to it. */

      if (status != TCL_OK) {
	simpleReportBackgroundError (interp);
      }
    }
  }
}

/* tcpClientReadError --
 *
 *	This procedure is called when an attempt to read the command from a
 * client fails.  There are two possibilities:
 *
 *	The first is that there really was a read error, originating in the
 * socket system.  In this case, the error should be reported at background
 * level, and the client should be closed.
 *
 *	The second is that the read reached the end-of-information marker in
 * the client's stream.  In this case, the `do' command should be called on
 * the client one last time, and then the client should be closed.
 *
 *	If the application needs to clean the client up after a read error,
 * it must define the `tcperror' procedure and process the error.
 */

static void
tcpClientReadError (client)
     Tcp_ClientData * client;
{
  Tcp_ServerData * server = client -> server;
  Tcl_Interp * interp = server -> interp;
  int status;

  if (errno != 0) {

    /* Read error */

    status = Tcl_VarEval (interp, "error {", client -> name, ": read error: ",
			  Tcl_UnixError (interp), "}", (char *) NULL);
    simpleReportBackgroundError (interp);
    
  } else {

    /* End of file */

    status = Tcl_VarEval (interp, client -> name, " do", (char *) NULL);
    if (status != TCL_OK)
      simpleReportBackgroundError (interp);
  }

  tcpCloseClient (client);
}

/* tcpClientWriteError --
 *
 *	This procedure is invoked when an attempt to return results to a client
 * has failed.  It reports the error at background level and closes the client.
 *
 *	If the application needs to clean up the client after a write error,
 * it must define the `tcperror' procedure to catch the error.
 */

static void
tcpClientWriteError (client)
     Tcp_ClientData * client;
{
  Tcp_ServerData * server = client -> server;
  Tcl_Interp * interp = server -> interp;

  (void) Tcl_VarEval (interp, "error {", client -> name, ": read error: ",
			Tcl_UnixError (interp), "}", (char *) NULL);
  simpleReportBackgroundError (interp);
  tcpCloseClient (client);
}

/* tcpSendCmdToServer --
 *
 *	This procedure is invoked to send a command originated by a client
 * using the `$connection send' Tcl command.
 *
 *	The message is passed without a newline appended.  The server requires
 * a newline, which is sent in a separate call.
 */

static int
tcpSendCmdToServer (interp, s, message)
     Tcl_Interp * interp;
     int s;
     char * message;
{
  int length;
  int unixStatus;
  int rubbish;
  static char newline = '\n';
  void (*oldPipeHandler) ();

  /* Set the socket for blocking I/O */

  rubbish = 0;
  unixStatus = ioctl (s, FIONBIO, (char *) &rubbish);
  if (unixStatus < 0) {
    Tcl_AppendResult (interp, "can't set blocking I/O on socket: ",
		      Tcl_UnixError (interp), (char *) NULL);
    return TCL_ERROR;
  }

  /* Keep a possible broken pipe from killing us silently */

  oldPipeHandler = signal (SIGPIPE, SIG_IGN);

  /* Write the message */
  
  length = strlen (message);
  unixStatus = write (s, message, length);
  if (unixStatus < length) {
    (void) signal (SIGPIPE, oldPipeHandler);
    Tcl_AppendResult (interp, "can't send message to server: ",
		      Tcl_UnixError (interp), (char *) NULL);
    return TCL_ERROR;
  }

  /* Write the terminating newline */

  unixStatus = write (s, &newline, 1);
  if (unixStatus < 1) {
    (void) signal (SIGPIPE, oldPipeHandler);
    Tcl_AppendResult (interp, "can't send newline to server: ",
		      Tcl_UnixError (interp), (char *) NULL);
    return TCL_ERROR;
  }

  (void) signal (SIGPIPE, oldPipeHandler);
  return TCL_OK;
}

/*
 * tcpReceiveResultFromServer --
 *
 *	This procedure is invoked to get the result transmitted from
 * a remote server, either on establishing the connection or on processing
 * a command.  It returns a standard Tcl result that is usually the result
 * returned by the server.
 */

static int
tcpReceiveResultFromServer (interp, s)
     Tcl_Interp * interp;
     int s;
{
  int status;
  int unixStatus;
  int junk;
  Tcl_CmdBuf cmdbuf;
  struct timeval tick;
  struct timeval * tickp;
  fd_set readfds;
  char buf [BUFSIZ+1];
  char * reply;
  int rargc;
  char * * rargv;
  int rstatus;

  /* Read the result using non-blocking I/O */

  junk = 1;
  unixStatus = ioctl (s, FIONBIO, (char *) &junk);
  if (unixStatus < 0) {
    Tcl_AppendResult (interp, "can't set nonblocking I/O on socket: ",
		      Tcl_UnixError (interp), (char *) NULL);
    return TCL_ERROR;
  }

  /* Make a buffer to receive the result */

  cmdbuf = Tcl_CreateCmdBuf ();

  /* Wait for the result to appear */

  tickp = (struct timeval *) 0;
  FD_ZERO( &readfds );
  FD_SET( s, &readfds );
  for ( ; ; ) {

    unixStatus = select (s + 1, &readfds, (fd_set *) NULL, (fd_set *) NULL,
			 tickp);

    if (unixStatus < 0) {
      status = TCL_ERROR;
      Tcl_AppendResult (interp, "error selecting socket for reply: ",
			Tcl_UnixError (interp), (char *) NULL);
      break;
    }

    if (unixStatus == 0) {
      status = TCL_ERROR;
      Tcl_SetResult (interp, "timed out waiting for server reply", TCL_STATIC);
      break;
    }

    /* Read the result */

    unixStatus = read (s, buf, BUFSIZ);

    if (unixStatus < 0) {
      status = TCL_ERROR;
      Tcl_AppendResult (interp, "error reading server reply: ",
			Tcl_UnixError (interp), (char *) NULL);
      break;
    }

    if (unixStatus == 0) {
      status = TCL_ERROR;
      Tcl_SetResult (interp, "Connection closed.", TCL_STATIC);
      break;
    }

    /* Parse the (partial) command */
    
    buf [unixStatus] = '\0';
    reply = Tcl_AssembleCmd (cmdbuf, buf);
    if (reply != NULL) {
      status = TCL_OK;
      break;
    }

    /* Partial command not yet complete.  Set timeout for reading the
     * rest of the result. */

    tick.tv_sec = 30;
    tick.tv_usec = 0;
    tickp = &tick;
  }

  /* When we come here, either the status is TCL_ERROR and the error
   * message is already set, or else the status is TCL_OK and `reply'
   * contains the result that we have to return.  The first element of
   * `reply' has the status, and the second has the result string. */

  /* Split the list elements */

  if (status == TCL_OK) {
    status = Tcl_SplitList (interp, reply, &rargc, &rargv);
    if (status != TCL_OK) {
      Tcl_SetResult (interp, "server returned malformed list", TCL_STATIC);
      status = TCL_ERROR;
    }
  }

  /* Verify the element count */

  if (status == TCL_OK) {
    if (rargc != 2) {
      Tcl_SetResult (interp, "server returned malformed list", TCL_STATIC);
      status = TCL_ERROR;
      free ((char *) rargv);
    } else {
      status = Tcl_GetInt (interp, rargv [0], &rstatus);
      if (status != TCL_OK) {
	Tcl_SetResult (interp, "server returned unrecognizable status",
		       TCL_STATIC);
	status = TCL_ERROR;
	free ((char *) rargv);
      }
    }
  }

  /* Return the result reported by the server */

  if (status == TCL_OK) {
    Tcl_SetResult (interp, rargv [1], TCL_VOLATILE);
    status = rstatus;
    free ((char *) rargv);
  }

  Tcl_DeleteCmdBuf (cmdbuf);
  return status;
}
