/* s_gen.c
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


/* Generate Map */


#define WATER_LOW	RIVER /* 2 */
#define WATER_HIGH	LASTRIVEDGE /* 20 */
#define WOODS_LOW	TREEBASE /* 21 */
#define WOODS_HIGH	UNUSED_TRASH2 /* 39 */


short XStart, YStart, MapX, MapY;
short Dir, LastDir;
int TreeLevel = -1;		/* level for tree creation */
int LakeLevel = -1;		/* level for lake creation */
int CurveLevel = -1;		/* level for river curviness */
int CreateIsland = -1;		/* -1 => 10%, 0 => never, 1 => always */


GenerateNewCity(void) 
{
  GenerateSomeCity(Rand16());
}


GenerateSomeCity(int r)
{
  if (CityFileName != NULL) {
    ckfree(CityFileName);
    CityFileName = NULL;
  }

  gettimeofday(&start_time, NULL);

  GenerateMap(r);
  ScenarioID = 0;
  CityTime = 0;
  InitSimLoad = 2;
  DoInitialEval = 0;

  InitWillStuff();
  ResetMapState();
  ResetEditorState();
  InvalidateEditors();
  InvalidateMaps();
  UpdateFunds();
  DoSimInit();
  Eval("UIDidGenerateNewCity");
  Kick();
}


ERand(short limit)
{
  short x, z;

  z = Rand(limit);
  x = Rand(limit);
  if (z < x)
    return (z);
  return (x);
}


GenerateMap(int r)
{
  SeedRand(r);

  if (CreateIsland < 0) {
    if (Rand(100) < 10) { /* chance that island is generated */
      MakeIsland();
      return;
    }
  }
  if (CreateIsland == 1) {
    MakeNakedIsland();
  } else {
    ClearMap();
  }
  GetRandStart();
  if (CurveLevel != 0) {
    DoRivers();
  }
  if (LakeLevel != 0) {
    MakeLakes();
  }
  SmoothRiver();
  if (TreeLevel != 0) {
    DoTrees();
  }
  RandomlySeedRand();
}


ClearMap(void)
{
  register short x, y;

  for (x = 0; x < WORLD_X; x++)
    for (y = 0; y < WORLD_Y; y++)
      Map[x][y] = DIRT;
}


ClearUnnatural(void)
{
  register short x, y;

  for (x = 0; x < WORLD_X; x++) {
    for (y = 0; y < WORLD_Y; y++) {
      if (Map[x][y] > WOODS) {
	Map[x][y] = DIRT;
      }
    }
  }
}


#define RADIUS 18

MakeNakedIsland()
{
  register int x, y;

  for (x = 0; x < WORLD_X; x++)
    for (y = 0; y < WORLD_Y; y++)
      Map[x][y] = RIVER;
  for (x = 5; x < WORLD_X - 5; x++)
    for (y = 5; y < WORLD_Y - 5; y++)
      Map[x][y] = DIRT;
  for (x = 0; x < WORLD_X - 5; x += 2) {
    MapX = x ;
    MapY = ERand(RADIUS);
    BRivPlop();
    MapY = (WORLD_Y - 10) - ERand(RADIUS);
    BRivPlop();
    MapY = 0;
    SRivPlop();
    MapY = (WORLD_Y - 6);
    SRivPlop();
  }
  for (y = 0; y < WORLD_Y - 5; y += 2) {
    MapY = y ;
    MapX = ERand(RADIUS);
    BRivPlop();
    MapX = (WORLD_X - 10) - ERand(RADIUS);
    BRivPlop();
    MapX = 0;
    SRivPlop();
    MapX = (WORLD_X - 6);
    SRivPlop();
  }
}


MakeIsland(void)
{
  MakeNakedIsland();
  SmoothRiver();
  DoTrees();
}


MakeLakes(void)
{
  short Lim1, Lim2, t, z;
  register short x, y;

  if (LakeLevel < 0) {
    Lim1 = Rand(10);
  } else {
    Lim1 = LakeLevel / 2;
  }
  for (t = 0; t < Lim1; t++) {
    x = Rand(WORLD_X - 21) + 10;
    y = Rand(WORLD_Y - 20) + 10;
    Lim2 = Rand(12) + 2;
    for (z = 0; z < Lim2; z++) {
      MapX = x - 6 + Rand(12);
      MapY = y - 6 + Rand(12);
      if (Rand(4))
	SRivPlop();
      else
	BRivPlop();
    }
  }
}


