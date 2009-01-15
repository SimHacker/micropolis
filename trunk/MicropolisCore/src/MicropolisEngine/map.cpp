/* map.cpp
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

/** @file map.cpp */

////////////////////////////////////////////////////////////////////////


#include "stdafx.h"


#if 0


////////////////////////////////////////////////////////////////////////
// Disabled this small map drawing, filtering and overlaying code.
// Going to re-implement it in the tile engine and Python.


#define VAL_NONE        0
#define VAL_LOW         1
#define VAL_MEDIUM      2
#define VAL_HIGH        3
#define VAL_VERYHIGH    4
#define VAL_PLUS        5
#define VAL_VERYPLUS    6
#define VAL_MINUS       7
#define VAL_VERYMINUS   8


/* These are names of the 16 colors */
#define COLOR_WHITE             0
#define COLOR_YELLOW            1
#define COLOR_ORANGE            2
#define COLOR_RED               3
#define COLOR_DARKRED           4
#define COLOR_DARKBLUE          5
#define COLOR_LIGHTBLUE         6
#define COLOR_BROWN             7
#define COLOR_LIGHTGREEN        8
#define COLOR_DARKGREEN         9
#define COLOR_OLIVE             10
#define COLOR_LIGHTBROWN        11
#define COLOR_LIGHTGRAY         12
#define COLOR_MEDIUMGRAY        13
#define COLOR_DARKGRAY          14
#define COLOR_BLACK             15


/*

static short valMap[] = {
    -1, COLOR_LIGHTGRAY, COLOR_YELLOW, COLOR_ORANGE, COLOR_RED,
    COLOR_DARKGREEN, COLOR_LIGHTGREEN, COLOR_ORANGE, COLOR_YELLOW
};


static short valGrayMap[] = {
    -1, 31, 127, 191, 255,
    223, 255, 31, 0
};

*/


/* color pixel values */
#define UNPOWERED       COLOR_LIGHTBLUE
#define POWERED         COLOR_RED
#define CONDUCTIVE      COLOR_LIGHTGRAY


#define DRAW_BEGIN \
    int col, row; \
    unsigned short tile; \
    short *mp; \
    unsigned char *imageBase; \
    unsigned char *image; \
    UQuad *mem; \
    UQuad m; \
    int lineBytes = view->line_bytes8; \
    int pixelBytes = view->pixel_bytes; \
    mp = &map[0][0]; \
    imageBase = view->x->color ? view->data : view->data8; \
    for (col = 0; col < WORLD_W; col++) { \
        image = imageBase + (3 * pixelBytes * col); \
        for (row = 0; row < WORLD_H; row++) { \
            tile = *(mp++) & LOMASK; \
            if (tile >= TILE_COUNT) { \
                tile -= TILE_COUNT; \
            }


#ifdef IS_INTEL

#define ROW1_8(n) \
            m = mem[n]; \
            image[0] = (unsigned char)(m); \
            image[1] = (unsigned char)(m >>8); \
            image[2] = (unsigned char)(m >>16); \
            image += lineBytes;

#define ROW1_16(n) \
            memcpy((char *)image, ((char *)mem) + (n * 4 * 2), (3 * 2)); \
            image += lineBytes;

#define ROW1_24(n) \
            memcpy((char *)image, ((char *)mem) + (n * 4 * 3), (3 * 3)); \
            image += lineBytes;

#define ROW1_32(n) \
            memcpy((char *)image, ((char *)mem) + (n * 4 * 4), (3 * 4)); \
            image += lineBytes;

#else

#define ROW1_8(n) \
            m = (UQuad)(mem[n]); \
            image[0] = (unsigned char)(m >>24); \
            image[1] = (unsigned char)(m >>16); \
            image[2] = (unsigned char)(m >>8); \
            image += lineBytes;

#define ROW1_16(n) \
            m = (UQuad)mem[n]; /* XXX: WRONG. handle depth */ \
            image[0] = (unsigned char)(m >>24); \
            image[1] = (unsigned char)(m >>16); \
            image[2] = (unsigned char)(m >>8); \
            image += lineBytes;

#define ROW1_24(n) \
            m = (UQuad)mem[n]; /* XXX: WRONG. handle depth */ \
            image[0] = (unsigned char)(m >>24); \
            image[1] = (unsigned char)(m >>16); \
            image[2] = (unsigned char)(m >>8); \
            image += lineBytes;

#define ROW1_32(n) \
            m = (UQuad)mem[n]; /* XXX: WRONG. handle depth */ \
            image[0] = (unsigned char)(m >>24); \
            image[1] = (unsigned char)(m >>16); \
            image[2] = (unsigned char)(m >>8); \
            image += lineBytes;

#endif

#define ROW3_8 ROW1_8(0) ROW1_8(1) ROW1_8(2)
#define ROW3_16 ROW1_16(0) ROW1_16(1) ROW1_16(2)
#define ROW3_24 ROW1_24(0) ROW1_24(1) ROW1_24(2)
#define ROW3_32 ROW1_32(0) ROW1_32(1) ROW1_32(2)

#define ROW3 \
            switch (view->x->depth) { \
                case 1: \
                case 8: \
                    ROW3_8 \
                    break; \
                case 15: \
                case 16: \
                    ROW3_16 \
                    break; \
                case 24: \
                    ROW3_24 \
                    break; \
                case 32: \
                    ROW3_32 \
                    break; \
                default: \
                    assert(0); /* Undefined depth */ \
                    break; \
            }

#define DRAW_END \
            mem = (UQuad *)&view->smalltiles[tile * 4 * 4 * pixelBytes]; \
            ROW3 \
        } \
    }


