/*
 * tclXfilecmds.c
 *
 * Extended Tcl pipe, copyfile and fstat commands.
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
 * $Id: tclXfilecmds.c,v 2.0 1992/10/16 04:50:41 markd Rel $
 *-----------------------------------------------------------------------------
 */

#include "tclxint.h"

/*
 * Prototypes of internal functions.
 */
static char *
GetFileType _ANSI_ARGS_((struct stat  *statBufPtr));

static void
ReturnStatList _ANSI_ARGS_((Tcl_Interp   *interp,
                            OpenFile     *filePtr,
                            struct stat  *statBufPtr));

static int
ReturnStatArray _ANSI_ARGS_((Tcl_Interp   *interp,
                             OpenFile     *filePtr,
                             struct stat  *statBufPtr,
                             char         *arrayName));

static int
ReturnStatItem _ANSI_ARGS_((Tcl_Interp   *interp,
                            OpenFile     *filePtr,
                            struct stat  *statBufPtr,
                            char         *itemName));

static int
ParseLockUnlockArgs _ANSI_ARGS_((Tcl_Interp    *interp,
                                 int            argc,
                                 char         **argv,
                                 int            argIdx,
                                 OpenFile     **filePtrPtr,
                                 struct flock  *lockInfoPtr));


/*
 *-----------------------------------------------------------------------------
 *
 * Tcl_PipeCmd --
 *     Implements the pipe TCL command:
 *         pipe [handle_var_r handle_var_w]
 *
 * Results:
 *      Standard TCL result.
 *
 * Side effects:
 *      Locates and creates entries in the handles table
 *
 *-----------------------------------------------------------------------------
 */
int
Tcl_PipeCmd (clientData, interp, argc, argv)
    ClientData  clientData;
    Tcl_Interp *interp;
    int         argc;
    char      **argv;
{
    Interp    *iPtr = (Interp *) interp;
    int        fileIds [2];
    char       fHandle [12];

    if (!((argc == 1) || (argc == 3))) {
        Tcl_AppendResult (interp, tclXWrongArgs, argv[0], 
                          " [handle_var_r handle_var_w]", (char*) NULL);
        return TCL_ERROR;
    }

    if (pipe (fileIds) < 0) {
        interp->result = Tcl_UnixError (interp);
        return TCL_ERROR;
    }

    if (Tcl_SetupFileEntry (interp, fileIds [0], TRUE,  FALSE) != TCL_OK)
        goto errorExit;
    if (Tcl_SetupFileEntry (interp, fileIds [1], FALSE, TRUE) != TCL_OK)
        goto errorExit;

    if (argc == 1)      
        sprintf (interp->result, "file%d file%d", fileIds [0], fileIds [1]);
    else {
        sprintf (fHandle, "file%d", fileIds [0]);
        if (Tcl_SetVar (interp, argv[1], fHandle, TCL_LEAVE_ERR_MSG) == NULL)
            goto errorExit;

        sprintf (fHandle, "file%d", fileIds [1]);
        if (Tcl_SetVar (interp, argv[2], fHandle, TCL_LEAVE_ERR_MSG) == NULL)
            goto errorExit;
    }
        
    return TCL_OK;

errorExit:
    close (fileIds [0]);
    close (fileIds [1]);
    return TCL_ERROR;
}

/*
 *-----------------------------------------------------------------------------
 *
 * Tcl_CopyfileCmd --
 *     Implements the copyfile TCL command:
 *         copyfile handle1 handle2 [lines]
 *
 * Results:
 *      Nothing if it worked, else an error.
 *
 *-----------------------------------------------------------------------------
 */
