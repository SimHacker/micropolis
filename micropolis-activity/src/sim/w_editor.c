/* w_editor.c
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


Tcl_HashTable EditorCmds;
int DoOverlay = 2;
int BobHeight = 8;


extern Tk_ConfigSpec TileViewConfigSpecs[];


int EditorCmdconfigure(VIEW_ARGS)
{
  int result = TCL_OK;

  if (argc == 2) {
    result = Tk_ConfigureInfo(interp, view->tkwin, TileViewConfigSpecs,
			      (char *) view, (char *) NULL, 0);
  } else if (argc == 3) {
    result = Tk_ConfigureInfo(interp, view->tkwin, TileViewConfigSpecs,
			      (char *) view, argv[2], 0);
  } else {
    result = ConfigureTileView(interp, view, argc-2, argv+2,
			       TK_CONFIG_ARGV_ONLY);
  }
  return TCL_OK;
}


int EditorCmdposition(VIEW_ARGS)
{
  if ((argc != 2) && (argc != 4)) {
    return TCL_ERROR;
  }
  if (argc == 4) {
    if ((Tcl_GetInt(interp, argv[2], &view->w_x) != TCL_OK) ||
	(Tcl_GetInt(interp, argv[3], &view->w_y) != TCL_OK)) {
      return TCL_ERROR;
    }
  }
  sprintf(interp->result, "%d %d", view->w_x, view->w_y);
  return TCL_OK;
}


int EditorCmdsize(VIEW_ARGS)
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
    view->w_width = w;
    view->w_height = h;
  }
  sprintf(interp->result, "%d %d", view->w_width, view->w_height);
  return TCL_OK;
}


int EditorCmdAutoGoto(VIEW_ARGS)
{
  if ((argc != 2) && (argc != 3)) {
    return TCL_ERROR;
  }
  if (argc == 3) {
    int val;

    if (Tcl_GetInt(interp, argv[2], &val) != TCL_OK) {
      return TCL_ERROR;
    }
    view->auto_goto = val;
    view->auto_going = view->auto_x_goal = view->auto_y_goal = 0;
  }
  sprintf(interp->result, "%d", view->auto_goto);
  return TCL_OK;
}


int EditorCmdSound(VIEW_ARGS)
{
  if ((argc != 2) && (argc != 3)) {
    return TCL_ERROR;
  }
  if (argc == 3) {
    int val;

    if (Tcl_GetInt(interp, argv[2], &val) != TCL_OK) {
      return TCL_ERROR;
    }
    view->sound = val;
    view->auto_going = view->auto_x_goal = view->auto_y_goal = 0;
  }
  sprintf(interp->result, "%d", view->sound);
  return TCL_OK;
}


int EditorCmdSkip(VIEW_ARGS)
{
  if ((argc != 2) && (argc != 3)) {
    return TCL_ERROR;
  }
  if (argc == 3) {
    int val;

    if (Tcl_GetInt(interp, argv[2], &val) != TCL_OK) {
      return TCL_ERROR;
    }
    view->skip = view->skips = val; // XXX? why setting skips too?
  }
  sprintf(interp->result, "%d", view->skips);
  return TCL_OK;
}


int EditorCmdUpdate(VIEW_ARGS)
{
  if (argc != 2) {
    return TCL_ERROR;
  }
  view->skip = 0;
  return TCL_OK;
}


int EditorCmdPan(VIEW_ARGS)
{
  if ((argc != 2) && (argc != 4)) {
    return TCL_ERROR;
  }
  if (argc == 4) {
    int x, y;

    if (Tcl_GetInt(interp, argv[2], &x) != TCL_OK) {
      return TCL_ERROR;
    }
    if (Tcl_GetInt(interp, argv[3], &y) != TCL_OK) {
      return TCL_ERROR;
    }
    DoPanTo(view, x, y); Kick();
  }
  sprintf(interp->result, "%d %d", view->pan_x, view->pan_y);
  return TCL_OK;
}


int EditorCmdToolConstrain(VIEW_ARGS)
{
  int x = -1, y = -1, tx, ty;

  if (argc != 4) {
    return TCL_ERROR;
  }

  if (Tcl_GetInt(interp, argv[2], &x) != TCL_OK) {
    return TCL_ERROR;
  }
  if (Tcl_GetInt(interp, argv[3], &y) != TCL_OK) {
    return TCL_ERROR;
  }
  view->tool_x_const = -1; view->tool_y_const = -1;
  ViewToTileCoords(view, x, y, &tx, &ty);
  view->tool_x_const = (x == -1) ? -1 : tx;
  view->tool_y_const = (y == -1) ? -1 : ty;
  return TCL_OK;
}


int EditorCmdToolState(VIEW_ARGS)
{
  if ((argc != 2) && (argc != 3)) {
    return TCL_ERROR;
  }
  if (argc == 3) {
    int state;

    if (Tcl_GetInt(interp, argv[2], &state) != TCL_OK) {
      return TCL_ERROR;
    }
    setWandState(view, state);
  }
  sprintf(interp->result, "%d", view->tool_state);
  return TCL_OK;
}


int EditorCmdToolMode(VIEW_ARGS)
{
  if ((argc != 2) && (argc != 3)) {
    return TCL_ERROR;
  }
  if (argc == 3) {
    int mode;

    if (Tcl_GetInt(interp, argv[2], &mode) != TCL_OK) {
      return TCL_ERROR;
    }
    view->tool_mode = mode;
  }
  sprintf(interp->result, "%d", view->tool_mode);
  return TCL_OK;
}


int EditorCmdDoTool(VIEW_ARGS)
{
  int tool, x, y;

  if (argc != 5) {
    return TCL_ERROR;
  }

  if ((Tcl_GetInt(interp, argv[2], &tool) != TCL_OK) ||
      (tool < 0) ||
      (tool > lastState) ||
      (Tcl_GetInt(interp, argv[3], &x) != TCL_OK) ||
      (Tcl_GetInt(interp, argv[4], &y) != TCL_OK)) {
    return TCL_ERROR;
  }

  DoTool(view, tool, x, y); Kick();
  return TCL_OK;
}


int EditorCmdToolDown(VIEW_ARGS)
{
  int x, y;

  if (argc != 4) {
    return TCL_ERROR;
  }

  if ((Tcl_GetInt(interp, argv[2], &x) != TCL_OK) ||
      (Tcl_GetInt(interp, argv[3], &y) != TCL_OK)) {
    return TCL_ERROR;
  }

  ToolDown(view, x, y); Kick();
  return TCL_OK;
}


int EditorCmdToolDrag(VIEW_ARGS)
{
  int x, y;

  if (argc != 4) {
    return TCL_ERROR;
  }

  if ((Tcl_GetInt(interp, argv[2], &x) != TCL_OK) ||
      (Tcl_GetInt(interp, argv[3], &y) != TCL_OK)) {
    return TCL_ERROR;
  }

  ToolDrag(view, x, y); Kick();
  return TCL_OK;
}


int EditorCmdToolUp(VIEW_ARGS)
{
  int x, y;

  if (argc != 4) {
    return TCL_ERROR;
  }

  if ((Tcl_GetInt(interp, argv[2], &x) != TCL_OK) ||
      (Tcl_GetInt(interp, argv[3], &y) != TCL_OK)) {
    return TCL_ERROR;
  }

  ToolUp(view, x, y); Kick();
  return TCL_OK;
}


int EditorCmdPanStart(VIEW_ARGS)
{
  int x, y;

  if (argc != 4) {
    return TCL_ERROR;
  }

  if ((Tcl_GetInt(interp, argv[2], &x) != TCL_OK) ||
      (Tcl_GetInt(interp, argv[3], &y) != TCL_OK)) {
    return TCL_ERROR;
  }

  view->last_x = x;
  view->last_y = y;
  return TCL_OK;
}


int EditorCmdPanTo(VIEW_ARGS)
{
  int x, y, dx, dy;

  if (argc != 4) {
    return TCL_ERROR;
  }

  if ((Tcl_GetInt(interp, argv[2], &x) != TCL_OK) ||
      (Tcl_GetInt(interp, argv[3], &y) != TCL_OK)) {
    return TCL_ERROR;
  }

  dx = (view->tool_x_const == -1) ? (view->last_x - x) : 0;
  dy = (view->tool_y_const == -1) ? (view->last_y - y) : 0;
  if (dx || dy) {
    view->last_x = x;
    view->last_y = y;
    DoPanBy(view, dx, dy); Kick();
  }
  return TCL_OK;
}


int EditorCmdPanBy(VIEW_ARGS)
{
  int dx, dy;

  if (argc != 4) {
    return TCL_ERROR;
  }

  if ((Tcl_GetInt(interp, argv[2], &dx) != TCL_OK) ||
      (Tcl_GetInt(interp, argv[3], &dy) != TCL_OK)) {
    return TCL_ERROR;
  }

  DoPanBy(view, dx, dy); Kick();
  return TCL_OK;
}


int EditorCmdTweakCursor(VIEW_ARGS)
{
  int x, y;

  XWarpPointer (view->x->dpy, None, None, 0, 0, 0, 0, 0, 0);

  return TCL_OK;
}


int EditorCmdVisible(VIEW_ARGS)
{
  int visible;

  if ((argc != 2) && (argc != 3)) {
    return TCL_ERROR;
  }

  if (argc == 3) {
    if ((Tcl_GetInt(interp, argv[2], &visible) != TCL_OK) ||
	(visible < 0) || (visible > 1)) {
      return TCL_ERROR;
    }

    visible = visible && Tk_IsMapped(view->tkwin);
    view->visible = visible;
  }

  sprintf(interp->result, "%d", view->visible);
  return TCL_OK;
}


int EditorCmdKeyDown(VIEW_ARGS)
{

  if (argc != 3) {
    return TCL_ERROR;
  }

  doKeyDown(view, argv[2][0]);
  return TCL_OK;
}


int EditorCmdKeyUp(VIEW_ARGS)
{

  if (argc != 3) {
    return TCL_ERROR;
  }

  doKeyUp(view, argv[2][0]);
  return TCL_OK;
}


int EditorCmdTileCoord(VIEW_ARGS)
{
  int x, y;

  if ((argc != 2) && (argc != 4)) {
    return TCL_ERROR;
  }

  if (Tcl_GetInt(interp, argv[2], &x) != TCL_OK) {
    return TCL_ERROR;
  }
  if (Tcl_GetInt(interp, argv[3], &y) != TCL_OK) {
    return TCL_ERROR;
  }

  ViewToTileCoords(view, x, y, &x, &y);

  sprintf(interp->result, "%d %d", x, y);
  return TCL_OK;
}


int EditorCmdChalkStart(VIEW_ARGS)
{
  int x, y;

  if (argc != 4) {
    return TCL_ERROR;
  }

  if ((Tcl_GetInt(interp, argv[2], &x) != TCL_OK) ||
      (Tcl_GetInt(interp, argv[3], &y) != TCL_OK)) {
    return TCL_ERROR;
  }

  ChalkStart(view, x, y, COLOR_WHITE);
  return TCL_OK;
}


int EditorCmdChalkTo(VIEW_ARGS)
{
  int x, y;

  if (argc != 4) {
    return TCL_ERROR;
  }

  if ((Tcl_GetInt(interp, argv[2], &x) != TCL_OK) ||
      (Tcl_GetInt(interp, argv[3], &y) != TCL_OK)) {
    return TCL_ERROR;
  }

  ChalkTo(view, x, y);
  return TCL_OK;
}


int EditorCmdAutoGoing(VIEW_ARGS)
{
  int flag;

  if ((argc != 2) && (argc != 3)) {
    return TCL_ERROR;
  }

  if (argc == 3) {
    if ((Tcl_GetInt(interp, argv[2], &flag) != TCL_OK)) {
      return TCL_ERROR;
    }

    view->auto_going = flag;
    if (view->auto_goto == -1)
      view->auto_goto = 0;
  }

  sprintf(interp->result, "%d", view->auto_going);
  return TCL_OK;
}


int EditorCmdAutoSpeed(VIEW_ARGS)
{
  int speed;

  if ((argc != 2) && (argc != 3)) {
    return TCL_ERROR;
  }

  if (argc == 3) {
    if ((Tcl_GetInt(interp, argv[2], &speed) != TCL_OK) ||
	(speed < 1)) {
      return TCL_ERROR;
    }

    view->auto_speed = speed;
  }

  sprintf(interp->result, "%d", view->auto_speed);
  return TCL_OK;
}


int EditorCmdAutoGoal(VIEW_ARGS)
{
  if ((argc != 2) && (argc != 4)) {
    return TCL_ERROR;
  }
  if (argc == 4) {
    int x, y, dx, dy;

    if (Tcl_GetInt(interp, argv[2], &x) != TCL_OK) {
      return TCL_ERROR;
    }
    if (Tcl_GetInt(interp, argv[3], &y) != TCL_OK) {
      return TCL_ERROR;
    }
    view->auto_x_goal = x;
    view->auto_y_goal = y;
    /* actually go there if more than a block away */
    dx = view->pan_x - x;
    dy = view->pan_y - y;
    view->auto_going = (((dx * dx) + (dy * dy)) > (64 * 64));
    if ((view->auto_going != 0) &&
	(view->auto_goto == 0))
      view->auto_goto = -1;
  }

  sprintf(interp->result, "%d %d", view->auto_x_goal, view->auto_y_goal);
  return TCL_OK;
}


