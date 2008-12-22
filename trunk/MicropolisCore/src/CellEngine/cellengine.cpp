/* cellengine.cpp
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


////////////////////////////////////////////////////////////////////////
/** @file cellengine.cpp
 * Cellular Automata Machine Engine
 * @author Don Hopkins
 */


#include "stdafx.h"


////////////////////////////////////////////////////////////////////////


/**
 * @todo generalized parameterized heat diffusion
 * @todo bit mask of neighbors to add (9 bits)
 * @todo shift count (power of two division)
 * @todo frob offset (overall heat gain)
 * @todo pass in different masks and shifts to implement symetric 8 and 4
 * @todo neighbor heats, as well as vertical, horizontal, diagonal, and
 * @todo asymetric heat flow fields
 * @todo logical operations
 * @todo add pixels
 * @todo map pixels
 */


////////////////////////////////////////////////////////////////////////


extern const char *ruleTables[];
static int ruleCount = -1;


////////////////////////////////////////////////////////////////////////


#define RANDOMS_COUNT 256

static long randomsPrimed = 0;
static long randoms[RANDOMS_COUNT];
static long randomRead = 0;
static long randomWrite = 0;


void PrimeRandoms()
{
    if (randomsPrimed) {
        return;
    } // if

    randomsPrimed = 1;

    long i;
    for (i = 0; i < RANDOMS_COUNT; i++) {
        randoms[i] ^=
#ifdef MACOS
            Random() ^
            (Random() >> 4) ^
            (Random() << 4);
#else
            rand() ^
            (rand() >> 4) ^
            (rand() >> 8) ^
            (rand() << 4);
#endif
    }

    randomRead = 0;
    randomWrite = 0;
}


void FeedRandom(
    long food)
{
    randomWrite--;
    if (randomWrite < 0) {
        randomWrite = RANDOMS_COUNT - 1;
    }
    randoms[randomWrite] ^= food;
}


long Rand32()
{
    PrimeRandoms();

    randomRead++;
    if (randomRead >= RANDOMS_COUNT) {
        randomRead = 0;
    }

    long result = randoms[randomRead];

    // TODO: make a better hasher
#ifdef MACOS
    randoms[randomRead] ^= Random();
#else
    int r = rand() ^ (rand() << 8);
    randoms[randomRead] ^= r;
#endif

    return result;
}


long Rand16()
{
    return Rand32() & 0xffff;
}


long Rand8()
{
    return Rand32() & 0xff;
}


////////////////////////////////////////////////////////////////////////


#ifdef DOSOUND

class SoundEvent {
    long x;
    long y;
    long w;
    long h;
    long s;
    static std::vector<SoundEvent> queue;

public:

    SoundEvent(
        long xx,
        long yy,
        long ww,
        long hh,
        long ss) :
        x(xx),
        y(yy),
        w(ww),
        h(hh),
        s(ss)
    {
    }

    SoundEvent();

    ~SoundEvent()
    {
    }

    static void Queue(
        long xx,
        long yy,
        long ww,
        long hh,
        long ss)
    {
        queue.push_back(
            SoundEvent(
               xx,
               yy,
               ww,
               hh,
               ss));
    }

    static void ClearQueue()
    {
        queue.erase(
            queue.begin(),
            queue.end());
    }

    void Play(float vol)
    {
        float speed =
            (float)0.2 +
            (((float)2.0 *
              (float)(h - y) /
              (float)h));
        float pan = ((float)2.0 *
                     (float)x /
                     (float)w) - (float)1.0;
#ifdef TODO
        PlayWave(
            s, // wave#
            0, // looping
            speed,
            vol,
            pan);
#endif
    }

    static void PlayReverse(float vol)
    {
        int count = queue.size();
        int i = count - 1;
        while (i >= 0) {
            long r = Rand8();
            if ((count < 10) ||
                ((r = Rand8()) < 50)) {
                if ((count < 50) ||
                    (r < 10)) {
                    queue[i].Play(vol);
                } // if
            } // if

            i--;
        } // while

    }

    static void PlayQueue()
    {
        if (queue.size() > 0) {
            float vol = (float)0.75 +
                        ((float)0.25 *
                         (float)sqrt((float)1.0 / queue.size()));
            PlayReverse(vol);
            ClearQueue();
        }
    }
};

std::vector<SoundEvent> SoundEvent::queue;

#endif


////////////////////////////////////////////////////////////////////////


CellEngine::CellEngine()
{
    Init();
}


void CellEngine::Init()
{
    name = "";
    screenMem = NULL;
    screenWidth = 0;
    screenHeight = 0;
    screenRowBytes = 0;
    backMem = NULL;
    backSize = 0;
    backRowBytes = 0;
    backWidth = 0;
    backHeight = 0;
    backBorder = 2;
    frontMem = NULL;
    maskMem = NULL;
    maskRowBytes = 0;
    maskWidth = 0;
    maskHeight = 0;
    masked = 0;
    neighborhood = 33; // garble
    rule = NULL;
    ruleStatic = 0;
    ruleSize = 0;
    ruleName = NULL;
    x = 0;
    y = 0;
    dx = 0;
    dy = 0;
    width = 0;
    height = 0;
    idealWidth = 0;
    idealHeight = 0;
    phase = 0;
    ticks = 0;
    wrap = 5;
    steps = 1;
    frob = -1;
    rumble = 0;
    rumblemax = 0;
    hubba = 0;
    inflation = 0;
    high = 255;
    low = 0;
    maskClip = 0;
    moveImage = 0;
    numbera = 0;
    numberb = 0;
    numberc = 0;
    anglea = 0;
    angleb = 0;
    anglec = 0;
    pointax = 0;
    pointay = 0;
    pointbx = 0;
    pointby = 0;
    pointcx = 0;
    pointcy = 0;
    clipRects = 0;
    rectList = NULL;
    skips = 0;
    skip = 0;
    total = 0;
    tracking = 0;
    action = 8 + 2;
    downx = 0;
    downy = 0;
    grabx = 0;
    graby = 0;
    lastx = 0;
    lasty = 0;
    saveskips = 0;
    still = 0;
    gravx = 0;
    gravy = 0;
    grabbable = 1;
    data = NULL;

    SetSize(16, 16);
}


CellEngine::~CellEngine()
{
    if (backMem != NULL) {
        delete [] backMem;
        backMem = NULL;
    }

#if 0
    if (maskMem != NULL) {
        delete [] maskMem;
        maskMem = NULL;
    }
#endif

    if ((rule != NULL) &&
        (!ruleStatic)) {
        delete [] rule;
        rule = NULL;
    }

    if (ruleName != NULL) {
        delete [] ruleName;
        ruleName = NULL;
    }

    if (rectList != NULL) {
        delete [] rectList;
        rectList = NULL;
    }

    if (screenMem != NULL) {
        delete [] screenMem;
        screenMem = NULL;
    }

}


void CellEngine::InitScreen(
    long ww,
    long hh)
{
    if (screenMem != NULL) {
        delete [] screenMem;
    }

    screenWidth = ww;
    screenHeight = hh;
    screenRowBytes = (ww + 3) & ~3;
    screenMem = new Byte[screenRowBytes * screenHeight];

    SetPos(0, 0);
}


void CellEngine::SetRect(
    long xx,
    long yy,
    long ww,
    long hh)
{
    long w2;
    long h2;
    long rb;
    long size;

    idealWidth = ww;
    idealHeight = hh;
    ww = (ww + 3) & ~3;

    w2 = backBorder + ww + backBorder;
    h2 = backBorder + hh + backBorder;
    rb = (w2 + 3) & (~3);
    size = (rb * h2);

    if ((backMem == NULL) ||
        (backSize < size)) {

        if (backMem != NULL) {
            delete [] backMem;
        }

        // inflate the size a bit if this isn't the first allocation
        if (backSize != 0) {
            size = (12 * size) / 10;
        }

        backSize = size;
        backMem = new Byte[size];
    }

    width = ww;
    height = hh;
    backWidth = w2;
    backHeight = h2;
    backRowBytes = rb;
    SetPos(xx, yy);
}


void CellEngine::SetPos(
    long xx,
    long yy)
{
    x = xx;
    y = yy;
    frontMem =
        screenMem +
        xx +
        (screenRowBytes * yy);
}


void CellEngine::SetSize(
    long ww,
    long hh)
{
    SetRect(x, y, ww, hh);
}


void CellEngine::ForceOnScreen()
{
    int hangout;
    int xx = x;
    int yy = y;
    int ww = idealWidth;
    int hh = idealHeight;
    int ww2 = width;
    int hh2 = height;

    hangout = (xx + ww2) - screenWidth;
    if (hangout > 0) {
        xx -= hangout;
    }
    hangout = (yy + hh2) - screenHeight;
    if (hangout > 0) {
        yy -= hangout;
    }
    hangout = -xx;
    if (hangout > 0) {
        xx += hangout;
    }
    hangout = -yy;
    if (hangout > 0) {
        yy -= hangout;
    }
    if ((xx + ww2) > screenWidth) {
        ww = screenWidth - xx;
    }
    if ((yy + hh2) > screenHeight) {
        hh = screenHeight - yy;
    }

    SetRect(xx, yy, ww, hh);
}


long CellEngine::OnScreen()
{
    return ((screenMem != NULL) &&
            (x >= 0) &&
            (y >= 0) &&
            ((x + width) <= screenWidth) &&
            ((y + height) <= screenHeight));
}


void CellEngine::Garble()
{

    if (!OnScreen()) {
        return;
    }

    long xx, yy;
    Byte *image = frontMem;

    for (yy = 0;
         yy < height;
         yy++, image += screenRowBytes) {
        for (xx = 0;
             xx < width;
             xx++) {
            image[xx] ^= (char)(Rand16() >> 4);
        }
    }
}


void CellEngine::GarbleRect(
    long xx,
    long yy,
    long ww,
    long hh)
{
    Byte *image;
    long x0, y0, x1, y1;

    if (!OnScreen()) {
        return;
    }

    if (xx < 0) x0 = 0; else x0 = xx;
    if (yy < 0) y0 = 0; else y0 = yy;
    if (xx + ww > width) x1 = width; else x1 = xx + ww;
    if (yy + hh > height) y1 = height; else y1 = yy + ww;

    if ((x0 < x1) && (y0 < y1)) {
        image = frontMem;

        for (yy = y0;
             yy < y1;
             yy++, image += screenRowBytes) {
            for (xx = x0;
                 xx < x1;
                 xx++) {
                image[xx] ^= (char)(Rand16() >> 4);
            }
        }
    }
}


void CellEngine::Fill(
    Byte c)
{
    if (!OnScreen()) {
        return;
    }

    Byte *image = frontMem;

    long yy;
    for (yy = 0;
         yy < height;
         yy++, image += screenRowBytes) {
        long xx;
        for (xx = 0;
             xx < width;
             xx++) {
            image[xx] = c;
        }
    }
}


void CellEngine::FillRect(
    Byte c,
    long xx,
    long yy,
    long ww,
    long hh)
{
    Byte *image;
    long x0, y0, x1, y1;

    if (!OnScreen()) {
        return;
    }

    if (xx < 0) x0 = 0; else x0 = xx;
    if (yy < 0) y0 = 0; else y0 = yy;
    if (xx + ww > width) x1 = width; else x1 = xx + ww;
    if (yy + hh > height) y1 = height; else y1 = yy + ww;

    if ((x0 < x1) && (y0 < y1)) {
        image = frontMem;

        for (yy = y0;
             yy < y1;
             yy++, image += screenRowBytes) {
            for (xx = x0;
                 xx < x1;
                 xx++) {
                image[xx] = c;
            }
        }
    }
}


void CellEngine::ResetMask()
{
    if (masked) {
        if (maskMem != NULL) {
            maskMem = NULL;
        } // if
        maskWidth = 0;
        maskHeight = 0;
        maskRowBytes = 0;
    } // if
    masked = 0;
    UpdateClip();
}


void CellEngine::SetMask(
    long ww,
    long hh,
    Byte *data,
    long rb)
{
    maskMem = data;
    maskRowBytes = rb;
    maskWidth = ww;
    maskHeight = hh;
    masked = 1;
    UpdateClip();
}


void CellEngine::UpdateClip()
{
    if (rectList != NULL) {
        delete [] rectList;
        rectList = NULL;
        clipRects = 0;
    } // if

    if (masked) {
        Byte *mem = maskMem;
        long rectCount = 0;
        long rectBufSize = 256;
        RECT *rects = new RECT[rectBufSize];
        RECT *r = NULL;
        long xx, yy;
        long curRect = -1;
        long aboveRect = -1;
        for (yy = 0; yy < maskHeight; yy++) {
            long isInside = 0;
            long wasInside = 0;
            for (xx = 0; xx < maskWidth; xx++) {
                long pixel = mem[xx];
                isInside = pixel > maskClip;
                if (wasInside) {
                    if (! isInside) {
                        r->right = xx;
                        if ((aboveRect != -1) &&
                            (rects[aboveRect].right == xx)) {
                            // merge with rect above
                            rects[aboveRect].bottom = r->bottom;
                            rectCount--;
                        } // if
                        wasInside = 0;
                        curRect = -1;
                        aboveRect = -1;
                    } // if (! isInside)
                } else { // if (wasInside)
                    if (isInside) {
                        wasInside = 1;

                        curRect = rectCount++;

                        if (rectCount > rectBufSize) {
                            long i;
                            RECT *newRects;

                            rectBufSize += 256;
                            newRects = new RECT[rectBufSize];
                            for (i = 0; i < curRect; i++) {
                                newRects[i] = rects[i];
                            } // for
                            delete [] rects;
                            rects = newRects;
                        } // if (rectCount > rectBufSize)

                        r = &rects[curRect];
                        r->left = xx;
                        r->top = yy;
                        r->right = xx + 1;
                        r->bottom = yy + 1;

                        aboveRect = -1;

                        {   long i;
                            for (i = 0; i < curRect; i++) {
                                RECT *ra = &rects[i];
                                if ((ra->left == xx) &&
                                    (ra->bottom == yy)) {
                                    aboveRect = i;
                                    break;
                                } // if
                            } // for i
                        }
                    } // if (isInside)
                } // if (wasInside)
            } // for xx

            if (curRect != -1) {
                r->right = xx;
                if ((aboveRect != -1) &&
                    (rects[aboveRect].right == xx)) {
                    // merge with rect above
                    rects[aboveRect].bottom = r->bottom;
                    rectCount--;
                } // if
            } // if (curRect != -1)

            curRect = -1;
            aboveRect = -1;
            mem += maskRowBytes;

        } // for yy

        {
            long i;
            RECT *newRects = new RECT[rectCount];

            clipRects = rectCount;
            rectList = newRects;
            for (i = 0; i < rectCount; i++) {
                newRects[i] = rects[i];
            } // for
            delete [] rects;
        }
    } // if (masked)
}


void CellEngine::LoadRule(
    const char *name)
{
    if (ruleName != NULL) {
        delete [] ruleName;
    }

    ruleName = new char[strlen(name) + 1];

    strcpy(ruleName, name);

    const char *stream =
        NULL;

    const char **pstr =
        ruleTables;

    while (pstr[0] != NULL) {
        if (stricmp(
                name,
                pstr[0]) == 0) {
            stream =
                pstr[1];
            break;
        } // if
        pstr += 2;
    } // while

    if (stream != NULL) {
        LoadRuleData(stream);
    } // if
}


void CellEngine::LoadRuleData(
    const char *stream)
{
    QUAD magic;

    if (stream == NULL) {
        ruleSize = 0;
        neighborhood = 0;
    } else {

        // XXX: Make this byte order independent!!!

        magic = *(long *)stream;
        SWAPLONG(magic);
        if (magic != (QUAD)0xCAC0CAC0) return;
        stream += sizeof(long);

        neighborhood = *(long *)stream;
        SWAPLONG(neighborhood);
        stream += sizeof(long);

        ruleSize = *(long *)stream;
        SWAPLONG(ruleSize);
        stream += sizeof(long);
    }

    if ((rule != NULL) &&
        (!ruleStatic)) {
        delete [] rule;
        rule = NULL;
        ruleStatic = 0;
    } // if

    if (ruleSize != 0) {
        rule = new Byte[ruleSize];
        memcpy(rule, stream, ruleSize);
        ruleStatic = 0;
    } // if
}


void CellEngine::LoadStaticRuleData(
    const char *stream)
{
    QUAD magic;

    if (stream == NULL) {
        ruleSize = 0;
        neighborhood = 0;
    } else {

        // XXX: Make this byte order independent!!!

        magic = *(long *)stream;
        SWAPLONG(magic);
        if (magic != (LONG)0xCAC0CAC0) return;
        stream += sizeof(long);

        neighborhood = *(long *)stream;
        SWAPLONG(neighborhood);
        stream += sizeof(long);

        ruleSize = *(long *)stream;
        SWAPLONG(ruleSize);
        stream += sizeof(long);
    }

    rule = (Byte *)stream;
    ruleStatic = 1;
}


void CellEngine::SetRuleTable(
    const char *table,
    int ruleSize,
    int neigh)
{
    neighborhood = neigh;
    if (rule != NULL) {
      delete [] rule;
      rule = NULL;
    } // if

    if (ruleSize != 0) {
      rule = new unsigned char[ruleSize];
      memcpy(rule, table, ruleSize);
      ruleStatic = 0;
    } // if
}


int CellEngine::CountRules()
{
    if (ruleCount < 0) {
        ruleCount = 0;
        while (ruleTables[ruleCount << 1] != NULL) {
            ruleCount++;
        } // while
    } // if

    return ruleCount;
}


const char *CellEngine::GetRuleName(
    int i)
{
    if ((i < 0) ||
        (i >= CountRules())) {
        return NULL;
    } // if

    return ruleTables[i << 1];
}


const char *CellEngine::GetRuleData(
    int i)
{
    if ((i < 0) ||
        (i >= CountRules())) {
        return NULL;
    } // if

    return ruleTables[(i << 1) + 1];
}


long CellEngine::DoPhysics()
{
    long xx = x;
    long yy = y;
    long oldx = xx;
    long oldy = yy;
    long dxx = dx;
    long dyy = dy;
    long events = 0;

    if (tracking) {
        dxx = dyy = 0;
    } // if

    xx = xx + dxx;
    if (xx < 0) {
        xx = 0;
        if (dxx < 0) dxx = -dxx;
        events |= EVENT_BOUNCE_LEFT;
    } else {
        if ((xx + width) > screenWidth) {
            xx = screenWidth - width;
            if (xx < 0) xx = 0;
            if (dxx > 0) dxx = -dxx;
            events |= EVENT_BOUNCE_RIGHT;
        } // if
    } // if

    yy = yy + dyy;
    if (yy < 0) {
        yy = 0;
        if (dyy < 0) {
            dyy = -dyy;
        } // if
        events |= EVENT_BOUNCE_TOP;
    } else {
        if ((yy + height) > screenHeight) {
            yy = screenHeight - height;
            if (yy < 0) yy = 0;
            if (dyy > 0) {
                dyy = -dyy;
            } // if
            events |= EVENT_BOUNCE_BOTTOM;
        } // if
    } // if

    if ((!tracking) && events) {
        if (hubba) {
            frob += (Rand16() % ((2 * hubba) + 1)) - hubba;
            if (frob > hubba) frob = hubba;
            if (frob < -hubba) frob = -hubba;
        } // if
        if (rumble) {
            do {
                if (rumble > 0) {
                    if (events & (EVENT_BOUNCE_LEFT |
                                  EVENT_BOUNCE_RIGHT |
                                  EVENT_BOUNCE_TOP |
                                  EVENT_BOUNCE_BOTTOM)) {
                        dxx += (Rand16() % ((2 * rumble) + 1)) - rumble;
                        dyy += (Rand16() % ((2 * rumble) + 1)) - rumble;
                        if (rumblemax) {
                            if (dxx > rumblemax) dxx = rumblemax;
                            if (dxx < -rumblemax) dxx = -rumblemax;
                            if (dyy > rumblemax) dyy = rumblemax;
                            if (dyy < -rumblemax) dyy = -rumblemax;
                        } // if
                    } // if
                } else {
                    if (events & (EVENT_BOUNCE_LEFT |
                                  EVENT_BOUNCE_RIGHT)) {
                        dxx += (Rand16() % ((-2 * rumble) + 1)) + rumble;
                    } // if
                    if (events & (EVENT_BOUNCE_TOP |
                                  EVENT_BOUNCE_BOTTOM)) {
                        dyy += (Rand16() % ((-2 * rumble) + 1)) + rumble;
                    } // if
                } // if
            } while ((dxx == 0) && (dyy == 0));
        } // if
    } // if

    dx = dxx + gravx;
    dy = dyy + gravy;

    if ((oldx != xx) || (oldy != yy)) {
        SetPos(xx, yy);
        ForceOnScreen();
        events |= EVENT_MOVE;
    } // if

    return events;
}


