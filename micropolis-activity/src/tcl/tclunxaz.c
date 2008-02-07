/* 
 * tclUnixAZ.c --
 *
 *	This file contains the top-level command procedures for
 *	commands in the Tcl core that require UNIX facilities
 *	such as files and process execution.  Much of the code
 *	in this file is based on earlier versions contributed
 *	by Karl Lehenbauer, Mark Diekhans and Peter da Silva.
 *
 * Copyright 1991 Regents of the University of California
 * Permission to use, copy, modify, and distribute this
 * software and its documentation for any purpose and without
 * fee is hereby granted, provided that this copyright
 * notice appears in all copies.  The University of California
 * makes no representations about the suitability of this
 * software for any purpose.  It is provided "as is" without
 * express or implied warranty.
 */

#ifndef lint
static char rcsid[] = "$Header: /user6/ouster/tcl/RCS/tclUnixAZ.c,v 1.36 92/04/16 13:32:02 ouster Exp $ sprite (Berkeley)";
#endif /* not lint */

#include "tclint.h"
#include "tclunix.h"

/*
 * The variable below caches the name of the current working directory
 * in order to avoid repeated calls to getwd.  The string is malloc-ed.
 * NULL means the cache needs to be refreshed.
 */

static char *currentDir =  NULL;

/*
 * Prototypes for local procedures defined in this file:
 */

static int		CleanupChildren _ANSI_ARGS_((Tcl_Interp *interp,
			    int numPids, int *pidPtr, int errorId));
static char *		GetFileType _ANSI_ARGS_((int mode));
static int		StoreStatData _ANSI_ARGS_((Tcl_Interp *interp,
			    char *varName, struct stat *statPtr));

/*
 *----------------------------------------------------------------------
 *
 * Tcl_CdCmd --
 *
 *	This procedure is invoked to process the "cd" Tcl command.
 *	See the user documentation for details on what it does.
 *
 * Results:
 *	A standard Tcl result.
 *
 * Side effects:
 *	See the user documentation.
 *
 *----------------------------------------------------------------------
 */

	/* ARGSUSED */
int
Tcl_CdCmd(dummy, interp, argc, argv)
    ClientData dummy;			/* Not used. */
    Tcl_Interp *interp;			/* Current interpreter. */
    int argc;				/* Number of arguments. */
    char **argv;			/* Argument strings. */
{
    char *dirName;

    if (argc > 2) {
	Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
		" dirName\"", (char *) NULL);
	return TCL_ERROR;
    }

    if (argc == 2) {
	dirName = argv[1];
    } else {
	dirName = "~";
    }
    dirName = Tcl_TildeSubst(interp, dirName);
    if (dirName == NULL) {
	return TCL_ERROR;
    }
    if (currentDir != NULL) {
	ckfree(currentDir);
	currentDir = NULL;
    }
    if (chdir(dirName) != 0) {
	Tcl_AppendResult(interp, "couldn't change working directory to \"",
		dirName, "\": ", Tcl_UnixError(interp), (char *) NULL);
	return TCL_ERROR;
    }
    return TCL_OK;
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_CloseCmd --
 *
 *	This procedure is invoked to process the "close" Tcl command.
 *	See the user documentation for details on what it does.
 *
 * Results:
 *	A standard Tcl result.
 *
 * Side effects:
 *	See the user documentation.
 *
 *----------------------------------------------------------------------
 */

	/* ARGSUSED */
int
Tcl_CloseCmd(dummy, interp, argc, argv)
    ClientData dummy;			/* Not used. */
    Tcl_Interp *interp;			/* Current interpreter. */
    int argc;				/* Number of arguments. */
    char **argv;			/* Argument strings. */
{
    OpenFile *filePtr;
    int result = TCL_OK;

    if (argc != 2) {
	Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
		" fileId\"", (char *) NULL);
	return TCL_ERROR;
    }
    if (TclGetOpenFile(interp, argv[1], &filePtr) != TCL_OK) {
	return TCL_ERROR;
    }
    ((Interp *) interp)->filePtrArray[fileno(filePtr->f)] = NULL;

    /*
     * First close the file (in the case of a process pipeline, there may
     * be two files, one for the pipe at each end of the pipeline).
     */

    if (filePtr->f2 != NULL) {
	if (fclose(filePtr->f2) == EOF) {
	    Tcl_AppendResult(interp, "error closing \"", argv[1],
		    "\": ", Tcl_UnixError(interp), "\n", (char *) NULL);
	    result = TCL_ERROR;
	}
    }
    if (fclose(filePtr->f) == EOF) {
	Tcl_AppendResult(interp, "error closing \"", argv[1],
		"\": ", Tcl_UnixError(interp), "\n", (char *) NULL);
	result = TCL_ERROR;
    }

    /*
     * If the file was a connection to a pipeline, clean up everything
     * associated with the child processes.
     */

    if (filePtr->numPids > 0) {
	if (CleanupChildren(interp, filePtr->numPids, filePtr->pidPtr,
		filePtr->errorId) != TCL_OK) {
	    result = TCL_ERROR;
	}
    }

    ckfree((char *) filePtr);
    return result;
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_EofCmd --
 *
 *	This procedure is invoked to process the "eof" Tcl command.
 *	See the user documentation for details on what it does.
 *
 * Results:
 *	A standard Tcl result.
 *
 * Side effects:
 *	See the user documentation.
 *
 *----------------------------------------------------------------------
 */

	/* ARGSUSED */
int
Tcl_EofCmd(notUsed, interp, argc, argv)
    ClientData notUsed;			/* Not used. */
    Tcl_Interp *interp;			/* Current interpreter. */
    int argc;				/* Number of arguments. */
    char **argv;			/* Argument strings. */
{
    OpenFile *filePtr;

    if (argc != 2) {
	Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
		" fileId\"", (char *) NULL);
	return TCL_ERROR;
    }
    if (TclGetOpenFile(interp, argv[1], &filePtr) != TCL_OK) {
	return TCL_ERROR;
    }
    if (feof(filePtr->f)) {
	interp->result = "1";
    } else {
	interp->result = "0";
    }
    return TCL_OK;
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_ExecCmd --
 *
 *	This procedure is invoked to process the "exec" Tcl command.
 *	See the user documentation for details on what it does.
 *
 * Results:
 *	A standard Tcl result.
 *
 * Side effects:
 *	See the user documentation.
 *
 *----------------------------------------------------------------------
 */

	/* ARGSUSED */
