/* w_sim.c
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


Tcl_HashTable SimCmds;


#define SIMCMD_CALL(proc) \
  int SimCmd##proc(ARGS) { proc(); return (TCL_OK); }

#define SIMCMD_CALL_KICK(proc) \
  int SimCmd##proc(ARGS) { proc(); Kick(); return (TCL_OK); }

#define SIMCMD_CALL_INT(proc) \
  int SimCmd##proc(ARGS) { \
    int val; \
    if (argc != 3) return (TCL_ERROR); \
    if ((Tcl_GetInt(interp, argv[2], &val) != TCL_OK)) return (TCL_ERROR); \
    proc(val); \
    return (TCL_OK); \
  }

#define SIMCMD_CALL_STR(proc) \
  int SimCmd##proc(ARGS) { \
    if (argc != 3) return (TCL_ERROR); \
    proc(argv[2]); \
    return (TCL_OK); \
  }

#define SIMCMD_CALL_TILEXY(proc) \
  int SimCmd##proc(ARGS) { \
    int x, y; \
    if (argc != 4) return (TCL_ERROR); \
    if ((Tcl_GetInt(interp, argv[2], &x) != TCL_OK) || \
	(x < 0) || (x >= WORLD_X)) return (TCL_ERROR); \
    if ((Tcl_GetInt(interp, argv[3], &y) != TCL_OK) || \
	(y < 0) || (y >= WORLD_Y)) return (TCL_ERROR); \
    proc(x, y); \
    return (TCL_OK); \
  }

#define SIMCMD_ACCESS_INT(var) \
  int SimCmd##var(ARGS) { \
    int val; \
    if ((argc != 2) && (argc != 3)) return (TCL_ERROR); \
    if (argc == 3) { \
      if (Tcl_GetInt(interp, argv[2], &val) != TCL_OK) return (TCL_ERROR); \
      var = val; \
    } \
    sprintf(interp->result, "%d", var); \
    return (TCL_OK); \
  }

#define SIMCMD_GET_INT(var) \
  int SimCmd##var(ARGS) { \
    sprintf(interp->result, "%d", var); \
    return (TCL_OK); \
  }

#define SIMCMD_GET_STR(var) \
  int SimCmd##var(ARGS) { \
    sprintf(interp->result, "%s", var); \
    return (TCL_OK); \
  }


SIMCMD_CALL_KICK(GameStarted)
SIMCMD_CALL_KICK(InitGame)
SIMCMD_CALL(SaveCity)
SIMCMD_CALL(ReallyQuit)
SIMCMD_CALL_KICK(UpdateHeads)
SIMCMD_CALL_KICK(UpdateMaps)
SIMCMD_CALL_KICK(UpdateEditors)
SIMCMD_CALL_KICK(RedrawMaps)
SIMCMD_CALL_KICK(RedrawEditors)
SIMCMD_CALL_KICK(UpdateGraphs)
SIMCMD_CALL_KICK(UpdateEvaluation)
SIMCMD_CALL_KICK(UpdateBudget)
SIMCMD_CALL_KICK(UpdateBudgetWindow)
SIMCMD_CALL_KICK(DoBudget)
SIMCMD_CALL_KICK(DoBudgetFromMenu)
SIMCMD_CALL_KICK(Pause)
SIMCMD_CALL_KICK(Resume)
SIMCMD_CALL(StartBulldozer)
SIMCMD_CALL(StopBulldozer)
SIMCMD_CALL(MakeFire)
SIMCMD_CALL(MakeFlood)
SIMCMD_CALL(MakeTornado)
SIMCMD_CALL(MakeEarthquake)
SIMCMD_CALL(MakeMonster)
SIMCMD_CALL(MakeMeltdown)
SIMCMD_CALL(FireBomb)
SIMCMD_CALL(SoundOff)
SIMCMD_CALL(GenerateNewCity)
SIMCMD_CALL_INT(GenerateSomeCity)
SIMCMD_ACCESS_INT(LakeLevel)
SIMCMD_ACCESS_INT(TreeLevel)
SIMCMD_ACCESS_INT(CurveLevel)
SIMCMD_ACCESS_INT(CreateIsland)
SIMCMD_CALL_KICK(SmoothTrees)
SIMCMD_CALL_KICK(SmoothWater)
SIMCMD_CALL_KICK(SmoothRiver)
SIMCMD_CALL_KICK(ClearMap)
SIMCMD_CALL_KICK(ClearUnnatural)
SIMCMD_CALL_INT(LoadScenario)
SIMCMD_CALL_STR(LoadCity)
SIMCMD_CALL_STR(SaveCityAs)
SIMCMD_CALL_TILEXY(MakeExplosion)
SIMCMD_CALL(EraseOverlay)
SIMCMD_ACCESS_INT(OverRide)
SIMCMD_ACCESS_INT(Expensive)
SIMCMD_ACCESS_INT(Players)
SIMCMD_ACCESS_INT(Votes)
SIMCMD_ACCESS_INT(BobHeight)
SIMCMD_ACCESS_INT(PendingTool)
SIMCMD_ACCESS_INT(PendingX)
SIMCMD_ACCESS_INT(PendingY)
SIMCMD_GET_STR(Displays)


int SimCmdCityName(ARGS)
{
  if ((argc != 2) && (argc != 3)) {
    return (TCL_ERROR);
  }

  if (argc == 3) {
    setCityName(argv[2]);
  }

  sprintf(interp->result, "%s", CityName);
  return (TCL_OK);
}


int SimCmdCityFileName(ARGS)
{
  if ((argc != 2) && (argc != 3)) {
    return (TCL_ERROR);
  }

  if (argc == 3) {
    if (CityFileName != NULL) {
      ckfree(CityFileName);
      CityFileName = NULL;
    }
    if (argv[2][0] != '\0') {
      CityFileName = (char *)ckalloc(strlen(argv[0]) + 1);
      strcpy(CityFileName, argv[2]);
    }
  }

  sprintf(interp->result, "%s", CityFileName ? CityFileName : "");
  return (TCL_OK);
}


int SimCmdGameLevel(ARGS)
{
  int level;

  if ((argc != 2) && (argc != 3)) {
    return (TCL_ERROR);
  }

  if (argc == 3) {
    if ((Tcl_GetInt(interp, argv[2], &level) != TCL_OK) ||
	(level < 0) || (level > 2)) {
      return (TCL_ERROR);
    }
    SetGameLevelFunds(level);
  }

  sprintf(interp->result, "%d", GameLevel);
  return (TCL_OK);
}


int SimCmdSpeed(ARGS)
{
  int speed;

  if ((argc != 2) && (argc != 3)) {
    return (TCL_ERROR);
  }

  if (argc == 3) {
    if ((Tcl_GetInt(interp, argv[2], &speed) != TCL_OK) ||
	(speed < 0) || (speed > 7)) {
      return (TCL_ERROR);
    }
    setSpeed(speed); Kick();
  }

  sprintf(interp->result, "%d", SimSpeed);
  return (TCL_OK);
}


int SimCmdSkips(ARGS)
{
  int skips;

  if ((argc != 2) && (argc != 3)) {
    return (TCL_ERROR);
  }

  if (argc == 3) {
    if ((Tcl_GetInt(interp, argv[2], &skips) != TCL_OK) ||
	(skips < 0)) {
      return (TCL_ERROR);
    }
    setSkips(skips); Kick();
  }

  sprintf(interp->result, "%d", sim_skips);

  return (TCL_OK);
}


int SimCmdSkip(ARGS)
{
  int skip;

  if ((argc != 2) && (argc != 3)) {
    return (TCL_ERROR);
  }

  if (argc == 3) {
    if ((Tcl_GetInt(interp, argv[2], &skip) != TCL_OK) ||
	(skip < 0)) {
      return (TCL_ERROR);
    }
    sim_skip = skip;
  }

  sprintf(interp->result, "%d", sim_skip);

  return (TCL_OK);
}


int SimCmdDelay(ARGS)
{
  int delay;

  if ((argc != 2) && (argc != 3)) {
    return (TCL_ERROR);
  }

  if (argc == 3) {
    if ((Tcl_GetInt(interp, argv[2], &delay) != TCL_OK) ||
	(delay < 0)) {
      return (TCL_ERROR);
    }
    sim_delay = delay; Kick();
  }

  sprintf(interp->result, "%d", sim_delay);
  return (TCL_OK);
}


int SimCmdWorldX(ARGS)
{
  int val;

  if (argc != 2) {
    return (TCL_ERROR);
  }

  sprintf(interp->result, "%d", WORLD_X);
  return (TCL_OK);
}


int SimCmdWorldY(ARGS)
{
  int val;

  if (argc != 2) {
    return (TCL_ERROR);
  }

  sprintf(interp->result, "%d", WORLD_Y);
  return (TCL_OK);
}


int SimCmdHeatSteps(ARGS)
{
  int steps;

  if ((argc != 2) && (argc != 3)) {
    return (TCL_ERROR);
  }

  if (argc == 3) {
    if ((Tcl_GetInt(interp, argv[2], &steps) != TCL_OK) ||
	(steps < 0)) {
      return (TCL_ERROR);
    }
    heat_steps = steps; Kick();
  }

  sprintf(interp->result, "%d", heat_steps);
  return (TCL_OK);
}


int SimCmdHeatFlow(ARGS)
{
  int flow;

  if ((argc != 2) && (argc != 3)) {
    return (TCL_ERROR);
  }

  if (argc == 3) {
    if (Tcl_GetInt(interp, argv[2], &flow) != TCL_OK) {
      return (TCL_ERROR);
    }
    heat_flow = flow;
  }

  sprintf(interp->result, "%d", heat_flow);
  return (TCL_OK);
}


int SimCmdHeatRule(ARGS)
{
  int rule;

  if ((argc != 2) && (argc != 3)) {
    return (TCL_ERROR);
  }

  if (argc == 3) {
    if (Tcl_GetInt(interp, argv[2], &rule) != TCL_OK) {
      return (TCL_ERROR);
    }
    heat_rule = rule;
  }

  sprintf(interp->result, "%d", heat_rule);
  return (TCL_OK);
}


#ifdef CAM

int SimCmdJustCam(ARGS)
{
  int cam;

  if ((argc != 2) && (argc != 3)) {
    return (TCL_ERROR);
  }

  if (argc == 3) {
    if (Tcl_GetInt(interp, argv[2], &cam) != TCL_OK) {
      return (TCL_ERROR);
    }
    sim_just_cam = cam;
  }

  sprintf(interp->result, "%d", sim_just_cam);
  return (TCL_OK);
}

#endif


#ifdef NET

int SimCmdListenTo(ARGS)
{
  int port, sock;

  if (argc != 3) {
    return (TCL_ERROR);
  }

  if (Tcl_GetInt(interp, argv[2], &port) != TCL_OK) {
    return (TCL_ERROR);
  }

#ifdef NET
  sock = udp_listen(port);
#endif  

  sprintf(interp->result, "%d", sock);

  return (TCL_OK);
}


int SimCmdHearFrom(ARGS)
{
  int sock;

  if (argc != 3) {
    return (TCL_ERROR);
  }

  if ((argv[2][0] != 'f') ||
      (argv[2][1] != 'i') ||
      (argv[2][2] != 'l') ||
      (argv[2][3] != 'e') ||
      (Tcl_GetInt(interp, argv[2] + 4, &sock) != TCL_OK)) {
    return (TCL_ERROR);
  }

#ifdef NET
  udp_hear(sock);
#endif  

  return (TCL_OK);
}

#endif /* NET */


