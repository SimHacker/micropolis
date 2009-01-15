# micropolisevaluationview.py
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


########################################################################
# Micropolis Evaluation View
# Don Hopkins


# Is The Mayor Doing a Good Job?
#   Yes: XX%
#   No: XX%
# What are the Worst Problems?
#   Problem 1: XXX%
#   Problem 2: XXX%
#   Problem 3: XXX%
#   Problem 4: XXX%
# Statistics
#   Population: XXX
#       Net Migration: XXX (last year)
#       Assessed Value: XXX
#       Category: XXX
#       Game Level: XXX
# Overall City Score (0 - 1000)
#   Current Score: XXX
#   Annual Change: XXX


########################################################################
# Import stuff


import gtk
import cairo
import pango
import micropolisengine
import micropolisview


########################################################################
# MicropolisEvaluationView


class MicropolisEvaluationView(micropolisview.MicropolisView):


    def __init__(
        self,
        **args):

        micropolisview.MicropolisView.__init__(
            self,
            aspect='evaluation',
            interests=('city', 'evaluation',),
            **args)


    def update(
        self,
        name,
        *args):

        #print "EVALUATION UPDATE", self, name, args

        engine = self.engine

        self.queue_draw()


    def drawContent(
        self,
        ctx,
        playout):

        print "==== MicropolisEvaluationView DRAWCONTENT", self

        winRect = self.get_allocation()
        winWidth = winRect.width
        winHeight = winRect.height

        ctx.save()

        ctx.set_source_rgb(1.0, 1.0, 1.0)
        ctx.rectangle(0, 0, winWidth, winHeight)
        ctx.fill()

        yesPercent = 50
        noPercent = 50
        problem1 = "Problem 1: 20%"
        problem2 = "Problem 2: 15%"
        problem3 = "Problem 3: 10%"
        problem4 = "Problem 4: 5%"
        population = '10,000'
        netMigration = '1,000'
        assessedValue = '$100,000'
        category = 'Town'
        gameLevel = 'Medium'
        currentScore = '800'
        annualChange = '+30'

        markup = """<span>
  <b>Is The Mayor Doing a Good Job?</b><br/>
    Yes: %s%%<br/>
    No: %s%%<br/>
  <b>What are the Worst Problems?</b><br/>
    %s<br/>
    %s<br/>
    %s<br/>
    %s<br/>
  <b>Statistics</b><br/>
    Population: %s<br/>
        Net Migration: %s (last year)<br/>
        Assessed Value: %s<br/>
        Category: %s<br/>
        Game Level: %s<br/>
  <b>Overall City Score (0 - 1000)</b><br/>
    Current Score: %s<br/>
    Annual Change: %s<br/>
</span>
""" % (
            yesPercent,
            noPercent,
            problem1,
            problem2,
            problem3,
            problem4,
            population,
            netMigration,
            assessedValue,
            category,
            gameLevel,
            currentScore,
            annualChange,
        )

        print markup

        playout.set_font_description(self.labelFont)
        playout.set_text(markup)
        playout.set_markup(markup)
        ctx.set_source_rgb(0.0, 0.0, 0.0)
        ctx.move_to(10, 10)
        ctx.show_layout(playout)


########################################################################
