/* 
 * tclXkeylist.c --
 *
 *  Extended Tcl keyed list commands and interfaces.
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
 * $Id: tclXkeylist.c,v 2.0 1992/10/16 04:50:53 markd Rel $
 *-----------------------------------------------------------------------------
 */

#include "tclxint.h"

/*
 * Type used to return information about a field that was found in a keyed
 * list.
 */
typedef struct fieldInfo_t {
        int    argc;
        char **argv;
        int    foundIdx;
        char  *valuePtr;
        int    valueSize;
        } fieldInfo_t;

/*
 * Prototypes of internal functions.
 */
static int
CompareKeyListField _ANSI_ARGS_((Tcl_Interp   *interp,
                                 CONST char   *fieldName,
                                 CONST char   *field,
                                 char        **valuePtr,
                                 int          *valueSizePtr));

static int
SplitAndFindField _ANSI_ARGS_((Tcl_Interp  *interp,
                               CONST char  *fieldName,
                               CONST char  *keyedList,
                               fieldInfo_t *fieldInfoPtr));


/*
 *-----------------------------------------------------------------------------
 *
 * CompareKeyListField --
 *   Compare a field name to a field (keyword/value pair) to determine if
 * the field names match.
 *
 * Parameters:
 *   o interp (I/O) - Error message will be return in result if there is an
 *     error.
 *   o fieldName (I) - Field name to compare against field.
 *   o field (I) - Field to see if its name matches.
 *   o valuePtr (O) - If the field names match, a pointer to value part is
 *     returned.
 *   o valueSizePtr (O) - If the field names match, the length of the value
 *     part is returned here.
 * Returns:
 *    TCL_OK - If the field names match.
 *    TCL_BREAK - If the fields names don't match.
 *    TCL_ERROR -  If the list has an invalid format.
 *-----------------------------------------------------------------------------
 */
static int
CompareKeyListField (interp, fieldName, field, valuePtr, valueSizePtr)
    Tcl_Interp   *interp;
    CONST char   *fieldName;
    CONST char   *field;
    char        **valuePtr;
    int          *valueSizePtr; 
{
    char *elementPtr, *nextPtr;
    int   fieldNameSize, elementSize;

    if (field [0] == '\0') {
        interp->result =
            "invalid keyed list format: list contains an empty field entry";
        return TCL_ERROR;
    }
    if (TclFindElement (interp, (char *) field, &elementPtr, &nextPtr, 
                        &elementSize, NULL) != TCL_OK)
        return TCL_ERROR;
    if (elementSize == 0) {
        interp->result =
            "invalid keyed list format: list contains an empty field name";
        return TCL_ERROR;
    }
    if (nextPtr[0] == '\0') {
        Tcl_AppendResult (interp, "invalid keyed list format or inconsistent ",
                          "field name scoping: no value associated with ",
                          "field \"", elementPtr, "\"", (char *) NULL);
        return TCL_ERROR;
    }

    fieldNameSize = strlen ((char *) fieldName);
    if (!((elementSize == fieldNameSize) && 
            STRNEQU (elementPtr, ((char *) fieldName), fieldNameSize)))
        return TCL_BREAK;   /* Names do not match */

    /*
     * Extract the value from the list.
     */
    if (TclFindElement (interp, nextPtr, &elementPtr, &nextPtr, &elementSize, 
                        NULL) != TCL_OK)
        return TCL_ERROR;
    if (nextPtr[0] != '\0') {
        Tcl_AppendResult (interp, "invalid keyed list format: ",
                          "trailing data following value in field: \"",
                          elementPtr, "\"", (char *) NULL);
        return TCL_ERROR;
    }
    *valuePtr = elementPtr;
    *valueSizePtr = elementSize;
    return TCL_OK;
}

