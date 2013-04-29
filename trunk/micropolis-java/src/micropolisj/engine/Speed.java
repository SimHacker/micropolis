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
 * Contains properties identifying how often the animation timer fires,
 * and how many animation steps are fired at each interval.
 * Note: for every 2 animation steps, one simulation step is triggered.
 */
public enum Speed
{
	PAUSED    ( 999,  0),
	SLOW      ( 625,  1),   //one sim step every 1250 ms
	NORMAL    ( 125,  1),   //one sim step every 250 ms
	FAST      (  25,  1),   //one sim step every 50 ms
	SUPER_FAST(  25,  5);   //one sim step every 10 ms

	/** The animation speed, expressed as an interval in milliseconds. */
	public final int animationDelay;

	/** For faster speeds, how many simulation steps should occur for every
	 * update to the screen. */
	public final int simStepsPerUpdate;

	private Speed(int delay, int simSteps)
	{
		this.animationDelay = delay;
		this.simStepsPerUpdate = simSteps;
	}
}
