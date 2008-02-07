/* w_tk.c
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

#ifdef MSDOS
#define filename2UNIX(name)	\
    {   char *p; for (p = name; *p; p++) if (*p == '\\') *p = '/'; }
#else
#define filename2UNIX(name)	/**/
#endif


Tcl_Interp *tk_mainInterp = NULL;
Tcl_CmdBuf buffer = NULL;
Tk_TimerToken sim_timer_token = 0;
int sim_timer_idle = 0;
int sim_timer_set = 0;
Tk_Window MainWindow;
int UpdateDelayed = 0;
int AutoScrollEdge = 16;
int AutoScrollStep = 16;
int AutoScrollDelay = 10;
Tk_TimerToken earthquake_timer_token;
int earthquake_timer_set = 0;
int earthquake_delay = 3000;
int PerformanceTiming;
double FlushTime;
int NeedRest = 0;


#define DEF_VIEW_FONT	"-Adobe-Helvetica-Bold-R-Normal-*-140-*"

Tk_ConfigSpec TileViewConfigSpecs[] = {
    {TK_CONFIG_FONT, "-font", (char *) NULL, (char *) NULL,
	DEF_VIEW_FONT, Tk_Offset(SimView, fontPtr), 0},
    {TK_CONFIG_STRING, "-messagevar", (char *) NULL, (char *) NULL,
	NULL, Tk_Offset(SimView, message_var), 0},
    {TK_CONFIG_PIXELS, "-width", "width", "Width",
	0, Tk_Offset(SimView, width), 0},
    {TK_CONFIG_PIXELS, "-height", "height", "Height",
	0, Tk_Offset(SimView, height), 0},
    {TK_CONFIG_END, (char *) NULL, (char *) NULL, (char *) NULL,
	(char *) NULL, 0, 0}
};


int TileViewCmd(CLIENT_ARGS);
int ConfigureTileView(Tcl_Interp *interp, SimView *view,
		      int argc, char **argv, int flags);
static void TileViewEventProc(ClientData clientData, XEvent *eventPtr);
static void DestroyTileView(ClientData clientData);

int ConfigureSimGraph(Tcl_Interp *interp, SimGraph *graph,
		      int argc, char **argv, int flags);

static void MicropolisTimerProc(ClientData clientData);

int SimCmd(CLIENT_ARGS);
int DoEditorCmd(CLIENT_ARGS);
int DoMapCmd(CLIENT_ARGS);
int GraphViewCmd(CLIENT_ARGS);
int DoGraphCmd(CLIENT_ARGS);
int SpriteCmd(CLIENT_ARGS);
extern int Tk_PieMenuCmd();
extern int Tk_IntervalCmd();


int
TileViewCmd(CLIENT_ARGS)
{
  Tk_Window tkwin = (Tk_Window) clientData;
  SimView *view;
  int viewclass;

  if (argc < 2) {
    Tcl_AppendResult(interp, "wrong # args:  should be \"",
		     argv[0], " pathName ?options?\"", (char *) NULL);
    return TCL_ERROR;
  }

  if (strcmp(argv[0], "editorview") == 0)
    viewclass = Editor_Class;
  else if (strcmp(argv[0], "mapview") == 0)
    viewclass = Map_Class;
  else {
    return TCL_ERROR;
  }

  tkwin = Tk_CreateWindowFromPath(interp, tkwin,
				  argv[1], (char *) NULL);
  if (tkwin == NULL) {
    return TCL_ERROR;
  }

  view = (SimView *)ckalloc(sizeof (SimView));

  view->tkwin = tkwin;
  view->interp = interp;
  view->flags = 0;

  if (viewclass == Editor_Class) {
    Tk_SetClass(view->tkwin, "EditorView");

    Tk_CreateEventHandler(view->tkwin,
			  VisibilityChangeMask |
			  ExposureMask |
			  StructureNotifyMask |
			  EnterWindowMask |
			  LeaveWindowMask |
			  PointerMotionMask,
			  TileViewEventProc, (ClientData) view);
    Tcl_CreateCommand(interp, Tk_PathName(view->tkwin),
		      DoEditorCmd, (ClientData) view, (void (*)()) NULL);
  } else {
    Tk_SetClass(view->tkwin, "MapView");

    Tk_CreateEventHandler(view->tkwin,
			  VisibilityChangeMask |
			  ExposureMask |
			  StructureNotifyMask /* |
			  EnterWindowMask |
			  LeaveWindowMask |
			  PointerMotionMask */ ,
			  TileViewEventProc, (ClientData) view);
    Tcl_CreateCommand(interp, Tk_PathName(view->tkwin),
		      DoMapCmd, (ClientData) view, (void (*)()) NULL);
  }

  Tk_MakeWindowExist(view->tkwin);

  if (getenv("XSYNCHRONIZE") != NULL) {
    XSynchronize(Tk_Display(tkwin), 1);
  }

  if (viewclass == Editor_Class) {
    InitNewView(view, "MicropolisEditor", Editor_Class, EDITOR_W, EDITOR_H);
    DoNewEditor(view);
  } else {
    InitNewView(view, "MicropolisMap", Map_Class, MAP_W, MAP_H);
    DoNewMap(view);
  }

  if (ConfigureTileView(interp, view, argc-2, argv+2, 0) != TCL_OK) {
    /* XXX: destroy view */
    Tk_DestroyWindow(view->tkwin);
    return TCL_ERROR;
  }

  switch (view->class) {
  case Editor_Class:
    break;
  case Map_Class:
    view->invalid = 1;
    view->update = 1;
    DoUpdateMap(view);
    break;
  }

  interp->result = Tk_PathName(view->tkwin);
  return TCL_OK;
}


