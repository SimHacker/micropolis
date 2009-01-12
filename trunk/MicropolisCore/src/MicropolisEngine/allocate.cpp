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


////////////////////////////////////////////////////////////////////////


void Micropolis::initMapArrays()
{
    short i;

    if (!mapBase) {
        mapBase = (unsigned short *)NewPtr(
            sizeof(unsigned short) *
            WORLD_X * WORLD_Y);
    }

    for (i = 0; i < WORLD_X; i++) {
        map[i] = (short *)(mapBase + (i * WORLD_Y));
    }

    populationDensityMapBase = NewPtr(HWLDX * HWLDY);
    trafficDensityMapBase = NewPtr(HWLDX * HWLDY);
    pollutionMapBase = NewPtr(HWLDX * HWLDY);
    landValueMapBase = NewPtr(HWLDX * HWLDY);
    crimeMapBase = NewPtr(HWLDX * HWLDY);

    tempMap1Base = NewPtr(HWLDX * HWLDY);
    tempMap2Base = NewPtr(HWLDX * HWLDY);
    tempMap3Base = NewPtr(QWX * QWY);

    for (i = 0; i < HWLDX; i++) {
        populationDensityMap[i] = (Byte *)populationDensityMapBase + (i * HWLDY);
        trafficDensityMap[i] = (Byte *)trafficDensityMapBase + (i * HWLDY);
        pollutionMap[i] = (Byte *)pollutionMapBase + (i * HWLDY);
        landValueMap[i] = (Byte *)landValueMapBase + (i * HWLDY);
        crimeMap[i] = (Byte *)crimeMapBase + (i * HWLDY);

        tempMap1[i] = (Byte *)tempMap1Base + (i * HWLDY);
        tempMap2[i] = (Byte *)tempMap2Base + (i * HWLDY);
    }

    terrainDensityMapBase = NewPtr(QWX * QWY);

    for (i = 0; i < QWX; i++) {
        terrainDensityMap[i] = (Byte *)terrainDensityMapBase + (i * QWY);
        tempMap3[i] = (Byte *)tempMap3Base + (i * QWY);
    }

    resHist = (short *)NewPtr(HISTLEN);
    comHist = (short *)NewPtr(HISTLEN);
    indHist = (short *)NewPtr(HISTLEN);
    moneyHist = (short *)NewPtr(HISTLEN);
    pollutionHist = (short *)NewPtr(HISTLEN);
    crimeHist = (short *)NewPtr(HISTLEN);
    miscHist = (short *)NewPtr(MISCHISTLEN);
    powerMap = (short *)NewPtr(POWERMAPLEN); // Power alloc.
}


void Micropolis::destroyMapArrays()
{

    if (mapBase != NULL) {
        FreePtr(mapBase);
        mapBase = NULL;
    }

    memset(map, 0, sizeof(short *) * WORLD_X);

    if (populationDensityMapBase != NULL) {
        FreePtr(populationDensityMapBase);
        populationDensityMapBase = NULL;
    }

    if (trafficDensityMapBase != NULL) {
        FreePtr(trafficDensityMapBase);
        trafficDensityMapBase = NULL;
    }

    if (pollutionMapBase != NULL) {
        FreePtr(pollutionMapBase);
        pollutionMapBase = NULL;
    }

    if (landValueMapBase != NULL) {
        FreePtr(landValueMapBase);
        landValueMapBase = NULL;
    }

    if (crimeMapBase != NULL) {
        FreePtr(crimeMapBase);
        crimeMapBase = NULL;
    }

    if (tempMap1Base != NULL) {
        FreePtr(tempMap1Base);
        tempMap1Base = NULL;
    }

    if (tempMap2Base != NULL) {
        FreePtr(tempMap2Base);
        tempMap2Base = NULL;
    }

    if (tempMap3Base != NULL) {
        FreePtr(tempMap3Base);
        tempMap3Base = NULL;
    }

    memset(populationDensityMap, 0, sizeof(Byte *) * HWLDX);
    memset(trafficDensityMap, 0, sizeof(Byte *) * HWLDX);
    memset(pollutionMap, 0, sizeof(Byte *) * HWLDX);
    memset(landValueMap, 0, sizeof(Byte *) * HWLDX);
    memset(crimeMap, 0, sizeof(Byte *) * HWLDX);
    memset(tempMap1, 0, sizeof(Byte *) * HWLDX);
    memset(tempMap2, 0, sizeof(Byte *) * HWLDX);
    memset(tempMap3, 0, sizeof(Byte *) * QWX);

    if (terrainDensityMapBase != NULL) {
        FreePtr(terrainDensityMapBase);
        terrainDensityMapBase = NULL;
    }

    memset(terrainDensityMap, 0, sizeof(Byte *) * QWX);

    if (resHist != NULL) {
        FreePtr(resHist);
        resHist = NULL;
    }

    if (comHist != NULL) {
        FreePtr(comHist);
        comHist = NULL;
    }

    if (indHist != NULL) {
        FreePtr(indHist);
        indHist = NULL;
    }

    if (moneyHist != NULL) {
        FreePtr(moneyHist);
        moneyHist = NULL;
    }

    if (pollutionHist != NULL) {
        FreePtr(pollutionHist);
        pollutionHist = NULL;
    }

    if (crimeHist != NULL) {
        FreePtr(crimeHist);
        crimeHist = NULL;
    }

    if (miscHist != NULL) {
        FreePtr(miscHist);
        miscHist = NULL;
    }

    if (powerMap != NULL) {
        FreePtr(powerMap);
        powerMap = NULL;
    }

    for (int i = 0; i < HISTORIES; i++) {
        if (history10[i] != NULL) {
            free(history10[i]);
        }
        if (history120[i] != NULL) {
            free(history120[i]);
        }
    }
}


