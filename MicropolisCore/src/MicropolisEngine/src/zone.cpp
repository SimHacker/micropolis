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


/**
 * Handle zone.
 * @param pos Position of the zone.
 */
void Micropolis::doZone(const Position &pos)
{
    // Set Power Bit in Map from powerGridMap
    bool zonePwrFlg = setZonePower(pos);

    if (zonePwrFlg) {
        poweredZoneCount++;
    } else {
        unpoweredZoneCount++;
    }

    MapTile tile = map[pos.posX][pos.posY] & LOMASK;

    if (tile > PORTBASE) {       /* do Special Zones  */
        doSpecialZone(pos, zonePwrFlg);
        return;
    }

    if (tile < HOSPITAL) {
        doResidential(pos, zonePwrFlg);
        return;
    }

    if (tile < COMBASE) {
        doHospitalChurch(pos);
        return;
    }

    if (tile < INDBASE)  {
        doCommercial(pos, zonePwrFlg);
        return;
    }

    doIndustrial(pos, zonePwrFlg);

    return;
}

/**
 * Handle repairing or removing of hospitals and churches.
 * @param pos Position of the hospital or church.
 */
void Micropolis::doHospitalChurch(const Position &pos)
{
    MapTile tile = map[pos.posX][pos.posY] & LOMASK;

    if (tile == HOSPITAL) {

        hospitalPop++;

        if (!(cityTime & 15)) {
            repairZone(pos, HOSPITAL, 3); /*post*/
        }

        if (needHospital == -1) { // Too many hospitals
            if (!getRandom(20)) {
                zonePlop(pos, RESBASE); // Remove hospital
            }
        }

    }

    if (tile == CHURCH) {

        churchPop++;

        if (!(cityTime & 15)) {
            repairZone(pos, CHURCH, 3); /*post*/
        }

        if (needChurch == -1) { // Too many churches
            if (!getRandom(20)) {
                zonePlop(pos, RESBASE); // Remove church
            }
        }

    }

}


#define ASCBIT (ANIMBIT | CONDBIT | BURNBIT)
#define REGBIT (CONDBIT | BURNBIT)

void Micropolis::setSmoke(const Position &pos, int zonePower)
{
    static bool aniThis[8] = {  true, false, true, true, false, false, true, true };
    static short dX1[8]     = {   -1,    0,    1,    0,    0,    0,    0,    1 };
    static short dY1[8]     = {   -1,    0,   -1,   -1,    0,    0,   -1,   -1 };
    //static short DX2[8]     = {   -1,    0,    1,    1,    0,    0,    1,    1 };
    //static short DY2[8]     = {   -1,    0,    0,   -1,    0,    0,   -1,    0 };
    static short aniTabA[8] = {    0,    0,   32,   40,    0,    0,   48,   56 };
    static short aniTabB[8] = {    0,    0,   36,   44,    0,    0,   52,   60 };
    static MapTile aniTabC[8] = { IND1,    0, IND2, IND4,    0,    0, IND6, IND8 };
    static MapTile aniTabD[8] = { IND1,    0, IND3, IND5,    0,    0, IND7, IND9 };

    MapTile tile = map[pos.posX][pos.posY] & LOMASK;

    if (tile < IZB) {
        return;
    }

    int z = (tile - IZB) >>3; /// @todo Why div 8? Industry is 9 tiles long!!
    z = z & 7;

    if (aniThis[z]) {
        int xx = pos.posX + dX1[z];
        int yy = pos.posY + dY1[z];

        if (testBounds(xx, yy)) {

            if (zonePower) {

                /// @todo Why do we assign the same map position twice?
                /// @todo Add #SMOKEBASE into aniTabA and aniTabB tables?
                if ((map[xx][yy] & LOMASK) == aniTabC[z]) {
                    map[xx][yy] = ASCBIT | (SMOKEBASE + aniTabA[z]);
                    map[xx][yy] = ASCBIT | (SMOKEBASE + aniTabB[z]);
                }

            } else {

                /// @todo Why do we assign the same map position twice?
                if ((map[xx][yy] & LOMASK) > aniTabC[z]) {
                  map[xx][yy] = REGBIT | aniTabC[z];
                  map[xx][yy] = REGBIT | aniTabD[z];
                }

            }

        }

    }

}


