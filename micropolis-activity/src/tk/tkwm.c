/* 
 * tkWm.c --
 *
 *	This module takes care of the interactions between a Tk-based
 *	application and the window manager.  Among other things, it
 *	implements the "wm" command and passes geometry information
 *	to the window manager.
 *
 * Copyright 1991 Regents of the University of California.
 * Permission to use, copy, modify, and distribute this
 * software and its documentation for any purpose and without
 * fee is hereby granted, provided that the above copyright
 * notice appear in all copies.  The University of California
 * makes no representations about the suitability of this
 * software for any purpose.  It is provided "as is" without
 * express or implied warranty.
 */

#ifndef lint
static char rcsid[] = "$Header: /user6/ouster/wish/RCS/tkWm.c,v 1.32 92/08/21 16:26:31 ouster Exp $ SPRITE (Berkeley)";
#endif

#include "tkconfig.h"
#include "tkint.h"
#include "tkwm.h"

/*
 * The definitions below compensate for the lack of some definitions
 * under X11R3.
 */

#ifdef X11R3
#define PBaseSize	(1L<<8)
#endif

/*
 * A data structure of the following type holds window-manager-related
 * information for each top-level window in an application.
 */

typedef struct TkWmInfo {
    TkWindow *winPtr;		/* Pointer to main Tk information for
				 * this window. */
    Window reparent;		/* If the window has been reparented, this
				 * gives the ID of the ancestor of the window
				 * that is a child of the root window (may
				 * not be window's immediate parent).  If
				 * the window isn't reparented, this has the
				 * value None. */
    Tk_Uid titleUid;		/* Title to display in window caption.  If
				 * NULL, use name of widget. */
    Tk_Uid iconName;		/* Name to display in icon. */
    Window master;		/* Master window for TRANSIENT_FOR property,
				 * or None. */
    XWMHints hints;		/* Various pieces of information for
				 * window manager. */
    Tk_Uid leaderName;		/* Path name of leader of window group
				 * (corresponds to hints.window_group).
				 * Note:  this field doesn't get updated
				 * if leader is destroyed. */
    Tk_Uid iconWindowName;	/* Path name of window specified as icon
				 * window for this window, or NULL.  Note:
				 * this field doesn't get updated if
				 * iconWindowName is destroyed. */
    Tk_Uid masterWindowName;	/* Path name of window specified as master
				 * in "wm transient" command, or NULL.
				 * Note:  this field doesn't get updated if
				 * masterWindowName is destroyed. */

    /*
     * Information used to construct an XSizeHints structure for
     * the window manager:
     */

    int sizeHintsFlags;		/* Flags word for XSizeHints structure.
				 * If the PBaseSize flag is set then the
				 * window is gridded;  otherwise it isn't
				 * gridded. */
    int minWidth, minHeight;	/* Minimum dimensions of window, in
				 * grid units, not pixels. */
    int maxWidth, maxHeight;	/* Maximum dimensions of window, in
				 * grid units, not pixels. */
    int widthInc, heightInc;	/* Increments for size changes (# pixels
				 * per step). */
    struct {
	int x;	/* numerator */
	int y;  /* denominator */
    } minAspect, maxAspect;	/* Min/max aspect ratios for window. */
    int reqGridWidth, reqGridHeight;
				/* The dimensions of the window (in
				 * grid units) requested through
				 * the geometry manager. */
    int gravity;		/* Desired window gravity. */

    /*
     * Information used to manage the size and location of a window.
     */

    int prevReqWidth, prevReqHeight;
				/* Last known size preferences, as specified
				 * to Tk_GeometryRequest.  Used to tell when
				 * the preferred dimensions have changed. */
    int width, height;		/* Desired dimensions of window, specified
				 * in grid units.  These values are
				 * set by the "wm geometry" command and by
				 * ConfigureNotify events (for when wm
				 * resizes window).  -1 means user hasn't
				 * requested dimensions. */
    int x, y;			/* Desired X and Y coordinates for window.
				 * These values are set by "wm geometry",
				 * plus by ConfigureNotify events (when wm
				 * moves window).  These numbers are
				 * different than the numbers stored in
				 * winPtr->changes because (a) they could be
				 * measured from the right or bottom edge
				 * of the screen (see WM_NEGATIVE_X and
				 * WM_NEGATIVE_Y flags) and (b) if the window
				 * has been reparented then they refer to the
				 * parent rather than the window itself. */
    int parentWidth, parentHeight;
				/* Width and height of reparent, in pixels
				 * *including border*.  If window hasn't been
				 * reparented then these will be the outer
				 * dimensions of the window, including
				 * border. */
    int xInParent, yInParent;	/* Offset of window within reparent,  measured
				 * from upper-left outer corner of parent's
				 * border.  If not reparented then these are
				 * zero. */
    unsigned long configRequest;/* Serial number of last request that we
				 * issued to change geometry of window.
				 * Used to discard configure events that
				 * we know will be superceded. */
    int configWidth, configHeight;
				/* Dimensions passed to last request that we
				 * issued to change geometry of window.  Used
				 * to eliminate redundant resize operations. */

    int flags;			/* Miscellaneous flags, defined below. */

    char *deleteCmd;            /* Command to execute when a WM_DELETE_WINDOW
				 * ICCCM ClientMessage arrives for this window.
				 *
				 * If it is the empty string "" or has never
				 * been set (is char *)NULL) via the "wm" tcl
				 * command the window is destroyed.
				 *
				 * If it is a non-empty string, the name of 
				 * the window is appended on to the end
				 * of the string and it is executed
				 * within the interpreter associated with
				 * the top level window. 
				 */
    struct TkWmInfo *nextPtr;	/* Next in list of all top-level windows. */
} WmInfo;

/*
 * Flag values for WmInfo structures:
 *
 * WM_NEVER_MAPPED -		non-zero means window has never been
 *				mapped;  need to update all info when
 *				window is first mapped.
 * WM_UPDATE_PENDING -		non-zero means a call to UpdateGeometryInfo
 *				has already been scheduled for this
 *				window;  no need to schedule another one.
 * WM_NEGATIVE_X -		non-zero means x-coordinate is measured in
 *				pixels from right edge of screen, rather
 *				than from left edge.
 * WM_NEGATIVE_Y -		non-zero means y-coordinate is measured in
 *				pixels up from bottom of screen, rather than
 *				down from top.
 * WM_UPDATE_SIZE_HINTS -	non-zero means that new size hints need to be
 *				propagated to window manager.
 * WM_NESTED_REPARENT -		non-zero means that the window has been
 *				reparented several levels deep in a hierarchy
 *				(i.e. reparent isn't the window's immediate
 *				parent).
 * WM_CONFIG_PENDING -		non-zero means we've asked for the top-level
 *				window to be resized but haven't seen a
 *				ConfigureNotify event to indicate that the
 *				resize occurred.
 * WM_CONFIG_AGAIN -		non-zero means we need to reconfigure the
 *				window again as soon as the current configure
 *				request has been processed by the window
 *				manager.
 * WM_FULL_SCREEN -		non-zero means that the window is in full screen mode.
 */

#define WM_NEVER_MAPPED		1
#define WM_UPDATE_PENDING	2
#define WM_NEGATIVE_X		4
#define WM_NEGATIVE_Y		8
#define WM_UPDATE_SIZE_HINTS	0x10
#define WM_NESTED_REPARENT	0x20
#define WM_CONFIG_PENDING	0x40
#define WM_CONFIG_AGAIN		0x100
#define WM_FULL_SCREEN		0x200

/*
 * This module keeps a list of all top-level windows, primarily to
 * simplify the job of Tk_CoordsToWindow.
 */

static WmInfo *firstWmPtr = NULL;	/* Points to first top-level window. */

#define IS_GRIDDED(wmPtr) ((wmPtr)->sizeHintsFlags & PBaseSize)

/*
 * Forward declarations for procedures defined in this file:
 */

static int		ParseGeometry _ANSI_ARGS_ ((Tcl_Interp *interp,
			    char *string, TkWindow *winPtr));
static void		TopLevelEventProc _ANSI_ARGS_((ClientData clientData,
			    XEvent *eventPtr));
static void		TopLevelReqProc _ANSI_ARGS_((ClientData dummy,
			    Tk_Window tkwin));
static void		UpdateGeometryInfo _ANSI_ARGS_((
			    ClientData clientData));
static void		UpdateHints _ANSI_ARGS_((TkWindow *winPtr));
static void		UpdateSizeHints _ANSI_ARGS_((TkWindow *winPtr));

/*
 *--------------------------------------------------------------
 *
 * TkWmNewWindow --
 *
 *	This procedure is invoked whenever a new top-level
 *	window is created.  Its job is to initialize the WmInfo
 *	structure for the window.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	A WmInfo structure gets allocated and initialized.
 *
 *--------------------------------------------------------------
 */

void
TkWmNewWindow(winPtr)
    TkWindow *winPtr;		/* Newly-created top-level window. */
{
    register WmInfo *wmPtr;

    wmPtr = (WmInfo *) ckalloc(sizeof(WmInfo));
    wmPtr->winPtr = winPtr;
    wmPtr->reparent = None;
    wmPtr->titleUid = NULL;
    wmPtr->iconName = NULL;
    wmPtr->master = None;
    wmPtr->hints.flags = InputHint | StateHint;
    wmPtr->hints.input = True;
    wmPtr->hints.initial_state = NormalState;
    wmPtr->hints.icon_pixmap = None;
    wmPtr->hints.icon_window = None;
    wmPtr->hints.icon_x = wmPtr->hints.icon_y = 0;
    wmPtr->hints.icon_mask = None;
    wmPtr->hints.window_group = None;
    wmPtr->leaderName = NULL;
    wmPtr->iconWindowName = NULL;
    wmPtr->masterWindowName = NULL;
    wmPtr->sizeHintsFlags = 0;
    wmPtr->minWidth = wmPtr->minHeight = 0;
    wmPtr->maxWidth = wmPtr->maxHeight = 10000;
    wmPtr->widthInc = wmPtr->heightInc = 1;
    wmPtr->minAspect.x = wmPtr->minAspect.y = 1;
    wmPtr->maxAspect.x = wmPtr->maxAspect.y = 1;
    wmPtr->reqGridWidth = wmPtr->reqGridHeight = -1;
    wmPtr->prevReqWidth = wmPtr->prevReqHeight = -1;
    wmPtr->gravity = NorthWestGravity;
    wmPtr->width = -1;
    wmPtr->height = -1;
    wmPtr->x = winPtr->changes.x;
    wmPtr->y = winPtr->changes.y;
    wmPtr->parentWidth = winPtr->changes.width
	    + 2*winPtr->changes.border_width;
    wmPtr->parentHeight = winPtr->changes.height
	    + 2*winPtr->changes.border_width;
    wmPtr->xInParent = wmPtr->yInParent = 0;
    wmPtr->configRequest = 0;
    wmPtr->configWidth = -1;
    wmPtr->configHeight = -1;
    wmPtr->flags = WM_NEVER_MAPPED;
    wmPtr->deleteCmd = (char *)0;
    wmPtr->nextPtr = firstWmPtr;
    firstWmPtr = wmPtr;
    winPtr->wmInfoPtr = wmPtr;

    /*
     * Tk must monitor certain events for top-level windows:
     * (a) structure events, in order to detect size and position changes
     *     caused by window managers.
     * (b) enter/level events, in order to perform focussing correctly.
     */

    Tk_CreateEventHandler((Tk_Window) winPtr,
	    StructureNotifyMask|EnterWindowMask|LeaveWindowMask,
	    TopLevelEventProc, (ClientData) winPtr);

    /*
     * Arrange for geometry requests to be reflected from the window
     * to the window manager.
     */

    Tk_ManageGeometry((Tk_Window) winPtr, TopLevelReqProc, (ClientData) 0);
}

