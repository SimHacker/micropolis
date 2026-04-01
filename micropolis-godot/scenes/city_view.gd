## CityView - TileMap-based city renderer
extends Node2D

@onready var tile_map: TileMap = $TileMap
@onready var camera: Camera2D = $Camera2D

# Camera settings
const PAN_SPEED: float = 500.0
const ZOOM_SPEED: float = 0.1
const MIN_ZOOM: float = 0.25
const MAX_ZOOM: float = 2.0

# Tile atlas configuration
# tiles.png is 16x15360 = single column of 960 tiles (16x16 each)
const TILES_PER_ROW: int = 1
const TOTAL_TILES: int = 960

var _dragging: bool = false
var _drag_start: Vector2

func _ready() -> void:
	# Setup the TileSet programmatically for our 960-tile column
	_setup_tileset()
	
	# Connect to map data signals
	if Simulation.map_data:
		Simulation.map_data.tile_changed.connect(_on_tile_changed)
		Simulation.map_data.map_changed.connect(_on_map_changed)
		# Initial map render
		call_deferred("refresh_map")
	
	# Center camera on map
	var map_center := Vector2(
		Constants.WORLD_W * Constants.TILE_SIZE / 2,
		Constants.WORLD_H * Constants.TILE_SIZE / 2
	)
	camera.position = map_center

func _setup_tileset() -> void:
	# Load the tiles texture
	var tiles_texture: Texture2D = load("res://assets/graphics/tiles.png")
	if not tiles_texture:
		push_error("Failed to load tiles.png")
		return
	
	# Create new TileSet
	var tileset := TileSet.new()
	tileset.tile_size = Vector2i(Constants.TILE_SIZE, Constants.TILE_SIZE)
	
	# Create atlas source
	var atlas := TileSetAtlasSource.new()
	atlas.texture = tiles_texture
	atlas.texture_region_size = Vector2i(Constants.TILE_SIZE, Constants.TILE_SIZE)
	
	# Add all tiles in the single column (960 tiles, each at column 0)
	for i: int in range(TOTAL_TILES):
		var coords := Vector2i(0, i)
		atlas.create_tile(coords)
	
	# Add atlas to tileset
	tileset.add_source(atlas, 0)
	
	# Apply to TileMap
	tile_map.tile_set = tileset

func _process(delta: float) -> void:
	_handle_camera_input(delta)

func _handle_camera_input(delta: float) -> void:
	# Keyboard panning
	var pan_dir := Vector2.ZERO
	if Input.is_action_pressed("pan_up"):
		pan_dir.y -= 1
	if Input.is_action_pressed("pan_down"):
		pan_dir.y += 1
	if Input.is_action_pressed("pan_left"):
		pan_dir.x -= 1
	if Input.is_action_pressed("pan_right"):
		pan_dir.x += 1
	
	if pan_dir != Vector2.ZERO:
		camera.position += pan_dir.normalized() * PAN_SPEED * delta / camera.zoom.x

func _unhandled_input(event: InputEvent) -> void:
	# Mouse wheel zoom
	if event.is_action_pressed("zoom_in"):
		_zoom(ZOOM_SPEED)
	elif event.is_action_pressed("zoom_out"):
		_zoom(-ZOOM_SPEED)
	
	# Middle mouse drag to pan
	if event is InputEventMouseButton:
		if event.button_index == MOUSE_BUTTON_MIDDLE:
			_dragging = event.pressed
			_drag_start = event.position
	
	if event is InputEventMouseMotion and _dragging:
		var motion_event := event as InputEventMouseMotion
		var delta: Vector2 = motion_event.position - _drag_start
		camera.position -= delta / camera.zoom.x
		_drag_start = event.position

func _zoom(amount: float) -> void:
	var new_zoom := clampf(camera.zoom.x + amount, MIN_ZOOM, MAX_ZOOM)
	camera.zoom = Vector2(new_zoom, new_zoom)

## Refresh entire map display
func refresh_map() -> void:
	if not Simulation.map_data:
		return
	
	for x in range(Constants.WORLD_W):
		for y in range(Constants.WORLD_H):
			_update_tile(x, y)

## Update a single tile display
func _update_tile(x: int, y: int) -> void:
	var tile_value: int = Simulation.map_data.get_tile(x, y)
	var tile_type: int = tile_value & Constants.LOMASK
	
	# Convert tile type to atlas coordinates
	var atlas_coords := _tile_to_atlas(tile_type)
	
	# Set the tile in the TileMap
	tile_map.set_cell(0, Vector2i(x, y), 0, atlas_coords)

## Convert tile type to atlas coordinates
func _tile_to_atlas(tile_type: int) -> Vector2i:
	# tiles.png is a single column, so tile N is at (0, N)
	# Clamp to valid range
	var clamped_tile := clampi(tile_type, 0, TOTAL_TILES - 1)
	return Vector2i(0, clamped_tile)

## Get world position from screen position
func screen_to_world(screen_pos: Vector2) -> Vector2:
	return camera.get_canvas_transform().affine_inverse() * screen_pos

## Get tile coordinates from screen position
func screen_to_tile(screen_pos: Vector2) -> Vector2i:
	var world_pos := screen_to_world(screen_pos)
	return Vector2i(
		int(world_pos.x / Constants.TILE_SIZE),
		int(world_pos.y / Constants.TILE_SIZE)
	)

## Signal handlers
func _on_tile_changed(x: int, y: int, _new_value: int) -> void:
	_update_tile(x, y)

func _on_map_changed() -> void:
	refresh_map()