int
Tcl_ExecCmd(dummy, interp, argc, argv)
    ClientData dummy;			/* Not used. */
    Tcl_Interp *interp;			/* Current interpreter. */
    int argc;				/* Number of arguments. */
    char **argv;			/* Argument strings. */
{
    int outputId;			/* File id for output pipe.  -1
					 * means command overrode. */
    int errorId;			/* File id for temporary file
					 * containing error output. */
    int *pidPtr;
    int numPids, result;

    /*
     * See if the command is to be run in background;  if so, create
     * the command, detach it, and return.
     */

    if ((argv[argc-1][0] == '&') && (argv[argc-1][1] == 0)) {
	argc--;
	argv[argc] = NULL;
	numPids = Tcl_CreatePipeline(interp, argc-1, argv+1, &pidPtr,
		(int *) NULL, (int *) NULL, (int *) NULL);
	if (numPids < 0) {
	    return TCL_ERROR;
	}
	Tcl_DetachPids(numPids, pidPtr);
	ckfree((char *) pidPtr);
	return TCL_OK;
    }

    /*
     * Create the command's pipeline.
     */

    numPids = Tcl_CreatePipeline(interp, argc-1, argv+1, &pidPtr,
	    (int *) NULL, &outputId, &errorId);
    if (numPids < 0) {
	return TCL_ERROR;
    }

    /*
     * Read the child's output (if any) and put it into the result.
     */

    result = TCL_OK;
    if (outputId != -1) {
	while (1) {
#	    define BUFFER_SIZE 1000
	    char buffer[BUFFER_SIZE+1];
	    int count;
    
	    count = read(outputId, buffer, BUFFER_SIZE);
    
	    if (count == 0) {
		break;
	    }
	    if (count < 0) {
		Tcl_ResetResult(interp);
		Tcl_AppendResult(interp,
			"error reading from output pipe: ",
			Tcl_UnixError(interp), (char *) NULL);
		result = TCL_ERROR;
		break;
	    }
	    buffer[count] = 0;
	    Tcl_AppendResult(interp, buffer, (char *) NULL);
	}
	close(outputId);
    }

    if (CleanupChildren(interp, numPids, pidPtr, errorId) != TCL_OK) {
	result = TCL_ERROR;
    }
    return result;
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_ExitCmd --
 *
 *	This procedure is invoked to process the "exit" Tcl command.
 *	See the user documentation for details on what it does.
 *
 * Results:
 *	A standard Tcl result.
 *
 * Side effects:
 *	See the user documentation.
 *
 *----------------------------------------------------------------------
 */

	/* ARGSUSED */
int
Tcl_ExitCmd(dummy, interp, argc, argv)
    ClientData dummy;			/* Not used. */
    Tcl_Interp *interp;			/* Current interpreter. */
    int argc;				/* Number of arguments. */
    char **argv;			/* Argument strings. */
{
    int value;

    if ((argc != 1) && (argc != 2)) {
	Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
		" ?returnCode?\"", (char *) NULL);
	return TCL_ERROR;
    }
    if (argc == 1) {
	exit(0);
    }
    if (Tcl_GetInt(interp, argv[1], &value) != TCL_OK) {
	return TCL_ERROR;
    }
    exit(value);
#if 0
    return TCL_OK;			/* Better not ever reach this! */
#endif
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_FileCmd --
 *
 *	This procedure is invoked to process the "file" Tcl command.
 *	See the user documentation for details on what it does.
 *
 * Results:
 *	A standard Tcl result.
 *
 * Side effects:
 *	See the user documentation.
 *
 *----------------------------------------------------------------------
 */

	/* ARGSUSED */
int
Tcl_FileCmd(dummy, interp, argc, argv)
    ClientData dummy;			/* Not used. */
    Tcl_Interp *interp;			/* Current interpreter. */
    int argc;				/* Number of arguments. */
    char **argv;			/* Argument strings. */
{
    char *p;
    int length, statOp;
    int mode = 0;			/* Initialized only to prevent
					 * compiler warning message. */
    struct stat statBuf;
    char *fileName, c;

    if (argc < 3) {
	Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
		" option name ?arg ...?\"", (char *) NULL);
	return TCL_ERROR;
    }
    c = argv[1][0];
    length = strlen(argv[1]);

    /*
     * First handle operations on the file name.
     */

    fileName = Tcl_TildeSubst(interp, argv[2]);
    if (fileName == NULL) {
	return TCL_ERROR;
    }
    if ((c == 'd') && (strncmp(argv[1], "dirname", length) == 0)) {
	if (argc != 3) {
	    argv[1] = "dirname";
	    not3Args:
	    Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
		    " ", argv[1], " name\"", (char *) NULL);
	    return TCL_ERROR;
	}
#ifdef MSDOS
	p = strrchr(fileName, '\\');
#else
	p = strrchr(fileName, '/');
#endif
	if (p == NULL) {
	    interp->result = ".";
	} else if (p == fileName) {
#ifdef MSDOS
	    interp->result = "\\";
#else
	    interp->result = "/";
#endif
	} else {
	    *p = 0;
	    Tcl_SetResult(interp, fileName, TCL_VOLATILE);
	    *p = '/';
	}
	return TCL_OK;
    } else if ((c == 'r') && (strncmp(argv[1], "rootname", length) == 0)
	    && (length >= 2)) {
	char *lastSlash;

	if (argc != 3) {
	    argv[1] = "rootname";
	    goto not3Args;
	}
	p = strrchr(fileName, '.');
#ifdef MSDOS
	lastSlash = strrchr(fileName, '\\');
#else
	lastSlash = strrchr(fileName, '/');
#endif
	if ((p == NULL) || ((lastSlash != NULL) && (lastSlash > p))) {
	    Tcl_SetResult(interp, fileName, TCL_VOLATILE);
	} else {
	    *p = 0;
	    Tcl_SetResult(interp, fileName, TCL_VOLATILE);
	    *p = '.';
	}
	return TCL_OK;
    } else if ((c == 'e') && (strncmp(argv[1], "extension", length) == 0)
	    && (length >= 3)) {
	char *lastSlash;

	if (argc != 3) {
	    argv[1] = "extension";
	    goto not3Args;
	}
	p = strrchr(fileName, '.');
#ifdef MSDOS
	lastSlash = strrchr(fileName, '\\');
#else
	lastSlash = strrchr(fileName, '/');
#endif
	if ((p != NULL) && ((lastSlash == NULL) || (lastSlash < p))) {
	    Tcl_SetResult(interp, p, TCL_VOLATILE);
	}
	return TCL_OK;
    } else if ((c == 't') && (strncmp(argv[1], "tail", length) == 0)
	    && (length >= 2)) {
	if (argc != 3) {
	    argv[1] = "tail";
	    goto not3Args;
	}
#ifdef MSDOS
	p = strrchr(fileName, '\\');
#else
	p = strrchr(fileName, '/');
#endif
	if (p != NULL) {
	    Tcl_SetResult(interp, p+1, TCL_VOLATILE);
	} else {
	    Tcl_SetResult(interp, fileName, TCL_VOLATILE);
	}
	return TCL_OK;
    }

    /*
     * Next, handle operations that can be satisfied with the "access"
     * kernel call.
     */

    if (fileName == NULL) {
	return TCL_ERROR;
    }
    if ((c == 'r') && (strncmp(argv[1], "readable", length) == 0)
	    && (length >= 5)) {
	if (argc != 3) {
	    argv[1] = "readable";
	    goto not3Args;
	}
	mode = R_OK;
	checkAccess:
	if (access(fileName, mode) == -1) {
	    interp->result = "0";
	} else {
	    interp->result = "1";
	}
	return TCL_OK;
    } else if ((c == 'w') && (strncmp(argv[1], "writable", length) == 0)) {
	if (argc != 3) {
	    argv[1] = "writable";
	    goto not3Args;
	}
	mode = W_OK;
	goto checkAccess;
    } else if ((c == 'e') && (strncmp(argv[1], "executable", length) == 0)
	    && (length >= 3)) {
	if (argc != 3) {
	    argv[1] = "executable";
	    goto not3Args;
	}
	mode = X_OK;
	goto checkAccess;
    } else if ((c == 'e') && (strncmp(argv[1], "exists", length) == 0)
	    && (length >= 3)) {
	if (argc != 3) {
	    argv[1] = "exists";
	    goto not3Args;
	}
	mode = F_OK;
	goto checkAccess;
    }

    /*
     * Lastly, check stuff that requires the file to be stat-ed.
     */

    if ((c == 'a') && (strncmp(argv[1], "atime", length) == 0)) {
	if (argc != 3) {
	    argv[1] = "atime";
	    goto not3Args;
	}
	if (stat(fileName, &statBuf) == -1) {
	    goto badStat;
	}
	sprintf(interp->result, "%ld", statBuf.st_atime);
	return TCL_OK;
    } else if ((c == 'i') && (strncmp(argv[1], "isdirectory", length) == 0)
	    && (length >= 3)) {
	if (argc != 3) {
	    argv[1] = "isdirectory";
	    goto not3Args;
	}
	statOp = 2;
    } else if ((c == 'i') && (strncmp(argv[1], "isfile", length) == 0)
	    && (length >= 3)) {
	if (argc != 3) {
	    argv[1] = "isfile";
	    goto not3Args;
	}
	statOp = 1;
    } else if ((c == 'l') && (strncmp(argv[1], "lstat", length) == 0)) {
	if (argc != 4) {
	    Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
		    " lstat name varName\"", (char *) NULL);
	    return TCL_ERROR;
	}

	if (lstat(fileName, &statBuf) == -1) {
	    Tcl_AppendResult(interp, "couldn't lstat \"", argv[2],
		    "\": ", Tcl_UnixError(interp), (char *) NULL);
	    return TCL_ERROR;
	}
	return StoreStatData(interp, argv[3], &statBuf);
    } else if ((c == 'm') && (strncmp(argv[1], "mtime", length) == 0)) {
	if (argc != 3) {
	    argv[1] = "mtime";
	    goto not3Args;
	}
	if (stat(fileName, &statBuf) == -1) {
	    goto badStat;
	}
	sprintf(interp->result, "%ld", statBuf.st_mtime);
	return TCL_OK;
    } else if ((c == 'o') && (strncmp(argv[1], "owned", length) == 0)) {
	if (argc != 3) {
	    argv[1] = "owned";
	    goto not3Args;
	}
	statOp = 0;
#ifdef S_IFLNK
    /*
     * This option is only included if symbolic links exist on this system
     * (in which case S_IFLNK should be defined).
     */
    } else if ((c == 'r') && (strncmp(argv[1], "readlink", length) == 0)
	    && (length >= 5)) {
	char linkValue[MAXPATHLEN+1];
	int linkLength;

	if (argc != 3) {
	    argv[1] = "readlink";
	    goto not3Args;
	}
	linkLength = readlink(fileName, linkValue, sizeof(linkValue) - 1);
	if (linkLength == -1) {
	    Tcl_AppendResult(interp, "couldn't readlink \"", argv[2],
		    "\": ", Tcl_UnixError(interp), (char *) NULL);
	    return TCL_ERROR;
	}
	linkValue[linkLength] = 0;
	Tcl_SetResult(interp, linkValue, TCL_VOLATILE);
	return TCL_OK;
#endif
    } else if ((c == 's') && (strncmp(argv[1], "size", length) == 0)
	    && (length >= 2)) {
	if (argc != 3) {
	    argv[1] = "size";
	    goto not3Args;
	}
	if (stat(fileName, &statBuf) == -1) {
	    goto badStat;
	}
	sprintf(interp->result, "%ld", statBuf.st_size);
	return TCL_OK;
    } else if ((c == 's') && (strncmp(argv[1], "stat", length) == 0)
	    && (length >= 2)) {
	if (argc != 4) {
	    Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
		    " stat name varName\"", (char *) NULL);
	    return TCL_ERROR;
	}

	if (stat(fileName, &statBuf) == -1) {
	    badStat:
	    Tcl_AppendResult(interp, "couldn't stat \"", argv[2],
		    "\": ", Tcl_UnixError(interp), (char *) NULL);
	    return TCL_ERROR;
	}
	return StoreStatData(interp, argv[3], &statBuf);
    } else if ((c == 't') && (strncmp(argv[1], "type", length) == 0)
	    && (length >= 2)) {
	if (argc != 3) {
	    argv[1] = "type";
	    goto not3Args;
	}
	if (lstat(fileName, &statBuf) == -1) {
	    goto badStat;
	}
	interp->result = GetFileType((int) statBuf.st_mode);
	return TCL_OK;
    } else {
	Tcl_AppendResult(interp, "bad option \"", argv[1],
		"\": should be atime, dirname, executable, exists, ",
		"extension, isdirectory, isfile, lstat, mtime, owned, ",
		"readable, ",
#ifdef S_IFLNK
		"readlink, ",
#endif
		"root, size, stat, tail, type, ",
		"or writable",
		(char *) NULL);
	return TCL_ERROR;
    }
    if (stat(fileName, &statBuf) == -1) {
	interp->result = "0";
	return TCL_OK;
    }
    switch (statOp) {
	case 0:
	    mode = (geteuid() == statBuf.st_uid);
	    break;
	case 1:
	    mode = S_ISREG(statBuf.st_mode);
	    break;
	case 2:
	    mode = S_ISDIR(statBuf.st_mode);
	    break;
    }
    if (mode) {
	interp->result = "1";
    } else {
	interp->result = "0";
    }
    return TCL_OK;
}

