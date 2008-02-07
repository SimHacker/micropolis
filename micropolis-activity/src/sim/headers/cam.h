/* cam.h
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

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <signal.h>
#ifndef HPUX
#include <stropts.h>
#endif
#include <time.h>
#include <errno.h>
#ifndef IS_LINUX
#include <sys/conf.h>
#endif
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/extensions/XShm.h>


#define CAM_ARGS \
	SimCam *scam, ARGS


struct Can {
  Byte *mem;
  int line_bytes;
  int width;
  int height;
};

typedef struct Can Can;


struct Cam {
  struct Cam *next;
  Can *back, *front;
  void (*neighborhood)();
  Byte *rule;
  int rule_size;
  int width;
  int height;
  int ideal_width;
  int ideal_height;
  int phase;
  int wrap;
  int steps;
  int frob;
  int x;
  int y;
  int dx;
  int dy;
  int gx;
  int gy;
  int dragging;
  int set_x;
  int set_y;
  int set_width;
  int set_height;
  int set_x0;
  int set_y0;
  int set_x1;
  int set_y1;
  char *name;
};


typedef struct Cam Cam;


struct SimCam {
  struct SimCam *next;
  int w_x, w_y, w_width, w_height;
  int visible;
  int invalid;
  int skips;
  int skip;
  Tk_Window tkwin;
  Tcl_Interp *interp;
  int flags;
  XDisplay *x;
  XImage *image;
  XShmSegmentInfo *shminfo;
  int line_bytes;
  u_char *data;
  int cam_count;
  Cam *cam_list;
};


typedef struct SimCam SimCam;


#define NORTHWEST	((Byte)((l0>>16) & 0xff))
#define NORTH		((Byte)((l0>>8) & 0xff))
#define NORTHEAST	((Byte)(l0 & 0xff))
#define WEST		((Byte)((l1>>16) & 0xff))
#define CENTER		((Byte)((l1>>8) & 0xff))
#define EAST		((Byte)(l1 & 0xff))
#define SOUTHWEST	((Byte)((l2>>16) & 0xff))
#define SOUTH		((Byte)((l2>>8) & 0xff))
#define SOUTHEAST	((Byte)(l2 & 0xff))

#define SUM8		((l0&1) + ((l0>>8)&1) + ((l0>>16)&1) + \
			 (l1&1) + 	        ((l1>>16)&1) + \
			 (l2&1) + ((l2>>8)&1) + ((l2>>16)&1))

#define SUM9		(SUM8 + ((l1>>8)&1))

#define SUM8p(p)	(((l0>>p)&1) + ((l0>>(p+8))&1) + ((l0>>(p+16))&1) + \
			 ((l1>>p)&1) + 			 ((l1>>(p+16))&1) + \
			 ((l2>>p)&1) + ((l2>>(p+8))&1) + ((l2>>(p+16))&1))

#define SUM9p(p)	(SUM8p(p) + ((l1>>(p+8))&1))

#define CAM_LOOP_BODY(BODY) \
      { int y; \
	int backline = cam->back->line_bytes, \
	    frontline = cam->front->line_bytes; \
	Byte *back = cam->back->mem, \
	       *front = cam->front->mem; \
	Byte *rule = cam->rule; \
	for (y=0; y<cam->height; y++) { \
	  QUAD l0 = (back[0]<<8) + \
		    back[1], \
	       l1 = (back[backline]<<8) + \
		    back[backline+1], \
	       l2 = (back[backline+backline]<<8) + \
		    back[backline+backline+1]; \
	  int x, w = cam->width; \
	  for (x=0; x<w; x++) { \
	    l0 = (l0<<8) + back[2]; \
	    l1 = (l1<<8) + back[backline+2]; \
	    l2 = (l2<<8) + back[backline+backline+2]; \
	    BODY; \
	    back++; front++; \
	  } \
	  back += backline - w; front += frontline - w; \
	} \
      }

#define CAM_LOOP(RULE) \
	CAM_LOOP_BODY(*front = (RULE) )

#define CAM_TABLE_LOOP(NEIGHBORHOOD) \
	  CAM_LOOP(rule[(NEIGHBORHOOD)])


typedef void (*vf)();