int
ConfigureTileView(Tcl_Interp *interp, SimView *view,
	     int argc, char **argv, int flags)
{
  if (Tk_ConfigureWidget(interp, view->tkwin, TileViewConfigSpecs,
			 argc, argv, (char *) view, flags) != TCL_OK) {
    return TCL_ERROR;
  }

  if (view->class == Map_Class) {
    Tk_GeometryRequest(view->tkwin, MAP_W, MAP_H);
  } else {
    if (view->width || view->height) {
      Tk_GeometryRequest(view->tkwin, view->width, view->height);
    }
  }
  EventuallyRedrawView(view);
  return TCL_OK;
}


InvalidateMaps()
{
  SimView *view;

//fprintf(stderr, "InvalidateMaps\n");
  for (view = sim->map; view != NULL; view = view->next) {
    view->invalid = 1;
    view->skip = 0;
    EventuallyRedrawView(view);
  }
  sim_skip = 0;
}


InvalidateEditors()
{
  SimView *view;

//fprintf(stderr, "InvalidateEditors\n");
  for (view = sim->editor; view != NULL; view = view->next) {
    view->invalid = 1;
    view->skip = 0;
    EventuallyRedrawView(view);
  }
  sim_skip = 0;
}


RedrawMaps()
{
  SimView *view;

//fprintf(stderr, "RedrawMaps\n");

  for (view = sim->map; view != NULL; view = view->next) {
    view->skip = 0;
    EventuallyRedrawView(view);
  }
  sim_skip = 0;
}


RedrawEditors()
{
  SimView *view;

//fprintf(stderr, "RedrawEditors\n");

  for (view = sim->editor; view != NULL; view = view->next) {
    view->skip = 0;
    EventuallyRedrawView(view);
  }
  sim_skip = 0;
}


static void
DisplayTileView(ClientData clientData)
{
  SimView *view = (SimView *) clientData;
  Tk_Window tkwin = view->tkwin;
  Pixmap pm = None;
  Drawable d;

  view->flags &= ~VIEW_REDRAW_PENDING;
  if (view->visible && (tkwin != NULL) && Tk_IsMapped(tkwin)) {
    switch (view->class) {
    case Editor_Class:
      view->skip = 0;
      view->update = 1;
      DoUpdateEditor(view);
      break;
    case Map_Class:
//fprintf(stderr, "DisplayTileView\n");
      view->skip = 0;
      view->update = 1;
      DoUpdateMap(view);
      break;
    }
  }
}


/* comefrom:
    ConfigureTileView
    TileViewEventProc expose configure motion
    InvalidateMaps
    EraserTo
    DoSetMapState
    AddInk
    EraserTo
 */

EventuallyRedrawView(SimView *view)
{
  if (!(view->flags & VIEW_REDRAW_PENDING)) {
    Tk_DoWhenIdle(DisplayTileView, (ClientData) view);
    view->flags |= VIEW_REDRAW_PENDING;
  }

}


CancelRedrawView(SimView *view)
{
  if (view->flags & VIEW_REDRAW_PENDING) {
    Tk_CancelIdleCall(DisplayTileView, (ClientData) view);
  }
  view->flags &= ~VIEW_REDRAW_PENDING;
}


