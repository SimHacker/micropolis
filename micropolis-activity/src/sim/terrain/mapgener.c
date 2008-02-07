/* mapgener.c
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

/* Micropolis simulator code.	Copyright 1988, 1989 Maxis, Will Wright */

#include "sim.h"

#define TRUE 1
#define FALSE 0
#define WORLD_X 120
#define WORLD_Y 100
#define RIVER 2
#define REDGE 3
#define CHANNEL 4
#define WOODS 37
#define BL 4096
#define BN 8192
#define BLN BL+BN

static int XStart, YStart;
static int  Dir, LastDir;

					/* trash values for GRand()  	*/
static int GRanArray[5] = { 1018,4521,202,419,3 }; 

far GenerateMap()
{
	GRanArray[0] = TickCount();  /* This is the only machine-specific call  */
								 /* It inits GRand() w/ clockcount(long)	*/
	
	if (!(GRand(10)))  {			/* 1 in 3 chance that island is generated  	*/
		MakeIsland();
		return;
	}
	ClearMap();					/* else make river map						*/
	GetRandStart();
	DoRivers();
	MakeLakes();
	SmoothRiver();
	DoTrees();
}

near ClearMap()
{
register int x, y;

	for (x=0; x<WORLD_X; x++)
		for (y=0; y<WORLD_Y; y++)
			Map[x][y] = 0;
}

#define RADIUS 18

near MakeIsland()
{
	register int x,y,z;
	
	for (x=0; x<WORLD_X; x++)
		for (y=0; y<WORLD_Y; y++)
			Map[x][y] = RIVER;
	for (x=5; x<WORLD_X-5; x++)
		for (y=5; y<WORLD_Y-5; y++)
			Map[x][y] = 0;
	for (x=0; x<WORLD_X-5; x+=2)	{
		MapX= x ;
		MapY= EGRand(RADIUS);
		BRivPlop();
		MapY= 90-EGRand(RADIUS);
		BRivPlop();
		MapY= 0;
		SRivPlop();
		MapY= 94;
		SRivPlop();
	}
	for (y=0; y<WORLD_Y-5; y+=2)	{
		MapY= y ;
		MapX= EGRand(RADIUS);
		BRivPlop();
		MapX= 110-EGRand(RADIUS);
		BRivPlop();
		MapX= 0;
		SRivPlop();
		MapX= 114;
		SRivPlop();
	}
	SmoothRiver();
	DoTrees();
}

near MakeLakes()
{
	int Lim1, Lim2, t, z;
	register int x, y;
	
	Lim1 = GRand(10);
	for (t = 0; t < Lim1; t++)  {
		x = GRand(99) + 10;
		y = GRand(80) + 10;
		Lim2 = GRand(12)+2;
		for (z = 0; z < Lim2; z++)  {
			MapX = x - 6 + GRand(12);
			MapY = y - 6 + GRand(12);
			if (GRand(4))  SRivPlop();
			else  BRivPlop();
		}
	}
}

near GetRandStart()
{
	XStart = 40 + GRand(40);
	YStart = 33 + GRand(33);
	MapX = XStart;
	MapY = YStart;
}

near DoTrees()
{
	int Amount,x,xloc,yloc;

	Amount = GRand(100)+50;
	for (x=0; x<Amount; x++)  {
		xloc = GRand(119);
		yloc = GRand(99);
		TreeSplash(xloc,yloc);
	}
	SmoothTrees();
	SmoothTrees();
}

near TreeSplash(xloc,yloc)
int xloc,yloc;
{
	int  Dis, Dir;
	register int xoff, yoff,z;
	
	Dis = GRand(150)+50;
	MapX = xloc;
	MapY = yloc;
	for (z=0; z<Dis; z++)	{
		Dir = GRand(7);
		MoveMap(Dir);
		if (!(TestBounds(MapX,MapY))) return;
		if (Map[MapX][MapY] == 0)  Map[MapX][MapY] = WOODS+ BLN;
	}
}
	
far SmoothRiver()
{
 static int DX[4] = {-1, 0, 1, 0};
 static int DY[4] = { 0, 1, 0,-1};
 static int REdTab[16] = { 13+BL,13+BL,17+BL,15+BL,5+BL,2,19+BL,
 						   17+BL,9+BL,11+BL,2,13+BL,7+BL,9+BL,5+BL,2};
 int  bitindex, z,Xtem,Ytem;
 register int temp,MapX,MapY;
 
 for (MapX = 0; MapX < WORLD_X; MapX++)
 	for (MapY = 0; MapY < WORLD_Y; MapY++)
 		if (Map[MapX][MapY] == REDGE)	{
 			bitindex = 0;
 			for (z=0; z<4; z++)	{
 				bitindex = bitindex << 1;
 				Xtem = MapX + DX[z];
 				Ytem = MapY + DY[z];
 				if (TestBounds(Xtem, Ytem))
 					if ( Map[Xtem][Ytem]) bitindex++;
 			}
 			temp = REdTab[bitindex & 15];
 			if ((temp != 2) && (GRand(1))) temp++;
 			Map[MapX][MapY] = temp;
 		}
 }

