/* tool.cpp
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


int OverRide = 0;
int Expensive = 1000;
int Players = 1;
int Votes = 0;
int PendingTool = -1;
int PendingX;
int PendingY;


Quad Micropolis::CostOf[] = {
    100,    100,    100,    500,
      0,    500,      5,      1,
     20,     10,      0,      0,
   5000,     10,   3000,   3000,
   5000,  10000,    100,      0,
};


short Micropolis::toolSize[] = {
  3, 3, 3, 3,
  1, 3, 1, 1,
  1, 1, 0, 0,
  4, 1, 4, 4, 
  4, 6, 1, 0,
};


short Micropolis::toolOffset[] = {
  1, 1, 1, 1,
  0, 1, 0, 0,
  0, 0, 0, 0,
  1, 0, 1, 1,
  1, 1, 0, 0,
};


Quad Micropolis::toolColors[] = {
 COLOR_LIGHTGREEN | (COLOR_LIGHTGREEN << 8),    /* residentialState */
 COLOR_LIGHTBLUE | (COLOR_LIGHTBLUE << 8),      /* commercialState */
 COLOR_YELLOW | (COLOR_YELLOW << 8),            /* industrialState */
 COLOR_LIGHTGREEN | (COLOR_RED << 8),           /* fireState */
 COLOR_ORANGE | (COLOR_ORANGE << 8),            /* queryState */
 COLOR_LIGHTGREEN | (COLOR_LIGHTBLUE << 8),     /* policeState */
 COLOR_DARKGRAY | (COLOR_YELLOW << 8),          /* wireState */
 COLOR_LIGHTBROWN | (COLOR_LIGHTBROWN << 8),    /* dozeState */
 COLOR_DARKGRAY | (COLOR_OLIVE << 8),           /* rrState */
 COLOR_DARKGRAY | (COLOR_WHITE << 8),           /* roadState */
 COLOR_LIGHTGRAY | (COLOR_LIGHTGRAY << 8),      /* chalkState */
 COLOR_DARKGRAY | (COLOR_DARKGRAY << 8),        /* eraserState */
 COLOR_LIGHTGRAY | (COLOR_LIGHTGREEN << 8),     /* stadiumState */
 COLOR_LIGHTBROWN | (COLOR_LIGHTGREEN << 8),    /* parkState */
 COLOR_LIGHTGRAY | (COLOR_LIGHTBLUE << 8),      /* seaportState */
 COLOR_LIGHTGRAY | (COLOR_YELLOW << 8),         /* powerState */
 COLOR_LIGHTGRAY | (COLOR_YELLOW << 8),         /* nuclearState */
 COLOR_LIGHTGRAY | (COLOR_LIGHTBROWN << 8),     /* airportState */
 COLOR_LIGHTGRAY | (COLOR_RED << 8),            /* networkState */
};


/*************************************************************************/
/* UTILITIES */


int Micropolis::putDownPark(
  short mapH, 
  short mapV)
{
  short value, tile;

  if (TotalFunds - CostOf[parkState] >= 0) {

    value = Rand(4);

    if (value == 4) {
      tile = FOUNTAIN | BURNBIT | BULLBIT | ANIMBIT;
    } else {
      tile = (value + WOODS2) | BURNBIT | BULLBIT;
    }

    if (Map[mapH][mapV] == 0) {
      Spend(CostOf[parkState]);
      UpdateFunds();
      Map[mapH][mapV] = tile;
      return 1;
    }

    return -1;
  }

  return -2;
}


int Micropolis::putDownNetwork(
  short state,
  short mapH, 
  short mapV)
{
  int tile = Map[mapH][mapV] & LOMASK;

  if ((TotalFunds > 0) && tally(tile)) {
    Map[mapH][mapV] = tile = 0;
    Spend(1);
  }

  if (tile == 0) {
    if ((TotalFunds - CostOf[state]) >= 0) {

      Map[mapH][mapV] = TELEBASE | CONDBIT | BURNBIT | BULLBIT | ANIMBIT;
      Spend(CostOf[state]);

      return 1;
    } else {
      return -2;
    }
  } else {
    return -1;
  }
}


