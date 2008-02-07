/* g_cam.c
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


#ifdef CAM


Can *
new_can(int w, int h, Byte *mem, int line_bytes)
{
  Can *can = (Can *)malloc(sizeof(Can));

  can->width = w;
  can->height = h;
  can->line_bytes = line_bytes;
  can->mem = mem;

  return (can);
}


Cam *
new_cam(SimCam *scam, int x, int y, int w, int h, int dx, int dy, vf func)
{
  Cam *cam = (Cam *)malloc(sizeof(Cam));
  int ww, hh;

  cam->x = x;
  cam->y = y;
  cam->ideal_width = w;
  cam->ideal_height = h;
  w = (w + 1) & ~1;
  h = (h + 1) & ~1;
  ww = w + 2;
  hh = h + 2;
  cam->width = w;
  cam->height = h;
  cam->back = new_can(ww, hh, (Byte *)malloc(ww * hh), ww);
  cam->front = new_can(w, h,
		       (Byte *)scam->data + x + (y * scam->line_bytes),
		       scam->line_bytes);
  cam->neighborhood = func;
  cam->rule = NULL;
  cam->rule_size = 0;
  cam->phase = 0;
  cam->wrap = 3;
  cam->frob = -1;
  cam->steps = 1;
  cam->dx = dx;
  cam->dy = dy;
  cam->gx = 0;
  cam->gy = 0;
  cam->dragging = 0;
  cam->set_x = -1;
  cam->set_y = -1;
  cam->set_width = -1;
  cam->set_height = -1;
  cam->set_x0 = -1;
  cam->set_y0 = -1;
  cam->set_x1 = -1;
  cam->set_y1 = -1;
  cam->name = NULL;
  cam->next = scam->cam_list;
  scam->cam_list = cam;
  scam->cam_count++;

  return (cam);
}


scam_randomize(SimCam *scam)
{
  u_char *data = scam->data;
  int line_bytes = scam->line_bytes;
  int pixels = line_bytes * scam->w_height;
  int i;

  for (i = 0; i < pixels; i++) {
    *data = (char)(Rand16() >>4);
    data++;
  }
}


cam_randomize(Cam *cam)
{
  int x, y, w, h, lb;
  Byte *image;

  w = cam->width;
  h = cam->height;
  lb = cam->front->line_bytes;
  image = cam->front->mem;

  for (y = 0; y < h; y++, image += lb) {
    for (x = 0; x < w; x++) {
      image[x] = (char)(Rand16() >> 4);
    }
  }
}


cam_do_rule(SimCam *scam, Cam *cam)
{
  Byte *back, *front;
  int frontline, backline;
  int w, h;
  int steps = cam->steps;
  int step;

  back = cam->back->mem;
  backline = cam->back->line_bytes;

  front = cam->front->mem;
  frontline = cam->front->line_bytes;

  w = cam->width;
  h = cam->height;

  for (step=0; step < steps; step++) {
    int x, y;
    Byte *p = back + backline + 1,
	   *f = front;
/*
* Copy wrapping edges from front=>back:
*
*	0	ff	f0 f1 ... fe ff		f0
*
*	1	0f	00 01 ... 0e 0f		00
*	2	1f	10 11 ... 1e 1f		10
*		..	.. ..     .. ..		..
*		ef	e0 e1 ... ee ef		e0
*	h	ff	f0 f1 ... fe ff		f0
*
*	h+1	0f	00 01 ... 0e 0f		00
*
* wrap value:	effect:
*	0	no effect
*	1	copy front=>back, no wrap
*	2	no copy, wrap edges
*	3	copy front=>back, wrap edges
*	4	copy front=>back, same edges
*	5	copy edges from screen
*/

    switch (cam->wrap) {

    case 0:
      break;

    case 1:
      for (y=0; y<h; y++) {
	memcpy(p, f, w);
	p += backline;
	f += frontline;
      }
      break;

    case 2:
      for (y=0; y<h; y++) {
	p[-1] = p[w-1];
	p[w] = p[0];
	p += backline;
	f += frontline;
      }
      memcpy(back, back + backline*h, backline);
      memcpy(back + backline*(h+1), back + backline, backline);
      break;

    case 3:
      for (y=0; y<h; y++) {
	memcpy(p, f, w);
	p[-1] = p[w-1];
	p[w] = p[0];
	p += backline;
	f += frontline;
      }
      memcpy(back, back + backline*h, backline);
      memcpy(back + backline*(h+1), back + backline, backline);
      break;

    case 4:
      for (y=0; y<h; y++) {
	memcpy(p, f, w);
	p[-1] = p[0];
	p[w] =  p[w-1];
	p += backline;
	f += frontline;
      }
      memcpy(back + (backline * (h + 1)), back + backline*h, backline);
      memcpy(back, back + backline, backline);
      break;

    case 5:
      { int left = (cam->x == 0) ? 1 : 0;
	int right = (cam->x + cam->front->width == scam->w_width) ? 1 : 0;
	int top = (cam->y == 0) ? 1 : 0;
	int bottom = (cam->y + cam->front->height == scam->w_height) ? 1 : 0;

	if (!left && !right && !top && !bottom) {
	  p = back;
	  f = front - 1 - frontline;
	  for (y=-1; y<=h; y++) {
	    memcpy(p, f, w + 2);
	    p += backline;
	    f += frontline;
	  }
	} else {
	  p = back + backline + 1;
	  f = front;

	  p[-1 - backline] = f[left - 1 - (top ? 0 : frontline)];
	  memcpy(p - backline, f, w);
	  p[w - backline] = f[w - right - (top ? 0 : frontline)];

	  for (y=0; y<h; y++) {
	    p[-1] = f[left - 1];
	    memcpy(p, f, w);
	    p[w] = f[w - right];
	    p += backline;
	    f += frontline;
	  }
	}
      }
      break;

    }

    cam_slide(scam, cam);

    (*cam->neighborhood)(cam);
    cam->phase = !cam->phase;

    cam_update(scam, cam);

    cam_adjust(scam, cam);
  } /* for step */
}