/*
 *--------------------------------------------------------------
 *
 * TkWmMapWindow --
 *
 *	This procedure is invoked just before a top-level window
 *	is mapped.  It gives this module a chance to update all
 *	window-manager-related information in properties before
 *	the window manager sees the map event and checks the
 *	properties.
 *
 * Results:
 *	Returns non-zero if it's OK for the window to be mapped, 0
 *	if the caller shouldn't map the window after all (e.g. because
 *	it has been withdrawn).
 *
 * Side effects:
 *	Properties of winPtr may get updated to provide up-to-date
 *	information to the window manager.
 *
 *--------------------------------------------------------------
 */

int
TkWmMapWindow(winPtr)
    TkWindow *winPtr;		/* Top-level window that's about to
				 * be mapped. */
{
    register WmInfo *wmPtr = winPtr->wmInfoPtr;
#ifndef X11R3
    XTextProperty textProp;
#endif

    /*
     * Set the MAPPED flag if the window is going to appear in its normal
     * state:  if it's going to be iconified or withdrawn then it won't
     * ever be mapped.
     */

    if (wmPtr->hints.initial_state == NormalState) {
	winPtr->flags |= TK_MAPPED;
    }
    if (!(wmPtr->flags & WM_NEVER_MAPPED)) {
	return 1;
    }
    wmPtr->flags &= ~WM_NEVER_MAPPED;

    /*
     * This is the first time this window has ever been mapped.
     * Store all the window-manager-related information for the
     * window.
     */

#ifndef X11R3
    if (wmPtr->titleUid == NULL) {
	wmPtr->titleUid = winPtr->nameUid;
    }
    if (XStringListToTextProperty(&wmPtr->titleUid, 1, &textProp)  != 0) {
	XSetWMName(winPtr->display, winPtr->window, &textProp);
	XFree((char *) textProp.value);
    }
#endif

    TkWmSetClass(winPtr);
    TkWmSetWmProtocols(winPtr);

    if (wmPtr->iconName != NULL) {
	XSetIconName(winPtr->display, winPtr->window, wmPtr->iconName);
    }

    if (wmPtr->master != None) {
	XSetTransientForHint(winPtr->display, winPtr->window, wmPtr->master);
    }

    wmPtr->flags |= WM_UPDATE_SIZE_HINTS;
    UpdateGeometryInfo((ClientData) winPtr);
    UpdateHints(winPtr);
    if (wmPtr->hints.initial_state == WithdrawnState) {
	return 0;
    }
    return 1;
}

/*
 *--------------------------------------------------------------
 *
 * TkWmDeadWindow --
 *
 *	This procedure is invoked when a top-level window is
 *	about to be deleted.  It cleans up the wm-related data
 *	structures for the window.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The WmInfo structure for winPtr gets freed up.
 *
 *--------------------------------------------------------------
 */

void
TkWmDeadWindow(winPtr)
    TkWindow *winPtr;		/* Newly-created top-level window. */
{
    register WmInfo *wmPtr = winPtr->wmInfoPtr;

    if (wmPtr == NULL) {
	return;
    }
    if (firstWmPtr == wmPtr) {
	firstWmPtr = wmPtr->nextPtr;
    } else {
	register WmInfo *prevPtr;

	for (prevPtr = firstWmPtr; ; prevPtr = prevPtr->nextPtr) {
	    if (prevPtr == NULL) {
		panic("couldn't unlink window in TkWmDeadWindow");
	    }
	    if (prevPtr->nextPtr == wmPtr) {
		prevPtr->nextPtr = wmPtr->nextPtr;
		break;
	    }
	}
    }
    if (wmPtr->hints.flags & IconPixmapHint) {
	Tk_FreeBitmap(wmPtr->hints.icon_pixmap);
    }
    if (wmPtr->hints.flags & IconMaskHint) {
	Tk_FreeBitmap(wmPtr->hints.icon_mask);
    }
    if (wmPtr->flags & WM_UPDATE_PENDING) {
	Tk_CancelIdleCall(UpdateGeometryInfo, (ClientData) winPtr);
    }
    if (wmPtr->deleteCmd) {
        ckfree(wmPtr->deleteCmd);
    }
    ckfree((char *) wmPtr);
    winPtr->wmInfoPtr = NULL;
}

/*
 *--------------------------------------------------------------
 *
 * TkWmSetClass --
 *
 *	This procedure is invoked whenever a top-level window's
 *	class is changed.  If the window has been mapped then this
 *	procedure updates the window manager property for the
 *	class.  If the window hasn't been mapped, the update is
 *	deferred until just before the first mapping.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	A window property may get updated.
 *
 *--------------------------------------------------------------
 */

void
TkWmSetClass(winPtr)
    TkWindow *winPtr;		/* Newly-created top-level window. */
{
    if (winPtr->wmInfoPtr->flags & WM_NEVER_MAPPED) {
	return;
    }

#ifndef X11R3
    if (winPtr->classUid != NULL) {
	XClassHint *classPtr;

	classPtr = XAllocClassHint();
	classPtr->res_name = winPtr->nameUid;
	classPtr->res_class = winPtr->classUid;
	XSetClassHint(winPtr->display, winPtr->window, classPtr);
	XFree((char *) classPtr);
    }
#endif
}