long CellEngine::DoRule()
{
    int step = 0;

    if (!moveImage) {
        DoPhysics();
    }

    if (skips > 0) {
        if (skip > 0) {
            skip--;
done:
            if (moveImage) {
                DoPhysics();
            }
            return step;
        } // if
        skip = skips;
    } // if

    if (!OnScreen()) goto done;

    for (step = 0;
         step < steps;
         step++) {

        CopyToBack();

        if (moveImage &&
            (step == 0)) {
            DoPhysics();
        }

        PumpToFront();

    } // for step

    return step;
}


void CellEngine::PostRule()
{
}


////////////////////////////////////////////////////////////////////////
//
// Before applying a rule, we copy the front buffer to the back buffer.
// The back buffer is two pixels wider and taller than the front buffer,
// so that we can handle the edge conditions efficiently.
// The main cellular automata loops do not need to worry about the special case
// at the edges, because we pre-wrap a one pixel wide perimeter around the back
// buffer before applying the rule.
//
//  0   ff  f0 f1 ... fe ff     f0
//
//  1   0f  aa bb ... ee ee     00
//  2   1f  10 11 ... ee ff     10
//      ..  .. ..     .. ..     ..
//      ef  e0 e1 ... ee ff     e0
//  h   ff  f0 f1 ... ee ff     f0
//
//  h+1 0f  00 01 ... 0e 0f     00
//
//
// There are several "wrap" modes to handle the edge conditions.
// Initially:
//      BACK    FRONT
//      ----    -----
//      abcd    0123
//      eFGh    4567
//      iJKl    8901
//      mnop    2345
//
// Legend:
//      aBCd=>  0AB3    Copy back to front.
//                      Upper case source letters are copied.
//             <2121>   Wrap from opposite edges (vertical and horizontal).
//             |1122|   Truncate from same edges (vertical and horizontal).
//
//  WRAP    EFFECT
//  ----    ------
//  0       no effect, don't change back
//      abcd    0123
//      efgh    4567
//      ijkl    8901
//      mnop    2345
//  1       copy front to back, no wrap, don't change back perimeter
//      abcd    0123
//      eFGh=>  4FG7
//      iJKl=>  8JK1
//      mnop    2345
//  2       don't copy front to back, just wrap edges
//      abcd    0909
//      eFGh   <6565>
//      iJKl   <0909>
//      mnop    6565
//  3       copy front to back, and wrap edges
//      abcd    KJKJ
//      eFGh=> <GFGF>
//      iJKl=> <KJKJ>
//      mnop    GFGF
//  4       copy front to back, truncate edges
//      abcd    FFGG
//      eFGh=> |FFGG|
//      iJKl=> |JJKK|
//      mnop    JJKK
//  5       copy edges from screen, don't wrap
//      ABCD=>  ABCD
//      EFGH=>  EFGH
//      IJKL=>  IJKL
//      MNOP=>  MNOP
//

void CellEngine::CopyToBack()
{
    long yy;
    Byte *f = frontMem;
    Byte *p = backMem +
              backBorder +
              (backBorder * backRowBytes);

    switch (wrap) {

    case 0:
        // no effect, don't change back
        break;

    case 1:
        // copy front to back, no wrap,
        // don't change back perimeter
        for (yy = 0; yy < height; yy++) {
            memcpy(p, f, width);
            p += backRowBytes;
            f += screenRowBytes;
        }
        break;

    case 2:
        // don't copy front to back, just wrap 1 pixel edges
        for (yy = 0; yy < height; yy++) {
            p[-1] = p[width - 1];
            p[width] = p[0];
            p += backRowBytes;
            f += screenRowBytes;
        } // for
        f = frontMem;
        p = backMem +
            backBorder +
            (backBorder * backRowBytes);
        memcpy(&p[(-1 * backRowBytes) - 1],
               &p[((height - 1) * backRowBytes) - 1],
               width + 2);
        memcpy(&p[(height * backRowBytes) - 1],
               &p[-1],
               width + 2);
        break;

    case 3:
        // copy front to back, and wrap 1 pixel edges
        for (yy = 0; yy < height; yy++) {
            memcpy(p, f, width);
            p[-1] = p[width - 1];
            p[width] = p[0];
            p += backRowBytes;
            f += screenRowBytes;
        } // for
        f = frontMem;
        p = backMem +
            backBorder +
            (backBorder * backRowBytes);
        memcpy(&p[(-1 * backRowBytes) - 1],
               &p[((height - 1) * backRowBytes) - 1],
               width + 2);
        memcpy(&p[(height * backRowBytes) - 1],
               &p[-1],
               width + 2);
        break;

    case 4:
        // copy front to back, truncate edges
        for (yy = 0; yy < height; yy++) {
            memcpy(p, f, width);
            p[-1] = p[0];
            p[width] =  p[width - 1];
            p += backRowBytes;
            f += screenRowBytes;
        } // for
        f = frontMem;
        p = backMem +
            backBorder +
            (backBorder * backRowBytes);
        memcpy(&p[(-1 * backRowBytes) - 1],
               &p[-1],
               width + 2);
        memcpy(&p[(height * backRowBytes) - 1],
               &p[((height - 1) * backRowBytes) - 1],
               width + 2);
        break;

    case 5:
        {
            // copy edges from screen, don't wrap
            // Mind those screen edge conditions!
            long left = (x == 0);
            long right = (x + width == screenWidth);
            long top = (y == 0);
            long bottom = (y + height == screenHeight);

            if (!left && !right && !top && !bottom) {
                p -= backRowBytes + 1;
                f -= screenRowBytes + 1;

                for (yy = -1; yy <= height; yy++) {
                    memcpy(p, f, width + 2);
                    p += backRowBytes;
                    f += screenRowBytes;
                } // for
            } else {
                left = left
                        ? (0)
                        : (-1);
                right = right
                        ? (width - 1)
                        : (width);
                top = top
                        ? (0)
                        : (-screenRowBytes);
                bottom = bottom
                        ? ((height - 1) * screenRowBytes)
                        : (height * screenRowBytes);

                p[-backRowBytes - 1] =
                    f[left + top];

                memcpy(p - backRowBytes,
                       f + top,
                       width);

                p[-backRowBytes + width] =
                    f[right + top];

                p[(height * backRowBytes) - 1] =
                    f[left + bottom];

                memcpy(p + (height * backRowBytes),
                       f + bottom,
                       width);

                p[(height * backRowBytes) + width] =
                    f[right + bottom];

                for (yy = 0; yy < height; yy++) {
                    p[-1] = f[left];
                    memcpy(p, f, width);
                    p[width] = f[right];
                    p += backRowBytes;
                    f += screenRowBytes;
                } // for
            } // if
        }
        break;

    case 6:
        // copy front to back, and wrap 2 pixel edges
        for (yy = 0; yy < height; yy++) {
            memcpy(p, f, width);
            p[-1] = p[width - 1];
            p[-2] = p[width - 2];
            p[width] = p[0];
            p[width + 1] = p[1];
            p += backRowBytes;
            f += screenRowBytes;
        } // for
        f = frontMem;
        p = backMem +
            backBorder +
            (backBorder * backRowBytes);
        memcpy(&p[(-1 * backRowBytes) - 2],
               &p[((height - 1) * backRowBytes) - 2],
               width + 4);
        memcpy(&p[(-2 * backRowBytes) - 2],
               &p[((height - 2) * backRowBytes) - 2],
               width + 4);
        memcpy(&p[(height * backRowBytes) - 2],
               &p[-2],
               width + 4);
        memcpy(&p[((height + 1) * backRowBytes) - 2],
               &p[-2 + backRowBytes],
               width + 4);
        break;

    } // switch wrap
}


void CellEngine::PumpToFront()
{
    switch (neighborhood) {
    case -1:
        break;
    case 0:
    case 1:
        n_moore_a();
        break;
    case 2:
        n_moore_ab();
        break;
    case 3:
        n_vonn_neumann();
        break;
    case 4:
        n_margolis();
        break;
    case 5:
        n_margolis_ph();
        break;
    case 6:
        n_margolis_hv();
        break;
    case 7:
        n_life();
        break;
    case 8:
        n_brain();
        break;
    case 9:
        n_heat();
        break;
    case 10:
        n_dheat();
        break;
    case 11:
        n_lheat();
        break;
    case 12:
        n_ldheat();
        break;
    case 13:
        n_ranch();
        break;
    case 14:
        n_anneal();
        break;
    case 15:
        n_anneal4();
        break;
    case 16:
        n_anneal8();
        break;
    case 17:
        n_eco();
        break;
    case 18:
        n_abdheat();
        break;
    case 19:
        n_edheat();
        break;
    case 20:
        n_abcdheat();
        break;
    case 21:
        n_torben();
        break;
    case 22:
        n_torben2();
        break;
    case 23:
        n_torben3();
        break;
    case 24:
        n_torben4();
        break;
    case 25:
        n_ball();
        break;
    case 26:
        n_fdheat();
        break;
    case 27:
        n_fabcdheat();
        break;
    case 28:
        n_risca();
        break;
    case 29:
        n_insert();
        break;
    case 30:
        n_heaco();
        break;
    case 31:
        n_marble();
        break;
    case 32:
        n_smarble();
        break;
    case 33:
        n_farble();
        break;
    case 34:
        n_garblebug();
        break;
    case 35:
        n_twoheats();
        break;
    case 36:
        n_spin();
        break;
    case 37:
        n_driven();
        break;
    case 38:
        n_daft();
        break;
    case 39:
        n_spinsonly();
        break;
    case 40:
        n_spinsbank();
        break;
    case 41:
        n_spinsheat();
        break;
    case 42:
        n_spinglass();
        break;
    case 43:
        n_glassbonds();
        break;
    case 44:
        n_glassheat();
        break;
    case 45:
        n_faders();
        break;
    case 46:
        n_harble();
        break;
    case 47:
        n_perlin();
        break;
    case 48:
        n_dendrite();
        break;
    case 49:
        n_vanneal();
        break;
    case 50:
        n_vanneal8();
        break;
    case 51:
        n_garble();
        break;
    case 52:
        n_jvn29();
        break;
    default:
        break;
  } // switch neighborhood

  phase = !phase;
  ticks++;
}


Byte CellEngine::GetCell(
    int col,
    int row)
{
  if ((col < 0) ||
      (col >= width) ||
      (row < 0) ||
      (row >= height) ||
      (frontMem == NULL)) {
    return 0;
  }

  Byte cell =
    frontMem[col + (row * screenRowBytes)];

  return cell;
}


void CellEngine::SetCell(
    int col,
    int row,
    Byte cell)
{
  if ((col < 0) ||
      (col >= width) ||
      (row < 0) ||
      (row >= height) ||
      (frontMem == NULL)) {
    return;
  }

  frontMem[col + (row * screenRowBytes)] =
    cell;
}


void *CellEngine::GetCellBuffer()
{
    return (void *)frontMem;
}


void CellEngine::n_moore_a()
{
    if (rule == NULL) return;

    // 0    1    2    3    4    5    6    7    8     9
    // c    c'   se   sw   ne   nw   e    w    s     n
    // 0x1  0x2  0x4  0x8  0x10 0x20 0x40 0x80 0x100 0x200

#define MOORE_A ( \
     ((nw & 1) << 5) |  \
     ((n  & 1) << 9) | \
     ((ne & 1) << 4) | \
     ((w  & 1) << 7) | \
     ((c  & 3) << 0) | \
     ((e  & 1) << 6) | \
     ((sw & 1) << 3) | \
     ((s  & 1) << 8) | \
     ((se & 1) << 2))

    if (inflation) {
        int last = 0;
        int heat = 0;

        CAM_MASKED_REG_TABLE_HEAT6_LOOP(MOORE_A)
    } else {
        CAM_MASKED_REG_TABLE_LOOP(MOORE_A)
    } // if
}


void CellEngine::n_moore_ab()
{
    if (rule == NULL) return;

    // 0    1    2    3    4    5    6    7    8     9     10    11
    // c    c'   se   sw   ne   nw   e    w    s     n     &c    &c'
    // 0x1  0x2  0x4  0x8  0x10 0x20 0x40 0x80 0x100 0x200 0x400 0x800

#define MOORE_AB ( \
    MOORE_A | \
    ((c & 0x0c) << 8))

    if (inflation) {
        int last = 0;
        int heat = 0;

        CAM_MASKED_REG_TABLE_HEAT4_LOOP(MOORE_AB)
    } else {
        CAM_MASKED_REG_TABLE_LOOP(MOORE_AB)
    } // if
}


void CellEngine::n_vonn_neumann()
{
    if (rule == NULL) return;

    // 0    1    2    3    4    5    6    7    8     9
    // c    c'   e'   w'   s'   n'   e    w    s     n
    // 0x1  0x2  0x4  0x8  0x10 0x20 0x40 0x80 0x100 0x200

#define VON_NEUMANN ( \
    ((c & 3) << 0) | \
    ((e & 1) << 6) | \
    ((e & 2) << 1) | \
    ((w & 1) << 7) | \
    ((w & 2) << 2) | \
    ((s & 1) << 8) | \
    ((s & 2) << 3) | \
    ((n & 1) << 9) | \
    ((n & 2) << 4))

    if (inflation) {
        int last = 0;
        int heat = 0;

        CAM_MASKED_REG_TABLE_HEAT6_LOOP(VON_NEUMANN)
    } else {
        CAM_MASKED_REG_TABLE_LOOP(VON_NEUMANN)
    }
}


void CellEngine::n_margolis()
{
    if (rule == NULL) return;

    register Byte i;

    // 0    1    2    3    4    5    6    7    8    9
    // c    c'   cw   ccw  opp  cw'  ccw' opp'
    // 0x1  0x2  0x4  0x8  0x10 0x20 0x40 0x80 0x100 0x200

#define MARGOLIS_ODD ( \
    /* c c' */ \
    (c & 3) | \
    /* cw cw' */ \
    (i = ((xx & 1) ? ((yy & 1) ? (e & 3) : (n & 3)) \
                   : ((yy & 1) ? (s & 3) : (w & 3))), \
     (((i & 1) << 2) | ((i & 2) << 4))) | \
    /* ccw ccw' */ \
    (i = ((xx & 1) ? ((yy & 1) ? (s & 3) : (e & 3)) \
                   : ((yy & 1) ? (w & 3) : (n & 3))), \
     (((i & 1) << 3) | ((i & 2) << 5))) | \
    /* opp opp' */ \
    (i = ((xx & 1) ? ((yy & 1) ? (se & 3) : (ne & 3)) \
                   : ((yy & 1) ? (sw & 3) : (nw & 3))), \
     (((i & 1) << 4) | ((i & 2) << 6))))

#define MARGOLIS_EVEN ( \
    /* c c' */ \
    (c & 3) | \
    /* cw cw' */ \
    (i = ((xx & 1) ? ((yy & 1) ? (w & 3) : (s & 3)) \
                   : ((yy & 1) ? (n & 3) : (e & 3))), \
     (((i & 1) << 2) | ((i & 2) << 4))) | \
    /* ccw ccw' */ \
    (i = ((xx & 1) ? ((yy & 1) ? (n & 3) : (w & 3)) \
                   : ((yy & 1) ? (e & 3) : (s & 3))), \
     (((i & 1) << 3) | ((i & 2) << 5))) | \
    /* opp opp' */ \
    (i = ((xx & 1) ? ((yy & 1) ? (nw & 3) : (sw & 3)) \
                   : ((yy & 1) ? (ne & 3) : (se & 3))), \
     (((i & 1) << 4) | ((i & 2) << 6))))

    if (inflation) {
        int last = 0;
        int heat = 0;

        if (phase) {
            CAM_MASKED_REG_TABLE_HEAT6_LOOP(MARGOLIS_ODD)
        } else {
            CAM_MASKED_REG_TABLE_HEAT6_LOOP(MARGOLIS_EVEN)
        }
    } else {
        if (phase) {
            CAM_MASKED_REG_TABLE_LOOP(MARGOLIS_ODD)
        } else {
            CAM_MASKED_REG_TABLE_LOOP(MARGOLIS_EVEN)
        }
    }
}


void CellEngine::n_margolis_ph()
{
    if (rule == NULL) return;

    register Byte i;

    // 0    1    2    3    4    5    6    7    8    9
    // c    c'   cw   ccw  opp  cw'  ccw' opp' pha   pha'
    // 0x1  0x2  0x4  0x8  0x10 0x20 0x40 0x80 0x100 0x200

#define MARGOLIS_ODD_PH ( \
    MARGOLIS_ODD | \
    0x100)

#define MARGOLIS_EVEN_PH ( \
    MARGOLIS_EVEN | \
    0x200)

    if (inflation) {
        int last = 0;
        int heat = 0;

        if (phase) {
            CAM_MASKED_REG_TABLE_HEAT6_LOOP(MARGOLIS_ODD_PH)
        } else {
            CAM_MASKED_REG_TABLE_HEAT6_LOOP(MARGOLIS_EVEN_PH)
        }
    } else {
        if (phase) {
            CAM_MASKED_REG_TABLE_LOOP(MARGOLIS_ODD_PH)
        } else {
            CAM_MASKED_REG_TABLE_LOOP(MARGOLIS_EVEN_PH)
        }
    }
}


void CellEngine::n_margolis_hv()
{
    if (rule == NULL) return;

    register Byte i;

    // 0    1    2    3    4    5    6    7    8    9
    // c    c'   cw   ccw  opp  cw'  ccw' opp' horz  vert
    // 0x1  0x2  0x4  0x8  0x10 0x20 0x40 0x80 0x100 0x200

#define MARGOLIS_ODD_HV ( \
    MARGOLIS_ODD | \
    ((xx & 1) << 8) | \
    ((yy & 1) << 9))

#define MARGOLIS_EVEN_HV ( \
    MARGOLIS_EVEN | \
    ((xx & 1) << 8) | \
    ((yy & 1) << 9))

    if (inflation) {
        int last = 0;
        int heat = 0;

        if (phase) {
            CAM_MASKED_REG_TABLE_HEAT6_LOOP(MARGOLIS_ODD_HV)
        } else {
            CAM_MASKED_REG_TABLE_HEAT6_LOOP(MARGOLIS_EVEN_HV)
        }
    } else {
        if (phase) {
            CAM_MASKED_REG_TABLE_LOOP(MARGOLIS_ODD_HV)
        } else {
            CAM_MASKED_REG_TABLE_LOOP(MARGOLIS_EVEN_HV)
        }
    }
}


