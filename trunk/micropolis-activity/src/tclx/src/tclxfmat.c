/* 
 * tclXfmath.c --
 *
 *      Contains the TCL trig and floating point math functions.
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
 * $Id: tclXfmath.c,v 2.5 1992/11/09 07:58:13 markd Exp $
 *-----------------------------------------------------------------------------
 */

#include "tclxint.h"
#include <math.h>

/*
 * Flag used to indicate if a floating point math routine is currently being
 * executed.  Used to determine if a matherr belongs to Tcl.
 */
static int G_inTclFPMath = FALSE;

/*
 * Flag indicating if a floating point math error occured during the execution
 * of a library routine called by a Tcl command.  Will not be set by the trap
 * handler if the error did not occur while the `G_inTclFPMath' flag was
 * set.  If the error did occur the error type and the name of the function
 * that got the error are save here.
 */
static int   G_gotTclFPMathErr = FALSE;
static int   G_errorType;

/*
 * Prototypes of internal functions.
 */
#ifdef TCL_IEEE_FP_MATH
static int
ReturnIEEEMathError _ANSI_ARGS_((Tcl_Interp *interp,
                                 double      dbResult));
#else
static int
ReturnFPMathError _ANSI_ARGS_((Tcl_Interp *interp));
#endif

static int
Tcl_UnaryFloatFunction _ANSI_ARGS_((Tcl_Interp *interp,
                                    int         argc,
                                    char      **argv,
                                    double (*function)()));


#ifdef TCL_IEEE_FP_MATH

/*
 *-----------------------------------------------------------------------------
 *
 * ReturnIEEEMathError --
 *    Handle return of floating point errors on machines that use IEEE 745-1985
 * error reporting instead of Unix matherr.  Some machines support both and
 * on these, either option may be used.
 *    Various tests are used to determine if a number is one of the special
 * values.  Not-a-number is tested by comparing the number against itself
 * (x != x if x is NaN).  Infinity is tested for by comparing against MAXDOUBLE.
 *
 * Parameters:
 *   o interp (I) - Error is returned in result.
 *   o dbResult (I) - Result of a function call that returned a special value.
 * Returns:
 *   Always returns the value TCL_ERROR, so if can be called as the
 * argument to `return'.
 *-----------------------------------------------------------------------------
 */
static int
ReturnIEEEMathError (interp, dbResult)
    Tcl_Interp *interp;
    double      dbResult;
{
    char *errorMsg;

    if (dbResult != dbResult)
        errorMsg = "domain";
    else if (dbResult > MAXDOUBLE)
        errorMsg = "overflow";
    else if (dbResult < -MAXDOUBLE)
        errorMsg = "underflow";

    Tcl_AppendResult (interp, "floating point ", errorMsg, " error",
                      (char *) NULL);
    return TCL_ERROR;
}
#else

/*
 *-----------------------------------------------------------------------------
 *
 * ReturnFPMathError --
 *    Routine to set an interpreter result to contain a floating point
 * math error message.  Will clear the `G_gotTclFPMathErr' flag.
 * This routine always returns the value TCL_ERROR, so if can be called
 * as the argument to `return'.
 *
 * Parameters:
 *   o interp (I) - Error is returned in result.
 * Globals:
 *   o G_gotTclFPMathErr (O) - Flag indicating an error occured, will be 
 *     cleared.
 *   o G_errorType (I) - Type of error that occured.
 * Returns:
 *   Always returns the value TCL_ERROR, so if can be called as the
 * argument to `return'.
 *-----------------------------------------------------------------------------
 */
