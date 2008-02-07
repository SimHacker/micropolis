/* 
 * tkTextTag.c --
 *
 *	This module implements the "tag" subcommand of the widget command
 *	for text widgets, plus most of the other high-level functions
 *	related to tags.
 *
 * Copyright 1992 Regents of the University of California.
 * Permission to use, copy, modify, and distribute this
 * software and its documentation for any purpose and without
 * fee is hereby granted, provided that the above copyright
 * notice appear in all copies.  The University of California
 * makes no representations about the suitability of this
 * software for any purpose.  It is provided "as is" without
 * express or implied warranty.
 */

#ifndef lint
static char rcsid[] = "$Header: /user6/ouster/wish/RCS/tkTextTag.c,v 1.3 92/07/28 15:38:59 ouster Exp $ SPRITE (Berkeley)";
#endif

#include "default.h"
#include "tkconfig.h"
#include "tk.h"
#include "tktext.h"

/*
 * Information used for parsing tag configuration information:
 */

static Tk_ConfigSpec tagConfigSpecs[] = {
    {TK_CONFIG_BORDER, "-background", (char *) NULL, (char *) NULL,
	(char *) NULL, Tk_Offset(TkTextTag, border), TK_CONFIG_NULL_OK},
    {TK_CONFIG_BITMAP, "-bgstipple", (char *) NULL, (char *) NULL,
	(char *) NULL, Tk_Offset(TkTextTag, bgStipple), TK_CONFIG_NULL_OK},
    {TK_CONFIG_PIXELS, "-borderwidth", (char *) NULL, (char *) NULL,
	"0", Tk_Offset(TkTextTag, borderWidth), TK_CONFIG_DONT_SET_DEFAULT},
    {TK_CONFIG_BITMAP, "-fgstipple", (char *) NULL, (char *) NULL,
	(char *) NULL, Tk_Offset(TkTextTag, fgStipple), TK_CONFIG_NULL_OK},
    {TK_CONFIG_FONT, "-font", (char *) NULL, (char *) NULL,
	(char *) NULL, Tk_Offset(TkTextTag, fontPtr), TK_CONFIG_NULL_OK},
    {TK_CONFIG_COLOR, "-foreground", (char *) NULL, (char *) NULL,
	(char *) NULL, Tk_Offset(TkTextTag, fgColor), TK_CONFIG_NULL_OK},
    {TK_CONFIG_RELIEF, "-relief", (char *) NULL, (char *) NULL,
	"flat", Tk_Offset(TkTextTag, relief), TK_CONFIG_DONT_SET_DEFAULT},
    {TK_CONFIG_BOOLEAN, "-underline", (char *) NULL, (char *) NULL,
	"false", Tk_Offset(TkTextTag, underline), TK_CONFIG_DONT_SET_DEFAULT},
    {TK_CONFIG_END, (char *) NULL, (char *) NULL, (char *) NULL,
	(char *) NULL, 0, 0}
};


/*
 * The following definition specifies the maximum number of characters
 * needed in a string to hold a position specifier.
 */

#define POS_CHARS 30

/*
 * Forward declarations for procedures defined later in this file:
 */

static void		ChangeTagPriority _ANSI_ARGS_((TkText *textPtr,
			    TkTextTag *tagPtr, int prio));
static TkTextTag *	FindTag _ANSI_ARGS_((Tcl_Interp *interp,
			    TkText *textPtr, char *tagName));
static void		SortTags _ANSI_ARGS_((int numTags,
			    TkTextTag **tagArrayPtr));
static int		TagSortProc _ANSI_ARGS_((CONST VOID *first,
			    CONST VOID *second));
static void		TextDoEvent _ANSI_ARGS_((TkText *textPtr,
			    XEvent *eventPtr));

