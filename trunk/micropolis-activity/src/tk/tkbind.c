/* 
 * tkBind.c --
 *
 *	This file provides procedures that associate Tcl commands
 *	with X events or sequences of X events.
 *
 * Copyright 1989-1991 Regents of the University of California
 * Permission to use, copy, modify, and distribute this
 * software and its documentation for any purpose and without
 * fee is hereby granted, provided that the above copyright
 * notice appear in all copies.  The University of California
 * makes no representations about the suitability of this
 * software for any purpose.  It is provided "as is" without
 * express or implied warranty.
 */

#ifndef lint
static char rcsid[] = "$Header: /user6/ouster/wish/RCS/tkBind.c,v 1.48 92/08/10 16:55:24 ouster Exp $ SPRITE (Berkeley)";
#endif /* not lint */

#include "tkconfig.h"
#include "tkint.h"

/*
 * The structure below represents a binding table.  A binding table
 * represents a domain in which event bindings may occur.  It includes
 * a space of objects relative to which events occur (usually windows,
 * but not always), a history of recent events in the domain, and
 * a set of mappings that associate particular Tcl commands with sequences
 * of events in the domain.  Multiple binding tables may exist at once,
 * either because there are multiple applications open, or because there
 * are multiple domains within an application with separate event
 * bindings for each (for example, each canvas widget has a separate
 * binding table for associating events with the items in the canvas).
 */

#define EVENT_BUFFER_SIZE 10
typedef struct BindingTable {
    XEvent eventRing[EVENT_BUFFER_SIZE];/* Circular queue of recent events
					 * (higher indices are for more recent
					 * events). */
    int detailRing[EVENT_BUFFER_SIZE];	/* "Detail" information (keySym or
					 * button or 0) for each entry in
					 * eventRing. */
    int curEvent;			/* Index in eventRing of most recent
					 * event.  Newer events have higher
					 * indices. */
    Tcl_HashTable patternTable;		/* Used to map from an event to a list
					 * of patterns that may match that
					 * event.  Keys are PatternTableKey
					 * structs, values are (PatSeq *). */
    Tcl_HashTable objectTable;		/* Used to map from an object to a list
					 * of patterns associated with that
					 * object.  Keys are ClientData,
					 * values are (PatSeq *). */
    Tcl_Interp *interp;			/* Interpreter in which commands are
					 * executed. */
} BindingTable;

/*
 * Structures of the following form are used as keys in the patternTable
 * for a binding table:
 */

typedef struct PatternTableKey {
    ClientData object;		/* Identifies object (or class of objects)
				 * relative to which event occurred.  For
				 * example, in the widget binding table for
				 * an application this is the path name of
				 * a widget, or a widget class, or "all". */
    int type;			/* Type of event (from X). */
    int detail;			/* Additional information, such as
				 * keysym or button, or 0 if nothing
				 * additional.*/
} PatternTableKey;

/*
 * The following structure defines a pattern, which is matched
 * against X events as part of the process of converting X events
 * into Tcl commands.
 */

typedef struct Pattern {
    int eventType;		/* Type of X event, e.g. ButtonPress. */
    int needMods;		/* Mask of modifiers that must be
				 * present (0 means no modifiers are
				 * required). */
    int hateMods;		/* Mask of modifiers that must not be
				 * present (0 means any modifiers are
				 * OK). */
    int detail;			/* Additional information that must
				 * match event.  Normally this is 0,
				 * meaning no additional information
				 * must match.  For KeyPress and
				 * KeyRelease events, a keySym may
				 * be specified to select a
				 * particular keystroke (0 means any
				 * keystrokes).  For button events,
				 * specifies a particular button (0
				 * means any buttons are OK). */
} Pattern;

/*
 * The structure below defines a pattern sequence, which consists
 * of one or more patterns.  In order to trigger, a pattern
 * sequence must match the most recent X events (first pattern
 * to most recent event, next pattern to next event, and so on).
 */

typedef struct PatSeq {
    int numPats;		/* Number of patterns in sequence
				 * (usually 1). */
    char *command;		/* Command to invoke when this
				 * pattern sequence matches (malloc-ed). */
    int flags;			/* Miscellaneous flag values;  see
				 * below for definitions. */
    struct PatSeq *nextSeqPtr;
				/* Next in list of all pattern
				 * sequences that have the same
				 * initial pattern.  NULL means
				 * end of list. */
    Tcl_HashEntry *hPtr;	/* Pointer to hash table entry for
				 * the initial pattern.  This is the
				 * head of the list of which nextSeqPtr
				 * forms a part. */
    ClientData object;		/* Identifies object with which event is
				 * associated (e.g. window). */
    struct PatSeq *nextObjPtr;
				/* Next in list of all pattern
				 * sequences for the same object
				 * (NULL for end of list).  Needed to
				 * implement Tk_DeleteAllBindings. */
    Pattern pats[1];		/* Array of "numPats" patterns.  Only
				 * one element is declared here but
				 * in actuality enough space will be
				 * allocated for "numPats" patterns.
				 * To match, pats[0] must match event
				 * n, pats[1] must match event n-1,
				 * etc. */
} PatSeq;

/*
 * Flag values for PatSeq structures:
 *
 * PAT_NEARBY		1 means that all of the events matching
 *			this sequence must occur with nearby X
 *			and Y mouse coordinates and close in time.
 *			This is typically used to restrict multiple
 *			button presses.
 * PAT_PERCENTS		1 means that the command for this pattern
 *			requires percent substitution.  0 means there
 *			are no percents in the command.
 */

#define PAT_NEARBY		1
#define PAT_PERCENTS		2

/*
 * Constants that define how close together two events must be
 * in milliseconds or pixels to meet the PAT_NEARBY constraint:
 */

#define NEARBY_PIXELS		5
#define NEARBY_MS		500

/*
 * The data structure and hash table below are used to map from
 * textual keysym names to keysym numbers.  This structure is
 * present here because the corresponding X procedures are
 * ridiculously slow.
 */

typedef struct {
    char *name;				/* Name of keysym. */
    KeySym value;			/* Numeric identifier for keysym. */
} KeySymInfo;
KeySymInfo keyArray[] = {
#ifndef lint
#include "ks_names.h"
#endif
    (char *) NULL, 0
};
static Tcl_HashTable keySymTable;	/* Hashed form of above structure. */

static int initialized = 0;

/*
 * A hash table is kept to map from the string names of event
 * modifiers to information about those modifiers.  The structure
 * for storing this information, and the hash table built at
 * initialization time, are defined below.
 */

typedef struct {
    char *name;			/* Name of modifier. */
    int mask;			/* Button/modifier mask value,							 * such as Button1Mask. */
    int flags;			/* Various flags;  see below for
				 * definitions. */
} ModInfo;

/*
 * Flags for ModInfo structures:
 *
 * DOUBLE -		Non-zero means duplicate this event,
 *			e.g. for double-clicks.
 * TRIPLE -		Non-zero means triplicate this event,
 *			e.g. for triple-clicks.
 * ANY -		Non-zero means that this event allows
 *			any unspecified modifiers.
 */

#define DOUBLE		1
#define TRIPLE		2
#define ANY		4

static ModInfo modArray[] = {
    "Control",		ControlMask,	0,
    "Shift",		ShiftMask,	0,
    "Lock",		LockMask,	0,
    "B1",		Button1Mask,	0,
    "Button1",		Button1Mask,	0,
    "B2",		Button2Mask,	0,
    "Button2",		Button2Mask,	0,
    "B3",		Button3Mask,	0,
    "Button3",		Button3Mask,	0,
    "B4",		Button4Mask,	0,
    "Button4",		Button4Mask,	0,
    "B5",		Button5Mask,	0,
    "Button5",		Button5Mask,	0,
    "Mod1",		Mod1Mask,	0,
    "M1",		Mod1Mask,	0,
    "Meta",		Mod1Mask,	0,
    "M",		Mod1Mask,	0,
    "Mod2",		Mod2Mask,	0,
    "M2",		Mod2Mask,	0,
    "Alt",		Mod2Mask,	0,
    "Mod3",		Mod3Mask,	0,
    "M3",		Mod3Mask,	0,
    "Mod4",		Mod4Mask,	0,
    "M4",		Mod4Mask,	0,
    "Mod5",		Mod5Mask,	0,
    "M5",		Mod5Mask,	0,
    "Double",		0,		DOUBLE,
    "Triple",		0,		TRIPLE,
    "Any",		0,		ANY,
    NULL,		0,		0};
static Tcl_HashTable modTable;

/*
 * This module also keeps a hash table mapping from event names
 * to information about those events.  The structure, an array
 * to use to initialize the hash table, and the hash table are
 * all defined below.
 */

typedef struct {
    char *name;			/* Name of event. */
    int type;			/* Event type for X, such as
				 * ButtonPress. */
    int eventMask;		/* Mask bits (for XSelectInput)
				 * for this event type. */
} EventInfo;