static int
ReturnFPMathError (interp)
    Tcl_Interp *interp;
{
    char *errorMsg;

    switch (G_errorType) {
       case DOMAIN: 
           errorMsg = "domain";
           break;
       case SING:
           errorMsg = "singularity";
           break;
       case OVERFLOW:
           errorMsg = "overflow";
           break;
       case UNDERFLOW:
           errorMsg = "underflow";
           break;
       case TLOSS:
       case PLOSS:
           errorMsg = "loss of significance";
           break;
    }
    Tcl_AppendResult (interp, "floating point ", errorMsg, " error",
                      (char *) NULL);
    G_gotTclFPMathErr = FALSE;  /* Clear the flag. */
    return TCL_ERROR;
}
#endif /* NO_MATH_ERR */

/*
 *-----------------------------------------------------------------------------
 *
 * Tcl_MathError --
 *    Tcl math error handler, should be called by an application `matherr'
 *    routine to determine if an error was caused by Tcl code or by other
 *    code in the application.  If the error occured in Tcl code, flags will
 *    be set so that a standard Tcl interpreter error can be returned.
 *
 * Paramenter:
 *   o functionName (I) - The name of the function that got the error.  From
 *     the exception structure supplied to matherr.
 *   o errorType (I) - The type of error that occured.  From the exception 
 *     structure supplied to matherr.
 * Results:
 *      Returns TRUE if the error was in Tcl code, in which case the
 *   matherr routine calling this function should return non-zero so no
 *   error message will be generated.  FALSE if the error was not in Tcl
 *   code, in which case the matherr routine can handle the error in any
 *   manner it choses.
 *
 *-----------------------------------------------------------------------------
 */
int
Tcl_MathError (functionName, errorType)
    char *functionName;
    int   errorType;
{

  if (G_inTclFPMath) {
      G_gotTclFPMathErr = TRUE;
      G_errorType = errorType;
      return TRUE;
  } else
      return FALSE;
  
}

/*
 *-----------------------------------------------------------------------------
 *
 * Tcl_UnaryFloatFunction --
 *     Helper routine that implements Tcl unary floating point
 *     functions by validating parameters, converting the
 *     argument, applying the function (the address of which
 *     is passed as an argument), and converting the result to
 *     a string and storing it in the result buffer
 *
 * Results:
 *      Returns TCL_OK if number is present, conversion succeeded,
 *        the function was performed, etc.
 *      Return TCL_ERROR for any error; an appropriate error message
 *        is placed in the result string in this case.
 *
 *-----------------------------------------------------------------------------
 */
static int
Tcl_UnaryFloatFunction(interp, argc, argv, function)
    Tcl_Interp *interp;
    int         argc;
    char      **argv;
    double    (*function)();
{
    double dbVal, dbResult;

    if (argc != 2) {
        Tcl_AppendResult (interp, tclXWrongArgs, argv [0], " expr",
                          (char *) NULL);
        return TCL_ERROR;
    }

    if (Tcl_ExprDouble (interp, argv [1], &dbVal) != TCL_OK)
        return TCL_ERROR;

    G_inTclFPMath = TRUE;
    dbResult = (*function)(dbVal);
    G_inTclFPMath = FALSE;

#ifdef TCL_IEEE_FP_MATH
    if ((dbResult != dbResult) ||
        (dbResult < -MAXDOUBLE) ||
        (dbResult >  MAXDOUBLE))
        return ReturnIEEEMathError (interp, dbResult);
#else
    if (G_gotTclFPMathErr)
        return ReturnFPMathError (interp);
#endif

    Tcl_ReturnDouble (interp, dbResult);
    return TCL_OK;
}

/*
 *-----------------------------------------------------------------------------
 *
 * Tcl_AcosCmd --
 *    Implements the TCL arccosine command:
 *        acos num
 *
 * Results:
 *      Returns TCL_OK if number is present and conversion succeeds.
 *
 *-----------------------------------------------------------------------------
 */
int
Tcl_AcosCmd(clientData, interp, argc, argv)
    ClientData  clientData;
    Tcl_Interp *interp;
    int         argc;
    char      **argv;
{
    return Tcl_UnaryFloatFunction(interp, argc, argv, acos);
}

