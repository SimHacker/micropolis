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


////////////////////////////////////////////////////////////////////////


#include "stdafx.h"


////////////////////////////////////////////////////////////////////////


// TODO: These strings should not be hard coded into the core simulator.
// The scripting language should look them up in translation files. 

char *Micropolis::cityClassStr[6] = {
  "VILLAGE", "TOWN", "CITY", "CAPITAL", "METROPOLIS", "MEGALOPOLIS"
};

char *Micropolis::cityLevelStr[3] = {
  "Easy", "Medium", "Hard"
};

char *Micropolis::probStr[10] = {
  "CRIME", "POLLUTION", "HOUSING COSTS", "TAXES",
  "TRAFFIC", "UNEMPLOYMENT", "FIRES"
};


////////////////////////////////////////////////////////////////////////


/* comefrom: SpecialInit Simulate */
void Micropolis::CityEvaluation()
{
  EvalValid = 0;

  if (TotalPop) {
    GetAssValue();
    DoPopNum();
    DoProblems();
    GetScore();
    DoVotes();
    ChangeEval();
  } else {
    EvalInit();
    ChangeEval();
  }

  EvalValid = 1;
}


/* comefrom: CityEvaluation SetCommonInits */
void Micropolis::EvalInit()
{
  register short x, z;

  z = 0;
  CityYes = z;
  CityNo = z;
  CityPop = z; 
  deltaCityPop = z;
  CityAssValue = z;
  CityClass = z;
  CityScore = 500; 
  deltaCityScore = z;
  EvalValid = 1;
  for (x = 0; x < PROBNUM; x++) {
    ProblemVotes[x] = z;
  }
  for (x = 0; x < 4; x++) {
    ProblemOrder[x] = z;
  }
}


/* comefrom: CityEvaluation */
void Micropolis::GetAssValue()
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
  CityAssValue = z * 1000;
}


/* comefrom: CityEvaluation */
void Micropolis::DoPopNum()
{
  Quad OldCityPop;

  OldCityPop = 
    CityPop;
  CityPop = 
    ((ResPop) + (ComPop * 8L) + (IndPop *8L)) * 20L;

  if (OldCityPop == -1) {
    OldCityPop = CityPop;
  }

  deltaCityPop = 
    CityPop - OldCityPop;

  CityClass = 0;        /* village */
  if (CityPop > 2000) {
    CityClass++;        /* town */
  }
  if (CityPop > 10000) {
    CityClass++;        /* city */
  }
  if (CityPop > 50000) {
    CityClass++;        /* capital */
  }
  if (CityPop > 100000) {
    CityClass++;        /* metropolis */
  }
  if (CityPop > 500000) {
    CityClass++;        /* megalopolis */
  }
}


/* comefrom: CityEvaluation */
void Micropolis::DoProblems()
{
  register short x, z;
  short ThisProb = 0;
  short Max = 0;

  for (z = 0; z < PROBNUM; z++) {
    ProblemTable[z] = 0;
  }
  ProblemTable[0] = CrimeAverage;               /* Crime */
  ProblemTable[1] = PolluteAverage;             /* Pollution */
  ProblemTable[2] = (short)(LVAverage * .7);    /* Housing */
  ProblemTable[3] = CityTax * 10;               /* Taxes */
  ProblemTable[4] = AverageTrf();               /* Traffic */
  ProblemTable[5] = GetUnemployment();          /* Unemployment */
  ProblemTable[6] = GetFire();                  /* Fire */
  VoteProblems();

  for (z = 0; z < PROBNUM; z++) {
    ProblemTaken[z] = 0;
  }

  for (z = 0; z < 4; z++) {
    Max = 0;

    for (x = 0; x < 7; x++) {
      if ((ProblemVotes[x] > Max) && (!ProblemTaken[x])) {
        ThisProb = x;
        Max = ProblemVotes[x];
      }
    }

    if (Max) {
      ProblemTaken[ThisProb] = 1;
      ProblemOrder[z] = ThisProb;
    } else {
      ProblemOrder[z] = 7;
      ProblemTable[7] = 0;
    }
  }
}