/*
 *----------------------------------------------------------------------
 *
 * StoreStatData --
 *
 *	This is a utility procedure that breaks out the fields of a
 *	"stat" structure and stores them in textual form into the
 *	elements of an associative array.
 *
 * Results:
 *	Returns a standard Tcl return value.  If an error occurs then
 *	a message is left in interp->result.
 *
 * Side effects:
 *	Elements of the associative array given by "varName" are modified.
 *
 *----------------------------------------------------------------------
 */

static int
StoreStatData(interp, varName, statPtr)
    Tcl_Interp *interp;			/* Interpreter for error reports. */
    char *varName;			/* Name of associative array variable
					 * in which to store stat results. */
    struct stat *statPtr;		/* Pointer to buffer containing
					 * stat data to store in varName. */
{
    char string[30];

    sprintf(string, "%d", statPtr->st_dev);
    if (Tcl_SetVar2(interp, varName, "dev", string, TCL_LEAVE_ERR_MSG)
	    == NULL) {
	return TCL_ERROR;
    }
    sprintf(string, "%d", statPtr->st_ino);
    if (Tcl_SetVar2(interp, varName, "ino", string, TCL_LEAVE_ERR_MSG)
	    == NULL) {
	return TCL_ERROR;
    }
    sprintf(string, "%d", statPtr->st_mode);
    if (Tcl_SetVar2(interp, varName, "mode", string, TCL_LEAVE_ERR_MSG)
	    == NULL) {
	return TCL_ERROR;
    }
    sprintf(string, "%d", statPtr->st_nlink);
    if (Tcl_SetVar2(interp, varName, "nlink", string, TCL_LEAVE_ERR_MSG)
	    == NULL) {
	return TCL_ERROR;
    }
    sprintf(string, "%d", statPtr->st_uid);
    if (Tcl_SetVar2(interp, varName, "uid", string, TCL_LEAVE_ERR_MSG)
	    == NULL) {
	return TCL_ERROR;
    }
    sprintf(string, "%d", statPtr->st_gid);
    if (Tcl_SetVar2(interp, varName, "gid", string, TCL_LEAVE_ERR_MSG)
	    == NULL) {
	return TCL_ERROR;
    }
    sprintf(string, "%ld", statPtr->st_size);
    if (Tcl_SetVar2(interp, varName, "size", string, TCL_LEAVE_ERR_MSG)
	    == NULL) {
	return TCL_ERROR;
    }
    sprintf(string, "%ld", statPtr->st_atime);
    if (Tcl_SetVar2(interp, varName, "atime", string, TCL_LEAVE_ERR_MSG)
	    == NULL) {
	return TCL_ERROR;
    }
    sprintf(string, "%ld", statPtr->st_mtime);
    if (Tcl_SetVar2(interp, varName, "mtime", string, TCL_LEAVE_ERR_MSG)
	    == NULL) {
	return TCL_ERROR;
    }
    sprintf(string, "%ld", statPtr->st_ctime);
    if (Tcl_SetVar2(interp, varName, "ctime", string, TCL_LEAVE_ERR_MSG)
	    == NULL) {
	return TCL_ERROR;
    }
    if (Tcl_SetVar2(interp, varName, "type",
	    GetFileType((int) statPtr->st_mode), TCL_LEAVE_ERR_MSG) == NULL) {
	return TCL_ERROR;
    }
    return TCL_OK;
}

