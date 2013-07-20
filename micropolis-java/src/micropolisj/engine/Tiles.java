package micropolisj.engine;

import java.io.*;
import java.nio.charset.Charset;
import java.util.*;

public class Tiles
{
	static final Charset UTF8 = Charset.forName("UTF-8");
	static TileSpec [] tiles = new TileSpec[960];
	static {
		try {
			readTiles();
		}
		catch (IOException e) {
			throw new RuntimeException(e);
		}
	}

	static void readTiles()
		throws IOException
	{
		tiles = new TileSpec[960];

		Properties tilesRc = new Properties();
		tilesRc.load(
			new InputStreamReader(
				Tiles.class.getResourceAsStream("/tiles.rc"),
				UTF8
				)
			);

		for (int i = 0; i < tiles.length; i++) {
			String tileName = Integer.toString(i);
			String rawSpec = tilesRc.getProperty(tileName);
			if (rawSpec == null) {
				continue;
			}

			tiles[i] = TileSpec.parse(i, rawSpec, tilesRc);
		}

		for (int i = 0; i < tiles.length; i++) {
			String tmp = tiles[i].getAttribute("becomes");
			if (tmp != null) {
				tiles[i].animNext = get(Integer.parseInt(tmp));
			}
			tmp = tiles[i].getAttribute("onpower");
			if (tmp != null) {
				tiles[i].onPower = get(Integer.parseInt(tmp));
			}
		}
	}

	public static TileSpec get(int tileNumber)
	{
		if (tileNumber >= 0 && tileNumber < tiles.length) {
			return tiles[tileNumber];
		}
		else {
			return null;
		}
	}
}
