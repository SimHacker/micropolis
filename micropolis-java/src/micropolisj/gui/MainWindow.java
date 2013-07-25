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
import java.io.*;
import java.net.URL;
import java.text.MessageFormat;
import java.text.NumberFormat;
import java.text.SimpleDateFormat;
import java.util.*;
import java.util.prefs.*;
import javax.sound.sampled.*;
import javax.swing.*;
import javax.swing.Timer;
import javax.swing.filechooser.FileNameExtensionFilter;

import micropolisj.engine.*;
import micropolisj.util.TranslationTool;

public class MainWindow extends JFrame
	implements Micropolis.Listener, EarthquakeListener
{
	Micropolis engine;
	MicropolisDrawingArea drawingArea;
	JScrollPane drawingAreaScroll;
	DemandIndicator demandInd;
	MessagesPane messagesPane;
	JLabel mapLegendLbl;
	OverlayMapView mapView;
	NotificationPane notificationPane;
	EvaluationPane evaluationPane;
	GraphsPane graphsPane;
	JLabel dateLbl;
	JLabel fundsLbl;
	JLabel popLbl;
	JLabel currentToolLbl;
	JLabel currentToolCostLbl;
	Map<MicropolisTool,JToggleButton> toolBtns;
	EnumMap<MapState,JMenuItem> mapStateMenuItems = new EnumMap<MapState,JMenuItem>(MapState.class);
	MicropolisTool currentTool;
	File currentFile;
	boolean doSounds = true;
	boolean dirty1 = false;  //indicates if a tool was successfully applied since last save
	boolean dirty2 = false;  //indicates if simulator took a step since last save
	long lastSavedTime = 0;  //real-time clock of when file was last saved
	boolean autoBudgetPending;

	static ImageIcon appIcon;
	static {
		appIcon = new ImageIcon(MainWindow.class.getResource("/micropolism.png"));
	}

	static ResourceBundle strings = ResourceBundle.getBundle("micropolisj.GuiStrings");
	static final String PRODUCT_NAME = strings.getString("PRODUCT");

	public MainWindow()
	{
		this(new Micropolis());
	}

	public MainWindow(Micropolis engine)
	{
		setIconImage(appIcon.getImage());

		this.engine = engine;

		JPanel mainArea = new JPanel(new BorderLayout());
		add(mainArea, BorderLayout.CENTER);

		drawingArea = new MicropolisDrawingArea(engine);
		drawingAreaScroll = new JScrollPane(drawingArea);
		mainArea.add(drawingAreaScroll);

		makeMenu();
		JToolBar tb = makeToolbar();
		mainArea.add(tb, BorderLayout.WEST);

		Box evalGraphsBox = new Box(BoxLayout.Y_AXIS);
		mainArea.add(evalGraphsBox, BorderLayout.SOUTH);

		graphsPane = new GraphsPane(engine);
		graphsPane.setVisible(false);
		evalGraphsBox.add(graphsPane);

		evaluationPane = new EvaluationPane(engine);
		evaluationPane.setVisible(false);
		evalGraphsBox.add(evaluationPane, BorderLayout.SOUTH);

		JPanel leftPane = new JPanel(new GridBagLayout());
		add(leftPane, BorderLayout.WEST);

		GridBagConstraints c = new GridBagConstraints();
		c.gridx = c.gridy = 0;
		c.anchor = GridBagConstraints.SOUTHWEST;
		c.insets = new Insets(4,4,4,4);
		c.weightx = 1.0;

		demandInd = new DemandIndicator();
		leftPane.add(demandInd, c);

		c.gridx = 1;
		c.weightx = 0.0;
		c.fill = GridBagConstraints.BOTH;
		c.insets = new Insets(4, 20, 4, 4);

		leftPane.add(makeDateFunds(), c);

		c.gridx = 0;
		c.gridy = 1;
		c.gridwidth = 2;
		c.weighty = 0.0;
		c.anchor = GridBagConstraints.NORTH;
		c.insets = new Insets(0,0,0,0);

		JPanel mapViewContainer = new JPanel(new BorderLayout());
		mapViewContainer.setBorder(BorderFactory.createLineBorder(Color.BLACK));
		leftPane.add(mapViewContainer, c);

		JMenuBar mapMenu = new JMenuBar();
		mapViewContainer.add(mapMenu, BorderLayout.NORTH);

		JMenu zonesMenu = new JMenu(strings.getString("menu.zones"));
		mapMenu.add(zonesMenu);

		zonesMenu.add(makeMapStateMenuItem(strings.getString("menu.zones.ALL"), MapState.ALL));
		zonesMenu.add(makeMapStateMenuItem(strings.getString("menu.zones.RESIDENTIAL"), MapState.RESIDENTIAL));
		zonesMenu.add(makeMapStateMenuItem(strings.getString("menu.zones.COMMERCIAL"), MapState.COMMERCIAL));
		zonesMenu.add(makeMapStateMenuItem(strings.getString("menu.zones.INDUSTRIAL"), MapState.INDUSTRIAL));
		zonesMenu.add(makeMapStateMenuItem(strings.getString("menu.zones.TRANSPORT"), MapState.TRANSPORT));

		JMenu overlaysMenu = new JMenu(strings.getString("menu.overlays"));
		mapMenu.add(overlaysMenu);

		overlaysMenu.add(makeMapStateMenuItem(strings.getString("menu.overlays.POPDEN_OVERLAY"), MapState.POPDEN_OVERLAY));
		overlaysMenu.add(makeMapStateMenuItem(strings.getString("menu.overlays.GROWTHRATE_OVERLAY"), MapState.GROWTHRATE_OVERLAY));
		overlaysMenu.add(makeMapStateMenuItem(strings.getString("menu.overlays.LANDVALUE_OVERLAY"), MapState.LANDVALUE_OVERLAY));
		overlaysMenu.add(makeMapStateMenuItem(strings.getString("menu.overlays.CRIME_OVERLAY"), MapState.CRIME_OVERLAY));
		overlaysMenu.add(makeMapStateMenuItem(strings.getString("menu.overlays.POLLUTE_OVERLAY"), MapState.POLLUTE_OVERLAY));
		overlaysMenu.add(makeMapStateMenuItem(strings.getString("menu.overlays.TRAFFIC_OVERLAY"), MapState.TRAFFIC_OVERLAY));
		overlaysMenu.add(makeMapStateMenuItem(strings.getString("menu.overlays.POWER_OVERLAY"), MapState.POWER_OVERLAY));
		overlaysMenu.add(makeMapStateMenuItem(strings.getString("menu.overlays.FIRE_OVERLAY"), MapState.FIRE_OVERLAY));
		overlaysMenu.add(makeMapStateMenuItem(strings.getString("menu.overlays.POLICE_OVERLAY"), MapState.POLICE_OVERLAY));

		mapMenu.add(Box.createHorizontalGlue());
		mapLegendLbl = new JLabel();
		mapMenu.add(mapLegendLbl);

		mapView = new OverlayMapView(engine);
		mapView.connectView(drawingArea, drawingAreaScroll);
		mapViewContainer.add(mapView, BorderLayout.CENTER);

		setMapState(MapState.ALL);

		c.gridx = 0;
		c.gridy = 2;
		c.gridwidth = 2;
		c.weighty = 1.0;
		c.fill = GridBagConstraints.BOTH;
		c.insets = new Insets(0,0,0,0);

		messagesPane = new MessagesPane();
		JScrollPane scroll2 = new JScrollPane(messagesPane);
		scroll2.setVerticalScrollBarPolicy(JScrollPane.VERTICAL_SCROLLBAR_ALWAYS);
		scroll2.setPreferredSize(new Dimension(0,0));
		scroll2.setMinimumSize(new Dimension(0,0));
		leftPane.add(scroll2, c);

		c.gridy = 3;
		c.weighty = 0.0;
		notificationPane = new NotificationPane(engine);
		leftPane.add(notificationPane, c);

		pack();
		setDefaultCloseOperation(WindowConstants.DO_NOTHING_ON_CLOSE);
		setLocationRelativeTo(null);

		InputMap inputMap = getRootPane().getInputMap(JComponent.WHEN_ANCESTOR_OF_FOCUSED_COMPONENT);
		inputMap.put(KeyStroke.getKeyStroke("ADD"), "zoomIn");
		inputMap.put(KeyStroke.getKeyStroke("shift EQUALS"), "zoomIn");
		inputMap.put(KeyStroke.getKeyStroke("SUBTRACT"), "zoomOut");
		inputMap.put(KeyStroke.getKeyStroke("MINUS"), "zoomOut");

		getRootPane().getActionMap().put("zoomIn", new AbstractAction() {
			public void actionPerformed(ActionEvent evt) {
				doZoom(1);
			}
			});
		getRootPane().getActionMap().put("zoomOut", new AbstractAction() {
			public void actionPerformed(ActionEvent evt) {
				doZoom(-1);
			}
			});

		MouseAdapter mouse = new MouseAdapter() {
			public void mousePressed(MouseEvent ev)
			{
				try {
					onToolDown(ev);
				} catch (Throwable e) {
					showErrorMessage(e);
				}
			}
			public void mouseReleased(MouseEvent ev)
			{
				try {
					onToolUp(ev);
				} catch (Throwable e) {
					showErrorMessage(e);
				}
			}
			public void mouseDragged(MouseEvent ev)
			{
				try {
					onToolDrag(ev);
				} catch (Throwable e) {
					showErrorMessage(e);
				}
			}
			public void mouseMoved(MouseEvent ev)
			{
				try {
					onToolHover(ev);
				} catch (Throwable e) {
					showErrorMessage(e);
				}
			}
			public void mouseExited(MouseEvent ev)
			{
				try {
					onToolExited(ev);
				} catch (Throwable e) {
					showErrorMessage(e);
				}
			}
			public void mouseWheelMoved(MouseWheelEvent evt)
			{
				try {
					onMouseWheelMoved(evt);
				} catch (Throwable e) {
					showErrorMessage(e);
				}
			}
			};
		drawingArea.addMouseListener(mouse);
		drawingArea.addMouseMotionListener(mouse);
		drawingArea.addMouseWheelListener(mouse);

		addWindowListener(new WindowAdapter() {
			public void windowClosing(WindowEvent ev)
			{
				closeWindow();
			}
			public void windowClosed(WindowEvent ev)
			{
				onWindowClosed(ev);
			}
			});

		Preferences prefs = Preferences.userNodeForPackage(MainWindow.class);
		doSounds = prefs.getBoolean(SOUNDS_PREF, true);

		// start things up
		mapView.setEngine(engine);
		engine.addListener(this);
		engine.addEarthquakeListener(this);
		reloadFunds();
		reloadOptions();
		startTimer();
		makeClean();
	}

	public void setEngine(Micropolis newEngine)
	{
		if (engine != null) { // old engine
			engine.removeListener(this);
			engine.removeEarthquakeListener(this);
		}

		engine = newEngine;

		if (engine != null) { // new engine
			engine.addListener(this);
			engine.addEarthquakeListener(this);
		}

		boolean timerEnabled = isTimerActive();
		if (timerEnabled) {
			stopTimer();
		}
		stopEarthquake();

		drawingArea.setEngine(engine);
		mapView.setEngine(engine);   //must change mapView after drawingArea
		evaluationPane.setEngine(engine);
		demandInd.setEngine(engine);
		graphsPane.setEngine(engine);
		reloadFunds();
		reloadOptions();
		notificationPane.setVisible(false);

		if (timerEnabled) {
			startTimer();
		}
	}

	boolean needsSaved()
	{
		if (dirty1)    //player has built something since last save
			return true;

		if (!dirty2)   //no simulator ticks since last save
			return false;

		// simulation time has passed since last save, but the player
		// hasn't done anything. Whether we need to prompt for save
		// will depend on how much real time has elapsed.
		// The threshold is 30 seconds.

		return (System.currentTimeMillis() - lastSavedTime > 30000);
	}

	boolean maybeSaveCity()
	{
		if (needsSaved())
		{
			boolean timerEnabled = isTimerActive();
			if (timerEnabled) {
				stopTimer();
			}

			try {
			int rv = JOptionPane.showConfirmDialog(
				this,
				strings.getString("main.save_query"),
				PRODUCT_NAME,
				JOptionPane.YES_NO_CANCEL_OPTION,
				JOptionPane.WARNING_MESSAGE);
			if (rv == JOptionPane.CANCEL_OPTION)
				return false;

			if (rv == JOptionPane.YES_OPTION) {
				if (!onSaveCityClicked()) {
					// canceled save dialog
					return false;
				}
			}
			}
			finally {
				if (timerEnabled) { startTimer(); }
			}
		}
		return true;
	}

	void closeWindow()
	{
		if (maybeSaveCity()) {
			dispose();
		}
	}

	JComponent makeDateFunds()
	{
		JPanel pane = new JPanel(new GridBagLayout());
		GridBagConstraints c0 = new GridBagConstraints();
		GridBagConstraints c1 = new GridBagConstraints();

		c0.gridx = 0;
		c1.gridx = 1;
		c0.gridy = c1.gridy = 0;
		c0.weightx = 1.0;
		c0.weighty = c1.weighty = 1.0;
		c0.anchor = GridBagConstraints.WEST;
		c1.anchor = GridBagConstraints.EAST;

		pane.add(new JLabel(strings.getString("main.date_label")), c0);
		dateLbl = new JLabel();
		pane.add(dateLbl, c1);

		c0.gridy = c1.gridy = 1;

		pane.add(new JLabel(strings.getString("main.funds_label")), c0);
		fundsLbl = new JLabel();
		pane.add(fundsLbl, c1);

		c0.gridy = c1.gridy = 2;

		pane.add(new JLabel(strings.getString("main.population_label")), c0);
		popLbl = new JLabel();
		pane.add(popLbl, c1);

		return pane;
	}

	private void setupKeys(JMenu menu, String prefix)
	{
		if (strings.containsKey(prefix+".key")) {
			String mnemonic = strings.getString(prefix+".key");
			menu.setMnemonic(
				KeyStroke.getKeyStroke(mnemonic).getKeyCode()
				);
		}
	}

	private void setupKeys(JMenuItem menuItem, String prefix)
	{
		if (strings.containsKey(prefix+".key")) {
			String mnemonic = strings.getString(prefix+".key");
			menuItem.setMnemonic(
				KeyStroke.getKeyStroke(mnemonic).getKeyCode()
				);
		}
		if (strings.containsKey(prefix+".shortcut")) {
			String shortcut = strings.getString(prefix+".shortcut");
			menuItem.setAccelerator(
				KeyStroke.getKeyStroke(shortcut)
				);
		}
	}

	private void makeMenu()
	{
		JMenuBar menuBar = new JMenuBar();

		JMenu gameMenu = new JMenu(strings.getString("menu.game"));
		setupKeys(gameMenu, "menu.game");
		menuBar.add(gameMenu);

		JMenuItem menuItem;
		menuItem = new JMenuItem(strings.getString("menu.game.new"));
		setupKeys(menuItem, "menu.game.new");
		menuItem.addActionListener(wrapActionListener(
			new ActionListener() {
			public void actionPerformed(ActionEvent ev)
			{
				onNewCityClicked();
			}
			}));
		gameMenu.add(menuItem);

		menuItem = new JMenuItem(strings.getString("menu.game.load"));
		setupKeys(menuItem, "menu.game.load");
		menuItem.addActionListener(wrapActionListener(
			new ActionListener() {
			public void actionPerformed(ActionEvent ev)
			{
				onLoadGameClicked();
			}
			}));
		gameMenu.add(menuItem);

		menuItem = new JMenuItem(strings.getString("menu.game.save"));
		setupKeys(menuItem, "menu.game.save");
		menuItem.addActionListener(wrapActionListener(
			new ActionListener() {
			public void actionPerformed(ActionEvent ev)
			{
				onSaveCityClicked();
			}
			}));
		gameMenu.add(menuItem);

		menuItem = new JMenuItem(strings.getString("menu.game.save_as"));
		setupKeys(menuItem, "menu.game.save_as");
		menuItem.addActionListener(wrapActionListener(
			new ActionListener() {
			public void actionPerformed(ActionEvent ev)
			{
				onSaveCityAsClicked();
			}
			}));
		gameMenu.add(menuItem);

		menuItem = new JMenuItem(strings.getString("menu.game.exit"));
		setupKeys(menuItem, "menu.game.exit");
		menuItem.addActionListener(wrapActionListener(
			new ActionListener() {
			public void actionPerformed(ActionEvent ev)
			{
				closeWindow();
			}
			}));
		gameMenu.add(menuItem);

		JMenu optionsMenu = new JMenu(strings.getString("menu.options"));
		setupKeys(optionsMenu, "menu.options");
		menuBar.add(optionsMenu);

		JMenu levelMenu = new JMenu(strings.getString("menu.difficulty"));
		setupKeys(levelMenu, "menu.difficulty");
		optionsMenu.add(levelMenu);

		difficultyMenuItems = new HashMap<Integer,JMenuItem>();
		for (int i = GameLevel.MIN_LEVEL; i <= GameLevel.MAX_LEVEL; i++)
		{
			final int level = i;
			menuItem = new JRadioButtonMenuItem(strings.getString("menu.difficulty."+level));
			setupKeys(menuItem, "menu.difficulty."+level);
			menuItem.addActionListener(wrapActionListener(
				new ActionListener() {
				public void actionPerformed(ActionEvent evt) {
					onDifficultyClicked(level);
				}
			}));
			levelMenu.add(menuItem);
			difficultyMenuItems.put(level, menuItem);
		}

		autoBudgetMenuItem = new JCheckBoxMenuItem(strings.getString("menu.options.auto_budget"));
		setupKeys(autoBudgetMenuItem, "menu.options.auto_budget");
		autoBudgetMenuItem.addActionListener(wrapActionListener(
			new ActionListener() {
			public void actionPerformed(ActionEvent ev)
			{
				onAutoBudgetClicked();
			}
			}));
		optionsMenu.add(autoBudgetMenuItem);

		autoBulldozeMenuItem = new JCheckBoxMenuItem(strings.getString("menu.options.auto_bulldoze"));
		setupKeys(autoBulldozeMenuItem, "menu.options.auto_bulldoze");
		autoBulldozeMenuItem.addActionListener(wrapActionListener(
			new ActionListener() {
			public void actionPerformed(ActionEvent ev)
			{
				onAutoBulldozeClicked();
			}
			}));
		optionsMenu.add(autoBulldozeMenuItem);

		disastersMenuItem = new JCheckBoxMenuItem(strings.getString("menu.options.disasters"));
		setupKeys(disastersMenuItem, "menu.options.disasters");
		disastersMenuItem.addActionListener(wrapActionListener(
			new ActionListener() {
			public void actionPerformed(ActionEvent ev)
			{
				onDisastersClicked();
			}
			}));
		optionsMenu.add(disastersMenuItem);

		soundsMenuItem = new JCheckBoxMenuItem(strings.getString("menu.options.sound"));
		setupKeys(soundsMenuItem, "menu.options.sound");
		soundsMenuItem.addActionListener(wrapActionListener(
			new ActionListener() {
			public void actionPerformed(ActionEvent ev)
			{
				onSoundClicked();
			}
			}));
		optionsMenu.add(soundsMenuItem);

		JMenu disastersMenu = new JMenu(strings.getString("menu.disasters"));
		setupKeys(disastersMenu, "menu.disasters");
		menuBar.add(disastersMenu);

		menuItem = new JMenuItem(strings.getString("menu.disasters.MONSTER"));
		setupKeys(menuItem, "menu.disasters.MONSTER");
		menuItem.addActionListener(wrapActionListener(
			new ActionListener() {
			public void actionPerformed(ActionEvent ev)
			{
				onInvokeDisasterClicked(Disaster.MONSTER);
			}
			}));
		disastersMenu.add(menuItem);

		menuItem = new JMenuItem(strings.getString("menu.disasters.FIRE"));
		setupKeys(menuItem, "menu.disasters.FIRE");
		menuItem.addActionListener(wrapActionListener(
			new ActionListener() {
			public void actionPerformed(ActionEvent ev)
			{
				onInvokeDisasterClicked(Disaster.FIRE);
			}
			}));
		disastersMenu.add(menuItem);

		menuItem = new JMenuItem(strings.getString("menu.disasters.FLOOD"));
		setupKeys(menuItem, "menu.disasters.FLOOD");
		menuItem.addActionListener(wrapActionListener(
			new ActionListener() {
			public void actionPerformed(ActionEvent ev)
			{
				onInvokeDisasterClicked(Disaster.FLOOD);
			}
			}));
		disastersMenu.add(menuItem);

		menuItem = new JMenuItem(strings.getString("menu.disasters.MELTDOWN"));
		setupKeys(menuItem, "menu.disasters.MELTDOWN");
		menuItem.addActionListener(wrapActionListener(
			new ActionListener() {
			public void actionPerformed(ActionEvent ev)
			{
				onInvokeDisasterClicked(Disaster.MELTDOWN);
			}
			}));
		disastersMenu.add(menuItem);

		menuItem = new JMenuItem(strings.getString("menu.disasters.TORNADO"));
		setupKeys(menuItem, "menu.disasters.TORNADO");
		menuItem.addActionListener(wrapActionListener(
			new ActionListener() {
			public void actionPerformed(ActionEvent ev)
			{
				onInvokeDisasterClicked(Disaster.TORNADO);
			}
			}));
		disastersMenu.add(menuItem);

		menuItem = new JMenuItem(strings.getString("menu.disasters.EARTHQUAKE"));
		setupKeys(menuItem, "menu.disasters.EARTHQUAKE");
		menuItem.addActionListener(wrapActionListener(
			new ActionListener() {
			public void actionPerformed(ActionEvent ev)
			{
				onInvokeDisasterClicked(Disaster.EARTHQUAKE);
			}
			}));
		disastersMenu.add(menuItem);

		JMenu priorityMenu = new JMenu(strings.getString("menu.speed"));
		setupKeys(priorityMenu, "menu.speed");
		menuBar.add(priorityMenu);

		priorityMenuItems = new EnumMap<Speed,JMenuItem>(Speed.class);
		menuItem = new JRadioButtonMenuItem(strings.getString("menu.speed.SUPER_FAST"));
		setupKeys(menuItem, "menu.speed.SUPER_FAST");
		menuItem.addActionListener(wrapActionListener(
			new ActionListener() {
			public void actionPerformed(ActionEvent ev)
			{
				onPriorityClicked(Speed.SUPER_FAST);
			}
			}));
		priorityMenu.add(menuItem);
		priorityMenuItems.put(Speed.SUPER_FAST, menuItem);

		menuItem = new JRadioButtonMenuItem(strings.getString("menu.speed.FAST"));
		setupKeys(menuItem, "menu.speed.FAST");
		menuItem.addActionListener(wrapActionListener(
			new ActionListener() {
			public void actionPerformed(ActionEvent ev)
			{
				onPriorityClicked(Speed.FAST);
			}
			}));
		priorityMenu.add(menuItem);
		priorityMenuItems.put(Speed.FAST, menuItem);

		menuItem = new JRadioButtonMenuItem(strings.getString("menu.speed.NORMAL"));
		setupKeys(menuItem, "menu.speed.NORMAL");
		menuItem.addActionListener(wrapActionListener(
			new ActionListener() {
			public void actionPerformed(ActionEvent ev)
			{
				onPriorityClicked(Speed.NORMAL);
			}
			}));
		priorityMenu.add(menuItem);
		priorityMenuItems.put(Speed.NORMAL, menuItem);

		menuItem = new JRadioButtonMenuItem(strings.getString("menu.speed.SLOW"));
		setupKeys(menuItem, "menu.speed.SLOW");
		menuItem.addActionListener(wrapActionListener(
			new ActionListener() {
			public void actionPerformed(ActionEvent ev)
			{
				onPriorityClicked(Speed.SLOW);
			}
			}));
		priorityMenu.add(menuItem);
		priorityMenuItems.put(Speed.SLOW, menuItem);

		menuItem = new JRadioButtonMenuItem(strings.getString("menu.speed.PAUSED"));
		setupKeys(menuItem, "menu.speed.PAUSED");
		menuItem.addActionListener(wrapActionListener(
			new ActionListener() {
			public void actionPerformed(ActionEvent ev)
			{
				onPriorityClicked(Speed.PAUSED);
			}
			}));
		priorityMenu.add(menuItem);
		priorityMenuItems.put(Speed.PAUSED, menuItem);

		JMenu windowsMenu = new JMenu(strings.getString("menu.windows"));
		setupKeys(windowsMenu, "menu.windows");
		menuBar.add(windowsMenu);

		menuItem = new JMenuItem(strings.getString("menu.windows.budget"));
		setupKeys(menuItem, "menu.windows.budget");
		menuItem.addActionListener(wrapActionListener(
			new ActionListener() {
			public void actionPerformed(ActionEvent ev)
			{
				onViewBudgetClicked();
			}
			}));
		windowsMenu.add(menuItem);

		menuItem = new JMenuItem(strings.getString("menu.windows.evaluation"));
		setupKeys(menuItem, "menu.windows.evaluation");
		menuItem.addActionListener(wrapActionListener(
			new ActionListener() {
			public void actionPerformed(ActionEvent ev)
			{
				onViewEvaluationClicked();
			}
			}));
		windowsMenu.add(menuItem);

		menuItem = new JMenuItem(strings.getString("menu.windows.graph"));
		setupKeys(menuItem, "menu.windows.graph");
		menuItem.addActionListener(wrapActionListener(
			new ActionListener() {
			public void actionPerformed(ActionEvent ev)
			{
				onViewGraphClicked();
			}
			}));
		windowsMenu.add(menuItem);

		JMenu helpMenu = new JMenu(strings.getString("menu.help"));
		setupKeys(helpMenu, "menu.help");
		menuBar.add(helpMenu);

		menuItem = new JMenuItem(strings.getString("menu.help.launch-translation-tool"));
		setupKeys(menuItem, "menu.help.launch-translation-tool");
		menuItem.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent evt)
			{
				onLaunchTranslationToolClicked();
			}});
		helpMenu.add(menuItem);

		menuItem = new JMenuItem(strings.getString("menu.help.about"));
		setupKeys(menuItem, "menu.help.about");
		menuItem.addActionListener(wrapActionListener(
			new ActionListener() {
			public void actionPerformed(ActionEvent ev)
			{
				onAboutClicked();
			}
			}));
		helpMenu.add(menuItem);

		setJMenuBar(menuBar);
	}

	private Micropolis getEngine()
	{
		return engine;
	}

	JMenuItem autoBudgetMenuItem;
	JMenuItem autoBulldozeMenuItem;
	JMenuItem disastersMenuItem;
	JMenuItem soundsMenuItem;
	Map<Speed,JMenuItem> priorityMenuItems;
	Map<Integer,JMenuItem> difficultyMenuItems;

	private void onAutoBudgetClicked()
	{
		dirty1 = true;
		getEngine().toggleAutoBudget();
	}

	private void onAutoBulldozeClicked()
	{
		dirty1 = true;
		getEngine().toggleAutoBulldoze();
	}

	private void onDisastersClicked()
	{
		dirty1 = true;
		getEngine().toggleDisasters();
	}

	static final String SOUNDS_PREF = "enable_sounds";
	private void onSoundClicked()
	{
		doSounds = !doSounds;
		Preferences prefs = Preferences.userNodeForPackage(MainWindow.class);
		prefs.putBoolean(SOUNDS_PREF, doSounds);
		reloadOptions();
	}

	void makeClean()
	{
		dirty1 = false;
		dirty2 = false;
		lastSavedTime = System.currentTimeMillis();
		if (currentFile != null) {
			String fileName = currentFile.getName();
			if (fileName.endsWith("."+EXTENSION)) {
				fileName = fileName.substring(0, fileName.length() - 1 - EXTENSION.length());
			}
			setTitle(MessageFormat.format(strings.getString("main.caption_named_city"), fileName));
		}
		else {
			setTitle(strings.getString("main.caption_unnamed_city"));
		}
	}

	private boolean onSaveCityClicked()
	{
		if (currentFile == null)
		{
			return onSaveCityAsClicked();
		}

		try
		{
			getEngine().save(currentFile);
			makeClean();
			return true;
		}
		catch (IOException e)
		{
			e.printStackTrace(System.err);
			JOptionPane.showMessageDialog(this, e, strings.getString("main.error_caption"),
				JOptionPane.ERROR_MESSAGE);
			return false;
		}
	}

	static final String EXTENSION = "cty";
	private boolean onSaveCityAsClicked()
	{
		boolean timerEnabled = isTimerActive();
		if (timerEnabled) {
			stopTimer();
		}
		try
		{
			JFileChooser fc = new JFileChooser();
			FileNameExtensionFilter filter1 = new FileNameExtensionFilter(strings.getString("cty_file"), EXTENSION);
			fc.setFileFilter(filter1);
			int rv = fc.showSaveDialog(this);
			if (rv == JFileChooser.APPROVE_OPTION) {
				currentFile = fc.getSelectedFile();
				if (!currentFile.getName().endsWith("."+EXTENSION)) {
					currentFile = new File(currentFile.getPath()+"."+EXTENSION);
				}
				getEngine().save(currentFile);
				makeClean();
				return true;
			}
		}
		catch (Exception e)
		{
			e.printStackTrace(System.err);
			JOptionPane.showMessageDialog(this, e, strings.getString("main.error_caption"),
				JOptionPane.ERROR_MESSAGE);
		}
		finally
		{
			if (timerEnabled) { startTimer(); }
		}
		return false;
	}

	private void onLoadGameClicked()
	{
		// check if user wants to save their current city
		if (!maybeSaveCity()) {
			return;
		}

		boolean timerEnabled = isTimerActive();
		if (timerEnabled) {
			stopTimer();
		}

		try
		{
			JFileChooser fc = new JFileChooser();
			FileNameExtensionFilter filter1 = new FileNameExtensionFilter(strings.getString("cty_file"), EXTENSION);
			fc.setFileFilter(filter1);

			assert !isTimerActive();

			int rv = fc.showOpenDialog(this);
			if (rv == JFileChooser.APPROVE_OPTION) {
				File file = fc.getSelectedFile();
				Micropolis newEngine = new Micropolis();
				newEngine.load(file);
				setEngine(newEngine);
				currentFile = file;
				makeClean();
			}
		}
		catch (Exception e)
		{
			e.printStackTrace(System.err);
			JOptionPane.showMessageDialog(this, e, strings.getString("main.error_caption"),
				JOptionPane.ERROR_MESSAGE);
		}
		finally {
			if (timerEnabled) {
				startTimer();
			}
		}
	}

	private JToggleButton makeToolBtn(final MicropolisTool tool)
	{
		JToggleButton btn = new JToggleButton();
		btn.setIcon(new ImageIcon(MainWindow.class.getResource(strings.getString("tool."+tool.name()+".icon"))));
		btn.setSelectedIcon(new ImageIcon(MainWindow.class.getResource(strings.getString("tool."+tool.name()+".selected_icon"))));
		btn.setToolTipText(strings.getString("tool."+tool.name()+".tip"));
		btn.setMargin(new Insets(0,0,0,0));
		btn.setBorderPainted(false);
		btn.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent ev)
			{
				selectTool(tool);
			}
			});
		toolBtns.put(tool, btn);
		return btn;
	}

	private JToolBar makeToolbar()
	{
		toolBtns = new EnumMap<MicropolisTool, JToggleButton>(MicropolisTool.class);

		JToolBar toolBar = new JToolBar(strings.getString("main.tools_caption"), JToolBar.VERTICAL);
		toolBar.setFloatable(false);
		toolBar.setRollover(false);

		JPanel gridBox = new JPanel(new GridBagLayout());
		toolBar.add(gridBox);

		GridBagConstraints c = new GridBagConstraints();
		c.gridx = c.gridy = 0;
		c.anchor = GridBagConstraints.NORTH;
		c.insets = new Insets(8,0,0,0);
		currentToolLbl = new JLabel(" ");
		gridBox.add(currentToolLbl, c);

		c.gridy = 1;
		c.insets = new Insets(0,0,12,0);
		currentToolCostLbl = new JLabel(" ");
		gridBox.add(currentToolCostLbl, c);

		c.gridy++;
		c.fill = GridBagConstraints.NONE;
		c.weightx = 1.0;
		c.insets = new Insets(0,0,0,0);
		Box b0 = new Box(BoxLayout.X_AXIS);
		gridBox.add(b0,c);

		b0.add(makeToolBtn(MicropolisTool.BULLDOZER));
		b0.add(makeToolBtn(MicropolisTool.WIRE));
		b0.add(makeToolBtn(MicropolisTool.PARK));

		c.gridy++;
		Box b1 = new Box(BoxLayout.X_AXIS);
		gridBox.add(b1,c);

		b1.add(makeToolBtn(MicropolisTool.ROADS));
		b1.add(makeToolBtn(MicropolisTool.RAIL));

		c.gridy++;
		Box b2 = new Box(BoxLayout.X_AXIS);
		gridBox.add(b2,c);

		b2.add(makeToolBtn(MicropolisTool.RESIDENTIAL));
		b2.add(makeToolBtn(MicropolisTool.COMMERCIAL));
		b2.add(makeToolBtn(MicropolisTool.INDUSTRIAL));

		c.gridy++;
		Box b3 = new Box(BoxLayout.X_AXIS);
		gridBox.add(b3,c);

		b3.add(makeToolBtn(MicropolisTool.FIRE));
		b3.add(makeToolBtn(MicropolisTool.QUERY));
		b3.add(makeToolBtn(MicropolisTool.POLICE));

		c.gridy++;
		Box b4 = new Box(BoxLayout.X_AXIS);
		gridBox.add(b4,c);

		b4.add(makeToolBtn(MicropolisTool.POWERPLANT));
		b4.add(makeToolBtn(MicropolisTool.NUCLEAR));

		c.gridy++;
		Box b5 = new Box(BoxLayout.X_AXIS);
		gridBox.add(b5,c);

		b5.add(makeToolBtn(MicropolisTool.STADIUM));
		b5.add(makeToolBtn(MicropolisTool.SEAPORT));

		c.gridy++;
		Box b6 = new Box(BoxLayout.X_AXIS);
		gridBox.add(b6,c);

		b6.add(makeToolBtn(MicropolisTool.AIRPORT));

		// add glue to make all elements align toward top
		c.gridy++;
		c.weighty = 1.0;
		gridBox.add(new JLabel(), c);

		return toolBar;
	}

	private void selectTool(MicropolisTool newTool)
	{
		toolBtns.get(newTool).setSelected(true);
		if (newTool == currentTool) {
			return;
		}

		if (currentTool != null) {
			toolBtns.get(currentTool).setSelected(false);
		}

		currentTool = newTool;

		currentToolLbl.setText(strings.getString("tool."+currentTool.name()+".name"));

		int cost = currentTool.getToolCost();
		currentToolCostLbl.setText(cost != 0 ? formatFunds(cost) : " ");
	}

	private void onNewCityClicked()
	{
		if (maybeSaveCity()) {
			doNewCity(false);
		}
	}

	public void doNewCity(boolean firstTime)
	{
		boolean timerEnabled = isTimerActive();
		if (timerEnabled) {
			stopTimer();
		}

		new NewCityDialog(this, !firstTime).setVisible(true);

		if (timerEnabled) {
			startTimer();
		}
	}

	void doQueryTool(int xpos, int ypos)
	{
		if (!engine.testBounds(xpos, ypos))
			return;

		ZoneStatus z = engine.queryZoneStatus(xpos, ypos);
		notificationPane.showZoneStatus(engine, xpos, ypos, z);
	}

	private void doZoom(int dir, Point mousePt)
	{
		int oldZoom = drawingArea.getTileSize();
		int newZoom = dir < 0 ? (oldZoom / 2) : (oldZoom * 2);
		if (newZoom < 8) { newZoom = 8; }
		if (newZoom > 32) { newZoom = 32; }

		if (oldZoom != newZoom)
		{
			// preserve effective mouse position in viewport when changing zoom level
			double f = (double)newZoom / (double)oldZoom;
			Point pos = drawingAreaScroll.getViewport().getViewPosition();
			int newX = (int)Math.round(mousePt.x * f - (mousePt.x - pos.x));
			int newY = (int)Math.round(mousePt.y * f - (mousePt.y - pos.y));
			drawingArea.selectTileSize(newZoom);
			drawingAreaScroll.validate();
			drawingAreaScroll.getViewport().setViewPosition(new Point(newX, newY));
		}
	}

	private void doZoom(int dir)
	{
		Rectangle rect = drawingAreaScroll.getViewport().getViewRect();
		doZoom(dir,
			new Point(rect.x + rect.width/2,
				rect.y + rect.height/2
				)
			);
	}

	private void onMouseWheelMoved(MouseWheelEvent evt)
	{
		if (evt.getWheelRotation() < 0) {
			doZoom(1, evt.getPoint());
		}
		else {
			doZoom(-1, evt.getPoint());
		}
	}

	// used when a tool is being pressed
	ToolStroke toolStroke;

	// where the tool was last applied during the current drag
	int lastX;
	int lastY;

	private void onToolDown(MouseEvent ev)
	{
		if (ev.getButton() == MouseEvent.BUTTON3) {
			CityLocation loc = drawingArea.getCityLocation(ev.getX(), ev.getY());
			doQueryTool(loc.x, loc.y);
			return;
		}

		if (ev.getButton() != MouseEvent.BUTTON1)
			return;

		if (currentTool == null)
			return;

		CityLocation loc = drawingArea.getCityLocation(ev.getX(), ev.getY());
		int x = loc.x;
		int y = loc.y;

		if (currentTool == MicropolisTool.QUERY) {
			doQueryTool(x, y);
			this.toolStroke = null;
		}
		else {
			this.toolStroke = currentTool.beginStroke(engine, x, y);
			previewTool();
		}

		this.lastX = x;
		this.lastY = y;
	}

	private void onToolUp(MouseEvent ev)
	{
		if (toolStroke != null) {
			drawingArea.setToolPreview(null);

			CityLocation loc = toolStroke.getLocation();
			ToolResult tr = toolStroke.apply();
			showToolResult(loc, tr);
			toolStroke = null;
		}

		onToolHover(ev);

		if (autoBudgetPending) {
			autoBudgetPending = false;
			showBudgetWindow(true);
		}
	}

	void previewTool()
	{
		assert this.toolStroke != null;
		assert this.currentTool != null;

		drawingArea.setToolCursor(
			toolStroke.getBounds(),
			currentTool
			);
		drawingArea.setToolPreview(
			toolStroke.getPreview()
			);
	}

	private void onToolDrag(MouseEvent ev)
	{
		if (currentTool == null)
			return;
		if ((ev.getModifiersEx() & MouseEvent.BUTTON1_DOWN_MASK) == 0)
			return;

		CityLocation loc = drawingArea.getCityLocation(ev.getX(), ev.getY());
		int x = loc.x;
		int y = loc.y;
		if (x == lastX && y == lastY)
			return;

		if (toolStroke != null) {
			toolStroke.dragTo(x, y);
			previewTool();
		}
		else if (currentTool == MicropolisTool.QUERY) {
			doQueryTool(x, y);
		}

		lastX = x;
		lastY = y;
	}

	private void onToolHover(MouseEvent ev)
	{
		if (currentTool == null || currentTool == MicropolisTool.QUERY)
		{
			drawingArea.setToolCursor(null);
			return;
		}

		CityLocation loc = drawingArea.getCityLocation(ev.getX(), ev.getY());
		int x = loc.x;
		int y = loc.y;
		int w = currentTool.getWidth();
		int h = currentTool.getHeight();

		if (w >= 3)
			x--;
		if (h >= 3)
			y--;

		drawingArea.setToolCursor(new Rectangle(x,y,w,h), currentTool);
	}

	private void onToolExited(MouseEvent ev)
	{
		drawingArea.setToolCursor(null);
	}

	private void showToolResult(CityLocation loc, ToolResult result)
	{
		switch (result) {
		case SUCCESS:
			citySound(currentTool == MicropolisTool.BULLDOZER ? Sound.BULLDOZE : Sound.BUILD, loc);
			dirty1 = true;
			break;

		case NONE: break;
		case UH_OH:
			messagesPane.appendCityMessage(MicropolisMessage.BULLDOZE_FIRST);
			citySound(Sound.UHUH, loc);
			break;

		case INSUFFICIENT_FUNDS:
			messagesPane.appendCityMessage(MicropolisMessage.INSUFFICIENT_FUNDS);
			citySound(Sound.SORRY, loc);
			break;

		default:
			assert false;
		}
	}

	public static String formatFunds(int funds)
	{
		return MessageFormat.format(
			strings.getString("funds"), funds
			);
	}

	public static String formatGameDate(int cityTime)
	{
		Calendar c = Calendar.getInstance();
		c.set(1900 + cityTime/48,
			(cityTime%48)/4,
			(cityTime%4)*7 + 1
			);

		return MessageFormat.format(
			strings.getString("citytime"),
			c.getTime()
			);
	}

	private void updateDateLabel()
	{
		dateLbl.setText(formatGameDate(engine.cityTime));

		NumberFormat nf = NumberFormat.getInstance();
		popLbl.setText(nf.format(getEngine().getCityPopulation()));
	}

	Timer simTimer;
	Timer shakeTimer;

	private void startTimer()
	{
		final Micropolis engine = getEngine();
		final int count = engine.simSpeed.simStepsPerUpdate;

		assert !isTimerActive();

		if (engine.simSpeed == Speed.PAUSED)
			return;

		if (currentEarthquake != null)
		{
			int interval = 3000 / MicropolisDrawingArea.SHAKE_STEPS;
			shakeTimer = new Timer(interval, new ActionListener() {
				public void actionPerformed(ActionEvent evt) {
					currentEarthquake.oneStep();
					if (currentEarthquake.count == 0) {
						stopTimer();
						currentEarthquake = null;
						startTimer();
					}
				}});
			shakeTimer.start();
			return;
		}

		ActionListener taskPerformer = new ActionListener() {
		public void actionPerformed(ActionEvent evt)
		{
			for (int i = 0; i < count; i++)
			{
				engine.animate();
				if (!engine.autoBudget && engine.isBudgetTime())
				{
					showAutoBudget();
					return;
				}
			}
			updateDateLabel();
			dirty2 = true;
		}};
		taskPerformer = wrapActionListener(taskPerformer);

		assert simTimer == null;
		simTimer = new Timer(engine.simSpeed.animationDelay, taskPerformer);
		simTimer.start();
	}

	ActionListener wrapActionListener(final ActionListener l)
	{
		return new ActionListener() {
		public void actionPerformed(ActionEvent evt) {
			try {
				l.actionPerformed(evt);
			} catch (Throwable e) {
				showErrorMessage(e);
			}
		}};
	}

	private void showErrorMessage(Throwable e)
	{
		StringWriter w = new StringWriter();
		e.printStackTrace(new PrintWriter(w));

		JTextPane stackTracePane = new JTextPane();
		stackTracePane.setEditable(false);
		stackTracePane.setText(w.toString());

		final JScrollPane detailsPane = new JScrollPane(stackTracePane);
		detailsPane.setVerticalScrollBarPolicy(JScrollPane.VERTICAL_SCROLLBAR_ALWAYS);
		detailsPane.setPreferredSize(new Dimension(480,240));
		detailsPane.setMinimumSize(new Dimension(0,0));

		int rv = JOptionPane.showOptionDialog(this, e,
			strings.getString("main.error_unexpected"),
			JOptionPane.DEFAULT_OPTION,
			JOptionPane.ERROR_MESSAGE,
			null,
			new String[] {
				strings.getString("main.error_show_stacktrace"),
				strings.getString("main.error_close"),
				strings.getString("main.error_shutdown")
				},
			1
			);
		if (rv == 0)
		{
			JOptionPane.showMessageDialog(this, detailsPane,
				strings.getString("main.error_unexpected"),
				JOptionPane.ERROR_MESSAGE);
		}
		if (rv == 2)
		{
			rv = JOptionPane.showConfirmDialog(
				this,
				strings.getString("error.shutdown_query"),
				strings.getString("main.error_unexpected"),
				JOptionPane.OK_CANCEL_OPTION,
				JOptionPane.WARNING_MESSAGE);
			if (rv == JOptionPane.OK_OPTION) {
				System.exit(1);
			}
		}
	}

	class EarthquakeStepper
	{
		int count = 0;
		void oneStep()
		{
			count = (count + 1) % MicropolisDrawingArea.SHAKE_STEPS;
			drawingArea.shake(count);
		}
	}
	EarthquakeStepper currentEarthquake;

	//implements EarthquakeListener
	public void earthquakeStarted()
	{
		if (isTimerActive()) {
			stopTimer();
		}

		currentEarthquake = new EarthquakeStepper();
		currentEarthquake.oneStep();
		startTimer();
	}

	void stopEarthquake()
	{
		drawingArea.shake(0);
		currentEarthquake = null;
	}

	private void stopTimer()
	{
		assert isTimerActive();

		if (simTimer != null) {
			simTimer.stop();
			simTimer = null;
		}
		if (shakeTimer != null) {
			shakeTimer.stop();
			shakeTimer = null;
		}
	}

	boolean isTimerActive()
	{
		return simTimer != null || shakeTimer != null;
	}

	private void onWindowClosed(WindowEvent ev)
	{
		if (isTimerActive()) {
			stopTimer();
		}
	}

	private void onDifficultyClicked(int newDifficulty)
	{
		getEngine().setGameLevel(newDifficulty);
	}

	private void onPriorityClicked(Speed newSpeed)
	{
		if (isTimerActive()) {
			stopTimer();
		}

		getEngine().setSpeed(newSpeed);
		startTimer();
	}

	private void onInvokeDisasterClicked(Disaster disaster)
	{
		dirty1 = true;
		switch (disaster) {
		case FIRE:
			getEngine().makeFire();
			break;
		case FLOOD:
			getEngine().makeFlood();
			break;
		case MONSTER:
			getEngine().makeMonster();
			break;
		case MELTDOWN:
			if (!getEngine().makeMeltdown()) {
				messagesPane.appendCityMessage(MicropolisMessage.NO_NUCLEAR_PLANTS);
			}
			break;
		case TORNADO:
			getEngine().makeTornado();
			break;
		case EARTHQUAKE:
			getEngine().makeEarthquake();
			break;
		default:
			assert false; //unknown disaster
		}
	}

	private void reloadFunds()
	{
		fundsLbl.setText(formatFunds(getEngine().budget.totalFunds));
	}

	//implements Micropolis.Listener
	public void cityMessage(MicropolisMessage m, CityLocation p, boolean pictureMessage)
	{
		messagesPane.appendCityMessage(m);

		if (pictureMessage && p != null)
		{
			notificationPane.showMessage(engine, m, p.x, p.y);
		}
	}

	//implements Micropolis.Listener
	public void fundsChanged()
	{
		reloadFunds();
	}

	//implements Micropolis.Listener
	public void optionsChanged()
	{
		reloadOptions();
	}

	private void reloadOptions()
	{
		autoBudgetMenuItem.setSelected(getEngine().autoBudget);
		autoBulldozeMenuItem.setSelected(getEngine().autoBulldoze);
		disastersMenuItem.setSelected(!getEngine().noDisasters);
		soundsMenuItem.setSelected(doSounds);
		for (Speed spd : priorityMenuItems.keySet())
		{
			priorityMenuItems.get(spd).setSelected(getEngine().simSpeed == spd);
		}
		for (int i = GameLevel.MIN_LEVEL; i <= GameLevel.MAX_LEVEL; i++)
		{
			difficultyMenuItems.get(i).setSelected(getEngine().gameLevel == i);
		}
	}

	//implements Micropolis.Listener
	public void citySound(Sound sound, CityLocation loc)
	{
		if (!doSounds)
			return;

		URL afile = sound.getAudioFile();
		if (afile == null)
			return;

		boolean isOnScreen = drawingAreaScroll.getViewport().getViewRect().contains(
				drawingArea.getTileBounds(loc.x, loc.y)
			);
		if (sound == Sound.HONKHONK_LOW && !isOnScreen)
			return;

		try
		{
			Clip clip = AudioSystem.getClip();
			clip.open(AudioSystem.getAudioInputStream(afile));
			clip.start();
		}
		catch (Exception e)
		{
			e.printStackTrace(System.err);
		}
	}

	//implements Micropolis.Listener
	public void censusChanged() { }
	public void demandChanged() { }
	public void evaluationChanged() { }

	void onViewBudgetClicked()
	{
		dirty1 = true;
		showBudgetWindow(false);
	}

	void onViewEvaluationClicked()
	{
		evaluationPane.setVisible(true);
	}

	void onViewGraphClicked()
	{
		graphsPane.setVisible(true);
	}

	private void showAutoBudget()
	{
		if (toolStroke == null) {
			showBudgetWindow(true);
		}
		else {
			autoBudgetPending = true;
		}
	}

	private void showBudgetWindow(boolean isEndOfYear)
	{
		boolean timerEnabled = isTimerActive();
		if (timerEnabled) {
			stopTimer();
		}

		BudgetDialog dlg = new BudgetDialog(this, getEngine());
		dlg.setModal(true);
		dlg.setVisible(true);

		if (timerEnabled) {
			startTimer();
		}
	}

	private JMenuItem makeMapStateMenuItem(String caption, final MapState state)
	{
		JMenuItem menuItem = new JRadioButtonMenuItem(caption);
		menuItem.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent evt) {
				setMapState(state);
			}
			});
		mapStateMenuItems.put(state, menuItem);
		return menuItem;
	}

	private void setMapState(MapState state)
	{
		mapStateMenuItems.get(mapView.getMapState()).setSelected(false);
		mapStateMenuItems.get(state).setSelected(true);
		mapView.setMapState(state);
		setMapLegend(state);
	}

	private void setMapLegend(MapState state)
	{
		String k = "legend_image."+state.name();
		java.net.URL iconUrl = null;
		if (strings.containsKey(k)) {
			String iconName = strings.getString(k);
			iconUrl = MainWindow.class.getResource(iconName);
		}
		if (iconUrl != null) {
			mapLegendLbl.setIcon(new ImageIcon(iconUrl));
		}
		else {
			mapLegendLbl.setIcon(null);
		}
	}

	private void onLaunchTranslationToolClicked()
	{
		if (maybeSaveCity()) {
			dispose();
			TranslationTool tt = new TranslationTool();
			tt.setVisible(true);
		}
	}

	private void onAboutClicked()
	{
		String version = getClass().getPackage().getImplementationVersion();
		String versionStr = MessageFormat.format(strings.getString("main.version_string"), version);
		versionStr = versionStr.replace("%java.version%", System.getProperty("java.version"));
		versionStr = versionStr.replace("%java.vendor%", System.getProperty("java.vendor"));

		JLabel appNameLbl = new JLabel(versionStr);
		JLabel appDetailsLbl = new JLabel(strings.getString("main.about_text"));
		JComponent [] inputs = new JComponent[] {  appNameLbl, appDetailsLbl };
		JOptionPane.showMessageDialog(this,
			inputs,
			strings.getString("main.about_caption"),
			JOptionPane.PLAIN_MESSAGE,
			appIcon);
	}
}
