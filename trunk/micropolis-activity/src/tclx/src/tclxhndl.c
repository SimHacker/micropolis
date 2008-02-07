/*
 *
 * tclXhandles.c --
 *
 * Tcl handles.  Provides a mechanism for managing expandable tables that are
 * addressed by textual handles.
 *-----------------------------------------------------------------------------
 * Copyright 1992 Karl Lehenbauer and Mark Diekhans.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies.  Karl Lehenbauer and
 * Mark Diekhans make no representations about the suitability of this
 * software for any purpose.  It is provided "as is" without express or
 * implied warranty.
 *-----------------------------------------------------------------------------
 * $Id: tclXhandles.c,v 2.0 1992/10/16 04:50:49 markd Rel $
 *-----------------------------------------------------------------------------
 */

#include "tclxint.h"

/*
 * This is the table header.  It is separately allocated from the table body,
 * since it must keep track of a table body that might move.  Each entry in the
 * table is preceded with a header which has the free list link, which is a
 * entry index of the next free entry.  Special values keep track of allocated
 * entries.
 */

#define NULL_IDX      -1
#define ALLOCATED_IDX -2

typedef unsigned char ubyte_t;
typedef ubyte_t *ubyte_pt;

typedef struct {
    int      useCount;          /* Keeps track of the number sharing       */
    int      entrySize;         /* Entry size in bytes, including overhead */
    int      tableSize;         /* Current number of entries in the table  */
    int      freeHeadIdx;       /* Index of first free entry in the table  */
    ubyte_pt bodyP;             /* Pointer to table body                   */
    int      baseLength;        /* Length of handleBase.                   */
    char     handleBase [1];    /* Base handle name.  MUST BE LAST FIELD!  */
    } tblHeader_t;
typedef tblHeader_t *tblHeader_pt;

typedef struct {
    int freeLink;
  } entryHeader_t;
typedef entryHeader_t *entryHeader_pt;

/*
 * This macro is used to return a pointer to an entry, given its index.
 */
#define TBL_INDEX(hdrP, idx) \
    ((entryHeader_pt) (hdrP->bodyP + (hdrP->entrySize * idx)))

/*
 * This macros to convert between pointers to the user and header area of
 * an table entry.
 */
#define USER_AREA(entryPtr) \
 (void_pt) (((ubyte_pt) entryPtr) + sizeof (entryHeader_t));
#define HEADER_AREA(entryPtr) \
 (entryHeader_pt) (((ubyte_pt) entryPtr) - sizeof (entryHeader_t));

/*
 * Prototypes of internal functions.
 */
static void
LinkInNewEntries _ANSI_ARGS_((tblHeader_pt tblHdrPtr,
                              int          newIdx,
                              int          numEntries));

static void
ExpandTable _ANSI_ARGS_((tblHeader_pt tblHdrPtr,
                         int          neededIdx));

static entryHeader_pt
AllocEntry _ANSI_ARGS_((tblHeader_pt  tblHdrPtr,
                        int          *entryIdxPtr));

static int
HandleDecode _ANSI_ARGS_((Tcl_Interp   *interp,
                          tblHeader_pt  tblHdrPtr,
                          CONST char   *handle));

/*=============================================================================
 * LinkInNewEntries --
 *   Build free links through the newly allocated part of a table.
 *   
 * Parameters:
 *   o tblHdrPtr (I) - A pointer to the table header.
 *   o newIdx (I) - Index of the first new entry.
 *   o numEntries (I) - The number of new entries.
 *-----------------------------------------------------------------------------
 */
static void
LinkInNewEntries (tblHdrPtr, newIdx, numEntries)
    tblHeader_pt tblHdrPtr;
    int          newIdx;
    int          numEntries;
{
    int            entIdx, lastIdx;
    entryHeader_pt entryPtr;
    
    lastIdx = newIdx + numEntries - 1;

    for (entIdx = newIdx; entIdx < lastIdx; entIdx++) {
        entryPtr = TBL_INDEX (tblHdrPtr, entIdx);
        entryPtr->freeLink = entIdx + 1;
    }
    entryPtr = TBL_INDEX (tblHdrPtr, lastIdx);
    entryPtr->freeLink = tblHdrPtr->freeHeadIdx;
    tblHdrPtr->freeHeadIdx = newIdx;

} /* LinkInNewEntries */

