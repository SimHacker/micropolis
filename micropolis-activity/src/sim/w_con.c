/* w_con.c
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


short _RoadTable[16] = {
  66, 67, 66, 68,
  67, 67, 69, 73,
  66, 71, 66, 72,
  70, 75, 74, 76
};

short _RailTable[16] = {
  226, 227, 226, 228,
  227, 227, 229, 233,
  226, 231, 226, 232,
  230, 235, 234, 236
};

short _WireTable[16] = {
  210, 211, 210, 212,
  211, 211, 213, 217,
  210, 215, 210, 216,
  214, 219, 218, 220
};


#define NeutralizeRoad(Tile) \
  if (((Tile &= LOMASK) >= 64) && \
      ((Tile & LOMASK) <= 207)) { \
    Tile = (Tile & 0x000F) + 64; \
  }


/* comefrom: check3Border check4Border check5Border processWand */
int
ConnecTile(short x, short y, short *TileAdrPtr, short Command)
{
  short Tile;
  int result = 1;

  /* make sure the array subscripts are in bounds */
  if (!TestBounds(x, y)) {
    return (0);
  }

  /* AutoDoze */
  if ((Command >= 2) && (Command <= 4)) {

    if ((autoBulldoze != 0) &&
	(TotalFunds > 0) &&
	((Tile = (*TileAdrPtr)) & BULLBIT)) {
      NeutralizeRoad(Tile);
      /* Maybe this should check BULLBIT instead of checking tile values? */
      if (((Tile >= TINYEXP) && (Tile <= LASTTINYEXP)) ||
	  ((Tile < 64) && (Tile != 0))) {
	Spend(1);
	(*TileAdrPtr) = 0;
      }
    }
  }

  switch (Command) {

  case 0:	/* Fix zone */
    _FixZone(x, y, TileAdrPtr);
    break;
    
  case 1:	/* Doze zone */
    result = _LayDoze(x, y, TileAdrPtr);
    _FixZone(x, y, TileAdrPtr);
    break;
    
  case 2:	/* Lay Road */
    result = _LayRoad(x, y, TileAdrPtr);
    _FixZone(x, y, TileAdrPtr);
    break;
    
  case 3:	/* Lay Rail */
    result = _LayRail(x, y, TileAdrPtr);
    _FixZone(x, y, TileAdrPtr);
    break;
    
  case 4:	/* Lay Wire */
    result = _LayWire(x, y, TileAdrPtr);
    _FixZone(x, y, TileAdrPtr);
    break;

  }
  
  return result;
}

/* comefrom: ConnecTile */
int
_LayDoze(int x, int y, short *TileAdrPtr)
{
  short Tile;

  if (!(TotalFunds)) {
    return -2;			/* no mas dinero. */
  }

  Tile = (*TileAdrPtr);

  if (!(Tile & BULLBIT)) {
    return 0;			/* Check dozeable bit. */
  }

  NeutralizeRoad(Tile);

  switch (Tile) {
  case HBRIDGE:
  case VBRIDGE:
  case BRWV:
  case BRWH:
  case HBRDG0:
  case HBRDG1:
  case HBRDG2:
  case HBRDG3:
  case VBRDG0:
  case VBRDG1:
  case VBRDG2:
  case VBRDG3:
  case HPOWER:
  case VPOWER:
  case HRAIL:
  case VRAIL:		/* Dozing over water, replace with water. */
    (*TileAdrPtr) = RIVER;
    break;

  default:		/* Dozing on land, replace with land.  Simple, eh? */
    (*TileAdrPtr) = DIRT;
    break;
  }

  Spend(1);			/* Costs $1.00....*/
  return 1;
}