void CellEngine::n_life()
{
  long sum;

#define LIFE ( \
     ((c & 1) \
        ? (((sum = SUM8REG(1)) == 2) || \
           (sum == 3)) \
        : ((SUM8REG(1) == 3) ? 1 : 0)) | \
     ((c & 0x7f) << 1))

    if (inflation) {
        int last = 0;
        int heat = 0;

        CAM_MASKED_REG_HEAT6_LOOP(LIFE)
    } else {
        CAM_MASKED_REG_LOOP(LIFE)
    }
}


void CellEngine::n_brain()
{

#define BRAIN ( \
    (((c & 0x03) == 0) \
        ? ((SUM8REG(1) == 2) ? 1 : 0) \
        : 0) | \
    ((c << 1) & 0xfe))

    if (inflation) {
        int last = 0;
        int heat = 0;

        CAM_MASKED_REG_HEAT6_LOOP(BRAIN)
    } else {
        CAM_MASKED_REG_LOOP(BRAIN)
    }
}


void CellEngine::n_heat()
{
#define HEAT ( \
    (nw + n + ne + w + frob + e + sw + s + se) >> 3)

    CAM_MASKED_REG_LOOP(HEAT)
}


void CellEngine::n_dheat()
{
  long last = 0;

#define DHEAT \
    last += (nw + n + ne + w + frob + e + sw + s + se); \
    *front = (Byte)last >> 3; \
    last &= 7;

    CAM_MASKED_REG_LOOP_BODY(DHEAT)
}


void CellEngine::n_lheat()
{
#define LHEAT \
    (Byte)((n + w + e + s + frob) >> 2)

    CAM_MASKED_REG_LOOP(LHEAT)
}


void CellEngine::n_ldheat()
{
    long last = 0;

#define LDHEAT \
    (Byte)((last = (n + w + e + s + frob + (last & 3))), (last >> 2))

    CAM_MASKED_REG_LOOP(LDHEAT)
}


void CellEngine::n_abdheat()
{
    //long last = 0;

#define ABDHEAT \
    (Byte)(((n + w + e + s + \
             (((xx^yy^phase)&1) ? frob : -frob)) >> 2))

    CAM_MASKED_REG_LOOP(ABDHEAT)
}


void CellEngine::n_abcdheat()
{
    long last = 0;

#define ABCDHEAT \
    (Byte)((last = (n + w + e + s + \
                    (((xx^yy^phase)&1) ? frob : -frob) + \
                    (last & 3)), \
            (last >> 2)))

    CAM_MASKED_REG_LOOP(ABCDHEAT)
}


void CellEngine::n_edheat()
{
    //long last = 0;

    CAM_MASKED_REG_LOOP(HEAT)
}


#define RANCH(nw, n, ne, w, c, e, sw, s, se) \
    (Byte)(n ^ w ^ e ^ s)

void CellEngine::n_ranch()
{
#define XRANCH (n ^ s ^ e ^ w)

    CAM_MASKED_REG_LOOP(XRANCH)
}


void CellEngine::n_anneal()
{
    long sum;

#define ANNEAL ( \
    (((sum = SUM9REG(1)) > 5) || (sum == 4)) | \
    ((c&0x7f) << 1))

    CAM_MASKED_REG_LOOP(ANNEAL)
}


void CellEngine::n_anneal4()
{
    long sum = 0;

#define ANNEAL4 ( \
    ((((sum = (SUM9REG(1) >> 0)) > 5) || (sum == 4)) ? 1 : 0) | \
    ((((sum = (SUM9REG(2) >> 1)) > 5) || (sum == 4)) ? 2 : 0) | \
    ((((sum = (SUM9REG(4) >> 2)) > 5) || (sum == 4)) ? 4 : 0) | \
    ((((sum = (SUM9REG(8) >> 3)) > 5) || (sum == 4)) ? 8 : 0) | \
    ((c&0x0f) << 4))

    CAM_MASKED_REG_LOOP(ANNEAL4)
}


void CellEngine::n_anneal8()
{
    long sum = 0;

#define ANNEAL8 ( \
    ((((sum = (SUM9REG(1) >> 0)) > 5) || (sum == 4)) ? 1 : 0) | \
    ((((sum = (SUM9REG(2) >> 1)) > 5) || (sum == 4)) ? 2 : 0) | \
    ((((sum = (SUM9REG(4) >> 2)) > 5) || (sum == 4)) ? 4 : 0) | \
    ((((sum = (SUM9REG(8) >> 3)) > 5) || (sum == 4)) ? 8 : 0) | \
    ((((sum = (SUM9REG(16) >> 4)) > 5) || (sum == 4)) ? 16 : 0) | \
    ((((sum = (SUM9REG(32) >> 5)) > 5) || (sum == 4)) ? 32 : 0) | \
    ((((sum = (SUM9REG(64) >> 6)) > 5) || (sum == 4)) ? 64 : 0) | \
    ((((sum = (SUM9REG(128) >> 7)) > 5) || (sum == 4)) ? 128 : 0))

    CAM_MASKED_REG_LOOP(ANNEAL8)
}


void CellEngine::n_eco()
{
    long sum;

#define ANTILIFE ( \
     ((sum = SUM8REG(1)), \
     (((c&1) ? (sum != 5) : (((sum != 5) && (sum != 6)) ? 1 : 0)) | \
       ((c&0x7f) << 1))))

#define ECO ( \
      ((((sum = (SUM9REG(128) >> 7)) > 5) || (sum == 4)) \
        ? ((ANTILIFE & 127) | 128) \
        : (BRAIN & 127)))

    if (inflation) {
        int last = 0;
        int heat = 0;

        CAM_MASKED_REG_HEAT4_LOOP(ECO)
    } else {
        CAM_MASKED_REG_LOOP(ECO)
    }
}


void CellEngine::n_torben()
{
    long sum;

    // 0 0 0 1 0 1 0 1 1 1

#define TORBEN ( \
    ((((sum = SUM9REG(1)) > 6) || (sum == 5) || (sum == 3)) ? 1 : 0) | \
    ((c&0x7f) << 1))

    CAM_MASKED_REG_LOOP(TORBEN)
}


void CellEngine::n_torben2()
{
    long sum;

    // 0 0 0 1 0 1 0 1 1 1
    // 0 0 1 0 1 0 1 0 1 1

#define TORBEN2 ( \
    ((((sum = SUM9REG(1)) > 6) || (sum == 3) || (sum == 4)) ? 1 : 0) | \
    ((c&0x7f) << 1))

    CAM_MASKED_REG_LOOP(TORBEN2)
}


void CellEngine::n_torben3()
{
    long sum;

    // 0 0 0 1 1 0 0 1 1 1

#define TORBEN3 ((TORBEN) ^ (TORBEN2))

    CAM_MASKED_REG_LOOP(TORBEN3)
}


void CellEngine::n_torben4()
{
    long sum;

    // 1 0 0 0 1 0 1 1 1 0

#define TORBEN4 ( \
    ((((sum = SUM9REG(1)) == 0) || \
                  (sum == 4) || \
                  (sum == 6) || \
                  (sum == 7) || \
                  (sum == 8)) ? 1 : 0) | \
    ((c&0x7f) << 1))

    CAM_MASKED_REG_LOOP(TORBEN4)
}


void CellEngine::n_ball()
{
    // This spatio temporal phase synchronization is busted.
    // It stays in synch for vertical and horizontal movement,
    // but gets out of phase for diagonal movement,
    // and I can't figure out how to keep it in phase
    // regardless of position.
    char p = ((phase ^ x ^ y) & 1);
    long xx, yy, r = Rand16();
    Byte *front = frontMem;
    Byte *back = backMem +
                 (backBorder - 1) +
                 ((backBorder - 1) * backRowBytes);

    if (!p) {
        back += 1 + backRowBytes;
    }

    for (yy = p + (height >>1);
         yy > 0;
         yy--) {
        for (xx = p + (width >>1);
             xx > 0;
             xx--) {
            Byte nw = back[0],              nw0 = nw >> 6,
                 ne = back[1],              ne0 = ne >> 6,
                 sw = back[backRowBytes],   sw0 = sw >> 6,
                 se = back[backRowBytes+1], se0 = se >> 6;

//          r += nw + ne + sw + sw; r >>= 1;
            r += (nw0&1) + (ne0&1) + (sw0&1) + (se0&1); r >>= 1;

            if ((nw0 | ne0 | sw0 | se0) & 2) {
                if (nw0 == 1) {
                    back[0] |= 128;
                }
                if (ne0 == 1) {
                    back[1] |= 128;
                }
                if (sw0 == 1) {
                    back[backRowBytes] |= 128;
                }
                if (se0 == 1) {
                    back[backRowBytes+1] |= 128;
                }
            } else {

                switch ((nw0&1) + (ne0&1) + (sw0&1) + (se0&1)) {

                case 0:
                case 3:
                case 4:
                    // same
                    back[0] = nw; back[1] = ne;
                    back[backRowBytes] = sw;
                    back[backRowBytes+1] = se;
                    break;

                case 1:
                    // reflect
                    back[0] = se; back[1] = sw;
                    back[backRowBytes] = ne;
                    back[backRowBytes+1] = nw;
                    break;

                case 2:
                    // turn
                    switch (((nw0&1) <<3) |
                            ((ne0&1) <<2) |
                            ((sw0&1) <<1) |
                            (se0&1)) {

                    case 6:
                    case 9:
                        if (r&1) {
                            // clockwise
                            back[0] = sw; back[1] = nw;
                            back[backRowBytes] = se;
                            back[backRowBytes+1] = ne;
                        } else {
                            // counterclockwise
                            back[0] = ne; back[1] = se;
                            back[backRowBytes] = nw;
                            back[backRowBytes+1] = sw;
                        }
                        break;

                    default:
                          back[0] = nw; back[1] = ne;
                          back[backRowBytes] = sw;
                          back[backRowBytes+1] = se;
                          break;

                    } // switch

                    break;

                } // switch
            } // if

            back += 2;
        }
        back += (backRowBytes * 2) - width - (p * 2);
    }

    back = backMem +
           backBorder +
           (backBorder * backRowBytes);
    front = frontMem;

    for (yy = height;
         yy > 0;
         yy--) {
        memcpy(front, back, width);
        back += backRowBytes;
        front += screenRowBytes;
    }
}


#define FDHEAT(nw, n, ne, w, c, e, sw, s, se) \
    error += (nw + n + ne + w + frob + e + sw + s + se); \
    result = error >> 3; \
    error &= 7;


#ifndef _WIN32


void CellEngine::n_fdheat()
{
    QUAD *front = (QUAD *)frontMem;
    QUAD *back = (QUAD *)(backMem +
                          (backBorder - 1) +
                          ((backBorder - 1) * backRowBytes));
    long backline = backRowBytes >> 2;
    long backstride = backline - ((width >> 2) + 1);
    long frontstride = (screenRowBytes >> 2) - (width >> 2);
    long xsteps = width >> 2;
    long yy;

    total = 0;

    for (yy = height;
         yy > 0;
         back += backstride,
         front += frontstride,
         yy--) {
        register QUAD b1, b2;
        register QUAD c1, c2;
        register QUAD d1, d2;
        long error = 0;
        long xx;

        b2 = back[0];
        c2 = back[backline];
        d2 = back[backline << 1];
        back++;

        for (xx = xsteps;
             xx > 0;
             xx--) {
            long result, cells = 0;

            b1 = b2; b2 = back[0];
            c1 = c2; c2 = back[backline];
            d1 = d2; d2 = back[backline << 1];
            back++;

            FDHEAT( BYTE0(b1), BYTE1(b1), BYTE2(b1),
                    BYTE0(c1), BYTE1(c1), BYTE2(c1),
                    BYTE0(d1), BYTE1(d1), BYTE2(d1) );
            total += result;
            SETBYTE0(cells, result);

            FDHEAT( BYTE1(b1), BYTE2(b1), BYTE3(b1),
                    BYTE1(c1), BYTE2(c1), BYTE3(c1),
                    BYTE1(d1), BYTE2(d1), BYTE3(d1) );
            total += result;
            SETBYTE1(cells, result);

            FDHEAT( BYTE2(b1), BYTE3(b1), BYTE0(b2),
                    BYTE2(c1), BYTE3(c1), BYTE0(c2),
                    BYTE2(d1), BYTE3(d1), BYTE0(d2) );
            total += result;
            SETBYTE2(cells, result);

            FDHEAT( BYTE3(b1), BYTE0(b2), BYTE1(b2),
                    BYTE3(c1), BYTE0(c2), BYTE1(c2),
                    BYTE3(d1), BYTE0(d2), BYTE1(d2) );
            total += result;
            SETBYTE3(cells, result);

            *front = cells;
            front++;
        }
    }
}


#else


class CellEngine *cellEngine = NULL; // to fool the assembler

