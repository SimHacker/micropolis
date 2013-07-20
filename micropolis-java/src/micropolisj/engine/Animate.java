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
 * Contains data used for tile animation.
 */
class Animate
{
	/**
	 * Contains data used for industrial zone animations.
	 */
	static class Smoke
	{
		// There are eight full Industry-zone images in the tiles bank.
		// This array indicates which of those eight zones have an animation.
		static boolean [] AniThis = {true, false, true, true, false, false, true, true };

		// Up to two tiles can be animated. Arrays DX1,DY1 indicate the relative
		// position of the first animated tile.
		static int [] DX1     = {   -1,    0,    1,    0,    0,    0,    0,    1 };
		static int [] DY1     = {   -1,    0,   -1,   -1,    0,    0,   -1,   -1 };

		// Arrays DX2,DY2 indicate the second animated tile.
		static int [] DX2     = {   -1,    0,    1,    1,    0,    0,    1,    1 };
		static int [] DY2     = {   -1,    0,    0,   -1,    0,    0,   -1,    0 };

		static int [] AniTabA = {    0,    0,   32,   40,    0,    0,   48,   56 };
		static int [] AniTabB = {    0,    0,   36,   44,    0,    0,   52,   60 };
		static int [] AniTabC = { IND1,    0, IND2, IND4,    0,    0, IND6, IND8 };
		static int [] AniTabD = { IND1,    0, IND3, IND5,    0,    0, IND7, IND9 };

		static final int ASCBIT = (CONDBIT);
		static final int REGBIT = (CONDBIT);
	}

}
