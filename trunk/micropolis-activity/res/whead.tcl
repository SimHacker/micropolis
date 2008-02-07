# Window: Head (global controls & menus), for Unix Micropolis.
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
global HeadWindows
set n [Unique]
set win .head$n

catch "destroy $win"
if {[catch "toplevel $win -screen $display"]} {
  puts stderr "Couldn't open X11 display \"$display\"."
  set win ""
  return ""
}

global HeadPanelWidth
global HeadPanelHeight
global MapPanelWidth
global MapPanelHeight
global NoticePanelWidth
global NoticePanelHeight

set visual [winfo screenvisual $win]
set depth [winfo screendepth $win]
#set screenwidth [winfo screenwidth $win]
#set screenheight [winfo screenheight $win]
set screenwidth 1200
set screenheight 900

if {!(("$visual" == "pseudocolor") ||
      ("$visual" == "truecolor") ||
      (("$visual" == "staticgray") &&
       ($depth == 1)))} {
  catch "destroy $win"
  puts stderr "Micropolis can't find an appropriate visual on display \"$display\"."
  set win ""
  return ""
}

set HeadWindows [linsert $HeadWindows 0 $win]

SetHelp $win Head

LinkWindow $win.head $win
LinkWindow $win.editor {}
LinkWindow $win.map {}
LinkWindow $win.graph {}
LinkWindow $win.budget {}
LinkWindow $win.evaluation {}
LinkWindow $win.splash {}
LinkWindow $win.scenario {}
LinkWindow $win.file {}
LinkWindow $win.config {}
LinkWindow $win.notice {}
LinkWindow $win.ask {}

tk_bindForTraversal $win
bind $win <F10> {tk_firstMenu %W} 
bind $win <Mod2-Key> {tk_traverseToMenu %W %A} 

wm title $win "Micropolis Controls"
wm iconname $win {Micropolis Controls}
wm geometry $win 1200x900+0+0
#wm positionfrom $win user
wm withdraw $win
#wm maxsize $win $screenwidth $screenheight
#wm minsize $win 100 100
wm protocol $win delete "DeleteHeadWindow $win ;"
wm fullscreen $win on

global $win.postedMenu
global $win.Sound

frame $win.col1
tk_bindForTraversal $win.col1

LinkWindow $win.col1 $win.col1

frame $win.col1.w1\
  -width $HeadPanelWidth\
  -height $HeadPanelHeight
tk_bindForTraversal $win.col1.w1

frame $win.col1.w1.f1\
  -borderwidth 1\
  -relief raised
tk_bindForTraversal $win.col1.w1.f1
bind $win.col1.w1.f1 <F10> {tk_firstMenu %W} 
bind $win.col1.w1.f1 <Mod2-Key> {tk_traverseToMenu %W %A} 

SetHelp $win.col1.w1.f1.micropolis Head.MicropolisMenu

menubutton $win.col1.w1.f1.micropolis\
  -menu $win.col1.w1.f1.micropolis.m\
  -text {Micropolis}\
  -font [Font $win Medium]\
  -variable $win.postedMenu
tk_bindForTraversal $win.col1.w1.f1.micropolis
bind $win.col1.w1.f1.micropolis <F10> {tk_firstMenu %W} 
bind $win.col1.w1.f1.micropolis <Mod2-Key> {tk_traverseToMenu %W %A} 

tk_menus $win $win.col1.w1.f1.micropolis

menu $win.col1.w1.f1.micropolis.m\
  -font [Font $win Medium]
tk_bindForTraversal $win.col1.w1.f1.micropolis.m
bind $win.col1.w1.f1.micropolis.m <F10> {tk_firstMenu %W} 
bind $win.col1.w1.f1.micropolis.m <Mod2-Key> {tk_traverseToMenu %W %A} 
  $win.col1.w1.f1.micropolis.m add command\
    -label {About...}\
    -command "UIShowPicture 300"
  $win.col1.w1.f1.micropolis.m add command\
    -label {Save City}\
    -command "UISaveCity $win"
  $win.col1.w1.f1.micropolis.m add command\
    -label {Save City as...}\
    -command "UISaveCityAs $win"
  if {[sim MultiPlayerMode]} {
    $win.col1.w1.f1.micropolis.m add command\
      -label {Add Player...}\
      -command "UIShowPlayer $win"
  }
  $win.col1.w1.f1.micropolis.m add command\
    -label {Choose City!}\
    -command "UISelectCity $win"
  $win.col1.w1.f1.micropolis.m add command\
    -label {Quit Playing!}\
    -command "UIQuit $win"