int SimCmdFunds(ARGS)
{
  int funds;

  if ((argc != 2) && (argc != 3)) {
    return (TCL_ERROR);
  }

  if (argc == 3) {
    if ((Tcl_GetInt(interp, argv[2], &funds) != TCL_OK) ||
	(funds < 0)) {
      return (TCL_ERROR);
    }
    TotalFunds = funds;
    MustUpdateFunds = 1;
    Kick();
  }

  sprintf(interp->result, "%d", TotalFunds);
  return (TCL_OK);
}


int SimCmdTaxRate(ARGS)
{
  int tax;

  if ((argc != 2) && (argc != 3)) {
    return (TCL_ERROR);
  }

  if (argc == 3) {
    if ((Tcl_GetInt(interp, argv[2], &tax) != TCL_OK) ||
	(tax < 0) || (tax > 20)) {
      return (TCL_ERROR);
    }
    CityTax = tax;
    drawBudgetWindow(); Kick();
  }

  sprintf(interp->result, "%d", CityTax);
  return (TCL_OK);
}


int SimCmdFireFund(ARGS)
{
  int percent;

  if ((argc != 2) && (argc != 3)) {
    return (TCL_ERROR);
  }

  if (argc == 3) {
    if ((Tcl_GetInt(interp, argv[2], &percent) != TCL_OK) ||
	(percent < 0) || (percent > 100)) {
      return (TCL_ERROR);
    }
    firePercent = percent / 100.0;
    FireSpend = (fireMaxValue * percent) / 100;
    UpdateFundEffects(); Kick();
  }

  sprintf(interp->result, "%d", (int)(firePercent * 100.0));
  return (TCL_OK);
}