/* comefrom: ConnecTile */
int
_LayRoad(int x, int y, short *TileAdrPtr)
{
  short Tile;
  int cost = 10;

  if (TotalFunds < 10) {
    return -2;
  }

  Tile = (*TileAdrPtr) & LOMASK;

  switch (Tile) {

  case DIRT:
    (*TileAdrPtr) = ROADS | BULLBIT | BURNBIT;
    break;
			
  case RIVER:			/* Road on Water */
  case REDGE:
  case CHANNEL:			/* Check how to build bridges, if possible. */
    if (TotalFunds < 50) {
      return -2;
    }

    cost = 50;

    if (x < (WORLD_X - 1)) {
      Tile = TileAdrPtr[WORLD_Y];
      NeutralizeRoad(Tile);
      if ((Tile == VRAILROAD) ||
	  (Tile == HBRIDGE) ||
	  ((Tile >= ROADS) &&
	   (Tile <= HROADPOWER))) {
	(*TileAdrPtr) = HBRIDGE | BULLBIT;
	break;
      }
    }

    if (x > 0) {
      Tile = TileAdrPtr[-WORLD_Y];
      NeutralizeRoad(Tile);
      if ((Tile == VRAILROAD) ||
	  (Tile == HBRIDGE) ||
	  ((Tile >= ROADS) &&
	   (Tile <= INTERSECTION))) {
	(*TileAdrPtr) = HBRIDGE | BULLBIT;
	break;
      }
    }

    if (y < (WORLD_Y - 1)) {
      Tile = TileAdrPtr[1];
      NeutralizeRoad(Tile);
      if ((Tile == HRAILROAD) ||
	  (Tile == VROADPOWER) ||
	  ((Tile >= VBRIDGE) &&
	   (Tile <= INTERSECTION))) {
	(*TileAdrPtr) = VBRIDGE | BULLBIT;
	break;
      }
    }

    if (y > 0) {
      Tile = TileAdrPtr[-1];
      NeutralizeRoad(Tile);
      if ((Tile == HRAILROAD) ||
	  (Tile == VROADPOWER) ||
	  ((Tile >= VBRIDGE) &&
	   (Tile <= INTERSECTION))) {
	(*TileAdrPtr) = VBRIDGE | BULLBIT;
	break;
      }
    }

    /* Can't do road... */
    return 0;

  case LHPOWER:		/* Road on power */
    (*TileAdrPtr) = VROADPOWER | CONDBIT | BURNBIT | BULLBIT;
    break;

  case LVPOWER:		/* Road on power #2 */
    (*TileAdrPtr) = HROADPOWER | CONDBIT | BURNBIT | BULLBIT;
    break;

  case LHRAIL:		/* Road on rail */
    (*TileAdrPtr) = HRAILROAD | BURNBIT | BULLBIT;
    break;

  case LVRAIL:		/* Road on rail #2 */
    (*TileAdrPtr) = VRAILROAD | BURNBIT | BULLBIT;
    break;

  default:		/* Can't do road */
    return 0;

  }

  Spend(cost);

  return 1;
}


/* comefrom: ConnecTile */
int
_LayRail(int x, int y, short *TileAdrPtr)
{
  short Tile;
  int cost = 20;

  if (TotalFunds < 20) {
    return -2;
  }

  Tile = (*TileAdrPtr) & LOMASK;
  NeutralizeRoad(Tile);

  switch (Tile) {
  case 0:			/* Rail on Dirt */
    (*TileAdrPtr) = 226 | BULLBIT | BURNBIT;
    break;

  case 2:			/* Rail on Water */
  case 3:
  case 4:			/* Check how to build underwater tunnel, if possible. */
    if (TotalFunds < 100) {
      return -2;
    }
    cost = 100;

    if (x < (WORLD_X - 1)) {
      Tile = TileAdrPtr[WORLD_Y];
      NeutralizeRoad(Tile);
      if ((Tile == 221) || (Tile == 224) || ((Tile >= 226) && (Tile <= 237))) {
	(*TileAdrPtr) = 224 | BULLBIT;
	break;
      }
    }
    
    if (x > 0) {
      Tile = TileAdrPtr[-WORLD_Y];
      NeutralizeRoad(Tile);
      if ((Tile == 221) || (Tile == 224) || ((Tile > 225) && (Tile < 238))) {
	(*TileAdrPtr) = 224 | BULLBIT;
	break;
      }
    }

    if (y < (WORLD_Y - 1)) {
      Tile = TileAdrPtr[1];
      NeutralizeRoad(Tile);
      if ((Tile == 222) || (Tile == 238) || ((Tile > 224) && (Tile < 237))) {
	(*TileAdrPtr) = 225 | BULLBIT;
	break;
      }
    }

    if (y > 0) {
      Tile = TileAdrPtr[-1];
      NeutralizeRoad(Tile);
      if ((Tile == 222) || (Tile == 238) || ((Tile > 224) && (Tile < 237))) {
	(*TileAdrPtr) = 225 | BULLBIT;
	break;
      }
    }

    /* Can't do rail... */
    return 0;

  case 210:		/* Rail on power */
    (*TileAdrPtr) = 222 | CONDBIT | BURNBIT | BULLBIT;
    break;

  case 211:		/* Rail on power #2 */
    (*TileAdrPtr) = 221 | CONDBIT | BURNBIT | BULLBIT;
    break;

  case 66:		/* Rail on road */
    (*TileAdrPtr) = 238 | BURNBIT | BULLBIT;
    break;

  case 67:		/* Rail on road #2 */
    (*TileAdrPtr) = 237 | BURNBIT | BULLBIT;
    break;

  default:		/* Can't do rail */
    return 0;
  }

  Spend(cost);
  return 1;
}