/*
 *----------------------------------------------------------------------
 *
 * Tk_WmCmd --
 *
 *	This procedure is invoked to process the "wm" Tcl command.
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
Tk_WmCmd(clientData, interp, argc, argv)
    ClientData clientData;	/* Main window associated with
				 * interpreter. */
    Tcl_Interp *interp;		/* Current interpreter. */
    int argc;			/* Number of arguments. */
    char **argv;		/* Argument strings. */
{
    Tk_Window tkwin = (Tk_Window) clientData;
    TkWindow *winPtr;
    register WmInfo *wmPtr;
    char c;
    int length;

    if (argc < 3) {
	Tcl_AppendResult(interp, "wrong # args: should be \"",
		argv[0], " option window ?arg ...?\"", (char *) NULL);
	return TCL_ERROR;
    }
    winPtr = (TkWindow *) Tk_NameToWindow(interp, argv[2], tkwin);
    if (winPtr == NULL) {
	return TCL_ERROR;
    }
    if (!(winPtr->flags & TK_TOP_LEVEL)) {
	Tcl_AppendResult(interp, "window \"", winPtr->pathName,
		"\" isn't a top-level window", (char *) NULL);
	return TCL_ERROR;
    }
    wmPtr = winPtr->wmInfoPtr;
    c = argv[1][0];
    length = strlen(argv[1]);
    if ((c == 'a') && (strncmp(argv[1], "aspect", length) == 0)) {
	int numer1, denom1, numer2, denom2;

	if ((argc != 3) && (argc != 7)) {
	    Tcl_AppendResult(interp, "wrong # arguments: must be \"",
		    argv[0], " aspect window ?minNumer minDenom ",
		    "maxNumer maxDenom?\"", (char *) NULL);
	    return TCL_ERROR;
	}
	if (argc == 3) {
	    if (wmPtr->sizeHintsFlags & PAspect) {
		sprintf(interp->result, "%d %d %d %d", wmPtr->minAspect.x,
			wmPtr->minAspect.y, wmPtr->maxAspect.x,
			wmPtr->maxAspect.y);
	    }
	    return TCL_OK;
	}
	if (*argv[3] == '\0') {
	    wmPtr->sizeHintsFlags &= ~PAspect;
	} else {
	    if ((Tcl_GetInt(interp, argv[3], &numer1) != TCL_OK)
		    || (Tcl_GetInt(interp, argv[4], &denom1) != TCL_OK)
		    || (Tcl_GetInt(interp, argv[5], &numer2) != TCL_OK)
		    || (Tcl_GetInt(interp, argv[6], &denom2) != TCL_OK)) {
		return TCL_ERROR;
	    }
	    if ((numer1 <= 0) || (denom1 <= 0) || (numer2 <= 0) ||
		    (denom2 <= 0)) {
		interp->result = "aspect number can't be <= 0";
		return TCL_ERROR;
	    }
	    wmPtr->minAspect.x = numer1;
	    wmPtr->minAspect.y = denom1;
	    wmPtr->maxAspect.x = numer2;
	    wmPtr->maxAspect.y = denom2;
	    wmPtr->sizeHintsFlags |= PAspect;
	}
	wmPtr->flags |= WM_UPDATE_SIZE_HINTS;
	goto updateGeom;
    } else if ((c == 'd') && (strncmp(argv[1], "deiconify", length) == 0)) {
	if (argc != 3) {
	    Tcl_AppendResult(interp, "wrong # arguments: must be \"",
		    argv[0], " deiconify window\"", (char *) NULL);
	    return TCL_ERROR;
	}
	wmPtr->hints.initial_state = NormalState;
	if (wmPtr->flags & WM_NEVER_MAPPED) {
	    return TCL_OK;
	}
	Tk_MapWindow((Tk_Window) winPtr);
    } else if ((c == 'f') && (strncmp(argv[1], "focusmodel", length) == 0)) {
	if ((argc != 3) && (argc != 4)) {
	    Tcl_AppendResult(interp, "wrong # arguments: must be \"",
		    argv[0], " focusmodel window ?active|passive?\"",
		    (char *) NULL);
	    return TCL_ERROR;
	}
	if (argc == 3) {
	    interp->result = wmPtr->hints.input ? "passive" : "active";
	    return TCL_OK;
	}
	c = argv[3][0];
	length = strlen(argv[3]);
	if ((c == 'a') && (strncmp(argv[3], "active", length) == 0)) {
	    wmPtr->hints.input = False;
	} else if ((c == 'p') && (strncmp(argv[3], "passive", length) == 0)) {
	    wmPtr->hints.input = True;
	} else {
	    Tcl_AppendResult(interp, "bad argument \"", argv[3],
		    "\": must be active or passive", (char *) NULL);
	    return TCL_ERROR;
	}
	UpdateHints(winPtr);
    } else if ((c == 'f') && (strncmp(argv[1], "fullscreen", length) == 0)) {
	if (argc != 4) {
	    Tcl_AppendResult(interp, "wrong # arguments: must be \"",
		    argv[0], " fullscreen window on|off\"",
		    (char *) NULL);
	    return TCL_ERROR;
	}
	c = argv[3][0];
	length = strlen(argv[3]);
	if (strncmp(argv[3], "on", length) == 0) {
	    wmPtr->flags &= ~WM_FULL_SCREEN;
	} else if (strncmp(argv[3], "off", length) == 0) {
	    wmPtr->flags |= WM_FULL_SCREEN;
	} else {
	    Tcl_AppendResult(interp, "bad argument \"", argv[3],
		    "\": must be on or off", (char *) NULL);
	    return TCL_ERROR;
	}

	static Atom _NET_WM_STATE;
	static Atom _NET_WM_STATE_REMOVE;
	static Atom _NET_WM_STATE_ADD;
	static Atom _NET_WM_STATE_FULLSCREEN;

	if (!_NET_WM_STATE) {
#define MAX_ATOMS 30
	  Atom *atom_ptr[MAX_ATOMS];
	  char *names[MAX_ATOMS];
	  int i = 0;
#define atom(a,b) atom_ptr[i] = &a; names[i] = b; i++
	  atom(_NET_WM_STATE, "_NET_WM_STATE");
	  atom(_NET_WM_STATE_REMOVE, "_NET_WM_STATE_REMOVE");
	  atom(_NET_WM_STATE_ADD, "_NET_WM_STATE_ADD");
	  atom(_NET_WM_STATE_FULLSCREEN, "_NET_WM_STATE_FULLSCREEN");
#undef atom
	  Atom atoms[MAX_ATOMS];
	  XInternAtoms(winPtr->display, names, i, 0, atoms);
	  for (; i--;) {
	    *atom_ptr[i] = atoms[i];
	  }
	}

	XEvent e;
	e.xany.type = ClientMessage;
	e.xany.window = winPtr->window;
	e.xclient.message_type = _NET_WM_STATE;
	e.xclient.format = 32;
	e.xclient.data.l[0] = 
	  (wmPtr->flags & WM_FULL_SCREEN)
	    ? _NET_WM_STATE_ADD
	    : _NET_WM_STATE_REMOVE;
	e.xclient.data.l[1] = (long)_NET_WM_STATE_FULLSCREEN;
	e.xclient.data.l[2] = (long)0;
	e.xclient.data.l[3] = (long)0;
	e.xclient.data.l[4] = (long)0;
	XSendEvent(winPtr->display, RootWindow(winPtr->display, winPtr->screenNum), 0,
		   SubstructureNotifyMask|SubstructureRedirectMask, &e);

    } else if ((c == 'g') && (strncmp(argv[1], "geometry", length) == 0)
	    && (length >= 2)) {
	char xSign, ySign;
	int width, height;

	if ((argc != 3) && (argc != 4)) {
	    Tcl_AppendResult(interp, "wrong # arguments: must be \"",
		    argv[0], " geometry window ?newGeometry?\"",
		    (char *) NULL);
	    return TCL_ERROR;
	}
	if (argc == 3) {
	    xSign = (wmPtr->flags & WM_NEGATIVE_X) ? '-' : '+';
	    ySign = (wmPtr->flags & WM_NEGATIVE_Y) ? '-' : '+';
	    if (wmPtr->width != -1) {
		width = wmPtr->width;
		height = wmPtr->height;
	    } else if (IS_GRIDDED(wmPtr)) {
		width = wmPtr->reqGridWidth;
		height = wmPtr->reqGridHeight;
	    } else {
		width = winPtr->reqWidth;
		height = winPtr->reqHeight;
	    }
	    sprintf(interp->result, "%dx%d%c%d%c%d", width, height,
		    xSign, wmPtr->x, ySign, wmPtr->y);
	    return TCL_OK;
	}
	if (*argv[3] == '\0') {
	    wmPtr->width = -1;
	    wmPtr->height = -1;
	    goto updateGeom;
	}
	return ParseGeometry(interp, argv[3], winPtr);
    } else if ((c == 'g') && (strncmp(argv[1], "grid", length) == 0)
	    && (length >= 3)) {
	int reqWidth, reqHeight, widthInc, heightInc;

	if ((argc != 3) && (argc != 7)) {
	    Tcl_AppendResult(interp, "wrong # arguments: must be \"",
		    argv[0], " reqsize window ?baseWidth baseHeight ",
		    "widthInc heightInc?\"", (char *) NULL);
	    return TCL_ERROR;
	}
	if (argc == 3) {
	    if (wmPtr->sizeHintsFlags & PBaseSize) {
		sprintf(interp->result, "%d %d %d %d", wmPtr->reqGridWidth,
			wmPtr->reqGridHeight, wmPtr->widthInc,
			wmPtr->heightInc);
	    }
	    return TCL_OK;
	}
	if (*argv[3] == '\0') {
	    /*
	     * Turn off gridding and reset the width and height
	     * to make sense as ungridded numbers.
	     */

	    wmPtr->sizeHintsFlags &= ~(PBaseSize|PResizeInc);
	    wmPtr->widthInc = 1;
	    wmPtr->heightInc = 1;
	    if (wmPtr->width != -1) {
		wmPtr->width = winPtr->reqWidth + (wmPtr->width
			- wmPtr->reqGridWidth)*wmPtr->widthInc;
		wmPtr->height = winPtr->reqHeight + (wmPtr->height
			- wmPtr->reqGridHeight)*wmPtr->heightInc;
	    }
	} else {
	    if ((Tcl_GetInt(interp, argv[3], &reqWidth) != TCL_OK)
		    || (Tcl_GetInt(interp, argv[4], &reqHeight) != TCL_OK)
		    || (Tcl_GetInt(interp, argv[5], &widthInc) != TCL_OK)
		    || (Tcl_GetInt(interp, argv[6], &heightInc) != TCL_OK)) {
		return TCL_ERROR;
	    }
	    if (reqWidth < 0) {
		interp->result = "baseWidth can't be < 0";
		return TCL_ERROR;
	    }
	    if (reqHeight < 0) {
		interp->result = "baseHeight can't be < 0";
		return TCL_ERROR;
	    }
	    if (widthInc < 0) {
		interp->result = "widthInc can't be < 0";
		return TCL_ERROR;
	    }
	    if (heightInc < 0) {
		interp->result = "heightInc can't be < 0";
		return TCL_ERROR;
	    }
	    Tk_SetGrid((Tk_Window) tkwin, reqWidth, reqHeight, widthInc,
		    heightInc);
	}
	wmPtr->flags |= WM_UPDATE_SIZE_HINTS;
	goto updateGeom;
    } else if ((c == 'g') && (strncmp(argv[1], "group", length) == 0)
	    && (length >= 3)) {
	Tk_Window tkwin2;

	if ((argc != 3) && (argc != 4)) {
	    Tcl_AppendResult(interp, "wrong # arguments: must be \"",
		    argv[0], " group window ?pathName?\"",
		    (char *) NULL);
	    return TCL_ERROR;
	}
	if (argc == 3) {
	    if (wmPtr->hints.flags & WindowGroupHint) {
		interp->result = wmPtr->leaderName;
	    }
	    return TCL_OK;
	}
	if (*argv[3] == '\0') {
	    wmPtr->hints.flags &= ~WindowGroupHint;
	    wmPtr->leaderName = NULL;
	} else {
	    tkwin2 = Tk_NameToWindow(interp, argv[3], tkwin);
	    if (tkwin2 == NULL) {
		return TCL_ERROR;
	    }
	    Tk_MakeWindowExist(tkwin2);
	    wmPtr->hints.window_group = Tk_WindowId(tkwin2);
	    wmPtr->hints.flags |= WindowGroupHint;
	    wmPtr->leaderName = Tk_PathName(tkwin2);
	}
	UpdateHints(winPtr);
    } else if ((c == 'i') && (strncmp(argv[1], "iconbitmap", length) == 0)
	    && (length >= 5)) {
	Pixmap pixmap;

	if ((argc != 3) && (argc != 4)) {
	    Tcl_AppendResult(interp, "wrong # arguments: must be \"",
		    argv[0], " iconbitmap window ?bitmap?\"",
		    (char *) NULL);
	    return TCL_ERROR;
	}
	if (argc == 3) {
	    if (wmPtr->hints.flags & IconPixmapHint) {
		interp->result = Tk_NameOfBitmap(wmPtr->hints.icon_pixmap);
	    }
	    return TCL_OK;
	}
	if (*argv[3] == '\0') {
	    if (wmPtr->hints.icon_pixmap != None) {
		Tk_FreeBitmap(wmPtr->hints.icon_pixmap);
	    }
	    wmPtr->hints.flags &= ~IconPixmapHint;
	} else {
	    pixmap = Tk_GetBitmap(interp, tkwin, Tk_GetUid(argv[3]));
	    if (pixmap == None) {
		return TCL_ERROR;
	    }
	    wmPtr->hints.icon_pixmap = pixmap;
	    wmPtr->hints.flags |= IconPixmapHint;
	}
	UpdateHints(winPtr);
    } else if ((c == 'i') && (strncmp(argv[1], "iconify", length) == 0)
	    && (length >= 5)) {
	if (argc != 3) {
	    Tcl_AppendResult(interp, "wrong # arguments: must be \"",
		    argv[0], " iconify window\"", (char *) NULL);
	    return TCL_ERROR;
	}
	wmPtr->hints.initial_state = IconicState;
	if (wmPtr->flags & WM_NEVER_MAPPED) {
	    return TCL_OK;
	}
#ifndef X11R3
	if (XIconifyWindow(winPtr->display, winPtr->window,
		winPtr->screenNum) == 0) {
	    interp->result =
		    "couldn't send iconify message to window manager";
	    return TCL_ERROR;
	}
#else
	interp->result = "can't iconify under X11R3";
	return TCL_ERROR;
#endif
    } else if ((c == 'i') && (strncmp(argv[1], "iconmask", length) == 0)
	    && (length >= 5)) {
	Pixmap pixmap;

	if ((argc != 3) && (argc != 4)) {
	    Tcl_AppendResult(interp, "wrong # arguments: must be \"",
		    argv[0], " iconmask window ?bitmap?\"",
		    (char *) NULL);
	    return TCL_ERROR;
	}
	if (argc == 3) {
	    if (wmPtr->hints.flags & IconMaskHint) {
		interp->result = Tk_NameOfBitmap(wmPtr->hints.icon_mask);
	    }
	    return TCL_OK;
	}
	if (*argv[3] == '\0') {
	    if (wmPtr->hints.icon_mask != None) {
		Tk_FreeBitmap(wmPtr->hints.icon_mask);
	    }
	    wmPtr->hints.flags &= ~IconMaskHint;
	} else {
	    pixmap = Tk_GetBitmap(interp, tkwin, Tk_GetUid(argv[3]));
	    if (pixmap == None) {
		return TCL_ERROR;
	    }
	    wmPtr->hints.icon_mask = pixmap;
	    wmPtr->hints.flags |= IconMaskHint;
	}
	UpdateHints(winPtr);
    } else if ((c == 'i') && (strncmp(argv[1], "iconname", length) == 0)
	    && (length >= 5)) {
	if (argc > 4) {
	    Tcl_AppendResult(interp, "wrong # arguments: must be \"",
		    argv[0], " iconname window ?newName?\"", (char *) NULL);
	    return TCL_ERROR;
	}
	if (argc == 3) {
	    interp->result = (wmPtr->iconName != NULL) ? wmPtr->iconName : "";
	    return TCL_OK;
	} else {
	    wmPtr->iconName = Tk_GetUid(argv[3]);
	    if (!(wmPtr->flags & WM_NEVER_MAPPED)) {
		XSetIconName(winPtr->display, winPtr->window, wmPtr->iconName);
	    }
	}
    } else if ((c == 'i') && (strncmp(argv[1], "iconposition", length) == 0)
	    && (length >= 5)) {
	int x, y;

	if ((argc != 3) && (argc != 5)) {
	    Tcl_AppendResult(interp, "wrong # arguments: must be \"",
		    argv[0], " iconposition window ?x y?\"",
		    (char *) NULL);
	    return TCL_ERROR;
	}
	if (argc == 3) {
	    if (wmPtr->hints.flags & IconPositionHint) {
		sprintf(interp->result, "%d %d", wmPtr->hints.icon_x,
			wmPtr->hints.icon_y);
	    }
	    return TCL_OK;
	}
	if (*argv[3] == '\0') {
	    wmPtr->hints.flags &= ~IconPositionHint;
	} else {
	    if ((Tcl_GetInt(interp, argv[3], &x) != TCL_OK)
		    || (Tcl_GetInt(interp, argv[4], &y) != TCL_OK)){
		return TCL_ERROR;
	    }
	    wmPtr->hints.icon_x = x;
	    wmPtr->hints.icon_y = y;
	    wmPtr->hints.flags |= IconPositionHint;
	}
	UpdateHints(winPtr);
    } else if ((c == 'i') && (strncmp(argv[1], "iconwindow", length) == 0)
	    && (length >= 5)) {
	Tk_Window tkwin2;

	if ((argc != 3) && (argc != 4)) {
	    Tcl_AppendResult(interp, "wrong # arguments: must be \"",
		    argv[0], " iconwindow window ?pathName?\"",
		    (char *) NULL);
	    return TCL_ERROR;
	}
	if (argc == 3) {
	    if (wmPtr->hints.flags & IconWindowHint) {
		interp->result = wmPtr->iconWindowName;
	    }
	    return TCL_OK;
	}
	if (*argv[3] == '\0') {
	    wmPtr->hints.flags &= ~IconWindowHint;
	    wmPtr->iconWindowName = NULL;
	} else {
	    tkwin2 = Tk_NameToWindow(interp, argv[3], tkwin);
	    if (tkwin2 == NULL) {
		return TCL_ERROR;
	    }
	    Tk_MakeWindowExist(tkwin2);
	    wmPtr->hints.icon_window = Tk_WindowId(tkwin2);
	    wmPtr->hints.flags |= IconWindowHint;
	    wmPtr->iconWindowName = Tk_PathName(tkwin2);
	}
	UpdateHints(winPtr);
    } else if ((c == 'm') && (strncmp(argv[1], "maxsize", length) == 0)
	    && (length >= 2)) {
	int width, height;
	if ((argc != 3) && (argc != 5)) {
	    Tcl_AppendResult(interp, "wrong # arguments: must be \"",
		    argv[0], " maxsize window ?width height?\"", (char *) NULL);
	    return TCL_ERROR;
	}
	if (argc == 3) {
	    if (wmPtr->sizeHintsFlags & PMaxSize) {
		sprintf(interp->result, "%d %d", wmPtr->maxWidth,
			wmPtr->maxHeight);
	    }
	    return TCL_OK;
	}
	if (*argv[3] == '\0') {
	    wmPtr->sizeHintsFlags &= ~PMaxSize;
	} else {
	    if ((Tcl_GetInt(interp, argv[3], &width) != TCL_OK)
		    || (Tcl_GetInt(interp, argv[4], &height) != TCL_OK)) {
		return TCL_ERROR;
	    }
	    wmPtr->maxWidth = width;
	    wmPtr->maxHeight = height;
	    wmPtr->sizeHintsFlags |= PMaxSize;
	}
	wmPtr->flags |= WM_UPDATE_SIZE_HINTS;
	goto updateGeom;
    } else if ((c == 'm') && (strncmp(argv[1], "minsize", length) == 0)
	    && (length >= 2)) {
	int width, height;
	if ((argc != 3) && (argc != 5)) {
	    Tcl_AppendResult(interp, "wrong # arguments: must be \"",
		    argv[0], " minsize window ?width height?\"", (char *) NULL);
	    return TCL_ERROR;
	}
	if (argc == 3) {
	    if (wmPtr->sizeHintsFlags & PMinSize) {
		sprintf(interp->result, "%d %d", wmPtr->minWidth,
			wmPtr->minHeight);
	    }
	    return TCL_OK;
	}
	if (*argv[3] == '\0') {
	    wmPtr->sizeHintsFlags &= ~PMinSize;
	} else {
	    if ((Tcl_GetInt(interp, argv[3], &width) != TCL_OK)
		    || (Tcl_GetInt(interp, argv[4], &height) != TCL_OK)) {
		return TCL_ERROR;
	    }
	    wmPtr->minWidth = width;
	    wmPtr->minHeight = height;
	    wmPtr->sizeHintsFlags |= PMinSize;
	}
	wmPtr->flags |= WM_UPDATE_SIZE_HINTS;
	goto updateGeom;
    } else if ((c == 'p') && (strncmp(argv[1], "positionfrom", length) == 0)) {
	if ((argc != 3) && (argc != 4)) {
	    Tcl_AppendResult(interp, "wrong # arguments: must be \"",
		    argv[0], " positionfrom window ?user/program?\"",
		    (char *) NULL);
	    return TCL_ERROR;
	}
	if (argc == 3) {
	    if (wmPtr->sizeHintsFlags & USPosition) {
		interp->result = "user";
	    } else if (wmPtr->sizeHintsFlags & PPosition) {
		interp->result = "program";
	    }
	    return TCL_OK;
	}
	if (*argv[3] == '\0') {
	    wmPtr->sizeHintsFlags &= ~(USPosition|PPosition);
	} else {
	    c = argv[3][0];
	    length = strlen(argv[3]);
	    if ((c == 'u') && (strncmp(argv[3], "user", length) == 0)) {
		wmPtr->sizeHintsFlags &= ~PPosition;
		wmPtr->sizeHintsFlags |= USPosition;
	    } else if ((c == 'p') && (strncmp(argv[3], "program", length) == 0)) {
		wmPtr->sizeHintsFlags &= ~USPosition;
		wmPtr->sizeHintsFlags |= PPosition;
	    } else {
		Tcl_AppendResult(interp, "bad argument \"", argv[3],
			"\": must be program or user", (char *) NULL);
		return TCL_ERROR;
	    }
	}
	wmPtr->flags |= WM_UPDATE_SIZE_HINTS;
	goto updateGeom;
    } else if ((c == 'r') && (strncmp(argv[1], "raise", length) == 0)) {
	if (argc != 3) {
	    Tcl_AppendResult(interp, "wrong # arguments: must be \"",
		    argv[0], " raise window\"", (char *) NULL);
	    return TCL_ERROR;
	}
	Tk_MakeWindowExist((Tk_Window) winPtr);
	XRaiseWindow(Tk_Display(winPtr), Tk_WindowId(winPtr));
    } else if ((c == 's') && (strncmp(argv[1], "sizefrom", length) == 0)) {
	if ((argc != 3) && (argc != 4)) {
	    Tcl_AppendResult(interp, "wrong # arguments: must be \"",
		    argv[0], " sizefrom window ?user|program?\"",
		    (char *) NULL);
	    return TCL_ERROR;
	}
	if (argc == 3) {
	    if (wmPtr->sizeHintsFlags & USSize) {
		interp->result = "user";
	    } else if (wmPtr->sizeHintsFlags & PSize) {
		interp->result = "program";
	    }
	    return TCL_OK;
	}
	if (*argv[3] == '\0') {
	    wmPtr->sizeHintsFlags &= ~(USSize|PSize);
	} else {
	    c = argv[3][0];
	    length = strlen(argv[3]);
	    if ((c == 'u') && (strncmp(argv[3], "user", length) == 0)) {
		wmPtr->sizeHintsFlags &= ~PSize;
		wmPtr->sizeHintsFlags |= USSize;
	    } else if ((c == 'p')
		    && (strncmp(argv[3], "program", length) == 0)) {
		wmPtr->sizeHintsFlags &= ~USSize;
		wmPtr->sizeHintsFlags |= PSize;
	    } else {
		Tcl_AppendResult(interp, "bad argument \"", argv[3],
			"\": must be program or user", (char *) NULL);
		return TCL_ERROR;
	    }
	}
	wmPtr->flags |= WM_UPDATE_SIZE_HINTS;
	goto updateGeom;
    } else if ((c == 't') && (strncmp(argv[1], "title", length) == 0)
	    && (length >= 2)) {
	if (argc > 4) {
	    Tcl_AppendResult(interp, "wrong # arguments: must be \"",
		    argv[0], " title window ?newTitle?\"", (char *) NULL);
	    return TCL_ERROR;
	}
	if (argc == 3) {
	    interp->result = (wmPtr->titleUid != NULL) ? wmPtr->titleUid
		    : winPtr->nameUid;
	    return TCL_OK;
	} else {
	    wmPtr->titleUid = Tk_GetUid(argv[3]);
#ifndef X11R3
	    if (!(wmPtr->flags & WM_NEVER_MAPPED)) {
		XTextProperty textProp;

		if (XStringListToTextProperty(&wmPtr->titleUid, 1,
			&textProp)  != 0) {
		    XSetWMName(winPtr->display, winPtr->window, &textProp);
		    XFree((char *) textProp.value);
		}
	    }
#endif
	}
#ifndef X11R3
    } else if ((c == 't') && (strncmp(argv[1], "transient", length) == 0)
	    && (length >= 2)) {
	Tk_Window master;

	if ((argc != 3) && (argc != 4)) {
	    Tcl_AppendResult(interp, "wrong # arguments: must be \"",
		    argv[0], " transient window ?master?\"", (char *) NULL);
	    return TCL_ERROR;
	}
	if (argc == 3) {
	    if (wmPtr->master != None) {
		interp->result = wmPtr->masterWindowName;
	    }
	    return TCL_OK;
	}
	if (argv[3][0] == '\0') {
	    wmPtr->master = None;
	    wmPtr->masterWindowName = NULL;
	} else {
	    master = Tk_NameToWindow(interp, argv[3], tkwin);
	    if (master == NULL) {
		return TCL_ERROR;
	    }
	    Tk_MakeWindowExist(master);
	    wmPtr->master = Tk_WindowId(master);
	    wmPtr->masterWindowName = Tk_PathName(master);
	}
	if (!(wmPtr->flags & WM_NEVER_MAPPED)) {
	    XSetTransientForHint(winPtr->display, winPtr->window,
		    wmPtr->master);
	}
    } else if ((c == 'w') && (strncmp(argv[1], "withdraw", length) == 0)) {
	if (argc != 3) {
	    Tcl_AppendResult(interp, "wrong # arguments: must be \"",
		    argv[0], " withdraw window\"", (char *) NULL);
	    return TCL_ERROR;
	}
	wmPtr->hints.initial_state = WithdrawnState;
	if (wmPtr->flags & WM_NEVER_MAPPED) {
	    return TCL_OK;
	}
	if (XWithdrawWindow(winPtr->display, winPtr->window,
		winPtr->screenNum) == 0) {
	    interp->result =
		    "couldn't send withdraw message to window manager";
	    return TCL_ERROR;
	}
	winPtr->flags &= ~TK_MAPPED;
    } else if ((c == 'p') && (strncmp(argv[1], "protocol", length) == 0)) {
	/*
         * handle various ICCCM WM_PROTOCOL attributes
         */
        if (argc < 4) {
            Tcl_AppendResult(interp, "wrong # arguments: must be \"",
                    argv[0], " protocol window type..\"", (char *) NULL);
            return TCL_ERROR;
        }
        if (!strcmp(argv[3], "delete")) {
	    return WmProtocolCmd(interp, &(wmPtr->deleteCmd), argc, argv);
	} else {
	    Tcl_AppendResult(interp,  argv[0], 
		": bad argument ", argv[3], " must be: ", 
		"delete", (char *) NULL);
            return TCL_ERROR;
	}
#endif
    } else {
	Tcl_AppendResult(interp, "unknown or ambiguous option \"", argv[1],
		"\": must be aspect, deiconify, focusmodel, ",
		"fullscreen, geometry, grid, group, iconbitmap, ",
		"iconify, iconmask, iconname, iconposition, ",
		"iconwindow, maxsize, minsize, positionfrom, raise, ",
		"sizefrom,  title, transient, withdraw, or protocol",
		(char *) NULL);
	return TCL_ERROR;
    }
    return TCL_OK;

    updateGeom:
    if (!(wmPtr->flags & (WM_UPDATE_PENDING|WM_NEVER_MAPPED))) {
	Tk_DoWhenIdle(UpdateGeometryInfo, (ClientData) winPtr);
	wmPtr->flags |= WM_UPDATE_PENDING;
    }
    return TCL_OK;
}

