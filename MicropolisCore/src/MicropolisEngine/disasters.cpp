/* disasters.cpp
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

/** @file disasters.cpp */

////////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "text.h"


////////////////////////////////////////////////////////////////////////


/**
 * Let disasters happen.
 * @todo Decide what to do with the 'nothing happens' disaster (since the
 *       chance that a disaster happens is expressed in the \c DisChance
 *       table).
 */
void Micropolis::doDisasters()
{
    /* Chance of disasters at lev 0 1 2 */
    static const short DisChance[3] = {
        10 * 48, // Game level 0
        5 * 48,  // Game level 1
        60 // Game level 2
    };
    assert(LEVEL_COUNT == LENGTH_OF(DisChance));

    if (floodCount) {
        floodCount--;
    }

    if (disasterEvent != SC_NONE) {
        scenarioDisaster();
    }

    if (!enableDisasters) { // Disasters have been disabled
        return;
    }

    int x = gameLevel;
    if (x > LEVEL_LAST) {
        x = LEVEL_EASY;
    }

    if (!getRandom(DisChance[x])) {
        switch (getRandom(8)) {
            case 0:
            case 1:
                setFire();  // 2/9 chance a fire breaks out
                break;

            case 2:
            case 3:
                makeFlood(); // 2/9 chance for a flood
                break;

            case 4:
                // 1/9 chance nothing happens (was airplane crash,
                // which EA removed after 9/11, and requested it be
                // removed from this code)
                break;

            case 5:
                makeTornado(); // 1/9 chance tornado
                break;

            case 6:
                makeEarthquake(); // 1/9 chance earthquake
                break;

            case 7:
            case 8:
                // 2/9 chance a scary monster arrives in a dirty town
                if (pollutionAverage > /* 80 */ 60) {
                    makeMonster();
                }
                break;
        }
    }
}


/** Let disasters of the scenario happen */
void Micropolis::scenarioDisaster()
{
    switch (disasterEvent) {
        case SC_DULLSVILLE:
            break;

        case SC_SAN_FRANCISCO:
            if (disasterWait == 1) {
                makeEarthquake();
            }
            break;

        case SC_HAMBURG:
            dropFireBombs();
            break;

        case SC_BERN:
            break;

        case SC_TOKYO:
            if (disasterWait == 1) {
                makeMonster();
            }
            break;

        case SC_DETROIT:
            break;

        case SC_BOSTON:
            if (disasterWait == 1) {
                makeMeltdown();
            }
            break;

        case SC_RIO:
            if ((disasterWait % 24) == 0) {
                makeFlood();
            }
            break;

        default:
            NOT_REACHED();
            break; // Never used
    }

    if (disasterWait > 0) {
        disasterWait--;
    } else {
        disasterEvent = SC_NONE;
    }
}


/**
 * Make a nuclear power plant melt
 * @todo Randomize which nuke plant melts down.
 */
void Micropolis::makeMeltdown()
{
    short x, y;

    for (x = 0; x < (WORLD_W - 1); x++) {
        for (y = 0; y < (WORLD_H - 1); y++) {
            if ((map[x][y] & LOMASK) == NUCLEAR) {
                doMeltdown(x, y);
                return;
            }
        }
    }
}

/** Let a fire bomb explode at a random location */
void Micropolis::fireBomb()
{
    crashX = getRandom(WORLD_W - 1);
    crashY = getRandom(WORLD_H - 1);
    makeExplosion(crashX, crashY);
    clearMessage();
    sendMessageAt(-STR301_FIREBOMBING, crashX, crashY);
}


/** Change random tiles to fire or dirt as result of the earthquake */
void Micropolis::makeEarthquake()
{
    short x, y, z;

    int strength = getRandom(700) + 300; // strength/duration of the earthquake

    doEarthquake(strength);

    sendMessageAt(-STR301_EARTHQUAKE, cityCenterX, cityCenterY);

    for (z = 0; z < strength; z++)  {
        x = getRandom(WORLD_W - 1);
        y = getRandom(WORLD_H - 1);

        if (vulnerable(map[x][y])) {

            if ((z & 0x3) != 0) { // 3 of 4 times reduce to rubble
                map[x][y] = randomRubble();
            } else {
                // 1 of 4 times start fire
                map[x][y] = randomFire();
            }
        }
    }
}


