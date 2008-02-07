/* 
 * tkPixmap.c --
 *
 *	This file maintains a database of read-only pixmaps for the Tk
 *	toolkit.  This allows pixmaps to be shared between widgets and
 *	also avoids interactions with the X server.
 *
 * Copyright 1992 by Sven Delmas
 *
 * This source is based upon the file tkBitmap.c from:
 *
 * Copyright 1990-1992 Regents of the University of California
 * Permission to use, copy, modify, and distribute this
 * software and its documentation for any purpose and without
 * fee is hereby granted, provided that the above copyright
 * notice appear in all copies.  The University of California
 * makes no representations about the suitability of this
 * software for any purpose.  It is provided "as is" without
 * express or implied warranty.
 *   file: /user6/ouster/wish/RCS/tkBitmap.c,v 1.16
 *           92/08/24 09:45:43 ouster Exp $ SPRITE (Berkeley);
 */

#if defined(USE_XPM3)

#ifndef lint
static char *AtFSid = "$Header: tkPixmap.c[1.1] Mon Sep 28 14:12:35 1992 garfield@cs.tu-berlin.de frozen $";
#endif /* not lint */

#include "tkconfig.h"
#include "tk.h"

/*
 * The includes below are for pre-defined bitmaps.
 */

#include "bitmaps/gray50"
#include "bitmaps/gray25"

/*
 * Include the xpm 3 defines for color pixmaps
 */
//#include "xpmtk.h"
#include <xpm.h>

/*
 * One of the following data structures exists for each bitmap that is
 * currently in use.  Each structure is indexed with both "idTable" and
 * "nameTable".
 */

typedef struct {
    Pixmap bitmap;		/* X identifier for bitmap.  None means this
				 * bitmap was created by Tk_DefinePixmap
				 * and it isn't currently in use. */
    unsigned int width, height;	/* Dimensions of bitmap. */
    Display *display;		/* Display for which bitmap is valid. */
    int refCount;		/* Number of active uses of bitmap. */
    Tcl_HashEntry *hashPtr;	/* Entry in nameTable for this structure
				 * (needed when deleting). */
} TkBitmap;

/*
 * Hash table to map from a textual description of a bitmap to the
 * TkBitmap record for the bitmap, and key structure used in that
 * hash table:
 */

static Tcl_HashTable nameTable;
typedef struct {
    Tk_Uid name;		/* Textual name for desired bitmap. */
    Screen *screen;		/* Screen for which bitmap will be used. */
} NameKey;

/*
 * Hash table that maps from bitmap identifiers to the TkBitmap structure
 * for the pixmap.  This table is indexed by Bitmap ids, and is used by
 * Tk_FreePixmap.
 */

static Tcl_HashTable idTable;

/*
 * For each call to Tk_DefinePixmap one of the following structures is
 * created to hold information about the pixmap.
 */

typedef struct {
    char *source;		/* Bits for bitmap. */
    Pixmap pixmap;              /* Pre-defined pixmap */
    unsigned int width, height;	/* Dimensions of bitmap. */
} PredefBitmap;

/*
 * Hash table create by Tk_DefinePixmap to map from a name to a
 * collection of in-core data about a bitmap.  The table is
 * indexed by the address of the data for the bitmap, and the entries
 * contain pointers to PredefBitmap structures.
 */

static Tcl_HashTable predefTable;

/*
 * Hash table used by Tk_GetPixmapFromData to map from a collection
 * of in-core data about a bitmap to a Tk_Uid giving an automatically-
 * generated name for the bitmap:
 */

static Tcl_HashTable dataTable;
typedef struct {
    char *source;		/* Bitmap bits. */
    unsigned int width, height;	/* Dimensions of bitmap. */
} DataKey;

static int initialized = 0;	/* 0 means static structures haven't been
				 * initialized yet. */

/*
 * Forward declarations for procedures defined in this file:
 */

