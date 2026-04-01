## Micropolis Constants
## Ported from micropolis-java TileConstants.java
extends Node

# =============================================================================
# Map Dimensions
# =============================================================================

const WORLD_W: int = 120
const WORLD_H: int = 100
const WORLD_W_2: int = 60   # Half-size overlays (2x2 blocks)
const WORLD_H_2: int = 50
const WORLD_W_4: int = 30   # Quarter-size overlays (4x4 blocks)
const WORLD_H_4: int = 25
const WORLD_W_8: int = 15   # Eighth-size overlays (8x8 blocks)
const WORLD_H_8: int = 13

const TILE_SIZE: int = 16   # Pixels per tile

# =============================================================================
# Tile Flags (upper 6 bits of 16-bit tile value)
# =============================================================================

const PWRBIT: int   = 0x8000  # 32768 - Currently powered
const CONDBIT: int  = 0x4000  # 16384 - Conducts power
const BURNBIT: int  = 0x2000  # 8192  - Can burn
const BULLBIT: int  = 0x1000  # 4096  - Bulldozable
const ANIMBIT: int  = 0x0800  # 2048  - Animated tile
const ZONEBIT: int  = 0x0400  # 1024  - Zone center

const ALLBITS: int  = 0xFC00  # Mask for upper 6 bits
const LOMASK: int   = 0x03FF  # Mask for low 10 bits (tile type 0-1023)

# =============================================================================
# Tile Type Constants
# =============================================================================

const DIRT: int = 0
const RIVER: int = 2
const REDGE: int = 3
const CHANNEL: int = 4
const RIVEDGE: int = 8
const FIRSTRIVEDGE: int = 5
const LASTRIVEDGE: int = 20
const TREEBASE: int = 21
const WOODS_LOW: int = 21
const WOODS: int = 37
const WOODS_HIGH: int = 39
const WOODS2: int = 40
const WOODS5: int = 43
const RUBBLE: int = 44
const LASTRUBBLE: int = 47
const FLOOD: int = 48
const LASTFLOOD: int = 51
const RADTILE: int = 52
const FIRE: int = 56
const ROADBASE: int = 64
const HBRIDGE: int = 64
const VBRIDGE: int = 65
const ROADS: int = 66
const ROADS2: int = 67
const ROADS3: int = 68
const ROADS4: int = 69
const ROADS5: int = 70
const ROADS6: int = 71
const ROADS7: int = 72
const ROADS8: int = 73
const ROADS9: int = 74
const ROADS10: int = 75
const INTERSECTION: int = 76
const HROADPOWER: int = 77
const VROADPOWER: int = 78
const BRWH: int = 79
const LTRFBASE: int = 80
const BRWV: int = 95
const HTRFBASE: int = 144
const LASTROAD: int = 206
const POWERBASE: int = 208
const HPOWER: int = 208
const VPOWER: int = 209
const LHPOWER: int = 210
const LVPOWER: int = 211
const LVPOWER2: int = 212
const LVPOWER3: int = 213
const LVPOWER4: int = 214
const LVPOWER5: int = 215
const LVPOWER6: int = 216
const LVPOWER7: int = 217
const LVPOWER8: int = 218
const LVPOWER9: int = 219
const LVPOWER10: int = 220
const RAILHPOWERV: int = 221
const RAILVPOWERH: int = 222
const LASTPOWER: int = 222
const RAILBASE: int = 224
const HRAIL: int = 224
const VRAIL: int = 225
const LHRAIL: int = 226
const LVRAIL: int = 227
const LVRAIL2: int = 228
const LVRAIL3: int = 229
const LVRAIL4: int = 230
const LVRAIL5: int = 231
const LVRAIL6: int = 232
const LVRAIL7: int = 233
const LVRAIL8: int = 234
const LVRAIL9: int = 235
const LVRAIL10: int = 236
const HRAILROAD: int = 237
const VRAILROAD: int = 238
const LASTRAIL: int = 238
const RESBASE: int = 244
const RESCLR: int = 244
const HOUSE: int = 249
const LHTHR: int = 249
const HHTHR: int = 260
const RZB: int = 265
const HOSPITAL: int = 409
const CHURCH: int = 418
const COMBASE: int = 423
const COMCLR: int = 427
const CZB: int = 436
const INDBASE: int = 612
const INDCLR: int = 616
const IZB: int = 625
const PORTBASE: int = 693
const PORT: int = 698
const LASTPORT: int = 708
const AIRPORTBASE: int = 709
const RADAR: int = 711
const AIRPORT: int = 716
const COALBASE: int = 745
const POWERPLANT: int = 750
const LASTPOWERPLANT: int = 760
const FIRESTBASE: int = 761
const FIRESTATION: int = 765
const POLICESTBASE: int = 770
const POLICESTATION: int = 774
const STADIUMBASE: int = 779
const STADIUM: int = 784
const FULLSTADIUM: int = 800
const NUCLEARBASE: int = 811
const NUCLEAR: int = 816
const LASTZONE: int = 826
const LIGHTNINGBOLT: int = 827
const HBRDG0: int = 828
const HBRDG1: int = 829
const HBRDG2: int = 830
const HBRDG3: int = 831
const FOUNTAIN: int = 840
const TINYEXP: int = 860
const LASTTINYEXP: int = 867
const FOOTBALLGAME1: int = 932
const FOOTBALLGAME2: int = 940
const VBRDG0: int = 948
const VBRDG1: int = 949
const VBRDG2: int = 950
const VBRDG3: int = 951

