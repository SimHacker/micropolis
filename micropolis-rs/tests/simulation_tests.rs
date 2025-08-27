// It's necessary to import the crate as a library
use micropolis_rs::Micropolis;

#[test]
fn test_initial_state() {
    let micro = Micropolis::new();
    assert_eq!(micro.city_time, 0);
    assert_eq!(micro.total_funds, 20000);
    assert_eq!(micro.city_tax, 7);
    assert_eq!(micro.game_level, 0);
}
