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

#define SimWidth                120
#define SimHeight               100

#define WORLD_X                 SimWidth
#define WORLD_Y                 SimHeight
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

#define PROBNUM                 10

#define HISTLEN                 480
#define MISCHISTLEN             240

#define HISTORY_COUNT			120

#define HISTORY_TYPE_RES		0
#define HISTORY_TYPE_COM		1
#define HISTORY_TYPE_IND		2
#define HISTORY_TYPE_MONEY		3
#define HISTORY_TYPE_CRIME		4
#define HISTORY_TYPE_POLLUTION	5
#define HISTORY_TYPE_COUNT		6

#define HISTORY_SCALE_SHORT		0
#define HISTORY_SCALE_LONG		1
#define HISTORY_SCALE_COUNT		2

#define POWERMAPROW             ((WORLD_X + 15) / 16)

#define POWERMAPLEN             1700 /* ??? PWRMAPSIZE */
#define POWERWORD(x, y)         (((x) >>4) + ((y) <<3))

#define SETPOWERBIT(x, y)       PowerMap[POWERWORD((x), (y))] |= 1 << ((x) & 15)
#define PWRMAPSIZE              (POWERMAPROW * WORLD_Y)
#define PWRSTKSIZE              ((WORLD_X * WORLD_Y) / 4)

#define POINT_BATCH				32

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

/* Status Bits */

#define PWRBIT                  32768   /*20    bit 15  */
#define CONDBIT                 16384   /*10    bit 14  */
#define BURNBIT                 8192    /*8     bit 13  */
#define BULLBIT                 4096    /*4     bit 12  */
#define ANIMBIT                 2048    /*2     bit 11  */
#define ZONEBIT                 1024    /*1     bit 10  */
#define ALLBITS                 64512   /*  mask for upper 6 bits       */
#define LOMASK                  1023    /*      mask for low 10 bits    */

#define BLBNBIT                 (BULLBIT+BURNBIT)
#define BLBNCNBIT               (BULLBIT+BURNBIT+CONDBIT)
#define BNCNBIT                 (BURNBIT+CONDBIT)

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

/* Character Mapping */

