/* terragen.c:  Terrain generator
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
/* modified by Paul Schmidt 10-89 to implement terrain editor quickly... - rax */

#include "..\sim\sim.h"

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

#define WATER_LOW	2		/* range for water */
#define WATER_HIGH	20
#define WOODS_LOW	21		/* range for woods */
#define WOODS_HIGH	39

static int XStart, YStart;
static int  Dir, LastDir;

					/* trash values for GRand()  	*/
static int GRanArray[5] = { 1018,4521,202,419,3 }; 

extern int treeLevel;		/* level for tree creation (terra.c) */
extern int lakeLevel;		/* level for lake creation (terra.c) */
extern int curvLevel;		/* level for river curviness (terra.c) */

rax_ClearMap()
{
register int x, y;

	for (x=0; x<WORLD_X; x++)
		for (y=0; y<WORLD_Y; y++)
			Map[x][y] = 0;
}

rax_WaterEdges()		/* set water edges */
{
	register int x,y;		/* temporary counters */

	for(x=0;x < WORLD_X;x++) {
		for(y=0;y < WORLD_Y;y++) {
			if((Map[x][y]&LOMASK) >= WATER_LOW && (Map[x][y]&LOMASK) <= WATER_HIGH) {		/* if water */
				if(x > 0) {
					if((Map[x-1][y]&LOMASK) < WATER_LOW || (Map[x-1][y]&LOMASK) > WATER_HIGH) {		/* if nearest object is not water */
						goto edge;
					}
				}
				if(x < WORLD_X-1) {
					if((Map[x+1][y]&LOMASK) < WATER_LOW || (Map[x+1][y]&LOMASK) > WATER_HIGH) {		/* if nearest object is not water */
						goto edge;
					}
				}
				if(y > 0) {
					if((Map[x][y-1]&LOMASK) < WATER_LOW || (Map[x][y-1]&LOMASK) > WATER_HIGH) {		/* if nearest object is not water */
						goto edge;
					}
				}
				if(y < WORLD_Y-1) {
					if((Map[x][y+1]&LOMASK) < WATER_LOW || (Map[x][y+1]&LOMASK) > WATER_HIGH) {		/* if nearest object is not water */
edge:
						Map[x][y]=REDGE;		/* set river edge */
						continue;
					}
				}
			}
		}
	}
	for(x=0;x < WORLD_X;x++) {
		for(y=0;y < WORLD_Y;y++) {
			if((Map[x][y]&LOMASK) != CHANNEL && (Map[x][y]&LOMASK) >= WATER_LOW && (Map[x][y]&LOMASK) <= WATER_HIGH) {		/* if water which is not a channel */
				if(x > 0) {
					if((Map[x-1][y]&LOMASK) < WATER_LOW || (Map[x-1][y]&LOMASK) > WATER_HIGH) {		/* if nearest object is not water */
						continue;
					}
				}
				if(x < WORLD_X-1) {
					if((Map[x+1][y]&LOMASK) < WATER_LOW || (Map[x+1][y]&LOMASK) > WATER_HIGH) {		/* if nearest object is not water */
						continue;
					}
				}
				if(y > 0) {
					if((Map[x][y-1]&LOMASK) < WATER_LOW || (Map[x][y-1]&LOMASK) > WATER_HIGH) {		/* if nearest object is not water */
						continue;
					}
				}
				if(y < WORLD_Y-1) {
					if((Map[x][y+1]&LOMASK) < WATER_LOW || (Map[x][y+1]&LOMASK) > WATER_HIGH) {		/* if nearest object is not water */
						continue;
					}
				}
				Map[x][y]=RIVER;		/* make it a river */
			}
		}
	}
	for(x=0;x < WORLD_X;x++) {
		for(y=0;y < WORLD_Y;y++) {
			if((Map[x][y]&LOMASK) >= WOODS_LOW && (Map[x][y]&LOMASK) <= WOODS_HIGH) {		/* if woods */
				if(x > 0) {
					if(Map[x-1][y] == RIVER || Map[x-1][y] == CHANNEL) {		/* if nearest object is water */
						Map[x][y]=REDGE;		/* make it water's edge */
						continue;
					}
				}
				if(x < WORLD_X-1) {
					if(Map[x+1][y] == RIVER || Map[x+1][y] == CHANNEL) {		/* if nearest object is water */
						Map[x][y]=REDGE;		/* make it water's edge */
						continue;
					}
				}
				if(y > 0) {
					if(Map[x][y-1] == RIVER || Map[x][y-1] == CHANNEL) {		/* if nearest object is water */
						Map[x][y]=REDGE;		/* make it water's edge */
						continue;
					}
				}
				if(y < WORLD_Y-1) {
					if(Map[x][y+1] == RIVER || Map[x][y+1] == CHANNEL) {		/* if nearest object is water */
						Map[x][y]=REDGE;		/* make it water's edge */
						continue;
					}
				}
			}
		}
	}
}

