# Original TCL/X sound server support -- not used in OLPC Micropolis.
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

global errorInfo
set errorInfo {}

global auto_noexec
set auto_noexec 1

proc echo {args} {
  puts stdout $args
  flush stdout
} 

source $tk_library/wish.tcl

global ResourceDir
set ResourceDir ""

proc StartSound {resourcedir} {
  global ResourceDir
  set ResourceDir $resourcedir
}

proc KillSoundServer {} {
  catch "destroy ."
  echo Sound server exiting.
  exit
}

proc ConnectNewClient {mode f} {
  echo "ConnectNewClient mode $mode f $f"
}


set ChatSocket 8888
set ChatServer {}

if {0} {

catch {
    set ChatServer [accept $ChatSocket]
}

if {$ChatServer != {}} then {
    filehandler $ChatServer e "ConnectNewClient"
}

}


set ResourceDir $tk_library

sound file Rumble $ResourceDir/rumble.au
sound file HeavyTraffic $ResourceDir/traffic.au
sound file Siren $ResourceDir/siren.au
sound file HonkHonk-Low $ResourceDir/honk-low.au
sound file Monster $ResourceDir/monster.au
sound file Explosion-High $ResourceDir/expl-hi.au
sound file HonkHonk-High $ResourceDir/honk-hi.au
sound file HonkHonk-Med $ResourceDir/honk-med.au
sound file Explosion-Low $ResourceDir/expl-low.au
sound file QuackQuack $ResourceDir/quack.au
sound file Ignition $ResourceDir/ignition.au
sound file Whip $ResourceDir/whip.au
sound file Cuckoo $ResourceDir/cuckoo.au
sound file Boing $ResourceDir/boing.au
sound file Skid $ResourceDir/skid.au
sound file Computer $ResourceDir/computer.au
sound file Unix $ResourceDir/unix.au
sound file Sorry $ResourceDir/sorry.au
sound file UhUh $ResourceDir/uhuh.au
sound file Woosh $ResourceDir/woosh.au
sound file Aaah $ResourceDir/aaah.au
sound file Oop $ResourceDir/oop.au
sound file Beep $ResourceDir/beep.au
sound file Bop $ResourceDir/bop.au
sound file Zone $ResourceDir/zone.au
sound file Build $ResourceDir/build.au
sound file Res $ResourceDir/res.au
sound file Com $ResourceDir/com.au
sound file Ind $ResourceDir/ind.au
sound file Fire $ResourceDir/fire.au
sound file Query $ResourceDir/query.au
sound file Police $ResourceDir/police.au
sound file Wire $ResourceDir/wire.au
sound file Bulldozer $ResourceDir/bulldoze.au
sound file Rail $ResourceDir/rail.au
sound file Road $ResourceDir/road.au
sound file Chalk $ResourceDir/chalk.au
sound file Eraser $ResourceDir/eraser.au
sound file Stadium $ResourceDir/stadium.au
sound file Park $ResourceDir/park.au
sound file Seaport $ResourceDir/seaport.au
sound file Coal $ResourceDir/coal.au
sound file Nuclear $ResourceDir/nuclear.au
sound file Airport $ResourceDir/airport.au
sound file O $ResourceDir/o.au
sound file A $ResourceDir/a.au
sound file E $ResourceDir/e.au
