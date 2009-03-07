/* tool.cpp
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

/** @file tool.cpp */

////////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "micropolis.h"
#include "text.h"


////////////////////////////////////////////////////////////////////////


static const short gCostOf[] = {
     100,    100,    100,    500, /* res, com, ind, fire */
     500,      0,      5,      1, /* police, query, wire, bulldozer */
      20,     10,   5000,     10, /* rail, road, stadium, park */
    3000,   3000,   5000,  10000, /* seaport, coal, nuclear, airport */
     100,      0,      0,      0, /* network, water, land, forest */
       0,
};


static const short gToolSize[] = {
    3, 3, 3, 3,
    3, 1, 1, 1,
    1, 1, 4, 1,
    4, 4, 4, 6,
    1, 1, 1, 1,
    0,
};


////////////////////////////////////////////////////////////////////////
// Utilities


/**
 * Put a park down at the give tile.
 * @param mapH X coordinate of the tile.
 * @param mapV Y coordinate of the tile.
 * @return Build result (-2 = no money, -1 = needs bulldoze, 1 = built).
 * @todo Add auto-bulldoze? (seems to be missing).
 */
int Micropolis::putDownPark(short mapH, short mapV)
{
    short value, tile;

    if (totalFunds - gCostOf[TOOL_PARK] < 0) return -2;

    value = getRandom(4);

    if (value == 4) {
        tile = FOUNTAIN | BURNBIT | BULLBIT | ANIMBIT;
    } else {
        tile = (value + WOODS2) | BURNBIT | BULLBIT;
    }

    if (map[mapH][mapV] != DIRT) return -1;

    map[mapH][mapV] = tile;

    spend(gCostOf[TOOL_PARK]);
    updateFunds();

    return 1;
}


/**
 * Put down a communication network.
 * @param mapH X coordinate of the tile.
 * @param mapV Y coordinate of the tile.
 * @return Build result (-2 = no money, -1 = needs bulldoze, 1 = built).
 * @todo Auto-bulldoze deducts always 1.
 * @todo Auto-bulldoze costs should be pulled from a table/constant.
 */
int Micropolis::putDownNetwork(short mapH, short mapV)
{
    int tile = map[mapH][mapV] & LOMASK;

    if (totalFunds > 0 && tally(tile)) {
        map[mapH][mapV] = DIRT;
        tile = DIRT;
        spend(1);
    }

    if (tile != DIRT) return -1;

    if (totalFunds - gCostOf[TOOL_NETWORK] < 0) return -2;

    map[mapH][mapV] = TELEBASE | CONDBIT | BURNBIT | BULLBIT | ANIMBIT;

    spend(gCostOf[TOOL_NETWORK]);
    updateFunds();

    return 1;
}


/**
 * Put down a water tile.
 * @param mapH X coordinate of the tile.
 * @param mapV Y coordinate of the tile.
 * @return Build result (-2 = no money, -1 = needs bulldoze, 1 = built).
 * @todo Auto-bulldoze deducts always 1.
 * @todo Auto-bulldoze costs should be pulled from a table/constant.
 */
int Micropolis::putDownWater(short mapH, short mapV)
{
    int tile = map[mapH][mapV] & LOMASK;

    if (tile == RIVER) return 0;

    if (totalFunds - gCostOf[TOOL_WATER] < 0) return -2;

    map[mapH][mapV] = RIVER;

    spend(gCostOf[TOOL_WATER]);
    updateFunds();

    return 1;
}


/**
 * Put down a land tile.
 * @param mapH X coordinate of the tile.
 * @param mapV Y coordinate of the tile.
 * @return Build result (-2 = no money, -1 = needs bulldoze, 1 = built).
 * @todo Auto-bulldoze deducts always 1.
 * @todo Auto-bulldoze costs should be pulled from a table/constant.
 */
int Micropolis::putDownLand(short mapH, short mapV)
{
    int tile = map[mapH][mapV] & LOMASK;
    static short dx[8] = { -1,  0,  1, -1, 1, -1,  0,  1, };
    static short dy[8] = { -1, -1, -1,  0, 0,  1,  1,  1, };


    if (tile == DIRT) return 0;

    if (totalFunds - gCostOf[TOOL_LAND] < 0) return -2;

    map[mapH][mapV] = DIRT;

    int i;
    for (i = 0; i < 8; i++) {
	int xx = mapH + dx[i];
	int yy = mapV + dy[i];
	if (testBounds(xx, yy)) {
	    //smoothTreesAt(xx, yy, true);
	}
    }

    spend(gCostOf[TOOL_LAND]);
    updateFunds();

    return 1;
}


