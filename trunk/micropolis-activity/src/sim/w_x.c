/* w_x.c:  X Window System support
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


struct XDisplay *XDisplays = NULL;
int DisplayCount = 0;
#ifdef IS_LINUX
int FlushStyle = 3;
#else
int FlushStyle = 4;
#endif
int GotXError;


unsigned char ColorIntensities[] = {
	/* COLOR_WHITE */		255,
	/* COLOR_YELLOW */		170,
	/* COLOR_ORANGE */		127,
	/* COLOR_RED */			85,
	/* COLOR_DARKRED */		63,
	/* COLOR_DARKBLUE */		76,
	/* COLOR_LIGHTBLUE */		144,
	/* COLOR_BROWN */		118,
	/* COLOR_LIGHTGREEN */		76,
	/* COLOR_DARKGREEN */		42,
	/* COLOR_OLIVE */		118,
	/* COLOR_LIGHTBROWN */		144,
	/* COLOR_LIGHTGRAY */		191,
	/* COLOR_MEDIUMGRAY */		127,
	/* COLOR_DARKGRAY */		63,
	/* COLOR_BLACK */		0,
};


ViewToTileCoords(SimView *view, int x, int y, int *outx, int *outy)
{
  x = (view->pan_x - ((view->w_width >>1) - x)) >>4;
  y = (view->pan_y - ((view->w_height >>1) - y)) >>4;

  if (x < 0) x = 0;
  if (x >= WORLD_X) x = WORLD_X - 1;
  if (y < 0) y = 0;
  if (y >= WORLD_Y) y = WORLD_Y - 1;

  if (x < view->tile_x)
    x = view->tile_x;
  if (x >= view->tile_x + view->tile_width)
    x = view->tile_x + view->tile_width - 1;
  if (y < view->tile_y)
    y = view->tile_y;
  if (y >= view->tile_y + view->tile_height)
    y = view->tile_y + view->tile_height - 1;

  if (view->tool_x_const != -1)
    x = view->tool_x_const;
  if (view->tool_y_const != -1)
    y = view->tool_y_const;

  *outx = x; *outy = y;
}


ViewToPixelCoords(SimView *view, int x, int y, int *outx, int *outy)
{
  x = view->pan_x - ((view->w_width >>1) - x);
  y = view->pan_y - ((view->w_height >>1) - y);

  if (x < 0) x = 0;
  if (x >= (WORLD_X <<4)) x = (WORLD_X <<4) - 1;
  if (y < 0) y = 0;
  if (y >= (WORLD_Y <<4)) y = (WORLD_Y <<4) - 1;

  if (x < (view->tile_x <<4))
    x = (view->tile_x <<4);
  if (x >= ((view->tile_x + view->tile_width) <<4))
    x = ((view->tile_x + view->tile_width) <<4) - 1;
  if (y < (view->tile_y <<4))
    y = (view->tile_y <<4);
  if (y >= ((view->tile_y + view->tile_height) <<4))
    y = ((view->tile_y + view->tile_height) <<4) - 1;

  if (view->tool_x_const != -1)
    x = (view->tool_x_const <<4) + 8;
  if (view->tool_y_const != -1)
    y = (view->tool_y_const <<4) + 8;

  *outx = x; *outy = y;
}


UpdateFlush()
{
  struct XDisplay *xd;

  if (sim_skips > 0) {
    if (sim_skip > 0) {
      sim_skip--;
      return;
    }
    sim_skip = sim_skips;
  }

  switch (FlushStyle) {

  case 0:
    break;

  case 1:
    for (xd = XDisplays; xd != NULL; xd = xd->next)
      XFlush(xd->dpy);
    break;

  case 2:
    for (xd = XDisplays; xd != NULL; xd = xd->next)
      XSync(xd->dpy, False);
    break;

  case 3:
    if (XDisplays && XDisplays->next) {
      for (xd = XDisplays; xd != NULL; xd = xd->next) {
	XFlush(xd->dpy);
      }
    }
    for (xd = XDisplays; xd != NULL; xd = xd->next) {
      XSync(xd->dpy, False);
    }
    break;

  case 4:
    for (xd = XDisplays; xd != NULL; xd = xd->next) {
#ifndef IS_LINUX
      /* XXX TODO: figure this out for linux and new x libs */
      if ((xd->request != xd->dpy->request) ||
	  (xd->last_request_read != xd->dpy->last_request_read)) {
	XSync(xd->dpy, False);
	xd->request = xd->dpy->request;
	xd->last_request_read = xd->dpy->last_request_read;
      }
#endif
    }
    break;

  }
}


int
CatchXError(Display *dpy, XErrorEvent *err)
{
  GotXError = 1;
#if 0
printf("GOT X ERROR code %d request code %d %d\n",
       err->error_code, err->request_code, err->minor_code);
#endif
  return (0);
}


DoStopMicropolis()
{
  (void)XSetErrorHandler(CatchXError);

  StopToolkit();

  if (sim) {
    while (sim->editor != NULL) {
      DestroyView(sim->editor);
    }

    while (sim->map != NULL) {
      DestroyView(sim->map);
    }

    while (sim->graph != NULL) {
      DestroyGraph(sim->graph);
    }

#ifdef CAM
    while (sim->scam != NULL) {
      DestroyCam(sim->scam);
    }
#endif
  }
}


DoTimeoutListen()
{
  while (Tk_DoOneEvent(TK_DONT_WAIT)) ;
}


