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


static bool load_short(
    short *buf,
    int len,
    FILE *f)
{
    size_t result =
        fread(buf, sizeof(short), len, f);
    if ((int)result != len) {
         return false;
    }

    SWAP_SHORTS(buf, len);        /* to intel */

    return true;
}


static bool save_short(
    short *buf,
    int len,
    FILE *f)
{
    SWAP_SHORTS(buf, len);        /* to MAC */

    if ((int)fwrite(buf, sizeof(short), len, f) != len) {
        return false;
    }

    SWAP_SHORTS(buf, len);        /* back to intel */

    return true;
}


bool Micropolis::load_file(const char *filename, const char *dir)
{
    bool result = false;
    char *path = NULL;
    FILE *f;
    Quad size;

    if (dir != NULL) {
        path = (char *)malloc(strlen(dir) + 1 + strlen(filename) + 1);
        sprintf(path, "%s/%s", dir, filename);
        filename = path;
    }

    if ((f = fopen(filename, "rb")) == NULL) {
        goto done;
    }

    fseek(f, 0L, SEEK_END);
    size = ftell(f);
    fseek(f, 0L, SEEK_SET);

    result =
      (size == 27120) &&
      load_short(ResHis, HISTLEN / sizeof(short), f) &&
      load_short(ComHis, HISTLEN / sizeof(short), f) &&
      load_short(IndHis, HISTLEN / sizeof(short), f) &&
      load_short(CrimeHis, HISTLEN / sizeof(short), f) &&
      load_short(PollutionHis, HISTLEN / sizeof(short), f) &&
      load_short(MoneyHis, HISTLEN / sizeof(short), f) &&
      load_short(MiscHis, MISCHISTLEN / sizeof(short), f) &&
      load_short((&Map[0][0]), WORLD_X * WORLD_Y, f);

    fclose(f);

 done:
    if (path != NULL) {
        free(path);
    }

    return result;
}


bool Micropolis::loadFile(const char *filename)
{
    long n;

    if (!load_file(filename, NULL)) {
        return false;
    }

    /* total funds is a long.....    MiscHis is array of shorts */
    /* total funds is being put in the 50th & 51th word of MiscHis */
    /* find the address, cast the ptr to a longPtr, take contents */

    n = *(Quad *)(MiscHis + 50);
    HALF_SWAP_LONGS(&n, 1);
    SetFunds(n);

    n = *(Quad *)(MiscHis + 8);
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

    n = *(Quad *)(MiscHis + 58);
    HALF_SWAP_LONGS(&n, 1);
    policePercent = ((float)n) / ((float)65536);

    n = *(Quad *)(MiscHis + 60);
    HALF_SWAP_LONGS(&n, 1);
    firePercent = (float)n / (float)65536.0;

    n = *(Quad *)(MiscHis + 62);
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

    CityTime = max((Quad)0, CityTime);

    // If the tax is nonsensical, set it to a reasonable value.
    if ((CityTax > 20) ||
        (CityTax < 0)) {
        CityTax = 7;
    }

    // If the speed is nonsensical, set it to a reasonable value.
    if ((SimSpeed < 0) ||
        (SimSpeed > 3)) {
        SimSpeed = 3;
    }

    setSpeed(SimSpeed);
    setSkips(0);
    InitFundingLevel();

    // Set the scenario id to 0.
    InitWillStuff();
    ScenarioID = SC_NONE;
    InitSimLoad = 1;
    DoInitialEval = false;
    DoSimInit();
    InvalidateEditors();
    InvalidateMaps();

    return true;
}


/**
 * Save a game to disk.
 * @param filename Name of the file to use for storing the game.
 * @return The game was saved successfully.
 */
