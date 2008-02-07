/*
 * tcl++.C --
 *
 * It also defines C++ classes that can be used to access a Tcl interpreter.
 * If tcl.h is not already included, it includes it. Tcl.h  has macros that
 * allow it to work with K&R C, ANSI C and C++.
 *---------------------------------------------------------------------------
 * Copyright 1992 Karl Lehenbauer and Mark Diekhans.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies.  Karl Lehenbauer and
 * Mark Diekhans make no representations about the suitability of this
 * software for any purpose.  It is provided "as is" without express or
 * implied warranty.
 *---------------------------------------------------------------------------
 * Based on Tcl C++ classes developed by Parag Patel.
 *-----------------------------------------------------------------------------
 * $Id: tcl++.C,v 2.0 1992/10/16 04:51:32 markd Rel $
 *-----------------------------------------------------------------------------
 */

#include "tclplus.h"

/*
 *----------------------------------------------------------------------
 *
 * TclInterp_cl::CatVarArgs --
 *
 * Concatenate all of the strings passed via varargs into a single string.
 *
 * Parameters:
 *   o argPtr (I) - A pointer to the first argument, as returned by va_start.
 *     Should be terminate by a NULL.
 *
 * Returns:
 *   A dynamically allocated string.
 *----------------------------------------------------------------------
 */
char *
TclInterp_cl::CatVarArgs (va_list argPtr)
{
    int      len = 0;
    char    *parmPtr, *ptr;
    va_list  nextArgPtr = argPtr;

    while (1) {
        parmPtr = va_arg (nextArgPtr, char *);
        if (parmPtr == NULL)
            break;
        len += strlen (parmPtr);
    }
    ptr = (char *) ckalloc (len + 1);
    ptr [0] = '\0';
    nextArgPtr = argPtr;
    while (1) {
        parmPtr = va_arg (nextArgPtr, char *);
        if (parmPtr == NULL)
            break;
        strcat (ptr, parmPtr);
    }
    return ptr;
}        

/*
 *----------------------------------------------------------------------
 *
 * TclInterp_cl::AppendResult --
 *
 *    Class interface to Tcl_AppendResult (see Tcl documentation for
 * details).  Not inlined since varargs and inline don't work on some C++
 * compilers.
 *----------------------------------------------------------------------
 */
void
TclInterp_cl::AppendResult (const char *p,
                            ...)
{
    va_list  argPtr;
    char    *strPtr;

    va_start (argPtr, p);
    strPtr = CatVarArgs (argPtr);
    Tcl_AppendResult (interp, p, strPtr, (char *) NULL);
    ckfree (strPtr)
    va_end (argPtr);
}

/*
 *----------------------------------------------------------------------
 *
 * TclInterp_cl::SetErrorCode --
 *
 *    Class interface to Tcl_SetErrorCode (see Tcl documentation for
 * details).  Not inlined since varargs and inline don't work on some C++
 * compilers.
 *----------------------------------------------------------------------
 */
void
TclInterp_cl::SetErrorCode (char *p, 
                            ...)
{
    va_list  argPtr;
    char    *strPtr;

    va_start (argPtr, p);
    strPtr = CatVarArgs (argPtr);
    Tcl_SetErrorCode (interp, p, strPtr, (char *) NULL);
    ckfree (strPtr)
    va_end (argPtr);
}

/*
 *----------------------------------------------------------------------
 *
 * TclInterp_cl::VarEval --
 *
 *    Class interface to Tcl_VarEval (see Tcl documentation for details).
 *  Not inlined since varargs and inline don't work on some C++ compilers.
 *----------------------------------------------------------------------
 */
int
TclInterp_cl::VarEval (const char *p,
                       ...)
{
    int      intResult;
    va_list  argPtr;
    char    *strPtr;

    va_start (argPtr, p);
    strPtr = CatVarArgs (argPtr);
    intResult = Tcl_VarEval (interp, (char *) p, strPtr, (char *) NULL);
    ckfree (strPtr);
    va_end (argPtr);
    return intResult;
}

