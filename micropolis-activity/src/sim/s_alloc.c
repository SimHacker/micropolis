/* s_alloc.c
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


/* Allocate Stuff */


short SMapX, SMapY;
short CChr, CChr9;

short RoadTotal, RailTotal, FirePop;

short ResPop, ComPop, IndPop, TotalPop, LastTotalPop;
short ResZPop, ComZPop, IndZPop, TotalZPop;
short HospPop, ChurchPop, StadiumPop;
short PolicePop, FireStPop;
short CoalPop, NuclearPop, PortPop, APortPop;

short NeedHosp, NeedChurch;
short CrimeAverage, PolluteAverage, LVAverage;

QUAD CityTime;
short StartingYear;

short *Map[WORLD_X];

short ResHisMax, Res2HisMax;
short ComHisMax, Com2HisMax;
short IndHisMax, Ind2HisMax;
short CensusChanged;

short MessagePort;
short MesX, MesY;

QUAD RoadSpend, PoliceSpend, FireSpend;
QUAD RoadFund, PoliceFund, FireFund;
short RoadEffect, PoliceEffect, FireEffect;
QUAD TaxFund; 
short CityTax, TaxFlag;
unsigned char tileSynch = 0x01;

Byte *PopDensity[HWLDX];
Byte *TrfDensity[HWLDX];
Byte *PollutionMem[HWLDX];
Byte *LandValueMem[HWLDX];
Byte *CrimeMem[HWLDX];
Byte *tem[HWLDX];
Byte *tem2[HWLDX];
Byte *TerrainMem[QWX];
Byte *Qtem[QWX];

short RateOGMem[SmX][SmY];
short FireStMap[SmX][SmY];
short PoliceMap[SmX][SmY];
short PoliceMapEffect[SmX][SmY];

short FireRate[SmX][SmY];
short ComRate[SmX][SmY];
short STem[SmX][SmY];

Ptr terrainBase;
Ptr qTemBase;
Ptr tem1Base;
Ptr tem2Base;

Ptr popPtr;
Ptr trfPtr;
Ptr polPtr;
Ptr landPtr;
Ptr crimePtr;
	
Ptr auxPopPtr;
Ptr auxTrfPtr;
Ptr auxPolPtr;
Ptr auxLandPtr;
Ptr auxCrimePtr;
Ptr brettPtr;

unsigned short *mapPtr;
short *ResHis;
short *ComHis;
short *IndHis;
short *MoneyHis;
short *PollutionHis;
short *CrimeHis;
short *MiscHis;
short *PowerMap;


int initMapArrays(void)
{
  short i;
  unsigned short *auxPtr, *temp1;

  if (!mapPtr)
    mapPtr = (unsigned short *)NewPtr(sizeof(unsigned short) *
				      WORLD_X * WORLD_Y);
  auxPtr = mapPtr;

  for (i = 0; i < WORLD_X; i++) {
    temp1 = auxPtr + i * WORLD_Y;
    Map[i] = (short *)temp1;
  }

  popPtr = NewPtr(HWLDX * HWLDY);
  trfPtr = NewPtr(HWLDX * HWLDY);
  polPtr = NewPtr(HWLDX * HWLDY);
  landPtr = NewPtr(HWLDX * HWLDY);
  crimePtr = NewPtr(HWLDX * HWLDY);

  tem1Base = NewPtr(HWLDX * HWLDY);
  tem2Base = NewPtr(HWLDX * HWLDY);

  auxPopPtr = popPtr;
  auxTrfPtr = trfPtr;
  auxPolPtr = polPtr;
  auxLandPtr = landPtr;
  auxCrimePtr = crimePtr;

  for (i = 0; i < HWLDX; i++) {
    PopDensity[i] = (Byte *) auxPopPtr + (i * HWLDY);
    TrfDensity[i] = (Byte *) auxTrfPtr + (i * HWLDY);
    PollutionMem[i] = (Byte *) auxPolPtr + (i * HWLDY);
    LandValueMem[i] = (Byte *) auxLandPtr + (i * HWLDY);
    CrimeMem[i] = (Byte *) auxCrimePtr + (i * HWLDY);

    tem[i] = (Byte *)tem1Base + (i * HWLDY);
    tem2[i] = (Byte *) tem2Base + (i * HWLDY);
  }

  brettPtr = (Ptr) &PopDensity[0][0];

  terrainBase = NewPtr(QWX * QWY);
  qTemBase = NewPtr(QWX * QWY);

  for (i = 0; i < QWX; i++) {
    TerrainMem[i] = (Byte *)terrainBase + (i * QWY);
    Qtem[i] = (Byte *)qTemBase + (i * QWY);
  }

  ResHis = (short *)NewPtr(HISTLEN);
  ComHis = (short *)NewPtr(HISTLEN);
  IndHis = (short *)NewPtr(HISTLEN);
  MoneyHis = (short *)NewPtr(HISTLEN);
  PollutionHis = (short *)NewPtr(HISTLEN);
  CrimeHis = (short *)NewPtr(HISTLEN);
  MiscHis = (short *)NewPtr(MISCHISTLEN);
  PowerMap = (short *)NewPtr(POWERMAPLEN);		/* power alloc */
}