/*
 * Note:  some of the masks below are an OR-ed combination of
 * several masks.  This is necessary because X doesn't report
 * up events unless you also ask for down events.  Also, X
 * doesn't report button state in motion events unless you've
 * asked about button events.
 */

static EventInfo eventArray[] = {
    "Motion",		MotionNotify,
	    ButtonPressMask|PointerMotionMask,
    "Button",		ButtonPress,		ButtonPressMask,
    "ButtonPress",	ButtonPress,		ButtonPressMask,
    "ButtonRelease",	ButtonRelease,
	    ButtonPressMask|ButtonReleaseMask,
    "Colormap",		ColormapNotify,		ColormapChangeMask,
    "Enter",		EnterNotify,		EnterWindowMask,
    "Leave",		LeaveNotify,		LeaveWindowMask,
    "Expose",		Expose,			ExposureMask,
    "FocusIn",		FocusIn,		FocusChangeMask,
    "FocusOut",		FocusOut,		FocusChangeMask,
    "Keymap",		KeymapNotify,		KeymapStateMask,
    "Key",		KeyPress,		KeyPressMask,
    "KeyPress",		KeyPress,		KeyPressMask,
    "KeyRelease",	KeyRelease,
	    KeyPressMask|KeyReleaseMask,
    "Property",		PropertyNotify,		PropertyChangeMask,
    "ResizeRequest",	ResizeRequest,		ResizeRedirectMask,
    "Circulate",	CirculateNotify,	StructureNotifyMask,
    "Configure",	ConfigureNotify,	StructureNotifyMask,
    "Destroy",		DestroyNotify,		StructureNotifyMask,
    "Gravity",		GravityNotify,		StructureNotifyMask,
    "Map",		MapNotify,		StructureNotifyMask,
    "Reparent",		ReparentNotify,		StructureNotifyMask,
    "Unmap",		UnmapNotify,		StructureNotifyMask,
    "Visibility",	VisibilityNotify,	VisibilityChangeMask,
    "CirculateRequest",	CirculateRequest,	SubstructureRedirectMask,
    "ConfigureRequest",	ConfigureRequest,	SubstructureRedirectMask,
    "MapRequest",	MapRequest,		SubstructureRedirectMask,
    (char *) NULL,	0,			0};
static Tcl_HashTable eventTable;

/*
 * The defines and table below are used to classify events into
 * various groups.  The reason for this is that logically identical
 * fields (e.g. "state") appear at different places in different
 * types of events.  The classification masks can be used to figure
 * out quickly where to extract information from events.
 */

#define KEY_BUTTON_MOTION	0x1
#define CROSSING		0x2
#define FOCUS			0x4
#define EXPOSE			0x8
#define VISIBILITY		0x10
#define CREATE			0x20
#define MAP			0x40
#define REPARENT		0x80
#define CONFIG			0x100
#define CONFIG_REQ		0x200
#define RESIZE_REQ		0x400
#define GRAVITY			0x800
#define PROP			0x0100
#define SEL_CLEAR		0x2000
#define SEL_REQ			0x4000
#define SEL_NOTIFY		0x8000
#define COLORMAP		0x10000
#define MAPPING			0x20000

static int flagArray[LASTEvent] = {
   /* Not used */		0,
   /* Not used */		0,
   /* KeyPress */		KEY_BUTTON_MOTION,
   /* KeyRelease */		KEY_BUTTON_MOTION,
   /* ButtonPress */		KEY_BUTTON_MOTION,
   /* ButtonRelease */		KEY_BUTTON_MOTION,
   /* MotionNotify */		KEY_BUTTON_MOTION,
   /* EnterNotify */		CROSSING,
   /* LeaveNotify */		CROSSING,
   /* FocusIn */		FOCUS,
   /* FocusOut */		FOCUS,
   /* KeymapNotify */		0,
   /* Expose */			EXPOSE,
   /* GraphicsExpose */		EXPOSE,
   /* NoExpose */		0,
   /* VisibilityNotify */	VISIBILITY,
   /* CreateNotify */		CREATE,
   /* DestroyNotify */		0,
   /* UnmapNotify */		0,
   /* MapNotify */		MAP,
   /* MapRequest */		0,
   /* ReparentNotify */		REPARENT,
   /* ConfigureNotify */	CONFIG,
   /* ConfigureRequest */	CONFIG_REQ,
   /* GravityNotify */		0,
   /* ResizeRequest */		RESIZE_REQ,
   /* CirculateNotify */	0,
   /* CirculateRequest */	0,
   /* PropertyNotify */		PROP,
   /* SelectionClear */		SEL_CLEAR,
   /* SelectionRequest */	SEL_REQ,
   /* SelectionNotify */	SEL_NOTIFY,
   /* ColormapNotify */		COLORMAP,
   /* ClientMessage */		0,
   /* MappingNotify */		MAPPING
};

/*
 * Forward declarations for procedures defined later in this
 * file:
 */

static char *		ExpandPercents _ANSI_ARGS_((char *before,
			    XEvent *eventPtr, KeySym keySym, char *after,
			    int afterSize));
static PatSeq *		FindSequence _ANSI_ARGS_((Tcl_Interp *interp,
			    BindingTable *bindPtr, ClientData object,
			    char *eventString, int create,
			    unsigned long *maskPtr));
static char *		GetField _ANSI_ARGS_((char *p, char *copy, int size));
static KeySym		GetKeySym _ANSI_ARGS_((TkDisplay *dispPtr,
			    XEvent *eventPtr));
static PatSeq *		MatchPatterns _ANSI_ARGS_((BindingTable *bindPtr,
			    PatSeq *psPtr));

/*
 *--------------------------------------------------------------
 *
 * Tk_CreateBindingTable --
 *
 *	Set up a new domain in which event bindings may be created.
 *
 * Results:
 *	The return value is a token for the new table, which must
 *	be passed to procedures like Tk_CreatBinding.
 *
 * Side effects:
 *	Memory is allocated for the new table.
 *
 *--------------------------------------------------------------
 */

Tk_BindingTable
Tk_CreateBindingTable(interp)
    Tcl_Interp *interp;		/* Interpreter to associate with the binding
				 * table:  commands are executed in this
				 * interpreter. */
{
    register BindingTable *bindPtr;
    int i;

    /*
     * If this is the first time a binding table has been created,
     * initialize the global data structures.
     */

    if (!initialized) {
	register KeySymInfo *kPtr;
	register Tcl_HashEntry *hPtr;
	register ModInfo *modPtr;
	register EventInfo *eiPtr;
	int dummy;

	initialized = 1;
    
	Tcl_InitHashTable(&keySymTable, TCL_STRING_KEYS);
	for (kPtr = keyArray; kPtr->name != NULL; kPtr++) {
	    hPtr = Tcl_CreateHashEntry(&keySymTable, kPtr->name, &dummy);
	    Tcl_SetHashValue(hPtr, kPtr->value);
	}
    
	Tcl_InitHashTable(&modTable, TCL_STRING_KEYS);
	for (modPtr = modArray; modPtr->name != NULL; modPtr++) {
	    hPtr = Tcl_CreateHashEntry(&modTable, modPtr->name, &dummy);
	    Tcl_SetHashValue(hPtr, modPtr);
	}
    
	Tcl_InitHashTable(&eventTable, TCL_STRING_KEYS);
	for (eiPtr = eventArray; eiPtr->name != NULL; eiPtr++) {
	    hPtr = Tcl_CreateHashEntry(&eventTable, eiPtr->name, &dummy);
	    Tcl_SetHashValue(hPtr, eiPtr);
	}
    }

    /*
     * Create and initialize a new binding table.
     */

    bindPtr = (BindingTable *) ckalloc(sizeof(BindingTable));
    for (i = 0; i < EVENT_BUFFER_SIZE; i++) {
	bindPtr->eventRing[i].type = -1;
    }
    bindPtr->curEvent = 0;
    Tcl_InitHashTable(&bindPtr->patternTable,
	    sizeof(PatternTableKey)/sizeof(int));
    Tcl_InitHashTable(&bindPtr->objectTable, TCL_ONE_WORD_KEYS);
    bindPtr->interp = interp;
    return (Tk_BindingTable) bindPtr;
}

/*
 *--------------------------------------------------------------
 *
 * Tk_DeleteBindingTable --
 *
 *	Destroy a binding table and free up all its memory.
 *	The caller should not use bindingTable again after
 *	this procedure returns.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Memory is freed.
 *
 *--------------------------------------------------------------
 */