/*=============================================================================
 * ExpandTable --
 *   Expand a handle table, doubling its size.
 * Parameters:
 *   o tblHdrPtr (I) - A pointer to the table header.
 *   o neededIdx (I) - If positive, then the table will be expanded so that
 *     this entry is available.  If -1, then just expand by the number of 
 *     entries specified on table creation.  MUST be smaller than this size.
 *-----------------------------------------------------------------------------
 */
static void
ExpandTable (tblHdrPtr, neededIdx)
    tblHeader_pt tblHdrPtr;
    int          neededIdx;
{
    ubyte_pt oldBodyP = tblHdrPtr->bodyP;
    int      numNewEntries;
    int      newSize;
    
    if (neededIdx < 0)
        numNewEntries = tblHdrPtr->tableSize;
    else
        numNewEntries = (neededIdx - tblHdrPtr->tableSize) + 1;
    newSize = (tblHdrPtr->tableSize + numNewEntries) * tblHdrPtr->entrySize;

    tblHdrPtr->bodyP = (ubyte_pt) ckalloc (newSize);
    memcpy (tblHdrPtr->bodyP, oldBodyP, newSize);
    LinkInNewEntries (tblHdrPtr, tblHdrPtr->tableSize, numNewEntries);
    tblHdrPtr->tableSize += numNewEntries;
    ckfree (oldBodyP);
    
} /* ExpandTable */

/*=============================================================================
 * AllocEntry --
 *   Allocate a table entry, expanding if necessary.
 *
 * Parameters:
 *   o tblHdrPtr (I) - A pointer to the table header.
 *   o entryIdxPtr (O) - The index of the table entry is returned here.
 * Returns:
 *    The a pointer to the entry.
 *-----------------------------------------------------------------------------
 */
static entryHeader_pt
AllocEntry (tblHdrPtr, entryIdxPtr)
    tblHeader_pt  tblHdrPtr;
    int          *entryIdxPtr;
{
    int            entryIdx;
    entryHeader_pt entryPtr;

    if (tblHdrPtr->freeHeadIdx == NULL_IDX)
        ExpandTable (tblHdrPtr, -1);

    entryIdx = tblHdrPtr->freeHeadIdx;    
    entryPtr = TBL_INDEX (tblHdrPtr, entryIdx);
    tblHdrPtr->freeHeadIdx = entryPtr->freeLink;
    entryPtr->freeLink = ALLOCATED_IDX;
    
    *entryIdxPtr = entryIdx;
    return entryPtr;
    
} /* AllocEntry */

/*=============================================================================
 * HandleDecode --
 *   Decode handle into an entry number.
 *
 * Parameters:
 *   o interp (I) - A error message may be returned in result.
 *   o tblHdrPtr (I) - A pointer to the table header.
 *   o handle (I) - Handle to decode.
 * Returns:
 *   The entry index decoded from the handle, or a negative number if an error
 *   occured.
 *-----------------------------------------------------------------------------
 */
static int
HandleDecode (interp, tblHdrPtr, handle)
    Tcl_Interp   *interp;
    tblHeader_pt  tblHdrPtr;
    CONST char   *handle;
{
    unsigned entryIdx;

    if ((strncmp (tblHdrPtr->handleBase, (char *) handle, 
             tblHdrPtr->baseLength) != 0) ||
             !Tcl_StrToUnsigned (&handle [tblHdrPtr->baseLength], 10, 
                                 &entryIdx)) {
        Tcl_AppendResult (interp, "invalid ", tblHdrPtr->handleBase,
                          " handle: ", handle, (char *) NULL);
        return -1;
    }
    return entryIdx;

} /* HandleDecode */

/*=============================================================================
 * Tcl_HandleTblInit --
 *   Create and initialize a Tcl dynamic handle table.  The use count on the
 *   table is set to one.
 * Parameters:
 *   o handleBase(I) - The base name of the handle, the handle will be returned
 *     in the form "baseNN", where NN is the table entry number.
 *   o entrySize (I) - The size of an entry, in bytes.
 *   o initEntries (I) - Initial size of the table, in entries.
 * Returns:
 *   A pointer to the table header.  
 *-----------------------------------------------------------------------------
 */
