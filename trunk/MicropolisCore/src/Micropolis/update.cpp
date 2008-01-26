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


////////////////////////////////////////////////////////////////////////


#include "stdafx.h"


/////////////////////////////////////////////////////////////////////////


char *Micropolis::dateStr[12] = {
  "Jan", "Feb", "Mar", "Apr", "May", "Jun", 
  "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};


////////////////////////////////////////////////////////////////////////


void Micropolis::DoUpdateHeads()
{
  showValves();
  doTimeStuff();
  ReallyUpdateFunds();
  updateOptions();
}


void Micropolis::UpdateEditors()
{
  InvalidateEditors();
  DoUpdateHeads();
}


void Micropolis::UpdateMaps()
{
  InvalidateMaps();
}


void Micropolis::UpdateGraphs()
{
  ChangeCensus();
}


void Micropolis::UpdateEvaluation()
{
  ChangeEval();
}


void Micropolis::UpdateHeads()
{
  MustUpdateFunds = ValveFlag = 1;
  LastCityTime = LastCityYear = LastCityMonth = LastFunds = LastR = -999999;
  DoUpdateHeads();
}

void Micropolis::UpdateFunds()
{
  MustUpdateFunds = 1;
//  Kick();
}


void Micropolis::ReallyUpdateFunds()
{
  char localStr[256], dollarStr[256];

  if (!MustUpdateFunds) {
    return;
  }

  MustUpdateFunds = 0;

  if (TotalFunds < 0) {
    TotalFunds = 0;
  }

  if (TotalFunds != LastFunds) {
    LastFunds = TotalFunds;
    sprintf(localStr, "%d", (int)TotalFunds);
    makeDollarDecimalStr(localStr, dollarStr);

    Callback(
      "UISetFunds",
      "s",
      dollarStr);
  }

}


void Micropolis::doTimeStuff()
{
//  if ((CityTime >> 2) != LastCityTime) {
    updateDate();
//  }
}


void Micropolis::updateDate()
{       
  int y;
  int m;
  char str[256];
  int megalinium = 1000000;

  LastCityTime = CityTime >> 2;

  y = ((int)CityTime / 48) + (int)StartingYear;
  m = ((int)CityTime % 48) >> 2;

  if (y >= megalinium) {
    SetYear(StartingYear);
    y = StartingYear;
    SendMes(-40);
  }

  doMessage();

  if ((LastCityYear != y) ||
      (LastCityMonth != m)) {

    LastCityYear = y;
    LastCityMonth = m;

    sprintf(
      str, 
      "%s %d", 
      dateStr[m], 
      (int)y);

    CityDate = str;

    Callback(
      "UISetDate",
      "sdd",
      str,
      (int)m,
      (int)y);
  }
}


void Micropolis::showValves()
{
  if (ValveFlag) {
    drawValve();
    ValveFlag = 0;
  }
}


void Micropolis::drawValve()
{
  double r, c, i;

  r = RValve;

  if (r < -1500) {
    r = -1500;
  }

  if (r > 1500) {
    r = 1500;
  }

  c = CValve;

  if (c < -1500) {
    c = -1500;
  }

  if (c > 1500) {
    c = 1500;
  }

  i = IValve;

  if (i < -1500) {
    i = -1500;
  }

  if (i > 1500) {
    i = 1500;
  }

  if ((r != LastR) ||
      (c != LastC) ||
      (i != LastI)) {

    LastR = (int)r;
    LastC = (int)c;
    LastI = (int)i;

    SetDemand(r, c, i);
  }
}


void Micropolis::SetDemand(
  double r, 
  double c, 
  double i)
{
  Callback(
    "UISetDemand",
    "ddd",
    (int)(r / 100), 
    (int)(c / 100), 
    (int)(i / 100));
}


void Micropolis::updateOptions()
{
  int options;

  if (MustUpdateOptions) {

    options = 0;

    if (autoBudget) {
      options |= 1;
    }

    if (autoGo) {
      options |= 2;
    }

    if (autoBulldoze) {
      options |= 4;
    }

    if (!NoDisasters) {
      options |= 8;
    }

    if (UserSoundOn) {
      options |= 16;
    }

    if (DoAnimation) {
      options |= 32;
    }

    if (DoMessages) {
      options |= 64;
    }

    if (DoNotices) {
      options |= 128;
    }

    MustUpdateOptions = 0;
    UpdateOptionsMenu(options);
  }
}


void Micropolis::UpdateOptionsMenu(
  int options)
{
  Callback(
    "UISetOptions",
    "dddddddd",
    (options & 1) ? 1 : 0, 
    (options & 2) ? 1 : 0,
    (options & 4) ? 1 : 0, 
    (options & 8) ? 1 : 0,
    (options & 16) ? 1 : 0, 
    (options & 32) ? 1 : 0,
    (options & 64) ? 1 : 0,
    (options & 128) ? 1 : 0);
}


////////////////////////////////////////////////////////////////////////