/**
 * Put down a forest tile.
 * @param mapH X coordinate of the tile.
 * @param mapV Y coordinate of the tile.
 * @return Build result (-2 = no money, -1 = needs bulldoze, 1 = built).
 * @todo Auto-bulldoze deducts always 1.
 * @todo Auto-bulldoze costs should be pulled from a table/constant.
 */
int Micropolis::putDownForest(short mapH, short mapV)
{
    static short dx[8] = { -1,  0,  1, -1, 1, -1,  0,  1, };
    static short dy[8] = { -1, -1, -1,  0, 0,  1,  1,  1, };

    if (totalFunds - gCostOf[TOOL_FOREST] < 0) return -2;

    map[mapH][mapV] = WOODS | BLBNBIT;

    int i;
    for (i = 0; i < 8; i++) {
	int xx = mapH + dx[i];
	int yy = mapV + dy[i];
	if (testBounds(xx, yy)) {
	    smoothTreesAt(xx, yy, true);
	}
    }

    spend(gCostOf[TOOL_FOREST]);
    updateFunds();

    return 1;
}


/**
 * Compute where the 'center' (at (1,1)) of the zone is, depending on where the
 * user clicked.
 * @param id Tile character value of the tile that the user clicked on.
 * @param deltaHPtr Pointer where horizontal position correction is written to.
 * @param deltaVPtr Pointer where vertical position correction is written to.
 * @return Size of the zone clicked at (or \c 0 if clicked outside zone).
 * @todo Make this table driven.
 */
short Micropolis::checkBigZone(short id, short *deltaHPtr, short *deltaVPtr)
{
    switch (id) {

    case POWERPLANT:      /* check coal plant */
    case PORT:            /* check sea port */
    case NUCLEAR:         /* check nuc plant */
    case STADIUM:         /* check stadium */
        *deltaHPtr = 0;
        *deltaVPtr = 0;
        return 4;

    case POWERPLANT + 1:  /* check coal plant */
    case COALSMOKE3:      /* check coal plant, smoke */
    case COALSMOKE3 + 1:  /* check coal plant, smoke */
    case COALSMOKE3 + 2:  /* check coal plant, smoke */
    case PORT + 1:        /* check sea port */
    case NUCLEAR + 1:     /* check nuc plant */
    case STADIUM + 1:     /* check stadium */
        *deltaHPtr = -1;
        *deltaVPtr = 0;
        return 4;

    case POWERPLANT + 4:  /* check coal plant */
    case PORT + 4:        /* check sea port */
    case NUCLEAR + 4:     /* check nuc plant */
    case STADIUM + 4:     /* check stadium */
        *deltaHPtr = 0;
        *deltaVPtr = -1;
        return 4;

    case POWERPLANT + 5:  /* check coal plant */
    case PORT + 5:        /* check sea port */
    case NUCLEAR + 5:     /* check nuc plant */
    case STADIUM + 5:     /* check stadium */
        *deltaHPtr = -1;
        *deltaVPtr = -1;
        return 4;

    case AIRPORT:         /* check airport */
        *deltaHPtr = 0;
        *deltaVPtr = 0;
        return 6;

    case AIRPORT + 1:
        *deltaHPtr = -1;
        *deltaVPtr = 0;
        return 6;

    case AIRPORT + 2:
        *deltaHPtr = -2;
        *deltaVPtr = 0;
        return 6;

    case AIRPORT + 3:
        *deltaHPtr = -3;
        *deltaVPtr = 0;
        return 6;

    case AIRPORT + 6:
        *deltaHPtr = 0;
        *deltaVPtr = -1;
        return 6;

    case AIRPORT + 7:
        *deltaHPtr = -1;
        *deltaVPtr = -1;
        return 6;

    case AIRPORT + 8:
        *deltaHPtr = -2;
        *deltaVPtr = -1;
        return 6;

    case AIRPORT + 9:
        *deltaHPtr = -3;
        *deltaVPtr = -1;
        return 6;

    case AIRPORT + 12:
        *deltaHPtr = 0;
        *deltaVPtr = -2;
        return 6;

    case AIRPORT + 13:
        *deltaHPtr = -1;
        *deltaVPtr = -2;
        return 6;

    case AIRPORT + 14:
        *deltaHPtr = -2;
        *deltaVPtr = -2;
        return 6;

    case AIRPORT + 15:
        *deltaHPtr = -3;
        *deltaVPtr = -2;
        return 6;

    case AIRPORT + 18:
        *deltaHPtr = 0;
        *deltaVPtr = -3;
        return 6;

    case AIRPORT + 19:
        *deltaHPtr = -1;
        *deltaVPtr = -3;
        return 6;

    case AIRPORT + 20:
        *deltaHPtr = -2;
        *deltaVPtr = -3;
        return 6;

    case AIRPORT + 21:
        *deltaHPtr = -3;
        *deltaVPtr = -3;
        return 6;

    default:
        *deltaHPtr = 0;
        *deltaVPtr = 0;
        return 0;

    }
}


