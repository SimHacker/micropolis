/* simulate.cpp
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

/** @file simulate.cpp */

////////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "micropolis.h"
#include "text.h"


////////////////////////////////////////////////////////////////////////
// Constants

/**
 * How often to perform the short term census.
 * @todo Rename to CENSUS_MONTHLY_FREQUENCY or so?
 */
static const int CENSUS_FREQUENCY_10 = 4;

/**
 * How often to perform the long term census.
 * @todo Rename to CENSUS_YEARLY_FREQUENCY or so?
 */
static const int CENSUS_FREQUENCY_120 = CENSUS_FREQUENCY_10 * 12;

/**
 * How often to collect taxes.
 */
static const int TAX_FREQUENCY = 48;


////////////////////////////////////////////////////////////////////////


/* comefrom: doEditWindow scoreDoer doMapInFront graphDoer doNilEvent */
void Micropolis::simFrame()
{

    if (simSpeed == 0) {
        return;
    }

    if (++speedCycle > 1023) {
        speedCycle = 0;
    }

    if (simSpeed == 1 && (speedCycle % 5) != 0) {
        return;
    }

    if (simSpeed == 2 && (speedCycle % 3) != 0) {
        return;
    }

    if (++phaseCycle > 1023) {
        phaseCycle = 0;
    }

    // Why is this disabled? Look useful for initializing after loading.
/*
    if (initSimLoad) {
        phaseCycle = 0;
    }
*/

    simulate(phaseCycle & 15);
}


/* comefrom: simFrame */
void Micropolis::simulate(int phase)
{
    static const short SpdPwr[4] = { 1,  2,  4,  5 };
    static const short SpdPtl[4] = { 1,  2,  7, 17 };
    static const short SpdCri[4] = { 1,  1,  8, 18 };
    static const short SpdPop[4] = { 1,  1,  9, 19 };
    static const short SpdFir[4] = { 1,  1, 10, 20 };

    short x = clamp(simSpeed, (short)1, (short)3);

    switch (phase)  {

        case 0:

            if (++simCycle > 1023) {
                simCycle = 0; // This is cosmic!
            }

            if (doInitialEval) {
                doInitialEval = false;
                cityEvaluation();
            }

            cityTime++;
            cityTaxAverage += cityTax;

            if (!(simCycle & 1)) {
                setValves();
            }

            clearCensus();

            break;

        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:

            // Scan 1/8 of the map for each of the 8 phases 1..8:
            mapScan((phase - 1) * WORLD_W / 8, phase * WORLD_W / 8);

            break;

        case 9:

            if ((cityTime % CENSUS_FREQUENCY_10) == 0) {
                take10Census();
            }

            if ((cityTime % (CENSUS_FREQUENCY_120)) == 0) {
                take120Census();
            }

            if ((cityTime % TAX_FREQUENCY) == 0)  {
                collectTax();
                cityEvaluation();
            }

            break;

        case 10:

            if (!(simCycle % 5)) {
                decRateOfGrowthMap();
            }

            decTrafficMap();

            newMapFlags[MAP_TYPE_TRAFFIC_DENSITY] = 1;
            newMapFlags[MAP_TYPE_ROAD] = 1;
            newMapFlags[MAP_TYPE_ALL] = 1;
            newMapFlags[MAP_TYPE_RES] = 1;
            newMapFlags[MAP_TYPE_COM] = 1;
            newMapFlags[MAP_TYPE_IND] = 1;
            newMapFlags[MAP_TYPE_DYNAMIC] = 1;

            sendMessages();

            break;

        case 11:

            if ((simCycle % SpdPwr[x]) == 0) {
                doPowerScan();
                newMapFlags[MAP_TYPE_POWER] = 1;
                newPower = true; /* post-release change */
            }

            break;

        case 12:

            if ((simCycle % SpdPtl[x]) == 0) {
                pollutionTerrainLandValueScan();
            }

            break;

        case 13:

            if ((simCycle % SpdCri[x]) == 0) {
                crimeScan();
            }

            break;

        case 14:

            if ((simCycle % SpdPop[x]) == 0) {
                populationDensityScan();
            }

            break;

        case 15:

            if ((simCycle % SpdFir[x]) == 0) {
                fireAnalysis();
            }

            doDisasters();

            break;

    }
}


/**
 * Initialize simulation.
 * @todo Create constants for initSimLoad.
 */
void Micropolis::doSimInit()
{
    phaseCycle = 0;
    simCycle = 0;

    if (initSimLoad == 2) {
        /* if new city */
        initSimMemory();
    }

    if (initSimLoad == 1) {
        /* if city just loaded */
        simLoadInit();
    }

    setValves();
    clearCensus();
    mapScan(0, WORLD_W);
    doPowerScan();
    newPower = true;         /* post rel */
    pollutionTerrainLandValueScan();
    crimeScan();
    populationDensityScan();
    fireAnalysis();
    newMap = 1;
    newGraph = true;
    totalPop = 1;
    doInitialEval = true;
}


/**
 * Copy bits from powerGridMap to the #PWRBIT in the map for all zones in the
 * world.
 */
void Micropolis::doNilPower()
{
    short x, y, z;

    for (x = 0; x < WORLD_W; x++) {
        for (y = 0; y < WORLD_H; y++) {
            z = map[x][y];
            if (z & ZONEBIT) {
                curMapX = x;
                curMapY = y;
                curNum = z;
                setZonePower(Position(x, y));
            }
        }
    }
}


