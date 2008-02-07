/* 
 * tkEvent.c --
 *
 *	This file provides basic event-managing facilities,
 *	whereby procedure callbacks may be attached to
 *	certain events.
 *
 * Copyright 1990-1992 Regents of the University of California.
 * Permission to use, copy, modify, and distribute this
 * software and its documentation for any purpose and without
 * fee is hereby granted, provided that the above copyright
 * notice appear in all copies.  The University of California
 * makes no representations about the suitability of this
 * software for any purpose.  It is provided "as is" without
 * express or implied warranty.
 */

#ifndef lint
static char rcsid[] = "$Header: /user6/ouster/wish/RCS/tkEvent.c,v 1.60 92/08/21 16:15:57 ouster Exp $ SPRITE (Berkeley)";
#endif

#include "tkconfig.h"
#include "tkint.h"
#include <errno.h>
#include <signal.h>
#include <sys/time.h>
#include <assert.h>


/*
 * For each timer callback that's pending, there is one record
 * of the following type, chained together in a list sorted by
 * time (earliest event first).
 */

typedef struct TimerEvent {
    struct timeval time;	/* When timer is to fire. */
    void (*proc)  _ANSI_ARGS_((ClientData clientData));
				/* Procedure to call. */
    ClientData clientData;	/* Argument to pass to proc. */
    Tk_TimerToken token;	/* Identifies event so it can be
				 * deleted. */
    struct TimerEvent *nextPtr;	/* Next event in queue, or NULL for
				 * end of queue. */
} TimerEvent;

static TimerEvent *timerQueue;	/* First event in queue. */

/*
 * The information below is used to provide read, write, and
 * exception masks to select during calls to Tk_DoOneEvent.
 */

static int readCount;		/* Number of files for which we */
static int writeCount;		/* care about each event type. */
static int exceptCount;
#define MASK_SIZE ((OPEN_MAX+(8*sizeof(int))-1)/(8*sizeof(int)))
static int masks[3*MASK_SIZE];	/* Integer array containing official
				 * copies of the three sets of
				 * masks. */
static int ready[3*MASK_SIZE];	/* Temporary copy of masks, passed
				 * to select and modified by kernel
				 * to indicate which files are
				 * actually ready. */
static int *readPtr;		/* Pointers to the portions of */
static int *writePtr;		/* *readyPtr for reading, writing, */
static int *exceptPtr;		/* and excepting.  Will be NULL if
				 * corresponding count (e.g. readCount
				 * is zero. */
static int numFds = 0;		/* Number of valid bits in mask
				 * arrays (this value is passed
				 * to select). */

/*
 * For each file registered in a call to Tk_CreateFileHandler,
 * and for each display that's currently active, there is one
 * record of the following type.  All of these records are
 * chained together into a single list.
 */

typedef struct FileEvent {
    int fd;			/* Descriptor number for this file. */
    int *readPtr;		/* Pointer to word in ready array
				 * for this file's read mask bit. */
    int *writePtr;		/* Same for write mask bit. */
    int *exceptPtr;		/* Same for except mask bit. */
    int mask;			/* Value to AND with mask word to
				 * select just this file's bit. */
    void (*proc)  _ANSI_ARGS_((ClientData clientData, int mask));
				/* Procedure to call.  NULL means
				 * this is a display. */
    ClientData clientData;	/* Argument to pass to proc.  For
				 * displays, this is a (Display *). */
    struct FileEvent *nextPtr;	/* Next in list of all files we
				 * care about (NULL for end of
				 * list). */
} FileEvent;

static FileEvent *fileList;	/* List of all file events. */

/*
 * There is one of the following structures for each of the
 * handlers declared in a call to Tk_DoWhenIdle.  All of the
 * currently-active handlers are linked together into a list.
 */

typedef struct IdleHandler {
    void (*proc)  _ANSI_ARGS_((ClientData clientData));
				/* Procedure to call. */
    ClientData clientData;	/* Value to pass to proc. */
    struct IdleHandler *nextPtr;/* Next in list of active handlers. */
} IdleHandler;

static IdleHandler *idleList = NULL;
				/* First in list of all idle handlers. */
static IdleHandler *lastIdlePtr = NULL;
				/* Last in list (or NULL for empty list). */

/*
 * There's a potential problem if a handler is deleted while it's
 * current (i.e. its procedure is executing), since Tk_HandleEvent
 * will need to read the handler's "nextPtr" field when the procedure
 * returns.  To handle this problem, structures of the type below
 * indicate the next handler to be processed for any (recursively
 * nested) dispatches in progress.  The nextHandler fields get
 * updated if the handlers pointed to are deleted.  Tk_HandleEvent
 * also needs to know if the entire window gets deleted;  the winPtr
 * field is set to zero if that particular window gets deleted.
 */

typedef struct InProgress {
    XEvent *eventPtr;		 /* Event currently being handled. */
    TkWindow *winPtr;		 /* Window for event.  Gets set to None if
				  * window is deleted while event is being
				  * handled. */
    TkEventHandler *nextHandler; /* Next handler in search. */
    struct InProgress *nextPtr;	 /* Next higher nested search. */
} InProgress;

static InProgress *pendingPtr = NULL;
				/* Topmost search in progress, or
				 * NULL if none. */

/*
 * For each call to Tk_CreateGenericHandler, an instance of the following
 * structure will be created.  All of the active handlers are linked into a
 * list.
 */

typedef struct GenericHandler {
    Tk_GenericProc *proc;	/* Procedure to dispatch on all X events. */
    ClientData clientData;	/* Client data to pass to procedure. */
    int deleteFlag;		/* Flag to set when this handler is deleted. */
    struct GenericHandler *nextPtr;
				/* Next handler in list of all generic
				 * handlers, or NULL for end of list. */
} GenericHandler;

static GenericHandler *genericList = NULL;
				/* First handler in the list, or NULL. */
static GenericHandler *lastGenericPtr = NULL;
				/* Last handler in list. */

/*
 * There's a potential problem if Tk_HandleEvent is entered recursively.
 * A handler cannot be deleted physically until we have returned from
 * calling it.  Otherwise, we're looking at unallocated memory in advancing to
 * its `next' entry.  We deal with the problem by using the `delete flag' and
 * deleting handlers only when it's known that there's no handler active.
 *
 * The following variable has a non-zero value when a handler is active.
 */

static int genericHandlersActive = 0;

/*
 * Array of event masks corresponding to each X event:
 */

