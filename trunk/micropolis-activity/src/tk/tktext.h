/*
 * tkText.h --
 *
 *	Declarations shared among the files that implement text
 *	widgets.
 *
 * Copyright 1992 Regents of the University of California.
 * Permission to use, copy, modify, and distribute this
 * software and its documentation for any purpose and without
 * fee is hereby granted, provided that the above copyright
 * notice appear in all copies.  The University of California
 * makes no representations about the suitability of this
 * software for any purpose.  It is provided "as is" without
 * express or implied warranty.
 *
 * $Header: /user6/ouster/wish/RCS/tkText.h,v 1.13 92/07/31 13:43:26 ouster Exp $ SPRITE (Berkeley)
 */

#ifndef _TKTEXT
#define _TKTEXT

#ifndef _TK
#include "tk.h"
#endif

/*
 * Opaque types for structures whose guts are only needed by a single
 * file:
 */

typedef struct TkTextBTree *TkTextBTree;

/*
 * The data structure below defines a single line of text (from newline
 * to newline, not necessarily what appears on one line of the screen).
 */

typedef struct TkTextLine {
    struct Node *parentPtr;		/* Pointer to parent node containing
					 * line. */
    struct TkTextLine *nextPtr;		/* Next in linked list of lines with
					 * same parent node in B-tree.  NULL
					 * means end of list. */
    struct TkAnnotation *annotPtr;	/* First in list of annotations for
					 * this line. */
    int numBytes;			/* Number of  bytes in line, including
					 * newline but not terminating NULL. */
    char bytes[4];			/* Contents of line, null-terminated.
					 * The actual length of the array will
					 * be as large as needed to hold the
					 * line.  THIS MUST BE THE LAST FIELD
					 * OF THE STRUCT. */
} TkTextLine;

/*
 * The structures below are used to describe annotations to the text
 * (such as marks and embedded windows).  Annotations are placed at
 * a given place in the text and then float to keep their position
 * as text is inserted and deleted.  Each actual annotation
 * contains a standard set of fields, plus a type-specific set of
 * fields.  The types are as follows:
 *
 * TK_ANNOT_TOGGLE -	Marks the beginning or end of a range of
 *			characters that have a given tag.
 * TK_ANNOT_MARK -	Holds information about a given "mark" (see
 *			user doc. for information on marks).
 * TK_ANNOT_WINDOW -	Holds information on a window embedded in the
 *			text.  Not implemented yet.
 */

typedef enum {TK_ANNOT_TOGGLE, TK_ANNOT_MARK, TK_ANNOT_WINDOW} TkAnnotType;

typedef struct TkAnnotation {
    TkAnnotType type;			/* Type of annotation. */
    TkTextLine *linePtr;		/* Pointer to line structure
					 * containing this annotation. */
    int ch;				/* Index of character that annotation
					 * is attached to (annotation is
					 * considered to be just before this
					 * character). */
    struct TkAnnotation *nextPtr;	/* Next in list of annotations for
					 * same line of text, or NULL if
					 * end of list. */
    union {				/* Type-specific information. */
	struct TkTextTag *tagPtr;	/* Type == TK_ANNOT_TOGGLE. */
	Tcl_HashEntry *hPtr;		/* Type == TK_ANNOT_MARK. */
    } info;
} TkAnnotation;

/*
 * One data structure of the following type is used for each tag that
 * is currently being used in a text widget.  These structures are kept
 * in textPtr->tagTable and referred to in other structures, like
 * TkTagToggles.
 */

typedef struct TkTextTag {
    char *name;			/* Name of this tag.  This field is actually
				 * a pointer to the key from the entry in
				 * textPtr->tagTable, so it needn't be freed
				 * explicitly. */
    int priority;		/* Priority of this tag within widget.  0
				 * means lowest priority.  Exactly one tag
				 * has each integer value between 0 and
				 * numTags-1. */

    /*
     * Information for displaying text with this tag.  The information
     * belows acts as an override on information specified by lower-priority
     * tags.  If no value is specified, then the next-lower-priority tag
     * on the text determins the value.  The text widget itself provides
     * defaults if no tag specifies an override.
     */

    Tk_3DBorder border;		/* Used for drawing background.  NULL means
				 * no value specified here. */
    int borderWidth;		/* Width of 3-D border for background. */
    int relief;			/* 3-D relief for background. */
    Pixmap bgStipple;		/* Stipple bitmap for background.  None
				 * means no value specified here. */
    XColor *fgColor;		/* Foreground color for text.  NULL means
				 * no value specified here. */
    XFontStruct *fontPtr;	/* Font for displaying text.  NULL means
				 * no value specified here. */
    Pixmap fgStipple;		/* Stipple bitmap for text and other
				 * foreground stuff.   None means no value
				 * specified here.*/
    int underline;		/* Non-zero means draw underline underneath
				 * text. */
} TkTextTag;