void Micropolis::drawAll()
{
    DRAW_BEGIN
    DRAW_END
}


void Micropolis::drawRes()
{
    DRAW_BEGIN
        if (tile >= COMBASE) {
            tile = DIRT;
        }
    DRAW_END
}


void Micropolis::drawCom()
{
    DRAW_BEGIN
        if ((tile > COMLAST) ||
            ((tile >= LVRAIL6) &&
             (tile < COMBASE))) {
            tile = DIRT;
        }
    DRAW_END
}


void Micropolis::drawInd()

{
    DRAW_BEGIN
        if (((tile >= RESBASE) && (tile < INDBASE)) ||
            ((tile >= PORTBASE) && (tile < SMOKEBASE)) ||
            ((tile >= TINYEXP) && (tile <= TINYEXPLAST)) ||
            (tile >= FOOTBALLGAME1)) {
            tile = DIRT;
        }
    DRAW_END
}


void Micropolis::drawLilTransMap()
{
    DRAW_BEGIN
        if ((tile >= RESBASE) ||
            ((tile >= BRWXXX7) && tile <= LVPOWER10) ||
            (tile == UNUSED_TRASH6)) {
            tile = DIRT;
        }
    DRAW_END
}


void Micropolis::drawPower()
{
    short col, row;
    unsigned short tile;
    short *mp;
    unsigned char *image, *imageBase;
    UQuad *mem;
    UQuad m;
    int lineBytes = view->line_bytes8;
    int pixelBytes = view->pixel_bytes;

    int pix;
    int powered, unpowered, conductive;

    if (view->x->color) {
        powered = view->pixels[POWERED];
        unpowered = view->pixels[UNPOWERED];
        conductive = view->pixels[CONDUCTIVE];
    } else {
        powered = 255;
        unpowered = 0;
        conductive = 127;
    }

    mp =
        &map[0][0];
    imageBase =
        view->x->color ? view->data : view->data8;

    for (col = 0; col < WORLD_W; col++) {

        image =
            imageBase + (3 * pixelBytes * col);

        for (row = 0; row < WORLD_H; row++) {
            tile = *(mp++);

            if ((tile & LOMASK) >= TILE_COUNT) {
                tile -= TILE_COUNT;
            }

            if ((tile & LOMASK) <= LASTFIRE) {
                tile &= LOMASK;
                pix = -1;
            } else if (tile & ZONEBIT) {
                pix = (tile & PWRBIT) ? powered : unpowered;
            } else {
                if (tile & CONDBIT) {
                    pix = conductive;
                } else {
                    tile = DIRT;
                    pix = -1;
                }
            }

            if (pix < 0) {
                mem = (UQuad *)&view->smalltiles[tile * 4 * 4 * pixelBytes];
                ROW3
            } else {
                switch (view->x->depth) {

                    case 1:
                    case 8:
                        image[0] = image[1] = image[2] = pix;
                        image += lineBytes;
                        image[0] = image[1] = image[2] = pix;
                        image += lineBytes;
                        image[0] = image[1] = image[2] = pix;
                        image += lineBytes;
                        break;

                    case 15:
                    case 16: {
                        unsigned short *p;
                        p = (unsigned short *)image;
                        p[0] = p[1] = p[2] = pix;
                        image += lineBytes;
                        p = (unsigned short *)image;
                        p[0] = p[1] = p[2] = pix;
                        image += lineBytes;
                        p = (unsigned short *)image;
                        p[0] = p[1] = p[2] = pix;
                        image += lineBytes;
                        break;
                    }

                    case 24:
                    case 32: {
                        int x, y;
                        for (y = 0; y < 3; y++) {
                            unsigned char *img = image;
                            for (x = 0; x < 4; x++) {
                                *(img++) = (pix >> 0) & 0xff;
                                *(img++) = (pix >> 8) & 0xff;
                                *(img++) = (pix >> 16) & 0xff;
                                if (pixelBytes == 4) {
                                    img++;
                                } // if
                            } // for x
                            image += lineBytes;
                          } // for y
                          break;
                      }

                    default:
                        assert(0); /* Undefined depth */
                        break;

                }
            }
        }
    }
}