/*
 *-----------------------------------------------------------------------------
 *
 * SplitAndFindField --
 *   Split a keyed list into an argv and locate a field (key/value pair)
 * in the list.
 *
 * Parameters:
 *   o interp (I/O) - Error message will be return in result if there is an
 *     error.
 *   o fieldName (I) - The name of the field to find.  Will validate that the
 *     name is not empty.  If the name has a sub-name (seperated by "."),
 *     search for the top level name.
 *   o fieldInfoPtr (O) - The following fields are filled in:
 *       o argc - The number of elements in the keyed list.
 *       o argv - The keyed list argv is returned here, even if the key was
 *         not found.  Client must free.  Will be NULL is an error occurs.
 *       o foundIdx - The argv index containing the list entry that matches
 *         the field name, or -1 if the key was not found.
 *       o valuePtr - Pointer to the value part of the found element. NULL
 *         in not found.
 *       o valueSize - The size of the value part.
 * Returns:
 *   Standard Tcl result.
 *-----------------------------------------------------------------------------
 */
static int
SplitAndFindField (interp, fieldName, keyedList, fieldInfoPtr)
    Tcl_Interp  *interp;
    CONST char  *fieldName;
    CONST char  *keyedList;
    fieldInfo_t *fieldInfoPtr;
{
    int  idx, result;

    if (fieldName == '\0') {
        interp->result = "null key not allowed";
        return TCL_ERROR;
    }

    fieldInfoPtr->argv = NULL;

    if (Tcl_SplitList (interp, (char *) keyedList, &fieldInfoPtr->argc,
                       &fieldInfoPtr->argv) != TCL_OK)
        goto errorExit;

    result = TCL_BREAK;
    for (idx = 0; idx < fieldInfoPtr->argc; idx++) {
        result = CompareKeyListField (interp, fieldName, 
                                      fieldInfoPtr->argv [idx],
                                      &fieldInfoPtr->valuePtr,
                                      &fieldInfoPtr->valueSize);
        if (result != TCL_BREAK)
            break;  /* Found or error, exit before idx is incremented. */
    }
    if (result == TCL_ERROR)
        goto errorExit;

    if (result == TCL_BREAK) {
        fieldInfoPtr->foundIdx = -1;  /* Not found */
        fieldInfoPtr->valuePtr = NULL;
    } else {
        fieldInfoPtr->foundIdx = idx;
    }
    return TCL_OK;

errorExit:
    if (fieldInfoPtr->argv != NULL)
        ckfree (fieldInfoPtr->argv);
    fieldInfoPtr->argv = NULL;
    return TCL_ERROR;
}

/*
 *-----------------------------------------------------------------------------
 *
 * Tcl_GetKeyedListKeys --
 *   Retrieve a list of keyes from a keyed list.  The list is walked rather
 * than converted to a argv for increased performance.
 *
 * Parameters:
 *   o interp (I/O) - Error message will be return in result if there is an
 *     error.
 *   o subFieldName (I) - If "" or NULL, then the keys are retreved for
 *     the top level of the list.  If specified, it is name of the field who's
 *     subfield keys are to be retrieve.
 *   o keyedList (I) - The list to search for the field.
 *   o keyesArgcPtr (O) - The number of keys in the keyed list is returned
 *     here.
 *   o keyesArgvPtr (O) - An argv containing the key names.  It is dynamically
 *     allocated, containing both the array and the strings. A single call
 *     to ckfree will release it.
 * Returns:
 *   TCL_OK - If the field was found.
 *   TCL_BREAK - If the field was not found.
 *   TCL_ERROR - If an error occured.
 *-----------------------------------------------------------------------------
 */
