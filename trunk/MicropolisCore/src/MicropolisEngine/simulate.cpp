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
#include "text.h"


////////////////////////////////////////////////////////////////////////


/* comefrom: doEditWindow scoreDoer doMapInFront graphDoer doNilEvent */
void Micropolis::SimFrame()
{

    if (SimSpeed == 0) {
        return;
    }

    if (++speedCycle > 1023) {
        speedCycle = 0;
    }

    if (SimSpeed == 1 && (speedCycle % 5) != 0) {
        return;
    }

    if (SimSpeed == 2 && (speedCycle % 3) != 0) {
        return;
    }

    if (++phaseCycle > 1023) {
        phaseCycle = 0;
    }

    // Why is this disabled? Look useful for initializing after loading.
/*
    if (InitSimLoad) {
        phaseCycle = 0;
    }
*/

    Simulate(phaseCycle & 15);
}


/* comefrom: SimFrame */
void Micropolis::Simulate(int mod16)
{
    static const short SpdPwr[4] = { 1,  2,  4,  5 };
    static const short SpdPtl[4] = { 1,  2,  7, 17 };
    static const short SpdCri[4] = { 1,  1,  8, 18 };
    static const short SpdPop[4] = { 1,  1,  9, 19 };
    static const short SpdFir[4] = { 1,  1, 10, 20 };

    short x = clamp(SimSpeed, (short)1, (short)3);

    switch (mod16)  {

        case 0:
            if (++simCycle > 1023) {
                simCycle = 0;     /* this is cosmic */
            }
            if (doInitialEval) {
                doInitialEval = false;
                CityEvaluation();
            }
            cityTime++;
            cityTaxAverage += cityTax;             /* post */
            if (!(simCycle & 1)) {
                SetValves();
            }
            ClearCensus();
            break;

        case 1:
            MapScan(0 * WORLD_X / 8, 1 * WORLD_X / 8);
            break;

        case 2:
            MapScan(1 * WORLD_X / 8, 2 * WORLD_X / 8);
            break;

        case 3:
            MapScan(2 * WORLD_X / 8, 3 * WORLD_X / 8);
            break;

        case 4:
            MapScan(3 * WORLD_X / 8, 4 * WORLD_X / 8);
            break;

        case 5:
            MapScan(4 * WORLD_X / 8, 5 * WORLD_X / 8);
            break;

        case 6:
            MapScan(5 * WORLD_X / 8, 6 * WORLD_X / 8);
            break;

        case 7:
            MapScan(6 * WORLD_X / 8, 7 * WORLD_X / 8);
            break;

        case 8:
            MapScan(7 * WORLD_X / 8, 8 * WORLD_X / 8);
            break;

        case 9:
            if ((cityTime % CENSUSRATE) == 0) {
                TakeCensus();
            }
            if ((cityTime % (CENSUSRATE * 12)) == 0) {
                Take2Census();
            }

            if ((cityTime % TAXFREQ) == 0)  {
                CollectTax();
                CityEvaluation();
            }
            break;

        case 10:
            if (!(simCycle % 5)) {
                DecROGMem();
            }
            DecTrafficMem();
            NewMapFlags[TDMAP] = 1;
            NewMapFlags[RDMAP] = 1;
            NewMapFlags[ALMAP] = 1;
            NewMapFlags[REMAP] = 1;
            NewMapFlags[COMAP] = 1;
            NewMapFlags[INMAP] = 1;
            NewMapFlags[DYMAP] = 1;
            SendMessages();
            break;

        case 11:
            if ((simCycle % SpdPwr[x]) == 0) {
                DoPowerScan();
                NewMapFlags[PRMAP] = 1;
                NewPower = 1; /* post-release change */
            }
            break;

        case 12:
            if ((simCycle % SpdPtl[x]) == 0) {
                PTLScan();
            }
            break;

        case 13:
            if ((simCycle % SpdCri[x]) == 0) {
                CrimeScan();
            }
            break;

        case 14:
            if ((simCycle % SpdPop[x]) == 0) {
                PopDenScan();
            }
            break;

        case 15:
            if ((simCycle % SpdFir[x]) == 0) {
                FireAnalysis();
            }
            DoDisasters();
            break;

    }
}


