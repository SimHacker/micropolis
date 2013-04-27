// This file is part of MicropolisJ.
// Copyright (C) 2013 Jason Long
// Portions Copyright (C) 1989-2007 Electronic Arts Inc.
//
// MicropolisJ is free software; you can redistribute it and/or modify
// it under the terms of the GNU GPLv3, with additional terms.
// See the README file, included in this distribution, for details.

package micropolisj.engine;

import static micropolisj.engine.TileConstants.*;

public class TrainSprite extends Sprite
{
	static int [] Cx = { 0, 16, 0, -16 };
	static int [] Cy = { -16, 0, 16, 0 };
	static int [] Dx = { 0, 4, 0, -4, 0 };
	static int [] Dy = { -4, 0, 4, 0, 0 };
	static int [] TrainPic2 = { 1, 2, 1, 2, 5 };
	static final int TRA_GROOVE_X = 8;
	static final int TRA_GROOVE_Y = 8;

	static final int FRAME_NORTHSOUTH = 1;
	static final int FRAME_EASTWEST = 2;
	static final int FRAME_NW_SE = 3;
	static final int FRAME_SW_NE = 4;
	static final int FRAME_UNDERWATER = 5;

	public TrainSprite(Micropolis engine, int xpos, int ypos)
	{
		super(engine, SpriteKind.TRA);
		this.x = xpos * 16 + TRA_GROOVE_X;
		this.y = ypos * 16 + TRA_GROOVE_Y;
		this.offx = -16;
		this.offy = -16;
		this.dir = 4;   //not moving
	}

	@Override
	public void moveImpl()
	{
		if (frame == 3 || frame == 4) {
			frame = TrainPic2[this.dir];
		}
		x += Dx[this.dir];
		y += Dy[this.dir];
		if (city.acycle % 4 == 0) {
			// should be at the center of a cell, if not, correct it
			x = (x/16) * 16 + TRA_GROOVE_X;
			y = (y/16) * 16 + TRA_GROOVE_Y;
			int d1 = city.PRNG.nextInt(4);
			for (int z = d1; z < d1 + 4; z++) {
				int d2 = z % 4;
				if (this.dir != 4) { //impossible?
					if (d2 == (this.dir + 2) % 4)
						continue;
				}

				int c = getChar(this.x + Cx[d2], this.y + Cy[d2]);
				if (((c >= RAILBASE) && (c <= LASTRAIL)) || //track?
					(c == RAILVPOWERH) ||
					(c == RAILHPOWERV))
				{
					if ((this.dir != d2) && (this.dir != 4)) {
						if (this.dir + d2 == 3)
							this.frame = 3;
						else
							this.frame = 4;
					}
					else {
						this.frame = TrainPic2[d2];
					}

					if ((c == RAILBASE) || (c == (RAILBASE+1))) {
						//underwater
						this.frame = 5;
					}
					this.dir = d2;
					return;
				}
			}
			if (this.dir == 4) {
				// train has nowhere to go, so retire
				this.frame = 0;
				return;
			}
			this.dir = 4;
		}
	}
}
