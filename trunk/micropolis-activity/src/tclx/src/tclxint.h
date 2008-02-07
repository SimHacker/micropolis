/*
 * tclExtdInt.h
 *
 * Standard internal include file for Extended Tcl library..
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
 * $Id: tclExtdInt.h,v 2.0 1992/10/16 04:51:27 markd Rel $
 *-----------------------------------------------------------------------------
 */

#ifndef TCLEXTDINT_H
#define TCLEXTDINT_H

#include "tclxtend.h"
#include "tclint.h"
#include "tclunix.h"
#include <sys/param.h>
#include <unistd.h>


#ifdef TCL_NEED_SYS_SELECT_H
#   include "sys/select.h"
#endif

/*
 * If tclUnix.h has already included time.h, don't include it again, some
 * systems don't #ifdef inside of the file.  On some systems, undef
 * CLK_TCK (defined in tclUnix.h) to avoid an annoying warning about
 * redefinition.
 */
#ifdef TCL_NEED_TIME_H
#    if TCL_SYS_TIME_H
#        ifdef TCL_DUP_CLK_TCK
#            undef CLK_TCK
#        endif        
#        include <time.h>
#    endif
#endif

/*
 * Precompute milliseconds-per-tick, the " + CLK_TCK / 2" bit gets it to
 * round off instead of truncate.  Take care of defining CLK_TCK if its not
 * defined.
 */
#ifndef CLK_TCK
#    ifdef HZ
#        define CLK_TCK HZ
#    else
#        define CLK_TCK 60
#    endif
#endif

#define MS_PER_TICK ((1000 + CLK_TCK/2) / CLK_TCK)

/*
 * If tclUnix.h did not bring times.h, bring it in here.
 */
#if TCL_GETTOD
#    include <sys/times.h>
#endif 

#include <values.h>
#include <grp.h>
/*
 * On some systems this is not included by tclUnix.h.
 */

/*
 * These should be take from an include file, but it got to be such a mess
 * to get the include files right that they are here for good measure.
 */
struct tm *gmtime ();
struct tm *localtime ();

#ifndef MAXINT
#    define BITSPERBYTE   8
#    define BITS(type)    (BITSPERBYTE * (int)sizeof(type))
#    define HIBITI        ((unsigned int)(1 << BITS(int) - 1))
#    define MAXINT        ((int)(~HIBITI))
#endif

#ifndef MININT
#    define MININT (-MAXINT)-1
#endif

#ifndef TRUE
#    define TRUE   (1)
#    define FALSE  (0)
#endif

/*
 * Structure to hold a regular expression, plus a Boyer-Moore compiled
 * pattern.
 */

typedef struct regexp_t {
    regexp *progPtr;
    char   *boyerMoorePtr;
    int     noCase;
    } regexp_t;
typedef regexp_t *regexp_pt;
/*
 * Flags used by RegExpCompile:
 */
#define REXP_NO_CASE         1   /* Do matching regardless of case    */
#define REXP_BOTH_ALGORITHMS 2   /* Use boyer-moore along with regexp */

/*
 * Data structure to control a dynamic buffer.  These buffers are primarly
 * used for reading things from files, were the maximum size is not known
 * in advance, and the buffer must grow.  These are used in the case were
 * the value is not to be returned as the interpreter result.
 */

#define INIT_DYN_BUFFER_SIZE 256

typedef struct dynamicBuf_t {
    char  buf [INIT_DYN_BUFFER_SIZE];   /* Initial buffer area.              */
    char *ptr;                          /* Pointer to buffer area.           */
    int   size;                         /* Current size of buffer.           */
    int   len;                          /* Current string length (less '\0') */
    } dynamicBuf_t;

/*
 * Used to return argument messages by most commands.
 */
extern char *tclXWrongArgs;

/*
 * Macros to do string compares.  They pre-check the first character before
 * checking of the strings are equal.
 */

#define STREQU(str1, str2) \
        (((str1) [0] == (str2) [0]) && (strcmp (str1, str2) == 0))
#define STRNEQU(str1, str2, cnt) \
        (((str1) [0] == (str2) [0]) && (strncmp (str1, str2, cnt) == 0))

/*
 * Prototypes for utility procedures.
 */