bool Micropolis::saveFile(const char *filename)
{
    long n;
    FILE *f;

    if ((f = fopen(filename, "wb")) == NULL) {
        /// @todo Report error saving file.
        return false;
    }

    /* total funds is a long.....    MiscHis is array of ints */
    /* total funds is bien put in the 50th & 51th word of MiscHis */
    /* find the address, cast the ptr to a longPtr, take contents */

    n = TotalFunds;
    HALF_SWAP_LONGS(&n, 1);
    (*(Quad *)(MiscHis + 50)) = n;

    n = CityTime;
    HALF_SWAP_LONGS(&n, 1);
    (*(Quad *)(MiscHis + 8)) = n;

    MiscHis[52] = autoBulldoze;   // flag for autoBulldoze
    MiscHis[53] = autoBudget;     // flag for autoBudget
    MiscHis[54] = autoGo;         // flag for auto-goto
    MiscHis[55] = UserSoundOn;    // flag for the sound on/off
    MiscHis[57] = SimSpeed;
    MiscHis[56] = CityTax;        /* post release */

    /* yayaya */

    n = (int)(policePercent * 65536);
    HALF_SWAP_LONGS(&n, 1);
    (*(Quad *)(MiscHis + 58)) = n;

    n = (int)(firePercent * 65536);
    HALF_SWAP_LONGS(&n, 1);
    (*(Quad *)(MiscHis + 60)) = n;

    n = (int)(roadPercent * 65536);
    HALF_SWAP_LONGS(&n, 1);
    (*(Quad *)(MiscHis + 62)) = n;

    bool result =
        save_short(ResHis, HISTLEN / 2, f) &&
        save_short(ComHis, HISTLEN / 2, f) &&
        save_short(IndHis, HISTLEN / 2, f) &&
        save_short(CrimeHis, HISTLEN / 2, f) &&
        save_short(PollutionHis, HISTLEN / 2, f) &&
        save_short(MoneyHis, HISTLEN / 2, f) &&
        save_short(MiscHis, MISCHISTLEN / 2, f) &&
        save_short((&Map[0][0]), WORLD_X * WORLD_Y, f);

    fclose(f);

    return result;
}


/**
 * Load a scenario.
 * @param s Scenario to load.
 * @note \a s cannot be \c SC_NONE.
 */
