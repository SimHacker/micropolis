/* scan.cpp
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

/** @file scan.cpp */

////////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "micropolis.h"


////////////////////////////////////////////////////////////////////////


/* comefrom: simulate SpecialInit */
void Micropolis::fireAnalysis()
{
    /* Make firerate map from firestation map */

    register int x, y;

    smoothFireStationMap();
    smoothFireStationMap();
    smoothFireStationMap();

    for (x = 0; x < WORLD_W_8; x++) {
        for (y = 0; y < WORLD_H_8; y++) {
            fireStationEffectMap.set(x, y, fireStationMap.get(x, y));
        }
    }

    newMapFlags[MAP_TYPE_FIRE_RADIUS] = 1;
    newMapFlags[MAP_TYPE_DYNAMIC] = 1;
}


/** @todo The tempMap1 has MAP_BLOCKSIZE > 1, so we may be able to optimize
 *        the first x, y loop.
 */
void Micropolis::populationDensityScan()
{
    /*  sets: populationDensityMap, , , comRateMap  */
    Quad Xtot, Ytot, Ztot;
    short x, y, z;

    tempMap1.clear();
    Xtot = 0;
    Ytot = 0;
    Ztot = 0;
    for (x = 0; x < WORLD_W; x++) {
        for (y = 0; y < WORLD_H; y++) {
            z = map[x][y];
            if (z & ZONEBIT) {
                z = z & LOMASK;
                curMapX = x;
                curMapY = y;
                z = getPopulationDensity(z) <<3;
                if (z > 254) {
                    z = 254;
                }
                tempMap1.set(x >>1, y >>1, (Byte)z);
                Xtot += x;
                Ytot += y;
                Ztot++;
            }
        }
    }

    doSmooth1(); // tempMap1 -> tempMap2
    doSmooth2(); // tempMap2 -> tempMap1
    doSmooth1(); // tempMap1 -> tempMap2

    assert(populationDensityMap.MAP_BLOCKSIZE == 2);
    for (x = 0; x < WORLD_W_2; x++) {
        for (y = 0; y < WORLD_H_2; y++) {
            populationDensityMap.set(x, y, tempMap2.get(x, y) <<1);
        }
    }

    computeComRateMap();          /* Compute the comRateMap */

    if (Ztot > 0) {               /* Find Center of Mass for City */
        cityCenterX = (short)(Xtot / Ztot);
        cityCenterY = (short)(Ytot / Ztot);
    } else {
        cityCenterX = WORLD_W_2;  /* if pop==0 center of map is city center */
        cityCenterY = WORLD_H_2;
    }

    cityCenterX2 = cityCenterX >>1;
    cityCenterY2 = cityCenterY >>1;

    newMapFlags[MAP_TYPE_POPULATION_DENSITY] = 1;
    newMapFlags[MAP_TYPE_RATE_OF_GROWTH] = 1;
    newMapFlags[MAP_TYPE_DYNAMIC] = 1;
}


/* comefrom: populationDensityScan */
int Micropolis::getPopulationDensity(int Ch9)
{
    int pop;

    if (Ch9 == FREEZ) {
        pop = doFreePop();
        return pop;
    }

    if (Ch9 < COMBASE) {
        pop = getResZonePop(Ch9);
        return pop;
    }

    if (Ch9 < INDBASE) {
        pop = (getComZonePop(Ch9) <<3);
        return pop;
    }

    if (Ch9 < PORTBASE) {
        pop = (getIndZonePop(Ch9) <<3);
        return pop;
    }

    return 0;
}


