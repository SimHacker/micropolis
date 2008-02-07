/* 
 * tclExtend.h
 *
 *    External declarations for the extended Tcl library.
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
 * $Id: tclExtend.h,v 2.0 1992/10/16 04:51:29 markd Rel $
 *-----------------------------------------------------------------------------
 */

#ifndef TCLEXTEND_H
#define TCLEXTEND_H

#include <stdio.h>
#include "tcl.h"

/*
 * Version suffix for extended Tcl, this is appended to the standard Tcl
 * version to form the actual extended Tcl version.
 */

#define TCL_EXTD_VERSION_SUFFIX "c"   /* 6.1c, 6.2c or 6.3c */

#ifdef MSDOS
#define TCL_NO_ITIMER
#define TCL_NO_FILE_LOCKING
#define TCL_IEEE_FP_MATH
#define TCL_32_BIT_RANDOM
#define TCL_POSIX_SIG
#define TCL_TM_GMTOFF
#endif

typedef void *void_pt;

/*
 * Flags for Tcl shell startup.
 */
#define TCLSH_QUICK_STARTUP       1   /* Don't process default & init files. */
#define TCLSH_ABORT_STARTUP_ERR   2   /* Abort on an error.                  */
#define TCLSH_NO_INIT_FILE        4   /* Don't process the init file.        */

/*
 * These globals are used by the infox command.  Should be set by main.
 */

extern char *tclxVersion;        /* Extended Tcl version number.            */
extern int   tclxPatchlevel;     /* Extended Tcl patch level.               */

extern char *tclAppName;         /* Application name                        */
extern char *tclAppLongname;     /* Long, natural language application name */
extern char *tclAppVersion;      /* Version number of the application       */

/*
 * If set to be a pointer to the procedure Tcl_RecordAndEval, will link in
 * history.  Should be set by main.
 */
extern int (*tclShellCmdEvalProc) ();

/*
 * If non-zero, a signal was received.  Normally signals are handled in
 * Tcl_Eval, but if an application does not return to eval for some period
 * of time, then this should be checked and Tcl_CheckForSignal called if
 * this is set.
 */
extern int tclReceivedSignal;

/*
 * Exported Extended Tcl functions.
 */

EXTERN int
Tcl_CheckForSignal _ANSI_ARGS_((Tcl_Interp *interp,
                                int         cmdResultCode));

EXTERN void 
Tcl_CommandLoop _ANSI_ARGS_((Tcl_Interp *interp,
                             FILE       *inFile,
                             FILE       *outFile,
                             int         (*evalProc) (),
                             unsigned    options));

EXTERN Tcl_Interp * 
Tcl_CreateExtendedInterp ();

EXTERN char *
Tcl_DeleteKeyedListField _ANSI_ARGS_((Tcl_Interp  *interp,
                                      CONST char  *fieldName,
                                      CONST char  *keyedList));
EXTERN char * 
Tcl_DownShift _ANSI_ARGS_((char       *targetStr,
                           CONST char *sourceStr));
EXTERN void
Tcl_ErrorAbort _ANSI_ARGS_((Tcl_Interp  *interp,
                            int          exitCode));

EXTERN char * 
Tcl_UpShift _ANSI_ARGS_((char       *targetStr,
                         CONST char *sourceStr));

EXTERN int
Tcl_GetKeyedListField _ANSI_ARGS_((Tcl_Interp  *interp,
                                   CONST char  *fieldName,
                                   CONST char  *keyedList,
                                   char       **fieldValuePtr));

int
Tcl_GetKeyedListKeys _ANSI_ARGS_((Tcl_Interp  *interp,
                                  CONST char  *subFieldName,
                                  CONST char  *keyedList,
                                  int         *keyesArgcPtr,
                                  char      ***keyesArgvPtr));

EXTERN int 
Tcl_GetLong _ANSI_ARGS_((Tcl_Interp  *interp,
                         CONST char *string,
                         long        *longPtr));

EXTERN int 
Tcl_GetUnsigned _ANSI_ARGS_((Tcl_Interp  *interp,
                             CONST char *string,
                             unsigned   *unsignedPtr));

EXTERN char *
Tcl_SetKeyedListField _ANSI_ARGS_((Tcl_Interp  *interp,
                                   CONST char  *fieldName,
                                   CONST char  *fieldvalue,
                                   CONST char  *keyedList));

EXTERN int
Tcl_StrToLong _ANSI_ARGS_((CONST char *string,
                           int          base,
                           long        *longPtr));

EXTERN int
Tcl_StrToInt _ANSI_ARGS_((CONST char *string,
                          int         base,
                          int        *intPtr));

EXTERN int
Tcl_StrToUnsigned _ANSI_ARGS_((CONST char *string,
                               int         base,
                               unsigned   *unsignedPtr));

EXTERN int
Tcl_StrToDouble _ANSI_ARGS_((CONST char  *string,
                             double      *doublePtr));

EXTERN void_pt  
Tcl_HandleAlloc _ANSI_ARGS_((void_pt   headerPtr,
                             char     *handlePtr));

EXTERN void 
Tcl_HandleFree _ANSI_ARGS_((void_pt  headerPtr,
                            void_pt  entryPtr));

EXTERN void_pt
Tcl_HandleTblInit _ANSI_ARGS_((CONST char *handleBase,
                               int         entrySize,
                               int         initEntries));

EXTERN void
Tcl_HandleTblRelease _ANSI_ARGS_((void_pt headerPtr));

EXTERN int
Tcl_HandleTblUseCount _ANSI_ARGS_((void_pt headerPtr,
                                   int     amount));

EXTERN void_pt
Tcl_HandleWalk _ANSI_ARGS_((void_pt   headerPtr,
                            int      *walkKeyPtr));

EXTERN void
Tcl_WalkKeyToHandle _ANSI_ARGS_((void_pt   headerPtr,
                                 int       walkKey,
                                 char     *handlePtr));

EXTERN void_pt
Tcl_HandleXlate _ANSI_ARGS_((Tcl_Interp  *interp,
                             void_pt      headerPtr,
                             CONST  char *handle));

EXTERN int
Tcl_MathError _ANSI_ARGS_((char *functionName,
                           int   errorType));

EXTERN void 
Tcl_Startup _ANSI_ARGS_((Tcl_Interp   *interp,
                         int           argc,
                         CONST char  **argv,
                         CONST char   *defaultFile,
                         unsigned      options));

EXTERN int
Tcl_ShellEnvInit _ANSI_ARGS_((Tcl_Interp  *interp,
                              unsigned     options,
                              CONST char  *programName,
                              int          argc,
                              CONST char **argv,
                              int          interactive,
                              CONST char  *defaultFile));

EXTERN int
Tcl_System _ANSI_ARGS_((Tcl_Interp *interp,
                        char       *command));

#endif