#define DIRT                    0
#define RIVER                   2
#define WATER_LOW               RIVER /* 2 */
#define REDGE                   3
#define CHANNEL                 4
#define FIRSTRIVEDGE            5
#define LASTRIVEDGE             20
#define WATER_HIGH              LASTRIVEDGE /* 20 */
#define TREEBASE                21
#define WOODS_LOW               TREEBASE /* 21 */
#define LASTTREE                36
#define WOODS                   37
#define UNUSED_TRASH1           38
#define UNUSED_TRASH2           39
#define WOODS_HIGH              UNUSED_TRASH2 /* 39 */
#define WOODS2                  40
#define WOODS3                  41
#define WOODS4                  42
#define WOODS5                  43
#define RUBBLE                  44
#define LASTRUBBLE              47
#define FLOOD                   48
#define LASTFLOOD               51
#define RADTILE                 52
#define UNUSED_TRASH3           53
#define UNUSED_TRASH4           54
#define UNUSED_TRASH5           55
#define FIRE                    56
#define FIREBASE                56
#define LASTFIRE                63
#define ROADBASE                64
#define HBRIDGE                 64
#define VBRIDGE                 65
#define ROADS                   66
#define ROADS2                  67
#define ROADS3                  68
#define ROADS4                  69
#define ROADS5                  70
#define ROADS6                  71
#define ROADS7                  72
#define ROADS8                  73
#define ROADS9                  74
#define ROADS10                 75
#define INTERSECTION            76
#define HROADPOWER              77
#define VROADPOWER              78
#define BRWH                    79
#define LTRFBASE                80
#define BRWV                    95
#define BRWXXX1                 111
#define BRWXXX2                 127
#define BRWXXX3                 143
#define HTRFBASE                144
#define BRWXXX4                 159
#define BRWXXX5                 175
#define BRWXXX6                 191
#define LASTROAD                206
#define BRWXXX7                 207
#define POWERBASE               208
#define HPOWER                  208
#define VPOWER                  209
#define LHPOWER                 210
#define LVPOWER                 211
#define LVPOWER2                212
#define LVPOWER3                213
#define LVPOWER4                214
#define LVPOWER5                215
#define LVPOWER6                216
#define LVPOWER7                217
#define LVPOWER8                218
#define LVPOWER9                219
#define LVPOWER10               220
#define RAILHPOWERV             221
#define RAILVPOWERH             222
#define LASTPOWER               222
#define UNUSED_TRASH6           223
#define RAILBASE                224
#define HRAIL                   224
#define VRAIL                   225
#define LHRAIL                  226
#define LVRAIL                  227
#define LVRAIL2                 228
#define LVRAIL3                 229
#define LVRAIL4                 230
#define LVRAIL5                 231
#define LVRAIL6                 232
#define LVRAIL7                 233
#define LVRAIL8                 234
#define LVRAIL9                 235
#define LVRAIL10                236
#define HRAILROAD               237
#define VRAILROAD               238
#define LASTRAIL                238
#define ROADVPOWERH             239 /* bogus? */
#define RESBASE                 240
#define FREEZ                   244
#define HOUSE                   249
#define LHTHR                   249
#define HHTHR                   260
#define RZB                     265
#define HOSPITAL                409
#define CHURCH                  418
#define COMBASE                 423
#define COMCLR                  427
#define CZB                     436
#define COMLAST					609
#define INDBASE                 612
#define INDCLR                  616
#define LASTIND                 620
#define IND1                    621
#define IZB                     625
#define IND2                    641
#define IND3                    644
#define IND4                    649
#define IND5                    650
#define IND6                    676
#define IND7                    677
#define IND8                    686
#define IND9                    689
#define PORTBASE                693
#define PORT                    698
#define LASTPORT                708
#define AIRPORTBASE             709
#define RADAR                   711
#define AIRPORT                 716
#define COALBASE                745
#define POWERPLANT              750
#define LASTPOWERPLANT          760
#define FIRESTBASE              761
#define FIRESTATION             765
#define POLICESTBASE            770
#define POLICESTATION           774
#define STADIUMBASE             779
#define STADIUM                 784
#define FULLSTADIUM             800
#define NUCLEARBASE             811
#define NUCLEAR                 816
#define LASTZONE                826
#define LIGHTNINGBOLT           827
#define HBRDG0                  828
#define HBRDG1                  829
#define HBRDG2                  830
#define HBRDG3                  831
#define HBRDG_END               832
#define RADAR0                  832
#define RADAR1                  833
#define RADAR2                  834
#define RADAR3                  835
#define RADAR4                  836
#define RADAR5                  837
#define RADAR6                  838
#define RADAR7                  839
#define FOUNTAIN                840
#define INDBASE2                844
#define TELEBASE                844
#define TELELAST                851
#define SMOKEBASE               852
#define TINYEXP                 860
#define SOMETINYEXP             864
#define LASTTINYEXP             867
#define TINYEXPLAST				883
#define COALSMOKE1              916
#define COALSMOKE2              920
#define COALSMOKE3              924
#define COALSMOKE4              928
#define FOOTBALLGAME1           932
#define FOOTBALLGAME2           940
#define VBRDG0                  948
#define VBRDG1                  949
#define VBRDG2                  950
#define VBRDG3                  951

#define TILE_COUNT              960

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
#define chalkState              10
#define eraserState             11
#define stadiumState            12
#define parkState               13
#define seaportState            14
#define powerState              15
#define nuclearState            16
#define airportState            17
#define networkState            18

#define firstState              residentialState
#define lastState               networkState

#define STATE_CMD               0
#define STATE_TILES             1
#define STATE_OVERLAYS          2
#define STATE_GRAPHS            3

#define ISLAND_RADIUS           18

#define MAX_TRAFFIC_DISTANCE    30


////////////////////////////////////////////////////////////////////////
// Macros


#define ABS(x) \
        (((x) < 0) ? (-(x)) : (x))

#define TestBounds(x, y) \
        (((x) >= 0) && ((x) < WORLD_X) && ((y) >= 0) && ((y) < WORLD_Y))

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
        if (((tile &= LOMASK) >= 64) && \
            ((tile & LOMASK) <= 207)) { \
          tile = (tile & 0x000F) + 64; \
        }


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