/* comefrom: Simulate */
void Micropolis::DoSimInit()
{
    phaseCycle = 0;
    simCycle = 0;

    if (InitSimLoad == 2) {
        /* if new city */
        InitSimMemory();
    }

    if (InitSimLoad == 1) {
        /* if city just loaded */
        SimLoadInit();
    }

    SetValves();
    ClearCensus();
    MapScan(0, WORLD_X);
    DoPowerScan();
    NewPower = 1;         /* post rel */
    PTLScan();
    CrimeScan();
    PopDenScan();
    FireAnalysis();
    NewMap = 1;
    doAllGraphs();
    newGraph = true;
    totalPop = 1;
    doInitialEval = true;
}


/**
 * Copy bits from powerMap to the #PWRBIT in the map for all zones in the
 * world.
 */
void Micropolis::DoNilPower()
{
    short x, y, z;

    for (x = 0; x < WORLD_X; x++) {
        for (y = 0; y < WORLD_Y; y++) {
            z = map[x][y];
            if (z & ZONEBIT) {
                curMapX = x;
                curMapY = y;
                curNum = z;
                /// @bug: Should set #curTile to (#curNum & #LOMASK), since it is
                ///       used by #SetZPower to distinguish nuclear and coal
                ///       power plants. Better yet, pass all parameters into
                ///       #SetZPower and rewrite it not to use globals.
                SetZPower();
            }
        }
    }
}


/** Decrease traffic memory */
void Micropolis::DecTrafficMem()
{
    /* tends to empty trafficDensityMap */
    short x, y, z;

    for (x = 0; x < HWLDX; x++) {
        for (y = 0; y < HWLDY; y++) {
            z = trafficDensityMap[x][y];
            if (z == 0) {
                continue;
            }

            if (z <= 24) {
                trafficDensityMap[x][y] = 0;
                continue;
            }

            if (z > 200) {
                trafficDensityMap[x][y] = z - 34;
            } else {
                trafficDensityMap[x][y] = z - 24;
            }
        }
    }
}


/**
 * Decrease rate of grow.
 * @todo Limiting rate should not be done here, but when we add a new value to
 *       it.
 */
void Micropolis::DecROGMem()
{
    /* tends to empty rateOfGrowthMap */
    short x, y, z;

    for (x = 0; x < SmX; x++) {
        for (y = 0; y < SmY; y++) {
            z = rateOfGrowthMap[x][y];
            if (z == 0) {
                continue;
            }

            if (z > 0) {
                --rateOfGrowthMap[x][y];
                if (z > 200) {
                    rateOfGrowthMap[x][y] = 200;    /* prevent overflow */
                }
                continue;
            }

            if (z < 0)  {
                ++rateOfGrowthMap[x][y];
                if (z < -200) {
                    rateOfGrowthMap[x][y] = -200;
                }
            }
        }
    }
}


/* comefrom: DoSimInit */
void Micropolis::InitSimMemory()
{
    SetCommonInits();

    for (short x = 0; x < 240; x++)  {
        resHist[x] = 0;
        comHist[x] = 0;
        indHist[x] = 0;
        moneyHist[x] = 128;
        crimeHist[x] = 0;
        pollutionHist[x] = 0;
    }

    CrimeRamp = 0;
    PolluteRamp = 0;
    totalPop = 0;
    RValve = 0;
    CValve = 0;
    IValve = 0;
    ResCap = 0;
    ComCap = 0;
    IndCap = 0;

    EMarket = 6.0;
    DisasterEvent = SC_NONE;
    ScoreType = SC_NONE;

    /* This clears powermem */
    powerStackNum = 0;
    DoPowerScan();
    NewPower = 1; /* post rel */

    InitSimLoad = 0;
}


