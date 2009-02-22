/* traffic.cpp
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

/** @file traffic.cpp Traffic generation
 * @todo Introduce a XY position class that can be passed around
 */

////////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "micropolis.h"


////////////////////////////////////////////////////////////////////////


/**
 * Find a connection over a road from the current place to a specified zone type
 * @param dest Zone type to go to
 * @return \c 1 if connection found, \c 0 if not found,
 *         \c -1 if no connection to road found
 */
short Micropolis::makeTraffic(ZoneType dest)
{
    short xtem, ytem;

    xtem = curMapX; // Temporarily save curMapX
    ytem = curMapY; // Temporarily save curMapY
    curMapStackPointer = 0; // Clear position stack

#if 0
      if ((!getRandom(2)) && findPermeterTelecom()) {
        /* printf("Telecom!\n"); */
        return 1;
      }
#endif

    Position pos(curMapX, curMapY);
    bool found = findPerimeterRoad(&pos);
    if (found) {
        curMapX = pos.posX;
        curMapY = pos.posY;
    }

    if (found) {            /* look for road on zone perimeter */

        if (tryDrive(dest)) {     /* attempt to drive somewhere */
            setTrafficMap();      /* if sucessful, inc trafdensity */
            curMapX = xtem;
            curMapY = ytem;
            return 1;             /* traffic passed */
        }

        curMapX = xtem;
        curMapY = ytem;

        return 0;                 /* traffic failed */
    } else {
        return -1;                /* no road found */
    }
}


/**
 * Update the #trafficDensityMap from the positions at the #curMapStackXY stack.
 */
void Micropolis::setTrafficMap()
{
    /* For each saved position of the drive */
    while (curMapStackPointer > 0) {

        Position pos = pullPos();
        if (pos.testBounds()) {

            MapTile tile = map[pos.posX][pos.posY] & LOMASK;

            if (tile >= ROADBASE && tile < POWERBASE) {
                SimSprite *sprite;

                // Update traffic density.
                int traffic = trafficDensityMap.worldGet(pos.posX, pos.posY);
                traffic += 50;
                traffic = min(traffic, 240);
                trafficDensityMap.worldSet(pos.posX, pos.posY, (Byte)traffic);

                // Check for heavy traffic.
                if (traffic >= 240 && getRandom(5) == 0) {

                    trafMaxX = pos.posX;
                    trafMaxY = pos.posY;

                    /* Direct helicopter towards heavy traffic */
                    sprite = getSprite(SPRITE_HELICOPTER);
                    if (sprite != NULL && sprite->control == -1) {

                        sprite->destX = trafMaxX * 16;
                        sprite->destY = trafMaxY * 16;

                    }
                }
            }
        }
    }
}


/**
 * Push a position onto the position stack.
 * @param pos Position to push.
 * @pre Stack may not be full.
 */
void Micropolis::pushPos(const Position &pos)
{
    curMapStackPointer++;
    assert(curMapStackPointer < MAX_TRAFFIC_DISTANCE + 1);
    curMapStackXY[curMapStackPointer] = pos;
}


/**
 * Pull top-most position from the position stack.
 * @return Pulled position.
 * @pre Stack may not be empty (curMapStackPointer > 0)
 */
Position Micropolis::pullPos()
{
    assert(curMapStackPointer > 0);
    curMapStackPointer--;
    return curMapStackXY[curMapStackPointer + 1];
}


/**
 * Find a connection to a road at the perimeter.
 * @param pos Starting position.
 *            Gets updated when a perimeter has been found.
 * @return Indication that a connection has been found.
 */
bool Micropolis::findPerimeterRoad(Position *pos)
{
    /* look for road on edges of zone */
    static const short PerimX[12] = {-1, 0, 1, 2, 2, 2, 1, 0,-1,-2,-2,-2};
    static const short PerimY[12] = {-2,-2,-2,-1, 0, 1, 2, 2, 2, 1, 0,-1};
    short tx, ty;

    for (short z = 0; z < 12; z++) {

        tx = pos->posX + PerimX[z];
        ty = pos->posY + PerimY[z];

        if (testBounds(tx, ty)) {

            if (roadTest(map[tx][ty])) {

                pos->posX = tx;
                pos->posY = ty;

                return true;
            }
        }
    }

    return false;
}


/**
 * Find a telecom connection at the perimeter
 * @return Indication that a telecom connection has been found
 * @pre  curMapX and curMapY contain the starting coordinates
 */
bool Micropolis::findPerimeterTelecom()
{
    /* look for telecom on edges of zone */
    static const short PerimX[12] = {-1, 0, 1, 2, 2, 2, 1, 0,-1,-2,-2,-2};
    static const short PerimY[12] = {-2,-2,-2,-1, 0, 1, 2, 2, 2, 1, 0,-1};
    short tx, ty, tile;

    for (short z = 0; z < 12; z++) {

        tx = curMapX + PerimX[z];
        ty = curMapY + PerimY[z];

        if (testBounds(tx, ty)) {

            tile = map[tx][ty] & LOMASK;
            if (tile >= TELEBASE && tile <= TELELAST) {
                return true;
            }
        }
    }

    return false;
}