/*
 *----------------------------------------------------------------------
 *
 * GetFileType --
 *
 *	Given a mode word, returns a string identifying the type of a
 *	file.
 *
 * Results:
 *	A static text string giving the file type from mode.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

static char *
GetFileType(mode)
    int mode;
{
    if (S_ISREG(mode)) {
	return "file";
    } else if (S_ISDIR(mode)) {
	return "directory";
    } else if (S_ISCHR(mode)) {
	return "characterSpecial";
    } else if (S_ISBLK(mode)) {
	return "blockSpecial";
    } else if (S_ISFIFO(mode)) {
	return "fifo";
    } else if (S_ISLNK(mode)) {
	return "link";
    } else if (S_ISSOCK(mode)) {
	return "socket";
    }
    return "unknown";
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_FlushCmd --
 *
 *	This procedure is invoked to process the "flush" Tcl command.
 *	See the user documentation for details on what it does.
 *
 * Results:
 *	A standard Tcl result.
 *
 * Side effects:
 *	See the user documentation.
 *
 *----------------------------------------------------------------------
 */

	/* ARGSUSED */
int
Tcl_FlushCmd(notUsed, interp, argc, argv)
    ClientData notUsed;			/* Not used. */
    Tcl_Interp *interp;			/* Current interpreter. */
    int argc;				/* Number of arguments. */
    char **argv;			/* Argument strings. */
{
    OpenFile *filePtr;
    FILE *f;

    if (argc != 2) {
	Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
		" fileId\"", (char *) NULL);
	return TCL_ERROR;
    }
    if (TclGetOpenFile(interp, argv[1], &filePtr) != TCL_OK) {
	return TCL_ERROR;
    }
    if (!filePtr->writable) {
	Tcl_AppendResult(interp, "\"", argv[1],
		"\" wasn't opened for writing", (char *) NULL);
	return TCL_ERROR;
    }
    f = filePtr->f2;
    if (f == NULL) {
	f = filePtr->f;
    }
    if (fflush(f) == EOF) {
	Tcl_AppendResult(interp, "error flushing \"", argv[1],
		"\": ", Tcl_UnixError(interp), (char *) NULL);
	clearerr(f);
	return TCL_ERROR;
    }
    return TCL_OK;
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_GetsCmd --
 *
 *	This procedure is invoked to process the "gets" Tcl command.
 *	See the user documentation for details on what it does.
 *
 * Results:
 *	A standard Tcl result.
 *
 * Side effects:
 *	See the user documentation.
 *
 *----------------------------------------------------------------------
 */

	/* ARGSUSED */