int
Tcl_CopyfileCmd (clientData, interp, argc, argv)
    ClientData  clientData;
    Tcl_Interp *interp;
    int         argc;
    char      **argv;
{
    OpenFile  *fromFilePtr, *toFilePtr;
    char       transferBuffer [2048];
    int        bytesRead;

    if (argc != 3) {
        Tcl_AppendResult (interp, tclXWrongArgs, argv [0], 
                          " fromfilehandle tofilehandle", (char *) NULL);
        return TCL_ERROR;
    }

    if (TclGetOpenFile (interp, argv[1], &fromFilePtr) != TCL_OK)
	return TCL_ERROR;
    if (TclGetOpenFile (interp, argv[2], &toFilePtr) != TCL_OK)
	return TCL_ERROR;

    if (!fromFilePtr->readable) {
        interp->result = "Source file is not open for read access";
	return TCL_ERROR;
    }
    if (!toFilePtr->writable) {
        interp->result = "Target file is not open for write access";
	return TCL_ERROR;
    }

    while (TRUE) {
        bytesRead = fread (transferBuffer, sizeof (char), 
                           sizeof (transferBuffer), fromFilePtr->f);
        if (bytesRead <= 0) {
            if (feof (fromFilePtr->f))
                break;
            else
                goto unixError;
        }
        if (fwrite (transferBuffer, sizeof (char), bytesRead, toFilePtr->f) != 
                    bytesRead)
            goto unixError;
    }

    return TCL_OK;

unixError:
    interp->result = Tcl_UnixError (interp);
    return TCL_ERROR;
}

/*
 *-----------------------------------------------------------------------------
 *
 * GetFileType --
 *
 *   Looks at stat mode and returns a text string indicating what type of
 * file it is.
 *
 * Parameters:
 *   o statBufPtr (I) - Pointer to a buffer initialized by stat or fstat.
 * Returns:
 *   A pointer static text string representing the type of the file.
 *-----------------------------------------------------------------------------
 */
static char *
GetFileType (statBufPtr)
    struct stat  *statBufPtr;
{
    char *typeStr;

    /*
     * Get a string representing the type of the file.
     */
    if (S_ISREG (statBufPtr->st_mode)) {
        typeStr = "file";
    } else if (S_ISDIR (statBufPtr->st_mode)) {
        typeStr = "directory";
    } else if (S_ISCHR (statBufPtr->st_mode)) {
        typeStr = "characterSpecial";
    } else if (S_ISBLK (statBufPtr->st_mode)) {
        typeStr = "blockSpecial";
    } else if (S_ISFIFO (statBufPtr->st_mode)) {
        typeStr = "fifo";
    } else if (S_ISLNK (statBufPtr->st_mode)) {
        typeStr = "link";
    } else if (S_ISSOCK (statBufPtr->st_mode)) {
        typeStr = "socket";
    } else {
        typeStr = "unknown";
    }

    return typeStr;
}

/*
 *-----------------------------------------------------------------------------
 *
 * ReturnStatList --
 *
 *   Return file stat infomation as a keyed list.
 *
 * Parameters:
 *   o interp (I) - The list is returned in result.
 *   o filePtr (I) - Pointer to the Tcl open file structure.
 *   o statBufPtr (I) - Pointer to a buffer initialized by stat or fstat.
 *-----------------------------------------------------------------------------
 */
static void
ReturnStatList (interp, filePtr, statBufPtr)
    Tcl_Interp   *interp;
    OpenFile     *filePtr;
    struct stat  *statBufPtr;
{
    char statList [200];

    sprintf (statList, 
             "{atime %d} {ctime %d} {dev %d} {gid %d} {ino %d} {mode %d} ",
              statBufPtr->st_atime, statBufPtr->st_ctime, statBufPtr->st_dev,
              statBufPtr->st_gid,   statBufPtr->st_ino,   statBufPtr->st_mode);
    Tcl_AppendResult (interp, statList, (char *) NULL);

    sprintf (statList, 
             "{mtime %d} {nlink %d} {size %d} {uid %d} {tty %d} {type %s}",
             statBufPtr->st_mtime,  statBufPtr->st_nlink, statBufPtr->st_size,
             statBufPtr->st_uid,    isatty (fileno (filePtr->f)),
             GetFileType (statBufPtr));
    Tcl_AppendResult (interp, statList, (char *) NULL);

}

