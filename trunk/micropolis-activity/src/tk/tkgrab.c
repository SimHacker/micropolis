/* 
 * tkGrab.c --
 *
 *	This file provides procedures that implement grabs for Tk.
 *
 * Copyright 1992 Regents of the University of California.
 * Permission to use, copy, modify, and distribute this
 * software and its documentation for any purpose and without
 * fee is hereby granted, provided that the above copyright
 * notice appear in all copies.  The University of California
 * makes no representations about the suitability of this
 * software for any purpose.  It is provided "as is" without
 * express or implied warranty.
 */

#ifndef lint
static char rcsid[] = "$Header: /user6/ouster/wish/RCS/tkGrab.c,v 1.18 92/08/07 09:55:31 ouster Exp $ SPRITE (Berkeley)";
#endif

#include "tkconfig.h"
#include "tkint.h"

/*
 *-------------------------------------------------------------------
 * Problems with current grab implementation (8/7/92):
 *
 * 1. In a local grab the synthesized events are always placed at the
 *    front of the event queue.  If there are several grabs and ungrabs
 *    in a row, the groups of events for the different grabs/ungrabs
 *    end up in backwards order.
 * 2. The variables serverWinPtr and pointerWinPtr are hardly used at
 *    all and should probably be eliminated.
 * 3. The fact that grabWinPtr is set at the time a grab is set or
 *    released, rather than when its events are processed, means that
 *    it can get out of sync with the event queue if there's a rapid
 *    sequence of grabs or ungrabs.  The only solution I can think of
 *    is to keep a parallel queue to the event queue to update grabWinPtr
 *    (or, synthesize an event to change the pointer?).
 *-------------------------------------------------------------------
 */

/*
 * Bit definitions for grabFlags field of TkDisplay structures:
 *
 * GRAB_GLOBAL			1 means this is a global grab (we grabbed via
 *				the server so all applications are locked out.
 *				0 means this is a local grab that affects
 *				only this application.
 * GRAB_BUTTON_RELEASE		1 means that a button-release event just
 *				occurred and we're in the middle of a sequence
 *				of Enter and Leave events with NotifyUngrab
 *				mode.
 */

#define GRAB_GLOBAL		1
#define GRAB_BUTTON_RELEASE	2

/*
 * Forward declarations for procedures declared later in this file:
 */

static void		ChangeEventWindow _ANSI_ARGS_((XEvent *eventPtr,
			    TkWindow *winPtr));
static void		MovePointer _ANSI_ARGS_((XEvent *eventPtr,
			    TkWindow *sourcePtr, TkWindow *destPtr));
static void		MovePointer2 _ANSI_ARGS_((TkWindow *sourcePtr,
			    TkWindow *destPtr, int mode));

/*
 *----------------------------------------------------------------------
 *
 * Tk_GrabCmd --
 *
 *	This procedure is invoked to process the "grab" Tcl command.
 *	See the user documentation for details on what it does.
 *
 * Results:
 *	A standard Tcl result.
 *
 * Side effects:
 *	See the user documentation.
 *
 *----------------------------------------------------------------------
 */

	/* ARGSUSED */
int
Tk_GrabCmd(clientData, interp, argc, argv)
    ClientData clientData;	/* Main window associated with
				 * interpreter. */
    Tcl_Interp *interp;		/* Current interpreter. */
    int argc;			/* Number of arguments. */
    char **argv;		/* Argument strings. */
{
    TkWindow *winPtr = (TkWindow *) clientData;
    int length, lockScreen;
    char *window;

    if (argc > 3) {
	badArgs:
	Tcl_AppendResult(interp, "wrong # args: should be \"",
		argv[0], " ?-global? ?window?\"", (char *) NULL);
	return TCL_ERROR;
    }
    if (argc == 1) {
	if ((winPtr->dispPtr->grabWinPtr != NULL)
		&& (winPtr->dispPtr->grabWinPtr->mainPtr
		== winPtr->mainPtr)) {
	    interp->result = Tk_PathName(winPtr->dispPtr->grabWinPtr);
	} else {
	    interp->result = "none";
	}
	return TCL_OK;
    }
    if (argc == 3) {
	length = strlen(argv[1]);
	if (strncmp(argv[1], "-off", length) == 0) {
	    lockScreen = -1;
	} else {
	    if ((strncmp(argv[1], "-global", length) != 0) || (length < 2)) {
		goto badArgs;
	    }
	    lockScreen = 1;
	}
	window = argv[2];
    } else {
	lockScreen = 0;
	window = argv[1];
    }
    if ((window[0] == '\0')
	    || (strncmp(window, "none", strlen(window)) == 0)) {
	Tk_Ungrab((Tk_Window) winPtr);	
    } else {
	Tk_Window tkwin;

	tkwin = Tk_NameToWindow(interp, window, (Tk_Window) winPtr);
	if (tkwin == NULL) {
	    return TCL_ERROR;
	}
	if (lockScreen < 0) {
	    Tk_Ungrab(tkwin);
	} else {
	    return Tk_Grab(interp, tkwin, lockScreen);
	}
    }
    return TCL_OK;
}