/* comefrom: DoSimInit */
void Micropolis::SimLoadInit()
{
    // Disaster delay table for each scenario
    static const short DisTab[9] = {
        0, 2, 10, 5, 20, 3, 5, 5, 2 * 48,
    };

    // Time to wait before score calculation for each scenario
    static const short ScoreWaitTab[9] = {
        0, 30 * 48, 5 * 48, 5 * 48, 10 * 48,
        5 * 48, 10 * 48, 5 * 48, 10 * 48
    };

    EMarket = (float)miscHist[1];
    resPop = miscHist[2];
    comPop = miscHist[3];
    indPop = miscHist[4];
    RValve = miscHist[5];
    CValve = miscHist[6];
    IValve = miscHist[7];
    CrimeRamp = miscHist[10];
    PolluteRamp = miscHist[11];
    landValueAverage = miscHist[12];
    crimeAverage = miscHist[13];
    pollutionAverage = miscHist[14];
    gameLevel = (GameLevel)miscHist[15];

    if (cityTime < 0) {
        cityTime = 0;
    }

    if (!EMarket) {
        EMarket = 4.0;
    }

    // Set game level
    if (gameLevel > LEVEL_LAST || gameLevel < LEVEL_FIRST) {
        gameLevel = LEVEL_FIRST;
    }
    SetGameLevel(gameLevel);

    SetCommonInits();

    // Load cityClass
    cityClass = (CityClass)(miscHist[16]);
    if (cityClass > CC_MEGALOPOLIS || cityClass < CC_VILLAGE) {
        cityClass = CC_VILLAGE;
    }

    cityScore = miscHist[17];
    if (cityScore > 999 || cityScore < 1) {
        cityScore = 500;
    }

    ResCap = 0;
    ComCap = 0;
    IndCap = 0;

    cityTaxAverage = (cityTime % 48) * 7;  /* post */

    for (int z = 0; z < PWRMAPSIZE; z++) {
        powerMap[z] = ~0; /* set power Map */
    }

    DoNilPower();

    if (ScenarioID >= SC_COUNT) {
        ScenarioID = SC_NONE;
    }

    if (ScenarioID != SC_NONE) {
        assert(LENGTH_OF(DisTab) == SC_COUNT);
        assert(LENGTH_OF(ScoreWaitTab) == SC_COUNT);

        DisasterEvent = ScenarioID;
        DisasterWait = DisTab[DisasterEvent];
        ScoreType = DisasterEvent;
        ScoreWait = ScoreWaitTab[DisasterEvent];
    } else {
        DisasterEvent = SC_NONE;
        ScoreType = SC_NONE;
    }

    roadEffect = MAX_ROAD_EFFECT;
    policeEffect = MAX_POLICESTATION_EFFECT;
    fireEffect = MAX_FIRESTATION_EFFECT;
    InitSimLoad = 0;
}


/* comefrom: InitSimMemory SimLoadInit */
void Micropolis::SetCommonInits()
{
    EvalInit();
    roadEffect = MAX_ROAD_EFFECT;
    policeEffect = MAX_POLICESTATION_EFFECT;
    fireEffect = MAX_FIRESTATION_EFFECT;
    taxFlag = false;
    taxFund = 0;
}


