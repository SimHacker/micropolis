/* micropolisengine-swig-python.i
 *
 * Micropolis, Unix Version.  This game was released for the Unix platform
 * in or about 1990 and has been modified for inclusion in the One Laptop
 * Per Child program.  Copyright (C) 1989 - 2007 Electronic Arts Inc.  If
 * you need assistance with this program, you may contact:
 *   http://wiki.laptop.org/go/Micropolis  or email  micropolis@laptop.org.
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or (at
 * your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.  You should have received a
 * copy of the GNU General Public License along with this program.  If
 * not, see <http://www.gnu.org/licenses/>.
 * 
 *             ADDITIONAL TERMS per GNU GPL Section 7
 * 
 * No trademark or publicity rights are granted.  This license does NOT
 * give you any right, title or interest in the trademark SimCity or any
 * other Electronic Arts trademark.  You may not distribute any
 * modification of this program using the trademark SimCity or claim any
 * affliation or association with Electronic Arts Inc. or its employees.
 * 
 * Any propagation or conveyance of this program must include this
 * copyright notice and these terms.
 * 
 * If you convey this program (or any modifications of it) and assume
 * contractual liability for the program to recipients of it, you agree
 * to indemnify Electronic Arts for any liability that those contractual
 * assumptions impose on Electronic Arts.
 * 
 * You may not misrepresent the origins of this program; modified
 * versions of the program must be marked as such and not identified as
 * the original program.
 * 
 * This disclaimer supplements the one included in the General Public
 * License.  TO THE FULLEST EXTENT PERMISSIBLE UNDER APPLICABLE LAW, THIS
 * PROGRAM IS PROVIDED TO YOU "AS IS," WITH ALL FAULTS, WITHOUT WARRANTY
 * OF ANY KIND, AND YOUR USE IS AT YOUR SOLE RISK.  THE ENTIRE RISK OF
 * SATISFACTORY QUALITY AND PERFORMANCE RESIDES WITH YOU.  ELECTRONIC ARTS
 * DISCLAIMS ANY AND ALL EXPRESS, IMPLIED OR STATUTORY WARRANTIES,
 * INCLUDING IMPLIED WARRANTIES OF MERCHANTABILITY, SATISFACTORY QUALITY,
 * FITNESS FOR A PARTICULAR PURPOSE, NONINFRINGEMENT OF THIRD PARTY
 * RIGHTS, AND WARRANTIES (IF ANY) ARISING FROM A COURSE OF DEALING,
 * USAGE, OR TRADE PRACTICE.  ELECTRONIC ARTS DOES NOT WARRANT AGAINST
 * INTERFERENCE WITH YOUR ENJOYMENT OF THE PROGRAM; THAT THE PROGRAM WILL
 * MEET YOUR REQUIREMENTS; THAT OPERATION OF THE PROGRAM WILL BE
 * UNINTERRUPTED OR ERROR-FREE, OR THAT THE PROGRAM WILL BE COMPATIBLE
 * WITH THIRD PARTY SOFTWARE OR THAT ANY ERRORS IN THE PROGRAM WILL BE
 * CORRECTED.  NO ORAL OR WRITTEN ADVICE PROVIDED BY ELECTRONIC ARTS OR
 * ANY AUTHORIZED REPRESENTATIVE SHALL CREATE A WARRANTY.  SOME
 * JURISDICTIONS DO NOT ALLOW THE EXCLUSION OF OR LIMITATIONS ON IMPLIED
 * WARRANTIES OR THE LIMITATIONS ON THE APPLICABLE STATUTORY RIGHTS OF A
 * CONSUMER, SO SOME OR ALL OF THE ABOVE EXCLUSIONS AND LIMITATIONS MAY
 * NOT APPLY TO YOU.
 */


////////////////////////////////////////////////////////////////////////
// Type Maps


%include "std_string.i"


////////////////////////////////////////////////////////////////////////


CallbackFunction GetPythonCallbackHook();
void *GetPythonCallbackData(PyObject *data);


////////////////////////////////////////////////////////////////////////