/*
 * The macro below determines whether or not a particular tag affects
 * the way information is displayed on the screen.  It's used, for
 * example, to determine when to redisplay in response to tag changes.
 */

#define TK_TAG_AFFECTS_DISPLAY(tagPtr) 					\
	(((tagPtr)->border != NULL) || ((tagPtr)->bgStipple != None)	\
	|| ((tagPtr)->fgColor != NULL) || ((tagPtr)->fontPtr != NULL) 	\
	|| ((tagPtr)->fgStipple != None) || ((tagPtr)->underline))

/*
 * The data structure below is used for searching a B-tree for transitions
 * on a single tag (or for all tag transitions).  No code outside of
 * tkTextBTree.c should ever modify any of the fields in these structures,
 * but it's OK to use them for read-only information.
 */

typedef struct TkTextSearch {
    TkTextBTree tree;			/* Tree being searched. */
    int line1, ch1;			/* Position of last tag returned
					 * by TkBTreeNextTag. */
    int line2, ch2;			/* Stop search after all tags at this
					 * character position have been
					 * processed. */
    TkTextTag *tagPtr;			/* Tag to search for (or tag found, if
					 * allTags is non-zero). */
    int allTags;			/* Non-zero means ignore tag check:
					 * search for transitions on all
					 * tags. */
    TkTextLine *linePtr;		/* Line currently being searched.  NULL
					 * means search is over. */
    TkAnnotation *annotPtr;		/* Pointer to next annotation to
					 * consider.  NULL means no annotations
					 * left in current line;  must go on
					 * to next line. */
} TkTextSearch;

/*
 * A data structure of the following type is kept for each text widget that
 * currently exists for this process:
 */

