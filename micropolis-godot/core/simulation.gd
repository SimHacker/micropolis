## Simulation Engine - Main simulation loop
## Ported from micropolis-java Micropolis.java
extends Node

signal city_message(message: String, location: Vector2i)
signal census_changed()
signal demand_changed()
signal evaluation_changed()
signal power_grid_changed()

# Map data
var map_data: MapData

# Simulation state
var running: bool = false
var speed: Constants.SimSpeed = Constants.SimSpeed.NORMAL
var cycle_count: int = 0
var phase: int = 0  # 0-15 phases per cycle

# Census counters (reset each cycle)
var res_pop: int = 0
var com_pop: int = 0
var ind_pop: int = 0
var res_zone_count: int = 0
var com_zone_count: int = 0
var ind_zone_count: int = 0
var road_total: int = 0
var rail_total: int = 0
var fire_station_count: int = 0
var police_station_count: int = 0
var hospital_count: int = 0
var powered_zone_count: int = 0
var unpowered_zone_count: int = 0

# Demand valves (-2000 to 2000)
var res_valve: int = 0
var com_valve: int = 0
var ind_valve: int = 0

# Averages
var land_value_avg: int = 0
var crime_avg: int = 0
var pollution_avg: int = 0

# City center (weighted by population)
var city_center: Vector2i = Vector2i(60, 50)

# Accumulated time for fixed timestep
var _time_accumulator: float = 0.0
const TICK_RATE: float = 1.0 / 30.0  # 30 simulation ticks per second at normal speed

func _ready() -> void:
	map_data = MapData.new()
	print("Simulation engine initialized")

func _process(delta: float) -> void:
	if not running or speed == Constants.SimSpeed.PAUSED:
		return
	
	_time_accumulator += delta
	
	# How many ticks to run based on speed
	var ticks_per_frame: int = Constants.SPEED_DELAYS.get(speed, 1)
	var tick_delta: float = TICK_RATE / float(ticks_per_frame)
	
	while _time_accumulator >= tick_delta:
		_time_accumulator -= tick_delta
		_simulation_tick()

func _simulation_tick() -> void:
	# The simulation runs in 16 phases per cycle
	phase = cycle_count % 16
	
	match phase:
		0:
			_phase_init()
		1, 2, 3, 4, 5, 6, 7, 8:
			_phase_map_scan(phase - 1)
		9:
			_phase_census()
		10:
			_phase_decay()
		11:
			_phase_power_scan()
		12:
			_phase_pollution_scan()
		13:
			_phase_crime_scan()
		14:
			_phase_pop_density_scan()
		15:
			_phase_fire_analysis()
	
	cycle_count += 1

## Phase 0: Initialize cycle, update time
func _phase_init() -> void:
	# Every 16 phases = 1 city time tick
	if cycle_count > 0 and cycle_count % 16 == 0:
		GameManager.advance_time()
	
	# Clear census counters
	res_pop = 0
	com_pop = 0
	ind_pop = 0
	res_zone_count = 0
	com_zone_count = 0
	ind_zone_count = 0
	road_total = 0
	rail_total = 0
	fire_station_count = 0
	police_station_count = 0
	hospital_count = 0
	powered_zone_count = 0
	unpowered_zone_count = 0

## Phases 1-8: Scan map in 8 horizontal bands
func _phase_map_scan(band: int) -> void:
	var start_x := band * 15  # 120 / 8 = 15 columns per band
	var end_x := mini(start_x + 15, Constants.WORLD_W)
	
	for x in range(start_x, end_x):
		for y in range(Constants.WORLD_H):
			_process_tile(x, y)

## Process a single tile during map scan
func _process_tile(x: int, y: int) -> void:
	var tile := map_data.get_tile(x, y)
	var tile_type := tile & Constants.LOMASK
	
	# Count infrastructure
	if Constants.is_road(tile_type):
		road_total += 1
	elif Constants.is_rail(tile_type):
		rail_total += 1
	
	# Process zone centers
	if tile & Constants.ZONEBIT:
		_process_zone(x, y, tile_type)