cam_slide(SimCam *scam, Cam *cam)
{
  int x = cam->x;
  int y = cam->y;
  int last_x = x;
  int last_y = y;
  int width = cam->width;
  int height = cam->height;
  int dx = cam->dx;
  int dy = cam->dy;
  int dragging = cam->dragging;
  int bounce = 0;
int dagnabit = 0;

  if (dragging ||
      (cam->set_x >= 0) ||
      (cam->set_y >= 0)) {
    if (cam->set_x >= 0) {
      x = cam->set_x;
      cam->set_x = -1;
    }
    if (cam->set_y >= 0) {
      y = cam->set_y;
      cam->set_y = -1;
    }
  } else {
    x += dx;
    y += dy;
  }

  if (x < 0) {
    x = 0;
    if (!dragging) {
      dx = ABS(dx);
      bounce = 1;
    }
  } else if ((x + width) > scam->w_width) {
    x = scam->w_width - width;
    if (!dragging) {
      dx = -ABS(dx);
      bounce = 1;
    }
  }
  
  if (y < 0) {
    y = 0;
    if (!dragging) {
      dy = ABS(dy);
      bounce = 1;
    }
else {
dagnabit = 1;
printf("dagnabit\n");
}
  } else if ((y + height) > scam->w_height) {
    y = scam->w_height - height;
    if (!dragging) {
      dy = -ABS(dy);
      bounce = 1;
    }
  }
  
  if (dragging) {
    dx = x - last_x;
    dy = y - last_y;
  } else {
    if (bounce) {
      cam->frob = (Rand16() & 15) * ((cam->frob > 0) ? -1 : 1);
    }
  }

  cam->x = x;
  cam->y = y;
  cam->front->mem = (Byte *)scam->data + x + (scam->line_bytes * y);
  cam->dx = dx + cam->gx;
  cam->dy = dy + cam->gy;
if (dagnabit) {
  printf("x %d y %d dx %d dy %d\n", cam->x, cam->y, cam->dx, cam->dy);
}
}


