/* 
 * tkSelect.c --
 *
 *	This file manages the selection for the Tk toolkit,
 *	translating between the standard X ICCCM conventions
 *	and Tcl commands.
 *
 * Copyright 1990 Regents of the University of California.
 * Permission to use, copy, modify, and distribute this
 * software and its documentation for any purpose and without
 * fee is hereby granted, provided that the above copyright
 * notice appear in all copies.  The University of California
 * makes no representations about the suitability of this
 * software for any purpose.  It is provided "as is" without
 * express or implied warranty.
 */

#ifndef lint
static char rcsid[] = "$Header: /user6/ouster/wish/RCS/tkSelect.c,v 1.27 92/08/10 15:03:03 ouster Exp $ SPRITE (Berkeley)";
#endif

#include "tkconfig.h"
#include "tkint.h"

/*
 * When the selection is being retrieved, one of the following
 * structures is present on a list of pending selection retrievals.
 * The structure is used to communicate between the background
 * procedure that requests the selection and the foreground
 * event handler that processes the events in which the selection
 * is returned.  There is a list of such structures so that there
 * can be multiple simultaneous selection retrievals (e.g. on
 * different displays).
 */

typedef struct RetrievalInfo {
    Tcl_Interp *interp;		/* Interpreter for error reporting. */
    TkWindow *winPtr;		/* Window used as requestor for
				 * selection. */
    Atom property;		/* Property where selection will appear. */
    Atom target;		/* Desired form for selection. */
    int (*proc) _ANSI_ARGS_((ClientData clientData, Tcl_Interp *interp,
	char *portion));	/* Procedure to call to handle pieces
				 * of selection. */
    ClientData clientData;	/* Argument for proc. */
    int result;			/* Initially -1.  Set to a Tcl
				 * return value once the selection
				 * has been retrieved. */
    Tk_TimerToken timeout;	/* Token for current timeout procedure. */
    int idleTime;		/* Number of seconds that have gone by
				 * without hearing anything from the
				 * selection owner. */
    struct RetrievalInfo *nextPtr;
				/* Next in list of all pending
				 * selection retrievals.  NULL means
				 * end of list. */
} RetrievalInfo;

static RetrievalInfo *pendingRetrievals = NULL;
				/* List of all retrievals currently
				 * being waited for. */

/*
 * When "selection get" is being used to retrieve the selection,
 * the following data structure is used for communication between
 * Tk_SelectionCmd and SelGetProc.  Its purpose is to keep track
 * of the selection contents, which are gradually assembled in a
 * string.
 */

typedef struct {
    char *string;		/* Contents of selection are
				 * here.  This space is malloc-ed. */
    int bytesAvl;		/* Total number of bytes available
				 * at string. */
    int bytesUsed;		/* Bytes currently in use in string,
				 * not including the terminating
				 * NULL. */
} GetInfo;

/*
 * When handling INCR-style selection retrievals, the selection owner
 * uses the following data structure to communicate between the
 * ConvertSelection procedure and TkSelPropProc.
 */

typedef struct IncrInfo {
    TkWindow *winPtr;		/* Window that owns selection. */
    Atom *multAtoms;		/* Information about conversions to
				 * perform:  one or more pairs of
				 * (target, property).  This either
				 * points to a retrieved  property (for
				 * MULTIPLE retrievals) or to a static
				 * array. */
    unsigned long numConversions;
				/* Number of entries in offsets (same as
				 * # of pairs in multAtoms). */
    int *offsets;		/* One entry for each pair in
				 * multAtoms;  -1 means all data has
				 * been transferred for this
				 * conversion.  -2 means only the
				 * final zero-length transfer still
				 * has to be done.  Otherwise it is the
				 * offset of the next chunk of data
				 * to transfer.  This array is malloc-ed. */
    int numIncrs;		/* Number of entries in offsets that
				 * aren't -1 (i.e. # of INCR-mode transfers
				 * not yet completed). */
    Tk_TimerToken timeout;	/* Token for timer procedure. */
    int idleTime;		/* Number of seconds since we heard
				 * anything from the selection
				 * requestor. */
    Window reqWindow;		/* Requestor's window id. */
    Time time;			/* Timestamp corresponding to
				 * selection at beginning of request;
				 * used to abort transfer if selection
				 * changes. */
    struct IncrInfo *nextPtr;	/* Next in list of all INCR-style
				 * retrievals currently pending. */
} IncrInfo;

static IncrInfo *pendingIncrs = NULL;
				/* List of all IncrInfo structures
				 * currently active. */

/*
 * When a selection handler is set up by invoking "selection handle",
 * one of the following data structures is set up to hold information
 * about the command to invoke and its interpreter.
 */

typedef struct {
    Tcl_Interp *interp;		/* Interpreter in which to invoke command. */
    int cmdLength;		/* # of non-NULL bytes in command. */
    char command[4];		/* Command to invoke.  Actual space is
				 * allocated as large as necessary.  This
				 * must be the last entry in the structure. */
} CommandInfo;

/*
 * Chunk size for retrieving selection.  It's defined both in
 * words and in bytes;  the word size is used to allocate
 * buffer space that's guaranteed to be word-aligned and that
 * has an extra character for the terminating NULL.
 */

#define TK_SEL_BYTES_AT_ONCE 4000
#define TK_SEL_WORDS_AT_ONCE 1001

/*
 * Largest property that we'll accept when sending or receiving the
 * selection:
 */

#define MAX_PROP_WORDS 100000

/*
 * Forward declarations for procedures defined in this file:
 */

static void		ConvertSelection _ANSI_ARGS_((TkWindow *winPtr,
			    XSelectionRequestEvent *eventPtr));
static int		DefaultSelection _ANSI_ARGS_((TkWindow *winPtr,
			    Atom target, char *buffer, int maxBytes,
			    Atom *typePtr));
static int		HandleTclCommand _ANSI_ARGS_((ClientData clientData,
			    int offset, char *buffer, int maxBytes));
static void		IncrTimeoutProc _ANSI_ARGS_((ClientData clientData));
static char *		SelCvtFromX _ANSI_ARGS_((long *propPtr, int numValues,
			    Atom type, Tk_Window tkwin));
static long *		SelCvtToX _ANSI_ARGS_((char *string, Atom type,
			    Tk_Window tkwin, int *numLongsPtr));
static int		SelGetProc _ANSI_ARGS_((ClientData clientData,
			    Tcl_Interp *interp, char *portion));
static void		SelInit _ANSI_ARGS_((Tk_Window tkwin));
static void		SelRcvIncrProc _ANSI_ARGS_((ClientData clientData,
			    XEvent *eventPtr));
static void		SelTimeoutProc _ANSI_ARGS_((ClientData clientData));

/*
 *--------------------------------------------------------------
 *
 * Tk_CreateSelHandler --
 *
 *	This procedure is called to register a procedure
 *	as the handler for selection requests of a particular
 *	target type on a particular window.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	In the future, whenever the selection is in tkwin's
 *	window and someone requests the selection in the
 *	form given by target, proc will be invoked to provide
 *	part or all of the selection in the given form.  If
 *	there was already a handler declared for the given
 *	window and target type, then it is replaced.  Proc
 *	should have the following form:
 *
 *	int
 *	proc(clientData, offset, buffer, maxBytes)
 *	    ClientData clientData;
 *	    int offset;
 *	    char *buffer;
 *	    int maxBytes;
 *	{
 *	}
 *
 *	The clientData argument to proc will be the same as
 *	the clientData argument to this procedure.  The offset
 *	argument indicates which portion of the selection to
 *	return:  skip the first offset bytes.  Buffer is a
 *	pointer to an area in which to place the converted
 *	selection, and maxBytes gives the number of bytes
 *	available at buffer.  Proc should place the selection
 *	in buffer as a string, and return a count of the number
 *	of bytes of selection actually placed in buffer (not
 *	including the terminating NULL character).  If the
 *	return value equals maxBytes, this is a sign that there
 *	is probably still more selection information available.
 *
 *--------------------------------------------------------------
 */