SetHelp $win.col1.w1.f1.options Head.OptionsMenu

menubutton $win.col1.w1.f1.options\
  -menu $win.col1.w1.f1.options.m\
  -text {Options}\
  -font [Font $win Medium]\
  -variable $win.postedMenu
tk_bindForTraversal $win.col1.w1.f1.options
bind $win.col1.w1.f1.options <F10> {tk_firstMenu %W} 
bind $win.col1.w1.f1.options <Mod2-Key> {tk_traverseToMenu %W %A} 

tk_menus $win $win.col1.w1.f1.options

menu $win.col1.w1.f1.options.m\
  -font [Font $win Medium]
tk_bindForTraversal $win.col1.w1.f1.options.m
bind $win.col1.w1.f1.options.m <F10> {tk_firstMenu %W} 
bind $win.col1.w1.f1.options.m <Mod2-Key> {tk_traverseToMenu %W %A} 
  $win.col1.w1.f1.options.m add checkbutton\
    -label {Auto Budget}\
    -variable AutoBudget\
    -command {sim AutoBudget $AutoBudget}
  $win.col1.w1.f1.options.m add checkbutton\
    -label {Auto Bulldoze}\
    -variable AutoBulldoze\
    -command {sim AutoBulldoze $AutoBulldoze}
  $win.col1.w1.f1.options.m add checkbutton\
    -label {Disasters}\
    -variable Disasters\
    -command {sim Disasters $Disasters}
  $win.col1.w1.f1.options.m add checkbutton\
    -label {Sound}\
    -variable Sound\
    -command {sim Sound $Sound}
  $win.col1.w1.f1.options.m add checkbutton\
    -label {Animation}\
    -variable DoAnimation\
    -command {sim DoAnimation $DoAnimation}
  $win.col1.w1.f1.options.m add checkbutton\
    -label {Messages}\
    -variable DoMessages\
    -command {sim DoMessages $DoMessages}
  $win.col1.w1.f1.options.m add checkbutton\
    -label {Notices}\
    -variable DoNotices\
    -command {sim DoNotices $DoNotices}

SetHelp $win.col1.w1.f1.disasters Head.DisastersMenu

menubutton $win.col1.w1.f1.disasters\
  -menu $win.col1.w1.f1.disasters.m\
  -text {Disasters}\
  -font [Font $win Medium]\
  -variable $win.postedMenu
tk_bindForTraversal $win.col1.w1.f1.disasters
bind $win.col1.w1.f1.disasters <F10> {tk_firstMenu %W} 
bind $win.col1.w1.f1.disasters <Mod2-Key> {tk_traverseToMenu %W %A} 

tk_menus $win $win.col1.w1.f1.disasters

menu $win.col1.w1.f1.disasters.m\
  -font [Font $win Medium]
tk_bindForTraversal $win.col1.w1.f1.disasters.m
bind $win.col1.w1.f1.disasters.m <F10> {tk_firstMenu %W} 
bind $win.col1.w1.f1.disasters.m <Mod2-Key> {tk_traverseToMenu %W %A} 
  $win.col1.w1.f1.disasters.m add command\
    -label {Monster}\
    -command "UIDisaster $win \"UIMakeMonster\" \"release a monster?\""
  $win.col1.w1.f1.disasters.m add command\
    -label {Fire}\
    -command "UIDisaster $win \"sim MakeFire\" \"start a fire?\""
  $win.col1.w1.f1.disasters.m add command\
    -label {Flood}\
    -command "UIDisaster $win \"sim MakeFlood\" \"bring on a flood?\""
  $win.col1.w1.f1.disasters.m add command\
    -label {Meltdown}\
    -command "UIDisaster $win \"sim MakeMeltdown\" \"have a nuclear meltdown?\""
  $win.col1.w1.f1.disasters.m add command\
    -label {Tornado}\
    -command "UIDisaster $win \"sim MakeTornado\" \"spin up a tornado?\""
  $win.col1.w1.f1.disasters.m add command\
    -label {Earthquake}\
    -command "UIDisaster $win \"sim MakeEarthquake\" \"cause an earthquake?\""

SetHelp $win.col1.w1.f1.priority Head.PriorityMenu

menubutton $win.col1.w1.f1.priority\
  -menu $win.col1.w1.f1.priority.m\
  -text {Priority}\
  -font [Font $win Medium]\
  -variable $win.postedMenu
