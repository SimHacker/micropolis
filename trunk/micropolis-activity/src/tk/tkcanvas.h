/*
 * tkCanvas.h --
 *
 *	Declarations shared among all the files that implement
 *	canvas widgets.
 *
 * Copyright 1991-1992 Regents of the University of California.
 * Permission to use, copy, modify, and distribute this
 * software and its documentation for any purpose and without
 * fee is hereby granted, provided that the above copyright
 * notice appear in all copies.  The University of California
 * makes no representations about the suitability of this
 * software for any purpose.  It is provided "as is" without
 * express or implied warranty.
 *
 * $Header: /user6/ouster/wish/RCS/tkCanvas.h,v 1.14 92/08/19 08:34:16 ouster Exp $ SPRITE (Berkeley)
 */

#ifndef _TKCANVAS
#define _TKCANVAS

#ifndef _TK
#include "tk.h"
#endif

/*
 * For each item in a canvas widget there exists one record with
 * the following structure.  Each actual item is represented by
 * a record with the following stuff at its beginning, plus additional
 * type-specific stuff after that.
 */

#define TK_TAG_SPACE 3

typedef struct Tk_Item  {
    int id;				/* Unique identifier for this item
					 * (also serves as first tag for
					 * item). */
    struct Tk_Item *nextPtr;		/* Next in display list of all
					 * items in this canvas.  Later items
					 * in list are drawn on top of earlier
					 * ones. */
    Tk_Uid staticTagSpace[TK_TAG_SPACE];/* Built-in space for limited # of
					 * tags. */
    Tk_Uid *tagPtr;			/* Pointer to array of tags.  Usually
					 * points to staticTagSpace, but
					 * may point to malloc-ed space if
					 * there are lots of tags. */
    int tagSpace;			/* Total amount of tag space available
					 * at tagPtr. */
    int numTags;			/* Number of tag slots actually used
					 * at *tagPtr. */
    struct Tk_ItemType *typePtr;	/* Table of procedures that implement
					 * this type of item. */
    int x1, y1, x2, y2;			/* Bounding box for item, in integer
					 * canvas units. Set by item-specific
					 * code and guaranteed to contain every
					 * pixel drawn in item.  Item area
					 * includes x1 and y1 but not x2
					 * and y2. */

    /*
     *------------------------------------------------------------------
     * Starting here is additional type-specific stuff;  see the
     * declarations for individual types to see what is part of
     * each type.  The actual space below is determined by the
     * "itemInfoSize" of the type's Tk_ItemType record.
     *------------------------------------------------------------------
     */
} Tk_Item;

/*
 * The record below describes a canvas widget.  It is made available
 * to the item procedures so they can access certain shared fields such
 * as the overall displacement and scale factor for the canvas.
 */

