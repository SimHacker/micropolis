/*
 * tclXlib.c --
 *
 * Tcl commands to load libraries of Tcl code.
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
 * $Id: tclXlib.c,v 2.0 1992/10/16 04:50:55 markd Rel $
 *-----------------------------------------------------------------------------
 */

/*-----------------------------------------------------------------------------
 *
 * The following data structures are used by the Tcl library code. All
 * structures are kept in the global array TCLENV, so that Tcl procs may be
 * written to access them.
 *
 *  o fileId - This is a small string used to uniquely identify a file, it is
 *    in the form "@$dev:$inode", where dev and inode are the values obtained
 *    from stat.
 *
 *  o TCLENV(fileId} filePath - This entry translates a file id to an
 *    file name, which may be an absolute path to a file or the name of 
 *    a file to find by searching a path.
 *
 *  o TCLENV(PKG:$packageName) {$fileId $offset $length} - This entry
 *    translates a package name into a fileId of the file containing the
 *    package and the byte and offset length of the package within the file.
 *    
 *  o TCLENV(PROC:$proc) {P $packageName} - This form of a procedure entry
 *    translates a procedure into a package name.
 *
 *  o TCLENV(PROC:$proc) {F $fileName} 0 - This form of a procedure entry
 *    translates a procedure into a file name.  The file name may be an
 *    absolute path to the file or a file to be found by searching TCLPATH
 *    or auto_path.
 *-----------------------------------------------------------------------------
 */
#include "tclxint.h"

typedef char fileId_t [64];

/*
 * Prototypes of internal functions.
 */
static int
EvalFilePart _ANSI_ARGS_((Tcl_Interp  *interp,
                          char        *fileName,
                          long         offset,
                          unsigned     length));

static char *
MakeAbsFile _ANSI_ARGS_((Tcl_Interp  *interp,
                         char        *fileName,
                         char        *buffer,
                         int          bufferSize));

static int
GenerateFileId _ANSI_ARGS_((Tcl_Interp *interp,
                            char       *filePath,
                            fileId_t    fileId));

static int
SetTCLENVFileIdEntry _ANSI_ARGS_((Tcl_Interp *interp,
                                  fileId_t    fileId,
                                  char       *filePath));

static int
CheckTCLENVFileIdEntry _ANSI_ARGS_((Tcl_Interp *interp,
                                    char       *filePath));
     
static char *
GetTCLENVFileIdEntry  _ANSI_ARGS_((Tcl_Interp  *interp,
                                   fileId_t     fileId));

static int
SetTCLENVPkgEntry _ANSI_ARGS_((Tcl_Interp *interp,
                               char       *packageName,
                               fileId_t    fileId,
                               char       *offset,
                               char       *length));

static int
GetTCLENVPkgEntry _ANSI_ARGS_((Tcl_Interp *interp,
                               char       *packageName,
                               char       *fileId,
                               long       *offsetPtr,
                               unsigned   *lengthPtr));

static int
SetTCLENVProcEntry _ANSI_ARGS_((Tcl_Interp *interp,
                                char       *procName,
                                char       *type,
                                char       *location));

static int
GetTCLENVProcEntry  _ANSI_ARGS_((Tcl_Interp *interp,
                                 char       *procName,
                                 char       *typePtr,
                                 char      **locationPtr));

static int
ProcessIndexFile _ANSI_ARGS_((Tcl_Interp *interp,
                              char       *tlibFilePath,
                              char       *tndxFilePath));

static int
BuildPackageIndex  _ANSI_ARGS_((Tcl_Interp *interp,
                                char       *tlibFilePath));

static int
LoadPackageIndex _ANSI_ARGS_((Tcl_Interp *interp,
                              char       *tlibFilePath,
                              int         pathLen,
                              int         dirLen));

static int
LoadOusterIndex _ANSI_ARGS_((Tcl_Interp *interp,
                             char       *indexFilePath,
                             int         dirLen));

static int
LoadDirIndexes _ANSI_ARGS_((Tcl_Interp  *interp,
                            char        *dirName));

static int
LoadPackageIndexes _ANSI_ARGS_((Tcl_Interp  *interp,
                                char        *path));

static int
LoadProc _ANSI_ARGS_((Tcl_Interp  *interp,
                      char        *procName,
                      int         *foundPtr));


/*
 *-----------------------------------------------------------------------------
 *
 * EvalFilePart --
 *
 *   Read in a byte range of a file and evaulate it.
 *
 * Parameters:
 *   o interp (I) - A pointer to the interpreter, error returned in result.
 *   o fileName (I) - The file to evaulate.
 *   o offset (I) - Byte offset into the file of the area to evaluate
 *   o length (I) - Number of bytes to evaulate..
 *
 * Results:
 *   A standard Tcl result.
 *-----------------------------------------------------------------------------
 */
static int
EvalFilePart (interp, fileName, offset, length)
    Tcl_Interp  *interp;
    char        *fileName;
    long         offset;
    unsigned     length;
{
    Interp       *iPtr = (Interp *) interp;
    int           fileNum, result;
    struct stat   statBuf;
    char         *oldScriptFile, *cmdBuffer;


    if (fileName [0] == '~')
        if ((fileName = Tcl_TildeSubst (interp, fileName)) == NULL)
            return TCL_ERROR;

    fileNum = open (fileName, O_RDONLY, 0);
    if (fileNum < 0) {
        Tcl_AppendResult (interp, "open failed on: ", fileName, ": ",
                          Tcl_UnixError (interp), (char *) NULL);
        return TCL_ERROR;
    }
    if (fstat (fileNum, &statBuf) == -1)
        goto accessError;

    if ((statBuf.st_size < offset + length) || (offset < 0)) {
        Tcl_AppendResult (interp, "range to eval outside of file bounds \"",
                          fileName, "\"", (char *) NULL);
        close (fileNum);
        return TCL_ERROR;
    }
    if (lseek (fileNum, offset, 0) < 0)
        goto accessError;

    cmdBuffer = ckalloc (length + 1);
    if (read (fileNum, cmdBuffer, length) != length)
        goto accessError;

    cmdBuffer [length] = '\0';

    if (close (fileNum) != 0)
        goto accessError;

    oldScriptFile = iPtr->scriptFile;
    iPtr->scriptFile = fileName;

    result = Tcl_Eval (interp, cmdBuffer, 0, (char **) NULL);

    iPtr->scriptFile = oldScriptFile;
    ckfree (cmdBuffer);
                         
    if (result != TCL_ERROR)
        return TCL_OK;

    /*
     * An error occured. Record information telling where it came from.
     */
    {
        char buf [100];
        sprintf (buf, "\n    (file \"%.50s\" line %d)", fileName,
                 interp->errorLine);
        Tcl_AddErrorInfo(interp, buf);
    }
    return TCL_ERROR;

    /*
     * Errors accessing the file once its opened are handled here.
     */
  accessError:
    Tcl_AppendResult (interp, "error accessing: ", fileName, ": ",
                      Tcl_UnixError (interp), (char *) NULL);

    close (fileNum);
    return TCL_ERROR;
}