/*
 *-----------------------------------------------------------------------------
 *
 * ReturnStatArray --
 *
 *   Return file stat infomation in an array.
 *
 * Parameters:
 *   o interp (I) - Current interpreter, error return in result.
 *   o filePtr (I) - Pointer to the Tcl open file structure.
 *   o statBufPtr (I) - Pointer to a buffer initialized by stat or fstat.
 *   o arrayName (I) - The name of the array to return the info in.
 * Returns:
 *   TCL_OK or TCL_ERROR.
 *-----------------------------------------------------------------------------
 */
static int
ReturnStatArray (interp, filePtr, statBufPtr, arrayName)
    Tcl_Interp   *interp;
    OpenFile     *filePtr;
    struct stat  *statBufPtr;
    char         *arrayName;
{
    char numBuf [30];

    sprintf (numBuf, "%d", statBufPtr->st_dev);
    if  (Tcl_SetVar2 (interp, arrayName, "dev", numBuf, 
                      TCL_LEAVE_ERR_MSG) == NULL)
        return TCL_ERROR;

    sprintf (numBuf, "%d", statBufPtr->st_ino);
    if  (Tcl_SetVar2 (interp, arrayName, "ino", numBuf,
                         TCL_LEAVE_ERR_MSG) == NULL)
        return TCL_ERROR;

    sprintf (numBuf, "%d", statBufPtr->st_mode);
    if  (Tcl_SetVar2 (interp, arrayName, "mode", numBuf, 
                      TCL_LEAVE_ERR_MSG) == NULL)
        return TCL_ERROR;

    sprintf (numBuf, "%d", statBufPtr->st_nlink);
    if  (Tcl_SetVar2 (interp, arrayName, "nlink", numBuf,
                      TCL_LEAVE_ERR_MSG) == NULL)
        return TCL_ERROR;

    sprintf (numBuf, "%d", statBufPtr->st_uid);
    if  (Tcl_SetVar2 (interp, arrayName, "uid", numBuf,
                      TCL_LEAVE_ERR_MSG) == NULL)
        return TCL_ERROR;

    sprintf (numBuf, "%d", statBufPtr->st_gid);
    if  (Tcl_SetVar2 (interp, arrayName, "gid", numBuf,
                      TCL_LEAVE_ERR_MSG) == NULL)
        return TCL_ERROR;

    sprintf (numBuf, "%d", statBufPtr->st_size);
    if  (Tcl_SetVar2 (interp, arrayName, "size", numBuf,
                      TCL_LEAVE_ERR_MSG) == NULL)
        return TCL_ERROR;

    sprintf (numBuf, "%d", statBufPtr->st_atime);
    if  (Tcl_SetVar2 (interp, arrayName, "atime", numBuf,
                      TCL_LEAVE_ERR_MSG) == NULL)
        return TCL_ERROR;

    sprintf (numBuf, "%d", statBufPtr->st_mtime);
    if  (Tcl_SetVar2 (interp, arrayName, "mtime", numBuf,
                      TCL_LEAVE_ERR_MSG) == NULL)
        return TCL_ERROR;

    sprintf (numBuf, "%d", statBufPtr->st_ctime);
    if  (Tcl_SetVar2 (interp, arrayName, "ctime", numBuf,
                      TCL_LEAVE_ERR_MSG) == NULL)
        return TCL_ERROR;

    if (Tcl_SetVar2 (interp, arrayName, "tty", 
                     isatty (fileno (filePtr->f)) ? "1" : "0",
                     TCL_LEAVE_ERR_MSG) == NULL)
        return TCL_ERROR;

    if (Tcl_SetVar2 (interp, arrayName, "type", GetFileType (statBufPtr),
                     TCL_LEAVE_ERR_MSG) == NULL)
        return TCL_ERROR;

    return TCL_OK;

}