int EditorCmdSU(VIEW_ARGS)
{
  int su;

  if ((argc != 2) && (argc != 4)) {
    return TCL_ERROR;
  }

  if (argc == 4) {
    if ((strcmp(argv[3], "xyzzy") != 0) ||
	(Tcl_GetInt(interp, argv[2], &su) != TCL_OK)) {
      return TCL_ERROR;
    }

    view->super_user = su;
  }

  sprintf(interp->result, "%d", view->super_user);
  return TCL_OK;
}


int EditorCmdShowMe(VIEW_ARGS)
{
  int flag;

  if ((argc != 2) && (argc != 3)) {
    return TCL_ERROR;
  }

  if (argc == 3) {
    if (Tcl_GetInt(interp, argv[2], &flag) != TCL_OK) {
      return TCL_ERROR;
    }

    view->show_me = flag;
  }

  sprintf(interp->result, "%d", view->show_me);
  return TCL_OK;
}


int EditorCmdFollow(VIEW_ARGS)
{
  int id;
  SimSprite *sprite;

  if ((argc != 2) && (argc != 3)) {
    return TCL_ERROR;
  }

  if (argc == 3) {
    view->follow = NULL;
    if (argv[2][0] != '\0') {
      for (sprite = sim->sprite; sprite != NULL; sprite = sprite->next) {
	if (strcmp(sprite->name, argv[2]) == 0) {
	  view->follow = sprite;
	  break;
	}
      }
    }

    if (view->follow != NULL) {
      HandleAutoGoto(view);
    }
  }

  sprintf(interp->result, "%s",
	  (view->follow == NULL) ? "" : view->follow->name);
  return TCL_OK;
}


