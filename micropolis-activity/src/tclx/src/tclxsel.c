/*
 * tclXselect.c
 *
 * Extended Tcl file I/O commands.
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
 * $Id: tclXselect.c,v 2.0 1992/10/16 04:51:10 markd Rel $
 *-----------------------------------------------------------------------------
 */

#include "tclxint.h"

#ifdef MSDOS
typedef struct {
	long	fds_bits[1];
} fd_set;		/* GRB for Micropolis */
#endif

extern
double floor ();

#ifdef TCL_USE_BZERO_MACRO
#    define bzero(to,length)    memset(to,'\0',length)
#endif

/*
 * Macro to probe the stdio buffer to see if any data is pending in the
 * buffer.  Different versions are provided for System V and BSD stdio.
 */

#ifdef __SLBF
#   define READ_DATA_PENDING(fp) (fp->_r > 0)
#else
#   define READ_DATA_PENDING(fp) (fp->_cnt != 0)
#endif

/*
 * A few systems (A/UX 2.0) have select but no macros, define em in this case.
 */
#if !defined(TCL_NO_SELECT) && !defined(FD_SET)
#   define FD_SET(fd,fdset)	(fdset)->fds_bits[0] |= (1<<(fd))
#   define FD_CLR(fd,fdset)	(fdset)->fds_bits[0] &= ~(1<<(fd))
#   define FD_ZERO(fdset)		(fdset)->fds_bits[0] = 0
#   define FD_ISSET(fd,fdset)	(((fdset)->fds_bits[0]) & (1<<(fd)))
#endif

/*
 * Prototypes of internal functions.
 */
static int
ParseSelectFileList _ANSI_ARGS_((Tcl_Interp *interp,
                                 char       *handleList,
                                 fd_set     *fileDescSetPtr,
                                 FILE     ***fileDescListPtr,
                                 int        *maxFileIdPtr));

static int
FindPendingData _ANSI_ARGS_((int         fileDescCnt,
                             FILE      **fileDescList,
                             fd_set     *fileDescSetPtr));

static char *
ReturnSelectedFileList _ANSI_ARGS_((fd_set     *fileDescSetPtr,
                                    fd_set     *fileDescSet2Ptr,
                                    int         fileDescCnt,
                                    FILE      **fileDescList));

#ifndef TCL_NO_SELECT

/*
 *-----------------------------------------------------------------------------
 *
 * ParseSelectFileList --
 *
 *   Parse a list of file handles for select.
 *
 * Parameters:
 *   o interp (O) - Error messages are returned in the result.
 *   o handleList (I) - The list of file handles to parse, may be empty.
 *   o fileDescSetPtr (O) - The select fd_set for the parsed handles is
 *     filled in.  Should be cleared before this procedure is called.
 *   o fileDescListPtr (O) - A pointer to a dynamically allocated list of
 *     the FILE ptrs that are in the set.  If the list is empty, NULL is
 *     returned.
 *   o maxFileIdPtr (I/O) - If a file id greater than the current value is
 *     encountered, it will be set to that file id.
 * Returns:
 *   The number of files in the list, or -1 if an error occured.
 *-----------------------------------------------------------------------------
 */
static int
ParseSelectFileList (interp, handleList, fileDescSetPtr, fileDescListPtr,
                     maxFileIdPtr)
    Tcl_Interp *interp;
    char       *handleList;
    fd_set     *fileDescSetPtr;
    FILE     ***fileDescListPtr;
    int        *maxFileIdPtr;
{
    int    handleCnt, idx;
    char **handleArgv;
    FILE **fileDescList;

    /*
     * Optimize empty list handling.
     */
    if (handleList [0] == '\0') {
        *fileDescListPtr = NULL;
        return 0;
    }

    if (Tcl_SplitList (interp, handleList, &handleCnt, &handleArgv) != TCL_OK)
        return -1;

    /*
     * Handle case of an empty list.
     */
    if (handleCnt == 0) {
        *fileDescListPtr = NULL;
        ckfree ((char *) handleArgv);
        return 0;
    }

    fileDescList = (FILE **) ckalloc (sizeof (FILE *) * handleCnt);

    for (idx = 0; idx < handleCnt; idx++) {
        OpenFile *filePtr;
        int       fileId;

        if (TclGetOpenFile (interp, handleArgv [idx], &filePtr) != TCL_OK) {
            ckfree ((char *) handleArgv);
            ckfree ((char *) fileDescList);
            return -1;
        }
        fileId = fileno (filePtr->f);
        fileDescList [idx] = filePtr->f;

        FD_SET (fileId, fileDescSetPtr);
        if (fileId > *maxFileIdPtr)
            *maxFileIdPtr = fileId;
    }

    *fileDescListPtr = fileDescList;
    ckfree ((char *) handleArgv);
    return handleCnt;
}

