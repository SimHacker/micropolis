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


////////////////////////////////////////////////////////////////////////


/* comefrom: drawTaxesCollected incBoxValue decBoxValue drawCurrentFunds
             drawActualBox UpdateFunds updateCurrentCost */
void Micropolis::makeDollarDecimalStr(
  char *numStr,
  char *dollarStr)
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
 * @see Resume
 */
void Micropolis::Pause()
{
  if (!sim_paused) {
    sim_paused_speed = SimMetaSpeed;
    setSpeed(0);
    sim_paused = true;
  }
}

/**
 * Resume simulation after pausing it
 * @see Pause
 */
void Micropolis::Resume()
{
  if (sim_paused) {
    sim_paused = false;
    setSpeed(sim_paused_speed);
  }
}


void Micropolis::setSpeed(
  short speed)
{
  if (speed < 0) {
    speed = 0;
  } else if (speed > 3) {
    speed = 3;
  }

  SimMetaSpeed = speed;

  if (sim_paused) {
    sim_paused_speed = SimMetaSpeed;
    speed = 0;
  }

  SimSpeed = speed;

  Callback(
    "UISetSpeed",
    "d",
    (int)(sim_paused ? 0 : SimMetaSpeed));

}


void Micropolis::setSkips(
  int skips)
{
  sim_skips = skips;
  sim_skip = 0;
}


void Micropolis::SetGameLevelFunds(
  short level)
{
  switch (level) {

  default:
  case 0:
    SetFunds(20000);
    SetGameLevel(0);
    break;

  case 1:
    SetFunds(10000);
    SetGameLevel(1);
    break;

  case 2:
    SetFunds(5000);
    SetGameLevel(2);
    break;

  }
}


void Micropolis::SetGameLevel(
  short level)
{
  GameLevel = level;
  UpdateGameLevel();
}


void Micropolis::UpdateGameLevel()
{
  Callback(
    "UISetGameLevel",
    "d",
    (int)GameLevel);
}


void Micropolis::setCityName(
  char *name)
{
  char *cp = name;

  // FIXME: This actually modifies the string passed in. Kinda gross.

  while (*cp) {

    if (!isalnum(*cp)) {
      *cp = '_';
    }

    cp++;
  }

  setAnyCityName(name);
}


void Micropolis::setAnyCityName(
  char *name)
{
  CityName = name;

  Callback(
    "UISetCityName",
    "s",
    CityName);
}


void Micropolis::SetYear(
  int year)
{
  // Must prevent year from going negative, since it screws up the non-floored modulo arithmetic.
  if (year < StartingYear) {
    year = StartingYear;
  }

  year = (year - StartingYear) - (CityTime / 48);
  CityTime += year * 48;
  doTimeStuff();
}


int Micropolis::CurrentYear()
{
  return ((CityTime / 48) + StartingYear);
}


void Micropolis::DoNewGame()
{
  Callback("UINewGame", "");
}


void Micropolis::DoGeneratedCityImage(
  char *name,
  int time,
  int pop,
  char *cityClassName,
  int score)
{
  /* XXX: TODO: print city */
}


////////////////////////////////////////////////////////////////////////
