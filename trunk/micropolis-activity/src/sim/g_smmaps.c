/* g_smmaps.c
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
#include "sim.h"


int DynamicData[32];


#define DRAW_BEGIN \
  int col, row; \
  unsigned short tile; \
  short *mp; \
  unsigned char *imageBase; \
  unsigned char *image; \
  unsigned QUAD *mem; \
  unsigned QUAD l; \
  int lineBytes = view->line_bytes8; \
  int pixelBytes = view->pixel_bytes; \
  mp = &Map[0][0]; \
  imageBase = view->x->color ? view->data : view->data8; \
  for (col = 0; col < WORLD_X; col++) { \
    image = imageBase + (3 * pixelBytes * col); \
    for (row = 0; row < WORLD_Y; row++) { \
      tile = *(mp++) & LOMASK; \
      if (tile >= TILE_COUNT) tile -= TILE_COUNT;


#if defined(MSDOS) || defined(OSF1) || defined(IS_INTEL)

#define ROW1_8(n) \
      l = mem[n]; \
      image[0] = l; \
      image[1] = l >>8; \
      image[2] = l >>16; \
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
      l = mem[n]; \
      image[0] = l >>24; \
      image[1] = l >>16; \
      image[2] = l >>8; \
      image += lineBytes;

#define ROW1_16(n) \
      l = mem[n]; /* XXX: WRONG. handle depth */ \
      image[0] = l >>24; \
      image[1] = l >>16; \
      image[2] = l >>8; \
      image += lineBytes;

#define ROW1_24(n) \
      l = mem[n]; /* XXX: WRONG. handle depth */ \
      image[0] = l >>24; \
      image[1] = l >>16; \
      image[2] = l >>8; \
      image += lineBytes;

#define ROW1_32(n) \
      l = mem[n]; /* XXX: WRONG. handle depth */ \
      image[0] = l >>24; \
      image[1] = l >>16; \
      image[2] = l >>8; \
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
      mem = (unsigned QUAD *)&view->smalltiles[tile * 4 * 4 * pixelBytes]; \
      ROW3 \
    } \
  }


void drawAll(SimView *view)
{
  DRAW_BEGIN
  DRAW_END
}


void drawRes(SimView *view)
{
  DRAW_BEGIN
    if (tile > 422)
      tile = 0;
  DRAW_END
}


void drawCom(SimView *view)
{
  DRAW_BEGIN
    if ((tile > 609) ||
	((tile >= 232) && (tile < 423)))
      tile = 0;
  DRAW_END
}


void drawInd(SimView *view)
{
  DRAW_BEGIN
    if (((tile >= 240) && (tile <= 611)) ||
	((tile >= 693) && (tile <= 851)) ||
	((tile >= 860) && (tile <= 883)) ||
	(tile >= 932))
      tile = 0;
  DRAW_END
}


void drawLilTransMap(SimView *view)
{
  DRAW_BEGIN
    if ((tile >= 240) ||
	((tile >= 207) && tile <= 220) ||
	(tile == 223))
      tile = 0;
  DRAW_END
}


/* color pixel values */
#define UNPOWERED	COLOR_LIGHTBLUE
#define POWERED		COLOR_RED
#define CONDUCTIVE	COLOR_LIGHTGRAY


void drawPower(SimView *view)
{
  short col, row;
  unsigned short tile;
  short *mp;
  unsigned char *image, *imageBase;
  unsigned QUAD *mem;
  unsigned QUAD l;
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

  mp = &Map[0][0];
  imageBase = view->x->color ? view->data : view->data8;

  for (col = 0; col < WORLD_X; col++) {
    image = imageBase + (3 * pixelBytes * col);
    for (row = 0; row < WORLD_Y; row++) {
      tile = *(mp++);

      if ((tile & LOMASK) >= TILE_COUNT) tile -= TILE_COUNT;

      if ((unsigned short)(tile & LOMASK) <= (unsigned short)63) {
		tile &= LOMASK;
		pix = -1;
      } else if (tile & ZONEBIT) {
		pix = (tile & PWRBIT) ? powered : unpowered;
      } else {
		if (tile & CONDBIT) {
		  pix = conductive;
		} else {
		  tile = 0;
		  pix = -1;
		}
      }

      if (pix < 0) {
		mem = (unsigned QUAD *)&view->smalltiles[tile * 4 * 4 * pixelBytes];
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
		case 16:
		  { 
			unsigned short *p;
			p = (short *)image;
			p[0] = p[1] = p[2] = pix;
			image += lineBytes;
			p = (short *)image;
			p[0] = p[1] = p[2] = pix;
			image += lineBytes;
			p = (short *)image;
			p[0] = p[1] = p[2] = pix;
			image += lineBytes;
		  }
		  break;

		case 24:
		case 32:
		  { 
			int x, y;
			for (y = 0; y < 3; y++) {
			  unsigned char *img =
				image;
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
		  }
		  break;

		default:
		  assert(0); /* Undefined depth */
		  break;

		}
      }
    }
  }
}


int dynamicFilter(int col, int row)
{
  int r, c, x;

  r = row >>1;
  c = col >>1;

  if (((DynamicData[0] > DynamicData[1]) ||
       ((x = PopDensity[c][r])			>= DynamicData[0]) &&
       (x								<= DynamicData[1])) &&
      ((DynamicData[2] > DynamicData[3]) ||
       ((x = RateOGMem[c>>2][r>>2])		>= ((2 * DynamicData[2]) - 256)) &&
       (x								<= ((2 * DynamicData[3]) - 256))) &&
      ((DynamicData[4] > DynamicData[5]) ||
       ((x = TrfDensity[c][r])			>= DynamicData[4]) &&
       (x								<= DynamicData[5])) &&
      ((DynamicData[6] > DynamicData[7]) ||
       ((x = PollutionMem[c][r])		>= DynamicData[6]) &&
       (x								<= DynamicData[7])) &&
      ((DynamicData[8] > DynamicData[9]) ||
       ((x = CrimeMem[c][r])			>= DynamicData[8]) &&
       (x								<= DynamicData[9])) &&
      ((DynamicData[10] > DynamicData[11]) ||
       ((x = LandValueMem[c][r])		>= DynamicData[10]) &&
       (x								<= DynamicData[11])) &&
      ((DynamicData[12] > DynamicData[13]) ||
       ((x = PoliceMapEffect[c>>2][r>>2]) >= DynamicData[12]) &&
       (x								<= DynamicData[13])) &&
      ((DynamicData[14] > DynamicData[15]) ||
       ((x = FireRate[c>>2][r>>2])		>= DynamicData[14]) &&
       (x								<= DynamicData[15]))) {
    return 1;
  } else {
    return 0;
  } // if
}


void drawDynamic(SimView *view)
{
  DRAW_BEGIN
    if (tile > 63) {
      if (!dynamicFilter(col, row)) {
	tile = 0;
      } // if
    } // if
  DRAW_END
}


