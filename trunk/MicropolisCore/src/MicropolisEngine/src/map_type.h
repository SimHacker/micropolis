/*
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

/** @file map_type.h Map data structures */

////////////////////////////////////////////////////////////////////////
// Constants

/**
 * Size of the world in horizontal direction.
 */
static const int WORLD_W = 120;

/**
 * Size of the world in vertical direction.
 */
static const int WORLD_H = 100;

////////////////////////////////////////////////////////////////////////
// Template class definitions

/**
 * Generic class for maps in the Micropolis game.
 *
 * A map is assumed to cover a 2D grid of #WORLD_W times #WORLD_H positions.
 * A block of positions may be clustered, and represented by a single data
 * value.
 * @tparam DATA    Data type of a data value.
 * @tparam BLKSIZE Size of the cluster.
 */
template <typename DATA, int BLKSIZE>
class Map
{
public:
    Map(DATA defaultValue);
    ~Map();

    /** Size of a cluster in number of world positions. */
    const int MAP_BLOCKSIZE;
    const int MAP_MAX_X; ///< Number of clusters in horizontal direction.
    const int MAP_MAX_Y; ///< Number of clusters in vertical direction.

    void clear();

    inline void set(int x, int y, DATA val);
    inline DATA get(int x, int y) const;
    inline bool onMap(int x, int y) const;

    inline void worldSet(int x, int y, DATA val);
    inline DATA worldGet(int x, int y) const;
    inline bool worldOnMap(int x, int y) const;

    DATA *getBase();
private:
    /** Data fields of the map in column-major mode. */
    DATA _mapData[((WORLD_W + BLKSIZE - 1) / BLKSIZE)
                 * ((WORLD_H + BLKSIZE -1) / BLKSIZE)];

    const DATA _MAP_DEFAULT_VALUE; ///< Default value of a cluster.
};

/**
 * Generic map constructor.
 * @param defaultValue Default value to use for off-map positions, and
 *                     for clearing the map.
 */
template <typename DATA, int BLKSIZE>
Map<DATA, BLKSIZE>::Map(DATA defaultValue): MAP_BLOCKSIZE(BLKSIZE)
            , MAP_MAX_X((WORLD_W + BLKSIZE - 1) / BLKSIZE)
            , MAP_MAX_Y((WORLD_H + BLKSIZE - 1) / BLKSIZE)
            , _MAP_DEFAULT_VALUE(defaultValue)
{
}

/** Generic map destructor */
template <typename DATA, int BLKSIZE>
Map<DATA, BLKSIZE>::~Map()
{
}

/**
 * Generic clear routine.
 *
 * Resets all data of the map to #_MAP_DEFAULT_VALUE.
 */
template <typename DATA, int BLKSIZE>
void Map<DATA, BLKSIZE>::clear()
{
    for (int i = 0; i < this->MAP_MAX_X * this->MAP_MAX_Y; i++) {
        this->_mapData[i] = this->_MAP_DEFAULT_VALUE;
    }
}

/**
 * Return the base address of the map data.
 * @note Data is stored in column-major mode.
 */
template <typename DATA, int BLKSIZE>
DATA *Map<DATA, BLKSIZE>::getBase()
{
    return this->_mapData;
}

/**
 * Set the value of a cluster.
 *
 * If the coordinate is off the map, the value is not stored.
 * @param x     X cluster position (at world position \a x * #MAP_BLOCKSIZE).
 * @param y     Y cluster position (at world position \a y * #MAP_BLOCKSIZE).
 * @param value Value to use.
 */
template <typename DATA, int BLKSIZE>
inline void Map<DATA, BLKSIZE>::set(int x, int y, DATA value)
{
    if(this->onMap(x, y)) {
        this->_mapData[x * MAP_MAX_Y + y] = value;
    }
}

/**
 * Return the value of a cluster.
 *
 * If the coordinate is off the map, the #_MAP_DEFAULT_VALUE is returned.
 * @param x X cluster position (at world position \a x * #MAP_BLOCKSIZE).
 * @param y Y cluster position (at world position \a y * #MAP_BLOCKSIZE).
 * @return Value of the cluster.
 */
template <typename DATA, int BLKSIZE>
inline DATA Map<DATA, BLKSIZE>::get(int x, int y) const
{
    if(!this->onMap(x, y)) {
        return this->_MAP_DEFAULT_VALUE;
    }

    return this->_mapData[x * MAP_MAX_Y + y];
}


/**
 * Verify that cluster coordinates are within map boundaries.
 * @param x X cluster position (at world position \a x * #MAP_BLOCKSIZE).
 * @param y Y cluster position (at world position \a y * #MAP_BLOCKSIZE).
 * @return Coordinate is within map boundaries.
 */
template <typename DATA, int BLKSIZE>
inline bool Map<DATA, BLKSIZE>::onMap(int x, int y) const
{
    return (x >= 0 && x < this->MAP_MAX_X) && (y >= 0 && y < this->MAP_MAX_Y);
}


/**
 * Set the value of a cluster.
 *
 * If the coordinate is off the map, the value is not stored.
 * @param x     X world position.
 * @param y     Y world position.
 * @param value Value to use.
 */
template <typename DATA, int BLKSIZE>
inline void Map<DATA, BLKSIZE>::worldSet(int x, int y, DATA value)
{
    if(this->worldOnMap(x, y)) {
        x /= BLKSIZE;
        y /= BLKSIZE;
        this->_mapData[x * MAP_MAX_Y + y] = value;
    }
}

/**
 * Return the value of a cluster.
 *
 * If the coordinate is off the map, the #_MAP_DEFAULT_VALUE is returned.
 * @param x X world position.
 * @param y Y world position.
 * @return Value of the cluster.
 */
template <typename DATA, int BLKSIZE>
inline DATA Map<DATA, BLKSIZE>::worldGet(int x, int y) const
{
    if(!this->worldOnMap(x, y)) {
        return this->_MAP_DEFAULT_VALUE;
    }

    x /= BLKSIZE;
    y /= BLKSIZE;
    return this->_mapData[x * MAP_MAX_Y + y];
}

/**
 * Verify that world coordinates are within map boundaries.
 * @param x X world position.
 * @param y Y world position.
 * @return Coordinate is within map boundaries.
 */
template <typename DATA, int BLKSIZE>
inline bool Map<DATA, BLKSIZE>::worldOnMap(int x, int y) const
{
    return (x >= 0 && x < WORLD_W) && (y >= 0 && y < WORLD_H);
}

////////////////////////////////////////////////////////////////////////
// Type definitions

typedef Map<Byte, 4> MapByte4;

////////////////////////////////////////////////////////////////////////