/** Decrease traffic memory */
void Micropolis::decTrafficMap()
{
    /* tends to empty trafficDensityMap */
    short x, y, z;

    for (x = 0; x < WORLD_W; x += trafficDensityMap.MAP_BLOCKSIZE) {
        for (y = 0; y < WORLD_H; y += trafficDensityMap.MAP_BLOCKSIZE) {
            z = trafficDensityMap.worldGet(x, y);
            if (z == 0) {
                continue;
            }

            if (z <= 24) {
                trafficDensityMap.worldSet(x, y, 0);
                continue;
            }

            if (z > 200) {
                trafficDensityMap.worldSet(x, y, z - 34);
            } else {
                trafficDensityMap.worldSet(x, y, z - 24);
            }
        }
    }
}


/**
 * Decrease rate of grow.
 * @todo Limiting rate should not be done here, but when we add a new value to
 *       it.
 */
void Micropolis::decRateOfGrowthMap()
{
    /* tends to empty rateOfGrowthMap */
    short x, y, z;

    for (x = 0; x < rateOfGrowthMap.MAP_W; x++) {
        for (y = 0; y < rateOfGrowthMap.MAP_H; y++) {
            z = rateOfGrowthMap.get(x, y);
            if (z == 0) {
                continue;
            }

            if (z > 0) {
                z--;
                z = clamp(z, (short)-200, (short)200);
                rateOfGrowthMap.set(x, y, z);
                continue;
            }

            if (z < 0)  {
                z++;
                z = clamp(z, (short)-200, (short)200);
                rateOfGrowthMap.set(x, y, z);
            }
        }
    }
}


/* comefrom: doSimInit */
void Micropolis::initSimMemory()
{
    setCommonInits();

    for (short x = 0; x < 240; x++)  {
        resHist[x] = 0;
        comHist[x] = 0;
        indHist[x] = 0;
        moneyHist[x] = 128;
        crimeHist[x] = 0;
        pollutionHist[x] = 0;
    }

    crimeRamp = 0;
    pollutionRamp = 0;
    totalPop = 0;
    resValve = 0;
    comValve = 0;
    indValve = 0;
    resCap = false; // Do not block residential growth
    comCap = false; // Do not block commercial growth
    indCap = false; // Do not block industrial growth

    externalMarket = 6.0;
    disasterEvent = SC_NONE;
    scoreType = SC_NONE;

    /* This clears powermem */
    powerStackPointer = 0;
    doPowerScan();
    newPower = true; /* post rel */

    initSimLoad = 0;
}


/* comefrom: doSimInit */
void Micropolis::simLoadInit()
{
    // Disaster delay table for each scenario
    static const short disasterWaitTable[SC_COUNT] = {
        0,          // No scenario (free playing)
        2,          // Dullsville (boredom)
        10,         // San francisco (earth quake)
        4 * 10,     // Hamburg (fire bombs)
        20,         // Bern (traffic)
        3,          // Tokyo (scary monster)
        5,          // Detroit (crime)
        5,          // Boston (nuclear meltdown)
        2 * 48,     // Rio (flooding)
    };

    // Time to wait before score calculation for each scenario
    static const short scoreWaitTable[SC_COUNT] = {
        0,          // No scenario (free playing)
        30 * 48,    // Dullsville (boredom)
        5 * 48,     // San francisco (earth quake)
        5 * 48,     // Hamburg (fire bombs)
        10 * 48,    // Bern (traffic)
        5 * 48,     // Tokyo (scary monster)
        10 * 48,    // Detroit (crime)
        5 * 48,     // Boston (nuclear meltdown)
        10 * 48,    // Rio (flooding)
    };

    externalMarket = (float)miscHist[1];
    resPop = miscHist[2];
    comPop = miscHist[3];
    indPop = miscHist[4];
    resValve = miscHist[5];
    comValve = miscHist[6];
    indValve = miscHist[7];
    crimeRamp = miscHist[10];
    pollutionRamp = miscHist[11];
    landValueAverage = miscHist[12];
    crimeAverage = miscHist[13];
    pollutionAverage = miscHist[14];
    gameLevel = (GameLevel)miscHist[15];

    if (cityTime < 0) {
        cityTime = 0;
    }

    if (!externalMarket) {
        externalMarket = 4.0;
    }

    // Set game level
    if (gameLevel > LEVEL_LAST || gameLevel < LEVEL_FIRST) {
        gameLevel = LEVEL_FIRST;
    }
    setGameLevel(gameLevel);

    setCommonInits();

    // Load cityClass
    cityClass = (CityClass)(miscHist[16]);
    if (cityClass > CC_MEGALOPOLIS || cityClass < CC_VILLAGE) {
        cityClass = CC_VILLAGE;
    }

    cityScore = miscHist[17];
    if (cityScore > 999 || cityScore < 1) {
        cityScore = 500;
    }

    resCap = false;
    comCap = false;
    indCap = false;

    cityTaxAverage = (cityTime % 48) * 7;  /* post */

    // Set power map.
    /// @todo What purpose does this serve? Weird...
    powerGridMap.fill(1);

    doNilPower();

    if (scenario >= SC_COUNT) {
        scenario = SC_NONE;
    }

    if (scenario != SC_NONE) {
        assert(LENGTH_OF(disasterWaitTable) == SC_COUNT);
        assert(LENGTH_OF(scoreWaitTable) == SC_COUNT);

        disasterEvent = scenario;
        disasterWait = disasterWaitTable[disasterEvent];
        scoreType = disasterEvent;
        scoreWait = scoreWaitTable[disasterEvent];
    } else {
        disasterEvent = SC_NONE;
        disasterWait = 0;
        scoreType = SC_NONE;
        scoreWait = 0;
    }

    roadEffect = MAX_ROAD_EFFECT;
    policeEffect = MAX_POLICE_STATION_EFFECT;
    fireEffect = MAX_FIRE_STATION_EFFECT;
    initSimLoad = 0;
}


