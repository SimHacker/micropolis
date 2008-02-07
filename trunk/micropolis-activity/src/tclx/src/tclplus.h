/*
 * tcl++.h --
 *
 * This file defines a C++ classes that can be used to access a Tcl
 * interpreter. If tcl.h is not already included, it includes it. Tcl.h has
 * macros that allow it to work with K&R C, ANSI C and C++.
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
 * Based on Tcl C++ classes developed by Parag Patel.
 *-----------------------------------------------------------------------------
 * $Id: tcl++.h,v 2.0 1992/10/16 04:51:25 markd Rel $
 *-----------------------------------------------------------------------------
 */

#ifndef _TCL_PLUS_PLUS_H
#define _TCL_PLUS_PLUS_H

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#ifndef TCLEXTEND_H
#    include "tclxtend.h"
#endif

class TclInterp_cl
{
    Tcl_Interp *interp;

    friend class TclTrace_cl;

private:
    char *
    CatVarArgs (va_list  argPtr);

public:
    inline 
    TclInterp_cl () 
    {
        interp = Tcl_CreateExtendedInterp ();
    }

    inline 
    ~TclInterp_cl () 
    { 
        Tcl_DeleteInterp (interp);
    }

    inline char *
    Result () 
    { 
        return interp->result; 
    }

    inline int 
    ErrorLine () 
    { 
        return interp->errorLine;
    }

    inline Tcl_Interp *
    GetInterp () 
    {
        return interp;
    }

   /*
    * Exported Tcl procedures (from standard baseline):
    */

    inline void
    AppendElement (const char *string,
                   int         noSep)
    { 
        Tcl_AppendElement (interp, (char *) string, noSep);
    }

    void
    AppendResult (const char *p,
                  ...);

    inline void
    AddErrorInfo (const char *message)
    {
        Tcl_AddErrorInfo (interp, (char *)message);
    }

    inline void 
    CreateCommand (const char        *cmdName,
                   Tcl_CmdProc       *proc, 
                   ClientData         data, 
                   Tcl_CmdDeleteProc *deleteProc)
    { 
        Tcl_CreateCommand (interp, (char*) cmdName, proc, data, deleteProc);
    }

    inline int
    CreatePipeline (int    argc, 
                    char **argv, 
                    int  **pidArrayPtr,
                    int   *inPipePtr, 
                    int   *outPipePtr,
                    int   *errFilePtr)
    {
        return Tcl_CreatePipeline (interp, argc, argv, pidArrayPtr, inPipePtr, 
                                   outPipePtr, errFilePtr);
    }

    inline Tcl_Trace 
    CreateTrace (int               level, 
                 Tcl_CmdTraceProc *proc, 
                 ClientData        data)
    {
        return Tcl_CreateTrace (interp, level, proc, data);
    }

    inline void 
    DeleteCommand (const char *cmdName)
    { 
        Tcl_DeleteCommand (interp, (char *)cmdName);
    }

    inline void 
    DeleteTrace (Tcl_Trace trace)
    { 
        Tcl_DeleteTrace(interp, trace);
    }

    inline void
    DetachPids (int  numPids, 
                int *pidPtr)
    {
        Tcl_DetachPids (numPids, pidPtr);
    }

    inline int 
    Eval (const char   *cmd, 
          char          flags = 0, 
          char        **termPtr = NULL)
        { return Tcl_Eval (interp, (char *) cmd, flags, termPtr); }

    inline int
    EvalFile (const char *fileName)
    { 
        return Tcl_EvalFile (interp, (char *) fileName);
    }

    inline int
    ExprBoolean (const char *string,
                 int        *ptr)
    {
        return Tcl_ExprBoolean (interp, (char *) string, ptr);
    }

    inline int
    ExprDouble (const char *string,
                double     *ptr)
    { 
        return Tcl_ExprDouble (interp, (char *) string, ptr);
    }

    inline int
    ExprLong (const char *string,
              long       *ptr)
    {
        return Tcl_ExprLong (interp, (char *) string, ptr);
    }

    inline int
    ExprString (const char *string)
    {
        return Tcl_ExprString (interp, (char *) string);
    }

    inline int
    GetBoolean (Tcl_Interp *interp,
                const char *string,
                int        *boolPtr)
    { 
        return Tcl_GetBoolean (interp, (char *) string, boolPtr);
    }

    inline int
    GetDouble (Tcl_Interp *interp,
               const char *string,
               double     *doublePtr)
    { 
        return Tcl_GetDouble (interp, (char *) string, doublePtr);
    }

    inline int
    GetInt (Tcl_Interp *interp,
            const char *string,
            int        *intPtr)
    { 
        return Tcl_GetInt (interp, (char *) string, intPtr);
    }

