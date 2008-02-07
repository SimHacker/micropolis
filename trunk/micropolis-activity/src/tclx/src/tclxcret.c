/*
 * tclXcreate.c
 *
 * Contains a routine to create an interpreter and initialize all the Extended
 * Tcl commands.  It is is a seperate file so that an application may create
 * the interpreter and add in only a subset of the Extended Tcl commands.
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
 * $Id: tclXcreate.c,v 2.0 1992/10/16 04:50:33 markd Rel $
 *-----------------------------------------------------------------------------
 */

#include "tclxint.h"

int matherr ();

/*
 * This is a little kludge to make sure matherr is brought in from the Tcl
 * library if it is not already defined.  This could be done on the link line,
 * but this makes sure it happens.  This is a global so optimizers don't thow
 * away the assignment to it.
 */
static int (*bringInMathErr)() = matherr;


/*
 *-----------------------------------------------------------------------------
 *
 * Tcl_CreateExtendedInterp --
 *
 *      Create a new TCL command interpreter and initialize all of the
 *      extended Tcl commands..
 *
 * Results:
 *      The return value is a token for the interpreter.
 *-----------------------------------------------------------------------------
 */
Tcl_Interp *
Tcl_CreateExtendedInterp ()
{
    Tcl_Interp *interp;

    interp = Tcl_CreateInterp ();

    /*
     * from tclCkalloc.c (now part of the UCB Tcl).
     */
#ifdef TCL_MEM_DEBUG    
    Tcl_InitMemory (interp);
#endif

    /*
     * from tclXbsearch.c
     */
    Tcl_CreateCommand (interp, "bsearch", Tcl_BsearchCmd, 
                      (ClientData)NULL, (void (*)())NULL);

    /*
     * from tclXchmod.c
     */
    Tcl_CreateCommand (interp, "chgrp", Tcl_ChgrpCmd, (ClientData)NULL,
                      (void (*)())NULL);
    Tcl_CreateCommand (interp, "chmod", Tcl_ChmodCmd, (ClientData)NULL,
                      (void (*)())NULL);
    Tcl_CreateCommand (interp, "chown", Tcl_ChownCmd, (ClientData)NULL,
                      (void (*)())NULL);

    /*
     * from tclXclock.c
     */
    Tcl_CreateCommand (interp, "getclock", Tcl_GetclockCmd, 
                      (ClientData)NULL, (void (*)())NULL);
    Tcl_CreateCommand (interp, "fmtclock", Tcl_FmtclockCmd, 
                      (ClientData)NULL, (void (*)())NULL);

    /*
     * from tclXcnvdate.c
     */
    Tcl_CreateCommand (interp, "convertclock", Tcl_ConvertclockCmd,
                      (ClientData)NULL, (void (*)())NULL);

    /*
     * from tclXcmdloop.c
     */
    Tcl_CreateCommand (interp, "commandloop", Tcl_CommandloopCmd, 
                      (ClientData)NULL, (void (*)())NULL);

    /*
     * from tclXdebug.c
     */
    Tcl_InitDebug (interp);

    /*
     * from tclXdup.c
     */
    Tcl_CreateCommand (interp, "dup",  Tcl_DupCmd, 
                       (ClientData) NULL, (void (*)())NULL);
    /*
     * from tclXtclXfcntl.c
     */
    Tcl_CreateCommand (interp, "fcntl", Tcl_FcntlCmd,
                       (ClientData) NULL, (void (*)())NULL);

    /*
     * from tclXfilecmds.c
     */
    Tcl_CreateCommand (interp, "pipe", Tcl_PipeCmd,
                       (ClientData) NULL, (void (*)())NULL);
    Tcl_CreateCommand (interp, "copyfile", Tcl_CopyfileCmd,
                       (ClientData) NULL, (void (*)())NULL);
    Tcl_CreateCommand (interp, "fstat", Tcl_FstatCmd,
                       (ClientData) NULL, (void (*)())NULL);
    Tcl_CreateCommand (interp, "lgets", Tcl_LgetsCmd,
                       (ClientData) NULL, (void (*)())NULL);
    Tcl_CreateCommand (interp, "flock", Tcl_FlockCmd,
                       (ClientData) NULL, (void (*)())NULL);
    Tcl_CreateCommand (interp, "funlock", Tcl_FunlockCmd,
                       (ClientData) NULL, (void (*)())NULL);

    /*
     * from tclXfilescan.c
     */
    Tcl_InitFilescan (interp);

    /*
     * from tclXfmath.c
     */
    Tcl_CreateCommand(interp, "acos", Tcl_AcosCmd, 
                     (ClientData)NULL, (void (*)())NULL);
    Tcl_CreateCommand(interp, "asin", Tcl_AsinCmd, 
                     (ClientData)NULL, (void (*)())NULL);
    Tcl_CreateCommand(interp, "atan", Tcl_AtanCmd, 
                     (ClientData)NULL, (void (*)())NULL);
    Tcl_CreateCommand(interp, "cos", Tcl_CosCmd, 
                     (ClientData)NULL, (void (*)())NULL);
    Tcl_CreateCommand(interp, "sin", Tcl_SinCmd, 
                     (ClientData)NULL, (void (*)())NULL);
    Tcl_CreateCommand(interp, "tan", Tcl_TanCmd, 
                     (ClientData)NULL, (void (*)())NULL);
    Tcl_CreateCommand(interp, "cosh", Tcl_CoshCmd, 
                     (ClientData)NULL, (void (*)())NULL);
    Tcl_CreateCommand(interp, "sinh", Tcl_SinhCmd, 
                     (ClientData)NULL, (void (*)())NULL);
    Tcl_CreateCommand(interp, "tanh", Tcl_TanhCmd, 
                     (ClientData)NULL, (void (*)())NULL);
    Tcl_CreateCommand(interp, "exp", Tcl_ExpCmd, 
                     (ClientData)NULL, (void (*)())NULL);
    Tcl_CreateCommand(interp, "log", Tcl_LogCmd, 
                     (ClientData)NULL, (void (*)())NULL);
    Tcl_CreateCommand(interp, "log10", Tcl_Log10Cmd, 
                     (ClientData)NULL, (void (*)())NULL);
    Tcl_CreateCommand(interp, "sqrt", Tcl_SqrtCmd, 
                     (ClientData)NULL, (void (*)())NULL);
    Tcl_CreateCommand(interp, "fabs", Tcl_FabsCmd, 
                     (ClientData)NULL, (void (*)())NULL);
    Tcl_CreateCommand(interp, "floor", Tcl_FloorCmd, 
                     (ClientData)NULL, (void (*)())NULL);
    Tcl_CreateCommand(interp, "ceil", Tcl_CeilCmd, 
                     (ClientData)NULL, (void (*)())NULL);
    Tcl_CreateCommand(interp, "fmod", Tcl_FmodCmd, 
                     (ClientData)NULL, (void (*)())NULL);
    Tcl_CreateCommand(interp, "pow", Tcl_PowCmd, 
                     (ClientData)NULL, (void (*)())NULL);

    /*
     * from tclXgeneral.c
     */
    Tcl_CreateCommand(interp, "echo", Tcl_EchoCmd, 
                     (ClientData)NULL, (void (*)())NULL);
    Tcl_CreateCommand(interp, "infox", Tcl_InfoxCmd, 
                     (ClientData)NULL, (void (*)())NULL);
    Tcl_CreateCommand(interp, "loop", Tcl_LoopCmd, 
                     (ClientData)NULL, (void (*)())NULL);

    /*
     * from tclXid.c
     */
    Tcl_CreateCommand (interp, "id", Tcl_IdCmd,
                       (ClientData)NULL, (void (*)())NULL);

    /*
     * from tclXkeylist.c
     */
    Tcl_CreateCommand(interp, "keyldel", Tcl_KeyldelCmd,
                     (ClientData)NULL, (void (*)())NULL);
    Tcl_CreateCommand(interp, "keylget", Tcl_KeylgetCmd,
                     (ClientData)NULL, (void (*)())NULL);
    Tcl_CreateCommand(interp, "keylkeys", Tcl_KeylkeysCmd,
                     (ClientData)NULL, (void (*)())NULL);
    Tcl_CreateCommand(interp, "keylset", Tcl_KeylsetCmd,
                     (ClientData)NULL, (void (*)())NULL);

    /*
     * from tclXlist.c
     */
    Tcl_CreateCommand(interp, "lvarcat", Tcl_LvarcatCmd, 
                     (ClientData)NULL, (void (*)())NULL);
    Tcl_CreateCommand(interp, "lvarpop", Tcl_LvarpopCmd, 
                     (ClientData)NULL, (void (*)())NULL);
    Tcl_CreateCommand(interp, "lvarpush", Tcl_LvarpushCmd, 
                     (ClientData)NULL, (void (*)())NULL);
    Tcl_CreateCommand(interp, "lempty", Tcl_LemptyCmd, 
                     (ClientData)NULL, (void (*)())NULL);
    /*
     * from tclXmath.c
     */
    Tcl_CreateCommand (interp, "max", Tcl_MaxCmd, (ClientData)NULL, 
              (void (*)())NULL);
    Tcl_CreateCommand (interp, "min", Tcl_MinCmd, (ClientData)NULL, 
              (void (*)())NULL);
    Tcl_CreateCommand (interp, "random", Tcl_RandomCmd, (ClientData)NULL, 
              (void (*)())NULL);

    /*
     * from tclXmsgcat.c
     */
    Tcl_InitMsgCat (interp);

    /*
     * from tclXprocess.c
     */
    Tcl_CreateCommand (interp, "execl", Tcl_ExeclCmd, (ClientData)NULL,
                      (void (*)())NULL);
    Tcl_CreateCommand (interp, "fork", Tcl_ForkCmd, (ClientData)NULL,
                      (void (*)())NULL);
    Tcl_CreateCommand (interp, "wait", Tcl_WaitCmd, (ClientData)NULL,
                      (void (*)())NULL);

    /*
     * from tclXprofile.c
     */
    Tcl_InitProfile (interp);

    /*
     * from tclXselect.c
     */
    Tcl_CreateCommand (interp, "select", Tcl_SelectCmd,
                       (ClientData) NULL, (void (*)())NULL);

    /*
     * from tclXsignal.c
     */
    Tcl_InitSignalHandling (interp);

    /*
     * from tclXstring.c
     */
    Tcl_CreateCommand(interp, "cindex", Tcl_CindexCmd, 
                     (ClientData)NULL, (void (*)())NULL);
    Tcl_CreateCommand(interp, "clength", Tcl_ClengthCmd, 
                     (ClientData)NULL, (void (*)())NULL);
    Tcl_CreateCommand(interp, "crange", Tcl_CrangeCmd, 
                     (ClientData)NULL, (void (*)())NULL);
    Tcl_CreateCommand(interp, "csubstr", Tcl_CrangeCmd, 
                     (ClientData)NULL, (void (*)())NULL);
    Tcl_CreateCommand(interp, "replicate", Tcl_ReplicateCmd, 
                     (ClientData)NULL, (void (*)())NULL);
    Tcl_CreateCommand (interp, "translit", Tcl_TranslitCmd, (ClientData)NULL,
                       (void (*)())NULL);
    Tcl_CreateCommand (interp, "ctype", Tcl_CtypeCmd,
                       (ClientData)NULL, (void (*)())NULL);

    /*
     * from tclXlib.c
     */
    Tcl_CreateCommand (interp, "demand_load", Tcl_Demand_loadCmd,
                      (ClientData)NULL, (void (*)())NULL);
    Tcl_CreateCommand (interp, "loadlibindex", Tcl_LoadlibindexCmd,
                      (ClientData)NULL, (void (*)())NULL);

    /*
     * from tclXunixcmds.c
     */
    Tcl_CreateCommand (interp, "system", Tcl_SystemCmd, (ClientData)NULL,
                      (void (*)())NULL);
    Tcl_CreateCommand (interp, "times", Tcl_TimesCmd, (ClientData)NULL,
                      (void (*)())NULL);
    Tcl_CreateCommand (interp, "umask", Tcl_UmaskCmd, (ClientData)NULL,
                      (void (*)())NULL);
    Tcl_CreateCommand (interp, "link", Tcl_LinkCmd, (ClientData)NULL,
                      (void (*)())NULL);
    Tcl_CreateCommand (interp, "unlink", Tcl_UnlinkCmd, (ClientData)NULL,
                      (void (*)())NULL);
    Tcl_CreateCommand (interp, "mkdir", Tcl_MkdirCmd, (ClientData)NULL,
                      (void (*)())NULL);
    Tcl_CreateCommand (interp, "rmdir", Tcl_RmdirCmd, (ClientData)NULL,
                      (void (*)())NULL);
    Tcl_CreateCommand (interp, "alarm", Tcl_AlarmCmd, (ClientData)NULL, 
                      (void (*)())NULL);
    Tcl_CreateCommand (interp, "sleep", Tcl_SleepCmd, (ClientData)NULL, 
                      (void (*)())NULL);

    return interp;
}
