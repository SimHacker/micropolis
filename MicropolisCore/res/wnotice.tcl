# Window: Notices, for Unix Micropolis.
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

global NoticeWindows
set n [Unique]
set parent [WindowLink $head.w3]
set win $parent.notice$n
set NoticeWindows [linsert $NoticeWindows 0 $win]

LinkWindow $head.notice $win
LinkWindow $win.head $head
LinkWindow $win.parent $parent

catch "destroy $win"
frame $win

SetHelp $win Notice

bind $win <Visibility> {[WindowLink %W.view] Visible [string compare %s FullyObscured]}
bind $win <Map> {[WindowLink %W.view] Visible 1}
bind $win <Unmap> {[WindowLink %W.view] Visible 0}

frame $win.top \
  -relief raised \
  -borderwidth 1
LinkWindow $win.background $win.top

label $win.top.title \
  -borderwidth 2 \
  -relief raised \
  -font [Font $win Big]
LinkWindow $win.title $win.top.title

SetHelp $win.top.title Notice.Title

text $win.top.text \
  -borderwidth 2 \
  -relief flat \
  -wrap word \
  -state disabled \
  -font [Font $win Large]
LinkWindow $win.text $win.top.text

SetHelp $win.top.text Notice.Text

global ResourceDir

button $win.top.text.left \
  -command "UIMakeSound fancy HeavyTraffic" \
  -font [Font $win Large]\
  -borderwidth 1 \
  -relief flat
LinkWindow $win.left $win.top.text.left

button $win.top.text.middle \
  -command "UIMakeSound fancy Computer" \
  -font [Font $win Large]\
  -borderwidth 1 \
  -relief flat
LinkWindow $win.middle $win.top.text.middle

button $win.top.text.right \
  -command "UIMakeSound fancy QuackQuack" \
  -font [Font $win Large]\
  -borderwidth 1 \
  -relief flat
LinkWindow $win.right $win.top.text.right

frame $win.top.viewframe\
  -borderwidth 4\
  -relief flat
LinkWindow $win.viewframe $win.top.viewframe

SetHelp $win.top.viewframe Notice.View

frame $win.top.viewframe.f\
  -cursor hand2\
  -borderwidth 1\
  -relief sunken

pack append $win.top.viewframe\
  $win.top.viewframe.f \
      {top frame center expand fill}

editorview $win.top.viewframe.f.view \
  -font [Font $win Big]\
  -width 128 -height 32
LinkWindow $win.view $win.top.viewframe.f.view
$win.top.viewframe.f.view ShowMe 0
bind $win.top.viewframe.f.view <ButtonPress> \
      "ComeToMe %W"

pack append $win.top.viewframe.f\
  $win.top.viewframe.f.view \
      {top frame center expand fill}

pack append $win.top\
  $win.top.title	{top frame center fillx} \
  $win.top.text		{right frame center expand fill}

button $win.dismiss\
  -font [Font $win Large]\
  -text {Dismiss}\
  -command "pack unpack $win"

SetHelp $win.dismiss Notice.Dismiss

pack append $win\
  $win.dismiss		{bottom frame center fillx} \
  $win.top		{left frame center expand fill}

update idletasks
