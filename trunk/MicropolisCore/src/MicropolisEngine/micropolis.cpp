/* micropolis.cpp
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

/** @file micropolis.cpp */

////////////////////////////////////////////////////////////////////////


#include "stdafx.h"


////////////////////////////////////////////////////////////////////////

/**
 * Give a fatal error and exit the program
 * @param line  Source line number of the fatal error
 * @param fname Filename of the source file
 * @return Does not return to the caller
 */
void not_reached(int line, const char *fname)
{
    fprintf(stderr, "Code unexpectedly executed at line %d, at file %s\n", line, fname);
    exit(1);
}


////////////////////////////////////////////////////////////////////////


Micropolis::Micropolis()
{
    init();
}


Micropolis::~Micropolis()
{
    destroy();
}


void Micropolis::init()
{


    ////////////////////////////////////////////////////////////////////////
    // allocate.cpp


    // short curMapX;
    curMapX = 0;

    // short curMapY;
    curMapY = 0;

    // short curNum;
    curNum = 0;

    // short curTile;
    curTile = 0;

    // short roadTotal;
    roadTotal = 0;

    // short railTotal;
    railTotal = 0;

    // short firePop;
    firePop = 0;

    // short resPop;
    resPop = 0;

    // short comPop;
    comPop = 0;

    // short indPop;
    indPop = 0;

    // short totalPop;
    totalPop = 0;

    // short totalPopLast;
    totalPopLast = 0;

    // short resZonePop;
    resZonePop = 0;

    // short comZonePop;
    comZonePop = 0;

    // short indZonePop;
    indZonePop = 0;

    // short totalZonePop;
    totalZonePop = 0;

    // short hospitalPop;
    hospitalPop = 0;

    // short churchPop;
    churchPop = 0;

    // short stadiumPop;
    stadiumPop = 0;

    // short policeStationPop;
    policeStationPop = 0;

    // short fireStationPop;
    fireStationPop = 0;

    // short coalPowerPop;
    coalPowerPop = 0;

    // short nuclearPowerPop;
    nuclearPowerPop = 0;

    // short seaportPop;
    seaportPop = 0;

    // short airportPop;
    airportPop = 0;

    // short needHospital;
    needHospital = 0;

    // short needChurch;
    needChurch = 0;

    // short crimeAverage;
    crimeAverage = 0;

    // short pollutionAverage;
    pollutionAverage = 0;

    // short landValueAverage;
    landValueAverage = 0;

    // Quad cityTime;
    cityTime = 0;

    // Quad cityMonth;
    cityMonth = 0;

    // Quad cityYear;
    cityYear = 0;

    // short startingYear;
    startingYear = 0;

    // short *map[WORLD_W];
    memset(map, 0, sizeof(short *) * WORLD_W);

    // short resHist10Max;
    resHist10Max = 0;

    // short resHist120Max;
    resHist120Max = 0;

    // short comHist10Max;
    comHist10Max = 0;

    // short comHist120Max;
    comHist120Max = 0;

    // short indHist10Max;
    indHist10Max = 0;

    // short indHist120Max;
    indHist120Max = 0;

    censusChanged = false;

    // short messagePort;
    messagePort = 0;

    messageX = 0;
    messageY = 0;

    // Quad roadSpend;
    roadSpend = 0;

    // Quad policeSpend;
    policeSpend = 0;

    // Quad fireSpend;
    fireSpend = 0;

    // Quad roadFund;
    roadFund = 0;

    // Quad policeFund;
    policeFund = 0;

    // Quad fireFund;
    fireFund = 0;

    roadEffect   = 0;
    policeEffect = 0;
    fireEffect   = 0;

    // Quad taxFund;
    taxFund = 0;

    // short cityTax;
    cityTax = 0;

    // bool taxFlag;
    taxFlag = false;

    // Byte *populationDensityMap[WORLD_W_2];
    memset(populationDensityMap, 0, sizeof(Byte *) * WORLD_W_2);

    // Byte *trafficDensityMap[WORLD_W_2];
    memset(trafficDensityMap, 0, sizeof(Byte *) * WORLD_W_2);

    // Byte *pollutionMap[WORLD_W_2];
    memset(pollutionMap, 0, sizeof(Byte *) * WORLD_W_2);

    // Byte *landValueMap[WORLD_W_2];
    memset(landValueMap, 0, sizeof(Byte *) * WORLD_W_2);

    // Byte *crimeMap[WORLD_W_2];
    memset(crimeMap, 0, sizeof(Byte *) * WORLD_W_2);

    // Byte *tempMap1[WORLD_W_2];
    memset(tempMap1, 0, sizeof(Byte *) * WORLD_W_2);

    // Byte *tempMap2[WORLD_W_2];
    memset(tempMap2, 0, sizeof(Byte *) * WORLD_W_2);

    // Byte *tempMap3[WORLD_W_4];
    memset(tempMap3, 0, sizeof(Byte *) * WORLD_W_4);

    // short tempMap4[WORLD_W_8][WORLD_H_8];
    memset(tempMap4, 0, sizeof(short) * WORLD_W_8 * WORLD_H_8);

    // Byte *terrainDensityMap[WORLD_W_4];
    memset(terrainDensityMap, 0, sizeof(Byte *) * WORLD_W_4);

    // short rateOfGrowthMap[WORLD_W_8][WORLD_H_8];
    memset(rateOfGrowthMap, 0, sizeof(short) * WORLD_W_8 * WORLD_H_8);

    // short fireStationMap[WORLD_W_8][WORLD_H_8];
    memset(fireStationMap, 0, sizeof(short) * WORLD_W_8 * WORLD_H_8);

    // short policeStationMap[WORLD_W_8][WORLD_H_8];
    memset(policeStationMap, 0, sizeof(short) * WORLD_W_8 * WORLD_H_8);

    // short policeStationMapEffect[WORLD_W_8][WORLD_H_8];
    memset(policeStationMapEffect, 0, sizeof(short) * WORLD_W_8 * WORLD_H_8);

    // short fireStationMapEffect[WORLD_W_8][WORLD_H_8];
    memset(fireStationMapEffect, 0, sizeof(short) * WORLD_W_8 * WORLD_H_8);

    // short comRateMap[WORLD_W_8][WORLD_H_8];
    memset(comRateMap, 0, sizeof(short) * WORLD_W_8 * WORLD_H_8);

    // Ptr terrainDensityMapBase;
    terrainDensityMapBase = NULL;

    // Ptr tempMap1Base;
    tempMap1Base = NULL;

    // Ptr tempMap2Base;
    tempMap2Base = NULL;

    // Ptr tempMap3Base;
    tempMap3Base = NULL;

    // Ptr populationDensityMapBase;
    populationDensityMapBase = NULL;

    // Ptr trafficDensityMapBase;
    trafficDensityMapBase = NULL;

    // Ptr pollitionMapBase;
    pollutionMapBase = NULL;

    // Ptr landValueMapBase;
    landValueMapBase = NULL;

    // Ptr crimeMapBase;
    crimeMapBase = NULL;

    // unsigned short *mapBase;
    mapBase = NULL;

    // short *resHist;
    resHist = NULL;

    // short *comHist;
    comHist = NULL;

    // short *indHist;
    indHist = NULL;

    // short *moneyHist;
    moneyHist = NULL;

    // short *pollutionHist;
    pollutionHist = NULL;

    // short *crimeHist;
    crimeHist = NULL;

    // short *miscHist;
    miscHist = NULL;

    // short *powerMap;
    powerMap = NULL;


    ////////////////////////////////////////////////////////////////////////
    // animate.cpp


    ////////////////////////////////////////////////////////////////////////
    // budget.cpp


    // float roadPercent;
    roadPercent = (float)0.0;

    // float policePercent;
    policePercent = (float)0.0;

    // float firePercent;
    firePercent = (float)0.0;

    // Quad roadValue;
    roadValue = 0;

    // Quad policeValue;
    policeValue = 0;

    // Quad fireValue;
    fireValue = 0;

    // int mustDrawCurrPercents;
    mustDrawCurrPercents = 0;

    // int mustDrawBudgetWindow;
    mustDrawBudgetWindow = 0;


    ////////////////////////////////////////////////////////////////////////
    // connect.cpp


    ////////////////////////////////////////////////////////////////////////
    // disasters.cpp


    // short floodCount;
    floodCount = 0;


    ////////////////////////////////////////////////////////////////////////
    // evaluate.cpp


    // short cityYes;
    cityYes = 0;

    // short problemVotes[PROBNUM]; /* these are the votes for each  */
    memset(problemVotes, 0, sizeof(short) * PROBNUM);

    // short problemOrder[CVP_PROBLEM_COMPLAINTS]; /* sorted index to above  */
    memset(problemOrder, 0, sizeof(short) * CVP_PROBLEM_COMPLAINTS);

    // Quad cityPop;
    cityPop = 0;

    // Quad deltaCityPop;
    deltaCityPop = 0;

    // Quad cityAssessedValue;
    cityAssessedValue = 0;

    // CityClass cityClass;
    cityClass = CC_VILLAGE;

    // short cityScore;
    cityScore = 0;

    // short cityScoreDelta;
    cityScoreDelta = 0;

    // short trafficAverage;
    trafficAverage = 0;


    ////////////////////////////////////////////////////////////////////////
    // fileio.cpp


    ////////////////////////////////////////////////////////////////////////
    // generate.cpp


    // short terrainXStart;
    terrainXStart = 0;

    // short terrainYStart;
    terrainYStart = 0;

    // short terrainMapX;
    terrainMapX = 0;

    // short terrainMapY;
    terrainMapY = 0;

    // short Dir;
    terrainDir = 0;

    // short LastDir;
    terrainDirLast = 0;

    // int TreeLevel; /* level for tree creation */
    terrainTreeLevel = -1;

    // int LakeLevel; /* level for lake creation */
    terrainLakeLevel = -1;

    // int CurveLevel; /* level for river curviness */
    terrainCurveLevel = -1;

    // int CreateIsland; /* -1 => 10%, 0 => never, 1 => always */
    terrainCreateIsland = -1;

    // int IslandRadius;
    terrainIslandRadius = ISLAND_RADIUS;


    ////////////////////////////////////////////////////////////////////////
    // graph.cpp


    newGraph = false;

    graph10Max = 0;
    graph120Max = 0;

    // int GraphUpdateTime;
    graphUpdateTime = 100;


    ////////////////////////////////////////////////////////////////////////
    // initialize.cpp


    ////////////////////////////////////////////////////////////////////////
    // main.cpp

    // int simLoops;
    simLoops = 0;

    // int simSkips;
    simSkips = 0;

    // int simSkip;
    simSkip = 0;

    simPaused = false; // Simulation is running

    // int simPausedSpeed;
    simPausedSpeed = 3;

    // int heatSteps;
    heatSteps = 0;

    // int heatFlow;
    heatFlow = -7;

    // int heatRule;
    heatRule = 0;

    // int heatWrap;
    heatWrap = 3;

    // std::string cityFileName;
    cityFileName = "";

    // std::string cityName;
    cityName = "";

    // int tilesAnimated;
    tilesAnimated = 0;

    // int doAnimation;
    doAnimation = 1;

    // int doMessages;
    doMessages = 1;

    // int doNotices;
    doNotices = 1;

    // short *cellSrc;
    cellSrc = NULL;

    // short *cellDst;
    cellDst = NULL;


    ////////////////////////////////////////////////////////////////////////
    // map.cpp


#if 0

    ////////////////////////////////////////////////////////////////////////
    // Disabled this small map drawing, filtering and overlaying code.
    // Going to re-implement it in the tile engine and Python.


    // int dynamicData[32];
    memset(dynamicData, 0, sizeof(int) * 32);

#endif


    ////////////////////////////////////////////////////////////////////////
    // message.cpp


    // Quad lastCityPop;
    lastCityPop = 0;

    // short lastCategory;
    lastCategory = 0;

    // short messagePictureLast;
    messagePictureLast = 0;

    autoGoto = false;

    messageLastValid = false;

    // char messageLast[256];
    memset(messageLast, 0, sizeof(char) * 256);


    ////////////////////////////////////////////////////////////////////////
    // power.cpp


    powerStackPointer = 0;

    // short powerStackX[POWER_STACK_SIZE];
    // short powerStackY[POWER_STACK_SIZE];
    memset(powerStackX, 0, sizeof(short) * POWER_STACK_SIZE);
    memset(powerStackY, 0, sizeof(short) * POWER_STACK_SIZE);

    // Quad maxPower;
    maxPower = 0;

    // Quad numPower;
    numPower = 0;


    ////////////////////////////////////////////////////////////////////////
    // random.cpp


    // UQuad nextRandom;
    nextRandom = 1;


    ////////////////////////////////////////////////////////////////////////
    // resource.cpp


    // char *HomeDir;
    homeDir = "";

    // char *ResourceDir;
    resourceDir = "";

    // Resource *resources;
    resources = NULL;

    // StringTable *stringTables;
    stringTables = NULL;


    ////////////////////////////////////////////////////////////////////////
    // scan.cpp

    // short newMap;
    newMap = 0;

    // short newMapFlags[MAP_TYPE_COUNT];
    memset(newMapFlags, 0, sizeof(short) * MAP_TYPE_COUNT);

    // short cityCenterX;
    cityCenterX = 0;

    // short cityCenterY;
    cityCenterY = 0;

    // short cityCenterX2;
    cityCenterX2 = 0;

    // short cityCenterY2;
    cityCenterY2 = 0;

    // short pollutionMaxX;
    pollutionMaxX = 0;

    // short pollutionMaxY;
    pollutionMaxY = 0;

    // short crimeMaxX;
    crimeMaxX = 0;

    // short crimeMaxY;
    crimeMaxY = 0;

    // Quad donDither;
    donDither = 0;


    ////////////////////////////////////////////////////////////////////////
    // simulate.cpp


    // short valveFlag;
    valveFlag = 0;

    // short crimeRamp;
    crimeRamp = 0;

    // short pollutionRamp;
    pollutionRamp = 0;

    // short resValve;
    resValve = 0;

    // short comValve;
    comValve = 0;

    // short indValve;
    indValve = 0;

    // short resCap;
    resCap = 0;

    // short comCap;
    comCap = 0;

    // short indCap;
    indCap = 0;

    // short cashFlow;
    cashFlow = 0;

    // float externalMarket;
    externalMarket = (float)4.0;

    disasterEvent = SC_NONE;

    // short disasterWait;
    disasterWait = 0;

    scoreType = SC_NONE;

    // short scoreWait;
    scoreWait = 0;

    // short poweredZoneCount;
    poweredZoneCount = 0;

    // short unpoweredZoneCount;
    unpoweredZoneCount = 0;

    // short newPower; /* post */
    newPower = 0;

    // short cityTaxAverage;
    cityTaxAverage = 0;

    // short simCycle;
    simCycle = 0;

    // short phaseCycle;
    phaseCycle = 0;

    // short speedCycle;
    speedCycle = 0;

    doInitialEval = false;


    ////////////////////////////////////////////////////////////////////////
    // sprite.cpp


    //SimSprite *spriteList;
    spriteList = NULL;

    // SimSprite *freeSprites;
    freeSprites = NULL;

    // SimSprite *globalSprites[SPRITE_COUNT];
    memset(globalSprites, 0, sizeof(SimSprite *) * SPRITE_COUNT);

    // short crashX;
    crashX = 0;

    // short crashY;
    crashY = 0;

    // int absDist;
    absDist = 0;

    // short spriteCycle;
    spriteCycle = 0;


    ////////////////////////////////////////////////////////////////////////
    // stubs.cpp


    // Quad totalFunds;
    totalFunds = 0;

    autoBulldoze = false; // Disable auto-bulldoze
    autoBudget = false;   // Disable auto-budget

    // Quad messageTimeLast;
    messageTimeLast = 0;

    gameLevel = LEVEL_EASY;

    // short initSimLoad;
    initSimLoad = 0;

    scenario = SC_NONE;

    // short simSpeed;
    simSpeed = 0;

    // short simSpeedMeta;
    simSpeedMeta = 0;

    enableSound = false; // Disable sound

    enableDisasters = true; // Enable disasters

    // short messageNumber;
    messageNumber = 0;

    evalChanged = false;

    // short blinkFlag;
    blinkFlag = 0;

    // CallbackFunction callbackHook;
    callbackHook = NULL;

    // void *callbackData;
    callbackData = NULL;

    // void *userData;
    userData = NULL;


    ////////////////////////////////////////////////////////////////////////
    //  tool.cpp


    // int toolX;
    toolX = 0;

    // int toolY;
    toolY = 0;

    // int toolXLast;
    toolXLast = 0;

    // int toolYLast;
    toolYLast = 0;


    ////////////////////////////////////////////////////////////////////////
    // traffic.cpp


    // short curMapStackPointer;
    curMapStackPointer = 0;

    // short curMapStackX[MAX_TRAFFIC_DISTANCE+1];
    memset(curMapStackX, 0, sizeof(short) * (MAX_TRAFFIC_DISTANCE + 1));

    // short curMapStackY[MAX_TRAFFIC_DISTANCE+1];
    memset(curMapStackY, 0, sizeof(short) * (MAX_TRAFFIC_DISTANCE + 1));

    dirLast = DIR_INVALID;

    zoneSource = ZT_COMMERCIAL;

    // short trafMaxX;
    trafMaxX = 0;

    // short trafMaxY;
    trafMaxY = 0;


    ////////////////////////////////////////////////////////////////////////
    // update.cpp


    // short mustUpdateFunds;
    mustUpdateFunds = 0;

    // short mustUpdateOptions;
    mustUpdateOptions = 0;

    // Quad cityTimeLast;
    cityTimeLast = 0;

    // Quad cityYearLast;
    cityYearLast = 0;

    // Quad cityMonthLast;
    cityMonthLast = 0;

    // Quad totalFundsLast;
    totalFundsLast = 0;

    // Quad resLast;
    resLast = 0;

    // Quad comLast;
    comLast = 0;

    // Quad indLast;
    indLast = 0;


    ////////////////////////////////////////////////////////////////////////
    // utilities.cpp


    ////////////////////////////////////////////////////////////////////////
    // zone.cpp


    ////////////////////////////////////////////////////////////////////////

    simInit();

}


void Micropolis::destroy()
{

    destroyMapArrays();

    // TODO: Clean up all other stuff:

}


////////////////////////////////////////////////////////////////////////