/**
 * Get the minimal and maximal values of a historic graph.
 * @param historyType  Type of history information. @see HistoryType
 * @param historyScale Scale of history data. @see HistoryScale
 * @param minValResult Pointer to variable to write minimal value to.
 * @param maxValResult Pointer to variable to write maximal value to.
 */
void Micropolis::getHistoryRange(int historyType, int historyScale,
                                 short *minValResult, short *maxValResult)
{
    if (historyType < 0 || historyType >= HISTORY_TYPE_COUNT
            || historyScale < 0 || historyScale >= HISTORY_SCALE_COUNT) {
        *minValResult = 0;
        *maxValResult = 0;
        return;
    }

    short *history = NULL;
    switch (historyType) {
        case HISTORY_TYPE_RES:
            history = resHist;
            break;
        case HISTORY_TYPE_COM:
            history = comHist;
            break;
        case HISTORY_TYPE_IND:
            history = indHist;
            break;
        case HISTORY_TYPE_MONEY:
            history = moneyHist;
            break;
        case HISTORY_TYPE_CRIME:
            history = crimeHist;
            break;
        case HISTORY_TYPE_POLLUTION:
            history = pollutionHist;
            break;
        default:
            NOT_REACHED();
            break;
    }

    int offset = 0;
    switch (historyScale) {
        case HISTORY_SCALE_SHORT:
            offset = 0;
            break;
        case HISTORY_SCALE_LONG:
            offset = 120;
            break;
        default:
            NOT_REACHED();
            break;
    }

    short minVal = 32000;
    short maxVal = -32000;

    for (int i = 0; i < HISTORY_COUNT; i++) {
        short val = history[i + offset];

        minVal = min(val, minVal);
        maxVal = max(val, maxVal);
    }

    *minValResult = minVal;
    *maxValResult = maxVal;
}


/**
 * Get a value from the history tables.
 * @param historyType  Type of history information. @see HistoryType
 * @param historyScale Scale of history data. @see HistoryScale
 * @param historyIndex Index in the data to obtain
 * @return Historic data value of the requested graph
 */
short Micropolis::getHistory(int historyType, int historyScale,
                             int historyIndex)
{
    if (historyType < 0 || historyType >= HISTORY_TYPE_COUNT
            || historyScale < 0 || historyScale >= HISTORY_SCALE_COUNT
            || historyIndex < 0 || historyIndex >= HISTORY_COUNT) {
        return 0;
    }

    short *history = NULL;
    switch (historyType) {
        case HISTORY_TYPE_RES:
            history = resHist;
            break;
        case HISTORY_TYPE_COM:
            history = comHist;
            break;
        case HISTORY_TYPE_IND:
            history = indHist;
            break;
        case HISTORY_TYPE_MONEY:
            history = moneyHist;
            break;
        case HISTORY_TYPE_CRIME:
            history = crimeHist;
            break;
        case HISTORY_TYPE_POLLUTION:
            history = pollutionHist;
            break;
        default:
            NOT_REACHED();
            break;
    }

    int offset = 0;
    switch (historyScale) {
        case HISTORY_SCALE_SHORT:
            offset = 0;
            break;
        case HISTORY_SCALE_LONG:
            offset = 120;
            break;
        default:
            NOT_REACHED();
            break;
    }

    short result = history[historyIndex + offset];

    return result;
}


////////////////////////////////////////////////////////////////////////
