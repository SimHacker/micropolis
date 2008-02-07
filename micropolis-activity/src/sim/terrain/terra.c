/* terra.c:  Terrain editor
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

/* terra.c	-	terrain editor for Sim City
					by Paul Schmidt, 1989
					Raxsoft, Inc.
					1194 Spring Valley Commons
					Livermore, CA 94550
					(415) 449-9079
*/

#include "..\sim\sim.h"
#include "..\gr\menu.h"
#include "..\gr\grdef.h"
#include "..\terra\tmenu.h"
#include "..\ed\eddef.h"
#include "..\ed\edext.h"
#include <setjmp.h>
#include <dos.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys\types.h>
#include <sys\stat.h>
#include <direct.h>

char *GSaveRect();

extern MOUSESTATE near mouse_state;
extern FILE *demoFP;
extern char realMouseFlag;

extern long lastEvent, TickCount();		/* from ..\sim\main.c */
extern long policeFund, fireFund, transFund;

char CreateWithIsland;

#define UMaps 4
#define URecs 5000
int UndoMapBuffer1[WORLD_X*WORLD_Y];
int UndoMapBuffer2[WORLD_X*WORLD_Y];
int UndoMapBuffer3[WORLD_X*WORLD_Y];
int UndoMapBuffer4[WORLD_X*WORLD_Y];
int *UndoMap[UMaps]={UndoMapBuffer1,UndoMapBuffer2,UndoMapBuffer3,UndoMapBuffer4};		/* maps for fill undos */
int UndoMaps;		/* number of occupied undo maps */
struct {
	char x,y;		/* coordinate of undo */
	int val;			/* cell value */
} UndoRec[URecs];		/* putdown undos */
int UndoHead=0,UndoTail=0;

char fillState=0;			/* flag telling if fill mode is on or off */

int treeLevel=50;		/* level for tree creation */
int lakeLevel=50;		/* level for lake creation */
int curvLevel=50;		/* level for river curviness */

/* ========================================================== */

void ClearUndo()		/* clear all undo records */
{
	UndoMaps=UndoHead=UndoTail=0;		/* kill undo pointers */
	DrawAllEdIcons(UPDATE);		/* update undo icon */
}

void AddUndo(pos)		/* add undo record */
Point pos;		/* position to add to undo */
{

	if(pos.h != -1 || pos.v != -1) {		/* if they're not setting to undo the entire map */
		if(pos.h > WORLD_X || pos.v > WORLD_Y || pos.h < 0 || pos.v < 0) {		/* if out of bounds */
			return;		/* do nothing */
		}
	}
	if(pos.h == -1 && pos.v == -1) {		/* if undo entire map */
		register int tem;		/* temporary counter */
		register int x,y;		/* temporary coordinates */

		if(UndoMaps == UMaps) {		/* if there are already five undo maps */
			while(UndoRec[UndoTail].x != -1 || UndoRec[UndoTail].y != -1) {		/* until we find the last map undo */
				UndoTail=Bound(0,UndoTail+1,URecs-1);		/* move tail */
			}
			UndoTail=Bound(0,UndoTail+1,URecs-1);		/* move tail to kill the last map */
			UndoMaps--;
			for(tem=0;tem < UndoMaps;tem++) {		/* for each map */
				for(x=0;x < WORLD_X*WORLD_Y;x++) {
					*(UndoMap[tem]+x)=*(UndoMap[tem+1]+x);
				}
			}
		}
		for(x=0;x < WORLD_X*WORLD_Y;x++) {
			*(UndoMap[UndoMaps]+x)=*((int*)Map+x);		/* save current map */
		}
		UndoMaps++;		/* one more map saved */
	}
	UndoRec[UndoHead].x=pos.h;		/* set x and y position */
	UndoRec[UndoHead].y=pos.v;
	UndoRec[UndoHead].val=Map[pos.h][pos.v];		/* set map value */
	UndoHead=Bound(0,UndoHead+1,URecs-1);		/* move head */
	if(UndoHead == UndoTail) {		/* if we pushed the tail up */
		UndoTail=Bound(0,UndoTail+1,URecs-1);		/* move tail */
	}
}

