# Window: Budget, for Unix Micropolis
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
global BudgetWindows
set n [Unique]
set win .budget$n
set BudgetWindows [linsert $BudgetWindows 0 $win]

LinkWindow $head.budget $win
LinkWindow $win.head $head

catch "destroy $win"
toplevel $win -screen $display

SetHelp $win Budget

global [set var $win.visible]
set $var 0

wm title $win {Micropolis Budget}
wm iconname $win {Micropolis Budget}
wm group $win $head
wm geometry $win 600x600+5+5
wm positionfrom $win user
wm maxsize $win 700 700
wm minsize $win 150 200
wm protocol $win delete "BudgetCancel"
wm transient $win $head

label $win.label\
  -borderwidth 1\
  -relief raised\
  -font [Font $win Large]\
  -text {Micropolis has paused to set the budget...}

SetHelp $win.label Budget.Label

frame $win.rightframe\
  -borderwidth 0

frame $win.rightframe.roadframe\
  -borderwidth 1\
  -relief raised

SetHelp $win.rightframe.roadframe Budget.Road

label $win.rightframe.roadframe.label\
  -relief flat\
  -font [Font $win Large]\
  -text {            Road Fund            }

frame $win.rightframe.roadframe.frame\
  -borderwidth 2\
  -relief flat
LinkWindow $win.road $win.rightframe.roadframe.frame

label $win.rightframe.roadframe.frame.request\
  -relief flat\
  -font [Font $win Medium]\
  -text {100% of $0 = $0}

scale $win.rightframe.roadframe.frame.fund\
  -command {BudgetSetRoadFund}\
  -font [Font $win Medium]\
  -orient horizontal\
  -showvalue false\
  -sliderlength 15\
  -to 100\
  -width 10

pack append $win.rightframe.roadframe.frame\
  $win.rightframe.roadframe.frame.fund      {top frame center expand fill} \
  $win.rightframe.roadframe.frame.request   {top frame center expand fill}

pack append $win.rightframe.roadframe\
  $win.rightframe.roadframe.label     {top frame center fillx} \
  $win.rightframe.roadframe.frame     {top frame center expand fill}


frame $win.rightframe.fireframe\
  -borderwidth 1\
  -relief raised

SetHelp $win.rightframe.fireframe Budget.Fire

label $win.rightframe.fireframe.label\
  -font [Font $win Large]\
  -relief flat\
  -text {Fire Fund}

frame $win.rightframe.fireframe.frame\
  -borderwidth 2\
  -relief flat
LinkWindow $win.fire $win.rightframe.fireframe.frame

label $win.rightframe.fireframe.frame.request\
  -relief flat\
  -font [Font $win Medium]\
  -text {100% of $0 = $0}

scale $win.rightframe.fireframe.frame.fund\
  -command {BudgetSetFireFund}\
  -font [Font $win Medium]\
  -orient horizontal\
  -showvalue false\
  -sliderlength 15\
  -to 100\
  -width 10

pack append $win.rightframe.fireframe.frame\
  $win.rightframe.fireframe.frame.fund      {top frame center expand fill} \
  $win.rightframe.fireframe.frame.request   {top frame center expand fill}

pack append $win.rightframe.fireframe\
  $win.rightframe.fireframe.label     {top frame center fillx} \
  $win.rightframe.fireframe.frame     {top frame center expand fill}


frame $win.rightframe.policeframe\
  -borderwidth 1\
  -relief raised

SetHelp $win.rightframe.policeframe Budget.Police

label $win.rightframe.policeframe.label\
  -relief flat\
  -font [Font $win Large]\
  -text {Police Fund}

frame $win.rightframe.policeframe.frame\
  -borderwidth 2\
  -relief flat
LinkWindow $win.police $win.rightframe.policeframe.frame

label $win.rightframe.policeframe.frame.request\
  -relief flat\
  -font [Font $win Medium]\
  -text {100% of $0 = $0}

scale $win.rightframe.policeframe.frame.fund\
  -command {BudgetSetPoliceFund}\
  -font [Font $win Medium]\
  -orient horizontal\
  -showvalue false\
  -sliderlength 15\
  -to 100\
  -width 10

pack append $win.rightframe.policeframe.frame\
  $win.rightframe.policeframe.frame.fund      {top frame center expand fill}\
  $win.rightframe.policeframe.frame.request   {top frame center expand fill}

pack append $win.rightframe.policeframe\
  $win.rightframe.policeframe.label     {top frame center fillx} \
  $win.rightframe.policeframe.frame     {top frame center expand fill}


frame $win.rightframe.taxrateframe\
  -borderwidth 1\
  -relief raised

SetHelp $win.rightframe.taxrateframe Budget.Tax

label $win.rightframe.taxrateframe.label\
  -relief flat\
  -font [Font $win Large]\
  -text {Tax Rate}

frame $win.rightframe.taxrateframe.frame\
  -borderwidth 2\
  -relief flat

label $win.rightframe.taxrateframe.frame.rate\
  -relief flat\
  -font [Font $win Medium]\
  -text {7%}
LinkWindow $win.taxlabel $win.rightframe.taxrateframe.frame.rate

scale $win.rightframe.taxrateframe.frame.scale\
  -command {BudgetSetTaxRate}\
  -font [Font $win Medium]\
  -orient horizontal\
  -showvalue false\
  -sliderlength 15\
  -to 20\
  -width 10
LinkWindow $win.taxrate $win.rightframe.taxrateframe.frame.scale

pack append $win.rightframe.taxrateframe.frame\
  $win.rightframe.taxrateframe.frame.scale    {top frame center expand fill}\
  $win.rightframe.taxrateframe.frame.rate     {top frame center expand fill}

