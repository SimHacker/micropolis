// This file is part of MicropolisJ.
// Copyright (C) 2013 Jason Long
// Portions Copyright (C) 1989-2007 Electronic Arts Inc.
//
// MicropolisJ is free software; you can redistribute it and/or modify
// it under the terms of the GNU GPLv3, with additional terms.
// See the README file, included in this distribution, for details.

package micropolisj.engine;

/**
 * Implements the airplane.
 * The airplane appears if the city contains an airport.
 * It first takes off, then flies around randomly,
 * occassionally crashing.
 */
public class AirplaneSprite extends Sprite
{
	int destX;
	int destY;

	// Note: frames 1-8 used for regular movement
	//    9-11 used for Taking off
	static int [] CDx = { 0,  0,  6,  8,  6,  0, -6, -8, -6,  8,  8,  8 };
	static int [] CDy = { 0, -8, -6,  0,  6,  8,  6,  0, -6,  0,  0,  0 };

	public AirplaneSprite(Micropolis engine, int xpos, int ypos)
	{
		super(engine, SpriteKind.AIR);
		this.x = xpos * 16 + 8;
		this.y = ypos * 16 + 8;
		this.width = 48;
		this.height = 48;
		this.offx = -24;
		this.offy = -24;

		this.destY = this.y;
		if (xpos > engine.getWidth()-20) {
			// not enough room to east of airport for taking off
			this.destX = x - 200;
			this.frame = 7;
		}
		else {
			this.destX = x + 200;
			this.frame = 11;
		}
	}

	@Override
	public void moveImpl()
	{
		int z = this.frame;

		if (city.acycle % 5 == 0) {
			if (z > 8) { //plane is still taking off
				z--;
				if (z < 9) { z = 3; }
				this.frame = z;
			}
			else { // go to destination
				int d = getDir(x, y, destX, destY);
				z = turnTo(z, d);
				this.frame = z;
			}
		}

		if (getDis(x, y, destX, destY) < 50) {		// at destination
			//FIXME- original code allows destination to be off-the-map
			destX = city.PRNG.nextInt(city.getWidth()) * 16 + 8;
			destY = city.PRNG.nextInt(city.getHeight()) * 16 + 8;
		}

		if (!city.noDisasters) {
			boolean explode = false;

			for (Sprite s : city.allSprites()) {
				if (s != this &&
					(s.kind == SpriteKind.AIR || s.kind == SpriteKind.COP) &&
					checkSpriteCollision(s))
				{
					s.explodeSprite();
					explode = true;
				}
			}
			if (explode) {
				explodeSprite();
			}
		}

		this.x += CDx[z];
		this.y += CDy[z];
	}
}