/*
 *--------------------------------------------------------------
 *
 * TkTextTagCmd --
 *
 *	This procedure is invoked to process the "tag" options of
 *	the widget command for text widgets. See the user documentation
 *	for details on what it does.
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
TkTextTagCmd(textPtr, interp, argc, argv)
    register TkText *textPtr;	/* Information about text widget. */
    Tcl_Interp *interp;		/* Current interpreter. */
    int argc;			/* Number of arguments. */
    char **argv;		/* Argument strings.  Someone else has already
				 * parsed this command enough to know that
				 * argv[1] is "tag". */
{
    int length, line1, ch1, line2, ch2, i, addTag;
    char c;
    char *fullOption;
    register TkTextTag *tagPtr;

    if (argc < 3) {
	Tcl_AppendResult(interp, "wrong # args: should be \"",
		argv[0], " tag option ?arg arg ...?\"", (char *) NULL);
	return TCL_ERROR;
    }
    c = argv[2][0];
    length = strlen(argv[2]);
    if ((c == 'a') && (strncmp(argv[2], "add", length) == 0)) {
	fullOption = "add";
	addTag = 1;

	addAndRemove:
	if ((argc != 5) && (argc != 6)) {
	    Tcl_AppendResult(interp, "wrong # args: should be \"",
		    argv[0], " tag ", fullOption, " tagName index1 ?index2?\"",
		    (char *) NULL);
	    return TCL_ERROR;
	}
	tagPtr = TkTextCreateTag(textPtr, argv[3]);
	if (TkTextGetIndex(interp, textPtr, argv[4], &line1, &ch1) != TCL_OK) {
	    return TCL_ERROR;
	}
	if (argc == 6) {
	    if (TkTextGetIndex(interp, textPtr, argv[5], &line2, &ch2)
		    != TCL_OK) {
		return TCL_ERROR;
	    }
	} else {
	    line2 = line1;
	    ch2 = ch1+1;
	}
	if (TK_TAG_AFFECTS_DISPLAY(tagPtr)) {
	    TkTextRedrawTag(textPtr, line1, ch1, line2, ch2, tagPtr, !addTag);
	}
	TkBTreeTag(textPtr->tree, line1, ch1, line2, ch2, tagPtr, addTag);

	/*
	 * If the tag is "sel" then grab the selection if we're supposed
	 * to export it and don't already have it.  Also, invalidate
	 * partially-completed selection retrievals.
	 */

	if (tagPtr == textPtr->selTagPtr) {
	    if (addTag && textPtr->exportSelection
		    && !(textPtr->flags & GOT_SELECTION)) {
		Tk_OwnSelection(textPtr->tkwin, TkTextLostSelection,
			(ClientData) textPtr);
		textPtr->flags |= GOT_SELECTION;
	    }
	    textPtr->selOffset = -1;
	}
    } else if ((c == 'b') && (strncmp(argv[2], "bind", length) == 0)) {
	if ((argc < 4) || (argc > 6)) {
	    Tcl_AppendResult(interp, "wrong # args: should be \"",
		    argv[0], " tag bind tagName ?sequence? ?command?\"",
		    (char *) NULL);
	    return TCL_ERROR;
	}
	tagPtr = TkTextCreateTag(textPtr, argv[3]);

	/*
	 * Make a binding table if the widget doesn't already have
	 * one.
	 */

	if (textPtr->bindingTable == NULL) {
	    textPtr->bindingTable = Tk_CreateBindingTable(interp);
	}

	if (argc == 6) {
	    int append = 0;
	    unsigned long mask;

	    if (argv[5][0] == 0) {
		return Tk_DeleteBinding(interp, textPtr->bindingTable,
			(ClientData) tagPtr, argv[4]);
	    }
	    if (argv[5][0] == '+') {
		argv[5]++;
		append = 1;
	    }
	    mask = Tk_CreateBinding(interp, textPtr->bindingTable,
		    (ClientData) tagPtr, argv[4], argv[5], append);
	    if (mask == 0) {
		return TCL_ERROR;
	    }
	    if (mask & ~(ButtonMotionMask|Button1MotionMask|Button2MotionMask
		    |Button3MotionMask|Button4MotionMask|Button5MotionMask
		    |ButtonPressMask|ButtonReleaseMask|EnterWindowMask
		    |LeaveWindowMask|KeyPressMask|KeyReleaseMask
		    |PointerMotionMask)) {
		Tk_DeleteBinding(interp, textPtr->bindingTable,
			(ClientData) tagPtr, argv[4]);
		Tcl_ResetResult(interp);
		Tcl_AppendResult(interp, "requested illegal events; ",
			"only key, button, motion, and enter/leave ",
			"events may be used", (char *) NULL);
		return TCL_ERROR;
	    }
	} else if (argc == 5) {
	    char *command;
    
	    command = Tk_GetBinding(interp, textPtr->bindingTable,
		    (ClientData) tagPtr, argv[4]);
	    if (command == NULL) {
		return TCL_ERROR;
	    }
	    interp->result = command;
	} else {
	    Tk_GetAllBindings(interp, textPtr->bindingTable,
		    (ClientData) tagPtr);
	}
    } else if ((c == 'c') && (strncmp(argv[2], "configure", length) == 0)) {
	if (argc < 4) {
	    Tcl_AppendResult(interp, "wrong # args: should be \"",
		    argv[0], " tag configure tagName ?option? ?value? ",
		    "?option value ...?\"", (char *) NULL);
	    return TCL_ERROR;
	}
	tagPtr = TkTextCreateTag(textPtr, argv[3]);
	if (argc == 4) {
	    return Tk_ConfigureInfo(interp, textPtr->tkwin, tagConfigSpecs,
		    (char *) tagPtr, (char *) NULL, 0);
	} else if (argc == 5) {
	    return Tk_ConfigureInfo(interp, textPtr->tkwin, tagConfigSpecs,
		    (char *) tagPtr, argv[4], 0);
	} else {
	    int result;

	    result = Tk_ConfigureWidget(interp, textPtr->tkwin, tagConfigSpecs,
		    argc-4, argv+4, (char *) tagPtr, 0);
	    /*
	     * If the "sel" tag was changed, be sure to mirror information
	     * from the tag back into the text widget record.   NOTE: we
	     * don't have to free up information in the widget record
	     * before overwriting it, because it was mirrored in the tag
	     * and hence freed when the tag field was overwritten.
	     */

	    if (tagPtr == textPtr->selTagPtr) {
		textPtr->selBorder = tagPtr->border;
		textPtr->selBorderWidth = tagPtr->borderWidth;
		textPtr->selFgColorPtr = tagPtr->fgColor;
	    }
	    TkTextRedrawTag(textPtr, 0, 0, TkBTreeNumLines(textPtr->tree),
		    0, tagPtr, 1);
	    return result;
	}
    } else if ((c == 'd') && (strncmp(argv[2], "delete", length) == 0)) {
	Tcl_HashEntry *hPtr;

	if (argc < 4) {
	    Tcl_AppendResult(interp, "wrong # args: should be \"",
		    argv[0], " tag delete tagName tagName ...\"",
		    (char *) NULL);
	    return TCL_ERROR;
	}
	for (i = 3; i < argc; i++) {
	    hPtr = Tcl_FindHashEntry(&textPtr->tagTable, argv[i]);
	    if (hPtr == NULL) {
		continue;
	    }
	    tagPtr = (TkTextTag *) Tcl_GetHashValue(hPtr);
	    if (tagPtr == textPtr->selTagPtr) {
		interp->result = "can't delete selection tag";
		return TCL_ERROR;
	    }
	    if (TK_TAG_AFFECTS_DISPLAY(tagPtr)) {
		TkTextRedrawTag(textPtr, 0, 0, TkBTreeNumLines(textPtr->tree),
			0, tagPtr, 1);
	    }
	    TkBTreeTag(textPtr->tree, 0, 0, TkBTreeNumLines(textPtr->tree),
		    0, tagPtr, 0);
	    Tcl_DeleteHashEntry(hPtr);
	    if (textPtr->bindingTable != NULL) {
		Tk_DeleteAllBindings(textPtr->bindingTable,
			(ClientData) tagPtr);
	    }
	
	    /*
	     * Update the tag priorities to reflect the deletion of this tag.
	     */

	    ChangeTagPriority(textPtr, tagPtr, textPtr->numTags-1);
	    textPtr->numTags -= 1;
	    TkTextFreeTag(tagPtr);
	}
    } else if ((c == 'l') && (strncmp(argv[2], "lower", length) == 0)) {
	TkTextTag *tagPtr2;
	int prio;

	if ((argc != 4) && (argc != 5)) {
	    Tcl_AppendResult(interp, "wrong # args: should be \"",
		    argv[0], " tag lower tagName ?belowThis?\"",
		    (char *) NULL);
	    return TCL_ERROR;
	}
	    tagPtr = FindTag(interp, textPtr, argv[3]);
	if (tagPtr == NULL) {
	    return TCL_ERROR;
	}
	if (argc == 5) {
	    tagPtr2 = FindTag(interp, textPtr, argv[4]);
	    if (tagPtr2 == NULL) {
		return TCL_ERROR;
	    }
	    if (tagPtr->priority < tagPtr2->priority) {
		prio = tagPtr2->priority - 1;
	    } else {
		prio = tagPtr2->priority;
	    }
	} else {
	    prio = 0;
	}
	ChangeTagPriority(textPtr, tagPtr, prio);
	TkTextRedrawTag(textPtr, 0, 0, TkBTreeNumLines(textPtr->tree),
		0, tagPtr, 1);
    } else if ((c == 'n') && (strncmp(argv[2], "names", length) == 0)
	    && (length >= 2)) {
	TkTextTag **arrayPtr;
	int arraySize;
	TkTextLine *linePtr;

	if ((argc != 3) && (argc != 4)) {
	    Tcl_AppendResult(interp, "wrong # args: should be \"",
		    argv[0], " tag names ?index?\"",
		    (char *) NULL);
	    return TCL_ERROR;
	}
	if (argc == 3) {
	    Tcl_HashSearch search;
	    Tcl_HashEntry *hPtr;

	    arrayPtr = (TkTextTag **) ckalloc((unsigned)
		    (textPtr->numTags * sizeof(TkTextTag *)));
	    for (i = 0, hPtr = Tcl_FirstHashEntry(&textPtr->tagTable, &search);
		    hPtr != NULL; i++, hPtr = Tcl_NextHashEntry(&search)) {
		arrayPtr[i] = (TkTextTag *) Tcl_GetHashValue(hPtr);
	    }
	    arraySize = textPtr->numTags;
	} else {
	    if (TkTextGetIndex(interp, textPtr, argv[3], &line1, &ch1)
		    != TCL_OK) {
		return TCL_ERROR;
	    }
	    linePtr = TkBTreeFindLine(textPtr->tree, line1);
	    if (linePtr == NULL) {
		return TCL_OK;
	    }
	    arrayPtr = TkBTreeGetTags(textPtr->tree, linePtr, ch1, &arraySize);
	    if (arrayPtr == NULL) {
		return TCL_OK;
	    }
	}
	SortTags(arraySize, arrayPtr);
	for (i = 0; i < arraySize; i++) {
	    tagPtr = arrayPtr[i];
	    Tcl_AppendElement(interp, tagPtr->name, 0);
	}
	ckfree((char *) arrayPtr);
    } else if ((c == 'n') && (strncmp(argv[2], "nextrange", length) == 0)
	    && (length >= 2)) {
	TkTextSearch tSearch;
	char position[POS_CHARS];

	if ((argc != 5) && (argc != 6)) {
	    Tcl_AppendResult(interp, "wrong # args: should be \"",
		    argv[0], " tag nextrange tagName index1 ?index2?\"",
		    (char *) NULL);
	    return TCL_ERROR;
	}
	tagPtr = FindTag((Tcl_Interp *) NULL, textPtr, argv[3]);
	if (tagPtr == NULL) {
	    return TCL_OK;
	}
	if (TkTextGetIndex(interp, textPtr, argv[4], &line1, &ch1) != TCL_OK) {
	    return TCL_ERROR;
	}
	if (argc == 5) {
	    line2 = TkBTreeNumLines(textPtr->tree);
	    ch2 = 0;
	} else if (TkTextGetIndex(interp, textPtr, argv[5], &line2, &ch2)
		!= TCL_OK) {
	    return TCL_ERROR;
	}

	/*
	 * The search below is a bit tricky.  Rather than use the B-tree
	 * facilities to stop the search at line2.ch2, let it search up
	 * until the end of the file but check for a position past line2.ch2
	 * ourselves.  The reason for doing it this way is that we only
	 * care whether the *start* of the range is before line2.ch2;  once
	 * we find the start, we don't want TkBTreeNextTag to abort the
	 * search because the end of the range is after line2.ch2.
	 */

	TkBTreeStartSearch(textPtr->tree, line1, ch1,
		TkBTreeNumLines(textPtr->tree), 0, tagPtr, &tSearch);
	if (!TkBTreeNextTag(&tSearch)) {
	     return TCL_OK;
	}
	if (!TkBTreeCharTagged(tSearch.linePtr, tSearch.ch1, tagPtr)) {
	    if (!TkBTreeNextTag(&tSearch)) {
		return TCL_OK;
	    }
	}
	if ((tSearch.line1 > line2) || ((tSearch.line1 == line2)
		&& (tSearch.ch1 >= ch2))) {
	    return TCL_OK;
	}
	TkTextPrintIndex(tSearch.line1, tSearch.ch1, position);
	Tcl_AppendElement(interp, position, 0);
	TkBTreeNextTag(&tSearch);
	TkTextPrintIndex(tSearch.line1, tSearch.ch1, position);
	Tcl_AppendElement(interp, position, 0);
    } else if ((c == 'r') && (strncmp(argv[2], "raise", length) == 0)
	    && (length >= 3)) {
	TkTextTag *tagPtr2;
	int prio;

	if ((argc != 4) && (argc != 5)) {
	    Tcl_AppendResult(interp, "wrong # args: should be \"",
		    argv[0], " tag raise tagName ?aboveThis?\"",
		    (char *) NULL);
	    return TCL_ERROR;
	}
	tagPtr = FindTag(interp, textPtr, argv[3]);
	if (tagPtr == NULL) {
	    return TCL_ERROR;
	}
	if (argc == 5) {
	    tagPtr2 = FindTag(interp, textPtr, argv[4]);
	    if (tagPtr2 == NULL) {
		return TCL_ERROR;
	    }
	    if (tagPtr->priority <= tagPtr2->priority) {
		prio = tagPtr2->priority;
	    } else {
		prio = tagPtr2->priority + 1;
	    }
	} else {
	    prio = textPtr->numTags-1;
	}
	ChangeTagPriority(textPtr, tagPtr, prio);
	TkTextRedrawTag(textPtr, 0, 0, TkBTreeNumLines(textPtr->tree),
		0, tagPtr, 1);
    } else if ((c == 'r') && (strncmp(argv[2], "ranges", length) == 0)
	    && (length >= 3)) {
	TkTextSearch tSearch;
	char position[POS_CHARS];

	if (argc != 4) {
	    Tcl_AppendResult(interp, "wrong # args: should be \"",
		    argv[0], " tag ranges tagName\"", (char *) NULL);
	    return TCL_ERROR;
	}
	tagPtr = FindTag((Tcl_Interp *) NULL, textPtr, argv[3]);
	if (tagPtr == NULL) {
	    return TCL_OK;
	}
	TkBTreeStartSearch(textPtr->tree, 0, 0, TkBTreeNumLines(textPtr->tree),
		0, tagPtr, &tSearch);
	while (TkBTreeNextTag(&tSearch)) {
	    TkTextPrintIndex(tSearch.line1, tSearch.ch1, position);
	    Tcl_AppendElement(interp, position, 0);
	}
    } else if ((c == 'r') && (strncmp(argv[2], "remove", length) == 0)
	    && (length >= 2)) {
	fullOption = "remove";
	addTag = 0;
	goto addAndRemove;
    } else {
	Tcl_AppendResult(interp, "bad tag option \"", argv[2],
		"\":  must be add, bind, configure, delete, lower, ",
		"names, nextrange, raise, ranges, or remove",
		(char *) NULL);
	return TCL_ERROR;
    }
    return TCL_OK;
}

