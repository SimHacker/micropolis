/* w_cam.c
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


Tk_ConfigSpec SimCamConfigSpecs[] = {
    {TK_CONFIG_PIXELS, "-width", "width", "Width",
	0, Tk_Offset(SimCam, w_width), 0},
    {TK_CONFIG_PIXELS, "-height", "height", "Height",
	0, Tk_Offset(SimCam, w_height), 0},
    {TK_CONFIG_END, (char *) NULL, (char *) NULL, (char *) NULL,
	(char *) NULL, 0, 0}
};


Tk_ConfigSpec CamConfigSpecs[] = {
    {TK_CONFIG_PIXELS, "-wrap", "wrap", "Wrap",
	0, Tk_Offset(Cam, wrap), 0},
    {TK_CONFIG_PIXELS, "-steps", "steps", "Steps",
	0, Tk_Offset(Cam, steps), 0},
    {TK_CONFIG_PIXELS, "-frob", "frob", "Frob",
	0, Tk_Offset(Cam, frob), 0},
    {TK_CONFIG_PIXELS, "-x", "x", "X",
	0, Tk_Offset(Cam, x), 0},
    {TK_CONFIG_PIXELS, "-y", "y", "Y",
	0, Tk_Offset(Cam, y), 0},
    {TK_CONFIG_PIXELS, "-width", "width", "Width",
	0, Tk_Offset(Cam, width), 0},
    {TK_CONFIG_PIXELS, "-height", "height", "Height",
	0, Tk_Offset(Cam, height), 0},
    {TK_CONFIG_PIXELS, "-dx", "dx", "Dx",
	0, Tk_Offset(Cam, dx), 0},
    {TK_CONFIG_PIXELS, "-dy", "dy", "Dy",
	0, Tk_Offset(Cam, dy), 0},
    {TK_CONFIG_PIXELS, "-gx", "gx", "Gx",
	0, Tk_Offset(Cam, gx), 0},
    {TK_CONFIG_PIXELS, "-gy", "gy", "Gy",
	0, Tk_Offset(Cam, gy), 0},
    {TK_CONFIG_PIXELS, "-dragging", "dragging", "Dragging",
	0, Tk_Offset(Cam, dragging), 0},
    {TK_CONFIG_PIXELS, "-setx", "setx", "SetX",
	"-1", Tk_Offset(Cam, set_x), 0},
    {TK_CONFIG_PIXELS, "-sety", "sety", "SetY",
	"-1", Tk_Offset(Cam, set_y), 0},
    {TK_CONFIG_PIXELS, "-setwidth", "setwidth", "SetWidth",
	"-1", Tk_Offset(Cam, set_width), 0},
    {TK_CONFIG_PIXELS, "-setheight", "setheight", "SetHeight",
	"-1", Tk_Offset(Cam, set_height), 0},
    {TK_CONFIG_PIXELS, "-setx0", "setx0", "SetX0",
	"-1", Tk_Offset(Cam, set_x0), 0},
    {TK_CONFIG_PIXELS, "-sety0", "sety0", "SetY0",
	"-1", Tk_Offset(Cam, set_y0), 0},
    {TK_CONFIG_PIXELS, "-setx1", "setx1", "SetX1",
	"-1", Tk_Offset(Cam, set_x1), 0},
    {TK_CONFIG_PIXELS, "-sety1", "sety1", "SetY1",
	"-1", Tk_Offset(Cam, set_y1), 0},
    {TK_CONFIG_END, (char *) NULL, (char *) NULL, (char *) NULL,
	(char *) NULL, 0, 0}
};


Tcl_HashTable CamCmds;

int next_cam_id = 1;


static void
DisplaySimCam(ClientData clientData)
{
  SimCam *scam = (SimCam *) clientData;
  Tk_Window tkwin = scam->tkwin;
  Pixmap pm = None;
  Drawable d;

  scam->flags &= ~VIEW_REDRAW_PENDING;
  if (scam->visible && (tkwin != NULL) && Tk_IsMapped(tkwin)) {
    DoUpdateCam(scam);
  }
}


void
DestroyCam(SimCam *scam, Cam *cam)
{
  Cam **cp;

  for (cp = &scam->cam_list;
       (*cp) != NULL;
       cp = &((*cp)->next)) {
    if ((*cp) == cam) {
      (*cp) = cam->next;
      scam->cam_count--;
      if (cam->front != NULL) {
	ckfree(cam->front);
      }
      if (cam->back != NULL) {
	if (cam->back->mem != NULL) {
	  ckfree(cam->back->mem);
	}
	ckfree(cam->back);
      }
      if (cam->rule != NULL) {
	ckfree(cam->rule);
      }
      if (cam->name != NULL) {
	ckfree(cam->name);
      }
      ckfree(cam);
      break;
    }
  }
}


void
DestroyScam(ClientData cdata)
{
  SimCam *scam = (SimCam *)cdata;
  SimCam **cp;

  CancelRedrawView(scam);

  while (scam->cam_list) {
    DestroyCam(scam, scam->cam_list);
  }

  for (cp = &sim->scam;
       (*cp) != NULL;
       cp = &((*cp)->next)) {
    if ((*cp) == scam) {
      (*cp) = scam->next;
      sim->scams--;
      break;
    }
  }

  if (scam->shminfo != NULL) {
    XShmDetach(scam->x->dpy, scam->shminfo);
    shmdt(scam->shminfo->shmaddr);
    shmctl(scam->shminfo->shmid, IPC_RMID, 0);
    ckfree(scam->shminfo);
    scam->shminfo = NULL;
    if (scam->image) {
      scam->image->data = NULL;
      scam->data = NULL;
      XDestroyImage(scam->image);
      scam->image = NULL;
    }
  } else {
    if (scam->image) {
      if (scam->image->data) {
	ckfree(scam->image->data);
	scam->image->data = NULL;
      }
      scam->data = NULL;
      XDestroyImage(scam->image);
      scam->image = NULL;
    }
  }

  DecRefDisplay(scam->x);

  ckfree((char *) scam);
}


void
CamEventProc(ClientData clientData, XEvent *eventPtr)
{
  SimCam *scam = (SimCam *) clientData;

  if ((eventPtr->type == Expose) && (eventPtr->xexpose.count == 0)) {
    scam->visible = 1;
    EventuallyRedrawCam(scam);
  } else if (eventPtr->type == MapNotify) {
    scam->visible = 1;
  } else if (eventPtr->type == UnmapNotify) {
    scam->visible = 0;
  } else if (eventPtr->type == VisibilityNotify) {
    if (eventPtr->xvisibility.state == VisibilityFullyObscured)
      scam->visible = 0;
    else
      scam->visible = 1;
  } else if (eventPtr->type == ConfigureNotify) {
    DoResizeCam(scam,
		eventPtr->xconfigure.width,
		eventPtr->xconfigure.height);
    EventuallyRedrawCam(scam);
  } else if (eventPtr->type == DestroyNotify) {
    Tcl_DeleteCommand(scam->interp, Tk_PathName(scam->tkwin));
    scam->tkwin = NULL;
    if (scam->flags & VIEW_REDRAW_PENDING) {
      Tk_CancelIdleCall(DisplaySimCam, (ClientData) scam);
    }
    Tk_EventuallyFree((ClientData) scam, DestroyScam);
  }
}


int
DoCamCmd(CLIENT_ARGS)
{
  SimCam *scam = (SimCam *) clientData;
  Tcl_HashEntry *ent;
  int result = TCL_OK;
  int (*cmd)();

  if (argc < 2) {
    return TCL_ERROR;
  }

  if (ent = Tcl_FindHashEntry(&CamCmds, argv[1])) {
    cmd = (int (*)())ent->clientData;
    Tk_Preserve((ClientData) scam);
    result = cmd(scam, interp, argc, argv);
    Tk_Release((ClientData) scam);
  } else {
    Tcl_AppendResult(interp, "unknown command name: \"",
		     argv[0], " ", argv[1], "\".", (char *) NULL);
    result = TCL_ERROR;
  }
  return result;
}


int
CamCmd(CLIENT_ARGS)
{
  Tk_Window tkwin = (Tk_Window) clientData;
  SimCam *scam;

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

  scam = (SimCam *)ckalloc(sizeof (SimCam));

  scam->w_x = 0;
  scam->w_y = 0;
  scam->w_width = 0;
  scam->w_height = 0;
  scam->visible = 0;
  scam->invalid = 1;
  scam->skips = 0;
  scam->skip = 0;
  scam->tkwin = tkwin;
  scam->interp = interp;
  scam->flags = 0;
  scam->x = NULL;
  scam->image = NULL;
  scam->shminfo = NULL;
  scam->line_bytes = 0;
  scam->data = NULL;
  scam->cam_count = 0;
  scam->cam_list = NULL;

  Tk_SetClass(scam->tkwin, "Cam");

  Tk_CreateEventHandler(scam->tkwin,
			VisibilityChangeMask |
			ExposureMask |
			StructureNotifyMask |
			EnterWindowMask |
			LeaveWindowMask |
			PointerMotionMask,
			CamEventProc, (ClientData) scam);
  Tcl_CreateCommand(interp, Tk_PathName(scam->tkwin),
		    DoCamCmd, (ClientData) scam, (void (*)()) NULL);

  Tk_MakeWindowExist(scam->tkwin);

  if (getenv("XSYNCHRONIZE") != NULL) {
    XSynchronize(Tk_Display(tkwin), 1);
  }

  InitNewCam(scam);
  DoNewCam(scam);

  if (ConfigureCam(interp, scam, argc-2, argv+2, 0) != TCL_OK) {
    /* XXX: destroy scam */
    Tk_DestroyWindow(scam->tkwin);
    return TCL_ERROR;
  }

  scam->invalid = 1;

  interp->result = Tk_PathName(scam->tkwin);
  return TCL_OK;
}


