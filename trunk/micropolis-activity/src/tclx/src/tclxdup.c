/*
 * tclXdup.c
 *
 * Extended Tcl dup command.
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
 * $Id: tclXdup.c,v 2.0 1992/10/16 04:50:36 markd Rel $
 *-----------------------------------------------------------------------------
 */

#include "tclxint.h"

/*
 * Prototypes of internal functions.
 */
static OpenFile *
DoNormalDup _ANSI_ARGS_((Tcl_Interp *interp,
                         OpenFile   *oldFilePtr));

static OpenFile *
DoSpecialDup _ANSI_ARGS_((Tcl_Interp *interp,
                          OpenFile   *oldFilePtr,
                          char       *newHandleName));


/*
 *-----------------------------------------------------------------------------
 *
 * DoNormalDup --
 *   Process a normal dup command (i.e. the new file is not specified).
 *
 * Parameters:
 *   o interp (I) - If an error occures, the error message is in result,
 *     otherwise the file handle is in result.
 *   o oldFilePtr (I) - Tcl file control block for the file to dup.
 * Returns:
 *   A pointer to the open file structure for the new file, or NULL if an
 * error occured.
 *-----------------------------------------------------------------------------
 */
static OpenFile *
DoNormalDup (interp, oldFilePtr)
    Tcl_Interp *interp;
    OpenFile   *oldFilePtr;
{
    Interp   *iPtr = (Interp *) interp;
    int       newFileId;
    FILE     *newFileCbPtr;
    char     *mode;

    newFileId = dup (fileno (oldFilePtr->f));
    if (newFileId < 0)
        goto unixError;

    if (Tcl_SetupFileEntry (interp, newFileId,
                            oldFilePtr->readable,
                            oldFilePtr->writable) != TCL_OK)
        return NULL;

    sprintf (interp->result, "file%d", newFileId);
    return iPtr->filePtrArray [newFileId];

unixError:
    interp->result = Tcl_UnixError (interp);
    return NULL;;
}

/*
 *-----------------------------------------------------------------------------
 *
 * DoSpecialDup --
 *   Process a special dup command.  This is the case were the file is
 *   dup-ed to stdin, stdout or stderr.  The new file may or be open or
 *   closed
 * Parameters:
 *   o interp (I) - If an error occures, the error message is in result,
 *     otherwise nothing is returned.
 *   o oldFilePtr (I) - Tcl file control block for the file to dup.
 *   o newFileHandle (I) - The handle name for the new file.
 * Returns:
 *   A pointer to the open file structure for the new file, or NULL if an
 * error occured.
 *-----------------------------------------------------------------------------
 */
static OpenFile *
DoSpecialDup (interp, oldFilePtr, newHandleName)
    Tcl_Interp *interp;
    OpenFile   *oldFilePtr;
    char       *newHandleName;
{
    Interp   *iPtr = (Interp *) interp;
    int       newFileId;
    FILE     *newFileCbPtr;
    OpenFile *newFilePtr;

    /*
     * Duplicate the old file to the specified file id.
     */
    newFileId = Tcl_ConvertFileHandle (interp, newHandleName);
    if (newFileId < 0)
        return NULL;
    if (newFileId > 2) {
        Tcl_AppendResult (interp, "target handle must be one of stdin, ",
                          "stdout, stderr, file0, file1, or file2: got \"",
                          newHandleName, "\"", (char *) NULL);
        return NULL;
    }
    switch (newFileId) {
        case 0: 
            newFileCbPtr = stdin;
            break;
        case 1: 
            newFileCbPtr = stdout;
            break;
        case 2: 
            newFileCbPtr = stderr;
            break;
    }

    /*
     * If the specified id is not open, set up a stdio file descriptor.
     */
    TclMakeFileTable (iPtr, newFileId);
    if (iPtr->filePtrArray [newFileId] == NULL) {
        char *mode;

        /*
         * Set up a stdio FILE control block for the new file.
         */
        if (oldFilePtr->readable && oldFilePtr->writable) {
            mode = "r+";
        } else if (oldFilePtr->writable) {
            mode = "w";
        } else {
            mode = "r";
        }
        if (freopen ("/dev/null", mode, newFileCbPtr) == NULL)
            goto unixError;
    }
    
    /*
     * This functionallity may be obtained with dup2 on most systems.  Being
     * open is optional.
     */
    close (newFileId);
#ifndef MSDOS
    if (fcntl (fileno (oldFilePtr->f), F_DUPFD, newFileId) < 0)
        goto unixError;
#endif
    /*
     * Set up a Tcl OpenFile structure for the new file handle.
     */
    newFilePtr = iPtr->filePtrArray [fileno (newFileCbPtr)];
    if (newFilePtr == NULL) {
        newFilePtr = (OpenFile*) ckalloc (sizeof (OpenFile));
        iPtr->filePtrArray [fileno (newFileCbPtr)] = newFilePtr;
    }
    newFilePtr->f        = newFileCbPtr;
    newFilePtr->f2       = NULL;
    newFilePtr->readable = oldFilePtr->readable;
    newFilePtr->writable = oldFilePtr->writable;
    newFilePtr->numPids  = 0;
    newFilePtr->pidPtr   = NULL;
    newFilePtr->errorId  = -1;

    return newFilePtr;

unixError:
    iPtr->result = Tcl_UnixError (interp);
    return NULL;
}