/*
 *----------------------------------------------------------------------
 *
 * TkTextCreateTag --
 *
 *	Find the record describing a tag within a given text widget,
 *	creating a new record if one doesn't already exist.
 *
 * Results:
 *	The return value is a pointer to the TkTextTag record for tagName.
 *
 * Side effects:
 *	A new tag record is created if there isn't one already defined
 *	for tagName.
 *
 *----------------------------------------------------------------------
 */

TkTextTag *
TkTextCreateTag(textPtr, tagName)
    TkText *textPtr;		/* Widget in which tag is being used. */
    char *tagName;		/* Name of desired tag. */
{
    register TkTextTag *tagPtr;
    Tcl_HashEntry *hPtr;
    int new;

    hPtr = Tcl_CreateHashEntry(&textPtr->tagTable, tagName, &new);
    if (!new) {
	return (TkTextTag *) Tcl_GetHashValue(hPtr);
    }

    /*
     * No existing entry.  Create a new one, initialize it, and add a
     * pointer to it to the hash table entry.
     */

    tagPtr = (TkTextTag *) ckalloc(sizeof(TkTextTag));
    tagPtr->name = Tcl_GetHashKey(&textPtr->tagTable, hPtr);
    tagPtr->priority = textPtr->numTags;
    tagPtr->border = NULL;
    tagPtr->borderWidth = 1;
    tagPtr->relief = TK_RELIEF_FLAT;
    tagPtr->bgStipple = None;
    tagPtr->fgColor = NULL;
    tagPtr->fontPtr = NULL;
    tagPtr->fgStipple = None;
    tagPtr->underline = 0;
    textPtr->numTags++;
    Tcl_SetHashValue(hPtr, tagPtr);
    return tagPtr;
}

