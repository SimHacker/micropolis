/* update.cpp
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

/** @file update.cpp */

////////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "micropolis.h"
#include "text.h"


/////////////////////////////////////////////////////////////////////////


void Micropolis::doUpdateHeads()
{
    showValves();
    doTimeStuff();
    reallyUpdateFunds();
    updateOptions();
}


void Micropolis::updateEditors()
{
    invalidateEditors();
    doUpdateHeads();
}


void Micropolis::updateMaps()
{
    invalidateMaps();
}


void Micropolis::updateGraphs()
{
    changeCensus();
}


void Micropolis::updateEvaluation()
{
    changeEval();
}


void Micropolis::updateHeads()
{
    mustUpdateFunds = true;
    valveFlag = true;
    cityTimeLast = cityYearLast = cityMonthLast = totalFundsLast = 
      resLast = comLast = indLast = -999999;
    doUpdateHeads();
}


void Micropolis::updateFunds()
{
    mustUpdateFunds = true;
}


void Micropolis::reallyUpdateFunds()
{
    if (!mustUpdateFunds) {
        return;
    }

    mustUpdateFunds = false;

    if (totalFunds != totalFundsLast) {
        totalFundsLast = totalFunds;

        callback(
            "UIUpdate",
            "s",
            "funds");
    }

}


void Micropolis::doTimeStuff()
{
    updateDate();
}


/**
 * @bug Message is wrong.
 */
void Micropolis::updateDate()
{
    int megalinium = 1000000;

    cityTimeLast = cityTime >> 2;

    cityYear = ((int)cityTime / 48) + (int)startingYear;
    cityMonth = ((int)cityTime % 48) >> 2;

    if (cityYear >= megalinium) {
        setYear(startingYear);
        cityYear = startingYear;
        sendMessage(MESSAGE_NOT_ENOUGH_POWER, NOWHERE, NOWHERE, true);

    }

    if ((cityYearLast != cityYear) ||
        (cityMonthLast != cityMonth)) {

        cityYearLast = cityYear;
        cityMonthLast = cityMonth;

        callback(
            "UIUpdate",
            "s",
            "date");
    }
}


void Micropolis::showValves()
{
    if (valveFlag) {
        drawValve();
        valveFlag = false;
    }
}


void Micropolis::drawValve()
{
    float r, c, i;

    r = resValve;

    if (r < -1500) {
        r = -1500;
    }

    if (r > 1500) {
        r = 1500;
    }

    c = comValve;

    if (c < -1500) {
        c = -1500;
    }

    if (c > 1500) {
        c = 1500;
    }

    i = indValve;

    if (i < -1500) {
        i = -1500;
    }

    if (i > 1500) {
        i = 1500;
    }

    if ((r != resLast) ||
        (c != comLast) ||
        (i != indLast)) {

        resLast = (int)r;
        comLast = (int)c;
        indLast = (int)i;

        setDemand(r, c, i);
    }
}


void Micropolis::setDemand(float r, float c, float i)
{
    callback(
        "UIUpdate",
        "s",
        "demand");
}


void Micropolis::updateOptions()
{
    if (mustUpdateOptions) {
        mustUpdateOptions = false;
	callback(
	    "UIUpdate",
	    "s",
	    "options");
    }
}


/** @todo Keeping track of pending updates should be moved to the interface
 *        (the simulator generates events, the interface forwards them to
 *        the GUI when possible/allowed.
 */
void Micropolis::updateUserInterface()
{
    /// @todo Send all pending update messages to the user interface.

    // city: after load file, load scenario, or generate city
    // map: when small overall map changes
    // editor: when large close-up map changes
    // graph: when graph changes
    // evaluation: when evaluation changes
    // budget: when budget changes
    // date: when date changes
    // funds: when funds change
    // demand: when demand changes
    // level: when level changes
    // speed: when speed changes
    // delay: when delay changes
    // option: when options change
}


////////////////////////////////////////////////////////////////////////