/* comefrom: initSimMemory simLoadInit */
void Micropolis::setCommonInits()
{
    evalInit();
    roadEffect = MAX_ROAD_EFFECT;
    policeEffect = MAX_POLICE_STATION_EFFECT;
    fireEffect = MAX_FIRE_STATION_EFFECT;
    taxFlag = false;
    taxFund = 0;
}


/* comefrom: simulate doSimInit */
void Micropolis::setValves()
{
    /// @todo Break the tax table out into configurable parameters.
    static const short taxTable[21] = {
        200, 150, 120, 100, 80, 50, 30, 0, -10, -40, -100,
        -150, -200, -250, -300, -350, -400, -450, -500, -550, -600,
    };
    static const float extMarketParamTable[3] = {
        1.2f, 1.1f, 0.98f,
    };
    assert(LEVEL_COUNT == LENGTH_OF(extMarketParamTable));

    /// @todo Make configurable parameters.
    short resPopDenom = 8;
    float birthRate = 0.02;
    float laborBaseMax = 1.3;
    float internalMarketDenom = 3.7;
    float projectedIndPopMin = 5.0;
    float resRatioDefault = 1.3;
    float resRatioMax = 2;
    float comRatioMax = 2;
    float indRatioMax = 2;
    short taxMax = 20;
    float taxTableScale = 600;

    /// @todo Break the interesting values out into public member
    ///       variables so the user interface can display them.
    float employment, migration, births, laborBase, internalMarket;
    float resRatio, comRatio, indRatio;
    float normalizedResPop, projectedResPop, projectedComPop, projectedIndPop;

    miscHist[1] = (short)externalMarket;
    miscHist[2] = resPop;
    miscHist[3] = comPop;
    miscHist[4] = indPop;
    miscHist[5] = resValve;
    miscHist[6] = comValve;
    miscHist[7] = indValve;
    miscHist[10] = crimeRamp;
    miscHist[11] = pollutionRamp;
    miscHist[12] = landValueAverage;
    miscHist[13] = crimeAverage;
    miscHist[14] = pollutionAverage;
    miscHist[15] = gameLevel;
    miscHist[16] = (short)cityClass;
    miscHist[17] = cityScore;

    normalizedResPop = (float)resPop / (float)resPopDenom;
    totalPopLast = totalPop;
    totalPop = (short)(normalizedResPop + comPop + indPop);

    if (resPop > 0) {
        employment = (comHist[1] + indHist[1]) / normalizedResPop;
    } else {
        employment = 1;
    }

    migration = normalizedResPop * (employment - 1);
    births = normalizedResPop * birthRate;
    projectedResPop = normalizedResPop + migration + births;   // Projected res pop.

    // Compute laborBase
    float temp = comHist[1] + indHist[1];
    if (temp > 0.0) {
        laborBase = (resHist[1] / temp);
    } else {
        laborBase = 1;
    }
    laborBase = clamp(laborBase, 0.0f, laborBaseMax);

    internalMarket = (float)(normalizedResPop + comPop + indPop) / internalMarketDenom;

    projectedComPop = internalMarket * laborBase;

    assert(gameLevel >= LEVEL_FIRST && gameLevel <= LEVEL_LAST);
    projectedIndPop = indPop * laborBase * extMarketParamTable[gameLevel];
    projectedIndPop = max(projectedIndPop, projectedIndPopMin);

    if (normalizedResPop > 0) {
        resRatio = (float)projectedResPop / (float)normalizedResPop; // projected -vs- actual.
    } else {
        resRatio = resRatioDefault;
    }

    if (comPop > 0) {
        comRatio = (float)projectedComPop / (float)comPop;
    } else {
        comRatio = (float)projectedComPop;
    }

    if (indPop > 0) {
        indRatio = (float)projectedIndPop / (float)indPop;
    } else {
        indRatio = (float)projectedIndPop;
    }

    resRatio = min(resRatio, resRatioMax);
    comRatio = min(comRatio, comRatioMax);
    resRatio = min(indRatio, indRatioMax);

    // Global tax and game level effects.
    short z = min((short)(cityTax + gameLevel), taxMax);
    resRatio = (resRatio - 1) * taxTableScale + taxTable[z];
    comRatio = (comRatio - 1) * taxTableScale + taxTable[z];
    indRatio = (indRatio - 1) * taxTableScale + taxTable[z];

    // Ratios are velocity changes to valves.
    resValve = clamp(resValve + (short)resRatio, -RES_VALVE_RANGE, RES_VALVE_RANGE);
    comValve = clamp(comValve + (short)comRatio, -COM_VALVE_RANGE, COM_VALVE_RANGE);
    indValve = clamp(indValve + (short)indRatio, -IND_VALVE_RANGE, IND_VALVE_RANGE);

    if (resCap && resValve > 0) {
        resValve = 0; // Need a stadium, so cap resValve.
    }

    if (comCap && comValve > 0) {
        comValve = 0; // Need a airport, so cap comValve.
    }

    if (indCap && indValve > 0) {
        indValve = 0; // Need an seaport, so cap indValve.
    }

    valveFlag = true;
}


