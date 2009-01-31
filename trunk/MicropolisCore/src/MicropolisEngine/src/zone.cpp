/* zone.cpp
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

/** @file zone.cpp Zone Stuff */

////////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "micropolis.h"


////////////////////////////////////////////////////////////////////////



void Micropolis::doZone()
{
    bool ZonePwrFlg = setZonePower(); // Set Power Bit in Map from powerMap

    if (ZonePwrFlg) {
        poweredZoneCount++;
    } else {
        unpoweredZoneCount++;
    }

    if (curTile > PORTBASE) {       /* do Special Zones  */
        doSpecialZone(ZonePwrFlg);
        return;
    }

    if (curTile < HOSPITAL) {
        doResidential(ZonePwrFlg);
        return;
    }

    if (curTile < COMBASE) {
        doHospitalChurch();
        return;
    }

    if (curTile < INDBASE)  {
        doCommercial(ZonePwrFlg);
        return;
    }

    doIndustrial(ZonePwrFlg);

    return;
}


void Micropolis::doHospitalChurch()
{

    if (curTile == HOSPITAL) {

        hospitalPop++;

        if (!(cityTime & 15)) {
            repairZone(HOSPITAL, 3); /*post*/
        }

        if (needHospital == -1) {
            if (!getRandom(20)) {
                zonePlop(RESBASE);
            }
        }

    }

    if (curTile == CHURCH) {

        churchPop++;

        if (!(cityTime & 15)) {
            repairZone(CHURCH, 3); /*post*/
        }

        if (needChurch == -1) {
            if (!getRandom(20)) {
                zonePlop(RESBASE);
            }
        }

    }

}


#define T 1
#define F 0
#define ASCBIT (ANIMBIT | CONDBIT | BURNBIT)
#define REGBIT (CONDBIT | BURNBIT)

void Micropolis::setSmoke(int ZonePower)
{
    static short AniThis[8] = {    T,    F,    T,    T,    F,    F,    T,    T };
    static short DX1[8]     = {   -1,    0,    1,    0,    0,    0,    0,    1 };
    static short DY1[8]     = {   -1,    0,   -1,   -1,    0,    0,   -1,   -1 };
    //static short DX2[8]     = {   -1,    0,    1,    1,    0,    0,    1,    1 };
    //static short DY2[8]     = {   -1,    0,    0,   -1,    0,    0,   -1,    0 };
    static short AniTabA[8] = {    0,    0,   32,   40,    0,    0,   48,   56 };
    static short AniTabB[8] = {    0,    0,   36,   44,    0,    0,   52,   60 };
    static short AniTabC[8] = { IND1,    0, IND2, IND4,    0,    0, IND6, IND8 };
    static short AniTabD[8] = { IND1,    0, IND3, IND5,    0,    0, IND7, IND9 };
    register short z;

    if (curTile < IZB) {
        return;
    }

    z = (curTile - IZB) >>3;
    z = z & 7;

    if (AniThis[z]) {
        int xx = curMapX + DX1[z];
        int yy = curMapY + DY1[z];

        if (testBounds(xx, yy)) {

            if (ZonePower) {

                if ((map[xx][yy] & LOMASK) == AniTabC[z]) {
                    map[xx][yy] =
                        ASCBIT | (SMOKEBASE + AniTabA[z]);
                    map[xx][yy] =
                        ASCBIT | (SMOKEBASE + AniTabB[z]);
                }

            } else {

                if ((map[xx][yy] & LOMASK) > AniTabC[z]) {
                  map[xx][yy] =
                      REGBIT | AniTabC[z];
                  map[xx][yy] =
                      REGBIT | AniTabD[z];
                }

            }

        }

    }

}


void Micropolis::makeHospital()
{
    if (needHospital > 0) {
        zonePlop(HOSPITAL - 4);
        needHospital = 0;
        return;
    }

    if (needChurch > 0) {
        zonePlop(CHURCH - 4);
        needChurch = 0;
        return;
    }
}

/**
 * Compute land value at (#curMapX, #curMapY), taking pollution into account.
 * @return Indication of land-value adjusted for pollution
 *         (\c 0 => low value, \c 3 => high value)
 */
short Micropolis::getLandPollutionValue()
{
    short landVal;

    landVal =  landValueMap.worldGet(curMapX, curMapY);
    landVal -= pollutionMap.worldGet(curMapX, curMapY);

    if (landVal < 30) {
        return 0;
    }

    if (landVal < 80) {
        return 1;
    }

    if (landVal < 150) {
        return 2;
    }

    return 3;
}


