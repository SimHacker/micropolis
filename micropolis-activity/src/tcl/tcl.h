/*
 * tcl.h --
 *
 *	This header file describes the externally-visible facilities
 *	of the Tcl interpreter.
 *
 * Copyright 1987-1991 Regents of the University of California
 * Permission to use, copy, modify, and distribute this
 * software and its documentation for any purpose and without
 * fee is hereby granted, provided that the above copyright
 * notice appear in all copies.  The University of California
 * makes no representations about the suitability of this
 * software for any purpose.  It is provided "as is" without
 * express or implied warranty.
 *
 * $Header: /user6/ouster/tcl/RCS/tcl.h,v 1.84 92/08/07 08:21:34 ouster Exp $ SPRITE (Berkeley)
 */

#ifndef _TCL
#define _TCL

#define TCL_VERSION "6.4"

/*
 * Definitions that allow this header file to be used either with or
 * without ANSI C features like function prototypes.
 */

#undef _ANSI_ARGS_
#undef const
#if ((defined(__STDC__) || defined(SABER)) && !defined(NO_PROTOTYPE)) || defined(__cplusplus)
#   define _ANSI_ARGS_(x)	x
#   define CONST const
#   ifdef __cplusplus
#       define VARARGS (...)
#   else
#       define VARARGS ()
#   endif
#else
#   define _ANSI_ARGS_(x)	()
#   define CONST
#endif

#ifdef __cplusplus
#   define EXTERN extern "C"
#else
#   define EXTERN extern
#endif

/*
 * Miscellaneous declarations (to allow Tcl to be used stand-alone,
 * without the rest of Sprite).
 */

#ifndef NULL
#define NULL 0
#endif

#ifndef _CLIENTDATA
typedef int *ClientData;
#define _CLIENTDATA
#endif

/*
 * Data structures defined opaquely in this module.  The definitions
 * below just provide dummy types.  A few fields are made visible in
 * Tcl_Interp structures, namely those for returning string values.
 * Note:  any change to the Tcl_Interp definition below must be mirrored
 * in the "real" definition in tclInt.h.
 */

typedef struct Tcl_Interp{
    char *result;		/* Points to result string returned by last
				 * command. */
    void (*freeProc) _ANSI_ARGS_((char *blockPtr));
				/* Zero means result is statically allocated.
				 * If non-zero, gives address of procedure
				 * to invoke to free the result.  Must be
				 * freed by Tcl_Eval before executing next
				 * command. */
    int errorLine;		/* When TCL_ERROR is returned, this gives
				 * the line number within the command where
				 * the error occurred (1 means first line). */
} Tcl_Interp;

typedef int *Tcl_Trace;
typedef int *Tcl_CmdBuf;

/*
 * When a TCL command returns, the string pointer interp->result points to
 * a string containing return information from the command.  In addition,
 * the command procedure returns an integer value, which is one of the
 * following:
 *
 * TCL_OK		Command completed normally;  interp->result contains
 *			the command's result.
 * TCL_ERROR		The command couldn't be completed successfully;
 *			interp->result describes what went wrong.
 * TCL_RETURN		The command requests that the current procedure
 *			return;  interp->result contains the procedure's
 *			return value.
 * TCL_BREAK		The command requests that the innermost loop
 *			be exited;  interp->result is meaningless.
 * TCL_CONTINUE		Go on to the next iteration of the current loop;
 *			interp->result is meaninless.
 */

#define TCL_OK		0
#define TCL_ERROR	1
#define TCL_RETURN	2
#define TCL_BREAK	3
#define TCL_CONTINUE	4

#define TCL_RESULT_SIZE 199

/*
 * Procedure types defined by Tcl:
 */

typedef void (Tcl_CmdDeleteProc) _ANSI_ARGS_((ClientData clientData));
typedef int (Tcl_CmdProc) _ANSI_ARGS_((ClientData clientData,
	Tcl_Interp *interp, int argc, char *argv[]));
