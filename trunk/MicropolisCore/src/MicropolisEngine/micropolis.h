/* micropolis.h
 * Micropolis include file
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


////////////////////////////////////////////////////////////////////////
// Constants


#define MICROPOLIS_VERSION              "5.0"

#define IS_INTEL                        1

#define RANDOM_RANGE                    0xffff

/**
 * Size of the world in horizontal direction.
 * @note Must be <= 128 due to powerMap bitmap
 *       (where 1 row is assumed to be less or equal to 8 words).
 */
static const int WORLD_X = 120;

/**
 * Size of the world in vertical direction.
 */
static const int WORLD_Y = 100;

/**
 * Horizontal size of the world for a map that stores a value for every 2x2
 * square.
 * @todo Make a Map class that keeps its 2x2 square storage details internally,
 *       so the code doesn't need to bother with it.
 */
static const int HWLDX = WORLD_X >>1;

/**
 * Vertical size of the world for a map that stores a value for every 2x2
 * square.
 */
static const int HWLDY = WORLD_Y >>1;

/**
 * Horizontal size of the world for a map that stores a value for every 4x4
 * square.
 * @todo Make a Map class that keeps its 4x4 square storage details internally,
 *       so the code doesn't need to bother with it.
 */
static const int QWX = WORLD_X >>2;

/**
 * Vertical size of the world for a map that stores a value for every 4x4
 * square.
 */
static const int QWY = WORLD_Y >>2;

/**
 * Horizontal size of the world for a map that stores a value for every 8x8
 * square.
 * @todo Make a Map class that keeps its 8x8 square storage details internally,
 *       so the code doesn't need to bother with it.
 */
static const int SmX = WORLD_X >>3;

/**
 * Vertical size of the world for a map that stores a value for every 8x8
 * square.
 */
static const int SmY = (WORLD_Y + 7) >>3;

#define EDITOR_W                        (WORLD_X * 16)
#define EDITOR_H                        (WORLD_Y * 16)
#define MAP_W                           (WORLD_X * 3)
#define MAP_H                           (WORLD_Y * 3)

#define NIL                             0
#define HORIZ                           1
#define VERT                            0

#define HISTLEN                         480
#define MISCHISTLEN                     240

#define ISLAND_RADIUS                   18

/**
 * Length of the history tables.
 * @todo It is not really a count of histories, rename to HISTORY_LENGTH?
 */
static const int HISTORY_COUNT = 120;


/**
 * Available types of historic data
 * @todo These enum values seem equal to the *_HIST constants below.
 *       Either eliminate one of the sets, or document how they differ.
 * @note ALL_HISTORIES does not seem to be used.
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

/* Graph Histories */
#define RES_HIST                        0
#define COM_HIST                        1
#define IND_HIST                        2
#define MONEY_HIST                      3
#define CRIME_HIST                      4
#define POLLUTION_HIST                  5

#define HISTORIES                       6
#define ALL_HISTORIES                   ((1 <<HISTORIES) - 1)

/** Available historic scales */
enum HistoryScale {
    HISTORY_SCALE_SHORT, ///< Short scale data (10 years)
    HISTORY_SCALE_LONG,  ///< Long scale data (120 years)

    HISTORY_SCALE_COUNT, ///< Number of history scales available
};

#define POWERMAPROW                     ((WORLD_X + 15) / 16)

#define POWERMAPLEN                     1700 /* ??? PWRMAPSIZE */
#define POWERWORD(x, y)                 (((x) >>4) + ((y) <<3))

#define SETPOWERBIT(x, y)               powerMap[POWERWORD((x), (y))] |= 1 << ((x) & 15)
#define PWRMAPSIZE                      (POWERMAPROW * WORLD_Y)
#define PWRSTKSIZE                      ((WORLD_X * WORLD_Y) / 4)

#define ALMAP                           0 /* all */
#define REMAP                           1 /* residential */
#define COMAP                           2 /* commercial */
#define INMAP                           3 /* industrial */

#define PRMAP                           4 /* power */
#define RDMAP                           5 /* road */

#define PDMAP                           6 /* population density */
#define RGMAP                           7 /* rate of growth */

#define TDMAP                           8 /* traffic density */
#define PLMAP                           9 /* pollution */
#define CRMAP                           10 /* crime */
#define LVMAP                           11 /* land value */

#define FIMAP                           12 /* fire radius */
#define POMAP                           13 /* police radius */
#define DYMAP                           14 /* dynamic */

#define NMAPS                           15

/* These adjust frequency in Simulate() */

#define VALVERATE                       2
#define CENSUSRATE                      4
#define TAXFREQ                         48

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

/**
 * Status bits of a map tile
 * @see MapTile MapCharacters
 * @todo #ALLBITS should end with MASK
 * @todo Decide what to do with #ANIMBIT (since sim-backend shouldn't do animation)
 */
enum MapTileBits {
    PWRBIT  = 0x8000, ///< bit 15, tile has power
    CONDBIT = 0x4000, ///< bit 14. tile can conduct electricity
    BURNBIT = 0x2000, ///< bit 13
    BULLBIT = 0x1000, ///< bit 12, tile is bulldozable
    ANIMBIT = 0x0800, ///< bit 11, tile is animated
    ZONEBIT = 0x0400, ///< bit 10, tile is a zone

    /// Mask for the bits-part of the tile
    ALLBITS = ZONEBIT | ANIMBIT | BULLBIT | BURNBIT | CONDBIT | PWRBIT,
    LOMASK = 0x03ff, ///< Mask for the #MapTileCharacters part of the tile

    BLBNBIT   = BULLBIT | BURNBIT,
    BLBNCNBIT = BULLBIT | BURNBIT | CONDBIT,
    BNCNBIT   =           BURNBIT | CONDBIT,
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
    // sprite 1 ?

