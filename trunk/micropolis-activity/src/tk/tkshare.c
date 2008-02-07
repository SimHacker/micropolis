/* 
 * tkShare.c --
 *
 *	This module implements a simple mechanism for sharing
 *	mouse- and button-related events among collections of
 *	windows.  It is used primarily for menus.  For example,
 *	if one menu is posted and mouse moves over the menu button
 *	for a different menu, then the menubutton needs to see the
 *	event so that it can post itself and unpost the first menu.
 *
 * Copyright 1990-1992 Regents of the University of California
 * Permission to use, copy, modify, and distribute this
 * software and its documentation for any purpose and without
 * fee is hereby granted, provided that the above copyright
 * notice appear in all copies.  The University of California
 * makes no representations about the suitability of this
 * software for any purpose.  It is provided "as is" without
 * express or implied warranty.
 */

#ifndef lint
static char rcsid[] = "$Header: /user6/ouster/wish/RCS/tkShare.c,v 1.10 92/05/31 16:20:12 ouster Exp $ SPRITE (Berkeley)";
#endif /* not lint */

#include "tkconfig.h"
#include "tk.h"

/*
 * the global variable below is used to tell TkPointerEvent
 * not to do any processing on an event that we're forwarding from one
 * window to another.  This is really ugly.  Eventually this file and
 * tkGrab.c need to get merged together to produce something cleaner.
 */

XEvent *tkShareEventPtr = NULL;

/*
 * Sharing is implemented in terms of groups of windows, where events
 * are shared among all the windows in a group.  One of the following
 * structures exists for each group.
 */

typedef struct Group {
    Tk_Uid groupId;			/* Identifies group uniquely among all
					 * share groups. */
    Tk_Window *windows;			/* Pointer to array of windows in
					 * this group.  Malloc'ed. */
    int numWindows;			/* Number of windows currently in
					 * this group. */
    Tk_Window lastWindow;		/* Last window found that contained
					 * an event.  Needed in order to
					 * notify window when mouse moves out
					 * of it.  NULL means nobody to
					 * notify. */
    XEvent *activeEvent;		/* If non-NULL, means that a recursive
					 * call to Tk_HandleEvent is in
					 * progress for this share group, and
					 * identifies event.  NULL means no
					 * recursive call in progress.  Used
					 * to avoid infinite recursion. */
    struct Group *nextPtr;		/* Next in list of all share groups. */
} Group;

static Group *groupList = NULL;		/* First in list of all share groups
					 * currently defined. */

/*
 * Forward declarations for procedures defined later in this file:
 */

static void		DeleteGroup _ANSI_ARGS_((Group *groupPtr));
static void		ShareEventProc _ANSI_ARGS_((ClientData clientData,
			    XEvent *eventPtr));

/*
 *----------------------------------------------------------------------
 *
 * Tk_ShareEvents --
 *
 *	Add tkwin to a group of windows sharing events.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	In the future, if a button- or mouse-related event occurs for
 *	any window in the same group as tkwin, but the mouse is actually
 *	in tkwin (the event went to a different window because of a
 *	grab) then a synthetic event will be generated with tkwin as
 *	window and adjusted coordinates.
 *
 *----------------------------------------------------------------------
 */

void
Tk_ShareEvents(tkwin, groupId)
    Tk_Window tkwin;			/* Token for window. */
    Tk_Uid groupId;			/* Identifier for group among which
					 * events are to be shared. */
{
    register Group *groupPtr;

    /*
     * See if this group exists.  If so, add the window to the group.
     */

    for (groupPtr = groupList; groupPtr != NULL;
	    groupPtr = groupPtr->nextPtr) {
	Tk_Window *new;

	if (groupPtr->groupId != groupId) {
	    continue;
	}
	new = (Tk_Window *) ckalloc((unsigned)
		(groupPtr->numWindows+1) * sizeof(Tk_Window *));
	memcpy((VOID *) (new+1), (VOID *) groupPtr->windows, 
		(groupPtr->numWindows * sizeof(Tk_Window *)));
	ckfree((char *) groupPtr->windows);
	groupPtr->windows = new;
	groupPtr->windows[0] = tkwin;
	groupPtr->numWindows++;
	break;
    }

    if (groupPtr == NULL) {
	/*
	 * Group doesn't exist.  Make a new one.
	 */
    
	groupPtr = (Group *) ckalloc(sizeof(Group));
	groupPtr->groupId = groupId;
	groupPtr->windows = (Tk_Window *) ckalloc(sizeof (Tk_Window *));
	groupPtr->windows[0] = tkwin;
	groupPtr->numWindows = 1;
	groupPtr->lastWindow = NULL;
	groupPtr->activeEvent = NULL;
	groupPtr->nextPtr = groupList;
	groupList = groupPtr;
    }

    /*
     * Create an event handler so we find out about relevant events
     * that are directed to tkwin.
     */

    Tk_CreateEventHandler(tkwin,
	    ButtonPressMask|ButtonReleaseMask|PointerMotionMask,
	    ShareEventProc, (ClientData) groupPtr);
}