void Micropolis::incRateOfGrowth(int amount)
{
    rateOfGrowthMap[curMapX>>3][curMapY>>3] += amount<<2;
}


bool Micropolis::zonePlop(int base)
{
    short z, x;
    static const short Zx[9] = {-1, 0, 1,-1, 0, 1,-1, 0, 1};
    static const short Zy[9] = {-1,-1,-1, 0, 0, 0, 1, 1, 1};

    for (z = 0; z < 9; z++) {             /* check for fire  */
        int xx = curMapX + Zx[z];
        int yy = curMapY + Zy[z];

        if (testBounds(xx, yy)) {
            x = map[xx][yy] & LOMASK;

            if ((x >= FLOOD) && (x < ROADBASE)) {
                return false;
            }

        }

    }

    for (z = 0; z < 9; z++) {
        int xx = curMapX + Zx[z];
        int yy = curMapY + Zy[z];

        if (testBounds(xx, yy)) {
            map[xx][yy] = base + BNCNBIT;
        }

        base++;
    }

    curNum = map[curMapX][curMapY];
    /// @bug: Should set Micropolis::curTile to (Micropolis::curNum & ::LOMASK),
    ///       since it is used by Micropolis::setZonePower to distinguish
    ///       nuclear and coal power plants.
    ///       Better yet, pass all parameters into Micropolis::setZonePower and
    ///       rewrite it not to use globals.
    setZonePower();
    map[curMapX][curMapY] |= ZONEBIT + BULLBIT;

    return true;
}


short Micropolis::doFreePop()
{
    short count;
    register short loc, x, y;

    count = 0;

    for (x = curMapX - 1; x <= curMapX + 1; x++) {
        for (y = curMapY - 1; y <= curMapY + 1; y++) {
            if (x >= 0 && x < WORLD_W &&
                y >= 0 && y < WORLD_H) {
                loc = map[x][y] & LOMASK;
                if ((loc >= LHTHR) &&
                    (loc <= HHTHR)) {
                    count++;
                }
            }
        }
    }

    return count;
}


/**
 * Set #PWRBIT in the map at #curMapX and #curMapY based on the corresponding
 * bit in the #powerMap.
 * @return Does the tile have power?
 */
bool Micropolis::setZonePower()
{
    if (curTile == NUCLEAR || curTile == POWERPLANT) {
        map[curMapX][curMapY] = curNum | PWRBIT;
        return true;
    }

    if (powerMap.get(curMapX, curMapY)) {
        map[curMapX][curMapY] = curNum | PWRBIT;
        return true;
    } else {
        map[curMapX][curMapY] = curNum & (~PWRBIT);
        return false;
    }
}


void Micropolis::buildHouse(int value)
{
    short z, score, hscore, BestLoc;
    static short ZeX[9] = { 0,-1, 0, 1,-1, 1,-1, 0, 1};
    static short ZeY[9] = { 0,-1,-1,-1, 0, 0, 1, 1, 1};

    BestLoc = 0;
    hscore = 0;

    for (z = 1; z < 9; z++) {
        int xx = curMapX + ZeX[z];
        int yy = curMapY + ZeY[z];

        if (testBounds(xx, yy)) {

            score = evalLot(xx, yy);

            if (score != 0) {

                if (score > hscore) {
                    hscore = score;
                    BestLoc = z;
                }

                if ((score == hscore) && !(getRandom16() & 7)) {
                    BestLoc = z;
                }

            }

        }

    }

    if (BestLoc) {
        int xx = curMapX + ZeX[BestLoc];
        int yy = curMapY + ZeY[BestLoc];

        if (testBounds(xx, yy)) {
            map[xx][yy] =
                HOUSE + BLBNCNBIT + getRandom(2) + (value * 3);
        }

    }
}


short Micropolis::evalLot(int x, int y)
{
    short z, score;
    static short DX[4] = { 0, 1, 0,-1};
    static short DY[4] = {-1, 0, 1, 0};

    /* test for clear lot */
    z = map[x][y] & LOMASK;

    if (z && ((z < RESBASE) || (z > RESBASE + 8))) {
        return -1;
    }

    score = 1;

    for (z = 0; z < 4; z++) {
        int xx = x + DX[z];
        int yy = y + DY[z];

        if (testBounds(xx, yy) &&
            (map[xx][yy] != DIRT) &&
            ((map[xx][yy] & LOMASK) <= LASTROAD)) {
            score++;          /* look for road */
        }

    }

    return score;
}