void CellEngine::n_fdheat()
{
    long nextbackline;
    long frontstride;
    char *back;
    char *front;
    long yy;
    long xsteps;
    long backline;
    long backstride;
    long b2;
    long c1;
    long c2;
    long d1;
    long d2;
    long error;
    long xx;
    long b1;
    long result;
    long cells;
    long frob;
    long total;

    __asm {

// 1846 :   QUAD *front = (QUAD *)frontMem;

    mov ecx, DWORD PTR [this]
    mov eax, DWORD PTR [ecx].frontMem
    mov DWORD PTR [front], eax

// 1847 :   QUAD *back = (QUAD *)(backMem +
// 1848 :                         (backBorder - 1) +
// 1849 :                         ((backBorder - 1) * backRowBytes));

//  mov ecx, DWORD PTR [this]
    mov eax, DWORD PTR [ecx].backBorder
//  mov ecx, DWORD PTR [this]
    mov edx, DWORD PTR [ecx].backMem
    lea eax, DWORD PTR [edx+eax-1]
//  mov ecx, DWORD PTR [this]
    mov edx, DWORD PTR [ecx].backBorder
    sub edx, 1
    mov ecx, DWORD PTR [this]
    imul    edx, DWORD PTR [ecx].backRowBytes
    add eax, edx
    mov DWORD PTR [back], eax

// 1850 :   long backline = backRowBytes >> 2;

//  mov ecx, DWORD PTR [this]
    mov eax, DWORD PTR [ecx].backRowBytes
    sar eax, 2
    mov DWORD PTR [backline], eax

// 1851 :   long nextbackline = backline << 1;

    mov eax, DWORD PTR [backline]
    shl eax, 1
    mov DWORD PTR [nextbackline], eax

// 1852 :   long backstride = backline - ((width >> 2) + 1);

//  mov ecx, DWORD PTR [this]
    mov eax, DWORD PTR [ecx]cellEngine.width
    sar eax, 2
    add eax, 1
    mov edx, DWORD PTR [backline]
    sub edx, eax
    mov DWORD PTR [backstride], edx

// 1853 :   long frontstride = (screenRowBytes >> 2) - (width >> 2);

//  mov ecx, DWORD PTR [this]
    mov eax, DWORD PTR [ecx].screenRowBytes
    sar eax, 2
//  mov ecx, DWORD PTR [this]
    mov edx, DWORD PTR [ecx]cellEngine.width
    sar edx, 2
    sub eax, edx
    mov DWORD PTR [frontstride], eax

// 1854 :   long xsteps = width >> 2;

//  mov eax, DWORD PTR [this]
    mov edx, DWORD PTR [ecx]cellEngine.width
    sar edx, 2
    mov DWORD PTR [xsteps], edx

// 1855 :   long yy;
// 1856 :
// 1857 :   total = 0;

//  mov ecx, DWORD PTR [this]
    mov DWORD PTR [total], 0

//          long frob = this->frob;
    mov eax, DWORD PTR [ecx].frob
    mov [frob], eax

// 1858 :
// 1859 :   for (yy = height;

//  mov ecx, DWORD PTR [this]
    mov eax, DWORD PTR [ecx]cellEngine.height
    mov DWORD PTR [yy], eax

// 1860 :        yy > 0;
// 1861 :        back += backstride,
// 1862 :        front += frontstride,
// 1863 :        yy--) {

    jmp SHORT _L76116

_L76117:
    mov edx, DWORD PTR [backstride]
    mov eax, DWORD PTR [back]
    lea ecx, DWORD PTR [eax+edx*4]
    mov DWORD PTR [back], ecx
    mov edx, DWORD PTR [frontstride]
    mov eax, DWORD PTR [front]
    lea ecx, DWORD PTR [eax+edx*4]
    mov DWORD PTR [front], ecx
    mov edx, DWORD PTR [yy]
    sub edx, 1
    mov DWORD PTR [yy], edx

_L76116:
    cmp DWORD PTR [yy], 0
    jle _L76118

// 1864 :       register QUAD b1, b2;
// 1865 :       register QUAD c1, c2;
// 1866 :       register QUAD d1, d2;
// 1867 :           long error = 0;

    mov DWORD PTR [error], 0

// 1868 :           long xx;
// 1869 :
// 1870 :       b2 = back[0];

    mov eax, DWORD PTR [back]
    mov ecx, DWORD PTR [eax]
    mov DWORD PTR [b2], ecx

// 1871 :       c2 = back[backline];

    mov edx, DWORD PTR [backline]
//  mov eax, DWORD PTR [back] // redundant
    mov ecx, DWORD PTR [eax+edx*4]
    mov DWORD PTR [c2], ecx

// 1872 :       d2 = back[backline << 1];

//  mov edx, DWORD PTR [backline] // redundant
    shl edx, 1
//  mov eax, DWORD PTR [back] // redundant
    mov ecx, DWORD PTR [eax+edx*4]
    mov DWORD PTR [d2], ecx

// 1873 :       back++;

//  mov edx, DWORD PTR [back] // redundant
//  add edx, 4 // redundant
//  mov DWORD PTR [back], edx // redundant
    add eax, 4
    mov DWORD PTR [back], eax

// 1874 :
// 1875 :           for (xx = xsteps;

    mov eax, DWORD PTR [xsteps]
    mov DWORD PTR [xx], eax

// 1876 :            xx > 0;
// 1877 :            xx--) {

    jmp SHORT _L76127

_L76128:
    mov ecx, DWORD PTR [xx]
    sub ecx, 1
    mov DWORD PTR [xx], ecx

_L76127:
    cmp DWORD PTR [xx], 0
    jle _L76129

// 1878 :               long result, cells = 0;

    mov DWORD PTR [cells], 0

// 1879 :
// 1880 :               b1 = b2; b2 = back[0];

    mov edx, DWORD PTR [b2]
    mov DWORD PTR [b1], edx
    mov eax, DWORD PTR [back]
    mov ecx, DWORD PTR [eax]
    mov DWORD PTR [b2], ecx

// 1881 :               c1 = c2; c2 = back[backline];

    mov edx, DWORD PTR [c2]
    mov DWORD PTR [c1], edx
    mov ecx, DWORD PTR [backline]
//  mov eax, DWORD PTR [back]
    mov edx, DWORD PTR [eax+ecx*4]
    mov DWORD PTR [c2], edx

// 1882 :               d1 = d2; d2 = back[backline << 1];

    mov edx, DWORD PTR [d2]
    mov DWORD PTR [d1], edx
//  mov ecx, DWORD PTR [backline]
    shl ecx, 1
//  mov eax, DWORD PTR [back]
    mov edx, DWORD PTR [eax+ecx*4]
    mov DWORD PTR [d2], edx

// 1883 :               back++;

//  mov eax, DWORD PTR [back]
    add eax, 4
    mov DWORD PTR [back], eax

// 1884 :
// 1885 :               FDHEAT( BYTE0(b1), BYTE1(b1), BYTE2(b1),
// 1886 :                   BYTE0(c1), BYTE1(c1), BYTE2(c1),
// 1887 :                   BYTE0(d1), BYTE1(d1), BYTE2(d1) );

    mov edx, DWORD PTR [b1]
    and edx, 255                ; 000000ffH
    mov eax, DWORD PTR [b1]
    sar eax, 8
    and eax, 255                ; 000000ffH
    add edx, eax
    mov ecx, DWORD PTR [b1]
    sar ecx, 16                 ; 00000010H
    and ecx, 255                ; 000000ffH
    add edx, ecx
    mov eax, DWORD PTR [c1]
    and eax, 255                ; 000000ffH
    add edx, eax
//  mov ecx, DWORD PTR [this]
    add edx, DWORD PTR [frob]
    mov eax, DWORD PTR [c1]
    sar eax, 16                 ; 00000010H
    and eax, 255                ; 000000ffH
    add edx, eax
    mov ecx, DWORD PTR [d1]
    and ecx, 255                ; 000000ffH
    add edx, ecx
    mov eax, DWORD PTR [d1]
    sar eax, 8
    and eax, 255                ; 000000ffH
    add edx, eax
    mov ecx, DWORD PTR [d1]
    sar ecx, 16                 ; 00000010H
    and ecx, 255                ; 000000ffH
    add ecx, DWORD PTR [error]
    add ecx, edx
    mov DWORD PTR [error], ecx
    mov edx, DWORD PTR [error]
    sar edx, 3
    mov DWORD PTR [result], edx
    mov eax, DWORD PTR [error]
    and eax, 7
    mov DWORD PTR [error], eax

// 1888 :               total += result;

//  mov ecx, DWORD PTR [this]
    mov edx, DWORD PTR [total]
    add edx, DWORD PTR [result]
//  mov eax, DWORD PTR [this]
    mov DWORD PTR [total], edx

// 1889 :               SETBYTE0(cells, result);

    mov ecx, DWORD PTR [cells]
    and ecx, -256               ; ffffff00H
    mov edx, DWORD PTR [result]
    and edx, 255                ; 000000ffH
    or  ecx, edx
    mov DWORD PTR [cells], ecx

// 1890 :
// 1891 :               FDHEAT( BYTE1(b1), BYTE2(b1), BYTE3(b1),
// 1892 :                   BYTE1(c1), BYTE2(c1), BYTE3(c1),
// 1893 :                   BYTE1(d1), BYTE2(d1), BYTE3(d1) );

    mov eax, DWORD PTR [b1]
    sar eax, 8
    and eax, 255                ; 000000ffH
    mov ecx, DWORD PTR [b1]
    sar ecx, 16                 ; 00000010H
    and ecx, 255                ; 000000ffH
    add eax, ecx
    mov edx, DWORD PTR [b1]
    sar edx, 24                 ; 00000018H
    and edx, 255                ; 000000ffH
    add eax, edx
    mov ecx, DWORD PTR [c1]
    sar ecx, 8
    and ecx, 255                ; 000000ffH
    add eax, ecx
//  mov edx, DWORD PTR [this]
    add eax, DWORD PTR [frob]
    mov ecx, DWORD PTR [c1]
    sar ecx, 24                 ; 00000018H
    and ecx, 255                ; 000000ffH
    add eax, ecx
    mov edx, DWORD PTR [d1]
    sar edx, 8
    and edx, 255                ; 000000ffH
    add eax, edx
    mov ecx, DWORD PTR [d1]
    sar ecx, 16                 ; 00000010H
    and ecx, 255                ; 000000ffH
    add eax, ecx
    mov edx, DWORD PTR [d1]
    sar edx, 24                 ; 00000018H
    and edx, 255                ; 000000ffH
    add edx, DWORD PTR [error]
    add edx, eax
    mov DWORD PTR [error], edx
//  mov eax, DWORD PTR [error]
    mov eax, edx
    sar eax, 3
    mov DWORD PTR [result], eax
//  mov ecx, DWORD PTR [error]
    and edx, 7
    mov DWORD PTR [error], edx

// 1894 :               total += result;

//  mov ecx, DWORD PTR [this]
    mov edx, DWORD PTR [total]
    add edx, DWORD PTR [result]
//  mov eax, DWORD PTR [this]
    mov DWORD PTR [total], edx

// 1895 :               SETBYTE1(cells, result);

    mov edx, DWORD PTR [cells]
    and edx, -65281             ; ffff00ffH
    mov eax, DWORD PTR [result]
    shl eax, 8
    and eax, 65280              ; 0000ff00H
    or  edx, eax
    mov DWORD PTR [cells], edx

// 1896 :
// 1897 :               FDHEAT( BYTE2(b1), BYTE3(b1), BYTE0(b2),
// 1898 :                   BYTE2(c1), BYTE3(c1), BYTE0(c2),
// 1899 :                   BYTE2(d1), BYTE3(d1), BYTE0(d2) );

    mov ecx, DWORD PTR [b1]
    sar ecx, 16                 ; 00000010H
    and ecx, 255                ; 000000ffH
    mov edx, DWORD PTR [b1]
    sar edx, 24                 ; 00000018H
    and edx, 255                ; 000000ffH
    add ecx, edx
    mov eax, DWORD PTR [b2]
    and eax, 255                ; 000000ffH
    add ecx, eax
    mov edx, DWORD PTR [c1]
    sar edx, 16                 ; 00000010H
    and edx, 255                ; 000000ffH
    add ecx, edx
//  mov eax, DWORD PTR [this]
    add ecx, DWORD PTR [frob]
    mov edx, DWORD PTR [c2]
    and edx, 255                ; 000000ffH
    add ecx, edx
    mov eax, DWORD PTR [d1]
    sar eax, 16                 ; 00000010H
    and eax, 255                ; 000000ffH
    add ecx, eax
    mov edx, DWORD PTR [d1]
    sar edx, 24                 ; 00000018H
    and edx, 255                ; 000000ffH
    add ecx, edx
    mov eax, DWORD PTR [d2]
    and eax, 255                ; 000000ffH
    add eax, DWORD PTR [error]
    add eax, ecx
    mov DWORD PTR [error], eax
//  mov ecx, DWORD PTR [error]
    mov ecx, eax
    sar ecx, 3
    mov DWORD PTR [result], ecx
//  mov edx, DWORD PTR [error]
    and eax, 7
    mov DWORD PTR [error], eax

// 1900 :               total += result;

//  mov ecx, DWORD PTR [this]
    mov edx, DWORD PTR [total]
    add edx, DWORD PTR [result]
//  mov eax, DWORD PTR [this]
    mov DWORD PTR [total], edx

// 1901 :               SETBYTE2(cells, result);

    mov eax, DWORD PTR [cells]
    and eax, -16711681              ; ff00ffffH
    mov ecx, DWORD PTR [result]
    shl ecx, 16                 ; 00000010H
    and ecx, 16711680               ; 00ff0000H
    or  eax, ecx
    mov DWORD PTR [cells], eax

// 1902 :
// 1903 :               FDHEAT( BYTE3(b1), BYTE0(b2), BYTE1(b2),
// 1904 :                   BYTE3(c1), BYTE0(c2), BYTE1(c2),
// 1905 :                   BYTE3(d1), BYTE0(d2), BYTE1(d2) );

    mov edx, DWORD PTR [b1]
    sar edx, 24                 ; 00000018H
    and edx, 255                ; 000000ffH
    mov eax, DWORD PTR [b2]
    and eax, 255                ; 000000ffH
    add edx, eax
    mov ecx, DWORD PTR [b2]
    sar ecx, 8
    and ecx, 255                ; 000000ffH
    add edx, ecx
    mov eax, DWORD PTR [c1]
    sar eax, 24                 ; 00000018H
    and eax, 255                ; 000000ffH
    add edx, eax
//  mov ecx, DWORD PTR [this]
    add edx, DWORD PTR [frob]
    mov eax, DWORD PTR [c2]
    sar eax, 8
    and eax, 255                ; 000000ffH
    add edx, eax
    mov ecx, DWORD PTR [d1]
    sar ecx, 24                 ; 00000018H
    and ecx, 255                ; 000000ffH
    add edx, ecx
    mov eax, DWORD PTR [d2]
    and eax, 255                ; 000000ffH
    add edx, eax
    mov ecx, DWORD PTR [d2]
    sar ecx, 8
    and ecx, 255                ; 000000ffH
    add ecx, DWORD PTR [error]
    add ecx, edx
    mov DWORD PTR [error], ecx
//  mov edx, DWORD PTR [error]
    mov edx, ecx
    sar edx, 3
    mov DWORD PTR [result], edx
//  mov eax, DWORD PTR [error]
    and ecx, 7
    mov DWORD PTR [error], ecx

// 1906 :               total += result;

//  mov ecx, DWORD PTR [this]
    mov edx, DWORD PTR [total]
    add edx, DWORD PTR [result]
//  mov eax, DWORD PTR [this]
    mov DWORD PTR [total], edx

// 1907 :               SETBYTE3(cells, result);

    mov ecx, DWORD PTR [cells]
    and ecx, 16777215               ; 00ffffffH
    mov edx, DWORD PTR [result]
    shl edx, 24                 ; 00000018H
    and edx, -16777216              ; ff000000H
    or  ecx, edx
    mov DWORD PTR [cells], ecx

// 1908 :
// 1909 :           *front = cells;

    mov eax, DWORD PTR [front]
    mov ecx, DWORD PTR [cells]
    mov DWORD PTR [eax], ecx

// 1910 :               front++;

//  mov eax, DWORD PTR [front]
    add eax, 4
    mov DWORD PTR [front], eax

// 1911 :           } // for xx

    jmp _L76128

_L76129:

// 1912 :   } // for yy

    jmp _L76117

_L76118:
    mov ecx, DWORD PTR [this]
    mov eax, DWORD PTR [total]
    mov DWORD PTR [ecx].total, eax

    } // asm
}


#endif



#define FABCDHEAT(nw, n, ne, w, c, e, sw, s, se) \
    error += (n + s + w + e + frobby); \
    result = error >> 2; \
    error &= 3; \
    frobby = -frobby;

void CellEngine::n_fabcdheat()
{
    QUAD *front = (QUAD *)frontMem;
    QUAD *back = (QUAD *)(backMem +
                          (backBorder - 1) +
                          ((backBorder - 1) * backRowBytes));
    long frobby = frob;
    long backline = backRowBytes >> 2;
    long nextbackline = backline << 1;
    long backstride = backline - ((width >> 2) + 1);
    long frontstride = (screenRowBytes >> 2) - (width >> 2);
    long xsteps = width >> 2;
    long yy;

    total = 0;

    // flip frob with the spatio-temporal phase,
    // since interlaced checkerboards
    // exchange places each step.
    if ((phase ^ x ^ y) & 1) {
        frobby = -frobby;
    }

    for (yy = height;
         yy > 0;
         back += backstride,
         front += frontstride,
         yy--) {
        register QUAD b1, b2;
        register QUAD c1, c2;
        register QUAD d1, d2;
        long error = 0;
        long xx;

        // Flip frob each y line, to apply opposite frobs
        // to each spatial phase of the checkerboard.
        // This works because the width must be divisible by 4.
        frobby = -frobby;

        b2 = back[0];
        c2 = back[backline];
        d2 = back[nextbackline];
        back++;

        for (xx = xsteps;
             xx > 0;
             xx--) {
            long result, cells = 0;

            b1 = b2; b2 = back[0];
            c1 = c2; c2 = back[backline];
            d1 = d2; d2 = back[nextbackline];
            back++;

            FABCDHEAT( BYTE0(b1), BYTE1(b1), BYTE2(b1),
                       BYTE0(c1), BYTE1(c1), BYTE2(c1),
                       BYTE0(d1), BYTE1(d1), BYTE2(d1) );
            SETBYTE0(cells, result);
            total += result;

            FABCDHEAT( BYTE1(b1), BYTE2(b1), BYTE3(b1),
                       BYTE1(c1), BYTE2(c1), BYTE3(c1),
                       BYTE1(d1), BYTE2(d1), BYTE3(d1) );
            SETBYTE1(cells, result);
            total += result;

            FABCDHEAT( BYTE2(b1), BYTE3(b1), BYTE0(b2),
                       BYTE2(c1), BYTE3(c1), BYTE0(c2),
                       BYTE2(d1), BYTE3(d1), BYTE0(d2) );
            SETBYTE2(cells, result);
            total += result;

            FABCDHEAT( BYTE3(b1), BYTE0(b2), BYTE1(b2),
                       BYTE3(c1), BYTE0(c2), BYTE1(c2),
                       BYTE3(d1), BYTE0(d2), BYTE1(d2) );
            SETBYTE3(cells, result);
            total += result;

            front++;
            *front = cells;
        }
    }
}


#define SUM8R(m, nw, n, ne, w, c, e, sw, s, se) \
    (((nw&m) + (n&m) + (ne&m) + (w&m) + (e&m) + (sw&m) + (s&m) + (se&m)))

#define SUM9R(m, nw, n, ne, w, c, e, sw, s, se) \
    (((nw&m) + (n&m) + (ne&m) + (w&m) + (c&m) + (e&m) + (sw&m) + (s&m) + (se&m)))

#define RISCA(nw, n, ne, w, c, e, sw, s, se) \
    result = c & 0xf0; \
    switch (result) { \
        case 0x00: /* c */ \
            result |= (c & 0x0f) ; \
            break; \
        case 0x10: /* life */ \
            sum = (SUM8R(8, nw, n, ne, w, c, e, sw, s, se) >> 3); \
            result |= ((c & 0x0e) >> 1) | \
                      ((((c & 1) && ((sum == 2) || (sum == 3))) || \
                        (sum == 3)) << 3); \
            break; \
        case 0x20: /* brain */ \
            result |= ((c & 0x0e) >> 1) | \
                      ((((c & 0x03) == 0) && \
                        ((SUM8R(8, nw, n, ne, w, c, e, sw, s, se) >> 3) == 2)) << 3); \
            break; \
        case 0x30: /* torben */ \
            sum = (SUM9R(8, nw, n, ne, w, c, e, sw, s, se) >> 3); \
            result |= ((c & 0x0e) >> 1) | \
                      (((sum > 6) || (sum == 5) || (sum == 3)) << 3); \
            break; \
        case 0x40: /* anneal */ \
            sum = (SUM9R(8, nw, n, ne, w, c, e, sw, s, se) >> 3); \
            result |= ((c & 0x0e) >> 1) | \
                      (((sum > 5) || (sum == 4)) << 3); \
            break; \
        case 0x50: /* ditto */ \
            sum = (SUM9R(8, nw, n, ne, w, c, e, sw, s, se) >> 3); \
            result |= ((c & 0x0e) >> 1) | \
                      ((sum > 5) << 3); \
            break; \
        case 0x60: /* logic */ \
            {   result |= (c & 0x07); \
                switch (c & 0x0f) { \
                    case 0x00: /* and */ \
                    case 0x08: \
                        result |= (nw & w) & 8; \
                        break; \
                    case 0x01: /* or */ \
                    case 0x09: \
                        result |= (nw | w) & 8; \
                        break; \
                    case 0x02: /* xor */ \
                    case 0x0a: \
                        result |= (nw ^ w) & 8; \
                        break; \
                    case 0x03: /* nand */ \
                    case 0x0b: \
                        result |= (~(nw & w)) & 8; \
                        break; \
                    case 0x04: /* nor */ \
                    case 0x0c: \
                        result |= (~(nw | w)) & 8; \
                        break; \
                    case 0x05: /* equiv */ \
                    case 0x0d: \
                        result |= (~(nw ^ w)) & 8; \
                        break; \
                    case 0x06: /* flip state 0 */ \
                        result |= (nw & 8); \
                        break; \
                    case 0x0e: /* flop state 1 */ \
                        result |= ((~w) & 8); \
                        break; \
                    case 0x07: /* relay */ \
                    case 0x0f: \
                        result |= ((w & 8) ? sw : nw) & 8; \
                        break; \
                } \
            } \
            break; \
        case 0x70: /* n */ \
            result |= (n & 0x0f) ; \
            break; \
        case 0x80: /* nw */ \
            result |= (nw & 0x0f) ; \
            break; \
        case 0x90: /* w */ \
            result |= (w & 0x0f) ; \
            break; \
        case 0xa0: /* sw */ \
            result |= (sw & 0x0f) ; \
            break; \
        case 0xb0: /* s */ \
            result |= (s & 0x0f) ; \
            break; \
        case 0xc0: /* se */ \
            result |= (se & 0x0f) ; \
            break; \
        case 0xd0: /* e */ \
            result |= (e & 0x0f) ; \
            break; \
        case 0xe0: /* ne */ \
            result |= (ne & 0x0f) ; \
            break; \
        case 0xf0: /* heat */ \
            leftover += nw + n + ne + w + e + sw + s + se + frob; \
            result |= ((leftover >> 3) & 15); \
            leftover &= 7; \
            break; \
    }


void CellEngine::n_risca()
{
    QUAD *front = (QUAD *)frontMem;
    QUAD *back = (QUAD *)(backMem +
                          (backBorder - 1) +
                          ((backBorder - 1) * backRowBytes));
    long backline = backRowBytes >> 2;
    long nextbackline = backline << 1;
    long backstride = backline - ((width >> 2) + 1);
    long frontstride = (screenRowBytes >> 2) - (width >> 2);
    long xsteps = width >> 2;
    long yy;
    long leftover = 0;

    for (yy = height;
         yy > 0;
         back += backstride,
         front += frontstride,
         yy--) {
        register QUAD b1, b2;
        register QUAD c1, c2;
        register QUAD d1, d2;

        b2 = back[0];
        c2 = back[backline];
        d2 = back[nextbackline];
        back++;

        long xx;
        for (xx = xsteps;
             xx > 0;
             xx--) {
            long result, sum, cells = 0;

            b1 = b2; b2 = back[0];
            c1 = c2; c2 = back[backline];
            d1 = d2; d2 = back[nextbackline];
            back++;

            RISCA( BYTE0(b1), BYTE1(b1), BYTE2(b1),
                   BYTE0(c1), BYTE1(c1), BYTE2(c1),
                   BYTE0(d1), BYTE1(d1), BYTE2(d1) );
            SETBYTE0(cells, result);

            RISCA( BYTE1(b1), BYTE2(b1), BYTE3(b1),
                   BYTE1(c1), BYTE2(c1), BYTE3(c1),
                   BYTE1(d1), BYTE2(d1), BYTE3(d1) );
            SETBYTE1(cells, result);

            RISCA( BYTE2(b1), BYTE3(b1), BYTE0(b2),
                   BYTE2(c1), BYTE3(c1), BYTE0(c2),
                   BYTE2(d1), BYTE3(d1), BYTE0(d2) );
            SETBYTE2(cells, result);

            RISCA( BYTE3(b1), BYTE0(b2), BYTE1(b2),
                   BYTE3(c1), BYTE0(c2), BYTE1(c2),
                   BYTE3(d1), BYTE0(d2), BYTE1(d2) );
            SETBYTE3(cells, result);

            *front = cells;
            front++;
        }
    }
}


