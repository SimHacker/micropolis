/* micropolis.h
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

/** @file micropolis.h
 * Micropolis include file
 */

////////////////////////////////////////////////////////////////////////
// Includes


#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <math.h>
#include <cstdarg>

#ifdef _WIN32

#include <winsock2.h>
#include <sys/stat.h>
#include <time.h>

#ifndef S_ISDIR
#define S_ISDIR(x) (((x) & S_IFMT) == S_IFDIR)
#endif

#else

#ifdef __APPLE__
#include <sys/stat.h>
#endif

#include <unistd.h>
#include <sys/time.h>
#include <sys/file.h>
#include <sys/types.h>

#endif

#include <string>
#include <vector>
#include <map>

#include "data_types.h"
#include "map_type.h"
#include "position.h"

////////////////////////////////////////////////////////////////////////
// Definitions


/**
 * Used to determine byte order.
 *
 * @todo Determine byte order a better way.
 */
#define IS_INTEL                        1

/**
 * The version number of Micropolis.
 */
#define MICROPOLIS_VERSION              "5.0"


////////////////////////////////////////////////////////////////////////
// Constants


/**
 * The number of bits per tile.
 * @todo Only used in python/micropolisdrawingarea.py
 */
static const int BITS_PER_TILE = 16;

/**
 * The number of bytes per tile.
 * @todo Only used in python/micropolisdrawingarea.py
 */
static const int BYTES_PER_TILE = 2;

/**
 * Horizontal size of the world for a map that stores a value for every 2x2
 * square.
 * @todo Make a Map class that keeps its 2x2 square storage details internally,
 *       so the code doesn't need to bother with it.
 */
static const int WORLD_W_2 = WORLD_W / 2;

/**
 * Vertical size of the world for a map that stores a value for every 2x2
 * square.
 */
static const int WORLD_H_2 = WORLD_H / 2;

/**
 * Horizontal size of the world for a map that stores a value for every 4x4
 * square.
 * @todo Make a Map class that keeps its 4x4 square storage details internally,
 *       so the code doesn't need to bother with it.
 */
static const int WORLD_W_4 = WORLD_W / 4;

/**
 * Vertical size of the world for a map that stores a value for every 4x4
 * square.
 */
static const int WORLD_H_4 = WORLD_H / 4;

/**
 * Horizontal size of the world for a map that stores a value for every 8x8
 * square.
 * @todo Make a Map class that keeps its 8x8 square storage details internally,
 *       so the code doesn't need to bother with it.
 */
static const int WORLD_W_8 = WORLD_W / 8;

/**
 * Vertical size of the world for a map that stores a value for every 8x8
 * square.
 */
static const int WORLD_H_8 = (WORLD_H + 7) / 8;

/**
 * The size of the editor view tiles, in pixels.
 */
static const int EDITOR_TILE_SIZE = 16;

/**
 * The number of history entries.
 */
static const int HISTORY_LENGTH = 480;

/**
 * The number of miscellaneous history entries.
 */
static const int MISC_HISTORY_LENGTH = 240;

/**
 * Length of the history tables.
 * @todo It is not really a count of histories, rename to something else?
 */
static const int HISTORY_COUNT = 120;

/**
 * The size of the power stack.
 */
static const int POWER_STACK_SIZE = (WORLD_W * WORLD_H) / 4;


/**
 * A constant used in place of an x or y position to indicate
 * "nowhere".
 */
static const int NOWHERE = -1;


///////////////////////////////////////////////////
// Traffic

/**
 * Maximal number of map tiles to drive, looking for a destination
 */
static const int MAX_TRAFFIC_DISTANCE = 30;

/**
 * Maximal value of Micropolis::roadEffect
 */
static const int MAX_ROAD_EFFECT = 32;

/**
 * Maximal value of Micropolis::policeEffect
 */
static const int MAX_POLICE_STATION_EFFECT = 1000;

/**
 * Maximal value of Micropolis::fireEffect
 */
static const int MAX_FIRE_STATION_EFFECT = 1000;


////////////////////////////////////////////////////////////////////////
// Valves


static const int RES_VALVE_RANGE = 2000;
static const int COM_VALVE_RANGE = 1500;
static const int IND_VALVE_RANGE = 1500;


////////////////////////////////////////////////////////////////////////
// Enumerated types.


/**
 * Available types of historic data.
 */
enum HistoryType {
    HISTORY_TYPE_RES,   ///< Residiential history type
    HISTORY_TYPE_COM,   ///< Commercial history type
    HISTORY_TYPE_IND,   ///< Industry history type
    HISTORY_TYPE_MONEY, ///< Money history type
    HISTORY_TYPE_CRIME, ///< Crime history type
    HISTORY_TYPE_POLLUTION, ///< Pollution history type

    HISTORY_TYPE_COUNT,  ///< Number of history types
};

/**
 * Available historic scales.
 */
enum HistoryScale {
    HISTORY_SCALE_SHORT, ///< Short scale data (10 years)
    HISTORY_SCALE_LONG,  ///< Long scale data (120 years)

    HISTORY_SCALE_COUNT, ///< Number of history scales available
};

/**
 * Available map types.
 */
enum MapType {
    MAP_TYPE_ALL,                   ///< All zones
    MAP_TYPE_RES,                   ///< Residential zones
    MAP_TYPE_COM,                   ///< Commercial zones
    MAP_TYPE_IND,                   ///< Industrial zones
    MAP_TYPE_POWER,                 ///< Power connectivity
    MAP_TYPE_ROAD,                  ///< Roads
    MAP_TYPE_POPULATION_DENSITY,    ///< Population density
    MAP_TYPE_RATE_OF_GROWTH,        ///< Rate of growth
    MAP_TYPE_TRAFFIC_DENSITY,       ///< Traffic
    MAP_TYPE_POLLUTION,             ///< Pollution
    MAP_TYPE_CRIME,                 ///< Crime rate
    MAP_TYPE_LAND_VALUE,            ///< Land value
    MAP_TYPE_FIRE_RADIUS,           ///< Fire station coverage radius
    MAP_TYPE_POLICE_RADIUS,         ///< Police station coverage radius
    MAP_TYPE_DYNAMIC,               ///< Dynamic filter

    MAP_TYPE_COUNT,                 ///< Number of map types
};

/* Object & Sound Numbers */

enum SpriteType {
    SPRITE_NOTUSED = 0,

    SPRITE_TRAIN, ///< Train sprite
    SPRITE_HELICOPTER, ///< Helicopter sprite
    SPRITE_AIRPLANE, ///< Airplane sprite
    SPRITE_SHIP, ///< Ship
    SPRITE_MONSTER, ///< Scary monster
    SPRITE_TORNADO, ///< Tornado sprite
    SPRITE_EXPLOSION, ///< Explosion sprite
    SPRITE_BUS, ///< Bus sprite

    SPRITE_COUNT, ///< Number of sprite objects
};


///////////////////////////////////////////////////
// Tiles

/** Value of a tile in the map array incuding the #MapTileBits. */
typedef unsigned short MapValue;

/**
 * Value of a tile in the map array excluding the #MapTileBits (that is, just
 * a value from #MapCharacters).
 */
typedef unsigned short MapTile;

/**
 * Status bits of a map tile.
 * @see MapTile MapCharacters MapTile MapValue
 * @todo #ALLBITS should end with MASK.
 * @todo Decide what to do with #ANIMBIT (since sim-backend may not be the
 *       optimal place to do animation).
 * @todo How many of these bits can be derived from the displayed tile?
 */
enum MapTileBits {
    PWRBIT  = 0x8000, ///< bit 15, tile has power.
    CONDBIT = 0x4000, ///< bit 14. tile can conduct electricity.
    BURNBIT = 0x2000, ///< bit 13, tile can be lit.
    BULLBIT = 0x1000, ///< bit 12, tile is bulldozable.
    ANIMBIT = 0x0800, ///< bit 11, tile is animated.
    ZONEBIT = 0x0400, ///< bit 10, tile is the center tile of the zone.

    /// Mask for the bits-part of the tile
    ALLBITS = ZONEBIT | ANIMBIT | BULLBIT | BURNBIT | CONDBIT | PWRBIT,
    LOMASK = 0x03ff, ///< Mask for the #MapTileCharacters part of the tile