int
Tcl_GetKeyedListKeys (interp, subFieldName, keyedList, keyesArgcPtr,
                      keyesArgvPtr)
    Tcl_Interp  *interp;
    CONST char  *subFieldName;
    CONST char  *keyedList;
    int         *keyesArgcPtr;
    char      ***keyesArgvPtr;
{
    char  *scanPtr, *subFieldList;
    int    result, keyCount, totalKeySize, idx;
    char  *fieldPtr, *keyPtr, *nextByte, *dummyPtr;
    int    fieldSize,  keySize;
    char **keyArgv;

    /*
     * If the keys of a subfield are requested, the dig out that field's
     * list and then rummage through in getting the keys.
     */
    subFieldList = NULL;
    if ((subFieldName != NULL) && (subFieldName [0] != '\0')) {
        result = Tcl_GetKeyedListField (interp, subFieldName, keyedList,
                                        &subFieldList);
        if (result != TCL_OK)
            return result;
        keyedList = subFieldList;
    }

    /*
     * Walk the list count the number of field names and their length.
     */
    keyCount = 0;
    totalKeySize = 0;    
    scanPtr = (char *) keyedList;

    while (*scanPtr != '\0') {
        result = TclFindElement (interp, scanPtr, &fieldPtr, &scanPtr, 
                                 &fieldSize, NULL);
        if (result != TCL_OK)
            goto errorExit;
        result = TclFindElement (interp, fieldPtr, &keyPtr, &dummyPtr,
                                 &keySize, NULL);
        if (result != TCL_OK)
            goto errorExit;

        keyCount++;
        totalKeySize += keySize + 1;
    }

    /*
     * Allocate a structure to hold both the argv and strings.
     */
    keyArgv = (char **) ckalloc (((keyCount + 1) * sizeof (char *)) +
                                 totalKeySize);
    keyArgv [keyCount] = NULL;
    nextByte = ((char *) keyArgv) + ((keyCount + 1) * sizeof (char *));

    /*
     * Walk the list once more, copying in the strings and building up the
     * argv.
     */
    scanPtr = (char *) keyedList;
    idx = 0;

    while (*scanPtr != '\0') {
        TclFindElement (interp, scanPtr, &fieldPtr, &scanPtr, &fieldSize,
                        NULL);
        TclFindElement (interp, fieldPtr, &keyPtr, &dummyPtr, &keySize, NULL);
        keyArgv [idx++] = nextByte;
        strncpy (nextByte, keyPtr, keySize);
        nextByte [keySize] = '\0';
        nextByte += keySize + 1; 
    }
    *keyesArgcPtr = keyCount;
    *keyesArgvPtr = keyArgv;
    
    if (subFieldList != NULL)
        ckfree (subFieldList);
    return TCL_OK;

  errorExit:
    if (subFieldList != NULL)
        ckfree (subFieldList);
    return TCL_ERROR;
}

/*
 *-----------------------------------------------------------------------------
 *
 * Tcl_GetKeyedListField --
 *   Retrieve a field value from a keyed list.  The list is walked rather than
 * converted to a argv for increased performance.  This if the name contains
 * sub-fields, this function recursive.
 *
 * Parameters:
 *   o interp (I/O) - Error message will be return in result if there is an
 *     error.
 *   o fieldName (I) - The name of the field to extract.  Will recusively
 *     process sub-field names seperated by `.'.
 *   o keyedList (I) - The list to search for the field.
 *   o fieldValuePtr (O) - If the field is found, a pointer to a dynamicly
 *     allocated string containing the value is returned here.  If NULL is
 *     specified, then only the presence of the field is validated, the
 *     value is not returned.
 * Returns:
 *   TCL_OK - If the field was found.
 *   TCL_BREAK - If the field was not found.
 *   TCL_ERROR - If an error occured.
 *-----------------------------------------------------------------------------
 */
