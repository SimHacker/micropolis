# Window:  File Chooser, for Unix Micropolis
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
global FileWindows
set n [Unique]
set win .file$n
set FileWindows [linsert $FileWindows 0 $win]

LinkWindow $head.file $win
LinkWindow $win.head $head

catch "destroy $win"
toplevel $win -screen $display

SetHelp $win File

wm title $win {File Chooser}
wm iconname $win {File Chooser}
wm geometry $win 350x480+5+5
wm positionfrom $win user
wm sizefrom $win user
wm maxsize $win 700 700
wm minsize $win 100 100
wm protocol $win delete "DeleteWindow file FileWindows"
wm transient $win $head

# Based on xf file selection box code by:
#   garfield@cs.tu-berlin.de
#   svoboda@transam.ece.cmu.edu (David Svoboda)

message $win.message1 \
  -aspect 1500 \
  -anchor n \
  -justify center \
  -relief flat \
  -font [Font $win Large] \
  -text ""

frame $win.frame1 \
  -borderwidth 0

button $win.frame1.ok -text {OK} -font [Font $win Large]

SetHelp $win.frame1.ok File.OK

button $win.frame1.rescan -text {Rescan} -font [Font $win Large]

SetHelp $win.frame1.rescan File.Rescan

button $win.frame1.cancel -text {Cancel} -font [Font $win Large]

SetHelp $win.frame1.cancel File.Cancel

frame $win.path \
  -borderwidth 2 \
  -relief flat

SetHelp $win.path File.Directory

label $win.path.pathlabel -text {Directory:} -font [Font $win Large]
entry $win.path.path -relief sunken -font [Font $win Large]

frame $win.files \
  -borderwidth 2 \
  -relief flat

SetHelp $win.files File.List

scrollbar $win.files.vscroll \
  -command "$win.files.files yview"

scrollbar $win.files.hscroll \
  -orient horizontal \
  -command "$win.files.files xview"

listbox $win.files.files \
  -exportselection false \
  -font [Font $win Large] \
  -xscrollcommand "$win.files.hscroll set" \
  -yscrollcommand "$win.files.vscroll set"

pack append $win.files \
	    $win.files.vscroll {left filly} \
	    $win.files.hscroll {bottom fillx} \
	    $win.files.files {left fill expand}

frame $win.file \
  -borderwidth 2 \
  -relief flat

SetHelp $win.file File.File

label $win.file.labelfile \
  -relief flat \
  -font [Font $win Large] \
  -text "File:"

entry $win.file.file \
  -font [Font $win Large] \
  -relief sunken

pack append $win.file \
	    $win.file.labelfile {left} \
	    $win.file.file {left fill expand}

bind $win.files.files <ButtonPress-1> "FileSelect $win %W %y"
bind $win.files.files <Button1-Motion> "FileSelect $win %W %y"
bind $win.files.files <Shift-Button1-Motion> "FileSelect $win %W %y"
bind $win.files.files <Shift-ButtonPress-1> "FileSelect $win %W %y"
bind $win.path.path <Tab> "NameComplete $win path"
catch "bind $win.path.path <Up> {}"
bind $win.path.path <Down> "
  $win.file.file cursor 0
  focus $win.file.file"

bind $win.file.file <Tab> "NameComplete $win file"
bind $win.file.file <Up> "
  $win.path.path cursor 0
  focus $win.path.path"
catch "bind $win.path.path <Down> {}"

pack append $win.frame1 \
	    $win.frame1.ok {left fill expand} \
	    $win.frame1.rescan {left fill expand} \
	    $win.frame1.cancel {left fill expand}
pack append $win.path \
	    $win.path.pathlabel {left} \
	    $win.path.path {left fill expand}
pack append $win \
	    $win.message1 {top fill} \
	    $win.frame1 {bottom fill} \
	    $win.file {bottom fill} \
	    $win.path {bottom fill} \
	    $win.files {left fill expand}

update idletasks