Sim *
MakeNewSim()
{
  Sim *sim;

  sim = (Sim *)ckalloc(sizeof(Sim));
  sim->editors = 0; sim->editor = NULL;
  sim->maps = 0; sim->map = NULL;
  sim->graphs = 0; sim->graph = NULL;
  sim->sprites = 0; sim->sprite = NULL;
#ifdef CAM
  sim->scams = 0; sim->scam = NULL;
#endif
  sim->overlay = NULL;

  return (sim);
}


XDisplay *
FindXDisplay(Tk_Window tkwin)
{
  XDisplay *xd;
  int d = 8;
  unsigned long valuemask = 0;
  XGCValues values;
  XColor rgb, *color;
  Display *dpy = Tk_Display(tkwin);
  Screen *screen = Tk_Screen(tkwin);
#ifdef IS_LINUX
  char *display = ":0"; /* XXX TODO: fix this for new x libs */
#else
  char *display = dpy->display_name;
#endif

  for (xd = XDisplays;
       xd && (xd->screen != screen);
       xd = xd->next) ;

  if (xd != NULL) {
    return (xd);
  } else {
    xd = (struct XDisplay *)ckalloc(sizeof (struct XDisplay));

    xd->references = 0;
    xd->dpy = dpy;
    xd->display = (char *)ckalloc(strlen(display) + 1);
    xd->tkDisplay = ((TkWindow *)tkwin)->dispPtr;
    strcpy(xd->display, display);
    xd->screen = screen;
    xd->root = RootWindowOfScreen(xd->screen);

    xd->visual = Tk_DefaultVisual(xd->screen);
    xd->depth = Tk_DefaultDepth(xd->screen);
    xd->colormap = Tk_DefaultColormap(xd->screen);

    xd->color = (xd->depth != 1);

    xd->pixels = (int  *)ckalloc(16 * sizeof(int));
    if (xd->color) { /* Color screen */
      int GotColor = 1;

#define GETCOLOR(i, name) \
      if (!GotColor) { \
	xd->pixels[i] = Rand16() & 255; \
      } else { \
	if ((color = Tk_GetColor(tk_mainInterp, tkwin, \
				 None, name)) == NULL) { \
	  xd->pixels[i] = Rand16() & 255; \
	  GotColor = 0; \
	} else { \
	  switch (xd->depth) { \
	  case 8: \
	    xd->pixels[i] = \
	      color->pixel; \
	    break; \
	  case 15: \
	    xd->pixels[i] = \
	      (((color->red >> (8 + 3)) & 0x1f) << (5 + 5)) | \
	      (((color->green >> (8 + 2)) & 0x1f) << (5)) | \
	      (((color->blue >> (8 + 3)) & 0x1f) << (0)); \
	    break; \
	  case 16: \
	    xd->pixels[i] = \
	      (((color->red >> (8 + 3)) & 0x1f) << (6 + 5)) | \
	      (((color->green >> (8 + 2)) & 0x3f) << (5)) | \
	      (((color->blue >> (8 + 3)) & 0x1f) << (0)); \
	    break; \
	  case 24: \
	    xd->pixels[i] = \
	      ((color->red & 0xff) << 16) | \
	      ((color->green & 0xff) << 8) | \
	      ((color->blue & 0xff) << 0); \
	    break; \
	  case 32: \
	    xd->pixels[i] = \
	      ((color->red & 0xff) << 16) | \
	      ((color->green & 0xff) << 8) | \
	      ((color->blue & 0xff) << 0); \
	    break; \
	  } \
	} \
      }
 
      if ((xd->depth == 8) &&
	  (Tk_DefaultColormap(xd->screen) ==
	   DefaultColormapOfScreen(xd->screen))) {
	xd->pixels[COLOR_WHITE] = WhitePixelOfScreen(xd->screen);
	xd->pixels[COLOR_BLACK] = BlackPixelOfScreen(xd->screen);
      } else {
	GETCOLOR(COLOR_WHITE,		"#ffffff");
	GETCOLOR(COLOR_BLACK,		"#000000");
      }

      GETCOLOR(COLOR_YELLOW,		"#ffff00");
      GETCOLOR(COLOR_ORANGE,		"#ff7f00");
      GETCOLOR(COLOR_RED,		"#ff0000");
      GETCOLOR(COLOR_DARKRED,		"#bf0000");
      GETCOLOR(COLOR_DARKBLUE,		"#0000e6");
      GETCOLOR(COLOR_LIGHTBLUE,		"#6666e6");
      GETCOLOR(COLOR_BROWN,		"#cc4c4c");
      GETCOLOR(COLOR_LIGHTGREEN,	"#00e600");
      GETCOLOR(COLOR_DARKGREEN,		"#007f00");
      GETCOLOR(COLOR_OLIVE,		"#997f4c");
      GETCOLOR(COLOR_LIGHTBROWN,	"#cc7f66");
      GETCOLOR(COLOR_LIGHTGRAY,		"#bfbfbf");
      GETCOLOR(COLOR_MEDIUMGRAY,	"#7f7f7f");
      GETCOLOR(COLOR_DARKGRAY,		"#3f3f3f");

      if (!GotColor) {
	fprintf(stderr,
		"Oh, dear.  There don't seem to be enough free colors on X display \"%s\".\n",
		xd->display);
	fprintf(stderr,
		"Micropolis will try to run anyway, but might look pretty weird!\n");
      }
    } else { /* Black and white screen */
      int white = WhitePixelOfScreen(xd->screen);
      int black = BlackPixelOfScreen(xd->screen);

      xd->pixels[COLOR_WHITE] = white;
      xd->pixels[COLOR_BLACK] = black;

      xd->pixels[COLOR_YELLOW] = white;
      xd->pixels[COLOR_ORANGE] = white;
      xd->pixels[COLOR_RED] = white;
      xd->pixels[COLOR_DARKRED] = black;
      xd->pixels[COLOR_DARKBLUE] = black;
      xd->pixels[COLOR_LIGHTBLUE] = white;
      xd->pixels[COLOR_BROWN] = black;
      xd->pixels[COLOR_LIGHTGREEN] = white;
      xd->pixels[COLOR_DARKGREEN] = black;
      xd->pixels[COLOR_OLIVE] = black;
      xd->pixels[COLOR_LIGHTBROWN] = white;
      xd->pixels[COLOR_LIGHTGRAY] = white;
      xd->pixels[COLOR_MEDIUMGRAY] = white;
      xd->pixels[COLOR_DARKGRAY] = black;
    }

    xd->gc = Tk_DefaultGC(xd->screen);
    XSetForeground(xd->dpy, xd->gc, xd->pixels[COLOR_BLACK]);
    XSetBackground(xd->dpy, xd->gc, xd->pixels[COLOR_WHITE]);
    XSetLineAttributes(xd->dpy, xd->gc,
		       1, LineSolid, CapButt, JoinMiter);
    XSetGraphicsExposures(xd->dpy, xd->gc, False);

#ifndef MSDOS
    { int major, minor, event, error, pixmaps;
      if (WireMode ||
	  (XQueryExtension(xd->dpy, "MIT-SHM", /* Jeez! */
			   &major, &event, &error) != True) ||
	  (XShmQueryVersion(xd->dpy,
			    &major, &minor, &pixmaps) != True)) {
	fprintf(stderr,
		"Darn, X display \"%s\" doesn't support the shared memory extension.\n",
		xd->display);
	xd->shared = 0;
      } else {
	if (!pixmaps) {
	  fprintf(stderr,
		  "Darn, X display \"%s\" claims to support the shared memory extension,\n",
		  xd->display);
	  fprintf(stderr,
		  "but is too lame to support shared memory pixmaps, so Micropolis will run slower.\n");
	  fprintf(stderr,
		  "Please complain to your X server vendor, %s\n",
		  XServerVendor(xd->dpy));
	  xd->shared = -1;
	} else {
	  fprintf(stderr,
		  "Cool, I found the shared memory extension!\n");
	  xd->shared = 1;
	}
      }
    }
#else
    xd->shared = 0;
#endif

    xd->request = -1;
    xd->last_request_read = -1;
    xd->big_tile_pixmap = None;
    xd->objects = NULL;
    xd->overlay_gc = NULL;
    xd->gray25_stipple = None;
    xd->gray50_stipple = None;
    xd->gray75_stipple = None;
    xd->vert_stipple = None;
    xd->horiz_stipple = None;
    xd->diag_stipple = None;

    xd->big_tile_image = xd->small_tile_image = NULL;

    xd->next = XDisplays; XDisplays = xd;
  }

  return (xd);
}