int EditorCmdShowOverlay(VIEW_ARGS)
{
  int flag;

  if ((argc != 2) && (argc != 3)) {
    return TCL_ERROR;
  }

  if (argc == 3) {
    if ((Tcl_GetInt(interp, argv[2], &flag) != TCL_OK)) {
      return TCL_ERROR;
    }

    view->show_overlay = flag;
  }

  sprintf(interp->result, "%d", view->show_overlay);
  return TCL_OK;
}


int EditorCmdOverlayMode(VIEW_ARGS)
{
  int flag;

  if ((argc != 2) && (argc != 3)) {
    return TCL_ERROR;
  }

  if (argc == 3) {
    if ((Tcl_GetInt(interp, argv[2], &flag) != TCL_OK)) {
      return TCL_ERROR;
    }

    view->overlay_mode = flag;
  }

  sprintf(interp->result, "%d", view->overlay_mode);
  return TCL_OK;
}


int EditorCmdDynamicFilter(VIEW_ARGS)
{
  int val;

  if ((argc != 2) && (argc != 3)) {
    return TCL_ERROR;
  }

  if (argc == 3) {
    if ((Tcl_GetInt(interp, argv[2], &val) != TCL_OK)) {
      return TCL_ERROR;
    }

    view->dynamic_filter = val;
  }

  sprintf(interp->result, "%d", view->dynamic_filter);
  return TCL_OK;
}


int EditorCmdWriteJpeg(VIEW_ARGS)
{
  int val;
  char *fileName = argv[2];

  if (argc != 3) {
    return TCL_ERROR;
  }
  
  // Write a jpeg file of this view.

  return TCL_OK;
}


editor_command_init()
{
  int new;
  extern int TileViewCmd(CLIENT_ARGS);

  Tcl_CreateCommand(tk_mainInterp, "editorview", TileViewCmd,
		    (ClientData)MainWindow, (void (*)()) NULL);

  Tcl_InitHashTable(&EditorCmds, TCL_STRING_KEYS);

#define EDITOR_CMD(name) HASHED_CMD(Editor, name)

  EDITOR_CMD(configure);
  EDITOR_CMD(position);
  EDITOR_CMD(size);
  EDITOR_CMD(AutoGoto);
  EDITOR_CMD(Sound);
  EDITOR_CMD(Skip);
  EDITOR_CMD(Update);
  EDITOR_CMD(Pan);
  EDITOR_CMD(ToolConstrain);
  EDITOR_CMD(ToolState);
  EDITOR_CMD(ToolMode);
  EDITOR_CMD(DoTool);
  EDITOR_CMD(ToolDown);
  EDITOR_CMD(ToolDrag);
  EDITOR_CMD(ToolUp);
  EDITOR_CMD(PanStart);
  EDITOR_CMD(PanTo);
  EDITOR_CMD(PanBy);
  EDITOR_CMD(TweakCursor);
  EDITOR_CMD(Visible);
  EDITOR_CMD(KeyDown);
  EDITOR_CMD(KeyUp);
  EDITOR_CMD(TileCoord);
  EDITOR_CMD(ChalkStart);
  EDITOR_CMD(ChalkTo);
  EDITOR_CMD(AutoGoing);
  EDITOR_CMD(AutoSpeed);
  EDITOR_CMD(AutoGoal);
  EDITOR_CMD(SU);
  EDITOR_CMD(ShowMe);
  EDITOR_CMD(Follow);
  EDITOR_CMD(ShowOverlay);
  EDITOR_CMD(OverlayMode);
  EDITOR_CMD(DynamicFilter);
}


