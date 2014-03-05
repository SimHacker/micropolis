// This file is part of MicropolisJ.
// Copyright (C) 2013 Jason Long
// Portions Copyright (C) 1989-2007 Electronic Arts Inc.
//
// MicropolisJ is free software; you can redistribute it and/or modify
// it under the terms of the GNU GPLv3, with additional terms.
// See the README file, included in this distribution, for details.

package micropolisj.engine;

/**
 * Implements a tornado (one of the Micropolis disasters).
 */
public class TornadoSprite extends Sprite
{
	static int [] CDx = {  2,  3,  2,  0, -2, -3 };
	static int [] CDy = { -2,  0,  2,  3,  2,  0 };

	boolean flag;
	int count;

	public TornadoSprite(Micropolis engine, int xpos, int ypos)
	{
		super(engine, SpriteKind.TOR);
		this.x = xpos * 16 + 8;
		this.y = ypos * 16 + 8;
		this.width = 48;
		this.height = 48;
		this.offx = -24;
		this.offy = -40;

		this.frame = 1;
		this.count = 200;
	}

	@Override
	public void moveImpl()
	{
		int z = this.frame;

		if (z == 2) {
			//cycle animation

			if (this.flag)
				z = 3;
			else
				z = 1;
		}
		else {
			this.flag = (z == 1);
			z = 2;
		}

		if (this.count > 0) {
			this.count--;
		}

		this.frame = z;

		for (Sprite s : city.allSprites()) {
			if (checkSpriteCollision(s) &&
				(s.kind == SpriteKind.AIR ||
				 s.kind == SpriteKind.COP ||
				 s.kind == SpriteKind.SHI ||
				 s.kind == SpriteKind.TRA)
				) {
				s.explodeSprite();
			}
		}

		int zz = city.PRNG.nextInt(CDx.length);
		x += CDx[zz];
		y += CDy[zz];

		if (!city.testBounds(x/16, y/16)) {
			// out of bounds
			this.frame = 0;
			return;
		}

		if (this.count == 0 && city.PRNG.nextInt(501) == 0) {
			// early termination
			this.frame = 0;
			return;
		}

		destroyTile(x/16, y/16);
	}
}
