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


#define MICROPOLIS_VERSION      "5.0"

#define IS_INTEL                1

#define TRUE                    1
#define FALSE                   0

#define RANDOM_RANGE                    0xffff

#define SimWidth                120
#define SimHeight               100

/**
 * Size of the world in horizontal direction
 * @note Must be <= 128 due to PowerMap bitmap
 *       (where 1 row is assumed to be less or equal to 8 words)
 */
static const int WORLD_X = SimWidth;

/**
 * Size of the world in vertical direction
 */
static const int WORLD_Y = SimHeight;

#define HWLDX                   (SimWidth >>1)
#define HWLDY                   (SimHeight >>1)
#define QWX                     (SimWidth >>2)
#define QWY                     (SimHeight >>2)
#define SmX                     (SimWidth >>3)
#define SmY                     ((SimHeight + 7) >>3)

#define EDITOR_W                (WORLD_X * 16)
#define EDITOR_H                (WORLD_Y * 16)
#define MAP_W                   (WORLD_X * 3)
#define MAP_H                   (WORLD_Y * 3)

#define NIL                     0
#define HORIZ                   1
#define VERT                    0

#define HISTLEN                 480
#define MISCHISTLEN             240

#define HISTORY_COUNT                   120

#define HISTORY_TYPE_RES                0
#define HISTORY_TYPE_COM                1
#define HISTORY_TYPE_IND                2
#define HISTORY_TYPE_MONEY              3
#define HISTORY_TYPE_CRIME              4
#define HISTORY_TYPE_POLLUTION  5
#define HISTORY_TYPE_COUNT              6

#define HISTORY_SCALE_SHORT             0
#define HISTORY_SCALE_LONG              1
#define HISTORY_SCALE_COUNT             2

#define POWERMAPROW             ((WORLD_X + 15) / 16)

#define POWERMAPLEN             1700 /* ??? PWRMAPSIZE */
#define POWERWORD(x, y)         (((x) >>4) + ((y) <<3))

#define SETPOWERBIT(x, y)       PowerMap[POWERWORD((x), (y))] |= 1 << ((x) & 15)
#define PWRMAPSIZE              (POWERMAPROW * WORLD_Y)
#define PWRSTKSIZE              ((WORLD_X * WORLD_Y) / 4)

#define POINT_BATCH                             32

#define ALMAP                   0 /* all */
#define REMAP                   1 /* residential */
#define COMAP                   2 /* commercial */
#define INMAP                   3 /* industrial */

#define PRMAP                   4 /* power */
#define RDMAP                   5 /* road */

#define PDMAP                   6 /* population density */
#define RGMAP                   7 /* rate of growth */

#define TDMAP                   8 /* traffic density */
#define PLMAP                   9 /* pollution */
#define CRMAP                   10 /* crime */
#define LVMAP                   11 /* land value */

#define FIMAP                   12 /* fire radius */
#define POMAP                   13 /* police radius */
#define DYMAP                   14 /* dynamic */

#define NMAPS                   15

/* These adjust frequency in Simulate() */

#define VALVERATE               2
#define CENSUSRATE              4
#define TAXFREQ                 48

/* These are names of the 16 colors */
#define COLOR_WHITE             0
#define COLOR_YELLOW            1
#define COLOR_ORANGE            2
#define COLOR_RED               3
#define COLOR_DARKRED           4
#define COLOR_DARKBLUE          5
#define COLOR_LIGHTBLUE         6
#define COLOR_BROWN             7
#define COLOR_LIGHTGREEN        8
#define COLOR_DARKGREEN         9
#define COLOR_OLIVE             10
#define COLOR_LIGHTBROWN        11
#define COLOR_LIGHTGRAY         12
#define COLOR_MEDIUMGRAY        13
#define COLOR_DARKGRAY          14
#define COLOR_BLACK             15


/* Object & Sound Numbers */

#define TRA                     1
#define COP                     2
#define AIR                     3
#define SHI                     4
#define GOD                     5
#define TOR                     6
#define EXP                     7
#define BUS                     8

/* Max # of Objects */

#define OBJN                    9

/* Graph Histories */
#define RES_HIST                0
#define COM_HIST                1
#define IND_HIST                2
#define MONEY_HIST              3
#define CRIME_HIST              4
#define POLLUTION_HIST          5
#define HISTORIES               6
#define ALL_HISTORIES           ((1 <<HISTORIES) - 1)

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
    CONDBIT = 0x4000, ///< bit 14
    BURNBIT = 0x2000, ///< bit 13
    BULLBIT = 0x1000, ///< bit 12, tile is bulldozable
    ANIMBIT = 0x0800, ///< bit 11, tile is animated
    ZONEBIT = 0x0400, ///< bit 10

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
    RUBBLE         = 44,
    // sprite 45, 46 ?
    LASTRUBBLE     = 47,
    FLOOD          = 48,
    // sprite 49, 50 ?
    LASTFLOOD      = 51,

    RADTILE        = 52, ///< Radio-active contaminated tile

    UNUSED_TRASH3  = 53,
    UNUSED_TRASH4  = 54,
    UNUSED_TRASH5  = 55,

    /* Fire */
    FIRE           = 56,
    FIREBASE       = FIRE,
    // sprite 57 -- 62 ?
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

