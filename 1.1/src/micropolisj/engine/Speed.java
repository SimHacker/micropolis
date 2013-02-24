// This file is part of MicropolisJ.
// Copyright (C) 2013 Jason Long
// Portions Copyright (C) 1989-2007 Electronic Arts Inc.
//
// MicropolisJ is free software; you can redistribute it and/or modify
// it under the terms of the GNU GPLv3, with additional terms.
// See the README file, included in this distribution, for details.

package micropolisj.engine;

/**
 * Lists the simulation speeds available.
 */
public enum Speed
{
	PAUSED    ( 999,999,  0),
	SLOW      ( 500,  5,  1),   //one step every 2500 ms
	NORMAL    ( 125,  2,  1),   //one step every 250 ms
	FAST      (  50,  1,  2),   //one step every 25 ms
	SUPER_FAST(  25,  1, 10);   //one step every 2.5 ms

	/** The animation speed, expressed as an interval in milliseconds. */
	public final int animationDelay;
	/** For slower speeds, how many animation occur for every simulation step.
	 * Faster speeds should set this to one. */
	public final int aniFramesPerStep;
	/** For faster speeds, how many simulation steps should occur for every
	 * update to the screen. */
	public final int simStepsPerUpdate;

	private Speed(int delay, int aniFrames, int simSteps)
	{
		this.animationDelay = delay;
		this.aniFramesPerStep = aniFrames;
		this.simStepsPerUpdate = simSteps;
	}
}