void
Tk_DeleteBindingTable(bindingTable)
    Tk_BindingTable bindingTable;	/* Token for the binding table to
					 * destroy. */
{
    BindingTable *bindPtr = (BindingTable *) bindingTable;
    PatSeq *psPtr, *nextPtr;
    Tcl_HashEntry *hPtr;
    Tcl_HashSearch search;

    /*
     * Find and delete all of the patterns associated with the binding
     * table.
     */

    for (hPtr = Tcl_FirstHashEntry(&bindPtr->patternTable, &search);
	    hPtr != NULL; hPtr = Tcl_NextHashEntry(&search)) {
	for (psPtr = (PatSeq *) Tcl_GetHashValue(hPtr);
		psPtr != NULL; psPtr = nextPtr) {
	    nextPtr = psPtr->nextSeqPtr;
	    Tk_EventuallyFree((ClientData) psPtr->command,
		    (Tk_FreeProc *) free);
	    ckfree((char *) psPtr);
	}
    }

    /*
     * Clean up the rest of the information associated with the
     * binding table.
     */

    Tcl_DeleteHashTable(&bindPtr->patternTable);
    Tcl_DeleteHashTable(&bindPtr->objectTable);
    ckfree((char *) bindPtr);
}

/*
 *--------------------------------------------------------------
 *
 * Tk_CreateBinding --
 *
 *	Add a binding to a binding table, so that future calls to
 *	Tk_BindEvent may execute the command in the binding.
 *
 * Results:
 *	The return value is 0 if an error occurred while setting
 *	up the binding.  In this case, an error message will be
 *	left in interp->result.  If all went well then the return
 *	value is a mask of the event types that must be made
 *	available to Tk_BindEvent in order to properly detect when
 *	this binding triggers.  This value can be used to determine
 *	what events to select for in a window, for example.
 *
 * Side effects:
 *	The new binding may cause future calls to Tk_BindEvent to
 *	behave differently than they did previously.
 *
 *--------------------------------------------------------------
 */

unsigned long
Tk_CreateBinding(interp, bindingTable, object, eventString, command, append)
    Tcl_Interp *interp;			/* Used for error reporting. */
    Tk_BindingTable bindingTable;	/* Table in which to create binding. */
    ClientData object;			/* Token for object with which binding
					 * is associated. */
    char *eventString;			/* String describing event sequence
					 * that triggers binding. */
    char *command;			/* Contains Tcl command to execute
					 * when binding triggers. */
    int append;				/* 0 means replace any existing
					 * binding for eventString;  1 means
					 * append to that binding. */
{
    BindingTable *bindPtr = (BindingTable *) bindingTable;
    register PatSeq *psPtr;
    unsigned long eventMask;

    psPtr = FindSequence(interp, bindPtr, object, eventString, 1, &eventMask);
    if (psPtr == NULL) {
	return 0;
    }
    if (append && (psPtr->command != NULL)) {
	int length;
	char *new;

	length = strlen(psPtr->command) + strlen(command) + 3;
	new = (char *) ckalloc((unsigned) length);
	sprintf(new, "%s; %s", psPtr->command, command);
	Tk_EventuallyFree((ClientData) psPtr->command, (Tk_FreeProc *) free);
	psPtr->command = new;
    } else {
	if (psPtr->command != NULL) {
	    Tk_EventuallyFree((ClientData) psPtr->command,
		    (Tk_FreeProc *) free);
	}
	psPtr->command = (char *) ckalloc((unsigned) (strlen(command) + 1));
	strcpy(psPtr->command, command);
    }

    /*
     * See if the command contains percents and thereby requires
     * percent substitution.
     */

    if (strchr(psPtr->command, '%') != NULL) {
	psPtr->flags |= PAT_PERCENTS;
    }
    return eventMask;
}

/*
 *--------------------------------------------------------------
 *
 * Tk_DeleteBinding --
 *
 *	Remove an event binding from a binding table.
 *
 * Results:
 *	The result is a standard Tcl return value.  If an error
 *	occurs then interp->result will contain an error message.
 *
 * Side effects:
 *	The binding given by object and eventString is removed
 *	from bindingTable.
 *
 *--------------------------------------------------------------
 */

int
Tk_DeleteBinding(interp, bindingTable, object, eventString)
    Tcl_Interp *interp;			/* Used for error reporting. */
    Tk_BindingTable bindingTable;	/* Table in which to delete binding. */
    ClientData object;			/* Token for object with which binding
					 * is associated. */
    char *eventString;			/* String describing event sequence
					 * that triggers binding. */
{
    BindingTable *bindPtr = (BindingTable *) bindingTable;
    register PatSeq *psPtr, *prevPtr;
    unsigned long eventMask;
    Tcl_HashEntry *hPtr;

    psPtr = FindSequence(interp, bindPtr, object, eventString, 0, &eventMask);
    if (psPtr == NULL) {
	Tcl_ResetResult(interp);
	return TCL_OK;
    }

    /*
     * Unlink the binding from the list for its object, then from the
     * list for its pattern.
     */

    hPtr = Tcl_FindHashEntry(&bindPtr->objectTable, (char *) object);
    if (hPtr == NULL) {
	panic("Tk_DeleteBinding couldn't find object table entry");
    }
    prevPtr = (PatSeq *) Tcl_GetHashValue(hPtr);
    if (prevPtr == psPtr) {
	Tcl_SetHashValue(hPtr, psPtr->nextObjPtr);
    } else {
	for ( ; ; prevPtr = prevPtr->nextObjPtr) {
	    if (prevPtr == NULL) {
		panic("Tk_DeleteBinding couldn't find on object list");
	    }
	    if (prevPtr->nextObjPtr == psPtr) {
		prevPtr->nextObjPtr = psPtr->nextObjPtr;
		break;
	    }
	}
    }
    prevPtr = (PatSeq *) Tcl_GetHashValue(psPtr->hPtr);
    if (prevPtr == psPtr) {
	if (psPtr->nextSeqPtr == NULL) {
	    Tcl_DeleteHashEntry(psPtr->hPtr);
	} else {
	    Tcl_SetHashValue(psPtr->hPtr, psPtr->nextSeqPtr);
	}
    } else {
	for ( ; ; prevPtr = prevPtr->nextSeqPtr) {
	    if (prevPtr == NULL) {
		panic("Tk_DeleteBinding couldn't find on hash chain");
	    }
	    if (prevPtr->nextSeqPtr == psPtr) {
		prevPtr->nextSeqPtr = psPtr->nextSeqPtr;
		break;
	    }
	}
    }
    Tk_EventuallyFree((ClientData) psPtr->command, (Tk_FreeProc *) free);
    ckfree((char *) psPtr);
    return TCL_OK;
}

/*
 *--------------------------------------------------------------
 *
 * Tk_GetBinding --
 *
 *	Return the command associated with a given event string.
 *
 * Results:
 *	The return value is a pointer to the command string
 *	associated with eventString for object in the domain
 *	given by bindingTable.  If there is no binding for
 *	eventString, or if eventString is improperly formed,
 *	then NULL is returned and an error message is left in
 *	interp->result.  The return value is semi-static:  it
 *	will persist until the binding is changed or deleted.
 *
 * Side effects:
 *	None.
 *
 *--------------------------------------------------------------
 */

char *
Tk_GetBinding(interp, bindingTable, object, eventString)
    Tcl_Interp *interp;			/* Interpreter for error reporting. */
    Tk_BindingTable bindingTable;	/* Table in which to look for
					 * binding. */
    ClientData object;			/* Token for object with which binding
					 * is associated. */
    char *eventString;			/* String describing event sequence
					 * that triggers binding. */
{
    BindingTable *bindPtr = (BindingTable *) bindingTable;
    register PatSeq *psPtr;
    unsigned long eventMask;

    psPtr = FindSequence(interp, bindPtr, object, eventString, 0, &eventMask);
    if (psPtr == NULL) {
	return NULL;
    }
    return psPtr->command;
}

/*
 *--------------------------------------------------------------
 *
 * Tk_GetAllBindings --
 *
 *	Return a list of event strings for all the bindings
 *	associated with a given object.
 *
 * Results:
 *	There is no return value.  Interp->result is modified to
 *	hold a Tcl list with one entry for each binding associated
 *	with object in bindingTable.  Each entry in the list
 *	contains the event string associated with one binding.
 *
 * Side effects:
 *	None.
 *
 *--------------------------------------------------------------
 */

void
Tk_GetAllBindings(interp, bindingTable, object)
    Tcl_Interp *interp;			/* Interpreter for error reporting. */
    Tk_BindingTable bindingTable;	/* Table in which to look for
					 * bindings. */
    ClientData object;			/* Token for object. */