/*
 *----------------------------------------------------------------------
 *
 * Tk_SetGrid --
 *
 *	This procedure is invoked by a widget when it wishes to set a grid
 *	coordinate system that controls the size of a top-level window.
 *	It provides a C interface equivalent to the "wm grid" command and
 *	is usually asscoiated with the -setgrid option.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Grid-related information will be passed to the window manager, so
 *	that the top-level window associated with tkwin will resize on
 *	even grid units.
 *
 *----------------------------------------------------------------------
 */

void
Tk_SetGrid(tkwin, reqWidth, reqHeight, widthInc, heightInc)
    Tk_Window tkwin;		/* Token for window.  New window mgr info
				 * will be posted for the top-level window
				 * associated with this window. */
    int reqWidth;		/* Width (in grid units) corresponding to
				 * the requested geometry for tkwin. */
    int reqHeight;		/* Height (in grid units) corresponding to
				 * the requested geometry for tkwin. */
    int widthInc, heightInc;	/* Pixel increments corresponding to a
				 * change of one grid unit. */
{
    TkWindow *winPtr = (TkWindow *) tkwin;
    register WmInfo *wmPtr;

    /*
     * Find the top-level window for tkwin, plus the window manager
     * information.
     */

    while (!(winPtr->flags & TK_TOP_LEVEL)) {
	winPtr = winPtr->parentPtr;
    }
    wmPtr = winPtr->wmInfoPtr;

    if ((wmPtr->reqGridWidth == reqWidth)
	    && (wmPtr->reqGridHeight != reqHeight)
	    && (wmPtr->widthInc != widthInc)
	    && (wmPtr->heightInc != heightInc)
	    && ((wmPtr->sizeHintsFlags & (PBaseSize|PResizeInc))
		    == PBaseSize|PResizeInc)) {
	return;
    }

    /*
     * If gridding was previously off, then forget about any window
     * size requests made by the user or via "wm geometry":  these are
     * in pixel units and there's no easy way to translate them to
     * grid units since the new requested size of the top-level window in
     * pixels may not yet have been registered yet (it may filter up
     * the hierarchy in DoWhenIdle handlers).
     */

    if (!(wmPtr->sizeHintsFlags & PBaseSize)) {
	wmPtr->width = -1;
	wmPtr->height = -1;
    }

    /* 
     * Set the new gridding information, and start the process of passing
     * all of this information to the window manager.
     */

    wmPtr->reqGridWidth = reqWidth;
    wmPtr->reqGridHeight = reqHeight;
    wmPtr->widthInc = widthInc;
    wmPtr->heightInc = heightInc;
    wmPtr->sizeHintsFlags |= PBaseSize|PResizeInc;
    wmPtr->flags |= WM_UPDATE_SIZE_HINTS;
    if (!(wmPtr->flags & (WM_UPDATE_PENDING|WM_NEVER_MAPPED))) {
	Tk_DoWhenIdle(UpdateGeometryInfo, (ClientData) winPtr);
	wmPtr->flags |= WM_UPDATE_PENDING;
    }
}

