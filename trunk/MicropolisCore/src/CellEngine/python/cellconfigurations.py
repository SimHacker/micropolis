# cellconfigurations.py
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
# von Neumann 29 State Cellular Automata Rule Configurations
# Don Hopkins


NameToCode = {

# These are masks that don't directly represent states. 
#  'CN':   0x10,       # 00010000: confluent state
#  'OS':   0x20,       # 00100000: ordinary state
#  'SS':   0x40,       # 01000000: special state
#  'EX':   0x80,       # 10000000: excited state
#  'NX':   0x7f,       # 01111111: not excited mask
#  'RD':   0x00,       # 00000000: right direction
#  'UD':   0x01,       # 00000001: up direction
#  'LD':   0x02,       # 00000010: left direction
#  'DD':   0x03,       # 00000011: down direction
#  'DM':   0x03,       # 00000011: direction mask

  'U':    0x00,       # 00000000: unexcited state

  'S':    0x01,       # 00000001: sensitized state S
  'S0':   0x02,       # 00000010: sensitized state S0
  'S1':   0x03,       # 00000011: sensitized state S1
  'S00':  0x04,       # 00000100: sensitized state S00
  'S01':  0x05,       # 00000101: sensitized state S01
  'S10':  0x06,       # 00000110: sensitized state S10
  'S11':  0x07,       # 00000111: sensitized state S11
  'S000': 0x08,       # 00001000: sensitized state S000

  'C00':  0x10,       # 00010000: confluent state C00, now quiescent, next quiescent
  'C10':  0x11,       # 00010001: confluent state C10, now excited, next quiescent
  'C01':  0x90,       # 10010000: confluent state C01, now quiescent, next excited
  'C11':  0x91,       # 10010001: confluent state C11, now excited, next excited

  'OR':   0x20,       # 00100000: unexcited ordinary transmission state right
  'OU':   0x21,       # 00100001: unexcited ordinary transmission state up
  'OL':   0x22,       # 00100010: unexcited ordinary transmission state left
  'OD':   0x23,       # 00100011: unexcited ordinary transmission state down

  'SR':   0x40,       # 01000000: unexcited special transmission state right
  'SU':   0x41,       # 01000001: unexcited special transmission state up
  'SL':   0x42,       # 01000010: unexcited special transmission state left
  'SD':   0x43,       # 01000011: unexcited special transmission state down

  'ORX':  0xa0,       # 10100000: excited ordinary transmission state right
  'OUX':  0xa1,       # 10100001: excited ordinary transmission state up
  'OLX':  0xa2,       # 10100010: excited ordinary transmission state left
  'ODX':  0xa3,       # 10100011: excited ordinary transmission state down

  'SRX':  0xc0,       # 11000000: excited special transmission state right
  'SUX':  0xc1,       # 11000001: excited special transmission state up
  'SLX':  0xc2,       # 11000010: excited special transmission state left
  'SDX':  0xc3,       # 11000011: excited special transmission state down

}


CodeToName = {}
for name, code in NameToCode.iteritems():
    CodeToName[code] = name


NameToChar = {
  'U':    '-',

  'S':    '1',
  'S0':   '2',
  'S1':   '3',
  'S00':  '4',
  'S01':  '5',
  'S10':  '6',
  'S11':  '7',
  'S000': '8',

  'C00':  'q',
  'C10':  'e',
  'C01':  't',
  'C11':  'u',

  'OR':   's',
  'OU':   'w',
  'OL':   'a',
  'OD':   'z',

  'SR':   'f',
  'SU':   'r',
  'SL':   'd',
  'SD':   'c',

  'ORX':  'h',
  'OUX':  'y',
  'OLX':  'g',
  'ODX':  'b',

  'SRX':  'k',
  'SUX':  'i',
  'SLX':  'j',
  'SDX':  'm',

}


CharToName = {}
CharToCode = {}
for name, char in NameToChar.iteritems():
    CharToName[char] = name
    CharToCode[char] = NameToCode[name]


NameToDescription = {

  'CN':   'confluent',
  'OS':   'ordinary',
  'SS':   'special',
  'EX':   'excited',
  'NX':   'not excited mask',
  'RD':   'right direction',
  'UD':   'up direction',
  'LD':   'left direction',
  'DD':   'down direction',
  'DM':   'direction mask',

  'U':    'unexcited',

  'S':    'sensitized S',
  'S0':   'sensitized S0',
  'S1':   'sensitized S1',
  'S00':  'sensitized S00',
  'S01':  'sensitized S01',
  'S10':  'sensitized S10',
  'S11':  'sensitized S11',
  'S000': 'sensitized S000',

  'C00':  'confluent C00, quiescent, quiescent',
  'C10':  'confluent C10, excited, quiescent',
  'C01':  'confluent C01, quiescent, excited',
  'C11':  'confluent C11, excited, excited',

  'OR':   'unexcited ordinary right',
  'OU':   'unexcited ordinary up',
  'OL':   'unexcited ordinary left',
  'OD':   'unexcited ordinary down',

  'SR':   'unexcited special right',
  'SU':   'unexcited special up',
  'SL':   'unexcited special left',
  'SD':   'unexcited special down',

  'ORX':  'excited ordinary right',
  'OUX':  'excited ordinary up',
  'OLX':  'excited ordinary left',
  'ODX':  'excited ordinary down',

  'SRX':  'excited special right',
  'SUX':  'excited special up',
  'SLX':  'excited special left',
  'SDX':  'excited special down',

}