    inline const char *
    GetVar (const char *varName, 
            int         flags = 0)
    { 
        return Tcl_GetVar (interp, (char *) varName, flags);
    }

    inline const char *
    GetVar2 (const char *name1, 
             const char *name2,
             int         flags = 0)
    { 
        return Tcl_GetVar2 (interp, (char *) name1, (char *) name2, flags); 
    }

    inline void
    InitMemory ()
    {
        Tcl_InitMemory (interp);
    }

    inline char *
    ParseVar (const char  *string, 
              char       **termPtr)
       { return Tcl_ParseVar (interp, (char *) string, termPtr); }

    inline int 
    RecordAndEval (const char *cmd, 
                   char        flags)
        { return Tcl_RecordAndEval (interp, (char *) cmd, flags); }

    inline void
    ResetResult ()
    {
        Tcl_ResetResult (interp);
    }

    void
    SetErrorCode (char *p, 
                  ...);

    inline void 
    SetResult (const char *string)
    {
        Tcl_SetResult (interp, (char *) string, TCL_VOLATILE);
    }

    inline void 
    SetResult (const char   *string, 
               Tcl_FreeProc *freeProc)
    { 
        Tcl_SetResult (interp, (char *) string, freeProc);
    }

    inline const char *
    SetVar (const char  *varName, 
            const char  *newValue, 
            int          global = 0)
    { 
        return Tcl_SetVar (interp, (char *) varName, (char *) newValue, 
                           global);
    }

    inline const char *
    SetVar2 (const char *name1, 
             const char *name2, 
             const char *newValue, 
             int         global)
    {
         return Tcl_SetVar2 (interp, (char *) name1, (char *) name2, 
                             (char *) newValue, global);
    }

    inline int 
    SplitList (const char   *list, 
               int          &argcP, 
               char       **&argvP)
        { return Tcl_SplitList (interp, (char *) list, &argcP, &argvP); }

    inline char *
    TildeSubst (Tcl_Interp *interp,
                const char *name)
        { return Tcl_TildeSubst (interp, (char *) name); }

    int
    TraceVar (const char       *varName,
              int               flags,
              Tcl_VarTraceProc *proc,
              ClientData        clientData)
    {
         return Tcl_TraceVar (interp, (char *) varName, flags, proc,
                              clientData);
    }

    inline int
    TraceVar2 (const char       *name1, 
               const char       *name2,
               int               flags, 
               Tcl_VarTraceProc *proc, 
               ClientData        clientData)
    {
         return Tcl_TraceVar2 (interp, (char *) name1, (char *) name2, flags,
                               proc, clientData); 
    }

    inline const char *
    UnixError ()
    {
        return Tcl_UnixError (interp);
    }

    inline void
    UnsetVar (Tcl_Interp *interp,
              const char *varName,
              int         global)
        { Tcl_UnsetVar (interp, (char *) varName, global); }

    inline void
    UnsetVar2 (Tcl_Interp *interp,
               const char *name1, 
               const char *name2, 
               int         global)
        { Tcl_UnsetVar2 (interp, (char *) name1, (char *) name2, global); }

    inline void
    UntraceVar (const char       *varName, 
                int               flags,
                Tcl_VarTraceProc *proc, 
                ClientData        clientData)
    {
        Tcl_UntraceVar (interp, (char *) varName, flags, proc, clientData);
    }


    inline void
    UntraceVar2 (Tcl_Interp       *interp,
                 const char       *name1,
                 const char       *name2,
                 int               flags, 
                 Tcl_VarTraceProc *proc, 
                 ClientData        clientData)
    { 
        Tcl_UntraceVar2 (interp, (char *) name1, (char *) name2, flags, proc,
                         clientData);
    }

    int
    VarEval (const char *p,
              ...);

    inline ClientData
    VarTraceInfo (const char       *varName,
                  int               flags,
                  Tcl_VarTraceProc *procPtr,
                  ClientData        prevClientData)
    { 
        return Tcl_VarTraceInfo (interp, (char *) varName, flags, procPtr,
                                 prevClientData);
    }

    inline ClientData
    VarTraceInfo2 (const char       *name1, 
                   const char       *name2, 
                   int               flags,
                   Tcl_VarTraceProc *procPtr,
                   ClientData        prevClientData)
    { 
        return Tcl_VarTraceInfo2 (interp, (char *) name1, (char *) name2, 
                                  flags, procPtr, prevClientData);
    }

    /*
     * Exported Tcl functions added to baseline Tcl by the Extended Tcl 
     * implementation.
     */