/*
 *----------------------------------------------------------------------
 *
 * TopLevelEventProc --
 *
 *	This procedure is invoked when a top-level (or other externally-
 *	managed window) is restructured in any way.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Tk's internal data structures for the window get modified to
 *	reflect the structural change.
 *
 *----------------------------------------------------------------------
 */

static void
TopLevelEventProc(clientData, eventPtr)
    ClientData clientData;		/* Window for which event occurred. */
    XEvent *eventPtr;			/* Event that just happened. */
{
    register TkWindow *winPtr = (TkWindow *) clientData;

    if (eventPtr->type == DestroyNotify) {
	if (!(winPtr->flags & TK_ALREADY_DEAD)) {
	    Tk_DestroyWindow((Tk_Window) winPtr);
	}
    } else if (eventPtr->type == ConfigureNotify) {
	register WmInfo *wmPtr = winPtr->wmInfoPtr;
	int diff, x, y;

	/*
	 * A top-level window has been reconfigured.  Problem #1:
	 * discard stale information.  If the application has recently
	 * tried to reconfigure itself, ignore all events until the
	 * response to that reconfiguration arrives (the response is
	 * assumed to be the first ConfigureNotify that arrives after
	 * the server has seen the request;  this suffers from potential
	 * races with user actions, but it's the best I can think of
	 * right now).
	 */

	diff = eventPtr->xconfigure.serial - wmPtr->configRequest;
	if (diff < 0) {
	    return;
	}

	/*
	 * Problem #2: reparenting window managers.  If the window
	 * manager reparents a top-level window then the x and y
	 * information that comes in events for the window is wrong:
	 * it gives the location of the window inside its decorative
	 * parent, rather than the location of the window in root
	 * coordinates, which is what we want.  Window managers
	 * are supposed to send synthetic events with the correct
	 * information, but ICCCM doesn't require them to do this
	 * under all conditions, and the information provided doesn't
	 * include everything we need here.  So, the code below
	 * maintains a bunch of information about the parent window.
	 * If the window hasn't been reparented, we pretend that
	 * there is a parent shrink-wrapped around the window.
	 */

	if (wmPtr->reparent == None) {
	    noReparent:
	    winPtr->changes.x = eventPtr->xconfigure.x;
	    winPtr->changes.y = eventPtr->xconfigure.y;
	    wmPtr->parentWidth = eventPtr->xconfigure.width
		    + 2*eventPtr->xconfigure.border_width;
	    wmPtr->parentHeight = eventPtr->xconfigure.height
		    + 2*eventPtr->xconfigure.border_width;
	} else {
	    unsigned int width, height, bd, dummy;
	    Window dummy2;
	    Status status;
	    Tk_ErrorHandler handler;

	    handler = Tk_CreateErrorHandler(winPtr->display, BadDrawable, -1,
		    -1, (Tk_ErrorProc *) NULL, (ClientData) NULL);
	    status = XGetGeometry(winPtr->display, wmPtr->reparent,
		    &dummy2, &x, &y, &width, &height, &bd, &dummy);
	    Tk_DeleteErrorHandler(handler);
	    if (status == 0) {
		/*
		 * It appears that the reparented parent went away and
		 * no-one told us.  Reset the window to indicate that
		 * it's not reparented, then handle it as a non-reparented
		 * window.
		 */
		wmPtr->reparent = None;
		wmPtr->flags &= ~WM_NESTED_REPARENT;
		wmPtr->xInParent = wmPtr->yInParent = 0;
		goto noReparent;
	    }
	    wmPtr->parentWidth = width + 2*bd;
	    wmPtr->parentHeight = height + 2*bd;
	    winPtr->changes.x = x;
	    winPtr->changes.y = y;
	    if (wmPtr->flags & WM_NESTED_REPARENT) {
		int xOffset, yOffset;

		(void) XTranslateCoordinates(winPtr->display, winPtr->window,
		    wmPtr->reparent, 0, 0, &xOffset, &yOffset, &dummy2);
		wmPtr->xInParent = xOffset + bd - winPtr->changes.border_width;
		wmPtr->yInParent = yOffset + bd - winPtr->changes.border_width;
	    } else {
		if (!eventPtr->xconfigure.send_event) {
		    wmPtr->xInParent = eventPtr->xconfigure.x + bd;
		    wmPtr->yInParent = eventPtr->xconfigure.y + bd;
		}
	    }
	    winPtr->changes.x = x + wmPtr->xInParent;
	    winPtr->changes.y = y + wmPtr->yInParent;
	}

	/*
	 * Problem #3: if the window size or location was changed
	 * externally, update the geometry information in wmPtr to make
	 * it look just as if the user had typed a "wm geometry" command
	 * to make the change.  There are many tricky situations to deal
	 * with:
	 * (a) the event is simply a reflection of an internal geometry
	 *     request from the window's widgets (must leave width and
	 *     height alone in this case).
	 * (b) the window manager might respond to a size request from
	 *     us with a different size than requested (e.g. it might
	 *     have a minimum allowable window size).  Because of this,
	 *     can't just compare new size with requested size to determine
	 *     whether this event is a reflection of an internal request
	 *     from within the application.  Use WM_CONFIG_PENDING flag
	 *     instead.
	 * (c) ConfigureNotify events also arise if the window has been
	 *     moved, even if its size hasn't changed.  Must distinguish
	 *     between the user moving the window and the user resizing
	 *     the window.
	 */

	if (wmPtr->flags & WM_CONFIG_PENDING) {
	    int diff;
	    /*
	     * Size change is just a reflection of something coming from
	     * application.
	     */

	    diff = eventPtr->xconfigure.serial - wmPtr->configRequest;
	    if (diff >= 0) {
		if (wmPtr->flags & WM_CONFIG_AGAIN) {
		    if (!(wmPtr->flags & WM_UPDATE_PENDING)) {
			Tk_DoWhenIdle(UpdateGeometryInfo, (ClientData) winPtr);
			wmPtr->flags |= WM_UPDATE_PENDING;
		    }
		}
		wmPtr->flags &= ~(WM_CONFIG_PENDING|WM_CONFIG_AGAIN);
	    }
	} else if ((winPtr->changes.width != eventPtr->xconfigure.width)
		|| (winPtr->changes.height != eventPtr->xconfigure.height)) {
	    wmPtr->configWidth = -1;
	    wmPtr->configHeight = -1;
	    if (IS_GRIDDED(wmPtr)) {
		wmPtr->width = wmPtr->reqGridWidth
			+ (eventPtr->xconfigure.width
			- winPtr->reqWidth)/wmPtr->widthInc;
		if (wmPtr->width < 0) {
		    wmPtr->width = 0;
		}
		wmPtr->height = wmPtr->reqGridHeight
			+ (eventPtr->xconfigure.height
			- winPtr->reqHeight)/wmPtr->heightInc;
		if (wmPtr->height < 0) {
		    wmPtr->height = 0;
		}
	    } else if ((eventPtr->xconfigure.width != winPtr->changes.width)
		    || (eventPtr->xconfigure.height
			    != winPtr->changes.height)) {
		/*
		 * The check above is needed so we don't think the user
		 * requested a new size when all he/she did was to move
		 * the window.
		 */

		wmPtr->width = eventPtr->xconfigure.width;
		wmPtr->height = eventPtr->xconfigure.height;
	    }
	}

	winPtr->changes.width = eventPtr->xconfigure.width;
	winPtr->changes.height = eventPtr->xconfigure.height;
	winPtr->changes.border_width = eventPtr->xconfigure.border_width;
	winPtr->changes.sibling = eventPtr->xconfigure.above;
	winPtr->changes.stack_mode = Above;

	x = winPtr->changes.x - wmPtr->xInParent;
	if (wmPtr->flags & WM_NEGATIVE_X) {
	    x = DisplayWidth(winPtr->display, winPtr->screenNum)
		    - (x + wmPtr->parentWidth);
	}
	y = winPtr->changes.y - wmPtr->yInParent;
	if (wmPtr->flags & WM_NEGATIVE_Y) {
	    y = DisplayHeight(winPtr->display, winPtr->screenNum)
		    - (y + wmPtr->parentHeight);
	}
	if ((x != wmPtr->x) || (y != wmPtr->y)) {
	    wmPtr->x = x;
	    wmPtr->y = y;
	}
    } else if (eventPtr->type == MapNotify) {
	winPtr->flags |= TK_MAPPED;
    } else if (eventPtr->type == UnmapNotify) {
	winPtr->flags &= ~TK_MAPPED;
    } else if (eventPtr->type == ReparentNotify) {
	WmInfo *wmPtr = winPtr->wmInfoPtr;
	Window root, *children, dummy2, *virtualRootPtr;
	Atom virtualRootAtom, actualType;
	int actualFormat;
	unsigned long numItems, bytesAfter;
	unsigned int dummy;

	/*
	 * Locate the ancestor of this window that is just below the
	 * root window for the screen (could be the window itself).
	 * This code is a bit tricky because it allows for the
	 * possibility of a virtual root window, which is identified
	 * with a property named __SWM_VROOT.
	 */

	virtualRootAtom = Tk_InternAtom((Tk_Window) winPtr, "__SWM_VROOT");
	wmPtr->flags &= ~WM_NESTED_REPARENT;
	wmPtr->reparent = None;
	root = eventPtr->xreparent.parent;
	while (root != RootWindow(winPtr->display, winPtr->screenNum)) {
	    Tk_ErrorHandler handler1, handler2;
	    int status;

	    virtualRootPtr = NULL;

	    handler1 =
	      Tk_CreateErrorHandler(winPtr->display, BadDrawable,
				    -1, -1, (Tk_ErrorProc *) NULL,
				    (ClientData) NULL);
	    handler2 =
	      Tk_CreateErrorHandler(winPtr->display, BadWindow,
				    -1, -1, (Tk_ErrorProc *) NULL,
				    (ClientData) NULL);

	    status = XGetWindowProperty(winPtr->display, root,
					virtualRootAtom,
					0, (long) 1, False, XA_WINDOW,
					&actualType, &actualFormat,
					&numItems, &bytesAfter,
					(unsigned char **) &virtualRootPtr);

	    Tk_DeleteErrorHandler(handler1);
	    Tk_DeleteErrorHandler(handler2);

	    if (status == Success) {
		if (virtualRootPtr != NULL) {
		    if (*virtualRootPtr != root) {
			panic("TopLevelEventProc confused over virtual root");
		    }
		    XFree((char *) virtualRootPtr);
		    break;
		}
	    }
	    wmPtr->reparent = root;
	    (void) XQueryTree(winPtr->display, root, &dummy2, &root,
		    &children, &dummy);
	    XFree((char *) children);
	}

	/*
	 * The ancestor just below the (virtual) root is in wmPtr->reparent
	 * now, and the (virtual) root is in root.
	 */


	if (eventPtr->xreparent.parent == root) {
	    wmPtr->reparent = None;
	    wmPtr->flags &= ~WM_NESTED_REPARENT;
	    wmPtr->parentWidth = winPtr->changes.width
		    + 2*winPtr->changes.border_width;
	    wmPtr->parentHeight = winPtr->changes.height
		    + 2*winPtr->changes.border_width;
	    wmPtr->xInParent = wmPtr->yInParent = 0;
	    winPtr->changes.x = eventPtr->xreparent.x;
	    winPtr->changes.y = eventPtr->xreparent.y;
	} else {
	    int x, y, xOffset, yOffset;
	    unsigned int width, height, bd;

	    if (wmPtr->reparent != eventPtr->xreparent.parent) {
		wmPtr->flags |= WM_NESTED_REPARENT;
	    } else {
		wmPtr->flags &= ~WM_NESTED_REPARENT;
	    }

	    /*
	     * Compute and save information about reparent and about
	     * the window's position in reparent.
	     */

	    (void) XGetGeometry(winPtr->display, wmPtr->reparent,
		    &dummy2, &x, &y, &width, &height, &bd, &dummy);
	    wmPtr->parentWidth = width + 2*bd;
	    wmPtr->parentHeight = height + 2*bd;
	    (void) XTranslateCoordinates(winPtr->display, winPtr->window,
		    wmPtr->reparent, 0, 0, &xOffset, &yOffset, &dummy2);
	    wmPtr->xInParent = xOffset + bd - winPtr->changes.border_width;
	    wmPtr->yInParent = yOffset + bd - winPtr->changes.border_width;
	    winPtr->changes.x = x + xOffset;
	    winPtr->changes.y = y + yOffset;
	}
    } else if ((eventPtr->type == EnterNotify)
	    || (eventPtr->type == LeaveNotify)) {
	TkFocusEventProc(winPtr, eventPtr);
    }
}

