/* tool.h
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

/** @file tool.h */

////////////////////////////////////////////////////////////////////////


#include <map>


////////////////////////////////////////////////////////////////////////
// Forward declarations


class Micropolis;
class Position;


////////////////////////////////////////////////////////////////////////

typedef std::map<Position, MapValue> WorldModificationsMap;

/**
 * Class for storing effects of applying a tool to the world.
 *
 * When applying a tool, two things change:
 *  - The world map.
 *  - The funds of the player.
 *
 * The latter gives a decision problem. To decide whether the tool can be
 * applied, you need to know the cost. To know the cost you need to know the
 * exact changes being made.
 * The simplest way to compute the exact changes is to simply apply the tool to
 * the world. This holds especially when tools get stacked on top of each
 * other.
 *
 * This class provides an easy way out, greatly simplifying the problem.
 * All tools do not modify the world directly, but instead put their results
 * in an instance of this class, thus collecting all the modifications.
 * After the whole operation is 'done', the #ToolEffects instance can tell the
 * precise cost and what has been changed in the world. At that moment, the
 * yes/no decision can be made, and the effects can be copied to the real map
 * and funds.
 *
 * @todo Introduce this class in the connect.cpp file first.
 */
class ToolEffects
{
public:
    ToolEffects(Micropolis *sim);
    ~ToolEffects();

    void clear();
    void modifyWorld();
    bool modifyIfEnoughFunding();

    MapValue getMapValue(const Position& pos) const;
    inline MapValue getMapValue(int x, int y) const;
    inline MapTile getMapTile(const Position& pos) const;
    inline MapTile getMapTile(int x, int y) const;
    inline int getCost() const;

    inline void addCost(int amount);
    void setMapValue(const Position& pos, MapValue mapVal);
    inline void setMapValue(int x, int y, MapValue mapVal);

private:
    Micropolis *sim; ///< Simulator to get map values from, and to apply changes.
    int cost; ///< Accumulated costs.
    WorldModificationsMap modifications; ///< Collected world modifications.
};

////////////////////////////////////////////////////////////////////////

/**
 * Get the tile of a map position.
 * @param pos Position being queried.
 * @return Tile at the specified position.
 *
 * @pre Position must be within map limits
 */
inline MapTile ToolEffects::getMapTile(const Position& pos) const
{
    return this->getMapValue(pos) & LOMASK;
}

/**
 * Get the tile of a map position.
 * @param x Horizontal coordinate of position being queried.
 * @param y Vertical coordinate of position being queried.
 * @return Tile at the specified position.
 *
 * @pre Position must be within map limits
 */
inline MapValue ToolEffects::getMapTile(int x, int y) const
{
    return this->getMapValue(Position(x, y)) & LOMASK;
}
/**
 * Get the total cost collected so far.
 * @return Total cost.
 */
inline int ToolEffects::getCost() const
{
    return this->cost;
}

/**
 * Add some amount to the total.
 */
inline void ToolEffects::addCost(int amount)
{
    assert(amount >= 0); // To be on the safe side.
    this->cost += amount;
}

/**
 * Get the value of a map position.
 * @param x Horizontal coordinate of position being queried.
 * @param y Vertical coordinate of position being queried.
 * @return Map value at the specified position.
 *
 * @pre Position must be within map limits
 */
inline MapValue ToolEffects::getMapValue(int x, int y) const
{
    return this->getMapValue(Position(x, y));
}

/**
 * Set a new map value.
 * @param pos    Position to set.
 * @param x      Horizontal coordinate of position to set.
 * @param y      Vertical coordinate of position to set.
 * @param mapVal Value to set.
 */
inline void ToolEffects::setMapValue(int x, int y, MapValue mapVal)
{
    this->setMapValue(Position(x, y), mapVal);
}

////////////////////////////////////////////////////////////////////////