/* comefrom: Simulate DoSimInit */
void Micropolis::SetValves()
{
    /// @todo Break the tax table out into configurable parameters.
    static const short TaxTable[21] = {
        200, 150, 120, 100, 80, 50, 30, 0, -10, -40, -100,
        -150, -200, -250, -300, -350, -400, -450, -500, -550, -600,
    };
    static const float extMarketParamTable[3] = {
        1.2f, 1.1f, 0.98f,
    };
    assert(LEVEL_COUNT == LENGTH_OF(extMarketParamTable));

    /// @todo Make configurable parameters.
    short resPopDenom = 8;
    float BirthRate = 0.02;
    float MaxLaborBase = 1.3;
    float IntMarketDenom = 3.7;
    float MinPjindPop = 5.0;
    float DefaultRratio = 1.3;
    float MaxRratio = 2;
    float MaxCratio = 2;
    float MaxIratio = 2;
    short MaxTax = 20;
    float TaxTableScale = 600;
    short MaxRValve = 2000;
    short MinRValve = -2000;
    short MaxCValve = 1500;
    short MinCValve = -1500;
    short MaxIValve = 1500;
    short MinIValve = -1500;

    /// @todo Break the interesting values out into public member
    ///       variables so the user interface can display them.
    float Employment, Migration, Births, LaborBase, IntMarket;
    float Rratio, Cratio, Iratio;
    float NormresPop, PjresPop, PjcomPop, PjindPop;

    miscHist[1] = (short)EMarket;
    miscHist[2] = resPop;
    miscHist[3] = comPop;
    miscHist[4] = indPop;
    miscHist[5] = RValve;
    miscHist[6] = CValve;
    miscHist[7] = IValve;
    miscHist[10] = CrimeRamp;
    miscHist[11] = PolluteRamp;
    miscHist[12] = landValueAverage;
    miscHist[13] = crimeAverage;
    miscHist[14] = pollutionAverage;
    miscHist[15] = gameLevel;
    miscHist[16] = (short)cityClass;
    miscHist[17] = cityScore;

    NormresPop = (float)resPop / (float)resPopDenom;
    totalPopLast = totalPop;
    totalPop = (short)(NormresPop + comPop + indPop);

    if (resPop > 0) {
        Employment = (comHist[1] + indHist[1]) / NormresPop;
    } else {
        Employment = 1;
    }

    Migration = NormresPop * (Employment - 1);
    Births = NormresPop * BirthRate;
    PjresPop = NormresPop + Migration + Births;   // Projected res pop.

    // Compute LaborBase
    float temp = comHist[1] + indHist[1];
    if (temp > 0.0) {
        LaborBase = (resHist[1] / temp);
    } else {
        LaborBase = 1;
    }
    LaborBase = clamp(LaborBase, 0.0f, MaxLaborBase);

    IntMarket = (float)(NormresPop + comPop + indPop) / IntMarketDenom;

    PjcomPop = IntMarket * LaborBase;

    assert(gameLevel >= LEVEL_FIRST && gameLevel <= LEVEL_LAST);
    PjindPop = indPop * LaborBase * extMarketParamTable[gameLevel];
    PjindPop = max(PjindPop, MinPjindPop);

    if (NormresPop > 0) {
        Rratio = (float)PjresPop / (float)NormresPop; // Projected -vs- actual.
    } else {
        Rratio = DefaultRratio;
    }

    if (comPop > 0) {
        Cratio = (float)PjcomPop / (float)comPop;
    } else {
        Cratio = (float)PjcomPop;
    }

    if (indPop > 0) {
        Iratio = (float)PjindPop / (float)indPop;
    } else {
        Iratio = (float)PjindPop;
    }

    Rratio = min(Rratio, MaxRratio);
    Cratio = min(Cratio, MaxCratio);
    Rratio = min(Iratio, MaxIratio);

    // Global tax and game level effects.
    short z = min((short)(cityTax + gameLevel), MaxTax);
    Rratio = (Rratio - 1) * TaxTableScale + TaxTable[z];
    Cratio = (Cratio - 1) * TaxTableScale + TaxTable[z];
    Iratio = (Iratio - 1) * TaxTableScale + TaxTable[z];

    // Ratios are velocity changes to valves.
    if (Rratio > 0) {
        if (RValve < MaxRValve) {
            RValve += (short)Rratio;
        }
    }

    if (Rratio < 0) {
        if (RValve > MinRValve) {
            RValve += (short)Rratio; // Adding a negative number
        }
    }

    if (Cratio > 0) {
        if (CValve <  MaxCValve) {
            CValve += (short)Cratio;
        }
    }

    if (Cratio < 0) {
        if (CValve > MinCValve) {
            CValve += (short)Cratio; // Adding a negative number
        }
    }

    if (Iratio > 0) {
        if (IValve <  MaxIValve) {
            IValve += (short)Iratio;
        }
    }

    if (Iratio < 0) {
        if (IValve > MinIValve) {
            IValve += (short)Iratio; // Adding a negative number
        }
    }

    RValve = clamp(RValve, MinRValve, MaxRValve);
    CValve = clamp(CValve, MinCValve, MaxCValve);
    IValve = clamp(IValve, MinIValve, MaxIValve);

    if (ResCap && (RValve > 0)) {
        RValve = 0; // Need a stadium, so cap RValve.
    }

    if (ComCap && (CValve > 0)) {
        CValve = 0; // Need a airport, so cap CValve.
    }

    if (IndCap && (IValve > 0)) {
        IValve = 0; // Need an seaport, so cap IValve.
    }

    ValveFlag = 1;
}


/* comefrom: Simulate DoSimInit */
void Micropolis::ClearCensus()
{
    PwrdZCnt = 0;
    unPwrdZCnt = 0;
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
    powerStackNum = 0;            /* Reset before Mapscan */

    for (short x = 0; x < SmX; x++) {
        for (short y = 0; y < SmY; y++) {
            fireStationMap[x][y] = 0;
            policeStationMap[x][y] = 0;
        }
    }

}


/**
 * @todo A lot of this max stuff is also done in graph.cpp
 */