#define residentialState        0
#define commercialState         1
#define industrialState         2
#define fireState               3
#define queryState              4
#define policeState             5
#define wireState               6
#define dozeState               7
#define rrState                 8
#define roadState               9
#define stadiumState            10
#define parkState               11
#define seaportState            12
#define powerState              13
#define nuclearState            14
#define airportState            15
#define networkState            16

#define firstState              residentialState
#define lastState               networkState

#define STATE_CMD               0
#define STATE_TILES             1
#define STATE_OVERLAYS          2
#define STATE_GRAPHS            3

#define ISLAND_RADIUS           18

///////////////////////////////////////////////////
// Scenarios

/** Available scenarios */
enum Scenario {
    SC_NONE,           ///< No scenario (free playing)

    SC_DULLSVILLE,     ///< Dullsville (boredom)
    SC_SAN_FRANCISCO,  ///< San francisco (earth quake)
    SC_HAMBURG,        ///< Hamburg (fire bombs)
    SC_BERN,           ///< Bern (traffic)
    SC_TOKYO,          ///< Tokyo (godzilla)
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
    CVP_CRIME,        ///< Crime
    CVP_POLLUTION,    ///< Pollution
    CVP_HOUSING,      ///< Housing
    CVP_TAXES,        ///< Taxes
    CVP_TRAFFIC,      ///< Traffic
    CVP_UNEMPLOYMENT, ///< Unemployment
    CVP_FIRE,         ///< Fire

    CVP_NUMPROBLEMS,  ///< Number of problems

    CVP_NUMTAKEN,     ///< Number of problems taken

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


class Resource {

public:

  char *buf;
  Quad size;
  char name[4];
  Quad id;
  Resource *next;

};


class StringTable {

public:

  Quad id;
  int lines;
  char **strings;
  StringTable *next;

};


class SimSprite {

public:

  SimSprite *next;
  char *name;
  int type;
  int frame;
  int x;
  int y;
  int width;
  int height;
  int x_offset;
  int y_offset;
  int x_hot;
  int y_hot;
  int orig_x;
  int orig_y;
  int dest_x;
  int dest_y;
  int count;
  int sound_count;
  int dir;
  int new_dir;
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
  short SMapX;


  /**
   * Communication variable with map scan y position.
   *
   * Used all over.
   */
  short SMapY;

  /**
   * Tile at SMapX, SMapY, raw.
   *
   * Used all over.
   */
  short CChr;

  /**
   * Tile at SMapX, SMapY, masked with TILEMASK.
   *
   * Used all over.
   */
  short CChr9;

  /**
   * Number of road tiles in the game.
   *
   * Bridges count as 4 tiles, and high density traffic counts as
   * 2 tiles.
   */
  short RoadTotal;

  /**
   * Total number of rails.
   *
   * No penalty for bridges or high traffic density.
   */
  short RailTotal;

  /**
   * Number of fires.
   */
  short FirePop;

  /**
   * Residential zone population.
   *
   * Depends on level of zone development.
   */
  short ResPop;

  /**
   * Commercial zone population.
   *
   * Depends on level of zone development.
   */
  short ComPop;

  /**
   * Industrial zone population.
   *
   * Depends on level of zone development.
   */
  short IndPop;

  /**
   * Total population.
   *
   * Includes residential pop / 8 plus industrial pop plus commercial
   * pop.
   */
  short TotalPop;

  /**
   * Last total population.
   *
   * Not used?
   */
  short LastTotalPop;

  /**
   * Residential zone population.
   */
  short ResZPop;

  /**
   * Commercial zone population.
   */
  short ComZPop;

  /**
   * Industrial zone population.
   */
  short IndZPop;

  /**
   * Total zone population.
   */
  short TotalZPop;

  /**
   * Hospital population.
   */
  short HospPop;

  /**
   * Church population.
   */
  short ChurchPop;

  /**
   * Stadium population.
   */
  short StadiumPop;

  /**
   * Police station population.
   */
  short PolicePop;

  /**
   * Fire station population.
   */
  short FireStPop;

  /**
   * Coal power plant population.
   */
  short CoalPop;

  /**
   * Nuclear power plant population.
   */
  short NuclearPop;

  /**
   * Seaport population.
   */
  short PortPop;

  /**
   * Airport population.
   */
  short APortPop;

  /**
   * Need hospital?
   *
   * 0 if no, 1 if yes, -1 if too many.
   */
  short NeedHosp;

  /**
   * Need church?
   *
   * 0 if no, 1 if yes, -1 if too many.
   */
  short NeedChurch;

  /**
   * Average crime.
   *
   * Affected by land value, population density, police station
   * distance.
   */
  short CrimeAverage;

  /**
   * Average pollution.
   *
   * Affected by PollutionMem, which is effected by traffic, fire,
   * radioactivity, industrial zones, seaports, airports, power
   * plants.
   */
  short PolluteAverage;

