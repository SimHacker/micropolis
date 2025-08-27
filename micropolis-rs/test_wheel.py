import micropolis_rs

def main():
    print("Testing micropolis_rs Python wheel...")

    # Create a new Micropolis instance
    print("Creating Micropolis instance...")
    micro = micropolis_rs.Micropolis()
    print("Instance created.")

    # Get initial city stats
    print("Getting initial city stats...")
    stats = micro.get_city_stats()
    print(f"Initial city time: {stats.city_time}")
    print(f"Initial total funds: {stats.total_funds}")
    print(f"Initial total population: {stats.total_pop}")
    assert stats.city_time == 0
    assert stats.total_funds == 20000

    # Step the simulation
    print("Stepping simulation...")
    micro.step_simulation()
    print("Simulation stepped.")

    # Get map view
    print("Getting map view...")
    map_view = micro.get_map_view()
    print(f"Map view length: {len(map_view)}")
    print(f"Expected length: {120 * 100}")
    assert len(map_view) == 120 * 100

    print("All tests passed!")

def test_save_load():
    print("\nTesting save/load functionality...")

    # Create a new Micropolis instance
    micro = micropolis_rs.Micropolis()
    micro.city_time = 123
    micro.total_funds = 456

    # Save the city
    file_path = "city.dat"
    print(f"Saving city to {file_path}...")
    micro.save_city(file_path)
    print("City saved.")

    # Load the city
    print(f"Loading city from {file_path}...")
    loaded_micro = micropolis_rs.Micropolis.load_city(file_path)
    print("City loaded.")

    # Compare stats
    print("Comparing stats...")
    assert loaded_micro.city_time == micro.city_time
    assert loaded_micro.total_funds == micro.total_funds
    print("Stats match.")

    print("Save/load test passed!")

if __name__ == "__main__":
    main()
    test_save_load()
