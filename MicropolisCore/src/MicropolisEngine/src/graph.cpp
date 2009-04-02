/* graph.cpp
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

/** @file graph.cpp */

////////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "micropolis.h"


////////////////////////////////////////////////////////////////////////


//char *Micropolis::histName[] = {
//  "Residential", "Commercial", "Industrial",
//  "Cash Flow", "Crime", "Pollution"
//};

//unsigned char Micropolis::histColor[] = {
//  COLOR_LIGHTGREEN, COLOR_DARKBLUE, COLOR_YELLOW,
//  COLOR_DARKGREEN, COLOR_RED, COLOR_OLIVE
//};


////////////////////////////////////////////////////////////////////////


/**
 * Copy history data to new array, scaling as needed.
 * @param hist  Source history data.
 * @param s     Destination byte array.
 * @param scale Scale factor.
 * @todo Figure out why we copy data.
 */
void Micropolis::drawMonth(short *hist, unsigned char *s, float scale)
{
    int val, x;

    for (x = 0; x < 120; x++) {
        val = (int)(hist[x] * scale);
        s[119 - x] = (unsigned char)clamp(val, 0, 255);
    }
}


/**
 * Set flag that graph data has been changed and graphs should be updated.
 * @todo Rename function.
 */
void Micropolis::changeCensus()
{
    censusChanged = true;
}


/**
 * If graph data has been changed, update all graphs.
 * If graphs have been changed, tell the user front-end about it.
 */
void Micropolis::graphDoer()
{
    if (censusChanged) {
        newGraph = true;
        censusChanged = false;
    }

    if (newGraph) {
        callback("UIUpdate", "s", "history");
        newGraph = false;
    }
}


/** Initialize graphs */
void Micropolis::initGraphs()
{
    if (!historyInitialized) {
        historyInitialized = true;
    }
}


/** Compute various max ranges of graphs */
void Micropolis::initGraphMax()
{
    int x;

    resHist10Max = 0;
    comHist10Max = 0;
    indHist10Max = 0;

    for (x = 118; x >= 0; x--) {

        if (resHist[x] < 0) {
            resHist[x] = 0;
        }
        if (comHist[x] < 0) {
            comHist[x] = 0;
        }
        if (indHist[x] < 0) {
            indHist[x] = 0;
        }

        resHist10Max = max(resHist10Max, resHist[x]);
        comHist10Max = max(comHist10Max, comHist[x]);
        indHist10Max = max(indHist10Max, indHist[x]);

    }

    graph10Max =
        max(resHist10Max,
            max(comHist10Max,
                indHist10Max));

    resHist120Max = 0;
    comHist120Max = 0;
    indHist120Max = 0;

    for (x = 238; x >= 120; x--) {

        if (resHist[x] < 0) {
            resHist[x] = 0;
        }
        if (comHist[x] < 0) {
            comHist[x] = 0;
        }
        if (indHist[x] < 0) {
            indHist[x] = 0;
        }

        resHist120Max = max(resHist120Max, resHist[x]);
        comHist120Max = max(comHist120Max, comHist[x]);
        indHist120Max = max(indHist120Max, indHist[x]);

    }

    graph120Max =
        max(resHist120Max,
            max(comHist120Max,
                indHist120Max));
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


/**
 * Store a value into the history tables.
 * @param historyType  Type of history information. @see HistoryType
 * @param historyScale Scale of history data. @see HistoryScale
 * @param historyIndex Index in the data to obtain
 * @param historyValue Index in the value to store
 */
void Micropolis::setHistory(int historyType, int historyScale,
                            int historyIndex, short historyValue)
{
    if (historyType < 0 || historyType >= HISTORY_TYPE_COUNT
            || historyScale < 0 || historyScale >= HISTORY_SCALE_COUNT
            || historyIndex < 0 || historyIndex >= HISTORY_COUNT) {
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

    history[historyIndex + offset] = historyValue;
}


////////////////////////////////////////////////////////////////////////
