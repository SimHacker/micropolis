/* position.h
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

/** @file position.h Position handling. */

#ifndef H_POSITION
#define H_POSITION

/** Another direction enumeration class, with 8 possible directions.
 * @todo Eliminate #Direction.
 * @todo After eliminating #Direction, rename this enum to Direction.
 */
enum Direction2 {
    DIR2_INVALID,    ///< Invalid direction.
    DIR2_NORTH,      ///< Direction pointing north.
    DIR2_NORTH_EAST, ///< Direction pointing north-east.
    DIR2_EAST,       ///< Direction pointing east.
    DIR2_SOUTH_EAST, ///< Direction pointing south-east.
    DIR2_SOUTH,      ///< Direction pointing south.
    DIR2_SOUTH_WEST, ///< Direction pointing south-west.
    DIR2_WEST,       ///< Direction pointing west.
    DIR2_NORTH_WEST, ///< Direction pointing north-west.

    DIR2_BEGIN = DIR2_NORTH,        ///< First valid direction.
    DIR2_END = DIR2_NORTH_WEST + 1, ///< End-condition for directions
};

/**
 * Rotate the direction by 45 degrees.
 * @param dir Direction to rotate.
 * @return Rotated direction, possibly >= DIR2_END.
 */
static inline Direction2 rotate45(Direction2 dir)
{
    return (Direction2)(dir + 1);
}

/**
 * Rotate the direction by 90 degrees.
 * @param dir Direction to rotate.
 * @return Rotated direction, possibly >= DIR2_END.
 */
static inline Direction2 rotate90(Direction2 dir)
{
    return (Direction2)(dir + 2);
}


/** X/Y position. */
class Position {

public:

    Position();
    Position(int x, int y);
    Position(const Position &pos);
    Position(const Position &pos, Direction2 dir);
    Position &operator=(const Position &pos);

    bool move(Direction2 dir);
    bool testBounds();

    int posX; ///< Horizontal coordinate of the position.
    int posY; ///< Vertical coordnate of the position.
};

#endif