tk_bindForTraversal $win.col1.w1.f1.priority
bind $win.col1.w1.f1.priority <F10> {tk_firstMenu %W} 
bind $win.col1.w1.f1.priority <Mod2-Key> {tk_traverseToMenu %W %A} 

tk_menus $win $win.col1.w1.f1.priority

menu $win.col1.w1.f1.priority.m\
  -font [Font $win Medium]
tk_bindForTraversal $win.col1.w1.f1.priority.m
bind $win.col1.w1.f1.priority.m <F10> {tk_firstMenu %W} 
bind $win.col1.w1.f1.priority.m <Mod2-Key> {tk_traverseToMenu %W %A} 
  $win.col1.w1.f1.priority.m add radiobutton\
    -label {Super Fast}\
    -command {SetPriority 4}\
    -value {4}\
    -variable Priority
  $win.col1.w1.f1.priority.m add radiobutton\
    -label {Fast}\
    -command {SetPriority 3}\
    -value {3}\
    -variable Priority
  $win.col1.w1.f1.priority.m add radiobutton\
    -label {Normal}\
    -command {SetPriority 2}\
    -value {2}\
    -variable Priority
  $win.col1.w1.f1.priority.m add radiobutton\
    -label {Slow}\
    -command {SetPriority 1}\
    -value {1}\
    -variable Priority
  $win.col1.w1.f1.priority.m add radiobutton\
    -label {Super Slow}\
    -command {SetPriority 0}\
    -value {0}\
    -variable Priority

SetHelp $win.col1.w1.f1.windows Head.WindowsMenu

menubutton $win.col1.w1.f1.windows\
  -menu $win.col1.w1.f1.windows.m\
  -text {Windows}\
  -font [Font $win Medium]\
  -variable $win.postedMenu
tk_bindForTraversal $win.col1.w1.f1.windows
bind $win.col1.w1.f1.windows <F10> {tk_firstMenu %W} 
bind $win.col1.w1.f1.windows <Mod2-Key> {tk_traverseToMenu %W %A} 

tk_menus $win $win.col1.w1.f1.windows

menu $win.col1.w1.f1.windows.m\
  -font [Font $win Medium]
tk_bindForTraversal $win.col1.w1.f1.windows.m
bind $win.col1.w1.f1.windows.m <F10> {tk_firstMenu %W} 
bind $win.col1.w1.f1.windows.m <Mod2-Key> {tk_traverseToMenu %W %A} 
  $win.col1.w1.f1.windows.m add command\
    -label {Budget}\
    -command "UIShowBudgetAndWait"
  $win.col1.w1.f1.windows.m add command\
    -label {Evaluation}\
    -command "ShowEvaluationOf $win"
  $win.col1.w1.f1.windows.m add command\
    -label {Graph}\
    -command "ShowGraphOf $win"
  #$win.col1.w1.f1.windows.m add command\
  #  -label {Map}\
  #  -command "ShowMapOf $win"
  #$win.col1.w1.f1.windows.m add command\
  #  -label {Editor}\
  #  -command "ShowEditorOf $win"
  #$win.col1.w1.f1.windows.m add command\
  #  -label {Frob}\
  #  -command "ShowFrobOf $win"
  #$win.col1.w1.f1.windows.m add command\
  #  -label {New Map}\
  #  -command "NewMapOf $win"
  #$win.col1.w1.f1.windows.m add command\
  #  -label {New Editor}\
  #  -command "NewEditorOf $win"

LinkWindow $win.m0 $win.col1.w1.f1.micropolis.m
LinkWindow $win.m1 $win.col1.w1.f1.options.m
LinkWindow $win.m2 $win.col1.w1.f1.disasters.m
LinkWindow $win.m3 $win.col1.w1.f1.priority.m
LinkWindow $win.m4 $win.col1.w1.f1.windows.m

LinkWindow $win.b0 $win.col1.w1.f1.micropolis
LinkWindow $win.b1 $win.col1.w1.f1.options
LinkWindow $win.b2 $win.col1.w1.f1.disasters
LinkWindow $win.b3 $win.col1.w1.f1.priority
LinkWindow $win.b4 $win.col1.w1.f1.windows

pack append $win.col1.w1.f1\
  $win.col1.w1.f1.micropolis	{left frame nw} \
  $win.col1.w1.f1.options	{left frame nw} \
  $win.col1.w1.f1.disasters	{left frame nw} \
  $win.col1.w1.f1.priority	{left frame nw} \
  $win.col1.w1.f1.windows	{left frame nw}

