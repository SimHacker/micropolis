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

		eff.spend(1);
		switch (checkSize(currTile))
		{
		case 3:
			eff.makeSound(0, 0, Sound.EXPLOSION_HIGH);
			putRubble(new TranslatedToolEffect(eff, -1, -1), 3, 3);
			break;
		case 4:
			eff.makeSound(0, 0, Sound.EXPLOSION_LOW);
			putRubble(new TranslatedToolEffect(eff, -1, -1), 4, 4);
			break;
		case 6:
			eff.makeSound(0, 0, Sound.EXPLOSION_BOTH);
			putRubble(new TranslatedToolEffect(eff, -1, -1), 6, 6);
			break;
		default:
			assert false;
			break;
		}
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
					int nTile = (TINYEXP + z) | ANIMBIT | BULLBIT;
					eff.setTile(xx, yy, nTile);
				}
			}
		}
	}

	int checkSize(int tile)
	{
		tile = tile & LOMASK;
		if ((tile >= (RESBASE-1) && tile <= (PORTBASE-1)) ||
			(tile >= (LASTPOWERPLANT+1) && tile <= (POLICESTATION+4)))
		{
			return 3;
		}
		else if ((tile >= PORTBASE && tile <= LASTPORT) ||
			(tile >= COALBASE && tile <= LASTPOWERPLANT) ||
			(tile >= STADIUMBASE && tile <= LASTZONE))
		{
			return 4;
		}
		else if (tile == TileConstants.AIRPORT)
		{
			return 6;
		}
		else
		{
			return 0;
		}
	}
}
