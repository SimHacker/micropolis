/* w_sprite.c
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


Tcl_HashTable SpriteCmds;
short CrashX, CrashY;
int absDist;
short Cycle;

SimSprite *GlobalSprites[OBJN];

SimSprite *NewSprite(char *name, int type, int x, int y);


#define TRA_GROOVE_X -39
#define TRA_GROOVE_Y 6
#define BUS_GROOVE_X -39
#define BUS_GROOVE_Y 6

#define SPRITECMD_ACCESS_INT(var) \
  int SpriteCmd##var(SPRITE_ARGS) { \
    int val; \
    if ((argc != 2) && (argc != 3)) return (TCL_ERROR); \
    if (argc == 3) { \
      if (Tcl_GetInt(interp, argv[2], &val) != TCL_OK) return (TCL_ERROR); \
      sprite->var = val; \
    } \
    sprintf(interp->result, "%d", sprite->var); \
    return (TCL_OK); \
  }


#define SPRITECMD_GET_STR(var) \
  int SpriteCmd##var(SPRITE_ARGS) { \
    sprintf(interp->result, "%s", sprite->var); \
    return (TCL_OK); \
  }


int
DoSpriteCmd(CLIENT_ARGS)
{
  SimSprite *sprite = (SimSprite *) clientData;
  Tcl_HashEntry *ent;
  int result = TCL_OK;
  int (*cmd)();

  if (argc < 2) {
    return TCL_ERROR;
  }

  if (ent = Tcl_FindHashEntry(&SpriteCmds, argv[1])) {
    cmd = (int (*)())ent->clientData;
    Tk_Preserve((ClientData) sprite);
    result = cmd(sprite, interp, argc, argv);
    Tk_Release((ClientData) sprite);
  } else {
    Tcl_AppendResult(interp, "unknown command name: \"",
		     argv[0], " ", argv[1], "\".", (char *) NULL);
    result = TCL_ERROR;
  }
  return result;
}


int
SpriteCmd(CLIENT_ARGS)
{
  SimSprite *sprite;
  int type;

  if ((argc != 3) ||
      (Tcl_GetInt(interp, argv[2], &type) != TCL_OK) ||
      (type < 1) || (type >= OBJN)) {
    return TCL_ERROR;
  }

  sprite = NewSprite(argv[1], type, 0, 0);
  sprite->frame = 0;

  Tcl_CreateCommand(interp, sprite->name,
		    DoSpriteCmd, (ClientData) sprite, (void (*)()) NULL);

  interp->result = sprite->name;
  return TCL_OK;
}


SPRITECMD_GET_STR(name)
SPRITECMD_ACCESS_INT(type)
SPRITECMD_ACCESS_INT(frame)
SPRITECMD_ACCESS_INT(x)
SPRITECMD_ACCESS_INT(y)
SPRITECMD_ACCESS_INT(width)
SPRITECMD_ACCESS_INT(height)
SPRITECMD_ACCESS_INT(x_offset)
SPRITECMD_ACCESS_INT(y_offset)
SPRITECMD_ACCESS_INT(x_hot)
SPRITECMD_ACCESS_INT(y_hot)
SPRITECMD_ACCESS_INT(orig_x)
SPRITECMD_ACCESS_INT(orig_y)
SPRITECMD_ACCESS_INT(dest_x)
SPRITECMD_ACCESS_INT(dest_y)
SPRITECMD_ACCESS_INT(count)
SPRITECMD_ACCESS_INT(sound_count)
SPRITECMD_ACCESS_INT(dir)
SPRITECMD_ACCESS_INT(new_dir)
SPRITECMD_ACCESS_INT(step)
SPRITECMD_ACCESS_INT(flag)
SPRITECMD_ACCESS_INT(control)
SPRITECMD_ACCESS_INT(turn)
SPRITECMD_ACCESS_INT(accel)
SPRITECMD_ACCESS_INT(speed)


int SpriteCmdExplode(SPRITE_ARGS)
{
  ExplodeSprite(sprite);
  return TCL_OK;
}


int SpriteCmdInit(SPRITE_ARGS)
{
  int x, y;

  if (argc != 4) {
    return (TCL_ERROR);
  }
  if ((Tcl_GetInt(interp, argv[2], &x) != TCL_OK) ||
      (x < 0) || (x >= (WORLD_X <<4)) ||
      (Tcl_GetInt(interp, argv[3], &y) != TCL_OK) ||
      (y < 0) || (y >= (WORLD_Y <<4))) {
    return (TCL_ERROR);
  }
  InitSprite(sprite, x, y);
  return TCL_OK;
}


sprite_command_init()
{
  int i;

  Tcl_CreateCommand(tk_mainInterp, "sprite", SpriteCmd,
		    (ClientData)NULL, (void (*)()) NULL);

  Tcl_InitHashTable(&SpriteCmds, TCL_STRING_KEYS);

#define SPRITE_CMD(cmd) HASHED_CMD(Sprite, cmd)

  SPRITE_CMD(name);
  SPRITE_CMD(type);
  SPRITE_CMD(frame);
  SPRITE_CMD(x);
  SPRITE_CMD(y);
  SPRITE_CMD(width);
  SPRITE_CMD(height);
  SPRITE_CMD(x_offset);
  SPRITE_CMD(y_offset);
  SPRITE_CMD(x_hot);
  SPRITE_CMD(y_hot);
  SPRITE_CMD(orig_x);
  SPRITE_CMD(orig_y);
  SPRITE_CMD(dest_x);
  SPRITE_CMD(dest_y);
  SPRITE_CMD(count);
  SPRITE_CMD(sound_count);
  SPRITE_CMD(dir);
  SPRITE_CMD(new_dir);
  SPRITE_CMD(step);
  SPRITE_CMD(flag);
  SPRITE_CMD(control);
  SPRITE_CMD(turn);
  SPRITE_CMD(accel);
  SPRITE_CMD(speed);
  SPRITE_CMD(Explode);
  SPRITE_CMD(Init);

  for (i = 0; i < OBJN; i++) {
    GlobalSprites[i] = NULL;
  }
}


SimSprite *FreeSprites = NULL;

SimSprite *
NewSprite(char *name, int type, int x, int y)
{
  SimSprite *sprite;

  if (FreeSprites) {
    sprite = FreeSprites;
    FreeSprites = sprite->next;
  } else {
    sprite = (SimSprite *)ckalloc(sizeof (SimSprite));
  }

  sprite->name = (char *)ckalloc(strlen(name) + 1);
  strcpy(sprite->name, name);
  sprite->type = type;

  InitSprite(sprite, x, y);

  sim->sprites++; sprite->next = sim->sprite; sim->sprite = sprite;

  return sprite;
}


InitSprite(SimSprite *sprite, int x, int y)
{
  sprite->x = x; sprite->y = y;
  sprite->frame = 0;
  sprite->orig_x = sprite->orig_y = 0;
  sprite->dest_x = sprite->dest_y = 0;
  sprite->count = sprite->sound_count = 0;
  sprite->dir = sprite->new_dir = 0;
  sprite->step = sprite->flag = 0;
  sprite->control = -1;
  sprite->turn = 0;
  sprite->accel = 0;
  sprite->speed = 100;

  if (GlobalSprites[sprite->type] == NULL) {
    GlobalSprites[sprite->type] = sprite;
  }

  switch (sprite->type) {

  case TRA:
    sprite->width = sprite->height = 32;
    sprite->x_offset = 32; sprite->y_offset = -16;
    sprite->x_hot = 40; sprite->y_hot = -8;
    sprite->frame = 1;
    sprite->dir = 4;
    break;

  case SHI:
    sprite->width = sprite->height = 48;
    sprite->x_offset = 32; sprite->y_offset = -16;
    sprite->x_hot = 48; sprite->y_hot = 0;
    if (x < (4 <<4)) sprite->frame = 3;
    else if (x >= ((WORLD_X - 4) <<4)) sprite->frame = 7;
    else if (y < (4 <<4)) sprite->frame = 5;
    else if (y >= ((WORLD_Y - 4) <<4)) sprite->frame = 1;
    else sprite->frame = 3;
    sprite->new_dir = sprite->frame;
    sprite->dir = 10;
    sprite->count = 1;
    break;

  case GOD:
    sprite->width = sprite->height = 48;
    sprite->x_offset = 24; sprite->y_offset = 0;
    sprite->x_hot = 40; sprite->y_hot = 16;
    if (x > ((WORLD_X <<4) / 2)) {
      if (y > ((WORLD_Y <<4) / 2)) sprite->frame = 10;
      else sprite->frame = 7;
    } else if (y > ((WORLD_Y <<4) / 2)) sprite->frame = 1;
    else sprite->frame = 4;
    sprite->count = 1000;
    sprite->dest_x = PolMaxX <<4;
    sprite->dest_y = PolMaxY <<4;
    sprite->orig_x = sprite->x;
    sprite->orig_y = sprite->y;
    break;

  case COP:
    sprite->width = sprite->height = 32;
    sprite->x_offset = 32; sprite->y_offset = -16;
    sprite->x_hot = 40; sprite->y_hot = -8;
    sprite->frame = 5;
    sprite->count = 1500;
    sprite->dest_x = Rand((WORLD_X <<4) - 1);
    sprite->dest_y = Rand((WORLD_Y <<4) - 1);
    sprite->orig_x = x - 30;
    sprite->orig_y = y;
    break;

  case AIR:
    sprite->width = sprite->height = 48;
    sprite->x_offset = 24; sprite->y_offset = 0;
    sprite->x_hot = 48; sprite->y_hot = 16;
    if (x > ((WORLD_X - 20) <<4)) {
      sprite->x -= 100 + 48;
      sprite->dest_x = sprite->x - 200;
      sprite->frame = 7;
    } else {
      sprite->dest_x = sprite->x + 200;
      sprite->frame = 11;
    }
    sprite->dest_y = sprite->y;
    break;

  case TOR:
    sprite->width = sprite->height = 48;
    sprite->x_offset = 24; sprite->y_offset = 0;
    sprite->x_hot = 40; sprite->y_hot = 36;
    sprite->frame = 1;
    sprite->count = 200;
    break;

  case EXP:
    sprite->width = sprite->height = 48;
    sprite->x_offset = 24; sprite->y_offset = 0;
    sprite->x_hot = 40; sprite->y_hot = 16;
    sprite->frame = 1;
    break;

  case BUS:
    sprite->width = sprite->height = 32;
    sprite->x_offset = 30; sprite->y_offset = -18;
    sprite->x_hot = 40; sprite->y_hot = -8;
    sprite->frame = 1;
    sprite->dir = 1;
    break;

  }
}


DestroyAllSprites()
{
  SimSprite *sprite;

  for (sprite = sim->sprite; sprite != NULL; sprite = sprite->next) {
    sprite->frame = 0;
  }
}


DestroySprite(SimSprite *sprite)
{
  SimView *view;
  SimSprite **sp;

  for (view = sim->editor; view != NULL; view = view->next)
    if (view->follow == sprite)
      view->follow = NULL;

  if (GlobalSprites[sprite->type] == sprite) {
    GlobalSprites[sprite->type] = (SimSprite *)NULL;
  }

  if (sprite->name != NULL) {
    ckfree(sprite->name);
    sprite->name = NULL;
  }

  for (sp = &sim->sprite; *sp != NULL; sp = &((*sp)->next)) {
    if (sprite == (*sp)) {
      *sp = sprite->next;
      break;
    }
  }

  sprite->next = FreeSprites;
  FreeSprites = sprite;
}


SimSprite *
GetSprite(int type)
{
  SimSprite *sprite;

  if (((sprite = GlobalSprites[type]) == NULL) ||
      (sprite->frame == 0))
    return (SimSprite *)NULL;
  else
    return sprite;
}


SimSprite *
MakeSprite(int type, int x, int y)
{
  SimSprite *sprite;
  
  if ((sprite = GlobalSprites[type]) == NULL) {
    sprite = NewSprite("", type, x, y);
  } else {
    InitSprite(sprite, x, y);
  }
  return sprite;
}


SimSprite *
MakeNewSprite(int type, int x, int y)
{
  SimSprite *sprite;
  
  sprite = NewSprite("", type, x, y);
  return sprite;
}


DrawObjects(SimView *view)
{
  SimSprite *sprite;

  /* XXX: sort these by layer */
