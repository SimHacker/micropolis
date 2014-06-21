Micropolis::map[] array-
 *Consider changing type from 'char' to 'short'

Finances-
 *Pay operating costs either every week or at the beginning of the year,
  instead of at the end of the year. This will fix an issue where it is
  possible to get negative money... just make your expenses exceed your
  income and run completely out of money.

On Game Load-
 *All zones are unpowered for the first several ticks. I'd imagine there
  are similar problems with the landValueMap, pollutionMap, tfDensity,
  policeCoverage etc. I think what should happen is that the various methods
  that update these maps should be called at load time.
 *Evaluation data is not available.
 *cityTime may not match history.cityTime

Disasters-
 *When running FAST or SUPER_FAST, automatically pause or slow down the
  simulation when a disaster occurs.
 *Floods- should be allowed to flood zones, but I think this is currently
  not the case.

City size notifications-
 *Pause and pop up a message the first time the city reaches 2000, 10000,
  50000, etc. people.

Graphics-
 *Let graphics be pluggable; use a mechanism similar to how the UI strings
  are pluggable according to locale.

Draw-Bridge-
 *If there's traffic on a bridge, the draw bridge does not open correctly.

Scenarios-
 *Implement. See original C/Tcl code, src/sim/s_fileio.c LoadScenario(),
  and res/snro.111, res/snro.222, etc.
