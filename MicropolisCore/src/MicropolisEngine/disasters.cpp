/* disasters.cpp
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

/** @file disasters.cpp */

////////////////////////////////////////////////////////////////////////


#include "stdafx.h"


////////////////////////////////////////////////////////////////////////


/** Let disasters happen */
void Micropolis::DoDisasters()
{
  /* Chance of disasters at lev 0 1 2 */
  static const short DisChance[3] = {
    10 * 48, // Game level 0
    5 * 48,  // Game level 1
    60 // Game level 2
  };
  assert(LEVEL_COUNT == LENGTH_OF(DisChance));

  if (FloodCnt) {
    FloodCnt--;
  }

  if (DisasterEvent != SC_NONE) {
    ScenarioDisaster();
  }

  if (NoDisasters) {
    return;
  }

  int x = gameLevel;
  if (x > LEVEL_LAST) {
    x = LEVEL_EASY;
  }

  if (!Rand(DisChance[x])) {
    switch (Rand(8)) {

    case 0:
    case 1:
      SetFire();  // 2/9 chance a fire breaks out
      break;

    case 2:
    case 3:
      MakeFlood(); // 2/9 chance for a flood
      break;

    case 4:
      // 1/9 chance nothing happens (was airplane crash,
      // which EA removed after 9/11, and requested it be
      // removed from this code)
      break;

    case 5:
      MakeTornado(); // 1/9 chance tornado
      break;

    case 6:
      MakeEarthquake(); // 1/9 chance earthquake
      break;

    case 7:
    case 8:
      // 2/9 chance godzilla arrives in a dirty town
      if (PolluteAverage > /* 80 */ 60) {
        MakeMonster();
      }
      break;

    }
  }
}


/** Let disasters of the scenario happen */
void Micropolis::ScenarioDisaster()
{

  switch (DisasterEvent) {

    case SC_DULLSVILLE:
      break;

    case SC_SAN_FRANCISCO:
      if (DisasterWait == 1) {
        MakeEarthquake();
      }
      break;

    case SC_HAMBURG:
      DropFireBombs();
      break;

    case SC_BERN:
      break;

    case SC_TOKYO:
      if (DisasterWait == 1) {
        MakeMonster();
      }
      break;

    case SC_DETROIT:
      break;

    case SC_BOSTON:
      if (DisasterWait == 1) {
        MakeMeltdown();
      }
      break;

    case SC_RIO:
      if ((DisasterWait % 24) == 0) {
        MakeFlood();
      }
      break;

    default:
      NOT_REACHED();
      break; // Never used

  }

  if (DisasterWait) {
    DisasterWait--;
  } else {
    DisasterEvent = SC_NONE;
  }
}


/**
 * Make a nuclear power plant melt
 * @todo Randomize which nuke plant melts down.
 */
void Micropolis::MakeMeltdown()
{
  short x, y;

  for (x = 0; x < (WORLD_X - 1); x++) {
    for (y = 0; y < (WORLD_Y - 1); y++) {
      if ((Map[x][y] & LOMASK) == NUCLEAR) {
        DoMeltdown(x, y);
        return;
      }
    }
  }
}

/** Let a fire bomb explode at a random location */
void Micropolis::FireBomb()
{
  CrashX = Rand(WORLD_X - 1);
  CrashY = Rand(WORLD_Y - 1);
  MakeExplosion(CrashX, CrashY);
  ClearMes();
  SendMesAt(-30, CrashX, CrashY);
}


/** Change random tiles to fire or dirt as result of the earthquake */
void Micropolis::MakeEarthquake()
{
  short x, y, z;

  DoEarthquake();

  SendMesAt(-23, CCx, CCy);
  short time = Rand(700) + 300; // strength/duration of the earthquake

  for (z = 0; z < time; z++)  {
    x = Rand(WORLD_X - 1);
    y = Rand(WORLD_Y - 1);


    if (Vulnerable(Map[x][y])) {

      if ((z & 0x3) != 0) { // 3 of 4 times reduce to rubble
        Map[x][y] = (RUBBLE + BULLBIT) + (Rand16() & 3);
      } else {
        // 1 of 4 times start fire
        Map[x][y] = (FIRE + ANIMBIT) + (Rand16() & 7);
      }
    }
  }
}


