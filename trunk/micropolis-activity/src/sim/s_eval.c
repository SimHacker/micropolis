/* s_eval.c
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
#include "sim.h"


/* City Evaluation */


short EvalValid;
short CityYes, CityNo;
short ProblemTable[PROBNUM];
short ProblemTaken[PROBNUM];
short ProblemVotes[PROBNUM];		/* these are the votes for each  */
short ProblemOrder[4];			/* sorted index to above  */
QUAD CityPop, deltaCityPop;
QUAD CityAssValue;
short CityClass;			/*  0..5  */
short CityScore, deltaCityScore, AverageCityScore;
short TrafficAverage;


/* comefrom: SpecialInit Simulate */
CityEvaluation(void)
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
EvalInit(void)
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
  for (x = 0; x < PROBNUM; x++)
    ProblemVotes[x] = z;
  for (x = 0; x < 4; x++)
    ProblemOrder[x] = z;
}


/* comefrom: CityEvaluation */
GetAssValue(void)
{
  QUAD z;

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
DoPopNum(void)
{
  QUAD OldCityPop;

  OldCityPop = CityPop;
  CityPop = ((ResPop) + (ComPop * 8L) + (IndPop *8L)) * 20L;
  if (OldCityPop == -1) {
    OldCityPop = CityPop;
  }
  deltaCityPop = CityPop - OldCityPop;

  CityClass = 0;			/* village */
  if (CityPop > 2000)	CityClass++;	/* town */
  if (CityPop > 10000)	CityClass++;	/* city */
  if (CityPop > 50000)	CityClass++;	/* capital */
  if (CityPop > 100000)	CityClass++;	/* metropolis */
  if (CityPop > 500000)	CityClass++;	/* megalopolis */
}


/* comefrom: CityEvaluation */
DoProblems(void)
{
  register short x, z;
  short ThisProb, Max;

  for (z = 0; z < PROBNUM; z++)
    ProblemTable[z] = 0;
  ProblemTable[0] = CrimeAverage;		/* Crime */
  ProblemTable[1] = PolluteAverage;		/* Pollution */
  ProblemTable[2] = LVAverage * .7;		/* Housing */
  ProblemTable[3] = CityTax * 10;		/* Taxes */
  ProblemTable[4] = AverageTrf();		/* Traffic */
  ProblemTable[5] = GetUnemployment();		/* Unemployment */
  ProblemTable[6] = GetFire();			/* Fire */
  VoteProblems();
  for (z = 0; z < PROBNUM; z++)
    ProblemTaken[z] = 0;
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
    }
    else {
      ProblemOrder[z] = 7;
      ProblemTable[7] = 0;
    }
  }
}


/* comefrom: DoProblems */
VoteProblems(void)
{
  register x, z, count;

  for (z = 0; z < PROBNUM; z++)
    ProblemVotes[z] = 0;

  x = 0;
  z = 0;
  count = 0;
  while ((z < 100) && (count < 600)) {
    if (Rand(300) < ProblemTable[x]) {
      ProblemVotes[x]++;
      z++;
    }
    x++;
    if (x > PROBNUM) x = 0;
    count++;
  }
}


/* comefrom: DoProblems */
AverageTrf(void)
{
  QUAD TrfTotal;
  register short x, y, count;

  TrfTotal = 0;
  count = 1;
  for (x=0; x < HWLDX; x++)
    for (y=0; y < HWLDY; y++)	
      if (LandValueMem[x][y]) {
	TrfTotal += TrfDensity[x][y];
	count++;
      }

  TrafficAverage = (TrfTotal / count) * 2.4;
  return(TrafficAverage);
}


/* comefrom: DoProblems */
GetUnemployment(void)
{
  float r;
  short b;

  b = (ComPop + IndPop) << 3;
  if (b)
    r = ((float)ResPop) / b;
  else
    return(0);

  b = (r - 1) * 255;
  if (b > 255)
    b = 255;
  return (b);
}


/* comefrom: DoProblems GetScore */
GetFire(void)
{
  short z;

  z = FirePop * 5;
  if (z > 255)
    return(255);
  else
    return(z);
}


/* comefrom: CityEvaluation */
GetScore(void)
{
  register x, z;
  short OldCityScore;
  float SM, TM;

  OldCityScore = CityScore;
  x = 0;
  for (z = 0; z < 7; z++)
    x += ProblemTable[z];	/* add 7 probs */

  x = x / 3;			/* 7 + 2 average */
  if (x > 256) x = 256;

  z = (256 - x) * 4;
  if (z > 1000) z = 1000;
  if (z < 0 ) z = 0;

  if (ResCap) z = z * .85;
  if (ComCap) z = z * .85;
  if (IndCap) z = z * .85;
  if (RoadEffect < 32)  z = z - (32 - RoadEffect);
  if (PoliceEffect < 1000) z = z * (.9 + (PoliceEffect / 10000.1));
  if (FireEffect < 1000) z = z * (.9 + (FireEffect / 10000.1));
  if (RValve < -1000) z = z * .85;
  if (CValve < -1000) z = z * .85;
  if (IValve < -1000) z = z * .85;

  SM = 1.0;
  if ((CityPop == 0) || (deltaCityPop == 0))
    SM = 1.0;
  else if (deltaCityPop == CityPop)
    SM = 1.0;
  else if (deltaCityPop > 0)
    SM = ((float)deltaCityPop/CityPop) + 1.0;
  else if (deltaCityPop < 0)  
    SM = .95 + ((float) deltaCityPop/(CityPop - deltaCityPop));
  z = z * SM;
  z = z - GetFire();		/* dec score for fires */
  z = z - (CityTax);

  TM = unPwrdZCnt + PwrdZCnt;	/* dec score for unpowered zones */
  if (TM) SM = PwrdZCnt / TM;
  else SM = 1.0;
  z = z * SM; 

  if (z > 1000) z = 1000;
  if (z < 0 ) z = 0;

  CityScore = (CityScore + z) / 2; 

  deltaCityScore = CityScore - OldCityScore;
}


/* comefrom: CityEvaluation */
DoVotes(void)
{
  register z;

  CityYes = 0;
  CityNo = 0;
  for (z = 0; z < 100; z++) {
    if (Rand(1000) < CityScore)
      CityYes++;
    else
      CityNo++;
  }
}
