/* s_scan.c
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


/* Map Updates */


short NewMap;
short NewMapFlags[NMAPS];
short CCx, CCy, CCx2, CCy2;
short PolMaxX, PolMaxY;
short CrimeMaxX, CrimeMaxY;
QUAD DonDither = 0;


/* comefrom: Simulate SpecialInit */
FireAnalysis(void)		/* Make firerate map from firestation map  */
{
  register x,y;

  SmoothFSMap();
  SmoothFSMap();
  SmoothFSMap();
  for (x = 0; x < SmX; x++)
    for (y = 0; y < SmY; y++)
      FireRate[x][y] = FireStMap[x][y];

  NewMapFlags[DYMAP] = NewMapFlags[FIMAP] = 1;
}


/* comefrom: Simulate SpecialInit */
PopDenScan(void)		/*  sets: PopDensity, , , ComRate  */
{
  QUAD Xtot, Ytot, Ztot;
  register short x, y, z;

  ClrTemArray();
  Xtot = 0;
  Ytot = 0;
  Ztot = 0;
  for (x = 0; x < WORLD_X; x++)
    for (y = 0; y < WORLD_Y; y++) {
      z = Map[x][y];
      if (z & ZONEBIT) {
	z = z & LOMASK;
	SMapX = x;
	SMapY = y;
	z = GetPDen(z) <<3;
	if (z > 254)
	  z = 254;
	tem[x >>1][y >>1] = z;
	Xtot += x;
	Ytot += y;
	Ztot++;
      }
    }
  DoSmooth();			/* T1 -> T2 */
  DoSmooth2();			/* T2 -> T1 */
  DoSmooth();			/* T1 -> T2 */

  for (x = 0; x < HWLDX; x++)
    for (y = 0; y < HWLDY; y++)
      PopDensity[x][y] = tem2[x][y] <<1;

  DistIntMarket();		/* set ComRate w/ (/ComMap) */

  if (Ztot) {			/* Find Center of Mass for City */
    CCx = Xtot / Ztot;
    CCy = Ytot / Ztot;
  } else {
    CCx = HWLDX;		/* if pop=0 center of Map is CC */
    CCy = HWLDY;
  }
  CCx2 = CCx >>1;
  CCy2 = CCy >>1;
  NewMapFlags[DYMAP] = NewMapFlags[PDMAP] = NewMapFlags[RGMAP] = 1;
}


/* comefrom: PopDenScan */
GetPDen(int Ch9)
{
  register int pop;

  if (Ch9 == FREEZ) {
    pop = DoFreePop(Ch9);
    return (pop);
  }
  if (Ch9 < COMBASE) {
    pop = RZPop(Ch9);
    return (pop);
  }
  if (Ch9 < INDBASE) {
    pop = (CZPop(Ch9) <<3);
    return (pop);
  }
  if (Ch9 < PORTBASE) {
    pop = (IZPop(Ch9) <<3);
    return (pop);
  }
  return (0);
}


