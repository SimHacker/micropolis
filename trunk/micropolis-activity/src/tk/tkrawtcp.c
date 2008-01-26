/*
 * tkRawTCP.c --
 *
 * 	This file contains a simple Tcl "connect" command
 *	that returns an standard Tcl File descriptor (as would
 *	be returned by Tcl_OpenCmd).
 * Extended to create servers, accept connections, shutdown parts of full
 *   duplex connections and handle UNIX domain sockets.
 *
 * Author: Pekka Nikander <pnr@innopoli.ajk.tele.fi>
 * Modified: Tim MacKenzie <tym@dibbler.cs.monash.edu.au) 
 *
 * Copyright 1992 Telecom Finland
 *
 * Permission to use, copy, modify, and distribute this
 * software and its documentation for any purpose and without
 * fee is hereby granted, provided that this copyright
 * notice appears in all copies.  Telecom Finland
 * makes no representations about the suitability of this
 * software for any purpose.  It is provided "as is" without
 * express or implied warranty.
 *
 * Created: Sun Mar 22 18:20:29 1992
 * based on: Last modified: Sun Mar 22 21:34:31 1992 pnr
 * Last modified: Mon Jun 29 15:25:14 EST 1992 tym
 * 
 */

#ifndef lint
static char rcsid[] = "...";
#endif /* not lint */

#include "tclint.h"
#include "tclunix.h"

#include <assert.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/un.h>

#include <tk.h>

static int inet_connect _ANSI_ARGS_((char *host, char *port,int server));
static int unix_connect _ANSI_ARGS_((char *path, int server));
static void HandleSocket _ANSI_ARGS_ ((ClientData clientData, int mask));

typedef struct {
    Tcl_Interp *interp;
    OpenFile *filePtr;
    char *tclCmd;
    char *fileId;
} FileCmd;

/*
 *------------------------------------------------------------------
 *
 * Tcp_MakeOpenFile --
 *
 *    Set up on OpenFile structure in the interpreter for a newly 
 *      opened file
 *	
 * Results:
 *	none
 *
 * Side effects:
 *	Adds an OpenFile to the list.
 *------------------------------------------------------------------
 */

/* ARGSUSED */
void
Tcp_MakeOpenFile(interp,fd,r,w)
    Tcl_Interp *interp;
    int fd;
    int r,w;
{/* Create an OpenFile structure using f and install it in the interpreter with
  * Readable and Writable set to r and w
  */
    Interp *iPtr = (Interp *) interp;
    register OpenFile *filePtr;

    filePtr = (OpenFile *) ckalloc(sizeof(OpenFile));

    filePtr->f = NULL;
    filePtr->f2 = NULL;

	 /* Open the file with the correct type (doesn't handle !r && !w) */
#ifdef MSDOS
    filePtr->f = fdopen(fd,(r&&w)?"rb+":(r?"rb":"wb"));
#else
    filePtr->f = fdopen(fd,(r&&w)?"r+":(r?"r":"w"));
#endif
	 /* Don't do buffered communication if full-duplex... it breaks! */
	 if (r&w) setbuf(filePtr->f,0);

    filePtr->readable = r;
    filePtr->writable = w;
    filePtr->numPids = 0;
    filePtr->pidPtr = NULL;
    filePtr->errorId = -1;

    /*
     * Enter this new OpenFile structure in the table for the
     * interpreter.  May have to expand the table to do this.
     */

    TclMakeFileTable(iPtr, fd);
    if (iPtr->filePtrArray[fd] != NULL) {
	panic("Tcl_OpenCmd found file already open");
    }
    iPtr->filePtrArray[fd] = filePtr;
}

/*
 *------------------------------------------------------------------
 *
 * Tcp_ConnectCmd --
 *
 *	Open a socket connection to a given host and service.
 *	
 * Results:
 *	A standard Tcl result.
 *
 * Side effects:
 *	An open socket connection.
 *      Sets the global variable connect_info(file%d) to the obtained
 *        port when setting up server.
 *------------------------------------------------------------------
 */