int
ConfigureCam(Tcl_Interp *interp, SimCam *scam,
	     int argc, char **argv, int flags)
{
  if (Tk_ConfigureWidget(interp, scam->tkwin, SimCamConfigSpecs,
			 argc, argv, (char *) scam, flags) != TCL_OK) {
    return TCL_ERROR;
  }

  if (scam->w_width || scam->w_height) {
    Tk_GeometryRequest(scam->tkwin, scam->w_width, scam->w_height);
  }

  EventuallyRedrawCam(scam);

  return TCL_OK;
}


EventuallyRedrawCam(SimCam *scam)
{
  if (!(scam->flags & VIEW_REDRAW_PENDING)) {
    Tk_DoWhenIdle(DisplaySimCam, (ClientData) scam);
    scam->flags |= VIEW_REDRAW_PENDING;
  }
}


CamCmdconfigure(CAM_ARGS)
{
  int result = TCL_OK;

  if (argc == 2) {
    result = Tk_ConfigureInfo(interp, scam->tkwin, SimCamConfigSpecs,
			      (char *) scam, (char *) NULL, 0);
  } else if (argc == 3) {
    result = Tk_ConfigureInfo(interp, scam->tkwin, SimCamConfigSpecs,
			      (char *) scam, argv[2], 0);
  } else {
    result = ConfigureCam(interp, scam, argc-2, argv+2,
			  TK_CONFIG_ARGV_ONLY);
  }
  return result;
}


