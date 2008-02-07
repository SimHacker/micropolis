/* 
 * tkSend.c --
 *
 *	This file provides procedures that implement the "send"
 *	command, allowing commands to be passed from interpreter
 *	to interpreter.
 *
 * Copyright 1989-1992 Regents of the University of California
 * Permission to use, copy, modify, and distribute this
 * software and its documentation for any purpose and without
 * fee is hereby granted, provided that the above copyright
 * notice appear in all copies.  The University of California
 * makes no representations about the suitability of this
 * software for any purpose.  It is provided "as is" without
 * express or implied warranty.
 */

#ifndef lint
static char rcsid[] = "$Header: /user6/ouster/wish/RCS/tkSend.c,v 1.26 92/08/13 10:29:26 ouster Exp $ SPRITE (Berkeley)";
#endif

#include "tkconfig.h"
#include "tkint.h"

/* 
 * The following structure is used to keep track of the
 * interpreters registered by this process.
 */

typedef struct RegisteredInterp {
    char *name;			/* Interpreter's name (malloc-ed). */
    Tcl_Interp *interp;		/* Interpreter associated with
				 * name. */
    TkDisplay *dispPtr;		/* Display associated with name. */
    TkWindow *winPtr;		/* Window associated with name. */
    struct RegisteredInterp *nextPtr;
				/* Next in list of names associated
				 * with interps in this process.
				 * NULL means end of list. */
} RegisteredInterp;

static RegisteredInterp *registry = NULL;
				/* List of all interpreters
				 * registered by this process. */

/*
 * When a result is being awaited from a sent command, one of
 * the following structures is present on a list of all outstanding
 * sent commands.  The information in the structure is used to
 * process the result when it arrives.  You're probably wondering
 * how there could ever be multiple outstanding sent commands.
 * This could happen if interpreters invoke each other recursively.
 * It's unlikely, but possible.
 */

typedef struct PendingCommand {
    int serial;			/* Serial number expected in
				 * result. */
    char *target;		/* Name of interpreter command is
				 * being sent to. */
    Tcl_Interp *interp;		/* Interpreter from which the send
				 * was invoked. */
    int code;			/* Tcl return code for command
				 * will be stored here. */
    char *result;		/* String result for command (malloc'ed).
				 * NULL means command still pending. */
    struct PendingCommand *nextPtr;
				/* Next in list of all outstanding
				 * commands.  NULL means end of
				 * list. */
} PendingCommand;

static PendingCommand *pendingCommands = NULL;
				/* List of all commands currently
				 * being waited for. */

/*
 * The information below is used for communication between
 * processes during "send" commands.  Each process keeps a
 * private window, never even mapped, with one property,
 * "Comm".  When a command is sent to an interpreter, the
 * command is appended to the comm property of the communication
 * window associated with the interp's process.  Similarly, when a
 * result is returned from a sent command, it is also appended
 * to the comm property.  In each case, the property information
 * is in the form of an ASCII string.  The exact syntaxes are:
 *
 * Command:
 *	'C' space window space serial space interpName '|' command '\0'
 * The 'C' character indicates that this is a command and not
 * a response.  Window is the hex identifier for the comm
 * window on which to append the response.  Serial is a hex
 * integer containing an identifying number assigned by the
 * sender;  it may be used by the sender to sort out concurrent
 * responses.  InterpName is the ASCII name of the desired
 * interpreter, which must not contain any vertical bar characters
 * The interpreter name is delimited by a vertical bar (this
 * allows the name to include blanks), and is followed by
 * the command to execute.  The command is terminated by a
 * NULL character.
 *
 * Response:
 *	'R' space serial space code space result '\0'
 * The 'R' character indicates that this is a response.  Serial
 * gives the identifier for the command (same value as in the
 * command message).  The code field is a decimal integer giving
 * the Tcl return code from the command, and result is the string
 * result.  The result is terminated by a NULL character.
 *
 * The register of interpreters is kept in a property
 * "InterpRegistry" on the root window of the display.  It is
 * organized as a series of zero or more concatenated strings
 * (in no particular order), each of the form
 * 	window space name '\0'
 * where "window" is the hex id of the comm. window to use to talk
 * to an interpreter named "name".
 */

/*
 * Maximum size property that can be read at one time by
 * this module:
 */

#define MAX_PROP_WORDS 100000

/*
 * Forward declarations for procedures defined later in this file:
 */