/*
 *-----------------------------------------------------------------------------
 *
 * MakeAbsFile --
 *
 * Convert a file name to an absolute path.  This handles tilde substitution
 * and preappend the current directory name if the path is relative.
 *
 * Parameters
 *   o interp (I) - A pointer to the interpreter, error returned in result.
 *   o fileName (I) - File name (should not start with a "/").
 *   o buffer (O) - Buffer to store string in, if it will fit.
 *   o bufferSize (I) - Size of buffer.
 * Returns:
 *   A pointer to the file name.  If the string would fit in buffer, then
 * a pointer to buffer is returned, otherwise a dynamicaly allocated file
 * name.   NULL is returned if an error occured.
 *-----------------------------------------------------------------------------
 */
static char *
MakeAbsFile (interp, fileName, buffer, bufferSize)
    Tcl_Interp  *interp;
    char        *fileName;
    char        *buffer;
    int          bufferSize;
{
    char   curDir [MAXPATHLEN+1];
    char  *pathName;
    int    pathLen;

    if (fileName [0] == '~') {
        fileName = Tcl_TildeSubst (interp, fileName);
        if (fileName == NULL)
            return NULL;
        pathLen = strlen (fileName);
        if (pathLen < bufferSize)
            pathName = buffer;
        else
            pathName = ckalloc (pathLen + 1);
        strcpy (pathName, fileName);
        return pathName;
    }

#if TCL_GETWD
    if (getwd (curDir) == NULL) {
        Tcl_AppendResult (interp, "error getting working directory name: ",
                          curDir, (char *) NULL);
    }
#else
    if (getcwd (curDir, MAXPATHLEN) == 0) {
        Tcl_AppendResult (interp, "error getting working directory name: ",
                          Tcl_UnixError (interp), (char *) NULL);
    }
#endif
    pathLen = strlen (curDir) + strlen (fileName) + 1;  /* For `/' */
    if (pathLen < bufferSize)
        pathName = buffer;
    else
        pathName = ckalloc (pathLen + 1);
    strcpy (pathName, curDir);
    strcat (pathName, "/");
    strcat (pathName, fileName);

    return pathName;
}

/*
 *-----------------------------------------------------------------------------
 *
 * GenerateFileId --
 *
 * Given a path to a file, generate its file Id, in the form:
 *
 *     "@dev:inode"
 *
 * Parameters
 *   o interp (I) - A pointer to the interpreter, error returned in result.
 *   o filepath (I) - Absolute path to the file.
 *   o fileId (O) - File id is returned here.
 * Returns:
 *   TCL_OK or TCL_ERROR.
 *-----------------------------------------------------------------------------
 */
static int
GenerateFileId (interp, filePath, fileId)
     Tcl_Interp *interp;
     char       *filePath;
     fileId_t    fileId;
{
    struct stat  statInfo;

    if (stat (filePath, &statInfo) < 0) {
        Tcl_AppendResult (interp, "stat of \"", filePath, "\" failed: ",
                          Tcl_UnixError (interp), (char *) NULL);
        return TCL_ERROR;
    }

    sprintf (fileId, "@%d:%d", statInfo.st_dev, statInfo.st_ino);

    return TCL_OK;
}

/*
 *-----------------------------------------------------------------------------
 *
 * SetTCLENVFileIdEntry --
 *
 * Set a file entry in the TCLENV array for a file path in the form:
 *
 *     TCLENV(@dev:inode) filepath
 *
 * This entry translates a dev:info into a full file path.
 *
 * Parameters
 *   o interp (I) - A pointer to the interpreter, error returned in result.
 *   o fileId (I) -  The file Id for the file.
 *   o filepath (I) - Absolute path to the file.
 * Returns:
 *   TCL_OK or TCL_ERROR.
 *-----------------------------------------------------------------------------
 */
static int
SetTCLENVFileIdEntry (interp, fileId, filePath)
     Tcl_Interp *interp;
     fileId_t    fileId;
     char       *filePath;
{

    if (Tcl_SetVar2 (interp, "TCLENV", fileId, filePath,
                     TCL_GLOBAL_ONLY | TCL_LEAVE_ERR_MSG) == NULL)
        return TCL_ERROR;

    return TCL_OK;
}

/*
 *-----------------------------------------------------------------------------
 *
 * CheckTCLENVFileIdEntry --
 *
 * Check if there is a file entry in for the specified file.
 *
 * Parameters
 *   o interp (I) - A pointer to the interpreter.
 *   o filePath (I) - Absolute path to the library file.
 * Returns:
 *   TRUE is returned if the entry exists, FALSE if it doesn't.
 *-----------------------------------------------------------------------------
 */
static int
CheckTCLENVFileIdEntry (interp, filePath)
    Tcl_Interp *interp;
    char       *filePath;
{
    fileId_t fileId;

    /*
     * If we can't generate the Id (stat failed), then just say it doesn't
     * exists, other, complain later when an attempt is made to process it.
     */
    if (GenerateFileId (interp, filePath, fileId) != TCL_OK) {
        Tcl_ResetResult (interp);
        return FALSE;
    }

    if (Tcl_GetVar2 (interp, "TCLENV", fileId, TCL_GLOBAL_ONLY) == NULL)
        return FALSE;

    return TRUE;
}

