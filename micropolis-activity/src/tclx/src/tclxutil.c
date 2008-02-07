/*
 * tclXutil.c
 *
 * Utility functions for Extended Tcl.
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
 * $Id: tclXutil.c,v 2.0 1992/10/16 04:51:21 markd Rel $
 *-----------------------------------------------------------------------------
 */

#include "tclxint.h"

#ifndef _tolower
#  define _tolower tolower
#  define _toupper toupper
#endif

/*
 * Used to return argument messages by most commands.
 */
char *tclXWrongArgs = "wrong # args: ";

extern double pow ();


/*
 *-----------------------------------------------------------------------------
 *
 * Tcl_StrToLong --
 *      Convert an Ascii string to an long number of the specified base.
 *
 * Parameters:
 *   o string (I) - String containing a number.
 *   o base (I) - The base to use for the number 8, 10 or 16 or zero to decide
 *     based on the leading characters of the number.  Zero to let the number
 *     determine the base.
 *   o longPtr (O) - Place to return the converted number.  Will be 
 *     unchanged if there is an error.
 *
 * Returns:
 *      Returns 1 if the string was a valid number, 0 invalid.
 *-----------------------------------------------------------------------------
 */
int
Tcl_StrToLong (string, base, longPtr)
    CONST char *string;
    int         base;
    long       *longPtr;
{
    char *end;
    long  num;

    num = strtol(string, &end, base);
    while ((*end != '\0') && isspace(*end)) {
        end++;
    }
    if ((end == string) || (*end != 0))
        return FALSE;
    *longPtr = num;
    return TRUE;

} /* Tcl_StrToLong */

/*
 *-----------------------------------------------------------------------------
 *
 * Tcl_StrToInt --
 *      Convert an Ascii string to an number of the specified base.
 *
 * Parameters:
 *   o string (I) - String containing a number.
 *   o base (I) - The base to use for the number 8, 10 or 16 or zero to decide
 *     based on the leading characters of the number.  Zero to let the number
 *     determine the base.
 *   o intPtr (O) - Place to return the converted number.  Will be 
 *     unchanged if there is an error.
 *
 * Returns:
 *      Returns 1 if the string was a valid number, 0 invalid.
 *-----------------------------------------------------------------------------
 */
int
Tcl_StrToInt (string, base, intPtr)
    CONST char *string;
    int         base;
    int        *intPtr;
{
    char *end;
    int   num;

    num = strtol(string, &end, base);
    while ((*end != '\0') && isspace(*end)) {
        end++;
    }
    if ((end == string) || (*end != 0))
        return FALSE;
    *intPtr = num;
    return TRUE;

} /* Tcl_StrToInt */

/*
 *-----------------------------------------------------------------------------
 *
 * Tcl_StrToUnsigned --
 *      Convert an Ascii string to an unsigned int of the specified base.
 *
 * Parameters:
 *   o string (I) - String containing a number.
 *   o base (I) - The base to use for the number 8, 10 or 16 or zero to decide
 *     based on the leading characters of the number.  Zero to let the number
 *     determine the base.
 *   o unsignedPtr (O) - Place to return the converted number.  Will be 
 *     unchanged if there is an error.
 *
 * Returns:
 *      Returns 1 if the string was a valid number, 0 invalid.
 *-----------------------------------------------------------------------------
 */
int
Tcl_StrToUnsigned (string, base, unsignedPtr)
    CONST char *string;
    int         base;
    unsigned   *unsignedPtr;
{
    char          *end;
    unsigned long  num;

    num = strtoul (string, &end, base);
    while ((*end != '\0') && isspace(*end)) {
        end++;
    }
    if ((end == string) || (*end != 0))
        return FALSE;
    *unsignedPtr = num;
    return TRUE;

} /* Tcl_StrToUnsigned */

/*
 *-----------------------------------------------------------------------------
 *
 * Tcl_StrToDouble --
 *   Convert a string to a double percision floating point number.
 *
 * Parameters:
 *   string (I) - Buffer containing double value to convert.
 *   doublePtr (O) - The convert floating point number.
 * Returns:
 *   TRUE if the number is ok, FALSE if it is illegal.
 *-----------------------------------------------------------------------------
 */
