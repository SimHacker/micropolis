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
 * Implements the cargo ship.
 * The cargo ship is created if the city contains a sea port.
 * It follows the river "channel" that was originally generated.
 * It frequently turns around.
 */
public class ShipSprite extends Sprite
{
	static int [] BDx = {  0,  0,  1,  1,  1,  0, -1, -1, -1 };
	static int [] BDy = {  0, -1, -1,  0,  1,  1,  1,  0, -1 };
	static int [] BPx = {  0,  0,  2,  2,  2,  0, -2, -2, -2 };
	static int [] BPy = {  0, -2, -2,  0,  2,  2,  2,  0, -2 };
	static int [] BtClrTab = { RIVER, CHANNEL, POWERBASE, POWERBASE+1,
			RAILBASE, RAILBASE+1, BRWH, BRWV };

	int newDir;
	int count;
	int soundCount;

	public static final int NORTH_EDGE = 5;
	public static final int EAST_EDGE = 7;
	public static final int SOUTH_EDGE = 1;
	public static final int WEST_EDGE = 3;

	public ShipSprite(Micropolis engine, int xpos, int ypos, int edge)
	{
		super(engine, SpriteKind.SHI);
		this.x = xpos * 16 + 8;
		this.y = ypos * 16 + 8;
		this.width = 48;
		this.height = 48;
		this.offx = -24;
		this.offy = -24;
		this.frame = edge;
		this.newDir = edge;
		this.dir = 10;
		this.count = 1;
	}

	@Override
	public void moveImpl()
	{
		int t = RIVER;

		this.soundCount--;
		if (this.soundCount <= 0) {
			if (city.PRNG.nextInt(4) == 0) {
				city.makeSound(x/16,y/16,Sound.HONKHONK_LOW);
			}
			this.soundCount = 200;
		}

		this.count--;
		if (this.count <= 0) {
			this.count = 9;
			if (this.newDir != this.frame) {
				this.frame = turnTo(this.frame, this.newDir);
				return;
			}
			int tem = city.PRNG.nextInt(8);
			int pem;
			for (pem = tem; pem < (tem + 8); pem++) {
				int z = (pem % 8) + 1;
				if (z == this.dir)
					continue;
	
				int xpos = this.x / 16 + BDx[z];
				int ypos = this.y / 16 + BDy[z];
	
				if (city.testBounds(xpos, ypos)) {
					t = city.getTile(xpos, ypos);
					if ((t == CHANNEL) || (t == BRWH) || (t == BRWV) ||
						tryOther(t, this.dir, z))
					{
						this.newDir = z;
						this.frame = turnTo(this.frame, this.newDir);
						this.dir = z + 4;
						if (this.dir > 8) { this.dir -= 8; }
						break;
					}
				}
			}
	
			if (pem == (tem + 8)) {
				this.dir = 10;
				this.newDir = city.PRNG.nextInt(8)+1;
			}
		}
		else {
			int z = this.frame;
			if (z == this.newDir) {
				this.x += BPx[z];
				this.y += BPy[z];
			}
		}

		if (!spriteInBounds()) {
			this.frame = 0;
			return;
		}

		boolean found = false;
		for (int z : BtClrTab) {
			if (t == z) {
				found = true;
			}
		}
		if (!found) {
			if (!city.noDisasters) {
			explodeSprite();
			destroyTile(x/16, y/16);
			}
		}
	}

	boolean tryOther(int tile, int oldDir, int newDir)
	{
		int z = oldDir + 4;
		if (z > 8) z -= 8;
		if (newDir != z) return false;

		return (tile == POWERBASE || tile == POWERBASE+1 ||
			tile == RAILBASE || tile == RAILBASE+1);
	}

	boolean spriteInBounds()
	{
		int xpos = x / 16;
		int ypos = y / 16;
		return city.testBounds(xpos, ypos);
	}
}
