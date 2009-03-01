/* message.cpp
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

/** @file message.cpp */

////////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "micropolis.h"
#include "text.h"


////////////////////////////////////////////////////////////////////////


/** Check progress of the user, and send him messages about it. */
void Micropolis::sendMessages()
{
    short PowerPop;
    float TM;

    // Running a scenario, and waiting it to 'end' so we can give a score
    if (scenario > SC_NONE && scoreType > SC_NONE && scoreWait > 0) {
        scoreWait--;
        if (scoreWait == 0) {
            doScenarioScore(scoreType);
        }
    }

    checkGrowth();

    totalZonePop = resZonePop + comZonePop + indZonePop;
    PowerPop = nuclearPowerPop + coalPowerPop;

    switch (cityTime & 63) {

    case 1:
        if (totalZonePop / 4 >= resZonePop) {
            sendMessage(MESSAGE_NEED_MORE_RESIDENTIAL);
        }
        break;

    case 5:
        if (totalZonePop / 8 >= comZonePop) {
            sendMessage(MESSAGE_NEED_MORE_COMMERCIAL);
        }
        break;

    case 10:
        if (totalZonePop / 8 >= indZonePop) {
            sendMessage(MESSAGE_NEED_MORE_INDUSTRIAL);
        }
        break;

    case 14:
        if (totalZonePop > 10 && totalZonePop * 2 > roadTotal) {
            sendMessage(MESSAGE_NEED_MORE_ROADS);
        }
        break;

    case 18:
        if (totalZonePop > 50 && totalZonePop > railTotal) {
            sendMessage(MESSAGE_NEED_MORE_RAILS);
        }
        break;

    case 22:
        if (totalZonePop > 10 && PowerPop == 0) {
            sendMessage(MESSAGE_NEED_ELECTRICITY);
        }
        break;

    case 26:
        if (resPop > 500 && stadiumPop == 0) {
            sendMessage(MESSAGE_NEED_STADIUM);
            resCap = true;
        } else {
            resCap = false;
        }
        break;

    case 28:
        if (indPop > 70 && seaportPop == 0) {
            sendMessage(MESSAGE_NEED_SEAPORT);
            indCap = true;
        } else {
            indCap = false;
        }
        break;

    case 30:
        if (comPop > 100 && airportPop == 0) {
            sendMessage(MESSAGE_NEED_AIRPORT);
            comCap = true;
        } else {
            comCap = false;
        }
        break;

    case 32:
        TM = (float)(unpoweredZoneCount + poweredZoneCount); /* dec score for unpowered zones */
        if (TM > 0) {
            if (poweredZoneCount / TM < 0.7) {
                sendMessage(MESSAGE_BLACKOUTS_REPORTED);
            }
        }
        break;

    case 35:
        if (pollutionAverage > /* 80 */ 60) {
            sendMessage(MESSAGE_HIGH_POLLUTION, -1, -1, true);
        }
        break;

    case 42:
        if (crimeAverage > 100) {
            sendMessage(MESSAGE_HIGH_CRIME, -1, -1, true);
        }
        break;

    case 45:
        if (totalPop > 60 && fireStationPop == 0) {
            sendMessage(MESSAGE_NEED_FIRE_STATION);
        }
        break;

    case 48:
        if (totalPop > 60 && policeStationPop == 0) {
            sendMessage(MESSAGE_NEED_POLICE_STATION);
        }
        break;

    case 51:
        if (cityTax > 12) {
            sendMessage(MESSAGE_TAX_TOO_HIGH);
        }
        break;

    case 54:
        // If roadEffect < 5/8 of max effect
        if (roadEffect < (5 * MAX_ROAD_EFFECT / 8) && roadTotal > 30) {
            sendMessage(MESSAGE_ROAD_NEEDS_FUNDING);
        }
        break;

    case 57:
        // If fireEffect < 0.7 of max effect
        if (fireEffect < (7 * MAX_FIRE_STATION_EFFECT / 10) && totalPop > 20) {
            sendMessage(MESSAGE_FIRE_STATION_NEEDS_FUNDING);
        }
        break;

    case 60:
        // If policeEffect < 0.7 of max effect
        if (policeEffect < (7 * MAX_POLICE_STATION_EFFECT / 10)
                                                        && totalPop > 20) {
            sendMessage(MESSAGE_POLICE_NEEDS_FUNDING);
        }
        break;

    case 63:
        if (trafficAverage > 60) {
            sendMessage(MESSAGE_TRAFFIC_JAMS, -1, -1, true);
        }
        break;

    }
}


/**
 * Detect a change in city class, and produce a message if the player has
 * reached the next class.
 * @todo This code is very closely related to Micropolis::doPopNum().
 *       Maybe merge both in some way?
 *       (This function gets called much more often however then doPopNum().
 *        Also, at the first call, the difference between thisCityPop and
 *        cityPop is huge.)
 */
