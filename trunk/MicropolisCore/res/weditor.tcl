# Window: Editor, for Unix Micropolis.
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
global EditorWindows
set n [Unique]
set parent [WindowLink $head.col2]
set win $parent.editor$n
set EditorWindows [linsert $EditorWindows 0 $win] 

global Skip.$win
set Skip.$win 0
global AutoGoto.$win
set AutoGoto.$win 0
global Controls.$win
set Controls.$win 1
global Overlay.$win
set Overlay.$win 0
global DynamicFilter.$win
set DynamicFilter.$win 0

LinkWindow $head.editor $win
LinkWindow $win.head $head
LinkWindow $win.parent $parent
LinkWindow $win.top $win

catch "destroy $win"
frame $win

SetHelp $win Editor

bind $win <Visibility> {[WindowLink %W.view] Visible [string compare %s FullyObscured]}
bind $win <Map> {[WindowLink %W.view] Visible 1}
bind $win <Unmap> {[WindowLink %W.view] Visible 0}
# $head.editor points to most recently created or entered editor
bind $win <Enter> "LinkWindow $head.editor $win"

global CityName

#wm title $win "Micropolis Editor"
#wm iconname $win $CityName
#wm group $win $head
#wm geometry $win 550x535+440+5
#wm positionfrom $win user
#wm withdraw $win
#wm maxsize $win 5000 5000
#wm minsize $win 32 32
#wm protocol $win delete "DeleteWindow editor EditorWindows"

frame $win.topframe\
  -borderwidth 0

frame $win.topframe.controls\
  -borderwidth 1\
  -relief raised

if {0} {
  menubutton $win.topframe.controls.update\
    -menu $win.topframe.controls.update.m\
    -text {Display}\
    -font [Font $win Medium]\
    -variable $win.postedMenu\
    -borderwidth 1\
    -relief flat
  tk_bindForTraversal $win.topframe.controls.update
  bind $win.topframe.controls.update <F10> {tk_firstMenu %W} 
  bind $win.topframe.controls.update <Mod2-Key> {tk_traverseToMenu %W %A} 
  tk_menus $win $win.topframe.controls.update

  SetHelp $win.topframe.controls.update Editor.Display

  menu $win.topframe.controls.update.m\
    -font [Font $win Medium]
  tk_bindForTraversal $win.topframe.controls.update.m
  bind $win.topframe.controls.update.m <F10> {tk_firstMenu %W} 
  bind $win.topframe.controls.update.m <Mod2-Key> {tk_traverseToMenu %W %A} 
    $win.topframe.controls.update.m add radiobutton\
      -label {Always}\
      -variable Skip.$win\
      -value 0\
      -command "SetEditorSkip $win 0"
    $win.topframe.controls.update.m add radiobutton\
      -label {Often}\
      -variable Skip.$win\
      -value 1\
      -command "SetEditorSkip $win 2"
    $win.topframe.controls.update.m add radiobutton\
      -label {Sometimes}\
      -variable Skip.$win\
      -value 2\
      -command "SetEditorSkip $win 8"
    $win.topframe.controls.update.m add radiobutton\
      -label {Seldom}\
      -variable Skip.$win\
      -value 3\
      -command "SetEditorSkip $win 64"
    $win.topframe.controls.update.m add radiobutton\
      -label {Rarely}\
      -variable Skip.$win\
      -value 4\
      -command "SetEditorSkip $win 256"
}

menubutton $win.topframe.controls.options\
  -menu $win.topframe.controls.options.m\
  -text {Options}\
  -font [Font $win Medium]\
  -variable $win.postedMenu\
  -borderwidth 1\
  -relief flat
tk_bindForTraversal $win.topframe.controls.options
bind $win.topframe.controls.options <F10> {tk_firstMenu %W} 
bind $win.topframe.controls.options <Mod2-Key> {tk_traverseToMenu %W %A} 
tk_menus $win $win.topframe.controls.options

SetHelp $win.topframe.controls.options Editor.Options

menu $win.topframe.controls.options.m\
  -font [Font $win Medium]
tk_bindForTraversal $win.topframe.controls.options.m
bind $win.topframe.controls.options.m <F10> {tk_firstMenu %W} 
bind $win.topframe.controls.options.m <Mod2-Key> {tk_traverseToMenu %W %A} 
$win.topframe.controls.options.m add checkbutton\
  -label {Auto Goto}\
  -variable AutoGoto.$win\
  -command "SetEditorAutoGoto $win \$\{AutoGoto.$win\}"
