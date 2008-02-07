/* s_power.c
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


/* Power Scan */


int PowerStackNum;
char PowerStackX[PWRSTKSIZE], PowerStackY[PWRSTKSIZE];
QUAD MaxPower, NumPower;


/* comefrom: TestForCond DoPowerScan TryGo */
MoveMapSim (short MDir)
{
  switch (MDir) {
  case 0:
    if (SMapY > 0) {
      SMapY--;
      return (TRUE);
    }
    if (SMapY < 0)
      SMapY = 0;
    return (FALSE);
  case 1:
    if (SMapX < (WORLD_X - 1)) {
      SMapX++;
      return (TRUE);
    }
    if (SMapX > (WORLD_X - 1))
      SMapX = WORLD_X - 1;
    return (FALSE);
  case 2:
    if (SMapY < (WORLD_Y - 1)) {
      SMapY++;
      return (TRUE);
    }
    if (SMapY > (WORLD_Y - 1))
      SMapY = WORLD_Y - 1;
    return (FALSE);
  case 3:
    if (SMapX > 0) {
      SMapX--;
      return (TRUE);
    }
    if (SMapX < 0)
      SMapX = 0;
    return (FALSE);
  case 4:
    return (TRUE);
  }
  return (FALSE);				
}


/* inlined -Don */
#if 0
/* comefrom: DoPowerScan */
SetPowerBit(void) 
{
  register int PowerWrd;

  /* XXX: assumes 120x100 */
  PowerWrd = (SMapX >>4) + (SMapY <<3);
  PowerMap[PowerWrd] |= 1 << (SMapX & 15);
}
#endif


/* inlined */
#if 0
/* comefrom: TestForCond */
int TestPowerBit(void)
{
  register int PowerWrd;

  if ((CChr9 == NUCLEAR) || (CChr9 == POWERPLANT))
    return(1);
  /* XXX: assumes 120x100 */
  PowerWrd = (SMapX >>4) + (SMapY <<3);
  if (PowerWrd >= PWRMAPSIZE)
    return(0);
  return ((PowerMap[PowerWrd] & (1 << (SMapX & 15))) ? 1 : 0);
}
#endif


/* comefrom: DoPowerScan */
short TestForCond(short TFDir)
{ 	
  register int xsave, ysave, PowerWrd;

  xsave = SMapX;
  ysave = SMapY;
  if (MoveMapSim(TFDir)) {
    /* TestPowerBit was taking a lot of time so I swapped the & order -Don */
#if 0
    if ((Map[SMapX][SMapY] & CONDBIT) &&
	(!TestPowerBit()))
#else
    if ((Map[SMapX][SMapY] & CONDBIT) &&
	(CChr9 != NUCLEAR) &&
	(CChr9 != POWERPLANT) &&
	(
#if 0
	 (PowerWrd = (SMapX >>4) + (SMapY <<3)),
#else
	 (PowerWrd = POWERWORD(SMapX, SMapY)),
#endif
	 ((PowerWrd > PWRMAPSIZE) ||
	  ((PowerMap[PowerWrd] & (1 << (SMapX & 15))) == 0))))
#endif
    {
      SMapX = xsave;
      SMapY = ysave;
      return (TRUE);
    }
  }
  SMapX = xsave;
  SMapY = ysave;
  return (FALSE);
}


/* comefrom: Simulate SpecialInit InitSimMemory */
DoPowerScan()
{	
  short ADir;
  register int ConNum, Dir, x;

  for (x = 0; x < PWRMAPSIZE; x++)
    PowerMap[x] = 0;	/* ClearPowerMem */

  MaxPower = (CoalPop * 700L) + (NuclearPop * 2000L); /* post release */
  NumPower = 0;

  while (PowerStackNum)	{
    PullPowerStack();
    ADir = 4;
    do {
      if (++NumPower > MaxPower) {
	SendMes(40);
	return;
      }
      MoveMapSim(ADir);
/* inlined -Don */
#if 0
      SetPowerBit();
#else
#if 1
      SETPOWERBIT(SMapX, SMapY);
#else
      PowerMap[(SMapX >>4) + (SMapY <<3)] |= 1 << (SMapX & 15);
#endif
#endif
      ConNum = 0;
      Dir = 0;
      while ((Dir < 4) && (ConNum < 2))	{
	if (TestForCond(Dir)) {
	  ConNum++;
	  ADir = Dir;
	}
	Dir++;
      }
      if (ConNum > 1)
	PushPowerStack();
    } while (ConNum);
  }
}


/* comefrom: DoPowerScan DoSPZone */
PushPowerStack(void) 
{ 	
  if (PowerStackNum < (PWRSTKSIZE - 2)) {
    PowerStackNum++;
    PowerStackX[PowerStackNum] = SMapX;
    PowerStackY[PowerStackNum] = SMapY;
   }
}


/* comefrom: DoPowerScan */
PullPowerStack(void)
{ 	
  if (PowerStackNum > 0)  {
    SMapX = PowerStackX[PowerStackNum];
    SMapY = PowerStackY[PowerStackNum];
    PowerStackNum--;
  }
}