bool Micropolis::dynamicFilter(
  int col,
  int row)
{
    int r = row >>1;
    int c = col >>1;
    int populationDensity = populationDensityMap[c][r];
    int rateOfGrowth = rateOfGrowthMap[c>>2][r>>2];
    int traffic = trafficDensityMap[c][r];
    int pollution = pollutionMap[c][r];
    int crime = crimeMap[c][r];
    int landValue = landValueMap[c][r];
    int police = policeStationMapEffect[c>>2][r>>2];
    int fire = fireStationMapEffect[c>>2][r>>2];


    return (
        ((dynamicData[0] > dynamicData[1]) ||
         (populationDensity >= dynamicData[0]) &&
         (populationDensity <= dynamicData[1])) &&
        ((dynamicData[2] > dynamicData[3]) ||
         (rateOfGrowth >= ((2 * dynamicData[2]) - 256)) &&
         (rateOfGrowth <= ((2 * dynamicData[3]) - 256))) &&
        ((dynamicData[4] > dynamicData[5]) ||
         (traffic >= dynamicData[4]) &&
         (traffic <= dynamicData[5])) &&
        ((dynamicData[6] > dynamicData[7]) ||
         (pollution >= dynamicData[6]) &&
         (pollution <= dynamicData[7])) &&
        ((dynamicData[8] > dynamicData[9]) ||
         (crime >= dynamicData[8]) &&
         (crime <= dynamicData[9])) &&
        ((dynamicData[10] > dynamicData[11]) ||
         (landValue >= dynamicData[10]) &&
         (landValue <= dynamicData[11])) &&
        ((dynamicData[12] > dynamicData[13]) ||
         (police >= dynamicData[12]) &&
         (police <= dynamicData[13])) &&
        ((dynamicData[14] > dynamicData[15]) ||
         (fire >= dynamicData[14]) &&
         (fire <= dynamicData[15])));
}


void Micropolis::drawDynamic()
{
    DRAW_BEGIN
        if (tile > LASTFIRE) {
            if (!dynamicFilter(col, row)) {
                tile = DIRT;
            } // if
        } // if
    DRAW_END
}