$win.topframe.controls.options.m add checkbutton\
  -label {Pallet Panel}\
  -variable Controls.$win\
  -command "SetEditorControls $win \$\{Controls.$win\}"
$win.topframe.controls.options.m add checkbutton\
  -label {Chalk Overlay}\
  -variable Overlay.$win\
  -command "SetEditorOverlay $win \$\{Overlay.$win\}"
# Disabled dynamic fulter for now.
#$win.topframe.controls.options.m add checkbutton\
#  -label {Dynamic Filter}\
#  -variable DynamicFilter.$win\
#  -command "SetEditorDynamicFilter $win \$\{DynamicFilter.$win\}"

pack append $win.topframe.controls\
  $win.topframe.controls.options {left frame ne}

#    $win.topframe.controls.update {left frame ne}

label $win.topframe.messagelabel\
  -borderwidth 1\
  -relief raised\
  -font [Font $win Large]\
  -text {}\
  -width 23
LinkWindow $win.message $win.topframe.messagelabel

SetHelp $win.message Editor.Message

pack append $win.topframe\
  $win.topframe.controls	{left frame ne} \
  $win.topframe.messagelabel	{left frame ne expand fill} 

frame $win.centerframe\
  -cursor hand2\
  -borderwidth 1\
  -relief sunken

SetHelp $win.centerframe Editor.View

editorview $win.centerframe.view\
  -font [Font $win Big]
LinkWindow $win.view $win.centerframe.view
LinkWindow $win.centerframe.view.top $win

BindEditorButtons $win.centerframe.view

bind $win.centerframe.view <Any-Enter> {focus %W}

if {[sim MultiPlayerMode]} {

  set entry [WindowLink $head.entry]
  bind $win.centerframe.view <Any-KeyPress> "
      if {\"%A\" != \"\"} {
	  $entry insert cursor %A
	  tk_entrySeeCaret $entry
      }
  "

  bind $win.centerframe.view <Delete> "tk_entryDelPress $entry"
  bind $win.centerframe.view <BackSpace> "tk_entryDelPress $entry"
  bind $win.centerframe.view <Control-h> "tk_entryDelPress $entry"
  bind $win.centerframe.view <Control-d> "tk_textCutPress $entry"
  bind $win.centerframe.view <Control-u> "tk_entryDelLine $entry"
  bind $win.centerframe.view <Control-v> "tk_entryCopyPress $entry"
  bind $win.centerframe.view <Control-w> "tk_entryBackword $entry; tk_entrySeeCaret $entry"
  bind $win.centerframe.view <Return> "DoEnterMessage $entry $entry.value"
  bind $win.centerframe.view <Escape> "DoEvalMessage $entry $entry.value"

} else {

  bind $win.centerframe.view <KeyPress> {EditorKeyDown %W %K}
  bind $win.centerframe.view <KeyRelease> {EditorKeyUp %W %K}

}

bind $win.centerframe.view <Up> "%W PanBy 0 16 ; %W TweakCursor"
bind $win.centerframe.view <Down> "%W PanBy 0 -16 ; %W TweakCursor"
bind $win.centerframe.view <Left> "%W PanBy 16 0 ; %W TweakCursor"
bind $win.centerframe.view <Right> "%W PanBy -16 0 ; %W TweakCursor"
bind $win.centerframe.view <Tab> "EditorToolDown none %W %x %y ; EditorToolUp %W %x %y"

bind $win.centerframe.view <Meta-KeyPress> {EditorKeyDown %W %K}
bind $win.centerframe.view <Meta-KeyRelease> {EditorKeyUp %W %K}
bind $win.centerframe.view <Shift-Meta-KeyPress> {EditorKeyDown %W %K}
bind $win.centerframe.view <Shift-Meta-KeyRelease> {EditorKeyUp %W %K}

pack append $win.centerframe\
  $win.centerframe.view {top frame center fill expand}

frame $win.leftframe\
  -borderwidth 1\
  -relief raised\
  -geometry 130x10

SetHelp $win.leftframe Editor.ToolPallet

frame $win.leftframe.tools\
  -borderwidth 0\
  -relief flat

label $win.leftframe.tools.costlabel1\
  -relief flat\
  -font [Font $win Small]\
  -text {}
