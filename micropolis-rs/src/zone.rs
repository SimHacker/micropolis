// This file will contain the zone-related logic ported from s_zone.c.

use crate::Micropolis;
use crate::{
    ANIMBIT, BNCNBIT, BULLBIT, BURNBIT, CHURCH, COMBASE, CONDBIT, FLOOD, HOSPITAL, INDBASE,
    LASTRUBBLE, LOMASK, NUCLEAR, PORTBASE, POWERPLANT, PWRBIT, PWRMAPSIZE, ROADBASE, RESBASE,
    RUBBLE, ZONEBIT,
};
impl Micropolis {
    pub(crate) fn do_zone(&mut self) {
        let zone_pwr_flag = self.set_z_power();
        if zone_pwr_flag {
            self.pwrd_z_cnt += 1;
        } else {
            self.un_pwrd_z_cnt += 1;
        }

        if self.c_chr9 > PORTBASE {
            // self.do_sp_zone(zone_pwr_flag);
            return;
        }
        if self.c_chr9 < HOSPITAL {
            // self.do_residential(zone_pwr_flag);
            return;
        }
        if self.c_chr9 < COMBASE {
            self.do_hosp_chur();
            return;
        }
        if self.c_chr9 < INDBASE {
            // self.do_commercial(zone_pwr_flag);
            return;
        }
        // self.do_industrial(zone_pwr_flag);
    }

    fn do_hosp_chur(&mut self) {
        if self.c_chr9 == HOSPITAL as u16 {
            self.hosp_pop += 1;
            if (self.city_time & 15) == 0 {
                self.repair_zone(HOSPITAL, 3);
            }
            if self.need_hosp == -1 {
                self.zone_plop(RESBASE);
            }
        }
        if self.c_chr9 == CHURCH as u16 {
            self.church_pop += 1;
            if (self.city_time & 15) == 0 {
                self.repair_zone(CHURCH, 3);
            }
            if self.need_church == -1 {
                self.zone_plop(RESBASE);
            }
        }
    }

    fn repair_zone(&mut self, zone_center: u16, mut zone_size: i16) {
        zone_size -= 1;
        let mut cnt = 0;
        for y in -1..zone_size {
            for x in -1..zone_size {
                cnt += 1;
                let xx = self.s_map_x + x;
                let yy = self.s_map_y + y;

                if self.test_bounds(xx, yy) {
                    let mut th_ch = self.map[xx as usize][yy as usize];
                    if (th_ch & ZONEBIT) != 0 {
                        continue;
                    }
                    if (th_ch & ANIMBIT) != 0 {
                        continue;
                    }
                    th_ch &= LOMASK;
                    if (th_ch >= RUBBLE && th_ch <= LASTRUBBLE) || th_ch < ROADBASE {
                        self.map[xx as usize][yy as usize] =
                            zone_center - 3 - zone_size as u16 + cnt + CONDBIT + BURNBIT;
                    }
                }
            }
        }
    }

    fn zone_plop(&mut self, mut base: u16) {
        let zx = [-1, 0, 1, -1, 0, 1, -1, 0, 1];
        let zy = [-1, -1, -1, 0, 0, 0, 1, 1, 1];

        for z in 0..9 {
            let xx = self.s_map_x + zx[z];
            let yy = self.s_map_y + zy[z];
            if self.test_bounds(xx, yy) {
                let tile = self.map[xx as usize][yy as usize] & LOMASK;
                if tile >= FLOOD && tile < ROADBASE {
                    return;
                }
            }
        }

        for z in 0..9 {
            let xx = self.s_map_x + zx[z];
            let yy = self.s_map_y + zy[z];
            if self.test_bounds(xx, yy) {
                self.map[xx as usize][yy as usize] = base + BNCNBIT;
            }
            base += 1;
        }

        self.c_chr = self.map[self.s_map_x as usize][self.s_map_y as usize];
        self.set_z_power();
        self.map[self.s_map_x as usize][self.s_map_y as usize] |= ZONEBIT | BULLBIT;
    }