/*
  if (z = Oframe[TRA]) DrawTrain(view, z);
  if (z = Oframe[SHI]) DrawBoat(view, z);
  if (z = Oframe[GOD]) DrawMonster(view, z);
  if (z = Oframe[COP]) DrawCopter(view, z);
  if (z = Oframe[AIR]) DrawPlane(view, z);
  if (z = Oframe[TOR]) DrawTor(view, z);
  if (z = Oframe[EXP]) DrawExp(view, z);
*/

  for (sprite = sim->sprite; sprite != NULL; sprite = sprite->next) {
    DrawSprite(view, sprite);
  }
}


DrawSprite(SimView *view, SimSprite *sprite)
{
  Pixmap pict, mask;
  int x, y, i;

  if (sprite->frame == 0)
    return;

  i = (sprite->frame - 1) * 2;
  pict = view->x->objects[sprite->type][i];
  mask = view->x->objects[sprite->type][i + 1];

  x = sprite->x
        - ((view->tile_x <<4) - view->screen_x)
	+ sprite->x_offset;
  y = sprite->y 
        - ((view->tile_y <<4) - view->screen_y)
	+ sprite->y_offset;

  XSetClipMask(view->x->dpy, view->x->gc, mask);
  XSetClipOrigin(view->x->dpy, view->x->gc, x, y);
  XCopyArea(view->x->dpy, pict, view->pixmap2, view->x->gc, 
	    0, 0, sprite->width, sprite->height, x, y);
  XSetClipMask(view->x->dpy, view->x->gc, None);
  XSetClipOrigin(view->x->dpy, view->x->gc, 0, 0);
}