/* comefrom: DoProblems */
void Micropolis::VoteProblems()
{
  register int x, z, count;

  for (z = 0; z < PROBNUM; z++) {
    ProblemVotes[z] = 0;
  }

  x = 0;
  z = 0;
  count = 0;
  while ((z < 100) && 
         (count < 600)) {
    if (Rand(300) < ProblemTable[x]) {
      ProblemVotes[x]++;
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

  TrafficAverage = 
    (short)((TrfTotal / count) * 2.4);

  return TrafficAverage;
}


/* comefrom: DoProblems */
short Micropolis::GetUnemployment()
{
  float r;
  short b;

  b = 
    (ComPop + IndPop) << 3;

  if (b) {
    r = ((float)ResPop) / b;
  } else {
    return (0);
  }

  b = 
    (short)((r - 1) * 255);
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
  register int x, z;
  short OldCityScore;
  float SM, TM;

  OldCityScore = CityScore;
  x = 0;

  for (z = 0; z < 7; z++) {
    x += ProblemTable[z];       /* add 7 probs */
  }

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

  if (RoadEffect < 32)  {
    z = (int)(z - (32 - RoadEffect));
  }

  if (PoliceEffect < 1000) {
    z = (int)(z * (.9 + (PoliceEffect / 10000.1)));
  }

  if (FireEffect < 1000) {
    z = (int)(z * (.9 + (FireEffect / 10000.1)));
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

  if ((CityPop == 0) || 
      (deltaCityPop == 0)) {
    SM = 
      1.0;
  } else if (deltaCityPop == CityPop) {
    SM = 
      1.0;
  } else if (deltaCityPop > 0) {
    SM =
      ((float)deltaCityPop / CityPop) + 
      (float)1.0;
  } else if (deltaCityPop < 0) {
    SM = 
      (float)0.95 + 
      ((float) deltaCityPop / (CityPop - deltaCityPop));
  }

  z = (int)(z * SM);
  z = z - GetFire();            /* dec score for fires */
  z = z - CityTax;

  TM = 
    (float)(unPwrdZCnt + PwrdZCnt);   /* dec score for unpowered zones */

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

  CityScore = 
    (CityScore + z) / 2; 

  deltaCityScore = 
    CityScore - OldCityScore;
}


/* comefrom: CityEvaluation */
void Micropolis::DoVotes()
{
  register int z;

  CityYes = 0;
  CityNo = 0;

  for (z = 0; z < 100; z++) {
    if (Rand(1000) < CityScore) {
      CityYes++;
    } else {
      CityNo++;
    }
  }
}


/* comefrom: DoSubUpDate scoreDoer */
// TODO: The scripting language should pull these raw values out and format them,
// instead of the simulator core formatting them and pushing them out. 
void Micropolis::doScoreCard()
{
  char title[256];
  char goodyes[32];
  char goodno[32];
  char prob0[32];
  char prob1[32];
  char prob2[32];
  char prob3[32];
  char pop[32];
  char delta[32];
  char assessed[32];
  char assessed_dollars[32];
  char score[32];
  char changed[32];

  /* send /EvaluationTitle /SetValue [ (titleStr year) ] */
  /* send /GoodJobPercents /SetValue [ (CityYes%) (CityNo%) ] */
  /* send /WorstProblemPercents /SetValue [ (ProblemVotes[0]%) ... ] */
  /* send /WorstProblemNames /SetValue [ (probStr[ProblemOrder[0]) ... ] */
  /*    put ^chars around first problem name to make it bold */
  /* send /Statistics /SetValue
          [ (CityPop) (deltaCityPop) () (CityAssValue)
            (cityClassStr[CityClass]) (cityLevelStr[GameLevel]) ] */
  /* send /CurrentScore /SetValue [ (CityScore) ] */
  /* send /AnnualChange /SetValue [ (deltaCityScore) ] */

  sprintf(title, "City Evaluation  %d", (int)CurrentYear());
  sprintf(goodyes, "%d%%", (int)CityYes);
  sprintf(goodno, "%d%%", (int)CityNo);
  sprintf(prob0, "%d%%", (int)ProblemVotes[ProblemOrder[0]]);
  sprintf(prob1, "%d%%", (int)ProblemVotes[ProblemOrder[1]]);
  sprintf(prob2, "%d%%", (int)ProblemVotes[ProblemOrder[2]]);
  sprintf(prob3, "%d%%", (int)ProblemVotes[ProblemOrder[3]]);
  sprintf(pop, "%d", (int)CityPop);
  sprintf(delta, "%d", (int)deltaCityPop);
  sprintf(assessed, "%d", (int)CityAssValue);
  makeDollarDecimalStr(assessed, assessed_dollars);

  sprintf(score, "%d", (int)CityScore);
  sprintf(changed, "%d", (int)deltaCityScore);

  SetEvaluation(
    changed, 
    score,
    ProblemVotes[ProblemOrder[0]] ? probStr[ProblemOrder[0]] : " ",
    ProblemVotes[ProblemOrder[1]] ? probStr[ProblemOrder[1]] : " ",
    ProblemVotes[ProblemOrder[2]] ? probStr[ProblemOrder[2]] : " ",
    ProblemVotes[ProblemOrder[3]] ? probStr[ProblemOrder[3]] : " ",
    ProblemVotes[ProblemOrder[0]] ? prob0 : " ",
    ProblemVotes[ProblemOrder[1]] ? prob1 : " ",
    ProblemVotes[ProblemOrder[2]] ? prob2 : " ",
    ProblemVotes[ProblemOrder[3]] ? prob3 : " ",
    pop, 
    delta, 
    assessed_dollars,
    cityClassStr[CityClass], 
    cityLevelStr[GameLevel],
    goodyes, 
    goodno, 
    title);
}


void Micropolis::ChangeEval()
{
  EvalChanged = 1;
}


void Micropolis::scoreDoer()
{
  if (EvalChanged) {
    doScoreCard();
    EvalChanged = 0;
  }
}


// TODO: The scripting language should pull these raw values out and format them,
// instead of the simulator core formatting them and pushing them out. 
void Micropolis::SetEvaluation(
  const char *changed, 
  const char *score,
  const char *ps0, 
  const char *ps1, 
  const char *ps2, 
  const char *ps3,
  const char *pv0, 
  const char *pv1, 
  const char *pv2, 
  const char *pv3,
  const char *pop, 
  const char *delta, 
  const char *assessed_dollars, 
  const char *cityclass, 
  const char *citylevel, 
  const char *goodyes, 
  const char *goodno, 
  const char *title)
{
  evalChanged = changed;
  evalScore = score;
  evalPs0 = ps0;
  evalPs1 = ps1;
  evalPs2 = ps2;
  evalPs3 = ps3;
  evalPv0 = pv0;
  evalPv1 = pv1;
  evalPv2 = pv2;
  evalPv3 = pv3;
  evalPop = pop;
  evalDelta = delta;
  evalAssessedDollars = assessed_dollars;
  evalCityClass = cityclass;
  evalCityLevel = citylevel;
  evalGoodYes = goodyes;
  evalGoodNo = goodno;
  evalTitle = title;

  Callback(
    "UISetEvaluation",
	"ssssssssssssssssss",
    changed, 
    score,
    ps0, 
    ps1, 
    ps2, 
    ps3,
    pv0, 
    pv1, 
    pv2, 
    pv3,
    pop, 
    delta, 
    assessed_dollars,
    cityclass, 
    citylevel,
    goodyes, 
    goodno, 
    title);
}


////////////////////////////////////////////////////////////////////////
