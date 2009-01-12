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


////////////////////////////////////////////////////////////////////////

/**
 * Find a connection over a road from the current place to a specified zone type
 * @param dest Zone type to go to
 * @return \c 1 if connection found, \c 0 if not found,
 *         \c -1 if no connection to road found
 */
short Micropolis::MakeTraf(ZoneType dest)
{
    short xtem, ytem;

    xtem = curMapX; // Temporarily save curMapX
    ytem = curMapY; // Temporarily save curMapY
    Zsource = dest;
    PosStackN = 0; // Clear position stack

#if 0
      if ((!getRandom(2)) && FindPTele()) {
        /* printf("Telecommute!\n"); */
        return 1;
      }
#endif

    if (FindPRoad()) {            /* look for road on zone perimeter */

        if (TryDrive()) {           /* attempt to drive somewhere */
            SetTrafMem();             /* if sucessful, inc trafdensity */
            curMapX = xtem;
            curMapY = ytem;
            return 1;            /* traffic passed */
        }

        curMapX = xtem;
        curMapY = ytem;

        return 0;             /* traffic failed */
    } else {
        return -1;             /* no road found */
    }
}


/* comefrom: MakeTraf */
void Micropolis::SetTrafMem()
{
    register short x, z;

    /* For each saved position of the drive */
    for (x = PosStackN; x > 0; x--) {

        PullPos();

        if (TestBounds(curMapX, curMapY)) {

            z = map[curMapX][curMapY] & LOMASK;

            if (z >= ROADBASE && z < POWERBASE) {
                SimSprite *sprite;

                /* check for rail */
                z = trafficDensityMap[curMapX >>1][curMapY >>1];
                z += 50;

                if (z > 240 && getRandom(5) == 0) {

                    z = 240;
                    TrafMaxX = curMapX <<4;
                    TrafMaxY = curMapY <<4;

                    /* Direct helicopter towards heavy traffic */
                    sprite = GetSprite(SPRITE_HELICOPTER);
                    if (sprite != NULL && sprite->control == -1) {

                        sprite->destX = TrafMaxX;
                        sprite->destY = TrafMaxY;

                    }
                }

                trafficDensityMap[curMapX >>1][curMapY >>1] = (Byte)z;
            }
        }
    }
}


/** Push current position (curMapX, curMapY) onto position stack */
void Micropolis::PushPos()
{
    PosStackN++;
    curMapXStack[PosStackN] = curMapX;
    curMapYStack[PosStackN] = curMapY;
}


/** Pull top-most position from stack and store in curMapX and curMapY */
void Micropolis::PullPos()
{
    curMapX = curMapXStack[PosStackN];
    curMapY = curMapYStack[PosStackN];
    PosStackN--;
}


/**
 * Find a connection to a road at the perimeter
 * @return Indication that a connection has been found
 * @pre  curMapX and curMapY contain the starting coordinates
 * @post If a connection is found, it is stored in curMapX and curMapY
 */
bool Micropolis::FindPRoad()
{
    /* look for road on edges of zone */
    static const short PerimX[12] = {-1, 0, 1, 2, 2, 2, 1, 0,-1,-2,-2,-2};
    static const short PerimY[12] = {-2,-2,-2,-1, 0, 1, 2, 2, 2, 1, 0,-1};
    short tx, ty;

    for (short z = 0; z < 12; z++) {

        tx = curMapX + PerimX[z];
        ty = curMapY + PerimY[z];

        if (TestBounds(tx, ty)) {

            if (RoadTest(map[tx][ty])) {

                curMapX = tx;
                curMapY = ty;

                return true;
            }
        }
    }

    return false;
}


/**
 * Find a telecomm connection at the perimeter
 * @return Indication that a telecomm connection has been found
 * @pre  curMapX and curMapY contain the starting coordinates
 */