pack append $win.rightframe.taxrateframe\
  $win.rightframe.taxrateframe.label    {top frame center fillx} \
  $win.rightframe.taxrateframe.frame    {top frame center expand fill}


pack append $win.rightframe\
  $win.rightframe.roadframe    {top frame center expand fill} \
  $win.rightframe.fireframe    {top frame center expand fill} \
  $win.rightframe.policeframe  {top frame center expand fill} \
  $win.rightframe.taxrateframe {top frame center expand fill}


frame $win.leftframe\
  -borderwidth 0

frame $win.leftframe.collectedframe\
  -borderwidth 1\
  -relief raised

SetHelp $win.leftframe.collectedframe Budget.Collected

label $win.leftframe.collectedframe.label\
  -relief flat\
  -font [Font $win Large]\
  -text {Taxes Collected}

label $win.leftframe.collectedframe.value\
  -relief flat\
  -font [Font $win Medium]\
  -text {$0}
LinkWindow $win.collected $win.leftframe.collectedframe.value

pack append $win.leftframe.collectedframe\
  $win.leftframe.collectedframe.label   {top frame center fillx} \
  $win.leftframe.collectedframe.value   {top frame center expand fill} 

frame $win.leftframe.cashflowframe\
  -borderwidth 1\
  -relief raised
LinkWindow $win.cashflow $win.leftframe.cashflowframe.value

SetHelp $win.leftframe.cashflowframe Budget.Flow

label $win.leftframe.cashflowframe.label\
  -relief flat\
  -font [Font $win Large]\
  -text {Cash Flow}

label $win.leftframe.cashflowframe.value\
  -relief flat\
  -font [Font $win Medium]\
  -text {+$0}

pack append $win.leftframe.cashflowframe\
  $win.leftframe.cashflowframe.label   {top frame center fillx} \
  $win.leftframe.cashflowframe.value   {top frame center expand fill} 

frame $win.leftframe.previousframe\
  -borderwidth 1\
  -relief raised
LinkWindow $win.previous $win.leftframe.previousframe.value

SetHelp $win.leftframe.previousframe Budget.Previous

label $win.leftframe.previousframe.label\
  -relief flat\
  -font [Font $win Large]\
  -text {Previous Funds}

label $win.leftframe.previousframe.value\
  -relief flat\
  -font [Font $win Medium]\
  -text {$0}

pack append $win.leftframe.previousframe\
  $win.leftframe.previousframe.label   {top frame center fillx} \
  $win.leftframe.previousframe.value   {top frame center expand fill} 

frame $win.leftframe.currentframe\
  -borderwidth 1\
  -relief raised
LinkWindow $win.current $win.leftframe.currentframe.value

SetHelp $win.leftframe.currentframe Budget.Current

label $win.leftframe.currentframe.label\
  -relief flat\
  -font [Font $win Large]\
  -text {Current Funds}

label $win.leftframe.currentframe.value\
  -relief flat\
  -font [Font $win Medium]\
  -text {$0}

pack append $win.leftframe.currentframe\
  $win.leftframe.currentframe.label   {top frame center fillx} \
  $win.leftframe.currentframe.value   {top frame center expand fill} 

pack append $win.leftframe\
  $win.leftframe.collectedframe   {top frame center expand fill} \
  $win.leftframe.cashflowframe    {top frame center expand fill} \
  $win.leftframe.previousframe    {top frame center expand fill} \
  $win.leftframe.currentframe     {top frame center expand fill}

frame $win.bottomframe\
  -borderwidth 0

frame $win.bottomframe.voteframe \
  -borderwidth 0 \
  -relief raised
LinkWindow $win.voteframe $win.bottomframe.voteframe

button $win.bottomframe.voteframe.continue\
  -text {Continue With These Figures}\
  -font [Font $win Large]\
  -relief raised \
  -command "DoVote $win Budget BudgetContinue {}"
LinkWindow $win.vote $win.bottomframe.voteframe.continue

SetHelp $win.bottomframe.voteframe Budget.Continue

pack append $win.bottomframe.voteframe\
  $win.bottomframe.voteframe.continue	{top frame center fillx}

BindVotingButton $win $win.bottomframe.voteframe.continue Budget

button $win.bottomframe.reset\
  -text {Reset to Original Figures}\
  -font [Font $win Large]\
  -command "BudgetReset"

SetHelp $win.bottomframe.reset Budget.Reset

button $win.bottomframe.cancel\
  -text {Cancel Changes and Continue}\
  -font [Font $win Large]\
  -command "BudgetCancel"

SetHelp $win.bottomframe.cancel Budget.Cancel

button $win.bottomframe.timer\
  -text {Timeout in 0 seconds ...}\
  -font [Font $win Large]\
  -command "ToggleBudgetTimer"
LinkWindow $win.timer $win.bottomframe.timer

SetHelp $win.bottomframe.timer Budget.AutoCancel

button $win.bottomframe.autobudget\
  -text {Enable Auto Budget}\
  -font [Font $win Large]\
  -command "ToggleAutoBudget"
LinkWindow $win.autobudget $win.bottomframe.autobudget

SetHelp $win.bottomframe.autobudget Budget.AutoBudget

pack append $win.bottomframe\
  $win.bottomframe.voteframe	{top frame center expand fillx}\
  $win.bottomframe.reset	{top frame center expand fillx}\
  $win.bottomframe.cancel	{top frame center expand fillx}\
  $win.bottomframe.timer	{top frame center expand fillx}\
  $win.bottomframe.autobudget	{top frame center expand fillx}

pack append $win\
  $win.label        {top frame center fillx} \
  $win.bottomframe  {bottom frame center fillx} \
  $win.leftframe    {left frame center expand fill} \
  $win.rightframe   {right frame center expand fill}

sim UpdateBudget

update idletasks

