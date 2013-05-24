// This file is part of MicropolisJ.
// Copyright (C) 2013 Jason Long
// Portions Copyright (C) 1989-2007 Electronic Arts Inc.
//
// MicropolisJ is free software; you can redistribute it and/or modify
// it under the terms of the GNU GPLv3, with additional terms.
// See the README file, included in this distribution, for details.

package micropolisj.engine;

public class GameLevel
{
	public static final int MIN_LEVEL = 0;
	public static final int MAX_LEVEL = 2;

	public static boolean isValid(int lev)
	{
		return lev >= MIN_LEVEL && lev <= MAX_LEVEL;
	}

	public static int getStartingFunds(int lev)
	{
		switch (lev) {
		case 0: return 20000;
		case 1: return 10000;
		case 2: return 5000;
		default:
			throw new Error("unexpected game level: "+lev);
		}
	}

	//prevent this class from being instantiated
	private GameLevel() {}
}