short GetChar(int x, int y)
{
  x >>= 4;
  y >>= 4;
  if (!TestBounds(x, y))
    return(-1);
  else
    return(Map[x][y] & LOMASK);
}


short TurnTo(int p, int d)
{
  if (p == d) return(p);
  if (p < d)
    if ((d - p) < 4) p++;
    else p--;
  else
    if ((p - d) < 4) p--;
    else p++;
  if (p > 8) p = 1;
  if (p < 1) p = 8;
  return(p);
}


TryOther(int Tpoo, int Told, int Tnew)
{
  register short z;

  z = Told + 4;
  if (z > 8) z -= 8;
  if (Tnew != z) return(0);
  if ((Tpoo == POWERBASE) || (Tpoo == POWERBASE + 1) ||
      (Tpoo == RAILBASE) || (Tpoo == RAILBASE + 1))
    return(1);
  return(0);
}


short SpriteNotInBounds(SimSprite *sprite)
{
  int x = sprite->x + sprite->x_hot;
  int y = sprite->y + sprite->y_hot;

  if ((x < 0) || (y < 0) ||
      (x >= (WORLD_X <<4)) ||
      (y >= (WORLD_Y <<4))) {
    return (1);
  }
  return (0);
}


short GetDir(int orgX, int orgY, int desX, int desY)
{
  static short Gdtab[13] = { 0, 3, 2, 1, 3, 4, 5, 7, 6, 5, 7, 8, 1 };
  int dispX, dispY, z;

  dispX = desX - orgX;
  dispY = desY - orgY;
  if (dispX < 0)
    if (dispY < 0) z = 11;
    else z = 8;
  else
    if (dispY < 0) z = 2;
    else z = 5;
  if (dispX < 0) dispX = -dispX;
  if (dispY < 0) dispY = -dispY;

  absDist = dispX + dispY;

  if ((dispX <<1) < dispY) z++;
  else if ((dispY <<1) < dispY) z--;

  if ((z < 0) || (z > 12)) z = 0;

  return (Gdtab[z]);
}


GetDis(int x1, int y1, int x2, int y2)
{
  register short dispX, dispY;
	
  if (x1 > x2) dispX = x1 - x2;
  else dispX = x2 - x1;
  if (y1 > y2) dispY = y1 - y2;
  else dispY = y2 - y1;

  return (dispX + dispY);
}


int CheckSpriteCollision(SimSprite *s1, SimSprite *s2)
{
  if ((s1->frame != 0) && (s2->frame != 0) &&
      GetDis(s1->x + s1->x_hot, s1->y + s1->y_hot,
	     s2->x + s2->x_hot, s2->y + s2->y_hot) < 30)
    return(1);
  return(0);
}


MoveObjects()
{
  SimSprite *sprite;

  if (!SimSpeed) return;
  Cycle++;

  for (sprite = sim->sprite; sprite != NULL;) {
    if (sprite->frame) {
      switch (sprite->type) {
      case TRA:
	DoTrainSprite(sprite);
	break;
      case COP:
	DoCopterSprite(sprite);
	break;
      case AIR:
	DoAirplaneSprite(sprite);
	break;
      case SHI:
	DoShipSprite(sprite);
	break;
      case GOD:
	DoMonsterSprite(sprite);
	break;
      case TOR:
	DoTornadoSprite(sprite);
	break;
      case EXP:
	DoExplosionSprite(sprite);
	break;
      case BUS:
	DoBusSprite(sprite);
	break;
      }
      sprite = sprite->next;
    } else {
      if (sprite->name[0] == '\0') {
	SimSprite *s = sprite;
	sprite = sprite->next;
	DestroySprite(s);
      } else {
	sprite = sprite->next;
      }
    }
  }
}