int
DoEditorCmd(CLIENT_ARGS)
{
  SimView *view = (SimView *) clientData;
  Tcl_HashEntry *ent;
  int result = TCL_OK;
  int (*cmd)();

  if (argc < 2) {
    return TCL_ERROR;
  }

  if (ent = Tcl_FindHashEntry(&EditorCmds, argv[1])) {
    cmd = (int (*)())ent->clientData;
    Tk_Preserve((ClientData) view);
    result = cmd(view, interp, argc, argv);
    Tk_Release((ClientData) view);
  } else {
    Tcl_AppendResult(interp, "unknown command name: \"",
		     argv[0], " ", argv[1], "\".", (char *) NULL);
    result = TCL_ERROR;
  }
  return result;
}



/*************************************************************************/


DoNewEditor(SimView *view)
{
  sim->editors++; view->next = sim->editor; sim->editor = view;
  view->invalid = 1;
}


DoUpdateEditor(SimView *view)
{
  int dx, dy, i;

  view->updates++;

  if (!view->visible) {
    return;
  }

  if ((!ShakeNow) &&
//      (!view->invalid) &&
      (!view->update) &&
      (sim_skips ||
       view->skips)) {
    if (sim_skips) {
      if (sim_skip > 0) {
	return 0;
      }
    } else {
      if (view->skip > 0) {
	--view->skip;
	return 0;
      } else {
	view->skip = view->skips;
      }
    }
  }

  view->skips = 0;
  view->update = 0;

  HandleAutoGoto(view);

  if (DoAnimation && SimSpeed && !heat_steps && !TilesAnimated) {
    TilesAnimated = 1;
    animateTiles();
  }

  if (view->invalid) {

    switch (view->type) {

    case X_Mem_View:
      MemDrawBeegMapRect(view, view->tile_x, view->tile_y,
			 view->tile_width, view->tile_height);
      break;

    case X_Wire_View:
      WireDrawBeegMapRect(view, view->tile_x, view->tile_y,
			  view->tile_width, view->tile_height);
      break;

    }

    XCopyArea(view->x->dpy, view->pixmap, view->pixmap2, view->x->gc,
	      0, 0, view->screen_width, view->screen_height,
	      view->screen_x, view->screen_y);
    DrawOutside(view);
    if (PendingTool != -1) {
      DrawPending(view);
    }
    DrawObjects(view);
    if (view->show_overlay) {
      DrawOverlay(view);
    }
  }

  for (dx = dy = i = 0; i < ShakeNow; i++) {
    dx += Rand(16) - 8;
    dy += Rand(16) - 8;
  }

  XCopyArea(view->x->dpy, view->pixmap2,
	    Tk_WindowId(view->tkwin), view->x->gc,
	    0, 0, view->w_width, view->w_height, dx, dy);

  DrawCursor(view);

  view->invalid = 0;
}


HandleAutoGoto(SimView *view)
{
  if (view->follow != NULL) {
    int x = view->follow->x + view->follow->x_hot,
        y = view->follow->y + view->follow->y_hot;

    if ((x != view->pan_x) ||
	(y != view->pan_y)) {
      DoPanTo(view, x, y);
    }
  } else if (view->auto_goto &&
	     view->auto_going &&
	     (view->tool_mode == 0)) {
    int dx, dy;
    int panx, pany, speed;
    double dist, sloth;

    speed = view->auto_speed;

    if (view->auto_going < 5) {
      sloth = ((double)view->auto_going) / 5.0;
    } else {
      sloth = 1.0;
    }

    dx = view->auto_x_goal - view->pan_x;
    dy = view->auto_y_goal - view->pan_y;

    dist = sqrt((double)((dx * dx) + (dy * dy)));

    if (dist < (speed * sloth)) {
      view->auto_going = 0;
      if (view->auto_goto == -1)
	view->auto_goto = 0;
      DoPanTo(view, view->auto_x_goal, view->auto_y_goal);
      NewMap = 1;
      DidStopPan(view);
    } else {
      double atan2(), cos(), sin();
      double direction, vx, vy;
      double co, si;

      direction = (double)atan2((double)dy, (double)dx);
      co = (double)cos(direction);
      si = (double)sin(direction);
      vx = co * (double)speed;
      vy = si * (double)speed;

      vx *= sloth; vy *= sloth;
      speed *= sloth;

      vx += 0.5; vy += 0.5;

      DoPanBy(view, (int)(vx), (int)(vy));
      view->auto_going++;
    }
  }
}

DrawOutside(SimView *view)
{
  Pixmap pm = view->pixmap2;
  int left = (view->w_width / 2) - view->pan_x;
  int right = left + view->i_width;
  int top = (view->w_height / 2) - view->pan_y;
  int bottom = top + view->i_height;

  if ((top > 0) || (bottom < view->w_height) ||
      (left > 0) || (right < view->w_width)) {
    if (view->x->color) {
      XSetForeground(view->x->dpy, view->x->gc,
		     view->pixels[COLOR_BLACK]);
    } else {
      XSetForeground(view->x->dpy, view->x->gc,
		     view->pixels[COLOR_WHITE]);
    }

    if (top > 0)
      XFillRectangle(view->x->dpy, pm, view->x->gc,
		     0, 0, view->w_width, top);
    if (bottom < view->w_height)
      XFillRectangle(view->x->dpy, pm, view->x->gc,
		     0, bottom, view->w_width,
		     view->w_height - bottom);
    if (left > 0)
      XFillRectangle(view->x->dpy, pm, view->x->gc,
		     0, top, left, bottom - top);
    if (right < view->w_width)
      XFillRectangle(view->x->dpy, pm, view->x->gc,
		     right, top, view->w_width - right, bottom - top);
  }
}


