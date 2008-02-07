/* 
 * tkGC.c --
 *
 *	This file maintains a database of read-only graphics contexts 
 *	for the Tk toolkit, in order to allow GC's to be shared.
 *
 * Copyright 1990 Regents of the University of California
 * Permission to use, copy, modify, and distribute this
 * software and its documentation for any purpose and without
 * fee is hereby granted, provided that the above copyright
 * notice appear in all copies.  The University of California
 * makes no representations about the suitability of this
 * software for any purpose.  It is provided "as is" without
 * express or implied warranty.
 */

#ifndef lint
static char rcsid[] = "$Header: /user6/ouster/wish/RCS/tkGC.c,v 1.9 92/05/13 08:48:45 ouster Exp $ SPRITE (Berkeley)";
#endif /* not lint */

#include "tkconfig.h"
#include "tk.h"

/*
 * One of the following data structures exists for each GC that is
 * currently active.  The structure is indexed with two hash tables,
 * one based on font name and one based on XFontStruct address.
 */

typedef struct {
    GC gc;			/* Graphics context. */
    Display *display;		/* Display to which gc belongs. */
    int refCount;		/* Number of active uses of gc. */
    Tcl_HashEntry *valueHashPtr;/* Entry in valueTable (needed when deleting
				 * this structure). */
} TkGC;

/*
 * Hash table to map from a GC's values to a TkGC structure describing
 * a GC with those values (used by Tk_GetGC).
 */

static Tcl_HashTable valueTable;
typedef struct {
    XGCValues values;		/* Desired values for GC. */
    Screen *screen;		/* Screen for which GC is valid. */
} ValueKey;

/*
 * Hash table for GC -> TkGC mapping. This table is indexed by the 
 * GC identifier, and is used by Tk_FreeGC.
 */

static Tcl_HashTable idTable;

static int initialized = 0;	/* 0 means static structures haven't been
				 * initialized yet. */

/*
 * Forward declarations for procedures defined in this file:
 */

static void		GCInit _ANSI_ARGS_((void));

/*
 *----------------------------------------------------------------------
 *
 * Tk_GetGC --
 *
 *	Given a desired set of values for a graphics context, find
 *	a read-only graphics context with the desired values.
 *
 * Results:
 *	The return value is the X identifer for the desired graphics
 *	context.  The caller should never modify this GC, and should
 *	call Tk_FreeGC when the GC is no longer needed.
 *
 * Side effects:
 *	The GC is added to an internal database with a reference count.
 *	For each call to this procedure, there should eventually be a call
 *	to Tk_FreeGC, so that the database can be cleaned up when GC's
 *	aren't needed anymore.
 *
 *----------------------------------------------------------------------
 */

