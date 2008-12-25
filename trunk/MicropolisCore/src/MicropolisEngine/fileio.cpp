/* fileio.cpp
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

/** @file fileio.cpp */

////////////////////////////////////////////////////////////////////////


#include "stdafx.h"


////////////////////////////////////////////////////////////////////////


#if defined(IS_INTEL)


#define SWAP_SHORTS(a,b)        swap_shorts(a,b)
#define HALF_SWAP_LONGS(a,b)    half_swap_longs(a,b)


static void swap_shorts(
  short *buf,
  int len)
{
  int i;

  /* Flip bytes in each short! */
  for (i = 0; i < len; i++) {
    *buf = ((*buf & 0xFF) <<8) | ((*buf &0xFF00) >>8);
    buf++;
  }
}


static void half_swap_longs(
  long *buf,
  int len)
{
  int i;

  /* Flip bytes in each long! */
  for (i = 0; i < len; i++) {
    long l = *buf;
    *buf =
      ((l & 0x0000ffff) << 16) |
      ((l & 0xffff0000) >> 16);
    buf++;
  }
}


#else


#define SWAP_SHORTS(a, b)
#define HALF_SWAP_LONGS(a, b)


#endif


static int load_short(
  short *buf,
  int len,
  FILE *f)
{
  size_t result =
    fread(buf, sizeof(short), len, f);
  if ((int)result != len) {
     return 0;
  }

  SWAP_SHORTS(buf, len);        /* to intel */

  return 1;
}


static int save_short(
  short *buf,
  int len,
  FILE *f)
{

  SWAP_SHORTS(buf, len);        /* to MAC */

  if ((int)fwrite(buf, sizeof(short), len, f) != len) {
     return 0;
  }

  SWAP_SHORTS(buf, len);        /* back to intel */

  return 1;
}


int Micropolis::load_file(
  char *filename,
  char *dir)
{
  FILE *f;
  char path[512];
  Quad size;

  if (dir != NULL) {
    sprintf(path, "%s/%s", dir, filename);
    filename = path;
  }

  if ((f = fopen(filename, "rb")) == NULL) {
    return (0);
  }

  fseek(f, 0L, SEEK_END);
  size = ftell(f);
  fseek(f, 0L, SEEK_SET);

  switch (size) {

  case 27120: /* Normal city */
    break;

  case 99120: /* 2x2 city */
    break;

  case 219120: /* 3x3 city */
    break;

  default:
    return (0);

  }

  if ((load_short(ResHis, HISTLEN / sizeof(short), f) == 0) ||
      (load_short(ComHis, HISTLEN / sizeof(short), f) == 0) ||
      (load_short(IndHis, HISTLEN / sizeof(short), f) == 0) ||
      (load_short(CrimeHis, HISTLEN / sizeof(short), f) == 0) ||
      (load_short(PollutionHis, HISTLEN / sizeof(short), f) == 0) ||
      (load_short(MoneyHis, HISTLEN / sizeof(short), f) == 0) ||
      (load_short(MiscHis, MISCHISTLEN / sizeof(short), f) == 0) ||
      (load_short((&Map[0][0]), WORLD_X * WORLD_Y, f) < 0)) {

    /* TODO:  report error */
    fclose(f);
    return (0);

  }

  fclose(f);
  return (1);
}


int Micropolis::loadFile(
  char *filename)
{
  long n;

  if (load_file(filename, NULL) == 0) {
    return(0);
  }

  /* total funds is a long.....    MiscHis is array of shorts */
  /* total funds is being put in the 50th & 51th word of MiscHis */
  /* find the address, cast the ptr to a lontPtr, take contents */

  n =
    *(Quad *)(MiscHis + 50);
  HALF_SWAP_LONGS(&n, 1);
  SetFunds(n);

  n =
    *(Quad *)(MiscHis + 8);
  HALF_SWAP_LONGS(&n, 1);
  CityTime = n;

  autoBulldoze = (MiscHis[52] != 0);   // flag for autoBulldoze
  autoBudget   = (MiscHis[53] != 0);   // flag for autoBudget
  autoGo       = (MiscHis[54] != 0);   // flag for auto-goto
  UserSoundOn  = (MiscHis[55] != 0);   // flag for the sound on/off
  CityTax = MiscHis[56];
  SimSpeed = MiscHis[57];
  ChangeCensus();
  MustUpdateOptions = 1;

  /* yayaya */

  n =
    *(Quad *)(MiscHis + 58);
  HALF_SWAP_LONGS(&n, 1);
  policePercent =
    ((float)n) / ((float)65536);

  n =
    *(Quad *)(MiscHis + 60);
  HALF_SWAP_LONGS(&n, 1);
  firePercent =
    (float)n / (float)65536.0;

  n =
    *(Quad *)(MiscHis + 62);
  HALF_SWAP_LONGS(&n, 1);
  roadPercent = (float)n / (float)65536.0;

  policePercent =
    (float)(*(Quad*)(MiscHis + 58)) /
    (float)65536.0;   /* and 59 */
  firePercent =
    (float)(*(Quad*)(MiscHis + 60)) /
    (float)65536.0;   /* and 61 */
  roadPercent =
    (float)(*(Quad*)(MiscHis + 62)) /
    (float)65536.0;   /* and 63 */

  if (CityTime < 0) {
    CityTime = 0;
  }

  if ((CityTax > 20) ||
      (CityTax < 0)) {
    CityTax = 7;
  }

  if ((SimSpeed < 0) ||
      (SimSpeed > 3)) {
    SimSpeed = 3;
  }

  setSpeed(SimSpeed);
  setSkips(0);
  InitFundingLevel();

  /* set the scenario id to 0 */
  InitWillStuff();
  ScenarioID = SC_NONE;
  InitSimLoad = 1;
  DoInitialEval = false;
  DoSimInit();
  InvalidateEditors();
  InvalidateMaps();

  return (1);
}