void Micropolis::checkGrowth()
{
    if ((cityTime & 3) == 0) {
        short category = 0;
        Quad thisCityPop = getPopulation();

        if (cityPopLast > 0) {

            CityClass lastClass = getCityClass(cityPopLast);
            CityClass newClass = getCityClass(thisCityPop);

            if (lastClass != newClass) {

                // Switched class, find appropiate message.
                switch (newClass) {
                    case CC_TOWN:
                        category = MESSAGE_REACHED_TOWN;
                        break;

                    case CC_CITY:
                        category = MESSAGE_REACHED_CITY;
                        break;

                    case CC_CAPITAL:
                        category = MESSAGE_REACHED_CAPITAL;
                        break;

                    case CC_METROPOLIS:
                        category = MESSAGE_REACHED_METROPOLIS;
                        break;

                    case CC_MEGALOPOLIS:
                        category = MESSAGE_REACHED_MEGALOPOLIS;
                        break;

                    default:
                        NOT_REACHED();
                        break;
                }
            }
        }

        if (category > 0 && category != categoryLast) {
            sendMessage(category, NOWHERE, NOWHERE, true);
            categoryLast = category;
        }

        cityPopLast = thisCityPop;
    }
}


/**
 * Compute score for each scenario
 * @param type Scenario used
 * @note Parameter \a type may not be \c SC_NONE
 */
void Micropolis::doScenarioScore(Scenario type)
{
    short z = MESSAGE_SCENARIO_LOST;     /* you lose */

    switch (type) {

    case SC_DULLSVILLE:
        if (cityClass >= CC_METROPOLIS) {
            z = MESSAGE_SCENARIO_WON;
        }
        break;

    case SC_SAN_FRANCISCO:
        if (cityClass >= CC_METROPOLIS) {
            z = MESSAGE_SCENARIO_WON;
        }
        break;

    case SC_HAMBURG:
        if (cityClass >= CC_METROPOLIS) {
            z = MESSAGE_SCENARIO_WON;
        }
        break;

    case SC_BERN:
        if (trafficAverage < 80) {
            z = MESSAGE_SCENARIO_WON;
        }
        break;

    case SC_TOKYO:
        if (cityScore > 500) {
            z = MESSAGE_SCENARIO_WON;
        }
        break;

    case SC_DETROIT:
        if (crimeAverage < 60) {
            z = MESSAGE_SCENARIO_WON;
        }
        break;

    case SC_BOSTON:
        if (cityScore > 500) {
            z = MESSAGE_SCENARIO_WON;
        }
        break;

    case SC_RIO:
        if (cityScore > 500) {
            z = MESSAGE_SCENARIO_WON;
        }
        break;

    default:
        NOT_REACHED();
        break;

    }

    sendMessage(z, NOWHERE, NOWHERE, true, true);

    if (z == MESSAGE_SCENARIO_LOST) {
        doLoseGame();
    }
}


/**
 * Send the user a message of an event that happens at a particular position
 * in the city.
 * @param mesgNum Message number of the message to display.
 * @param x          X coordinate of the position of the event.
 * @param y          Y coordinate of the position of the event.
 * @param picture    Flag that is true if a picture should be shown.
 * @param important  Flag that is true if the message is important.
 */
void Micropolis::sendMessage(short mesgNum, short x, short y, bool picture, bool important)
{
    callback( "UIUpdate", "sdddbb", "message",
        (int)mesgNum,
        (int)x,
        (int)y,
        picture ? 1 : 0,
        important ? 1 : 0);
}


/**
 * Make a sound for message \a mesgNum if appropriate.
 * @param mesgNum Message number displayed.
 * @param x       Horizontal coordinate in the city of the sound.
 * @param y       Vertical coordinate in the city of the sound.
 */
void Micropolis::doMakeSound(int mesgNum, int x, int y)
{
    assert(mesgNum >= 0);

    switch (mesgNum) {

        case MESSAGE_TRAFFIC_JAMS:
            if (getRandom(5) == 1) {
                makeSound("city", "HonkHonk-Med", x, y);
            } else if (getRandom(5) == 1) {
                makeSound("city", "HonkHonk-Low", x, y);
            } else if (getRandom(5) == 1) {
                makeSound("city", "HonkHonk-High", x, y);
            }
            break;

        case MESSAGE_HIGH_CRIME:
        case MESSAGE_FIRE_REPORTED:
        case MESSAGE_TORNADO_SIGHTED:
        case MESSAGE_EARTHQUAKE:
        case MESSAGE_PLANE_CRASHED:
        case MESSAGE_SHIP_CRASHED:
        case MESSAGE_TRAIN_CRASHED:
        case MESSAGE_HELICOPTER_CRASHED:
            makeSound("city", "Siren", x, y);
            break;

        case  MESSAGE_MONSTER_SIGHTED:
            makeSound("city", "Monster", x, y);
            break;

        case MESSAGE_FIREBOMBING:
            makeSound("city", "Explosion-Low", x, y);
            makeSound("city", "Siren", x, y);
            break;

        case MESSAGE_NUCLEAR_MELTDOWN:
            makeSound("city", "Explosion-High", x, y);
            makeSound("city", "Explosion-Low", x, y);
            makeSound("city", "Siren", x, y);
            break;

        case MESSAGE_RIOTS_REPORTED:
            makeSound("city", "Siren", x, y);
            break;

    }
}

/**
 * Tell the front-end that it should perform an auto-goto
 * @param x   X position at the map
 * @param y   Y position at the map
 * @param msg Message
 * @todo \a msg parameter is not used!
 */
void Micropolis::doAutoGoto(short x, short y, char *msg)
{
    callback("UIAutoGoto", "dd", (int)x, (int)y);
}


/** Tell the front-end that the player has lost the game */
void Micropolis::doLoseGame()
{
    callback("UILoseGame", "");
}


/** Tell the front-end that the player has won the game */
void Micropolis::doWinGame()
{
    callback("UIWinGame", "");
}


////////////////////////////////////////////////////////////////////////