    BLBNBIT   = BULLBIT | BURNBIT,
    BLBNCNBIT = BULLBIT | BURNBIT | CONDBIT,
    BNCNBIT   =           BURNBIT | CONDBIT,
};


/**
 * Connect tile commands.
 */
enum ConnectTileCommand {
    CONNECT_TILE_FIX, ///< Fix zone (connect wire, road, and rail).
    CONNECT_TILE_BULLDOZE, ///< Bulldoze and fix zone.
    CONNECT_TILE_ROAD, ///< Lay road and fix zone.
    CONNECT_TILE_RAILROAD, ///< Lay rail and fix zone.
    CONNECT_TILE_WIRE, ///< Lay wire and fix zone.
};


/**
 * Tool result.
 * @todo Make the rest of the code use this instead of magic numbers.
 */
enum ToolResult {
    TOOLRESULT_NO_MONEY = -2,  ///< User has not enough money for tool.
    TOOLRESULT_NEED_BULLDOZE = -1, ///< Clear the area first.
    TOOLRESULT_FAILED = 0, ///< Cannot build here.
    TOOLRESULT_OK = 1, ///< Build succeeded.
};


/**
 * Characters of the map tiles, the lower 10 bits (0--9).
 * @see LOMASK MapTileBitmasks
 * @todo Add TILE_ prefix
 * @todo Make LOW/BASE and LAST/HIGH consistent everywhere?
 * @todo Figure out what sprite groups really exist (maybe we can learn more by
 *       examining the actual sprites, and/or by using hexadecimal or bite-wise
 *       notation?)
 * @todo Add comments for each sprite (0--1023)
 */
enum MapTileCharacters {
    DIRT           = 0, ///< Clear tile
    // tile 1 ?

    /* Water */
    RIVER          = 2,
    REDGE          = 3,
    CHANNEL        = 4,
    FIRSTRIVEDGE   = 5,
    // tile 6 -- 19 ?
    LASTRIVEDGE    = 20,
    WATER_LOW      = RIVER,       ///< First water tile
    WATER_HIGH     = LASTRIVEDGE, ///< Last water tile (inclusive)

    TREEBASE       = 21,
    WOODS_LOW      = TREEBASE,
    LASTTREE       = 36,
    WOODS          = 37,
    UNUSED_TRASH1  = 38,
    UNUSED_TRASH2  = 39,
    WOODS_HIGH     = UNUSED_TRASH2, // Why is an 'UNUSED' tile used?
    WOODS2         = 40,
    WOODS3         = 41,
    WOODS4         = 42,
    WOODS5         = 43,

    /* Rubble (4 tiles) */
    RUBBLE         = 44,
    LASTRUBBLE     = 47,

    FLOOD          = 48,
    // tile 49, 50 ?
    LASTFLOOD      = 51,

    RADTILE        = 52, ///< Radio-active contaminated tile

    UNUSED_TRASH3  = 53,
    UNUSED_TRASH4  = 54,
    UNUSED_TRASH5  = 55,

    /* Fire animation (8 tiles) */
    FIRE           = 56,
    FIREBASE       = FIRE,
    LASTFIRE       = 63,

    HBRIDGE        = 64, ///< Horizontal bridge
    ROADBASE       = HBRIDGE,
    VBRIDGE        = 65, ///< Vertical bridge
    ROADS          = 66,
    ROADS2         = 67,
    ROADS3         = 68,
    ROADS4         = 69,
    ROADS5         = 70,
    ROADS6         = 71,
    ROADS7         = 72,
    ROADS8         = 73,
    ROADS9         = 74,
    ROADS10        = 75,
    INTERSECTION   = 76,
    HROADPOWER     = 77,
    VROADPOWER     = 78,
    BRWH           = 79,
    LTRFBASE       = 80, ///< First tile with low traffic
    // tile 81 -- 94 ?
    BRWV           = 95,
    // tile 96 -- 110 ?
    BRWXXX1        = 111,
    // tile 96 -- 110 ?
    BRWXXX2        = 127,
    // tile 96 -- 110 ?
    BRWXXX3        = 143,
    HTRFBASE       = 144, ///< First tile with high traffic
    // tile 145 -- 158 ?
    BRWXXX4        = 159,
    // tile 160 -- 174 ?
    BRWXXX5        = 175,
    // tile 176 -- 190 ?
    BRWXXX6        = 191,
    // tile 192 -- 205 ?
    LASTROAD       = 206,
    BRWXXX7        = 207,

    /* Power lines */
    HPOWER         = 208,
    VPOWER         = 209,
    LHPOWER        = 210,
    LVPOWER        = 211,
    LVPOWER2       = 212,
    LVPOWER3       = 213,
    LVPOWER4       = 214,
    LVPOWER5       = 215,
    LVPOWER6       = 216,
    LVPOWER7       = 217,
    LVPOWER8       = 218,
    LVPOWER9       = 219,
    LVPOWER10      = 220,
    RAILHPOWERV    = 221, ///< Horizontal rail, vertical power
    RAILVPOWERH    = 222, ///< Vertical rail, horizontal power
    POWERBASE      = HPOWER,
    LASTPOWER      = RAILVPOWERH,

    UNUSED_TRASH6  = 223,

    /* Rail */
    HRAIL          = 224,
    VRAIL          = 225,
    LHRAIL         = 226,
    LVRAIL         = 227,
    LVRAIL2        = 228,
    LVRAIL3        = 229,
    LVRAIL4        = 230,
    LVRAIL5        = 231,
    LVRAIL6        = 232,
    LVRAIL7        = 233,
    LVRAIL8        = 234,
    LVRAIL9        = 235,
    LVRAIL10       = 236,
    HRAILROAD      = 237,
    VRAILROAD      = 238,
    RAILBASE       = HRAIL,
    LASTRAIL       = 238,

    ROADVPOWERH    = 239, /* bogus? */

    // Residential zone tiles

    RESBASE        = 240, // Empty residential, tiles 240--248
    FREEZ          = 244, // center-tile of 3x3 empty residential

    HOUSE          = 249, // Single tile houses until 260
    LHTHR          = HOUSE,
    HHTHR          = 260,

    RZB            = 265, // center tile first 3x3 tile residential

    HOSPITAL       = 409, // Center of hospital (tiles 405--413)
    CHURCH         = 418, // Center of church (tiles 414--422)

    // Commercial zone tiles

    COMBASE        = 423, // Empty commercial, tiles 423--431
    // tile 424 -- 426 ?
    COMCLR         = 427,
    // tile 428 -- 435 ?
    CZB            = 436,
    // tile 437 -- 608 ?
    COMLAST        = 609,
    // tile 610, 611 ?

    // Industrial zone tiles.
    INDBASE        = 612, ///< Top-left tile of empty industrial zone.
    INDCLR         = 616, ///< Center tile of empty industrial zone.
    LASTIND        = 620, ///< Last tile of empty industrial zone.

    // Industrial zone population 0, value 0: 621 -- 629
    IND1           = 621, ///< Top-left tile of first non-empty industry zone.
    IZB            = 625, ///< Center tile of first non-empty industry zone.

    // Industrial zone population 1, value 0: 630 -- 638

    // Industrial zone population 2, value 0: 639 -- 647
    IND2           = 641,
    IND3           = 644,

    // Industrial zone population 3, value 0: 648 -- 656
    IND4           = 649,
    IND5           = 650,

    // Industrial zone population 0, value 1: 657 -- 665

    // Industrial zone population 1, value 1: 666 -- 674

    // Industrial zone population 2, value 1: 675 -- 683
    IND6           = 676,
    IND7           = 677,

    // Industrial zone population 3, value 1: 684 -- 692
    IND8           = 686,
    IND9           = 689,

    // Seaport
    PORTBASE       = 693, ///< Top-left tile of the seaport.
    PORT           = 698, ///< Center tile of the seaport.
    LASTPORT       = 708, ///< Last tile of the seaport.

    AIRPORTBASE    = 709,
    // tile 710 ?
    RADAR          = 711,
    // tile 712 -- 715 ?
    AIRPORT        = 716,
    // tile 717 -- 744 ?

    // Coal power plant (4x4).
    COALBASE       = 745, ///< First tile of coal power plant.
    POWERPLANT     = 750, ///< 'Center' tile of coal power plant.
    LASTPOWERPLANT = 760, ///< Last tile of coal power plant.