typedef struct {
    Tk_Window tkwin;		/* Window that embodies the canvas.  NULL
				 * means that the window has been destroyed
				 * but the data structures haven't yet been
				 * cleaned up.*/
    Tcl_Interp *interp;		/* Interpreter associated with canvas. */
    Tk_Item *firstItemPtr;	/* First in list of all items in canvas,
				 * or NULL if canvas empty. */
    Tk_Item *lastItemPtr;	/* Last in list of all items in canvas,
				 * or NULL if canvas empty. */

    /*
     * Information used when displaying widget:
     */

    int borderWidth;		/* Width of 3-D border around window. */
    Tk_3DBorder bgBorder;	/* Used for canvas background. */
    XColor *bgColor;		/* Color used for clearing to background. */
    int relief;			/* Indicates whether window as a whole is
				 * raised, sunken, or flat. */
    GC pixmapGC;		/* Used to copy bits from a pixmap to the
				 * screen and also to clear the pixmap. */
    int width, height;		/* Dimensions to request for canvas window,
				 * specified in pixels. */
    int redrawX1, redrawY1;	/* Upper left corner of area to redraw,
				 * in pixel coordinates.  Border pixels
				 * are included.  Only valid if
				 * REDRAW_PENDING flag is set. */
    int redrawX2, redrawY2;	/* Lower right corner of area to redraw,
				 * in pixel coordinates.  Border pixels
				 * will *not* be redrawn. */
    int confine;		/* Non-zero means constrain view to keep
				 * as much of canvas visible as possible. */

    /*
     * Information used to manage and display selection:
     */

    Tk_3DBorder selBorder;	/* Border and background for selected
				 * characters. */
    int selBorderWidth;		/* Width of border around selection. */
    XColor *selFgColorPtr;	/* Foreground color for selected text. */
    Tk_Item *selItemPtr;	/* Pointer to selected item.  NULL means
				 * selection isn't in this canvas. */
    int selectFirst;		/* Index of first selected character. */
    int selectLast;		/* Index of last selected character. */
    Tk_Item *anchorItemPtr;	/* Item corresponding to "selectAnchor":
				 * not necessarily selItemPtr. */
    int selectAnchor;		/* Fixed end of selection (i.e. "select to"
				 * operation will use this as one end of the
				 * selection). */

    /*
     * Information for display insertion cursor in text:
     */

    Tk_3DBorder cursorBorder;	/* Used to draw vertical bar for insertion
				 * cursor. */
    int cursorWidth;		/* Total width of insertion cursor. */
    int cursorBorderWidth;	/* Width of 3-D border around insert cursor. */
    int cursorOnTime;		/* Number of milliseconds cursor should spend
				 * in "on" state for each blink. */
    int cursorOffTime;		/* Number of milliseconds cursor should spend
				 * in "off" state for each blink. */
    Tk_TimerToken cursorBlinkHandler;
				/* Timer handler used to blink cursor on and
				 * off. */
    Tk_Item *focusItemPtr;	/* Item that currently has the input focus,
				 * or NULL if no such item. */

    /*
     * Transformation applied to canvas as a whole:  to compute screen
     * coordinates (X,Y) from canvas coordinates (x,y), do the following:
     *
     * X = x - xOrigin;
     * Y = y - yOrigin;
     */

    int xOrigin, yOrigin;	/* Canvas coordinates corresponding to
				 * upper-left corner of window, given in
				 * canvas pixel units. */
    int drawableXOrigin, drawableYOrigin;
				/* During redisplay, these fields give the
				 * canvas coordinates corresponding to
				 * the upper-left corner of the drawable
				 * where items are actually being drawn
				 * (typically a pixmap smaller than the
				 * whole window). */

    /*
     * Information used for event bindings associated with items.
     */

    Tk_BindingTable bindingTable;
				/* Table of all bindings currently defined
				 * for this canvas.  NULL means that no
				 * bindings exist, so the table hasn't been
				 * created.  Each "object" used for this
				 * table is either a Tk_Uid for a tag or
				 * the address of an item named by id. */
    Tk_Item *currentItemPtr;	/* The item currently containing the mouse
				 * pointer, or NULL if none. */
    double closeEnough;		/* The mouse is assumed to be inside an
				 * item if it is this close to it. */
    XEvent pickEvent;		/* The event upon which the current choice
				 * of currentItem is based.  Must be saved
				 * so that if the currentItem is deleted,
				 * can pick another. */

    /*
     * Information used for managing scrollbars:
     */

    char *xScrollCmd;		/* Command prefix for communicating with
				 * horizontal scrollbar.  NULL means no
				 * horizontal scrollbar.  Malloc'ed*/
    char *yScrollCmd;		/* Command prefix for communicating with
				 * vertical scrollbar.  NULL means no
				 * vertical scrollbar.  Malloc'ed*/
    int scrollX1, scrollY1, scrollX2, scrollY2;
				/* These four coordinates define the region
				 * that is the 100% area for scrolling (i.e.
				 * these numbers determine the size and
				 * location of the sliders on scrollbars).
				 * Units are pixels in canvas coords. */
    char *regionString;		/* The option string from which scrollX1
				 * etc. are derived.  Malloc'ed. */
    int scrollIncrement;	/* The number of canvas units that the
				 * picture shifts when a scrollbar up or
				 * down arrow is pressed. */

    /*
     * Information used for scanning:
     */

    int scanX;			/* X-position at which scan started (e.g.
				 * button was pressed here). */
    int scanXOrigin;		/* Value of xOrigin field when scan started. */
    int scanY;			/* Y-position at which scan started (e.g.
				 * button was pressed here). */
    int scanYOrigin;		/* Value of yOrigin field when scan started. */

    /*
     * Information used to speed up searches by remembering the last item
     * created or found with an item id search.
     */

    Tk_Item *hotPtr;		/* Pointer to "hot" item (one that's been
				 * recently used.  NULL means there's no
				 * hot item. */
    Tk_Item *hotPrevPtr;	/* Pointer to predecessor to hotPtr (NULL
				 * means item is first in list).  This is
				 * only a hint and may not really be hotPtr's
				 * predecessor. */

    /*
     * Miscellaneous information:
     */

    Cursor cursor;		/* Current cursor for window, or None. */
    double pixelsPerMM;		/* Scale factor between MM and pixels;
				 * used when converting coordinates. */
    int flags;			/* Various flags;  see below for
				 * definitions. */
    int nextId;			/* Number to use as id for next item
				 * created in widget. */
    Tk_TimerToken updateTimerToken; /* Added by Don to optimize rapid 
				     * updates. */
} Tk_Canvas;

