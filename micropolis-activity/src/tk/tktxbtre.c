/* 
 * tkTextBTree.c --
 *
 *	This file contains code that manages the B-tree representation
 *	of text for Tk's text widget.  The B-tree holds both the text
 *	and tag information related to the text.
 *
 * Copyright 1992 Regents of the University of California
 * Permission to use, copy, modify, and distribute this
 * software and its documentation for any purpose and without
 * fee is hereby granted, provided that this copyright
 * notice appears in all copies.  The University of California
 * makes no representations about the suitability of this
 * software for any purpose.  It is provided "as is" without
 * express or implied warranty.
 */

#ifndef lint
static char rcsid[] = "$Header: /user6/ouster/wish/RCS/tkTextBTree.c,v 1.16 92/08/17 09:13:58 ouster Exp $ SPRITE (Berkeley)";
#endif /* not lint */

#include "tkint.h"
#include "tkconfig.h"
#include "tktext.h"


/*
 * The data structure below keeps summary information about one tag as part
 * of the tag information in a node.
 */

typedef struct Summary {
    TkTextTag *tagPtr;			/* Handle for tag. */
    int toggleCount;			/* Number of transitions into or
					 * out of this tag that occur in
					 * the subtree rooted at this node. */
    struct Summary *nextPtr;		/* Next in list of all tags for same
					 * node, or NULL if at end of list. */
} Summary;

/*
 * The data structure below defines a node in the B-tree representing
 * all of the lines in a text widget.
 */

typedef struct Node {
    struct Node *parentPtr;		/* Pointer to parent node, or NULL if
					 * this is the root. */
    struct Node *nextPtr;		/* Next in list of children of the
					 * same parent node, or NULL for end
					 * of list. */
    Summary *summaryPtr;		/* First in malloc-ed list of info
					 * about tags in this subtree (NULL if
					 * no tag info in the subtree). */
    int level;				/* Level of this node in the B-tree.
					 * 0 refers to the bottom of the tree
					 * (children are lines, not nodes). */
    union {				/* First in linked list of children. */
	struct Node *nodePtr;		/* Used if level > 0. */
	TkTextLine *linePtr;		/* Used if level == 0. */
    } children;
    int numChildren;			/* Number of children of this node. */
    int numLines;			/* Total number of lines (leaves) in
					 * the subtree rooted here. */
} Node;

/*
 * Upper and lower bounds on how many children a node may have:
 * rebalance when either of these limits is exceeded.  MAX_CHILDREN
 * should be twice MIN_CHILDREN and MIN_CHILDREN must be >= 2.
 */

#define MAX_CHILDREN 12
#define MIN_CHILDREN 6

/*
 * The data structure below defines an entire B-tree.
 */

typedef struct BTree {
    Node *rootPtr;			/* Pointer to root of B-tree. */
} BTree;

/*
 * The structure below is used to pass information between
 * TkBTreeGetTags and IncCount:
 */

typedef struct TagInfo {
    int numTags;			/* Number of tags for which there
					 * is currently information in
					 * tags and counts. */
    int arraySize;			/* Number of entries allocated for
					 * tags and counts. */
    TkTextTag **tagPtrs;		/* Array of tags seen so far.
					 * Malloc-ed. */
    int *counts;			/* Toggle count (so far) for each
					 * entry in tags.  Malloc-ed. */
} TagInfo;

/*
 * Macro to compute the space needed for a line that holds n non-null
 * characters:
 */

#define LINE_SIZE(n) ((unsigned) (sizeof(TkTextLine) - 3 + (n)))

/*
 * Variable that indicates whether to enable consistency checks for
 * debugging.
 */

int tkBTreeDebug = 0;

/*
 * Forward declarations for procedures defined in this file:
 */

static void		AddToggleToLine _ANSI_ARGS_((TkTextLine *linePtr,
			    int index, TkTextTag *tagPtr));
static void		ChangeNodeToggleCount _ANSI_ARGS_((Node *nodePtr,
			    TkTextTag *tagPtr, int delta));
static void		CheckNodeConsistency _ANSI_ARGS_((Node *nodePtr));
static void		DeleteSummaries _ANSI_ARGS_((Summary *tagPtr));
static void		DestroyNode _ANSI_ARGS_((Node *nodePtr));
static void		IncCount _ANSI_ARGS_((TkTextTag *tagPtr, int inc,
			    TagInfo *tagInfoPtr));
static void		Rebalance _ANSI_ARGS_((BTree *treePtr, Node *nodePtr));
static void		RecomputeNodeCounts _ANSI_ARGS_((Node *nodePtr));

/*
 *----------------------------------------------------------------------
 *
 * TkBTreeCreate --
 *
 *	This procedure is called to create a new text B-tree.
 *
 * Results:
 *	The return value is a pointer to a new B-tree containing
 *	one line with nothing but a newline character.
 *
 * Side effects:
 *	Memory is allocated and initialized.
 *
 *----------------------------------------------------------------------
 */

TkTextBTree
TkBTreeCreate()
{
    register BTree *treePtr;
    register Node *rootPtr;
    register TkTextLine *linePtr;

    rootPtr = (Node *) ckalloc(sizeof(Node));
    linePtr = (TkTextLine *) ckalloc(LINE_SIZE(1));
    rootPtr->parentPtr = NULL;
    rootPtr->nextPtr = NULL;
    rootPtr->summaryPtr = NULL;
    rootPtr->level = 0;
    rootPtr->children.linePtr = linePtr;
    rootPtr->numChildren = 1;
    rootPtr->numLines = 1;

    linePtr->parentPtr = rootPtr;
    linePtr->nextPtr = NULL;
    linePtr->annotPtr = NULL;
    linePtr->numBytes = 1;
    linePtr->bytes[0] = '\n';
    linePtr->bytes[1] = 0;

    treePtr = (BTree *) ckalloc(sizeof(BTree));
    treePtr->rootPtr = rootPtr;

    return (TkTextBTree) treePtr;
}

/*
 *----------------------------------------------------------------------
 *
 * TkBTreeDestroy --
 *
 *	Delete a B-tree, recycling all of the storage it contains.
 *
 * Results:
 *	The tree given by treePtr is deleted.  TreePtr should never
 *	again be used.
 *
 * Side effects:
 *	Memory is freed.
 *
 *----------------------------------------------------------------------
 */

void
TkBTreeDestroy(tree)
    TkTextBTree tree;			/* Pointer to tree to delete. */ 
{
    BTree *treePtr = (BTree *) tree;

    DestroyNode(treePtr->rootPtr);
    ckfree((char *) treePtr);
}

/*
 *----------------------------------------------------------------------
 *
 * DestroyNode --
 *
 *	This is a recursive utility procedure used during the deletion
 *	of a B-tree.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	All the storage for nodePtr and its descendants is freed.
 *
 *----------------------------------------------------------------------
 */

static void
DestroyNode(nodePtr)
    register Node *nodePtr;
{
    if (nodePtr->level == 0) {
	register TkTextLine *curPtr, *nextLinePtr;
	register TkAnnotation *annotPtr, *nextAnnotPtr;

	for (curPtr = nodePtr->children.linePtr; curPtr != NULL; ) {
	    nextLinePtr = curPtr->nextPtr;
	    for (annotPtr = curPtr->annotPtr; annotPtr != NULL; ) {
		nextAnnotPtr = annotPtr->nextPtr;
		if (annotPtr->type == TK_ANNOT_TOGGLE) {
		    ckfree((char *) annotPtr);
		}
		annotPtr = nextAnnotPtr;
	    }
	    ckfree((char *) curPtr);
	    curPtr = nextLinePtr;
	}
    } else {
	register Node *curPtr, *nextPtr;

	for (curPtr = nodePtr->children.nodePtr; curPtr != NULL; ) {
	    nextPtr = curPtr->nextPtr;
	    DestroyNode(curPtr);
	    curPtr = nextPtr;
	}
    }
    DeleteSummaries(nodePtr->summaryPtr);
    ckfree((char *) nodePtr);
}

/*
 *----------------------------------------------------------------------
 *
 * DeleteSummaries --
 *
 *	Free up all of the memory in a list of tag summaries associated
 *	with a node.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Storage is released.
 *
 *----------------------------------------------------------------------
 */

static void
DeleteSummaries(summaryPtr)
    register Summary *summaryPtr;	/* First in list of node's tag
					 * summaries. */
{
    register Summary *nextPtr;
    while (summaryPtr != NULL) {
	nextPtr = summaryPtr->nextPtr;
	ckfree((char *) summaryPtr);
	summaryPtr = nextPtr;
    }
}

/*
 *----------------------------------------------------------------------
 *
 * TkBTreeInsertChars --
 *
 *	Insert characters at a given position in a B-tree.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	NumBytes characters are added to the B-tree at the given
 *	character position.  This can cause the structure of the
 *	B-tree to change.
 *
 *----------------------------------------------------------------------
 */