cam_update(SimCam *scam, Cam *cam)
{
  if (scam->x->shared) {
    XShmPutImage(scam->x->dpy, Tk_WindowId(scam->tkwin), scam->x->gc,
		 scam->image, cam->x, cam->y, 
		 cam->x, cam->y, cam->width, cam->height,
		 False);
  } else {
    XPutImage(scam->x->dpy, Tk_WindowId(scam->tkwin), scam->x->gc,
	      scam->image, cam->x, cam->y, 
	      cam->x, cam->y, cam->width, cam->height);
  }
}


cam_adjust(SimCam *scam, Cam *cam)
{
  int x0 = cam->set_x0;
  int y0 = cam->set_y0;
  int x1 = cam->set_x1;
  int y1 = cam->set_y1;
  int width = cam->set_width;
  int height = cam->set_height;
  int min_size = 8;
  int tmp;

  if ((x0 >= 0) ||
      (y0 >= 0) ||
      (x1 >= 0) ||
      (y1 >= 0) ||
      (width > 0) ||
      (height > 0)) {
    if (x0 < 0) x0 = cam->x;
    if (y0 < 0) y0 = cam->y;
    if (x1 < 0) x1 = cam->x + cam->ideal_width;
    if (y1 < 0) y1 = cam->y + cam->ideal_height;
    if (width > 0) x1 = x0 + width;
    if (height > 0) y1 = y0 + height;

    cam->set_width = cam->set_height = 
      cam->set_x0 = cam->set_y0 =
      cam->set_x1 = cam->set_y1 = -1;

    if (x0 > x1) {
      tmp = x0; x0 = x1; x1 = tmp;
    }

    if (y0 > y1) {
      tmp = y0; y0 = y1; y1 = tmp;
    }

    if (x0 < 0) x0 = 0;
    if (y0 < 0) y0 = 0;
    if (x0 > scam->w_width - min_size) x0 = scam->w_width - min_size;
    if (y0 > scam->w_height - min_size) x0 = scam->w_height - min_size;
    if (x1 < x0 + min_size) x1 = x0 + min_size;
    if (y1 < y0 + min_size) y1 = y0 + min_size;
    if (x1 > scam->w_width) x1 = scam->w_width;
    if (y1 > scam->w_height) y1 = scam->w_height;

    cam->x = x0;
    cam->y = y0;
    cam->ideal_width = x1 - x0;
    cam->ideal_height = y1 - y0;
    cam->width = cam->ideal_width & ~1;
    cam->height = cam->ideal_height & ~1;
    cam->front->mem = (Byte *)scam->data + x0 + (y0 * scam->line_bytes);
    cam->front->width = cam->width;
    cam->front->height = cam->height;
    free(cam->back->mem);
    cam->back->mem = (Byte *)malloc((cam->width + 2) * (cam->height + 2));
    cam->back->width = cam->width + 2;
    cam->back->height = cam->height + 2;
    cam->back->line_bytes = cam->back->width;
  }
}


void
n_moore_a(Cam *cam)
{
    /* 0    1    2    3    4    5    6    7    8     9     */
    /* c    c'   se   sw   ne   nw   e    w    s     n     */
    /* 0x1  0x2  0x4  0x8  0x10 0x20 0x40 0x80 0x100 0x200 */

#define MOORE_A (							\
	((NORTHWEST&1)<<5) |	((NORTH&1)<<9) |((NORTHEAST&1)<<4) |	\
	((WEST&1)<<7) |		(CENTER&3) |	((EAST&1)<<6) |		\
	((SOUTHWEST&1)<<3) |	((SOUTH&1)<<8) |((SOUTHEAST&1)<<2)	\
    )

    CAM_TABLE_LOOP(MOORE_A)
}


void
n_moore_ab(Cam *cam)
{
    /* 0    1    2    3    4    5    6    7    8     9     10    11    */
    /* c    c'   se   sw   ne   nw   e    w    s     n     &c    &c'   */
    /* 0x1  0x2  0x4  0x8  0x10 0x20 0x40 0x80 0x100 0x200 0x400 0x800 */

#define MOORE_AB (MOORE_A | ((CENTER&12)<<8))

    CAM_TABLE_LOOP(MOORE_AB)
}


