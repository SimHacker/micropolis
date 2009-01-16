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

/** @file main.cpp */

////////////////////////////////////////////////////////////////////////


#include "stdafx.h"


////////////////////////////////////////////////////////////////////////


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


#define SRCCOL (WORLD_H + 2)
#define DSTCOL WORLD_H

#define CLIPPER_LOOP_BODY(CODE) \
    src = cellSrc; dst = cellDst; \
    for (x = 0; x < WORLD_W;) { \
        short nw, n, ne, w, c, e, sw, s, se; \
        src = cellSrc + (x * SRCCOL); \
        dst = cellDst + (x * DSTCOL); \
        w = src[0]; c = src[SRCCOL]; e = src[2 * SRCCOL]; \
        sw = src[1]; s = src[SRCCOL + 1]; se = src[(2 * SRCCOL) + 1]; \
        for (y = 0; y < WORLD_H; y++) { \
            nw = w; w = sw; sw = src[2]; \
            n = c; c = s; s = src[SRCCOL + 2]; \
            ne = e; e = se; se = src[(2 * SRCCOL) + 2]; \
            { CODE } \
            src++; dst++; \
        } \
        x++; \
        src = cellSrc + ((x + 1) * SRCCOL) - 3; \
        dst = cellDst + ((x + 1) * DSTCOL) - 1; \
        nw = src[1]; n = src[SRCCOL + 1]; ne = src[(2 * SRCCOL) + 1]; \
        w = src[2]; c = src[SRCCOL + 2]; e = src[(2 * SRCCOL) + 2]; \
        for (y = WORLD_H - 1; y >= 0; y--) { \
            sw = w; w = nw; nw = src[0]; \
            s = c; c = n; n = src[SRCCOL]; \
            se = e; e = ne; ne = src[2 * SRCCOL]; \
            { CODE } \
            src--; dst--; \
        } \
        x++; \
    }


////////////////////////////////////////////////////////////////////////


const char *Micropolis::getMicropolisVersion()
{
    return MICROPOLIS_VERSION;
}


void Micropolis::environmentInit()
{
    char *s;
    struct stat statbuf;
    int lost = 0;

    if ((s = getenv("SIMHOME")) == NULL) {
        s = ".";
    }
    homeDir = s;
    TESTDIR(homeDir.c_str(), "$SIMHOME");

    resourceDir = homeDir;
    resourceDir += "/res/";
    TESTDIR(resourceDir.c_str(), "$SIMHOME/res");

    if (lost) {
        fprintf(stderr,
            "Please check the environment or reinstall Micropolis and try again! Sorry!\n");
        // TODO: Tell application to exit.
    }

}


void Micropolis::simInit()
{
    enableSound = true; // Enable sound
    mustUpdateOptions = 1;
    messageLastValid = false; // No message seen yet
    scenario = SC_NONE;
    startingYear = 1900;
    simSkips = simSkip = 0;
    autoGoto = true;  // Enable auto-goto
    cityTax = 7;
    cityTime = 50;
    enableDisasters = true; // Enable disasters
    autoBulldoze = true; // Enable auto bulldoze
    autoBudget   = true; // Enable auto-budget
    messageNumber = 0;
    messageTimeLast = 0;
    blinkFlag = 1;
    simSpeed = 3;
    changeEval();
    messagePort = 0;
    messageX = -1;
    messageY = -1;
    simPaused = false; // Simulation is running
    simLoops = 0;
    initSimLoad = 2;

    initializeSound();
    initMapArrays();
    initGraphs();
    initFundingLevel();
    resetMapState();
    resetEditorState();
    clearMap();
    initWillStuff();
    setFunds(5000);
    setGameLevelFunds(LEVEL_EASY);
    setSpeed(0);
    setSkips(0);
}


void Micropolis::simUpdate()
{
    blinkFlag = ((tickCount() % 60) < 30) ? 1 : -1;

    if (simSpeed && !heatSteps) {
      tilesAnimated = 0;
    }

    doUpdateHeads();
    graphDoer();
    updateBudgetWindow();
    scoreDoer();
}