short Micropolis::getCI(short x)
{
    if (x < 50) {
        return VAL_NONE;
    }
    if (x < 100) {
        return VAL_LOW;
    }
    if (x < 150) {
        return VAL_MEDIUM;
    }
    if (x < 200) {
        return VAL_HIGH;
    }
    return VAL_VERYHIGH;
}


void Micropolis::drawPopulationDensity()
{
    short x, y;

    drawAll();
    for (x = 0; x < WORLD_W_2; x++) {
        for (y = 0; y < WORLD_H_2; y++) {
            maybeDrawRect(
                getCI(populationDensityMap[x][y]),
                x * 6,
                y * 6,
                6,
                6);
        }
    }
}


void Micropolis::drawRateOfGrowth()
{
    short x, y;

    drawAll();

    for (x = 0; x < WORLD_W_8; x++) {
        for (y = 0; y < WORLD_H_8; y++) {
            short val;
            short z = rateOfGrowthMap[x][y];

            if (z > 100) {
                val = VAL_VERYPLUS;
            } else {
                if (z > 20) {
                    val = VAL_PLUS;
                } else {
                    if (z < -100) {
                        val = VAL_VERYMINUS;
                    } else {
                        if (z < -20) {
                            val = VAL_MINUS;
                        } else {
                            val = VAL_NONE;
                        }
                    }
                }
            }
            maybeDrawRect(
                val,
                x * 24,
                y * 24,
                24,
                24);
        }
    }
}


void Micropolis::drawTrafMap()
{
    short x;
    short y;

    drawLilTransMap();

    for (x = 0; x < WORLD_W_2; x++) {
        for (y = 0; y < WORLD_H_2; y++) {
            maybeDrawRect(
                getCI(trafficDensityMap[x][y]),
                x * 6,
                y * 6,
                6,
                6);
        }
    }
}


void Micropolis::drawPollutionMap()
{
    short x, y;

    drawAll(view);

    for (x = 0; x < WORLD_W_2; x++) {
        for (y = 0; y < WORLD_H_2; y++) {
            maybeDrawRect(
                getCI(10 + pollutionMap[x][y]),
                x * 6,
                y * 6,
                6,
                6);
        }
    }
}


void Micropolis::drawCrimeMap()
{
    short x, y;

    drawAll();

    for (x = 0; x < WORLD_W_2; x++) {
        for (y = 0; y < WORLD_H_2; y++) {
            maybeDrawRect(
                getCI(crimeMap[x][y]),
                x * 6,
                y * 6,
                6,
                6);
        }
    }
}


void Micropolis::drawLandMap()
{
    short x, y;

    drawAll();

    for (x = 0; x < WORLD_W_2; x++) {
        for (y = 0; y < WORLD_H_2; y++) {
            maybeDrawRect(
                view,
                getCI(landValueMap[x][y]),
                x * 6,
                y * 6,
                6,
                6);
        }
    }
}


void Micropolis::drawFireRadius()
{
    short x, y;

    drawAll();
    for (x = 0; x < WORLD_H_8; x++) {
        for (y = 0; y < WORLD_H_8; y++) {
            maybeDrawRect(
                getCI(fireStationMapEffect[x][y]),
                x * 24,
                y * 24,
                24,
                24);
        }
    }
}


void Micropolis::drawPoliceRadius()
{
    short x, y;

    drawAll();
    for (x = 0; x < WORLD_W_8; x++) {
        for (y = 0; y < WORLD_H_8; y++) {
            maybeDrawRect(
                getCI(policeStationMapEffect[x][y]),
                x * 24,
                y * 24,
                24,
                24);
        }
    }
}