int
Tcl_StrToDouble (string, doublePtr)
    CONST char *string;
    double     *doublePtr;
{
    char   *end;
    double  num;

    num = strtod (string, &end);
    while ((*end != '\0') && isspace(*end)) {
        end++;
    }
    if ((end == string) || (*end != 0))
        return FALSE;

    *doublePtr = num;
    return TRUE;

} /* Tcl_StrToDouble */

/*
 *-----------------------------------------------------------------------------
 *
 * Tcl_DownShift --
 *     Utility procedure to down-shift a string.  It is written in such
 *     a way as that the target string maybe the same as the source string.
 *
 * Parameters:
 *   o targetStr (I) - String to store the down-shifted string in.  Must
 *     have enough space allocated to store the string.  If NULL is specified,
 *     then the string will be dynamicly allocated and returned as the
 *     result of the function. May also be the same as the source string to
 *     shift in place.
 *   o sourceStr (I) - The string to down-shift.
 *
 * Returns:
 *   A pointer to the down-shifted string
 *-----------------------------------------------------------------------------
 */
char *
Tcl_DownShift (targetStr, sourceStr)
    char       *targetStr;
    CONST char *sourceStr;
{
    register char theChar;

    if (targetStr == NULL)
        targetStr = ckalloc (strlen ((char *) sourceStr) + 1);

    for (; (theChar = *sourceStr) != '\0'; sourceStr++) {
        if (isupper (theChar))
            theChar = _tolower (theChar);
        *targetStr++ = theChar;
    }
    *targetStr = '\0';
    return targetStr;
}

/*
 *-----------------------------------------------------------------------------
 *
 * Tcl_UpShift --
 *     Utility procedure to up-shift a string.
 *
 * Parameters:
 *   o targetStr (I) - String to store the up-shifted string in.  Must
 *     have enough space allocated to store the string.  If NULL is specified,
 *     then the string will be dynamicly allocated and returned as the
 *     result of the function. May also be the same as the source string to
 *     shift in place.
 *   o sourceStr (I) - The string to up-shift.
 *
 * Returns:
 *   A pointer to the up-shifted string
 *-----------------------------------------------------------------------------
 */
char *
Tcl_UpShift (targetStr, sourceStr)
    char       *targetStr;
    CONST char *sourceStr;
{
    register char theChar;

    if (targetStr == NULL)
        targetStr = ckalloc (strlen ((char *) sourceStr) + 1);

    for (; (theChar = *sourceStr) != '\0'; sourceStr++) {
        if (islower (theChar))
            theChar = _toupper (theChar);
        *targetStr++ = theChar;
    }
    *targetStr = '\0';
    return targetStr;
}

/*
 *-----------------------------------------------------------------------------
 *
 * Tcl_ExpandDynBuf --
 *
 *    Expand a dynamic buffer so that it will have room to hold the 
 *    specified additional space.  If `appendSize' is zero, the buffer
 *    size will just be doubled.
 *
 *-----------------------------------------------------------------------------
 */
void
Tcl_ExpandDynBuf (dynBufPtr, appendSize)
    dynamicBuf_t *dynBufPtr;
    int           appendSize;
{
    int   newSize, minSize;
    char *oldBufPtr;

    newSize = dynBufPtr->size * 2;
    minSize = dynBufPtr->len + 1 + appendSize;
    if (newSize < minSize)
        newSize = minSize;

    oldBufPtr = dynBufPtr->ptr;
    dynBufPtr->ptr = ckalloc (newSize);
    memcpy (dynBufPtr->ptr, oldBufPtr, dynBufPtr->len + 1);
    if (oldBufPtr != dynBufPtr->buf)
        ckfree ((char *) oldBufPtr);
    dynBufPtr->size = newSize;
}

/*
 *-----------------------------------------------------------------------------
 *
 * Tcl_DynBufInit --
 *
 *    Initializes a dynamic buffer.
 *
 *-----------------------------------------------------------------------------
 */
void
Tcl_DynBufInit (dynBufPtr)
    dynamicBuf_t *dynBufPtr;
{
    dynBufPtr->buf [0] = '\0';
    dynBufPtr->ptr = dynBufPtr->buf;
    dynBufPtr->size = INIT_DYN_BUFFER_SIZE;
    dynBufPtr->len = 0;
}