void
TkBTreeInsertChars(tree, linePtr, ch, string)
    TkTextBTree tree;			/* B-tree in which to insert. */
    register TkTextLine *linePtr;	/* Pointer to line in which to
					 * insert. */
    int ch;				/* Index of character before which
					 * to insert.  Must not be after
					 * last character in line.*/
    char *string;			/* Pointer to bytes to insert (may
					 * contain newlines, must be null-
					 * terminated). */
{
    BTree *treePtr = (BTree *) tree;
    register Node *nodePtr;
    register TkAnnotation *annotPtr;
    TkTextLine *prevPtr;
    int newChunkLength;			/* # chars in current line being
					 * inserted. */
    register char *eol;			/* Pointer to last character in
					 * current line being inserted. */
    int changeToLineCount;		/* Counts change to total number of
					 * lines in file. */
    TkAnnotation *afterPtr;		/* List of annotations that occur
					 * at or after the insertion point
					 * in the line of the insertion. */
    int prefixLength, suffixLength, totalLength;
    register TkTextLine *newPtr;

    /*
     * Find the line just before the one where the insertion will occur
     * but with the same parent node (if there is one).  This is needed
     * so we can replace the insertion line with a new one.  Remove this
     * line from the list for its parent, since it's going to be discarded
     * when we're all done).
     */

    nodePtr = linePtr->parentPtr;
    prevPtr = nodePtr->children.linePtr;
    if (prevPtr == linePtr) {
	prevPtr = NULL;
	nodePtr->children.linePtr = linePtr->nextPtr;
    } else {
	for ( ; prevPtr->nextPtr != linePtr;  prevPtr = prevPtr->nextPtr) {
	    /* Empty loop body. */
	}
	prevPtr->nextPtr = linePtr->nextPtr;
    }

    /*
     * Break up the annotations for the insertion line into two pieces:
     * those before the insertion point, and those at or after the insertion
     * point.
     */

    afterPtr = NULL;
    if ((linePtr->annotPtr != NULL) && (linePtr->annotPtr->ch >= ch)) {
	afterPtr = linePtr->annotPtr;
	linePtr->annotPtr = NULL;
    } else {
	for (annotPtr = linePtr->annotPtr; annotPtr != NULL;
		annotPtr = annotPtr->nextPtr) {
	    if ((annotPtr->nextPtr != NULL)
		    && (annotPtr->nextPtr->ch >= ch)) {
		afterPtr = annotPtr->nextPtr;
		annotPtr->nextPtr = NULL;
		break;
	    }
	}
    }

    /*
     * Chop the string up into lines and insert each line individually.
     */

    changeToLineCount = -1;
    prefixLength = ch;
    while (1) {
	for (newChunkLength = 0, eol = string; *eol != 0; eol++) {
	    newChunkLength++;
	    if (*eol == '\n') {
		break;
	    }
	}

	/*
	 * Create a new line consisting of up to three parts: a prefix
	 * from linePtr, some material from string, and a suffix from
	 * linePtr.
	 */

	if ((newChunkLength == 0) || (*eol != '\n')) {
	    suffixLength = linePtr->numBytes - ch;
	} else {
	    suffixLength = 0;
	}
	totalLength = prefixLength + newChunkLength + suffixLength;
	newPtr = (TkTextLine *) ckalloc(LINE_SIZE(totalLength));
	newPtr->parentPtr = nodePtr;
	if (prevPtr == NULL) {
	    newPtr->nextPtr = nodePtr->children.linePtr;
	    nodePtr->children.linePtr = newPtr;
	} else {
	    newPtr->nextPtr = prevPtr->nextPtr;
	    prevPtr->nextPtr = newPtr;
	}
	if (linePtr->annotPtr != NULL) {
	    newPtr->annotPtr = linePtr->annotPtr;
	    for (annotPtr = newPtr->annotPtr; annotPtr != NULL;
		    annotPtr = annotPtr->nextPtr) {
		annotPtr->linePtr = newPtr;
	    }
	    linePtr->annotPtr = NULL;
	} else {
	    newPtr->annotPtr = NULL;
	}
	newPtr->numBytes = totalLength;
	if (prefixLength != 0) {
	    memcpy((VOID *) newPtr->bytes, (VOID *) linePtr->bytes,
		    prefixLength);
	}
	if (newChunkLength != 0) {
	    memcpy((VOID *) (newPtr->bytes + prefixLength), (VOID *) string,
		    newChunkLength);
	}
	if (suffixLength != 0) {
	    memcpy((VOID *) (newPtr->bytes + prefixLength + newChunkLength),
		    (VOID *) (linePtr->bytes + ch), suffixLength);
	}
	newPtr->bytes[totalLength] = 0;
	changeToLineCount += 1;

	/*
	 * Quit after the suffix has been output (there is always at least
	 * one character of suffix: the newline).  Before jumping out of the
	 * loop, put back the annotations that pertain to the suffix.
	 * Careful!  If no newlines were inserted, there could already be
	 * annotations at the beginning of the line;  add back to the end.
	 */

	if (suffixLength != 0) {
	    if (newPtr->annotPtr == NULL) {
		newPtr->annotPtr = afterPtr;
	    } else {
		for (annotPtr = newPtr->annotPtr; annotPtr->nextPtr != NULL;
			annotPtr = annotPtr->nextPtr) {
		    /* Empty loop body. */
		}
		annotPtr->nextPtr = afterPtr;
	    }
	    for (annotPtr = afterPtr; annotPtr != NULL;
		    annotPtr = annotPtr->nextPtr) {
		annotPtr->linePtr = newPtr;
		annotPtr->ch += prefixLength+newChunkLength-ch;
	    }
	    break;
	}

	/*
	 * Advance to insert the next line chunk.
	 */

	string += newChunkLength;
	prefixLength = 0;
	prevPtr = newPtr;
    }

    /*
     * Increment the line counts in all the parent nodes of the insertion
     * point, then rebalance the tree if necessary.
     */

    for ( ; nodePtr != NULL; nodePtr = nodePtr->parentPtr) {
	nodePtr->numLines += changeToLineCount;
    }
    nodePtr = linePtr->parentPtr;
    nodePtr->numChildren += changeToLineCount;
    if (nodePtr->numChildren > MAX_CHILDREN) {
	Rebalance(treePtr, nodePtr);
    }

    ckfree((char *) linePtr);
    if (tkBTreeDebug) {
	TkBTreeCheck(tree);
    }
}

/*
 *----------------------------------------------------------------------
 *
 * TkBTreeDeleteChars --
 *
 *	Delete a range of characters from a B-tree.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Information is deleted from the B-tree.  This can cause the
 *	internal structure of the B-tree to change.  Note: the two
 *	lines given by line1Ptr and line2Ptr will be replaced with
 *	a single line containing the undeleted parts of the original
 *	lines.  This could potentially result in an empty line;
 *	normally the caller should adjust the deletion range to prevent
 *	this sort of behavior.
 *
 *----------------------------------------------------------------------
 */

