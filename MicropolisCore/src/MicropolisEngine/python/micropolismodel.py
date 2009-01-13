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
import micropolisengine
import gobject


########################################################################
# Globals


__version__ = "0.9"


########################################################################
# MicropolisModel Class


class MicropolisModel(micropolisengine.Micropolis):


    def __init__(
            self,
            running=True,
            timeDelay=50,
            *args,
            **kwargs):
        print "MicropolisModel.__init__", self, "calling micropolisengine.Micropolis.__init__", micropolisengine.Micropolis.__init__, args, kwargs

        micropolisengine.Micropolis.__init__(self, *args, **kwargs)

        self.running = running
        self.timeDelay = timeDelay
        self.timerActive = False
        self.timerId = None
        self.views = []
        self.graphs = []
        self.demands = []
        self.evaluations = []
        self.budgets = []
        self.evaluation = None
        self.interests = {}

        # NOTE: Because of a bug in SWIG, printing out the wrapped objects results in a crash.
        # So don't do that! I hope this bug in SWIG gets fixed. 
        # TODO: Report SWIG bug, if it's not already known or fixed. 

        # Hook the engine up so it has a handle on its Python object side. 
        self.userData = micropolisengine.GetPythonCallbackData(self)
        #print "USERDATA"#, self.userData

        # Hook up the language independent callback mechanism to our low level C++ Python dependent callback handler. 
        self.callbackHook = micropolisengine.GetPythonCallbackHook()
        #print "CALLBACKHOOK"#, self.callbackHook

        # Hook up the Python side of the callback handler, defined in our scripted subclass of the SWIG wrapper. 
        self._invokeCallback = self.invokeCallback # Cache to prevent GC
        self.callbackData = micropolisengine.GetPythonCallbackData(self._invokeCallback)
        #print "CALLBACKDATA"#, self.callbackData

        if self.running:
            self.startTimer()

        print "MicropolisModel.__init__ done", self


    def __del__(
        self):

        self.stopTimer()

        micropolisengine.Micropolis.__del__(self)


    def addView(self, view):
        self.views.append(view)


    def addGraph(self, graph):
        self.graphs.append(graph)


    def addDemand(self, demand):
        self.demands.append(demand)


    def addEvaluation(self, evaluation):
        self.evaluations.append(evaluation)


    def addBudget(self, budget):
        self.budgets.append(budget)


    def startTimer(
        self):
        
        if self.timerActive:
            return

        self.timerId = gobject.timeout_add(self.timeDelay, self.tickTimer)
        self.timerActive = True


    def stopTimer(
        self):

        # FIXME: Is there some way to immediately cancel self.timerId? 

        self.timerActive = False


    def tickTimer(
        self):

        if not self.timerActive:
            return False

        self.stopTimer()

        self.tickEngine()

        for view in self.views:
            view.tickActiveTool()

        for view in self.views:
            view.tickTimer()

        if self.running:
            self.startTimer()

        return False


    def tickEngine(self):

        self.simTick()
        #self.animateTiles()
        self.simUpdate()


    def sendUpdate(self, name, *args):
        interests = self.interests
        if name in interests:
            a = interests[name]
            for view in a:
                view.update(name, *args)


    def expressInterest(self, view, names):
        interests = self.interests
        for name in names:
            if name not in interests:
                a = []
                interests[name] = a
            else:
                a = interests[name]
            if view not in a:
                a.append(view)


    def revokeInterest(self, view, names):
        interests = self.interests
        for name in names:
            if name in interests:
                a = interest[name]
                if view in a:
                    a.remove(view)


    def invokeCallback(self, micropolis, name, *args):
        #print "==== MicropolisDrawingArea invokeCallback", "SELF", sys.getrefcount(self), self, "micropolis", sys.getrefcount(micropolis), micropolis, "name", name
        # In this case, micropolis is the same is self, so ignore it. 
        handler = getattr(self, 'handle_' + name, None)
        if handler:
            print "Calling handler", handler, args
            handler(*args)
            #print "Called handler."
        else:
            print "No handler for", name
    

    def __repr__(self):
        return "<MicropolisModel>"


    def handle_UIAutoGoto(self, x, y):
        print "handle_UIAutoGoto(self, x, y)", (self, x, y)
    

    def handle_UIDidGenerateNewCity(self):
        print "handle_UIDidGenerateNewCity(self)", (self,)

    
    def handle_UIDidLoadCity(self):
        print "handle_UIDidLoadCity(self)", (self,)

    
    def handle_UIDidLoadScenario(self):
        print "handle_UIDidLoadScenario(self)", (self,)

    
    def handle_UIDidSaveCity(self):
        print "handle_UIDidSaveCity(self)", (self,)

    
    def handle_UIDidTool(self, name, x, y):
        print "handle_UIDidTool(self, name, x, y)", (self, name, x, y)

    
    def handle_UIDidntLoadCity(self, msg):
        print "handle_UIDidntLoadCity(self, msg)", (self, msg)

    
    def handle_UIDidntSaveCity(self, msg):
        print "handle_UIDidntSaveCity(self, msg)", (self, msg)

    
    def handle_UIDoPendTool(self, tool, x, y):
        print "handle_DoPendTool(self, tool, x, y)", (self, tool, x, y)

    
    def handle_UIDropFireBombs(self):
        print "handle_DropFireBombs(self)", (self,)

    
    def handle_UIInitializeSound(self):
        print "handle_UIInitializeSound(self)", (self,)

    
    def handle_UILoseGame(self):
        print "handle_UILoseGame(self)", (self,)

    
    def handle_UIMakeSound(self, channel, sound, x, y):
        #print "handle_UIMakeSound(self, channel, sound, x, y)", (self, channel, sound, x, y)
        pass # print "SOUND", channel, sound, x, y

    
    def handle_UINewGame(self):
        print "handle_UINewGame(self)", (self,)

    
    def handle_UIPlayNewCity(self):
        print "handle_UIPlayNewCity(self)", (self,)

    
    def handle_UIPopUpMessage(self, msg):
        print "handle_UIPopUpMessage(self, msg)", (self, msg)

    
    def handle_UIReallyStartGame(self):
        print "handle_UIReallyStartGame(self)", (self,)

    
    def handle_UISaveCityAs(self):
        print "handle_UISaveCityAs(self)", (self,)

    
    def handle_UISetBudget(self, flowStr, previousStr, currentStr, collectedStr, tax):
        pass # print "handle_UISetBudget(self, flowStr, previousStr, currentStr, collectedStr, tax)", (self, flowStr, previousStr, currentStr, collectedStr, tax)

    
    def handle_UISetBudgetValues(self, roadGot, roadWant, roadPercent, policeGot, policeWant, policePercent, fireGot, fireWant, firePercent):
        pass # print "handle_UISetBudgetValues(self, roadGot, roadWant, roadPercent, policeGot, policeWant, policePercent, fireGot, fireWant, firePercent)", (self, roadGot, roadWant, roadPercent, policeGot, policeWant, policePercent, fireGot, fireWant, firePercent)

    
    def handle_UISetCityName(self, CityName):
        print "handle_UISetCityName(self, CityName)", (self, CityName)

    
    def handle_UISetDate(self, str, m, y):
        #print "handle_UISetDate(self, str, m, d)", (self, str, m, y)
        pass#print "DATE", str, m, y

    
    def handle_UISetDemand(self, r, c, i):
        #print "handle_UISetDemand(self, r, c, i)", (self, r, c, i)
        self.r = r
        self.c = c
        self.i = i
        for demand in self.demands:
            demand.update()

    
    def handle_UISetEvaluation(self, *args):
        #print "handle_UISetEvaluation(self, args)
        self.evaluation = args
        for evaluation in self.evaluations:
            evaluation.update()

    
    def handle_UISetFunds(self, funds):
        #print "handle_UISetFunds(self, funds)", (self, funds)
        pass # print "FUNDS", funds


    def handle_UISetGameLevel(self, GameLevel):
        print "handle_UISetGameLevel(self, GameLevel)", (self, GameLevel)

    
    def handle_UISetMapState(self, state):
        print "handle_UISetMapState(self, state)", (self, state)

    
    def handle_UISetMessage(self, str):
        #print "handle_UISetMessage(self, str)", (self, str)
        print "MESSAGE", str


    def handle_UISetOptions(self, autoBudget, gotoGoto, autoBulldoze, noDisasters, sound, doAnimation, doMessages, doNotices):
        print "handle_UISetOptions(self, autoBudget, gotoGoto, autoBulldoze, noDisasters, sound, doAnimation, doMessages, doNotices)", (self, autoBudget, gotoGoto, autoBulldoze, noDisasters, sound, doAnimation, doMessages, doNotices)

    def handle_UISetSpeed(self, speed):
        print "handle_UISetSpeed(self, speed)", (self, speed)

    
    def handle_UIShowBudgetAndWait(self):
        print "handle_UIShowBudgetAndWait(self)", (self,)

    
    def handle_UIShowPicture(self, id):
        #print "handle_UIShowPicture(self, id)", (self, id)
        print "SHOWPICTURE", id

    
    def handle_UIShowZoneStatus(self, str, s0, s1, s2, s3, s4, x, y):
        print "handle_UIShowZoneStatus(self, str, s0, s1, s2, s3, s4, x, y)", (self, str, s0, s1, s2, s3, s4, x, y)

    
    def handle_UIStartEarthquake(self, magnitude):
        print "handle_UIStartEarthquake(self, magnitude)", (self, magnitude,)

    
    def handle_UIStartLoad(self):
        print "handle_UIStartLoad(self)", (self,)

    
    def handle_UIStartScenario(self, scenario):
        print "handle_UIStartScenario(self, scenario)", (self, scenario)

    
    def handle_UIUpdateBudget(self):
        #print "handle_UIUpdateBudget(self)", (self,)
        for budget in self.budgets:
            budget.update()

    
    def handle_UIWinGame(self):
        print "handle_UIWinGame(self)", (self,)


    def handle_UINewGraph(self):
        #print "handle_UINewGraph(self)", (self,)
        for graph in self.graphs:
            graph.update()


    def handle_UIUpdate(self, name, *args):
        print "handle_UIUpdate(self, name, *args)", (self, name, args)

        self.sendUpdate(name, *args)


########################################################################


def CreateTestEngine():

    # Get our nice scriptable subclass of the SWIG Micropolis wrapper object. 
    engine = MicropolisModel()

    engine.resourceDir = 'res'
    engine.initGame()

    # Load a city file.
    cityFileName = 'cities/haight.cty'
    print "Loading city file:", cityFileName
    engine.loadFile(cityFileName)

    # Initialize the simulator engine.

    engine.resume()
    engine.setSpeed(2)
    engine.cityTax = 8
    engine.autoGoto = False
    engine.cityTax = 8

    # Testing...

    #engine.setSkips(1000)
    engine.setSkips(10)
    #engine.setSkips(0)
    engine.setFunds(1000000000)

    return engine


########################################################################
