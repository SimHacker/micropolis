_(This page is several years old. A lot of progress rewriting the code has been made since the notes written below.)_

# Notes on a development plan, in progress #

I (Don Hopkins) have been cleaning up and translated the Micropolis (SimCity) source code to C++, and used SWIG to integrate it with Python.
It runs and animates the city on the screen, but it's not yet a playable game.

The ongoing Micropolis Core development work is checked into subversion on google code, in the "MicropolisCore" subdirectory: http://code.google.com/p/micropolis/source/browse

Micropolis Core is still a work in progress, of course -- the game is not yet playable, but you can scroll and zoom around the map, and I've hooked up the pie menus for switching tools.

I've been working on the editing and navigation tools -- they are not actually hooked up for editing the map yet, but the tool framework is in place.
The TileTool base class represents an editing tool, including the tile based SimCity map editors, the ink drawing and eraser tools, and the panning tool. The TileDrawingView distributes events to the appropriate tools. There is a current editing tool, as well as a spring loaded navigation tool (for panning and zooming), which activates when you press a key.

One consideration is that I'm factoring the TileDrawingView and TileTool code into two layers: one generic layer that is specific to the tile view but not specific to Micropolis, and then a higher level layer which subclasses the generic classes. There is a set of subclasses specific to Micropolis, as well as another set of subclasses specific to a cellular automata machine (CellEngine). This layer of abstraction makes it a bit more complicated than it would be if it were monolithic, but it's well worth it, because I want the tile engine to be useful for other projects. Using the generic classes for more than one project ensures that they will be well factored and support other projects without special cases.

The C++ code still needs to be reorganized and cleaned up a lot -- I've just done the first cut of a translation. There is still a lot of old cruft in there that needs to be stripped out. But the tool stuff was a good obvious place to start working.

The essential thing is to decide what is user interface stuff that goes on the outside and should not be handled by C++, and what is essential simulation stuff that goes on the inside and should be handled by C++, and then defining a nice clean simple interface between them.

For example, one thing that I did to support the new tool stuff was to eliminate all the old "view" and "x11" oriented structures from C++ code that were being passed into the tool editing API. The old "view" was a TCL/Tk widget in the old micropolis-core code, with lots of X-Windows cruft hanging off of it. So now the view and user interface layer is implemented in Python (and another independent module, TileEngine), totally independent of the simulation engine. So somebody else could write a 3D view, or a web server view, or whatever.

Now the C++ code does not know about any view stuff -- it just knows how to expose the information that any kind of view might need to know. The "tile engine" is a kind of view that draws the map with zoomable tile images, using the Cairo graphics library. The "Micropolis engine" (the city simulator) has an interface for getting a pointer to its tile memory buffer. Python code directly introduces the Micropolis engine's tiles to the tile drawing engine, along with some information describing the layout and format of the tiles in memory, and a bitmap of tiles to use. So there is no Python interpreter overhead when drawing the tiles.

The high level tool user interface layer written in Python transforms mouse coordinates to tile coordinates, and calls micropolis to edit on the map in terms of tile coordinates. So the core simulator does not have to know anything about view transformations.

The tile engine has a lazy optimization to make it zoom faster, by not scaling all possible tiles in advance. The first time it draws a tile at a certain scale, it calls back into Python to render that tiles on demand. The Python code can scale a bitmap or draw scaled graphics with Cairo or SVG. Only the needed tiles are cached at any particular scale, which makes it possible to zoom in close to the map quickly without wasting a lot of time and memory.

There is lots of stuff that needs to be done along the lines of cleaning up and rationalizing the C++ code and interface.

The code is cluttered with unnecessary layers of indirection, superfluous methods and unused variables, much of which is left over from the old legacy platforms like TCL/Tk, X11 and MacOS.
Right now it is one giant public cluster fuck of a C++ class that encapsulates the entire simulator. In the long term it would be good to refactor it into multiple classes that don't know as much about each other. But the prerequisite to that is to strip it down, minimalize and privatize it as much as practical, then judiciously build it back up by adding useful utilities and interfaces, to support all kinds of interesting views and editors.

I've tried to do all the hard stuff first that requires intimate knowledge of the code and its history, and leave the stuff that is more obvious and straightforward (and fun) for other people to do.

The C++ header file and source code is organized to reflect the files that the original code was arranged in. This was useful for the clean-up process, just getting the code to work, but now that it works, there is no reason for it to be organized the arbitrary way it is.

For example there are several groups of randomly jumbled miscelaneuous functions, thunks, adaptors, compatibility libraries, glue, utilities, etc. Like functions that emulate old Mac APIs that are no longer necessary. They need to be ripped out and replaced by better organized code that does what is needed instead of doing what was being done for historical reasons.

A typical example is the random number generator, which should be standardized and seedable (so it can be synchronized for the multi player game) and replaced with a much better one. Another example is the diverse set of formatting utilities (like money and time and text messages): those don't really belong in the core, because they should be part of the user interface layer because of internationalization issues. The core should just expose raw data like floating point numbers or dates, and the user interface should fish them out and format them according to the locale.

Something that needs to be done systematically is to remove all strings hard coded into the code, and replace them with symbols that can be looked up by the user interface layer in translation files to display to the user. So any C++ code that formats messages should be rewritten to just expose the raw data, and then Python code should be written to take that data and properly format and display it so it can be internationalized and translated to other languages.

There are a bunch of old Mac resource oriented things inherited from the old code, which are ugly and brittle, and only make it harder to internationalize the code and manage its resources. Anything that looks like it's using Mac resources should be rewritten to use a simpler lower tech way of accomplishing whatever it's trying to do, or defer the work and decisions to Python code if appropriate, and just export raw essential data out of the C++ code.

It would be great for programmers to read over the C++ code with fresh eyes and write some notes and questions about "what the hell is it doing here" and "why is this so weird". Please send any notes or questions you have to me, and we can come up with a more detailed battle plan about what needs to be done to clean up and rationalize the code.

> -Don