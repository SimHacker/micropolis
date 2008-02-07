/* w_date.c
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


short NewDate = 0;
Tcl_HashTable DateCmds;
int DateUpdateTime = 200;


#define DEF_DATE_FONT	"-Adobe-Helvetica-Bold-R-Normal-*-140-*"
#define DEF_DATE_BG_COLOR	"#b0b0b0"
#define DEF_DATE_BG_MONO	"#ffffff"
#define DEF_DATE_BORDER_WIDTH	"2"
#define DEF_DATE_PADY		"1"
#define DEF_DATE_PADX		"1"
#define DEF_DATE_WIDTH		"0"
#define DEF_DATE_MONTHTAB	"7"
#define DEF_DATE_YEARTAB	"13"

Tk_ConfigSpec DateConfigSpecs[] = {
    {TK_CONFIG_FONT, "-font", (char *) NULL, (char *) NULL,
       DEF_DATE_FONT, Tk_Offset(SimDate, fontPtr), 0},
    {TK_CONFIG_BORDER, "-background", "background", "Background",
       DEF_DATE_BG_COLOR, Tk_Offset(SimDate, border),
       TK_CONFIG_COLOR_ONLY},
    {TK_CONFIG_BORDER, "-background", "background", "Background",
       DEF_DATE_BG_MONO, Tk_Offset(SimDate, border),
       TK_CONFIG_MONO_ONLY},
    {TK_CONFIG_PIXELS, "-borderwidth", "borderWidth", "BorderWidth",
       DEF_DATE_BORDER_WIDTH, Tk_Offset(SimDate, borderWidth), 0},
    {TK_CONFIG_PIXELS, "-padx", "padX", "Pad",
	DEF_DATE_PADX, Tk_Offset(SimDate, padX), 0},
    {TK_CONFIG_PIXELS, "-pady", "padY", "Pad",
	DEF_DATE_PADY, Tk_Offset(SimDate, padY), 0},
    {TK_CONFIG_INT, "-width", "width", "Width",
	DEF_DATE_WIDTH, Tk_Offset(SimDate, width), 0},
    {TK_CONFIG_INT, "-monthtab", "monthtab", "MonthTab",
	DEF_DATE_MONTHTAB, Tk_Offset(SimDate, monthTab), 0},
    {TK_CONFIG_INT, "-yeartab", "yeartab", "YearTab",
	DEF_DATE_YEARTAB, Tk_Offset(SimDate, yearTab), 0},
    {TK_CONFIG_END, (char *) NULL, (char *) NULL, (char *) NULL,
       (char *) NULL, 0, 0}
  };


XDisplay *FindXDisplay();


static void
DisplaySimDate(ClientData clientData)
{
  SimDate *date = (SimDate *) clientData;
  Tk_Window tkwin = date->tkwin;
  Pixmap pm = None;
  Drawable d;

  date->flags &= ~VIEW_REDRAW_PENDING;
//fprintf(stderr, "DisplaySimDate cleared VIEW_REDRAW_PENDING\n");

  assert(date->draw_date_token != 0);
  if (date->draw_date_token != 0) {
//    Tk_DeleteTimerHandler(date->draw_date_token);
    date->draw_date_token = 0;
  }

  if (date->visible && (tkwin != NULL) && Tk_IsMapped(tkwin)) {
    DoUpdateDate(date);
  }
}


void
DestroySimDate(ClientData clientData)
{
  SimDate *date = (SimDate *) clientData;

  DestroyDate(date);
}


EventuallyRedrawDate(SimDate *date)
{
  if (!(date->flags & VIEW_REDRAW_PENDING)) {
    assert(date->draw_date_token == 0);
    if (date->draw_date_token == 0) {
      date->draw_date_token =
	Tk_CreateTimerHandler(
	  DateUpdateTime,
	  DisplaySimDate,
	  (ClientData) date);
      date->flags |= VIEW_REDRAW_PENDING;
//fprintf(stderr, "EventuallyRedrawDate set VIEW_REDRAW_PENDING\n");
    }
  }
}


void
SimDateEventProc(ClientData clientData, XEvent *eventPtr)
{
  SimDate *date = (SimDate *) clientData;

  if ((eventPtr->type == Expose) && (eventPtr->xexpose.count == 0)) {
    date->visible = 1;
    EventuallyRedrawDate(date);
  } else if (eventPtr->type == MapNotify) {
    date->visible = 1;
  } else if (eventPtr->type == UnmapNotify) {
    date->visible = 0;
  } else if (eventPtr->type == VisibilityNotify) {
    if (eventPtr->xvisibility.state == VisibilityFullyObscured)
      date->visible = 0;
    else
      date->visible = 1;
  } else if (eventPtr->type == ConfigureNotify) {
    DoResizeDate(date,
		  eventPtr->xconfigure.width,
		  eventPtr->xconfigure.height);
    EventuallyRedrawDate(date);
  } else if (eventPtr->type == DestroyNotify) {
    Tcl_DeleteCommand(date->interp, Tk_PathName(date->tkwin));
    date->tkwin = NULL;
    if (date->flags & VIEW_REDRAW_PENDING) {
      assert(date->draw_date_token != 0);
      if (date->draw_date_token != 0) {
	Tk_DeleteTimerHandler(date->draw_date_token);
	date->draw_date_token = 0;
      }
      date->flags &= ~VIEW_REDRAW_PENDING;
//fprintf(stderr, "SimDateEventProc cleared VIEW_REDRAW_PENDING\n");
    }
   Tk_EventuallyFree((ClientData) date, DestroySimDate);
  }
}


static void
ComputeDateGeometry(SimDate *date)
{
    XCharStruct bbox;
    int dummy;
    unsigned int width, height;
    int charWidth;

    XTextExtents(date->fontPtr, "0", 1,
		 &dummy, &dummy, &dummy, &bbox);
    charWidth = (bbox.lbearing + bbox.rbearing);

    if (date->width == 0) {
      char *maxString = "Date:  MMM    1000000";
      int maxStringLength = strlen(maxString);

      XTextExtents(date->fontPtr, maxString, maxStringLength,
		   &dummy, &dummy, &dummy, &bbox);
      width = bbox.lbearing + bbox.rbearing;
    } else {
      width = date->width * charWidth;
    }

    height = date->fontPtr->ascent + date->fontPtr->descent;

    width += 2 * date->padX;
    height += 2 * date->padY;

    Tk_GeometryRequest(
	date->tkwin,
	(int) (width + (2 * date->borderWidth) + 2),
	(int) (height + (2 * date->borderWidth) + 2));
    Tk_SetInternalBorder(
	date->tkwin,
	date->borderWidth);

    date->yearTabX = date->yearTab * charWidth;
    date->monthTabX = date->monthTab * charWidth;
}


int DateCmdconfigure(DATE_ARGS)
{
  int result = TCL_OK;

  if (argc == 2) {
    result = Tk_ConfigureInfo(interp, date->tkwin, DateConfigSpecs,
			      (char *) date, (char *) NULL, 0);
  } else if (argc == 3) {
    result = Tk_ConfigureInfo(interp, date->tkwin, DateConfigSpecs,
			      (char *) date, argv[2], 0);
  } else {
    result = ConfigureSimDate(interp, date, argc-2, argv+2,
			    TK_CONFIG_ARGV_ONLY);
  }
  return TCL_OK;
}


int DateCmdposition(DATE_ARGS)
{
  int result = TCL_OK;

    if ((argc != 2) && (argc != 4)) {
      return TCL_ERROR;
    }
    if (argc == 4) {
      if ((Tcl_GetInt(interp, argv[2], &date->w_x) != TCL_OK)
	  || (Tcl_GetInt(interp, argv[3], &date->w_y) != TCL_OK)) {
	return TCL_ERROR;
      }
    }
    sprintf(interp->result, "%d %d", date->w_x, date->w_y);
    return TCL_OK;
}


int DateCmdsize(DATE_ARGS)
{
  if ((argc != 2) && (argc != 4)) {
    return TCL_ERROR;
  }
  if (argc == 4) {
    int w, h;
    
    if (Tcl_GetInt(interp, argv[2], &w) != TCL_OK) {
      return TCL_ERROR;
    }
    if (Tcl_GetInt(interp, argv[3], &h) != TCL_OK) {
      return TCL_ERROR;
    }
    date->w_width = w;
    date->w_height = h;
  }
  sprintf(interp->result, "%d %d", date->w_width, date->w_height);
  return TCL_OK;
}


int DateCmdVisible(DATE_ARGS)
{
  int visible;

  if ((argc != 2) && (argc != 3)) {
    Tcl_AppendResult(interp, "wrong # args", (char *) NULL);
    return TCL_ERROR;
  }

  if (argc == 3) {
    if ((Tcl_GetInt(interp, argv[2], &visible) != TCL_OK) ||
	(visible < 0) || (visible > 1)) {
      Tcl_AppendResult(interp, " bogus args", (char *) NULL);
      return TCL_ERROR;
    }

    date->visible = visible;
  }

  sprintf(interp->result, "%d", date->visible);

  return TCL_OK;
}


int DateCmdReset(DATE_ARGS)
{
  int range;

  if (argc != 2) {
    Tcl_AppendResult(interp, "wrong # args", (char *) NULL);
    return TCL_ERROR;
  }

  date->reset = 1;

//  ComputeDateGeometry(date); // ???

  EventuallyRedrawDate(date);

  return TCL_OK;
}


int DateCmdSet(DATE_ARGS)
{
  int range;

  if (argc != 4) {
    Tcl_AppendResult(interp, "wrong # args", (char *) NULL);
    return TCL_ERROR;
  }

  if ((Tcl_GetInt(interp, argv[2], &date->month) != TCL_OK) ||
      (date->month < 0) ||
      (date->month >= 12)) {
    Tcl_AppendResult(interp, " bogus args", (char *) NULL);
    return TCL_ERROR;
  }
  
  if ((Tcl_GetInt(interp, argv[3], &date->year) != TCL_OK) ||
      (date->year < 0)) {
    Tcl_AppendResult(interp, " bogus args", (char *) NULL);
    return TCL_ERROR;
  }

//  ComputeDateGeometry(date); // ???

  EventuallyRedrawDate(date);

  return TCL_OK;
}


int
DoDateCmd(CLIENT_ARGS)
{
  SimDate *date = (SimDate *) clientData;
  Tcl_HashEntry *ent;
  int result = TCL_OK;
  int (*cmd)();

  if (argc < 2) {
    return TCL_ERROR;
  }

  if (ent = Tcl_FindHashEntry(&DateCmds, argv[1])) {
    cmd = (int (*)())ent->clientData;
    Tk_Preserve((ClientData) date);
    result = cmd(date, interp, argc, argv);
    Tk_Release((ClientData) date);
  } else {
    Tcl_AppendResult(interp, "unknown command name: \"",
		     argv[0], " ", argv[1], "\".", (char *) NULL);
    result = TCL_ERROR;
  }
  return result;
}


int
DateViewCmd(CLIENT_ARGS)
{
  SimDate *date;
  Tk_Window tkwin = (Tk_Window) clientData;

  if (argc < 2) {
    Tcl_AppendResult(interp, "wrong # args:  should be \"",
		     argv[0], " pathName ?options?\"", (char *) NULL);
    return TCL_ERROR;
  }

  tkwin = Tk_CreateWindowFromPath(interp, tkwin,
				  argv[1], (char *) NULL);
  if (tkwin == NULL) {
    return TCL_ERROR;
  }

  date = (SimDate *)ckalloc(sizeof (SimDate));

  date->tkwin = tkwin;
  date->interp = interp;
  date->flags = 0;
  date->reset = 1;
  date->month = 0;
  date->year = 0;
  date->lastmonth = 0;
  date->lastyear = 0;
  
  Tk_SetClass(date->tkwin, "DateView");
  Tk_CreateEventHandler(date->tkwin,
			VisibilityChangeMask |
			ExposureMask |
			StructureNotifyMask,
			SimDateEventProc, (ClientData) date);
  Tcl_CreateCommand(interp, Tk_PathName(date->tkwin),
		    DoDateCmd, (ClientData) date, (void (*)()) NULL);

/*
  Tk_MakeWindowExist(date->tkwin);
*/
  
  if (getenv("XSYNCHRONIZE") != NULL) {
    XSynchronize(Tk_Display(tkwin), 1);
  }

  InitNewDate(date);
  DoNewDate(date);

  if (ConfigureSimDate(interp, date, argc-2, argv+2, 0) != TCL_OK) {
    /* XXX: destroy date */
    Tk_DestroyWindow(date->tkwin);
    return TCL_ERROR;
  }

  interp->result = Tk_PathName(date->tkwin);
  return TCL_OK;
}


