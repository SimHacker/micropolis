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

/** @file generate.cpp */

////////////////////////////////////////////////////////////////////////


#include "stdafx.h"


////////////////////////////////////////////////////////////////////////


void Micropolis::generateNewCity()
{
    generateSomeCity(getRandom16());
}


void Micropolis::generateSomeCity(int r)
{
    cityFileName = "";

    generateMap(r);
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


void Micropolis::generateMap(int r)
{
    seedRandom(r);

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

    getRandStart();

    if (terrainCurveLevel != 0) {
        doRivers();
    }

    if (terrainLakeLevel != 0) {
        makeLakes();
    }

    smoothRiver();

    if (terrainTreeLevel != 0) {
        doTrees();
    }

}


void Micropolis::clearMap()
{
    short x, y;

    for (x = 0; x < WORLD_W; x++) {
        for (y = 0; y < WORLD_H; y++) {
            map[x][y] = DIRT;
        }
    }
}


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


void Micropolis::makeNakedIsland()
{
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


void Micropolis::getRandStart()
{
    terrainXStart = 40 + getRandom(WORLD_W - 80);
    terrainYStart = 33 + getRandom(WORLD_H - 67);

    terrainMapX = terrainXStart;
    terrainMapY = terrainYStart;
}


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


bool Micropolis::isTree(int cell)
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


void Micropolis::doRivers()
{
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


void Micropolis::putOnMap(short Mchar, short Xoff, short Yoff)
{
    short Xloc, Yloc, temp;

    if (Mchar == 0) {
        return;
    }

    Xloc = terrainMapX + Xoff;
    Yloc = terrainMapY + Yoff;

    if (!testBounds(Xloc, Yloc)) {
        return;
    }

    temp = map[Xloc][Yloc];

    if (temp != DIRT) {
        temp = temp & LOMASK;
        if (temp == RIVER) {
            if (Mchar != CHANNEL) {
                return;
            }
        }
        if (temp == CHANNEL) {
            return;
        }
    }
    map[Xloc][Yloc] = Mchar;
}


void Micropolis::plopBRiver()
{
    short x, y;
    static short BRMatrix[9][9] = {
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
            putOnMap(BRMatrix[y][x], x, y);
        }
    }
}


void Micropolis::plopSRiver()
{
    short x, y;
    static short SRMatrix[6][6] = {
        { 0, 0, 3, 3, 0, 0 },
        { 0, 3, 2, 2, 3, 0 },
        { 3, 2, 2, 2, 2, 3 },
        { 3, 2, 2, 2, 2, 3 },
        { 0, 3, 2, 2, 3, 0 },
        { 0, 0, 3, 3, 0, 0 },
    };

    for (x = 0; x < 6; x++) {
        for (y = 0; y < 6; y++) {
          putOnMap(SRMatrix[y][x], x, y);
        }
    }
}


void Micropolis::smoothWater()
{
    int x, y;

    for (x = 0; x < WORLD_W; x++) {
        for (y = 0; y < WORLD_H; y++) {

            /* If water: */
            if (((map[x][y] & LOMASK) >= WATER_LOW) &&
                ((map[x][y] & LOMASK) <= WATER_HIGH)) {

                if (x > 0) {
                    /* If nearest object is not water: */
                    if (((map[x - 1][y] & LOMASK) < WATER_LOW) ||
                        ((map[x - 1][y] & LOMASK) > WATER_HIGH)) {
                        goto edge;
                    }
                }

                if (x < (WORLD_W - 1)) {
                    /* If nearest object is not water: */
                    if (((map[x + 1][y] & LOMASK) < WATER_LOW) ||
                        ((map[x + 1][y] & LOMASK) > WATER_HIGH)) {
                        goto edge;
                    }
                }

                if (y > 0) {
                    /* If nearest object is not water: */
                    if (((map[x][y - 1] & LOMASK) < WATER_LOW) ||
                        ((map[x][y - 1] & LOMASK) > WATER_HIGH)) {
                        goto edge;
                    }
                }

                if (y < (WORLD_H - 1)) {
                    /* If nearest object is not water: */
                    if (((map[x][y + 1] & LOMASK) < WATER_LOW) ||
                        ((map[x][y + 1] & LOMASK) > WATER_HIGH)) {
                    edge:
                        map[x][y]=REDGE; /* set river edge */
                        continue;
                    }
                }
            }
        }
    }

    for (x = 0; x < WORLD_W; x++) {
        for (y = 0; y < WORLD_H; y++) {

            /* If water which is not a channel: */
            if (((map[x][y] & LOMASK) != CHANNEL) &&
                ((map[x][y] & LOMASK) >= WATER_LOW) &&
                ((map[x][y] & LOMASK) <= WATER_HIGH)) {

                if (x > 0) {
                    /* If nearest object is not water; */
                    if (((map[x - 1][y] & LOMASK) < WATER_LOW) ||
                        ((map[x - 1][y] & LOMASK) > WATER_HIGH)) {
                        continue;
                    }
                }

                if (x < (WORLD_W - 1)) {
                    /* If nearest object is not water: */
                    if (((map[x + 1][y] & LOMASK) < WATER_LOW) ||
                        ((map[x + 1][y] & LOMASK) > WATER_HIGH)) {
                        continue;
                    }
                }

                if (y > 0) {
                    /* If nearest object is not water: */
                    if (((map[x][y - 1] & LOMASK) < WATER_LOW) ||
                        ((map[x][y - 1] & LOMASK) > WATER_HIGH)) {
                        continue;
                    }
                }

                if (y < (WORLD_H - 1)) {
                    /* If nearest object is not water: */
                    if (((map[x][y + 1] & LOMASK) < WATER_LOW) ||
                        ((map[x][y + 1] & LOMASK) > WATER_HIGH)) {
                        continue;
                    }
                }

                map[x][y] = RIVER; /* make it a river */
            }
        }
    }

    for (x = 0; x < WORLD_W; x++) {
        for (y = 0; y < WORLD_H; y++) {

            /* If woods: */
            if (((map[x][y] & LOMASK) >= WOODS_LOW) &&
                ((map[x][y] & LOMASK) <= WOODS_HIGH)) {

                if (x > 0) {
                    /* If nearest object is water: */
                    if ((map[x - 1][y] == RIVER) ||
                        (map[x - 1][y] == CHANNEL)) {
                        map[x][y] = REDGE; /* make it water's edge */
                        continue;
                    }
                }

                if (x < (WORLD_W - 1)) {
                    /* If nearest object is water: */
                    if ((map[x + 1][y] == RIVER) ||
                        (map[x + 1][y] == CHANNEL)) {
                        map[x][y] = REDGE; /* make it water's edge */
                        continue;
                    }
                }

                if (y > 0) {
                    /* If nearest object is water: */
                    if ((map[x][y - 1] == RIVER) ||
                        (map[x][y - 1] == CHANNEL)) {
                        map[x][y] = REDGE; /* make it water's edge */
                        continue;
                    }
                }

                if (y < (WORLD_H - 1)) {
                    /* If nearest object is water; */
                    if ((map[x][y + 1] == RIVER) ||
                        (map[x][y + 1] == CHANNEL)) {
                        map[x][y] = REDGE; /* make it water's edge */
                        continue;
                    }
                }
            }
        }
    }
}


////////////////////////////////////////////////////////////////////////