IncRefDisplay(XDisplay *xd)
{
  xd->references++;
}


DecRefDisplay(XDisplay *xd)
{
  if ((--xd->references) == 0) {
    /* I'd blow it away, but tk may still be using the display */
  }
}


SimView *
InitNewView(SimView *view, char *title, int class, int w, int h)
{
  int type, i;
  int d = 8;
  unsigned long valuemask = 0;
  char *t;
  struct XDisplay *xd;
  XGCValues values;
  XColor rgb, *color;

  t = (char *)ckalloc(strlen(title) + 1);
  strcpy(t, title);

  view->next = NULL;
  view->title = t;
  view->type = -1;
  view->class = class;
  view->bigtiles = view->smalltiles = NULL;
  view->pixels = NULL;
  view->line_bytes = 0;
  view->line_bytes8 = 0;
  view->pixel_bytes = 0;
  view->depth = 0;
  view->data = NULL;
  view->data8 = NULL;
  view->visible = 0;
  view->invalid = 0;
  view->skips = view->skip = 0;
  view->update = 0;
  view->map_state = ALMAP;
  view->show_editors = 1;
  view->tool_showing = 0;
  view->tool_mode = 0;
  view->tool_x = view->tool_y = 0;
  view->tool_x_const = view->tool_y_const = -1;
  view->tool_state = dozeState;
  view->tool_state_save = -1;
  view->super_user = 0;
  view->show_me = 1;
  view->dynamic_filter = 0;
  view->auto_scroll_token = 0;
  view->tool_event_time = 0;
  view->tool_last_event_time = 0;
  view->w_x = view->w_y = 0;
  view->w_width = view->w_height = 16;
  view->m_width = view->m_height = 0;
  view->i_width = w; view->i_height = h;
  view->pan_x = view->pan_y = 0;
  view->tile_x = view->tile_y = 0;
  view->tile_width = view->tile_height = 0;
  view->screen_x = view->screen_y = 0;
  view->screen_width = view->screen_height = 0;
  view->last_x = view->last_y = view->last_button = 0;
  view->track_info = NULL;
  view->message_var = NULL;

/* This stuff was initialized in our caller (SimViewCmd) */
/*  view->tkwin = NULL; */
/*  view->interp = NULL; */
/*  view->flags = 0; */

  view->x = NULL;
  view->shminfo = NULL;
  view->tiles = NULL;
  view->other_tiles = NULL;
  view->image = NULL;
  view->other_image = NULL;
  view->other_data = NULL;
  view->pixmap = None;
  view->pixmap2 = None;
  view->overlay_pixmap = None;
  view->overlay_valid = 0;
  view->fontPtr = NULL;
  view->updates = 0;
  view->update_real = view->update_user = view->update_system = 0.0;
  view->update_context = 0;
  view->auto_goto = 0;
  view->auto_going = 0;
  view->auto_x_goal = view->auto_x_goal = 0;
  view->auto_speed = 75;
  view->follow = NULL;
  view->sound = 1;
  view->width = 0; view->height = 0;
  view->show_overlay = 1;
  view->overlay_mode = 0;

  view->x = FindXDisplay(view->tkwin);
  IncRefDisplay(view->x);

  /* view->x->shared is 1 if the shared memory extension is present and
     supports shared memory pixmaps, and -1 if it is present but doesn't. */
  if (view->x->shared != 1) {
    view->type = X_Wire_View;
  } else {
    view->type = X_Mem_View;
  }

  GetPixmaps(view->x);
  view->pixels = view->x->pixels;

  if (w == EDITOR_W) w = 256; /* XXX */
  if (h == EDITOR_H) h = 256; /* XXX */

  view->pan_x = w / 2; view->pan_y = h / 2;
  DoResizeView(view, w, h);

  GetViewTiles(view);

  return (view);
}