/*
 *-----------------------------------------------------------------------------
 *
 * Tcl_AsinCmd --
 *    Implements the TCL arcsin command:
 *        asin num
 *
 * Results:
 *      Returns TCL_OK if number is present and conversion succeeds.
 *
 *-----------------------------------------------------------------------------
 */
int
Tcl_AsinCmd(clientData, interp, argc, argv)
    ClientData  clientData;
    Tcl_Interp *interp;
    int         argc;
    char      **argv;
{
    return Tcl_UnaryFloatFunction(interp, argc, argv, asin);
}

/*
 *-----------------------------------------------------------------------------
 *
 * Tcl_AtanCmd --
 *    Implements the TCL arctangent command:
 *        atan num
 *
 * Results:
 *      Returns TCL_OK if number is present and conversion succeeds.
 *
 *-----------------------------------------------------------------------------
 */
int
Tcl_AtanCmd(clientData, interp, argc, argv)
    ClientData  clientData;
    Tcl_Interp *interp;
    int         argc;
    char      **argv;
{
    return Tcl_UnaryFloatFunction(interp, argc, argv, atan);
}

/*
 *-----------------------------------------------------------------------------
 *
 * Tcl_CosCmd --
 *    Implements the TCL cosine command:
 *        cos num
 *
 * Results:
 *      Returns TCL_OK if number is present and conversion succeeds.
 *
 *-----------------------------------------------------------------------------
 */
int
Tcl_CosCmd(clientData, interp, argc, argv)
    ClientData  clientData;
    Tcl_Interp *interp;
    int         argc;
    char      **argv;
{
    return Tcl_UnaryFloatFunction(interp, argc, argv, cos);
}

/*
 *-----------------------------------------------------------------------------
 *
 * Tcl_SinCmd --
 *    Implements the TCL sin command:
 *        sin num
 *
 * Results:
 *      Returns TCL_OK if number is present and conversion succeeds.
 *
 *-----------------------------------------------------------------------------
 */
int
Tcl_SinCmd(clientData, interp, argc, argv)
    ClientData  clientData;
    Tcl_Interp *interp;
    int         argc;
    char      **argv;
{
    return Tcl_UnaryFloatFunction(interp, argc, argv, sin);
}

/*
 *-----------------------------------------------------------------------------
 *
 * Tcl_TanCmd --
 *    Implements the TCL tangent command:
 *        tan num
 *
 * Results:
 *      Returns TCL_OK if number is present and conversion succeeds.
 *
 *-----------------------------------------------------------------------------
 */
int
Tcl_TanCmd(clientData, interp, argc, argv)
    ClientData  clientData;
    Tcl_Interp *interp;
    int         argc;
    char      **argv;
{
    return Tcl_UnaryFloatFunction(interp, argc, argv, tan);
}

/*
 *-----------------------------------------------------------------------------
 *
 * Tcl_CoshCmd --
 *    Implements the TCL hyperbolic cosine command:
 *        cosh num
 *
 * Results:
 *      Returns TCL_OK if number is present and conversion succeeds.
 *
 *-----------------------------------------------------------------------------
 */
int
Tcl_CoshCmd(clientData, interp, argc, argv)
    ClientData  clientData;
    Tcl_Interp *interp;
    int         argc;
    char      **argv;
{
    return Tcl_UnaryFloatFunction(interp, argc, argv, cosh);
}

/*
 *-----------------------------------------------------------------------------
 *
 * Tcl_SinhCmd --
 *    Implements the TCL hyperbolic sin command:
 *        sinh num
 *
 * Results:
 *      Returns TCL_OK if number is present and conversion succeeds.
 *
 *-----------------------------------------------------------------------------
 */
int
Tcl_SinhCmd(clientData, interp, argc, argv)
    ClientData  clientData;
    Tcl_Interp *interp;
    int         argc;
    char      **argv;
{
    return Tcl_UnaryFloatFunction(interp, argc, argv, sinh);
}

