# Micropolis Source Code Description #

This page describes the several source code releases for Micropolis,
some of which are hosted here (click the Source tab above), others
are hosted elsewhere.

## DiverCity ##

Written in Java (based on MicropolisJ),
this project implements a large number of enhancements
to the basic Micropolis game, including better traffic,
education and culture points, a new road type, schools, museums,
solar plants and more.
Introduced March 2014.
Hosted on Github.
Check it out at https://github.com/Team--Rocket/divercity.

## micropolis-android ##

Written in Java (based on MicropolisJ),
this project brings Micropolis to the Android operating
system. It is still a work-in-progress.
Hosted on Github.
Check it out at https://github.com/jason17055/micropolis-android.

## MicropolisJ (micropolis-java) ##

This is the original Java rewrite of Micropolis.
First released in Feburary 2013, it is functionality-wise almost
equivalent to the TCL/Tk edition,
but the source code is probably more cleanly organized, and all
strings are translatable. Being in Java, it can easily be made to
run on any modern desktop operating system.

The `micropolisj.engine` package contains the guts of the city
simulation. It has no dependencies on the front-end or any particular
graphical system.

The `micropolisj.gui` package provides the user interface for the
game. It renders the city, controls the speed of the simulation, and responds to event messages from the engine.

The Java release was contributed by Jason Long. Email him at jason@long.name with any questions or comments about the Java release, or if you want to contribute translated strings.


## MicropolisCore ##

This is the C++/Python rewrite of Micropolis. The code is organized as several modules: micropolis, cellengine, and tileengine.
  * The micropolis module is the new version of Micropolis, cleaned up and recast as a C++ class.
  * The cellengine module is a cellular automata machine engine.
  * The tileengine module is a Cairo based tile renderer, used to display micropolis tiles and cellengine cells. It plugs into micropolis and cellengine, but is independent of them and useful for other applications.

These modules require Python 2.5 and the Python modules pygtk, pycairo and PIL to be installed. This code will compile on Windows or Linux. It is intended to be used with the OLPC's Sugar user interface environment (depending on Cairo and Pango for graphics and text formatting), but layered so the core code is useful in other contexts such as different scripting languages, web servers, desktop applications, embedded devices, etc.

## micropolis-activity ##

This is the original TCL/Tk edition of Micropolis for Linux systems.
The back-end is entirely in C, and the front-end user interface
is a mixture of C and Tcl.

## turbogears ##

This is the server-side piece of the web-based edition of Micropolis, known as Micropolis Online. It uses the TurboGears web application framework for Python.

## laszlo ##

This is the client-side piece of the web-based edition of Micropolis. It uses OpenLaszlo and Adobe Flash.

## wikimedia ##

This is an OpenLaszlo plugin for MediaWiki, allowing the client-side piece of Micropolis Online to be embedded in a wiki page.

## aws ##

This repository contains some utility scripts and configuration files
helpful for launching a Micropolis Online server on Amazon's EC2
cloud.