  /**
   * Land value average.
   *
   * Affected by distance from city center, development density
   * (terrainMem), pollution, and crime.
   */
  short LVAverage;

  /** @name Dates */
  //@{

  /**

   * City time tick counter. 48 ticks per year.
   *
   * Four ticks per 12 months, so one tick is about a week (7.6 days).
   */
  Quad CityTime;

  /**
   * City month, 4 ticks per month.
   */
  Quad CityMonth;

  /**
   * City year, (CityTime / 48) + StartingYear.
   */
  Quad CityYear;

  /**
   * City starting year.
   */
  short StartingYear;

  //@}

  /* Maps */

  /**
   * Two-dimensional array of map tiles.
   *
   * Map[0 <= x < 120][0 <= y < 100]
   */
  short *Map[WORLD_X];

  /**
   * 10 year residential history maximum value.
   */
  short ResHisMax;

  /**
   * 120 year residential history maximum value.
   */
  short Res2HisMax;

  /**
   * 10 year commercial history maximum value.
   */
  short ComHisMax;

  /**
   * 120 year commercial history maximum value.
   */
  short Com2HisMax;

  /**
   * 10 year industrial history maximum value.
   */
  short IndHisMax;

  /**
   * 120 year industrial history maximum value.
   */
  short Ind2HisMax;

  /**
   * Census changed flag.
   *
   * Need to redraw census dependent stuff. Set by ChangeCensus,
   * UpdateGraphs, TakeCensus, Take2Census, loadFile.
   */
  short CensusChanged;

  /**
   * Message number to display asynchronously.
   *
   * Clean this up to use a simpler interface, and a queue.
   * Might need to collapse some messages.
   */
  short MessagePort;

  /**
   * Message X location.
   */
  short MesX;

  /**
   * Message Y location.
   */
  short MesY;

  /** @name Budget */
  //@{

  /**
   * Spending on roads.
   */
  Quad RoadSpend;

  /**
   * Spending on police stations.
   */
  short PoliceSpend;

  /**
   * Spending on fire stations.
   */
  short FireSpend;

  /**
   * Requested funds for roads.
   *
   * Depends on number of roads, rails, and game level.
   */
  Quad RoadFund;

  /**
   * Requested funds for police stations.
   *
   * Depends on police station population.
   */
  short PoliceFund;

  /**
   * Requested funds for fire stations.
   *
   * Depends on fire station population.
   */
  short FireFund;

  /**
   * Radio of road spending over road funding, times 32.
   */
  short RoadEffect;

  /**
   * Radio of police spending over road funding, times 32.
   */
  short PoliceEffect;

  /**
   * Radio of fire spending over road funding, times 32.
   */
  short FireEffect;

  /**
   * Funds from taxes.
   *
   * Depends on total population, average land value, city tax, and
   * game level.
   */
  Quad TaxFund;

  /**
   * City tax rate.
   */
  short CityTax;

  /**
   * Tax port flag.
   *
   * Apparently never used. CollectTax checks it.
   *
   * @todo Apparently TaxFlag is never set to true in MicropolisEngine
   *       or the TCL code.
   * @todo Don should check old Mac code to see if it's ever set.
   * @todo Variable is always \c 0. Decide whether to keep it, and if yes,
   *       create means to modify its value
   * @todo It looks like a boolean. If we keep it, modify accordingly
   */
  short TaxFlag;

  //@}

  /**
   * Population density map.
   */
  Byte *PopDensity[HWLDX];

  /**
   * Traffic map.
   */
  Byte *TrfDensity[HWLDX];

  /**
   * Pollution map.
   */
  Byte *PollutionMem[HWLDX];

  /**
   * Land value mep.
   */
  Byte *LandValueMem[HWLDX];

  /**
   * Crime map.
   */
  Byte *CrimeMem[HWLDX];

  /**
   * Temporary map.
   *
   * Used to smooth population density, pollution.
   */
  Byte *tem[HWLDX];

  /**
   * Temporary map 2.
   *
   * Used to smooth population density, pollution.
   */
  Byte *tem2[HWLDX];

  /**
   * Terrain development  density map.
   *
   * Used to calculate land value.
   */
  Byte *TerrainMem[QWX];

  /**
   * Temporary map Q.
   *
   * Used to smooth development density, for TerrainMem.
   */
  Byte *Qtem[QWX];

  /**
   * Rate of growth map.
   *
   * Affected by DecROGMem, incROG called by zones. Decreased by fire
   * explosions from sprites, fire spreading. Doesn't seem to
   * actually feed back into the simulation. Output only.
   */
  short RateOGMem[SmX][SmY];

  /**
   * Fire station map.
   *
   * Affected by fire stations, powered, fire funding ratio, road
   * access. Affects how long fires burn.
   */
  short FireStMap[SmX][SmY];

  /**
   * Police station map.
   *
   * Affected by police stations, powered, police funding ratio, road
   * access. Affects crime rate.
   */
  short PoliceMap[SmX][SmY];

  /**
   * Copy of police station map to display.
   */
  short PoliceMapEffect[SmX][SmY];