void
TkBTreeDeleteChars(tree, line1Ptr, ch1, line2Ptr, ch2)
    TkTextBTree tree;			/* B-tree in which to delete. */
    register TkTextLine *line1Ptr;	/* Line containing first character
					 * to delete. */
    int ch1;				/* Index within linePtr1 of first
					 * character to delete. */
    register TkTextLine *line2Ptr;	/* Line containing character just
					 * after last one to delete. */
    int ch2;				/* Index within linePtr2 of character
					 * just after last one to delete. */
{
    BTree *treePtr = (BTree *) tree;
    TkTextLine *linePtr, *nextPtr, *prevLinePtr;
    Node *nodePtr, *parentPtr, *nextNodePtr;
    TkAnnotation *annotPtr, *annotPtr2;
    int ch;
    int linesDeleted;			/* Counts lines deleted from current
					 * level-0 node. */

    /*
     * Work through the tree deleting all of the lines between line1Ptr
     * and line2Ptr (but don't delete line1Ptr or line2Ptr yet).  Also
     * delete any nodes in the B-tree that become empty because of
     * this process.
     */

    linePtr = line1Ptr->nextPtr;
    nodePtr = line1Ptr->parentPtr;
    if (line1Ptr == line2Ptr) {
	goto middleLinesDeleted;
    }
    while (1) {

	/*
	 * Delete all relevant lines within the same level-0 node.
	 */

	linesDeleted = 0;
	while ((linePtr != line2Ptr) && (linePtr != NULL)) {
	    /*
	     * Move any annotations in this line to the end of the
	     * deletion range.  If both the starting and ending toggle
	     * for a tagged range get moved, they'll cancel each other
	     * automatically and be dropped, which is the right behavior.
	     */

	    for (annotPtr = linePtr->annotPtr; annotPtr != NULL;
		    annotPtr = annotPtr2) {
		if (annotPtr->type == TK_ANNOT_TOGGLE) {
		    AddToggleToLine(line2Ptr, ch2, annotPtr->info.tagPtr);
		    ChangeNodeToggleCount(nodePtr, annotPtr->info.tagPtr, -1);
		    annotPtr2 = annotPtr->nextPtr;
		    ckfree((char *) annotPtr);
		} else {
		    annotPtr2 = annotPtr->nextPtr;
		    TkBTreeRemoveAnnotation(annotPtr);
		    annotPtr->linePtr = line2Ptr;
		    annotPtr->ch = ch2;
		    TkBTreeAddAnnotation(annotPtr);
		}
	    }
	    nextPtr = linePtr->nextPtr;
	    ckfree((char *) linePtr);
	    linesDeleted++;
	    linePtr = nextPtr;
	}
	if (nodePtr == line1Ptr->parentPtr) {
	    line1Ptr->nextPtr = linePtr;
	} else {
	    nodePtr->children.linePtr = linePtr;
	}
	for (parentPtr = nodePtr; parentPtr != NULL;
		parentPtr = parentPtr->parentPtr) {
	    parentPtr->numLines -= linesDeleted;
	}
	nodePtr->numChildren -= linesDeleted;
	if (linePtr == line2Ptr) {
	    break;
	}

	/*
	 * Find the next level-0 node to visit, and its first line (but
	 * remember the current node so we can come back to delete it if
	 * it's empty).
	 */

	nextNodePtr = nodePtr;
	while (nextNodePtr->nextPtr == NULL) {
	    nextNodePtr = nextNodePtr->parentPtr;
	}
	nextNodePtr = nextNodePtr->nextPtr;
	while (nextNodePtr->level > 0) {
	    nextNodePtr = nextNodePtr->children.nodePtr;
	}
	linePtr = nextNodePtr->children.linePtr;

	/*
	 * Now go back to the node we just left and delete it if
	 * it's empty, along with any of its ancestors that are
	 * empty.  It may seem funny to go back like this, but it's
	 * simpler to find the next place to visit before modifying
	 * the tree structure.
	 */

	while (nodePtr->numChildren == 0) {
	    parentPtr = nodePtr->parentPtr;
	    if (parentPtr->children.nodePtr == nodePtr) {
		parentPtr->children.nodePtr = nodePtr->nextPtr;
	    } else {
		Node *prevPtr;

		for (prevPtr = parentPtr->children.nodePtr;
			prevPtr->nextPtr != nodePtr;
			prevPtr = prevPtr->nextPtr) {
		}
		prevPtr->nextPtr = nodePtr->nextPtr;
	    }
	    parentPtr->numChildren--;
	    DeleteSummaries(nodePtr->summaryPtr);
	    ckfree((char *) nodePtr);
	    nodePtr = parentPtr;
	}
	nodePtr = nextNodePtr;
    }

    /*
     * Make a new line that consists of the first part of the first
     * line of the deletion range and the last part of the last line
     * of the deletion range.
     */

    middleLinesDeleted:
    nodePtr = line1Ptr->parentPtr;
    linePtr = (TkTextLine *) ckalloc(LINE_SIZE(ch1 + line2Ptr->numBytes - ch2));
    linePtr->parentPtr = nodePtr;
    linePtr->nextPtr = line1Ptr->nextPtr;
    linePtr->annotPtr = NULL;
    linePtr->numBytes = ch1 + line2Ptr->numBytes - ch2;
    if (ch1 != 0) {
	memcpy((VOID *) linePtr->bytes, (VOID *) line1Ptr->bytes, ch1);
    }
    strcpy(linePtr->bytes + ch1, line2Ptr->bytes + ch2);

    /*
     * Process the annotations for the starting and ending lines.  Enter
     * a new annotation on linePtr (the joined line) for each of these
     * annotations, then delete the originals.  The code below is a little
     * tricky (e.g. the "break" in the first loop) to handle the case where
     * the starting and ending lines are the same.
     */

    for (annotPtr = line1Ptr->annotPtr; annotPtr != NULL;
	    annotPtr = line1Ptr->annotPtr) {
	if (annotPtr->ch <= ch1) {
	    ch = annotPtr->ch;
	} else {
	    if (line1Ptr == line2Ptr) {
		break;
	    }
	    ch = ch1;
	}
	line1Ptr->annotPtr = annotPtr->nextPtr;
	if (annotPtr->type == TK_ANNOT_TOGGLE) {
	    AddToggleToLine(linePtr, ch, annotPtr->info.tagPtr);
	    ChangeNodeToggleCount(line1Ptr->parentPtr, annotPtr->info.tagPtr,
		    -1);
	    ckfree((char *) annotPtr);
	} else {
	    annotPtr->linePtr = linePtr;
	    annotPtr->ch = ch;
	    TkBTreeAddAnnotation(annotPtr);
	}
    }
    for (annotPtr = line2Ptr->annotPtr; annotPtr != NULL;
	    annotPtr = line2Ptr->annotPtr) {
	if (annotPtr->ch >= ch2) {
	    ch = annotPtr->ch - ch2 + ch1;
	} else {
	    ch = ch1;
	}
	line2Ptr->annotPtr = annotPtr->nextPtr;
	if (annotPtr->type == TK_ANNOT_TOGGLE) {
	    AddToggleToLine(linePtr, ch, annotPtr->info.tagPtr);
	    ChangeNodeToggleCount(line2Ptr->parentPtr, annotPtr->info.tagPtr,
		    -1);
	    ckfree((char *) annotPtr);
	} else {
	    annotPtr->linePtr = linePtr;
	    annotPtr->ch = ch;
	    TkBTreeAddAnnotation(annotPtr);
	}
    }

    /*
     * Delete the original starting and stopping lines (don't forget
     * that the annotations have already been deleted) and insert the
     * new line in place of line1Ptr.
     */

    nodePtr = line1Ptr->parentPtr;
    if (nodePtr->children.linePtr == line1Ptr) {
	nodePtr->children.linePtr = linePtr;
    } else {
	for (prevLinePtr = nodePtr->children.linePtr;
		prevLinePtr->nextPtr != line1Ptr;
		prevLinePtr = prevLinePtr->nextPtr) {
	    /* Empty loop body. */
	}
	prevLinePtr->nextPtr = linePtr;
    }
    ckfree((char *) line1Ptr);
    nodePtr = line2Ptr->parentPtr;
    if (line2Ptr != line1Ptr) {
	if (nodePtr->children.linePtr == line2Ptr) {
	    nodePtr->children.linePtr = line2Ptr->nextPtr;
	} else {
	    for (prevLinePtr = nodePtr->children.linePtr;
		    prevLinePtr->nextPtr != line2Ptr;
		    prevLinePtr = prevLinePtr->nextPtr) {
		/* Empty loop body. */
	    }
	    prevLinePtr->nextPtr = line2Ptr->nextPtr;
	}
	ckfree((char *) line2Ptr);
	for (parentPtr = nodePtr; parentPtr != NULL;
		parentPtr = parentPtr->parentPtr) {
	    parentPtr->numLines--;
	}
	nodePtr->numChildren--;
    }

    /*
     * Rebalance the tree, starting from each of the endpoints of the
     * deletion range.  This code is a tricky, because the act of
     * rebalancing the parent of one endpoint can cause the parent of
     * the other endpoint to be reallocated.  The only thing it's safe
     * to hold onto is a pointer to a line.  Thus, rebalance line2Ptr's
     * parent first, then use linePtr find the second parent to rebalance
     * second.  
     */

    if (nodePtr != linePtr->parentPtr) {
	Rebalance(treePtr, nodePtr);
    }
    Rebalance(treePtr, linePtr->parentPtr);
    if (tkBTreeDebug) {
	TkBTreeCheck(tree);
    }
}

/*
 *----------------------------------------------------------------------
 *
 * TkBTreeTag --
 *
 *	Turn a given tag on or off for a given range of characters in
 *	a B-tree of text.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The given tag is added to the given range of characters
 *	in the tree or removed from all those characters, depending
 *	on the "add" argument.
 *
 *----------------------------------------------------------------------
 */

void
TkBTreeTag(tree, line1, ch1, line2, ch2, tagPtr, add)
    TkTextBTree tree;			/* B-tree in which to add tag
					 * information. */
    int line1, ch1;			/* Position of first character to
					 * tag. */
    int line2, ch2;			/* Position of character just after
					 * last one to tag. */
    TkTextTag *tagPtr;			/* Tag to associate with the range
					 * of characters. */
    int add;				/* One means add tag to the given
					 * range of characters;  zero means
					 * remove the tag from the range. */
{
    BTree *treePtr = (BTree *) tree;
    register TkTextLine *line1Ptr, *line2Ptr;
    TkTextSearch search;
    int oldState;

    /*
     * Find the lines containing the first and last characters to be tagged,
     * and adjust the starting and stopping locations if they don't already
     * point within lines.  If the range would have started or stopped at the
     * end of a line, round it up to the beginning of the next line (right
     * now this restriction keeps the final newline from being tagged).
     */

    if (line1 < 0) {
	line1 = 0;
	ch1 = 0;
    }
    line1Ptr = TkBTreeFindLine(tree, line1);
    if (line1Ptr == NULL) {
	return;
    }
    if (ch1 >= line1Ptr->numBytes) {
	TkTextLine *nextLinePtr;

	nextLinePtr = TkBTreeNextLine(line1Ptr);
	if (nextLinePtr == NULL) {
	    return;
	} else {
	    line1Ptr = nextLinePtr;
	    line1++;
	    ch1 = 0;
	}
    }
    if (line2 < 0) {
	return;
    }
    line2Ptr = TkBTreeFindLine(tree, line2);
    if (line2Ptr == NULL) {
	line2Ptr = TkBTreeFindLine(tree, treePtr->rootPtr->numLines-1);
	ch2 = line2Ptr->numBytes-1;
    }
    if (ch2 >= line2Ptr->numBytes) {
	TkTextLine *nextLinePtr;

	nextLinePtr = TkBTreeNextLine(line2Ptr);
	if (nextLinePtr == NULL) {
	    ch2 = line2Ptr->numBytes-1;
	} else {
	    line2Ptr = nextLinePtr;
	    line2++;
	    ch2 = 0;
	}
    }

    /*
     * See if the tag is already present or absent at the start of the
     * range.  If the state doesn't already match what we want then add
     * a toggle there.
     */

    oldState = TkBTreeCharTagged(line1Ptr, ch1, tagPtr);
    if ((add != 0) ^ oldState) {
	AddToggleToLine(line1Ptr, ch1, tagPtr);
    }

    /*
     * Scan the range of characters covered by the change and delete
     * any existing tag transitions except those on the first and
     * last characters.  Keep track of whether the old state just before
     * the last character (not including any tags on it) is what we
     * want now;  if not, then add a tag toggle there.
     */

    TkBTreeStartSearch(tree, line1, ch1+1, line2, ch2, tagPtr, &search);
    while (TkBTreeNextTag(&search)) {
	if ((search.linePtr == line2Ptr) && (search.ch1 == ch2)) {
	    break;
	}
	oldState ^= 1;
	AddToggleToLine(search.linePtr, search.ch1, tagPtr);
    }
    if ((add != 0) ^ oldState) {
	AddToggleToLine(line2Ptr, ch2, tagPtr);
    }

    if (tkBTreeDebug) {
	TkBTreeCheck(tree);
    }
}