int
Tcl_GetKeyedListField (interp, fieldName, keyedList, fieldValuePtr)
    Tcl_Interp  *interp;
    CONST char  *fieldName;
    CONST char  *keyedList;
    char       **fieldValuePtr;
{
    char *nameSeparPtr, *scanPtr, *valuePtr;
    int   valueSize, result;

    if (fieldName == '\0') {
        interp->result = "null key not allowed";
        return TCL_ERROR;
    }

    /*
     * Check for sub-names, temporarly delimit the top name with a '\0'.
     */
    nameSeparPtr = strchr ((char *) fieldName, '.');
    if (nameSeparPtr != NULL)
        *nameSeparPtr = '\0';

    /*
     * Walk the list looking for a field name that matches.
     */
    scanPtr = (char *) keyedList;
    result = TCL_BREAK;   /* Assume not found */

    while (*scanPtr != '\0') {
        char *fieldPtr;
        int   fieldSize;
        char  saveChar;

        result = TclFindElement (interp, scanPtr, &fieldPtr, &scanPtr, 
                                 &fieldSize, NULL);
        if (result != TCL_OK)
            break;

        saveChar = fieldPtr [fieldSize];
        fieldPtr [fieldSize] = '\0';

        result = CompareKeyListField (interp, (char *) fieldName, fieldPtr,
                                      &valuePtr, &valueSize);
        fieldPtr [fieldSize] = saveChar;
        if (result != TCL_BREAK)
            break;  /* Found or an error */
    }

    if (result != TCL_OK)
        goto exitPoint;   /* Not found or an error */

    /*
     * If a subfield is requested, recurse to get the value otherwise allocate
     * a buffer to hold the value.
     */
    if (nameSeparPtr != NULL) {
        char  saveChar;

        saveChar = valuePtr [valueSize];
        valuePtr [valueSize] = '\0';
        result = Tcl_GetKeyedListField (interp, nameSeparPtr+1, valuePtr, 
                                        fieldValuePtr);
        valuePtr [valueSize] = saveChar;
    } else {
        if (fieldValuePtr != NULL) {
            char *fieldValue;

            fieldValue = ckalloc (valueSize + 1);
            strncpy (fieldValue, valuePtr, valueSize);
            fieldValue [valueSize] = '\0';
            *fieldValuePtr = fieldValue;
        }
    }
exitPoint:
    if (nameSeparPtr != NULL)
         *nameSeparPtr = '.';
    return result;
}

/*
 *-----------------------------------------------------------------------------
 *
 * Tcl_SetKeyedListField --
 *   Set a field value in keyed list.
 *
 * Parameters:
 *   o interp (I/O) - Error message will be return in result if there is an
 *     error.
 *   o fieldName (I) - The name of the field to extract.  Will recusively
 *     process sub-field names seperated by `.'.
 *   o fieldValue (I) - The value to set for the field.
 *   o keyedList (I) - The keyed list to set a field value in, may be an
 *     NULL or an empty list to create a new keyed list.
 * Returns:
 *   A pointer to a dynamically allocated string, or NULL if an error
 *   occured.
 *-----------------------------------------------------------------------------
 */
char *
Tcl_SetKeyedListField (interp, fieldName, fieldValue, keyedList)
    Tcl_Interp  *interp;
    CONST char  *fieldName;
    CONST char  *fieldValue;
    CONST char  *keyedList;
{
    char        *nameSeparPtr;
    char        *newField = NULL, *newList;
    fieldInfo_t  fieldInfo;
    char        *elemArgv [2];

    if (keyedList == NULL)
        keyedList = "";

    /*
     * Check for sub-names, temporarly delimit the top name with a '\0'.
     */
    nameSeparPtr = strchr ((char *) fieldName, '.');
    if (nameSeparPtr != NULL)
        *nameSeparPtr = '\0';

    if (SplitAndFindField (interp, fieldName, keyedList, &fieldInfo) != TCL_OK)
        goto errorExit;

    /*
     * Either recursively retrieve build the field value or just use the
     * supplied value.
     */
    elemArgv [0] = (char *) fieldName;
    if (nameSeparPtr != NULL) {
        char saveChar;

        if (fieldInfo.valuePtr != NULL) {
            saveChar = fieldInfo.valuePtr [fieldInfo.valueSize];
            fieldInfo.valuePtr [fieldInfo.valueSize] = '\0';
        }
        elemArgv [1] = Tcl_SetKeyedListField (interp, nameSeparPtr+1,
                                              fieldValue, fieldInfo.valuePtr);

        if (fieldInfo.valuePtr != NULL)
            fieldInfo.valuePtr [fieldInfo.valueSize] = saveChar;
        if (elemArgv [1] == NULL)
            goto errorExit;
        newField = Tcl_Merge (2, elemArgv);
        ckfree (elemArgv [1]);
    } else {
        elemArgv [1] = (char *) fieldValue;
        newField = Tcl_Merge (2, elemArgv);
    }

    /*
     * If the field does not current exist in the keyed list, append it,
     * otherwise replace it.
     */
    if (fieldInfo.foundIdx == -1) {
        fieldInfo.foundIdx = fieldInfo.argc;
        fieldInfo.argc++;
    }

    fieldInfo.argv [fieldInfo.foundIdx] = newField;
    newList = Tcl_Merge (fieldInfo.argc, fieldInfo.argv);

    if (nameSeparPtr != NULL)
         *nameSeparPtr = '.';
    ckfree ((char *) newField);
    ckfree ((char *) fieldInfo.argv);
    return newList;

errorExit:
    if (nameSeparPtr != NULL)
         *nameSeparPtr = '.';
    if (newField != NULL)
        ckfree ((char *) newField);
    if (fieldInfo.argv != NULL)
        ckfree ((char *) fieldInfo.argv);
    return NULL;
}