/* comefrom: ConnecTile */
int
_LayWire(int x, int y, short *TileAdrPtr)
{
  short Tile;
  int cost = 5;

  if (TotalFunds < 5) {
    return -2;
  }

  Tile = (*TileAdrPtr) & LOMASK;
  NeutralizeRoad(Tile);

  switch (Tile) {
  case 0:			/* Wire on Dirt */
    (*TileAdrPtr) = 210 | CONDBIT | BURNBIT | BULLBIT;
    break;

  case 2:			/* Wire on Water */
  case 3:
  case 4:			/* Check how to lay underwater wire, if possible. */
    if (TotalFunds < 25)
      return -2;
    cost = 25;

    if (x < (WORLD_X - 1)) {
      Tile = TileAdrPtr[WORLD_Y];
      if (Tile & CONDBIT) {
	NeutralizeRoad(Tile);
	if ((Tile != 77) && (Tile != 221) && (Tile != 208)) {
	  (*TileAdrPtr) = 209 | CONDBIT | BULLBIT;
	  break;		
	}
      }
    }

    if (x > 0) {
      Tile = TileAdrPtr[-WORLD_Y];
      if (Tile & CONDBIT) {
	NeutralizeRoad(Tile);		
	if ((Tile != 77) && (Tile != 221) && (Tile != 208)) {
	  (*TileAdrPtr) = 209 | CONDBIT | BULLBIT;
	  break;		
	}
      }
    }

    if (y < (WORLD_Y - 1)) {
      Tile = TileAdrPtr[1];
      if (Tile & CONDBIT) {
	NeutralizeRoad(Tile);		
	if ((Tile != 78) && (Tile != 222) && (Tile != 209)) {
	  (*TileAdrPtr) = 208 | CONDBIT | BULLBIT;
	  break;
	}
      }
    }

    if (y > 0) {
      Tile = TileAdrPtr[-1];
      if (Tile & CONDBIT) {
	NeutralizeRoad(Tile);		
	if ((Tile != 78) && (Tile != 222) && (Tile != 209)) {
	  (*TileAdrPtr) = 208 | CONDBIT | BULLBIT;
	  break;		
	}
      }
    }

    /* Can't do wire... */
    return 0;

  case 66:		/* Wire on Road */
    (*TileAdrPtr) = 77 | CONDBIT | BURNBIT | BULLBIT;
    break;

  case 67:		/* Wire on Road #2 */
    (*TileAdrPtr) = 78 | CONDBIT | BURNBIT | BULLBIT;
			break;

  case 226:		/* Wire on rail */
    (*TileAdrPtr) = 221 | CONDBIT | BURNBIT | BULLBIT;
    break;

  case 227:		/* Wire on rail #2 */
    (*TileAdrPtr) = 222 | CONDBIT | BURNBIT | BULLBIT;
    break;

  default:		/* Can't do wire */
    return 0;
  }

  Spend(cost);
  return 1;
}


/* comefrom: ConnecTile */
_FixZone(int x, int y, short *TileAdrPtr)
{
  _FixSingle(x,y, &TileAdrPtr[0]);

  if (y > 0) {
    _FixSingle(x, y-1, &TileAdrPtr[-1]);
  }

  if (x < (WORLD_X - 1)) {
    _FixSingle(x+1, y, &TileAdrPtr[WORLD_Y]);
  }

  if (y < (WORLD_Y - 1)) {
    _FixSingle(x, y+1, &TileAdrPtr[1]);
  }

  if (x > 0) {
    _FixSingle(x-1, y, &TileAdrPtr[-WORLD_Y]);
  }

}