/* comefrom: simulate doSimInit */
void Micropolis::clearCensus()
{
    poweredZoneCount = 0;
    unpoweredZoneCount = 0;
    firePop = 0;
    roadTotal = 0;
    railTotal = 0;
    resPop = 0;
    comPop = 0;
    indPop = 0;
    resZonePop = 0;
    comZonePop = 0;
    indZonePop = 0;
    hospitalPop = 0;
    churchPop = 0;
    policeStationPop = 0;
    fireStationPop = 0;
    stadiumPop = 0;
    coalPowerPop = 0;
    nuclearPowerPop = 0;
    seaportPop = 0;
    airportPop = 0;
    powerStackPointer = 0; /* Reset before Mapscan */

    fireStationMap.clear();
    //fireStationEffectMap.clear(); // Added in rev293
    policeStationMap.clear();
    //policeStationEffectMap.clear(); // Added in rev293

}


/**
 * Take monthly snaphsot of all relevant data for the historic graphs.
 * Also update variables that control building new churches and hospitals.
 * @todo Rename to takeMonthlyCensus (or takeMonthlySnaphshot?).
 * @todo A lot of this max stuff is also done in graph.cpp
 */
void Micropolis::take10Census()
{
    // TODO: Make configurable parameters.
    int resPopDenom = 8;

    short x;

    /* put census#s in Historical Graphs and scroll data  */
    resHist10Max = 0;
    comHist10Max = 0;
    indHist10Max = 0;

    for (x = 118; x >= 0; x--)    {

        resHist10Max = max(resHist10Max, resHist[x]);
        comHist10Max = max(comHist10Max, comHist[x]);
        indHist10Max = max(indHist10Max, indHist[x]);

        resHist[x + 1] = resHist[x];
        comHist[x + 1] = comHist[x];
        indHist[x + 1] = indHist[x];
        crimeHist[x + 1] = crimeHist[x];
        pollutionHist[x + 1] = pollutionHist[x];
        moneyHist[x + 1] = moneyHist[x];

    }

    graph10Max = resHist10Max;
    graph10Max = max(graph10Max, comHist10Max);
    graph10Max = max(graph10Max, indHist10Max);

    resHist[0] = resPop / resPopDenom;
    comHist[0] = comPop;
    indHist[0] = indPop;

    crimeRamp += (crimeAverage - crimeRamp) / 4;
    crimeHist[0] = min(crimeRamp, (short)255);

    pollutionRamp += (pollutionAverage - pollutionRamp) / 4;
    pollutionHist[0] = min(pollutionRamp, (short)255);

    x = (cashFlow / 20) + 128;    /* scale to 0..255  */
    moneyHist[0] = clamp(x, (short)0, (short)255);

    changeCensus();

    short resPopScaled = resPop >> 8;

    if (hospitalPop < resPopScaled) {
        needHospital = 1;
    }

    if (hospitalPop > resPopScaled) {
        needHospital = -1;
    }

    if (hospitalPop == resPopScaled) {
        needHospital = 0;
    }

    if (churchPop < resPopScaled) {
        needChurch = 1;
    }

    if (churchPop > resPopScaled) {
        needChurch = -1;
    }

    if (churchPop == resPopScaled) {
        needChurch = 0;
    }
}


/* comefrom: simulate */
void Micropolis::take120Census()
{
    // TODO: Make configurable parameters.
    int resPopDenom = 8;

    /* Long Term Graphs */
    short x;

    resHist120Max = 0;
    comHist120Max = 0;
    indHist120Max = 0;

    for (x = 238; x >= 120; x--)  {

        resHist120Max = max(resHist120Max, resHist[x]);
        comHist120Max = max(comHist120Max, comHist[x]);
        indHist120Max = max(indHist120Max, indHist[x]);

        resHist[x + 1] = resHist[x];
        comHist[x + 1] = comHist[x];
        indHist[x + 1] = indHist[x];
        crimeHist[x + 1] = crimeHist[x];
        pollutionHist[x + 1] = pollutionHist[x];
        moneyHist[x + 1] = moneyHist[x];

    }

    graph120Max = resHist120Max;
    graph120Max = max(graph120Max, comHist120Max);
    graph120Max = max(graph120Max, indHist120Max);

    resHist[120] = resPop / resPopDenom;
    comHist[120] = comPop;
    indHist[120] = indPop;
    crimeHist[120] = crimeHist[0] ;
    pollutionHist[120] = pollutionHist[0];
    moneyHist[120] = moneyHist[0];
    changeCensus();
}


/** Collect taxes
 * @bug Function seems to be doing different things depending on
 *      Micropolis::totalPop value. With an non-empty city it does fund
 *      calculations. For an empty city, it immediately sets effects of
 *      funding, which seems inconsistent at least, and may be wrong
 * @bug If Micropolis::taxFlag is set, no variable is touched which seems
 *      non-robust at least
 */
