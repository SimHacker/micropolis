// It's necessary to import the crate as a library
use micropolis_engine::Micropolis;

#[test]
fn test_initial_state() {
    let micro = Micropolis::new();
    assert_eq!(micro.city_time, 0);
    assert_eq!(micro.total_funds, 20000);
    assert_eq!(micro.city_tax, 7);
    assert_eq!(micro.game_level, 0);
}

#[test]
fn test_step_simulation() {
    let mut micro = Micropolis::new();
    micro.sim_speed = 1;
    for _ in 0..80 {
        micro.step_simulation();
    }
    assert_eq!(micro.city_time, 1);
}