/* comefrom: simulate SpecialInit */
void Micropolis::pollutionTerrainLandValueScan()
{
    /* Does pollution, terrain, land value */
    Quad ptot, LVtot;
    register int x, y, z, dis;
    int Plevel, LVflag, loc, zx, zy, Mx, My, pnum, LVnum, pmax;

    // tempMap3 is a map of development density, smoothed into terrainMap.
    tempMap3.clear();

    LVtot = 0;
    LVnum = 0;

    for (x = 0; x < WORLD_W_2; x++) {
        for (y = 0; y < WORLD_H_2; y++) {
            Plevel = 0;
            LVflag = 0;
            zx = x <<1;
            zy = y <<1;

            for (Mx = zx; Mx <= zx + 1; Mx++) {
                for (My = zy; My <= zy + 1; My++) {
                    loc = (map[Mx][My] & LOMASK);
                    if (loc) {
                        if (loc < RUBBLE) {
                            // Incremenet terrain memory.
                            Byte value = tempMap3.get(x >>1, y >>1);
                            tempMap3.set(x >>1, y >>1, value + 15);
                            continue;
                        }
                        Plevel += getPollutionValue(loc);
                        if (loc >= ROADBASE) {
                            LVflag++;
                        }
                    }
                }
            }

/* XXX ??? This might have to do with the radiation tile returning -40.
            if (Plevel < 0) {
                Plevel = 250;
            }
*/

            if (Plevel > 255) {
                Plevel = 255;
            }

            tempMap1.set(x, y, Plevel);



            if (LVflag) {                     /* LandValue Equation */
                dis = 34 - getCityCenterDistance(x, y);
                dis = dis <<2;
                dis += terrainDensityMap.get(x >>1, y >>1);
                dis -= pollutionMap.get(x, y);
                if (crimeMap.get(x, y) > 190) {
                    dis -= 20;
                }
                if (dis > 250) {
                    dis = 250;
                }
                if (dis < 1) {
                    dis = 1;
                }
                landValueMap.set(x, y, dis);
                LVtot += dis;
                LVnum++;
            } else {
                landValueMap.set(x, y, 0);
            }
        }
    }

    if (LVnum) {
        landValueAverage = (short)(LVtot / LVnum);
    } else {
        landValueAverage = 0;
    }

    doSmooth1(); // tempMap1 -> tempMap2
    doSmooth2(); // tempMap2 -> tempMap1

    pmax = 0;
    pnum = 0;
    ptot = 0;

    for (x = 0; x < WORLD_W; x += pollutionMap.MAP_BLOCKSIZE) {
        for (y = 0; y < WORLD_H; y += pollutionMap.MAP_BLOCKSIZE)  {
            z = tempMap1.worldGet(x, y);
            pollutionMap.worldSet(x, y, z);

            if (z) { /*  get pollute average  */
                pnum++;
                ptot += z;
                /* find max pol for monster  */
                if (z > pmax || (z == pmax && (getRandom16() & 3) == 0)) {
                    pmax = z;
                    pollutionMaxX = x;
                    pollutionMaxY = y;
                }
            }
        }
    }
    if (pnum) {
        pollutionAverage = (short)(ptot / pnum);
    } else {
        pollutionAverage = 0;
    }

    smoothTerrain();

    newMapFlags[MAP_TYPE_POLLUTION] = 1;
    newMapFlags[MAP_TYPE_LAND_VALUE] = 1;
    newMapFlags[MAP_TYPE_DYNAMIC] = 1;
}


/**
 * Return pollution of a tile value
 * @param loc Tile character
 * @return Value of the pollution (0..255, bigger is worse)
 */
int Micropolis::getPollutionValue(int loc)
{
    if (loc < POWERBASE) {

        if (loc >= HTRFBASE) {
            return /* 25 */ 75;     /* heavy traf  */
        }

        if (loc >= LTRFBASE) {
            return /* 10 */ 50;     /* light traf  */
        }

        if (loc <  ROADBASE) {

            if (loc > FIREBASE) {
                return /* 60 */ 90;
            }

            /* XXX: Why negative pollution from radiation? */
            if (loc >= RADTILE) {
                return /* -40 */ 255; /* radioactivity  */
            }

        }
        return 0;
    }

    if (loc <= LASTIND) {
        return 0;
    }

    if (loc < PORTBASE) {
        return 50;        /* Ind  */
    }

    if (loc <= LASTPOWERPLANT) {
        return /* 60 */ 100;      /* prt, aprt, cpp */
    }

    return 0;
}


/**
 * Compute Manhattan distance from given position to (#cityCenterX2, #cityCenterY2).
 * @param x X coordinate of given position.
 * @param y Y coordinate of given position.
 * @return Manhattan distance (\c dx+dy ) between both positions.
 * @note For long distances (> 32), value 32 is returned.
 */
int Micropolis::getCityCenterDistance(int x, int y)
{
    int xDis, yDis;

    if (x > cityCenterX2) {
        xDis = x - cityCenterX2;
    } else {
        xDis = cityCenterX2 - x;
    }

    if (y > cityCenterY2) {
        yDis = y - cityCenterY2;
    } else {
        yDis = cityCenterY2 - y;
    }

    return min(xDis + yDis, 32);
}