/*
 *----------------------------------------------------------------------
 *
 * TkBTreeAddAnnotation --
 *
 *	Given a filled in annotation, this procedure links it into
 *	a B-tree structure so that it will track changes to the B-tree.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	AnnotPtr will be linked into its tree.  Note:  the storage for
 *	annotPtr is assumed to have been malloc'ed by the caller.
 *
 *----------------------------------------------------------------------
 */

	/* ARGSUSED */
void
TkBTreeAddAnnotation(annotPtr)
    TkAnnotation *annotPtr;	/* Pointer to annotation.  The caller must
				 * have filled in all the fields except the
				 * "nextPtr" field.  The type should NOT be
				 * TK_ANNOT_TOGGLE;  these annotations are
				 * managed by the TkBTreeTag procedure. */
{
    register TkAnnotation *annotPtr2, *prevPtr;

    for (prevPtr = NULL, annotPtr2 = annotPtr->linePtr->annotPtr;
	    annotPtr2 != NULL;
	    prevPtr = annotPtr2, annotPtr2 = annotPtr2->nextPtr) {
	if (annotPtr2->ch > annotPtr->ch) {
	    break;
	}
    }
    if (prevPtr == NULL) {
	annotPtr->nextPtr = annotPtr->linePtr->annotPtr;
	annotPtr->linePtr->annotPtr = annotPtr;
    } else {
	annotPtr->nextPtr = prevPtr->nextPtr;
	prevPtr->nextPtr = annotPtr;
    }
}

/*
 *----------------------------------------------------------------------
 *
 * TkBTreeRemoveAnnotation --
 *
 *	This procedure unlinks an annotation from a B-tree so that
 *	the annotation will no longer be managed by the B-tree code.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	AnnotPtr will be unlinked from its tree.  Note:  it is up to the
 *	caller to free the storage for annotPtr, if that is desired.
 *
 *----------------------------------------------------------------------
 */

	/* ARGSUSED */
void
TkBTreeRemoveAnnotation(annotPtr)
    TkAnnotation *annotPtr;	/* Pointer to annotation, which must
				 * have been linked into tree by a previous
				 * call to TkBTreeAddAnnotation. */
{
    register TkAnnotation *prevPtr;

    if (annotPtr->linePtr->annotPtr == annotPtr) {
	annotPtr->linePtr->annotPtr = annotPtr->nextPtr;
    } else {
	for (prevPtr = annotPtr->linePtr->annotPtr;
/* BUG: fixed by dhopkins, prevPtr was null!
		prevPtr->nextPtr != annotPtr;
*/
		(prevPtr != NULL) && (prevPtr->nextPtr != annotPtr);
		prevPtr = prevPtr->nextPtr) {
	    /* Empty loop body. */
	}
	if (prevPtr != NULL) { /* Bullet proofing by dhopkins */
	    prevPtr->nextPtr = annotPtr->nextPtr;
	}
    }
}

/*
 *----------------------------------------------------------------------
 *
 * TkBTreeFindLine --
 *
 *	Find a particular line in a B-tree based on its line number.
 *
 * Results:
 *	The return value is a pointer to the line structure for the
 *	line whose index is "line", or NULL if no such line exists.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

TkTextLine *
TkBTreeFindLine(tree, line)
    TkTextBTree tree;			/* B-tree in which to find line. */
    int line;				/* Index of desired line. */
{
    BTree *treePtr = (BTree *) tree;
    register Node *nodePtr;
    register TkTextLine *linePtr;
    int linesLeft;

    nodePtr = treePtr->rootPtr;
    linesLeft = line;
    if ((line < 0) || (line >= nodePtr->numLines)) {
	return NULL;
    }

    /*
     * Work down through levels of the tree until a node is found at
     * level 0.
     */

    while (nodePtr->level != 0) {
	for (nodePtr = nodePtr->children.nodePtr;
		nodePtr->numLines <= linesLeft;
		nodePtr = nodePtr->nextPtr) {
	    if (nodePtr == NULL) {
		panic("TkBTreeFindLine ran out of nodes");
	    }
	    linesLeft -= nodePtr->numLines;
	}
    }

    /*
     * Work through the lines attached to the level-0 node.
     */

    for (linePtr = nodePtr->children.linePtr; linesLeft > 0;
	    linePtr = linePtr->nextPtr) {
	if (linePtr == NULL) {
	    panic("TkBTreeFindLine ran out of lines");
	}
	linesLeft -= 1;
    }
    return linePtr;
}

/*
 *----------------------------------------------------------------------
 *
 * TkBTreeNextLine --
 *
 *	Given an existing line in a B-tree, this procedure locates the
 *	next line in the B-tree.  This procedure is used for scanning
 *	through the B-tree.
 *
 * Results:
 *	The return value is a pointer to the line that immediately
 *	follows linePtr, or NULL if there is no such line.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

TkTextLine *
TkBTreeNextLine(linePtr)
    register TkTextLine *linePtr;	/* Pointer to existing line in
					 * B-tree. */
{
    register Node *nodePtr;

    if (linePtr->nextPtr != NULL) {
	return linePtr->nextPtr;
    }

    /*
     * This was the last line associated with the particular parent node.
     * Search up the tree for the next node, then search down from that
     * node to find the first line,
     */

    for (nodePtr = linePtr->parentPtr; ; nodePtr = nodePtr->parentPtr) {
	if (nodePtr->nextPtr != NULL) {
	    nodePtr = nodePtr->nextPtr;
	    break;
	}
	if (nodePtr->parentPtr == NULL) {
	    return (TkTextLine *) NULL;
	}
    }
    while (nodePtr->level > 0) {
	nodePtr = nodePtr->children.nodePtr;
    }
    return nodePtr->children.linePtr;
}

/*
 *----------------------------------------------------------------------
 *
 * TkBTreeLineIndex --
 *
 *	Given a pointer to a line in a B-tree, return the numerical
 *	index of that line.
 *
 * Results:
 *	The result is the index of linePtr within the tree, where 0
 *	corresponds to the first line in the tree.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

int
TkBTreeLineIndex(linePtr)
    TkTextLine *linePtr;		/* Pointer to existing line in
					 * B-tree. */
{
    register TkTextLine *linePtr2;
    register Node *nodePtr, *parentPtr, *nodePtr2;
    int index;

    /*
     * First count how many lines precede this one in its level-0
     * node.
     */

    nodePtr = linePtr->parentPtr;
    index = 0;
    for (linePtr2 = nodePtr->children.linePtr; linePtr2 != linePtr;
	    linePtr2 = linePtr2->nextPtr) {
	if (linePtr2 == NULL) {
	    panic("TkBTreeLineIndex couldn't find line");
	}
	index += 1;
    }

    /*
     * Now work up through the levels of the tree one at a time,
     * counting how many lines are in nodes preceding the current
     * node.
     */

    for (parentPtr = nodePtr->parentPtr ; parentPtr != NULL;
	    nodePtr = parentPtr, parentPtr = parentPtr->parentPtr) {
	for (nodePtr2 = parentPtr->children.nodePtr; nodePtr2 != nodePtr;
		nodePtr2 = nodePtr2->nextPtr) {
	    if (nodePtr2 == NULL) {
		panic("TkBTreeLineIndex couldn't find node");
	    }
	    index += nodePtr2->numLines;
	}
    }
    return index;
}

/*
 *----------------------------------------------------------------------
 *
 * TkBTreeStartSearch --
 *
 *	This procedure sets up a search for tag transitions involving
 *	a given tag (or all tags) in a given range of the text.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The information at *searchPtr is set up so that subsequent calls
 *	to TkBTreeNextTag will return information about the locations of
 *	tag transitions.  Note that TkBTreeNextTag must be called to get
 *	the first transition.
 *
 *----------------------------------------------------------------------
 */

void
TkBTreeStartSearch(tree, line1, ch1, line2, ch2, tagPtr, searchPtr)
    TkTextBTree tree;			/* Tree to search. */
    int line1, ch1;			/* Character position at which to						 * start search (tags at this position
					 * will be returned). */
    int line2, ch2;			/* Character position at which to						 * stop search (tags at this position
					 * will be returned). */
    TkTextTag *tagPtr;			/* Tag to search for.  NULL means
					 * search for any tag. */
    register TkTextSearch *searchPtr;	/* Where to store information about
					 * search's progress. */
{
    register TkAnnotation *annotPtr;

    searchPtr->tree = tree;
    if (line1 < 0) {
	searchPtr->line1 = 0;
	searchPtr->ch1 = 0;
    } else {
	searchPtr->line1 = line1;
	searchPtr->ch1 = ch1;
    }
    searchPtr->line2 = line2;
    searchPtr->ch2 = ch2;
    searchPtr->tagPtr = tagPtr;
    searchPtr->allTags = (tagPtr == NULL);

    searchPtr->linePtr = TkBTreeFindLine(searchPtr->tree, searchPtr->line1);
    if (searchPtr->linePtr == NULL) {
	searchPtr->line1 = searchPtr->line2;
	searchPtr->ch1 = searchPtr->ch2;
	searchPtr->annotPtr = NULL;
    } else {
	for (annotPtr = searchPtr->linePtr->annotPtr;
		(annotPtr != NULL) && (annotPtr->ch < ch1);
		annotPtr = annotPtr->nextPtr) {
	    /* Empty loop body. */
	}
	searchPtr->annotPtr = annotPtr;
    }
}