static void		PixmapInit _ANSI_ARGS_((Tk_Window tkwin));

/*
 *----------------------------------------------------------------------
 *
 * Tk_GetPixmap --
 *
 *	Given a string describing a bitmap or pixmap, locate (or create
 *      if necessary) a pixmap that fits the description.
 *
 * Results:
 *	The return value is the X identifer for the desired pixmap,
 *      unless string couldn't be parsed correctly.  In this case,
 *      None is returned and an error message is left in
 *      interp->result.  The caller should never modify the pixmap
 *      that is returned, and should eventually call Tk_FreePixmap
 *      when the pixmap is no longer needed.
 *
 * Side effects:
 *	The pixmap is added to an internal database with a reference count.
 *	For each call to this procedure, there should eventually be a call
 *	to Tk_FreePixmap, so that the database can be cleaned up when pixmaps
 *	aren't needed anymore.
 *
 *----------------------------------------------------------------------
 */
Pixmap
Tk_GetPixmap(interp, tkwin, string)
    Tcl_Interp *interp;		/* Interpreter to use for error reporting. */
    Tk_Window tkwin;		/* Window in which pixmap will be used. */
    Tk_Uid string;		/* Description of pixmap.  See manual entry
				 * for details on legal syntax. */
{
    NameKey key;
    Tcl_HashEntry *nameHashPtr, *idHashPtr, *predefHashPtr;
    register TkBitmap *bitmapPtr;
    PredefBitmap *predefPtr;
    int new;
    Pixmap bitmap;
    Pixmap bitmap_shape;
    unsigned int width, height;
    int dummy2;
    XpmAttributes xpm_attributes;
    Display *dpy = Tk_Display(tkwin);

    if (!initialized) {
	PixmapInit(tkwin);
    }

    key.name = string;
    key.screen = Tk_Screen(tkwin);
    nameHashPtr = Tcl_CreateHashEntry(&nameTable, (char *) &key, &new);
    if (!new) {
	bitmapPtr = (TkBitmap *) Tcl_GetHashValue(nameHashPtr);
	bitmapPtr->refCount++;
	return bitmapPtr->bitmap;
    }

    /*
     * No suitable bitmap exists.  Create a new bitmap from the
     * information contained in the string.  If the string starts
     * with "@" then the rest of the string is a file name containing
     * the bitmap.  Otherwise the string must refer to a bitmap
     * defined by a call to Tk_DefinePixmap.
     */

    if (*string == '@') {
	string = Tcl_TildeSubst(interp, string + 1);
	if (string == NULL) {
	    goto error;
	}

	if (XReadBitmapFile(dpy, RootWindowOfScreen(Tk_Screen(tkwin)),
		string, &width, &height, &bitmap_shape, &dummy2, &dummy2)
		!= BitmapSuccess)
	  {
	    xpm_attributes.visual = Tk_DefaultVisual(Tk_Screen(tkwin));
	    xpm_attributes.colormap = Tk_DefaultColormap(Tk_Screen(tkwin));
	    xpm_attributes.depth = Tk_DefaultDepth(Tk_Screen(tkwin));
	    xpm_attributes.valuemask = XpmVisual | XpmColormap | XpmDepth;

	    if (XpmReadFileToPixmap(dpy, RootWindowOfScreen(Tk_Screen(tkwin)),
				    string, &bitmap, &bitmap_shape,
				    &xpm_attributes) != BitmapSuccess) {
	        Tcl_AppendResult(interp, "error reading bitmap file \"",
			       string, "\"", (char *) NULL);
	        goto error;
	    } {
                width = xpm_attributes.width;
                height = xpm_attributes.height;
		XpmFreeAttributes(&xpm_attributes);
	    }
	} else {
            bitmap = XCreatePixmap(dpy, RootWindowOfScreen(Tk_Screen(tkwin)),
				   width, height,
				   Tk_DefaultDepth(Tk_Screen(tkwin)));
	    XCopyPlane(dpy, bitmap_shape, bitmap,
		       Tk_DefaultGC(Tk_Screen(tkwin)),
		       0, 0, width, height, 0, 0, 1);
	}
    } else {
        /* first try for a display-specific version */
	predefHashPtr = Tcl_FindHashEntry(&predefTable, (char *) &key);
	if (predefHashPtr == NULL) {
            /* try for a non-display specific version */
	    key.screen = (Screen*) NULL;
	    predefHashPtr = Tcl_FindHashEntry(&predefTable, (char *) &key);
	    if (predefHashPtr == NULL) {
	        /* give up */
	        Tcl_AppendResult(interp, "pixmap \"", string,
				 "\" not defined", (char *) NULL);
		goto error;
	    }
	}
	predefPtr = (PredefBitmap *) Tcl_GetHashValue(predefHashPtr);
	width = predefPtr->width;
	height = predefPtr->height;
	if (predefPtr->source != NULL) {
	    bitmap_shape =
	        XCreateBitmapFromData(dpy, Tk_DefaultRootWindow(dpy),
				      predefPtr->source, width, height);
	    bitmap = XCreatePixmap(dpy, RootWindowOfScreen(Tk_Screen(tkwin)),
				   width, height,
				   Tk_DefaultDepth(Tk_Screen(tkwin)));
	    XCopyPlane(dpy, bitmap_shape, bitmap,
		       Tk_DefaultGC(Tk_Screen(tkwin)),
		       0, 0, width, height, 0, 0, 1);
	} else {
	    if (predefPtr->pixmap != None) {
	        bitmap = predefPtr->pixmap;
	    } else {
	      bitmap =
		  XCreatePixmap(dpy, RootWindowOfScreen(Tk_Screen(tkwin)),
			        width, height,
				Tk_DefaultDepth(Tk_Screen(tkwin)));
	    }
        }
    }

    /*
     * Add information about this bitmap to our database.
     */

    bitmapPtr = (TkBitmap *) ckalloc(sizeof(TkBitmap));
    bitmapPtr->bitmap = bitmap;
    bitmapPtr->width = width;
    bitmapPtr->height = height;
    bitmapPtr->display = dpy;
    bitmapPtr->refCount = 1;
    bitmapPtr->hashPtr = nameHashPtr;
    idHashPtr = Tcl_CreateHashEntry(&idTable, (char *) bitmapPtr->bitmap,
	    &new);
    if (!new) {
/* deh patched to support multiple displays */
/*	panic("pixmap already registered in Tk_GetPixmap"); */
        bitmapPtr->refCount = 1000;
    }
    Tcl_SetHashValue(nameHashPtr, bitmapPtr);
    Tcl_SetHashValue(idHashPtr, bitmapPtr);
    return bitmapPtr->bitmap;

    error:
    Tcl_DeleteHashEntry(nameHashPtr);
    return None;
}