char CursorDashes[] = { 4, 4 };

DrawPending(SimView *view)
{
  Pixmap pm = view->pixmap2;
  int left = (view->w_width / 2) - view->pan_x;
  int top = (view->w_height / 2) - view->pan_y;
  int x, y, size;
  char *iconname = NULL;

  x = (PendingX - toolOffset[PendingTool]) <<4;
  y = (PendingY - toolOffset[PendingTool]) <<4;
  size = toolSize[PendingTool] <<4;
  x += left; y += top;

  XSetStipple(view->x->dpy, view->x->gc, view->x->gray50_stipple);
  XSetTSOrigin(view->x->dpy, view->x->gc, 0, 0);
  XSetForeground(view->x->dpy, view->x->gc, view->x->pixels[COLOR_BLACK]);
  XSetFillStyle(view->x->dpy, view->x->gc, FillStippled);
  XFillRectangle(view->x->dpy, pm, view->x->gc,
		 x, y, size, size);
  XSetFillStyle(view->x->dpy, view->x->gc, FillSolid);

  switch (PendingTool) {
  case residentialState:
    iconname = "@images/res.xpm"; break;
  case commercialState:
    iconname = "@images/com.xpm"; break;
  case industrialState:
    iconname = "@images/ind.xpm"; break;
  case fireState:
    iconname = "@images/fire.xpm"; break;
  case policeState:
    iconname = "@images/police.xpm"; break;
  case stadiumState:
    iconname = "@images/stadium.xpm"; break;
  case seaportState:
    iconname = "@images/seaport.xpm"; break;
  case powerState:
    iconname = "@images/coal.xpm"; break;
  case nuclearState:
    iconname = "@images/nuclear.xpm"; break;
  case airportState:
    iconname = "@images/airport.xpm"; break;
  default:
    break;
  }

  if (iconname != NULL) {
    Pixmap icon = Tk_GetPixmap(view->interp, view->tkwin, iconname);
    float f;
    int i;

    gettimeofday(&now_time, NULL);
    f = (2 * now_time.tv_usec / 1000000.0);
    if (f > 1.0) f = 2.0 - f;
    i = (int)(f * BobHeight * (Players - Votes));

    if (icon != None) {
      XCopyArea(view->x->dpy, icon, pm, view->x->gc, 
		0, 0, size, size, x + i, y - i);
    }
  }
}