far SmoothTrees()
{
 static int DX[4] = {-1, 0, 1, 0};
 static int DY[4] = { 0, 1, 0,-1};
 static int TEdTab[16] = {0,0,0,34,0,0,36,35,0,32,0,33,30,31,29,37};
 int   bitindex, z,Xtem,Ytem;
 register int temp,MapX,MapY;
 
 for (MapX = 0; MapX < WORLD_X; MapX++)
 	for (MapY = 0; MapY < WORLD_Y; MapY++)
 		if ((Map[MapX][MapY] & BLN) == BLN)	{
 			bitindex = 0;
 			for (z=0; z<4; z++)	{
 				bitindex = bitindex << 1;
 				Xtem = MapX + DX[z];
 				Ytem = MapY + DY[z];
 				if (TestBounds(Xtem, Ytem))		
 					if (Map[Xtem][Ytem] & BN) bitindex++;
 				
 			}
 			temp = TEdTab[bitindex & 15];
 			if (temp) {
 				if (temp != 37) 
 					if ((MapX+MapY) & 1)
 						temp = temp-8;
 				Map[MapX][MapY] = temp+BLN;
 			}
 			else Map[MapX][MapY] = temp;
 		}
 }
 
near DoRivers()
{	

	LastDir = GRand(3);
	Dir = LastDir;
	DoBRiv();
	MapX = XStart;
	MapY = YStart;
	LastDir = LastDir ^ 4;
	Dir = LastDir;
	DoBRiv();	
	MapX = XStart;
	MapY = YStart;
	LastDir = GRand(3);
	DoSRiv();
}

near DoBRiv()
{
int temp, count;

	count = 0;
	while (TestBounds (MapX+4, MapY+4))		{
		BRivPlop();
		if (GRand(10) > 4) Dir++;
		if (GRand(10) > 4) Dir--;
		if (!(GRand(10))) Dir = LastDir;	
		MoveMap (Dir);
	}
}

near DoSRiv()
{
int temp;

	while (TestBounds (MapX+3, MapY+3))		{
		SRivPlop();
		if (GRand(10) > 5) Dir++;
		if (GRand(10) > 5) Dir--;
		if (!(GRand(12))) Dir = LastDir;
		MoveMap (Dir);
	}
}

near MoveMap	(dir)
int dir;
{
static int DirTab[2][8] ={		{ 0, 1, 1, 1, 0, -1, -1, -1},
								{-1,-1, 0, 1, 1,  1,  0, -1}	};
	dir = dir & 7;
	MapX += DirTab[0][dir];
	MapY += DirTab[1][dir];
}




near BRivPlop()
{
static int BRMatrix[9][9] ={
		{0,0,0,3,3,3,0,0,0},
		{0,0,3,2,2,2,3,0,0},
		{0,3,2,2,2,2,2,3,0},
		{3,2,2,2,2,2,2,2,3},
		{3,2,2,2,4,2,2,2,3},
		{3,2,2,2,2,2,2,2,3},
		{0,3,2,2,2,2,2,3,0},
		{0,0,3,2,2,2,3,0,0},
		{0,0,0,3,3,3,0,0,0}		};
int x, y;

	for (x=0; x<9; x++)
		for (y=0; y<9; y++)
			PutOnMap (BRMatrix[y][x], x, y);
}

near SRivPlop()
{
static int SRMatrix[6][6] ={
		{0,0,3,3,0,0},
		{0,3,2,2,3,0},
		{3,2,2,2,2,3},
		{3,2,2,2,2,3},
		{0,3,2,2,3,0},
		{0,0,3,3,0,0}		};
int x, y;

	for (x=0; x<6; x++)
		for (y=0; y<6; y++)
			PutOnMap (SRMatrix[y][x], x, y);
}

near PutOnMap (Mchar, Xoff, Yoff)
int Mchar, Xoff, Yoff;
{
register int Xloc, Yloc, temp;

	if (Mchar == 0) return;
	Xloc = MapX + Xoff;
	Yloc = MapY + Yoff;
	if (TestBounds (Xloc, Yloc) == FALSE) return (FALSE);
	if (temp = Map [Xloc][Yloc])	{
		temp = temp & 1023;
		if (temp == RIVER) 
			if (Mchar != CHANNEL)
				return (FALSE);
		if (temp == CHANNEL) return (FALSE);
	}					
	Map [Xloc][Yloc] = Mchar;	
}

far TestBounds(x, y)
register int x, y;
{
	if ((( x >= 0) && (x < WORLD_X)) && (( y >= 0) && (y < WORLD_Y)))
		return (TRUE);
	return (FALSE);
}



near EGRand(limit)
int limit;
{
	int x,z;
	
	z= GRand(limit);
	x= GRand(limit);
	if (z < x) return(z);
	return(x);
}

#define RANMASK 32767

near GRand(range)			/* stupid but works  */
int range;
{
	register x, newv, divisor;
	
	divisor = RANMASK/ (range+1);
	newv = 0;
	for (x=4; x!=0; x--)
		newv += (GRanArray[x] = GRanArray[x-1]);
	GRanArray[0] = newv;
	x = (newv & RANMASK) / divisor;
	if (x > range)  return(range);
	return(x);
}