/*
 *----------------------------------------------------------------------
 *
 * Tk_Grab --
 *
 *	Grabs the pointer and keyboard, so that mouse-related events are
 *	only reported relative to a given window and its descendants.
 *
 * Results:
 *	A standard Tcl result is returned.  TCL_OK is the normal return
 *	value;  if the grab could not be set then TCL_ERROR is returned
 *	and interp->result will hold an error message.
 *
 * Side effects:
 *	Once this call completes successfully, no window outside the
 *	tree rooted at tkwin will receive pointer- or keyboard-related
 *	events until the next call to Tk_Ungrab.  If a previous grab was
 *	in effect within this application, then it is replaced with a new
 *	one.
 *
 *----------------------------------------------------------------------
 */

int
Tk_Grab(interp, tkwin, grabGlobal)
    Tcl_Interp *interp;			/* Used for error reporting. */
    Tk_Window tkwin;			/* Window on whose behalf the pointer
					 * is to be grabbed. */
    int grabGlobal;			/* Non-zero means issue a grab to the
					 * server so that no other application
					 * gets mouse or keyboard events.
					 * Zero means the grab only applies
					 * within this application. */
{
    int grabResult;
    TkWindow *winPtr = (TkWindow *) tkwin;
    TkDisplay *dispPtr = winPtr->dispPtr;
    int grabRequest, inSequence, ignoring, numEvents, i, diff;
    XEvent *events, *eventPtr;
    TkWindow *winPtr2;

    if (dispPtr->grabWinPtr != NULL) {
	if ((dispPtr->grabWinPtr == winPtr)
		&& (grabGlobal == ((dispPtr->grabFlags & GRAB_GLOBAL) != 0))) {
	    return TCL_OK;
	}
	if (dispPtr->grabWinPtr->mainPtr != winPtr->mainPtr) {
	    alreadyGrabbed:
	    interp->result = "grab failed: another application has grab";
	    return TCL_ERROR;
	}
	Tk_Ungrab(tkwin);
    }

    if (grabGlobal) {
	grabRequest = NextRequest(dispPtr->display);
	grabResult = XGrabPointer(dispPtr->display, Tk_WindowId(tkwin),
		True, ButtonPressMask|ButtonReleaseMask|ButtonMotionMask|PointerMotionMask,
		GrabModeAsync, GrabModeAsync, None, None,
		TkCurrentTime(dispPtr));
	if (grabResult != 0) {
	    grabError:
	    if (grabResult == GrabNotViewable) {
		interp->result = "grab failed: window not viewable";
	    } else if (grabResult == AlreadyGrabbed) {
		goto alreadyGrabbed;
	    } else if (grabResult == GrabFrozen) {
		interp->result = "grab failed: keyboard or pointer frozen";
	    } else if (grabResult == GrabInvalidTime) {
		interp->result = "grab failed: invalid time";
	    } else {
		char msg[100];
	
		sprintf(msg, "grab failed for unknown reason (code %d)",
			grabResult);
		Tcl_AppendResult(interp, msg, (char *) NULL);
	    }
	    return TCL_ERROR;
	}
	grabResult = XGrabKeyboard(dispPtr->display, Tk_WindowId(tkwin),
		False, GrabModeAsync, GrabModeAsync, TkCurrentTime(dispPtr));
	if (grabResult != 0) {
	    XUngrabPointer(dispPtr->display, TkCurrentTime(dispPtr));
	    goto grabError;
	}
	dispPtr->grabFlags |= GRAB_GLOBAL;
    } else {
	/*
	 * The call to XUngrabPointer below is needed to release any
	 * existing auto-grab due to a button press.  This is needed
	 * so that local grabs behave the same as global grabs (the
	 * button grab is released by the X server in a global grab).
	 */

	XUngrabPointer(dispPtr->display, TkCurrentTime(dispPtr));
	grabRequest = LastKnownRequestProcessed(dispPtr->display);
	dispPtr->grabFlags &= ~GRAB_GLOBAL;

	/*
	 * Since we're not telling the server about the grab, we have
	 * to generate Leave and Enter events to move the pointer from
	 * its current window to the grab window.
	 */

	MovePointer2(dispPtr->pointerWinPtr, winPtr, NotifyGrab);
    }
    dispPtr->grabWinPtr = winPtr;

    /*
     * When a grab occurs, X generates Enter and Leave events to move
     * the pointer from its current window to the grab window, even if
     * the current window is in the grab tree.  We don't want these
     * events getting through to the application if the current window
     * is in the grab tree.  In order to eliminate the bogus events,
     * process all pending events and filter out the bogus ones.
     *
     * Also, filter out the final enter event into the grab window in
     * any case:  this event shouldn't be delivered until the mouse really
     * moves into that window.
     *
     * The code below reads in all the pending events, filters out the bad
     * ones, and then pushes back all the events that weren't filtered.
     * Another alternative would be to simply process the events
     * immediately rather than pushing them back again.  However, this
     * tends to interfere with scripts since it causes pending events
     * to be processed during the "grab" command.  The "grab" command
     * might have been invoked in the middle of some computation where
     * it's a bad idea to process new events.
     */

    XSync(dispPtr->display, False);
    numEvents = QLength(dispPtr->display);
    if (numEvents == 0) {
	return TCL_OK;
    }
    events = (XEvent *) ckalloc((unsigned) (numEvents * sizeof(XEvent)));
    for (i = 0; i < numEvents; i++) {
	XNextEvent(dispPtr->display, &events[i]);
    }
    inSequence = ignoring = 0;
    for (i = numEvents-1, eventPtr = events; i >= 0; i--, eventPtr++) {
	if (((eventPtr->type != EnterNotify)
		&& (eventPtr->type != LeaveNotify))
		|| (eventPtr->xcrossing.mode != NotifyGrab)) {
	    continue;
	}

	/*
	 * The diff caculcation below is trickier than you might think,
	 * due to the fact that the event serial number is unsigned and
	 * serial numbers can wrap around.
	 */

	diff = eventPtr->xcrossing.serial;
	diff -= grabRequest;
	if (!inSequence && (diff >= 0)) {
	    /*
	     * This is the first event of the grab sequence.  See if its
	     * window is in the grab tree and ignore the sequence if it is.
	     */

	    inSequence = 1;
	    if (XFindContext(dispPtr->display, eventPtr->xcrossing.window,
		    tkWindowContext, (void *) &winPtr2) == 0) {
		for ( ; winPtr2 != NULL; winPtr2 = winPtr2->parentPtr) {
		    if (winPtr2 == dispPtr->grabWinPtr) {
			ignoring = 1;
			break;
		    }
		}
	    }
	}
	if (ignoring) {
	    eventPtr->type = 0;
	}
	if (inSequence && (eventPtr->type == EnterNotify)
		&& (dispPtr->grabWinPtr->window
		== eventPtr->xcrossing.window)) {
	    eventPtr->type = 0;
	    break;
	}
    }
    for (i = numEvents-1, eventPtr = &events[i]; i >= 0; i--, eventPtr--) {
	if (eventPtr->type != 0) {
	    XPutBackEvent(dispPtr->display, eventPtr);
	}
    }
    ckfree((char *) events);
    return TCL_OK;
}

