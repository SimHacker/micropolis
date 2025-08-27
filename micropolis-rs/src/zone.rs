// This file will contain the zone-related logic ported from s_zone.c.

use crate::Micropolis;
use crate::{CHURCH, COMBASE, HOSPITAL, INDBASE, PORTBASE};

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
            // self.do_hosp_chur();
            return;
        }
        if self.c_chr9 < INDBASE {
            // self.do_commercial(zone_pwr_flag);
            return;
        }
        // self.do_industrial(zone_pwr_flag);
    }

    pub(crate) fn set_z_power(&mut self) -> bool {
        // TODO: Port power logic
        false
    }
}