/**
 * If needed, add a new hospital or a new church.
 * @param pos Center position of the new hospital or church.
 */
void Micropolis::makeHospital(const Position &pos)
{
    if (needHospital > 0) {
        zonePlop(pos, HOSPITAL - 4);
        needHospital = 0;
        return;
    }

    if (needChurch > 0) {
        zonePlop(pos, CHURCH - 4);
        needChurch = 0;
        return;
    }
}

/**
 * Compute land value at \a pos, taking pollution into account.
 * @param pos Position of interest.
 * @return Indication of land-value adjusted for pollution
 *         (\c 0 => low value, \c 3 => high value)
 */
short Micropolis::getLandPollutionValue(const Position &pos)
{
    short landVal;

    landVal =  landValueMap.worldGet(pos.posX, pos.posY);
    landVal -= pollutionDensityMap.worldGet(pos.posX, pos.posY);

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


/**
 * Update the rate of growth at position \a pos by \a amount.
 * @param pos Position to modify.
 * @param amount Amount of change (can both be positive and negative).
 */
void Micropolis::incRateOfGrowth(const Position &pos, int amount)
{
    int value = rateOfGrowthMap.worldGet(pos.posX, pos.posY);

    value = clamp(value + amount * 4, -200, 200);
    rateOfGrowthMap.worldSet(pos.posX, pos.posY, value);
}


/**
 * Put down a 3x3 zone around the center tile at \a pos..
 * @param base Tile number of the top-left tile. @see MapTileCharacters
 * @return Build was a success.
 * @todo This function allows partial on-map construction. Is that intentional?
 */
bool Micropolis::zonePlop(const Position &pos, int base)
{
    short z, x;
    static const short Zx[9] = {-1, 0, 1,-1, 0, 1,-1, 0, 1};
    static const short Zy[9] = {-1,-1,-1, 0, 0, 0, 1, 1, 1};

    for (z = 0; z < 9; z++) {             /* check for fire  */
        int xx = pos.posX + Zx[z];
        int yy = pos.posY + Zy[z];

        if (testBounds(xx, yy)) {
            x = map[xx][yy] & LOMASK;

            if ((x >= FLOOD) && (x < ROADBASE)) {
                return false;
            }

        }

    }

    for (z = 0; z < 9; z++) {
        int xx = pos.posX + Zx[z];
        int yy = pos.posY + Zy[z];

        if (testBounds(xx, yy)) {
            map[xx][yy] = base + BNCNBIT;
        }

        base++;
    }

    setZonePower(pos);
    map[pos.posX][pos.posY] |= ZONEBIT + BULLBIT;

    return true;
}

/**
 * Count the number of single tile houses in a residential zone.
 * @param pos Position of the residential zone.
 * @return Number of single tile houses.
 */
short Micropolis::doFreePop(const Position &pos)
{
    short count = 0;

    for (short x = pos.posX - 1; x <= pos.posX + 1; x++) {
        for (short y = pos.posY - 1; y <= pos.posY + 1; y++) {
            if (x >= 0 && x < WORLD_W && y >= 0 && y < WORLD_H) {
                MapTile tile = map[x][y] & LOMASK;
                if (tile >= LHTHR && tile <= HHTHR) {
                    count++;
                }
            }
        }
    }

    return count;
}


/**
 * Copy the value of #powerGridMap at position \a pos to the map.
 * @param pos Position to copy.
 * @return Does the tile have power?
 */
bool Micropolis::setZonePower(const Position &pos)
{
    MapValue mapValue = map[pos.posX][pos.posY];
    MapTile tile = mapValue & LOMASK;

    if (tile == NUCLEAR || tile == POWERPLANT) {
        map[pos.posX][pos.posY] = mapValue | PWRBIT;
        return true;
    }

    if (powerGridMap.worldGet(pos.posX, pos.posY)) {
        map[pos.posX][pos.posY] = mapValue | PWRBIT;
        return true;
    } else {
        map[pos.posX][pos.posY] = mapValue & (~PWRBIT);
        return false;
    }
}


/**
 * Try to build a house at the zone at \a pos.
 * @param pos Center tile of the zone.
 * @param value Value to build (land value?)
 * @todo Have some form of looking around the center tile (like getFromMap())
 */
void Micropolis::buildHouse(const Position &pos, int value)
{
    short z, score, hscore, BestLoc;
    static short ZeX[9] = { 0,-1, 0, 1,-1, 1,-1, 0, 1};
    static short ZeY[9] = { 0,-1,-1,-1, 0, 0, 1, 1, 1};

    BestLoc = 0;
    hscore = 0;

    for (z = 1; z < 9; z++) {
        int xx = pos.posX + ZeX[z];
        int yy = pos.posY + ZeY[z];

        if (testBounds(xx, yy)) {

            score = evalLot(xx, yy);

            /// @bug score is never 0 !!
            if (score != 0) {

                if (score > hscore) {
                    hscore = score;
                    BestLoc = z;
                }

                /// @todo Move the code below to a better place.
                ///       If we just updated hscore above, we could
                //        trigger this code too.
                if (score == hscore && !(getRandom16() & 7)) {
                    BestLoc = z;
                }

            }

        }

    }

    if (BestLoc) {
        int xx = pos.posX + ZeX[BestLoc];
        int yy = pos.posY + ZeY[BestLoc];

        if (testBounds(xx, yy)) {
            /// @todo Is HOUSE the proper constant here?
            map[xx][yy] = HOUSE + BLBNCNBIT + getRandom(2) + value * 3;
        }

    }
}


/**
 * Evaluate suitability of the position for placing a new house.
 * @return Suitability.
 */
short Micropolis::evalLot(int x, int y)
{
    short z, score;
    static short DX[4] = { 0, 1, 0,-1};
    static short DY[4] = {-1, 0, 1, 0};

    /* test for clear lot */
    z = map[x][y] & LOMASK;

    if (z && (z < RESBASE || z > RESBASE + 8)) {
        return -1;
    }

    score = 1;

    for (z = 0; z < 4; z++) {
        int xx = x + DX[z];
        int yy = y + DY[z];

        if (testBounds(xx, yy) &&
            map[xx][yy] != DIRT && (map[xx][yy] & LOMASK) <= LASTROAD) {
            score++;          /* look for road */
        }

    }

    return score;
}

/**
 * Handle residential zone.
 * @param pos Center tile of the residential zone.
 * @param zonePwrFlg Does the zone have power?
 */
void Micropolis::doResidential(const Position &pos, int zonePwrFlg)
{
    short tpop, zscore, locvalve, value, TrfGood;

    resZonePop++;

    MapTile tile = map[pos.posX][pos.posY] & LOMASK;

    if (tile == FREEZ) {
        tpop = doFreePop(pos);
    } else {
        tpop = getResZonePop(tile);
    }

    resPop += tpop;

    if (tpop > getRandom(35)) {
        /* Try driving from residential to commercial */
        TrfGood = makeTraffic(pos, ZT_COMMERCIAL);
    } else {
        TrfGood = 1;
    }

    if (TrfGood == -1) {
        value = getLandPollutionValue(pos);
        doResOut(pos, tpop, value);
        return;
    }

    if (tile == FREEZ || !(getRandom16() & 7)) {

        locvalve = evalRes(pos, TrfGood);
        zscore = resValve + locvalve;

        if (!zonePwrFlg) {
            zscore = -500;
        }

        if (zscore > -350 &&
            ((short)(zscore - 26380) > ((short)getRandom16Signed()))) {

            if (!tpop && !(getRandom16() & 3)) {
                makeHospital(pos);
                return;
            }

            value = getLandPollutionValue(pos);
            doResIn(pos, tpop, value);
            return;
        }

        if (zscore < 350 &&
            (((short)(zscore + 26380)) < ((short)getRandom16Signed()))) {
            value = getLandPollutionValue(pos);
            doResOut(pos, tpop, value);
        }
    }
}


/**
 * Perform residential immigration into the current residential tile.
 * @param pos Position of the tile.
 * @param pop Population ?
 * @param value Land value corrected for pollution.
 */
void Micropolis::doResIn(const Position &pos, int pop, int value)
{
    short pollution = pollutionDensityMap.worldGet(pos.posX, pos.posY);

    if (pollution > 128) {
        return;
    }

    MapTile tile = map[pos.posX][pos.posY] & LOMASK;

    if (tile == FREEZ) {

        if (pop < 8) {
            buildHouse(pos, value);
            incRateOfGrowth(pos, 1);
            return;
        }

        if (populationDensityMap.worldGet(pos.posX, pos.posY) > 64) {
            resPlop(pos, 0, value);
            incRateOfGrowth(pos, 8);
            return;
        }

        return;
    }

    if (pop < 40) {
        resPlop(pos, (pop / 8) - 1, value);
        incRateOfGrowth(pos, 8);
    }

}


/**
 * Perform residential emigration from the current residential tile.
 * @param pos Position of the tile.
 * @param pop Population ?
 * @param value Land value corrected for pollution.
 */
void Micropolis::doResOut(const Position &pos, int pop, int value)
{
    static short Brdr[9] = {0,3,6,1,4,7,2,5,8};
    short x, y, loc, z;

    if (!pop) {
        return;
    }

    if (pop > 16) {
        resPlop(pos, (pop - 24) / 8, value);
        incRateOfGrowth(pos, -8);
        return;
    }

    if (pop == 16) {
        incRateOfGrowth(pos, -8);
        map[pos.posX][pos.posY] = (FREEZ | BLBNCNBIT | ZONEBIT);
        for (x = pos.posX - 1; x <= pos.posX + 1; x++) {
            for (y = pos.posY - 1; y <= pos.posY + 1; y++) {
                if (testBounds(x, y)) {
                    if ((map[x][y] & LOMASK) != FREEZ) {
                        map[x][y] = LHTHR + value + getRandom(2) + BLBNCNBIT;
                    }
                }
            }
        }
    }

    if (pop < 16) {
        incRateOfGrowth(pos, -1);
        z = 0;
        for (x = pos.posX - 1; x <= pos.posX + 1; x++) {
            for (y = pos.posY - 1; y <= pos.posY + 1; y++) {
                if (testBounds(x, y)) {
                    loc = map[x][y] & LOMASK;
                    if ((loc >= LHTHR) && (loc <= HHTHR)) {
                        map[x][y] = Brdr[z] + BLBNCNBIT + FREEZ - 4;
                        return;
                    }
                }
                z++;
            }
        }
    }
}


/**
 * Return population of a residential zone center tile
 * (RZB, RZB+9, ..., HOSPITAL - 9).
 *
 * @param mapTile Center tile of a residential zone.
 * @return Population of the residential zone.
 *         (16, 24, 32, 40, 16, ..., 40 )
 */
short Micropolis::getResZonePop(MapTile mapTile)
{
    short CzDen = ((mapTile - RZB) / 9) % 4;

    return CzDen * 8 + 16;
}

/**
 * Put down a residential zone.
 * @param pos Center tile of the residential zone.
 * @param den Population density (0..3)
 * @param value Land value - pollution (0..3), higher is better.
 */
void Micropolis::resPlop(const Position &pos, int den, int value)
{
    short base;

    base = ((value * 4 + den) * 9) + RZB - 4;
    zonePlop(pos, base);
}


/**
 * Evaluate residential zone.
 */
short Micropolis::evalRes(const Position &pos, int traf)
{
    short value;

    if (traf < 0) {
        return -3000;
    }

    value =  landValueMap.worldGet(pos.posX, pos.posY);
    value -= pollutionDensityMap.worldGet(pos.posX, pos.posY);

    if (value < 0) {
        value = 0;          /* Cap at 0 */
    } else {
        value = min(value * 32, 6000); /* Cap at 6000 */
    }

    value = value - 3000;

    return value;
}


/**
 * Handle commercial zone.
 * @param pos        Position of the commercial zone.
 * @param zonePwrFlg Does the zone have power?
 * @todo Make zonePwrFlg a boolean.
 */
void Micropolis::doCommercial(const Position &pos, int zonePwrFlg)
{
    short tpop, TrfGood;
    short zscore, locvalve, value;

    MapTile tile = map[pos.posX][pos.posY] & LOMASK;

    comZonePop++;
    tpop = getComZonePop(tile);
    comPop += tpop;

    if (tpop > getRandom(5)) {
        /* Try driving from commercial to industrial */
        TrfGood = makeTraffic(pos, ZT_INDUSTRIAL);
    } else {
        TrfGood = 1;
    }

    if (TrfGood == -1) {
        value = getLandPollutionValue(pos);
        doComOut(pos, tpop, value);
        return;
    }

    if (!(getRandom16() & 7)) {

        locvalve = evalCom(pos, TrfGood);
        zscore = comValve + locvalve;

        if (!zonePwrFlg) {
            zscore = -500;
        }

        if (TrfGood &&
            (zscore > -350) &&
            (((short)(zscore - 26380)) > ((short)getRandom16Signed()))) {
            value = getLandPollutionValue(pos);
            doComIn(pos, tpop, value);
            return;
        }

        if ((zscore < 350) &&
            (((short)(zscore + 26380)) < ((short)getRandom16Signed()))) {
            value = getLandPollutionValue(pos);
            doComOut(pos, tpop, value);
        }

    }

}


/**
 * Handle immigration of commercial zone.
 * @param pos Position of the commercial zone.
 * @param pop Population ?
 * @param value Land value corrected for pollution.
 */
void Micropolis::doComIn(const Position &pos, int pop, int value)
{
    short z;

    z = landValueMap.worldGet(pos.posX, pos.posY);
    z = z >>5;

    if (pop > z) {
        return;
    }

    if (pop < 5) {
        comPlop(pos, pop, value);
        incRateOfGrowth(pos, 8);
    }
}

/**
 * Handle emigration of commercial zone.
 * @param pos Position of the commercial zone.
 * @param pop Population ?
 * @param value Land value corrected for pollution.
 */
void Micropolis::doComOut(const Position &pos, int pop, int value)
{
    if (pop > 1) {
        comPlop(pos, pop - 2, value);
        incRateOfGrowth(pos, -8);
        return;
    }

    if (pop == 1) {
        zonePlop(pos, COMBASE);
        incRateOfGrowth(pos, -8);
    }
}


/**
 * Get commercial zone population number.
 * @param tile Tile of the commercial zone.
 * @return Population number of the zone.
 */
short Micropolis::getComZonePop(MapTile tile)
{
    if (tile == COMCLR) {
        return 0;
    }

    short CzDen = ((tile - CZB) / 9) % 5 + 1;
    return CzDen;
}


/**
 * Build a commercial zone.
 * @param pos Position of the commercial zone.
 * @param Den Density
 * @param Value Land value corrected for pollution.
 */
void Micropolis::comPlop(const Position &pos, int Den, int Value)
{
    short base;

    base = ((Value * 5) + Den) * 9 + CZB - 4;
    zonePlop(pos, base);
}


/**
 * Compute evaluation of a commercial zone.
 * @param traf Result if traffic attempt.
 * @return Evaluation value of the commercial zone.
 */
short Micropolis::evalCom(const Position &pos, int traf)
{
    short Value;

    if (traf < 0) {
        return -3000;
    }

    Value = comRateMap.worldGet(pos.posX, pos.posY);

    return Value;
}


/**
 * Handle industrial zone.
 * @param pos Position of the industrial zone.
 * @param zonePwrFlg Does the zone have power?
 * @todo Make zonePwrFlg a boolean.
 */
void Micropolis::doIndustrial(const Position &pos, int zonePwrFlg)
{
    short tpop, zscore, TrfGood;

    MapTile tile = map[pos.posX][pos.posY] & LOMASK;

    indZonePop++;
    setSmoke(pos, zonePwrFlg);
    tpop = getIndZonePop(tile);
    indPop += tpop;

    if (tpop > getRandom(5)) {
        /* Try driving from industrial to residential */
        TrfGood = makeTraffic(pos, ZT_RESIDENTIAL);
    } else {
        TrfGood = 1;
    }

    if (TrfGood == -1) {
        doIndOut(pos, tpop, getRandom16() & 1);
        return;
    }

    if (!(getRandom16() & 7)) {
        zscore = indValve + evalInd(TrfGood);

        if (!zonePwrFlg) {
            zscore = -500;
        }

        if (zscore > -350 &&
            (((short)(zscore - 26380)) > ((short)getRandom16Signed()))) {
            doIndIn(pos, tpop, getRandom16() & 1);
            return;
        }

        if (zscore < 350 &&
            (((short)(zscore + 26380)) < ((short)getRandom16Signed()))) {
            doIndOut(pos, tpop, getRandom16() & 1);
        }
    }
}


/**
 * Handle immigration of industrial zone.
 * @param pos   Position of the center tile of the industrial tile.
 * @param pos   Population value of the industrial zone.
 * @param value Random land value (it seems).
 */
void Micropolis::doIndIn(const Position &pos, int pop, int value)
{
    if (pop < 4) {
        indPlop(pos, pop, value);
        incRateOfGrowth(pos, 8);
    }
}

/**
 * Handle industrial zone emigration.
 * @param pos   Position of the center tile of the industrial tile.
 * @param pos   Population value of the industrial zone.
 * @param value Random land value (it seems).
 */
void Micropolis::doIndOut(const Position &pos, int pop, int value)
{
    if (pop > 1) {
        indPlop(pos, pop - 2, value);
        incRateOfGrowth(pos, -8);
        return;
    }

    if (pop == 1) {
        zonePlop(pos, INDBASE); // empty industrial zone
        incRateOfGrowth(pos, -8);
    }
}


/**
 * Get the population value for the given industrial tile.
 * @param tile Center tile value of the industrial zone.
 * @return Population value.
 */
short Micropolis::getIndZonePop(MapTile tile)
{
    if (tile == INDCLR) {
        return 0;
    }

    short CzDen = (((tile - IZB) / 9) % 4) + 1;
    return CzDen;
}

/**
 * Place an industrial zone around center tile \a pos.
 * @param pos   Center of the industrial zone.
 * @param den   Population density of the industrial zone (0, 1, 2, or 3).
 * @param value Landvalue of the industrial zone (0 or 1).
 */
void Micropolis::indPlop(const Position &pos, int den, int value)
{
    short base = ((value * 4) + den) * 9 + IND1;
    zonePlop(pos, base);
}


/**
 * Compute evaluation of an industrial zone.
 * @param traf Result if traffic attempt.
 * @return Evaluation value of the industrial zone.
 */
short Micropolis::evalInd(int traf)
{
    if (traf < 0) {
        return -1000;
    }

    return 0;
}


////////////////////////////////////////////////////////////////////////