DoTrainSprite(SimSprite *sprite)
{
  static short Cx[4] = {   0,  16,   0, -16 };
  static short Cy[4] = { -16,   0,  16,   0 };
  static short Dx[5] = {   0,   4,   0,  -4,   0 };
  static short Dy[5] = {  -4,   0,   4,   0,   0 };
  static short TrainPic2[5] = { 1, 2, 1, 2, 5 };
  register short z, dir, dir2;
  short c;

  if ((sprite->frame == 3) || (sprite->frame == 4))
    sprite->frame = TrainPic2[sprite->dir];
  sprite->x += Dx[sprite->dir];
  sprite->y += Dy[sprite->dir];
  if (!(Cycle & 3)) {
    dir = Rand16() & 3;
    for (z = dir; z < (dir + 4); z++) {
      dir2 = z & 3;
      if (sprite->dir != 4) {
	if (dir2 == ((sprite->dir + 2) & 3)) continue;
      }
      c = GetChar(sprite->x + Cx[dir2] + 48,
		  sprite->y + Cy[dir2]);
      if (((c >= RAILBASE) && (c <= LASTRAIL)) || /* track? */
	  (c == RAILVPOWERH) ||
	  (c == RAILHPOWERV)) {
	if ((sprite->dir != dir2) &&
	    (sprite->dir != 4)) {
	  if ((sprite->dir + dir2) == 3)
	    sprite->frame = 3;
	  else
	    sprite->frame = 4;
	} else
	  sprite->frame = TrainPic2[dir2];

	if ((c == RAILBASE) || (c == (RAILBASE + 1)))
	  sprite->frame = 5;
	sprite->dir = dir2;
	return;
      }
    }
    if (sprite->dir == 4) {
      sprite->frame = 0;
      return;
    }
    sprite->dir = 4;
  }
}


DoCopterSprite(SimSprite *sprite)
{
  static short CDx[9] = { 0,  0,  3,  5,  3,  0, -3, -5, -3 };
  static short CDy[9] = { 0, -5, -3,  0,  3,  5,  3,  0, -3 };
  register short z, d, x, y;

  if (sprite->sound_count > 0) sprite->sound_count--;

  if (sprite->control < 0) {

    if (sprite->count > 0) sprite->count--;

    if (!sprite->count) {
      /* Attract copter to monster and tornado so it blows up more often */
      SimSprite *s = GetSprite(GOD);
      if (s != NULL) {
	sprite->dest_x = s->x;
	sprite->dest_y = s->y;
      } else {
	s = GetSprite(TOR);
	if (s != NULL) {
	  sprite->dest_x = s->x;
	  sprite->dest_y = s->y;
	} else {
	  sprite->dest_x = sprite->orig_x;
	  sprite->dest_y = sprite->orig_y;
	}
      }
    }
    if (!sprite->count) { /* land */
      GetDir(sprite->x, sprite->y, sprite->orig_x, sprite->orig_y);
      if (absDist < 30) {
	sprite->frame = 0;
	return;
      }
    }
  } else {
    GetDir(sprite->x, sprite->y, sprite->dest_x, sprite->dest_y);
    if (absDist < 16) {
      sprite->dest_x = sprite->orig_x;
      sprite->dest_y = sprite->orig_y;
      sprite->control = -1;
    }
  }

  if (!sprite->sound_count) { /* send report  */
    x = (sprite->x + 48) >>5;
    y = sprite->y >>5;
    if ((x >= 0) &&
	(x < (WORLD_X >>1)) &&
	(y >= 0) &&
	(y < (WORLD_Y >>1))) {
      /* Don changed from 160 to 170 to shut the #$%#$% thing up! */
      if ((TrfDensity[x][y] > 170) && ((Rand16() & 7) == 0)) {
	SendMesAt(-41, (x <<1) + 1, (y <<1) + 1);
	MakeSound("city", "HeavyTraffic"); /* chopper */
	sprite->sound_count = 200;
      }
    }
  }
  z = sprite->frame;
  if (!(Cycle & 3)) {
    d = GetDir(sprite->x, sprite->y, sprite->dest_x, sprite->dest_y);
    z = TurnTo(z, d);
    sprite->frame = z;
  }

  sprite->x += CDx[z];
  sprite->y += CDy[z];
}


DoAirplaneSprite(SimSprite *sprite)
{
  static short CDx[12] = { 0,  0,  6,  8,  6,  0, -6, -8, -6,  8,  8,  8 };
  static short CDy[12] = { 0, -8, -6,  0,  6,  8,  6,  0, -6,  0,  0,  0 };

  register short z, d;

  z = sprite->frame;
	
  if (!(Cycle % 5)) {
    if (z > 8) { /* TakeOff  */
      z--;
      if (z < 9) z = 3;
      sprite->frame = z;
    } else { /* goto destination */
      d = GetDir(sprite->x, sprite->y, sprite->dest_x, sprite->dest_y);
      z = TurnTo(z, d);
      sprite->frame = z;
    }
  }	

  if (absDist < 50) { /* at destination  */
    sprite->dest_x = Rand((WORLD_X * 16) + 100) - 50;
    sprite->dest_y = Rand((WORLD_Y * 16) + 100) - 50;
  }

  /* deh added test for !Disasters */
  if (!NoDisasters) {
    SimSprite *s;
    int explode = 0;

    for (s = sim->sprite; s != NULL; s = s->next) {
      if ((s->frame != 0) &&
	  ((s->type == COP) ||
	   ((sprite != s) &&
	    (s->type == AIR))) &&
	  CheckSpriteCollision(sprite, s)) {
	ExplodeSprite(s);
	explode = 1;
      }
    }
    if (explode)
      ExplodeSprite(sprite);
  }

  sprite->x += CDx[z];
  sprite->y += CDy[z];
  if (SpriteNotInBounds(sprite)) sprite->frame = 0;
}


