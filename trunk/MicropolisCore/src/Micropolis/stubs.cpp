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
  return ((Ptr)calloc(size, sizeof(Byte)));
}


void Micropolis::FreePtr(
  void *data)
{
  ckfree(data);
}


void Micropolis::GameStarted()
{
  InvalidateMaps();
  InvalidateEditors();

#ifndef _WIN32
  gettimeofday(&start_time, NULL);
#endif

  switch (Startup) {

  case -2: /* Load a city */
    if (LoadCity(StartupName)) {
      DoStartLoad();
      StartupName = NULL;
      break;
    }
    StartupName = NULL;
    // Fall through into...

  case -1:
    if (StartupName != NULL) {
      setCityName(StartupName);
      StartupName = NULL;
    } else {
      setCityName("NowHere");
    }
    DoPlayNewCity();
    break;

  case 0:
    DoReallyStartGame();
    break;

  default: /* scenario number */
    DoStartScenario(Startup);
    break;

  }
}


void Micropolis::DoPlayNewCity()
{
  Eval("UIPlayNewCity");
}


void Micropolis::DoReallyStartGame()
{
  Eval("UIReallyStartGame");
}


void Micropolis::DoStartLoad()
{
  Eval("UIStartLoad");
}


void Micropolis::DoStartScenario(
  int scenario)
{
  char buf[256];

  sprintf(buf, "UIStartScenario %d", scenario);
  Eval(buf);
}


void Micropolis::DropFireBombs()
{
  Eval("DropFireBombs");
}


void Micropolis::InitGame()
{
  sim_skips = sim_skip = sim_paused = sim_paused_speed = heat_steps = 0;
  setSpeed(0);
}


void Micropolis::ReallyQuit()
{
  exit(0);
}


void Micropolis::Eval(
  char *tcl)
{
  //printf("EVAL: %s\n", tcl);
}


void Micropolis::DoEarthquake()
{
  MakeSound("city", "Explosion-Low");
  Eval("UIEarthquake");
  ShakeNow++;
/*
  if (earthquake_timer_set) {
    Tk_DeleteTimerHandler(earthquake_timer_token);
  }
  Tk_CreateTimerHandler(earthquake_delay, (void (*)())StopEarthquake, (ClientData) 0);
  earthquake_timer_set = 1;
*/
}


void Micropolis::StopEarthquake()
{
}


void Micropolis::InvalidateEditors()
{
}


void Micropolis::InvalidateMaps()
{
}


void *Micropolis::ckalloc(
  int size)
{
  return malloc(size);
}


void Micropolis::ckfree(
  void *data)
{
  free(data);
}


void Micropolis::Kick()
{
  if (!UpdateDelayed) {
    UpdateDelayed = 1;
    //Tk_DoWhenIdle(DelayedUpdate, (ClientData) NULL);
  }
}


void Micropolis::ResetLastKeys()
{
}


void Micropolis::InitializeSound()
{
}


void Micropolis::MakeSound(
  char *channel,
  char *sound)
{
}


void Micropolis::MakeSoundOn(
  SimView *view,
  char *channel,
  char *sound)
{
}


void Micropolis::UpdateFlush()
{
}


void Micropolis::StartMicropolisTimer()
{
}


void Micropolis::StopMicropolisTimer()
{
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
