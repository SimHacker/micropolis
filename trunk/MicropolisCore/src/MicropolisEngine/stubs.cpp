/* stubs.cpp
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

/** @file stubs.cpp */

////////////////////////////////////////////////////////////////////////


#include "stdafx.h"


////////////////////////////////////////////////////////////////////////


/**
 * Deduct \a dollars from the player funds.
 * @param dollars Amount of money spent.
 */
void Micropolis::Spend(int dollars)
{
    SetFunds(TotalFunds - dollars);
}


/**
 * Set player funds to \a dollars.
 *
 * Modify the player funds, and warn the front-end about the new amount of
 * money.
 * @param dollars New value for the player funds.
 */
void Micropolis::SetFunds(int dollars)
{
    TotalFunds = dollars;
    UpdateFunds();
}


/**
 * Get number of ticks.
 * @todo Figure out what a 'tick' is.
 * @bug Unix version looks wrong, \c time.tv_usec should be divided to get
 *      seconds or \c time.tc_sec should be multiplied.
 */
Quad Micropolis::TickCount()
{
#ifdef _WIN32
    return (::GetTickCount() * 60) / 1000;
#else
    struct timeval time;
    gettimeofday(&time, 0);
    return (Quad)((time.tv_sec / 60) + (time.tv_usec * 1000000 / 60));
#endif
}


/**
 * Claim \a size bytes of memory.
 * @param size Number of bytes to claim.
 * @return Pointer to the claimed memory.
 */
Ptr Micropolis::NewPtr(int size)
{
    return (Ptr)malloc(size);
}

/**
 * Release claimed memory.
 * @param data Pointer to previously claimed memory.
 */
void Micropolis::FreePtr(void *data)
{
    free(data);
}


/** @bug Function is never called. */
void Micropolis::DoPlayNewCity()
{
    Callback("UIPlayNewCity", "");
}


/** @bug Function is never called. */
void Micropolis::DoReallyStartGame()
{
    Callback("UIReallyStartGame", "");
}


/** @bug Function is never called. */
void Micropolis::DoStartLoad()
{
    Callback("UIStartLoad", "");
}


/**
 * Tell the front-end a scenario is started.
 * @param scenario The scenario being started.
 * @see Scenario.
 * @bug Function is never called.
 */
void Micropolis::DoStartScenario(int scenario)
{
    Callback( "UIStartScenario", "d", (int)scenario);
}


/** Tell the front-end that fire bombs are being dropped. */
void Micropolis::DropFireBombs()
{
    Callback("UIDropFireBombs", "");
}


/**
 * Initialize the game.
 * @todo we seem to have several of these functions.
 * @bug Function is never called.
 */
void Micropolis::InitGame()
{
    sim_paused = false; // Simulation is running.
    sim_paused_speed = 0;
    sim_skip = 0;
    sim_skips = 0;
    heat_steps = 0; // Disable cellular automata machine.
    setSpeed(0);
}

/**
 * Scripting language independent callback mechanism.
 *
 * This allows Micropolis to send callback messages with
 * a variable number of typed parameters back to the
 * scripting language, while maintining independence from
 * the particular scripting language (or user interface
 * runtime).
 *
 * The name is the name of a message to send.
 * The params is a string that specifies the number and
 * types of the following vararg parameters.
 * There is one character in the param string per vararg
 * parameter. The following parameter types are currently
 * supported:
 *  - i: integer
 *  - f: float
 *  - s: string
 *
 * See PythonCallbackHook defined in \c swig/micropolis-swig-python.i
 * for an example of a callback function.
 * @param name   Name of the callback.
 * @param params Parameters of the callback.
 */
void Micropolis::Callback(const char *name, const char *params, ...)
{
    if (callbackHook == NULL) {
        return;
    }

    va_list arglist;
    va_start(arglist, params); // beginning after last named argument: params

    (*callbackHook)(this, callbackData, name, params, arglist);

    va_end(arglist);
}


/**
 * Tell the front-end to show an earth quake to the user (shaking the map for
 * some time).
 */
void Micropolis::DoEarthquake()
{
    MakeSound("city", "Explosion-Low");

    int magnitude = Rand(10) + 3;
    Callback("UIStartEarthquake", "d", magnitude);
}


/** Tell the front-end that the editors are not valid any more */
void Micropolis::InvalidateEditors()
{
    Callback("UIInvalidateEditors", "");
}


/** Tell the front-end that the maps are not valid any more */
void Micropolis::InvalidateMaps()
{
    Callback("UIInvalidateMaps", "");
}

/**
 * Tell the front-end to initialize its sound system.
 * @todo Should be handled by the front-end entirely?
 */
void Micropolis::InitializeSound()
{
    Callback("UIInitializeSound", "");
}

/**
 * Instruct the front-end to make a sound.
 * @param channel Name of the channel.
 * @param sound   Name of the sound.
 * @todo Figure out what 'name of the channel' actually is and whether we need
 *       it.
 */
void Micropolis::MakeSound(const char *channel, const char *sound)
{
    Callback( "UIMakeSound", "ss", channel, sound);
}


/**
 * Get a tile from the map.
 * @param x X coordinate of the position to get.
 * @param y Y coordinate of the position to get.
 * @return Value of the map at the given position.
 * @note Off-map positions are considered to contain #DIRT.
 * @todo Find out where this function is used (probably in he front-end).
 */
int Micropolis::getTile(int x, int y)
{
    if (!TestBounds(x, y)) {
        return DIRT;
    }

    return Map[x][y];
}


/**
 * Get the address of the internal buffer containing the map.
 * @return Pointer to the start of the world map buffer.
 * @todo Why does this function exist? Internal function can access it
 *       directly, external functions should not need it.
 * @todo Find out where this function is used.
 */
void *Micropolis::getMapBuffer()
{
    return (void *)mapPtr;
}


////////////////////////////////////////////////////////////////////////
