/* 
 * tkBitmap.c --
 *
 *	This file maintains a database of read-only bitmaps for the Tk
 *	toolkit.  This allows bitmaps to be shared between widgets and
 *	also avoids interactions with the X server.
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
static char rcsid[] = "$Header: /user6/ouster/wish/RCS/tkBitmap.c,v 1.16 92/08/24 09:45:43 ouster Exp $ SPRITE (Berkeley)";
#endif /* not lint */

#include "tkconfig.h"
#include "tk.h"

/*
 * The includes below are for pre-defined bitmaps.
 */

#include "bitmaps/gray50"
#include "bitmaps/gray25"

/*
 * One of the following data structures exists for each bitmap that is
 * currently in use.  Each structure is indexed with both "idTable" and
 * "nameTable".
 */

typedef struct {
    Pixmap bitmap;		/* X identifier for bitmap.  None means this
				 * bitmap was created by Tk_DefineBitmap
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
 * for the bitmap.  This table is indexed by Bitmap ids, and is used by
 * Tk_FreeBitmap.
 */

static Tcl_HashTable idTable;

/*
 * For each call to Tk_DefineBitmap one of the following structures is
 * created to hold information about the bitmap.
 */

typedef struct {
    char *source;		/* Bits for bitmap. */
    unsigned int width, height;	/* Dimensions of bitmap. */
} PredefBitmap;

/*
 * Hash table create by Tk_DefineBitmap to map from a name to a
 * collection of in-core data about a bitmap.  The table is
 * indexed by the address of the data for the bitmap, and the entries
 * contain pointers to PredefBitmap structures.
 */

static Tcl_HashTable predefTable;

/*
 * Hash table used by Tk_GetBitmapFromData to map from a collection
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

static void		BitmapInit _ANSI_ARGS_((void));

/*
 *----------------------------------------------------------------------
 *
 * Tk_GetBitmap --
 *
 *	Given a string describing a bitmap, locate (or create if necessary)
 *	a bitmap that fits the description.
 *
 * Results:
 *	The return value is the X identifer for the desired bitmap
 *	(i.e. a Pixmap with a single plane), unless string couldn't be
 *	parsed correctly.  In this case, None is returned and an error
 *	message is left in interp->result.  The caller should never
 *	modify the bitmap that is returned, and should eventually call
 *	Tk_FreeBitmap when the bitmap is no longer needed.
 *
 * Side effects:
 *	The bitmap is added to an internal database with a reference count.
 *	For each call to this procedure, there should eventually be a call
 *	to Tk_FreeBitmap, so that the database can be cleaned up when bitmaps
 *	aren't needed anymore.
 *
 *----------------------------------------------------------------------
 */

Pixmap
Tk_GetBitmap(interp, tkwin, string)
    Tcl_Interp *interp;		/* Interpreter to use for error reporting. */
    Tk_Window tkwin;		/* Window in which bitmap will be used. */
    Tk_Uid string;		/* Description of bitmap.  See manual entry
				 * for details on legal syntax. */
{
    NameKey key;
    Tcl_HashEntry *nameHashPtr, *idHashPtr, *predefHashPtr;
    register TkBitmap *bitmapPtr;
    PredefBitmap *predefPtr;
    int new;
    Pixmap bitmap;
    unsigned int width, height;
    int dummy2;

    if (!initialized) {
	BitmapInit();
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
     * defined by a call to Tk_DefineBitmap.
     */

    if (*string == '@') {
	string = Tcl_TildeSubst(interp, string + 1);
	if (string == NULL) {
	    goto error;
	}
	if (XReadBitmapFile(Tk_Display(tkwin),
			    RootWindowOfScreen(Tk_Screen(tkwin)),
		string, &width, &height, &bitmap, &dummy2, &dummy2)
		!= BitmapSuccess) {
	    Tcl_AppendResult(interp, "error reading bitmap file \"", string,
		    "\"", (char *) NULL);
	    goto error;
	}
    } else {
	predefHashPtr = Tcl_FindHashEntry(&predefTable, string);
	if (predefHashPtr == NULL) {
	    Tcl_AppendResult(interp, "bitmap \"", string,
		    "\" not defined", (char *) NULL);
	    goto error;
	}
	predefPtr = (PredefBitmap *) Tcl_GetHashValue(predefHashPtr);
	width = predefPtr->width;
	height = predefPtr->height;
	bitmap = XCreateBitmapFromData(Tk_Display(tkwin),
		RootWindowOfScreen(Tk_Screen(tkwin)), predefPtr->source,
		width, height);
    }

    /*
     * Add information about this bitmap to our database.
     */

    bitmapPtr = (TkBitmap *) ckalloc(sizeof(TkBitmap));
    bitmapPtr->bitmap = bitmap;
    bitmapPtr->width = width;
    bitmapPtr->height = height;
    bitmapPtr->display = Tk_Display(tkwin);
    bitmapPtr->refCount = 1;
    bitmapPtr->hashPtr = nameHashPtr;
    idHashPtr = Tcl_CreateHashEntry(&idTable, (char *) bitmapPtr->bitmap,
	    &new);
    if (!new) {
/* deh patched to support multiple displays */
/*	panic("bitmap already registered in Tk_GetBitmap"); */
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
 * Tk_DefineBitmap --
 *
 *	This procedure associates a textual name with a binary bitmap
 *	description, so that the name may be used to refer to the
 *	bitmap in future calls to Tk_GetBitmap.
 *
 * Results:
 *	A standard Tcl result.  If an error occurs then TCL_ERROR is
 *	returned and a message is left in interp->result.
 *
 * Side effects:
 *	"Name" is entered into the bitmap table and may be used from
 *	here on to refer to the given bitmap.
 *
 *----------------------------------------------------------------------
 */

int
Tk_DefineBitmap(interp, name, source, width, height)
    Tcl_Interp *interp;		/* Interpreter to use for error reporting. */
    Tk_Uid name;		/* Name to use for bitmap.  Must not already
				 * be defined as a bitmap. */
    char *source;		/* Address of bits for bitmap. */
    unsigned int width;		/* Width of bitmap. */
    unsigned int height;	/* Height of bitmap. */
{
    int new;
    Tcl_HashEntry *predefHashPtr;
    PredefBitmap *predefPtr;

    if (!initialized) {
	BitmapInit();
    }

    predefHashPtr = Tcl_CreateHashEntry(&predefTable, name, &new);
    if (!new) {
        Tcl_AppendResult(interp, "bitmap \"", name,
		"\" is already defined", (char *) NULL);
	return TCL_ERROR;
    }
    predefPtr = (PredefBitmap *) malloc(sizeof(PredefBitmap));
    predefPtr->source = source;
    predefPtr->width = width;
    predefPtr->height = height;
    Tcl_SetHashValue(predefHashPtr, predefPtr);
    return TCL_OK;
}

/*
 *--------------------------------------------------------------
 *
 * Tk_NameOfBitmap --
 *
 *	Given a bitmap, return a textual string identifying the
 *	bitmap.
 *
 * Results:
 *	The return value is the string name associated with bitmap.
 *
 * Side effects:
 *	None.
 *
 *--------------------------------------------------------------
 */

Tk_Uid
Tk_NameOfBitmap(bitmap)
    Pixmap bitmap;			/* Bitmap whose name is wanted. */
{
    Tcl_HashEntry *idHashPtr;
    TkBitmap *bitmapPtr;

    if (!initialized) {
	unknown:
	panic("Tk_NameOfBitmap received unknown bitmap argument");
    }

    idHashPtr = Tcl_FindHashEntry(&idTable, (char *) bitmap);
    if (idHashPtr == NULL) {
	goto unknown;
    }
    bitmapPtr = (TkBitmap *) Tcl_GetHashValue(idHashPtr);
    return ((NameKey *) bitmapPtr->hashPtr->key.words)->name;
}

/*
 *--------------------------------------------------------------
 *
 * Tk_SizeOfBitmap --
 *
 *	Given a bitmap managed by this module, returns the width
 *	and height of the bitmap..
 *
 * Results:
 *	The words at *widthPtr and *heightPtr are filled in with
 *	the dimenstions of bitmap.
 *
 * Side effects:
 *	If bitmap isn't managed by this module then the procedure
 *	panics..
 *
 *--------------------------------------------------------------
 */

void
Tk_SizeOfBitmap(bitmap, widthPtr, heightPtr)
    Pixmap bitmap;			/* Bitmap whose size is wanted. */
    unsigned int *widthPtr;		/* Store bitmap width here. */
    unsigned int *heightPtr;		/* Store bitmap height here. */
{
    Tcl_HashEntry *idHashPtr;
    TkBitmap *bitmapPtr;

    if (!initialized) {
	unknownBitmap:
	panic("Tk_SizeOfBitmap received unknown bitmap argument");
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
 * Tk_FreeBitmap --
 *
 *	This procedure is called to release a bitmap allocated by
 *	Tk_GetBitmap or TkGetBitmapFromData.
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
Tk_FreeBitmap(bitmap)
    Pixmap bitmap;			/* Bitmap to be released. */
{
    Tcl_HashEntry *idHashPtr;
    register TkBitmap *bitmapPtr;

    if (!initialized) {
	panic("Tk_FreeBitmap called before Tk_GetBitmap");
    }

    idHashPtr = Tcl_FindHashEntry(&idTable, (char *) bitmap);
    if (idHashPtr == NULL) {
	panic("Tk_FreeBitmap received unknown bitmap argument");
    }
    bitmapPtr = (TkBitmap *) Tcl_GetHashValue(idHashPtr);
    bitmapPtr->refCount--;
    if (bitmapPtr->refCount == 0) {
	XFreePixmap(bitmapPtr->display, bitmapPtr->bitmap);
	Tcl_DeleteHashEntry(idHashPtr);
	Tcl_DeleteHashEntry(bitmapPtr->hashPtr);
	ckfree((char *) bitmapPtr);
    }
}

/*
 *----------------------------------------------------------------------
 *
 * Tk_GetBitmapFromData --
 *
 *	Given a description of the bits for a bitmap, make a bitmap that
 *	has the given properties.
 *
 * Results:
 *	The return value is the X identifer for the desired bitmap
 *	(a one-plane Pixmap), unless it couldn't be created properly.
 *	In this case, None is returned and an error message is left in
 *	interp->result.  The caller should never modify the bitmap that
 *	is returned, and should eventually call Tk_FreeBitmap when the
 *	bitmap is no longer needed.
 *
 * Side effects:
 *	The bitmap is added to an internal database with a reference count.
 *	For each call to this procedure, there should eventually be a call
 *	to Tk_FreeBitmap, so that the database can be cleaned up when bitmaps
 *	aren't needed anymore.
 *
 *----------------------------------------------------------------------
 */

	/* ARGSUSED */
Pixmap
Tk_GetBitmapFromData(interp, tkwin, source, width, height)
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
	BitmapInit();
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
	if (Tk_DefineBitmap(interp, name, source, width, height) != TCL_OK) {
	    Tcl_DeleteHashEntry(dataHashPtr);
	    return TCL_ERROR;
	}
    }
    return Tk_GetBitmap(interp, tkwin, name);
}

/*
 *----------------------------------------------------------------------
 *
 * BitmapInit --
 *
 *	Initialize the structures used for bitmap management.
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
BitmapInit()
{
    Tcl_Interp *dummy;

    dummy = Tcl_CreateInterp();
    initialized = 1;
    Tcl_InitHashTable(&nameTable, sizeof(NameKey)/sizeof(int));
    Tcl_InitHashTable(&dataTable, sizeof(DataKey)/sizeof(int));
    Tcl_InitHashTable(&predefTable, TCL_ONE_WORD_KEYS);
    Tcl_InitHashTable(&idTable, TCL_ONE_WORD_KEYS);

    Tk_DefineBitmap(dummy, Tk_GetUid("gray50"), (char *)gray50_bits,
		    gray50_width, gray50_height);
    Tk_DefineBitmap(dummy, Tk_GetUid("gray25"),
		    (char *)gray25_bits, gray25_width, gray25_height);
    Tcl_DeleteInterp(dummy);
}
