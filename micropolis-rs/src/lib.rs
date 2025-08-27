use pyo3::prelude::*;
use serde::{Serialize, Deserialize};

//
// Micropolis - a city simulation engine in Rust
//

mod sim;
mod zone;

// Constants from sim.h
pub const PWRBIT: u16 = 32768;
pub const CONDBIT: u16 = 16384;
pub const BURNBIT: u16 = 8192;
pub const BULLBIT: u16 = 4096;
pub const ANIMBIT: u16 = 2048;
pub const ZONEBIT: u16 = 1024;
pub const ALLBITS: u16 = 64512;
pub const LOMASK: u16 = 1023;

const WORLD_X: usize = 120;
const WORLD_Y: usize = 100;
const HWLDX: usize = WORLD_X / 2;
const HWLDY: usize = WORLD_Y / 2;
const QWX: usize = WORLD_X / 4;
const QWY: usize = WORLD_Y / 4;
const SMX: usize = WORLD_X / 8;
const SMY: usize = (WORLD_Y + 7) / 8;

const PWRMAPROW: usize = (WORLD_X + 15) / 16;
const PWRMAPSIZE: usize = PWRMAPROW * WORLD_Y;

// Tile definitions
pub const DIRT: u16 = 0;
pub const RIVER: u16 = 2;
pub const REDGE: u16 = 3;
pub const CHANNEL: u16 = 4;
pub const FIRSTRIVEDGE: u16 = 5;
pub const LASTRIVEDGE: u16 = 20;
pub const TREEBASE: u16 = 21;
pub const LASTTREE: u16 = 36;
pub const WOODS: u16 = 37;
pub const UNUSED_TRASH1: u16 = 38;
pub const UNUSED_TRASH2: u16 = 39;
pub const WOODS2: u16 = 40;
pub const WOODS3: u16 = 41;
pub const WOODS4: u16 = 42;
pub const WOODS5: u16 = 43;
pub const RUBBLE: u16 = 44;
pub const LASTRUBBLE: u16 = 47;
pub const FLOOD: u16 = 48;
pub const LASTFLOOD: u16 = 51;
pub const RADTILE: u16 = 52;
pub const UNUSED_TRASH3: u16 = 53;
pub const UNUSED_TRASH4: u16 = 54;
pub const UNUSED_TRASH5: u16 = 55;
pub const FIRE: u16 = 56;
pub const FIREBASE: u16 = 56;
pub const LASTFIRE: u16 = 63;
pub const ROADBASE: u16 = 64;
pub const HBRIDGE: u16 = 64;
pub const VBRIDGE: u16 = 65;
pub const ROADS: u16 = 66;
pub const INTERSECTION: u16 = 76;
pub const HROADPOWER: u16 = 77;
pub const VROADPOWER: u16 = 78;
pub const BRWH: u16 = 79;
pub const LTRFBASE: u16 = 80;
pub const BRWV: u16 = 95;
pub const BRWXXX1: u16 = 111;
pub const BRWXXX2: u16 = 127;
pub const BRWXXX3: u16 = 143;
pub const HTRFBASE: u16 = 144;
pub const BRWXXX4: u16 = 159;
pub const BRWXXX5: u16 = 175;
pub const BRWXXX6: u16 = 191;
pub const LASTROAD: u16 = 206;
pub const BRWXXX7: u16 = 207;
pub const POWERBASE: u16 = 208;
pub const HPOWER: u16 = 208;
pub const VPOWER: u16 = 209;
pub const LHPOWER: u16 = 210;
pub const LVPOWER: u16 = 211;
pub const RAILHPOWERV: u16 = 221;
pub const RAILVPOWERH: u16 = 222;
pub const LASTPOWER: u16 = 222;
pub const UNUSED_TRASH6: u16 = 223;
pub const RAILBASE: u16 = 224;
pub const HRAIL: u16 = 224;
pub const VRAIL: u16 = 225;
pub const LHRAIL: u16 = 226;
pub const LVRAIL: u16 = 227;
pub const HRAILROAD: u16 = 237;
pub const VRAILROAD: u16 = 238;
pub const LASTRAIL: u16 = 238;
pub const ROADVPOWERH: u16 = 239; /* bogus? */
pub const RESBASE: u16 = 240;
pub const FREEZ: u16 = 244;
pub const HOUSE: u16 = 249;
pub const LHTHR: u16 = 249;
pub const HHTHR: u16 = 260;
pub const RZB: u16 = 265;
pub const HOSPITAL: u16 = 409;
pub const CHURCH: u16 = 418;
pub const COMBASE: u16 = 423;
pub const COMCLR: u16 = 427;
pub const CZB: u16 = 436;
pub const INDBASE: u16 = 612;
pub const INDCLR: u16 = 616;
pub const LASTIND: u16 = 620;
pub const IND1: u16 = 621;
pub const IZB: u16 = 625;
pub const IND2: u16 = 641;
pub const IND3: u16 = 644;
pub const IND4: u16 = 649;
pub const IND5: u16 = 650;
pub const IND6: u16 = 676;
pub const IND7: u16 = 677;
pub const IND8: u16 = 686;
pub const IND9: u16 = 689;
pub const PORTBASE: u16 = 693;
pub const PORT: u16 = 698;
pub const LASTPORT: u16 = 708;
pub const AIRPORTBASE: u16 = 709;
pub const RADAR: u16 = 711;
pub const AIRPORT: u16 = 716;
pub const COALBASE: u16 = 745;
pub const POWERPLANT: u16 = 750;
pub const LASTPOWERPLANT: u16 = 760;
pub const FIRESTBASE: u16 = 761;
pub const FIRESTATION: u16 = 765;
pub const POLICESTBASE: u16 = 770;
pub const POLICESTATION: u16 = 774;
pub const STADIUMBASE: u16 = 779;
pub const STADIUM: u16 = 784;
pub const FULLSTADIUM: u16 = 800;
pub const NUCLEARBASE: u16 = 811;
pub const NUCLEAR: u16 = 816;
pub const LASTZONE: u16 = 826;
pub const LIGHTNINGBOLT: u16 = 827;
pub const HBRDG0: u16 = 828;
pub const HBRDG1: u16 = 829;
pub const HBRDG2: u16 = 830;
pub const HBRDG3: u16 = 831;
pub const RADAR0: u16 = 832;
pub const RADAR1: u16 = 833;
pub const RADAR2: u16 = 834;
pub const RADAR3: u16 = 835;
pub const RADAR4: u16 = 836;
pub const RADAR5: u16 = 837;
pub const RADAR6: u16 = 838;
pub const RADAR7: u16 = 839;
pub const FOUNTAIN: u16 = 840;
pub const INDBASE2: u16 = 844;
pub const TELEBASE: u16 = 844;
pub const TELELAST: u16 = 851;
pub const SMOKEBASE: u16 = 852;
pub const TINYEXP: u16 = 860;
pub const SOMETINYEXP: u16 = 864;
pub const LASTTINYEXP: u16 = 867;
pub const COALSMOKE1: u16 = 916;
pub const COALSMOKE2: u16 = 920;
pub const COALSMOKE3: u16 = 924;
pub const COALSMOKE4: u16 = 928;
pub const FOOTBALLGAME1: u16 = 932;
pub const FOOTBALLGAME2: u16 = 940;
pub const VBRDG0: u16 = 948;
pub const VBRDG1: u16 = 949;
pub const VBRDG2: u16 = 950;
pub const VBRDG3: u16 = 951;