CodeToFrame = {

  0x00: 0,

  0x01: 1,
  0x02: 2,
  0x03: 3,
  0x04: 4,
  0x05: 5,
  0x06: 6,
  0x07: 7,
  0x08: 8,

  0x10: 9,
  0x11: 10,
  0x90: 11,
  0x91: 12,

  0x20: 13,
  0x21: 14,
  0x22: 15,
  0x23: 16,

  0x40: 17,
  0x41: 18,
  0x42: 19,
  0x43: 20,

  0xa0: 21,
  0xa1: 22,
  0xa2: 23,
  0xa3: 24,

  0xc0: 25,
  0xc1: 26,
  0xc2: 27,
  0xc3: 28,

}


FrameToCode = {}
for code, frame in CodeToFrame.iteritems():
    FrameToCode[frame] = code


Configurations = [

  {
    'name': "Real Time Crossing" ,
    'neighborhood': 52,
    'rows': 16,
    'cols': 16,
    'description': "This real time crossing is not easily constructible, but nonetheless here it is.",
    'state': """
sssssw-----sbhb-
yggggggggg-wbyb-
hhhhhhhhhy-wbyb-
yggggggggg-wbyb-
------sssw-wbyb-
------w----wbyb-
------w----wbyb-
--ezshqaez-wbyb-
--yty--wyt-wbyb-
--shqshqsb-wbyb-
ssq-wezw-z-wbybs
--z-wyty-qswbybw
--shqshqsy--bybw
--ezw--wez--bybw
--ytyqswyt--bybw
-----w------hyhw
""", },

  {
    'name': "Coded Channel Crossing",
    'neighborhood': 52,
    'rows': 8,
    'cols': 64,
    'description': "Coded channel crossings have interference problems, demonstrated here.",
    'state': """
-zaaaaaaa-----qsssesqsq-----------qhshqsssqsqsqsq---------------
-z------w-----w---w-z-z-----------w-------w-z---z---------------
-sshhsssqsssssqsqsq-ssssssssssssqsusqhesqsq-ssssshshssssssssssq-
--------------------------w-----z-------------------------------
zaagagaaa-thssqsssqsqsqsq-w-----b-tsssqsqsq---------------------
z-------w-w-------w-z---b-w-----b-w---w-z-z---------------------
ssssssssqsqsthqsqsq-ssssshw-----ssestsq-sshhsssssssssssssssssse-
----------------------------------------------------------------
""", },

  {
    'name': "Autoinitializing Exclusive Or",
    'neighborhood': 52,
    'rows': 28,
    'cols': 40,
    'description': "William R. Buckley's autoinitializing exclusive-or is intialized by 11111 at each input.",
    'state': """
sstsb-sqqssssssssssssssssssssssssz--szsz
y-z-b-wsw------------------------z--wzwz
w-b-b-w-sqqsqsssssssqfqssqsssssz-z--wzwz
y-z-b-qawsw----ssz--wsw--wsz---z-z--wzwz
w-b-b-qww-----sq-sqzqqz--wqaa--z-z--wzwz
y-z-z-w-qsq---wsqr-swwassz--w--z-z--wzwz
w-b-z-qaw-zcqsqsqsqssqsq-ssqw--z-z--wzwz
y-z-z-qww-zfw-zsqc-szzasqr-szssz-z--wzwz
w-b-zsw-w-z-w-sq-szwqqw--zqaqq-z-z--wzwz
yaa-sqz-w-q-w--sswswzsz--zsw-w-z-z--wzwz
------qsq-ssqsssssssqfqssqssqw-zsqz-wzwz
zga-sqw-z-ssqsssssssqfqssqssqszsq-szwzwz
b-y-wsz-z-wa---ssz--wsw--wsz--zwsqwzwzwz
z-w-w-q-z--w--sq-sssqqz--wqaaazw-w-zwzwz
b-y-w-z-z-qw--wsqr-wawassz---wzw-w-zwzwz
z-w-w-q-qswcqsqsqsqssqsq-ssqswsw-w-zwzwz
b-y-y-z-z--fw-zsqc-zazasqr-z---w-w-zwzwz
z-w-y-z-z---w-sq-sqsqqw--zqa---w-w-zwzwz
b-y-y-z-zsszw--ssw--zsz--zsw--qw-w-zwzwz
z-w-y-z-sq-sqsssssssqfqssqssssww-w-zwzwz
hstsy-z--sqsqsssssssqfqssqsssssw-w-zwzwz
------z-----z--ssz--wsw--wsz-----w-zwzwz
------z-----z-sq-sszqqz--wqaa----w-zwzwz
------z----fz-wsqr-swwasqc--w----w-zwzwz
------z----rqsqsqsqssqsq-sssw----w-zwzwz
------z----------------ssw-------w-zwzwz
------sssssssssssssssssssssssssssw-zwzwz
-----------------------------------swsww
""", },

  {
    'name': "Blank",
    'neighborhood': 52,
    'rows': 16,
    'cols': 16,
    'description': "A blank grid.",
    'state': """
----------------
----------------
----------------
----------------
----------------
----------------
----------------
----------------
----------------
----------------
----------------
----------------
----------------
----------------
----------------
----------------
""", },

  {
    'name': "Small Loop",
    'neighborhood': 52,
    'rows': 8,
    'cols': 8,
    'description': "A small loop.",
    'state': """
--------
-essssz-
-w----z-
-w----z-
-w----z-
-w----z-
-waaaaa-
--------
""", },

  {
    'name': "Loop",
    'neighborhood': 52,
    'rows': 16,
    'cols': 16,
    'description': "A medium loop.",
    'state': """
----------------
-essssssssssssz-
-w------------z-
-w------------z-
-w------------z-
-w------------z-
-w------------z-
-w------------z-
-w------------z-
-w------------z-
-w------------z-
-w------------z-
-w------------z-
-w------------z-
-waaaaaaaaaaaaa-
----------------
""", },

]