/*
 *----------------------------------------------------------------------
 *
 * TkBTreeNextTag --
 *
 *	Once a tag search has begun, successive calls to this procedure
 *	return successive tag toggles.  Note:  it is NOT SAFE to call this
 *	procedure if characters have been inserted into or deleted from
 *	the B-tree since the call to TkBTreeStartSearch.
 *
 * Results:
 *	The return value is 1 if another toggle was found that met the
 *	criteria specified in the call to TkBTreeStartSearch.  0 is
 *	returned if no more matching tag transitions were found.
 *
 * Side effects:
 *	Information in *searchPtr is modified to update the state of the
 *	search and indicate where the next tag toggle is located.
 *
 *----------------------------------------------------------------------
 */

int
TkBTreeNextTag(searchPtr)
    register TkTextSearch *searchPtr;	/* Information about search in
					 * progress;  must have been set up by
					 * call to TkBTreeStartSearch. */
{
    register TkAnnotation *annotPtr;
    register Node *nodePtr;
    register Summary *summaryPtr;

    if (searchPtr->linePtr == NULL) {
	return 0;
    }

    /*
     * The outermost loop iterates over lines that may potentially contain
     * a relevant tag transition, starting from the current line and tag.
     */

    while (1) {
	/*
	 * See if there are more tags on the current line that are relevant.
	 */
    
	for (annotPtr = searchPtr->annotPtr; annotPtr != NULL;
		annotPtr = annotPtr->nextPtr) {
	    if ((annotPtr->type == TK_ANNOT_TOGGLE)
		    && (searchPtr->allTags
		    || (annotPtr->info.tagPtr == searchPtr->tagPtr))) {
		if ((searchPtr->line1 == searchPtr->line2)
			&& (annotPtr->ch > searchPtr->ch2)) {
		    goto searchOver;
		}
		searchPtr->tagPtr = annotPtr->info.tagPtr;
		searchPtr->ch1 = annotPtr->ch;
		searchPtr->annotPtr = annotPtr->nextPtr;
		return 1;
	    }
	}
    
	/*
	 * See if there are more lines associated with the current parent
	 * node.  If so, go back to the top of the loop to search the next
	 * one of them.
	 */
    
	if (searchPtr->line1 >= searchPtr->line2) {
	    goto searchOver;
	}
	searchPtr->line1++;
	if (searchPtr->linePtr->nextPtr != NULL) {
	    searchPtr->linePtr = searchPtr->linePtr->nextPtr;
	    searchPtr->annotPtr = searchPtr->linePtr->annotPtr;
	    continue;
	}
    
	/*
	 * Search across and up through the B-tree's node hierarchy looking
	 * for the next node that has a relevant tag transition somewhere in
	 * its subtree.  Be sure to update the current line number as we
	 * skip over large chunks of lines.
	 */
    
	nodePtr = searchPtr->linePtr->parentPtr;
	while (1) {
	    while (nodePtr->nextPtr == NULL) {
		if (nodePtr->parentPtr == NULL) {
		    goto searchOver;
		}
		nodePtr = nodePtr->parentPtr;
	    }
	    nodePtr = nodePtr->nextPtr;
	    for (summaryPtr = nodePtr->summaryPtr; summaryPtr != NULL;
		    summaryPtr = summaryPtr->nextPtr) {
		if ((searchPtr->allTags) ||
			(summaryPtr->tagPtr == searchPtr->tagPtr)) {
		    goto gotNodeWithTag;
		}
	    }
	    searchPtr->line1 += nodePtr->numLines;
	}
    
	/*
	 * At this point we've found a subtree that has a relevant tag
	 * transition.  Now search down (and across) through that subtree
	 * to find the first level-0 node that has a relevant tag transition.
	 */
    
	gotNodeWithTag:
	while (nodePtr->level > 0) {
	    for (nodePtr = nodePtr->children.nodePtr; ;
		    nodePtr = nodePtr->nextPtr) {
		for (summaryPtr = nodePtr->summaryPtr; summaryPtr != NULL;
			summaryPtr = summaryPtr->nextPtr) {
		    if ((searchPtr->allTags)
			    || (summaryPtr->tagPtr == searchPtr->tagPtr)) {
			goto nextChild;
		    }
		}
		searchPtr->line1 += nodePtr->numLines;
		if (nodePtr->nextPtr == NULL) {
		    panic("TkBTreeNextTag found incorrect tag summary info.");
		}
	    }
	    nextChild:
	    continue;
	}
    
	/*
	 * Now we're down to a level-0 node that contains a line that contains
	 * a relevant tag transition.  Set up line information and go back to
	 * the beginning of the loop to search through lines.
	 */

	searchPtr->linePtr = nodePtr->children.linePtr;
	searchPtr->annotPtr = searchPtr->linePtr->annotPtr;
	if (searchPtr->line1 > searchPtr->line2) {
	    goto searchOver;
	}
	continue;
    }

    searchOver:
    searchPtr->line1 = searchPtr->line2;
    searchPtr->ch1 = searchPtr->ch2;
    searchPtr->annotPtr = NULL;
    searchPtr->linePtr = NULL;
    return 0;
}

/*
 *----------------------------------------------------------------------
 *
 * TkBTreeCheck --
 *
 *	This procedure runs a set of consistency checks over a B-tree
 *	and panics if any inconsistencies are found.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	If a structural defect is found, the procedure panics with an
 *	error message.
 *
 *----------------------------------------------------------------------
 */

void
TkBTreeCheck(tree)
    TkTextBTree tree;		/* Tree to check. */
{
    BTree *treePtr = (BTree *) tree;
    register Summary *summaryPtr;

    /*
     * Make sure that overall there is an even count of tag transitions
     * for the whole text.
     */

    for (summaryPtr = treePtr->rootPtr->summaryPtr; summaryPtr != NULL;
	    summaryPtr = summaryPtr->nextPtr) {
	if (summaryPtr->toggleCount & 1) {
	    panic("TkBTreeCheck found odd toggle count for \"%s\" (%d)",
		    summaryPtr->tagPtr->name, summaryPtr->toggleCount);
	}
    }

    /*
     * Call a recursive procedure to do all of the rest of the checks.
     */

    CheckNodeConsistency(treePtr->rootPtr);
}

/*
 *----------------------------------------------------------------------
 *
 * Rebalance --
 *
 *	This procedure is called when a node of a B-tree appears to be
 *	out of balance (too many children, or too few).  It rebalances
 *	that node and all of its ancestors in the tree.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The internal structure of treePtr may change.
 *
 *----------------------------------------------------------------------
 */