void Micropolis::collectTax()
{
    short z;

    /**
     * @todo Break out so the user interface can configure this.
     */
    static const float RLevels[3] = { 0.7, 0.9, 1.2 };
    static const float FLevels[3] = { 1.4, 1.2, 0.8 };

    assert(LEVEL_COUNT == LENGTH_OF(RLevels));
    assert(LEVEL_COUNT == LENGTH_OF(FLevels));

    cashFlow = 0;

    /**
     * @todo Apparently taxFlag is never set to true in MicropolisEngine
     *       or the TCL code, so this always runs.
     * @todo Check old Mac code to see if it's ever set, and why.
     */

    if (!taxFlag) { // If the Tax Port is clear

        /// @todo Do something with z? Check old Mac code to see if it's used.
        z = cityTaxAverage / 48;  // post release

        cityTaxAverage = 0;

        policeFund = (long)policeStationPop * 100;
        fireFund = (long)fireStationPop * 100;
        roadFund = (long)((roadTotal + (railTotal * 2)) * RLevels[gameLevel]);
        taxFund = (long)((((Quad)totalPop * landValueAverage) / 120) * cityTax * FLevels[gameLevel]);

        if (totalPop > 0) {
            /* There are people to tax. */
            cashFlow = (short)(taxFund - (policeFund + fireFund + roadFund));
            doBudget();
        } else {
            /* Nobody lives here. */
            roadEffect   = MAX_ROAD_EFFECT;
            policeEffect = MAX_POLICE_STATION_EFFECT;
            fireEffect   = MAX_FIRE_STATION_EFFECT;
        }
    }
}


/**
 * Update effects of (possibly reduced) funding
 *
 * It updates effects with respect to roads, police, and fire.
 * @note This function should probably not be used when #totalPop is
 *       clear (ie with an empty) city. See also bugs of #collectTax()
 * @bug I think this should be called after loading a city, or any
 *      time anything it depends on changes.
 */
void Micropolis::updateFundEffects()
{
    // Compute road effects of funding
    roadEffect = MAX_ROAD_EFFECT;
    if (roadFund > 0) {
        // Multiply with funding fraction
        roadEffect = (short)((float)roadEffect * (float)roadSpend / (float)roadFund);
    }

    // Compute police station effects of funding
    policeEffect = MAX_POLICE_STATION_EFFECT;
    if (policeFund > 0) {
        // Multiply with funding fraction
        policeEffect = (short)((float)policeEffect * (float)policeSpend / (float)policeFund);
    }

    // Compute fire station effects of funding
    fireEffect = MAX_FIRE_STATION_EFFECT;
    if (fireFund > 0) {
        // Multiply with funding fraction
        fireEffect = (short)((float)fireEffect * (float)fireSpend / (float)fireFund);
    }

    mustDrawBudget = 1;
}


/* comefrom: simulate doSimInit */
void Micropolis::mapScan(int x1, int x2)
{
    short x, y;

    for (x = x1; x < x2; x++) {
        for (y = 0; y < WORLD_H; y++) {
            curNum = map[x][y];
            if (curNum) {

                curTile = curNum & LOMASK;  /* Mask off status bits  */

                if (curTile >= FLOOD) {

                    curMapX = x;
                    curMapY = y;

                    if (curTile < ROADBASE) {

                        if (curTile >= FIREBASE) {
                            firePop++;
                            if (!(getRandom16() & 3)) {
                                doFire();    /* 1 in 4 times */
                            }
                            continue;
                        }

                        if (curTile < RADTILE) {
                            doFlood();
                        } else {
                            doRadTile();
                        }

                        continue;
                    }

                    if (newPower && (curNum & CONDBIT)) {
                        // Copy PWRBIT from powerGridMap
                        setZonePower(Position(curMapX, curMapY));
                    }

                    if ((curTile >= ROADBASE) &&
                        (curTile < POWERBASE)) {
                        doRoad();
                        continue;
                    }

                    if (curNum & ZONEBIT) { /* process Zones */
                        doZone();
                        continue;
                    }

                    if ((curTile >= RAILBASE) &&
                        (curTile < RESBASE)) {
                        doRail();
                        continue;
                    }

                    if ((curTile >= SOMETINYEXP) &&
                        (curTile <= LASTTINYEXP)) {
                        /* clear AniRubble */
                        map[x][y] = randomRubble();
                    }
                }
            }
        }
    }
}


/** Handle rail tail */
void Micropolis::doRail()
{
    railTotal++;

    generateTrain(curMapX, curMapY);

    if (roadEffect < (15 * MAX_ROAD_EFFECT / 16)) {
        // roadEffect < 15/16 of max road, enable deteriorating rail
        if (!(getRandom16() & 511)) {
            if (!(curNum & CONDBIT)) {
                assert(MAX_ROAD_EFFECT == 32); // Otherwise the '(getRandom16() & 31)' makes no sense
                if (roadEffect < (getRandom16() & 31)) {
                    if (curTile < (RAILBASE + 2)) {
                        map[curMapX][curMapY] = RIVER;
                    } else {
                        map[curMapX][curMapY] = randomRubble();
                    }
                    return;
                }
            }
        }
    }
}


/** Handle decay of radio-active tile */
void Micropolis::doRadTile()
{
    if ((getRandom16() & 4095) == 0) {
        map[curMapX][curMapY] = DIRT; /* Radioactive decay */
    }
}


