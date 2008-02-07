/* 
 * main.c --
 *
 * Main to run the Tcl shell.  This file is a useful template for custom
 * applications that wish to have Tcl as the top level command language.
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
 * $Id: main.c,v 2.1 1992/11/10 03:54:12 markd Exp $
 *-----------------------------------------------------------------------------
 */

#include <unistd.h>

#include "tclxtend.h"

int
main(argc, argv)
    int		argc;
    CONST char	**argv;
{
    Tcl_Interp *interp;

    /*
     * If history is to be used, then set the eval procedure pointer that
     * Tcl_CommandLoop so that history will be recorded.  This reference
     * also brings in history from libtcl.a.
     */
#ifndef TCL_NOHISTORY
     tclShellCmdEvalProc = Tcl_RecordAndEval;
#endif

    /* 
     * Create a Tcl interpreter for the session, with all extended commands
     * initialized.  This can be replaced with Tcl_CreateInterp followed
     * by a subset of the extended command initializaton procedures if 
     * desired.
     */
    interp = Tcl_CreateExtendedInterp();

    /*
     *   >>>>>> INITIALIZE APPLICATION SPECIFIC COMMANDS HERE <<<<<<
     */

    /*
     * Load the tcl startup code, this should pull in all of the tcl
     * procs, paths, command line processing, autoloads, packages, etc.
     * If Tcl was invoked interactively, Tcl_Startup will give it
     * a command loop.
     */

    Tcl_Startup (interp, argc, argv, NULL, 0);

    /* 
     * Delete the interpreter (not neccessary under Unix, but we do
     * it if TCL_MEM_DEBUG is set to better enable us to catch memory
     * corruption problems)
     */

#ifdef TCL_MEM_DEBUG
    Tcl_DeleteInterp(interp);
#endif

#ifdef TCL_SHELL_MEM_LEAK
    printf (" >>> Dumping active memory list to mem.lst <<<\n");
    if (Tcl_DumpActiveMemory ("mem.lst") != TCL_OK)
        panic ("error accessing `mem.lst': %s", strerror (errno));
#endif

    _exit(0);
}