    /* Water */
    RIVER          = 2,
    REDGE          = 3,
    CHANNEL        = 4,
    FIRSTRIVEDGE   = 5,
    // sprite 6 -- 19 ?
    LASTRIVEDGE    = 20,
    WATER_LOW      = RIVER,       ///< First water sprite
    WATER_HIGH     = LASTRIVEDGE, ///< Last water sprite (inclusive)

    TREEBASE       = 21,
    WOODS_LOW      = TREEBASE,
    LASTTREE       = 36,
    WOODS          = 37,
    UNUSED_TRASH1  = 38,
    UNUSED_TRASH2  = 39,
    WOODS_HIGH     = UNUSED_TRASH2, // Why is an 'UNUSED' sprite used?
    WOODS2         = 40,
    WOODS3         = 41,
    WOODS4         = 42,
    WOODS5         = 43,

    /* Rubble (4 sprites) */
    RUBBLE         = 44,
    LASTRUBBLE     = 47,

    FLOOD          = 48,
    // sprite 49, 50 ?
    LASTFLOOD      = 51,

    RADTILE        = 52, ///< Radio-active contaminated tile

    UNUSED_TRASH3  = 53,
    UNUSED_TRASH4  = 54,
    UNUSED_TRASH5  = 55,

    /* Fire animation (8 sprites) */
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
    LTRFBASE       = 80, ///< First sprite with low traffic
    // sprite 81 -- 94 ?
    BRWV           = 95,
    // sprite 96 -- 110 ?
    BRWXXX1        = 111,
    // sprite 96 -- 110 ?
    BRWXXX2        = 127,
    // sprite 96 -- 110 ?
    BRWXXX3        = 143,
    HTRFBASE       = 144, ///< First sprite with high traffic
    // sprite 145 -- 158 ?
    BRWXXX4        = 159,
    // sprite 160 -- 174 ?
    BRWXXX5        = 175,
    // sprite 176 -- 190 ?
    BRWXXX6        = 191,
    // sprite 192 -- 205 ?
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

    RESBASE        = 240,
    // sprite 241 -- 243 ?
    FREEZ          = 244,
    // sprite 245 -- 248 ?
    HOUSE          = 249,
    LHTHR          = HOUSE,
    // sprite 249 -- 259 ?
    HHTHR          = 260,
    // sprite 261 -- 264 ?
    RZB            = 265,
    // sprite 266 -- 408 ?
    HOSPITAL       = 409,
    // sprite 410 -- 417 ?
    CHURCH         = 418,
    // sprite 419 -- 422 ?
    COMBASE        = 423,
    // sprite 424 -- 426 ?
    COMCLR         = 427,
    // sprite 428 -- 435 ?
    CZB            = 436,
    // sprite 437 -- 608 ?
    COMLAST        = 609,
    // sprite 610, 611 ?
    INDBASE        = 612,
    // sprite 613 -- 615 ?
    INDCLR         = 616,
    // sprite 617 -- 619 ?
    LASTIND        = 620,
    IND1           = 621,
    // sprite 622 -- 624 ?
    IZB            = 625,
    // sprite 626 -- 640 ?
    IND2           = 641,
    // sprite 642, 643 ?
    IND3           = 644,
    // sprite 645 -- 648 ?
    IND4           = 649,
    IND5           = 650,
    // sprite 651 -- 675 ?
    IND6           = 676,
    IND7           = 677,
    // sprite 678 -- 685 ?
    IND8           = 686,
    // sprite 687, 688 ?
    IND9           = 689,
    // sprite 690 -- 692 ?
    PORTBASE       = 693,
    // sprite 694 -- 697 ?
    PORT           = 698,
    // sprite 699 -- 707 ?
    LASTPORT       = 708,
    AIRPORTBASE    = 709,
    // sprite 710 ?
    RADAR          = 711,
    // sprite 712 -- 715 ?
    AIRPORT        = 716,
    // sprite 717 -- 744 ?
    COALBASE       = 745,
    // sprite 746 -- 749 ?
    POWERPLANT     = 750,
    // sprite 751 -- 759 ?
    LASTPOWERPLANT = 760, // Why is NUCLEAR further down?

    FIRESTBASE     = 761,
    // sprite 762 -- 764 ?
    FIRESTATION    = 765,
    // sprite 766 -- 769 ?
    POLICESTBASE   = 770,
    // sprite 771 -- 773 ?
    POLICESTATION  = 774,
    // sprite 775 -- 778 ?
    STADIUMBASE    = 779,
    // sprite 780 -- 783 ?
    STADIUM        = 784,
    // sprite 785 -- 799 ?
    FULLSTADIUM    = 800,
    // sprite 801 -- 810 ?
    NUCLEARBASE    = 811,
    // sprite 812 -- 815 ?
    NUCLEAR        = 816, ///< Nuclear power plant
    // sprite 817 -- 825 ?
    LASTZONE       = 826,
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
    // sprite 841 -- 843 ?
    INDBASE2       = 844,
    TELEBASE       = 844,
    // sprite 845 -- 850 ?
    TELELAST       = 851,
    SMOKEBASE      = 852,
    // sprite 853 -- 859 ?
    TINYEXP        = 860,
    // sprite 861 -- 863 ?
    SOMETINYEXP    = 864,
    // sprite 865 -- 866 ?
    LASTTINYEXP    = 867,
    // sprite 868 -- 882 ?
    TINYEXPLAST    = 883,
    // sprite 884 -- 915 ?
    COALSMOKE1     = 916,
    // sprite 917 -- 919 ?
    COALSMOKE2     = 920,
    // sprite 921 -- 923 ?
    COALSMOKE3     = 924,
    // sprite 925 -- 927 ?
    COALSMOKE4     = 928,
    // sprite 929 -- 931 ?
    FOOTBALLGAME1  = 932,
    // sprite 933 -- 939 ?
    FOOTBALLGAME2  = 940,
    // sprite 941 -- 947 ?
    VBRDG0         = 948,
    VBRDG1         = 949,
    VBRDG2         = 950,
    VBRDG3         = 951,
    // sprite 952 -- 959 ?

