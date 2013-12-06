// This file is part of MicropolisJ.
// Copyright (C) 2013 Jason Long
// Portions Copyright (C) 1989-2007 Electronic Arts Inc.
//
// MicropolisJ is free software; you can redistribute it and/or modify
// it under the terms of the GNU GPLv3, with additional terms.
// See the README file, included in this distribution, for details.

package micropolisj.engine;

import static micropolisj.engine.TileConstants.*;

class Bulldozer extends ToolStroke
{
	Bulldozer(Micropolis city, int xpos, int ypos)
	{
		super(city, MicropolisTool.BULLDOZER, xpos, ypos);
	}

	@Override
	protected void applyArea(ToolEffectIfc eff)
	{
		CityRect b = getBounds();

		// scan selection area for rubble, forest, etc...
		for (int y = 0; y < b.height; y++) {
			for (int x = 0; x < b.width; x++) {

				ToolEffectIfc subEff = new TranslatedToolEffect(eff, b.x+x, b.y+y);
				if (city.isTileDozeable(subEff)) {

					dozeField(subEff);
				}

			}
		}

		// scan selection area for zones...
		for (int y = 0; y < b.height; y++) {
			for (int x = 0; x < b.width; x++) {

				if (isZoneCenter(eff.getTile(b.x+x,b.y+y))) {
					dozeZone(new TranslatedToolEffect(eff, b.x+x, b.y+y));
				}
			}
		}
	}

	void dozeZone(ToolEffectIfc eff)
	{
		int currTile = eff.getTile(0, 0);

		// zone center bit is set
		assert isZoneCenter(currTile);

		CityDimension dim = getZoneSizeFor(currTile);
		assert dim != null;
		assert dim.width >= 3;
		assert dim.height >= 3;

		eff.spend(1);

		// make explosion sound;
		// bigger zones => bigger explosions

		if (dim.width * dim.height < 16) {
			eff.makeSound(0, 0, Sound.EXPLOSION_HIGH);
		}
		else if (dim.width * dim.height < 36) {
			eff.makeSound(0, 0, Sound.EXPLOSION_LOW);
		}
		else {
			eff.makeSound(0, 0, Sound.EXPLOSION_BOTH);
		}

		putRubble(new TranslatedToolEffect(eff, -1, -1), dim.width, dim.height);
		return;
	}

	void dozeField(ToolEffectIfc eff)
	{
		int tile = eff.getTile(0, 0);

		if (isOverWater(tile))
		{
			// dozing over water, replace with water.
			eff.setTile(0, 0, RIVER);
		}
		else
		{
			// dozing on land, replace with land. Simple, eh?
			eff.setTile(0, 0, DIRT);
		}

		fixZone(eff);
		eff.spend(1);
		return;
	}

	void putRubble(ToolEffectIfc eff, int w, int h)
	{
		for (int yy = 0; yy < h; yy++) {
			for (int xx = 0; xx < w; xx++) {
				int tile = eff.getTile(xx,yy);
				if (tile == CLEAR)
					continue;

				if (tile != RADTILE && tile != DIRT) {
					int z = inPreview ? 0 : city.PRNG.nextInt(3);
					int nTile = TINYEXP + z;
					eff.setTile(xx, yy, nTile);
				}
			}
		}
		fixBorder(eff, w, h);
	}
}
