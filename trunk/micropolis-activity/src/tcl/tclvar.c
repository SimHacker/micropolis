/* 
 * tclVar.c --
 *
 *	This file contains routines that implement Tcl variables
 *	(both scalars and arrays).
 *
 *	The implementation of arrays is modelled after an initial
 *	implementation by Karl Lehenbauer, Mark Diekhans and
 *	Peter da Silva.
 *
 * Copyright 1987-1991 Regents of the University of California
 * Permission to use, copy, modify, and distribute this
 * software and its documentation for any purpose and without
 * fee is hereby granted, provided that the above copyright
 * notice appear in all copies.  The University of California
 * makes no representations about the suitability of this
 * software for any purpose.  It is provided "as is" without
 * express or implied warranty.
 */

#ifndef lint
static char rcsid[] = "$Header: /user6/ouster/tcl/RCS/tclVar.c,v 1.27 92/05/07 09:24:59 ouster Exp $ SPRITE (Berkeley)";
#endif

#include "tclint.h"

/*
 * The strings below are used to indicate what went wrong when a
 * variable access is denied.
 */

static char *noSuchVar =	"no such variable";
static char *isArray =		"variable is array";
static char *needArray =	"variable isn't array";
static char *noSuchElement =	"no such element in array";
static char *traceActive =	"trace is active on variable";

/*
 * Forward references to procedures defined later in this file:
 */

static  char *		CallTraces _ANSI_ARGS_((Interp *iPtr, Var *arrayPtr,
			    Tcl_HashEntry *hPtr, char *name1, char *name2,
			    int flags));
static void		DeleteSearches _ANSI_ARGS_((Var *arrayVarPtr));
static void		DeleteArray _ANSI_ARGS_((Interp *iPtr, char *arrayName,
			    Var *varPtr, int flags));
static Var *		NewVar _ANSI_ARGS_((int space));
static ArraySearch *	ParseSearchId _ANSI_ARGS_((Tcl_Interp *interp,
			    Var *varPtr, char *varName, char *string));
static void		VarErrMsg _ANSI_ARGS_((Tcl_Interp *interp,
			    char *name1, char *name2, char *operation,
			    char *reason));

