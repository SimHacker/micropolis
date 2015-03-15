# News #

This page presents a history of the Micropolis open source release.

## Students from Freie Universität in Berlin enhance Micropolis (June 16, 2014) ##

A team of students taking a three-week programming course at
Freie Universität Berlin this Spring have studied the MicropolisJ
source code and developed enhancements to it, which they have named
**DiverCity**.

The source code (along with a description of their enhancements)
is available from their Github repository:
https://github.com/Team--Rocket/divercity

Go check it out!

## Java Micropolis, version 1.6 (December 1, 2013) ##

A minor update to the Java Micropolis project. This version adds
German language support, bringing the total number of supported locales to four (English, French, German, and Swedish). There are also some fixes for a few minor bugs in how pollution was handled, and some more internal improvements to the code. The internal improvements are mainly to do with how tile behavior is specified. Instead of hard-coding tile numbers all through the code, effort is being made to make the behavior dependent on attributes specified in the tiles.rc control file.

## Java Micropolis, version 1.5 (August 31, 2013) ##

Another update to the Java Micropolis project. This version adds
support for zooming the main map in/out and canceling a tool
operation by pressing Escape. It also contains fixes for a few
minor bugs in the simulation logic, and various internal
improvements to the code.

## Java Micropolis, version 1.3 (May 22, 2013) ##