static int	AppendErrorProc _ANSI_ARGS_((ClientData clientData,
			XErrorEvent *errorPtr));
static void	AppendPropCarefully _ANSI_ARGS_((Display *display,
		    Window window, Atom property, char *value,
		    PendingCommand *pendingPtr));
static void	DeleteProc _ANSI_ARGS_((ClientData clientData));
static Window	LookupName _ANSI_ARGS_((TkDisplay *dispPtr, char *name,
		    int delete));
static void	SendEventProc _ANSI_ARGS_((ClientData clientData,
		    XEvent *eventPtr));
static int	SendInit _ANSI_ARGS_((Tcl_Interp *interp, TkDisplay *dispPtr));
static Bool	SendRestrictProc _ANSI_ARGS_((Display *display,
		    XEvent *eventPtr, char *arg));
static void	TimeoutProc _ANSI_ARGS_((ClientData clientData));

/*
 *--------------------------------------------------------------
 *
 * Tk_RegisterInterp --
 *
 *	This procedure is called to associate an ASCII name
 *	with an interpreter.  Tk_InitSend must previously
 *	have been called to set up communication channels
 *	and specify a display.
 *
 * Results:
 *	Zero is returned if the name was registered successfully.
 *	Non-zero means the name was already in use.
 *
 * Side effects:
 *	Registration info is saved, thereby allowing the
 *	"send" command to be used later to invoke commands
 *	in the interpreter.  The registration will be removed
 *	automatically when the interpreter is deleted.
 *
 *--------------------------------------------------------------
 */

int
Tk_RegisterInterp(interp, name, tkwin)
    Tcl_Interp *interp;		/* Interpreter associated with name. */
    char *name;			/* The name that will be used to
				 * refer to the interpreter in later
				 * "send" commands.  Must be globally
				 * unique. */
    Tk_Window tkwin;		/* Token for window associated with
				 * interp;  used to identify display
				 * for communication.  */
{
#define TCL_MAX_NAME_LENGTH 1000
    char propInfo[TCL_MAX_NAME_LENGTH + 20];
    register RegisteredInterp *riPtr;
    Window w;
    TkWindow *winPtr = (TkWindow *) tkwin;
    TkDisplay *dispPtr;

    if (strchr(name, '|') != NULL) {
	interp->result =
		"interpreter name cannot contain '|' character";
	return TCL_ERROR;
    }

    dispPtr = winPtr->dispPtr;
    if (dispPtr->commWindow == NULL) {
	int result;

	result = SendInit(interp, dispPtr);
	if (result != TCL_OK) {
	    return result;
	}
    }

    /*
     * Make sure the name is unique, and append info about it to
     * the registry property.  Eventually, it would probably be
     * a good idea to lock the server here to prevent conflicting
     * changes to the registry property.  But that would make
     * testing more difficult, and probably isn't necessary
     * anyway because new windows don't get created all that often.
     */

    w = LookupName(dispPtr, name, 0);
    if (w != (Window) 0) {
	Tcl_Interp *tmpInterp;
	RegisteredInterp tmpRi;
	int result;
	char *argv[3];

	/*
	 * Name already exists.  Ping the interpreter with a
	 * NULL command to see if it already exists.  If not,
	 * unregister the old name (this could happen if an
	 * application dies without cleaning up the registry).
	 */

	tmpInterp = Tcl_CreateInterp();
	argv[0] = "send";
	argv[1] = name;
	argv[2] = "";
	tmpRi.dispPtr = dispPtr;
	tmpRi.winPtr = winPtr;
	result = Tk_SendCmd((ClientData) &tmpRi, tmpInterp, 3, argv);
	Tcl_DeleteInterp(tmpInterp);
	if (result == TCL_OK) {
	    Tcl_AppendResult(interp, "interpreter name \"", name,
		    "\" is already in use", (char *) NULL);
	    return TCL_ERROR;
	}
	(void) LookupName(winPtr->dispPtr, name, 1);
    }
    sprintf(propInfo, "%x %.*s", Tk_WindowId(dispPtr->commWindow),
	    TCL_MAX_NAME_LENGTH, name);
    XChangeProperty(dispPtr->display,
	    Tk_DefaultRootWindow(dispPtr->display),
	    dispPtr->registryProperty, XA_STRING, 8, PropModeAppend,
	    (unsigned char *) propInfo, strlen(propInfo)+1);

    /*
     * Add an entry in the local registry of names owned by this
     * process.
     */

    riPtr = (RegisteredInterp *) ckalloc(sizeof(RegisteredInterp));
    riPtr->name = (char *) ckalloc((unsigned) (strlen(name) + 1));
    strcpy(riPtr->name, name);
    riPtr->interp = interp;
    riPtr->dispPtr = dispPtr;
    riPtr->winPtr = winPtr;
    riPtr->nextPtr = registry;
    registry = riPtr;

    /*
     * Add the "send" command to this interpreter, and arrange for
     * us to be notified when the interpreter is deleted (actually,
     * when the "send" command is deleted).
     */

    Tcl_CreateCommand(interp, "send", Tk_SendCmd, (ClientData) riPtr,
	    DeleteProc);

    return TCL_OK;
}

