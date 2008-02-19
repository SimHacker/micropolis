How to compile MicropolisCore on Mac OS/X:

Install MacPorts:


Install required packages:

port install cairo pango gtk2 python25 py25-cairo py25-gobject py25-gtk py25-numeric

Make sure the MacPorts environment is set up in your .profile:

export PATH=~/bin:/opt/local/bin:/opt/local/sbin:$PATH
export PKG_CONFIG_PATH=${PKG_CONFIG_PATH}:/opt/local/lib/pkgconfig:/usr/local/lib/pkgconfig

