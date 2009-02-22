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
 * Check at position \a pos for a power-less conducting tile in the
 * direction \a testDir.
 * @param pos     Position to start from.
 * @param testDir Direction to investigate.
 * @return Unpowered tile has been found in the indicated direction.
 * @todo Re-use something like Micropolis::getFromMap(), and fold this function
 *       into its caller.
 */
bool Micropolis::testForConductive(const Position& pos, Direction2 testDir)
{
    Position movedPos(pos);

    if (movedPos.move(testDir)) {
        if ((map[movedPos.posX][movedPos.posY] & CONDBIT) == CONDBIT) {
            if (!powerGridMap.worldGet(movedPos.posX, movedPos.posY)) {
                return true;
            }
        }
    }

    return false;
}


/**
 * Scan the map for powered tiles, and copy them to the Micropolis::powerGridMap
 * array.
 * Also warns the user about using too much power ('buy another power plant').
 */
void Micropolis::doPowerScan()
{
    Direction2 anyDir,dir;
    int conNum;

    // Clear power map.
    powerGridMap.clear();

    // Power that the combined coal and nuclear power plants can deliver.
    Quad maxPower = coalPowerPop * COAL_POWER_STRENGTH +
                    nuclearPowerPop * NUCLEAR_POWER_STRENGTH;

    Quad numPower = 0; // Amount of power used.

    while (powerStackPointer > 0) {
        Position pos = pullPowerStack();
        anyDir = DIR2_INVALID;
        do {
            numPower++;
            if (numPower > maxPower) {
                sendMessage(MESSAGE_NOT_ENOUGH_POWER);
                return;
            }
            if (anyDir != DIR2_INVALID) {
                pos.move(anyDir);
            }
            powerGridMap.worldSet(pos.posX, pos.posY, 1);
            conNum = 0;
            dir = DIR2_BEGIN;
            while (dir < DIR2_END && conNum < 2) {
                if (testForConductive(pos, dir)) {
                    conNum++;
                    anyDir = dir;
                }
                dir = increment90(dir);
            }
            if (conNum > 1) {
                pushPowerStack(pos);
            }
        } while (conNum);
    }
}


/**
 * Push position \a pos onto the power stack if there is room.
 * @param pos Position to push.
 */
void Micropolis::pushPowerStack(const Position &pos)
{
    if (powerStackPointer < (POWER_STACK_SIZE - 2)) {
        powerStackPointer++;
        powerStackXY[powerStackPointer] = pos;
    }
}


/**
 * Pull a position from the power stack.
 * @return Pulled position.
 * @pre Stack must be non-empty (powerStackPointer > 0).
 */
Position Micropolis::pullPowerStack()
{
    assert(powerStackPointer > 0);
    powerStackPointer--;
    return powerStackXY[powerStackPointer + 1];
}


////////////////////////////////////////////////////////////////////////