static void
TileAutoScrollProc(ClientData clientData)
{
  SimView *view = (SimView *)clientData;
  char buf[256];

  if (view->tool_mode != 0) {
    int dx = 0, dy = 0;
    int result, root_x, root_y, x, y;
    unsigned int key_buttons;
    Window root, child;

    XQueryPointer(Tk_Display(view->tkwin), Tk_WindowId(view->tkwin),
		  &root, &child, &root_x, &root_y, &x, &y, &key_buttons);

    if (x < AutoScrollEdge)
      dx = -AutoScrollStep;
    else if (x > (view->w_width - AutoScrollEdge))
      dx = AutoScrollStep;
    if (y < AutoScrollEdge)
      dy = -AutoScrollStep;
    else if (y > (view->w_height - AutoScrollEdge))
      dy = AutoScrollStep;

    if (dx || dy) {
      int px = view->pan_x, py = view->pan_y;

      if (view->tool_mode == -1) {
	dx = -dx; dy = -dy;
      }

      DoPanBy(view, dx, dy);
      view->tool_x += view->pan_x - px;
      view->tool_y += view->pan_y - py;
      view->auto_scroll_token =
	Tk_CreateTimerHandler(AutoScrollDelay, TileAutoScrollProc,
			      (ClientData) view);

      sprintf(buf, "UIDidPan %s %d %d", Tk_PathName(view->tkwin), x, y);
      Eval(buf);
    }
  }
}


static void
TileViewEventProc(ClientData clientData, XEvent *eventPtr)
{
  SimView *view = (SimView *) clientData;

  if ((eventPtr->type == Expose) && (eventPtr->xexpose.count == 0)) {
    view->visible = 1;
    EventuallyRedrawView(view);
  } else if (eventPtr->type == MapNotify) {
    view->visible = 1;
  } else if (eventPtr->type == UnmapNotify) {
    view->visible = 0;
  } else if (eventPtr->type == VisibilityNotify) {
    if (eventPtr->xvisibility.state == VisibilityFullyObscured)
      view->visible = 0;
    else
      view->visible = 1;
  } else if (eventPtr->type == ConfigureNotify) {
    if (view->class == Editor_Class)
      DoResizeView(view,
		   eventPtr->xconfigure.width,
		   eventPtr->xconfigure.height);
    EventuallyRedrawView(view);
  } else if (eventPtr->type == DestroyNotify) {
    Tcl_DeleteCommand(view->interp, Tk_PathName(view->tkwin));
    view->tkwin = NULL;
    CancelRedrawView(view);
    Tk_EventuallyFree((ClientData) view, DestroyTileView);
  } else if ((view->class == Editor_Class) &&
	     (view->show_me != 0) &&
	     ((eventPtr->type == EnterNotify) ||
	      (eventPtr->type == LeaveNotify) ||
	      (eventPtr->type == MotionNotify))) {
    int last_x = view->tool_x, last_y = view->tool_y,
        last_showing = view->tool_showing;
    int x, y, showing, autoscroll;

    if (eventPtr->type == EnterNotify) {
      showing = 1;
      x = eventPtr->xcrossing.x; y = eventPtr->xcrossing.y;
    } else if (eventPtr->type == LeaveNotify) {
      showing = 0;
      x = eventPtr->xcrossing.x; y = eventPtr->xcrossing.y;
    } else {
      showing = 1;
      x = eventPtr->xmotion.x; y = eventPtr->xmotion.y;
    }

    if (view->tool_mode != 0) {

      if ((x < AutoScrollEdge) ||
	  (x > (view->w_width - AutoScrollEdge)) ||
	  (y < AutoScrollEdge) ||
	  (y > (view->w_height - AutoScrollEdge))) {
	if (!view->auto_scroll_token) {
	  view->auto_scroll_token =
	    Tk_CreateTimerHandler(AutoScrollDelay, TileAutoScrollProc,
				  (ClientData) view);
	}
      } else {
	if (view->auto_scroll_token) {
	  Tk_DeleteTimerHandler(view->auto_scroll_token);
	  view->auto_scroll_token = 0;
	}
      }
    }

    ViewToPixelCoords(view, x, y, &x, &y);
    view->tool_showing = showing;

    if (view->tool_mode != -1) {
      view->tool_x = x; view->tool_y = y;
    }

/* XXX: redraw all views showing cursor */
/* XXX: also, make sure switching tools works w/out moving */
    if (((view->tool_showing != last_showing) ||
	 (view->tool_x != last_x) ||
	 (view->tool_y != last_y))) {
#if 1
      EventuallyRedrawView(view);
#else
      RedrawEditors();
#endif
    }
  }
}


