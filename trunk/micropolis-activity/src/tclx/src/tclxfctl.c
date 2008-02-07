/*
 * tclXfcntl.c
 *
 * Extended Tcl fcntl command.
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
 * $Id: tclXfcntl.c,v 2.0 1992/10/16 04:50:38 markd Rel $
 *-----------------------------------------------------------------------------
 */

#include "tclxint.h"

/*
 * Macro to enable line buffering mode on a file.  Macros assure that the
 * resulting expression returns zero if the function call does not return
 * a value.
 */
#ifdef TCL_HAVE_SETLINEBUF
#   define SET_LINE_BUF(fp)  (setlinebuf (fp),0)
#else
#   define SET_LINE_BUF(fp)  setvbuf (fp, NULL, _IOLBF, BUFSIZ)
#endif

/*
 * If we don't have O_NONBLOCK, use O_NDELAY.
 */
#ifndef O_NONBLOCK
#   define O_NONBLOCK O_NDELAY
#endif

/*
 * Attributes used by fcntl command and the maximum length of any attribute
 * name.
 */
#define   ATTR_CLOEXEC  1
#define   ATTR_NOBUF    2
#define   ATTR_LINEBUF  4
#define   MAX_ATTR_NAME_LEN  20

/*
 * Prototypes of internal functions.
 */
static int
XlateFcntlAttr  _ANSI_ARGS_((Tcl_Interp *interp,
                             char       *attrName,
                             int        *fcntlAttrPtr,
                             int        *otherAttrPtr));

static int
GetFcntlAttr _ANSI_ARGS_((Tcl_Interp *interp,
                          OpenFile   *filePtr,
                          char       *attrName));

static int
SetFcntlAttr _ANSI_ARGS_((Tcl_Interp *interp,
                          OpenFile   *filePtr,
                          char       *attrName,
                          char       *valueStr));

/*
 *-----------------------------------------------------------------------------
 *
 * XlateFcntlAttr --
 *    Translate an fcntl attribute.
 *
 * Parameters:
 *   o interp (I) - Tcl interpreter.
 *   o attrName (I) - The attrbute name to translate, maybe upper or lower
 *     case.
 *   o fcntlAttrPtr (O) - If the attr specified is one of the standard
 *     fcntl attrs, it is returned here, otherwise zero is returned.
 *   o otherAttrPtr (O) - If the attr specified is one of the additional
 *     attrs supported by the Tcl command, it is returned here, otherwise
 *     zero is returned.
 * Result:
 *   Returns TCL_OK if all is well, TCL_ERROR if there is an error.
 *-----------------------------------------------------------------------------
 */
static int
XlateFcntlAttr (interp, attrName, fcntlAttrPtr, otherAttrPtr)
    Tcl_Interp *interp;
    char       *attrName;
    int        *fcntlAttrPtr;
    int        *otherAttrPtr;
{
    char attrNameUp [MAX_ATTR_NAME_LEN];

    *fcntlAttrPtr = 0;
    *otherAttrPtr = 0;

    if (strlen (attrName) >= MAX_ATTR_NAME_LEN)
        goto invalidAttrName;

    Tcl_UpShift (attrNameUp, attrName);

    if (STREQU (attrNameUp, "RDONLY")) {
        *fcntlAttrPtr = O_RDONLY;
        return TCL_OK;
    }
    if (STREQU (attrNameUp, "WRONLY")) {
        *fcntlAttrPtr = O_WRONLY;
        return TCL_OK;
    }
    if (STREQU (attrNameUp, "RDWR")) {
        *fcntlAttrPtr = O_RDWR;
        return TCL_OK;
    }
    if (STREQU (attrNameUp, "READ")) {
        *fcntlAttrPtr = O_RDONLY | O_RDWR;
        return TCL_OK;
    }
    if (STREQU (attrNameUp, "WRITE")) {
        *fcntlAttrPtr = O_WRONLY | O_RDWR;
        return TCL_OK;
    }
    if (STREQU (attrNameUp, "NONBLOCK")) {
        *fcntlAttrPtr = O_NONBLOCK;
        return TCL_OK;
    }
    if (STREQU (attrNameUp, "APPEND")) {
        *fcntlAttrPtr = O_APPEND;
        return TCL_OK;
    }
    if (STREQU (attrNameUp, "CLOEXEC")) {
        *otherAttrPtr = ATTR_CLOEXEC;
        return TCL_OK;
    }
    if (STREQU (attrNameUp, "NOBUF")) {
        *otherAttrPtr = ATTR_NOBUF;
        return TCL_OK;
    }
    if (STREQU (attrNameUp, "LINEBUF")) {
        *otherAttrPtr = ATTR_LINEBUF;
        return TCL_OK;
    }

    /*
     * Error return code.
     */
  invalidAttrName:
    Tcl_AppendResult (interp, "unknown attribute name \"", attrName,
                      "\", expected one of APPEND, CLOEXEC, LINEBUF, ",
                      "NONBLOCK, NOBUF, READ, RDONLY, RDWR, WRITE, WRONLY",
                      (char *) NULL);
    return TCL_ERROR;

}

/*
 *-----------------------------------------------------------------------------
 *
 * GetFcntlAttr --
 *    Return the value of a specified fcntl attribute.
 *
 * Parameters:
 *   o interp (I) - Tcl interpreter, value is returned in the result
 *   o filePtr (I) - Pointer to the file descriptor.
 *   o attrName (I) - The attrbute name to translate, maybe upper or lower
 *     case.
 * Result:
 *   Returns TCL_OK if all is well, TCL_ERROR if fcntl returns an error.
 *-----------------------------------------------------------------------------
 */