/*
 *--------------------------------------------------------------
 *
 * Tk_SendCmd --
 *
 *	This procedure is invoked to process the "send" Tcl command.
 *	See the user documentation for details on what it does.
 *
 * Results:
 *	A standard Tcl result.
 *
 * Side effects:
 *	See the user documentation.
 *
 *--------------------------------------------------------------
 */

int
Tk_SendCmd(clientData, interp, argc, argv)
    ClientData clientData;		/* Information about sender (only
					 * dispPtr field is used). */
    Tcl_Interp *interp;			/* Current interpreter. */
    int argc;				/* Number of arguments. */
    char **argv;			/* Argument strings. */
{
    RegisteredInterp *senderRiPtr = (RegisteredInterp *) clientData;
    Window w;
#define STATIC_PROP_SPACE 100
    char *property, staticSpace[STATIC_PROP_SPACE];
    int length;
    int quick = 0;
    static int serial = 0;	/* Running count of sent commands.
				 * Used to give each command a
				 * different serial number. */
    PendingCommand pending;
    Tk_TimerToken timeout;
    register RegisteredInterp *riPtr;
    char *cmd;
    int result;
    Bool (*prevRestrictProc)();
    char *prevArg;
    TkWindow *winPtr = senderRiPtr->winPtr;
    TkDisplay *dispPtr = senderRiPtr->dispPtr;
    int to_server = 0;

    if (argc >= 2) {
      Tk_Window tkwin = NULL;

      if ((argv[1][0] == '-') &&
	  (strncmp(argv[1], "-quick", strlen(argv[1])) == 0)) {
	quick = 1;
	argv += 1; argc -= 1;
      }
    }

    if (argc >= 3) {
      Tk_Window tkwin = NULL;

      if ((argv[1][0] == '-') &&
	  (strncmp(argv[1], "-server", strlen(argv[1])) == 0)) {
	to_server = 1;
	tkwin = Tk_NameToWindow(interp, argv[2], (Tk_Window) winPtr);
	if (tkwin == NULL) {
	  Tcl_AppendResult(interp, "bad server arg, should be window name: ",
			   argv[2], (char *) NULL);
	  return TCL_ERROR;
	}
	winPtr = (TkWindow *) tkwin;
	dispPtr = winPtr->dispPtr;
	argv += 2; argc -= 2;
      }
    }

    if (dispPtr->commWindow == NULL) {
	result = SendInit(interp, dispPtr);
	if (result != TCL_OK) {
	    return result;
	}
    }

    if (argc < 3) {
    badargs:
	Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
		" interpName arg ?arg ...?\"", (char *) NULL);
	return TCL_ERROR;
    }

    if (argc == 3) {
	cmd = argv[2];
    } else {
	cmd = Tcl_Concat(argc-2, argv+2);
    }

    /*
     * See if the target interpreter is local.  If so, execute
     * the command directly without going through the X server.
     * The only tricky thing is passing the result from the target
     * interpreter to the invoking interpreter.  Watch out:  they
     * could be the same!
     */

    for (riPtr = registry; riPtr != NULL; riPtr = riPtr->nextPtr) {
	if (strcmp(riPtr->name, argv[1]) != 0) {
	    continue;
	}
	if (interp == riPtr->interp) {
	    result = Tcl_GlobalEval(interp, cmd);
	} else {
	    result = Tcl_GlobalEval(riPtr->interp, cmd);
	    interp->result = riPtr->interp->result;
	    interp->freeProc = riPtr->interp->freeProc;
	    riPtr->interp->freeProc = 0;
	    Tcl_ResetResult(riPtr->interp);
	}
	if (cmd != argv[2]) {
	    ckfree(cmd);
	}
	return result;
    }

    /*
     * Bind the interpreter name to a communication window.
     */

    w = LookupName(dispPtr, argv[1], 0);
    if (w == 0) {
	Tcl_AppendResult(interp, "no registered interpeter named \"",
		argv[1], "\"", (char *) NULL);
	if (cmd != argv[2]) {
	    ckfree(cmd);
	}
	return TCL_ERROR;
    }

    if (!quick) {
	/*
	 * Register the fact that we're waiting for a command to
	 * complete (this is needed by SendEventProc and by
	 * AppendErrorProc to pass back the command's results).
	 */

	serial++;
	pending.serial = serial;
	pending.target = argv[1];
	pending.interp = interp;
	pending.result = NULL;
	pending.nextPtr = pendingCommands;
	pendingCommands = &pending;
    }

    /*
     * Send the command to target interpreter by appending it to the
     * comm window in the communication window.
     */

    length = strlen(argv[1]) + strlen(cmd) + 30;
    if (length <= STATIC_PROP_SPACE) {
	property = staticSpace;
    } else {
	property = (char *) ckalloc((unsigned) length);
    }
    sprintf(property, "C %x %x %s|%s",
	    Tk_WindowId(dispPtr->commWindow), serial, argv[1], cmd);
    (void) AppendPropCarefully(dispPtr->display, w, dispPtr->commProperty,
	    property, &pending);
    if (length > STATIC_PROP_SPACE) {
	ckfree(property);
    }
    if (cmd != argv[2]) {
	ckfree(cmd);
    }

    if (quick) {
      sprintf(interp->result, "NoReturnValue");
      return TCL_OK;
    }

    /*
     * Enter a loop processing X events until the result comes
     * in.  If no response is received within a few seconds,
     * then timeout.  While waiting for a result, look only at
     * send-related events (otherwise it would be possible for
     * additional input events, such as mouse motion, to cause
     * other sends, leading eventually to such a large number
     * of nested Tcl_Eval calls that the Tcl interpreter panics).
     */

    prevRestrictProc = Tk_RestrictEvents(SendRestrictProc,
	    (char *) dispPtr->commWindow, &prevArg);
    timeout = Tk_CreateTimerHandler(5000, TimeoutProc,
	    (ClientData) &pending);
    while (pending.result == NULL) {
	Tk_DoOneEvent(0);
    }
    Tk_DeleteTimerHandler(timeout);
    (void) Tk_RestrictEvents(prevRestrictProc, prevArg, &prevArg);

    /*
     * Unregister the information about the pending command
     * and return the result.
     */

    if (pendingCommands == &pending) {
	pendingCommands = pending.nextPtr;
    } else {
	PendingCommand *pcPtr;

	for (pcPtr = pendingCommands; pcPtr != NULL;
		pcPtr = pcPtr->nextPtr) {
	    if (pcPtr->nextPtr == &pending) {
		pcPtr->nextPtr = pending.nextPtr;
		break;
	    }
	}
    }
    Tcl_SetResult(interp, pending.result, TCL_DYNAMIC);
    return pending.code;
    
}