void
Tk_CreateSelHandler(tkwin, target, proc, clientData, format)
    Tk_Window tkwin;		/* Token for window. */
    Atom target;		/* The kind of selection conversions
				 * that can be handled by proc,
				 * e.g. TARGETS or XA_STRING. */
    Tk_SelectionProc *proc;	/* Procedure to invoke to convert
				 * selection to type "target". */
    ClientData clientData;	/* Value to pass to proc. */
    Atom format;		/* Format in which the selection
				 * information should be returned to
				 * the requestor. XA_STRING is best by
				 * far, but anything listed in the ICCCM
				 * will be tolerated (blech). */
{
    register TkSelHandler *selPtr;
    TkWindow *winPtr = (TkWindow *) tkwin;

    if (winPtr->dispPtr->multipleAtom == None) {
	SelInit(tkwin);
    }

    /*
     * See if there's already a handler for this target on
     * this window.  If so, re-use it.  If not, create a new one.
     */

    for (selPtr = winPtr->selHandlerList; ; selPtr = selPtr->nextPtr) {
	if (selPtr == NULL) {
	    selPtr = (TkSelHandler *) ckalloc(sizeof(TkSelHandler));
	    selPtr->nextPtr = winPtr->selHandlerList;
	    winPtr->selHandlerList = selPtr;
	    break;
	}
	if (selPtr->target == target) {

	    /*
	     * Special case:  when replacing handler created by
	     * "selection handle" free up memory.  Should there be a
	     * callback to allow other clients to do this too?
	     */

	    if (selPtr->proc == HandleTclCommand) {
		ckfree((char *) selPtr->clientData);
	    }
	    break;
	}
    }
    selPtr->target = target;
    selPtr->format = format;
    selPtr->proc = proc;
    selPtr->clientData = clientData;
    if (format == XA_STRING) {
	selPtr->size = 8;
    } else {
	selPtr->size = 32;
    }
}

/*
 *--------------------------------------------------------------
 *
 * Tk_OwnSelection --
 *
 *	Arrange for tkwin to become the selection owner.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	From now on, requests for the selection will be
 *	directed to procedures associated with tkwin (they
 *	must have been declared with calls to Tk_CreateSelHandler).
 *	When the selection is lost by this window, proc will
 *	be invoked (see the manual entry for details).
 *
 *--------------------------------------------------------------
 */

void
Tk_OwnSelection(tkwin, proc, clientData)
    Tk_Window tkwin;		/* Window to become new selection
				 * owner. */
    Tk_LostSelProc *proc;	/* Procedure to call when selection
				 * is taken away from tkwin. */
    ClientData clientData;	/* Arbitrary one-word argument to
				 * pass to proc. */
{
    register TkWindow *winPtr = (TkWindow *) tkwin;
    TkDisplay *dispPtr = winPtr->dispPtr;

    if (dispPtr->multipleAtom == None) {
	SelInit(tkwin);
    }

    winPtr->selClearProc = proc;
    winPtr->selClearData = clientData;
    if (dispPtr->selectionOwner != tkwin) {
	TkWindow *ownerPtr = (TkWindow *) dispPtr->selectionOwner;

	if ((ownerPtr != NULL)
		&& (ownerPtr->selClearProc != NULL)) {
	    (*ownerPtr->selClearProc)(ownerPtr->selClearData);
	    ownerPtr->selClearProc = NULL;
	}
    }
    dispPtr->selectionOwner = tkwin;
    dispPtr->selectionSerial = NextRequest(winPtr->display);
    dispPtr->selectionTime = TkCurrentTime(dispPtr);
    XSetSelectionOwner(winPtr->display, XA_PRIMARY, winPtr->window,
	    dispPtr->selectionTime);
}

/*
 *--------------------------------------------------------------
 *
 * Tk_GetSelection --
 *
 *	Retrieve the selection and pass it off (in pieces,
 *	possibly) to a given procedure.
 *
 * Results:
 *	The return value is a standard Tcl return value.
 *	If an error occurs (such as no selection exists)
 *	then an error message is left in interp->result.
 *
 * Side effects:
 *	The standard X11 protocols are used to retrieve the
 *	selection.  When it arrives, it is passed to proc.  If
 *	the selection is very large, it will be passed to proc
 *	in several pieces.  Proc should have the following
 *	structure:
 *
 *	int
 *	proc(clientData, interp, portion)
 *	    ClientData clientData;
 *	    Tcl_Interp *interp;
 *	    char *portion;
 *	{
 *	}
 *
 *	The interp and clientData arguments to proc will be the
 *	same as the corresponding arguments to Tk_GetSelection.
 *	The portion argument points to a character string
 *	containing part of the selection, and numBytes indicates
 *	the length of the portion, not including the terminating
 *	NULL character.  If the selection arrives in several pieces,
 *	the "portion" arguments in separate calls will contain
 *	successive parts of the selection.  Proc should normally
 *	return TCL_OK.  If it detects an error then it should return
 *	TCL_ERROR and leave an error message in interp->result; the
 *	remainder of the selection retrieval will be aborted.
 *
 *--------------------------------------------------------------
 */

