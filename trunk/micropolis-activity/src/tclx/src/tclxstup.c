/*
 * tclXstartup.c --
 *
 * Startup code for the Tcl shell and other interactive applications.  Also
 * create special commands used just by Tcl shell features.
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
 * $Id: tclXstartup.c,v 2.1 1992/11/10 04:02:06 markd Exp $
 *-----------------------------------------------------------------------------
 */

#include "tclxint.h"
#include "patchlvl.h"

extern char * etenv ();

extern char *optarg;
extern int   optind, opterr;

typedef struct tclParms_t {
    int       execFile;      /* Run the specified file. (no searching)       */
    int       execCommand;   /* Execute the specified command.               */
    unsigned  options;       /* Quick startup option.                        */
    char     *execStr;       /* Command file or command to execute.          */
    char    **tclArgv;       /* Arguments to pass to tcl script.             */
    int       tclArgc;       /* Count of arguments to pass to tcl script.    */
    char     *programName;   /* Name of program (less path).                 */
    } tclParms_t;

/*
 * Prototypes of internal functions.
 */
static void
ParseCmdArgs _ANSI_ARGS_((int          argc,
                          char       **argv,
                          tclParms_t  *tclParmsPtr));

static int
FindDefaultFile _ANSI_ARGS_((Tcl_Interp  *interp,
                             char        *defaultFile));

static int
ProcessDefaultFile _ANSI_ARGS_((Tcl_Interp  *interp,
                                char        *defaultFile));

static int
ProcessInitFile _ANSI_ARGS_((Tcl_Interp  *interp));


/*
 *-----------------------------------------------------------------------------
 *
 * Tcl_ErrorAbort --
 *
 * Display error information and abort when an error is returned in the
 * interp->result.
 *
 * Parameters:
 *   o interp - A pointer to the interpreter, should contain the
 *     error message in `result'.
 *   o exitCode - The code to pass to exit.
 *-----------------------------------------------------------------------------
 */
void
Tcl_ErrorAbort (interp, exitCode)
    Tcl_Interp  *interp;
    int          exitCode;
{
    char *errorStack;

    fflush (stdout);
    fprintf (stderr, "Error: %s\n", interp->result);

    errorStack = Tcl_GetVar (interp, "errorInfo", 1);
    if (errorStack != NULL)
        fprintf (stderr, "%s\n", errorStack);
    exit (exitCode);
}

/*
 *-----------------------------------------------------------------------------
 *
 * ParseCmdArgs --
 *
 * Parse the arguments passed to the Tcl shell
 *
 * Parameters:
 *   o argc, argv - Arguments passed to main.
 *   o tclParmsPtr - Results of the parsed Tcl shell command line.
 *-----------------------------------------------------------------------------
 */
static void
ParseCmdArgs (argc, argv, tclParmsPtr)
    int          argc;
    char       **argv;
    tclParms_t  *tclParmsPtr;
{
    char   *scanPtr, *programName;
    int     programNameLen;
    int     option;

    tclParmsPtr->execFile = FALSE;
    tclParmsPtr->execCommand = FALSE;
    tclParmsPtr->options = 0;
    tclParmsPtr->execStr = NULL;

    /*
     * Determine file name (less directories) that the Tcl interpreter is
     * being run under.
     */
    scanPtr = programName = argv[0];
    while (*scanPtr != '\0') {
        if (*scanPtr == '/')
            programName = scanPtr + 1;
        scanPtr++;
    }
    tclParmsPtr->programName = programName;
    programNameLen = strlen (programName);
    
    /*
     * Scan arguments looking for flags to process here rather than to pass
     * on to the scripts.  The '-c' or '-f' must also be the last option to
     * allow for script arguments starting with `-'.
     */
    while ((option = getopt (argc, argv, "qc:f:u")) != -1) {
        switch (option) {
            case 'q':
                if (tclParmsPtr->options & TCLSH_QUICK_STARTUP)
                    goto usageError;
                tclParmsPtr->options |= TCLSH_QUICK_STARTUP;
                break;
            case 'c':
                tclParmsPtr->execCommand = TRUE;
                tclParmsPtr->execStr = optarg;
                goto exitParse;
            case 'f':
                tclParmsPtr->execFile = TRUE;
                tclParmsPtr->execStr = optarg;
                goto exitParse;
            case 'u':
            default:
                goto usageError;
        }
    }
    exitParse:
  
    /*
     * If neither `-c' nor `-f' were specified and at least one parameter
     * is supplied, then if is the file to execute.  The rest of the arguments
     * are passed to the script.  Check for '--' as the last option, this also
     * is a terminator for the file to execute.
     */
    if ((!tclParmsPtr->execCommand) && (!tclParmsPtr->execFile) &&
        (optind != argc) && !STREQU (argv [optind-1], "--")) {
        tclParmsPtr->execFile = TRUE;
        tclParmsPtr->execStr = argv [optind];
        optind++;
    }

    tclParmsPtr->tclArgv = &argv [optind];
    tclParmsPtr->tclArgc = argc - optind;
    return;

usageError:
    fprintf (stderr, "usage: %s %s\n", argv [0],
             "[-qu] [[-f] script]|[-c command] [args]");
    exit (1);
}