pub const TILE_COUNT: u16 = 960;

#[pyclass]
#[derive(Clone, Serialize, Deserialize)]
pub struct CityStats {
    #[pyo3(get)]
    pub city_time: i64,
    #[pyo3(get)]
    pub total_funds: i64,
    #[pyo3(get)]
    pub total_pop: i32,
    #[pyo3(get)]
    pub res_pop: i32,
    #[pyo3(get)]
    pub com_pop: i32,
    #[pyo3(get)]
    pub ind_pop: i32,
}

#[pyclass]
#[derive(Clone, Serialize, Deserialize)]
pub struct Micropolis {
    // The main map
    map: Vec<Vec<u16>>,

    // 2x2 averaged maps
    pop_density: Vec<Vec<u8>>,
    trf_density: Vec<Vec<u8>>,
    pollution_mem: Vec<Vec<u8>>,
    land_value_mem: Vec<Vec<u8>>,
    crime_mem: Vec<Vec<u8>>,

    // 4x4 averaged maps
    terrain_mem: Vec<Vec<u8>>,

    // 8x8 averaged maps
    rate_og_mem: Vec<Vec<i16>>,
    fire_st_map: Vec<Vec<i16>>,
    police_map: Vec<Vec<i16>>,

    // Power map
    power_map: Vec<i16>,

    // Simulation state variables
    s_map_x: i16,
    s_map_y: i16,
    c_chr: u16,
    c_chr9: u16,
    new_power: bool,
    pwrd_z_cnt: i32,
    un_pwrd_z_cnt: i32,
    res_z_pop: i32,
    com_z_pop: i32,
    ind_z_pop: i32,
    fire_pop: i32,

    #[pyo3(get, set)]
    pub city_time: i64,
    #[pyo3(get, set)]
    pub total_funds: i64,
    #[pyo3(get, set)]
    pub city_tax: i16,
    pub game_level: i16,
    pub sim_speed: i16,
    spd_cycle: i16,
    f_cycle: i16,
    s_cycle: i16,
    do_initial_eval: bool,
    av_city_tax: i16,