DrawCursor(SimView *view)
{
  Pixmap pm = Tk_WindowId(view->tkwin);
  int left = (view->w_width / 2) - view->pan_x;
  int top = (view->w_height / 2) - view->pan_y;
  int x, y, mode, size, offset, fg, bg, light, dark;
  SimView *v;

  for (v = sim->editor; v != NULL; v = v->next) {
    mode = v->tool_mode;
    if ((v->show_me != 0) &&
	((mode == -1) || v->tool_showing)) {
      x = v->tool_x; y = v->tool_y;
      if (mode == -1) { /* pan cursor */

	x += left; y += top;

	XSetLineAttributes(view->x->dpy, view->x->gc, 3,
			   LineSolid, CapRound, JoinMiter);
	XSetForeground(view->x->dpy, view->x->gc,
		       view->pixels[COLOR_BLACK]);
	XDrawLine(view->x->dpy, pm, view->x->gc,
		  x - 6, y - 6, x + 6, y + 6);
	XDrawLine(view->x->dpy, pm, view->x->gc,
		  x - 6, y + 6, x + 6, y - 6);
	XDrawLine(view->x->dpy, pm, view->x->gc,
		  x - 8, y, x + 8, y);
	XDrawLine(view->x->dpy, pm, view->x->gc,
		  x, y + 8, x, y - 8);
	XSetLineAttributes(view->x->dpy, view->x->gc, 1,
			   LineSolid, CapRound, JoinMiter);
	XSetForeground(view->x->dpy, view->x->gc,
		       view->pixels[COLOR_WHITE]);
	XDrawLine(view->x->dpy, pm, view->x->gc,
		  x - 6, y - 6, x + 6, y + 6);
	XDrawLine(view->x->dpy, pm, view->x->gc,
		  x - 6, y + 6, x + 6, y - 6);
	XDrawLine(view->x->dpy, pm, view->x->gc,
		  x - 8, y, x + 8, y);
	XDrawLine(view->x->dpy, pm, view->x->gc,
		  x, y + 8, x, y - 8);
	XSetLineAttributes(view->x->dpy, view->x->gc, 1,
			   LineSolid, CapButt, JoinMiter);

      } else { /* edit cursor */

	size = toolSize[v->tool_state];
	fg = toolColors[v->tool_state] & 0xff;
	light = COLOR_WHITE;
	dark = COLOR_BLACK;
	if (mode == 1) {
	  int temp = dark;
	  dark = light;
	  light = temp;
	}
	switch (v->tool_state) {

	case chalkState:
	  x += left; y += top;
	  if (mode == 1) {
	    offset = 2;
	  } else {
	    offset = 0;

	    if (view->x->color) {
	      XSetForeground(view->x->dpy, view->x->gc,
			     view->pixels[COLOR_MEDIUMGRAY]);
	      XFillArc(view->x->dpy, pm, view->x->gc,
		       x - 8, y + 7, 7, 7, 0, 360 * 64);
	    } else {
	      XSetStipple(view->x->dpy, view->x->gc,
			  view->x->gray50_stipple);
	      XSetTSOrigin(view->x->dpy, view->x->gc, 0, 0);
	      XSetForeground(view->x->dpy, view->x->gc,
			     view->x->pixels[COLOR_BLACK]);
	      XSetBackground(view->x->dpy, view->x->gc,
			     view->x->pixels[COLOR_WHITE]);
	      XSetFillStyle(view->x->dpy, view->x->gc, FillOpaqueStippled);
	      XFillArc(view->x->dpy, pm, view->x->gc,
		       x - 8, y + 7, 7, 7, 0, 360 * 64);
	      XSetFillStyle(view->x->dpy, view->x->gc, FillSolid);
	    }
	  }

	  if (view->x->color) {
	    XSetLineAttributes(view->x->dpy, view->x->gc, 3,
			       LineSolid, CapRound, JoinMiter);
	    XSetForeground(view->x->dpy, view->x->gc,
			   view->pixels[COLOR_LIGHTGRAY]);
	    XFillArc(view->x->dpy, pm, view->x->gc,
		     x - 6 - offset, y + 5 + offset, 7, 7, 0, 360 * 64);
	    XDrawLine(view->x->dpy, pm, view->x->gc,
		      x + 13 - offset, y - 5 + offset,
		      x - 1 - offset, y + 9 + offset);
	    XSetForeground(view->x->dpy, view->x->gc,
			   view->pixels[COLOR_WHITE]);
	    XDrawLine(view->x->dpy, pm, view->x->gc,
		      x + 11 - offset, y - 7 + offset,
		      x - 3 - offset, y + 7 + offset);
	    XFillArc(view->x->dpy, pm, view->x->gc,
		     x + 8 - offset, y - 9 + offset, 7, 7, 0, 360 * 64);
	    XSetLineAttributes(view->x->dpy, view->x->gc, 1,
			       LineSolid, CapButt, JoinMiter);
	  } else {
	    XSetLineAttributes(view->x->dpy, view->x->gc, 3,
			       LineSolid, CapRound, JoinMiter);
	    XSetStipple(view->x->dpy, view->x->gc,
			view->x->gray25_stipple);
	    XSetTSOrigin(view->x->dpy, view->x->gc, 0, 0);
	    XSetForeground(view->x->dpy, view->x->gc,
			   view->x->pixels[COLOR_BLACK]);
	    XSetBackground(view->x->dpy, view->x->gc,
			   view->x->pixels[COLOR_WHITE]);
	    XSetFillStyle(view->x->dpy, view->x->gc, FillOpaqueStippled);
	    XFillArc(view->x->dpy, pm, view->x->gc,
		     x - 6 - offset, y + 5 + offset, 7, 7, 0, 360 * 64);
	    XDrawLine(view->x->dpy, pm, view->x->gc,
		      x + 13 - offset, y - 5 + offset,
		      x - 1 - offset, y + 9 + offset);
	    XSetStipple(view->x->dpy, view->x->gc,
			view->x->gray75_stipple);
	    XDrawLine(view->x->dpy, pm, view->x->gc,
		      x + 11 - offset, y - 7 + offset,
		      x - 3 - offset, y + 7 + offset);
	    XFillArc(view->x->dpy, pm, view->x->gc,
		     x + 8 - offset, y - 9 + offset, 7, 7, 0, 360 * 64);
	    XSetLineAttributes(view->x->dpy, view->x->gc, 1,
			       LineSolid, CapButt, JoinMiter);
	    XSetFillStyle(view->x->dpy, view->x->gc, FillSolid);
	  }

	  break;

	case eraserState:
	  x += left; y += top;
	  if (mode == 1) {
	    offset = 0;
	  } else {
	    offset = 2;

	    if (view->x->color) {
	      XSetForeground(view->x->dpy, view->x->gc,
			     view->pixels[COLOR_MEDIUMGRAY]);
	      XFillRectangle(view->x->dpy, pm, view->x->gc,
		       x - 8, y - 8, 16, 16);
	    } else {
	      XSetStipple(view->x->dpy, view->x->gc,
			  view->x->gray50_stipple);
	      XSetTSOrigin(view->x->dpy, view->x->gc, 0, 0);
	      XSetForeground(view->x->dpy, view->x->gc,
			     view->x->pixels[COLOR_BLACK]);
	      XSetBackground(view->x->dpy, view->x->gc,
			     view->x->pixels[COLOR_WHITE]);
	      XSetFillStyle(view->x->dpy, view->x->gc, FillOpaqueStippled);
	      XFillRectangle(view->x->dpy, pm, view->x->gc,
		       x - 8, y - 8, 16, 16);
	      XSetFillStyle(view->x->dpy, view->x->gc, FillSolid);
	    }
	  }

	  if (view->x->color) {
	    XSetForeground(view->x->dpy, view->x->gc,
			   view->pixels[COLOR_LIGHTGRAY]);
	  } else {
	    XSetStipple(view->x->dpy, view->x->gc,
			view->x->gray75_stipple);
	    XSetTSOrigin(view->x->dpy, view->x->gc, 0, 0);
	    XSetForeground(view->x->dpy, view->x->gc,
			   view->x->pixels[COLOR_BLACK]);
	    XSetBackground(view->x->dpy, view->x->gc,
			   view->x->pixels[COLOR_WHITE]);
	    XSetFillStyle(view->x->dpy, view->x->gc, FillOpaqueStippled);
	  }

	  /* top */
	  XDrawLine(view->x->dpy, pm, view->x->gc,
		    x - 8 + offset, y - 8 - offset,
		    x + 8 + offset, y - 8 - offset);
	  XDrawLine(view->x->dpy, pm, view->x->gc,
		    x - 7 + offset, y - 7 - offset,
		    x + 7 + offset, y - 7 - offset);
	  XDrawLine(view->x->dpy, pm, view->x->gc,
		    x - 6 + offset, y - 6 - offset,
		    x + 6 + offset, y - 6 - offset);

	  /* left */
	  XDrawLine(view->x->dpy, pm, view->x->gc,
		    x - 8 + offset, y - 8 - offset,
		    x - 8 + offset, y + 8 - offset);
	  XDrawLine(view->x->dpy, pm, view->x->gc,
		    x - 7 + offset, y - 7 - offset,
		    x - 7 + offset, y + 7 - offset);
	  XDrawLine(view->x->dpy, pm, view->x->gc,
		    x - 6 + offset, y - 6 - offset,
		    x - 6 + offset, y + 6 - offset);

	  if (view->x->color) {
	    XSetForeground(view->x->dpy, view->x->gc,
			   view->pixels[COLOR_BLACK]);
	  } else {
	    XSetFillStyle(view->x->dpy, view->x->gc, FillSolid);
	  }

	  /* bottom */
	  XDrawLine(view->x->dpy, pm, view->x->gc,
		    x - 7 + offset, y + 7 - offset,
		    x + 8 + offset, y + 7 - offset);
	  XDrawLine(view->x->dpy, pm, view->x->gc,
		    x - 6 + offset, y + 6 - offset,
		    x + 7 + offset, y + 6 - offset);
	  XDrawLine(view->x->dpy, pm, view->x->gc,
		    x - 5 + offset, y + 5 - offset,
		    x + 6 + offset, y + 5 - offset);

	  /* right */
	  XDrawLine(view->x->dpy, pm, view->x->gc,
		    x + 7 + offset, y + 8 - offset,
		    x + 7 + offset, y - 7 - offset);
	  XDrawLine(view->x->dpy, pm, view->x->gc,
		    x + 6 + offset, y + 7 - offset,
		    x + 6 + offset, y - 6 - offset);
	  XDrawLine(view->x->dpy, pm, view->x->gc,
		    x + 5 + offset, y + 6 - offset,
		    x + 5 + offset, y - 5 - offset);

	  if (view->x->color) {
	    XSetForeground(view->x->dpy, view->x->gc,
			   view->pixels[COLOR_DARKGRAY]);
	    XFillRectangle(view->x->dpy, pm, view->x->gc,
			   x - 5 + offset, y - 5 - offset, 10, 10);
	  } else {
	    XSetStipple(view->x->dpy, view->x->gc,
			view->x->gray50_stipple);
	    XSetForeground(view->x->dpy, view->x->gc,
			   view->x->pixels[COLOR_BLACK]);
	    XSetBackground(view->x->dpy, view->x->gc,
			   view->x->pixels[COLOR_WHITE]);
	    XSetFillStyle(view->x->dpy, view->x->gc, FillOpaqueStippled);
	    XFillRectangle(view->x->dpy, pm, view->x->gc,
			   x - 5 + offset, y - 5 - offset, 10, 10);
	    XSetFillStyle(view->x->dpy, view->x->gc, FillSolid);
	  }

	  break;

	default:
	  offset = toolOffset[v->tool_state];

	  bg = (toolColors[v->tool_state] >> 8) & 0xff;

	  x = (x & ~15) - (offset <<4);
	  y = (y & ~15) - (offset <<4);
	  size <<= 4;
	  x += left; y += top;

	  XSetForeground(view->x->dpy, view->x->gc,
			 view->pixels[dark]);
	  XDrawRectangle(view->x->dpy, pm, view->x->gc,
			 x - 1, y - 1, size + 4, size + 4);
	  XDrawLine(view->x->dpy, pm, view->x->gc,
		    x - 3, y + size + 3,
		    x - 1, y + size + 3);
	  XDrawLine(view->x->dpy, pm, view->x->gc,
		    x + size + 3, y - 3,
		    x + size + 3, y - 1);

	  XSetForeground(view->x->dpy, view->x->gc,
			 view->pixels[light]);
	  XDrawRectangle(view->x->dpy, pm, view->x->gc,
			 x - 4, y - 4, size + 4, size + 4);
	  XDrawLine(view->x->dpy, pm, view->x->gc,
		    x - 4, y + size + 1,
		    x - 4, y + size + 3);
	  XDrawLine(view->x->dpy, pm, view->x->gc,
		    x + size + 1, y - 4,
		    x + size + 3, y - 4);

	  if (view->x->color) {
	    if (fg == bg) {
	      XSetForeground(view->x->dpy, view->x->gc,
			     view->x->pixels[fg]);
	      XSetLineAttributes(view->x->dpy, view->x->gc, 2,
				 LineSolid, CapButt, JoinMiter);
	    } else {
	      XSetForeground(view->x->dpy, view->x->gc,
			     view->x->pixels[fg]);
	      XSetBackground(view->x->dpy, view->x->gc,
			     view->pixels[bg]);

	      XSetLineAttributes(view->x->dpy, view->x->gc, 2,
				 LineDoubleDash, CapButt, JoinMiter);
	      XSetDashes(view->x->dpy, view->x->gc, 0, CursorDashes, 2);
	    }
	  } else {
	    XSetStipple(view->x->dpy, view->x->gc,
			view->x->gray50_stipple);
	    XSetForeground(view->x->dpy, view->x->gc,
			   view->x->pixels[COLOR_BLACK]);
	    XSetBackground(view->x->dpy, view->x->gc,
			   view->x->pixels[COLOR_WHITE]);
	    XSetFillStyle(view->x->dpy, view->x->gc, FillOpaqueStippled);
	    XSetLineAttributes(view->x->dpy, view->x->gc, 2,
			       LineSolid, CapButt, JoinMiter);
	  }

	  XDrawLine(view->x->dpy, pm, view->x->gc,
		    x - 2, y - 1, x - 2, y + size + 3);
	  XDrawLine(view->x->dpy, pm, view->x->gc,
		    x - 1, y + size + 2, x + size + 3, y + size + 2);
	  XDrawLine(view->x->dpy, pm, view->x->gc,
		    x + size + 2, y + size + 1, x + size + 2, y - 3);
	  XDrawLine(view->x->dpy, pm, view->x->gc,
		    x + size + 1, y - 2, x - 3, y - 2);

	  if (!view->x->color) {
	    XSetFillStyle(view->x->dpy, view->x->gc, FillSolid);
	  }
	  XSetLineAttributes(view->x->dpy, view->x->gc, 1,
			     LineSolid, CapButt, JoinMiter);
	  
	  break;
	}
      }
    }
  }
}