void
n_vonn_neumann(Cam *cam)
{
    /* 0    1    2    3    4    5    6    7    8     9     */
    /* c    c'   e'   w'   s'   n'   e    w    s     n     */
    /* 0x1  0x2  0x4  0x8  0x10 0x20 0x40 0x80 0x100 0x200 */

#define VON_NEUMANN (							\
	(CENTER&3) |							\
 	((EAST&1)<<6) | ((EAST&2)<<1) |					\
	((WEST&1)<<7) | ((WEST&2)<<2) |					\
	((SOUTH&1)<<8) | ((SOUTH&2)<<3) |				\
	((NORTH&1)<<9) | ((NORTH&2)<<4)					\
    )

    CAM_TABLE_LOOP(VON_NEUMANN)
}


void
n_margolis(Cam *cam)
{
    register Byte i;

    /* 0    1    2    3    4    5    6    7    8    9      */
    /* c    c'   cw   ccw  opp  cw'  ccw' opp'             */
    /* 0x1  0x2  0x4  0x8  0x10 0x20 0x40 0x80 0x100 0x200 */

#define MARGOLIS_ODD (							\
	(CENTER & 3) |							\
	(i=(x&1 ? (y&1 ? (EAST) : (NORTH))				\
		: (y&1 ? (SOUTH) : (WEST))),				\
	 (((i&1)<<2) | ((i&2)<<4))) |					\
	(i=(x&1 ? (y&1 ? (SOUTH) : (EAST))				\
		: (y&1 ? (WEST) : (NORTH))),				\
	 (((i&1)<<3) | ((i&2)<<5))) |					\
	(i=(x&1 ? (y&1 ? (SOUTHEAST):(NORTHEAST))			\
		: (y&1 ? (SOUTHWEST):(NORTHWEST))),			\
	 (((i&1)<<4) | ((i&2)<<6)))					\
    )

#define MARGOLIS_EVEN (							\
	(CENTER & 3) |							\
	(i=(x&1 ? (y&1 ? (WEST) : (SOUTH))				\
		: (y&1 ? (NORTH) : (EAST))),				\
	 (((i&1)<<2) | ((i&2)<<4))) |					\
	(i=(x&1 ? (y&1 ? (NORTH) : (WEST))				\
		: (y&1 ? (EAST) : (SOUTH))),				\
	 (((i&1)<<3) | ((i&2)<<5))) |					\
	(i=(x&1 ? (y&1 ? (NORTHWEST) : (SOUTHWEST))			\
		: (y&1 ? (NORTHEAST) : (SOUTHEAST))),			\
	 (((i&1)<<4) | ((i&2)<<6)))					\
    )

    if (cam->phase) {
        CAM_TABLE_LOOP(MARGOLIS_ODD)
    } else {
        CAM_TABLE_LOOP(MARGOLIS_EVEN)
    }
}


void
n_margolis_ph(Cam *cam)
{
    register Byte i;

    /* 0    1    2    3    4    5    6    7    8    9      */
    /* c    c'   cw   ccw  opp  cw'  ccw' opp' pha   pha'  */
    /* 0x1  0x2  0x4  0x8  0x10 0x20 0x40 0x80 0x100 0x200 */

#define MARGOLIS_ODD_PH (MARGOLIS_ODD | 0x100)
#define MARGOLIS_EVEN_PH (MARGOLIS_EVEN | 0x200)

    if (cam->phase) {
	CAM_TABLE_LOOP(MARGOLIS_ODD_PH)
    } else {
	CAM_TABLE_LOOP(MARGOLIS_EVEN_PH)
    }
}


void
n_margolis_hv(Cam *cam)
{
    register Byte i;

    /* 0    1    2    3    4    5    6    7    8    9      */
    /* c    c'   cw   ccw  opp  cw'  ccw' opp' horz  vert  */
    /* 0x1  0x2  0x4  0x8  0x10 0x20 0x40 0x80 0x100 0x200 */

#define MARGOLIS_ODD_HV (MARGOLIS_ODD | ((x&1)<<8) | ((y&1)<<9))
#define MARGOLIS_EVEN_HV (MARGOLIS_EVEN | ((x&1)<<8) | ((y&1)<<9))

    if (cam->phase) {
	CAM_TABLE_LOOP(MARGOLIS_ODD_HV)
    } else {
	CAM_TABLE_LOOP(MARGOLIS_EVEN_HV)
    }
}