/*
 *----------------------------------------------------------------------
 *
 * FindTag --
 *
 *	See if tag is defined for a given widget.
 *
 * Results:
 *	If tagName is defined in textPtr, a pointer to its TkTextTag
 *	structure is returned.  Otherwise NULL is returned and an
 *	error message is recorded in interp->result unless interp
 *	is NULL.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

static TkTextTag *
FindTag(interp, textPtr, tagName)
    Tcl_Interp *interp;		/* Interpreter to use for error message;
				 * if NULL, then don't record an error
				 * message. */
    TkText *textPtr;		/* Widget in which tag is being used. */
    char *tagName;		/* Name of desired tag. */
{
    Tcl_HashEntry *hPtr;

    hPtr = Tcl_FindHashEntry(&textPtr->tagTable, tagName);
    if (hPtr != NULL) {
	return (TkTextTag *) Tcl_GetHashValue(hPtr);
    }
    if (interp != NULL) {
	Tcl_AppendResult(interp, "tag \"", tagName,
		"\" isn't defined in text widget", (char *) NULL);
    }
    return NULL;
}

/*
 *----------------------------------------------------------------------
 *
 * TkTextFreeTag --
 *
 *	This procedure is called when a tag is deleted to free up the
 *	memory and other resources associated with the tag.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Memory and other resources are freed.
 *
 *----------------------------------------------------------------------
 */