int Micropolis::saveFile(
  char *filename)
{
  long n;
  FILE *f;

  if ((f = fopen(filename, "wb")) == NULL) {
    /* TODO: report error */
    return(0);
  }

  /* total funds is a long.....    MiscHis is array of ints */
  /* total funds is bien put in the 50th & 51th word of MiscHis */
  /* find the address, cast the ptr to a lontPtr, take contents */

  n = TotalFunds;
  HALF_SWAP_LONGS(&n, 1);
  (*(Quad *)(MiscHis + 50)) =
    n;

  n = CityTime;
  HALF_SWAP_LONGS(&n, 1);
  (*(Quad *)(MiscHis + 8)) =
    n;

  MiscHis[52] = autoBulldoze;   // flag for autoBulldoze
  MiscHis[53] = autoBudget;     // flag for autoBudget
  MiscHis[54] = autoGo;         // flag for auto-goto
  MiscHis[55] = UserSoundOn;    // flag for the sound on/off
  MiscHis[57] = SimSpeed;
  MiscHis[56] = CityTax;        /* post release */

  /* yayaya */

  n =
    (int)(policePercent * 65536);
  HALF_SWAP_LONGS(&n, 1);
  (*(Quad *)(MiscHis + 58)) =
    n;

  n =
    (int)(firePercent * 65536);
  HALF_SWAP_LONGS(&n, 1);
  (*(Quad *)(MiscHis + 60)) =
    n;

  n =
    (int)(roadPercent * 65536);
  HALF_SWAP_LONGS(&n, 1);
  (*(Quad *)(MiscHis + 62)) =
    n;

  if ((save_short(ResHis, HISTLEN / 2, f) == 0) ||
      (save_short(ComHis, HISTLEN / 2, f) == 0) ||
      (save_short(IndHis, HISTLEN / 2, f) == 0) ||
      (save_short(CrimeHis, HISTLEN / 2, f) == 0) ||
      (save_short(PollutionHis, HISTLEN / 2, f) == 0) ||
      (save_short(MoneyHis, HISTLEN / 2, f) == 0) ||
      (save_short(MiscHis, MISCHISTLEN / 2, f) == 0) ||
      (save_short((&Map[0][0]), WORLD_X * WORLD_Y, f) < 0)) {

    /* TODO:  report error */
    fclose(f);
    return(0);

  }

  fclose(f);
  return(1);
}

/**
 * Load a scenario
 * @param s Scenario to load
 * @note \a s cannot be \c SC_NONE
 */