/*
 *----------------------------------------------------------------------
 *
 * Tk_Ungrab --
 *
 *	Releases a grab on the mouse pointer and keyboard.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Pointer and keyboard events will start being delivered to other
 *	windows again.
 *
 *----------------------------------------------------------------------
 */

void
Tk_Ungrab(tkwin)
    Tk_Window tkwin;			/* Window that identifies display
					 * for grab to be released. */
{
    TkDisplay *dispPtr = ((TkWindow *) tkwin)->dispPtr;
    int inSequence, ignoring, ungrabRequest, numEvents, i, j, diff;
    TkWindow *grabWinPtr, *winPtr;
    XEvent *events, *eventPtr, *eventPtr2;

    grabWinPtr = dispPtr->grabWinPtr;
    if (grabWinPtr == NULL) {
	return;
    }
    dispPtr->grabWinPtr = NULL;
    dispPtr->buttonWinPtr = NULL;
    if (dispPtr->grabFlags & GRAB_GLOBAL) {
	ungrabRequest = NextRequest(dispPtr->display);
	XUngrabPointer(dispPtr->display, TkCurrentTime(dispPtr));
	XUngrabKeyboard(dispPtr->display, TkCurrentTime(dispPtr));
	XSync(dispPtr->display, False);
    } else {
	ungrabRequest = LastKnownRequestProcessed(dispPtr->display);
	if ((dispPtr->ungrabWinPtr != NULL)
		&& (dispPtr->ungrabWinPtr->mainPtr != grabWinPtr->mainPtr)) {

	    /*
	     * Don't report entries down into a window of a different
	     * application, since it's already seen those entries earlier.
	     */

	    dispPtr->ungrabWinPtr = NULL;
	}
	MovePointer2(grabWinPtr, dispPtr->ungrabWinPtr, NotifyUngrab);
    }

    /*
     * We have to filter all the pending events in a fashion similar to
     * Tk_Grab. As with grabs, the X server generates an Enter-Leave event
     * sequence to move the pointer from the grab window back to its
     * current window.  We need to ignore this sequence if the pointer
     * is being moved to a window that's already in the grab tree.
     */

    numEvents = QLength(dispPtr->display);
    if (numEvents == 0) {
	return;
    }
    events = (XEvent *) ckalloc((unsigned) (numEvents * sizeof(XEvent)));
    for (i = 0; i < numEvents; i++) {
	XNextEvent(dispPtr->display, &events[i]);
    }
    inSequence = ignoring = 0;
    for (i = numEvents-1, eventPtr = events; i >= 0; i--, eventPtr++) {
	if (((eventPtr->type != EnterNotify)
		&& (eventPtr->type != LeaveNotify))
		|| (eventPtr->xcrossing.mode != NotifyUngrab)) {
	    continue;
	}
	diff = eventPtr->xcrossing.serial;
	diff -= ungrabRequest;
	if (!inSequence && (diff >= 0)) {

	    /*
	     * This is the first event of the ungrab sequence.  Scan forward
	     * looking for the final Enter event in the sequence.  Then see
	     * if that event's window is in the grab tree.
	     */

	    inSequence = 1;
	    for (j = i, eventPtr2 = eventPtr; j >= 0; j--, eventPtr2++) {
		if (eventPtr2->type == EnterNotify) {
		    if (eventPtr2->xcrossing.mode != NotifyUngrab) {
			break;
		    }
		    if ((eventPtr2->xcrossing.detail != NotifyAncestor)
			    && (eventPtr2->xcrossing.detail != NotifyInferior)
			    && (eventPtr2->xcrossing.detail
				    != NotifyNonlinear)) {
			continue;
		    }
		    if (XFindContext(dispPtr->display,
			    eventPtr2->xcrossing.window,
			    tkWindowContext, (void *) &winPtr) == 0) {
			for ( ; winPtr != NULL; winPtr = winPtr->parentPtr) {
			    if (winPtr == grabWinPtr) {
				ignoring = 1;
				break;
			    }
			}
		    }
		    break;
		} else if ((eventPtr2->type != LeaveNotify)
			|| (eventPtr2->xcrossing.mode != NotifyUngrab)) {
		    break;
		}
	    }
	}
	if (ignoring) {
	    eventPtr->type = 0;
	}
    }
    for (i = numEvents-1, eventPtr = &events[i]; i >= 0; i--, eventPtr--) {
	if (eventPtr->type != 0) {
	    XPutBackEvent(dispPtr->display, eventPtr);
	}
    }
    ckfree((char *) events);
}

