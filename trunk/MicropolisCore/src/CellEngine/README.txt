Cellular Automata Machine Engine
By Don Hopkins

Building with Visual Studio 2008
================================
As of December 17, 2009 the Micropolis solution has been upgraded to use Visual Studio 2008.

The CellEngine requires cairo.lib from the gtk+ toolkit. The easiest way is to grab
the all-in-one bundle from the download homepage:
http://www.gtk.org/download-windows.html

At the time of this writing, the current stable release is 2.18.5. You can download
the all-in-one bundle for this version here (23mb):
http://ftp.gnome.org/pub/gnome/binaries/win32/gtk+/2.18/gtk+-bundle_2.18.5-20091215_win32.zip

Unzip it to c:\gtk as the projects are configured to look in the c:\gtk\lib directories for their
files. If you unzip to a different directory you'll have to modify the project properties
accordingly.

Building with Visual Studio 2010
================================
As of April 30, 2012 the Micropolis solution has been upgraded to use Visual Studio 2010.