int SimCmdPoliceFund(ARGS)
{
  int percent;

  if ((argc != 2) && (argc != 3)) {
    return (TCL_ERROR);
  }

  if (argc == 3) {
    if ((Tcl_GetInt(interp, argv[2], &percent) != TCL_OK) ||
	(percent < 0) || (percent > 100)) {
      return (TCL_ERROR);
    }
    policePercent = percent / 100.0;
    PoliceSpend = (policeMaxValue * percent) / 100;
    UpdateFundEffects(); Kick();
  }

  sprintf(interp->result, "%d", (int)(policePercent * 100.0));
  return (TCL_OK);
}


int SimCmdRoadFund(ARGS)
{
  int percent;

  if ((argc != 2) && (argc != 3)) {
    return (TCL_ERROR);
  }

  if (argc == 3) {
    if ((Tcl_GetInt(interp, argv[2], &percent) != TCL_OK) ||
	(percent < 0) || (percent > 100)) {
      return (TCL_ERROR);
    }
    roadPercent = percent / 100.0;
    RoadSpend = (roadMaxValue * percent) / 100;
    UpdateFundEffects(); Kick();
  }

  sprintf(interp->result, "%d", (int)(roadPercent * 100.0));
  return (TCL_OK);
}


int SimCmdYear(ARGS)
{
  int year;

  if ((argc != 2) && (argc != 3)) {
    return (TCL_ERROR);
  }

  if (argc == 3) {
    if ((Tcl_GetInt(interp, argv[2], &year) != TCL_OK)) {
      return (TCL_ERROR);
    }
    SetYear(year);
  }

  sprintf(interp->result, "%d", CurrentYear());
  return (TCL_OK);
}


