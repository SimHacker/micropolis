## CityFileLoader - Load .cty files (classic SimCity format)
## Based on micropolis-java FILE_FORMAT.txt
class_name CityFileLoader
extends RefCounted

const HEADER_SIZE := 0x0C30  # 3120 bytes before map data
const MAP_DATA_SIZE := 24000  # 120 * 100 * 2 bytes

# History arrays (each 240 entries of 16-bit values = 480 bytes)
const HISTORY_RES_OFFSET := 0x0000
const HISTORY_COM_OFFSET := 0x01E0
const HISTORY_IND_OFFSET := 0x03C0
const HISTORY_CRIME_OFFSET := 0x05A0
const HISTORY_POLLUTION_OFFSET := 0x0780
const HISTORY_MONEY_OFFSET := 0x0960

# Miscellaneous values offset
const MISC_OFFSET := 0x0B40

# Map data starts here
const MAP_OFFSET := 0x0C30

## Load a .cty file into MapData
static func load_city(path: String, map_data: MapData) -> Dictionary:
	var result := {
		"success": false,
		"error": "",
		"city_time": 0,
		"total_funds": 20000,
		"tax_rate": 7,
		"res_pop": 0,
		"com_pop": 0,
		"ind_pop": 0
	}
	
	var file := FileAccess.open(path, FileAccess.READ)
	if file == null:
		result.error = "Could not open file: %s" % path
		return result
	
	var file_size := file.get_length()
	if file_size < MAP_OFFSET + MAP_DATA_SIZE:
		result.error = "File too small: %d bytes (expected at least %d)" % [file_size, MAP_OFFSET + MAP_DATA_SIZE]
		file.close()
		return result
	
	# Read miscellaneous values
	file.seek(MISC_OFFSET)
	var misc_data := file.get_buffer(240)  # 120 16-bit values
	
	if misc_data.size() >= 120:
		# Misc values are stored as 16-bit little-endian
		result.res_pop = _read_int16(misc_data, 4)
		result.com_pop = _read_int16(misc_data, 6)
		result.ind_pop = _read_int16(misc_data, 8)
		
		# City time is 32-bit at offset 16-19 (indices 8-9 as 16-bit pairs)
		result.city_time = _read_int16(misc_data, 16) | (_read_int16(misc_data, 18) << 16)
		
		# Total funds is 32-bit at offset 100-103 (indices 50-51)
		result.total_funds = _read_int16(misc_data, 100) | (_read_int16(misc_data, 102) << 16)
		
		# Tax rate at offset 112
		result.tax_rate = _read_int16(misc_data, 112)
	
	# Read map data
	file.seek(MAP_OFFSET)
	
	# Map is stored in COLUMN-MAJOR order (x varies slowly, y varies quickly)
	for x in range(Constants.WORLD_W):
		for y in range(Constants.WORLD_H):
			var lo := file.get_8()
			var hi := file.get_8()
			var tile_value := lo | (hi << 8)
			
			# Strip the synthesized bits and keep only tile type + some flags
			# The file stores CONDBIT, BURNBIT, BULLBIT, ANIMBIT, ZONEBIT
			# but we derive these from tile properties
			var tile_type := tile_value & Constants.LOMASK
			
			# Preserve power bit if set
			var flags := tile_value & Constants.PWRBIT
			
			map_data.set_tile(x, y, tile_type | flags)
	
	file.close()
	
	result.success = true
	return result

## Read 16-bit little-endian integer from buffer
static func _read_int16(buffer: PackedByteArray, offset: int) -> int:
	if offset + 1 >= buffer.size():
		return 0
	return buffer[offset] | (buffer[offset + 1] << 8)

## Save map to .cty file
static func save_city(path: String, map_data: MapData, game_state: Dictionary) -> Dictionary:
	var result := {
		"success": false,
		"error": ""
	}
	
	var file := FileAccess.open(path, FileAccess.WRITE)
	if file == null:
		result.error = "Could not create file: %s" % path
		return result
	
	# Write empty history (we don't track this yet)
	var empty_history := PackedByteArray()
	empty_history.resize(MISC_OFFSET)
	empty_history.fill(0)
	file.store_buffer(empty_history)
	
	# Write misc values
	var misc := PackedByteArray()
	misc.resize(240)
	misc.fill(0)
	
	# Population values
	_write_int16(misc, 4, game_state.get("res_pop", 0))
	_write_int16(misc, 6, game_state.get("com_pop", 0))
	_write_int16(misc, 8, game_state.get("ind_pop", 0))
	
	# City time (32-bit)
	var city_time: int = game_state.get("city_time", 0)
	_write_int16(misc, 16, city_time & 0xFFFF)
	_write_int16(misc, 18, (city_time >> 16) & 0xFFFF)
	
	# Total funds (32-bit)
	var funds: int = game_state.get("total_funds", 20000)
	_write_int16(misc, 100, funds & 0xFFFF)
	_write_int16(misc, 102, (funds >> 16) & 0xFFFF)
	
	# Tax rate
	_write_int16(misc, 112, game_state.get("tax_rate", 7))
	
	file.store_buffer(misc)
	
	# Write map data (column-major order)
	for x in range(Constants.WORLD_W):
		for y in range(Constants.WORLD_H):
			var tile_value := map_data.get_tile(x, y)
			file.store_8(tile_value & 0xFF)
			file.store_8((tile_value >> 8) & 0xFF)
	
	file.close()
	
	result.success = true
	return result

## Write 16-bit little-endian integer to buffer
static func _write_int16(buffer: PackedByteArray, offset: int, value: int) -> void:
	buffer[offset] = value & 0xFF
	buffer[offset + 1] = (value >> 8) & 0xFF