static unsigned long eventMasks[] = {
    0,
    0,
    KeyPressMask,			/* KeyPress */
    KeyReleaseMask,			/* KeyRelease */
    ButtonPressMask,			/* ButtonPress */
    ButtonReleaseMask,			/* ButtonRelease */
    PointerMotionMask|PointerMotionHintMask|ButtonMotionMask
	    |Button1MotionMask|Button2MotionMask|Button3MotionMask
	    |Button4MotionMask|Button5MotionMask,
					/* MotionNotify */
    EnterWindowMask,			/* EnterNotify */
    LeaveWindowMask,			/* LeaveNotify */
    FocusChangeMask,			/* FocusIn */
    FocusChangeMask,			/* FocusOut */
    KeymapStateMask,			/* KeymapNotify */
    ExposureMask,			/* Expose */
    ExposureMask,			/* GraphicsExpose */
    ExposureMask,			/* NoExpose */
    VisibilityChangeMask,		/* VisibilityNotify */
    SubstructureNotifyMask,		/* CreateNotify */
    StructureNotifyMask,		/* DestroyNotify */
    StructureNotifyMask,		/* UnmapNotify */
    StructureNotifyMask,		/* MapNotify */
    SubstructureRedirectMask,		/* MapRequest */
    StructureNotifyMask,		/* ReparentNotify */
    StructureNotifyMask,		/* ConfigureNotify */
    SubstructureRedirectMask,		/* ConfigureRequest */
    StructureNotifyMask,		/* GravityNotify */
    ResizeRedirectMask,			/* ResizeRequest */
    StructureNotifyMask,		/* CirculateNotify */
    SubstructureRedirectMask,		/* CirculateRequest */
    PropertyChangeMask,			/* PropertyNotify */
    0,					/* SelectionClear */
    0,					/* SelectionRequest */
    0,					/* SelectionNotify */
    ColormapChangeMask,			/* ColormapNotify */
    0,					/* ClientMessage */
    0,					/* Mapping Notify */
};

/*
 * If someone has called Tk_RestrictEvents, the information below
 * keeps track of it.
 */

static Bool (*restrictProc)  _ANSI_ARGS_((Display *display, XEvent *eventPtr,
    char *arg));		/* Procedure to call.  NULL means no
				 * restrictProc is currently in effect. */
static char *restrictArg;	/* Argument to pass to restrictProc. */

/*
 * The following array keeps track of the last TK_NEVENTS X events, for
 * memory dump analysis.  The tracing is only done if tkEventDebug is set
 * to 1.
 */

#define TK_NEVENTS 32
static XEvent eventTrace[TK_NEVENTS];
static int traceIndex = 0;
int tkEventDebug = 0;

int tkCollapseMotion = 1;
int tkMustExit = 0;


#define DefPool(type) \
type *Unused##type = NULL; \
\
type *New##type() { \
  if (Unused##type == NULL) { \
    return (type *)ckalloc(sizeof (type)); \
  } else { \
    type *ptr = Unused##type; \
    Unused##type = ptr->nextPtr; \
    return (ptr); \
  } \
} \
\
void Free##type(type *ptr) { \
  ptr->nextPtr = Unused##type; \
  Unused##type = ptr; \
}

DefPool(TkEventHandler)
DefPool(GenericHandler)
DefPool(FileEvent)
DefPool(TimerEvent)
DefPool(IdleHandler)


/*
 *--------------------------------------------------------------
 *
 * Tk_CreateEventHandler --
 *
 *	Arrange for a given procedure to be invoked whenever
 *	events from a given class occur in a given window.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	From now on, whenever an event of the type given by
 *	mask occurs for token and is processed by Tk_HandleEvent,
 *	proc will be called.  See the manual entry for details
 *	of the calling sequence and return value for proc.
 *
 *--------------------------------------------------------------
 */

void
Tk_CreateEventHandler(token, mask, proc, clientData)
    Tk_Window token;		/* Token for window in which to
				 * create handler. */
    unsigned long mask;		/* Events for which proc should
				 * be called. */
    Tk_EventProc *proc;		/* Procedure to call for each
				 * selected event */
    ClientData clientData;	/* Arbitrary data to pass to proc. */
{
    register TkEventHandler *handlerPtr;
    register TkWindow *winPtr = (TkWindow *) token;
    int found;

    /*
     * Skim through the list of existing handlers to (a) compute the
     * overall event mask for the window (so we can pass this new
     * value to the X system) and (b) see if there's already a handler
     * declared with the same callback and clientData (if so, just
     * change the mask).  If no existing handler matches, then create
     * a new handler.
     */

    found = 0;
    if (winPtr->handlerList == NULL) {
	handlerPtr = (TkEventHandler *) NewTkEventHandler();
	winPtr->handlerList = handlerPtr;
	goto initHandler;
    } else {
	for (handlerPtr = winPtr->handlerList; ;
		handlerPtr = handlerPtr->nextPtr) {
	    if ((handlerPtr->proc == proc)
		    && (handlerPtr->clientData == clientData)) {
		handlerPtr->mask = mask;
		found = 1;
	    }
	    if (handlerPtr->nextPtr == NULL) {
		break;
	    }
	}
    }

    /*
     * Create a new handler if no matching old handler was found.
     */

    if (!found) {
	handlerPtr->nextPtr = NewTkEventHandler();
	handlerPtr = handlerPtr->nextPtr;
	initHandler:
	handlerPtr->mask = mask;
	handlerPtr->proc = proc;
	handlerPtr->clientData = clientData;
	handlerPtr->nextPtr = NULL;
    }

    /*
     * No need to call XSelectInput:  Tk always selects on all events
     * for all windows (needed to support bindings on classes and "all").
     */
}

/*
 *--------------------------------------------------------------
 *
 * Tk_DeleteEventHandler --
 *
 *	Delete a previously-created handler.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	If there existed a handler as described by the
 *	parameters, the handler is deleted so that proc
 *	will not be invoked again.
 *
 *--------------------------------------------------------------
 */

void
Tk_DeleteEventHandler(token, mask, proc, clientData)
    Tk_Window token;		/* Same as corresponding arguments passed */
    unsigned long mask;		/* previously to Tk_CreateEventHandler. */
    Tk_EventProc *proc;
    ClientData clientData;
{
    register TkEventHandler *handlerPtr;
    register InProgress *ipPtr;
    TkEventHandler *prevPtr;
    register TkWindow *winPtr = (TkWindow *) token;

    /*
     * Find the event handler to be deleted, or return
     * immediately if it doesn't exist.
     */

    for (handlerPtr = winPtr->handlerList, prevPtr = NULL; ;
	    prevPtr = handlerPtr, handlerPtr = handlerPtr->nextPtr) {
	if (handlerPtr == NULL) {
	    return;
	}
	if ((handlerPtr->mask == mask) && (handlerPtr->proc == proc)
		&& (handlerPtr->clientData == clientData)) {
	    break;
	}
    }

    /*
     * If Tk_HandleEvent is about to process this handler, tell it to
     * process the next one instead.
     */

    for (ipPtr = pendingPtr; ipPtr != NULL; ipPtr = ipPtr->nextPtr) {
	if (ipPtr->nextHandler == handlerPtr) {
	    ipPtr->nextHandler = handlerPtr->nextPtr;
	}
    }

    /*
     * Free resources associated with the handler.
     */

    if (prevPtr == NULL) {
	winPtr->handlerList = handlerPtr->nextPtr;
    } else {
	prevPtr->nextPtr = handlerPtr->nextPtr;
    }
    (void) FreeTkEventHandler(handlerPtr);


    /*
     * No need to call XSelectInput:  Tk always selects on all events
     * for all windows (needed to support bindings on classes and "all").
     */
}

/*--------------------------------------------------------------
 *
 * Tk_CreateGenericHandler --
 *
 *	Register a procedure to be called on each X event, regardless
 *	of display or window.  Generic handlers are useful for capturing
 *	events that aren't associated with windows, or events for windows
 *	not managed by Tk.
 *
 * Results:
 *	None.
 *
 * Side Effects:
 *	From now on, whenever an X event is given to Tk_HandleEvent,
 *	invoke proc, giving it clientData and the event as arguments.
 *
 *--------------------------------------------------------------
 */