/**
 * Can the tile be auto-bulldozed?.
 * @param tileValue Value of the tile.
 * @return \c True if the tile can be auto-bulldozed, else \c false.
 */
bool Micropolis::tally(short tileValue)
{
    return (tileValue >= FIRSTRIVEDGE  && tileValue <= LASTRUBBLE) ||
           (tileValue >= POWERBASE + 2 && tileValue <= POWERBASE + 12) ||
           (tileValue >= TINYEXP       && tileValue <= LASTTINYEXP + 2);
}


/**
 * Return the size of the zone that the tile belongs to.
 * @param tileValue Value of the tile in the zone.
 * @return Size of the zone if it is a known tile value, else \c 0.
 */
short Micropolis::checkSize(short tileValue)
{
    // check for the normal com, resl, ind 3x3 zones & the fireDept & PoliceDept
    if ((tileValue >= RESBASE - 1        && tileValue <= PORTBASE - 1) ||
        (tileValue >= LASTPOWERPLANT + 1 && tileValue <= POLICESTATION + 4)) {
        return 3;
    }

    if ((tileValue >= PORTBASE    && tileValue <= LASTPORT) ||
        (tileValue >= COALBASE    && tileValue <= LASTPOWERPLANT) ||
        (tileValue >= STADIUMBASE && tileValue <= LASTZONE)) {
        return 4;
    }

    return 0;
}


/**
 * Check and connect a new zone around the border.
 * @param xMap  X coordinate of top-left tile.
 * @param yMap  Y coordinate of top-left tile.
 * @param sizeX Horizontal size of the new zone.
 * @param sizeY Vertical size of the new zone.
 */
void Micropolis::checkBorder(short xMap, short yMap, int sizeX, int sizeY)
{
    short cnt;

    /* this will do the upper bordering row */
    for (cnt = 0; cnt < sizeX; cnt++) {
        connectTile(xMap + cnt, yMap - 1, CONNECT_TILE_FIX);
    }

    /* this will do the left bordering row */
    for (cnt = 0; cnt < sizeY; cnt++) {
        connectTile(xMap - 1, yMap + cnt, CONNECT_TILE_FIX);
    }

    /* this will do the bottom bordering row */
    for (cnt = 0; cnt < sizeX; cnt++) {
        connectTile(xMap + cnt, yMap + sizeY, CONNECT_TILE_FIX);
    }

    /* this will do the right bordering row */
    for (cnt = 0; cnt < sizeY; cnt++) {
        connectTile(xMap + sizeX, yMap + cnt, CONNECT_TILE_FIX);
    }
}

/**
 * Put down a building, starting at (\a leftX, \a topY) with size
 * (\a sizeX, \a sizeY).
 * @param leftX    Position of left column of tiles of the building.
 * @param topY     Position of top row of tiles of the building.
 * @param sizeX    Horizontal size of the building.
 * @param sizeY    Vertical size of the building.
 * @param baseTile Tile value to use at the top-left position. Tiles are laid
 *                 in column major mode.
 * @param aniFlag  Set animation flag at relative position (1, 2)
 *
 * @pre All tiles are within world boundaries.
 *
 * @todo We should ask the buildings themselves how they should be drawn.
 */
void Micropolis::putBuilding(int leftX, int topY, int sizeX, int sizeY,
                            unsigned short baseTile, bool aniFlag)
{
    for (int dy = 0; dy < sizeY; dy++) {
        int posY = topY + dy;

        for (int dx = 0; dx < sizeX; dx++) {
            int posX = leftX + dx;

            unsigned short tileValue = baseTile | BNCNBIT;
            if (dx == 1) {
                if (dy == 1) {
                    tileValue = baseTile | BNCNBIT | ZONEBIT;
                } else if (dy == 2 && aniFlag) {
                    tileValue = baseTile | BNCNBIT | ANIMBIT;
                }
            }

            map[posX][posY] = tileValue;

            baseTile++;
        }
    }
}