int
ConfigureSimDate(Tcl_Interp *interp, SimDate *date,
		  int argc, char **argv, int flags)
{
  if (Tk_ConfigureWidget(interp, date->tkwin, DateConfigSpecs,
			 argc, argv, (char *) date, flags) != TCL_OK) {
    return TCL_ERROR;
  }
  
  Tk_SetBackgroundFromBorder(date->tkwin, date->border);

  ComputeDateGeometry(date);

  EventuallyRedrawDate(date);

  return TCL_OK;
}


date_command_init()
{
  int new;

  Tcl_CreateCommand(tk_mainInterp, "dateview", DateViewCmd,
		    (ClientData)MainWindow, (void (*)()) NULL);

  Tcl_InitHashTable(&DateCmds, TCL_STRING_KEYS);

#define DATE_CMD(name) HASHED_CMD(Date, name)

  DATE_CMD(configure);
  DATE_CMD(position);
  DATE_CMD(size);
  DATE_CMD(Visible);
  DATE_CMD(Reset);
  DATE_CMD(Set);
}


InitNewDate(SimDate *date)
{
  int d = 8;
  struct XDisplay *xd;

  date->next = NULL;

/* This stuff was initialized in our caller (DateCmd) */
/*  date->tkwin = NULL; */
/*  date->interp = NULL; */
/*  date->flags = 0; */

  date->x = NULL;
  date->visible = 0;
  date->w_x = date->w_y = 0;
  date->w_width = date->w_height = 0;
  date->pixmap = None;
  date->pixels = NULL;
  date->fontPtr = NULL;
  date->border = NULL;
  date->borderWidth = 0;
  date->padX = 0;
  date->padY = 0;
  date->width = 0;
  date->monthTab = 0;
  date->monthTabX = 0;
  date->yearTab = 0;
  date->yearTabX = 0;
  date->draw_date_token = 0;
  date->reset = 1;
  date->year = 0;
  date->month = 0;
  date->lastyear = 0;
  date->lastmonth = 0;

  date->x = FindXDisplay(date->tkwin);
  IncRefDisplay(date->x);

  date->pixels = date->x->pixels;
  date->fontPtr = NULL;

  DoResizeDate(date, 16, 16);
}