void Undo()		/* undo one map operation */
{
	register int x,y;

	if(UndoHead == UndoTail) {		/* if there's nothing to undo */
		MakeSound(7);		/* bad sound */
		return;		/* do nothing */
	}
	UndoHead=Bound(0,UndoHead-1,URecs-1);		/* set new head */
	if(UndoRec[UndoHead].x == -1) {		/* if the entire map changed */
		register int tem;		/* temporary counter */
		register int x,y;		/* temporary coordinates */

		for(x=0;x < WORLD_X*WORLD_Y;x++) {
			*((int*)Map+x)=*(UndoMap[UndoMaps-1]+x);		/* set undo map */
		}
		UndoMaps--;		/* one less map */
	} else {
		Map[UndoRec[UndoHead].x][UndoRec[UndoHead].y]=UndoRec[UndoHead].val;		/* set old value */
	}

	UpdateOurMaps();		/* update big and small maps */
}

/* ======================================================================== */

DoEvent()
{

	int object;
	EVENT event;

#if DEBUG && 0
	PrintLock("DoEvent - get event ");
#endif
	GetMouseEvent(&event);
	object=event.object;
#if DEBUG && 0
	PrintLock("DoEvent ");
#endif

	switch(object&0xff00)
	{
		case 0:	DoMenu(object);
				break;
		case 0x100:
				DoEdEvent(&event);
				break;
		case 0x200:
				DoKeyDown(object);
				break;
		case 0x300:
				MenuEvent(&event);
				break;
		case 0x400:
				DoMapEvent(&event);
				break;
		case 0x600:
				DoScreen(&event);
				break;
	}
}

UpdateOurSmallMap()		/* update small map */
{
	register int tem;		/* temporary counter */
	
	MouseHide();		/* hide mouse */
	DrawSmallMap();		/* updates small map */
	for(tem=0;tem < WORLD_X*WORLD_Y;tem++) {
		*((int*)lastSmallMap+tem)=(*((int*)Map+tem))&LOMASK;		/* copy map */
	}
	MouseShow();		/* show mouse */
}

UpdateOurMaps()		/* update edit and map widnows */
{
	UpdateMapCursorOff();		/* turn off map cursor */
	MouseHide();		/* hide mouse */
	DrawBigMap();		/* updates large map */
	MouseShow();		/* show mouse */
	UpdateOurSmallMap();
	DrawAllEdIcons(UPDATE);		/* update icons */
	UpdateMapCursorOn();		/* turn map cursor on */
}

