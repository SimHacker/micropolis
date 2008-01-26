# Window: Player evaluation, for Unix Micropolis.
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
global EvaluationWindows
set n [Unique]
set parent [WindowLink $head.col2]
set win $parent.evaluation$n
set EvaluationWindows [linsert $EvaluationWindows 0 $win]

LinkWindow $head.evaluation $win
LinkWindow $win.head $head
LinkWindow $win.parent $parent
LinkWindow $win.top $win

catch "destroy $win"
frame $win

SetHelp $win Evaluation

global [set var $win.visible]
set $var 0

bind $win <Visibility> {EvaluationVisible %W [string compare %s FullyObscured]}
bind $win <Map> {EvaluationVisible %W 1}
bind $win <Unmap> {EvaluationVisible %W 0}

#wm title $win {Micropolis Evaluation}
#wm iconname $win {Micropolis Evaluation}
#wm group $win $head
#wm geometry $win 480x240+5-5
#wm withdraw $win
#wm maxsize $win 700 700
#wm minsize $win 400 200
#wm protocol $win delete "DeleteWindow evaluation EvaluationWindows"


frame $win.leftframe\
  -borderwidth 1\
  -relief raised

SetHelp $win.leftframe Evaluation.Opinion

label $win.leftframe.title\
  -font [Font $win Big]\
  -padx 0\
  -relief flat\
  -text {Public Opinion}\
  -width 5

label $win.leftframe.isthe\
  -font [Font $win Large]\
  -text {Is the mayor doing a good job?}

SetHelp $win.leftframe.isthe Evaluation.GoodJob

frame $win.leftframe.goodjobframe\
  -borderwidth 1

SetHelp $win.leftframe.goodjobframe Evaluation.GoodJob

message $win.leftframe.goodjobframe.yesno\
  -aspect 1500\
  -justify right\
  -padx 5\
  -pady 2\
  -font [Font $win Medium]\
  -text {YES
NO}

message $win.leftframe.goodjobframe.percents\
  -aspect 1500\
  -justify left\
  -padx 5\
  -pady 2\
  -font [Font $win Medium]\
  -text {100%
0%}
LinkWindow $win.goodjob $win.leftframe.goodjobframe.percents

pack append $win.leftframe.goodjobframe\
  $win.leftframe.goodjobframe.percents	{right frame w expand} \
  $win.leftframe.goodjobframe.yesno		{right frame e expand} 

label $win.leftframe.whatare\
  -font [Font $win Large]\
  -text {What are the worst problems?}

SetHelp $win.leftframe.whatare Evaluation.WorstProblems

frame $win.leftframe.worstproblems\
  -borderwidth 1

SetHelp $win.leftframe.worstproblems Evaluation.WorstProblems

message $win.leftframe.worstproblems.problemnames\
  -aspect 1500\
  -justify right\
  -padx 5\
  -pady 2\
  -font [Font $win Medium]\
  -text {        



}
LinkWindow $win.problemnames $win.leftframe.worstproblems.problemnames

message $win.leftframe.worstproblems.problempercents\
  -aspect 1500\
  -justify left\
  -padx 5\
  -pady 2\
  -font [Font $win Medium]\
  -text {        



}
LinkWindow $win.problempercents $win.leftframe.worstproblems.problempercents

pack append $win.leftframe.worstproblems\
  $win.leftframe.worstproblems.problempercents {right frame w expand} \
  $win.leftframe.worstproblems.problemnames    {right frame e expand}

pack append $win.leftframe\
  $win.leftframe.title		{top frame center fillx expand} \
  $win.leftframe.isthe		{top frame center fillx expand} \
  $win.leftframe.goodjobframe	{top frame center fillx expand} \
  $win.leftframe.whatare	{top frame center fillx expand} \
  $win.leftframe.worstproblems	{top frame center fillx expand} 

frame $win.rightframe\
  -borderwidth 1\
  -relief raised

SetHelp $win.rightframe Evaluation.Statistics

label $win.rightframe.title\
  -font [Font $win Big]\
  -padx 0\
  -relief flat\
  -text {Statistics}\
  -width 5

frame $win.rightframe.statsframe\
  -borderwidth 1

message $win.rightframe.statsframe.statlabels\
  -aspect 1500\
  -justify right\
  -padx 5\
  -pady 2\
  -font [Font $win Medium]\
  -text {Population:
Net Migration:
(last year) 
Assessed Value:
Category:
Game Level:}

message $win.rightframe.statsframe.stats\
  -aspect 1500\
  -justify left\
  -padx 5\
  -pady 2\
  -font [Font $win Medium]\
  -text {                




}
LinkWindow $win.stats $win.rightframe.statsframe.stats

pack append $win.rightframe.statsframe\
  $win.rightframe.statsframe.statlabels	{left frame e expand} \
  $win.rightframe.statsframe.stats	{right frame w expand}

message $win.rightframe.cityscore\
  -aspect 1500\
  -justify center\
  -padx 5\
  -pady 2\
  -font [Font $win Medium]\
  -text {Overall City Score
(0 - 1000)}

SetHelp $win.rightframe.cityscore Evaluation.Score

frame $win.rightframe.scoreframe\
  -borderwidth 1

SetHelp $win.rightframe.scoreframe Evaluation.Score

message $win.rightframe.scoreframe.scorelabels\
  -aspect 1500\
  -justify right\
  -padx 5\
  -pady 2\
  -font [Font $win Medium]\
  -text {Current Score:
Annual Change:}

message $win.rightframe.scoreframe.scores\
  -aspect 1500\
  -justify left\
  -padx 5\
  -pady 2\
  -font [Font $win Medium]\
  -text {        

}
LinkWindow $win.score $win.rightframe.scoreframe.scores

pack append $win.rightframe.scoreframe\
  $win.rightframe.scoreframe.scorelabels	{left frame e expand} \
  $win.rightframe.scoreframe.scores		{right frame w expand}

pack append $win.rightframe\
  $win.rightframe.title	{top frame center fillx expand} \
  $win.rightframe.statsframe	{top frame center fillx expand} \
  $win.rightframe.cityscore	{top frame center fillx expand} \
  $win.rightframe.scoreframe	{top frame center fillx expand} 

frame $win.bottomframe\
  -borderwidth 0

SetHelp $win.bottomframe Evaluation.Dismiss

button $win.bottomframe.button\
  -font [Font $win Large]\
  -text {Dismiss Evaluation}\
  -command "pack unpack $win"

pack append $win.bottomframe\
  $win.bottomframe.button   {top frame center expand fillx}

pack append $win\
  $win.bottomframe	{bottom frame center fillx}\
  $win.leftframe	{left frame center expand fill} \
  $win.rightframe	{right frame center expand fill}

sim UpdateEvaluation

update idletasks