    TILE_COUNT     = 960,
};

/*
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

    TOOL_COUNT,
    TOOL_FIRST = TOOL_RESIDENTIAL,
    TOOL_LAST = TOOL_NETWORK,
};


///////////////////////////////////////////////////
// Scenarios

/** Available scenarios */
enum Scenario {
    SC_NONE,           ///< No scenario (free playing)

    SC_DULLSVILLE,     ///< Dullsville (boredom)
    SC_SAN_FRANCISCO,  ///< San francisco (earth quake)
    SC_HAMBURG,        ///< Hamburg (fire bombs)
    SC_BERN,           ///< Bern (traffic)
    SC_TOKYO,          ///< Tokyo (scary monster)
    SC_DETROIT,        ///< Detroit (crime)
    SC_BOSTON,         ///< Boston (nuclear meltdown)
    SC_RIO,            ///< Rio (flooding)

    SC_COUNT,          ///< Number of scenarios
};

///////////////////////////////////////////////////
// Directions

/** Directions on the map */
enum Direction {
    DIR_NORTH, ///< North (0, -1)
    DIR_WEST,  ///< West  (-1, 0)
    DIR_SOUTH, ///< South (0, +1)
    DIR_EAST,  ///< East  (+1, 0)

    DIR_DIR4,

    DIR_INVALID ///< Invalid direction (to 'nowhere')
};

/**
 * Return reverse direction
 * @param d Direction to reverse
 * @return Reversed direction
 */
static inline Direction ReverseDirection(Direction d)
{
    return (Direction)((d + 2) & 0x3);
}

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
// Traffic

/** Maximal number of map tiles to drive, looking for a destination */
static const int MAX_TRAFFIC_DISTANCE = 30;

/**
 * Maximal value of Micropolis::RoadEffect
 */
static const int MAX_ROAD_EFFECT = 32;

/**
 * Maximal value of Micropolis::PoliceEffect
 */
static const int MAX_POLICESTATION_EFFECT = 1000;

/**
 * Maximal value of Micropolis::FireEffect
 */
static const int MAX_FIRESTATION_EFFECT = 1000;


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
 * Check that the given coordinate is within world bounds
 * @param wx World x coordinate
 * @param wy World y coordinate
 * @return Boolean indicating (wx, wy) is inside the world bounds
 */
static inline bool TestBounds(int wx, int wy)
{
    return (wx >= 0 && wx < WORLD_X && wy >= 0 && wy < WORLD_Y);
}

/**
 * Compute minimal value.
 * @param a First value.
 * @parem b Second value.
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
 * @parem b Second value.
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
 * @parem lower Lower bound (inclusive).
 * @parem upper Upper bound (inclusive).
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
static inline int absoluteValue(int val)
{
    if (val < 0) {
        return -val;
    }
    return val;
}

////////////////////////////////////////////////////////////////////////
// Macros


#define ABS(x) \
    (((x) < 0) ? (-(x)) : (x))

#define TILE_IS_NUCLEAR(tile) \
    ((tile & LOMASK) == NUCLEAR)

#define TILE_IS_VULNERABLE(tile) \
    (!(tile & ZONEBIT) && \
     ((tile & LOMASK) >= RBRDR) && \
     ((tile & LOMASK) <= LASTZONE))

#define TILE_IS_ARSONABLE(tile) \
    (!(tile & ZONEBIT) && \
     ((tile & LOMASK) >= RBRDR) && \
     ((tile & LOMASK) <= LASTZONE))

#define TILE_IS_RIVER_EDGE(tile) \
    (((tile & LOMASK) >= FIRSTRIVEDGE) && \
     ((tile & LOMASK) <= LASTRIVEDGE))

#define TILE_IS_FLOODABLE(tile) \
    ((tile == DIRT) || \
     ((tile & BULLBIT) && \
      (tile & BURNBIT)))

#define TILE_IS_RUBBLE(tile) \
     (((tile & LOMASK) >= RUBBLE) && \
      ((tile & LOMASK) <= LASTRUBBLE)))

#define TILE_IS_FLOODABLE2(tile) \
    ((tile == 0) || \
     (tile & BURNBIT) || \
     TILE_IS_RUBBLE(tile))

#define NeutralizeRoad(tile) \
    tile &= LOMASK; \
    if ((tile >= 64) && \
        (tile <= 207)) { \
        tile = (tile & 0x000F) + 64; \
    }

/**
 * Compute length of array
 * @param array Array to get length from
 * @note Macro only works for statically allocated arrays
 */
#define LENGTH_OF(array) (sizeof(array) / sizeof((array)[0]))

/**
 * Give a fatal error and exit
 */
#define NOT_REACHED() not_reached(__LINE__, __FILE__)

void not_reached(int line, const char *fname);


////////////////////////////////////////////////////////////////////////
// Forward class definitions


class Micropolis;


////////////////////////////////////////////////////////////////////////
// Typedefs


typedef unsigned char Byte;

typedef Byte *Ptr;

typedef long Quad;

typedef unsigned long UQuad;

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
 * @todo #name is never set to anything else than \c "", and only used to
 *       detect a non-removed non-active sprite (in a non-robust way).
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
    int xHot; ///< Offset of the hot-spot of the sprite relative to #x?
    int yHot; ///< Offset of the hot-spot of the sprite relative to #y?
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
 * @todo Micropolis::CrimeMaxX and Micropolis::CrimeMaxY seem unused.
 */
class Micropolis {


public:


    ////////////////////////////////////////////////////////////////////////


    Micropolis();

    ~Micropolis();

    void init();

    void destroy();


    ////////////////////////////////////////////////////////////////////////
    // allocate.cpp

public:


    /**
     * Communication variable with map scan x position.
     *
     * Used all over.
     */
    short curMapX;