/**
 * Check the site where a building is about to be put down.
 * @param leftX    Position of left column of tiles of the building.
 * @param topY     Position of top row of tiles of the building.
 * @param sizeX    Horizontal size of the building.
 * @param sizeY    Vertical size of the building.
 * @return: Suitability of the site.
 *          <0: not-buildable,
 *          >=0: number of tiles to bulldoze before construction can start.
 *
 * @note With Micropolis::autoBulldoze off, the function never returns
 *       a non-zero number of bulldozable tiles.
 */
int Micropolis::checkBuildingSite(int leftX, int topY, int sizeX, int sizeY)
{
    // Check that the entire site is on the map
    if (leftX < 0 || leftX + sizeX > WORLD_W) {
        return -1;
    }
    if (topY < 0 || topY + sizeY > WORLD_H) {
        return -1;
    }

    // Check whether the tiles are clear
    int numToDoze = 0; // Number of tiles that need bull-dozing
    for (int dy = 0; dy < sizeY; dy++) {
        int posY = topY + dy;

        for (int dx = 0; dx < sizeX; dx++) {
            int posX = leftX + dx;

            unsigned short tileValue = map[posX][posY] & LOMASK;

            if (tileValue == DIRT) { // DIRT tile is buidable
                continue;
            }

            if (!autoBulldoze) {
                return -1; // No DIRT and no bull-dozer => not buildable
            }
            if (!tally(tileValue)) {
                return -1; // tilevalue cannot be auto-bulldozed
            }

            numToDoze++;
        }
    }

    return numToDoze;
}


/**
 * Build a building.
 * @param mapH    Horizontal position of the 'center' tile in the world.
 * @param mapV    Vertical position of the 'center' tile in the world.
 * @param sizeX   Horizontal size of the building.
 * @param sizeY   Vertical size of the building.
 * @param base    Tile number for the top-left position.
 * @param tool    Identification of the tool used.
 * @param aniFlag Set animation flag at relative position (1, 2)
 * @return Build result. -2=no money, -1=cannot build here, 1=ok
 */
int Micropolis::buildBuilding(int mapH, int mapV,
                                int sizeX, int sizeY,
                                unsigned short base,
                                short tool, bool aniFlag)
{
    mapH--; mapV--; // Move position to top-left

    int result = checkBuildingSite(mapH, mapV, sizeX, sizeY);
    if (result < 0) {
        return -1;
    }
    assert(result == 0 || autoBulldoze);

    int cost = result + gCostOf[tool];
    /// @todo Multiply survey result with bulldoze cost
    ///       (or better, ask bulldoze tool).

    if (totalFunds - cost < 0) return -2;

    /* take care of the money situtation here */
    spend(cost);
    updateFunds();

    putBuilding(mapH, mapV, sizeX, sizeY, base, aniFlag);

    checkBorder(mapH, mapV, sizeX, sizeY);

    return 1;
}


/* Query */

/* search table for zone status string match */
static short idArray[29] = {
    DIRT, RIVER, TREEBASE, RUBBLE,
    FLOOD, RADTILE, FIRE, ROADBASE,
    POWERBASE, RAILBASE, RESBASE, COMBASE,
    INDBASE, PORTBASE, AIRPORTBASE, COALBASE,
    FIRESTBASE, POLICESTBASE, STADIUMBASE, NUCLEARBASE,
    // FIXME: I think HBRDG_END should be HBRDG0...?
    HBRDG0, RADAR0, FOUNTAIN, INDBASE2,
    // FIXME: What are tiles 952 and 956?
    FOOTBALLGAME1, VBRDG0, 952, 956,
    9999, // a huge short
};

/*
    0, 2, 21, 44,
    48, 52, 53, 64,
    208, 224, 240, 423,
    612, 693, 709, 745,
    761, 770, 779, 811,
    827, 832, 840, 844,
    932, 948, 952, 956

    Clear, Water, Trees, Rubble,
    Flood, Radioactive Waste, Fire, Road,
    Power, Rail, Residential, Commercial,
    Industrial, Port, AirPort, Coal Power,
    Fire Department, Police Department, Stadium, Nuclear Power,
    Draw Bridge, Radar Dish, Fountain, Industrial,
    49er's 38  Bears 3, Draw Bridge, Ur 238, Unknown
*/