int SimCmdAutoBudget(ARGS)
{
  int val;

  if ((argc != 2) && (argc != 3)) {
    return (TCL_ERROR);
  }

  if (argc == 3) {
    if ((Tcl_GetInt(interp, argv[2], &val) != TCL_OK) ||
	(val < 0) || (val > 1)) {
      return (TCL_ERROR);
    }
    autoBudget = val;
    MustUpdateOptions = 1; Kick();
    UpdateBudget();
  }

  sprintf(interp->result, "%d", autoBudget);
  return (TCL_OK);
}


int SimCmdAutoGoto(ARGS)
{
  int val;

  if ((argc != 2) && (argc != 3)) {
    return (TCL_ERROR);
  }

  if (argc == 3) {
    if ((Tcl_GetInt(interp, argv[2], &val) != TCL_OK) ||
	(val < 0) || (val > 1)) {
      return (TCL_ERROR);
    }
    autoGo = val;
    MustUpdateOptions = 1; Kick();
  }

  sprintf(interp->result, "%d", autoGo);
  return (TCL_OK);
}


int SimCmdAutoBulldoze(ARGS)
{
  int val;

  if ((argc != 2) && (argc != 3)) {
    return (TCL_ERROR);
  }

  if (argc == 3) {
    if ((Tcl_GetInt(interp, argv[2], &val) != TCL_OK) ||
	(val < 0) || (val > 1)) {
      return (TCL_ERROR);
    }
    autoBulldoze = val;
    MustUpdateOptions = 1; Kick();
  }

  sprintf(interp->result, "%d", autoBulldoze);
  return (TCL_OK);
}


int SimCmdDisasters(ARGS)
{
  int val;

  if ((argc != 2) && (argc != 3)) {
    return (TCL_ERROR);
  }

  if (argc == 3) {
    if ((Tcl_GetInt(interp, argv[2], &val) != TCL_OK) ||
	(val < 0) || (val > 1)) {
      return (TCL_ERROR);
    }
    NoDisasters = val ? 0 : 1;
    MustUpdateOptions = 1; Kick();
  }

  sprintf(interp->result, "%d", NoDisasters ? 0 : 1);
  return (TCL_OK);
}


int SimCmdSound(ARGS)
{
  int val;

  if ((argc != 2) && (argc != 3)) {
    return (TCL_ERROR);
  }

  if (argc == 3) {
    if ((Tcl_GetInt(interp, argv[2], &val) != TCL_OK) ||
	(val < 0) || (val > 1)) {
      return (TCL_ERROR);
    }
    UserSoundOn = val;
    MustUpdateOptions = 1; Kick();
  }

  sprintf(interp->result, "%d", UserSoundOn);
  return (TCL_OK);
}


int SimCmdFlush(ARGS)
{
  int style;

  if (argc != 2) {
    return (TCL_ERROR);
  }
  
  return (TCL_OK);
}


int SimCmdFlushStyle(ARGS)
{
  int style;

  if ((argc != 2) && (argc != 3)) {
    return (TCL_ERROR);
  }

  if (argc == 3) {
    if ((Tcl_GetInt(interp, argv[2], &style) != TCL_OK) ||
	(style < 0)) {
      return (TCL_ERROR);
    }
    FlushStyle = style;
  }

  sprintf(interp->result, "%d", FlushStyle);
  return (TCL_OK);
}


int SimCmdDonDither(ARGS)
{
  int dd;

  if ((argc != 2) && (argc != 3)) {
    return (TCL_ERROR);
  }

  if (argc == 3) {
    if ((Tcl_GetInt(interp, argv[2], &dd) != TCL_OK) ||
	(dd < 0)) {
      return (TCL_ERROR);
    }
    DonDither = dd;
  }

  sprintf(interp->result, "%d", DonDither);
  return (TCL_OK);
}


int SimCmdDoOverlay(ARGS)
{
  int dd;

  if ((argc != 2) && (argc != 3)) {
    return (TCL_ERROR);
  }

  if (argc == 3) {
    if ((Tcl_GetInt(interp, argv[2], &dd) != TCL_OK) ||
	(dd < 0)) {
      return (TCL_ERROR);
    }
    DoOverlay = dd;
  }

  sprintf(interp->result, "%d", DoOverlay);
  return (TCL_OK);
}


int SimCmdMonsterGoal(ARGS)
{
  SimSprite *sprite;
  int x, y;

  if (argc != 4) {
    return (TCL_ERROR);
  }

  if (Tcl_GetInt(interp, argv[2], &x) != TCL_OK) {
    return (TCL_ERROR);
  }
  if (Tcl_GetInt(interp, argv[3], &y) != TCL_OK) {
    return (TCL_ERROR);
  }
  if ((sprite = GetSprite(GOD)) == NULL) {
    MakeMonster();
    if ((sprite = GetSprite(GOD)) == NULL)
      return (TCL_ERROR);
  }
  sprite->dest_x = x;
  sprite->dest_y = y;
  sprite->control = -2;
  sprite->count = -1;

  return (TCL_OK);
}


