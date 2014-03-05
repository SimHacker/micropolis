// This file is part of MicropolisJ.
// Copyright (C) 2013 Jason Long
// Portions Copyright (C) 1989-2007 Electronic Arts Inc.
//
// MicropolisJ is free software; you can redistribute it and/or modify
// it under the terms of the GNU GPLv3, with additional terms.
// See the README file, included in this distribution, for details.

package micropolisj.engine;

/**
 * Implements the helicopter.
 * The helicopter appears if the city contains an airport.
 * It usually flies to the location in the city with the highest
 * traffic density, but sometimes flies to other locations.
 */
public class HelicopterSprite extends Sprite
{
	int count;
	int destX;
	int destY;
	int origX;
	int origY;

	static int [] CDx = { 0,  0,  3,  5,  3,  0, -3, -5, -3 };
	static int [] CDy = { 0, -5, -3,  0,  3,  5,  3,  0, -3 };
	static final int SOUND_FREQ = 200;

	public HelicopterSprite(Micropolis engine, int xpos, int ypos)
	{
		super(engine, SpriteKind.COP);
		this.x = xpos * 16 + 8;
		this.y = ypos * 16 + 8;
		this.width = 32;
		this.height = 32;
		this.offx = -16;
		this.offy = -16;

		this.destX = city.PRNG.nextInt(city.getWidth()) * 16 + 8;
		this.destY = city.PRNG.nextInt(city.getHeight()) * 16 + 8;

		this.origX = x;
		this.origY = y;
		this.count = 1500;
		this.frame = 5;
	}

	@Override
	public void moveImpl()
	{
		if (this.count > 0) {
			this.count--;
		}

		if (this.count == 0) {

			// attract copter to monster and tornado so it blows up more often
			if (city.hasSprite(SpriteKind.GOD)) {

				MonsterSprite monster = (MonsterSprite) city.getSprite(SpriteKind.GOD);
				this.destX = monster.x;
				this.destY = monster.y;

			}
			else if (city.hasSprite(SpriteKind.TOR)) {

				TornadoSprite tornado = (TornadoSprite) city.getSprite(SpriteKind.TOR);
				this.destX = tornado.x;
				this.destY = tornado.y;

			}
			else {
				this.destX = origX;
				this.destY = origY;
			}

			if (getDis(x, y, origX, origY) < 30) {
				// made it back to airport, go ahead and land.
				this.frame = 0;
				return;
			}
		}

		if (city.acycle % SOUND_FREQ == 0) {
			// send report, if hovering over high traffic area
			int xpos = this.x / 16;
			int ypos = this.y / 16;

			if (city.getTrafficDensity(xpos, ypos) > 170 &&
				city.PRNG.nextInt(8) == 0)
			{
				city.sendMessageAt(MicropolisMessage.HEAVY_TRAFFIC_REPORT,
					xpos, ypos);
				city.makeSound(xpos, ypos, Sound.HEAVYTRAFFIC);
			}
		}

		int z = this.frame;
		if (city.acycle % 3 == 0) {
			int d = getDir(x, y, destX, destY);
			z = turnTo(z, d);
			this.frame = z;
		}
		x += CDx[z];
		y += CDy[z];
	}
}