/** Handle road tile */
void Micropolis::doRoad()
{
    short tden, z;
    static const short densityTable[3] = { ROADBASE, LTRFBASE, HTRFBASE };

    roadTotal++;

    /* generateBus(curMapX, curMapY); */

    if (roadEffect < (15 * MAX_ROAD_EFFECT / 16)) {
        // roadEffect < 15/16 of max road, enable deteriorating road
        if ((getRandom16() & 511) == 0) {
            if (!(curNum & CONDBIT)) {
                assert(MAX_ROAD_EFFECT == 32); // Otherwise the '(getRandom16() & 31)' makes no sense
                if (roadEffect < (getRandom16() & 31)) {
                    if ((curTile & 15) < 2 || (curTile & 15) == 15) {
                        map[curMapX][curMapY] = RIVER;
                    } else {
                        map[curMapX][curMapY] = randomRubble();
                    }
                    return;
                }
            }
        }
    }

    if ((curNum & BURNBIT) == 0) { /* If Bridge */
        roadTotal += 4; // Bridge counts as 4 road tiles
        if (doBridge()) {
            return;
        }
    }

    if (curTile < LTRFBASE) {
        tden = 0;
    } else if (curTile < HTRFBASE) {
        tden = 1;
    } else {
        roadTotal++;
        tden = 2;
    }

    short trafficDensity = trafficDensityMap.worldGet(curMapX, curMapY) >>6;

    if (trafficDensity > 1) {
        trafficDensity--;
    }

    if (tden != trafficDensity) { /* tden 0..2   */
        z = ((curTile - ROADBASE) & 15) + densityTable[trafficDensity];
        z |= curNum & (ALLBITS - ANIMBIT);

        if (trafficDensity > 0) {
            z |= ANIMBIT;
        }

        map[curMapX][curMapY] = z;
    }
}


/** Handle bridge */
bool Micropolis::doBridge()
{
    static short HDx[7] = { -2,  2, -2, -1,  0,  1,  2 };
    static short HDy[7] = { -1, -1,  0,  0,  0,  0,  0 };
    static short HBRTAB[7] = {
        HBRDG1 | BULLBIT, HBRDG3 | BULLBIT, HBRDG0 | BULLBIT,
        RIVER, BRWH | BULLBIT, RIVER, HBRDG2 | BULLBIT,
    };
    static short HBRTAB2[7] = {
        RIVER, RIVER, HBRIDGE | BULLBIT, HBRIDGE | BULLBIT, HBRIDGE | BULLBIT,
        HBRIDGE | BULLBIT, HBRIDGE | BULLBIT,
    };
    static short VDx[7] = {  0,  1,  0,  0,  0,  0,  1 };
    static short VDy[7] = { -2, -2, -1,  0,  1,  2,  2 };
    static short VBRTAB[7] = {
        VBRDG0 | BULLBIT, VBRDG1 | BULLBIT, RIVER, BRWV | BULLBIT,
        RIVER, VBRDG2 | BULLBIT, VBRDG3 | BULLBIT,
    };
    static short VBRTAB2[7] = {
        VBRIDGE | BULLBIT, RIVER, VBRIDGE | BULLBIT, VBRIDGE | BULLBIT,
        VBRIDGE | BULLBIT, VBRIDGE | BULLBIT, RIVER,
    };
    int z, x, y, MPtem;

    if (curTile == BRWV) { /*  Vertical bridge close */

        if ((!(getRandom16() & 3)) && getBoatDistance() > 340) {

            for (z = 0; z < 7; z++) { /* Close */

                x = curMapX + VDx[z];
                y = curMapY + VDy[z];

                if (testBounds(x, y)) {

                    if ((map[x][y] & LOMASK) == (VBRTAB[z] & LOMASK)) {
                        map[x][y] = VBRTAB2[z];
                    }

                }
            }
        }
        return true;
    }

    if (curTile == BRWH) { /*  Horizontal bridge close  */

        if ((!(getRandom16() & 3)) && getBoatDistance() > 340) {

            for (z = 0; z < 7; z++) { /* Close */

                x = curMapX + HDx[z];
                y = curMapY + HDy[z];

                if (testBounds(x, y)) {

                    if ((map[x][y] & LOMASK) == (HBRTAB[z] & LOMASK)) {

                        map[x][y] = HBRTAB2[z];

                    }
                }
            }
        }
        return true;
    }

    if (getBoatDistance() < 300 || (!(getRandom16() & 7))) {
        if (curTile & 1) {
            if (curMapX < WORLD_W - 1) {
                if (map[curMapX + 1][curMapY] == CHANNEL) { /* Vertical open */

                    for (z = 0; z < 7; z++) {

                        x = curMapX + VDx[z];
                        y = curMapY + VDy[z];

                        if (testBounds(x, y)) {

                            MPtem = map[x][y];
                            if (MPtem == CHANNEL || ((MPtem & 15) == (VBRTAB2[z] & 15))) {
                                map[x][y] = VBRTAB[z];
                            }
                        }
                    }
                    return true;
                }
            }
            return false;

        } else {

            if (curMapY > 0) {
                if (map[curMapX][curMapY - 1] == CHANNEL) {

                    /* Horizontal open  */
                    for (z = 0; z < 7; z++) {

                        x = curMapX + HDx[z];
                        y = curMapY + HDy[z];

                        if (testBounds(x, y)) {

                            MPtem = map[x][y];
                            if (((MPtem & 15) == (HBRTAB2[z] & 15)) || MPtem == CHANNEL) {
                                map[x][y] = HBRTAB[z];
                            }
                        }
                    }
                    return true;
                }
            }
            return false;
        }

    }
    return false;
}


/**
 * Compute distance to nearest boat.
 * @return Distance to nearest boat.
 */
int Micropolis::getBoatDistance()
{
    int dist, mx, my, sprDist;
    SimSprite *sprite;

    dist = 99999;
    mx = (curMapX <<4) + 8;
    my = (curMapY <<4) + 8;

    for (sprite = spriteList; sprite != NULL; sprite = sprite->next) {
        if (sprite->type == SPRITE_SHIP && sprite->frame != 0) {

            sprDist = absoluteValue(sprite->x + sprite->xHot - mx)
                    + absoluteValue(sprite->y + sprite->yHot - my);

            dist = min(dist, sprDist);
        }
    }
    return dist;
}


