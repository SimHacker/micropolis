/* g_setup.c
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


#define SIM_SMTILE	385
#define SIM_BWTILE	386
#define SIM_GSMTILE	388
#define SIM_LGTILE	544


#define gray25_width 16
#define gray25_height 16
static unsigned char gray25_bits[] = {
  0x77, 0x77,
  0xdd, 0xdd,
  0x77, 0x77,
  0xdd, 0xdd,
  0x77, 0x77,
  0xdd, 0xdd,
  0x77, 0x77,
  0xdd, 0xdd,
  0x77, 0x77,
  0xdd, 0xdd,
  0x77, 0x77,
  0xdd, 0xdd,
  0x77, 0x77,
  0xdd, 0xdd,
  0x77, 0x77,
  0xdd, 0xdd,
};


#define gray50_width 16
#define gray50_height 16
static unsigned char gray50_bits[] = {
  0x55, 0x55,
  0xaa, 0xaa,
  0x55, 0x55,
  0xaa, 0xaa,
  0x55, 0x55,
  0xaa, 0xaa,
  0x55, 0x55,
  0xaa, 0xaa,
  0x55, 0x55,
  0xaa, 0xaa,
  0x55, 0x55,
  0xaa, 0xaa,
  0x55, 0x55,
  0xaa, 0xaa,
  0x55, 0x55,
  0xaa, 0xaa,
};


#define gray75_width 16
#define gray75_height 16
static unsigned char gray75_bits[] = {
  0x88, 0x88,
  0x22, 0x22,
  0x88, 0x88,
  0x22, 0x22,
  0x88, 0x88,
  0x22, 0x22,
  0x88, 0x88,
  0x22, 0x22,
  0x88, 0x88,
  0x22, 0x22,
  0x88, 0x88,
  0x22, 0x22,
  0x88, 0x88,
  0x22, 0x22,
  0x88, 0x88,
  0x22, 0x22,
};

#define vert_width 16
#define vert_height 16
static unsigned char vert_bits[] = {
  0xaa, 0xaa,
  0xaa, 0xaa,
  0xaa, 0xaa,
  0xaa, 0xaa,
  0xaa, 0xaa,
  0xaa, 0xaa,
  0xaa, 0xaa,
  0xaa, 0xaa,
  0xaa, 0xaa,
  0xaa, 0xaa,
  0xaa, 0xaa,
  0xaa, 0xaa,
  0xaa, 0xaa,
  0xaa, 0xaa,
  0xaa, 0xaa,
  0xaa, 0xaa,
};


#define horiz_width 16
#define horiz_height 16
static unsigned char horiz_bits[] = {
  0xff, 0xff,
  0x00, 0x00,
  0xff, 0xff,
  0x00, 0x00,
  0xff, 0xff,
  0x00, 0x00,
  0xff, 0xff,
  0x00, 0x00,
  0xff, 0xff,
  0x00, 0x00,
  0xff, 0xff,
  0x00, 0x00,
  0xff, 0xff,
  0x00, 0x00,
  0xff, 0xff,
  0x00, 0x00,
};


#define diag_width 16
#define diag_height 16
static unsigned char diag_bits[] = {
  0x55, 0x55, 
  0xee, 0xee, 
  0x55, 0x55, 
  0xba, 0xbb,
  0x55, 0x55, 
  0xee, 0xee, 
  0x55, 0x55, 
  0xba, 0xbb,
  0x55, 0x55, 
  0xee, 0xee,
  0x55, 0x55, 
  0xba, 0xbb,
  0x55, 0x55, 
  0xee, 0xee, 
  0x55, 0x55, 
  0xba, 0xbb,
};


Ptr MickGetHexa(short theID)
{
  Handle theData;

  theData = GetResource("hexa", theID);
  return ((Ptr)*theData);
}


Pixmap *
GetObjectXpms(XDisplay *xd, int id, int frames)
{
  int i;
  Pixmap *pixmaps = (Pixmap *)ckalloc(2 * frames * sizeof (Pixmap));
  XpmAttributes attributes;
  char name[256];

  attributes.visual = Tk_DefaultVisual(xd->screen);
  attributes.colormap = Tk_DefaultColormap(xd->screen);
  attributes.depth = Tk_DefaultDepth(xd->screen);
  attributes.valuemask = XpmVisual | XpmColormap | XpmDepth;

  for (i = 0; i < frames; i++) {
    sprintf(name, "%s/images/obj%d-%d.xpm", HomeDir, id, i);
    if (XpmReadFileToPixmap(xd->dpy, 
			    RootWindowOfScreen(xd->screen),
			    name,
			    &pixmaps[i + i], &pixmaps[i + i + 1],
			    &attributes) < 0) {
      fprintf(stderr,
	      "Uh oh, Micropolis couldn't read the pixmap file \"%s\".\n",
	      name);
      sim_exit(1); // Just sets tkMustExit and ExitReturn
      return NULL;
    }
  }
  return (pixmaps);
}


GetPixmaps(XDisplay *xd)
{
  if (xd->gray25_stipple == None) {
    xd->gray25_stipple =
      XCreatePixmapFromBitmapData(xd->dpy, xd->root,
				  gray25_bits, gray25_width, gray25_height,
				  1, 0, 1);
    xd->gray50_stipple =
      XCreatePixmapFromBitmapData(xd->dpy, xd->root,
				  gray50_bits, gray50_width, gray50_height,
				  1, 0, 1);
    xd->gray75_stipple =
      XCreatePixmapFromBitmapData(xd->dpy, xd->root,
				  gray75_bits, gray75_width, gray75_height,
				  1, 0, 1);
    xd->vert_stipple =
      XCreatePixmapFromBitmapData(xd->dpy, xd->root,
				  vert_bits, vert_width, vert_height,
				  1, 0, 1);
    xd->horiz_stipple =
      XCreatePixmapFromBitmapData(xd->dpy, xd->root,
				  horiz_bits, horiz_width, horiz_height,
				  1, 0, 1);
    xd->diag_stipple =
      XCreatePixmapFromBitmapData(xd->dpy, xd->root,
				  diag_bits, diag_width, diag_height,
				  1, 0, 1);
  }

  if (xd->objects == NULL) {
    Pixmap **pm;

    xd->objects = pm = (Pixmap **)ckalloc(OBJN * sizeof (Pixmap *));

    pm[0] = NULL; /* no object number 0 */
    pm[TRA] = GetObjectXpms(xd, TRA, 5);
    pm[COP] = GetObjectXpms(xd, COP, 8);
    pm[AIR] = GetObjectXpms(xd, AIR, 11);
    pm[SHI] = GetObjectXpms(xd, SHI, 8);
    pm[GOD] = GetObjectXpms(xd, GOD, 16);
    pm[TOR] = GetObjectXpms(xd, TOR, 3);
    pm[EXP] = GetObjectXpms(xd, EXP, 6);
    pm[BUS] = GetObjectXpms(xd, BUS, 4);
  }
}