TimeElapsed(struct timeval *elapsed,
	    struct timeval *start,
	    struct timeval *finish)
{
  int usec = finish->tv_usec - start->tv_usec;
  int sec = finish->tv_sec - start->tv_sec;

  while (usec < 0) {
    usec += 1000000;
    sec--;
  }
  elapsed->tv_usec = usec;
  elapsed->tv_sec = sec;
}



DrawOverlay(SimView *view)
{
  int width = view->w_width;
  int height = view->w_height;
  int left = view->pan_x - (width / 2);
  int top = view->pan_y - (height / 2);
  int right = left + width;
  int bottom = top + height;
  int showing = 0;
  Ink *ink;
  struct timeval start, finished, elapsed;

  for (ink = sim->overlay; ink != NULL; ink = ink->next) {
    if ((ink->bottom >= top) && (ink->top <= bottom) &&
	(ink->right >= left) && (ink->left <= right)) {
      showing = 1;
      break;
    }
  }

  if (!showing) return;

/* overlay_mode state machine:
   0 => overlay invalid: 
        draw lines to pm => 1
   1 => overlay stable: 
        sync, time draw lines to pm => 2
   2 => overlay stable: 
        draw lines to ol,
        sync, time clip ol to pm,
        lines faster? => 3,
	clipping faster? => 4
   3 => lines faster: 
        draw lines to pm => 3
   4 => clipping faster: 
        clip ol to pm => 4
*/   

  switch (view->overlay_mode) {
  case 0:
    DrawTheOverlay(view, view->x->gc,
		   view->pixmap2, view->pixels[COLOR_WHITE],
		   top, bottom, left, right, 0);
    view->overlay_mode = 1;
    break;
  case 1:
    XSync(view->x->dpy, False);
    gettimeofday(&start, NULL);
    DrawTheOverlay(view, view->x->gc,
		   view->pixmap2, view->pixels[COLOR_WHITE],
		   top, bottom, left, right, 0);
    XSync(view->x->dpy, False);
    gettimeofday(&finished, NULL);
    TimeElapsed(&view->overlay_time, &start, &finished);
    view->overlay_mode = 2;
    break;
  case 2:
    XSetForeground(view->x->dpy, view->x->overlay_gc, 0);
    XFillRectangle(view->x->dpy, view->overlay_pixmap, view->x->overlay_gc,
		   0, 0, view->m_width, view->m_height);
    DrawTheOverlay(view, view->x->overlay_gc,
		   view->overlay_pixmap, 1,
		   top, bottom, left, right, 1);
    XSync(view->x->dpy, False);
    gettimeofday(&start, NULL);
    ClipTheOverlay(view);
    XSync(view->x->dpy, False);
    gettimeofday(&finished, NULL);
    TimeElapsed(&elapsed, &start, &finished);
    if ((elapsed.tv_sec > view->overlay_time.tv_sec) ||
	((elapsed.tv_sec == view->overlay_time.tv_sec) &&
	 ((elapsed.tv_usec > view->overlay_time.tv_usec)))) {
      view->overlay_mode = 3;
    } else {
      view->overlay_mode = 4;
    }
    break;
  case 3:
    DrawTheOverlay(view, view->x->gc,
		   view->pixmap2, view->pixels[COLOR_WHITE],
		   top, bottom, left, right, 0);
    break;
  case 4:
    ClipTheOverlay(view);
    break;
  }
}


