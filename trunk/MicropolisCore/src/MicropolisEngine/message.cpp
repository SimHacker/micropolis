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
#include "text.h"


////////////////////////////////////////////////////////////////////////


/** Check progress of the user, and send him messages about it. */
void Micropolis::sendMessages()
{
    short PowerPop;
    float TM;

    // Running a scenario, and waiting it to 'end' so we can give a score
    if (ScenarioID > SC_NONE && ScoreType > SC_NONE && ScoreWait > 0) {
        ScoreWait--;
        if (ScoreWait == 0) {
            doScenarioScore(ScoreType);
        }
    }

    checkGrowth();

    totalZonePop = resZonePop + comZonePop + indZonePop;
    PowerPop = nuclearPowerPop + coalPowerPop;

    switch (cityTime & 63) {

    case 1:
        if (totalZonePop / 4 >= resZonePop) {
            sendMessage(STR301_NEED_MORE_RESIDENTIAL);
        }
        break;

    case 5:
        if (totalZonePop / 8 >= comZonePop) {
            sendMessage(STR301_NEED_MORE_COMMERCIAL);
        }
        break;

    case 10:
        if (totalZonePop / 8 >= indZonePop) {
            sendMessage(STR301_NEED_MORE_INDUSTRIAL);
        }
        break;

    case 14:
        if (totalZonePop > 10 && totalZonePop * 2 > roadTotal) {
            sendMessage(STR301_NEED_MORE_ROADS);
        }
        break;

    case 18:
        if (totalZonePop > 50 && totalZonePop > railTotal) {
            sendMessage(STR301_NEED_MORE_RAILS);
        }
        break;

    case 22:
        if (totalZonePop > 10 && PowerPop == 0) {
            sendMessage(STR301_NEED_ELECTRICITY);
        }
        break;

    case 26:
        if (resPop > 500 && stadiumPop == 0) {
            sendMessage(STR301_NEED_STADIUM);
            ResCap = 1;
        } else {
            ResCap = 0;
        }
        break;

    case 28:
        if (indPop > 70 && seaportPop == 0) {
            sendMessage(STR301_NEED_SEAPORT);
            IndCap = 1;
        } else {
            IndCap = 0;
        }
        break;

    case 30:
        if (comPop > 100 && airportPop == 0) {
            sendMessage(STR301_NEED_AIRPORT);
            ComCap = 1;
        } else {
            ComCap = 0;
        }
        break;

    case 32:
        TM = (float)(unPwrdZCnt + PwrdZCnt); /* dec score for unpowered zones */
        if (TM > 0) {
            if (PwrdZCnt / TM < 0.7) {
                sendMessage(STR301_BLACKOUTS_REPORTED);
            }
        }
        break;

    case 35:
        if (pollutionAverage > /* 80 */ 60) {
            sendMessage(-STR301_HIGH_POLLUTION);
        }
        break;

    case 42:
        if (crimeAverage > 100) {
            sendMessage(-STR301_HIGH_CRIME);
        }
        break;

    case 45:
        if (totalPop > 60 && fireStationPop == 0) {
            sendMessage(STR301_NEED_FIRE_STATION);
        }
        break;

    case 48:
        if (totalPop > 60 && policeStationPop == 0) {
            sendMessage(STR301_NEED_POLICE_STATION);
        }
        break;

    case 51:
        if (cityTax > 12) {
            sendMessage(STR301_TAX_TOO_HIGH);
        }
        break;

    case 54:
        // If roadEffect < 5/8 of max effect
        if (roadEffect < (5 * MAX_ROAD_EFFECT / 8) && roadTotal > 30) {
            sendMessage(STR301_ROAD_NEEDS_FUNDING);
        }
        break;

    case 57:
        // If fireEffect < 0.7 of max effect
        if (fireEffect < (7 * MAX_FIRESTATION_EFFECT / 10) && totalPop > 20) {
            sendMessage(STR301_FIRE_STATION_NEEDS_FUNDING);
        }
        break;

    case 60:
        // If policeEffect < 0.7 of max effect
        if (policeEffect < (7 * MAX_POLICESTATION_EFFECT / 10)
                                                        && totalPop > 20) {
            sendMessage(STR301_POLICE_NEEDS_FUNDING);
        }
        break;

    case 63:
        if (trafficAverage > 60) {
            sendMessage(-STR301_TRAFFIC_JAMS);
        }
        break;

    }
}