static void
Rebalance(treePtr, nodePtr)
    BTree *treePtr;			/* Tree that is being rebalanced. */
    register Node *nodePtr;		/* Node that may be out of balance. */
{
    /*
     * Loop over the entire ancestral chain of the node, working up
     * through the tree one node at a time until the root node has
     * been processed.
     */

    for ( ; nodePtr != NULL; nodePtr = nodePtr->parentPtr) {
	register Node *newPtr, *childPtr;
	register TkTextLine *linePtr;
	int i;

	/*
	 * Check to see if the node has too many children.  If it does,
	 * then split off all but the first MIN_CHILDREN into a separate
	 * node following the original one.  Then repeat until the
	 * node has a decent size.
	 */

	if (nodePtr->numChildren > MAX_CHILDREN) {
	    while (1) {
		/*
		 * If the node being split is the root node, then make a
		 * new root node above it first.
		 */
    
		if (nodePtr->parentPtr == NULL) {
		    newPtr = (Node *) ckalloc(sizeof(Node));
		    newPtr->parentPtr = NULL;
		    newPtr->nextPtr = NULL;
		    newPtr->summaryPtr = NULL;
		    newPtr->level = nodePtr->level + 1;
		    newPtr->children.nodePtr = nodePtr;
		    newPtr->numChildren = 1;
		    newPtr->numLines = nodePtr->numLines;
		    RecomputeNodeCounts(newPtr);
		    treePtr->rootPtr = newPtr;
		}
		newPtr = (Node *) ckalloc(sizeof(Node));
		newPtr->parentPtr = nodePtr->parentPtr;
		newPtr->nextPtr = nodePtr->nextPtr;
		nodePtr->nextPtr = newPtr;
		newPtr->summaryPtr = NULL;
		newPtr->level = nodePtr->level;
		newPtr->numChildren = nodePtr->numChildren - MIN_CHILDREN;
		if (nodePtr->level == 0) {
		    for (i = MIN_CHILDREN-1,
			    linePtr = nodePtr->children.linePtr;
			    i > 0; i--, linePtr = linePtr->nextPtr) {
			/* Empty loop body. */
		    }
		    newPtr->children.linePtr = linePtr->nextPtr;
		    linePtr->nextPtr = NULL;
		} else {
		    for (i = MIN_CHILDREN-1,
			    childPtr = nodePtr->children.nodePtr;
			    i > 0; i--, childPtr = childPtr->nextPtr) {
			/* Empty loop body. */
		    }
		    newPtr->children.nodePtr = childPtr->nextPtr;
		    childPtr->nextPtr = NULL;
		}
		RecomputeNodeCounts(nodePtr);
		nodePtr->parentPtr->numChildren++;
		nodePtr = newPtr;
		if (nodePtr->numChildren <= MAX_CHILDREN) {
		    RecomputeNodeCounts(nodePtr);
		    break;
		}
	    }
	}

	while (nodePtr->numChildren < MIN_CHILDREN) {
	    register Node *otherPtr;
	    Node *halfwayNodePtr = NULL;	/* Initialization needed only */
	    TkTextLine *halfwayLinePtr = NULL;	/* to prevent cc warnings. */
	    int totalChildren, firstChildren, i;

	    /*
	     * Too few children for this node.  If this is the root,
	     * it's OK for it to have less than MIN_CHILDREN children
	     * as long as it's got at least two.  If it has only one
	     * (and isn't at level 0), then chop the root node out of
	     * the tree and use its child as the new root.
	     */

	    if (nodePtr->parentPtr == NULL) {
		if ((nodePtr->numChildren == 1) && (nodePtr->level > 0)) {
		    treePtr->rootPtr = nodePtr->children.nodePtr;
		    treePtr->rootPtr->parentPtr = NULL;
		    DeleteSummaries(nodePtr->summaryPtr);
		    ckfree((char *) nodePtr);
		}
		return;
	    }

	    /*
	     * Not the root.  Make sure that there are siblings to
	     * balance with.
	     */

	    if (nodePtr->parentPtr->numChildren < 2) {
		Rebalance(treePtr, nodePtr->parentPtr);
		continue;
	    }

	    /*
	     * Find a sibling to borrow from, and arrange for nodePtr to
	     * be the earlier of the pair.
	     */

	    if (nodePtr->nextPtr == NULL) {
		for (otherPtr = nodePtr->parentPtr->children.nodePtr;
			otherPtr->nextPtr != nodePtr;
			otherPtr = otherPtr->nextPtr) {
		    /* Empty loop body. */
		}
		nodePtr = otherPtr;
	    }
	    otherPtr = nodePtr->nextPtr;

	    /*
	     * We're going to either merge the two siblings together
	     * into one node or redivide the children among them to
	     * balance their loads.  As preparation, join their two
	     * child lists into a single list and remember the half-way
	     * point in the list.
	     */

	    totalChildren = nodePtr->numChildren + otherPtr->numChildren;
	    firstChildren = totalChildren/2;
	    if (nodePtr->children.nodePtr == NULL) {
		nodePtr->children = otherPtr->children;
	    } else if (nodePtr->level == 0) {
		register TkTextLine *linePtr;

		for (linePtr = nodePtr->children.linePtr, i = 1;
			linePtr->nextPtr != NULL;
			linePtr = linePtr->nextPtr, i++) {
		    if (i == firstChildren) {
			halfwayLinePtr = linePtr;
		    }
		}
		linePtr->nextPtr = otherPtr->children.linePtr;
		while (i <= firstChildren) {
		    halfwayLinePtr = linePtr;
		    linePtr = linePtr->nextPtr;
		    i++;
		}
	    } else {
		register Node *childPtr;

		for (childPtr = nodePtr->children.nodePtr, i = 1;
			childPtr->nextPtr != NULL;
			childPtr = childPtr->nextPtr, i++) {
		    if (i <= firstChildren) {
			if (i == firstChildren) {
			    halfwayNodePtr = childPtr;
			}
		    }
		}
		childPtr->nextPtr = otherPtr->children.nodePtr;
		while (i <= firstChildren) {
		    halfwayNodePtr = childPtr;
		    childPtr = childPtr->nextPtr;
		    i++;
		}
	    }

	    /*
	     * If the two siblings can simply be merged together, do it.
	     */

	    if (totalChildren < MAX_CHILDREN) {
		RecomputeNodeCounts(nodePtr);
		nodePtr->nextPtr = otherPtr->nextPtr;
		nodePtr->parentPtr->numChildren--;
		DeleteSummaries(otherPtr->summaryPtr);
		ckfree((char *) otherPtr);
		continue;
	    }

	    /*
	     * The siblings can't be merged, so just divide their
	     * children evenly between them.
	     */

	    if (nodePtr->level == 0) {
		otherPtr->children.linePtr = halfwayLinePtr->nextPtr;
		halfwayLinePtr->nextPtr = NULL;
	    } else {
		otherPtr->children.nodePtr = halfwayNodePtr->nextPtr;
		halfwayNodePtr->nextPtr = NULL;
	    }
	    RecomputeNodeCounts(nodePtr);
	    RecomputeNodeCounts(otherPtr);
	}
    }
}

/*
 *----------------------------------------------------------------------
 *
 * RecomputeNodeCounts --
 *
 *	This procedure is called to recompute all the counts in a node
 *	(tags, child information, etc.) by scaning the information in
 *	its descendants.  This procedure is called during rebalancing
 *	when a node's child structure has changed.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The tag counts for nodePtr are modified to reflect its current
 *	child structure, as are its numChildren and numLines fields.
 *	Also, all of the children's parentPtr fields are made to point
 *	to nodePtr.
 *
 *----------------------------------------------------------------------
 */

static void
RecomputeNodeCounts(nodePtr)
    register Node *nodePtr;		/* Node whose tag summary information
					 * must be recomputed. */
{
    register Summary *summaryPtr, *summaryPtr2;
    register Node *childPtr;
    register TkTextLine *linePtr;
    register TkAnnotation *annotPtr;

    /*
     * Zero out all the existing counts for the node, but don't delete
     * the existing Summary records (most of them will probably be reused).
     */

    for (summaryPtr = nodePtr->summaryPtr; summaryPtr != NULL;
	    summaryPtr = summaryPtr->nextPtr) {
	summaryPtr->toggleCount = 0;
    }
    nodePtr->numChildren = 0;
    nodePtr->numLines = 0;

    /*
     * Scan through the children, adding the childrens' tag counts into
     * the node's tag counts and adding new Summarys to the node if
     * necessary.
     */

    if (nodePtr->level == 0) {
	for (linePtr = nodePtr->children.linePtr; linePtr != NULL;
		linePtr = linePtr->nextPtr) {
	    nodePtr->numChildren++;
	    nodePtr->numLines++;
	    linePtr->parentPtr = nodePtr;
	    for (annotPtr = linePtr->annotPtr; annotPtr != NULL;
		    annotPtr = annotPtr->nextPtr) {
		if (annotPtr->type != TK_ANNOT_TOGGLE) {
		    continue;
		}
		for (summaryPtr = nodePtr->summaryPtr; ;
			summaryPtr = summaryPtr->nextPtr) {
		    if (summaryPtr == NULL) {
			summaryPtr = (Summary *) ckalloc(sizeof(Summary));
			summaryPtr->tagPtr = annotPtr->info.tagPtr;
			summaryPtr->toggleCount = 1;
			summaryPtr->nextPtr = nodePtr->summaryPtr;
			nodePtr->summaryPtr = summaryPtr;
			break;
		    }
		    if (summaryPtr->tagPtr == annotPtr->info.tagPtr) {
			summaryPtr->toggleCount++;
			break;
		    }
		}
	    }
	}
    } else {
	for (childPtr = nodePtr->children.nodePtr; childPtr != NULL;
		childPtr = childPtr->nextPtr) {
	    nodePtr->numChildren++;
	    nodePtr->numLines += childPtr->numLines;
	    childPtr->parentPtr = nodePtr;
	    for (summaryPtr2 = childPtr->summaryPtr; summaryPtr2 != NULL;
		    summaryPtr2 = summaryPtr2->nextPtr) {
		for (summaryPtr = nodePtr->summaryPtr; ;
			summaryPtr = summaryPtr->nextPtr) {
		    if (summaryPtr == NULL) {
			summaryPtr = (Summary *) ckalloc(sizeof(Summary));
			summaryPtr->tagPtr = summaryPtr2->tagPtr;
			summaryPtr->toggleCount = summaryPtr2->toggleCount;
			summaryPtr->nextPtr = nodePtr->summaryPtr;
			nodePtr->summaryPtr = summaryPtr;
			break;
		    }
		    if (summaryPtr->tagPtr == summaryPtr2->tagPtr) {
			summaryPtr->toggleCount += summaryPtr2->toggleCount;
			break;
		    }
		}
	    }
	}
    }

    /*
     * Scan through the node's tag records again and delete any Summary
     * records that still have a zero count.
     */

    summaryPtr2 = NULL;
    for (summaryPtr = nodePtr->summaryPtr; summaryPtr != NULL; ) {
	if (summaryPtr->toggleCount > 0) {
	    summaryPtr2 = summaryPtr;
	    summaryPtr = summaryPtr->nextPtr;
	    continue;
	}
	if (summaryPtr2 != NULL) {
	    summaryPtr2->nextPtr = summaryPtr->nextPtr;
	    ckfree((char *) summaryPtr);
	    summaryPtr = summaryPtr2->nextPtr;
	} else {
	    nodePtr->summaryPtr = summaryPtr->nextPtr;
	    ckfree((char *) summaryPtr);
	    summaryPtr = nodePtr->summaryPtr;
	}
    }
}

/*
 *----------------------------------------------------------------------
 *
 * AddToggleToLine --
 *
 *	Insert a tag transition at a particular point in a particular
 *	line.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	LinePtr and all its ancestors in the B-tree stucture are modified
 *	to indicate the presence of a transition (either on or off) on
 *	tag at the given place in the given line.
 *
 *----------------------------------------------------------------------
 */