{
    BindingTable *bindPtr = (BindingTable *) bindingTable;
    register PatSeq *psPtr;
    register Pattern *patPtr;
    Tcl_HashEntry *hPtr;
    char string[200*EVENT_BUFFER_SIZE];
    register char *p;
    int patsLeft, needMods;
    register ModInfo *modPtr;

    hPtr = Tcl_FindHashEntry(&bindPtr->objectTable, (char *) object);
    if (hPtr == NULL) {
	return;
    }
    for (psPtr = (PatSeq *) Tcl_GetHashValue(hPtr); psPtr != NULL;
	    psPtr = psPtr->nextObjPtr) {

	p = string;

	/*
	 * For each binding, output information about each of the
	 * patterns in its sequence.  The order of the patterns in
	 * the sequence is backwards from the order in which they
	 * must be output.
	 */

	for (patsLeft = psPtr->numPats,
		patPtr = &psPtr->pats[psPtr->numPats - 1];
		patsLeft > 0; patsLeft--, patPtr--) {

	    /*
	     * Check for simple case of an ASCII character.
	     */

	    if ((patPtr->eventType == KeyPress)
		    && (patPtr->needMods == 0)
		    && (patPtr->hateMods == ~ShiftMask)
		    && isascii(patPtr->detail) && isprint(patPtr->detail)
		    && (patPtr->detail != '<')
		    && (patPtr->detail != ' ')) {

		*p = patPtr->detail;
		p++;
		continue;
	    }

	    /*
	     * It's a more general event specification.  First check
	     * for "Double" or "Triple", then "Any", then modifiers,
	     * the event type, then keysym or button detail.
	     */

	    *p = '<';
	    p++;
	    if ((patsLeft > 1) && (memcmp((char *) patPtr,
		    (char *) (patPtr-1), sizeof(Pattern)) == 0)) {
		patsLeft--;
		patPtr--;
		if ((patsLeft > 1) && (memcmp((char *) patPtr,
			(char *) (patPtr-1), sizeof(Pattern)) == 0)) {
		    patsLeft--;
		    patPtr--;
		    strcpy(p, "Triple-");
		} else {
		    strcpy(p, "Double-");
		}
		p += strlen(p);
	    }

	    if (patPtr->hateMods == 0) {
		strcpy(p, "Any-");
		p += strlen(p);
	    }

	    for (needMods = patPtr->needMods, modPtr = modArray;
		    needMods != 0; modPtr++) {
		if (modPtr->mask & needMods) {
		    needMods &= ~modPtr->mask;
		    strcpy(p, modPtr->name);
		    p += strlen(p);
		    *p = '-';
		    p++;
		}
	    }

	    if ((patPtr->eventType != KeyPress)
		    || (patPtr->detail == 0)) {
		register EventInfo *eiPtr;

		for (eiPtr = eventArray; eiPtr->name != NULL; eiPtr++) {
		    if (eiPtr->type == patPtr->eventType) {
			strcpy(p, eiPtr->name);
			p += strlen(p);
			if (patPtr->detail != 0) {
			    *p = '-';
			    p++;
			}
			break;
		    }
		}
	    }

	    if (patPtr->detail != 0) {
		if ((patPtr->eventType == KeyPress)
			|| (patPtr->eventType == KeyRelease)) {
		    register KeySymInfo *kPtr;

		    for (kPtr = keyArray; kPtr->name != NULL; kPtr++) {
			if (patPtr->detail == (int) kPtr->value) {
			    sprintf(p, "%.100s",  kPtr->name);
			    p += strlen(p);
			    break;
			}
		    }
		} else {
		    sprintf(p, "%d", patPtr->detail);
		    p += strlen(p);
		}
	    }
	    *p = '>';
	    p++;
	}
	*p = 0;
	if ((p - string) >= sizeof(string)) {
	    panic("Tk_GetAllBindings overflowed buffer");
	}
	Tcl_AppendElement(interp, string, 0);
    }
}

/*
 *--------------------------------------------------------------
 *
 * Tk_DeleteAllBindings --
 *
 *	Remove all bindings associated with a given object in a
 *	given binding table.
 *
 * Results:
 *	All bindings associated with object are removed from
 *	bindingTable.
 *
 * Side effects:
 *	None.
 *
 *--------------------------------------------------------------
 */

void
Tk_DeleteAllBindings(bindingTable, object)
    Tk_BindingTable bindingTable;	/* Table in which to delete
					 * bindings. */
    ClientData object;			/* Token for object. */
{
    BindingTable *bindPtr = (BindingTable *) bindingTable;
    register PatSeq *psPtr, *prevPtr;
    PatSeq *nextPtr;
    Tcl_HashEntry *hPtr;

    hPtr = Tcl_FindHashEntry(&bindPtr->objectTable, (char *) object);
    if (hPtr == NULL) {
	return;
    }
    for (psPtr = (PatSeq *) Tcl_GetHashValue(hPtr); psPtr != NULL;
	    psPtr = nextPtr) {
	nextPtr  = psPtr->nextObjPtr;

	/*
	 * Be sure to remove each binding from its hash chain in the
	 * pattern table.  If this is the last pattern in the chain,
	 * then delete the hash entry too.
	 */

	prevPtr = (PatSeq *) Tcl_GetHashValue(psPtr->hPtr);
	if (prevPtr == psPtr) {
	    if (psPtr->nextSeqPtr == NULL) {
		Tcl_DeleteHashEntry(psPtr->hPtr);
	    } else {
		Tcl_SetHashValue(psPtr->hPtr, psPtr->nextSeqPtr);
	    }
	} else {
	    for ( ; ; prevPtr = prevPtr->nextSeqPtr) {
		if (prevPtr == NULL) {
		    panic("Tk_DeleteAllBindings couldn't find on hash chain");
		}
		if (prevPtr->nextSeqPtr == psPtr) {
		    prevPtr->nextSeqPtr = psPtr->nextSeqPtr;
		    break;
		}
	    }
	}
	Tk_EventuallyFree((ClientData) psPtr->command, (Tk_FreeProc *) free);
	ckfree((char *) psPtr);
    }
    Tcl_DeleteHashEntry(hPtr);
}

/*
 *--------------------------------------------------------------
 *
 * Tk_BindEvent --
 *
 *	This procedure is invoked to process an X event.  The
 *	event is added to those recorded for the binding table.
 *	Then each of the objects at *objectPtr is checked in
 *	order to see if it has a binding that matches the recent
 *	events.  If so, that binding is invoked and the rest of
 *	objects are skipped.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Depends on the command associated with the matching
 *	binding.
 *
 *--------------------------------------------------------------
 */

void
Tk_BindEvent(bindingTable, eventPtr, tkwin, numObjects, objectPtr)
    Tk_BindingTable bindingTable;	/* Table in which to look for
					 * bindings. */
    XEvent *eventPtr;			/* What actually happened. */
    Tk_Window tkwin;			/* Window on display where event
					 * occurred (needed in order to
					 * locate display information). */
    int numObjects;			/* Number of objects at *objectPtr. */
    ClientData *objectPtr;		/* Array of one or more objects
					 * to check for a matching binding. */
{
    BindingTable *bindPtr = (BindingTable *) bindingTable;
    TkDisplay *dispPtr = ((TkWindow *) tkwin)->dispPtr;
    XEvent *ringPtr;
    PatSeq *matchPtr;
    PatternTableKey key;
    Tcl_HashEntry *hPtr;
    int detail;

    /*
     * Add the new event to the ring of saved events for the
     * binding table.  Consecutive MotionNotify events get combined:
     * if both the new event and the previous event are MotionNotify,
     * then put the new event *on top* of the previous event.
     */

    if ((eventPtr->type != MotionNotify)
	    || (bindPtr->eventRing[bindPtr->curEvent].type != MotionNotify)) {
	bindPtr->curEvent++;
	if (bindPtr->curEvent >= EVENT_BUFFER_SIZE) {
	    bindPtr->curEvent = 0;
	}
    }
    ringPtr = &bindPtr->eventRing[bindPtr->curEvent];
    memcpy((VOID *) ringPtr, (VOID *) eventPtr, sizeof(XEvent));
    detail = 0;
    bindPtr->detailRing[bindPtr->curEvent] = 0;
    if ((ringPtr->type == KeyPress) || (ringPtr->type == KeyRelease)) {
	detail = (int) GetKeySym(dispPtr, ringPtr);
	if (detail == NoSymbol) {
	    detail = 0;
	}
    } else if ((ringPtr->type == ButtonPress)
	    || (ringPtr->type == ButtonRelease)) {
	detail = ringPtr->xbutton.button;
    }
    bindPtr->detailRing[bindPtr->curEvent] = detail;

    /*
     * Loop over all the objects, matching the new event against
     * each in turn.
     */

    for ( ; numObjects > 0; numObjects--, objectPtr++) {

	/*
	 * Match the new event against those recorded in the
	 * pattern table, saving the longest matching pattern.
	 * For events with details (button and key events) first
	 * look for a binding for the specific key or button.
	 * If none is found, then look for a binding for all
	 * keys or buttons (detail of 0).
	 */
    
	matchPtr = NULL;
	key.object = *objectPtr;
	key.type = ringPtr->type;
	key.detail = detail;
	hPtr = Tcl_FindHashEntry(&bindPtr->patternTable, (char *) &key);
	if (hPtr != NULL) {
	    matchPtr = MatchPatterns(bindPtr,
		    (PatSeq *) Tcl_GetHashValue(hPtr));
	}
	if ((detail != 0) && (matchPtr == NULL)) {
	    key.detail = 0;
	    hPtr = Tcl_FindHashEntry(&bindPtr->patternTable, (char *) &key);
	    if (hPtr != NULL) {
		matchPtr = MatchPatterns(bindPtr,
			(PatSeq *) Tcl_GetHashValue(hPtr));
	    }
	}
    
	if (matchPtr != NULL) {
    
	    /*
	     * %-substitution can increase the length of the command.
	     * This code handles three cases:  (a) no substitution;
	     * (b) substitution results in short command (use space
	     * on stack); and (c) substitution results in long
	     * command (malloc it).
	     */
    
#define STATIC_SPACE 200
	    char shortSpace[STATIC_SPACE];
	    int result;

	    if (matchPtr->flags & PAT_PERCENTS) {
		char *p;
		p = ExpandPercents(matchPtr->command, eventPtr,
			(KeySym) detail, shortSpace, STATIC_SPACE);
		result = Tcl_GlobalEval(bindPtr->interp, p);
		if (p != shortSpace) {
		    ckfree(p);
		}
	    } else {
		/*
		 * The code below is tricky in order allow the binding to
		 * be modified or deleted as part of the command that the
		 * binding invokes.  Must make sure that the actual command
		 * string isn't freed until the command completes, and must
		 * copy the address of this string into a local variable
		 * in case it's modified by the command.
		 */

		char *cmd = matchPtr->command;

		Tk_Preserve((ClientData) cmd);
		result = Tcl_GlobalEval(bindPtr->interp, cmd);
		Tk_Release((ClientData) cmd);
	    }
	    if (result != TCL_OK) {
		Tcl_AddErrorInfo(bindPtr->interp,
			"\n    (command bound to event)");
		TkBindError(bindPtr->interp);
	    }
	    return;
	}
    }
}