/*
 *-----------------------------------------------------------------------------
 *
 * GetTCLENVFileIdEntry --
 *
 * Translate a file id into a file path.
 *
 * Parameters
 *   o interp (I) - A pointer to the interpreter.
 *   o fileId (I) - The file identifier, in the form: "@$dev:$inode"
 * Returns:
 *   A pointer to the absolute path to the library file is returned
 *     here.  This pointer remains valid until the TCLENV entry is changed,
 *     do not free.
 *-----------------------------------------------------------------------------
 */
static char *
GetTCLENVFileIdEntry (interp, fileId)
    Tcl_Interp  *interp;
    fileId_t     fileId;
{
    char *filePath;

    filePath = Tcl_GetVar2 (interp, "TCLENV", fileId, TCL_GLOBAL_ONLY);
    if (filePath == NULL) {
        Tcl_AppendResult (interp, "TCLENV file id entry not found for: \"",
                          fileId, "\"", (char *) NULL);
        return NULL;
    }
    
    return filePath;
}

/*
 *-----------------------------------------------------------------------------
 *
 * SetTCLENVPkgEntry --
 *
 * Set the package entry in the TCLENV array for a package in the form:
 *
 *     TCLENV(PKG:$packageName) [list $fileId $offset $length]
 *
 * Duplicate package names are rejected.
 *
 * Parameters
 *   o interp (I) - A pointer to the interpreter, error returned in result.
 *   o packageName (I) - Package name.
 *   o fileId (I) - File id for the file.
 *   o offset (I) - String containing the numeric start of the package.
 *   o length (I) - Strign containing the numeric length of the package.
 * Returns:
 *   TCL_OK,r TCL_ERROR of TCL_CONTINUE if the package name is already defined
 * and should be skipped.
 *-----------------------------------------------------------------------------
 */
static int
SetTCLENVPkgEntry (interp, packageName, fileId, offset, length)
     Tcl_Interp *interp;
     char       *packageName;
     fileId_t    fileId;
     char       *offset;
     char       *length;
{
    int   nameLen;
    char  indexBuffer [64], *indexPtr;
    char *pkgDataArgv [3], *dataStr, *setResult;

    nameLen = strlen (packageName) + 5;  /* includes "PKG:" and '\0' */
    if (nameLen <= sizeof (indexBuffer))
        indexPtr = indexBuffer;
    else
        indexPtr = ckalloc (nameLen);

    strcpy (indexPtr,     "PKG:");
    strcpy (indexPtr + 4, packageName);

    /*
     * Check for duplicate package name.
     */
    if (Tcl_GetVar2 (interp, "TCLENV", indexPtr, TCL_GLOBAL_ONLY) != NULL) {
        if (indexPtr != indexBuffer)
            ckfree (indexPtr);
        return TCL_CONTINUE;
    }

    pkgDataArgv [0] = fileId;
    pkgDataArgv [1] = offset;
    pkgDataArgv [2] = length;
    dataStr = Tcl_Merge (3, pkgDataArgv);

    setResult = Tcl_SetVar2 (interp, "TCLENV", indexPtr, dataStr,
                             TCL_GLOBAL_ONLY | TCL_LEAVE_ERR_MSG);
    ckfree (dataStr);
    if (indexPtr != indexBuffer)
        ckfree (indexPtr);

    return (setResult == NULL) ? TCL_ERROR : TCL_OK;
}

/*
 *-----------------------------------------------------------------------------
 *
 * GetTCLENVPkgEntry --
 *
 * Get the package entry in the TCLENV array for a package.
 *
 * Parameters
 *   o interp (I) - A pointer to the interpreter, error returned in result.
 *   o packageName (I) - Package name to find.
 *   o fileId (O) - The fileId for the library file is returned here.
 *   o offsetPtr (O) - Start of the package in the library.
 *   o lengthPtr (O) - Length of the package in the library.
 * Returns:
 *   TCL_OK or TCL_ERROR.
 *-----------------------------------------------------------------------------
 */
static int
GetTCLENVPkgEntry (interp, packageName, fileId, offsetPtr, lengthPtr)
     Tcl_Interp *interp;
     char       *packageName;
     fileId_t    fileId;
     long       *offsetPtr;
     unsigned   *lengthPtr;
{
    int            nameLen, pkgDataArgc;
    char           indexBuffer [64], *indexPtr;
    char          *dataStr, **pkgDataArgv = NULL;
    register char *srcPtr, *destPtr;    

    nameLen = strlen (packageName) + 5;  /* includes "PKG:" and '\0' */
    if (nameLen <= sizeof (indexBuffer))
        indexPtr = indexBuffer;
    else
        indexPtr = ckalloc (nameLen);

    strcpy (indexPtr,     "PKG:");
    strcpy (indexPtr + 4, packageName);

    dataStr = Tcl_GetVar2 (interp, "TCLENV", indexPtr, TCL_GLOBAL_ONLY);
    if (dataStr == NULL) {
        Tcl_AppendResult (interp, "entry not found in TCLENV for package \"",
                          packageName, "\"", (char *) NULL);
        if (indexPtr != indexBuffer)
            ckfree (indexPtr);
        return TCL_ERROR;
    }

    /*
     * Extract the data from the array entry.
     */

    if (Tcl_SplitList (interp, dataStr, &pkgDataArgc,
                       &pkgDataArgv) != TCL_OK)
        goto invalidEntry;
    if (pkgDataArgc != 3)
        goto invalidEntry;
    if (strlen (pkgDataArgv [0]) >= sizeof (fileId_t))
        goto invalidEntry;
    strcpy (fileId, pkgDataArgv [0]);
    if (!Tcl_StrToLong (pkgDataArgv [1], 0, offsetPtr))
        goto invalidEntry;
    if (!Tcl_StrToUnsigned (pkgDataArgv [2], 0, lengthPtr))
        goto invalidEntry;

    ckfree (pkgDataArgv);
    if (indexPtr != indexBuffer)
        ckfree (indexPtr);
    return TCL_OK;
    
    /*
     * Exit point when an invalid entry is found.
     */
  invalidEntry:
    if (pkgDataArgv != NULL)
        ckfree (pkgDataArgv);
    Tcl_ResetResult (interp);
    Tcl_AppendResult (interp, "invalid entry for package library: TCLENV(",
                      indexPtr,") is \"", dataStr, "\"", (char *) NULL);
    if (indexPtr != indexBuffer)
        ckfree (indexPtr);
    return TCL_ERROR;
}