/*
 *----------------------------------------------------------------------
 *
 * TkPointerEvent --
 *
 *	This procedure is called for each pointer-related event, before
 *	the event has been processed.  It does various things to make
 *	grabs work correctly.
 *
 * Results:
 *	If the return value is 1 it means the event should be processed
 *	(event handlers should be invoked).  If the return value is 0
 *	it means the event should be ignored in order to make grabs
 *	work correctly.  Note:  the event may be modified by this procedure.
 *
 * Side effects:
 *	Grab state information may be updated.
 *
 *----------------------------------------------------------------------
 */

int
TkPointerEvent(eventPtr, winPtr)
    register XEvent *eventPtr;		/* Pointer to the event. */
    TkWindow *winPtr;			/* Tk's information for window
					 * where event was reported. */
{
    register TkWindow *winPtr2;
    TkDisplay *dispPtr = winPtr->dispPtr;
    int outsideGrabTree = 0;
    int originalFlags;
    int appGrabbed = 0;			/* Non-zero means event is being
					 * reported to an application that is
					 * affected by the grab. */
#define ALL_BUTTONS \
	(Button1Mask|Button2Mask|Button3Mask|Button4Mask|Button5Mask)
    static unsigned int state[] = {
	Button1Mask, Button2Mask, Button3Mask, Button4Mask, Button5Mask
    };

    /*
     * Don't do any filtering on events generated by the event-sharing code.
     */

    if (eventPtr == tkShareEventPtr) {
	return 1;
    }

    /*
     * If a grab is in effect, see if the event is being reported to
     * a window in the grab tree.  Also see if the event is being reported
     * to an application that is affected by the grab.
     */

    if (dispPtr->grabWinPtr != NULL) {
	if ((winPtr->mainPtr == dispPtr->grabWinPtr->mainPtr)
		|| (dispPtr->grabFlags & GRAB_GLOBAL)) {
	    appGrabbed = 1;
	}
	for (winPtr2 = winPtr; winPtr2 != dispPtr->grabWinPtr;
		winPtr2 = winPtr2->parentPtr) {
	    if (winPtr2 == NULL) {
		outsideGrabTree = 1;
		break;
	    }
	}
    }

    originalFlags = dispPtr->grabFlags;
    dispPtr->grabFlags &= ~GRAB_BUTTON_RELEASE;
    if ((eventPtr->type == EnterNotify) || (eventPtr->type == LeaveNotify)) {
	if ((eventPtr->type == EnterNotify)
		&& (eventPtr->xcrossing.detail != NotifyVirtual)
		&& (eventPtr->xcrossing.detail != NotifyNonlinearVirtual)) {
	    if ((dispPtr->grabWinPtr == NULL)
		    || (dispPtr->grabWinPtr->mainPtr == winPtr->mainPtr)) {
		dispPtr->ungrabWinPtr = winPtr;
	    }
	    dispPtr->serverWinPtr = winPtr;
	} else {
	    dispPtr->serverWinPtr = NULL;
	}
	if (dispPtr->grabWinPtr != NULL) {
	    if (eventPtr->xcrossing.mode == NotifyNormal) {
		/*
		 * When a grab is active, X continues to report enter and
		 * leave events for windows outside the tree of the grab
		 * window.  Detect these events and ignore them.
		 */

		if (outsideGrabTree && appGrabbed) {
		    return 0;
		}
    
		/*
		 * Make buttons have the same grab-like behavior inside a grab
		 * as they do outside a grab:  do this by ignoring enter and
		 * leave events except for the window in which the button was
		 * pressed.
		 */

		if ((dispPtr->buttonWinPtr != NULL)
			&& (winPtr != dispPtr->buttonWinPtr)) {
		    return 0;
		}
	    } else if (eventPtr->xcrossing.mode == NotifyUngrab) {
		/*
		 * Keep the GRAB_BUTTON_RELEASE flag on if it used to be on.
		 */

		dispPtr->grabFlags = originalFlags;
		if (outsideGrabTree && appGrabbed
			&& (dispPtr->grabFlags & GRAB_BUTTON_RELEASE)) {
		    /*
		     * The only way we get here is if a button was pressed,
		     * then moved to a different window and released.  Enter
		     * and leave events were deferred while the button was
		     * down, but now we're getting them to move the pointer
		     * back to the right window, and this particular event
		     * is for a window outside the grab tree.  Ignore it.
		     */
    
		    return 0;
		}
	    }
	}

	/*
	 * Keep track of the window containing the mouse, in order to
	 * detect various bogus event sequences.
	 */

	dispPtr->pointerWinPtr = dispPtr->serverWinPtr;
	return 1;
    }
    if ((dispPtr->grabWinPtr == NULL) || !appGrabbed) {
	return 1;
    }

    if (eventPtr->type == MotionNotify) {
	/*
	 * When grabs are active, X reports motion events relative to the
	 * window under the pointer.  Instead, it should report the events
	 * relative to the window the button went down in, if there is a
	 * button down.  Otherwise, if the pointer window is outside the
	 * subtree of the grab window, the events should be reported
	 * relative to the grab window.  Otherwise, the event should be
	 * reported to the pointer window.
	 */

	winPtr2 = winPtr;
	if (dispPtr->buttonWinPtr != NULL) {
	    winPtr2 = dispPtr->buttonWinPtr;
	} else if (outsideGrabTree || (dispPtr->serverWinPtr == NULL)) {
	    winPtr2 = dispPtr->grabWinPtr;
	}
	if (winPtr2 != winPtr) {
	    XEvent newEvent;

	    newEvent = *eventPtr;
	    ChangeEventWindow(&newEvent, winPtr2);
	    XPutBackEvent(winPtr2->display, &newEvent);
	    return 0;
	}
	return 1;
    }

    /*
     * Process ButtonPress and ButtonRelease events:
     * 1. Keep track of whether a button is down and what window it
     *    went down in.
     * 2. If the first button goes down outside the grab tree, pretend
     *    it went down in the grab window.  Note: it's important to
     *    redirect events to the grab window like this in order to make
     *    things like menus work, where button presses outside the
     *    grabbed menu need to be seen.  An application can always
     *    ignore the events if they occur outside its window.
     * 3. If a button press or release occurs outside the window where
     *    the first button was pressed, retarget the event so it's reported
     *    to the window where the first button was pressed.
     * 4. If the last button is released in a window different than where
     *    the first button was pressed, generate Enter/Leave events to
     *    move the mouse from the button window to its current window.
     * 5. If the grab is set at a time when a button is already down, or
     *    if the window where the button was pressed was deleted, then
     *    dispPtr->buttonWinPtr will stay NULL.  Just forget about the
     *    auto-grab for the button press;  events will go to whatever
     *    window contains the pointer.  If this window isn't in the grab
     *    tree then redirect events to the grab window.
     */

    if ((eventPtr->type == ButtonPress) || (eventPtr->type == ButtonRelease)) {
	winPtr2 = dispPtr->buttonWinPtr;
	if (winPtr2 == NULL) {
	    if (outsideGrabTree) {
		winPtr2 = dispPtr->grabWinPtr;			/* Note 5. */
	    } else {
		winPtr2 = winPtr;				/* Note 5. */
	    }
	}
	if (eventPtr->type == ButtonPress) {
	    if ((eventPtr->xbutton.state & ALL_BUTTONS) == 0) {
		if (outsideGrabTree) {
		    XEvent newEvent;

		    newEvent = *eventPtr;
		    ChangeEventWindow(&newEvent, dispPtr->grabWinPtr);
		    XPutBackEvent(dispPtr->display, &newEvent);
		    return 0;					/* Note 2. */
		}
		dispPtr->buttonWinPtr = winPtr;
		return 1;
	    }
	} else {
	    if ((eventPtr->xbutton.state & ALL_BUTTONS)
		    == state[eventPtr->xbutton.button - Button1]) {
		if ((dispPtr->buttonWinPtr != winPtr)
			&& (dispPtr->buttonWinPtr != NULL)) {
		    XEvent newEvent;				/* Note 4. */

		    /*
		     * If the button release is made with pointer outside
		     * all applications, X reports it relative to the grab
		     * window.   Change the current window to NULL to
		     * reflect that the pointer's outside everything.  Do
		     * the same if the pointer's in a window that's not
		     * part of the grab tree.
		     */

		    if (outsideGrabTree || (dispPtr->serverWinPtr == NULL)) {
			winPtr = NULL;
		    }
		    newEvent = *eventPtr;
		    newEvent.xcrossing.mode = NotifyUngrab;
		    newEvent.xcrossing.focus = False;
		    newEvent.xcrossing.state =
			    eventPtr->xbutton.state & ~ALL_BUTTONS;
		    MovePointer(&newEvent, dispPtr->buttonWinPtr, winPtr);
		}
		dispPtr->buttonWinPtr = NULL;
		dispPtr->grabFlags |= GRAB_BUTTON_RELEASE;
	    }
	}
	if (winPtr2 != winPtr) {
	    XEvent newEvent;

	    newEvent = *eventPtr;
	    ChangeEventWindow(&newEvent, winPtr2);
	    XPutBackEvent(dispPtr->display, &newEvent);
	    return 0;						/* Note 3. */
	}
    }

    return 1;
}

