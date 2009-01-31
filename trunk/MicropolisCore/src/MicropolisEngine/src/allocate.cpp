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

/** @file allocate.cpp */

////////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "micropolis.h"


////////////////////////////////////////////////////////////////////////

/** Allocate and initialize arrays for the maps */
void Micropolis::initMapArrays()
{
    short i;

    if (!mapBase) {
        mapBase = (unsigned short *)newPtr(
            sizeof(unsigned short) *
            WORLD_W * WORLD_H);
    }

    for (i = 0; i < WORLD_W; i++) {
        map[i] = (unsigned short *)(mapBase + (i * WORLD_H));
    }

    populationDensityMapBase = newPtr(WORLD_W_2 * WORLD_H_2);

    tempMap1Base = newPtr(WORLD_W_2 * WORLD_H_2);
    tempMap2Base = newPtr(WORLD_W_2 * WORLD_H_2);
    tempMap3Base = newPtr(WORLD_W_4 * WORLD_H_4);

    for (i = 0; i < WORLD_W_2; i++) {
        populationDensityMap[i] = (Byte *)populationDensityMapBase + (i * WORLD_H_2);

        tempMap1[i] = (Byte *)tempMap1Base + (i * WORLD_H_2);
        tempMap2[i] = (Byte *)tempMap2Base + (i * WORLD_H_2);
    }

    for (i = 0; i < WORLD_W_4; i++) {
        tempMap3[i] = (Byte *)tempMap3Base + (i * WORLD_H_4);
    }

    resHist = (short *)newPtr(HISTORY_LENGTH);
    comHist = (short *)newPtr(HISTORY_LENGTH);
    indHist = (short *)newPtr(HISTORY_LENGTH);
    moneyHist = (short *)newPtr(HISTORY_LENGTH);
    pollutionHist = (short *)newPtr(HISTORY_LENGTH);
    crimeHist = (short *)newPtr(HISTORY_LENGTH);
    miscHist = (short *)newPtr(MISC_HISTORY_LENGTH);
    powerMap = (char *)newPtr(POWER_MAP_LENGTH);
}


/** Free all map arrays */
void Micropolis::destroyMapArrays()
{

    if (mapBase != NULL) {
        freePtr(mapBase);
        mapBase = NULL;
    }

    memset(map, 0, sizeof(short *) * WORLD_W);

    memset(populationDensityMap, 0, sizeof(Byte *) * WORLD_W_2);
    trafficDensityMap.clear();
    pollutionMap.clear();
    landValueMap.clear();
    crimeMap.clear();
    memset(tempMap1, 0, sizeof(Byte *) * WORLD_W_2);
    memset(tempMap2, 0, sizeof(Byte *) * WORLD_W_2);
    memset(tempMap3, 0, sizeof(Byte *) * WORLD_W_4);

    terrainDensityMap.clear();



    if (populationDensityMapBase != NULL) {
        freePtr(populationDensityMapBase);
        populationDensityMapBase = NULL;
    }

    if (tempMap1Base != NULL) {
        freePtr(tempMap1Base);
        tempMap1Base = NULL;
    }

    if (tempMap2Base != NULL) {
        freePtr(tempMap2Base);
        tempMap2Base = NULL;
    }

    if (tempMap3Base != NULL) {
        freePtr(tempMap3Base);
        tempMap3Base = NULL;
    }



    if (resHist != NULL) {
        freePtr(resHist);
        resHist = NULL;
    }

    if (comHist != NULL) {
        freePtr(comHist);
        comHist = NULL;
    }

    if (indHist != NULL) {
        freePtr(indHist);
        indHist = NULL;
    }

    if (moneyHist != NULL) {
        freePtr(moneyHist);
        moneyHist = NULL;
    }

    if (pollutionHist != NULL) {
        freePtr(pollutionHist);
        pollutionHist = NULL;
    }

    if (crimeHist != NULL) {
        freePtr(crimeHist);
        crimeHist = NULL;
    }

    if (miscHist != NULL) {
        freePtr(miscHist);
        miscHist = NULL;
    }

    if (powerMap != NULL) {
        freePtr(powerMap);
        powerMap = NULL;
    }

}


////////////////////////////////////////////////////////////////////////