/** Start a fire at a random place, random disaster or scenario */
void Micropolis::setFire()
{
    short x, y, z;

    x = getRandom(WORLD_W - 1);
    y = getRandom(WORLD_H - 1);
    z = map[x][y];

    /* TILE_IS_ARSONABLE(z) */
    if ((z & ZONEBIT) == 0) {
        z = z & LOMASK;
        if (z > LHTHR && z < LASTZONE) {
            map[x][y] = randomFire();
            crashX = x;
            crashY = y;
            sendMessageAt(-STR301_FIRE_REPORTED, x, y);
        }
    }
}


/** Start a fire at a random place, requested by user */
void Micropolis::makeFire()
{
    short t, x, y, z;

    for (t = 0; t < 40; t++)  {
        x = getRandom(WORLD_W - 1);
        y = getRandom(WORLD_H - 1);
        z = map[x][y];

        if ((!(z & ZONEBIT)) && (z & BURNBIT)) {
            z = z & LOMASK;
            if ((z > 21) && (z < LASTZONE)) {
                map[x][y] = randomFire();
                sendMessageAt(STR301_FIRE_REPORTED, x, y);
                return;
            }
        }
    }
}


/**
 * Is tile vulnerable for an earthquake?
 * @param tem Tile data
 * @return Function returns \c true if tile is vulnerable, and \c false if not
 */
bool Micropolis::vulnerable(int tem)
{
    int tem2 = tem & LOMASK;

    if (tem2 < RESBASE || tem2 > LASTZONE || (tem & ZONEBIT)) {
        return false;
    }

    return true;
}


/**
 * Flood many tiles
 * @todo Use Direction and some form of XYPosition class here
 */
void Micropolis::makeFlood()
{
    static const short Dx[4] = {  0,  1,  0,  -1 };
    static const short Dy[4] = { -1,  0,  1,   0 };
    short xx, yy, c;
    short z, t, x, y;

    for (z = 0; z < 300; z++) {
        x = getRandom(WORLD_W - 1);
        y = getRandom(WORLD_H - 1);
        c = map[x][y] & LOMASK;

        if (c > CHANNEL && c <= WATER_HIGH) { /* if riveredge  */
            for (t = 0; t < 4; t++) {
                xx = x + Dx[t];
                yy = y + Dy[t];
                if (TestBounds(xx, yy)) {
                    c = map[xx][yy];

                    /* tile is floodable */
                    if (c == DIRT
                          || (c & (BULLBIT | BURNBIT)) == (BULLBIT | BURNBIT)) {
                        map[xx][yy] = FLOOD;
                        floodCount = 30;
                        sendMessageAt(-STR301_FLOODING_REPORTED, xx, yy);
                        return;
                    }
                }
            }
        }
    }
}


/**
 * Flood around the (curMapX, curMapY) tile
 * @todo Use Direction and some form of XYPosition class here
 */
void Micropolis::doFlood()
{
    static const short Dx[4] = {  0,  1,  0, -1 };
    static const short Dy[4] = { -1,  0,  1,  0 };
    register short z, c, xx, yy, t;

    if (floodCount > 0) {
        // Flood is not over yet
        for (z = 0; z < 4; z++) {
            if ((getRandom16() & 7) == 0) { // 12.5% chance
                xx = curMapX + Dx[z];
                yy = curMapY + Dy[z];
                if (TestBounds(xx, yy)) {
                    c = map[xx][yy];
                    t = c & LOMASK;

                    /* TILE_IS_FLOODABLE2(c) */
                    if ((c & BURNBIT) == BURNBIT || c == DIRT
                                            || (t >= WOODS5 && t < FLOOD)) {
                        if ((c & ZONEBIT) == ZONEBIT) {
                            fireZone(xx, yy, c);
                        }
                        map[xx][yy] = FLOOD + getRandom(2);
                    }
                }
            }
         }
    } else {
        if ((getRandom16() & 15) == 0) { // 1/16 chance
            map[curMapX][curMapY] = DIRT;
        }
    }
}


////////////////////////////////////////////////////////////////////////