/* ======================================================================== */
DoMenu(itemNum)
int itemNum;
{
	static char far *popGameLevelStrs[]={"Easy","Medium","Hard",NULL};
	int cur_windowActive;		/* current active window */
	register int x,y;		/* temporary integers */
	Rect msgBox;
	Rect msgRect;
	char *savePtr;		/* pointer to saved rectangle */
	int smoothFlag;
	long tmpFunds;
	int virginCity=YES;

	switch(itemNum)
	{
/*		case MNUSCNO:
/*					/* Load a scenario - if it fails we need to redraw the */
/*					/* screen */
/*				if (!LoadScenario()) {
/*					InitWindows();
/*				} else {
/*					ClearUndo();		/* clear undo records */
/*				}
/*				SelectNewWindow(EDWINDOW);		/* enable edit window */
/*				SelectNewWindow(MAPWINDOW);		/* enable map window */
/*				break;		/**/
		case MNUQUIT:
				if (Verify("EXIT"))
					Quit("Micropolis Terrain Editor");
				break;
		case MNU_SoundToggle:		/* sound on/off */
				userSoundOn^=YES;
sso:
				ShowSelectedOptions();
				break;
		case MNUABOUT:
				DoAbout();
				break;
		case MNULOAD:
				if (LoadGame(NULL,NO))
					virginCity=NO;
				SelectNewWindow(EDWINDOW);		/* enable edit window */
				SelectNewWindow(MAPWINDOW);		/* enable map window */
				ClearUndo();		/* clear undo records */
				break;
		case MNUSAVE:
				SaveGame(lastFileName);
				break;
		case MNUSVAS:
				SaveGame(NULL);
				break;
		case MNUNEW: /* Start new game */
				if (Verify("NEW GAME")) {
					totalFunds=20000L;
					NewGame(NO);
					transFund=policeFund=fireFund=65535L;
					CityTax=7;
					virginCity=YES;
					ClearUndo();		/* clear undo records */
				}
				SelectNewWindow(EDWINDOW);		/* enable edit window */
				SelectNewWindow(MAPWINDOW);		/* enable map window */
				break;
		case MNUPRINT:
				PrintCity();
				break;
		case MNU_SmoothTrees:
			smoothFlag=1;
			goto dosmooth;
		case MNU_ClearMap:		/* if map is to be cleared */
			AddUndo(-1,-1);		/* save map */
			rax_ClearMap();		/* clear map (..\sim\mapgener.c) */
			GameLevel=0;		/* assume game is easy */
			goto updateBoth;
			break;
		case MNU_ClearUnnatural:		/* if unnatural objects are to be cleared */
			AddUndo(-1,-1);		/* save map */
			for(x=0;x < WORLD_X;x++) {
				for(y=0;y < WORLD_Y;y++) {
					if((Map[x][y]&LOMASK) > 37) {		/* get rid of everything unnatural */
						Map[x][y]=0;		/* turn it into dirt */
					}
				}
			}
updateBoth:
			UpdateOurMaps();		/* update both windows */
			break;
		case MNU_GenerateRandom:		/* if random map is to be generated */
			if(SetTerrainParameters()) {		/* allow user to set terrain generation parameters */
				CenterRect(&msgBox, 20, 5);
				msgRect=*AdjRect(&msgBox);
				savePtr=GSaveRect(&msgRect);		/* save rectangle */
				GRectFill(&msgRect, WHITE|PWHITE);
				GSetAttrib(DGREEN, DGREEN, PBLACK);
				GRectOutline(&msgRect, 4);
				GSetAttrib(LGREEN, LGREEN, PMGREY);
				GRectOutline(&msgRect, 2);
				GSetAttrib(DBLUE, WHITE, PWHITE|PINV);
				CenterPrint(&msgBox, msgBox.top+2, "Now terraforming");

				AddUndo(-1,-1);		/* save map */
				rax_ClearMap();					/* make river map */
				rax_GetRandStart();
				if(CreateWithIsland) {		/* if we're creating an island */
					rax_MakeIsland();		/* make an island */
				}
				if(curvLevel) {		/* if we're supposedly creating an island */
					rax_DoRivers();		/* create river */
				}
				if(lakeLevel) {		/* if there are to be lakes */
					rax_MakeLakes();		/* add lakes */
				}
				rax_SmoothRiver();		/* smooth out river */
				if(treeLevel) {		/* if we're creating a woodsy terrain */
					rax_DoTrees();		/* add trees */
				}
				rax_SmoothTrees();		/* smooth trees */
				rax_SmoothTrees();		/* smooth trees */

				GRestoreRect(&msgRect,savePtr);		/* restore rectangle */
				MapX=MapY=0;
				goto updateBoth;
			}
			break;
		case MNU_SmoothRiver:		/* if water is to be smoothed */
			smoothFlag=2;
			goto dosmooth;
		case MNU_SmoothBoth:		/* if both are to be smoothed */
			smoothFlag=3;			/* Both */
dosmooth:
			CenterRect(&msgBox, 16, 5);
			msgRect=*AdjRect(&msgBox);
			savePtr=GSaveRect(&msgRect);		/* save rectangle */
			GRectFill(&msgRect, WHITE|PWHITE);
			GSetAttrib(DGREEN, DGREEN, PBLACK);
			GRectOutline(&msgRect, 4);
			GSetAttrib(LGREEN, LGREEN, PMGREY);
			GRectOutline(&msgRect, 2);
			GSetAttrib(DBLUE, WHITE, PWHITE|PINV);
			CenterPrint(&msgBox, msgBox.top+2, "Smoothing...");

			AddUndo(-1,-1);		/* save map */
			if (smoothFlag & 2)	/* If water smooth flag set */
			{
				rax_WaterEdges();		/* make sure water edges are ok */
				rax_SmoothRiver();
			}
			if (smoothFlag & 1)
			{
				rax_SmoothTrees();
				rax_SmoothTrees();
			}

			GRestoreRect(&msgRect,savePtr);		/* restore rectangle */
			goto updateBoth;

		case MNU_RandomIsland:		/* if random Island toggle */
			CreateWithIsland^=1;		/* toggle state of flag */
			ShowSelectedOptions();		/* update menu flag status */
			break;

		case MNU_GameLevel:		/* if game level */
/*			GameLevel++;		/* add one for PopUpMenuBox */
/*			PopUpMenuBox(&GameLevel, popGameLevelStrs);
/*			GameLevel--;		/**/
			
			tmpFunds=totalFunds;	/* Don't alter the amount of funds! */
			ChooseGameLevel();		/* get city name & level from user */
			if (!virginCity)
				totalFunds=tmpFunds;
			NewName();		/* set the name of the city */
			break;
		case MNU_GameYear:		/* if game year */
			SetGameYear();		/* allow user to set the game year */
			break;

		case MNU_EditWindow:		/* if edit window */
			SelectNewWindow(EDWINDOW);		/* bring the edit window to the foreground */
			break;
		case MNU_MapWindow:		/* if map window */
			SelectNewWindow(MAPWINDOW);		/* bring the map window to the foreground */
			break;

		default:
			/* unimplemented option */
			break;
	}
}