/* ARGSUSED */
int
Tcp_ConnectCmd(notUsed, interp, argc, argv)
    ClientData notUsed;
    Tcl_Interp *interp;
    int argc;
    char **argv;
{
    Interp *iPtr = (Interp *) interp;
    char *host,*port;
    int fd;
    int server=0;
    int unicks = 0;
    
    if (argc != 2 && argc != 3 &&
	(argc != 4 || (argc == 4 && strcmp(argv[1],"-server")))) {
	Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
		     "[{-server}] address_spec\"", (char *) NULL);
	return TCL_ERROR;
    }

    if (!strcmp(argv[1],"-server"))
	server = 1;

    /*
     * Create the connection
     */
    if (argc - server == 2) {/* Unix domain socket */
	unicks = 1;
	fd = unix_connect(argv[1+server],server);
    } else
	fd = inet_connect(argv[1+server], argv[2+server],server);

    if (fd < 0) {
	/* Tell them why it fell apart */
	if (unicks)
	    if (server)
		Tcl_AppendResult(interp,
		    "Couldn't setup listening socket with path \"",
		    argv[1+server],"\" : ",Tcl_UnixError(interp),
		    (char *) NULL);
	    else
		Tcl_AppendResult(interp,
		    "Couldn't connect to \"",argv[1],"\" : ",
		    Tcl_UnixError(interp),(char *) NULL);
	else
	    if (server)
		Tcl_AppendResult(interp,
		    "couldn't setup listening socket on port:",
		    atoi(argv[3])==0?"any":argv[3]," using address \"",
		    strlen(argv[2])?argv[2]:"anywhere.","\": ",
		    Tcl_UnixError(interp), (char *)NULL);
	    else
		Tcl_AppendResult(interp, "couldn't open connection to \"",
				 argv[1], "\" port \"", argv[2], "\": ",
				 Tcl_UnixError(interp), (char *) NULL);
	return TCL_ERROR;
    }

    sprintf(interp->result, "file%d", fd);
    if (server && !unicks) {
	/* Find out what port we got */
	char buf[50];
	struct sockaddr_in sockaddr;
	int res,len=sizeof(sockaddr);
	res =getsockname(fd,(struct sockaddr *) &sockaddr, &len);
	if (res < 0) {
	    sprintf(buf,"%d",errno);
	} else 
	    sprintf(buf,"%d",(int)ntohs(sockaddr.sin_port));
	Tcl_SetVar2(interp,"connect_info",interp->result,buf,TCL_GLOBAL_ONLY);
    }

    Tcp_MakeOpenFile(iPtr,fd,1,1-server);

    return TCL_OK;
}

/*
 *------------------------------------------------------------------
 *
 * Tcp_ShutdownCmd --
 *
 *    Shutdown a socket for reading writing or both using shutdown(2)
 *	
 * Results:
 *	standard tcl result.
 *
 * Side effects:
 *	Modifies the OpenFile structure appropriately
 *------------------------------------------------------------------
 */

/* ARGSUSED */
int
Tcp_ShutdownCmd(notUsed, interp, argc, argv)
    ClientData notUsed;
    Tcl_Interp *interp;
    int argc;
    char **argv;
{
    Interp *iPtr = (Interp *) interp;
    OpenFile *filePtr;
    register FILE *f;
    int fd;

    if (argc != 3) {
    wrong_args:
	Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
		     " fileid <option>\"", (char *) NULL);
	return TCL_ERROR;
    }

    if (TclGetOpenFile(interp, argv[1], &filePtr) != TCL_OK) {
        return TCL_ERROR;
    }

    f = filePtr->f;
    fd = fileno(filePtr->f);
    if (!strcmp(argv[2],"0") || !strcmp(argv[2],"receives") || 
	    !strcmp(argv[2],"read")) {
	if (!filePtr->readable) {
	    Tcl_AppendResult(interp, "File is not readable",(char *) NULL);
	    return TCL_ERROR;
	}
	if (shutdown(fd,0)) {
	    Tcl_AppendResult(interp, "shutdown: ", Tcl_UnixError(interp),
		(char *) NULL);
	    return TCL_ERROR;
	}
	filePtr->readable=0;
    } else if (!strcmp(argv[2],"1") || !strcmp(argv[2],"sends") ||
	    !strcmp(argv[2],"write")) {
	if (!filePtr->writable) {
	    Tcl_AppendResult(interp, "File is not writable",(char *) NULL);
	    return TCL_ERROR;
	}
	if (shutdown(fd,1)) {
	    Tcl_AppendResult(interp, "shutdown: ", Tcl_UnixError(interp),
		(char *) NULL);
	    return TCL_ERROR;
	}
	filePtr->writable=0;
    } else if (!strcmp(argv[2],"2") || !strcmp(argv[2],"all") ||
	    !strcmp(argv[2],"both")) {
	if (shutdown(fd,2)) {
	    Tcl_AppendResult(interp, "shutdown: ", Tcl_UnixError(interp),
		(char *) NULL);
	    return TCL_ERROR;
	}
	filePtr->writable=0;
	filePtr->readable=0;
    } else
	goto wrong_args;
    return TCL_OK;
}
	

		
/*
 *------------------------------------------------------------------
 *
 * Tcp_AcceptCmd --
 *
 *    Accept a connection on a listening socket
 *	
 * Results:
 *	a standard tcl result
 *
 * Side effects:
 *	Opens a new file.
 *      Sets the global variable connect_info(file%d) to a list
 *         containing the remote address (host ip, port) of the
 *         connector.
 *------------------------------------------------------------------
 */