void
Tk_CreateGenericHandler(proc, clientData)
     Tk_GenericProc *proc;	/* Procedure to call on every event. */
     ClientData clientData;	/* One-word value to pass to proc. */
{
    GenericHandler *handlerPtr;
    
    handlerPtr = NewGenericHandler();
    
    handlerPtr->proc = proc;
    handlerPtr->clientData = clientData;
    handlerPtr->deleteFlag = 0;
    handlerPtr->nextPtr = NULL;
    if (genericList == NULL) {
	genericList = handlerPtr;
    } else {
	lastGenericPtr->nextPtr = handlerPtr;
    }
    lastGenericPtr = handlerPtr;
}

/*
 *--------------------------------------------------------------
 *
 * Tk_DeleteGenericHandler --
 *
 *	Delete a previously-created generic handler.
 *
 * Results:
 *	None.
 *
 * Side Effects:
 *	If there existed a handler as described by the parameters,
 *	that handler is logically deleted so that proc will not be
 *	invoked again.  The physical deletion happens in the event
 *	loop in Tk_HandleEvent.
 *
 *--------------------------------------------------------------
 */

void
Tk_DeleteGenericHandler(proc, clientData)
     Tk_GenericProc *proc;
     ClientData clientData;
{
    GenericHandler * handler;
    
    for (handler = genericList; handler; handler = handler->nextPtr) {
	if ((handler->proc == proc) && (handler->clientData == clientData)) {
	    handler->deleteFlag = 1;
	}
    }
}

/*
 *--------------------------------------------------------------
 *
 * Tk_HandleEvent --
 *
 *	Given an event, invoke all the handlers that have
 *	been registered for the event.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Depends on the handlers.
 *
 *--------------------------------------------------------------
 */

void
Tk_HandleEvent(eventPtr)
    XEvent *eventPtr;		/* Event to dispatch. */
{
    register TkEventHandler *handlerPtr;
    register GenericHandler *genericPtr;
    register GenericHandler *genPrevPtr;
    TkWindow *winPtr;
    register unsigned long mask;
    InProgress ip;
    Window handlerWindow;

    /* 
     * First off, invoke all the generic event handlers (those that are
     * invoked for all events).  If a generic event handler reports that
     * an event is fully processed, go no further.
     */

    for (genPrevPtr = NULL, genericPtr = genericList;  genericPtr != NULL; ) {
	if (genericPtr->deleteFlag) {
	    if (!genericHandlersActive) {
		GenericHandler *tmpPtr;

		/*
		 * This handler needs to be deleted and there are no
		 * calls pending through the handler, so now is a safe
		 * time to delete it.
		 */

		tmpPtr = genericPtr->nextPtr;
		if (genPrevPtr == NULL) {
		    genericList = tmpPtr;
		} else {
		    genPrevPtr->nextPtr = tmpPtr;
		}
		(void) FreeGenericHandler(genericPtr);
		genericPtr = tmpPtr;
		continue;
	    }
	} else {
	    int done;

	    genericHandlersActive++;
	    done = (*genericPtr->proc)(genericPtr->clientData, eventPtr);
	    genericHandlersActive--;
	    if (done) {
		return;
	    }
	}
	genPrevPtr = genericPtr;
	genericPtr = genPrevPtr->nextPtr;
    }

    /*
     * Events selected by StructureNotify look the same as those
     * selected by SubstructureNotify;  the only difference is
     * whether the "event" and "window" fields are the same.
     * Check it out and convert StructureNotify to
     * SubstructureNotify if necessary.
     */

    handlerWindow = eventPtr->xany.window;
    mask = eventMasks[eventPtr->xany.type];
    if (mask == StructureNotifyMask) {
	if (eventPtr->xmap.event != eventPtr->xmap.window) {
	    mask = SubstructureNotifyMask;
	    handlerWindow = eventPtr->xmap.event;
	}
    }
    if (XFindContext(eventPtr->xany.display, handlerWindow,
	    tkWindowContext, (void *) &winPtr) != 0) {

	/*
	 * There isn't a TkWindow structure for this window.
	 * However, if the event is a PropertyNotify event then call
	 * the selection manager (it deals beneath-the-table with
	 * certain properties).
	 */

	if (eventPtr->type == PropertyNotify) {
	    TkSelPropProc(eventPtr);
	}
	return;
    }

    /*
     * Redirect KeyPress and KeyRelease events if input focussing
     * is happening.  Map the x and y coordinates between the two
     * windows, if possible (make both -1 if the map-from and map-to
     * windows don't share the same top-level window).
     */

    if (mask & (KeyPressMask|KeyReleaseMask)) {
	winPtr->dispPtr->lastEventTime = eventPtr->xkey.time;
/* XXX: FOCUS */
	    if (winPtr->dispPtr->focusPtr != NULL) {
	    TkWindow *focusPtr;
	    int winX, winY, focusX, focusY;
    
	    focusPtr = winPtr->dispPtr->focusPtr;
	    if ((focusPtr->display != winPtr->display)
		    || (focusPtr->screenNum != winPtr->screenNum)) {
		eventPtr->xkey.x = -1;
		eventPtr->xkey.y = -1;
	    } else {
		Tk_GetRootCoords((Tk_Window) winPtr, &winX, &winY);
		Tk_GetRootCoords((Tk_Window) focusPtr, &focusX, &focusY);
		eventPtr->xkey.x -= focusX - winX;
		eventPtr->xkey.y -= focusY - winY;
	    }
	    eventPtr->xkey.window = focusPtr->window;
	    winPtr = focusPtr;
	}
    }

    /*
     * Call a grab-related procedure to do special processing on
     * pointer events.
     */

    if (mask & (ButtonPressMask|ButtonReleaseMask|PointerMotionMask
	    |EnterWindowMask|LeaveWindowMask)) {
	if (mask & (ButtonPressMask|ButtonReleaseMask)) {
	    winPtr->dispPtr->lastEventTime = eventPtr->xbutton.time;
	} else if (mask & PointerMotionMask) {
	    winPtr->dispPtr->lastEventTime = eventPtr->xmotion.time;
	} else {
	    winPtr->dispPtr->lastEventTime = eventPtr->xcrossing.time;
	}
	if (TkPointerEvent(eventPtr, winPtr) == 0) {
	    return;
	}
    }

    /*
     * For events where it hasn't already been done, update the current
     * time in the display.
     */

    if (eventPtr->type == PropertyNotify) {
	winPtr->dispPtr->lastEventTime = eventPtr->xproperty.time;
    }

    /*
     * There's a potential interaction here with Tk_DeleteEventHandler.
     * Read the documentation for pendingPtr.
     */

    ip.eventPtr = eventPtr;
    ip.winPtr = winPtr;
    ip.nextHandler = NULL;
    ip.nextPtr = pendingPtr;
    pendingPtr = &ip;
    if (mask == 0) {
	if ((eventPtr->type == SelectionClear)
		|| (eventPtr->type == SelectionRequest)
		|| (eventPtr->type == SelectionNotify)) {
	    TkSelEventProc((Tk_Window) winPtr, eventPtr);
	} else if ((eventPtr->type == ClientMessage)
		&& (eventPtr->xclient.message_type == 
		    Tk_InternAtom((Tk_Window) winPtr, "WM_PROTOCOLS"))) {
		/*
		 * this is a ICCCM WM_PROTOCOL ClientMessage 
		 */
		TkWmProtocolEventProc(winPtr, eventPtr);
	}
    } else {
	for (handlerPtr = winPtr->handlerList; handlerPtr != NULL; ) {
	    if ((handlerPtr->mask & mask) != 0) {
		ip.nextHandler = handlerPtr->nextPtr;
		(*(handlerPtr->proc))(handlerPtr->clientData, eventPtr);
		handlerPtr = ip.nextHandler;
	    } else {
		handlerPtr = handlerPtr->nextPtr;
	    }
	}

	/*
	 * Pass the event to the "bind" command mechanism.  But, don't
	 * do this for SubstructureNotify events.  The "bind" command
	 * doesn't support them anyway, and it's easier to filter out
	 * these events here than in the lower-level procedures.
	 */

	if ((ip.winPtr != None) && (mask != SubstructureNotifyMask)) {
	    TkBindEventProc(winPtr, eventPtr);
	}
    }
    pendingPtr = ip.nextPtr;
}