    // Fire station (3x3).
    FIRESTBASE     = 761, ///< First tile of fire station.
    FIRESTATION    = 765, ///< 'Center tile' of fire station.
    // 769 last tile fire station.

    POLICESTBASE   = 770,
    // tile 771 -- 773 ?
    POLICESTATION  = 774,
    // tile 775 -- 778 ?

    // Stadium (4x4).
    STADIUMBASE    = 779, ///< First tile stadium.
    STADIUM        = 784, ///< 'Center tile' stadium.
    // Last tile stadium 794.

    // tile 785 -- 799 ?
    FULLSTADIUM    = 800,
    // tile 801 -- 810 ?

    // Nuclear power plant (4x4).
    NUCLEARBASE    = 811, ///< First tile nuclear power plant.
    NUCLEAR        = 816, ///< 'Center' tile nuclear power plant.
    LASTZONE       = 826, ///< Also last tile nuclear power plant.

    LIGHTNINGBOLT  = 827,
    HBRDG0         = 828,
    HBRDG1         = 829,
    HBRDG2         = 830,
    HBRDG3         = 831,
    HBRDG_END      = 832,
    RADAR0         = 832,
    RADAR1         = 833,
    RADAR2         = 834,
    RADAR3         = 835,
    RADAR4         = 836,
    RADAR5         = 837,
    RADAR6         = 838,
    RADAR7         = 839,
    FOUNTAIN       = 840,
    // tile 841 -- 843 ?
    INDBASE2       = 844,
    TELEBASE       = 844,
    // tile 845 -- 850 ?
    TELELAST       = 851,
    SMOKEBASE      = 852,
    // tile 853 -- 859 ?
    TINYEXP        = 860,
    // tile 861 -- 863 ?
    SOMETINYEXP    = 864,
    // tile 865 -- 866 ?
    LASTTINYEXP    = 867,
    // tile 868 -- 882 ?
    TINYEXPLAST    = 883,
    // tile 884 -- 915 ?

    COALSMOKE1     = 916, ///< Chimney animation at coal power plant (2, 0).
    ///< 919 last animation tile for chimney at coal power plant (2, 0).

    COALSMOKE2     = 920, ///< Chimney animation at coal power plant (3, 0).
    ///< 923 last animation tile for chimney at coal power plant (3, 0).

    COALSMOKE3     = 924, ///< Chimney animation at coal power plant (2, 1).
    ///< 927 last animation tile for chimney at coal power plant (2, 1).

    COALSMOKE4     = 928, ///< Chimney animation at coal power plant (3, 1).
    ///< 931 last animation tile for chimney at coal power plant (3, 1).

    FOOTBALLGAME1  = 932,
    // tile 933 -- 939 ?
    FOOTBALLGAME2  = 940,
    // tile 941 -- 947 ?
    VBRDG0         = 948,
    VBRDG1         = 949,
    VBRDG2         = 950,
    VBRDG3         = 951,
    // tile 952 -- 959 ?

    TILE_COUNT     = 960,

    TILE_INVALID   = 1023, ///< Invalid tile (not used in the world map).
};

/**
 * Available tools.
 *
 * These describe the wand values, the object dragged around on the screen.
 */
enum EditingTool {
    TOOL_RESIDENTIAL,
    TOOL_COMMERCIAL,
    TOOL_INDUSTRIAL,
    TOOL_FIRESTATION,
    TOOL_POLICESTATION,
    TOOL_QUERY,
    TOOL_WIRE,
    TOOL_BULLDOZER,
    TOOL_RAILROAD,
    TOOL_ROAD,
    TOOL_STADIUM,
    TOOL_PARK,
    TOOL_SEAPORT,
    TOOL_COALPOWER,
    TOOL_NUCLEARPOWER,
    TOOL_AIRPORT,
    TOOL_NETWORK,
    TOOL_WATER,
    TOOL_LAND,
    TOOL_FOREST,

    TOOL_COUNT,
    TOOL_FIRST = TOOL_RESIDENTIAL,
    TOOL_LAST = TOOL_FOREST,
};


///////////////////////////////////////////////////
// Scenarios

/** Available scenarios */
enum Scenario {
    SC_NONE,           ///< No scenario (free playing)

    SC_DULLSVILLE,     ///< Dullsville (boredom)
    SC_SAN_FRANCISCO,  ///< San francisco (earthquake)
    SC_HAMBURG,        ///< Hamburg (fire bombs)
    SC_BERN,           ///< Bern (traffic)
    SC_TOKYO,          ///< Tokyo (scary monster)
    SC_DETROIT,        ///< Detroit (crime)
    SC_BOSTON,         ///< Boston (nuclear meltdown)
    SC_RIO,            ///< Rio (flooding)

    SC_COUNT,          ///< Number of scenarios
};


///////////////////////////////////////////////////
// Zones

/** Available zone types */
enum ZoneType {
    ZT_COMMERCIAL,       ///< Commercial zone
    ZT_INDUSTRIAL,       ///< Industrial zone
    ZT_RESIDENTIAL,      ///< Residential zone

    ZT_NUM_DESTINATIONS, ///< Number of available zones
};


///////////////////////////////////////////////////
// City problems

/**
 * Problems in the city where citizens vote on
 * @todo Eliminate PROBNUM
 */
enum CityVotingProblems {
    CVP_CRIME,                    ///< Crime
    CVP_POLLUTION,                ///< Pollution
    CVP_HOUSING,                  ///< Housing
    CVP_TAXES,                    ///< Taxes
    CVP_TRAFFIC,                  ///< Traffic
    CVP_UNEMPLOYMENT,             ///< Unemployment
    CVP_FIRE,                     ///< Fire

    CVP_NUMPROBLEMS,              ///< Number of problems

    CVP_PROBLEM_COMPLAINTS = 4,   ///< Number of problems to complain about.

    PROBNUM = 10,
};

/** Available classes of cities */
enum CityClass {
    CC_VILLAGE,     ///< Village
    CC_TOWN,        ///< Town, > 2000 citizens
    CC_CITY,        ///< City, > 10000 citizens
    CC_CAPITAL,     ///< Capital, > 50000 citizens
    CC_METROPOLIS,  ///< Metropolis, > 100000 citizens
    CC_MEGALOPOLIS, ///< Megalopolis, > 500000 citizens

    CC_NUM_CITIES,  ///< Number of city classes
};

/** Game levels */
enum GameLevel {
    LEVEL_EASY,   ///< Simple game level
    LEVEL_MEDIUM, ///< Intermediate game level
    LEVEL_HARD,   ///< Difficult game level

    LEVEL_COUNT,  ///< Number of game levels

    LEVEL_FIRST = LEVEL_EASY, ///< First game level value
    LEVEL_LAST  = LEVEL_HARD, ///< Last game level value
};

////////////////////////////////////////////////////////////////////////
// Inline functions


/**
 * Compute minimal value.
 * @param a First value.
 * @param b Second value.
 * @return Minimum of \a a and \a b.
 */
template <typename T>
static inline T min(const T a, const T b)
{
    return (a < b) ? a : b;
}

/**
 * Compute maximal value.
 * @param a First value.
 * @param b Second value.
 * @return Maximum of \a a and \a b.
 */
template <typename T>
static inline T max(const T a, const T b)
{
    return (a > b) ? a : b;
}

/**
 * Clamp value between lower and upper bound.
 * @param val   Value.
 * @param lower Lower bound (inclusive).
 * @param upper Upper bound (inclusive).
 * @return \a val, limited to the range given by the lower and upper bound.
 */
template <typename T>
static inline T clamp(const T val, const T lower, const T upper)
{
    if (val < lower) {
        return lower;
    }
    if (val > upper) {
        return upper;
    }
    return val;
}

/**
 * Compute absolute value.
 * @param val Input value.
 * @return Absolute value of \a val.
 */
template <typename T>
static inline T absoluteValue(const T val)
{
  if (val < 0) {
        return -val;
    }
    return val;
}

////////////////////////////////////////////////////////////////////////
// Macros


/**
 * Compute length of array
 * @param array Array to get length from
 * @note Macro only works for statically allocated arrays
 */
#define LENGTH_OF(array) (sizeof(array) / sizeof((array)[0]))

/**
 * Give a fatal error and exit.
 */