/*
 *----------------------------------------------------------------------
 *
 * TkGetInterpNames --
 *
 *	This procedure is invoked to fetch a list of all the
 *	interpreter names currently registered for the display
 *	of a particular window.
 *
 * Results:
 *	A standard Tcl return value.  Interp->result will be set
 *	to hold a list of all the interpreter names defined for
 *	tkwin's display.  If an error occurs, then TCL_ERROR
 *	is returned and interp->result will hold an error message.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

int
TkGetInterpNames(interp, tkwin)
    Tcl_Interp *interp;		/* Interpreter for returning a result. */
    Tk_Window tkwin;		/* Window whose display is to be used
				 * for the lookup. */
{
    TkDisplay *dispPtr = ((TkWindow *) tkwin)->dispPtr;
    char *regProp, *separator, *name;
    register char *p;
    int result, actualFormat;
    unsigned long numItems, bytesAfter;
    Atom actualType;

    /*
     * Read the registry property.
     */

    regProp = NULL;
    result = XGetWindowProperty(dispPtr->display,
	    Tk_DefaultRootWindow(dispPtr->display),
	    dispPtr->registryProperty, 0, MAX_PROP_WORDS,
	    False, XA_STRING, &actualType, &actualFormat,
	    &numItems, &bytesAfter, (unsigned char **) &regProp);

    if (actualType == None) {
	sprintf(interp->result, "couldn't read intepreter registry property");
	return TCL_ERROR;
    }

    /*
     * If the property is improperly formed, then delete it.
     */

    if ((result != Success) || (actualFormat != 8)
	    || (actualType != XA_STRING)) {
	if (regProp != NULL) {
	    XFree(regProp);
	}
	sprintf(interp->result, "intepreter registry property is badly formed");
	return TCL_ERROR;
    }

    /*
     * Scan all of the names out of the property.
     */

    separator = "";
    for (p = regProp; (p-regProp) < numItems; p++) {
	name = p;
	while ((*p != 0) && (!isspace(*p))) {
	    p++;
	}
	if (*p != 0) {
	    name = p+1;
	    name = Tcl_Merge(1, &name);
	    Tcl_AppendResult(interp, separator, name, (char *) NULL);
	    while (*p != 0) {
		p++;
	    }
	    separator = " ";
	}
    }
    XFree(regProp);
    return TCL_OK;
}