void Micropolis::memDrawMap()
{

    switch (view->map_state) {

      case MAP_TYPE_ALL:
          drawAll(view);
          break;

      case MAP_TYPE_RES:
          drawRes(view);
          break;

      case MAP_TYPE_COM:
          drawCom(view);
          break;

      case MAP_TYPE_IND:
          drawInd(view);
          break;

      case MAP_TYPE_POWER:
          drawPower(view);
          break;

      case MAP_TYPE_ROAD:
          drawLilTransMap(view);
          break;

      case MAP_TYPE_POPULATION_DENSITY:
          drawPopulationDensity(view);
          break;

      case MAP_TYPE_RATE_OF_GROWTH:
          drawRateOfGrowth(view);
          break;

      case MAP_TYPE_TRAFFIC:
          drawTrafficMap(view);
          break;

      case MAP_TYPE_POLLUTION:
          drawPollutionMap(view);
          break;

      case MAP_TYPE_CRIME:
          drawCrimeMap(view);
          break;

      case MAP_TYPE_LAND_VALUE:
          drawLandMap(view);
          break;

      case MAP_TYPE_FIRE_RADIUS:
          drawFireRadius(view);
          break;

      case MAP_TYPE_POLICE_RADIUS:
          drawPoliceRadius(view);
          break;

      case MAP_TYPE_DYNAMIC:
          drawDynamic(view);
          break;

      default:
          assert(0); /* Undefined map */
          break;

    }

  /*
    if (!view->x->color) {
        ditherMap(view);
        XSetForeground(view->x->dpy, view->x->gc, view->pixels[COLOR_BLACK]);
        XSetBackground(view->x->dpy, view->x->gc, view->pixels[COLOR_WHITE]);
        XPutImage(view->x->dpy, view->pixmap, view->x->gc, view->image,
                  0, 0, 0, 0, view->m_width, view->m_height);
    }
  */

}


void Micropolis::ditherMap()
{
/*
    int i, x, y, width, height;
    int err, pixel1, pixel8;
    int line_bytes1 = view->line_bytes;
    int line_bytes8 = view->line_bytes8;
    unsigned char *image1 = view->data;
    unsigned char *image8 = view->data8;
    int *errors;

    width = view->m_width;
    height = view->m_height;

    errors = (int *)newPtr(sizeof(int) * width);

    for (i = 0; i < width; i++) {
        errors[i] = (getRandom16() & 15) - 7;
    }

    err = (getRandom16() & 15) - 7;

    for (y = 0; y < height; y += 2) {
        unsigned char *i1 = image1;
        unsigned char *i8 = image8;

        image1 += line_bytes1;
        image8 += line_bytes8;

        for (x = 0; x < width; x += 8) {
            pixel1 = 0;
            for (i = 0; i < 8; i++) {
                pixel1 <<= 1;
                pixel8 = *(i8++) + err + errors[x + i];
                if (pixel8 > 127) {
                    err = pixel8 - 255;
                } else {
                    pixel1 |= 1;
                    err = pixel8;
                }
                errors[x + i] = err/2;
                err = err/2;
            }
            *(i1++) = pixel1;
        }

        i1 = image1 + (width / 8) - 1;
        i8 = image8 + width - 1;

        image1 += line_bytes1;
        image8 += line_bytes8;

        for (x = width - 8; x >= 0; x -= 8) {
            pixel1 = 0;
            for (i = 7; i >= 0; i--) {
                pixel1 >>= 1;
                pixel8 = *(i8--) + err + errors[x + i];
                if (pixel8 > 127) {
                    err = pixel8 - 255;
                } else {
                    pixel1 |= 128;
                    err = pixel8;
                }
                errors[x + i] = err/2;
                err = err/2;
            }
            *(i1--) = pixel1;
        }
    }

    freePtr(errors);
*/
}


void Micropolis::maybeDrawRect(
  int val,
  int x,
  int y,
  int w,
  int h)
{
    if (val == VAL_NONE) {
        return;
    }

/*
    if (view->x->color) {
        drawRect(view, view->pixels[valMap[val]], 0, x, y, w, h);
    } else {
        drawRect(view, valGrayMap[val], 1, x, y, w, h);
    }
*/
}


