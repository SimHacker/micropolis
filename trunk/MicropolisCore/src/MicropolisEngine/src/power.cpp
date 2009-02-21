/* power.cpp
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

/** @file power.cpp */

////////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "micropolis.h"
#include "text.h"

////////////////////////////////////////////////////////////////////////
// Constants

/** Number of tiles that a coal power plant can supply power to. */
static const Quad COAL_POWER_STRENGTH = 700L;

/** Number of tiles that a nuclear power plant can supply power to. */
static const Quad NUCLEAR_POWER_STRENGTH = 2000L;


////////////////////////////////////////////////////////////////////////


/**
 * Move (Micropolis::curMapX, Micropolis::curMapY) in direction \a mDir.
 * @param mDir Direction to move in.
 * @return Movement was succesfull.
 * @note Also silently moves (Micropolis::curMapX, Micropolis::curMapY)
 *       back onto the map in the reverse direction if off-map.
 */
bool Micropolis::moveMapSim(Direction mDir)
{
    switch (mDir) {

        case DIR_NORTH:
            if (curMapY > 0) {
                curMapY--;
                return true;
            }
            if (curMapY < 0) {
                curMapY = 0;
            }
            return false;

        case DIR_WEST:
            if (curMapX < WORLD_W - 1) {
                curMapX++;
                return true;
            }
            if (curMapX > WORLD_W - 1) {
                curMapX = WORLD_W - 1;
            }
            return false;

        case DIR_SOUTH:
            if (curMapY < WORLD_H - 1) {
                curMapY++;
                return true;
            }
            if (curMapY > WORLD_H - 1) {
                curMapY = WORLD_H - 1;
            }
            return false;

        case DIR_EAST:
            if (curMapX > 0) {
                curMapX--;
                return true;
            }
            if (curMapX < 0) {
                curMapX = 0;
            }
            return false;

        default:
            NOT_REACHED();
            return false; // Never reached, but keeps the compiler happy
    }
}


/**
 * Check whether from position (Micropolis::curMapX, Micropolis::curMapY) in the
 * direction \a tfDir for a conducting tile that has no power.
 * @param tfDir Direction to investigate.
 * @return Unpowered tile has been found in the indicated direction.
 */
bool Micropolis::testForConductive(Direction tfDir)
{
    int xsave, ysave;

    xsave = curMapX;
    ysave = curMapY;

    if (moveMapSim(tfDir)) {
        if ((map[curMapX][curMapY] & CONDBIT) == CONDBIT
                            && curTile != NUCLEAR && curTile != POWERPLANT) {
            if (!powerGridMap.getWorld(curMapX, curMapY)) {
                curMapX = xsave;
                curMapY = ysave;
                return true;
            }
        }
    }

    curMapX = xsave;
    curMapY = ysave;

    return false;
}


/**
 * Scan the map for powered tiles, and copy them to the Micropolis::powerGridMap
 * array.
 * Also warns the user about using too much power ('buy another power plant').
 */
void Micropolis::doPowerScan()
{
    short ADir;
    int ConNum, Dir;

    // Clear power map.
    powerGridMap.clear();

    // Power that the combined coal and nuclear power plants can deliver.
    Quad maxPower = coalPowerPop * COAL_POWER_STRENGTH +
                    nuclearPowerPop * NUCLEAR_POWER_STRENGTH;

    Quad numPower = 0; // Amount of power used.

    while (powerStackPointer > 0) {
        pullPowerStack();
        ADir = 4;
        do {
            numPower++;
            if (numPower > maxPower) {
                sendMessage(MESSAGE_NOT_ENOUGH_POWER);
                return;
            }
            if (ADir < 4) {  // ADir == 4 does nothing in moveMapSim()
                moveMapSim((Direction)ADir);
            }
            powerGridMap.setWorld(curMapX, curMapY, 1);
            ConNum = 0;
            Dir = 0;
            while ((Dir < 4) && (ConNum < 2)) {
                if (testForConductive((Direction)Dir)) {
                    ConNum++;
                    ADir = Dir;
                }
                Dir++;
            }
            if (ConNum > 1) {
                pushPowerStack();
            }
        } while (ConNum);
    }
}


/**
 * Push the (Micropolis::curMapX, Micropolis::curMapY) pair onto the power stack.
 * @see powerStackPointer, powerStackX, powerStackY
 */
void Micropolis::pushPowerStack()
{
    if (powerStackPointer < (POWER_STACK_SIZE - 2)) {
        powerStackPointer++;
        powerStackX[powerStackPointer] = curMapX;
        powerStackY[powerStackPointer] = curMapY;
    }
}


/**
 * Pull a position from the power stack and store it in Micropolis::curMapX and
 * Micropolis::curMapY.
 * @see powerStackPointer, powerStackX, powerStackY
 */
void Micropolis::pullPowerStack()
{
    if (powerStackPointer > 0)  {
        curMapX = powerStackX[powerStackPointer];
        curMapY = powerStackY[powerStackPointer];
        powerStackPointer--;
    }
}


////////////////////////////////////////////////////////////////////////