/** Start a fire at a random place, random disaster or scenario */
void Micropolis::SetFire()
{
  short x, y, z;

  x = Rand(WORLD_X - 1);
  y = Rand(WORLD_Y - 1);
  z = Map[x][y];

  /* TILE_IS_ARSONABLE(z) */
  if (!(z & ZONEBIT)) {
    z = z & LOMASK;
    if (z > LHTHR && z < LASTZONE) {
      Map[x][y] = FIRE + ANIMBIT + (Rand16() & 7);
      CrashX = x;
      CrashY = y;
      SendMesAt(-20, x, y);
    }
  }
}


/** Start a fire at a random place, requested by user */
void Micropolis::MakeFire()
{
  short t, x, y, z;

  for (t = 0; t < 40; t++)  {
    x = Rand(WORLD_X - 1);
    y = Rand(WORLD_Y - 1);
    z = Map[x][y];

    /* !(z & BURNBIT) && TILE_IS_ARSONABLE(z) */
    if ((!(z & ZONEBIT)) && (z & BURNBIT)) {
      z = z & LOMASK;
      if ((z > 21) && (z < LASTZONE)) {
        Map[x][y] = FIRE + ANIMBIT + (Rand16() & 7);
        SendMesAt(20, x, y);
        return;
      }
    }
  }
}


/**
 * Is tile vulnerable for an earthquake?
 * @param tem Tile data
 * @return Function returns \c true if tile is vulnerable, and \c false if not
 */
bool Micropolis::Vulnerable(int tem)
{
  int tem2 = tem & LOMASK;

  if (tem2 < RESBASE || tem2 > LASTZONE || (tem & ZONEBIT)) {
    return false;
  }
  
  return true;
}


/**
 * Flood many tiles
 * @todo Use Direction and some form of XYPosition class here
 */
void Micropolis::MakeFlood()
{
  static const short Dx[4] = {  0,  1,  0,  -1 };
  static const short Dy[4] = { -1,  0,  1,   0 };
  short xx, yy, c;
  short z, t, x, y;

  for (z = 0; z < 300; z++) {
    x = Rand(WORLD_X - 1);
    y = Rand(WORLD_Y - 1);
    c = Map[x][y] & LOMASK;

    if (c > CHANNEL && c <= WATER_HIGH) { /* if riveredge  */
      for (t = 0; t < 4; t++) {
        xx = x + Dx[t];
        yy = y + Dy[t];
        if (TestBounds(xx, yy)) {
          c = Map[xx][yy];

          /* tile is floodable */
          if (c == DIRT || ((c & (BULLBIT | BURNBIT)) == (BULLBIT | BURNBIT))) {
            Map[xx][yy] = FLOOD;
            FloodCnt = 30;
            SendMesAt(-42, xx, yy);
            return;
          }
        }
      }
    }
  }
}


/**
 * Flood around the (SMapX, SMapY) tile
 * @todo Use Direction and some form of XYPosition class here
 */
void Micropolis::DoFlood()
{
  static const short Dx[4] = {  0,  1,  0, -1 };
  static const short Dy[4] = { -1,  0,  1,  0 };
  register short z, c, xx, yy, t;

  if (FloodCnt) {
    for (z = 0; z < 4; z++) {
      if ((Rand16() & 7) == 0) { // 12.5% chance
        xx = SMapX + Dx[z];
        yy = SMapY + Dy[z];
        if (TestBounds(xx, yy)) {
          c = Map[xx][yy];
          t = c & LOMASK;

          /* TILE_IS_FLOODABLE2(c) */
          if ((c & BURNBIT) || c == DIRT || (t >= WOODS5 && t < FLOOD)) {
            if ((c & ZONEBIT) != 0) {
              FireZone(xx, yy, c);
            }
            Map[xx][yy] = FLOOD + Rand(2);
          }
        }
      }
    }
  } else {
    if ((Rand16() & 15) == 0) { // 1/16 chance
      Map[SMapX][SMapY] = DIRT;
    }
  }
}


////////////////////////////////////////////////////////////////////////
