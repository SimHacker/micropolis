/*
 * tclXbsearch.c
 *
 * Extended Tcl binary file search command.
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
 * $Id: tclXbsearch.c,v 2.0 1992/10/16 04:50:24 markd Rel $
 *-----------------------------------------------------------------------------
 */

#include "tclxint.h"

/*
 * Control block used to pass data used by the binary search routines.
 */
typedef struct binSearchCB_t {
    Tcl_Interp   *interp;         /* Pointer to the interpreter.             */
    char         *fileHandle;     /* Handle of file.                         */
    char         *key;            /* The key to search for.                  */

    FILE         *fileCBPtr;      /* Open file structure.                    */
    dynamicBuf_t  dynBuf;         /* Dynamic buffer to hold a line of file.  */
    long          lastRecOffset;  /* Offset of last record read.             */
    int           cmpResult;      /* -1, 0 or 1 result of string compare.    */
    char         *tclProc;        /* Name of Tcl comparsion proc, or NULL.   */
    } binSearchCB_t;

/*
 * Prototypes of internal functions.
 */
static int
StandardKeyCompare _ANSI_ARGS_((char *key,
                                char *line));

static int
TclProcKeyCompare _ANSI_ARGS_((binSearchCB_t *searchCBPtr));

static int
ReadAndCompare _ANSI_ARGS_((long           fileOffset,
                            binSearchCB_t *searchCBPtr));

static int
BinSearch _ANSI_ARGS_((binSearchCB_t *searchCBPtr));

/*
 *-----------------------------------------------------------------------------
 *
 * StandardKeyCompare --
 *    Standard comparison routine for BinSearch, compares the key to the
 *    first white-space seperated field in the line.
 *
 * Parameters:
 *   o key (I) - The key to search for.
 *   o line (I) - The line to compare the key to.
 *
 * Results:
 *   o < 0 if key < line-key
 *   o = 0 if key == line-key
 *   o > 0 if key > line-key.
 *-----------------------------------------------------------------------------
 */
static int
StandardKeyCompare (key, line)
    char *key;
    char *line;
{
    int  cmpResult, fieldLen;
    char saveChar;

    fieldLen = strcspn (line, " \t\r\n\v\f");

    saveChar = line [fieldLen];
    line [fieldLen] = 0;
    cmpResult = strcmp (key, line);
    line [fieldLen] = saveChar;

    return cmpResult;
}

/*
 *-----------------------------------------------------------------------------
 *
 * TclProcKeyCompare --
 *    Comparison routine for BinSearch that runs a Tcl procedure to, 
 *    compare the key to a line from the file.
 *
 * Parameters:
 *   o searchCBPtr (I/O) - The search control block, the line should be in
 *     dynBuf, the comparsion result is returned in cmpResult.
 *
 * Results:
 *   TCL_OK or TCL_ERROR.
 *-----------------------------------------------------------------------------
 */
static int
TclProcKeyCompare (searchCBPtr)
    binSearchCB_t *searchCBPtr;
{
    char *cmdArgv [3];
    char *command;
    int   result;

    cmdArgv [0] = searchCBPtr->tclProc;
    cmdArgv [1] = searchCBPtr->key;
    cmdArgv [2] = searchCBPtr->dynBuf.ptr;
    command = Tcl_Merge (3, cmdArgv);

    result = Tcl_Eval (searchCBPtr->interp, command, 0, (char **) NULL);

    ckfree (command);
    if (result == TCL_ERROR)
        return TCL_ERROR;

    if (!Tcl_StrToInt (searchCBPtr->interp->result, 0, 
                       &searchCBPtr->cmpResult)) {
        char *oldResult = ckalloc (strlen (searchCBPtr->interp->result + 1));
        
        strcpy (oldResult, searchCBPtr->interp->result);
        Tcl_ResetResult (searchCBPtr->interp);
        Tcl_AppendResult (searchCBPtr->interp, "invalid integer \"", oldResult,
                          "\" returned from compare proc \"",
                          searchCBPtr->tclProc, "\"", (char *) NULL);
        ckfree (oldResult);
        return TCL_ERROR;
    }
    Tcl_ResetResult (searchCBPtr->interp);
    return TCL_OK;
}

/*
 *-----------------------------------------------------------------------------
 *
 * ReadAndCompare --
 *    Search for the next line in the file starting at the specified
 *    offset.  Read the line into the dynamic buffer and compare it to
 *    the key using the specified comparison method.  The start of the
 *    last line read is saved in the control block, and if the start of
 *    the same line is found in the search, then it will not be recompared.
 *    This is needed since the search algorithm has to hit the same line
 *    a couple of times before failing, due to the fact that the records are
 *    not fixed length.
 *
 * Parameters:
 *   o fileOffset (I) - The offset of the next byte of the search, not
 *     necessarly the start of a record.
 *   o searchCBPtr (I/O) - The search control block, the comparsion result
 *     is returned in cmpResult.  If the EOF is hit, a less-than result is
 *     returned.
 *
 * Results:
 *   TCL_OK or TCL_ERROR.
 *-----------------------------------------------------------------------------
 */
