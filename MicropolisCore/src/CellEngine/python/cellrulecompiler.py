# cellrulecompiler.py
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
# Cellular Automata Engine Rule Compiler, by Don Hopkins.


import sys, os, thread, time


########################################################################
# Make a rule table, by applying a function to all neighbor permutations.


def MakeRuleTable(ruleFn, neigh):
    ruleTableBits = len(neigh)
    ruleTableSize = 1 << ruleTableBits
    ruleBuf = ""
    state = {};
    for index in range(0, ruleTableSize):
        bit = 1
        for line in neigh:
            if (index & bit):
                state[line] = 1
            else:
                state[line] = 0
            bit *= 2
        ruleBuf += chr(ruleFn(state) & 255)
    return ruleBuf


########################################################################
# Neighborhoods


vonnNeumannNeighborhood = \
    ('c0', 'c1', 'e1', 'w1', 's1', 'n1', 'e0', 'w0', 's0', 'n0')
def MakeVonnNeumannRule(ruleFn):
    return (MakeRuleTable(ruleFn, vonnNeumannNeighborhood), 3)


mooreNeighborhood = \
    ('c0', 'c1', 'se0', 'sw0', 'ne0', 'nw0', 'e0', 'w0', 's0', 'n0', 'c2', 'c3')
def MakeMooreRule(ruleFn):
    return (MakeRuleTable(ruleFn, mooreNeighborhood), 1)


margolisNeighborhood = \
    ('c0', 'c1', 'cw0', 'ccw0', 'opp0', 'cw1', 'ccw1', 'opp1', 'pha0', 'pha1')
def MakeMargolisRule(ruleFn):
    return (MakeRuleTable(ruleFn, margolisNeighborhood), 5)


########################################################################
# Rule Utilities


def SumNeighbors(state, neighbors):
    sum = 0
    for key in neighbors:
        sum += state[key]
    return sum

def Sum8(state):
    return SumNeighbors(
        state, (
            'nw0', 'n0', 'ne0',
            'w0',        'e0',
            'sw0', 's0', 'se0'))


def Sum9(state):
    return SumNeighbors(
        state, (
            'nw0', 'n0', 'ne0',
            'w0',  'c0', 'e0',
            'sw0', 's0', 'se0'))


########################################################################
# Rules


def MakeLifeRule():
    def LifeRule(state):
        sum = Sum8(state)
        if (state['c0']):
            if (sum == 3) or (sum == 2):
                return 1
            else:
                return 0
        else:
            if (sum == 3):
                return 1
            else:
                return 0
    return MakeMooreRule(LifeRule)


def MakeBrainRule():
    def BrainRule(state):
        sum = Sum8(state)
        c0 = state['c0']
        c1 = state['c1']
        if ((c0 == 0) and (c1 == 0)):
            if (sum == 2):
                return 1
            else:
                return 0
        else:
            return (c0 * 2)
    return MakeMooreRule(BrainRule)


global glassTable
glassTable = (
  0, 1, 1, 1, 0, 0, 0, 0,
  0, 0, 0, 1, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 1, 0, 0,
  0, 1, 0, 0, 0, 1, 1, 1
)


def MakeHGlassDownRule():
    def HGlassDownRule(state):
        global glassTable
        index = \
            (state['e0'] << 4) | \
            (state['w0'] << 3) | \
            (state['s0'] << 2) | \
            (state['n0'] << 1) | \
            (state['c0'] << 0);
        return glassTable[index]
    return MakeVonnNeumannRule(HGlassDownRule)


def MakeHGlassUpRule():
    def HGlassUpRule(state):
        global glassTable
        index = \
            (state['w0'] << 4) | \
            (state['e0'] << 3) | \
            (state['n0'] << 2) | \
            (state['s0'] << 1) | \
            (state['c0'] << 0);
        return glassTable[index]
    return MakeVonnNeumannRule(HGlassUpRule)


def MakeHGlassRightRule():
    def HGlassRightRule(state):
        global glassTable
        index = \
            (state['n0'] << 4) | \
            (state['s0'] << 3) | \
            (state['e0'] << 2) | \
            (state['w0'] << 1) | \
            (state['c0'] << 0);
        return glassTable[index]
    return MakeVonnNeumannRule(HGlassRightRule)


def MakeHGlassLeftRule():
    def HGlassLeftRule(state):
        global glassTable
        index = \
            (state['s0'] << 4) | \
            (state['n0'] << 3) | \
            (state['w0'] << 2) | \
            (state['e0'] << 1) | \
            (state['c0'] << 0);
        return glassTable[index]
    return MakeVonnNeumannRule(HGlassLeftRule)


def MakeGasRule():
    def GasRule(state):
        def Collision(state):
            return (state['c0'] == state['opp0']) and \
                   (state['cw0'] == state['ccw0']) and \
                   (state['c0'] != state['cw0'])
        def Wall(state):
            return state['c1'] or \
                   state['opp1'] or \
                   state['cw1'] or \
                   state['ccw1']
        plane1 = 2 * state['c1']
        if Wall(state) or Collision(state):
            return state['c0'] + plane1
        else:
            if state['pha0']:
                return state['cw0'] + plane1
            else:
                return state['ccw0'] + plane1
    return MakeMargolisRule(GasRule)


def MakeDendriteTableRule():
    def DendriteRule(state):
        def Collision(state):
                return (state['c0'] == state['opp0']) and \
                       (state['cw0'] == state['ccw0']) and \
                       (state['c0'] != state['cw0'])
        def Wall(state):
            return state['c1'] or \
                   state['opp1'] or \
                   state['cw1'] or \
                   state['ccw1']
        plane1 = state['c1']
        if (plane1):
            plane0 = 1
        elif Wall(state):
            plane0 = state['c0']
            plane1 |= 1 # state['c0']
        elif Collision(state):
            plane0 = state['c0']
        else:
            if state['pha0']:
                plane0 = state['cw0']
            else:
                plane0 = state['ccw0']
        return plane0 + (plane1 * 2)
    return MakeMargolisRule(DendriteRule)


def MakeDendriteRule():
    return ('', 48);


########################################################################