typedef void (Tcl_CmdTraceProc) _ANSI_ARGS_((ClientData clientData,
	Tcl_Interp *interp, int level, char *command, Tcl_CmdProc *proc,
	ClientData cmdClientData, int argc, char *argv[]));
typedef void (Tcl_FreeProc) _ANSI_ARGS_((char *blockPtr));
typedef char *(Tcl_VarTraceProc) _ANSI_ARGS_((ClientData clientData,
	Tcl_Interp *interp, char *name1, char *name2, int flags));

/*
 * Flag values passed to Tcl_Eval (see the man page for details;  also
 * see tclInt.h for additional flags that are only used internally by
 * Tcl):
 */

#define TCL_BRACKET_TERM	1

/*
 * Flag that may be passed to Tcl_ConvertElement to force it not to
 * output braces (careful!  if you change this flag be sure to change
 * the definitions at the front of tclUtil.c).
 */

#define TCL_DONT_USE_BRACES	1

/*
 * Flag value passed to Tcl_RecordAndEval to request no evaluation
 * (record only).
 */

#define TCL_NO_EVAL		-1

/*
 * Specil freeProc values that may be passed to Tcl_SetResult (see
 * the man page for details):
 */

#define TCL_VOLATILE	((Tcl_FreeProc *) -1)
#define TCL_STATIC	((Tcl_FreeProc *) 0)
#define TCL_DYNAMIC	((Tcl_FreeProc *) free)

/*
 * Flag values passed to variable-related procedures.
 */

#define TCL_GLOBAL_ONLY		1
#define TCL_APPEND_VALUE	2
#define TCL_LIST_ELEMENT	4
#define TCL_NO_SPACE		8
#define TCL_TRACE_READS		0x10
#define TCL_TRACE_WRITES	0x20
#define TCL_TRACE_UNSETS	0x40
#define TCL_TRACE_DESTROYED	0x80
#define TCL_INTERP_DESTROYED	0x100
#define TCL_LEAVE_ERR_MSG	0x200

/*
 * Additional flag passed back to variable watchers.  This flag must
 * not overlap any of the TCL_TRACE_* flags defined above or the
 * TRACE_* flags defined in tclInt.h.
 */

#define TCL_VARIABLE_UNDEFINED	8

/*
 * The following declarations either map ckalloc and ckfree to
 * malloc and free, or they map them to procedures with all sorts
 * of debugging hooks defined in tclCkalloc.c.
 */

#ifdef TCL_MEM_DEBUG

EXTERN char *		Tcl_DbCkalloc _ANSI_ARGS_((unsigned int size,
			    char *file, int line));
EXTERN int		Tcl_DbCkfree _ANSI_ARGS_((char *ptr,
			    char *file, int line));
#  define ckalloc(x) Tcl_DbCkalloc(x, __FILE__, __LINE__)
#  define ckfree(x)  Tcl_DbCkfree(x, __FILE__, __LINE__)

#else

#  define ckalloc(x) malloc(x)
#  define ckfree(x)  free(x)

#endif /* TCL_MEM_DEBUG */

/*
 * Macro to free up result of interpreter.
 */

#define Tcl_FreeResult(interp)					\
    if ((interp)->freeProc != 0) {				\
	if ((interp)->freeProc == (Tcl_FreeProc *) free) {	\
	    ckfree((interp)->result);				\
	} else {						\
	    (*(interp)->freeProc)((interp)->result);		\
	}							\
	(interp)->freeProc = 0;					\
    }

/*
 * Exported Tcl procedures:
 */

EXTERN void		Tcl_AppendElement _ANSI_ARGS_((Tcl_Interp *interp,
			    char *string, int noSep));
EXTERN void		Tcl_AppendResult _ANSI_ARGS_((Tcl_Interp *interp, ...));
EXTERN char *		Tcl_AssembleCmd _ANSI_ARGS_((Tcl_CmdBuf buffer,
			    char *string));
EXTERN void		Tcl_AddErrorInfo _ANSI_ARGS_((Tcl_Interp *interp,
			    char *message));