/*
 *--------------------------------------------------------------
 *
 * SendInit --
 *
 *	This procedure is called to initialize the
 *	communication channels for sending commands and
 *	receiving results.
 *
 * Results:
 *	The result is a standard Tcl return value, which is
 *	normally TCL_OK.  If an error occurs then an error
 *	message is left in interp->result and TCL_ERROR is
 *	returned.
 *
 * Side effects:
 *	Sets up various data structures and windows.
 *
 *--------------------------------------------------------------
 */

static int
SendInit(interp, dispPtr)
    Tcl_Interp *interp;		/* Interpreter to use for error
				 * reporting. */
    register TkDisplay *dispPtr;/* Display to initialize. */

{
    XSetWindowAttributes atts;

    /*
     * Create the window used for communication, and set up an
     * event handler for it.
     */

    dispPtr->commWindow = Tk_CreateWindow(interp, (Tk_Window) NULL,
	    "_comm", DisplayString(dispPtr->display));
    if (dispPtr->commWindow == NULL) {
	return TCL_ERROR;
    }
    atts.override_redirect = True;
    Tk_ChangeWindowAttributes(dispPtr->commWindow,
	    CWOverrideRedirect, &atts);
    Tk_CreateEventHandler(dispPtr->commWindow, PropertyChangeMask,
	    SendEventProc, (ClientData) dispPtr);
    Tk_MakeWindowExist(dispPtr->commWindow);

    /*
     * Get atoms used as property names.
     */

    dispPtr->commProperty = XInternAtom(dispPtr->display,
	    "Comm", False);
    dispPtr->registryProperty = XInternAtom(dispPtr->display,
	    "InterpRegistry", False);
    return TCL_OK;
}

/*
 *--------------------------------------------------------------
 *
 * LookupName --
 *
 *	Given an interpreter name, see if the name exists in
 *	the interpreter registry for a particular display.
 *
 * Results:
 *	If the given name is registered, return the ID of
 *	the window associated with the name.  If the name
 *	isn't registered, then return 0.
 *
 * Side effects:
 *	If the registry property is improperly formed, then
 *	it is deleted.  If "delete" is non-zero, then if the
 *	named interpreter is found it is removed from the
 *	registry property.
 *
 *--------------------------------------------------------------
 */

