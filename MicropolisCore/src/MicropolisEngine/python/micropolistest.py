# micropolistest.py
#
# Micropolis, Unix Version.  This game was released for the Unix platform
# in or about 1990 and has been modified for inclusion in the One Laptop
# Per Child program.  Copyright (C) 1989 - 2007 Electronic Arts Inc.  If
# you need assistance with this program, you may contact:
#   http://wiki.laptop.org/go/Micropolis  or email  micropolis@laptop.org.
# 
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or (at
# your option) any later version.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# General Public License for more details.  You should have received a
# copy of the GNU General Public License along with this program.  If
# not, see <http://www.gnu.org/licenses/>.
# 
#             ADDITIONAL TERMS per GNU GPL Section 7
# 
# No trademark or publicity rights are granted.  This license does NOT
# give you any right, title or interest in the trademark SimCity or any
# other Electronic Arts trademark.  You may not distribute any
# modification of this program using the trademark SimCity or claim any
# affliation or association with Electronic Arts Inc. or its employees.
# 
# Any propagation or conveyance of this program must include this
# copyright notice and these terms.
# 
# If you convey this program (or any modifications of it) and assume
# contractual liability for the program to recipients of it, you agree
# to indemnify Electronic Arts for any liability that those contractual
# assumptions impose on Electronic Arts.
# 
# You may not misrepresent the origins of this program; modified
# versions of the program must be marked as such and not identified as
# the original program.
# 
# This disclaimer supplements the one included in the General Public
# License.  TO THE FULLEST EXTENT PERMISSIBLE UNDER APPLICABLE LAW, THIS
# PROGRAM IS PROVIDED TO YOU "AS IS," WITH ALL FAULTS, WITHOUT WARRANTY
# OF ANY KIND, AND YOUR USE IS AT YOUR SOLE RISK.  THE ENTIRE RISK OF
# SATISFACTORY QUALITY AND PERFORMANCE RESIDES WITH YOU.  ELECTRONIC ARTS
# DISCLAIMS ANY AND ALL EXPRESS, IMPLIED OR STATUTORY WARRANTIES,
# INCLUDING IMPLIED WARRANTIES OF MERCHANTABILITY, SATISFACTORY QUALITY,
# FITNESS FOR A PARTICULAR PURPOSE, NONINFRINGEMENT OF THIRD PARTY
# RIGHTS, AND WARRANTIES (IF ANY) ARISING FROM A COURSE OF DEALING,
# USAGE, OR TRADE PRACTICE.  ELECTRONIC ARTS DOES NOT WARRANT AGAINST
# INTERFERENCE WITH YOUR ENJOYMENT OF THE PROGRAM; THAT THE PROGRAM WILL
# MEET YOUR REQUIREMENTS; THAT OPERATION OF THE PROGRAM WILL BE
# UNINTERRUPTED OR ERROR-FREE, OR THAT THE PROGRAM WILL BE COMPATIBLE
# WITH THIRD PARTY SOFTWARE OR THAT ANY ERRORS IN THE PROGRAM WILL BE
# CORRECTED.  NO ORAL OR WRITTEN ADVICE PROVIDED BY ELECTRONIC ARTS OR
# ANY AUTHORIZED REPRESENTATIVE SHALL CREATE A WARRANTY.  SOME
# JURISDICTIONS DO NOT ALLOW THE EXCLUSION OF OR LIMITATIONS ON IMPLIED
# WARRANTIES OR THE LIMITATIONS ON THE APPLICABLE STATUTORY RIGHTS OF A
# CONSUMER, SO SOME OR ALL OF THE ABOVE EXCLUSIONS AND LIMITATIONS MAY
# NOT APPLY TO YOU.


########################################################################
# Micropolis Test
# Don Hopkins


########################################################################
# Import standard Python modules.


import sys, os, random
import gtk


########################################################################
# Set up the path for development, to load the latest compiled code 
# from the build directory and the fresh python code from its place 
# in the source tree.


#print "CWD", os.getcwd()

cwd = os.getcwd()

for relPath in (
  'ReleaseSymbols',
  'build/lib.macosx-10.5-i386-2.5',
  '../../TileEngine/python/ReleaseSymbols',
  '../../TileEngine/python/build/lib.macosx-10.5-i386-2.5',
  '../../TileEngine/python',
  '.',
):
    sys.path.insert(0, os.path.join(cwd, relPath))


########################################################################
# Import our modules


import micropolisengine
import micropolismodel
import micropoliswindow
import micropolisdrawingarea


########################################################################


if __name__ == '__main__':

    engine = micropolismodel.CreateTestEngine()

    engine.cityTax = 6
    #engine.setSkips(500)
    engine.setSkips(10)
    setTile = engine.setTile

    for y in range(0, micropolisengine.WORLD_H):
        for x in range(0, micropolisengine.WORLD_W):
            setTile(x, y, micropolisengine.RUBBLE | micropolisengine.BLBNBIT)

    for y in range(10, 15):
        for x in range(10, 15):
            setTile(x, y, micropolisengine.FIRE | micropolisengine.ANIMBIT)

    fudge = 0
    width = int((120 * 4) + fudge)
    height = int((100 * 4) + fudge)

    w = width
    h = height

    x1 = 0
    y1 = 0
    x2 = w + 20
    y2 = h + 40

    if True:
        win2 = micropoliswindow.MicropolisPanedWindow(engine=engine)
        win2.set_default_size(800, 800)
        win2.move(x1, y1)
        win2.show_all()
        win2.tileView.setScale(1.0)
        win2.tileView.panTo(-200, -200)

    if False:
        win2 = micropoliswindow.MicropolisWindow(engine=engine)
        win2.set_default_size(w, h)
        win2.move(x1, y1)
        win2.show_all()
        win2.da.setScale(1.0)

    if False:
        win1 = micropoliswindow.MicropolisWindow(
            engine=engine, 
            tileViewClass=micropolisdrawingarea.MiniMicropolisDrawingArea)
        win1.set_default_size(width, height)
        win1.move(x2, y1)
        win1.show_all()

    if False:
        win3 = micropoliswindow.MicropolisWindow(engine=engine)
        win3.set_default_size(w, h)
        win3.move(x1, y2)
        win3.show_all()
        win3.da.setScale(2.0)

    if False:
        win4 = micropoliswindow.MicropolisWindow(engine=engine)
        win4.set_default_size(w, h)
        win4.move(x2, y2)
        win4.show_all()
        win4.da.setScale(4.0)

    gtk.main()


########################################################################