/*
 *-----------------------------------------------------------------------------
 *
 * ReturnStatItem --
 *
 *   Return a single file status item.
 *
 * Parameters:
 *   o interp (I) - Item or error returned in result.
 *   o filePtr (I) - Pointer to the Tcl open file structure.
 *   o statBufPtr (I) - Pointer to a buffer initialized by stat or fstat.
 *   o itemName (I) - The name of the desired item.
 * Returns:
 *   TCL_OK or TCL_ERROR.
 *-----------------------------------------------------------------------------
 */
static int
ReturnStatItem (interp, filePtr, statBufPtr, itemName)
    Tcl_Interp   *interp;
    OpenFile     *filePtr;
    struct stat  *statBufPtr;
    char         *itemName;
{
    if (STREQU (itemName, "dev"))
        sprintf (interp->result, "%d", statBufPtr->st_dev);
    else if (STREQU (itemName, "ino"))
        sprintf (interp->result, "%d", statBufPtr->st_ino);
    else if (STREQU (itemName, "mode"))
        sprintf (interp->result, "%d", statBufPtr->st_mode);
    else if (STREQU (itemName, "nlink"))
        sprintf (interp->result, "%d", statBufPtr->st_nlink);
    else if (STREQU (itemName, "uid"))
        sprintf (interp->result, "%d", statBufPtr->st_uid);
    else if (STREQU (itemName, "gid"))
        sprintf (interp->result, "%d", statBufPtr->st_gid);
    else if (STREQU (itemName, "size"))
        sprintf (interp->result, "%d", statBufPtr->st_size);
    else if (STREQU (itemName, "atime"))
        sprintf (interp->result, "%d", statBufPtr->st_atime);
    else if (STREQU (itemName, "mtime"))
        sprintf (interp->result, "%d", statBufPtr->st_mtime);
    else if (STREQU (itemName, "ctime"))
        sprintf (interp->result, "%d", statBufPtr->st_ctime);
    else if (STREQU (itemName, "type"))
        interp->result = GetFileType (statBufPtr);
    else if (STREQU (itemName, "tty"))
        interp->result = isatty (fileno (filePtr->f)) ? "1" : "0";
    else {
        Tcl_AppendResult (interp, "Got \"", itemName, "\", expected one of ",
                          "\"atime\", \"ctime\", \"dev\", \"gid\", \"ino\", ",
                          "\"mode\", \"mtime\", \"nlink\", \"size\", ",
                          "\"tty\", \"type\", \"uid\"", (char *) NULL);

        return TCL_ERROR;
    }

    return TCL_OK;

}

/*
 *-----------------------------------------------------------------------------
 *
 * Tcl_FstatCmd --
 *     Implements the fstat TCL command:
 *         fstat handle [item]|[stat arrayvar]
 *
 *-----------------------------------------------------------------------------
 */
int
Tcl_FstatCmd (clientData, interp, argc, argv)
    ClientData  clientData;
    Tcl_Interp *interp;
    int         argc;
    char      **argv;
{
    OpenFile    *filePtr;
    struct stat  statBuf;

    if ((argc < 2) || (argc > 4)) {
        Tcl_AppendResult (interp, tclXWrongArgs, argv [0], 
                          " handle [item]|[stat arrayVar]", (char *) NULL);
        return TCL_ERROR;
    }

    if (TclGetOpenFile (interp, argv[1], &filePtr) != TCL_OK)
	return TCL_ERROR;
    
    if (fstat (fileno (filePtr->f), &statBuf)) {
        interp->result = Tcl_UnixError (interp);
        return TCL_ERROR;
    }

    /*
     * Return data in the requested format.
     */
    if (argc == 4) {
        if (!STREQU (argv [2], "stat")) {
            Tcl_AppendResult (interp, "expected item name of \"stat\" when ",
                              "using array name", (char *) NULL);
            return TCL_ERROR;
        }
        return ReturnStatArray (interp, filePtr, &statBuf, argv [3]);
    }
    if (argc == 3)
        return ReturnStatItem (interp, filePtr, &statBuf, argv [2]);

    ReturnStatList (interp, filePtr, &statBuf);
    return TCL_OK;

}

