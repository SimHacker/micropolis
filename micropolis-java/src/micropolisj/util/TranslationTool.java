package micropolisj.util;

import java.awt.*;
import java.awt.event.*;
import java.io.*;
import java.util.*;
import javax.swing.*;
import javax.swing.table.*;

public class TranslationTool extends JFrame
{
	JTable stringsTable;
	StringsModel stringsModel;
	String lastLanguage;
	String lastCountry;
	String lastVariant;

	public TranslationTool()
	{
		setTitle("MicropolisJ Translation Tool");

		try {
		stringsModel = new StringsModel();
		stringsModel.addLocale(null);

		}
		catch (IOException e) {
			JOptionPane.showMessageDialog(this,
				e, "Error", JOptionPane.ERROR_MESSAGE);
			System.exit(1);
		}

		stringsTable = new JTable(stringsModel);
		stringsTable.setDefaultEditor(String.class, new DefaultCellEditor(new JTextField()));
		JScrollPane scrollPane = new JScrollPane(stringsTable);
		stringsTable.setFillsViewportHeight(true);
		getContentPane().add(scrollPane, BorderLayout.CENTER);

		JPanel buttonPane = new JPanel();
		getContentPane().add(buttonPane, BorderLayout.SOUTH);

		JButton btn;
		btn = new JButton("Add Locale");
		btn.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent evt) {
				onAddLocaleClicked();
			}});
		buttonPane.add(btn);

		btn = new JButton("Save");
		btn.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent evt) {
				onSaveClicked();
			}});
		buttonPane.add(btn);

		btn = new JButton("Test");
		btn.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent evt) {
				onTestClicked();
			}});
		buttonPane.add(btn);

		pack();
		setDefaultCloseOperation(WindowConstants.DISPOSE_ON_CLOSE);
		setLocationRelativeTo(null);
	}

	private void onTestClicked()
	{
		try
		{
			String javaPath = "java";
			String classPath = "." + System.getProperty("path.separator") + "micropolisj.jar";

			ProcessBuilder processBuilder =
			new ProcessBuilder(javaPath,
				"-Duser.language="+lastLanguage,
				"-Duser.country="+lastCountry,
				"-Duser.variant="+lastVariant,
				"-cp",
				classPath,
				"micropolisj.Main"
				);
			processBuilder.start();
		}
		catch (Exception e)
		{
			JOptionPane.showMessageDialog(this,
				e.getMessage(),
				"Error",
				JOptionPane.ERROR_MESSAGE);
		}
	}

	private void onSaveClicked()
	{
		try
		{
			stringsModel.save();
		}
		catch (Exception e)
		{
			JOptionPane.showMessageDialog(this,
				e.getMessage(),
				"Error",
				JOptionPane.ERROR_MESSAGE);
		}
	}

	private void onAddLocaleClicked()
	{
		Locale L = Locale.getDefault();

		JTextField langEntry = new JTextField(L.getLanguage());
		JTextField countryEntry = new JTextField(L.getCountry());
		JTextField variantEntry = new JTextField(L.getVariant());

		JComponent [] inputs = new JComponent[] {
			new JLabel("Language"),
			langEntry,
			new JLabel("Country"),
			countryEntry,
			new JLabel("Variant (optional)"),
			variantEntry
			};
		int rv = JOptionPane.showOptionDialog(this,
			inputs,
			"Add Locale",
			JOptionPane.OK_CANCEL_OPTION,
			JOptionPane.PLAIN_MESSAGE,
			null, null, null);
		if (rv != JOptionPane.OK_OPTION)
			return;

		try
		{
			lastLanguage = langEntry.getText();
			lastCountry = countryEntry.getText();
			lastVariant = variantEntry.getText();

			if (lastLanguage.length() == 0) {
				throw new Exception("Language is required");
			}

			String code = lastLanguage;
			if (lastCountry.length() != 0) {
				code += "_" + lastCountry;
				if (lastVariant.length() != 0) {
					code += "_" + lastVariant;
				}
			}
			else if (lastVariant.length() != 0) {
				throw new Exception("Cannot specify variant without a country code.");
			}

			stringsModel.addLocale(code);
		}
		catch (Exception e)
		{
			JOptionPane.showMessageDialog(this,
				e.getMessage(),
				"Error",
				JOptionPane.ERROR_MESSAGE);
		}
	}

	public static void main(String [] args)
		throws Exception
	{
		new TranslationTool().setVisible(true);
	}

	static class StringsModel extends AbstractTableModel
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

		StringsModel() throws IOException
		{
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
			p.load(new FileInputStream("strings/"+file+".properties"));
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

		File getPFile(String file, String localeCode)
		{
			return new File("micropolisj/"
				+file
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
}