/*
 *-----------------------------------------------------------------------------
 *
 * Tcl_TanhCmd --
 *    Implements the TCL hyperbolic tangent command:
 *        tanh num
 *
 * Results:
 *      Returns TCL_OK if number is present and conversion succeeds.
 *
 *-----------------------------------------------------------------------------
 */
int
Tcl_TanhCmd(clientData, interp, argc, argv)
    ClientData  clientData;
    Tcl_Interp *interp;
    int         argc;
    char      **argv;
{
    return Tcl_UnaryFloatFunction(interp, argc, argv, tanh);
}

/*
 *-----------------------------------------------------------------------------
 *
 * Tcl_ExpCmd --
 *    Implements the TCL exponent command:
 *        exp num
 *
 * Results:
 *      Returns TCL_OK if number is present and conversion succeeds.
 *
 *-----------------------------------------------------------------------------
 */
int
Tcl_ExpCmd(clientData, interp, argc, argv)
    ClientData  clientData;
    Tcl_Interp *interp;
    int         argc;
    char      **argv;
{
    return Tcl_UnaryFloatFunction(interp, argc, argv, exp);
}

/*
 *-----------------------------------------------------------------------------
 *
 * Tcl_LogCmd --
 *    Implements the TCL logarithm command:
 *        log num
 *
 * Results:
 *      Returns TCL_OK if number is present and conversion succeeds.
 *
 *-----------------------------------------------------------------------------
 */
int
Tcl_LogCmd(clientData, interp, argc, argv)
    ClientData  clientData;
    Tcl_Interp *interp;
    int         argc;
    char      **argv;
{
    return Tcl_UnaryFloatFunction(interp, argc, argv, log);
}

/*
 *-----------------------------------------------------------------------------
 *
 * Tcl_Log10Cmd --
 *    Implements the TCL base-10 logarithm command:
 *        log10 num
 *
 * Results:
 *      Returns TCL_OK if number is present and conversion succeeds.
 *
 *-----------------------------------------------------------------------------
 */
int
Tcl_Log10Cmd(clientData, interp, argc, argv)
    ClientData  clientData;
    Tcl_Interp *interp;
    int         argc;
    char      **argv;
{
    return Tcl_UnaryFloatFunction(interp, argc, argv, log10);
}

/*
 *-----------------------------------------------------------------------------
 *
 * Tcl_SqrtCmd --
 *    Implements the TCL square root command:
 *        sqrt num
 *
 * Results:
 *      Returns TCL_OK if number is present and conversion succeeds.
 *
 *-----------------------------------------------------------------------------
 */
int
Tcl_SqrtCmd(clientData, interp, argc, argv)
    ClientData  clientData;
    Tcl_Interp *interp;
    int         argc;
    char      **argv;
{
    return Tcl_UnaryFloatFunction(interp, argc, argv, sqrt);
}

/*
 *-----------------------------------------------------------------------------
 *
 * Tcl_FabsCmd --
 *    Implements the TCL floating point absolute value command:
 *        fabs num
 *
 * Results:
 *      Returns TCL_OK if number is present and conversion succeeds.
 *
 *-----------------------------------------------------------------------------
 */
int
Tcl_FabsCmd(clientData, interp, argc, argv)
    ClientData  clientData;
    Tcl_Interp *interp;
    int         argc;
    char      **argv;
{
    return Tcl_UnaryFloatFunction(interp, argc, argv, fabs);
}

/*
 *-----------------------------------------------------------------------------
 *
 * Tcl_FloorCmd --
 *    Implements the TCL floor command:
 *        floor num
 *
 * Results:
 *      Returns TCL_OK if number is present and conversion succeeds.
 *
 *-----------------------------------------------------------------------------
 */
