# Window: Query.  Ask the player something.  For Unix Micropolis
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

global AskWindows
set n [Unique]
set win .ask$n
set AskWindows [linsert $AskWindows 0 $win]

LinkWindow $head.ask $win
LinkWindow $win.head $head

catch "destroy $win"
toplevel $win -screen $display

SetHelp $win Ask

wm title $win {Micropolis Query}
wm iconname $win {Micropolis Query}
wm geometry $win 350x200+5+5
wm positionfrom $win user
wm sizefrom $win user
wm maxsize $win 700 700
wm minsize $win 100 100
wm protocol $win delete "DeleteWindow ask AskWindows"
wm transient $win $head

frame $win.top \
  -relief raised \
  -borderwidth 1
LinkWindow $win.background $win.top

label $win.top.title \
  -borderwidth 2 \
  -relief raised \
  -font [Font $win Big]
LinkWindow $win.title $win.top.title

text $win.top.text \
  -borderwidth 2 \
  -relief flat \
  -wrap word \
  -state disabled \
  -font [Font $win Big]
LinkWindow $win.text $win.top.text

frame $win.top.frame \
  -borderwidth 2 \
  -relief flat
LinkWindow $win.frame $win.top.frame

button $win.top.frame.left \
  -font [Font $win Large]\
  -borderwidth 1\
  -padx 6 -pady 12
LinkWindow $win.left $win.top.frame.left

button $win.top.frame.middle \
  -font [Font $win Large]\
  -borderwidth 1\
  -padx 6 -pady 12
LinkWindow $win.middle $win.top.frame.middle

frame $win.top.frame.rightframe \
  -borderwidth 0 \
  -relief raised
LinkWindow $win.voteframe $win.top.frame.rightframe

button $win.top.frame.rightframe.right \
  -font [Font $win Large]\
  -relief raised \
  -borderwidth 1
LinkWindow $win.vote $win.top.frame.rightframe.right

pack append $win.top.frame.rightframe\
  $win.top.frame.rightframe.right	{top frame center}

BindVotingButton $win $win.top.frame.rightframe.right Ask

pack append $win.top\
  $win.top.title	{top frame center fillx} \
  $win.top.text		{top frame center expand fill} \
  $win.top.frame	{bottom frame center fillx}

pack append $win\
  $win.top		{left frame center expand fill}

update idletasks