/*
 * Flag bits for canvases:
 *
 * REDRAW_PENDING -		1 means a DoWhenIdle handler has already
 *				been created to redraw some or all of the
 *				canvas.
 * REPICK_NEEDED -		1 means DisplayCanvas should pick a new
 *				current item before redrawing the canvas.
 * GOT_FOCUS -			1 means the focus is currently in this
 *				widget, so should draw the insertion cursor.
 * CURSOR_ON -			1 means the insertion cursor is in the "on"
 *				phase of its blink cycle.  0 means either
 *				we don't have the focus or the cursor is in
 *				the "off" phase of its cycle.
 * BUTTON_DOWN -		1 means that a button is currently down;
 *				this is used to implement grabs for the
 *				duration of button presses.
 * UPDATE_SCROLLBARS -		1 means the scrollbars should get updated
 *				as part of the next display operation.
 */

#define REDRAW_PENDING		1
#define REPICK_NEEDED		2
#define GOT_FOCUS		4
#define CURSOR_ON		8
#define BUTTON_DOWN		0x10
#define UPDATE_SCROLLBARS	0x20

/*
 * Records of the following type are used to describe a type of
 * item (e.g.  lines, circles, etc.) that can form part of a
 * canvas widget.
 */

typedef int	Tk_ItemCreateProc _ANSI_ARGS_((Tk_Canvas *canvasPtr,
		    Tk_Item *itemPtr, int argc, char **argv));
typedef int	Tk_ItemConfigureProc _ANSI_ARGS_((Tk_Canvas *canvasPtr,
		    Tk_Item *itemPtr, int argc, char **argv, int flags));
typedef int	Tk_ItemCoordProc _ANSI_ARGS_((Tk_Canvas *canvasPtr,
		    Tk_Item *itemPtr, int argc, char **argv));
typedef void	Tk_ItemDeleteProc _ANSI_ARGS_((Tk_Item *itemPtr));
typedef void	Tk_ItemDisplayProc _ANSI_ARGS_((Tk_Canvas *canvasPtr,
		    Tk_Item *itemPtr, Drawable dst));
typedef double	Tk_ItemPointProc _ANSI_ARGS_((Tk_Canvas *canvasPtr,
		    Tk_Item *itemPtr, double *pointPtr));
typedef int	Tk_ItemAreaProc _ANSI_ARGS_((Tk_Canvas *canvasPtr,
		    Tk_Item *itemPtr, double *rectPtr));
typedef void	Tk_ItemPostscriptProc _ANSI_ARGS_((Tk_Canvas *canvasPtr,
		    Tk_Item *itemPtr));
typedef void	Tk_ItemScaleProc _ANSI_ARGS_((Tk_Canvas *canvasPtr,
		    Tk_Item *itemPtr, double originX, double originY,
		    double scaleX, double scaleY));
typedef void	Tk_ItemTranslateProc _ANSI_ARGS_((Tk_Canvas *canvasPtr,
		    Tk_Item *itemPtr, double deltaX, double deltaY));
typedef int	Tk_ItemIndexProc _ANSI_ARGS_((Tk_Canvas *canvasPtr,
		    Tk_Item *itemPtr, char *indexString,
		    int *indexPtr));
typedef void	Tk_ItemCursorProc _ANSI_ARGS_((Tk_Canvas *canvasPtr,
		    Tk_Item *itemPtr, int index));
typedef int	Tk_ItemSelectionProc _ANSI_ARGS_((Tk_Canvas *canvasPtr,
		    Tk_Item *itemPtr, int offset, char *buffer,
		    int maxBytes));