int
Tcl_GetsCmd(notUsed, interp, argc, argv)
    ClientData notUsed;			/* Not used. */
    Tcl_Interp *interp;			/* Current interpreter. */
    int argc;				/* Number of arguments. */
    char **argv;			/* Argument strings. */
{
#   define BUF_SIZE 200
    char buffer[BUF_SIZE+1];
    int totalCount, done, flags;
    OpenFile *filePtr;
    register FILE *f;

    if ((argc != 2) && (argc != 3)) {
	Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
		" fileId ?varName?\"", (char *) NULL);
	return TCL_ERROR;
    }
    if (TclGetOpenFile(interp, argv[1], &filePtr) != TCL_OK) {
	return TCL_ERROR;
    }
    if (!filePtr->readable) {
	Tcl_AppendResult(interp, "\"", argv[1],
		"\" wasn't opened for reading", (char *) NULL);
	return TCL_ERROR;
    }

    /*
     * We can't predict how large a line will be, so read it in
     * pieces, appending to the current result or to a variable.
     */

    totalCount = 0;
    done = 0;
    flags = 0;
    f = filePtr->f;
    while (!done) {
	register int c, count;
	register char *p;

	for (p = buffer, count = 0; count < BUF_SIZE-1; count++, p++) {
	    c = getc(f);
	    if (c == EOF) {
		if (ferror(filePtr->f)) {
		    Tcl_ResetResult(interp);
		    Tcl_AppendResult(interp, "error reading \"", argv[1],
			    "\": ", Tcl_UnixError(interp), (char *) NULL);
		    clearerr(filePtr->f);
		    return TCL_ERROR;
		} else if (feof(filePtr->f)) {
		    if ((totalCount == 0) && (count == 0)) {
			totalCount = -1;
		    }
		    done = 1;
		    break;
		}
	    }
	    if (c == '\n') {
		done = 1;
		break;
	    }
	    *p = c;
	}
	*p = 0;
	if (argc == 2) {
	    Tcl_AppendResult(interp, buffer, (char *) NULL);
	} else {
	    if (Tcl_SetVar(interp, argv[2], buffer, flags|TCL_LEAVE_ERR_MSG)
		    == NULL) {
		return TCL_ERROR;
	    }
	    flags = TCL_APPEND_VALUE;
	}
	totalCount += count;
    }

    if (argc == 3) {
	sprintf(interp->result, "%d", totalCount);
    }
    return TCL_OK;
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_OpenCmd --
 *
 *	This procedure is invoked to process the "open" Tcl command.
 *	See the user documentation for details on what it does.
 *
 * Results:
 *	A standard Tcl result.
 *
 * Side effects:
 *	See the user documentation.
 *
 *----------------------------------------------------------------------
 */

	/* ARGSUSED */
int
Tcl_OpenCmd(notUsed, interp, argc, argv)
    ClientData notUsed;			/* Not used. */
    Tcl_Interp *interp;			/* Current interpreter. */
    int argc;				/* Number of arguments. */
    char **argv;			/* Argument strings. */
{
    Interp *iPtr = (Interp *) interp;
    int pipeline, fd;
    char *access;
    register OpenFile *filePtr;

    if (argc == 2) {
	access = "r";
    } else if (argc == 3) {
	access = argv[2];
    } else {
	Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
		" filename ?access?\"", (char *) NULL);
	return TCL_ERROR;
    }

    filePtr = (OpenFile *) ckalloc(sizeof(OpenFile));
    filePtr->f = NULL;
    filePtr->f2 = NULL;
    filePtr->readable = 0;
    filePtr->writable = 0;
    filePtr->numPids = 0;
    filePtr->pidPtr = NULL;
    filePtr->errorId = -1;

    /*
     * Verify the requested form of access.
     */

    pipeline = 0;
    if (argv[1][0] == '|') {
	pipeline = 1;
    }
    switch (access[0]) {
	case 'r':
	    filePtr->readable = 1;
	    break;
	case 'w':
	    filePtr->writable = 1;
	    break;
	case 'a':
	    filePtr->writable = 1;
	    break;
	default:
	    badAccess:
	    Tcl_AppendResult(interp, "illegal access mode \"", access,
		    "\"", (char *) NULL);
	    goto error;
    }
    if (access[1] == '+') {
	filePtr->readable = filePtr->writable = 1;
	if (access[2] != 0) {
	    goto badAccess;
	}
    } else if (access[1] != 0) {
	goto badAccess;
    }

    /*
     * Open the file or create a process pipeline.
     */

    if (!pipeline) {
	char *fileName = argv[1];

	if (fileName[0] == '~') {
	    fileName = Tcl_TildeSubst(interp, fileName);
	    if (fileName == NULL) {
		goto error;
	    }
	}
	filePtr->f = fopen(fileName, access);
	if (filePtr->f == NULL) {
	    Tcl_AppendResult(interp, "couldn't open \"", argv[1],
		    "\": ", Tcl_UnixError(interp), (char *) NULL);
	    goto error;
	}
    } else {
	int *inPipePtr, *outPipePtr;
	int cmdArgc, inPipe, outPipe;
	char **cmdArgv;

	if (Tcl_SplitList(interp, argv[1]+1, &cmdArgc, &cmdArgv) != TCL_OK) {
	    goto error;
	}
	inPipePtr = (filePtr->writable) ? &inPipe : NULL;
	outPipePtr = (filePtr->readable) ? &outPipe : NULL;
	inPipe = outPipe = -1;
	filePtr->numPids = Tcl_CreatePipeline(interp, cmdArgc, cmdArgv,
		&filePtr->pidPtr, inPipePtr, outPipePtr, &filePtr->errorId);
	ckfree((char *) cmdArgv);
	if (filePtr->numPids < 0) {
	    goto error;
	}
	if (filePtr->readable) {
	    if (outPipe == -1) {
		if (inPipe != -1) {
		    close(inPipe);
		}
		Tcl_AppendResult(interp, "can't read output from command:",
			" standard output was redirected", (char *) NULL);
		goto error;
	    }
	    filePtr->f = fdopen(outPipe, "r");
	}
	if (filePtr->writable) {
	    if (inPipe == -1) {
		Tcl_AppendResult(interp, "can't write input to command:",
			" standard input was redirected", (char *) NULL);
		goto error;
	    }
	    if (filePtr->f != NULL) {
		filePtr->f2 = fdopen(inPipe, "w");
	    } else {
		filePtr->f = fdopen(inPipe, "w");
	    }
	}
    }

    /*
     * Enter this new OpenFile structure in the table for the
     * interpreter.  May have to expand the table to do this.
     */

    fd = fileno(filePtr->f);
    TclMakeFileTable(iPtr, fd);
    if (iPtr->filePtrArray[fd] != NULL) {
	panic("Tcl_OpenCmd found file already open");
    }
    iPtr->filePtrArray[fd] = filePtr;
    sprintf(interp->result, "file%d", fd);
    return TCL_OK;

    error:
    if (filePtr->f != NULL) {
	fclose(filePtr->f);
    }
    if (filePtr->f2 != NULL) {
	fclose(filePtr->f2);
    }
    if (filePtr->numPids > 0) {
	Tcl_DetachPids(filePtr->numPids, filePtr->pidPtr);
	ckfree((char *) filePtr->pidPtr);
    }
    if (filePtr->errorId != -1) {
	close(filePtr->errorId);
    }
    ckfree((char *) filePtr);
    return TCL_ERROR;
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_PwdCmd --
 *
 *	This procedure is invoked to process the "pwd" Tcl command.
 *	See the user documentation for details on what it does.
 *
 * Results:
 *	A standard Tcl result.
 *
 * Side effects:
 *	See the user documentation.
 *
 *----------------------------------------------------------------------
 */

	/* ARGSUSED */