void_pt
Tcl_HandleTblInit (handleBase, entrySize, initEntries)
    CONST char *handleBase;
    int         entrySize;
    int         initEntries;
{
    tblHeader_pt tblHdrPtr;
    int          baseLength = strlen ((char *) handleBase);

    tblHdrPtr = (tblHeader_pt) ckalloc (sizeof (tblHeader_t) + baseLength + 1);

    tblHdrPtr->useCount = 1;
    tblHdrPtr->baseLength = baseLength;
    strcpy (tblHdrPtr->handleBase, (char *) handleBase);

    /* 
     * Calculate entry size, including header, rounded up to sizeof (int). 
     */
    tblHdrPtr->entrySize = entrySize + sizeof (entryHeader_t);
    tblHdrPtr->entrySize = ((tblHdrPtr->entrySize + sizeof (int) - 1) / 
                          sizeof (int)) * sizeof (int);
    tblHdrPtr->freeHeadIdx = NULL_IDX;
    tblHdrPtr->tableSize = initEntries;
    tblHdrPtr->bodyP = (ubyte_pt) ckalloc (initEntries * tblHdrPtr->entrySize);
    LinkInNewEntries (tblHdrPtr, 0, initEntries);

    return (void_pt) tblHdrPtr;

} /* Tcl_HandleTblInit */

/*=============================================================================
 * Tcl_HandleTblUseCount --
 *   Alter the handle table use count by the specified amount, which can be
 *   positive or negative.  Amount may be zero to retrieve the use count.
 * Parameters:
 *   o headerPtr (I) - Pointer to the table header.
 *   o amount (I) - The amount to alter the use count by.
 * Returns:
 *   The resulting use count.
 *-----------------------------------------------------------------------------
 */
int
Tcl_HandleTblUseCount (headerPtr, amount)
    void_pt  headerPtr;
    int      amount;
{
    tblHeader_pt   tblHdrPtr = (tblHeader_pt)headerPtr;
        
    tblHdrPtr->useCount += amount;
    return tblHdrPtr->useCount;
}

/*=============================================================================
 * Tcl_HandleTblRelease --
 *   Decrement the use count on a Tcl dynamic handle table.  If the count
 *   goes to zero or negative, then release the table.  It is designed to be 
 *   called when a command is released.
 * Parameters:
 *   o headerPtr (I) - Pointer to the table header.
 *-----------------------------------------------------------------------------
 */
void
Tcl_HandleTblRelease (headerPtr)
    void_pt headerPtr;
{
    tblHeader_pt   tblHdrPtr = (tblHeader_pt)headerPtr;

    tblHdrPtr->useCount--;
    if (tblHdrPtr->useCount <= 0) {
        ckfree (tblHdrPtr->bodyP);
        ckfree ((char *) tblHdrPtr);
    }
}

/*=============================================================================
 * Tcl_HandleAlloc --
 *   Allocate an entry and associate a handle with it.
 *
 * Parameters:
 *   o headerPtr (I) - A pointer to the table header.
 *   o handlePtr (O) - Buffer to return handle in. It must be big enough to
 *     hold the name.
 * Returns:
 *   A pointer to the allocated entry (user part).
 *-----------------------------------------------------------------------------
 */
void_pt
Tcl_HandleAlloc (headerPtr, handlePtr)
    void_pt   headerPtr;
    char     *handlePtr;
{
    tblHeader_pt   tblHdrPtr = (tblHeader_pt)headerPtr;
    entryHeader_pt entryPtr;
    int            entryIdx;

    entryPtr = AllocEntry ((tblHeader_pt) headerPtr, &entryIdx);
    sprintf (handlePtr, "%s%d", tblHdrPtr->handleBase, entryIdx);
     
    return USER_AREA (entryPtr);

} /* Tcl_HandleAlloc */

/*=============================================================================
 * Tcl_HandleXlate --
 *   Translate a handle to a entry pointer.
 *
 * Parameters:
 *   o interp (I) - A error message may be returned in result.
 *   o headerPtr (I) - A pointer to the table header.
 *   o handle (I) - The handle assigned to the entry.
 * Returns:
 *   A pointer to the entry, or NULL if an error occured.
 *-----------------------------------------------------------------------------
 */