/*
 *----------------------------------------------------------------------
 *
 * Tcl_GetVar --
 *
 *	Return the value of a Tcl variable.
 *
 * Results:
 *	The return value points to the current value of varName.  If
 *	the variable is not defined or can't be read because of a clash
 *	in array usage then a NULL pointer is returned and an error
 *	message is left in interp->result if the TCL_LEAVE_ERR_MSG
 *	flag is set.  Note:  the return value is only valid up until
 *	the next call to Tcl_SetVar or Tcl_SetVar2;  if you depend on
 *	the value lasting longer than that, then make yourself a private
 *	copy.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

char *
Tcl_GetVar(interp, varName, flags)
    Tcl_Interp *interp;		/* Command interpreter in which varName is
				 * to be looked up. */
    char *varName;		/* Name of a variable in interp. */
    int flags;			/* OR-ed combination of TCL_GLOBAL_ONLY
				 * or TCL_LEAVE_ERR_MSG bits. */
{
    register char *p;

    /*
     * If varName refers to an array (it ends with a parenthesized
     * element name), then handle it specially.
     */

    for (p = varName; *p != '\0'; p++) {
	if (*p == '(') {
	    char *result;
	    char *open = p;

	    do {
		p++;
	    } while (*p != '\0');
	    p--;
	    if (*p != ')') {
		goto scalar;
	    }
	    *open = '\0';
	    *p = '\0';
	    result = Tcl_GetVar2(interp, varName, open + 1, flags);
	    *open = '(';
	    *p = ')';
#ifdef sgi
	    strcmp("a", "b"); /* XXX SGI compiler optimizer bug */
#endif
	    return result;
	}
    }

    scalar:
    return Tcl_GetVar2(interp, varName, (char *) NULL, flags);
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_GetVar2 --
 *
 *	Return the value of a Tcl variable, given a two-part name
 *	consisting of array name and element within array.
 *
 * Results:
 *	The return value points to the current value of the variable
 *	given by name1 and name2.  If the specified variable doesn't
 *	exist, or if there is a clash in array usage, then NULL is
 *	returned and a message will be left in interp->result if the
 *	TCL_LEAVE_ERR_MSG flag is set.  Note:  the return value is
 *	only valid up until the next call to Tcl_SetVar or Tcl_SetVar2;
 *	if you depend on the value lasting longer than that, then make
 *	yourself a private copy.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

char *
Tcl_GetVar2(interp, name1, name2, flags)
    Tcl_Interp *interp;		/* Command interpreter in which variable is
				 * to be looked up. */
    char *name1;		/* Name of array (if name2 is NULL) or
				 * name of variable. */
    char *name2;		/* If non-null, gives name of element in
				 * array. */
    int flags;			/* OR-ed combination of TCL_GLOBAL_ONLY
				 * or TCL_LEAVE_ERR_MSG bits. */
{
    Tcl_HashEntry *hPtr;
    Var *varPtr;
    Interp *iPtr = (Interp *) interp;
    Var *arrayPtr = NULL;

    /*
     * Lookup the first name.
     */

    if ((flags & TCL_GLOBAL_ONLY) || (iPtr->varFramePtr == NULL)) {
	hPtr = Tcl_FindHashEntry(&iPtr->globalTable, name1);
    } else {
	hPtr = Tcl_FindHashEntry(&iPtr->varFramePtr->varTable, name1);
    }
    if (hPtr == NULL) {
	if (flags & TCL_LEAVE_ERR_MSG) {
	    VarErrMsg(interp, name1, name2, "read", noSuchVar);
	}
	return NULL;
    }
    varPtr = (Var *) Tcl_GetHashValue(hPtr);
    if (varPtr->flags & VAR_UPVAR) {
	hPtr = varPtr->value.upvarPtr;
	varPtr = (Var *) Tcl_GetHashValue(hPtr);
    }

    /*
     * If this is an array reference, then remember the traces on the array
     * and lookup the element within the array.
     */

    if (name2 != NULL) {
	if (varPtr->flags & VAR_UNDEFINED) {
	    if (flags & TCL_LEAVE_ERR_MSG) {
		VarErrMsg(interp, name1, name2, "read", noSuchVar);
	    }
	    return NULL;
	} else if (!(varPtr->flags & VAR_ARRAY)) {
	    if (flags & TCL_LEAVE_ERR_MSG) {
		VarErrMsg(interp, name1, name2, "read", needArray);
	    }
	    return NULL;
	}
	arrayPtr = varPtr;
	hPtr = Tcl_FindHashEntry(varPtr->value.tablePtr, name2);
	if (hPtr == NULL) {
	    if (flags & TCL_LEAVE_ERR_MSG) {
		VarErrMsg(interp, name1, name2, "read", noSuchElement);
	    }
	    return NULL;
	}
	varPtr = (Var *) Tcl_GetHashValue(hPtr);
    }

    /*
     * Invoke any traces that have been set for the variable.
     */

    if ((varPtr->tracePtr != NULL)
	    || ((arrayPtr != NULL) && (arrayPtr->tracePtr != NULL))) {
	char *msg;

	msg = CallTraces(iPtr, arrayPtr, hPtr, name1, name2,
		(flags & TCL_GLOBAL_ONLY) | TCL_TRACE_READS);
	if (msg != NULL) {
	    VarErrMsg(interp, name1, name2, "read", msg);
	    return NULL;
	}

	/*
	 * Watch out!  The variable could have gotten re-allocated to
	 * a larger size.  Fortunately the hash table entry will still
	 * be around.
	 */

	varPtr = (Var *) Tcl_GetHashValue(hPtr);
    }
    if (varPtr->flags & (VAR_UNDEFINED|VAR_UPVAR|VAR_ARRAY)) {
	if (flags & TCL_LEAVE_ERR_MSG) {
	    VarErrMsg(interp, name1, name2, "read", noSuchVar);
	}
	return NULL;
    }
    return varPtr->value.string;
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_SetVar --
 *
 *	Change the value of a variable.
 *
 * Results:
 *	Returns a pointer to the malloc'ed string holding the new
 *	value of the variable.  The caller should not modify this
 *	string.  If the write operation was disallowed then NULL
 *	is returned;  if the TCL_LEAVE_ERR_MSG flag is set, then
 *	an explanatory message will be left in interp->result.
 *
 * Side effects:
 *	If varName is defined as a local or global variable in interp,
 *	its value is changed to newValue.  If varName isn't currently
 *	defined, then a new global variable by that name is created.
 *
 *----------------------------------------------------------------------
 */

char *
Tcl_SetVar(interp, varName, newValue, flags)
    Tcl_Interp *interp;		/* Command interpreter in which varName is
				 * to be looked up. */
    char *varName;		/* Name of a variable in interp. */
    char *newValue;		/* New value for varName. */
    int flags;			/* Various flags that tell how to set value:
				 * any of TCL_GLOBAL_ONLY, TCL_APPEND_VALUE,
				 * TCL_LIST_ELEMENT, TCL_NO_SPACE, or
				 * TCL_LEAVE_ERR_MSG. */
{
    register char *p;

    /*
     * If varName refers to an array (it ends with a parenthesized
     * element name), then handle it specially.
     */

    for (p = varName; *p != '\0'; p++) {
	if (*p == '(') {
	    char *result;
	    char *open = p;

	    do {
		p++;
	    } while (*p != '\0');
	    p--;
	    if (*p != ')') {
		goto scalar;
	    }
	    *open = '\0';
	    *p = '\0';
	    result = Tcl_SetVar2(interp, varName, open+1, newValue, flags);
	    *open = '(';
	    *p = ')';
	    return result;
	}
    }

    scalar:
    return Tcl_SetVar2(interp, varName, (char *) NULL, newValue, flags);
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_SetVar2 --
 *
 *	Given a two-part variable name, which may refer either to a
 *	scalar variable or an element of an array, change the value
 *	of the variable.  If the named scalar or array or element
 *	doesn't exist then create one.
 *
 * Results:
 *	Returns a pointer to the malloc'ed string holding the new
 *	value of the variable.  The caller should not modify this
 *	string.  If the write operation was disallowed because an
 *	array was expected but not found (or vice versa), then NULL
 *	is returned;  if the TCL_LEAVE_ERR_MSG flag is set, then
 *	an explanatory message will be left in interp->result.
 *
 * Side effects:
 *	The value of the given variable is set.  If either the array
 *	or the entry didn't exist then a new one is created.
 *
 *----------------------------------------------------------------------
 */

char *
Tcl_SetVar2(interp, name1, name2, newValue, flags)
    Tcl_Interp *interp;		/* Command interpreter in which variable is
				 * to be looked up. */
    char *name1;		/* If name2 is NULL, this is name of scalar
				 * variable.  Otherwise it is name of array. */
    char *name2;		/* Name of an element within array, or NULL. */
    char *newValue;		/* New value for variable. */
    int flags;			/* Various flags that tell how to set value:
				 * any of TCL_GLOBAL_ONLY, TCL_APPEND_VALUE,
				 * TCL_LIST_ELEMENT, and TCL_NO_SPACE, or
				 * TCL_LEAVE_ERR_MSG . */
{
    Tcl_HashEntry *hPtr;
    register Var *varPtr = NULL;
				/* Initial value only used to stop compiler
				 * from complaining; not really needed. */
    register Interp *iPtr = (Interp *) interp;
    int length, new, listFlags;
    Var *arrayPtr = NULL;

    /*
     * Lookup the first name.
     */

    if ((flags & TCL_GLOBAL_ONLY) || (iPtr->varFramePtr == NULL)) {
	hPtr = Tcl_CreateHashEntry(&iPtr->globalTable, name1, &new);
    } else {
	hPtr = Tcl_CreateHashEntry(&iPtr->varFramePtr->varTable,
		name1, &new);
    }
    if (!new) {
	varPtr = (Var *) Tcl_GetHashValue(hPtr);
	if (varPtr->flags & VAR_UPVAR) {
	    hPtr = varPtr->value.upvarPtr;
	    varPtr = (Var *) Tcl_GetHashValue(hPtr);
	}
    }

    /*
     * If this is an array reference, then create a new array (if
     * needed), remember any traces on the array, and lookup the
     * element within the array.
     */

    if (name2 != NULL) {
	if (new) {
	    varPtr = NewVar(0);
	    Tcl_SetHashValue(hPtr, varPtr);
	    varPtr->flags = VAR_ARRAY;
	    varPtr->value.tablePtr = (Tcl_HashTable *)
		    ckalloc(sizeof(Tcl_HashTable));
	    Tcl_InitHashTable(varPtr->value.tablePtr, TCL_STRING_KEYS);
	} else {
	    if (varPtr->flags & VAR_UNDEFINED) {
		varPtr->flags = VAR_ARRAY;
		varPtr->value.tablePtr = (Tcl_HashTable *)
			ckalloc(sizeof(Tcl_HashTable));
		Tcl_InitHashTable(varPtr->value.tablePtr, TCL_STRING_KEYS);
	    } else if (!(varPtr->flags & VAR_ARRAY)) {
		if (flags & TCL_LEAVE_ERR_MSG) {
		    VarErrMsg(interp, name1, name2, "set", needArray);
		}
		return NULL;
	    }
	    arrayPtr = varPtr;
	}
	hPtr = Tcl_CreateHashEntry(varPtr->value.tablePtr, name2, &new);
    }

    /*
     * Compute how many bytes will be needed for newValue (leave space
     * for a separating space between list elements).
     */

    if (flags & TCL_LIST_ELEMENT) {
	length = Tcl_ScanElement(newValue, &listFlags) + 1;
    } else {
	length = strlen(newValue);
    }

    /*
     * If the variable doesn't exist then create a new one.  If it
     * does exist then clear its current value unless this is an
     * append operation.
     */

    if (new) {
	varPtr = NewVar(length);
	Tcl_SetHashValue(hPtr, varPtr);
	if ((arrayPtr != NULL) && (arrayPtr->searchPtr != NULL)) {
	    DeleteSearches(arrayPtr);
	}
    } else {
	varPtr = (Var *) Tcl_GetHashValue(hPtr);
	if (varPtr->flags & VAR_ARRAY) {
	    if (flags & TCL_LEAVE_ERR_MSG) {
		VarErrMsg(interp, name1, name2, "set", isArray);
	    }
	    return NULL;
	}
	if (!(flags & TCL_APPEND_VALUE) || (varPtr->flags & VAR_UNDEFINED)) {
	    varPtr->valueLength = 0;
	}
    }

    /*
     * Make sure there's enough space to hold the variable's
     * new value.  If not, enlarge the variable's space.
     */

    if ((length + varPtr->valueLength) >= varPtr->valueSpace) {
	Var *newVarPtr;
	int newSize;

	newSize = 2*varPtr->valueSpace;
	if (newSize <= (length + varPtr->valueLength)) {
	    newSize += length;
	}
	newVarPtr = NewVar(newSize);
	newVarPtr->valueLength = varPtr->valueLength;
	newVarPtr->upvarUses = varPtr->upvarUses;
	newVarPtr->tracePtr = varPtr->tracePtr;
	strcpy(newVarPtr->value.string, varPtr->value.string);
	Tcl_SetHashValue(hPtr, newVarPtr);
	ckfree((char *) varPtr);
	varPtr = newVarPtr;
    }

    /*
     * Append the new value to the variable, either as a list
     * element or as a string.
     */

    if (flags & TCL_LIST_ELEMENT) {
	if ((varPtr->valueLength > 0) && !(flags & TCL_NO_SPACE)) {
	    varPtr->value.string[varPtr->valueLength] = ' ';
	    varPtr->valueLength++;
	}
	varPtr->valueLength += Tcl_ConvertElement(newValue,
		varPtr->value.string + varPtr->valueLength, listFlags);
	varPtr->value.string[varPtr->valueLength] = 0;
    } else {
	strcpy(varPtr->value.string + varPtr->valueLength, newValue);
	varPtr->valueLength += length;
    }
    varPtr->flags &= ~VAR_UNDEFINED;

    /*
     * Invoke any write traces for the variable.
     */

    if ((varPtr->tracePtr != NULL)
	    || ((arrayPtr != NULL) && (arrayPtr->tracePtr != NULL))) {
	char *msg;

	msg = CallTraces(iPtr, arrayPtr, hPtr, name1, name2,
		(flags & TCL_GLOBAL_ONLY) | TCL_TRACE_WRITES);
	if (msg != NULL) {
	    VarErrMsg(interp, name1, name2, "set", msg);
	    return NULL;
	}

	/*
	 * Watch out!  The variable could have gotten re-allocated to
	 * a larger size.  Fortunately the hash table entry will still
	 * be around.
	 */

	varPtr = (Var *) Tcl_GetHashValue(hPtr);
    }
    return varPtr->value.string;
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_UnsetVar --
 *
 *	Delete a variable, so that it may not be accessed anymore.
 *
 * Results:
 *	Returns 0 if the variable was successfully deleted, -1
 *	if the variable can't be unset.  In the event of an error,
 *	if the TCL_LEAVE_ERR_MSG flag is set then an error message
 *	is left in interp->result.
 *
 * Side effects:
 *	If varName is defined as a local or global variable in interp,
 *	it is deleted.
 *
 *----------------------------------------------------------------------
 */

int
Tcl_UnsetVar(interp, varName, flags)
    Tcl_Interp *interp;		/* Command interpreter in which varName is
				 * to be looked up. */
    char *varName;		/* Name of a variable in interp.  May be
				 * either a scalar name or an array name
				 * or an element in an array. */
    int flags;			/* OR-ed combination of any of
				 * TCL_GLOBAL_ONLY or TCL_LEAVE_ERR_MSG. */
{
    register char *p;
    int result;

    /*
     * Figure out whether this is an array reference, then call
     * Tcl_UnsetVar2 to do all the real work.
     */

    for (p = varName; *p != '\0'; p++) {
	if (*p == '(') {
	    char *open = p;

	    do {
		p++;
	    } while (*p != '\0');
	    p--;
	    if (*p != ')') {
		goto scalar;
	    }
	    *open = '\0';
	    *p = '\0';
	    result = Tcl_UnsetVar2(interp, varName, open+1, flags);
	    *open = '(';
	    *p = ')';
	    return result;
	}
    }

    scalar:
    return Tcl_UnsetVar2(interp, varName, (char *) NULL, flags);
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_UnsetVar2 --
 *
 *	Delete a variable, given a 2-part name.
 *
 * Results:
 *	Returns 0 if the variable was successfully deleted, -1
 *	if the variable can't be unset.  In the event of an error,
 *	if the TCL_LEAVE_ERR_MSG flag is set then an error message
 *	is left in interp->result.
 *
 * Side effects:
 *	If name1 and name2 indicate a local or global variable in interp,
 *	it is deleted.  If name1 is an array name and name2 is NULL, then
 *	the whole array is deleted.
 *
 *----------------------------------------------------------------------
 */

int
Tcl_UnsetVar2(interp, name1, name2, flags)
    Tcl_Interp *interp;		/* Command interpreter in which varName is
				 * to be looked up. */
    char *name1;		/* Name of variable or array. */
    char *name2;		/* Name of element within array or NULL. */
    int flags;			/* OR-ed combination of any of
				 * TCL_GLOBAL_ONLY or TCL_LEAVE_ERR_MSG. */
{
    Tcl_HashEntry *hPtr, dummyEntry;
    Var *varPtr, dummyVar;
    Interp *iPtr = (Interp *) interp;
    Var *arrayPtr = NULL;

    if ((flags & TCL_GLOBAL_ONLY) || (iPtr->varFramePtr == NULL)) {
	hPtr = Tcl_FindHashEntry(&iPtr->globalTable, name1);
    } else {
	hPtr = Tcl_FindHashEntry(&iPtr->varFramePtr->varTable, name1);
    }
    if (hPtr == NULL) {
	if (flags & TCL_LEAVE_ERR_MSG) {
	    VarErrMsg(interp, name1, name2, "unset", noSuchVar);
	}
	return -1;
    }
    varPtr = (Var *) Tcl_GetHashValue(hPtr);

    /*
     * For global variables referenced in procedures, leave the procedure's
     * reference variable in place, but unset the global variable.  Can't
     * decrement the actual variable's use count, since we didn't delete
     * the reference variable.
     */

    if (varPtr->flags & VAR_UPVAR) {
	hPtr = varPtr->value.upvarPtr;
	varPtr = (Var *) Tcl_GetHashValue(hPtr);
    }

    /*
     * If the variable being deleted is an element of an array, then
     * remember trace procedures on the overall array and find the
     * element to delete.
     */

    if (name2 != NULL) {
	if (!(varPtr->flags & VAR_ARRAY)) {
	    if (flags & TCL_LEAVE_ERR_MSG) {
		VarErrMsg(interp, name1, name2, "unset", needArray);
	    }
	    return -1;
	}
	if (varPtr->searchPtr != NULL) {
	    DeleteSearches(varPtr);
	}
	arrayPtr = varPtr;
	hPtr = Tcl_FindHashEntry(varPtr->value.tablePtr, name2);
	if (hPtr == NULL) {
	    if (flags & TCL_LEAVE_ERR_MSG) {
		VarErrMsg(interp, name1, name2, "unset", noSuchElement);
	    }
	    return -1;
	}
	varPtr = (Var *) Tcl_GetHashValue(hPtr);
    }

    /*
     * If there is a trace active on this variable or if the variable
     * is already being deleted then don't delete the variable:  it
     * isn't safe, since there are procedures higher up on the stack
     * that will use pointers to the variable.  Also don't delete an
     * array if there are traces active on any of its elements.
     */

    if (varPtr->flags &
	    (VAR_TRACE_ACTIVE|VAR_ELEMENT_ACTIVE)) {
	if (flags & TCL_LEAVE_ERR_MSG) {
	    VarErrMsg(interp, name1, name2, "unset", traceActive);
	}
	return -1;
    }

    /*
     * The code below is tricky, because of the possibility that
     * a trace procedure might try to access a variable being
     * deleted.  To handle this situation gracefully, copy the
     * contents of the variable and its hash table entry to
     * dummy variables, then clean up the actual variable so that
     * it's been completely deleted before the traces are called.
     * Then call the traces, and finally clean up the variable's
     * storage using the dummy copies.
     */

    dummyVar = *varPtr;
    Tcl_SetHashValue(&dummyEntry, &dummyVar);
    if (varPtr->upvarUses == 0) {
	Tcl_DeleteHashEntry(hPtr);
	ckfree((char *) varPtr);
    } else {
	varPtr->flags = VAR_UNDEFINED;
	varPtr->tracePtr = NULL;
    }

    /*
     * Call trace procedures for the variable being deleted and delete
     * its traces.
     */

    if ((dummyVar.tracePtr != NULL)
	    || ((arrayPtr != NULL) && (arrayPtr->tracePtr != NULL))) {
	(void) CallTraces(iPtr, arrayPtr, &dummyEntry, name1, name2,
		(flags & TCL_GLOBAL_ONLY) | TCL_TRACE_UNSETS);
	while (dummyVar.tracePtr != NULL) {
	    VarTrace *tracePtr = dummyVar.tracePtr;
	    dummyVar.tracePtr = tracePtr->nextPtr;
	    ckfree((char *) tracePtr);
	}
    }

    /*
     * If the variable is an array, delete all of its elements.  This
     * must be done after calling the traces on the array, above (that's
     * the way traces are defined).
     */

    if (dummyVar.flags & VAR_ARRAY) {
	DeleteArray(iPtr, name1, &dummyVar,
	    (flags & TCL_GLOBAL_ONLY) | TCL_TRACE_UNSETS);
    }
    if (dummyVar.flags & VAR_UNDEFINED) {
	if (flags & TCL_LEAVE_ERR_MSG) {
	    VarErrMsg(interp, name1, name2, "unset", 
		    (name2 == NULL) ? noSuchVar : noSuchElement);
	}
	return -1;
    }
    return 0;
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_TraceVar --
 *
 *	Arrange for reads and/or writes to a variable to cause a
 *	procedure to be invoked, which can monitor the operations
 *	and/or change their actions.
 *
 * Results:
 *	A standard Tcl return value.
 *
 * Side effects:
 *	A trace is set up on the variable given by varName, such that
 *	future references to the variable will be intermediated by
 *	proc.  See the manual entry for complete details on the calling
 *	sequence for proc.
 *
 *----------------------------------------------------------------------
 */

int
Tcl_TraceVar(interp, varName, flags, proc, clientData)
    Tcl_Interp *interp;		/* Interpreter in which variable is
				 * to be traced. */
    char *varName;		/* Name of variable;  may end with "(index)"
				 * to signify an array reference. */
    int flags;			/* OR-ed collection of bits, including any
				 * of TCL_TRACE_READS, TCL_TRACE_WRITES,
				 * TCL_TRACE_UNSETS, and TCL_GLOBAL_ONLY. */
    Tcl_VarTraceProc *proc;	/* Procedure to call when specified ops are
				 * invoked upon varName. */
    ClientData clientData;	/* Arbitrary argument to pass to proc. */
{
    register char *p;

    /*
     * If varName refers to an array (it ends with a parenthesized
     * element name), then handle it specially.
     */

    for (p = varName; *p != '\0'; p++) {
	if (*p == '(') {
	    int result;
	    char *open = p;

	    do {
		p++;
	    } while (*p != '\0');
	    p--;
	    if (*p != ')') {
		goto scalar;
	    }
	    *open = '\0';
	    *p = '\0';
	    result = Tcl_TraceVar2(interp, varName, open+1, flags,
		    proc, clientData);
	    *open = '(';
	    *p = ')';
	    return result;
	}
    }

    scalar:
    return Tcl_TraceVar2(interp, varName, (char *) NULL, flags,
	    proc, clientData);
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_TraceVar2 --
 *
 *	Arrange for reads and/or writes to a variable to cause a
 *	procedure to be invoked, which can monitor the operations
 *	and/or change their actions.
 *
 * Results:
 *	A standard Tcl return value.
 *
 * Side effects:
 *	A trace is set up on the variable given by name1 and name2, such
 *	that future references to the variable will be intermediated by
 *	proc.  See the manual entry for complete details on the calling
 *	sequence for proc.
 *
 *----------------------------------------------------------------------
 */

int
Tcl_TraceVar2(interp, name1, name2, flags, proc, clientData)
    Tcl_Interp *interp;		/* Interpreter in which variable is
				 * to be traced. */
    char *name1;		/* Name of scalar variable or array. */
    char *name2;		/* Name of element within array;  NULL means
				 * trace applies to scalar variable or array
				 * as-a-whole. */
    int flags;			/* OR-ed collection of bits, including any
				 * of TCL_TRACE_READS, TCL_TRACE_WRITES,
				 * TCL_TRACE_UNSETS, and TCL_GLOBAL_ONLY. */
    Tcl_VarTraceProc *proc;	/* Procedure to call when specified ops are
				 * invoked upon varName. */
    ClientData clientData;	/* Arbitrary argument to pass to proc. */
{
    Tcl_HashEntry *hPtr;
    Var *varPtr = NULL;		/* Initial value only used to stop compiler
				 * from complaining; not really needed. */
    Interp *iPtr = (Interp *) interp;
    register VarTrace *tracePtr;
    int new;

    /*
     * Locate the variable, making a new (undefined) one if necessary.
     */

    if ((flags & TCL_GLOBAL_ONLY) || (iPtr->varFramePtr == NULL)) {
	hPtr = Tcl_CreateHashEntry(&iPtr->globalTable, name1, &new);
    } else {
	hPtr = Tcl_CreateHashEntry(&iPtr->varFramePtr->varTable, name1, &new);
    }
    if (!new) {
	varPtr = (Var *) Tcl_GetHashValue(hPtr);
	if (varPtr->flags & VAR_UPVAR) {
	    hPtr = varPtr->value.upvarPtr;
	    varPtr = (Var *) Tcl_GetHashValue(hPtr);
	}
    }

    /*
     * If the trace is to be on an array element, make sure that the
     * variable is an array variable.  If the variable doesn't exist
     * then define it as an empty array.  Then find the specific
     * array element.
     */

    if (name2 != NULL) {
	if (new) {
	    varPtr = NewVar(0);
	    Tcl_SetHashValue(hPtr, varPtr);
	    varPtr->flags = VAR_ARRAY;
	    varPtr->value.tablePtr = (Tcl_HashTable *)
		    ckalloc(sizeof(Tcl_HashTable));
	    Tcl_InitHashTable(varPtr->value.tablePtr, TCL_STRING_KEYS);
	} else {
	    if (varPtr->flags & VAR_UNDEFINED) {
		varPtr->flags = VAR_ARRAY;
		varPtr->value.tablePtr = (Tcl_HashTable *)
			ckalloc(sizeof(Tcl_HashTable));
		Tcl_InitHashTable(varPtr->value.tablePtr, TCL_STRING_KEYS);
	    } else if (!(varPtr->flags & VAR_ARRAY)) {
		iPtr->result = needArray;
		return TCL_ERROR;
	    }
	}
	hPtr = Tcl_CreateHashEntry(varPtr->value.tablePtr, name2, &new);
    }

    if (new) {
	if ((name2 != NULL) && (varPtr->searchPtr != NULL)) {
	    DeleteSearches(varPtr);
	}
	varPtr = NewVar(0);
	varPtr->flags = VAR_UNDEFINED;
	Tcl_SetHashValue(hPtr, varPtr);
    } else {
	varPtr = (Var *) Tcl_GetHashValue(hPtr);
    }

    /*
     * Set up trace information.
     */

    tracePtr = (VarTrace *) ckalloc(sizeof(VarTrace));
    tracePtr->traceProc = proc;
    tracePtr->clientData = clientData;
    tracePtr->flags = flags &
	    (TCL_TRACE_READS|TCL_TRACE_WRITES|TCL_TRACE_UNSETS);
    tracePtr->nextPtr = varPtr->tracePtr;
    varPtr->tracePtr = tracePtr;
    return TCL_OK;
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_UntraceVar --
 *
 *	Remove a previously-created trace for a variable.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	If there exists a trace for the variable given by varName
 *	with the given flags, proc, and clientData, then that trace
 *	is removed.
 *
 *----------------------------------------------------------------------
 */

void
Tcl_UntraceVar(interp, varName, flags, proc, clientData)
    Tcl_Interp *interp;		/* Interpreter containing traced variable. */
    char *varName;		/* Name of variable;  may end with "(index)"
				 * to signify an array reference. */
    int flags;			/* OR-ed collection of bits describing
				 * current trace, including any of
				 * TCL_TRACE_READS, TCL_TRACE_WRITES,
				 * TCL_TRACE_UNSETS, and TCL_GLOBAL_ONLY. */
    Tcl_VarTraceProc *proc;	/* Procedure assocated with trace. */
    ClientData clientData;	/* Arbitrary argument to pass to proc. */
{
    register char *p;

    /*
     * If varName refers to an array (it ends with a parenthesized
     * element name), then handle it specially.
     */

    for (p = varName; *p != '\0'; p++) {
	if (*p == '(') {
	    char *open = p;

	    do {
		p++;
	    } while (*p != '\0');
	    p--;
	    if (*p != ')') {
		goto scalar;
	    }
	    *open = '\0';
	    *p = '\0';
	    Tcl_UntraceVar2(interp, varName, open+1, flags, proc, clientData);
	    *open = '(';
	    *p = ')';
	    return;
	}
    }

    scalar:
    Tcl_UntraceVar2(interp, varName, (char *) NULL, flags, proc, clientData);
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_UntraceVar2 --
 *
 *	Remove a previously-created trace for a variable.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	If there exists a trace for the variable given by name1
 *	and name2 with the given flags, proc, and clientData, then
 *	that trace is removed.
 *
 *----------------------------------------------------------------------
 */

void
Tcl_UntraceVar2(interp, name1, name2, flags, proc, clientData)
    Tcl_Interp *interp;		/* Interpreter containing traced variable. */
    char *name1;		/* Name of variable or array. */
    char *name2;		/* Name of element within array;  NULL means
				 * trace applies to scalar variable or array
				 * as-a-whole. */
    int flags;			/* OR-ed collection of bits describing
				 * current trace, including any of
				 * TCL_TRACE_READS, TCL_TRACE_WRITES,
				 * TCL_TRACE_UNSETS, and TCL_GLOBAL_ONLY. */
    Tcl_VarTraceProc *proc;	/* Procedure assocated with trace. */
    ClientData clientData;	/* Arbitrary argument to pass to proc. */
{
    register VarTrace *tracePtr;
    VarTrace *prevPtr;
    Var *varPtr;
    Interp *iPtr = (Interp *) interp;
    Tcl_HashEntry *hPtr;
    ActiveVarTrace *activePtr;

    /*
     * First, lookup the variable.
     */

    if ((flags & TCL_GLOBAL_ONLY) || (iPtr->varFramePtr == NULL)) {
	hPtr = Tcl_FindHashEntry(&iPtr->globalTable, name1);
    } else {
	hPtr = Tcl_FindHashEntry(&iPtr->varFramePtr->varTable, name1);
    }
    if (hPtr == NULL) {
	return;
    }
    varPtr = (Var *) Tcl_GetHashValue(hPtr);
    if (varPtr->flags & VAR_UPVAR) {
	hPtr = varPtr->value.upvarPtr;
	varPtr = (Var *) Tcl_GetHashValue(hPtr);
    }
    if (name2 != NULL) {
	if (!(varPtr->flags & VAR_ARRAY)) {
	    return;
	}
	hPtr = Tcl_FindHashEntry(varPtr->value.tablePtr, name2);
	if (hPtr == NULL) {
	    return;
	}
	varPtr = (Var *) Tcl_GetHashValue(hPtr);
    }

    flags &= (TCL_TRACE_READS | TCL_TRACE_WRITES | TCL_TRACE_UNSETS);
    for (tracePtr = varPtr->tracePtr, prevPtr = NULL; ;
	    prevPtr = tracePtr, tracePtr = tracePtr->nextPtr) {
	if (tracePtr == NULL) {
	    return;
	}
	if ((tracePtr->traceProc == proc) && (tracePtr->flags == flags)
		&& (tracePtr->clientData == clientData)) {
	    break;
	}
    }

    /*
     * The code below makes it possible to delete traces while traces
     * are active:  it makes sure that the deleted trace won't be
     * processed by CallTraces.
     */

    for (activePtr = iPtr->activeTracePtr; activePtr != NULL;
	    activePtr = activePtr->nextPtr) {
	if (activePtr->nextTracePtr == tracePtr) {
	    activePtr->nextTracePtr = tracePtr->nextPtr;
	}
    }
    if (prevPtr == NULL) {
	varPtr->tracePtr = tracePtr->nextPtr;
    } else {
	prevPtr->nextPtr = tracePtr->nextPtr;
    }
    ckfree((char *) tracePtr);
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_VarTraceInfo --
 *
 *	Return the clientData value associated with a trace on a
 *	variable.  This procedure can also be used to step through
 *	all of the traces on a particular variable that have the
 *	same trace procedure.
 *
 * Results:
 *	The return value is the clientData value associated with
 *	a trace on the given variable.  Information will only be
 *	returned for a trace with proc as trace procedure.  If
 *	the clientData argument is NULL then the first such trace is
 *	returned;  otherwise, the next relevant one after the one
 *	given by clientData will be returned.  If the variable
 *	doesn't exist, or if there are no (more) traces for it,
 *	then NULL is returned.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

ClientData
Tcl_VarTraceInfo(interp, varName, flags, proc, prevClientData)
    Tcl_Interp *interp;		/* Interpreter containing variable. */
    char *varName;		/* Name of variable;  may end with "(index)"
				 * to signify an array reference. */
    int flags;			/* 0 or TCL_GLOBAL_ONLY. */
    Tcl_VarTraceProc *proc;	/* Procedure assocated with trace. */
    ClientData prevClientData;	/* If non-NULL, gives last value returned
				 * by this procedure, so this call will
				 * return the next trace after that one.
				 * If NULL, this call will return the
				 * first trace. */
{
    register char *p;

    /*
     * If varName refers to an array (it ends with a parenthesized
     * element name), then handle it specially.
     */

    for (p = varName; *p != '\0'; p++) {
	if (*p == '(') {
	    ClientData result;
	    char *open = p;

	    do {
		p++;
	    } while (*p != '\0');
	    p--;
	    if (*p != ')') {
		goto scalar;
	    }
	    *open = '\0';
	    *p = '\0';
	    result = Tcl_VarTraceInfo2(interp, varName, open+1, flags, proc,
		prevClientData);
	    *open = '(';
	    *p = ')';
	    return result;
	}
    }

    scalar:
    return Tcl_VarTraceInfo2(interp, varName, (char *) NULL, flags, proc,
	    prevClientData);
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_VarTraceInfo2 --
 *
 *	Same as Tcl_VarTraceInfo, except takes name in two pieces
 *	instead of one.
 *
 * Results:
 *	Same as Tcl_VarTraceInfo.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

ClientData
Tcl_VarTraceInfo2(interp, name1, name2, flags, proc, prevClientData)
    Tcl_Interp *interp;		/* Interpreter containing variable. */
    char *name1;		/* Name of variable or array. */
    char *name2;		/* Name of element within array;  NULL means
				 * trace applies to scalar variable or array
				 * as-a-whole. */
    int flags;			/* 0 or TCL_GLOBAL_ONLY. */
    Tcl_VarTraceProc *proc;	/* Procedure assocated with trace. */
    ClientData prevClientData;	/* If non-NULL, gives last value returned
				 * by this procedure, so this call will
				 * return the next trace after that one.
				 * If NULL, this call will return the
				 * first trace. */
{
    register VarTrace *tracePtr;
    Var *varPtr;
    Interp *iPtr = (Interp *) interp;
    Tcl_HashEntry *hPtr;

    /*
     * First, lookup the variable.
     */

    if ((flags & TCL_GLOBAL_ONLY) || (iPtr->varFramePtr == NULL)) {
	hPtr = Tcl_FindHashEntry(&iPtr->globalTable, name1);
    } else {
	hPtr = Tcl_FindHashEntry(&iPtr->varFramePtr->varTable, name1);
    }
    if (hPtr == NULL) {
	return NULL;
    }
    varPtr = (Var *) Tcl_GetHashValue(hPtr);
    if (varPtr->flags & VAR_UPVAR) {
	hPtr = varPtr->value.upvarPtr;
	varPtr = (Var *) Tcl_GetHashValue(hPtr);
    }
    if (name2 != NULL) {
	if (!(varPtr->flags & VAR_ARRAY)) {
	    return NULL;
	}
	hPtr = Tcl_FindHashEntry(varPtr->value.tablePtr, name2);
	if (hPtr == NULL) {
	    return NULL;
	}
	varPtr = (Var *) Tcl_GetHashValue(hPtr);
    }

    /*
     * Find the relevant trace, if any, and return its clientData.
     */

    tracePtr = varPtr->tracePtr;
    if (prevClientData != NULL) {
	for ( ; tracePtr != NULL; tracePtr = tracePtr->nextPtr) {
	    if ((tracePtr->clientData == prevClientData)
		    && (tracePtr->traceProc == proc)) {
		tracePtr = tracePtr->nextPtr;
		break;
	    }
	}
    }
    for ( ; tracePtr != NULL; tracePtr = tracePtr->nextPtr) {
	if (tracePtr->traceProc == proc) {
	    return tracePtr->clientData;
	}
    }
    return NULL;
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_SetCmd --
 *
 *	This procedure is invoked to process the "set" Tcl command.
 *	See the user documentation for details on what it does.
 *
 * Results:
 *	A standard Tcl result value.
 *
 * Side effects:
 *	A variable's value may be changed.
 *
 *----------------------------------------------------------------------
 */

	/* ARGSUSED */
int
Tcl_SetCmd(dummy, interp, argc, argv)
    ClientData dummy;			/* Not used. */
    register Tcl_Interp *interp;	/* Current interpreter. */
    int argc;				/* Number of arguments. */
    char **argv;			/* Argument strings. */
{
    if (argc == 2) {
	char *value;

	value = Tcl_GetVar(interp, argv[1], TCL_LEAVE_ERR_MSG);
	if (value == NULL) {
	    return TCL_ERROR;
	}
	interp->result = value;
	return TCL_OK;
    } else if (argc == 3) {
	char *result;

	result = Tcl_SetVar(interp, argv[1], argv[2], TCL_LEAVE_ERR_MSG);
	if (result == NULL) {
	    return TCL_ERROR;
	}
	interp->result = result;
	return TCL_OK;
    } else {
	Tcl_AppendResult(interp, "wrong # args: should be \"",
		argv[0], " varName ?newValue?\"", (char *) NULL);
	return TCL_ERROR;
    }
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_UnsetCmd --
 *
 *	This procedure is invoked to process the "unset" Tcl command.
 *	See the user documentation for details on what it does.
 *
 * Results:
 *	A standard Tcl result value.
 *
 * Side effects:
 *	See the user documentation.
 *
 *----------------------------------------------------------------------
 */

	/* ARGSUSED */
int
Tcl_UnsetCmd(dummy, interp, argc, argv)
    ClientData dummy;			/* Not used. */
    register Tcl_Interp *interp;	/* Current interpreter. */
    int argc;				/* Number of arguments. */
    char **argv;			/* Argument strings. */
{
    int i;

    if (argc < 2) {
	Tcl_AppendResult(interp, "wrong # args: should be \"",
		argv[0], " varName ?varName ...?\"", (char *) NULL);
	return TCL_ERROR;
    }
    for (i = 1; i < argc; i++) {
	if (Tcl_UnsetVar(interp, argv[i], TCL_LEAVE_ERR_MSG) != 0) {
	    return TCL_ERROR;
	}
    }
    return TCL_OK;
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_AppendCmd --
 *
 *	This procedure is invoked to process the "append" Tcl command.
 *	See the user documentation for details on what it does.
 *
 * Results:
 *	A standard Tcl result value.
 *
 * Side effects:
 *	A variable's value may be changed.
 *
 *----------------------------------------------------------------------
 */

	/* ARGSUSED */
int
Tcl_AppendCmd(dummy, interp, argc, argv)
    ClientData dummy;			/* Not used. */
    register Tcl_Interp *interp;	/* Current interpreter. */
    int argc;				/* Number of arguments. */
    char **argv;			/* Argument strings. */
{
    int i;
    char *result = NULL;		/* (Initialization only needed to keep
					 * the compiler from complaining) */

    if (argc < 3) {
	Tcl_AppendResult(interp, "wrong # args: should be \"",
		argv[0], " varName value ?value ...?\"", (char *) NULL);
	return TCL_ERROR;
    }

    for (i = 2; i < argc; i++) {
	result = Tcl_SetVar(interp, argv[1], argv[i],
		TCL_APPEND_VALUE|TCL_LEAVE_ERR_MSG);
	if (result == NULL) {
	    return TCL_ERROR;
	}
    }
    interp->result = result;
    return TCL_OK;
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_LappendCmd --
 *
 *	This procedure is invoked to process the "lappend" Tcl command.
 *	See the user documentation for details on what it does.
 *
 * Results:
 *	A standard Tcl result value.
 *
 * Side effects:
 *	A variable's value may be changed.
 *
 *----------------------------------------------------------------------
 */

	/* ARGSUSED */
int
Tcl_LappendCmd(dummy, interp, argc, argv)
    ClientData dummy;			/* Not used. */
    register Tcl_Interp *interp;	/* Current interpreter. */
    int argc;				/* Number of arguments. */
    char **argv;			/* Argument strings. */
{
    int i;
    char *result = NULL;		/* (Initialization only needed to keep
					 * the compiler from complaining) */

    if (argc < 3) {
	Tcl_AppendResult(interp, "wrong # args: should be \"",
		argv[0], " varName value ?value ...?\"", (char *) NULL);
	return TCL_ERROR;
    }

    for (i = 2; i < argc; i++) {
	result = Tcl_SetVar(interp, argv[1], argv[i],
		TCL_APPEND_VALUE|TCL_LIST_ELEMENT|TCL_LEAVE_ERR_MSG);
	if (result == NULL) {
	    return TCL_ERROR;
	}
    }
    interp->result = result;
    return TCL_OK;
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_ArrayCmd --
 *
 *	This procedure is invoked to process the "array" Tcl command.
 *	See the user documentation for details on what it does.
 *
 * Results:
 *	A standard Tcl result value.
 *
 * Side effects:
 *	See the user documentation.
 *
 *----------------------------------------------------------------------
 */

	/* ARGSUSED */
int
Tcl_ArrayCmd(dummy, interp, argc, argv)
    ClientData dummy;			/* Not used. */
    register Tcl_Interp *interp;	/* Current interpreter. */
    int argc;				/* Number of arguments. */
    char **argv;			/* Argument strings. */
{
    int length;
    char c;
    Var *varPtr;
    Tcl_HashEntry *hPtr;
    Interp *iPtr = (Interp *) interp;

    if (argc < 3) {
	Tcl_AppendResult(interp, "wrong # args: should be \"",
		argv[0], " option arrayName ?arg ...?\"", (char *) NULL);
	return TCL_ERROR;
    }

    /*
     * Locate the array variable (and it better be an array).
     */

    if (iPtr->varFramePtr == NULL) {
	hPtr = Tcl_FindHashEntry(&iPtr->globalTable, argv[2]);
    } else {
	hPtr = Tcl_FindHashEntry(&iPtr->varFramePtr->varTable, argv[2]);
    }
    if (hPtr == NULL) {
	notArray:
	Tcl_AppendResult(interp, "\"", argv[2], "\" isn't an array",
		(char *) NULL);
	return TCL_ERROR;
    }
    varPtr = (Var *) Tcl_GetHashValue(hPtr);
    if (varPtr->flags & VAR_UPVAR) {
	varPtr = (Var *) Tcl_GetHashValue(varPtr->value.upvarPtr);
    }
    if (!(varPtr->flags & VAR_ARRAY)) {
	goto notArray;
    }

    /*
     * Dispatch based on the option.
     */

    c = argv[1][0];
    length = strlen(argv[1]);
    if ((c == 'a') && (strncmp(argv[1], "anymore", length) == 0)) {
	ArraySearch *searchPtr;

	if (argc != 4) {
	    Tcl_AppendResult(interp, "wrong # args: should be \"",
		    argv[0], " anymore arrayName searchId\"", (char *) NULL);
	    return TCL_ERROR;
	}
	searchPtr = ParseSearchId(interp, varPtr, argv[2], argv[3]);
	if (searchPtr == NULL) {
	    return TCL_ERROR;
	}
	while (1) {
	    Var *varPtr2;

	    if (searchPtr->nextEntry != NULL) {
		varPtr2 = (Var *) Tcl_GetHashValue(searchPtr->nextEntry);
		if (!(varPtr2->flags & VAR_UNDEFINED)) {
		    break;
		}
	    }
	    searchPtr->nextEntry = Tcl_NextHashEntry(&searchPtr->search);
	    if (searchPtr->nextEntry == NULL) {
		interp->result = "0";
		return TCL_OK;
	    }
	}
	interp->result = "1";
	return TCL_OK;
    } else if ((c == 'd') && (strncmp(argv[1], "donesearch", length) == 0)) {
	ArraySearch *searchPtr, *prevPtr;

	if (argc != 4) {
	    Tcl_AppendResult(interp, "wrong # args: should be \"",
		    argv[0], " donesearch arrayName searchId\"", (char *) NULL);
	    return TCL_ERROR;
	}
	searchPtr = ParseSearchId(interp, varPtr, argv[2], argv[3]);
	if (searchPtr == NULL) {
	    return TCL_ERROR;
	}
	if (varPtr->searchPtr == searchPtr) {
	    varPtr->searchPtr = searchPtr->nextPtr;
	} else {
	    for (prevPtr = varPtr->searchPtr; ; prevPtr = prevPtr->nextPtr) {
		if (prevPtr->nextPtr == searchPtr) {
		    prevPtr->nextPtr = searchPtr->nextPtr;
		    break;
		}
	    }
	}
	ckfree((char *) searchPtr);
    } else if ((c == 'n') && (strncmp(argv[1], "names", length) == 0)
	    && (length >= 2)) {
	Tcl_HashSearch search;
	Var *varPtr2;

	if (argc != 3) {
	    Tcl_AppendResult(interp, "wrong # args: should be \"",
		    argv[0], " names arrayName\"", (char *) NULL);
	    return TCL_ERROR;
	}
	for (hPtr = Tcl_FirstHashEntry(varPtr->value.tablePtr, &search);
		hPtr != NULL; hPtr = Tcl_NextHashEntry(&search)) {
	    varPtr2 = (Var *) Tcl_GetHashValue(hPtr);
	    if (varPtr2->flags & VAR_UNDEFINED) {
		continue;
	    }
	    Tcl_AppendElement(interp,
		    Tcl_GetHashKey(varPtr->value.tablePtr, hPtr), 0);
	}
    } else if ((c == 'n') && (strncmp(argv[1], "nextelement", length) == 0)
	    && (length >= 2)) {
	ArraySearch *searchPtr;
	Tcl_HashEntry *hPtr;

	if (argc != 4) {
	    Tcl_AppendResult(interp, "wrong # args: should be \"",
		    argv[0], " nextelement arrayName searchId\"",
		    (char *) NULL);
	    return TCL_ERROR;
	}
	searchPtr = ParseSearchId(interp, varPtr, argv[2], argv[3]);
	if (searchPtr == NULL) {
	    return TCL_ERROR;
	}
	while (1) {
	    Var *varPtr2;

	    hPtr = searchPtr->nextEntry;
	    if (hPtr == NULL) {
		hPtr = Tcl_NextHashEntry(&searchPtr->search);
		if (hPtr == NULL) {
		    return TCL_OK;
		}
	    } else {
		searchPtr->nextEntry = NULL;
	    }
	    varPtr2 = (Var *) Tcl_GetHashValue(hPtr);
	    if (!(varPtr2->flags & VAR_UNDEFINED)) {
		break;
	    }
	}
	interp->result = Tcl_GetHashKey(varPtr->value.tablePtr, hPtr);
    } else if ((c == 's') && (strncmp(argv[1], "size", length) == 0)
	    && (length >= 2)) {
	Tcl_HashSearch search;
	Var *varPtr2;
	int size;

	if (argc != 3) {
	    Tcl_AppendResult(interp, "wrong # args: should be \"",
		    argv[0], " size arrayName\"", (char *) NULL);
	    return TCL_ERROR;
	}
	size = 0;
	for (hPtr = Tcl_FirstHashEntry(varPtr->value.tablePtr, &search);
		hPtr != NULL; hPtr = Tcl_NextHashEntry(&search)) {
	    varPtr2 = (Var *) Tcl_GetHashValue(hPtr);
	    if (varPtr2->flags & VAR_UNDEFINED) {
		continue;
	    }
	    size++;
	}
	sprintf(interp->result, "%d", size);
    } else if ((c == 's') && (strncmp(argv[1], "startsearch", length) == 0)
	    && (length >= 2)) {
	ArraySearch *searchPtr;

	if (argc != 3) {
	    Tcl_AppendResult(interp, "wrong # args: should be \"",
		    argv[0], " startsearch arrayName\"", (char *) NULL);
	    return TCL_ERROR;
	}
	searchPtr = (ArraySearch *) ckalloc(sizeof(ArraySearch));
	if (varPtr->searchPtr == NULL) {
	    searchPtr->id = 1;
	    Tcl_AppendResult(interp, "s-1-", argv[2], (char *) NULL);
	} else {
	    char string[20];

	    searchPtr->id = varPtr->searchPtr->id + 1;
	    sprintf(string, "%d", searchPtr->id);
	    Tcl_AppendResult(interp, "s-", string, "-", argv[2],
		    (char *) NULL);
	}
	searchPtr->varPtr = varPtr;
	searchPtr->nextEntry = Tcl_FirstHashEntry(varPtr->value.tablePtr,
		&searchPtr->search);
	searchPtr->nextPtr = varPtr->searchPtr;
	varPtr->searchPtr = searchPtr;
    } else {
	Tcl_AppendResult(interp, "bad option \"", argv[1],
		"\": should be anymore, donesearch, names, nextelement, ",
		"size, or startsearch", (char *) NULL);
	return TCL_ERROR;
    }
    return TCL_OK;
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_GlobalCmd --
 *
 *	This procedure is invoked to process the "global" Tcl command.
 *	See the user documentation for details on what it does.
 *
 * Results:
 *	A standard Tcl result value.
 *
 * Side effects:
 *	See the user documentation.
 *
 *----------------------------------------------------------------------
 */

	/* ARGSUSED */
int
Tcl_GlobalCmd(dummy, interp, argc, argv)
    ClientData dummy;			/* Not used. */
    Tcl_Interp *interp;			/* Current interpreter. */
    int argc;				/* Number of arguments. */
    char **argv;			/* Argument strings. */
{
    Var *varPtr, *gVarPtr;
    register Interp *iPtr = (Interp *) interp;
    Tcl_HashEntry *hPtr, *hPtr2;
    int new;

    if (argc < 2) {
	Tcl_AppendResult((Tcl_Interp *) iPtr, "wrong # args: should be \"",
		argv[0], " varName ?varName ...?\"", (char *) NULL);
	return TCL_ERROR;
    }
    if (iPtr->varFramePtr == NULL) {
	return TCL_OK;
    }

    for (argc--, argv++; argc > 0; argc--, argv++) {
	hPtr = Tcl_CreateHashEntry(&iPtr->globalTable, *argv, &new);
	if (new) {
	    gVarPtr = NewVar(0);
	    gVarPtr->flags |= VAR_UNDEFINED;
	    Tcl_SetHashValue(hPtr, gVarPtr);
	} else {
	    gVarPtr = (Var *) Tcl_GetHashValue(hPtr);
	}
	hPtr2 = Tcl_CreateHashEntry(&iPtr->varFramePtr->varTable, *argv, &new);
	if (!new) {
	    Var *varPtr;
	    varPtr = (Var *) Tcl_GetHashValue(hPtr2);
	    if (varPtr->flags & VAR_UPVAR) {
		continue;
	    } else {
		Tcl_AppendResult((Tcl_Interp *) iPtr, "variable \"", *argv,
		    "\" already exists", (char *) NULL);
		return TCL_ERROR;
	    }
	}
	varPtr = NewVar(0);
	varPtr->flags |= VAR_UPVAR;
	varPtr->value.upvarPtr = hPtr;
	gVarPtr->upvarUses++;
	Tcl_SetHashValue(hPtr2, varPtr);
    }
    return TCL_OK;
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_UpvarCmd --
 *
 *	This procedure is invoked to process the "upvar" Tcl command.
 *	See the user documentation for details on what it does.
 *
 * Results:
 *	A standard Tcl result value.
 *
 * Side effects:
 *	See the user documentation.
 *
 *----------------------------------------------------------------------
 */

	/* ARGSUSED */
int
Tcl_UpvarCmd(dummy, interp, argc, argv)
    ClientData dummy;			/* Not used. */
    Tcl_Interp *interp;			/* Current interpreter. */
    int argc;				/* Number of arguments. */
    char **argv;			/* Argument strings. */
{
    register Interp *iPtr = (Interp *) interp;
    int result;
    CallFrame *framePtr;
    Var *varPtr = NULL;
    Tcl_HashTable *upVarTablePtr;
    Tcl_HashEntry *hPtr, *hPtr2;
    int new;
    Var *upVarPtr;

    if (argc < 3) {
	upvarSyntax:
	Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
		" ?level? otherVar localVar ?otherVar localVar ...?\"",
		(char *) NULL);
	return TCL_ERROR;
    }

    /*
     * Find the hash table containing the variable being referenced.
     */

    result = TclGetFrame(interp, argv[1], &framePtr);
    if (result == -1) {
	return TCL_ERROR;
    }
    argc -= result+1;
    argv += result+1;
    if (framePtr == NULL) {
	upVarTablePtr = &iPtr->globalTable;
    } else {
	upVarTablePtr = &framePtr->varTable;
    }

    if ((argc & 1) != 0) {
	goto upvarSyntax;
    }

    /*
     * Iterate over all the pairs of (local variable, other variable)
     * names.  For each pair, create a hash table entry in the upper
     * context (if the name wasn't there already), then associate it
     * with a new local variable.
     */

    while (argc > 0) {
        hPtr = Tcl_CreateHashEntry(upVarTablePtr, argv[0], &new);
        if (new) {
            upVarPtr = NewVar(0);
            upVarPtr->flags |= VAR_UNDEFINED;
            Tcl_SetHashValue(hPtr, upVarPtr);
        } else {
            upVarPtr = (Var *) Tcl_GetHashValue(hPtr);
	    if (upVarPtr->flags & VAR_UPVAR) {
		hPtr = upVarPtr->value.upvarPtr;
		upVarPtr = (Var *) Tcl_GetHashValue(hPtr);
	    }
        }

        hPtr2 = Tcl_CreateHashEntry(&iPtr->varFramePtr->varTable,
                    argv[1], &new);
        if (!new) {
            Tcl_AppendResult((Tcl_Interp *) iPtr, "variable \"", argv[1],
                "\" already exists", (char *) NULL);
            return TCL_ERROR;
        }
        varPtr = NewVar(0);
        varPtr->flags |= VAR_UPVAR;
        varPtr->value.upvarPtr = hPtr;
        upVarPtr->upvarUses++;
        Tcl_SetHashValue(hPtr2, varPtr);

        argc -= 2;
        argv += 2;
    }
    return TCL_OK;
}

/*
 *----------------------------------------------------------------------
 *
 * TclDeleteVars --
 *
 *	This procedure is called to recycle all the storage space
 *	associated with a table of variables.  For this procedure
 *	to work correctly, it must not be possible for any of the
 *	variable in the table to be accessed from Tcl commands
 *	(e.g. from trace procedures).
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Variables are deleted and trace procedures are invoked, if
 *	any are declared.
 *
 *----------------------------------------------------------------------
 */

void
TclDeleteVars(iPtr, tablePtr)
    Interp *iPtr;		/* Interpreter to which variables belong. */
    Tcl_HashTable *tablePtr;	/* Hash table containing variables to
				 * delete. */
{
    Tcl_HashSearch search;
    Tcl_HashEntry *hPtr;
    register Var *varPtr;
    int flags, globalFlag;

    flags = TCL_TRACE_UNSETS;
    if (tablePtr == &iPtr->globalTable) {
	flags |= TCL_INTERP_DESTROYED | TCL_GLOBAL_ONLY;
    }
    for (hPtr = Tcl_FirstHashEntry(tablePtr, &search); hPtr != NULL;
	    hPtr = Tcl_NextHashEntry(&search)) {
	varPtr = (Var *) Tcl_GetHashValue(hPtr);

	/*
	 * For global/upvar variables referenced in procedures, free up the
	 * local space and then decrement the reference count on the
	 * variable referred to.  If there are no more references to the
	 * global/upvar and it is undefined and has no traces set, then
	 * follow on and delete the referenced variable too.
	 */

	globalFlag = 0;
	if (varPtr->flags & VAR_UPVAR) {
	    hPtr = varPtr->value.upvarPtr;
	    ckfree((char *) varPtr);
	    varPtr = (Var *) Tcl_GetHashValue(hPtr);
	    varPtr->upvarUses--;
	    if ((varPtr->upvarUses != 0) || !(varPtr->flags & VAR_UNDEFINED)
		    || (varPtr->tracePtr != NULL)) {
		continue;
	    }
	    globalFlag = TCL_GLOBAL_ONLY;
	}

	/*
	 * Invoke traces on the variable that is being deleted, then
	 * free up the variable's space (no need to free the hash entry
	 * here, unless we're dealing with a global variable:  the
	 * hash entries will be deleted automatically when the whole
	 * table is deleted).
	 */

	if (varPtr->tracePtr != NULL) {
	    (void) CallTraces(iPtr, (Var *) NULL, hPtr,
		    Tcl_GetHashKey(tablePtr, hPtr), (char *) NULL,
		    flags | globalFlag);
	    while (varPtr->tracePtr != NULL) {
		VarTrace *tracePtr = varPtr->tracePtr;
		varPtr->tracePtr = tracePtr->nextPtr;
		ckfree((char *) tracePtr);
	    }
	}
	if (varPtr->flags & VAR_ARRAY) {
	    DeleteArray(iPtr, Tcl_GetHashKey(tablePtr, hPtr), varPtr,
		    flags | globalFlag);
	}
	if (globalFlag) {
	    Tcl_DeleteHashEntry(hPtr);
	}
	ckfree((char *) varPtr);
    }
    Tcl_DeleteHashTable(tablePtr);
}

/*
 *----------------------------------------------------------------------
 *
 * CallTraces --
 *
 *	This procedure is invoked to find and invoke relevant
 *	trace procedures associated with a particular operation on
 *	a variable.  This procedure invokes traces both on the
 *	variable and on its containing array (where relevant).
 *
 * Results:
 *	The return value is NULL if no trace procedures were invoked, or
 *	if all the invoked trace procedures returned successfully.
 *	The return value is non-zero if a trace procedure returned an
 *	error (in this case no more trace procedures were invoked after
 *	the error was returned).  In this case the return value is a
 *	pointer to a static string describing the error.
 *
 * Side effects:
 *	Almost anything can happen, depending on trace;  this procedure
 *	itself doesn't have any side effects.
 *
 *----------------------------------------------------------------------
 */

static char *
CallTraces(iPtr, arrayPtr, hPtr, name1, name2, flags)
    Interp *iPtr;			/* Interpreter containing variable. */
    register Var *arrayPtr;		/* Pointer to array variable that
					 * contains the variable, or NULL if
					 * the variable isn't an element of an
					 * array. */
    Tcl_HashEntry *hPtr;		/* Hash table entry corresponding to
					 * variable whose traces are to be
					 * invoked. */
    char *name1, *name2;		/* Variable's two-part name. */
    int flags;				/* Flags to pass to trace procedures:
					 * indicates what's happening to
					 * variable, plus other stuff like
					 * TCL_GLOBAL_ONLY and
					 * TCL_INTERP_DESTROYED. */
{
    Var *varPtr;
    register VarTrace *tracePtr;
    ActiveVarTrace active;
    char *result;
    int savedArrayFlags = 0;		/* (Initialization not needed except
					 * to prevent compiler warning) */

    /*
     * If there are already similar trace procedures active for the
     * variable, don't call them again.
     */

    varPtr = (Var *) Tcl_GetHashValue(hPtr);
    if (varPtr->flags & VAR_TRACE_ACTIVE) {
	return NULL;
    }
    varPtr->flags |= VAR_TRACE_ACTIVE;

    /*
     * Invoke traces on the array containing the variable, if relevant.
     */

    result = NULL;
    active.nextPtr = iPtr->activeTracePtr;
    iPtr->activeTracePtr = &active;
    if (arrayPtr != NULL) {
	savedArrayFlags = arrayPtr->flags;
	arrayPtr->flags |= VAR_ELEMENT_ACTIVE;
	for (tracePtr = arrayPtr->tracePtr;  tracePtr != NULL;
		tracePtr = active.nextTracePtr) {
	    active.nextTracePtr = tracePtr->nextPtr;
	    if (!(tracePtr->flags & flags)) {
		continue;
	    }
	    result = (*tracePtr->traceProc)(tracePtr->clientData,
		    (Tcl_Interp *) iPtr, name1, name2, flags);
	    if (result != NULL) {
		if (flags & TCL_TRACE_UNSETS) {
		    result = NULL;
		} else {
		    goto done;
		}
	    }
	}
    }

    /*
     * Invoke traces on the variable itself.
     */

    if (flags & TCL_TRACE_UNSETS) {
	flags |= TCL_TRACE_DESTROYED;
    }
    for (tracePtr = varPtr->tracePtr; tracePtr != NULL;
	    tracePtr = active.nextTracePtr) {
	active.nextTracePtr = tracePtr->nextPtr;
	if (!(tracePtr->flags & flags)) {
	    continue;
	}
	result = (*tracePtr->traceProc)(tracePtr->clientData,
		(Tcl_Interp *) iPtr, name1, name2, flags);
	if (result != NULL) {
	    if (flags & TCL_TRACE_UNSETS) {
		result = NULL;
	    } else {
		goto done;
	    }
	}
    }

    /*
     * Restore the variable's flags, remove the record of our active
     * traces, and then return.  Remember that the variable could have
     * been re-allocated during the traces, but its hash entry won't
     * change.
     */

    done:
    if (arrayPtr != NULL) {
	arrayPtr->flags = savedArrayFlags;
    }
    varPtr = (Var *) Tcl_GetHashValue(hPtr);
    varPtr->flags &= ~VAR_TRACE_ACTIVE;
    iPtr->activeTracePtr = active.nextPtr;
    return result;
}

/*
 *----------------------------------------------------------------------
 *
 * NewVar --
 *
 *	Create a new variable with a given initial value.
 *
 * Results:
 *	The return value is a pointer to the new variable structure.
 *	The variable will not be part of any hash table yet, and its
 *	upvarUses count is initialized to 0.  Its initial value will
 *	be empty, but "space" bytes will be available in the value
 *	area.
 *
 * Side effects:
 *	Storage gets allocated.
 *
 *----------------------------------------------------------------------
 */

static Var *
NewVar(space)
    int space;		/* Minimum amount of space to allocate
			 * for variable's value. */
{
    int extra;
    register Var *varPtr;

    extra = space - sizeof(varPtr->value);
    if (extra < 0) {
	extra = 0;
	space = sizeof(varPtr->value);
    }
    varPtr = (Var *) ckalloc((unsigned) (sizeof(Var) + extra));
    varPtr->valueLength = 0;
    varPtr->valueSpace = space;
    varPtr->upvarUses = 0;
    varPtr->tracePtr = NULL;
    varPtr->searchPtr = NULL;
    varPtr->flags = 0;
    varPtr->value.string[0] = 0;
    return varPtr;
}

/*
 *----------------------------------------------------------------------
 *
 * ParseSearchId --
 *
 *	This procedure translates from a string to a pointer to an
 *	active array search (if there is one that matches the string).
 *
 * Results:
 *	The return value is a pointer to the array search indicated
 *	by string, or NULL if there isn't one.  If NULL is returned,
 *	interp->result contains an error message.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

static ArraySearch *
ParseSearchId(interp, varPtr, varName, string)
    Tcl_Interp *interp;		/* Interpreter containing variable. */
    Var *varPtr;		/* Array variable search is for. */
    char *varName;		/* Name of array variable that search is
				 * supposed to be for. */
    char *string;		/* String containing id of search.  Must have
				 * form "search-num-var" where "num" is a
				 * decimal number and "var" is a variable
				 * name. */
{
    char *end;
    int id;
    ArraySearch *searchPtr;

    /*
     * Parse the id into the three parts separated by dashes.
     */

    if ((string[0] != 's') || (string[1] != '-')) {
	syntax:
	Tcl_AppendResult(interp, "illegal search identifier \"", string,
		"\"", (char *) NULL);
	return NULL;
    }
    id = strtoul(string+2, &end, 10);
    if ((end == (string+2)) || (*end != '-')) {
	goto syntax;
    }
    if (strcmp(end+1, varName) != 0) {
	Tcl_AppendResult(interp, "search identifier \"", string,
		"\" isn't for variable \"", varName, "\"", (char *) NULL);
	return NULL;
    }

    /*
     * Search through the list of active searches on the interpreter
     * to see if the desired one exists.
     */

    for (searchPtr = varPtr->searchPtr; searchPtr != NULL;
	    searchPtr = searchPtr->nextPtr) {
	if (searchPtr->id == id) {
	    return searchPtr;
	}
    }
    Tcl_AppendResult(interp, "couldn't find search \"", string, "\"",
	    (char *) NULL);
    return NULL;
}

/*
 *----------------------------------------------------------------------
 *
 * DeleteSearches --
 *
 *	This procedure is called to free up all of the searches
 *	associated with an array variable.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Memory is released to the storage allocator.
 *
 *----------------------------------------------------------------------
 */

static void
DeleteSearches(arrayVarPtr)
    register Var *arrayVarPtr;		/* Variable whose searches are
					 * to be deleted. */
{
    ArraySearch *searchPtr;

    while (arrayVarPtr->searchPtr != NULL) {
	searchPtr = arrayVarPtr->searchPtr;
	arrayVarPtr->searchPtr = searchPtr->nextPtr;
	ckfree((char *) searchPtr);
    }
}

/*
 *----------------------------------------------------------------------
 *
 * DeleteArray --
 *
 *	This procedure is called to free up everything in an array
 *	variable.  It's the caller's responsibility to make sure
 *	that the array is no longer accessible before this procedure
 *	is called.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	All storage associated with varPtr's array elements is deleted
 *	(including the hash table).  Any delete trace procedures for
 *	array elements are invoked.
 *
 *----------------------------------------------------------------------
 */

static void
DeleteArray(iPtr, arrayName, varPtr, flags)
    Interp *iPtr;			/* Interpreter containing array. */
    char *arrayName;			/* Name of array (used for trace
					 * callbacks). */
    Var *varPtr;			/* Pointer to variable structure. */
    int flags;				/* Flags to pass to CallTraces:
					 * TCL_TRACE_UNSETS and sometimes
					 * TCL_INTERP_DESTROYED and/or
					 * TCL_GLOBAL_ONLY. */
{
    Tcl_HashSearch search;
    register Tcl_HashEntry *hPtr;
    register Var *elPtr;

    DeleteSearches(varPtr);
    for (hPtr = Tcl_FirstHashEntry(varPtr->value.tablePtr, &search);
	    hPtr != NULL; hPtr = Tcl_NextHashEntry(&search)) {
	elPtr = (Var *) Tcl_GetHashValue(hPtr);
	if (elPtr->tracePtr != NULL) {
	    (void) CallTraces(iPtr, (Var *) NULL, hPtr, arrayName,
		    Tcl_GetHashKey(varPtr->value.tablePtr, hPtr), flags);
	    while (elPtr->tracePtr != NULL) {
		VarTrace *tracePtr = elPtr->tracePtr;
		elPtr->tracePtr = tracePtr->nextPtr;
		ckfree((char *) tracePtr);
	    }
	}
	if (elPtr->flags & VAR_SEARCHES_POSSIBLE) {
	    panic("DeleteArray found searches on array alement!");
	}
	ckfree((char *) elPtr);
    }
    Tcl_DeleteHashTable(varPtr->value.tablePtr);
    ckfree((char *) varPtr->value.tablePtr);
}

/*
 *----------------------------------------------------------------------
 *
 * VarErrMsg --
 *
 *	Generate a reasonable error message describing why a variable
 *	operation failed.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Interp->result is reset to hold a message identifying the
 *	variable given by name1 and name2 and describing why the
 *	variable operation failed.
 *
 *----------------------------------------------------------------------
 */

static void
VarErrMsg(interp, name1, name2, operation, reason)
    Tcl_Interp *interp;		/* Interpreter in which to record message. */
    char *name1, *name2;	/* Variable's two-part name. */
    char *operation;		/* String describing operation that failed,
				 * e.g. "read", "set", or "unset". */
    char *reason;		/* String describing why operation failed. */
{
    Tcl_ResetResult(interp);
    Tcl_AppendResult(interp, "can't ", operation, " \"", name1, (char *) NULL);
    if (name2 != NULL) {
	Tcl_AppendResult(interp, "(", name2, ")", (char *) NULL);
    }
    Tcl_AppendResult(interp, "\": ", reason, (char *) NULL);
}