/*
 *-----------------------------------------------------------------------------
 *
 * Tcl_DynBufFree --
 *
 *    Clean up a dynamic buffer, release space if it was dynamicly
 * allocated.
 *
 *-----------------------------------------------------------------------------
 */
void
Tcl_DynBufFree (dynBufPtr)
    dynamicBuf_t *dynBufPtr;
{
    if (dynBufPtr->ptr != dynBufPtr->buf)
        ckfree (dynBufPtr->ptr);
}

/*
 *-----------------------------------------------------------------------------
 *
 * Tcl_DynBufReturn --
 *
 *    Return the contents of the dynamic buffer as an interpreter result.
 * Don't call DynBufFree after calling this procedure.  The dynamic buffer
 * must be re-initialized to reuse it.
 *
 *-----------------------------------------------------------------------------
 */
void
Tcl_DynBufReturn (interp, dynBufPtr)
    Tcl_Interp    *interp;
    dynamicBuf_t *dynBufPtr;
{
    if (dynBufPtr->ptr != dynBufPtr->buf)
        Tcl_SetResult (interp, dynBufPtr->ptr, TCL_DYNAMIC);
    else
        Tcl_SetResult (interp, dynBufPtr->ptr, TCL_VOLATILE);
}

/*
 *-----------------------------------------------------------------------------
 *
 * Tcl_DynBufAppend --
 *
 *    Append the specified string to the dynamic buffer, expanding if
 *    necessary. Assumes the string in the buffer is zero terminated.
 *
 *-----------------------------------------------------------------------------
 */
void
Tcl_DynBufAppend (dynBufPtr, newStr)
    dynamicBuf_t *dynBufPtr;
    char         *newStr;
{
    int newLen, currentUsed;

    newLen = strlen (newStr);
    if ((dynBufPtr->len + newLen + 1) > dynBufPtr->size)
        Tcl_ExpandDynBuf (dynBufPtr, newLen);
    strcpy (dynBufPtr->ptr + dynBufPtr->len, newStr);
    dynBufPtr->len += newLen;
}

/*
 *-----------------------------------------------------------------------------
 *
 * Tcl_DynamicFgets --
 *
 *    Reads a line from a file into a dynamic buffer.  The buffer will be
 * expanded, if necessary and reads are done until EOL or EOF is reached.
 * Any data already in the buffer will be overwritten. if append is not
 * specified.  Even if an error or EOF is encountered, the buffer should
 * be cleaned up, as storage may have still been allocated.
 *
 * Results:
 *    If data was transfered, returns 1, if EOF was encountered without
 * transfering any data, returns 0.  If an error occured, returns, -1.
 *
 *-----------------------------------------------------------------------------
 */
int
Tcl_DynamicFgets (dynBufPtr, filePtr, append)
    dynamicBuf_t *dynBufPtr;
    FILE         *filePtr;
    int           append;
{
    int   readVal;

    if (!append)
        dynBufPtr->len = 0;

    while (TRUE) {
        if (dynBufPtr->len + 1 == dynBufPtr->size)
            Tcl_ExpandDynBuf (dynBufPtr, 0);

        readVal = getc (filePtr);
        if (readVal == '\n')      /* Is it a new-line? */
            break;
        if (readVal == EOF) {     /* Is it an EOF or an error? */
            if (feof (filePtr)) {
                break;
            }
            return -1;   /* Error */
        }
        dynBufPtr->ptr [dynBufPtr->len++] = readVal;
    }
    dynBufPtr->ptr [dynBufPtr->len] = '\0';
    return (readVal == EOF) ? 0 : 1;
}

/*
 *-----------------------------------------------------------------------------
 *
 * Tcl_GetLong --
 *
 *      Given a string, produce the corresponding long value.
 *
 * Results:
 *      The return value is normally TCL_OK;  in this case *intPtr
 *      will be set to the integer value equivalent to string.  If
 *      string is improperly formed then TCL_ERROR is returned and
 *      an error message will be left in interp->result.
 *
 * Side effects:
 *      None.
 *
 *-----------------------------------------------------------------------------
 */
