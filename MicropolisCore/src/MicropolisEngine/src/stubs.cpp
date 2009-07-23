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
#include "micropolis.h"
#include "stubs.h"

////////////////////////////////////////////////////////////////////////
// Frontend message storage

/** Base class constructor. */
FrontendMessage::FrontendMessage()
{
}


/** Base class destructor. */
FrontendMessage::~FrontendMessage()
{
}


/**
 * @fn void FrontendMessage::sendMessage(Micropolis *sim)
 * @brief Send the message to the front-end.
 * @param sim Simulator instance to use.
 */



FrontendMessageDidTool::FrontendMessageDidTool(const char *tool, int x, int y)
{
    this->tool = tool;
    this->x = x;
    this->y = y;
}


FrontendMessageDidTool::~FrontendMessageDidTool()
{
}


void FrontendMessageDidTool::sendMessage(Micropolis *sim) const
{
    sim->didTool(this->tool, this->x, this->y);
}


FrontendMessageMakeSound::FrontendMessageMakeSound(
                                const char *channel, const char *sound,
                                int x, int y)
{
    this->channel = channel;
    this->sound = sound;
    this->x = x;
    this->y = y;
}


FrontendMessageMakeSound::~FrontendMessageMakeSound()
{
}


void FrontendMessageMakeSound::sendMessage(Micropolis *sim) const
{
    sim->makeSound(this->channel, this->sound, this->x, this->y);
}


////////////////////////////////////////////////////////////////////////


/**
 * Deduct \a dollars from the player funds.
 * @param dollars Amount of money spent.
 */
void Micropolis::spend(int dollars)
{
    setFunds(totalFunds - dollars);
}


/**
 * Set player funds to \a dollars.
 *
 * Modify the player funds, and warn the front-end about the new amount of
 * money.
 * @param dollars New value for the player funds.
 */
void Micropolis::setFunds(int dollars)
{
    totalFunds = dollars;
    updateFunds();
}


/**
 * Get number of ticks.
 * @todo Figure out what a 'tick' is.
 * @bug Unix version looks wrong, \c time.tv_usec should be divided to get
 *      seconds or \c time.tc_sec should be multiplied.
 */
Quad Micropolis::tickCount()
{
#ifdef _WIN32
    return (::GetTickCount() * 60) / 1000;
#else
    struct timeval time;
    gettimeofday(&time, 0);
    return (Quad)((time.tv_sec * 60) + (time.tv_usec * 60) / 1000000);
#endif
}


/**
 * Claim \a size bytes of memory.
 * @param size Number of bytes to claim.
 * @return Pointer to the claimed memory.
 */
Ptr Micropolis::newPtr(int size)
{
    return (Ptr)malloc(size);
}


/**
 * Release claimed memory.
 * @param data Pointer to previously claimed memory.
 */
void Micropolis::freePtr(void *data)
{
    free(data);
}


/** @bug Function is never called. */
void Micropolis::doPlayNewCity()
{
    callback("playNewCity", "");
}


/** @bug Function is never called. */
void Micropolis::doReallyStartGame()
{
    callback("reallyStartGame", "");
}


/** @bug Function is never called. */
void Micropolis::doStartLoad()
{
    callback("startLoad", "");
}


/**
 * Tell the front-end a scenario is started.
 * @param scenario The scenario being started.
 * @see Scenario.
 * @bug Function is never called.
 */
void Micropolis::doStartScenario(int scenario)
{
    callback( "startScenario", "d", (int)scenario);
}


/**
 * Initialize the game.
 * This is called from the scripting language.
 * @todo we seem to have several of these functions.
 */
