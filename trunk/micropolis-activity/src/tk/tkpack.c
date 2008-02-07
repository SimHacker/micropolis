/* 
 * tkPack.c --
 *
 *	This file contains code to implement the "packer"
 *	geometry manager for Tk.
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
static char rcsid[] = "$Header: /user6/ouster/wish/RCS/tkPack.c,v 1.27 92/01/04 15:16:41 ouster Exp $ SPRITE (Berkeley)";
#endif

#include "tkconfig.h"
#include "tkint.h"

typedef enum {TOP, BOTTOM, LEFT, RIGHT} Side;

/* For each window that the packer cares about (either because
 * the window is managed by the packer or because the window
 * has children that are managed by the packer), there is a
 * structure of the following type:
 */

typedef struct Packer {
    Tk_Window tkwin;		/* Tk token for window.  NULL means that
				 * the window has been deleted, but the
				 * packet hasn't had a chance to clean up
				 * yet because the structure is still in
				 * use. */
    struct Packer *parentPtr;	/* Parent within which this window
				 * is packed (NULL means this window
				 * isn't managed by the packer). */
    struct Packer *nextPtr;	/* Next window packed within same
				 * parent.  List is priority-ordered:
				 * first on list gets packed first. */
    struct Packer *childPtr;	/* First in list of children packed
				 * inside this window (NULL means
				 * no packed children). */
    Side side;			/* Side of parent against which
				 * this window is packed. */
    Tk_Anchor anchorPoint;	/* If frame allocated for window is larger
				 * than window needs, this indicates how
				 * where to position window in frame. */
    int padX, padY;		/* Additional amounts of space to give window
				 * besides what it asked for. */
    int doubleBw;		/* Twice the window's last known border
				 * width.  If this changes, the window
				 * must be repacked within its parent. */
    int *abortPtr;		/* If non-NULL, it means that there is a nested
				 * call to ArrangePacking already working on
				 * this window.  *abortPtr may be set to 1 to
				 * abort that nested call.  This happens, for
				 * example, if tkwin or any of its children
				 * is deleted. */
    int flags;			/* Miscellaneous flags;  see below
				 * for definitions. */
} Packer;

/*
 * Flag values for Packer structures:
 *
 * REQUESTED_REPACK:		1 means a Tk_DoWhenIdle request
 *				has already been made to repack
 *				all the children of this window.
 * FILLX:			1 means if frame allocated for window
 *				is wider than window needs, expand window
 *				to fill frame.  0 means don't make window
 *				any larger than needed.
 * FILLY:			Same as FILLX, except for height.
 * EXPAND:			1 means this window's frame will absorb any
 *				extra space in the parent window.
 */

#define REQUESTED_REPACK	1
#define FILLX			2
#define FILLY			4
#define EXPAND			8

/*
 * Hash table used to map from Tk_Window tokens to corresponding
 * Packer structures:
 */

static Tcl_HashTable packerHashTable;

/*
 * Have statics in this module been initialized?
 */

static initialized = 0;

/*
 * Forward declarations for procedures defined later in this file:
 */

static void		ArrangePacking _ANSI_ARGS_((ClientData clientData));
static Packer *		GetPacker _ANSI_ARGS_((Tk_Window tkwin));
static int		PackAfter _ANSI_ARGS_((Tcl_Interp *interp,
			    Packer *prevPtr, Packer *parentPtr, int argc,
			    char **argv));
static void		PackReqProc _ANSI_ARGS_((ClientData clientData,
			    Tk_Window tkwin));
static void		PackStructureProc _ANSI_ARGS_((ClientData clientData,
			    XEvent *eventPtr));
static void		Unlink _ANSI_ARGS_((Packer *packPtr));