int
Tcl_GetLong(interp, string, longPtr)
    Tcl_Interp *interp;         /* Interpreter to use for error reporting. */
    CONST char *string;         /* String containing a (possibly signed)
                                 * integer in a form acceptable to strtol. */
    long       *longPtr;        /* Place to store converted result. */
{
    char *end;
    long  i;

    i = strtol(string, &end, 0);
    while ((*end != '\0') && isspace(*end)) {
        end++;
    }
    if ((end == string) || (*end != 0)) {
        Tcl_AppendResult (interp, "expected integer but got \"", string,
                          "\"", (char *) NULL);
        return TCL_ERROR;
    }
    *longPtr = i;
    return TCL_OK;
}

/*
 *-----------------------------------------------------------------------------
 *
 * Tcl_GetUnsigned --
 *
 *      Given a string, produce the corresponding unsigned integer value.
 *
 * Results:
 *      The return value is normally TCL_OK;  in this case *intPtr
 *      will be set to the integer value equivalent to string.  If
 *      string is improperly formed then TCL_ERROR is returned and
 *      an error message will be left in interp->result.
 *
 * Side effects:
 *      None.
 *
 *-----------------------------------------------------------------------------
 */
int
Tcl_GetUnsigned(interp, string, unsignedPtr)
    Tcl_Interp *interp;         /* Interpreter to use for error reporting. */
    CONST char *string;         /* String containing a (possibly signed)
                                 * integer in a form acceptable to strtoul. */
    unsigned   *unsignedPtr;    /* Place to store converted result. */
{
    char          *end;
    unsigned long  i;

    /*
     * Since some strtoul functions don't detect negative numbers, check
     * in advance.
     */
    while (isspace(*string))
        string++;
    if (string [0] == '-')
        goto badUnsigned;

    i = strtoul(string, &end, 0);
    while ((*end != '\0') && isspace(*end))
        end++;

    if ((end == string) || (*end != '\0'))
        goto badUnsigned;

    *unsignedPtr = i;
    return TCL_OK;

  badUnsigned:
    Tcl_AppendResult (interp, "expected unsigned integer but got \"", 
                      string, "\"", (char *) NULL);
    return TCL_ERROR;
}

/*
 *-----------------------------------------------------------------------------
 *
 * Tcl_ConvertFileHandle --
 *
 * Convert a file handle to its file number. The file handle maybe one 
 * of "stdin", "stdout" or "stderr" or "fileNNN", were NNN is the file
 * number.  If the handle is invalid, -1 is returned and a error message
 * will be returned in interp->result.  This is used when the file may
 * not be currently open.
 *
 *-----------------------------------------------------------------------------
 */
int
Tcl_ConvertFileHandle (interp, handle)
    Tcl_Interp *interp;
    char       *handle;
{
    int fileId = -1;

    if (handle [0] == 's') {
        if (STREQU (handle, "stdin"))
            fileId = 0;
        else if (STREQU (handle, "stdout"))
            fileId = 1;
        else if (STREQU (handle, "stderr"))
            fileId = 2;
    } else {
       if (STRNEQU (handle, "file", 4))
           Tcl_StrToInt (&handle [4], 10, &fileId);
    }
    if (fileId < 0)
        Tcl_AppendResult (interp, "invalid file handle: ", handle,
                          (char *) NULL);
    return fileId;
}

/*
 *-----------------------------------------------------------------------------
 *
 * Tcl_SetupFileEntry --
 *
 * Set up an entry in the Tcl file table for a file number, including the stdio
 * FILE structure.
 *
 * Parameters:
 *   o interp (I) - Current interpreter.
 *   o fileNum (I) - File number to set up the entry for.
 *   o readable (I) - TRUE if read access to the file.
 *   o writable (I) - TRUE if  write access to the file.
 * Returns:
 *   TCL_OK or TCL_ERROR;
 *-----------------------------------------------------------------------------
 */
int
Tcl_SetupFileEntry (interp, fileNum, readable, writable)
    Tcl_Interp *interp;
    int         fileNum;
    int         readable;
    int         writable;
{
    Interp   *iPtr = (Interp *) interp;
    char     *mode;
    FILE     *fileCBPtr;
    OpenFile *filePtr;

    /*
     * Set up a stdio FILE control block for the new file.
     */
    if (readable && writable) {
        mode = "r+";
    } else if (writable) {
        mode = "w";
    } else {
        mode = "r";
    }
    fileCBPtr = fdopen (fileNum, mode);
    if (fileCBPtr == NULL) {
        iPtr->result = Tcl_UnixError (interp);
        return TCL_ERROR;
    }

    /*
     * Put the file in the Tcl table.
     */
    TclMakeFileTable (iPtr, fileNum);
    if (iPtr->filePtrArray [fileno (fileCBPtr)] != NULL)
        panic ("file already open");
    filePtr = (OpenFile *) ckalloc (sizeof (OpenFile));
    iPtr->filePtrArray [fileno (fileCBPtr)] = filePtr;

    filePtr->f        = fileCBPtr;
    filePtr->f2       = NULL;
    filePtr->readable = readable;
    filePtr->writable = writable;
    filePtr->numPids  = 0;
    filePtr->pidPtr   = NULL;
    filePtr->errorId  = -1;

    return TCL_OK;
}