void CellEngine::n_insert()
{
    long mask = (frob >> 8) & 0xff;
    long val = frob & 0xff;

#define INSERT (Byte)((c & mask) | val)

    CAM_MASKED_REG_LOOP(INSERT)
}


void CellEngine::n_heaco()
{
    long sum;
    long last = 0;

#define HEACO \
    (Byte)( \
        (sum = nw + n + ne + w + e + sw + s + se + frob + last), \
        (last = sum & 31), \
        (sum = (sum >> 3) & (~3)), \
        (sum | ((c&1) << 1) | \
         (((c&3) == 0) && \
          (SUM8REG(1) == 2))))

    CAM_MASKED_REG_LOOP(HEACO)
}


// This is a thicker version of dithered heat diffusion,
// that exhibits this wonderful marbling behavior when the
// frob is 1, -1, 2, or -2. It averages the center cell
// with the average of the 8 neighbors, so it's thicker
// than dheat, which does not look at the center cell,
// just the average of the 8 neighbors.

#define MARBLE(nw, n, ne, w, c, e, sw, s, se) \
    error += ((c << 3) + nw + n + ne + w + frob + e + sw + s + se); \
    result = error >> 4; \
    error &= 0x0f;

void CellEngine::n_marble()
{
    QUAD *front = (QUAD *)frontMem;
    QUAD *back = (QUAD *)(backMem +
                          (backBorder - 1) +
                          ((backBorder - 1) * backRowBytes));
    long backline = backRowBytes >> 2;
    long backstride = backline - ((width >> 2) + 1);
    long frontstride = (screenRowBytes >> 2) - (width >> 2);
    long xsteps = width >> 2;
    long yy;

    total = 0;

    for (yy = height; yy > 0;
         back += backstride,
         front += frontstride,
         yy--) {
        register QUAD b1, b2;
        register QUAD c1, c2;
        register QUAD d1, d2;
        long error = 0;
        long xx;

        b2 = back[0];
        c2 = back[backline];
        d2 = back[backline << 1];
        back++;

        for (xx = xsteps;
             xx > 0;
             xx--) {
            long result, cells = 0;

            b1 = b2; b2 = back[0];
            c1 = c2; c2 = back[backline];
            d1 = d2; d2 = back[backline << 1];
            back++;

            MARBLE( BYTE0(b1), BYTE1(b1), BYTE2(b1),
                    BYTE0(c1), BYTE1(c1), BYTE2(c1),
                    BYTE0(d1), BYTE1(d1), BYTE2(d1) );
            total += result;
            SETBYTE0(cells, result);

            MARBLE( BYTE1(b1), BYTE2(b1), BYTE3(b1),
                    BYTE1(c1), BYTE2(c1), BYTE3(c1),
                    BYTE1(d1), BYTE2(d1), BYTE3(d1) );
            total += result;
            SETBYTE1(cells, result);

            MARBLE( BYTE2(b1), BYTE3(b1), BYTE0(b2),
                    BYTE2(c1), BYTE3(c1), BYTE0(c2),
                    BYTE2(d1), BYTE3(d1), BYTE0(d2) );
            total += result;
            SETBYTE2(cells, result);

            MARBLE( BYTE3(b1), BYTE0(b2), BYTE1(b2),
                    BYTE3(c1), BYTE0(c2), BYTE1(c2),
                    BYTE3(d1), BYTE0(d2), BYTE1(d2) );
            total += result;
            SETBYTE3(cells, result);

            *front = cells;
            front++;
        }
    }
}


// This is the smooth, undithered version of marble.
// It does not exhibit the marbling behavior, which
// seems to be dampened out because of the accuracy
// lost by not dithering the leftover fractional heat.
// It's still interesting as a thick version of the
// smooth angular undithered heat diffusion.

#define SMARBLE(nw, n, ne, w, c, e, sw, s, se) \
    result = ((c << 3) + nw + n + ne + w + frob + e + sw + s + se) >> 4;

void CellEngine::n_smarble()
{
    QUAD *front = (QUAD *)frontMem;
    QUAD *back = (QUAD *)(backMem +
                          (backBorder - 1) +
                          ((backBorder - 1) * backRowBytes));
    long backline = backRowBytes >> 2;
    long backstride = backline - ((width >> 2) + 1);
    long frontstride = (screenRowBytes >> 2) - (width >> 2);
    long xsteps = width >> 2;
    long yy;

    total = 0;

    for (yy = height;
         yy > 0;
         back += backstride,
         front += frontstride,
         yy--) {
        register QUAD b1, b2;
        register QUAD c1, c2;
        register QUAD d1, d2;
        long xx;

        b2 = back[0];
        c2 = back[backline];
        d2 = back[backline << 1];
        back++;

        for (xx = xsteps;
             xx > 0;
             xx--) {
            long result, cells = 0;

            b1 = b2; b2 = back[0];
            c1 = c2; c2 = back[backline];
            d1 = d2; d2 = back[backline << 1];
            back++;

            SMARBLE( BYTE0(b1), BYTE1(b1), BYTE2(b1),
                     BYTE0(c1), BYTE1(c1), BYTE2(c1),
                     BYTE0(d1), BYTE1(d1), BYTE2(d1) );
            total += result;
            SETBYTE0(cells, result);

            SMARBLE( BYTE1(b1), BYTE2(b1), BYTE3(b1),
                     BYTE1(c1), BYTE2(c1), BYTE3(c1),
                     BYTE1(d1), BYTE2(d1), BYTE3(d1) );
            total += result;
            SETBYTE1(cells, result);

            SMARBLE( BYTE2(b1), BYTE3(b1), BYTE0(b2),
                     BYTE2(c1), BYTE3(c1), BYTE0(c2),
                     BYTE2(d1), BYTE3(d1), BYTE0(d2) );
            total += result;
            SETBYTE2(cells, result);

            SMARBLE( BYTE3(b1), BYTE0(b2), BYTE1(b2),
                     BYTE3(c1), BYTE0(c2), BYTE1(c2),
                     BYTE3(d1), BYTE0(d2), BYTE1(d2) );
            total += result;
            SETBYTE3(cells, result);

            *front = cells;
            front++;
        }
    }
}


// This is a flaming version of marble, which diffuses
// heat upward so it looks like a flame. Think of heat
// diffusion as repeated bluring, or an iterative
// convulution kernel. Normally, all the neighbors are
// averaged with equal weights. In this case, we ignore
// the nw, n, and ne neighbors, and look at the sw, s,
// and se neighbors twice, so heat flows to the north.

#define FARBLE(nw, n, ne, w, c, e, sw, s, se) \
    error += ((c << 3) + sw + se + w + frob + e + sw + s + s + se); \
    result = error >> 4; \
    error &= 0x0f;

void CellEngine::n_farble()
{
    QUAD *front = (QUAD *)frontMem;
    QUAD *back = (QUAD *)(backMem +
                          (backBorder - 1) +
                          ((backBorder - 1) * backRowBytes));
    long backline = backRowBytes >> 2;
    long backstride = backline - ((width >> 2) + 1);
    long frontstride = (screenRowBytes >> 2) - (width >> 2);
    long xsteps = width >> 2;
    long yy;

    total = 0;

    for (yy = height;
         yy > 0;
         back += backstride,
         front += frontstride,
         yy--) {
        register QUAD b1, b2;
        register QUAD c1, c2;
        register QUAD d1, d2;
        long error = 0;
        long xx;

        b2 = back[0];
        c2 = back[backline];
        d2 = back[backline << 1];
        back++;

        for (xx = xsteps;
             xx > 0;
             xx--) {
            long result, cells = 0;

            b1 = b2; b2 = back[0];
            c1 = c2; c2 = back[backline];
            d1 = d2; d2 = back[backline << 1];
            back++;

            FARBLE( BYTE0(b1), BYTE1(b1), BYTE2(b1),
                    BYTE0(c1), BYTE1(c1), BYTE2(c1),
                    BYTE0(d1), BYTE1(d1), BYTE2(d1) );
            total += result;
            SETBYTE0(cells, result);

            FARBLE( BYTE1(b1), BYTE2(b1), BYTE3(b1),
                    BYTE1(c1), BYTE2(c1), BYTE3(c1),
                    BYTE1(d1), BYTE2(d1), BYTE3(d1) );
            total += result;
            SETBYTE1(cells, result);

            FARBLE( BYTE2(b1), BYTE3(b1), BYTE0(b2),
                    BYTE2(c1), BYTE3(c1), BYTE0(c2),
                    BYTE2(d1), BYTE3(d1), BYTE0(d2) );
            total += result;
            SETBYTE2(cells, result);

            FARBLE( BYTE3(b1), BYTE0(b2), BYTE1(b2),
                    BYTE3(c1), BYTE0(c2), BYTE1(c2),
                    BYTE3(d1), BYTE0(d2), BYTE1(d2) );
            total += result;
            SETBYTE3(cells, result);

            *front = cells;
            front++;
        }
    }
}


// This is a wavy version of flaming marble.
// We define four different convolution kernels, that diffuse
// the heat in different directions (nw, n, ne, and side to side),
// and select between then based on the phase (0 to 3).
// What defines the phase?
// There are several interesting defintions.
// Generally, we don't want to change phase too often, and we
// want to do it in interesting patterns so the heat flows nicely.
// The right shifting lowers the frequency of the phase changes.
// The anding with 3 is because there are only 4 phases.
// (y >> 2) & 3:
//      Vertically driven phase.
//      Horizontally synchronized vertical waves.
// ((x + y) >> 2) & 3:
//      Vertically and horizontally driven phase.
//      Sheared diamond synchronized waves.
// ((x + y + c) >> 2) & 3:
//      Vertically, horizontally, and smoothly varying
//      temperature driven phase.
//      Diamonds smoothly shifting phase with the heat contours.
//      Interesting feedback driving heat flow direction by heat.
//
// OOPS: There's a bug in the following code that I'm preserving
// since it still looks nice. I meant to type "xx" instead of "x",
// and "yy" instead of "y", because x and y are the position of the
// upper left corner, not the cell. GARBLEBUG is the old buggy one,
// and GARBLE is the fixed version that is described above.

#define GARBLEBUG(nw, n, ne, w, c, e, sw, s, se) \
    switch (((y + x + c) >> 2) & 3) { \
    case 0: \
        error += (frob + (c << 3) + w + sw + sw + s + s + se + se + e); \
        break; \
    case 1: \
        error += (frob + (c << 3) + w + w + w + sw + sw + s + se + e); \
        break; \
    case 2: \
        error += (frob + (c << 3) + w + w + sw + sw + se + se + e + e); \
        break; \
    case 3: \
        error += (frob + (c << 3) + w + sw + s + se + se + e + e + e); \
        break; \
    } \
    result = error >> 4; \
    error &= 0x0f;

void CellEngine::n_garblebug()
{
    QUAD *front = (QUAD *)frontMem;
    QUAD *back = (QUAD *)(backMem +
                          (backBorder - 1) +
                          ((backBorder - 1) * backRowBytes));
    long backline = backRowBytes >> 2;
    long backstride = backline - ((width >> 2) + 1);
    long frontstride = (screenRowBytes >> 2) - (width >> 2);
    long xsteps = width >> 2;
    long yy;

    total = 0;

    for (yy = height;
         yy > 0;
         back += backstride,
         front += frontstride,
         yy--) {
        register QUAD b1, b2;
        register QUAD c1, c2;
        register QUAD d1, d2;
        long error = 0;
        long xx;

        b2 = back[0];
        c2 = back[backline];
        d2 = back[backline << 1];
        back++;

        for (xx = xsteps;
             xx > 0;
             xx--) {
            long result, cells = 0;

            b1 = b2; b2 = back[0];
            c1 = c2; c2 = back[backline];
            d1 = d2; d2 = back[backline << 1];
            back++;

            GARBLEBUG( BYTE0(b1), BYTE1(b1), BYTE2(b1),
                    BYTE0(c1), BYTE1(c1), BYTE2(c1),
                    BYTE0(d1), BYTE1(d1), BYTE2(d1) );
            total += result;
            SETBYTE0(cells, result);

            GARBLEBUG( BYTE1(b1), BYTE2(b1), BYTE3(b1),
                    BYTE1(c1), BYTE2(c1), BYTE3(c1),
                    BYTE1(d1), BYTE2(d1), BYTE3(d1) );
            total += result;
            SETBYTE1(cells, result);

            GARBLEBUG( BYTE2(b1), BYTE3(b1), BYTE0(b2),
                    BYTE2(c1), BYTE3(c1), BYTE0(c2),
                    BYTE2(d1), BYTE3(d1), BYTE0(d2) );
            total += result;
            SETBYTE2(cells, result);

            GARBLEBUG( BYTE3(b1), BYTE0(b2), BYTE1(b2),
                    BYTE3(c1), BYTE0(c2), BYTE1(c2),
                    BYTE3(d1), BYTE0(d2), BYTE1(d2) );
            total += result;
            SETBYTE3(cells, result);

            *front = cells;
            front++;
        }
    }
}


#define GARBLE(nw, n, ne, w, c, e, sw, s, se) \
    switch (((yy + xx + c) >> 2) & 3) { \
    case 0: \
        error += (frob + (c << 3) + w + sw + sw + s + s + se + se + e); \
        break; \
    case 1: \
        error += (frob + (c << 3) + w + w + w + sw + sw + s + se + e); \
        break; \
    case 2: \
        error += (frob + (c << 3) + w + w + sw + sw + se + se + e + e); \
        break; \
    case 3: \
        error += (frob + (c << 3) + w + sw + s + se + se + e + e + e); \
        break; \
    } \
    result = error >> 4; \
    error &= 0x0f;

void CellEngine::n_garble()
{
    QUAD *front = (QUAD *)frontMem;
    QUAD *back = (QUAD *)(backMem +
                          (backBorder - 1) +
                          ((backBorder - 1) * backRowBytes));
    long backline = backRowBytes >> 2;
    long backstride = backline - ((width >> 2) + 1);
    long frontstride = (screenRowBytes >> 2) - (width >> 2);
    long xsteps = width >> 2;
    long yy;

    total = 0;

    for (yy = height;
         yy > 0;
         back += backstride,
         front += frontstride,
         yy--) {
        register QUAD b1, b2;
        register QUAD c1, c2;
        register QUAD d1, d2;
        long error = 0;
        long xx;

        b2 = back[0];
        c2 = back[backline];
        d2 = back[backline << 1];
        back++;

        for (xx = xsteps;
             xx > 0;
             xx--) {
            long result, cells = 0;

            b1 = b2; b2 = back[0];
            c1 = c2; c2 = back[backline];
            d1 = d2; d2 = back[backline << 1];
            back++;

            GARBLE( BYTE0(b1), BYTE1(b1), BYTE2(b1),
                    BYTE0(c1), BYTE1(c1), BYTE2(c1),
                    BYTE0(d1), BYTE1(d1), BYTE2(d1) );
            total += result;
            SETBYTE0(cells, result);

            GARBLE( BYTE1(b1), BYTE2(b1), BYTE3(b1),
                    BYTE1(c1), BYTE2(c1), BYTE3(c1),
                    BYTE1(d1), BYTE2(d1), BYTE3(d1) );
            total += result;
            SETBYTE1(cells, result);

            GARBLE( BYTE2(b1), BYTE3(b1), BYTE0(b2),
                    BYTE2(c1), BYTE3(c1), BYTE0(c2),
                    BYTE2(d1), BYTE3(d1), BYTE0(d2) );
            total += result;
            SETBYTE2(cells, result);

            GARBLE( BYTE3(b1), BYTE0(b2), BYTE1(b2),
                    BYTE3(c1), BYTE0(c2), BYTE1(c2),
                    BYTE3(d1), BYTE0(d2), BYTE1(d2) );
            total += result;
            SETBYTE3(cells, result);

            *front = cells;
            front++;
        }
    }
}


#define TWOHEATS(nw, n, ne, w, c, e, sw, s, se) \
    error += ((nw&0x0f) + (n&0x0f) + (ne&0x0f) + (w&0x0f) + (frob&0x0f) + (e&0x0f) + (sw&0x0f) + (s&0x0f) + (se&0x0f)); \
    result = (error >> 3) & 0x0f; \
    error &= 7; \
    error += ((nw>>4) + (n>>4) + (ne>>4) + (w>>4) + ((frob>>1)&0x0f) + (e>>4) + (sw>>4) + (s>>4) + (se>>4)); \
    result |= (error<<1) & 0xf0; \
    error &= 7;

void CellEngine::n_twoheats()
{
    QUAD *front = (QUAD *)frontMem;
    QUAD *back = (QUAD *)(backMem +
                          (backBorder - 1) +
                          ((backBorder - 1) * backRowBytes));
    long backline = backRowBytes >> 2;
    long backstride = backline - ((width >> 2) + 1);
    long frontstride = (screenRowBytes >> 2) - (width >> 2);
    long xsteps = width >> 2;
    long yy;

    total = 0;

    for (yy = height;
         yy > 0;
         back += backstride,
         front += frontstride,
         yy--) {
        register QUAD b1, b2;
        register QUAD c1, c2;
        register QUAD d1, d2;
        long error = 0;
        long xx;

        b2 = back[0];
        c2 = back[backline];
        d2 = back[backline << 1];
        back++;

        for (xx = xsteps;
             xx > 0;
             xx--) {
            long result, cells = 0;

            b1 = b2; b2 = back[0];
            c1 = c2; c2 = back[backline];
            d1 = d2; d2 = back[backline << 1];
            back++;

            TWOHEATS( BYTE0(b1), BYTE1(b1), BYTE2(b1),
                      BYTE0(c1), BYTE1(c1), BYTE2(c1),
                      BYTE0(d1), BYTE1(d1), BYTE2(d1) );
            total += result;
            SETBYTE0(cells, result);

            TWOHEATS( BYTE1(b1), BYTE2(b1), BYTE3(b1),
                      BYTE1(c1), BYTE2(c1), BYTE3(c1),
                      BYTE1(d1), BYTE2(d1), BYTE3(d1) );
            total += result;
            SETBYTE1(cells, result);

            TWOHEATS( BYTE2(b1), BYTE3(b1), BYTE0(b2),
                      BYTE2(c1), BYTE3(c1), BYTE0(c2),
                      BYTE2(d1), BYTE3(d1), BYTE0(d2) );
            total += result;
            SETBYTE2(cells, result);

            TWOHEATS( BYTE3(b1), BYTE0(b2), BYTE1(b2),
                      BYTE3(c1), BYTE0(c2), BYTE1(c2),
                      BYTE3(d1), BYTE0(d2), BYTE1(d2) );
            total += result;
            SETBYTE3(cells, result);

            *front = cells;
            front++;
        }
    }
}


void CellEngine::n_spin()
{
    Byte *front = frontMem;
    Byte *back = backMem +
                 (backBorder - 1) +
                 ((backBorder - 1) * backRowBytes);
    Byte *f = front;
    long xx, yy;
//  float jitter = ((float)1.0 * ((float)Rand8() / (float)255.0));
    float jitter = 0.0;
    float angle = ((float)8.0 * (float)frob) + jitter;
    angle += ((float) hubba) * (float)-0.3;
    float rad = (-angle) / (float)(180.0 / 3.14152973);
    float dy = (float)sin(rad);
    float dx = (float)cos(rad);
    float cx = (float)width / (float)2.0;
    float cy = (float)height / (float)2.0;

    for (yy = 0;
         yy < height;
         f += screenRowBytes,
         yy++) {
        for (xx = 0;
             xx < width;
             xx++) {
            float x1, y1;
            long x2, y2;

            x1 = (float)xx - cx;
            y1 = (float)yy - cy;

            x2 = (long)floor(1.5 + (x1 * dx) + (y1 * dy) + cx);
            y2 = (long)floor(1.5 + (y1 * dx) - (x1 * dy) + cy);

            if ((x2 >= 0) &&
                (x2 < width) &&
                (y2 >= 0) &&
                (y2 < height)) {

                f[xx] = back[(y2 * backRowBytes) + x2];
            }
        }
    }
}