/* comefrom: Simulate SpecialInit */
PTLScan(void)   	/* Does pollution, terrain, land value   */
{
  QUAD ptot, LVtot;
  register int x, y, z, dis;
  int Plevel, LVflag, loc, zx, zy, Mx, My, pnum, LVnum, pmax;

  for (x = 0; x < QWX; x++)
    for (y = 0; y < QWY; y++)
      Qtem[x][y] = 0;
  LVtot = 0;
  LVnum = 0;
  for (x = 0; x < HWLDX; x++)
    for (y = 0; y < HWLDY; y++) {
      Plevel = 0;
      LVflag = 0;
      zx = x <<1;
      zy = y <<1;
      for (Mx = zx; Mx <= zx + 1; Mx++)
	for (My = zy; My <= zy + 1; My++) {
	  if (loc = (Map[Mx][My] & LOMASK)) {
	    if (loc < RUBBLE) {
	      Qtem[x >>1][y >>1] += 15;	/* inc terrainMem */
	      continue;
	    }
	    Plevel += GetPValue(loc);
	    if (loc >= ROADBASE)
	      LVflag++;
	  }
	}
/* XXX ???
      if (Plevel < 0)
	Plevel = 250;
*/
      if (Plevel > 255)
	Plevel = 255;
      tem[x][y] = Plevel;
      if (LVflag) {			/* LandValue Equation */
	dis = 34 - GetDisCC(x, y);
	dis = dis <<2;
	dis += (TerrainMem[x >>1][y >>1] );
	dis -= (PollutionMem[x][y]);
	if (CrimeMem[x][y] > 190) dis -= 20;
	if (dis > 250) dis = 250;
	if (dis < 1) dis = 1;
	LandValueMem[x][y] = dis;
	LVtot += dis;
	LVnum++;
      } else
	LandValueMem[x][y] = 0;
    }

  if (LVnum)
    LVAverage = LVtot / LVnum;
  else
    LVAverage = 0;

  DoSmooth();
  DoSmooth2();
  pmax = 0;
  pnum = 0;
  ptot = 0;
  for (x = 0; x < HWLDX; x++) {
    for (y = 0; y < HWLDY; y++)  {
      z = tem[x][y];
      PollutionMem[x][y] = z;
      if (z) {				/*  get pollute average  */
	pnum++;
	ptot += z;
	/* find max pol for monster  */
	if ((z > pmax) ||
	    ((z == pmax) && (!(Rand16() & 3)))) {
	  pmax = z;
	  PolMaxX = x <<1;
	  PolMaxY = y <<1;
	}
      }
    }
  }
  if (pnum)
    PolluteAverage = ptot / pnum;
  else
    PolluteAverage = 0;

  SmoothTerrain();

  NewMapFlags[DYMAP] = NewMapFlags[PLMAP] = NewMapFlags[LVMAP] = 1;
}


/* comefrom: PTLScan */
GetPValue(int loc)
{
  if (loc < POWERBASE) {
    if (loc >= HTRFBASE) return (/* 25 */ 75);	/* heavy traf  */
    if (loc >= LTRFBASE) return (/* 10 */ 50);	/* light traf  */
    if (loc <  ROADBASE) {
      if (loc > FIREBASE) return (/* 60 */ 90);
      /* XXX: Why negative pollution from radiation? */
      if (loc >= RADTILE) return (/* -40 */ 255);	/* radioactivity  */
    }
    return (0);
  }
  if (loc <= LASTIND) return (0);
  if (loc < PORTBASE) return (50);	/* Ind  */
  if (loc <= LASTPOWERPLANT) return (/* 60 */ 100);	/* prt, aprt, cpp */
  return (0);
}


/* comefrom: PTLScan DistIntMarket */
GetDisCC(int x, int y)
{
  short xdis, ydis, z;

  if (x > CCx2)
    xdis = x - CCx2;
  else
    xdis = CCx2 - x;

  if (y > CCy2)
    ydis = y - CCy2;
  else
    ydis = CCy2 - y;

  z = (xdis + ydis);
  if (z > 32)
    return (32);
  else
    return (z);
}


/* comefrom: Simulate SpecialInit */
CrimeScan(void)
{
  short numz;
  QUAD totz;
  register short x, y, z;
  short cmax;

  SmoothPSMap();
  SmoothPSMap();
  SmoothPSMap();
  totz = 0;
  numz = 0;
  cmax = 0;
  for (x = 0; x < HWLDX; x++)
    for (y = 0; y < HWLDY; y++) {
      if (z = LandValueMem[x][y]) {
	++numz;
	z = 128 - z;
	z += PopDensity[x][y];
	if (z > 300) z = 300;
	z -= PoliceMap[x >>2][y >>2];
	if (z > 250) z = 250;
	if (z < 0) z = 0;
	CrimeMem[x][y] = z;
	totz += z;
	if ((z > cmax) ||
	    ((z == cmax) && (!(Rand16() & 3)))) {
	  cmax = z;
	  CrimeMaxX = x <<1;
	  CrimeMaxY = y <<1;
	}
      } else {
	CrimeMem[x][y] = 0;
      }
    }
  if (numz)
    CrimeAverage = totz / numz;
  else
    CrimeAverage = 0;
  for (x = 0; x < SmX; x++)
    for (y = 0; y < SmY; y++)
      PoliceMapEffect[x][y] = PoliceMap[x][y];
  NewMapFlags[DYMAP] = NewMapFlags[CRMAP] = NewMapFlags[POMAP] = 1;
}