/*
 *--------------------------------------------------------------
 *
 * Tk_PackCmd --
 *
 *	This procedure is invoked to process the "pack" Tcl command.
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
Tk_PackCmd(clientData, interp, argc, argv)
    ClientData clientData;	/* Main window associated with
				 * interpreter. */
    Tcl_Interp *interp;		/* Current interpreter. */
    int argc;			/* Number of arguments. */
    char **argv;		/* Argument strings. */
{
    Tk_Window tkwin = (Tk_Window) clientData;
    int length;
    char c;

    if (argc < 3) {
	Tcl_AppendResult(interp, "wrong # args: should be \"",
		argv[0], " option arg ?arg ...?\"", (char *) NULL);
	return TCL_ERROR;
    }
    c = argv[1][0];
    length = strlen(argv[1]);
    if ((c == 'a') && (length >= 2)
	    && (strncmp(argv[1], "after", length) == 0)) {
	Packer *prevPtr;
	Tk_Window tkwin2;

	tkwin2 = Tk_NameToWindow(interp, argv[2], tkwin);
	if (tkwin2 == NULL) {
	    return TCL_ERROR;
	}
	prevPtr = GetPacker(tkwin2);
	if (prevPtr->parentPtr == NULL) {
	    Tcl_AppendResult(interp, "window \"", argv[2],
		    "\" isn't packed", (char *) NULL);
	    return TCL_ERROR;
	}
	return PackAfter(interp, prevPtr, prevPtr->parentPtr, argc-3, argv+3);
    } else if ((c == 'a') && (length >= 2)
	    && (strncmp(argv[1], "append", length) == 0)) {
	Packer *parentPtr;
	register Packer *prevPtr;
	Tk_Window tkwin2;

	tkwin2 = Tk_NameToWindow(interp, argv[2], tkwin);
	if (tkwin2 == NULL) {
	    return TCL_ERROR;
	}
	parentPtr = GetPacker(tkwin2);
	prevPtr = parentPtr->childPtr;
	if (prevPtr != NULL) {
	    while (prevPtr->nextPtr != NULL) {
		prevPtr = prevPtr->nextPtr;
	    }
	}
	return PackAfter(interp, prevPtr, parentPtr, argc-3, argv+3);
    } else if ((c == 'b') && (strncmp(argv[1], "before", length) == 0)) {
	Packer *packPtr, *parentPtr;
	register Packer *prevPtr;
	Tk_Window tkwin2;

	tkwin2 = Tk_NameToWindow(interp, argv[2], tkwin);
	if (tkwin2 == NULL) {
	    return TCL_ERROR;
	}
	packPtr = GetPacker(tkwin2);
	if (packPtr->parentPtr == NULL) {
	    Tcl_AppendResult(interp, "window \"", argv[2],
		    "\" isn't packed", (char *) NULL);
	    return TCL_ERROR;
	}
	parentPtr = packPtr->parentPtr;
	prevPtr = parentPtr->childPtr;
	if (prevPtr == packPtr) {
	    prevPtr = NULL;
	} else {
	    for ( ; ; prevPtr = prevPtr->nextPtr) {
		if (prevPtr == NULL) {
		    panic("\"pack before\" couldn't find predecessor");
		}
		if (prevPtr->nextPtr == packPtr) {
		    break;
		}
	    }
	}
	return PackAfter(interp, prevPtr, parentPtr, argc-3, argv+3);
    } else if ((c == 'i') && (strncmp(argv[1], "info", length) == 0)) {
	char *prefix;
	register Packer *packPtr;
	Tk_Window tkwin2;
	char tmp[20];
	static char *sideNames[] = {"top", "bottom", "left", "right"};

	if (argc != 3) {
	    Tcl_AppendResult(interp, "wrong # args: should be \"",
		    argv[0], " info window\"", (char *) NULL);
	    return TCL_ERROR;
	}
	tkwin2 = Tk_NameToWindow(interp, argv[2], tkwin);
	if (tkwin2 == NULL) {
	    return TCL_ERROR;
	}
	packPtr = GetPacker(tkwin2);
	prefix = "";
	for (packPtr = packPtr->childPtr; packPtr != NULL;
		packPtr = packPtr->nextPtr) {
	    Tcl_AppendResult(interp, prefix, Tk_PathName(packPtr->tkwin),
		    " {", sideNames[(int) packPtr->side],
		    " frame ", Tk_NameOfAnchor(packPtr->anchorPoint),
		    (char *) NULL);
	    if (packPtr->padX != 0) {
		sprintf(tmp, "%d", packPtr->padX);
		Tcl_AppendResult(interp, " padx ", tmp, (char *) NULL);
	    }
	    if (packPtr->padY != 0) {
		sprintf(tmp, "%d", packPtr->padY);
		Tcl_AppendResult(interp, " pady ", tmp, (char *) NULL);
	    }
	    if (packPtr->flags & EXPAND) {
		Tcl_AppendResult(interp, " expand", (char *) NULL);
	    }
	    if ((packPtr->flags & (FILLX|FILLY)) == (FILLX|FILLY)) {
		Tcl_AppendResult(interp, " fill", (char *) NULL);
	    } else if (packPtr->flags & FILLX) {
		Tcl_AppendResult(interp, " fillx", (char *) NULL);
	    } else if (packPtr->flags & FILLY) {
		Tcl_AppendResult(interp, " filly", (char *) NULL);
	    }
	    Tcl_AppendResult(interp, "}", (char *) NULL);
	    prefix = " ";
	}
	return TCL_OK;
    } else if ((c == 'u') && (strncmp(argv[1], "unpack", length) == 0)) {
	Tk_Window tkwin2;
	Packer *packPtr;

	if (argc != 3) {
	    Tcl_AppendResult(interp, "wrong # args: should be \"",
		    argv[0], " unpack window\"", (char *) NULL);
	    return TCL_ERROR;
	}
	tkwin2 = Tk_NameToWindow(interp, argv[2], tkwin);
	if (tkwin2 == NULL) {
	    return TCL_ERROR;
	}
	packPtr = GetPacker(tkwin2);
	if ((packPtr != NULL) && (packPtr->parentPtr != NULL)) {
	    Tk_ManageGeometry(tkwin2, (Tk_GeometryProc *) NULL,
		    (ClientData) NULL);
	    Unlink(packPtr);
	    Tk_UnmapWindow(packPtr->tkwin);
	}
    } else {
	Tcl_AppendResult(interp, "bad option \"", argv[1],
		"\":  must be after, append, before, or info", (char *) NULL);
	return TCL_ERROR;
    }
    return TCL_OK;
}

