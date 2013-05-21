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

	public TranslationTool()
		throws IOException
	{
		setTitle("MicropolisJ Translation Tool");

		stringsModel = new StringsModel();
		stringsModel.addLocale(null);

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

		pack();
		setDefaultCloseOperation(WindowConstants.DISPOSE_ON_CLOSE);
		setLocationRelativeTo(null);
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
			String language = langEntry.getText();
			String country = countryEntry.getText();
			String variant = variantEntry.getText();

			if (language.length() == 0) {
				throw new Exception("Language is required");
			}

			String code = language;
			if (country.length() != 0) {
				code += "_" + country;
				if (variant.length() != 0) {
					code += "_" + variant;
				}
			}
			else if (variant.length() != 0) {
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
		ArrayList<MyLocaleInfo> locales = new ArrayList<>();

		static class MyLocaleInfo
		{
			String code;
			HashMap<String,Properties> propsMap = new HashMap<>();
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
			ArrayList<StringInfo> ss = new ArrayList<>();
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
			return col != 0;
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
			return new File("strings/"
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
				File f = getPFile(file, localeCode);
				if (f.exists()) {
					p.load(new FileInputStream(f));
				}
				li.propsMap.put(file, p);
			}

			locales.add(li);
			fireTableStructureChanged();
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
					p.store(new FileOutputStream(f), l.code);
				}
			}
		}
	}
}