/*
 *----------------------------------------------------------------------
 *
 * Tk_DefinePixmap --
 *
 *	This procedure associates a textual name with a binary pixmap
 *	description, so that the name may be used to refer to the
 *	pixmap in future calls to Tk_GetPixmap. The pixmap can
 *      be pre-created by the user, or can be created later
 *      by Tk_GetPixmap().  Since pixmaps are display-specific,
 *      a user supplied pixmap will be associated with tkwin's display.
 *      If pixmap is given as None, then a new pixmap will be created
 *      by Tk_GetPixmap for each unique display.
 *
 * Results:
 *	A standard Tcl result.  If an error occurs then TCL_ERROR is
 *	returned and a message is left in interp->result.
 *
 * Side effects:
 *	"Name" is entered into the pixmap table and may be used from
 *	here on to refer to the given pixmap.
 *
 *----------------------------------------------------------------------
 */

int
Tk_DefinePixmap(interp, name, tkwin, pixmap, source, width, height)
    Tcl_Interp *interp;		/* Interpreter to use for error reporting. */
    Tk_Uid name;		/* Name to use for bitmap.  Must not already
				 * be defined as a bitmap. */
    Tk_Window tkwin;            /* any window on screen where pixmap lives */
    Pixmap pixmap;              /* pixmap to associate with name,
				 * or None to create a new pixmap */
    char *source;		/* Address of bits for bitmap. */
    unsigned int width;		/* Width of bitmap. */
    unsigned int height;	/* Height of bitmap. */
{
    int new;
    Tcl_HashEntry *predefHashPtr;
    PredefBitmap *predefPtr;
    NameKey key;

    if (!initialized) {
	PixmapInit(tkwin);
    }

    key.name = name;
    key.screen = (pixmap != None) ? Tk_Screen(tkwin) : (Screen *)NULL;
    predefHashPtr = Tcl_CreateHashEntry(&predefTable, (char *) &key, &new);
    if (!new) {
        Tcl_AppendResult(interp, "bitmap \"", name,
		"\" is already defined", (char *) NULL);
	return TCL_ERROR;
    }
    predefPtr = (PredefBitmap *) malloc(sizeof(PredefBitmap));
    predefPtr->source = source;
    predefPtr->pixmap = pixmap;
    predefPtr->width = width;
    predefPtr->height = height;
    Tcl_SetHashValue(predefHashPtr, predefPtr);
    return TCL_OK;
}