typedef struct TkText {
    Tk_Window tkwin;		/* Window that embodies the text.  NULL
				 * means that the window has been destroyed
				 * but the data structures haven't yet been
				 * cleaned up.*/
    Tcl_Interp *interp;		/* Interpreter associated with widget.  Used
				 * to delete widget command.  */
    TkTextBTree tree;		/* B-tree representation of text and tags for
				 * widget. */
    Tcl_HashTable tagTable;	/* Hash table that maps from tag names to
				 * pointers to TkTextTag structures. */
    int numTags;		/* Number of tags currently defined for
				 * widget;  needed to keep track of
				 * priorities. */
    Tcl_HashTable markTable;	/* Hash table that maps from mark names to
				 * pointer to TkAnnotation structures of
				 * type TK_ANNOT_MARK. */
    Tk_Uid state;		/* Normal or disabled.  Text is read-only
				 * when disabled. */

    /*
     * Default information for displaying (may be overridden by tags
     * applied to ranges of characters).
     */

    Tk_3DBorder border;		/* Structure used to draw 3-D border and
				 * default background. */
    int borderWidth;		/* Width of 3-D border to draw around entire
				 * widget. */
    int padX, padY;		/* Padding between text and window border. */
    int relief;			/* 3-d effect for border around entire
				 * widget: TK_RELIEF_RAISED etc. */
    Cursor cursor;		/* Current cursor for window, or None. */
    XColor *fgColor;		/* Default foreground color for text. */
    XFontStruct *fontPtr;	/* Default font for displaying text. */

    /*
     * Additional information used for displaying:
     */

    Tk_Uid wrapMode;		/* How to handle wrap-around.  Must be
				 * tkTextCharUid, tkTextNoneUid, or
				 * tkTextWordUid. */
    int width, height;		/* Desired dimensions for window, measured
				 * in characters. */
    int setGrid;		/* Non-zero means pass gridding information
				 * to window manager. */
    int prevWidth, prevHeight;	/* Last known dimensions of window;  used to
				 * detect changes in size. */
    TkTextLine *topLinePtr;	/* Text line that is supposed to be displayed
				 * at top of the window:  set only by
				 * tkTextDisp.c. */
    struct DInfo *dInfoPtr;	/* Additional information maintained by
				 * tkTextDisp.c. */
    Tk_TimerToken updateTimerToken; /* Added by Don to optimize rapid 
				     * updates. */
    
    /*
     * Information related to selection.
     */

    TkTextTag *selTagPtr;	/* Pointer to "sel" tag.  Used to tell when
				 * a new selection has been made. */
    Tk_3DBorder selBorder;	/* Border and background for selected
				 * characters.  This is a copy of information
				 * in *cursorTagPtr, so it shouldn't be
				 * explicitly freed. */
    int selBorderWidth;		/* Width of border around selection. */
    XColor *selFgColorPtr;	/* Foreground color for selected text.
				 * This is a copy of information in
				 * *cursorTagPtr, so it shouldn't be
				 * explicitly freed. */
    int exportSelection;	/* Non-zero means tie "sel" tag to X
				 * selection. */
    int selLine, selCh;		/* Used during multi-pass selection retrievals.
				 * These identify the next character to be
				 * returned from the selection. */
    int selOffset;		/* Offset in selection corresponding to
				 * selLine and selCh.  -1 means neither
				 * this information nor selLine or selCh
				 * is of any use. */

    /*
     * Information related to insertion cursor:
     */

    TkAnnotation *insertAnnotPtr;
				/* Always points to annotation for "insert"
				 * mark. */
    Tk_3DBorder insertBorder;	/* Used to draw vertical bar for insertion
				 * cursor. */
    int insertWidth;		/* Total width of insert cursor. */
    int insertBorderWidth;	/* Width of 3-D border around insert cursor. */
    int insertOnTime;		/* Number of milliseconds cursor should spend
				 * in "on" state for each blink. */
    int insertOffTime;		/* Number of milliseconds cursor should spend
				 * in "off" state for each blink. */
    Tk_TimerToken insertBlinkHandler;
				/* Timer handler used to blink cursor on and
				 * off. */

    /*
     * Information used for event bindings associated with tags:
     */

    Tk_BindingTable bindingTable;
				/* Table of all bindings currently defined
				 * for this widget.  NULL means that no
				 * bindings exist, so the table hasn't been
				 * created.  Each "object" used for this
				 * table is the address of a tag. */
    TkAnnotation *currentAnnotPtr;
				/* Pointer to annotation for "current" mark,
				 * or NULL if none. */
    XEvent pickEvent;		/* The event from which the current character
				 * was chosen.  Must be saved so that we
				 * can repick after insertions and deletions. */

    /*
     * Miscellaneous additional information:
     */

    char *yScrollCmd;		/* Prefix of command to issue to update
				 * vertical scrollbar when view changes. */
    int scanMarkLine;		/* Line that was at the top of the window
				 * when the scan started. */
    int scanMarkY;		/* Y-position of mouse at time scan started. */
    int flags;			/* Miscellaneous flags;  see below for
				 * definitions. */
} TkText;

/*
 * Flag values for TkText records:
 *
 * GOT_SELECTION:		Non-zero means we've already claimed the
 *				selection.
 * INSERT_ON:			Non-zero means insertion cursor should be
 *				displayed on screen.
 * GOT_FOCUS:			Non-zero means this window has the input
 *				focus.
 * BUTTON_DOWN:			1 means that a mouse button is currently
 *				down;  this is used to implement grabs
 *				for the duration of button presses.
 * IN_CURRENT:			1 means that an EnterNotify event has been
 *				delivered to the current character with
 *				no matching LeaveNotify event yet.
 */

#define GOT_SELECTION	1
#define INSERT_ON	2
#define GOT_FOCUS	4
#define BUTTON_DOWN	8
#define IN_CURRENT	0x10

/*
 * The constant below is used to specify a line when what is really
 * wanted is the entire text.  For now, just use a very big number.
 */

#define TK_END_OF_TEXT 1000000

/*
 * Declarations for variables shared among the text-related files:
 */

extern int tkBTreeDebug;
extern Tk_Uid tkTextCharUid;
extern Tk_Uid tkTextDisabledUid;
extern Tk_Uid tkTextNoneUid;
extern Tk_Uid tkTextNormalUid;
extern Tk_Uid tkTextWordUid;

/*
 * Declarations for procedures that are used by the text-related files
 * but shouldn't be used anywhere else in Tk (or by Tk clients):
 */

extern void		TkBTreeAddAnnotation _ANSI_ARGS_((
			    TkAnnotation *annotPtr));
