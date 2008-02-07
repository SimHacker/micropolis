/* w_budget.c
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


float roadPercent = 0.0;
float policePercent = 0.0;
float firePercent = 0.0;
QUAD roadValue;
QUAD policeValue;
QUAD fireValue;
QUAD roadMaxValue;
QUAD policeMaxValue;
QUAD fireMaxValue;
int MustDrawCurrPercents = 0;
int MustDrawBudgetWindow = 0;
int SetBudget(char *flowStr, char *previousStr,
	      char *currentStr, char *collectedStr, short tax);


void InitFundingLevel(void)
{
  firePercent = 1.0;		/* 1.0 */
  fireValue = 0;
  policePercent = 1.0;		/* 1.0 */
  policeValue = 0;
  roadPercent = 1.0;		/* 1.0 */
  roadValue = 0;
  drawBudgetWindow();
  drawCurrPercents();
}


DoBudget()
{
  DoBudgetNow(0);
}


DoBudgetFromMenu()
{
  DoBudgetNow(1);
}


DoBudgetNow(int fromMenu)
{
  QUAD yumDuckets;
  QUAD total;
  QUAD moreDough;
  QUAD fireInt, policeInt, roadInt;

  fireInt = (int)(((float)FireFund) * firePercent);
  policeInt = (int)(((float)PoliceFund) * policePercent);
  roadInt = (int)(((float)RoadFund) * roadPercent);

  total = fireInt + policeInt + roadInt;

  yumDuckets = TaxFund + TotalFunds;

  if (yumDuckets > total) {
    fireValue = fireInt;
    policeValue = policeInt;
    roadValue = roadInt;
  } else if (total > 0) {
    if (yumDuckets > roadInt) {
      roadValue = roadInt;
      yumDuckets -= roadInt;

      if (yumDuckets > fireInt) {
	fireValue = fireInt;
	yumDuckets -= fireInt;

	if (yumDuckets > policeInt) {
	  policeValue = policeInt;
	  yumDuckets -= policeInt;
	} else {
	  policeValue = yumDuckets;
	  if (yumDuckets > 0)
	    policePercent = ((float)yumDuckets) / ((float)PoliceFund);
	  else
	    policePercent = 0.0;
	}
      } else {
	fireValue = yumDuckets;
	policeValue = 0;
	policePercent = 0.0;
	if (yumDuckets > 0)
	  firePercent = ((float)yumDuckets) / ((float)FireFund);
	else
	  firePercent = 0.0;
      }
    } else {
      roadValue = yumDuckets;
      if (yumDuckets > 0)
	roadPercent = ((float)yumDuckets) / ((float)RoadFund);
      else
	roadPercent = 0.0;

      fireValue = 0;
      policeValue = 0;
      firePercent = 0.0;
      policePercent = 0.0;
    }
  } else {
    fireValue = 0;
    policeValue = 0;
    roadValue = 0;
    firePercent = 1.0;
    policePercent = 1.0;
    roadPercent = 1.0;
  }

  fireMaxValue = FireFund;
  policeMaxValue = PoliceFund;
  roadMaxValue = RoadFund;

  drawCurrPercents();

 noMoney:	
  if ((!autoBudget) || fromMenu) {
    if (!autoBudget) {
      /* TODO: append the the current year to the budget string */
    }

    ShowBudgetWindowAndStartWaiting();

    if (!fromMenu) {
      FireSpend = fireValue;
      PoliceSpend = policeValue;
      RoadSpend = roadValue;

      total = FireSpend + PoliceSpend + RoadSpend;
      moreDough = (QUAD)(TaxFund - total);
      Spend(-moreDough);
    }
    drawBudgetWindow();
    drawCurrPercents();
    DoUpdateHeads();

  } else { /* autoBudget & !fromMenu */
    if ((yumDuckets) > total) {
      moreDough = (QUAD)(TaxFund - total);
      Spend(-moreDough);
      FireSpend = FireFund;
      PoliceSpend = PoliceFund;
      RoadSpend = RoadFund;
      drawBudgetWindow();
      drawCurrPercents();
      DoUpdateHeads();
    } else {
      autoBudget = 0; /* XXX: force autobudget */
      MustUpdateOptions = 1;
      ClearMes();
      SendMes(29);
      goto noMoney;
    }
  }
}