static int
GetFcntlAttr (interp, filePtr, attrName)
    Tcl_Interp *interp;
    OpenFile   *filePtr;
    char       *attrName;
{
    int fcntlAttr, otherAttr, current;

    if (XlateFcntlAttr (interp, attrName, &fcntlAttr, &otherAttr) != TCL_OK)
        return TCL_ERROR;

    if (fcntlAttr != 0) {
        current = fcntl (fileno (filePtr->f), F_GETFL, 0);
        if (current == -1)
            goto unixError;
        interp->result = (current & fcntlAttr) ? "1" : "0";
        return TCL_OK;
    }
    
    if (otherAttr & ATTR_CLOEXEC) {
        current = fcntl (fileno (filePtr->f), F_GETFD, 0);
        if (current == -1)
            goto unixError;
        interp->result = (current & 1) ? "1" : "0";
        return TCL_OK;
    }

    /*
     * Poke the stdio FILE structure to determine the buffering status.
     */

#ifndef IS_LINUX

#ifdef _IONBF
    if (otherAttr & ATTR_NOBUF) {
        interp->result = (filePtr->f->_flag & _IONBF) ? "1" : "0";
        return TCL_OK;
    }
    if (otherAttr & ATTR_LINEBUF) {
        interp->result = (filePtr->f->_flag & _IOLBF) ? "1" : "0";
        return TCL_OK;
    }
#else
    if (otherAttr & ATTR_NOBUF) {
        interp->result = (filePtr->f->_flags & _SNBF) ? "1" : "0";
        return TCL_OK;
    }
    if (otherAttr & ATTR_LINEBUF) {
        interp->result = (filePtr->f->_flags & _SLBF) ? "1" : "0";
        return TCL_OK;
    }
#endif

#endif

unixError:
    interp->result = Tcl_UnixError (interp);
    return TCL_ERROR;
}

/*
 *-----------------------------------------------------------------------------
 *
 * SetFcntlAttr --
 *    Set the specified fcntl attr to the given value.
 *
 * Parameters:
 *   o interp (I) - Tcl interpreter, value is returned in the result
 *   o filePtr (I) - Pointer to the file descriptor.
 *   o attrName (I) - The attrbute name to translate, maybe upper or lower
 *     case.
 *   o valueStr (I) - The string value to set the attribiute to.
 *
 * Result:
 *   Returns TCL_OK if all is well, TCL_ERROR if there is an error.
 *-----------------------------------------------------------------------------
 */
static int
SetFcntlAttr (interp, filePtr, attrName, valueStr)
    Tcl_Interp *interp;
    OpenFile   *filePtr;
    char       *attrName;
    char       *valueStr;
{

    int fcntlAttr, otherAttr, current, setValue;
 
    if (Tcl_GetBoolean (interp, valueStr, &setValue) != TCL_OK)
        return TCL_ERROR;

    if (XlateFcntlAttr (interp, attrName, &fcntlAttr, &otherAttr) != TCL_OK)
        return TCL_ERROR;

    /*
     * Validate that this the attribute may be set (or cleared).
     */

    if (fcntlAttr & (O_RDONLY | O_WRONLY | O_RDWR)) {
        Tcl_AppendResult (interp, "Attribute \"", attrName, "\" may not be ",
                          "altered after open", (char *) NULL);
        return TCL_ERROR;
    }

    if ((otherAttr & (ATTR_NOBUF | ATTR_LINEBUF)) && !setValue) {
        Tcl_AppendResult (interp, "Attribute \"", attrName, "\" may not be ",
                          "cleared once set", (char *) NULL);
        return TCL_ERROR;
    }

    if (otherAttr == ATTR_CLOEXEC) {
        if (fcntl (fileno (filePtr->f), F_SETFD, setValue) == -1)
            goto unixError;
        return TCL_OK;
    }

    if (otherAttr == ATTR_NOBUF) {
        setbuf (filePtr->f, NULL);
        return TCL_OK;
    }

    if (otherAttr == ATTR_LINEBUF) {
        if (SET_LINE_BUF (filePtr->f) != 0)
            goto unixError;
        return TCL_OK;
    }

    /*
     * Handle standard fcntl attrs.
     */
       
    current = fcntl (fileno (filePtr->f), F_GETFL, 0);
    if (current == -1)
        goto unixError;
    current &= ~fcntlAttr;
    if (setValue)
        current |= fcntlAttr;
    if (fcntl (fileno (filePtr->f), F_SETFL, current) == -1)
        goto unixError;

    return TCL_OK;

  unixError:
    interp->result = Tcl_UnixError (interp);
    return TCL_ERROR;
   
}

/*
 *-----------------------------------------------------------------------------
 *
 * Tcl_FcntlCmd --
 *     Implements the fcntl TCL command:
 *         fcntl handle [attribute value]
 *-----------------------------------------------------------------------------
 */
int
Tcl_FcntlCmd (clientData, interp, argc, argv)
    ClientData  clientData;
    Tcl_Interp *interp;
    int         argc;
    char      **argv;
{
    OpenFile    *filePtr;

    if ((argc < 3) || (argc > 4)) {
        Tcl_AppendResult (interp, tclXWrongArgs, argv [0], 
                          " handle attribute [value]", (char *) NULL);
        return TCL_ERROR;
    }

    if (TclGetOpenFile (interp, argv[1], &filePtr) != TCL_OK)
	return TCL_ERROR;
    if (argc == 3) {    
        if (GetFcntlAttr (interp, filePtr, argv [2]) != TCL_OK)
            return TCL_ERROR;
    } else {
        if (SetFcntlAttr (interp, filePtr, argv [2], argv [3]) != TCL_OK)
            return TCL_ERROR;
    }
    return TCL_OK;
}