bool Micropolis::FindPTele()
{
    /* look for telecommunication on edges of zone */
    static const short PerimX[12] = {-1, 0, 1, 2, 2, 2, 1, 0,-1,-2,-2,-2};
    static const short PerimY[12] = {-2,-2,-2,-1, 0, 1, 2, 2, 2, 1, 0,-1};
    short tx, ty, tile;

    for (short z = 0; z < 12; z++) {

        tx = curMapX + PerimX[z];
        ty = curMapY + PerimY[z];

        if (TestBounds(tx, ty)) {

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
 * @return Was drive succesful?
 * @post Position stack (curMapXStack, curMapYStack, PosStackN)
 *       is filled with some intermediate positions of the drive
 * @todo Find out why the stack is popped, but curMapX and sMapY are not updated
 */
bool Micropolis::TryDrive()
{
    short dist;

    LDir = DIR_INVALID;
    for (dist = 0; dist < MAX_TRAFFIC_DISTANCE; dist++) {  /* Maximum distance to try */

        if (tryGo(dist)) {                /* if it got a road */

            if (DriveDone()) {              /* if destination is reached */
                return true;                  /* pass */
            }

        } else {

            if (PosStackN > 0) {            /* deadend , backup */
                PosStackN--;
                dist += 3;
            } else {
                return false;                 /* give up at start  */
            }

        }
    }

    return false;                       /* gone maxdis */
}


/* comefrom: TryDrive */
bool Micropolis::tryGo(int dist)
{
    short x, rdir;
    Direction realdir;

#if 0
    rdir = getRandom(3); /* XXX: Heaviest user of Rand */
#else
    rdir = getRandom16() & 3;
#endif

    for (x = rdir; x < (rdir + 4); x++) { /* for the 4 directions */

        realdir = (Direction)(x & 3);

        if (realdir == LDir) {
            continue; /* skip last direction */
        }

        if (RoadTest(GetFromMap(realdir))) {
            moveMapSim(realdir);
            LDir = ReverseDirection(realdir);

            if (dist & 1) {
                /* save pos every other move */
                PushPos();
            }

            return true;
        }
    }

    return false;
}


/**
 * Get neighbouring tile from the map.
 * @param d Direction of neighbouring tile
 * @return The tile in the indicated direction. If tile is off-world or an
 *         incorrect direction is given, \c DIRT is returned.
 */
short Micropolis::GetFromMap(Direction d)
{
    switch (d) {

        case DIR_NORTH:
            if (curMapY > 0) {
              return map[curMapX][curMapY - 1] & LOMASK;
            }

            return DIRT;

        case DIR_WEST:
            if (curMapX < (WORLD_X - 1)) {
              return map[curMapX + 1][curMapY] & LOMASK;
            }

            return DIRT;

        case DIR_SOUTH:
            if (curMapY < (WORLD_Y - 1)) {
              return map[curMapX][curMapY + 1] & LOMASK;
            }

            return DIRT;

        case DIR_EAST:
            if (curMapX > 0) {
              return map[curMapX - 1][curMapY] & LOMASK;
            }

            return DIRT;

        default:
            return DIRT;

    }
}


/**
 * Has the journey arrived at its destination?
 * @return Indication that destination has been reached
 * @pre Zsource contains the zone type to drive to
 */
bool Micropolis::DriveDone()
{
    /* commercial, industrial, residential destinations */
    static const short TARGL[3] = {COMBASE, LHTHR, LHTHR};
    static const short TARGH[3] = {NUCLEAR, PORT, COMBASE};

    assert(ZT_NUM_DESTINATIONS == LENGTH_OF(TARGL));
    assert(ZT_NUM_DESTINATIONS == LENGTH_OF(TARGH));

    short l = TARGL[Zsource]; // Lowest acceptable tile value
    short h = TARGH[Zsource]; // Highest acceptable tile value

    if (curMapY > 0) {
        short z = map[curMapX][curMapY - 1] & LOMASK;
        if ((z >= l) && (z <= h)) {
            return true;
        }
    }

    if (curMapX < (WORLD_X - 1)) {
        short z = map[curMapX + 1][curMapY] & LOMASK;
        if ((z >= l) && (z <= h)) {
            return true;
        }
    }

    if (curMapY < (WORLD_Y - 1)) {
        short z = map[curMapX][curMapY + 1] & LOMASK;
        if ((z >= l) && (z <= h)) {
            return true;
        }
    }

    if (curMapX > 0) {
        short z = map[curMapX - 1][curMapY] & LOMASK;
        if ((z >= l) && (z <= h)) {
            return true;
        }
    }

    return false;
}


/**
 * Can the given tile be used as road?
 * @param t Tile
 * @return Indication that you can drive on the given tile
 */
bool Micropolis::RoadTest(int t)
{
    t = t & LOMASK;

    if (t < ROADBASE || t > LASTRAIL) {
        return false;
    }

    if (t >= POWERBASE && t < LASTPOWER) {
        return false;
    }

    return true;
}


////////////////////////////////////////////////////////////////////////
