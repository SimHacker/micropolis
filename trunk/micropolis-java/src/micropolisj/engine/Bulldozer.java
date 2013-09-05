package micropolisj.engine;

import java.awt.Rectangle;
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
		Rectangle b = getBounds();

		// scan selection area for rubble, forest, etc...
		for (int y = 0; y < b.height; y++) {
			for (int x = 0; x < b.width; x++) {

				int tile = eff.getTile(b.x+x, b.y+y);
				if (isDozeable(tile) && !isZoneCenter(tile)) {

					dozeField(new TranslatedToolEffect(eff, b.x+x, b.y+y));
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

		// check dozeable bit
		assert isDozeable(tile);

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
				tile = tile & LOMASK;
				if (tile != RADTILE && tile != DIRT) {
					int z = inPreview ? 0 : city.PRNG.nextInt(3);
					int nTile = (TINYEXP + z) | BULLBIT;
					eff.setTile(xx, yy, nTile);
				}
			}
		}
		fixBorder(eff, w, h);
	}
}