int SimCmdHelicopterGoal(ARGS)
{
  int x, y;
  SimSprite *sprite;

  if (argc != 4) {
    return (TCL_ERROR);
  }

  if (Tcl_GetInt(interp, argv[2], &x) != TCL_OK) {
    return (TCL_ERROR);
  }
  if (Tcl_GetInt(interp, argv[3], &y) != TCL_OK) {
    return (TCL_ERROR);
  }
  
  if ((sprite = GetSprite(COP)) == NULL) {
    GenerateCopter(x, y);
    if ((sprite = GetSprite(COP)) == NULL) {
      return (TCL_ERROR);
    }
  }
  sprite->dest_x = x;
  sprite->dest_y = y;

  return (TCL_OK);
}


int SimCmdMonsterDirection(ARGS)
{
  int dir;
  SimSprite *sprite;

  if (argc != 3) {
    return (TCL_ERROR);
  }

  if ((Tcl_GetInt(interp, argv[2], &dir) != TCL_OK) ||
      (dir < -1) || (dir > 7)) {
    return (TCL_ERROR);
  }
  if ((sprite = GetSprite(GOD)) == NULL) {
    MakeMonster();
    if ((sprite = GetSprite(GOD)) == NULL) {
      return (TCL_ERROR);
    }
  }
  sprite->control = dir;

  return (TCL_OK);
}


int SimCmdTile(ARGS)
{
  int x, y, tile;

  if ((argc != 4) && (argc != 5)) {
    return (TCL_ERROR);
  }
  if ((Tcl_GetInt(interp, argv[2], &x) != TCL_OK) ||
      (x < 0) ||
      (x >= WORLD_X) ||
      (Tcl_GetInt(interp, argv[3], &y) != TCL_OK) ||
      (y < 0) ||
      (y >= WORLD_Y)) {
    return (TCL_ERROR);
  }
  if (argc == 5) {
    if (Tcl_GetInt(interp, argv[4], &tile) != TCL_OK) {
      return (TCL_ERROR);
    }
    Map[x][y] = tile;
  }
  sprintf(interp->result, "%d", Map[x][y]);
  return (TCL_OK);
}


int SimCmdFill(ARGS)
{
  int tile, x, y;

  if (argc != 3) {
    return (TCL_ERROR);
  }
  if (Tcl_GetInt(interp, argv[2], &tile) != TCL_OK) {
    return (TCL_ERROR);
  }
  for (x = 0; x < WORLD_X; x++) {
    for (y = 0; y < WORLD_Y; y++) {
      Map[x][y] = tile;
    }
  }
  sprintf(interp->result, "%d", tile);
  return (TCL_OK);
}


int SimCmdDynamicData(ARGS)
{
  int index, val;

  if ((argc != 3) && (argc != 4)) {
    return (TCL_ERROR);
  }

  if ((Tcl_GetInt(interp, argv[2], &index) != TCL_OK) ||
      (index < 0) ||
      (index >= 32)) {
    return (TCL_ERROR);
  }

  if (argc == 4) {
    int val;

    if (Tcl_GetInt(interp, argv[3], &val) != TCL_OK) {
      return (TCL_ERROR);
    }
    DynamicData[index] = val;
    NewMapFlags[DYMAP] = 1;
    Kick();
  }

  sprintf(interp->result, "%d", DynamicData[index]);
  return (TCL_OK);
}


int SimCmdResetDynamic(ARGS)
{
  int i;

  for (i = 0; i < 16; i++) {
    DynamicData[i] = (i & 1) ? 99999 : -99999;
  }
  NewMapFlags[DYMAP] = 1;
  Kick();
  return (TCL_OK);
}


int SimCmdPerformance(ARGS)
{
  SimView *view;

  PerformanceTiming = 1;
  FlushTime = 0.0;
  for (view = sim->editor; view != NULL; view = view->next) {
    view->updates = 0;
    view->update_real = view->update_user = view->update_system = 0.0;
  }
  return (TCL_OK);
}


int SimCmdCollapseMotion(ARGS)
{
  int val;

  if ((argc != 2) && (argc != 3)) {
    return (TCL_ERROR);
  }

  if (argc == 3) {
    if ((Tcl_GetInt(interp, argv[2], &val) != TCL_OK)) {
      return (TCL_ERROR);
    }
    tkCollapseMotion = val;
  }

  sprintf(interp->result, "%d", tkCollapseMotion);
  return (TCL_OK);
}


int SimCmdUpdate(ARGS)
{
  sim_update();
  return (TCL_OK);
}


int SimCmdLandValue(ARGS)
{
  int val;

  if (argc != 2) {
    return (TCL_ERROR);
  }

  sprintf(interp->result, "%d", LVAverage);
  return (TCL_OK);
}


int SimCmdTraffic(ARGS)
{
  int val;

  if (argc != 2) {
    return (TCL_ERROR);
  }

  sprintf(interp->result, "%d", AverageTrf());
  return (TCL_OK);
}