/*
 *-----------------------------------------------------------------------------
 *
 * Tcl_System --
 *     does the equivalent of the Unix "system" library call, but
 *     uses waitpid to wait on the correct process, rather than
 *     waiting on all processes and throwing the exit statii away
 *     for the processes it isn't interested in, plus does it with
 *     a Tcl flavor
 *
 * Results:
 *  Standard TCL results, may return the UNIX system error message.
 *
 *-----------------------------------------------------------------------------
 */
int 
Tcl_System (interp, command)
    Tcl_Interp *interp;
    char       *command;
{
    int processID, waitStatus, processStatus;

    if ((processID = Tcl_Fork()) < 0) {
        interp->result = Tcl_UnixError (interp);
        return -1;
    }
    if (processID == 0) {
        if (execl ("/bin/sh", "sh", "-c", command, (char *) NULL) < 0) {
            interp->result = Tcl_UnixError (interp);
            return -1;
        }
        exit(256);
    }

    /*
     * Parent process.
     */
#ifndef TCL_HAVE_WAITPID
    if (Tcl_WaitPids(1, &processID, &processStatus) == -1) {
        interp->result = Tcl_UnixError (interp);
        return -1;
    }
#else
    if (waitpid (processID, &processStatus, 0) == -1) {
        interp->result = Tcl_UnixError (interp);
        return -1;
    }
#endif
    return (WEXITSTATUS(processStatus));

}

/*
 *--------------------------------------------------------------
 *
 * Tcl_ReturnDouble --
 *
 *	Format a double to the maximum precision supported on
 *	this machine.  If the number formats to an even integer,
 *	a ".0" is append to assure that the value continues to
 *	represent a floating point number.
 *
 * Results:
 *	A standard Tcl result.	If the result is TCL_OK, then the
 *	interpreter's result is set to the string value of the
 *	double.	 If the result is TCL_OK, then interp->result
 *	contains an error message (If the number had the value of
 *	"not a number" or "infinite").
 *
 * Side effects:
 *	None.
 *
 *--------------------------------------------------------------
 */

int
Tcl_ReturnDouble(interp, number)
    Tcl_Interp *interp;			/* ->result gets converted number */
    double number;			/* Number to convert */
{
    static int precision = 0;
    register char *scanPtr;

    /*
     * On the first call, determine the number of decimal digits that represent
     * the precision of a double.
     */
    if (precision == 0) {
#ifdef IS_LINUX
	precision = 8;
#else
	sprintf (interp->result, "%.0f", pow (2.0, (double) DSIGNIF));
	precision = strlen (interp->result);
#endif
    }

    sprintf (interp->result, "%.*g", precision, number);

    /*
     * Scan the number for "." or "e" to assure that the number has not been
     * converted to an integer.	 Also check for NaN on infinite
     */

    scanPtr = interp->result;
    if (scanPtr [0] == '-')
	scanPtr++;
    for (; isdigit (*scanPtr); scanPtr++)
	continue;

    switch (*scanPtr) {
      case '.':
      case 'e':
	return TCL_OK;
      case 'n':
      case 'N':
	interp->result = "Floating point error, result is not a number";
	return TCL_ERROR;
      case 'i':
      case 'I':
	interp->result = "Floating point error, result is infinite";
	return TCL_ERROR;
      case '\0':
	scanPtr [0] = '.';
	scanPtr [1] = '0';
	scanPtr [2] = '\0';
	return TCL_OK;
    }

    /*
     * If we made it here, this sprintf returned something we did not expect.
     */
    Tcl_AppendResult (interp, ": unexpected floating point conversion result",
		      (char *) NULL);
    return TCL_ERROR;
}
     
