## Main Game Scene Controller
extends Control

@onready var city_view: Node2D = $CityView
@onready var hud: Control = $HUD
@onready var funds_label: Label = $HUD/TopBar/FundsLabel
@onready var date_label: Label = $HUD/TopBar/DateLabel
@onready var speed_label: Label = $HUD/TopBar/SpeedLabel
@onready var pop_label: Label = $HUD/TopBar/PopLabel
@onready var file_dialog: FileDialog = $FileDialog

var current_tool: Constants.ToolType = Constants.ToolType.NONE

func _ready() -> void:
	# Connect signals
	GameManager.funds_changed.connect(_on_funds_changed)
	GameManager.date_changed.connect(_on_date_changed)
	Simulation.census_changed.connect(_on_census_changed)
	Simulation.demand_changed.connect(_on_demand_changed)
	
	# Initial UI update
	_update_hud()
	
	# Check for command line args to auto-load a city
	var args := OS.get_cmdline_args()
	for arg in args:
		if arg.ends_with(".cty"):
			_load_city(arg)
			return
	
	# Try to load a default city for testing
	var default_city := "res://assets/cities/haight.cty"
	if FileAccess.file_exists(default_city):
		_load_city(default_city)
	else:
		# Generate empty city
		Simulation.new_city()
		city_view.refresh_map()

func _input(event: InputEvent) -> void:
	# Keyboard shortcuts
	if event is InputEventKey and event.pressed:
		match event.keycode:
			KEY_SPACE:
				Simulation.toggle_pause()
				_update_speed_display()
			KEY_1:
				Simulation.set_speed(Constants.SimSpeed.SLOW)
				_update_speed_display()
			KEY_2:
				Simulation.set_speed(Constants.SimSpeed.NORMAL)
				_update_speed_display()
			KEY_3:
				Simulation.set_speed(Constants.SimSpeed.FAST)
				_update_speed_display()
			KEY_4:
				Simulation.set_speed(Constants.SimSpeed.SUPER_FAST)
				_update_speed_display()
			KEY_O:
				if event.ctrl_pressed:
					_show_open_dialog()
			KEY_N:
				if event.ctrl_pressed:
					_new_city()
			KEY_ESCAPE:
				# Could show pause menu
				pass

func _update_hud() -> void:
	funds_label.text = "$%d" % GameManager.total_funds
	date_label.text = GameManager.get_date_string()
	_update_speed_display()
	_update_population_display()

func _update_speed_display() -> void:
	var speed_names := ["Paused", "Slow", "Normal", "Fast", "Ultra"]
	var speed_idx := Simulation.speed as int
	if not Simulation.running:
		speed_label.text = "⏸ Paused"
	else:
		speed_label.text = "▶ " + speed_names[speed_idx]

func _update_population_display() -> void:
	var total: int = Simulation.res_pop + Simulation.com_pop + Simulation.ind_pop
	pop_label.text = "Pop: %d" % total

func _on_funds_changed(amount: int) -> void:
	funds_label.text = "$%d" % amount

func _on_date_changed(_year: int, _month: int) -> void:
	date_label.text = GameManager.get_date_string()

func _on_census_changed() -> void:
	_update_population_display()

func _on_demand_changed() -> void:
	# Could update RCI demand display here
	pass

func _show_open_dialog() -> void:
	file_dialog.file_mode = FileDialog.FILE_MODE_OPEN_FILE
	file_dialog.filters = ["*.cty ; City Files"]
	file_dialog.current_dir = "res://assets/cities"
	file_dialog.popup_centered(Vector2i(600, 400))

func _load_city(path: String) -> void:
	if Simulation.load_city(path):
		city_view.refresh_map()
		Simulation.start()
		_update_hud()

func _new_city() -> void:
	Simulation.new_city()
	city_view.refresh_map()
	Simulation.start()
	_update_hud()

func _on_file_dialog_file_selected(path: String) -> void:
	_load_city(path)