/*
 *--------------------------------------------------------------
 *
 * PackReqProc --
 *
 *	This procedure is invoked by Tk_GeometryRequest for
 *	windows managed by the packer.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Arranges for tkwin, and all its managed siblings, to
 *	be re-packed at the next idle point.
 *
 *--------------------------------------------------------------
 */

	/* ARGSUSED */
static void
PackReqProc(clientData, tkwin)
    ClientData clientData;	/* Packer's information about
				 * window that got new preferred
				 * geometry.  */
    Tk_Window tkwin;		/* Other Tk-related information
				 * about the window. */
{
    register Packer *packPtr = (Packer *) clientData;

    packPtr = packPtr->parentPtr;
    if (!(packPtr->flags & REQUESTED_REPACK)) {
	packPtr->flags |= REQUESTED_REPACK;
	Tk_DoWhenIdle(ArrangePacking, (ClientData) packPtr);
    }
}

/*
 *--------------------------------------------------------------
 *
 * ArrangePacking --
 *
 *	This procedure is invoked (using the Tk_DoWhenIdle
 *	mechanism) to re-layout a set of windows managed by
 *	the packer.  It is invoked at idle time so that a
 *	series of packer requests can be merged into a single
 *	layout operation.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The packed children of parentPtr may get resized or
 *	moved.
 *
 *--------------------------------------------------------------
 */