/**
 * Get string index for a status report on tile \a mapH, \a mapV on a
 * given status category.
 * @param catNo Category number:
 *  0: population density
 *  1: land value.
 *  2: crime rate.
 *  3: pollution.
 *  4: growth rate.
 * @param mapH  X coordinate of the tile.
 * @param mapV  Y coordinate of the tile.
 * @return Index into stri.202 file.
 */
int Micropolis::getDensityStr(short catNo, short mapH, short mapV)
{
    int z;

    switch (catNo) {

    case 0:
    default:
        z = populationDensityMap.worldGet(mapH, mapV);
        z = z >> 6;
        z = z & 3;
        return z + STR202_POPULATIONDENSITY_LOW;

    case 1:
        z = landValueMap.worldGet(mapH, mapV);
        if (z < 30) return STR202_LANDVALUE_SLUM;
        if (z < 80) return STR202_LANDVALUE_LOWER_CLASS;
        if (z < 150) return STR202_LANDVALUE_MIDDLE_CLASS;
        return STR202_LANDVALUE_HIGH_CLASS;

    case 2:
        z = crimeRateMap.worldGet(mapH, mapV);
        z = z >> 6;
        z = z & 3;
        return z + STR202_CRIME_NONE;

    case 3:
        z = pollutionDensityMap.worldGet(mapH, mapV);
        if (z < 64 && z > 0) return 13;
        z = z >> 6;
        z = z & 3;
        return z + STR202_POLLUTION_NONE;

    case 4:
        z = rateOfGrowthMap.worldGet(mapH, mapV);
        if (z < 0) return STR202_GROWRATE_DECLINING;
        if (z == 0) return STR202_GROWRATE_STABLE;
        if (z > 100) return STR202_GROWRATE_FASTGROWTH;
        return STR202_GROWRATE_SLOWGROWTH;

    }
}


/**
 * Report about the status of a tile.
 * @param mapH X coordinate of the tile.
 * @param mapV Y coordinate of the tile.
 * @bug Program breaks for status on 'dirt'
 */
void Micropolis::doZoneStatus(short mapH, short mapV)
{
    char localStr[256]; // Textual version of the category the tile belongs to
    char statusStr[5][256];

    short tileNum = map[mapH][mapV] & LOMASK;

    if (tileNum >= COALSMOKE1 && tileNum < FOOTBALLGAME1) {
      tileNum = COALBASE;
    }

    // Find the category where the tile belongs to
    // Note: If 'tileNum < idArray[i]', it belongs to category i-1
    short i;
    for (i = 1; i < 29; i++) {
        if (tileNum < idArray[i]) {
            break;
        }
    }

    i--;
    // i contains the category that the tile belongs to (in theory 0..27).
    // However, it is 0..26, since 956 is the first unused tile

    // Code below looks buggy, 0 is a valid value (namely 'dirt'), and upper
    // limit is not correctly checked either ('stri.219' has only 27 lines).

    // FIXME: This is strange... Normalize to zero based index.
    if (i < 1 || i > 28) {
      i = 28;  // This breaks the program (when you click 'dirt')
    }

    // Obtain the string of the tile category.
    // 'stri.219' has only 27 lines, so 0 <= i <= 26 is acceptable.
    getIndString(localStr, 219, i + 1);

    for (i = 0; i < 5; i++) {
        short id = clamp(getDensityStr(i, mapH, mapV) + 1, 1, 20);
        getIndString(statusStr[i], 202, id);
    }

    doShowZoneStatus(
        localStr, 
        statusStr[0], statusStr[1], statusStr[2],
        statusStr[3], statusStr[4], 
        mapH, mapV);
}


/** Tell front-end to report on status of a tile.
 * @param str Category of the tile.
 * @param s0  Population density text.
 * @param s1  Land value text.
 * @param s2  Crime rate text.
 * @param s3  Pollution text.
 * @param s4  Grow rate text.
 * @param x   X coordinate of the tile.
 * @param y   Y coordinate of the tile.
 */
void Micropolis::doShowZoneStatus(
    char *str,
    char *s0, char *s1, char *s2, char *s3, char *s4,
    int x, int y)
{
    callback("UIShowZoneStatus", "ssssssdd", str, s0, s1, s2, s3, s4, x, y);
}