DestroyView(SimView *view)
{
  SimView **vp;

  CancelRedrawView(view);

  for (vp = ((view->class == Editor_Class) ?
	     (&sim->editor) : (&sim->map));
       (*vp) != NULL;
       vp = &((*vp)->next)) {
    if ((*vp) == view) {
      (*vp) = view->next;
      if (view->class == Editor_Class)
	sim->editors--;
      else
	sim->maps--;
	  
      break;
    }
  }

  if (view->title != NULL) {
    ckfree (view->title);
    view->title = NULL;
  }

  if (view->pixmap != None) {
    XFreePixmap(view->x->dpy, view->pixmap);
    view->pixmap = None;
  }

  if (view->pixmap2 != None) {
    XFreePixmap(view->x->dpy, view->pixmap2);
    view->pixmap2 = None;
  }

  if (view->overlay_pixmap != None) {
    XFreePixmap(view->x->dpy, view->overlay_pixmap);
    view->overlay_pixmap = None;
  }

  if (view->auto_scroll_token) {
    Tk_DeleteTimerHandler(view->auto_scroll_token);
    view->auto_scroll_token = 0;
  }

#ifndef MSDOS
  if (view->shminfo) {
    XShmDetach(view->x->dpy, view->shminfo);
    shmdt(view->shminfo->shmaddr);
    shmctl(view->shminfo->shmid, IPC_RMID, 0);
    ckfree(view->shminfo);
    view->shminfo = NULL;
    if (view->image) {
      view->image->data = NULL;
      view->data = NULL;
      XDestroyImage(view->image);
      view->image = NULL;
    }
  } else {
#endif
    if (view->image) {
      if (view->image->data) {
	ckfree(view->image->data);
	view->image->data = NULL;
      }
      view->data = NULL;
      XDestroyImage(view->image);
      view->image = NULL;
    }
#ifndef MSDOS
  }
#endif

  if (view->other_image) {
      if (view->other_image->data) {
	ckfree(view->other_image->data);
	view->other_image->data = NULL;
      }
      view->other_data = NULL;
      XDestroyImage(view->other_image);
      view->other_image = NULL;
  }

  if (view->tiles)
    FreeTiles(view);

  DecRefDisplay(view->x);

  ckfree((char *) view);
}


unsigned char *
AllocPixels(int len, unsigned char pixel)
{
  int i;
  unsigned char *data, *cp;

  cp = data = (unsigned char *)ckalloc(len);
  for (i = len; i > 0; i--) {
    *(cp++) = pixel;
  }

  return (data);
}