drawBudgetWindow(void)
{
  MustDrawBudgetWindow = 1;
}


ReallyDrawBudgetWindow(void)
{
  short cashFlow, cashFlow2;
  char numStr[256], dollarStr[256], collectedStr[256],
       flowStr[256], previousStr[256], currentStr[256];

  cashFlow = TaxFund - fireValue - policeValue - roadValue;
	
  cashFlow2 = cashFlow;
  if (cashFlow < 0)   {
    cashFlow = -cashFlow;
    sprintf(numStr, "%d", cashFlow);
    makeDollarDecimalStr(numStr, dollarStr);
    sprintf(flowStr, "-%s", dollarStr);
  } else {
    sprintf(numStr, "%d", cashFlow);
    makeDollarDecimalStr(numStr, dollarStr);
    sprintf(flowStr, "+%s", dollarStr);
  }

  sprintf(numStr, "%d", TotalFunds);
  makeDollarDecimalStr(numStr, previousStr);

  sprintf(numStr, "%d", cashFlow2 + TotalFunds);
  makeDollarDecimalStr(numStr, currentStr);

  sprintf(numStr, "%d", TaxFund);
  makeDollarDecimalStr(numStr, collectedStr);

  SetBudget(flowStr, previousStr, currentStr, collectedStr, CityTax);
}


drawCurrPercents(void)
{
  MustDrawCurrPercents = 1;
}


ReallyDrawCurrPercents(void)
{
  char num[256];
  char fireWant[256], policeWant[256], roadWant[256];
  char fireGot[256], policeGot[256], roadGot[256];

  sprintf(num, "%d", fireMaxValue);
  makeDollarDecimalStr(num, fireWant);

  sprintf(num, "%d", policeMaxValue);
  makeDollarDecimalStr(num, policeWant);

  sprintf(num, "%d", roadMaxValue);
  makeDollarDecimalStr(num, roadWant);

  sprintf(num, "%d", (int)(fireMaxValue * firePercent));
  makeDollarDecimalStr(num, fireGot);

  sprintf(num, "%d", (int)(policeMaxValue * policePercent));
  makeDollarDecimalStr(num, policeGot);

  sprintf(num, "%d", (int)(roadMaxValue * roadPercent));
  makeDollarDecimalStr(num, roadGot);

  SetBudgetValues(roadGot, roadWant, 
		  policeGot, policeWant, 
		  fireGot, fireWant);
}


UpdateBudgetWindow()
{
  if (MustDrawCurrPercents) {
    ReallyDrawCurrPercents();
    MustDrawCurrPercents = 0;
  }
  if (MustDrawBudgetWindow) {
    ReallyDrawBudgetWindow();
    MustDrawBudgetWindow = 0;
  }
}


UpdateBudget()
{
  drawCurrPercents();
  drawBudgetWindow();
  Eval("UIUpdateBudget");
}


ShowBudgetWindowAndStartWaiting()
{
  Eval("UIShowBudgetAndWait");

  Pause();
}


SetBudget(char *flowStr, char *previousStr,
	  char *currentStr, char *collectedStr, short tax)
{
  char buf[256];

  sprintf(buf, "UISetBudget {%s} {%s} {%s} {%s} {%d}",
	  flowStr, previousStr, currentStr, collectedStr, tax);
  Eval(buf);
}


SetBudgetValues(char *roadGot, char *roadWant,
		char *policeGot, char *policeWant,
		char *fireGot, char *fireWant)
{
  char buf[256];

  sprintf(buf, "UISetBudgetValues {%s} {%s} %d {%s} {%s} %d {%s} {%s} %d",
	  roadGot, roadWant, (int)(roadPercent * 100),
	  policeGot, policeWant, (int)(policePercent * 100),
	  fireGot, fireWant, (int)(firePercent * 100));
  Eval(buf);
}