LinkWindow $win.cost1 $win.leftframe.tools.costlabel1
LinkWindow $win.centerframe.view.cost1 $win.leftframe.tools.costlabel1

SetHelp $win.leftframe.tools.costlabel1 Editor.ToolName

label $win.leftframe.tools.costlabel2\
  -relief flat\
  -font [Font $win Small]\
  -text {}
LinkWindow $win.cost2 $win.leftframe.tools.costlabel2
LinkWindow $win.centerframe.view.cost2 $win.leftframe.tools.costlabel2
LinkWindow $win.centerframe.top $win

SetHelp $win.leftframe.tools.costlabel2 Editor.ToolCost

canvas $win.leftframe.tools.spacer\
  -width 132 -height 4

button $win.leftframe.tools.palletres\
  -bitmap "@images/icres.xpm"\
  -font [Font $win Medium] \
  -borderwidth 1 -relief flat -padx 0 -pady 0 -text {} -cursor hand2\
  -command "EditorPallet $win 0"
SetHelp $win.leftframe.tools.palletres Editor.ToolRes

button $win.leftframe.tools.palletcom\
  -bitmap "@images/iccom.xpm"\
  -font [Font $win Medium] \
  -borderwidth 1 -relief flat -padx 0 -pady 0 -text {} -cursor hand2\
  -command "EditorPallet $win 1"
SetHelp $win.leftframe.tools.palletcom Editor.ToolCom

button $win.leftframe.tools.palletind\
  -bitmap "@images/icind.xpm"\
  -font [Font $win Medium] \
  -borderwidth 1 -relief flat -padx 0 -pady 0 -text {} -cursor hand2\
  -command "EditorPallet $win 2"
SetHelp $win.leftframe.tools.palletind Editor.ToolInd

button $win.leftframe.tools.palletfire\
  -bitmap "@images/icfire.xpm"\
  -font [Font $win Medium] \
  -borderwidth 1 -relief flat -padx 0 -pady 0 -text {} -cursor hand2\
  -command "EditorPallet $win 3"
SetHelp $win.leftframe.tools.palletfire Editor.ToolFire

button $win.leftframe.tools.palletquery\
  -bitmap "@images/icqry.xpm"\
  -font [Font $win Medium] \
  -borderwidth 1 -relief flat -padx 0 -pady 0 -text {} -cursor hand2\
  -command "EditorPallet $win 4"
SetHelp $win.leftframe.tools.palletquery Editor.ToolQuery

button $win.leftframe.tools.palletpolice\
  -bitmap "@images/icpol.xpm"\
  -font [Font $win Medium] \
  -borderwidth 1 -relief flat -padx 0 -pady 0 -text {} -cursor hand2\
  -command "EditorPallet $win 5"
SetHelp $win.leftframe.tools.palletpolice Editor.ToolPolice

button $win.leftframe.tools.palletwire\
  -bitmap "@images/icwire.xpm"\
  -font [Font $win Medium] \
  -borderwidth 1 -relief flat -padx 0 -pady 0 -text {} -cursor hand2\
  -command "EditorPallet $win 6"
SetHelp $win.leftframe.tools.palletwire Editor.ToolWire

button $win.leftframe.tools.palletbulldozer\
  -bitmap "@images/icdozr.xpm"\
  -font [Font $win Medium] \
  -borderwidth 1 -relief flat -padx 0 -pady 0 -text {} -cursor hand2\
  -command "EditorPallet $win 7"
SetHelp $win.leftframe.tools.palletbulldozer Editor.ToolBulldozer

button $win.leftframe.tools.palletrail\
  -bitmap "@images/icrail.xpm"\
  -font [Font $win Medium] \
  -borderwidth 1 -relief flat -padx 0 -pady 0 -text {} -cursor hand2\
  -command "EditorPallet $win 8"
SetHelp $win.leftframe.tools.palletrail Editor.ToolRail

button $win.leftframe.tools.palletroad\
  -bitmap "@images/icroad.xpm"\
  -font [Font $win Medium] \
  -borderwidth 1 -relief flat -padx 0 -pady 0 -text {} -cursor hand2\
  -command "EditorPallet $win 9"
SetHelp $win.leftframe.tools.palletroad Editor.ToolRoad

button $win.leftframe.tools.palletchalk\
  -bitmap "@images/icchlk.xpm"\
  -font [Font $win Medium] \
  -borderwidth 1 -relief flat -padx 0 -pady 0 -text {} -cursor hand2\
  -command "EditorPallet $win 10"