int
Tcl_PwdCmd(dummy, interp, argc, argv)
    ClientData dummy;			/* Not used. */
    Tcl_Interp *interp;			/* Current interpreter. */
    int argc;				/* Number of arguments. */
    char **argv;			/* Argument strings. */
{
    char buffer[MAXPATHLEN+1];

    if (argc != 1) {
	Tcl_AppendResult(interp, "wrong # args: should be \"",
		argv[0], "\"", (char *) NULL);
	return TCL_ERROR;
    }
    if (currentDir == NULL) {
#if TCL_GETWD
	if (getwd(buffer) == NULL) {
	    Tcl_AppendResult(interp, "error getting working directory name: ",
		    buffer, (char *) NULL);
	    return TCL_ERROR;
	}
#else
	if (getcwd(buffer, MAXPATHLEN) == 0) {
	    if (errno == ERANGE) {
		interp->result = "working directory name is too long";
	    } else {
		Tcl_AppendResult(interp,
			"error getting working directory name: ",
			Tcl_UnixError(interp), (char *) NULL);
	    }
	    return TCL_ERROR;
	}
#endif
	currentDir = (char *) ckalloc((unsigned) (strlen(buffer) + 1));
	strcpy(currentDir, buffer);
    }
    interp->result = currentDir;
    return TCL_OK;
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_PutsCmd --
 *
 *	This procedure is invoked to process the "puts" Tcl command.
 *	See the user documentation for details on what it does.
 *
 * Results:
 *	A standard Tcl result.
 *
 * Side effects:
 *	See the user documentation.
 *
 *----------------------------------------------------------------------
 */

	/* ARGSUSED */
int
Tcl_PutsCmd(dummy, interp, argc, argv)
    ClientData dummy;			/* Not used. */
    Tcl_Interp *interp;			/* Current interpreter. */
    int argc;				/* Number of arguments. */
    char **argv;			/* Argument strings. */
{
    OpenFile *filePtr;
    FILE *f;

    if (argc == 4) {
	if (strncmp(argv[3], "nonewline", strlen(argv[3])) != 0) {
	    Tcl_AppendResult(interp, "bad argument \"", argv[3],
		    "\": should be \"nonewline\"", (char *) NULL);
	    return TCL_ERROR;
	}
    } else if (argc != 3) {
	Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
		" fileId string ?nonewline?\"", (char *) NULL);
	return TCL_ERROR;
    }
    if (TclGetOpenFile(interp, argv[1], &filePtr) != TCL_OK) {
	return TCL_ERROR;
    }
    if (!filePtr->writable) {
	Tcl_AppendResult(interp, "\"", argv[1],
		"\" wasn't opened for writing", (char *) NULL);
	return TCL_ERROR;
    }

    f = filePtr->f2;
    if (f == NULL) {
	f = filePtr->f;
    }
    fputs(argv[2], f);
    if (argc == 3) {
	fputc('\n', f);
    }
    if (ferror(f)) {
	Tcl_AppendResult(interp, "error writing \"", argv[1],
		"\": ", Tcl_UnixError(interp), (char *) NULL);
	clearerr(f);
	return TCL_ERROR;
    }
    return TCL_OK;
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_ReadCmd --
 *
 *	This procedure is invoked to process the "read" Tcl command.
 *	See the user documentation for details on what it does.
 *
 * Results:
 *	A standard Tcl result.
 *
 * Side effects:
 *	See the user documentation.
 *
 *----------------------------------------------------------------------
 */

	/* ARGSUSED */
int
Tcl_ReadCmd(dummy, interp, argc, argv)
    ClientData dummy;			/* Not used. */
    Tcl_Interp *interp;			/* Current interpreter. */
    int argc;				/* Number of arguments. */
    char **argv;			/* Argument strings. */
{
    OpenFile *filePtr;
    int bytesLeft, bytesRead, count;
#define READ_BUF_SIZE 4096
    char buffer[READ_BUF_SIZE+1];
    int newline;

    if ((argc != 2) && (argc != 3)) {
	Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
		" fileId ?numBytes|nonewline?\"", (char *) NULL);
	return TCL_ERROR;
    }
    if (TclGetOpenFile(interp, argv[1], &filePtr) != TCL_OK) {
	return TCL_ERROR;
    }
    if (!filePtr->readable) {
	Tcl_AppendResult(interp, "\"", argv[1],
		"\" wasn't opened for reading", (char *) NULL);
	return TCL_ERROR;
    }

    /*
     * Compute how many bytes to read, and see whether the final
     * newline should be dropped.
     */

    newline = 1;
    if ((argc > 2) && isdigit(argv[2][0])) {
	if (Tcl_GetInt(interp, argv[2], &bytesLeft) != TCL_OK) {
	    return TCL_ERROR;
	}
    } else {
	bytesLeft = 1<<30;
	if (argc > 2) {
	    if (strncmp(argv[2], "nonewline", strlen(argv[2])) == 0) {
		newline = 0;
	    } else {
		Tcl_AppendResult(interp, "bad argument \"", argv[2],
			"\": should be \"nonewline\"", (char *) NULL);
		return TCL_ERROR;
	    }
	}
    }

    /*
     * Read the file in one or more chunks.
     */

    bytesRead = 0;
    while (bytesLeft > 0) {
	count = READ_BUF_SIZE;
	if (bytesLeft < READ_BUF_SIZE) {
	    count = bytesLeft;
	}
	count = fread(buffer, 1, count, filePtr->f);
	if (ferror(filePtr->f)) {
	    Tcl_ResetResult(interp);
	    Tcl_AppendResult(interp, "error reading \"", argv[1],
		    "\": ", Tcl_UnixError(interp), (char *) NULL);
	    clearerr(filePtr->f);
	    return TCL_ERROR;
	}
	if (count == 0) {
	    break;
	}
	buffer[count] = 0;
	Tcl_AppendResult(interp, buffer, (char *) NULL);
	bytesLeft -= count;
	bytesRead += count;
    }
    if ((newline == 0) && (interp->result[bytesRead-1] == '\n')) {
	interp->result[bytesRead-1] = 0;
    }
    return TCL_OK;
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_SeekCmd --
 *
 *	This procedure is invoked to process the "seek" Tcl command.
 *	See the user documentation for details on what it does.
 *
 * Results:
 *	A standard Tcl result.
 *
 * Side effects:
 *	See the user documentation.
 *
 *----------------------------------------------------------------------
 */

	/* ARGSUSED */
int
Tcl_SeekCmd(notUsed, interp, argc, argv)
    ClientData notUsed;			/* Not used. */
    Tcl_Interp *interp;			/* Current interpreter. */
    int argc;				/* Number of arguments. */
    char **argv;			/* Argument strings. */
{
    OpenFile *filePtr;
    int offset, mode;

    if ((argc != 3) && (argc != 4)) {
	Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
		" fileId offset ?origin?\"", (char *) NULL);
	return TCL_ERROR;
    }
    if (TclGetOpenFile(interp, argv[1], &filePtr) != TCL_OK) {
	return TCL_ERROR;
    }
    if (Tcl_GetInt(interp, argv[2], &offset) != TCL_OK) {
	return TCL_ERROR;
    }
    mode = SEEK_SET;
    if (argc == 4) {
	int length;
	char c;

	length = strlen(argv[3]);
	c = argv[3][0];
	if ((c == 's') && (strncmp(argv[3], "start", length) == 0)) {
	    mode = SEEK_SET;
	} else if ((c == 'c') && (strncmp(argv[3], "current", length) == 0)) {
	    mode = SEEK_CUR;
	} else if ((c == 'e') && (strncmp(argv[3], "end", length) == 0)) {
	    mode = SEEK_END;
	} else {
	    Tcl_AppendResult(interp, "bad origin \"", argv[3],
		    "\": should be start, current, or end", (char *) NULL);
	    return TCL_ERROR;
	}
    }
    if (fseek(filePtr->f, offset, mode) == -1) {
	Tcl_AppendResult(interp, "error during seek: ",
		Tcl_UnixError(interp), (char *) NULL);
	clearerr(filePtr->f);
	return TCL_ERROR;
    }

    return TCL_OK;
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_SourceCmd --
 *
 *	This procedure is invoked to process the "source" Tcl command.
 *	See the user documentation for details on what it does.
 *
 * Results:
 *	A standard Tcl result.
 *
 * Side effects:
 *	See the user documentation.
 *
 *----------------------------------------------------------------------
 */

	/* ARGSUSED */
int
Tcl_SourceCmd(dummy, interp, argc, argv)
    ClientData dummy;			/* Not used. */
    Tcl_Interp *interp;			/* Current interpreter. */
    int argc;				/* Number of arguments. */
    char **argv;			/* Argument strings. */
{
    if (argc != 2) {
	Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
		" fileName\"", (char *) NULL);
	return TCL_ERROR;
    }
    return Tcl_EvalFile(interp, argv[1]);
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_TellCmd --
 *
 *	This procedure is invoked to process the "tell" Tcl command.
 *	See the user documentation for details on what it does.
 *
 * Results:
 *	A standard Tcl result.
 *
 * Side effects:
 *	See the user documentation.
 *
 *----------------------------------------------------------------------
 */

	/* ARGSUSED */
int
Tcl_TellCmd(notUsed, interp, argc, argv)
    ClientData notUsed;			/* Not used. */
    Tcl_Interp *interp;			/* Current interpreter. */
    int argc;				/* Number of arguments. */
    char **argv;			/* Argument strings. */
{
    OpenFile *filePtr;

    if (argc != 2) {
	Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
		" fileId\"", (char *) NULL);
	return TCL_ERROR;
    }
    if (TclGetOpenFile(interp, argv[1], &filePtr) != TCL_OK) {
	return TCL_ERROR;
    }
    sprintf(interp->result, "%d", ftell(filePtr->f));
    return TCL_OK;
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_TimeCmd --
 *
 *	This procedure is invoked to process the "time" Tcl command.
 *	See the user documentation for details on what it does.
 *
 * Results:
 *	A standard Tcl result.
 *
 * Side effects:
 *	See the user documentation.
 *
 *----------------------------------------------------------------------
 */

	/* ARGSUSED */
int
Tcl_TimeCmd(dummy, interp, argc, argv)
    ClientData dummy;			/* Not used. */
    Tcl_Interp *interp;			/* Current interpreter. */
    int argc;				/* Number of arguments. */
    char **argv;			/* Argument strings. */
{
    int count, i, result;
    double timePer;
#if TCL_GETTOD
    struct timeval start, stop;
    struct timezone tz;
    int micros;
#else
    struct tms dummy2;
    long start, stop;
#endif

    if (argc == 2) {
	count = 1;
    } else if (argc == 3) {
	if (Tcl_GetInt(interp, argv[2], &count) != TCL_OK) {
	    return TCL_ERROR;
	}
    } else {
	Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
		" command ?count?\"", (char *) NULL);
	return TCL_ERROR;
    }
#if TCL_GETTOD
    gettimeofday(&start, &tz);
#else
    start = times(&dummy2);
#endif
    for (i = count ; i > 0; i--) {
	result = Tcl_Eval(interp, argv[1], 0, (char **) NULL);
	if (result != TCL_OK) {
	    if (result == TCL_ERROR) {
		char msg[60];
		sprintf(msg, "\n    (\"time\" body line %d)",
			interp->errorLine);
		Tcl_AddErrorInfo(interp, msg);
	    }
	    return result;
	}
    }
#if TCL_GETTOD
    gettimeofday(&stop, &tz);
    micros = (stop.tv_sec - start.tv_sec)*1000000
	    + (stop.tv_usec - start.tv_usec);
    timePer = micros;
#else
    stop = times(&dummy2);
    timePer = (((double) (stop - start))*1000000.0)/CLK_TCK;
#endif
    Tcl_ResetResult(interp);
    sprintf(interp->result, "%.0f microseconds per iteration", timePer/count);
    return TCL_OK;
}