/**
 * Detect a change in city class, and produce a message if the player has
 * reached the next class.
 * @todo Replace magic population numbers with constants in a table.
 * @todo City class detection seems duplicated. Find other instances, and merge
 *       them to a single function.
 */
void Micropolis::checkGrowth()
{
    Quad thisCityPop;
    short z;

    if ((cityTime & 3) == 0) {
        z = 0;
        thisCityPop = (resPop + (comPop + indPop) * 8) * 20;

        if (lastCityPop > 0) {

            if (lastCityPop < 2000 && thisCityPop >= 2000) {
                z = STR301_REACHED_TOWN;
            }

            if (lastCityPop < 10000 && thisCityPop >= 10000) {
                z = STR301_REACHED_CITY;
            }

            if (lastCityPop < 50000L && thisCityPop >= 50000L) {
                z = STR301_REACHED_CAPITAL;
            }

            if (lastCityPop < 100000L && thisCityPop >= 100000L) {
                z = STR301_REACHED_METROPOLIS;
            }

            if (lastCityPop < 500000L && thisCityPop >= 500000L) {
                z = STR301_REACHED_MEGALOPOLIS;
            }

        }

        if (z > 0 && z != lastCategory) {
            sendMessage(-z);
            lastCategory = z;
        }

        lastCityPop = thisCityPop;
    }
}


/**
 * Compute score for each scenario
 * @param type Scenario used
 * @note Parameter \a type may not be \c SC_NONE
 * @bug Messages #STR301_SCENARIO_LOST and #STR301_SCENARIO_WON are handled
 *      special (they are larger than #STR301_LAST). Fix this.
 */
void Micropolis::doScenarioScore(Scenario type)
{
    short z = -STR301_SCENARIO_LOST;     /* you lose */

    switch (type) {

    case SC_DULLSVILLE:
        if (cityClass >= CC_METROPOLIS) {
            z = -STR301_SCENARIO_WON;
        }
        break;

    case SC_SAN_FRANCISCO:
        if (cityClass >= CC_METROPOLIS) {
            z = -STR301_SCENARIO_WON;
        }
        break;

    case SC_HAMBURG:
        if (cityClass >= CC_METROPOLIS) {
            z = -STR301_SCENARIO_WON;
        }
        break;

    case SC_BERN:
        if (trafficAverage < 80) {
            z = -STR301_SCENARIO_WON;
        }
        break;

    case SC_TOKYO:
        if (cityScore > 500) {
            z = -STR301_SCENARIO_WON;
        }
        break;

    case SC_DETROIT:
        if (crimeAverage < 60) {
            z = -STR301_SCENARIO_WON;
        }
        break;

    case SC_BOSTON:
        if (cityScore > 500) {
            z = -STR301_SCENARIO_WON;
        }
        break;

    case SC_RIO:
        if (cityScore > 500) {
            z = -STR301_SCENARIO_WON;
        }
        break;

    default:
        NOT_REACHED();
        break;

    }

    clearMessage();
    sendMessage(z);

    if (z == -STR301_SCENARIO_LOST) {
        doLoseGame();
    }
}

/** Remove any pending message and picture */
void Micropolis::clearMessage()
{
    messagePort = 0;
    messageX = 0;
    messageY = 0;
    messagePictureLast = 0;
}


/**
 * Setup a message for the front-end to display
 * @param mesgNum Message number of the message to display
 * @return Setup succeeded (there was no other message/picture displayed)
 */
bool Micropolis::sendMessage(int mesgNum)
{
    if (mesgNum < 0) {
        if (mesgNum != messagePictureLast) {
            messagePort = mesgNum;
            messageX = -1;
            messageY = -1;
            messagePictureLast = mesgNum;
            return true;
        }
    } else {
        if (messagePort == 0) {
            messagePort = mesgNum;
            messageX = 0;
            messageY = 0;
            return true;
        }
    }

    return false;
}


/**
 * Send the user a message of an event that happens at a particular position
 * in the city.
 * @param mesgNum Message number of the message to display.
 * @param x       X coordinate of the position of the event.
 * @param y       Y coordinate of the position of the event.
 * @todo Merge Micropolis::sendMessage() and Micropolis::sendMessageAt().
 */
void Micropolis::sendMessageAt(short mesgNum, short x, short y)
{
    if (sendMessage(mesgNum)) {
        messageX = x;
        messageY = y;
    }
}