/*
 *-----------------------------------------------------------------------------
 *
 * SetTCLENVProcEntry --
 *
 * Set the proc entry in the TCLENV array for a package in the form:
 *
 *     TCLENV(PROC:$proc) [list P $packageName]
 * or
 *     TCLENV(PROC:$proc) [list F $fileId]
 *
 * Parameters
 *   o interp (I) - A pointer to the interpreter, error returned in result.
 *   o procName (I) - The Tcl proc name.
 *   o type (I) - "P" for a package entry or "F" for a file entry.
 *   o location (I) - Either the package name or file name containing the
 *                    procedure.
 * Returns:
 *   TCL_OK or TCL_ERROR.
 *-----------------------------------------------------------------------------
 */
static int
SetTCLENVProcEntry (interp, procName, type, location)
    Tcl_Interp *interp;
    char       *procName;
    char       *type;
    char       *location;
{
    int   nameLen;
    char  indexBuffer [64], *indexPtr;
    char *procDataArgv [2], *dataStr, *setResult;

    nameLen = strlen (procName) + 6;  /* includes "PROC:" and '\0' */
    if (nameLen <= sizeof (indexBuffer))
        indexPtr = indexBuffer;
    else
        indexPtr = ckalloc (nameLen);

    strcpy (indexPtr,     "PROC:");
    strcpy (indexPtr + 5, procName);

    procDataArgv [0] = type;
    procDataArgv [1] = location;
    dataStr = Tcl_Merge (2, procDataArgv);

    setResult = Tcl_SetVar2 (interp, "TCLENV", indexPtr, dataStr,
                             TCL_GLOBAL_ONLY | TCL_LEAVE_ERR_MSG);
    ckfree (dataStr);
    if (indexPtr != indexBuffer)
        ckfree (indexPtr);

    return (setResult == NULL) ? TCL_ERROR : TCL_OK;
}

/*
 *-----------------------------------------------------------------------------
 *
 * GetTCLENVProcEntry --
 *
 * Get the proc entry in the TCLENV array for a package.
 *
 * Parameters
 *   o interp (I) - A pointer to the interpreter, error returned in result.
 *   o procName (I) - The Tcl proc name.
 *   o typePtr (O) - 'P' for a package entry or 'F' for a file entry.  This
 *     is a single character result.
 *   o location (O) - Either the package name or the file name.  It is
 *     dynamically allocated and must be freed when finished.  NULL is
 *     return if the procedure is not found.
 * Returns:
 *   TCL_OK or TCL_ERROR.
 *-----------------------------------------------------------------------------
 */
static int
GetTCLENVProcEntry (interp, procName, typePtr, locationPtr)
    Tcl_Interp *interp;
    char       *procName;
    char       *typePtr;
    char      **locationPtr;
{
    int            nameLen, procDataArgc;
    char           indexBuffer [64], *indexPtr;
    char          *dataStr, *setResult, **procDataArgv;
    register char *srcPtr, *destPtr;    

    nameLen = strlen (procName) + 6;  /* includes "PROC:" and '\0' */
    if (nameLen <= sizeof (indexBuffer))
        indexPtr = indexBuffer;
    else
        indexPtr = ckalloc (nameLen);

    strcpy (indexPtr,     "PROC:");
    strcpy (indexPtr + 5, procName);

    dataStr = Tcl_GetVar2 (interp, "TCLENV", indexPtr, TCL_GLOBAL_ONLY);
    if (dataStr == NULL) {
        if (indexPtr != indexBuffer)
            ckfree (indexPtr);
        *locationPtr = NULL;
        return TCL_OK;
    }

    /*
     * Extract the data from the array entry.
     */

    if (Tcl_SplitList (interp, dataStr, &procDataArgc,
                       &procDataArgv) != TCL_OK)
        goto invalidEntry;
    if ((procDataArgc != 2) || (procDataArgv [0][1] != '\0'))
        goto invalidEntry;
    if (!((procDataArgv [0][0] == 'F') || (procDataArgv [0][0] == 'P')))
        goto invalidEntry;
    *typePtr = procDataArgv [0][0];

    /*
     * Now do a nasty trick to save a malloc.  Since procDataArgv contains
     * the string, just move the string to the top and type cast.
     */
    destPtr = (char *) procDataArgv;
    srcPtr  = procDataArgv [1];
    while (*srcPtr != '\0')
        *(destPtr++) = *(srcPtr++);
    *destPtr = '\0';
    *locationPtr = (char *) procDataArgv;

    if (indexPtr != indexBuffer)
        ckfree (indexPtr);
    return TCL_OK;

    /*
     * Exit point when an invalid entry is found.
     */
  invalidEntry:
    if (procDataArgv != NULL)
        ckfree (procDataArgv);
    Tcl_ResetResult (interp);
    Tcl_AppendResult (interp, "invalid entry for procedure: TCLENV(",
                      indexPtr,") is \"", dataStr, "\"", (char *) NULL);
    if (indexPtr != indexBuffer)
        ckfree (indexPtr);
    return TCL_ERROR;
}

/*
 *-----------------------------------------------------------------------------
 *
 * ProcessIndexFile --
 *
 * Open and process a package library index file (.tndx).  Creates an
 * entry in the form:
 *
 *     TCLENV(PKG:$packageName) [list $fileId $start $len]
 *
 * for each package and a entry in the from
 *
 *     TCLENV(PROC:$proc) [list P $packageName]
 *
 * for each entry procedure in a package.   If the package is already defined,
 * it it skipped.
 *
 * Parameters
 *   o interp (I) - A pointer to the interpreter, error returned in result.
 *   o tlibFilePath (I) - Absolute path name to the library file.
 *   o tndxFilePath (I) - Absolute path name to the library file index.
 * Returns:
 *   TCL_OK or TCL_ERROR.
 *-----------------------------------------------------------------------------
 */