void Micropolis::simHeat()
{
    int x, y;
    static int a = 0;
    short *src, *dst;
    register int fl = heatFlow;

    if (cellSrc == NULL) {
        cellSrc = (short *)newPtr((WORLD_W + 2) * (WORLD_H + 2) * sizeof (short));
        cellDst = &map[0][0];
    }

    src = cellSrc + SRCCOL + 1;
    dst = cellDst;

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

    switch (heatWrap) {
        case 0:
            break;
        case 1:
            for (x = 0; x < WORLD_W; x++) {
                memcpy(src, dst, WORLD_H * sizeof (short));
                src += SRCCOL;
                dst += DSTCOL;
            }
            break;
        case 2:
            for (x = 0; x < WORLD_W; x++) {
                src[-1] = src[WORLD_H - 1];
                src[WORLD_H] = src[0];
                src += SRCCOL;
                dst += DSTCOL;
            }
            memcpy(
                cellSrc,
                cellSrc + (SRCCOL * WORLD_W),
                SRCCOL * sizeof (short));
            memcpy(
                cellSrc + SRCCOL * (WORLD_W + 1),
                cellSrc + SRCCOL,
                SRCCOL * sizeof (short));
            break;
        case 3:
            for (x = 0; x < WORLD_W; x++) {
                memcpy(src, dst, WORLD_H * sizeof (short));
                src[-1] = src[WORLD_H - 1];
                src[WORLD_H] = src[0];
                src += SRCCOL;
                dst += DSTCOL;
            }
            memcpy(
                cellSrc,
                cellSrc + (SRCCOL * WORLD_W),
                SRCCOL * sizeof (short));
            memcpy(
                cellSrc + SRCCOL * (WORLD_W + 1),
                cellSrc + SRCCOL,
                SRCCOL * sizeof (short));
            break;
        case 4:
            src[0] =
                dst[0];
            src[1 + WORLD_H] =
                dst[WORLD_H - 1];
            src[(1 + WORLD_W) * SRCCOL] =
                dst[(WORLD_W - 1) * DSTCOL];
            src[((2 + WORLD_W) * SRCCOL) - 1] =
                dst[(WORLD_W * WORLD_H) - 1];
            for (x = 0; x < WORLD_W; x++) {
                memcpy(src, dst, WORLD_H * sizeof (short));
                src[-1] = src[0];
                src[WORLD_H] =  src[WORLD_H - 1];
                src += SRCCOL;
                dst += DSTCOL;
            }
            memcpy(
                cellSrc + (SRCCOL * (WORLD_W + 1)),
                cellSrc + (SRCCOL * WORLD_W),
                SRCCOL * sizeof (short));
            memcpy(
                cellSrc,
                cellSrc + SRCCOL,
                SRCCOL * sizeof (short));
            break;
        default:
            NOT_REACHED();
            break;
    }

    switch (heatRule) {

      case 0:

#define HEAT \
    a += nw + n + ne + w + e + sw + s + se + fl; \
    dst[0] = ((a >> 3) & LOMASK) | ANIMBIT | BURNBIT | BULLBIT; \
    a &= 7;

        CLIPPER_LOOP_BODY(HEAT);
        break;

      case 1:

#define ECOMASK 0x3fc
#define ECO \
    { \
        c -= fl; n -= fl; s -= fl; e -= fl; w -= fl; \
        ne -= fl; nw -= fl; se -= fl; sw -= fl; \
        /* anneal */ \
        int sum = \
            (c&1) + (n&1) + (s&1) + (e&1) + (w&1) + \
            (ne&1) + (nw&1) + (se&1) + (sw&1), cell; \
        if (((sum > 5) || (sum == 4))) { \
            /* brian's brain */ \
            cell = \
                ((c <<1) & (0x3fc)) | \
                (((((c >>1)&3) == 0) && \
                  (((n&2) + (s&2) + (e&2) + (w&2) + \
                    (ne&2) + (nw&2) + (se&2) + (sw&2)) == (2 <<1))      \
                 ) ? 2 : 0) | \
                 1; \
        } else { \
            /* anti-life */ \
            sum = \
                ((n&2) + (s&2) + (e&2) + (w&2) + \
                 (ne&2) + (nw&2) + (se&2) + (sw&2)) >>1; \
            cell = \
                (((c ^ 2) <<1) & ECOMASK) | \
                ((c&2) \
                   ? ((sum != 5) ? 2 : 0) \
                   : (((sum != 5) && (sum != 6)) ? 2 : 0)); \
        } \
        dst[0] = \
            ((fl + cell) & LOMASK) | ANIMBIT | BURNBIT | BULLBIT; \
        c += fl; n += fl; s += fl; e += fl; w += fl; \
        ne += fl; nw += fl; se += fl; sw += fl; \
    }

        CLIPPER_LOOP_BODY(ECO);

        break;

        default:
            NOT_REACHED();
            break;
    }
}


void Micropolis::simLoop(int doSim)
{
   if (heatSteps) {
       int j;

       for (j = 0; j < heatSteps; j++) {
           simHeat();
       }

       moveObjects();

       newMap = 1;

   } else {
     if (doSim) {
         simFrame();
     }

     moveObjects();
   }

   simLoops++;
   simUpdate();
}


void Micropolis::simTick()
{
    if (simSpeed) {
        int i;
        for (i = 0; i < simSkips; i++) {
            simLoop(1);
        }
    }
}


////////////////////////////////////////////////////////////////////////