/*
 *-----------------------------------------------------------------------------
 *
 * FindPendingData --
 *
 *   Scan a list of read file descriptors to determine if any of them
 *   have data pending in their stdio buffers.
 *
 * Parameters:
 *   o fileDescCnt (I) - Number of descriptors in the list.
 *   o fileDescListPtr (I) - A pointer to a list of the FILE pointers for
 *     files that are in the set.
 *   o fileDescSetPtr (I) - A select fd_set with will have a bit set for
 *     every file that has data pending it its buffer.
 * Returns:
 *   TRUE if any where found that had pending data, FALSE if none were found.
 *-----------------------------------------------------------------------------
 */
static int
FindPendingData (fileDescCnt, fileDescList, fileDescSetPtr)
    int         fileDescCnt;
    FILE      **fileDescList;
    fd_set     *fileDescSetPtr;
{
    int idx, found = FALSE;

    FD_ZERO (fileDescSetPtr);

#ifndef IS_LINUX
    for (idx = 0; idx < fileDescCnt; idx++) {
        if (READ_DATA_PENDING (fileDescList [idx])) {
            FD_SET (fileno (fileDescList [idx]), fileDescSetPtr);
            found = TRUE;
        }
    }
#endif
    return found;
}

/*
 *-----------------------------------------------------------------------------
 *
 * ReturnSelectedFileList --
 *
 *   Take the resulting file descriptor sets from a select, and the
 *   list of file descritpors and build up a list of Tcl file handles.
 *
 * Parameters:
 *   o fileDescSetPtr (I) - The select fd_set.
 *   o fileDescSet2Ptr (I) - Pointer to a second descriptor to also check
 *     (their may be overlap).  NULL if no second set.
 *   o fileDescCnt (I) - Number of descriptors in the list.
 *   o fileDescListPtr (I) - A pointer to a list of the FILE pointers for
 *     files that are in the set.  If the list is empty, NULL is returned.
 * Returns:
 *   A dynamicly allocated list of file handles.  If the handles are empty,
 *   it still returns a NULL list to make clean up easy.
 *-----------------------------------------------------------------------------
 */
static char *
ReturnSelectedFileList (fileDescSetPtr, fileDescSet2Ptr, fileDescCnt,
                        fileDescList) 
    fd_set     *fileDescSetPtr;
    fd_set     *fileDescSet2Ptr;
    int         fileDescCnt;
    FILE      **fileDescList;
{
    int    idx, handleCnt, fileNum;
    char  *fileHandleList;
    char **fileHandleArgv, *nextByte;

    /*
     * Special case the empty list.
     */
    if (fileDescCnt == 0) {
        fileHandleList = ckalloc (1);
        fileHandleList [0] = '\0';
        return fileHandleList;
    }

    /*
     * Allocate enough room to hold the argv plus all the `fileNNN' strings
     */
    fileHandleArgv = (char **)
        ckalloc ((fileDescCnt * sizeof (char *)) + (9 * fileDescCnt));
    nextByte = ((char *) fileHandleArgv) + (fileDescCnt * sizeof (char *));

    handleCnt = 0;
    for (idx = 0; idx < fileDescCnt; idx++) {
        fileNum = fileno (fileDescList [idx]);

        if (FD_ISSET (fileNum, fileDescSetPtr) ||
            (fileDescSet2Ptr != NULL &&
             FD_ISSET (fileNum, fileDescSet2Ptr))) {

            fileHandleArgv [handleCnt] = nextByte;  /* Allocate storage */
            nextByte += 8;
            sprintf (fileHandleArgv [handleCnt], "file%d", fileNum);
            handleCnt++;
        }
    }

    fileHandleList = Tcl_Merge (handleCnt, fileHandleArgv);
    ckfree ((char *) fileHandleArgv);

    return fileHandleList;
}

/*
 *-----------------------------------------------------------------------------
 *
 * Tcl_SelectCmd --
 *  Implements the select TCL command:
 *      select readhandles [writehandles] [excepthandles] [timeout]
 *
 *  This command is extra smart in the fact that it checks for read data
 * pending in the stdio buffer first before doing a select.
 *   
 * Results:
 *     A list in the form:
 *        {readhandles writehandles excepthandles}
 *     or {} it the timeout expired.
 *-----------------------------------------------------------------------------
 */
