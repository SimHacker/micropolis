/* main.cpp
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


////////////////////////////////////////////////////////////////////////


#include "stdafx.h"


////////////////////////////////////////////////////////////////////////



#define COPY(FROM, TO) \
  TO = (char *)NewPtr((int)strlen(FROM) + 1); \
  strcpy(TO, FROM);


#define TESTDIR(DIR, NAME) \
  if ((stat(DIR, &statbuf) == -1) || \
      !(S_ISDIR(statbuf.st_mode))) { \
    fprintf(stderr, \
        "Can't find the directory \"%s\"!\n", DIR); \
    fprintf(stderr, \
        "The environment variable \"%s\" should name a directory.\n", \
        NAME); \
    lost = 1; \
  }


void Micropolis::env_init()
{
  char dir[4096];
  char *s;
  struct stat statbuf;
  int lost = 0;

  if ((s = getenv("SIMHOME")) == NULL) {
    s = ".";
  }
  COPY(s, HomeDir);
  TESTDIR(HomeDir, "$SIMHOME");

  sprintf(dir, "%s/res/", HomeDir);
  COPY(dir, ResourceDir);
  TESTDIR(ResourceDir, "$SIMHOME/res");

  if (lost) {
    fprintf(stderr,
        "Please check the environment or reinstall Micropolis and try again! Sorry!\n");
    // TODO: Tell application to exit.
  }

}


void Micropolis::sim_init()
{
  UserSoundOn = 1;
  MustUpdateOptions = 1;
  HaveLastMessage = 0;
  ScenarioID = 0;
  StartingYear = 1900;
  sim_skips = sim_skip = 0;
  autoGo = 1;
  CityTax = 7;
  CityTime = 50;
  NoDisasters = 0;
  PunishCnt = 0;
  autoBulldoze = 1;
  autoBudget = 1;
  MesNum = 0;
  LastMesTime = 0;
  flagBlink = 1;
  SimSpeed = 3;
  ChangeEval();
  MessagePort = 0;
  MesX = 0;
  MesY = 0;
  sim_paused = 0;
  sim_loops = 0;
  InitSimLoad = 2;

  InitializeSound();
  initMapArrays();
  initGraphs();
  InitFundingLevel();
  StopEarthquake();
  ResetMapState();
  ResetEditorState();
  ClearMap();
  InitWillStuff();
  SetFunds(5000);
  SetGameLevelFunds(0);
  setSpeed(0);
  setSkips(0);
}


void Micropolis::sim_update()
{
  flagBlink = 
    ((TickCount() % 60) < 30) ? 1 : -1;

  if (SimSpeed && !heat_steps) {
    TilesAnimated = 0;
  }

  DoUpdateHeads();
  graphDoer();
  UpdateBudgetWindow();
  scoreDoer();
}


short *CellSrc = NULL;
short *CellDst = NULL;

#define SRCCOL (WORLD_Y + 2)
#define DSTCOL WORLD_Y

#define CLIPPER_LOOP_BODY(CODE) \
    src = CellSrc; dst = CellDst; \
    for (x = 0; x < WORLD_X;) { \
      short nw, n, ne, w, c, e, sw, s, se; \
      \
      src = CellSrc + (x * SRCCOL); dst = CellDst + (x * DSTCOL); \
      w = src[0]; c = src[SRCCOL]; e = src[2 * SRCCOL]; \
      sw = src[1]; s = src[SRCCOL + 1]; se = src[(2 * SRCCOL) + 1]; \
      \
      for (y = 0; y < WORLD_Y; y++) { \
        nw = w; w = sw; sw = src[2]; \
    n = c; c = s; s = src[SRCCOL + 2]; \
    ne = e; e = se; se = src[(2 * SRCCOL) + 2]; \
    { CODE } \
    src++; dst++; \
      } \
      x++; /* src += SRCCOL - 3; dst += DSTCOL - 1; */ \
      src = CellSrc + ((x + 1) * SRCCOL) - 3; dst = CellDst + ((x + 1) * DSTCOL) - 1; \
      \
      nw = src[1]; n = src[SRCCOL + 1]; ne = src[(2 * SRCCOL) + 1]; \
      w = src[2]; c = src[SRCCOL + 2]; e = src[(2 * SRCCOL) + 2]; \
      \
      for (y = WORLD_Y - 1; y >= 0; y--) { \
        sw = w; w = nw; nw = src[0]; \
        s = c; c = n; n = src[SRCCOL]; \
        se = e; e = ne; ne = src[2 * SRCCOL]; \
    { CODE } \
    src--; dst--; \
      } \
      x++; /* src += SRCCOL + 3; dst += DSTCOL + 1; */ \
    }