    inline void
    CommandLoop (FILE       *inFile,
                 FILE       *outFile,
                 int         (*evalProc) (),
                 unsigned    options)
    {
        Tcl_CommandLoop (interp, inFile, outFile, evalProc, options);
    }

    inline char *
    DeleteKeyedListField (const char  *fieldName,
                          const char  *keyedList)
    {
        return Tcl_DeleteKeyedListField (interp, fieldName, keyedList);
    }

    inline void
    ErrorAbort (int  exitCode)
    {
        Tcl_ErrorAbort (interp, exitCode);
    }

    inline int
    GetKeyedListField (const char  *fieldName,
                       const char  *keyedList,
                       char       **fieldValuePtr)
    {
        return Tcl_GetKeyedListField (interp, fieldName, keyedList,
                                      fieldValuePtr);
    }

    inline int
    GetKeyedListKeys (const char  *subFieldName,
                      const char  *keyedList,
                      int         *keyesArgcPtr,
                      char      ***keyesArgvPtr)
    {

        return Tcl_GetKeyedListKeys (interp, subFieldName, keyedList,
                                     keyesArgcPtr, keyesArgvPtr);
    }

    inline int
    GetLong (Tcl_Interp *interp,
             const char *string,
             long       *longPtr)
    {
        return Tcl_GetLong (interp, string, longPtr);
    }

    inline int
    GetUnsigned (Tcl_Interp *interp,
                 const char *string,
                 unsigned   *unsignedPtr)
    {
         return Tcl_GetUnsigned (interp, string, unsignedPtr);
    }

    inline char *
    SetKeyedListField (const char  *fieldName,
                       const char  *fieldvalue,
                       const char  *keyedList)
    {
        return Tcl_SetKeyedListField (interp, fieldName, fieldvalue,
                                      keyedList);
    }

    inline int
    ShellEnvInit (unsigned     options,
                  const char  *programName, 
                  int          argc,
                  const char **argv,
                  int          interactive,
                  const char  *defaultFile)
     {
         return Tcl_ShellEnvInit (interp, options, programName, argc, argv,
                                  interactive, defaultFile);
     }

    inline void 
    Startup (int          argc,
             const char **argv,
             const char  *defaultFile,
             unsigned     options = 0) 
    {
        Tcl_Startup (interp, argc, argv, defaultFile, options);
    }

    inline int
    System (const char *command)
    {
        return Tcl_System (interp, (char*) command);
    }
};

class TclTrace_cl
{
    Tcl_Trace trace;
    Tcl_Interp *interp;

public:
    inline 
    TclTrace_cl (TclInterp_cl     &interpCl, 
                 int               level, 
                 Tcl_CmdTraceProc *proc, 
                 ClientData        data)
    { 
        trace = Tcl_CreateTrace (interp = interpCl.interp, level, proc, data);
    }

    inline ~TclTrace_cl () 
        { Tcl_DeleteTrace (interp, trace); }
};


class TclCmdBuf_cl
{
    Tcl_CmdBuf buf;

public:
    inline 
    TclCmdBuf_cl () 
        { buf = Tcl_CreateCmdBuf (); }

    inline 
    ~TclCmdBuf_cl () 
        { Tcl_DeleteCmdBuf (buf); }

    inline const char *
    AssembleCmd (const char *string)
        { return Tcl_AssembleCmd (buf, (char *) string); }
};

class TclHandleTbl_cl
{
    void_pt headerPtr;

public:
    inline
    TclHandleTbl_cl (const char *handleBase,
                     int         entrySize,
                     int         initEntries)

    {
        headerPtr = Tcl_HandleTblInit (handleBase, entrySize, initEntries);
    }

    inline
    ~TclHandleTbl_cl ()
    {
        Tcl_HandleTblRelease (headerPtr);
    }

    inline void_pt  
    HandleAlloc (char *handlePtr)
    {
        return Tcl_HandleAlloc (headerPtr, handlePtr);
    }

    inline void 
    HandleFree (void_pt  entryPtr)
    {
        Tcl_HandleFree (headerPtr, entryPtr);
    }

    inline int
    HandleTblUseCount (int amount)
    {
        return Tcl_HandleTblUseCount (headerPtr, amount);
    }

    inline void_pt
    HandleWalk (int *walkKeyPtr)
    {
        return Tcl_HandleWalk (headerPtr, walkKeyPtr);
    }

    inline void_pt
    HandleXlate (Tcl_Interp  *interp,
                 const  char *handle)
    {
        return Tcl_HandleXlate (interp, headerPtr, handle);
    }
};

#endif /* _TCL_PLUS_PLUS_H */
 