/*
 *-----------------------------------------------------------------------------
 * FindDefaultFile --
 *
 *   Find the Tcl default file.  If is looked for in the following order:
 *       o A environment variable named `TCLDEFAULT'.
 *       o The specified defaultFile (which normally has an version number
 *         appended.
 *   A tcl variable `TCLDEFAULT', will contain the path of the default file
 *   to use after this procedure is executed, or a null string if it is not
 *   found.
 * Parameters
 *   o interp (I) - A pointer to the interpreter.
 *   o defaultFile (I) - The file name of the default file to use, it
 *     normally contains a version number.
 * Returns:
 *     TCL_OK if all is ok, TCL_ERROR if a error occured.
 *-----------------------------------------------------------------------------
 */
static int
FindDefaultFile (interp, defaultFile)
    Tcl_Interp  *interp;
    char        *defaultFile;
{
    char        *defaultFileToUse;
    struct stat  statBuf;

    if ((defaultFileToUse = getenv ("TCLDEFAULT")) == NULL)
        defaultFileToUse = defaultFile;

    if (stat (defaultFileToUse, &statBuf) < 0)
        defaultFileToUse = "";
    if (Tcl_SetVar (interp, "TCLDEFAULT", defaultFileToUse,
                    TCL_GLOBAL_ONLY | TCL_LEAVE_ERR_MSG) == NULL)
        return TCL_ERROR;
    else
        return TCL_OK;
}

/*
 *-----------------------------------------------------------------------------
 * ProcessDefaultFile --
 *
 *   Process the Tcl default file and TclInit files.  The default file
 * is the only file at a fixed path. It is a script file that usaually 
 * defines a variable "TCLINIT", which has the path of the  full
 * initialization file. The default file can also set things such as path
 * variables.  If the TCLINIT variable is set, that file is then evaluated.
 * If usually does the full Tcl initialization.
 *
 * Parameters
 *   o interp  (I) - A pointer to the interpreter.
 *   o defaultFile (I) - The file name of the default file to use, it
 *     normally contains a version number.
 * Returns:
 *   TCL_OK if all is ok, TCL_ERROR if an error occured.
 *-----------------------------------------------------------------------------
 */
static int
ProcessDefaultFile (interp, defaultFile)
    Tcl_Interp  *interp;
    char        *defaultFile;
{
    char *defaultFileToUse;

    defaultFileToUse = Tcl_GetVar (interp, "TCLDEFAULT", 1);
    if (*defaultFileToUse == '\0') {
        Tcl_AppendResult (interp,
                          "Can't access Tcl default file,\n",
                          "  Located in one of the following ways:\n",
                          "    Environment variable: `TCLDEFAULT' or,\n",
                          "    File `", defaultFile, "'.\n", 
                          (char *) NULL);
        return TCL_ERROR;
    }
    if (Tcl_EvalFile (interp, defaultFileToUse) != TCL_OK)
        return TCL_ERROR;
    Tcl_ResetResult (interp);

    return TCL_OK;
}

/*
 *-----------------------------------------------------------------------------
 * ProcessInitFile --
 *
 *    Process the Tcl init file, its abolute patch should be contained in
 * a Tcl variable "TCLINIT".  If the variable is not found, the file will
 * not be evaulated.
 *
 * Parameters
 *   o interp  (I) - A pointer to the interpreter.
 * Returns:
 *   TCL_OK if all is ok, TCL_ERROR if an error occured.
 *-----------------------------------------------------------------------------
 */