int
Tk_GetSelection(interp, tkwin, target, proc, clientData)
    Tcl_Interp *interp;		/* Interpreter to use for reporting
				 * errors. */
    Tk_Window tkwin;		/* Window on whose behalf to retrieve
				 * the selection (determines display
				 * from which to retrieve). */
    Atom target;		/* Desired form in which selection
				 * is to be returned. */
    Tk_GetSelProc *proc;	/* Procedure to call to process the
				 * selection, once it has been retrieved. */
    ClientData clientData;	/* Arbitrary value to pass to proc. */
{
    RetrievalInfo retr;
    TkWindow *winPtr = (TkWindow *) tkwin;
    TkDisplay *dispPtr = winPtr->dispPtr;

    if (dispPtr->multipleAtom == None) {
	SelInit(tkwin);
    }

    /*
     * If the selection is owned by a window managed by this
     * process, then call the retrieval procedure directly,
     * rather than going through the X server (it's dangerous
     * to go through the X server in this case because it could
     * result in deadlock if an INCR-style selection results).
     */

    if (dispPtr->selectionOwner != NULL) {
	register TkSelHandler *selPtr;
	int offset, result, count;
	char buffer[TK_SEL_BYTES_AT_ONCE+1];
	Time time;

	/*
	 * Make sure that the selection predates the request
	 * time.
	 */

	time = TkCurrentTime(dispPtr);
	if ((time < dispPtr->selectionTime)
		&& (time != CurrentTime)
		&& (dispPtr->selectionTime != CurrentTime)) {
	    interp->result = "selection changed before it could be retrieved";
	    return TCL_ERROR;
	}

	for (selPtr = ((TkWindow *) dispPtr->selectionOwner)->selHandlerList;
		; selPtr = selPtr->nextPtr) {
	    if (selPtr == NULL) {
		Atom type;

		count = DefaultSelection((TkWindow *) dispPtr->selectionOwner,
			target, buffer, TK_SEL_BYTES_AT_ONCE, &type);
		if (count > TK_SEL_BYTES_AT_ONCE) {
		    panic("selection handler returned too many bytes");
		}
		if (count < 0) {
		    cantget:
		    Tcl_AppendResult(interp, "selection doesn't exist",
			    " or form \"", Tk_GetAtomName(tkwin, target),
			    "\" not defined", (char *) NULL);
		    return TCL_ERROR;
		}
		buffer[count] = 0;
		return (*proc)(clientData, interp, buffer);
	    }
	    if (selPtr->target == target) {
		break;
	    }
	}
	offset = 0;
	while (1) {
	    count = (*selPtr->proc)(selPtr->clientData, offset,
		buffer, TK_SEL_BYTES_AT_ONCE);
	    if (count < 0) {
		goto cantget;
	    }
	    if (count > TK_SEL_BYTES_AT_ONCE) {
		panic("selection handler returned too many bytes");
	    }
	    buffer[count] = '\0';
	    result = (*proc)(clientData, interp, buffer);
	    if (result != TCL_OK) {
		return result;
	    }
	    if (count < TK_SEL_BYTES_AT_ONCE) {
		return TCL_OK;
	    }
	    offset += count;
	}
    }

    /*
     * The selection is owned by some other process.  To
     * retrieve it, first record information about the retrieval
     * in progress.  Also, try to use a non-top-level window
     * as the requestor (property changes on this window may
     * be monitored by a window manager, which will waste time).
     */

    retr.interp = interp;
    if ((winPtr->flags & TK_TOP_LEVEL)
	    && (winPtr->childList != NULL)) {
	winPtr = winPtr->childList;
    }
    retr.winPtr = winPtr;
    retr.property = XA_PRIMARY;
    retr.target = target;
    retr.proc = proc;
    retr.clientData = clientData;
    retr.result = -1;
    retr.idleTime = 0;
    retr.nextPtr = pendingRetrievals;
    pendingRetrievals = &retr;

    /*
     * Initiate the request for the selection.
     */

    XConvertSelection(winPtr->display, XA_PRIMARY, target,
	    retr.property, winPtr->window, TkCurrentTime(dispPtr));

    /*
     * Enter a loop processing X events until the selection
     * has been retrieved and processed.  If no response is
     * received within a few seconds, then timeout.
     */

    retr.timeout = Tk_CreateTimerHandler(1000, SelTimeoutProc,
	    (ClientData) &retr);
    while (retr.result == -1) {
	Tk_DoOneEvent(0);
    }
    Tk_DeleteTimerHandler(retr.timeout);

    /*
     * Unregister the information about the selection retrieval
     * in progress.
     */

    if (pendingRetrievals == &retr) {
	pendingRetrievals = retr.nextPtr;
    } else {
	RetrievalInfo *retrPtr;

	for (retrPtr = pendingRetrievals; retrPtr != NULL;
		retrPtr = retrPtr->nextPtr) {
	    if (retrPtr->nextPtr == &retr) {
		retrPtr->nextPtr = retr.nextPtr;
		break;
	    }
	}
    }
    return retr.result;
}

/*
 *--------------------------------------------------------------
 *
 * Tk_SelectionCmd --
 *
 *	This procedure is invoked to process the "selection" Tcl
 *	command.  See the user documentation for details on what
 *	it does.
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
Tk_SelectionCmd(clientData, interp, argc, argv)
    ClientData clientData;	/* Main window associated with
				 * interpreter. */
    Tcl_Interp *interp;		/* Current interpreter. */
    int argc;			/* Number of arguments. */
    char **argv;		/* Argument strings. */
{
    Tk_Window tkwin = (Tk_Window) clientData;
    int length;
    char *cmd = argv[0];
    char c;

    if (argc < 2) {
	sprintf(interp->result,
		"wrong # args: should be \"%.50s [-window win] option ?arg arg ...?\"",
		cmd);
	return TCL_ERROR;
    }

    argc--; argv++;
    c = argv[0][0];
    length = strlen(argv[0]);

    if ((c == '-') && (strncmp(argv[0], "-window", length) == 0)) {
      if ((argc < 2) ||
	  ((tkwin = Tk_NameToWindow(interp, argv[1], tkwin)) == NULL)) {
	    sprintf(interp->result, "bad arg to %s -window", cmd);
	    return TCL_ERROR;
      }
      argc -= 2; argv += 2;

      if (argc == 0) {
	sprintf(interp->result, "not enough args to %s", cmd);
	return TCL_ERROR;
      }

      c = argv[0][0];
      length = strlen(argv[0]);
    }

    if ((c == 'g') && (strncmp(argv[0], "get", length) == 0)) {
	Atom target;
	GetInfo getInfo;
	int result;

	argc--; argv++;

	if (argc > 1) {
	    sprintf(interp->result,
		    "too may args: should be \"%.50s get ?type?\"",
		    cmd);
	    return TCL_ERROR;
	}
	if (argc == 1) {
	    target = Tk_InternAtom(tkwin, argv[0]);
	} else {
	    target = XA_STRING;
	}
	getInfo.string = (char *) ckalloc(100);
	getInfo.bytesAvl = 100;
	getInfo.bytesUsed = 0;
	result = Tk_GetSelection(interp, tkwin, target, SelGetProc,
		(ClientData) &getInfo);
	if (result == TCL_OK) {
	    Tcl_SetResult(interp, getInfo.string, TCL_DYNAMIC);
	} else {
	    ckfree(getInfo.string);
	}
	return result;
    } else if ((c == 'h') && (strncmp(argv[0], "handle", length) == 0)) {
	Tk_Window window;
	Atom target, format;
	register CommandInfo *cmdInfoPtr;
	int cmdLength;

	argc--; argv++;

	if ((argc < 2) || (argc > 4)) {
	    Tcl_AppendResult(interp, "wrong # args: should be \"", cmd,
		    " handle window command ?type? ?format?\"", (char *) NULL);
	    return TCL_ERROR;
	}
	window = Tk_NameToWindow(interp, argv[0], tkwin);
	if (window == NULL) {
	    return TCL_ERROR;
	}
	if (argc > 2) {
	    target = Tk_InternAtom(window, argv[2]);
	} else {
	    target = XA_STRING;
	}
	if (argc > 3) {
	    format = Tk_InternAtom(window, argv[3]);
	} else {
	    format = XA_STRING;
	}
	cmdLength = strlen(argv[1]);
	cmdInfoPtr = (CommandInfo *) ckalloc((unsigned) (sizeof(CommandInfo)
		+ cmdLength));
	cmdInfoPtr->interp = interp;
	cmdInfoPtr->cmdLength = cmdLength;
	strcpy(cmdInfoPtr->command, argv[1]);
	Tk_CreateSelHandler(window, target, HandleTclCommand,
		(ClientData) cmdInfoPtr, format);
	return TCL_OK;
    } else {
	sprintf(interp->result,
		"bad option to \"%.50s\":  must be get or handle",
		cmd);
	return TCL_ERROR;
    }
}

/*
 *----------------------------------------------------------------------
 *
 * TkSelDeadWindow --
 *
 *	This procedure is invoked just before a TkWindow is deleted.
 *	It performs selection-related cleanup.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Frees up memory associated with the selection.
 *
 *----------------------------------------------------------------------
 */

