/* 
 * tkAtom.c --
 *
 *	This file manages a cache of X Atoms in order to avoid
 *	interactions with the X server.  It's much like the Xmu
 *	routines, except it has a cleaner interface (caller
 *	doesn't have to provide permanent storage for atom names,
 *	for example).
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
static char rcsid[] = "$Header: /user6/ouster/wish/RCS/tkAtom.c,v 1.6 92/05/07 09:51:06 ouster Exp $ SPRITE (Berkeley)";
#endif

#include "tkconfig.h"
#include "tkint.h"

/*
 * Forward references to procedures defined in this file:
 */

static void	AtomInit _ANSI_ARGS_((TkDisplay *dispPtr));

/*
 *--------------------------------------------------------------
 *
 * Tk_InternAtom --
 *
 *	Given a string, produce the equivalent X atom.  This
 *	procedure is equivalent to XInternAtom, except that it
 *	keeps a local cache of atoms.  Once a name is known,
 *	the server need not be contacted again for that name.
 *
 * Results:
 *	The return value is the Atom corresponding to name.
 *
 * Side effects:
 *	A new entry may be added to the local atom cache.
 *
 *--------------------------------------------------------------
 */

Atom
Tk_InternAtom(tkwin, name)
    Tk_Window tkwin;		/* Window token;  map name to atom
				 * for this window's display. */
    char *name;			/* Name to turn into atom. */
{
    register TkDisplay *dispPtr;
    register Tcl_HashEntry *hPtr;
    int new;

    dispPtr = ((TkWindow *) tkwin)->dispPtr;
    if (!dispPtr->atomInit) {
	AtomInit(dispPtr);
    }

    hPtr = Tcl_CreateHashEntry(&dispPtr->nameTable, name, &new);
    if (new) {
	Tcl_HashEntry *hPtr2;
	Atom atom;

	atom = XInternAtom(dispPtr->display, name, False);
	Tcl_SetHashValue(hPtr, atom);
	hPtr2 = Tcl_CreateHashEntry(&dispPtr->atomTable, (char *) atom,
		&new);
	Tcl_SetHashValue(hPtr2, Tcl_GetHashKey(&dispPtr->nameTable, hPtr));
    }
    return (Atom) Tcl_GetHashValue(hPtr);
}

/*
 *--------------------------------------------------------------
 *
 * Tk_GetAtomName --
 *
 *	This procedure is equivalent to XGetAtomName except that
 *	it uses the local atom cache to avoid contacting the
 *	server.
 *
 * Results:
 *	The return value is a character string corresponding to
 *	the atom given by "atom".  This string's storage space
 *	is static:  it need not be freed by the caller, and should
 *	not be modified by the caller.  If "atom" doesn't exist
 *	on tkwin's display, then the string "?bad atom?" is returned.
 *
 * Side effects:
 *	None.
 *
 *--------------------------------------------------------------
 */

char *
Tk_GetAtomName(tkwin, atom)
    Tk_Window tkwin;		/* Window token;  map atom to name
				 * relative to this window's
				 * display. */
    Atom atom;			/* Atom whose name is wanted. */
{
    register TkDisplay *dispPtr;
    register Tcl_HashEntry *hPtr;

    dispPtr = ((TkWindow *) tkwin)->dispPtr;
    if (!dispPtr->atomInit) {
	AtomInit(dispPtr);
    }

    hPtr = Tcl_FindHashEntry(&dispPtr->atomTable, (char *) atom);
    if (hPtr == NULL) {
	char *name;
	Tk_ErrorHandler handler;
	int new;

	handler= Tk_CreateErrorHandler(dispPtr->display, BadAtom,
		-1, -1, (int (*)()) NULL, (ClientData) NULL);
	name = XGetAtomName(dispPtr->display, atom);
	if (name == NULL) {
	    name = "?bad atom?";
	}
	Tk_DeleteErrorHandler(handler);
	hPtr = Tcl_CreateHashEntry(&dispPtr->nameTable, (char *) name,
		&new);
	Tcl_SetHashValue(hPtr, atom);
	name = Tcl_GetHashKey(&dispPtr->nameTable, hPtr);
	hPtr = Tcl_CreateHashEntry(&dispPtr->atomTable, (char *) atom,
		&new);
	Tcl_SetHashValue(hPtr, name);
    }
    return (char *) Tcl_GetHashValue(hPtr);
}

/*
 *--------------------------------------------------------------
 *
 * AtomInit --
 *
 *	Initialize atom-related information for a display.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Tables get initialized, etc. etc..
 *
 *--------------------------------------------------------------
 */

static void
AtomInit(dispPtr)
    register TkDisplay *dispPtr;	/* Display to initialize. */
{
    dispPtr->atomInit = 1;
    Tcl_InitHashTable(&dispPtr->nameTable, TCL_STRING_KEYS);
    Tcl_InitHashTable(&dispPtr->atomTable, TCL_ONE_WORD_KEYS);
}