/*
 *--------------------------------------------------------------
 *
 * Tk_CreateFileHandler --
 *
 *	Arrange for a given procedure to be invoked whenever
 *	a given file becomes readable or writable.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	From now on, whenever the I/O channel given by fd becomes
 *	ready in the way indicated by mask, proc will be invoked.
 *	See the manual entry for details on the calling sequence
 *	to proc.  If fd is already registered then the old mask
 *	and proc and clientData values will be replaced with
 *	new ones.
 *
 *--------------------------------------------------------------
 */

void
Tk_CreateFileHandler(fd, mask, proc, clientData)
    int fd;			/* Integer identifier for stream. */
    int mask;			/* OR'ed combination of TK_READABLE,
				 * TK_WRITABLE, and TK_EXCEPTION:
				 * indicates conditions under which
				 * proc should be called. */
    Tk_FileProc *proc;		/* Procedure to call for each
				 * selected event.  NULL means that
				 * this is a display, and that
				 * clientData is the (Display *)
				 * for it, and that events should
				 * be handled automatically. */
    ClientData clientData;	/* Arbitrary data to pass to proc. */
{
    register FileEvent *filePtr;
    int index;

    if (fd >= OPEN_MAX) {
	panic("Tk_CreatefileHandler can't handle file id %d", fd);
    }

    /*
     * Make sure the file isn't already registered.  Create a
     * new record in the normal case where there's no existing
     * record.
     */

    for (filePtr = fileList; filePtr != NULL;
	    filePtr = filePtr->nextPtr) {
	if (filePtr->fd == fd) {
	    break;
	}
    }
    index = fd/(8*sizeof(int));
    if (filePtr == NULL) {
	filePtr = NewFileEvent();
	filePtr->fd = fd;
	filePtr->readPtr = &ready[index];
	filePtr->writePtr = &ready[index+MASK_SIZE];
	filePtr->exceptPtr = &ready[index+2*MASK_SIZE];
	filePtr->mask = 1 << (fd%(8*sizeof(int)));
	filePtr->nextPtr = fileList;
	fileList = filePtr;
    } else {
	if (masks[index] & filePtr->mask) {
	    readCount--;
	    *filePtr->readPtr &= ~filePtr->mask;
	    masks[index] &= ~filePtr->mask;
	}
	if (masks[index+MASK_SIZE] & filePtr->mask) {
	    writeCount--;
	    *filePtr->writePtr &= ~filePtr->mask;
	    masks[index+MASK_SIZE] &= ~filePtr->mask;
	}
	if (masks[index+2*MASK_SIZE] & filePtr->mask) {
	    exceptCount--;
	    *filePtr->exceptPtr &= ~filePtr->mask;
	    masks[index+2*MASK_SIZE] &= ~filePtr->mask;
	}
    }

    /*
     * The remainder of the initialization below is done
     * regardless of whether or not this is a new record
     * or a modification of an old one.
     */

    if (mask & TK_READABLE) {
	masks[index] |= filePtr->mask;
	readCount++;
    }
    readPtr = (readCount == 0 ? NULL : &ready[0]);

    if (mask & TK_WRITABLE) {
	masks[index+MASK_SIZE] |= filePtr->mask;
	writeCount++;
    }
    writePtr = (writeCount == 0 ? NULL : &ready[MASK_SIZE]);

    if (mask & TK_EXCEPTION) {
	masks[index+2*MASK_SIZE] |= filePtr->mask;
	exceptCount++;
    }
    exceptPtr = (exceptCount == 0 ? NULL : &ready[2*MASK_SIZE]);

    filePtr->proc = proc;
    filePtr->clientData = clientData;

    if (numFds <= fd) {
	numFds = fd+1;
    }
}

/*
 *--------------------------------------------------------------
 *
 * Tk_DeleteFileHandler --
 *
 *	Cancel a previously-arranged callback arrangement for
 *	a file.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	If a callback was previously registered on fd, remove it.
 *
 *--------------------------------------------------------------
 */

void
Tk_DeleteFileHandler(fd)
    int fd;			/* Stream id for which to remove
				 * callback procedure. */
{
    register FileEvent *filePtr;
    FileEvent *prevPtr;
    int index;

    /*
     * Find the entry for the given file (and return if there
     * isn't one).
     */

    for (prevPtr = NULL, filePtr = fileList; ;
	    prevPtr = filePtr, filePtr = filePtr->nextPtr) {
	if (filePtr == NULL) {
	    return;
	}
	if (filePtr->fd == fd) {
	    break;
	}
    }

    /*
     * Clean up information in the callback record.
     */

    index = filePtr->fd/(8*sizeof(int));
    if (masks[index] & filePtr->mask) {
	readCount--;
	*filePtr->readPtr &= ~filePtr->mask;
	masks[index] &= ~filePtr->mask;
    }
    if (masks[index+MASK_SIZE] & filePtr->mask) {
	writeCount--;
	*filePtr->writePtr &= ~filePtr->mask;
	masks[index+MASK_SIZE] &= ~filePtr->mask;
    }
    if (masks[index+2*MASK_SIZE] & filePtr->mask) {
	exceptCount--;
	*filePtr->exceptPtr &= ~filePtr->mask;
	masks[index+2*MASK_SIZE] &= ~filePtr->mask;
    }
    if (prevPtr == NULL) {
	fileList = filePtr->nextPtr;
    } else {
	prevPtr->nextPtr = filePtr->nextPtr;
    }
    FreeFileEvent(filePtr);

    /*
     * Recompute numFds.
     */

    numFds = 0;
    for (filePtr = fileList; filePtr != NULL;
	    filePtr = filePtr->nextPtr) {
	if (numFds <= filePtr->fd) {
	    numFds = filePtr->fd+1;
	}
    }
}

/*
 *--------------------------------------------------------------
 *
 * Tk_CreateTimerHandler --
 *
 *	Arrange for a given procedure to be invoked at a particular
 *	time in the future.
 *
 * Results:
 *	The return value is a token for the timer event, which
 *	may be used to delete the event before it fires.
 *
 * Side effects:
 *	When milliseconds have elapsed, proc will be invoked
 *	exactly once.
 *
 *--------------------------------------------------------------
 */