extern int		TkBTreeCharTagged _ANSI_ARGS_((TkTextLine *linePtr,
			    int index, TkTextTag *tagPtr));
extern void		TkBTreeCheck _ANSI_ARGS_((TkTextBTree tree));
extern TkTextBTree	TkBTreeCreate _ANSI_ARGS_((void));
extern void		TkBTreeDestroy _ANSI_ARGS_((TkTextBTree tree));
extern void		TkBTreeDeleteChars _ANSI_ARGS_((TkTextBTree tree,
			    TkTextLine *line1Ptr, int ch1,
			    TkTextLine *line2Ptr, int ch2));
extern TkTextLine *	TkBTreeFindLine _ANSI_ARGS_((TkTextBTree tree,
			    int line));
extern TkTextTag **	TkBTreeGetTags _ANSI_ARGS_((TkTextBTree tree,
			    TkTextLine *linePtr, int ch, int *numTagsPtr));
extern void		TkBTreeInsertChars _ANSI_ARGS_((TkTextBTree tree,
			    TkTextLine *linePtr, int ch, char *string));
extern int		TkBTreeLineIndex _ANSI_ARGS_((TkTextLine *linePtr));
extern TkTextLine *	TkBTreeNextLine _ANSI_ARGS_((TkTextLine *linePtr));
extern int		TkBTreeNextTag _ANSI_ARGS_((TkTextSearch *searchPtr));
extern int		TkBTreeNumLines _ANSI_ARGS_((TkTextBTree tree));
extern void		TkBTreeRemoveAnnotation _ANSI_ARGS_((
			    TkAnnotation *annotPtr));
extern void		TkBTreeStartSearch _ANSI_ARGS_((TkTextBTree tree,
			    int line1, int ch1, int line2, int ch2,
			    TkTextTag *tagPtr, TkTextSearch *searchPtr));
extern void		TkBTreeTag _ANSI_ARGS_((TkTextBTree tree, int line1,
			    int ch1, int line2, int ch2, TkTextTag *tagPtr,
			    int add));
extern void		TkTextBindProc _ANSI_ARGS_((ClientData clientData,
			    XEvent *eventPtr));
extern TkTextLine *	TkTextCharAtLoc _ANSI_ARGS_((TkText *textPtr,
			    int x, int y, int *chPtr));
extern void		TkTextCreateDInfo _ANSI_ARGS_((TkText *textPtr));
extern TkTextTag *	TkTextCreateTag _ANSI_ARGS_((TkText *textPtr,
			    char *tagName));
extern void		TkTextFreeDInfo _ANSI_ARGS_((TkText *textPtr));
extern void		TkTextFreeTag _ANSI_ARGS_((TkTextTag *tagPtr));
extern int		TkTextGetIndex _ANSI_ARGS_((Tcl_Interp *interp,
			    TkText *textPtr, char *string, int *lineIndexPtr,
			    int *chPtr));
extern void		TkTextLinesChanged _ANSI_ARGS_((TkText *textPtr,
			    int first, int last));
extern void		TkTextLostSelection _ANSI_ARGS_((
			    ClientData clientData));
extern void		TkTextPickCurrent _ANSI_ARGS_((TkText *textPtr,
			    XEvent *eventPtr));
extern void		TkTextPrintIndex _ANSI_ARGS_((int line, int ch,
			    char *string));
extern TkTextLine *	TkTextRoundIndex _ANSI_ARGS_((TkText *textPtr,
			    int *lineIndexPtr, int *chPtr));
extern void		TkTextRedrawRegion _ANSI_ARGS_((TkText *textPtr,
			    int x, int y, int width, int height));
extern void		TkTextRedrawTag _ANSI_ARGS_((TkText *textPtr,
			    int line1, int ch1, int line2, int ch2,
			    TkTextTag *tagPtr, int withTag));
extern void		TkTextRelayoutWindow _ANSI_ARGS_((TkText *textPtr));
extern TkAnnotation *	TkTextSetMark _ANSI_ARGS_((TkText *textPtr, char *name,
			    int line, int ch));
extern void		TkTextSetView _ANSI_ARGS_((TkText *textPtr,
			    int line, int pickPlace));
extern int		TkTextTagCmd _ANSI_ARGS_((TkText *textPtr,
			    Tcl_Interp *interp, int argc, char **argv));
extern void		TkTextUnpickCurrent _ANSI_ARGS_((TkText *textPtr));

#endif /* _TKTEXT */