frame $win.col1.w1.f2\
  -background #BFBFBF \
  -borderwidth 1\
  -relief raised

frame $win.col1.w1.f2.f1\
  -background #BFBFBF \
  -borderwidth 1\
  -relief flat

frame $win.col1.w1.f2.f1.frame \
  -background #BFBFBF \
  -borderwidth 0\
  -relief flat

canvas $win.col1.w1.f2.f1.frame.demand\
  -scrollincrement 0 \
  -borderwidth 0 \
  -background #BFBFBF \
  -width 80 -height 55
LinkWindow $win.demand $win.col1.w1.f2.f1.frame.demand
$win.col1.w1.f2.f1.frame.demand create bitmap 41 4 \
  -tags picture \
  -bitmap "@images/demandg.xpm" \
  -anchor nw
$win.col1.w1.f2.f1.frame.demand create rectangle -10 -10 1 1 \
  -tags r \
  -fill [Color $win #00ff00 #000000]
$win.col1.w1.f2.f1.frame.demand create rectangle -10 -10 1 1 \
  -tags c \
  -fill [Color $win #0000ff #000000]
$win.col1.w1.f2.f1.frame.demand create rectangle -10 -10 1 1 \
  -tags i \
  -fill [Color $win #ffff00 #000000]
$win.col1.w1.f2.f1.frame.demand create bitmap 0 4 \
  -tags micropolis \
  -bitmap "@images/micropoliss.xpm" \
  -anchor nw 

$win.col1.w1.f2.f1.frame.demand bind micropolis <1> {TogglePause}
$win.col1.w1.f2.f1.frame.demand bind micropolis <2> {TogglePause}
$win.col1.w1.f2.f1.frame.demand bind micropolis <3> {TogglePause}
$win.col1.w1.f2.f1.frame.demand bind picture <1> "ToggleEvaluationOf $win"
$win.col1.w1.f2.f1.frame.demand bind picture <2> "ToggleEvaluationOf $win"
$win.col1.w1.f2.f1.frame.demand bind picture <3> "ToggleEvaluationOf $win"
$win.col1.w1.f2.f1.frame.demand bind r <1> "ToggleEvaluationOf $win"
$win.col1.w1.f2.f1.frame.demand bind r <2> "ToggleEvaluationOf $win"
$win.col1.w1.f2.f1.frame.demand bind r <3> "ToggleEvaluationOf $win"
$win.col1.w1.f2.f1.frame.demand bind c <1> "ToggleEvaluationOf $win"
$win.col1.w1.f2.f1.frame.demand bind c <2> "ToggleEvaluationOf $win"
$win.col1.w1.f2.f1.frame.demand bind c <3> "ToggleEvaluationOf $win"
$win.col1.w1.f2.f1.frame.demand bind i <1> "ToggleEvaluationOf $win"
$win.col1.w1.f2.f1.frame.demand bind i <2> "ToggleEvaluationOf $win"
$win.col1.w1.f2.f1.frame.demand bind i <3> "ToggleEvaluationOf $win"

SetHelp $win.col1.w1.f2.f1.frame.demand Head.Demand

frame $win.col1.w1.f2.f1.frame.graphframe \
  -background #BFBFBF \
  -borderwidth 1\
  -relief sunken

graphview $win.col1.w1.f2.f1.frame.graphframe.graph\
  -background #BFBFBF \
  -font [Font $win Tiny]
$win.col1.w1.f2.f1.frame.graphframe.graph Range 10
$win.col1.w1.f2.f1.frame.graphframe.graph Mask 7
LinkWindow $win.graphview $win.col1.w1.f2.f1.frame.graphframe.graph
bind $win.col1.w1.f2.f1.frame.graphframe.graph <ButtonPress> "ToggleGraphOf $win"


SetHelp $win.col1.w1.f2.f1.frame.graphframe.graph Head.Graph

pack append $win.col1.w1.f2.f1.frame.graphframe \
  $win.col1.w1.f2.f1.frame.graphframe.graph	{left expand fill}

pack append $win.col1.w1.f2.f1.frame \
  $win.col1.w1.f2.f1.frame.demand		{left frame sw padx 4} \
  $win.col1.w1.f2.f1.frame.graphframe	{right frame center expand fill}

frame $win.col1.w1.f2.f1.info\
  -background #BFBFBF \
  -borderwidth 1\
  -relief flat

dateview $win.col1.w1.f2.f1.info.date\
  -background #BFBFBF \
  -width 20 \
  -font [Font $win Medium]
LinkWindow $win.date $win.col1.w1.f2.f1.info.date

SetHelp $win.col1.w1.f2.f1.info.date Head.Date

label $win.col1.w1.f2.f1.info.fundslabel\
  -background #BFBFBF \
  -relief flat\
  -font [Font $win Medium]\
  -text {}\
  -anchor w\
  -width 20
LinkWindow $win.funds $win.col1.w1.f2.f1.info.fundslabel
bind $win.col1.w1.f2.f1.info.fundslabel <ButtonPress> "UIShowBudgetAndWait"

SetHelp $win.col1.w1.f2.f1.info.fundslabel Head.Funds

label $win.col1.w1.f2.f1.info.rate\
  -background #BFBFBF \
  -relief flat\
  -font [Font $win Medium]\
  -text {Tax Rate: 7%} \
  -anchor w\
  -width 20
LinkWindow $win.taxlabel $win.col1.w1.f2.f1.info.rate
bind $win.col1.w1.f2.f1.info.rate <ButtonPress> "UIShowBudgetAndWait"


scale $win.col1.w1.f2.f1.info.scale\
  -background #BFBFBF \
  -command {SetTaxRate}\
  -orient horizontal\
  -showvalue false\
  -font [Font $win Medium]\
  -sliderlength 15\
  -to 20
$win.col1.w1.f2.f1.info.scale set 7
LinkWindow $win.taxrate $win.col1.w1.f2.f1.info.scale


pack append $win.col1.w1.f2.f1.info \
  $win.col1.w1.f2.f1.info.date		{top frame nw} \
  $win.col1.w1.f2.f1.info.fundslabel	{top frame nw} \
  $win.col1.w1.f2.f1.info.rate		{top frame nw} \
  $win.col1.w1.f2.f1.info.scale		{top frame nw fill expand}

pack append $win.col1.w1.f2.f1 \
  $win.col1.w1.f2.f1.frame		{left frame nw expand fill} \
  $win.col1.w1.f2.f1.info		{left frame nw}

frame $win.col1.w1.f2.f2\
  -borderwidth 1 \
  -relief flat
tk_bindForTraversal $win.col1.w1.f2.f2
bind $win.col1.w1.f2.f2 <F10> {tk_firstMenu %W} 
bind $win.col1.w1.f2.f2 <Mod2-Key> {tk_traverseToMenu %W %A} 

SetHelp $win.col1.w1.f2.f2 Head.Log

scrollbar $win.col1.w1.f2.f2.scroll\
  -command "$win.col1.w1.f2.f2.text yview" \
  -borderwidth 1

SetHelp $win.col1.w1.f2.f2.scroll Head.Scrollbar

text $win.col1.w1.f2.f2.text \
  -yscroll "$win.col1.w1.f2.f2.scroll set" \
  -borderwidth 1 \
  -relief sunken \
  -wrap word \
  -state disabled \
  -height 5 \
  -font [Font $win Text]
LinkWindow $win.text $win.col1.w1.f2.f2.text

$win.col1.w1.f2.f2.text tag configure status \
  -font [Font $win Message]

$win.col1.w1.f2.f2.text tag configure message \
  -font [Font $win Message] \
  -foreground #ffffff \
  -background #3f3f3f

$win.col1.w1.f2.f2.text tag configure alert \
  -font [Font $win Alert] \
  -foreground [Color $win #ff3f3f #000000]

pack append $win.col1.w1.f2.f2 \
  $win.col1.w1.f2.f2.scroll	{left frame center filly} \
  $win.col1.w1.f2.f2.text	{right frame center fill expand}

if {[sim MultiPlayerMode]} {
  frame $win.col1.w1.f2.f3 \
    -borderwidth 1 \
    -relief flat
  tk_bindForTraversal $win.col1.w1.f2.f3
  bind $win.col1.w1.f2.f3 <F10> {tk_firstMenu %W} 
  bind $win.col1.w1.f2.f3 <Mod2-Key> {tk_traverseToMenu %W %A} 

  button $win.col1.w1.f2.f3.chat \
    -font [Font $win Large] \
    -relief flat \
    -text {Chat:}
  LinkWindow $win.chat $win.col1.w1.f2.f3.chat
  bind $win.col1.w1.f2.f3.chat <1> {ChatDown %W}
  bind $win.col1.w1.f2.f3.chat <ButtonRelease-1> {ChatUp %W}

  SetHelp $win.col1.w1.f2.f3.chat Head.Chat

  entry $win.col1.w1.f2.f3.entry \
    -relief sunken\
    -text {}\
    -foreground #ffffff\
    -background #4f4f4f\
    -textvariable $win.col1.w1.f2.f3.entry.value\
    -font [Font $win Message]
  global $win.col1.w1.f2.f3.entry.value
  set $win.col1.w1.f2.f3.entry.value ""
  tk_bindForTraversal $win.col1.w1.f2.f3.entry
  bind $win.col1.w1.f2.f3.entry <F10> {tk_firstMenu %W} 
  bind $win.col1.w1.f2.f3.entry <Mod2-Key> {tk_traverseToMenu %W %A}
  bind $win.col1.w1.f2.f3.entry <Return> "DoEnterMessage %W %W.value"
  bind $win.col1.w1.f2.f3.entry <Escape> "DoEvalMessage %W %W.value"
  bind $win.col1.w1.f2.f3.entry <Any-Enter> {focus %W}
  LinkWindow $win.entry $win.col1.w1.f2.f3.entry

  SetHelp $win.col1.w1.f2.f3.entry Head.Entry
}

frame $win.col1.w2
tk_bindForTraversal $win.col1.w2

LinkWindow $win.w2 $win.col1.w2

frame $win.col1.w3
tk_bindForTraversal $win.col1.w3

LinkWindow $win.w3 $win.col1.w3

frame $win.col2
tk_bindForTraversal $win.col2

LinkWindow $win.col2 $win.col2

#frame $win.col2.x1 -width 400 -height 400
#tk_bindForTraversal $win.col2.x1

#LinkWindow $win.x1 $win.col2.x1

#frame $win.col2.x2
#tk_bindForTraversal $win.col2.x2

#LinkWindow $win.x2 $win.col2.x2

#frame $win.col2.x3
#tk_bindForTraversal $win.col2.x3

#LinkWindow $win.x3 $win.col2.x3

#frame $win.col2.x4
#tk_bindForTraversal $win.col2.x4

#LinkWindow $win.x4 $win.col2.x4

if {[sim MultiPlayerMode]} {

  pack append $win.col1.w1.f2.f3 \
    $win.col1.w1.f2.f3.chat	{left frame center padx 4} \
    $win.col1.w1.f2.f3.entry	{left frame center fillx expand padx 4}

  pack append $win.col1.w1.f2 \
    $win.col1.w1.f2.f1		{top frame center fillx} \
    $win.col1.w1.f2.f2		{top frame center expand fill} \
    $win.col1.w1.f2.f3		{top frame center fillx}

} else {

  pack append $win.col1.w1.f2 \
    $win.col1.w1.f2.f1		{top frame center fillx} \
    $win.col1.w1.f2.f2		{top frame center expand fill}

}

pack append $win.col1.w1\
  $win.col1.w1.f1		{top frame center fillx} \
  $win.col1.w1.f2		{top frame center expand fill}

#pack append $win\
#    $win.col1			{left frame center filly} \
#    $win.col2			{left frame center expand fill}

place configure $win.col1\
  -x 0\
  -y 0\
  -width $HeadPanelWidth\
  -height $screenheight

pack append $win.col1\
    $win.col1.w1		{top frame nw fillx} \
    $win.col1.w2		{top frame nw fillx} \
    $win.col1.w3		{bottom frame sw fillx} 

#place configure $win.col1.w1\
#  -x 0\
#  -y 0\
#  -width $HeadPanelWidth\
#  -height $HeadPanelHeight

#place configure $win.col1.w2\
#  -x 0\
#  -y $HeadPanelHeight\
#  -width $MapPanelWidth\
#  -height $MapPanelHeight

#place configure $win.col1.w3\
#  -x 0\
#  -y [expr "$HeadPanelHeight + $MapPanelHeight"]\
#  -width $NoticePanelWidth\
#  -height [expr "$screenheight - ($HeadPanelHeight + $MapPanelHeight)"]

place configure $win.col2\
  -x [expr "$HeadPanelWidth + 5"]\
  -y 0\
  -width [expr "($screenwidth - $HeadPanelWidth) - 5"]\
  -height $screenheight

#pack append $win.col2\
#    $win.col2.x1		{top frame nw fillx} \
#    $win.col2.x2		{top frame nw fillx} \
#    $win.col2.x3		{top frame nw expand fill} \
#    $win.col2.x4		{top frame nw fillx}

SetupSoundServer $win

InitHead $win
InitHeadMenus $win

update idletasks
