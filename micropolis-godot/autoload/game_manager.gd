## Game Manager - Global game state singleton
extends Node

signal city_loaded(city_name: String)
signal funds_changed(amount: int)
signal date_changed(year: int, month: int)

# Current city data
var city_name: String = "Unnamed City"
var total_funds: int = 20000
var city_time: int = 0  # Weeks since start (Jan 1900)

# Settings
var auto_bulldoze: bool = true
var auto_budget: bool = false
var disasters_enabled: bool = true
var sound_enabled: bool = true

func _ready() -> void:
	print("GameManager initialized")

## Get current year (starts at 1900)
func get_year() -> int:
	return 1900 + (city_time / 48)

## Get current month (1-12)
func get_month() -> int:
	return ((city_time % 48) / 4) + 1

## Get formatted date string
func get_date_string() -> String:
	var months := ["Jan", "Feb", "Mar", "Apr", "May", "Jun", 
	               "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"]
	return "%s %d" % [months[get_month() - 1], get_year()]

## Spend funds, returns true if successful
func spend(amount: int) -> bool:
	if total_funds >= amount:
		total_funds -= amount
		funds_changed.emit(total_funds)
		return true
	return false

## Add funds
func add_funds(amount: int) -> void:
	total_funds += amount
	funds_changed.emit(total_funds)

## Advance time by one tick (called by simulation)
func advance_time() -> void:
	city_time += 1
	if city_time % 4 == 0:  # Monthly
		date_changed.emit(get_year(), get_month())