/*
 *----------------------------------------------------------------------
 *
 * TopLevelReqProc --
 *
 *	This procedure is invoked by the geometry manager whenever
 *	the requested size for a top-level window is changed.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Arrange for the window to be resized to satisfy the request
 *	(this happens as a when-idle action).
 *
 *----------------------------------------------------------------------
 */

	/* ARGSUSED */
static void
TopLevelReqProc(dummy, tkwin)
    ClientData dummy;			/* Not used. */
    Tk_Window tkwin;			/* Information about window. */
{
    TkWindow *winPtr = (TkWindow *) tkwin;
    WmInfo *wmPtr;

    wmPtr = winPtr->wmInfoPtr;
    if ((wmPtr->prevReqWidth == winPtr->reqWidth)
	    && (wmPtr->prevReqHeight == winPtr->reqHeight)) {
	return;
    }
    wmPtr->prevReqWidth = winPtr->reqWidth;
    wmPtr->prevReqHeight = winPtr->reqHeight;
    wmPtr->flags |= WM_UPDATE_SIZE_HINTS;
    if (!(wmPtr->flags & (WM_UPDATE_PENDING|WM_NEVER_MAPPED))) {
	Tk_DoWhenIdle(UpdateGeometryInfo, (ClientData) winPtr);
	wmPtr->flags |= WM_UPDATE_PENDING;
    }
}

/*
 *----------------------------------------------------------------------
 *
 * UpdateGeometryInfo --
 *
 *	This procedure is invoked when a top-level window is first
 *	mapped, and also as a when-idle procedure, to bring the
 *	geometry and/or position of a top-level window back into
 *	line with what has been requested by the user and/or widgets.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The window's size and location may change, unless the WM prevents
 *	that from happening.
 *
 *----------------------------------------------------------------------
 */

