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
#include "text.h"


////////////////////////////////////////////////////////////////////////


static const short gCostOf[] = {
     100,    100,    100,    500,
     500,      0,      5,      1,
      20,     10,   5000,     10,
    3000,   3000,   5000,  10000,
     100,      0,
};


static const short gToolSize[] = {
    3, 3, 3, 3,
    3, 1, 1, 1,
    1, 1, 4, 1,
    4, 4, 4, 6,
    1, 0,
};


////////////////////////////////////////////////////////////////////////
// Utilities

/**
 * Put a park down at the give tile.
 * @param mapH X coordinate of the tile.
 * @param mapV Y coordinate of the tile.
 * @return Build result (-2 = no money, -1 = cannot build, 1 = built).
 * @todo Add auto-bulldoze? (seems to be missing).
 */
int Micropolis::putDownPark(short mapH, short mapV)
{
    short value, tile;

    if (TotalFunds - gCostOf[TOOL_PARK] < 0) return -2;

    value = Rand(4);

    if (value == 4) {
        tile = FOUNTAIN | BURNBIT | BULLBIT | ANIMBIT;
    } else {
        tile = (value + WOODS2) | BURNBIT | BULLBIT;
    }

    if (Map[mapH][mapV] != DIRT) return -1;

    Map[mapH][mapV] = tile;

    Spend(gCostOf[TOOL_PARK]);
    UpdateFunds();

    return 1;
}

/**
 * Put down a communication network.
 * @param mapH X coordinate of the tile.
 * @param mapV Y coordinate of the tile.
 * @return Build result (-2 = no money, -1 = cannot build, 1 = built).
 * @todo Auto-bulldoze deducts always 1.
 * @todo Auto-bulldoze costs should be pulled from a table/constant.
 */
