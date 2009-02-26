/* position.cpp
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

/** @file position.cpp Position handling. */

/////////////////////////////////////////////////////////////////////////////

#include "micropolis.h"
#include "position.h"

/////////////////////////////////////////////////////////////////////////////

/** Default constructor. */
Position::Position()
{
    this->posX = 0;
    this->posY = 0;
}

/**
 * Construct a position at a given \a x and \a y coordinate.
 * @param x X coordinate of the new position.
 * @param y Y coordinate of the new position.
 */
Position::Position(int x, int y)
{
    this->posX = x;
    this->posY = y;
}

/**
 * Copy constructor.
 * @param pos Position to copy.
 */
Position::Position(const Position &pos)
{
    this->posX = pos.posX;
    this->posY = pos.posY;
}

/**
 * Copy constructor with a single tile movement.
 * @param pos Position to copy.
 * @param dir Direction to move into.
 */
Position::Position(const Position &pos, Direction2 dir)
{
    this->posX = pos.posX;
    this->posY = pos.posY;
    this->move(dir);
}

/**
 * Copy sonstructor with arbitrary movement.
 * @param pos Position to copy.
 * @param dx  Horizontal offset.
 * @param dy  Vertical offset.
 */
Position::Position(const Position &pos, int dx, int dy)
{
    this->posX = pos.posX + dx;
    this->posY = pos.posY + dy;
}

/**
 * Assignment operator.
 * @param pos Position to copy.
 */
Position &Position::operator=(const Position &pos)
{
    if (this != &pos) {
        this->posX = pos.posX;
        this->posY = pos.posY;
    }
    return *this;
}


/**
 * Move the position one step in the indicated direction.
 * @param dir Direction to move into.
 * @return Position moved in the indicated direction.
 */
bool Position::move(Direction2 dir)
{
    switch (dir) {
        case DIR2_INVALID:
            return true;

        case DIR2_NORTH:
            if (this->posY > 0) {
                this->posY--;
                return true;
            }
            break;

        case DIR2_NORTH_EAST:
            if (this->posX < WORLD_W - 1 && this->posY > 0) {
                this->posX++;
                this->posY--;
                return true;
            }
        case DIR2_EAST:
            if (this->posX < WORLD_W - 1) {
                this->posX++;
                return true;
            }
            break;

        case DIR2_SOUTH_EAST:
            if (this->posX < WORLD_W -1 && this->posY < WORLD_H - 1) {
                this->posX++;
                this->posY++;
                return true;
            }
            break;

        case DIR2_SOUTH:
            if (this->posY < WORLD_H - 1) {
                this->posY++;
                return true;
            }
            break;

        case DIR2_SOUTH_WEST: this->posX--; this->posY++; break;
            if (this->posX > 0 && this->posY < WORLD_H - 1) {
                this->posX--;
                this->posY++;
                return true;
            }
            break;

        case DIR2_WEST:
            if (this->posX > 0) {
                this->posX--;
                return true;
            }
            break;

        case DIR2_NORTH_WEST:
            if (this->posX > 0 && this->posY > 0) {
                this->posX--;
                this->posY--;
                return true;
            }
            break;


        default: NOT_REACHED();
    }

    // Movement was not possible, silently repair the position.
    if (this->posX < 0)        this->posX = 0;
    if (this->posX >= WORLD_W) this->posX = WORLD_W - 1;
    if (this->posY < 0)        this->posY = 0;
    if (this->posY >= WORLD_H) this->posY = WORLD_H - 1;
    return false;
}



/////////////////////////////////////////////////////////////////////////////