/*
 *----------------------------------------------------------------------
 *
 * FindSequence --
 *
 *	Find the entry in a binding table that corresponds to a
 *	particular pattern string, and return a pointer to that
 *	entry.
 *
 * Results:
 *	The return value is normally a pointer to the PatSeq
 *	in patternTable that corresponds to eventString.  If an error
 *	was found while parsing eventString, or if "create" is 0 and
 *	no pattern sequence previously existed, then NULL is returned
 *	and interp->result contains a message describing the problem.
 *	If no pattern sequence previously existed for eventString, then
 *	a new one is created with a NULL command field.  In a successful
 *	return, *maskPtr is filled in with a mask of the event types
 *	on which the pattern sequence depends.
 *
 * Side effects:
 *	A new pattern sequence may be created.
 *
 *----------------------------------------------------------------------
 */

static PatSeq *
FindSequence(interp, bindPtr, object, eventString, create, maskPtr)
    Tcl_Interp *interp;		/* Interpreter to use for error
				 * reporting. */
    BindingTable *bindPtr;	/* Table to use for lookup. */
    ClientData object;		/* Token for object(s) with which binding
				 * is associated. */
    char *eventString;		/* String description of pattern to
				 * match on.  See user documentation
				 * for details. */
    int create;			/* 0 means don't create the entry if
				 * it doesn't already exist.   Non-zero
				 * means create. */
    unsigned long *maskPtr;	/* *maskPtr is filled in with the event
				 * types on which this pattern sequence
				 * depends. */

{
    Pattern pats[EVENT_BUFFER_SIZE];
    int numPats;
    register char *p;
    register Pattern *patPtr;
    register PatSeq *psPtr;
    register Tcl_HashEntry *hPtr;
#define FIELD_SIZE 20
    char field[FIELD_SIZE];
    int flags, any, count, new, sequenceSize;
    unsigned long eventMask;
    PatternTableKey key;

    /*
     *-------------------------------------------------------------
     * Step 1: parse the pattern string to produce an array
     * of Patterns.  The array is generated backwards, so
     * that the lowest-indexed pattern corresponds to the last
     * event that must occur.
     *-------------------------------------------------------------
     */

    p = eventString;
    flags = 0;
    eventMask = 0;
    for (numPats = 0, patPtr = &pats[EVENT_BUFFER_SIZE-1];
	    numPats < EVENT_BUFFER_SIZE;
	    numPats++, patPtr--) {
	patPtr->eventType = -1;
	patPtr->needMods = 0;
	patPtr->hateMods = ~0;
	patPtr->detail = 0;
	while (isspace(*p)) {
	    p++;
	}
	if (*p == '\0') {
	    break;
	}

	/*
	 * Handle simple ASCII characters.  Note:  the shift
	 * modifier is ignored in this case (it's really part
	 * of the character, rather than a "modifier").
	 */

	if (*p != '<') {
	    char string[2];

	    patPtr->eventType = KeyPress;
	    eventMask |= KeyPressMask;
	    string[0] = *p;
	    string[1] = 0;
	    hPtr = Tcl_FindHashEntry(&keySymTable, string);
	    if (hPtr != NULL) {
		patPtr->detail = (int) Tcl_GetHashValue(hPtr);
	    } else {
		if (isprint(*p)) {
		    patPtr->detail = *p;
		} else {
		    sprintf(interp->result,
			    "bad ASCII character 0x%x", *p);
		    return NULL;
		}
	    }
	    patPtr->hateMods = ~ShiftMask;
	    p++;
	    continue;
	}

	/*
	 * A fancier event description.  Must consist of
	 * 1. open angle bracket.
	 * 2. any number of modifiers, each followed by spaces
	 *    or dashes.
	 * 3. an optional event name.
	 * 4. an option button or keysym name.  Either this or
	 *    item 3 *must* be present;  if both are present
	 *    then they are separated by spaces or dashes.
	 * 5. a close angle bracket.
	 */

	any = 0;
	count = 1;
	p++;
	while (1) {
	    register ModInfo *modPtr;
	    p = GetField(p, field, FIELD_SIZE);
	    hPtr = Tcl_FindHashEntry(&modTable, field);
	    if (hPtr == NULL) {
		break;
	    }
	    modPtr = (ModInfo *) Tcl_GetHashValue(hPtr);
	    patPtr->needMods |= modPtr->mask;
	    if (modPtr->flags & (DOUBLE|TRIPLE)) {
		flags |= PAT_NEARBY;
		if (modPtr->flags & DOUBLE) {
		    count = 2;
		} else {
		    count = 3;
		}
	    }
	    if (modPtr->flags & ANY) {
		any = 1;
	    }
	    while ((*p == '-') || isspace(*p)) {
		p++;
	    }
	}
	if (any) {
	    patPtr->hateMods = 0;
	} else {
	    patPtr->hateMods = ~patPtr->needMods;
	}
	hPtr = Tcl_FindHashEntry(&eventTable, field);
	if (hPtr != NULL) {
	    register EventInfo *eiPtr;
	    eiPtr = (EventInfo *) Tcl_GetHashValue(hPtr);
	    patPtr->eventType = eiPtr->type;
	    eventMask |= eiPtr->eventMask;
	    while ((*p == '-') || isspace(*p)) {
		p++;
	    }
	    p = GetField(p, field, FIELD_SIZE);
	}
	if (*field != '\0') {
	    if ((*field >= '1') && (*field <= '5') && (field[1] == '\0')) {
		static int masks[] = {~0, ~Button1Mask, ~Button2Mask,
			~Button3Mask, ~Button4Mask, ~Button5Mask};

		if (patPtr->eventType == -1) {
		    patPtr->eventType = ButtonPress;
		    eventMask |= ButtonPressMask;
		} else if ((patPtr->eventType == KeyPress)
			|| (patPtr->eventType == KeyRelease)) {
		    goto getKeysym;
		} else if ((patPtr->eventType != ButtonPress)
			&& (patPtr->eventType != ButtonRelease)) {
		    Tcl_AppendResult(interp, "specified button \"", field,
			    "\" for non-button event", (char *) NULL);
		    return NULL;
		}
		patPtr->detail = (*field - '0');

		/*
		 * Ignore this button as a modifier:  its state is already
		 * fixed.
		 */

		patPtr->needMods &= masks[patPtr->detail];
		patPtr->hateMods &= masks[patPtr->detail];
	    } else {
		getKeysym:
		hPtr = Tcl_FindHashEntry(&keySymTable, (char *) field);
		if (hPtr == NULL) {
		    Tcl_AppendResult(interp, "bad event type or keysym \"",
			    field, "\"", (char *) NULL);
		    return NULL;
		}
		if (patPtr->eventType == -1) {
		    patPtr->eventType = KeyPress;
		    eventMask |= KeyPressMask;
		} else if ((patPtr->eventType != KeyPress)
			&& (patPtr->eventType != KeyRelease)) {
		    Tcl_AppendResult(interp, "specified keysym \"", field,
			    "\" for non-key event", (char *) NULL);
		    return NULL;
		}
		patPtr->detail = (int) Tcl_GetHashValue(hPtr);

		/*
		 * Don't get upset about the shift modifier with keys:
		 * if the key doesn't permit the shift modifier then
		 * that will already be factored in when translating
		 * from keycode to keysym in Tk_BindEvent.  If the keysym
		 * has both a shifted and unshifted form, we want to allow
		 * the shifted form to be specified explicitly, though.
		 */

		patPtr->hateMods &= ~ShiftMask;
	    }
	} else if (patPtr->eventType == -1) {
	    interp->result = "no event type or button # or keysym";
	    return NULL;
	}
	while ((*p == '-') || isspace(*p)) {
	    p++;
	}
	if (*p != '>') {
	    interp->result = "missing \">\" in binding";
	    return NULL;
	}
	p++;

	/*
	 * Replicate events for DOUBLE and TRIPLE.
	 */

	if ((count > 1) && (numPats < EVENT_BUFFER_SIZE-1)) {
	    patPtr[-1] = patPtr[0];
	    patPtr--;
	    numPats++;
	    if ((count == 3) && (numPats < EVENT_BUFFER_SIZE-1)) {
		patPtr[-1] = patPtr[0];
		patPtr--;
		numPats++;
	    }
	}
    }

    /*
     *-------------------------------------------------------------
     * Step 2: find the sequence in the binding table if it exists,
     * and add a new sequence to the table if it doesn't.
     *-------------------------------------------------------------
     */

    if (numPats == 0) {
	interp->result = "no events specified in binding";
	return NULL;
    }
    patPtr = &pats[EVENT_BUFFER_SIZE-numPats];
    key.object = object;
    key.type = patPtr->eventType;
    key.detail = patPtr->detail;
    hPtr = Tcl_CreateHashEntry(&bindPtr->patternTable, (char *) &key, &new);
    sequenceSize = numPats*sizeof(Pattern);
    if (!new) {
	for (psPtr = (PatSeq *) Tcl_GetHashValue(hPtr); psPtr != NULL;
		psPtr = psPtr->nextSeqPtr) {
	    if ((numPats == psPtr->numPats)
		    && ((flags & PAT_NEARBY) == (psPtr->flags & PAT_NEARBY))
		    && (memcmp((char *) patPtr, (char *) psPtr->pats,
		    sequenceSize) == 0)) {
		*maskPtr = eventMask; /*don't forget to pass back the mask*/
		goto done;
	    }
	}
    }
    if (!create) {
	if (new) {
	    Tcl_DeleteHashEntry(hPtr);
	}
	Tcl_AppendResult(interp, "no binding exists for \"",
		eventString, "\"", (char *) NULL);
	return NULL;
    }
    psPtr = (PatSeq *) ckalloc((unsigned) (sizeof(PatSeq)
	    + (numPats-1)*sizeof(Pattern)));
    psPtr->numPats = numPats;
    psPtr->command = NULL;
    psPtr->flags = flags;
    psPtr->nextSeqPtr = (PatSeq *) Tcl_GetHashValue(hPtr);
    psPtr->hPtr = hPtr;
    Tcl_SetHashValue(hPtr, psPtr);

    /*
     * Link the pattern into the list associated with the object.
     */

    psPtr->object = object;
    hPtr = Tcl_CreateHashEntry(&bindPtr->objectTable, (char *) object, &new);
    if (new) {
	psPtr->nextObjPtr = NULL;
    } else {
	psPtr->nextObjPtr = (PatSeq *) Tcl_GetHashValue(hPtr);
    }
    Tcl_SetHashValue(hPtr, psPtr);

    memcpy((VOID *) psPtr->pats, (VOID *) patPtr, sequenceSize);

    done:
    *maskPtr = eventMask;
    return psPtr;
}