/* ARGSUSED */
int
Tcp_AcceptCmd(notUsed, interp, argc, argv)
    ClientData notUsed;
    Tcl_Interp *interp;
    int argc;
    char **argv;
{
    Interp *iPtr = (Interp *) interp;
    struct sockaddr_in sockaddr;
    int len = sizeof sockaddr;
    OpenFile *filePtr;
    register FILE *f;
    int fd;

    if (argc != 2) {
	Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
		     " listening_socket\"", (char *) NULL);
	return TCL_ERROR;
    }

    if (TclGetOpenFile(interp, argv[1], &filePtr) != TCL_OK) {
        return TCL_ERROR;
    }
    if (!filePtr->readable) {
        Tcl_AppendResult(interp, "\"", argv[1],
                "\" wasn't opened for reading", (char *) NULL);
        return TCL_ERROR;
    }

    f = filePtr->f;
    fd = fileno(filePtr->f);

    fd = accept(fd,(struct sockaddr *)&sockaddr,&len);
    if (fd < 0) {
	Tcl_AppendResult(interp, "system error in accept()", (char *)NULL);
	return TCL_ERROR;
    }

    {/* Set the global connect_info */
	char buf[100];
	char nm[10];
	if (sockaddr.sin_family == AF_INET)
	    sprintf(buf,"%s %d",inet_ntoa(sockaddr.sin_addr),
		   ntohs(sockaddr.sin_port));
	else
	    buf[0]=0;  /* Empty string for UNIX domain sockets */
	sprintf(nm,"file%d",fd);
	Tcl_SetVar2(interp,"connect_info",nm,buf,TCL_GLOBAL_ONLY);
    }

    /*
     * Create the FILE*
     */
    Tcp_MakeOpenFile(iPtr,fd,1,1);

    sprintf(interp->result, "file%d", fd);
    return TCL_OK;
}

/*
 *----------------------------------------------------------------
 *
 * unix_connect --
 *
 * 	Create a (unix_domain) fd connection using given rendeavous
 *
 * Results:
 *	An open fd or -1.
 *
 * Side effects:
 * 	None.
 *----------------------------------------------------------------
 */

static int
unix_connect(path,server)
    char *path;		/* Path name to create or use */
    int  server;        /* 1->make server, 0->connect to server */
{
    struct sockaddr_un sockaddr;
    int sock, status;
    extern int errno;
    
    sock = socket(PF_UNIX, SOCK_STREAM, 0);
    if (sock < 0) {
	return -1;
    }
    
    sockaddr.sun_family = AF_UNIX;
    strncpy(sockaddr.sun_path,path,sizeof(sockaddr.sun_path)-1);
    sockaddr.sun_path[sizeof(sockaddr.sun_path)-1] = 0; /* Just in case */
    
    if (server)
	status = bind(sock,(struct sockaddr *) &sockaddr, sizeof(sockaddr));
    else
	status = connect(sock, (struct sockaddr *) &sockaddr, sizeof(sockaddr));
    
    if (status < 0) {
	close (sock);
	return -1;
    }

    if (server) {
	listen(sock,5);
	return sock;
    }
    
    return sock;
}