/*
 *-----------------------------------------------------------------------------
 *
 * Tcl_DupCmd --
 *     Implements the dup TCL command:
 *         dup filehandle [stdhandle]
 *
 * Results:
 *      Returns TCL_OK and interp->result containing a filehandle
 *      if the requested file or pipe was successfully duplicated.
 *
 *      Return TCL_ERROR and interp->result containing an
 *      explanation of what went wrong if an error occured.
 *
 * Side effects:
 *      Locates and creates an entry in the handles table
 *
 *-----------------------------------------------------------------------------
 */
int
Tcl_DupCmd (clientData, interp, argc, argv)
    ClientData  clientData;
    Tcl_Interp *interp;
    int         argc;
    char      **argv;
{
    OpenFile *oldFilePtr, *newFilePtr;
    long      seekOffset = -1;

    if ((argc < 2) || (argc > 3)) {
        Tcl_AppendResult (interp, tclXWrongArgs, argv[0], 
                          " filehandle [stdhandle]", (char *) NULL);
        return TCL_ERROR;
    }

    if (TclGetOpenFile(interp, argv[1], &oldFilePtr) != TCL_OK)
	return TCL_ERROR;
    if (oldFilePtr->numPids > 0) { /*??????*/
        Tcl_AppendResult (interp, "can not `dup' a pipeline", (char *) NULL);
        return TCL_ERROR;
    }

    /*
     * If writable, flush out the buffer.  If readable, remember were we are
     * so the we can set it up for the next stdio read to come from the same
     * place.  The location is only recorded if the file is a reqular file,
     * since you cann't seek on other types of files.
     */
    if (oldFilePtr->writable) {
        if (fflush (oldFilePtr->f) != 0)
            goto unixError;
    }
    if (oldFilePtr->readable) {
        struct stat statBuf;
        
        if (fstat (fileno (oldFilePtr->f), &statBuf) < 0)
            goto unixError;
        if ((statBuf.st_mode & S_IFMT) == S_IFREG) {
            seekOffset = ftell (oldFilePtr->f);
            if (seekOffset < 0)
                goto unixError;
        }
    }

    /*
     * Process the dup depending on if dup-ing to a new file or a target
     * file handle.
     */
    if (argc == 2)
        newFilePtr = DoNormalDup (interp, oldFilePtr);
    else
        newFilePtr = DoSpecialDup (interp, oldFilePtr, argv [2]);

    if (newFilePtr == NULL)
        return TCL_ERROR;

    if (seekOffset >= 0) {
        if (fseek (newFilePtr->f, seekOffset, SEEK_SET) != 0)
            goto unixError;
    }
    return TCL_OK;

unixError:
    Tcl_ResetResult (interp);
    interp->result = Tcl_UnixError (interp);
    return TCL_ERROR;
}
