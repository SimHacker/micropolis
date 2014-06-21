// This file is part of MicropolisJ.
// Copyright (C) 2013 Jason Long
// Portions Copyright (C) 1989-2007 Electronic Arts Inc.
//
// MicropolisJ is free software; you can redistribute it and/or modify
// it under the terms of the GNU GPLv3, with additional terms.
// See the README file, included in this distribution, for details.

package micropolisj.engine;

import java.util.Random;
import static micropolisj.engine.TileConstants.*;

public abstract class TileBehavior
{
	protected final Micropolis city;
	protected final Random PRNG;
	int xpos;
	int ypos;
	int tile;

	protected TileBehavior(Micropolis city)
	{
		this.city = city;
		this.PRNG = city.PRNG;
	}

	public final void processTile(int xpos, int ypos)
	{
		this.xpos = xpos;
		this.ypos = ypos;
		this.tile = city.getTile(xpos, ypos);
		apply();
	}

	/**
	 * Activate the tile identified by xpos and ypos properties.
	 */
	public abstract void apply();
}