DoResizeView(SimView *view, int w, int h)
{
  int resize = 0;

  view->w_width = w;
  view->w_height = h;

  if (view->class == Map_Class) { /* Map_Class */
    view->m_width = w;
    view->m_height = h;

    if (view->pixmap2 == None) {

      view->pixmap2 = XCreatePixmap(view->x->dpy, view->x->root,
				    w, h, view->x->depth);
      if (view->pixmap2 == None) {
	fprintf(stderr,
		"Sorry, Micropolis can't create a pixmap on X display \"%s\"!\n",
		view->x->display);
	sim_exit(1); // Just sets tkMustExit and ExitReturn
	return;
      }
    }

  } else { /* Editor_Class */

    if ((w = (w + 31) & (~15)) > view->m_width)
      view->m_width = w, resize++;
    if ((h = (h + 31) & (~15)) > view->m_height)
      view->m_height = h, resize++;

    if (resize || (view->pixmap2 == None)) {
      if (view->pixmap2 != None) {
	XFreePixmap(view->x->dpy, view->pixmap2);
	view->pixmap2 = None;
      }
      view->pixmap2 = XCreatePixmap(view->x->dpy, view->x->root,
				    view->m_width, view->m_height,
				    view->x->depth);
      if (view->pixmap2 == None) {
	fprintf(stderr,
		"Sorry, Micropolis couldn't create a pixmap on X display \"%s\"!\n",
		view->x->display);
	sim_exit(1); // Just sets tkMustExit and ExitReturn
	return;
      }
    }

    if (resize || (view->overlay_pixmap == None)) {
      view->overlay_mode = 0;
      if (view->overlay_pixmap != None) {
	XFreePixmap(view->x->dpy, view->overlay_pixmap);
	view->overlay_pixmap = None;
      }
      view->overlay_pixmap = XCreatePixmap(view->x->dpy, view->x->root,
					   view->m_width, view->m_height,
					   1);
      if (view->overlay_pixmap == None) {
	fprintf(stderr,
		"Sorry, Micropolis couldn't create another pixmap on X display \"%s\".\n",
		view->x->display);
	sim_exit(1); // Just sets tkMustExit and ExitReturn
	return;
      }
      if (view->x->overlay_gc == NULL) {
	unsigned long valuemask = 0;
	XGCValues values;

	view->x->overlay_gc =
	  XCreateGC(view->x->dpy, view->overlay_pixmap, valuemask, &values);
	XSetForeground(view->x->dpy, view->x->overlay_gc, 0);
	XSetBackground(view->x->dpy, view->x->overlay_gc, 1);
	XSetLineAttributes(view->x->dpy, view->x->overlay_gc, 
			   1, LineSolid, CapButt, JoinMiter);
	XSetGraphicsExposures(view->x->dpy, view->x->overlay_gc, False);
      }
    }

  }

#ifndef MSDOS
  if (view->type != X_Mem_View) {
    goto SPRING_FORWARD;
  }

  if (resize || (view->image == NULL)) {
    if (view->shminfo && view->image) {
      if (view->pixmap != None) {
	XFreePixmap(view->x->dpy, view->pixmap);
	view->pixmap = None;
      }
      XShmDetach(view->x->dpy, view->shminfo);
      shmdt(view->shminfo->shmaddr);
      shmctl(view->shminfo->shmid, IPC_RMID, 0);
      view->image->data = NULL;
      if (view->data == view->data8)
	view->data8 = NULL;
      view->data = NULL;
      XDestroyImage(view->image);
      view->image = NULL;
    }

#if 0
    /* XShmPixmapFormat is documented but does not exist !!! */
    if (XShmPixmapFormat(view->x->dpy) != ZPixmap) {
      fprintf(stderr,
	      "Darn, display \"%s\" has the wrong shared memory format.\n",
	      view->x->display);
      goto FALL_BACK;
    }
#endif

    if (!view->shminfo) {
      view->shminfo = (XShmSegmentInfo *)ckalloc(sizeof (XShmSegmentInfo));
    }

    view->image =
      XShmCreateImage(view->x->dpy, view->x->visual, view->x->depth,
		      view->x->color ? ZPixmap : XYBitmap,
		      NULL, view->shminfo,
		      view->m_width, view->m_height);

    view->line_bytes = view->image->bytes_per_line;

    switch (view->x->depth) {

    case 1:
      view->pixel_bytes = 0;
      view->depth = 1;
      break;

    case 8:
      view->pixel_bytes = 1;
      view->depth = 8;
      break;

    case 15:
      view->pixel_bytes = 2;
      view->depth = 15;
      break;

    case 16:
      view->pixel_bytes = 2;
      view->depth = 16;
      break;

    case 24:
      /* XXX: TODO: 24 and 32 bit support */
      view->pixel_bytes = 4;
      //view->pixel_bytes = 3;
      view->depth = 24;
      break;

    case 32:
      /* XXX: TODO: 24 and 32 bit support */
      view->pixel_bytes = 4;
      view->depth = 32;
      break;

    default:
      view->pixel_bytes = 0;
      view->depth = 0;
      break;

    } // switch

    view->shminfo->shmid = shmget(IPC_PRIVATE,
				  (view->line_bytes *
				   view->m_height),
				  (IPC_CREAT | 0777));
    if (view->shminfo->shmid < 0) {
      perror("shmget");
      fprintf(stderr,
	      "Darn, Micropolis can't share memory with X display \"%s\".\n",
	      view->x->display);
      goto FALL_BACK;
    }

    view->data = (unsigned char *)shmat(view->shminfo->shmid, 0, 0);
    if ((int)view->data == -1) {
      perror("shmat");
      fprintf(stderr,
	      "Darn, Micropolis can't find any memory to share with display \"%s\".\n",
	      view->x->display);
      goto FALL_BACK;
    }

    view->image->data = (char *)view->data;
    view->shminfo->shmaddr = (char *)view->data;
    view->shminfo->readOnly = False;

    { int (*old)();
      int result;
      int attached = 0;
      GotXError = 0;
      old = XSetErrorHandler(CatchXError);

      result = 
	XShmAttach(view->x->dpy, view->shminfo);
      if (result  == 0) {
	fprintf(stderr,
		"Darn, the X display \"%s\" can't access Micropolis's shared memory.\n",
		view->x->display);
	GotXError = 1;
      }

      XSync(view->x->dpy, False);

      if (!GotXError) {
	attached = 1;
	view->pixmap = XShmCreatePixmap(view->x->dpy, view->x->root,
					view->data, view->shminfo,
					view->m_width, view->m_height,
					view->x->depth);
	XSync(view->x->dpy, False);

	if (GotXError ||
	    (view->pixmap == None)) {
	  fprintf(stderr,
		  "Darn, Micropolis couldn't get a shared memory pixmap on X display \"%s\".\n",
		  view->x->display);
	  GotXError = 1;
	}
      }

      XSetErrorHandler(old);

      if (GotXError) {
	view->pixmap = None;
	if (attached) {
          XShmDetach(view->x->dpy, view->shminfo);
	} // if
        result = shmdt(view->shminfo->shmaddr);
        result = shmctl(view->shminfo->shmid, IPC_RMID, 0);
        ckfree(view->shminfo);
        view->shminfo = NULL;
        if (view->image) {
          view->image->data = NULL;
          view->data = NULL;
          XDestroyImage(view->image);
          view->image = NULL;
        }
	goto FALL_BACK;
      }

      if (view->x->color) {
	XSetForeground(view->x->dpy, view->x->gc,
		       view->pixels[COLOR_LIGHTBROWN]);
      } else {
	XSetForeground(view->x->dpy, view->x->gc,
		       view->pixels[COLOR_WHITE]);
      }

      XFillRectangle(view->x->dpy, view->pixmap, view->x->gc,
		     0, 0, view->m_width, view->m_height);
    }
  }

  goto FINISH;

 FALL_BACK:

  fprintf(stderr,
	  "Falling back to the X network protocol on display \"%s\"...\n",
	  view->x->display);
#endif

  view->x->shared = 0;
  view->type = X_Wire_View;
  if (view->pixmap != None) {
    XFreePixmap(view->x->dpy, view->pixmap);
    view->pixmap = None;
  }
#ifndef MSDOS
  if (view->shminfo) {
    if (view->shminfo->shmid >= 0) {
      if (view->shminfo->shmaddr) {
	shmdt(view->shminfo->shmaddr);
      }
      shmctl(view->shminfo->shmid, IPC_RMID, 0);
    }
    ckfree((char *)view->shminfo);
    view->shminfo = NULL;
  }
#endif
  if (view->image) {
    view->image->data = NULL;
    XDestroyImage(view->image);
    view->image = NULL;
  }
  view->data = NULL;
  view->line_bytes = 0;
  view->pixel_bytes = 0;
  view->depth = 0;

 SPRING_FORWARD:

  if (resize || (view->pixmap == None)) {
    if (view->pixmap != None) {
      XFreePixmap(view->x->dpy, view->pixmap);
      view->pixmap = None;
    }
    view->pixmap = XCreatePixmap(view->x->dpy, view->x->root,
				 view->m_width, view->m_height,
				 view->x->depth);
    if (view->pixmap == None) {
      fprintf(stderr,
	      "Sorry, Micropolis can't create pixmap on X display \"%s\".\n",
	      view->x->display);
      sim_exit(1); // Just sets tkMustExit and ExitReturn
      return;
    }
    if (view->x->color) {
      XSetForeground(view->x->dpy, view->x->gc,
		     view->pixels[COLOR_LIGHTBROWN]);
    } else {
      XSetForeground(view->x->dpy, view->x->gc,
		     view->pixels[COLOR_WHITE]);
    }
    XFillRectangle(view->x->dpy, view->pixmap, view->x->gc,
		     0, 0, view->m_width, view->m_height);
  }

 FINISH:

  if (view->class == Editor_Class) {

    AllocTiles(view);
    DoAdjustPan(view);

  } else if (view->class == Map_Class) {

    if (view->type == X_Mem_View) { /* Memory Map */

      if (view->x->color) {

	/* Color, Shared Memory */

	view->data8 = view->data;
	view->line_bytes8 = view->line_bytes; /* XXX: ??? */

	switch (view->x->depth) {

	case 8:
	  view->pixel_bytes = 1;
	  view->depth = 8;
	  break;

	case 15:
	  view->pixel_bytes = 2;
	  view->depth = 15;
	  break;

	case 16:
	  view->pixel_bytes = 2;
	  view->depth = 16;
	  break;

	case 24:
	  /* XXX: TODO: 24 and 32 bit support */
	  view->pixel_bytes = 4;
	  //view->pixel_bytes = 3;
	  view->depth = 24;
	  break;

	case 32:
	  /* XXX: TODO: 24 and 32 bit support */
	  view->pixel_bytes = 4;
	  view->depth = 32;
	  break;

	default:
	  view->pixel_bytes = 0;
	  view->depth = 0;
	  break;

	} // switch

      } else {

	/* Black and White, Shared Memory */

	if (view->other_image != NULL) {
	  XDestroyImage(view->other_image);
	}

	view->line_bytes8 = view->m_width; /* XXX: fix depth */
	view->pixel_bytes = 0;
	view->depth = 1;

	view->other_data = view->data8 =
	  AllocPixels(view->m_height * view->line_bytes8, /* XXX: fix depth */
		      view->pixels[COLOR_WHITE]);
	view->other_image =
	  XCreateImage(view->x->dpy, view->x->visual, 8, /* XXX: fix depth */
		       ZPixmap, 0, (char *)view->other_data,
		       view->m_width, view->m_height,
		       8, view->line_bytes8); /* XXX: fix depth */
      }

    } else { /* Wire Map */
      int bitmap_pad;
      int bitmap_depth;

      if (view->image != NULL) {
	XDestroyImage(view->image);
	view->image = NULL;
      }

      if (view->other_image != NULL) {
	XDestroyImage(view->other_image);
	view->other_image = NULL;
      }

      if (view->x->color) {

	/* Color, Wire */

	switch (view->x->depth) {

	case 8:
	  view->pixel_bytes = 1;
	  view->depth = 8;
	  bitmap_pad = 8;
	  bitmap_depth = 8;
	  view->line_bytes8 =
	    ((view->m_width * view->pixel_bytes) + 3) & (~3);
	  break;

	case 15:
	  view->pixel_bytes = 2;
	  view->depth = 15;
	  bitmap_pad = 16;
	  bitmap_depth = 16;
	  view->line_bytes8 =
	    ((view->m_width * view->pixel_bytes) + 3) & (~3);
	  break;

	case 16:
	  view->pixel_bytes = 2;
	  view->depth = 16;
	  bitmap_pad = 16;
	  bitmap_depth = 16;
	  view->line_bytes8 =
	    ((view->m_width * view->pixel_bytes) + 3) & (~3);
	  break;

	case 24:
	  view->pixel_bytes = 4;
	  //view->pixel_bytes = 3;
	  view->depth = 24;
	  bitmap_depth = 32;
	  bitmap_pad = 32;
	  view->line_bytes8 =
	    ((view->m_width * 4) + 3) & (~3);
	  break;

	case 32:
	  view->pixel_bytes = 4;
	  view->depth = 32;
	  bitmap_pad = 32;
	  bitmap_depth = 32;
	  view->line_bytes8 =
	    ((view->m_width * 4) + 3) & (~3);
	  break;

	default:
	  assert(0); /* Unknown depth */
	  break;

	} // switch

	view->line_bytes = 
	  view->line_bytes8;

      } else {

	/* Black and White, Wire */

	view->pixel_bytes = 0;
	view->depth = 1;
        view->line_bytes8 =
	  (view->m_width + 3) & (~3); /* XXX: handle depth */
	view->line_bytes =
	  (view->m_width + 7) >>3;
	bitmap_pad = 8;
	bitmap_depth = 8;

      }

      view->data =
	AllocPixels(view->m_height * view->line_bytes, 0);
      view->image =
	XCreateImage(view->x->dpy, view->x->visual,
		     bitmap_depth,
		     view->x->color ? ZPixmap : XYBitmap,
		     0, (char *)view->data,
		     view->m_width, view->m_height,
		     bitmap_pad,
		     view->line_bytes);

      view->other_data =
	AllocPixels(view->m_height * view->line_bytes8, 0);
      view->other_image =
	XCreateImage(view->x->dpy, view->x->visual,
		     bitmap_depth,
		     ZPixmap,
		     0, (char *)view->other_data,
		     view->m_width, view->m_height,
		     bitmap_pad,
		     view->line_bytes);

      if (view->x->color) {
	view->data8 = view->data;
      } else {
	view->data8 = view->other_data;
      }
    }
  }
}