/*
 *--------------------------------------------------------------
 *
 * Tk_NameOfPixmap --
 *
 *	Given a pixmap, return a textual string identifying the
 *	pixmap.
 *
 * Results:
 *	The return value is the string name associated with pixmap.
 *
 * Side effects:
 *	None.
 *
 *--------------------------------------------------------------
 */

Tk_Uid
Tk_NameOfPixmap(bitmap)
    Pixmap bitmap;			/* Bitmap whose name is wanted. */
{
    Tcl_HashEntry *idHashPtr;
    TkBitmap *bitmapPtr;

    if (!initialized) {
	unknown:
	panic("Tk_NameOfPixmap received unknown pixmap argument");
    }

    idHashPtr = Tcl_FindHashEntry(&idTable, (char *) bitmap);
    if (idHashPtr == NULL) {
	goto unknown;
    }
    bitmapPtr = (TkBitmap *) Tcl_GetHashValue(idHashPtr);
    if (bitmapPtr->hashPtr == NULL) {
        /* the bitmap has been un-defined */
        return Tk_GetUid("UndefinedBitmap");
    }
    return ((NameKey *) bitmapPtr->hashPtr->key.words)->name;
}

/*
 *--------------------------------------------------------------
 *
 * Tk_SizeOfPixmap --
 *
 *	Given a pixmap managed by this module, returns the width
 *	and height of the pixmap..
 *
 * Results:
 *	The words at *widthPtr and *heightPtr are filled in with
 *	the dimenstions of pixmap.
 *
 * Side effects:
 *	If pixmap isn't managed by this module then the procedure
 *	panics..
 *
 *--------------------------------------------------------------
 */

void
Tk_SizeOfPixmap(bitmap, widthPtr, heightPtr)
    Pixmap bitmap;			/* Bitmap whose size is wanted. */
    unsigned int *widthPtr;		/* Store bitmap width here. */
    unsigned int *heightPtr;		/* Store bitmap height here. */
{
    Tcl_HashEntry *idHashPtr;
    TkBitmap *bitmapPtr;

    if (!initialized) {
	unknownBitmap:
	panic("Tk_SizeOfPixmap received unknown pixmap argument");
    }

    idHashPtr = Tcl_FindHashEntry(&idTable, (char *) bitmap);
    if (idHashPtr == NULL) {
	goto unknownBitmap;
    }
    bitmapPtr = (TkBitmap *) Tcl_GetHashValue(idHashPtr);
    *widthPtr = bitmapPtr->width;
    *heightPtr = bitmapPtr->height;
}