void
TkTextFreeTag(tagPtr)
    register TkTextTag *tagPtr;		/* Tag being deleted. */
{
    if (tagPtr->border != None) {
	Tk_Free3DBorder(tagPtr->border);
    }
    if (tagPtr->bgStipple != None) {
	Tk_FreeBitmap(tagPtr->bgStipple);
    }
    if (tagPtr->fgColor != None) {
	Tk_FreeColor(tagPtr->fgColor);
    }
    if (tagPtr->fgStipple != None) {
	Tk_FreeBitmap(tagPtr->fgStipple);
    }
    ckfree((char *) tagPtr);
}

/*
 *----------------------------------------------------------------------
 *
 * SortTags --
 *
 *	This procedure sorts an array of tag pointers in increasing
 *	order of priority, optimizing for the common case where the
 *	array is small.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

static void
SortTags(numTags, tagArrayPtr)
    int numTags;		/* Number of tag pointers at *tagArrayPtr. */
    TkTextTag **tagArrayPtr;	/* Pointer to array of pointers. */
{
    int i, j, prio;
    register TkTextTag **tagPtrPtr;
    TkTextTag **maxPtrPtr, *tmp;

    if (numTags < 2) {
	return;
    }
    if (numTags < 20) {
	for (i = numTags-1; i > 0; i--, tagArrayPtr++) {
	    maxPtrPtr = tagPtrPtr = tagArrayPtr;
	    prio = tagPtrPtr[0]->priority;
	    for (j = i, tagPtrPtr++; j > 0; j--, tagPtrPtr++) {
		if (tagPtrPtr[0]->priority < prio) {
		    prio = tagPtrPtr[0]->priority;
		    maxPtrPtr = tagPtrPtr;
		}
	    }
	    tmp = *maxPtrPtr;
	    *maxPtrPtr = *tagArrayPtr;
	    *tagArrayPtr = tmp;
	}
    } else {
	qsort((VOID *) tagArrayPtr, numTags, sizeof (TkTextTag *),
		    TagSortProc);
    }
}

