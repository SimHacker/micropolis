/* sim.h:  Main include file
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

/* workaround sun c compiler junk */

#ifdef sun

#define _TTYCHARS_
#define CTRL(c) (#c[0]&037)

#ifndef SOLARIS2
#define __sys_ioccom_h
#define _IOCPARM_MASK   0xff            /* parameters must be < 256 bytes */
#define _IOC_VOID       0x20000000      /* no parameters */
#define _IOC_OUT        0x40000000      /* copy out parameters */
#define _IOC_IN         0x80000000      /* copy in parameters */
#define _IOC_INOUT      (_IOC_IN|_IOC_OUT)
#define _IO(x,y)        (_IOC_VOID|(#x[0]<<8)|y)
#define _IOR(x,y,t)     (_IOC_OUT|((sizeof(t)&_IOCPARM_MASK)<<16)|(#x[0]<<8)|y)
#define _IORN(x,y,t)    (_IOC_OUT|(((t)&_IOCPARM_MASK)<<16)|(#x[0]<<8)|y)
#define _IOW(x,y,t)     (_IOC_IN|((sizeof(t)&_IOCPARM_MASK)<<16)|(#x[0]<<8)|y)
#define _IOWN(x,y,t)    (_IOC_IN|(((t)&_IOCPARM_MASK)<<16)|(#x[0]<<8)|y)
#define _IOWR(x,y,t)    (_IOC_INOUT|((sizeof(t)&_IOCPARM_MASK)<<16)|(#x[0]<<8)|y)
#define _IOWRN(x,y,t)   (_IOC_INOUT|(((t)&_IOCPARM_MASK)<<16)|(#x[0]<<8)|y)
#endif

#endif


#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>
#include <setjmp.h>
#include <malloc.h>
#include <errno.h>
#include <signal.h>
#include <math.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <sys/ioctl.h>
#include <sys/resource.h>
#ifndef MSDOS
#ifndef HPUX
#include <alloca.h>
#endif
#include <sys/mman.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

#ifdef sun
#ifdef SOLARIS2
#include <sys/systeminfo.h>
#else
#include <sys/vadvise.h>
#endif
#endif

#ifdef sgi
#if 0
#include <audio.h>
#endif
#endif

#include <sys/ipc.h>
#include <sys/shm.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/extensions/XShm.h>

//#include <xpmtk.h>
#include <X11/xpm.h>

/* gcc compat workaround! */
#define _STDDEF_H

#include "tclxtend.h"
#include "tclint.h"
#include "tclunix.h"
#include "tkconfig.h"
#include "tkint.h"

#include "mac.h"
#include "view.h"
#include "macros.h"

/* Constants */

#define TRUE		 1
#define FALSE		 0

#ifdef MEGA
#define SimWidth	(120 * MEGA)
#define SimHeight	(100 * MEGA)
#else
#define SimWidth	120
#define SimHeight	100
#endif

#define WORLD_X		SimWidth
#define WORLD_Y		SimHeight
#define HWLDX		(SimWidth >>1)
#define HWLDY		(SimHeight >>1)
#define QWX		(SimWidth >>2)
#define QWY		(SimHeight >>2)
#define SmX		(SimWidth >>3)
#define SmY		((SimHeight + 7) >>3)

#define EDITOR_W (WORLD_X * 16)
#define EDITOR_H (WORLD_Y * 16)
#define MAP_W (WORLD_X * 3)
#define MAP_H (WORLD_Y * 3)

#define NIL		0
#define HORIZ		1
#define VERT		0

#define PROBNUM 10

#define HISTLEN		480
#define MISCHISTLEN	240

#define POWERMAPROW		((WORLD_X + 15) / 16)

#ifdef MEGA
#define POWERMAPLEN		((QUAD)(2 * POWERMAPROW * WORLD_Y))
#define POWERWORD(x, y)		((((QUAD)(x)) >>4) + (((QUAD)(y)) * POWERMAPROW))
#else
#define POWERMAPLEN		1700 /* ??? PWRMAPSIZE */
#define POWERWORD(x, y)		(((x) >>4) + ((y) <<3))
#endif

#define SETPOWERBIT(x, y)	PowerMap[POWERWORD((x), (y))] |= 1 << ((x) & 15)
#define PWRMAPSIZE		(POWERMAPROW * WORLD_Y)
#define PWRSTKSIZE		((WORLD_X * WORLD_Y) / 4)

#define ALMAP 0 /* all */
#define REMAP 1 /* residential */
#define COMAP 2 /* commercial */
#define INMAP 3 /* industrial */

#define PRMAP 4 /* power */
#define RDMAP 5 /* road */

#define PDMAP 6 /* population density */
#define RGMAP 7 /* rate of growth */

#define TDMAP 8 /* traffic density */
#define PLMAP 9 /* pollution */
#define CRMAP 10 /* crime */
#define LVMAP 11 /* land value */