void Micropolis::doResidential(int ZonePwrFlg)
{
    short tpop, zscore, locvalve, value, TrfGood;

    resZonePop++;

    if (curTile == FREEZ) {
        tpop = doFreePop();
    } else {
        tpop = getResZonePop(curTile);
    }

    resPop += tpop;

    if (tpop > getRandom(35)) {
        /* Try driving from residential to commercial */
        TrfGood = makeTraffic(ZT_COMMERCIAL);
    } else {
        TrfGood = 1;
    }

    if (TrfGood == -1) {
        value = getLandPollutionValue();
        doResOut(tpop, value);
        return;
    }

    if ((curTile == FREEZ) || (!(getRandom16() & 7))) {

        locvalve = evalRes(TrfGood);
        zscore = resValve + locvalve;

        if (!ZonePwrFlg) {
            zscore = -500;
        }

        if ((zscore > -350) &&
            (((short)(zscore - 26380)) > ((short)getRandom16Signed()))) {

          if ((!tpop) && (!(getRandom16() & 3))) {
              makeHospital();
              return;
          }

          value = getLandPollutionValue();
          doResIn(tpop, value);

          return;
        }

        if ((zscore < 350) &&
            (((short)(zscore + 26380)) < ((short)getRandom16Signed()))) {
            value = getLandPollutionValue();
            doResOut(tpop, value);
        }

    }

}


/** Perform residential immigration into current tile. */
void Micropolis::doResIn(int pop, int value)
{
    short pollution = pollutionMap.worldGet(curMapX, curMapY);

    if (pollution > 128) {
        return;
    }

    if (curTile == FREEZ) {

        if (pop < 8) {
            buildHouse(value);
            incRateOfGrowth(1);
            return;
        }

        if (populationDensityMap.worldGet(curMapX, curMapY) > 64) {
            resPlop(0, value);
            incRateOfGrowth(8);
            return;
        }

        return;
    }

    if (pop < 40) {
        resPlop((pop / 8) - 1, value);
        incRateOfGrowth(8);
    }

}


void Micropolis::doResOut(int pop, int value)
{
    static short Brdr[9] = {0,3,6,1,4,7,2,5,8};
    register short x, y, loc, z;

    if (!pop) {
        return;
    }

    if (pop > 16) {
        resPlop(((pop - 24) / 8), value);
        incRateOfGrowth(-8);
        return;
    }

    if (pop == 16) {
        incRateOfGrowth(-8);
        map[curMapX][curMapY] = (FREEZ | BLBNCNBIT | ZONEBIT);
        for (x = curMapX - 1; x <= curMapX + 1; x++) {
            for (y = curMapY - 1; y <= curMapY + 1; y++) {
                if (x >= 0 && x < WORLD_W &&
                    y >= 0 && y < WORLD_H) {
                    if ((map[x][y] & LOMASK) != FREEZ) {
                        map[x][y] =
                            LHTHR + value + getRandom(2) + BLBNCNBIT;
                    }
                }
            }
        }
    }

    if (pop < 16) {
        incRateOfGrowth(-1);
        z = 0;
        for (x = curMapX - 1; x <= curMapX + 1; x++) {
            for (y = curMapY - 1; y <= curMapY + 1; y++) {
                if (x >= 0 && x < WORLD_W &&
                    y >= 0 && y < WORLD_H) {
                    loc = map[x][y] & LOMASK;
                    if ((loc >= LHTHR) && (loc <= HHTHR)) {
                        map[x][y] =
                            Brdr[z] + BLBNCNBIT + FREEZ - 4;
                        return;
                    }
                }
                z++;
            }
        }
    }

}


short Micropolis::getResZonePop(int Ch9)
{
    short CzDen;

    CzDen = (((Ch9 - RZB) / 9) % 4);

    return ((CzDen * 8) + 16);
}


void Micropolis::resPlop(int den, int value)
{
    short base;

    base = (((value * 4) + den) * 9) + RZB - 4;
    zonePlop(base);
}


short Micropolis::evalRes(int traf)
{
    short value;

    if (traf < 0) {
        return -3000;
    }

    value =  landValueMap.worldGet(curMapX, curMapY);
    value -= pollutionMap.worldGet(curMapX, curMapY);

    if (value < 0) {
        value = 0;          /* Cap at 0 */
    } else {
        value = min(value * 32, 6000); /* Cap at 6000 */
    }

    value = value - 3000;

    return value;
}


