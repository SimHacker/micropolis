/* initialize.cpp
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

/** @file initialize.cpp */

////////////////////////////////////////////////////////////////////////


#include "stdafx.h"


////////////////////////////////////////////////////////////////////////


/* comefrom: loadFile bootUp main doMessage */
void Micropolis::InitWillStuff()
{
  register short x, y;

  RandomlySeedRand();
  InitGraphMax();
  DestroyAllSprites();

  RoadEffect = MAX_ROAD_EFFECT;
  PoliceEffect = MAX_POLICESTATION_EFFECT;
  FireEffect = MAX_FIRESTATION_EFFECT;
  cityScore = 500;
  cityPop = -1;
  LastCityTime = -1;
  LastCityYear = -1;
  LastCityMonth = -1;
  LastFunds = -1;
  LastR = LastC = LastI = -999999;
  MesNum = 0;
  MessagePort = 0;
  RoadFund = 0;
  PoliceFund = 0;
  FireFund = 0;
  ValveFlag = 1;
  DisasterEvent = SC_NONE;
  TaxFlag = 0;

  for (x = 0; x < HWLDX; x++) {
    for (y = 0; y < HWLDY; y++) {
      PopDensity[x][y] = 0;
      TrfDensity[x][y] = 0;
      PollutionMem[x][y] = 0;
      LandValueMem[x][y] = 0;
      CrimeMem[x][y] = 0;
    }
  }

  for (x = 0; x < QWX; x++) {
    for (y = 0; y < QWY; y++) {
      TerrainMem[x][y] = 0;
    }
  }

  for (x = 0; x < SmX; x++) {
    for (y = 0; y < SmY; y++) {
      RateOGMem[x][y] = 0;
      FireRate[x][y] = 0;
      ComRate[x][y] = 0;
      PoliceMap[x][y] = 0;
      PoliceMapEffect[x][y] = 0;
      FireRate[x][y] = 0;
    }
  }

  DoNewGame();
  DoUpdateHeads();
}


void Micropolis::ResetMapState()
{
/*
  SimView *view;

  for (view = sim->map; view != NULL; view = view->next) {
    view->map_state = ALMAP;
  }
*/
}


void Micropolis::ResetEditorState()
{
/*
  SimView *view;

  for (view = sim->editor; view != NULL; view = view->next) {
    view->tool_state = dozeState;
    view->tool_state_save = -1;
  }
*/
}


////////////////////////////////////////////////////////////////////////