#define FIMAP 12 /* fire radius */
#define POMAP 13 /* police radius */
#define DYMAP 14 /* dynamic */

#define NMAPS 15

/* These adjust frequency in Simulate() */

#define VALVERATE 2
#define CENSUSRATE 4
#define TAXFREQ 48

/* These are names of the 16 colors */
#define COLOR_WHITE		0
#define COLOR_YELLOW		1
#define COLOR_ORANGE		2
#define COLOR_RED		3
#define COLOR_DARKRED		4
#define COLOR_DARKBLUE		5
#define COLOR_LIGHTBLUE		6
#define COLOR_BROWN		7
#define COLOR_LIGHTGREEN	8
#define COLOR_DARKGREEN		9
#define COLOR_OLIVE		10
#define COLOR_LIGHTBROWN	11
#define COLOR_LIGHTGRAY		12
#define COLOR_MEDIUMGRAY	13
#define COLOR_DARKGRAY		14
#define COLOR_BLACK		15

/* Status Bits */

#define PWRBIT		32768	/*20	bit 15	*/
#define CONDBIT		16384	/*10	bit 14	*/
#define BURNBIT		8192	/*8	bit 13	*/
#define BULLBIT		4096	/*4	bit 12	*/
#define ANIMBIT		2048	/*2	bit 11	*/
#define ZONEBIT		1024	/*1	bit 10	*/
#define ALLBITS		64512	/*  mask for upper 6 bits	*/
#define LOMASK		1023	/*	mask for low 10 bits	*/

#define BLBNBIT		(BULLBIT+BURNBIT)
#define BLBNCNBIT	(BULLBIT+BURNBIT+CONDBIT)
#define BNCNBIT		(BURNBIT+CONDBIT)

/* Object & Sound Numbers */

#define TRA		1
#define COP		2
#define AIR		3
#define SHI		4
#define GOD		5
#define TOR		6
#define EXP		7
#define BUS		8

/* Max # of Objects */

#define OBJN		9

/* Graph Histories */
#define RES_HIST	0
#define COM_HIST	1
#define IND_HIST	2
#define MONEY_HIST	3
#define CRIME_HIST	4
#define POLLUTION_HIST	5
#define HISTORIES	6
#define ALL_HISTORIES	((1 <<HISTORIES) - 1)

/* Character Mapping */

#define DIRT		0
#define RIVER		2
#define REDGE		3
#define CHANNEL		4
#define FIRSTRIVEDGE	5
#define LASTRIVEDGE	20
#define TREEBASE	21
#define LASTTREE	36
#define WOODS		37
#define UNUSED_TRASH1	38
#define UNUSED_TRASH2	39
#define WOODS2		40
#define WOODS3		41
#define WOODS4		42
#define WOODS5		43
#define RUBBLE		44
#define LASTRUBBLE	47
#define FLOOD		48
#define LASTFLOOD	51
#define RADTILE		52
#define UNUSED_TRASH3	53
#define UNUSED_TRASH4	54
#define UNUSED_TRASH5	55
#define FIRE		56
#define FIREBASE	56
#define LASTFIRE	63
#define ROADBASE	64
#define HBRIDGE		64
#define VBRIDGE		65
#define ROADS		66
#define INTERSECTION	76
#define HROADPOWER	77
#define VROADPOWER	78
#define BRWH		79
#define LTRFBASE	80
#define BRWV		95
#define BRWXXX1		111
#define BRWXXX2		127
#define BRWXXX3		143
#define HTRFBASE	144
#define BRWXXX4		159
#define BRWXXX5		175
#define BRWXXX6		191
#define LASTROAD	206
#define BRWXXX7		207
#define POWERBASE	208
#define HPOWER		208
#define VPOWER		209
#define LHPOWER		210
#define LVPOWER		211
#define RAILHPOWERV	221
#define RAILVPOWERH	222
#define LASTPOWER	222
#define UNUSED_TRASH6	223
#define RAILBASE	224
#define HRAIL		224
#define VRAIL		225
#define LHRAIL		226
#define LVRAIL		227
#define HRAILROAD	237
#define VRAILROAD	238
#define LASTRAIL	238
#define ROADVPOWERH	239 /* bogus? */
#define RESBASE		240
#define FREEZ		244
#define HOUSE		249
#define LHTHR		249
#define HHTHR		260
#define RZB		265
#define HOSPITAL	409
#define CHURCH		418
#define COMBASE		423
#define COMCLR		427
#define CZB		436
#define INDBASE		612
#define INDCLR		616
#define LASTIND		620
#define IND1		621
#define IZB		625
#define IND2		641
#define IND3		644
#define IND4		649
#define IND5		650
#define IND6		676
#define IND7		677
#define IND8		686
#define IND9		689
#define PORTBASE	693
#define PORT		698
#define LASTPORT	708
#define AIRPORTBASE	709
#define RADAR		711
#define AIRPORT		716
#define COALBASE	745
#define POWERPLANT	750
#define LASTPOWERPLANT	760
#define FIRESTBASE	761
#define FIRESTATION	765
#define POLICESTBASE	770
#define POLICESTATION	774
#define STADIUMBASE	779
#define STADIUM		784
#define FULLSTADIUM	800
#define NUCLEARBASE	811
#define NUCLEAR		816
#define LASTZONE	826
#define LIGHTNINGBOLT	827
#define HBRDG0		828
#define HBRDG1		829
#define HBRDG2		830
#define HBRDG3		831
#define RADAR0		832
#define RADAR1		833
#define RADAR2		834
#define RADAR3		835
#define RADAR4		836
#define RADAR5		837
#define RADAR6		838
#define RADAR7		839
#define FOUNTAIN	840
#define INDBASE2	844
#define TELEBASE	844
#define TELELAST	851
#define SMOKEBASE	852
#define TINYEXP		860
#define SOMETINYEXP	864
#define LASTTINYEXP	867
#define COALSMOKE1	916
#define COALSMOKE2	920
#define COALSMOKE3	924
#define COALSMOKE4	928
#define FOOTBALLGAME1	932
#define FOOTBALLGAME2	940
#define VBRDG0		948
#define VBRDG1		949
#define VBRDG2		950
#define VBRDG3		951