static void
UpdateGeometryInfo(clientData)
    ClientData clientData;		/* Pointer to the window's record. */
{
    register TkWindow *winPtr = (TkWindow *) clientData;
    register WmInfo *wmPtr = winPtr->wmInfoPtr;
    int x, y, width, height;

    /*
     * It isn't safe to issue a new reconfigure request while there is
     * another reconfigure request outstanding.  If this happens, skip
     * the second reconfigure operation but set a flag so it will get
     * done with the first one finishes.
     */

    wmPtr->flags &= ~WM_UPDATE_PENDING;
    if (wmPtr->flags & WM_CONFIG_PENDING) {
	wmPtr->flags |= WM_CONFIG_AGAIN;
	return;
    }

    /*
     * Compute the new size for the top-level window.  See the
     * user documentation for details on this, but the size
     * requested depends on (a) the size requested internally
     * by the window's widgets, (b) the size requested by the
     * user in a "wm geometry" command or via wm-based interactive
     * resizing (if any), and (c) whether or not the window
     * gridded.  Don't permit sizes <= 0 because this upsets
     * the X server.
     */

    if (wmPtr->width == -1) {
	width = winPtr->reqWidth;
	height = winPtr->reqHeight;
    } else if (IS_GRIDDED(wmPtr)) {
	width = winPtr->reqWidth
		+ (wmPtr->width - wmPtr->reqGridWidth)*wmPtr->widthInc;
	height = winPtr->reqHeight
		+ (wmPtr->height - wmPtr->reqGridHeight)*wmPtr->heightInc;
    } else {
	width = wmPtr->width;
	height = wmPtr->height;
    }
    if (width <= 0) {
	width = 1;
    }
    if (height <= 0) {
	height = 1;
    }

    /*
     * Compute the new position for the window.  This is tricky, because
     * we need to include the border widths supplied by a reparented
     * parent in this calculation, but can't use the parent's current
     * overall size since that may change as a result of this code.
     */

    if (wmPtr->flags & WM_NEGATIVE_X) {
	x = DisplayWidth(winPtr->display, winPtr->screenNum) - wmPtr->x
		- (width + (wmPtr->parentWidth - winPtr->changes.width))
		+ wmPtr->xInParent;
    } else {
	x =  wmPtr->x + wmPtr->xInParent;
    }
    if (wmPtr->flags & WM_NEGATIVE_Y) {
	y = DisplayHeight(winPtr->display, winPtr->screenNum) - wmPtr->y
		- (height + (wmPtr->parentHeight - winPtr->changes.height))
		+ wmPtr->yInParent;
    } else {
	y =  wmPtr->y + wmPtr->yInParent;
    }

    /*
     * If the window's size is going to change and the window is
     * supposed to not be resizable by the user, then we have to
     * update the size hints.  There may also be a size-hint-update
     * request pending from somewhere else, too.
     */

    if (((width != winPtr->changes.width) || (width != winPtr->changes.width))
	    && !IS_GRIDDED(wmPtr)
	    && ((wmPtr->sizeHintsFlags & (PMinSize|PMaxSize)) == 0)) {
	wmPtr->flags |= WM_UPDATE_SIZE_HINTS;
    }
    if (wmPtr->flags & WM_UPDATE_SIZE_HINTS) {
	UpdateSizeHints(winPtr);
    }

    /*
     * If the geometry hasn't changed, be careful to  use only a
     * resize operation.  This is because of bugs in some window
     * managers (e.g. twm, as of 4/24/91) where they don't interpret
     * coordinates according to ICCCM.
     */

    if ((x != winPtr->changes.x) || (y != winPtr->changes.y)) {
	wmPtr->configRequest = XNextRequest(winPtr->display);
	wmPtr->configWidth = width;
	wmPtr->configHeight = height;
	Tk_MoveResizeWindow((Tk_Window) winPtr, x, y, (unsigned) width,
		(unsigned) height);
	wmPtr->flags |= WM_CONFIG_PENDING;
    } else if ((width != wmPtr->configWidth)
	    || (height != wmPtr->configHeight)) {
	wmPtr->configRequest = XNextRequest(winPtr->display);
	wmPtr->configWidth = width;
	wmPtr->configHeight = height;
	Tk_ResizeWindow((Tk_Window) winPtr, (unsigned) width,
		(unsigned) height);
	wmPtr->flags |= WM_CONFIG_PENDING;
    }
}

/*
 *--------------------------------------------------------------
 *
 * UpdateSizeHints --
 *
 *	This procedure is called to update the window manager's
 *	size hints information from the information in a WmInfo
 *	structure.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Properties get changed for winPtr.
 *
 *--------------------------------------------------------------
 */

static void
UpdateSizeHints(winPtr)
    TkWindow *winPtr;
{
    register WmInfo *wmPtr = winPtr->wmInfoPtr;
    XSizeHints *hintsPtr;

    wmPtr->flags &= ~WM_UPDATE_SIZE_HINTS;

#ifndef X11R3
    hintsPtr = XAllocSizeHints();
    if (hintsPtr == NULL) {
	return;
    }

    /*
     * Compute the pixel-based sizes for the various fields in the
     * size hints structure, based on the grid-based sizes in
     * our structure.
     */

    if (IS_GRIDDED(wmPtr)) {
	hintsPtr->base_width = winPtr->reqWidth
		- (wmPtr->reqGridWidth * wmPtr->widthInc);
	if (hintsPtr->base_width < 0) {
	    hintsPtr->base_width = 0;
	}
	hintsPtr->base_height = winPtr->reqHeight
		- (wmPtr->reqGridHeight * wmPtr->heightInc);
	if (hintsPtr->base_height < 0) {
	    hintsPtr->base_height = 0;
	}
	hintsPtr->min_width = hintsPtr->base_width
		+ (wmPtr->minWidth * wmPtr->widthInc);
	hintsPtr->min_height = hintsPtr->base_height
		+ (wmPtr->minHeight * wmPtr->heightInc);
	hintsPtr->max_width = hintsPtr->base_width
		+ (wmPtr->maxWidth * wmPtr->widthInc);
	hintsPtr->max_height = hintsPtr->base_height
		+ (wmPtr->maxHeight * wmPtr->heightInc);
    } else {
	hintsPtr->min_width = wmPtr->minWidth;
	hintsPtr->min_height = wmPtr->minHeight;
	hintsPtr->max_width = wmPtr->maxWidth;
	hintsPtr->max_height = wmPtr->maxHeight;
	hintsPtr->base_width = 0;
	hintsPtr->base_height = 0;
    }
    hintsPtr->width_inc = wmPtr->widthInc;
    hintsPtr->height_inc = wmPtr->heightInc;
    hintsPtr->min_aspect.x = wmPtr->minAspect.x;
    hintsPtr->min_aspect.y = wmPtr->minAspect.y;
    hintsPtr->max_aspect.x = wmPtr->maxAspect.x;
    hintsPtr->max_aspect.y = wmPtr->maxAspect.y;
    hintsPtr->win_gravity = wmPtr->gravity;
    hintsPtr->flags = wmPtr->sizeHintsFlags;

    /*
     * If a window is non-gridded and no minimum or maximum size has
     * been specified, don't let the window be resized at all.
     */

    if (!IS_GRIDDED(wmPtr)
	    && ((wmPtr->sizeHintsFlags & (PMinSize|PMaxSize)) == 0)) {
	int width, height;

	width = wmPtr->width;
	height = wmPtr->height;
	if (width < 0) {
	    width = winPtr->reqWidth;
	    height = winPtr->reqHeight;
	}
	hintsPtr->min_width = hintsPtr->max_width = width;
	hintsPtr->min_height = hintsPtr->max_height = height;
	hintsPtr->flags |= PMinSize|PMaxSize;
    }

    /*
     * If min or max size isn't specified, fill in with extreme values
     * rather than leaving unspecified.  Otherwise window manager may
     * do someting counter-intuitive like the last value ever specified.
     */

    if (!(hintsPtr->flags & PMinSize)) {
	hintsPtr->min_width = hintsPtr->min_height = 0;
	hintsPtr->flags |= PMinSize;
    }
    if (!(hintsPtr->flags & PMaxSize)) {
	hintsPtr->max_width = hintsPtr->max_height = 1000000;
	hintsPtr->flags |= PMaxSize;
    }

    XSetWMNormalHints(winPtr->display, winPtr->window, hintsPtr);

    XFree((char *) hintsPtr);
#endif /* X11R3 */
}

/*
 *--------------------------------------------------------------
 *
 * UpdateHints --
 *
 *	This procedure is called to update the window manager's
 *	hints information from the information in a WmInfo
 *	structure.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Properties get changed for winPtr.
 *
 *--------------------------------------------------------------
 */

static void
UpdateHints(winPtr)
    TkWindow *winPtr;
{
    WmInfo *wmPtr = winPtr->wmInfoPtr;

    if (wmPtr->flags & WM_NEVER_MAPPED) {
	return;
    }
    XSetWMHints(winPtr->display, winPtr->window, &wmPtr->hints);
}

/*
 *--------------------------------------------------------------
 *
 * ParseGeometry --
 *
 *	This procedure parses a geometry string and updates
 *	information used to control the geometry of a top-level
 *	window.
 *
 * Results:
 *	A standard Tcl return value, plus an error message in
 *	interp->result if an error occurs.
 *
 * Side effects:
 *	The size and/or location of winPtr may change.
 *
 *--------------------------------------------------------------
 */

static int
ParseGeometry(interp, string, winPtr)
    Tcl_Interp *interp;		/* Used for error reporting. */
    char *string;	/* String containing new geometry.  Has the
				 * standard form "=wxh+x+y". */
    TkWindow *winPtr;		/* Pointer to top-level window whose
				 * geometry is to be changed. */
{
    register WmInfo *wmPtr = winPtr->wmInfoPtr;
    int x, y, width, height, flags;
    char *end;
    register char *p = string;

    /*
     * The leading "=" is optional.
     */

    if (*p == '=') {
	p++;
    }

    /*
     * Parse the width and height, if they are present.  Don't
     * actually update any of the fields of wmPtr until we've
     * successfully parsed the entire geometry string.
     */

    width = wmPtr->width;
    height = wmPtr->height;
    x = wmPtr->x;
    y = wmPtr->y;
    flags = wmPtr->flags;
    if (isdigit(*p)) {
	width = strtoul(p, &end, 10);
	p = end;
	if (*p != 'x') {
	    goto error;
	}
	p++;
	if (!isdigit(*p)) {
	    goto error;
	}
	height = strtoul(p, &end, 10);
	p = end;
    }

    /*
     * Parse the X and Y coordinates, if they are present.
     */

    if (*p != '\0') {
	flags &= ~(WM_NEGATIVE_X | WM_NEGATIVE_Y);
	if (*p == '-') {
	    flags |= WM_NEGATIVE_X;
	} else if (*p != '+') {
	    goto error;
	}
	x = strtol(p+1, &end, 10);
	p = end;
	if (*p == '-') {
	    flags |= WM_NEGATIVE_Y;
	} else if (*p != '+') {
	    goto error;
	}
	y = strtol(p+1, &end, 10);
	if (*end != '\0') {
	    goto error;
	}

	/*
	 * Assume that the geometry information came from the user,
	 * unless an explicit source has been specified.  Otherwise
	 * most window managers assume that the size hints were
	 * program-specified and they ignore them.
	 */

	if ((wmPtr->sizeHintsFlags & (USPosition|PPosition)) == 0) {
	    wmPtr->sizeHintsFlags |= USPosition;
	    wmPtr->flags |= WM_UPDATE_SIZE_HINTS;
	}
    }

    /*
     * Everything was parsed OK.  Update the fields of *wmPtr and
     * arrange for the appropriate information to be percolated out
     * to the window manager at the next idle moment.
     */

    wmPtr->width = width;
    wmPtr->height = height;
    wmPtr->x = x;
    wmPtr->y = y;
    wmPtr->flags = flags;

    if (!(wmPtr->flags & (WM_UPDATE_PENDING|WM_NEVER_MAPPED))) {
	Tk_DoWhenIdle(UpdateGeometryInfo, (ClientData) winPtr);
	wmPtr->flags |= WM_UPDATE_PENDING;
    }
    return TCL_OK;

    error:
    Tcl_AppendResult(interp, "bad geometry specifier \"",
	    string, "\"", (char *) NULL);
    return TCL_ERROR;
}

