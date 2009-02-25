/* utilities.cpp
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

/** @file utilities.cpp */

////////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "micropolis.h"


////////////////////////////////////////////////////////////////////////


/* comefrom: drawTaxesCollected incBoxValue decBoxValue drawCurrentFunds
             drawActualBox updateFunds updateCurrentCost */
void Micropolis::makeDollarDecimalStr(char *numStr, char *dollarStr)
{
    register short leftMostSet;
    short numOfDigits;
    short numOfChars;
    register short numOfCommas;
    register short dollarIndex = 0;
    register short numIndex = 0;
    register short x;

    numOfDigits = (short)strlen(numStr);

    if (numOfDigits == 1) {
        dollarStr[0] = '$';
        dollarStr[1] = numStr[ 0 ];
        dollarStr[2] = 0;
        return;
    } else if (numOfDigits == 2) {
        dollarStr[0] = '$';
        dollarStr[1] = numStr[0];
        dollarStr[2] = numStr[1];
        dollarStr[3] = 0;
        return;
    } else if (numOfDigits == 3) {
        dollarStr[0] = '$';
        dollarStr[1] = numStr[0];
        dollarStr[2] = numStr[1];
        dollarStr[3] = numStr[2];
        dollarStr[4] = 0;
    } else {
        leftMostSet = numOfDigits % 3;

        if (leftMostSet == 0) {
            leftMostSet = 3;
        }

        numOfCommas = (numOfDigits - 1) / 3;

        /* add 1 for the dollar sign */
        numOfChars = numOfDigits + numOfCommas + 1;

        dollarStr[numOfChars] = 0;

        dollarStr[dollarIndex++] = '$';

        for (x = 1; x <= leftMostSet; x++) {
            dollarStr[dollarIndex++] = numStr[numIndex++];
        }

        for (x = 1; x <= numOfCommas; x++) {
            dollarStr[dollarIndex++] = ',';
            dollarStr[dollarIndex++] = numStr[numIndex++];
            dollarStr[dollarIndex++] = numStr[numIndex++];
            dollarStr[dollarIndex++] = numStr[numIndex++];
        }

    }
}

/**
 * Pause a simulation
 * @see resume
 */
void Micropolis::pause()
{
    if (!simPaused) {
        simPausedSpeed = simSpeedMeta;
        setSpeed(0);
        simPaused = true;
    }

    // Call back even if the state did not change.
    callback("UIUpdate", "s", "paused");
}

/**
 * Resume simulation after pausing it
 * @see pause
 */
void Micropolis::resume()
{
    if (simPaused) {
        simPaused = false;
        setSpeed(simPausedSpeed);
    }

    // Call back even if the state did not change.
    callback("UIUpdate", "s", "paused");
}


void Micropolis::setSpeed(short speed)
{
    if (speed < 0) {
        speed = 0;
    } else if (speed > 3) {
        speed = 3;
    }

    simSpeedMeta = speed;

    if (simPaused) {
        simPausedSpeed = simSpeedMeta;
        speed = 0;
    }

    simSpeed = speed;

    callback("UIUpdate", "s", "speed");
}


void Micropolis::setPasses(int passes)
{
    simPasses = passes;
    simPass = 0;
    callback("UIUpdate", "s", "passes");
}

/**
 * Set the game level and initial funds.
 * @param level New game level.
 */
void Micropolis::setGameLevelFunds(GameLevel level)
{
    switch (level) {

        default:
        case LEVEL_EASY:
            setFunds(20000);
            setGameLevel(LEVEL_EASY);
            break;

        case LEVEL_MEDIUM:
            setFunds(10000);
            setGameLevel(LEVEL_MEDIUM);
            break;

        case LEVEL_HARD:
            setFunds(5000);
            setGameLevel(LEVEL_HARD);
            break;

    }
}


/** Set/change the game level.
 * @param level New game level.
 */
void Micropolis::setGameLevel(GameLevel level)
{
    assert(level >= LEVEL_FIRST && level <= LEVEL_LAST);
    gameLevel = level;
    updateGameLevel();
}