    pub(crate) fn set_z_power(&mut self) -> bool {
        if self.c_chr9 == NUCLEAR || self.c_chr9 == POWERPLANT {
            self.map[self.s_map_x as usize][self.s_map_y as usize] |= PWRBIT;
            return true;
        }

        let power_word = self.power_word(self.s_map_x, self.s_map_y);
        if power_word < PWRMAPSIZE {
            if self.power_map[power_word] & (1 << (self.s_map_x & 15)) != 0 {
                self.map[self.s_map_x as usize][self.s_map_y as usize] |= PWRBIT;
                return true;
            }
        }

        self.map[self.s_map_x as usize][self.s_map_y as usize] &= !PWRBIT;
        false
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::{FIRE, NUCLEAR, POWERPLANT, PWRBIT};

    #[test]
    fn test_do_hosp_chur() {
        let mut micropolis = Micropolis::new();
        micropolis.c_chr9 = HOSPITAL as u16;
        micropolis.do_hosp_chur();
        assert_eq!(micropolis.hosp_pop, 1);

        micropolis.need_hosp = -1;
        micropolis.do_hosp_chur();
        assert_eq!(micropolis.hosp_pop, 2);
    }

    #[test]
    fn test_zone_plop() {
        let mut micropolis = Micropolis::new();
        micropolis.s_map_x = 10;
        micropolis.s_map_y = 10;

        micropolis.zone_plop(RESBASE);

        // Assert that the tiles have been plopped
        for y in -1..=1 {
            for x in -1..=1 {
                let xx = (micropolis.s_map_x + x) as usize;
                let yy = (micropolis.s_map_y + y) as usize;
                assert_ne!(micropolis.map[xx][yy], 0);
            }
        }

        // Assert that fire prevents plopping
        micropolis.map[10][10] = FIRE;
        micropolis.zone_plop(RESBASE);
        assert_eq!(micropolis.map[10][10], FIRE);
    }

    #[test]
    fn test_set_z_power() {
        let mut micropolis = Micropolis::new();
        micropolis.s_map_x = 10;
        micropolis.s_map_y = 10;

        // Test with power map
        let power_word = micropolis.power_word(micropolis.s_map_x, micropolis.s_map_y);
        micropolis.power_map[power_word] |= 1 << (micropolis.s_map_x & 15);
        assert!(micropolis.set_z_power());
        assert_ne!(
            micropolis.map[micropolis.s_map_x as usize][micropolis.s_map_y as usize] & PWRBIT,
            0
        );

        micropolis.power_map[power_word] &= !(1 << (micropolis.s_map_x & 15));
        assert!(!micropolis.set_z_power());
        assert_eq!(
            micropolis.map[micropolis.s_map_x as usize][micropolis.s_map_y as usize] & PWRBIT,
            0
        );

        // Test with nuclear power plant
        micropolis.c_chr9 = NUCLEAR;
        assert!(micropolis.set_z_power());

        // Test with coal power plant
        micropolis.c_chr9 = POWERPLANT;
        assert!(micropolis.set_z_power());
    }

    #[test]
    fn test_repair_zone() {
        let mut micropolis = Micropolis::new();
        micropolis.s_map_x = 10;
        micropolis.s_map_y = 10;

        // Set some tiles to rubble
        micropolis.map[10][10] = RUBBLE;
        micropolis.map[11][11] = RUBBLE;

        micropolis.repair_zone(HOSPITAL, 3);

        // Assert that the rubble has been repaired
        assert_ne!(micropolis.map[10][10], RUBBLE);
        assert_ne!(micropolis.map[11][11], RUBBLE);

        // Also assert that a non-rubble tile was not changed
        micropolis.map[12][12] = ROADBASE;
        micropolis.repair_zone(HOSPITAL, 3);
        assert_eq!(micropolis.map[12][12], ROADBASE);
    }
}
