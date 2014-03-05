// This file is part of MicropolisJ.
// Copyright (C) 2013 Jason Long
// Portions Copyright (C) 1989-2007 Electronic Arts Inc.
//
// MicropolisJ is free software; you can redistribute it and/or modify
// it under the terms of the GNU GPLv3, with additional terms.
// See the README file, included in this distribution, for details.

package micropolisj.engine;

import static micropolisj.engine.TileConstants.*;

/**
 * Implements an explosion.
 * An explosion occurs when certain sprites collide,
 * or when a zone is demolished by fire.
 */
public class ExplosionSprite extends Sprite
{
	public ExplosionSprite(Micropolis engine, int x, int y)
	{
		super(engine, SpriteKind.EXP);
		this.x = x;
		this.y = y;
		this.width = 48;
		this.height = 48;
		this.offx = -24;
		this.offy = -24;
		this.frame = 1;
	}

	@Override
	public void moveImpl()
	{
		if (city.acycle % 2 == 0) {
			if (this.frame == 1) {
				city.makeSound(x/16, y/16, Sound.EXPLOSION_HIGH);
				city.sendMessageAt(MicropolisMessage.EXPLOSION_REPORT, x/16, y/16);
			}
			this.frame++;
		}

		if (this.frame > 6) {
			this.frame = 0;

			startFire(x/16, y/16);
			startFire(x/16-1, y/16-1);
			startFire(x/16+1, y/16-1);
			startFire(x/16-1, y/16+1);
			startFire(x/16+1, y/16+1);
			return;
		}
	}

	void startFire(int xpos, int ypos)
	{
		if (!city.testBounds(xpos, ypos))
			return;

		int t = city.getTile(xpos, ypos);
		if (!isCombustible(t) && t != DIRT)
			return;
		if (isZoneCenter(t))
			return;
		city.setTile(xpos, ypos, (char)(FIRE + city.PRNG.nextInt(4)));
	}
}
