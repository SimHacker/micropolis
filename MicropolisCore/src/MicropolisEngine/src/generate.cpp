/* generate.cpp
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

/** @file generate.cpp Terrain generator functions. */

////////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "micropolis.h"
#include "generate.h"


////////////////////////////////////////////////////////////////////////


/**
 * Create a new map for a city.
 * @bug We use a random number generator to draw a seed for initializing the
 *      random number generator?
 */
void Micropolis::generateNewCity()
{
    generateSomeCity(getRandom16());
}


/**
 * Generate a map for a city.
 * @param seed Random number generator initializing seed
 */
void Micropolis::generateSomeCity(int seed)
{
    cityFileName = "";

    generateMap(seed);
    scenario = SC_NONE;
    cityTime = 0;
    initSimLoad = 2;
    doInitialEval = false;

    initWillStuff();
    resetMapState();
    resetEditorState();
    invalidateEditors();
    invalidateMaps();
    updateFunds();
    doSimInit();

    simUpdate();

    callback("UIDidGenerateNewCity", "");
}


/**
 * Generate a map.
 * @param seed Initialization seed for the random generator.
 */
void Micropolis::generateMap(int seed)
{
    short terrainXStart; // Starting X location of the terrain generator.
    short terrainYStart; // Starting Y location of the terrain generator.

    seedRandom(seed);

    if (terrainCreateIsland < 0) {
        if (getRandom(100) < 10) { /* chance that island is generated */
            makeIsland();
            return;
        }
    }

    if (terrainCreateIsland == 1) {
        makeNakedIsland();
    } else {
        clearMap();
    }

    terrainXStart = 40 + getRandom(WORLD_W - 80);
    terrainYStart = 33 + getRandom(WORLD_H - 67);

    terrainMapX = terrainXStart;
    terrainMapY = terrainYStart;


    if (terrainCurveLevel != 0) {
        doRivers(terrainXStart, terrainYStart);
    }

    if (terrainLakeLevel != 0) {
        makeLakes();
    }

    smoothRiver();

    if (terrainTreeLevel != 0) {
        doTrees();
    }

}


/** Clear the whole world to ::DIRT tiles */
void Micropolis::clearMap()
{
    short x, y;

    for (x = 0; x < WORLD_W; x++) {
        for (y = 0; y < WORLD_H; y++) {
            map[x][y] = DIRT;
        }
    }
}


/** Clear everything from all land */
void Micropolis::clearUnnatural()
{
    short x, y;

    for (x = 0; x < WORLD_W; x++) {
        for (y = 0; y < WORLD_H; y++) {
            if (map[x][y] > WOODS) {
                map[x][y] = DIRT;
            }
        }
    }
}

/**
 * Construct a plain island as world, surrounded by 5 tiles of river.
 */
void Micropolis::makeNakedIsland()
{
    const int terrainIslandRadius = ISLAND_RADIUS;
    int x, y;

    for (x = 0; x < WORLD_W; x++) {
        for (y = 0; y < WORLD_H; y++) {
            map[x][y] = RIVER;
        }
    }

    for (x = 5; x < WORLD_W - 5; x++) {
        for (y = 5; y < WORLD_H - 5; y++) {
            map[x][y] = DIRT;
        }
    }

    for (x = 0; x < WORLD_W - 5; x += 2) {
        terrainMapX = x ;
        terrainMapY = getERandom(terrainIslandRadius);
        plopBRiver();
        terrainMapY = (WORLD_H - 10) - getERandom(terrainIslandRadius);
        plopBRiver();
        terrainMapY = 0;
        plopSRiver();
        terrainMapY = (WORLD_H - 6);
        plopSRiver();
    }

    for (y = 0; y < WORLD_H - 5; y += 2) {
        terrainMapY = y ;
        terrainMapX = getERandom(terrainIslandRadius);
        plopBRiver();
        terrainMapX = (WORLD_W - 10) - getERandom(terrainIslandRadius);
        plopBRiver();
        terrainMapX = 0;
        plopSRiver();
        terrainMapX = (WORLD_W - 6);
        plopSRiver();
    }

}


/** Construct a new world as an island */
void Micropolis::makeIsland()
{
    makeNakedIsland();
    smoothRiver();
    doTrees();
}


