/* 
 * tkColor.c --
 *
 *	This file maintains a database of color values for the Tk
 *	toolkit, in order to avoid round-trips to the server to
 *	map color names to pixel values.
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
static char rcsid[] = "$Header: /user6/ouster/wish/RCS/tkColor.c,v 1.15 92/07/14 08:44:49 ouster Exp $ SPRITE (Berkeley)";
#endif /* not lint */

#include "tkconfig.h"
#include "tk.h"

/*
 * A two-level data structure is used to manage the color database.
 * The top level consists of one entry for each color name that is
 * currently active, and the bottom level contains one entry for each
 * pixel value that is still in use.  The distinction between
 * levels is necessary because the same pixel may have several
 * different names.  There are two hash tables, one used to index into
 * each of the data structures.  The name hash table is used when
 * allocating colors, and the pixel hash table is used when freeing
 * colors.
 */

/*
 * One of the following data structures is used to keep track of
 * each color that this module has allocated from the X display
 * server.  These entries are indexed by two hash tables defined
 * below:  nameTable and valueTable.
 */

#define COLOR_MAGIC 0xc6140277

typedef struct TkColor {
    XColor color;		/* Information about this color. */
    int magic;			/* Used for quick integrity check on this
				 * structure.   Must always have the
				 * value COLOR_MAGIC. */
    Screen *screen;		/* Screen where this color is valid.  Used
				 * to delete it. */
    Colormap colormap;		/* Colormap from which this entry was
				 * allocated. */
    int refCount;		/* Number of uses of this structure. */
    Tcl_HashTable *tablePtr;	/* Hash table that indexes this structure
				 * (needed when deleting structure). */
    Tcl_HashEntry *hashPtr;	/* Pointer to hash table entry for this
				 * structure. (for use in deleting entry). */
} TkColor;

typedef struct VisInfo {
    Visual *visual;
    Screen *screen;
    Colormap colormap;
    int depth;
    Pixmap pixmap;
    GC gc;
} VisInfo;

/*
 * Hash table for name -> TkColor mapping, and key structure used to
 * index into that table:
 */

static Tcl_HashTable nameTable;
typedef struct {
    Tk_Uid name;		/* Name of desired color. */
    Colormap colormap;		/* Colormap from which color will be
				 * allocated. */
    Display *display;		/* Display for colormap. */
} NameKey;

/*
 * Hash table for value -> TkColor mapping, and key structure used to
 * index into that table:
 */

static Tcl_HashTable valueTable;
typedef struct {
    int red, green, blue;	/* Values for desired color. */
    Colormap colormap;		/* Colormap from which color will be
				 * allocated. */
    Display *display;		/* Display for colormap. */
} ValueKey;

/*
 * Global colormap creation flag
 */
char *TK_CreateColormap = 0;

/*
 * Hash table for screen -> VisInfo mapping, and key structure used to
 * index into that table:
 */

static Tcl_HashTable screenTable;

static int initialized = 0;	/* 0 means static structures haven't been
				 * initialized yet. */

/*
 * Forward declarations for procedures defined in this file:
 */

static void		ColorInit _ANSI_ARGS_((void));

/*
 *----------------------------------------------------------------------
 *
 * Tk_GetColor --
 *
 *	Given a string name for a color, map the name to a corresponding
 *	XColor structure.
 *
 * Results:
 *	The return value is a pointer to an XColor structure that
 *	indicates the red, blue, and green intensities for the color
 *	given by "name", and also specifies a pixel value to use to
 *	draw in that color in window "tkwin".  If an error occurs,
 *	then NULL is returned and an error message will be left in
 *	interp->result.
 *
 * Side effects:
 *	The color is added to an internal database with a reference count.
 *	For each call to this procedure, there should eventually be a call
 *	to Tk_FreeColor, so that the database is cleaned up when colors
 *	aren't in use anymore.
 *
 *----------------------------------------------------------------------
 */

