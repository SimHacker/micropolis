/* 
 * tclXmatherr.c --
 *
 *   Extended Tcl default matherr routine, may be replace by an application
 *   specified version.
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
 * $Id: tclXmatherr.c,v 2.1 1992/11/09 07:33:02 markd Exp $
 *-----------------------------------------------------------------------------
 */
#include "tclxtend.h"

#ifndef TCL_IEEE_FP_MATH

#include <math.h>

/*
 *-----------------------------------------------------------------------------
 *
 * matherr --
 *    Default matherr routine for extended Tcl.  If the error does not
 *  belong to Tcl, standard math error processing is done.  An
 *  aplication may replace this routine with their own, however it must
 *  call \fBTcl_MathError\fR to check if the error belongs to tcl.
 *
 *-----------------------------------------------------------------------------
 */
int
matherr (except)
    struct exception *except;
{
    if (Tcl_MathError (except->name, except->type))
        return 1;
    else
        return 0;
}

#else

/*
 *-----------------------------------------------------------------------------
 *
 * matherr --
 *    Dummy matherr for systems wanting to use IEEE 745-1985 floating point
 * math error reporting.  This just disables standard Unix matherr message
 * printing by returning 1.  Except parameter not specified, since some systems
 * don't define it or even use matherr.
 *
 *-----------------------------------------------------------------------------
 */
int
matherr ()
{
    return 1;
}

#endif /* TCL_IEEE_FP_MATH */