# Special tiles
const TELEBASE: int = 948  # Network/telecom tower
const CHURCH1BASE: int = 418
const CHURCH7LAST: int = 422

# =============================================================================
# Zone Types
# =============================================================================

enum ZoneType {
	RESIDENTIAL,
	COMMERCIAL,
	INDUSTRIAL,
	NONE
}

# =============================================================================
# Tool Types
# =============================================================================

enum ToolType {
	NONE,
	BULLDOZER,
	WIRE,
	ROAD,
	RAIL,
	RESIDENTIAL,
	COMMERCIAL,
	INDUSTRIAL,
	FIRE_STATION,
	POLICE_STATION,
	STADIUM,
	PARK,
	SEAPORT,
	COAL_POWER,
	NUCLEAR_POWER,
	AIRPORT,
	QUERY
}

# Tool costs
const TOOL_COST: Dictionary = {
	ToolType.BULLDOZER: 1,
	ToolType.WIRE: 5,
	ToolType.ROAD: 10,
	ToolType.RAIL: 20,
	ToolType.RESIDENTIAL: 100,
	ToolType.COMMERCIAL: 100,
	ToolType.INDUSTRIAL: 100,
	ToolType.FIRE_STATION: 500,
	ToolType.POLICE_STATION: 500,
	ToolType.STADIUM: 5000,
	ToolType.PARK: 10,
	ToolType.SEAPORT: 3000,
	ToolType.COAL_POWER: 3000,
	ToolType.NUCLEAR_POWER: 5000,
	ToolType.AIRPORT: 10000,
	ToolType.QUERY: 0
}

# Tool sizes (width x height in tiles)
const TOOL_SIZE: Dictionary = {
	ToolType.BULLDOZER: Vector2i(1, 1),
	ToolType.WIRE: Vector2i(1, 1),
	ToolType.ROAD: Vector2i(1, 1),
	ToolType.RAIL: Vector2i(1, 1),
	ToolType.RESIDENTIAL: Vector2i(3, 3),
	ToolType.COMMERCIAL: Vector2i(3, 3),
	ToolType.INDUSTRIAL: Vector2i(3, 3),
	ToolType.FIRE_STATION: Vector2i(3, 3),
	ToolType.POLICE_STATION: Vector2i(3, 3),
	ToolType.STADIUM: Vector2i(4, 4),
	ToolType.PARK: Vector2i(1, 1),
	ToolType.SEAPORT: Vector2i(4, 4),
	ToolType.COAL_POWER: Vector2i(4, 4),
	ToolType.NUCLEAR_POWER: Vector2i(4, 4),
	ToolType.AIRPORT: Vector2i(6, 6),
	ToolType.QUERY: Vector2i(1, 1)
}