static int
ProcessIndexFile (interp, tlibFilePath, tndxFilePath)
     Tcl_Interp *interp;
     char       *tlibFilePath;
     char       *tndxFilePath;
{
    fileId_t      fileId;
    FILE         *indexFilePtr;
    dynamicBuf_t  lineBuffer;
    int           lineArgc, idx, result;
    char        **lineArgv = NULL;

    if (GenerateFileId (interp, tlibFilePath, fileId) != TCL_OK)
        return TCL_ERROR;

    indexFilePtr = fopen (tndxFilePath, "r");
    if (indexFilePtr == NULL) {
        Tcl_AppendResult (interp, "open failed on: ", tndxFilePath, ": ",
                          Tcl_UnixError (interp), (char *) NULL);
        return TCL_ERROR;           
    }
    
    Tcl_DynBufInit (&lineBuffer);

    while (TRUE) {
        switch (Tcl_DynamicFgets (&lineBuffer, indexFilePtr, FALSE)) {
          case 0:  /* EOF */
            goto reachedEOF;
          case -1: /* Error */
            Tcl_AppendResult (interp, Tcl_UnixError (interp), (char *) NULL);
            goto errorExit;
        }
        if ((Tcl_SplitList (interp, lineBuffer.ptr, &lineArgc,
                            &lineArgv) != TCL_OK) || (lineArgc < 4))
            goto formatError;
        
        /*
         * lineArgv [0] is the package name.
         * lineArgv [1] is the package offset in the library.
         * lineArgv [2] is the package length in the library.
         * lineArgv [3-n] are the entry procedures for the package.
         */
        result = SetTCLENVPkgEntry (interp, lineArgv [0], fileId, lineArgv [1],
                                    lineArgv [2]);
        if (result == TCL_ERROR)
            goto errorExit;

        /*
         * If the package is not duplicated, add the procedures.
         */
        if (result != TCL_CONTINUE) {
            for (idx = 3; idx < lineArgc; idx++) {
                if (SetTCLENVProcEntry (interp, lineArgv [idx], "P",
                                        lineArgv [0]) != TCL_OK)
                    goto errorExit;
            }
        }
        ckfree (lineArgv);
        lineArgv = NULL;
    }

  reachedEOF:
    fclose (indexFilePtr);
    Tcl_DynBufFree (&lineBuffer);

    if (SetTCLENVFileIdEntry (interp, fileId, tlibFilePath) != TCL_OK)
        return TCL_ERROR;

    return TCL_OK;

    /*
     * Handle format error in library input line.
     */
  formatError:
    Tcl_ResetResult (interp);
    Tcl_AppendResult (interp, "format error in library index \"",
                      tndxFilePath, "\" (", lineBuffer.ptr, ")",
                      (char *) NULL);
    goto errorExit;

    /*
     * Error exit here, releasing resources and closing the file.
     */
  errorExit:
    if (lineArgv != NULL)
        ckfree (lineArgv);
    Tcl_DynBufFree (&lineBuffer);
    fclose (indexFilePtr);
    return TCL_ERROR;
}

/*
 *-----------------------------------------------------------------------------
 *
 * BuildPackageIndex --
 *
 * Call the "buildpackageindex" Tcl procedure to rebuild a package index.
 * If the procedure has not been loaded, then load it.  It MUST have an
 * proc record setup by autoload.
 *
 * Parameters
 *   o interp (I) - A pointer to the interpreter, error returned in result.
 *   o tlibFilePath (I) - Absolute path name to the library file.
 * Returns:
 *   TCL_OK or TCL_ERROR.
 *-----------------------------------------------------------------------------
 */
static int
BuildPackageIndex (interp, tlibFilePath)
     Tcl_Interp *interp;
     char       *tlibFilePath;
{
    char *cmdPtr, *initCmd;

    /*
     * Load buildpackageindex if it is not loaded
     */
    if (TclFindProc ((Interp *) interp, "buildpackageindex") == NULL) {

        cmdPtr = "demand_load buildpackageindex";

        if (Tcl_Eval (interp, cmdPtr, 0, (char **) NULL) != TCL_OK)
            return TCL_ERROR;

        if (!STREQU (interp->result, "1")) {
            Tcl_ResetResult (interp);
            interp->result =
                "can not find \"buildpackageindex\" on \"TCLPATH\"";
            return TCL_ERROR;
        }
        Tcl_ResetResult (interp);
    }

    /*
     * Build the package index.
     */
    initCmd = "buildpackageindex ";

    cmdPtr = ckalloc (strlen (initCmd) + strlen (tlibFilePath) + 1);
    strcpy (cmdPtr, initCmd);
    strcat (cmdPtr, tlibFilePath);

    if (Tcl_Eval (interp, cmdPtr, 0, (char **) NULL) != TCL_OK) {
        ckfree (cmdPtr);
        return TCL_ERROR;
    }
    ckfree (cmdPtr);
    Tcl_ResetResult (interp);
    return TCL_OK;
}

/*
 *-----------------------------------------------------------------------------
 *
 * LoadPackageIndex --
 *
 * Load a package .tndx file.  Rebuild .tlib if non-existant or out of
 * date.  An entry is made in the TCLENV array indicating that this file
 * has been loaded.
 *
 * Parameters
 *   o interp (I) - A pointer to the interpreter, error returned in result.
 *   o tlibFilePath (I) - Absolute path name to the library file.
 *   o pathLen (I) - Length of tlibFilePath.
 *   o dirLen (I) - The length of the leading directory path in the name.
 * Returns:
 *   TCL_OK or TCL_ERROR.
 *-----------------------------------------------------------------------------
 */
