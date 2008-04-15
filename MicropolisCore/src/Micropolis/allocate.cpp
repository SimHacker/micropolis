/* allocate.cpp
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


void Micropolis::initMapArrays()
{
  short i;
  unsigned short *auxPtr, *temp1;

  if (!mapPtr) {
    mapPtr = (unsigned short *)NewPtr(
      sizeof(unsigned short) *
      WORLD_X * WORLD_Y);
  }

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
    PopDensity[i] = (Byte *)auxPopPtr + (i * HWLDY);
    TrfDensity[i] = (Byte *)auxTrfPtr + (i * HWLDY);
    PollutionMem[i] = (Byte *)auxPolPtr + (i * HWLDY);
    LandValueMem[i] = (Byte *)auxLandPtr + (i * HWLDY);
    CrimeMem[i] = (Byte *)auxCrimePtr + (i * HWLDY);

    tem[i] = (Byte *)tem1Base + (i * HWLDY);
    tem2[i] = (Byte *)tem2Base + (i * HWLDY);
  }

  brettPtr = (Ptr)&PopDensity[0][0];

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
  PowerMap = (short *)NewPtr(POWERMAPLEN);              /* power alloc */
}


void Micropolis::destroyMapArrays()
{

  if (mapPtr) {
    FreePtr(mapPtr);
    mapPtr = NULL;
  }

  memset(Map, 0, sizeof(short *) * WORLD_X);

  if (popPtr) {
    FreePtr(popPtr);
    popPtr = NULL;
  }

  if (trfPtr) {
    FreePtr(trfPtr);
    trfPtr = NULL;
  }

  if (polPtr) {
    FreePtr(polPtr);
    polPtr = NULL;
  }

  if (landPtr) {
    FreePtr(landPtr);
    landPtr = NULL;
  }

  if (crimePtr) {
    FreePtr(crimePtr);
    crimePtr = NULL;
  }

  if (tem1Base) {
    FreePtr(tem1Base);
    tem1Base = NULL;
  }

  if (tem2Base) {
    FreePtr(tem2Base);
    tem2Base = NULL;
  }

  auxPopPtr = NULL;
  auxTrfPtr = NULL;
  auxPolPtr = NULL;
  auxLandPtr = NULL;
  auxCrimePtr = NULL;

  memset(PopDensity, 0, sizeof(Byte *) * HWLDX);
  memset(TrfDensity, 0, sizeof(Byte *) * HWLDX);
  memset(PollutionMem, 0, sizeof(Byte *) * HWLDX);
  memset(LandValueMem, 0, sizeof(Byte *) * HWLDX);
  memset(CrimeMem, 0, sizeof(Byte *) * HWLDX);
  memset(tem, 0, sizeof(Byte *) * HWLDX);
  memset(tem2, 0, sizeof(Byte *) * HWLDX);
  memset(tem2, 0, sizeof(Byte *) * HWLDX);

  brettPtr = NULL;

  if (terrainBase) {
    FreePtr(terrainBase);
    terrainBase = NULL;
  }

  if (qTemBase) {
    FreePtr(qTemBase);
    qTemBase = NULL;
  }

  memset(TerrainMem, 0, sizeof(Byte *) * QWX);
  memset(Qtem, 0, sizeof(Byte *) * QWX);

  if (ResHis) {
    FreePtr(ResHis);
    ResHis = NULL;
  }

  if (ComHis) {
    FreePtr(ComHis);
    ComHis = NULL;
  }

  if (IndHis) {
    FreePtr(IndHis);
    IndHis = NULL;
  }

  if (MoneyHis) {
    FreePtr(MoneyHis);
    MoneyHis = NULL;
  }

  if (PollutionHis) {
    FreePtr(PollutionHis);
    PollutionHis = NULL;
  }

  if (CrimeHis) {
    FreePtr(CrimeHis);
    CrimeHis = NULL;
  }

  if (MiscHis) {
    FreePtr(MiscHis);
    MiscHis = NULL;
  }

  if (PowerMap) {
    FreePtr(PowerMap);
    PowerMap = NULL;
  }

}


void Micropolis::GetHistoryRange(
	int historyType,
	int historyScale,
	short *minValResult,
	short *maxValResult)
{
	if ((historyType < 0) ||
		(historyType >= HISTORY_TYPE_COUNT) ||
		(historyScale < 0) ||
		(historyScale >= HISTORY_SCALE_COUNT)) {
		*minValResult = 0;
		*maxValResult = 0;
		return;
	}

	short *history =
		NULL;
	switch (historyType) {
		case HISTORY_TYPE_RES:
			history = ResHis;
			break;
		case HISTORY_TYPE_COM:
			history = ComHis;
			break;
		case HISTORY_TYPE_IND:
			history = IndHis;
			break;
		case HISTORY_TYPE_MONEY:
			history = MoneyHis;
			break;
		case HISTORY_TYPE_CRIME:
			history = CrimeHis;
			break;
		case HISTORY_TYPE_POLLUTION:
			history = PollutionHis;
			break;
		default:
			assert(0); // Should never happen!
			break;
	}

	int offset = 
		0;
	switch (historyScale) {
		case HISTORY_SCALE_SHORT:
			offset = 0;
			break;
		case HISTORY_SCALE_LONG:
			offset = 120;
			break;
		default:
			assert(0); // Should never happen!
			break;
	}

	short minVal = 32000;
	short maxVal = -32000;

	int i;
	for (i = 0; i < HISTORY_COUNT; i++) {
		short val = 
			history[i + offset];
		if (val < minVal) {
			minVal = val;
		}
		if (val > maxVal) {
			maxVal = val;
		}
	}

	*minValResult = minVal;
	*maxValResult = maxVal;
}


short Micropolis::GetHistory(
	int historyType,
	int historyScale,
	int historyIndex)
{
	if ((historyType < 0) ||
		(historyType >= HISTORY_TYPE_COUNT) ||
		(historyScale < 0) ||
		(historyScale >= HISTORY_SCALE_COUNT) ||
		(historyIndex < 0) ||
		(historyIndex >= HISTORY_COUNT)) {
		return 0;
	}

	short *history =
		NULL;
	switch (historyType) {
		case HISTORY_TYPE_RES:
			history = ResHis;
			break;
		case HISTORY_TYPE_COM:
			history = ComHis;
			break;
		case HISTORY_TYPE_IND:
			history = IndHis;
			break;
		case HISTORY_TYPE_MONEY:
			history = MoneyHis;
			break;
		case HISTORY_TYPE_CRIME:
			history = CrimeHis;
			break;
		case HISTORY_TYPE_POLLUTION:
			history = PollutionHis;
			break;
		default:
			assert(0); // Should never happen!
			break;
	}

	int offset = 
		0;
	switch (historyScale) {
		case HISTORY_SCALE_SHORT:
			offset = 0;
			break;
		case HISTORY_SCALE_LONG:
			offset = 120;
			break;
		default:
			assert(0); // Should never happen!
			break;
	}

	short result =
		history[historyIndex + offset];

	return result;
}


////////////////////////////////////////////////////////////////////////