static void
AddToggleToLine(linePtr, index, tagPtr)
    TkTextLine *linePtr;		/* Line within which to add
					 * transition. */
    int index;				/* Character before which to
					 * add transition. */
    TkTextTag *tagPtr;			/* Information about tag. */
{
    register TkAnnotation *annotPtr, *prevPtr;
    int delta = 1;

    /*
     * Find the position where the toggle should be inserted into
     * the array (just after prevPtr), and see if there is already
     * a toggle at exactly the point where we're going to insert a
     * new toggle.  If so then the two toggles cancel;  just delete
     * the existing toggle.
     */

    for (prevPtr = NULL, annotPtr = linePtr->annotPtr; annotPtr != NULL;
	    prevPtr = annotPtr, annotPtr = annotPtr->nextPtr) {
	if (annotPtr->ch > index) {
	    break;
	}
	if ((annotPtr->type == TK_ANNOT_TOGGLE)
		&& (annotPtr->ch == index)
		&& (annotPtr->info.tagPtr == tagPtr)) {
	    if (prevPtr == NULL) {
		linePtr->annotPtr = annotPtr->nextPtr;
	    } else {
		prevPtr->nextPtr = annotPtr->nextPtr;
	    }
	    ckfree((char *) annotPtr);
	    delta = -1;
	    goto updateNodes;
	}
    }

    /*
     * Create a new toggle and insert it into the list.
     */

    annotPtr = (TkAnnotation *) ckalloc(sizeof(TkAnnotation));
    annotPtr->type = TK_ANNOT_TOGGLE;
    annotPtr->linePtr = linePtr;
    annotPtr->ch = index;
    annotPtr->info.tagPtr = tagPtr;
    if (prevPtr == NULL) {
	annotPtr->nextPtr = linePtr->annotPtr;
	linePtr->annotPtr = annotPtr;
    } else {
	annotPtr->nextPtr = prevPtr->nextPtr;
	prevPtr->nextPtr = annotPtr;
    }

    /*
     * Update all the nodes above this line to reflect the change in
     * toggle structure.
     */

    updateNodes:
    ChangeNodeToggleCount(linePtr->parentPtr, tagPtr, delta);
}

/*
 *----------------------------------------------------------------------
 *
 * ChangeNodeToggleCount --
 *
 *	This procedure increments or decrements the toggle count for
 *	a particular tag in a particular node and all its ancestors.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The toggle count for tag is adjusted up or down by "delta" in
 *	nodePtr.
 *
 *----------------------------------------------------------------------
 */

static void
ChangeNodeToggleCount(nodePtr, tagPtr, delta)
    register Node *nodePtr;		/* Node whose toggle count for a tag
					 * must be changed. */
    TkTextTag *tagPtr;			/* Information about tag. */
    int delta;				/* Amount to add to current toggle
					 * count for tag (may be negative). */
{
    register Summary *summaryPtr, *prevPtr;

    /*
     * Iterate over the node and all of its ancestors.
     */

    for ( ; nodePtr != NULL; nodePtr = nodePtr->parentPtr) {
	/*
	 * See if there's already an entry for this tag for this node.  If so,
	 * perhaps all we have to do is adjust its count.
	 */
    
	for (prevPtr = NULL, summaryPtr = nodePtr->summaryPtr;
		summaryPtr != NULL;
		prevPtr = summaryPtr, summaryPtr = summaryPtr->nextPtr) {
	    if (summaryPtr->tagPtr != tagPtr) {
		continue;
	    }
	    summaryPtr->toggleCount += delta;
	    if (summaryPtr->toggleCount > 0) {
		goto nextAncestor;
	    }
	    if (summaryPtr->toggleCount < 0) {
		panic("ChangeNodeToggleCount: negative toggle count");
	    }
    
	    /*
	     * Zero count;  must remove this tag from the list.
	     */
    
	    if (prevPtr == NULL) {
		nodePtr->summaryPtr = summaryPtr->nextPtr;
	    } else {
		prevPtr->nextPtr = summaryPtr->nextPtr;
	    }
	    ckfree((char *) summaryPtr);
	    goto nextAncestor;
	}
    
	/*
	 * This tag isn't in the list.  Add a new entry to the list.
	 */
    
	if (delta < 0) {
	    panic("ChangeNodeToggleCount: negative delta, no tag entry");
	}
	summaryPtr = (Summary *) ckalloc(sizeof(Summary));
	summaryPtr->tagPtr = tagPtr;
	summaryPtr->toggleCount = delta;
	summaryPtr->nextPtr = nodePtr->summaryPtr;
	nodePtr->summaryPtr = summaryPtr;

	nextAncestor:
	continue;
    }
}

/*
 *----------------------------------------------------------------------
 *
 * TkBTreeCharTagged --
 *
 *	Determine whether a particular character has a particular tag.
 *
 * Results:
 *	The return value is 1 if the given tag is in effect at the
 *	character given by linePtr and ch, and 0 otherwise.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

int
TkBTreeCharTagged(linePtr, ch, tagPtr)
    TkTextLine *linePtr;		/* Line containing character of
					 * interest. */
    int ch;				/* Index of character in linePtr. */
    TkTextTag *tagPtr;			/* Tag of interest. */
{
    register Node *nodePtr;
    register TkTextLine *siblingLinePtr;
    int toggles;

    /*
     * Count the number of toggles for the tag at the line level (i.e.
     * in all the sibling lines that precede this one, plus in this line
     * up to the character of interest.
     */

    toggles = 0;
    for (siblingLinePtr = linePtr->parentPtr->children.linePtr; ;
	    siblingLinePtr = siblingLinePtr->nextPtr) {
	register TkAnnotation *annotPtr;

	for (annotPtr = siblingLinePtr->annotPtr;
		(annotPtr != NULL) && ((siblingLinePtr != linePtr)
		    || (annotPtr->ch <= ch));
		annotPtr = annotPtr->nextPtr) {
	    if ((annotPtr->type == TK_ANNOT_TOGGLE)
		    && (annotPtr->info.tagPtr == tagPtr)) {
		toggles++;
	    }
	}
	if (siblingLinePtr == linePtr) {
	    break;
	}
    }

    /*
     * For each node in the ancestry of this line, count the number of
     * toggles of the given tag in siblings that precede that node.
     */

    for (nodePtr = linePtr->parentPtr; nodePtr->parentPtr != NULL;
	    nodePtr = nodePtr->parentPtr) {
	register Node *siblingPtr;
	register Summary *summaryPtr;

	for (siblingPtr = nodePtr->parentPtr->children.nodePtr; 
		siblingPtr != nodePtr; siblingPtr = siblingPtr->nextPtr) {
	    for (summaryPtr = siblingPtr->summaryPtr; summaryPtr != NULL;
		    summaryPtr = summaryPtr->nextPtr) {
		if (summaryPtr->tagPtr == tagPtr) {
		    toggles += summaryPtr->toggleCount;
		}
	    }
	}
    }

    /*
     * An odd number of toggles means that the tag is present at the
     * given point.
     */

    return toggles & 1;
}

/*
 *----------------------------------------------------------------------
 *
 * TkBTreeGetTags --
 *
 *	Return information about all of the tags that are associated
 *	with a particular character in a B-tree of text.
 *
 * Results:
 *	The return value is a malloc-ed array containing pointers to
 *	information for each of the tags that is associated with
 *	the character at the position given by linePtr and ch.  The
 *	word at *numTagsPtr is filled in with the number of pointers
 *	in the array.  It is up to the caller to free the array by
 *	passing it to free.  If there are no tags at the given character
 *	then a NULL pointer is returned and *numTagsPtr will be set to 0.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

	/* ARGSUSED */
TkTextTag **
TkBTreeGetTags(tree, linePtr, ch, numTagsPtr)
    TkTextBTree tree;		/* Tree to check. */
    TkTextLine *linePtr;	/* Line containing character of interest. */
    int ch;			/* Index within linePtr of character for
				 * which tag information is wanted. */
    int *numTagsPtr;		/* Store number of tags found at this
				 * location. */
{
    register Node *nodePtr;
    register TkTextLine *siblingLinePtr;
    int src, dst;
    TagInfo tagInfo;
#define NUM_TAG_INFOS 10

    tagInfo.numTags = 0;
    tagInfo.arraySize = NUM_TAG_INFOS;
    tagInfo.tagPtrs = (TkTextTag **) ckalloc((unsigned)
	    NUM_TAG_INFOS*sizeof(TkTextTag *));
    tagInfo.counts = (int *) ckalloc((unsigned)
	    NUM_TAG_INFOS*sizeof(int));

    /*
     * Record tag toggles at the line level (i.e. in all the sibling
     * lines that precede this one, plus in this line up to the character
     * of interest.
     */

    for (siblingLinePtr = linePtr->parentPtr->children.linePtr; ;
	    siblingLinePtr = siblingLinePtr->nextPtr) {
	register TkAnnotation *annotPtr;

	for (annotPtr = siblingLinePtr->annotPtr;
		(annotPtr != NULL) && ((siblingLinePtr != linePtr)
		    || (annotPtr->ch <= ch));
		annotPtr = annotPtr->nextPtr) {
	    if (annotPtr->type == TK_ANNOT_TOGGLE) {
		IncCount(annotPtr->info.tagPtr, 1, &tagInfo);
	    }
	}
	if (siblingLinePtr == linePtr) {
	    break;
	}
    }

    /*
     * For each node in the ancestry of this line, record tag toggles
     * for all siblings that precede that node.
     */

    for (nodePtr = linePtr->parentPtr; nodePtr->parentPtr != NULL;
	    nodePtr = nodePtr->parentPtr) {
	register Node *siblingPtr;
	register Summary *summaryPtr;

	for (siblingPtr = nodePtr->parentPtr->children.nodePtr; 
		siblingPtr != nodePtr; siblingPtr = siblingPtr->nextPtr) {
	    for (summaryPtr = siblingPtr->summaryPtr; summaryPtr != NULL;
		    summaryPtr = summaryPtr->nextPtr) {
		IncCount(summaryPtr->tagPtr, summaryPtr->toggleCount, &tagInfo);
	    }
	}
    }

    /*
     * Go through the tag information and squash out all of the tags
     * that have even toggle counts (these tags exist before the point
     * of interest, but not at the desired character itself).
     */

    for (src = 0, dst = 0; src < tagInfo.numTags; src++) {
	if (tagInfo.counts[src] & 1) {
	    tagInfo.tagPtrs[dst] = tagInfo.tagPtrs[src];
	    dst++;
	}
    }
    *numTagsPtr = dst;
    ckfree((char *) tagInfo.counts);
    if (dst == 0) {
	ckfree((char *) tagInfo.tagPtrs);
	return NULL;
    }
    return tagInfo.tagPtrs;
}

