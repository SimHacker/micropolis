/* s_zone.c
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


/* Zone Stuff */


DoZone(void)
{
  short ZonePwrFlg;

  ZonePwrFlg = SetZPower();	/* Set Power Bit in Map from PowerMap */
  if (ZonePwrFlg) PwrdZCnt++;
  else unPwrdZCnt++;

  if (CChr9 > PORTBASE) {	/* do Special Zones  */
    DoSPZone(ZonePwrFlg);
    return;
  }
  if (CChr9 < HOSPITAL) {	
    DoResidential(ZonePwrFlg);
    return;
  }
  if (CChr9 < COMBASE) {
    DoHospChur();
    return;
  }
  if (CChr9 < INDBASE)  {
    DoCommercial(ZonePwrFlg);
    return;
  }
  DoIndustrial(ZonePwrFlg);
  return;
}


DoHospChur(void)
{
  if (CChr9 == HOSPITAL) {
    HospPop++;
    if (!(CityTime & 15)) RepairZone (HOSPITAL, 3); /*post*/
    if (NeedHosp == -1)
      if (!Rand(20))
	ZonePlop(RESBASE);
  }
  if (CChr9 == CHURCH) {
    ChurchPop++;
    if (!(CityTime & 15)) RepairZone (CHURCH, 3); /*post*/
    if (NeedChurch == -1)
      if (!Rand(20))
	ZonePlop(RESBASE);
  }
}


#define T 1
#define F 0
#define ASCBIT (ANIMBIT | CONDBIT | BURNBIT)
#define REGBIT (CONDBIT | BURNBIT)

SetSmoke(int ZonePower)
{
  static short AniThis[8] = {    T,    F,    T,    T,    F,    F,    T,    T };
  static short DX1[8]	  = {   -1,    0,    1,    0,    0,    0,    0,    1 };
  static short DY1[8]	  = {   -1,    0,   -1,   -1,    0,    0,   -1,   -1 };
  static short DX2[8]	  = {   -1,    0,    1,    1,    0,    0,    1,    1 };
  static short DY2[8]	  = {   -1,    0,    0,   -1,    0,    0,   -1,    0 };
  static short AniTabA[8] = {    0,    0,   32,   40,    0,    0,   48,   56 };
  static short AniTabB[8] = {    0,    0,   36,   44,    0,    0,   52,   60 };
  static short AniTabC[8] = { IND1,    0, IND2, IND4,    0,    0, IND6, IND8 };
  static short AniTabD[8] = { IND1,    0, IND3, IND5,    0,    0, IND7, IND9 };
  register short z;

  if (CChr9 < IZB) return;
  z = (CChr9 - IZB) >>3;
  z = z & 7;
  if (AniThis[z]) {
    int xx = SMapX + DX1[z];
    int yy = SMapY + DY1[z];
    if (TestBounds(xx, yy)) {
      if (ZonePower) {
	if ((Map[xx][yy] & LOMASK) == AniTabC[z]) {
	  Map[xx][yy] =
	    ASCBIT | (SMOKEBASE + AniTabA[z]);
	  Map[xx][yy] =
	    ASCBIT | (SMOKEBASE + AniTabB[z]);
	}
      } else {
	if ((Map[xx][yy] & LOMASK) > AniTabC[z]) {
	  Map[xx][yy] =
	    REGBIT | AniTabC[z];
	  Map[xx][yy] =
	    REGBIT | AniTabD[z];
	}
      }
    }
  }
}


DoIndustrial(int ZonePwrFlg)
{
  short tpop, zscore, TrfGood;

  IndZPop++;
  SetSmoke(ZonePwrFlg);
  tpop = IZPop(CChr9);
  IndPop += tpop;
  if (tpop > Rand(5)) TrfGood = MakeTraf(2);
  else TrfGood = TRUE;

  if (TrfGood == -1) {
    DoIndOut(tpop, Rand16() & 1);
    return;
  }

  if (!(Rand16() & 7)) {
    zscore = IValve + EvalInd(TrfGood);
    if (!ZonePwrFlg) zscore = -500;
    if ((zscore > -350) &&
	(((short)(zscore - 26380)) > ((short)Rand16Signed()))) {
      DoIndIn(tpop, Rand16() & 1);
      return;
    }
    if ((zscore < 350) &&
	(((short)(zscore + 26380)) < ((short)Rand16Signed())))
      DoIndOut(tpop, Rand16() & 1);
  }
}