DrawTheOverlay(SimView *view, GC gc, Pixmap pm, int color, 
	       int top, int bottom, int left, int right,
	       int onoverlay)
{
  Ink *ink;

  if (view->x->color) {
    XSetForeground(view->x->dpy, gc, color);
    XSetLineAttributes(view->x->dpy, gc, 3,
		       LineSolid, CapButt, JoinBevel);
  } else {
    if (!onoverlay) {
      XSetStipple(view->x->dpy, gc, view->x->gray50_stipple);
      XSetTSOrigin(view->x->dpy, gc, view->updates & 1, 0);
      XSetBackground(view->x->dpy, gc, 0);
      XSetFillStyle(view->x->dpy, gc, FillOpaqueStippled);
    }
    XSetForeground(view->x->dpy, gc, 1);
    XSetLineAttributes(view->x->dpy, gc, 3,
		       LineSolid, CapButt, JoinBevel);
  }
  for (ink = sim->overlay; ink != NULL; ink = ink->next) {
    if ((ink->bottom >= top) && (ink->top <= bottom) &&
	(ink->right >= left) && (ink->left <= right)) {
      if (ink->length <= 1) {
	XFillArc(view->x->dpy, pm, gc,
		 ink->x - 3, ink->y - 3, 6, 6, 0, 360 * 64);
      } else {
	ink->points[0].x = ink->x - left;
	ink->points[0].y = ink->y - top;
	XDrawLines(view->x->dpy, pm, gc,
		   ink->points, ink->length, CoordModePrevious);
      }
    }
  }
  if (!view->x->color) {
    XSetFillStyle(view->x->dpy, gc, FillSolid);
  }
  XSetLineAttributes(view->x->dpy, gc, 1,
		     LineSolid, CapButt, JoinMiter);
}


ClipTheOverlay(SimView *view)
{
  if (view->x->color) {
    XSetForeground(view->x->dpy, view->x->gc, view->pixels[COLOR_WHITE]);
    XSetFillStyle(view->x->dpy, view->x->gc, FillStippled);
    XSetStipple(view->x->dpy, view->x->gc, view->overlay_pixmap);
    XSetTSOrigin(view->x->dpy, view->x->gc, 0, 0);
    XFillRectangle(view->x->dpy, view->pixmap2, view->x->gc,
		   0, 0, view->w_width, view->w_height);
    XSetFillStyle(view->x->dpy, view->x->gc, FillSolid);
  } else {
    XSetStipple(view->x->dpy, view->x->gc, view->x->gray50_stipple);
    XSetTSOrigin(view->x->dpy, view->x->gc, view->updates & 1, 0);
    XSetForeground(view->x->dpy, view->x->gc, view->pixels[COLOR_WHITE]);
    XSetBackground(view->x->dpy, view->x->gc, view->pixels[COLOR_BLACK]);
    XSetFillStyle(view->x->dpy, view->x->gc, FillOpaqueStippled);
    XSetLineAttributes(view->x->dpy, view->x->gc, 3,
		       LineSolid, CapButt, JoinBevel);
    XSetClipOrigin(view->x->dpy, view->x->gc, 0, 0);
    XSetClipMask(view->x->dpy, view->x->gc, view->overlay_pixmap);
    XFillRectangle(view->x->dpy, view->pixmap2, view->x->gc,
		   0, 0, view->w_width, view->w_height);
    XSetFillStyle(view->x->dpy, view->x->gc, FillSolid);
    XSetClipMask(view->x->dpy, view->x->gc, None);
  }
}
