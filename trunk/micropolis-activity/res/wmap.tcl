# Windows: Maps, for Unix Micropolis.
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
global MapWindows
set n [Unique]
set parent [WindowLink $head.w2]
set win $parent.map$n
set MapWindows [linsert $MapWindows 0 $win]

global MapState.$win
set MapState.$win 0

LinkWindow $head.map $win
LinkWindow $win.head $head
LinkWindow $win.parent $parent
LinkWindow $win.top $win

catch "destroy $win"
frame $win

SetHelp $win Map

bind $win <Visibility> {[WindowLink %W.view] Visible [string compare %s FullyObscured]}
bind $win <Map> {[WindowLink %W.view] Visible 1}
bind $win <Unmap> {[WindowLink %W.view] Visible 0}

frame $win.centerframe\
  -borderwidth 1\
  -relief sunken

SetHelp $win.centerframe Map.View

mapview $win.centerframe.view\
  -font [Font $win Large]
$win.centerframe.view size [expr "[sim WorldX] * 3"] [expr "[sim WorldY] * 3"]

LinkWindow $win.view $win.centerframe.view
LinkWindow $win.centerframe.view.pallet $win.topframe
LinkWindow $win.centerframe.view.win $win

bind $win.centerframe.view <1> {MapPanDown %W %x %y}
bind $win.centerframe.view <B1-Motion> {MapPanDrag %W %x %y}
bind $win.centerframe.view <ButtonRelease-1> {MapPanUp %W %x %y}

bind $win.centerframe.view <2> {MapPanDown %W %x %y}
bind $win.centerframe.view <B2-Motion> {MapPanDrag %W %x %y}
bind $win.centerframe.view <ButtonRelease-2> {MapPanUp %W %x %y}

bind $win.centerframe.view <3> {MapPanDown %W %x %y}
bind $win.centerframe.view <B3-Motion> {MapPanDrag %W %x %y}
bind $win.centerframe.view <ButtonRelease-3> {MapPanUp %W %x %y}

pack append $win.centerframe\
  $win.centerframe.view {top frame center expand}

frame $win.topframe\
  -borderwidth 1\
  -relief raised

menubutton $win.topframe.zones\
  -menu $win.topframe.zones.m\
  -text {Zones}\
  -font [Font $win Medium]\
  -variable $win.postedMenu\
  -borderwidth 1\
  -relief flat
LinkWindow $win.zones $win.topframe.zones
tk_bindForTraversal $win.topframe.zones
bind $win.topframe.zones <F10> {tk_firstMenu %W} 
bind $win.topframe.zones <Mod2-Key> {tk_traverseToMenu %W %A} 
tk_menus $win $win.topframe.zones

SetHelp $win.topframe.zones Map.Zones

menu $win.topframe.zones.m\
  -font [Font $win Medium]
tk_bindForTraversal $win.topframe.zones.m
bind $win.topframe.zones.m <F10> {tk_firstMenu %W} 
bind $win.topframe.zones.m <Mod2-Key> {tk_traverseToMenu %W %A} 
  $win.topframe.zones.m add radiobutton\
    -label {All}\
    -variable MapState.$win\
    -value 0\
    -command "SetMapState $win 0"
  $win.topframe.zones.m add radiobutton\
    -label {Residential}\
    -variable MapState.$win\
    -value 1\
    -command "SetMapState $win 1"
  $win.topframe.zones.m add radiobutton\
    -label {Commercial}\
    -variable MapState.$win\
    -value 2\
    -command "SetMapState $win 2"
  $win.topframe.zones.m add radiobutton\
    -label {Industrial}\
    -variable MapState.$win\
    -value 3\
    -command "SetMapState $win 3"
  $win.topframe.zones.m add radiobutton\
    -label {Transportation}\
    -variable MapState.$win\
    -value 5\
    -command "SetMapState $win 5"
  # Disabled dynamic filter for now.
  #$win.topframe.zones.m add radiobutton\
  #  -label {Dynamic}\
  #  -variable MapState.$win\
  #  -value 14\
  #  -command "SetMapState $win 14"

menubutton $win.topframe.overlays\
  -menu $win.topframe.overlays.m\
  -text {Overlays}\
  -font [Font $win Medium]\
  -variable $win.postedMenu\
  -borderwidth 1\
  -relief flat
LinkWindow $win.overlays $win.topframe.overlays
tk_bindForTraversal $win.topframe.overlays
bind $win.topframe.overlays <F10> {tk_firstMenu %W} 
bind $win.topframe.overlays <Mod2-Key> {tk_traverseToMenu %W %A} 
tk_menus $win $win.topframe.overlays

SetHelp $win.topframe.overlays Map.Overlays

menu $win.topframe.overlays.m\
  -font [Font $win Medium]
tk_bindForTraversal $win.topframe.overlays.m
bind $win.topframe.overlays.m <F10> {tk_firstMenu %W} 
bind $win.topframe.overlays.m <Mod2-Key> {tk_traverseToMenu %W %A} 
  $win.topframe.overlays.m add radiobutton\
    -label {Population Density}\
    -variable MapState.$win\
    -value 6\
    -command "SetMapState $win 6"
  $win.topframe.overlays.m add radiobutton\
    -label {Rate of Growth}\
    -variable MapState.$win\
    -value 7\
    -command "SetMapState $win 7"
  $win.topframe.overlays.m add radiobutton\
    -label {Land Value}\
    -variable MapState.$win\
    -value 11\
    -command "SetMapState $win 11"
  $win.topframe.overlays.m add radiobutton\
    -label {Crime Rate}\
    -variable MapState.$win\
    -value 10\
    -command "SetMapState $win 10"
  $win.topframe.overlays.m add radiobutton\
    -label {Pollution Density}\
    -variable MapState.$win\
    -value 9\
    -command "SetMapState $win 9"
  $win.topframe.overlays.m add radiobutton\
    -label {Traffic Density}\
    -variable MapState.$win\
    -value 8\
    -command "SetMapState $win 8"
  $win.topframe.overlays.m add radiobutton\
    -label {Power Grid}\
    -variable MapState.$win\
    -value 4\
    -command "SetMapState $win 4"
  $win.topframe.overlays.m add radiobutton\
    -label {Fire Coverage}\
    -variable MapState.$win\
    -value 12\
    -command "SetMapState $win 12"
  $win.topframe.overlays.m add radiobutton\
    -label {Police Coverage}\
    -variable MapState.$win\
    -value 13\
    -command "SetMapState $win 13"

label $win.topframe.legend\
  -bitmap "@images/legendn.xpm" \
  -font [Font $win Medium]
LinkWindow $win.legend $win.topframe.legend

pack append $win.topframe\
  $win.topframe.zones		{left frame nw} \
  $win.topframe.overlays	{left frame nw} \
  $win.topframe.legend		{right frame ne}

pack append $win\
  $win.topframe			{top frame center fillx} \
  $win.centerframe		{top frame center fill}

pack append $parent\
  $win {top frame nw expand fill}

InitMap $win

update idletasks