SetHelp $win.leftframe.tools.palletchalk Editor.ToolChalk

button $win.leftframe.tools.palleteraser\
  -bitmap "@images/icersr.xpm"\
  -font [Font $win Medium] \
  -borderwidth 1 -relief flat -padx 0 -pady 0 -text {} -cursor hand2\
  -command "EditorPallet $win 11"
SetHelp $win.leftframe.tools.palleteraser Editor.ToolEraser

button $win.leftframe.tools.palletstadium\
  -bitmap "@images/icstad.xpm"\
  -font [Font $win Medium] \
  -borderwidth 1 -relief flat -padx 0 -pady 0 -text {} -cursor hand2\
  -command "EditorPallet $win 12"
SetHelp $win.leftframe.tools.palletstadium Editor.ToolStadium

button $win.leftframe.tools.palletpark\
  -bitmap "@images/icpark.xpm"\
  -font [Font $win Medium] \
  -borderwidth 1 -relief flat -padx 0 -pady 0 -text {} -cursor hand2\
  -command "EditorPallet $win 13"
SetHelp $win.leftframe.tools.palletpark Editor.ToolPark

button $win.leftframe.tools.palletseaport\
  -bitmap "@images/icseap.xpm"\
  -font [Font $win Medium] \
  -borderwidth 1 -relief flat -padx 0 -pady 0 -text {} -cursor hand2\
  -command "EditorPallet $win 14"
SetHelp $win.leftframe.tools.palletseaport Editor.ToolSeaport

button $win.leftframe.tools.palletcoal\
  -bitmap "@images/iccoal.xpm"\
  -font [Font $win Medium] \
  -borderwidth 1 -relief flat -padx 0 -pady 0 -text {} -cursor hand2\
  -command "EditorPallet $win 15"
SetHelp $win.leftframe.tools.palletcoal Editor.ToolCoal

button $win.leftframe.tools.palletnuclear\
  -bitmap "@images/icnuc.xpm"\
  -font [Font $win Medium] \
  -borderwidth 1 -relief flat -padx 0 -pady 0 -text {} -cursor hand2\
  -command "EditorPallet $win 16"
SetHelp $win.leftframe.tools.palletnuclear Editor.ToolNuclear

button $win.leftframe.tools.palletairport\
  -bitmap "@images/icairp.xpm"\
  -font [Font $win Medium] \
  -borderwidth 1 -relief flat -padx 0 -pady 0 -text {} -cursor hand2\
  -command "EditorPallet $win 17"
SetHelp $win.leftframe.tools.palletairport Editor.ToolAirport

pack append $win.leftframe.tools\
  $win.leftframe.tools.costlabel1 {top frame center fillx}\
  $win.leftframe.tools.costlabel2 {top frame center fillx}\
  $win.leftframe.tools.spacer     {top frame center fillx}

place $win.leftframe.tools.palletres -x 9 -y 58
place $win.leftframe.tools.palletcom -x 47 -y 58
place $win.leftframe.tools.palletind -x 85 -y 58
place $win.leftframe.tools.palletfire -x 9 -y 112
place $win.leftframe.tools.palletquery -x 47 -y 112
place $win.leftframe.tools.palletpolice -x 85 -y 112
place $win.leftframe.tools.palletwire -x 28 -y 150
place $win.leftframe.tools.palletbulldozer -x 66 -y 150
place $win.leftframe.tools.palletrail -x 6 -y 188
place $win.leftframe.tools.palletroad -x 66 -y 188
place $win.leftframe.tools.palletchalk -x 28 -y 216
place $win.leftframe.tools.palleteraser -x 66 -y 216
place $win.leftframe.tools.palletstadium -x 1 -y 254
place $win.leftframe.tools.palletpark -x 47 -y 254
place $win.leftframe.tools.palletseaport -x 85 -y 254
place $win.leftframe.tools.palletcoal -x 1 -y 300
place $win.leftframe.tools.palletnuclear -x 85 -y 300
place $win.leftframe.tools.palletairport -x 35 -y 346

pack append $win.leftframe\
  $win.leftframe.tools	{top frame center expand fill}

pack append $win\
  $win.topframe	{top frame center fillx} \
  $win.centerframe	{right frame center expand fill} \
  $win.leftframe	{left frame center filly} 

global ShapePies