DoShipSprite(SimSprite *sprite)
{
  static short BDx[9] = { 0,  0,  1,  1,  1,  0, -1, -1, -1 };
  static short BDy[9] = { 0, -1, -1,  0,  1,  1,  1,  0, -1 };
  static short BPx[9] = { 0,  0,  2,  2,  2,  0, -2, -2, -2 };
  static short BPy[9] = { 0, -2, -2,  0,  2,  2,  2,  0, -2 };
  static short BtClrTab[8] = { RIVER, CHANNEL, POWERBASE, POWERBASE + 1,
			       RAILBASE, RAILBASE + 1, BRWH, BRWV };
  register short x, y, z, t = RIVER;
  short tem, pem;

  if (sprite->sound_count > 0) sprite->sound_count--;
  if (!sprite->sound_count) {
    if ((Rand16() & 3) == 1) {
      if ((ScenarioID == 2) && /* San Francisco */
	  (Rand(10) < 5)) {
	MakeSound("city", "HonkHonk-Low -speed 80");
      } else {
	MakeSound("city", "HonkHonk-Low");
      }
    }
    sprite->sound_count = 200;
  }

  if (sprite->count > 0) sprite->count--;
  if (!sprite->count) {
    sprite->count = 9;
    if (sprite->frame != sprite->new_dir) {
      sprite->frame = TurnTo(sprite->frame, sprite->new_dir);
      return;
    }
    tem = Rand16() & 7;
    for (pem = tem; pem < (tem + 8); pem++) {
      z = (pem & 7) + 1;

      if (z == sprite->dir) continue;
      x = ((sprite->x + (48 - 1)) >>4) + BDx[z];
      y = (sprite->y >>4) + BDy[z];
      if (TestBounds(x, y)) {
	t = Map[x][y] & LOMASK;
	if ((t == CHANNEL) || (t == BRWH) || (t == BRWV) ||
	    TryOther(t, sprite->dir, z)) {
	  sprite->new_dir = z;
	  sprite->frame = TurnTo(sprite->frame, sprite->new_dir);
	  sprite->dir = z + 4;
	  if (sprite->dir > 8) sprite->dir -= 8;
	  break;
	}
      }
    }
    if (pem == (tem + 8)) {
      sprite->dir = 10;
      sprite->new_dir = (Rand16() & 7) + 1;
    }
  } else {
    z = sprite->frame;
    if (z == sprite->new_dir)  {
      sprite->x += BPx[z];
      sprite->y += BPy[z];
    }
  }
  if (SpriteNotInBounds(sprite)) {
    sprite->frame = 0;
    return;
  }
  for (z = 0; z < 8; z++) {
    if (t == BtClrTab[z]) break;
    if (z == 7) {
      ExplodeSprite(sprite);
      Destroy(sprite->x + 48, sprite->y);
    }
  }
}


DoMonsterSprite(SimSprite *sprite)
{
  static short Gx[5] = {  2,  2, -2, -2,  0 };
  static short Gy[5] = { -2,  2,  2, -2,  0 };
  static short ND1[4] = {  0,  1,  2,  3 };
  static short ND2[4] = {  1,  2,  3,  0 };
  static short nn1[4] = {  2,  5,  8, 11 };
  static short nn2[4] = { 11,  2,  5,  8 };
  register short d, z, c;

  if (sprite->sound_count > 0) sprite->sound_count--;

  if (sprite->control < 0) {
    /* business as usual */

    if (sprite->control == -2) {
      d = (sprite->frame - 1) / 3;
      z = (sprite->frame - 1) % 3;
      if (z == 2) sprite->step = 0;
      if (z == 0) sprite->step = 1;
      if (sprite->step) z++;
      else z--;
      c = GetDir(sprite->x, sprite->y, sprite->dest_x, sprite->dest_y);
      if (absDist < 18) {
	sprite->control = -1;
	sprite->count = 1000;
	sprite->flag = 1;
	sprite->dest_x = sprite->orig_x;
	sprite->dest_y = sprite->orig_y;
      } else {
	c = (c - 1) / 2;
	if (((c != d) && (!Rand(5))) ||
	    (!Rand(20))) {
	  int diff = (c - d) & 3;
	  if ((diff == 1) || (diff == 3)) {
	    d = c;
	  } else {
	    if (Rand16() & 1) d++; else d--;
	    d &= 3;
	  }
	} else {
	  if (!Rand(20)) {
	    if (Rand16() & 1) d++; else d--;
	    d &= 3;
	  }
	}
      }
    } else {

      d = (sprite->frame - 1) / 3;

      if (d < 4) { /* turn n s e w */
	z = (sprite->frame - 1) % 3;
	if (z == 2) sprite->step = 0;
	if (z == 0) sprite->step = 1;
	if (sprite->step) z++;
	else z--;
	GetDir(sprite->x, sprite->y, sprite->dest_x, sprite->dest_y);
	if (absDist < 60) {
	  if (sprite->flag == 0) {
	    sprite->flag = 1;
	    sprite->dest_x = sprite->orig_x;
	    sprite->dest_y = sprite->orig_y;
	  } else {
	    sprite->frame = 0;
	    return;
	  }
	}
	c = GetDir(sprite->x, sprite->y, sprite->dest_x, sprite->dest_y);
	c = (c - 1) / 2;
	if ((c != d) && (!Rand(10))) {
	  if (Rand16() & 1) z = ND1[d];
	  else z = ND2[d];
	  d = 4;
	  if (!sprite->sound_count) {
	    MakeSound("city", "Monster -speed [MonsterSpeed]"); /* monster */
	    sprite->sound_count = 50 + Rand(100);
	  }
	}
      } else {
	d = 4;
	c = sprite->frame;
	z = (c - 13) & 3;
	if (!(Rand16() & 3)) {
	  if (Rand16() & 1) z = nn1[z];
	  else z = nn2[z];
	  d = (z - 1) / 3;
	  z = (z - 1) % 3;
	}
      }
    }
  } else {
    /* somebody's taken control of the monster */

    d = sprite->control;
    z = (sprite->frame - 1) % 3;

    if (z == 2) sprite->step = 0;
    if (z == 0) sprite->step = 1;
    if (sprite->step) z++;
    else z--;
  }

  z = (((d * 3) + z) + 1);
  if (z > 16) z = 16;
  sprite->frame = z;

  sprite->x += Gx[d];
  sprite->y += Gy[d];

  if (sprite->count > 0) sprite->count--;
  c = GetChar(sprite->x + sprite->x_hot, sprite->y + sprite->y_hot);
  if ((c == -1) ||
      ((c == RIVER) &&
       (sprite->count != 0) &&
       (sprite->control == -1))) {
    sprite->frame = 0; /* kill zilla */
  }

  { SimSprite *s;
    for (s = sim->sprite; s != NULL; s = s->next) {
      if ((s->frame != 0) &&
	  ((s->type == AIR) ||
	   (s->type == COP) ||
	   (s->type == SHI) ||
	   (s->type == TRA)) &&
	  CheckSpriteCollision(sprite, s)) {
	  ExplodeSprite(s);
	}
      }
    }

  Destroy(sprite->x + 48, sprite->y + 16);
}