/*
 *----------------------------------------------------------------------
 *
 * Tk_UnshareEvents --
 *
 *	Remove tkwin from a group of windows sharing events.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Tkwin will no longer participate in event-sharing for the
 *	given group, either as source of events or as destination.
 *
 *----------------------------------------------------------------------
 */

void
Tk_UnshareEvents(tkwin, groupId)
    Tk_Window tkwin;			/* Token for window. */
    Tk_Uid groupId;			/* Identifier for group. */
{
    register Group *groupPtr;
    int i;

    for (groupPtr = groupList; groupPtr != NULL;
	    groupPtr = groupPtr->nextPtr) {
	if (groupPtr->groupId != groupId) {
	    continue;
	}
	if (groupPtr->lastWindow == tkwin) {
	    groupPtr->lastWindow = NULL;
	}
	for (i = 0; i < groupPtr->numWindows; i++) {
	    if (groupPtr->windows[i] != tkwin) {
		continue;
	    }
	    if ((i+1) < groupPtr->numWindows) {
		memcpy((VOID *) (groupPtr->windows + i),
			(VOID *) (groupPtr->windows + i + 1),
			(groupPtr->numWindows - (i+1))*sizeof(Tk_Window *));
	    }
	    groupPtr->numWindows--;
	    Tk_DeleteEventHandler(tkwin,
		    ButtonPressMask|ButtonReleaseMask|PointerMotionMask,
		    ShareEventProc, (ClientData) groupPtr);
	    if (groupPtr->numWindows == 0) {
		DeleteGroup(groupPtr);
	    }
	    return;
	}
    }
}

/*
 *----------------------------------------------------------------------
 *
 * DeleteGroup --
 *
 *	This procedure is called when a group has no more members.
 *	It deletes the group from the list of existing groups.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Memory gets freed.
 *
 *----------------------------------------------------------------------
 */

static void
DeleteGroup(groupPtr)
    Group *groupPtr;			/* Group to delete. */
{
    if (groupList == groupPtr) {
	groupList = groupPtr->nextPtr;
    } else {
	register Group *prevPtr;

	for (prevPtr = groupList; ; prevPtr = prevPtr->nextPtr) {
	    if (prevPtr == NULL) {
		panic("DeleteGroup couldn't find group on shareList");
	    }
	    if (prevPtr->nextPtr == groupPtr) {
		prevPtr->nextPtr = groupPtr->nextPtr;
		break;
	    }
	}
    }
    ckfree((char *) groupPtr->windows);
    ckfree((char *) groupPtr);
}

/*
 *----------------------------------------------------------------------
 *
 * ShareEventProc --
 *
 *	This procedure is invoked by the Tk dispatcher when an event
 *	occurs for which we need to implement sharing.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	If the mouse is actually in a window other than the one for
 *	which the event occurred, generate a new event translated to
 *	that window.
 *
 *----------------------------------------------------------------------
 */