DoCommercial(int ZonePwrFlg)
{
  register short tpop, TrfGood;
  short zscore, locvalve,value;

  ComZPop++;
  tpop = CZPop(CChr9);
  ComPop += tpop;
  if (tpop > Rand(5)) TrfGood = MakeTraf(1);
  else TrfGood = TRUE;

  if (TrfGood == -1) {
    value = GetCRVal();
    DoComOut(tpop, value);
    return;
  }

  if (!(Rand16() & 7)) {
    locvalve = EvalCom(TrfGood);
    zscore = CValve + locvalve;
    if (!ZonePwrFlg) zscore = -500;

    if (TrfGood &&
	(zscore > -350) &&
	(((short)(zscore - 26380)) > ((short)Rand16Signed()))) {
      value = GetCRVal();
      DoComIn(tpop, value);
      return;
    }
    if ((zscore < 350) &&
	(((short)(zscore + 26380)) < ((short)Rand16Signed()))) {
      value = GetCRVal();
      DoComOut(tpop, value);
    }
  }
}


DoResidential(int ZonePwrFlg)
{
  short tpop, zscore, locvalve, value, TrfGood;

  ResZPop++;
  if (CChr9 == FREEZ) tpop = DoFreePop();
  else tpop = RZPop(CChr9);

  ResPop += tpop;
  if (tpop > Rand(35)) TrfGood = MakeTraf(0);
  else TrfGood = TRUE;

  if (TrfGood == -1) {
    value = GetCRVal();
    DoResOut(tpop, value);
    return;
  }

  if ((CChr9 == FREEZ) || (!(Rand16() & 7))) {
    locvalve = EvalRes(TrfGood);
    zscore = RValve + locvalve;
    if (!ZonePwrFlg) zscore = -500;

    if ((zscore > -350) &&
	(((short)(zscore - 26380)) > ((short)Rand16Signed()))) {
      if ((!tpop) && (!(Rand16() & 3))) {
	MakeHosp();
	return;
      }
      value = GetCRVal();
      DoResIn(tpop, value);
      return;
    }
    if ((zscore < 350) &&
	(((short)(zscore + 26380)) < ((short)Rand16Signed()))) {
	    value = GetCRVal();
	    DoResOut(tpop, value);
    }
  }
}


MakeHosp(void)
{
  if (NeedHosp > 0) {
    ZonePlop(HOSPITAL - 4);
    NeedHosp = FALSE;
    return;
  }
  if (NeedChurch > 0) {
    ZonePlop(CHURCH - 4);
    NeedChurch = FALSE;
    return;
  }
}


GetCRVal(void)
{
  register short LVal;

  LVal = LandValueMem[SMapX >>1][SMapY >>1];
  LVal -= PollutionMem[SMapX >>1][SMapY >>1];
  if (LVal < 30) return (0);
  if (LVal < 80) return (1);
  if (LVal < 150) return (2);
  return (3);
}


DoResIn(int pop, int value)
{
  short z;

  z = PollutionMem[SMapX >>1][SMapY >>1];
  if (z > 128) return;

  if (CChr9 == FREEZ) {
    if (pop < 8) {
      BuildHouse(value);
      IncROG(1);
      return;
    }
    if (PopDensity[SMapX >>1][SMapY >>1] > 64) {
      ResPlop(0, value);
      IncROG(8);
      return;
    }
    return;
  }
  if (pop < 40) {
    ResPlop((pop / 8) - 1, value);
    IncROG(8);
  }
}


DoComIn(int pop, int value)
{
  register short z;

  z = LandValueMem[SMapX >>1][SMapY >>1];
  z = z >>5;
  if (pop > z) return;

  if (pop < 5) {
    ComPlop(pop, value);
    IncROG(8);
  }
}


DoIndIn(int pop, int value)
{
  if (pop < 4) {
    IndPlop(pop, value);
    IncROG(8);
  }
}