/* comefrom: PTLScan */
SmoothTerrain(void)
{
  if (DonDither & 1) {
    register int x, y = 0, z = 0, dir = 1;

    for (x = 0; x < QWX; x++) {
      for (; y != QWY && y != -1; y += dir) {
	z += Qtem[(x == 0) ? x : (x - 1)][y] +
	     Qtem[(x == (QWX - 1)) ? x : (x + 1)][y] +
	     Qtem[x][(y == 0) ? (0) : (y - 1)] +
	     Qtem[x][(y == (QWY - 1)) ? y : (y + 1)] +
	     (Qtem[x][y] <<2);
	TerrainMem[x][y] = (unsigned char)(((unsigned)z) >>3);
	z &= 0x7;
      }
      dir = -dir;
      y += dir;
    }
  } else {
    register short x,y,z;

    for (x = 0; x < QWX; x++)
      for (y = 0; y < QWY; y++) {
	z = 0;
	if (x > 0) z += Qtem[x - 1][y];
	if (x < (QWX - 1)) z += Qtem[x + 1][y];
	if (y > 0) z += Qtem[x][y - 1];
	if (y < (QWY - 1)) z += Qtem[x][y + 1];
	TerrainMem[x][y] = (unsigned char)((z >>2) + Qtem[x][y]) >>1;
      }
  }
}

/* comefrom: PopDenScan */
DoSmooth (void)        /* smooths data in tem[x][y] into tem2[x][y]  */
{
  if (DonDither & 2) {
    register int x, y = 0, z = 0, dir = 1;

    for (x = 0; x < HWLDX; x++) {
      for (; y != HWLDY && y != -1; y += dir) {
/*
	z += tem[(x == 0) ? x : (x - 1)][y] +
	     tem[(x == (HWLDX - 1)) ? x : (x + 1)][y] +
	     tem[x][(y == 0) ? (0) : (y - 1)] +
	     tem[x][(y == (HWLDY - 1)) ? y : (y + 1)] +
	     tem[x][y];
	tem2[x][y] = (unsigned char)(((unsigned int)z) >>2);
	z &= 0x3;
*/
	z += tem[(x == 0) ? x : (x - 1)][y] +
	     tem[(x == (HWLDX - 1)) ? x : (x + 1)][y] +
	     tem[x][(y == 0) ? (0) : (y - 1)] +
	     tem[x][(y == (HWLDY - 1)) ? y : (y + 1)] +
	     tem[x][y];
	tem2[x][y] = (unsigned char)(((unsigned int)z) >>2);
	z &= 3;
      }
      dir = -dir;
      y += dir;
    }
  } else {
    register short x,y,z;

    for (x = 0; x < HWLDX; x++) {
      for (y = 0; y < HWLDY; y++) {
	z = 0;
	if (x > 0) z += tem[x - 1][y];
	if (x < (HWLDX - 1)) z += tem[x + 1][y];
	if (y > 0) z += tem[x][y - 1];
	if (y < (HWLDY - 1)) z += tem[x][y + 1];
	z = (z + tem[x][y]) >>2;
	if (z > 255) z = 255;
	tem2[x][y] = (unsigned char)z;
      }
    }
  }
}