void
TkSelDeadWindow(winPtr)
    register TkWindow *winPtr;	/* Window that's being deleted. */
{
    register TkSelHandler *selPtr;

    while (1) {
	selPtr = winPtr->selHandlerList;
	if (selPtr == NULL) {
	    break;
	}
	winPtr->selHandlerList = selPtr->nextPtr;
	ckfree((char *) selPtr);
    }
    winPtr->selClearProc = NULL;

    if (winPtr->dispPtr->selectionOwner == (Tk_Window) winPtr) {
	winPtr->dispPtr->selectionOwner = NULL;
    }
}

/*
 *----------------------------------------------------------------------
 *
 * SelInit --
 *
 *	Initialize selection-related information for a display.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	.
 *
 *----------------------------------------------------------------------
 */

static void
SelInit(tkwin)
    Tk_Window tkwin;		/* Window token (used to find
				 * display to initialize). */
{
    register TkDisplay *dispPtr = ((TkWindow *) tkwin)->dispPtr;

    /*
     * Fetch commonly-used atoms.
     */

    dispPtr->multipleAtom = Tk_InternAtom(tkwin, "MULTIPLE");
    dispPtr->incrAtom = Tk_InternAtom(tkwin, "INCR");
    dispPtr->targetsAtom = Tk_InternAtom(tkwin, "TARGETS");
    dispPtr->timestampAtom = Tk_InternAtom(tkwin, "TIMESTAMP");
    dispPtr->textAtom = Tk_InternAtom(tkwin, "TEXT");
    dispPtr->compoundTextAtom = Tk_InternAtom(tkwin, "COMPOUND_TEXT");
}

/*
 *--------------------------------------------------------------
 *
 * TkSelEventProc --
 *
 *	This procedure is invoked whenever a selection-related
 *	event occurs.  It does the lion's share of the work
 *	in implementing the selection protocol.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Lots:  depends on the type of event.
 *
 *--------------------------------------------------------------
 */

void
TkSelEventProc(tkwin, eventPtr)
    Tk_Window tkwin;		/* Window for which event was
				 * targeted. */
    register XEvent *eventPtr;	/* X event:  either SelectionClear,
				 * SelectionRequest, or
				 * SelectionNotify. */
{
    register TkWindow *winPtr = (TkWindow *) tkwin;

    /*
     * Case #1: SelectionClear events.  Invoke clear procedure
     * for window that just lost the selection.  This code is a
     * bit tricky, because any callbacks to due selection changes
     * between windows managed by the process have already been
     * made.  Thus, ignore the event unless it refers to the
     * window that's currently the selection owner and the event
     * was generated after the server saw the SetSelectionOwner
     * request.
     */

    if (eventPtr->type == SelectionClear) {
	if ((eventPtr->xselectionclear.selection == XA_PRIMARY)
		&& (winPtr->dispPtr->selectionOwner == tkwin)
		&& (eventPtr->xselectionclear.serial
			>= winPtr->dispPtr->selectionSerial)
		&& (winPtr->selClearProc != NULL)) {
	    (*winPtr->selClearProc)(winPtr->selClearData);
	    winPtr->selClearProc = NULL;
	    winPtr->dispPtr->selectionOwner = NULL;
	}
	return;
    }

    /*
     * Case #2: SelectionNotify events.  Call the relevant procedure
     * to handle the incoming selection.
     */

    if (eventPtr->type == SelectionNotify) {
	register RetrievalInfo *retrPtr;
	char *propInfo;
	Atom type;
	int format, result;
	unsigned long numItems, bytesAfter;

	for (retrPtr = pendingRetrievals; ; retrPtr = retrPtr->nextPtr) {
	    if (retrPtr == NULL) {
		return;
	    }
	    if ((retrPtr->winPtr == winPtr)
		    && (eventPtr->xselection.selection == XA_PRIMARY)
		    && (retrPtr->target == eventPtr->xselection.target)
		    && (retrPtr->result == -1)) {
		if (retrPtr->property == eventPtr->xselection.property) {
		    break;
		}
		if (eventPtr->xselection.property == None) {
		    Tcl_SetResult(retrPtr->interp, (char *) NULL, TCL_STATIC);
		    Tcl_AppendResult(retrPtr->interp,
			    "selection doesn't exist or form \"",
			    Tk_GetAtomName(tkwin, retrPtr->target),
			    "\" not defined", (char *) NULL);
		    retrPtr->result = TCL_ERROR;
		    return;
		}
	    }
	}

	propInfo = NULL;
	result = XGetWindowProperty(eventPtr->xselection.display,
		eventPtr->xselection.requestor, retrPtr->property,
		0, MAX_PROP_WORDS, False, (Atom) AnyPropertyType,
		&type, &format, &numItems, &bytesAfter,
		(unsigned char **) &propInfo);
	if ((result != Success) || (type == None)) {
	    return;
	}
	if (bytesAfter != 0) {
	    Tcl_SetResult(retrPtr->interp, "selection property too large",
		TCL_STATIC);
	    retrPtr->result = TCL_ERROR;
	    XFree(propInfo);
	    return;
	}
	if ((type == XA_STRING) || (type == winPtr->dispPtr->textAtom)
		|| (type == winPtr->dispPtr->compoundTextAtom)) {
	    if (format != 8) {
		sprintf(retrPtr->interp->result,
		    "bad format for string selection: wanted \"8\", got \"%d\"",
		    format);
		retrPtr->result = TCL_ERROR;
		return;
	    }
	    retrPtr->result = (*retrPtr->proc)(retrPtr->clientData,
		    retrPtr->interp, propInfo);
	} else if (type == winPtr->dispPtr->incrAtom) {

	    /*
	     * It's a !?#@!?!! INCR-style reception.  Arrange to receive
	     * the selection in pieces, using the ICCCM protocol, then
	     * hang around until either the selection is all here or a
	     * timeout occurs.
	     */

	    retrPtr->idleTime = 0;
	    Tk_CreateEventHandler(tkwin, PropertyChangeMask, SelRcvIncrProc,
		    (ClientData) retrPtr);
	    XDeleteProperty(Tk_Display(tkwin), Tk_WindowId(tkwin),
		    retrPtr->property);
	    while (retrPtr->result == -1) {
		Tk_DoOneEvent(0);
	    }
	    Tk_DeleteEventHandler(tkwin, PropertyChangeMask, SelRcvIncrProc,
		    (ClientData) retrPtr);
	} else {
	    char *string;

	    if (format != 32) {
		sprintf(retrPtr->interp->result,
		    "bad format for selection: wanted \"32\", got \"%d\"",
		    format);
		retrPtr->result = TCL_ERROR;
		return;
	    }
	    string = SelCvtFromX((long *) propInfo, (int) numItems, type,
		    (Tk_Window) winPtr);
	    retrPtr->result = (*retrPtr->proc)(retrPtr->clientData,
		    retrPtr->interp, string);
	    ckfree(string);
	}
	XFree(propInfo);
	return;
    }

    /*
     * Case #3: SelectionRequest events.  Call ConvertSelection to
     * do the dirty work.
     */

    if ((eventPtr->type == SelectionRequest)
	    && (eventPtr->xselectionrequest.selection == XA_PRIMARY)) {
	ConvertSelection(winPtr, &eventPtr->xselectionrequest);
	return;
    }
}

/*
 *--------------------------------------------------------------
 *
 * SelGetProc --
 *
 *	This procedure is invoked to process pieces of the
 *	selection as they arrive during "selection get"
 *	commands.
 *
 * Results:
 *	Always returns TCL_OK.
 *
 * Side effects:
 *	Bytes get appended to the result currently stored
 *	in interp->result, and its memory area gets
 *	expanded if necessary.
 *
 *--------------------------------------------------------------
 */

	/* ARGSUSED */
