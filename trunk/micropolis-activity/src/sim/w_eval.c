/* w_eval.c
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


char *cityClassStr[6] = {
  "VILLAGE", "TOWN", "CITY", "CAPITAL", "METROPOLIS", "MEGALOPOLIS"
};

char *cityLevelStr[3] = {
  "Easy", "Medium", "Hard"
};

char *probStr[10] = {
  "CRIME", "POLLUTION", "HOUSING COSTS", "TAXES",
  "TRAFFIC", "UNEMPLOYMENT", "FIRES"
};


/* comefrom: DoSubUpDate scoreDoer */
doScoreCard(void)
{
  char title[256],
  goodyes[32], goodno[32],
  prob0[32], prob1[32], prob2[32], prob3[32],
  pop[32], delta[32], assessed[32], assessed_dollars[32],
  score[32], changed[32];

  /* send /EvaluationTitle /SetValue [ (titleStr year) ] */
  /* send /GoodJobPercents /SetValue [ (CityYes%) (CityNo%) ] */
  /* send /WorstProblemPercents /SetValue [ (ProblemVotes[0]%) ... ] */
  /* send /WorstProblemNames /SetValue [ (probStr[ProblemOrder[0]) ... ] */
  /*	put ^chars around first problem name to make it bold */
  /* send /Statistics /SetValue
	  [ (CityPop) (deltaCityPop) () (CityAssValue)
	    (cityClassStr[CityClass]) (cityLevelStr[GameLevel]) ] */
  /* send /CurrentScore /SetValue [ (CityScore) ] */
  /* send /AnnualChange /SetValue [ (deltaCityScore) ] */

  sprintf(title, "City Evaluation  %d", CurrentYear());
  sprintf(goodyes, "%d%%", CityYes);
  sprintf(goodno, "%d%%", CityNo);
  sprintf(prob0, "%d%%", ProblemVotes[ProblemOrder[0]]);
  sprintf(prob1, "%d%%", ProblemVotes[ProblemOrder[1]]);
  sprintf(prob2, "%d%%", ProblemVotes[ProblemOrder[2]]);
  sprintf(prob3, "%d%%", ProblemVotes[ProblemOrder[3]]);
  sprintf(pop, "%d", CityPop);
  sprintf(delta, "%d", deltaCityPop);
  sprintf(assessed, "%d", CityAssValue);
  makeDollarDecimalStr(assessed, assessed_dollars);

  sprintf(score, "%d", CityScore);
  sprintf(changed, "%d", deltaCityScore);

  SetEvaluation(changed, score,
		ProblemVotes[ProblemOrder[0]] ? probStr[ProblemOrder[0]] : " ",
		ProblemVotes[ProblemOrder[1]] ? probStr[ProblemOrder[1]] : " ",
		ProblemVotes[ProblemOrder[2]] ? probStr[ProblemOrder[2]] : " ",
		ProblemVotes[ProblemOrder[3]] ? probStr[ProblemOrder[3]] : " ",
		ProblemVotes[ProblemOrder[0]] ? prob0 : " ",
		ProblemVotes[ProblemOrder[1]] ? prob1 : " ",
		ProblemVotes[ProblemOrder[2]] ? prob2 : " ",
		ProblemVotes[ProblemOrder[3]] ? prob3 : " ",
		pop, delta, assessed_dollars,
		cityClassStr[CityClass], cityLevelStr[GameLevel],
		goodyes, goodno, title);
}


ChangeEval()
{
  EvalChanged = 1;
}


scoreDoer(void)
{
  if (EvalChanged) {
    doScoreCard();
    EvalChanged = 0;
  }
}


SetEvaluation(char *changed, char *score,
	      char *ps0, char *ps1, char *ps2, char *ps3,
	      char *pv0, char *pv1, char *pv2, char *pv3,
	      char *pop, char *delta, char *assessed_dollars,
	      char *cityclass, char *citylevel,
	      char *goodyes, char *goodno, char *title)
{
  char buf[2048];

  sprintf(buf, "UISetEvaluation {%s} {%s} {%s} {%s} {%s} {%s} {%s} {%s} {%s} {%s} {%s} {%s} {%s} {%s} {%s} {%s} {%s} {%s}",
	  changed, score,
	  ps0, ps1, ps2, ps3,
	  pv0, pv1, pv2, pv3,
	  pop, delta, assessed_dollars,
	  cityclass, citylevel,
	  goodyes, goodno, title);
  Eval(buf);
}