IncROG(int amount)
{
  RateOGMem[SMapX>>3][SMapY>>3] += amount<<2;
}


DoResOut(int pop, int value)
{
  static short Brdr[9] = {0,3,6,1,4,7,2,5,8};
  register short x, y, loc, z;

  if (!pop) return;
  if (pop > 16) {
    ResPlop(((pop - 24) / 8), value);
    IncROG(-8);
    return;
  }
  if (pop == 16) {
    IncROG(-8);
    Map[SMapX][SMapY] = (FREEZ | BLBNCNBIT | ZONEBIT);
    for (x = SMapX - 1; x <= SMapX + 1; x++)
      for (y = SMapY - 1; y <= SMapY + 1; y++)
	if (x >= 0 && x < WORLD_X &&
	    y >= 0 && y < WORLD_Y) {
	  if ((Map[x][y] & LOMASK) != FREEZ)
	    Map[x][y] = LHTHR + value +
	      Rand(2) + BLBNCNBIT;
	}
  }
  if (pop < 16) {
    IncROG(-1);
    z = 0;
    for (x = SMapX - 1; x <= SMapX + 1; x++)
      for (y = SMapY - 1; y <= SMapY + 1; y++) {
	if (x >= 0 && x < WORLD_X &&
	    y >= 0 && y < WORLD_Y) {
	  loc = Map[x][y] & LOMASK;
	  if ((loc >= LHTHR) && (loc <= HHTHR)) {
	    Map[x][y] = Brdr[z] +
	      BLBNCNBIT + FREEZ - 4;
	    return;
	  }
	}
	z++;
      }
  }
}


DoComOut(int pop, int value)
{
  if (pop > 1) {
    ComPlop(pop - 2, value);
    IncROG(-8);
    return;
  }
  if (pop == 1) {
    ZonePlop(COMBASE);
    IncROG(-8);
  }
}


DoIndOut(int pop, int value)
{
  if (pop > 1) {
    IndPlop(pop - 2, value);
    IncROG(-8);
    return;
  }
  if (pop == 1) {
    ZonePlop(INDCLR - 4);
    IncROG(-8);
  }
}


RZPop(int Ch9)
{
  short CzDen;

  CzDen = (((Ch9 - RZB) / 9) % 4);
  return ((CzDen * 8) + 16);
}


CZPop(int Ch9)
{
  short CzDen;

  if (Ch9 == COMCLR) return (0);
  CzDen = (((Ch9 - CZB) / 9) % 5) + 1;
  return (CzDen);
}


IZPop(int Ch9)
{
  short CzDen;

  if (Ch9 == INDCLR) return (0);
  CzDen = (((Ch9 - IZB) / 9) % 4) + 1;
  return (CzDen);
}


BuildHouse(int value)
{
  short z, score, hscore, BestLoc;
  static short ZeX[9] = { 0,-1, 0, 1,-1, 1,-1, 0, 1};
  static short ZeY[9] = { 0,-1,-1,-1, 0, 0, 1, 1, 1};

  BestLoc = 0;
  hscore = 0;
  for (z = 1; z < 9; z++) {
    int xx = SMapX + ZeX[z];
    int yy = SMapY + ZeY[z];
    if (TestBounds(xx, yy)) {
      score = EvalLot(xx, yy);
      if (score != 0) {
	if (score > hscore) {
	  hscore = score;
	  BestLoc = z;
	}
	if ((score == hscore) && !(Rand16() & 7))
	  BestLoc = z;
      }
    }
  }
  if (BestLoc) {
    int xx = SMapX + ZeX[BestLoc];
    int yy = SMapY + ZeY[BestLoc];
    if (TestBounds(xx, yy)) {
      Map[xx][yy] = HOUSE + BLBNCNBIT + Rand(2) + (value * 3);
    }
  }
}


ResPlop (int Den, int Value)
{
  short base;

  base = (((Value * 4) + Den) * 9) + RZB - 4;
  ZonePlop(base);
}


ComPlop (int Den, int Value)
{
  short base;
	
  base = (((Value * 5) + Den) * 9) + CZB - 4;
  ZonePlop(base);
}