/**
 * Handle tile being on fire
 * @todo Needs a notion of iterative neighbour tiles computing
 */
void Micropolis::doFire()
{
    static const short DX[4] = { -1,  0,  1,  0 };
    static const short DY[4] = {  0, -1,  0,  1 };

    // Try to set neighbouring tiles on fire as well
    for (short z = 0; z < 4; z++) {

        if ((getRandom16() & 7) == 0) {

            short Xtem = curMapX + DX[z];
            short Ytem = curMapY + DY[z];

            if (testBounds(Xtem, Ytem)) {

                short c = map[Xtem][Ytem];
                if (!(c & BURNBIT)) {
                    continue;
                }

                if (c & ZONEBIT) {
                    // Neighbour is a zone and burnable
                    fireZone(Xtem, Ytem, c);

                    if ((c & LOMASK) > IZB) { /* Explode */
                          makeExplosionAt((Xtem <<4) + 8, (Ytem <<4) + 8);
                    }
                }

                map[Xtem][Ytem] = randomFire();
            }
        }
    }

    // Compute likelyhood of fire running out of fuel
    short rate = 10; // Likelyhood of extinguishing (bigger means less chance)
    short z = fireStationEffectMap.worldGet(curMapX, curMapY);

    if (z > 0) {
        rate = 3;
        if (z > 20) {
            rate = 2;
        }
        if (z > 100) {
            rate = 1;
        }
    }

    // Decide whether to put out the fire.
    if (getRandom(rate) == 0) {
        map[curMapX][curMapY] = randomRubble();
    }
}


/**
 * Handle a zone on fire.
 *
 * Decreases rate of growth of the zone, and makes remaining tiles bulldozable.
 *
 * @param Xloc X coordinate of the zone.
 * @param Yloc Y coordinate of the zone.
 * @param ch   Character of the zone.
 */
void Micropolis::fireZone(int Xloc, int Yloc, int ch)
{
    short XYmax;

    int value = rateOfGrowthMap.worldGet(Xloc, Yloc);
    value = clamp(value - 20, -200, 200);
    rateOfGrowthMap.worldSet(Xloc, Yloc, value);

    ch = ch & LOMASK;

    if (ch < PORTBASE) {
        XYmax = 2;
    } else {
        if (ch == AIRPORT) {
            XYmax = 5;
        } else {
            XYmax = 4;
        }
    }

    // Make remaining tiles of the zone bulldozable
    for (short x = -1; x < XYmax; x++) {
        for (short y = -1; y < XYmax; y++) {

            short Xtem = Xloc + x;
            short Ytem = Yloc + y;

            if (Xtem < 0 || Xtem > WORLD_W - 1 ||
                Ytem < 0 || Ytem > WORLD_H - 1) {
                continue;
            }

            if ((short)(map[Xtem][Ytem] & LOMASK) >= ROADBASE) {
                /* post release */
                map[Xtem][Ytem] |= BULLBIT;
            }

        }
    }
}


/**
 * Repair a zone at \a pos.
 * @param pos   Center-tile position of the zone.
 * @param zCent Value of the center tile.
 * @param zSize Size of the zone (in both directions).
 */
void Micropolis::repairZone(const Position &pos, MapTile zCent, short zSize)
{
    MapTile tile = zCent - 2 - zSize;

    // y and x loops one position shifted to compensate for the center-tile position.
    for (short y = -1; y < zSize - 1; y++) {
        for (short x = -1; x < zSize - 1; x++) {

            int xx = pos.posX + x;
            int yy = pos.posY + y;

            tile++;

            if (testBounds(xx, yy)) {

                MapValue mapValue = map[xx][yy];

                if (mapValue & ZONEBIT) {
                    continue;
                }

                if (mapValue & ANIMBIT) {
                    continue;
                }

                MapTile mapTile = mapValue & LOMASK;

                if (mapTile < RUBBLE || mapTile >= ROADBASE) {
                    map[xx][yy] = tile | CONDBIT | BURNBIT;
                }
            }
        }
    }
}


/**
 * Manage special zones.
 * @param powerOn Zone is powered.
 */