## Process a zone center tile
func _process_zone(x: int, y: int, tile_type: int) -> void:
	var is_powered := map_data.is_powered(x, y)
	
	if is_powered:
		powered_zone_count += 1
	else:
		unpowered_zone_count += 1
	
	# Identify zone type and count population
	var zone_type := Constants.get_zone_type(tile_type)
	
	match zone_type:
		Constants.ZoneType.RESIDENTIAL:
			res_zone_count += 1
			res_pop += _get_zone_population(tile_type)
		Constants.ZoneType.COMMERCIAL:
			com_zone_count += 1
			com_pop += _get_zone_population(tile_type)
		Constants.ZoneType.INDUSTRIAL:
			ind_zone_count += 1
			ind_pop += _get_zone_population(tile_type)

## Get population value from zone tile
func _get_zone_population(tile_type: int) -> int:
	# Simplified - actual logic depends on zone level
	var zone_type := Constants.get_zone_type(tile_type)
	match zone_type:
		Constants.ZoneType.RESIDENTIAL:
			if tile_type == Constants.RESCLR:
				return 0
			elif tile_type >= Constants.HOUSE and tile_type < Constants.HOUSE + 9:
				return (tile_type - Constants.HOUSE) + 1
			else:
				return 16  # Developed zone
		Constants.ZoneType.COMMERCIAL:
			if tile_type == Constants.COMCLR:
				return 0
			else:
				return 8
		Constants.ZoneType.INDUSTRIAL:
			if tile_type == Constants.INDCLR:
				return 0
			else:
				return 8
	return 0

## Phase 9: Collect census data
func _phase_census() -> void:
	# Update demand valves based on population
	_update_demand_valves()
	census_changed.emit()

## Update RCI demand valves
func _update_demand_valves() -> void:
	# Simplified demand calculation
	var total_pop := res_pop + com_pop + ind_pop
	
	# Residential demand based on jobs available
	var employment := com_pop + ind_pop
	if total_pop > 0:
		res_valve = clampi((employment * 8) - res_pop, -2000, 2000)
	else:
		res_valve = 500  # Starting demand
	
	# Commercial demand based on population
	if res_pop > 0:
		com_valve = clampi((res_pop / 8) - com_pop, -1500, 1500)
	else:
		com_valve = 0
	
	# Industrial demand (external market simulation)
	ind_valve = clampi(500 - ind_pop, -1500, 1500)
	
	demand_changed.emit()

## Phase 10: Decay traffic density, etc.
func _phase_decay() -> void:
	# Decay traffic density over time
	for i in range(map_data.traffic_density.size()):
		if map_data.traffic_density[i] > 0:
			map_data.traffic_density[i] = maxi(0, map_data.traffic_density[i] - 2)

## Phase 11: Power grid propagation
func _phase_power_scan() -> void:
	# Clear power grid
	map_data.power_grid.fill(0)
	
	# Find power plants and propagate power
	for x in range(Constants.WORLD_W):
		for y in range(Constants.WORLD_H):
			var tile := map_data.get_tile(x, y)
			var tile_type := tile & Constants.LOMASK
			
			# Check if this is a power plant
			if tile_type == Constants.POWERPLANT or tile_type == Constants.NUCLEAR:
				_propagate_power(x, y)
	
	power_grid_changed.emit()

## Flood-fill power from a power source
func _propagate_power(start_x: int, start_y: int) -> void:
	var queue: Array[Vector2i] = [Vector2i(start_x, start_y)]
	var visited: Dictionary = {}
	
	while queue.size() > 0:
		var pos: Vector2i = queue.pop_front()
		var key: int = pos.x * 1000 + pos.y
		
		if visited.has(key):
			continue
		visited[key] = true
		
		var tile := map_data.get_tile(pos.x, pos.y)
		if tile < 0:
			continue
		
		var tile_type := tile & Constants.LOMASK
		
		# Check if tile conducts power
		var conducts := Constants.is_power_line(tile_type) or \
		                Constants.is_road(tile_type) or \
		                Constants.is_rail(tile_type) or \
		                (tile & Constants.ZONEBIT) != 0
		
		if not conducts and tile_type != Constants.POWERPLANT and tile_type != Constants.NUCLEAR:
			continue
		
		# Power this tile
		map_data.set_powered(pos.x, pos.y, true)
		
		# Add neighbors to queue
		for offset: Vector2i in [Vector2i(-1, 0), Vector2i(1, 0), Vector2i(0, -1), Vector2i(0, 1)]:
			var next: Vector2i = pos + offset
			if map_data.is_valid(next.x, next.y):
				queue.append(next)