GetRandStart(void)
{
  XStart = 40 + Rand(WORLD_X - 80);
  YStart = 33 + Rand(WORLD_Y - 67);
  MapX = XStart;
  MapY = YStart;
}


MoveMap(short dir)
{
  static short DirTab[2][8] = { { 0, 1, 1, 1, 0, -1, -1, -1},
				{-1,-1, 0, 1, 1,  1,  0, -1} };
  dir = dir & 7;
  MapX += DirTab[0][dir];
  MapY += DirTab[1][dir];
}


TreeSplash(short xloc, short yloc)
{
  short dis, dir;
  register short xoff, yoff, z;

  if (TreeLevel < 0) {
    dis = Rand(150) + 50;
  } else {
    dis = Rand(100 + (TreeLevel * 2)) + 50;
  }
  MapX = xloc;
  MapY = yloc;
  for (z = 0; z < dis; z++) {
    dir = Rand(7);
    MoveMap(dir);
    if (!(TestBounds(MapX, MapY)))
      return;
    if ((Map[MapX][MapY] & LOMASK) == DIRT)
      Map[MapX][MapY] = WOODS + BLBNBIT;
  }
}


DoTrees(void)
{
  short Amount, x, xloc, yloc;

  if (TreeLevel < 0) {
    Amount = Rand(100) + 50;
  } else {
    Amount = TreeLevel + 3;
  }
  for(x = 0; x < Amount; x++) {
    xloc = Rand(WORLD_X - 1);
    yloc = Rand(WORLD_Y - 1);
    TreeSplash(xloc, yloc);
  }
  SmoothTrees();
  SmoothTrees();
}


SmoothRiver(void)
{
  static short DX[4] = {-1, 0, 1, 0};
  static short DY[4] = { 0, 1, 0,-1};
  static short REdTab[16] = {
    13+BULLBIT,	13+BULLBIT,	17+BULLBIT,	15+BULLBIT,
    5+BULLBIT,	2,		19+BULLBIT,	17+BULLBIT,
    9+BULLBIT,	11+BULLBIT,	2,		13+BULLBIT,
    7+BULLBIT,	9+BULLBIT,	5+BULLBIT,	2 };
  short bitindex, z, Xtem, Ytem;
  register short temp, MapX, MapY;

  for (MapX = 0; MapX < WORLD_X; MapX++) {
    for (MapY = 0; MapY < WORLD_Y; MapY++) {
      if (Map[MapX][MapY] == REDGE) {
	bitindex = 0;
	for (z = 0; z < 4; z++) {
	  bitindex = bitindex << 1;
	  Xtem = MapX + DX[z];
	  Ytem = MapY + DY[z];
	  if (TestBounds(Xtem, Ytem) &&
	      ((Map[Xtem][Ytem] & LOMASK) != DIRT) &&
	      (((Map[Xtem][Ytem]&LOMASK) < WOODS_LOW) ||
	       ((Map[Xtem][Ytem]&LOMASK) > WOODS_HIGH)))
	      bitindex++;
	}
	temp = REdTab[bitindex & 15];
	if ((temp != RIVER) && (Rand(1)))
	  temp++;
	Map[MapX][MapY] = temp;
      }
    }
  }
}


IsTree(int cell)
{
  if (((cell & LOMASK) >= WOODS_LOW) &&
      ((cell & LOMASK) <= WOODS_HIGH))
    return TRUE;
  return FALSE;
}
 

SmoothTrees(void)
{
  static short DX[4] = {-1, 0, 1, 0};
  static short DY[4] = { 0, 1, 0,-1};
  static short TEdTab[16] = { 0, 0, 0, 34,
			      0, 0, 36, 35,
			      0, 32, 0, 33,
			      30, 31, 29, 37 };
  short   bitindex, z, Xtem, Ytem;
  register short temp, MapX, MapY;

  for (MapX = 0; MapX < WORLD_X; MapX++) {
    for (MapY = 0; MapY < WORLD_Y; MapY++) {
      if (IsTree(Map[MapX][MapY])) {
	bitindex = 0;
	for (z = 0; z < 4; z++) {
	  bitindex = bitindex << 1;
	  Xtem = MapX + DX[z];
	  Ytem = MapY + DY[z];
	  if (TestBounds(Xtem, Ytem) &&
	      IsTree(Map[Xtem][Ytem])) {
	    bitindex++;
	  }
	}
	temp = TEdTab[bitindex & 15];
	if (temp) {
	  if (temp != WOODS)
	    if ((MapX + MapY) & 1)
	      temp = temp - 8;
	  Map[MapX][MapY] = temp + BLBNBIT;
	}
	else Map[MapX][MapY] = temp;
      }
    }
  }
}


