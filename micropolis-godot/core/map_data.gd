## MapData - Core map storage class
## Holds tile data and overlay grids
class_name MapData
extends RefCounted

signal tile_changed(x: int, y: int, new_value: int)
signal map_changed()

# Full resolution map (120x100 tiles)
var tiles: PackedInt32Array
var power_grid: PackedByteArray  # Boolean: is tile powered?

# Half-size overlays (60x50) - one value per 2x2 block
var land_value: PackedByteArray
var pollution: PackedByteArray
var crime: PackedByteArray
var pop_density: PackedByteArray
var traffic_density: PackedByteArray

# Quarter-size overlays (30x25) - one value per 4x4 block
var terrain: PackedByteArray

# Eighth-size overlays (15x13) - one value per 8x8 block
var rate_of_growth: PackedByteArray
var fire_coverage: PackedByteArray
var police_coverage: PackedByteArray
var com_rate: PackedByteArray

func _init() -> void:
	_allocate_arrays()

func _allocate_arrays() -> void:
	var full_size := Constants.WORLD_W * Constants.WORLD_H
	var half_size := Constants.WORLD_W_2 * Constants.WORLD_H_2
	var quarter_size := Constants.WORLD_W_4 * Constants.WORLD_H_4
	var eighth_size := Constants.WORLD_W_8 * Constants.WORLD_H_8
	
	tiles.resize(full_size)
	power_grid.resize(full_size)
	
	land_value.resize(half_size)
	pollution.resize(half_size)
	crime.resize(half_size)
	pop_density.resize(half_size)
	traffic_density.resize(half_size)
	
	terrain.resize(quarter_size)
	
	rate_of_growth.resize(eighth_size)
	fire_coverage.resize(eighth_size)
	police_coverage.resize(eighth_size)
	com_rate.resize(eighth_size)
	
	# Initialize to dirt
	tiles.fill(Constants.DIRT)
	power_grid.fill(0)

## Clear map to all dirt
func clear() -> void:
	tiles.fill(Constants.DIRT)
	power_grid.fill(0)
	land_value.fill(0)
	pollution.fill(0)
	crime.fill(0)
	pop_density.fill(0)
	traffic_density.fill(0)
	terrain.fill(0)
	rate_of_growth.fill(0)
	fire_coverage.fill(0)
	police_coverage.fill(0)
	com_rate.fill(0)
	map_changed.emit()

# =============================================================================
# Tile Access (Full Resolution)
# =============================================================================

func _idx(x: int, y: int) -> int:
	return y * Constants.WORLD_W + x

func is_valid(x: int, y: int) -> bool:
	return x >= 0 and x < Constants.WORLD_W and y >= 0 and y < Constants.WORLD_H

func get_tile(x: int, y: int) -> int:
	if not is_valid(x, y):
		return -1
	return tiles[_idx(x, y)]

func get_tile_type(x: int, y: int) -> int:
	var tile := get_tile(x, y)
	if tile < 0:
		return -1
	return tile & Constants.LOMASK

func set_tile(x: int, y: int, value: int) -> void:
	if not is_valid(x, y):
		return
	var idx := _idx(x, y)
	if tiles[idx] != value:
		tiles[idx] = value
		tile_changed.emit(x, y, value)

func set_tile_type(x: int, y: int, tile_type: int, preserve_flags: bool = false) -> void:
	if not is_valid(x, y):
		return
	var idx := _idx(x, y)
	var old_value := tiles[idx]
	var new_value: int
	if preserve_flags:
		new_value = (old_value & Constants.ALLBITS) | (tile_type & Constants.LOMASK)
	else:
		new_value = tile_type & Constants.LOMASK
	if tiles[idx] != new_value:
		tiles[idx] = new_value
		tile_changed.emit(x, y, new_value)

func is_powered(x: int, y: int) -> bool:
	if not is_valid(x, y):
		return false
	return power_grid[_idx(x, y)] != 0

func set_powered(x: int, y: int, powered: bool) -> void:
	if not is_valid(x, y):
		return
	power_grid[_idx(x, y)] = 1 if powered else 0

# =============================================================================
# Half-Size Overlay Access (2x2 blocks)
# =============================================================================

func _idx2(x: int, y: int) -> int:
	return (y / 2) * Constants.WORLD_W_2 + (x / 2)

func get_land_value(x: int, y: int) -> int:
	if not is_valid(x, y):
		return 0
	return land_value[_idx2(x, y)]

func set_land_value(x: int, y: int, value: int) -> void:
	if not is_valid(x, y):
		return
	land_value[_idx2(x, y)] = clampi(value, 0, 255)

func get_pollution(x: int, y: int) -> int:
	if not is_valid(x, y):
		return 0
	return pollution[_idx2(x, y)]

func set_pollution(x: int, y: int, value: int) -> void:
	if not is_valid(x, y):
		return
	pollution[_idx2(x, y)] = clampi(value, 0, 255)

func get_crime(x: int, y: int) -> int:
	if not is_valid(x, y):
		return 0
	return crime[_idx2(x, y)]

func set_crime(x: int, y: int, value: int) -> void:
	if not is_valid(x, y):
		return
	crime[_idx2(x, y)] = clampi(value, 0, 255)

func get_pop_density(x: int, y: int) -> int:
	if not is_valid(x, y):
		return 0
	return pop_density[_idx2(x, y)]

func set_pop_density(x: int, y: int, value: int) -> void:
	if not is_valid(x, y):
		return
	pop_density[_idx2(x, y)] = clampi(value, 0, 255)

func get_traffic(x: int, y: int) -> int:
	if not is_valid(x, y):
		return 0
	return traffic_density[_idx2(x, y)]

func set_traffic(x: int, y: int, value: int) -> void:
	if not is_valid(x, y):
		return
	traffic_density[_idx2(x, y)] = clampi(value, 0, 255)

# =============================================================================
# Eighth-Size Overlay Access (8x8 blocks)
# =============================================================================

func _idx8(x: int, y: int) -> int:
	return (y / 8) * Constants.WORLD_W_8 + (x / 8)

func get_fire_coverage(x: int, y: int) -> int:
	if not is_valid(x, y):
		return 0
	return fire_coverage[_idx8(x, y)]

func get_police_coverage(x: int, y: int) -> int:
	if not is_valid(x, y):
		return 0
	return police_coverage[_idx8(x, y)]

func get_growth_rate(x: int, y: int) -> int:
	if not is_valid(x, y):
		return 0
	return rate_of_growth[_idx8(x, y)]