GC
Tk_GetGC(tkwin, valueMask, valuePtr)
    Tk_Window tkwin;		/* Window in which GC will be used. */
    register unsigned long valueMask;
				/* 1 bits correspond to values specified
				 * in *valuesPtr;  other values are set
				 * from defaults. */
    register XGCValues *valuePtr;
				/* Values are specified here for bits set
				 * in valueMask. */
{
    ValueKey key;
    Tcl_HashEntry *valueHashPtr, *idHashPtr;
    register TkGC *gcPtr;
    int new;

    if (!initialized) {
	GCInit();
    }

    /*
     * Must zero key at start to clear out pad bytes that may be
     * part of structure on some systems.
     */

    memset((VOID *) &key, 0, sizeof(key));

    /*
     * First, check to see if there's already a GC that will work
     * for this request (exact matches only, sorry).
     */

    if (valueMask & GCFunction) {
	key.values.function = valuePtr->function;
    } else {
	key.values.function = GXcopy;
    }
    if (valueMask & GCPlaneMask) {
	key.values.plane_mask = valuePtr->plane_mask;
    } else {
	key.values.plane_mask = ~0;
    }
    if (valueMask & GCForeground) {
	key.values.foreground = valuePtr->foreground;
    } else {
	key.values.foreground = 0;
    }
    if (valueMask & GCBackground) {
	key.values.background = valuePtr->background;
    } else {
	key.values.background = 1;
    }
    if (valueMask & GCLineWidth) {
	key.values.line_width = valuePtr->line_width;
    } else {
	key.values.line_width = 0;
    }
    if (valueMask & GCLineStyle) {
	key.values.line_style = valuePtr->line_style;
    } else {
	key.values.line_style = LineSolid;
    }
    if (valueMask & GCCapStyle) {
	key.values.cap_style = valuePtr->cap_style;
    } else {
	key.values.cap_style = CapButt;
    }
    if (valueMask & GCJoinStyle) {
	key.values.join_style = valuePtr->join_style;
    } else {
	key.values.join_style = JoinMiter;
    }
    if (valueMask & GCFillStyle) {
	key.values.fill_style = valuePtr->fill_style;
    } else {
	key.values.fill_style = FillSolid;
    }
    if (valueMask & GCFillRule) {
	key.values.fill_rule = valuePtr->fill_rule;
    } else {
	key.values.fill_rule = EvenOddRule;
    }
    if (valueMask & GCArcMode) {
	key.values.arc_mode = valuePtr->arc_mode;
    } else {
	key.values.arc_mode = ArcPieSlice;
    }
    if (valueMask & GCTile) {
	key.values.tile = valuePtr->tile;
    } else {
	key.values.tile = None;
    }
    if (valueMask & GCStipple) {
	key.values.stipple = valuePtr->stipple;
    } else {
	key.values.stipple = None;
    }
    if (valueMask & GCTileStipXOrigin) {
	key.values.ts_x_origin = valuePtr->ts_x_origin;
    } else {
	key.values.ts_x_origin = 0;
    }
    if (valueMask & GCTileStipYOrigin) {
	key.values.ts_y_origin = valuePtr->ts_y_origin;
    } else {
	key.values.ts_y_origin = 0;
    }
    if (valueMask & GCFont) {
	key.values.font = valuePtr->font;
    } else {
	key.values.font = None;
    }
    if (valueMask & GCSubwindowMode) {
	key.values.subwindow_mode = valuePtr->subwindow_mode;
    } else {
	key.values.subwindow_mode = ClipByChildren;
    }
    if (valueMask & GCGraphicsExposures) {
	key.values.graphics_exposures = valuePtr->graphics_exposures;
    } else {
	key.values.graphics_exposures = True;
    }
    if (valueMask & GCClipXOrigin) {
	key.values.clip_x_origin = valuePtr->clip_x_origin;
    } else {
	key.values.clip_x_origin = 0;
    }
    if (valueMask & GCClipYOrigin) {
	key.values.clip_y_origin = valuePtr->clip_y_origin;
    } else {
	key.values.clip_y_origin = 0;
    }
    if (valueMask & GCClipMask) {
	key.values.clip_mask = valuePtr->clip_mask;
    } else {
	key.values.clip_mask = None;
    }
    if (valueMask & GCDashOffset) {
	key.values.dash_offset = valuePtr->dash_offset;
    } else {
	key.values.dash_offset = 0;
    }
    if (valueMask & GCDashList) {
	key.values.dashes = valuePtr->dashes;
    } else {
	key.values.dashes = 4;
    }
    key.screen = Tk_Screen(tkwin);
    valueHashPtr = Tcl_CreateHashEntry(&valueTable, (char *) &key, &new);
    if (!new) {
	gcPtr = (TkGC *) Tcl_GetHashValue(valueHashPtr);
	gcPtr->refCount++;
	return gcPtr->gc;
    }

    /*
     * No GC is currently available for this set of values.  Allocate a
     * new GC and add a new structure to the database.
     */

    gcPtr = (TkGC *) ckalloc(sizeof(TkGC));
#if 0
    gcPtr->gc = XCreateGC(Tk_Display(tkwin),
			  RootWindowOfScreen(Tk_Screen(tkwin)),
			  valueMask, &key.values);
#else
    gcPtr->gc = XCreateGC(Tk_Display(tkwin),
			  Tk_DefaultPixmap(Tk_Screen(tkwin)),
			  valueMask, &key.values);
#endif
    gcPtr->display = Tk_Display(tkwin);
    gcPtr->refCount = 1;
    gcPtr->valueHashPtr = valueHashPtr;
    idHashPtr = Tcl_CreateHashEntry(&idTable, (char *) gcPtr->gc, &new);
    if (!new) {
	panic("GC already registered in Tk_GetGC");
    }
    Tcl_SetHashValue(valueHashPtr, gcPtr);
    Tcl_SetHashValue(idHashPtr, gcPtr);
    return gcPtr->gc;
}

/*
 *----------------------------------------------------------------------
 *
 * Tk_FreeGC --
 *
 *	This procedure is called to release a font allocated by
 *	Tk_GetGC.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The reference count associated with gc is decremented, and
 *	gc is officially deallocated if no-one is using it anymore.
 *
 *----------------------------------------------------------------------
 */

void
Tk_FreeGC(gc)
    GC gc;			/* Graphics context to be released. */
{
    Tcl_HashEntry *idHashPtr;
    register TkGC *gcPtr;

    if (!initialized) {
	panic("Tk_FreeGC called before Tk_GetGC");
    }

    idHashPtr = Tcl_FindHashEntry(&idTable, (char *) gc);
    if (idHashPtr == NULL) {
	panic("Tk_FreeGC received unknown gc argument");
    }
    gcPtr = (TkGC *) Tcl_GetHashValue(idHashPtr);
    gcPtr->refCount--;
    if (gcPtr->refCount == 0) {
	XFreeGC(gcPtr->display, gcPtr->gc);
	Tcl_DeleteHashEntry(gcPtr->valueHashPtr);
	Tcl_DeleteHashEntry(idHashPtr);
	ckfree((char *) gcPtr);
    }
}

/*
 *----------------------------------------------------------------------
 *
 * GCInit --
 *
 *	Initialize the structures used for GC management.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Read the code.
 *
 *----------------------------------------------------------------------
 */

static void
GCInit()
{
    initialized = 1;
    Tcl_InitHashTable(&valueTable, sizeof(ValueKey)/sizeof(int));
    Tcl_InitHashTable(&idTable, TCL_ONE_WORD_KEYS);
}
