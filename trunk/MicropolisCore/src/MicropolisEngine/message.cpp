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


////////////////////////////////////////////////////////////////////////


/** Check progress of the user, and send him messages about it. */
void Micropolis::SendMessages()
{
    short PowerPop;
    float TM;

    // Running a scenario, and waiting it to 'end' so we can give a score
    if (ScenarioID > SC_NONE && ScoreType > SC_NONE && ScoreWait > 0) {
        ScoreWait--;
        if (ScoreWait == 0) {
            DoScenarioScore(ScoreType);
        }
    }

    CheckGrowth();

    TotalZPop = ResZPop + ComZPop + IndZPop;
    PowerPop = NuclearPop + CoalPop;

    switch(CityTime & 63) {

    case 1:
        if (TotalZPop / 4 >= ResZPop) {
            SendMes(1); /* need Res */
        }
        break;

    case 5:
        if (TotalZPop / 8 >= ComZPop) {
            SendMes(2); /* need Com */
        }
        break;

    case 10:
        if (TotalZPop / 8 >= IndZPop) {
            SendMes(3); /* need Ind */
        }
        break;

    case 14:
        if (TotalZPop > 10 && TotalZPop * 2 > RoadTotal) {
            SendMes(4);
        }
        break;

    case 18:
        if (TotalZPop > 50 && TotalZPop > RailTotal) {
            SendMes(5);
        }
        break;

    case 22:
        if (TotalZPop > 10 && PowerPop == 0) {
            SendMes(6); /* need Power */
        }
        break;

    case 26:
        if (ResPop > 500 && StadiumPop == 0) {
            SendMes(7); /* need Stad */
            ResCap = 1;
        } else {
            ResCap = 0;
        }
        break;

    case 28:
        if (IndPop > 70 && PortPop == 0) {
            SendMes(8);
            IndCap = 1;
        } else {
            IndCap = 0;
        }
        break;

    case 30:
        if (ComPop > 100 && APortPop == 0) {
            SendMes(9);
            ComCap = 1;
        } else {
            ComCap = 0;
        }
        break;

    case 32:
        TM = (float)(unPwrdZCnt + PwrdZCnt); /* dec score for unpowered zones */
        if (TM > 0) {
            if (PwrdZCnt / TM < 0.7) {
                SendMes(15);
            }
        }
        break;

    case 35:
        if (PolluteAverage > /* 80 */ 60) {
            SendMes(-10);
        }
        break;

    case 42:
        if (CrimeAverage > 100) {
            SendMes(-11);
        }
        break;

    case 45:
        if (TotalPop > 60 && FireStPop == 0) {
            SendMes(13);
        }
        break;

    case 48:
        if (TotalPop > 60 && PolicePop == 0) {
            SendMes(14);
        }
        break;

    case 51:
        if (CityTax > 12) {
            SendMes(16);
        }
        break;

    case 54:
        // If RoadEffect < 5/8 of max effect
        if (RoadEffect < (5 * MAX_ROAD_EFFECT / 8) && RoadTotal > 30) {
            SendMes(17);
        }
        break;

    case 57:
        // If FireEffect < 0.7 of max effect
        if (FireEffect < (7 * MAX_FIRESTATION_EFFECT / 10) && TotalPop > 20) {
            SendMes(18);
        }
        break;

    case 60:
        // If PoliceEffect < 0.7 of max effect
        if (PoliceEffect < (7 * MAX_POLICESTATION_EFFECT / 10)
                                                        && TotalPop > 20) {
            SendMes(19);
        }
        break;

    case 63:
        if (trafficAverage > 60) {
            SendMes(-12);
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
void Micropolis::CheckGrowth()
{
    Quad thisCityPop;
    short z;

    if ((CityTime & 3) == 0) {
        z = 0;
        thisCityPop = (ResPop + (ComPop + IndPop) * 8) * 20;

        if (lastCityPop > 0) {

            if (lastCityPop < 2000 && thisCityPop >= 2000) {
                z = 35;
            }

            if (lastCityPop < 10000 && thisCityPop >= 10000) {
                z = 36;
            }

            if (lastCityPop < 50000L && thisCityPop >= 50000L) {
                z = 37;
            }

            if (lastCityPop < 100000L && thisCityPop >= 100000L) {
                z = 38;
            }

            if (lastCityPop < 500000L && thisCityPop >= 500000L) {
                z = 39;
            }

        }

        if (z > 0 && z != LastCategory) {
                SendMes(-z);
                LastCategory = z;
        }

        lastCityPop = thisCityPop;
    }
}


/**
 * Compute score for each scenario
 * @param type Scenario used
 * @note Parameter \a type may not be \c SC_NONE
 */
void Micropolis::DoScenarioScore(Scenario type)
{
    short z = -200;     /* you lose */

    switch(type) {

    case SC_DULLSVILLE:
        if (cityClass >= CC_METROPOLIS) {
            z = -100;
        }
        break;

    case SC_SAN_FRANCISCO:
        if (cityClass >= CC_METROPOLIS) {
            z = -100;
        }
        break;

    case SC_HAMBURG:
        if (cityClass >= CC_METROPOLIS) {
            z = -100;
        }
        break;

    case SC_BERN:
        if (trafficAverage < 80) {
            z = -100;
        }
        break;

    case SC_TOKYO:
        if (cityScore > 500) {
            z = -100;
        }
        break;

    case SC_DETROIT:
        if (CrimeAverage < 60) {
            z = -100;
        }
        break;

    case SC_BOSTON:
        if (cityScore > 500) {
            z = -100;
        }
        break;

    case SC_RIO:
        if (cityScore > 500) {
            z = -100;
        }
        break;

    default:
        NOT_REACHED();
        break;

    }

    ClearMes();
    SendMes(z);

    if (z == -200) {
        DoLoseGame();
    }
}

/** Remove any pending message and picture */
void Micropolis::ClearMes()
{
    messagePort = 0;
    mesX = 0;
    mesY = 0;
    LastPicNum = 0;
}


/**
 * Setup a message for the front-end to display
 * @param mesgNum Message number of the message to display
 * @return Setup succeeded (there was no other message/picture displayed)
 */
bool Micropolis::SendMes(int mesgNum)
{
    if (mesgNum < 0) {
        if (mesgNum != LastPicNum) {
            messagePort = mesgNum;
            mesX = 0;
            mesY = 0;
            LastPicNum = mesgNum;
            return true;
        }
    } else {
        if (messagePort == 0) {
            messagePort = mesgNum;
            mesX = 0;
            mesY = 0;
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
 * @todo Merge Micropolis::SendMes() and Micropolis::SendMesAt().
 */
void Micropolis::SendMesAt(short mesgNum, short x, short y)
{
    if (SendMes(mesgNum)) {
        mesX = x;
        mesY = y;
    }
}

/**
 * Forward the message from Micropolis::messagePort to the front-end.
 *
 * Convert the message number to text and display it. Also add a sound if
 * appropiate.
 * @todo A picture (that is, a negative value in Micropolis::messagePort)
 *       causes 2 messages to be send. A picture, immediately followed by a
 *       text message. Why not do this in one step?
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
        doMakeSound((MesNum < 0) ? -MesNum : MesNum);
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

        if (MesNum > 60) {
            MesNum = 0;
            return;
        }

        GetIndString(messageStr, 301, MesNum);

        if (autoGo && (mesX != 0 || mesY != 0)) {
            DoAutoGoto(mesX, mesY, messageStr);
            mesX = 0;
            mesY = 0;
        } else {
            SetMessageField(messageStr);
        }

    } else { /* picture message */

        pictId = -(MesNum);

        if (pictId < 43) {
            GetIndString(messageStr, 301, pictId);
        } else {
            messageStr[0] = '\0';
        }

        DoShowPicture(pictId);

        messagePort = pictId; /* resend text message */

        if (autoGo && (mesX != 0 || mesY != 0)) {

            DoAutoGoto(mesX, mesY, messageStr);
            mesX = 0;
            mesY = 0;
        }
    }
}

/**
 * Make a sound for message \a mesNum if appropiate.
 * @param mesgNum Message number displayed
 */
void Micropolis::doMakeSound(int mesgNum)
{
    assert(mesgNum >= 0);

    switch (mesgNum) {
        case 12:
            if (Rand(5) == 1) {
                MakeSound("city", "HonkHonk-Med");
            } else if (Rand(5) == 1) {
                MakeSound("city", "HonkHonk-Low");
            } else if (Rand(5) == 1) {
                MakeSound("city", "HonkHonk-High");
            }
            break;

        case 11:
        case 20:
        case 22:
        case 23:
        case 24:
        case 25:
        case 26:
        case 27:
            MakeSound("city", "Siren");
            break;

        case  21:
            MakeSound("city", "Monster -speed [MonsterSpeed]");
            break;

        case 30:
            MakeSound("city", "Explosion-Low");
            MakeSound("city", "Siren");
            break;

        case  43:
            MakeSound("city", "Explosion-High");
            MakeSound("city", "Explosion-Low");
            MakeSound("city", "Siren");
            break;

        case  44:
            MakeSound("city", "Siren");
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
void Micropolis::DoAutoGoto(short x, short y, char *msg)
{
    Callback("UIAutoGoto", "dd", (int)x, (int)y);
}


/**
 * Display message to the user
 * @param str Text message
 */
void Micropolis::SetMessageField(char *str)
{
    if (!HaveLastMessage || strcmp(LastMessage, str) != 0) {
        strcpy(LastMessage, str);
        HaveLastMessage = true;

        Callback("UISetMessage", "s", str);
    }
}


/**
 * Tell the front-end to display a picture
 * @param id Identification of the picture to show
 */
void Micropolis::DoShowPicture(short id)
{
    Callback("UIShowPicture", "d", (int)id);
}


/** Tell the front-end that the player has lost the game */
void Micropolis::DoLoseGame()
{
    Callback("UILoseGame", "");
}


/** Tell the front-end that the player has won the game */
void Micropolis::DoWinGame()
{
    Callback("UIWinGame", "");
}


////////////////////////////////////////////////////////////////////////