void Micropolis::drawRect(
  int pixel,
  int solid,
  int x,
  int y,
  int w,
  int h)
{
/*
    int W = view->m_width, H = view->m_height;

    if (x < 0) {
        if ((w += x) < 0) {
            w = 0;
        }
        x = 0;
    } else if (x > W) {
        x = 0;
        w = 0;
    }
    if (x + w > W) {
        w = W - x;
    }
    if (y < 0) {
        if ((h += y) < 0) {
            h = 0;
        }
        y = 0;
    } else if (y > H) {
        y = 0;
        h = 0;
    }
    if (y + h > H) {
        h = H - y;
    }

    if (w && h) {
        int i, j, stipple = (x ^ y) & 1;
        unsigned char *data =
            view->x->color ? view->data : view->data8;

        // In the case of black and white, we use an 8 bit buffer and dither it.
        int pixelBytes =
            view->x->color ? view->pixel_bytes : 1;
        Quad line =
            view->x->color ? view->line_bytes : view->line_bytes8;

        unsigned char *image =
            &(data[(line * y) + (x * pixelBytes)]);

        switch (pixelBytes) {

            case 1: {
                unsigned char *data =
                    view->data8;
                unsigned char *image =
                    &data[(line * y) + (x * pixelBytes)];

                if (solid) {
                    for (i = h; i > 0; i--) {
                        for (j = w; j > 0; j--) {
                            *image = pixel;
                            image++;
                        }
                        image += line - w;
                    }
                } else {
                    for (i = h; i > 0; i--) {
                        for (j = w; j > 0; j--) {
                            if (stipple++ & 1) {
                                *image = pixel;
                            }
                            image++;
                        }
                        if (!(w & 1))
                            stipple++;
                        image += line - w;
                    }
                }
                break;
            }

            case 2: {
                unsigned short *data =
                    (unsigned short *)view->data;
                unsigned short *image;
                line >>= 1; // Convert from byte offset to short offset
                image =
                    &data[(line * y) + x];

                if (solid) {
                    for (i = h; i > 0; i--) {
                        for (j = w; j > 0; j--) {
                            *image = pixel;
                            image++;
                        }
                        image += line - w;
                    }
                } else {
                    for (i = h; i > 0; i--) {
                        for (j = w; j > 0; j--) {
                            if (stipple++ & 1) {
                                *image = pixel;
                            }
                            image++;
                        }
                        if (!(w & 1)) {
                            stipple++;
                        }
                        image += line - w;
                    }
                }
                break;
            }

            case 3:
            case 4: {
                unsigned char *data =
                    (unsigned char *)view->data;
                unsigned char *image;
                int bitmapPad = view->x->small_tile_image->bitmap_pad;
                int rowBytes = view->x->small_tile_image->bytes_per_line;
                line = rowBytes >> 1; // Convert from byte offset to short offset
                image = &data[(line * y) + x];

                if (solid) {
                    for (i = h; i > 0; i--) {
                        for (j = w; j > 0; j--) {
                            *(image++) = (pixel >> 0) & 0xff;
                            *(image++) = (pixel >> 8) & 0xff;
                            *(image++) = (pixel >> 16) & 0xff;
                            if (bitmapPad == 32) {
                                image++;
                            }
                        }
                        image += line - w;
                    }
                } else {
                    for (i = h; i > 0; i--) {
                        for (j = w; j > 0; j--) {
                            if (stipple++ & 1) {
                              *(image++) = (pixel >> 0) & 0xff;
                              *(image++) = (pixel >> 8) & 0xff;
                              *(image++) = (pixel >> 16) & 0xff;
                              if (bitmapPad == 32) {
                                  image++;
                              }
                          }
                        }
                        if (!(w & 1)) {
                            stipple++;
                        }
                        image += line - w;
                    }
                }
                break;
            }

            default:
                assert(0); // Undefined depth
                break;

        }

    }
*/
}


#endif


////////////////////////////////////////////////////////////////////////
