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

		stringsTable = new TranslatedStringsTable(stringsModel);
		JScrollPane scrollPane = new JScrollPane(stringsTable);
		stringsTable.setFillsViewportHeight(true);
		getContentPane().add(scrollPane, BorderLayout.CENTER);

		JPanel buttonPane = new JPanel();
		getContentPane().add(buttonPane, BorderLayout.SOUTH);

		JButton btn;
		btn = new JButton("Add Locale...");
		btn.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent evt) {
				onAddLocaleClicked();
			}});
		buttonPane.add(btn);

		btn = new JButton("Remove Locale");
		btn.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent evt) {
				onRemoveLocaleClicked();
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

		btn = new JButton("Submit");
		btn.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent evt) {
				onSubmitClicked();
			}});
		buttonPane.add(btn);

		pack();
		setDefaultCloseOperation(WindowConstants.DISPOSE_ON_CLOSE);
		setLocationRelativeTo(null);
	}

	private File getMicropolisJarFile()
	{
		try{

		Class mclass = micropolisj.engine.Micropolis.class;
		return new File(
			mclass.getProtectionDomain()
				.getCodeSource().getLocation().toURI().getPath()
			);

		} catch (java.net.URISyntaxException e) {
			throw new Error("unexpected: "+e, e);
		}
	}

	private void onTestClicked()
	{
		try
		{
			String javaPath = "java";
			String classPath = stringsModel.workingDirectory.toString()
				+ System.getProperty("path.separator")
				+ getMicropolisJarFile().toString();

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

	private void onRemoveLocaleClicked()
	{
		//TODO
	}

	private void onSubmitClicked()
	{
		//TODO
	}

	public static void main(String [] args)
		throws Exception
	{
		new TranslationTool().setVisible(true);
	}

}