/**
 * Try to drive to a destination.
 * @param destZone Zonetype to drive to.
 * @return Was drive succesful?
 * @post Position stack (curMapStackX, curMapStackY, curMapStackPointer)
 *       is filled with some intermediate positions of the drive
 * @todo Find out why the stack is popped, but curMapX and curMapY are not updated
 */
bool Micropolis::tryDrive(ZoneType destZone)
{
    short dist;

    dirLast = DIR_INVALID;
    for (dist = 0; dist < MAX_TRAFFIC_DISTANCE; dist++) {  /* Maximum distance to try */

        if (tryGo(dist)) { /* if it got a road */

            if (driveDone(Position(curMapX, curMapY), destZone)) { /* if destination is reached */
                return true; /* pass */
            }

        } else {

            if (curMapStackPointer > 0) { /* dead end, backup */
                curMapStackPointer--;
                dist += 3;
            } else {
                return false; /* give up at start  */
            }

        }
    }

    return false; /* gone MAX_TRAFFIC_DISTANCE */
}


/**
 * Try to drive one tile in a random direction.
 * @param dist Distance traveled.
 * @return A move has been made.
 */
bool Micropolis::tryGo(int dist)
{
    short dir, dirRandom;
    Direction dirReal;

    dirRandom = getRandom16() & 3;

    for (dir = dirRandom; dir < (dirRandom + 4); dir++) { /* for the 4 directions */

        dirReal = (Direction)(dir & 3);

        if (dirReal == dirLast) {
            continue; /* skip last direction */
        }

        if (roadTest(getFromMap(Position(curMapX, curMapY), dirReal))) {
            moveMapSim(dirReal);
            dirLast = reverseDirection(dirReal);

            if (dist & 1) {
                /* Save pos every other move.
                 * This also relates to Micropolis::trafficDensityMap::MAP_BLOCKSIZE
                 */
                pushPos(Position(curMapX, curMapY));
            }

            return true;
        }
    }

    return false;
}


/**
 * Get neighbouring tile from the map.
 * @param pos Current position.
 * @param d   Direction of neighbouring tile.
 * @return The tile in the indicated direction. If tile is off-world or an
 *         incorrect direction is given, \c DIRT is returned.
 */
MapTile Micropolis::getFromMap(const Position &pos, Direction d)
{
    switch (d) {

        case DIR_NORTH:
            if (pos.posY > 0) {
              return map[pos.posX][pos.posY - 1] & LOMASK;
            }

            return DIRT;

        case DIR_EAST:
            if (pos.posX < WORLD_W - 1) {
              return map[pos.posX + 1][pos.posY] & LOMASK;
            }

            return DIRT;

        case DIR_SOUTH:
            if (pos.posY < WORLD_H - 1) {
              return map[pos.posX][pos.posY + 1] & LOMASK;
            }

            return DIRT;

        case DIR_WEST:
            if (pos.posX > 0) {
              return map[pos.posX - 1][pos.posY] & LOMASK;
            }

            return DIRT;

        default:
            return DIRT;

    }
}


/**
 * Has the journey arrived at its destination?
 * @param pos      Current position.
 * @param destZone Zonetype to drive to.
 * @return Destination has been reached.
 */
bool Micropolis::driveDone(const Position &pos, ZoneType destZone)
{
    /* commercial, industrial, residential destinations */
    static const MapTile targetLow[3] = {COMBASE, LHTHR, LHTHR};
    static const MapTile targetHigh[3] = {NUCLEAR, PORT, COMBASE};

    assert(ZT_NUM_DESTINATIONS == LENGTH_OF(targetLow));
    assert(ZT_NUM_DESTINATIONS == LENGTH_OF(targetHigh));

    MapTile l = targetLow[destZone]; // Lowest acceptable tile value
    MapTile h = targetHigh[destZone]; // Highest acceptable tile value

    if (pos.posY > 0) {
        MapTile z = map[pos.posX][pos.posY - 1] & LOMASK;
        if (z >= l && z <= h) {
            return true;
        }
    }

    if (pos.posX < (WORLD_W - 1)) {
        MapTile z = map[pos.posX + 1][pos.posY] & LOMASK;
        if (z >= l && z <= h) {
            return true;
        }
    }

    if (pos.posY < (WORLD_H - 1)) {
        MapTile z = map[pos.posX][pos.posY + 1] & LOMASK;
        if (z >= l && z <= h) {
            return true;
        }
    }

    if (pos.posX > 0) {
        MapTile z = map[pos.posX - 1][pos.posY] & LOMASK;
        if (z >= l && z <= h) {
            return true;
        }
    }

    return false;
}


/**
 * Can the given tile be used as road?
 * @param mv Value from the map.
 * @return Indication that you can drive on the given tile
 */
bool Micropolis::roadTest(MapValue mv)
{
    MapTile tile = mv & LOMASK;

    if (tile < ROADBASE || tile > LASTRAIL) {
        return false;
    }

    if (tile >= POWERBASE && tile < LASTPOWER) {
        return false;
    }

    return true;
}


////////////////////////////////////////////////////////////////////////