void
n_life(Cam *cam)
{
  int s;

#define LIFE (							\
	 ((CENTER&1) ? (((s = SUM8) == 2) || (s == 3))		\
	 	     : (SUM8 == 3)) |				\
	 (CENTER<<1)						\
	)

  CAM_LOOP(LIFE)
}


void
n_brain(Cam *cam)
{
  int s;

#define BRAIN (							\
	 (((((s = CENTER)&3) == 0) && (SUM8 == 2)) ? 1 : 0) |	\
	 (s<<1)							\
	)

  CAM_LOOP(BRAIN)
}


void
n_heat(Cam *cam)
{
  int frob = cam->frob;

#define HEAT (								\
	((QUAD)(NORTHWEST + NORTH + NORTHEAST +				\
		WEST + EAST +						\
		SOUTHWEST + SOUTH + SOUTHEAST + frob)) >> 3	\
    )

    CAM_LOOP(HEAT)
}


void
n_dheat(Cam *cam)
{
  int frob = cam->frob;
  int last = 0;

#define DHEAT \
	last += NORTHWEST + NORTH + NORTHEAST + \
		WEST + frob + EAST + \
		SOUTHWEST + SOUTH + SOUTHEAST; \
	*front = last >> 3; \
	last &= 7;
    CAM_LOOP_BODY(DHEAT)

}


void
n_lheat(Cam *cam)
{
  int frob = cam->frob;

#define LHEAT (								\
	((QUAD)(NORTH + WEST + EAST + SOUTH + frob)) >> 2		\
    )

    CAM_LOOP(LHEAT)
}


void
n_ldheat(Cam *cam)
{
  int frob = cam->frob;
  int last; /* I meant to do that! */

#define LDHEAT (							\
	((last = (QUAD)(NORTH + WEST + EAST + SOUTH + frob		\
			+ (last&0x03))), last >> 2)			\
  )

  CAM_LOOP(LDHEAT)
}


void
n_abdheat(Cam *cam)
{
  int frob = cam->frob;
  int lasta = 0, lastb = 0; /* I meant to do that! */

#define YUM(x) (((QUAD)(x))&0x0f)
#define YUK(x) (((QUAD)(x))&0xf0)

#define ABDHEAT (							\
	(lasta = (QUAD)(YUM(NORTHWEST) + YUM(NORTH) + YUM(NORTHEAST) +	\
			YUM(WEST) + YUM(EAST) +				\
			YUM(SOUTHWEST) + YUM(SOUTH) + YUM(SOUTHEAST) +	\
			frob + (lasta&0x07))),			\
	(lastb = (QUAD)(YUK(NORTHWEST) + YUK(NORTH) + YUK(NORTHEAST) +	\
			YUK(WEST) + YUK(EAST) +				\
			YUK(SOUTHWEST) + YUK(SOUTH) + YUK(SOUTHEAST) +	\
			(frob<<4) + (lastb&0x70))),		\
	(((lasta>>3)&0x0f) | ((lastb>>3)&0xf0))				\
    )

    CAM_LOOP(ABDHEAT)
}


void
n_abcdheat(Cam *cam)
{
  int last;
  int frob = cam->frob;

  CAM_LOOP(HEAT)
}


void
n_edheat(Cam *cam)
{
  int frob = cam->frob;
  int last = 0;

#define EDHEAT (							\
	(last = (QUAD)(YUM(NORTHWEST) + YUM(NORTH) + YUM(NORTHEAST) +	\
		       YUM(WEST) + YUM(EAST) +				\
		       YUM(SOUTHWEST) + YUM(SOUTH) + YUM(SOUTHEAST) +	\
		       frob + (last&0x07))),			\
	(((last>>3)&0x0f) | ((CENTER<<4)&0xf0))				\
    )

    CAM_LOOP(EDHEAT)
}


int ranch(QUAD l0, QUAD l1, QUAD l2)
{
  int s = SUM8;
  int v = SUM9p(1);
  int o = 0;

  o = (CENTER&4)<<1;

  if (v < 4 || v == 5) {
    o |= 0x04;
  } else {
  }

  return (o);
} 