static int
LoadPackageIndex (interp, tlibFilePath, pathLen, dirLen)
     Tcl_Interp *interp;
     char       *tlibFilePath;
     int         pathLen;
     int         dirLen;
{
    char        *tndxFilePath, tndxPathBuf [64], *msg;
    struct stat  tlibStat;
    struct stat  tndxStat;

    if (pathLen < sizeof (tndxPathBuf))
        tndxFilePath = tndxPathBuf;
    else
        tndxFilePath = ckalloc (pathLen + 1);
    strcpy (tndxFilePath, tlibFilePath);
    tndxFilePath [pathLen - 3] = 'n';
    tndxFilePath [pathLen - 2] = 'd';
    tndxFilePath [pathLen - 1] = 'x';

    /*
     * Get library's modification time.  If the file can't be accessed, set
     * time so the library does not get built.  Other code will report the
     * error.
     */
    if (stat (tlibFilePath, &tlibStat) < 0)
        tlibStat.st_mtime = MAXINT;

    /*
     * Get the time for the index.  If the file does not exists or is
     * out of date, rebuild it.
     */

    if ((stat (tndxFilePath, &tndxStat) < 0) ||
        (tndxStat.st_mtime < tlibStat.st_mtime)) {
        if (BuildPackageIndex (interp, tlibFilePath) != TCL_OK)
            goto errorExit;
    }

    if (ProcessIndexFile (interp, tlibFilePath, tndxFilePath) != TCL_OK)
        goto errorExit;
    if (tndxFilePath != tndxPathBuf)
        ckfree (tndxFilePath);
    return TCL_OK;

  errorExit:
    if (tndxFilePath != tndxPathBuf)
        ckfree (tndxFilePath);
    msg = ckalloc (strlen (tlibFilePath) + 60);
    strcpy (msg, "\n    while loading Tcl package library index \"");
    strcat (msg, tlibFilePath);
    strcat (msg, "\"");
    Tcl_AddErrorInfo (interp, msg);
    ckfree (msg);
    return TCL_ERROR;
}

/*
 *-----------------------------------------------------------------------------
 *
 * LoadOusterIndex --
 *
 * Load a standard Tcl index (tclIndex).  An entry is made in the TCLENV
 * array indicating that this file has been loaded.
 *
 * Parameters
 *   o interp (I) - A pointer to the interpreter, error returned in result.
 *   o indexFilePath (I) - Absolute path name to the tclIndex file.
 *   o dirLen (I) - The length of the directory component of indexFilePath.
 * Returns:
 *   TCL_OK or TCL_ERROR.
 *-----------------------------------------------------------------------------
 */
static int
LoadOusterIndex (interp, indexFilePath, dirLen)
     Tcl_Interp *interp;
     char       *indexFilePath;
     int         dirLen;
{
    FILE         *indexFilePtr;
    fileId_t      fileId;
    dynamicBuf_t  lineBuffer;
    int           lineArgc, result, filePathLen;
    char        **lineArgv = NULL, *filePath, filePathBuf [64], *msg;

    indexFilePtr = fopen (indexFilePath, "r");
    if (indexFilePtr == NULL) {
        Tcl_AppendResult (interp, "open failed on: ", indexFilePath, ": ",
                          Tcl_UnixError (interp), (char *) NULL);
        return TCL_ERROR;           
    }
    
    Tcl_DynBufInit (&lineBuffer);

    while (TRUE) {
        switch (Tcl_DynamicFgets (&lineBuffer, indexFilePtr, FALSE)) {
          case 0:  /* EOF */
            goto reachedEOF;
          case -1: /* Error */
            Tcl_AppendResult (interp, "read filed on: ", indexFilePath, ": ",
                              Tcl_UnixError (interp), (char *) NULL);
            goto errorExit;
        }
        if ((lineBuffer.ptr [0] == '\0') || (lineBuffer.ptr [0] == '#'))
            continue;

        if (Tcl_SplitList (interp, lineBuffer.ptr, &lineArgc,
                           &lineArgv) != TCL_OK)
            goto formatError;
        if (! ((lineArgc == 0) || (lineArgc == 2)))
            goto formatError;

        if (lineArgc != 0) {
            filePathLen = strlen (lineArgv [1]) + dirLen + 1;
            if (filePathLen < sizeof (filePathBuf))
                filePath = filePathBuf;
            else
                filePath = ckalloc (filePathLen + 1);
            strncpy (filePath, indexFilePath, dirLen + 1);
            strcpy (filePath + dirLen + 1, lineArgv [1]);

            result = SetTCLENVProcEntry (interp, lineArgv [0], "F", filePath);

            if (filePath != filePathBuf)
                ckfree (filePath);
            if (result != TCL_OK)
                goto errorExit;
        }
        ckfree (lineArgv);
        lineArgv = NULL;
    }

  reachedEOF:
    Tcl_DynBufFree (&lineBuffer);
    fclose (indexFilePtr);

    if (GenerateFileId (interp, indexFilePath, fileId) != TCL_OK)
        return TCL_ERROR;
    if (SetTCLENVFileIdEntry (interp, fileId, indexFilePath) != TCL_OK)
        return TCL_ERROR;

    return TCL_OK;

    /*
     * Handle format error in library input line. If data is already in the
     * result, its assumed to be the error that brought us here.
     */
  formatError:
    if (interp->result [0] != '\0')
        Tcl_AppendResult (interp, "\n",  (char *) NULL);
    Tcl_AppendResult (interp, "format error in library index \"",
                      indexFilePath, "\" (", lineBuffer.ptr, ")",
                      (char *) NULL);

    /*
     * Error exit here, releasing resources and closing the file.
     */
  errorExit:
    if (lineArgv != NULL)
        ckfree (lineArgv);
    Tcl_DynBufFree (&lineBuffer);
    fclose (indexFilePtr);

    msg = ckalloc (strlen (indexFilePath) + 45);
    strcpy (msg, "\n    while loading Tcl procedure index \"");
    strcat (msg, indexFilePath);
    strcat (msg, "\"");
    Tcl_AddErrorInfo (interp, msg);
    ckfree (msg);
    return TCL_ERROR;
}