#define NOT_REACHED() not_reached(__LINE__, __FILE__)


/**
 * Give a fatal error and exit the program.
 * @param line  Source line number of the fatal error.
 * @param fname Filename of the source file.
 * @return Does not return to the caller.
 * @todo Generalize error handling/exiting of the simulator.
 */
static inline void not_reached(int line, const char *fname)
{
    fprintf(stderr, "Code unexpectedly executed at line %d, at file %s\n", line, fname);
    exit(1);
}


////////////////////////////////////////////////////////////////////////
// Forward class definitions


class Micropolis;
class ToolEffects;
class BuildingProperties;


////////////////////////////////////////////////////////////////////////
// Typedefs


// This is the signature of the scripting language independent
// callback function.
typedef void (*CallbackFunction)(
    Micropolis *micropolis,
    void *data,
    const char *name,
    const char *params,
    va_list arglist);


////////////////////////////////////////////////////////////////////////
// Classes


/** Resource of the game (a file with data loaded in memory). */
class Resource {

public:

    char *buf; ///< Pointer to loaded file data.
    Quad size; ///< Size of the loaded file data.
    char name[4]; ///< Name of the resource (not zero-terminated).
    Quad id; ///< Identification of the resource.
    Resource *next; ///< Pointer to next #Resource.
};


/** Table of strings. */
class StringTable {

public:

    Quad id; ///< Identification of the string table.
    int lines; ///< Number of lines in the table.
    char **strings; ///< Array of pointers to start of each line.
    StringTable *next; ///< Pointer to next #StringTable.
};


/** Sprite in the simulator.
 * @todo SimSprite::name is never set to anything else than \c "", and only
 *       used to detect a non-removed non-active sprite (in a non-robust way).
 */
class SimSprite {

public:

    SimSprite *next; ///< Pointer to next #SimSprite object in the list.
    char *name; ///< Name of the sprite.
    int type; ///< Type of the sprite (TRA -- BUS).
    int frame; ///< Frame (\c 0 means non-active sprite)
    int x; ///< X coordinate of the sprite in pixels?
    int y; ///< Y coordinate of the sprite in pixels?
    int width;
    int height;
    int xOffset;
    int yOffset;
    int xHot; ///< Offset of the hot-spot relative to SimSprite::x?
    int yHot; ///< Offset of the hot-spot relative to SimSprite::y?
    int origX;
    int origY;
    int destX; ///< Destination X coordinate of the sprite.
    int destY; ///< Destination Y coordinate of the sprite.
    int count;
    int soundCount;
    int dir;
    int newDir;
    int step;
    int flag;
    int control;
    int turn;
    int accel;
    int speed;
};


/**
 * Main simulator class
 * @todo Modify Micropolis::roadPercent, Micropolis::policePercent, and
 *       Micropolis::firePercent to hold real percentage from \c 0 to \c 100
 *       instead of a floating point fraction
 * @todo Micropolis::crimeMaxX and Micropolis::crimeMaxY seem unused.
 */
class Micropolis {


public:


    ////////////////////////////////////////////////////////////////////////


    Micropolis();

    ~Micropolis();

private:

    void init();

    void destroy();


    ////////////////////////////////////////////////////////////////////////
    // allocate.cpp

public:


    /**
     * Number of road tiles in the game.
     *
     * Bridges count as 4 tiles, and high density traffic counts as
     * 2 tiles.
     */
    short roadTotal;

    /**
     * Total number of rails.
     *
     * No penalty for bridges or high traffic density.
     */
    short railTotal;

    /**
     * Number of fires.
     */
    short firePop;

    /**
     * Number of people in the residential zones.
     *
     * Depends on level of zone development.
     */
    short resPop;

    /**
     * Commercial zone population.
     *
     * Depends on level of zone development.
     */
    short comPop;

    /**
     * Industrial zone population.
     *
     * Depends on level of zone development.
     */
    short indPop;

    /**
     * Total population.
     *
     * Includes residential pop / 8 plus industrial pop plus commercial
     * pop.
     */
    short totalPop;

    /**
     * Last total population.
     *
     * Not used?
     */
    short totalPopLast;

    short resZonePop; ///< Number of residential zones.
    short comZonePop; ///< Number of commercial zones.
    short indZonePop; ///< Number of industrial zones.

    /**
     * Total zone population.
     */
    short totalZonePop;

    short hospitalPop; ///< Number of hospitals.
    short churchPop; ///< Number of churches.
    short stadiumPop; ///< Number of stadiums.

    /**
     * Police station population.
     */
    short policeStationPop;

    /**
     * Fire station population.
     */
    short fireStationPop;

    /**
     * Coal power plant population.
     */
    short coalPowerPop;

    /**
     * Nuclear power plant population.
     */
    short nuclearPowerPop;

    /**
     * Seaport population.
     */
    short seaportPop;

    /**
     * Airport population.
     */
    short airportPop;

    /**
     * Average crime.
     *
     * Affected by land value, population density, police station
     * distance.
     */
    short crimeAverage;

    /**
     * Average pollution.
     *
     * Affected by PollutionMem, which is effected by traffic, fire,
     * radioactivity, industrial zones, seaports, airports, power
     * plants.
     */
    short pollutionAverage;

    /**
     * Land value average.
     *
     * Affected by distance from city center, development density
     * (terrainMem), pollution, and crime.
     */
    short landValueAverage;

    /** @name Dates */
    //@{

    /**
     * City time tick counter. 48 ticks per year, relative to #startingYear.
     *
     * Four ticks per 12 months, so one tick is about a week (7.6 days).
     */
    Quad cityTime;

    /**
     * City month, 4 ticks per month.
     * @todo Remove, can be computed from Micropolis::cityTime.
     */
    Quad cityMonth;

    /**
     * City year, (Micropolis::cityTime / 48) + Micropolis::startingYear.
     * @todo Remove, can be computed from Micropolis::cityTime.
     */
    Quad cityYear;

    /**
     * City starting year.
     */
    short startingYear;

    //@}

    /* Maps */

    /**
     * Two-dimensional array of map tiles.
     *
     * Map[0 <= x < 120][0 <= y < 100]
     */
    unsigned short *map[WORLD_W];

    /**
     * 10 year residential history maximum value.
     */
    short resHist10Max;

    /**
     * 120 year residential history maximum value.
     */
    short resHist120Max;

    /**
     * 10 year commercial history maximum value.
     */
    short comHist10Max;

    /**
     * 120 year commercial history maximum value.
     */
    short comHist120Max;

    /**
     * 10 year industrial history maximum value.
     */
    short indHist10Max;

    /**
     * 120 year industrial history maximum value.
     */
    short indHist120Max;

    /**
     * Census changed flag.
     *
     * Need to redraw census dependent stuff.
     */
    bool censusChanged;

    /** @name Budget */
    //@{

    /**
     * Spending on roads.
     */
    Quad roadSpend;

    /**
     * Spending on police stations.
     */
    Quad policeSpend;

    /**
     * Spending on fire stations.
     */
    Quad fireSpend;

    /**
     * Requested funds for roads.
     *
     * Depends on number of roads, rails, and game level.
     */
    Quad roadFund;

    /**
     * Requested funds for police stations.
     *
     * Depends on police station population.
     */
    Quad policeFund;

    /**
     * Requested funds for fire stations.
     *
     * Depends on fire station population.
     */
    Quad fireFund;

    /**
     * Ratio of road spending over road funding, times #MAX_ROAD_EFFECT.
     */
    Quad roadEffect;

    /**
     * Ratio of police spending over police funding, times #MAX_POLICE_EFFECT.
     */
    Quad policeEffect;

    /**
     * Ratio of fire spending over fire funding, times #MAX_FIRE_EFFECT.
     */
    Quad fireEffect;

    /**
     * Funds from taxes.
     *
     * Depends on total population, average land value, city tax, and
     * game level.
     */
    Quad taxFund;

    /**
     * City tax rate.
     */
    short cityTax;

    /**
     * Tax port flag.
     *
     * Apparently never used. CollectTax checks it.
     *
     * @todo Apparently taxFlag is never set to true in MicropolisEngine
     *       or the TCL code.
     * @todo Don should check old Mac code to see if it's ever set.
     * @todo Variable is always \c 0. Decide whether to keep it, and if yes,
     *       create means to modify its value
     */
    bool taxFlag;