class InkPoint {

public:

  int x;
  int y;

};


class Ink {

public:

  Ink *next;
  int x;
  int y;
  int color;
  int length;
  int maxlength;
  InkPoint *points;
  int left;
  int top;
  int right;
  int bottom;
  int last_x;
  int last_y;

};


class Micropolis {


 public:


  ////////////////////////////////////////////////////////////////////////


  Micropolis();

  ~Micropolis();

  void init();

  void destroy();


  ////////////////////////////////////////////////////////////////////////
  // allocate.cpp


  short SMapX;

  short SMapY;

  short CChr;

  short CChr9;

  short RoadTotal;

  short RailTotal;

  short FirePop;

  short ResPop;

  short ComPop;

  short IndPop;

  short TotalPop;

  short LastTotalPop;

  short ResZPop;

  short ComZPop;

  short IndZPop;

  short TotalZPop;

  short HospPop;

  short ChurchPop;

  short StadiumPop;

  short PolicePop;

  short FireStPop;

  short CoalPop;

  short NuclearPop;

  short PortPop;

  short APortPop;

  short NeedHosp;

  short NeedChurch;

  short CrimeAverage;

  short PolluteAverage;

  short LVAverage;

  Quad CityTime;

  short StartingYear;

  short *Map[WORLD_X];

  short ResHisMax;

  short Res2HisMax;

  short ComHisMax;

  short Com2HisMax;

  short IndHisMax;

  short Ind2HisMax;

  short CensusChanged;

  short MessagePort;

  short MesX;

  short MesY;

  Quad RoadSpend;

  short PoliceSpend;

  short FireSpend;

  Quad RoadFund;

  short PoliceFund;

  short FireFund;

  short RoadEffect;

  short PoliceEffect;

  short FireEffect;

  Quad TaxFund; 

  short CityTax;

  short TaxFlag;

  Byte *PopDensity[HWLDX];

  Byte *TrfDensity[HWLDX];

  Byte *PollutionMem[HWLDX];

  Byte *LandValueMem[HWLDX];

  Byte *CrimeMem[HWLDX];

  Byte *tem[HWLDX];

  Byte *tem2[HWLDX];

  Byte *TerrainMem[QWX];

  Byte *Qtem[QWX];

  short RateOGMem[SmX][SmY];

  short FireStMap[SmX][SmY];

  short PoliceMap[SmX][SmY];

  short PoliceMapEffect[SmX][SmY];

  short FireRate[SmX][SmY];

  short ComRate[SmX][SmY];

  short STem[SmX][SmY];

  Ptr terrainBase;

  Ptr qTemBase;

  Ptr tem1Base;

  Ptr tem2Base;

  Ptr popPtr;

  Ptr trfPtr;

  Ptr polPtr;

  Ptr landPtr;

  Ptr crimePtr;

  Ptr auxPopPtr;

  Ptr auxTrfPtr;

  Ptr auxPolPtr;

  Ptr auxLandPtr;

  Ptr auxCrimePtr;

  Ptr brettPtr;

  unsigned short *mapPtr;

  short *ResHis;

  short *ComHis;

  short *IndHis;

  short *MoneyHis;

  short *PollutionHis;

  short *CrimeHis;

  short *MiscHis;

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


  void animateTiles();


  ////////////////////////////////////////////////////////////////////////
  // budget.cpp


  float roadPercent;

  float policePercent;

  float firePercent;

  Quad roadValue;

  Quad policeValue;

  Quad fireValue;

  Quad roadMaxValue;

  Quad policeMaxValue;

  Quad fireMaxValue;

  int MustDrawCurrPercents;

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


  short ShakeNow;

  short FloodCnt;

  short FloodX;

  short FloodY;


  void DoDisasters();

  void ScenarioDisaster();

  void MakeMeltdown();

  void FireBomb();

  void MakeEarthquake();

  void SetFire();

  void MakeFire();

  int Vulnerable(int tem);

  void MakeFlood();

  void DoFlood();