DoRivers(void)
{	

  LastDir = Rand(3);
  Dir = LastDir;
  DoBRiv();
  MapX = XStart;
  MapY = YStart;
  LastDir = LastDir ^ 4;
  Dir = LastDir;
  DoBRiv();	
  MapX = XStart;
  MapY = YStart;
  LastDir = Rand(3);
  DoSRiv();
}


DoBRiv(void)
{
  int r1, r2;

  if (CurveLevel < 0) {
    r1 = 100;
    r2 = 200;
  } else {
    r1 = CurveLevel + 10;
    r2 = CurveLevel + 100;
  }

  while (TestBounds (MapX + 4, MapY + 4)) {
    BRivPlop();
    if (Rand(r1) < 10) {
      Dir = LastDir;
    } else {
      if (Rand(r2) > 90) Dir++;
      if (Rand(r2) > 90) Dir--;
    }
    MoveMap(Dir);
  }
}


DoSRiv(void)
{
  int r1, r2;

  if (CurveLevel < 0) {
    r1 = 100;
    r2 = 200;
  } else {
    r1 = CurveLevel + 10;
    r2 = CurveLevel + 100;
  }

  while (TestBounds (MapX + 3, MapY + 3)) {
    SRivPlop();
    if (Rand(r1) < 10) {
      Dir = LastDir;
    } else {
      if (Rand(r2) > 90) Dir++;
      if (Rand(r2) > 90) Dir--;
    }
    MoveMap(Dir);
  }
}


PutOnMap(short Mchar, short Xoff, short Yoff)
{
  register short Xloc, Yloc, temp;

  if (Mchar == 0)
    return;
  Xloc = MapX + Xoff;
  Yloc = MapY + Yoff;
  if (TestBounds(Xloc, Yloc) == FALSE)
    return;
  if (temp = Map[Xloc][Yloc]) {
    temp = temp & LOMASK;
    if (temp == RIVER)
      if (Mchar != CHANNEL)
	return;
    if (temp == CHANNEL)
      return;
  }
  Map[Xloc][Yloc] = Mchar;	
}


BRivPlop(void)
{
  static short BRMatrix[9][9] = {
    { 0, 0, 0, 3, 3, 3, 0, 0, 0 },
    { 0, 0, 3, 2, 2, 2, 3, 0, 0 },
    { 0, 3, 2, 2, 2, 2, 2, 3, 0 },
    { 3, 2, 2, 2, 2, 2, 2, 2, 3 },
    { 3, 2, 2, 2, 4, 2, 2, 2, 3 },
    { 3, 2, 2, 2, 2, 2, 2, 2, 3 },
    { 0, 3, 2, 2, 2, 2, 2, 3, 0 },
    { 0, 0, 3, 2, 2, 2, 3, 0, 0 },
    { 0, 0, 0, 3, 3, 3, 0, 0, 0 } };
  short x, y;

  for (x = 0; x < 9; x++)
    for (y = 0; y < 9; y++)
      PutOnMap(BRMatrix[y][x], x, y);
}


SRivPlop(void)
{
  static short SRMatrix[6][6] = {
    { 0, 0, 3, 3, 0, 0 },
    { 0, 3, 2, 2, 3, 0 },
    { 3, 2, 2, 2, 2, 3 },
    { 3, 2, 2, 2, 2, 3 },
    { 0, 3, 2, 2, 3, 0 },
    { 0, 0, 3, 3, 0, 0 } };
  short x, y;

  for (x = 0; x < 6; x++)
    for (y = 0; y < 6; y++)
      PutOnMap(SRMatrix[y][x], x, y);
}