/*
 *----------------------------------------------------------------------
 *
 * GetField --
 *
 *	Used to parse pattern descriptions.  Copies up to
 *	size characters from p to copy, stopping at end of
 *	string, space, "-", ">", or whenever size is
 *	exceeded.
 *
 * Results:
 *	The return value is a pointer to the character just
 *	after the last one copied (usually "-" or space or
 *	">", but could be anything if size was exceeded).
 *	Also places NULL-terminated string (up to size
 *	character, including NULL), at copy.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

static char *
GetField(p, copy, size)
    register char *p;		/* Pointer to part of pattern. */
    register char *copy;	/* Place to copy field. */
    int size;			/* Maximum number of characters to
				 * copy. */
{
    while ((*p != '\0') && !isspace(*p) && (*p != '>')
	    && (*p != '-') && (size > 1)) {
	*copy = *p;
	p++;
	copy++;
	size--;
    }
    *copy = '\0';
    return p;
}

/*
 *----------------------------------------------------------------------
 *
 * GetKeySym --
 *
 *	Given an X KeyPress or KeyRelease event, map the
 *	keycode in the event into a KeySym.
 *
 * Results:
 *	The return value is the KeySym corresponding to
 *	eventPtr, or NoSymbol if no matching Keysym could be
 *	found.
 *
 * Side effects:
 *	In the first call for a given display, keycode-to-
 *	KeySym maps get loaded.
 *
 *----------------------------------------------------------------------
 */

static KeySym
GetKeySym(dispPtr, eventPtr)
    register TkDisplay *dispPtr;	/* Display in which to
					 * map keycode. */
    register XEvent *eventPtr;		/* Description of X event. */
{
    KeySym *symPtr;
    KeySym sym;

    /*
     * Read the key mapping information from the server if
     * we don't have it already.
     */

    if (dispPtr->symsPerCode == 0) {
	Display *dpy = dispPtr->display;

#ifdef IS_LINUX
	XDisplayKeycodes(dpy, &dispPtr->firstKeycode, &dispPtr->lastKeycode);
#else
	dispPtr->firstKeycode = 
	  dpy->min_keycode;
	dispPtr->lastKeycode = 
	  dpy->max_keycode;
#endif
	dispPtr->keySyms = XGetKeyboardMapping(dpy,
		dispPtr->firstKeycode, dispPtr->lastKeycode + 1
		- dispPtr->firstKeycode, &dispPtr->symsPerCode);
    }

    /*
     * Compute the lower-case KeySym for this keycode.  May
     * have to convert an upper-case KeySym to a lower-case
     * one if the list only has a single element.
     */

    if ((eventPtr->xkey.keycode < dispPtr->firstKeycode)
	    || (eventPtr->xkey.keycode > dispPtr->lastKeycode)) {
	return NoSymbol;
    }
    symPtr = &dispPtr->keySyms[(eventPtr->xkey.keycode
	    - dispPtr->firstKeycode) * dispPtr->symsPerCode];
    sym = *symPtr;
    if ((dispPtr->symsPerCode == 1) || (symPtr[1] == NoSymbol)) {
	if ((sym >= XK_A) && (sym <= XK_Z)) {
	    sym += (XK_a - XK_A);
	} else if ((sym >= XK_Agrave) && (sym <= XK_Odiaeresis)) {
	    sym += (XK_agrave - XK_Agrave);
	} else if ((sym >= XK_Ooblique) && (sym <= XK_Thorn)) {
	    sym += (XK_oslash - XK_Ooblique);
	}
    }

    /*
     * See whether the key is shifted or caps-locked.  If so,
     * use an upper-case equivalent if provided, or compute
     * one (for caps-lock, just compute upper-case: don't
     * use shifted KeySym since that would shift non-alphabetic
     * keys).
     */

    if (eventPtr->xkey.state & ShiftMask) {
	if ((dispPtr->symsPerCode > 1) && (symPtr[1] != NoSymbol)) {
	    return symPtr[1];
	}
	shiftToUpper:
	if ((sym >= XK_a) && (sym <= XK_z)) {
	    sym += (XK_A - XK_a);
	} else if ((sym >= XK_agrave) && (sym <= XK_adiaeresis)) {
	    sym += (XK_Agrave - XK_agrave);
	} else if ((sym >= XK_oslash) && (sym <= XK_thorn)) {
	    sym += (XK_Ooblique - XK_oslash);
	}
	return sym;
    }
    if (eventPtr->xkey.state & LockMask) {
	goto shiftToUpper;
    }
    return sym;
}

