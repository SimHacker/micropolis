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
import java.awt.image.*;
import java.net.URL;
import java.util.*;
import javax.swing.*;
import javax.swing.event.*;

import micropolisj.engine.*;
import static micropolisj.engine.TileConstants.*;

public class OverlayMapView extends JComponent
	implements Scrollable, MapListener
{
	Micropolis engine;
	ArrayList<ConnectedView> views = new ArrayList<ConnectedView>();
	MapState mapState = MapState.ALL;

	public OverlayMapView(Micropolis _engine)
	{
		assert _engine != null;

		MouseAdapter mouse = new MouseAdapter() {
			@Override
			public void mousePressed(MouseEvent ev)
			{
				onMousePressed(ev);
			}
			@Override
			public void mouseDragged(MouseEvent ev)
			{
				onMouseDragged(ev);
			}
		};
		addMouseListener(mouse);
		addMouseMotionListener(mouse);

		setEngine(_engine);
	}

	public Micropolis getEngine()
	{
		return engine;
	}

	public void setEngine(Micropolis newEngine)
	{
		assert newEngine != null;

		if (engine != null) { //old engine
			engine.removeMapListener(this);
		}
		engine = newEngine;
		if (engine != null) { //new engine
			engine.addMapListener(this);
		}

		invalidate();  //map size may have changed
		repaint();
		engine.calculateCenterMass();
		dragViewToCityCenter();
	}

	public MapState getMapState()
	{
		return mapState;
	}

	@Override
	public Dimension getPreferredSize()
	{
		return new Dimension(
			getInsets().left + getInsets().right + TILE_WIDTH*engine.getWidth(),
			getInsets().top + getInsets().bottom + TILE_HEIGHT*engine.getHeight()
			);
	}

	public void setMapState(MapState newState)
	{
		if (mapState == newState)
			return;

		mapState = newState;
		repaint();
	}

	static BufferedImage tileArrayImage = loadImage("/tilessm.png");
	static final int TILE_WIDTH = 3;
	static final int TILE_HEIGHT = 3;
	static final int TILE_OFFSET_Y = 3;

	static BufferedImage loadImage(String resourceName)
	{
		URL iconUrl = MicropolisDrawingArea.class.getResource(resourceName);
		Image refImage = new ImageIcon(iconUrl).getImage();

		BufferedImage bi = new BufferedImage(refImage.getWidth(null), refImage.getHeight(null),
					BufferedImage.TYPE_INT_RGB);
		Graphics2D gr = bi.createGraphics();
		gr.drawImage(refImage, 0, 0, null);

		return bi;
	}

	static final Color VAL_LOW       = new Color(0xbfbfbf);
	static final Color VAL_MEDIUM    = new Color(0xffff00);
	static final Color VAL_HIGH      = new Color(0xff7f00);
	static final Color VAL_VERYHIGH  = new Color(0xff0000);
	static final Color VAL_PLUS      = new Color(0x007f00);
	static final Color VAL_VERYPLUS  = new Color(0x00e600);
	static final Color VAL_MINUS     = new Color(0xff7f00);
	static final Color VAL_VERYMINUS = new Color(0xffff00);

	private Color getCI(int x)
	{
		if (x < 50)
			return null;
		else if (x < 100)
			return VAL_LOW;
		else if (x < 150)
			return VAL_MEDIUM;
		else if (x < 200)
			return VAL_HIGH;
		else
			return VAL_VERYHIGH;
	}

	private Color getCI_rog(int x)
	{
		if (x > 100)
			return VAL_VERYPLUS;
		else if (x > 20)
			return VAL_PLUS;
		else if (x < -100)
			return VAL_VERYMINUS;
		else if (x < -20)
			return VAL_MINUS;
		else
			return null;
	}

	private void drawPollutionMap(Graphics gr)
	{
		int [][] A = engine.pollutionMem;

		for (int y = 0; y < A.length; y++) {
			for (int x = 0; x < A[y].length; x++) {
				maybeDrawRect(gr, getCI(10 + A[y][x]),x*6,y*6,6,6);
			}
		}
	}

	private void drawCrimeMap(Graphics gr)
	{
		int [][] A = engine.crimeMem;

		for (int y = 0; y < A.length; y++) {
			for (int x = 0; x < A[y].length; x++) {
				maybeDrawRect(gr, getCI(A[y][x]),x*6,y*6,6,6);
			}
		}
	}

	private void drawPopDensity(Graphics gr)
	{
		int [][] A = engine.popDensity;

		for (int y = 0; y < A.length; y++) {
			for (int x = 0; x < A[y].length; x++) {
				maybeDrawRect(gr, getCI(A[y][x]),x*6,y*6,6,6);
			}
		}
	}

	private void drawRateOfGrowth(Graphics gr)
	{
		int [][] A = engine.rateOGMem;

		for (int y = 0; y < A.length; y++) {
			for (int x = 0; x < A[y].length; x++) {
				maybeDrawRect(gr, getCI_rog(A[y][x]),x*24,y*24,24,24);
			}
		}
	}

	private void drawFireRadius(Graphics gr)
	{
		int [][] A = engine.fireRate;

		for (int y = 0; y < A.length; y++) {
			for (int x = 0; x < A[y].length; x++) {
				maybeDrawRect(gr, getCI(A[y][x]),x*24,y*24,24,24);
			}
		}
	}

	private void drawPoliceRadius(Graphics gr)
	{
		int [][] A = engine.policeMapEffect;

		for (int y = 0; y < A.length; y++) {
			for (int x = 0; x < A[y].length; x++) {
				maybeDrawRect(gr, getCI(A[y][x]),x*24,y*24,24,24);
			}
		}
	}

	private void maybeDrawRect(Graphics gr, Color col, int x, int y, int width, int height)
	{
		if (col != null) {
			gr.setColor(col);
			gr.fillRect(x,y,width,height);
		}
	}

	static final int UNPOWERED  = 0x6666e6;   //lightblue
	static final int POWERED    = 0xff0000;   //red
	static final int CONDUCTIVE = 0xbfbfbf;   //lightgray

	private int checkPower(BufferedImage img, int x, int y, int rawTile)
	{
		int pix;

		if ((rawTile & LOMASK) <= 63) {
			return rawTile & LOMASK;
		}
		else if (isZoneCenter(rawTile)) {
			// zone
			pix = ((rawTile & PWRBIT) != 0) ? POWERED : UNPOWERED;
		}
		else if (isConductive(rawTile)) {
			pix = CONDUCTIVE;
		}
		else {
			return DIRT;
		}

		for (int yy = 0; yy < TILE_HEIGHT; yy++)
		{
			for (int xx = 0; xx < TILE_WIDTH; xx++)
			{
				img.setRGB(x*TILE_WIDTH+xx,y*TILE_HEIGHT+yy, pix);
			}
		}
		return -1; //this special value tells caller to skip the tile bitblt,
		           //since it was performed here
	}

	private int checkLandValueOverlay(BufferedImage img, int xpos, int ypos, int tile)
	{
		int v = engine.getLandValue(xpos, ypos);
		Color c = getCI(v);
		if (c == null) {
			return tile;
		}

		int pix = c.getRGB();
		for (int yy = 0; yy < TILE_HEIGHT; yy++) {
			for (int xx = 0; xx < TILE_WIDTH; xx++) {
				img.setRGB(
					xpos*TILE_WIDTH+xx,
					ypos*TILE_HEIGHT+yy,
					pix);
			}
		}
		return CLEAR;
	}

	private int checkTrafficOverlay(BufferedImage img, int xpos, int ypos, int tile)
	{
		int d = engine.getTrafficDensity(xpos, ypos);
		Color c = getCI(d);
		if (c == null) {
			return tile;
		}

		int pix = c.getRGB();
		for (int yy = 0; yy < TILE_HEIGHT; yy++) {
			for (int xx = 0; xx < TILE_WIDTH; xx++) {
				img.setRGB(
					xpos*TILE_WIDTH+xx,
					ypos*TILE_HEIGHT+yy,
					pix);
			}
		}
		return CLEAR;
	}

	@Override
	public void paintComponent(Graphics gr)
	{
		final int width = engine.getWidth();
		final int height = engine.getHeight();

		BufferedImage img = new BufferedImage(width*TILE_WIDTH, height*TILE_HEIGHT,
				BufferedImage.TYPE_INT_RGB);

		final Insets INSETS = getInsets();
		Rectangle clipRect = gr.getClipBounds();
		int minX = Math.max(0, (clipRect.x - INSETS.left) / TILE_WIDTH);
		int minY = Math.max(0, (clipRect.y - INSETS.top) / TILE_HEIGHT);
		int maxX = Math.min(width, 1 + (clipRect.x - INSETS.left + clipRect.width-1) / TILE_WIDTH);
		int maxY = Math.min(height, 1 + (clipRect.y - INSETS.top + clipRect.height-1) / TILE_HEIGHT);

		for (int y = minY; y < maxY; y++)
		{
			for (int x = minX; x < maxX; x++)
			{
				int tile = engine.getTile(x,y);
				switch (mapState) {
				case RESIDENTIAL:
					if (isZoneAny(tile) &&
						!isResidentialZoneAny(tile))
					{
						tile = DIRT;
					}
					break;
				case COMMERCIAL:
					if (isZoneAny(tile) &&
						!isCommercialZone(tile))
					{
						tile = DIRT;
					}
					break;
				case INDUSTRIAL:
					if (isZoneAny(tile) &&
						!isIndustrialZone(tile))
					{
						tile = DIRT;
					}
					break;
				case POWER_OVERLAY:
					tile = checkPower(img, x, y, engine.getTile(x,y));
					break;
				case TRANSPORT:
				case TRAFFIC_OVERLAY:
					if (isConstructed(tile)
						&& !isRoadAny(tile)
						&& !isRailAny(tile))
					{
						tile = DIRT;
					}
					if (mapState == MapState.TRAFFIC_OVERLAY)
					{
						tile = checkTrafficOverlay(img, x, y, tile);
					}
					break;

				case LANDVALUE_OVERLAY:
					tile = checkLandValueOverlay(img, x, y, tile);
					break;

				default:
				}

				// tile == -1 means it's already been drawn
				// in the checkPower function

				if (tile != -1) {
					paintTile(img, x, y, tile);
				}
			}
		}

		gr.drawImage(img, INSETS.left, INSETS.top, null);

		gr = gr.create();
		gr.translate(INSETS.left, INSETS.top);

		switch (mapState) {
		case POLICE_OVERLAY:
			drawPoliceRadius(gr); break;
		case FIRE_OVERLAY:
			drawFireRadius(gr); break;
		case CRIME_OVERLAY:
			drawCrimeMap(gr); break;
		case POLLUTE_OVERLAY:
			drawPollutionMap(gr); break;
		case GROWTHRATE_OVERLAY:
			drawRateOfGrowth(gr); break;
		case POPDEN_OVERLAY:
			drawPopDensity(gr); break;
		default:
		}

		for (ConnectedView cv : views)
		{
			Rectangle rect = getViewRect(cv);
			gr.setColor(Color.WHITE);
			gr.drawRect(rect.x-2,rect.y-2,rect.width+2,rect.height+2);

			gr.setColor(Color.BLACK);
			gr.drawRect(rect.x-0,rect.y-0,rect.width+2,rect.height+2);

			gr.setColor(Color.YELLOW);
			gr.drawRect(rect.x-1,rect.y-1,rect.width+2,rect.height+2);
		}
	}

	void paintTile(BufferedImage img, int x, int y, int tile)
	{
		assert tile >= 0;

		for (int yy = 0; yy < TILE_HEIGHT; yy++)
		{
			for (int xx = 0; xx < TILE_WIDTH; xx++)
			{
				img.setRGB(x*TILE_WIDTH+xx,y*TILE_HEIGHT+yy,
					tileArrayImage.getRGB(xx,tile*TILE_OFFSET_Y+yy));
			}
		}
	}

	Rectangle getViewRect(ConnectedView cv)
	{
		Rectangle rawRect = cv.scrollPane.getViewport().getViewRect();
		return new Rectangle(
			rawRect.x * 3 / cv.view.getTileSize(), 
			rawRect.y * 3 / cv.view.getTileSize(),
			rawRect.width * 3 / cv.view.getTileSize(),
			rawRect.height * 3 / cv.view.getTileSize()
			);
	}

	private void dragViewTo(Point p)
	{
		if (views.isEmpty())
			return;

		ConnectedView cv = views.get(0);
		Dimension d = cv.scrollPane.getViewport().getExtentSize();
		Dimension mapSize = cv.scrollPane.getViewport().getViewSize();

		Point np = new Point(
				p.x * cv.view.getTileSize() / 3 - d.width / 2,
				p.y * cv.view.getTileSize() / 3 - d.height / 2
				);
		np.x = Math.max(0, Math.min(np.x, mapSize.width - d.width));
		np.y = Math.max(0, Math.min(np.y, mapSize.height - d.height));

		cv.scrollPane.getViewport().setViewPosition(np);
	}

	//implements Scrollable
	public Dimension getPreferredScrollableViewportSize()
	{
		return new Dimension(120,120);
	}

	//implements Scrollable
	public int getScrollableBlockIncrement(Rectangle visibleRect, int orientation, int direction)
	{
		if (orientation == SwingConstants.VERTICAL)
			return visibleRect.height;
		else
			return visibleRect.width;
	}

	//implements Scrollable
	public boolean getScrollableTracksViewportWidth()
	{
		return false;
	}

	//implements Scrollable
	public boolean getScrollableTracksViewportHeight()
	{
		return false;
	}

	//implements Scrollable
	public int getScrollableUnitIncrement(Rectangle visibleRect, int orientation, int direction)
	{
		if (orientation == SwingConstants.VERTICAL)
			return TILE_HEIGHT;
		else
			return TILE_WIDTH;
	}

	//implements MapListener
	public void mapOverlayDataChanged(MapState overlayDataType)
	{
		repaint();
	}

	//implements MapListener
	public void spriteMoved(Sprite sprite)
	{
	}

	//implements MapListener
	public void tileChanged(int xpos, int ypos)
	{
		Rectangle r = new Rectangle(xpos*TILE_WIDTH, ypos * TILE_HEIGHT, TILE_WIDTH, TILE_HEIGHT);
		repaint(r);
	}

	//implements MapListener
	public void wholeMapChanged()
	{
		repaint();
		engine.calculateCenterMass();
		dragViewToCityCenter();
	}

	public void dragViewToCityCenter()
	{
		dragViewTo(new Point(TILE_WIDTH * engine.centerMassX + 1,
			TILE_HEIGHT * engine.centerMassY + 1));
	}

	class ConnectedView implements ChangeListener
	{
		MicropolisDrawingArea view;
		JScrollPane scrollPane;

		ConnectedView(MicropolisDrawingArea view, JScrollPane scrollPane)
		{
			this.view = view;
			this.scrollPane = scrollPane;
			scrollPane.getViewport().addChangeListener(this);
		}

		public void stateChanged(ChangeEvent ev)
		{
			repaint();
		}
	}

	public void connectView(MicropolisDrawingArea view, JScrollPane scrollPane)
	{
		ConnectedView cv = new ConnectedView(view, scrollPane);
		views.add(cv);
		repaint();
	}

	private void onMousePressed(MouseEvent ev)
	{
		if (ev.getButton() == MouseEvent.BUTTON1)
			dragViewTo(ev.getPoint());
	}

	private void onMouseDragged(MouseEvent ev)
	{
		if ((ev.getModifiersEx() & MouseEvent.BUTTON1_DOWN_MASK) == 0)
			return;

		dragViewTo(ev.getPoint());
	}
}
