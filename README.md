# Open Source Micropolis, based on the original SimCity Classic from Maxis, by Will Wright. #

This is the source code for Micropolis (based on [SimCity](http://en.wikipedia.org/wiki/SimCity_(1989_video_game))), released under the GPL. Micropolis is based on the original SimCity from Electronic Arts / Maxis, and designed and written by Will Wright.

# NOTE: Please See: [MicropolisCore](https://github.com/SimHacker/MicropolisCore)

This "micropolis" repo is mainly for historical purpose, and it's probably better to use the newer [MicropolisCore](https://github.com/SimHacker/MicropolisCore) repo for development. 

I rebooted micropolis into the fresh new [MicropolisCore](https://github.com/SimHacker/MicropolisCore) repo, without all the old X11/TCL/Tk stuff.

[MicropolisCore](https://github.com/SimHacker/MicropolisCore) is a C++ rewrite/refactor/cleanup of the original C code into a simulation engine that's independent of the user interface. 

The full lineage of the code is C64 => Mac => SunOS/NeWS/HyperLook => Unix/X11/TCL/Tk + multiplayer => Linux/X11/TCL/Tk + multiplayer => OLPC/X11/TCL/Tk (single player) => C++ MicropolisCore => C++/SWIG/Python/PyGTK => C++/SWIG/Python/TurboGears/AMF/Flash/OpenLaszlo => C++/Emscripten/Embind/TypeScript/SvelteKit.

Now it compiles with emscripten/embind into WASM, and it's possible to run it "headless" in node, or with any kind of user interface in the browser.

So you're making something new, you should use [MicropolisCore](https://github.com/SimHacker/MicropolisCore), but if you're into retrocomputing and want to renovate the TCL version, or even the old Python/TurboGears/AMF/Flash/OpenLaszlo version, then this repo's got what you need!

## [Description](../wiki/Description.md) ##
A description of the Micropolis project source code release.

## [News](../wiki/News.md) ##
The latest news about recent development.

## [DevelopmentPlan](../wiki/DevelopmentPlan.md) ##
The development plan, and a high level description of tasks that need to be done.

## [ThePlan](../wiki/ThePlan.md) ##
Older development plan for the TCL/Tk version of Micropolis and the C++/Python version too.

## [Assets](../wiki/Assets.md) ##
List of art and text assets, and work that needs to be done for Micropolis.

## [Micropolis Public Name License](../wiki/MicropolisPublicNameLicense.md) ##
The name/term "MICROPOLIS" is a registered trademark owned by and
exclusively licensed to Micropolis GmbH (Micropolis Corporation, the
"licensor"). It is here (upstream) licensed to the authors/publishers of
the "Micropolis" city simulation game and its source code (the project or
"licensee(s)").

## Documentation ##

This is the old documentation of the HyperLook version of SimCity, converted to wiki text.
It needs to be brought up to date and illustrated.

  * [Introduction](../wiki/Introduction.md)
  * [Tutorial](../wiki/Tutorial.md)
  * [User Reference](../wiki/UserReference.md)
  * [Inside The Simulator](../wiki/InsideTheSimulator.md)
  * [History Of Cities And City Planning](../wiki/History.md)
  * [Bibliography](../wiki/Bibliography.md)
  * [Credits](../wiki/Credits.md)

## [License](../wiki/License.md) ##
The Micropolis GPL license.

## Tools ##
[![](http://wingware.com/images/coded-with-logo-129x66.png)](http://wingware.com/)