long messageTime;

DoEdEvent(ePtr)
EVENT *ePtr;
{
	int object;

	object=ePtr->object&0xff;

	if (object & 0xf0)
	{	/* Must be an icon */
		SetWandState(object&0xf, YES);
	}
	switch (object)
	{	
		case 0: DoEdMapEvent(ePtr);
				break;
		case 2: DoMoveWindow(ePtr, edWinList);
				break;
		case 8: ResizeEdWindow(ePtr);
				break;
	}
}

/* ======================================================================== */
DoMapEvent(ePtr)
EVENT *ePtr;
{
	int object;

	object=ePtr->object&0xff;

	{
		switch (object)
		{	case 0:	DoMapWindowEvent(ePtr);		/* move locus of edit window */
					break;
			case 2: DoMoveMap(ePtr);		/* move window */
					break;
		}
	}
}

void EditTerra()		/* main program entry point */
{
	int updateWait=0, updateWait2=0;

	CreateWithIsland=0;
	ShowSelectedOptions();		/* update menu flag status */
	GameLevel=0;		/* assume game is easy */
	totalFunds=20000L;
	transFund=policeFund=fireFund=65535L;
	CityTax=7;

	EnableMenus();		/* make sure menus are enabled */
	for (;;)
	{
		long lastMapCursor;		/* time value for map window cursor flashing */

		if (WaitedEnough(&lastMapCursor,2))
		{		/* if we should flash the map cursor */
			UpdateMapCursor();		/* update the map cursor */
		}


		if (!menuActive)		/* if no menus are active */
		{
			char moveFlag;
			if (CheckKeyState(CNTRLSCAN))	/* Is the control key hit? */
			{
				int dx, dy;
				dx=joyMoveX;
				dy=joyMoveY;
				
				if (CheckKeyState(HOMESCAN))	/* Home arrow */
				{	dx--; dy--;
				}
				if (CheckKeyState(PGUPSCAN))	/* PgUp arrow */
				{ 	dx++; dy--;
				}
				if (CheckKeyState(ENDSCAN))	/* End arrow */
				{ 	dx--; dy++;
				}
				if (CheckKeyState(PGDNSCAN))	/* PgDn arrow */
				{ 	dx++; dy++;
				}
				if (CheckKeyState(RIGHTSCAN))	/* Right arrow */
					dx++;
				if (CheckKeyState(LEFTSCAN))	/* Left arrow */
					dx--;
				if (CheckKeyState(UPSCAN))	/* Up arrow */
					dy--;
				if (CheckKeyState(DOWNSCAN))	/* Down arrow */
					dy++;
				if (dx || dy)
				{
					DisableMenus();
					freeze=YES;
					EdScroll(dx,dy);
					freeze=NO;
				}
			}
			if (!ScrollLock())
			{
moveagn:
				moveFlag=NO;
				if (mouse_state.x <= 1)
				{	
					if (MapX > 0)
					{
						MapX--;
						moveFlag=YES;
					}
				} else if (mouse_state.x >= screenWidth-6)
				{
					if (MapX+edWinWid < WORLD_X)
					{
						MapX++;
						moveFlag=YES;
					}
				}
				if (mouse_state.y < 1 )
				{
					if (MapY > 0)
					{
						MapY--;
						moveFlag=YES;
					}
				}
				else if (mouse_state.y >= screenHeight-6)
				{
					if (MapY+edWinLen < WORLD_Y)
					{
						MapY++;
						moveFlag=YES;
					}
				}

				if (moveFlag == YES)
				{	
					DisableMenus();	/* locks to current process */
					newMapFlags[CYMAP]=0;
					DrawBigMap();
#if 0
					DrawObjects();
#endif
					EnableMenus();
					goto moveagn;
				}

			} 		 /* End of "if (!ScrollLock" */
		}	/* End if if (!menuActive) */
		if (KBHit() || CheckMouseEvents())		/* if an event happened */
		{
			DisableMenus();
			while(KBHit())
			{	DoKey();
			}
			while (CheckMouseEvents())
			{	DoEvent();
			}
			lastEvent=TickCount();
		}
		if (simSpeed == 0)
		{
			if (WaitedEnough(&updateWait, 18*3))
			{		DisableMenus();
					UpdateEdWindow();
			}
		}
#if DEBUGINI
	OutStr("\nMAIN: re-enable");
#endif

		EnableMenus();	/* Make sure the menus are enabled */
		copyOK=YES;
#if DEBUGINI
	OutStr("\nMAIN: copyok, pass");
#endif
		Pass();
		copyOK=NO;
#if DEBUGINI
	OutStr("\nMAIN: copy not OK");
#endif
	}
}