DoPanBy(struct SimView *view, int dx, int dy)
{
  DoPanTo(view, view->pan_x + dx, view->pan_y + dy);
}


DoPanTo(struct SimView *view, int x, int y)
{
  if (view->class != Editor_Class) {
    return;
  }

  if (x < 0) x = 0;
  if (y < 0) y = 0;
  if (x > view->i_width) x = view->i_width - 1;
  if (y > view->i_height) y = view->i_height - 1;
  if ((view->pan_x != x) ||
      (view->pan_y != y)) {
    view->pan_x = x;
    view->pan_y = y;
    DoAdjustPan(view);
  }
}

/* #define DEBUG_PAN */

DoAdjustPan(struct SimView *view)
{
  int ww2 = view->w_width >>1, wh2 = view->w_height >>1;
  int px = view->pan_x, py = view->pan_y;
  int last_tile_x = view->tile_x, last_tile_y = view->tile_y;
  int last_tile_width = view->tile_width, last_tile_height = view->tile_height;
  int total_width = view->m_width >>4, total_height = view->m_height >>4;
//fprintf(stderr, "DoAdjustPan\n");

#ifdef DEBUG_PAN
  printf("AdjustPan window %d %d  ww2 %d wh2 %d  pan %d %d\n",
	 view->w_width, view->w_height, ww2, wh2, px, py);
  printf("  last tile %d %d %d %d\n",
	 last_tile_x, last_tile_y, last_tile_width, last_tile_height);
#endif

  if ((view->tile_x = ((px - ww2) >>4)) < 0)
    view->tile_x = 0;
  if ((view->tile_y = ((py - wh2) >>4)) < 0)
    view->tile_y = 0;

#ifdef DEBUG_PAN
  printf("  now tile %d %d\n", view->tile_x, view->tile_y);
#endif

  view->tile_width = ((15 + px + ww2) >>4);
  view->tile_height = ((15 + py + wh2) >>4);

#ifdef DEBUG_PAN
  printf("    outer tile %d %d\n", view->tile_width, view->tile_height);
#endif

  if (view->tile_width > (view->i_width >>4))
    view->tile_width = (view->i_width >>4);
  view->tile_width -= view->tile_x;
  if (view->tile_height > (view->i_height >>4))
    view->tile_height = (view->i_height >>4);
  view->tile_height -= view->tile_y;

#ifdef DEBUG_PAN
  printf("    tile size %d %d\n", view->tile_width, view->tile_height);
#endif

  if (view->tile_width > (view->m_width >>4))
    view->tile_width = (view->m_width >>4);
  if (view->tile_height > (view->m_height >>4))
    view->tile_height = (view->m_height >>4);

#ifdef DEBUG_PAN
  printf("    clipped size %d %d\n", view->tile_width, view->tile_height);
  printf("    maximum size %d %d\n", view->m_width >>4, view->m_height >>4);
#endif

  view->screen_x = (ww2 - px) + (view->tile_x <<4);
  view->screen_y = (wh2 - py) + (view->tile_y <<4);
  view->screen_width = (view->tile_width <<4);
  view->screen_height = (view->tile_height <<4);

#ifdef DEBUG_PAN
  printf("    screen %d %d %d %d\n",
	 view->screen_x, view->screen_y,
	 view->screen_width, view->screen_height);
#endif

  view->overlay_mode = 0;
//  view->skip = 0;
  view->invalid = 1;
  if (SimSpeed == 0) {
    EventuallyRedrawView(view);
  }
/*  InvalidateEditors(); */
  if (view->show_me) {
    RedrawMaps();
  }
/*  FixMicropolisTimer(); */

  { int dx = last_tile_x - view->tile_x,
        dy = last_tile_y - view->tile_y;
    short **want = view->other_tiles,
    	  **have = view->tiles;

#ifdef DEBUG_PAN
    printf("scrolling %d %d\n", dx, dy);
#endif

    if ((dx != 0) || (dy != 0)) {
      int row, col, x, y,
          width = view->tile_width,
          height = view->tile_height;

      for (col = 0; col < width; col++)
	memcpy(want[col], have[col], (height * sizeof(short)));

      for (col = 0; col < total_width; col++) {
	x = col - dx;
	for (row = 0; row < total_height; row++) {
	  y = row - dy;
	  if ((x >= 0) && (x < width) &&
	      (y >= 0) && (y < height)) {
	    have[col][row] = want[x][y];
	  } else {
	    have[col][row] = -1;
	  }
	}
      }

      XCopyArea(view->x->dpy, view->pixmap, view->pixmap, view->x->gc,
		0, 0, view->tile_width <<4, view->tile_height <<4,
		dx <<4, dy <<4);

      if (view->type == X_Mem_View) {
	XSync(view->x->dpy, False);
      }
    }
  }
}


