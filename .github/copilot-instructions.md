# Micropolis AI Coding Assistant Guide

## Project Overview
Micropolis is an open-source port of the original SimCity Classic (1989) by Will Wright. This codebase contains **multiple parallel implementations** targeting different platforms and use cases.

## Critical Architecture Understanding

### Multi-Implementation Structure
This is NOT a monolithic project. It contains 5+ distinct implementations:

1. **MicropolisCore/** - Modern C++ engine (v5.0) with Python/SWIG bindings
   - Core simulation: `MicropolisCore/src/MicropolisEngine/`
   - Tile rendering: `MicropolisCore/src/TileEngine/`
   - Python bindings: `MicropolisCore/src/pyMicropolis/`

2. **micropolis-activity/** - Original C/Tcl/Tk version for OLPC (One Laptop Per Child)
   - Legacy engine: `micropolis-activity/src/sim/*.c`
   - Tcl/Tk UI: `micropolis-activity/res/*.tcl`

3. **micropolis-java/** - Pure Java port (MicropolisJ)
   - Build: `ant build` (not make)
   - Engine: `micropolis-java/src/micropolisj/engine/`

4. **turbogears/** - Python web server with TurboGears + OpenLaszlo/Flash frontend
   - AMF remoting for efficient binary messaging
   - Duplicate MicropolisCore: `turbogears/micropolis/MicropolisCore/`

5. **laszlo/** - OpenLaszlo/Flash client (XML compiled to SWF)

**Critical**: Changes to one implementation DO NOT affect others. They share concepts but not code.

## Build System Patterns

### Top-level Makefile
```bash
make          # Builds MicropolisCore and micropolis-activity only
make install  # Installs those two
```
Does NOT build Java or TurboGears versions automatically.

### Component-specific Builds
- **C++ Engine**: `cd MicropolisCore/src && make all`
- **Java**: `cd micropolis-java && ant build`  
- **TurboGears**: `cd turbogears && python setup.py develop`
- **OLPC Activity**: `cd micropolis-activity/src/sim && make`

### Python 2 Legacy
All Python code uses Python 2 syntax:
- `print "text"` not `print("text")`
- `except Exception, e:` not `except Exception as e:`
- `from StringIO import StringIO` not `from io import StringIO`
- `import urllib2` not `import urllib.request`

## Map Data Structure Patterns

### Multi-Scale Map Storage
The simulation uses **multiple overlapping grids** at different resolutions:

- **Full resolution**: 120×100 tiles (`WORLD_W` × `WORLD_H`)
- **2×2 blocks**: 60×50 (`WORLD_W_2` × `WORLD_H_2`) - land value, pollution, crime
- **4×4 blocks**: 30×25 (`WORLD_W_4` × `WORLD_H_4`) - terrain density
- **8×8 blocks**: 15×13 (`WORLD_W_8` × `WORLD_H_8`) - fire/police coverage, growth rate

See `MicropolisCore/src/MicropolisEngine/src/map_type.h` for the `Map<DATA, BLKSIZE>` template.

### Tile Value Encoding
Tiles use 16-bit values with bit flags (see `micropolis.h`):
- Low 10 bits: tile type
- High 6 bits: flags (ANIMBIT, BULLBIT, BURNBIT, CONDBIT, ZONEBIT, etc.)

## Code Quality Issues to Address

### Commented-Out Code
- `MicropolisCore/src/MicropolisEngine/src/tool.cpp:328-343` - Dead coordinate arrays
- `MicropolisCore/src/MicropolisEngine/src/graph.cpp:75-83` - Unused color arrays
- `MicropolisCore/src/MicropolisEngine/src/micropolis.cpp:116+` - Type comments

**When editing**: Remove dead code rather than accumulating more comments.

### Debug Print Statements
Many files have uncommented debug prints:
- `turbogears/micropolis/controllers.py` - Facebook API debugging
- `MicropolisCore/src/pyMicropolis/` - Various `print "DEBUG"` statements

**Pattern**: Convert to proper logging with `logging` module, don't use bare prints.

### TODO/FIXME Markers
Extensive TODO comments exist (20+ in tool.cpp alone). Common patterns:
- Auto-bulldoze costs should use table-driven approach (lines 272, 322-323)
- Building properties duplication (line 220)
- Complex code needing refactoring (line 981+)

**When addressing**: Look for similar patterns elsewhere; fixes often apply broadly.

## License Header Pattern
EVERY source file must include the 60-line Electronic Arts GPL header with "ADDITIONAL TERMS per GNU GPL Section 7". See any .cpp/.h file for the template. This is legally required.

## Testing Approach
**No automated test suite exists.** Testing is manual:
- Build and run each version independently
- Load sample cities from `*/cities/*.cty`
- Verify simulation mechanics manually

## Integration Points

### C++/Python Bridge (SWIG)
- Interface: `MicropolisCore/src/MicropolisEngine/swig/micropolis.i`
- Python modules generated: `Micropolis`, `TileEngine`, `CellEngine`
- Pattern: C++ objects wrapped as Python classes with automatic memory management

### TurboGears/Flash Communication
- Protocol: AMF (Action Message Format) binary serialization
- Python: PyAMF library in `turbogears/micropolis/controllers.py`
- Pattern: RPC calls with binary tile data transfer for efficiency

## Current State Context
- Project last actively maintained ~2010-2013
- Modern Python 3 migration NOT done
- Some Facebook/Google API keys are placeholder 'XXX' values
- Windows build configurations likely outdated (.vcproj files)
