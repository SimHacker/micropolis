# Window: Player, for Unix Micropolis.
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
#
global PlayerWindows
set n [Unique]
set win .player$n
set PlayerWindows [linsert $PlayerWindows 0 $win]

LinkWindow $head.player $win
LinkWindow $win.head $head

catch "destroy $win"
toplevel $win -screen $display

SetHelp $win Player

wm title $win {Micropolis Players}
wm iconname $win {Micropolis Players}
wm geometry $win 400x250+5+5
wm positionfrom $win user
wm sizefrom $win user
wm maxsize $win 700 700
wm minsize $win 100 100
wm protocol $win delete "DeleteWindow player PlayerWindows"
wm transient $win $head

frame $win.top \
  -relief raised \
  -borderwidth 1
LinkWindow $win.background $win.top

label $win.top.label \
  -text "X11 Servers of Micropolis Players" \
  -font [Font $win Large]

text $win.top.text \
  -borderwidth 2 \
  -relief flat \
  -wrap word \
  -state normal \
  -height 4 \
  -font [Font $win Medium]
LinkWindow $win.text $win.top.text
$win.top.text delete 0.0 end
$win.top.text insert end {\
Micropolis is connected to the X11 servers listed above.
To add another player to the game, enter their display name.
If you don't have a "Multi Player" license, you can just \
add more players on your own display, by entering ":0".}

$win.top.text configure -state disabled

frame $win.top.f \
  -relief flat \
  -borderwidth 1

SetHelp $win.top.f Player.Display

button $win.top.f.add \
  -text "Add Player on X11 Display:" \
  -borderwidth 1 \
  -font [Font $win Large] \
  -command "DoNewPlayer $win"

entry $win.top.f.display \
  -relief sunken \
  -font [Font $win Large] \
  -text ""
bind $win.top.f.display <Return> "DoNewPlayer $win"
bind $win.top.f.display <Any-Enter> {focus %W}
LinkWindow $win.display $win.top.f.display

pack append $win.top.f \
  $win.top.f.add	{left frame nw padx 4} \
  $win.top.f.display	{left frame nw expand fillx padx 4}

frame $win.top.g \
  -relief flat \
  -borderwidth 1

SetHelp $win.top.g Player.Players

scrollbar $win.top.g.scroll \
  -borderwidth 1 \
  -command "$win.top.g.list yview"

listbox $win.top.g.list \
  -relief sunken \
  -borderwidth 1 \
  -font [Font $win Large] \
  -yscroll "$win.top.g.scroll set"
LinkWindow $win.players $win.top.g.list

pack append $win.top.g \
  $win.top.g.scroll {left filly frame e} \
  $win.top.g.list {right expand fill frame w}

pack append $win.top \
  $win.top.label	{top frame center fillx} \
  $win.top.g		{top frame center expand fill} \
  $win.top.text		{top frame center fill} \
  $win.top.f		{top frame center fillx}

button $win.dismiss\
  -font [Font $win Large]\
  -borderwidth 1 \
  -text {Dismiss}\
  -command "wm withdraw $win"

SetHelp $win.dismiss Player.Dismiss

pack append $win\
  $win.top		{top frame center expand fill} \
  $win.dismiss		{bottom frame center fillx}

UpdatePlayers

update idletasks
