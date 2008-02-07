/*
 * main++.C --
 *
 *   C++ based main.  It is an alternative to the existing main.c to set up
 * the Tcl shell and may be used as a example on how to use tcl++.h
 *      
 *---------------------------------------------------------------------------
 * Copyright 1992 Karl Lehenbauer and Mark Diekhans.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies.  Karl Lehenbauer,
 * Mark Diekhans, and Peter da Silva make no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *---------------------------------------------------------------------------
 * Based on Tcl C++ classes developed by Parag Patel.
 *-----------------------------------------------------------------------------
 * $Id: main++.C,v 2.0 1992/10/16 04:51:31 markd Rel $
 *-----------------------------------------------------------------------------
 */
#include <stdlib.h>

#include "tcl++.h"
/*
 * This file is optional.
 */
#include "patchlevel.h"

int
main (int     argc,
      char  **argv)
{
    TclInterp_cl *interpPtr;

    /*
     * If history is to be used, then set the eval procedure pointer that
     * Tcl_CommandLoop so that history will be recorded.  This reference
     * also brings in history from Tcl.a.
     */
#ifndef TCL_NOHISTORY
     tclShellCmdEvalProc = (int (*)())Tcl_RecordAndEval;
#endif

    /* 
     * Create a Tcl interpreter for the session, with all extended commands
     * initialized.  This can be replaced with Tcl_CreateInterp followed
     * by a subset of the extended command initializaton procedures if 
     * desired.
     */
    interpPtr = new TclInterp_cl;

    /*
     *   >>>>>> INITIALIZE APPLICATION SPECIFIC COMMANDS HERE <<<<<<
     */

    /*
     * Load the tcl startup code, this should pull in all of the tcl
     * procs, paths, command line processing, autoloads, packages, etc.
     * If Tcl was invoked interactively, Tcl_Startup will give it
     * a command loop.
     */

    interpPtr->Startup (argc, argv, NULL, 0);

    /* 
     * Delete the interpreter (not neccessary under Unix, but we do
     * it if TCL_MEM_DEBUG is set to better enable us to catch memory
     * corruption problems)
     */

#ifdef TCL_MEM_DEBUG
    delete interpPtr;
#endif

#ifdef TCL_SHELL_MEM_LEAK
    printf (" >>> Dumping active memory list to mem.lst <<<\n");
    if (Tcl_DumpActiveMemory ("mem.lst") != TCL_OK)
        panic ("error accessing `mem.lst': %s", strerror (errno));
#endif

    exit(0);
}

