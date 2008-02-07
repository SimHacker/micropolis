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
set parent [WindowLink $head.col2]
set win $parent.scenario$n
set ScenarioWindows [linsert $ScenarioWindows 0 $win]

global ScenarioPanelWidth
global ScenarioPanelHeight

LinkWindow $head.scenario $win
LinkWindow $win.head $head
LinkWindow $win.parent $parent

catch "destroy $win"
frame $win \
  -width $ScenarioPanelWidth \
  -height $ScenarioPanelHeight

SetHelp $win Scenario

frame $win.scen\
  -borderwidth 0 -geometry 100x280

label $win.scen.micropolis\
  -bitmap "@images/micropolisl.xpm" \
  -font [Font $win Medium]

button $win.scen.newcity\
  -command "UIGenerateNewCity" \
  -relief flat \
  -borderwidth 0 \
  -bitmap "@images/scncty.xpm" \
  -font [Font $win Medium]

SetHelp $win.scen.newcity Scenario.NewCity

button $win.scen.loadcity\
  -command "UILoadCity $head" \
  -relief flat \
  -borderwidth 0 \
  -bitmap "@images/sclcty.xpm" \
  -font [Font $win Medium]

SetHelp $win.scen.loadcity Scenario.LoadCity

button $win.scen.dullsville\
  -command {UILoadScenario 1} \
  -relief flat \
  -borderwidth 0 \
  -bitmap "@images/scdull.xpm" \
  -font [Font $win Medium]

SetHelp $win.scen.dullsville Scenario.Dullsville

button $win.scen.sanfrancisco\
  -command {UILoadScenario 2} \
  -relief flat \
  -borderwidth 0 \
  -bitmap "@images/scsfo.xpm" \
  -font [Font $win Medium]

SetHelp $win.scen.sanfrancisco Scenario.SanFran

button $win.scen.hamburg\
  -command {UILoadScenario 3} \
  -relief flat \
  -borderwidth 0 \
  -bitmap "@images/scham.xpm" \
  -font [Font $win Medium]

SetHelp $win.scen.hamburg Scenario.Hamburg

button $win.scen.bern\
  -command {UILoadScenario 4} \
  -relief flat \
  -borderwidth 0 \
  -bitmap "@images/scbern.xpm" \
  -font [Font $win Medium]

SetHelp $win.scen.bern Scenario.Bern

button $win.scen.tokyo\
  -command {UILoadScenario 5} \
  -relief flat \
  -borderwidth 0 \
  -bitmap "@images/sctkyo.xpm" \
  -font [Font $win Medium]

SetHelp $win.scen.tokyo Scenario.Tokyo

button $win.scen.rio\
  -command {UILoadScenario 8} \
  -relief flat \
  -borderwidth 0 \
  -bitmap "@images/scrio.xpm" \
  -font [Font $win Medium]

SetHelp $win.scen.rio Scenario.Rio

button $win.scen.boston\
  -command {UILoadScenario 7} \
  -relief flat \
  -borderwidth 0 \
  -bitmap "@images/scbos.xpm" \
  -font [Font $win Medium]

SetHelp $win.scen.boston Scenario.Boston

button $win.scen.detroit\
  -command {UILoadScenario 6} \
  -relief flat \
  -borderwidth 0 \
  -bitmap "@images/scdet.xpm" \
  -font [Font $win Medium]

SetHelp $win.scen.detroit Scenario.Detroit

message $win.scen.labeldullville\
  -aspect 1500\
  -font [Font $win Small] \
  -justify center \
  -text {Dullsville 1900
Boredom}

SetHelp $win.scen.labeldullville Scenario.Dullsville

message $win.scen.labelsanfrancisco\
  -aspect 1500\
  -font [Font $win Small] \
  -justify center \
  -text {San Francisco 1906
Earthquake}

SetHelp $win.scen.labelsanfrancisco Scenario.SanFran

message $win.scen.labelhamburg\
  -aspect 1500\
  -font [Font $win Small] \
  -justify center \
  -text {Hamburg 1944
Bombing}

SetHelp $win.scen.labelhamburg Scenario.Hamburg

message $win.scen.labelbern\
  -aspect 1500\
  -font [Font $win Small] \
  -justify center \
  -text {Bern 1965
Traffic}

SetHelp $win.scen.labelbern Scenario.Bern

message $win.scen.labeltokyo\
  -aspect 1500\
  -font [Font $win Small] \
  -justify center \
  -text {Tokyo 1957
Monster Attack}

SetHelp $win.scen.labeltokyo Scenario.Tokyo

message $win.scen.labelrio\
  -aspect 1500\
  -font [Font $win Small] \
  -justify center \
  -text {Rio de Janeiro 2047
Coastal Flooding}

SetHelp $win.scen.labelrio Scenario.Rio

message $win.scen.labelboston\
  -aspect 1500\
  -font [Font $win Small] \
  -justify center \
  -text {Boston 2010
Nuclear Meltdown}

SetHelp $win.scen.labelboston Scenario.Boston

message $win.scen.labeldetroit\
  -aspect 1500\
  -font [Font $win Small] \
  -justify center \
  -text {Detroit 1972
Crime}

SetHelp $win.scen.labeldetroit Scenario.Detroit