An update to the Java Micropolis project. This version redesigns the tool functionality, making it easier to place roads and buildings in straight lines. (However, now it's harder to free-draw things; let us know what you think!)

This update also includes a new utility for translating the text that
appears in the game. See [Localization](Localization.md) for information on how to use it.

## Java Micropolis, version 1.2 (April 27, 2013) ##

Another minor update to the Java Micropolis project. This version
fixes just a few minor bugs that were discovered, but
introduces no new features or major changes in functionality.

If you are using version 1.0 or version 1.1, simply download the
new version and extract it on top of the old version. Saved cities
will load just fine.

## Java Micropolis, version 1.1 (February 24, 2013) ##

Released a minor update to the Java Micropolis project. This version
is recompiled so that it will run on version 6 of the JRE (making it
easier for Mac users, since they can now use the Apple-provided run-time). It also fixes a couple bugs, the most critical being that the city randomly stops running when there are roads or tracks along the edge of the map.

If you are using version 1.0, simply download the new version and extract it on top of the old version. Saved cities will load just fine in the new version.

## Java Rewrite (February 15, 2013) ##

There is now a version of Micropolis written in the Java programming language, contributed by Jason Long. This makes Micropolis more easily accessible to users on Windows and Mac platforms. First make sure you have a recent version of Java on your computer, then download Micropolis (Java edition) from this site.

Or, if you're interested in the source, check out the `trunk/micropolis-java` repository under the Source tab of this site.

For assistance with this version of Micropolis, contact Jason Long at jason@long.name.

## New Update (April 2011) ##

We have come a long way in the years since releasing the original TCL/Tk/X11 version of SimCity for the OLPC:
  * Cleaned up and simplified the original SimCity Classic C code.
  * Rewrote all the old C code into portable C++ code with modern best practices for reusability.
  * Refactored code into independent modules for comprehensibility, modularity and maintainability.
  * Renamed variables, functions and classes for readability, consistency and obviousness.
  * Extensively commented and documented code and headers for learnability.
  * Created a reusable, language and user interface independent [MicropolisCore](http://code.google.com/p/micropolis/source/browse/#svn/trunk/MicropolisCore/src/MicropolisEngine/src) module.
  * Created a reusable [TileEngine](http://code.google.com/p/micropolis/source/browse/#svn/trunk/MicropolisCore/src/TileEngine/src) module, supporting Cairo and Flash AMF.
  * Wrote Doxygen source code comments with detailed explanations of how the simulator works, to create [cross referenced code and API documentation](http://micropolisonline.com/doc/index.html).
  * Integrated MicropolisCore with [Python](http://code.google.com/p/micropolis/source/browse/#svn/trunk/MicropolisCore/src/pyMicropolis/micropolisEngine) and [C#](http://code.google.com/p/micropolis/source/browse/#svn/trunk/MicropolisCore/src/csMicropolis) (via SWIG), so the user interface and game logic can be [scripted and extended in Python](http://code.google.com/p/micropolis/source/browse/trunk/MicropolisCore/src/pyMicropolis/micropolisEngine/micropolisrobot.py?r=477).
  * Developed a [desktop user interface](http://code.google.com/p/micropolis/source/browse/trunk/MicropolisCore/src/pyMicropolis/micropolisEngine/micropolisgenericengine.py?r=477) with the Python/GTK/Cairo/Pango/X11 stack.
  * Developed a [web based user interface](http://code.google.com/p/micropolis/source/browse/trunk/MicropolisCore/src/pyMicropolis/micropolisEngine/micropolisturbogearsengine.py?r=477) with the Python/[TurboGears](http://code.google.com/p/micropolis/source/browse/trunk/?r=477#trunk/turbogears/micropolis)/AMF/Flash/[OpenLaszlo](http://code.google.com/p/micropolis/source/browse/#svn/trunk/laszlo/micropolis/classes) stack.
  * Published a [playable web based Micropolis Online](http://micropolisonline.com/lps/micropolis/micropolis/micropolis_en-US.lzx?lzt=swf&lzr=swf10) server based game, in English and  [Dutch](http://micropolisonline.com/lps/micropolis/micropolis/micropolis_nl-NL.lzx?lzt=swf&lzr=swf10).
  * Presented a [Lightning Talk and Demo of Micropolis](http://www.youtube.com/watch?v=_9oy0LjGXnM&feature=&p=3509FB083052CD90&index=0&playnext=1) at HAR 2009.
  * [Lots](http://code.google.com/p/micropolis/source/browse/trunk/micropolis-activity/src/notes/MultiPlayerIdeas.txt?r=477) [and](http://code.google.com/p/micropolis/source/browse/trunk/micropolis-activity/src/notes/Beyond-Intelligent-Machines?r=477) [lots](http://code.google.com/p/micropolis/source/browse/trunk/micropolis-activity/src/notes/InterCHI-Interactive-Experience-Proposal?r=477) [of](http://code.google.com/p/micropolis/source/browse/trunk/?r=477#trunk/micropolis-activity/manual) [other](http://code.google.com/p/micropolis/source/browse/#svn/trunk/MicropolisCore/src/CellEngine/src) [wild](http://code.google.com/p/micropolis/source/browse/trunk/micropolis-activity/src/notes/MultiPlayerIdeas.txt?r=477) [and](http://code.google.com/p/micropolis/source/browse/trunk/micropolis-activity/src/notes/OLPC-notes.txt?r=477) [crazy](http://code.google.com/p/micropolis/source/browse/trunk/MicropolisCore/src/pyMicropolis/piemenu/piemenu.py?r=477) [stuff](http://code.google.com/p/micropolis/source/browse/trunk/laszlo/micropolis/classes/piemenu.lzx?r=477) you can find browsing around the [source code](http://code.google.com/p/micropolis/source/browse/trunk/?r=477).
  * Don Hopkins (the primary developer of the Micropolis branch of SimCity) joined the [Stupid Fun Club](http://www.StupidFunClub.com), an entertainment think tank started up by Will Wright (the creator of SimCity). Will's strong support, advice and mentoring made the Micropolis project possible, and the open source project will continue to benefit from his involvement and ideas.


## Web-Based Version of Micropolis (February 2010, Don Hopkins) ##

Latest update: I have started developing a web based version of Micropolis, using TurboGears, for the "Micropolis Eduverse" project. The open source code is now checked in, in the "micropolis" directory!


## Multiple Map Views (April 1 2007, Don Hopkins) ##

I just implemented support for multiple views and the map overview: one window has the entire map overview at 25% scale, with rectangles to show the positions of other views on the map. Editing is disabled in the map overview because it's used for panning the other views. Three more windows have different views on the map, at different scales, which you can pan and zoom around. You can pan the views by dragging the rectangles on the map overview, or pressing shift in the map window and moving the mouse, or by using the arrow keys to move the cursor off the edge of the screen. You can zoom in and out by using the mouse wheel in the map view, or while you're pointing at the rectangle for a view in the map overview, or typing into the map view "i" and "o" for in and out (a little), "I" and "O" for in and out (a lot), or typing "r" to reset to 100% scale.

Now the MicropolisModel class keeps a list of all views, and has the timer that drives the simulation. When it ticks the model, all views are updated. Refactored the TileTool class to keep all information about the view in the view, not the tool, because now with multiple views, tools are being used by more than one view, so all view specific information should go in the view instead of the tool.

## Other Stuff ##

For more Micropolis news, see Don Hopkins' Blog:  http://www.DonHopkins.com.