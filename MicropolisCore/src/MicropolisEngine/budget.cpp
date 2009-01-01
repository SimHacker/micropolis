/* budget.cpp
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

/** @file budget.cpp */

////////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "text.h"


////////////////////////////////////////////////////////////////////////


void Micropolis::InitFundingLevel()
{
  firePercent = 1.0;            /* 1.0 */
  fireValue = 0;
  policePercent = 1.0;          /* 1.0 */
  policeValue = 0;
  roadPercent = 1.0;            /* 1.0 */
  roadValue = 0;
  drawBudgetWindow();
  drawCurrPercents();
}


void Micropolis::DoBudget()
{
  DoBudgetNow(0);
}


void Micropolis::DoBudgetFromMenu()
{
  DoBudgetNow(1);
}


void Micropolis::DoBudgetNow(
  int fromMenu)
{
  Quad fireInt   = (int)(FireFund   * firePercent);
  Quad policeInt = (int)(PoliceFund * policePercent);
  Quad roadInt   = (int)(RoadFund   * roadPercent);

  Quad total = fireInt + policeInt + roadInt;

  Quad yumDuckets = TaxFund + TotalFunds;

  if (yumDuckets > total) {

        // Enough yumDuckets to fully fund fire, police and road.

    fireValue = fireInt;
    policeValue = policeInt;
    roadValue = roadInt;

        // FIXME: Why are we not subtracting from yumDuckets what we
        // spend, like the code below is doing?

  } else if (total > 0) {

        assert(yumDuckets <= total);

        // Not enough yumDuckets to fund everything. 
        // First spend on roads, then on fire, then on police.

    if (yumDuckets > roadInt) {

          // Enough yumDuckets to fully fund roads.

      roadValue = roadInt;
      yumDuckets -= roadInt;

      if (yumDuckets > fireInt) {

                // Enough yumDuckets to fully fund fire. 

        fireValue = fireInt;
        yumDuckets -= fireInt;

        if (yumDuckets > policeInt) {

                  // Enough yumDuckets to fully fund police. 
                  // Hey what are we doing here? Should never get here. 
                  // We tested for yumDuckets > total above
                  // (where total = fireInt + policeInt + roadInt), 
                  // so this should never happen.

          policeValue = policeInt;
          yumDuckets -= policeInt;

        } else {

                  // Fuly funded roads and fire. 
                  // Partially fund police. 

          policeValue = yumDuckets;

          if (yumDuckets > 0) {

                        // Scale back police percentage to available cash.

            policePercent =
              ((float)yumDuckets) / ((float)PoliceFund);

          } else {

                        // Exactly nothing left, so scale back police percentage to zero.

            policePercent = 0.0;

          }
        }
      } else {

                // Not enough yumDuckets to fully fund fire. 

        fireValue = yumDuckets;

                // No police after funding roads and fire.

        policeValue = 0;
        policePercent = 0.0;

        if (yumDuckets > 0) {

                  // Scale back fire percentage to available cash.

          firePercent =
            ((float)yumDuckets) / ((float)FireFund);

        } else {

                  // Exactly nothing left, so scale back fire percentage to zero.

          firePercent = 0.0;

        }

      }

    } else {

          // Not enough yumDuckets to fully fund roads.

      roadValue = yumDuckets;

          // No fire or police after funding roads.

      fireValue = 0;
      policeValue = 0;
      firePercent = 0.0;
      policePercent = 0.0;

      if (yumDuckets > 0) {

                // Scale back road percentage to available cash.

        roadPercent =
          ((float)yumDuckets) / ((float)RoadFund);

      } else {

                // Exactly nothing left, so scale back road percentage to zero.

        roadPercent = 0.0;

      }

    }

  } else {

        assert(yumDuckets == total);
        assert(total == 0);

        // Zero funding, so no values but full percentages.

    fireValue = 0;
    policeValue = 0;
    roadValue = 0;
    firePercent = 1.0;
    policePercent = 1.0;
    roadPercent = 1.0;

  }

  drawCurrPercents();

noMoney:

  if (!autoBudget || fromMenu) {

    // FIXME: This might have blocked on the Mac, but now it's asynchronous.
        // Make sure the stuff we do just afterwards is intended to be done immediately
        // and is not supposed to wait until after the budget dialog is dismissed.
    // Otherwise don't do it after this and arrange for it to happen when the
    // modal budget dialog is dismissed.
    ShowBudgetWindowAndStartWaiting();

        // FIXME: Only do this AFTER the budget window is accepted.

    if (!fromMenu) {

      FireSpend = (short)fireValue;
      PoliceSpend = (short)policeValue;
      RoadSpend = (short)roadValue;

      total =
        FireSpend + PoliceSpend + RoadSpend;

      Quad moreDough =
        (Quad)(TaxFund - total);
      Spend(-moreDough);

    }

    drawBudgetWindow();
    drawCurrPercents();
    DoUpdateHeads();

  } else { /* autoBudget & !fromMenu */

        // FIXME: Not sure yumDuckets is the right value here. It gets the
        // amount spent subtracted from it above in some cases, but not if
        // we are fully funded. I think we want to use the original value
        // of yumDuckets, which is TaxFund + TotalFunds.

    if (yumDuckets > total) {

      Quad moreDough =
        (Quad)(TaxFund - total);
      Spend(-moreDough);

      FireSpend = FireFund;
      PoliceSpend = PoliceFund;
      RoadSpend = RoadFund;

      drawBudgetWindow();
      drawCurrPercents();
      DoUpdateHeads();

    } else {

      autoBudget = false; /* force autobudget */
      MustUpdateOptions = 1;
      ClearMes();
      SendMes(STR301_NO_MONEY);
      goto noMoney;

    }

  }

}