/* comefrom: _FixZone */
_FixSingle(int x, int y, short *TileAdrPtr)
{
  short Tile;
  short adjTile = 0;

  Tile = (*TileAdrPtr) & LOMASK;
  NeutralizeRoad(Tile);
  if ((Tile >= 66) && (Tile <= 76)) {		/* Cleanup Road */

    if (y > 0) {
      Tile = TileAdrPtr[-1];
      NeutralizeRoad(Tile);
      if (((Tile == 237) || ((Tile >= 64) && (Tile <= 78))) &&
	  (Tile != 77) && (Tile != 238) && (Tile != 64))
	adjTile |= 0x0001;
    }

    if (x < (WORLD_X - 1)) {
      Tile = TileAdrPtr[WORLD_Y];
      NeutralizeRoad(Tile);
      if (((Tile == 238) || ((Tile >= 64) && (Tile <= 78))) &&
	  (Tile != 78) && (Tile != 237) && (Tile != 65))
	adjTile |= 0x0002;
    }

    if (y < (WORLD_Y - 1)) {
      Tile = TileAdrPtr[1];
      NeutralizeRoad(Tile);
      if (((Tile == 237) || ((Tile >= 64) && (Tile <= 78))) &&
	  (Tile != 77) && (Tile != 238) && (Tile != 64))
	adjTile |= 0x0004;
    }

    if (x > 0) {
      Tile = TileAdrPtr[-WORLD_Y];
      NeutralizeRoad(Tile);
      if (((Tile == 238) || ((Tile >= 64) && (Tile <= 78))) &&
	  (Tile != 78) && (Tile != 237) && (Tile != 65))
	adjTile |= 0x0008;
    }

    (*TileAdrPtr) = _RoadTable[adjTile] | BULLBIT | BURNBIT;
    return;
  }

  if ((Tile >= 226) && (Tile <= 236)) {		/* Cleanup Rail */

    if (y > 0) {
      Tile = TileAdrPtr[-1];
      NeutralizeRoad(Tile);
      if ((Tile >= 221) && (Tile <= 238) &&
	  (Tile != 221) && (Tile != 237) && (Tile != 224))
	adjTile |= 0x0001;
    }

    if (x < (WORLD_X - 1)) {
      Tile = TileAdrPtr[WORLD_Y];
      NeutralizeRoad(Tile);
      if ((Tile >= 221) && (Tile <= 238) &&
	  (Tile != 222) && (Tile != 238) && (Tile != 225))
	adjTile |= 0x0002;
    }

    if (y < (WORLD_Y - 1)) {
      Tile = TileAdrPtr[1];
      NeutralizeRoad(Tile);
      if ((Tile >= 221) && (Tile <= 238) &&
	  (Tile != 221) && (Tile != 237) && (Tile != 224))
	adjTile |= 0x0004;
    }

    if (x > 0) {
      Tile = TileAdrPtr[-WORLD_Y];
      NeutralizeRoad(Tile);
      if ((Tile >= 221) && (Tile <= 238) &&
	  (Tile != 222) && (Tile != 238) && (Tile != 225))
	adjTile |= 0x0008;
    }

    (*TileAdrPtr) = _RailTable[adjTile] | BULLBIT | BURNBIT;
    return;
  }

  if ((Tile >= 210) && (Tile <= 220)) {		/* Cleanup Wire */

    if (y > 0) {
      Tile = TileAdrPtr[-1];
      if (Tile & CONDBIT) {
	NeutralizeRoad(Tile);
	if ((Tile != 209) && (Tile != 78) && (Tile != 222))
	  adjTile |= 0x0001;
      }
    }

    if (x < (WORLD_X - 1)) {
      Tile = TileAdrPtr[WORLD_Y];
      if (Tile & CONDBIT) {
	NeutralizeRoad(Tile);
	if ((Tile != 208) && (Tile != 77) && (Tile != 221))
	  adjTile |= 0x0002;
      }
    }

    if (y < (WORLD_Y - 1)) {
      Tile = TileAdrPtr[1];
      if (Tile & CONDBIT) {
	NeutralizeRoad(Tile);
	if ((Tile != 209) && (Tile != 78) && (Tile != 222))
	  adjTile |= 0x0004;
      }
    }

    if (x > 0) {
      Tile = TileAdrPtr[-WORLD_Y];
      if (Tile & CONDBIT) {
	NeutralizeRoad(Tile);
	if ((Tile != 208) && (Tile != 77) && (Tile != 221))
	  adjTile |= 0x0008;
      }
    }

    (*TileAdrPtr) = _WireTable[adjTile] | BULLBIT | BURNBIT | CONDBIT;
    return;
  }
}