static void
DestroyTileView(ClientData clientData)
{
  SimView *view = (SimView *) clientData;

  DestroyView(view);
}


void
StdinProc(ClientData clientData, int mask)
{
  char line[200];
  static int gotPartial = 0;
  char *cmd;
  int result;
  
  if (mask & TK_READABLE) {
    if (fgets(line, 200, stdin) == NULL) {
      if (!gotPartial) {
	if (sim_tty) {
	  sim_exit(0); // Just sets tkMustExit and ExitReturn
	  return;
	} else {
	  Tk_DeleteFileHandler(0);
	}
	return;
      } else {
	line[0] = 0;
      }
    }
    cmd = Tcl_AssembleCmd(buffer, line);
    if (cmd == NULL) {
      gotPartial = 1;
      return;
    }
    gotPartial = 0;
    result = Tcl_RecordAndEval(tk_mainInterp, cmd, 0);
    if (*tk_mainInterp->result != 0) {
      if ((result != TCL_OK) || sim_tty) {
	printf("%s\n", tk_mainInterp->result);
      }
    }
    if (sim_tty) {
      printf("sim:\n");
      fflush(stdout);
    }
  }
}


static void
StructureProc(ClientData clientData, XEvent *eventPtr)
{
  if (eventPtr->type == DestroyNotify) {
    MainWindow = NULL;
  }
}


static void
DelayedMap(ClientData clientData)
{
  while (Tk_DoOneEvent(TK_IDLE_EVENTS) != 0) {
    /* Empty loop body. */
  }
  if (MainWindow == NULL) {
    return;
  }
  Tk_MapWindow(MainWindow);
}


DidStopPan(SimView *view)
{
  char buf[256];
  sprintf(buf, "UIDidStopPan %s", Tk_PathName(view->tkwin));
	
  Eval(buf);
}


static void
MicropolisTimerProc(ClientData clientData)
{
  sim_timer_token = NULL;
  sim_timer_set = 0;

  if (NeedRest > 0) {
    NeedRest--;
  }

  if (SimSpeed) {
    sim_loop(1);
    StartMicropolisTimer();
  } else {
    StopMicropolisTimer();
  }
}


void
ReallyStartMicropolisTimer(ClientData clientData)
{
  int delay = sim_delay;
  XDisplay *xd = XDisplays;

  StopMicropolisTimer();

  while (xd != NULL) {
    if ((NeedRest > 0) ||
	ShakeNow ||
	(xd->tkDisplay->buttonWinPtr != NULL) ||
	(xd->tkDisplay->grabWinPtr != NULL)) {
      if (ShakeNow || NeedRest) {
	if (delay < 50000) delay = 50000;
      } else {
      }
      break;
    }
    xd = xd->next;
  }

  sim_timer_token =
    Tk_CreateMicroTimerHandler(
      0,
      delay,
      MicropolisTimerProc,
      (ClientData)0);

  sim_timer_set = 1;
}


StartMicropolisTimer()
{
  if (sim_timer_idle == 0) {
    sim_timer_idle = 1;
    Tk_DoWhenIdle(
      ReallyStartMicropolisTimer,
      NULL);
  }
}


StopMicropolisTimer()
{
  if (sim_timer_idle != 0) {
    sim_timer_idle = 0;
    Tk_CancelIdleCall(
      ReallyStartMicropolisTimer,
      NULL);
  }

  if (sim_timer_set) {
    if (sim_timer_token != 0) {
      Tk_DeleteTimerHandler(sim_timer_token);
      sim_timer_token = 0;
    }
    sim_timer_set = 0;
  }
}


FixMicropolisTimer()
{
  if (sim_timer_set) {
    StartMicropolisTimer(NULL);
  }
}


static void
DelayedUpdate(ClientData clientData)
{
//fprintf(stderr, "DelayedUpdate\n");
  UpdateDelayed = 0;
  sim_skip = 0;
  sim_update();
}


Kick()
{
  if (!UpdateDelayed) {
    UpdateDelayed = 1;
    Tk_DoWhenIdle(DelayedUpdate, (ClientData) NULL);
  }
}


void
StopEarthquake()
{
  ShakeNow = 0;
  if (earthquake_timer_set) {
    Tk_DeleteTimerHandler(earthquake_timer_token);
  }
  earthquake_timer_set = 0;
}


DoEarthQuake(void)
{
  MakeSound("city", "Explosion-Low");
  Eval("UIEarthQuake");
  ShakeNow++;
  if (earthquake_timer_set) {
    Tk_DeleteTimerHandler(earthquake_timer_token);
  }
  Tk_CreateTimerHandler(earthquake_delay, (void (*)())StopEarthquake, (ClientData) 0);
  earthquake_timer_set = 1;
}