/* comefrom: PopDenScan */
DoSmooth2 (void)        /* smooths data in tem2[x][y] into tem[x][y]  */
{
  if (DonDither & 4) {
    register int x, y = 0, z = 0, dir = 1;

    for (x = 0; x < HWLDX; x++) {
      for (; y != HWLDY && y != -1; y += dir) {
/*
	z += tem2[(x == 0) ? x : (x - 1)][y] +
	     tem2[(x == (HWLDX - 1)) ? x : (x + 1)][y] +
	     tem2[x][(y == 0) ? (0) : (y - 1)] +
	     tem2[x][(y == (HWLDY - 1)) ? y : (y + 1)] +
	     tem2[x][y];
	tem[x][y] = (unsigned char)(z >>2);
	z &= 0x3;
*/
	z += tem2[(x == 0) ? x : (x - 1)][y] +
	    tem2[(x == (HWLDX - 1)) ? x : (x + 1)][y] +
	    tem2[x][(y == 0) ? (0) : (y - 1)] +
	    tem2[x][(y == (HWLDY - 1)) ? y : (y + 1)] +
	    tem2[x][y];
	tem[x][y] = (unsigned char)(((unsigned char)z) >>2);
	z &= 3;
      }
      dir = -dir;
      y += dir;
    }
  } else {
    register short x,y,z;

    for (x = 0; x < HWLDX; x++) {
      for (y = 0; y < HWLDY; y++) {
	z = 0;
	if (x > 0) z += tem2[x - 1][y];
	if (x < (HWLDX - 1)) z += tem2[x + 1][y];
	if (y > 0) z += tem2[x][y - 1];
	if (y < (HWLDY - 1)) z += tem2[x][y + 1];
	z = (z + tem2[x][y]) >>2;
	if (z > 255) z = 255;
	tem[x][y] = (unsigned char)z;
      }
    }
  }
}


/* comefrom: PopDenScan */
ClrTemArray(void)
{
  register short x, y, z;

  z = 0;
  for (x = 0; x < HWLDX; x++)
    for (y = 0; y < HWLDY; y++)
      tem[x][y] = z;
}


/* comefrom: FireAnalysis */
SmoothFSMap(void)
{
  register short x, y, edge;

  for (x = 0; x < SmX; x++)
    for (y = 0; y < SmY; y++) {
      edge = 0;
      if (x) edge += FireStMap[x - 1][y];
      if (x < (SmX - 1)) edge += FireStMap[x + 1][y];
      if (y) edge += FireStMap[x][y - 1];
      if (y < (SmY - 1)) edge += FireStMap[x][y + 1];
      edge = (edge >>2) + FireStMap[x][y];
      STem[x][y] = edge >>1;
    }
  for (x = 0; x < SmX; x++)
    for (y = 0; y < SmY; y++)
      FireStMap[x][y] = STem[x][y];
}


/* comefrom: CrimeScan */
SmoothPSMap(void)
{
  register x, y, edge;

  for (x = 0; x < SmX; x++)
    for (y = 0; y < SmY; y++) {
      edge = 0;
      if (x) edge += PoliceMap[x - 1][y];
      if (x < (SmX - 1)) edge += PoliceMap[x + 1][y];
      if (y) edge += PoliceMap[x][y - 1];
      if (y < (SmY - 1)) edge += PoliceMap[x][y + 1];
      edge = (edge >>2) + PoliceMap[x][y];
      STem[x][y] = edge >>1;
    }
  for (x = 0; x < SmX; x++)
    for (y = 0; y < SmY; y++)
      PoliceMap[x][y] = STem[x][y];
}


/* comefrom: PopDenScan */
DistIntMarket(void)
{
  register short x, y, z;

  for (x = 0; x < SmX; x++)
    for (y = 0; y < SmY; y++) {
      z = GetDisCC(x <<2,y <<2);
      z = z <<2;
      z = 64 - z;
      ComRate[x][y] = z;
  }
}