/**
 * Make a \a size by \a size tiles square of rubble
 * @param x    Horizontal position of the left-most tile
 * @param y    Vertical position of the left-most tile
 * @param size Size of the rubble square
 */
void Micropolis::putRubble(int x, int y, int size)
{
    for (int xx = x; xx < x + size; xx++) {
        for (int yy = y; yy < y + size; yy++)  {

            if (testBounds(xx, yy)) {
                int tile = map[xx][yy] & LOMASK;

                if (tile != RADTILE && tile != DIRT) {
                    tile = (doAnimation ? (TINYEXP + getRandom(2)) : SOMETINYEXP);
                    map[xx][yy] = tile | ANIMBIT | BULLBIT;
                }
            }
        }
    }
}


/**
 * Report to the front-end that a tool was used.
 * @param name: Name of the tool.
 * @param x     X coordinate of where the tool was applied.
 * @param y     Y coordinate of where the tool was applied.
 */
void Micropolis::didTool(const char *name, short x, short y)
{
    callback("UIDidTool", "sdd", name, (int)x, (int)y);
}


////////////////////////////////////////////////////////////////////////
// Tools


/**
 * Do query tool.
 * @param x X coordinate of the position of the tool application.
 * @param y Y coordinate of the position of the tool application.
 * @return (-1 = outside map, 1=ok)
 */
int Micropolis::queryTool(short x, short y)
{
    if (!testBounds(x, y)) {
        return -1;
    }

    doZoneStatus(x, y);
    didTool("Qry", x, y);

    return 1;
}


/** @bug case 6 is never returned from checkSize() */
int Micropolis::bulldozerTool(short x, short y)
{
    unsigned short currTile, temp;
    short zoneSize, deltaH, deltaV;
    int result = 1;

    if (!testBounds(x, y)) {
        return -1;
    }

    currTile = map[x][y];
    temp = currTile & LOMASK;

    if (currTile & ZONEBIT) { /* zone center bit is set */

        if (totalFunds > 0) {

            spend(1);

            switch (checkSize(temp)) {

            case 3:
                makeSound("city", "Explosion-High", x, y);
                putRubble(x - 1, y - 1, 3);
                break;

            case 4:
                makeSound("city", "Explosion-Low", x, y);
                putRubble(x - 1, y - 1, 4);
                break;

            case 6:
                makeSound("city", "Explosion-High", x, y);
                makeSound("city", "Explosion-Low", x, y);
                putRubble(x - 1, y - 1, 6);
                break;

            default:
                break;

            }

        }

    } else if ((zoneSize = checkBigZone(temp, &deltaH, &deltaV))) {

        if (totalFunds > 0) {

            spend(1);

            switch (zoneSize) {

            case 3:
                makeSound("city", "Explosion-High", x, y);
                break;

            case 4:
                makeSound("city", "Explosion-Low", x, y);
                putRubble(x + deltaH - 1, y + deltaV - 1, 4);
                break;

            case 6:
                makeSound("city", "Explosion-High", x, y);
                makeSound("city", "Explosion-Low", x, y);
                putRubble(x + deltaH - 1, y + deltaV - 1, 6);
                break;

            }

        }

    } else {

        if (temp == RIVER || temp == REDGE || temp == CHANNEL) {

            if (totalFunds >= 6) {

                result = connectTile(x, y, CONNECT_TILE_BULLDOZE);

                if (temp != (map[x][y] & LOMASK)) {
                  spend(5);
                }

            } else {
                result = 0;
            }
        } else {
            result = connectTile(x, y, CONNECT_TILE_BULLDOZE);
        }

    }

    updateFunds();

    if (result == 1) {
        didTool("Dozr", x, y);
    }

    return result;
}


int Micropolis::roadTool(short x, short y)
{
    int result;

    if (!testBounds(x, y)) {
        return -1;
    }

    result = connectTile(x, y, CONNECT_TILE_ROAD);
    updateFunds();

    if (result == 1) {
        didTool("Road", x, y);
    }

    return result;
}


int Micropolis::railroadTool(short x, short y)
{
    int result;

    if (!testBounds(x, y)) {
        return -1;
    }

    result = connectTile(x, y, CONNECT_TILE_RAILROAD);
    updateFunds();

    if (result == 1) {
        didTool("Rail", x, y);
    }

    return result;
}