CamCmdposition(CAM_ARGS)
{
  if ((argc != 2) && (argc != 4)) {
    return TCL_ERROR;
  }
  if (argc == 4) {
    int x, y;

    if ((Tcl_GetInt(interp, argv[2], &x) != TCL_OK)
	|| (Tcl_GetInt(interp, argv[3], &y) != TCL_OK)) {
      return TCL_ERROR;
    }
    scam->w_x = x;
    scam->w_y = y;
  }
  sprintf(interp->result, "%d %d", scam->w_x, scam->w_y);
  return TCL_OK;
}


CamCmdsize(CAM_ARGS)
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
    scam->w_width = w;
    scam->w_height = h;
  }
  sprintf(interp->result, "%d %d", scam->w_width, scam->w_height);
  return TCL_OK;
}


CamCmdVisible(CAM_ARGS)
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

    visible = visible && Tk_IsMapped(scam->tkwin);
    scam->visible = visible;
  }

  sprintf(interp->result, "%d", scam->visible);
  return TCL_OK;
}


CamCmdStoreColor(CAM_ARGS)
{
  int index, r, g, b, err;
  XColor color;

  if (argc != 6) {
    return TCL_ERROR;
  }

  if ((Tcl_GetInt(interp, argv[2], &index) != TCL_OK) ||
      (Tcl_GetInt(interp, argv[2], &r) != TCL_OK) ||
      (Tcl_GetInt(interp, argv[2], &g) != TCL_OK) ||
      (Tcl_GetInt(interp, argv[2], &b) != TCL_OK)) {
    return TCL_ERROR;
  }

  color.pixel = index;
  color.red = r;
  color.green = g;
  color.blue = b;
  color.flags = DoRed | DoGreen | DoBlue;
  err = XStoreColor(scam->x->dpy, scam->x->colormap, &color);

  sprintf(interp->result, "%d", err);
  return TCL_OK;
}