static int
SelGetProc(clientData, interp, portion)
    ClientData clientData;	/* Information about partially-
				 * assembled result. */
    Tcl_Interp *interp;		/* Interpreter used for error
				 * reporting (not used). */
    char *portion;		/* New information to be appended. */
{
    register GetInfo *getInfoPtr = (GetInfo *) clientData;
    int newLength;

    newLength = strlen(portion) + getInfoPtr->bytesUsed;

    /*
     * Grow the result area if we've run out of space.
     */

    if (newLength >= getInfoPtr->bytesAvl) {
	char *newString;

	getInfoPtr->bytesAvl *= 2;
	if (getInfoPtr->bytesAvl <= newLength) {
	    getInfoPtr->bytesAvl = newLength + 1;
	}
	newString = (char *) ckalloc((unsigned) getInfoPtr->bytesAvl);
	memcpy((VOID *) newString, (VOID *) getInfoPtr->string,
		getInfoPtr->bytesUsed);
	ckfree(getInfoPtr->string);
	getInfoPtr->string = newString;
    }

    /*
     * Append the new data to what was already there.
     */

    strcpy(getInfoPtr->string + getInfoPtr->bytesUsed, portion);
    getInfoPtr->bytesUsed = newLength;
    return TCL_OK;
}

/*
 *----------------------------------------------------------------------
 *
 * SelCvtToX --
 *
 *	Given a selection represented as a string (the normal Tcl form),
 *	convert it to the ICCCM-mandated format for X, depending on
 *	the type argument.  This procedure and SelCvtFromX are inverses.
 *
 * Results:
 *	The return value is a malloc'ed buffer holding a value
 *	equivalent to "string", but formatted as for "type".  It is
 *	the caller's responsibility to free the string when done with
 *	it.  The word at *numLongsPtr is filled in with the number of
 *	32-bit words returned in the result.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

static long *
SelCvtToX(string, type, tkwin, numLongsPtr)
    char *string;		/* String representation of selection. */
    Atom type;			/* Atom specifying the X format that is
				 * desired for the selection.  Should not
				 * be XA_STRING (if so, don't bother calling
				 * this procedure at all). */
    Tk_Window tkwin;		/* Window that governs atom conversion. */
    int *numLongsPtr;		/* Number of 32-bit words contained in the
				 * result. */
{
    register char *p;
    char *field;
    int numFields;
    long *propPtr, *longPtr;
#define MAX_ATOM_NAME_LENGTH 100
    char atomName[MAX_ATOM_NAME_LENGTH+1];

    /*
     * The string is assumed to consist of fields separated by spaces.
     * The property gets generated by converting each field to an
     * integer number, in one of two ways:
     * 1. If type is XA_ATOM, convert each field to its corresponding
     *	  atom.
     * 2. If type is anything else, convert each field from an ASCII number
     *    to a 32-bit binary number.
     */

    numFields = 1;
    for (p = string; *p != 0; p++) {
	if (isspace(*p)) {
	    numFields++;
	}
    }
    propPtr = (long *) ckalloc((unsigned) numFields*sizeof(long));

    /*
     * Convert the fields one-by-one.
     */

    for (longPtr = propPtr, *numLongsPtr = 0, p = string;
	    ; longPtr++, (*numLongsPtr)++) {
	while (isspace(*p)) {
	    p++;
	}
	if (*p == 0) {
	    break;
	}
	field = p;
	while ((*p != 0) && !isspace(*p)) {
	    p++;
	}
	if (type == XA_ATOM) {
	    int length;

	    length = p - field;
	    if (length > MAX_ATOM_NAME_LENGTH) {
		length = MAX_ATOM_NAME_LENGTH;
	    }
	    strncpy(atomName, field, length);
	    atomName[length] = 0;
	    *longPtr = (long) Tk_InternAtom(tkwin, atomName);
	} else {
	    char *dummy;

	    *longPtr = strtol(field, &dummy, 0);
	}
    }
    return propPtr;
}

/*
 *----------------------------------------------------------------------
 *
 * SelCvtFromX --
 *
 *	Given an X property value, formatted as a collection of 32-bit
 *	values according to "type" and the ICCCM conventions, convert
 *	the value to a string suitable for manipulation by Tcl.  This
 *	procedure is the inverse of SelCvtToX.
 *
 * Results:
 *	The return value is the string equivalent of "property".  It is
 *	malloc-ed and should be freed by the caller when no longer
 *	needed.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

static char *
SelCvtFromX(propPtr, numValues, type, tkwin)
    register long *propPtr;	/* Property value from X. */
    int numValues;		/* Number of 32-bit values in property. */
    Atom type;			/* Type of property  Should not be
				 * XA_STRING (if so, don't bother calling
				 * this procedure at all). */
    Tk_Window tkwin;		/* Window to use for atom conversion. */
{
    char *result;
    int resultSpace, curSize, fieldSize;
    char *atomName;

    /*
     * Convert each long in the property to a string value, which is
     * either the name of an atom (if type is XA_ATOM) or a hexadecimal
     * string.  Make an initial guess about the size of the result, but
     * be prepared to enlarge the result if necessary.
     */

    resultSpace = 12*numValues;
    curSize = 0;
    atomName = "";	/* Not needed, but eliminates compiler warning. */
    result = (char *) ckalloc((unsigned) resultSpace);
    for ( ; numValues > 0; propPtr++, numValues--) {
	if (type == XA_ATOM) {
	    atomName = Tk_GetAtomName(tkwin, (Atom) *propPtr);
	    fieldSize = strlen(atomName) + 1;
	} else {
	    fieldSize = 12;
	}
	if (curSize+fieldSize >= resultSpace) {
	    char *newResult;

	    resultSpace *= 2;
	    if (curSize+fieldSize >= resultSpace) {
		resultSpace = curSize + fieldSize + 1;
	    }
	    newResult = (char *) ckalloc((unsigned) resultSpace);
	    strcpy(newResult, result);
	    ckfree(result);
	    result = newResult;
	}
	if (curSize != 0) {
	    result[curSize] = ' ';
	    curSize++;
	}
	if (type == XA_ATOM) {
	    strcpy(result+curSize, atomName);
	} else {
	    sprintf(result+curSize, "%#x", *propPtr);
	}
	curSize += strlen(result+curSize);
    }
    return result;
}

/*
 *----------------------------------------------------------------------
 *
 * ConvertSelection --
 *
 *	This procedure is invoked to handle SelectionRequest events.
 *	It responds to the requests, obeying the ICCCM protocols.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Properties are created for the selection requestor, and a
 *	SelectionNotify event is generated for the selection
 *	requestor.  In the event of long selections, this procedure
 *	implements INCR-mode transfers, using the ICCCM protocol.
 *
 *----------------------------------------------------------------------
 */