Tk_TimerToken
Tk_CreateTimerHandler(milliseconds, proc, clientData)
    int milliseconds;		/* How many milliseconds to wait
				 * before invoking proc. */
    Tk_TimerProc *proc;		/* Procedure to invoke. */
    ClientData clientData;	/* Arbitrary data to pass to proc. */
{
    register TimerEvent *timerPtr, *tPtr2, *prevPtr;
    static int id = 0;

    timerPtr = NewTimerEvent();

    /*
     * Compute when the event should fire.
     */

    (void) gettimeofday(&timerPtr->time, (struct timezone *) NULL);
    timerPtr->time.tv_sec += milliseconds/1000;
    timerPtr->time.tv_usec += (milliseconds%1000)*1000;
    if (timerPtr->time.tv_usec > 1000000) {
	timerPtr->time.tv_usec -= 1000000;
	timerPtr->time.tv_sec += 1;
    }

    /*
     * Fill in other fields for the event.
     */

    timerPtr->proc = proc;
    timerPtr->clientData = clientData;
    id++;
    timerPtr->token = (Tk_TimerToken) id;

    /*
     * Add the event to the queue in the correct position
     * (ordered by event firing time).
     */

    for (tPtr2 = timerQueue, prevPtr = NULL; tPtr2 != NULL;
	    prevPtr = tPtr2, tPtr2 = tPtr2->nextPtr) {
	if ((tPtr2->time.tv_sec > timerPtr->time.tv_sec)
		|| ((tPtr2->time.tv_sec == timerPtr->time.tv_sec)
		&& (tPtr2->time.tv_usec > timerPtr->time.tv_usec))) {
	    break;
	}
    }
    if (prevPtr == NULL) {
	timerPtr->nextPtr = timerQueue;
	timerQueue = timerPtr;
    } else {
	timerPtr->nextPtr = prevPtr->nextPtr;
	prevPtr->nextPtr = timerPtr;
    }
    return timerPtr->token;
}

// Added by Don to support finer timer resolution.
/*
 *--------------------------------------------------------------
 *
 * Tk_CreateMicroTimerHandler --
 *
 *	Arrange for a given procedure to be invoked at a particular
 *	time in the future.
 *
 * Results:
 *	The return value is a token for the timer event, which
 *	may be used to delete the event before it fires.
 *
 * Side effects:
 *	When seconds and seconds have elapsed, proc will be invoked
 *	exactly once.
 *
 *--------------------------------------------------------------
 */

Tk_TimerToken
Tk_CreateMicroTimerHandler(seconds, microseconds, proc, clientData)
    int seconds;		/* How many seconds to wait
				 * before invoking proc. */
    int microseconds;		/* How many microseconds to wait
				 * before invoking proc. */
    Tk_TimerProc *proc;		/* Procedure to invoke. */
    ClientData clientData;	/* Arbitrary data to pass to proc. */
{
    register TimerEvent *timerPtr, *tPtr2, *prevPtr;
    static int id = 0;

    timerPtr = NewTimerEvent();

    /*
     * Compute when the event should fire.
     */

    (void) gettimeofday(&timerPtr->time, (struct timezone *) NULL);
    timerPtr->time.tv_sec += seconds;
    timerPtr->time.tv_usec += microseconds;
    while (timerPtr->time.tv_usec > 1000000) {
	timerPtr->time.tv_usec -= 1000000;
	timerPtr->time.tv_sec += 1;
    }

    /*
     * Fill in other fields for the event.
     */

    timerPtr->proc = proc;
    timerPtr->clientData = clientData;
    id++;
    timerPtr->token = (Tk_TimerToken) id;

    /*
     * Add the event to the queue in the correct position
     * (ordered by event firing time).
     */

    for (tPtr2 = timerQueue, prevPtr = NULL; tPtr2 != NULL;
	    prevPtr = tPtr2, tPtr2 = tPtr2->nextPtr) {
	if ((tPtr2->time.tv_sec > timerPtr->time.tv_sec)
		|| ((tPtr2->time.tv_sec == timerPtr->time.tv_sec)
		&& (tPtr2->time.tv_usec > timerPtr->time.tv_usec))) {
	    break;
	}
    }
    if (prevPtr == NULL) {
	timerPtr->nextPtr = timerQueue;
	timerQueue = timerPtr;
    } else {
	timerPtr->nextPtr = prevPtr->nextPtr;
	prevPtr->nextPtr = timerPtr;
    }
    return timerPtr->token;
}


/*
 *--------------------------------------------------------------
 *
 * Tk_DeleteTimerHandler --
 *
 *	Delete a previously-registered timer handler.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Destroy the timer callback identified by TimerToken,
 *	so that its associated procedure will not be called.
 *	If the callback has already fired, or if the given
 *	token doesn't exist, then nothing happens.
 *
 *--------------------------------------------------------------
 */

void
Tk_DeleteTimerHandler(token)
    Tk_TimerToken token;	/* Result previously returned by
				 * Tk_DeleteTimerHandler. */
{
    register TimerEvent *timerPtr, *prevPtr;

    if (token == 0) return;

    for (timerPtr = timerQueue, prevPtr = NULL; timerPtr != NULL;
	    prevPtr = timerPtr, timerPtr = timerPtr->nextPtr) {
	if (timerPtr->token != token) {
	    continue;
	}
	if (prevPtr == NULL) {
	    timerQueue = timerPtr->nextPtr;
	} else {
	    prevPtr->nextPtr = timerPtr->nextPtr;
	}
	FreeTimerEvent(timerPtr);
	return;
    }

//    fprintf(stderr, "Tk_DeleteTimerHandler called on bogus timer %d\n", token);
}

/*
 *--------------------------------------------------------------
 *
 * Tk_DoWhenIdle --
 *
 *	Arrange for proc to be invoked the next time the
 *	system is idle (i.e., just before the next time
 *	that Tk_DoOneEvent would have to wait for something
 *	to happen).
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Proc will eventually be called, with clientData
 *	as argument.  See the manual entry for details.
 *
 *--------------------------------------------------------------
 */

void
Tk_DoWhenIdle(proc, clientData)
    Tk_IdleProc *proc;		/* Procedure to invoke. */
    ClientData clientData;	/* Arbitrary value to pass to proc. */
{
    register IdleHandler *idlePtr;

    idlePtr = NewIdleHandler();
    idlePtr->proc = proc;
    idlePtr->clientData = clientData;
    idlePtr->nextPtr = NULL;
    if (lastIdlePtr == NULL) {
	idleList = idlePtr;
    } else {
	lastIdlePtr->nextPtr = idlePtr;
    }
    lastIdlePtr = idlePtr;
}

/*
 *----------------------------------------------------------------------
 *
 * Tk_CancelIdleCall --
 *
 *	If there are any when-idle calls requested to a given procedure
 *	with given clientData, cancel all of them.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	If the proc/clientData combination were on the when-idle list,
 *	they are removed so that they will never be called.
 *
 *----------------------------------------------------------------------
 */

void
Tk_CancelIdleCall(proc, clientData)
    Tk_IdleProc *proc;		/* Procedure that was previously registered. */
    ClientData clientData;	/* Arbitrary value to pass to proc. */
{
    register IdleHandler *idlePtr, *prevPtr;
    IdleHandler *nextPtr;

    for (prevPtr = NULL, idlePtr = idleList; idlePtr != NULL;
	    prevPtr = idlePtr, idlePtr = idlePtr->nextPtr) {
	while ((idlePtr->proc == proc)
		&& (idlePtr->clientData == clientData)) {
	    nextPtr = idlePtr->nextPtr;
	    FreeIdleHandler(idlePtr);
	    idlePtr = nextPtr;
	    if (prevPtr == NULL) {
		idleList = idlePtr;
	    } else {
		prevPtr->nextPtr = idlePtr;
	    }
	    if (idlePtr == NULL) {
		lastIdlePtr = prevPtr;
		return;
	    }
	}
    }
}

