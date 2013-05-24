package micropolisj.util;

import java.awt.*;
import javax.swing.*;
import javax.swing.table.*;

public class TranslatedStringsTable extends JTable
{
	public TranslatedStringsTable(TableModel tm)
	{
		super(tm);
		setDefaultEditor(String.class, new DefaultCellEditor(new JTextField()));
	}
}