void_pt
Tcl_HandleXlate (interp, headerPtr, handle)
    Tcl_Interp *interp;
    void_pt     headerPtr;
    CONST char *handle;
{
    tblHeader_pt   tblHdrPtr = (tblHeader_pt)headerPtr;
    entryHeader_pt entryPtr;
    int            entryIdx;
    
    if ((entryIdx = HandleDecode (interp, tblHdrPtr, handle)) < 0)
        return NULL;
    entryPtr = TBL_INDEX (tblHdrPtr, entryIdx);

    if ((entryIdx >= tblHdrPtr->tableSize) ||
            (entryPtr->freeLink != ALLOCATED_IDX)) {
        Tcl_AppendResult (interp, tblHdrPtr->handleBase, " is not open",
                          (char *) NULL);
        return NULL;
    }     

    return USER_AREA (entryPtr);
 
} /* Tcl_HandleXlate */

/*=============================================================================
 * Tcl_HandleWalk --
 *   Walk through and find every allocated entry in a table.  Entries may
 *   be deallocated during a walk, but should not be allocated.
 *
 * Parameters:
 *   o headerPtr (I) - A pointer to the table header.
 *   o walkKeyPtr (I/O) - Pointer to a variable to use to keep track of the
 *     place in the table.  The variable should be initialized to -1 before
 *     the first call.
 * Returns:
 *   A pointer to the next allocated entry, or NULL if there are not more.
 *-----------------------------------------------------------------------------
 */
void_pt
Tcl_HandleWalk (headerPtr, walkKeyPtr)
    void_pt   headerPtr;
    int      *walkKeyPtr;
{
    tblHeader_pt   tblHdrPtr = (tblHeader_pt)headerPtr;
    int            entryIdx;
    entryHeader_pt entryPtr;

    if (*walkKeyPtr == -1)
        entryIdx = 0;
    else
        entryIdx = *walkKeyPtr + 1;
        
    while (entryIdx < tblHdrPtr->tableSize) {
        entryPtr = TBL_INDEX (tblHdrPtr, entryIdx);
        if (entryPtr->freeLink == ALLOCATED_IDX) {
            *walkKeyPtr = entryIdx;
            return USER_AREA (entryPtr);
        }
        entryIdx++;
    }
    return NULL;

} /* Tcl_HandleWalk */

/*=============================================================================
 * Tcl_WalkKeyToHandle --
 *   Convert a walk key, as returned from a call to Tcl_HandleWalk into a
 *   handle.  The Tcl_HandleWalk must have succeeded.
 * Parameters:
 *   o headerPtr (I) - A pointer to the table header.
 *   o walkKey (I) - The walk key.
 *   o handlePtr (O) - Buffer to return handle in. It must be big enough to
 *     hold the name.
 *-----------------------------------------------------------------------------
 */
void
Tcl_WalkKeyToHandle (headerPtr, walkKey, handlePtr)
    void_pt   headerPtr;
    int       walkKey;
    char     *handlePtr;
{
    tblHeader_pt   tblHdrPtr = (tblHeader_pt)headerPtr;

    sprintf (handlePtr, "%s%d", tblHdrPtr->handleBase, walkKey);

} /* Tcl_WalkKeyToHandle */

/*=============================================================================
 * Tcl_HandleFree --
 *   Frees a handle table entry.
 *
 * Parameters:
 *   o headerPtr (I) - A pointer to the table header.
 *   o entryPtr (I) - Entry to free.
 *-----------------------------------------------------------------------------
 */
void
Tcl_HandleFree (headerPtr, entryPtr)
    void_pt headerPtr;
    void_pt entryPtr;
{
    tblHeader_pt   tblHdrPtr = (tblHeader_pt)headerPtr;
    entryHeader_pt freeentryPtr;

    freeentryPtr = HEADER_AREA (entryPtr);
    freeentryPtr->freeLink = tblHdrPtr->freeHeadIdx;
    tblHdrPtr->freeHeadIdx = (((ubyte_pt) entryPtr) - tblHdrPtr->bodyP) /
                           tblHdrPtr->entrySize;
    
} /* Tcl_HandleFree */