static void
ArrangePacking(clientData)
    ClientData clientData;	/* Structure describing parent
				 * whose children are to be
				 * re-layed out. */
{
    register Packer *parentPtr = (Packer *) clientData;
    register Packer *childPtr;	
    int numExpX, numExpY;	/* # of windows that are expandable in
				 * each direction. */
    int spareX, spareY;		/* Amount of extra space to give to each
				 * expandable window. */
    int leftOverX, leftOverY;	/* Extra chunk of space to give to last
				 * expandable window. */
    int cavityX, cavityY, cavityWidth, cavityHeight;
				/* These variables keep track of the
				 * as-yet-unallocated space remaining in
				 * the middle of the parent window. */
    int frameX, frameY, frameWidth, frameHeight;
				/* These variables keep track of the frame
				 * allocated to the current window. */
    int x, y, width, height;	/* These variables are used to hold the
				 * actual geometry of the current window. */
    int intBWidth;		/* Width of internal border in parent window,
				 * if any. */
    int abort;			/* May get set to non-zero to abort this
				 * repacking operation. */
    int maxWidth, maxHeight, tmp;

    parentPtr->flags &= ~REQUESTED_REPACK;

    /*
     * If the parent has no children anymore, then don't do anything
     * at all:  just leave the parent's size as-is.
     */

    if (parentPtr->childPtr == NULL) {
	return;
    }

    /*
     * Abort any nested call to ArrangePacking for this window, since
     * we'll do everything necessary here, and set up so this call
     * can be aborted if necessary.  
     */

    if (parentPtr->abortPtr != NULL) {
	*parentPtr->abortPtr = 1;
    }
    parentPtr->abortPtr = &abort;
    abort = 0;
    Tk_Preserve((ClientData) parentPtr);

    /*
     * Pass #1: scan all the children to figure out the total amount
     * of space needed.  Two separate widths and heights are computed.
     *
     * "Width" and "height" compute the minimum parent size to meet
     * the needs of each window in the direction "where there is
     * flexibility".  For example, if a child is packed TOP, then
     * y is the flexible direction:  the child's requested height
     * will determine its size.  For this window x is the inflexible
     * direction:  the window's width will be determined by the amount
     * of space left in the parent's cavity, not by the window's
     * requested width.  "Width" and "height" are needed in order to
     * compute how much extra space there is, so that it can be divided
     * among the windows that have the EXPAND flag.
     *
     * "MaxWidth" and "maxHeight" compute the minimum parent size to
     * meet all the needs of every window in both directions, flexible
     * or inflexible.  These values are needed to make geometry requests
     * of the parent's parent.
     */

    intBWidth = Tk_InternalBorderWidth(parentPtr->tkwin);
    width = height = maxWidth = maxHeight = 2*intBWidth;
    numExpX = numExpY = 0;
    for (childPtr = parentPtr->childPtr; childPtr != NULL;
	    childPtr = childPtr->nextPtr) {
	if ((childPtr->side == TOP) || (childPtr->side == BOTTOM)) {
	    tmp = Tk_ReqWidth(childPtr->tkwin) + childPtr->doubleBw
		    + childPtr->padX + width;
	    if (tmp > maxWidth) {
		maxWidth = tmp;
	    }
	    height += Tk_ReqHeight(childPtr->tkwin) + childPtr->doubleBw
		    + childPtr->padY;
	    if (childPtr->flags & EXPAND) {
		numExpY++;
	    }
	} else {
	    tmp = Tk_ReqHeight(childPtr->tkwin) + childPtr->doubleBw
		    + childPtr->padY + height;
	    if (tmp > maxHeight) {
		maxHeight = tmp;
	    }
	    width += Tk_ReqWidth(childPtr->tkwin) + childPtr->doubleBw
		    + childPtr->padX;
	    if (childPtr->flags & EXPAND) {
		numExpX++;
	    }
	}
    }
    if (width > maxWidth) {
	maxWidth = width;
    }
    if (height > maxHeight) {
	maxHeight = height;
    }

    /*
     * If the total amount of space needed in the parent window has
     * changed, then notify the next geometry manager up and requeue
     * ourselves to start again after the parent has had a chance to
     * resize us.
     */

    if ((maxWidth != Tk_ReqWidth(parentPtr->tkwin))
	    || (maxHeight != Tk_ReqHeight(parentPtr->tkwin))) {
	Tk_GeometryRequest(parentPtr->tkwin, maxWidth, maxHeight);
	parentPtr->flags |= REQUESTED_REPACK;
	Tk_DoWhenIdle(ArrangePacking, (ClientData) parentPtr);
	goto done;
    }

    /*
     * If there is spare space, figure out how much of it goes to
     * each of the windows that is expandable.
     */

    spareX = Tk_Width(parentPtr->tkwin) - width;
    spareY = Tk_Height(parentPtr->tkwin) - height;
    if ((spareX <= 0) || (numExpX == 0)) {
	leftOverX = 0;
	spareX = 0;
    } else {
	leftOverX = spareX % numExpX;
	spareX /= numExpX;
    }
    if ((spareY <= 0) || (numExpY == 0)) {
	leftOverY = spareY;
	spareY = 0;
    } else {
	leftOverY = spareY % numExpY;
	spareY /= numExpY;
    }

    /*
     * Pass #2: scan the children a second time assigning
     * new sizes.  The "cavity" variables keep track of the
     * unclaimed space in the cavity of the window;  this
     * shrinks inward as we allocate windows around the
     * edges.  The "frame" variables keep track of the space
     * allocated to the current window and its frame.  The
     * current window is then placed somewhere inside the
     * frame, depending on anchorPoint.
     */

    cavityX = cavityY = x = y = intBWidth;
    cavityWidth = Tk_Width(parentPtr->tkwin) - 2*intBWidth;
    cavityHeight = Tk_Height(parentPtr->tkwin) - 2*intBWidth;
    for (childPtr = parentPtr->childPtr; childPtr != NULL;
	    childPtr = childPtr->nextPtr) {
	if ((childPtr->side == TOP) || (childPtr->side == BOTTOM)) {
	    frameWidth = cavityWidth;
	    frameHeight = Tk_ReqHeight(childPtr->tkwin) + childPtr->padY
		    + childPtr->doubleBw;
	    if (childPtr->flags & EXPAND) {
		frameHeight += spareY;
		numExpY--;
		if (numExpY == 0) {
		    frameHeight += leftOverY;
		}
	    }
	    cavityHeight -= frameHeight;
	    if (cavityHeight < 0) {
		frameHeight += cavityHeight;
		cavityHeight = 0;
	    }
	    frameX = cavityX;
	    if (childPtr->side == TOP) {
		frameY = cavityY;
		cavityY += frameHeight;
	    } else {
		frameY = cavityY + cavityHeight;
	    }
	} else {
	    frameHeight = cavityHeight;
	    frameWidth = Tk_ReqWidth(childPtr->tkwin) + childPtr->padX
		    + childPtr->doubleBw;
	    if (childPtr->flags & EXPAND) {
		frameWidth += spareX;
		numExpX--;
		if (numExpX == 0) {
		    frameWidth += leftOverX;
		}
	    }
	    cavityWidth -= frameWidth;
	    if (cavityWidth < 0) {
		frameWidth += cavityWidth;
		cavityWidth = 0;
	    }
	    frameY = cavityY;
	    if (childPtr->side == LEFT) {
		frameX = cavityX;
		cavityX += frameWidth;
	    } else {
		frameX = cavityX + cavityWidth;
	    }
	}

	/*
	 * Now that we've got the size of the frame for the window,
	 * compute the window's actual size and location using the
	 * fill and frame factors.
	 */

	width = Tk_ReqWidth(childPtr->tkwin) + childPtr->doubleBw;
	if ((childPtr->flags & FILLX) || (width > frameWidth)) {
	    width = frameWidth;
	}
	height = Tk_ReqHeight(childPtr->tkwin) + childPtr->doubleBw;
	if ((childPtr->flags & FILLY) || (height > frameHeight)) {
	    height = frameHeight;
	}
	switch (childPtr->anchorPoint) {
	    case TK_ANCHOR_N:
		x = frameX + (frameWidth - width)/2;
		y = frameY;
		break;
	    case TK_ANCHOR_NE:
		x = frameX + frameWidth - width;
		y = frameY;
		break;
	    case TK_ANCHOR_E:
		x = frameX + frameWidth - width;
		y = frameY + (frameHeight - height)/2;
		break;
	    case TK_ANCHOR_SE:
		x = frameX + frameWidth - width;
		y = frameY + frameHeight - height;
		break;
	    case TK_ANCHOR_S:
		x = frameX + (frameWidth - width)/2;
		y = frameY + frameHeight - height;
		break;
	    case TK_ANCHOR_SW:
		x = frameX;
		y = frameY + frameHeight - height;
		break;
	    case TK_ANCHOR_W:
		x = frameX;
		y = frameY + (frameHeight - height)/2;
		break;
	    case TK_ANCHOR_NW:
		x = frameX;
		y = frameY;
		break;
	    case TK_ANCHOR_CENTER:
		x = frameX + (frameWidth - width)/2;
		y = frameY + (frameHeight - height)/2;
		break;
	    default:
		panic("bad frame factor in ArrangePacking");
	}
	width -= childPtr->doubleBw;
	height -= childPtr->doubleBw;

	/*
	 * If the window is too small to be interesting then
	 * unmap it.  Otherwise configure it and then make sure
	 * it's mapped.
	 */

	if ((width <= 0) || (height <= 0)) {
	    Tk_UnmapWindow(childPtr->tkwin);
	} else {
	    if ((x != Tk_X(childPtr->tkwin))
		    || (y != Tk_Y(childPtr->tkwin))
		    || (width != Tk_Width(childPtr->tkwin))
		    || (height != Tk_Height(childPtr->tkwin))) {
		Tk_MoveResizeWindow(childPtr->tkwin, x, y,
			(unsigned int) width, (unsigned int) height);
	    }
	    if (abort) {
		goto done;
	    }
	    Tk_MapWindow(childPtr->tkwin);
	}

	/*
	 * Changes to the window's structure could cause almost anything
	 * to happen, including deleting the parent or child.  If this
	 * happens, we'll be told to abort.
	 */

	if (abort) {
	    goto done;
	}
    }

    done:
    parentPtr->abortPtr = NULL;
    Tk_Release((ClientData) parentPtr);
}