int Micropolis::putDownNetwork(short mapH, short mapV)
{
    int tile = Map[mapH][mapV] & LOMASK;

    if (TotalFunds > 0 && tally(tile)) {
        Map[mapH][mapV] = DIRT;
        tile = DIRT;
        Spend(1);
    }

    if (tile != DIRT) return -1;

    if (TotalFunds - gCostOf[TOOL_NETWORK] < 0) return -2;

    Map[mapH][mapV] = TELEBASE | CONDBIT | BURNBIT | BULLBIT | ANIMBIT;

    Spend(gCostOf[TOOL_NETWORK]);
    UpdateFunds();

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
 * @param xMap X coordinate of top-left tile.
 * @param yMap Y coordinate of top-left tile.
 * @param size Square size of the new zone.
 */
void Micropolis::checkBorder(short xMap, short yMap, int size)
{
    short xPos, yPos;
    short cnt;

    xPos = xMap;
    yPos = yMap - 1;

    for (cnt = 0; cnt < size; cnt++) {
        /* this will do the upper bordering row */
        ConnecTile(xPos, yPos, &Map[xPos][yPos], 0);
        xPos++;
    }

    xPos = xMap - 1;
    yPos = yMap;

    for (cnt = 0; cnt < size; cnt++) {
        /* this will do the left bordering row */
        ConnecTile(xPos, yPos, &Map[xPos][yPos], 0);
        yPos++;
    }

    xPos = xMap;
    yPos = yMap + size;

    for (cnt = 0; cnt < size; cnt++) {
        /* this will do the bottom bordering row */
        ConnecTile(xPos, yPos, &Map[xPos][yPos], 0);
        xPos++;
    }

    xPos = xMap + size;
    yPos = yMap;

    for (cnt = 0; cnt < size; cnt++) {
        /* this will do the right bordering row */
        ConnecTile(xPos, yPos, &Map[xPos][yPos], 0);
        yPos++;
    }
}

/* 3x3 */


int Micropolis::check3x3(
    short mapH, short mapV, 
    short base, short tool)
{
    short rowNum, columnNum;
    short holdMapH, holdMapV;
    short xPos, yPos;
    short cost = 0;
    short tileValue;
    short flag;

    mapH--; mapV--;

    // Tool partly sticking outside world boundaries?
    if (mapH < 0 || mapH > WORLD_X - 3 || mapV < 0 || mapV > WORLD_Y - 3) {
        return -1;
    }

    xPos = holdMapH = mapH;
    yPos = holdMapV = mapV;

    flag = 1;

    for (rowNum = 0; rowNum <= 2; rowNum++) {

        mapH = holdMapH;

        for (columnNum = 0; columnNum <= 2; columnNum++) {

            tileValue = Map[mapH++][mapV] & LOMASK;

            if (autoBulldoze) {

                /* if autoDoze is enabled, add up the cost of bulldozed tiles */
                if (tileValue != DIRT) {

                    if (tally(tileValue)) {
                        cost++;
                    } else {
                        flag = 0;
                    }

                }

            } else {

                /* check and see if the tile is clear or not  */
                if (tileValue != 0) flag = 0;

            }
        }

        mapV++;
    }

    if (flag == 0) return -1;

    cost += (short)gCostOf[tool];

    if ((TotalFunds - cost) < 0) return -2;

    /* take care of the money situtation here */
    Spend(cost);
    UpdateFunds();

    mapV = holdMapV;

    for (rowNum = 0; rowNum <= 2; rowNum++) {

      mapH = holdMapH;

      for (columnNum = 0; columnNum <= 2; columnNum++) {

        if ((columnNum == 1) &&
            (rowNum == 1)) {
          Map[mapH++][mapV] = base + BNCNBIT + ZONEBIT;
        } else {
          Map[mapH++][mapV] = base + BNCNBIT;
        }

        base++;
      }

      mapV++;
    }

    checkBorder(xPos, yPos, 3);

    return 1;
}


/* 4x4 */


short Micropolis::check4x4(
    short mapH, short mapV,
    short base, short aniFlag, short tool)
{
    short rowNum, columnNum;
    short h, v;
    short holdMapH;
    short xMap, yMap;
    short tileValue;
    short flag;
    short cost = 0;

    mapH--;
    mapV--;

    if ((mapH < 0) ||
        (mapH > (WORLD_X - 4)) ||
        (mapV < 0) ||
        (mapV > (WORLD_Y - 4))) {
        return -1;
    }

    h = xMap = holdMapH = mapH;
    v = yMap = mapV;

    flag = 1;

    for (rowNum = 0; rowNum <= 3; rowNum++) {

        mapH = holdMapH;

        for (columnNum = 0; columnNum <= 3; columnNum++) {

            tileValue = Map[mapH++][mapV] & LOMASK;

            if (autoBulldoze) {

                /* if autoDoze is enabled, add up the cost of bulldozed tiles */
                if (tileValue != DIRT) {

                    if (tally(tileValue)) {
                        cost++;
                    } else {
                        flag = 0;
                    }

                }

            } else {

                /* check and see if the tile is clear or not  */
                if (tileValue != 0) flag = 0;

            }

        }

        mapV++;
    }

    if (flag == 0) return -1;

    cost += (short)gCostOf[tool];

    if ((TotalFunds - cost) < 0) return -2;

    /* take care of the money situtation here */
    Spend(cost);
    UpdateFunds();

    mapV = v;
    holdMapH = h;

    for (rowNum = 0; rowNum <= 3; rowNum++) {

        mapH = holdMapH;

        for (columnNum = 0; columnNum <= 3; columnNum++) {

            if ((columnNum == 1) && (rowNum == 1)) {
                Map[mapH++][mapV] = base + BNCNBIT + ZONEBIT;
            } else if ((columnNum == 1) && (rowNum == 2) && aniFlag) {
                Map[mapH++][mapV] = base + BNCNBIT + ANIMBIT;
            } else {
                Map[mapH++][mapV] = base + BNCNBIT;
            }

            base++;
        }

        mapV++;
    }

    checkBorder(xMap, yMap, 4);

    return 1;
}


/* 6x6 */


short Micropolis::check6x6(
  short mapH, short mapV,
  short base, short tool)
{
    short rowNum, columnNum;
    short h, v;
    short holdMapH;
    short xMap, yMap;
    short flag;
    short tileValue;
    short cost = 0;

    mapH--; mapV--;
    if ((mapH < 0) ||
        (mapH > (WORLD_X - 6)) ||
        (mapV < 0) ||
        (mapV > (WORLD_Y - 6))) {
        return -1;
    }

    h = xMap = holdMapH = mapH;
    v = yMap = mapV;

    flag = 1;

    for (rowNum = 0; rowNum <= 5; rowNum++) {

        mapH = holdMapH;

        for (columnNum = 0; columnNum <= 5; columnNum++) {

            tileValue = Map[mapH++][mapV] & LOMASK;

            if (autoBulldoze) {

                /* if autoDoze is enabled, add up the cost of bulldozed tiles */
                if (tileValue != DIRT) {

                    if (tally(tileValue)) {
                        cost++;
                    } else {
                        flag = 0;
                    }

                }

            } else {

              /* check and see if the tile is clear or not  */
              if (tileValue != DIRT) flag = 0;

            }

        }

        mapV++;
    }

    if (flag == 0) return -1;

    cost += (short)gCostOf[tool];

    if ((TotalFunds - cost) < 0) return -2;

    /* take care of the money situtation here */
    Spend(cost);
    UpdateFunds();

    mapV = v;
    holdMapH = h;

    for (rowNum = 0; rowNum <= 5; rowNum++) {

        mapH = holdMapH;

        for (columnNum = 0; columnNum <= 5; columnNum++) {

            if ((columnNum == 1) && (rowNum == 1)) {
                Map[mapH++][mapV] = base + BNCNBIT + ZONEBIT;
            } else {
                Map[mapH++][mapV] = base + BNCNBIT;
            }

            base++;
        }

        mapV++;
    }

    checkBorder(xMap, yMap, 6);

    return 1;
}


/* QUERY */


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
        z = PopDensity[mapH >>1][mapV >>1];
        z = z >> 6;
        z = z & 3;
        return z + STR202_POPULATIONDENSITY_LOW;

    case 1:
        z = LandValueMem[mapH >>1][mapV >>1];
        if (z < 30) return STR202_LANDVALUE_SLUM;
        if (z < 80) return STR202_LANDVALUE_LOWER_CLASS;
        if (z < 150) return STR202_LANDVALUE_MIDDLE_CLASS;
        return STR202_LANDVALUE_HIGH_CLASS;

    case 2:
        z = CrimeMem[mapH >>1][mapV >>1];
        z = z >> 6;
        z = z & 3;
        return z + STR202_CRIME_NONE;

    case 3:
        z = PollutionMem[mapH >>1][mapV >>1];
        if ((z < 64) && (z > 0)) return 13;
        z = z >> 6;
        z = z & 3;
        return z + STR202_POLLUTION_NONE;

    case 4:
        z = RateOGMem[mapH >>3][mapV >>3];
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

    short tileNum = Map[mapH][mapV] & LOMASK;

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
    GetIndString(localStr, 219, i + 1);

    for (i = 0; i < 5; i++) {
        short id = clamp(getDensityStr(i, mapH, mapV) + 1, 1, 20);
        GetIndString(statusStr[i], 202, id);
    }

    doShowZoneStatus(localStr, statusStr[0], statusStr[1], statusStr[2],
                    statusStr[3], statusStr[4], mapH, mapV);
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
void Micropolis::doShowZoneStatus(char *str, char *s0, char *s1, char *s2, char *s3, char *s4,
                                    int x, int y)
{
    Callback("UIShowZoneStatus", "ssssssdd", str, s0, s1, s2, s3, s4, x, y);
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

            if (TestBounds(xx, yy)) {
                int tile = Map[xx][yy] & LOMASK;

                if (tile != RADTILE && tile != DIRT) {
                    tile = (DoAnimation ? (TINYEXP + Rand(2)) : SOMETINYEXP);
                    Map[xx][yy] = tile | ANIMBIT | BULLBIT;
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
    Callback("UIDidTool", "sdd", name, (int)x, (int)y);
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
    if (x < 0 || x > WORLD_X - 1 || y < 0 || y > WORLD_Y - 1) {
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

    if (x < 0 || x > WORLD_X - 1 || y < 0 || y > WORLD_Y - 1) {
        return -1;
    }

    currTile = Map[x][y];
    temp = currTile & LOMASK;

    if (currTile & ZONEBIT) { /* zone center bit is set */

        if (TotalFunds > 0) {

            Spend(1);

            switch (checkSize(temp)) {

            case 3:
                MakeSound("city", "Explosion-High", x, y);
                putRubble(x - 1, y - 1, 3);
                break;

            case 4:
                MakeSound("city", "Explosion-Low", x, y);
                putRubble(x - 1, y - 1, 4);
                break;

            case 6:
                MakeSound("city", "Explosion-High", x, y);
                MakeSound("city", "Explosion-Low", x, y);
                putRubble(x - 1, y - 1, 6);
                break;

            default:
                break;

            }

        }

    } else if ((zoneSize = checkBigZone(temp, &deltaH, &deltaV))) {

        if (TotalFunds > 0) {

            Spend(1);

            switch (zoneSize) {

            case 3:
                MakeSound("city", "Explosion-High", x, y);
                break;

            case 4:
                MakeSound("city", "Explosion-Low", x, y);
                putRubble(x + deltaH - 1, y + deltaV - 1, 4);
                break;

            case 6:
                MakeSound("city", "Explosion-High", x, y);
                MakeSound("city", "Explosion-Low", x, y);
                putRubble(x + deltaH - 1, y + deltaV - 1, 6);
                break;

            }

        }

    } else {

        if (temp == RIVER || temp == REDGE || temp == CHANNEL) {

            if (TotalFunds >= 6) {

                result = ConnecTile(x, y, &Map[x][y], 1);

                if (temp != (Map[x][y] & LOMASK)) {
                  Spend(5);
                }

            } else {
                result = 0;
            }
        } else {
            result = ConnecTile(x, y, &Map[x][y], 1);
        }

    }

    UpdateFunds();

    if (result == 1) {
        didTool("Dozr", x, y);
    }

    return result;
}


/** @todo Generalize TestBounds for different upper bounds */
int Micropolis::roadTool(short x, short y)
{
    int result;

    if (x < 0 || x > WORLD_X - 1 || y < 0 || y > WORLD_Y - 1) {
        return -1;
    }

    result = ConnecTile(x, y, &Map[x][y], 2);
    UpdateFunds();

    if (result == 1) {
        didTool("Road", x, y);
    }

    return result;
}


int Micropolis::railroadTool(short x, short y)
{
    int result;

    if (x < 0 || x > WORLD_X - 1 || y < 0 || y > WORLD_Y - 1) {
        return -1;
    }

    result = ConnecTile(x, y, &Map[x][y], 3);
    UpdateFunds();

    if (result == 1) {
        didTool("Rail", x, y);
    }

    return result;
}


int Micropolis::wireTool(short x, short y)
{
    int result;

    if (x < 0 || x > WORLD_X - 1 || y < 0 || y > WORLD_Y - 1) {
        return -1;
    }

    result = ConnecTile(x, y, &Map[x][y], 4);
    UpdateFunds();

    if (result == 1) {
        didTool("Wire", x, y);
    }

    return result;
}


int Micropolis::parkTool(short x, short y)
{
    int result;

    if (x < 0 || x > WORLD_X - 1 || y < 0 || y > WORLD_Y - 1) {
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

    if (x < 0 || x > WORLD_X - 1 || y < 0 || y > WORLD_Y - 1) {
        return -1;
    }

    result = check3x3(x, y, RESBASE, TOOL_RESIDENTIAL);

    if (result == 1) {
        didTool("Res", x, y);
    }

    return result;
}


int Micropolis::commercialTool(short x, short y)
{
    int result;

    if (x < 0 || x > WORLD_X - 1 || y < 0 || y > WORLD_Y - 1) {
        return -1;
    }

    result = check3x3(x, y, COMBASE, TOOL_COMMERCIAL);

    if (result == 1) {
        didTool("Com", x, y);
    }

    return result;
}


int Micropolis::industrialTool(short x, short y)
{
    int result;

    if (x < 0 || x > WORLD_X - 1 || y < 0 || y > WORLD_Y - 1) {
        return -1;
    }

    result = check3x3(x, y, INDBASE, TOOL_INDUSTRIAL);

    if (result == 1) {
        didTool("Ind", x, y);
    }

    return result;
}


int Micropolis::policeStationTool(short x, short y)
{
    int result;

    if (x < 0 || x > WORLD_X - 1 || y < 0 || y > WORLD_Y - 1) {
        return -1;
    }

    result = check3x3(x, y, POLICESTBASE, TOOL_POLICESTATION);

    if (result == 1) {
        didTool("Pol", x, y);
    }

    return result;
}


int Micropolis::fireStationTool(short x, short y)
{
    int result;

    if (x < 0 || x > WORLD_X - 1 || y < 0 || y > WORLD_Y - 1) {
        return -1;
    }

    result = check3x3(x, y, FIRESTBASE, TOOL_FIRESTATION);

    if (result == 1) {
        didTool("Fire", x, y);
    }

    return result;
}


int Micropolis::stadiumTool(short x, short y)
{
    int result;

    if (x < 0 || x > WORLD_X - 1 || y < 0 || y > WORLD_Y - 1) {
        return -1;
    }

    result = check4x4(x, y, STADIUMBASE, 0, TOOL_STADIUM);

    if (result == 1) {
        didTool("Stad", x, y);
    }

    return result;
}


int Micropolis::coalPowerTool(short x, short y)
{
    int result;

    if (x < 0 || x > WORLD_X - 1 || y < 0 || y > WORLD_Y - 1) {
        return -1;
    }

    result = check4x4(x, y, COALBASE, 1, TOOL_COALPOWER);

    if (result == 1) {
        didTool("Coal", x, y);
    }

    return result;
}


int Micropolis::nuclearPowerTool(short x, short y)
{
    int result;

    if (x < 0 || x > WORLD_X - 1 || y < 0 || y > WORLD_Y - 1) {
        return -1;
    }

    result = check4x4(x, y, NUCLEARBASE, 1, TOOL_NUCLEARPOWER);

    if (result == 1) {
        didTool("Nuc", x, y);
    }

    return result;
}


int Micropolis::seaportTool(short x, short y)
{
    int result;

    if (x < 0 || x > WORLD_X - 1 || y < 0 || y > WORLD_Y - 1) {
        return -1;
    }

    result = check4x4(x, y, PORTBASE, 0, TOOL_SEAPORT);

    if (result == 1) {
        didTool("Seap", x, y);
    }

    return result;
}


int Micropolis::airportTool(short x, short y)
{
    int result;

    if (x < 0 || x > WORLD_X - 1 || y < 0 || y > WORLD_Y - 1) {
        return -1;
    }

    result = check6x6(x, y, AIRPORTBASE, TOOL_AIRPORT);

    if (result == 1) {
        didTool("Airp", x, y);
    }

    return result;
}


int Micropolis::networkTool(short x, short y)
{
    int result;

    if (x < 0 || x > WORLD_X - 1 || y < 0 || y > WORLD_Y - 1) {
        return -1;
    }

    result =
        putDownNetwork(x, y);

    if (result == 1) {
        didTool("Net", x, y);
    }

    return result;
}


int Micropolis::doTool(EditingTool tool, short x, short y, bool first)
{
    short tileX = x >>4;
    short tileY = y >>4;

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

    default:
        return 0;

    }
}


/**
 * @todo The last coordinates should be passed from the tool so the
 *       simulator is stateless and can support multiple tools
 *       drawing at once. Get rid of last_x and last_y.
 * @todo Add enum for tool result values.
 */
void Micropolis::toolDown(EditingTool tool, short x, short y)
{
    int result;

    last_x = x;
    last_y = y;

    result = doTool(tool, x <<4, y <<4, true);

    if (result == -1) {
        ClearMes();
        SendMes(STR301_BULLDOZE_AREA_FIRST);
	// @todo: Multi player: This sound should only be heard by the user who called this function. 
        MakeSound("interface", "UhUh", x <<4, y <<4);
    } else if (result == -2) {
        ClearMes();
        SendMes(STR301_NOT_ENOUGH_FUNDS);
	// @todo: Multi player: This sound should only be heard by the user who called this function. 
        MakeSound("interface", "Sorry", x <<4, y <<4);
    }

    sim_skip = 0;
    InvalidateEditors();
}


void Micropolis::toolUp(EditingTool tool, short x, short y)
{
    toolDrag(tool, x, y);
}


void Micropolis::toolDrag(EditingTool tool, short px, short py)
{
    short x, y, dx, dy, adx, ady, lx, ly, dist;
    float i, step, tx, ty, dtx, dty, rx, ry;

    x = px;
    y = py;

    tool_x = x;
    tool_y = y;

    dist = gToolSize[tool];

    x >>= 4;
    y >>= 4;
    lx = last_x >> 4;
    ly = last_y >> 4;

    dx = x - lx;
    dy = y - ly;

    if ((dx == 0) && (dy == 0)) return;

    adx = ABS(dx);
    ady = ABS(dy);

    if (adx > ady) {
        step = (float)0.3 / adx;
    } else {
        step = (float)0.3 / ady;
    }

    rx = (float)(dx < 0 ? 1 : 0);
    ry = (float)(dy < 0 ? 1 : 0);

    if (dist == 1) {
        for (i = 0.0; i <= 1 + step; i += step) {
            tx = (last_x >>4) + i * dx;
            ty = (last_y >>4) + i * dy;
            dtx = ABS(tx - lx);
            dty = ABS(ty - ly);
            if ((dtx >= 1) || (dty >= 1)) {
                // fill in corners
                if ((dtx >= 1) && (dty >= 1)) {
                    if (dtx > dty) {
                        doTool(tool, ((int)(tx + rx)) <<4, ly <<4, false);
                    } else {
                        doTool(tool, lx <<4, ((int)(ty + ry)) <<4, false);
                    }
                }
                lx = (int)(tx + rx);
                ly = (int)(ty + ry);
                doTool(tool, lx <<4, ly <<4, false);
            }
        }
    } else {
        for (i = 0.0; i <= 1 + step; i += step) {
            tx = (last_x >>4) + i * dx;
            ty = (last_y >>4) + i * dy;
            dtx = ABS(tx - lx);
            dty = ABS(ty - ly);
            lx = (int)(tx + rx);
            ly = (int)(ty + ry);
            doTool(tool, lx <<4, ly <<4, false);
        }
    }

    last_x = (lx <<4) + 8;
    last_y = (ly <<4) + 8;

    sim_skip = 0; // update editors overlapping this one

    InvalidateEditors();
}


////////////////////////////////////////////////////////////////////////