    //@}

    /** @name Maps */
    //@{

    MapByte2 populationDensityMap; ///< Population density map.
    MapByte2 trafficDensityMap; ///< Traffic density map.
    MapByte2 pollutionDensityMap; ///< Pollution density map.
    MapByte2 landValueMap; ///< Land value map.
    MapByte2 crimeRateMap; ///< Crime rate map.

    /**
     * Terrain development density map.
     *
     * Used to calculate land value.
     */
    MapByte4 terrainDensityMap;

    /**
     * Temporary map 1.
     *
     * Used to smooth population density, pollution.
     */
    MapByte2 tempMap1;

    /**
     * Temporary map 2.
     *
     * Used to smooth population density, pollution.
     */
    MapByte2 tempMap2;

    /**
     * Temporary map 3.
     *
     * Used to smooth development density, for terrainDensityMap.
     */
    MapByte4 tempMap3;

    /**
     * Power grid map.
     */
    MapByte1 powerGridMap;

    /**
     * Rate of growth map.
     *
     * Affected by DecROGMem, incROG called by zones. Decreased by fire
     * explosions from sprites, fire spreading. Doesn't seem to
     * actually feed back into the simulation. Output only.
     */
    MapShort8 rateOfGrowthMap;

    /**
     * Fire station map.
     *
     * Effectivity of fire control in this area.
     *
     * Affected by fire stations, powered, fire funding ratio, road
     * access. Affects how long fires burn.
     * @see #fireEffect
     */
    MapShort8 fireStationMap;

    /**
     * Copy of fire station map to display.
     */
    MapShort8 fireStationEffectMap;

    /**
     * Police station map.
     *
     * Effectivity of police in fighting crime.
     *
     * Affected by police stations, powered, police funding ratio, road
     * access. Affects crime rate.
     * @see #policeEffect
     */
    MapShort8 policeStationMap;

    /**
     * Copy of police station map to display.
     */
    MapShort8 policeStationEffectMap;

    /**
     * Commercial rate map.
     *
     * Depends on distance to city center. Affects commercial zone
     * evaluation.
     */
    MapShort8 comRateMap;

    //@}


    /** @name Historic graphs. */
    //@{

    /**
     * Residential population history.
     */
    short *resHist;

    /**
     * Commercial population history.
     */
    short *comHist;

    /**
     * Industrial population history.
     */
    short *indHist;

    /**
     * Money history.
     */
    short *moneyHist;

    /**
     * Pollution history.
     */
    short *pollutionHist;

    /**
     * Crime history.
     */
    short *crimeHist;

    /**
     * Memory used to save miscelaneous game values in save file.
     */
    short *miscHist;

    //@}


private:

    /**
     * Need hospital?
     *
     * 0 if no, 1 if yes, -1 if too many.
     */
    short needHospital;

    /**
     * Need church?
     *
     * 0 if no, 1 if yes, -1 if too many.
     */
    short needChurch;


    /**
     * Memory for map array.
     */
    unsigned short *mapBase;


    void initMapArrays();

    void destroyMapArrays();


    ////////////////////////////////////////////////////////////////////////
    // animate.cpp

public:


    void animateTiles();


    ////////////////////////////////////////////////////////////////////////
    // budget.cpp

public:


    /**
     * Percentage of requested road and rail costs to funding level.
     *
     * Value between \c 0 and ::MAX_ROAD_EFFECT.
     * Affected by road funds slider and budgetary constraints.
     */
    float roadPercent;

    /**
     * Percentage of requested police station costs to funding level.
     *
     * Value between \c 0 and ::MAX_POLICESTATION_EFFECT.
     * Affected by road funds slider and budgetary constraints.
     */
    float policePercent;

    /**
     * Percentage of requested fire station costs to funding level.
     *
     * Value between \c 0 and ::MAX_FIRESTATION_EFFECT.
     * Affected by road funds slider and budgetary constraints.
     */
    float firePercent;

    /**
     * Amount of road funding granted.
     */
    Quad roadValue;

    /**
     * Amount of police funding granted.
     */
    Quad policeValue;

    /**
     * Amount of fire station funding granted.
     */
    Quad fireValue;

    /**
     * Flag set when budget window needs to be updated.
     */
    int mustDrawBudget;


    void initFundingLevel();

    void doBudget();

    void doBudgetFromMenu();

    void doBudgetNow(bool fromMenu);

    void updateBudget();

    void showBudgetWindowAndStartWaiting();

    void setCityTax(short tax);


    ////////////////////////////////////////////////////////////////////////
    // connect.cpp

private:


    ToolResult connectTile(short x, short y, ConnectTileCommand cmd);
    ToolResult connectTile(short x, short y,
                           ConnectTileCommand cmd, ToolEffects *effects);

    ToolResult layDoze(int x, int y, ToolEffects *effects);

    ToolResult layRoad(int x, int y, ToolEffects *effects);

    ToolResult layRail(int x, int y, ToolEffects *effects);

    ToolResult layWire(int x, int y, ToolEffects *effects);

    void fixZone(int x, int y, ToolEffects *effects);

    void fixSingle(int x, int y, ToolEffects *effects);


    ////////////////////////////////////////////////////////////////////////
    // disasters.cpp

public:


    /**
     * Size of flooding disaster.
     */
    short floodCount;


    void makeMeltdown();

    void makeFireBombs();

    void makeEarthquake();

    void makeFire();

    void makeFlood();

    void setFire();

  private:

    void fireBomb();

    void doDisasters();

    void scenarioDisaster();

    bool vulnerable(int tem);

    void doFlood(const Position &pos);


    ////////////////////////////////////////////////////////////////////////
    // evaluate.cpp

public:


    /**
     * Yes votes.
     *
     * Percentage of people who think the mayor is doing a good job.
     */
    short cityYes;

    /**
     * Problem votes.
     *
     * The number of votes for each problem.
     * @see CityVotingProblems
     */
    short problemVotes[PROBNUM];

    /**
     * Order of taken problems.
     *
     * Contains index of MicropolisEngine::problemVotes of taken problems, in decreasing order.
     * @note Value CVP_NUMPROBLEMS means that the entry is not used
     */
    short problemOrder[CVP_PROBLEM_COMPLAINTS];

    /**
     * City population.
     *
     * Depends of ResPop, ComPop and IndPop.
     */
    Quad cityPop;

    /**
     * Change in the city population.
     *
     * Depends on last cityPop.
     */
    Quad cityPopDelta;

    /**
     * City assessed value.
     *
     * Depends on roadTotal, railTotal, policeStationPop,
     * fireStationPop, hospitalPop, stadiumPop, seaportPop,
     * airportPop, coalPowerPop, and nuclearPowerPop, and their
     * respective values.
     */
    Quad cityAssessedValue;

    CityClass cityClass; ///< City class, affected by city population.

    /**
     * City score.
     *
     * Affected by average of problems, residential cap, commercial cap,
     * industrial cap, road effect, police effect, fire effect,
     * residential valve, commercial valve, industrial valve, city
     * population, delta city population, fires, tax rate, and unpowered
     * zones.
     */
    short cityScore;

    /**
     * Change in the city score.
     *
     * Depends on city score.
     */
    short cityScoreDelta;

    /**
     * Average traffic.
     *
     * Depends on average traffic density of tiles with non-zero land value.
     */
    short trafficAverage;


    void cityEvaluation();

    void evalInit();

    void doScoreCard();

    void changeEval();

    void scoreDoer();

    int countProblems();

    int getProblemNumber(int i);

    int getProblemVotes(int i);


private:

    void getAssessedValue();

    void doPopNum();

    Quad getPopulation();
    CityClass getCityClass(Quad cityPop);

    void doProblems(short problemTable[PROBNUM]);

    void voteProblems(const short problemTable[PROBNUM]);

    short getTrafficAverage();

    short getUnemployment();

    short getFireSeverity();

    void getScore(const short problemTable[PROBNUM]);

    void doVotes();

    ////////////////////////////////////////////////////////////////////////
    // fileio.cpp

public:


    bool loadFileDir(const char *filename, const char *dir);

    bool loadFile(const char *filename);

    bool saveFile(const char *filename);

    void loadScenario(Scenario s);

    void didLoadScenario();