DoTornadoSprite(SimSprite *sprite)
{
  static short CDx[9] = {  2,  3,  2,  0, -2, -3 };
  static short CDy[9] = { -2,  0,  2,  3,  2,  0 };
  register short z;

  z = sprite->frame;

  if (z == 2) /* cycle animation... post Rel */
    if (sprite->flag)
      z = 3;
    else
      z = 1;
  else {
    if (z == 1)
      sprite->flag = 1;
    else
      sprite->flag = 0;
    z = 2;
  }

  if (sprite->count > 0) sprite->count--;

  sprite->frame = z;

  { SimSprite *s;
    for (s = sim->sprite; s != NULL; s = s->next) {
      if ((s->frame != 0) &&
	  ((s->type == AIR) ||
	   (s->type == COP) ||
	   (s->type == SHI) ||
	   (s->type == TRA)) &&
	  CheckSpriteCollision(sprite, s)) {
	  ExplodeSprite(s);
	}
      }
    }

  z = Rand(5);
  sprite->x += CDx[z];
  sprite->y += CDy[z];
  if (SpriteNotInBounds(sprite)) sprite->frame = 0;

  if ((sprite->count != 0) &&
      (!Rand(500)))
    sprite->frame = 0;

  Destroy(sprite->x + 48, sprite->y + 40);
}


DoExplosionSprite(SimSprite *sprite)
{
  short x, y;

  if (!(Cycle & 1)) {
    if (sprite->frame == 1) {
      MakeSound("city", "Explosion-High"); /* explosion */
      x = (sprite->x >>4) + 3;
      y = (sprite->y >>4);
      SendMesAt(32, x, y);
    }
    sprite->frame++;
  }

  if (sprite->frame > 6) {
    sprite->frame = 0;

    StartFire(sprite->x + 48 - 8, sprite->y + 16);
    StartFire(sprite->x + 48 - 24, sprite->y);
    StartFire(sprite->x + 48 + 8, sprite->y);
    StartFire(sprite->x + 48 - 24, sprite->y + 32);
    StartFire(sprite->x + 48 + 8, sprite->y + 32);
    return;
  }
}


