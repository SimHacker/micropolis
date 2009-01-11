/* sprite.cpp
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

/** @file sprite.cpp */

////////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "text.h"


////////////////////////////////////////////////////////////////////////


#define TRA_GROOVE_X -39
#define TRA_GROOVE_Y 6
#define BUS_GROOVE_X -39
#define BUS_GROOVE_Y 6


////////////////////////////////////////////////////////////////////////


/**
 * Create and initialize a sprite.
 * @param name Name of the sprite (always \c "").
 * @param type Type pf the sprite. @see SpriteType.
 * @param x X coordinate of the sprite (in pixels).
 * @param y Y coordinate of the sprite (in pixels).
 * @return New sprite object.
 */
SimSprite *Micropolis::NewSprite(const char *name, int type, int x, int y)
{
    SimSprite *sprite;

    // If a sprite is available at the pool, use one.
    // else, allocate a new one.
    if (FreeSprites) {
	sprite = FreeSprites;
	FreeSprites = sprite->next;
    } else {
	sprite = (SimSprite *)NewPtr(sizeof (SimSprite));
    }

    sprite->name = (char *)NewPtr((int)strlen(name) + 1);
    strcpy(sprite->name, name);
    sprite->type = type;

    InitSprite(sprite, x, y);

    sprite->next = spriteList;
    spriteList = sprite;

    return sprite;
}


/** Re-initialize an existing sprite.
 * @param sprite Sprite to re-use.
 * @param x      New x coordinate of the sprite (in pixels?).
 * @param y      New y coordinate of the sprite (in pixels?).
 * @todo Make derived classes for each type.
 * @todo Move code to (derived) #SimSprite methods.
 */
void Micropolis::InitSprite(SimSprite *sprite, int x, int y)
{
    sprite->x = x;
    sprite->y = y;
    sprite->frame = 0;
    sprite->orig_x = 0;
    sprite->orig_y = 0;
    sprite->dest_x = 0;
    sprite->dest_y = 0;
    sprite->count = 0;
    sprite->sound_count = 0;
    sprite->dir = 0;
    sprite->new_dir = 0;
    sprite->step = 0;
    sprite->flag = 0;
    sprite->control = -1;
    sprite->turn = 0;
    sprite->accel = 0;
    sprite->speed = 100;

    if (GlobalSprites[sprite->type] == NULL) {
	GlobalSprites[sprite->type] = sprite;
    }

    switch (sprite->type) {

	case TRA:
	    sprite->width = 32;
	    sprite->height = 32;
	    sprite->x_offset = 32;
	    sprite->y_offset = -16;
	    sprite->x_hot = 40;
	    sprite->y_hot = -8;
	    sprite->frame = 1;
	    sprite->dir = 4;
	    break;

	case SHI:
	    sprite->width = 48;
	    sprite->height = 48;
	    sprite->x_offset = 32;
	    sprite->y_offset = -16;
	    sprite->x_hot = 48;
	    sprite->y_hot = 0;

	    if (x < (4 <<4)) {
		sprite->frame = 3;
	    } else if (x >= ((WORLD_X - 4) <<4)) {
		sprite->frame = 7;
	    } else if (y < (4 <<4)) {
		sprite->frame = 5;
	    } else if (y >= ((WORLD_Y - 4) <<4)) {
		sprite->frame = 1;
	    } else {
		sprite->frame = 3;
	    }

	    sprite->new_dir = sprite->frame;
	    sprite->dir = 10;
	    sprite->count = 1;
	    break;

	case MON:
	    sprite->width = 48;
	    sprite->height = 48;
	    sprite->x_offset = 24;
	    sprite->y_offset = 0;
	    sprite->x_hot = 40;
	    sprite->y_hot = 16;

	    if (x > ((WORLD_X <<4) / 2)) {
		if (y > ((WORLD_Y <<4) / 2)) {
		    sprite->frame = 10;
		} else {
		    sprite->frame = 7;
		}
	    } else if (y > ((WORLD_Y <<4) / 2)) {
		sprite->frame = 1;
	    } else {
		sprite->frame = 4;
	    }

	    sprite->count = 1000;
	    sprite->dest_x = PolMaxX <<4;
	    sprite->dest_y = PolMaxY <<4;
	    sprite->orig_x = sprite->x;
	    sprite->orig_y = sprite->y;
	    break;

	case COP:
	    sprite->width = 32;
	    sprite->height = 32;
	    sprite->x_offset = 32;
	    sprite->y_offset = -16;
	    sprite->x_hot = 40;
	    sprite->y_hot = -8;
	    sprite->frame = 5;
	    sprite->count = 1500;
	    sprite->dest_x = Rand((WORLD_X <<4) - 1);
	    sprite->dest_y = Rand((WORLD_Y <<4) - 1);
	    sprite->orig_x = x - 30;
	    sprite->orig_y = y;
	    break;

	case AIR:
	    sprite->width = 48;
	    sprite->height = 48;
	    sprite->x_offset = 24;
	    sprite->y_offset = 0;
	    sprite->x_hot = 48;
	    sprite->y_hot = 16;
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
	    sprite->width = 48;
	    sprite->height = 48;
	    sprite->x_offset = 24;
	    sprite->y_offset = 0;
	    sprite->x_hot = 40;
	    sprite->y_hot = 36;
	    sprite->frame = 1;
	    sprite->count = 200;
	    break;

	case EXP:
	    sprite->width = 48;
	    sprite->height = 48;
	    sprite->x_offset = 24;
	    sprite->y_offset = 0;
	    sprite->x_hot = 40;
	    sprite->y_hot = 16;
	    sprite->frame = 1;
	    break;

	case BUS:
	    sprite->width = 32;
	    sprite->height = 32;
	    sprite->x_offset = 30;
	    sprite->y_offset = -18;
	    sprite->x_hot = 40;
	    sprite->y_hot = -8;
	    sprite->frame = 1;
	    sprite->dir = 1;
	    break;

    }
}


