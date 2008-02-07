/* s_disast.c
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
#include "sim.h"


/* Disasters */


short ShakeNow;
short FloodCnt;
short FloodX, FloodY;


/* comefrom: Simulate */
DoDisasters(void)
{ 
  /* Chance of disasters at lev 0 1 2 */
  static short DisChance[3] = { 10*48, 5*48, 60}; 
  register short x;

  if (FloodCnt) FloodCnt--;
  if (DisasterEvent) 
    ScenarioDisaster();

  x = GameLevel;
  if (x > 2) x = 0;

  if (NoDisasters) return;		/*post*/
  if (!Rand(DisChance[x])) {
    x = Rand(8);
    switch (x) {
    case 0:
    case 1:
      SetFire();
      break;
    case 2:
    case 3:
      MakeFlood();
      break;
    case 4:
      break;
    case 5:
      MakeTornado();
      break;
    case 6:
      MakeEarthquake();
      break;
    case 7:
    case 8:
      if (PolluteAverage > /* 80 */ 60) MakeMonster();
      break;
    }
  }
}


/* comefrom: DoDisasters */
ScenarioDisaster(void)
{
  int x, y;

  switch (DisasterEvent) {
    case 1:			/* Dullsville */
      break;
    case 2: 			/* San Francisco */
      if (DisasterWait == 1) MakeEarthquake();
      break;
    case 3:			/* Hamburg */
      DropFireBombs();
      break;
    case 4:			/* Bern */
      break;
    case 5: 			/* Tokyo */
      if (DisasterWait == 1) MakeMonster();
      break;
    case 6: 			/* Detroit */
      break;
    case 7: 			/* Boston */
      if (DisasterWait == 1) MakeMeltdown();
      break;
    case 8:			/* Rio */
      if ((DisasterWait % 24) == 0) MakeFlood();
      break;
  }
  if (DisasterWait) DisasterWait--;
  else DisasterEvent = 0;
}


/* comefrom: ScenarioDisaster */
MakeMeltdown(void)
{
  short x, y;

  for (x = 0; x < (WORLD_X - 1); x ++) {
    for (y = 0; y < (WORLD_Y - 1); y++) {
      /* TILE_IS_NUCLEAR(Map[x][y]) */
      if ((Map[x][y] & LOMASK) == NUCLEAR) {
	DoMeltdown(x, y);
	return;
      }
    }
  }
}


FireBomb()
{
  CrashX = Rand(WORLD_X - 1);
  CrashY = Rand(WORLD_Y - 1);
  MakeExplosion(CrashX, CrashY);
  ClearMes();
  SendMesAt(-30, CrashX, CrashY);
}


/* comefrom: DoDisasters ScenarioDisaster */
MakeEarthquake(void)
{
  register short x, y, z;
  short time;

  DoEarthQuake();

  SendMesAt(-23, CCx, CCy);
  time = Rand(700) + 300;
  for (z = 0; z < time; z++)  {
    x = Rand(WORLD_X - 1);
    y = Rand(WORLD_Y - 1);
    if ((x < 0) || (x > (WORLD_X - 1)) ||
	(y < 0) || (y > (WORLD_Y - 1)))
      continue;
    /* TILE_IS_VULNERABLE(Map[x][y]) */
    if (Vunerable(Map[x][y])) {
      if (z & 0x3)
	Map[x][y] = (RUBBLE + BULLBIT) + (Rand16() & 3);
      else
	Map[x][y] = (FIRE + ANIMBIT) + (Rand16() & 7);
    }
  }
}


/* comefrom: DoDisasters */
SetFire(void)
{
  register short x, y, z;

  x = Rand(WORLD_X - 1);
  y = Rand(WORLD_Y - 1);
  z = Map[x][y];
  /* TILE_IS_ARSONABLE(z) */
  if (!(z & ZONEBIT)) {
    z = z & LOMASK;
    if ((z > LHTHR) && (z < LASTZONE)) {
      Map[x][y] = FIRE + ANIMBIT + (Rand16() & 7);
      CrashX = x; CrashY = y;
      SendMesAt(-20, x, y);
    }
  }
}


/* comefrom: DoDisasters */
MakeFire(void)
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


/* comefrom: MakeEarthquake */
Vunerable(int tem)
{
  register int tem2;

  tem2 = tem & LOMASK;
  if ((tem2 < RESBASE) ||
      (tem2 > LASTZONE) ||
      (tem & ZONEBIT))
    return(FALSE);
  return(TRUE);
}


/* comefrom: DoDisasters ScenarioDisaster */
MakeFlood(void)
{
  static short Dx[4] = { 0, 1, 0,-1};
  static short Dy[4] = {-1, 0, 1, 0};
  register short xx, yy, c;
  short z, t, x, y;

  for (z = 0; z < 300; z++) {
    x = Rand(WORLD_X - 1);
    y = Rand(WORLD_Y - 1);
    c = Map[x][y] & LOMASK; /* XXX: & LOMASK */
    /* TILE_IS_RIVER_EDGE(c) */
    if ((c > 4) && (c < 21))		/* if riveredge  */
      for (t = 0; t < 4; t++) {
	xx = x + Dx[t];
	yy = y + Dy[t];
	if (TestBounds(xx, yy)) {
	  c = Map[xx][yy];
	  /* TILE_IS_FLOODABLE(c) */
	  if ((c == 0) || ((c & BULLBIT) && (c & BURNBIT))) {
	    Map[xx][yy] = FLOOD;
	    FloodCnt = 30;
	    SendMesAt(-42, xx, yy);
	    FloodX = xx; FloodY = yy;
	    return;
	  }
	}
      }
  }
}


/* comefrom: MapScan */
DoFlood(void)
{
  static short Dx[4] = { 0, 1, 0,-1};
  static short Dy[4] = {-1, 0, 1, 0};
  register short z, c, xx, yy, t;

  if (FloodCnt)
    for (z = 0; z < 4; z++) {
      if (!(Rand16() & 7)) {
	xx = SMapX + Dx[z];
	yy = SMapY + Dy[z];
	if (TestBounds(xx, yy)) {
	  c = Map[xx][yy];
	  t = c & LOMASK;
	  /* TILE_IS_FLOODABLE2(c) */
	  if ((c & BURNBIT) ||
	      (c == 0) ||
	      ((t >= WOODS5 /* XXX */) && (t < FLOOD))) {
	    if (c & ZONEBIT)
	      FireZone(xx, yy, c);
	    Map[xx][yy] = FLOOD + Rand(2);
	  }
	}
      }
    }
  else
    if (!(Rand16() & 15))
      Map[SMapX][SMapY] = 0;
}