/*
 *----------------------------------------------------------------------
 *
 * TagSortProc --
 *
 *	This procedure is called by qsort when sorting an array of
 *	tags in priority order.
 *
 * Results:
 *	The return value is -1 if the first argument should be before
 *	the second element (i.e. it has lower priority), 0 if it's
 *	equivalent (this should never happen!), and 1 if it should be
 *	after the second element.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

static int
TagSortProc(first, second)
    CONST VOID *first, *second;		/* Elements to be compared. */
{
    TkTextTag *tagPtr1, *tagPtr2;

    tagPtr1 = * (TkTextTag **) first;
    tagPtr2 = * (TkTextTag **) second;
    return tagPtr1->priority - tagPtr2->priority;
}

/*
 *----------------------------------------------------------------------
 *
 * ChangeTagPriority --
 *
 *	This procedure changes the priority of a tag by modifying
 *	its priority and all other ones whose priority is affected
 *	by the change.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Priorities may be changed for some or all of the tags in
 *	textPtr.  The tags will be arranged so that there is exactly
 *	one tag at each priority level between 0 and textPtr->numTags-1,
 *	with tagPtr at priority "prio".
 *
 *----------------------------------------------------------------------
 */

static void
ChangeTagPriority(textPtr, tagPtr, prio)
    TkText *textPtr;			/* Information about text widget. */
    TkTextTag *tagPtr;			/* Tag whose priority is to be
					 * changed. */
    int prio;				/* New priority for tag. */
{
    int low, high, delta;
    register TkTextTag *tagPtr2;
    Tcl_HashEntry *hPtr;
    Tcl_HashSearch search;

    if (prio < 0) {
	prio = 0;
    }
    if (prio >= textPtr->numTags) {
	prio = textPtr->numTags-1;
    }
    if (prio == tagPtr->priority) {
	return;
    } else if (prio < tagPtr->priority) {
	low = prio;
	high = tagPtr->priority-1;
	delta = 1;
    } else {
	low = tagPtr->priority+1;
	high = prio;
	delta = -1;
    }
    for (hPtr = Tcl_FirstHashEntry(&textPtr->tagTable, &search);
	    hPtr != NULL; hPtr = Tcl_NextHashEntry(&search)) {
	tagPtr2 = (TkTextTag *) Tcl_GetHashValue(hPtr);
	if ((tagPtr2->priority >= low) && (tagPtr2->priority <= high)) {
	    tagPtr2->priority += delta;
	}
    }
    tagPtr->priority = prio;
}