/*
 *-----------------------------------------------------------------------------
 *
 * Tcl_DeleteKeyedListField --
 *   Delete a field value in keyed list.
 *
 * Parameters:
 *   o interp (I/O) - Error message will be return in result if there is an
 *     error.
 *   o fieldName (I) - The name of the field to extract.  Will recusively
 *     process sub-field names seperated by `.'.
 *   o fieldValue (I) - The value to set for the field.
 *   o keyedList (I) - The keyed list to delete the field from.
 * Returns:
 *   A pointer to a dynamically allocated string containing the new list, or
 *   NULL if an error occured.
 *-----------------------------------------------------------------------------
 */
char *
Tcl_DeleteKeyedListField (interp, fieldName, keyedList)
    Tcl_Interp  *interp;
    CONST char  *fieldName;
    CONST char  *keyedList;
{
    char        *nameSeparPtr;
    char        *newList;
    int          idx;
    fieldInfo_t  fieldInfo;
    char        *elemArgv [2];
    char        *newElement;
    /*
     * Check for sub-names, temporarly delimit the top name with a '\0'.
     */
    nameSeparPtr = strchr ((char *) fieldName, '.');
    if (nameSeparPtr != NULL)
        *nameSeparPtr = '\0';

    if (SplitAndFindField (interp, fieldName, keyedList, &fieldInfo) != TCL_OK)
        goto errorExit;

    if (fieldInfo.foundIdx == -1) {
        Tcl_AppendResult (interp, "field name not found: \"",  fieldName,
                          "\"", (char *) NULL);
        goto errorExit;
    }

    /*
     * If sub-field, recurse down to find the field to delete. If empty field
     * returned or no sub-field, delete the found entry by moving everything
     * up in the argv.
     */
    elemArgv [0] = (char *) fieldName;
    if (nameSeparPtr != NULL) {
        char saveChar;

        if (fieldInfo.valuePtr != NULL) {
            saveChar = fieldInfo.valuePtr [fieldInfo.valueSize];
            fieldInfo.valuePtr [fieldInfo.valueSize] = '\0';
        }
        elemArgv [1] = Tcl_DeleteKeyedListField (interp, nameSeparPtr+1, 
                                                 fieldInfo.valuePtr);
        if (fieldInfo.valuePtr != NULL)
            fieldInfo.valuePtr [fieldInfo.valueSize] = saveChar;
        if (elemArgv [1] == NULL)
            goto errorExit;
        if (elemArgv [1][0] == '\0')
            newElement = NULL;
        else
            newElement = Tcl_Merge (2, elemArgv);
        ckfree (elemArgv [1]);
    } else
        newElement = NULL;

    if (newElement == NULL) {
        for (idx = fieldInfo.foundIdx; idx < fieldInfo.argc; idx++)
             fieldInfo.argv [idx] = fieldInfo.argv [idx + 1];
        fieldInfo.argc--;
    } else
        fieldInfo.argv [fieldInfo.foundIdx] = newElement;

    newList = Tcl_Merge (fieldInfo.argc, fieldInfo.argv);

    if (nameSeparPtr != NULL)
         *nameSeparPtr = '.';
    if (newElement != NULL)
        ckfree (newElement);
    ckfree ((char *) fieldInfo.argv);
    return newList;

errorExit:
    if (nameSeparPtr != NULL)
         *nameSeparPtr = '.';
    if (fieldInfo.argv != NULL)
         ckfree ((char *) fieldInfo.argv);
    return NULL;
}