static int
ProcessInitFile (interp)
    Tcl_Interp  *interp;
{
    char *initFile;

    initFile = Tcl_GetVar (interp, "TCLINIT", 1);
    if (initFile != NULL) {
        if (Tcl_EvalFile (interp, initFile) != TCL_OK)
            return TCL_ERROR;
    }
    Tcl_ResetResult (interp);
    return TCL_OK;
}

/*
 *-----------------------------------------------------------------------------
 *
 * Tcl_ShellEnvInit --
 *
 *   Process the Tcl default file.  The default file is the only file at a
 * fixed path. It is a script file that usaually defines a variable "TCLINIT",
 * which has the path of the full initialization file. The default file can
 * also set things such as path variables.  
 *
 * Parameters
 *   o interp - A pointer to the interpreter.
 *   o options - Flags to control the behavior of this routine, the following
 *     option is supported:
 *       o TCLSH_QUICK_STARTUP - Don't source the default file or Tcl init
 *         file.
 *       o TCLSH_ABORT_STARTUP_ERR - If set, abort the process if an error
 *         occurs.
 *       o TCLSH_NO_INIT_FILE - If set, process the default file, but not the
 *         init file.  This can be used to make the default file do all
 *         initialization.
 *   o programName (I) - The name of the program being executed, usually
 *     taken from the main argv [0].  Used to set the Tcl variable.  If NULL
 *     then the variable will not be set.
 *   o argc, argv (I) - Arguments to pass to the program in a Tcl list variable
 *     `argv'.  Argv [0] should contain the first argument not the program
 *     name.  If argv is NULL, then the variable will not be set.
 *   o interactive (I) - The value to assign to the `interactiveSession' Tcl
 *     variable. TRUE if an interactive Tcl command loop will be entered,
 *     FALSE if a script will be executed .  The function does not enter the
 *     command loop, it just sets the variable.
 *   o defaultFile (I) - The file name of the default file to use.  If NULL,
 *     then the standard Tcl default file is used, which is formed from a
 *     location specified at compile time and the Extended Tcl version
 *     number.
 * Notes:
 *   The variables tclAppName, tclAppLongName, tclAppVersion 
 * must be set before calling thus routine if special values are desired.
 *
 * Returns:
 *   TCL_OK if all is ok, TCL_ERROR if an error occured.
 *-----------------------------------------------------------------------------
 */
int
Tcl_ShellEnvInit (interp, options, programName, argc, argv, interactive,
                  defaultFile)
    Tcl_Interp  *interp;
    unsigned     options;
    CONST char  *programName; 
    int          argc;
    CONST char **argv;
    int          interactive;
    CONST char	*defaultFile;
{
    int   result = TCL_OK;
    char *defaultFilePath;

    /*
     * Setup patch to default file, if not specified.
     */
    if (defaultFile == NULL) {
        defaultFilePath = ckalloc (strlen (TCL_DEFAULT) +
                                   strlen (TCL_VERSION) +
                                   strlen (TCL_EXTD_VERSION_SUFFIX) + 1);
        strcpy (defaultFilePath, TCL_DEFAULT);
        strcat (defaultFilePath, TCL_VERSION);
        strcat (defaultFilePath, TCL_EXTD_VERSION_SUFFIX);
    } else {
        defaultFilePath = (char *) defaultFile;
    }

    if (programName != NULL) {
        if (Tcl_SetVar (interp, "programName", (char *) programName,
                        TCL_GLOBAL_ONLY | TCL_LEAVE_ERR_MSG) == NULL)
            goto errorExit;
    }

    if (argv != NULL) {
        char *args;

        args = Tcl_Merge (argc, (char **) argv);
        if (Tcl_SetVar (interp, "argv", args,
                        TCL_GLOBAL_ONLY | TCL_LEAVE_ERR_MSG) == NULL)
            result = TCL_ERROR;
        ckfree (args);
        if (result != TCL_OK)
            goto errorExit;
    }
    if (Tcl_SetVar (interp, "interactiveSession", interactive ? "1" : "0",
                    TCL_GLOBAL_ONLY | TCL_LEAVE_ERR_MSG) == NULL)
        goto errorExit;

    tclxVersion = ckalloc (strlen (TCL_VERSION) + 
                           strlen (TCL_EXTD_VERSION_SUFFIX) + 1);
    strcpy (tclxVersion, TCL_VERSION);
    strcat (tclxVersion, TCL_EXTD_VERSION_SUFFIX);

#ifdef PATCHLEVEL
    tclxPatchlevel = PATCHLEVEL;
#else
    tclxPatchlevel = 0;
#endif

    /*
     * Set application specific values to return from the infox if they
     * have not been set.
     */
    if (tclAppName == NULL)
        tclAppName = "TclX";
    if (tclAppLongname == NULL)
        tclAppLongname = "Extended Tcl";
    if (tclAppVersion == NULL)
        tclAppVersion = tclxVersion;

    /*
     * Locate the default file and save in Tcl var TCLDEFAULT.  If not quick
     * startup, process the Tcl default file and execute the Tcl
     * initialization file.
     */
    if (FindDefaultFile (interp, (char *) defaultFilePath) != TCL_OK)
        goto errorExit;
    if (!(options & TCLSH_QUICK_STARTUP)) {
        if (ProcessDefaultFile (interp, defaultFilePath) != TCL_OK)
            goto errorExit;
        if (!(options & TCLSH_NO_INIT_FILE)) {
            if (ProcessInitFile (interp) != TCL_OK)
                goto errorExit;
        }
    }
    if (defaultFilePath != defaultFile)
        ckfree (defaultFilePath);
    return TCL_OK;

errorExit:
    if (defaultFilePath != defaultFile)
        ckfree (defaultFilePath);
    if (options & TCLSH_ABORT_STARTUP_ERR)
        Tcl_ErrorAbort (interp, 255);
    return TCL_ERROR;
}