XColor *
Tk_GetColor(interp, tkwin, colormap, name)
    Tcl_Interp *interp;		/* Place to leave error message if
				 * color can't be found. */
    Tk_Window tkwin;		/* Window in which color will be used. */
    Colormap colormap;		/* Map from which to allocate color.  None
				 * means use default. */
    Tk_Uid name;		/* Name of color to allocated (in form
				 * suitable for passing to XParseColor). */
{
    NameKey nameKey;
    Tcl_HashEntry *nameHashPtr;
    int new;
    TkColor *tkColPtr;
    XColor color;

    if (!initialized) {
	ColorInit();
    }

    /*
     * First, check to see if there's already a mapping for this color
     * name.
     */

    nameKey.name = name;
    if (colormap == None) {
	colormap = Tk_DefaultColormap(Tk_Screen(tkwin));
    }
    nameKey.colormap = colormap;
    nameKey.display = Tk_Display(tkwin);
    nameHashPtr = Tcl_CreateHashEntry(&nameTable, (char *) &nameKey, &new);
    if (!new) {
	tkColPtr = (TkColor *) Tcl_GetHashValue(nameHashPtr);
	tkColPtr->refCount++;
	return &tkColPtr->color;
    }

    /*
     * The name isn't currently known.  Map from the name to a pixel
     * value.  Be tricky here, and call XAllocNamedColor instead of
     * XParseColor for non-# names:  this saves a server round-trip
     * for those names.
     */

    if (*name != '#') {
	XColor screen;

	if (XAllocNamedColor(Tk_Display(tkwin), colormap, name,
		&screen, &color) == 0) {
	    allocFailed:
	    Tcl_AppendResult(interp, "couldn't allocate a color for \"",
		    name, "\"", (char *) NULL);
	    Tcl_DeleteHashEntry(nameHashPtr);
	    return (XColor *) NULL;
	}
    } else {
	if (XParseColor(Tk_Display(tkwin), colormap, name, &color) == 0) {
	    Tcl_AppendResult(interp, "invalid color name \"", name,
		    "\"", (char *) NULL);
	    Tcl_DeleteHashEntry(nameHashPtr);
	    return (XColor *) NULL;
	}
	if (XAllocColor(Tk_Display(tkwin), colormap, &color) == 0) {
	    goto allocFailed;
	}
    }

    /*
     * Now create a new TkColor structure and add it to nameTable.
     */

    tkColPtr = (TkColor *) ckalloc(sizeof(TkColor));
    tkColPtr->color = color;
    tkColPtr->magic = COLOR_MAGIC;
    tkColPtr->screen = Tk_Screen(tkwin);
    tkColPtr->colormap = colormap;
    tkColPtr->refCount = 1;
    tkColPtr->tablePtr = &nameTable;
    tkColPtr->hashPtr = nameHashPtr;
    Tcl_SetHashValue(nameHashPtr, tkColPtr);

    return &tkColPtr->color;
}

/*
 *----------------------------------------------------------------------
 *
 * Tk_GetColorByValue --
 *
 *	Given a desired set of red-green-blue intensities for a color,
 *	locate a pixel value to use to draw that color in a given
 *	window.
 *
 * Results:
 *	The return value is a pointer to an XColor structure that
 *	indicates the closest red, blue, and green intensities available
 *	to those specified in colorPtr, and also specifies a pixel
 *	value to use to draw in that color in window "tkwin".  If an
 *	error occurs, then NULL is returned and an error message will
 *	be left in interp->result.
 *
 * Side effects:
 *	The color is added to an internal database with a reference count.
 *	For each call to this procedure, there should eventually be a call
 *	to Tk_FreeColor, so that the database is cleaned up when colors
 *	aren't in use anymore.
 *
 *----------------------------------------------------------------------
 */