    res_pop: i32,
    com_pop: i32,
    ind_pop: i32,
    total_pop: i32,
    last_total_pop: i32,
    res_valve: i16,
    com_valve: i16,
    ind_valve: i16,
    res_cap: bool,
    com_cap: bool,
    ind_cap: bool,

    // History arrays
    res_his: Vec<i16>,
    com_his: Vec<i16>,
    ind_his: Vec<i16>,
    money_his: Vec<i16>,
    crime_his: Vec<i16>,
    pollution_his: Vec<i16>,
    misc_his: Vec<i16>,
}

#[pymethods]
impl Micropolis {
    #[new]
    pub fn new() -> Self {
        Self {
            map: vec![vec![DIRT; WORLD_Y]; WORLD_X],
            pop_density: vec![vec![0; HWLDY]; HWLDX],
            trf_density: vec![vec![0; HWLDY]; HWLDX],
            pollution_mem: vec![vec![0; HWLDY]; HWLDX],
            land_value_mem: vec![vec![0; HWLDY]; HWLDX],
            crime_mem: vec![vec![0; HWLDY]; HWLDX],
            terrain_mem: vec![vec![0; QWY]; QWX],
            rate_og_mem: vec![vec![0; SMY]; SMX],
            fire_st_map: vec![vec![0; SMY]; SMX],
            police_map: vec![vec![0; SMY]; SMX],
            power_map: vec![0; PWRMAPSIZE],

            s_map_x: 0,
            s_map_y: 0,
            c_chr: 0,
            c_chr9: 0,
            new_power: false,
            pwrd_z_cnt: 0,
            un_pwrd_z_cnt: 0,
            res_z_pop: 0,
            com_z_pop: 0,
            ind_z_pop: 0,
            fire_pop: 0,

            city_time: 0,
            total_funds: 20000,
            city_tax: 7,
            game_level: 0,
            sim_speed: 0,
            spd_cycle: 0,
            f_cycle: 0,
            s_cycle: 0,
            do_initial_eval: false,
            av_city_tax: 0,
            res_pop: 0,
            com_pop: 0,
            ind_pop: 0,
            total_pop: 0,
            last_total_pop: 0,
            res_valve: 0,
            com_valve: 0,
            ind_valve: 0,
            res_cap: false,
            com_cap: false,
            ind_cap: false,
            res_his: vec![0; 480],
            com_his: vec![0; 480],
            ind_his: vec![0; 480],
            money_his: vec![0; 480],
            crime_his: vec![0; 480],
            pollution_his: vec![0; 480],
            misc_his: vec![0; 240],
        }
    }

    pub fn step_simulation(&mut self) {
        self.sim_frame();
    }

    pub fn get_map_view(&self) -> Vec<u16> {
        self.map.iter().flatten().cloned().collect()
    }

    pub fn get_city_stats(&self) -> CityStats {
        CityStats {
            city_time: self.city_time,
            total_funds: self.total_funds,
            total_pop: self.total_pop,
            res_pop: self.res_pop,
            com_pop: self.com_pop,
            ind_pop: self.ind_pop,
        }
    }

    pub fn save_city(&self, path: String) -> PyResult<()> {
        let encoded = bincode::serialize(self).map_err(|e| PyErr::new::<pyo3::exceptions::PyIOError, _>(format!("Failed to serialize city: {}", e)))?;
        std::fs::write(&path, encoded).map_err(|e| PyErr::new::<pyo3::exceptions::PyIOError, _>(format!("Failed to write to file {}: {}", path, e)))?;
        Ok(())
    }

    #[staticmethod]
    pub fn load_city(path: String) -> PyResult<Micropolis> {
        let data = std::fs::read(&path).map_err(|e| PyErr::new::<pyo3::exceptions::PyIOError, _>(format!("Failed to read from file {}: {}", path, e)))?;
        let decoded: Micropolis = bincode::deserialize(&data).map_err(|e| PyErr::new::<pyo3::exceptions::PyIOError, _>(format!("Failed to deserialize city: {}", e)))?;
        Ok(decoded)
    }
}

/// A Python module implemented in Rust.
#[pymodule]
fn micropolis_rs(_py: Python<'_>, m: &Bound<'_, PyModule>) -> PyResult<()> {
    m.add_class::<CityStats>()?;
    m.add_class::<Micropolis>()?;
    m.add("DIRT", DIRT)?;
    m.add("RIVER", RIVER)?;
    m.add("TREEBASE", TREEBASE)?;
    m.add("WOODS", WOODS)?;
    m.add("ROADBASE", ROADBASE)?;
    m.add("RESBASE", RESBASE)?;
    m.add("COMBASE", COMBASE)?;
    m.add("INDBASE", INDBASE)?;
    m.add("PORTBASE", PORTBASE)?;
    Ok(())
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn it_works() {
        let micro = Micropolis::new();
        assert_eq!(micro.city_time, 0);
    }
}