/*
 *----------------------------------------------------------------------
 *
 * CleanupChildren --
 *
 *	This is a utility procedure used to wait for child processes
 *	to exit, record information about abnormal exits, and then
 *	collect any stderr output generated by them.
 *
 * Results:
 *	The return value is a standard Tcl result.  If anything at
 *	weird happened with the child processes, TCL_ERROR is returned
 *	and a message is left in interp->result.
 *
 * Side effects:
 *	If the last character of interp->result is a newline, then it
 *	is removed.  File errorId gets closed, and pidPtr is freed
 *	back to the storage allocator.
 *
 *----------------------------------------------------------------------
 */

static int
CleanupChildren(interp, numPids, pidPtr, errorId)
    Tcl_Interp *interp;		/* Used for error messages. */
    int numPids;		/* Number of entries in pidPtr array. */
    int *pidPtr;		/* Array of process ids of children. */
    int errorId;		/* File descriptor index for file containing
				 * stderr output from pipeline.  -1 means
				 * there isn't any stderr output. */
{
    int result = TCL_OK;
    int i, pid, length;
    WAIT_STATUS_TYPE waitStatus;

    for (i = 0; i < numPids; i++) {
	pid = Tcl_WaitPids(1, &pidPtr[i], (int *) &waitStatus);
	if (pid == -1) {
	    Tcl_AppendResult(interp, "error waiting for process to exit: ",
		    Tcl_UnixError(interp), (char *) NULL);
	    continue;
	}

	/*
	 * Create error messages for unusual process exits.  An
	 * extra newline gets appended to each error message, but
	 * it gets removed below (in the same fashion that an
	 * extra newline in the command's output is removed).
	 */

	if (!WIFEXITED(waitStatus) || (WEXITSTATUS(waitStatus) != 0)) {
	    char msg1[20], msg2[20];

	    result = TCL_ERROR;
	    sprintf(msg1, "%d", pid);
	    if (WIFEXITED(waitStatus)) {
		sprintf(msg2, "%d", WEXITSTATUS(waitStatus));
		Tcl_SetErrorCode(interp, "CHILDSTATUS", msg1, msg2,
			(char *) NULL);
	    } else if (WIFSIGNALED(waitStatus)) {
		char *p;
	
		p = Tcl_SignalMsg((int) (WTERMSIG(waitStatus)));
		Tcl_SetErrorCode(interp, "CHILDKILLED", msg1,
			Tcl_SignalId((int) (WTERMSIG(waitStatus))), p,
			(char *) NULL);
		Tcl_AppendResult(interp, "child killed: ", p, "\n",
			(char *) NULL);
	    } else if (WIFSTOPPED(waitStatus)) {
		char *p;

		p = Tcl_SignalMsg((int) (WSTOPSIG(waitStatus)));
		Tcl_SetErrorCode(interp, "CHILDSUSP", msg1,
			Tcl_SignalId((int) (WSTOPSIG(waitStatus))), p, (char *) NULL);
		Tcl_AppendResult(interp, "child suspended: ", p, "\n",
			(char *) NULL);
	    } else {
		Tcl_AppendResult(interp,
			"child wait status didn't make sense\n",
			(char *) NULL);
	    }
	}
    }
    ckfree((char *) pidPtr);

    /*
     * Read the standard error file.  If there's anything there,
     * then return an error and add the file's contents to the result
     * string.
     */

    if (errorId >= 0) {
	while (1) {
#	    define BUFFER_SIZE 1000
	    char buffer[BUFFER_SIZE+1];
	    int count;
    
	    count = read(errorId, buffer, BUFFER_SIZE);
    
	    if (count == 0) {
		break;
	    }
	    if (count < 0) {
		Tcl_AppendResult(interp,
			"error reading stderr output file: ",
			Tcl_UnixError(interp), (char *) NULL);
		break;
	    }
	    buffer[count] = 0;
	    Tcl_AppendResult(interp, buffer, (char *) NULL);
	}
	close(errorId);
    }

    /*
     * If the last character of interp->result is a newline, then remove
     * the newline character (the newline would just confuse things).
     */

    length = strlen(interp->result);
    if ((length > 0) && (interp->result[length-1] == '\n')) {
	interp->result[length-1] = '\0';
    }

    return result;
}