  ////////////////////////////////////////////////////////////////////////
  // evaluate.cpp


  short EvalValid;

  short CityYes;

  short CityNo;

  short ProblemTable[PROBNUM];

  short ProblemTaken[PROBNUM];

  short ProblemVotes[PROBNUM]; /* these are the votes for each  */

  short ProblemOrder[4]; /* sorted index to above  */

  Quad CityPop;

  Quad deltaCityPop;

  Quad CityAssValue;

  short CityClass; /*  0..5  */

  short CityScore;

  short deltaCityScore;

  short AverageCityScore;

  short TrafficAverage;

  static char *cityClassStr[6];

  static char *cityLevelStr[3];

  static char *probStr[10];

  std::string evalChanged;

  std::string evalScore;

  std::string evalPs0;

  std::string evalPs1;

  std::string evalPs2;

  std::string evalPs3;

  std::string evalPv0;

  std::string evalPv1;

  std::string evalPv2;

  std::string evalPv3;

  std::string evalPop;

  std::string evalDelta;

  std::string evalAssessedDollars;

  std::string evalCityClass;

  std::string evalCityLevel;

  std::string evalGoodYes;

  std::string evalGoodNo;

  std::string evalTitle;

  void CityEvaluation();

  void EvalInit();

  void GetAssValue();

  void DoPopNum();

  void DoProblems();

  void VoteProblems();

  short AverageTrf();

  short GetUnemployment();

  short GetFire();

  void GetScore();

  void DoVotes();

  void doScoreCard();

  void ChangeEval();

  void scoreDoer();

  void SetEvaluation(
    const char *changed, 
    const char *score,
    const char *ps0, 
    const char *ps1, 
    const char *ps2, 
    const char *ps3,
    const char *pv0, 
    const char *pv1, 
    const char *pv2, 
    const char *pv3,
    const char *pop, 
    const char *delta, 
    const char *assessed_dollars, 
    const char *cityclass, 
    const char *citylevel, 
    const char *goodyes, 
    const char *goodno, 
    const char *title);


  ////////////////////////////////////////////////////////////////////////
  // fileio.cpp


  int load_file(
    char *filename, 
    char *dir);

  int loadFile(
    char *filename);

  int saveFile(
    char *filename);

  void LoadScenario(
    short s);

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


  short XStart;

  short YStart;

  short MapX;

  short MapY;

  short Dir;

  short LastDir;

  int TreeLevel; /* level for tree creation */

  int LakeLevel; /* level for lake creation */

  int CurveLevel; /* level for river curviness */

  int CreateIsland; /* -1 => 10%, 0 => never, 1 => always */


  void GenerateNewCity() ;

  void GenerateSomeCity(
    int r);

  short ERand(
    short limit);

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


  short NewGraph;

  short AllMax;

  unsigned char *History10[HISTORIES];

  unsigned char *History120[HISTORIES];

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


  void InitWillStuff();

  void ResetMapState();

  void ResetEditorState();


  ////////////////////////////////////////////////////////////////////////
  // main.cpp

  char *MicropolisVersion;

  int sim_loops;

  int sim_delay;

  int sim_skips;

  int sim_skip;

  int sim_paused;

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


  Quad LastCityPop;

  short LastCategory;

  short LastPicNum;

  short autoGo;

  short HaveLastMessage;

  char LastMessage[256];


  void SendMessages();

  void CheckGrowth();

  void DoScenarioScore(
    int type);

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


  int PowerStackNum;

  short PowerStackX[PWRSTKSIZE];

  short PowerStackY[PWRSTKSIZE];

  Quad MaxPower;

  Quad NumPower;


  int MoveMapSim(
    short MDir);

  short TestForCond(
    short TFDir);

  void DoPowerScan();

  void PushPowerStack();

  void PullPowerStack();


  ////////////////////////////////////////////////////////////////////////
  // random.cpp


  UQuad nextRandom;


  int sim_rand();

  void sim_srand(
    UQuad seed);


  ////////////////////////////////////////////////////////////////////////
  // resource.cpp


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

  short DisasterEvent;

  short DisasterWait;

  short ScoreType;

  short ScoreWait;

  short PwrdZCnt;