static void
ConvertSelection(winPtr, eventPtr)
    TkWindow *winPtr;			/* Window that owns selection. */
    register XSelectionRequestEvent *eventPtr;
					/* Event describing request. */
{
    XSelectionEvent reply;		/* Used to notify requestor that
					 * selection info is ready. */
    int multiple;			/* Non-zero means a MULTIPLE request
					 * is being handled. */
    IncrInfo info;			/* State of selection conversion. */
    Atom singleInfo[2];			/* info.multAtoms points here except
					 * for multiple conversions. */
    int i;
    Tk_ErrorHandler errorHandler;

    errorHandler = Tk_CreateErrorHandler(eventPtr->display, -1, -1,-1,
	    (int (*)()) NULL, (ClientData) NULL);

    /*
     * Initialize the reply event.
     */

    reply.type = SelectionNotify;
    reply.serial = 0;
    reply.send_event = True;
    reply.display = eventPtr->display;
    reply.requestor = eventPtr->requestor;
    reply.selection = XA_PRIMARY;
    reply.target = eventPtr->target;
    reply.property = eventPtr->property;
    if (reply.property == None) {
	reply.property = reply.target;
    }
    reply.time = eventPtr->time;

    /*
     * Watch out for races between conversion requests and
     * selection ownership changes:  reject the conversion
     * request if it's for the wrong window or the wrong
     * time.
     */

    if ((winPtr->dispPtr->selectionOwner != (Tk_Window) winPtr)
	    || ((eventPtr->time < winPtr->dispPtr->selectionTime)
	    && (eventPtr->time != CurrentTime)
	    && (winPtr->dispPtr->selectionTime != CurrentTime))) {
	goto refuse;
    }

    /*
     * Figure out which kind(s) of conversion to perform.  If handling
     * a MULTIPLE conversion, then read the property describing which
     * conversions to perform.
     */

    info.winPtr = winPtr;
    if (eventPtr->target != winPtr->dispPtr->multipleAtom) {
	multiple = 0;
	singleInfo[0] = reply.target;
	singleInfo[1] = reply.property;
	info.multAtoms = singleInfo;
	info.numConversions = 1;
    } else {
	Atom type;
	int format, result;
	unsigned long bytesAfter;

	multiple = 1;
	info.multAtoms = NULL;
	if (eventPtr->property == None) {
	    goto refuse;
	}
	result = XGetWindowProperty(eventPtr->display,
		eventPtr->requestor, eventPtr->property,
		0, MAX_PROP_WORDS, False, XA_ATOM,
		&type, &format, &info.numConversions, &bytesAfter,
		(unsigned char **) &info.multAtoms);
	if ((result != Success) || (bytesAfter != 0) || (format != 32)
		|| (type == None)) {
	    if (info.multAtoms != NULL) {
		XFree((char *) info.multAtoms);
	    }
	    goto refuse;
	}
	info.numConversions /= 2;		/* Two atoms per conversion. */
    }

    /*
     * Loop through all of the requested conversions, and either return
     * the entire converted selection, if it can be returned in a single
     * bunch, or return INCR information only (the actual selection will
     * be returned below).
     */

    info.offsets = (int *) ckalloc((unsigned) (info.numConversions*sizeof(int)));
    info.numIncrs = 0;
    for (i = 0; i < info.numConversions; i++) {
	Atom target, property;
	long buffer[TK_SEL_WORDS_AT_ONCE];
	register TkSelHandler *selPtr;

	target = info.multAtoms[2*i];
	property = info.multAtoms[2*i + 1];
	info.offsets[i] = -1;

	for (selPtr = winPtr->selHandlerList; ; selPtr = selPtr->nextPtr) {
	    int numItems, format;
	    char *propPtr;
	    Atom type;

	    if (selPtr == NULL) {

		/*
		 * Nobody seems to know about this kind of request.  If
		 * it's of a sort that we can handle without any help, do
		 * it.  Otherwise mark the request as an errror.
		 */

		numItems = DefaultSelection(winPtr, target, (char *) buffer,
			TK_SEL_BYTES_AT_ONCE, &type);
		if (numItems != 0) {
		    goto gotStuff;
		}
		info.multAtoms[2*i + 1] = None;
		break;
	    } else if (selPtr->target == target) {
		numItems = (*selPtr->proc)(selPtr->clientData, 0,
			(char *) buffer, TK_SEL_BYTES_AT_ONCE);
		if (numItems < 0) {
		    info.multAtoms[2*i + 1] = None;
		    break;
		}
		if (numItems > TK_SEL_BYTES_AT_ONCE) {
		    panic("selection handler returned too many bytes");
		}
		((char *) buffer)[numItems] = '\0';
		type = selPtr->format;
	    } else {
		continue;
	    }

	    gotStuff:
	    if (numItems == TK_SEL_BYTES_AT_ONCE) {
		info.numIncrs++;
		type = winPtr->dispPtr->incrAtom;
		buffer[0] = 10;	/* Guess at # items avl. */
		numItems = 1;
		propPtr = (char *) buffer;
		format = 32;
		info.offsets[i] = 0;
	    } else if (type == XA_STRING) {
		propPtr = (char *) buffer;
		format = 8;
	    } else {
		propPtr = (char *) SelCvtToX((char *) buffer,
			type, (Tk_Window) winPtr, &numItems);
		format = 32;
	    }
	    XChangeProperty(reply.display, reply.requestor,
		    property, type, format, PropModeReplace,
		    (unsigned char *) propPtr, numItems);
	    if (propPtr != (char *) buffer) {
		ckfree(propPtr);
	    }
	    break;
	}
    }

    /*
     * Send an event back to the requestor to indicate that the
     * first stage of conversion is complete (everything is done
     * except for long conversions that have to be done in INCR
     * mode).
     */

    if (info.numIncrs > 0) {
	XSelectInput(reply.display, reply.requestor, PropertyChangeMask);
	info.timeout = Tk_CreateTimerHandler(1000, IncrTimeoutProc,
	    (ClientData) &info);
	info.idleTime = 0;
	info.reqWindow = reply.requestor;
	info.time = winPtr->dispPtr->selectionTime;
	info.nextPtr = pendingIncrs;
	pendingIncrs = &info;
    }
    if (multiple) {
	XChangeProperty(reply.display, reply.requestor, reply.property,
		XA_ATOM, 32, PropModeReplace,
		(unsigned char *) info.multAtoms,
		(int) info.numConversions*2);
    } else {

	/*
	 * Not a MULTIPLE request.  The first property in "multAtoms"
	 * got set to None if there was an error in conversion.
	 */

	reply.property = info.multAtoms[1];
    }
    XSendEvent(reply.display, reply.requestor, False, 0, (XEvent *) &reply);
    Tk_DeleteErrorHandler(errorHandler);

    /*
     * Handle any remaining INCR-mode transfers.  This all happens
     * in callbacks to TkSelPropProc, so just wait until the number
     * of uncompleted INCR transfers drops to zero.
     */

    if (info.numIncrs > 0) {
	IncrInfo *infoPtr2;

	while (info.numIncrs > 0) {
	    Tk_DoOneEvent(0);
	}
	Tk_DeleteTimerHandler(info.timeout);
	errorHandler = Tk_CreateErrorHandler(winPtr->display,
		-1, -1,-1, (int (*)()) NULL, (ClientData) NULL);
	XSelectInput(reply.display, reply.requestor, 0L);
	Tk_DeleteErrorHandler(errorHandler);
	if (pendingIncrs == &info) {
	    pendingIncrs = info.nextPtr;
	} else {
	    for (infoPtr2 = pendingIncrs; infoPtr2 != NULL;
		    infoPtr2 = infoPtr2->nextPtr) {
		if (infoPtr2->nextPtr == &info) {
		    infoPtr2->nextPtr = info.nextPtr;
		    break;
		}
	    }
	}
    }

    /*
     * All done.  Cleanup and return.
     */

    ckfree((char *) info.offsets);
    if (multiple) {
	XFree((char *) info.multAtoms);
    }
    return;

    /*
     * An error occurred.  Send back a refusal message.
     */

    refuse:
    reply.property = None;
    XSendEvent(reply.display, reply.requestor, False, 0, (XEvent *) &reply);
    Tk_DeleteErrorHandler(errorHandler);
    return;
}