SetGameYear()		/* get game year */
{
	Rect verBox;
	static char *label="Enter Game Year:";		/* label for window */

	char dateBuffer[10];		/* buffer for date */
	EVENT event;
	char *savePtr;
	int len, x, y, i,c, optLens[8];
	Rect tmpBox;
	Size strSize;
	const char **optPtr;
	int posIndex=(-1);
	long tmpTime;

	FlushKeys();

	CenterRect(&verBox,18,5);

	tmpBox=*AdjRect(&verBox);
	savePtr=GSaveRect(&tmpBox);

	GRectFill(&tmpBox,BUDGFC+PMGREY);
	GSetAttrib(BUDGBC,BUDGBC,PWHITE);
	GRectOutline(&tmpBox, 4);
				/* Outline outline in BLACK */
	GSetAttrib(BLACK,BLACK,PBLACK);
	GRectOutline(&tmpBox,1);
	GSetAttrib(BUDGBC,BUDGFC,PINV|PWHITE);
	PrintRectLines(&verBox,verBox.top,label);
	i=20;
	x=verBox.left+i;
	y=verBox.bottom-2;
	GSetAttrib(BUDGBC,BUDBOXC,PINV|PWHITE);

/*	GSetAttrib(BUDGBC,BUDBOXC,0); */
/*	TempFullScreenCursor(); */

	sprintf(dateBuffer,"%4d",(CityTime/48)+1900);		/* set date */
	GetStrSetNum();	/* Numbers only */
	GetStrn(verBox.left+6,verBox.top+3,dateBuffer,5,1);		/* allow user to edit the date and set eos */
	GetStrSetAll();	/* allow any alphnum input */
	if(strlen(dateBuffer) == 4) {		/* if they entered a good date */
		tmpTime=(atoi(dateBuffer)-1900)*48L;		/* set new date */
		if (tmpTime > 0)
			CityTime=tmpTime;
	} else {		/* if not the correct length */
		MakeSound(7);		/* this should be CANTSND for bad date */
	}
	for(i=0;i < 100;i++) {		/* kill any and all events */
		GetMouseEvent(&event);		/* get event */
	}
/*	RemoveTempCursor(); */
	GRestoreRect(&tmpBox,savePtr);
	DoDate();		/* update the date in the edit window */
}