typedef int	Tk_ItemInsertProc _ANSI_ARGS_((Tk_Canvas *canvasPtr,
		    Tk_Item *itemPtr, int beforeThis, char *string));
typedef int	Tk_ItemDCharsProc _ANSI_ARGS_((Tk_Canvas *canvasPtr,
		    Tk_Item *itemPtr, int first, int last));

typedef struct Tk_ItemType {
    char *name;				/* The name of this type of item, such
					 * as "line". */
    int itemSize;			/* Total amount of space needed for
					 * item's record. */
    Tk_ItemCreateProc *createProc;	/* Procedure to create a new item of
					 * this type. */
    Tk_ConfigSpec *configSpecs;		/* Pointer to array of configuration
					 * specs for this type.  Used for
					 * returning configuration info. */
    Tk_ItemConfigureProc *configProc;	/* Procedure to call to change
					 * configuration options. */
    Tk_ItemCoordProc *coordProc;	/* Procedure to call to get and set
					 * the item's coordinates. */
    Tk_ItemDeleteProc *deleteProc;	/* Procedure to delete existing item of
					 * this type. */
    Tk_ItemDisplayProc *displayProc;	/* Procedure to display items of
					 * this type. */
    int alwaysRedraw;			/* Non-zero means displayProc should
					 * be called even when the item has
					 * been moved off-screen. */
    Tk_ItemPointProc *pointProc;	/* Computes distance from item to
					 * a given point. */
    Tk_ItemAreaProc *areaProc;		/* Computes whether item is inside,
					 * outside, or overlapping an area. */
    Tk_ItemPostscriptProc *postscriptProc;
					/* Procedure to write a Postscript
					 * description for items of this
					 * type. */
    Tk_ItemScaleProc *scaleProc;	/* Procedure to rescale items of
					 * this type. */
    Tk_ItemTranslateProc *translateProc;/* Procedure to translate items of
					 * this type. */
    Tk_ItemIndexProc *indexProc;	/* Procedure to determine index of
					 * indicated character.  NULL if
					 * item doesn't support indexing. */
    Tk_ItemCursorProc *cursorProc;	/* Procedure to set cursor position
					 * to just before a given position. */
    Tk_ItemSelectionProc *selectionProc;/* Procedure to return selection (in
					 * STRING format) when it is in this
					 * item. */
    Tk_ItemInsertProc *insertProc;	/* Procedure to insert something into
					 * an item. */
    Tk_ItemDCharsProc *dCharsProc;	/* Procedure to delete characters
					 * from an item. */
    struct Tk_ItemType *nextPtr;	/* Used to link types together into
					 * a list. */
} Tk_ItemType;

/*
 * Macros to transform a point from double-precision canvas coordinates
 * to integer pixel coordinates in the pixmap where redisplay is being
 * done.
 */

#define SCREEN_X(canvasPtr, x) \
	(((int) ((x) + 0.5)) - (canvasPtr)->drawableXOrigin)
#define SCREEN_Y(canvasPtr, y) \
	(((int) ((y) + 0.5)) - (canvasPtr)->drawableYOrigin)

/*
 * Canvas-related variables that are shared among Tk modules but not
 * exported to the outside world:
 */

extern Tk_CustomOption tkCanvasTagsOption;

/*
 * Canvas-related procedures that are shared among Tk modules but not
 * exported to the outside world:
 */

extern void		TkBezierScreenPoints _ANSI_ARGS_((Tk_Canvas *canvasPtr,
			    double control[], int numSteps,
			    XPoint *xPointPtr));
extern void		TkFillPolygon _ANSI_ARGS_((Tk_Canvas *canvasPtr,
			    double *coordPtr, int numPoints, Drawable drawable,
			    GC gc));
extern int		TkGetCanvasCoord _ANSI_ARGS_((Tk_Canvas *canvasPtr,
			    char *string, double *doublePtr));
extern void		TkIncludePoint _ANSI_ARGS_((Tk_Canvas *canvasPtr,
			    Tk_Item *itemPtr, double *pointPtr));
extern int		TkMakeBezierCurve _ANSI_ARGS_((Tk_Canvas *canvasPtr,
			    double *pointPtr, int numPoints, int numSteps,
			    XPoint xPoints[], double dblPoints[]));

#endif /* _TKCANVAS */