void Micropolis::makeLakes()
{
    short Lim1, Lim2, t, z;
    short x, y;

    if (terrainLakeLevel < 0) {
        Lim1 = getRandom(10);
    } else {
        Lim1 = terrainLakeLevel / 2;
    }

    for (t = 0; t < Lim1; t++) {
        x = getRandom(WORLD_W - 21) + 10;
        y = getRandom(WORLD_H - 20) + 10;
        Lim2 = getRandom(12) + 2;

        for (z = 0; z < Lim2; z++) {
            terrainMapX = x - 6 + getRandom(12);
            terrainMapY = y - 6 + getRandom(12);

            if (getRandom(4)) {
                plopSRiver();
            } else {
                plopBRiver();
            }
        }
    }
}


/**
 * Move (Micropolis::terrainMapX, Micropolis::terrainMapY) a tile in the
 * indicated direction.
 * @param dir Direction to move in (0..7).
 * @todo Merge with moveMapSim()
 */
void Micropolis::moveMap(short dir)
{
    static short dirTab[2][8] = {
        {  0,  1,  1,  1,  0, -1, -1, -1 },
        { -1, -1,  0,  1,  1,  1,  0, -1 },
    };

    dir = dir & 7;
    terrainMapX += dirTab[0][dir];
    terrainMapY += dirTab[1][dir];
}


/**
 * Splash a bunch of trees down near (\a xloc, \a yloc).
 *
 * Amount of trees is controlled by Micropolis::terrainTreeLevel.
 * @param xloc Horizontal position of starting point for splashing trees.
 * @param yloc Vertical position of starting point for splashing trees.
 * @note Trees are not smoothed.
 * @bug Function generates trees even if Micropolis::terrainTreeLevel is 0.
 */
void Micropolis::treeSplash(short xloc, short yloc)
{
    short dis, dir;
    short z;

    if (terrainTreeLevel < 0) {
        dis = getRandom(150) + 50;
    } else {
        dis = getRandom(100 + (terrainTreeLevel * 2)) + 50;
    }

    terrainMapX = xloc;
    terrainMapY = yloc;

    for (z = 0; z < dis; z++) {
        dir = getRandom(7);
        moveMap(dir);

        if (!(testBounds(terrainMapX, terrainMapY))) {
            return;
        }

        if ((map[terrainMapX][terrainMapY] & LOMASK) == DIRT) {
            map[terrainMapX][terrainMapY] = WOODS | BLBNBIT;
        }

    }
}


/** Splash trees around the world. */
void Micropolis::doTrees()
{
    short Amount, x, xloc, yloc;

    if (terrainTreeLevel < 0) {
        Amount = getRandom(100) + 50;
    } else {
        Amount = terrainTreeLevel + 3;
    }

    for (x = 0; x < Amount; x++) {
        xloc = getRandom(WORLD_W - 1);
        yloc = getRandom(WORLD_H - 1);
        treeSplash(xloc, yloc);
    }

    smoothTrees();
    smoothTrees();
}


void Micropolis::smoothRiver()
{
    static short DX[4] = { -1,  0,  1,  0 };
    static short DY[4] = {  0,  1,  0, -1 };
    static short REdTab[16] = {
        13 | BULLBIT,   13 | BULLBIT,     17 | BULLBIT,     15 | BULLBIT,
        5 | BULLBIT,    2,                19 | BULLBIT,     17 | BULLBIT,
        9 | BULLBIT,    11 | BULLBIT,     2,                13 | BULLBIT,
        7 | BULLBIT,    9 | BULLBIT,      5 | BULLBIT,      2 };

    short bitindex, z, Xtem, Ytem;
    short temp, x, y;

    for (x = 0; x < WORLD_W; x++) {
        for (y = 0; y < WORLD_H; y++) {

            if (map[x][y] == REDGE) {
                bitindex = 0;

                for (z = 0; z < 4; z++) {
                    bitindex = bitindex << 1;
                    Xtem = x + DX[z];
                    Ytem = y + DY[z];
                    if (testBounds(Xtem, Ytem) &&
                        ((map[Xtem][Ytem] & LOMASK) != DIRT) &&
                        (((map[Xtem][Ytem]&LOMASK) < WOODS_LOW) ||
                         ((map[Xtem][Ytem]&LOMASK) > WOODS_HIGH))) {
                        bitindex++;
                    }
                }

                temp = REdTab[bitindex & 15];

                if ((temp != RIVER) &&
                    getRandom(1)) {
                    temp++;
                }

                map[x][y] = temp;
            }
        }
    }
}


