/* view.h
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

#define X_Mem_View 1
#define X_Wire_View 2

#define Editor_Class 0
#define Map_Class 1

#define Button_Press 0
#define Button_Move 1
#define Button_Release 2

#define VIEW_REDRAW_PENDING 1


typedef struct Ink {
  struct Ink *next;
  int x, y;
  int color;
  int length;
  int maxlength;
  XPoint *points;
  int left, top, right, bottom;
  int last_x, last_y;
} Ink;


typedef struct XDisplay {
  struct XDisplay *next;
  int references;
  char *display;
  TkDisplay *tkDisplay;
  Display *dpy;
  Screen *screen;
  Window root;
  Visual *visual;
  int depth;
  int color;
  Colormap colormap;
  int *pixels;
  GC gc;
  int shared;
  unsigned long last_request_read;
  unsigned long request;
  XImage *big_tile_image;
  XImage *small_tile_image;
  Pixmap big_tile_pixmap;
  Pixmap **objects;
  GC overlay_gc;
  Pixmap gray25_stipple;
  Pixmap gray50_stipple;
  Pixmap gray75_stipple;
  Pixmap vert_stipple;
  Pixmap horiz_stipple;
  Pixmap diag_stipple;
} XDisplay;


typedef struct SimView {
  struct SimView *next;
  char *title;
  int type;
  int class;

/* graphics stuff */
  int *pixels;
  int line_bytes;
  int pixel_bytes;
  int depth;
  unsigned char *data;
  int line_bytes8;
  unsigned char *data8;
  int visible;
  int invalid;
  int skips;
  int skip;
  int update;

/* map stuff */
  unsigned char *smalltiles;
  short map_state;
  int show_editors;

/* editor stuff */
  unsigned char *bigtiles;
  short power_type;
  short tool_showing;
  short tool_mode;
  short tool_x, tool_y;
  short tool_x_const, tool_y_const;
  short tool_state;
  short tool_state_save;
  short super_user;
  short show_me;
  short dynamic_filter;
  Tk_TimerToken auto_scroll_token;
  Time tool_event_time;
  Time tool_last_event_time;

/* scrolling */
  int w_x, w_y;					/* view window position */
  int w_width, w_height;			/* view window size */
  int m_width, m_height;			/* memory buffer size */
  int i_width, i_height;			/* ideal whole size */
  int pan_x, pan_y;				/* centered in window */
  int tile_x, tile_y, tile_width, tile_height;	/* visible tiles */
  int screen_x, screen_y, screen_width, screen_height; /* visible pixels */

/* tracking */
  int orig_pan_x, orig_pan_y;
  int last_x, last_y;
  int last_button;
  char *track_info;
  char *message_var;

/* window system */
  Tk_Window tkwin;
  Tcl_Interp *interp;
  int flags;

  XDisplay *x;
  XShmSegmentInfo *shminfo;
  short **tiles;
  short **other_tiles;
  XImage *image;
  XImage *other_image;
  unsigned char *other_data;
  Pixmap pixmap;
  Pixmap pixmap2;
  Pixmap overlay_pixmap;
  Pixmap overlay_valid;
  XFontStruct *fontPtr;

/* timing */
  int updates;
  double update_real;
  double update_user;
  double update_system;
  int update_context;

/* auto goto */
  int auto_goto;
  int auto_going;
  int auto_x_goal, auto_y_goal;
  int auto_speed;
  struct SimSprite *follow;

/* sound */
  int sound;

/* configuration */
  int width, height;

/* overlay */
  int show_overlay;
  int overlay_mode;
  struct timeval overlay_time;
} SimView;


typedef struct SimGraph {
  struct SimGraph *next;
  int range;
  int mask;
  Tk_Window tkwin;
  Tcl_Interp *interp;
  int flags;
  XDisplay *x;
  int visible;
  int w_x, w_y;
  int w_width, w_height;
  Pixmap pixmap;
  int *pixels;
  XFontStruct *fontPtr;
  Tk_3DBorder border;
  int borderWidth;
  int relief;
  Tk_TimerToken draw_graph_token;
} SimGraph;


typedef struct SimDate {
  struct SimDate *next;
  int reset;
  int month;
  int year;
  int lastmonth;
  int lastyear;
  Tk_Window tkwin;
  Tcl_Interp *interp;
  int flags;
  XDisplay *x;
  int visible;
  int w_x, w_y;
  int w_width, w_height;
  Pixmap pixmap;
  int *pixels;
  XFontStruct *fontPtr;
  Tk_3DBorder border;
  int borderWidth;
  int padX;
  int padY;
  int width;
  int monthTab;
  int monthTabX;
  int yearTab;
  int yearTabX;
  Tk_TimerToken draw_date_token;
} SimDate;


typedef struct SimSprite {
  struct SimSprite *next;
  char *name;
  int type;
  int frame;
  int x, y;
  int width, height;
  int x_offset, y_offset;
  int x_hot, y_hot;
  int orig_x, orig_y;
  int dest_x, dest_y;
  int count, sound_count;
  int dir, new_dir;
  int step, flag, control;
  int turn;
  int accel;
  int speed;
} SimSprite;


#ifdef CAM
#include "cam.h"
#endif


typedef struct Person {
  int id;
  char *name;
} Person;


typedef struct Sim {
  int editors;
  SimView *editor;
  int maps;
  SimView *map;
  int graphs;
  SimGraph *graph;
  int dates;
  SimDate *date;
  int sprites;
  SimSprite *sprite;
#ifdef CAM
  int scams;
  SimCam *scam;
#endif
  Ink *overlay;
} Sim;


typedef struct Cmd {
  char *name;
  int (*cmd)();
} Cmd;