/*
 *--------------------------------------------------------------
 *
 * Tk_DoOneEvent --
 *
 *	Process a single event of some sort.  If there's no
 *	work to do, wait for an event to occur, then process
 *	it.
 *
 * Results:
 *	The return value is 1 if the procedure actually found
 *	an event to process.  If no event was found then 0 is
 *	returned.
 *
 * Side effects:
 *	May delay execution of process while waiting for an
 *	X event, X error, file-ready event, or timer event.
 *	The handling of the event could cause additional
 *	side effects.  Collapses sequences of mouse-motion
 *	events for the same window into a single event by
 *	delaying motion event processing.
 *
 *--------------------------------------------------------------
 */

int
Tk_DoOneEvent(flags)
    int flags;			/* Miscellaneous flag values:  may be any
				 * combination of TK_DONT_WAIT, TK_X_EVENTS,
				 * TK_FILE_EVENTS, TK_TIMER_EVENTS, and
				 * TK_IDLE_EVENTS. */
{
    register FileEvent *filePtr;
    struct timeval curTime, timeout, *timeoutPtr;
    int numFound;
    static XEvent delayedMotionEvent;	/* Used to hold motion events that
					 * are being saved until later. */
    static int eventDelayed = 0;	/* Non-zero means there is an event
					 * in delayedMotionEvent. */

    if ((flags & TK_ALL_EVENTS) == 0) {
	flags |= TK_ALL_EVENTS;
    }

    /*
     * Phase One: see if there's already something ready
     * (either a file or a display) that was left over
     * from before (i.e don't do a select, just check the
     * bits from the last select).
     */

    checkFiles:
    for (filePtr = fileList; filePtr != NULL;
	    filePtr = filePtr->nextPtr) {
	int mask;

	/*
	 * Displays:  flush output, check for queued events,
	 * and read events from the server if display is ready.
	 * If there are any events, process one and then
	 * return.
	 */

	if ((filePtr->proc == NULL) && (flags & TK_X_EVENTS)) {
	    Display *display = (Display *) filePtr->clientData;
	    XEvent event;

	    XFlush(display);
	    if ((*filePtr->readPtr) & filePtr->mask) {
		*filePtr->readPtr &= ~filePtr->mask;
		if (XEventsQueued(display, QueuedAfterReading) == 0) {

		    /*
		     * Things are very tricky if there aren't any events
		     * readable at this point (after all, there was
		     * supposedly data available on the connection).
		     * A couple of things could have occurred:
		     * 
		     * One possibility is that there were only error events
		     * in the input from the server.  If this happens,
		     * we should return (we don't want to go to sleep
		     * in XNextEvent below, since this would block out
		     * other sources of input to the process).
		     *
		     * Another possibility is that our connection to the
		     * server has been closed.  This will not necessarily
		     * be detected in XEventsQueued (!!), so if we just
		     * return then there will be an infinite loop.  To
		     * detect such an error, generate a NoOp protocol
		     * request to exercise the connection to the server,
		     * then return.  However, must disable SIGPIPE while
		     * sending the event, or else the process will die
		     * from the signal and won't invoke the X error
		     * function to print a nice message.
		     */

		    void (*oldHandler)();

		    oldHandler = (void (*)()) signal(SIGPIPE, SIG_IGN);
		    XNoOp(display);
		    XFlush(display);
		    (void) signal(SIGPIPE, oldHandler);
		    return 1;
		}
		if (restrictProc != NULL) {
		    if (!XCheckIfEvent(display, &event, restrictProc,
			    restrictArg)) {
			return 1;
		    }
		} else {
		    XNextEvent(display, &event);
		}
	    } else {
		if (QLength(display) == 0) {
		    continue;
		}
		if (restrictProc != NULL) {
		    if (!XCheckIfEvent(display, &event, restrictProc,
			    restrictArg)) {
			continue;
		    }
		} else {
		    XNextEvent(display, &event);
		}
	    }

	    /*
	     * Got an event.  Deal with mouse-motion-collapsing and
	     * event-delaying here.  If there's already an event delayed,
	     * then process that event if it's incompatible with the new
	     * event (new event not mouse motion, or window changed, or
	     * state changed).  If the new event is mouse motion, then
	     * don't process it now;  delay it until later in the hopes
	     * that it can be merged with other mouse motion events
	     * immediately following.
	     */

	    if (tkEventDebug) {
		eventTrace[traceIndex] = event;
		traceIndex = (traceIndex+1) % TK_NEVENTS;
	    }

	    if (eventDelayed) {
		if (((event.type != MotionNotify)
			    && (event.type != GraphicsExpose)
			    && (event.type != NoExpose)
			    && (event.type != Expose))
			|| (event.xmotion.display
			    != delayedMotionEvent.xmotion.display)
			|| (event.xmotion.window
			    != delayedMotionEvent.xmotion.window)) {
		    XEvent copy;

		    /*
		     * Must copy the event out of delayedMotionEvent before
		     * processing it, in order to allow recursive calls to
		     * Tk_DoOneEvent as part of the handler.
		     */

		    copy = delayedMotionEvent;
		    eventDelayed = 0;
		    Tk_HandleEvent(&copy);
		}
	    }
	    if (tkCollapseMotion && event.type == MotionNotify) {
		delayedMotionEvent = event;
		eventDelayed = 1;
	    } else {
		Tk_HandleEvent(&event);
	    }
	    return 1;
	}

	/*
	 * Not a display:  if the file is ready, call the
	 * appropriate handler.
	 */

	if (((*filePtr->readPtr | *filePtr->writePtr
		| *filePtr->exceptPtr) & filePtr->mask) == 0) {
	    continue;
	}
	if (!(flags & TK_FILE_EVENTS)) {
	    continue;
	}
	mask = 0;
	if (*filePtr->readPtr & filePtr->mask) {
	    mask |= TK_READABLE;
	    *filePtr->readPtr &= ~filePtr->mask;
	}
	if (*filePtr->writePtr & filePtr->mask) {
	    mask |= TK_WRITABLE;
	    *filePtr->writePtr &= ~filePtr->mask;
	}
	if (*filePtr->exceptPtr & filePtr->mask) {
	    mask |= TK_EXCEPTION;
	    *filePtr->exceptPtr &= ~filePtr->mask;
	}
	(*filePtr->proc)(filePtr->clientData, mask);
	return 1;
    }

    /*
     * Phase Two: get the current time and see if any timer
     * events are ready to fire.  If so, fire one and return.
     */

    checkTime:
    if ((timerQueue != NULL) && (flags & TK_TIMER_EVENTS)) {
	register TimerEvent *timerPtr = timerQueue;

	(void) gettimeofday(&curTime, (struct timezone *) NULL);
	if ((timerPtr->time.tv_sec < curTime.tv_sec)
		|| ((timerPtr->time.tv_sec == curTime.tv_sec)
		&&  (timerPtr->time.tv_usec < curTime.tv_usec))) {
	    timerQueue = timerPtr->nextPtr;
	    (*timerPtr->proc)(timerPtr->clientData);
	    FreeTimerEvent(timerPtr);
	    return 1;
	}
    }


    /*
     * Phase Three: if there is a delayed motion event, process it
     * now, before any DoWhenIdle handlers.  Better to process before
     * idle handlers than after, because the goal of idle handlers is
     * to delay until after all pending events have been processed.
     * Must free up delayedMotionEvent *before* calling Tk_HandleEvent,
     * so that the event handler can call Tk_DoOneEvent recursively
     * without infinite looping.
     */

    if ((eventDelayed) && (flags & TK_X_EVENTS)) {
	XEvent copy;

	copy = delayedMotionEvent;
	eventDelayed = 0;
	Tk_HandleEvent(&copy);
	return 1;
    }

    /*
     * Phase Four: if there are DoWhenIdle requests pending (or
     * if we're not allowed to block), then do a select with an
     * instantaneous timeout.  If a ready file is found, then go
     * back to process it.
     */

    if (((idleList != NULL) && (flags & TK_IDLE_EVENTS))
	    || (flags & TK_DONT_WAIT)) {
	if (flags & (TK_X_EVENTS|TK_FILE_EVENTS)) {
	    memcpy((VOID *) ready, (VOID *) masks, 3*MASK_SIZE*sizeof(int));
	    timeout.tv_sec = timeout.tv_usec = 0;
	    do {
		numFound = select(numFds, (SELECT_MASK *) readPtr,
			(SELECT_MASK *) writePtr, (SELECT_MASK *) exceptPtr,
		    &timeout);
	    } while ((numFound == -1) && (errno == EINTR));
	    if (numFound > 0) {
		goto checkFiles;
	    }
	}
    }

    /*
     * Phase Five:  process all pending DoWhenIdle requests.
     */

    if ((idleList != NULL) && (flags & TK_IDLE_EVENTS)) {
	register IdleHandler *idlePtr;

	/*
	 * If you change the code below, be aware that new handlers
	 * can get added to the list while the current one is being
	 * processed.
	 *
	 * NOTE!  Must remove the entry from the list before calling
	 * it, in case the idle handler calls Tk_DoOneEvent:  don't
	 * want to loop infinitely.  Must also be careful because
	 * Tk_CancelIdleCall could change the list during the call.
	 */

	while (idleList != NULL) {
	    idlePtr = idleList;
	    idleList = idlePtr->nextPtr;
	    if (idleList == NULL) {
		lastIdlePtr = NULL;
	    }
	    (*idlePtr->proc)(idlePtr->clientData);
	    FreeIdleHandler(idlePtr);
	}
	return 1;
    }

    /*
     * Phase Six: do a select to wait for either one of the
     * files to become ready or for the first timer event to
     * fire.  Then go back to process the event.
     */

    if ((flags & TK_DONT_WAIT)
	    || !(flags & (TK_TIMER_EVENTS|TK_FILE_EVENTS|TK_X_EVENTS))) {
	return 0;
    }
    if ((timerQueue == NULL) || !(flags & TK_TIMER_EVENTS)) {
	timeoutPtr = NULL;
    } else {
	timeoutPtr = &timeout;
	timeout.tv_sec = timerQueue->time.tv_sec - curTime.tv_sec;
	timeout.tv_usec = timerQueue->time.tv_usec - curTime.tv_usec;
	if (timeout.tv_usec < 0) {
	    timeout.tv_sec -= 1;
	    timeout.tv_usec += 1000000;
	}
    }
    memcpy((VOID *) ready, (VOID *) masks, 3*MASK_SIZE*sizeof(int));
    do {
	numFound = select(numFds, (SELECT_MASK *) readPtr,
		(SELECT_MASK *) writePtr, (SELECT_MASK *) exceptPtr,
		timeoutPtr);
    } while ((numFound == -1) && (errno == EINTR));
    if (numFound == 0) {
	goto checkTime;
    }
    goto checkFiles;
}