void Micropolis::sim_heat()
{
  int x, y;
  static int a = 0;
  short *src, *dst;
  register int fl = heat_flow;

  if (CellSrc == NULL) {
    CellSrc = (short *)NewPtr((WORLD_X + 2) * (WORLD_Y + 2) * sizeof (short));
    CellDst = &Map[0][0];
  }

  src = CellSrc + SRCCOL + 1;
  dst = CellDst;

/*
 * Copy wrapping edges:
 *
 *  0   ff  f0 f1 ... fe ff     f0
 *
 *  1   0f  00 01 ... 0e 0f     00
 *  2   1f  10 11 ... 1e 1f     10
 *      ..  .. ..     .. ..     ..
 *      ef  e0 e1 ... ee ef     e0
 *  h   ff  f0 f1 ... fe ff     f0
 *
 *  h+1 0f  00 01 ... 0e 0f     00
 *
 * wrap value:  effect:
 *  0   no effect
 *  1   copy future=>past, no wrap
 *  2   no copy, wrap edges
 *  3   copy future=>past, wrap edges
 *  4   copy future=>past, same edges
 */

  switch (heat_wrap) {
  case 0:
    break;
  case 1:
    for (x = 0; x < WORLD_X; x++) {
      memcpy(src, dst, WORLD_Y * sizeof (short));
      src += SRCCOL;
      dst += DSTCOL;
    }
    break;
  case 2:
    for (x = 0; x < WORLD_X; x++) {
      src[-1] = src[WORLD_Y - 1];
      src[WORLD_Y] = src[0];
      src += SRCCOL;
      dst += DSTCOL;
    }
    memcpy(CellSrc,CellSrc + (SRCCOL * WORLD_X), 
      SRCCOL * sizeof (short));
    memcpy(CellSrc + SRCCOL * (WORLD_X + 1), CellSrc + SRCCOL,
      SRCCOL * sizeof (short));
    break;
  case 3:
    for (x = 0; x < WORLD_X; x++) {
      memcpy(src, dst, WORLD_Y * sizeof (short));
      src[-1] = src[WORLD_Y - 1];
      src[WORLD_Y] = src[0];
      src += SRCCOL;
      dst += DSTCOL;
    }
    memcpy(CellSrc, CellSrc + (SRCCOL * WORLD_X),
       SRCCOL * sizeof (short));
    memcpy(CellSrc + SRCCOL * (WORLD_X + 1), CellSrc + SRCCOL,
       SRCCOL * sizeof (short));
    break;
  case 4:
    src[0] = dst[0];
    src[1 + WORLD_Y] = dst[WORLD_Y - 1];
    src[(1 + WORLD_X) * SRCCOL] = dst[(WORLD_X - 1) * DSTCOL];
    src[((2 + WORLD_X) * SRCCOL) - 1] = dst[(WORLD_X * WORLD_Y) - 1];
    for (x = 0; x < WORLD_X; x++) {
      memcpy(src, dst, WORLD_Y * sizeof (short));
      src[-1] = src[0];
      src[WORLD_Y] =  src[WORLD_Y - 1];
      src += SRCCOL;
      dst += DSTCOL;
    }
    memcpy(CellSrc + (SRCCOL * (WORLD_X + 1)), CellSrc + (SRCCOL * WORLD_X),
       SRCCOL * sizeof (short));
    memcpy(CellSrc, CellSrc + SRCCOL,
       SRCCOL * sizeof (short));
    break;
  }


  switch (heat_rule) {

  case 0:
#define HEAT \
    a += nw + n + ne + w + e + sw + s + se + fl; \
    dst[0] = ((a >> 3) & LOMASK) | \
             (ANIMBIT | BURNBIT | BULLBIT); \
    a &= 7;

    CLIPPER_LOOP_BODY(HEAT);
    break;

  case 1:
#define ECOMASK 0x3fc
#define ECO \
      c -= fl; n -= fl; s -= fl; e -= fl; w -= fl; \
      ne -= fl; nw -= fl; se -= fl; sw -= fl; \
      \
      /* anneal */ \
      { int sum = (c&1) + (n&1) + (s&1) + (e&1) + (w&1) + \
          (ne&1) + (nw&1) + (se&1) + (sw&1), cell; \
    if (((sum > 5) || (sum == 4))) { \
      /* brian's brain */ \
      cell = ((c <<1) & (0x3fc)) | \
         (((((c >>1)&3) == 0) && \
           (((n&2) + (s&2) + (e&2) + (w&2) + \
             (ne&2) + (nw&2) + (se&2) + (sw&2)) == (2 <<1)) \
          ) ? 2 : 0) | \
         1; \
    } else { \
      /* anti-life */ \
      sum = ((n&2) + (s&2) + (e&2) + (w&2) + \
         (ne&2) + (nw&2) + (se&2) + (sw&2)) >>1; \
      cell = (((c ^ 2) <<1) & ECOMASK) | \
         ((c&2) ? ((sum != 5) ? 2 : 0) \
            : (((sum != 5) && (sum != 6)) ? 2 : 0)); \
    } \
    dst[0] = ((fl + cell) & LOMASK) | \
         (ANIMBIT | BURNBIT | BULLBIT); \
      } \
      c += fl; n += fl; s += fl; e += fl; w += fl; \
      ne += fl; nw += fl; se += fl; sw += fl;

    CLIPPER_LOOP_BODY(ECO);
    break;
  }
}


void Micropolis::sim_loop(
  int doSim)
{
  if (heat_steps) {
    int j;

    for (j = 0; j < heat_steps; j++) {
      sim_heat();
    }

    MoveObjects();

    NewMap = 1;

  } else {
    if (doSim) {
      SimFrame();
    }

    MoveObjects();
  }

  sim_loops++;
  sim_update();
}


void Micropolis::sim_tick()
{
  if (SimSpeed) {
    int i;
	for (i = 0; i < sim_skips; i++) {
	  sim_loop(1);
	}
  }
}


////////////////////////////////////////////////////////////////////////
