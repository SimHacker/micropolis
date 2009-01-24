/* text.h
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

/** @file text.h Identification numbers for texts. */

////////////////////////////////////////////////////////////////////////

/** String numbers of score card. */
enum Stri202 {
    STR202_POPULATIONDENSITY_LOW = 0, ///< Low
    STR202_POPULATIONDENSITY_MEDIUM = 1, ///< Medium
    STR202_POPULATIONDENSITY_HIGH = 2, ///< High
    STR202_POPULATIONDENSITY_VERYHIGH = 3, ///< Very High

    STR202_LANDVALUE_SLUM = 4, ///< Slum
    STR202_LANDVALUE_LOWER_CLASS = 5, ///< Lower Class
    STR202_LANDVALUE_MIDDLE_CLASS = 6, ///< Middle Class
    STR202_LANDVALUE_HIGH_CLASS = 7, ///< High

    STR202_CRIME_NONE = 8, ///< Safe
    STR202_CRIME_LIGHT = 9, ///< Light
    STR202_CRIME_MODERATE = 10, ///< Moderate
    STR202_CRIME_DANGEROUS = 11, ///< Dangerous

    STR202_POLLUTION_NONE = 12, ///< None
    STR202_POLLUTION_MODERATE = 13, ///< Moderate
    STR202_POLLUTION_HEAVY = 14, ///< Heavy
    STR202_POLLUTION_VERY_HEAVY = 15, ///< Very Heavy

    STR202_GROWRATE_DECLINING = 16, ///< Declining
    STR202_GROWRATE_STABLE = 17, ///< Stable
    STR202_GROWRATE_SLOWGROWTH = 18, ///< Slow Growth
    STR202_GROWRATE_FASTGROWTH = 19, ///< Fast Growth
};

/** String numbers of messages. */
enum MessageNumber {
    MESSAGE_NEED_MORE_RESIDENTIAL = 1, ///< More residential zones needed.
    MESSAGE_NEED_MORE_COMMERCIAL, ///< More commercial zones needed.
    MESSAGE_NEED_MORE_INDUSTRIAL, ///< More industrial zones needed.
    MESSAGE_NEED_MORE_ROADS, ///< More roads required.
    MESSAGE_NEED_MORE_RAILS, ///< 5: Inadequate rail system.
    MESSAGE_NEED_ELECTRICITY, ///< Build a Power Plant.
    MESSAGE_NEED_STADIUM, ///< Residents demand a Stadium.
    MESSAGE_NEED_SEAPORT, ///< Industry requires a Sea Port.
    MESSAGE_NEED_AIRPORT, ///< Commerce requires an Airport.
    MESSAGE_HIGH_POLLUTION, ///< 10: Pollution very high.
    MESSAGE_HIGH_CRIME, ///< Crime very high.
    MESSAGE_TRAFFIC_JAMS, ///< Frequent traffic jams reported.
    MESSAGE_NEED_FIRE_STATION, ///< Citizens demand a Fire Department.
    MESSAGE_NEED_POLICE_STATION, ///< Citizens demand a Police Department.
    MESSAGE_BLACKOUTS_REPORTED, ///< 15: Blackouts reported. Check power map.
    MESSAGE_TAX_TOO_HIGH, ///< Citizens upset. The tax rate is too high.
    MESSAGE_ROAD_NEEDS_FUNDING, ///< Roads deteriorating, due to lack of funds.
    MESSAGE_FIRE_STATION_NEEDS_FUNDING, ///< Fire departments need funding.
    MESSAGE_POLICE_NEEDS_FUNDING, ///< Police departments need funding.
    MESSAGE_FIRE_REPORTED, ///< 20: Fire reported !
    MESSAGE_MONSTER_SIGHTED, ///< A Monster has been sighted !!
    MESSAGE_TORNADO_SIGHTED, ///< Tornado reported !!
    MESSAGE_EARTHQUAKE, ///< Major earthquake reported !!!
    MESSAGE_PLANE_CRASHED, ///< A plane has crashed !
    MESSAGE_SHIP_CRASHED, ///< 25: Shipwreck reported !
    MESSAGE_TRAIN_CRASHED, ///< A train crashed !
    MESSAGE_HELICOPTER_CRASHED, ///< A helicopter crashed !
    MESSAGE_HIGH_UNEMPLOYMENT, ///< Unemployment rate is high.
    MESSAGE_NO_MONEY, ///< YOUR CITY HAS GONE BROKE!
    MESSAGE_FIREBOMBING, ///< 30: Firebombing reported !
    MESSAGE_NEED_MORE_PARKS, ///< Need more parks.
    MESSAGE_EXPLOSION_REPORTED, ///< Explosion detected !
    MESSAGE_NOT_ENOUGH_FUNDS, ///< Insufficient funds to build that.
    MESSAGE_BULLDOZE_AREA_FIRST, ///< Area must be bulldozed first.
    MESSAGE_REACHED_TOWN, ///< 35: Population has reached 2,000.
    MESSAGE_REACHED_CITY, ///< Population has reached 10,000.
    MESSAGE_REACHED_CAPITAL, ///< Population has reached 50,000.
    MESSAGE_REACHED_METROPOLIS, ///< Population has reached 100,000.
    MESSAGE_REACHED_MEGALOPOLIS, ///< Population has reached 500,000.
    MESSAGE_NOT_ENOUGH_POWER, ///< 40: Brownouts, build another Power Plant.
    MESSAGE_HEAVY_TRAFFIC, ///< Heavy Traffic reported.
    MESSAGE_FLOODING_REPORTED, ///< Flooding reported !!
    MESSAGE_NUCLEAR_MELTDOWN, ///< A Nuclear Meltdown has occurred !!!
    MESSAGE_RIOTS_REPORTED, ///< They're rioting in the streets !!
    MESSAGE_STARTED_NEW_CITY, ///< 45: Started a New City.
    MESSAGE_LOADED_SAVED_CITY, ///< Restored a Saved City.
    MESSAGE_SCENARIO_WON,  ///< You won the scenario
    MESSAGE_SCENARIO_LOST, ///< You lose the scenario
    MESSAGE_ABOUT_MICROPOLIS, ///< About micropolis.
    MESSAGE_SCENARIO_DULLSVILLE, ///< 50: Dullsville scenario.
    MESSAGE_SCENARIO_SAN_FRANCISCO, ///< San Francisco scenario.
    MESSAGE_SCENARIO_HAMBURG, ///< Hamburg scenario.
    MESSAGE_SCENARIO_BERN, ///< Bern scenario.
    MESSAGE_SCENARIO_TOKYO, ///< Tokyo scenario.
    MESSAGE_SCENARIO_DETROIT, ///< 55: Detroit scenario.
    MESSAGE_SCENARIO_BOSTON, ///< Boston scenario.
    MESSAGE_SCENARIO_RIO_DE_JANEIRO, ///< 57: Rio de Janeiro scenario.
 
    MESSAGE_LAST = 57, ///< Last valid message
};