static Window
LookupName(dispPtr, name, delete)
    register TkDisplay *dispPtr;
			/* Display whose registry to check. */
    char *name;		/* Name of an interpreter. */
    int delete;		/* If non-zero, delete info about name. */
{
    char *regProp, *entry;
    register char *p;
    int result, actualFormat;
    unsigned long numItems, bytesAfter;
    Atom actualType;
    Window returnValue;

    /*
     * Read the registry property.
     */

    regProp = NULL;
    result = XGetWindowProperty(dispPtr->display,
	    Tk_DefaultRootWindow(dispPtr->display),
	    dispPtr->registryProperty, 0, MAX_PROP_WORDS,
	    False, XA_STRING, &actualType, &actualFormat,
	    &numItems, &bytesAfter, (unsigned char **) &regProp);

    if (actualType == None) {
	return 0;
    }

    /*
     * If the property is improperly formed, then delete it.
     */

    if ((result != Success) || (actualFormat != 8)
	    || (actualType != XA_STRING)) {
	if (regProp != NULL) {
	    XFree(regProp);
	}
	XDeleteProperty(dispPtr->display,
		Tk_DefaultRootWindow(dispPtr->display),
		dispPtr->registryProperty);
	return 0;
    }

    /*
     * Scan the property for the desired name.
     */

    returnValue = (Window) 0;
    entry = NULL;	/* Not needed, but eliminates compiler warning. */
    for (p = regProp; (p-regProp) < numItems; ) {
	entry = p;
	while ((*p != 0) && (!isspace(*p))) {
	    p++;
	}
	if ((*p != 0) && (strcmp(name, p+1) == 0)) {
	    sscanf(entry, "%x", &returnValue);
	    break;
	}
	while (*p != 0) {
	    p++;
	}
	p++;
    }

    /*
     * Delete the property, if that is desired (copy down the
     * remainder of the registry property to overlay the deleted
     * info, then rewrite the property).
     */

    if ((delete) && (returnValue != 0)) {
	int count;

	while (*p != 0) {
	    p++;
	}
	p++;
	count = numItems - (p-regProp);
	if (count > 0) {
	    memcpy((VOID *) entry, (VOID *) p, count);
	}
	XChangeProperty(dispPtr->display,
		Tk_DefaultRootWindow(dispPtr->display),
		dispPtr->registryProperty, XA_STRING, 8,
		PropModeReplace, (unsigned char *) regProp,
		(int) (numItems - (p-entry)));
	XSync(dispPtr->display, False);
    }

    XFree(regProp);
    return returnValue;
}

/*
 *--------------------------------------------------------------
 *
 * SendEventProc --
 *
 *	This procedure is invoked automatically by the toolkit
 *	event manager when a property changes on the communication
 *	window.  This procedure reads the property and handles
 *	command requests and responses.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	If there are command requests in the property, they
 *	are executed.  If there are responses in the property,
 *	their information is saved for the (ostensibly waiting)
 *	"send" commands. The property is deleted.
 *
 *--------------------------------------------------------------
 */