/*
 *----------------------------------------------------------------
 *
 * inet_connect --
 *
 * 	Create a (inet domain) fd connection to given host and port.
 *
 * Results:
 *	An open fd or -1.
 *
 * Side effects:
 * 	None.
 *----------------------------------------------------------------
 */

static int
inet_connect(host, service,server)
    char *host;			/* Host to connect, name or IP address */
    char *service;		/* Port to use, service name or port number */
    int  server;
{
    struct hostent *hostent, _hostent;
    struct servent *servent, _servent;
    struct protoent *protoent;
    struct sockaddr_in sockaddr;
    int sock, status;
    int hostaddr, hostaddrPtr[2];
    int servport;
    extern int errno;
    
    hostent = gethostbyname(host);
    if (hostent == NULL) {
	hostaddr = inet_addr(host);
	if (hostaddr == -1) {
	    if (server && !strlen(host)) 
		hostaddr = INADDR_ANY;
	    else {
		errno = EINVAL;
		return -1;
	    }
	}	
	_hostent.h_addr_list = (char **)hostaddrPtr;
	_hostent.h_addr_list[0] = (char *)&hostaddr;
	_hostent.h_addr_list[1] = NULL;
	_hostent.h_length = sizeof(hostaddr);
	_hostent.h_addrtype = AF_INET;
	hostent = &_hostent;
    }
    servent = getservbyname(service, "tcp");
    if (servent == NULL) {
	servport = htons(atoi(service));
	if (servport == -1) { 
	    errno = EINVAL;
	    return -1;
	}
	_servent.s_port = servport;
	_servent.s_proto = "tcp";
	servent = &_servent;
    }
    protoent = getprotobyname(servent->s_proto);
    if (protoent == NULL) {
	errno = EINVAL;
	return -1;
    }
    
    sock = socket(PF_INET, SOCK_STREAM, protoent->p_proto);
    if (sock < 0) {
	return -1;
    }
    
    sockaddr.sin_family = AF_INET;
    memcpy((char *)&(sockaddr.sin_addr.s_addr),
	   (char *) hostent->h_addr_list[0],
	   (size_t) hostent->h_length);
    sockaddr.sin_port = servent->s_port;
    
    if (server)
	status = bind(sock,(struct sockaddr *) &sockaddr, sizeof(sockaddr));
    else
	status = connect(sock, (struct sockaddr *) &sockaddr, sizeof(sockaddr));
    
    if (status < 0) {
	close (sock);
	return -1;
    }

    if (server) {
	listen(sock,5);
	return sock;
    }
    
    return sock;
}

/*
 *----------------------------------------------------------------
 *
 * Tcp_FileHandlerCmd --
 *
 * 	Register a file handler with an open file.  If there is
 *	already and existing handler, it will be no longer called.
 *	If no mask and command are given, any existing handler
 *	will be deleted.
 *
 * Results:
 *	A standard Tcl result. (Always OK).
 *
 * Side effects:
 *	A new file handler is associated with a give TCL open file.
 *	Whenever the file is readable, writeable and/or there is
 *	an expection condition on the file, a user supplied TCL
 *	command is called.
 *
 *----------------------------------------------------------------
 */