  /**
   * Copy of fire station map to display.
   */
  short FireRate[SmX][SmY];

  /**
   * Commercial rate map.
   *
   * Depends on distance to city center. Effects commercial zone
   * evaluation.
   */
  short ComRate[SmX][SmY];

  /**
   * Temporary array for smoothing fire and police station maps.
   */
  short STem[SmX][SmY];

  /**
   * Memory for TerrainMem array.
   */
  Ptr terrainBase;

  /**
   * Memory for Qtem array.
   */
  Ptr qTemBase;

  /**
   * Memory for tem array.
   */
  Ptr tem1Base;

  /**
   * Memory for tem2 array.
   */
  Ptr tem2Base;

  /**
   * Memory for PopDensity array.
   */
  Ptr popPtr;

  /**
   * Memory for TrfDensity array.
   */
  Ptr trfPtr;

  /**
   * Memory for PollutionMem array.
   */
  Ptr polPtr;

  /**
   * Memory for LandValueMem array.
   */
  Ptr landPtr;

  /**
   * Memory for CrimeMem array.
   */
  Ptr crimePtr;

  /**
   * Memory for Map array.
   */
  unsigned short *mapPtr;

  /**
   * Residential population history.
   */
  short *ResHis;

  /**
   * Commercial population history.
   */
  short *ComHis;

  /**
   * Industrial population history.
   */
  short *IndHis;

  /**
   * Money history.
   */
  short *MoneyHis;

  /**
   * Pollution history.
   */
  short *PollutionHis;

  /**
   * Crime history.
   */
  short *CrimeHis;

  /**
   * Memory used to save miscelaneous game values in save file.
   */
  short *MiscHis;

  /**
   * Power distribution bitmap.
   */
  short *PowerMap;


  void initMapArrays();

  void destroyMapArrays();

#ifdef SWIG
// This tells SWIG that minValResult, maxValResult are output parameters,
// which will be returned in a tuple of length two.
%apply short *OUTPUT { short *minValResult };
%apply short *OUTPUT { short *maxValResult };
#endif

  void GetHistoryRange(
        int historyType,
        int historyScale,
        short *minValResult,
        short *maxValResult);