void
n_ranch(Cam *cam)
{
#define RANCH ranch(l0, l1, l2)

  CAM_LOOP(RANCH)
}


void
n_anneal(Cam *cam)
{
  int s;

#define ANNEAL (							\
	  ((s = SUM9) > 5) || (s == 4)					\
	)
  CAM_LOOP(ANNEAL)
}


void
n_anneal4(Cam *cam)
{
  int s;

#define ANNEAL4 (							\
	  ((((s = SUM9p(0)) > 5) || (s == 4)) ? 1 : 0) |		\
	  ((((s = SUM9p(1)) > 5) || (s == 4)) ? 2 : 0) |		\
	  ((((s = SUM9p(2)) > 5) || (s == 4)) ? 4 : 0) |		\
	  ((((s = SUM9p(3)) > 5) || (s == 4)) ? 8 : 0) |		\
	  (CENTER << 4)							\
	)
  CAM_LOOP(ANNEAL4)
}


void
n_anneal8(Cam *cam)
{
  int s;

#define ANNEAL8 (							\
	  ((((s = SUM9p(0)) > 5) || (s == 4)) ? 1 : 0) |		\
	  ((((s = SUM9p(1)) > 5) || (s == 4)) ? 2 : 0) |		\
	  ((((s = SUM9p(2)) > 5) || (s == 4)) ? 4 : 0) |		\
	  ((((s = SUM9p(3)) > 5) || (s == 4)) ? 8 : 0) |		\
	  ((((s = SUM9p(4)) > 5) || (s == 4)) ? 16 : 0) |		\
	  ((((s = SUM9p(5)) > 5) || (s == 4)) ? 32 : 0) |		\
	  ((((s = SUM9p(6)) > 5) || (s == 4)) ? 64 : 0) |		\
	  ((((s = SUM9p(7)) > 5) || (s == 4)) ? 128 : 0)		\
	)
  CAM_LOOP(ANNEAL8)
}


void
n_eco(Cam *cam)
{
  int s;

#define ANTILIFE (							\
	 ((CENTER&1) ? (SUM8 != 5)					\
	 	     : (((s = SUM8) != 5) && (s != 6))) |		\
	 (CENTER<<1)							\
	)

#define ECO (								\
	  (((s = SUM9p(7)) > 5) || (s == 4) ? 128 : 0) |		\
	  ((CENTER&128) ? ((ANTILIFE)&127) : ((BRAIN)&127))		\
	)
  CAM_LOOP(ECO)
}


void
n_torben(Cam *cam)
{
  int s;

/* 0 0 0 1 0 1 0 1 1 1 */

#define TORBEN (							\
 (CENTER << 1) | ((((s = SUM9) > 6) || (s == 5) || (s == 3)) ? 1 : 0)	\
	)
  CAM_LOOP(TORBEN)
}


void
n_torben2(Cam *cam)
{
  int s;


/* 0 0 0 1 0 1 0 1 1 1 */
/* 0 0 1 0 1 0 1 0 1 1 */
#define TORBEN2 (							\
	  TORBEN | (CENTER <<1)						\
	)
  CAM_LOOP(TORBEN2)
}


void
n_torben3(Cam *cam)
{
  int s;

/* 0 0 0 1 1 0 0 1 1 1 */

#define TORBEN3 (							\
	  ((s = SUM9) > 6) || (s == 3) || (s == 4)			\
	)
  CAM_LOOP(TORBEN3)
}


void
n_torben4(Cam *cam)
{
  int s;


/* 0 0 0 1 0 1 0 1 1 1 */
/* 0 0 1 0 1 0 1 0 1 1 */
#define TORBEN4 (							\
	  TORBEN3 | (CENTER <<1)					\
	)
  CAM_LOOP(TORBEN4)
}