short Micropolis::checkBigZone(
  short id, 
  short *deltaHPtr, 
  short *deltaVPtr)
{
  switch (id) {

  case POWERPLANT:      /* check coal plant */
  case PORT:            /* check sea port */
  case NUCLEAR:         /* check nuc plant */
  case STADIUM:         /* check stadium */
    *deltaHPtr = 0;
    *deltaVPtr = 0;
    return (4);

  case POWERPLANT + 1:  /* check coal plant */
  case COALSMOKE3:      /* check coal plant, smoke */
  case COALSMOKE3 + 1:  /* check coal plant, smoke */
  case COALSMOKE3 + 2:  /* check coal plant, smoke */
  case PORT + 1:        /* check sea port */
  case NUCLEAR + 1:     /* check nuc plant */
  case STADIUM + 1:     /* check stadium */
    *deltaHPtr = -1;
    *deltaVPtr = 0;
    return (4);

  case POWERPLANT + 4:  /* check coal plant */
  case PORT + 4:        /* check sea port */
  case NUCLEAR + 4:     /* check nuc plant */
  case STADIUM + 4:     /* check stadium */
    *deltaHPtr = 0;
    *deltaVPtr = -1;
    return (4);

  case POWERPLANT + 5:  /* check coal plant */
  case PORT + 5:        /* check sea port */
  case NUCLEAR + 5:     /* check nuc plant */
  case STADIUM + 5:     /* check stadium */
    *deltaHPtr = -1;
    *deltaVPtr = -1;
    return (4);

    /* check airport */
    /*** first row ***/
  case AIRPORT:
    *deltaHPtr = 0;
    *deltaVPtr = 0;
    return (6);

  case AIRPORT + 1:
    *deltaHPtr = -1;
    *deltaVPtr = 0;
    return (6);

  case AIRPORT + 2:
    *deltaHPtr = -2;
    *deltaVPtr = 0;
    return (6);

  case AIRPORT + 3:
    *deltaHPtr = -3;
    *deltaVPtr = 0;
    return (6);

    /*** second row ***/
  case AIRPORT + 6:
    *deltaHPtr = 0;
    *deltaVPtr = -1;
    return (6);

  case AIRPORT + 7:
    *deltaHPtr = -1;
    *deltaVPtr = -1;
    return (6);

  case AIRPORT + 8:
    *deltaHPtr = -2;
    *deltaVPtr = -1;
    return (6);

  case AIRPORT + 9:
    *deltaHPtr = -3;
    *deltaVPtr = -1;
    return (6);

    /*** third row ***/
  case AIRPORT + 12:
    *deltaHPtr = 0;
    *deltaVPtr = -2;
    return (6);

  case AIRPORT + 13:
    *deltaHPtr = -1;
    *deltaVPtr = -2;
    return (6);

  case AIRPORT + 14:
    *deltaHPtr = -2;
    *deltaVPtr = -2;
    return (6);

  case AIRPORT + 15:
    *deltaHPtr = -3;
    *deltaVPtr = -2;
    return (6);

    /*** fourth row ***/
  case AIRPORT + 18:
    *deltaHPtr = 0;
    *deltaVPtr = -3;
    return (6);

  case AIRPORT + 19:
    *deltaHPtr = -1;
    *deltaVPtr = -3;
    return (6);

  case AIRPORT + 20:
    *deltaHPtr = -2;
    *deltaVPtr = -3;
    return (6);

  case AIRPORT + 21:
    *deltaHPtr = -3;
    *deltaVPtr = -3;
    return (6);

  default:
    *deltaHPtr = 0;
    *deltaVPtr = 0;
    return (0);

  }

}


short Micropolis::tally(
  short tileValue)
{
  /* can we autobulldoze this tile? */
  if (((tileValue >= FIRSTRIVEDGE) &&
       (tileValue <= LASTRUBBLE)) || 
      ((tileValue >= (POWERBASE + 2)) &&
       (tileValue <= (POWERBASE + 12))) ||
      ((tileValue >= TINYEXP) &&
       (tileValue <= (LASTTINYEXP + 2)))) { /* ??? */
    return (1);
  } else {
    return (0);
  }
}


short Micropolis::checkSize(
  short temp)
{
  /* check for the normal com, resl, ind 3x3 zones & the fireDept & PoliceDept */
  if (((temp >= (RESBASE - 1)) && 
      (temp  <= (PORTBASE - 1))) ||
      ((temp >= (LASTPOWERPLANT + 1)) && 
      (temp <= (POLICESTATION + 4)))) {

    return (3);

  } else if (((temp >= PORTBASE) && 
              (temp <= LASTPORT)) ||
             ((temp >= COALBASE) && 
              (temp <= LASTPOWERPLANT)) ||
             ((temp >= STADIUMBASE) && 
              (temp <= LASTZONE))) {

    return (4);

  }

  return (0);
}


/* 3x3 */


void Micropolis::check3x3border(
  short xMap, 
  short yMap)
{
  short xPos, yPos;
  short cnt;

  xPos = xMap; yPos = yMap - 1;

  for (cnt = 0; cnt < 3; cnt++) {
    /*** this will do the upper bordering row ***/
    ConnecTile(xPos, yPos, &Map[xPos][yPos], 0);
    xPos++;
  }

  xPos = xMap - 1; yPos = yMap;

  for (cnt = 0; cnt < 3; cnt++) {
    /*** this will do the left bordering row ***/
    ConnecTile(xPos, yPos, &Map[xPos][yPos], 0);
    yPos++;
  }

  xPos = xMap; yPos = yMap + 3;

  for (cnt = 0; cnt < 3; cnt++) {
    /*** this will do the bottom bordering row ***/
    ConnecTile(xPos, yPos, &Map[xPos][yPos], 0);
    xPos++;
  }
        
  xPos = xMap + 3; yPos = yMap;

  for (cnt = 0; cnt < 3; cnt++) {
    /*** this will do the right bordering row ***/
    ConnecTile(xPos, yPos, &Map[xPos][yPos], 0);
    yPos++;
  }

}