static void
SendEventProc(clientData, eventPtr)
    ClientData clientData;	/* Display information. */	
    XEvent *eventPtr;		/* Information about event. */
{
    TkDisplay *dispPtr = (TkDisplay *) clientData;
    char *propInfo;
    register char *p;
    int result, actualFormat;
    unsigned long numItems, bytesAfter;
    Atom actualType;

    if ((eventPtr->xproperty.atom != dispPtr->commProperty)
	    || (eventPtr->xproperty.state != PropertyNewValue)) {
	return;
    }

    /*
     * Read the comm property and delete it.
     */

    propInfo = NULL;
    result = XGetWindowProperty(dispPtr->display,
	    Tk_WindowId(dispPtr->commWindow),
	    dispPtr->commProperty, 0, MAX_PROP_WORDS, True,
	    XA_STRING, &actualType, &actualFormat,
	    &numItems, &bytesAfter, (unsigned char **) &propInfo);

    /*
     * If the property doesn't exist or is improperly formed
     * then ignore it.
     */

    if ((result != Success) || (actualType != XA_STRING)
	    || (actualFormat != 8)) {
	if (propInfo != NULL) {
	    XFree(propInfo);
	}
	return;
    }

    /*
     * The property is divided into records separated by null
     * characters.  Each record represents one command request
     * or response.  Scan through the property one record at a
     * time.
     */

    for (p = propInfo; (p-propInfo) < numItems; ) {
	if (*p == 'C') {
	    Window window;
	    int serial, resultSize;
	    char *resultString, *interpName, *returnProp, *end;
	    register RegisteredInterp *riPtr;
	    char errorMsg[100];
#define STATIC_RESULT_SPACE 100
	    char staticSpace[STATIC_RESULT_SPACE];

	    /*
	     *-----------------------------------------------------
	     * This is an incoming command sent by another window.
	     * Parse the fields of the command string.  If the command
	     * string isn't properly formed, send back an error message
	     * if there's enough well-formed information to generate
	     * a proper reply;  otherwise just ignore the message.
	     *-----------------------------------------------------
	     */

	    p++;
	    window = (Window) strtol(p, &end, 16);
	    if (end == p) {
		goto nextRecord;
	    }
	    p = end;
	    if (*p != ' ') {
		goto nextRecord;
	    }
	    p++;
	    serial = strtol(p, &end, 16);
	    if (end == p) {
		goto nextRecord;
	    }
	    p = end;
	    if (*p != ' ') {
		goto nextRecord;
	    }
	    p++;
	    interpName = p;
	    while ((*p != 0) && (*p != '|')) {
		p++;
	    }
	    if (*p != '|') {
		result = TCL_ERROR;
		resultString = "bad property format for sent command";
		goto returnResult;
	    }
	    *p = 0;
	    p++;

	    /*
	     * Locate the interpreter for the command, then
	     * execute the command.
	     */

	    for (riPtr = registry; ; riPtr = riPtr->nextPtr) {
		if (riPtr == NULL) {
		    result = TCL_ERROR;
		    sprintf(errorMsg,
			    "receiver never heard of interpreter \"%.40s\"",
			    interpName);
		    resultString = errorMsg;
		    goto returnResult;
		}
		if (strcmp(riPtr->name, interpName) == 0) {
		    break;
		}
	    }
	    result = Tcl_GlobalEval(riPtr->interp, p);
	    resultString = riPtr->interp->result;

	    /*
	     * Return the result to the sender.
	     */

	    returnResult:
	    resultSize = strlen(resultString) + 30;
	    if (resultSize <= STATIC_RESULT_SPACE) {
		returnProp = staticSpace;
	    } else {
		returnProp = (char *) ckalloc((unsigned) resultSize);
	    }
	    sprintf(returnProp, "R %x %d %s", serial, result,
		    resultString);
	    (void) AppendPropCarefully(dispPtr->display, window,
		    dispPtr->commProperty, returnProp,
		    (PendingCommand *) NULL);
	    if (returnProp != staticSpace) {
		ckfree(returnProp);
	    }
	} else if (*p == 'R') {
	    int serial, code;
	    char *end;
	    register PendingCommand *pcPtr;

	    /*
	     *-----------------------------------------------------
	     * This record in the property is a result being
	     * returned for a command sent from here.  First
	     * parse the fields.
	     *-----------------------------------------------------
	     */

	    p++;
	    serial = strtol(p, &end, 16);
	    if (end == p) {
		goto nextRecord;
	    }
	    p = end;
	    if (*p != ' ') {
		goto nextRecord;
	    }
	    p++;
	    code = strtol(p, &end, 10);
	    if (end == p) {
		goto nextRecord;
	    }
	    p = end;
	    if (*p != ' ') {
		goto nextRecord;
	    }
	    p++;

	    /*
	     * Give the result information to anyone who's
	     * waiting for it.
	     */

	    for (pcPtr = pendingCommands; pcPtr != NULL;
		    pcPtr = pcPtr->nextPtr) {
		if ((serial != pcPtr->serial) || (pcPtr->result != NULL)) {
		    continue;
		}
		pcPtr->code = code;
		pcPtr->result = ckalloc((unsigned) (strlen(p) + 1));
		strcpy(pcPtr->result, p);
		break;
	    }
	}

	nextRecord:
	while (*p != 0) {
	    p++;
	}
	p++;
    }
    XFree(propInfo);
}

/*
 *--------------------------------------------------------------
 *
 * AppendPropCarefully --
 *
 *	Append a given property to a given window, but set up
 *	an X error handler so that if the append fails this
 *	procedure can return an error code rather than having
 *	Xlib panic.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The given property on the given window is appended to.
 *	If this operation fails and if pendingPtr is non-NULL,
 *	then the pending operation is marked as complete with
 *	an error.
 *
 *--------------------------------------------------------------
 */

static void
AppendPropCarefully(display, window, property, value, pendingPtr)
    Display *display;		/* Display on which to operate. */
    Window window;		/* Window whose property is to
				 * be modified. */
    Atom property;		/* Name of property. */
    char *value;		/* Characters (null-terminated) to
				 * append to property. */
    PendingCommand *pendingPtr;	/* Pending command to mark complete
				 * if an error occurs during the
				 * property op.  NULL means just
				 * ignore the error. */
{
    Tk_ErrorHandler handler;

    handler = Tk_CreateErrorHandler(display, -1, -1, -1, AppendErrorProc,
	(ClientData) pendingPtr);
    XChangeProperty(display, window, property, XA_STRING, 8,
	    PropModeAppend, (unsigned char *) value, strlen(value)+1);
    Tk_DeleteErrorHandler(handler);
}

