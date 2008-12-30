# Makefile for MicropolisCore
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
########################################################################


PYTHON=python2.5

SOURCES = \
	allocate.cpp \
	animate.cpp \
	budget.cpp \
	connect.cpp \
	disasters.cpp \
	evaluate.cpp \
	fileio.cpp \
	generate.cpp \
	graph.cpp \
	initialize.cpp \
	main.cpp \
	map.cpp \
	message.cpp \
	micropolis.cpp \
	power.cpp \
	random.cpp \
	resource.cpp \
	scan.cpp \
	simulate.cpp \
	sprite.cpp \
	stubs.cpp \
	tool.cpp \
	traffic.cpp \
	update.cpp \
	utilities.cpp \
	zone.cpp


all: MICROPOLISENGINE

MICROPOLISENGINE: $(SOURCES) python/micropolisengine_wrap.cpp
	cd python ; $(PYTHON) setup.py build

python/stdafx.h: micropolis.h

swig/micropolisengine.i: swig/micropolisengine-swig-python.i micropolis.h

swig/micropolisengine-swig-python.i: micropolis.h

swig/micropolisengine.py: swig/micropolisengine.i

python/micropoliswindow.py:

python/micropolisengine_wrap.cpp: swig/micropolisengine.i micropolis.h
	swig \
		-python \
		-c++ \
		-outdir python \
		-o python/micropolisengine_wrap.cpp \
		swig/micropolisengine.i

clean:
	rm -rf *~ *.ncb python/build
	rm -rf swig/*~
	rm -rf python/*~ python/*.pyc python/*.o python/*.pyd python/ReleaseSymbols python/Release python/Debug

cleanswig: clean
	rm -rf python/micropolisengine.py python/micropolisengine_wrap.cpp

install: all
	(cd python ; $(PYTHON) setup.py install)

allocate.cpp: python/stdafx.h micropolis.h
animate.cpp: python/stdafx.h micropolis.h
budget.cpp: python/stdafx.h micropolis.h
connect.cpp: python/stdafx.h micropolis.h
disasters.cpp: python/stdafx.h micropolis.h
evaluate.cpp: python/stdafx.h micropolis.h
evaluation.cpp: python/stdafx.h micropolis.h
fileio.cpp: python/stdafx.h micropolis.h
generate.cpp: python/stdafx.h micropolis.h
graph.cpp: python/stdafx.h micropolis.h
initialize.cpp: python/stdafx.h micropolis.h
main.cpp: python/stdafx.h micropolis.h
map.cpp: python/stdafx.h micropolis.h
message.cpp: python/stdafx.h micropolis.h
micropolis.cpp: python/stdafx.h micropolis.h
power.cpp: python/stdafx.h micropolis.h
random.cpp: python/stdafx.h micropolis.h
resource.cpp: python/stdafx.h micropolis.h
scan.cpp: python/stdafx.h micropolis.h
simulate.cpp: python/stdafx.h micropolis.h
sprite.cpp: python/stdafx.h micropolis.h
stubs.cpp: python/stdafx.h micropolis.h
tool.cpp: python/stdafx.h micropolis.h text.h
traffic.cpp: python/stdafx.h micropolis.h
update.cpp: python/stdafx.h micropolis.h
utilities.cpp: python/stdafx.h micropolis.h
zone.cpp: python/stdafx.h micropolis.h


########################################################################