void Micropolis::initGame()
{
    simPaused = false; // Simulation is running.
    simPausedSpeed = 0;
    simPass = 0;
    simPasses = 1;
    heatSteps = 0; // Disable cellular automata machine.
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
void Micropolis::callback(const char *name, const char *params, ...)
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
 * Tell the front-end to show an earthquake to the user (shaking the map for
 * some time).
 */
void Micropolis::doEarthquake(int strength)
{
    makeSound("city", "ExplosionLow"); // Make the sound all over.

    callback("startEarthquake", "d", strength);
}


/** Tell the front-end that the maps are not valid any more */
void Micropolis::invalidateMaps()
{
    mapSerial++;
    callback("update", "s", "map"); // new
}


/**
 * Instruct the front-end to make a sound.
 * @param channel Name of the sound channel, which can effect the
 *                sound (location, volume, spatialization, etc).
 *                Use "city" for city sounds effects, and "interface"
 *                for user interface sounds.
 * @param sound   Name of the sound.
 * @param x       Tile X position of sound, 0 to WORLD_W, or -1 for everywhere.
 * @param y       Tile Y position of sound, 0 to WORLD_H, or -1 for everywhere.
 */
void Micropolis::makeSound(const char *channel, const char *sound,
                           int x, int y)
{
    if (enableSound) {
        callback("makeSound", "ssdd", channel, sound, x, y);
    }
}


/**
 * Get a tile from the map.
 * @param x X coordinate of the position to get, 0 to WORLD_W.
 * @param y Y coordinate of the position to get, 0 to WORLD_H.
 * @return Value of the map at the given position.
 * @note Off-map positions are considered to contain #DIRT.
 */
int Micropolis::getTile(int x, int y)
{
    if (!testBounds(x, y)) {
        return DIRT;
    }

    return map[x][y];
}


/**
 * Set a tile into the map.
 * @param x X coordinate of the position to get, 0 to WORLD_W.
 * @param y Y coordinate of the position to get, 0 to WORLD_H.
 * @param tile the tile value to set.
 * @note Off-map positions are ignored.
 */
void Micropolis::setTile(int x, int y, int tile)
{
    if (!testBounds(x, y)) {
        return;
    }

    map[x][y] = (unsigned short)tile;
}


/**
 * Get the address of the internal buffer containing the map. This is
 * used to enable the tile engine to access the tiles directly.
 * @return Pointer to the start of the world map buffer.
 */
void *Micropolis::getMapBuffer()
{
    return (void *)mapBase;
}


/**
 * Get a value from the power grid map.
 * @param x X coordinate of the position to get, 0 to WORLD_W.
 * @param y Y coordinate of the position to get, 0 to WORLD_H.
 * @return Value of the power grid map at the given position.
 * @note Off-map positions are considered to contain 0.
 * @todo Use world coordinates instead (use powerGridMap.worldGet() instead).
 */
int Micropolis::getPowerGrid(int x, int y)
{
    return powerGridMap.worldGet(x, y);
}


/**
 * Set a value in the power grid map.
 * @param x X coordinate of the position to get, 0 to WORLD_W.
 * @param y Y coordinate of the position to get, 0 to WORLD_H.
 * @param power the value to set.
 * @note Off-map positions are ignored.
 * @todo Use world coordinates instead (use powerGridMap.worldSet() instead).
 */
void Micropolis::setPowerGrid(int x, int y, int power)
{
  powerGridMap.worldSet(x, y, power);
}


/**
 * Get the address of the internal buffer containing the power grid
 * map.  This is used to enable the tile engine to access the power
 * grid map directly.
 * @return Pointer to the start of the power grid map buffer.
 */
void *Micropolis::getPowerGridMapBuffer()
{
    return (void *)powerGridMap.getBase();
}


/**
 * Get a value from the population density map.
 * @param x X coordinate of the position to get, 0 to WORLD_W_2.
 * @param y Y coordinate of the position to get, 0 to WORLD_H_2.
 * @return Value of the population density map at the given position.
 * @note Off-map positions are considered to contain 0.
 * @todo Use world coordinates instead (use populationDensityMap.worldGet() instead).
 */
int Micropolis::getPopulationDensity(int x, int y)
{
    return populationDensityMap.get(x, y);
}


/**
 * Set a value in the population density map.
 * @param x X coordinate of the position to get, 0 to WORLD_W_2.
 * @param y Y coordinate of the position to get, 0 to WORLD_H_2.
 * @param density the value to set.
 * @note Off-map positions are ignored.
 * @todo Use world coordinates instead (use populationDensityMap.worldSet() instead).
 */
void Micropolis::setPopulationDensity(int x, int y, int density)
{
    populationDensityMap.set(x, y, density);
}


/**
 * Get the address of the internal buffer containing the population
 * density map. This is used to enable the tile engine to access the
 * population density map directly.
 * @return Pointer to the start of the population density map buffer.
 */
void *Micropolis::getPopulationDensityMapBuffer()
{
    return (void *)populationDensityMap.getBase();
}


/**
 * Get a value from the rate of growth map.
 * @param x X coordinate of the position to get, 0 to WORLD_W_8.
 * @param y Y coordinate of the position to get, 0 to WORLD_H_8.
 * @return Value of the rate of growth map at the given position.
 * @note Off-map positions are considered to contain 0.
 * @todo Use world coordinates instead (use rateOfGrowthMap.worldGet() instead).
 */
int Micropolis::getRateOfGrowth(int x, int y)
{
    return rateOfGrowthMap.get(x, y);
}


/**
 * Set a value in the rate of growth map.
 * @param x X coordinate of the position to get, 0 to WORLD_W_8.
 * @param y Y coordinate of the position to get, 0 to WORLD_H_8.
 * @param rate the value to set.
 * @note Off-map positions are ignored.
 * @todo Use world coordinates instead (use rateOfGrowthMap.worldSet() instead).
 */
void Micropolis::setRateOfGrowth(int x, int y, int rate)
{
    rateOfGrowthMap.set(x, y, rate);
}


/**
 * Get the address of the internal buffer containing the rate of
 * growth map.  This is used to enable the tile engine to access the
 * rate of growth map directly.
 * @return Pointer to the start of the rate of growth map buffer.
 */
void *Micropolis::getRateOfGrowthMapBuffer()
{
    return (void *)rateOfGrowthMap.getBase();
}


/**
 * Get a value from the traffic density map.
 * @param x X coordinate of the position to get, 0 to WORLD_W_2.
 * @param y Y coordinate of the position to get, 0 to WORLD_H_2.
 * @return Value of the traffic density at the given position.
 * @note Off-map positions are considered to contain 0.
 * @todo Use world coordinates instead (use trafficDensityMap.worldGet() instead).
 */
int Micropolis::getTrafficDensity(int x, int y)
{
    return trafficDensityMap.get(x, y);
}


/**
 * Set a value in the traffic density map.
 * @param x X coordinate of the position to get, 0 to WORLD_W_2.
 * @param y Y coordinate of the position to get, 0 to WORLD_H_2.
 * @param density the value to set.
 * @note Off-map positions are ignored.
 * @todo Use world coordinates instead (use trafficDensityMap.worldSet() instead).
 */
void Micropolis::setTrafficDensity(int x, int y, int density)
{
    trafficDensityMap.set(x, y, density);
}


/**
 * Get the address of the internal buffer containing the traffic
 * density map. This is used to enable the tile engine to access the
 * traffic density map directly.
 * @return Pointer to the start of the traffic density map buffer.
 */
void *Micropolis::getTrafficDensityMapBuffer()
{
    return (void *)trafficDensityMap.getBase();
}


/**
 * Get a value from the pollution density map.
 * @param x X coordinate of the position to get, 0 to WORLD_W_2.
 * @param y Y coordinate of the position to get, 0 to WORLD_H_2.
 * @return Value of the rate of pollution density map at the given position.
 * @note Off-map positions are considered to contain 0.
 * @todo Use world coordinates instead (use pollutionDensityMap.worldGet() instead).
 */
int Micropolis::getPollutionDensity(int x, int y)
{
    return pollutionDensityMap.get(x, y);
}


/**
 * Set a value in the pollition density map.
 * @param x X coordinate of the position to get, 0 to WORLD_W_2.
 * @param y Y coordinate of the position to get, 0 to WORLD_H_2.
 * @param density the value to set.
 * @note Off-map positions are ignored.
 * @todo Use world coordinates instead (use pollutionDensityMap.worldSet() instead).
 */
void Micropolis::setPollutionDensity(int x, int y, int density)
{
    pollutionDensityMap.set(x, y, density);
}


/**
 * Get the address of the internal buffer containing the pollution
 * density map. This is used to enable the tile engine to access the
 * pollution density map directly.
 * @return Pointer to the start of the pollution density map buffer.
 */
void *Micropolis::getPollutionDensityMapBuffer()
{
    return (void *)pollutionDensityMap.getBase();
}


/**
 * Get a value from the crime rate map.
 * @param x X coordinate of the position to get, 0 to WORLD_W_2.
 * @param y Y coordinate of the position to get, 0 to WORLD_H_2.
 * @return Value of the population density map at the given position.
 * @note Off-map positions are considered to contain 0.
 * @todo Use world coordinates instead (use crimeRateMap.worldGet() instead).
 */
int Micropolis::getCrimeRate(int x, int y)
{
    return crimeRateMap.get(x, y);
}


/**
 * Set a value in the crime rate map.
 * @param x X coordinate of the position to get, 0 to WORLD_W_2.
 * @param y Y coordinate of the position to get, 0 to WORLD_H_2.
 * @param rate the value to set.
 * @note Off-map positions are ignored.
 * @todo Use world coordinates instead (use crimeRateMap.worldSet() instead).
 */
void Micropolis::setCrimeRate(int x, int y, int rate)
{
    crimeRateMap.set(x, y, rate);
}


/**
 * Get the address of the internal buffer containing the crime rate
 * map. This is used to enable the tile engine to access the crime
 * rate map directly.
 * @return Pointer to the start of the crime rate map buffer.
 */
void *Micropolis::getCrimeRateMapBuffer()
{
    return (void *)crimeRateMap.getBase();
}


/**
 * Get a value from the land value map.
 * @param x X coordinate of the position to get, 0 to WORLD_W_2.
 * @param y Y coordinate of the position to get, 0 to WORLD_H_2.
 * @return Value of the land value map at the given position.
 * @note Off-map positions are considered to contain 0.
 * @todo Use world coordinates instead (use landValueMap.worldGet() instead).
 */
int Micropolis::getLandValue(int x, int y)
{
    return landValueMap.get(x, y);
}


/**
 * Set a value in the land value map.
 * @param x X coordinate of the position to get, 0 to WORLD_W_2.
 * @param y Y coordinate of the position to get, 0 to WORLD_H_2.
 * @param value the value to set.
 * @note Off-map positions are ignored.
 * @todo Use world coordinates instead (use landValueMap.worldSet() instead).
 */
void Micropolis::setLandValue(int x, int y, int value)
{
    landValueMap.set(x, y, value);
}


/**
 * Get the address of the internal buffer containing the land value
 * map. This is used to enable the tile engine to access the land
 * value map directly.
 * @return Pointer to the start of the land value map buffer.
 */
void *Micropolis::getLandValueMapBuffer()
{
    return (void *)landValueMap.getBase();
}


/**
 * Get a value from the fire coverage map.
 * @param x X coordinate of the position to get, 0 to WORLD_W_8.
 * @param y Y coordinate of the position to get, 0 to WORLD_H_8.
 * @return Value of the fir coverage map at the given position.
 * @note Off-map positions are considered to contain 0.
 * @todo Use world coordinates instead (use fireStationEffectMap.worldGet() instead).
 */
int Micropolis::getFireCoverage(int x, int y)
{
    return fireStationEffectMap.get(x, y);
}


/**
 * Set a value in the fire coverage map.
 * @param x X coordinate of the position to get, 0 to WORLD_W_8.
 * @param y Y coordinate of the position to get, 0 to WORLD_H_8.
 * @param coverage the value to set.
 * @note Off-map positions are ignored.
 * @todo Use world coordinates instead (use fireStationEffectMap.worldSet() instead).
 */
void Micropolis::setFireCoverage(int x, int y, int coverage)
{
    fireStationEffectMap.set(x, y, coverage);
}


/**
 * Get the address of the internal buffer containing the fire coverage
 * map. This is used to enable the tile engine to access the fire
 * coverage map directly.
 * @return Pointer to the start of the fire coverage map buffer.
 */
void *Micropolis::getFireCoverageMapBuffer()
{
    return (void *)fireStationEffectMap.getBase();
}

/**
 * Get a value from the police coverage map.
 * @param x X coordinate of the position to get, 0 to WORLD_W_8.
 * @param y Y coordinate of the position to get, 0 to WORLD_H_8.
 * @return Value of the fir coverage map at the given position.
 * @note Off-map positions are considered to contain 0.
 * @todo Use world coordinates instead (use policeStationEffectMap.worldGet() instead).
 */
int Micropolis::getPoliceCoverage(int x, int y)
{
    return policeStationEffectMap.get(x, y);
}


/**
 * Set a value in the police coverage map.
 * @param x X coordinate of the position to get, 0 to WORLD_W_8.
 * @param y Y coordinate of the position to get, 0 to WORLD_H_8.
 * @param coverage the value to set.
 * @note Off-map positions are ignored.
 * @todo Use world coordinates instead (use policeStationEffectMap.worldSet() instead).
 */
void Micropolis::setPoliceCoverage(int x, int y, int coverage)
{
    policeStationEffectMap.set(x, y, coverage);
}


/**
 * Get the address of the internal buffer containing the police coverage
 * map. This is used to enable the tile engine to access the police
 * coverage map directly.
 * @return Pointer to the start of the police coverage map buffer.
 */
void *Micropolis::getPoliceCoverageMapBuffer()
{
    return (void *)policeStationEffectMap.getBase();
}


////////////////////////////////////////////////////////////////////////