/*
 *--------------------------------------------------------------
 *
 * GetPacker --
 *
 *	This internal procedure is used to locate a Packer
 *	structure for a given window, creating one if one
 *	doesn't exist already.
 *
 * Results:
 *	The return value is a pointer to the Packer structure
 *	corresponding to tkwin.
 *
 * Side effects:
 *	A new packer structure may be created.  If so, then
 *	a callback is set up to clean things up when the
 *	window is deleted.
 *
 *--------------------------------------------------------------
 */

static Packer *
GetPacker(tkwin)
    Tk_Window tkwin;		/* Token for window for which
				 * packer structure is desired. */
{
    register Packer *packPtr;
    Tcl_HashEntry *hPtr;
    int new;

    if (!initialized) {
	initialized = 1;
	Tcl_InitHashTable(&packerHashTable, TCL_ONE_WORD_KEYS);
    }

    /*
     * See if there's already packer for this window.  If not,
     * then create a new one.
     */

    hPtr = Tcl_CreateHashEntry(&packerHashTable, (char *) tkwin, &new);
    if (!new) {
	return (Packer *) Tcl_GetHashValue(hPtr);
    }
    packPtr = (Packer *) ckalloc(sizeof(Packer));
    packPtr->tkwin = tkwin;
    packPtr->parentPtr = NULL;
    packPtr->nextPtr = NULL;
    packPtr->childPtr = NULL;
    packPtr->side = TOP;
    packPtr->anchorPoint = TK_ANCHOR_CENTER;
    packPtr->padX = packPtr->padY = 0;
    packPtr->doubleBw = 2*Tk_Changes(tkwin)->border_width;
    packPtr->abortPtr = NULL;
    packPtr->flags = 0;
    Tcl_SetHashValue(hPtr, packPtr);
    Tk_CreateEventHandler(tkwin, StructureNotifyMask,
	    PackStructureProc, (ClientData) packPtr);
    return packPtr;
}