int Micropolis::check3x3(
  short mapH, 
  short mapV, 
  short base, 
  short tool)
{
  register short rowNum, columnNum;
  register short holdMapH, holdMapV;
  short xPos, yPos;
  short cost = 0;
  short tileValue;
  short flag;

  mapH--; mapV--;

  if ((mapH < 0) || 
      (mapH > (WORLD_X - 3)) ||
      (mapV < 0) || 
      (mapV > (WORLD_Y - 3))) {
    return -1;
  }

  xPos = holdMapH = mapH;
  yPos = holdMapV = mapV;

  flag = 1;

  for (rowNum = 0; rowNum <= 2; rowNum++) {

    mapH = holdMapH;

    for (columnNum = 0; columnNum <= 2; columnNum++) {

      tileValue = Map[mapH++][mapV] & LOMASK;

      if (autoBulldoze) {

        /* if autoDoze is enabled, add up the cost of bulldozed tiles */
        if (tileValue != DIRT) {

          if (tally(tileValue)) {
            cost++;
          } else {
            flag = 0;
          }

        }

      } else {

        /* check and see if the tile is clear or not  */
        if (tileValue != 0) {
          flag = 0;
        }

      }
    }

    mapV++;
  }

  if (flag == 0) {
    return -1;
  }

  cost += (short)CostOf[tool];

  if ((TotalFunds - cost) < 0) {
    return -2;
  }

  if ((Players > 1) &&
      (OverRide == 0) &&
      (cost >= Expensive)) {
    return -3;
  }

  /* take care of the money situtation here */
  Spend(cost);
  UpdateFunds();

  mapV = holdMapV;

  for (rowNum = 0; rowNum <= 2; rowNum++) {

    mapH = holdMapH;

    for (columnNum = 0; columnNum <= 2; columnNum++) {

      if (columnNum == 1 && rowNum == 1) {
        Map[mapH++][mapV] = base + BNCNBIT + ZONEBIT;
      } else {
        Map[mapH++][mapV] = base + BNCNBIT;
      }

      base++;

    }

    mapV++;

  }

  check3x3border(xPos, yPos);

  return 1;
}


/* 4x4 */


void Micropolis::check4x4border(
  short xMap, 
  short yMap)
{
  short *tilePtr;
  short xPos, yPos;
  short cnt;

  xPos = xMap; yPos = yMap - 1;

  for (cnt = 0; cnt < 4; cnt++) {
    /* this will do the upper bordering row */
    tilePtr = &Map[xPos][yPos];
    ConnecTile(xPos, yPos, tilePtr, 0);
    xPos++;
  }

  xPos = xMap - 1; yPos = yMap;

  for (cnt = 0; cnt < 4; cnt++) {
    /* this will do the left bordering row */
    tilePtr = &Map[xPos][yPos];
    ConnecTile(xPos, yPos, tilePtr, 0);
    yPos++;
  }

  xPos = xMap; yPos = yMap + 4;

  for (cnt = 0; cnt < 4;cnt++) {
    /* this will do the bottom bordering row */
    tilePtr = &Map[xPos][yPos];
    ConnecTile(xPos, yPos, tilePtr, 0);
    xPos++;
  }
        
  xPos = xMap + 4; yPos = yMap;

  for (cnt = 0; cnt < 4; cnt++) {
    /* this will do the right bordering row */
    tilePtr = &Map[xPos][yPos];
    ConnecTile(xPos, yPos, tilePtr, 0);
    yPos++;
  }
}


short Micropolis::check4x4(
  short mapH, 
  short mapV, 
  short base, 
  short aniFlag, 
  short tool)
{
  register short rowNum, columnNum;
  short h, v;
  short holdMapH;
  short xMap, yMap;
  short tileValue;
  short flag;
  short cost = 0;

  mapH--; mapV--;

  if ((mapH < 0) || 
      (mapH > (WORLD_X - 4)) ||
      (mapV < 0) || 
      (mapV > (WORLD_Y - 4))) {
    return -1;
  }

  h = xMap = holdMapH = mapH;
  v = yMap = mapV;

  flag = 1;

  for (rowNum = 0; rowNum <= 3; rowNum++) {

    mapH = holdMapH;

    for (columnNum = 0; columnNum <= 3; columnNum++) {

      tileValue = Map[mapH++][mapV] & LOMASK;

      if (autoBulldoze) {

        /* if autoDoze is enabled, add up the cost of bulldozed tiles */
        if (tileValue != DIRT) {

          if (tally(tileValue)) {
            cost++;
          } else {
            flag = 0;
          }

        }

      } else {

        /* check and see if the tile is clear or not  */
        if (tileValue != 0) {
          flag = 0;
        }

      }

    }

    mapV++;

  }

  if (flag == 0) {
    return -1;
  }

  cost += (short)CostOf[tool];

  if ((TotalFunds - cost) < 0) {
    return -2;
  }

  if ((Players > 1) &&
      (OverRide == 0) &&
      (cost >= Expensive)) {
    return -3;
  }

  /* take care of the money situtation here */
  Spend(cost);
  UpdateFunds();

  mapV = v; holdMapH = h;

  for (rowNum = 0; rowNum <= 3; rowNum++) {

    mapH = holdMapH;

    for (columnNum = 0; columnNum <= 3; columnNum++) {

      if (columnNum == 1 && 
          rowNum == 1) {
        Map[mapH++][mapV] = base + BNCNBIT + ZONEBIT;
      } else if (columnNum == 1 && 
                 rowNum == 2 && 
                 aniFlag) {
        Map[mapH++][mapV] = base + BNCNBIT + ANIMBIT;
      } else {
        Map[mapH++][mapV] = base + BNCNBIT;
      }

      base++;

    }

    mapV++;

  }

  check4x4border(xMap, yMap);

  return 1;
}