void Micropolis::TakeCensus()
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

    Graph10Max = resHist10Max;
    Graph10Max = max(Graph10Max, comHist10Max);
    Graph10Max = max(Graph10Max, indHist10Max);

    resHist[0] = resPop / resPopDenom;
    comHist[0] = comPop;
    indHist[0] = indPop;

    CrimeRamp += (crimeAverage - CrimeRamp) / 4;
    crimeHist[0] = min(CrimeRamp, (short)255);

    PolluteRamp += (pollutionAverage - PolluteRamp) / 4;
    pollutionHist[0] = min(PolluteRamp, (short)255);

    x = (CashFlow / 20) + 128;    /* scale to 0..255  */
    moneyHist[0] = clamp(x, (short)0, (short)255);


    ChangeCensus();

    short resPopScaled = resPop >> 8;

    if (hospitalPop < resPopScaled) {
        needHosp = 1;
    }

    if (hospitalPop > resPopScaled) {
        needHosp = -1;
    }

    if (hospitalPop == resPopScaled) {
        needHosp = 0;
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


/* comefrom: Simulate */
void Micropolis::Take2Census()
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

    Graph120Max = resHist120Max;
    Graph120Max = max(Graph120Max, comHist120Max);
    Graph120Max = max(Graph120Max, indHist120Max);

    resHist[120] = resPop / resPopDenom;
    comHist[120] = comPop;
    indHist[120] = indPop;
    crimeHist[120] = crimeHist[0] ;
    pollutionHist[120] = pollutionHist[0];
    moneyHist[120] = moneyHist[0];
    ChangeCensus();
}


/** Collect taxes
 * @bug Function seems to be doing different things depending on
 *      Micropolis::totalPop value. With an non-empty city it does fund
 *      calculations. For an empty city, it immediately sets effects of
 *      funding, which seems inconsistent at least, and may be wrong
 * @bug If Micropolis::taxFlag is set, no variable is touched which seems
 *      non-robust at least
 */
void Micropolis::CollectTax()
{
    short z;

    /**
     * @todo Break out so the user interface can configure this.
     */
    static const float RLevels[3] = { 0.7, 0.9, 1.2 };
    static const float FLevels[3] = { 1.4, 1.2, 0.8 };

    assert(LEVEL_COUNT == LENGTH_OF(RLevels));
    assert(LEVEL_COUNT == LENGTH_OF(FLevels));

    CashFlow = 0;

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
            CashFlow = (short)(taxFund - (policeFund + fireFund + roadFund));
            DoBudget();
        } else {
            /* Nobody lives here. */
            roadEffect   = MAX_ROAD_EFFECT;
            policeEffect = MAX_POLICESTATION_EFFECT;
            fireEffect   = MAX_FIRESTATION_EFFECT;
        }
    }
}


/**
 * Update effects of (possibly reduced) funding
 *
 * It updates effects with respect to roads, police, and fire.
 * @note This function should probably not be used when #totalPop is
 *       clear (ie with an empty) city. See also bugs of #CollectTax()
 */
void Micropolis::UpdateFundEffects()
{
    // Compute road effects of funding
    roadEffect = MAX_ROAD_EFFECT;
    if (roadFund > 0) {
        // Multiply with funding fraction
        roadEffect = (short)((float)roadEffect * (float)roadSpend / (float)roadFund);
    }

    // Compute police station effects of funding
    policeEffect = MAX_POLICESTATION_EFFECT;
    if (policeFund > 0) {
        // Multiply with funding fraction
        policeEffect = (short)((float)policeEffect * (float)policeSpend / (float)policeFund);
    }

    // Compute fire station effects of funding
    fireEffect = MAX_FIRESTATION_EFFECT;
    if (fireFund > 0) {
        // Multiply with funding fraction
        fireEffect = (short)((float)fireEffect * (float)fireSpend / (float)fireFund);
    }

    drawCurrPercents();
}


/* comefrom: Simulate DoSimInit */
void Micropolis::MapScan(int x1, int x2)
{
    short x, y;

    for (x = x1; x < x2; x++) {
        for (y = 0; y < WORLD_Y; y++) {
            curNum = map[x][y];
            if (curNum) {

                curTile = curNum & LOMASK;  /* Mask off status bits  */

                if (curTile >= FLOOD) {

                    curMapX = x;
                    curMapY = y;

                    if (curTile < ROADBASE) {

                        if (curTile >= FIREBASE) {
                            firePop++;
                            if (!(Rand16() & 3)) {
                                DoFire();    /* 1 in 4 times */
                            }
                            continue;
                        }

                        if (curTile < RADTILE) {
                            DoFlood();
                        } else {
                            DoRadTile();
                        }

                        continue;
                    }

                    if (NewPower && (curNum & CONDBIT)) {
                        SetZPower(); // Set PWRBIT from powerMap
                    }

                    if ((curTile >= ROADBASE) &&
                        (curTile < POWERBASE)) {
                        DoRoad();
                        continue;
                    }

                    if (curNum & ZONEBIT) { /* process Zones */
                        DoZone();
                        continue;
                    }

                    if ((curTile >= RAILBASE) &&
                        (curTile < RESBASE)) {
                        DoRail();
                        continue;
                    }

                    if ((curTile >= SOMETINYEXP) &&
                        (curTile <= LASTTINYEXP)) {
                        /* clear AniRubble */
                        map[x][y] = RandomRubble();
                    }
                }
            }
        }
    }
}