AllocTiles(SimView *view)
{
  int row, col;
  short **have, **want;
  int w = view->m_width / 16, h = view->m_height / 16;
  int n = (w + 1) * sizeof (short *);

  if (view->tiles)
    FreeTiles(view);

  have = view->tiles =
    (short **)ckalloc(n);

  want = view->other_tiles =
    (short **)ckalloc(n);

  have[w] = want[w] = NULL;

  n = h * sizeof(short);
  for (col = 0; col < w; col++) {

    have[col] = (short *)ckalloc(n);
    want[col] = (short *)ckalloc(n);
    for (row = 0; row < h; row++) {
      have[col][row] = -1;
      want[col][row] = -1;
    }
  }
}


FreeTiles(SimView *view)
{
  int col;

  for (col = 0; view->tiles[col] != NULL; col++) {
    ckfree ((char *)view->tiles[col]);
    ckfree ((char *)view->other_tiles[col]);
  }
  ckfree ((char *)view->tiles);
  view->tiles = NULL;
  ckfree ((char *)view->other_tiles);
  view->other_tiles = NULL;
}


#define POINT_BATCH 32

Ink *OldInk = NULL;


/* XXX: todo: ink locking so someone doesn't erase ink that's being drawn */

Ink *
NewInk()
{
  Ink *ink;

  if (OldInk) {
    ink = OldInk;
    OldInk = OldInk->next;
  } else {
    ink = (Ink *)ckalloc(sizeof(Ink));
    ink->maxlength = POINT_BATCH;
    ink->points = (XPoint *)ckalloc(POINT_BATCH * sizeof(XPoint));
  }
  ink->length = 0;
  ink->color = COLOR_WHITE;
  ink->next = NULL;
  ink->left = ink->right = ink->top = ink->bottom =
    ink->last_x = ink->last_y = -1;
  return (ink);
}