/*
 *--------------------------------------------------------------
 *
 * PackAfter --
 *
 *	This procedure does most of the real work of adding
 *	one or more windows into the packing order for its parent.
 *
 * Results:
 *	A standard Tcl return value.
 *
 * Side effects:
 *	The geometry of the specified windows may change, both now and
 *	again in the future.
 *
 *--------------------------------------------------------------
 */

static int
PackAfter(interp, prevPtr, parentPtr, argc, argv)
    Tcl_Interp *interp;		/* Interpreter for error reporting. */
    Packer *prevPtr;		/* Pack windows in argv just after this
				 * window;  NULL means pack as first
				 * child of parentPtr. */
    Packer *parentPtr;		/* Parent in which to pack windows. */
    int argc;			/* Number of elements in argv. */
    char **argv;		/* Array of lists, each containing 2
				 * elements:  window name and side
				 * against which to pack. */
{
    register Packer *packPtr;
    Tk_Window tkwin;
    int length, optionCount;
    char **options;
    int index;
    char c;

    /*
     * Iterate over all of the window specifiers, each consisting of
     * two arguments.  The first argument contains the window name and
     * the additional arguments contain options such as "top" or
     * "padx 20".
     */

    for ( ; argc > 0; argc -= 2, argv += 2, prevPtr = packPtr) {
	if (argc < 2) {
	    Tcl_AppendResult(interp, "wrong # args: window \"",
		    argv[0], "\" should be followed by options",
		    (char *) NULL);
	    return TCL_ERROR;
	}

	/*
	 * Find the packer for the window to be packed, and make sure
	 * that the window in which it will be packed is its parent.
	 */

	tkwin = Tk_NameToWindow(interp, argv[0], parentPtr->tkwin);
	if (tkwin == NULL) {
	    return TCL_ERROR;
	}
	if (Tk_Parent(tkwin) != parentPtr->tkwin) {
	    Tcl_AppendResult(interp, "tried to pack \"",
		    argv[0], "\" in window that isn't its parent",
		    (char *) NULL);
	    return TCL_ERROR;
	}
	packPtr = GetPacker(tkwin);

	/*
	 * Process options for this window.
	 */

	if (Tcl_SplitList(interp, argv[1], &optionCount, &options) != TCL_OK) {
	    return TCL_ERROR;
	}
	packPtr->side = TOP;
	packPtr->anchorPoint = TK_ANCHOR_CENTER;
	packPtr->padX = packPtr->padY = 0;
	packPtr->flags &= ~(FILLX|FILLY|EXPAND);
	for (index = 0 ; index < optionCount; index++) {
	    char *curOpt = options[index];

	    c = curOpt[0];
	    length = strlen(curOpt);

	    if ((c == 't')
		    && (strncmp(curOpt, "top", length)) == 0) {
		packPtr->side = TOP;
	    } else if ((c == 'b')
		    && (strncmp(curOpt, "bottom", length)) == 0) {
		packPtr->side = BOTTOM;
	    } else if ((c == 'l')
		    && (strncmp(curOpt, "left", length)) == 0) {
		packPtr->side = LEFT;
	    } else if ((c == 'r')
		    && (strncmp(curOpt, "right", length)) == 0) {
		packPtr->side = RIGHT;
	    } else if ((c == 'e')
		    && (strncmp(curOpt, "expand", length)) == 0) {
		packPtr->flags |= EXPAND;
	    } else if ((c == 'f')
		    && (strcmp(curOpt, "fill")) == 0) {
		packPtr->flags |= FILLX|FILLY;
	    } else if ((length == 5) && (strcmp(curOpt, "fillx")) == 0) {
		packPtr->flags |= FILLX;
	    } else if ((length == 5) && (strcmp(curOpt, "filly")) == 0) {
		packPtr->flags |= FILLY;
	    } else if ((c == 'p') && (strcmp(curOpt, "padx")) == 0) {
		if (optionCount < (index+2)) {
		    missingPad:
		    Tcl_AppendResult(interp, "wrong # args: \"", curOpt,
			    "\" option must be followed by count",
			    (char *) NULL);
		    goto error;
		}
		if ((Tcl_GetInt(interp, options[index+1], &packPtr->padX)
			!= TCL_OK) || (packPtr->padX < 0)) {
		    badPad:
		    Tcl_AppendResult(interp, "bad pad value \"",
			    options[index+1], "\":  must be positive integer",
			    (char *) NULL);
		    goto error;
		}
		index++;
	    } else if ((c == 'p') && (strcmp(curOpt, "pady")) == 0) {
		if (optionCount < (index+2)) {
		    goto missingPad;
		}
		if ((Tcl_GetInt(interp, options[index+1], &packPtr->padY)
			!= TCL_OK) || (packPtr->padY < 0)) {
		    goto badPad;
		}
		index++;
	    } else if ((c == 'f') && (length > 1)
		    && (strncmp(curOpt, "frame", length) == 0)) {
		if (optionCount < (index+2)) {
		    Tcl_AppendResult(interp, "wrong # args: \"frame\" ",
			    "option must be followed by anchor point",
			    (char *) NULL);
		    goto error;
		}
		if (Tk_GetAnchor(interp, options[index+1],
			&packPtr->anchorPoint) != TCL_OK) {
		    goto error;
		}
		index++;
	    } else {
		Tcl_AppendResult(interp, "bad option \"", curOpt,
			"\":  should be top, bottom, left, right, ",
			"expand, fill, fillx, filly, padx, pady, or frame",
			(char *) NULL);
		goto error;
	    }
	}

	if (packPtr != prevPtr) {

	    /*
	     * Unpack this window if it's currently packed.
	     */
	
	    if (packPtr->parentPtr != NULL) {
		Unlink(packPtr);
	    }
	
	    /*
	     * Add the window in the correct place in its parent's
	     * packing order, then make sure that the window is
	     * managed by us.
	     */

	    packPtr->parentPtr = parentPtr;
	    if (prevPtr == NULL) {
		packPtr->nextPtr = parentPtr->childPtr;
		parentPtr->childPtr = packPtr;
	    } else {
		packPtr->nextPtr = prevPtr->nextPtr;
		prevPtr->nextPtr = packPtr;
	    }
	    Tk_ManageGeometry(tkwin, PackReqProc, (ClientData) packPtr);
	}
	ckfree((char *) options);
    }

    /*
     * Arrange for the parent to be re-packed at the first
     * idle moment.
     */

    if (parentPtr->abortPtr != NULL) {
	*parentPtr->abortPtr = 1;
    }
    if (!(parentPtr->flags & REQUESTED_REPACK)) {
	parentPtr->flags |= REQUESTED_REPACK;
	Tk_DoWhenIdle(ArrangePacking, (ClientData) parentPtr);
    }
    return TCL_OK;

    error:
    ckfree((char *) options);
    return TCL_ERROR;
}