SmoothWater()
{
  int x, y;

  for(x = 0; x < WORLD_X; x++) {
    for(y = 0; y < WORLD_Y; y++) {
      /* If water: */
      if (((Map[x][y] & LOMASK) >= WATER_LOW) &&
	  ((Map[x][y] & LOMASK) <= WATER_HIGH)) {
	if (x > 0) {
	  /* If nearest object is not water: */
	  if (((Map[x - 1][y] & LOMASK) < WATER_LOW) ||
	      ((Map[x - 1][y] & LOMASK) > WATER_HIGH)) {
	    goto edge;
	  }
	}
	if (x < (WORLD_X - 1)) {
	  /* If nearest object is not water: */
	  if (((Map[x+1][y]&LOMASK) < WATER_LOW) ||
	      ((Map[x+1][y]&LOMASK) > WATER_HIGH)) {
	    goto edge;
	  }
	}
	if (y > 0) {
	  /* If nearest object is not water: */
	  if (((Map[x][y - 1] & LOMASK) < WATER_LOW) ||
	      ((Map[x][y-1]&LOMASK) > WATER_HIGH)) {
	    goto edge;
	  }
	}
	if (y < (WORLD_Y - 1)) {
	  /* If nearest object is not water: */
	  if (((Map[x][y + 1] & LOMASK) < WATER_LOW) ||
	      ((Map[x][y + 1] & LOMASK) > WATER_HIGH)) {
	  edge:
	    Map[x][y]=REDGE; /* set river edge */
	    continue;
	  }
	}
      }
    }
  }
  for (x = 0; x < WORLD_X; x++) {
    for (y = 0; y < WORLD_Y; y++) {
      /* If water which is not a channel: */
      if (((Map[x][y] & LOMASK) != CHANNEL) &&
	  ((Map[x][y] & LOMASK) >= WATER_LOW) &&
	  ((Map[x][y] & LOMASK) <= WATER_HIGH)) {
	if (x > 0) {
	  /* If nearest object is not water; */
	  if (((Map[x - 1][y] & LOMASK) < WATER_LOW) ||
	      ((Map[x - 1][y] & LOMASK) > WATER_HIGH)) {
	    continue;
	  }
	}
	if (x < (WORLD_X - 1)) {
	  /* If nearest object is not water: */
	  if (((Map[x + 1][y] & LOMASK) < WATER_LOW) ||
	      ((Map[x + 1][y] & LOMASK) > WATER_HIGH)) {
	    continue;
	  }
	}
	if (y > 0) {
	  /* If nearest object is not water: */
	  if (((Map[x][y - 1] & LOMASK) < WATER_LOW) ||
	      ((Map[x][y - 1] & LOMASK) > WATER_HIGH)) {
	    continue;
	  }
	}
	if (y < (WORLD_Y - 1)) {
	  /* If nearest object is not water: */
	  if (((Map[x][y + 1] & LOMASK) < WATER_LOW) ||
	      ((Map[x][y + 1] & LOMASK) > WATER_HIGH)) {
	    continue;
	  }
	}
	Map[x][y] = RIVER; /* make it a river */
      }
    }
  }
  for (x = 0; x < WORLD_X; x++) {
    for (y = 0; y < WORLD_Y; y++) {
      /* If woods: */
      if (((Map[x][y] & LOMASK) >= WOODS_LOW) &&
	  ((Map[x][y] & LOMASK) <= WOODS_HIGH)) {
	if (x > 0) {
	  /* If nearest object is water: */
	  if ((Map[x - 1][y] == RIVER) ||
	      (Map[x - 1][y] == CHANNEL)) {
	    Map[x][y] = REDGE; /* make it water's edge */
	    continue;
	  }
	}
	if (x < (WORLD_X - 1)) {
	  /* If nearest object is water: */
	  if ((Map[x + 1][y] == RIVER) ||
	      (Map[x + 1][y] == CHANNEL)) {
	    Map[x][y] = REDGE; /* make it water's edge */
	    continue;
	  }
	}
	if (y > 0) {
	  /* If nearest object is water: */
	  if ((Map[x][y - 1] == RIVER) ||
	      (Map[x][y - 1] == CHANNEL)) {
	    Map[x][y] = REDGE; /* make it water's edge */
	    continue;
	  }
	}
	if (y < (WORLD_Y - 1)) {
	  /* If nearest object is water; */
	  if ((Map[x][y + 1] == RIVER) ||
	      (Map[x][y + 1] == CHANNEL)) {
	    Map[x][y] = REDGE; /* make it water's edge */
	    continue;
	  }
	}
      }
    }
  }
}