int SimCmdCrime(ARGS)
{
  int val;

  if (argc != 2) {
    return (TCL_ERROR);
  }

  sprintf(interp->result, "%d", CrimeAverage);
  return (TCL_OK);
}


int SimCmdUnemployment(ARGS)
{
  int val;

  if (argc != 2) {
    return (TCL_ERROR);
  }

  sprintf(interp->result, "%d", GetUnemployment());
  return (TCL_OK);
}


int SimCmdFires(ARGS)
{
  int val;

  if (argc != 2) {
    return (TCL_ERROR);
  }

  sprintf(interp->result, "%d", GetFire());
  return (TCL_OK);
}


int SimCmdPollution(ARGS)
{
  int val;

  if (argc != 2) {
    return (TCL_ERROR);
  }

  sprintf(interp->result, "%d", PolluteAverage);
  return (TCL_OK);
}


int SimCmdPolMaxX(ARGS)
{
  int val;

  if (argc != 2) {
    return (TCL_ERROR);
  }

  sprintf(interp->result, "%d", (PolMaxX <<4) + 8);
  return (TCL_OK);
}


int SimCmdPolMaxY(ARGS)
{
  int val;

  if (argc != 2) {
    return (TCL_ERROR);
  }

  sprintf(interp->result, "%d", (PolMaxY <<4) + 8);
  return (TCL_OK);
}


int SimCmdTrafMaxX(ARGS)
{
  int val;

  if (argc != 2) {
    return (TCL_ERROR);
  }

  sprintf(interp->result, "%d", TrafMaxX);
  return (TCL_OK);
}


int SimCmdTrafMaxY(ARGS)
{
  int val;

  if (argc != 2) {
    return (TCL_ERROR);
  }

  sprintf(interp->result, "%d", TrafMaxY);
  return (TCL_OK);
}


int SimCmdMeltX(ARGS)
{
  int val;

  if (argc != 2) {
    return (TCL_ERROR);
  }

  sprintf(interp->result, "%d", (MeltX <<4) + 8);
  return (TCL_OK);
}


int SimCmdMeltY(ARGS)
{
  int val;

  if (argc != 2) {
    return (TCL_ERROR);
  }

  sprintf(interp->result, "%d", (MeltY <<4) + 8);
  return (TCL_OK);
}


int SimCmdCrimeMaxX(ARGS)
{
  int val;

  if (argc != 2) {
    return (TCL_ERROR);
  }

  sprintf(interp->result, "%d", (CrimeMaxX <<4) + 8);
  return (TCL_OK);
}


int SimCmdCrimeMaxY(ARGS)
{
  int val;

  if (argc != 2) {
    return (TCL_ERROR);
  }

  sprintf(interp->result, "%d", (CrimeMaxY <<4) + 8);
  return (TCL_OK);
}


int SimCmdCenterX(ARGS)
{
  int val;

  if (argc != 2) {
    return (TCL_ERROR);
  }

  sprintf(interp->result, "%d", (CCx <<4) + 8);
  return (TCL_OK);
}


int SimCmdCenterY(ARGS)
{
  int val;

  if (argc != 2) {
    return (TCL_ERROR);
  }

  sprintf(interp->result, "%d", (CCy <<4) + 8);
  return (TCL_OK);
}


int SimCmdFloodX(ARGS)
{
  int val;

  if (argc != 2) {
    return (TCL_ERROR);
  }

  sprintf(interp->result, "%d", (FloodX <<4) + 8);
  return (TCL_OK);
}


int SimCmdFloodY(ARGS)
{
  int val;

  if (argc != 2) {
    return (TCL_ERROR);
  }

  sprintf(interp->result, "%d", (FloodY <<4) + 8);
  return (TCL_OK);
}


int SimCmdCrashX(ARGS)
{
  int val;

  if (argc != 2) {
    return (TCL_ERROR);
  }

  sprintf(interp->result, "%d", (CrashX <<4) + 8);
  return (TCL_OK);
}


int SimCmdCrashY(ARGS)
{
  int val;

  if (argc != 2) {
    return (TCL_ERROR);
  }

  sprintf(interp->result, "%d", (CrashY <<4) + 8);
  return (TCL_OK);
}


int SimCmdDollars(ARGS)
{
  int val;

  if (argc != 2) {
    return (TCL_ERROR);
  }

  makeDollarDecimalStr(argv[1], interp->result);
  return (TCL_OK);
}


int SimCmdDoAnimation(ARGS)
{
  int val;

  if ((argc != 2) && (argc != 3)) {
    return (TCL_ERROR);
  }

  if (argc == 3) {
    if ((Tcl_GetInt(interp, argv[2], &val) != TCL_OK)) {
      return (TCL_ERROR);
    }
    DoAnimation = val;
    MustUpdateOptions = 1; Kick();
  }

  sprintf(interp->result, "%d", DoAnimation);
  return (TCL_OK);
}