/*
 *----------------------------------------------------------------------
 *
 * SelRcvIncrProc --
 *
 *	This procedure handles the INCR protocol on the receiving
 *	side.  It is invoked in response to property changes on
 *	the requestor's window (which hopefully are because a new
 *	chunk of the selection arrived).
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	If a new piece of selection has arrived, a procedure is
 *	invoked to deal with that piece.  When the whole selection
 *	is here, a flag is left for the higher-level procedure that
 *	initiated the selection retrieval.
 *
 *----------------------------------------------------------------------
 */

static void
SelRcvIncrProc(clientData, eventPtr)
    ClientData clientData;		/* Information about retrieval. */
    register XEvent *eventPtr;		/* X PropertyChange event. */
{
    register RetrievalInfo *retrPtr = (RetrievalInfo *) clientData;
    char *propInfo;
    Atom type;
    int format, result;
    unsigned long numItems, bytesAfter;

    if ((eventPtr->xproperty.atom != retrPtr->property)
	    || (eventPtr->xproperty.state != PropertyNewValue)
	    || (retrPtr->result != -1)) {
	return;
    }
    propInfo = NULL;
    result = XGetWindowProperty(eventPtr->xproperty.display,
	    eventPtr->xproperty.window, retrPtr->property, 0, MAX_PROP_WORDS,
	    True, (Atom) AnyPropertyType, &type, &format, &numItems,
	    &bytesAfter, (unsigned char **) &propInfo);
    if ((result != Success) || (type == None)) {
	return;
    }
    if (bytesAfter != 0) {
	Tcl_SetResult(retrPtr->interp, "selection property too large",
		TCL_STATIC);
	retrPtr->result = TCL_ERROR;
	goto done;
    }
    if (numItems == 0) {
	retrPtr->result = TCL_OK;
    } else if ((type == XA_STRING)
	    || (type == retrPtr->winPtr->dispPtr->textAtom)
	    || (type == retrPtr->winPtr->dispPtr->compoundTextAtom)) {
	if (format != 8) {
	    Tcl_SetResult(retrPtr->interp, (char *) NULL, TCL_STATIC);
	    sprintf(retrPtr->interp->result,
		"bad format for string selection: wanted \"8\", got \"%d\"",
		format);
	    retrPtr->result = TCL_ERROR;
	    goto done;
	}
	result = (*retrPtr->proc)(retrPtr->clientData, retrPtr->interp,
		propInfo);
	if (result != TCL_OK) {
	    retrPtr->result = result;
	}
    } else {
	char *string;

	if (format != 32) {
	    Tcl_SetResult(retrPtr->interp, (char *) NULL, TCL_STATIC);
	    sprintf(retrPtr->interp->result,
		"bad format for selection: wanted \"32\", got \"%d\"",
		format);
	    retrPtr->result = TCL_ERROR;
	    goto done;
	}
	string = SelCvtFromX((long *) propInfo, (int) numItems, type,
		(Tk_Window) retrPtr->winPtr);
	result = (*retrPtr->proc)(retrPtr->clientData, retrPtr->interp,
		string);
	if (result != TCL_OK) {
	    retrPtr->result = result;
	}
	ckfree(string);
    }

    done:
    XFree(propInfo);
    retrPtr->idleTime = 0;
}

/*
 *----------------------------------------------------------------------
 *
 * TkSelPropProc --
 *
 *	This procedure is invoked when property-change events
 *	occur on windows not known to the toolkit.  Its function
 *	is to implement the sending side of the INCR selection
 *	retrieval protocol when the selection requestor deletes
 *	the property containing a part of the selection.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	If the property that is receiving the selection was just
 *	deleted, then a new piece of the selection is fetched and
 *	placed in the property, until eventually there's no more
 *	selection to fetch.
 *
 *----------------------------------------------------------------------
 */

void
TkSelPropProc(eventPtr)
    register XEvent *eventPtr;		/* X PropertyChange event. */
{
    register IncrInfo *infoPtr;
    int i, format;
    Atom target;
    register TkSelHandler *selPtr;
    long buffer[TK_SEL_WORDS_AT_ONCE];
    int numItems;
    char *propPtr;
    Tk_ErrorHandler errorHandler;

    /*
     * See if this event announces the deletion of a property being
     * used for an INCR transfer.  If so, then add the next chunk of
     * data to the property.
     */

    if (eventPtr->xproperty.state != PropertyDelete) {
	return;
    }
    for (infoPtr = pendingIncrs; infoPtr != NULL;
	    infoPtr = infoPtr->nextPtr) {

	/*
	 * To avoid races between selection conversions and
	 * changes in selection ownership, make sure the window
	 * and timestamp for the current selection match those
	 * in the INCR request.
	 */

	if ((infoPtr->reqWindow != eventPtr->xproperty.window)
		|| (infoPtr->winPtr->dispPtr->selectionOwner
			!= (Tk_Window) infoPtr->winPtr)
		|| (infoPtr->winPtr->dispPtr->selectionTime
		!= infoPtr->time)) {
	    continue;
	}
	for (i = 0; i < infoPtr->numConversions; i++) {
	    if ((eventPtr->xproperty.atom != infoPtr->multAtoms[2*i + 1])
		    || (infoPtr->offsets[i] == -1)){
		continue;
	    }
	    target = infoPtr->multAtoms[2*i];
	    infoPtr->idleTime = 0;
	    for (selPtr = infoPtr->winPtr->selHandlerList; ;
		    selPtr = selPtr->nextPtr) {
		if (selPtr == NULL) {
		    infoPtr->multAtoms[2*i + 1] = None;
		    infoPtr->offsets[i] = -1;
		    infoPtr->numIncrs --;
		    return;
		}
		if (selPtr->target == target) {
		    if (infoPtr->offsets[i] == -2) {
			numItems = 0;
			((char *) buffer)[0] = 0;
		    } else {
			numItems = (*selPtr->proc)(selPtr->clientData,
				infoPtr->offsets[i], (char *) buffer,
				TK_SEL_BYTES_AT_ONCE);
			if (numItems > TK_SEL_BYTES_AT_ONCE) {
			    panic("selection handler returned too many bytes");
			} else {
			    if (numItems < 0) {
				numItems = 0;
			    }
			}
			((char *) buffer)[numItems] = '\0';
		    }
		    if (numItems < TK_SEL_BYTES_AT_ONCE) {
			if (numItems <= 0) {
			    infoPtr->offsets[i] = -1;
			    infoPtr->numIncrs--;
			} else {
			    infoPtr->offsets[i] = -2;
			}
		    } else {
			infoPtr->offsets[i] += numItems;
		    }
		    if (selPtr->format == XA_STRING) {
			propPtr = (char *) buffer;
			format = 8;
		    } else {
			propPtr = (char *) SelCvtToX((char *) buffer,
				selPtr->format,
				(Tk_Window) infoPtr->winPtr,
				&numItems);
			format = 32;
		    }
		    errorHandler = Tk_CreateErrorHandler(
			    eventPtr->xproperty.display, -1, -1, -1,
			    (int (*)()) NULL, (ClientData) NULL);
		    XChangeProperty(eventPtr->xproperty.display,
			    eventPtr->xproperty.window,
			    eventPtr->xproperty.atom, selPtr->format,
			    format, PropModeReplace,
			    (unsigned char *) propPtr, numItems);
		    Tk_DeleteErrorHandler(errorHandler);
		    if (propPtr != (char *) buffer) {
			ckfree(propPtr);
		    }
		    return;
		}
	    }
	}
    }
}