DestroyDate(SimDate *date)
{
  SimDate **gp;

  for (gp = &sim->date;
       (*gp) != NULL;
       gp = &((*gp)->next)) {
    if ((*gp) == date) {
      (*gp) = date->next;
      sim->dates--;
      break;
    }
  }

  if (date->pixmap != None) {
    XFreePixmap(date->x->dpy, date->pixmap);
    date->pixmap = None;
  }

  DecRefDisplay(date->x);

  ckfree((char *) date);
}


DoResizeDate(SimDate *date, int w, int h)
{
  int resize = 0;

  date->w_width = w; date->w_height = h;

  if (date->pixmap != None) {
    XFreePixmap(date->x->dpy, date->pixmap);
    date->pixmap = None;
  }
  date->pixmap = XCreatePixmap(date->x->dpy, date->x->root,
				w, h, date->x->depth);
  if (date->pixmap == None) {
    fprintf(stderr,
	    "Sorry, Micropolis can't create a pixmap on X display \"%s\".\n",
	    date->x->display);
    sim_exit(1); // Just sets tkMustExit and ExitReturn
    return;
  }
}


DoNewDate(SimDate *date)
{
  sim->dates++; date->next = sim->date; sim->date = date;

  NewDate = 1;
}


#define BORDER 1

DoUpdateDate(SimDate *date)
{
  Display *dpy;
  GC gc;
  Pixmap pm;
  int *pix;
  int w, h, i, j, x, y;
  int tx, ty;
  float sx, sy;

  if (!date->visible) {
    return;
  }

  dpy = date->x->dpy;
  gc = date->x->gc;
  pm = date->pixmap;
  pix = date->pixels;

  w = date->w_width;
  h = date->w_height;

  XSetFont(date->x->dpy, date->x->gc, date->fontPtr->fid);

#if 0
  if (date->x->color) {
    XSetForeground(dpy, gc, pix[COLOR_LIGHTGRAY]);
  } else {
    XSetForeground(dpy, gc, pix[COLOR_WHITE]);
  }
#else
  XSetForeground(dpy, gc, Tk_3DBorderColor(date->border)->pixel);
#endif

  XFillRectangle(dpy, pm, gc, 0, 0, w, h);

  tx = BORDER; ty = BORDER;

  if ((w -= (2 * BORDER)) < 1) w = 1;
  if ((h -= (2 * BORDER)) < 1) h = 1;

  x = date->borderWidth + date->padX + 1;
  y = date->borderWidth + date->padY + date->fontPtr->ascent;

  if (date->reset) {
    date->reset = 0;
    date->lastyear = date->year;
    date->lastmonth = date->month;
  }

  {
    char *dateString = "Date:";
    char yearString[256];
    int month = date->month;
    int year = date->year;
    int lastmonth = date->lastmonth;
    int lastyear = date->lastyear;
    int yearsPassed;
    int monthsPassed;
    yearsPassed =
        (year - lastyear);
    if (yearsPassed < 0) yearsPassed = 1;
    if (yearsPassed > 9) yearsPassed = 9;
    monthsPassed =
      (month - lastmonth) +
      (12 * yearsPassed);

    if (monthsPassed > 11) monthsPassed = 11;
    if (monthsPassed == 1) monthsPassed = 0;
    if (monthsPassed) {
      int m = lastmonth;
      int i;

      XSetForeground(dpy, gc, pix[COLOR_DARKGRAY]);

      for (i = 0; i < monthsPassed; i++) {
	
	XDrawString(date->x->dpy, pm, date->x->gc,
		    x + date->monthTabX, y,
		    dateStr[m],
		    strlen(dateStr[date->month]));

	m++;
	if (m == 12) m = 0;
      }

      if (year != lastyear) {
	int yy = lastyear;
	if ((year - yy) > 10) {
	  yy = year - 10;
	}

	for (i = yy; i < year; i++) {
	  sprintf(
	    yearString,
	    "%d",
	    i);

	  XDrawString(date->x->dpy, pm, date->x->gc,
		      x + date->yearTabX, y,
		      yearString,
		      strlen(yearString));
	}
      }

      EventuallyRedrawDate(date);
    }

    date->lastmonth = month;
    date->lastyear = year;

    XSetForeground(dpy, gc, pix[COLOR_BLACK]);

    XDrawString(date->x->dpy, pm, date->x->gc,
		x, y,
		dateString,
		strlen(dateString));

    XDrawString(date->x->dpy, pm, date->x->gc,
		x + date->monthTabX, y,
		dateStr[date->month],
		strlen(dateStr[date->month]));

    sprintf(
      yearString,
      "%d",
      year);

    XDrawString(date->x->dpy, pm, date->x->gc,
		x + date->yearTabX, y,
		yearString,
		strlen(yearString));
  }

  XCopyArea(date->x->dpy, date->pixmap,
	    Tk_WindowId(date->tkwin), date->x->gc,
	    0, 0, date->w_width, date->w_height, 0, 0);
}