/*
 *----------------------------------------------------------------------
 *
 * IncCount --
 *
 *	This is a utility procedure used by TkBTreeGetTags.  It
 *	increments the count for a particular tag, adding a new
 *	entry for that tag if there wasn't one previously.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The information at *tagInfoPtr may be modified, and the arrays
 *	may be reallocated to make them larger.
 *
 *----------------------------------------------------------------------
 */

static void
IncCount(tagPtr, inc, tagInfoPtr)
    TkTextTag *tagPtr;		/* Handle for tag. */
    int inc;			/* Amount by which to increment tag count. */
    TagInfo *tagInfoPtr;	/* Holds cumulative information about tags;
				 * increment count here. */
{
    register TkTextTag **tagPtrPtr;
    int count;

    for (tagPtrPtr = tagInfoPtr->tagPtrs, count = tagInfoPtr->numTags;
	    count > 0; tagPtrPtr++, count--) {
	if (*tagPtrPtr == tagPtr) {
	    tagInfoPtr->counts[tagInfoPtr->numTags-count] += inc;
	    return;
	}
    }

    /*
     * There isn't currently an entry for this tag, so we have to
     * make a new one.  If the arrays are full, then enlarge the
     * arrays first.
     */

    if (tagInfoPtr->numTags == tagInfoPtr->arraySize) {
	TkTextTag **newTags;
	int *newCounts, newSize;

	newSize = 2*tagInfoPtr->arraySize;
	newTags = (TkTextTag **) ckalloc((unsigned)
		(newSize*sizeof(TkTextTag *)));
	memcpy((VOID *) newTags, (VOID *) tagInfoPtr->tagPtrs,
		tagInfoPtr->arraySize * sizeof(TkTextTag *));
	ckfree((char *) tagInfoPtr->tagPtrs);
	tagInfoPtr->tagPtrs = newTags;
	newCounts = (int *) ckalloc((unsigned) (newSize*sizeof(int)));
	memcpy((VOID *) newCounts, (VOID *) tagInfoPtr->counts,
		tagInfoPtr->arraySize * sizeof(int));
	ckfree((char *) tagInfoPtr->counts);
	tagInfoPtr->counts = newCounts;
	tagInfoPtr->arraySize = newSize;
    }

    tagInfoPtr->tagPtrs[tagInfoPtr->numTags] = tagPtr;
    tagInfoPtr->counts[tagInfoPtr->numTags] = inc;
    tagInfoPtr->numTags++;
}

/*
 *----------------------------------------------------------------------
 *
 * CheckNodeConsistency --
 *
 *	This procedure is called as part of consistency checking for
 *	B-trees:  it checks several aspects of a node and also runs
 *	checks recursively on the node's children.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	If anything suspicious is found in the tree structure, the
 *	procedure panics.
 *
 *----------------------------------------------------------------------
 */

static void
CheckNodeConsistency(nodePtr)
    register Node *nodePtr;		/* Node whose subtree should be
					 * checked. */
{
    register Node *childNodePtr;
    register Summary *summaryPtr, *summaryPtr2;
    register TkAnnotation *annotPtr;
    register TkTextLine *linePtr;
    register char *p;
    int numChildren, numLines, toggleCount, minChildren, index, numBytes;

    if (nodePtr->parentPtr != NULL) {
	minChildren = MIN_CHILDREN;
    } else if (nodePtr->level > 0) {
	minChildren = 2;
    } else  {
	minChildren = 1;
    }
    if ((nodePtr->numChildren < minChildren)
	    || (nodePtr->numChildren > MAX_CHILDREN)) {
	panic("CheckNodeConsistency found bad child count (%d)",
		nodePtr->numChildren);
    }

    numChildren = 0;
    numLines = 0;
    if (nodePtr->level == 0) {
	for (linePtr = nodePtr->children.linePtr; linePtr != NULL;
		linePtr = linePtr->nextPtr) {
	    if (linePtr->parentPtr != nodePtr) {
		panic("CheckNodeConsistency found line that %s",
			"didn't point to parent");
	    }
	    for (p = linePtr->bytes, numBytes = 0; *p != 0; p++, numBytes++) {
		if ((*p == '\n') && (numBytes != linePtr->numBytes-1)) {
		    panic("CheckNodeConsistency found line with extra newline");
		}
	    }
	    if (numBytes != linePtr->numBytes) {
		panic("CheckNodeConsistency found line with bad numBytes");
	    }
	    if (linePtr->bytes[numBytes-1] != '\n') {
		panic("CheckNodeConsistency found line with no newline");
	    }
	    index = 0;
	    for (annotPtr = linePtr->annotPtr; annotPtr != NULL;
			annotPtr = annotPtr->nextPtr) {
		if (annotPtr->ch < index) {
		    panic("CheckNodeConsistency found %s (%d %d)",
			    "out-of-order tag indices", index,
			    annotPtr->ch);
		}
		index = annotPtr->ch;
		if (annotPtr->type == TK_ANNOT_TOGGLE) {
		    for (summaryPtr = nodePtr->summaryPtr; ;
			    summaryPtr = summaryPtr->nextPtr) {
			if (summaryPtr == NULL) {
			    panic("CheckNodeConsistency found line %s",
				    "tag with no node tag: %s",
				    summaryPtr->tagPtr->name);
			}
			if (summaryPtr->tagPtr == annotPtr->info.tagPtr) {
			    break;
			}
		    }
		}
	    }
	    numChildren++;
	    numLines++;
	}
    } else {
	for (childNodePtr = nodePtr->children.nodePtr; childNodePtr != NULL;
		childNodePtr = childNodePtr->nextPtr) {
	    CheckNodeConsistency(childNodePtr);
	    for (summaryPtr = childNodePtr->summaryPtr; summaryPtr != NULL;
			summaryPtr = summaryPtr->nextPtr) {
		for (summaryPtr2 = nodePtr->summaryPtr; ;
			summaryPtr2 = summaryPtr2->nextPtr) {
		    if (summaryPtr2 == NULL) {
			panic("CheckNodeConsistency found %s (%s)",
				"node tag with no parent tag",
				summaryPtr->tagPtr->name);
		    }
		    if (summaryPtr->tagPtr == summaryPtr2->tagPtr) {
			break;
		    }
		}
	    }
	    numChildren++;
	    numLines += childNodePtr->numLines;
	    if (childNodePtr->parentPtr != nodePtr) {
		panic("CheckNodeConsistency found node that %s",
			"didn't point to parent");
	    }
	    if (childNodePtr->level != (nodePtr->level-1)) {
		panic("CheckNodeConsistency found level mismatch (%d %d)",
			nodePtr->level, childNodePtr->level);
	    }
	}
    }
    if (numChildren != nodePtr->numChildren) {
	panic("CheckNodeConsistency found mismatch in numChildren (%d %d)",
		numChildren, nodePtr->numChildren);
    }
    if (numLines != nodePtr->numLines) {
	panic("CheckNodeConsistency found mismatch in numLines (%d %d)",
		numLines, nodePtr->numLines);
    }

    for (summaryPtr = nodePtr->summaryPtr; summaryPtr != NULL;
	    summaryPtr = summaryPtr->nextPtr) {
	toggleCount = 0;
	if (nodePtr->level == 0) {
	    for (linePtr = nodePtr->children.linePtr; linePtr != NULL;
		    linePtr = linePtr->nextPtr) {
		for (annotPtr = linePtr->annotPtr; annotPtr != NULL;
			annotPtr = annotPtr->nextPtr) {
		    if (annotPtr->info.tagPtr == summaryPtr->tagPtr) {
			toggleCount++;
		    }
		}
	    }
	} else {
	    for (childNodePtr = nodePtr->children.nodePtr;
		    childNodePtr != NULL;
		    childNodePtr = childNodePtr->nextPtr) {
		for (summaryPtr2 = childNodePtr->summaryPtr;
			summaryPtr2 != NULL;
			summaryPtr2 = summaryPtr2->nextPtr) {
		    if (summaryPtr2->tagPtr == summaryPtr->tagPtr) {
			toggleCount += summaryPtr2->toggleCount;
		    }
		}
	    }
	}
	if (toggleCount != summaryPtr->toggleCount) {
	    panic("CheckNodeConsistency found mismatch in toggleCount (%d %d)",
		    toggleCount, summaryPtr->toggleCount);
	}
	for (summaryPtr2 = summaryPtr->nextPtr; summaryPtr2 != NULL;
		summaryPtr2 = summaryPtr2->nextPtr) {
	    if (summaryPtr2->tagPtr == summaryPtr->tagPtr) {
		panic("CheckNodeConsistency found duplicated node tag: %s",
			summaryPtr->tagPtr->name);
	    }
	}
    }
}

/*
 *----------------------------------------------------------------------
 *
 * TkBTreeNumLines --
 *
 *	This procedure returns a count of the number of lines of
 *	text present in a given B-tree.
 *
 * Results:
 *	The return value is a count of the number of lines in tree.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

int
TkBTreeNumLines(tree)
    TkTextBTree tree;			/* Information about tree. */
{
    BTree *treePtr = (BTree *) tree;
    return treePtr->rootPtr->numLines;
}