EXTERN char		Tcl_Backslash _ANSI_ARGS_((char *src,
			    int *readPtr));
EXTERN char *		Tcl_Concat _ANSI_ARGS_((int argc, char **argv));
EXTERN int		Tcl_ConvertElement _ANSI_ARGS_((char *src,
			    char *dst, int flags));
EXTERN Tcl_CmdBuf	Tcl_CreateCmdBuf _ANSI_ARGS_((void));
EXTERN void		Tcl_CreateCommand _ANSI_ARGS_((Tcl_Interp *interp,
			    char *cmdName, Tcl_CmdProc *proc,
			    ClientData clientData,
			    Tcl_CmdDeleteProc *deleteProc));
EXTERN Tcl_Interp *	Tcl_CreateInterp _ANSI_ARGS_((void));
EXTERN int		Tcl_CreatePipeline _ANSI_ARGS_((Tcl_Interp *interp,
			    int argc, char **argv, int **pidArrayPtr,
			    int *inPipePtr, int *outPipePtr,
			    int *errFilePtr));
EXTERN Tcl_Trace	Tcl_CreateTrace _ANSI_ARGS_((Tcl_Interp *interp,
			    int level, Tcl_CmdTraceProc *proc,
			    ClientData clientData));
EXTERN void		Tcl_DeleteCmdBuf _ANSI_ARGS_((Tcl_CmdBuf buffer));
EXTERN int		Tcl_DeleteCommand _ANSI_ARGS_((Tcl_Interp *interp,
			    char *cmdName));
EXTERN void		Tcl_DeleteInterp _ANSI_ARGS_((Tcl_Interp *interp));
EXTERN void		Tcl_DeleteTrace _ANSI_ARGS_((Tcl_Interp *interp,
			    Tcl_Trace trace));
EXTERN void		Tcl_DetachPids _ANSI_ARGS_((int numPids, int *pidPtr));
EXTERN int		Tcl_DumpActiveMemory _ANSI_ARGS_((char *fileName));
EXTERN char *		Tcl_ErrnoId _ANSI_ARGS_((void));
EXTERN int		Tcl_Eval _ANSI_ARGS_((Tcl_Interp *interp, char *cmd,
			    int flags, char **termPtr));
EXTERN int		Tcl_EvalFile _ANSI_ARGS_((Tcl_Interp *interp,
			    char *fileName));
EXTERN int		Tcl_ExprBoolean _ANSI_ARGS_((Tcl_Interp *interp,
			    char *string, int *ptr));
EXTERN int		Tcl_ExprDouble _ANSI_ARGS_((Tcl_Interp *interp,
			    char *string, double *ptr));
EXTERN int		Tcl_ExprLong _ANSI_ARGS_((Tcl_Interp *interp,
			    char *string, long *ptr));
EXTERN int		Tcl_ExprString _ANSI_ARGS_((Tcl_Interp *interp,
			    char *string));
EXTERN int		Tcl_Fork _ANSI_ARGS_((void));
EXTERN int		Tcl_GetBoolean _ANSI_ARGS_((Tcl_Interp *interp,
			    char *string, int *boolPtr));
EXTERN int		Tcl_GetDouble _ANSI_ARGS_((Tcl_Interp *interp,
			    char *string, double *doublePtr));
EXTERN int		Tcl_GetInt _ANSI_ARGS_((Tcl_Interp *interp,
			    char *string, int *intPtr));
EXTERN char *		Tcl_GetVar _ANSI_ARGS_((Tcl_Interp *interp,
			    char *varName, int flags));
EXTERN char *		Tcl_GetVar2 _ANSI_ARGS_((Tcl_Interp *interp,
			    char *name1, char *name2, int flags));
EXTERN int		Tcl_GlobalEval _ANSI_ARGS_((Tcl_Interp *interp,
			    char *command));