#define TILE_COUNT	960

/* 
 * These describe the wand values, the object dragged around on the screen.
 */

#define residentialState 0
#define commercialState 1
#define industrialState 2
#define fireState 3
#define queryState 4
#define policeState 5
#define wireState 6
#define dozeState 7
#define rrState 8
#define roadState 9
#define chalkState 10
#define eraserState 11
#define stadiumState 12
#define parkState 13
#define seaportState  14
#define powerState 15
#define nuclearState 16
#define airportState 17
#define networkState 18


#define firstState residentialState
#define lastState networkState

#if 0
#define specialState 16
#define monsterGoalState 17
#define helicopterGoalState 18
#define blackState 19
#define whiteState 20
#define redState 21
#define orangeState 23
#define blueState 24
#define greenState 25
#define brownState 26
#endif


#define STATE_CMD 0
#define STATE_TILES 1
#define STATE_OVERLAYS 2
#define STATE_GRAPHS 3


extern short *Map[WORLD_X];		/* Main Map 120 x 100  */
extern Byte *PopDensity[HWLDX];		/* 2X2 Maps  60 x 50 */
extern Byte *TrfDensity[HWLDX];
extern Byte *PollutionMem[HWLDX];
extern Byte *LandValueMem[HWLDX];
extern Byte *CrimeMem[HWLDX];
extern Byte *tem[HWLDX];
extern Byte *tem2[HWLDX];
extern Byte *TerrainMem[QWX];		/* 4X4 Maps  30 x 25 */
extern Byte *Qtem[QWX];
extern short RateOGMem[SmX][SmY];
extern short FireStMap[SmX][SmY];	/* 8X8 Maps  15 x 13 */
extern short PoliceMap[SmX][SmY];
extern short PoliceMapEffect[SmX][SmY];
extern short ComRate[SmX][SmY];
extern short FireRate[SmX][SmY];
extern short STem[SmX][SmY];
extern short SpriteXOffset[OBJN];
extern short SpriteYOffset[OBJN];
extern short SMapX, SMapY;
extern short CChr, CChr9;
extern short RoadTotal, RailTotal, FirePop;
extern short ResPop, ComPop, IndPop, TotalPop, LastTotalPop;
extern short ResZPop, ComZPop, IndZPop, TotalZPop;
extern short HospPop, ChurchPop, StadiumPop;
extern short PolicePop, FireStPop;
extern short CoalPop, NuclearPop, PortPop, APortPop;
extern short NeedHosp, NeedChurch;
extern short CrimeAverage, PolluteAverage, LVAverage;
extern char *MicropolisVersion;
extern char *CityName;
extern char *CityFileName;
extern char *StartupName;
extern short StartingYear;
extern QUAD CityTime;
extern QUAD LastCityTime;
extern QUAD LastCityMonth;
extern QUAD LastCityYear;
extern QUAD LastFunds;
extern QUAD LastR, LastC, LastI;
extern short GameLevel;
extern short Cycle;
extern short ScenarioID;
extern short ShakeNow;
extern QUAD DonDither;
extern int DoOverlay;

extern short *ResHis, ResHisMax;
extern short *ComHis, ComHisMax;
extern short *IndHis, IndHisMax;
extern short *MoneyHis, *CrimeHis, *PollutionHis, *MiscHis;
extern short *PowerMap;

