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
			if (localeCode == null) {
				p.load(getClass().getResourceAsStream("/micropolisj/"+file+".properties"));
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