/* 6x6 */


void Micropolis::check6x6border(
  short xMap, 
  short yMap)
{
  short xPos, yPos;
  short cnt;

  xPos = xMap; yPos = yMap - 1;

  for (cnt = 0; cnt < 6; cnt++) {
    /* this will do the upper bordering row */
    ConnecTile(xPos, yPos, &Map[xPos][yPos], 0);
    xPos++;
  }

  xPos = xMap - 1; yPos = yMap;

  for (cnt=0; cnt < 6; cnt++) {
    /* this will do the left bordering row */
    ConnecTile(xPos, yPos, &Map[xPos][yPos], 0);
    yPos++;
  }

  xPos = xMap; yPos = yMap + 6;

  for (cnt = 0; cnt < 6; cnt++) {
    /* this will do the bottom bordering row */
    ConnecTile(xPos, yPos, &Map[xPos][yPos], 0);
    xPos++;
  }
        
  xPos = xMap + 6; yPos = yMap;

  for (cnt = 0; cnt < 6; cnt++) {
    /* this will do the right bordering row */
    ConnecTile(xPos, yPos, &Map[xPos][yPos], 0);
    yPos++;
  }

}


short Micropolis::check6x6(
  short mapH, 
  short mapV, 
  short base, 
  short tool)
{
  register short rowNum, columnNum;
  short h, v;
  short holdMapH;
  short xMap, yMap;
  short flag;
  short tileValue;
  short cost = 0;

  mapH--; mapV--;
  if ((mapH < 0) || 
      (mapH > (WORLD_X - 6)) ||
      (mapV < 0) || 
      (mapV > (WORLD_Y - 6))) {
    return -1;
  }

  h = xMap = holdMapH = mapH;
  v = yMap = mapV;

  flag = 1;

  for (rowNum = 0; rowNum <= 5; rowNum++) {

    mapH = holdMapH;

    for (columnNum = 0; columnNum <= 5; columnNum++) {

      tileValue = Map[mapH++][mapV] & LOMASK;

      if (autoBulldoze) {

        /* if autoDoze is enabled, add up the cost of bulldozed tiles */
        if (tileValue != DIRT) {

          if (tally(tileValue)) {
            cost++;
          } else {
            flag = 0;
          }

        }

      } else {

        /* check and see if the tile is clear or not  */
        if (tileValue != DIRT) {
          flag = 0;
        }

      }

    }

    mapV++;

  }

  if (flag == 0) {
    return -1;
  }

  cost += (short)CostOf[tool];

  if ((TotalFunds - cost) < 0) {
    return -2;
  }

  if ((Players > 1) &&
      (OverRide == 0) &&
      (cost >= Expensive)) {
    return -3;
  }

  /* take care of the money situtation here */
  Spend(cost);
  UpdateFunds();

  mapV = v; 
  holdMapH = h;

  for (rowNum = 0; rowNum <= 5; rowNum++) {

    mapH = holdMapH;

    for (columnNum = 0; columnNum <= 5; columnNum++) {

      if (columnNum == 1 && rowNum == 1) {
        Map[mapH++][mapV] = base + BNCNBIT + ZONEBIT;
      } else {
        Map[mapH++][mapV] = base + BNCNBIT;
      }

      base++;

    }

    mapV++;

  }

  check6x6border(xMap, yMap);

  return 1;
}


/* QUERY */


/* search table for zone status string match */
static short idArray[28] = {
  DIRT, RIVER, TREEBASE, RUBBLE,
  FLOOD, RADTILE, FIRE, ROADBASE,
  POWERBASE, RAILBASE, RESBASE, COMBASE,
  INDBASE, PORTBASE, AIRPORTBASE, COALBASE,
  FIRESTBASE, POLICESTBASE, STADIUMBASE, NUCLEARBASE,
  827, 832, FOUNTAIN, INDBASE2,
  FOOTBALLGAME1, VBRDG0, 952, 956
};

/*
  0, 2, 21, 44, 
  48, 52, 53, 64,
  208, 224, 240, 423, 
  612, 693, 709, 745,
  761, 770, 779, 811, 
  827, 832, 840, 844,
  932, 948, 952, 956

  Clear, Water, Trees, Rubble, 
  Flood, Radioactive Waste, Fire, Road,
  Power, Rail, Residential, Commercial,
  Industrial, Port, AirPort, Coal Power,
  Fire Department, Police Department, Stadium, Nuclear Power, 
  Draw Bridge, Radar Dish, Fountain, Industrial,
  49er's 38  Bears 3, Draw Bridge, Ur 238
*/


int Micropolis::getDensityStr(
  short catNo, 
  short mapH, 
  short mapV)
{
  int z;

  switch(catNo) {

  case 0:
  default:
    z = PopDensity[mapH >>1][mapV >>1];
    z = z >> 6;
    z = z & 3;

    return (z);

  case 1:
    z = LandValueMem[mapH >>1][mapV >>1];

    if (z < 30) {
      return (4);
    }

    if (z < 80) {
      return (5);
    }

    if (z < 150) {
      return (6);
    }

    return (7);

  case 2:
    z = CrimeMem[mapH >>1][mapV >>1];
    z = z >> 6;
    z = z & 3;

    return (z + 8);

  case 3:
    z = PollutionMem[mapH >>1][mapV >>1];

    if ((z < 64) && (z > 0)) {
      return (13);
    }

    z = z >> 6;
    z = z & 3;

    return (z + 12);

  case 4:
    z = RateOGMem[mapH >>3][mapV >>3];

    if (z < 0) {
      return (16);
    }

    if (z == 0) {
      return (17);
    }

    if (z > 100) {
      return (19);
    }

    return (18);

  }
}