/*
 *--------------------------------------------------------------
 *
 * TkTextBindProc --
 *
 *	This procedure is invoked by the Tk dispatcher to handle
 *	events associated with bindings on items.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Depends on the command invoked as part of the binding
 *	(if there was any).
 *
 *--------------------------------------------------------------
 */

void
TkTextBindProc(clientData, eventPtr)
    ClientData clientData;		/* Pointer to canvas structure. */
    XEvent *eventPtr;			/* Pointer to X event that just
					 * happened. */
{
    TkText *textPtr = (TkText *) clientData;
    int repick  = 0;

    Tk_Preserve((ClientData) textPtr);

    /*
     * This code simulates grabs for mouse buttons by refusing to
     * pick a new current character between the time a mouse button goes
     * down and the time when the last mouse button is released.
     */

    if (eventPtr->type == ButtonPress) {
	textPtr->flags |= BUTTON_DOWN;
    } else if (eventPtr->type == ButtonRelease) {
	int mask;

	switch (eventPtr->xbutton.button) {
	    case Button1:
		mask = Button1Mask;
		break;
	    case Button2:
		mask = Button2Mask;
		break;
	    case Button3:
		mask = Button3Mask;
		break;
	    case Button4:
		mask = Button4Mask;
		break;
	    case Button5:
		mask = Button5Mask;
		break;
	    default:
		mask = 0;
		break;
	}
	if ((eventPtr->xbutton.state & (Button1Mask|Button2Mask
		|Button3Mask|Button4Mask|Button5Mask)) == mask) {
	    textPtr->flags &= ~BUTTON_DOWN;
	    repick = 1;
	}
    } else if ((eventPtr->type == EnterNotify)
	    || (eventPtr->type == LeaveNotify)) {
	TkTextPickCurrent(textPtr, eventPtr);
	goto done;
    } else if (eventPtr->type == MotionNotify) {
	TkTextPickCurrent(textPtr, eventPtr);
    }
    TextDoEvent(textPtr, eventPtr);
    if (repick) {
	unsigned int oldState;

	oldState = eventPtr->xbutton.state;
	eventPtr->xbutton.state &= ~(Button1Mask|Button2Mask
		|Button3Mask|Button4Mask|Button5Mask);
	TkTextPickCurrent(textPtr, eventPtr);
	eventPtr->xbutton.state = oldState;
    }

    done:
    Tk_Release((ClientData) textPtr);
}

/*
 *--------------------------------------------------------------
 *
 * TkTextPickCurrent --
 *
 *	Find the topmost item in a canvas that contains a given
 *	location and mark the the current item.  If the current
 *	item has changed, generate a fake exit event on the old
 *	current item and a fake enter event on the new current
 *	item.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The current item for textPtr may change.  If it does,
 *	then the commands associated with item entry and exit
 *	could do just about anything.
 *
 *--------------------------------------------------------------
 */