static void
ShareEventProc(clientData, eventPtr)
    ClientData clientData;		/* Information about share group. */
    register XEvent *eventPtr;		/* Event that just occurred. */
{
    register Group *groupPtr = (Group *) clientData;
    register Tk_Window tkwin;
    Window window;
    XEvent newEvent, *savedActive, *savedShareEventPtr;
    int i, x, y;
    Tk_Uid savedId;
    register Group *grpPtr;

    /*
     * If this event was a synthetic one that we generated, then
     * don't bother to process it again.
     */

    if (groupPtr->activeEvent == eventPtr) {
	return;
    }
    savedActive = groupPtr->activeEvent;
    groupPtr->activeEvent = &newEvent;
    savedId = groupPtr->groupId;

    savedShareEventPtr = tkShareEventPtr;
    tkShareEventPtr = &newEvent;

    /*
     * Scan through all of the windows for this group to find the
     * first one (if any) that contains the event.
     */

    tkwin = NULL;		/* Not needed, but stops compiler warning. */
    for (i = 0; i < groupPtr->numWindows; i++) {
	Tk_Window tkwin2;

	tkwin = groupPtr->windows[i];
	Tk_GetRootCoords(tkwin, &x, &y);
	x = eventPtr->xmotion.x_root - x - Tk_Changes(tkwin)->border_width;
	y = eventPtr->xmotion.y_root - y - Tk_Changes(tkwin)->border_width;
	if ((x < 0) || (y < 0) || (x >= Tk_Width(tkwin))
		|| (y >= Tk_Height(tkwin))) {
	    continue;
	}
	for (tkwin2 = tkwin; ; tkwin2 = Tk_Parent(tkwin2)) {
	    if (tkwin2 == NULL) {
		goto foundWindow;
	    }
	    if (!Tk_IsMapped(tkwin2)) {
		break;
	    }
	    if (((Tk_FakeWin *) (tkwin2))->flags & TK_TOP_LEVEL) {
		goto foundWindow;
	    }
	}
    }

    foundWindow:
    window = None;	/* Not really needed but stops compiler warning. */
    if (i >= groupPtr->numWindows) {
	tkwin = NULL;
    } else {
	window = Tk_WindowId(tkwin);
    }

    /*
     * SPECIAL NOTE:  it is possible that any or all of the information
     * in groupPtr could be modified as part of the processing of the
     * events that we generate and hand to Tk_HandleEvent below.  For this
     * to work smoothly, it is imperative that we extract any information
     * we need from groupPtr (and from tkwin's, since they could be
     * deleted) before the first call to Tk_HandleEvent below.  The code
     * below may potentially pass an X window identifier to Tk_HandleEvent
     * after the window has been deleted, but as long as identifiers
     * aren't recycled Tk_HandleEvent will simply discard the event if
     * this occurs.
     */

    /*
     * If the pointer is in a different window now than the last time
     * we were invoked, send a LeaveNotify event to the old window and
     * an EnterNotify event to the new window.
     */

    newEvent = *eventPtr;
    newEvent.xany.send_event = True;
    if (tkwin != groupPtr->lastWindow) {
	newEvent = *eventPtr;
	newEvent.xany.send_event = True;
	newEvent.xcrossing.mode = TK_NOTIFY_SHARE;
	newEvent.xcrossing.detail = NotifyAncestor;
	newEvent.xcrossing.same_screen = True;
	newEvent.xcrossing.state = eventPtr->xmotion.state;
	if (groupPtr->lastWindow != NULL) {
	    newEvent.xcrossing.type = LeaveNotify;
	    newEvent.xcrossing.window = Tk_WindowId(groupPtr->lastWindow);
	    Tk_GetRootCoords(groupPtr->lastWindow, &newEvent.xcrossing.x,
		    &newEvent.xcrossing.y);
	    newEvent.xcrossing.x = eventPtr->xmotion.x_root
		    - newEvent.xcrossing.x
		    - Tk_Changes(groupPtr->lastWindow)->border_width;
	    newEvent.xcrossing.y = eventPtr->xmotion.y_root
		    - newEvent.xcrossing.y
		    - Tk_Changes(groupPtr->lastWindow)->border_width;
	    Tk_HandleEvent(&newEvent);
	}
	if (tkwin != NULL) {
	    newEvent.xcrossing.type = EnterNotify;
	    newEvent.xcrossing.window = window;
	    newEvent.xcrossing.x = x;
	    newEvent.xcrossing.y = y;
	    Tk_HandleEvent(&newEvent);
	}
	groupPtr->lastWindow = tkwin;
    }

    /*
     * If the pointer is in the window to which the event was sent,
     * then we needn't do any forwarding at all.  Ditto if the pointer
     * isn't in any window at all.
     */

    if ((tkwin != NULL) && (Tk_WindowId(tkwin) != eventPtr->xmotion.window)) {
	newEvent = *eventPtr;
	newEvent.xmotion.send_event = True;
	newEvent.xmotion.window = window;
	newEvent.xmotion.x = x;
	newEvent.xmotion.y = y;
	Tk_HandleEvent(&newEvent);
    }

    /*
     * Only restore the activeEvent if the group still exists.
     * (It could be deleted as a side effect of processing the event.)
     */

    for (grpPtr = groupList; grpPtr != NULL; grpPtr = grpPtr->nextPtr) {
	if (grpPtr->groupId == savedId) {
	    groupPtr->activeEvent = savedActive;
	    break;
	}
    }

    tkShareEventPtr = savedShareEventPtr;
}