void Micropolis::doZoneStatus(
  short mapH, 
  short mapV)
{
  char localStr[256];
  char statusStr[5][256];
  short id;
  short x;
  short tileNum;
  short found;

  tileNum = Map[mapH][mapV] & LOMASK;

  if (tileNum >= COALSMOKE1 && 
      tileNum < FOOTBALLGAME1) {
    tileNum = COALBASE;
  }

  found = 1;

  for (x = 1; x < 29; x++) {
    if (tileNum < idArray[x]) {
      found = 0;
      break;
    }
  }

  x--;

  if ((x < 1) || 
      (x > 28)) {
    x = 28;
  }

  GetIndString(localStr, 219, x);

  for (x = 0; x < 5; x++) {

    id = getDensityStr(x, mapH, mapV);
    id++;

    if (id <= 0) {
      id = 1;
    }

    if (id > 20) {
      id = 20;
    }

    GetIndString(statusStr[x], 202, id);

  }

  DoShowZoneStatus(
    localStr,
    statusStr[0],
    statusStr[1],
    statusStr[2],
    statusStr[3],
    statusStr[4],
    mapH,
    mapV);
}


void Micropolis::DoShowZoneStatus(
  char *str, 
  char *s0, 
  char *s1, 
  char *s2, 
  char *s3, 
  char *s4,
  int x, 
  int y)
{
  Callback(
    "UIShowZoneStatus",
    "ssssssdd",
    str, 
    s0, 
    s1, 
    s2, 
    s3, 
    s4, 
    (int)x, 
    (int)y);
}


/* comefrom: processWand */
void Micropolis::put3x3Rubble(
  short x, 
  short y)
{
  register int xx, yy, zz;
        
  for (xx = x - 1; xx < x + 2; xx++) {
    for (yy = y - 1; yy < y + 2; yy++)  {

      if (TestBounds(xx, yy)) {

        zz = Map[xx][yy] & LOMASK;

        if ((zz != RADTILE) && 
            (zz != DIRT)) {

          Map[xx][yy] =
            (DoAnimation
             ? (TINYEXP + Rand(2))
             : SOMETINYEXP) |
            ANIMBIT | BULLBIT;

        }
      }
    }
  }
}


/* comefrom: processWand */
void Micropolis::put4x4Rubble(
  short x, 
  short y)
{
  register int xx, yy, zz;
        
  for (xx = x - 1; xx < x + 3; xx++) {
    for (yy = y - 1; yy < y + 3; yy++) {

      if (TestBounds(xx, yy)) {

        zz = Map[xx][yy] & LOMASK;

        if ((zz != RADTILE) && 
            (zz != DIRT)) {

          Map[xx][yy] =
            (DoAnimation
             ? (TINYEXP + Rand(2))
             : SOMETINYEXP) |
            ANIMBIT | BULLBIT;

        }
      }
    }
  }
}


/* comefrom: processWand */
void Micropolis::put6x6Rubble(
  short x, 
  short y)
{
  register int xx, yy, zz;

  for (xx = x - 1; xx < x + 5; xx++) {
    for (yy = y - 1; yy < y + 5; yy++)  {

      if (TestBounds(xx, yy)) {

        zz = Map[xx][yy] & LOMASK;

        if ((zz != RADTILE) && 
            (zz != DIRT)) {

          Map[xx][yy] =
            (DoAnimation
             ? (TINYEXP + Rand(2))
             : SOMETINYEXP) |
            ANIMBIT | BULLBIT;

        }
      }
    }
  }
}       


void Micropolis::DidTool(
  char *name, 
  short x, 
  short y)
{
  Callback(
    "UIDidTool",
    "sdd",
    name, 
    (int)x, 
    (int)y);
}


/************************************************************************/
/* TOOLS */


int Micropolis::query_tool(
  short x, 
  short y)
{
  if ((x < 0) || 
      (x > (WORLD_X - 1)) ||
      (y < 0) || 
      (y > (WORLD_Y - 1))) {
    return -1;
  }

  doZoneStatus(x, y);
  DidTool("Qry", x, y);

  return 1;
}


