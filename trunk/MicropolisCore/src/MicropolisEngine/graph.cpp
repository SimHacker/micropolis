/* graph.cpp
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

/** @file graph.cpp */

////////////////////////////////////////////////////////////////////////


#include "stdafx.h"


////////////////////////////////////////////////////////////////////////


//char *Micropolis::HistName[] = {
//  "Residential", "Commercial", "Industrial",
//  "Cash Flow", "Crime", "Pollution"
//};

//unsigned char Micropolis::HistColor[] = {
//  COLOR_LIGHTGREEN, COLOR_DARKBLUE, COLOR_YELLOW,
//  COLOR_DARKGREEN, COLOR_RED, COLOR_OLIVE
//};


////////////////////////////////////////////////////////////////////////


void Micropolis::drawMonth(
  short *hist,
  unsigned char *s,
  float scale)
{
  register short val;
  register short x;

  for (x = 0; x < 120; x++) {
    val = (short)(hist[x] * scale);
    if (val < 0) {
      val = 0;
    }
    if (val > 255) {
      val = 255;
    }
    s[119 - x] =
      (unsigned char)val;
  }
}


void Micropolis::doAllGraphs()
{
  float scaleValue;
  short AllMax = 0;

  if (ResHisMax > AllMax) {
    AllMax = ResHisMax;
  }

  if (ComHisMax > AllMax) {
    AllMax = ComHisMax;
  }

  if (IndHisMax > AllMax) {
    AllMax = IndHisMax;
  }

  if (AllMax <= 128) {
    AllMax = 0;
  }

  if (AllMax) {
    scaleValue =
      (float)128.0 /
      (float)AllMax;
  } else {
    scaleValue = 1.0;
  }

  // scaleValue = 0.5; // XXX

  drawMonth(ResHis, History10[RES_HIST], scaleValue);
  drawMonth(ComHis, History10[COM_HIST], scaleValue);
  drawMonth(IndHis, History10[IND_HIST], scaleValue);
  drawMonth(MoneyHis, History10[MONEY_HIST], 1.0);
  drawMonth(CrimeHis, History10[CRIME_HIST], 1.0);
  drawMonth(PollutionHis, History10[POLLUTION_HIST], 1.0);

  AllMax = 0;

  if (Res2HisMax > AllMax) {
    AllMax = Res2HisMax;
  }

  if (Com2HisMax > AllMax) {
    AllMax = Com2HisMax;
  }

  if (Ind2HisMax > AllMax) {
    AllMax = Ind2HisMax;
  }

  if (AllMax <= 128) {
    AllMax = 0;
  }

  if (AllMax) {
    scaleValue =
      (float)128.0 /
      (float)AllMax;
  } else {
    scaleValue =
      (float)1.0;
  }

  // scaleValue = 0.5; // XXX

  drawMonth(ResHis + 120, History120[RES_HIST], scaleValue);
  drawMonth(ComHis + 120, History120[COM_HIST], scaleValue);
  drawMonth(IndHis + 120, History120[IND_HIST], scaleValue);
  drawMonth(MoneyHis + 120, History120[MONEY_HIST], 1.0);
  drawMonth(CrimeHis + 120, History120[CRIME_HIST], 1.0);
  drawMonth(PollutionHis + 120, History120[POLLUTION_HIST], 1.0);
}


void Micropolis::ChangeCensus()
{
  CensusChanged = 1;
}


void Micropolis::graphDoer()
{
  if (CensusChanged) {
    doAllGraphs();
    NewGraph = 1;
    CensusChanged = 0;
  }

  if (NewGraph) {
        Callback(
          "UIUpdate",
          "s",
          "graph");
    NewGraph = 0;
  }
}


void Micropolis::initGraphs()
{
  if (!HistoryInitialized) {
    HistoryInitialized = 1;
  }
}


/* comefrom: InitWillStuff */
void Micropolis::InitGraphMax()
{
  register int x;

  ResHisMax = 0;
  ComHisMax = 0;
  IndHisMax = 0;

  for (x = 118; x >= 0; x--) {

    if (ResHis[x] > ResHisMax) {
      ResHisMax = ResHis[x];
    }

    if (ComHis[x] > ComHisMax) {
      ComHisMax = ComHis[x];
    }

    if (IndHis[x] > IndHisMax) {
      IndHisMax = IndHis[x];
    }

    if (ResHis[x] < 0) {
      ResHis[x] = 0;
    }

    if (ComHis[x] < 0) {
      ComHis[x] = 0;
    }

    if (IndHis[x] < 0) {
      IndHis[x] = 0;
    }

  }

  Graph10Max = ResHisMax;

  if (ComHisMax > Graph10Max) {
    Graph10Max = ComHisMax;
  }

  if (IndHisMax > Graph10Max) {
    Graph10Max = IndHisMax;
  }

  Res2HisMax = 0;
  Com2HisMax = 0;
  Ind2HisMax = 0;

  for (x = 238; x >= 120; x--) {

    if (ResHis[x] > Res2HisMax) {
      Res2HisMax = ResHis[x];
    }

    if (ComHis[x] > Com2HisMax) {
      Com2HisMax = ComHis[x];
    }

    if (IndHis[x] > Ind2HisMax) {
      Ind2HisMax = IndHis[x];
    }

    if (ResHis[x] < 0) {
      ResHis[x] = 0;
    }

    if (ComHis[x] < 0) {
      ComHis[x] = 0;
    }

    if (IndHis[x] < 0) {
      IndHis[x] = 0;
    }

  }

  Graph120Max = Res2HisMax;

  if (Com2HisMax > Graph120Max) {
    Graph120Max = Com2HisMax;
  }

  if (Ind2HisMax > Graph120Max) {
    Graph120Max = Ind2HisMax;
  }

}


////////////////////////////////////////////////////////////////////////