IndPlop (int Den, int Value)
{
  short base;
	
  base = (((Value * 4) + Den) * 9) + (IZB - 4);
  ZonePlop (base);
}


EvalLot (int x, int y)
{
  short z, score;
  static short DX[4] = { 0, 1, 0,-1};
  static short DY[4] = {-1, 0, 1, 0};

  /* test for clear lot */
  z = Map[x][y] & LOMASK;
  if (z && ((z < RESBASE) || (z > RESBASE + 8)))
    return (-1);
  score = 1;
  for (z = 0; z < 4; z++) {
    int xx = x + DX[z];
    int yy = y + DY[z];
    if (TestBounds(xx, yy) && 
	Map[xx][yy] &&
	((Map[xx][yy] & LOMASK) <= LASTROAD)) {
      score++;		/* look for road */
    }
  }
  return (score);
}


ZonePlop (int base)
{
  short z, x;
  static short Zx[9] = {-1, 0, 1,-1, 0, 1,-1, 0, 1};
  static short Zy[9] = {-1,-1,-1, 0, 0, 0, 1, 1, 1};

  for (z = 0; z < 9; z++) {		/* check for fire  */
    int xx = SMapX + Zx[z];
    int yy = SMapY + Zy[z];
    if (TestBounds(xx, yy)) {
      x = Map[xx][yy] & LOMASK;
      if ((x >= FLOOD) && (x < ROADBASE)) return (FALSE);
    }
  }
  for (z = 0; z < 9; z++) {
    int xx = SMapX + Zx[z];
    int yy = SMapY + Zy[z];
    if (TestBounds(xx, yy)) {
      Map[xx][yy] = base + BNCNBIT;
    }
  base++;
  }
  CChr = Map[SMapX][SMapY];
  SetZPower();
  Map[SMapX][SMapY] |= ZONEBIT + BULLBIT;
}


EvalRes (int traf)
{
  register short Value;

  if (traf < 0) return (-3000);

  Value = LandValueMem[SMapX >>1][SMapY >>1];
  Value -= PollutionMem[SMapX >>1][SMapY >>1];

  if (Value < 0) Value = 0;		/* Cap at 0 */
  else Value = Value <<5;

  if (Value > 6000) Value = 6000;	/* Cap at 6000 */

  Value = Value - 3000;
  return (Value);
}


EvalCom (int traf)
{
  short Value;

  if (traf < 0) return (-3000);
  Value = ComRate[SMapX >>3][SMapY >>3];
  return (Value);
}


EvalInd (int traf)
{
  if (traf < 0) return (-1000);
  return (0);
}


DoFreePop (void)
{
  short count;
  register short loc, x, y;

  count = 0;
  for (x = SMapX - 1; x <= SMapX + 1; x++)
    for (y = SMapY - 1; y <= SMapY + 1; y++) {
      if (x >= 0 && x < WORLD_X &&
	  y >= 0 && y < WORLD_Y) {
	loc = Map[x][y] & LOMASK;
	if ((loc >= LHTHR) && (loc <= HHTHR))
	  count++;
      }
    }
  return (count);
}


SetZPower(void)		/* set bit in MapWord depending on powermap  */
{
  short z;
  QUAD PowerWrd;

/* TestPowerBit was taking alot of time so I inlined it. -Don */

#if 0

  if (z = TestPowerBit())
    Map[SMapX][SMapY] = CChr | PWRBIT;
  else
    Map[SMapX][SMapY] = CChr & (~PWRBIT);
  return (z);

#else

  if ((CChr9 == NUCLEAR) ||
      (CChr9 == POWERPLANT) ||
      (
#if 1
       (PowerWrd = POWERWORD(SMapX, SMapY)),
#else
       (PowerWrd = (SMapX >>4) + (SMapY <<3)),
#endif
       ((PowerWrd < PWRMAPSIZE) &&
	(PowerMap[PowerWrd] & (1 << (SMapX & 15)))))) {
    Map[SMapX][SMapY] = CChr | PWRBIT;
    return 1;
  } else {
    Map[SMapX][SMapY] = CChr & (~PWRBIT);
    return 0;
  }

#endif
}