  short GetHistory(
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
   * Causes ReallyDrawCurrPercents to be called when UpdateBudgetWindow called.
   * @todo Clean this up.
   */
  int MustDrawCurrPercents;

  /**
   * Flag set when drawBudgetWindow called.
   *
   * Causes ReallyDrawBudgetWindow to be called when UpdateBudgetWindow called.
   * @todo Clean this up.
   */
  int MustDrawBudgetWindow;


  void InitFundingLevel();

  void DoBudget();

  void DoBudgetFromMenu();

  void DoBudgetNow(
    int fromMenu);

  void drawBudgetWindow();

  void ReallyDrawBudgetWindow();

  void drawCurrPercents();

  void ReallyDrawCurrPercents();

  void UpdateBudgetWindow();

  void UpdateBudget();

  void ShowBudgetWindowAndStartWaiting();

  void SetBudget(
    char *flowStr,
    char *previousStr,
    char *currentStr,
    char *collectedStr,
    short tax);

  void SetBudgetValues(
    char *roadGot,
    char *roadWant,
    char *policeGot,
    char *policeWant,
    char *fireGot,
    char *fireWant);


  ////////////////////////////////////////////////////////////////////////
  // connect.cpp

public:


  int ConnecTile(
    short x,
    short y,
    short *TileAdrPtr,
    short Command);

  int LayDoze(
    int x,
    int y,
    short *TileAdrPtr);

  int LayRoad(
    int x,
    int y,
    short *TileAdrPtr);

  int LayRail(
    int x,
    int y,
    short *TileAdrPtr);

  int LayWire(
    int x,
    int y,
    short *TileAdrPtr);

  void FixZone(
    int x,
    int y,
    short *TileAdrPtr);

  void FixSingle(
    int x,
    int y,
    short *TileAdrPtr);


  ////////////////////////////////////////////////////////////////////////
  // disasters.cpp

public:


  /**
   * Size of flooding disaster.
   */
  short FloodCnt;


  void MakeMeltdown();

  void FireBomb();

  void MakeEarthquake();

  void MakeFire();

  void MakeFlood();

private:

  void DoDisasters();

  void ScenarioDisaster();

  void SetFire();

  bool Vulnerable(int tem);

  void DoFlood();


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
  short problemOrder[CVP_NUMTAKEN];

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
   * Depends on RoadTotal, RailTotal, PolicePop, FireStPop, HospPop,
   * StadiumPop, PortPop, APortPop, coalPop, and NuclearPop, and
   * their respective values.
   */
  Quad cityAssValue;

  /**
   * City class.
   *
   * 0: village, 1: town, 2: city, 3: capital, 4: metropolis, 5: megalopolis.
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
  short deltaCityScore;

  /**
   * Average traffic.
   *
   * Depends on average traffic density of tiles with non-zero land value.
   */
  short trafficAverage;


  void CityEvaluation();

  void EvalInit();

  void DoPopNum();

  void DoProblems();

  void getAssValue();

  void voteProblems();

  short AverageTrf();

  short GetUnemployment();

  short GetFire();

  void GetScore();

  void DoVotes();

  void doScoreCard();

  void ChangeEval();

  void scoreDoer();


  ////////////////////////////////////////////////////////////////////////
  // fileio.cpp

public:


  int load_file(
    char *filename,
    char *dir);

  int loadFile(
    char *filename);

  int saveFile(
    char *filename);

  void LoadScenario(Scenario s);

  void DidLoadScenario();

  int LoadCity(
    char *filename);

  void DidLoadCity();

  void DidntLoadCity(
    char *msg);

  void SaveCity();

  void DoSaveCityAs();

  void DidSaveCity();

  void DidntSaveCity(
    char *msg);

  void SaveCityAs(
    char *filename);


  ////////////////////////////////////////////////////////////////////////
  // generate.cpp

public:


  // It would be nice to open up the terrain generator, and make its features available incrementally as city building tools.
  // The user should be able to place water and trees, and it should dynamically smooth the edges.
  // The user interface could restrict the user to only drawing terrain before any zones were built,
  // but it would be best if the terrain editing tools worked properly when there were zones built
  // (by automatically bulldozing zones whose underlying terrain it's modifying).

  // Starting X location of the terrain generator.
  // Only used internally by the terrain generator. Should be private.
  short XStart;

  // Starting Y location of the terrain generator.
  // Only used internally by the terrain generator. Should be private.
  short YStart;

  // Current X location of the terrain generator.
  // Only used internally by the terrain generator. Should be private.
  short MapX;

  // Current Y location of the terrain generator.
  // Only used internally by the terrain generator. Should be private.
  short MapY;

  // Current direction of the terrain generator.
  // Only used internally by the terrain generator. Should be private.
  short Dir;

  // Last direction of the terrain generator.
  // Only used internally by the terrain generator. Should be private.
  short LastDir;

  // Controls the level of tree creation.
  // -1 => create default number of trees, 0 => never create trees, >0 => create more trees
  int TreeLevel;

  // Controls the level of lake creation.
  // -1 => create default number of lakes, 0 => never create lakes, >0 => create more lakes
  int LakeLevel;

  // Controls the level of river curviness.
  // -1 => default curve level, 0 => never create rivers, >0 => create curvier rivers
  int CurveLevel;

  // Controls how often to create an island.
  // -1 => 10% chance of island, 0 => never create island, 1 => always create island
  int CreateIsland;


  void GenerateNewCity() ;

  void GenerateSomeCity(
    int r);

  void GenerateMap(
    int r);

  void ClearMap();

  void ClearUnnatural();

  void MakeNakedIsland();

  void MakeIsland();

  void MakeLakes();

  void GetRandStart();

  void MoveMap(
    short dir);

  void TreeSplash(
    short xloc,
    short yloc);

  void DoTrees();

  void SmoothRiver();

  int IsTree(
    int cell);

  void SmoothTrees();

  void DoRivers();

  void DoBRiv();

  void DoSRiv();

  void PutOnMap(
    short Mchar,
    short Xoff,
    short Yoff);

  void BRivPlop();

  void SRivPlop();

  void SmoothWater();


  ////////////////////////////////////////////////////////////////////////
  // graph.cpp

public:


  // Flag that tells if there is a new graph to draw.
  // This should be replaced by a general purpose view updating system.
  short NewGraph;

  // 10 year history graphs.
  unsigned char *History10[HISTORIES];

  // 120 year history graphs.
  unsigned char *History120[HISTORIES];

  //
  int HistoryInitialized;

  short Graph10Max;

  short Graph120Max;

  int GraphUpdateTime;

  static char *HistName[];

  static unsigned char HistColor[];


  void drawMonth(
    short *hist,
    unsigned char *s,
    float scale);

  void doAllGraphs();

  void ChangeCensus();

  void graphDoer();

  void initGraphs();

  void InitGraphMax();


  ////////////////////////////////////////////////////////////////////////
  // initialize.cpp

public:


  void InitWillStuff();

  void ResetMapState();

  void ResetEditorState();


  ////////////////////////////////////////////////////////////////////////
  // main.cpp

public:


  const char *MicropolisVersion;

  int sim_loops;

  int sim_delay;

  int sim_skips;

  int sim_skip;

  /**
   * Simulation is paused
   * @todo Variable has reversed logic, maybe use sim_running instead?
   */
  bool sim_paused;

  int sim_paused_speed;

  int heat_steps;

  int heat_flow;

  int heat_rule;

  int heat_wrap;

  char *CityFileName;

  int MultiPlayerMode;

  int TilesAnimated;

  int DoAnimation;

  int DoMessages;

  int DoNotices;

  short *CellSrc;

  short *CellDst;


  void env_init();

  void sim_init();

  void sim_update();

  void sim_heat();

  void sim_loop(
    int doSim);

  void sim_tick();


  ////////////////////////////////////////////////////////////////////////
  // map.cpp

public:


#if 0
  ////////////////////////////////////////////////////////////////////////
  // Disabled this small map drawing, filtering and overlaying code.
  // Going to re-implement it in the tile engine and Python.


  int DynamicData[32];


  void drawAll(
    SimView *view);

  void drawRes(
    SimView *view);

  void drawCom(
    SimView *view);

  void drawInd(
    SimView *view);

  void drawLilTransMap(
    SimView *view);

  void drawPower(
    SimView *view);

  int dynamicFilter(
    int col,
    int row);

  void drawDynamic(
    SimView *view);

  short GetCI(
    short x);

  void drawPopDensity(
   SimView *view);

  void drawRateOfGrowth(
    SimView *view);

  void drawTrafMap(
    SimView *view);

  void drawPolMap(
    SimView *view);

  void drawCrimeMap(
    SimView *view);

  void drawLandMap(
    SimView *view);

  void drawFireRadius(
    SimView *view);

  void drawPoliceRadius(
    SimView *view);

  void MemDrawMap(
    SimView *view);

  void ditherMap(
    SimView *view);

  void maybeDrawRect(
    SimView *view,
    int val,
    int x,
    int y,
    int w,
    int h);

  void drawRect(
    SimView *view,
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


  Quad lastCityPop;

  short LastCategory;

  short LastPicNum;

  /**
   * Enable auto goto
   *
   * When enabled and an important event happens, the map display will jump to
   * the location of the event
   */
  bool autoGo;

  /**
   * Do we have a valid message in LastMessage?
   * @todo Remove the variable by using LastMessage instead (for example the
   *       empty string)
   */
  bool HaveLastMessage;

  char LastMessage[256];


  void SendMessages();

  void CheckGrowth();

  void DoScenarioScore(Scenario type);

  void ClearMes();

  int SendMes(
    int Mnum);

  void SendMesAt(
    short Mnum,
    short x,
    short y);

  void doMessage();

  void DoAutoGoto(
    short x,
    short y,
    char *msg);

  void SetMessageField(
    char *str);

  void DoShowPicture(
    short id);

  void DoLoseGame();

  void DoWinGame();


  ////////////////////////////////////////////////////////////////////////
  // power.cpp

public:


  int PowerStackNum;

  short PowerStackX[PWRSTKSIZE];

  short PowerStackY[PWRSTKSIZE];

  /**
   * Maximal power that the combined coal and nuclear power plants can deliver
   * @see NumPower CoalPop NuclearPop
   */
  Quad MaxPower;
  Quad NumPower; ///< Amount of power used


  int MoveMapSim(
    short MDir);

  short TestForCond(
    short TFDir);

  void DoPowerScan();

  void PushPowerStack();

  void PullPowerStack();


  ////////////////////////////////////////////////////////////////////////
  // random.cpp

public:


  UQuad nextRandom;


  int sim_rand();

  void sim_srand(
    UQuad seed);

  short Rand(
    short range);

  int Rand16();

  int Rand16Signed();

  short ERand(
    short limit);

  void RandomlySeedRand();

  void SeedRand(
    int seed);


  ////////////////////////////////////////////////////////////////////////
  // resource.cpp

public:


  char *HomeDir;

  char *ResourceDir;

  char *KeyDir;

  char *HostName;

  Resource *Resources;

  StringTable *StringTables;


  char **GetResource(
    char *name,
    Quad id);

  void ReleaseResource(
    char **r);

  Quad ResourceSize(
    char **h);

  char *ResourceName(
    char **h);

  Quad ResourceID(
    char **h);

  void GetIndString(
    char *str,
    int id,
    short num);



  ////////////////////////////////////////////////////////////////////////
  // scan.cpp

public:


  short NewMap;

  short NewMapFlags[NMAPS];

  short CCx;

  short CCy;

  short CCx2;

  short CCy2;

  short PolMaxX;

  short PolMaxY;

  short CrimeMaxX;

  short CrimeMaxY;

  Quad DonDither;


  void FireAnalysis();

  void PopDenScan();

  int GetPDen(
    int Ch9);

  void PTLScan();

  int GetPValue(
    int loc);

  int GetDisCC(
    int x,
    int y);

  void CrimeScan();

  void SmoothTerrain();

  void DoSmooth();

  void DoSmooth2();

  void ClrTemArray();

  void SmoothFSMap();

  void SmoothPSMap();

  void DistIntMarket();


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

  short DisasterWait;

  Scenario ScoreType;     ///< The type of score table to use

  short ScoreWait;

  short PwrdZCnt;

  short unPwrdZCnt;

  short NewPower; /* post */

  short AvCityTax;

  short Scycle;

  short Fcycle;

  short Spdcycle;

  bool DoInitialEval; ///< Need to perform initial city evaluation

  short MeltX;

  short MeltY;


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

  void MapScan(
    int x1,
    int x2);

  void DoRail();

  void DoRadTile();

  void DoRoad();

  int DoBridge();

  int GetBoatDis();

  void DoFire();

  void FireZone(
    int Xloc,
    int Yloc,
    int ch);

  void RepairZone(
    short ZCent,
    short zsize);

  void DoSPZone(
    short PwrOn);

  void DrawStadium(
    int z);

  void DoAirport();

  void CoalSmoke(
    int mx,
    int my);

  void DoMeltdown(
    int SX,
    int SY);


  ////////////////////////////////////////////////////////////////////////
  // sprite.cpp

public:


  SimSprite *spriteList;

  int spriteCount;

  SimSprite *FreeSprites;

  SimSprite *GlobalSprites[OBJN];

  short CrashX;

  short CrashY;

  int absDist;

  short Cycle;


  SimSprite *NewSprite(
    char *name,
    int type,
    int x,
    int y);

  void InitSprite(
    SimSprite *sprite,
    int x,
    int y);

  void DestroyAllSprites();

  void DestroySprite(
    SimSprite *sprite);

  SimSprite *GetSprite(
    int type);

  SimSprite *MakeSprite(
    int type,
    int x,
    int y);

  SimSprite *MakeNewSprite(
    int type,
    int x,
    int y);

  void DrawObjects();

  void DrawSprite(
    SimSprite *sprite);

  short GetChar(
    int x,
    int y);

  short TurnTo(
    int p,
    int d);

  short TryOther(
    int Tpoo,
    int Told,
    int Tnew);

  short SpriteNotInBounds(
    SimSprite *sprite);

  short GetDir(
    int orgX,
    int orgY,
    int desX,
    int desY);

  short GetDis(
    int x1,
    int y1,
    int x2,
    int y2);

  int CheckSpriteCollision(
    SimSprite *s1,
    SimSprite *s2);

  void MoveObjects();

  void DoTrainSprite(
    SimSprite *sprite);

  void DoCopterSprite(
    SimSprite *sprite);

  void DoAirplaneSprite(
    SimSprite *sprite);

  void DoShipSprite(
    SimSprite *sprite);

  void DoMonsterSprite(
    SimSprite *sprite);

  void DoTornadoSprite(
    SimSprite *sprite);

  void DoExplosionSprite(
    SimSprite *sprite);

  void DoBusSprite(
    SimSprite *sprite);

  int CanDriveOn(
    int x,
    int y);

  void ExplodeSprite(
    SimSprite *sprite);

  int checkWet(
    int x);

  void Destroy(
    int ox,
    int oy);

  void OFireZone(
    int Xloc,
    int Yloc,
    int ch);

  void StartFire(
    int x,
    int y);

  void GenerateTrain(
    int x,
    int y);

  void GenerateBus(
    int x,
    int y);

  void GenerateShip();

  void MakeShipHere(
    int x,
    int y);

  void MakeMonster();

  void MonsterHere(
    int x,
    int y);

  void GenerateCopter(
    int x,
    int y);

  void GeneratePlane(
    int x,
    int y);

  void MakeTornado();

  void MakeExplosion(
    int x,
    int y);

  void MakeExplosionAt(
    int x,
    int y);


  ////////////////////////////////////////////////////////////////////////
  // stubs.cpp

public:


  Quad TotalFunds;

  short PunishCnt;

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

  /**
   * Difficulty level of the game (0..2)
   * @todo Rename to game_level and create a GameLevel enum
   */
  short GameLevel;

  short InitSimLoad;

  Scenario ScenarioID; ///< Scenario being played

  short SimSpeed;

  short SimMetaSpeed;

  bool UserSoundOn; ///< Enable sound

  char *CityName;

  /**
   * Disable disasters
   * @todo Variable has reverse logic,
   *       we should use AllowDisasters or EnableDisasters instead
   */
  bool NoDisasters;

  short MesNum;

  bool EvalChanged; ///< The evaluation window should be shown to the user

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

  void Spend(
    int dollars);

  void SetFunds(
    int dollars);

  Quad TickCount();

  Ptr NewPtr(
    int size);

  void FreePtr(
    void *data);

  void DoPlayNewCity();

  void DoReallyStartGame();

  void DoStartLoad();

  void DoStartScenario(
    int scenario);

  void DropFireBombs();

  void InitGame();

  void Callback(
        const char *name,
        const char *params,
        ...);

  void DoEarthquake();

  void InvalidateEditors();

  void InvalidateMaps();

  void InitializeSound();

  void MakeSound(
    const char *channel,
    const char *sound);

  int getTile(
        int x,
        int y);

  void *getMapBuffer();


  ////////////////////////////////////////////////////////////////////////
  // tool.cpp

public:


  int OverRide;

  int Expensive;

  int Players;

  int Votes;

  int PendingTool;

  int PendingX;

  int PendingY;

  int last_x;

  int last_y;

  int tool_x;

  int tool_y;

  static Quad CostOf[];

  static short toolSize[];

  static short toolOffset[];

  static Quad toolColors[];


  int putDownPark(
    short mapH,
    short mapV);

  int putDownNetwork(
    short mapH,
    short mapV);

  short checkBigZone(
    short id,
    short *deltaHPtr,
    short *deltaVPtr);

  void check3x3border(
    short xMap,
    short yMap);

  int check3x3(
    short mapH,
    short mapV,
    short base,
    short tool);

  void check4x4border(
    short xMap,
    short yMap);

  short check4x4(
    short mapH,
    short mapV,
    short base,
    short aniFlag,
    short tool);

  void check6x6border(
    short xMap,
    short yMap);

  short check6x6(
    short mapH,
    short mapV,
    short base,
    short tool);

  int getDensityStr(
    short catNo,
    short mapH,
    short mapV);

  void doZoneStatus(
    short mapH,
    short mapV);

  void DoShowZoneStatus(
    char *str,
    char *s0,
    char *s1,
    char *s2,
    char *s3,
    char *s4,
    int x,
    int y);

  void DidTool(
    const char *name,
    short x,
    short y);

  int query_tool(
    short x,
    short y);

  int bulldozer_tool(
    short x,
    short y);

  int road_tool(
    short x,
    short y);

  int rail_tool(
    short x,
    short y);

  int wire_tool(
    short x,
    short y);

  int park_tool(
    short x,
    short y);

  int residential_tool(
    short x,
    short y);

  int commercial_tool(
    short x,
    short y);

  int industrial_tool(
    short x,
    short y);

  int police_dept_tool(
    short x,
    short y);

  int fire_dept_tool(
    short x,
    short y);

  int stadium_tool(
    short x,
    short y);

  int coal_power_plant_tool(
    short x,
    short y);

  int nuclear_power_plant_tool(
    short x,
    short y);

  int seaport_tool(
    short x,
    short y);

  int airport_tool(
    short x,
    short y);

  int network_tool(
    short x,
    short y);

  int do_tool(
    short state,
    short x,
    short y,
    short first);

  void DoTool(
    short tool,
    short x,
    short y);

  void ToolDown(
        short tool,
    int x,
    int y);

  void ToolUp(
        short tool,
    int x,
    int y);

  void ToolDrag(
        short tool,
    int px,
    int py);

  void DoPendTool(
    int tool,
    int x,
    int y);

private:

  bool tally(short tileValue);

  short checkSize(short tile_value);

  void putRubble(int x, int y, int size);

  ////////////////////////////////////////////////////////////////////////
  // traffic.cpp

public:

  /* Position stack */
  short PosStackN; ///< Position stack top pointer, points to top position
  short SMapXStack[MAX_TRAFFIC_DISTANCE + 1]; ///< X positions
  short SMapYStack[MAX_TRAFFIC_DISTANCE + 1]; ///< Y positions

  Direction LDir; ///< Last moved direction

  short TrafMaxX;

  short TrafMaxY;


  short MakeTraf(ZoneType dest);

  void SetTrafMem();

  void PushPos();

  void PullPos();

  bool FindPRoad();

  bool FindPTele();

  bool TryDrive();

  bool TryGo(int dist);

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

  Quad LastCityTime;

  Quad LastCityYear;

  Quad LastCityMonth;

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

  void SetDemand(
    double r,
    double c,
    double i);

  void updateOptions();

  void UpdateOptionsMenu(
    int options);

  void UpdateUserInterface();


  ////////////////////////////////////////////////////////////////////////
  // utilities.cpp

public:


  void makeDollarDecimalStr(
    char *numStr,
    char *dollarStr);

  void Pause();

  void Resume();

  void setSpeed(
    short speed);

  void setSkips(
    int skips);

  void SetGameLevelFunds(
    short level);

  void SetGameLevel(
    short level);

  void UpdateGameLevel();

  void setCityName(
    char *name);

  void setAnyCityName(
    char *name);

  void SetYear(
    int year);

  int CurrentYear();

  void DoNewGame();

  void DoGeneratedCityImage(
    char *name,
    int time,
    int pop,
    char *cityClassName,
    int score);


  ////////////////////////////////////////////////////////////////////////
  // zone.cpp

public:


  void DoZone();

  void DoHospChur();

  void SetSmoke(
    int ZonePower);

  void DoIndustrial(
    int ZonePwrFlg);

  void DoCommercial(
    int ZonePwrFlg);

  void DoResidential(
    int ZonePwrFlg);

  void MakeHosp();

  short GetCRVal();

  void DoResIn(
    int pop,
    int value);

  void DoComIn(
    int pop,
    int value);

  void DoIndIn(
    int pop,
    int value);

  void IncROG(
    int amount);

  void DoResOut(
    int pop,
    int value);

  void DoComOut(
    int pop,
    int value);

  void DoIndOut(
    int pop,
    int value);

  short RZPop(
    int Ch9);

  short CZPop(
    int Ch9);

  short IZPop(
    int Ch9);

  void BuildHouse(
    int value);

  void ResPlop(
    int Den,
    int Value);

  void ComPlop(
    int Den,
    int Value);

  void IndPlop(
    int Den,
    int Value);

  short EvalLot(
    int x,
    int y);

  short ZonePlop(
    int base);

  short EvalRes(
    int traf);

  short EvalCom(
    int traf);

  short EvalInd(
    int traf);

  short DoFreePop();

  short SetZPower();


  ////////////////////////////////////////////////////////////////////////


};


////////////////////////////////////////////////////////////////////////
