package micropolisj.util;

import java.awt.*;
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
		stringsTable = new JTable(stringsModel);
		JScrollPane scrollPane = new JScrollPane(stringsTable);
		stringsTable.setFillsViewportHeight(true);
		getContentPane().add(scrollPane, BorderLayout.CENTER);

		JPanel buttonPane = new JPanel();
		getContentPane().add(buttonPane, BorderLayout.SOUTH);

		JButton btn;
		btn = new JButton("Add Locale");
		buttonPane.add(btn);

		pack();
		setDefaultCloseOperation(WindowConstants.DISPOSE_ON_CLOSE);
		setLocationRelativeTo(null);
	}

	public static void main(String [] args)
		throws Exception
	{
		new TranslationTool().setVisible(true);
	}

	class StringsModel extends AbstractTableModel
	{
		Properties p;
		String [] propNames;

		StringsModel() throws IOException
		{
			p = new Properties();
			p.load(new FileInputStream("strings/CityMessages.properties"));
			p.load(new FileInputStream("strings/CityStrings.properties"));
			p.load(new FileInputStream("strings/GuiStrings.properties"));
			p.load(new FileInputStream("strings/StatusMessages.properties"));

			propNames = p.keySet().toArray(new String[0]);
			Arrays.sort(propNames);
		}

		public Object getValueAt(int row, int col)
		{
			return propNames[row];
		}

		public int getRowCount()
		{
			return propNames.length;
		}

		public int getColumnCount()
		{
			return 1;
		}

		public String getColumnName(int col)
		{
			return "String Id";
		}
	}
}
