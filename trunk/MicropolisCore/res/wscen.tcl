# Window: Scenarios, for Unix Micropolis.
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
global ScenarioWindows
set n [Unique]
set win .scenario$n

LinkWindow $head.scenario $win
LinkWindow $win.head $head
LinkWindow $win.scenarioTarget -1
LinkWindow $win.scenarioTargetDown -1

catch "destroy $win"
if {[catch "toplevel $win -screen $display"]} {
  puts stderr "Couldn't open X11 display \"$display\"."
  set win ""
  return ""
}

wm title $win "Micropolis Scenarios"
wm iconname $win {Micropolis Scenarios}
wm geometry $win 1200x900+0+0
wm withdraw $win
wm protocol $win delete "DeleteScenarioWindow $win ;"
wm fullscreen $win on

set ScenarioWindows [linsert $ScenarioWindows 0 $win]

SetHelp $win Scenario

canvas $win.canvas \
  -scrollincrement 0 \
  -borderwidth 0 \
  -background #000000 \
  -width 1200 -height 900
LinkWindow $win.canvas $win.canvas
LinkWindow $win.canvas.w $win

$win.canvas create bitmap 0 0 \
  -tags background  \
  -bitmap "@images/background-micropolis.xpm" \
  -anchor nw

$win.canvas bind background <ButtonPress> {HandleScenarioDown %W %x %y}
$win.canvas bind background <ButtonRelease-1> {HandleScenarioUp %W %x %y}
$win.canvas bind background <ButtonRelease-2> {HandleScenarioUp %W %x %y}
$win.canvas bind background <ButtonRelease-3> {HandleScenarioUp %W %x %y}
$win.canvas bind background <Motion> {HandleScenarioMove %W %x %y}
$win.canvas bind background <Button1-Motion> {HandleScenarioMove %W %x %y}
$win.canvas bind background <Button2-Motion> {HandleScenarioMove %W %x %y}
$win.canvas bind background <Button3-Motion> {HandleScenarioMove %W %x %y}

global ScenarioButtons
set i 0
set len [llength $ScenarioButtons]
while {$i < $len} {
  set data [lindex $ScenarioButtons $i]
  set type [lindex $data 0]
  set id [lindex $data 1]
  set xx [lindex $data 5]
  set yy [lindex $data 6]
  set ww [lindex $data 7]
  set hh [lindex $data 8]
  
  $win.canvas create bitmap $xx $yy \
    -tags $id \
    -anchor nw

  $win.canvas bind $id <ButtonPress> {HandleScenarioDown %W %x %y}
  $win.canvas bind $id <ButtonRelease-1> {HandleScenarioUp %W %x %y}
  $win.canvas bind $id <ButtonRelease-2> {HandleScenarioUp %W %x %y}
  $win.canvas bind $id <ButtonRelease-3> {HandleScenarioUp %W %x %y}
  $win.canvas bind $id <Motion> {HandleScenarioMove %W %x %y}
  $win.canvas bind $id <Button1-Motion> {HandleScenarioMove %W %x %y}
  $win.canvas bind $id <Button2-Motion> {HandleScenarioMove %W %x %y}
  $win.canvas bind $id <Button3-Motion> {HandleScenarioMove %W %x %y}

  set i [expr "$i + 1"]
}

mapview $win.canvas.view \
  -font [Font $win Large]
LinkWindow $win.view $win.canvas.view
$win.canvas.view size [expr "[sim WorldX] * 3"] [expr "[sim WorldY] * 3"]

# Hide the editor rectangles.
$win.canvas.view ShowEditors 0

frame $win.canvas.name \
  -borderwidth 2\
  -relief flat

SetHelp $win.name Scenario.Name

label $win.canvas.name.namelabel \
  -text {City Name:} \
  -font [Font $win Text]

entry $win.canvas.name.cityname \
  -text {}\
  -textvariable CityName \
  -font [Font $win Text] \
  -width 33
bind $win.canvas.name.cityname <Return> "DoEnterCityName $win"
bind $win.canvas.name.cityname <Any-Enter> {focus %W}
LinkWindow $win.cityname $win.canvas.name.cityname

pack append $win.canvas.name \
  $win.canvas.name.namelabel	{left frame e}\
  $win.canvas.name.cityname	{left frame e}

place $win.canvas -x 0 -y 0
place $win.canvas.view -x 534 -y 48
place $win.canvas.name -x 530 -y 0

InitScenario $win

update idletasks
