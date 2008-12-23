/* stubs.cpp
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

/** @file stubs.cpp */

////////////////////////////////////////////////////////////////////////


#include "stdafx.h"


////////////////////////////////////////////////////////////////////////


void Micropolis::Spend(
  int dollars)
{
  SetFunds(TotalFunds - dollars);
}


void Micropolis::SetFunds(
  int dollars)
{
  TotalFunds = dollars;
  UpdateFunds();
}


Quad Micropolis::TickCount()
{
#ifdef _WIN32
  return (::GetTickCount() * 60) / 1000;
#else
  struct timeval time;
  gettimeofday(&time, 0);
  return (Quad)((time.tv_sec / 60) + (time.tv_usec * 1000000 / 60));
#endif
}


Ptr Micropolis::NewPtr(
  int size)
{
  return (Ptr)malloc(size);
}


void Micropolis::FreePtr(
  void *data)
{
  free(data);
}


void Micropolis::DoPlayNewCity()
{
  Callback("UIPlayNewCity", "");
}


void Micropolis::DoReallyStartGame()
{
  Callback("UIReallyStartGame", "");
}


void Micropolis::DoStartLoad()
{
  Callback("UIStartLoad", "");
}


void Micropolis::DoStartScenario(
  int scenario)
{
  Callback(
    "UIStartScenario", 
    "d", 
    (int)scenario);
}


void Micropolis::DropFireBombs()
{
  Callback("UIDropFireBombs", "");
}


void Micropolis::InitGame()
{
  sim_paused = false; // Simulation is running.
  sim_paused_speed = 0;
  sim_skip = 0;
  sim_skips = 0;
  heat_steps = 0; // Disable cellular automata machine.
  setSpeed(0);
}


// Scripting language independent callback mechanism. 
// This allows Micropolis to send callback messages with 
// a variable number of typed parameters back to the 
// scripting language, while maintining independence from
// the particular scripting language (or user interface 
// runtime). 
//
// The name is the name of a message to send. 
// The params is a string that specifies the number and 
// types of the following vararg parameters. 
// There is one character in the param string per vararg
// parameter. The following parameter types are currently
// supported:
//
// i: integer
// f: float
// s: string
//
// See PythonCallbackHook defined in swig/micropolis-swig-python.i
// for an example of a callback function. 
//
void Micropolis::Callback(
  const char *name,
  const char *params,
  ...)
{
  if (callbackHook == NULL) {
    return;
  }

  va_list arglist;
  va_start(arglist, params); // beginning after last named argument: params

  (*callbackHook)(
    this,
    callbackData,
    name,
    params,
    arglist);

  va_end(arglist);
}


void Micropolis::DoEarthquake()
{
  MakeSound("city", "Explosion-Low");
  int magnitude = Rand(10) + 3;
  Callback("UIStartEarthquake", "d", magnitude);
}


void Micropolis::InvalidateEditors()
{
  Callback("UIInvalidateEditors", "");
}


void Micropolis::InvalidateMaps()
{
  Callback("UIInvalidateMaps", "");
}


void Micropolis::InitializeSound()
{
  Callback("UIInitializeSound", "");
}


void Micropolis::MakeSound(
  char *channel,
  char *sound)
{
  Callback(
    "UIMakeSound",
    "ss",
    channel,
    sound);
}


int Micropolis::getTile(
    int x,
    int y)
{
    if (!TestBounds(x, y)) {
        return DIRT;
    }

    int tile =
        Map[x][y];

    return tile;
}


void *Micropolis::getMapBuffer()
{
    return (void *)mapPtr;
}


////////////////////////////////////////////////////////////////////////