void Micropolis::doSpecialZone(bool powerOn)
{
    // Bigger numbers reduce chance of nuclear melt down
    static const short meltdownTable[3] = { 30000, 20000, 10000 };

    switch (curTile) {

        case POWERPLANT:

            coalPowerPop++;

            if ((cityTime & 7) == 0) {
                repairZone(Position(curMapX, curMapY), POWERPLANT, 4); /* post */
            }

            pushPowerStack(Position(curMapX, curMapY));
            coalSmoke(Position(curMapX, curMapY));

            return;

        case NUCLEAR:

            assert(LEVEL_COUNT == LENGTH_OF(meltdownTable));

            if (enableDisasters && !getRandom(meltdownTable[gameLevel])) {
                doMeltdown(Position(curMapX, curMapY));
                return;
            }

            nuclearPowerPop++;

            if ((cityTime & 7) == 0) {
                repairZone(Position(curMapX, curMapY), NUCLEAR, 4); /* post */
            }

            pushPowerStack(Position(curMapX, curMapY));

            return;

        case FIRESTATION: {

            int z;

            fireStationPop++;

            if (!(cityTime & 7)) {
                repairZone(Position(curMapX, curMapY), FIRESTATION, 3); /* post */
            }

            if (powerOn) {
                z = fireEffect;                   /* if powered get effect  */
            } else {
                z = fireEffect / 2;               /* from the funding ratio  */
            }

            if (!findPerimeterRoad()) {
                z = z / 2;                        /* post FD's need roads  */
            }

            int value = fireStationMap.worldGet(curMapX, curMapY);
            value += z;
            fireStationMap.worldSet(curMapX, curMapY, value);

            return;
        }

        case POLICESTATION: {

            int z;

            policeStationPop++;

            if (!(cityTime & 7)) {
                repairZone(Position(curMapX, curMapY), POLICESTATION, 3); /* post */
            }

            if (powerOn) {
                z = policeEffect;
            } else {
                z = policeEffect / 2;
            }

            if (!findPerimeterRoad()) {
                z = z / 2; /* post PD's need roads */
            }

            int value = policeStationMap.worldGet(curMapX, curMapY);
            value += z;
            policeStationMap.worldSet(curMapX, curMapY, value);

            return;
        }

        case STADIUM:  // Empty stadium

            stadiumPop++;

            if (!(cityTime & 15)) {
                repairZone(Position(curMapX, curMapY), STADIUM, 4);
            }

            if (powerOn) {
                // Every now and then, display a match
                if (((cityTime + curMapX + curMapY) & 31) == 0) {
                    drawStadium(FULLSTADIUM);
                    map[curMapX + 1][curMapY] = FOOTBALLGAME1 + ANIMBIT;
                    map[curMapX + 1][curMapY + 1] = FOOTBALLGAME2 + ANIMBIT;
                }
            }

            return;

       case FULLSTADIUM:  // Full stadium

            stadiumPop++;

            if (((cityTime + curMapX + curMapY) & 7) == 0) {
                // Stop the match
                drawStadium(STADIUM);
            }

            return;

       case AIRPORT:

            airportPop++;

            if ((cityTime & 7) == 0) {
                repairZone(Position(curMapX, curMapY), AIRPORT, 6);
            }

            // If powered, display a rotating radar
            if (powerOn) {
                if ((map[curMapX + 1][curMapY - 1] & LOMASK) == RADAR) {
                    map[curMapX + 1][curMapY - 1] = RADAR + ANIMBIT + CONDBIT + BURNBIT;
                }
            } else {
                map[curMapX + 1][curMapY - 1] = RADAR + CONDBIT + BURNBIT;
            }

            if (powerOn) { // Handle the airport only if there is power
                doAirport();
            }

            return;

       case PORT:

            seaportPop++;

            if ((cityTime & 15) == 0) {
                repairZone(Position(curMapX, curMapY), PORT, 4);
            }

            // If port has power and there is no ship, generate one
            if (powerOn && getSprite(SPRITE_SHIP) == NULL) {
                generateShip();
            }

            return;
    }
}


/**
 * Draw the stadium.
 * @param z Base character.
 */
void Micropolis::drawStadium(int z)
{
    int x, y;

    z = z - 5;

    for (y = (curMapY - 1); y < (curMapY + 3); y++) {
        for (x = (curMapX - 1); x < (curMapX + 3); x++) {
            map[x][y] = (z++) | BNCNBIT;
        }
    }

    map[curMapX][curMapY] |= ZONEBIT | PWRBIT;
}


/** Generate a airplane or helicopter every now and then. */
void Micropolis::doAirport()
{
    if (getRandom(5) == 0) {
        generatePlane(curMapX, curMapY);
        return;
    }

    if (getRandom(12) == 0) {
        generateCopter(curMapX, curMapY);
    }
}


/**
 * Draw coal smoke tiles around given position (of a coal power plant).
 * @param pos Center tile of the coal power plant
 */
void Micropolis::coalSmoke(const Position &pos)
{
    static const short SmTb[4] = {
        COALSMOKE1, COALSMOKE2,
        COALSMOKE3, COALSMOKE4,
    };
    static const short dx[4] = {  1,  2,  1,  2 };
    static const short dy[4] = { -1, -1,  0,  0 };

    for (short x = 0; x < 4; x++) {
        map[pos.posX + dx[x]][pos.posY + dy[x]] =
            SmTb[x] | ANIMBIT | CONDBIT | PWRBIT | BURNBIT;
    }
}


/**
 * Perform a nuclear melt-down disaster
 * @param pos Position of the nuclear power plant that melts.
 */
void Micropolis::doMeltdown(const Position &pos)
{
    makeExplosion(pos.posX - 1, pos.posY - 1);
    makeExplosion(pos.posX - 1, pos.posY + 2);
    makeExplosion(pos.posX + 2, pos.posY - 1);
    makeExplosion(pos.posX + 2, pos.posY + 2);

    // Whole power plant is at fire
    for (int x = pos.posX - 1; x < pos.posX + 3; x++) {
        for (int y = pos.posY - 1; y < pos.posY + 3; y++) {
            map[x][y] = randomFire();
        }
    }

    // Add lots of radiation tiles around the plant
    for (int z = 0; z < 200; z++)  {

        int x = pos.posX - 20 + getRandom(40);
        int y = pos.posY - 15 + getRandom(30);

        if (!testBounds(x, y)) { // Ignore off-map positions
            continue;
        }

        MapValue t = map[x][y];

        if (t & ZONEBIT) {
            continue; // Ignore zones
        }

        if ((t & BURNBIT) || t == DIRT) {
            map[x][y] = RADTILE; // Make tile radio-active
        }

    }

    // Report disaster to the user
    sendMessage(MESSAGE_NUCLEAR_MELTDOWN, pos.posX, pos.posY, true, true);
}


////////////////////////////////////////////////////////////////////////