void
Tcl_DynBufInit _ANSI_ARGS_((dynamicBuf_t *dynBufPtr));

void
Tcl_DynBufFree _ANSI_ARGS_((dynamicBuf_t *dynBufPtr));

void
Tcl_DynBufReturn _ANSI_ARGS_((Tcl_Interp    *interp,
                              dynamicBuf_t *dynBufPtr));

void
Tcl_DynBufAppend _ANSI_ARGS_((dynamicBuf_t *dynBufPtr,
                              char         *newStr));

void
Tcl_ExpandDynBuf _ANSI_ARGS_((dynamicBuf_t *dynBufPtr,
                              int           appendSize));

int
Tcl_DynamicFgets _ANSI_ARGS_((dynamicBuf_t *dynBufPtr,
                              FILE         *filePtr,
                              int           append));

int
Tcl_ConvertFileHandle _ANSI_ARGS_((Tcl_Interp *interp,
                                  char       *handle));

time_t
Tcl_GetDate _ANSI_ARGS_((char   *p,
                         time_t  now,
                         long    zone));

int
Tcl_ProcessSignal _ANSI_ARGS_((Tcl_Interp *interp,
                               int         cmdResultCode));

void
Tcl_RegExpClean _ANSI_ARGS_((regexp_pt regExpPtr));

int
Tcl_RegExpCompile _ANSI_ARGS_((Tcl_Interp  *interp,
                               regexp_pt    regExpPtr,
                               char        *expression,
                               int          flags));

int
Tcl_RegExpExecute _ANSI_ARGS_((Tcl_Interp  *interp,
                               regexp_pt    regExpPtr,
                               char        *matchStrIn,
                               char        *matchStrLower));
void
Tcl_ResetSignals ();

int
Tcl_ReturnDouble _ANSI_ARGS_((Tcl_Interp *interp,
                              double      number));

int
Tcl_SetupFileEntry _ANSI_ARGS_((Tcl_Interp *interp,
                                int         fileNum,
                                int         readable,
                                int         writable));

/*
 * Definitions required to initialize all extended commands.  These are either
 * the command executors or initialization routines that do the command
 * initialization.  The initialization routines are used when there is more
 * to initializing the command that just binding the command name to the
 * executor.  Usually, this means initializing some command local data via
 * the ClientData mechanism.  The command executors should be declared to be of
 * type `Tcl_CmdProc', but this blows up some compilers, so they are declared
 * with an ANSI prototype.
 */

/*
 * from tclXbsearch.c
 */
extern int 
Tcl_BsearchCmd _ANSI_ARGS_((ClientData, Tcl_Interp*, int, char**));

/*
 * from tclXchmod.c
 */
extern int 
Tcl_ChmodCmd _ANSI_ARGS_((ClientData, Tcl_Interp*, int, char**));

extern int 
Tcl_ChownCmd _ANSI_ARGS_((ClientData, Tcl_Interp*, int, char**));

extern int 
Tcl_ChgrpCmd _ANSI_ARGS_((ClientData, Tcl_Interp*, int, char**));

/*
 * from tclXclock.c
 */
extern int 
Tcl_GetclockCmd _ANSI_ARGS_((ClientData, Tcl_Interp*, int, char**));

extern int 
Tcl_FmtclockCmd _ANSI_ARGS_((ClientData, Tcl_Interp*, int, char**));

/*
 * from tclXcnvclock.c
 */
extern int 
Tcl_ConvertclockCmd _ANSI_ARGS_((ClientData, Tcl_Interp*, int, char**));

/*
 * from tclXcmdloop.c
 */
extern int 
Tcl_CommandloopCmd _ANSI_ARGS_((ClientData, Tcl_Interp*, int, char**));

/*
 * from tclXdebug.c
 */
extern void
Tcl_InitDebug _ANSI_ARGS_((Tcl_Interp *interp));

/*
 * from tclXdup.c
 */
extern int 
Tcl_DupCmd _ANSI_ARGS_((ClientData, Tcl_Interp*, int, char**));

/*
 * from tclXfcntl.c
 */
extern int 
Tcl_FcntlCmd _ANSI_ARGS_((ClientData, Tcl_Interp*, int, char**));

/*
 * from tclXfilecmds.c
 */
extern int 
Tcl_PipeCmd _ANSI_ARGS_((ClientData, Tcl_Interp*, int, char**));