/*
 * The procedure below is invoked if an error occurs during
 * the XChangeProperty operation above.
 */

	/* ARGSUSED */
static int
AppendErrorProc(clientData, errorPtr)
    ClientData clientData;	/* Command to mark complete, or NULL. */
    XErrorEvent *errorPtr;	/* Information about error. */
{
    PendingCommand *pendingPtr = (PendingCommand *) clientData;
    register PendingCommand *pcPtr;

    if (pendingPtr == NULL) {
	return 0;
    }

    /*
     * Make sure this command is still pending.
     */

    for (pcPtr = pendingCommands; pcPtr != NULL;
	    pcPtr = pcPtr->nextPtr) {
	if ((pcPtr == pendingPtr) && (pcPtr->result == NULL)) {
	    pcPtr->result = ckalloc((unsigned) (strlen(pcPtr->target) + 50));
	    sprintf(pcPtr->result,
		    "send to \"%s\" failed (no communication window)",
		    pcPtr->target);
	    pcPtr->code = TCL_ERROR;
	    break;
	}
    }
    return 0;
}

/*
 *--------------------------------------------------------------
 *
 * TimeoutProc --
 *
 *	This procedure is invoked when too much time has elapsed
 *	during the processing of a sent command.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Mark the pending command as complete, with an error
 *	message signalling the timeout.
 *
 *--------------------------------------------------------------
 */

static void
TimeoutProc(clientData)
    ClientData clientData;	/* Information about command that
				 * has been sent but not yet
				 * responded to. */
{
    PendingCommand *pcPtr = (PendingCommand *) clientData;
    register PendingCommand *pcPtr2;

    /*
     * Make sure that the command is still in the pending list
     * and that it hasn't already completed.  Then register the
     * error.
     */

    for (pcPtr2 = pendingCommands; pcPtr2 != NULL;
	    pcPtr2 = pcPtr2->nextPtr) {
	static char msg[] = "remote interpreter did not respond";
	if ((pcPtr2 != pcPtr) || (pcPtr2->result != NULL)) {
	    continue;
	}
	pcPtr2->code = TCL_ERROR;
	pcPtr2->result = ckalloc((unsigned) (sizeof(msg) + 1));
	strcpy(pcPtr2->result, msg);
	return;
    }
}

/*
 *--------------------------------------------------------------
 *
 * DeleteProc --
 *
 *	This procedure is invoked by Tcl when a registered
 *	interpreter is about to be deleted.  It unregisters
 *	the interpreter.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The interpreter given by riPtr is unregistered.
 *
 *--------------------------------------------------------------
 */

static void
DeleteProc(clientData)
    ClientData clientData;	/* Info about registration, passed
				 * as ClientData. */
{
    RegisteredInterp *riPtr = (RegisteredInterp *) clientData;
    register RegisteredInterp *riPtr2;

    (void) LookupName(riPtr->dispPtr, riPtr->name, 1);
    if (registry == riPtr) {
	registry = riPtr->nextPtr;
    } else {
	for (riPtr2 = registry; riPtr2 != NULL;
		riPtr2 = riPtr2->nextPtr) {
	    if (riPtr2->nextPtr == riPtr) {
		riPtr2->nextPtr = riPtr->nextPtr;
		break;
	    }
	}
    }
    ckfree((char *) riPtr->name);
    ckfree((char *) riPtr);
}

/*
 *----------------------------------------------------------------------
 *
 * SendRestrictProc --
 *
 *	This procedure filters incoming events when a "send" command
 *	is outstanding.  It defers all events except those containing
 *	send commands and results.
 *
 * Results:
 *	False is returned except for property-change events on the
 *	given commWindow.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

    /* ARGSUSED */
static Bool
SendRestrictProc(display, eventPtr, arg)
    Display *display;		/* Display from which event arrived. */
    register XEvent *eventPtr;	/* Event that just arrived. */
    char *arg;			/* Comunication window in which
				 * we're interested. */
{
    register Tk_Window comm = (Tk_Window) arg;

    if ((display != Tk_Display(comm))
	    || (eventPtr->type != PropertyNotify)
	    || (eventPtr->xproperty.window != Tk_WindowId(comm))) {
	return False;
    }
    return True;
}