/*
 *-----------------------------------------------------------------------------
 *
 * Tcl_KeyldelCmd --
 *     Implements the TCL keyldel command:
 *         keyldel listvar key
 *
 * Results:
 *    Standard TCL results.
 *
 *----------------------------------------------------------------------------
 */
int
Tcl_KeyldelCmd (clientData, interp, argc, argv)
    ClientData  clientData;
    Tcl_Interp *interp;
    int         argc;
    char      **argv;
{
    char  *keyedList, *newList;
    int    listArgc, fieldIdx, idx;
    char **listArgv;
    char  *varPtr;

    if (argc != 3) {
        Tcl_AppendResult (interp, tclXWrongArgs, argv [0],
                          " listvar key", (char *) NULL);
        return TCL_ERROR;
    }

    keyedList = Tcl_GetVar (interp, argv[1], TCL_LEAVE_ERR_MSG);
    if (keyedList == NULL)
        return TCL_ERROR;

    newList = Tcl_DeleteKeyedListField (interp, argv [2], keyedList);
    if (newList == NULL)
        return TCL_ERROR;

    varPtr = Tcl_SetVar (interp, argv [1], newList, TCL_LEAVE_ERR_MSG);
    ckfree ((char *) newList);

    return (varPtr == NULL) ? TCL_ERROR : TCL_OK;
}

/*
 *-----------------------------------------------------------------------------
 *
 * Tcl_KeylgetCmd --
 *     Implements the TCL keylget command:
 *         keylget listvar [key] [retvar | {}]
 *
 * Results:
 *    Standard TCL results.
 *
 *-----------------------------------------------------------------------------
 */
int
Tcl_KeylgetCmd (clientData, interp, argc, argv)
    ClientData  clientData;
    Tcl_Interp *interp;
    int         argc;
    char      **argv;
{
    char   *keyedList;
    char   *fieldValue;
    char  **fieldValuePtr;
    int     result;

    if ((argc < 2) || (argc > 4)) {
        Tcl_AppendResult (interp, tclXWrongArgs, argv [0],
                          " listvar [key] [retvar | {}]", (char *) NULL);
        return TCL_ERROR;
    }
    keyedList = Tcl_GetVar (interp, argv[1], TCL_LEAVE_ERR_MSG);
    if (keyedList == NULL)
        return TCL_ERROR;

    /*
     * Handle request for list of keys, use keylkeys command.
     */
    if (argc == 2)
        return Tcl_KeylkeysCmd (clientData, interp, argc, argv);

    /*
     * Handle retrieving a value for a specified key.
     */
    if (argv [2] == '\0') {
        interp->result = "null key not allowed";
        return TCL_ERROR;
    }
    if ((argc == 4) && (argv [3][0] == '\0'))
        fieldValuePtr = NULL;
    else
        fieldValuePtr = &fieldValue;

    result = Tcl_GetKeyedListField (interp, argv [2], keyedList,
                                    fieldValuePtr);
    if (result == TCL_ERROR)
        return TCL_ERROR;

    /*
     * Handle field name not found.
     */
    if (result == TCL_BREAK) {
        if (argc == 3) {
            Tcl_AppendResult (interp, "key \"", argv [2], 
                              "\" not found in keyed list", (char *) NULL);
            return TCL_ERROR;
        } else {
            interp->result = "0";
            return TCL_OK;
        }
    }

    /*
     * Handle field name found and return in the result.
     */
    if (argc == 3) {
        Tcl_SetResult (interp, fieldValue, TCL_DYNAMIC);
        return TCL_OK;
    }

    /*
     * Handle null return variable specified and key was found.
     */
    if (argv [3][0] == '\0') {
        interp->result = "1";
        return TCL_OK;
    }

    /*
     * Handle returning the value to the variable.
     */
    if (Tcl_SetVar (interp, argv [3], fieldValue, TCL_LEAVE_ERR_MSG) == NULL)
        result = TCL_ERROR;
    else
        result = TCL_OK;
    ckfree (fieldValue);
    interp->result = "1";
    return result;
}