GetViewTiles(SimView *view)
{
  char name[256];
  XpmAttributes attributes;

  attributes.visual = Tk_DefaultVisual(view->x->screen);
  attributes.colormap = Tk_DefaultColormap(view->x->screen);
  attributes.depth = Tk_DefaultDepth(view->x->screen);
  attributes.valuemask = XpmVisual | XpmColormap | XpmDepth;

  if (view->class == Editor_Class) {

    sprintf(name, "%s/images/%s", HomeDir,
	    view->x->color ? "tiles.xpm" : "tilesbw.xpm");

    switch (view->type) {

    case X_Mem_View:
      if (view->x->big_tile_image == NULL) {
	if (XpmReadFileToImage(view->x->dpy, name,
			       &view->x->big_tile_image, NULL,
			       &attributes) < 0) {
	  fprintf(stderr,
		  "Uh oh, Micropolis couldn't read the pixmap file \"%s\".\n",
		  name);
	  sim_exit(1); // Just sets tkMustExit and ExitReturn
	  return;
	}
      }
      view->bigtiles = (unsigned char *)view->x->big_tile_image->data;
      break;

    case X_Wire_View:
      if (view->x->big_tile_pixmap == None) {
	if (XpmReadFileToPixmap(view->x->dpy,
				RootWindowOfScreen(view->x->screen),
				name,
				&view->x->big_tile_pixmap, NULL,
				&attributes) < 0) {
	  fprintf(stderr,
		  "Uh oh, Micropolis couldn't read the pixmap file \"%s\".\n",
		  name);
	  sim_exit(1); // Just sets tkMustExit and ExitReturn
	  return;
	}
      }
      break;

    }

  } else if (view->class == Map_Class) {

    if (view->x->small_tile_image == NULL) {
      if (view->x->color) {

	sprintf(name, "%s/images/%s", HomeDir, "tilessm.xpm");
	if (XpmReadFileToImage(view->x->dpy, name,
			       &view->x->small_tile_image, NULL,
			       &attributes) < 0) {
	  fprintf(stderr,
		  "Uh oh, Micropolis couldn't read the pixmap file \"%s\".\n",
		  name);
	  sim_exit(1); // Just sets tkMustExit and ExitReturn
	  return;
	}

      } else {

	view->x->small_tile_image = 
	  XCreateImage(view->x->dpy, view->x->visual, 8,
		       ZPixmap,
		       0, (char *)MickGetHexa(SIM_GSMTILE),
		       4, 3 * TILE_COUNT, 8, 4);

      }
    }

    { int x, y, b, tile;
      unsigned char *from, *to;
      int pixelBytes = view->pixel_bytes;
      int rowBytes = view->x->small_tile_image->bytes_per_line;
      
      if (pixelBytes == 0) {
	/* handle the case of monochrome display (8 bit map) */
	pixelBytes = 1;
      }

      /* from is 4 pixels wide per 3 pixel wide tile */
      from = (unsigned char *)view->x->small_tile_image->data;
      to = (unsigned char *)ckalloc(4 * 4 * TILE_COUNT * pixelBytes);
      view->smalltiles = to;

      switch (pixelBytes) {

      case 1:
	for (tile = 0; tile < TILE_COUNT; tile++) {
	  for (y = 0; y < 3; y++) {
	    for (x = 0; x < 4; x++) {
	      *to++ = *from++;
	    }
	  }
	  for (x = 0; x < 4; x++) {
	    *to++ = 0;
	  }
	}
	break;

      case 2:
	for (tile = 0; tile < TILE_COUNT; tile++) {
	  for (y = 0; y < 3; y++) {
	    for (x = 0; x < 4; x++) {
	      *to++ = *from++;
	      *to++ = *from++;
	    }
	  }
	  for (x = 0; x < 4; x++) {
	    *to++ = 0;
	    *to++ = 0;
	  }
	}
	break;

      case 3:
      case 4:
	for (tile = 0; tile < TILE_COUNT; tile++) {
	  for (y = 0; y < 3; y++) {
	    for (x = 0; x < 4; x++) {
	      *to++ = *from++;
	      *to++ = *from++;
	      *to++ = *from++;
	      if (pixelBytes == 4) {
		*to++ = *from++;
	      }
	    }
	  }
	  for (x = 0; x < 4; x++) {
	    *to++ = 0;
	    *to++ = 0;
	    *to++ = 0;
	    if (pixelBytes == 4) {
	      *to++ = 0;
	    }
	  }
	}
	break;

      default:
	assert(0); /* Undefined depth */
	break;

      }

    }
  }
}
