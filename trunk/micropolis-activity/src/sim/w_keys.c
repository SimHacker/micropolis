/* w_keys.c
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


static char LastKeys[5];


/* TODO:
  <n> new city
  <l> load city
  <s> save city
  <q> quit
  <a> autobulldoze
  <0><1><2><3>... game speed
  <m> map window
  <g> graph window
  <b> budget window
  <e> evaluation window
*/


ResetLastKeys()
{
  LastKeys[0] = ' ';
  LastKeys[1] = ' ';
  LastKeys[2] = ' ';
  LastKeys[3] = ' ';
  LastKeys[4] = '\0';
  PunishCnt = 0;
}


/* comefrom: processEvent */
doKeyDown(SimView *view, short charCode)
{
  LastKeys[0] = LastKeys[1];
  LastKeys[1] = LastKeys[2];
  LastKeys[2] = LastKeys[3];
  LastKeys[3] = tolower(charCode);

  if (strcmp(LastKeys, "fund") == 0) {
    Spend(-10000);
    PunishCnt++;				/* punish for cheating */
    if (PunishCnt == 5) {
      PunishCnt = 0;
      MakeEarthquake();
    }
    LastKeys[0] = '\0';
  } else if (strcmp(LastKeys, "fart") == 0) {
    MakeSound("city", "Explosion-High");
    MakeSound("city", "Explosion-Low");
    MakeFire();
    MakeFlood();
    MakeTornado();
    MakeEarthquake();
    MakeMonster();
    LastKeys[0] = '\0';
  } else if (strcmp(LastKeys, "nuke") == 0) {
    int i, j;
    MakeSound("city", "Explosion-High");
    MakeSound("city", "Explosion-Low");
    for (i = 0; i < WORLD_X; i++) {
      for (j = 0; j < WORLD_Y; j++) {
	short tile = Map[i][j] & LOMASK;
	if ((tile >= RUBBLE) &&
	    ((tile < CHURCH - 4) ||
	     (tile > CHURCH + 4))) {
	  if ((tile >= HBRIDGE && tile <= VBRIDGE) ||
	      (tile >= BRWH && tile <= LTRFBASE + 1) ||
	      (tile >= BRWV && tile <= BRWV + 2) ||
	      (tile >= BRWXXX1 && tile <= BRWXXX1 + 2) ||
	      (tile >= BRWXXX2 && tile <= BRWXXX2 + 2) ||
	      (tile >= BRWXXX3 && tile <= BRWXXX3 + 2) ||
	      (tile >= BRWXXX4 && tile <= BRWXXX4 + 2) ||
	      (tile >= BRWXXX5 && tile <= BRWXXX5 + 2) ||
	      (tile >= BRWXXX6 && tile <= BRWXXX6 + 2) ||
	      (tile >= BRWXXX7 && tile <= BRWXXX7 + 2)) {
	    Map[i][j] = RIVER;
	  } else {
	    Map[i][j] = TINYEXP + ANIMBIT + BULLBIT + Rand(2);
	  }
	}
      }
    }
    LastKeys[0] = '\0';
  } else if (strcmp(LastKeys, "stop") == 0) {
    heat_steps = 0;
    LastKeys[0] = '\0';
    Kick();
  } else if (strcmp(LastKeys, "will") == 0) {
    int i;
    int n = 500;
    for (i = 0; i < n; i++) {
      int x1 = Rand(WORLD_X - 1);
      int y1 = Rand(WORLD_Y - 1);
      int x2 = Rand(WORLD_X - 1);
      int y2 = Rand(WORLD_Y - 1);
      short temp =
	Map[x1][y1];
      Map[x1][y1] =
	Map[x2][y2];
      Map[x2][y2] =
	temp;
    }
    Kick();
  } else if (strcmp(LastKeys, "bobo") == 0) {
    heat_steps = 1;
    heat_flow = -1;
    heat_rule = 0;
    LastKeys[0] = '\0';
    Kick();
  } else if (strcmp(LastKeys, "boss") == 0) {
    heat_steps = 1;
    heat_flow = 1;
    heat_rule = 0;
    LastKeys[0] = '\0';
    Kick();
  } else if (strcmp(LastKeys, "mack") == 0) {
    heat_steps = 1;
    heat_flow = 0;
    heat_rule = 0;
    LastKeys[0] = '\0';
    Kick();
  } else if (strcmp(LastKeys, "donh") == 0) {
    heat_steps = 1;
    heat_flow = -1;
    heat_rule = 1;
    LastKeys[0] = '\0';
    Kick();
  } else if (strcmp(LastKeys, "patb") == 0) {
    heat_steps = 1;
    heat_flow = Rand(40) - 20;
    heat_rule = 0;
    LastKeys[0] = '\0';
    Kick();
  } else if (strcmp(LastKeys, "lucb") == 0) {
    heat_steps = 1;
    heat_flow = Rand(1000) - 500;
    heat_rule = 0;
    LastKeys[0] = '\0';
    Kick();
  } else if (strcmp(LastKeys, "olpc") == 0) {
    Spend(-1000000);
  }

  switch (charCode) {
    
    case 'X':
    case 'x': {
      short s = view->tool_state;
      if (++s > lastState) {
	s = firstState;
      }
      setWandState(view, s);
      break;
    }

    case 'Z':
    case 'z': {
      short s = view->tool_state;
      if (--s < firstState) {
	  s = lastState;
      }
      setWandState(view, s);
      break;
    }

    /***** shift wand state to bull dozer *****/
    case 'B':
    case 'b':
    case 'B'-'@': {
      if (view->tool_state_save == -1) {
	view->tool_state_save = view->tool_state;
      }
      setWandState(view, dozeState);
      break;
    }

    /***** shift wand state to roads *****/
    case 'R':
    case 'r':
    case 'R'-'@': {
      if (view->tool_state_save == -1) {
	view->tool_state_save = view->tool_state;
      }
      setWandState(view, roadState);
      break;
    }

    /***** shift wand state to power *****/
    case 'P':
    case 'p':
    case 'P'-'@': {
      if (view->tool_state_save == -1) {
	view->tool_state_save = view->tool_state;
      }
      setWandState(view, wireState);
      break;
    }

    /***** shift wand state to transit *****/
    case 'T':
    case 't':
    case 'T'-'@': {
      if (view->tool_state_save == -1) {
	view->tool_state_save = view->tool_state;
      }
      setWandState(view, rrState);
      break;
    }

#if 0
    /***** shift wand state to query *****/
    case 'Q':
    case 'q':
    case 'Q'-'@': {
      if (view->tool_state_save == -1)
	view->tool_state_save = view->tool_state;
      setWandState(view, queryState);
      break;
    }
#endif

    case 27: {
      SoundOff();
      break;
    }

  }
}


/* comefrom: processEvent */
doKeyUp(SimView *view, short charCode)
{
  switch(charCode) {

    case 'b':
    case 'B':
    case 'B'-'@':
    case 'R':
    case 'r':
    case 'R'-'@':
    case 'P':
    case 'p':
    case 'P'-'@':
    case 'T':
    case 't':
    case 'T'-'@':
    case 'q':
    case 'Q':
    case 'Q'-'@': {
      if (view->tool_state_save != -1) {
	setWandState(view, view->tool_state_save);
      }
      view->tool_state_save = -1;
      break;
    }

    default: {
      break;
    }

  }
}