int
Tcl_FloorCmd(clientData, interp, argc, argv)
    ClientData  clientData;
    Tcl_Interp *interp;
    int         argc;
    char      **argv;
{
    return Tcl_UnaryFloatFunction(interp, argc, argv, floor);
}

/*
 *-----------------------------------------------------------------------------
 *
 * Tcl_CeilCmd --
 *    Implements the TCL ceil command:
 *        ceil num
 *
 * Results:
 *      Returns TCL_OK if number is present and conversion succeeds.
 *
 *-----------------------------------------------------------------------------
 */
int
Tcl_CeilCmd(clientData, interp, argc, argv)
    ClientData  clientData;
    Tcl_Interp *interp;
    int         argc;
    char      **argv;
{
    return Tcl_UnaryFloatFunction(interp, argc, argv, ceil);
}

/*
 *-----------------------------------------------------------------------------
 *
 * Tcl_FmodCmd --
 *    Implements the TCL floating modulo command:
 *        fmod num1 num2
 *
 * Results:
 *      Returns TCL_OK if number is present and conversion succeeds.
 *
 *-----------------------------------------------------------------------------
 */
int
Tcl_FmodCmd(clientData, interp, argc, argv)
    ClientData  clientData;
    Tcl_Interp *interp;
    int         argc;
    char      **argv;
{
    double dbVal, dbDivisor, dbResult;

    if (argc != 3) {
        Tcl_AppendResult (interp, tclXWrongArgs, argv [0], " expr divisor",
                          (char *) NULL);
        return TCL_ERROR;
    }

    if (Tcl_ExprDouble (interp, argv [1], &dbVal) != TCL_OK)
        return TCL_ERROR;

    if (Tcl_ExprDouble (interp, argv [2], &dbDivisor) != TCL_OK)
        return TCL_ERROR;

    G_inTclFPMath = TRUE;
    dbResult = fmod (dbVal, dbDivisor);
    G_inTclFPMath = FALSE;

#ifdef TCL_IEEE_FP_MATH
    if ((dbResult != dbResult) ||
        (dbResult < -MAXDOUBLE) ||
        (dbResult >  MAXDOUBLE))
        return ReturnIEEEMathError (interp, dbResult);
#else
    if (G_gotTclFPMathErr)
        return ReturnFPMathError (interp);
#endif

    Tcl_ReturnDouble (interp, dbResult);
    return TCL_OK;
}

/*
 *-----------------------------------------------------------------------------
 *
 * Tcl_PowCmd --
 *    Implements the TCL power (exponentiation) command:
 *        pow num1 num2
 *
 * Results:
 *      Returns TCL_OK if number is present and conversion succeeds.
 *
 *-----------------------------------------------------------------------------
 */
int
Tcl_PowCmd(clientData, interp, argc, argv)
    ClientData  clientData;
    Tcl_Interp *interp;
    int         argc;
    char      **argv;
{
    double dbVal, dbExp, dbResult;

    if (argc != 3) {
        Tcl_AppendResult (interp, tclXWrongArgs, argv [0], " expr exp",
                          (char *) NULL);
        return TCL_ERROR;
    }

    if (Tcl_ExprDouble (interp, argv [1], &dbVal) != TCL_OK)
        return TCL_ERROR;

    if (Tcl_ExprDouble (interp, argv [2], &dbExp) != TCL_OK)
        return TCL_ERROR;

    G_inTclFPMath = TRUE;
    dbResult = pow (dbVal,dbExp);
    G_inTclFPMath = FALSE;

#ifdef TCL_IEEE_FP_MATH
    if ((dbResult != dbResult) ||
        (dbResult < -MAXDOUBLE) ||
        (dbResult >  MAXDOUBLE))
        return ReturnIEEEMathError (interp, dbResult);
#else
    if (G_gotTclFPMathErr)
        return ReturnFPMathError (interp);
#endif

    Tcl_ReturnDouble (interp, dbResult);
    return TCL_OK;
}