  short unPwrdZCnt;

  short NewPower; /* post */

  short AvCityTax;

  short Scycle;

  short Fcycle;

  short Spdcycle;

  short DoInitialEval;

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

  short Rand(
    short range);

  int Rand16();

  int Rand16Signed();

  void RandomlySeedRand();

  void SeedRand(
    int seed);


  ////////////////////////////////////////////////////////////////////////
  // sprite.cpp


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


  Quad TotalFunds;

  short PunishCnt;

  short autoBulldoze;

  short autoBudget;

  Quad LastMesTime;

  short GameLevel;

  short InitSimLoad;

  short ScenarioID;

  short SimSpeed;

  short SimMetaSpeed;

  short UserSoundOn;

  char *CityName;

  short NoDisasters;

  short MesNum;

  short EvalChanged;

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

  void ReallyQuit();

  void Callback(
	const char *name,
	const char *params,
	...);

  void DoEarthquake();

  void StopEarthquake();

  void InvalidateEditors();

  void InvalidateMaps();

  void *ckalloc(
    int size);

  void ckfree(
    void *data);

  void ResetLastKeys();

  void InitializeSound();

  void MakeSound(
    char *channel,
    char *sound);

  void StartMicropolisTimer();

  void StopMicropolisTimer();

  int getTile(
	int x, 
	int y);

  void *getMapBuffer();

  Ink *NewInk();

  void FreeInk(
    Ink *ink);

  void StartInk(
    Ink *ink, 
    int x, 
    int y);

  void AddInk(
    Ink *ink, 
    int x, 
    int y);

  void EraseOverlay();


  ////////////////////////////////////////////////////////////////////////
  // tool.cpp


  int OverRide;

  int Expensive;

  int Players;

  int Votes;

  int PendingTool;

  int PendingX;

  int PendingY;

  Ink *OldInk;

  Ink *overlay;

  Ink *track_ink;

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

  short tally(
    short tileValue);

  short checkSize(
    short temp);

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

  void put3x3Rubble(
    short x, 
    short y);

  void put4x4Rubble(
    short x, 
    short y);

  void put6x6Rubble(
    short x, 
    short y);

  void DidTool(
    char *name, 
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

  int ChalkTool(
    short x, 
    short y, 
    short color, 
    short first);

  void ChalkStart(
    int x, 
    int y, 
    int color);

  void ChalkTo(
    int x, 
    int y);

  int EraserTool(
    short x, 
    short y, 
    short first);

  int InkInBox(
    Ink *ink, 
    int left, 
    int top, 
    int right, 
    int bottom);

  void EraserStart(
    int x, 
    int y);

  void EraserTo(
    int x, 
    int y);

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


  ////////////////////////////////////////////////////////////////////////
  // traffic.cpp


  short PosStackN;

  short SMapXStack[MAX_TRAFFIC_DISTANCE+1];

  short SMapYStack[MAX_TRAFFIC_DISTANCE+1];

  short LDir;

  short Zsource;

  short TrafMaxX;

  short TrafMaxY;


  short MakeTraf(
    int Zt);

  void SetTrafMem();


  void PushPos();

  void PullPos();

  short FindPRoad();

  short FindPTele();

  short TryDrive();

  short TryGo(
    int z);

  short GetFromMap(
    int x);

  short DriveDone();

  short RoadTest(
    int x);


  ////////////////////////////////////////////////////////////////////////
  // update.cpp


  short MustUpdateFunds;

  short MustUpdateOptions;

  Quad LastCityTime;

  Quad LastCityYear;

  Quad LastCityMonth;

  Quad LastFunds;

  Quad LastR;

  Quad LastC;

  Quad LastI;

  std::string CityDate;

  static char *dateStr[12];

  
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


  ////////////////////////////////////////////////////////////////////////
  // utilities.cpp


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

  void DoSetMapState(
    short state);

  void DoNewGame();

  void DoGeneratedCityImage(
    char *name, 
    int time, 
    int pop, 
    char *cityClass, 
    int score);

  void DoPopUpMessage(
    char *msg);


  ////////////////////////////////////////////////////////////////////////
  // zone.cpp


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