/**
 * Destroy all sprites bt de-activating them all (setting their
 * SimSprite::frame to 0).
 */
void Micropolis::DestroyAllSprites()
{
    SimSprite *sprite;

    for (sprite = spriteList; sprite != NULL; sprite = sprite->next) {
	sprite->frame = 0;
    }
}


/**
 * Destroy the sprite by taking it out of the active list.
 * @param sprite Sprite to destroy.
 * @todo Break the connection between any views that are following this sprite.
 */
void Micropolis::DestroySprite(SimSprite *sprite)
{
    SimSprite **sp;

    if (GlobalSprites[sprite->type] == sprite) {
	GlobalSprites[sprite->type] = (SimSprite *)NULL;
    }

    if (sprite->name != NULL) {
	FreePtr(sprite->name);
	sprite->name = NULL;
    }

    for (sp = &spriteList; *sp != NULL; sp = &((*sp)->next)) {
	if (sprite == (*sp)) {
	    *sp = sprite->next;
	    break;
	}
    }

    sprite->next = FreeSprites;
    FreeSprites = sprite;
}


/**
 * Return the sprite of the give type, if available.
 * @param type Type of the sprite.
 * @return Pointer to the active sprite if avaiable, else \c NULL.
 */
SimSprite *Micropolis::GetSprite(int type)
{
    SimSprite *sprite = GlobalSprites[type];
    if (sprite == NULL || sprite->frame == 0) {
	return (SimSprite *)NULL;
    } else {
	return sprite;
    }
}


/**
 * Make a sprite either by re-using the old one, or by making a new one.
 * @param type Sprite type of the new sprite.
 * @param x    X coordinate of the new sprite.
 * @param y    Y coordinate of the new sprite.
 */
SimSprite *Micropolis::MakeSprite(int type, int x, int y)
{
    SimSprite *sprite;

    sprite = GlobalSprites[type];
    if (sprite == NULL) {
	sprite = NewSprite("", type, x, y);
    } else {
	InitSprite(sprite, x, y);
    }
    return sprite;
}


/** Draw all sprites */
void Micropolis::DrawObjects()
{
    SimSprite *sprite;

    for (sprite = spriteList; sprite != NULL; sprite = sprite->next) {
	DrawSprite(sprite);
    }
}


/**
 * Draw sprite.
 * @param sprite Sprite to draw.
 * @bug Sprite is not actually drawn.
 */
void Micropolis::DrawSprite(SimSprite *sprite)
{
    int x, y, i;

    if (sprite->frame == 0) {
	return;
    }

    i = (sprite->frame - 1) * 2;

    x = sprite->x + sprite->x_offset;
    y = sprite->y + sprite->y_offset;

    // TODO: draw sprite

    // pict = spriteImages[sprite->type][i];
    // mask = spriteImages[sprite->type][i + 1];

}


/**
 * Get character from the map.
 * @param x X coordinate in pixels.
 * @param y Y coordinate in pixels.
 * @return Map character if on-map, or \c -1 if off-map.
 */
short Micropolis::GetChar(int x, int y)
{
    // Convert sprite coordinates to tile coordinates.
    x >>= 4;
    y >>= 4;

    if (!TestBounds(x, y)) {
	return -1;
    } else {
	return Map[x][y] & LOMASK;
    }
}


/**
 * Turn.
 * @param p Present direction (1..8).
 * @param d Destination direction (1..8).
 * @return New direction.
 * @todo Remove local magic constants and document the code.
 */
short Micropolis::TurnTo(int p, int d)
{
    if (p == d) {
	return p;
    }

    if (p < d) {
	if (d - p < 4) {
	    p++;
	} else {
	    p--;
	}
    } else {
	if (p - d < 4) {
	    p--;
	} else {
	    p++;
	}
    }

    if (p > 8) {
	p = 1;
    }

    if (p < 1) {
	p = 8;
    }

    return p;
}

/** ???
 * @todo Figure out what this function is doing.
 * @todo Remove local magic constants and document the code.
 */
bool Micropolis::TryOther(int Tpoo, int Told, int Tnew)
{
    short z;

    z = Told + 4;

    if (z > 8) {
	z -= 8;
    }

    if (Tnew != z) {
	return false;
    }

    if (Tpoo == POWERBASE || Tpoo == POWERBASE + 1
	  || Tpoo == RAILBASE || Tpoo == RAILBASE + 1) {
	return true;
    }

    return false;
}


/**
 * Check whether a sprite is still entirely on-map.
 * @param sprite Sprite to check.
 * @return Sprite is at least partly off-map.
 */