int Micropolis::wireTool(short x, short y)
{
    int result;

    if (!testBounds(x, y)) {
        return -1;
    }

    result = connectTile(x, y, CONNECT_TILE_WIRE);
    updateFunds();

    if (result == 1) {
        didTool("Wire", x, y);
    }

    return result;
}


int Micropolis::parkTool(short x, short y)
{
    int result;

    if (!testBounds(x, y)) {
        return -1;
    }

    result = putDownPark(x, y);

    if (result == 1) {
        didTool("Park", x, y);
    }

    return result;
}


int Micropolis::residentialTool(short x, short y)
{
    int result;

    result = buildBuilding(x, y, 3, 3, RESBASE, TOOL_RESIDENTIAL, false);

    if (result == 1) {
        didTool("Res", x, y);
    }

    return result;
}


int Micropolis::commercialTool(short x, short y)
{
    int result;

    result = buildBuilding(x, y, 3, 3, COMBASE, TOOL_COMMERCIAL, false);

    if (result == 1) {
        didTool("Com", x, y);
    }

    return result;
}


int Micropolis::industrialTool(short x, short y)
{
    int result;

    result = buildBuilding(x, y, 3, 3, INDBASE, TOOL_INDUSTRIAL, false);

    if (result == 1) {
        didTool("Ind", x, y);
    }

    return result;
}


int Micropolis::policeStationTool(short x, short y)
{
    int result;

    result = buildBuilding(x, y, 3, 3, POLICESTBASE, TOOL_POLICESTATION, false);

    if (result == 1) {
        didTool("Pol", x, y);
    }

    return result;
}


int Micropolis::fireStationTool(short x, short y)
{
    int result;

    result = buildBuilding(x, y, 3, 3, FIRESTBASE, TOOL_FIRESTATION, false);

    if (result == 1) {
        didTool("Fire", x, y);
    }

    return result;
}


int Micropolis::stadiumTool(short x, short y)
{
    int result;

    result = buildBuilding(x, y, 4, 4, STADIUMBASE, TOOL_STADIUM, false);

    if (result == 1) {
        didTool("Stad", x, y);
    }

    return result;
}


int Micropolis::coalPowerTool(short x, short y)
{
    int result;

    // XXX Modified animation flag
    result = buildBuilding(x, y, 4, 4, COALBASE, TOOL_COALPOWER, false);

    if (result == 1) {
        didTool("Coal", x, y);
    }

    return result;
}


int Micropolis::nuclearPowerTool(short x, short y)
{
    int result;

    result = buildBuilding(x, y, 4, 4, NUCLEARBASE, TOOL_NUCLEARPOWER, true);

    if (result == 1) {
        didTool("Nuc", x, y);
    }

    return result;
}


int Micropolis::seaportTool(short x, short y)
{
    int result;

    result = buildBuilding(x, y, 4, 4, PORTBASE, TOOL_SEAPORT, false);

    if (result == 1) {
        didTool("Seap", x, y);
    }

    return result;
}


int Micropolis::airportTool(short x, short y)
{
    int result;

    result = buildBuilding(x, y, 6, 6, AIRPORTBASE, TOOL_AIRPORT, false);

    if (result == 1) {
        didTool("Airp", x, y);
    }

    return result;
}


int Micropolis::networkTool(short x, short y)
{
    int result;

    if (!testBounds(x, y)) {
        return -1;
    }

    result = putDownNetwork(x, y);

    if (result == 1) {
        didTool("Net", x, y);
    }

    return result;
}


int Micropolis::waterTool(short x, short y)
{
    int result;

    if (!testBounds(x, y)) {
        return -1;
    }

    result = bulldozerTool(x, y);

    if (result == 1) {
        result = putDownWater(x, y);
    }

    if (result == 1) {
        didTool("Water", x, y);
    }

    return result;
}


int Micropolis::landTool(short x, short y)
{
    int result;

    if (!testBounds(x, y)) {
        return -1;
    }

    result = bulldozerTool(x, y);

    result = putDownLand(x, y);

    if (result == 1) {
        didTool("Land", x, y);
    }

    return result;
}


int Micropolis::forestTool(short x, short y)
{
    int result;

    if (!testBounds(x, y)) {
        return -1;
    }

    int tile = map[x][y];

    if (isTree(tile)) {
	return 0;
    }

    if ((tile & LOMASK) != DIRT) {
        /// @todo bulldozer should be free in terrain mode or from a free tool.
	result = bulldozerTool(x, y);
    }

    tile = map[x][y];

    if (tile == DIRT) {
	result = putDownForest(x, y);

	if (result == 1) {
	    didTool("Forest", x, y);
	}

    } else {
        result = 0;
    }
  

    return result;
}