#define RADIUS 18

rax_MakeIsland()
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
		MapY= rax_EGRand(RADIUS);
		rax_BRivPlop();
		MapY= 90-rax_EGRand(RADIUS);
		rax_BRivPlop();
		MapY= 0;
		rax_SRivPlop();
		MapY= 94;
		rax_SRivPlop();
	}
	for (y=0; y<WORLD_Y-5; y+=2)	{
		MapY= y ;
		MapX= rax_EGRand(RADIUS);
		rax_BRivPlop();
		MapX= 110-rax_EGRand(RADIUS);
		rax_BRivPlop();
		MapX= 0;
		rax_SRivPlop();
		MapX= 114;
		rax_SRivPlop();
	}
}

rax_MakeLakes()
{
	int Lim1, Lim2, t, z;
	register int x, y;
	
/*	Lim1 = rax_GRand(10);		/**/
	Lim1=lakeLevel/2;
	for (t = 0; t < Lim1; t++)  {
		x = rax_GRand(99) + 10;
		y = rax_GRand(80) + 10;
		Lim2 = rax_GRand(12)+2;
		for (z = 0; z < Lim2; z++)  {
			MapX = x - 6 + rax_GRand(12);
			MapY = y - 6 + rax_GRand(12);
			if (rax_GRand(4))  rax_SRivPlop();
			else  rax_BRivPlop();
		}
	}
}

rax_GetRandStart()
{
	XStart = 40 + rax_GRand(40);
	YStart = 33 + rax_GRand(33);
	MapX = XStart;
	MapY = YStart;
}

rax_DoTrees()
{
	int x,xloc,yloc;

	for(x=0;x < (treeLevel*3);x++) {
		xloc=rax_GRand(119);
		yloc=rax_GRand(99);
		rax_TreeSplash(xloc,yloc);
	}
	rax_SmoothTrees();
	rax_SmoothTrees();
}

rax_TreeSplash(xloc,yloc)
int xloc,yloc;
{
	int  Dis, Dir;
	register int xoff, yoff,z;
	
	Dis = rax_GRand(100+(treeLevel*2))+50;
	MapX = xloc;
	MapY = yloc;
	for (z=0; z<Dis; z++)	{
		Dir = rax_GRand(7);
		rax_MoveMap(Dir);
		if (!(rax_TestBounds(MapX,MapY))) return;
		if (Map[MapX][MapY] == 0)  Map[MapX][MapY] = WOODS+ BLN;
	}
}
	
rax_SmoothRiver()
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
 				if (rax_TestBounds(Xtem, Ytem))
/* 					if(Map[Xtem][Ytem]) bitindex++;		/* original code */
 					if((Map[Xtem][Ytem]&LOMASK) && ((Map[Xtem][Ytem]&LOMASK) < WOODS_LOW || (Map[Xtem][Ytem]&LOMASK) > WOODS_HIGH)) bitindex++;		/* new code - rax */
 			}
 			temp = REdTab[bitindex & 15];
 			if ((temp != 2) && (rax_GRand(1))) temp++;
 			Map[MapX][MapY] = temp;
 		}
 }

IsTree(cell)		/* return TRUE or FALSE if cell value is a tree cell */
register int cell;
{
	if((cell&LOMASK) >= WOODS_LOW && (cell&LOMASK) <= WOODS_HIGH) return TRUE; else return FALSE;
}
 