# =============================================================================
# Simulation Speed
# =============================================================================

enum SimSpeed {
	PAUSED,
	SLOW,
	NORMAL,
	FAST,
	SUPER_FAST
}

const SPEED_DELAYS: Dictionary = {
	SimSpeed.PAUSED: 0,
	SimSpeed.SLOW: 1,
	SimSpeed.NORMAL: 2,
	SimSpeed.FAST: 4,
	SimSpeed.SUPER_FAST: 8
}

# =============================================================================
# Disasters
# =============================================================================

enum DisasterType {
	NONE,
	FIRE,
	FLOOD,
	TORNADO,
	EARTHQUAKE,
	MONSTER,
	MELTDOWN
}

# =============================================================================
# Map Overlay Types
# =============================================================================

enum MapOverlay {
	NONE,
	RESIDENTIAL,
	COMMERCIAL,
	INDUSTRIAL,
	TRANSPORT,
	POPULATION,
	GROWTH_RATE,
	POLLUTION,
	CRIME,
	LAND_VALUE,
	FIRE_COVERAGE,
	POLICE_COVERAGE,
	POWER_GRID
}

# =============================================================================
# Utility Functions
# =============================================================================

## Get tile type from full tile value (strips flags)
static func get_tile_type(tile_value: int) -> int:
	return tile_value & LOMASK

## Check if tile has specific flag
static func has_flag(tile_value: int, flag: int) -> bool:
	return (tile_value & flag) != 0

## Check if tile is powered
static func is_powered(tile_value: int) -> bool:
	return has_flag(tile_value, PWRBIT)

## Check if tile conducts power
static func conducts_power(tile_value: int) -> bool:
	return has_flag(tile_value, CONDBIT)

## Check if tile is a zone center
static func is_zone_center(tile_value: int) -> bool:
	return has_flag(tile_value, ZONEBIT)

## Check if tile is water
static func is_water(tile_type: int) -> bool:
	return tile_type >= RIVER and tile_type <= LASTRIVEDGE

## Check if tile is a road
static func is_road(tile_type: int) -> bool:
	return tile_type >= ROADBASE and tile_type <= LASTROAD

## Check if tile is a power line
static func is_power_line(tile_type: int) -> bool:
	return tile_type >= POWERBASE and tile_type <= LASTPOWER

## Check if tile is a rail
static func is_rail(tile_type: int) -> bool:
	return tile_type >= RAILBASE and tile_type <= LASTRAIL

## Check if tile is tree/woods
static func is_tree(tile_type: int) -> bool:
	return tile_type >= WOODS_LOW and tile_type <= WOODS5

## Check if tile is rubble
static func is_rubble(tile_type: int) -> bool:
	return tile_type >= RUBBLE and tile_type <= LASTRUBBLE

## Check if tile is on fire
static func is_fire(tile_type: int) -> bool:
	return tile_type >= FIRE and tile_type < FIRE + 8

## Get zone type from tile
static func get_zone_type(tile_type: int) -> ZoneType:
	if tile_type >= RESBASE and tile_type < COMBASE:
		return ZoneType.RESIDENTIAL
	elif tile_type >= COMBASE and tile_type < INDBASE:
		return ZoneType.COMMERCIAL
	elif tile_type >= INDBASE and tile_type < PORTBASE:
		return ZoneType.INDUSTRIAL
	return ZoneType.NONE