    bool loadCity(const char *filename);

    void didLoadCity();

    void didntLoadCity(const char *msg);

    void saveCity();

    void doSaveCityAs();

    void didSaveCity();

    void didntSaveCity(const char *msg);

    void saveCityAs(const char *filename);


    ////////////////////////////////////////////////////////////////////////
    // generate.cpp

public:

    /** @name Terrain generator.
     * features available incrementally as city building tools.
     *
     * The user should be able to place water and trees, and it should
     * dynamically smooth the edges.
     *
     * The user interface could restrict the user to only drawing
     * terrain before any zones were built, but it would be best if
     * the terrain editing tools worked properly when there were zones
     * built (by automatically bulldozing zones whose underlying
     * terrain it's modifying).
     */
    //@{

    /**
     * Controls the level of tree creation.
     * -1 => create default number of trees, 0 => never create trees, >0 => create more trees
     */
    int terrainTreeLevel;

    /**
     * Controls the level of lake creation.
     * -1 => create default number of lakes, 0 => never create lakes, >0 => create more lakes
     */
    int terrainLakeLevel;

    /**
     * Controls the level of river curviness.
     * -1 => default curve level, 0 => never create rivers, >0 => create curvier rivers
     */
    int terrainCurveLevel;

    /**
     * Controls how often to create an island.
     * -1 => 10% chance of island, 0 => never create island, 1 => always create island
     */
    int terrainCreateIsland;


    void generateNewCity() ;

    void clearMap();

    void clearUnnatural();

    void makeIsland();

    void treeSplash(short xloc, short yloc);

    void makeSingleLake(const Position &pos);

    void plopBRiver(const Position &pos);
    void plopSRiver(const Position &pos);

private:

    void generateSomeCity(int seed);

    void generateMap(int seed);

    void makeNakedIsland();

    void doTrees();

    bool isTree(MapValue cell);

    void smoothTrees();

    void smoothTreesAt(int x, int y, bool preserve);

    void makeLakes();

    void doRivers(const Position &terrainPos);

    Direction2 doBRiver(const Position &riverPos,
                        Direction2 riverDir, Direction2 terrainDir);
    Direction2 doSRiver(const Position &riverPos,
                        Direction2 riverDir, Direction2 terrainDir);

    void smoothRiver();

    void smoothWater();

    void putOnMap(MapValue mChar, short xLoc, short yLoc);

    //@}

    ////////////////////////////////////////////////////////////////////////
    // graph.cpp

public:


    /**
     * Flag that tells if there is a new graph to draw.
     * @todo This should be replaced by a general purpose view updating system.
     */
    bool newGraph;

    /**
     * @todo Nobody uses this variable. Can it be removed?
     */
    bool historyInitialized;

    /**
     * @todo Write-only variable. Can it be removed?
     */
    short graph10Max;

    /**
     * @todo Write-only variable. Can it be removed?
     */
    short graph120Max;


    void drawMonth(short *hist, unsigned char *s, float scale);

    void changeCensus();

    void graphDoer();

    void initGraphs();

    void initGraphMax();

#ifdef SWIG
// This tells SWIG that minValResult, maxValResult are output parameters,
// which will be returned in a tuple of length two.
%apply short *OUTPUT { short *minValResult };
%apply short *OUTPUT { short *maxValResult };
#endif

    void getHistoryRange(int historyType, int historyScale,
                         short *minValResult, short *maxValResult);

    short getHistory(int historyType, int historyScale,
                     int historyIndex);

    void setHistory(int historyType, int historyScale,
                    int historyIndex, short historyValue);


    ////////////////////////////////////////////////////////////////////////
    // initialize.cpp

private:


    void initWillStuff();

    void resetMapState();

    void resetEditorState();


    ////////////////////////////////////////////////////////////////////////
    // main.cpp

public:


    int simLoops;

    /**
     * The number of passes through the simulator loop to take each tick.
     */
    int simPasses;

    /**
     * The count of the current pass through the simulator loop.
     */
    int simPass;

    /**
     * Simulation is paused
     * @todo Variable has reversed logic, maybe use sim_running instead?
     */
    bool simPaused;

    int simPausedSpeed;

    bool tilesAnimated; ///< @todo Not currently used, should hook it up.

    /**
     * Enable animation.
     * @todo Currently always \c true. Should connect it to a frontend option.
     */
    bool doAnimation;

    bool doMessages; ///< @todo Not currently used, should hook it up.

    bool doNotices; ///< @todo Not currently used, should hook it up.


    const char *getMicropolisVersion();

    void simUpdate();

    void simTick();

    void simRobots();

public:

    std::string cityFileName; ///< Filename of the last loaded city

    /**
     * Name of the city.
     * @todo Write-only variable, should it be removed?
     */
    std::string cityName;     ///< Name of the city

private:

    int heatSteps;

    /**
     * @todo Always -7, should this variable be moved or removed?
     */
    int heatFlow;

    int heatRule;

    /**
     * @todo Always 3, should this variable be moved or removed?
     */
    int heatWrap;

    short *cellSrc;

    short *cellDst;


    void environmentInit();

    void simInit();

    void simHeat();

    void simLoop(bool doSim);


#if 0

    ////////////////////////////////////////////////////////////////////////
    // map.cpp
    //
    // Disabled this small map drawing, filtering and overlaying code.
    // Going to re-implement it in the tile engine and Python.

public:


    int dynamicData[32]; // Read-only, it seems


    void drawAll();

    void drawRes();

    void drawCom();

    void drawInd();

    void drawLilTransMap();

    void drawPower();

    bool dynamicFilter(int col, int row);

    void drawDynamic();

    short getCI(short x);

    void drawPopulationDensity();

    void drawRateOfGrowth();

    void drawTrafficDensityMap();

    void drawPollutionDensityMap();

    void drawCrimeRateMap();

    void drawLandValueMap();

    void drawFireRadius();

    void drawPoliceRadius();

    void memDrawMap();

    void ditherMap();

    void maybeDrawRect(
        int val,
        int x,
        int y,
        int w,
        int h);

    void drawRect(
        int pixel,
        int solid,
        int x,
        int y,
        int w,
        int h);

#endif


    ////////////////////////////////////////////////////////////////////////
    // message.cpp

public:


    Quad cityPopLast;   ///< Population of last city class check. @see CheckGrowth
    short categoryLast; ///< City class of last city class check. @see CheckGrowth

    /**
     * Enable auto goto
     *
     * When enabled and an important event happens, the map display will jump to
     * the location of the event
     */
    bool autoGoto;


    void sendMessages();

    void checkGrowth();

    void doScenarioScore(Scenario type);

    void sendMessage(
        short Mnum,
        short x=NOWHERE, short y=NOWHERE,
        bool picture=false, bool important=false);

    void doMakeSound(int mesgNum, int x, int y);

    void doAutoGoto(short x, short y, char *msg);

    void doLoseGame();
    void doWinGame(); ///< @todo This may not be called. Call it when appropriate.


    ////////////////////////////////////////////////////////////////////////
    // power.cpp

private:


    /** @name Power stack
     * Stack used to find powered tiles by tracking conductive tiles.
     */
    //@{

    int powerStackPointer; ///< Stack counter, points to top-most item.

    /** Stack of X/Y positions for traversing setting the power grid. */
    Position powerStackXY[POWER_STACK_SIZE];


    void doPowerScan();

    bool testForConductive(const Position &pos, Direction2 testDir);

    void pushPowerStack(const Position &pos);

    Position pullPowerStack();

    //@}


    ////////////////////////////////////////////////////////////////////////
    // random.cpp

private:


    UQuad nextRandom;


    int simRandom();

    short getRandom(short range);

    int getRandom16();

    int getRandom16Signed();

    short getERandom(short limit);

    void randomlySeedRandom();

    void seedRandom(int seed);


    ////////////////////////////////////////////////////////////////////////
    // resource.cpp

public:


    /** Name of the Micropolis top level home directory. */
    std::string homeDir;

    /** Name of the sub-directory where the resources are located. */
    std::string resourceDir;

    Resource *resources; ///< Linked list of loaded resources
    StringTable *stringTables; ///< Linked list of loaded string tables


    Resource *getResource(const char *name, Quad id);

    void getIndString(char *str, int id, short num);


    ////////////////////////////////////////////////////////////////////////
    // scan.cpp

private:


    /** @todo Write-only variable, can be removed? */
    short newMap;

    /** @todo Write-only variable, can be removed? */
    short newMapFlags[MAP_TYPE_COUNT];

    short cityCenterX; ///< X coordinate of city center
    short cityCenterY; ///< Y coordinate of city center

    short pollutionMaxX; ///< X coordinate of most polluted area
    short pollutionMaxY; ///< Y coordinate of most polluted area

    short crimeMaxX; ///< X coordinate of most criminal area. Not used.
    short crimeMaxY; ///< Y coordinate of most criminal area. Not used.

    /**
     * Integer with bits 0..2 that control smoothing.
     * @todo Variable is always \c 0. Can we delete the variable?
     * @todo Introduce constants for the bits and/or a bool array.
     */
    Quad donDither;


    void fireAnalysis();

    void populationDensityScan();

    int getPopulationDensity(const Position &pos, MapTile tile);

    void pollutionTerrainLandValueScan();

    int getPollutionValue(int loc);

    int getCityCenterDistance(int x, int y);

    void crimeScan();

    void smoothTerrain();

    void doSmooth1();

    void doSmooth2();

    void computeComRateMap();

    ////////////////////////////////////////////////////////////////////////
    // simulate.cpp

public:


    bool valveFlag;

    short crimeRamp;

    short pollutionRamp;

    bool resCap; ///< Block residential growth
    bool comCap; ///< Block commercial growth
    bool indCap; ///< Block industrial growth

    short cashFlow;

    float externalMarket;

    Scenario disasterEvent; ///< The disaster for which a count-down is running
    short disasterWait; ///< Count-down timer for the disaster

    Scenario scoreType; ///< The type of score table to use
    short scoreWait; ///< Time to wait before computing the score

    short poweredZoneCount; ///< Number of powered tiles in all zone
    short unpoweredZoneCount; ///< Number of unpowered tiles in all zones

    bool newPower; /* post */

    short cityTaxAverage;

    short simCycle;

    short phaseCycle;

    short speedCycle;

    bool doInitialEval; ///< Need to perform initial city evaluation.

    int mapSerial; ///< The invalidateMaps method increases the map serial number every time the maps changes.

private:

    short resValve;
    short comValve;
    short indValve;


public:

    void updateFundEffects();


private:

    /** Generate a random animated MapTileCharacters::FIRE tile */
    inline MapValue randomFire()
    {
        return (FIRE + (getRandom16() & 7)) | ANIMBIT;
    };

    /** Generate a random MapTileCharacters::RUBBLE tile */
    inline MapValue randomRubble()
    {
        return (RUBBLE + (getRandom16() & 3)) | BULLBIT;
    };

    void simFrame();

    void simulate(int phase);

    void doSimInit();

    void doNilPower();

    void decTrafficMap();

    void decRateOfGrowthMap();

    void initSimMemory();

    void simLoadInit();

    void setCommonInits();

    void setValves();

    void clearCensus();

    void take10Census();

    void take120Census();

    void collectTax();

    void mapScan(int x1, int x2);

    void doRail(const Position &pos);

    void doRadTile(const Position &pos);

    void doRoad(const Position &pos);

    bool doBridge(const Position &pos, MapTile tile);

    int getBoatDistance(const Position &pos);

    void doFire(const Position &pos);

    void fireZone(const Position &pos, MapValue ch);

    void repairZone(const Position &pos, MapTile zCent, short zSize);

    void doSpecialZone(const Position &pos, bool PwrOn);

    void drawStadium(const Position &center, MapTile z);

    void doAirport(const Position &pos);

    void coalSmoke(const Position &pos);

    void doMeltdown(const Position &pos);


    ////////////////////////////////////////////////////////////////////////
    // sprite.cpp

public:


    SimSprite *spriteList; ///< List of active sprites.


    SimSprite *newSprite(const char *name, int type, int x, int y);

    void initSprite(SimSprite *sprite, int x, int y);

    void destroyAllSprites();

    void destroySprite(SimSprite *sprite);

    SimSprite *getSprite(int type);

    SimSprite *makeSprite(int type, int x, int y);


private:

    SimSprite *freeSprites; ///< Pool of free #SimSprite objects.

    SimSprite *globalSprites[SPRITE_COUNT];

    int absDist;

    short spriteCycle;


public:

    short getChar(int x, int y);

    short turnTo(int p, int d);

    bool tryOther(int Tpoo, int Told, int Tnew);

    bool spriteNotInBounds(SimSprite *sprite);

    short getDir(int orgX, int orgY, int desX, int desY);

    int getDistance(int x1, int y1, int x2, int y2);

    bool checkSpriteCollision(SimSprite *s1, SimSprite *s2);

    void moveObjects();

    void doTrainSprite(SimSprite *sprite);

    void doCopterSprite(SimSprite *sprite);

    void doAirplaneSprite(SimSprite *sprite);

    void doShipSprite(SimSprite *sprite);

    void doMonsterSprite(SimSprite *sprite);

    void doTornadoSprite(SimSprite *sprite);

    void doExplosionSprite(SimSprite *sprite);

    void doBusSprite(SimSprite *sprite);

    int canDriveOn(int x, int y);

    void explodeSprite(SimSprite *sprite);

    bool checkWet(int x);

    void destroyMapTile(int ox, int oy);

    void startFireInZone(int Xloc, int Yloc, int ch);

    void startFire(int x, int y);

    void generateTrain(int x, int y);

    void generateBus(int x, int y);

    void generateShip();

    void makeShipHere(int x, int y);

    void makeMonster();

    void makeMonsterAt(int x, int y);

    void generateCopter(const Position &pos);

    void generatePlane(const Position &pos);

    void makeTornado();

    void makeExplosion(int x, int y);

    void makeExplosionAt(int x, int y);


    ////////////////////////////////////////////////////////////////////////
    // stubs.cpp

public:


    Quad totalFunds; ///< Funds of the player

    /**
     * Enable auto-bulldoze
     *
     * When enabled, the game will silently clear tiles when the user
     * builds something on non-clear and bulldozable tiles
     */
    bool autoBulldoze;

    /**
     * Enable auto budget
     *
     * When enabled, the program will perform budgetting of the city
     */
    bool autoBudget;

    Quad messageTimeLast;

    GameLevel gameLevel; ///< Difficulty level of the game (0..2)

    short initSimLoad;

    Scenario scenario; ///< Scenario being played

    short simSpeed;

    short simSpeedMeta;

    bool enableSound; ///< Enable sound

    bool enableDisasters; ///< Enable disasters

    short messageNumber;

    bool evalChanged; ///< The evaluation window should be shown to the user

    /**
     * @todo Variable is not used, can we remove it?
     */
    short blinkFlag;

    /**
     * Hook into scripting language to send callbacks.
     * (i.e. a function that calls back into the Python interpreter.)
     */
    CallbackFunction callbackHook;

    /**
     * Hook for scripting language to store scripted callback function.
     * (i.e. a callable Python object.)
     */
    void *callbackData;

    /**
     * Hook for scripting language to store context (i.e. peer object).
     * (i.e. Python SWIG wrapper of this Micropolis object.)
     */
    void *userData;


    /**
     * Check that the given coordinate is within world bounds
     * @param wx World x coordinate
     * @param wy World y coordinate
     * @return Boolean indicating (wx, wy) is inside the world bounds
     */
    static inline bool testBounds(int wx, int wy)
    {
        return (wx >= 0 && wx < WORLD_W && wy >= 0 && wy < WORLD_H);
    };

    void spend(int dollars);

    void setFunds(int dollars);

    Quad tickCount();

    Ptr newPtr(int size);

    void freePtr(void *data);

    void doPlayNewCity();

    void doReallyStartGame();

    void doStartLoad();

    void doStartScenario(int scenario);

    void dropFireBombs();

    void initGame();

    void callback(const char *name, const char *params, ...);

    void doEarthquake(int strength);

    void invalidateMaps();

    void makeSound(
      const char *channel,
      const char *sound,
      int x=-1,
      int y=-1);

    // Map

    int getTile(int x, int y);

    void setTile(int x, int y, int tile);

    void *getMapBuffer();

    // Residential.

    // Commercial.

    // Industrial.

    // Power.

    int getPowerGrid(int x, int y);

