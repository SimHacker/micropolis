/* micropolis.cpp
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


Micropolis::Micropolis()
{
  init();
}


Micropolis::~Micropolis()
{
  destroy();
}


void Micropolis::init()
{


  ////////////////////////////////////////////////////////////////////////
  // allocate.cpp


  // short SMapX;
  SMapX = 0;

  // short SMapY;
  SMapY = 0;

  // short CChr;
  CChr = 0;

  // short CChr9;
  CChr9 = 0;

  // short RoadTotal;
  RoadTotal = 0;

  // short RailTotal;
  RailTotal = 0;

  // short FirePop;
  FirePop = 0;

  // short ResPop;
  ResPop = 0;

  // short ComPop;
  ComPop = 0;

  // short IndPop;
  IndPop = 0;

  // short TotalPop;
  TotalPop = 0;

  // short LastTotalPop;
  LastTotalPop = 0;

  // short ResZPop;
  ResZPop = 0;

  // short ComZPop;
  ComZPop = 0;

  // short IndZPop;
  IndZPop = 0;

  // short TotalZPop;
  TotalZPop = 0;

  // short HospPop;
  HospPop = 0;

  // short ChurchPop;
  ChurchPop = 0;

  // short StadiumPop;
  StadiumPop = 0;

  // short PolicePop;
  PolicePop = 0;

  // short FireStPop;
  FireStPop = 0;

  // short CoalPop;
  CoalPop = 0;

  // short NuclearPop;
  NuclearPop = 0;

  // short PortPop;
  PortPop = 0;

  // short APortPop;
  APortPop = 0;

  // short NeedHosp;
  NeedHosp = 0;

  // short NeedChurch;
  NeedChurch = 0;

  // short CrimeAverage;
  CrimeAverage = 0;

  // short PolluteAverage;
  PolluteAverage = 0;

  // short LVAverage;
  LVAverage = 0;

  // Quad CityTime;
  CityTime = 0;

  // short StartingYear;
  StartingYear = 0;

  // short *Map[WORLD_X];
  memset(Map, 0, sizeof(short *) * WORLD_X);

  // short ResHisMax;
  ResHisMax = 0;

  // short Res2HisMax;
  Res2HisMax = 0;

  // short ComHisMax;
  ComHisMax = 0;

  // short Com2HisMax;
  Com2HisMax = 0;

  // short IndHisMax;
  IndHisMax = 0;

  // short Ind2HisMax;
  Ind2HisMax = 0;

  // short CensusChanged;
  CensusChanged = 0;

  // short MessagePort;
  MessagePort = 0;

  // short MesX;
  MesX = 0;

  // short MesY;
  MesY = 0;

  // Quad RoadSpend;
  RoadSpend = 0;

  // short PoliceSpend;
  PoliceSpend = 0;

  // short FireSpend;
  FireSpend = 0;

  // Quad RoadFund;
  RoadFund = 0;

  // short PoliceFund;
  PoliceFund = 0;

  // short FireFund;
  FireFund = 0;

  // short RoadEffect;
  RoadEffect = 0;

  // short PoliceEffect;
  PoliceEffect = 0;

  // short FireEffect;
  FireEffect = 0;

  // Quad TaxFund; 
  TaxFund = 0; 

  // short CityTax;
  CityTax = 0;

  // short TaxFlag;
  TaxFlag = 0;

  // Byte *PopDensity[HWLDX];
  memset(PopDensity, 0, sizeof(Byte *) * HWLDX);

  // Byte *TrfDensity[HWLDX];
  memset(TrfDensity, 0, sizeof(Byte *) * HWLDX);

  // Byte *PollutionMem[HWLDX];
  memset(PollutionMem, 0, sizeof(Byte *) * HWLDX);

  // Byte *LandValueMem[HWLDX];
  memset(LandValueMem, 0, sizeof(Byte *) * HWLDX);

  // Byte *CrimeMem[HWLDX];
  memset(CrimeMem, 0, sizeof(Byte *) * HWLDX);

  // Byte *tem[HWLDX];
  memset(tem, 0, sizeof(Byte *) * HWLDX);

  // Byte *tem2[HWLDX];
  memset(tem2, 0, sizeof(Byte *) * HWLDX);

  // Byte *TerrainMem[QWX];
  memset(TerrainMem, 0, sizeof(Byte *) * QWX);

  // Byte *Qtem[QWX];
  memset(Qtem, 0, sizeof(Byte *) * QWX);

  // short RateOGMem[SmX][SmY];
  memset(RateOGMem, 0, sizeof(short) * SmX * SmY);

  // short FireStMap[SmX][SmY];
  memset(FireStMap, 0, sizeof(short) * SmX * SmY);

  // short PoliceMap[SmX][SmY];
  memset(PoliceMap, 0, sizeof(short) * SmX * SmY);

  // short PoliceMapEffect[SmX][SmY];
  memset(PoliceMapEffect, 0, sizeof(short) * SmX * SmY);

  // short FireRate[SmX][SmY];
  memset(FireRate, 0, sizeof(short) * SmX * SmY);

  // short ComRate[SmX][SmY];
  memset(ComRate, 0, sizeof(short) * SmX * SmY);

  // short STem[SmX][SmY];
  memset(STem, 0, sizeof(short) * SmX * SmY);

  // Ptr terrainBase;
  terrainBase = NULL;

  // Ptr qTemBase;
  qTemBase = NULL;

  // Ptr tem1Base;
  tem1Base = NULL;

  // Ptr tem2Base;
  tem2Base = NULL;

  // Ptr popPtr;
  popPtr = NULL;

  // Ptr trfPtr;
  trfPtr = NULL;

  // Ptr polPtr;
  polPtr = NULL;

  // Ptr landPtr;
  landPtr = NULL;

  // Ptr crimePtr;
  crimePtr = NULL;

  // Ptr auxPopPtr;
  auxPopPtr = NULL;

  // Ptr auxTrfPtr;
  auxTrfPtr = NULL;

  // Ptr auxPolPtr;
  auxPolPtr = NULL;

  // Ptr auxLandPtr;
  auxLandPtr = NULL;

  // Ptr auxCrimePtr;
  auxCrimePtr = NULL;

  // Ptr brettPtr;
  brettPtr = NULL;

  // unsigned short *mapPtr;
  mapPtr = NULL;

  // short *ResHis;
  ResHis = NULL;

  // short *ComHis;
  ComHis = NULL;

  // short *IndHis;
  IndHis = NULL;

  // short *MoneyHis;
  MoneyHis = NULL;

  // short *PollutionHis;
  PollutionHis = NULL;

  // short *CrimeHis;
  CrimeHis = NULL;

  // short *MiscHis;
  MiscHis = NULL;

  // short *PowerMap;
  PowerMap = NULL;


  ////////////////////////////////////////////////////////////////////////
  // animate.cpp


  ////////////////////////////////////////////////////////////////////////
  // budget.cpp


  // float roadPercent;
  roadPercent = (float)0.0;

  // float policePercent;
  policePercent = (float)0.0;

  // float firePercent;
  firePercent = (float)0.0;

  // Quad roadValue;
  roadValue = 0;

  // Quad policeValue;
  policeValue = 0;

  // Quad fireValue;
  fireValue = 0;

  // Quad roadMaxValue;
  roadMaxValue = 0;

  // Quad policeMaxValue;
  policeMaxValue = 0;

  // Quad fireMaxValue;
  fireMaxValue = 0;

  // int MustDrawCurrPercents;
  MustDrawCurrPercents = 0;

  // int MustDrawBudgetWindow;
  MustDrawBudgetWindow = 0;


  ////////////////////////////////////////////////////////////////////////
  // connect.cpp


  ////////////////////////////////////////////////////////////////////////
  // disasters.cpp


  // short ShakeNow;
  ShakeNow = 0;

  // short FloodCnt;
  FloodCnt = 0;

  // short FloodX;
  FloodX = 0;

  // short FloodY;
  FloodY = 0;


  ////////////////////////////////////////////////////////////////////////
  // evaluate.cpp


  // short EvalValid;
  EvalValid = 0;

  // short CityYes;
  CityYes = 0;

  // short CityNo;
  CityNo = 0;

  // short ProblemTable[PROBNUM];
  memset(ProblemTable, 0, sizeof(short) * PROBNUM);

  // short ProblemTaken[PROBNUM];
  memset(ProblemTaken, 0, sizeof(short) * PROBNUM);

  // short ProblemVotes[PROBNUM]; /* these are the votes for each  */
  memset(ProblemVotes, 0, sizeof(short) * PROBNUM);

  // short ProblemOrder[4]; /* sorted index to above  */
  memset(ProblemOrder, 0, sizeof(short) * 4);

  // Quad CityPop;
  CityPop = 0;

  // Quad deltaCityPop;
  deltaCityPop = 0;

  // Quad CityAssValue;
  CityAssValue = 0;

  // short CityClass; /*  0..5  */
  CityClass = 0;

  // short CityScore;
  CityScore = 0;

  // short deltaCityScore;
  deltaCityScore = 0;

  // short AverageCityScore;
  AverageCityScore = 0;

  // short TrafficAverage;
  TrafficAverage = 0;


  ////////////////////////////////////////////////////////////////////////
  // fileio.cpp


  ////////////////////////////////////////////////////////////////////////
  // generate.cpp


  // short XStart;
  XStart = 0;

  // short YStart;
  YStart = 0;

  // short MapX;
  MapX = 0;

  // short MapY;
  MapY = 0;

  // short Dir;
  Dir = 0;

  // short LastDir;
  LastDir = 0;

  // int TreeLevel; /* level for tree creation */
  TreeLevel = -1;

  // int LakeLevel; /* level for lake creation */
  LakeLevel = -1;

  // int CurveLevel; /* level for river curviness */
  CurveLevel = -1;

  // int CreateIsland; /* -1 => 10%, 0 => never, 1 => always */
  CreateIsland = -1;


  ////////////////////////////////////////////////////////////////////////
  // graph.cpp


  // short NewGraph;
  NewGraph = 0;

  // short AllMax;
  AllMax = 0;

  // unsigned char *History10[HISTORIES];
  memset(History10, 0, sizeof(unsigned char *) * HISTORIES);

  // unsigned char *History120[HISTORIES];
  memset(History120, 0, sizeof(unsigned char *) * HISTORIES);

  // int HistoryInitialized;
  HistoryInitialized = 0;

  // short Graph10Max;
  Graph10Max = 0;

  // short Graph120Max;
  Graph120Max = 0;

  // int GraphUpdateTime;
  GraphUpdateTime = 100;


  ////////////////////////////////////////////////////////////////////////
  // initialize.cpp


  ////////////////////////////////////////////////////////////////////////
  // main.cpp

  // char *MicropolisVersion;
  MicropolisVersion = MICROPOLIS_VERSION;

  // int sim_loops;
  sim_loops = 0;

  // int sim_delay;
  sim_delay = 50;

  // int sim_skips;
  sim_skips = 0;

  // int sim_skip;
  sim_skip = 0;

  // int sim_paused;
  sim_paused = 0;

  // int sim_paused_speed;
  sim_paused_speed = 3;

  // int heat_steps;
  heat_steps = 0;

  // int heat_flow;
  heat_flow = -7;

  // int heat_rule;
  heat_rule = 0;

  // int heat_wrap;
  heat_wrap = 3;

  // char *CityFileName;
  CityFileName = NULL;

  // int MultiPlayerMode;
  MultiPlayerMode = 0;

  // int TilesAnimated;
  TilesAnimated = 0;

  // int DoAnimation;
  DoAnimation = 1;

  // int DoMessages;
  DoMessages = 1;

  // int DoNotices;
  DoNotices = 1;

  // short *CellSrc;
  CellSrc = NULL;

  // short *CellDst;
  CellDst = NULL;


  ////////////////////////////////////////////////////////////////////////
  // map.cpp


