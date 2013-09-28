// This file is part of MicropolisJ.
// Copyright (C) 2013 Jason Long
// Portions Copyright (C) 1989-2007 Electronic Arts Inc.
//
// MicropolisJ is free software; you can redistribute it and/or modify
// it under the terms of the GNU GPLv3, with additional terms.
// See the README file, included in this distribution, for details.

package micropolisj.util;

import java.io.*;
import java.util.*;
import javax.swing.table.*;

class StringsModel extends AbstractTableModel
{
	StringInfo [] strings;
	ArrayList<MyLocaleInfo> locales = new ArrayList<MyLocaleInfo>();

	static class MyLocaleInfo
	{
		String code;
		HashMap<String,Properties> propsMap = new HashMap<String,Properties>();
		boolean dirty;

		MyLocaleInfo(String code) {
			this.code = code;
		}
	}

	static class StringInfo
	{
		String file;
		String id;

		StringInfo(String file, String id)
		{
			this.file = file;
			this.id = id;
		}
	}

	static final String [] FILES = {
		"CityMessages",
		"CityStrings",
		"GuiStrings",
		"StatusMessages"
		};

	File workingDirectory;

	StringsModel() throws IOException
	{
		workingDirectory = new File(
			new File(System.getProperty("user.home")),
			"micropolis-translations"
			);

		ArrayList<StringInfo> ss = new ArrayList<StringInfo>();
		for (String f : FILES) {
			loadStrings(f, ss);
		}
		strings = ss.toArray(new StringInfo[0]);
	}

	static void loadStrings(String file, ArrayList<StringInfo> ss)
		throws IOException
	{
		Properties p = new Properties();
		p.load(StringsModel.class.getResourceAsStream("/micropolisj/"+file+".properties"));
		String [] propNames = p.keySet().toArray(new String[0]);
		Arrays.sort(propNames);

		for (String propName : propNames)
		{
			StringInfo si = new StringInfo(file, propName);
			ss.add(si);
		}
	}

	public Object getValueAt(int row, int col)
	{
		StringInfo si = strings[row];
		if (col == 0) {
			return si.id;
		}

		MyLocaleInfo l = locales.get(col-1);
		Properties p = l.propsMap.get(si.file);
		return p.getProperty(si.id);
	}

	@Override
	public int getRowCount()
	{
		return strings.length;
	}

	@Override
	public int getColumnCount()
	{
		return 1 + locales.size();
	}

	@Override
	public Class getColumnClass(int col)
	{
		return String.class;
	}

	@Override
	public String getColumnName(int col)
	{
		if (col == 0) {
			return "String";
		}
		else {
			MyLocaleInfo l = locales.get(col-1);
			return l.code != null ? l.code : "C";
		}
	}

	@Override
	public boolean isCellEditable(int row, int col)
	{
		if (col == 0) {
			return false;
		}
		else {
			MyLocaleInfo l = locales.get(col-1);
			return l.code != null;
		}
	}

	@Override
	public void setValueAt(Object aValue, int row, int col)
	{
		StringInfo si = strings[row];
		if (col == 0) {
			return;
		}

		MyLocaleInfo l = locales.get(col-1);
		Properties p = l.propsMap.get(si.file);
		p.setProperty(si.id, (String)aValue);
		l.dirty = true;
	}

	/**
	 * Gets the file in the user's working directory.
	 */
	File getPFile(String file, String localeCode)
	{
		File d = new File(workingDirectory, "micropolisj");
		return new File(d,
			file
			+(localeCode != null ? "_"+localeCode : "")
			+".properties");
	}

	void addLocale(String localeCode)
		throws IOException
	{
		MyLocaleInfo li = new MyLocaleInfo(localeCode);
		for (String file : FILES)
		{
			Properties p = new Properties();
			{
				// load strings from our jar file
				String s = "/micropolisj/"+file+(localeCode != null ? "_"+localeCode : "") + ".properties";
				InputStream in = getClass().getResourceAsStream(s);
				if (in != null) {
					p.load(in);
				}
			}
			File f = getPFile(file, localeCode);
			if (f.exists()) {
				p.load(new FileInputStream(f));
			}
			li.propsMap.put(file, p);
		}

		locales.add(li);
		fireTableStructureChanged();
	}

	String [] getAllLocaleCodes()
	{
		String [] rv = new String[locales.size()];
		for (int i = 0; i < rv.length; i++) {
			rv[i] = locales.get(i).code;
		}
		return rv;
	}

	void removeLocale(String localeCode)
	{
		assert localeCode != null;

		boolean found = false;
		for (int i = locales.size()-1; i >= 0; i--) {
			String loc = locales.get(i).code;
			if (localeCode.equals(loc)) {
				locales.remove(i);
				found = true;
			}
		}
		if (found) {
			fireTableStructureChanged();
		}
	}

	void makeDirectories(File f)
		throws IOException
	{
		File d = f.getParentFile();
		if (d != null) {
			d.mkdirs();
		}
	}

	void save()
		throws IOException
	{
		for (MyLocaleInfo l : locales)
		{
			if (!l.dirty) continue;

			for (String file : FILES)
			{
				Properties p = l.propsMap.get(file);
				File f = getPFile(file, l.code);
				makeDirectories(f);
				p.store(new FileOutputStream(f), l.code);
			}
			l.dirty = false;
		}
	}
}