void
n_ball(Cam *cam)
{
  char p = (cam->phase ? 1 : 0);
  int x, y, r = Rand16();
  int backline = cam->back->line_bytes,
      frontline = cam->front->line_bytes;
  Byte *back = cam->back->mem,
	 *front = cam->front->mem;

  if (!p) {
    back += 1 + backline;
  }

  for (y = p + (cam->height >>1); y > 0; y--) {
    for (x = p + (cam->width >>1); x > 0; x--) {
      Byte nw = back[0], ne = back[1],
             sw = back[backline], se = back[backline+1];
      
      r += nw + ne + sw + sw; r >>= 1;

      switch ((nw&1) + (ne&1) + (sw&1) + (se&1)) {
      case 0:
      case 3:
      case 4:
	/* same */
	back[0] = nw; back[1] = ne;
	back[backline] = sw; back[backline+1] = se;
	break;
      case 1:
	/* reflect */
	back[0] = se; back[1] = sw;
	back[backline] = ne; back[backline+1] = nw;
	break;
      case 2:
	/* turn */
	switch (((nw&1) <<3) | ((ne&1) <<2) | ((sw&1) <<1) | (se&1)) {
	case 6:
	case 9:
	  if (r&1) {
	    /* clockwise */
	    back[0] = sw; back[1] = nw;
	    back[backline] = se; back[backline+1] = ne;
	  } else {
	    /* counterclockwise */
	    back[0] = ne; back[1] = se;
	    back[backline] = nw; back[backline+1] = sw;
	  }
	default:
	  back[0] = nw; back[1] = ne;
	  back[backline] = sw; back[backline+1] = se;
	  break;
	}
	break;
      }

      back += 2;
    }
    back += backline + backline - cam->width - (2*p);
  }
  back = cam->back->mem + backline + 1,
  front = cam->front->mem;

  for (y = cam->height; y > 0; y--) {
    memcpy(front, back, cam->width);
    back += backline;
    front += frontline;
  }
}


void
n_logic(Cam *cam)
{
  char p = (cam->phase ? 1 : 0);
  int x, y;
  int backline = cam->back->line_bytes,
      frontline = cam->front->line_bytes;
  Byte tmp;
  Byte *back = cam->back->mem,
	 *front = cam->front->mem;

  /* bit 8 is center' */
  /* bit 7 is center */

  if (cam->phase) {
    back += 1 + backline;
    front += 1 + frontline;
  }

  for (y = cam->height >>1; y > 0; y--) {
    for (x = cam->width >>1; x > 0; x--) {
      Byte nw = back[0], ne = back[1],
             sw = back[backline], se = back[backline+1];
      
      switch (((nw&128) >>4) | ((ne&128) >>5) |
	      ((sw&128) >>6) | ((se&128) >>7)) {
      case 0:
	break;

      case 1:
	break;

      case 2:
	break;

      case 3:
	break;

      case 4:
	break;

      case 5:
	break;

      case 6:
	break;

      case 7:
	break;

      case 8:
	break;

      case 9:
	break;

      case 10:
	break;

      case 11:
	break;

      case 12:
	break;

      case 13:
	break;

      case 14:
	break;

      case 15:
	tmp = nw; nw = se; se = tmp;
	tmp = sw; ne = sw; sw = tmp;
	break;

      }

      back += 2; front += 2;
    }
    back += backline + backline - cam->width;
    front += frontline + frontline - cam->width;
  }

  back = cam->back->mem + backline + 1,
  front = cam->front->mem;

  for (y = cam->height; y > 0; y--) {
    memcpy(front, back, cam->width);
    back += backline;
    front += frontline;
  }
}


void
n_party(Cam *cam)
{
#define PARTY (CENTER ^ NORTH ^ SOUTH ^ EAST ^ WEST)
  CAM_LOOP(PARTY)
}


vf neighborhoods[] = {
/* 0 1 2 3 */
  &n_moore_a, &n_moore_a, &n_moore_ab, &n_vonn_neumann,
/* 4 5 6 */
  &n_margolis, &n_margolis_ph, &n_margolis_hv, 
/* 7 8			9 10 11 12 */
  &n_life, &n_brain, &n_heat, &n_dheat, &n_lheat, &n_ldheat,
/* 13 14 15 16 17 */
  &n_ranch, &n_anneal, &n_anneal4, &n_anneal8, &n_eco,
/* 18 19 20 21 22 */
  &n_abdheat, &n_edheat, &n_abcdheat, &n_torben, &n_torben2,
/* 23 24 25 26 27 */
  &n_torben3, &n_torben4, &n_ball, &n_logic, &n_party
};