/*
 *----------------------------------------------------------------------
 *
 * Tk_FreePixmap --
 *
 *	This procedure is called to release a pixmap allocated by
 *	Tk_GetPixmap.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The reference count associated with bitmap is decremented, and
 *	it is officially deallocated if no-one is using it anymore.
 *
 *----------------------------------------------------------------------
 */

void
Tk_FreePixmap(bitmap)
    Pixmap bitmap;			/* Bitmap to be released. */
{
    Tcl_HashEntry *idHashPtr;
    register TkBitmap *bitmapPtr;

    if (!initialized) {
	panic("Tk_FreePixmap called before Tk_GetPixmap");
    }

    idHashPtr = Tcl_FindHashEntry(&idTable, (char *) bitmap);
    if (idHashPtr == NULL) {
	panic("Tk_FreePixmap received unknown pixmap argument");
    }
    bitmapPtr = (TkBitmap *) Tcl_GetHashValue(idHashPtr);
    bitmapPtr->refCount--;
    if (bitmapPtr->refCount == 0) {
	XFreePixmap(bitmapPtr->display, bitmapPtr->bitmap);
	Tcl_DeleteHashEntry(idHashPtr);
        if (bitmapPtr->hashPtr != NULL) {
            /* If hashPtr is NULL, the bitmap has been undefined,
             * and already removed from the name table */
            Tcl_DeleteHashEntry(bitmapPtr->hashPtr);
        }
	ckfree((char *) bitmapPtr);
    }
}

/*
 *----------------------------------------------------------------------
 *
 * Tk_GetPixmapFromData --
 *
 *	Given a description of the bits for a pixmap, make a pixmap that
 *	has the given properties.
 *
 * Results:
 *	The return value is the X identifer for the desired pixmap,
 *	unless it couldn't be created properly. In this case, None is
 *      returned and an error message is left in interp->result.  The
 *      caller should never modify the bitmap that is returned, and
 *      should eventually call Tk_FreePixmap when the pixmap is no
 *      longer needed.
 *
 * Side effects:
 *	The pixmap is added to an internal database with a reference count.
 *	For each call to this procedure, there should eventually be a call
 *	to Tk_FreePixmap, so that the database can be cleaned up when pixmaps
 *	aren't needed anymore.
 *
 *----------------------------------------------------------------------
 */

	/* ARGSUSED */
Pixmap
Tk_GetPixmapFromData(interp, tkwin, source, width, height)
    Tcl_Interp *interp;		/* Interpreter to use for error reporting. */
    Tk_Window tkwin;		/* Window in which bitmap will be used. */
    char *source;		/* Bitmap data for bitmap shape. */
    unsigned int width, height;	/* Dimensions of bitmap. */
{
    DataKey key;
    Tcl_HashEntry *dataHashPtr;
    Tk_Uid name = NULL;		/* Initialization need only to prevent
				 * compiler warning. */
    int new;
    static autoNumber = 0;
    char string[20];

    if (!initialized) {
	PixmapInit(tkwin);
    }

    key.source = source;
    key.width = width;
    key.height = height;
    dataHashPtr = Tcl_CreateHashEntry(&dataTable, (char *) &key, &new);
    if (!new) {
	name = (Tk_Uid) Tcl_GetHashValue(dataHashPtr);
    } else {
	autoNumber++;
	sprintf(string, "_tk%d", autoNumber);
	name = Tk_GetUid(string);
	Tcl_SetHashValue(dataHashPtr, name);
	if (Tk_DefinePixmap(interp, name, tkwin, None, source, width,
			    height) != TCL_OK) {
	    Tcl_DeleteHashEntry(dataHashPtr);
	    return TCL_ERROR;
	}
    }
    return Tk_GetPixmap(interp, tkwin, name);
}

