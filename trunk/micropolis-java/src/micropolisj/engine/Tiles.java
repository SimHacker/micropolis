// This file is part of MicropolisJ.
// Copyright (C) 2013 Jason Long
// Portions Copyright (C) 1989-2007 Electronic Arts Inc.
//
// MicropolisJ is free software; you can redistribute it and/or modify
// it under the terms of the GNU GPLv3, with additional terms.
// See the README file, included in this distribution, for details.

package micropolisj.engine;

import java.io.*;
import java.nio.charset.Charset;
import java.util.*;

public class Tiles
{
	static final Charset UTF8 = Charset.forName("UTF-8");
	static TileSpec [] tiles;
	static Map<String,TileSpec> tilesByName = new HashMap<String,TileSpec>();
	static {
		try {
			readTiles();
			checkTiles();
		}
		catch (IOException e) {
			throw new RuntimeException(e);
		}
	}

	static void readTiles()
		throws IOException
	{
		ArrayList<TileSpec> tilesList = new ArrayList<TileSpec>();

		Properties tilesRc = new Properties();
		tilesRc.load(
			new InputStreamReader(
				Tiles.class.getResourceAsStream("/tiles.rc"),
				UTF8
				)
			);

		for (int i = 0; ; i++) {
			String tileName = Integer.toString(i);
			String rawSpec = tilesRc.getProperty(tileName);
			if (rawSpec == null) {
				break;
			}

			TileSpec ts = TileSpec.parse(i, rawSpec, tilesRc);
			tilesByName.put(tileName, ts);
			tilesList.add(ts);
		}
		tiles = tilesList.toArray(new TileSpec[0]);

		for (int i = 0; i < tiles.length; i++) {
			String tmp = tiles[i].getAttribute("becomes");
			if (tmp != null) {
				tiles[i].animNext = get(Integer.parseInt(tmp));
			}
			tmp = tiles[i].getAttribute("onpower");
			if (tmp != null) {
				tiles[i].onPower = get(Integer.parseInt(tmp));
			}
			tmp = tiles[i].getAttribute("onshutdown");
			if (tmp != null) {
				tiles[i].onShutdown = get(Integer.parseInt(tmp));
			}
			tmp = tiles[i].getAttribute("building-part");
			if (tmp != null) {
				handleBuildingPart(tiles[i], tmp);
			}
			TileSpec.BuildingInfo bi = tiles[i].getBuildingInfo();
			if (bi != null) {
				for (int j = 0; j < bi.members.length; j++) {
					int tid = bi.members[j];
					int offx = (bi.width >= 3 ? -1 : 0) + j % bi.width;
					int offy = (bi.height >= 3 ? -1 : 0) + j / bi.width;

					if (tiles[tid].owner == null &&
						(offx != 0 || offy != 0)
						)
					{
						tiles[tid].owner = tiles[i];
						tiles[tid].ownerOffsetX = offx;
						tiles[tid].ownerOffsetY = offy;
					}
				}
			}
		}
	}

	private static void handleBuildingPart(TileSpec partTile, String tmp)
	{
		String [] parts = tmp.split(",");
		if (parts.length != 3) {
			throw new Error("Invalid building-part specification");
		}

		partTile.owner = get(Integer.parseInt(parts[0]));
		partTile.ownerOffsetX = Integer.parseInt(parts[1]);
		partTile.ownerOffsetY = Integer.parseInt(parts[2]);

		assert partTile.ownerOffsetX != 0 || partTile.ownerOffsetY != 0;
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

	static void checkTiles()
	{
		for (int i = 0; i < tiles.length; i++) {
			// do something here
		}
	}
}