bool Micropolis::SpriteNotInBounds(SimSprite *sprite)
{
    int x = sprite->x + sprite->x_hot;
    int y = sprite->y + sprite->y_hot;

    return x < 0 || y < 0 || x >= (WORLD_X <<4) || y >= (WORLD_Y <<4);
}

/**
 * Get direction (0..8?) to get from starting point to destination point.
 * @param orgX X coordinate starting point.
 * @param orgY Y coordinate starting point.
 * @param desX X coordinate destination point.
 * @param desY Y coordinate destination point.
 * @return Direction to go in.
 * @todo Remove local magic constants and document the code.
 * @bug Has a condition that never holds.
 */
short Micropolis::GetDir(int orgX, int orgY, int desX, int desY)
{
    static const short Gdtab[13] = { 0, 3, 2, 1, 3, 4, 5, 7, 6, 5, 7, 8, 1 };
    int dispX, dispY, z;

    dispX = desX - orgX;
    dispY = desY - orgY;

    if (dispX < 0) {
	if (dispY < 0) {
	    z = 11;
	} else {
	    z = 8;
	}
    } else {
	if (dispY < 0) {
	    z = 2;
	} else {
	    z = 5;
	}
    }

    dispX = absoluteValue(dispX);
    dispY = absoluteValue(dispY);
    absDist = dispX + dispY;

    if (dispX * 2 < dispY) {
	z++;
    } else if (dispY * 2 < dispY) {  // XXX This never holds!!
	z--;
    }

    if (z < 0 || z > 12) {
	z = 0;
    }

    return Gdtab[z];
}


/**
 * Compute Manhattan distance between two points.
 * @param x1 X coordinate first point.
 * @param y1 Y coordinate first point.
 * @param x2 X coordinate second point.
 * @param y2 Y coordinate second point.
 * @return Manhattan distance between both points.
 */
int Micropolis::GetDistance(int x1, int y1, int x2, int y2)
{
    return absoluteValue(x1 - x2) + absoluteValue(y1 - y2);
}


/**
 * Check whether two sprites collide with each other.
 * @param s1 First sprite.
 * @param s2 Second sprite.
 * @return Sprites are colliding.
 */
bool Micropolis::CheckSpriteCollision(SimSprite *s1, SimSprite *s2)
{
    return s1->frame != 0 && s2->frame != 0 &&
	   GetDistance(s1->x + s1->x_hot, s1->y + s1->y_hot,
		       s2->x + s2->x_hot, s2->y + s2->y_hot) < 30;
}


/**
 * Move all sprites.
 *
 * Sprites with SimSprite::frame == 0 are removed.
 * @todo It uses SimSprite::name[0] == '\0' as condition which seems stupid.
 * @todo #DestroySprite modifies the #spriteList while we loop over it.
 */