/*
 *----------------------------------------------------------------------
 *
 * HandleTclCommand --
 *
 *	This procedure acts as selection handler for handlers created
 *	by the "selection handle" command.  It invokes a Tcl command to
 *	retrieve the selection.
 *
 * Results:
 *	The return value is a count of the number of bytes actually
 *	stored at buffer.
 *
 * Side effects:
 *	None except for things done by the Tcl command.
 *
 *----------------------------------------------------------------------
 */

static int
HandleTclCommand(clientData, offset, buffer, maxBytes)
    ClientData clientData;	/* Information about command to execute. */
    int offset;			/* Return selection bytes starting at this
				 * offset. */
    char *buffer;		/* Place to store converted selection. */
    int maxBytes;		/* Maximum # of bytes to store at buffer. */
{
    register CommandInfo *cmdInfoPtr = (CommandInfo *) clientData;
    char *oldResultString;
    Tcl_FreeProc *oldFreeProc;
    int spaceNeeded, length;
#define MAX_STATIC_SIZE 100
    char staticSpace[MAX_STATIC_SIZE];
    char *command;

    /*
     * First, generate a command by taking the command string
     * and appending the offset and maximum # of bytes.
     */

    spaceNeeded = cmdInfoPtr->cmdLength + 30;
    if (spaceNeeded < MAX_STATIC_SIZE) {
	command = staticSpace;
    } else {
	command = (char *) ckalloc((unsigned) spaceNeeded);
    }
    sprintf(command, "%s %d %d", cmdInfoPtr->command, offset, maxBytes);

    /*
     * Execute the command.  Be sure to restore the state of the
     * interpreter after executing the command.
     */

    oldFreeProc = cmdInfoPtr->interp->freeProc;
    if (oldFreeProc != 0) {
	oldResultString = cmdInfoPtr->interp->result;
    } else {
	oldResultString = (char *) ckalloc((unsigned)
		(strlen(cmdInfoPtr->interp->result) + 1));
	strcpy(oldResultString, cmdInfoPtr->interp->result);
	oldFreeProc = TCL_DYNAMIC;
    }
    cmdInfoPtr->interp->freeProc = 0;
    if (Tcl_GlobalEval(cmdInfoPtr->interp, command) == TCL_OK) {
	length = strlen(cmdInfoPtr->interp->result);
    } else {
	length = 0;
    }
    if (length > maxBytes) {
	length = maxBytes;
    }
    memcpy((VOID *) buffer, (VOID *) cmdInfoPtr->interp->result, length);
    buffer[length] = '\0';
    Tcl_FreeResult(cmdInfoPtr->interp);
    cmdInfoPtr->interp->result = oldResultString;
    cmdInfoPtr->interp->freeProc = oldFreeProc;

    if (command != staticSpace) {
	ckfree(command);
    }

    return length;
}

/*
 *----------------------------------------------------------------------
 *
 * SelTimeoutProc --
 *
 *	This procedure is invoked once every second while waiting for
 *	the selection to be returned.  After a while it gives up and
 *	aborts the selection retrieval.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	A new timer callback is created to call us again in another
 *	second, unless time has expired, in which case an error is
 *	recorded for the retrieval.
 *
 *----------------------------------------------------------------------
 */

static void
SelTimeoutProc(clientData)
    ClientData clientData;		/* Information about retrieval
					 * in progress. */
{
    register RetrievalInfo *retrPtr = (RetrievalInfo *) clientData;

    /*
     * Make sure that the retrieval is still in progress.  Then
     * see how long it's been since any sort of response was received
     * from the other side.
     */

    if (retrPtr->result != -1) {
	return;
    }
    retrPtr->idleTime++;
    if (retrPtr->idleTime >= 5) {

	/*
	 * Use a careful procedure to store the error message, because
	 * the result could already be partially filled in with a partial
	 * selection return.
	 */

	Tcl_SetResult(retrPtr->interp, "selection owner didn't respond",
		TCL_STATIC);
	retrPtr->result = TCL_ERROR;
    } else {
	retrPtr->timeout = Tk_CreateTimerHandler(1000, SelTimeoutProc,
	    (ClientData) retrPtr);
    }
}

/*
 *----------------------------------------------------------------------
 *
 * IncrTimeoutProc --
 *
 *	This procedure is invoked once a second while sending the
 *	selection to a requestor in INCR mode.  After a while it
 *	gives up and aborts the selection operation.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	A new timeout gets registered so that this procedure gets
 *	called again in another second, unless too many seconds
 *	have elapsed, in which case infoPtr is marked as "all done".
 *
 *----------------------------------------------------------------------
 */

static void
IncrTimeoutProc(clientData)
    ClientData clientData;		/* Information about INCR-mode
					 * selection retrieval for which
					 * we are selection owner. */
{
    register IncrInfo *infoPtr = (IncrInfo *) clientData;

    infoPtr->idleTime++;
    if (infoPtr->idleTime >= 5) {
	infoPtr->numIncrs = 0;
    } else {
	infoPtr->timeout = Tk_CreateTimerHandler(1000, IncrTimeoutProc,
		(ClientData) infoPtr);
    }
}

/*
 *----------------------------------------------------------------------
 *
 * DefaultSelection --
 *
 *	This procedure is called to generate selection information
 *	for a few standard targets such as TIMESTAMP and TARGETS.
 *	It is invoked only if no handler has been declared by the
 *	application.
 *
 * Results:
 *	If "target" is a standard target understood by this procedure,
 *	the selection is converted to that form and stored as a
 *	character string in buffer.  The type of the selection (e.g.
 *	STRING or ATOM) is stored in *typePtr, and the return value is
 *	a count of the # of non-NULL bytes at buffer.  If the target
 *	wasn't understood, or if there isn't enough space at buffer
 *	to hold the entire selection (no INCR-mode transfers for this
 *	stuff!), then -1 is returned.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

static int
DefaultSelection(winPtr, target, buffer, maxBytes, typePtr)
    TkWindow *winPtr;		/* Window that owns selection. */
    Atom target;		/* Desired form of selection. */
    char *buffer;		/* Place to put selection characters. */
    int maxBytes;		/* Maximum # of bytes to store at buffer. */
    Atom *typePtr;		/* Store here the type of the selection,
				 * for use in converting to proper X format. */
{
    if (target == winPtr->dispPtr->timestampAtom) {
	if (maxBytes < 20) {
	    return -1;
	}
	sprintf(buffer, "%#x", winPtr->dispPtr->selectionTime);
	*typePtr = XA_INTEGER;
	return strlen(buffer);
    }

    if (target == winPtr->dispPtr->targetsAtom) {
	register TkSelHandler *selPtr;
	char *atomString;
	int length, atomLength;

	if (maxBytes < 50) {
	    return -1;
	}
	strcpy(buffer, "TARGETS MULTIPLE TIMESTAMP");
	length = strlen(buffer);
	for (selPtr = winPtr->selHandlerList; selPtr != NULL;
		selPtr = selPtr->nextPtr) {
	    atomString = Tk_GetAtomName((Tk_Window) winPtr, selPtr->target);
	    atomLength = strlen(atomString) + 1;
	    if ((length + atomLength) >= maxBytes) {
		return -1;
	    }
	    sprintf(buffer+length, " %s", atomString);
	    length += atomLength;
	}
	*typePtr = XA_ATOM;
	return length;
    }

    return -1;
}