#if 0
  ////////////////////////////////////////////////////////////////////////
  // Disabled this small map drawing, filtering and overlaying code. 
  // Going to re-implement it in the tile engine and Python.


  // int DynamicData[32];
  memset(DynamicData, 0, sizeof(int) * 32);


#endif


  ////////////////////////////////////////////////////////////////////////
  // message.cpp


  // Quad LastCityPop;
  LastCityPop = 0;

  // short LastCategory;
  LastCategory = 0;

  // short LastPicNum;
  LastPicNum = 0
;
  // short autoGo;
  autoGo = 0;

  // short HaveLastMessage;
  HaveLastMessage = 0;

  // char LastMessage[256];
  memset(LastMessage, 0, sizeof(char) * 256);


  ////////////////////////////////////////////////////////////////////////
  // power.cpp


  // int PowerStackNum;
  PowerStackNum = 0;

  // short PowerStackX[PWRSTKSIZE];
  memset(PowerStackX, 0, sizeof(short) * PWRSTKSIZE);

  // short PowerStackY[PWRSTKSIZE];
  memset(PowerStackY, 0, sizeof(short) * PWRSTKSIZE);

  // Quad MaxPower;
  MaxPower = 0;

  // Quad NumPower;
  NumPower = 0;


  ////////////////////////////////////////////////////////////////////////
  // random.cpp


  // UQuad nextRandom;
  nextRandom = 1;


  ////////////////////////////////////////////////////////////////////////
  // resource.cpp


  // char *HomeDir;
  HomeDir = NULL;

  // char *ResourceDir;
  ResourceDir = NULL;

  // char *KeyDir;
  KeyDir = NULL;

  // char *HostName;
  HostName = NULL;

  // Resource *Resources;
  Resources = NULL;

  // StringTable *StringTables;
  StringTables = NULL;


  ////////////////////////////////////////////////////////////////////////
  // scan.cpp

  // short NewMap;
  NewMap = 0;

  // short NewMapFlags[NMAPS];
  memset(NewMapFlags, 0, sizeof(short) * NMAPS);

  // short CCx;
  CCx = 0;

  // short CCy;
  CCy = 0;

  // short CCx2;
  CCx2 = 0;

  // short CCy2;
  CCy2 = 0;

  // short PolMaxX;
  PolMaxX = 0;

  // short PolMaxY;
  PolMaxY = 0;

  // short CrimeMaxX;
  CrimeMaxX = 0;

  // short CrimeMaxY;
  CrimeMaxY = 0;

  // Quad DonDither;
  DonDither = 0;


  ////////////////////////////////////////////////////////////////////////
  // simulate.cpp


  // short ValveFlag;
  ValveFlag = 0;

  // short CrimeRamp;
  CrimeRamp = 0;

  // short PolluteRamp;
  PolluteRamp = 0;

  // short RValve;
  RValve = 0;

  // short CValve;
  CValve = 0;

  // short IValve;
  IValve = 0;

  // short ResCap;
  ResCap = 0;

  // short ComCap;
  ComCap = 0;

  // short IndCap;
  IndCap = 0;

  // short CashFlow;
  CashFlow = 0;

  // float EMarket;
  EMarket = (float)4.0;

  // short DisasterEvent;
  DisasterEvent = 0;

  // short DisasterWait;
  DisasterWait = 0;

  // short ScoreType;
  ScoreType = 0;

  // short ScoreWait;
  ScoreWait = 0;

  // short PwrdZCnt;
  PwrdZCnt = 0;

  // short unPwrdZCnt;
  unPwrdZCnt = 0;

  // short NewPower; /* post */
  NewPower = 0;

  // short AvCityTax;
  AvCityTax = 0;

  // short Scycle;
  Scycle = 0;

  // short Fcycle;
  Fcycle = 0;

  // short Spdcycle;
  Spdcycle = 0;

  // short DoInitialEval;
  DoInitialEval = 0;

  // short MeltX;
  MeltX = 0;

  // short MeltY;
  MeltY = 0;


  ////////////////////////////////////////////////////////////////////////
  // sprite.cpp


  //SimSprite *spriteList;
  spriteList = NULL;

  //int spriteCount;
  spriteCount = 0;

  // SimSprite *FreeSprites;
  FreeSprites = NULL;

  // SimSprite *GlobalSprites[OBJN];
  memset(GlobalSprites, 0, sizeof(SimSprite *) * OBJN);

  // short CrashX;
  CrashX = 0;

  // short CrashY;
  CrashY = 0;

  // int absDist;
  absDist = 0;

  // short Cycle;
  Cycle = 0;


  ////////////////////////////////////////////////////////////////////////
  // stubs.cpp


  // Quad TotalFunds;
  TotalFunds = 0;

  // short PunishCnt;
  PunishCnt = 0;

  // short autoBulldoze;
  autoBulldoze = 0;

  // short autoBudget;
  autoBudget = 0;

  // Quad LastMesTime;
  LastMesTime = 0;

  // short GameLevel;
  GameLevel = 0;

  // short InitSimLoad;
  InitSimLoad = 0;

  // short ScenarioID;
  ScenarioID = 0;

  // short SimSpeed;
  SimSpeed = 0;

  // short SimMetaSpeed;
  SimMetaSpeed = 0;

  // short UserSoundOn;
  UserSoundOn = 0;

  // char *CityName;
  CityName = NULL;

  // short NoDisasters;
  NoDisasters = 0;

  // short MesNum;
  MesNum = 0;

  // short EvalChanged;
  EvalChanged = 0;

  // short flagBlink;
  flagBlink = 0;

  // CallbackFunction callbackHook;
  callbackHook = NULL;

  // void *callbackData;
  callbackData = NULL;

  // void *userData;
  userData = NULL;


  ////////////////////////////////////////////////////////////////////////
  //  tool.cpp


  // int OverRide;
  OverRide = 0;

  // int Expensive;
  Expensive = 1000;

  // int Players;
  Players = 1;

  // int Votes;
  Votes = 0;

  // int PendingTool;
  PendingTool = -1;

  // int PendingX;
  PendingX = 0;

  // int PendingY;
  PendingY = 0;

  // Ink *OldInk;
  OldInk = NULL;

  // Ink *overlay;
  overlay = NULL;

  // Ink *track_ink;
  track_ink = NULL;

  // int last_x;
  last_x = 0;

  // int last_y;
  last_y = 0;

  // int tool_x;
  tool_x = 0;

  // int tool_y;
  tool_y = 0;


  ////////////////////////////////////////////////////////////////////////
  // traffic.cpp


  // short PosStackN;
  PosStackN = 0;

  // short SMapXStack[MAX_TRAFFIC_DISTANCE+1];
  memset(SMapXStack, 0, sizeof(short) * (MAX_TRAFFIC_DISTANCE+1));

  // short SMapYStack[MAX_TRAFFIC_DISTANCE+1];
  memset(SMapYStack, 0, sizeof(short) * (MAX_TRAFFIC_DISTANCE+1));

  // short LDir;
  LDir = 0;

  // short Zsource;
  Zsource = 0;

  // short TrafMaxX;
  TrafMaxX = 0;

  // short TrafMaxY;
  TrafMaxY = 0;


  ////////////////////////////////////////////////////////////////////////
  // update.cpp


  // short MustUpdateFunds;
  MustUpdateFunds = 0;

  // short MustUpdateOptions;
  MustUpdateOptions = 0;

  // Quad LastCityTime;
  LastCityTime = 0;

  // Quad LastCityYear;
  LastCityYear = 0;

  // Quad LastCityMonth;
  LastCityMonth = 0;

  // Quad LastFunds;
  LastFunds = 0;

  // Quad LastR;
  LastR = 0;

  // Quad LastC;
  LastC = 0;

  // Quad LastI;
  LastI = 0;


  ////////////////////////////////////////////////////////////////////////
  // utilities.cpp


  ////////////////////////////////////////////////////////////////////////
  // zone.cpp


  ////////////////////////////////////////////////////////////////////////

  sim_init();

}


void Micropolis::destroy()
{

  destroyMapArrays();

  // TODO: Clean up all other stuff:
  // Ink *overlay

}


////////////////////////////////////////////////////////////////////////