/*
 *--------------------------------------------------------------
 *
 * Tk_MainLoop --
 *
 *	Call Tk_DoOneEvent over and over again in an infinite
 *	loop as long as there exist any main windows.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Arbitrary;  depends on handlers for events.
 *
 *--------------------------------------------------------------
 */

void
Tk_MainLoop()
{
    while (!tkMustExit &&
	   tk_NumMainWindows > 0) {
	Tk_DoOneEvent(0);
    }
}

/*
 *----------------------------------------------------------------------
 *
 * Tk_Sleep --
 *
 *	Delay execution for the specified number of milliseconds.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Time passes.
 *
 *----------------------------------------------------------------------
 */

void
Tk_Sleep(ms)
    int ms;			/* Number of milliseconds to sleep. */
{
    static struct timeval delay;

    delay.tv_sec = ms/1000;
    delay.tv_usec = (ms%1000)*1000;
    (void) select(0, (SELECT_MASK *) 0, (SELECT_MASK *) 0,
	    (SELECT_MASK *) 0, &delay);
}

/*
 *----------------------------------------------------------------------
 *
 * Tk_RestrictEvents --
 *
 *	This procedure is used to globally restrict the set of events
 *	that will be dispatched.  The restriction is done by filtering
 *	all incoming X events through a procedure that determines
 *	whether they are to be processed immediately or deferred.
 *
 * Results:
 *	The return value is the previous restriction procedure in effect,
 *	if there was one, or NULL if there wasn't.
 *
 * Side effects:
 *	From now on, proc will be called to determine whether to process
 *	or defer each incoming X event.
 *
 *----------------------------------------------------------------------
 */

Tk_RestrictProc *
Tk_RestrictEvents(proc, arg, prevArgPtr)
    Tk_RestrictProc *proc;	/* X "if" procedure to call for each
				 * incoming event.  See "XIfEvent" doc.
				 * for details. */
    char *arg;			/* Arbitrary argument to pass to proc. */
    char **prevArgPtr;		/* Place to store information about previous
				 * argument. */
{
    Bool (*prev)  _ANSI_ARGS_((Display *display, XEvent *eventPtr, char *arg));

    prev = restrictProc;
    *prevArgPtr = restrictArg;
    restrictProc = proc;
    restrictArg = arg;
    return prev;
}

/*
 *--------------------------------------------------------------
 *
 * Tk_CreateFocusHandler --
 *
 *	Arrange for a procedure to be called whenever the focus
 *	enters or leaves a given window.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	After this procedure has been invoked, whenever tkwin gets
 *	or loses the input focus, proc will be called.  It should have
 *	the following structure:
 *
 *	void
 *	proc(clientData, gotFocus)
 *	    ClientData clientData;
 *	    int gotFocus;
 *	{
 *	}
 *
 *	The clientData argument to "proc" will be the same as the
 *	clientData argument to this procedure.  GotFocus will be
 *	1 if tkwin is getting the focus, and 0 if it's losing the
 *	focus.
 *
 *--------------------------------------------------------------
 */

void
Tk_CreateFocusHandler(tkwin, proc, clientData)
    Tk_Window tkwin;		/* Token for window. */
    Tk_FocusProc *proc;		/* Procedure to call when tkwin gets
				 * or loses the input focus. */
    ClientData clientData;	/* Arbitrary value to pass to proc. */
{
    register TkWindow *winPtr = (TkWindow *) tkwin;

    winPtr->focusProc = proc;
    winPtr->focusData = clientData;
}