int Micropolis::bulldozer_tool(
  short x, 
  short y)
{
  unsigned short currTile, temp;
  short zoneSize, deltaH, deltaV;
  int result = 1;

  if ((x < 0) || 
      (x > (WORLD_X - 1)) ||
      (y < 0) || 
      (y > (WORLD_Y - 1))) {
    return -1;
  }

  currTile = Map[x][y];
  temp = currTile & LOMASK;

  if (currTile & ZONEBIT) { /* zone center bit is set */

    if (TotalFunds > 0) {

      Spend(1);

      switch (checkSize(temp)) {

      case 3:
        MakeSound("city", "Explosion-High");
        put3x3Rubble(x, y);
        break;

      case 4:
        put4x4Rubble(x, y);
        MakeSound("city", "Explosion-Low");
        break;

      case 6: 
        MakeSound("city", "Explosion-High");
        MakeSound("city", "Explosion-Low");
        put6x6Rubble(x, y);
        break;

      default:
        break;

      }

    }

  } else if ((zoneSize = checkBigZone(temp, &deltaH, &deltaV))) {

    if (TotalFunds > 0) {

      Spend(1);

      switch (zoneSize) {

      case 3:
        MakeSound("city", "Explosion-High");
        break;

      case 4:
        MakeSound("city", "Explosion-Low");
        put4x4Rubble(x + deltaH, y + deltaV);
        break;

      case 6: 
        MakeSound("city", "Explosion-High");
        MakeSound("city", "Explosion-Low");
        put6x6Rubble(x + deltaH, y + deltaV);
        break;

      }

    }

  } else {

    if (temp == RIVER || 
        temp == REDGE || 
        temp == CHANNEL) {

      if (TotalFunds >= 6) {

        result = ConnecTile(x, y, &Map[x][y], 1);

        if (temp != (Map[x][y] & LOMASK)) {
          Spend(5);
        }

      } else {
        result = 0;
      }
    } else {
      result = ConnecTile(x, y, &Map[x][y], 1);
    }

  }

  UpdateFunds();

  if (result == 1) {
    DidTool("Dozr", x, y);
  }

  return result;
}


int Micropolis::road_tool(
  short x, 
  short y)
{
  int result;

  if ((x < 0) || 
      (x > (WORLD_X - 1)) ||
      (y < 0) || 
      (y > (WORLD_Y - 1))) {
    return -1;
  }

  result = ConnecTile(x, y, &Map[x][y], 2);
  UpdateFunds();

  if (result == 1) {
    DidTool("Road", x, y);
  }

  return result;
}


int Micropolis::rail_tool(
  short x, 
  short y)
{
  int result;

  if ((x < 0) || 
      (x > (WORLD_X - 1)) ||
      (y < 0) || 
      (y > (WORLD_Y - 1))) {
    return -1;
  }

  result = ConnecTile(x, y, &Map[x][y], 3);
  UpdateFunds();

  if (result == 1) {
    DidTool("Rail", x, y);
  }

  return result;
}


int Micropolis::wire_tool(
  short x, 
  short y)
{
  int result;

  if ((x < 0) || 
      (x > (WORLD_X - 1)) ||
      (y < 0) || 
      (y > (WORLD_Y - 1))) {
    return -1;
  }

  result = ConnecTile(x, y, &Map[x][y], 4);
  UpdateFunds();

  if (result == 1) {
    DidTool("Wire", x, y);
  }

  return result;
}


int Micropolis::park_tool(
  short x, 
  short y)
{
  int result;

  if ((x < 0) || 
      (x > (WORLD_X - 1)) ||
      (y < 0) || 
      (y > (WORLD_Y - 1))) {
    return -1;
  }

  result = putDownPark(x, y);

  if (result == 1) {
    DidTool("Park", x, y);
  }

  return result;
}


int Micropolis::residential_tool(
  short x, 
  short y)
{
  int result;

  if ((x < 0) || 
      (x > (WORLD_X - 1)) ||
      (y < 0) || 
      (y > (WORLD_Y - 1))) {
    return -1;
  }

  result = check3x3(x, y, RESBASE, residentialState);

  if (result == 1) {
    DidTool("Res", x, y);
  }

  return result;
}


int Micropolis::commercial_tool(
  short x, 
  short y)
{
  int result;

  if ((x < 0) || 
      (x > (WORLD_X - 1)) ||
      (y < 0) || 
      (y > (WORLD_Y - 1))) {
    return -1;
  }

  result = check3x3(x, y, COMBASE, commercialState);

  if (result == 1) {
    DidTool("Com", x, y);
  }

  return result;
}


int Micropolis::industrial_tool(
  short x, 
  short y)
{
  int result;

  if ((x < 0) || 
      (x > (WORLD_X - 1)) ||
      (y < 0) || 
      (y > (WORLD_Y - 1))) {
    return -1;
  }

  result = check3x3(x, y, INDBASE, industrialState);

  if (result == 1) {
    DidTool("Ind", x, y);
  }

  return result;
}


int Micropolis::police_dept_tool(
  short x, 
  short y)
{
  int result;

  if ((x < 0) || 
      (x > (WORLD_X - 1)) ||
      (y < 0) || 
      (y > (WORLD_Y - 1))) {
    return -1;
  }

  result = check3x3(x, y, POLICESTBASE, policeState);

  if (result == 1) {
    DidTool("Pol", x, y);
  }

  return result;
}


int Micropolis::fire_dept_tool(
  short x, 
  short y)
{
  int result;

  if ((x < 0) || 
      (x > (WORLD_X - 1)) ||
      (y < 0) || 
      (y > (WORLD_Y - 1))) {
    return -1;
  }

  result = check3x3(x, y, FIRESTBASE, fireState);

  if (result == 1) {
    DidTool("Fire", x, y);
  }

  return result;
}


int Micropolis::stadium_tool(
  short x, 
  short y)
{
  int result;

  if ((x < 0) || 
      (x > (WORLD_X - 1)) ||
      (y < 0) || 
      (y > (WORLD_Y - 1))) {
    return -1;
  }

  result = check4x4(x, y, STADIUMBASE, 0, stadiumState);

  if (result == 1) {
    DidTool("Stad", x, y);
  }

  return result;
}


