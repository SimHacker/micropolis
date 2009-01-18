/* random.cpp
 *
 * Copyright (c) 1990 The Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *      This product includes software developed by the University of
 *      California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
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