/*
 *----------------------------------------------------------------------
 *
 * MatchPatterns --
 *
 *	Given a list of pattern sequences and a list of
 *	recent events, return a pattern sequence that matches
 *	the event list.
 *
 * Results:
 *	The return value is NULL if no pattern matches the
 *	recent events from bindPtr.  If one or more patterns
 *	matches, then the longest (or most specific) matching
 *	pattern is returned.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

static PatSeq *
MatchPatterns(bindPtr, psPtr)
    BindingTable *bindPtr;	/* Information about binding table, such
				 * as ring of recent events. */
    register PatSeq *psPtr;	/* List of pattern sequences. */
{
    register PatSeq *bestPtr = NULL;

    /*
     * Iterate over all the pattern sequences.
     */

    for ( ; psPtr != NULL; psPtr = psPtr->nextSeqPtr) {
	register XEvent *eventPtr;
	register Pattern *patPtr;
	Window window;
	int *detailPtr;
	int patCount, ringCount, flags, state;

	/*
	 * Iterate over all the patterns in a sequence to be
	 * sure that they all match.
	 */

	eventPtr = &bindPtr->eventRing[bindPtr->curEvent];
	detailPtr = &bindPtr->detailRing[bindPtr->curEvent];
	window = eventPtr->xany.window;
	patPtr = psPtr->pats;
	patCount = psPtr->numPats;
	ringCount = EVENT_BUFFER_SIZE;
	while (patCount > 0) {
	    if (ringCount <= 0) {
		goto nextSequence;
	    }
	    if (eventPtr->xany.window != window) {
		goto nextSequence;
	    }
	    if (eventPtr->xany.type != patPtr->eventType) {
		/*
		 * If the event is a mouse motion, button release,
		 * or key release event, and it didn't match
		 * the pattern, then just skip the event and try
		 * the next event against the same pattern.
		 */

		if ((eventPtr->xany.type == MotionNotify)
			|| (eventPtr->xany.type == ButtonRelease)
			|| (eventPtr->xany.type == KeyRelease)
			|| (eventPtr->xany.type == NoExpose)
			|| (eventPtr->xany.type == GraphicsExpose)) {
		    goto nextEvent;
		}
		goto nextSequence;
	    }

	    flags = flagArray[eventPtr->type];
	    if (flags & KEY_BUTTON_MOTION) {
		state = eventPtr->xkey.state;
	    } else if (flags & CROSSING) {
		state = eventPtr->xcrossing.state;
	    } else {
		state = 0;
	    }
	    if ((state & patPtr->needMods)
		    != patPtr->needMods) {
		goto nextSequence;
	    }
	    if ((state & patPtr->hateMods) != 0) {
		goto nextSequence;
	    }
	    if ((patPtr->detail != 0)
		    && (patPtr->detail != *detailPtr)) {
		goto nextSequence;
	    }
	    if (psPtr->flags & PAT_NEARBY) {
		register XEvent *firstPtr;

		firstPtr = &bindPtr->eventRing[bindPtr->curEvent];
		if ((firstPtr->xkey.x_root
			    < (eventPtr->xkey.x_root - NEARBY_PIXELS))
			|| (firstPtr->xkey.x_root
			    > (eventPtr->xkey.x_root + NEARBY_PIXELS))
			|| (firstPtr->xkey.y_root
			    < (eventPtr->xkey.y_root - NEARBY_PIXELS))
			|| (firstPtr->xkey.y_root
			    > (eventPtr->xkey.y_root + NEARBY_PIXELS))
			|| (firstPtr->xkey.time
			    > (eventPtr->xkey.time + NEARBY_MS))) {
		    goto nextSequence;
		}
	    }
	    patPtr++;
	    patCount--;
	    nextEvent:
	    if (eventPtr == bindPtr->eventRing) {
		eventPtr = &bindPtr->eventRing[EVENT_BUFFER_SIZE-1];
		detailPtr = &bindPtr->detailRing[EVENT_BUFFER_SIZE-1];
	    } else {
		eventPtr--;
		detailPtr--;
	    }
	    ringCount--;
	}

	/*
	 * This sequence matches.  If we've already got another match,
	 * pick whichever is most specific.  Detail is most important,
	 * then needMods, then hateMods.
	 */

	if (bestPtr != NULL) {
	    register Pattern *patPtr2;
	    int i;

	    if (psPtr->numPats != bestPtr->numPats) {
		if (bestPtr->numPats > psPtr->numPats) {
		    goto nextSequence;
		} else {
		    goto newBest;
		}
	    }
	    for (i = 0, patPtr = psPtr->pats, patPtr2 = bestPtr->pats;
		    i < psPtr->numPats; i++,patPtr++, patPtr2++) {
		if (patPtr->detail != patPtr2->detail) {
		    if (patPtr->detail == 0) {
			goto nextSequence;
		    } else {
			goto newBest;
		    }
		}
		if (patPtr->needMods != patPtr2->needMods) {
		    if ((patPtr->needMods & patPtr2->needMods)
			== patPtr->needMods) {
		    goto nextSequence;
		    } else {
			goto newBest;
		    }
		}
		if (patPtr->hateMods != patPtr2->hateMods) {
		    if ((patPtr->hateMods & patPtr2->hateMods)
			== patPtr2->hateMods) {
			goto newBest;
		    } else {
			goto nextSequence;
		    }
		}
	    }
	    goto nextSequence;	/* Tie goes to newest pattern. */
	}
	newBest:
	bestPtr = psPtr;

	nextSequence: continue;
    }
    return bestPtr;
}

/*
 *--------------------------------------------------------------
 *
 * ExpandPercents --
 *
 *	Given a command and an event, produce a new command
 *	by replacing % constructs in the original command
 *	with information from the X event.
 *
 * Results:
 *	The return result is a pointer to the new %-substituted
 *	command.  If the command fits in the space at after, then
 *	the return value is after.  If the command is too large
 *	to fit at after, then the return value is a pointer to
 *	a malloc-ed buffer holding the command;  in this case it
 *	is the caller's responsibility to free up the buffer when
 *	finished with it.
 *
 * Side effects:
 *	None.
 *
 *--------------------------------------------------------------
 */

