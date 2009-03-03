# micropolisturbogearsengine.py
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
# Micropolis TurboGears Engine
# Don Hopkins


########################################################################
# Import stuff


import micropolisgenericengine


########################################################################
# MicropolisTurboGearsEngine Class


class MicropolisTurboGearsEngine(micropolisgenericengine.MicropolisGenericEngine):


    def __init__(
            self,
            *args,
            **kw):

        super(MicropolisTurboGearsEngine, self).__init__(*args, **kw)

        self.sessions = []


    def startTimer(
        self):

        pass


    def stopTimer(
        self):

        pass


    def addSession(self, session):
        sessions = self.sessions
        if session not in sessions:
            self.sessions.append(session)


    def removeSession(self, session):
        sessions = self.sessions
        if session in sessions:
            self.sessions.remove(session)


    def sendSessions(self, message):
        for session in self.sessions:
            session.sendMessage(message)


    def handle_UIAutoGoto(self, x, y):
        print "handle_UIAutoGoto(self, x, y)", (self, x, y)
        self.sendSessions({
            'message': "UIAutoGoto",
            'x': x,
            'y': y,
        })
    

    def handle_UIDidGenerateNewCity(self):
        print "handle_UIDidGenerateNewCity(self)", (self,)
        self.sendSessions({
            'message': "UIDidGenerateNewCity",
        })

    
    def handle_UIDidLoadCity(self):
        print "handle_UIDidLoadCity(self)", (self,)
        self.sendSessions({
            'message': "UIDidLoadCity",
        })

    
    def handle_UIDidLoadScenario(self):
        print "handle_UIDidLoadScenario(self)", (self,)
        self.sendSessions({
            'message': "UIDidLoadScenario",
        })

    
    def handle_UIDidSaveCity(self):
        print "handle_UIDidSaveCity(self)", (self,)
        self.sendSessions({
            'message': "UIDidSaveCity",
        })

    
    def handle_UIDidTool(self, name, x, y):
        print "handle_UIDidTool(self, name, x, y)", (self, name, x, y)
        self.sendSessions({
            'message': "UIDidTool",
            'name': name,
            'x': x,
            'y': y,
        })

    
    def handle_UIDidntLoadCity(self, msg):
        print "handle_UIDidntLoadCity(self, msg)", (self, msg)
        self.sendSessions({
            'message': "UIDidntLoadCity",
            'msg': msg,
        })

    
    def handle_UIDidntSaveCity(self, msg):
        print "handle_UIDidntSaveCity(self, msg)", (self, msg)
        self.sendSessions({
            'message': "UIDidntSaveCity",
            'msg': msg,
        })

    
    def handle_UILoseGame(self):
        print "handle_UILoseGame(self)", (self,)
        self.sendSessions({
            'message': "UILoseGame",
        })

    
    def handle_UIMakeSound(self, channel, sound, x, y):
        print "handle_UIMakeSound(self, channel, sound)", (self, channel, sound, x, y)
        self.sendSessions({
            'message': "UIMakeSound",
            'channel': channel,
            'sound': sound,
            'x': x,
            'y': y,
        })

    
    def handle_UINewGame(self):
        print "handle_UINewGame(self)", (self,)
        self.sendSessions({
            'message': "UINewGame",
        })

    
    def handle_UIPlayNewCity(self):
        print "handle_UIPlayNewCity(self)", (self,)
        self.sendSessions({
            'message': "UIPlayNewCity",
        })

    
    def handle_UIReallyStartGame(self):
        print "handle_UIReallyStartGame(self)", (self,)
        self.sendSessions({
            'message': "UIReallyStartGame",
        })

    
    def handle_UISaveCityAs(self):
        print "handle_UISaveCityAs(self)", (self,)
        self.sendSessions({
            'message': "UISaveCityAs",
        })

    
    def handle_UIShowBudgetAndWait(self):
        print "handle_UIShowBudgetAndWait(self)", (self,)
        self.sendSessions({
            'message': "UIShowBudgetAndWait",
        })

    
    def handle_UIShowPicture(self, id):
        print "handle_UIShowPicture(self, id)", (self, id)
        self.sendSessions({
            'message': "UIShowPicture",
            'id': id,
        })

    
    def handle_UIShowZoneStatus(self, str, s0, s1, s2, s3, s4, x, y):
        print "handle_UIShowZoneStatus(self, str, s0, s1, s2, s3, s4, x, y)", (self, str, s0, s1, s2, s3, s4, x, y)
        self.sendSessions({
            'message': "UIShowZoneStatus",
            'str': str,
            's0': s0,
            's1': s1,
            's2': s2,
            's3': s3,
            's4': s4,
            'x': x,
            'y': y,
        })

    
    def handle_UIStartEarthquake(self, magnitude):
        print "handle_UIStartEarthquake(self, magnitude)", (self, magnitude,)
        self.sendSessions({
            'message': "UIStartEarthquake",
            'magnitude': magnitude,
        })

    
    def handle_UIStartLoad(self):
        print "handle_UIStartLoad(self)", (self,)
        self.sendSessions({
            'message': "UIStartLoad",
        })

    
    def handle_UIStartScenario(self, scenario):
        print "handle_UIStartScenario(self, scenario)", (self, scenario)
        self.sendSessions({
            'message': "UIStartScenario",
            'scenario': scenario,
        })

    
    def handle_UIStopEarthquake(self):
        print "handle_UIStopEarthquake(self)", (self,)
        self.sendSessions({
            'message': "UIStopEarthquake",
        })


    def handle_UIWinGame(self):
        print "handle_UIWinGame(self)", (self,)
        self.sendSessions({
            'message': "UIWinGame",
        })


    def handle_UIUpdate(self, aspect):
        print "handle_UIUpdate(self, aspect)", (self, aspect)

        if aspect == "funds":

            self.sendSessions({
                'message': "UIUpdateFunds",
                'funds': self.totalFunds,
            })

        elif aspect == "date":

            self.sendSessions({
                'message': "UIUpdateDate",
                'cityTime': self.cityTime,
            })

        elif aspect == "graph":

            self.sendSessions({
                'message': "UIUpdateGraph",
                # TODO
            })

        elif aspect == "evaluation":

            problems = []
            for i in range(0, self.countProblems()):
                problems.append((
                    self.getProblemNumber(i),
                    self.getProblemVotes(i)))

            self.sendSessions({
                'message': "UIUpdateEvaluation",
                'currentYear': self.currentYear(),
                'cityYes': self.cityYes,
                'cityScore': self.cityScore,
                'deltaCityScore': self.cityScoreDelta,
                'cityPop': self.cityPop,
                'deltaCityPop': self.deltaCityPop,
                'cityAssessedValue': self.cityAssessedValue,
                'cityClass': self.cityClass,
                'gameLevel': self.gameLevel,
                'problems': problems,
            })

        else:
            print "UPDATE unknown aspect", aspect



########################################################################


def CreateTurboGearsEngine(**params):

    # Get our nice scriptable subclass of the SWIG Micropolis wrapper object.
    engine = MicropolisTurboGearsEngine(**params)

    return engine


########################################################################
