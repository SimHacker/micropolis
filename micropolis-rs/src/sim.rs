use crate::Micropolis;

impl Micropolis {
    pub fn sim_frame(&mut self) {
        if self.sim_speed == 0 {
            return;
        }

        self.spd_cycle = (self.spd_cycle + 1) % 1024;

        if self.sim_speed == 1 && self.spd_cycle % 5 != 0 {
            return;
        }

        if self.sim_speed == 2 && self.spd_cycle % 3 != 0 {
            return;
        }

        self.f_cycle = (self.f_cycle + 1) % 1024;
        self.simulate(self.f_cycle & 15);
    }

    fn simulate(&mut self, mod16: i16) {
        let spd_pwr = [1, 2, 4, 5];
        let spd_ptl = [1, 2, 7, 17];
        let spd_cri = [1, 1, 8, 18];
        let spd_pop = [1, 1, 9, 19];
        let spd_fir = [1, 1, 10, 20];
        let mut x = self.sim_speed;
        if x > 3 {
            x = 3;
        }

        match mod16 {
            0 => {
                self.s_cycle = (self.s_cycle + 1) % 1024;
                if self.do_initial_eval {
                    self.do_initial_eval = false;
                    self.city_evaluation();
                }
                self.city_time += 1;
                self.av_city_tax += self.city_tax;
                if self.s_cycle & 1 == 0 {
                    self.set_valves();
                }
                self.clear_census();
            }
            1 => self.map_scan(0, (1 * crate::WORLD_X / 8) as i16),
            2 => self.map_scan((1 * crate::WORLD_X / 8) as i16, (2 * crate::WORLD_X / 8) as i16),
            3 => self.map_scan((2 * crate::WORLD_X / 8) as i16, (3 * crate::WORLD_X / 8) as i16),
            4 => self.map_scan((3 * crate::WORLD_X / 8) as i16, (4 * crate::WORLD_X / 8) as i16),
            5 => self.map_scan((4 * crate::WORLD_X / 8) as i16, (5 * crate::WORLD_X / 8) as i16),
            6 => self.map_scan((5 * crate::WORLD_X / 8) as i16, (6 * crate::WORLD_X / 8) as i16),
            7 => self.map_scan((6 * crate::WORLD_X / 8) as i16, (7 * crate::WORLD_X / 8) as i16),
            8 => self.map_scan((7 * crate::WORLD_X / 8) as i16, crate::WORLD_X as i16),
            9 => {
                // ...
            }
            10 => {
                // ...
            }
            11 => {
                if self.s_cycle % spd_pwr[x as usize] == 0 {
                    self.do_power_scan();
                }
            }
            12 => {
                if self.s_cycle % spd_ptl[x as usize] == 0 {
                    self.ptl_scan();
                }
            }
            13 => {
                if self.s_cycle % spd_cri[x as usize] == 0 {
                    self.crime_scan();
                }
            }
            14 => {
                if self.s_cycle % spd_pop[x as usize] == 0 {
                    self.pop_den_scan();
                }
            }
            15 => {
                if self.s_cycle % spd_fir[x as usize] == 0 {
                    self.fire_analysis();
                }
                self.do_disasters();
            }
            _ => (),
        }
    }

    fn city_evaluation(&mut self) {
        self.eval_valid = false;
        if self.total_pop > 0 {
            self.get_ass_value();
            self.do_pop_num();
            self.do_problems();
            self.get_score();
            self.do_votes();
            self.change_eval();
        } else {
            self.eval_init();
            self.change_eval();
        }
        self.eval_valid = true;
    }
    fn get_ass_value(&mut self) {
        let mut z = (self.road_total * 5) as i64;
        z += (self.rail_total * 10) as i64;
        z += (self.police_pop * 1000) as i64;
        z += (self.fire_st_pop * 1000) as i64;
        z += (self.hosp_pop * 400) as i64;
        z += (self.stadium_pop * 3000) as i64;
        z += (self.port_pop * 5000) as i64;
        z += (self.aport_pop * 10000) as i64;
        z += (self.coal_pop * 3000) as i64;
        z += (self.nuclear_pop * 6000) as i64;
        self.city_ass_value = z * 1000;
    }
    fn do_pop_num(&mut self) {
        let old_city_pop = self.city_pop;
        self.city_pop = ((self.res_pop + (self.com_pop * 8) + (self.ind_pop * 8)) * 20) as i64;
        if old_city_pop == -1 {
            self.delta_city_pop = 0;
        } else {
            self.delta_city_pop = self.city_pop - old_city_pop;
        }

        self.city_class = 0;
        if self.city_pop > 2000 {
            self.city_class += 1;
        }
        if self.city_pop > 10000 {
            self.city_class += 1;
        }
        if self.city_pop > 50000 {
            self.city_class += 1;
        }
        if self.city_pop > 100000 {
            self.city_class += 1;
        }
        if self.city_pop > 500000 {
            self.city_class += 1;
        }
    }
    fn do_problems(&mut self) {
        for z in 0..crate::PROBNUM {
            self.problem_table[z] = 0;
        }
        self.problem_table[0] = self.crime_average;
        self.problem_table[1] = self.pollute_average;
        self.problem_table[2] = (self.lv_average as f32 * 0.7) as i16;
        self.problem_table[3] = self.city_tax * 10;
        self.problem_table[4] = self.average_trf();
        self.problem_table[5] = self.get_unemployment();
        self.problem_table[6] = self.get_fire();
        self.vote_problems();
        for z in 0..crate::PROBNUM {
            self.problem_taken[z] = 0;
        }
        for z in 0..4 {
            let mut max = 0;
            let mut this_prob = 0;
            for x in 0..7 {
                if self.problem_votes[x] > max && self.problem_taken[x] == 0 {
                    this_prob = x;
                    max = self.problem_votes[x];
                }
            }
            if max > 0 {
                self.problem_taken[this_prob] = 1;
                self.problem_order[z] = this_prob as i16;
            } else {
                self.problem_order[z] = 7;
                self.problem_table[7] = 0;
            }
        }
    }
    fn vote_problems(&mut self) {}
    fn average_trf(&mut self) -> i16 {
        0
    }
    fn get_unemployment(&mut self) -> i16 {
        0
    }
    fn get_fire(&mut self) -> i16 {
        0
    }
    fn get_score(&mut self) {}
    fn do_votes(&mut self) {}
    fn change_eval(&mut self) {}
    fn eval_init(&mut self) {}

