_(This page is several years old. A lot of progress rewriting the code has been made since the list of assets below.)_

# Micropolis Assets #

This is a list of required and optional assets for Micropolis.

As noted, some of them are for the current TCL/Tk based version of Micropolis that we want to get out right away.  Others are for the more advanced Python version that we're developing after releasing the TCL/Tk version.

The TCL/Tk version currently works ok with its current assets, and these are just suggestions that would improve it but are not necessary. The Python version is not yet fully designed or implemented, and it will be more open-ended and powerful, so this is just a rough estimate of the assets it will need.

## User Interface Graphics and Icons ##

These aren't all necessary, but it would be nice to have some of them to make the interface look nicer, more graphical, and kid friendly.

The style should be international, kid oriented, big, bright, saturated, simple.

Icons should look good and be readable when shown in desaturated gray scale on the OLPC's monochrome screen in low power reflective mode. The the brightness detail can be full resolution but the color detail should be lower resolution by about half (not depending on color detail as fine as brightness detail), because of the nature of the OLPC's screen: 200 dots per inch monochrome gray scale, with lower resolution color. For example, text on an icon should be a noticeably different brightness than the background color, instead of relying on color variation, which is not as detailed and won't show up in monochrome mode.

SVG is the idea source format, because it can be scaled to any size, rendered with antialiased edges, and will be easily used in the Python version. Illustrator exports SVG. For now the TCL/Tk version can use versions of the SVG files converted to "XPM" format (which I can do).

Later, the Python user interface will require a bunch more icons and graphics, depending on the user interface and features we decide to support. It would be great to be able to use the same SVG source graphics in the Python version, scaling them as appropriate to adapt to the screen layout. We should revisit the icons and graphics after we port Micropolis to Python.

What follows is a complete list of graphics we could use in the TCL/Tk version. SVG is ideal but PNG is sufficient for this time.

Micropolis icon for use in Sugar user interface, in SVG or PNG.

  * Logos:
    * Micropolis logo
    * Micropolis lettering
    * OLPC Project logo
    * XO Laptop logo
    * Linux Penguin logo
    * Python Programming Language logo

  * Small menu item icons for map overlay menus:
    * Zones
      * All
      * Residential
      * Commercial
      * Industrial
      * Transportation
    * Overlays
      * Population Density
      * Rate of Growth
      * Land Value
      * Crime Rate
      * Pollution Density
      * Traffic Density
      * Power Grid
      * Fire Coverage
      * Police Coverage

  * Small menu item icons for File menu:
    * Save City
    * Save City as...
    * Add Player...
    * Choose City!
    * Quit Playing!

  * Small menu item icons for Options menu:
    * Auto Budget
    * Auto Bulldoze
    * Disasters
    * Sound
    * Animation
    * Messages
    * Notices

  * Small menu item icons for Disasters menu:
    * Monster
    * Fire
    * Flood
    * Meltdown
    * Air Crash
    * Tornado
    * Earthquake

  * Small menu item icons for Time menu:
    * Pause
    * Slow
    * Medium
    * Fast

  * Small menu item icons for Priority menu:
    * Flat Out!
    * Zoom Zoom
    * Whoosh Whoosh
    * Buzz Buzz
    * Putter Putter
    * Drip Drip
    * Snore Snore

  * Small menu item icons for Windows menu (maybe the same as ones used in buttons):
    * Budget
    * Evaluation
    * Graph

  * Small menu item icons for Map Options menu:
    * Auto Goto
    * Palette Panel
    * Chalk Overlay

  * Small "OK" button for confirmation dialog buttons.

  * Small "Cancel" button for confirmation dialog buttons.

  * Large "Question" icon for dialogs asking questions (ok/cancel questions).

  * Large "Notice" icon for notice window (informational).

  * Large "Query" icon for zone query window (zone tricorder).

  * Medium "Generate New City" icon for button to generate new city, on start screen.

  * Medium Game Level icons for start screen radio buttons:
    * Easy
    * Medium
    * Hard

  * Big "Load City" icon for load city dialog.
  * Meduim "Load City" icon for button to load city, on start screen.

  * Big "Save City" icon for save city dialog.
  * Small "Save City" icon for button to save city.

  * Big Evaluation icon for evaluation window.
  * Small Evaluation icon for button to bring up evaluation window.

  * Big Graph icon for graph window.
  * Small Graph icon for button to bring up graph window.

  * Big Simolean/Budget symbol for Budget window.
  * Small Simolean/Budget symbol for button to bring up budget window.

  * Big Quit symbol for Quit confirmation dialog.
  * Small Quit symbol for button to bring up Quit confirmation dialog.

  * Big About symbol for About dialog.
  * Small About symbol for button to open About dialog.

  * Big Choose City symbol for Choose City confirmation dialog.
  * Small Choose City symbol for button to open Choose City confirmation dialog.

## Simulation and User Interface Sounds ##

Right now it's using the original sounds, plus some interface sounds I recorded. I can convert them to the format required by the Linux sound interface we use.

I can supply a list of required interface sounds, but I think we can go with the sounds we have right now without any trouble.

Later, the Python version will require a bunch more sounds for its user interface, which will depend on the user interface features we decide to support. So we should revisit the sound effects later once Micropolis is ported to Python.

To be authentic, I would like to use the original sounds that the simulator made (like the Skywatch One helicopter), or re-record high quality replacements. Maybe we can get Will to record a fresh version of "Skywatch One reporting heavy traffic!" in his own voice (he made that original sound) and the sound people can add in the helicopter sound over his voice to re-create the original sound at high quality. The other sounds could be re-made from scratch without any problem or help from Will.

Voice overs reading the user interface text, notices, help text and documentation would be great for pre-literate children. Would need to be translated to other languages.

## Update Documentation ##

I have translated the Micropolis manual to HTML.

It's the original version that applies to the multi player X11/TCL/Tk version, which needs to be update in a few places where it's specific about the user interface or installation instructions.

It would benefit from some nice reformatting and CSS styles, as well as some illustrations and screen snapshots.

After we port Micropolis to Python, the interface-specific documentation will have to be rewritten, re-illustrated, and then it will have to be translated to different languages.

The documentation consists of the following files:

  * Index
    * Introduction
    * Tutorial
    * User Reference
    * Inside the Simulator
    * History of Cities and City Planning
    * Bibliography
    * Credits

### Help Text ###

There are also HTML files corresponding to user interface help text.  The user interface components are tagged with help ids that are the base names of html files. The existing HTML help files are just placeholders that don't have any useful help text, and there are some help files for user interface components and windows that have since been removed, as well as some help files missing for user interface components and windows that have been added.

To finish implementing help, we need to go through the code and identify all help tags and opportunities for adding missing help tags to the user interface, then make all help files corresponding to the existing tags, delete obsolete help files, and write all the help text. It would be nice for the help text to hsve illustrations of the corresponding windows and components in use.

After we port Micropolis to Python, the help files will have to be re-inventoried, rewritten, re-illustrated, and translated to different languages.

### Courseware ###

We need to include any Micropolis courseware or teacher's guides with the documentation, and update it to relect the current user interface.

After we port Micropolis to Python, we will need to update the courseware to reflect the features and user interface of the new user interface.