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

/** @file traffic.cpp Traffic generation. */

////////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "micropolis.h"


////////////////////////////////////////////////////////////////////////


/**
 * Find a connection over a road from \a startPos to a specified zone type.
 * @param startPos Start position of the attempt.
 * @param dest     Zone type to go to.
 * @return \c 1 if connection found, \c 0 if not found,
 *         \c -1 if no connection to road found.
 */
short Micropolis::makeTraffic(const Position &startPos, ZoneType dest)
{
    curMapStackPointer = 0; // Clear position stack

    Position pos(startPos);

#if 0
      if ((!getRandom(2)) && findPerimeterTelecom(pos)) {
        /* printf("Telecom!\n"); */
        return 1;
      }
#endif

    if (findPerimeterRoad(&pos)) { /* look for road on zone perimeter */

        if (tryDrive(pos, dest)) { /* attempt to drive somewhere */
            addToTrafficDensityMap(); /* if sucessful, inc trafdensity */
            return 1;             /* traffic passed */
        }

        return 0;                 /* traffic failed */
    } else {
        return -1;                /* no road found */
    }
}


/**
 * Update the #trafficDensityMap from the positions at the #curMapStackXY stack.
 */
void Micropolis::addToTrafficDensityMap()
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
 * Find a telecom connection at the perimeter.
 * @param pos Position to start searching.
 * @return A telecom connection has been found.
 *
 * @todo Decide whether we want telecomm code.
 */
bool Micropolis::findPerimeterTelecom(const Position &pos)
{
    /* look for telecom on edges of zone */
    static const short PerimX[12] = {-1, 0, 1, 2, 2, 2, 1, 0,-1,-2,-2,-2};
    static const short PerimY[12] = {-2,-2,-2,-1, 0, 1, 2, 2, 2, 1, 0,-1};
    short tx, ty, tile;

    for (short z = 0; z < 12; z++) {

        tx = pos.posX + PerimX[z];
        ty = pos.posX + PerimY[z];

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
 * @param startPos Starting position.
 * @param destZone Zonetype to drive to.
 * @return Was drive succesful?
 * @post Position stack (curMapStackXY) is filled with some intermediate
 *       positions of the drive.
 *
 * @bug The stack is popped, but position is not updated.
 */
bool Micropolis::tryDrive(const Position &startPos, ZoneType destZone)
{
    Direction2 dirLast = DIR2_INVALID;
    Position drivePos(startPos);

    /* Maximum distance to try */
    for (short dist = 0; dist < MAX_TRAFFIC_DISTANCE; dist++) {

        if (tryGo(&drivePos, &dirLast, dist)) { /* if it got a road */

            if (driveDone(drivePos, destZone)) { /* if destination is reached */
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
 * @param pos     Current position. Updated by function.
 * @param dirLast Last direction traveled in. Updated by the function.
 * @param dist    Distance traveled.
 * @return A move has been made.
 */
bool Micropolis::tryGo(Position *pos, Direction2 *dirLast, int dist)
{
    Direction2 dirReal;

    // Initialize to a random direction.
    switch(getRandom16() & 3) {
        case 0: dirReal = DIR2_NORTH; break;
        case 1: dirReal = DIR2_EAST; break;
        case 2: dirReal = DIR2_SOUTH; break;
        case 3: dirReal = DIR2_WEST; break;
        default:
            NOT_REACHED();
            dirReal = DIR2_NORTH; // to keep the compiler happy.
    }

    for (int i = 0; i < 4; i++) { /* for the 4 directions */

        dirReal = rotate90(dirReal); // Rotate direction.

        if (dirReal == *dirLast) {
            continue; /* skip last direction */
        }

        if (roadTest(getFromMap(*pos, dirReal))) {
            pos->move(dirReal); // Found road, move forward.
            *dirLast = rotate180(dirReal); // Don't allow moving back.

            if (dist & 1) {
                /* Save pos every other move.
                 * This also relates to Micropolis::trafficDensityMap::MAP_BLOCKSIZE
                 */
                pushPos(*pos);
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
MapTile Micropolis::getFromMap(const Position &pos, Direction2 dir)
{
    switch (dir) {

        case DIR2_NORTH:
            if (pos.posY > 0) {
              return map[pos.posX][pos.posY - 1] & LOMASK;
            }

            return DIRT;

        case DIR2_EAST:
            if (pos.posX < WORLD_W - 1) {
              return map[pos.posX + 1][pos.posY] & LOMASK;
            }

            return DIRT;

        case DIR2_SOUTH:
            if (pos.posY < WORLD_H - 1) {
              return map[pos.posX][pos.posY + 1] & LOMASK;
            }

            return DIRT;

        case DIR2_WEST:
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