DoBusSprite(SimSprite *sprite)
{
  static short Dx[5] = {   0,   1,   0,  -1,   0 };
  static short Dy[5] = {  -1,   0,   1,   0,   0 };
  static short Dir2Frame[4] = { 1, 2, 1, 2 };
  register int dir, dir2;
  int c, dx, dy, crossed, tx, ty, otx, oty;
  int turned = 0;
  int speed, z;

#ifdef DEBUGBUS
printf("Bus dir %d turn %d frame %d\n",
       sprite->dir, sprite->turn, sprite->frame);
#endif

  if (sprite->turn) {
    if (sprite->turn < 0) { /* ccw */
      if (sprite->dir & 1) { /* up or down */
	sprite->frame = 4;
      } else { /* left or right */
	sprite->frame = 3;
      }
      sprite->turn++;
      sprite->dir = (sprite->dir - 1) & 3;
    } else { /* cw */
      if (sprite->dir & 1) { /* up or down */
	sprite->frame = 3;
      } else { /* left or right */
	sprite->frame = 4;
      }
      sprite->turn--;
      sprite->dir = (sprite->dir + 1) & 3;
    }
    turned = 1;
  } else {
    /* finish turn */
    if ((sprite->frame == 3) || (sprite->frame == 4)) {
      turned = 1;
      sprite->frame = Dir2Frame[sprite->dir];
    }
  }

  if (sprite->speed == 0) {
    /* brake */
    dx = 0; dy = 0;
  } else { /* cruise at traffic speed */

    tx = (sprite->x + sprite->x_hot) >>5;
    ty = (sprite->y + sprite->y_hot) >>5;
    if ((tx >= 0) &&
	(tx < (WORLD_X >>1)) &&
	(ty >= 0) &&
	(ty < (WORLD_Y >>1))) {
      z = TrfDensity[tx][ty] >>6;
      if (z > 1) z--;
    } else z = 0;

    switch (z) {
    case 0:
      speed = 8;
      break;
    case 1:
      speed = 4;
      break;
    case 2:
      speed = 1;
      break;
    }

    /* govern speed */
    if (speed > sprite->speed)
      speed = sprite->speed;

    if (turned) {
#ifdef DEBUGBUS
printf("turned\n");
#endif
      if (speed > 1) speed = 1;
      dx = Dx[sprite->dir] * speed;
      dy = Dy[sprite->dir] * speed;
    } else {
      dx = Dx[sprite->dir] * speed;
      dy = Dy[sprite->dir] * speed;

      tx = (sprite->x + sprite->x_hot) >>4;
      ty = (sprite->y + sprite->y_hot) >>4;

      /* drift into the right lane */
      switch (sprite->dir) {
      case 0: /* up */
	z = ((tx <<4) + 4) - (sprite->x + sprite->x_hot);
	if (z < 0) dx = -1;
	else if (z > 0) dx = 1;
#ifdef DEBUGBUS
printf("moving up x %x z %d dx %d\n", sprite->x + sprite->x_hot, z, dx);
#endif
	break;
      case 1: /* right */
	z = ((ty <<4) + 4) - (sprite->y + sprite->y_hot);
	if (z < 0) dy = -1;
	else if (z > 0) dy = 1;
#ifdef DEBUGBUS
printf("moving right y %x z %d dy %d\n", sprite->y + sprite->y_hot, z, dy);
#endif
	break;
      case 2: /* down */
	z = ((tx <<4)) - (sprite->x + sprite->x_hot);
	if (z < 0) dx = -1;
	else if (z > 0) dx = 1;
#ifdef DEBUGBUS
printf("moving down x %x z %d dx %d\n", sprite->x + sprite->x_hot, z, dx);
#endif
	break;
      case 3: /* left */
	z = ((ty <<4)) - (sprite->y + sprite->y_hot);
	if (z < 0) dy = -1;
	else if (z > 0) dy = 1;
#ifdef DEBUGBUS
printf("moving left y %x z %d dy %d\n", sprite->y + sprite->y_hot, z, dy);
#endif
	break;
      }
    }
  }
#ifdef DEBUGBUS
printf("speed dx %d dy %d\n", dx, dy);
#endif

#define AHEAD 8

  otx = (sprite->x + sprite->x_hot + (Dx[sprite->dir] * AHEAD)) >>4;
  oty = (sprite->y + sprite->y_hot + (Dy[sprite->dir] * AHEAD)) >>4;
  if (otx < 0) otx = 0; else if (otx >= WORLD_X) otx = WORLD_X - 1;
  if (oty < 0) oty = 0; else if (oty >= WORLD_Y) oty = WORLD_Y - 1;

  tx = (sprite->x + sprite->x_hot + dx + (Dx[sprite->dir] * AHEAD)) >>4;
  ty = (sprite->y + sprite->y_hot + dy + (Dy[sprite->dir] * AHEAD)) >>4;
  if (tx < 0) tx = 0; else if (tx >= WORLD_X) tx = WORLD_X - 1;
  if (ty < 0) ty = 0; else if (ty >= WORLD_Y) ty = WORLD_Y - 1;

  if ((tx != otx) || (ty != oty)) {
#ifdef DEBUGBUS
printf("drive from tile %d %d to %d %d\n",
       otx, oty, tx, ty);
#endif
    z = CanDriveOn(tx, ty);
    if (z == 0) {
      /* can't drive forward into a new tile */
      if (speed == 8) {
	bulldozer_tool(NULL, tx, ty);
      } else {
      }
    } else {
      /* drive forward into a new tile */
      if (z > 0) {
	/* smooth */
      } else {
	/* bumpy */
	dx /= 2;
	dy /= 2;
      }
    }
  }

  tx = (sprite->x + sprite->x_hot + dx) >>4;
  ty = (sprite->y + sprite->y_hot + dy) >>4;
  z = CanDriveOn(tx, ty);
  if (z > 0) {
    /* cool, cruise along */
  } else {
    if (z < 0) {
      /* bumpy */
    } else {
      /* something in the way */
    }
  }

  sprite->x += dx;
  sprite->y += dy;

  if (!NoDisasters) {
    SimSprite *s;
    int explode = 0;

    for (s = sim->sprite; s != NULL; s = s->next) {
      if ((sprite != s) &&
	  (s->frame != 0) &&
	  ((s->type == BUS) ||
	   ((s->type == TRA) &&
	    (s->frame != 5))) &&
	  CheckSpriteCollision(sprite, s)) {
	ExplodeSprite(s);
	explode = 1;
      }
    }
    if (explode)
      ExplodeSprite(sprite);
  }
}


int
CanDriveOn(int x, int y)
{
  int tile;

  if (!TestBounds(x, y))
    return 0;

  tile = Map[x][y] & LOMASK;

  if (((tile >= ROADBASE) &&
       (tile <= LASTROAD) &&
       (tile != BRWH) &&
       (tile != BRWV)) ||
      (tile == HRAILROAD) ||
      (tile == VRAILROAD))
    return 1;

  if ((tile == DIRT) || tally(tile))
    return -1;

  return 0;
}


ExplodeSprite(SimSprite *sprite)
{
  int x, y;

  sprite->frame = 0;

  x = sprite->x + sprite->x_hot;
  y = sprite->y + sprite->y_hot;
  MakeExplosionAt(x, y);

  x = (x >>4);
  y = (y >>4);

  switch (sprite->type) {
  case AIR:
    CrashX = x;
    CrashY = y;
    SendMesAt(-24, x, y);
    break;
  case SHI:
    CrashX = x;
    CrashY = y;
    SendMesAt(-25, x, y);
    break;
  case TRA:
    CrashX = x;
    CrashY = y;
    SendMesAt(-26, x, y);
    break;
  case COP:
    CrashX = x;
    CrashY = y;
    SendMesAt(-27, x, y);
    break;
  case BUS:
    CrashX = x;
    CrashY = y;
    SendMesAt(-26, x, y); /* XXX for now */
    break;
  }
  MakeSound("city", "Explosion-High"); /* explosion */
  return;
}


int checkWet(int x)
{
  if ((x == POWERBASE) || (x == POWERBASE + 1) ||
      (x == RAILBASE) || (x == RAILBASE + 1) ||
      (x == BRWH) || (x == BRWV))
    return(1);
  else
    return(0);
}