/*
 *-----------------------------------------------------------------------------
 *
 * Tcl_Startup --
 *
 *    Initializes the Tcl extended environment.  This function processes the
 * standard command line arguments and locates the Tcl default file.  It then
 * sources the default file and initialization file pointed to by the default
 * file.  Either an interactive command loop is created or a Tcl script file
 * is executed depending on the command line.  This functions calls
 * Tcl_ShellEnvInit, so it should not be called separately.
 *
 * Parameters
 *   o interp - A pointer to the interpreter.
 *   o argc, argv - Arguments passed to main for the command line.
 *   o defaultFile (I) - The file name of the default file to use.  If NULL,
 *     then the standard Tcl default file is used, which is formed from a
 *     location specified at compile time and the Extended Tcl version
 *     number.
 *   o options (I) - Options that control startup behavior.  None are
 *     currently defined.
 * Notes:
 *   The variables tclAppName, tclAppLongName, tclAppVersion 
 * must be set before calling thus routine if special values are desired.
 *-----------------------------------------------------------------------------
 */
void
Tcl_Startup (interp, argc, argv, defaultFile, options)
    Tcl_Interp  *interp;
    int          argc;
    CONST char **argv;
    CONST char  *defaultFile;
    unsigned     options;
{
    char       *cmdBuf;
    tclParms_t  tclParms;
    int         result;

    /*
     * Process the arguments.
     */
    ParseCmdArgs (argc, (char **) argv, &tclParms);

    if (Tcl_ShellEnvInit (interp,
                          tclParms.options,
                          (CONST char *)tclParms.programName,
                          tclParms.tclArgc, (CONST char **)tclParms.tclArgv,
                          (tclParms.execStr == NULL),
                          (CONST char *)defaultFile) != TCL_OK)
        goto errorAbort;

    /*
     * If the invoked tcl interactively, give the user an interactive session,
     * otherwise, source the command file or execute the specified command.
     */
    if (tclParms.execFile) {
        result = Tcl_EvalFile (interp, tclParms.execStr);
        if (result != TCL_OK)
            goto errorAbort;
    } else if (tclParms.execCommand) {
        result = Tcl_Eval (interp, tclParms.execStr, 0, NULL);
        if (result != TCL_OK)
            goto errorAbort;
    } else
        Tcl_CommandLoop (interp, stdin, stdout, tclShellCmdEvalProc, 0);

    Tcl_ResetResult (interp);
    return;

errorAbort:
    Tcl_ErrorAbort (interp, 255);
}