extern float roadPercent, policePercent, firePercent;
extern QUAD RoadSpend, PoliceSpend, FireSpend;
extern QUAD roadMaxValue, policeMaxValue, fireMaxValue;
extern QUAD TaxFund, RoadFund, PoliceFund, FireFund;
extern short RoadEffect, PoliceEffect, FireEffect;
extern short TaxFlag, CityTax;
extern QUAD TotalFunds;

extern QUAD costOf[];

extern short flagBlink;
extern unsigned char tileSynch;
extern short aniTile[];
extern unsigned char aniSynch[];
extern int TilesAnimated;
extern int DoAnimation;
extern int DoMessages;
extern int DoNotices;
extern unsigned char ColorIntensities[];

extern short MesX, MesY;
extern short MesNum, MessagePort;
extern QUAD LastMesTime;

extern short SimSpeed;
extern short SimMetaSpeed;
extern short NoDisasters;
extern short autoBulldoze;
extern short autoBudget;
extern short autoGo;
extern short UserSoundOn;

extern short DisasterEvent;
extern short DisasterWait;

extern short ResCap, ComCap, IndCap;
extern short RValve, CValve, IValve;
extern short PwrdZCnt;
extern short unPwrdZCnt;

extern char *HomeDir, *ResourceDir, *HostName;

extern short Graph10Max, Graph120Max;
extern short Res2HisMax, Com2HisMax, Ind2HisMax;
extern unsigned char *History10[HISTORIES], *History120[HISTORIES];
extern short CityScore;
extern short deltaCityScore;
extern short ScoreType;
extern short ScoreWait;
extern short CityClass;
extern short PolMaxX, PolMaxY;
extern int PowerStackNum;
extern short TrafficAverage;
extern short PosStackN;
extern short SMapXStack[], SMapYStack[];
extern short Zsource;
extern short HaveLastMessage;
extern short PdestX, PdestY;
extern short CdestX, CdestY;
extern int absDist;
extern short CopFltCnt;
extern short GodCnt;
extern short GdestX, GdestY;
extern short GorgX, GorgY;
extern short GodControl;
extern short CopControl;
extern short CdestX, CdestY;
extern short TrafMaxX, TrafMaxY;
extern short CrimeMaxX, CrimeMaxY;
extern short FloodX, FloodY;
extern short CrashX, CrashY;
extern short CCx, CCy;
extern QUAD CityPop, deltaCityPop;
extern char *cityClassStr[6];
extern short CityYes, CityNo;
extern short ProblemTable[PROBNUM];
extern short ProblemVotes[PROBNUM];
extern short ProblemOrder[4];
extern QUAD CityAssValue;

extern short InitSimLoad;
extern short DoInitialEval;
extern int Startup;
extern int StartupGameLevel;
extern int PerformanceTiming;
extern double FlushTime;

extern char *optarg;
extern int optind;
extern struct timeval start_time, now_time, beat_time, last_now_time;
extern Sim *sim;
extern int WireMode;
extern int MultiPlayerMode;
extern int SugarMode;
extern int sim_delay;
extern int sim_skips;
extern int sim_skip;
extern int sim_paused;
extern int sim_paused_speed;
extern int sim_tty;
#ifdef CAM
extern int sim_just_cam;
#endif
extern int heat_steps;
extern int heat_flow;
extern int heat_rule;
extern int UpdateDelayed;
extern int DynamicData[32];
extern int Players;
extern int Votes;
extern int BobHeight;
extern int OverRide;
extern int Expensive;
extern int PendingTool;
extern int PendingX;
extern int PendingY;
extern int TreeLevel;
extern int LakeLevel;
extern int CurveLevel;
extern int CreateIsland;
extern short specialBase;
extern short PunishCnt;
extern short Dozing;
extern short toolSize[];
extern short toolOffset[];
extern QUAD toolColors[];
extern char *Displays;
extern char *FirstDisplay;
extern char *dateStr[12];

extern short NewMap;
extern short NewMapFlags[NMAPS];
extern short NewGraph;
extern short ValveFlag;
extern short MustUpdateFunds;
extern short MustUpdateOptions;
extern short CensusChanged;
extern short EvalChanged;
extern short MeltX, MeltY;
extern int NeedRest;
extern int ExitReturn;

extern Tcl_Interp *tk_mainInterp;
extern Tk_Window MainWindow;
extern int FlushStyle;
extern int GotXError;

extern short Rand(short range);
extern short RandInt(void);

extern Sim *MakeNewSim();
extern SimView *MakeNewView();
extern SimSprite *GetSprite();
extern SimSprite *MakeSprite();
extern SimSprite *MakeNewSprite();

extern int setSpeed(short speed);
extern int setSkips(int skips);
extern int SetGameLevel(short level);
extern int SetGameLevelFunds(short level);

extern struct XDisplay *XDisplays;