void Micropolis::drawBudgetWindow()
{
  MustDrawBudgetWindow = 1;
}


// TODO: The scripting language should pull these raw values out and format them,
// instead of the simulator core formatting them and pushing them out.
void Micropolis::ReallyDrawBudgetWindow()
{
  short cashFlow, cashFlow2;
  char numStr[256], dollarStr[256], collectedStr[256],
       flowStr[256], previousStr[256], currentStr[256];

  cashFlow =
    (short)(TaxFund - fireValue - policeValue - roadValue);

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

  sprintf(numStr, "%d", (int)TotalFunds);
  makeDollarDecimalStr(numStr, previousStr);

  sprintf(numStr, "%d", (int)(cashFlow2 + TotalFunds));
  makeDollarDecimalStr(numStr, currentStr);

  sprintf(numStr, "%d", (int)TaxFund);
  makeDollarDecimalStr(numStr, collectedStr);

  SetBudget(
    flowStr,
    previousStr,
    currentStr,
    collectedStr,
    CityTax);

}


void Micropolis::drawCurrPercents()
{
  MustDrawCurrPercents = 1;
}


// TODO: The scripting language should pull these raw values out and format them,
// instead of the simulator core formatting them and pushing them out.
void Micropolis::ReallyDrawCurrPercents()
{
  char num[256];
  char fireWant[256], policeWant[256], roadWant[256];
  char fireGot[256], policeGot[256], roadGot[256];

  sprintf(num, "%d", (int)FireFund);
  makeDollarDecimalStr(num, fireWant);

  sprintf(num, "%d", (int)PoliceFund);
  makeDollarDecimalStr(num, policeWant);

  sprintf(num, "%d", (int)RoadFund);
  makeDollarDecimalStr(num, roadWant);

  sprintf(num, "%d", (int)(FireFund * firePercent));
  makeDollarDecimalStr(num, fireGot);

  sprintf(num, "%d", (int)(PoliceFund * policePercent));
  makeDollarDecimalStr(num, policeGot);

  sprintf(num, "%d", (int)(RoadFund * roadPercent));
  makeDollarDecimalStr(num, roadGot);

  SetBudgetValues(
    roadGot,
    roadWant,
    policeGot,
    policeWant,
    fireGot,
    fireWant);

}


// TODO: The scripting language should pull these raw values out and format them,
// instead of the simulator core formatting them and pushing them out.
void Micropolis::UpdateBudgetWindow()
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


void Micropolis::UpdateBudget()
{
  drawCurrPercents();
  drawBudgetWindow();

  Callback("UIUpdateBudget", "");
}


void Micropolis::ShowBudgetWindowAndStartWaiting()
{
  Callback("UIShowBudgetAndWait", "");

  Pause();
}


void Micropolis::SetBudget(
  char *flowStr,
  char *previousStr,
  char *currentStr,
  char *collectedStr,
  short tax)
{
  Callback(
    "UISetBudget",
        "ssssd",
    flowStr,
    previousStr,
    currentStr,
    collectedStr,
    (int)tax);
}


void Micropolis::SetBudgetValues(
  char *roadGot,
  char *roadWant,
  char *policeGot,
  char *policeWant,
  char *fireGot,
  char *fireWant)
{
  Callback(
    "UISetBudgetValues",
    "ssdssdssd",
    roadGot,
    roadWant,
    (int)(roadPercent * 100),
    policeGot,
    policeWant,
    (int)(policePercent * 100),
    fireGot,
    fireWant,
    (int)(firePercent * 100));
}


////////////////////////////////////////////////////////////////////////