/*
 *-----------------------------------------------------------------------------
 *
 * Tcl_LgetsCmd --
 *
 * Implements the `lgets' Tcl command:
 *    lgets fileId [varName]
 *
 * Results:
 *      A standard Tcl result.
 *
 * Side effects:
 *      See the user documentation.
 *
 *-----------------------------------------------------------------------------
 */
int
Tcl_LgetsCmd (notUsed, interp, argc, argv)
    ClientData   notUsed;
    Tcl_Interp  *interp;
    int          argc;
    char       **argv;
{
    dynamicBuf_t  dynBuf;
    char          prevChar;
    int           bracesDepth, inQuotes, inChar;
    OpenFile     *filePtr;

    if ((argc != 2) && (argc != 3)) {
        Tcl_AppendResult (interp, tclXWrongArgs, argv[0],
                          " fileId [varName]", (char *) NULL);
        return TCL_ERROR;
    }
    if (TclGetOpenFile(interp, argv[1], &filePtr) != TCL_OK) {
        return TCL_ERROR;
    }
    if (!filePtr->readable) {
        Tcl_AppendResult (interp, "\"", argv[1],
                          "\" wasn't opened for reading", (char *) NULL);
        return TCL_ERROR;
    }

    Tcl_DynBufInit (&dynBuf);

    prevChar = '\0';
    bracesDepth = 0;
    inQuotes = FALSE;

    /*
     * Read in characters, keeping trace of if we are in the middle of a {}
     * or "" part of the list.
     */

    while (TRUE) {
        if (dynBuf.len + 1 == dynBuf.size)
            Tcl_ExpandDynBuf (&dynBuf, 0);
        inChar = getc (filePtr->f);
        if (inChar == EOF) {
            if (ferror (filePtr->f))
                goto readError;
            break;
        }
        if (prevChar != '\\') {
            switch (inChar) {
                case '{':
                    bracesDepth++;
                    break;
                case '}':
                    if (bracesDepth == 0)
                        break;
                    bracesDepth--;
                    break;
                case '"':
                    if (bracesDepth == 0)
                        inQuotes = !inQuotes;
                    break;
            }
        }
        prevChar = inChar;
        if ((inChar == '\n') && (bracesDepth == 0) && !inQuotes)
            break;
        dynBuf.ptr [dynBuf.len++] = inChar;
    }

    dynBuf.ptr [dynBuf.len] = '\0';

    if ((bracesDepth != 0) || inQuotes) {
        Tcl_AppendResult (interp, "miss-matched ",
                         (bracesDepth != 0) ? "braces" : "quote",
                         " in inputed list: ", dynBuf.ptr, (char *) NULL);
        goto errorExit;
    }

    if (argc == 2) {
        Tcl_DynBufReturn (interp, &dynBuf);
    } else {
        if (Tcl_SetVar (interp, argv[2], dynBuf.ptr, 
                        TCL_LEAVE_ERR_MSG) == NULL)
            goto errorExit;
        if (feof (filePtr->f) && (dynBuf.len == 0))
            interp->result = "-1";
        else
            sprintf (interp->result, "%d", dynBuf.len);
        Tcl_DynBufFree (&dynBuf);
    }
    return TCL_OK;

readError:
    Tcl_ResetResult (interp);
    interp->result = Tcl_UnixError (interp);
    clearerr (filePtr->f);
    goto errorExit;

errorExit:
    Tcl_DynBufFree (&dynBuf);
    return TCL_ERROR;

}

#ifndef TCL_NO_FILE_LOCKING