    /**
     * Communication variable with map scan y position.
     *
     * Used all over.
     */
    short curMapY;

    /**
     * Current number at curMapX, curMapY, raw.
     *
     * Used all over.
     */
    short curNum;

    /**
     * Current tile at curMapX, curMapY, the number masked with TILEMASK.
     *
     * Used all over.
     */
    short curTile;

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
     * Residential zone population.
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

    /**
     * Residential zone population.
     */
    short resZonePop;

    /**
     * Commercial zone population.
     */
    short comZonePop;

    /**
     * Industrial zone population.
     */
    short indZonePop;

    /**
     * Total zone population.
     */
    short totalZonePop;

    /**
     * Hospital population.
     */
    short hospitalPop;

    /**
     * Church population.
     */
    short churchPop;

    /**
     * Stadium population.
     */
    short stadiumPop;

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
     * @todo Remove, can be computed from #cityTime.
     */
    Quad cityMonth;

    /**
     * City year, (Micropolis::cityTime / 48) + Micropolis::startingYear.
     * @todo Remove, can be computed from #cityTime.
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
    short *map[WORLD_X];

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

    /**
     * Message number to display asynchronously.
     * Value \c 0 means 'no message to display'.
     *
     * Clean this up to use a simpler interface, and a queue.
     * Might need to collapse some messages.
     */
    short messagePort;

    /**
     * Message X location.
     */
    short messageX;

    /**
     * Message Y location.
     */
    short messageY;

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
     * Radio of road spending over road funding, times 32.
     */
    Quad roadEffect;

    /**
     * Radio of police spending over road funding, times 32.
     */
    Quad policeEffect;

    /**
     * Radio of fire spending over road funding, times 32.
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

    /**
     * Population density map.
     */
    Byte *populationDensityMap[HWLDX];

    /**
     * Traffic map.
     */
    Byte *trafficDensityMap[HWLDX];

    /**
     * Pollution map.
     */
    Byte *pollutionMap[HWLDX];

    /**
     * Land value mep.
     */
    Byte *landValueMap[HWLDX];

    /**
     * Crime map.
     */
    Byte *crimeMap[HWLDX];

    /**
     * Temporary map 1.
     *
     * Used to smooth population density, pollution.
     */
    Byte *tempMap1[HWLDX];

    /**
     * Temporary map 2.
     *
     * Used to smooth population density, pollution.
     */
    Byte *tempMap2[HWLDX];

    /**
     * Temporary map 3.
     *
     * Used to smooth development density, for terrainDensityMap.
     */
    Byte *tempMap3[QWX];

    /**
     * Temporary array for smoothing fire and police station maps.
     */
    short tempMap4[SmX][SmY];

    /**
     * Terrain development density map.
     *
     * Used to calculate land value.
     */
    Byte *terrainDensityMap[QWX];

    /**
     * Rate of growth map.
     *
     * Affected by DecROGMem, incROG called by zones. Decreased by fire
     * explosions from sprites, fire spreading. Doesn't seem to
     * actually feed back into the simulation. Output only.
     */
    short rateOfGrowthMap[SmX][SmY];

    /**
     * Fire station map.
     *
     * Effectivity of fire control in this area.
     *
     * Affected by fire stations, powered, fire funding ratio, road
     * access. Affects how long fires burn.
     * @see #FireEffect
     */
    short fireStationMap[SmX][SmY];

    /**
     * Police station map.
     *
     * Effectivity of police in fighting crime.
     *
     * Affected by police stations, powered, police funding ratio, road
     * access. Affects crime rate.
     * @see #PoliceEffect
     */
    short policeStationMap[SmX][SmY];

    /**
     * Copy of police station map to display.
     */
    short policeStationMapEffect[SmX][SmY];

    /**
     * Copy of fire station map to display.
     */
    short fireStationMapEffect[SmX][SmY];

    /**
     * Commercial rate map.
     *
     * Depends on distance to city center. Affects commercial zone
     * evaluation.
     */
    short comRateMap[SmX][SmY];

    /**
     * Memory for terrainDensityMap array.
     */
    Ptr terrainDensityMapBase;

    /**
     * Memory for tempMap1 array.
     */
    Ptr tempMap1Base;

    /**
     * Memory for tempMap2 array.
     */
    Ptr tempMap2Base;

    /**
     * Memory for tempMap3 array.
     */
    Ptr tempMap3Base;

    /**
     * Memory for populationDensityMap array.
     */
    Ptr populationDensityMapBase;

    /**
     * Memory for trafficDensityMap array.
     */
    Ptr trafficDensityMapBase;

    /**
     * Memory for pollutionMap array.
     */
    Ptr pollutionMapBase;

    /**
     * Memory for landValueMap array.
     */
    Ptr landValueMapBase;

    /**
     * Memory for crimeMap array.
     */
    Ptr crimeMapBase;

    /**
     * Memory for map array.
     */
    unsigned short *mapBase;

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

    /**
     * Power distribution bitmap.
     */
    short *powerMap;


    void initMapArrays();

    void destroyMapArrays();

#ifdef SWIG
// This tells SWIG that minValResult, maxValResult are output parameters,
// which will be returned in a tuple of length two.
%apply short *OUTPUT { short *minValResult };
%apply short *OUTPUT { short *maxValResult };
#endif

    void getHistoryRange(
        int historyType,
        int historyScale,
        short *minValResult,
        short *maxValResult);

    short getHistory(
        int historyType,
        int historyScale,
        int historyIndex);


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
     * Value between \c 0 and #MAX_ROAD_EFFECT.
     * Affected by road funds slider and budgetary constraints.
     */
    float roadPercent;

    /**
     * Percentage of requested police station costs to funding level.
     *
     * Value between \c 0 and #MAX_POLICESTATION_EFFECT.
     * Affected by road funds slider and budgetary constraints.
     */
    float policePercent;