EXTERN void		Tcl_InitHistory _ANSI_ARGS_((Tcl_Interp *interp));
EXTERN void		Tcl_InitMemory _ANSI_ARGS_((Tcl_Interp *interp));
EXTERN char *		Tcl_Merge _ANSI_ARGS_((int argc, char **argv));
EXTERN char *		Tcl_ParseVar _ANSI_ARGS_((Tcl_Interp *interp,
			    char *string, char **termPtr));
EXTERN int		Tcl_RecordAndEval _ANSI_ARGS_((Tcl_Interp *interp,
			    char *cmd, int flags));
EXTERN void		Tcl_ResetResult _ANSI_ARGS_((Tcl_Interp *interp));
#define Tcl_Return Tcl_SetResult
EXTERN int		Tcl_ScanElement _ANSI_ARGS_((char *string,
			    int *flagPtr));
EXTERN void		Tcl_SetErrorCode _ANSI_ARGS_((Tcl_Interp *interp, ...));
EXTERN void		Tcl_SetResult _ANSI_ARGS_((Tcl_Interp *interp,
			    char *string, Tcl_FreeProc *freeProc));
EXTERN char *		Tcl_SetVar _ANSI_ARGS_((Tcl_Interp *interp,
			    char *varName, char *newValue, int flags));
EXTERN char *		Tcl_SetVar2 _ANSI_ARGS_((Tcl_Interp *interp,
			    char *name1, char *name2, char *newValue,
			    int flags));
EXTERN char *		Tcl_SignalId _ANSI_ARGS_((int sig));
EXTERN char *		Tcl_SignalMsg _ANSI_ARGS_((int sig));
EXTERN int		Tcl_SplitList _ANSI_ARGS_((Tcl_Interp *interp,
			    char *list, int *argcPtr, char ***argvPtr));
EXTERN int		Tcl_StringMatch _ANSI_ARGS_((char *string,
			    char *pattern));
EXTERN char *		Tcl_TildeSubst _ANSI_ARGS_((Tcl_Interp *interp,
			    char *name));
EXTERN int		Tcl_TraceVar _ANSI_ARGS_((Tcl_Interp *interp,
			    char *varName, int flags, Tcl_VarTraceProc *proc,
			    ClientData clientData));
EXTERN int		Tcl_TraceVar2 _ANSI_ARGS_((Tcl_Interp *interp,
			    char *name1, char *name2, int flags,
			    Tcl_VarTraceProc *proc, ClientData clientData));
EXTERN char *		Tcl_UnixError _ANSI_ARGS_((Tcl_Interp *interp));
EXTERN int		Tcl_UnsetVar _ANSI_ARGS_((Tcl_Interp *interp,
			    char *varName, int flags));
EXTERN int		Tcl_UnsetVar2 _ANSI_ARGS_((Tcl_Interp *interp,
			    char *name1, char *name2, int flags));
EXTERN void		Tcl_UntraceVar _ANSI_ARGS_((Tcl_Interp *interp,
			    char *varName, int flags, Tcl_VarTraceProc *proc,
			    ClientData clientData));
EXTERN void		Tcl_UntraceVar2 _ANSI_ARGS_((Tcl_Interp *interp,
			    char *name1, char *name2, int flags,
			    Tcl_VarTraceProc *proc, ClientData clientData));
EXTERN int		Tcl_VarEval _ANSI_ARGS_((Tcl_Interp *interp, ...));
EXTERN ClientData	Tcl_VarTraceInfo _ANSI_ARGS_((Tcl_Interp *interp,
			    char *varName, int flags,
			    Tcl_VarTraceProc *procPtr,
			    ClientData prevClientData));
EXTERN ClientData	Tcl_VarTraceInfo2 _ANSI_ARGS_((Tcl_Interp *interp,
			    char *name1, char *name2, int flags,
			    Tcl_VarTraceProc *procPtr,
			    ClientData prevClientData));
EXTERN int		Tcl_WaitPids _ANSI_ARGS_((int numPids, int *pidPtr,
			    int *statusPtr));

#endif /* _TCL */