extern int 
Tcl_CopyfileCmd _ANSI_ARGS_((ClientData, Tcl_Interp*, int, char**));

extern int 
Tcl_FstatCmd _ANSI_ARGS_((ClientData, Tcl_Interp*, int, char**));

extern int 
Tcl_LgetsCmd _ANSI_ARGS_((ClientData, Tcl_Interp*, int, char**));

extern int
Tcl_FlockCmd _ANSI_ARGS_((ClientData, Tcl_Interp*, int, char**));

extern int
Tcl_FunlockCmd _ANSI_ARGS_((ClientData, Tcl_Interp*, int, char**));

/*
 * from tclXfilescan.c
 */
extern void
Tcl_InitFilescan _ANSI_ARGS_((Tcl_Interp *interp));

/*
 * from tclXfmath.c
 */
extern int 
Tcl_AcosCmd _ANSI_ARGS_((ClientData, Tcl_Interp*, int, char**));

extern int 
Tcl_AsinCmd _ANSI_ARGS_((ClientData, Tcl_Interp*, int, char**));

extern int 
Tcl_AtanCmd _ANSI_ARGS_((ClientData, Tcl_Interp*, int, char**));

extern int 
Tcl_CosCmd _ANSI_ARGS_((ClientData, Tcl_Interp*, int, char**));

extern int 
Tcl_SinCmd _ANSI_ARGS_((ClientData, Tcl_Interp*, int, char**));

extern int 
Tcl_TanCmd _ANSI_ARGS_((ClientData, Tcl_Interp*, int, char**));

extern int 
Tcl_CoshCmd _ANSI_ARGS_((ClientData, Tcl_Interp*, int, char**));

extern int 
Tcl_SinhCmd _ANSI_ARGS_((ClientData, Tcl_Interp*, int, char**));

extern int 
Tcl_TanhCmd _ANSI_ARGS_((ClientData, Tcl_Interp*, int, char**));

extern int 
Tcl_ExpCmd _ANSI_ARGS_((ClientData, Tcl_Interp*, int, char**));

extern int 
Tcl_LogCmd _ANSI_ARGS_((ClientData, Tcl_Interp*, int, char**));

extern int 
Tcl_Log10Cmd _ANSI_ARGS_((ClientData, Tcl_Interp*, int, char**));

extern int 
Tcl_SqrtCmd _ANSI_ARGS_((ClientData, Tcl_Interp*, int, char**));

extern int 
Tcl_FabsCmd _ANSI_ARGS_((ClientData, Tcl_Interp*, int, char**));

extern int 
Tcl_FloorCmd _ANSI_ARGS_((ClientData, Tcl_Interp*, int, char**));

extern int 
Tcl_CeilCmd _ANSI_ARGS_((ClientData, Tcl_Interp*, int, char**));

extern int 
Tcl_FmodCmd _ANSI_ARGS_((ClientData, Tcl_Interp*, int, char**));

extern int 
Tcl_PowCmd _ANSI_ARGS_((ClientData, Tcl_Interp*, int, char**));

/*
 * from tclXgeneral.c
 */

extern int 
Tcl_EchoCmd _ANSI_ARGS_((ClientData, Tcl_Interp*, int, char**));

extern int 
Tcl_InfoxCmd _ANSI_ARGS_((ClientData, Tcl_Interp*, int, char**));

extern int 
Tcl_LoopCmd _ANSI_ARGS_((ClientData, Tcl_Interp*, int, char**));

/*
 * from tclXid.c
 */
extern int 
Tcl_IdCmd _ANSI_ARGS_((ClientData, Tcl_Interp*, int, char**));

/*
 * from tclXkeylist.c
 */
extern int 
Tcl_KeyldelCmd _ANSI_ARGS_((ClientData, Tcl_Interp*, int, char**));

extern int 
Tcl_KeylgetCmd _ANSI_ARGS_((ClientData, Tcl_Interp*, int, char**));

extern int 
Tcl_KeylkeysCmd _ANSI_ARGS_((ClientData, Tcl_Interp*, int, char**));

extern int 
Tcl_KeylsetCmd _ANSI_ARGS_((ClientData, Tcl_Interp*, int, char**));

/*
 * from tclXlist.c
 */