int PSBound(a,n,b)		/* return number between a and b */
{
	if(n < a) n=a;
	if(n > b) n=b;
	return n;
}

/* here is the concept for SetTerrainParameters():
ZDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDD?
3                                    3
3    Terrain Creation Parameters     3
3                                    3
3    Number     Number     River     3
3   of Trees   of Lakes  Curviness   3
3                                    3
3    xxx%     xxx%     xxx%    3
3                                    3
3  ZDDDDDDDD?            ZDDDDDDDD?  3
3  3   Go   3            3 Cancel 3  3
3  @DDDDDDDDY            @DDDDDDDDY  3
@DDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDY
*/
SetTerrainParameters()		/* allow user to set up terrain grneration parameters */
{
	char *savePtr;		/* pointer to saved screen data */
	char temStr[20];		/* temporary string for percentages */
	EVENT event;		/* temporary event */
	int atButton=0;		/* current button we're at */
	int buttonDown=0;		/* flag telling if mouse button is down */
	int goFlag=0;		/* flag telling if terrain is to be generated */
	long startTime;		/* temporary timer value */
	int tem;		/* temporary counter */
	int x,y;		/* temporary positions */
	Rect wRect;		/* rectangle showing extents of window */
	Rect tmpRect;		/* temporary rectangle for adjusted window rectangle */
	Point buttonPos[8];		/* positions of buttons */

	FlushKeys();		/* do something...flush keyboard buffer? */

	CenterRect(&wRect,36,10);		/* center our window */

	tmpRect=*AdjRect(&wRect);
	savePtr=GSaveRect(&tmpRect);

	GRectFill(&tmpRect,BUDGFC+PMGREY);
	GSetAttrib(BUDGBC,BUDGBC,PWHITE);
	GRectOutline(&tmpRect,4);
				/* Outline outline in BLACK */
	GSetAttrib(BLACK,BLACK,PBLACK);
	GRectOutline(&tmpRect,1);
	GSetAttrib(BUDGBC,BUDGFC,PINV|PWHITE);
	PrintRectLines(&wRect,wRect.top,"Terrain Creation Parameters");
	x=(wRect.left+2)*8;
	y=(wRect.top+2)*fontHeight;
	GPPrintf(x+(1*8),y+(1*fontHeight)-3," Number     Number     River  ");
	GPPrintf(x+(1*8),y+(2*fontHeight)-3,"of Trees   of Lakes  Curviness");

	buttonPos[0].h=wRect.left+3;		/* positions of buttons */
	buttonPos[1].h=wRect.left+10;
	buttonPos[2].h=wRect.left+14;
	buttonPos[3].h=wRect.left+21;
	buttonPos[4].h=wRect.left+25;
	buttonPos[5].h=wRect.left+32;
	for(tem=0;tem < 6;tem++) {
		buttonPos[tem].v=wRect.top+5;		/* set vertical position */
	}
	buttonPos[6].h=wRect.left+3;
	buttonPos[7].h=wRect.left+25;
	buttonPos[6].v=buttonPos[7].v=wRect.top+8;

	GSetAttrib(BUDGBC,BUDBOXC,PINV|PWHITE);
	for(tem=0;tem < 3;tem++) {		/* for each set of value buttons */
		AddTextButton(buttonPos[tem*2],"",0x800+tem*2);		/* add button */
		AddTextButton(buttonPos[tem*2+1],"",0x801+tem*2);		/* add button */
	}
	AddTextButton(buttonPos[6],"   Go   ",0x806);		/* add 'go' button */
	AddTextButton(buttonPos[7]," Cancel ",0x807);		/* add 'cancel' button */

		/* output initial values */
	GSetAttrib(BUDGBC,BUDGFC,PINV|PWHITE);
	sprintf(temStr,"%3d%%%%",treeLevel);		/* get string for tree level */
	GPPrintf(x+(3*8),y+(3*fontHeight),temStr);		/* output string */
	sprintf(temStr,"%3d%%%%",lakeLevel);		/* get string for lake level */
	GPPrintf(x+(14*8),y+(3*fontHeight),temStr);		/* output string */
	sprintf(temStr,"%3d%%%%",curvLevel);		/* get string for curviness level */
	GPPrintf(x+(25*8),y+(3*fontHeight),temStr);		/* output string */

/*	GSetAttrib(BUDGBC,BUDBOXC,0); */
	TempFullScreenCursor();		/* give us a cursor */

	while(1) {		/* until break */
		if(0) {		/* never unless below code calls it */
repos:
			SetMouseAtButton(atButton+0x800);
		}
		while (KBHit())		/* Clear any keys pressed */
		{	
			switch (GetECH())
			{	case 27:		/* if escape */
				case 'C':		/* (or 'cancel') */
				case 'c':
					goFlag=0;		/* don't create random terrain */
					goto xit;
				case 13:		/* if carriage return */
				case 'G':		/* (or 'go') */
				case 'g':
					goFlag=1;		/* create random terrain */
					goto xit;
				case '+':	atButton=((atButton+1) % 8);		/* for each button */
							goto repos;
				case '-':
							atButton=(atButton)?(atButton-1):7;
							goto repos;
			}
		}
		if(CheckMouseEvents()) {		/* if there's an activity */
			GetMouseEvent(&event);		/* get event */
downAgain:
			switch(event.object) {		/* get event id */
				case 0x800:		/* if - trees */
					treeLevel=PSBound(0,treeLevel-1,100);
					goto updateValues;		/* update values */
				case 0x801:		/* if + trees */
					treeLevel=PSBound(0,treeLevel+1,100);
					goto updateValues;		/* update values */
				case 0x802:		/* if - lakes */
					lakeLevel=PSBound(0,lakeLevel-1,100);
					goto updateValues;		/* update values */
				case 0x803:		/* if + lakes */
					lakeLevel=PSBound(0,lakeLevel+1,100);
					goto updateValues;		/* update values */
				case 0x804:		/* if - curviness */
					curvLevel=PSBound(0,curvLevel-1,100);
					goto updateValues;		/* update values */
				case 0x805:		/* if + curviness */
					curvLevel=PSBound(0,curvLevel+1,100);
					goto updateValues;		/* update values */
				case 0x806:		/* if 'go' */
					goFlag=1;		/* return flag telling to create terrain */
					break;
				case 0x807:		/* if 'cancel' */
					goFlag=0;		/* return flag telling to create terrain */
					break;
			}
			if(0) {		/* only if above code calls us */
updateValues:
				RemoveTempCursor();		/* remove cursor from screen */
				GSetAttrib(BUDGBC,BUDGFC,PINV|PWHITE);
				sprintf(temStr,"%3d%%%%",treeLevel);		/* get string for tree level */
				GPPrintf(x+(3*8),y+(3*fontHeight),temStr);		/* output string */
				sprintf(temStr,"%3d%%%%",lakeLevel);		/* get string for lake level */
				GPPrintf(x+(14*8),y+(3*fontHeight),temStr);		/* output string */
				sprintf(temStr,"%3d%%%%",curvLevel);		/* get string for curviness level */
				GPPrintf(x+(25*8),y+(3*fontHeight),temStr);		/* output string */
				TempFullScreenCursor();		/* put cursor back */
				if(!buttonDown) {		/* if button was not down */
					buttonDown=1;		/* set button down flag */
					startTime=TickCount();		/* get current time */
					while(StillDown()) {		/* while a button is down */
						if(TickCount()-startTime > 5) {		/* if autorepeat is up */
							goto downAgain;
						}
					}
					buttonDown=0;		/* button is no longer down */
				} else {		/* if button is already down */
					if(StillDown()) {		/* if button is still down */
						startTime=TickCount();		/* get current time */
						while(startTime == TickCount()) ;		/* wait for 1/18th of a second */
						goto downAgain;
					}
					buttonDown=0;		/* no button is down */
				}
			}
			if(event.object == 0x806 || event.object == 0x807) {		/* if one of the exit buttons */
xit:
				break;		/* exit loop */
			}
		}
	}

	for(tem=0x800;tem <= 0x809;tem++) {		/* remove buttons from hot list */
		DelButtonHot(tem);
	}
	RemoveTempCursor();		/* remove our cursor */
	GRestoreRect(&tmpRect,savePtr);
	return goFlag;		/* return flag telling if terrain is to be created */
}