/*
 *-----------------------------------------------------------------------------
 *
 * LoadDirIndexes --
 *
 *     Load the indexes for all package library (.tlib) or a Ousterhout
 *  "tclIndex" file in a directory.  Nonexistent or unreadable directories
 *  are skipped.
 *
 * Parameters
 *   o interp (I) - A pointer to the interpreter, error returned in result.
 *   o dirName (I) - The absolute path name of the directory to search for
 *     libraries.
 * Results:
 *   A standard Tcl result.
 *-----------------------------------------------------------------------------
 */
static int
LoadDirIndexes (interp, dirName)
    Tcl_Interp  *interp;
    char        *dirName;
{
    DIR           *dirPtr;
    struct dirent *entryPtr;
    int            dirLen, nameLen;
    char          *filePath = NULL;
    int            filePathSize = 0;

    dirLen = strlen (dirName);

    dirPtr = opendir (dirName);
    if (dirPtr == NULL)
        return TCL_OK;   /* Skip directory */

    while (TRUE) {
        entryPtr = readdir (dirPtr);
        if (entryPtr == NULL)
            break;
        nameLen = strlen (entryPtr->d_name);

        if ((nameLen > 5) && 
            ((STREQU (entryPtr->d_name + nameLen - 5, ".tlib")) ||
             (STREQU (entryPtr->d_name, "tclIndex")))) {

            /*
             * Expand the filePath buffer if necessary (always allow extra).
             */
            if ((nameLen + dirLen + 2) > filePathSize) {
                if (filePath != NULL)
                    ckfree (filePath);
                filePathSize = nameLen + dirLen + 2 + 16;
                filePath = ckalloc (filePathSize);
                strcpy (filePath, dirName);
                filePath [dirLen] = '/';
            }
            strcpy (filePath + dirLen + 1, entryPtr->d_name);

            /*
             * Skip index if it has been loaded before or if it can't be
             * accessed.
             */
            if (CheckTCLENVFileIdEntry (interp, filePath) ||
                (access (filePath, R_OK) < 0))
                continue;

            if (entryPtr->d_name [nameLen - 5] == '.') {
                if (LoadPackageIndex (interp, filePath, dirLen + nameLen + 1,
                                      dirLen) != TCL_OK)
                    goto errorExit;
            } else {
                if (LoadOusterIndex (interp, filePath, dirLen) != TCL_OK)
                    goto errorExit;
            }
        }
    }

    if (filePath != NULL)
        ckfree (filePath);
    closedir (dirPtr);
    return TCL_OK;

  errorExit:
    if (filePath != NULL)
        ckfree (filePath);
    closedir (dirPtr);
    return TCL_ERROR;

}

/*
 *-----------------------------------------------------------------------------
 *
 * LoadPackageIndexes --
 *
 * Loads the all indexes for all package libraries (.tlib)* or a
 * Ousterhout "tclIndex" files found in all directories in the path.
 * If an index has already been loaded, it will not be reloaded.
 * Non-existent or unreadable directories are skipped.
 *
 * Results:
 *   A standard Tcl result.  Tcl array variable TCLENV is updated to
 * indicate the procedures that were defined in the library.
 *
 *-----------------------------------------------------------------------------
 */
static int
LoadPackageIndexes (interp, path)
    Tcl_Interp  *interp;
    char        *path;
{
    char  *dirName, dirNameBuf [64];
    int    idx, dirLen, pathArgc, status;
    char **pathArgv;

    if (Tcl_SplitList (interp, path, &pathArgc, &pathArgv) != TCL_OK)
        return TCL_OK;

    for (idx = 0; idx < pathArgc; idx++) {
        /*
         * Get the absolute dir name.  if the conversion fails (most likely
         * invalid "~") or thje directory cann't be read, skip it.
         */
        dirName = pathArgv [idx];
        if (dirName [0] != '/') {
            dirName = MakeAbsFile (interp, dirName, dirNameBuf, 
                                   sizeof (dirNameBuf));
            if (dirName == NULL)
                continue;
        }
        if (access (dirName, X_OK) == 0)
            status = LoadDirIndexes (interp, dirName);
        else
            status = TCL_OK;

        if ((dirName != pathArgv [idx]) && (dirName != dirNameBuf))
            ckfree (dirName);
        if (status != TCL_OK)
            goto errorExit;
    }
    ckfree (pathArgv);
    return TCL_OK;

  errorExit:
    ckfree (pathArgv);
    return TCL_ERROR;

}

/*
 *-----------------------------------------------------------------------------
 *
 * LoadProc --
 *
 *    Attempt to load a procedure (or command) by checking the TCLENV 
 * array for its location (either in a file or package library).
 *
 * Parameters
 *   o interp (I) - A pointer to the interpreter, error returned in result.
 *   o procName (I) - The name of the procedure (or command) to load
 *     libraries.
 *   o foundPtr (O) - TRUE is returned if the procedure or command was
 *     loaded, FALSE if it was not.
 * Results:
 *   A standard Tcl result.
 *
 *-----------------------------------------------------------------------------
 */
static int
LoadProc (interp, procName, foundPtr)
    Tcl_Interp  *interp;
    char        *procName;
    int         *foundPtr;
{
    Interp        *iPtr = (Interp *) interp;
    char           type, *location, *filePath, *cmdPtr, cmdBuf [80];
    int            cmdLen, result;
    long           offset;
    unsigned       length;
    fileId_t       fileId;
    Tcl_HashEntry *cmdEntryPtr;

    if (GetTCLENVProcEntry (interp, procName, &type, &location) != TCL_OK)
        return TCL_ERROR;
    if (location == NULL) {
        *foundPtr = FALSE;
        return TCL_OK;
    }

    /*
     * If this is a file entry (type = 'F'), location is a file name or
     * absolute file path.  If it's an absolute path, just eval it, otherwise
     * load the source using the "load" procdure (still in Tcl). If this is a
     * package entry, location is a package name. Source part of the package
     * library (Must look up the file, offset and length in the package entry
     * in TCLENV).
     */
    if (type == 'F') {
        if (location [0] == '/') {
            result = Tcl_EvalFile (interp, location);
        } else {
            cmdLen = strlen (location) + 5;
            if (cmdLen < sizeof (cmdBuf))
                cmdPtr = cmdBuf;
            else
                cmdPtr = ckalloc (cmdLen + 1);
            strcpy (cmdPtr, "load ");
            strcat (cmdPtr, location);

            result = Tcl_Eval (interp, cmdPtr, 0, NULL);
            if (cmdPtr != cmdBuf)
                ckfree (cmdPtr);
        }
    } else {
        result = GetTCLENVPkgEntry (interp, location, fileId, &offset,
                                    &length);
        if (result == TCL_OK) {
            filePath = GetTCLENVFileIdEntry (interp, fileId);
            if (filePath == NULL)
                result = TCL_ERROR;
        }
        
        if (result == TCL_OK)
            result = EvalFilePart (interp, filePath, offset, length);

    }

    ckfree (location);
    
    /*
     * If we are ok to this point, make sure that the procedure or command is
     * actually loaded.
     */
    if (result == TCL_OK) {
        cmdEntryPtr = Tcl_FindHashEntry (&iPtr->commandTable, procName);
        *foundPtr = (cmdEntryPtr != NULL);
    }

    return result;
}