StopToolkit()
{
  if (tk_mainInterp != NULL) {
    Eval("catch {DoStopMicropolis}");
  }
}


Eval(char *buf)
{
  int result = Tcl_Eval(tk_mainInterp, buf, 0, (char **) NULL);
  if (result != TCL_OK) {
    char *errorinfo = Tcl_GetVar(tk_mainInterp, "errorInfo",
				 TCL_GLOBAL_ONLY);
    if (errorinfo == NULL) errorinfo = "<no backtrace>";
    fprintf(stderr, "Micropolis: error in TCL code: %s\n%s\n",
	    tk_mainInterp->result, errorinfo);
  }
  return (result);
}


tk_main()
{
  char *p, *msg;
  char buf[20];
  char initCmd[256];
  Tk_3DBorder border;

  tk_mainInterp = Tcl_CreateExtendedInterp();

#if 0
  /* XXX: Figure out Extended TCL */
  tclAppName     = "Wish";
  tclAppLongname = "Wish - Tk Shell";
  tclAppVersion  = TK_VERSION;
  Tcl_ShellEnvInit (interp, TCLSH_ABORT_STARTUP_ERR,
                    name,
                    0, NULL,           /* argv var already set  */
                    fileName == NULL,  /* interactive?          */
                    NULL);             /* Standard default file */
#endif

  MainWindow = Tk_CreateMainWindow(tk_mainInterp, FirstDisplay, "Micropolis");
  if (MainWindow == NULL) {
    fprintf(stderr, "%s\n", tk_mainInterp->result);
    sim_really_exit(1); // Just sets tkMustExit and ExitReturn
  }
  Tk_SetClass(MainWindow, "Tk");
  Tk_CreateEventHandler(MainWindow, StructureNotifyMask,
			StructureProc, (ClientData) NULL);
/*  Tk_DoWhenIdle(DelayedMap, (ClientData) NULL); */

  Tk_GeometryRequest(MainWindow, 256, 256);
  border = Tk_Get3DBorder(tk_mainInterp, MainWindow, None, "gray75");
  if (border == NULL) {
    Tcl_SetResult(tk_mainInterp, (char *) NULL, TCL_STATIC);
    Tk_SetWindowBackground(MainWindow,
			   WhitePixelOfScreen(Tk_Screen(MainWindow)));
  } else {
    Tk_SetBackgroundFromBorder(MainWindow, border);
  }
  XSetForeground(Tk_Display(MainWindow),
		 DefaultGCOfScreen(Tk_Screen(MainWindow)),
		 BlackPixelOfScreen(Tk_Screen(MainWindow)));

  sim_command_init();
  map_command_init();
  editor_command_init();
  graph_command_init();
  date_command_init();
  sprite_command_init();

#ifdef CAM
  cam_command_init();
#endif

  Tcl_CreateCommand(tk_mainInterp, "piemenu", Tk_PieMenuCmd,
		    (ClientData)MainWindow, (void (*)()) NULL);
  Tcl_CreateCommand(tk_mainInterp, "interval", Tk_IntervalCmd,
		    (ClientData)MainWindow, (void (*)()) NULL);

  sim = MakeNewSim();

  sprintf(initCmd, "source %s/micropolis.tcl", ResourceDir);
  filename2UNIX(initCmd);
  if (Eval(initCmd)) {
    sim_exit(1); // Just sets tkMustExit and ExitReturn
    goto bail;
  }

  sim_init();

  buffer = Tcl_CreateCmdBuf();

  if (sim_tty) {
    Tk_CreateFileHandler(0, TK_READABLE, StdinProc, (ClientData) 0);
  }

  { char buf[1024];

    sprintf(buf, "UIStartMicropolis {%s} {%s} {%s}",
	    HomeDir, ResourceDir, HostName);
    filename2UNIX(buf);
    if (Eval(buf) != TCL_OK) {
      sim_exit(1); // Just sets tkMustExit and ExitReturn
      goto bail;
    }
  }

  if (sim_tty) {
    printf("sim:\n");
  }
  fflush(stdout);

  Tk_MainLoop();

  sim_exit(0); // Just sets tkMustExit and ExitReturn

 bail:

  if (buffer != NULL) {
    Tcl_DeleteCmdBuf(buffer);
  }

  Tcl_DeleteInterp(tk_mainInterp);
}

