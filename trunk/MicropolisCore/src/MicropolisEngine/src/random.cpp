/* random.cpp
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

/** @file random.cpp Random number generator functions. */

/** @bug Code seems to assume that \c sizeof(short)==2 and \c sizeof(int)==4
 *       However, this depends on the compiler. We should introduce typedefs
 *       for them, and check correctness of our assumptions w.r.t. size of
 *       them (eg in Micropolis::randomlySeedRandom() or in
 *       Micropolis::Micropolis()).
 * @bug Code stores unsigned 16 bit numbers in \c short which is a signed type.
 */

////////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "micropolis.h"


////////////////////////////////////////////////////////////////////////


/**
 * Draw a random number (internal function).
 * @todo Use Wolfram's fast cellular automata pseudo random number generator.
 * @return Unsigned 16 bit random number.
 */
int Micropolis::simRandom()
{
    nextRandom = nextRandom * 1103515245 + 12345;
    return (nextRandom & 0xffff00) >> 8;
}


/**
 * Draw a random number in a given range.
 * @param range Upper bound of the range (inclusive).
 * @return Random number between \c 0 and \a range (inclusive).
 */
short Micropolis::getRandom(short range)
{
    int maxMultiple, rnum;

    range++; /// @bug Increment may cause range overflow.
    maxMultiple = 0xffff / range;
    maxMultiple *= range;

    do {
        rnum = getRandom16();
    } while (rnum >= maxMultiple);

    return (rnum % range);
}


/**
 * Get random 16 bit number.
 * @return Unsigned 16 bit random number.
 */
int Micropolis::getRandom16()
{
    return simRandom() & 0x0000ffff;
}


/** Get signed 16 bit random number. */
int Micropolis::getRandom16Signed()
{
    int i = getRandom16();

    if (i > 0x7fff) {
      i = 0x7fff - i;
    }

    return i;
}


/**
 * Get a random number within a given range, with a preference to smaller
 * values.
 * @param limit Upper bound of the range (inclusive).
 * @return Random number between \c 0 and \a limit (inclusive).
 */
short Micropolis::getERandom(short limit)
{
    short z = getRandom(limit);
    short x = getRandom(limit);

    return min(z, x);
}


/** Initialize the random number generator with a 'random' seed. */
void Micropolis::randomlySeedRandom()
{
#ifdef _WIN32
    seedRandom(::GetTickCount());
#else
    struct timeval time;
    gettimeofday(&time, NULL);
    seedRandom(time.tv_usec ^ time.tv_sec);
#endif
}


/**
 * Set seed of the random number generator.
 * @param seed New seed.
 */
void Micropolis::seedRandom(int seed)
{
    nextRandom = seed;
}


////////////////////////////////////////////////////////////////////////