XColor *
Tk_GetColorByValue(interp, tkwin, colormap, colorPtr)
    Tcl_Interp *interp;		/* Place to leave error message if
				 * color can't be found. */
    Tk_Window tkwin;		/* Window in which color will be used. */
    Colormap colormap;		/* Map from which to allocate color.  None
				 * means use default. */
    XColor *colorPtr;		/* Red, green, and blue fields indicate
				 * desired color. */
{
    ValueKey valueKey;
    Tcl_HashEntry *valueHashPtr;
    int new;
    TkColor *tkColPtr;

    if (!initialized) {
	ColorInit();
    }

    /*
     * First, check to see if there's already a mapping for this color
     * name.
     */

    valueKey.red = colorPtr->red;
    valueKey.green = colorPtr->green;
    valueKey.blue = colorPtr->blue;
    if (colormap == None) {
	colormap = Tk_DefaultColormap(Tk_Screen(tkwin));
    }
    valueKey.colormap = colormap;
    valueKey.display = Tk_Display(tkwin);
    valueHashPtr = Tcl_CreateHashEntry(&valueTable, (char *) &valueKey, &new);
    if (!new) {
	tkColPtr = (TkColor *) Tcl_GetHashValue(valueHashPtr);
	tkColPtr->refCount++;
	return &tkColPtr->color;
    }

    /*
     * The name isn't currently known.  Find a pixel value for this
     * color and add a new structure to valueTable.
     */

    tkColPtr = (TkColor *) ckalloc(sizeof(TkColor));
    tkColPtr->color.red = valueKey.red;
    tkColPtr->color.green = valueKey.green;
    tkColPtr->color.blue = valueKey.blue;
    if (XAllocColor(Tk_Display(tkwin), colormap, &tkColPtr->color) == 0) {
	sprintf(interp->result, "couldn't allocate color");
	Tcl_DeleteHashEntry(valueHashPtr);
	ckfree((char *) tkColPtr);
	return (XColor *) NULL;
    }
    tkColPtr->magic = COLOR_MAGIC;
    tkColPtr->screen = Tk_Screen(tkwin);
    tkColPtr->colormap = colormap;
    tkColPtr->refCount = 1;
    tkColPtr->tablePtr = &valueTable;
    tkColPtr->hashPtr = valueHashPtr;
    Tcl_SetHashValue(valueHashPtr, tkColPtr);

    return &tkColPtr->color;
}

/*
 *--------------------------------------------------------------
 *
 * Tk_NameOfColor --
 *
 *	Given a color, return a textual string identifying
 *	the color.
 *
 * Results:
 *	If colorPtr was created by Tk_GetColor, then the return
 *	value is the "string" that was used to create it.
 *	Otherwise the return value is a string that could have
 *	been passed to Tk_GetColor to allocate that color.  The
 *	storage for the returned string is only guaranteed to
 *	persist up until the next call to this procedure.
 *
 * Side effects:
 *	None.
 *
 *--------------------------------------------------------------
 */

char *
Tk_NameOfColor(colorPtr)
    XColor *colorPtr;		/* Color whose name is desired. */
{
    register TkColor *tkColPtr = (TkColor *) colorPtr;
    static char string[20];

    if ((tkColPtr->magic == COLOR_MAGIC)
	    && (tkColPtr->tablePtr == &nameTable)) {
	return ((NameKey *) tkColPtr->hashPtr->key.words)->name;
    }
    sprintf(string, "#%4x%4x%4x", colorPtr->red, colorPtr->green,
	    colorPtr->blue);
    return string;
}

/*
 *----------------------------------------------------------------------
 *
 * Tk_FreeColor --
 *
 *	This procedure is called to release a color allocated by
 *	Tk_GetColor.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The reference count associated with colorPtr is deleted, and
 *	the color is released to X if there are no remaining uses
 *	for it.
 *
 *----------------------------------------------------------------------
 */

void
Tk_FreeColor(colorPtr)
    XColor *colorPtr;		/* Color to be released.  Must have been
				 * allocated by Tk_GetColor or
				 * Tk_GetColorByValue. */
{
    register TkColor *tkColPtr = (TkColor *) colorPtr;
    Visual *visual;
    Screen *screen = tkColPtr->screen;

    /*
     * Do a quick sanity check to make sure this color was really
     * allocated by Tk_GetColor.
     */

    if (tkColPtr->magic != COLOR_MAGIC) {
	panic("Tk_FreeColor called with bogus color");
    }

    tkColPtr->refCount--;
    if (tkColPtr->refCount == 0) {

	/*
	 * Careful!  Don't free black or white, since this will
	 * make some servers very unhappy.
	 */

	visual = Tk_DefaultVisual(screen);
	if ((visual->class != StaticGray) && (visual->class != StaticColor)
		&& (tkColPtr->color.pixel != BlackPixelOfScreen(screen))
		&& (tkColPtr->color.pixel != WhitePixelOfScreen(screen))) {
	    XFreeColors(DisplayOfScreen(screen), tkColPtr->colormap,
		    &tkColPtr->color.pixel, 1, 0L);
	}
	Tcl_DeleteHashEntry(tkColPtr->hashPtr);
	tkColPtr->magic = 0;
	ckfree((char *) tkColPtr);
    }
}