## Phase 12: Calculate pollution
func _phase_pollution_scan() -> void:
	# Simplified pollution calculation
	var total_pollution := 0
	var count := 0
	
	for x in range(0, Constants.WORLD_W, 2):
		for y in range(0, Constants.WORLD_H, 2):
			var pollution_val := 0
			
			# Industrial zones cause pollution
			var tile_type := map_data.get_tile_type(x, y)
			if tile_type >= Constants.INDBASE and tile_type < Constants.PORTBASE:
				pollution_val += 20
			
			# Traffic causes pollution
			pollution_val += map_data.get_traffic(x, y) / 4
			
			map_data.set_pollution(x, y, pollution_val)
			total_pollution += pollution_val
			count += 1
	
	if count > 0:
		pollution_avg = total_pollution / count

## Phase 13: Calculate crime
func _phase_crime_scan() -> void:
	# Simplified crime calculation
	var total_crime := 0
	var count := 0
	
	for x in range(0, Constants.WORLD_W, 2):
		for y in range(0, Constants.WORLD_H, 2):
			var crime_val := 0
			
			# Base crime from population density
			crime_val = map_data.get_pop_density(x, y) / 2
			
			# Reduce by land value
			crime_val -= map_data.get_land_value(x, y) / 4
			
			# Reduce by police coverage
			crime_val -= map_data.get_police_coverage(x, y)
			
			crime_val = maxi(0, crime_val)
			map_data.set_crime(x, y, crime_val)
			total_crime += crime_val
			count += 1
	
	if count > 0:
		crime_avg = total_crime / count

## Phase 14: Calculate population density
func _phase_pop_density_scan() -> void:
	# Calculate population density from zones
	for x in range(0, Constants.WORLD_W, 2):
		for y in range(0, Constants.WORLD_H, 2):
			var density := 0
			
			# Sample 2x2 area
			for dx in range(2):
				for dy in range(2):
					var tile := map_data.get_tile(x + dx, y + dy)
					if tile > 0 and (tile & Constants.ZONEBIT):
						var tile_type := tile & Constants.LOMASK
						density += _get_zone_population(tile_type)
			
			map_data.set_pop_density(x, y, mini(density * 4, 255))

## Phase 15: Fire station and disaster analysis
func _phase_fire_analysis() -> void:
	# Update fire coverage from fire stations
	map_data.fire_coverage.fill(0)
	
	for x in range(Constants.WORLD_W):
		for y in range(Constants.WORLD_H):
			var tile_type := map_data.get_tile_type(x, y)
			if tile_type == Constants.FIRESTATION:
				_add_fire_coverage(x, y, 100)
				fire_station_count += 1

func _add_fire_coverage(center_x: int, center_y: int, amount: int) -> void:
	# Add coverage in a radius around fire station
	for dx in range(-8, 9):
		for dy in range(-8, 9):
			var x := center_x + dx
			var y := center_y + dy
			if not map_data.is_valid(x, y):
				continue
			var dist := absi(dx) + absi(dy)
			if dist <= 8:
				var coverage := amount - (dist * 10)
				if coverage > 0:
					var idx := (y / 8) * Constants.WORLD_W_8 + (x / 8)
					if idx < map_data.fire_coverage.size():
						map_data.fire_coverage[idx] = mini(255, map_data.fire_coverage[idx] + coverage)

# =============================================================================
# Public API
# =============================================================================

## Start simulation
func start() -> void:
	running = true

## Pause simulation
func pause() -> void:
	running = false

## Toggle pause
func toggle_pause() -> void:
	running = not running

## Set simulation speed
func set_speed(new_speed: Constants.SimSpeed) -> void:
	speed = new_speed

## Load a city file
func load_city(path: String) -> bool:
	var result := CityFileLoader.load_city(path, map_data)
	if result.success:
		GameManager.city_time = result.city_time
		GameManager.total_funds = result.total_funds
		map_data.map_changed.emit()
		print("City loaded: %s" % path)
		return true
	else:
		push_error("Failed to load city: %s" % result.error)
		return false

## Generate a new empty city
func new_city() -> void:
	map_data.clear()
	GameManager.city_time = 0
	GameManager.total_funds = 20000
	cycle_count = 0
	phase = 0
	res_valve = 0
	com_valve = 0
	ind_valve = 0
