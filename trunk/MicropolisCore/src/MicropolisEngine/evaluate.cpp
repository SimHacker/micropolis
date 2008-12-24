/* evaluate.cpp
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

/** @file evaluate.cpp */

////////////////////////////////////////////////////////////////////////


#include "stdafx.h"

#include <cassert>


////////////////////////////////////////////////////////////////////////


/**
 * @todo: These strings should not be hard coded into the core simulator.
 *        The scripting language should look them up in translation files.
 */

// City Classes:
// "VILLAGE", "TOWN", "CITY", "CAPITAL", "METROPOLIS", "MEGALOPOLIS"

// City Levels:
// "Easy", "Medium", "Hard"

// City Problems:
//  "CRIME", "POLLUTION", "HOUSING COSTS", "TAXES", "TRAFFIC", "UNEMPLOYMENT", "FIRES"


////////////////////////////////////////////////////////////////////////


/** Evaluate city */
void Micropolis::CityEvaluation()
{
  if (TotalPop) {
    getAssValue();
    DoPopNum();
    DoProblems();
    GetScore();
    DoVotes();
    ChangeEval();
  } else {
    EvalInit();
    ChangeEval();
  }
}


/* comefrom: CityEvaluation SetCommonInits */
void Micropolis::EvalInit()
{
  cityYes = 0;
  cityPop = 0;
  deltaCityPop = 0;
  cityAssValue = 0;
  cityClass = CC_VILLAGE;
  cityScore = 500;
  deltaCityScore = 0;
  for (int i = 0; i < PROBNUM; i++) {
    problemVotes[i] = 0;
  }
  for (int i = 0; i < 4; i++) {
    problemOrder[i] = 0;
  }
}


/* comefrom: CityEvaluation */
void Micropolis::getAssValue()
{
  Quad z;

  z = RoadTotal * 5;
  z += RailTotal * 10;
  z += PolicePop * 1000;
  z += FireStPop * 1000;
  z += HospPop * 400;
  z += StadiumPop * 3000;
  z += PortPop * 5000;
  z += APortPop * 10000;
  z += CoalPop * 3000;
  z += NuclearPop * 6000;
  cityAssValue = z * 1000;
}


/**
 * Compute city population and city classification
 * @see cityPop cityClass
 */
void Micropolis::DoPopNum()
{
  Quad oldCityPop = cityPop;

  cityPop = (ResPop + (ComPop + IndPop) * 8L) * 20L;

  if (oldCityPop == -1) {
    oldCityPop = cityPop;
  }

  deltaCityPop = cityPop - oldCityPop;

  // Compute cityClass
  cityClass = CC_VILLAGE;
  if (cityPop > 2000) {
    cityClass = CC_TOWN;
  }
  if (cityPop > 10000) {
    cityClass = CC_CITY;
  }
  if (cityPop > 50000) {
    cityClass = CC_CAPITAL;
  }
  if (cityPop > 100000) {
    cityClass = CC_METROPOLIS;
  }
  if (cityPop > 500000) {
    cityClass = CC_MEGALOPOLIS;
  }
}


/** Evaluate problems of the city, take votes, and decide the most important ones */
void Micropolis::DoProblems()
{
  bool problemTaken[PROBNUM]; // Which problems are taken?

  for (int z = 0; z < PROBNUM; z++) {
    problemTaken[z] = false;
    problemTable[z] = 0;
  }

  problemTable[CVP_CRIME]        = CrimeAverage;          /* Crime */
  problemTable[CVP_POLLUTION]    = PolluteAverage;        /* Pollution */
  problemTable[CVP_HOUSING]      = LVAverage * 7 / 10;    /* Housing */
  problemTable[CVP_TAXES]        = CityTax * 10;          /* Taxes */
  problemTable[CVP_TRAFFIC]      = AverageTrf();          /* Traffic */
  problemTable[CVP_UNEMPLOYMENT] = GetUnemployment();     /* Unemployment */
  problemTable[CVP_FIRE]         = GetFire();             /* Fire */
  voteProblems();

  for (int z = 0; z < 4; z++) {
    // Find biggest problem not taken yet
    int maxVotes = 0;
    int bestProblem = CVP_NUMPROBLEMS;
    for (int i = 0; i < CVP_NUMPROBLEMS; i++) {
      if ((problemVotes[i] > maxVotes) && (!problemTaken[i])) {
        bestProblem = i;
        maxVotes = problemVotes[i];
      }
    }

    // bestProblem == CVP_NUMPROBLEMS means no problem found
    problemOrder[z] = bestProblem;
    if (bestProblem < CVP_NUMPROBLEMS) {
      problemTaken[bestProblem] = true;
    }
    // else: No problem found.
    //       Repeating the procedure will give the same result.
    //       Optimize by filling all remaining entries, and breaking out
  }
}


/**
 * Vote on the problems of the city
 *
 * @post problemVotes contains the vote counts
 */
void Micropolis::voteProblems()
{
  int x, z, count;

  for (z = 0; z < PROBNUM; z++) {
    problemVotes[z] = 0;
  }

  x = 0;
  z = 0;
  count = 0;
  while ((z < 100) &&
         (count < 600)) {
    if (Rand(300) < problemTable[x]) {
      problemVotes[x]++;
      z++;
    }
    x++;
    if (x > PROBNUM) {
      x = 0;
    }
    count++;
  }
}