void Micropolis::MoveObjects()
{
    SimSprite *sprite;

    if (!SimSpeed) {
	return;
    }

    Cycle++;

    for (sprite = spriteList; sprite != NULL;) {
	if (sprite->frame > 0) {
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

		case MON:
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


/**
 * Move train sprite.
 * @param sprite Train sprite.
 * @todo Remove local magic constants and document the code.
 */
void Micropolis::DoTrainSprite(SimSprite *sprite)
{
    /* Offset in pixels of sprite x and y to map tile */
    static const short Cx[4] = {   0,  16,   0, -16 };
    static const short Cy[4] = { -16,   0,  16,   0 };
    /* X and Y movement of the sprite in pixels */
    static const short Dx[5] = {   0,   4,   0,  -4,   0 };
    static const short Dy[5] = {  -4,   0,   4,   0,   0 };

    static const short TrainPic2[5] = { 1, 2, 1, 2, 5 };
    short z, dir, dir2;
    short c;

    if (sprite->frame == 3 || sprite->frame == 4) {
	sprite->frame = TrainPic2[sprite->dir];
    }

    sprite->x += Dx[sprite->dir];
    sprite->y += Dy[sprite->dir];

    if ((Cycle & 3) == 0) {

	dir = Rand16() & 3;
	for (z = dir; z < dir + 4; z++) {
	    dir2 = z & 3;

	    if (sprite->dir != 4) {
		if (dir2 == ((sprite->dir + 2) & 3)) {
		    continue;
		}
	    }

	    c = GetChar(sprite->x + Cx[dir2] + 48, sprite->y + Cy[dir2]);

	    if ((c >= RAILBASE && c <= LASTRAIL) /* track? */
		      || c == RAILVPOWERH || c == RAILHPOWERV) {

		if (sprite->dir != dir2 && sprite->dir != 4) {

		    if (sprite->dir + dir2 == 3) {
			sprite->frame = 3;
		    } else {
			sprite->frame = 4;
		    }

		} else {
		    sprite->frame = TrainPic2[dir2];
		}

		if (c == HRAIL || c == VRAIL) {
		    sprite->frame = 5;
		}

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

/**
 * Move helicopter sprite.
 * @param sprite Helicopter sprite.
 * @todo Remove local magic constants and document the code.
 */
void Micropolis::DoCopterSprite(
  SimSprite *sprite)
{
    static const short CDx[9] = { 0,  0,  3,  5,  3,  0, -3, -5, -3 };
    static const short CDy[9] = { 0, -5, -3,  0,  3,  5,  3,  0, -3 };
    short x, y;

    if (sprite->sound_count > 0) {
	sprite->sound_count--;
    }

    if (sprite->control < 0) {

	if (sprite->count > 0) {
	    sprite->count--;
	}

	if (sprite->count == 0) {

	    /* Attract copter to monster so it blows up more often */
	    SimSprite *s = GetSprite(MON);

	    if (s != NULL) {
		sprite->dest_x = s->x;
		sprite->dest_y = s->y;
	    } else {

		/* Attract copter to tornado so it blows up more often */
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

	if (sprite->count == 0) { /* land */
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

    if (sprite->sound_count == 0) { /* send report  */

	// Convert sprite coordinates to traffic density map coordinates.
	x = (sprite->x + 48) >>5;
	y = sprite->y >>5;

	if (x >= 0 && x < (WORLD_X >>1) && y >= 0 && y < (WORLD_Y >>1)) {

	    /* Don changed from 160 to 170 to shut the #$%#$% thing up! */

	    int chopperX = (x <<1) + 1;
	    int chopperY = (y <<1) + 1;
	    if ((TrfDensity[x][y] > 170) && ((Rand16() & 7) == 0)) {
		SendMesAt(-STR301_HEAVY_TRAFFIC, chopperX, chopperY);
		MakeSound("city", "HeavyTraffic", chopperX, chopperY); /* chopper */
		sprite->sound_count = 200;
	    }

	}

    }

    short z = sprite->frame;

    if ((Cycle & 3) == 0) {
	short d = GetDir(sprite->x, sprite->y, sprite->dest_x, sprite->dest_y);
	z = TurnTo(z, d);
	sprite->frame = z;
    }

    sprite->x += CDx[z];
    sprite->y += CDy[z];
}


/**
 * Move airplane sprite.
 * @param sprite Airplane sprite.
 * @todo Remove local magic constants and document the code.
 */
void Micropolis::DoAirplaneSprite(
  SimSprite *sprite)
{
    static const short CDx[12] = { 0,  0,  6, 8, 6, 0, -6, -8, -6, 8, 8, 8 };
    static const short CDy[12] = { 0, -8, -6, 0, 6, 8,  6,  0, -6, 0, 0, 0 };

    short z = sprite->frame;

    if ((Cycle % 5) == 0) {

	if (z > 8) { /* TakeOff  */
	    z--;
	    if (z < 9) {
		z = 3;
	    }
	    sprite->frame = z;
	} else { /* goto destination */
	    short d = GetDir(sprite->x, sprite->y, sprite->dest_x, sprite->dest_y);
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
	bool explode = false;

	/* Check whether another sprite is near enough to collide with */
	for (s = spriteList; s != NULL; s = s->next) {
	    if (s->frame == 0 || s == sprite) {
		/* Non-active sprite, or self: skip */
		continue;
	    }

	    if ((s->type == COP || s->type == AIR)
					  && CheckSpriteCollision(sprite, s)) {
		ExplodeSprite(s);
		explode = true;
	    }
	}

	if (explode) {
	    ExplodeSprite(sprite);
	}
    }

    sprite->x += CDx[z];
    sprite->y += CDy[z];

    if (SpriteNotInBounds(sprite)) {
	sprite->frame = 0;
    }
}


/**
 * Move ship sprite.
 * @param sprite Ship sprite.
 * @todo Remove local magic constants and document the code.
 */
void Micropolis::DoShipSprite(SimSprite *sprite)
{
    static const short BDx[9] = { 0,  0,  1,  1,  1,  0, -1, -1, -1 };
    static const short BDy[9] = { 0, -1, -1,  0,  1,  1,  1,  0, -1 };
    static const short BPx[9] = { 0,  0,  2,  2,  2,  0, -2, -2, -2 };
    static const short BPy[9] = { 0, -2, -2,  0,  2,  2,  2,  0, -2 };
    static const short BtClrTab[8] = { RIVER, CHANNEL, POWERBASE, POWERBASE + 1,
				       RAILBASE, RAILBASE + 1, BRWH, BRWV };
    short x, y, z, t = RIVER;
    short tem, pem;

    if (sprite->sound_count > 0) {
	sprite->sound_count--;
    }

    if (!sprite->sound_count) {

	if ((Rand16() & 3) == 1) {

	    // Convert sprite coordinates to tile coordinates.
	    int shipX = sprite->x >>4;
	    int shipY = sprite->y >>4;

	    if (ScenarioID == SC_SAN_FRANCISCO && Rand(10) < 5) {
		MakeSound("city", "FogHorn-Low", shipX, shipY);
	    } else {
		MakeSound("city", "HonkHonk-Low", shipX, shipY);
	    }

	}

	sprite->sound_count = 200;
    }

    if (sprite->count > 0) {
	sprite->count--;
    }

    if (sprite->count == 0) {

	sprite->count = 9;

	if (sprite->frame != sprite->new_dir) {
	    sprite->frame = TurnTo(sprite->frame, sprite->new_dir);
	    return;
	}

	tem = Rand16() & 7;

	for (pem = tem; pem < (tem + 8); pem++) {

	    z = (pem & 7) + 1;

	    if (z == sprite->dir) {
		continue;
	    }

	    x = ((sprite->x + (48 - 1)) >>4) + BDx[z];
	    y = (sprite->y >>4) + BDy[z];

	    if (TestBounds(x, y)) {

		t = Map[x][y] & LOMASK;

		if (t == CHANNEL || t == BRWH || t == BRWV
						    || TryOther(t, sprite->dir, z)) {

		    sprite->new_dir = z;
		    sprite->frame = TurnTo(sprite->frame, sprite->new_dir);
		    sprite->dir = z + 4;

		    if (sprite->dir > 8) {
			sprite->dir -= 8;
		    }

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

	if (t == BtClrTab[z]) {
	    break;
	}

	if (z == 7) {
	    ExplodeSprite(sprite);
	    Destroy(sprite->x + 48, sprite->y);
	}

    }
}


/**
 * Move monster sprite.
 * @param sprite Monster sprite.
 * @todo Remove local magic constants and document the code.
 */
void Micropolis::DoMonsterSprite(SimSprite *sprite)
{
    static const short Gx[5] = {  2,  2, -2, -2,  0 };
    static const short Gy[5] = { -2,  2,  2, -2,  0 };
    static const short ND1[4] = {  0,  1,  2,  3 };
    static const short ND2[4] = {  1,  2,  3,  0 };
    static const short nn1[4] = {  2,  5,  8, 11 };
    static const short nn2[4] = { 11,  2,  5,  8 };
    short d, z, c;

    if (sprite->sound_count > 0) {
	sprite->sound_count--;
    }

    if (sprite->control < 0) {
	/* business as usual */

	if (sprite->control == -2) {

	    d = (sprite->frame - 1) / 3;
	    z = (sprite->frame - 1) % 3;

	    if (z == 2) {
		sprite->step = 0;
	    }

	    if (z == 0) {
		sprite->step = 1;
	    }

	    if (sprite->step) {
		z++;
	    } else {
		z--;
	    }

	    c = GetDir(sprite->x, sprite->y, sprite->dest_x, sprite->dest_y);

	    if (absDist < 18) {

		sprite->control = -1;
		sprite->count = 1000;
		sprite->flag = 1;
		sprite->dest_x = sprite->orig_x;
		sprite->dest_y = sprite->orig_y;

	    } else {

		c = (c - 1) / 2;

		if ((c != d && Rand(5) == 0) || Rand(20) == 0) {

		    int diff = (c - d) & 3;

		    if (diff == 1 || diff == 3) {
			d = c;
		    } else {

			if (Rand16() & 1) {
			    d++;
			} else {
			    d--;
			}

			d &= 3;
		    }
		} else {

		    if (Rand(20) == 0) {

			if (Rand16() & 1) {
			    d++;
			} else {
			    d--;
			}

			d &= 3;
		    }
		}
	    }
	} else {

	    d = (sprite->frame - 1) / 3;

	    if (d < 4) { /* turn n s e w */

		z = (sprite->frame - 1) % 3;

		if (z == 2) {
		    sprite->step = 0;
		}

		if (z == 0) {
		    sprite->step = 1;
		}

		if (sprite->step) {
		    z++;
		} else {
		    z--;
		}

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

		    if (Rand16() & 1) {
			z = ND1[d];
		    } else {
			z = ND2[d];
		    }

		    d = 4;

		    if (!sprite->sound_count) {
			// Convert sprite coordinates to tile coordinates.
			int monsterX = sprite->x >>4;
			int monsterY = sprite->y >>4;
			MakeSound("city", "Monster", monsterX, monsterY); /* monster */
			sprite->sound_count = 50 + Rand(100);
		    }

		}

	    } else {

		d = 4;
		c = sprite->frame;
		z = (c - 13) & 3;

		if (!(Rand16() & 3)) {

		    if (Rand16() & 1) {
			z = nn1[z];
		    } else {
			z = nn2[z];
		    }

		    d = (z - 1) / 3;
		    z = (z - 1) % 3;

		}

	    }

	}

    } else {

	  /* somebody's taken control of the monster */

	  d = sprite->control;
	  z = (sprite->frame - 1) % 3;

	  if (z == 2) {
	      sprite->step = 0;
	  }

	  if (z == 0) {
	      sprite->step = 1;
	  }

	  if (sprite->step) {
	      z++;
	  } else {
	      z--;
	  }

    }

    z = d * 3 + z + 1;

    if (z > 16) {
	z = 16;
    }

    sprite->frame = z;

    sprite->x += Gx[d];
    sprite->y += Gy[d];

    if (sprite->count > 0) {
	sprite->count--;
    }

    c = GetChar(sprite->x + sprite->x_hot, sprite->y + sprite->y_hot);

    if (c == -1
	  || (c == RIVER && sprite->count != 0 && sprite->control == -1)) {
	sprite->frame = 0; /* kill scary monster */
    }

    {
	SimSprite *s;
	for (s = spriteList; s != NULL; s = s->next) {
	    if (s->frame != 0 &&
		(s->type == AIR || s->type == COP ||
		 s->type == SHI || s->type == TRA) &&
		CheckSpriteCollision(sprite, s)) {
		ExplodeSprite(s);
	    }
	}
    }

    Destroy(sprite->x + 48, sprite->y + 16);
}

/**
 * Move tornado.
 * @param sprite Tornado sprite to move.
 * @todo Remove local magic constants and document the code.
 */
void Micropolis::DoTornadoSprite(SimSprite *sprite)
{
    static const short CDx[9] = {  2,  3,  2,  0, -2, -3 };
    static const short CDy[9] = { -2,  0,  2,  3,  2,  0 };
    register short z;

    z = sprite->frame;

    if (z == 2) {

	/* cycle animation... post Rel */

	if (sprite->flag) {
	    z = 3;
	} else {
	    z = 1;
	}

    } else {

	if (z == 1) {
	    sprite->flag = 1;
	} else {
	    sprite->flag = 0;
	}

	z = 2;
    }

    if (sprite->count > 0) {
	sprite->count--;
    }

    sprite->frame = z;

    {
	SimSprite *s;
	for (s = spriteList; s != NULL; s = s->next) {
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

    if (SpriteNotInBounds(sprite)) {
	sprite->frame = 0;
    }

    if ((sprite->count != 0) &&
	(!Rand(500))) {
	sprite->frame = 0;
    }

    Destroy(sprite->x + 48, sprite->y + 40);
}


/**
 * 'Move' fire sprite.
 * @param sprite Fire sprite.
 */
void Micropolis::DoExplosionSprite(SimSprite *sprite)
{
    short x, y;

    if ((Cycle & 1) == 0) {

	if (sprite->frame == 1) {
	    // Convert sprite coordinates to tile coordinates.
	    int explosionX = sprite->x >>4;
	    int explosionY = sprite->y >>4;
	    MakeSound("city", "Explosion-High", explosionX, explosionY); /* explosion */
	    x = (sprite->x >>4) + 3;
	    y = (sprite->y >>4);
	    SendMesAt(STR301_EXPLOSION_REPORTED, x, y);
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
    }
}


/**
 * Move bus sprite.
 * @param sprite Bus sprite.
 * @todo Remove local magic constants and document the code.
 */
void Micropolis::DoBusSprite(SimSprite *sprite)
{
    static const short Dx[5] = {   0,   1,   0,  -1,   0 };
    static const short Dy[5] = {  -1,   0,   1,   0,   0 };
    static const short Dir2Frame[4] = { 1, 2, 1, 2 };
    int dx, dy, tx, ty, otx, oty;
    int turned = 0;
    int speed = 0;
    int z;

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
	if ((sprite->frame == 3) ||
	    (sprite->frame == 4)) {
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

	if (tx >= 0 && tx < (WORLD_X >>1) && ty >= 0 && ty < (WORLD_Y >>1)) {

	    z = TrfDensity[tx][ty] >>6;

	    if (z > 1) {
	      z--;
	    }

	} else {

	    z = 0;

	}

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
	if (speed > sprite->speed) {
	    speed = sprite->speed;
	}

	if (turned) {

#ifdef DEBUGBUS
	  printf("turned\n");
#endif

	    if (speed > 1) {
	        speed = 1;
	    }

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

		    if (z < 0) {
			dx = -1;
		    } else if (z > 0) {
			dx = 1;
		    }

#ifdef DEBUGBUS
		    printf("moving up x %x z %d dx %d\n", sprite->x + sprite->x_hot, z, dx);
#endif

		    break;

		case 1: /* right */

		    z = ((ty <<4) + 4) - (sprite->y + sprite->y_hot);

		    if (z < 0) {
			dy = -1;
		    } else if (z > 0) {
			dy = 1;
		    }

#ifdef DEBUGBUS
		    printf("moving right y %x z %d dy %d\n", sprite->y + sprite->y_hot, z, dy);
#endif

		    break;

		case 2: /* down */

		    z = (tx <<4) - (sprite->x + sprite->x_hot);

		    if (z < 0) {
			dx = -1;
		    } else if (z > 0) {
			dx = 1;
		    }

#ifdef DEBUGBUS
		    printf("moving down x %x z %d dx %d\n", sprite->x + sprite->x_hot, z, dx);
#endif

		    break;

		case 3: /* left */

		    z = (ty <<4) - (sprite->y + sprite->y_hot);

		    if (z < 0) {
			dy = -1;
		    } else if (z > 0) {
			dy = 1;
		    }

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

    otx = clamp(otx, 0, WORLD_X - 1);
    oty = clamp(oty, 0, WORLD_Y - 1);

    tx = (sprite->x + sprite->x_hot + dx + (Dx[sprite->dir] * AHEAD)) >>4;
    ty = (sprite->y + sprite->y_hot + dy + (Dy[sprite->dir] * AHEAD)) >>4;

    tx = clamp(tx, 0, WORLD_X - 1);
    ty = clamp(ty, 0, WORLD_Y - 1);

    if (tx != otx || ty != oty) {

#ifdef DEBUGBUS
	printf("drive from tile %d %d to %d %d\n",
	       otx, oty, tx, ty);
#endif

	z = CanDriveOn(tx, ty);

	if (z == 0) {

	    /* can't drive forward into a new tile */
	    if (speed == 8) {
		bulldozerTool(tx, ty);
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

	for (s = spriteList; s != NULL; s = s->next) {
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

	if (explode) {
	    ExplodeSprite(sprite);
	}

    }
}


/**
 * Can one drive at the specified tile?
 * @param x X coordinate at map.
 * @param y Y coordinate at map.
 * @return 0 if not, 1 if you can, -1 otherwise
 */
int Micropolis::CanDriveOn(int x, int y)
{
    int tile;

    if (!TestBounds(x, y)) {
	return 0;
    }

    tile = Map[x][y] & LOMASK;

    if ((tile >= ROADBASE && tile <= LASTROAD && tile != BRWH && tile != BRWV)
	      || tile == HRAILROAD || tile == VRAILROAD) {
	return 1;
    }

    if (tile == DIRT || tally(tile)) {
	return -1;
    }

    return 0;
}


/**
 * Handle explosion of sprite (mostly due to collision?).
 * @param sprite that should explode.
 * @todo Add a 'bus crashed' message to #Stri301.
 */
void Micropolis::ExplodeSprite(SimSprite *sprite)
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
	    SendMesAt(-STR301_PLANE_CRASHED, x, y);
	    break;

	case SHI:
	    CrashX = x;
	    CrashY = y;
	    SendMesAt(-STR301_SHIP_CRASHED, x, y);
	    break;

	case TRA:
	    CrashX = x;
	    CrashY = y;
	    SendMesAt(-STR301_TRAIN_CRASHED, x, y);
	    break;

	case COP:
	    CrashX = x;
	    CrashY = y;
	    SendMesAt(-STR301_HELICOPTER_CRASHED, x, y);
	    break;

	case BUS:
	    CrashX = x;
	    CrashY = y;
	    SendMesAt(-STR301_TRAIN_CRASHED, x, y); /* XXX for now */
	    break;

    }

    // Convert sprite coordinates to tile coordinates.
    MakeSound("city", "Explosion-High", x, y); /* explosion */

    return;
}


bool Micropolis::checkWet(int x)
{
    if (x == HPOWER || x == VPOWER || x == HRAIL || x == VRAIL
	      || x == BRWH || x == BRWV) {
	return true;
    } else {
	return false;
    }
}


/**
 * Destroy a map tile.
 * @param ox X coordinate in pixels.
 * @param oy Y coordinate in pixels.
 */
void Micropolis::Destroy(int ox, int oy)
{
    short t, z, x, y;

    x = ox >>4;
    y = oy >>4;

    if (!TestBounds(x, y)) {
	return;
    }

    z = Map[x][y];
    t = z & LOMASK;

    if (t >= TREEBASE) {
	/* TILE_IS_BRIDGE(t) */
	if (!(z & BURNBIT)) {

	  if (t >= ROADBASE && t <= LASTROAD) {
	      Map[x][y] = RIVER;
	  }

	  return;
	}

	if (z & ZONEBIT) {

	    OFireZone(x, y, z);

	    if (t > RZB) {
		MakeExplosionAt(ox, oy);
	    }

	}

	if (checkWet(t)) {
	    Map[x][y] = RIVER;
	} else {
	    Map[x][y] = (DoAnimation ? TINYEXP : (LASTTINYEXP - 3))
		      | BULLBIT | ANIMBIT;
	}
    }
}


/**
 * Start a fire in a zone.
 * @param Xloc X coordinate in map coordinate.
 * @param Yloc Y coordinate in map coordinate.
 * @param ch   Map character at (\a Xloc, \a Yloc).
 */
void Micropolis::OFireZone(int Xloc, int Yloc, int ch)
{
    short Xtem, Ytem;
    short x, y, XYmax;

    RateOGMem[Xloc >>3][Yloc >>3] -= 20;

    ch &= LOMASK;

    if (ch < PORTBASE) {
	XYmax = 2;
    } else {
	if (ch == AIRPORT) {
	    XYmax = 5;
	} else {
	    XYmax = 4;
	}
    }

    for (x = -1; x < XYmax; x++) {
	for (y = -1; y < XYmax; y++) {

	    Xtem = Xloc + x;
	    Ytem = Yloc + y;

	    if (TestBounds(Xtem, Ytem) && (Map[Xtem][Ytem] & LOMASK) >= ROADBASE) {
		Map[Xtem][Ytem] |= BULLBIT;
	    }

	}
    }
}


/**
 * Start a fire at a single tile.
 * @param x X coordinate in map coordinate.
 * @param y Y coordinate in map coordinate.
 */
void Micropolis::StartFire(int x, int y)
{
    int t, z;

    x >>= 4;
    y >>= 4;

    if (!TestBounds(x, y)) {
	return;
    }

    z = Map[x][y];
    t = z & LOMASK;

    if (!(z & BURNBIT) && t != DIRT) {
	return;
    }

    if (z & ZONEBIT) {
	return;
    }

    Map[x][y] = RandomFire();
}


/**
 * Try to start a new train sprite at the given map tile.
 * @param x X coordinate in map coordinate.
 * @param y Y coordinate in map coordinate.
 */
void Micropolis::GenerateTrain(int x, int y)
{
    if (TotalPop > 20 && GetSprite(TRA) == NULL && Rand(25) == 0) {
	MakeSprite(TRA, (x <<4) + TRA_GROOVE_X, (y <<4) + TRA_GROOVE_Y);
    }
}


/**
 * Try to start a new bus sprite at the given map tile.
 * @param x X coordinate in map coordinate.
 * @param y Y coordinate in map coordinate.
 */
void Micropolis::GenerateBus(int x, int y)
{
    if (GetSprite(BUS) == NULL && Rand(25) == 0) {
	MakeSprite(BUS, (x <<4) + BUS_GROOVE_X, (y <<4) + BUS_GROOVE_Y);
    }
}


/** Try to construct a new ship sprite */
void Micropolis::GenerateShip()
{
    short x, y;

    if (!(Rand16() & 3)) {
	for (x = 4; x < WORLD_X - 2; x++) {
	    if (Map[x][0] == CHANNEL)  {
		MakeShipHere(x, 0);
		return;
	    }
	}
    }

    if (!(Rand16() & 3)) {
	for (y = 1; y < WORLD_Y - 2; y++) {
	    if (Map[0][y] == CHANNEL)  {
		MakeShipHere(0, y);
		return;
	    }
	}
    }

    if (!(Rand16() & 3)) {
	for (x = 4; x < WORLD_X - 2; x++) {
	    if (Map[x][WORLD_Y - 1] == CHANNEL)  {
		MakeShipHere(x, WORLD_Y - 1);
		return;
	    }
	}
    }

    if (!(Rand16() & 3)) {
	for (y = 1; y < WORLD_Y - 2; y++) {
	    if (Map[WORLD_X - 1][y] == CHANNEL)  {
		MakeShipHere(WORLD_X - 1, y);
		return;
	    }
	}
    }
}


/**
 * Start a new ship sprite at the given map tile.
 * @param x X coordinate in map coordinate.
 * @param y Y coordinate in map coordinate.
 */
void Micropolis::MakeShipHere(int x, int y)
{
    MakeSprite(SHI, (x <<4) - (48 - 1), (y <<4));
}


/**
 * Start a new monster sprite.
 * @bug The "!done == 0" looks wrong.
 */
void Micropolis::MakeMonster()
{
    int x, y, z, done = 0;
    SimSprite *sprite;

    sprite = GetSprite(MON);
    if (sprite != NULL) {
	sprite->sound_count = 1;
	sprite->count = 1000;
	sprite->dest_x = PolMaxX <<4;
	sprite->dest_y = PolMaxY <<4;
	return;
    }

    for (z = 0; z < 300; z++)  {

	x = Rand(WORLD_X - 20) + 10;
	y = Rand(WORLD_Y - 10) + 5;

	if (Map[x][y] == RIVER || Map[x][y] == RIVER + BULLBIT) {
	    MonsterHere(x, y);
	    done = 1;
	    break;
	}

    }

    if (!done == 0) {
	MonsterHere(60, 50);
    }

}


/**
 * Start a new monster sprite at the given map tile.
 * @param x X coordinate in map coordinate.
 * @param y Y coordinate in map coordinate.
 */
void Micropolis::MonsterHere(int x, int y)
{
    MakeSprite(MON, (x << 4) + 48, (y << 4));
    ClearMes();
    SendMesAt(-STR301_MONSTER_SIGHTED, x + 5, y);
}


/**
 * Ensure a helicopter sprite exists.
 *
 * If it does not exist, create one at the given coordinates.
 * @param x X coordinate in map coordinate.
 * @param y Y coordinate in map coordinate.
 */
void Micropolis::GenerateCopter(int x, int y)
{
    if (GetSprite(COP) != NULL) {
	return;
    }

    MakeSprite(COP, (x << 4), (y << 4) + 30);
}


/**
 * Ensure a airplane sprite exists.
 *
 * If it does not exist, create one at the given coordinates.
 * @param x X coordinate in map coordinate.
 * @param y Y coordinate in map coordinate.
 */
void Micropolis::GeneratePlane(int x, int y)
{
    if (GetSprite(AIR) != NULL) {
	return;
    }

    MakeSprite(AIR, (x <<4) + 48, (y <<4) + 12);
}


/** Ensure a airplane sprite exists. */
void Micropolis::MakeTornado()
{
    short x, y;
    SimSprite *sprite;

    sprite = GetSprite(TOR);
    if (sprite != NULL) {
	sprite->count = 200;
	return;
    }

    x = Rand((WORLD_X <<4) - 800) + 400;
    y = Rand((WORLD_Y <<4) - 200) + 100;

    MakeSprite(TOR, x, y);
    ClearMes();
    SendMesAt(-STR301_TORNADO_SIGHTED, (x >>4) + 3, (y >>4) + 2);
}


/**
 * Construct an explosion sprite.
 * @param x X coordinate of the explosion (in map coordinates).
 * @param y Y coordinate of the explosion (in map coordinates).
 */
void Micropolis::MakeExplosion(int x, int y)
{
    if (TestBounds(x, y)) {
	MakeExplosionAt((x << 4) + 8, (y << 4) + 8);
    }
}


/**
 * Construct an explosion sprite.
 * @param x X coordinate of the explosion (in pixels).
 * @param y Y coordinate of the explosion (in pixels).
 */
void Micropolis::MakeExplosionAt( int x, int y)
{
    NewSprite("", EXP, x - 40, y - 16);
}


////////////////////////////////////////////////////////////////////////
