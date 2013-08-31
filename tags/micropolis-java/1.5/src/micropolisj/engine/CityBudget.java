// This file is part of MicropolisJ.
// Copyright (C) 2013 Jason Long
// Portions Copyright (C) 1989-2007 Electronic Arts Inc.
//
// MicropolisJ is free software; you can redistribute it and/or modify
// it under the terms of the GNU GPLv3, with additional terms.
// See the README file, included in this distribution, for details.

package micropolisj.engine;

public class CityBudget
{
	private final Micropolis city;

	/**
	 * The amount of cash on hand.
	 */
	public int totalFunds;

	/**
	 * Amount of taxes collected so far in the current financial
	 * period (in 1/TAXFREQ's).
	 */
	int taxFund;

	/**
	 * Amount of prepaid road maintenance (in 1/TAXFREQ's).
	 */
	int roadFundEscrow;

	/**
	 * Amount of prepaid fire station maintenance (in 1/TAXFREQ's).
	 */
	int fireFundEscrow;

	/**
	 * Amount of prepaid police station maintenance (in 1/TAXFREQ's).
	 */
	int policeFundEscrow;

	CityBudget(Micropolis city)
	{
		this.city = city;
	}
}