piemenu $win.toolpie\
    -title Tool\
    -titlefont [Font $win Medium]\
    -font [Font $win Medium]\
    -fixedradius 26\
    -shaped $ShapePies\
    -preview "UIMakeSoundOn $head fancy Woosh {-volume 40}"
  $win.toolpie add command\
    -label Road -bitmap "@images/icroadhi.xpm"\
    -xoffset -4\
    -command "EditorSetTool $win 9"
  $win.toolpie add command\
    -label Bulldozer -bitmap "@images/icdozrhi.xpm"\
    -xoffset 5 -yoffset 17\
    -command "EditorSetTool $win 7"
  $win.toolpie add piemenu\
    -font [Font $win Medium]\
    -label Zone -piemenu $win.zonepie 
  $win.toolpie add command\
    -label Wire -bitmap "@images/icwirehi.xpm"\
    -xoffset -4 -yoffset 17\
    -command "EditorSetTool $win 6"
  $win.toolpie add command\
    -label Rail -bitmap "@images/icrailhi.xpm"\
    -xoffset 4\
    -command "EditorSetTool $win 8"
  $win.toolpie add command\
    -label Chalk -bitmap "@images/icchlkhi.xpm"\
    -xoffset -4 -yoffset -17\
    -command "EditorSetTool $win 10"
  $win.toolpie add piemenu\
    -font [Font $win Medium]\
    -label Build -piemenu $win.buildpie
  $win.toolpie add command\
    -label Eraser -bitmap "@images/icersrhi.xpm"\
    -xoffset 4 -yoffset -17\
    -command "EditorSetTool $win 11"

piemenu $win.zonepie\
    -title Zone\
    -titlefont [Font $win Medium]\
    -font [Font $win Medium]\
    -shaped $ShapePies\
    -initialangle 270 -fixedradius 20
  $win.zonepie add command\
    -label Query -bitmap "@images/icqryhi.xpm"\
    -yoffset 5\
    -command "EditorSetTool $win 4"
  $win.zonepie add command\
    -label Police -bitmap "@images/icpolhi.xpm"\
    -xoffset 4 -yoffset -10\
    -command "EditorSetTool $win 5"
  $win.zonepie add command\
    -label Ind -bitmap "@images/icindhi.xpm"\
    -xoffset 4 -yoffset 25\
    -command "EditorSetTool $win 2"
  $win.zonepie add command\
    -label Com -bitmap "@images/iccomhi.xpm"\
    -yoffset -5\
    -command "EditorSetTool $win 1"
  $win.zonepie add command\
    -label Res -bitmap "@images/icreshi.xpm"\
    -xoffset -4 -yoffset 25\
    -command "EditorSetTool $win 0"
  $win.zonepie add command\
    -label Fire -bitmap "@images/icfirehi.xpm"\
    -xoffset -4 -yoffset -10\
    -command "EditorSetTool $win 3"

piemenu $win.buildpie\
    -title Build\
    -titlefont [Font $win Medium]\
    -font [Font $win Medium]\
    -shaped $ShapePies\
    -initialangle 270 -fixedradius 25
  $win.buildpie add command\
    -label Airport -bitmap "@images/icairphi.xpm"\
    -yoffset 7\
    -command "EditorSetTool $win 17"
  $win.buildpie add command\
    -label Nuclear -bitmap "@images/icnuchi.xpm"\
    -xoffset 11 -yoffset -10\
    -command "EditorSetTool $win 16"
  $win.buildpie add command\
    -label Seaport -bitmap "@images/icseaphi.xpm"\
    -xoffset 0 -yoffset 14\
    -command "EditorSetTool $win 14"
  $win.buildpie add command\
    -label Park -bitmap "@images/icparkhi.xpm"\
    -yoffset -5\
    -command "EditorSetTool $win 13"
  $win.buildpie add command\
    -label Stadium -bitmap "@images/icstadhi.xpm"\
    -xoffset 0 -yoffset 14\
    -command "EditorSetTool $win 12"
  $win.buildpie add command\
    -label Coal -bitmap "@images/iccoalhi.xpm"\
    -xoffset -11 -yoffset -10\
    -command "EditorSetTool $win 15"

SetEditorAutoGoto $win 0
SetEditorControls $win 1
SetEditorOverlay $win 1
SetEditorDynamicFilter $win 0

InitEditor $win

global CityName
UISetCityName $CityName

update idletasks