bool Micropolis::isTree(MapValue cell)
{
    if ((cell & LOMASK) >= WOODS_LOW && (cell & LOMASK) <= WOODS_HIGH) {
        return true;
    }

    return false;
}


void Micropolis::smoothTrees()
{
    static short DX[4] = { -1,  0,  1,  0 };
    static short DY[4] = {  0,  1,  0, -1 };
    static short TEdTab[16] = {
        0,  0,  0,  34,
        0,  0,  36, 35,
        0,  32, 0,  33,
        30, 31, 29, 37,
    };

    short bitindex, z, Xtem, Ytem;
    short temp, x, y;

    for (x = 0; x < WORLD_W; x++) {
        for (y = 0; y < WORLD_H; y++) {
            if (isTree(map[x][y])) {
                bitindex = 0;
                for (z = 0; z < 4; z++) {
                    bitindex = bitindex << 1;
                    Xtem = x + DX[z];
                    Ytem = y + DY[z];
                    if (testBounds(Xtem, Ytem) &&
                        isTree(map[Xtem][Ytem])) {
                        bitindex++;
                    }
                }
                temp = TEdTab[bitindex & 15];
                if (temp) {
                    if (temp != WOODS) {
                        if ((x + y) & 1) {
                            temp = temp - 8;
                        }
                    }
                    map[x][y] = temp | BLBNBIT;
                } else {
                    map[x][y] = temp;
                }
            }
        }
    }
}


/**
 * Construct rivers.
 * @param terrainXStart Horizontal coordinate of the start position.
 * @param terrainYStart Vertical coordinate of the start position.
 */
void Micropolis::doRivers(short terrainXStart, short terrainYStart)
{
    terrainMapX = terrainXStart;
    terrainMapY = terrainYStart;
    terrainDirLast = getRandom(3);
    terrainDir = terrainDirLast;
    doBRiver();

    terrainMapX = terrainXStart;
    terrainMapY = terrainYStart;
    terrainDirLast = terrainDirLast ^ 4;
    terrainDir = terrainDirLast;
    doBRiver();

    terrainMapX = terrainXStart;
    terrainMapY = terrainYStart;
    terrainDirLast = getRandom(3);
    doSRiver();
}


void Micropolis::doBRiver()
{
    int r1, r2;

    if (terrainCurveLevel < 0) {
        r1 = 100;
        r2 = 200;
    } else {
        r1 = terrainCurveLevel + 10;
        r2 = terrainCurveLevel + 100;
    }

    while (testBounds (terrainMapX + 4, terrainMapY + 4)) {
        plopBRiver();
        if (getRandom(r1) < 10) {
            terrainDir = terrainDirLast;
        } else {
            if (getRandom(r2) > 90) {
                terrainDir++;
            }
            if (getRandom(r2) > 90) {
                terrainDir--;
            }
        }
        moveMap(terrainDir);
    }
}


void Micropolis::doSRiver()
{
    int r1, r2;

    if (terrainCurveLevel < 0) {
        r1 = 100;
        r2 = 200;
    } else {
        r1 = terrainCurveLevel + 10;
        r2 = terrainCurveLevel + 100;
    }

    while (testBounds (terrainMapX + 3, terrainMapY + 3)) {
        plopSRiver();
        if (getRandom(r1) < 10) {
            terrainDir = terrainDirLast;
        } else {
            if (getRandom(r2) > 90) {
                terrainDir++;
            }
            if (getRandom(r2) > 90) {
                terrainDir--;
            }
        }
        moveMap(terrainDir);
    }
}


void Micropolis::putOnMap(MapValue mChar, short xLoc, short yLoc)
{
    if (mChar == 0) {
        return;
    }

    if (!testBounds(xLoc, yLoc)) {
        return;
    }

    MapValue temp = map[xLoc][yLoc];

    if (temp != DIRT) {
        temp = temp & LOMASK;
        if (temp == RIVER) {
            if (mChar != CHANNEL) {
                return;
            }
        }
        if (temp == CHANNEL) {
            return;
        }
    }
    map[xLoc][yLoc] = mChar;
}