CamCmdNewCam(CAM_ARGS)
{
  Cam *cam;
  char *name;
  char *rule_name = NULL;
  int rule_number = 0;
  int x, y, w, h;

  if (argc < 8) {
    return TCL_ERROR;
  }

  name = argv[2];

  if ((Tcl_GetInt(interp, argv[3], &rule_number) != TCL_OK) ||
      (rule_number == 0)) {
    rule_name = argv[3];
  }

  if ((Tcl_GetInt(interp, argv[4], &x) != TCL_OK) ||
      (Tcl_GetInt(interp, argv[5], &y) != TCL_OK) ||
      (Tcl_GetInt(interp, argv[6], &w) != TCL_OK) ||
      (Tcl_GetInt(interp, argv[7], &h) != TCL_OK)) {
    return TCL_ERROR;
  }

  cam = (Cam *)find_cam_by_name(scam, name);

  if (cam != NULL) {
    DestroyCam(scam, cam);
  }

  cam = (Cam *)new_cam(scam, x, y, w, h, 0, 0, NULL);
  cam->name = (char *)malloc(strlen(name) + 1);
  strcpy(cam->name, name);

  if (rule_name != NULL) {
    cam_load_rule(cam, rule_name);
  } else {
    cam_set_neighborhood(cam, rule_number);
  }

  return Tk_ConfigureWidget(interp, scam->tkwin, CamConfigSpecs,
			    argc - 8, argv + 8, (char *) cam, 0);
}


CamCmdDeleteCam(CAM_ARGS)
{
  Cam *cam;
  char *name;

  if (argc != 3) {
    return TCL_ERROR;
  }

  name = argv[2];

  cam = (Cam *)find_cam_by_name(scam, name);

  if (cam != NULL) {
    DestroyCam(scam, cam);
  }

  return TCL_OK;
}


CamCmdRandomizeCam(CAM_ARGS)
{
  Cam *cam;
  char *name;

  if (argc != 3) {
    return TCL_ERROR;
  }

  name = argv[2];

  cam = (Cam *)find_cam_by_name(scam, name);

  if (cam != NULL) {
    cam_randomize(cam);
  }

  return TCL_OK;
}


CamCmdConfigCam(CAM_ARGS)
{
  Cam *cam;
  int result = TCL_OK;

  if (argc < 3) {
    return TCL_ERROR;
  }

  cam = (Cam *)find_cam_by_name(scam, argv[2]);

  if (cam == NULL) {
    return TCL_ERROR;
  }

  if (argc == 3) {
    result = Tk_ConfigureInfo(interp, scam->tkwin, CamConfigSpecs,
		    (char *) cam, (char *) NULL, 0);
  } else if (argc == 4) {
    result = Tk_ConfigureInfo(interp, scam->tkwin, CamConfigSpecs,
		    (char *) cam, argv[3], 0);
  } else {
    result = Tk_ConfigureWidget(interp, scam->tkwin, CamConfigSpecs,
				argc - 3, argv + 3, (char *) cam, 0);
  }

  return result;
}