rax_SmoothTrees()
{
 static int DX[4] = {-1, 0, 1, 0};
 static int DY[4] = { 0, 1, 0,-1};
 static int TEdTab[16] = {0,0,0,34,0,0,36,35,0,32,0,33,30,31,29,37};
 int   bitindex, z,Xtem,Ytem;
 register int temp,MapX,MapY;
 
 for (MapX = 0; MapX < WORLD_X; MapX++)
 	for (MapY = 0; MapY < WORLD_Y; MapY++)
 		if (IsTree(Map[MapX][MapY]))	{
 			bitindex = 0;
 			for (z=0; z<4; z++)	{
 				bitindex = bitindex << 1;
 				Xtem = MapX + DX[z];
 				Ytem = MapY + DY[z];
 				if (rax_TestBounds(Xtem, Ytem))		
 					if(IsTree(Map[Xtem][Ytem])) bitindex++;
 				
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
 
rax_DoRivers()
{	
	LastDir = rax_GRand(3);
	Dir = LastDir;
	rax_DoBRiv();
	MapX = XStart;
	MapY = YStart;
	LastDir = LastDir ^ 4;
	Dir = LastDir;
	rax_DoBRiv();	
	MapX = XStart;
	MapY = YStart;
	LastDir = rax_GRand(3);
	rax_DoSRiv();
}

rax_DoBRiv()
{
	while(rax_TestBounds (MapX+4, MapY+4)) {
		rax_BRivPlop();
		if(rax_GRand(curvLevel+10) < 10) {
			Dir=LastDir;
		} else {
			if(rax_GRand(curvLevel+100) > 90) {
				Dir++;
			}
			if(rax_GRand(curvLevel+100) > 90) {
				Dir--;
			}
		}
		rax_MoveMap(Dir);
	}
}

rax_DoSRiv()
{
	while(rax_TestBounds (MapX+3, MapY+3)) {
		rax_SRivPlop();
		if(rax_GRand(curvLevel+10) < 10) {
			Dir=LastDir;
		} else {
			if(rax_GRand(curvLevel+100) > 90) {
				Dir++;
			}
			if(rax_GRand(curvLevel+100) > 90) {
				Dir--;
			}
		}
		rax_MoveMap(Dir);
	}
}

rax_MoveMap	(dir)
int dir;
{
static int DirTab[2][8] ={		{ 0, 1, 1, 1, 0, -1, -1, -1},
								{-1,-1, 0, 1, 1,  1,  0, -1}	};
	dir = dir & 7;
	MapX += DirTab[0][dir];
	MapY += DirTab[1][dir];
}




rax_BRivPlop()
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
			rax_PutOnMap (BRMatrix[y][x], x, y);
}

rax_SRivPlop()
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
			rax_PutOnMap (SRMatrix[y][x], x, y);
}

rax_PutOnMap (Mchar, Xoff, Yoff)
int Mchar, Xoff, Yoff;
{
register int Xloc, Yloc, temp;

	if (Mchar == 0) return;
	Xloc = MapX + Xoff;
	Yloc = MapY + Yoff;
	if (rax_TestBounds (Xloc, Yloc) == FALSE) return (FALSE);
	if (temp = Map [Xloc][Yloc])	{
		temp = temp & 1023;
		if (temp == RIVER) 
			if (Mchar != CHANNEL)
				return (FALSE);
		if (temp == CHANNEL) return (FALSE);
	}					
	Map [Xloc][Yloc] = Mchar;	
}

rax_TestBounds(x, y)
register int x, y;
{
	if ((( x >= 0) && (x < WORLD_X)) && (( y >= 0) && (y < WORLD_Y)))
		return (TRUE);
	return (FALSE);
}



rax_EGRand(limit)
int limit;
{
	int x,z;
	
	z= rax_GRand(limit);
	x= rax_GRand(limit);
	if (z < x) return(z);
	return(x);
}

#define RANMASK 32767

rax_GRand(range)			/* stupid but works  */
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