Destroy(int ox, int oy)
{
  short t, z, x, y;
	
  x = ox >>4;
  y = oy >>4;
  if (!TestBounds(x, y))
    return;
  z = Map[x][y];
  t = z & LOMASK;
  if (t >= TREEBASE) {
    /* TILE_IS_BRIDGE(t) */
    if (!(z & BURNBIT)) {		
      if ((t >= ROADBASE) && (t <= LASTROAD))
	Map[x][y] = RIVER;
      return;
    }
    if (z & ZONEBIT) {
      OFireZone(x, y, z);
      if (t > RZB) {
	MakeExplosionAt(ox, oy);
      }
    }
    if (checkWet(t))
      Map[x][y] = RIVER;
    else
      Map[x][y] = (DoAnimation
		   ? TINYEXP
		   : (LASTTINYEXP - 3)) | BULLBIT | ANIMBIT;
  }
}


OFireZone(int Xloc, int Yloc, int ch)
{
  register short Xtem, Ytem;
  short x, y, XYmax;

  RateOGMem[Xloc >>3][Yloc >>3] -= 20;

  ch &= LOMASK;
  if (ch < PORTBASE)
    XYmax = 2;
  else
    if (ch == AIRPORT) XYmax = 5;
    else XYmax = 4;

  for (x = -1; x < XYmax; x++)
    for (y = -1; y < XYmax; y++) {
      Xtem = Xloc + x;
      Ytem = Yloc + y;
      if ((Map[Xtem][Ytem] & LOMASK) >= ROADBASE)
	Map[Xtem][Ytem] |= BULLBIT;
    }
}


StartFire(int x, int y)
{
  register t, z;

  x >>= 4;
  y >>= 4;
  if ((x >= WORLD_X) ||
      (y >= WORLD_Y) ||
      (x < 0) || (y < 0))
    return;
  z = Map[x][y];
  t = z & LOMASK;
  if ((!(z & BURNBIT)) && (t != 0)) return;
  if (z & ZONEBIT) return;
  Map[x][y] = FIRE + (Rand16() & 3) + ANIMBIT;
}


GenerateTrain(int x, int y)
{
  if ((TotalPop > 20) &&
      (GetSprite(TRA) == NULL) &&
      (!Rand(25))) {
    MakeSprite(TRA, (x <<4) + TRA_GROOVE_X, (y <<4) + TRA_GROOVE_Y);
  }
}


GenerateBus(int x, int y)
{
  if ((GetSprite(BUS) == NULL) &&
      (!Rand(25))) {
    MakeSprite(BUS, (x <<4) + BUS_GROOVE_X, (y <<4) + BUS_GROOVE_Y);
  }
}


GenerateShip(void)
{
  register short x, y;

  if (!(Rand16() & 3))
    for (x = 4; x < WORLD_X - 2; x++)
      if (Map[x][0] == CHANNEL)  {
	MakeShipHere(x, 0);
	return;
      }
  if (!(Rand16() & 3))
    for (y = 1; y < WORLD_Y - 2; y++)
      if (Map[0][y] == CHANNEL)  {
	MakeShipHere(0, y);
	return;
      }
  if (!(Rand16() & 3))
    for (x = 4; x < WORLD_X - 2; x++)			
      if (Map[x][WORLD_Y - 1] == CHANNEL)  {
	MakeShipHere(x, WORLD_Y - 1);
	return;
      }
  if (!(Rand16() & 3))
    for (y = 1; y < WORLD_Y - 2; y++)
      if (Map[WORLD_X - 1][y] == CHANNEL)  {
	MakeShipHere(WORLD_X - 1, y);
	return;
      }
}


MakeShipHere(int x, int y, int z)	
{
  MakeSprite(SHI, (x <<4) - (48 - 1), (y <<4));
}


MakeMonster(void)
{
  register x, y, z, done = 0;
  SimSprite *sprite;

  if ((sprite = GetSprite(GOD)) != NULL) {
    sprite->sound_count = 1;
    sprite->count = 1000;
    sprite->dest_x = PolMaxX <<4;
    sprite->dest_y = PolMaxY <<4;
    return;
  }

  for (z = 0; z < 300; z++)  {
    x = Rand(WORLD_X - 20) + 10;
    y = Rand(WORLD_Y - 10) + 5;
    if ((Map[x][y] == RIVER) || (Map[x][y] == RIVER + BULLBIT)) {
      MonsterHere(x, y);
      done = 1;
      break;
    }
  }
  if (!done == 0)
    MonsterHere(60, 50);
}


MonsterHere(int x, int y)
{
  short z;

  MakeSprite(GOD, (x <<4) + 48, (y <<4));
  ClearMes();
  SendMesAt(-21, x + 5, y);
}


GenerateCopter(int x, int y)
{
  if (GetSprite(COP) != NULL) return;

  MakeSprite(COP, (x <<4), (y <<4) + 30);
}


GeneratePlane(int x, int y)
{
  if (GetSprite(AIR) != NULL) return;

  MakeSprite(AIR, (x <<4) + 48, (y <<4) + 12);
}


MakeTornado(void)
{
  short x, y;
  SimSprite *sprite;

  if ((sprite = GetSprite(TOR)) != NULL) {
    sprite->count = 200;
    return;
  }

  x = Rand((WORLD_X <<4) - 800) + 400;
  y = Rand((WORLD_Y <<4) - 200) + 100;
  MakeSprite(TOR, x, y);
  ClearMes();
  SendMesAt(-22, (x >>4) + 3, (y >>4) + 2);
}


MakeExplosion(int x, int y)
{
  if ((x >= 0) && (x < WORLD_X) &&
      (y >= 0) && (y < WORLD_Y)) {
    MakeExplosionAt((x << 4) + 8, (y << 4) + 8);
  }
}


MakeExplosionAt(int x, int y)
{
  MakeNewSprite(EXP, x - 40, y - 16);
}