int Micropolis::doTool(EditingTool tool, short tileX, short tileY)
{
    switch (tool) {

    case TOOL_RESIDENTIAL:
        return residentialTool(tileX, tileY);

    case TOOL_COMMERCIAL:
        return commercialTool(tileX, tileY);

    case TOOL_INDUSTRIAL:
        return industrialTool(tileX, tileY);

    case TOOL_FIRESTATION:
        return fireStationTool(tileX, tileY);

    case TOOL_POLICESTATION:
        return policeStationTool(tileX, tileY);

    case TOOL_QUERY:
        return queryTool(tileX, tileY);

    case TOOL_WIRE:
        return wireTool(tileX, tileY);

    case TOOL_BULLDOZER:
        return bulldozerTool(tileX, tileY);

    case TOOL_RAILROAD:
        return railroadTool(tileX, tileY);

    case TOOL_ROAD:
        return roadTool(tileX, tileY);

    case TOOL_STADIUM:
        return stadiumTool(tileX, tileY);

    case TOOL_PARK:
        return parkTool(tileX, tileY);

    case TOOL_SEAPORT:
        return seaportTool(tileX, tileY);

    case TOOL_COALPOWER:
        return coalPowerTool(tileX, tileY);

    case TOOL_NUCLEARPOWER:
        return nuclearPowerTool(tileX, tileY);

    case TOOL_AIRPORT:
        return airportTool(tileX, tileY);

    case TOOL_NETWORK:
        return networkTool(tileX, tileY);

    case TOOL_WATER:
        return waterTool(tileX, tileY);

    case TOOL_LAND:
        return landTool(tileX, tileY);

    case TOOL_FOREST:
        return forestTool(tileX, tileY);

    default:
        return 0;

    }
}


/**
 * @todo Add enum for tool result values.
 */
void Micropolis::toolDown(EditingTool tool, short tileX, short tileY)
{
    int result;

    result = doTool(tool, tileX, tileY);

    if (result == -1) {
        sendMessage(MESSAGE_BULLDOZE_AREA_FIRST, NOWHERE, NOWHERE, false, true);
        /// @todo: Multi player: This sound should only be heard by the user who called this function.
        makeSound("interface", "UhUh", tileX <<4, tileY <<4);
    } else if (result == -2) {
        sendMessage(MESSAGE_NOT_ENOUGH_FUNDS, NOWHERE, NOWHERE, false, true);
        /// @todo: Multi player: This sound should only be heard by the user who called this function.
        makeSound("interface", "Sorry", tileX <<4, tileY <<4);
    }

    simPass = 0;
    invalidateEditors();
}


void Micropolis::toolDrag(EditingTool tool, short fromX, short fromY, short toX, short toY)
{
    short x = toX;
    short y = toY;
    short lx = fromX;
    short ly = fromY;
    short dx = x - lx;
    short dy = y - ly;

    if ((dx == 0) && (dy == 0)) return;

    // @bug This is crap code that needs to be rewritten.
    // @todo Use a proper breshenham that draws the corners.

    int toolSize = gToolSize[tool];
    float rx = (float)(dx < 0 ? 1 : 0);
    float ry = (float)(dy < 0 ? 1 : 0);
    short adx = absoluteValue(dx);
    short ady = absoluteValue(dy);

    float step;
    if (adx > ady) {
        step = (float)0.3 / adx;
    } else {
        step = (float)0.3 / ady;
    }

    float i;
    for (i = 0.0; i <= 1 + step; i += step) {
        float tx = fromX + i * dx;
        float ty = fromY + i * dy;
        float dtx = absoluteValue(tx - lx);
        float dty = absoluteValue(ty - ly);
        if (toolSize == 1) {
            if ((dtx >= 1) || (dty >= 1)) {
                // fill in corners
                if ((dtx >= 1) && (dty >= 1)) {
                    if (dtx > dty) {
                        doTool(tool, (int)(tx + rx), ly);
                    } else {
                        doTool(tool, lx, (int)(ty + ry));
                    }
                }
            }
        }
        lx = (int)(tx + rx);
        ly = (int)(ty + ry);
        doTool(tool, lx, ly);
    }

    simPass = 0; // update editors overlapping this one

    invalidateEditors();
}


////////////////////////////////////////////////////////////////////////