/* comefrom: simulate SpecialInit */
void Micropolis::crimeScan()
{
    short numz;
    Quad totz;
    short x, y, z;
    short cmax;

    smoothPoliceStationMap();
    smoothPoliceStationMap();
    smoothPoliceStationMap();

    totz = 0;
    numz = 0;
    cmax = 0;

    for (x = 0; x < WORLD_W_2; x++) {
        for (y = 0; y < WORLD_H_2; y++) {
            z = landValueMap.get(x, y);
            if (z > 0) {
                ++numz;
                z = 128 - z;
                z += populationDensityMap.get(x, y);
                if (z > 300) {
                    z = 300;
                }
                z -= policeStationMap.get(x >>2, y >>2) ;
                z = clamp(z, (short)0, (short)250);
                crimeMap.set(x, y, (Byte)z);
                totz += z;

                // Update new crime hot-spot
                if (z > cmax || (z == cmax && (getRandom16() & 3) == 0)) {
                    cmax = z;
                    crimeMaxX = x <<1;
                    crimeMaxY = y <<1;
                }

            } else {
                crimeMap.set(x, y, 0);
            }
        }
    }

    if (numz > 0) {
        crimeAverage = (short)(totz / numz);
    } else {
        crimeAverage = 0;
    }

    for (x = 0; x < WORLD_W_8; x++) {
        for (y = 0; y < WORLD_H_8; y++) {
            policeStationEffectMap.set(x, y, policeStationMap.get(x, y));
        }
    }

    newMapFlags[MAP_TYPE_CRIME] = 1;
    newMapFlags[MAP_TYPE_POLICE_RADIUS] = 1;
    newMapFlags[MAP_TYPE_DYNAMIC] = 1;
}


/* comefrom: pollutionTerrainLandValueScan */
void Micropolis::smoothTerrain()
{
    if (donDither & 1) {
        int x, y = 0, dir = 1;
        unsigned z = 0;

        for (x = 0; x < WORLD_W_4; x++) {
            for (; y != WORLD_H_4 && y != -1; y += dir) {
                z +=
                    tempMap3.get((x == 0) ? x : (x - 1), y) +
                    tempMap3.get((x == (WORLD_W_4 - 1)) ? x : (x + 1), y) +
                    tempMap3.get(x, (y == 0) ? (0) : (y - 1)) +
                    tempMap3.get(x, (y == (WORLD_H_4 - 1)) ? y : (y + 1)) +
                    (tempMap3.get(x, y) <<2);
                Byte val = (Byte)(z / 8);
                terrainDensityMap.set(x, y, val);
                z &= 0x7;
            }
            dir = -dir;
            y += dir;
        }
    } else {
        short x, y;

        for (x = 0; x < WORLD_W_4; x++) {
            for (y = 0; y < WORLD_H_4; y++) {
                unsigned z = 0;
                if (x > 0) {
                    z += tempMap3.get(x - 1, y);
                }
                if (x < (WORLD_W_4 - 1)) {
                    z += tempMap3.get(x + 1, y);
                }
                if (y > 0) {
                    z += tempMap3.get(x, y - 1);
                }
                if (y < (WORLD_H_4 - 1)) {
                    z += tempMap3.get(x, y + 1);
                }
                Byte val = (Byte)(z / 4 + tempMap3.get(x, y)) / 2;
                terrainDensityMap.set(x, y, val);
            }
        }
    }
}


/* comefrom: populationDensityScan */
void Micropolis::doSmooth1()
{
    /* smooths data in tempMap1 into tempMap2  */
    if (donDither & 2) {
        register int x, y = 0, z = 0, dir = 1;

        for (x = 0; x < WORLD_W_2; x++) {
            for (; y != WORLD_H_2 && y != -1; y += dir) {
                z +=
                    tempMap1.get((x == 0) ? x : (x - 1), y) +
                    tempMap1.get((x == (WORLD_W_2 - 1)) ? x : (x + 1), y) +
                    tempMap1.get(x, (y == 0) ? (0) : (y - 1)) +
                    tempMap1.get(x, (y == (WORLD_H_2 - 1)) ? y : (y + 1)) +
                    tempMap1.get(x, y);
                tempMap2.set(x, y, (unsigned char)(((unsigned int)z) >>2));
                z &= 3;
            }
            dir = -dir;
            y += dir;
        }
    } else {
        register short x,y,z;

        for (x = 0; x < WORLD_W_2; x++) {
            for (y = 0; y < WORLD_H_2; y++) {
                z = 0;
                if (x > 0) {
                    z += tempMap1.get(x - 1, y);
                }
                if (x < (WORLD_W_2 - 1)) {
                    z += tempMap1.get(x + 1, y);
                }
                if (y > 0) {
                    z += tempMap1.get(x, y - 1);
                }
                if (y < (WORLD_H_2 - 1)) {
                    z += tempMap1.get(x, y + 1);
                }
                z = (z + tempMap1.get(x, y)) >>2;
                if (z > 255) {
                    z = 255;
                }
                tempMap2.set(x, y, (unsigned char)z);
            }
        }
    }
}