/** Handle rail tail */
void Micropolis::DoRail()
{
    railTotal++;

    GenerateTrain(curMapX, curMapY);

    if (roadEffect < (15 * MAX_ROAD_EFFECT / 16)) {
        // roadEffect < 15/16 of max road, enable deteriorating rail
        if (!(Rand16() & 511)) {
            if (!(curNum & CONDBIT)) {
                assert(MAX_ROAD_EFFECT == 32); // Otherwise the '(Rand16() & 31)' makes no sense
                if (roadEffect < (Rand16() & 31)) {
                    if (curTile < (RAILBASE + 2)) {
                        map[curMapX][curMapY] = RIVER;
                    } else {
                        map[curMapX][curMapY] = RandomRubble();
                    }
                    return;
                }
            }
        }
    }
}


/** Handle decay of radio-active tile */
void Micropolis::DoRadTile()
{
    if ((Rand16() & 4095) == 0) {
        map[curMapX][curMapY] = DIRT; /* Radioactive decay */
    }
}


/** Handle road tile */
void Micropolis::DoRoad()
{
    short Density, tden, z;
    static short DenTab[3] = { ROADBASE, LTRFBASE, HTRFBASE };

    roadTotal++;

    /* GenerateBus(curMapX, curMapY); */

    if (roadEffect < (15 * MAX_ROAD_EFFECT / 16)) {
        // roadEffect < 15/16 of max road, enable deteriorating road
        if ((Rand16() & 511) == 0) {
            if (!(curNum & CONDBIT)) {
                assert(MAX_ROAD_EFFECT == 32); // Otherwise the '(Rand16() & 31)' makes no sense
                if (roadEffect < (Rand16() & 31)) {
                    if ((curTile & 15) < 2 || (curTile & 15) == 15) {
                        map[curMapX][curMapY] = RIVER;
                    } else {
                        map[curMapX][curMapY] = RandomRubble();
                    }
                    return;
                }
            }
        }
    }

    if ((curNum & BURNBIT) == 0) { /* If Bridge */
        roadTotal += 4; // Bridge counts as 4 road tiles
        if (DoBridge()) {
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

    Density = (trafficDensityMap[curMapX >>1][curMapY >>1]) >>6;  /* Set Traf Density */

    if (Density > 1) {
        Density--;
    }

    if (tden != Density) { /* tden 0..2   */
        z = ((curTile - ROADBASE) & 15) + DenTab[Density];
        z |= curNum & (ALLBITS - ANIMBIT);

        if (Density > 0) {
            z |= ANIMBIT;
        }

        map[curMapX][curMapY] = z;
    }
}


/** Handle bridge */
bool Micropolis::DoBridge()
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

        if ((!(Rand16() & 3)) && GetBoatDis() > 340) {

            for (z = 0; z < 7; z++) { /* Close */

                x = curMapX + VDx[z];
                y = curMapY + VDy[z];

                if (TestBounds(x, y)) {

                    if ((map[x][y] & LOMASK) == (VBRTAB[z] & LOMASK)) {
                        map[x][y] = VBRTAB2[z];
                    }

                }
            }
        }
        return true;
    }

    if (curTile == BRWH) { /*  Horizontal bridge close  */

        if ((!(Rand16() & 3)) && GetBoatDis() > 340) {

            for (z = 0; z < 7; z++) { /* Close */

                x = curMapX + HDx[z];
                y = curMapY + HDy[z];

                if (TestBounds(x, y)) {

                    if ((map[x][y] & LOMASK) == (HBRTAB[z] & LOMASK)) {

                        map[x][y] = HBRTAB2[z];

                    }
                }
            }
        }
        return true;
    }

    if (GetBoatDis() < 300 || (!(Rand16() & 7))) {
        if (curTile & 1) {
            if (curMapX < WORLD_X - 1) {
                if (map[curMapX + 1][curMapY] == CHANNEL) { /* Vertical open */

                    for (z = 0; z < 7; z++) {

                        x = curMapX + VDx[z];
                        y = curMapY + VDy[z];

                        if (TestBounds(x, y)) {

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

                        if (TestBounds(x, y)) {

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
int Micropolis::GetBoatDis()
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
void Micropolis::DoFire()
{
    static const short DX[4] = { -1,  0,  1,  0 };
    static const short DY[4] = {  0, -1,  0,  1 };

    // Try to set neighbouring tiles on fire as well
    for (short z = 0; z < 4; z++) {

        if ((Rand16() & 7) == 0) {

            short Xtem = curMapX + DX[z];
            short Ytem = curMapY + DY[z];

            if (TestBounds(Xtem, Ytem)) {

                short c = map[Xtem][Ytem];
                if ((c & (BURNBIT | ZONEBIT)) == (BURNBIT | ZONEBIT)) {
                    // Neighbour is a zone and burnable
                    FireZone(Xtem, Ytem, c);

                    if ((c & LOMASK) > IZB) { /* Explode */
                          MakeExplosionAt((Xtem <<4) + 8, (Ytem <<4) + 8);
                    }
                }

                map[Xtem][Ytem] = RandomFire();
            }
        }
    }

    // Compute likelyhood of fire running out of fuel
    short Rate = 10; // Likelyhood of extinguishing (bigger means less chance)
    short z = fireStationMapEffect[curMapX >>3][curMapY >>3];

    if (z > 0) {
        Rate = 3;
        if (z > 20) {
            Rate = 2;
        }
        if (z > 100) {
            Rate = 1;
        }
    }

    // Decide whether to put out the fire.
    if (Rand(Rate) == 0) {
        map[curMapX][curMapY] = RandomRubble();
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
void Micropolis::FireZone(int Xloc, int Yloc, int ch)
{
    short XYmax;

    rateOfGrowthMap[Xloc >>3][Yloc >>3] -= 20;

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

            if (Xtem < 0 || Xtem > WORLD_X - 1 ||
                Ytem < 0 || Ytem > WORLD_Y - 1) {
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
 * Repair a zone at (#curMapX, #curMapY).
 * @param ZCent Value of the center tile.
 * @param zsize Size of the zone (in both directions).
 */
void Micropolis::RepairZone(short ZCent, short zsize)
{
    short cnt;
    short x, y, ThCh;

    zsize--;
    cnt = 0;

    for (y = -1; y < zsize; y++) {

        for (x = -1; x < zsize; x++) {

            int xx = curMapX + x;
            int yy = curMapY + y;

            cnt++;

            if (TestBounds(xx, yy)) {

                ThCh = map[xx][yy];

                if (ThCh & ZONEBIT) {
                    continue;
                }

                if (ThCh & ANIMBIT) {
                    continue;
                }

                ThCh = ThCh & LOMASK;

                if (ThCh < RUBBLE || ThCh >= ROADBASE) {
                    map[xx][yy] = ZCent - 3 - zsize + cnt + CONDBIT + BURNBIT;
                }
            }
        }
    }
}


/**
 * Manage special zones.
 * @param pwrOn Zone is powered.
 */
void Micropolis::DoSPZone(bool pwrOn)
{
    // Bigger numbers reduce chance of nuclear melt down
    static const short MeltdownTable[3] = { 30000, 20000, 10000 };

    switch (curTile) {

        case POWERPLANT:

            coalPowerPop++;

            if ((cityTime & 7) == 0) {
                RepairZone(POWERPLANT, 4); /* post */
            }

            PushPowerStack();
            CoalSmoke(curMapX, curMapY);

            return;

        case NUCLEAR:

            assert(LEVEL_COUNT == LENGTH_OF(MeltdownTable));

            if (!NoDisasters && !Rand(MeltdownTable[gameLevel])) {
                DoMeltdown(curMapX, curMapY);
                return;
            }

            nuclearPowerPop++;

            if ((cityTime & 7) == 0) {
                RepairZone(NUCLEAR, 4); /* post */
            }

            PushPowerStack();

            return;

        case FIRESTATION: {

            int z;

            fireStationPop++;

            if (!(cityTime & 7)) {
                RepairZone(FIRESTATION, 3); /* post */
            }

            if (pwrOn) {
                z = fireEffect;                   /* if powered get effect  */
            } else {
                z = fireEffect / 2;               /* from the funding ratio  */
            }

            if (!FindPRoad()) {
                z = z / 2;                        /* post FD's need roads  */
            }

            fireStationMap[curMapX >>3][curMapY >>3] += z;

            return;
        }

        case POLICESTATION: {

            int z;

            policeStationPop++;

            if (!(cityTime & 7)) {
                RepairZone(POLICESTATION, 3); /* post */
            }

            if (pwrOn) {
                z = policeEffect;
            } else {
                z = policeEffect / 2;
            }

            if (!FindPRoad()) {
                z = z / 2; /* post PD's need roads */
            }

            policeStationMap[curMapX >>3][curMapY >>3] += z;

            return;
        }

        case STADIUM:  // Empty stadium

            stadiumPop++;

            if (!(cityTime & 15)) {
                RepairZone(STADIUM, 4);
            }

            if (pwrOn) {
                // Every now and then, display a match
                if (((cityTime + curMapX + curMapY) & 31) == 0) {
                    DrawStadium(FULLSTADIUM);
                    map[curMapX + 1][curMapY] = FOOTBALLGAME1 + ANIMBIT;
                    map[curMapX + 1][curMapY + 1] = FOOTBALLGAME2 + ANIMBIT;
                }
            }

            return;

       case FULLSTADIUM:  // Full stadium

            stadiumPop++;

            if (((cityTime + curMapX + curMapY) & 7) == 0) {
                // Stop the match
                DrawStadium(STADIUM);
            }

            return;

       case AIRPORT:

            airportPop++;

            if ((cityTime & 7) == 0) {
                RepairZone(AIRPORT, 6);
            }

            // If powered, display a rotating radar
            if (pwrOn) {
                if ((map[curMapX + 1][curMapY - 1] & LOMASK) == RADAR) {
                    map[curMapX + 1][curMapY - 1] = RADAR + ANIMBIT + CONDBIT + BURNBIT;
                }
            } else {
                map[curMapX + 1][curMapY - 1] = RADAR + CONDBIT + BURNBIT;
            }

            if (pwrOn) { // Handle the airport only if there is power
                DoAirport();
            }

            return;

       case PORT:

            seaportPop++;

            if ((cityTime & 15) == 0) {
                RepairZone(PORT, 4);
            }

            // If port has power and there is no ship, generate one
            if (pwrOn && GetSprite(SPRITE_SHIP) == NULL) {
                GenerateShip();
            }

            return;
    }
}


/**
 * Draw the stadium.
 * @param z Base character.
 */
void Micropolis::DrawStadium(int z)
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
void Micropolis::DoAirport()
{
    if (Rand(5) == 0) {
        GeneratePlane(curMapX, curMapY);
        return;
    }

    if (Rand(12) == 0) {
        GenerateCopter(curMapX, curMapY);
    }
}


/**
 * Draw coal smoke tiles around given position (of a coal power plant).
 * @param mx X coordinate of the position.
 * @param my Y coordinate of the position.
 */
void Micropolis::CoalSmoke(int mx, int my)
{
    static const short SmTb[4] = {
        COALSMOKE1, COALSMOKE2,
        COALSMOKE3, COALSMOKE4,
    };
    static const short dx[4] = {  1,  2,  1,  2 };
    static const short dy[4] = { -1, -1,  0,  0 };

    for (short x = 0; x < 4; x++) {
        map[mx + dx[x]][my + dy[x]] =
            SmTb[x] | ANIMBIT | CONDBIT | PWRBIT | BURNBIT;
    }
}


/**
 * Perform a nuclear melt-down disaster
 * @param SX X coordinate of the disaster
 * @param SY Y coordinate of the disaster
 */
void Micropolis::DoMeltdown(int SX, int SY)
{
    MakeExplosion(SX - 1, SY - 1);
    MakeExplosion(SX - 1, SY + 2);
    MakeExplosion(SX + 2, SY - 1);
    MakeExplosion(SX + 2, SY + 2);

    // Whole power plant is at fire
    for (int x = SX - 1; x < SX + 3; x++) {
        for (int y = SY - 1; y < SY + 3; y++) {
            map[x][y] = RandomFire();
        }
    }

    // and lots of radiation tiles around the plant
    for (int z = 0; z < 200; z++)  {

        int x = SX - 20 + Rand(40);
        int y = SY - 15 + Rand(30);

        if (!TestBounds(x, y)) { // Ignore off-map positions
            continue;
        }

        int t = map[x][y];

        if (t & ZONEBIT) {
            continue; // Ignore zones
        }

        if ((t & BURNBIT) || t == DIRT) {
            map[x][y] = RADTILE; // Make tile radio-active
        }

    }

    // Report disaster to the user
    ClearMes();
    SendMesAt(-STR301_NUCLEAR_MELTDOWN, SX, SY);
}


////////////////////////////////////////////////////////////////////////