/*
 *-----------------------------------------------------------------------------
 *
 * Tcl_LoadlibindexCmd --
 *
 *   This procedure is invoked to process the "Loadlibindex" Tcl command:
 *
 *      loadlibindex libfile
 *
 * which loads the index for a package library (.tlib) or a Ousterhout
 * "tclIndex" file.
 *
 * Results:
 *    A standard Tcl result.  Tcl array variable TCLENV is updated to
 * indicate the procedures that were defined in the library.
 *
 *-----------------------------------------------------------------------------
 */
int
Tcl_LoadlibindexCmd (dummy, interp, argc, argv)
    ClientData   dummy;
    Tcl_Interp  *interp;
    int          argc;
    char       **argv;
{
    char *pathName, pathNameBuf [64];
    int   pathLen, dirLen;

    if (argc != 2) {
        Tcl_AppendResult (interp, tclXWrongArgs, argv [0], " libFile",
                          (char *) NULL);
        return TCL_ERROR;
    }

    pathName = argv [1];
    if (pathName [0] != '/') {
        pathName = MakeAbsFile (interp, pathName, pathNameBuf, 
                                sizeof (pathNameBuf));
        if (pathName == NULL)
            return TCL_ERROR;
    }

    /*
     * Find the length of the directory name. Validate that we have a .tlib
     * extension or file name is "tclIndex" and call the routine to process
     * the specific type of index.
     */
    pathLen = strlen (pathName);
    for (dirLen = pathLen - 1; pathName [dirLen] != '/'; dirLen--)
        continue;

    if ((pathLen > 5) && (pathName [pathLen - 5] == '.')) {
        if (!STREQU (pathName + pathLen - 5, ".tlib"))
            goto invalidName;
        if (LoadPackageIndex (interp, pathName, pathLen, dirLen) != TCL_OK)
            goto errorExit;
    } else {
        if (!STREQU (pathName + dirLen, "/tclIndex"))
            goto invalidName;
        if (LoadOusterIndex (interp, pathName, dirLen) != TCL_OK)
            goto errorExit;
    }
    if ((pathName != argv [1]) && (pathName != pathNameBuf))
        ckfree (pathName);
    return TCL_OK;

  invalidName:
    Tcl_AppendResult (interp, "invalid library name, must have an extension ",
                      "of \".tlib\" or the name \"tclIndex\", got \"",
                      argv [1], "\"", (char *) NULL);

  errorExit:
    if ((pathName != argv [1]) && (pathName != pathNameBuf))
        ckfree (pathName);
    return TCL_ERROR;;
}

/*
 *-----------------------------------------------------------------------------
 *
 * Tcl_Demand_loadCmd --
 *
 *   This procedure is invoked to process the "demand_load" Tcl command:
 *
 *         demand_load proc
 *
 * which searchs the TCLENV tables for the specified procedure.  If it
 * is not found, an attempt is made to load unloaded libraries, first
 * the variable "TCLPATH" is searched.  If the procedure is not defined
 * after that, then "auto_path" is searched.
 *
 * Results:
 *   A standard Tcl result.
 *
 *-----------------------------------------------------------------------------
 */
int
Tcl_Demand_loadCmd (dummy, interp, argc, argv)
    ClientData   dummy;
    Tcl_Interp  *interp;
    int          argc;
    char       **argv;
{
    int   found;
    char *path, *msg;

    if (argc != 2) {
        Tcl_AppendResult (interp, tclXWrongArgs, argv [0], " procedure",
                          (char *) NULL);
        return TCL_ERROR;
    }

    if (LoadProc (interp, argv [1], &found) != TCL_OK)
        goto errorExit;
    if (found) {
        interp->result = "1";
        return TCL_OK;
    }

    /*
     * Slow path, load the libraries indices on "TCLPATH".
     */
    path = Tcl_GetVar (interp, "TCLPATH", TCL_GLOBAL_ONLY);
    if (path != NULL) {
        if (LoadPackageIndexes (interp, path) != TCL_OK)
            goto errorExit;
        if (LoadProc (interp, argv [1], &found) != TCL_OK)
            goto errorExit;
        if (found) {
            interp->result = "1";
            return TCL_OK;
        }
    }

    /*
     * Final gasp, check the "auto_path"
     */
    path = Tcl_GetVar (interp, "auto_path", TCL_GLOBAL_ONLY);
    if (path != NULL) {
        if (LoadPackageIndexes (interp, path) != TCL_OK)
            goto errorExit;
        if (LoadProc (interp, argv [1], &found) != TCL_OK)
            goto errorExit;
        if (found) {
            interp->result = "1";
            return TCL_OK;
        }
    }

    /*
     * Procedure or command was not found.
     */
    interp->result = "0";
    return TCL_OK;

  errorExit:
    msg = ckalloc (strlen (argv [1]) + 35);
    strcpy (msg, "\n    while demand loading \"");
    strcat (msg, argv [1]);
    strcat (msg, "\"");
    Tcl_AddErrorInfo (interp, msg);
    ckfree (msg);
    return TCL_ERROR;
}