FreeInk(Ink *ink)
{
  ink->next = OldInk;
  OldInk = ink;
}


StartInk(Ink *ink, int x, int y)
{
  ink->length = 1;
  ink->left = ink->right = ink->last_x = ink->points[0].x = x;
  ink->top = ink->bottom = ink->last_y = ink->points[0].y = y;
}


AddInk(Ink *ink, int x, int y)
{
  int dx = x - ink->last_x;
  int dy = y - ink->last_y;

  if ((dx != 0) || (dy != 0)) {
/*
    if (ink->length > 1) {
      if ((dx == 0) &&
	  (ink->points[ink->length - 1].x == 0) &&
	  ((ink->points[ink->length - 1].y < 0) ?
	   (dy < 0) : (dy > 0))) {
	ink->points[ink->length - 1].y += dy;
	goto ADJUST;
      } else if ((dy == 0) &&
		 (ink->points[ink->length - 1].y == 0) &&
		 ((ink->points[ink->length - 1].x < 0) ?
		  (dx < 0) : (dx > 0))) {
	ink->points[ink->length - 1].x += dx;
	goto ADJUST;
      }
    }
*/

    if (ink->length >= ink->maxlength) {
      ink->maxlength += POINT_BATCH;
      ink->points = (XPoint *)realloc((void *)ink->points,
				      ink->maxlength * sizeof(XPoint));
    }
    ink->points[ink->length].x = dx;
    ink->points[ink->length].y = dy;
    ink->length++;

  ADJUST:
    if (x < ink->left)
      ink->left = x;
    if (x > ink->right)
      ink->right = x;
    if (y < ink->top)
      ink->top = y;
    if (y > ink->bottom)
      ink->bottom = y;

    { int left, right, top, bottom;
      SimView *view;

      if (ink->last_x < x) { left = ink->last_x; right = x; }
      else { left = x; right = ink->last_x; }
      if (ink->last_y < y) { top = ink->last_y; bottom = y; }
      else { top = y; bottom = ink->last_y; }

      left -= 5; right += 5; top -= 5; bottom += 5;

      for (view = sim->editor; view != NULL; view = view->next) {
	int vleft, vtop;

	if ((right >= (vleft = view->pan_x - (view->w_width / 2))) &&
	    (left <= vleft + view->w_width) &&
	    (bottom >= (vtop = view->pan_y - (view->w_height / 2))) &&
	    (top <= vtop + view->w_height)) {
	  /* XXX: do studly incremental update instead */
	  view->overlay_mode = 0;
	  EventuallyRedrawView(view);
	}
      }
    }
    ink->last_x = x; ink->last_y = y;
  }
}


EraseOverlay()
{
  Ink *ink;

  while (sim->overlay) {
    ink = sim->overlay;
    sim->overlay = ink->next;
    FreeInk(ink);
  }
}