int Micropolis::coal_power_plant_tool(
  short x, 
  short y)
{
  int result;

  if ((x < 0) || 
      (x > (WORLD_X - 1)) ||
      (y < 0) || 
      (y > (WORLD_Y - 1))) {
    return -1;
  }

  result = check4x4(x, y, COALBASE, 1, powerState);

  if (result == 1) {
    DidTool("Coal", x, y);
  }

  return result;
}


int Micropolis::nuclear_power_plant_tool(
  short x, 
  short y)
{
  int result;

  if ((x < 0) || 
      (x > (WORLD_X - 1)) ||
      (y < 0) || 
      (y > (WORLD_Y - 1))) {
    return -1;
  }

  result = check4x4(x, y, NUCLEARBASE, 1, nuclearState);

  if (result == 1) {
    DidTool("Nuc", x, y);
  }

  return result;
}


int Micropolis::seaport_tool(
  short x, 
  short y)
{
  int result;

  if ((x < 0) || 
      (x > (WORLD_X - 1)) ||
      (y < 0) || 
      (y > (WORLD_Y - 1))) {
    return -1;
  }

  result = check4x4(x, y, PORTBASE, 0, seaportState);

  if (result == 1) {
    DidTool("Seap", x, y);
  }

  return result;
}


int Micropolis::airport_tool(
  short x, 
  short y)
{
  int result;

  if ((x < 0) || 
      (x > (WORLD_X - 1)) ||
      (y < 0) || 
      (y > (WORLD_Y - 1))) {
    return -1;
  }

  result = check6x6(x, y, AIRPORTBASE, airportState);

  if (result == 1) {
    DidTool("Airp", x, y);
  }

  return result;
}


int Micropolis::network_tool(
  short x, 
  short y)
{
  int result;

  if ((x < 0) || 
      (x > (WORLD_X - 1)) ||
      (y < 0) || 
      (y > (WORLD_Y - 1))) {
    return -1;
  }

  result = 
	putDownNetwork(networkState, x, y);

  if (result == 1) {
    DidTool("Net", x, y);
  }

  return result;
}


int Micropolis::ChalkTool(
  short x, 
  short y, 
  short color, 
  short first)
{
  if (first) {
    ChalkStart(x, y, color);
  } else {
    ChalkTo(x, y);
  }

  DidTool("Chlk", x, y);

  return 1;
}


void Micropolis::ChalkStart(
  int x, 
  int y, 
  int color)
{
  Ink *ink;
  Ink **ip;

  for (ip = &overlay; *ip != NULL; ip = &((*ip)->next)) ;

  *ip = ink = NewInk();
  ink->x = x; ink->y = y;
  ink->color = color;
  StartInk(ink, x, y);
  track_ink = ink;
  last_x = x;
  last_y = y;
}


void Micropolis::ChalkTo(
  int x, 
  int y)
{
  AddInk(track_ink, x, y);
  last_x = x;
  last_y = y;
}


int Micropolis::EraserTool(
  short x, 
  short y, 
  short first)
{
  if (first) {
    EraserStart(x, y);
  } else {
    EraserTo(x, y);
  }

  DidTool("Eraser", x, y);

  return 1;
}


int Micropolis::InkInBox(
  Ink *ink, 
  int left, 
  int top, 
  int right, 
  int bottom)
{
  if ((left <= ink->right) &&
      (right >= ink->left) &&
      (top <= ink->bottom) &&
      (bottom >= ink->top)) {
    int x, y, lx, ly, i;

    if (ink->length == 1) {
      return 1;
    }

    x = ink->x;  y = ink->y;
    for (i = 1; i < ink->length; i++) {
      int ileft, iright, itop, ibottom;

      lx = x; ly = y;
      x += ink->points[i].x;  y += ink->points[i].y;
      if (x < lx) { 
        ileft = x; iright = lx;
      } else { 
        ileft = lx; 
        iright = x;
      }
      if (y < ly) { 
        itop = y; 
        ibottom = ly; 
      } else { 
        itop = ly; 
        ibottom = y; 
      }
      if ((left <= iright) &&
          (right >= ileft) &&
          (top <= ibottom) &&
          (bottom >= itop)) {
        return 1;
      }
    }
  }

  return 0;
}


void Micropolis::EraserStart(
  int x, 
  int y)
{
  EraserTo(x, y);
}


void Micropolis::EraserTo(
  int x, 
  int y)
{
  Ink **ip, *ink;

  for (ip = &overlay; *ip != NULL;) {
    ink = *ip;
    if (InkInBox(ink, x - 8, y - 8, x + 8, y + 8)) {

	  // TODO: Redraw views that contain this rectangle. 

      *ip = ink->next;

      FreeInk(ink);
    } else {
      ip = &((*ip)->next);
    }
  }
}


