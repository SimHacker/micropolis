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
    seedRandom(seed);

    // Construct land.
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


    // Lay a river.
    if (terrainCurveLevel != 0) {
        int terrainXStart = 40 + getRandom(WORLD_W - 80);
        int terrainYStart = 33 + getRandom(WORLD_H - 67);

        Position terrainPos(terrainXStart, terrainYStart);

        doRivers(terrainPos);
    }

    // Lay a few lakes.
    if (terrainLakeLevel != 0) {
        makeLakes();
    }

    smoothRiver();

    // And add trees.
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

        int mapY = getERandom(terrainIslandRadius);
        plopBRiver(Position(x, mapY));

        mapY = (WORLD_H - 10) - getERandom(terrainIslandRadius);
        plopBRiver(Position(x, mapY));

        plopSRiver(Position(x, 0));
        plopSRiver(Position(x, WORLD_H - 6));
    }

    for (y = 0; y < WORLD_H - 5; y += 2) {

        int mapX = getERandom(terrainIslandRadius);
        plopBRiver(Position(mapX, y));

        mapX = (WORLD_W - 10) - getERandom(terrainIslandRadius);
        plopBRiver(Position(mapX, y));

        plopSRiver(Position(0, y));
        plopSRiver(Position(WORLD_W - 6, y));
    }

}


/** Construct a new world as an island */
void Micropolis::makeIsland()
{
    makeNakedIsland();
    smoothRiver();
    doTrees();
}


/**
 * Make a number of lakes, depending on the Micropolis::terrainLakeLevel.
 */
void Micropolis::makeLakes()
{
    short numLakes;

    if (terrainLakeLevel < 0) {
        numLakes = getRandom(10);
    } else {
        numLakes = terrainLakeLevel / 2;
    }

    while (numLakes > 0) {
        int x = getRandom(WORLD_W - 21) + 10;
        int y = getRandom(WORLD_H - 20) + 10;

        makeSingleLake(Position(x, y));

        numLakes--;
    }
}

/**
 * Make a random lake at \a pos.
 * @param pos Rough position of the lake.
 */
void Micropolis::makeSingleLake(const Position &pos)
{
    int numPlops = getRandom(12) + 2;

    while (numPlops > 0) {
        Position plopPos(pos, getRandom(12) - 6, getRandom(12) - 6);

        if (getRandom(4)) {
            plopSRiver(plopPos);
        } else {
            plopBRiver(plopPos);
        }

        numPlops--;
    }
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
    short numTrees;

    if (terrainTreeLevel < 0) {
        numTrees = getRandom(150) + 50;
    } else {
        numTrees = getRandom(100 + (terrainTreeLevel * 2)) + 50;
    }

    Position treePos(xloc, yloc);

    while (numTrees > 0) {
        Direction2 dir = (Direction2)(DIR2_NORTH + getRandom(7));
        treePos.move(dir);

        if (!treePos.testBounds()) {
            return;
        }

        if ((map[treePos.posX][treePos.posY] & LOMASK) == DIRT) {
            map[treePos.posX][treePos.posY] = WOODS | BLBNBIT;
        }

        numTrees--;
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
                    if (testBounds(Xtem, Ytem) && isTree(map[Xtem][Ytem])) {
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
 * @param terrainPos Coordinate to start making a river.
 */
void Micropolis::doRivers(const Position &terrainPos)
{
    Direction2 riverDir;   // Global direction of the river
    Direction2 terrainDir; // Local direction of the river

    riverDir = (Direction2)(DIR2_NORTH + getRandom(3));
    doBRiver(terrainPos, riverDir, riverDir);

    riverDir = rotate180(riverDir);
    terrainDir = doBRiver(terrainPos, riverDir, riverDir);

    riverDir = (Direction2)(DIR2_NORTH + getRandom(3));
    doSRiver(terrainPos, riverDir, terrainDir);
}


/**
 * Make a big river.
 * @param pos        Start position of making a river.
 * @param riverDir   Global direction of the river.
 * @param terrainDir Local direction of the terrain.
 * @return Last used local terrain direction.
 */
Direction2 Micropolis::doBRiver(const Position &riverPos,
                                Direction2 riverDir, Direction2 terrainDir)
{
    int rate1, rate2;

    if (terrainCurveLevel < 0) {
        rate1 = 100;
        rate2 = 200;
    } else {
        rate1 = terrainCurveLevel + 10;
        rate2 = terrainCurveLevel + 100;
    }

    Position pos(riverPos);

    while (testBounds(pos.posX + 4, pos.posY + 4)) {
        plopBRiver(pos);
        if (getRandom(rate1) < 10) {
            terrainDir = riverDir;
        } else {
            if (getRandom(rate2) > 90) {
                terrainDir = rotate45(terrainDir);
            }
            if (getRandom(rate2) > 90) {
                terrainDir = rotate45(terrainDir, 7);
            }
        }
        pos.move(terrainDir);
    }

    return terrainDir;
}

/**
 * Make a small river.
 * @param pos        Start position of making a river.
 * @param riverDir   Global direction of the river.
 * @param terrainDir Local direction of the terrain.
 * @return Last used local terrain direction.
 */
Direction2 Micropolis::doSRiver(const Position &riverPos,
                                Direction2 riverDir, Direction2 terrainDir)
{
    int rate1, rate2;

    if (terrainCurveLevel < 0) {
        rate1 = 100;
        rate2 = 200;
    } else {
        rate1 = terrainCurveLevel + 10;
        rate2 = terrainCurveLevel + 100;
    }

    Position pos(riverPos);

    while (testBounds(pos.posX + 3, pos.posY + 3)) {
        plopSRiver(pos);
        if (getRandom(rate1) < 10) {
            terrainDir = riverDir;
        } else {
            if (getRandom(rate2) > 90) {
                terrainDir = rotate45(terrainDir);
            }
            if (getRandom(rate2) > 90) {
                terrainDir = rotate45(terrainDir, 7);
            }
        }
        pos.move(terrainDir);
    }

    return terrainDir;
}


/**
 * Put \a mChar onto the map at position \a xLoc, \a yLoc if possible.
 * @param mChar Map value to put ont the map.
 * @param xLoc  Horizontal position at the map to put \a mChar.
 * @param yLoc  Vertical position at the map to put \a mChar.
 */
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

/**
 * Put down a big river diamond-like shape.
 * @param pos Base coordinate of the blob (top-left position).
 */
void Micropolis::plopBRiver(const Position &pos)
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
            putOnMap(BRMatrix[y][x], pos.posX + x, pos.posY + y);
        }
    }
}


/**
 * Put down a small river diamond-like shape.
 * @param pos Base coordinate of the blob (top-left position).
 */
void Micropolis::plopSRiver(const Position &pos)
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
          putOnMap(SRMatrix[y][x], pos.posX + x, pos.posY + y);
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