/*
 *----------------------------------------------------------------------
 *
 * Tk_GetRootCoords --
 *
 *	Given a token for a window, this procedure traces through the
 *	window's lineage to find the root-window coordinates corresponding
 *	to point (0,0) in the window.
 *
 * Results:
 *	The locations pointed to by xPtr and yPtr are filled in with
 *	the root coordinates of the (0,0) point in tkwin.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

void
Tk_GetRootCoords(tkwin, xPtr, yPtr)
    Tk_Window tkwin;		/* Token for window. */
    int *xPtr;			/* Where to store x-displacement of (0,0). */
    int *yPtr;			/* Where to store y-displacement of (0,0). */
{
    int x, y;
    register TkWindow *winPtr = (TkWindow *) tkwin;

    /*
     * Search back through this window's parents all the way to a
     * top-level window, combining the offsets of each window within
     * its parent.
     */

    x = y = 0;
    while (1) {
	x += winPtr->changes.x + winPtr->changes.border_width;
	y += winPtr->changes.y + winPtr->changes.border_width;
	if (winPtr->flags & TK_TOP_LEVEL) {
	    break;
	}
	winPtr = winPtr->parentPtr;
    }
    *xPtr = x;
    *yPtr = y;
}



/*
 *--------------------------------------------------------------
 *
 * TkWmSetWmProtocols --
 *	Set the ICCCM WM_PROTOCOLS to be honored by this window.
 *	Currently, it is just WM_DELETE_WINDOW.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	A window property may get updated.
 *
 *--------------------------------------------------------------
 */

void
TkWmSetWmProtocols(winPtr)
    TkWindow *winPtr;		/* Newly-created top-level window. */
{
    if (winPtr->wmInfoPtr->flags & WM_NEVER_MAPPED) {
	return;
    }
#ifndef X11R3
    else {
	/* assemble the WM_PROTOCOLS that we honor */
	int count = 0;
	Atom atomlist[8];
	atomlist[count++] = Tk_InternAtom((Tk_Window) winPtr,
					  "WM_DELETE_WINDOW");
/* 
 * 	other WM_PROTOCOLS go here -- e.g...
 * 	atomlist[count++] = Tk_InternAtom((Tk_Window) winPtr,
 *					  "WM_SAVE_YOURSELF");
 */

	/* 
	 * assign the honor list to the window not all X11R4's have 
	 * XSetWmProtocols() so use XChangeProperty() 
	 */

	/* XSetWmProtocols(winPtr->display, winPtr->window, atomlist, count); */

	XChangeProperty(winPtr->display, 
			winPtr->window, 
			Tk_InternAtom((Tk_Window) winPtr, "WM_PROTOCOLS"),
			XA_ATOM, 32,
			PropModeReplace,
			(unsigned char *)atomlist, 
			count);

    }
#endif

    return;
}


/*
 *----------------------------------------------------------------------
 *
 * TkWmProtocolEventProc --
 *
 *	Handle a WM_PROTOCOL ICCCM event sent by the window manager to
 *	top level window.
 *
 *	The WM_PROTOCOL's currently handled are:
 *
 *		WM_DELETE_PROTOCOL:
 *
 * Results: None
 *
 * Side effects:
 *	for WM_DELETE_WINDOW:
 *		- window may be deleted if specified earlier by a 
 *		wm tcl command
 *		- a tcl command may be executed if sepcified earlier by a
 *		wm tcl command
 *	
 *
 */
void
TkWmProtocolEventProc(winPtr, eventPtr)
TkWindow *winPtr; 
XEvent *eventPtr;
{
    if ((Atom)(eventPtr->xclient.data.l)[0] ==
	Tk_InternAtom((Tk_Window) winPtr, "WM_DELETE_WINDOW")) {

	WmInfo *wmPtr = winPtr->wmInfoPtr;

	if (wmPtr->deleteCmd) {
	    if (*(wmPtr->deleteCmd) == '\0') {
		/* callback is empty, just delete the window */
		Tk_DestroyWindow((Tk_Window) winPtr);
	    } else {
		/* there is a callback so run it */
		(void) Tcl_Eval(winPtr->mainPtr->interp, 
				wmPtr->deleteCmd, 0, (char **)0);
	    }
	} else {
	    Tk_DestroyWindow((Tk_Window) winPtr);
	}
    }
    /*
     * else { .. other WM_<ETC> cases go here ... }
     */
    return;
}


/* 
 *----------------------------------------------------------------------
 *
 * WmProtocolCmd
 *
 * implements 
 *
 *	wm protocol <window> delete [command_str] 
 *
 * right now just delete is supported for OPTION
 *
 * Kind of artificial, But makes it easier to merge into new
 * versions of Stock Tk.
 */
int
WmProtocolCmd(interp, CmdPtr, argc, argv)
Tcl_Interp *interp;
char **CmdPtr;
int argc;
char **argv;
{
#define Cmd (*CmdPtr)

    switch(argc) {
    case 4:
	/* 
	 * return current command 
	 */
	if (!Cmd || *Cmd == '\0') {
	    return TCL_OK;
	} else {
	    /* 
	     * chop off the <blank><window_name>
	     * and return just the cmd 
	     */
	    int x = strlen(Cmd) - strlen(argv[2]) - 1;
	    char tmpc = Cmd[x];
	    Cmd[x] = '\0';
	    {
		/* maybe should just have them put the window in the cmd */
		Tcl_AppendResult(interp, Cmd, (char *)NULL);
	    }
	    /* 
	     * tack the blank and window name back on 
	     */
	    Cmd[x] = tmpc;
	    return TCL_OK;
	}
    case 5:
	/* 
	 * (re)set command 
	 */
	if (Cmd) {
	    ckfree(Cmd);
	    Cmd = (char *)NULL;
	}
	if (*argv[4] != '\0') {
	    int x = strlen(argv[4]) + strlen(argv[2]) + 2;
	    if (!(Cmd = ckalloc(x))) {
		perror("wm protocol:");
	    } else {
		sprintf(Cmd, "%s %s", argv[4], argv[2]);
	    }
	}
	return TCL_OK;
    default:
	Tcl_AppendResult(interp, "wrong # of arguments: must be \"",
	    argv[0], " protocol window <attribute> [cmd]\"", (char *) NULL);
	return TCL_ERROR;
    }

#undef Cmd
}


/*
 *----------------------------------------------------------------------
 *
 * Tk_CoordsToWindow --
 *
 *	Given the root coordinates of a point, this procedure
 *	returns the token for the top-most window covering that point,
 *	if there exists such a window in this application.
 *
 * Results:
 *	The return result is either a token for the window corresponding
 *	to rootX and rootY, or else NULL to indicate that there is no such
 *	window.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

Tk_Window
Tk_CoordsToWindow(rootX, rootY, tkwin)
    int rootX, rootY;		/* Coordinates of point in root window. */
    Tk_Window tkwin;		/* Token for any window in application;
				 * used to identify the application. */
{
    Window rootChild, dummy3, dummy4;
    int i, dummy1, dummy2;
    register WmInfo *wmPtr;
    register TkWindow *winPtr, *childPtr;
    TkWindow *nextPtr;		/* Coordinates of highest child found so
				 * far that contains point. */
    int x, y;			/* Coordinates in winPtr. */
    int tmpx, tmpy, bd;
    Window *children;		/* Children of winPtr, or NULL. */
    unsigned int numChildren;	/* Size of children array. */

    /*
     * Step 1:  find the top-level window that contains the desired
     * coordinates.
     */

    if (XTranslateCoordinates(Tk_Display(tkwin),
	    RootWindowOfScreen(Tk_Screen(tkwin)),
	    RootWindowOfScreen(Tk_Screen(tkwin)), rootX, rootY, &dummy1,
	    &dummy2, &rootChild) == False) {
	panic("Tk_CoordsToWindow get False return from XTranslateCoordinates");
    }
    for (wmPtr = firstWmPtr; ; wmPtr = wmPtr->nextPtr) {
	if (wmPtr == NULL) {
	    return NULL;
	}
	if ((wmPtr->reparent == rootChild) || ((wmPtr->reparent == None)
		&& (wmPtr->winPtr->window == rootChild))) {
	    break;
	}
    }
    winPtr = wmPtr->winPtr;
    if (winPtr->mainPtr != ((TkWindow *) tkwin)->mainPtr) {
	return NULL;
    }

    /*
     * Step 2: work down through the hierarchy underneath this window.
     * At each level, scan through all the children to see if any contain
     * the point.  If none do, then we're done.  If one does, then do the
     * same thing on that child.  If two or more do, then fetch enough
     * information from the window server to figure out which is on top,
     * and repeat on that child.
     */

    x = rootX;
    y = rootY;
    while (1) {
	x -= winPtr->changes.x;
	y -= winPtr->changes.y;
	nextPtr = NULL;
	children = NULL;
	for (childPtr = winPtr->childList; childPtr != NULL;
		childPtr = childPtr->nextPtr) {
	    if (!Tk_IsMapped(childPtr) || (childPtr->flags & TK_TOP_LEVEL)) {
		continue;
	    }
	    tmpx = x - childPtr->changes.x;
	    tmpy = y - childPtr->changes.y;
	    bd = childPtr->changes.border_width;
	    if ((tmpx < -bd) || (tmpy < -bd)
		    || (tmpx >= (childPtr->changes.width + bd))
		    || (tmpy >= (childPtr->changes.height + bd))) {
		continue;
	    }
	    if (nextPtr == NULL) {
		nextPtr = childPtr;
		continue;
	    }

	    /*
	     * More than one child of same parent overlaps point.  Must
	     * figure out which is on top.  Keep a cache of the stacking
	     * order for winPtr to help with this, in case there are >2
	     * children overlapping.
	     */

	    if (children == NULL) {
		if (XQueryTree(winPtr->display, winPtr->window, &dummy3,
			&dummy4, &children, &numChildren) == 0) {
		    panic("Tk_CoordsToWindow get error return from XQueryTree");
		}
	    }
	    for (i = 0; i < numChildren; i++) {
		if (children[i] == childPtr->window) {
		    break;
		}
		if (children[i] == nextPtr->window) {
		    nextPtr = childPtr;
		    break;
		}
	    }
	}
	if (children != NULL) {
	    XFree((char *) children);
	}
	if (nextPtr == NULL) {
	    break;
	}
	winPtr = nextPtr;
    }
    return (Tk_Window) winPtr;
}