4lace $win.scen.micropolis -anchor n -x 205 -y 25
place $win.scen.newcity -x 20 -y 10
place $win.scen.loadcity -x 320 -y 10
place $win.scen.dullsville -x 20 -y 80
place $win.scen.sanfrancisco -x 120 -y 80
place $win.scen.hamburg -x 220 -y 80
place $win.scen.bern -x 320 -y 80
place $win.scen.tokyo -x 20 -y 180
place $win.scen.rio -x 120 -y 180
place $win.scen.boston -x 220 -y 180
place $win.scen.detroit -x 320 -y 180
place $win.scen.labeldullville -anchor n -x 54 -y 140
place $win.scen.labelsanfrancisco -anchor n -x 154 -y 140
place $win.scen.labelhamburg -anchor n -x 254 -y 140
place $win.scen.labelbern -anchor n -x 354 -y 140
place $win.scen.labeltokyo -anchor n -x 54 -y 240
place $win.scen.labelrio -anchor n -x 154 -y 240
place $win.scen.labelboston -anchor n -x 254 -y 240
place $win.scen.labeldetroit -anchor n -x 354 -y 240

BindSimButton $win.scen.newcity
BindSimButton $win.scen.loadcity
BindSimButton $win.scen.dullsville
BindSimButton $win.scen.sanfrancisco
BindSimButton $win.scen.hamburg
BindSimButton $win.scen.bern
BindSimButton $win.scen.tokyo
BindSimButton $win.scen.rio
BindSimButton $win.scen.boston
BindSimButton $win.scen.detroit

frame $win.name \
  -borderwidth 2\
  -relief flat

SetHelp $win.name Scenario.Name

label $win.name.namelabel \
  -text {City Name:} \
  -font [Font $win Text]

entry $win.name.cityname \
  -text {}\
  -textvariable CityName \
  -font [Font $win Text] \
  -width 50
bind $win.name.cityname <Return> "DoEnterCityName $win"
bind $win.name.cityname <Any-Enter> {focus %W}
LinkWindow $win.cityname $win.name.cityname

pack append $win.name \
  $win.name.namelabel	{left frame e}\
  $win.name.cityname	{left frame e}

frame $win.level \
  -borderwidth 2\
  -relief flat

SetHelp $win.level Scenario.Level

label $win.level.levellabel \
  -text {Game Level:} \
  -font [Font $win Text]

frame $win.level.radio \
  -borderwidth 0\
  -relief flat

radiobutton $win.level.radio.easy \
  -text "Easy" -variable GameLevel -value 0 \
  -command "DoSetGameLevel 0" \
  -font [Font $win Large]

radiobutton $win.level.radio.medium \
  -text "Medium" -variable GameLevel -value 1 \
  -command "DoSetGameLevel 1" \
  -font [Font $win Large]

radiobutton $win.level.radio.hard \
  -text "Hard" -variable GameLevel -value 2 \
  -command "DoSetGameLevel 2" \
  -font [Font $win Large]

pack append $win.level.radio \
  $win.level.radio.easy		{top frame e expand fillx}\
  $win.level.radio.medium	{top frame e expand fillx}\
  $win.level.radio.hard		{top frame e expand fillx}

frame $win.level.label \
  -borderwidth 0\
  -relief flat

label $win.level.label.easylabel \
  -text {Initial Funds: $20,000} \
  -font [Font $win Large] \
  -anchor w

label $win.level.label.mediumlabel \
  -text {Initial Funds: $10,000} \
  -font [Font $win Large] \
  -anchor w

label $win.level.label.hardlabel \
  -text {Initial Funds: $5,000} \
  -font [Font $win Large] \
  -anchor w

pack append $win.level.label \
  $win.level.label.easylabel		{top frame e expand fillx} \
  $win.level.label.mediumlabel		{top frame e expand fillx} \
  $win.level.label.hardlabel		{top frame e expand fillx}

pack append $win.level \
   $win.level.levellabel		{left frame ne} \
   $win.level.radio			{left frame ne} \
   $win.level.label			{left frame ne expand fillx}

frame $win.buttons \
  -borderwidth 0\
  -relief flat -borderwidth 2

button $win.buttons.previous \
  -text {Previous Map}\
  -font [Font $win Large]\
  -command "PrevHistory"\
  -relief raised \
  -width 20
LinkWindow $win.previous $win.buttons.previous

SetHelp $win.buttons.previous Scenario.Previous

frame $win.buttons.voteframe \
  -borderwidth 0 \
  -relief raised
LinkWindow $win.voteframe $win.buttons.voteframe

SetHelp $win.buttons.voteframe Scenario.Use

button $win.buttons.voteframe.vote \
  -font [Font $win Large]\
  -text {    Use This Map    }\
  -command "DoVote $win UseThisMap UIUseThisMap {}"\
  -relief raised \
  -borderwidth 1\
  -width 20
LinkWindow $win.vote $win.buttons.voteframe.vote

pack append $win.buttons.voteframe\
  $win.buttons.voteframe.vote	{top frame center}

BindVotingButton $win $win.buttons.voteframe.vote UseThisMap

button $win.buttons.next \
  -font [Font $win Large]\
  -text {Next Map}\
  -command "NextHistory"\
  -width 20
LinkWindow $win.next $win.buttons.next

SetHelp $win.buttons.next Scenario.Next

pack append $win.buttons\
  $win.buttons.previous		{left frame nw}\
  $win.buttons.voteframe	{left frame nw}\
  $win.buttons.next		{left frame nw}

pack append $win\
  $win.scen			{top frame center fillx}\
  $win.buttons			{top frame center fillx}\
  $win.name			{top frame center fillx}\
  $win.level			{top frame center fillx}\

#place configure $win -x 0 -y 0 -width $ScenarioPanelWidth -height $ScenarioPanelHeight

InitScenario $win

update idletasks