/*
 *----------------------------------------------------------------------
 *
 * Unlink --
 *
 *	Remove a packer from its parent's list of children.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The parent will be scheduled for repacking.
 *
 *----------------------------------------------------------------------
 */

static void
Unlink(packPtr)
    register Packer *packPtr;		/* Window to unlink. */
{
    register Packer *parentPtr, *packPtr2;

    parentPtr = packPtr->parentPtr;
    if (parentPtr == NULL) {
	return;
    }
    if (parentPtr->childPtr == packPtr) {
	parentPtr->childPtr = packPtr->nextPtr;
    } else {
	for (packPtr2 = parentPtr->childPtr; ; packPtr2 = packPtr2->nextPtr) {
	    if (packPtr2 == NULL) {
		panic("Unlink couldn't find previous window");
	    }
	    if (packPtr2->nextPtr == packPtr) {
		packPtr2->nextPtr = packPtr->nextPtr;
		break;
	    }
	}
    }
    if (!(parentPtr->flags & REQUESTED_REPACK)) {
	parentPtr->flags |= REQUESTED_REPACK;
	Tk_DoWhenIdle(ArrangePacking, (ClientData) parentPtr);
    }
    if (parentPtr->abortPtr != NULL) {
	*parentPtr->abortPtr = 1;
    }

    packPtr->parentPtr = NULL;
}