void Micropolis::doCommercial(int ZonePwrFlg)
{
    short tpop, TrfGood;
    short zscore, locvalve, value;

    comZonePop++;
    tpop = getComZonePop(curTile);
    comPop += tpop;

    if (tpop > getRandom(5)) {
        /* Try driving from commercial to industrial */
        TrfGood = makeTraffic(ZT_INDUSTRIAL);
    } else {
        TrfGood = 1;
    }

    if (TrfGood == -1) {
        value = getLandPollutionValue();
        doComOut(tpop, value);
        return;
    }

    if (!(getRandom16() & 7)) {

        locvalve = evalCom(TrfGood);
        zscore = comValve + locvalve;

        if (!ZonePwrFlg) {
            zscore = -500;
        }

        if (TrfGood &&
            (zscore > -350) &&
            (((short)(zscore - 26380)) > ((short)getRandom16Signed()))) {
            value = getLandPollutionValue();
            doComIn(tpop, value);
            return;
        }

        if ((zscore < 350) &&
            (((short)(zscore + 26380)) < ((short)getRandom16Signed()))) {
            value = getLandPollutionValue();
            doComOut(tpop, value);
        }

    }

}


void Micropolis::doComIn(int pop, int value)
{
    register short z;

    z = landValueMap.worldGet(curMapX, curMapY);
    z = z >>5;

    if (pop > z) {
        return;
    }

    if (pop < 5) {
        comPlop(pop, value);
        incRateOfGrowth(8);
    }
}


void Micropolis::doComOut(int pop, int value)
{
    if (pop > 1) {
        comPlop(pop - 2, value);
        incRateOfGrowth(-8);
        return;
    }

    if (pop == 1) {
        zonePlop(COMBASE);
        incRateOfGrowth(-8);
    }
}


short Micropolis::getComZonePop(int Ch9)
{
    short CzDen;

    if (Ch9 == COMCLR) {
        return (0);
    }

    CzDen = (((Ch9 - CZB) / 9) % 5) + 1;

    return (CzDen);
}


void Micropolis::comPlop(int Den, int Value)
{
    short base;

    base = (((Value * 5) + Den) * 9) + CZB - 4;
    zonePlop(base);
}


short Micropolis::evalCom(int traf)
{
    short Value;

    if (traf < 0) {
        return -3000;
    }

    Value = comRateMap[curMapX >>3][curMapY >>3];

    return Value;
}


void Micropolis::doIndustrial(int ZonePwrFlg)
{
    short tpop, zscore, TrfGood;

    indZonePop++;
    setSmoke(ZonePwrFlg);
    tpop = getIndZonePop(curTile);
    indPop += tpop;

    if (tpop > getRandom(5)) {
        /* Try driving from industrial to residential */
        TrfGood = makeTraffic(ZT_RESIDENTIAL);
    } else {
        TrfGood = 1;
    }

    if (TrfGood == -1) {
        doIndOut(tpop, getRandom16() & 1);
        return;
    }

    if (!(getRandom16() & 7)) {
        zscore = indValve + evalInd(TrfGood);

        if (!ZonePwrFlg) {
            zscore = -500;
        }

        if ((zscore > -350) &&
            (((short)(zscore - 26380)) > ((short)getRandom16Signed()))) {
            doIndIn(tpop, getRandom16() & 1);
            return;
        }

        if ((zscore < 350) &&
            (((short)(zscore + 26380)) < ((short)getRandom16Signed()))) {
            doIndOut(tpop, getRandom16() & 1);
        }

    }

}


void Micropolis::doIndIn(int pop, int value)
{
    if (pop < 4) {
        indPlop(pop, value);
        incRateOfGrowth(8);
    }
}


void Micropolis::doIndOut(int pop, int value)
{
    if (pop > 1) {
        indPlop(pop - 2, value);
        incRateOfGrowth(-8);
        return;
    }

    if (pop == 1) {
        zonePlop(INDCLR - 4);
        incRateOfGrowth(-8);
    }
}


short Micropolis::getIndZonePop(int Ch9)
{
    short CzDen;

    if (Ch9 == INDCLR) {
        return (0);
    }

    CzDen = (((Ch9 - IZB) / 9) % 4) + 1;

    return (CzDen);
}


void Micropolis::indPlop(int Den, int Value)
{
    short base;

    base = (((Value * 4) + Den) * 9) + (IZB - 4);
    zonePlop(base);
}


short Micropolis::evalInd(int traf)
{
    if (traf < 0) {
        return -1000;
    }

    return 0;
}


////////////////////////////////////////////////////////////////////////