int SimCmdDoMessages(ARGS)
{
  int val;

  if ((argc != 2) && (argc != 3)) {
    return (TCL_ERROR);
  }

  if (argc == 3) {
    if ((Tcl_GetInt(interp, argv[2], &val) != TCL_OK)) {
      return (TCL_ERROR);
    }
    DoMessages = val;
    MustUpdateOptions = 1; Kick();
  }

  sprintf(interp->result, "%d", DoMessages);
  return (TCL_OK);
}


int SimCmdDoNotices(ARGS)
{
  int val;

  if ((argc != 2) && (argc != 3)) {
    return (TCL_ERROR);
  }

  if (argc == 3) {
    if ((Tcl_GetInt(interp, argv[2], &val) != TCL_OK)) {
      return (TCL_ERROR);
    }
    DoNotices = val;
    MustUpdateOptions = 1; Kick();
  }

  sprintf(interp->result, "%d", DoNotices);
  return (TCL_OK);
}


int SimCmdRand(ARGS)
{
  int val, r;

  if ((argc != 2) && (argc != 3)) {
    return (TCL_ERROR);
  }

  if (argc == 3) {
    if ((Tcl_GetInt(interp, argv[2], &val) != TCL_OK)) {
      return (TCL_ERROR);
    }
    r = Rand(val);
  } else {
    r = Rand16();
  }

  sprintf(interp->result, "%d", r);
  return (TCL_OK);
}


int SimCmdPlatform(ARGS)
{

#ifdef MSDOS
  sprintf(interp->result, "msdos");
#else
  sprintf(interp->result, "unix");
#endif

  return (TCL_OK);
}


int SimCmdVersion(ARGS)
{
  sprintf(interp->result, MicropolisVersion);

  return (TCL_OK);
}


int SimCmdOpenWebBrowser(ARGS)
{
  int result = 1;
  char buf[512];

  if ((argc != 3) ||
      (strlen(argv[2]) > 255)) {
    return (TCL_ERROR);
  }

  sprintf(buf, 
	  "netscape -no-about-splash '%s' &",
	  argv[2]);

  result = system(buf);
  
  sprintf(interp->result, "%d", result);

  return (TCL_OK);
}


int SimCmdQuoteURL(ARGS)
{
  int result = 1;
  char buf[2048];
  char *from, *to;
  int ch;
  static char *hexDigits =
    "0123456789ABCDEF";

  if ((argc != 3) ||
      (strlen(argv[2]) > 255)) {
    return (TCL_ERROR);
  }

  from = argv[2];
  to = buf;

  while ((ch = *(from++)) != '\0') {
    if ((ch < 32) ||
	(ch >= 128) ||
	(ch == '+') ||
	(ch == '%') ||
	(ch == '&') ||
	(ch == '<') ||
	(ch == '>') ||
	(ch == '"') ||
	(ch == '\'')) {
      *to++ = '%';
      *to++ = hexDigits[(ch >> 4) & 0x0f];
      *to++ = hexDigits[ch & 0x0f];
    } else if (ch == 32) {
      *to++ = '+';
    } else {
      *to++ = ch;
    } // if
  } // while
  
  *to = '\0';

  sprintf(interp->result, "%s", buf);

  return (TCL_OK);
}


int SimCmdNeedRest(ARGS)
{
  int needRest;

  if ((argc != 2) && (argc != 3)) {
    return (TCL_ERROR);
  }

  if (argc == 3) {
    if (Tcl_GetInt(interp, argv[2], &needRest) != TCL_OK) {
      return (TCL_ERROR);
    }
    NeedRest = needRest;
  }

  sprintf(interp->result, "%d", NeedRest);
  return (TCL_OK);
}


int SimCmdMultiPlayerMode(ARGS)
{
  /* This is read-only because it's specified on
     the command line and effects how the user 
     interface is initialized. */

  if (argc != 2) {
    return (TCL_ERROR);
  }

  sprintf(interp->result, "%d", MultiPlayerMode);
  return (TCL_OK);
}


int SimCmdSugarMode(ARGS)
{
  /* This is read-only because it's specified on
     the command line and effects how the user 
     interface is initialized. */

  if (argc != 2) {
    return (TCL_ERROR);
  }

  sprintf(interp->result, "%d", SugarMode);
  return (TCL_OK);
}


/************************************************************************/

int
SimCmd(CLIENT_ARGS)
{
  Tcl_HashEntry *ent;
  int result = TCL_OK;
  int (*cmd)();

  if (argc < 2) {
    return TCL_ERROR;
  }

  if (ent = Tcl_FindHashEntry(&SimCmds, argv[1])) {
    cmd = (int (*)())ent->clientData;
    result = cmd(interp, argc, argv);
  } else {
    result = TCL_ERROR;
  }
  return result;
}