static int
ReadAndCompare (fileOffset, searchCBPtr)
    long           fileOffset;
    binSearchCB_t *searchCBPtr;
{
    int  recChar, status;

    if (fseek (searchCBPtr->fileCBPtr, fileOffset, SEEK_SET) != 0)
        goto unixError;

    /*
     * Go to beginning of next line.
     */
    
    if (fileOffset != 0) {
        while (((recChar = getc (searchCBPtr->fileCBPtr)) != EOF) &&
                (recChar != '\n'))
            fileOffset++;
        if ((recChar == EOF) && ferror (searchCBPtr->fileCBPtr))
            goto unixError;
    }
    /*
     * If this is the same line as before, then just leave the comparison
     * result unchanged.
     */
    if (fileOffset == searchCBPtr->lastRecOffset)
        return TCL_OK;

    searchCBPtr->lastRecOffset = fileOffset;

    status = Tcl_DynamicFgets (&searchCBPtr->dynBuf, searchCBPtr->fileCBPtr, 
                               FALSE);
    if (status < 0)
        goto unixError;

    /* 
     * Only compare if EOF was not hit, otherwise, treat as if we went
     * above the key we are looking for.
     */
    if (status == 0) {
        searchCBPtr->cmpResult = -1;
        return TCL_OK;
    }

    if (searchCBPtr->tclProc == NULL) {
        searchCBPtr->cmpResult = StandardKeyCompare (searchCBPtr->key, 
                                                     searchCBPtr->dynBuf.ptr);
    } else {
        if (TclProcKeyCompare (searchCBPtr) != TCL_OK)
            return TCL_ERROR;
    }

    return TCL_OK;

unixError:
   Tcl_AppendResult (searchCBPtr->interp, searchCBPtr->fileHandle, ": ",
                     Tcl_UnixError (searchCBPtr->interp), (char *) NULL);
   return TCL_ERROR;
}

/*
 *-----------------------------------------------------------------------------
 *
 * BinSearch --
 *      Binary search a sorted ASCII file.
 *
 * Parameters:
 *   o searchCBPtr (I/O) - The search control block, if the line is found,
 *     it is returned in dynBuf.
 * Results:
 *     TCL_OK - If the key was found.
 *     TCL_BREAK - If it was not found.
 *     TCL_ERROR - If there was an error.
 *
 * based on getpath.c from smail 2.5 (9/15/87)
 *
 *-----------------------------------------------------------------------------
 */
static int
BinSearch (searchCBPtr)
    binSearchCB_t *searchCBPtr;
{
    OpenFile   *filePtr;
    long        middle, high, low;
    struct stat statBuf;

    if (TclGetOpenFile (searchCBPtr->interp, searchCBPtr->fileHandle, 
                        &filePtr) != TCL_OK)
        return TCL_ERROR;

    searchCBPtr->fileCBPtr = filePtr->f;
    searchCBPtr->lastRecOffset = -1;

    if (fstat (fileno (searchCBPtr->fileCBPtr), &statBuf) < 0)
        goto unixError;

    low = 0;
    high = statBuf.st_size;

    /*
     * "Binary search routines are never written right the first time around."
     * - Robert G. Sheldon.
     */

    while (TRUE) {
        middle = (high + low + 1) / 2;

        if (ReadAndCompare (middle, searchCBPtr) != TCL_OK)
            return TCL_ERROR;

        if (searchCBPtr->cmpResult == 0)
            return TCL_OK;     /* Found   */
        
        if (low >= middle)  
            return TCL_BREAK;  /* Failure */

        /*
         * Close window.
         */
        if (searchCBPtr->cmpResult > 0) {
            low = middle;
        } else {
            high = middle - 1;
        }
    }

unixError:
   Tcl_AppendResult (searchCBPtr->interp, searchCBPtr->fileHandle, ": ",
                     Tcl_UnixError (searchCBPtr->interp), (char *) NULL);
   return TCL_ERROR;
}

/*
 *-----------------------------------------------------------------------------
 *
 * Tcl_BsearchCmd --
 *     Implements the TCL bsearch command:
 *        bsearch filehandle key [retvar]
 *
 * Results:
 *      Standard TCL results.
 *
 *-----------------------------------------------------------------------------
 */
int
Tcl_BsearchCmd (clientData, interp, argc, argv)
    ClientData  clientData;
    Tcl_Interp *interp;
    int         argc;
    char      **argv;
{
    int           status;
    binSearchCB_t searchCB;

    if ((argc < 3) || (argc > 5)) {
        Tcl_AppendResult (interp, tclXWrongArgs, argv [0], 
                          " handle key [retvar] [compare_proc]"
                          , (char *) NULL);
        return TCL_ERROR;
    }

    searchCB.interp = interp;
    searchCB.fileHandle = argv [1];
    searchCB.key = argv [2];
    searchCB.tclProc = (argc == 5) ? argv [4] : NULL;
    Tcl_DynBufInit (&searchCB.dynBuf);

    status = BinSearch (&searchCB);
    if (status == TCL_ERROR) {
        Tcl_DynBufFree (&searchCB.dynBuf);
        return TCL_ERROR;
    }

    if (status == TCL_BREAK) {
        Tcl_DynBufFree (&searchCB.dynBuf);
        if ((argc >= 4) && (argv [3][0] != '\0'))
            interp->result = "0";
        return TCL_OK;
    }

    if ((argc == 3) || (argv [3][0] == '\0')) {
        Tcl_DynBufReturn (interp, &searchCB.dynBuf);
    } else {
        char *varPtr;

        varPtr = Tcl_SetVar (interp, argv[3], searchCB.dynBuf.ptr,
                             TCL_LEAVE_ERR_MSG);
        Tcl_DynBufFree (&searchCB.dynBuf);
        if (varPtr == NULL)
            return TCL_ERROR;
        interp->result = "1";
    }
    return TCL_OK;
}