CamCmdFindCam(CAM_ARGS)
{
  Cam *cam;
  int x, y;

  if (argc != 4) {
    return TCL_ERROR;
  }

  if ((Tcl_GetInt(interp, argv[2], &x) != TCL_OK) ||
      (Tcl_GetInt(interp, argv[3], &y) != TCL_OK)) {
    return TCL_ERROR;
  }

  cam = (Cam *)find_cam(scam, x, y);
  sprintf(interp->result, "%s", (cam == NULL) ? "" : cam->name);

  return TCL_OK;
}


CamCmdFindSomeCam(CAM_ARGS)
{
  Cam *cam;
  int x, y;

  if (argc != 4) {
    return TCL_ERROR;
  }

  if ((Tcl_GetInt(interp, argv[2], &x) != TCL_OK) ||
      (Tcl_GetInt(interp, argv[3], &y) != TCL_OK)) {
    return TCL_ERROR;
  }

  cam = (Cam *)find_cam(scam, x, y);
  if (cam == NULL) {
    cam = scam->cam_list;
  }

  sprintf(interp->result, "%s", (cam == NULL) ? "" : cam->name);

  return TCL_OK;
}

/*************************************************************************/

InitNewCam(SimCam *scam)
{
  scam->x = (XDisplay *)FindXDisplay(scam->tkwin);
  IncRefDisplay(scam->x);

  DoResizeCam(scam, 512, 512);

  init_scam(scam);
}