void Micropolis::plopBRiver()
{
    short x, y;
    static MapValue BRMatrix[9][9] = {
        { 0, 0, 0, 3, 3, 3, 0, 0, 0 },
        { 0, 0, 3, 2, 2, 2, 3, 0, 0 },
        { 0, 3, 2, 2, 2, 2, 2, 3, 0 },
        { 3, 2, 2, 2, 2, 2, 2, 2, 3 },
        { 3, 2, 2, 2, 4, 2, 2, 2, 3 },
        { 3, 2, 2, 2, 2, 2, 2, 2, 3 },
        { 0, 3, 2, 2, 2, 2, 2, 3, 0 },
        { 0, 0, 3, 2, 2, 2, 3, 0, 0 },
        { 0, 0, 0, 3, 3, 3, 0, 0, 0 },
    };

    for (x = 0; x < 9; x++) {
        for (y = 0; y < 9; y++) {
            putOnMap(BRMatrix[y][x], terrainMapX + x, terrainMapY + y);
        }
    }
}


void Micropolis::plopSRiver()
{
    short x, y;
    static MapValue SRMatrix[6][6] = {
        { 0, 0, 3, 3, 0, 0 },
        { 0, 3, 2, 2, 3, 0 },
        { 3, 2, 2, 2, 2, 3 },
        { 3, 2, 2, 2, 2, 3 },
        { 0, 3, 2, 2, 3, 0 },
        { 0, 0, 3, 3, 0, 0 },
    };

    for (x = 0; x < 6; x++) {
        for (y = 0; y < 6; y++) {
          putOnMap(SRMatrix[y][x], terrainMapX + x, terrainMapY + y);
        }
    }
}


void Micropolis::smoothWater()
{
    int x, y;
    MapTile tile;
    Direction2 dir;

    for (x = 0; x < WORLD_W; x++) {
        for (y = 0; y < WORLD_H; y++) {

            tile = map[x][y] & LOMASK;

            /* If (x, y) is water: */
            if (tile >= WATER_LOW && tile <= WATER_HIGH) {

                const Position pos(x, y);
                for (dir = DIR2_BEGIN; dir < DIR2_END; dir = increment90(dir)) {

                    /* If getting a tile off-map, condition below fails. */
                    tile = getTileFromMap(pos, DIR2_WEST, WATER_LOW);

                    /* If nearest object is not water: */
                    if (tile < WATER_LOW || tile > WATER_HIGH) {
                        map[x][y]=REDGE; /* set river edge */
                        break; // Continue with next tile
                    }
                }
            }
        }
    }

    for (x = 0; x < WORLD_W; x++) {
        for (y = 0; y < WORLD_H; y++) {

            tile = map[x][y] & LOMASK;

            /* If water which is not a channel: */
            if (tile != CHANNEL && tile >= WATER_LOW && tile <= WATER_HIGH) {

                bool makeRiver = true; // make (x, y) a river

                const Position pos(x, y);
                for (dir = DIR2_BEGIN; dir < DIR2_END; dir = increment90(dir)) {

                    /* If getting a tile off-map, condition below fails. */
                    tile = getTileFromMap(pos, DIR2_WEST, WATER_LOW);

                    /* If nearest object is not water: */
                    if (tile < WATER_LOW || tile > WATER_HIGH) {
                        makeRiver = false;
                        break;
                    }
                }

                if (makeRiver) {
                    map[x][y] = RIVER; /* make it a river */
                }
            }
        }
    }

    for (x = 0; x < WORLD_W; x++) {
        for (y = 0; y < WORLD_H; y++) {

            tile = map[x][y] & LOMASK;

            /* If woods: */
            if (tile >= WOODS_LOW && tile <= WOODS_HIGH) {

                const Position pos(x, y);
                for (dir = DIR2_BEGIN; dir < DIR2_END; dir = increment90(dir)) {

                    /* If getting a tile off-map, condition below fails. */
                    tile = getTileFromMap(pos, DIR2_WEST, TILE_INVALID);

                    if (tile == RIVER || tile == CHANNEL) {
                        map[x][y] = REDGE; /* make it water's edge */
                        break;
                    }
                }
            }
        }
    }
}


////////////////////////////////////////////////////////////////////////