/** Report to the front-end that a new game level has been set. */
void Micropolis::updateGameLevel()
{
    callback("UIUpdate", "s", "gamelevel");
}


void Micropolis::setCityName(const std::string &name)
{
    std::string cleanName;

    int i;
    int n = name.length();
    for (i = 0; i < n; i++) {
        char ch = name[i];
        if (!isalnum(ch)) {
            ch = '_';
        }
        cleanName.push_back(ch);
    }

    setCleanCityName(cleanName);
}


/**
 * Set the name of the city.
 * @param name New name of the city.
 */
void Micropolis::setCleanCityName(const std::string &name)
{
    cityName = name;

    callback("UIUpdate", "s", "cityname");
}


void Micropolis::setYear(int year)
{
    // Must prevent year from going negative, since it screws up the non-floored modulo arithmetic.
    if (year < startingYear) {
        year = startingYear;
    }

    year = (year - startingYear) - (cityTime / 48);
    cityTime += year * 48;
    doTimeStuff();
}


/**
 * Get the current year.
 * @return The current game year.
 */
int Micropolis::currentYear()
{
    return (cityTime / 48) + startingYear;
}


/**
 * Notify the user interface to start a new game.
 */
void Micropolis::doNewGame()
{
    callback("UINewGame", "");
}


/**
 * set the enableDisasters flag, and set the flag to
 * update the user interface.
 * @param value New setting for #enableDisasters
 */
void Micropolis::setEnableDisasters(bool value)
{
    enableDisasters = value;
    mustUpdateOptions = true;
}


/**
 * Set the auto-budget to the given value.
 * @param value New value for the auto-budget setting.
 */
void Micropolis::setAutoBudget(bool value)
{
    autoBudget = value;
    mustUpdateOptions = true;
}


/**
 * Set the autoBulldoze flag to the given value,
 * and set the mustUpdateOptions flag to update
 * the user interface.
 *
 * @param value The value to set autoBulldoze to.
 */
void Micropolis::setAutoBulldoze(bool value)
{
    autoBulldoze = value;
    mustUpdateOptions = true;
}


/**
 * Set the autoGoto flag to the given value,
 * and set the mustUpdateOptions flag to update
 * the user interface.
 *
 * @param value The value to set autoGoto to.
 */
void Micropolis::setAutoGoto(bool value)
{
    autoGoto = value;
    mustUpdateOptions = true;
}


/**
 * Set the enableSound flag to the given value,
 * and set the mustUpdateOptions flag to update
 * the user interface.
 *
 * @param value The value to set enableSound to.
 */
void Micropolis::setEnableSound(bool value)
{
    enableSound = value;
    mustUpdateOptions = true;
}


/**
 * Set the doAnimation flag to the given value,
 * and set the mustUpdateOptions flag to update
 * the user interface.
 *
 * @param value The value to set doAnimation to.
 */
void Micropolis::setDoAnimation(bool value)
{
    doAnimation = value;
    mustUpdateOptions = true;
}


/**
 * Set the doMessages flag to the given value,
 * and set the mustUpdateOptions flag to update
 * the user interface.
 *
 * @param value The value to set doMessages to.
 */
void Micropolis::setDoMessages(bool value)
{
    doMessages = value;
    mustUpdateOptions = true;
}


/**
 * Set the doNotices flag to the given value,
 * and set the mustUpdateOptions flag to update
 * the user interface.
 *
 * @param value The value to set doNotices to.
 */
void Micropolis::setDoNotices(bool value)
{
    doNotices = value;
    mustUpdateOptions = true;
}


/**
 * Return the residential, commercial and industrial
 * development demands, as floating point numbers
 * from -1 (lowest demand) to 1 (highest demand).
 */
void Micropolis::getDemands(
    float *resDemandResult,
    float *comDemandResult,
    float *indDemandResult)
{
    *resDemandResult = (float)resValve / (float)RES_VALVE_RANGE;
    *comDemandResult = (float)comValve / (float)COM_VALVE_RANGE;
    *indDemandResult = (float)indValve / (float)IND_VALVE_RANGE;
}


////////////////////////////////////////////////////////////////////////
