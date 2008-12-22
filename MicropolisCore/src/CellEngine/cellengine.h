/* cellengine.h
 * Cell Engine include file
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
// Includes


#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <math.h>

#ifdef _WIN32

#include <winsock2.h>
#include <sys/stat.h>
#include <time.h>

#ifndef S_ISDIR
#define S_ISDIR(x) (((x) & S_IFMT) == S_IFDIR)
#endif

#else

#include <unistd.h>
#include <sys/time.h>
#include <sys/file.h>
#include <sys/types.h>

#endif

#include <string>
#include <vector>
#include <map>

#include <Python.h>

#include "linuxcompat.h"

////////////////////////////////////////////////////////////////////////
// Constants


#define CELLENGINE_VERSION      "1.0"


////////////////////////////////////////////////////////////////////////
// Typedefs


typedef unsigned char Byte;
typedef long QUAD;


////////////////////////////////////////////////////////////////////////
// Functions


long Rand32();
long Rand16();
long Rand8();
void PrimeRandoms();
void FeedRandom(long food);


////////////////////////////////////////////////////////////////////////
// Classes


class CellEngine {

public:

    std::string name;

    Byte *screenMem;
    long screenWidth;
    long screenHeight;
    long screenRowBytes;
    
    Byte *backMem;
    long backSize;
    long backRowBytes;
    long backWidth;
    long backHeight;
    long backBorder;

    Byte *frontMem;

    Byte *maskMem;
    long maskRowBytes;
    long maskWidth;
    long maskHeight;
    long masked;
     
    long neighborhood;
    
    Byte *rule;
    long ruleStatic;
    long ruleSize;
    char *ruleName;
    
    long x;
    long y;
    long dx;
    long dy;
    long width;
    long height;
    long idealWidth;
    long idealHeight;
    
    long phase;
    long ticks;
    long wrap;
    long steps;
    long frob;
    long rumble;
    long rumblemax;
    long hubba;
    long inflation;
    long high;
    long low;
    long maskClip;
    long moveImage;

    int numbera;
    int numberb;
    int numberc;
    int anglea;
    int angleb;
    int anglec;
    int pointax;
    int pointay;
    int pointbx;
    int pointby;
    int pointcx;
    int pointcy;

    long clipRects;
    RECT *rectList;

    long skips;
    long skip;
    long total;
    
    long tracking;
    long action;
    long downx;
    long downy;
    long grabx;
    long graby;
    long lastx;
    long lasty;
    long saveskips;
    long still;
    long gravx;
    long gravy;
    long grabbable;
    void *data;

    CellEngine();
    ~CellEngine();

    void Init();

        void InitScreen(
                long ww,
                long hh);

    void SetRect(
                long xx,
        long yy,
        long ww,
        long hh);

    void SetPos(
                long xx,
        long yy);

    void SetSize(
                long ww,
        long hh);

    void ForceOnScreen();

    long OnScreen();

    void Garble();
    
    void GarbleRect(
                long xx,
        long yy,
        long ww,
        long hh);
    
    void Fill(
                Byte c);

    void FillRect(
                Byte c,
        long xx,
        long yy,
        long ww,
        long hh);

    void ResetMask();

    void SetMask(
                long ww,
        long hh,
        Byte *data,
        long rb);

    void UpdateClip();

    void LoadRule(
                const char *name);

    void LoadRuleData(
        const char *stream);

    void LoadStaticRuleData(
        const char *stream);

    void SetRuleTable(
        const char *rule,
        int ruleSize,
        int neigh);

    int CountRules();

    const char *GetRuleName(
        int i);

    const char *GetRuleData(
        int i);

    long DoPhysics();

    long DoRule();

    void PostRule();

    void CopyToBack();

    void PumpToFront();

    Byte GetCell(
                int col, 
                int row);
    
        void SetCell(
                int col, 
                int row, 
                Byte cell);

        void *GetCellBuffer();

    void n_moore_a();
    void n_moore_ab();
    void n_vonn_neumann();
    void n_margolis();
    void n_margolis_ph();
    void n_margolis_hv();
    void n_life();
    void n_brain();
    void n_heat();
    void n_dheat();
    void n_lheat();
    void n_ldheat();
    void n_abdheat();
    void n_abcdheat();
    void n_edheat();
    void n_ranch();
    void n_anneal();
    void n_anneal4();
    void n_anneal8();
    void n_eco();
    void n_torben();
    void n_torben2();
    void n_torben3();
    void n_torben4();
    void n_ball();
    void n_fdheat();
    void n_fabcdheat();
    void n_risca();
    void n_insert();
    void n_heaco();
    void n_marble();
    void n_smarble();
    void n_farble();
    void n_garble();
    void n_garblebug();
    void n_twoheats();
    void n_spin();
    void n_driven();
    void n_daft();
    void n_spinsonly();
    void n_spinsbank();
    void n_spinsheat();
    void n_spinglass();
    void n_glassbonds();
    void n_glassheat();
    void n_faders();
    void n_harble();
    void n_perlin();
    void n_dendrite();
    void n_vanneal();
    void n_vanneal8();
    void n_jvn29();


#ifdef SWIG
#ifdef SWIGPYTHON


%extend {


    void SetRuleBuffer(
            PyObject *buf,
            int neigh)
    {
        char *buffer =
            NULL;
        Py_ssize_t len =
            0;

        PyObject_AsReadBuffer(
            buf,
            (const void **)&buffer,
            &len);

        self->SetRuleTable(
            buffer, 
            (int)len, 
            neigh);

    }


}


#endif
#endif


};


////////////////////////////////////////////////////////////////////////


#define EVENT_BOUNCE_TOP    0x0100
#define EVENT_BOUNCE_BOTTOM 0x0200
#define EVENT_BOUNCE_LEFT   0x0400
#define EVENT_BOUNCE_RIGHT  0x0800
#define EVENT_MOVE          0x1000


////////////////////////////////////////////////////////////////////////


#ifdef MACOS

#define SWAPLONG(x) 0

#define BYTE0(x) ((x>>24)&0xff)
#define BYTE1(x) ((x>>16)&0xff)
#define BYTE2(x) ((x>>8)&0xff)
#define BYTE3(x) (x&0xff)

#define SETBYTE0(q, b) (q = (q & 0x00ffffff) | ((b << 24) & 0xff000000))
#define SETBYTE1(q, b) (q = (q & 0xff00ffff) | ((b << 16) & 0x00ff0000))
#define SETBYTE2(q, b) (q = (q & 0xffff00ff) | ((b << 8) & 0x0000ff00))
#define SETBYTE3(q, b) (q = (q & 0xffffff00) | (b & 0x000000ff))

#else

#define SWAPLONG(x) \
    ((x = ((x <<24) & 0xff000000) | \
          ((x <<8)  & 0x00ff0000) | \
          ((x >>8)  & 0x0000ff00) | \
          ((x >>24) & 0x000000ff)), 0)

#define BYTE3(x) ((x>>24)&0xff)
#define BYTE2(x) ((x>>16)&0xff)
#define BYTE1(x) ((x>>8)&0xff)
#define BYTE0(x) (x&0xff)

#define SETBYTE3(q, b) (q = (q & 0x00ffffff) | ((b << 24) & 0xff000000))
#define SETBYTE2(q, b) (q = (q & 0xff00ffff) | ((b << 16) & 0x00ff0000))
#define SETBYTE1(q, b) (q = (q & 0xffff00ff) | ((b << 8) & 0x0000ff00))
#define SETBYTE0(q, b) (q = (q & 0xffffff00) | (b & 0x000000ff))

#endif


////////////////////////////////////////////////////////////////////////


#define sNORTHWEST  16
#define sNORTH      8
#define sNORTHEAST  0
#define sWEST       16
#define sCENTER     8
#define sEAST       0
#define sSOUTHWEST  16
#define sSOUTH      8
#define sSOUTHEAST  0


#define xNORTHWEST(m)   (l0 & (m << sNORTHWEST))
#define xNORTH(m)       (l0 & (m << sNORTH))
#define xNORTHEAST(m)   (l0 & (m << sNORTHEAST))
#define xWEST(m)        (l1 & (m << sWEST))
#define xCENTER(m)      (l1 & (m << sCENTER))
#define xEAST(m)        (l1 & (m << sEAST))
#define xSOUTHWEST(m)   (l2 & (m << sSOUTHWEST))
#define xSOUTH(m)       (l2 & (m << sSOUTH))
#define xSOUTHEAST(m)   (l2 & (m << sSOUTHEAST))


#define NORTHWEST(m)    ((Byte)(xNORTHWEST(m) >> sNORTHWEST))
#define NORTH(m)        ((Byte)(xNORTH(m) >> sNORTH))
#define NORTHEAST(m)    ((Byte)(xNORTHEAST(m) >> sNORTHEAST))
#define WEST(m)         ((Byte)(xWEST(m) >> sWEST))
#define CENTER(m)       ((Byte)(xCENTER(m) >> sCENTER))
#define EAST(m)         ((Byte)(xEAST(m) >> sEAST))
#define SOUTHWEST(m)    ((Byte)(xSOUTHWEST(m) >> sSOUTHWEST))
#define SOUTH(m)        ((Byte)(xSOUTH(m) >> sSOUTH))
#define SOUTHEAST(m)    ((Byte)(xSOUTHEAST(m) >> sSOUTHEAST))


#define SUM4(m) ( \
    (NORTH(m)) + \
    (WEST(m)) + \
    (EAST(m)) + \
    (SOUTH(m)))


#define SUM8(m) ( \
    (SUM4(m)) + \
    (NORTHWEST(m)) + \
    (NORTHEAST(m)) + \
    (SOUTHWEST(m)) + \
    (SOUTHEAST(m)))


#define SUM9(m) ( \
    (SUM8(m)) + \
    (CENTER(m)))


#define SUM4REG(m) ( \
    (n & m) + \
    (w & m) + \
    (e & m) + \
    (s & m))

#define SUM8REG(m) ( \
    (SUM4REG(m)) + \
    (nw & m) + \
    (ne & m) + \
    (sw & m) + \
    (se & m))

#define SUM9REG(m) ( \
    (SUM8REG(m)) + \
    (c & m))

// This is half baked
#define CAM_REG_RECT_LOOP(BODY, \
                          WIDTH, HEIGHT, \
                          FROMMEM, FROMSTRIDE, \
                          TOMEM, TOSTRIDE) \
    { \
        long x, y; \
        long width = WIDTH; \
        long height = HEIGHT; \
        Byte *fromMem = FROMMEM; \
        long fromStride = FROMSTRIDE; \
        Byte *toMem = TOMEM; \
        long toStride = TOSTRIDE; \
        for (y = 0; \
             y < height; \
             y++) { \
            register Byte nw, n, ne, w, c, e, sw, s, se; \
            n = fromMem[0]; \
            ne = fromMem[1]; \
            c = fromMem[fromStride]; \
            e = fromMem[fromStride + 1]; \
            s = fromMem[fromStride + fromStride]; \
            se = fromMem[fromStride + fromStride + 1]; \
            for (x = 0; \
                 x < fromWidth; \
                 x++) { \
                nw = n; n = ne; ne = fromMem[2]; \
                w = c; c = e; e = fromMem[fromStride + 2]; \
                sw = s; s = se; se = fromMem[fromStride + fromStride + 2]; \
                BODY(toMem, nw, n, ne, w, c, e, sw, s, se); \
                fromMem++; toMem++; \
            } \
            fromMem += fromStride - fromWidth; \
            toMem += toStride - fromWidth; \
        } \
    }


// This is half baked
#define CAM_XTRACT_RECT_LOOP(BODY, \
                             WIDTH, HEIGHT, \
                             FROMMEM, FROMSTRIDE, \
                             TOMEM, TOSTRIDE) \
    { \
        long x, y; \
        long width = WIDTH; \
        long height = HEIGHT; \
        Byte *fromMem = FROMMEM; \
        long fromStride = FROMSTRIDE; \
        Byte *toMem = TOMEM; \
        long toStride = TOSTRIDE; \
        for (y = 0; \
             y < height; \
             y++) { \
            QUAD l0 = (fromMem[0]<<8) + \
                       fromMem[1], \
                 l1 = (fromMem[fromStride]<<8) + \
                       fromMem[fromStride+1], \
                 l2 = (fromMem[fromStride+fromStride]<<8) + \
                       fromMem[fromStride+fromStride+1]; \
            for (x = 0; \
                 x < w; \
                 x++) { \
                l0 = (l0<<8) + fromMem[2]; \
                l1 = (l1<<8) + fromMem[fromStride+2]; \
                l2 = (l2<<8) + fromMem[fromStride+fromStride+2]; \
                BODY(toMem, \
                     NORTHWEST(0xff), NORTH(0xff), NORTHEAST(0xff), \
                     WEST(0xff), CENTER(0xff), EAST(0xff), \
                     SOUTHWEST(0xff), SOUTH(0xff), SOUTHEAST(0xff)) \
                fromMem++; toMem++; mask++; \
            } \
            fromMem += fromStride - width; \
            toMem += toStride - width; \
        } \
    }


#define CAM_REG_LOOP_BODY(BODY) \
    {   Byte *front = frontMem; \
        Byte *back = backMem + \
                     (backBorder - 1) + \
                     ((backBorder - 1) * backRowBytes); \
        long yy; \
        for (yy = 0; \
             yy < height; \
             yy++) { \
            Byte nw, n, ne, w, c, e, sw, s, se; \
            n = back[0]; \
            ne = back[1]; \
            c = back[backRowBytes]; \
            e = back[backRowBytes + 1]; \
            s = back[backRowBytes + backRowBytes]; \
            se = back[backRowBytes + backRowBytes + 1]; \
            long xx; \
            for (xx = 0; \
                 xx < width; \
                 xx++) { \
                nw = n; n = ne; ne = back[2]; \
                w = c; c = e; e = back[backRowBytes + 2]; \
                sw = s; s = se; se = back[backRowBytes + backRowBytes + 2]; \
                BODY; \
                back++; front++; \
            } \
            back += backRowBytes - width; \
            front += screenRowBytes - width; \
        } \
    }


#define CAM_MASKED_LOOP_BODY(BODY) \
    if (!masked) { \
        CAM_LOOP_BODY(BODY); \
    } else { \
        Byte *front = frontMem; \
        Byte *back = backMem + \
                     (backBorder - 1) + \
                     ((backBorder - 1) * backRowBytes); \
        Byte *mask = maskMem; \
        Byte maskClip = maskClip; \
        long w = width; \
        if (maskWidth < w) w = maskWidth; \
        long h = height; \
        if (maskHeight < h) h = maskHeight; \
        long yy; \
        for (yy = 0; \
             yy < h; \
             yy++) { \
            QUAD l0 = (back[0]<<8) + \
                       back[1], \
                 l1 = (back[backRowBytes]<<8) + \
                       back[backRowBytes+1], \
                 l2 = (back[backRowBytes+backRowBytes]<<8) + \
                       back[backRowBytes+backRowBytes+1]; \
            Byte m; \
            long xx; \
            for (xx = 0; \
                 xx < w; \
                 xx++) { \
                l0 = (l0<<8) + back[2]; \
                l1 = (l1<<8) + back[backRowBytes+2]; \
                l2 = (l2<<8) + back[backRowBytes+backRowBytes+2]; \
                m = *mask; \
                if (m > maskClip) { \
                    BODY; \
                } \
                back++; front++; mask++; \
            } \
            back += backRowBytes - w; \
            front += screenRowBytes - w; \
            mask += maskRowBytes - w; \
        } \
    }


#define CAM_MASKED_REG_LOOP_BODY(BODY) \
    if (clipRects == 0) { \
        CAM_REG_LOOP_BODY(BODY) \
    } else { \
        RECT *rect = rectList; \
        long rect_count = clipRects; \
        while (rect_count-- > 0) { \
            long x1 = rect->left; \
            long x2 = rect->right; \
            Byte *front = frontMem; \
            Byte *back = backMem + \
                         (backBorder - 1) + \
                         ((backBorder - 1) * backRowBytes); \
            front += x1 + (rect->top * screenRowBytes); \
            back += x1 + (rect->top * backRowBytes); \
            long yy; \
            for (yy = rect->top; \
                 yy < rect->bottom; \
                 yy++) { \
                register Byte nw, n, ne, w, c, e, sw, s, se; \
                n = back[0]; \
                ne = back[1]; \
                c = back[backRowBytes]; \
                e = back[backRowBytes + 1]; \
                s = back[backRowBytes + backRowBytes]; \
                se = back[backRowBytes + backRowBytes + 1]; \
                long xx; \
                for (xx = x1; \
                     xx < x2; \
                     xx++) { \
                    nw = n; n = ne; ne = back[2]; \
                    w = c; c = e; e = back[backRowBytes + 2]; \
                    sw = s; s = se; se = back[backRowBytes + backRowBytes + 2]; \
                    BODY; \
                    back++; front++; \
                } \
                back += backRowBytes - x2 + x1; \
                front += screenRowBytes - x2 + x1; \
            } \
            rect++; \
        } \
    }


#define CAM_LOOP(RULE) \
    CAM_LOOP_BODY(*front = (RULE))


#define CAM_MASKED_LOOP(RULE) \
    CAM_MASKED_LOOP_BODY(*front = (RULE))


#define CAM_REG_LOOP(RULE) \
    CAM_REG_LOOP_BODY(*front = (RULE))


#define CAM_MASKED_REG_LOOP(RULE) \
    CAM_MASKED_REG_LOOP_BODY(*front = (RULE))


#define CAM_TABLE_LOOP(NEIGHBORHOOD) \
    CAM_LOOP(rule[(NEIGHBORHOOD)])


#define CAM_MASKED_TABLE_LOOP(NEIGHBORHOOD) \
    CAM_MASKED_LOOP(rule[(NEIGHBORHOOD)])


#define CAM_REG_TABLE_LOOP(NEIGHBORHOOD) \
    CAM_REG_LOOP((c << 4) | rule[(NEIGHBORHOOD)])


#define CAM_MASKED_REG_TABLE_LOOP(NEIGHBORHOOD) \
    CAM_MASKED_REG_LOOP(rule[(NEIGHBORHOOD)])

#define CAM_MASKED_REG_TABLE_LOOP_ECHO2(NEIGHBORHOOD) \
    CAM_MASKED_REG_LOOP((c << 2) | rule[(NEIGHBORHOOD)])


#define UPPER6HEAT() \
    ((last += (nw + n + ne + w + frob + e + sw + s + se)), \
     (heat = (last >> 3) & 0xfc), \
     (last = last & 0x1f), \
     heat)


#define UPPER4HEAT() \
    ((last += (nw + n + ne + w + frob + e + sw + s + se)), \
     (heat = (last >> 3) & 0xf0), \
     (last = last & 0x7f), \
     heat)


#define CAM_MASKED_REG_HEAT6_LOOP(RULE) \
    CAM_MASKED_REG_LOOP_BODY(*front = UPPER6HEAT() + (RULE))


#define CAM_MASKED_REG_TABLE_HEAT6_LOOP(NEIGHBORHOOD) \
    CAM_MASKED_REG_LOOP(UPPER6HEAT() + rule[(NEIGHBORHOOD)])


#define CAM_MASKED_REG_HEAT4_LOOP(RULE) \
    CAM_MASKED_REG_LOOP_BODY(*front = UPPER4HEAT() + (RULE))


#define CAM_MASKED_REG_TABLE_HEAT4_LOOP(NEIGHBORHOOD) \
    CAM_MASKED_REG_LOOP(UPPER4HEAT() + rule[(NEIGHBORHOOD)])


////////////////////////////////////////////////////////////////////////