cam_set_neighborhood(Cam *cam, int code)
{
  cam->neighborhood = neighborhoods[code];
}


cam_load_rule(Cam *cam, char *filename)
{
  FILE *fp;
  QUAD magic, neighborhood, rule_size;
  Byte *rule;

  if ((fp = fopen(filename, "r")) == NULL) {
    fprintf(stderr, "cam: Can't open rule file \"%s\"\n", filename);
    return;
  }

/* XXX: Make this byte order independent!!! */

#if defined(MSDOS) || defined(OSF1) || defined(IS_INTEL)

#define SWAPQUAD(x) ((x = ((x <<24) & 0xff000000) | \
			  ((x <<8)  & 0x00ff0000) | \
			  ((x >>8)  & 0x0000ff00) | \
			  ((x >>24) & 0x000000ff)), 0)

#else

#define SWAPQUAD(x) 0

#endif

  if ((fread(&magic, 1, sizeof(QUAD), fp) != sizeof(QUAD)) ||
      SWAPQUAD(magic) ||
      (magic != 0xcac0cac0) ||
      (fread(&neighborhood, 1, sizeof(QUAD), fp) != sizeof(QUAD)) ||
      SWAPQUAD(neighborhood) ||
      (fread(&rule_size, 1, sizeof(QUAD), fp) != sizeof(QUAD)) ||
      SWAPQUAD(rule_size) ||
      ((rule = (Byte *)malloc(rule_size)) == NULL) ||
      (fread(rule, 1, rule_size, fp) != rule_size)) {
    fprintf(stderr, "cam: Bad rule file \"%s\"\n", filename);
    fclose(fp);
    return;
  }

  fclose(fp);
  if (cam->rule != NULL)
    free(cam->rule);
  cam->rule = rule;
  cam->rule_size = rule_size;
  cam_set_neighborhood(cam, neighborhood);
}


Cam *
find_cam(SimCam *scam, int x, int y)
{
  Cam *cam;

  for (cam = scam->cam_list; cam != NULL; cam = cam->next) {
    if ((x >= cam->x) &&
	(y >= cam->y) &&
	(x < cam->x + cam->width) &&
	(y < cam->y + cam->height)) {
      break;
    }
  }

  return cam;
}


Cam *
find_cam_by_name(SimCam *scam, char *name)
{
  Cam *cam;

  for (cam = scam->cam_list; cam != NULL; cam = cam->next) {
    if ((cam->name != NULL) &&
	(strcmp(name, cam->name) == 0)) {
      return cam;
    }
  }

  return NULL;
}


Cam *
get_cam_number(SimCam *scam, int i)
{
  Cam *cam;

  for (cam = scam->cam_list;
       (i != 0) && (cam != NULL);
       (i--), (cam = cam->next)) ;

  return cam;
}


cam_layout(SimCam *scam)
{
  int x, y, gap, border, maxwidth, lastmax;
  Cam *cam;

  border = 8;
  gap = 8;
  x = border; y = border;
  maxwidth = lastmax = gap;

  for (cam = scam->cam_list; cam != NULL; cam = cam->next) {
    cam->dx = 0; cam->dy = 0;    
    if (cam->width > maxwidth) {
      lastmax = maxwidth;
      maxwidth = cam->width;
    }
    if (y + cam->height + border > scam->w_height) {
      y = border; x = x + maxwidth + gap;
      maxwidth = lastmax = gap;
    }
    if ((x + cam->width > scam->w_width) ||
	(y + cam->height > scam->w_height)) {
      cam->x = 0; cam->y = 0;
      cam->front->mem = (Byte *)scam->data;
      maxwidth = lastmax;
    } else {
      cam->x = x; cam->y = y;
      cam->front->mem =
	scam->data + x + (scam->line_bytes * y);
      y = y + cam->height + gap;
    }
  }
}


init_scam(SimCam *scam)
{
  scam_randomize(scam);
}


handle_scam(SimCam *scam)
{
  Cam *cam;

  for (cam = scam->cam_list; cam != NULL; cam = cam->next) {
    cam_do_rule(scam, cam);
  }
}


#endif /* CAM */