int Micropolis::do_tool(
  short state, 
  short x, 
  short y, 
  short first)
{
  int result = 0;

  switch (state) {

  case residentialState:
    result = residential_tool(x >>4, y >>4);
    break;

  case commercialState:
    result = commercial_tool(x >>4, y >>4);
    break;

  case industrialState:
    result = industrial_tool(x >>4, y >>4);
    break;

  case fireState:
    result = fire_dept_tool(x >>4, y >>4);
    break;

  case queryState:
    result = query_tool(x >>4, y >>4);
    break;

  case policeState:
    result = police_dept_tool(x >>4, y >>4);
    break;

  case wireState:
    result = wire_tool(x >>4, y >>4);
    break;

  case dozeState:
    result = bulldozer_tool(x >>4, y >>4);
    break;
    
  case rrState:
    result = rail_tool(x >>4, y >>4);
    break;

  case roadState:
    result = road_tool(x >>4, y >>4);
    break;

  case chalkState:
    result = ChalkTool(x - 5, y + 11, COLOR_WHITE, first);
    break;

  case eraserState:
    result = EraserTool(x, y, first);
    break;

  case stadiumState:
    result = stadium_tool(x >>4, y >>4);
    break;

  case parkState:
    result = park_tool(x >>4, y >>4);
    break;

  case seaportState:
    result = seaport_tool(x >>4, y >>4);
    break;

  case powerState:
    result = coal_power_plant_tool(x >>4, y >>4);
    break;

  case nuclearState:
    result = nuclear_power_plant_tool(x >>4, y >>4);
    break;

  case airportState:
    result = airport_tool(x >>4, y >>4);
    break;

  case networkState:
    result = network_tool(x >>4, y >>4);
    break;

  default:
    result = 0;
    break;

  }

  return result;
}


void Micropolis::DoTool(
  short tool, 
  short x, 
  short y)
{
  int result;

  result = do_tool(tool, x <<4, y <<4, 1);

  if (result == -1) {
    ClearMes();
    SendMes(34);
    MakeSound("edit", "UhUh");
  } else if (result == -2) {
    ClearMes();
    SendMes(33);
    MakeSound("edit", "Sorry");
  }

  sim_skip = 0;
  InvalidateEditors();
}


void Micropolis::ToolDown(
  short tool,
  int x, 
  int y)
{
  int result;

  // TODO: fix this
  //ViewToPixelCoords(x, y, &x, &y);
  last_x = x;
  last_y = y;

  result = do_tool(tool, x, y, 1);

  if (result == -1) {
    ClearMes();
    SendMes(34);
    MakeSound("edit", "UhUh");
  } else if (result == -2) {
    ClearMes();
    SendMes(33);
    MakeSound("edit", "Sorry");
  } else if (result == -3) {
    DoPendTool(tool, x >>4, y >>4);
  }

  sim_skip = 0;

  // TODO: update views
}


void Micropolis::ToolUp(
  short tool,
  int x, 
  int y)
{
  ToolDrag(tool, x, y);
}


void Micropolis::ToolDrag(
  short tool,
  int px, 
  int py)
{
  int x, y, dx, dy, adx, ady, lx, ly, dist;
  float i, step, tx, ty, dtx, dty, rx, ry;

  // TODO: fix this
  // ViewToPixelCoords(px, py, &x, &y);
  x = px;
  y = py;

  tool_x = x; 
  tool_y = y;

  if ((tool == chalkState) ||
      (tool == eraserState)) {

    do_tool(tool, x, y, 0);
    last_x = x; 
	last_y = y;

  } else {

    dist = toolSize[tool];

    x >>= 4; 
	y >>= 4;
    lx = last_x >> 4;
    ly = last_y >> 4;

    dx = x - lx;
    dy = y - ly;

    if ((dx == 0) && 
		(dy == 0)) {
      return;
    }

    adx = ABS(dx); 
	ady = ABS(dy);

    if (adx > ady) {
      step = (float)0.3 / adx;
    } else {
      step = (float)0.3 / ady;
    }

    rx = (float)(dx < 0 ? 1 : 0);
    ry = (float)(dy < 0 ? 1 : 0);

    if (dist == 1) {
      for (i = 0.0; i <= 1 + step; i += step) {
        tx = (last_x >>4) + i * dx;
        ty = (last_y >>4) + i * dy;
        dtx = ABS(tx - lx);
        dty = ABS(ty - ly);
        if ((dtx >= 1) || 
			(dty >= 1)) {
          // fill in corners
          if ((dtx >= 1) && 
			  (dty >= 1)) {
            if (dtx > dty) {
              do_tool(tool, ((int)(tx + rx)) <<4, ly <<4, 0);
            } else {
              do_tool(tool, lx <<4, ((int)(ty + ry)) <<4, 0);
            }
          }
          lx = (int)(tx + rx);
          ly = (int)(ty + ry);
          do_tool(tool, lx <<4, ly <<4, 0);
        }
      }
    } else {
      for (i = 0.0; i <= 1 + step; i += step) {
        tx = (last_x >>4) + i * dx;
        ty = (last_y >>4) + i * dy;
        dtx = ABS(tx - lx);
        dty = ABS(ty - ly);
        lx = (int)(tx + rx);
        ly = (int)(ty + ry);
        do_tool(tool, lx <<4, ly <<4, 0);
      }
    }

    last_x = (lx <<4) + 8;
    last_y = (ly <<4) + 8;
  }

  sim_skip = 0; // update editors overlapping this one
  
  // TODO: update views
}


void Micropolis::DoPendTool(
  int tool, 
  int x, 
  int y)
{
  Callback(
    "UIDoPendTool",
    "ddd",
    tool,
    x,
    y);
}


////////////////////////////////////////////////////////////////////////