/*
 *-----------------------------------------------------------------------------
 *
 * ParseLockUnlockArgs --
 *
 * Parse the positional arguments common to both the flock and funlock
 * commands:
 *   ... handle [start] [length] [origin]
 *
 * Parameters:
 *   o interp (I) - Pointer to the interpreter, errors returned in result.
 *   o argc (I) - Count of arguments supplied to the comment.
 *   o argv (I) - Commant argument vector.
 *   o argIdx (I) - Index of the first common agument to parse.
 *   o filePtrPtr (O) - Pointer to the open file structure returned here.
 *   o lockInfoPtr (O) - Fcntl info structure, start, length and whence
 *     are initialized by this routine.
 * Returns:
 *   TCL_OK if all is OK,  TCL_ERROR and an error message is result.
 *
 *-----------------------------------------------------------------------------
 */
static int
ParseLockUnlockArgs (interp, argc, argv, argIdx, filePtrPtr, lockInfoPtr)
    Tcl_Interp    *interp;
    int            argc;
    char         **argv;
    int            argIdx;
    OpenFile     **filePtrPtr;
    struct flock  *lockInfoPtr;
{

    lockInfoPtr->l_start  = 0;
    lockInfoPtr->l_len    = 0;
    lockInfoPtr->l_whence = 0;

    if (TclGetOpenFile (interp, argv [argIdx], filePtrPtr) != TCL_OK)
	return TCL_ERROR;
    argIdx++;

    if ((argIdx < argc) && (argv [argIdx][0] != '\0')) {
        if (Tcl_GetLong (interp, argv [argIdx],
                         &lockInfoPtr->l_start) != TCL_OK)
            return TCL_ERROR;
    }
    argIdx++;

    if ((argIdx < argc) && (argv [argIdx][0] != '\0')) {
        if (Tcl_GetLong (interp, argv [argIdx], &lockInfoPtr->l_len) != TCL_OK)
            return TCL_ERROR;
    }
    argIdx++;

    if (argIdx < argc) {
        if (STREQU (argv [argIdx], "start"))
            lockInfoPtr->l_whence = 0;
        else if (STREQU (argv [argIdx], "current"))
            lockInfoPtr->l_whence = 1;
        else if (STREQU (argv [argIdx], "end"))
            lockInfoPtr->l_whence = 2;
        else
            goto badOrgin;
    }

    return TCL_OK;

  badOrgin:
    Tcl_AppendResult(interp, "bad origin \"", argv [argIdx],
                     "\": should be \"start\", \"current\", or \"end\"",
                     (char *) NULL);
    return TCL_ERROR;
   
}

/*
 *-----------------------------------------------------------------------------
 *
 * Tcl_FlockCmd --
 *
 * Implements the `flock' Tcl command:
 *    flock [-read|-write] [-nowait] handle [start] [length] [origin]
 *
 * Results:
 *      A standard Tcl result.
 *
 *-----------------------------------------------------------------------------
 */