// Driven Diffusion by Two Species
// by Richard J. Gaylord and Kazume Nishidate
// Mathematica in Education Research
// Volume 6, Number 2, Spring 1997

// 00 000_____ : empty
// 20 001_____ : empty
// 40 010_____ : wall
// 60 011_____ : mirror
// 80 100_____ : A particle
// a0 101_____ : B particle
// c0 110_____ : C particle
// e0 111_____ : D particle

#define PART_MASK       0xe0
#define ABCD_MASK       0x60
#define PART(x)         ((x) & PART_MASK)
#define ABCD(x)         (((x) & ABCD_MASK) | 0x80)
#define PART_EMPTY      0x00
#define PART_PLASMA     0x20
#define PART_WALL       0x40
#define PART_MIRROR     0x60
#define PART_A          0x80
#define PART_B          0xa0
#define PART_C          0xc0
#define PART_D          0xe0
#define ISPLASMA(x)     (PART(x) == PART_PLASMA)
#define ISWALL(x)       (PART(x) == PART_WALL)
#define ISMIRROR(x)     (PART(x) == PART_MIRROR)
#define ISA(x)          (PART(x) == PART_A)
#define ISB(x)          (PART(x) == PART_B)
#define ISC(x)          (PART(x) == PART_C)
#define ISD(x)          (PART(x) == PART_D)
#define ISABCD(x)       ((x) & 0x80)
//#define ISEMPTY(x)        (((x) & 0xc0) == PART_EMPTY)
#define ISEMPTY(x)      (PART(x) == PART_EMPTY)

// __XXXX__ : payload

#define PAYLOAD_MASK    0x1c
#define PAYLOAD(x)      ((x) & PAYLOAD_MASK)

//  0
// 3 1
//  2
//
// ______00 : north
// ______01 : east
// ______10 : south
// ______11 : west

#define DIR_MASK        0x03
#define DIR(x)          ((x) & DIR_MASK)
#define DIR_N           0x00
#define DIR_E           0x01
#define DIR_S           0x02
#define DIR_W           0x03

#define NOTPART_MASK    (PAYLOAD_MASK | DIR_MASK)
#define NOTPART(x)      ((x) & NOTPART_MASK)


Byte RandDir(Byte x);

Byte RandDir(Byte x)
{
    long b = Rand8() < 128;
    Byte result = 0;

    switch (ABCD(x)) {

    case PART_A:
        result = b ? DIR_N : DIR_W;
        break;

    case PART_B:
        result = b ? DIR_W : DIR_S;
        break;

    case PART_C:
        result = b ? DIR_S : DIR_E;
        break;

    case PART_D:
        result = b ? DIR_E : DIR_N;
        break;

    }

    return PART(x) | result | PAYLOAD(x);
}


void CellEngine::n_driven()
{
    Byte *front = frontMem;
    Byte *back = backMem +
                 backBorder +
                 (backBorder * backRowBytes);
    Byte *f = front, *b = back;
    long xx, yy;
    long heat = 0;
    Byte     nw,  n, ne;
    Byte ww, w,   c,  e, ee;
    Byte     sw,  s, se;

    for (yy = 0;
         yy < height;
         f += screenRowBytes,
         b += backRowBytes,
         yy++) {

        // .. __ .. .. ..
        // __  n ne .. ..
        //  w  c  e ee ..
        // __  s se .. ..
        // .. __ .. .. ..

        n = b[-1 + -backRowBytes];
        ne = b[-backRowBytes];
        w = b[-2];
        c = b[-1];
        e = b[0];
        ee = b[1];
        s = b[-1 + backRowBytes];
        se = b[backRowBytes];

        for (xx = 0;
             xx < width;
             xx++) {
            // .. .. NN .. ..
            // .. nw  n NE ..
            // ww  w  c  e EE
            // .. sw  s SE ..
            // .. .. SS .. ..

            nw = n;
            n = ne;
            ne = b[xx + 1 + -backRowBytes];
            ww = w;
            w = c;
            c = e;
            e = ee;
            ee = b[xx + 2];
            sw = s;
            s = se;
            se = b[xx + 1 + backRowBytes];

            if (ISEMPTY(c)) {

                switch (((ISABCD(n) && (DIR(n) == DIR_S)) ? 1 : 0) +
                        ((ISABCD(e) && (DIR(e) == DIR_W)) ? 2 : 0) +
                        ((ISABCD(s) && (DIR(s) == DIR_N)) ? 4 : 0) +
                        ((ISABCD(w) && (DIR(w) == DIR_E)) ? 8 : 0)) {

                case 1:
                    f[xx] = RandDir(n);
                    break;

                case 2:
                    f[xx] = RandDir(e);
                    break;

                case 4:
                    f[xx] = RandDir(s);
                    break;

                case 8:
                    f[xx] = RandDir(w);
                    break;

                default:
                    // stay the same
                    break;

                }
            } else if (ISPLASMA(c)) {
                heat += (Byte)frob;
                heat += ISEMPTY(nw)     ? c : nw;
                heat += ISEMPTY(n)      ? c : n;
                heat += ISEMPTY(ne)     ? c : ne;
                heat += ISEMPTY(w)      ? c : w;
                heat += ISEMPTY(e)      ? c : e;
                heat += ISEMPTY(sw)     ? c : sw;
                heat += ISEMPTY(s)      ? c : s;
                heat += ISEMPTY(se)     ? c : se;
                f[xx] = PART_PLASMA | NOTPART(heat >> 3);
                heat &= 7;
            } else if (ISWALL(c)) {
                // stay the same
            } else if (ISMIRROR(c)) {
                // stay the same
            } else {
                switch (DIR(c)) {

                case DIR_N:
                    {
                        Byte nn;

                        if (ISMIRROR(n) || ISPLASMA(n)) {
                            f[xx] = RandDir(ABCD(Rand8()) | PAYLOAD(c));
#ifdef DOSOUND
                            SoundEvent::Queue(xx, yy, width, height,
                                              ((c >> 2) & 0x0f) |
                                              ((n & 3) << 4));
#endif
                        } else if ((!ISEMPTY(n)) ||
                            (ISABCD(nw) && (DIR(nw) == DIR_E)) ||
                            (ISABCD(ne) && (DIR(ne) == DIR_W)) ||
                            ((nn = b[xx + (-2 * backRowBytes)]),
                             (ISABCD(nn) && (DIR(nn) == DIR_S)))) {
                            // stay the same
                            f[xx] = RandDir(c);
                        } else {
                            // move north
                            f[xx] = PART_EMPTY | PAYLOAD(c) | DIR(c);
                        }
                        break;
                    }

                case DIR_S:
                    {
                        Byte ss;

                        if (ISMIRROR(s) || ISPLASMA(s)) {
                            f[xx] = RandDir(ABCD(Rand8()) | PAYLOAD(c));
#ifdef DOSOUND
                            SoundEvent::Queue(xx, yy, width, height,
                                              ((c >> 2) & 0x0f) |
                                              ((s & 3) << 4));
#endif
                        } else if ((!ISEMPTY(s)) ||
                            (ISABCD(sw) && (DIR(sw) == DIR_E)) ||
                            (ISABCD(se) && (DIR(se) == DIR_W)) ||
                            ((ss = b[xx + (2 * backRowBytes)]),
                             (ISABCD(ss) && (DIR(ss) == DIR_N)))) {
                            // stay the same
                            f[xx] = RandDir(c);
                        } else {
                            // move south
                            f[xx] = PART_EMPTY | PAYLOAD(c) | DIR(c);
                        }
                        break;
                    }

                case DIR_W:
                    {
                        if (ISMIRROR(w) || ISPLASMA(w)) {
                            f[xx] = RandDir(ABCD(Rand8()) | PAYLOAD(c));
#ifdef DOSOUND
                            SoundEvent::Queue(xx, yy, width, height,
                                              ((c >> 2) & 0x0f) |
                                              ((w & 3) << 4));
#endif
                        } else if ((!ISEMPTY(w)) ||
                            (ISABCD(ww) && (DIR(ww) == DIR_E)) ||
                            (ISABCD(nw) && (DIR(nw) == DIR_S)) ||
                            (ISABCD(sw) && (DIR(sw) == DIR_N))) {
                            // stay the same
                            f[xx] = RandDir(c);
                        } else {
                            // move west
                            f[xx] = PART_EMPTY | PAYLOAD(c) | DIR(c);
                        }
                        break;
                    }

                case DIR_E:
                    {
                        if (ISMIRROR(e) || ISPLASMA(e)) {
                            f[xx] = RandDir(ABCD(Rand8()) | PAYLOAD(c));
#ifdef DOSOUND
                            SoundEvent::Queue(xx, yy, width, height,
                                              ((c >> 2) & 0x0f) |
                                              ((e & 3) << 4));
#endif
                        } else if ((!ISEMPTY(e)) ||
                            (ISABCD(ee) && (DIR(ee) == DIR_W)) ||
                            (ISABCD(ne) && (DIR(ne) == DIR_S)) ||
                            (ISABCD(se) && (DIR(se) == DIR_N))) {
                            // stay the same
                            f[xx] = RandDir(c);
                        } else {
                            // move east
                            f[xx] = PART_EMPTY | PAYLOAD(c) | DIR(c);
                        }
                        break;
                    }
                }
            }
        }
    }

#ifdef DOSOUND
    SoundEvent::PlayQueue(gui);
#endif
}


////////////////////////////////////////////////////////////////////////
// daft

//  0
// 3 1
//  2

// 00 0000____ : north
// 10 0001____ : east
// 20 0010____ : south
// 30 0011____ : west
// 40 0100____ : north mirror
// 50 0101____ : east mirror
// 60 0110____ : south mirror
// 70 0111____ : west mirror
// 80 1000____ : space
// 90 0001____ : magic
// a0 1010____ : metal
// b0 1011____ : microphone
// c0 1100____ : speaker
// d0 1101____ : input
// e0 1110____ : output
// f0 1111____ : random

#define OPCODE_MASK         0xf0
#define OPCODE(x)           ((x) & OPCODE_MASK)

#define OPCODE_NORTH        0x00
#define OPCODE_EAST         0x10
#define OPCODE_SOUTH        0x20
#define OPCODE_WEST         0x30
#define OPCODE_NORTHMIRROR  0x40
#define OPCODE_EASTMIRROR   0x50
#define OPCODE_SOUTHMIRROR  0x60
#define OPCODE_WESTMIRROR   0x70
#define OPCODE_SPACE        0x80
#define OPCODE_MAGIC        0x90
#define OPCODE_METAL        0xa0
#define OPCODE_MICROPHONE   0xb0
#define OPCODE_SPEAKER      0xc0
#define OPCODE_INPUT        0xd0
#define OPCODE_OUTPUT       0xe0
#define OPCODE_RANDOM       0xf0

#define FLOW_MASK           0x70
#define FLOW(x)             ((x) & FLOW_MASK)
#define FLOW_N              0x00
#define FLOW_E              0x10
#define FLOW_S              0x20
#define FLOW_W              0x30

#define IS_FLOWING(x)   ((x) < OPCODE_MAGIC)

#define OPERAND_MASK    0x0f
#define OPERAND(x)      ((x) & OPERAND_MASK)

void CellEngine::n_daft()
{
    Byte *front = frontMem;
    Byte *back = backMem +
                 backBorder +
                 (backBorder * backRowBytes);
    Byte *f = front, *b = back;
    long xx, yy;

    Byte     nw,  n, ne;
    Byte ww, w,   c,  e, ee;
    Byte     sw,  s, se;
    for (yy = 0;
         yy < height;
         f += screenRowBytes,
         b += backRowBytes,
         yy++) {

        // .. __ .. .. ..
        // __  n ne .. ..
        //  w  c  e ee ..
        // __  s se .. ..
        // .. __ .. .. ..

        n = b[-1 + -backRowBytes];
        ne = b[-backRowBytes];
        w = b[-2];
        c = b[-1];
        e = b[0];
        ee = b[1];
        s = b[-1 + backRowBytes];
        se = b[backRowBytes];

        for (xx = 0;
             xx < width;
             xx++) {
            // .. .. NN .. ..
            // .. nw  n NE ..
            // ww  w  c  e EE
            // .. sw  s SE ..
            // .. .. SS .. ..

            nw = n;
            n = ne;
            ne = b[xx + 1 + -backRowBytes];
            ww = w;
            w = c;
            c = e;
            e = ee;
            ee = b[xx + 2];
            sw = s;
            s = se;
            se = b[xx + 1 + backRowBytes];

            switch (OPCODE(c)) {
            case OPCODE_NORTH:
                break;
            case OPCODE_EAST:
                break;
            case OPCODE_SOUTH:
                break;
            case OPCODE_WEST:
                break;
            case OPCODE_NORTHMIRROR:
                break;
            case OPCODE_EASTMIRROR:
                break;
            case OPCODE_SOUTHMIRROR:
                break;
            case OPCODE_WESTMIRROR:
                break;
            case OPCODE_SPACE:
                break;
            case OPCODE_MAGIC:
                break;
            case OPCODE_METAL:
                break;
            case OPCODE_MICROPHONE:
                break;
            case OPCODE_SPEAKER:
                break;
            case OPCODE_INPUT:
                break;
            case OPCODE_OUTPUT:
                break;
            case OPCODE_RANDOM:
                break;
            }
        }
    }

#ifdef DOSOUND
    SoundEvent::PlayQueue(gui);
#endif
}


void CellEngine::n_spinsonly()
{

#define SPINSONLY ( \
    ((((xx + x) ^ (yy + y) ^ phase) & 1) /* spatio-temporal phase */ \
        ? c /* stay the same if true */ \
        : (((SUM4REG(1) != 2) /* can we not flip? */ \
                ? (c & 1) /* no flip */ \
                : ((c ^ 1) & 1) /* do flip */ \
           ) \
          ) \
    ) \
)

    CAM_MASKED_REG_LOOP(SPINSONLY)
}


void CellEngine::n_spinsbank()
{
  long bonds;

#define SPINSBANK ( \
    ((((xx + x) ^ (yy + y) ^ phase) & 1) /* spatio-temporal phase */ \
        ? c /* stay the same if true */ \
        : (((c & 3) ^ /* else xor center with the following */ \
            ((bonds = SUM4REG(1)), \
             (bonds = ((c & 1) /* if center then subtract from 4 */ \
                ? (4 - bonds) \
                : (bonds))), \
             (((bonds == 0) || /* case 0, 4 */ \
               (bonds == 4)) \
                ? 0 /* xor with 0 */ \
                : ((bonds == 2) /* case 2 */ \
                    ? 1 /* xor with 1 */ \
                    : ((bonds == 1) /* case 1 */ \
                        ? ((c & 2) /* can we get from bank? */ \
                            ? 0 /* xor with 0 */ \
                            : 3 /* xor with 3 */ \
                          ) \
                        : ((c & 2) /* can we put to bank? */ \
                            ? 3 /* xor with 3 */ \
                            : 0 /* xor with 0 */ \
                          ) \
                      ) \
                  ) \
             ) \
            ) \
           ) \
          ) \
    ) \
)

    CAM_MASKED_REG_LOOP(SPINSBANK)
}



void CellEngine::n_spinsheat()
{
  long bonds;
  long heat;
  long error = 0;

#define SPINSHEAT ( \
    (error += SUM8REG(0x7f) + frob), \
    (error = ((error < 0) \
        ? 0 \
        : ((error > (0x7f << 3)) \
            ? (0x7f << 3) \
            : error))), \
    (c = (((error >> 3) & 0x7f) | (c & 0x80))), \
    (error &= 0x07), \
    (unsigned char)((((xx + x) ^ (yy + y) ^ phase) & 1) /* spatio-temporal phase */ \
        ? c /* stay the same if true */ \
        : ((bonds = (SUM4REG(0x80) >> 7)), \
           (bonds = ((c & 0x80) /* if center then subtract from 4 */ \
                ? (4 - bonds) \
                : (bonds))), \
           (heat = (c & 0x7f)), \
           (((bonds == 0) || /* case 0, 4 */ \
             (bonds == 4)) \
                ? c /* xor with 0 */ \
                : ((bonds == 2) /* case 2 */ \
                    ? (c ^ 0x80) /* xor with 1 */ \
                    : ((bonds == 1) /* case 1 */ \
                        ? (((heat >= low) && \
                            (heat >= inflation)) /* can we get from bank? */ \
                            ? c /* xor with 0 */ \
                            : ((c ^ 0x80) - inflation) /* flip and debit */ \
                          ) \
                        : (((heat <= high) && \
                            (heat < 0x80 - inflation)) /* can we put to bank? */ \
                            ? ((c ^ 0x80) + inflation) /* flip and credit */ \
                            : c /* xor with 0 */ \
                          ) \
                      ) \
                  ) \
           ) \
          ) \
    ) \
)

    CAM_MASKED_REG_LOOP(SPINSHEAT)
}


void CellEngine::n_spinglass()
{

#define SPINGLASS ( \
    ((((xx + x) ^ (yy + y) ^ phase) & 1) /* spatio-temporal phase */ \
        ? c /* stay the same if true */ \
        : (((((((n >> 7) ^ (c >> 0)) & 1) + \
              (((w >> 7) ^ (c >> 1)) & 1) + \
              (((s >> 7) ^ (s >> 0)) & 1) + \
              (((e >> 7) ^ (e >> 1)) & 1)) != 2) \
            ? (c & 1) /* stay the same if energy != 2 */ \
            : ((c & 1) ^ 0x80) /* flip if energy == 2 */ \
           ) \
          ) \
    ) \
)

    CAM_MASKED_REG_LOOP(SPINGLASS)
}


static Byte GlassBondsTable[16] = {
    0x00,   // 00 -\ 00
            // 00 -/ 00

    0x01,   // 00 -\ 00
            // 10 -/ 10

    0x02,   // 10 -\ 10
            // 00 -/ 00

    0x0c,   // 10 -\ 01
            // 10 -/ 01

    0x04,   // 00 -\ 00
            // 01 -/ 01

    0x0a,   // 00 -\ 11
            // 11 -/ 00

    0x09,   // 10 -\ 01
            // 01 -/ 10

    0x07,   // 10 -\ 10
            // 11 -/ 11

    0x08,   // 01 -\ 01
            // 00 -/ 00

    0x06,   // 01 -\ 10
            // 10 -/ 01

    0x05,   // 11 -\ 00
            // 00 -/ 11

    0x0b,   // 11 -\ 11
            // 10 -/ 10

    0x03,   // 01 -\ 10
            // 01 -/ 10

    0x0d,   // 01 -\ 01
            // 11 -/ 11

    0x0e,   // 11 -\ 11
            // 01 -/ 01

    0x0f    // 11 -\ 11
            // 11 -/ 11

};