    fn set_valves(&mut self) {}
    fn clear_census(&mut self) {
        self.pwrd_z_cnt = 0;
        self.un_pwrd_z_cnt = 0;
        self.fire_pop = 0;
        self.road_total = 0;
        self.rail_total = 0;
        self.res_pop = 0;
        self.com_pop = 0;
        self.ind_pop = 0;
        self.res_z_pop = 0;
        self.com_z_pop = 0;
        self.ind_z_pop = 0;
        self.hosp_pop = 0;
        self.church_pop = 0;
        self.police_pop = 0;
        self.fire_st_pop = 0;
        self.stadium_pop = 0;
        self.coal_pop = 0;
        self.nuclear_pop = 0;
        self.port_pop = 0;
        self.aport_pop = 0;
        self.power_stack_num = 0;
        for x in 0..crate::SMX {
            for y in 0..crate::SMY {
                self.fire_st_map[x][y] = 0;
                self.police_map[x][y] = 0;
            }
        }
    }
    fn map_scan(&mut self, x1: i16, x2: i16) {
        for x in x1..x2 {
            for y in 0..crate::WORLD_Y as i16 {
                self.s_map_x = x;
                self.s_map_y = y;
                if let Some(chr) = self.get_tile(x, y) {
                    self.c_chr = chr;
                    if chr == 0 {
                        continue;
                    }
                    self.c_chr9 = chr & crate::LOMASK;
                    if self.c_chr9 >= crate::FLOOD {
                        if self.c_chr9 < crate::ROADBASE {
                            if self.c_chr9 >= crate::FIREBASE {
                                self.fire_pop += 1;
                                if self.rand(4) == 0 {
                                    self.do_fire();
                                }
                                continue;
                            }
                            if self.c_chr9 < crate::RADTILE {
                                self.do_flood();
                            } else {
                                self.do_rad_tile();
                            }
                            continue;
                        }

                        if self.new_power && (self.c_chr & crate::CONDBIT) != 0 {
                            self.set_z_power();
                        }

                        if self.c_chr9 >= crate::ROADBASE && self.c_chr9 < crate::POWERBASE {
                            self.do_road();
                            continue;
                        }

                        if (self.c_chr & crate::ZONEBIT) != 0 {
                            self.do_zone();
                            continue;
                        }

                        if self.c_chr9 >= crate::RAILBASE && self.c_chr9 < crate::RESBASE {
                            self.do_rail();
                            continue;
                        }

                        if self.c_chr9 >= crate::SOMETINYEXP && self.c_chr9 <= crate::LASTTINYEXP {
                            let rand_val = self.rand(4);
                            self.set_tile(x, y, crate::RUBBLE + rand_val | crate::BULLBIT);
                        }
                    }
                }
            }
        }
    }

    fn get_tile(&self, x: i16, y: i16) -> Option<u16> {
        if x < 0 || x >= crate::WORLD_X as i16 || y < 0 || y >= crate::WORLD_Y as i16 {
            None
        } else {
            Some(self.map[x as usize][y as usize])
        }
    }

    fn set_tile(&mut self, x: i16, y: i16, tile: u16) {
        if x >= 0 && x < crate::WORLD_X as i16 && y >= 0 && y < crate::WORLD_Y as i16 {
            self.map[x as usize][y as usize] = tile;
        }
    }

    fn rand(&mut self, _range: u16) -> u16 {
        // A proper random number generator will be needed here.
        // For now, returning a constant to allow compilation.
        0
    }

    fn do_fire(&mut self) {
        // TODO: Port fire logic
    }

    fn do_flood(&mut self) {
        // TODO: Port flood logic
    }

    fn do_rad_tile(&mut self) {
        // TODO: Port radiation logic
    }