/*
 *----------------------------------------------------------------------
 *
 * DestroyPacker --
 *
 *	This procedure is invoked by Tk_EventuallyFree or Tk_Release
 *	to clean up the internal structure of a packer at a safe time
 *	(when no-one is using it anymore).
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Everything associated with the packer is freed up.
 *
 *----------------------------------------------------------------------
 */

static void
DestroyPacker(clientData)
    ClientData clientData;		/* Info about packed window that
					 * is now dead. */
{
    register Packer *packPtr = (Packer *) clientData;
    ckfree((char *) packPtr);
}

/*
 *----------------------------------------------------------------------
 *
 * PackStructureProc --
 *
 *	This procedure is invoked by the Tk event dispatcher in response
 *	to StructureNotify events.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	If a window was just deleted, clean up all its packer-related
 *	information.  If it was just resized, repack its children, if
 *	any.
 *
 *----------------------------------------------------------------------
 */

static void
PackStructureProc(clientData, eventPtr)
    ClientData clientData;		/* Our information about window
					 * referred to by eventPtr. */
    XEvent *eventPtr;			/* Describes what just happened. */
{
    register Packer *packPtr = (Packer *) clientData;
    if (eventPtr->type == ConfigureNotify) {
	if ((packPtr->childPtr != NULL)
		&& !(packPtr->flags & REQUESTED_REPACK)) {
	    packPtr->flags |= REQUESTED_REPACK;
	    Tk_DoWhenIdle(ArrangePacking, (ClientData) packPtr);
	}
	if (packPtr->doubleBw != 2*Tk_Changes(packPtr->tkwin)->border_width) {
	    if ((packPtr->parentPtr != NULL)
		    && !(packPtr->parentPtr->flags & REQUESTED_REPACK)) {
		packPtr->doubleBw = 2*Tk_Changes(packPtr->tkwin)->border_width;
		packPtr->parentPtr->flags |= REQUESTED_REPACK;
		Tk_DoWhenIdle(ArrangePacking, (ClientData) packPtr->parentPtr);
	    }
	}
    } else if (eventPtr->type == DestroyNotify) {
	register Packer *packPtr2;

	if (packPtr->parentPtr != NULL) {
	    Unlink(packPtr);
	}
	for (packPtr2 = packPtr->childPtr; packPtr2 != NULL;
		packPtr2 = packPtr2->nextPtr) {
	    packPtr2->parentPtr = NULL;
	    packPtr2->nextPtr = NULL;
	}
	Tcl_DeleteHashEntry(Tcl_FindHashEntry(&packerHashTable,
		(char *) packPtr->tkwin));
	if (packPtr->flags & REQUESTED_REPACK) {
	    Tk_CancelIdleCall(ArrangePacking, (ClientData) packPtr);
	}
	packPtr->tkwin = NULL;
	Tk_EventuallyFree((ClientData) packPtr, DestroyPacker);
    }
}