int
Tcl_FlockCmd (notUsed, interp, argc, argv)
    ClientData   notUsed;
    Tcl_Interp  *interp;
    int          argc;
    char       **argv;
{
    int           argIdx, stat;
    int           readLock = FALSE, writeLock = FALSE, noWaitLock = FALSE;
    OpenFile     *filePtr;
    struct flock  lockInfo;

    if (argc < 2)
        goto invalidArgs;

    /*
     * Parse off the options.
     */
    
    for (argIdx = 1; (argIdx < argc) && (argv [argIdx][0] == '-'); argIdx++) {
        if (STREQU (argv [argIdx], "-read")) {
            readLock = TRUE;
            continue;
        }
        if (STREQU (argv [argIdx], "-write")) {
            writeLock = TRUE;
            continue;
        }
        if (STREQU (argv [argIdx], "-nowait")) {
            noWaitLock = TRUE;
            continue;
        }
        goto invalidOption;
    }

    if (readLock && writeLock)
        goto bothReadAndWrite;
    if (!(readLock || writeLock))
        writeLock = TRUE;

    /*
     * Make sure there are enough arguments left and then parse the 
     * positional ones.
     */
    if ((argIdx > argc - 1) || (argIdx < argc - 4))
        goto invalidArgs;

    if (ParseLockUnlockArgs (interp, argc, argv, argIdx, &filePtr,
                             &lockInfo) != TCL_OK)
        return TCL_ERROR;

    if (readLock && !filePtr->readable)
        goto notReadable;
    if (writeLock && !filePtr->writable)
        goto notWritable;

    lockInfo.l_type = writeLock ? F_WRLCK : F_RDLCK;
    
    stat = fcntl (fileno (filePtr->f), noWaitLock ? F_SETLK : F_SETLKW, 
                  &lockInfo);
    if ((stat < 0) && (errno != EACCES)) {
        interp->result = Tcl_UnixError (interp);
        return TCL_ERROR;
    }
    
    if (noWaitLock)
        interp->result = (stat < 0) ? "0" : "1";

    return TCL_OK;

    /*
     * Code to return error messages.
     */

  invalidArgs:
    Tcl_AppendResult (interp, tclXWrongArgs, argv [0], " [-read|-write] ",
                      "[-nowait] handle [start] [length] [origin]",
                      (char *) NULL);
    return TCL_ERROR;

    /*
     * Invalid option found at argv [argIdx].
     */
  invalidOption:
    Tcl_AppendResult (interp, "invalid option \"", argv [argIdx],
                      "\" expected one of \"-read\", \"-write\", or ",
                      "\"-nowait\"", (char *) NULL);
    return TCL_ERROR;

  bothReadAndWrite:
    interp->result = "can not specify both \"-read\" and \"-write\"";
    return TCL_ERROR;

  notReadable:
    interp->result = "file not open for reading";
    return TCL_ERROR;

  notWritable:
    interp->result = "file not open for writing";
    return TCL_ERROR;
}

/*
 *-----------------------------------------------------------------------------
 *
 * Tcl_FunlockCmd --
 *
 * Implements the `funlock' Tcl command:
 *    funlock handle [start] [length] [origin]
 *
 * Results:
 *      A standard Tcl result.
 *
 *-----------------------------------------------------------------------------
 */
int
Tcl_FunlockCmd (notUsed, interp, argc, argv)
    ClientData   notUsed;
    Tcl_Interp  *interp;
    int          argc;
    char       **argv;
{
    OpenFile     *filePtr;
    struct flock  lockInfo;

    if ((argc < 2) || (argc > 5))
        goto invalidArgs;

    if (ParseLockUnlockArgs (interp, argc, argv, 1, &filePtr,
                             &lockInfo) != TCL_OK)
        return TCL_ERROR;

    lockInfo.l_type = F_UNLCK;
    
    if (fcntl (fileno(filePtr->f), F_SETLK, &lockInfo) < 0) {
        interp->result = Tcl_UnixError (interp);
        return TCL_ERROR;
    }
    
    return TCL_OK;

  invalidArgs:
    Tcl_AppendResult (interp, tclXWrongArgs, argv [0], 
                      " handle [start] [length] [origin]", (char *) NULL);
    return TCL_ERROR;

}
#else

/*
 *-----------------------------------------------------------------------------
 *
 * Tcl_FlockCmd --
 *
 * Version of the command that always returns an error on systems that
 * don't have file locking.
 *
 *-----------------------------------------------------------------------------
 */
int
Tcl_FlockCmd (notUsed, interp, argc, argv)
    ClientData   notUsed;
    Tcl_Interp  *interp;
    int          argc;
    char       **argv;
{
    interp->result = "File locking is not available on this system";
    return TCL_ERROR;
}

/*
 *-----------------------------------------------------------------------------
 *
 * Tcl_FunlockCmd --
 *
 * Version of the command that always returns an error on systems that
 * don't have file locking/
 *
 *-----------------------------------------------------------------------------
 */
int
Tcl_FunlockCmd (notUsed, interp, argc, argv)
    ClientData   notUsed;
    Tcl_Interp  *interp;
    int          argc;
    char       **argv;
{
    return Tcl_FlockCmd (notUsed, interp, argc, argv);
}
#endif