    void setPowerGrid(int x, int y, int power);

    void *getPowerGridMapBuffer();

    // Roads.

    // Population density.

    int getPopulationDensity(int x, int y);

    void setPopulationDensity(int x, int y, int density);

    void *getPopulationDensityMapBuffer();

    // Rate of growth.

    int getRateOfGrowth(int x, int y);

    void setRateOfGrowth(int x, int y, int rate);

    void *getRateOfGrowthMapBuffer();

    // Traffic density.

    int getTrafficDensity(int x, int y);

    void setTrafficDensity(int x, int y, int density);

    void *getTrafficDensityMapBuffer();

    // Pollution density.

    int getPollutionDensity(int x, int y);

    void setPollutionDensity(int x, int y, int density);

    void *getPollutionDensityMapBuffer();

    // Crime rate.

    int getCrimeRate(int x, int y);

    void setCrimeRate(int x, int y, int rate);

    void *getCrimeRateMapBuffer();

    // Land value.

    int getLandValue(int x, int y);

    void setLandValue(int x, int y, int value);

    void *getLandValueMapBuffer();

    // Fire coverage.

    int getFireCoverage(int x, int y);

    void setFireCoverage(int x, int y, int coverage);

    void *getFireCoverageMapBuffer();

    // Police coverage.

    int getPoliceCoverage(int x, int y);

    void setPoliceCoverage(int x, int y, int coverage);

    void *getPoliceCoverageMapBuffer();


    ////////////////////////////////////////////////////////////////////////
    // tool.cpp

public:

    ToolResult doTool(EditingTool tool, short tileX, short tileY);

    void toolDown(EditingTool tool, short tileX, short tileY);

    void toolDrag(EditingTool tool, short fromX, short fromY,
                                    short toX, short toY);


    void didTool(const char *name, short x, short y);

private:

    ToolResult queryTool(short x, short y);

    ToolResult bulldozerTool(short x, short y);

    ToolResult roadTool(short x, short y);

    ToolResult railroadTool(short x, short y);

    ToolResult wireTool(short x, short y);

    ToolResult parkTool(short x, short y);

    ToolResult buildBuildingTool(short x, short y,
                                  const BuildingProperties *bprops,
                                  ToolEffects *effects);


    ToolResult networkTool(short x, short y);

    ToolResult waterTool(short x, short y);

    ToolResult landTool(short x, short y);

    ToolResult forestTool(short x, short y);

    ToolResult putDownPark(short mapH, short mapV);

    ToolResult putDownNetwork(short mapH, short mapV);

    ToolResult putDownWater(short mapH, short mapV);

    ToolResult putDownLand(short mapH, short mapV);

    ToolResult putDownForest(short mapH, short mapV);

    void doZoneStatus(short mapH, short mapV);

    void doShowZoneStatus(
        char *str,
        char *s0, char *s1, char *s2, char *s3, char *s4,
        int x, int y);

    void putBuilding(int leftX, int topY, int sizeX, int sizeY,
                     MapTile baseTile, bool aniFlag,
                     ToolEffects *effects);

    ToolResult prepareBuildingSite(int leftX, int topY, int sizeX, int sizeY,
                                   ToolEffects *effects);

    ToolResult buildBuilding(int mapH, int mapV,
                                  const BuildingProperties *bprops,
                                  ToolEffects *effects);

    int getDensityStr(short catNo, short mapH, short mapV);

    bool tally(short tileValue);

    short checkSize(short tileValue);

    void checkBorder(short xMap, short yMap, int sizeX, int sizeY,
                     ToolEffects *effects);

    void putRubble(int x, int y, int size);


    ////////////////////////////////////////////////////////////////////////
    // traffic.cpp

private:

    /* Position stack */

    /** Position stack top pointer, points to top position. */
    short curMapStackPointer;
    Position curMapStackXY[MAX_TRAFFIC_DISTANCE + 1]; ///< Position stack.

    short trafMaxX; ///< X coordinate of a position with heavy traffic
    short trafMaxY; ///< Y coordinate of a position with heavy traffic


    short makeTraffic(const Position &startPos, ZoneType dest);

    void addToTrafficDensityMap();

    void pushPos(const Position &pos);

    Position pullPos();

    bool findPerimeterRoad(Position *pos);

    bool findPerimeterTelecom(const Position &pos);

    bool tryDrive(const Position &startPos, ZoneType destZone);

    Direction2 tryGo(const Position &pos, Direction2 dirLast);

    MapTile getTileFromMap(const Position &pos,
                            Direction2 dir, MapTile defaultTile);

    bool driveDone(const Position &pos, ZoneType destZone);

    bool roadTest(MapValue tile);


    ////////////////////////////////////////////////////////////////////////
    // update.cpp

public:


    bool mustUpdateFunds;

    bool mustUpdateOptions; ///< Options displayed at user need updating.

    Quad cityTimeLast;

    Quad cityYearLast;

    Quad cityMonthLast;

    Quad totalFundsLast;

    Quad resLast;

    Quad comLast;

    Quad indLast;


    void doUpdateHeads();

    void updateMaps();

    void updateGraphs();

    void updateEvaluation();

    void updateHeads();

    void updateFunds();

    void reallyUpdateFunds();

    void doTimeStuff();

    void updateDate();

    void showValves();

    void drawValve();

    void setDemand(float r, float c, float i);

    void updateOptions();

    void updateUserInterface();


    ////////////////////////////////////////////////////////////////////////
    // utilities.cpp

public:

    void pause();

    void resume();

    void setSpeed(short speed);

    void setPasses(int passes);

    void setGameLevelFunds(GameLevel level);

    void setGameLevel(GameLevel level);

    void updateGameLevel();

    void setCityName(const std::string &name);

    void setCleanCityName(const std::string &name);

    void setYear(int year);

    int currentYear();

    void doNewGame();

    void setEnableDisasters(bool value);

    void setAutoBudget(bool value);

    void setAutoBulldoze(bool value);

    void setAutoGoto(bool value);

    void setEnableSound(bool value);

    void setDoAnimation(bool value);

    void setDoMessages(bool value);

    void setDoNotices(bool value);

#ifdef SWIG
// This tells SWIG that resDemandResult, comDemandResult and indDemandResult
// are output parameters, which will be returned in a tuple of length three.
%apply float *OUTPUT { float *resDemandResult };
%apply float *OUTPUT { float *comDemandResult };
%apply float *OUTPUT { float *indDemandResult };
#endif

    void getDemands(
        float *resDemandResult,
        float *comDemandResult,
        float *indDemandResult);


private:

    void makeDollarDecimalStr(char *numStr, char *dollarStr);


    ////////////////////////////////////////////////////////////////////////
    // zone.cpp

private:


    void doZone(const Position &pos);

    void doHospitalChurch(const Position &pos);

    void setSmoke(const Position &pos, bool zonePower);

    void makeHospital(const Position &pos);

    short getLandPollutionValue(const Position &pos);

    void incRateOfGrowth(const Position &pos, int amount);

    bool zonePlop(const Position &pos, int base);

    short doFreePop(const Position &pos);

    bool setZonePower(const Position& pos);

    void buildHouse(const Position &pos, int value);

    short evalLot(int x, int y);

    void doResidential(const Position &pos, bool zonePower);

    void doResIn(const Position &pos, int pop, int value);

    void doResOut(const Position &pos, int pop, int value);

    short getResZonePop(MapTile mapTile);

    void resPlop(const Position &pos, int Den, int Value);

    short evalRes(const Position &pos, int traf);

    // Commercial zone handling

    void doCommercial(const Position &pos, bool zonePower);

    void doComIn(const Position &pos, int pop, int value);

    void doComOut(const Position &pos, int pop, int value);

    short getComZonePop(MapTile tile);

    void comPlop(const Position &pos, int Den, int Value);

    short evalCom(const Position &pos, int traf);

    // Industrial zone handling

    void doIndustrial(const Position &pos, bool zonePower);

    void doIndIn(const Position &pos, int pop, int value);

    void doIndOut(const Position &pos, int pop, int value);

    short getIndZonePop(MapTile tile);

    void indPlop(const Position &pos, int den, int value);

    short evalInd(int traf);


    ////////////////////////////////////////////////////////////////////////


};


////////////////////////////////////////////////////////////////////////