DoResizeCam(SimCam *scam, int w, int h)
{
  Cam *cam;

  for (cam = scam->cam_list; cam != NULL; cam = cam->next) {
    if (cam->front->width > w) w = cam->front->width;
    if (cam->front->height > h) h = cam->front->height;
  }

  scam->w_width = w;
  scam->w_height = h;

  if (scam->shminfo != NULL) {
    XShmDetach(scam->x->dpy, scam->shminfo);
    shmdt(scam->shminfo->shmaddr);
    shmctl(scam->shminfo->shmid, IPC_RMID, 0);
    ckfree(scam->shminfo);
    scam->shminfo = NULL;
    if (scam->image) {
      scam->image->data = NULL;
      scam->data = NULL;
      XDestroyImage(scam->image);
      scam->image = NULL;
    }
  } else {
    if (scam->image) {
      if (scam->image->data) {
	ckfree(scam->image->data);
	scam->image->data = NULL;
      }
      scam->data = NULL;
      XDestroyImage(scam->image);
      scam->image = NULL;
    }
  }

  if (scam->x->shared) {
    scam->shminfo = (XShmSegmentInfo *)ckalloc(sizeof (XShmSegmentInfo));

    scam->image =
      XShmCreateImage(scam->x->dpy, scam->x->visual, scam->x->depth,
			scam->x->color ? ZPixmap : XYBitmap,
			NULL, scam->shminfo,
			scam->w_width, scam->w_height);

    scam->line_bytes = scam->image->bytes_per_line;
    scam->shminfo->readOnly = False;
    scam->shminfo->shmid = shmget(IPC_PRIVATE,
				  (scam->line_bytes *
				   scam->w_height),
				  (IPC_CREAT | 0777));
    if (scam->shminfo->shmid < 0) {
      perror("shmget");
      fprintf(stderr,
	      "Drat, Micropolis can't share memory with X display \"%s\".\n",
	      scam->x->display);
      goto FALL_BACK;
    }

    scam->data = (unsigned char *)shmat(scam->shminfo->shmid, 0, 0);
    scam->image->data = (char *)scam->data;
    if ((int)scam->data == -1) {
      perror("shmat");
      fprintf(stderr,
	      "Drat, Micropolis can't find any memory to share with display \"%s\".\n",
	      scam->x->display);
      goto FALL_BACK;
    }

    scam->shminfo->shmaddr = (char *)scam->data;
    scam->shminfo->readOnly = False;


    { int (*old)();
      int CatchXError();

      GotXError = 0;
      old = XSetErrorHandler(CatchXError);

      if (XShmAttach(scam->x->dpy, scam->shminfo) == 0) {
	fprintf(stderr,
		"Drat, the X display \"%s\" can't access Micropolis's shared memory.\n",
		scam->x->display);
	GotXError = 1;
      }

      XSync(scam->x->dpy, False);

      XSetErrorHandler(old);

      if (GotXError) {
	goto FALL_BACK;
      }
    }
  } else {
    goto SPRING_FORWARD;
  FALL_BACK:
    fprintf(stderr,
	    "Falling back to the X network protocol on display \"%s\"...\n",
	    scam->x->display);
  SPRING_FORWARD:
    scam->x->shared = 0;
    if (scam->shminfo) {
      if (scam->shminfo->shmid >= 0) {
	if (scam->shminfo->shmaddr) {
	  shmdt(scam->shminfo->shmaddr);
	}
	shmctl(scam->shminfo->shmid, IPC_RMID, 0);
      }
      ckfree((char *)scam->shminfo);
      scam->shminfo = NULL;
    }
    if (scam->image) {
      scam->image->data = NULL;
      XDestroyImage(scam->image);
      scam->image = NULL;
    }
    scam->data = NULL;
    scam->line_bytes = scam->w_width;
    scam->data = (Byte *)ckalloc(scam->line_bytes * scam->w_height);
    scam->image = 
      XCreateImage(scam->x->dpy, scam->x->visual,
		   scam->x->depth,
		   scam->x->color ? ZPixmap : XYBitmap,
		   0, (char *)scam->data,
		   scam->w_width, scam->w_height,
		   8, scam->line_bytes); /* XXX: handle other depths */
  }

  for (cam = scam->cam_list; cam != NULL; cam = cam->next) {
    if (cam->x + cam->front->width > scam->w_width) {
      cam->x = scam->w_width - cam->front->width;
    }
    if (cam->y + cam->front->height > scam->w_height) {
      cam->y = scam->w_height - cam->front->height;
    }
    cam->front->line_bytes = scam->line_bytes;
    cam->front->mem = /* XXX: handle other depths */
      (Byte *)scam->data + cam->x + (scam->line_bytes * cam->y);
  }
}


DoNewCam(SimCam *scam)
{
  sim->scams++; scam->next = sim->scam; sim->scam = scam;
  scam->invalid = 1;
}


DoUpdateCam(SimCam *scam)
{
  if (!scam->visible) {
    return;
  }

  if (scam->invalid) {
    scam->invalid = 0;
  }

  if (scam->x->shared) {
    XShmPutImage(scam->x->dpy, Tk_WindowId(scam->tkwin), scam->x->gc,
		 scam->image, 0, 0,
		 0, 0, scam->w_width, scam->w_height,
		 False);
  } else {
    XPutImage(scam->x->dpy, Tk_WindowId(scam->tkwin), scam->x->gc,
		 scam->image, 0, 0,
		 0, 0, scam->w_width, scam->w_height);
  }
}


cam_command_init()
{
  int new;
  extern int TileCamCmd(CLIENT_ARGS);

  Tcl_CreateCommand(tk_mainInterp, "camview", CamCmd,
		    (ClientData)MainWindow, (void (*)()) NULL);

  Tcl_InitHashTable(&CamCmds, TCL_STRING_KEYS);

#define CAM_CMD(name) HASHED_CMD(Cam, name)

  CAM_CMD(configure);
  CAM_CMD(position);
  CAM_CMD(size);
  CAM_CMD(Visible);
  CAM_CMD(StoreColor);
  CAM_CMD(NewCam);
  CAM_CMD(DeleteCam);
  CAM_CMD(RandomizeCam);
  CAM_CMD(ConfigCam);
  CAM_CMD(FindCam);
  CAM_CMD(FindSomeCam);
}


#endif /* CAM */