void Micropolis::LoadScenario(Scenario s)
{
  char *name = NULL;
  char *fname = NULL;

  if (CityFileName != NULL) {
    FreePtr(CityFileName);
    CityFileName = NULL;
  }

  SetGameLevel(LEVEL_EASY);

  if (s < SC_DULLSVILLE || s > SC_RIO) {
    s = SC_DULLSVILLE;
  }

  switch (s) {
  case SC_DULLSVILLE:
    name = "Dullsville";
    fname = "snro.111";
    ScenarioID = SC_DULLSVILLE;
    CityTime =
      ((1900 - 1900) * 48) + 2;
    SetFunds(5000);
    break;
  case SC_SAN_FRANCISCO:
    name = "San Francisco";
    fname = "snro.222";
    ScenarioID = SC_SAN_FRANCISCO;
    CityTime =
      ((1906 - 1900) * 48) + 2;
    SetFunds(20000);
    break;
  case SC_HAMBURG:
    name = "Hamburg";
    fname = "snro.333";
    ScenarioID = SC_HAMBURG;
    CityTime =
      ((1944 - 1900) * 48) + 2;
    SetFunds(20000);
    break;
  case SC_BERN:
    name = "Bern";
    fname = "snro.444";
    ScenarioID = SC_BERN;
    CityTime =
      ((1965 - 1900) * 48) + 2;
    SetFunds(20000);
    break;
  case SC_TOKYO:
    name = "Tokyo";
    fname = "snro.555";
    ScenarioID = SC_TOKYO;
    CityTime =
      ((1957 - 1900) * 48) + 2;
    SetFunds(20000);
    break;
  case SC_DETROIT:
    name = "Detroit";
    fname = "snro.666";
    ScenarioID = SC_DETROIT;
    CityTime =
      ((1972 - 1900) * 48) + 2;
    SetFunds(20000);
    break;
  case SC_BOSTON:
    name = "Boston";
    fname = "snro.777";
    ScenarioID = SC_BOSTON;
    CityTime =
      ((2010 - 1900) * 48) + 2;
    SetFunds(20000);
    break;
  case SC_RIO:
    name = "Rio de Janeiro";
    fname = "snro.888";
    ScenarioID = SC_RIO;
    CityTime =
      ((2047 - 1900) * 48) + 2;
    SetFunds(20000);
    break;
  default:
    NOT_REACHED();
    break;
  }

  setAnyCityName(name);
  setSpeed(3);
  CityTax = 7;

  load_file(
    fname,
    ResourceDir);

  InitWillStuff();
  InitFundingLevel();
  UpdateFunds();
  InvalidateEditors();
  InvalidateMaps();
  InitSimLoad = 1;
  DoInitialEval = false;
  DoSimInit();
  DidLoadScenario();
}


void Micropolis::DidLoadScenario()
{
  Callback("UIDidLoadScenario", "");
}


int Micropolis::LoadCity(
  char *filename)
{
  char *cp;
  char msg[256];

  if (loadFile(filename)) {
    if (CityFileName != NULL) {
      FreePtr(CityFileName);
    }

    CityFileName =
      (char *)NewPtr((int)strlen(filename) + 1);

    strcpy(CityFileName, filename);

    cp = (char *)strrchr(filename, '.');
    if (cp != NULL) {
      *cp = 0;
    }

    cp = (char *)strrchr(filename, '/');
    if (cp != NULL) {
      cp++;
    } else {
      cp = filename;
    }

    filename =
      (char *)NewPtr((int)strlen(cp) + 1);

    strcpy(filename, cp);

    setCityName(filename);

    DidLoadCity();

    return (1);

  } else {

    sprintf(
      msg,
      "Unable to load a city from the file named \"%s\". %s",
      filename ? filename : "(null)",
      errno ? strerror(errno) : "");

    DidntLoadCity(msg);

    return (0);

  }
}


void Micropolis::DidLoadCity()
{
  Callback("UIDidLoadCity", "");
}


void Micropolis::DidntLoadCity(
  char *msg)
{
  Callback(
        "UIDidntLoadCity",
        "s",
        msg);
}


void Micropolis::SaveCity()
{
  char msg[256];

  if (CityFileName == NULL) {

    DoSaveCityAs();

  } else {
    if (saveFile(CityFileName)) {

      DidSaveCity();

    } else {

      sprintf(
        msg,
        "Unable to save the city to the file named \"%s\". %s",
        CityFileName ? CityFileName : "(null)",
        errno ? strerror(errno) : "");

      DidntSaveCity(msg);

    }
  }
}


void Micropolis::DoSaveCityAs()
{
  Callback("UISaveCityAs", "");
}


void Micropolis::DidSaveCity()
{
  Callback("UIDidSaveCity", "");
}


void Micropolis::DidntSaveCity(
  char *msg)
{
  Callback(
        "UIDidntSaveCity",
        "s",
        msg);
}


void Micropolis::SaveCityAs(
  char *filename)
{
  char *cp;

  if (CityFileName != NULL) {
    FreePtr(CityFileName);
  }
  CityFileName =
    (char *)NewPtr((int)strlen(filename) + 1);
  strcpy(CityFileName, filename);

  if (saveFile(CityFileName)) {

    cp = (char *)strrchr(filename, '.');
    if (cp != NULL) {
      *cp = 0;
    }

    cp = (char *)strrchr(filename, '/');
    if (cp != NULL) {
      cp++;
    } else {
      cp = filename;
    }

    filename =
      (char *)NewPtr((int)strlen(cp) + 1);

    strcpy(filename, cp);

    setCityName(cp);

    DidSaveCity();

  } else {

    char msg[2048];

    sprintf(
      msg,
      "Unable to save the city to the file named \"%1024s\". %512s",
      CityFileName ? CityFileName : "(null)",
      errno ? strerror(errno) : "");

    DidntSaveCity(msg);

  }
}


////////////////////////////////////////////////////////////////////////