/**
 * Forward the message from Micropolis::messagePort to the front-end.
 *
 * Convert the message number to text and display it. Also add a sound if
 * appropriate.
 * @todo A picture (that is, a negative value in Micropolis::messagePort)
 *       causes 2 messages to be send. A picture, immediately followed by a
 *       text message. Why not do this in one step?
 * @bug Last valid message is #STR301_LOADED_SAVED_CITY, which is much less
 *      than #STR301_LAST. Close this gap.
 * @todo Document meaning of \c -1 and \c 0 values of #messageX and #messageY.
 */
void Micropolis::doMessage()
{
    char messageStr[256];
    short pictId;

    messageStr[0] = 0;

    if (messagePort != 0) {
        MesNum = messagePort;
        messagePort = 0;
        LastMesTime = TickCount();
        doMakeSound((MesNum < 0) ? -MesNum : MesNum, messageX, messageY);
    } else {
        if (MesNum == 0) {
            return;
        }
        if (MesNum < 0) {
            MesNum = -MesNum;
            LastMesTime = TickCount();
        } else if (TickCount() - LastMesTime > 60 * 30) {
            MesNum = 0;
            return;
        }
    }


    if (MesNum >= 0) {
        if (MesNum == 0) {
            return;
        }

        if (MesNum > STR301_LAST) {
            MesNum = 0;
            return;
        }

        getIndString(messageStr, 301, MesNum);

        if (autoGoto && messageX != -1 && messageY != -1) {
            doAutoGoto(messageX, messageY, messageStr);
            messageX = -1;
            messageY = -1;
        } else {
            setMessageField(messageStr);
        }

    } else { /* picture message */

        pictId = -MesNum;

        if (pictId < 43) {
            getIndString(messageStr, 301, pictId);
        } else {
            messageStr[0] = '\0';
        }

        doShowPicture(pictId);

        messagePort = pictId; /* resend text message */

        if (autoGoto && messageX != -1 && messageY != -1) {

            doAutoGoto(messageX, messageY, messageStr);
            messageX = 0;
            messageY = 0;
        }
    }
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

        case STR301_TRAFFIC_JAMS:
            if (getRandom(5) == 1) {
                MakeSound("city", "HonkHonk-Med", x, y);
            } else if (getRandom(5) == 1) {
                MakeSound("city", "HonkHonk-Low", x, y);
            } else if (getRandom(5) == 1) {
                MakeSound("city", "HonkHonk-High", x, y);
            }
            break;

        case STR301_HIGH_CRIME:
        case STR301_FIRE_REPORTED:
        case STR301_TORNADO_SIGHTED:
        case STR301_EARTHQUAKE:
        case STR301_PLANE_CRASHED:
        case STR301_SHIP_CRASHED:
        case STR301_TRAIN_CRASHED:
        case STR301_HELICOPTER_CRASHED:
            MakeSound("city", "Siren", x, y);
            break;

        case  STR301_MONSTER_SIGHTED:
            MakeSound("city", "Monster", x, y);
            break;

        case STR301_FIREBOMBING:
            MakeSound("city", "Explosion-Low", x, y);
            MakeSound("city", "Siren", x, y);
            break;

        case STR301_NUCLEAR_MELTDOWN:
            MakeSound("city", "Explosion-High", x, y);
            MakeSound("city", "Explosion-Low", x, y);
            MakeSound("city", "Siren", x, y);
            break;

        case STR301_RIOTS_REPORTED:
            MakeSound("city", "Siren", x, y);
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
    Callback("UIAutoGoto", "dd", (int)x, (int)y);
}


/**
 * Display message to the user
 * @param str Text message
 */
void Micropolis::setMessageField(char *str)
{
    if (!messageLastValid || strcmp(messageLast, str) != 0) {
        strcpy(messageLast, str);
        messageLastValid = true;

        Callback("UISetMessage", "s", str);
    }
}


/**
 * Tell the front-end to display a picture
 * @param id Identification of the picture to show
 */
void Micropolis::doShowPicture(short id)
{
    Callback("UIShowPicture", "d", (int)id);
}


/** Tell the front-end that the player has lost the game */
void Micropolis::doLoseGame()
{
    Callback("UILoseGame", "");
}


/** Tell the front-end that the player has won the game */
void Micropolis::doWinGame()
{
    Callback("UIWinGame", "");
}


////////////////////////////////////////////////////////////////////////