    /**
     * Percentage of requested fire station costs to funding level.
     *
     * Value between \c 0 and #MAX_FIRESTATION_EFFECT.
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
     * Flag set when drawCurrPercents called.
     *
     * Causes reallyDrawCurrPercents to be called when updateBudgetWindow called.
     * @todo Clean this up.
     */
    int mustDrawCurrPercents;

    /**
     * Flag set when drawBudgetWindow called.
     *
     * Causes reallyDrawBudgetWindow to be called when updateBudgetWindow called.
     * @todo Clean this up.
     */
    int mustDrawBudgetWindow;


    void initFundingLevel();

    void doBudget();

    void doBudgetFromMenu();

    void doBudgetNow(bool fromMenu);

    void drawBudgetWindow();

    void reallyDrawBudgetWindow();

    void drawCurrPercents();

    void reallyDrawCurrPercents();

    void updateBudgetWindow();

    void updateBudget();

    void showBudgetWindowAndStartWaiting();

    void setBudget(
        char *flowStr,
        char *previousStr,
        char *currentStr,
        char *collectedStr,
        short tax);

    void setBudgetValues(
        char *roadGot,
        char *roadWant,
        char *policeGot,
        char *policeWant,
        char *fireGot,
        char *fireWant);


    ////////////////////////////////////////////////////////////////////////
    // connect.cpp

public:


    int connectTile(
        short x,
        short y,
        short *TileAdrPtr,
        short Command);

    int layDoze(
        int x,
        int y,
        short *TileAdrPtr);

    int layRoad(
        int x,
        int y,
        short *TileAdrPtr);

    int layRail(
        int x,
        int y,
        short *TileAdrPtr);

    int layWire(
          int x,
          int y,
          short *TileAdrPtr);

    void fixZone(
        int x,
        int y,
        short *TileAdrPtr);

    void fixSingle(
        int x,
        int y,
        short *TileAdrPtr);


    ////////////////////////////////////////////////////////////////////////
    // disasters.cpp

public:


    /**
     * Size of flooding disaster.
     */
    short floodCount;


    void makeMeltdown();

    void fireBomb();

    void makeEarthquake();

    void makeFire();

    void makeFlood();

  private:

    void doDisasters();

    void scenarioDisaster();

    void setFire();

    bool vulnerable(int tem);

    void doFlood();


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
     * Problem table.
     *
     * Score for each problem, higher the more severe the problem is.
     */
    short problemTable[PROBNUM];

    /**
     * Problem votes.
     *
     * The number of votes for each problem.
     */
    short problemVotes[PROBNUM];

    /**
     * Order of taken problems.
     *
     * Contains index of ProblemTable of taken problems, in decreasing order.
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
    Quad deltaCityPop;

    /**
     * City assessed value.
     *
     * Depends on roadTotal, railTotal, policeStationPop,
     * fireStationPop, hospitalPop, stadiumPop, seaportPop,
     * airportPop, coalPowerPop, and nuclearPowerPop, and their
     * respective values.
     */
    Quad cityAssessedValue;

    /**
     * City class.
     *
     * 0: village, 1: town, 2: city, 3: capital, 
     * 4: metropolis, 5: megalopolis.
     * Affected by city population.
     */
    short cityClass;

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

    void doPopNum();

    void doProblems();

    void getAssessedValue();

    void voteProblems();

    short getTrafficAverage();

    short getUnemployment();

    short getFireSeverity();

    void getScore();

    void doVotes();

    void doScoreCard();

    void changeEval();

    void scoreDoer();

    int countProblems();

    int getProblemNumber(int i);

    int getProblemVotes(int i);

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


    // It would be nice to open up the terrain generator, and make its
    // features available incrementally as city building tools.
    //
    // The user should be able to place water and trees, and it should
    // dynamically smooth the edges.
    //
    // The user interface could restrict the user to only drawing
    // terrain before any zones were built, but it would be best if
    // the terrain editing tools worked properly when there were zones
    // built (by automatically bulldozing zones whose underlying
    // terrain it's modifying).

    // Starting X location of the terrain generator.
    // Only used internally by the terrain generator. Should be private.
    short terrainXStart;

    // Starting Y location of the terrain generator.
    // Only used internally by the terrain generator. Should be private.
    short terrainYStart;

    // Current X location of the terrain generator.
    // Only used internally by the terrain generator. Should be private.
    short terrainMapX;

    // Current Y location of the terrain generator.
    // Only used internally by the terrain generator. Should be private.
    short terrainMapY;

    // Current direction of the terrain generator.
    // Only used internally by the terrain generator. Should be private.
    short terrainDir;

    // Last direction of the terrain generator.
    // Only used internally by the terrain generator. Should be private.
    short terrainDirLast;

    // Controls the level of tree creation.
    // -1 => create default number of trees, 0 => never create trees, >0 => create more trees
    int terrainTreeLevel;

    // Controls the level of lake creation.
    // -1 => create default number of lakes, 0 => never create lakes, >0 => create more lakes
    int terrainLakeLevel;

    // Controls the level of river curviness.
    // -1 => default curve level, 0 => never create rivers, >0 => create curvier rivers
    int terrainCurveLevel;

    // Controls how often to create an island.
    // -1 => 10% chance of island, 0 => never create island, 1 => always create island
    int terrainCreateIsland;

    // Controls the radius of islands.
    int terrainIslandRadius;


    void generateNewCity() ;

    void generateSomeCity(int r);

    void generateMap(int r);

    void clearMap();

    void clearUnnatural();

    void makeNakedIsland();

    void makeIsland();

    void makeLakes();

    void getRandStart();

    void moveMap(short dir);

    void treeSplash(short xloc, short yloc);

    void doTrees();

    void smoothRiver();

    bool isTree(int cell);

    void smoothTrees();

    void doRivers();

    void doBRiver();

    void doSRiver();