void Micropolis::LoadScenario(Scenario s)
{
    const char *name = NULL;
    const char *fname = NULL;

    CityFileName = "";

    SetGameLevel(LEVEL_EASY);

    if (s < SC_DULLSVILLE || s > SC_RIO) {
        s = SC_DULLSVILLE;
    }

    switch (s) {
        case SC_DULLSVILLE:
            name = "Dullsville";
            fname = "snro.111";
            ScenarioID = SC_DULLSVILLE;
            CityTime = ((1900 - 1900) * 48) + 2;
            SetFunds(5000);
            break;
        case SC_SAN_FRANCISCO:
            name = "San Francisco";
            fname = "snro.222";
            ScenarioID = SC_SAN_FRANCISCO;
            CityTime = ((1906 - 1900) * 48) + 2;
            SetFunds(20000);
            break;
        case SC_HAMBURG:
            name = "Hamburg";
            fname = "snro.333";
            ScenarioID = SC_HAMBURG;
            CityTime = ((1944 - 1900) * 48) + 2;
            SetFunds(20000);
            break;
        case SC_BERN:
            name = "Bern";
            fname = "snro.444";
            ScenarioID = SC_BERN;
            CityTime = ((1965 - 1900) * 48) + 2;
            SetFunds(20000);
            break;
        case SC_TOKYO:
            name = "Tokyo";
            fname = "snro.555";
            ScenarioID = SC_TOKYO;
            CityTime = ((1957 - 1900) * 48) + 2;
            SetFunds(20000);
            break;
        case SC_DETROIT:
            name = "Detroit";
            fname = "snro.666";
            ScenarioID = SC_DETROIT;
            CityTime = ((1972 - 1900) * 48) + 2;
            SetFunds(20000);
            break;
        case SC_BOSTON:
            name = "Boston";
            fname = "snro.777";
            ScenarioID = SC_BOSTON;
            CityTime = ((2010 - 1900) * 48) + 2;
            SetFunds(20000);
            break;
        case SC_RIO:
            name = "Rio de Janeiro";
            fname = "snro.888";
            ScenarioID = SC_RIO;
            CityTime = ((2047 - 1900) * 48) + 2;
            SetFunds(20000);
            break;
        default:
            NOT_REACHED();
            break;
    }

    setCleanCityName(name);
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


/** Report to the front-end that the scenario was loaded. */
void Micropolis::DidLoadScenario()
{
    Callback("UIDidLoadScenario", "");
}

/**
 * Try to load a new game from disk.
 * @param filename Name of the file to load.
 * @return Game was loaded successfully.
 * @todo In what state is the game left when loading fails?
 * @todo String normalization code is duplicated in #SaveCityAs(). Extract to
 *       a sub-function.
 * @bug Function fails if \c lastDot<lastSlash (ie with \c "x.y/bla" )
 */
bool Micropolis::LoadCity(const char *filename)
{
    if (loadFile(filename)) {

        CityFileName = filename;

        unsigned int lastSlash = CityFileName.find_last_of('/');
        unsigned int pos = (lastSlash == std::string::npos) ? 0 : lastSlash + 1;

        unsigned int lastDot = CityFileName.find_last_of('.');
        unsigned int last =
            (lastDot == std::string::npos) ? CityFileName.length() : lastDot;

        std::string newCityName = CityFileName.substr(pos, last - pos);
        setCityName(newCityName);

        DidLoadCity();

        return true;

    } else {

        DidntLoadCity((filename && *filename) ? filename : "(null)");

        return false;

    }
}

/** Report to the frontend that the game was successfully loaded. */
void Micropolis::DidLoadCity()
{
    Callback("UIDidLoadCity", "");
}


/**
 * Report to the frontend that the game failed to load.
 * @param msg File that attempted to load
 */
void Micropolis::DidntLoadCity(const char *msg)
{
    Callback(
        "UIDidntLoadCity",
        "s",
        msg);
}


/**
 * Try to save the game.
 * @todo This is a no-op if the #CityFileName is empty.
 *       In that case, we should probably warn the user about the failure.
 */
void Micropolis::SaveCity()
{
    if (CityFileName.length() > 0) {

        DoSaveCityAs();

    } else {
        if (saveFile(CityFileName.c_str())) {

            DidSaveCity();

        } else {

            DidntSaveCity(CityFileName.c_str());

        }
    }
}


/** Report to the frontend that the city is being saved. */
void Micropolis::DoSaveCityAs()
{
    Callback("UISaveCityAs", "");
}


/** Report to the frontend that the city was saved successfully. */
void Micropolis::DidSaveCity()
{
    Callback("UIDidSaveCity", "");
}


/**
 * Report to the frontend that the city could not be saved.
 * @param msg Name of the file used
 */
void Micropolis::DidntSaveCity(const char *msg)
{
    Callback(
        "UIDidntSaveCity",
        "s",
        msg);
}


/**
 * Save the city under a new name (?)
 * @param filename Name of the file to use for storing the game.
 * @todo String normalization code is duplicated in #LoadCity(). Extract to
 *       a sub-function.
 * @bug Function fails if \c lastDot<lastSlash (ie with \c "x.y/bla" )
 */
void Micropolis::SaveCityAs(const char *filename)
{
    CityFileName = filename;

    if (saveFile(CityFileName.c_str())) {

        unsigned int lastDot = CityFileName.find_last_of('.');
        unsigned int lastSlash = CityFileName.find_last_of('/');

        unsigned int pos =
            (lastSlash == std::string::npos) ? 0 : lastSlash + 1;
        unsigned int last =
            (lastDot == std::string::npos) ? CityFileName.length() : lastDot;
        unsigned int len =
            last - pos;
        std::string newCityName =
            CityFileName.substr(pos, len);

        setCityName(newCityName);

        DidSaveCity();

    } else {

        DidntSaveCity(CityFileName.c_str());

    }
}


////////////////////////////////////////////////////////////////////////