extern int 
Tcl_LvarpopCmd _ANSI_ARGS_((ClientData, Tcl_Interp*, int, char**));

extern int 
Tcl_LvarcatCmd _ANSI_ARGS_((ClientData, Tcl_Interp*, int, char**));

extern int 
Tcl_LvarpushCmd _ANSI_ARGS_((ClientData, Tcl_Interp*, int, char**));

extern int 
Tcl_LemptyCmd _ANSI_ARGS_((ClientData, Tcl_Interp*, int, char**));

/*
 * from tclXmath.c
 */
extern int 
Tcl_MaxCmd _ANSI_ARGS_((ClientData, Tcl_Interp*, int, char**));

extern int 
Tcl_MinCmd _ANSI_ARGS_((ClientData, Tcl_Interp*, int, char**));

extern int 
Tcl_RandomCmd _ANSI_ARGS_((ClientData, Tcl_Interp*, int, char**));

/*
 * from tclXmsgcat.c
 */
extern void
Tcl_InitMsgCat _ANSI_ARGS_((Tcl_Interp *interp));

/*
 * from tclXprocess.c
 */
extern int 
Tcl_ExeclCmd _ANSI_ARGS_((ClientData, Tcl_Interp*, int, char**));

extern int 
Tcl_ForkCmd _ANSI_ARGS_((ClientData, Tcl_Interp*, int, char**));

extern int 
Tcl_WaitCmd _ANSI_ARGS_((ClientData, Tcl_Interp*, int, char**));

/*
 * from tclXprofile.c
 */
void
Tcl_InitProfile _ANSI_ARGS_((Tcl_Interp *interp));

/*
 * from tclXselect.c
 */
extern int 
Tcl_SelectCmd _ANSI_ARGS_((ClientData, Tcl_Interp*, int, char**));

/*
 * from tclXsignal.c
 */
extern void
Tcl_InitSignalHandling _ANSI_ARGS_((Tcl_Interp *interp));

/*
 * from tclXstring.c
 */
extern int 
Tcl_CindexCmd _ANSI_ARGS_((ClientData, Tcl_Interp*, int, char**));

extern int 
Tcl_ClengthCmd _ANSI_ARGS_((ClientData, Tcl_Interp*, int, char**));

extern int 
Tcl_CrangeCmd _ANSI_ARGS_((ClientData, Tcl_Interp*, int, char**));

extern int 
Tcl_ReplicateCmd _ANSI_ARGS_((ClientData, Tcl_Interp*, int, char**));

extern int 
Tcl_TranslitCmd _ANSI_ARGS_((ClientData, Tcl_Interp*, int, char**));

extern int 
Tcl_CtypeCmd _ANSI_ARGS_((ClientData, Tcl_Interp*, int, char**));

/*
 * from tclXlib.c
 */
extern int
Tcl_Demand_loadCmd _ANSI_ARGS_((ClientData, Tcl_Interp*, int, char**));

extern int
Tcl_LoadlibindexCmd _ANSI_ARGS_((ClientData, Tcl_Interp*, int, char**));

/*
 * from tclXunixcmds.c
 */
extern int 
Tcl_AlarmCmd _ANSI_ARGS_((ClientData, Tcl_Interp*, int, char**));

extern int 
Tcl_SleepCmd _ANSI_ARGS_((ClientData, Tcl_Interp*, int, char**));

extern int 
Tcl_SystemCmd _ANSI_ARGS_((ClientData, Tcl_Interp*, int, char**));

extern int 
Tcl_TimesCmd _ANSI_ARGS_((ClientData, Tcl_Interp*, int, char**));

extern int 
Tcl_UmaskCmd _ANSI_ARGS_((ClientData, Tcl_Interp*, int, char**));

extern int 
Tcl_LinkCmd _ANSI_ARGS_((ClientData, Tcl_Interp*, int, char**));

extern int 
Tcl_UnlinkCmd _ANSI_ARGS_((ClientData, Tcl_Interp*, int, char**));

extern int 
Tcl_MkdirCmd _ANSI_ARGS_((ClientData, Tcl_Interp*, int, char**));

extern int 
Tcl_RmdirCmd _ANSI_ARGS_((ClientData, Tcl_Interp*, int, char**));

#endif