int
Tcl_SelectCmd (clientData, interp, argc, argv)
    ClientData  clientData;
    Tcl_Interp *interp;
    int         argc;
    char      **argv;
{

    fd_set readFdSet,            writeFdSet,            exceptFdSet;
    int    readDescCnt = 0,      writeDescCnt = 0,      exceptDescCnt = 0;
    FILE **readDescList = NULL,**writeDescList = NULL,**exceptDescList = NULL;
    fd_set readFdSet2;
    char  *retListArgv [3];

    int             numSelected, maxFileId = 0, pending;
    int             result = TCL_ERROR;
    struct timeval  timeoutRec;
    struct timeval *timeoutRecPtr;


    if (argc < 2) {
        Tcl_AppendResult (interp, tclXWrongArgs, argv [0], 
                          " readhandles [writehandles] [excepthandles]",
                          " [timeout]", (char *) NULL);
        return TCL_ERROR;
    }
    
    /*
     * Parse the file handles and set everything up for the select call.
     */
    FD_ZERO (&readFdSet);
    FD_ZERO (&writeFdSet);
    FD_ZERO (&exceptFdSet);
    readDescCnt = ParseSelectFileList (interp, argv [1], &readFdSet, 
                                       &readDescList, &maxFileId);
    if (readDescCnt < 0)
        goto exitPoint;
    if (argc > 2) {
        writeDescCnt = ParseSelectFileList (interp, argv [2], &writeFdSet, 
                                            &writeDescList, &maxFileId);
        if (writeDescCnt < 0)
            goto exitPoint;
    }
    if (argc > 3) {
        exceptDescCnt = ParseSelectFileList (interp, argv [3], &exceptFdSet, 
                                             &exceptDescList, &maxFileId);
        if (exceptDescCnt < 0)
            goto exitPoint;
    }
    
    /*
     * Get the time out.  Zero is different that not specified.
     */
    timeoutRecPtr = NULL;
    if ((argc > 4) && (argv [4][0] != '\0')) {
        double  timeout, seconds, microseconds;

        if (Tcl_GetDouble (interp, argv [4], &timeout) != TCL_OK)
            goto exitPoint;
        if (timeout < 0) {
            Tcl_AppendResult (interp, "timeout must be greater than or equal",
                              " to zero", (char *) NULL);
            goto exitPoint;
        }
        seconds = floor (timeout);
        microseconds = (timeout - seconds) * 1000000.0;
        timeoutRec.tv_sec = seconds;
        timeoutRec.tv_usec = microseconds;
        timeoutRecPtr = &timeoutRec;
    }

    /*
     * Check if any data is pending in the read stdio buffers.  If there is,
     * then do the select, but don't block in it.
     */

    pending = FindPendingData (readDescCnt, readDescList, &readFdSet2);
    if (pending) {
        timeoutRec.tv_sec = 0;
        timeoutRec.tv_usec = 0;
        timeoutRecPtr = &timeoutRec;
    }

    /*
     * All set, do the select.
     */
    numSelected = select (maxFileId + 1, &readFdSet, &writeFdSet, &exceptFdSet,
                          timeoutRecPtr);
    if (numSelected < 0) {
        interp->result = Tcl_UnixError (interp);
        goto exitPoint;
    }

    /*
     * Return the result, either a 3 element list, or leave the result
     * empty if the timeout occured.
     */
    if (numSelected > 0) {
        retListArgv [0] = ReturnSelectedFileList (&readFdSet,
                                                  &readFdSet2,
                                                  readDescCnt,
                                                  readDescList);
        retListArgv [1] = ReturnSelectedFileList (&writeFdSet,
                                                  NULL,
                                                  writeDescCnt, 
                                                  writeDescList);
        retListArgv [2] = ReturnSelectedFileList (&exceptFdSet,
                                                  NULL,
                                                  exceptDescCnt, 
                                                  exceptDescList);
        Tcl_SetResult (interp, Tcl_Merge (3, retListArgv), TCL_DYNAMIC); 
        ckfree ((char *) retListArgv [0]);
        ckfree ((char *) retListArgv [1]);
        ckfree ((char *) retListArgv [2]);
    }

    result = TCL_OK;

exitPoint:
    if (readDescList != NULL)
        ckfree ((char *) readDescList);
    if (writeDescList != NULL)
        ckfree ((char *) writeDescList);
    if (exceptDescList != NULL)
        ckfree ((char *) exceptDescList);
    return result;

}
#else
/*
 *-----------------------------------------------------------------------------
 *
 * Tcl_SelectCmd --
 *     Dummy select command that returns an error for systems that don't
 *     have select.
 *-----------------------------------------------------------------------------
 */
int
Tcl_SelectCmd (clientData, interp, argc, argv)
    ClientData  clientData;
    Tcl_Interp *interp;
    int         argc;
    char      **argv;
{
    Tcl_AppendResult (interp, 
                      "select is not available on this version of Unix",
                      (char *) NULL);
    return TCL_ERROR;
}
#endif