    void putOnMap(short Mchar, short Xoff, short Yoff);

    void plopBRiver();

    void plopSRiver();

    void smoothWater();


    ////////////////////////////////////////////////////////////////////////
    // graph.cpp

public:


    /**
     * Flag that tells if there is a new graph to draw.
     * @todo This should be replaced by a general purpose view updating system.
     */
    bool newGraph;

    ///< 10 year history graphs.
    unsigned char *history10[HISTORIES];

    ///< 120 year history graphs.
    unsigned char *history120[HISTORIES];

    //
    int historyInitialized;

    short graph10Max;

    short graph120Max;

    int graphUpdateTime;


    void drawMonth(short *hist, unsigned char *s, float scale);

    void doAllGraphs();

    void changeCensus();

    void graphDoer();

    void initGraphs();

    void initGraphMax();


    ////////////////////////////////////////////////////////////////////////
    // initialize.cpp

public:


    void initWillStuff();

    void resetMapState();

    void resetEditorState();


    ////////////////////////////////////////////////////////////////////////
    // main.cpp

public:


    int simLoops;

    int simSkips;

    int simSkip;

    /**
     * Simulation is paused
     * @todo Variable has reversed logic, maybe use sim_running instead?
     */
    bool simPaused;

    int simPausedSpeed;

    int heatSteps;

    int heatFlow;

    int heatRule;

    int heatWrap;

    std::string cityFileName; ///< Filename of the last loaded city
    std::string cityName;     ///< Name of the city

    int tilesAnimated; ///< @todo Not currently used, should hook it up.

    int doAnimation;

    int doMessages; ///< @todo Not currently used, should hook it up.

    int doNotices; ///< @todo Not currently used, should hook it up.


private:

    short *cellSrc;

    short *cellDst;


public:

    const char *getMicropolisVersion();

    void environmentInit();

    void simInit();

    void simUpdate();

    void simHeat();

    void simLoop(int doSim);

    void simTick();


#if 0

    ////////////////////////////////////////////////////////////////////////
    // map.cpp
    //
    // Disabled this small map drawing, filtering and overlaying code.
    // Going to re-implement it in the tile engine and Python.

public:


    int dynamicData[32];


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

    void drawTrafficMap();

    void drawPolMap();

    void drawCrimeMap();

    void drawLandMap();

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


    Quad lastCityPop;   ///< Population of last city class check. @see CheckGrowth
    short lastCategory; ///< City class of last city class check. @see CheckGrowth

    short messagePictureLast; ///< Last picture displayed to the user

    /**
     * Enable auto goto
     *
     * When enabled and an important event happens, the map display will jump to
     * the location of the event
     */
    bool autoGoto;

    /**
     * Do we have a valid message in LastMessage?
     * @todo Remove the variable by using LastMessage instead (for example the
     *       empty string)
     */
    bool messageLastValid;

    char messageLast[256]; ///< Last message displayed to the user


    void sendMessages();

    void checkGrowth();

    void doScenarioScore(Scenario type);

    void clearMessage();

    bool sendMessage(int Mnum);

    void sendMessageAt(short Mnum, short x, short y);

    void doMessage();

    void doMakeSound(int mesgNum, int x, int y);

    void doAutoGoto(short x, short y, char *msg);

    void setMessageField(char *str);

    void doShowPicture(short id);

    void doLoseGame();
    void doWinGame(); ///< @todo This may not be called. Call it when appropriate.


    ////////////////////////////////////////////////////////////////////////
    // power.cpp

public:

    /** @name Power stack
     * Stack used to find powered tiles by tracking conductive tiles.
     */
    //@{

    int powerStackNum; ///< Stack counter, points to top-most item.

    short powerStackX[PWRSTKSIZE]; ///< X coordinates at the power stack.
    short powerStackY[PWRSTKSIZE]; ///< Y coordinates at the power stack.


    void doPowerScan();

    bool testForConductive(Direction TFDir);

    void pushPowerStack();

    void pullPowerStack();

    //@}

    /**
     * Maximal power that the combined coal and nuclear power plants can deliver.
     * @see NumPower CoalPop NuclearPop
     */
    Quad maxPower;
    Quad numPower; ///< Amount of power used.


    bool moveMapSim(Direction mDir);


    ////////////////////////////////////////////////////////////////////////
    // random.cpp

public:


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


    /** Name of the Micrpolis top level home directory. */
    std::string homeDir;

    /** Name of the sub-directory where the resources are located. */
    std::string resourceDir;

    Resource *resources; ///< Linked list of loaded resources
    StringTable *stringTables; ///< Linked list of loaded string tables


    Resource *getResource(const char *name, Quad id);

    void getIndString(char *str, int id, short num);


    ////////////////////////////////////////////////////////////////////////
    // scan.cpp

public:


    short newMap;

    short newMapFlags[NMAPS];

    short cityCenterX; ///< X coordinate of city center
    short cityCenterY; ///< Y coordinate of city center

    short cityCenterX2; ///< X coordinate of city center divided by 2
    short cityCenterY2; ///< Y coordinate of city center divided by 2

    short pollutionMaxX; ///< X coordinate of most polluted area
    short pollutionMaxY; ///< Y coordinate of most polluted area

    short crimeMaxX; ///< X coordinate of most criminal area
    short crimeMaxY; ///< Y coordinate of most criminal area

    Quad donDither;


    void fireAnalysis();

    void populationDensityScan();

    int getPopulationDensity(int Ch9);

    void pollutionTerrainLandValueScan();

    int getPollutionValue(int loc);

    int getCityCenterDistance(int x, int y);

    void crimeScan();

    void smoothTerrain();

    void doSmooth1();

    void doSmooth2();

    void clearTempMap1();

    void smoothFireStationMap();

    void smoothPoliceStationMap();

    void computeComRateMap();


    ////////////////////////////////////////////////////////////////////////
    // simulate.cpp

public:


    short ValveFlag;

    short CrimeRamp;

    short PolluteRamp;

    short RValve;

    short CValve;

    short IValve;

    short ResCap;

    short ComCap;

    short IndCap;

    short CashFlow;

    float EMarket;

    Scenario DisasterEvent; ///< The disaster for which a count-down is running
    short DisasterWait; ///< Count-down timer for the disaster

    Scenario ScoreType; ///< The type of score table to use
    short ScoreWait; ///< Time to wait before computing the score

    short PwrdZCnt; ///< Number of powered tiles in all zone
    short unPwrdZCnt; ///< Number of unpowered tiles in all zones

    short NewPower; /* post */

    short cityTaxAverage;

    short simCycle;

    short phaseCycle;

    short speedCycle;

    bool doInitialEval; ///< Need to perform initial city evaluation.


    /** Generate a random animated MapTileCharacters::FIRE tile */
    inline short RandomFire()
    {
        return (FIRE + (getRandom16() & 7)) | ANIMBIT;
    };

    /** Generate a random MapTileCharacters::RUBBLE tile */
    inline short RandomRubble()
    {
        return (RUBBLE + (getRandom16() & 3)) | BULLBIT;
    };

    void SimFrame();

    void Simulate(
      int mod16);

    void DoSimInit();

    void DoNilPower();

    void DecTrafficMem();

    void DecROGMem();

    void InitSimMemory();

    void SimLoadInit();

    void SetCommonInits();

    void SetValves();

    void ClearCensus();

    void TakeCensus();

    void Take2Census();

    void CollectTax();

    void UpdateFundEffects();

    void MapScan(int x1, int x2);

    void DoRail();

    void DoRadTile();

    void DoRoad();

    bool DoBridge();

    int GetBoatDis();

    void DoFire();

    void FireZone(int Xloc, int Yloc, int ch);


private:

    void RepairZone(short ZCent, short zsize);

    void DoSPZone(bool PwrOn);

    void DrawStadium(int z);

    void DoAirport();

    void CoalSmoke(int mx, int my);

    void DoMeltdown(int SX, int SY);


    ////////////////////////////////////////////////////////////////////////
    // sprite.cpp

public:


    SimSprite *spriteList; ///< List of active sprites.

    SimSprite *FreeSprites; ///< Pool of free #SimSprite objects.

    SimSprite *GlobalSprites[SPRITE_COUNT];

    short CrashX;

    short CrashY;

    int absDist;

    short Cycle;


    SimSprite *NewSprite(const char *name, int type, int x, int y);

    void InitSprite(SimSprite *sprite, int x, int y);

    void DestroyAllSprites();

    void DestroySprite(SimSprite *sprite);

    SimSprite *GetSprite(int type);

    SimSprite *MakeSprite(int type, int x, int y);

    void DrawObjects();

    void DrawSprite(SimSprite *sprite);

    short GetChar(int x, int y);

    short TurnTo(int p, int d);

    bool TryOther(int Tpoo, int Told, int Tnew);

    bool SpriteNotInBounds(SimSprite *sprite);

    short GetDir(int orgX, int orgY, int desX, int desY);

    int GetDistance(int x1, int y1, int x2, int y2);

    bool CheckSpriteCollision(SimSprite *s1, SimSprite *s2);

    void MoveObjects();

    void DoTrainSprite(SimSprite *sprite);

    void DoCopterSprite(SimSprite *sprite);

    void DoAirplaneSprite(SimSprite *sprite);

    void DoShipSprite(SimSprite *sprite);

    void DoMonsterSprite(SimSprite *sprite);

    void DoTornadoSprite(SimSprite *sprite);

    void DoExplosionSprite(SimSprite *sprite);

    void DoBusSprite(SimSprite *sprite);

    int CanDriveOn(int x, int y);

    void ExplodeSprite(SimSprite *sprite);

    bool checkWet(int x);

    void Destroy(int ox, int oy);

    void OFireZone(int Xloc, int Yloc, int ch);

    void StartFire(int x, int y);

    void GenerateTrain(int x, int y);

    void GenerateBus(int x, int y);

    void GenerateShip();

    void MakeShipHere(int x, int y);

    void makeMonster();

    void MonsterHere(int x, int y);

    void GenerateCopter(int x, int y);

    void GeneratePlane(int x, int y);

    void makeTornado();

    void MakeExplosion(int x, int y);

    void MakeExplosionAt(int x, int y);


    ////////////////////////////////////////////////////////////////////////
    // stubs.cpp

public:


    Quad TotalFunds; ///< Funds of the player

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

    Quad LastMesTime;

    GameLevel gameLevel; ///< Difficulty level of the game (0..2)

    short InitSimLoad;

    Scenario ScenarioID; ///< Scenario being played

    short SimSpeed;

    short SimMetaSpeed;

    bool UserSoundOn; ///< Enable sound

    /**
     * Disable disasters
     * @todo Variable has reverse logic,
     *       we should use AllowDisasters or EnableDisasters instead
     */
    bool NoDisasters;

    short MesNum;

    bool evalChanged; ///< The evaluation window should be shown to the user

    short flagBlink;

    // Hook into scripting language to send callbacks.
    // (i.e. a function that calls back into the Python interpreter.)
    CallbackFunction callbackHook;

    // Hook for scripting language to store scripted callback function.
    // (i.e. a callable Python object.)
    void *callbackData;

    // Hook for scripting language to store context (i.e. peer object).
    // (i.e. Python SWIG wrapper of this Micropolis object.)
    void *userData;

    void Spend(int dollars);

    void SetFunds(int dollars);

    Quad TickCount();

    Ptr NewPtr(int size);

    void FreePtr(void *data);

    void DoPlayNewCity();

    void DoReallyStartGame();

    void DoStartLoad();

    void DoStartScenario(
      int scenario);

    void dropFireBombs();

    void InitGame();