/*
 *----------------------------------------------------------------------
 *
 * ChangeEventWindow --
 *
 *	Given an event and a new window to which the event should be
 *	retargeted, modify fields of the event so that the event is
 *	properly retargeted to the new window.
 *
 * Results:
 *	The following fields of eventPtr are modified:  window,
 *	subwindow, x, y, same_screen.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

static void
ChangeEventWindow(eventPtr, winPtr)
    register XEvent *eventPtr;	/* Event to retarget.  Must have
				 * type ButtonPress, ButtonRelease, KeyPress,
				 * KeyRelease, MotionNotify, EnterNotify,
				 * or LeaveNotify. */
    TkWindow *winPtr;		/* New target window for event. */
{
    int x, y, sameScreen, bd;
    register TkWindow *childPtr;

    eventPtr->xmotion.window = Tk_WindowId(winPtr);
    if (eventPtr->xmotion.root ==
	    RootWindow(winPtr->display, winPtr->screenNum)) {
	Tk_GetRootCoords((Tk_Window) winPtr, &x, &y);
	eventPtr->xmotion.x = eventPtr->xmotion.x_root - x;
	eventPtr->xmotion.y = eventPtr->xmotion.y_root - y;
	eventPtr->xmotion.subwindow = None;
	for (childPtr = winPtr->childList; childPtr != NULL;
		childPtr = childPtr->nextPtr) {
	    if (childPtr->flags & TK_TOP_LEVEL) {
		continue;
	    }
	    x = eventPtr->xmotion.x - childPtr->changes.x;
	    y = eventPtr->xmotion.y - childPtr->changes.y;
	    bd = childPtr->changes.border_width;
	    if ((x >= -bd) && (y >= -bd)
		    && (x < (childPtr->changes.width + bd))
		    && (y < (childPtr->changes.width + bd))) {
		eventPtr->xmotion.subwindow = childPtr->window;
	    }
	}
	sameScreen = 1;
    } else {
	eventPtr->xmotion.x = 0;
	eventPtr->xmotion.y = 0;
	eventPtr->xmotion.subwindow = None;
	sameScreen = 0;
    }
    if (eventPtr->type == MotionNotify) {
	eventPtr->xmotion.same_screen = sameScreen;
    } else {
	eventPtr->xbutton.same_screen = sameScreen;
    }
}

