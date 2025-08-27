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

    fn city_evaluation(&mut self) {}
    fn set_valves(&mut self) {}
    fn clear_census(&mut self) {}
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