sim_command_init()
{
  int new;

  Tcl_CreateCommand(tk_mainInterp, "sim", SimCmd,
		    (ClientData)MainWindow, (void (*)()) NULL);

  Tcl_InitHashTable(&SimCmds, TCL_STRING_KEYS);

#define SIM_CMD(name) HASHED_CMD(Sim, name)

  SIM_CMD(GameStarted);
  SIM_CMD(InitGame);
  SIM_CMD(SaveCity);
  SIM_CMD(ReallyQuit);
  SIM_CMD(UpdateHeads);
  SIM_CMD(UpdateMaps);
  SIM_CMD(RedrawEditors);
  SIM_CMD(RedrawMaps);
  SIM_CMD(UpdateEditors);
  SIM_CMD(UpdateGraphs);
  SIM_CMD(UpdateEvaluation);
  SIM_CMD(UpdateBudget);
  SIM_CMD(UpdateBudgetWindow);
  SIM_CMD(DoBudget);
  SIM_CMD(DoBudgetFromMenu);
  SIM_CMD(Pause);
  SIM_CMD(Resume);
  SIM_CMD(StartBulldozer);
  SIM_CMD(StopBulldozer);
  SIM_CMD(MakeFire);
  SIM_CMD(MakeFlood);
  SIM_CMD(MakeTornado);
  SIM_CMD(MakeEarthquake);
  SIM_CMD(MakeMonster);
  SIM_CMD(MakeMeltdown);
  SIM_CMD(FireBomb);
  SIM_CMD(SoundOff);
  SIM_CMD(GenerateNewCity);
  SIM_CMD(GenerateSomeCity);
  SIM_CMD(TreeLevel);
  SIM_CMD(LakeLevel);
  SIM_CMD(CurveLevel);
  SIM_CMD(CreateIsland);
  SIM_CMD(ClearMap);
  SIM_CMD(ClearUnnatural);
  SIM_CMD(SmoothTrees);
  SIM_CMD(SmoothWater);
  SIM_CMD(SmoothRiver);
  SIM_CMD(LoadScenario);
  SIM_CMD(LoadCity);
  SIM_CMD(SaveCityAs);
  SIM_CMD(MakeExplosion);
  SIM_CMD(CityName);
  SIM_CMD(CityFileName);
  SIM_CMD(GameLevel);
  SIM_CMD(Speed);
  SIM_CMD(Skips);
  SIM_CMD(Skip);
  SIM_CMD(WorldX);
  SIM_CMD(WorldY);
  SIM_CMD(Delay);
  SIM_CMD(HeatSteps);
  SIM_CMD(HeatFlow);
  SIM_CMD(HeatRule);
#ifdef CAM
  SIM_CMD(JustCam);
#endif
#ifdef NET
  SIM_CMD(ListenTo);
  SIM_CMD(HearFrom);
#endif
  SIM_CMD(Funds);
  SIM_CMD(TaxRate);
  SIM_CMD(FireFund);
  SIM_CMD(PoliceFund);
  SIM_CMD(RoadFund);
  SIM_CMD(Year);
  SIM_CMD(AutoBudget);
  SIM_CMD(AutoGoto);
  SIM_CMD(AutoBulldoze);
  SIM_CMD(Disasters);
  SIM_CMD(Sound);
  SIM_CMD(Flush);
  SIM_CMD(FlushStyle);
  SIM_CMD(DonDither);
  SIM_CMD(DoOverlay);
  SIM_CMD(MonsterGoal);
  SIM_CMD(HelicopterGoal);
  SIM_CMD(MonsterDirection);
  SIM_CMD(EraseOverlay);
  SIM_CMD(Tile);
  SIM_CMD(Fill);
  SIM_CMD(DynamicData);
  SIM_CMD(ResetDynamic);
  SIM_CMD(Performance);
  SIM_CMD(CollapseMotion);
  SIM_CMD(Update);
  SIM_CMD(OverRide);
  SIM_CMD(Expensive);
  SIM_CMD(Players);
  SIM_CMD(Votes);
  SIM_CMD(BobHeight);
  SIM_CMD(PendingTool);
  SIM_CMD(PendingX);
  SIM_CMD(PendingY);
  SIM_CMD(Displays);
  SIM_CMD(LandValue);
  SIM_CMD(Traffic);
  SIM_CMD(Crime);
  SIM_CMD(Unemployment);
  SIM_CMD(Fires);
  SIM_CMD(Pollution);
  SIM_CMD(PolMaxX);
  SIM_CMD(PolMaxY);
  SIM_CMD(TrafMaxX);
  SIM_CMD(TrafMaxY);
  SIM_CMD(MeltX);
  SIM_CMD(MeltY);
  SIM_CMD(CrimeMaxX);
  SIM_CMD(CrimeMaxY);
  SIM_CMD(CenterX);
  SIM_CMD(CenterY);
  SIM_CMD(FloodX);
  SIM_CMD(FloodY);
  SIM_CMD(CrashX);
  SIM_CMD(CrashY);
  SIM_CMD(Dollars);
  SIM_CMD(DoAnimation);
  SIM_CMD(DoMessages);
  SIM_CMD(DoNotices);
  SIM_CMD(Rand);
  SIM_CMD(Platform);
  SIM_CMD(Version);
  SIM_CMD(OpenWebBrowser);
  SIM_CMD(QuoteURL);
  SIM_CMD(NeedRest);
  SIM_CMD(MultiPlayerMode);
  SIM_CMD(SugarMode);
}