/* comefrom: populationDensityScan */
void Micropolis::doSmooth2()
{
    /* smooths data in tempMap2 into tempMap1  */
    if (donDither & 4) {
        int x, y = 0, z = 0, dir = 1;

        for (x = 0; x < WORLD_W_2; x++) {
            for (; y != WORLD_H_2 && y != -1; y += dir) {
                z +=
                    tempMap2.get((x == 0) ? x : (x - 1), y) +
                    tempMap2.get((x == (WORLD_W_2 - 1)) ? x : (x + 1), y) +
                    tempMap2.get(x, (y == 0) ? (0) : (y - 1)) +
                    tempMap2.get(x, (y == (WORLD_H_2 - 1)) ? y : (y + 1)) +
                    tempMap2.get(x, y);
                Byte val = (Byte)(z / 4);
                tempMap1.set(x, y, val);
                z &= 3;
            }
            dir = -dir;
            y += dir;
        }
    } else {
        short x, y, z;

        for (x = 0; x < WORLD_W_2; x++) {
            for (y = 0; y < WORLD_H_2; y++) {
                z = 0;
                if (x > 0) {
                    z += tempMap2.get(x - 1, y);
                }
                if (x < (WORLD_W_2 - 1)) {
                    z += tempMap2.get(x + 1, y);
                }
                if (y > 0) {
                    z += tempMap2.get(x, y - 1);
                }
                if (y < (WORLD_H_2 - 1)) {
                    z += tempMap2.get(x, y + 1);
                }
                z = (z + tempMap2.get(x, y)) >>2;
                if (z > 255) {
                    z = 255;
                }
                tempMap1.set(x, y, (Byte)z);
            }
        }
    }
}


/* comefrom: fireAnalysis */
void Micropolis::smoothFireStationMap()
{
    short x, y, edge;

    for (x = 0; x < WORLD_W_8; x++) {
        for (y = 0; y < WORLD_H_8; y++) {
            edge = 0;
            if (x > 0) {
                edge += fireStationMap.get(x - 1, y);
            }
            if (x < (WORLD_W_8 - 1)) {
                edge += fireStationMap.get(x + 1, y);
            }
            if (y > 0) {
                edge += fireStationMap.get(x, y - 1);
            }
            if (y < (WORLD_H_8 - 1)) {
                edge += fireStationMap.get(x, y + 1);
            }
            edge = (edge >>2) + fireStationMap.get(x, y);
            tempMap4.set(x, y, edge >>1);
        }
    }

    for (x = 0; x < WORLD_W_8; x++) {
        for (y = 0; y < WORLD_H_8; y++) {
            fireStationMap.set(x, y, tempMap4.get(x, y));
        }
    }
}


/* comefrom: crimeScan */
void Micropolis::smoothPoliceStationMap()
{
    int x, y, edge;

    for (x = 0; x < WORLD_W_8; x++) {
        for (y = 0; y < WORLD_H_8; y++) {
            edge = 0;
            if (x > 0) {
                edge += policeStationMap.get(x - 1, y);
            }
            if (x < (WORLD_W_8 - 1)) {
                edge += policeStationMap.get(x + 1, y);
            }
            if (y> 0) {
                edge += policeStationMap.get(x, y - 1);
            }
            if (y < (WORLD_H_8 - 1)) {
                edge += policeStationMap.get(x, y + 1);
            }
            edge = (edge >>2) + policeStationMap.get(x, y);
            tempMap4.set(x, y, edge >>1);
        }
    }

    for (x = 0; x < WORLD_W_8; x++) {
        for (y = 0; y < WORLD_H_8; y++) {
            policeStationMap.set(x, y, tempMap4.get(x, y));
        }
    }
}


/**
 * Compute distance to city center for the entire map.
 * @see comRateMap
 */
void Micropolis::computeComRateMap()
{
    short x, y, z;

    for (x = 0; x < WORLD_W_8; x++) {
        for (y = 0; y < WORLD_H_8; y++) {
            z = getCityCenterDistance(x * 4,y * 4); // 0..32
            z = z * 4;  // 0..128
            z = 64 - z; // 64..-64
            comRateMap.set(x, y, z);
        }
    }
}


////////////////////////////////////////////////////////////////////////