void CellEngine::n_glassbonds()
{

    // 0    1    2    3
    // c    cw   ccw  opp
    // 0x1  0x2  0x4  0x8

#define MARG \
    (phase \
        ? (((xx + x) & 1) \
            ? (((yy + y) & 1) \
                ? ( /* p1 x1 y1 = c e s se */ \
                    ((c & 0x80) >> 7) |  \
                    ((e & 0x80) >> 6) | \
                    ((s & 0x80) >> 5) | \
                    ((se & 0x80) >> 4) \
                  ) \
                : ( /* p1 x1 y0 = c n e ne */ \
                    ((c & 0x80) >> 7) |  \
                    ((n & 0x80) >> 6) | \
                    ((e & 0x80)>> 5) | \
                    ((ne & 0x80) >> 4) \
                  ) \
              ) \
            : (((yy + y) & 1) \
                ? ( /* p1 x0 y1 = c s w sw */ \
                    ((c & 0x80) >> 7) |  \
                    ((s & 0x80) >> 6) | \
                    ((w & 0x80) >> 5) | \
                    ((sw & 0x80) >> 4) \
                  ) \
                : ( /* p1 x0 y0 = c w n nw */ \
                    ((c & 0x80) >> 7) |  \
                    ((w & 0x80) >> 6) | \
                    ((n & 0x80) >> 5) | \
                    ((nw & 0x80) >> 4) \
                  ) \
              ) \
          ) \
        : (((xx + x) & 1) \
            ? (((yy + y) & 1) \
                ? ( /* p0 x1 y1 = c w n nw */ \
                    ((c & 0x80) >> 7) |  \
                    ((w & 0x80) >> 6) | \
                    ((n & 0x80) >> 5) | \
                    ((nw & 0x80) >> 4) \
                  ) \
                : ( /* p0 x1 y0 = c s w sw */ \
                    ((c & 0x80) >> 7) |  \
                    ((s & 0x80) >> 6) | \
                    ((w & 0x80) >> 5) | \
                    ((sw & 0x80) >> 4) \
                  ) \
              ) \
            : (((yy + y) & 1) \
                ? ( /* p0 x0 y1 = c n e ne */ \
                    ((c & 0x80) >> 7) |  \
                    ((n & 0x80) >> 6) | \
                    ((e & 0x80) >> 5) | \
                    ((ne & 0x80) >> 4) \
                  ) \
                : ( /* p0 x0 y0 = c e s se */ \
                    ((c & 0x80) >> 7) |  \
                    ((e & 0x80) >> 6) | \
                    ((s & 0x80) >> 5) | \
                    ((se & 0x80) >> 4) \
                  ) \
              ) \
          ) \
    )

#define GLASSBONDS ( \
    ((c  /* >> 1*/ ) & 0x7f) | \
    ((GlassBondsTable[MARG] & 1) << 7) \
)

    CAM_MASKED_REG_LOOP(GLASSBONDS)

}


// 0    1    2    3
// c    cw   ccw  opp
// 0x1  0x2  0x4  0x8

// cw opp   2 8
// c  ccw   1 4

static Byte GlassHeatTable[16] = {

    0x00,   // 00 -\ 00
            // 00 -/ 00

    0x03,   // 00 -\ 00   00
            // 10 -/ 10 ? 00

    0x00,   // 10 -\ 10
            // 00 -/ 00

    0x00,   // 10 -\ 01
            // 10 -/ 01

    0x00,   // 00 -\ 00
            // 01 -/ 01

    0x00,   // 00 -\ 11
            // 11 -/ 00

    0x01,   // 10 -\ 01
            // 01 -/ 10

    0x01,   // 10 -\ 10
            // 11 -/ 11

    0x00,   // 01 -\ 01
            // 00 -/ 00

    0x00,   // 01 -\ 10
            // 10 -/ 01

    0x01,   // 11 -\ 00
            // 00 -/ 11

    0x01,   // 11 -\ 11
            // 10 -/ 10

    0x01,   // 01 -\ 10
            // 01 -/ 10

    0x01,   // 01 -\ 01
            // 11 -/ 11

    0x02,   // 11 -\ 11   11
            // 01 -/ 01 ? 11

    0x01    // 11 -\ 11
            // 11 -/ 11

};


void CellEngine::n_glassheat()
{
    int code;
    int error = 0;

#define GLASSHEAT ( \
    (code = GlassHeatTable[MARG]), \
    (error += SUM8REG(0x7f) + frob), \
    (error = ((error < 0) \
        ? 0 \
        : ((error > (0x7f << 3)) \
            ? (0x7f << 3) \
            : error))), \
    ((code == 2) \
        ? (((error >= low) && \
            (error >= inflation)) /* can we get from bank? */ \
            ? (error -= inflation, code ^= 1) /* flip and debit */  \
            : 0 \
          ) \
        : ((code == 3) \
            ? (((error <= high) && \
                (error < (0x80 << 3) - inflation)) /* can we put to bank? */ \
                ? (error += inflation, code ^= 1) /* flip and credit */ \
                : 0 \
              ) \
            : 0 \
          ) \
    ), \
    (code = ((code << 7) & 0x80) | \
            ((error >> 3) & 0x7f)), \
    (error &= 0x07), \
    code \
)

    CAM_MASKED_REG_LOOP(GLASSHEAT)

}


#if 0

FUNCTION JCRule(Oldstate,NW,N,NE,W,Self,E,SW,S,SE:integer):integer;
CONST
     RN=127; L=2; U=2; K=2; Y=2;
VAR     EightSum,NewState:integer;
BEGIN  {Function}
    EightSum:=NW+N+NE+E+SE+S+SW+W;     ]
    NewState:=0;
     IF (OldState=0) AND
         (L<=EightSum) AND (EightSum<=U) THEN
        NewState:=1;
    IF (OldState=1) THEN
        IF (K<=EightSum) AND (EightSum<=Y) THEN
           NewState:=1
        ELSE
           NewState:=2;
    IF NOT(odd(OldState)) AND
       (0<OldState)AND(OldState<2*RN) THEN
        NewState:=OldState+2;
     JCRule:=NewState
END;  {Function}

#endif


void CellEngine::n_faders()
{
  long sum;
  long state;

#define FADERS ( \
    (sum = SUM8REG(1)), \
    ( ((c == 0) && (sum == 2)) \
        ? (state = 1) \
        : (state = 0)), \
    ( (c == 1) \
        ? ((sum == 2) \
            ? (state = 1) \
            : (state = 2)) \
        : 0), \
    ( (((c & 1) == 0) && \
       (0 < c) && \
       (c < (unsigned char)(frob & 0xff))) \
        ? (state = c + 2) \
        : 0), \
    (unsigned char)state)

    CAM_MASKED_REG_LOOP(FADERS)
}


// This is a wavy version of flaming marble.
// We define four different convolution kernels, that diffuse
// the heat in different directions (nw, n, ne, and side to side),
// and select between then based on the phase (0 to 3).
// What defines the phase?
// There are several interesting defintions.
// Generally, we don't want to change phase too often, and we
// want to do it in interesting patterns so the heat flows nicely.
// The right shifting lowers the frequency of the phase changes.
// The anding with 3 is because there are only 4 phases.
// (y >> 2) & 3:
//      Vertically driven phase.
//      Horizontally synchronized vertical waves.
// ((x + y) >> 2) & 3:
//      Vertically and horizontally driven phase.
//      Sheared diamond synchronized waves.
// ((x + y + c) >> 2) & 3:
//      Vertically, horizontally, and smoothly varying
//      temperature driven phase.
//      Diamonds smoothly shifting phase with the heat contours.
//      Interesting feedback driving heat flow direction by heat.

// New interactive harble using joystick hubba.

#define HARBLE(nw, n, ne, w, c, e, sw, s, se) \
    switch ((((xx - yy) >> 3) + (((xx - yy * 2 - 1) ^ (xx * 2 + yy + 1)) >> 2) + ((c + (hubba >> 4)) >> 2)) & 7) { \
    case 0: \
        error += (frob + (c << 3) + w + sw + s + s + s + s + se + e); \
        break; \
    case 1: \
        error += (frob + (c << 3) + nw + w + sw + sw + sw + sw + s + se); \
        break; \
    case 2: \
        error += (frob + (c << 3) + n + nw + w + w + w + w + sw + s); \
        break; \
    case 3: \
        error += (frob + (c << 3) + ne + n + nw + nw + nw + nw + w + sw); \
        break; \
    case 4: \
        error += (frob + (c << 3) + e + ne + n + n + n + n + nw + w); \
        break; \
    case 5: \
        error += (frob + (c << 3) + se + e + ne + ne + ne + ne + n + nw); \
        break; \
    case 6: \
        error += (frob + (c << 3) + s + se + e + e + e + e + ne + n); \
        break; \
    case 7: \
        error += (frob + (c << 3) + sw + s + se + se + se + se + e + ne); \
        break; \
    } \
    result = error >> 4; \
    error &= 0x0f;

void CellEngine::n_harble()
{
    QUAD *front = (QUAD *)frontMem;
    QUAD *back = (QUAD *)(backMem +
                          (backBorder - 1) +
                          ((backBorder - 1) * backRowBytes));
    long backline = backRowBytes >> 2;
    long backstride = backline - ((width >> 2) + 1);
    long frontstride = (screenRowBytes >> 2) - (width >> 2);
    long xsteps = width >> 2;
    long yy;

    total = 0;

    for (yy = height;
         yy > 0;
         back += backstride,
         front += frontstride,
         yy--) {
        register QUAD b1, b2;
        register QUAD c1, c2;
        register QUAD d1, d2;
        long error = 0;
        long xx;

        b2 = back[0];
        c2 = back[backline];
        d2 = back[backline << 1];
        back++;

        for (xx = xsteps;
             xx > 0;
             xx--) {
            long result, cells = 0;

            b1 = b2; b2 = back[0];
            c1 = c2; c2 = back[backline];
            d1 = d2; d2 = back[backline << 1];
            back++;

            HARBLE( BYTE0(b1), BYTE1(b1), BYTE2(b1),
                    BYTE0(c1), BYTE1(c1), BYTE2(c1),
                    BYTE0(d1), BYTE1(d1), BYTE2(d1) );
            total += result;
            SETBYTE0(cells, result);

            HARBLE( BYTE1(b1), BYTE2(b1), BYTE3(b1),
                    BYTE1(c1), BYTE2(c1), BYTE3(c1),
                    BYTE1(d1), BYTE2(d1), BYTE3(d1) );
            total += result;
            SETBYTE1(cells, result);

            HARBLE( BYTE2(b1), BYTE3(b1), BYTE0(b2),
                    BYTE2(c1), BYTE3(c1), BYTE0(c2),
                    BYTE2(d1), BYTE3(d1), BYTE0(d2) );
            total += result;
            SETBYTE2(cells, result);

            HARBLE( BYTE3(b1), BYTE0(b2), BYTE1(b2),
                    BYTE3(c1), BYTE0(c2), BYTE1(c2),
                    BYTE3(d1), BYTE0(d2), BYTE1(d2) );
            total += result;
            SETBYTE3(cells, result);

            *front = cells;
            front++;
        }
    }
}


// From Intel's paper on the Perlin noise function at:
// http://www.pentium.com/drg/mmx/appnotes/proctex.htm
//
// Procedure C_Noise()
// Inputs: u_init, v_init: starting u and v parameters into the image.
//        du, dv: the incremental change to u_init and v_init.
//        ddu, ddv: the incremental change to du and dv.
//        Num_Pix: number of pixels in the scan line to draw.
//        screen_buffer: Pointer to the screen buffer.
// Output: 16 bit pixels are drawn to the screen.

#define random1(i)  ((i * i) & 65535)        //GREAT! 1 MMX(tm) instr
#define random2(i)  (((i * i) & 65535) >> 2)

static void C_Noise(
    long u_init,
    long v_init,
    long du,
    long dv,
    long ddu,
    long ddv,
    unsigned char *screen_buffer,
    unsigned int count)
{
  unsigned char color;
  unsigned char bx0, bx1, by0, by1;
  unsigned char rx0, ry0;

  // Original Perlin's noise program used an array g[512][2].
  // This program replaces the array with real-time calculations.
  // The results are stored into variables of the form g_b##_#.

  short g_b00_0; // Used to replace array g[b00][0]
  short g_b00_1; // Used to replace array g[b00][1]
  short g_b01_0; // replaces array g[b01][0]
  short g_b01_1; // replaces array g[b01][1]
  short g_b10_0; // replaces array g[b10][0]
  short g_b10_1; // replaces array g[b10][1]
  short g_b11_0; // replaces array g[b11][0]
  short g_b11_1; // replaces array g[b11][1]

  // Used to replace array: b00 = p[p[bx0] + by0];
  short b00;
  short b01;
  short b10;
  short b11;

  unsigned short i;
  unsigned short u_16bit, v_16bit;
  signed   short rx1, ry1;
  signed   short sx, sy;
  signed   long color_p0, color_p1, color_p2, color_p3;
  signed   long color_y0, color_y1;

  // Inner loop for the scan line.  "count" pixels will be drawn.
  for (i = 0; i < count; i++) {
    // Convert the u and v parameters from 10.22 to 8.8 format.
    u_16bit = (short)(u_init >> 14);
    v_16bit = (short)(v_init >> 14);

    // Imagine having a square of the type
    //  p0---p1    Where p0 = (bx0, by0)   +----> U
    //  |(u,v)|          p1 = (bx1, by0)   |
    //  |     |          p2 = (bx0, by1)   |    Coordinate System
    //  p2---p3          p3 = (bx1, by1)   V
    // The u, v point in 2D texture space is bounded by this rectangle.

    // Goal: determine the color of the points p0, p1, p2, p3.
    // Then the color of the point (u, v) will be found by linear interpolation.

    // First step:  Get the 2D coordinates of the points p0, p1, p2, p3.

    // Same as Perlin's original code except floating point
    // is converted over to fixed integer.

    // Process the x component.
    bx0 = u_16bit >> 8;     // Integer part of u, x component of p0 and p2.
    bx1 = bx0 + 1;          // Integer part of u + 1, x component of p1 and p3.

    // Process the y component.
    by0 = v_16bit >> 8;     // Integer part of v, y component of p0 and p1.
    by1 = by0 + 1;          // Integer part of v + 1, y component of p2 and p3.

    // Next, we need vectors pointing from each point in the square above and
    // ending at the (u,v) coordinate located inside the square.
    // The vector (rx0, ry0) goes from P0 to the (u,v) coordinate.
    // The vector (rx1, ry0) goes from P1 to the (u,v) coordinate.
    // The vector (rx0, ry1) goes from P2 to the (u,v) coordinate.
    // The vector (rx1, ry1) goes from P3 to the (u,v) coordinate.

    // Process the x component.
    rx0 = u_16bit & 255;    // Fractional part of u. (0.0 <= rx0 < 1.0) or (0 <= rx0 < 255) for fixed math.
    rx1 = rx0 - 256;        // Fractional part of u + 1. (-1.0 < rx1 <= 0.0) or (-255 < rx1 <= 0) for fixed math.

    // Process the y component.
    ry0 = v_16bit & 255;    // Fractional part of v. (0.0 <= ry0 < 1.0) or (0 <= ry0 < 255) for fixed math.
    ry1 = ry0 - 256;        // Fractional part of v + 1. (-1.0 < rx1 <= 0.0) or (-255 < rx1 <= 0) for fixed math.

    // Next, calculate the dropoff component about the point p0.

    // Same as Perlin's original code except floating point
    // is converted over to fixed integer.  The ">> 1" is used to
    // avoid overflow when the values are multiplied together.
    // This is not a problem in "C" but will be in the MMX implementation.

    sx = (((rx0 * rx0) >> 1) * ((1536 - (rx0 << 2)))) >> 16;
    sy = (((ry0 * ry0) >> 1) * ((1536 - (ry0 << 2)))) >> 16;

    // Now, for each point p0, p1, p2, p3 in the square above, image having
    // a unit gradiant vector pointing in any random direction.  This step
    // attaches a unit gradiant vector to each point of the square.  This is
    // done by precalculating 256, random, uniform, unit vectors.  Then to attach
    // the gradiant vector to a point, an index into the array is needed.  The
    // index is acquired from the x and y coordinates of the square corner point.
    // The algorithm used is called "Folding Over".
    // b00, b10, b01, and b11 contain indexes for a gradiant vector for each
    // corner of the square shown above.

    // This is where the code differs from Perlins.
    // Rather than use arrays p[] and g[][], the values are
    // calculated real-time.  Here random1() replaces array p[].
    // Perlin's equivalent: b00 = p[p[bx0] + by0];

    b00 = random1((random1(bx0) + by0));
    b01 = random1((random1(bx0) + by1));
    b10 = random1((random1(bx1) + by0));
    b11 = random1((random1(bx1) + by1));

    // Here, random2() replaces array g[][].
    // Perlin's equivalent: g_b00_0 = g[b00][0];

    g_b00_0 = (random2(b00) & 511) - 256;
    g_b01_0 = (random2(b01) & 511) - 256;
    g_b10_0 = (random2(b10) & 511) - 256;
    g_b11_0 = (random2(b11) & 511) - 256;

    g_b00_1 = (random2((b00 + 1)) & 511) - 256;
    g_b01_1 = (random2((b01 + 1)) & 511) - 256;
    g_b10_1 = (random2((b10 + 1)) & 511) - 256;
    g_b11_1 = (random2((b11 + 1)) & 511) - 256;

    // Now, for each point in the square shown above, calculate the dot
    // product of the gradiant vector and the vector going from each square
    // corner point to the (u,v) point inside the square.

    // Same as Perlin's original code.

    color_p0 = rx0 * g_b00_0 + ry0 * g_b00_1;
    color_p1 = rx1 * g_b10_0 + ry0 * g_b10_1;
    color_p2 = rx0 * g_b01_0 + ry1 * g_b01_1;
    color_p3 = rx1 * g_b11_0 + ry1 * g_b11_1;

    // color_p0, color_p1, ... are the colors of the points p0, p1, p2, p3.
    // Now use linear interpolation to get the color of the point (sx, sy) inside
    // the square.

    // Interpolation along the X axis.

    color_y0 = color_p0 + sx * ((color_p1 - color_p0) >> 8);
    color_y1 = color_p2 + sx * ((color_p3 - color_p2) >> 8);

    // Interpolation along the Y axis.
    // Output is from -256 to +256, so a 256 color offset is added and
    // the final result is divided by 2.  (>>1).  (0 <= next_color < 256).

    // Same as Perlin's original code except the output is
    // converted from fixed point to regular integer.  Also
    // since the output ranges from -256 to +256, a 256 offest
    // is added to make the range from 0 to 511.  This offset
    // is the 65536 value.  Then the 0 to 511 is scaled down.
    // to a range of 0 to 255.

    color =
        (unsigned char)
            ((color_y0 +
              65536 +
              (sy * ((color_y1 - color_y0) >> 8))
             ) >> 9);

    *(screen_buffer) = color;

    u_init += du;    // New u for calc the color of the next pixel
    v_init += dv;    // New v for calc the color of the next pixel
    du += ddu;       // New du for calc the color of the next pixel
    dv += ddv;       // New dv for calc the color of the next pixel

    screen_buffer++; // Advance 1 pixel.
  } // for x

} // C_Noise()


void CellEngine::n_perlin()
{
    unsigned char *front = frontMem;
    long yy;
    long u = pointax;
    long v = pointay;
    long du = pointbx * frob;
    long dv = pointby * frob;
    long ddu = pointcx;
    long ddv = pointcy;

    for (yy = 0;
         yy < height;
         front += screenRowBytes,
         yy++) {

        C_Noise(
            u,
            v,
            du,
            dv,
            ddu,
            ddv,
            front,
            width);

        // Step to the next scan line, by adding du,dv vector rotated 90 degrees to u,v

        u += dv;
        v -= du;
    } // for yy
}