/*
 *----------------------------------------------------------------------
 *
 * MovePointer --
 *
 *	This procedure synthesizes EnterNotify and LeaveNotify events
 *	to correctly transfer the pointer from one window to another.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Synthesized events may be pushed back onto the event queue.
 *	The event pointed to by eventPtr is modified.
 *
 *----------------------------------------------------------------------
 */

static void
MovePointer(eventPtr, sourcePtr, destPtr)
    XEvent *eventPtr;		/* A template X event.  Must have all fields
				 * properly set for EnterNotify and LeaveNotify
				 * events except window, subwindow, x, y,
				 * detail, and same_screen.  (x_root and y_root
				 * must be valid, even though x and y needn't
				 * be valid). */
    TkWindow *sourcePtr;	/* Window currently containing pointer (NULL
				 * means it's not one managed by this
				 * process). */
    TkWindow *destPtr;		/* Window that is to end up containing the
				 * pointer (NULL means it's not one managed
				 * by this process). */
{
    TkDisplay *dispPtr;
    register TkWindow *ancestorPtr;	/* Lowest ancestor shared between
					 * sourcePtr and destPtr, or
					 * sourcePtr's top-level window if no
					 * shared ancestor. */
    register TkWindow *winPtr;
    int upLevels, downLevels, i, j;

    /*
     * There are four possible cases to deal with:
     *
     * 1. SourcePtr and destPtr are the same.  There's nothing to do in
     *    this case.
     * 2. SourcePtr is an ancestor of destPtr in the same top-level
     *    window.  Must generate events down the window tree from source
     *    to dest.
     * 3. DestPtr is an ancestor of sourcePtr in the same top-level
     *    window.  Must generate events up the window tree from sourcePtr
     *    to destPtr.
     * 4. All other cases.  Must first generate events up the window tree
     *    from sourcePtr to its top-level, then down from destPtr's
     *    top-level to destPtr. This form is called "non-linear."
     *
     * The code below separates these four cases and decides how many levels
     * up and down events have to be generated for.
     */

    if (sourcePtr == destPtr) {
	return;
    }

    /*
     * Mark destPtr and all of its ancestors with a special flag bit.
     */

    if (destPtr != NULL) {
	dispPtr = destPtr->dispPtr;
	for (winPtr = destPtr; ; winPtr = winPtr->parentPtr) {
	    winPtr->flags |= TK_GRAB_FLAG;
	    if (winPtr->flags & TK_TOP_LEVEL) {
		break;
	    }
	}
    } else {
	dispPtr = sourcePtr->dispPtr;
    }

    /*
     * Search upwards from sourcePtr until an ancestor of destPtr is
     * found or a top-level window is reached.  Remember if we pass out
     * of the grab tree along the way, since this means we'll have to
     * skip some of the events that would otherwise be generated.
     */

    ancestorPtr = sourcePtr;
    upLevels = 0;
    if (sourcePtr != NULL) {
	for (; ; upLevels++, ancestorPtr = ancestorPtr->parentPtr) {
	    if (ancestorPtr->flags & TK_GRAB_FLAG) {
		break;
	    }
	    if (ancestorPtr->flags & TK_TOP_LEVEL)  {
		upLevels++;
		break;
	    }
	}
    }

    /*
     * Search upwards from destPtr again, clearing the flag bits and
     * remembering how many levels up we had to go.
     */

    if (destPtr == NULL) {
	downLevels = 0;
    } else {
	downLevels = -1;
	for (i = 0, winPtr = destPtr; ; i++, winPtr = winPtr->parentPtr) {
	    winPtr->flags &= ~TK_GRAB_FLAG;
	    if (winPtr == ancestorPtr) {
		downLevels = i;
	    }
	    if (winPtr->flags & TK_TOP_LEVEL) {
		if (downLevels == -1) {
		    downLevels = i+1;
		}
		break;
	    }
	}
    }

    /*
     * Generate enter/leave events and push them back onto the event
     * queue.  This has to be done backwards, since the last event
     * pushed will be the first one processed.
     */

#define PUSH_EVENT(w, t, d)			\
    if (w->window != None) {			\
	eventPtr->type = t;			\
	eventPtr->xcrossing.detail = d;		\
	ChangeEventWindow(eventPtr, w);		\
	XPutBackEvent(w->display, eventPtr);	\
    }

    if (downLevels == 0) {
    
	/*
	 * SourcePtr is an inferior of destPtr.
	 */

	if (destPtr != NULL) {
	    PUSH_EVENT(destPtr, EnterNotify, NotifyInferior);
	}
	for (i = upLevels-1; i > 0; i--) {
	    for (winPtr = sourcePtr, j = 0; j < i;
		    winPtr = winPtr->parentPtr, j++) {
		if (winPtr == dispPtr->grabWinPtr) {
		    goto nextIteration;
		}
	    }
	    PUSH_EVENT(winPtr, LeaveNotify, NotifyVirtual);
	    nextIteration: continue;
	}
	PUSH_EVENT(sourcePtr, LeaveNotify, NotifyAncestor);
    } else if (upLevels == 0) {

	/*
	 * DestPtr is an inferior of sourcePtr.
	 */

	if (destPtr != NULL) {
	    PUSH_EVENT(destPtr, EnterNotify, NotifyAncestor);
	}
	for (winPtr = destPtr->parentPtr, i = downLevels-1; i > 0;
		winPtr = winPtr->parentPtr, i--) {
	    PUSH_EVENT(winPtr, EnterNotify, NotifyVirtual);
	}
	if (sourcePtr != NULL) {
	    PUSH_EVENT(sourcePtr, LeaveNotify, NotifyInferior);
	}
    } else {

	/*
	 * Non-linear:  neither window is an inferior of the other.
	 */

	if (destPtr != NULL) {
	    PUSH_EVENT(destPtr, EnterNotify, NotifyNonlinear);
	}
	if (destPtr != dispPtr->grabWinPtr) {
	    for (winPtr = destPtr->parentPtr, i = downLevels-1; i > 0;
		    winPtr = winPtr->parentPtr, i--) {
		PUSH_EVENT(winPtr, EnterNotify, NotifyNonlinearVirtual);
		if (winPtr == dispPtr->grabWinPtr) {
		    break;
		}
	    }
	}
	for (i = upLevels-1; i > 0; i--) {
	    for (winPtr = sourcePtr, j = 0; j < i;
		    winPtr = winPtr->parentPtr, j++) {
		if (winPtr == dispPtr->grabWinPtr) {
		    goto nextWindow;
		}
	    }
	    PUSH_EVENT(winPtr, LeaveNotify, NotifyNonlinearVirtual);
	    nextWindow: continue;
	}
	PUSH_EVENT(sourcePtr, LeaveNotify, NotifyNonlinear);
    }
}