/*
 *----------------------------------------------------------------------
 *
 * PixmapInit --
 *
 *	Initialize the structures used for pixmap management.
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
PixmapInit(tkwin)
    Tk_Window tkwin;          /* any window on screen where pixmap lives */
{
    Tcl_Interp *dummy;

    dummy = Tcl_CreateInterp();
    initialized = 1;
    Tcl_InitHashTable(&nameTable, sizeof(NameKey)/sizeof(int));
    Tcl_InitHashTable(&dataTable, sizeof(DataKey)/sizeof(int));
    Tcl_InitHashTable(&predefTable, sizeof(NameKey)/sizeof(int));
    Tcl_InitHashTable(&idTable, TCL_ONE_WORD_KEYS);

    Tk_DefinePixmap(dummy, Tk_GetUid("gray50"), tkwin, None,
		    (char *)gray50_bits, gray50_width, gray50_height);
    Tk_DefinePixmap(dummy, Tk_GetUid("gray25"), tkwin, None,
		    (char *)gray25_bits, gray25_width, gray25_height);
    Tcl_DeleteInterp(dummy);
}

/*
 *----------------------------------------------------------------------
 *
 * Tk_UndefinePixmap --
 *
 *	This procedure removes any association "name" with
 *	a bitmap or pixmap.  This can be used to undefine
 *	names defined by either Tk_DefinePixmap or Tk_DefinePixmap.
 *	If tkwin is NULL, only display-independent pixmaps will
 *	be removed, otherwise both display-independent and 
 *	the pixmap associated with tkwin will be undefined.
 *
 * Results:
 *	A standard Tcl result.  If an error occurs then TCL_ERROR is
 *	returned and a message is left in interp->result.
 *
 * Side effects:
 *	"Name" is removed from the predef table.
 *
 *----------------------------------------------------------------------
 */

int
Tk_UndefinePixmap(interp, name, tkwin)
    Tcl_Interp *interp;		/* Interpreter to use for error reporting. */
    Tk_Uid name;		/* Name of bitmap/pixmap to undefine */
    Tk_Window tkwin;            /* any window on screen where pixmap lives */
{
    NameKey key;
    Tcl_HashEntry *predefHashPtr, *nameHashPtr;
    TkBitmap *bitmapPtr;
    int cnt = 0;

    if (!initialized) {
	PixmapInit(tkwin);
    }

    key.name = name;
    key.screen = (Screen *) NULL;
    predefHashPtr = Tcl_FindHashEntry(&predefTable, (char*)&key);
    if (predefHashPtr != NULL) {
	++cnt;
        Tcl_DeleteHashEntry(predefHashPtr);
    }

    key.screen = Tk_Screen(tkwin);
    predefHashPtr = Tcl_FindHashEntry(&predefTable, (char*)&key);
    if (predefHashPtr != NULL) {
	++cnt;
        Tcl_DeleteHashEntry(predefHashPtr);
    }
    if (cnt == 0) {
        Tcl_AppendResult(interp, "pixmap \"", name,
		"\" doesn't exist", (char *) NULL);
	return TCL_ERROR;
    }

    /*
     * Remove it from the name table if it is there (it might not
     * have been created yet, it which case we wont find it).  We
     * delete it from the name table and mark the hashPtr as NULL
     * so that we know it has been deleted.  The pixmap still exists,
     * and well later be freed and removed from idTable by Tk_FreeBitmap().
     */
    nameHashPtr = Tcl_FindHashEntry(&nameTable, (char *) &key);
    if (nameHashPtr != NULL) {
	bitmapPtr = (TkBitmap *) Tcl_GetHashValue(nameHashPtr);
	/* assert(nameHashPtr == bitmapPtr->hashPtr); */
	/* assert(bitmapPtr->refCount > 0); */
	bitmapPtr->hashPtr = (Tcl_HashEntry*) NULL;
    	Tcl_DeleteHashEntry(nameHashPtr);
    }
    return TCL_OK;
}

#endif