%{

////////////////////////////////////////////////////////////////////////
// Headers inserted into micropolisengine_wrap.cpp, 
// from micropolisengine-swig-python.i


#include "micropolis.h"


////////////////////////////////////////////////////////////////////////


// This callback hook glues Python into Micropolis's language 
// independent callback mechanism.
//
// Arguments:
//
// micropolis: the C++ micropolis object
//
// data: a void pointer which came from micropolis->data,
// that is actually a PyObject * to a Python callback function.
//
// name: the name of the callback message.
//
// params: a string describing the types of the additional parameters. 
//
// arglist: a list of arguments (in a varargs va_list), described by params. 
//
// There is one character in the param string per vararg
// parameter. The following parameter types are currently
// supported:
//
// i: integer
// f: float
// s: string
//
void PythonCallbackHook(
  Micropolis *micropolis,
  void *data,
  const char *name,
  const char *params,
  va_list arglist)
{
  // Cast the void *data into a PyObject *callback.
  PyObject *callback =
    (PyObject *)data;
  
  // We will pass a PyObject wrapper of micropolis, 
  // a Python callback name string, and the params
  // to the Python callback function. 
  int paramCount = 
    2 + ((params == NULL) ? 0 : (int)strlen(params));

  // Put the parameters together in a tuple of 
  // the appropriate size.
  PyObject *paramTuple =
    PyTuple_New(paramCount);

#ifdef TRACE_CALLBACKS
  printf(
    "PythonCallbackHook: micropolis 0x%X data 0x%X name \"%s\" params \"%s\"\n", 
    (int)micropolis, 
    (int)data, 
    name,
    (params == NULL) ? "(null)" : params);
#endif

  // Get the SWIG PyObject *micropolisObj wrapper from
  // the userData of the micropolis. If the userData is NULL,
  // then somebody forgot to set the it, so we do nothing. 
  if (micropolis->userData == NULL) {
    // We have not been hooked up yet, so do nothing.
    return;
  }

  PyObject *micropolisObj = 
    (PyObject *)micropolis->userData;

  // Need to increment the reference count here.
  Py_INCREF(micropolisObj);

  int i = 0; // Parameter index.
  
  // Pass the SWIG micropolis wrapper as the first parameter. 
  PyTuple_SetItem(
    paramTuple,
    i,
    micropolisObj);
  i++;

  // Pass the callback name as the next parameter.
  PyTuple_SetItem(
    paramTuple,
    i,
    PyString_FromString(
      name));
  i++;

  // Now pass the params as the subsequent parameters,
  // according to their type specified in params. 
  if (params != NULL) {
    char ch;
    while ((ch = *params++) != '\0') {
      switch (ch) {

        case 'd': {
          // Pass an integer. 
          int d =
            va_arg(arglist, int);
#ifdef TRACE_CALLBACKS
          printf(
            "  int: %d\n",
            d);
#endif
          PyTuple_SetItem(
            paramTuple,
            i,
            PyInt_FromLong(
              (long)d));
          i++;
          break;
        }

        case 'F': {
          // Pass a double. 
          double d =
            va_arg(arglist, double);
#ifdef TRACE_CALLBACKS
          printf(
            "  double: %d\n",
            d);
#endif
          PyTuple_SetItem(
            paramTuple,
            i,
            PyFloat_FromDouble(
              (double)d));
          i++;
          break;
        }

        case 's': {
          // Pass a string.
          char *s =
            va_arg(arglist, char *);
#ifdef TRACE_CALLBACKS
          printf(
            "  string: %s\n",
            s);
#endif
          PyTuple_SetItem(
            paramTuple,
            i,
            PyString_FromString(
              s));
          i++;
          break;
        }

        default: {
          // Unrecognized type code. 
#ifdef TRACE_CALLBACKS
          void *v =
            va_arg(arglist, void *);
          printf(
            "  ?%c?: 0x%X\n",
            ch,
            (int)v);
#endif
          break;
        }
        
      }

    }
    
  }

  // Now call the Python callback with all the parameters.   
  PyObject *result =
    PyObject_CallObject(
      callback,
      paramTuple);

  // Clean up nicely. 
  Py_XDECREF(result);
  Py_XDECREF(paramTuple);
}


// This returns a reference to the PythonCallbackHook. 
// I wrapped it in a function that returns the object 
// (which gets wrapped by SWIG), because just exposing 
// the function itself makes a callable method, not a 
// way to get a pointer to it. 
CallbackFunction GetPythonCallbackHook()
{
  return PythonCallbackHook;
}


// This can be called from Python to "cast" any Python object 
// into a (wrapped) void pointer that you can store in a member 
// that takes such a type, like the userData or callbackData. 
// Beware that this subverts the reference counting system. 
void *GetPythonCallbackData(
  PyObject *data)
{
  return (void *)data;
}


////////////////////////////////////////////////////////////////////////


%}


////////////////////////////////////////////////////////////////////////
// Micropoplis extension methods.

%extend Micropolis {


    // None now, but here is where they go. 
    // Write a regular function that takes no "self" parameter, 
    // but then use "self" to refer to the Micropolis object. 


};


////////////////////////////////////////////////////////////////////////
