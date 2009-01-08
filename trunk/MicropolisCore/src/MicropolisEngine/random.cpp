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

/** @file random.cpp */

////////////////////////////////////////////////////////////////////////


#include "stdafx.h"


////////////////////////////////////////////////////////////////////////


int Micropolis::sim_rand()
{
  // TODO: Use Wolfram's fast cellular automata pseudo random number generator.

  nextRandom = nextRandom * 1103515245 + 12345;
  return ((nextRandom % (0x10000 <<8)) >>8);
}


void Micropolis::sim_srand(UQuad seed)
{
  nextRandom = seed;
}


short Micropolis::Rand(short range)
{
  int maxMultiple, rnum;

  range++;
  maxMultiple = RANDOM_RANGE / range;
  maxMultiple *= range;

  while ((rnum = Rand16()) >= maxMultiple) ;

  return (rnum % range);
}


int Micropolis::Rand16()
{
  return sim_rand() & 0x0000ffff;
}


int Micropolis::Rand16Signed()
{
  int i =
          Rand16();

  if (i > 0x7fff) {
    i = 0x7fff - i;
  }

  return i;
}


short Micropolis::ERand(short limit)
{
  short x, z;

  z = Rand(limit);
  x = Rand(limit);
  if (z < x) {
    return (z);
  }
  return (x);
}


void Micropolis::RandomlySeedRand()
{
#ifdef _WIN32
  SeedRand(::GetTickCount());
#else
  struct timeval time;
  gettimeofday(&time, NULL);
  SeedRand(time.tv_usec ^ time.tv_sec);
#endif
}


void Micropolis::SeedRand(int seed)
{
  sim_srand(seed);
}


////////////////////////////////////////////////////////////////////////
