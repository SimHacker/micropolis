// This file is part of MicropolisJ.
// Copyright (C) 2013 Jason Long
// Portions Copyright (C) 1989-2007 Electronic Arts Inc.
//
// MicropolisJ is free software; you can redistribute it and/or modify
// it under the terms of the GNU GPLv3, with additional terms.
// See the README file, included in this distribution, for details.

package micropolisj.gui;

import java.awt.*;
import java.awt.event.*;
import java.awt.geom.Path2D;
import java.text.*;
import java.util.*;
import javax.swing.*;

import micropolisj.engine.*;
import static micropolisj.gui.ColorParser.parseColor;

public class GraphsPane extends JPanel
	implements Micropolis.Listener
{
	Micropolis engine;

	JToggleButton tenYearsBtn;
	JToggleButton onetwentyYearsBtn;
	GraphArea graphArea;

	static enum TimePeriod
	{
		TEN_YEARS,
		ONETWENTY_YEARS;
	}

	static enum GraphData
	{
		RESPOP,
		COMPOP,
		INDPOP,
		MONEY,
		CRIME,
		POLLUTION;
	}
	EnumMap<GraphData,JToggleButton> dataBtns = new EnumMap<GraphData,JToggleButton>(GraphData.class);

	static ResourceBundle strings = MainWindow.strings;
	static final int LEFT_MARGIN = 4;
	static final int RIGHT_MARGIN = 4;
	static final int TOP_MARGIN = 2;
	static final int BOTTOM_MARGIN = 2;
	static final int LEGEND_PADDING = 6;

	public GraphsPane(Micropolis engine)
	{
		super(new BorderLayout());

		assert engine != null;
		this.engine = engine;
		engine.addListener(this);

		JButton dismissBtn = new JButton(strings.getString("dismiss_graph"));
		dismissBtn.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent evt) {
				onDismissClicked();
			}});
		add(dismissBtn, BorderLayout.SOUTH);

		JPanel b1 = new JPanel(new BorderLayout());
		add(b1, BorderLayout.CENTER);

		JPanel toolsPane = new JPanel(new GridBagLayout());
		b1.add(toolsPane, BorderLayout.WEST);

		GridBagConstraints c = new GridBagConstraints();
		c.gridx = c.gridy = 0;
		c.gridwidth = 2;
		c.fill = GridBagConstraints.BOTH;
		c.insets = new Insets(1,1,1,1);
		tenYearsBtn = new JToggleButton(strings.getString("ten_years"));
		tenYearsBtn.setMargin(new Insets(0,0,0,0));
		tenYearsBtn.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent evt) {
				setTimePeriod(TimePeriod.TEN_YEARS);
			}});
		toolsPane.add(tenYearsBtn, c);

		c.gridy++;
		onetwentyYearsBtn = new JToggleButton(strings.getString("onetwenty_years"));
		onetwentyYearsBtn.setMargin(new Insets(0,0,0,0));
		onetwentyYearsBtn.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent evt) {
				setTimePeriod(TimePeriod.ONETWENTY_YEARS);
			}});
		toolsPane.add(onetwentyYearsBtn, c);

		c.gridx = 0;
		c.gridy = 2;
		c.gridwidth = 1;
		c.anchor = GridBagConstraints.NORTH;
		c.weightx = 0.5;
		toolsPane.add(makeDataBtn(GraphData.RESPOP), c);

		c.gridy = 3;
		toolsPane.add(makeDataBtn(GraphData.COMPOP), c);

		c.gridy = 4;
		toolsPane.add(makeDataBtn(GraphData.INDPOP), c);

		c.gridx = 1;
		c.gridy = 2;
		toolsPane.add(makeDataBtn(GraphData.MONEY), c);

		c.gridy = 3;
		toolsPane.add(makeDataBtn(GraphData.CRIME), c);

		c.gridy = 4;
		toolsPane.add(makeDataBtn(GraphData.POLLUTION), c);

		graphArea = new GraphArea();
		b1.add(graphArea, BorderLayout.CENTER);

		setTimePeriod(TimePeriod.TEN_YEARS);
		dataBtns.get(GraphData.MONEY).setSelected(true);
		dataBtns.get(GraphData.POLLUTION).setSelected(true);
	}

	public void setEngine(Micropolis newEngine)
	{
		if (engine != null) {  //old engine
			engine.removeListener(this);
		}
		engine = newEngine;
		if (engine != null) {  //new engine
			engine.addListener(this);
			graphArea.repaint();
		}
	}

	private void onDismissClicked()
	{
		setVisible(false);
	}

	//implements Micropolis.Listener
	public void cityMessage(MicropolisMessage message, CityLocation loc) {}
	public void citySound(Sound sound, CityLocation loc) {}
	public void demandChanged() {}
	public void evaluationChanged() {}
	public void fundsChanged() {}
	public void optionsChanged() {}

	//implements Micropolis.Listener
	public void censusChanged()
	{
		graphArea.repaint();
	}

	private JToggleButton makeDataBtn(GraphData graph)
	{
		String icon1name = strings.getString("graph_button."+graph.name());
		String icon2name = strings.getString("graph_button."+graph.name()+".selected");

		ImageIcon icon1 = new ImageIcon(getClass().getResource("/"+icon1name));
		ImageIcon icon2 = new ImageIcon(getClass().getResource("/"+icon2name));

		JToggleButton btn = new JToggleButton();
		btn.setIcon(icon1);
		btn.setSelectedIcon(icon2);
		btn.setBorder(null);
		btn.setBorderPainted(false);
		btn.setFocusPainted(false);
		btn.setContentAreaFilled(false);
		btn.setMargin(new Insets(0,0,0,0));

		btn.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent evt) {
				graphArea.repaint();
			}});

		dataBtns.put(graph, btn);
		return btn;
	}

	int getHistoryMax()
	{
		int max = 0;
		for (GraphData g : GraphData.values()) {
			for (int pos = 0; pos < 240; pos++) {
				max = Math.max(max, getHistoryValue(g, pos));
			}
		}
		return max;
	}

	int getHistoryValue(GraphData graph, int pos)
	{
		assert pos >= 0 && pos < 240;
		switch(graph) {
		case RESPOP: return engine.history.res[pos];
		case COMPOP: return engine.history.com[pos];
		case INDPOP: return engine.history.ind[pos];
		case MONEY: return engine.history.money[pos];
		case CRIME: return engine.history.crime[pos];
		case POLLUTION: return engine.history.pollution[pos];
		default: throw new Error("unexpected");
		}
	}

	void setTimePeriod(TimePeriod period)
	{
		tenYearsBtn.setSelected(period == TimePeriod.TEN_YEARS);
		onetwentyYearsBtn.setSelected(period == TimePeriod.ONETWENTY_YEARS);
		graphArea.repaint();
	}

	class GraphArea extends JComponent
	{
		GraphArea()
		{
			setBorder(BorderFactory.createLoweredBevelBorder());
		}

		@Override
		public void paintComponent(Graphics gr1)
		{
			Graphics2D gr = (Graphics2D)gr1;
			FontMetrics fm = gr.getFontMetrics();

			gr.setRenderingHint(RenderingHints.KEY_ANTIALIASING, RenderingHints.VALUE_ANTIALIAS_ON);
			gr.setColor(Color.WHITE);
			gr.fill(gr.getClipBounds());

			// determine length of longest label
			int maxLabelWidth = 0;
			for (GraphData gd : GraphData.values())
			{
				String labelStr = strings.getString("graph_label."+gd.name());
				int adv = fm.stringWidth(labelStr);
				if (adv > maxLabelWidth) {
					maxLabelWidth = adv;
				}
			}

		int leftEdge = getInsets().left + LEFT_MARGIN;
		int topEdge = getInsets().top + TOP_MARGIN + fm.getHeight()*2;
		int bottomEdge = getHeight() - getInsets().bottom - getInsets().top - BOTTOM_MARGIN;
		int rightEdge = getWidth() - getInsets().right - getInsets().left - RIGHT_MARGIN - maxLabelWidth - LEGEND_PADDING;

			// draw graph lower, upper borders
			gr.setColor(Color.BLACK);
			gr.drawLine(leftEdge,topEdge,rightEdge,topEdge);
			gr.drawLine(leftEdge,bottomEdge,rightEdge,bottomEdge);

			// draw vertical bars and label the dates
			boolean isOneTwenty = onetwentyYearsBtn.isSelected();
			int unitPeriod = isOneTwenty ? 12*Micropolis.CENSUSRATE : Micropolis.CENSUSRATE;
			int hashPeriod = isOneTwenty ? 10*unitPeriod : 12*unitPeriod;
			int startTime = ((engine.history.cityTime / unitPeriod) - 119) * unitPeriod;

			double x_interval = (rightEdge - leftEdge) / 120.0;
			for (int i = 0; i < 120; i++) {
				int t = startTime + i * unitPeriod;  // t might be negative
				if (t % hashPeriod == 0) {
					// year
					int year = 1900+(t/(12*Micropolis.CENSUSRATE));
					int numHashes = t/hashPeriod;
					int x = (int)Math.round(leftEdge+i*x_interval);
					int y = getInsets().top + TOP_MARGIN +
						(numHashes % 2 == 0 ? fm.getHeight() : 0) +
						fm.getAscent();
					gr.drawString(Integer.toString(year), x, y);
					gr.drawLine(x,topEdge,x,bottomEdge);
				}
			}

			int H = isOneTwenty ? 239 : 119;
			final HashMap<GraphData, Path2D.Double> paths = new HashMap<GraphData,Path2D.Double>();
			double scale = Math.max(256.0, getHistoryMax());
			for (GraphData gd : GraphData.values())
			{
				if (dataBtns.get(gd).isSelected()) {

					Path2D.Double path = new Path2D.Double();
					for (int i = 0; i < 120; i++) {
						double xp = leftEdge + i * x_interval;
						double yp = bottomEdge - getHistoryValue(gd,H-i) * (bottomEdge-topEdge) / scale;
						if (i == 0) {
							path.moveTo(xp, yp);
						} else {
							path.lineTo(xp, yp);
						}
					}
					paths.put(gd, path);
				}
			}

			GraphData [] myGraphs = paths.keySet().toArray(new GraphData[0]);
			Arrays.sort(myGraphs, new Comparator<GraphData>() {
				public int compare(GraphData a, GraphData b) {
					double y0 = paths.get(a).getCurrentPoint().getY();
					double y1 = paths.get(b).getCurrentPoint().getY();
					return -Double.compare(y0,y1);
				}});

			int lbottom = bottomEdge;
			for (GraphData gd : myGraphs)
			{
				String labelStr = strings.getString("graph_label."+gd.name());
				String colStr = strings.getString("graph_color."+gd.name());
				Color col = parseColor(colStr);
				Path2D.Double path = paths.get(gd);

				gr.setColor(col);
				gr.setStroke(new BasicStroke(2));
				gr.draw(path);

				int x = rightEdge + LEGEND_PADDING;
				int y = (int)Math.round(path.getCurrentPoint().getY()+fm.getAscent()/2);
				y = Math.min(lbottom, y);
				lbottom = y - fm.getAscent();

				gr.setColor(col);
				gr.drawString(labelStr, x-1, y);
				gr.drawString(labelStr, x, y-1);

				gr.setColor(Color.BLACK);
				gr.drawString(labelStr, x, y);
			}
		}
	}
}
