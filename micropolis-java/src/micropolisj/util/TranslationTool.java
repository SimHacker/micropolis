// This file is part of MicropolisJ.
// Copyright (C) 2013 Jason Long
// Portions Copyright (C) 1989-2007 Electronic Arts Inc.
//
// MicropolisJ is free software; you can redistribute it and/or modify
// it under the terms of the GNU GPLv3, with additional terms.
// See the README file, included in this distribution, for details.

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

	JButton removeBtn;
	JButton testBtn;
	JButton submitBtn;

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

		removeBtn = new JButton("Remove Locale");
		removeBtn.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent evt) {
				onRemoveLocaleClicked();
			}});
		buttonPane.add(removeBtn);

		testBtn = new JButton("Test");
		testBtn.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent evt) {
				onTestClicked();
			}});
		buttonPane.add(testBtn);

		submitBtn = new JButton("Submit");
		submitBtn.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent evt) {
				onSubmitClicked();
			}});
		buttonPane.add(submitBtn);

		updateButtonsEnabled();

		pack();
		setDefaultCloseOperation(WindowConstants.DO_NOTHING_ON_CLOSE);
		setLocationRelativeTo(null);

		addWindowListener(new WindowAdapter() {
			public void windowClosing(WindowEvent evt) {
				closeWindow();
			}});
	}

	private void closeWindow()
	{
		maybeSave();
		dispose();
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
		maybeSave();

		String code = pickLocale(
				"Which locale do you want to test?",
				"Test Locale"
				);
		if (code == null) {
			return;
		}

		String [] localeParts = code.split("_");
		String selLanguage = localeParts.length >= 1 ? localeParts[0] : "";
		String selCountry = localeParts.length >= 2 ? localeParts[1] : "";
		String selVariant = localeParts.length >= 3 ? localeParts[2] : "";

		File javaExe = new File(System.getProperty("java.home"));
		javaExe = new File(javaExe, "bin");
		javaExe = new File(javaExe, "java");

		try
		{
			String javaPath = javaExe.toString();
			String classPath = stringsModel.workingDirectory.toString()
				+ System.getProperty("path.separator")
				+ getMicropolisJarFile().toString();

			ProcessBuilder processBuilder =
			new ProcessBuilder(javaPath,
				"-Duser.language="+selLanguage,
				"-Duser.country="+selCountry,
				"-Duser.variant="+selVariant,
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

	private void maybeSave()
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
		maybeSave();

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
			String lastLanguage = langEntry.getText();
			String lastCountry = countryEntry.getText();
			String lastVariant = variantEntry.getText();

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
			updateButtonsEnabled();
		}
		catch (Exception e)
		{
			JOptionPane.showMessageDialog(this,
				e.getMessage(),
				"Error",
				JOptionPane.ERROR_MESSAGE);
		}
	}

	private void updateButtonsEnabled()
	{
		int count = stringsModel.getAllLocaleCodes().length;
		removeBtn.setEnabled(count > 1);
		testBtn.setEnabled(count > 1);
		submitBtn.setEnabled(count > 1);
	}

	String pickLocale(String message, String dlgTitle)
	{
		String[] locales = stringsModel.getAllLocaleCodes();
		JComboBox<String> localeCb = new JComboBox<String>();
		for (int i = 0; i < locales.length; i++) {
			if (locales[i] != null) {
				localeCb.addItem(locales[i]);
			}
		}

		if (localeCb.getItemCount() == 1) {
			return (String) localeCb.getItemAt(0);
		}
		else if (localeCb.getItemCount() == 0) {
			return null;
		}

		localeCb.setSelectedIndex(localeCb.getItemCount()-1);

		JComponent [] inputs = new JComponent[] {
			new JLabel(message),
			localeCb
			};
		int rv = JOptionPane.showOptionDialog(this,
			inputs,
			dlgTitle,
			JOptionPane.OK_CANCEL_OPTION,
			JOptionPane.PLAIN_MESSAGE,
			null, null, null);
		if (rv != JOptionPane.OK_OPTION)
			return null;

		return (String) localeCb.getSelectedItem();
	}

	private void onRemoveLocaleClicked()
	{
		maybeSave();
		String code = pickLocale(
				"Which locale do you want to remove?",
				"Remove Locale"
				);
		if (code != null) {
			stringsModel.removeLocale(code);
			updateButtonsEnabled();
		}
	}

	private void onSubmitClicked()
	{
		maybeSave();
		String code = pickLocale(
				"Which locale do you want to submit?",
				"Submit Locale"
				);
		if (code == null) return;

		String msg = "";
		msg = msg + "Your translated strings have been saved to\n";
		msg = msg + new File(stringsModel.workingDirectory, "micropolisj").toString() + "\n";
		msg = msg + "as:\n";
		for (int i = 0; i < stringsModel.FILES.length; i++) {
			msg = msg + " * "
				+ stringsModel.FILES[i]+"_"+code+".properties"
				+ "\n";
		}
		msg = msg + "\n";
		msg = msg + "Submit these files to the Micropolis website\n";
		msg = msg + "https://code.google.com/p/micropolis\n";
		msg = msg + "(Open a new \"Issue\" and attach the files to the issue.)";

		JOptionPane.showMessageDialog(this,
			msg, "Submit Locale",
			JOptionPane.INFORMATION_MESSAGE);
	}

	public static void main(String [] args)
		throws Exception
	{
		new TranslationTool().setVisible(true);
	}

}