/*
 *----------------------------------------------------------------------
 *
 * ColorInit --
 *
 *	Initialize the structure used for color management.
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
ColorInit()
{
    initialized = 1;
    Tcl_InitHashTable(&nameTable, sizeof(NameKey)/sizeof(int));
    Tcl_InitHashTable(&valueTable, sizeof(ValueKey)/sizeof(int));
    Tcl_InitHashTable(&screenTable, TCL_ONE_WORD_KEYS);
}


int
Tk_IndexOfScreen(Screen *screen)
{
  Display *dpy = DisplayOfScreen(screen);
  int i, nscreens = ScreenCount(dpy);

  for (i = 0; i < nscreens; i++) {
    if (screen == ScreenOfDisplay(dpy, i))
      return (i);
  }
  return (DefaultScreen(dpy));
}


VisInfo *
Tk_VisInfo(Screen *screen)
{
    Tcl_HashEntry *hashPtr;
    VisInfo *info;
    XVisualInfo vTemplate;
    XVisualInfo *visualList;
    Visual *visual;
    XGCValues values;
    int visualsMatched, scrnum, new;

    if (!initialized) {
	ColorInit();
    }

    hashPtr = Tcl_CreateHashEntry(&screenTable, (char *) screen, &new);
    if (!new) {
	info = (VisInfo *) Tcl_GetHashValue(hashPtr);
    } else {
	info = (VisInfo *) ckalloc(sizeof(VisInfo));
	info->screen = screen;

/* Workaround to support non-default visuals */
#if 0
	info->visual = XDefaultVisualOfScreen(screen);
	info->depth = XDefaultDepthOfScreen(screen);
	info->colormap = XDefaultColormapOfScreen(screen);
	info->gc = DefaultGCOfScreen(screen);
#else
	scrnum = Tk_IndexOfScreen(screen);
	vTemplate.screen = scrnum;
	vTemplate.depth = 8;
	vTemplate.class = PseudoColor;
	visualList =
	  XGetVisualInfo(DisplayOfScreen(screen),
			 VisualScreenMask |
			 VisualDepthMask |
			 VisualClassMask,
			 &vTemplate, &visualsMatched);
	if (visualsMatched > 0) {
	    info->visual = visualList[0].visual;
	    info->depth = 8;
	    info->pixmap = XCreatePixmap(screen->display,
					 RootWindowOfScreen(screen),
					 1, 1, 8);
//fprintf(stderr, "TK_CreateColormap %d\n", TK_CreateColormap);
	    if ((TK_CreateColormap == 0) &&
		(info->visual == DefaultVisualOfScreen(screen))) {
	      info->colormap = DefaultColormapOfScreen(screen);
	      info->gc = DefaultGCOfScreen(screen);
	    } else {
	      info->colormap =
		XCreateColormap(screen->display,
				RootWindowOfScreen(screen),
				info->visual, AllocNone);
	      info->gc =
		XCreateGC(screen->display,
			  info->pixmap, 0, &values);
	    }
	} else {
	    info->visual = XDefaultVisualOfScreen(screen);
	    info->depth = XDefaultDepthOfScreen(screen);
	    info->pixmap = XCreatePixmap(screen->display,
					 RootWindowOfScreen(screen),
					 1, 1, info->depth);
	    info->colormap = XDefaultColormapOfScreen(screen);
	    info->gc = DefaultGCOfScreen(screen);
	}

	XFree((char *)visualList);
#endif

	Tcl_SetHashValue(hashPtr, info);
    }

    return (info);
}


int
Tk_DefaultDepth(Screen *screen)
{
    return (Tk_VisInfo(screen)->depth);
}


Visual *
Tk_DefaultVisual(Screen *screen)
{
    return (Tk_VisInfo(screen)->visual);
}


Colormap
Tk_DefaultColormap(Screen *screen)
{
    return (Tk_VisInfo(screen)->colormap);
}


Window
Tk_DefaultRootWindow(Display *dpy)
{
    return (DefaultRootWindow(dpy));
}


GC
Tk_DefaultGC(Screen *screen)
{
  return (Tk_VisInfo(screen)->gc);
}


Pixmap
Tk_DefaultPixmap(Screen *screen)
{
  return (Tk_VisInfo(screen)->pixmap);
}