static char *
ExpandPercents(before, eventPtr, keySym, after, afterSize)
    register char *before;	/* Command containing percent
				 * expressions to be replaced. */
    register XEvent *eventPtr;	/* X event containing information
				 * to be used in % replacements. */
    KeySym keySym;		/* KeySym: only relevant for
				 * KeyPress and KeyRelease events). */
    char *after;		/* Place to generate new expanded
				 * command.  Must contain at least
				 * "afterSize" bytes of space. */
    int afterSize;		/* Number of bytes of space available at
				 * after. */
{
    register char *buffer;	/* Pointer to buffer currently being used
				 * as destination. */
    register char *dst;		/* Pointer to next place to store character
				 * in substituted string. */
    int spaceLeft;		/* Indicates how many more non-null bytes
				 * may be stored at *dst before space
				 * runs out. */
    int spaceNeeded, cvtFlags;	/* Used to substitute string as proper Tcl
				 * list element. */
    int number, flags;
#define NUM_SIZE 40
    register char *string;
    char numStorage[NUM_SIZE+1];

    if (eventPtr->type < LASTEvent) {
	flags = flagArray[eventPtr->type];
    } else {
	flags = 0;
    }
    dst = buffer = after;
    spaceLeft = afterSize - 1;
    while (*before != 0) {
	if (*before != '%') {

	    /*
	     * Expand the destination string if necessary.
	     */

	    if (spaceLeft <= 0) {
		char *newSpace;

		newSpace = (char *) ckalloc((unsigned) (2*afterSize));
		memcpy((VOID *) newSpace, (VOID *) buffer, afterSize);
		afterSize *= 2;
		dst = newSpace + (dst - buffer);
		if (buffer != after) {
		    ckfree(buffer);
		}
		buffer = newSpace;
		spaceLeft = afterSize - (dst-buffer) - 1;
	    }
	    *dst = *before;
	    dst++;
	    before++;
	    spaceLeft--;
	    continue;
	}

	number = 0;
	string = "??";
	switch (before[1]) {
	    case '#':
		number = eventPtr->xany.serial;
		goto doNumber;
	    case 'a':
		number = (int) eventPtr->xconfigure.above;
		goto doNumber;
	    case 'b':
		number = eventPtr->xbutton.button;
		goto doNumber;
	    case 'c':
		if (flags & EXPOSE) {
		    number = eventPtr->xexpose.count;
		} else if (flags & MAPPING) {
		    number = eventPtr->xmapping.count;
		}
		goto doNumber;
	    case 'd':
		if (flags & (CROSSING|FOCUS)) {
		    switch (eventPtr->xcrossing.detail) {
			case NotifyAncestor:
			    string = "NotifyAncestor";
			    break;
			case NotifyVirtual:
			    string = "NotifyVirtual";
			    break;
			case NotifyInferior:
			    string = "NotifyInferior";
			    break;
			case NotifyNonlinear:
			    string = "NotifyNonlinear";
			    break;
			case NotifyNonlinearVirtual:
			    string = "NotifyNonlinearVirtual";
			    break;
			case NotifyPointer:
			    string = "NotifyPointer";
			    break;
			case NotifyPointerRoot:
			    string = "NotifyPointerRoot";
			    break;
			case NotifyDetailNone:
			    string = "NotifyDetailNone";
			    break;
		    }
		} else if (flags & CONFIG_REQ) {
		    switch (eventPtr->xconfigurerequest.detail) {
			case Above:
			    string = "Above";
			    break;
			case Below:
			    string = "Below";
			    break;
			case TopIf:
			    string = "TopIf";
			    break;
			case BottomIf:
			    string = "BottomIf";
			    break;
			case Opposite:
			    string = "Opposite";
			    break;
		    }
		}
		goto doString;
	    case 'f':
		number = eventPtr->xcrossing.focus;
		goto doNumber;
	    case 'h':
		if (flags & EXPOSE) {
		    number = eventPtr->xexpose.height;
		} else if (flags & (CONFIG|CONFIG_REQ)) {
		    number = eventPtr->xconfigure.height;
		} else if (flags & RESIZE_REQ) {
		    number = eventPtr->xresizerequest.height;
		}
		goto doNumber;
	    case 'k':
		number = eventPtr->xkey.keycode;
		goto doNumber;
	    case 'm':
		if (flags & CROSSING) {
		    number = eventPtr->xcrossing.mode;
		} else if (flags & FOCUS) {
		    number = eventPtr->xfocus.mode;
		}
		switch (number) {
		    case NotifyNormal:
			string = "NotifyNormal";
			break;
		    case NotifyGrab:
			string = "NotifyGrab";
			break;
		    case NotifyUngrab:
			string = "NotifyUngrab";
			break;
		    case NotifyWhileGrabbed:
			string = "NotifyWhileGrabbed";
			break;
		}
		goto doString;
	    case 'o':
		if (flags & CREATE) {
		    number = eventPtr->xcreatewindow.override_redirect;
		} else if (flags & MAP) {
		    number = eventPtr->xmap.override_redirect;
		} else if (flags & REPARENT) {
		    number = eventPtr->xreparent.override_redirect;
		} else if (flags & CONFIG) {
		    number = eventPtr->xconfigure.override_redirect;
		}
		goto doNumber;
	    case 'p':
		switch (eventPtr->xcirculate.place) {
		    case PlaceOnTop:
			string = "PlaceOnTop";
			break;
		    case PlaceOnBottom:
			string = "PlaceOnBottom";
			break;
		}
		goto doString;
	    case 's':
		if (flags & KEY_BUTTON_MOTION) {
		    number = eventPtr->xkey.state;
		} else if (flags & CROSSING) {
		    number = eventPtr->xcrossing.state;
		} else if (flags & VISIBILITY) {
		    switch (eventPtr->xvisibility.state) {
			case VisibilityUnobscured:
			    string = "VisibilityUnobscured";
			    break;
			case VisibilityPartiallyObscured:
			    string = "VisibilityPartiallyObscured";
			    break;
			case VisibilityFullyObscured:
			    string = "VisibilityFullyObscured";
			    break;
		    }
		    goto doString;
		}
		goto doNumber;
	    case 't':
		if (flags & (KEY_BUTTON_MOTION|PROP|SEL_CLEAR)) {
		    number = (int) eventPtr->xkey.time;
		} else if (flags & SEL_REQ) {
		    number = (int) eventPtr->xselectionrequest.time;
		} else if (flags & SEL_NOTIFY) {
		    number = (int) eventPtr->xselection.time;
		}
		goto doNumber;
	    case 'v':
		number = eventPtr->xconfigurerequest.value_mask;
		goto doNumber;
	    case 'w':
		if (flags & EXPOSE) {
		    number = eventPtr->xexpose.width;
		} else if (flags & (CONFIG|CONFIG_REQ)) {
		    number = eventPtr->xconfigure.width;
		} else if (flags & RESIZE_REQ) {
		    number = eventPtr->xresizerequest.width;
		}
		goto doNumber;
	    case 'x':
		if (flags & KEY_BUTTON_MOTION) {
		    number = eventPtr->xkey.x;
		} else if (flags & EXPOSE) {
		    number = eventPtr->xexpose.x;
		} else if (flags & (CREATE|CONFIG|GRAVITY|CONFIG_REQ)) {
		    number = eventPtr->xcreatewindow.x;
		} else if (flags & REPARENT) {
		    number = eventPtr->xreparent.x;
		} else if (flags & CROSSING) {
		    number = eventPtr->xcrossing.x;
		}
		goto doNumber;
	    case 'y':
		if (flags & KEY_BUTTON_MOTION) {
		    number = eventPtr->xkey.y;
		} else if (flags & EXPOSE) {
		    number = eventPtr->xexpose.y;
		} else if (flags & (CREATE|CONFIG|GRAVITY|CONFIG_REQ)) {
		    number = eventPtr->xcreatewindow.y;
		} else if (flags & REPARENT) {
		    number = eventPtr->xreparent.y;
		} else if (flags & CROSSING) {
		    number = eventPtr->xcrossing.y;

		}
		goto doNumber;
	    case 'A':
		if ((eventPtr->type == KeyPress)
			|| (eventPtr->type == KeyRelease)) {
		    int numChars;

		    numChars = XLookupString(&eventPtr->xkey, numStorage,
			    NUM_SIZE, (KeySym *) NULL,
			    (XComposeStatus *) NULL);
		    numStorage[numChars] = '\0';
		    string = numStorage;
		}
		goto doString;
	    case 'B':
		number = eventPtr->xcreatewindow.border_width;
		goto doNumber;
	    case 'D':
		number = (int) eventPtr->xany.display;
		goto doNumber;
	    case 'E':
		number = (int) eventPtr->xany.send_event;
		goto doNumber;
	    case 'K':
		if ((eventPtr->type == KeyPress)
			|| (eventPtr->type == KeyRelease)) {
		    register KeySymInfo *kPtr;

		    for (kPtr = keyArray; kPtr->name != NULL; kPtr++) {
			if (kPtr->value == keySym) {
			    string = kPtr->name;
			    break;
			}
		    }
		}
		goto doString;
	    case 'N':
		number = (int) keySym;
		goto doNumber;
	    case 'R':
		number = (int) eventPtr->xkey.root;
		goto doNumber;
	    case 'S':
		number = (int) eventPtr->xkey.subwindow;
		goto doNumber;
	    case 'T':
		number = eventPtr->type;
		goto doNumber;
	    case 'W': {
		TkWindow *winPtr;

		if (XFindContext(eventPtr->xany.display, eventPtr->xany.window,
			tkWindowContext, (void *) &winPtr) == 0) {
		    string = winPtr->pathName;
		} else {
		    string = "??";
		}
		goto doString;
	    }
	    case 'X':
		number = eventPtr->xkey.x_root;
		goto doNumber;
	    case 'Y':
		number = eventPtr->xkey.y_root;
		goto doNumber;
	    default:
		numStorage[0] = before[1];
		numStorage[1] = '\0';
		string = numStorage;
		goto doString;
	}

	doNumber:
	sprintf(numStorage, "%d", number);
	string = numStorage;

	doString:
	spaceNeeded = Tcl_ScanElement(string, &cvtFlags);
	if (spaceNeeded >= spaceLeft) {
	    char *newSpace;

	    newSpace = (char *) ckalloc((unsigned)
		    (afterSize + spaceNeeded + 50));
	    memcpy((VOID *) newSpace, (VOID *) buffer, afterSize);
	    afterSize += spaceNeeded + 50;
	    dst = newSpace + (dst - buffer);
	    if (buffer != after) {
		ckfree(buffer);
	    }
	    buffer = newSpace;
	    spaceLeft = afterSize - (dst-buffer) - 1;
	}
	spaceNeeded = Tcl_ConvertElement(string, dst,
		cvtFlags | TCL_DONT_USE_BRACES);
	dst += spaceNeeded;
	spaceLeft -= spaceNeeded;
	before += 2;
    }
    *dst = '\0';
    return buffer;
}

/*
 *----------------------------------------------------------------------
 *
 * TkBindError --
 *
 *	This procedure is invoked to handle errors that occur in Tcl
 *	commands that are invoked in "background" (e.g. from event or
 *	timer bindings).
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The command "tkerror" is invoked to process the error, passing
 *	it the error message.  If that fails, then an error message
 *	is output on stderr.
 *
 *----------------------------------------------------------------------
 */

void
TkBindError(interp)
    Tcl_Interp *interp;		/* Interpreter in which an error has
				 * occurred. */
{
    char *argv[2];
    char *command;
    char *error;
    char *errorInfo, *tmp;
    int result;

    error = (char *) ckalloc((unsigned) (strlen(interp->result) + 1));
    strcpy(error, interp->result);
    tmp = Tcl_GetVar(interp, "errorInfo", TCL_GLOBAL_ONLY);
    if (tmp == NULL) {
	errorInfo = error;
    } else {
	errorInfo = (char *) ckalloc((unsigned) (strlen(tmp) + 1));
	strcpy(errorInfo, tmp);
    }
    argv[0] = "tkerror";
    argv[1] = error;
    command = Tcl_Merge(2, argv);
    result = Tcl_GlobalEval(interp, command);
    if (result != TCL_OK) {
	if (strcmp(interp->result, "\"tkerror\" is an invalid command name or ambiguous abbreviation") == 0) {
	    fprintf(stderr, "%s\n", errorInfo);
	} else {
	    fprintf(stderr, "tkerror failed to handle background error.\n");
	    fprintf(stderr, "    Original error: %s\n", error);
	    fprintf(stderr, "    Error in tkerror: %s\n", interp->result);
	}
    }
    Tcl_ResetResult(interp);
    ckfree(command);
    ckfree(error);
    if (errorInfo != error) {
	ckfree(errorInfo);
    }
}
