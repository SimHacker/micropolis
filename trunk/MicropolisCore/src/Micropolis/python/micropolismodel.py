# micropolismodel.py
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
# Micropolis Model
# Don Hopkins


########################################################################
# Import stuff


import sys
import os
import time
import micropolis


########################################################################
# Globals


__version__ = "0.9"


########################################################################
# MicropolisModel Class


class MicropolisModel(micropolis.Micropolis):


    def __init__(
            self,
            *args,
            **kwargs):
        print "MicropolisModel.__init__", self, "calling micropolis.Micropolis.__init__", micropolis.Micropolis.__init__, args, kwargs
        micropolis.Micropolis.__init__(self, *args, **kwargs)
        print "MicropolisModel.__init__ done", self


    def invokeCallback(self, micropolis, name, *params):
        #print "MicropolisDrawingArea invokeCallback", "micropolis", micropolis, "name", name, "params", params
        handler = getattr(self, 'handle_' + name, None)
        if handler:
            handler(micropolis, *params)
        else:
            print "No handler for", name
    

    def handle_UIAutoGoto(self, micropolis, x, y):
        print "handle_UIAutoGoto(self, micropolis, x, y)", (self, micropolis, x, y)
    

    def handle_UIDidGenerateNewCity(self, micropolis):
        print "handle_UIDidGenerateNewCity(self, micropolis)", (self, micropolis)

    
    def handle_UIDidLoadCity(self, micropolis):
        print "handle_UIDidLoadCity(self, micropolis)", (self, micropolis)

    
    def handle_UIDidLoadScenario(self, micropolis):
        print "handle_UIDidLoadScenario(self, micropolis)", (self, micropolis)

    
    def handle_UIDidSaveCity(self, micropolis):
        print "handle_UIDidSaveCity(self, micropolis)", (self, micropolis)

    
    def handle_UIDidTool(self, micropolis, name, x, y):
        print "handle_UIDidTool(self, micropolis, name, x, y)", (self, micropolis, name, x, y)

    
    def handle_UIDidntLoadCity(self, micropolis, msg):
        print "handle_UIDidntLoadCity(self, micropolis, msg)", (self, micropolis, msg)

    
    def handle_UIDidntSaveCity(self, micropolis, msg):
        print "handle_UIDidntSaveCity(self, micropolis, msg)", (self, micropolis, msg)

    
    def handle_UIDoPendTool(self, micropolis, tool, x, y):
        print "handle_DoPendTool(self, micropolis, tool, x, y)", (self, micropolis, tool, x, y)

    
    def handle_UIDropFireBombs(self, micropolis):
        print "handle_DropFireBombs(self, micropolis)", (self, micropolis)

    
    def handle_UIInitializeSound(self, micropolis):
        print "handle_UIInitializeSound(self, micropolis)", (self, micropolis)

    
    def handle_UILoseGame(self, micropolis):
        print "handle_UILoseGame(self, micropolis)", (self, micropolis)

    
    def handle_UIMakeSound(self, micropolis, channel, sound):
        print "handle_UIMakeSound(self, micropolis, channel, sound)", (self, micropolis, channel, sound)

    
    def handle_UINewGame(self, micropolis):
        print "handle_UINewGame(self, micropolis)", (self, micropolis)

    
    def handle_UIPlayNewCity(self, micropolis):
        print "handle_UIPlayNewCity(self, micropolis)", (self, micropolis)

    
    def handle_UIPopUpMessage(self, micropolis, msg):
        print "handle_UIPopUpMessage(self, micropolis, msg)", (self, micropolis, msg)

    
    def handle_UIReallyStartGame(self, micropolis):
        print "handle_UIReallyStartGame(self, micropolis)", (self, micropolis)

    
    def handle_UISaveCityAs(self, micropolis):
        print "handle_UISaveCityAs(self, micropolis)", (self, micropolis)

    
    def handle_UISetBudget(self, micropolis, flowStr, previousStr, currentStr, collectedStr, tax):
        print "handle_UISetBudget(self, micropolis, flowStr, previousStr, currentStr, collectedStr, tax)", (self, micropolis, flowStr, previousStr, currentStr, collectedStr, tax)

    
    def handle_UISetBudgetValues(self, micropolis, roadGot, roadWant, roadPercent, policeGot, policeWant, policePercent, fireGot, fireWant, firePercent):
        print "handle_UISetBudgetValues(self, micropolis, roadGot, roadWant, roadPercent, policeGot, policeWant, policePercent, fireGot, fireWant, firePercent)", (self, micropolis, roadGot, roadWant, roadPercent, policeGot, policeWant, policePercent, fireGot, fireWant, firePercent)

    
    def handle_UISetCityName(self, micropolis, CityName):
        print "handle_UISetCityName(self, micropolis, CityName)", (self, micropolis, CityName)

    
    def handle_UISetDate(self, micropolis, str, m, y):
        print "handle_UISetDate(self, micropolis, str, m, d)", (self, micropolis, str, m, y)

    
    def handle_UISetDemand(self, micropolis, r, c, i):
        print "handle_UISetDemand(self, micropolis, r, c, i)", (self, micropolis, r, c, i)

    
    def handle_UISetEvaluation(self, micropolis, changed, score, ps0, ps1, ps2, ps3, pv0, pv1, pv2, pv3, pop, delta, assessed_dollars, cityclass, citylevel, goodyes, goodno, title):
        print "handle_UISetEvaluation(self, micropolis, changed, score, ps0, ps1, ps2, ps3, pv0, pv1, pv2, pv3, pop, delta, assessed_dollars, cityclass, citylevel, goodyes, goodno, title)", (self, micropolis, changed, score, ps0, ps1, ps2, ps3, pv0, pv1, pv2, pv3, pop, delta, assessed_dollars, cityclass, citylevel, goodyes, goodno, title)

    
    def handle_UISetFunds(self, micropolis, funds):
        print "handle_UISetFunds(self, micropolis, funds)", (self, micropolis, funds)

    
    def handle_UISetGameLevel(self, micropolis, GameLevel):
        print "handle_UISetGameLevel(self, micropolis, GameLevel)", (self, micropolis, GameLevel)

    
    def handle_UISetMapState(self, micropolis, state):
        print "handle_UISetMapState(self, micropolis, state)", (self, micropolis, state)

    
    def handle_UISetMessage(self, micropolis, str):
        print "handle_UISetMessage(self, micropolis, str)", (self, micropolis, str)


    def handle_UISetOptions(self, autoBudget, gotoGoto, autoBulldoze, noDisasters, sound, doAnimation, doMessages, doNotices):
        print "handle_UISetOptions(self, autoBudget, gotoGoto, autoBulldoze, noDisasters, sound, doAnimation, doMessages, doNotices)", (self, autoBudget, gotoGoto, autoBulldoze, noDisasters, sound, doAnimation, doMessages, doNotices)


    def handle_UISetSpeed(self, micropolis, speed):
        print "handle_UISetSpeed(self, micropolis, speed)", (self, micropolis, speed)

    
    def handle_UIShowBudgetAndWait(self, micropolis):
        print "handle_UIShowBudgetAndWait(self, micropolis)", (self, micropolis)

    
    def handle_UIShowPicture(self, micropolis, id):
        print "handle_UIShowPicture(self, micropolis, id)", (self, micropolis, id)

    
    def handle_UIShowZoneStatus(self, micropolis, str, s0, s1, s2, s3, s4, x, y):
        print "handle_UIShowZoneStatus(self, micropolis, str, s0, s1, s2, s3, s4, x, y)", (self, micropolis, str, s0, s1, s2, s3, s4, x, y)

    
    def handle_UIStartEarthquake(self, micropolis):
        print "handle_UIStartEarthquake(self, micropolis)", (self, micropolis)

    
    def handle_UIStartLoad(self, micropolis):
        print "handle_UIStartLoad(self, micropolis)", (self, micropolis)

    
    def handle_UIStartScenario(self, micropolis, scenario):
        print "handle_UIStartScenario(self, micropolis, scenario)", (self, micropolis, scenario)

    
    def handle_UIStopEarthquake(self, micropolis):
        print "handle_UIStopEarthquake(self, micropolis)", (self, micropolis)

    
    def handle_UIUpdateBudget(self, micropolis):
        print "handle_UIUpdateBudget(self, micropolis)", (self, micropolis)

    
    def handle_UIWinGame(self, micropolis):
        print "handle_UIWinGame(self, micropolis)", (self, micropolis)


########################################################################