/*
 *--------------------------------------------------------------
 *
 * Tk_FocusCmd --
 *
 *	This procedure is invoked to process the "focus" Tcl command.
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
Tk_FocusCmd(clientData, interp, argc, argv)
    ClientData clientData;	/* Main window associated with
				 * interpreter. */
    Tcl_Interp *interp;		/* Current interpreter. */
    int argc;			/* Number of arguments. */
    char **argv;		/* Argument strings. */
{
    Tk_Window tkwin = (Tk_Window) clientData;
    register TkWindow *winPtr = (TkWindow *) clientData;
    register TkWindow *newPtr;

    if (argc > 3) {
	focusSyntax:
	Tcl_AppendResult(interp, "too many args: should be \"",
		argv[0], " ?-query? ?window?\"", (char *) NULL);
	return TCL_ERROR;
    }

    if (argc == 1) {
	if (winPtr->dispPtr->focusPtr == NULL) {
	    interp->result = "none";
	} else {
	    interp->result = winPtr->dispPtr->focusPtr->pathName;
	}
	return TCL_OK;
    }

    if (argv[1][0] == '-') {
	int switchLength;

	switchLength = strlen(argv[1]);
	if ((switchLength >= 2)
		&& (strncmp(argv[1], "-query", switchLength) == 0)) {

	    if (argc != 3) {
		goto focusSyntax;
	    }

	    newPtr = (TkWindow *) Tk_NameToWindow(interp, argv[2], tkwin);
	    if (newPtr == NULL) {
		return TCL_ERROR;
	    }
	    if (newPtr->dispPtr->focusPtr == NULL) {
		interp->result = "none";
	    } else {
		interp->result = newPtr->dispPtr->focusPtr->pathName;
	    }
	    return TCL_OK;
	}
    }

    if (argc != 2) {
	goto focusSyntax;
    }

    if (strcmp(argv[1], "none") == 0) {
	newPtr = NULL;
    } else {
	newPtr = (TkWindow *) Tk_NameToWindow(interp, argv[1], tkwin);
	if (newPtr == NULL) {
	    return TCL_ERROR;
	}
    }
    /* XXX: mumble frotz */
/*    if (newPtr->dispPtr->focusPtr == newPtr) { */
    if ((!newPtr) || (newPtr->dispPtr->focusPtr == newPtr)) {
	return TCL_OK;
    }
    if (winPtr == newPtr->dispPtr->mouseMainPtr) { /* XXX: ??? presumably */
	if ((newPtr->dispPtr->focusPtr != NULL)
		&& (newPtr->dispPtr->focusPtr->focusProc != NULL)) {
	    (*newPtr->dispPtr->focusPtr->focusProc)(
		    newPtr->dispPtr->focusPtr->focusData, 0);
	}
	newPtr->dispPtr->focusPtr = newPtr;
	if ((newPtr != NULL) && (newPtr->focusProc != NULL)) {
	    (*newPtr->focusProc)(newPtr->focusData, 1);
	}
    } else {
	newPtr->dispPtr->focusPtr = newPtr;
    }
    return TCL_OK;
}

/*
 *--------------------------------------------------------------
 *
 * TkFocusEventProc --
 *
 *	This procedure is invoked whenever the pointer enters
 *	or leaves a top-level window.  It notifies the current
 *	owner of the focus, if any.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	None.
 *
 *--------------------------------------------------------------
 */

void
TkFocusEventProc(winPtr, eventPtr)
    register TkWindow *winPtr;	/* Top-level window just entered or left. */
    XEvent *eventPtr;		/* EnterWindow or LeaveWindow event. */
{
    register TkWindow *focusPtr;
    TkWindow *newMouseMainPtr = NULL;

    if (eventPtr->type == EnterNotify) {
	newMouseMainPtr = winPtr->mainPtr->winPtr;
    }
    if (winPtr->dispPtr->mouseMainPtr == newMouseMainPtr) {
	return;
    }
    if (winPtr->dispPtr->mouseMainPtr != NULL) {
	focusPtr = winPtr->dispPtr->focusPtr;
	if ((focusPtr != NULL)
		&& (focusPtr->focusProc != NULL)) {
	    (*focusPtr->focusProc)(focusPtr->focusData, 0);
	}
    }
    winPtr->dispPtr->mouseMainPtr = newMouseMainPtr;
    if (newMouseMainPtr != NULL) {
	focusPtr = newMouseMainPtr->dispPtr->focusPtr;
	if ((focusPtr != NULL)
		&& (focusPtr->focusProc != NULL)) {
	    (*focusPtr->focusProc)(focusPtr->focusData, 1);
	}
    }
}

/*
 *--------------------------------------------------------------
 *
 * TkEventDeadWindow --
 *
 *	This procedure is invoked when it is determined that
 *	a window is dead.  It cleans up event-related information
 *	about the window.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Various things get cleaned up and recycled.
 *
 *--------------------------------------------------------------
 */

void
TkEventDeadWindow(winPtr)
    TkWindow *winPtr;		/* Information about the window
				 * that is being deleted. */
{
    register TkEventHandler *handlerPtr;
    register InProgress *ipPtr;

    /*
     * While deleting all the handlers, be careful to check for
     * Tk_HandleEvent being about to process one of the deleted
     * handlers.  If it is, tell it to quit (all of the handlers
     * are being deleted).
     */

    while (winPtr->handlerList != NULL) {
	handlerPtr = winPtr->handlerList;
	winPtr->handlerList = handlerPtr->nextPtr;
	for (ipPtr = pendingPtr; ipPtr != NULL; ipPtr = ipPtr->nextPtr) {
	    if (ipPtr->nextHandler == handlerPtr) {
		ipPtr->nextHandler = NULL;
	    }
	    if (ipPtr->winPtr == winPtr) {
		ipPtr->winPtr = None;
	    }
	}
	ckfree((char *) handlerPtr);
    }
    if ((winPtr->dispPtr != NULL) && (winPtr->dispPtr->focusPtr == winPtr)) {
	winPtr->dispPtr->focusPtr = NULL;
    }
}

/*
 *----------------------------------------------------------------------
 *
 * TkCurrentTime --
 *
 *	Try to deduce the current time.  "Current time" means the time
 *	of the event that led to the current code being executed, which
 *	means the time in the most recently-nested invocation of
 *	Tk_HandleEvent.
 *
 * Results:
 *	The return value is the time from the current event, or
 *	CurrentTime if there is no current event or if the current
 *	event contains no time.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

Time
TkCurrentTime(dispPtr)
    TkDisplay *dispPtr;		/* Display for which the time is desired. */
{
    register XEvent *eventPtr;

    if (pendingPtr == NULL) {
	return dispPtr->lastEventTime;
    }
    eventPtr = pendingPtr->eventPtr;
    switch (eventPtr->type) {
	case ButtonPress:
	case ButtonRelease:
	    return eventPtr->xbutton.time;
	case KeyPress:
	case KeyRelease:
	    return eventPtr->xkey.time;
	case MotionNotify:
	    return eventPtr->xmotion.time;
	case EnterNotify:
	case LeaveNotify:
	    return eventPtr->xcrossing.time;
	case PropertyNotify:
	    return eventPtr->xproperty.time;
    }
    return dispPtr->lastEventTime;
}