/* ARGSUSED */
int
Tcp_FileHandlerCmd(notUsed, interp, argc, argv)
    ClientData notUsed;
    Tcl_Interp *interp;
    int argc;
    char **argv;
{
    FileCmd *cmdPtr;
    OpenFile *filePtr;
    int mask;

    if (argc != 2 && argc != 4) {
	Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
		  " fileId ?mode command?\"", (char *) NULL);
	return TCL_ERROR;
    }

    if (TclGetOpenFile(interp, argv[1], &filePtr) != TCL_OK) {
	return TCL_ERROR;
    }

    if (argc == 2) {
	/*
	 * NOTE!  Currently the cmdPtr structure will be left
	 * 	  *unfreed* if the file handler is deleted
	 *	  via this code.  Tough.  Would need a hash table
	 *	  or something...
	 */
	Tk_DeleteFileHandler(fileno(filePtr->f));
	return TCL_OK;
    }
    
    /*
     * Find out on what situations the user is interested in.
     * This is not the most elegant or efficient way to do this,
     * but who cares?  (I do, but not much enough :-)
     */
    mask = 0;
    if (strchr(argv[2], 'r')) {
	mask |= TK_READABLE;
    }
    if (strchr(argv[2], 'w')) {
	mask |= TK_WRITABLE;
    }
    if (strchr(argv[2], 'e')) {
	mask |= TK_EXCEPTION;
    }
    if (mask == 0 || (strlen(argv[2]) != strspn(argv[2], "rwe"))) {
	Tcl_AppendResult(interp, "bad mask argument \"", argv[2],
		 "\": should be any combination of \"r\", \"w\" and \"e\"",
			 (char *) NULL);
	fclose(filePtr->f);
	return TCL_ERROR;
    }

    cmdPtr = (FileCmd *)ckalloc(sizeof(FileCmd));
    cmdPtr->interp = interp;
    cmdPtr->filePtr = filePtr;
    cmdPtr->tclCmd = ckalloc(strlen(argv[3]) + 1);
    strcpy(cmdPtr->tclCmd, argv[3]);
    cmdPtr->fileId = ckalloc(strlen(argv[1]) + 1);
    strcpy(cmdPtr->fileId, argv[1]);
    
    /*
     * NOTE! There may be an earlier file handler.   Should do something.
     */
    Tk_CreateFileHandler(fileno(filePtr->f), mask, HandleSocket,
			 (ClientData) cmdPtr);

    return TCL_OK;
}
/*
 *----------------------------------------------------------------
 *
 * HandleSocket --
 *
 * 	This procedure is called from Tk_DoOneEvent whenever there is
 *	a desired condition on a given open socket.  An Tcl command
 *	given by the user is executed to handle the connection.  If
 *	and EOF or ERROR condition is noticed, all memory resources
 *	associated with the socket are released and the socket is closed.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The user supplied command can do anything.
 *
 *----------------------------------------------------------------
 */

static void
HandleSocket(clientData, mask)
    ClientData clientData;
    int mask;
{
    int result;
    FileCmd *cmdPtr = (FileCmd *) clientData;
    OpenFile *filePtr = cmdPtr->filePtr;
    Tcl_Interp *interp = cmdPtr->interp;
    OpenFile *dummy;
    int delete;
    int fd = fileno(filePtr->f);

    Tk_Preserve((ClientData)cmdPtr);

    delete = 0;
    if (TclGetOpenFile(interp, cmdPtr->fileId, &dummy) != TCL_OK) {
	/* File is closed! */
	Tcl_ResetResult(interp);
	delete = 1;
    } else {
	assert(dummy == cmdPtr->filePtr);

	if (mask & TK_READABLE) {
	    result = Tcl_VarEval(interp, cmdPtr->tclCmd, " r ", cmdPtr->fileId,
				 (char *) NULL);
	    if (result != TCL_OK) {
		TkBindError(interp);
	    }
	}
	if (mask & TK_WRITABLE) {
	    result = Tcl_VarEval(interp, cmdPtr->tclCmd, " w ", cmdPtr->fileId,
				 (char *) NULL);
	    if (result != TCL_OK) {
		TkBindError(interp);
	    }
	}
	if (mask & TK_EXCEPTION) {
	    result = Tcl_VarEval(interp, cmdPtr->tclCmd, " e ", cmdPtr->fileId,
				 (char *) NULL);
	    if (result != TCL_OK) {
		TkBindError(interp);
	    }
	}

	if (feof(filePtr->f) || ferror(filePtr->f)) {
	    result = Tcl_VarEval(interp, "close ", cmdPtr->fileId,
				 (char *) NULL);
	    if (result != TCL_OK) {
		TkBindError(interp);
	    }
	    delete = 1;
	}
    }

    Tk_Release((ClientData)cmdPtr);

    if (delete) {
	Tk_DeleteFileHandler(fd);
	Tk_EventuallyFree((ClientData)cmdPtr, (Tk_FreeProc *)free);
    }
}