void
TkTextPickCurrent(textPtr, eventPtr)
    register TkText *textPtr;		/* Text widget in which to select
					 * current character. */
    XEvent *eventPtr;			/* Event describing location of
					 * mouse cursor.  Must be EnterWindow,
					 * LeaveWindow, ButtonRelease, or
					 * MotionNotify. */
{
    TkTextLine *linePtr;
    int ch;

    /*
     * If a button is down, then don't do anything at all;  we'll be
     * called again when all buttons are up, and we can repick then.
     * This implements a form of mouse grabbing.
     */

    if (textPtr->flags & BUTTON_DOWN) {
	return;
    }

    /*
     * Save information about this event in the widget for use if we have
     * to synthesize more enter and leave events later (e.g. because a
     * character was deleting, causing a new character to be underneath
     * the mouse cursor).  Also translate MotionNotify events into
     * EnterNotify events, since that's what gets reported to event
     * handlers when the current character changes.
     */

    if (eventPtr != &textPtr->pickEvent) {
	if ((eventPtr->type == MotionNotify)
		|| (eventPtr->type == ButtonRelease)) {
	    textPtr->pickEvent.xcrossing.type = EnterNotify;
	    textPtr->pickEvent.xcrossing.serial = eventPtr->xmotion.serial;
	    textPtr->pickEvent.xcrossing.send_event
		    = eventPtr->xmotion.send_event;
	    textPtr->pickEvent.xcrossing.display = eventPtr->xmotion.display;
	    textPtr->pickEvent.xcrossing.window = eventPtr->xmotion.window;
	    textPtr->pickEvent.xcrossing.root = eventPtr->xmotion.root;
	    textPtr->pickEvent.xcrossing.subwindow = None;
	    textPtr->pickEvent.xcrossing.time = eventPtr->xmotion.time;
	    textPtr->pickEvent.xcrossing.x = eventPtr->xmotion.x;
	    textPtr->pickEvent.xcrossing.y = eventPtr->xmotion.y;
	    textPtr->pickEvent.xcrossing.x_root = eventPtr->xmotion.x_root;
	    textPtr->pickEvent.xcrossing.y_root = eventPtr->xmotion.y_root;
	    textPtr->pickEvent.xcrossing.mode = NotifyNormal;
	    textPtr->pickEvent.xcrossing.detail = NotifyNonlinear;
	    textPtr->pickEvent.xcrossing.same_screen
		    = eventPtr->xmotion.same_screen;
	    textPtr->pickEvent.xcrossing.focus = False;
	    textPtr->pickEvent.xcrossing.state = eventPtr->xmotion.state;
	} else  {
	    textPtr->pickEvent = *eventPtr;
	}
    }

    linePtr = NULL;
    if (textPtr->pickEvent.type != LeaveNotify) {
	linePtr = TkTextCharAtLoc(textPtr, textPtr->pickEvent.xcrossing.x,
		textPtr->pickEvent.xcrossing.y, &ch);
    }

    /*
     * Simulate a LeaveNotify event on the previous current character and
     * an EnterNotify event on the new current character.  Also, move the
     * "current" mark to its new place.
     */

    if (textPtr->flags & IN_CURRENT) {
	if ((linePtr == textPtr->currentAnnotPtr->linePtr)
		&& (ch == textPtr->currentAnnotPtr->ch)) {
	    return;
	}
    } else {
	if (linePtr == NULL) {
	    return;
	}
    }
    if (textPtr->flags & IN_CURRENT) {
	XEvent event;

	event = textPtr->pickEvent;
	event.type = LeaveNotify;
	TextDoEvent(textPtr, &event);
	textPtr->flags &= ~IN_CURRENT;
    }
    if (linePtr != NULL) {
	XEvent event;

	TkBTreeRemoveAnnotation(textPtr->currentAnnotPtr);
	textPtr->currentAnnotPtr->linePtr = linePtr;
	textPtr->currentAnnotPtr->ch = ch;
	TkBTreeAddAnnotation(textPtr->currentAnnotPtr);
	event = textPtr->pickEvent;
	event.type = EnterNotify;
	TextDoEvent(textPtr, &event);
	textPtr->flags |= IN_CURRENT;
    }
}

/*
 *----------------------------------------------------------------------
 *
 * TkTextUnpickCurrent --
 *
 *	This procedure is called when the "current" character is
 *	deleted:  it synthesizes a "leave" event for the character.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	A binding associated with one of the tags on the current
 *	character may be triggered.
 *
 *----------------------------------------------------------------------
 */

void
TkTextUnpickCurrent(textPtr)
    TkText *textPtr;		/* Text widget information. */
{
    if (textPtr->flags & IN_CURRENT) {
	XEvent event;

	event = textPtr->pickEvent;
	event.type = LeaveNotify;
	TextDoEvent(textPtr, &event);
	textPtr->flags &= ~IN_CURRENT;
    }
}

/*
 *--------------------------------------------------------------
 *
 * TextDoEvent --
 *
 *	This procedure is called to invoke binding processing
 *	for a new event that is associated with the current character
 *	for a text widget.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Depends on the bindings for the text.
 *
 *--------------------------------------------------------------
 */

static void
TextDoEvent(textPtr, eventPtr)
    TkText *textPtr;			/* Text widget in which event
					 * occurred. */
    XEvent *eventPtr;			/* Real or simulated X event that
					 * is to be processed. */
{
    TkTextTag **tagArrayPtr, **p1, **p2, *tmp;
    int numTags;

    if (textPtr->bindingTable == NULL) {
	return;
    }

    /*
     * Set up an array containing all of the tags that are associated
     * with the current character.  This array will be used to look
     * for bindings.  If there are no tags then there can't be any
     * bindings.
     */

    tagArrayPtr = TkBTreeGetTags(textPtr->tree,
	    textPtr->currentAnnotPtr->linePtr, textPtr->currentAnnotPtr->ch,
	    &numTags);
    if (numTags == 0) {
	return;
    }

    /*
     * Sort the array of tags.  SortTags sorts it backwards, so after it
     * returns we have to reverse the order in the array.
     */

    SortTags(numTags, tagArrayPtr);
    for (p1 = tagArrayPtr, p2 = tagArrayPtr + numTags - 1;
	    p1 < p2;  p1++, p2--) {
	tmp = *p1;
	*p1 = *p2;
	*p2 = tmp;
    }

    /*
     * Invoke the binding system, then free up the tag array.
     */

    Tk_BindEvent(textPtr->bindingTable, eventPtr, textPtr->tkwin,
	    numTags, (ClientData *) tagArrayPtr);
    ckfree((char *) tagArrayPtr);
}