    fn do_road(&mut self) {
        // TODO: Port road logic
    }

    fn do_rail(&mut self) {
        // TODO: Port rail logic
    }

    fn do_power_scan(&mut self) {}
    fn ptl_scan(&mut self) {}
    fn crime_scan(&mut self) {}
    fn pop_den_scan(&mut self) {}
    fn fire_analysis(&mut self) {}
    fn do_disasters(&mut self) {}
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_do_problems() {
        let mut micropolis = Micropolis::new();
        micropolis.crime_average = 100;
        micropolis.pollute_average = 100;
        micropolis.lv_average = 100;
        micropolis.city_tax = 7;

        micropolis.do_problems();

        assert_eq!(micropolis.problem_table[0], 100);
        assert_eq!(micropolis.problem_table[1], 100);
        assert_eq!(micropolis.problem_table[2], 70);
        assert_eq!(micropolis.problem_table[3], 70);
    }

    #[test]
    fn test_do_pop_num() {
        let mut micropolis = Micropolis::new();
        micropolis.res_pop = 100;
        micropolis.com_pop = 100;
        micropolis.ind_pop = 100;

        micropolis.do_pop_num();

        let expected_city_pop = (100 + (100 * 8) + (100 * 8)) * 20;
        assert_eq!(micropolis.city_pop, expected_city_pop);
        assert_eq!(micropolis.delta_city_pop, expected_city_pop);
        assert_eq!(micropolis.city_class, 2);
    }

    #[test]
    fn test_get_ass_value() {
        let mut micropolis = Micropolis::new();
        micropolis.road_total = 10;
        micropolis.rail_total = 10;
        micropolis.police_pop = 10;
        micropolis.fire_st_pop = 10;
        micropolis.hosp_pop = 10;
        micropolis.stadium_pop = 1;
        micropolis.port_pop = 1;
        micropolis.aport_pop = 1;
        micropolis.coal_pop = 1;
        micropolis.nuclear_pop = 1;

        micropolis.get_ass_value();

        let mut expected_value = (10 * 5) as i64;
        expected_value += (10 * 10) as i64;
        expected_value += (10 * 1000) as i64;
        expected_value += (10 * 1000) as i64;
        expected_value += (10 * 400) as i64;
        expected_value += (1 * 3000) as i64;
        expected_value += (1 * 5000) as i64;
        expected_value += (1 * 10000) as i64;
        expected_value += (1 * 3000) as i64;
        expected_value += (1 * 6000) as i64;
        expected_value *= 1000;

        assert_eq!(micropolis.city_ass_value, expected_value);
    }

    #[test]
    fn test_clear_census() {
        let mut micropolis = Micropolis::new();
        micropolis.pwrd_z_cnt = 1;
        micropolis.un_pwrd_z_cnt = 1;
        micropolis.fire_pop = 1;
        micropolis.road_total = 1;
        micropolis.rail_total = 1;
        micropolis.res_pop = 1;
        micropolis.com_pop = 1;
        micropolis.ind_pop = 1;
        micropolis.res_z_pop = 1;
        micropolis.com_z_pop = 1;
        micropolis.ind_z_pop = 1;
        micropolis.hosp_pop = 1;
        micropolis.church_pop = 1;
        micropolis.police_pop = 1;
        micropolis.fire_st_pop = 1;
        micropolis.stadium_pop = 1;
        micropolis.coal_pop = 1;
        micropolis.nuclear_pop = 1;
        micropolis.port_pop = 1;
        micropolis.aport_pop = 1;
        micropolis.power_stack_num = 1;
        micropolis.fire_st_map[0][0] = 1;
        micropolis.police_map[0][0] = 1;

        micropolis.clear_census();

        assert_eq!(micropolis.pwrd_z_cnt, 0);
        assert_eq!(micropolis.un_pwrd_z_cnt, 0);
        assert_eq!(micropolis.fire_pop, 0);
        assert_eq!(micropolis.road_total, 0);
        assert_eq!(micropolis.rail_total, 0);
        assert_eq!(micropolis.res_pop, 0);
        assert_eq!(micropolis.com_pop, 0);
        assert_eq!(micropolis.ind_pop, 0);
        assert_eq!(micropolis.res_z_pop, 0);
        assert_eq!(micropolis.com_z_pop, 0);
        assert_eq!(micropolis.ind_z_pop, 0);
        assert_eq!(micropolis.hosp_pop, 0);
        assert_eq!(micropolis.church_pop, 0);
        assert_eq!(micropolis.police_pop, 0);
        assert_eq!(micropolis.fire_st_pop, 0);
        assert_eq!(micropolis.stadium_pop, 0);
        assert_eq!(micropolis.coal_pop, 0);
        assert_eq!(micropolis.nuclear_pop, 0);
        assert_eq!(micropolis.port_pop, 0);
        assert_eq!(micropolis.aport_pop, 0);
        assert_eq!(micropolis.power_stack_num, 0);
        assert_eq!(micropolis.fire_st_map[0][0], 0);
        assert_eq!(micropolis.police_map[0][0], 0);
    }
}
