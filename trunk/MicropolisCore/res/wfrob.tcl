# Window: Dynamic Zone Filter, examining traffic, density, etc, for Unix Micropolis
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
global FrobWindows
set n [Unique]
set win .frob$n
set FrobWindows [linsert $FrobWindows 0 $win]

LinkWindow $head.frob $win
LinkWindow $head.frob $win
LinkWindow $win.head $head

catch "destroy $win"
toplevel $win -screen $display

SetHelp $win Frob

wm title $win {Micropolis Dynamic Zone Filter}
wm iconname $win {Micropolis Frob}
wm geometry $win 350x480+5+5
wm positionfrom $win user
wm sizefrom $win user
wm maxsize $win 700 700
wm minsize $win 100 100
wm protocol $win delete "DeleteWindow frob FrobWindows"
wm transient $win $head

frame $win.top \
  -relief raised \
  -borderwidth 1
LinkWindow $win.background $win.top

label $win.top.title \
  -text {Frob-O-Matic Dynamic Zone Filter} \
  -borderwidth 1 \
  -relief raised \
  -font [Font $win Big]
LinkWindow $win.title $win.top.title

#  SetHelp $win.top.title Frob.Title

frame $win.top.border \
  -relief flat \
  -borderwidth 4

label $win.top.border.l0 \
  -text {Population Density} \
  -borderwidth 1 \
  -font [Font $win Small]

SetHelp $win.top.border.l0 Frob.PopulationDensity

interval $win.top.border.i0\
      -font [Font $win Medium]\
      -borderwidth 1 \
      -orient horizontal \
      -from 0 -to 255 \
      -command "UISetDynamic 0"

SetHelp $win.top.border.i0 Frob.PopulationDensity

label $win.top.border.l1 \
  -text {Rate Of Growth} \
  -borderwidth 2 \
  -font [Font $win Small]

SetHelp $win.top.border.l0 Frob.RateOfGrowth

interval $win.top.border.i1\
      -font [Font $win Medium]\
      -borderwidth 1 \
      -orient horizontal \
      -from 0 -to 255 \
      -command "UISetDynamic 1"

SetHelp $win.top.border.i0 Frob.RateOfGrowth

label $win.top.border.l2 \
  -text {Traffic Density} \
  -borderwidth 2 \
  -font [Font $win Small]

SetHelp $win.top.border.l0 Frob.TrafficDensity

interval $win.top.border.i2\
      -font [Font $win Medium]\
      -borderwidth 1 \
      -orient horizontal \
      -from 0 -to 255 \
      -command "UISetDynamic 2"

SetHelp $win.top.border.i0 Frob.TrafficDensity

label $win.top.border.l3 \
  -text {Pollution Rate} \
  -borderwidth 2 \
  -font [Font $win Small]

SetHelp $win.top.border.l0 Frob.PollutionRate

interval $win.top.border.i3\
      -font [Font $win Medium]\
      -borderwidth 1 \
      -orient horizontal \
      -from 0 -to 255 \
      -command "UISetDynamic 3"

SetHelp $win.top.border.i0 Frob.PollutionRate

label $win.top.border.l4 \
  -text {Crime Rate} \
  -borderwidth 2 \
  -font [Font $win Small]

SetHelp $win.top.border.l0 Frob.CrimeRate

interval $win.top.border.i4\
      -font [Font $win Medium]\
      -borderwidth 1 \
      -orient horizontal \
      -from 0 -to 255 \
      -command "UISetDynamic 4"

SetHelp $win.top.border.i0 Frob.CrimeRate

label $win.top.border.l5 \
  -text {Land Value} \
  -borderwidth 2 \
  -font [Font $win Small]

SetHelp $win.top.border.l0 Frob.LandValue

interval $win.top.border.i5\
      -font [Font $win Medium]\
      -borderwidth 1 \
      -orient horizontal \
      -from 0 -to 255 \
      -command "UISetDynamic 5"

SetHelp $win.top.border.i0 Frob.LandValue

label $win.top.border.l6 \
  -text {Police Effect} \
  -borderwidth 2 \
  -font [Font $win Small]

SetHelp $win.top.border.l0 Frob.PoliceEffect

interval $win.top.border.i6\
      -font [Font $win Medium]\
      -borderwidth 1 \
      -orient horizontal \
      -from 0 -to 255 \
      -command "UISetDynamic 6"

SetHelp $win.top.border.i0 Frob.PoliceEffect

label $win.top.border.l7 \
  -text {Fire Effect} \
  -borderwidth 2 \
  -font [Font $win Small]

SetHelp $win.top.border.l0 Frob.FireEffect

interval $win.top.border.i7\
      -font [Font $win Medium]\
      -borderwidth 1 \
      -orient horizontal \
      -from 0 -to 255 \
      -command "UISetDynamic 7"

SetHelp $win.top.border.i0 Frob.FireEffect

pack append $win.top.border\
  $win.top.border.l0		{top frame center fillx} \
  $win.top.border.i0		{top frame center fillx} \
  $win.top.border.l1		{top frame center fillx} \
  $win.top.border.i1		{top frame center fillx} \
  $win.top.border.l2		{top frame center fillx} \
  $win.top.border.i2		{top frame center fillx} \
  $win.top.border.l3		{top frame center fillx} \
  $win.top.border.i3		{top frame center fillx} \
  $win.top.border.l4		{top frame center fillx} \
  $win.top.border.i4		{top frame center fillx} \
  $win.top.border.l5		{top frame center fillx} \
  $win.top.border.i5		{top frame center fillx} \
  $win.top.border.l6		{top frame center fillx} \
  $win.top.border.i6		{top frame center fillx} \
  $win.top.border.l7		{top frame center fillx} \
  $win.top.border.i7		{top frame center fillx}

pack append $win.top\
  $win.top.title		{top frame center fillx} \
  $win.top.border		{top frame center fill}

button $win.dismiss\
  -font [Font $win Large]\
  -text {Dismiss}\
  -command "wm withdraw $win"

SetHelp $win.dismiss Frob.Dismiss

pack append $win\
  $win.dismiss	{bottom frame center fillx}\
  $win.top		{left frame center expand fill}

update idletasks
