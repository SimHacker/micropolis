Cairo Tile Engine
By Don Hopkins

Building with Visual Studio 2008
================================
As of December 17, 2009 the Micropolis solution has been upgraded to use Visual Studio 2008.

The TileEngine requires cairo.lib from the gtk+ toolkit. The easiest way is to grab
the all-in-one bundle from the download homepage:
http://www.gtk.org/download-windows.html

At the time of this writing, the current stable release is 2.18.5. You can download
the all-in-one bundle for this version here (23mb):
http://ftp.gnome.org/pub/gnome/binaries/win32/gtk+/2.18/gtk+-bundle_2.18.5-20091215_win32.zip

Unzip it to c:\gtk as the projects are configured to look in the c:\gtk\lib directories for their
files. If you unzip to a different directory you'll have to modify the project properties
accordingly.

The TileEngine also requires PyCairo, a Python binding for the cairo library. PyCairo is part of
the PyGTK package (Python bindings for GTK+). You can download a copy of PyCairo from here:
http://ftp.gnome.org/pub/GNOME/binaries/win32/pycairo/

Make sure you download a version of PyCairo that matches the version of Python you're using. The
Python version will be part of the PyCairo filename:
pycairo-[cairo version].[platform]-py[python version].exe

Building with Visual Studio 2010
================================
As of April 30, 2012 the Micropolis solution has been upgraded to use Visual Studio 2010.