    void Callback(const char *name, const char *params, ...);

    void DoEarthquake(int strength);

    void InvalidateEditors();

    void InvalidateMaps();

    void InitializeSound();

    void MakeSound(
      const char *channel,
      const char *sound,
      int x=-1,
      int y=-1);

    int getTile(int x, int y);

    void *getMapBuffer();


    ////////////////////////////////////////////////////////////////////////
    // tool.cpp

public:


    int last_x; ///< Get rid of this.

    int last_y; ///< Get rid of this.

    int tool_x; ///< Get rid of this.

    int tool_y; ///< Get rid of this.


    int putDownPark(short mapH, short mapV);

    int putDownNetwork(short mapH, short mapV);

    short checkBigZone(short id, short *deltaHPtr, short *deltaVPtr);

    int check3x3(short mapH, short mapV, short base, short tool);

    short check4x4(short mapH, short mapV, short base, short aniFlag, short tool);

    short check6x6(short mapH, short mapV, short base, short tool);

    int getDensityStr(short catNo, short mapH, short mapV);

    void doZoneStatus(short mapH, short mapV);

    void doShowZoneStatus(
        char *str,
        char *s0, char *s1, char *s2, char *s3, char *s4,
        int x, int y);

    void didTool(const char *name, short x, short y);

    int queryTool(short x, short y);

    int bulldozerTool(short x, short y);

    int roadTool(short x, short y);

    int railroadTool(short x, short y);

    int wireTool(short x, short y);

    int parkTool(short x, short y);

    int residentialTool(short x, short y);

    int commercialTool(short x, short y);

    int industrialTool(short x, short y);

    int policeStationTool(short x, short y);

    int fireStationTool(short x, short y);

    int stadiumTool(short x, short y);

    int coalPowerTool(short x, short y);

    int nuclearPowerTool(short x, short y);

    int seaportTool(short x, short y);

    int airportTool(short x, short y);

    int networkTool(short x, short y);

    int doTool(EditingTool tool, short x, short y, bool first);

    void toolDown(EditingTool tool, short x, short y);

    void toolUp(EditingTool tool, short x, short y);

    void toolDrag(EditingTool tool, short px, short py);

private:

    bool tally(short tileValue);

    short checkSize(short tileValue);

    void checkBorder(short xMap, short yMap, int size);

    void putRubble(int x, int y, int size);


    ////////////////////////////////////////////////////////////////////////
    // traffic.cpp

public:

    /* Position stack */
    short PosStackN; ///< Position stack top pointer, points to top position
    short curMapXStack[MAX_TRAFFIC_DISTANCE + 1]; ///< X positions
    short curMapYStack[MAX_TRAFFIC_DISTANCE + 1]; ///< Y positions

    Direction LDir; ///< Last moved direction

    short TrafMaxX; ///< X coordinate of a position with heavy traffic
    short TrafMaxY; ///< Y coordinate of a position with heavy traffic


    short MakeTraf(ZoneType dest);

    void SetTrafMem();

    void PushPos();

    void PullPos();

    bool FindPRoad();

    bool FindPTele();

    bool TryDrive();

    bool tryGo(int dist);

    short GetFromMap(Direction d);


private:

    ZoneType Zsource; ///< Destination of traffic


    bool DriveDone();

    bool RoadTest(int tile);


    ////////////////////////////////////////////////////////////////////////
    // update.cpp

public:


    short MustUpdateFunds;

    short MustUpdateOptions;

    Quad cityTimeLast;

    Quad cityYearLast;

    Quad cityMonthLast;

    Quad LastFunds;

    Quad LastR;

    Quad LastC;

    Quad LastI;


    void DoUpdateHeads();

    void UpdateEditors();

    void UpdateMaps();

    void UpdateGraphs();

    void UpdateEvaluation();

    void UpdateHeads();

    void UpdateFunds();

    void ReallyUpdateFunds();

    void doTimeStuff();

    void updateDate();

    void showValves();

    void drawValve();

    void SetDemand(float r, float c, float i);

    void updateOptions();

    void UpdateOptionsMenu(int options);

    void UpdateUserInterface();


    ////////////////////////////////////////////////////////////////////////
    // utilities.cpp

public:


    void makeDollarDecimalStr(char *numStr, char *dollarStr);

    void Pause();

    void Resume();

    void setSpeed(short speed);

    void setSkips(int skips);

    void SetGameLevelFunds(GameLevel level);

    void SetGameLevel(GameLevel level);

    void UpdateGameLevel();

    void setCityName(const std::string &name);

    void setCleanCityName(const std::string &name);

    void SetYear(int year);

    int CurrentYear();

    void DoNewGame();


    ////////////////////////////////////////////////////////////////////////
    // zone.cpp

public:


    void DoZone();

    void DoHospChur();

    void SetSmoke(int ZonePower);

    void DoIndustrial(int ZonePwrFlg);

    void DoCommercial(int ZonePwrFlg);

    void DoResidential(int ZonePwrFlg);

    void MakeHosp();

    short GetCRVal();

    void DoResIn(int pop, int value);

    void DoComIn(int pop, int value);

    void DoIndIn(int pop, int value);

    void IncROG(int amount);

    void DoResOut(int pop, int value);

    void DoComOut(int pop, int value);

    void DoIndOut(int pop, int value);

    short getResZonePop(int Ch9);

    short getComZonePop(int Ch9);

    short getIndZonePop(int Ch9);

    void BuildHouse(int value);

    void ResPlop(int Den, int Value);

    void ComPlop(int Den, int Value);

    void IndPlop(int Den, int Value);

    short EvalLot(int x, int y);

    bool ZonePlop(int base);

    short EvalRes(int traf);

    short EvalCom(int traf);

    short EvalInd(int traf);

    short DoFreePop();

    bool SetZPower();


    ////////////////////////////////////////////////////////////////////////


};


////////////////////////////////////////////////////////////////////////