/*
 *-----------------------------------------------------------------------------
 *
 * Tcl_KeylkeysCmd --
 *     Implements the TCL keylkeys command:
 *         keylkeys listvar [key]
 *
 * Results:
 *    Standard TCL results.
 *
 *-----------------------------------------------------------------------------
 */
int
Tcl_KeylkeysCmd (clientData, interp, argc, argv)
    ClientData  clientData;
    Tcl_Interp *interp;
    int         argc;
    char      **argv;
{
    char   *keyedList, **keyesArgv;
    int    result, keyesArgc;

    if ((argc < 2) || (argc > 3)) {
        Tcl_AppendResult (interp, tclXWrongArgs, argv [0],
                          " listvar [key]", (char *) NULL);
        return TCL_ERROR;
    }
    keyedList = Tcl_GetVar (interp, argv[1], TCL_LEAVE_ERR_MSG);
    if (keyedList == NULL)
        return TCL_ERROR;

    /*
     * If key argument is not specified, then argv [2] is NULL, meaning get
     * top level keys.
     */
    result = Tcl_GetKeyedListKeys (interp, argv [2], keyedList, &keyesArgc,
                                   &keyesArgv);
    if (result == TCL_ERROR)
        return TCL_ERROR;
    if (result  == TCL_BREAK) {
        Tcl_AppendResult (interp, "field name not found: \"",  argv [2],
                          "\"", (char *) NULL);
        return TCL_ERROR;
    }

    Tcl_SetResult (interp, Tcl_Merge (keyesArgc, keyesArgv), TCL_DYNAMIC);
    ckfree (keyesArgv);
    return TCL_OK;
}

/*
 *-----------------------------------------------------------------------------
 *
 * Tcl_KeylsetCmd --
 *     Implements the TCL keylset command:
 *         keylset listvar key value [key value...]
 *
 * Results:
 *    Standard TCL results.
 *
 *-----------------------------------------------------------------------------
 */
int
Tcl_KeylsetCmd (clientData, interp, argc, argv)
    ClientData  clientData;
    Tcl_Interp *interp;
    int         argc;
    char      **argv;
{
    char *keyedList, *newList, *prevList;
    char *varPtr;
    int   idx;

    if ((argc < 4) || ((argc % 2) != 0)) {
        Tcl_AppendResult (interp, tclXWrongArgs, argv [0],
                          " listvar key value [key value...]", (char *) NULL);
        return TCL_ERROR;
    }

    keyedList = Tcl_GetVar (interp, argv[1], 0);
    
    newList = keyedList;
    for (idx = 2; idx < argc; idx += 2) {
        prevList = newList;
        newList = Tcl_SetKeyedListField (interp, argv [idx], argv [idx + 1],
                                         prevList);
        if (prevList != keyedList)
            ckfree (prevList);
        if (newList == NULL)
           return TCL_ERROR;
    }
    varPtr = Tcl_SetVar (interp, argv [1], newList, TCL_LEAVE_ERR_MSG);
    ckfree ((char *) newList);

    return (varPtr == NULL) ? TCL_ERROR : TCL_OK;
}