/*
 *----------------------------------------------------------------------
 *
 * MovePointer2 --
 *
 *	This procedure synthesizes  EnterNotify and LeaveNotify events
 *	to correctly transfer the pointer from one window to another.
 *	It is different from MovePointer in that no template X event
 *	needs to be supplied;  this procedure generates the template
 *	event and calls MovePointer.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Synthesized events may be pushed back onto the event queue.
 *
 *----------------------------------------------------------------------
 */

static void
MovePointer2(sourcePtr, destPtr, mode)
    TkWindow *sourcePtr;	/* Window currently containing pointer (NULL
				 * means it's not one managed by this
				 * process). */
    TkWindow *destPtr;		/* Window that is to end up containing the
				 * pointer (NULL means it's not one managed
				 * by this process). */
    int mode;			/* Mode for enter/leave events, such as
				 * NotifyNormal or NotifyUngrab. */
{
    XEvent event;
    Window dummy1, dummy2;
    int dummy3, dummy4;
    TkWindow *winPtr;

    winPtr = sourcePtr;
    if ((winPtr == NULL) || (winPtr->window == None)) {
	winPtr = destPtr;
	if ((winPtr == NULL) || (winPtr->window == None)) {
	    return;
	}
    }

    event.xcrossing.serial = LastKnownRequestProcessed(winPtr->display);
    event.xcrossing.send_event = False;
    event.xcrossing.display = winPtr->display;
    event.xcrossing.root = RootWindow(winPtr->display, winPtr->screenNum);
    event.xcrossing.time = TkCurrentTime(winPtr->dispPtr);
    XQueryPointer(winPtr->display, winPtr->window, &dummy1, &dummy2,
	    &event.xcrossing.x_root, &event.xcrossing.y_root,
	    &dummy3, &dummy4, &event.xcrossing.state);
    event.xcrossing.mode = mode;
    event.xcrossing.focus = False;
    MovePointer(&event, sourcePtr, destPtr);
}