inline unsigned char MARG_RULE_DIFFUSION(
    unsigned char c,
    unsigned char cw,
    unsigned char ccw,
    unsigned char opp,
    unsigned char r,
    unsigned char p)
{
    unsigned char result = 0;
    unsigned char wall =
        (c | cw | ccw | opp) & 2;
    if (wall) {
        if (c & 3) {
            result = 2;
        } // if
    } else {
        result =
            (((r & 1) ? cw : ccw) & 1) |
            (c & 2);
    } // if

    return result;
}


unsigned char MARG_RULE_GAS(
    unsigned char c,
    unsigned char cw,
    unsigned char ccw,
    unsigned char opp,
    unsigned char r,
    unsigned char p)
{
    unsigned char result = 0;
    unsigned char collision =
        ((c & 1) == (opp & 1)) &&
        ((cw & 1) == (ccw & 1)) &&
        ((c & 1) != (cw & 1));
    unsigned char frozen =
        (c | cw | ccw | opp) & 2;
    if (frozen) {
        if (c & 3) {
            result = 2;
        } // if
    } else if (collision) {
        result = cw;
    } else {
        result = opp;
    } // if

    return result;
}


void CellEngine::n_dendrite()
{
    Byte *front = frontMem;
    unsigned char *nw;
    unsigned char *ne;
    unsigned char *sw;
    unsigned char *se;
    unsigned char *data = front;
    int xx, yy;
    // nw  ne
    // sw  se
    for (yy = 0; yy < height; yy += 2) {
        if (phase == 0) {
            nw = data;
            ne = nw + 1;
            sw = data + screenRowBytes;
            se = sw + 1;
        } else {
            se = data;
            sw = se + (width - 1);
            ne = data +
                 ((yy == 0)
                    ? ((height - 1) * screenRowBytes)
                    : (-screenRowBytes));
            nw = ne + (width - 1);
        } // if

        for (xx = 0; xx < width; xx += 2) {
            unsigned char nw0 = *nw;
            unsigned char ne0 = *ne;
            unsigned char sw0 = *sw;
            unsigned char se0 = *se;
            unsigned char r = (unsigned char)Rand8();

            // nw  ne
            // sw  se

            // MARG_RULE(c, cw, ccw, opp, r, phase)

#if 1
            unsigned char collision =
                ((nw0 & 1) == (ne0 & 1)) &&
                ((sw0 & 1) == (se0 & 1)) &&
                ((nw0 & 1) != (sw0 & 1));
            unsigned char frozen =
                (nw0 | ne0 | sw0 | se0) & 128;
            unsigned char nw1, ne1, sw1, se1;
            // nw  ne
            // sw  se
            if (frozen) {
                int t = ticks >> 4;
                nw1 =
                    (nw0 == 0)
                        ? 0
                        : ((nw0 & 128)
                            ? nw0
                            : (t | 128));
                ne1 =
                    (ne0 == 0)
                        ? 0
                        : ((ne0 & 128)
                            ? ne0
                            : (t | 128));
                sw1 =
                    (sw0 == 0)
                        ? 0
                        : ((sw0 & 128)
                            ? sw0
                            : (t | 128));
                se1 =
                    (se0 == 0)
                        ? 0
                        : ((se0 & 128)
                            ? se0
                            : (t | 128));

#if 1
                int count =
                    ((nw1 & 128) + (ne1 & 128) +
                     (sw1 & 128) + (se1 & 128)) >> 7;

                if (count > 0) {
//                  static int countHeatMod[5] = { 0, -20, -5, 10, 30 };
                    static int countHeatMod[5] = { 0, -1, 0, 0, 0 };
                    int avg =
                        ((((nw1 & 128) ? (nw1 & 127) : 0) +
                          ((ne1 & 128) ? (ne1 & 127) : 0) +
                          ((sw1 & 128) ? (sw1 & 127) : 0) +
                          ((se1 & 128) ? (se1 & 127) : 0) +
//                        countHeatMod[count] +
//                        frob +
                          0) / count) +
                         frob +
                         countHeatMod[count] +
                         0;
                    if (nw1 & 128) {
                        nw1 = avg | 128; // (((nw1 & 127) + avg) >> 1) | 128;
                    } // if
                    if (ne1 & 128) {
                        ne1 = avg | 128; // (((ne1 & 127) + avg) >> 1) | 128;
                    } // if
                    if (sw1 & 128) {
                        sw1 = avg | 128; // (((sw1 & 127) + avg) >> 1) | 128;
                    } // if
                    if (se1 & 128) {
                        se1 = avg | 128; // (((se1 & 127) + avg) >> 1) | 128;
                    } // if
                } // if
#endif

            } else if (collision) {
                if (r & 16) {
                    // Rotate clockwise
                    nw1 = sw0;
                    ne1 = nw0;
                    se1 = ne0;
                    sw1 = se0;
                } else {
                    // Rotate counter-clockwise
                    nw1 = ne0;
                    ne1 = se0;
                    se1 = sw0;
                    sw1 = nw0;
                } // if
            } else {
                if (r & 7) {
                    // Flip opposite
                    nw1 = se0;
                    ne1 = sw0;
                    se1 = nw0;
                    sw1 = ne0;
                } else {
                    if (r & 16) {
                        // Rotate clockwise
                        nw1 = sw0;
                        ne1 = nw0;
                        se1 = ne0;
                        sw1 = se0;
                    } else {
                        // Rotate counter-clockwise
                        nw1 = ne0;
                        ne1 = se0;
                        se1 = sw0;
                        sw1 = nw0;
                    } // if
                } // if
            } // if
#else
            // c   cw
            // ccw opp
            unsigned char nw1 = MARG_RULE_GAS(nw0, ne0, sw0, se0, r, phase);

            // ccw c
            // opp cw
            unsigned char ne1 = MARG_RULE_GAS(ne0, se0, nw0, sw0, r, phase);

            // opp ccw
            // cw  c
            unsigned char se1 = MARG_RULE_GAS(se0, sw0, ne0, nw0, r, phase);

            // cw  opp
            // c   ccw
            unsigned char sw1 = MARG_RULE_GAS(sw0, nw0, se0, ne0, r, phase);

#endif

#if 0
            int nwgas =  ((nw1 > 0) && (nw1 < 128));
            int negas =  ((ne1 > 0) && (ne1 < 128));
            int swgas =  ((sw1 > 0) && (sw1 < 128));
            int segas =  ((se1 > 0) && (se1 < 128));
            int gasses = nwgas + negas + swgas + segas;
            if (gasses > 1) {
                int total =
                    (nwgas ? nw1 : 0) +
                    (negas ? ne1 : 0) +
                    (swgas ? sw1 : 0) +
                    (segas ? se1 : 0) +
                    leftover;
                if ((Rand8() & 15) == 0) {
                    total += frob;
                } // if
                int avg =
                    (total / gasses) & 127;
                leftover =
                    total % gasses;
                while (avg <= 0) avg += 127;
                if (nwgas) nw1 = avg;
                if (negas) ne1 = avg;
                if (swgas) sw1 = avg;
                if (segas) se1 = avg;
            } // if
#endif

            *nw = nw1;
            *ne = ne1;
            *sw = sw1;
            *se = se1;

            nw = ne + 1;
            sw = se + 1;
            ne = ne + 2;
            se = se + 2;
        } // for xx

        data +=
            2 * screenRowBytes;

    } // for yy
}


void CellEngine::n_vanneal()
{
    long sum;

    if (total < hubba) {
        inflation = 1;
    } else if (total == hubba) {
        inflation = 0;
    } else {
        inflation = -1;
    } // if

    total = 0;

#define VANNEAL ( \
    (((((sum = inflation + SUM9REG(1)) > 5) || (sum == 4))) ? (total++, 1) : 0) | \
    ((c&0x7f) << 1))

    CAM_MASKED_REG_LOOP(VANNEAL)
}


void CellEngine::n_vanneal8()
{
    long sum;

    if (total < hubba) {
        inflation = 1;
    } else if (total == hubba) {
        inflation = 0;
    } else {
        inflation = -1;
    } // if

    total = 0;

#define VANNEAL8 ( \
    ((((sum = inflation + SUM9REG(1)) > 5) || (sum == 4)) ? (total++, 1) : 0) | \
    ((((sum = inflation + (SUM9REG(2) >> 1)) > 5) || (sum == 4)) ? (total++, 2) : 0) | \
    ((((sum = inflation + (SUM9REG(4) >> 2)) > 5) || (sum == 4)) ? (total++, 4) : 0) | \
    ((((sum = inflation + (SUM9REG(8) >> 3)) > 5) || (sum == 4)) ? (total++, 8) : 0) | \
    ((((sum = inflation + (SUM9REG(16) >> 4)) > 5) || (sum == 4)) ? (total++, 16) : 0) | \
    ((((sum = inflation + (SUM9REG(32) >> 5)) > 5) || (sum == 4)) ? (total++, 32) : 0) | \
    ((((sum = inflation + (SUM9REG(64) >> 6)) > 5) || (sum == 4)) ? (total++, 64) : 0) | \
    ((((sum = inflation + (SUM9REG(128) >> 7)) > 5) || (sum == 4)) ? (total++, 128) : 0))

    CAM_MASKED_REG_LOOP(VANNEAL8)
}

#if 0

void CellEngine::n_freaco()
{
    long sum;

#define FRAIN ( \
    (((c & 0x03) == 0) \
        ? ((SUM8REG(1) == 2) ? 1 : 0) \
        : 0) | \
    ((c & 1) ? (0xfe) | \
             : (
         : 0)
    ((sum = c % 0xfe),
     (c << 1) & 0xfe)) \
)

#define FRANTILIFE ( \
     ((sum = SUM8REG(1)), \
     (((c&1) ? (sum != 5) : (((sum != 5) && (sum != 6)) ? 1 : 0)) | \
       ((c&0x7f) << 1))))

#define FREACO ( \
      ((((sum = (SUM9REG(128) >> 7)) > 5) || (sum == 4)) \
        ? ((FRANTILIFE & 127) | 128) \
        : (FRAIN & 127)))

    CAM_MASKED_REG_LOOP(FREACO)
}

#endif


////////////////////////////////////////////////////////////////////////
// von Neumann 29 state cellular automata


enum {
  U    = 0x00,

  S    = 0x01,
  S0   = 0x02,
  S1   = 0x03,
  S00  = 0x04,
  S01  = 0x05,
  S10  = 0x06,
  S11  = 0x07,
  S000 = 0x08,

  C00  = 0x10,
  C10  = 0x11,
  C01  = 0x90,
  C11  = 0x91,

  OR   = 0x20,
  OU   = 0x21,
  OL   = 0x22,
  OD   = 0x23,

  SR   = 0x40,
  SU   = 0x41,
  SL   = 0x42,
  SD   = 0x43,

  ORX  = 0xa0,
  OUX  = 0xa1,
  OLX  = 0xa2,
  ODX  = 0xa3,

  SRX  = 0xc0,
  SUX  = 0xc1,
  SLX  = 0xc2,
  SDX  = 0xc3,

  DM   = 0x03, // direction mask
  RD   = 0x00, // right direction
  UD   = 0x01, // up direction
  LD   = 0x02, // left direction
  DD   = 0x03, // down direction
  EX   = 0x80, // excited
  NX   = 0x7f, // not excited

};


typedef struct StateInfo {
  char *name;
  unsigned char code;
} StateInfo;

const int jvnStates = 29;
StateInfo States[jvnStates] = {
  { "U",    U     },

  { "S",    S     },
  { "S0",   S0    },
  { "S1",   S1    },
  { "S00",  S00   },
  { "S01",  S01   },
  { "S10",  S10   },
  { "S11",  S11   },
  { "S000", S000  },

  { "C00",  C00   },
  { "C10",  C10   },
  { "C01",  C01   },
  { "C11",  C11   },

  { "OR",   OR    },
  { "OU",   OU    },
  { "OL",   OL    },
  { "OD",   OD    },

  { "SR",   SR    },
  { "SU",   SU    },
  { "SL",   SL    },
  { "SD",   SD    },

  { "ORX",  ORX   },
  { "OUX",  OUX   },
  { "OLX",  OLX   },
  { "ODX",  ODX   },

  { "SRX",  SRX   },
  { "SUX",  SUX   },
  { "SLX",  SLX   },
  { "SDX",  SDX   },
};


int jvnInitialized = 0;
char *codeToName[256];


/* Return 1 if pointed by an excited special transmission state,
   else returns 0. */
int sp(Byte right, Byte up, Byte left, Byte down)
{
    return (
      (right == SLX) ||
      (up == SDX) ||
      (left == SRX) ||
      (down == SUX)
    );
}


/* Return 1 if pointed by an excited ordinary transmission state,
   else returns 0. */
int op(Byte right, Byte up, Byte left, Byte down)
{
    return (
      (right == OLX) ||
      (up == ODX) ||
      (left == ORX) ||
      (down == OUX)
    );
}


/* Return 1 if pointed by an excited transmission state (ordinary or special),
   else returns 0. */
int osp(Byte right, Byte up, Byte left, Byte down)
{
    return (
      (right == OLX) ||
      (right == SLX) ||
      (up == ODX) ||
      (up == SDX) ||
      (left == ORX) ||
      (left == SRX) ||
      (down == OUX) ||
      (down == SUX)
    );
}


/* Return 1 if well pointed by an excited ordinary transmission state,
   else returns 0. */
int wop(Byte state, Byte right, Byte up, Byte left, Byte down)
{
    state &= DM;
    return (
      ((right == OLX) &&
       (state != RD)) ||
      ((up == ODX) &&
       (state != UD)) ||
      ((left == ORX) &&
       (state != LD)) ||
      ((down == OUX) &&
       (state != DD))
    );
}


/* Return 1 if well pointed by an excited special transmission state,
   else returns 0. */
int wsp(Byte state, Byte right, Byte up, Byte left, Byte down)
{
    state &= DM;
    return (
      ((right == SLX) &&
       (state != RD)) ||
      ((up == SDX) &&
       (state != UD)) ||
      ((left == SRX) &&
       (state != LD)) ||
      ((down == SUX) &&
       (state != DD))
    );
}


/* Return 1 if well flanked by an excited (not next excited) confluent state,
   else returns 0. */
int wce(Byte state, Byte right, Byte up, Byte left, Byte down)
{
    state &= DM;
    return (
      (((right == C10) || (right == C11)) &&
       (state != RD)) ||
      (((up == C10) || (up == C11)) &&
       (state != UD)) ||
      (((left == C10) || (left == C11)) &&
       (state != LD)) ||
      (((down == C10) || (down == C11)) &&
       (state != DD))
    );
}


/* Return excited confluent state. */
Byte cascade(Byte right, Byte up, Byte left, Byte down)
{
    return ( ((right == OLX) || (up == ODX) || (left == ORX) || (down == OUX)) &&
            !((right == OL) || (up == OD) || (left == OR) || (down == OU)) );
}


/* Return excited confluent state. */
Byte excite(Byte state)
{
    if ((state == C00) || (state == C10)) {
        return C01;
    } else {
        return C11;
    } // if
}


/* Return decayed confluent state. */
Byte decay(Byte state)
{
    if ((state == C00) || (state == C10)) {
        return C00;
    } else {
        return C10;
    } // if
}


  /*

  Instructions to the construction arm:

    Bits    Construct

    1111    C00

    10000   OR
    10001   OU
    1001    OL
    1010    OD

    1011    SR
    1100    SU
    1101    SL
    1110    SD

  */

void CellEngine::n_jvn29()
{
    if (!jvnInitialized) {
        jvnInitialized = 1;
        int i;
        for (i = 0; i < 256; i++) {
            codeToName[i] = 0;
        } // for i
        for (i = 0; i < jvnStates; i++) {
            codeToName[States[i].code] =
                States[i].name;
        } // for i
    } // if

    Byte *front = frontMem;
    Byte *back = backMem +
                 backBorder +
                 (backBorder * backRowBytes);
    Byte *f = front, *b = back;
    long xx, yy;

    Byte up, down, left, right, state;
    for (yy = 0;
         yy < height;
         f += screenRowBytes,
         b += backRowBytes,
         yy++) {

        state = b[-1];
        right = b[0];

        for (xx = 0;
             xx < width;
             xx++) {

            up = b[xx + -backRowBytes];
            down = b[xx + backRowBytes];
            left = state;
            state = right;
            right = b[xx + 1];

            Byte nextstate = state;

            switch (state) {


            case U: // unexcited state
              {
                if ((right == U) && (up == U) && (left == U) && (down == U)) {
                  // no change
                } else if (osp(right, up, left, down)) {
                  nextstate = S;
                } else {
                  // no change
                } // if
                break;
              } // case

            case OR: // ordinary states
            case OU:
            case OL:
            case OD:
            case ORX:
            case OUX:
            case OLX:
            case ODX:
              {
                if (sp(right, up, left, down)) {
                  nextstate = U; // blank
                } else if (wce(state, right, up, left, down) ||
                           wop(state, right, up, left, down)) {
                  nextstate |= EX; // excited
                } else {
                  nextstate &= NX; // not excited
                } // if
                break;
              } // case

            case SR: // special states
            case SU:
            case SL:
            case SD:
            case SRX:
            case SUX:
            case SLX:
            case SDX:
              {
                if (op(right, up, left, down)) {
                  nextstate = U; // blank
                } else if (wce(state, right, up, left, down) ||
                           wsp(state, right, up, left, down)) {
                  nextstate |= EX; // excited
                } else {
                  nextstate &= NX; // not excited
                } // if
                break;
              } // case

            case C00:
            case C10:
            case C01:
            case C11:
              {
                if (sp(right, up, left, down)) {
                  nextstate = U; // blank
                } else if (cascade(right, up, left, down)) {
                  nextstate = excite(state);
                } else {
                  nextstate = decay(state);
                } // if
                break;
              } // case

            case S: // sensitized state
              {
                if (osp(right, up, left, down)) {
                  nextstate = S1;
                } else {
                  nextstate = S0;
                } // if
                break;
              } // case

            case S0: // sensitized state
              {
                if (osp(right, up, left, down)) {
                  nextstate = S01;
                } else {
                  nextstate = S00;
                } // if
                break;
              } // case

            case S1: // sensitized state
              {
                if (osp(right, up, left, down)) {
                  nextstate = S11;
                } else {
                  nextstate = S10;
                } // if
                break;
              } // case

            case S00: // sensitized state
              {
                if (osp(right, up, left, down)) {
                  nextstate = OL;
                } else {
                  nextstate = S000;
                } // if
                break;
              } // case

            case S01: // sensitized state
              {
                if (osp(right, up, left, down)) {
                  nextstate = SR;
                } else {
                  nextstate = OD;
                } // if
                break;
              } // case

            case S10: // sensitized state
              {
                if (osp(right, up, left, down)) {
                  nextstate = SL;
                } else {
                  nextstate = SU;
                } // if
                break;
              } // case

            case S11: // sensitized state
              {
                if (osp(right, up, left, down)) {
                  nextstate = C00;
                } else {
                  nextstate = SD;
                } // if
                break;
              } // case

            case S000: // sensitized state
              {
                if (osp(right, up, left, down)) {
                  nextstate = OU;
                } else {
                  nextstate = OR;
                } // if
                break;
              } // case

            default:
              {
                nextstate = U;
                break;
              } // case

            } // switch

            f[xx] = nextstate;

        } // for xx
    } // for yy

}

////////////////////////////////////////////////////////////////////////