/* comefrom: DoProblems */
short Micropolis::AverageTrf()
{
  Quad TrfTotal;
  register short x, y, count;

  TrfTotal = 0;
  count = 1;
  for (x=0; x < HWLDX; x++) {
    for (y=0; y < HWLDY; y++) {
      if (LandValueMem[x][y]) {
        TrfTotal += TrfDensity[x][y];
        count++;
      }
    }
  }

  trafficAverage = (TrfTotal / count) * 2.4;

  return trafficAverage;
}


/* comefrom: DoProblems */
short Micropolis::GetUnemployment()
{
  float r;
  short b;

  b = (ComPop + IndPop) * 8;

  if (b) {
    r = ((float)ResPop) / b;
  } else {
    return 0;
  }

  b = (r - 1) * 255;
  if (b > 255) {
    b = 255;
  }

  return b;
}


/* comefrom: DoProblems GetScore */
short Micropolis::GetFire()
{
  short z;

  z = FirePop * 5;
  if (z > 255) {
    return 255;
  } else {
    return z;
  }
}


/* comefrom: CityEvaluation */
void Micropolis::GetScore()
{
  int x, z;
  short oldCityScore;
  float SM, TM;

  oldCityScore = cityScore;
  x = 0;

  for (z = 0; z < CVP_NUMPROBLEMS; z++) {
    x += problemTable[z];       /* add 7 probs */
  }

  /*
   * @todo Should this expression depend on CVP_NUMPROBLEMS?
   */
  x = x / 3;                    /* 7 + 2 average */

  if (x > 256) {
    x = 256;
  }

  z = (256 - x) * 4;

  if (z > 1000) {
    z = 1000;
  }

  if (z < 0) {
    z = 0;
  }

  if (ResCap) {
    z = (int)(z * .85);
  }

  if (ComCap) {
    z = (int)(z * .85);
  }

  if (IndCap) {
    z = (int)(z * .85);
  }

  if (RoadEffect < MAX_ROAD_EFFECT)  {
    z -= MAX_ROAD_EFFECT - RoadEffect;
  }

  if (PoliceEffect < MAX_POLICESTATION_EFFECT) {
    // 10.0001 = 10000.1 / 1000, 1/10.0001 is about 0.1
    z = (int)(z * (0.9 + (PoliceEffect / (10.0001 * MAX_POLICESTATION_EFFECT))));
  }

  if (FireEffect < MAX_FIRESTATION_EFFECT) {
    // 10.0001 = 10000.1 / 1000, 1/10.0001 is about 0.1
    z = (int)(z * (0.9 + (FireEffect / (10.0001 * MAX_FIRESTATION_EFFECT))));
  }

  if (RValve < -1000) {
    z = (int)(z * .85);
  }

  if (CValve < -1000) {
    z = (int)(z * .85);
  }

  if (IValve < -1000) {
    z = (int)(z * .85);
  }

  SM = 1.0;

  if (cityPop == 0 || deltaCityPop == 0) {
    SM = 1.0;
  } else if (deltaCityPop == cityPop) {
    SM = 1.0;
  } else if (deltaCityPop > 0) {
    SM = ((float)deltaCityPop / cityPop) + 1.0f;
  } else if (deltaCityPop < 0) {
    SM = 0.95f + ((float)deltaCityPop / (cityPop - deltaCityPop));
  }

  z = (int)(z * SM);
  z = z - GetFire();            /* dec score for fires */
  z = z - CityTax;

  TM = unPwrdZCnt + PwrdZCnt;   /* dec score for unpowered zones */

  if (TM) {
    SM = PwrdZCnt / TM;
  } else {
    SM = 1.0;
  }

  z = (int)(z * SM);

  if (z > 1000) {
    z = 1000;
  }

  if (z < 0) {
    z = 0;
  }

  cityScore = (cityScore + z) / 2;

  deltaCityScore = cityScore - oldCityScore;

}


/* comefrom: CityEvaluation */
void Micropolis::DoVotes()
{
  register int z;

  cityYes = 0;

  for (z = 0; z < 100; z++) {
    if (Rand(1000) < cityScore) {
      cityYes++;
    }
  }
}


/* comefrom: DoSubUpDate scoreDoer */
void Micropolis::doScoreCard()
{
  Callback(
        "UIUpdate",
        "s",
        "evaluation");

  // The user interface should pull these raw values out and format
  // them. The simulator core used to format them and push them out,
  // but the user interface should pull them out and format them
  // itself.

  // City Evaluation ${FormatYear(CurrentYear())}
  // Public Opinion
  //   Is the mayor doing a good job?
  //     Yes: ${FormatPercent(cityYes)}
  //     No: ${FormatPercent(100 - cityYes)}
  //   What are the worst problems?
  //     for i in range(0, 4), while problemOrder[i] < CVP_NUMPROBLEMS:
  //     ${probStr[problemOrder[i]]}: ${FormatPercent(problemVotes[problemOrder[i]])}
  // Statistics
  //   Population: ${FormatNumber?(pop)}
  //   Net Migration: ${FormatNumber(cityPop)}
  //   (last year): ${FormatNumber(deltaCityPop)}
  //   Assessed Value: ${FormatMoney(cityAssValue))
  //   Category: ${cityClassStr[cityClass]}
  //   Game Level: ${cityLevelStr[GameLevel]}
}


void Micropolis::ChangeEval()
{
  evalChanged = true;
}


void Micropolis::scoreDoer()
{
  if (evalChanged) {
    doScoreCard();
    evalChanged = false;
  }
}


////////////////////////////////////////////////////////////////////////