/*
 *----------------------------------------------------------------------
 *
 * TkGrabDeadWindow --
 *
 *	This procedure is invoked whenever a window is deleted, so that
 *	grab-related cleanup can be performed.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Various cleanups happen, such as generating events to move the
 *	pointer back to its "natural" window as if an ungrab had been
 *	done.  See the code.
 *
 *----------------------------------------------------------------------
 */

void
TkGrabDeadWindow(winPtr)
    register TkWindow *winPtr;		/* Window that is in the process
					 * of being deleted. */
{
    TkDisplay *dispPtr = winPtr->dispPtr;

    if (dispPtr->grabWinPtr == winPtr) {
	dispPtr->grabWinPtr = NULL;
	if (!(dispPtr->grabFlags & GRAB_GLOBAL)) {
	    /*
	     * Must generate enter/leave events to move back to the window
	     * that contains the mouse pointer.  We needn't filter events
	     * here like we do in Tk_Ungrab because there are no children
	     * of the grab window left in existence.
	     */

	    movePointerBack:
	    if ((dispPtr->ungrabWinPtr != NULL)
		    && (dispPtr->ungrabWinPtr->mainPtr != winPtr->mainPtr)) {
		dispPtr->ungrabWinPtr = NULL;
	    }
	    MovePointer2(winPtr, dispPtr->ungrabWinPtr, NotifyUngrab);
	}
    } else if (dispPtr->buttonWinPtr == winPtr) {
	/*
	 * The window in which a button was pressed was deleted.  Simulate
	 * dropping the button auto-grab by generating Enter and Leave
	 * events to move the pointer back to the window it's really on
	 * top of.
	 */

	dispPtr->buttonWinPtr = NULL;
	goto movePointerBack;
    }
    if (dispPtr->ungrabWinPtr == winPtr) {
	dispPtr->ungrabWinPtr = NULL;
    }
    if (dispPtr->pointerWinPtr == winPtr) {
	dispPtr->pointerWinPtr = NULL;
    }
    if (dispPtr->serverWinPtr == winPtr) {
	dispPtr->serverWinPtr = NULL;
    }
}
