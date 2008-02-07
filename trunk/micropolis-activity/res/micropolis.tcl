#######################################################################
# Micropolis.tcl, by Don Hopkins.
# Copyright (C) 2002 by Electronic Arts.
# This file defines the user interface of Micropolis. 
# Modify at your own risk!
########################################################################
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
# Libraries
########################################################################


set errorInfo {}
set auto_noexec 1

source $tk_library/wish.tcl


########################################################################
# Globals
########################################################################


set UniqueID 0
set State uninitialized
set CityName "Micropolis"
set GameLevel 0
set SimHome [pwd]
set CityLibDir $SimHome/cities
set CityDir $CityLibDir
set OldBudget 0
set BudgetRoadFund 0
set BudgetFireFund 0
set BudgetPoliceFund 0
set BudgetTaxRate 0
set DemandRes 0
set DemandCom 0
set DemandInd 0
set Priority 2
set Time 3
set AutoGoto 1
set AutoBudget 1
set Disasters 1
set AutoBulldoze 1
set Sound 1
set DoAnimation 1
set DoMessages 1
set DoNotices 1
set ShapePies 1
set SoundServers {}
set AudioChannels {mode edit fancy warning intercom}
set BudgetTimeout 30
set BudgetTimer 0
set BudgetTimerActive 0
set BudgetsVisible 0
set EvaluationsVisible 0
set SplashScreenDelay 5000
set Scenario -1
set Chatting 0
set ChatServer "localhost"
set ChatSocket 6667
set ChatConnection {}
set NickName "nickname"
set UserName "username"
set ServerName "servername"
set RealName "realname"
set ChannelName "#Micropolis"
set HomeDir ""
set ResourceDir ""
set HostName ""
set LocalHostName "[exec hostname]"
set SaveCityWin ""
set MapHistory {}
set MapHistoryNum -1
set HelpLoaded 0
set QueryX 0
set QueryY 0
set FreeVotes 0
set ShowingPicture 300
set MaxLines 500
set ShrinkLines 250
set ShowingParms {}
set VoteNames {UseThisMap Ask Zone}
set VotesForUseThisMap {}
set VotesForAsk {}
set VotesForZone {}
set VotesForBudget {}
set CurrentDate {}

set HeadWindows {}
set EditorWindows {}
set MapWindows {}
set GraphWindows {}
set BudgetWindows {}
set EvaluationWindows {}
set SplashWindows {}
set ScenarioWindows {}
set FileWindows {}
set AskWindows {}
set PlayerWindows {}
set NoticeWindows {}
set HelpWindows {}
set FrobWindows {}

set HeadPanelWidth 360
set HeadPanelHeight 200

set MapPanelWidth 360
set MapPanelHeight 330

set NoticePanelWidth 360
set NoticePanelHeight 250

set SplashPanelWidth 1200
set SplashPanelHeight 900

set ScenarioPanelWidth 420
set ScenarioPanelHeight 440

set SugarURI ""
set SugarNickName ""
set SugarShared 0
set SugarActivated 0
set SugarBuddies {}

set SubWindows {
  {editor EditorWindows}
  {map MapWindows}
  {graph GraphWindows}
  {budget BudgetWindows}
  {evaluation EvaluationWindows}
  {scenario ScenarioWindows}
  {splash SplashWindows}
  {file FileWindows}
  {ask AskWindows}
  {player PlayerWindows}
  {notice NoticeWindows}
  {help HelpWindows}
  {frob FrobWindows}
  {head HeadWindows}
}


#   0           1               2               3       4         5   6   7   8         9       10   11       12      13
#   type	id		callback	param	var	  x   y   w   h		normal	over disabled checked checkedover
#   ----------- --------------- --------------- ------- ------- --- --- --- ---         ------- ---- -------- ------- -----------
set ScenarioButtons {
  { button	load		DoLoad		""	""	 70 238 157  90		""	@images/button1hilite.xpm "" }
  { button	generate	DoGenerate	""	""	 62 392 157  90		""	@images/button2hilite.xpm "" }
  { button	quit		DoQuit		""	""	 68 544 157  90		""	@images/button3hilite.xpm "" }
  { button	about		DoAbout		""	""	101 705 157  90		""	@images/button4hilite.xpm "" }
  { checkbox	easy		DoLevel		0	""	982 106 190  70		""	@images/checkbox1hilite.xpm "" @images/checkbox1checked.xpm @images/checkbox1hilitechecked.xpm }
  { checkbox	medium		DoLevel		1	""	982 176 190  70		""	@images/checkbox2hilite.xpm "" @images/checkbox2checked.xpm @images/checkbox2hilitechecked.xpm }
  { checkbox	hard		DoLevel		2	""	982 246 190  70		""	@images/checkbox3hilite.xpm "" @images/checkbox3checked.xpm @images/checkbox3hilitechecked.xpm }
  { button	left		DoLeft		""	""	540 375  50  50		""	@images/lefthilite.xpm @images/leftdisabled.xpm }
  { button	right		DoRight		""	""	841 375  50  50		""	@images/righthilite.xpm @images/rightdisabled.xpm }
  { button	play		DoPlay		""	""	625 376 180  50		""	@images/playhilite.xpm "" }
  { button	scenario1	DoPickScenario	"1"	""	310 451 209 188		""	@images/scenario1hilite.xpm "" }
  { button	scenario2	DoPickScenario	"2"	""	519 451 209 188		""	@images/scenario2hilite.xpm "" }
  { button	scenario3	DoPickScenario	"3"	""	727 450 209 188		""	@images/scenario3hilite.xpm "" }
  { button	scenario4	DoPickScenario	"4"	""	936 450 209 188		""	@images/scenario4hilite.xpm "" }
  { button	scenario5	DoPickScenario	"5"	""	310 639 209 188		""	@images/scenario5hilite.xpm "" }
  { button	scenario6	DoPickScenario	"8"	""	519 639 209 188		""	@images/scenario6hilite.xpm "" }
  { button	scenario7	DoPickScenario	"7"	""	728 638 209 188		""	@images/scenario7hilite.xpm "" }
  { button	scenario8	DoPickScenario	"6"	""	937 638 209 188		""	@images/scenario8hilite.xpm "" }
}

# Disabled until we handle mouse events on the map itself.
#   { button	map		DoMap		""	""	516  30 396 338		""	@images/maphilite.xpm }


########################################################################
# Fonts


set FontInfo {
  {Big {
	{-*-dejavu lgc sans-medium-r-normal-*-90-*}
  }}
  {Large {
	{-*-dejavu lgc sans-medium-r-normal-*-80-*}
  }}
  {Medium {
	{-*-dejavu lgc sans-medium-r-normal-*-70-*}
  }}
  {Small {
	{-*-dejavu lgc sans-medium-r-normal-*-60-*}
  }}
  {Narrow {
	{-*-dejavu lgc sans-medium-r-normal-*-60-*}
  }}
  {Tiny {
	{-*-dejavu lgc sans-medium-r-normal-*-60-*}
  }}
  {Text {
	{-*-dejavu lgc sans-medium-r-normal-*-70-*}
  }}
  {Message {
	{-*-dejavu lgc sans-medium-r-normal-*-70-*}
  }}
  {Alert {
	{-*-dejavu lgc sans-medium-r-normal-*-70-*}
  }}
}


########################################################################


set MapTitles {
  {Micropolis Overall Map}
  {Residential Zone Map}
  {Commercial Zone Map}
  {Industrial Zone Map}
  {Power Grid Map}
  {Transportation Map}
  {Population Density Map}
  {Rate of Growth Map}
  {Traffic Density Map}
  {Pollution Desity Map}
  {Crime Rate Map}
  {Land Value Map}
  {Fire Coverage Map}
  {Police Coverage Map}
  {Dynamic Filter Map}
}


set EditorPallets {
  leftframe.tools.palletres
  leftframe.tools.palletcom
  leftframe.tools.palletind
  leftframe.tools.palletfire
  leftframe.tools.palletquery
  leftframe.tools.palletpolice
  leftframe.tools.palletwire
  leftframe.tools.palletbulldozer
  leftframe.tools.palletrail
  leftframe.tools.palletroad
  leftframe.tools.palletchalk
  leftframe.tools.palleteraser
  leftframe.tools.palletstadium
  leftframe.tools.palletpark
  leftframe.tools.palletseaport
  leftframe.tools.palletcoal
  leftframe.tools.palletnuclear
  leftframe.tools.palletairport
}


set EditorPalletImages { 
  res com ind fire qry pol
  wire dozr rail road chlk ersr
  stad park seap coal nuc airp
}


set EditorPalletSounds { 
  Res Com Ind Fire Query Police
  Wire Bulldozer Rail Road Chalk Eraser
  Stadium Park Seaport Coal Nuclear Airport
}


set GraphPallets {
  leftframe.left.res
  leftframe.left.com
  leftframe.left.ind
  leftframe.right.money
  leftframe.right.crime
  leftframe.right.pollution
}


set GraphPalletImages { 
  res com ind mony crim poll
}


set GraphYearPallets {
  leftframe.year.year10
  leftframe.year.year120
}


set GraphYearPalletImages { 10 120 }


set ToolInfo {
  {     {a}	{Residential Zone}	{$100}}
  {     {a}	{Commercial Zone}	{$100}}
  {     {an}	{Industrial Zone}	{$100}}
  {     {a}	{Fire Station}		{$500}}
  {     {a}	{Query}			{free}}
  {     {a}	{Police Station}	{$500}}
  {     {a}	{Wire}			{$5}}
  {     {a}	{Bulldozer}		{$1}}
  {     {a}	{Rail}			{$20}}
  {     {a}	{Road}			{$10}}
  {     {a}	{Chalk}			{free}}
  {     {an}	{Eraser}		{free}}
  {     {a}	{Stadium}		{$5,000}}
  {     {a}	{Park}			{$20}}
  {     {a}	{Seaport}		{$3,000}}
  {     {a}	{Coal Power Plant}	{$3,000}}
  {     {a}	{Nuclear Power Plant}	{$5,000}}
  {     {an}	{Airport}		{$10,000}}
  {     {a}	{Network}		{$1,000}}
}


set DynamicDataNames {
  {Population Density}
  {Rate of Growth}
  {Traffic Density}
  {Pollution Density}
  {Crime Rate}
  {Land Value}
  {Police Coverage}
  {Fire Coverage}
}

########################################################################
# Initialization
########################################################################


wm title . {Micropolis Root}


if {"[sim Platform]" == "msdos"} {
  sim DoAnimation 0
  set DoAnimation 0
  set ShapePies 0
} else {
  sim DoAnimation 1
  set DoAnimation 1
  set ShapePies 1
}

sprite explosion 7
sprite tornado 6
sprite airplane 3
sprite helicopter 2
sprite monster 5
sprite ship 4
sprite bus 8
sprite train 1

sim ResetDynamic

# The next three lines have no effect on the OLPC, where we don't provide
# the font in res (because it's already in the system fonts).  These lines
# are for other systems that lack the font.
set FontPath "[pwd]/res/dejavu-lgc"
system "xset -fp $FontPath >&/dev/null"
system "xset +fp $FontPath >&/dev/null"


########################################################################
# Messages
########################################################################


proc Message {id color title msg {props {}}} {
  global Messages
  set Messages($id) [list $color $title $msg $props]
}


Message 1 #7f7fff {DULLSVILLE, USA  1900} \
{Things haven't changed much around here in the last hundred years or so and the residents are beginning to get bored.  They think Dullsville could be the next great city with the right leader. 

It is your job to attract new growth and development, turning Dullsville into a Metropolis within 30 years.}

Message 2 #7f7fff {SAN FRANCISCO, CA.  1906} \
{Damage from the earthquake was minor compared to that of the ensuing fires, which took days to control.  1500 people died.

Controlling the fires should be your initial concern.  Then clear the rubble and start rebuilding.  You have 5 years.}
	
Message 3 #7f7fff {HAMBURG, GERMANY  1944} \
{Allied fire-bombing of German cities in WWII caused tremendous damage and loss of life.  People living in the inner cities were at greatest risk.

You must control the firestorms during the bombing and then rebuild the city after the war.  You have 5 years.}

Message 4 #7f7fff {BERN, SWITZERLAND  1965} \
{The roads here are becoming more congested every day, and the residents are upset.  They demand that you do something about it.

Some have suggested a mass transit system as the answer, but this would require major rezoning in the downtown area.  You have 10 years.}

Message 5 #7f7fff {TOKYO, JAPAN  1957} \
{A large reptilian creature has been spotted heading for Tokyo bay.  It seems to be attracted to the heavy levels of industrial pollution there.

Try to control the fires, then rebuild the industrial center.  You have 5 years.}

Message 6 #7f7fff {DETROIT, MI.  1972} \
{By 1970, competition from overseas and other economic factors pushed the once "automobile capital of the world" into recession.  Plummeting land values and unemployment then increased crime in the inner-city to chronic levels.

You have 10 years to reduce crime and rebuild the industrial base of the city.}

Message 7 #7f7fff {BOSTON, MA.  2010} \
{A major meltdown is about to occur at one of the new downtown nuclear reactors.  The area in the vicinity of the reactor will be severly contaminated by radiation, forcing you to restructure the city around it.

You have 5 years to get the situation under control.}

Message 8 #7f7fff {RIO DE JANEIRO, BRAZIL  2047} \
{In the mid-21st century, the greenhouse effect raised global temperatures 6 degrees F.  Polar icecaps melted and raised sea levels worldwide.  Coastal areas were devastated by flood and erosion.

You have 10 years to turn this swamp back into a city again.}

Message 9 #ffa500 {Query Zone Status} \
{
Zone:	    %s
Density:    %s
Value:	    %s
Crime:	    %s
Pollution:  %s
Growth:	    %s} \
{{view {PanView $v $QueryX $QueryY}}}

Message 10 #ff4f4f {POLLUTION ALERT!} \
{Pollution in your city has exceeded the maximum allowable amounts established by the Micropolis Pollution Agency.  You are running the risk of grave ecological consequences.

Either clean up your act or open a gas mask concession at city hall.} \
{{view {PanView $v [sim PolMaxX] [sim PolMaxY]}}}

Message 11 #ff4f4f {CRIME ALERT!} \
{Crime in your city is our of hand.  Angry mobs are looting and vandalizing the central city.  The president will send in the national guard soon if you cannot control the problem.} \
{{view {PanView $v [sim CrimeMaxX] [sim CrimeMaxY]}}}

Message 12 #ff4f4f {TRAFFIC WARNING!} \
{Traffic in this city is horrible.  The city gridlock is expanding.  The commuters are getting militant.

Either build more roads and rails or get a bulletproof limo.} \
{{view {PanView $v [sim TrafMaxX] [sim TrafMaxY]}}}

Message 20 #ff4f4f {FIRE REPORTED!} \
"A fire has been reported!" \
{{view {PanView $v [sim CrashX] [sim CrashY]}}}

Message 21 #ff4f4f {MONSTER ATTACK!} \
"A large reptilian creature has been spotted in the water.  It seems to be attracted to areas of high pollution.  There is a trail of destruction wherever it goes.  \
All you can do is wait till he leaves, then rebuild from the rubble." \
{{view {FollowView $v monster}}}

# XXX: write more text
Message 22 #ff4f4f {TORNADO ALERT!} \
{A tornado has been reported!  There's nothing you can do to stop it, so you'd better prepare to clean up after the disaster!} \
{{view {FollowView $v tornado}}}

# XXX: write more text
Message 23 #ff4f4f {EARTHQUAKE!} \
{A major earthquake has occurred!  Put out the fires as quickly as possible, before they spread, then reconnect the power grid and rebuild the city.} \
{{view {PanView $v [sim CenterX] [sim CenterY]}}}

# XXX: write more text
Message 24 #ff4f4f {PLANE CRASH!} \
{A plane has crashed!} \
{{view {PanView $v [sim CrashX] [sim CrashY]}}}

# XXX: write more text
Message 25 #ff4f4f {SHIPWRECK!} \
{A ship has wrecked!} \
{{view {PanView $v [sim CrashX] [sim CrashY]}}}

# XXX: write more text
Message 26 #ff4f4f {TRAIN CRASH!} \
{A train has crashed!} \
{{view {PanView $v [sim CrashX] [sim CrashY]}}}

# XXX: write more text
Message 27 #ff4f4f {HELICOPTER CRASH!} \
{A helicopter has crashed!} \
{{view {PanView $v [sim CrashX] [sim CrashY]}}}

Message 30 #ff4f4f {FIREBOMBING REPORTED!} \
{Firebombs are falling!!} \
{{view {PanView $v [sim CrashX] [sim CrashY]}}}

Message 35 #7fff7f {TOWN} \
{Congratulations, your village has grown to town status.  You now have 2,000 citizens.} \
{{view {PanView $v [sim CenterX] [sim CenterY]}}}

Message 36 #7fff7f {CITY} \
{Your town has grown into a full sized city, with a current population of 10,000.  Keep up the good work!} \
{{view {PanView $v [sim CenterX] [sim CenterY]}}}

Message 37 #7fff7f {CAPITAL} \
{Your city has become a capital.  The current population here is 50,000.  Your political future looks bright.} \
{{view {PanView $v [sim CenterX] [sim CenterY]}}}

Message 38 #7fff7f {METROPOLIS} \
{Your capital city has now achieved the status of metropolis.  The current population is 100,000.  With your management skills, you should seriously consider running for governor.} \
{{view {PanView $v [sim CenterX] [sim CenterY]}}}

Message 39 #7fff7f {MEGALOPOLIS} \
{Congratulation, you have reached the highest category of urban development, the megalopolis.

If you manage to reach this level, send us email at micropolis@laptop.org or send us a copy of your city.  We might do something interesting with it.} \
{{view {PanView $v [sim CenterX] [sim CenterY]}}}

Message 40 #7fff7f {MEGALINIUM} \
{Congratulation, you have reached the end of time!

Because of the toroidal nature of the the Micropolis Space/Time Continuum, your city has wrapped back in time to 1900!} \
{{view {PanView $v [sim CenterX] [sim CenterY]}}}

# XXX: write more text
Message 41 #ff4f4f {HEAVY TRAFFIC!} \
{Sky Watch One
reporting heavy traffic!} \
{{view {FollowView $v helicopter}}}

# XXX: write more text
Message 42 #ff4f4f {FLOODING REPORTED!} \
{Flooding has been been reported along the water's edge!} \
{{view {PanView $v [sim FloodX] [sim FloodY]}}}

Message 43 #ff4f4f {NUCLEAR MELTDOWN!} \
{A nuclear meltdown has occured at your power plant.  You are advised to avoid the area until the radioactive isotopes decay.

Many generations will confront this problem before it goes away, so don't hold your breath.} \
{{view {PanView $v [sim MeltX] [sim MeltY]}}}


Message 44 #ff4f4f {RIOTS!} \
{The citizens are rioting in the streets, setting cars and houses on fire, and bombing government buildings and businesses!

All media coverage is blacked out, while the fascist pigs beat the poor citizens into submission.}

Message 46 #ff4f4f {NO SOUND SERVER!} \
{There is no sound server running on your X11 display "%s".  You won't hear any noise unless you run a sound server, and turn the sound back on in the "Options" menu.}

Message 48 #7f7fff {Start a New City} \
{Build your very own city from the ground up, starting with this map of uninhabited land.}

Message 49 #7f7fff {Restore a Saved City} \
{This city was saved in the file named: %s}

Message 100 #7fff7f {YOU'RE A WINNER!} \
{Your mayorial skill and city planning expertise have earned you the KEY TO THE CITY.  Local residents will erect monuments to your glory and name their first-born children after you.  Why not run for governor?} \
{{middle {@images/key2city.xpm}}}

Message 200 #ff4f4f {IMPEACHMENT NOTICE!} \
{The entire population of this city has finally had enough of your inept planning and incompetant management.  An angry mob -- led by your mother -- has been spotted in the vicinity of city hall.

You should seriously consider taking an extended vacation -- NOW.  (Or read the manual and try again.)}

Message 300 #ffd700 {About Micropolis} \
"Micropolis Version [sim Version] Copyright (C) 2007
    by Electronic Arts.
Based on the Original Micropolis Concept and Design
    by Will Wright.
TCL/Tk User Interface Designed and Created
    by Don Hopkins, DUX Software.
Ported to Linux, Optimized and Adapted for OLPC
    by Don Hopkins.
Licensed under the GNU General Public License, 
    version 3, with additional conditions." 


########################################################################
# Options
########################################################################


option add *CheckButton.relief		flat
option add *Dialog.cursor		top_left_arrow
option add *Entry.relief		sunken
option add *Frame.borderWidth		0
option add *Listbox.relief		sunken
option add *Scrollbar.relief		sunken
option add *RadioButton.anchor		w
option add *RadioButton.relief		flat

option add *background			#b0b0b0
option add *foreground			#000000
option add *activeBackground		#d0d0d0
option add *activeForeground		#000000
option add *disabledForeground		""
option add *selectBackground		#d0d0d0
option add *selectForeground		#000000
#option add *selector			#ffff80
option add *selector			#bf0000

option add *Scrollbar.Background 	#b0b0b0
option add *Scrollbar.Foreground	#d0d0d0
option add *Interval.Background 	#b0b0b0
option add *Interval.Foreground		#000000
option add *Interval.activeForeground	#d0d0d0
option add *Interval.sliderForeground	#b0b0b0
option add *Scale.activeForeground	#d0d0d0
option add *Scale.sliderForeground	#b0b0b0
option add *PieMenu.activeBackground	#b0b0b0

option add Mwm*Micropolis.clientDecoration -maximize


########################################################################
# Global Bindings
########################################################################


bind all <Help> {HandleHelp %W %x %y %X %Y}
bind all <Shift-ButtonPress> {HandleHelp %W %x %y %X %Y}
bind all <Meta-ButtonPress> {HandleHelp %W %x %y %X %Y}
bind all <Shift-Meta-ButtonPress> {HandleHelp %W %x %y %X %Y}


########################################################################
# Utilities
########################################################################


proc echo {args} {
  puts stdout $args
  flush stdout
} 


proc Unique {} {
  global UniqueID
  set id $UniqueID
  incr UniqueID
  return $id
}


proc tkerror {err} {
    global errorInfo
    puts stderr "$errorInfo"
}


proc ident {i} {
  return "$i"
}

proc NoFunction {args} {}


proc LinkWindow {fromname to} {
  global WindowLinks
  set WindowLinks($fromname) $to
}


proc WindowLink {fromname} {
  global WindowLinks
  set to ""
  catch {set to $WindowLinks($fromname)}
  return $to
}


proc DeleteWindow {sym name win} {
  set head [WindowLink $win.head]
  LinkWindow $head.$sym {}
  global $name
  set wins [eval ident "\$$name"]
  set i [lsearch $wins $win]
  if {$i != -1} {
    set $name [lreplace $wins $i $i]
  }
  destroy $win
}


proc Font {win name} {
  global FontInfo FontCache HeadWindows
  set scr [winfo screen $win]
  set font ""
  catch {
    set font $FontCache($scr,$name)
  }
  if {"$font" == ""} {
    set label ""
    catch {
      set label $FontCache($scr)
    }
    if {"$label" == ""} {
      foreach head $HeadWindows {
	if {"[winfo screen $head]" == "$scr"} {
	  set label $head.fontlabel
	  label $label -text "X11 Sucks" -font fixed
	  set FontCache($scr) $label
	}
      }
    }

    set fonts [keylget FontInfo $name]
    foreach font $fonts {
      #echo "Configuring font $font on $label"
      if {[catch "$label config -font \"$font\""] == 0} {
        #echo "yow!"
	break
      } else {
        #echo "oops!"
	set font ""
      }
    }
    if {"$font" == ""} {
      #set font 8x16
      # OLCP default font
      set font 6x13
    }
    set FontCache($scr,$name) $font
  }
  return $font
}


proc Color {win color mono} {
  if {[winfo screendepth $win] == 1} {
    return $mono
  } else {
    return $color
  }
}


########################################################################
# Window Definition Functions
########################################################################


proc MakeWindow.head {{display ":0"}} {
  global ResourceDir
  source $ResourceDir/whead.tcl
  return $win
}


proc MakeWindow.editor {head {display ":0"}} {
  global ResourceDir
  source $ResourceDir/weditor.tcl
  return $win
}


proc MakeWindow.map {head {display ":0"}} {
  global ResourceDir
  source $ResourceDir/wmap.tcl
  return $win
}


proc MakeWindow.graph {head {display ":0"}} {
  global ResourceDir
  source $ResourceDir/wgraph.tcl
  return $win
}


proc MakeWindow.budget {head {display ":0"}} {
  global ResourceDir
  source $ResourceDir/wbudget.tcl
  return $win
}


proc MakeWindow.evaluation {head {display ":0"}} {
  global ResourceDir
  source $ResourceDir/weval.tcl
  return $win
}


proc MakeWindow.splash {head {display ":0"}} {
  global ResourceDir
  source $ResourceDir/wsplash.tcl
  return $win
}


proc MakeWindow.scenario {head {display ":0"}} {
  global ResourceDir
  source $ResourceDir/wscen.tcl
  return $win
}


proc MakeWindow.file {head {display ":0"}} {
  global ResourceDir
  source $ResourceDir/wfile.tcl
  return $win
}


proc MakeWindow.ask {head {display ":0"}} {
  global ResourceDir
  source $ResourceDir/wask.tcl
  return $win
}


proc MakeWindow.player {head {display ":0"}} {
  global ResourceDir
  source $ResourceDir/wplayer.tcl
  return $win
}


proc MakeWindow.notice {head {display ":0"}} {
  global ResourceDir
  source $ResourceDir/wnotice.tcl
  return $win
}

proc MakeWindow.help {head {display ":0"}} {
  global ResourceDir
  source $ResourceDir/whelp.tcl
  return $win
}

proc MakeWindow.frob {head {display ":0"}} {
  global ResourceDir
  source $ResourceDir/wfrob.tcl
  return $win
}


########################################################################
# Sound Support
########################################################################


proc UIInitializeSound {} {
}


proc UIShutDownSound {} {
}


proc UIDoSoundOn {win cmd} {
  global Sound SoundServers
  if {$Sound} {
    set win [WindowLink [winfo toplevel $win].head]
    if {[lsearch $SoundServers $win] != -1} {
      set cmd "send -quick -server $win Sound sound $cmd"
      if {[catch $cmd]} {
	# XXX: Lost a sound server...
	LostSoundServer $win
      }
    }
  }
}


proc UIDoSound {cmd} {
  global Sound SoundServers
  if {$Sound} {
    foreach win $SoundServers {
      set foo "send -quick -server $win Sound $cmd"
      if {[catch $foo]} {
	# XXX: Lost a sound server...
        LostSoundServer $win
      }
    }
  }
}


proc KillSoundServers {} {
  global SoundServers
  foreach win $SoundServers {
    set foo "send -quick -server $win Sound KillSoundServer"
    catch $foo
  }
  set SoundServers {}
}


proc UISetChannelVolume {win chan vol} {
  UIDoSoundOn $win "channel $chan -volume $vol"
}


proc EchoPlaySound {soundspec} {
  # Temporary workaround to tell Python Sugar app to play sound.
  global Sound
  if {$Sound} {
    echo PlaySound [lindex $soundspec 0]
  }
}


proc UIMakeSoundOn {win chan sound {opts ""}} {
  # Send message to Python to play sound.
  EchoPlaySound $sound

  #UIDoSoundOn $win "play $sound -replay -channel $chan $opts"
}


proc UIStartSoundOn {win chan sound {opts ""}} {
  # Send message to Python to play sound.
  EchoPlaySound $sound

  #UIDoSoundOn $win "play $sound -replay -channel $chan -repeat 100 $opts"
}


proc UIStopSoundOn {win chan sound {opts ""}} {
  UIDoSoundOn $win "stop $sound"
}


proc UIMakeSound {chan sound {opts ""}} {
  # Send message to Python to play sound.
  EchoPlaySound $sound

  #UIDoSound "sound play $sound -replay -channel $chan $opts"
}


proc UIStartSound {chan sound {opts ""}} {
  # Send message to Python to play sound.
  EchoPlaySound $sound

  #UIDoSound "sound play $sound -channel $chan -repeat 100 $opts"
}


proc UIStopSound {chan sound {opts ""}} {
  UIDoSound "sound stop $sound"
}


proc SetupSoundServer {win} {
  AddSoundServer $win
}


proc AddSoundServer {win} {
  global SoundServers
  set i [lsearch $SoundServers $win]
  if {$i < 0} {
    set SoundServers [linsert $SoundServers 0 $win]
  }
}


proc LostSoundServer {win} {
  DeleteSoundServer $win
#  UIShowPictureOn [WindowLink $win.head] 46 [winfo screen $win]
}


proc DeleteSoundServer {win} {
  global SoundServers
  set i [lsearch $SoundServers $win]
  if {$i >= 0} {
    set SoundServers [lreplace $SoundServers $i $i]
  }
}


proc UISoundOff {} {
}


proc MonsterSpeed {} {
  return [expr "[sim Rand 40] + 70"]
}


proc ExplosionPitch {} {
  return [expr "[sim Rand 20] + 90"]
}


proc HonkPitch {} {
  return [expr "[sim Rand 20] + 90"]
}


########################################################################
# Global Window Handlers


proc WithdrawAll {} {
  WithdrawHeads
  WithdrawEditors
  WithdrawMaps
  WithdrawGraphs
  WithdrawBudgets
  WithdrawEvaluations
  WithdrawSplashes
  WithdrawScenarios
  WithdrawFiles
  WithdrawAsks
  WithdrawPlayers
  WithdrawNotices
  WithdrawHelps
  WithdrawFrobs
}


proc ShowInitial {} {
  ShowHeads
  ShowEditors
  ShowMaps
  EnableMaps
}


########################################################################
# Head Window Handlers


proc PrepHead {head} {
  global State
  InitHeadMenus $head
  case $State {
    uninitialized {
    }
    splash {
      WithdrawHeadOf $head
      ShowSplashOf $head
    }
    scenario {
      after 1000 "WithdrawSplashOf $head"
      WithdrawHeadOf $head
      ShowScenarioOf $head
      sim UpdateMaps
    }
    play {
      WithdrawSplashOf $head
      WithdrawScenarioOf $head
      ShowHeadOf $head
      ShowEditorOf $head
      ShowMapOf $head
      EnableMaps
      ReShowPictureOn $head
      InitHead $head
      InitHeadMenus $head
    }
  }
}


proc ShowHeadOf {head} {
  wm deiconify $head
}


proc WithdrawHeadOf {head} {
  wm withdraw $head
}


proc ShowHeads {} {
  global HeadWindows
  foreach win $HeadWindows {
    ShowHeadOf $win
  }
}


proc WithdrawHeads {} {
  global HeadWindows
  foreach win $HeadWindows {
    wm withdraw $win
  }
}


proc DeleteHeadWindow {head} {
  UIQuit $head
}


proc InitHeads {} {
  global HeadWindows
  foreach win $HeadWindows {
    InitHead $win
  }
}


proc InitHead {win} {
  set w [WindowLink $win.text]
  $w configure -state normal
  $w delete 0.0 end
  $w insert end "\n"
  $w configure -state disabled

  if {[sim MultiPlayerMode]} {
    set w [WindowLink $win.entry]
    $w delete 0 end
    global ${w}.value
    set ${w}.value ""
  }

  sim UpdateHeads
  UIUpdateRunning
}


proc InitAllHeadMenus {} {
  global HeadWindows

  foreach win $HeadWindows {
    InitHeadMenus $win
  }
}


proc InitHeadMenus {win} {
  global State
  set m0 [WindowLink $win.m0]
  set m1 [WindowLink $win.m1]
  set m2 [WindowLink $win.m2]
  set m3 [WindowLink $win.m3]
  set m4 [WindowLink $win.m4]
  set b0 [WindowLink $win.b0]
  set b1 [WindowLink $win.b1]
  set b2 [WindowLink $win.b2]
  set b3 [WindowLink $win.b3]
  set b4 [WindowLink $win.b4]
  case $State {
    uninitialized {
    }
    splash {
    }
    scenario {
      $m0 unpost
      $m1 unpost
      $m2 unpost
      $m3 unpost
      $m4 unpost
      $m0 disable 1
      $m0 disable 2
      # When in multi player mode there is an extra menu
      # item to add another player, before the quit item,
      # so compensate for it here.
      if {[sim MultiPlayerMode]} {
        $m0 disable 4
      } else {
        $m0 disable 3
      }
      $b0 config -state normal
      $b2 config -state disabled
      $b4 config -state disabled
    }
    play {
      $m0 enable 1
      $m0 enable 2
      # When in multi player mode there is an extra menu
      # item to add another player, before the quit item,
      # so compensate for it here.
      if {[sim MultiPlayerMode]} {
        $m0 enable 4
      } else {
        $m0 enable 3
      }
      $b0 config -state normal
      $b2 config -state normal
      $b4 config -state normal
    }
  }
}


proc UIDisaster {win cmd action} {
  AskQuestion [Color $win #ff0000 #ffffff] "Cause a Disaster" \
    "Oh no! Do you really want to $action" \
    "{No way!} Disaster.No {WithdrawAsks}" \
    "" \
    "{I guess so.} Disaster.Yes {WithdrawAsks ; $cmd}"
}


proc CrushHead {head} {
  global SubWindows VoteNames

  foreach foo $VoteNames {
    global VotesFor$foo
    set votes [eval ident \$VotesFor$foo]
    set i [lsearch $votes $head]
    if {$i != -1} {
      set VotesFor$foo [lreplace $votes $i $i]
    }
  }

  foreach foo $SubWindows {
    set sym [lindex $foo 0]
    set name [lindex $foo 1]
    global $name
    set wins [eval ident "\$$name"]
    foreach win $wins {
      if {[WindowLink $win.head] == $head} {
	DeleteWindow $sym $name $win
      }
    }
  }
}


proc ChatDown {win} {
  tk_butDown $win
}


proc ChatUp {win} {
  tk_butUp $win

  global Chatting

  if {$Chatting} {
    CloseChat
  } else {
    OpenChat
  }
}


proc OpenChat {} {
  global Chatting
  global ChatServer
  global ChatSocket
  global ChatConnection

  DoSendMessage "Opening Connection to Chat Server $ChatServer socket $ChatSocket ..." status

  set ChatConnection {}
  catch {
    set ChatConnection [connect $ChatServer $ChatSocket]
  }

  if {$ChatConnection == {}} then {
    DoSendMessage "Sorry, I could not open a connection to chat server $ChatServer socket $ChatSocket." status
    set Chatting 0
  } else {
    filehandler $ChatConnection r "ReceiveChat"
    set Chatting 1
  }

  global NickName
  global UserName
  global HostName
  global ServerName
  global RealName
  global ChannelName
  catch {
    puts $ChatConnection "USER $UserName $HostName $ServerName $RealName\r\n" nonewline
    puts $ChatConnection "NICK $NickName\r\n" nonewline
    puts $ChatConnection "JOIN $ChannelName\r\n" nonewline
    flush $ChatConnection
  }
}


proc ReceiveChat {mode f} {
#    DoSendMessage "ReceiveChat: MODE $mode F $f"
    set msg ""
    gets $f msg

    if {[string first {:} $msg]} {
	set msg ": $msg"
    }

    set from [lindex $msg 0]
    set cmd [string tolower [lindex $msg 1]]

    set handled 0

    if {($cmd == "privmsg") ||
        ($cmd == "notice")} then {
      set handled 1
      set chan [lindex $msg 2]
      set i [expr "[string length $from] + 1 + [string length $cmd] + 1 + [string length $chan] + 2"]
      set j [expr "[string length $msg] - 2"]
      set line [string range $msg $i $j]
      set chan [string range $chan 1 [string length $chan]]
      set from [string range $from 1 [expr "[string length $from] - 1"]]
      set from [split $from {!}]
      set from [lindex $from 0]
      DoSendMessage "$from> $line"
    }

    if {$handled == 0} {
	set msg [string range $msg \
		  [expr "[string length $from] + 1"] \
		  [expr "[string length $msg] - 2"]]
	DoSendMessage $msg
    }
}


proc SendChatMessage {msg win} {
    global ChatConnection ChannelName NickName

    if {"$msg" == ""} {
	return
    }

    if {[string range $msg 0 0] == "/"} {
	set cmd [string range $msg 1 [string length $msg]]
        DoSendMessage "$NickName> \[$cmd\]"
	set cmd "$cmd\r\n"
    } else {
	set cmd "PRIVMSG $ChannelName :$msg\r\n"
        DoSendMessage "$NickName> $msg"
    }
    

    set result 1

    catch {
	puts $ChatConnection $cmd nonewline
	flush $ChatConnection
	set result 0
    }

    if {$result} {
	DoSendMessage "IRC Broke the connection"
	CloseChat
    }
}


proc CloseChat {} {
  global Chatting
  global ChatServer
  global ChatSocket
  global ChatConnection

  DoSendMessage "Closing Connection to Chat Server." status

  catch {
    puts $ChatConnection "QUIT\r\n" nonewline
    flush $ChatConnection
    $ChatConnection close
  }

  set ChatConnection {}
  set Chatting 0
}


proc DoEnterMessage {win var} {
  global Chatting
  global $var
  set msg [eval ident "\$\{$var\}"]

  if {$Chatting} then {
    SendChatMessage $msg $win
  } else {
    DoSendMessage $msg
  }
  $win delete 0 end
}


proc DoEvalMessage {win var} {
  global $var
  set command [eval ident "\$\{$var\}"]
  $win delete 0 end
  DoSendMessage "Evaluating TCL: $command"
  catch {uplevel #0 $command} result
  DoSendMessage "Result: $result"
}


proc DoSendMessage {msg {tag message}} {
  global HeadWindows
  foreach win $HeadWindows {
    appendWithTag [WindowLink $win.text] $tag "$msg"
  }
}


proc UISetMessage {msg {tag status}} {
  global DoMessages EditorWindows HeadWindows

  if {$DoMessages == 0} {
      return
  }

  foreach win $EditorWindows {
    [WindowLink $win.message] configure -text "$msg"
  }

  foreach win $HeadWindows {
    appendWithTag [WindowLink $win.text] $tag "$msg"
  }
}




proc appendWithTag {w tag text} {
    global MaxLines
    global ShrinkLines

    set start [$w index end]
    $w configure -state normal
    if {$start > $MaxLines} then {
	$w delete 0.0 $ShrinkLines.0
	set start [$w index end]
    }
    $w insert end "${text}\n"
    $w tag add $tag $start {end - 1 char}
    $w configure -state disabled
    $w yview -pickplace {end - 1 char}
}


########################################################################
# Budget Window Handlers


proc ShowBudgetOf {head} {
  set win [WindowLink $head.budget]
  if {$win == {}} {
    set win [MakeWindow.budget $head [winfo screen $head]]
  }
  wm deiconify $win
  wm raise $win
  update idletasks
  sim UpdateBudget
}


proc ShowBudgets {} {
  global HeadWindows BudgetsVisible
  set BudgetsVisible 1
  foreach win $HeadWindows {
    ShowBudgetOf $win
  }
}


proc WithdrawBudgets {} {
  global BudgetWindows BudgetsVisible
  foreach win $BudgetWindows {
    wm withdraw $win
  }
  StopBudgetTimer
  set BudgetsVisible 0
}


proc BudgetContinue {{win ""}} {
  global OldBudget BudgetRoadFund BudgetFireFund BudgetPoliceFund BudgetTaxRate
  set OldBudget 0
  if {([sim RoadFund] != $BudgetRoadFund) ||
      ([sim FireFund] != $BudgetFireFund) ||
      ([sim PoliceFund] != $BudgetPoliceFund) ||
      ([sim TaxRate] != $BudgetTaxRate)} {
    UISetMessage "The budget was changed."
  } else {
    UISetMessage "The budget wasn't changed."
  }
  WithdrawBudgets
  sim Resume
  MakeRunningSound
}


proc BudgetReset {{win ""}} {
  global OldBudget BudgetRoadFund BudgetFireFund BudgetPoliceFund BudgetTaxRate
  if {([sim RoadFund] != $BudgetRoadFund) ||
      ([sim FireFund] != $BudgetFireFund) ||
      ([sim PoliceFund] != $BudgetPoliceFund) ||
      ([sim TaxRate] != $BudgetTaxRate)} {
    UISetMessage "The budget was reset."
    if {[sim Players] > 1} {
      UIMakeSound edit Sorry
    }
  } else {
    UISetMessage "The budget was reset."
  }
  sim RoadFund $BudgetRoadFund
  sim FireFund $BudgetFireFund
  sim PoliceFund $BudgetPoliceFund 
  sim TaxRate $BudgetTaxRate
  set OldBudget 0
  ChangeBudget
}


proc BudgetCancel {{win ""}} {
  BudgetReset
  WithdrawBudgets
  sim Resume
  MakeRunningSound
}


proc SetTaxRate {rate} {
  sim TaxRate $rate
}


proc BudgetSetTaxRate {rate} {
  SetTaxRate $rate
  ChangeBudget
}


proc BudgetSetRoadFund {percent} {
  sim RoadFund $percent
  ChangeBudget
}


proc BudgetSetFireFund {percent} {
  sim FireFund $percent
  ChangeBudget
}


proc BudgetSetPoliceFund {percent} {
  sim PoliceFund $percent
  ChangeBudget
}


proc UIShowBudgetAndWait {} {
  global OldBudget BudgetRoadFund BudgetFireFund BudgetPoliceFund BudgetTaxRate
  if {$OldBudget == 0} {
    set BudgetRoadFund [sim RoadFund]
    set BudgetFireFund [sim FireFund]
    set BudgetPoliceFund [sim PoliceFund]
    set BudgetTaxRate [sim TaxRate]
    set OldBudget 1
  }
  ShowBudgets
  UISetMessage "Pausing to set the budget ..."
  sim Pause
  MakeRunningSound
  StartBudgetTimer
  InitVotesForBudget
  sim UpdateBudget
  sim UpdateBudgetWindow
}


proc ToggleBudgetTimer {} {
  global BudgetTimerActive
  if {$BudgetTimerActive} {
    StopBudgetTimer
  } else {
    StartBudgetTimer
  }
}


proc StopBudgetTimer {} {
  global BudgetTimerActive
  set BudgetTimerActive 0
  UpdateBudgetTimer
}


proc StartBudgetTimer {} {
  global BudgetTimerActive BudgetTimer BudgetTimeout
  set me [Unique]
  set BudgetTimerActive $me
  set BudgetTimer $BudgetTimeout
  UpdateBudgetTimer
  after 1000 TickBudgetTimer $me
}


proc RestartBudgetTimer {} {
  global BudgetTimerActive
  if {$BudgetTimerActive} {
    StopBudgetTimer
    StartBudgetTimer
  }
}


proc UpdateBudgetTimer {} {
  global BudgetWindows BudgetTimerActive BudgetTimer
  if {$BudgetTimerActive} {
    set text [format "Auto Cancel In %d Seconds (click to disable)" $BudgetTimer]
  } else {
    set text [format "Enable Auto Cancel (currently disabled)"]
  }
  foreach win $BudgetWindows {
    set t [WindowLink $win.timer]
    $t config -text "$text"
  }
}


proc TickBudgetTimer {me} {
  global BudgetTimerActive BudgetTimer BudgetTimeout
  if {$BudgetTimerActive == $me} {
    incr BudgetTimer -1
    if {$BudgetTimer < 0} {
      StopBudgetTimer
      UpdateBudgetTimer
      FireBudgetTimer
    } else {
      UpdateBudgetTimer
      after 1000 TickBudgetTimer $me
    }
  }
}


proc FireBudgetTimer {} {
  BudgetCancel
}


proc funds {n} {
  sim Funds $n
}


proc ToggleAutoBudget {} {
  global AutoBudget
  if {$AutoBudget} {
      #echo "ToggleAutoBudget found on, clearing"
      sim AutoBudget 0
  } else {
      #echo "ToggleAutoBudget found off, setting"
      sim AutoBudget 1
  }
}


proc UIUpdateBudget {} {
  UpdateAutoBudget
}


proc UpdateAutoBudget {} {
  global BudgetWindows AutoBudget
  if {[sim AutoBudget]} {
    set text "Disable Auto Budget (currently enabled)"
  } else {
    set text "Enable Auto Budget (currently disabled)"
  }
  #echo "UpdateAutoBudget $AutoBudget $text"
  foreach win $BudgetWindows {
    set t [WindowLink $win.autobudget]
    $t config -text "$text"
  }
}


proc UISetBudget {cashflow previous current collected taxrate} {
  global BudgetWindows BudgetsVisible

  #echo "UISetBudgetValues $taxrate $BudgetsVisible"

  if {$BudgetsVisible} {

    foreach win $BudgetWindows {
      set w [WindowLink $win.cashflow]
      set old [lindex [$w configure -text] 4]
      if {$old != $cashflow} {
	$w configure -text $cashflow
      }
      set w [WindowLink $win.previous]
      set old [lindex [$w configure -text] 4]
      if {$old != $previous} {
	$w configure -text $previous
      }
      set w [WindowLink $win.current]
      set old [lindex [$w configure -text] 4]
      if {$old != $current} {
	$w configure -text $current
      }
      set w [WindowLink $win.collected]
      set old [lindex [$w configure -text] 4]
      if {$old != $collected} {
	$w configure -text $collected
      }

      set w [WindowLink $win.taxrate]
      set old [$w get]
      if {$old != $taxrate} {
	$w set $taxrate
      }

      set w [WindowLink $win.taxlabel]
      set old [lindex [$w configure -text] 4]
      set new "$taxrate%"
      if {$old != $new} {
	$w configure -text $new
      }
    }
  }

  global HeadWindows
  foreach win $HeadWindows {

    set w [WindowLink $win.taxrate]
    set old [$w get]
    if {$old != $taxrate} {
      # FIXME: this might not work if the slider is disabled in multi player mode.
      $w set $taxrate
    }

    set w [WindowLink $win.taxlabel]
    set old [lindex [$w configure -text] 4]
    set new "Tax Rate: $taxrate%"
    if {$old != $new} {
      $w configure -text $new
    }
  }
}


proc UISetBudgetValues {roadgot roadwant roadpercent policegot policewant policepercent firegot firewant firepercent} {
  global BudgetWindows BudgetsVisible

  if {$BudgetsVisible == 0} {
      return;
  }

  foreach win $BudgetWindows {
    set w [WindowLink $win.fire].request
    set old [lindex [$w configure -text] 4]
    set new "$firepercent% of $firewant = $firegot"
    if {$old != $new} {
      $w configure -text $new
    }
    set w [WindowLink $win.fire].fund
    set old [$w get]
    if {$old != $firepercent} {
      $w set $firepercent
    }

    set w [WindowLink $win.police].request
    set old [lindex [$w configure -text] 4]
    set new "$policepercent% of $policewant = $policegot"
    if {$old != $new} {
      $w configure -text $new
    }
    set w [WindowLink $win.police].fund
    set old [$w get]
    if {$old != $policepercent} {
      $w set $policepercent
    }

    set w [WindowLink $win.road].request
    set old [lindex [$w configure -text] 4]
    set new "$roadpercent% of $roadwant = $roadgot"
    if {$old != $new} {
      $w configure -text $new
    }
    set w [WindowLink $win.road].fund
    set old [$w get]
    if {$old != $roadpercent} {
      $w set $roadpercent
    }
  }
}


proc ChangeBudget {} {
  global VotesForBudget
  if {"$VotesForBudget" != ""} {
    InitVotesForBudget
  }
  RestartBudgetTimer
}


proc InitVotesForBudget {} {
  global VotesForBudget BudgetWindows
  set VotesForBudget {}
  foreach win $BudgetWindows {
    [WindowLink $win.vote] config -relief raised
  }
  UpdateVotesForBudget
}


proc UpdateVotesForBudget {} {
  global BudgetWindows
  UpdateVotesFor Budget $BudgetWindows
}


proc UpdateTaxRateSliders {} {
  global HeadWindows
  set players [sim Players]
  foreach win $HeadWindows {
    set slider [WindowLink $win.taxrate]
    #echo "UpdateTaxRateSliders players $players win $win slider $slider"
    if {$players == 1} {
      $slider configure -state normal
    } else {
      $slider configure -state disabled
    }
  }
}


########################################################################
# Evaluation Window Handlers


proc ToggleEvaluationOf {head} {
  global State
  if {"$State" != "play"} {
    return
  }

  set mapped 0
  set win [WindowLink $head.evaluation]
  if {$win != ""} {
    set mapped [winfo ismapped $win]
  }

  if {$mapped} {
    WithdrawEvaluationOf $head
  } else {
    ShowEvaluationOf $head
  }

}


proc WithdrawEvaluationOf {head} {
  set win [WindowLink $head.evaluation]
  if {"$win" != ""} {
    pack unpack $win
  }
}


proc ShowEvaluationOf {head} {
  set win [WindowLink $head.evaluation]
  if {$win == {}} {
    set win [MakeWindow.evaluation $head [winfo screen $head]]
  }
  #wm raise $win
  #wm deiconify $win
  set parent [WindowLink $win.parent]
  #pack append [WindowLink $head.col2]\
  #  $parent {top frame nw fill}
  pack append $parent\
    $win {top frame nw fillx}
  update idletasks
  sim UpdateEvaluation
}


proc WithdrawEvaluations {} {
  global EvaluationWindows EvaluationsVisible
  foreach win $EvaluationWindows {
    #wm withdraw $win
    #set parent [WindowLink $win.parent]
    #pack unpack $parent
    pack unpack $win
  }
  set EvaluationsVisible 0
}


proc EvaluationVisible {w v} {
    global EvaluationsVisible
    global [set var $w.visible]
    
    set $var $v

    if ($v) {
	set EvaluationsVisible [expr "$EvaluationsVisible + 1"]
    } else {
	set EvaluationsVisible [expr "$EvaluationsVisible - 1"]
    }
}


proc UISetEvaluation {changed score ps0 ps1 ps2 ps3 pv0 pv1 pv2 pv3 pop delta assessed cityclass citylevel goodyes goodno title} {
  global EvaluationWindows EvaluationsVisible CurrentDate

  set class [string tolower $cityclass]
  UISetMessage "$CurrentDate: Score $score, $class population $pop."

  if {$EvaluationsVisible} {

    foreach win $EvaluationWindows {

      global [set var $win.visible]
      set visible [eval ident "\$\{$var\}"]

      if {$visible} {

	# TODO: set evaluation window and icon title
	#wm title $win "$title"
	#wm iconname $win "$title"

	set w [WindowLink $win.goodjob]
	set old [lindex [$w configure -text] 4]
	set new "$goodyes\n$goodno"
	if {$old != $new} {
	  $w configure -text $new
	}

	set w [WindowLink $win.problemnames]
	set old [lindex [$w configure -text] 4]
	set new "$ps0\n$ps1\n$ps2\n$ps3"
	if {$old != $new} {
	  $w configure -text $new
	}

	set w [WindowLink $win.problempercents]
	set old [lindex [$w configure -text] 4]
	set new "$pv0\n$pv1\n$pv2\n$pv3"
	if {$old != $new} {
	  $w configure -text $new
	}

	set w [WindowLink $win.stats]
	set old [lindex [$w configure -text] 4]
	set new "$pop\n$delta\n\n$assessed\n$cityclass\n$citylevel"
	if {$old != $new} {
	  $w configure -text $new
	}

	set w [WindowLink $win.score]
	set old [lindex [$w configure -text] 4]
	set new "$score\n$changed"
	if {$old != $new} {
	  $w configure -text $new
	}
      }
    }
  }
}


########################################################################
# File Window Handlers


proc ShowFileOf {head} {
  set win [WindowLink $head.file]
  if {$win == {}} {
    set win [MakeWindow.file $head [winfo screen $head]]
  }
  wm deiconify $win
  wm raise $win
  return $win
}


proc ShowFiles {} {
  global HeadWindows
  foreach win $HeadWindows {
    ShowFileOf $win
  }
}


proc WithdrawFiles {} {
  global FileWindows
  foreach win $FileWindows {
    wm withdraw $win
  }
}


proc DoFileDialog {win Message Path Pattern FileName ActionOk ActionCancel} {
  ShowFileDialog $win "$Path" "$Pattern"
  $win.message1 configure -text "$Message"
  $win.path.path delete 0 end
  $win.path.path insert 0 $Path
  $win.file.file delete 0 end
  $win.file.file insert 0 "$FileName"
  $win.frame1.ok config -command "
      $ActionOk \[$win.file.file get\] \[$win.path.path get\]
      wm withdraw $win"
  $win.frame1.rescan config -command "
      ShowFileDialog $win \[$win.path.path get\] $Pattern"
  $win.frame1.cancel config -command "
      $ActionCancel
      wm withdraw $win"
  bind $win.files.files "<Double-Button-1>" "\
    FileSelectDouble $win %W %y $Pattern \"
	$ActionOk \[$win.file.file get\] \[$win.path.path get\]\""
  bind $win.path.path <Return> "
    ShowFileDialog $win \[$win.path.path get\] $Pattern
    $win.file.file cursor 0
    focus $win.file.file"
  bind $win.file.file <Return> "\
    $ActionOk \[$win.file.file get\] \[$win.path.path get]
    wm withdraw $win"
}


proc BindSelectOne {win Y} {
  set Nearest [$win nearest $Y]
  if {$Nearest >= 0} {
    $win select from $Nearest
    $win select to $Nearest
  }
}


proc FileSelect {win widget Y} {
  BindSelectOne $widget $Y
  set Nearest [$widget nearest $Y]
  if {$Nearest >= 0} {
    set Path [$win.path.path get]
    set TmpEntry [$widget get $Nearest]
    if {[string compare "/" [string index $TmpEntry \
          [expr [string length $TmpEntry]-1]]] == 0 || \
        [string compare "@" [string index $TmpEntry \
          [expr [string length $TmpEntry]-1]]] == 0} {
      # handle directories, and symbolic links to directories
      set FileName [string range $TmpEntry 0 \
            [expr [string length $TmpEntry]-2]]
      # whoops / or @ is part of the name
      if {[MiscIsDir $Path/$FileName] != 1} {
        set FileName $TmpEntry
      }
    } {
      if {[string compare "*" [string index $TmpEntry \
            [expr [string length $TmpEntry]-1]]] == 0} {
        # handle executable filenames
        set FileName [string range $TmpEntry 0 \
              [expr [string length $TmpEntry]-2]]
        # whoops * is part of the name
        if {[file executable $Path/$FileName] != 1} {
          set FileName $TmpEntry
        }
      } {
        # a ordinary filename
        set FileName $TmpEntry
      }
    }
    # enter the selected filename into the filename field
    if {[MiscIsDir $Path/$FileName] != 1} {
      $win.file.file delete 0 end
      $win.file.file insert 0 $FileName
    }
  }
}


proc FileSelectDouble {win widget Y Pattern Action} {
  BindSelectOne $widget $Y
  set Nearest [$widget nearest $Y]
  if {$Nearest >= 0} {
    set Path [$win.path.path get]
    set TmpEntry [$widget get $Nearest]
    if {[string compare $TmpEntry "../"] == 0} {
      # go up one directory
      set TmpEntry \
        [string trimright [string trimright [string trim $Path] /] @]
      if {[string length $TmpEntry] <= 0} {
        return
      }
      set Path [file dirname $TmpEntry]
      $win.path.path delete 0 end
      $win.path.path insert 0 $Path
      ShowFileDialog $win $Path $Pattern
    } {
      if {[string compare "/" [string index $TmpEntry \
            [expr [string length $TmpEntry]-1]]] == 0 || \
          [string compare "@" [string index $TmpEntry \
            [expr [string length $TmpEntry]-1]]] == 0} {
        # handle directorys, and symbolic links to directorys
        set FileName [string range $TmpEntry 0 \
              [expr [string length $TmpEntry]-2]]
        # whoops / or @ is part of the name
        if {[MiscIsDir $Path/$FileName] != 1} {
          set FileName $TmpEntry
        }
      } {
        if {[string compare "*" [string index $TmpEntry \
              [expr [string length $TmpEntry]-1]]] == 0} {
          # handle executable filenames
          set FileName [string range $TmpEntry 0 \
                [expr [string length $TmpEntry]-2]]
          # whoops * is part of the name
          if {[file executable $Path/$FileName] != 1} {
            set FileName $TmpEntry
          }
        } {
          # a ordinary filename
          set FileName $TmpEntry
        }
      }
      # change directory
      if {[MiscIsDir $Path/$FileName] == 1} {
        if {[string compare "/" [string index $Path \
              [expr [string length $Path]-1]]] == 0} {
           append Path $FileName
        } {
          append Path / $FileName
        }
        $win.path.path delete 0 end
        $win.path.path insert 0 $Path
        ShowFileDialog $win $Path $Pattern
      } {
        # enter the selected filename into the filename field
	$win.file.file delete 0 end
	$win.file.file insert 0 "$FileName"
        if {[string length $Action] > 0} {
          eval $Action
        }
        wm withdraw $win
      }
    }
  }
}


proc NameComplete {win Type} {

  set NewFile ""
  set Matched ""

  if {[string compare $Type path] == 0} {
    set DirName [file dirname [$win.path.path get]]
    set FileName [file tail [$win.path.path get]]
  } {
    set DirName [file dirname [$win.path.path get]/]
    set FileName [file tail [$win.file.file get]]
  }

  set FoundCounter 0
  if {[MiscIsDir $DirName] == 1} {
    catch "exec ls $DirName/" Result
    set Counter 0
    set ListLength [llength $Result]
    # go through list
    while {$Counter < $ListLength} {
      if {[string length $FileName] == 0} {
        if {$FoundCounter == 0} {
          set NewFile [lindex $Result $Counter]
        } {
          set Counter1 0
          set TmpFile1 $NewFile
          set TmpFile2 [lindex $Result $Counter]
          set Length1 [string length $TmpFile1]
          set Length2 [string length $TmpFile2]
          set NewFile ""
          if {$Length1 > $Length2} {
            set Length1 $Length2
          }
          while {$Counter1 < $Length1} {
            if {[string compare [string index $TmpFile1 $Counter1] \
                  [string index $TmpFile2 $Counter1]] == 0} {
              append NewFile [string index $TmpFile1 $Counter1]
            } {
              break
            }
            incr Counter1 1
          }
        }
        incr FoundCounter 1
      } {
        if {[regexp "^$FileName" [lindex $Result $Counter] \
              Matched] == 1} {
          if {$FoundCounter == 0} {
            set NewFile [lindex $Result $Counter]
          } {
            set Counter1 0
            set TmpFile1 $NewFile
            set TmpFile2 [lindex $Result $Counter]
            set Length1 [string length $TmpFile1]
            set Length2 [string length $TmpFile2]
            set NewFile ""
            if {$Length1 > $Length2} {
              set Length1 $Length2
            }
            while {$Counter1 < $Length1} {
              if {[string compare [string index $TmpFile1 $Counter1] \
                    [string index $TmpFile2 $Counter1]] == 0} {
                append NewFile [string index $TmpFile1 $Counter1]
              } {
                break
              }
              incr Counter1 1
            }
          }
          incr FoundCounter 1
        }
      }
      incr Counter 1
    }
  }

  if {$FoundCounter == 1} {
    if {[MiscIsDir $DirName/$NewFile] == 1} {
      if {[string compare $DirName "/"] == 0} {
        $win.path.path delete 0 end
        $win.path.path insert 0 "/[string trim [string trim $NewFile /] @]/"
      } {
        $win.path.path delete 0 end
        $win.path.path insert 0 "[string trimright $DirName /]/[string trim [string trim $NewFile /] @]/"
      }
    } {
      $win.path.path delete 0 end
      $win.path.path insert 0 \
        "[string trim [string trimright $DirName /] @]/"
      $win.file.file delete 0 end
      $win.file.file insert 0 "$NewFile"
    }
  } {
    if {[MiscIsDir $DirName/$NewFile] == 1 ||
        [string compare $Type path] == 0} {
      $win.path.path delete 0 end
      $win.path.path insert 0 \
        "[string trimright $DirName /]/[string trim [string trim $NewFile /] @]"
    } {
      $win.path.path delete 0 end
      $win.path.path insert 0 "$DirName"
      if {[string length $NewFile] > 0} {
        $win.file.file delete 0 end
        $win.file.file insert 0 "$NewFile"
      }
    }
  }
}


proc ShowFileDialog {win Path Pattern} {
  busy $win {
    set Path [lindex [split $Path] 0]
    if {[$win.files.files size] > 0} {
      $win.files.files delete 0 end
    }
    # read directory
    if {[catch "exec ls -F $Path" Result]} {
      set ElementList {}
    }
    if {[string match $Result "* not found"]} {
      set ElementList {}
    }
    set ElementList [lsort $Result]

    # insert ..
    if {[string compare $Path "/"]} {
      $win.files.files insert end "../"
    }

    # walk through list
    foreach Counter $ElementList {
      # insert filename
      if {[string match $Pattern $Counter] == 1} {
	if {[string compare $Counter "../"] &&
	    [string compare $Counter "./"]} {
	  $win.files.files insert end $Counter
	}
      } else {
        set fn $Path/[string trim [string trim [string trim $Counter /] @] *]
	if {[MiscIsDir $fn]} {
	  $win.files.files insert end $Counter
	}
      }
    }
  }
}


proc MiscIsDir {PathName} {

  if {[file isdirectory $PathName] == 1} {
    return 1
  } {
    catch "file type $PathName" Type
    if {[string compare $Type link] == 0} {
      set LinkName [file readlink $PathName]
      catch "file type $LinkName" Type
      while {[string compare $Type link] == 0} {
        set LinkName [file readlink $LinkName]
      }
      return [file isdirectory $LinkName]
    }
  }
  return 0
}


proc busy {win cmds} {
    set busy {}
    set list [winfo children $win]
    set busy $list
    while {$list != ""} {
	set next {}
	foreach w $list {
	    set class [winfo class $w]
	    set cursor [lindex [$w config -cursor] 4]
	    if {[winfo toplevel $w] == $w} {
		lappend busy [list $w $cursor]
	    }
	    set next [concat $next [winfo children $w]]
	}
	set list $next
    }

    foreach w $busy {
	catch {[lindex $w 0] config -cursor watch}
    }

    update idletasks

    set error [catch {uplevel eval [list $cmds]} result]

    foreach w $busy {
	catch {[lindex $w 0] config -cursor [lindex $w 1]}
    }

    if $error {
	error $result
    } else {
	return $result
    }
}


########################################################################
# Editor Window Handlers

proc ShowEditorOf {head} {
  global EditorWindows
  set found 0
  foreach win $EditorWindows {
    if {[WindowLink $win.head] == $head} {
      set parent [WindowLink $win.parent]
      #pack append [WindowLink $head.col2]\
      #  $parent {top frame nw expand fill}
      pack append $parent\
	$win {top frame nw expand fill}
      set found 1
    }
  }
  if {$found == 0} {
    NewEditorOf $head
  } else {
    update idletasks
    sim UpdateEditors
    sim UpdateMaps
  }
}


proc NewEditorOf {head} {
  set win [MakeWindow.editor $head [winfo screen $head]]
  #wm deiconify $win
  set parent [WindowLink $win.parent]
  pack append $parent\
    $win {top frame nw expand fill}
  update idletasks
  sim UpdateEditors
  sim UpdateMaps
}


proc ShowEditors {} {
  global HeadWindows
  foreach win $HeadWindows {
    ShowEditorOf $win
  }
}


proc WithdrawEditors {} {
  global EditorWindows
  foreach win $EditorWindows {
    #set parent [WindowLink $win.parent]
    #pack unpack $parent
    pack unpack $win
  }
}


proc InitEditors {} {
  global EditorWindows
  foreach win $EditorWindows {
    InitEditor $win
  }
}


proc InitEditor {win} {
  set e [WindowLink $win.view]
  UISetToolState $win 7
  $e ToolState 7
  set size [$e size]
  $e Pan 960 800
  $e AutoGoing 0
  global $e.TrackState
  set $e.TrackState {}
}


proc SetEditorAutoGoto {win val} {
  global AutoGoto.$win
  set AutoGoto.$win $val
  set e [WindowLink $win.view]
  $e AutoGoto $val
}


proc SetEditorControls {win val} {
  global Controls.$win
  set Controls.$win $val
  if {$val} {
    pack append $win $win.leftframe {left frame center filly} 
  } else {
    pack unpack $win.leftframe
  }
}


proc SetEditorOverlay {win val} {
  global Overlay.$win
  set Overlay.$win $val
  set e [WindowLink $win.view]
  $e ShowOverlay $val
}


proc SetEditorDynamicFilter {win val} {
  global DynamicFilter.$win
  set DynamicFilter.$win $val
  set e [WindowLink $win.view]
  $e DynamicFilter $val
  if {$val == 1} then {
    ShowFrobOf [WindowLink $win.head]
  }
}


proc SetEditorSkip {win val} {
  set e [WindowLink $win.view]
  $e Skip $val
}


proc EditorToolDown {mod w x y} {
  global [set var $w.TrackState]

  $w ToolMode 1

  case [$w ToolState] in \
    7 { # bulldozer
      UIMakeSoundOn $w edit Rumble "-repeat 4"
    } \
    10 { # chalk
      StartChalk $w
    }

  case $mod in \
    constrain {
      set $var [list constrain_start $x $y]
      $w ToolConstrain $x $y
    } \
    default {
      set $var none
    }
  EditorTool ToolDown $w $x $y
  sim NeedRest 5
}


proc EditorToolDrag {w x y} {
  EditorTool ToolDrag $w $x $y
  sim NeedRest 5
}


proc EditorToolUp {w x y} {
  global [set var $w.TrackState]
  $w ToolMode 0

  case [$w ToolState] in \
    7 { # bulldozer
     UIStopSoundOn $w edit 1
    } \
    10 { # chalk
      StopChalk $w
    }

  EditorTool ToolUp $w $x $y
  set $var {}
  $w ToolConstrain -1 -1
  sim UpdateMaps
  sim UpdateEditors
  sim NeedRest 5
}


proc EditorTool {action w x y} {
  global [set var $w.TrackState]
  set state [eval ident "\$\{$var\}"]
  case [lindex $state 0] in \
    constrain_start {
      set x0 [lindex $state 1]
      set y0 [lindex $state 2]
      set dx [expr "$x - $x0"]
      set dy [expr "$y - $y0"]
      if [expr "($dx > 16) || ($dx < -16)"] then {
        $w ToolConstrain -1 $y0
        set $var none
      } else {
	if [expr "($dy > 16) || ($dy < -16)"] then {
	  $w ToolConstrain $x0 -1
	  set $var none
	}
      }
    }
  $w $action $x $y
}


proc StartChalk {w} {
  sim CollapseMotion 0
}


proc StopChalk {w} {
  sim CollapseMotion 1
}


proc EditorPanDown {mod w x y} {
  global [set var $w.TrackState]
  $w ToolMode -1
  case $mod in \
    constrain {
      set $var [list constrain_start $x $y]
      $w ToolConstrain $x $y
    } \
    default {
      set $var none
    }
  EditorTool PanStart $w $x $y
}


proc EditorPanDrag {w x y} {
  EditorTool PanTo $w $x $y
}


proc EditorPanUp {w x y} {
  $w AutoGoing 0
  $w ToolMode 0
  EditorTool PanTo $w $x $y
  $w ToolConstrain -1 -1
  sim UpdateMaps
  sim UpdateEditors
}


proc EditorKeyDown {w k} {
  $w KeyDown $k
}


proc EditorKeyUp {w k} {
  $w KeyUp $k
}


proc BindEditorButtons {win} {
  set w [WindowLink $win.top]

  bind $win <1> "CancelPie $win ; EditorToolDown none %W %x %y"
  bind $win <B1-Motion> {EditorToolDrag %W %x %y}
  bind $win <ButtonRelease-1> {EditorToolUp %W %x %y}

  bind $win <Control-1> "CancelPie $win ; EditorToolDown constrain %W %x %y"
  bind $win <Control-B1-Motion> {EditorToolDrag %W %x %y}
  bind $win <Control-ButtonRelease-1> {EditorToolUp %W %x %y}

  bind $win <2> "CancelPie $win ; EditorPanDown none %W %x %y"
  bind $win <B2-Motion> {EditorPanDrag %W %x %y}
  bind $win <ButtonRelease-2> {EditorPanUp %W %x %y}

  bind $win <Control-2> "CancelPie $win ; EditorPanDown constrain %W %x %y"
  bind $win <Control-B2-Motion> {EditorPanDrag %W %x %y}
  bind $win <Control-ButtonRelease-2> {EditorPanUp %W %x %y}

  InitPie $win $w.toolpie
}


proc UISetFunds {funds} {
  global HeadWindows
  foreach win $HeadWindows {
    [WindowLink $win.funds] configure -text "$funds"
  }
}


proc UISetDate {date month year} {
  global HeadWindows
  global CurrentDate
  
  set CurrentDate "$date"

  foreach win $HeadWindows {
    [WindowLink $win.date] Set $month $year
  }
}


proc SetPriority {index} {
  case $index { \
    {0} {
      sim Delay 500000
      sim Skips 0
    } \
    {1} {
      sim Delay 100000
      sim Skips 0
    } \
    {2} {
      sim Delay 25000
      sim Skips 0
    } \
    {3} {
      sim Delay 5000
      sim Skips 20
    } \
    {4} {
      sim Delay 5
      sim Skips 500
    }
  }
}


proc UISetDemand {r c i} {
  global HeadWindows DemandRes DemandCom DemandInd

  set DemandRes $r
  set DemandCom $c
  set DemandInd $i

  if {$r <= 0} then {set ry0 32} else {set ry0 24}
  set ry1 [expr "$ry0 - $r"]
  if {$c <= 0} then {set cy0 32} else {set cy0 24}
  set cy1 [expr "$cy0 - $c"]
  if {$i <= 0} then {set iy0 32} else {set iy0 24}
  set iy1 [expr "$iy0 - $i"]

  foreach win $HeadWindows {
    set can [WindowLink $win.demand]
#    $can coords r 8 $ry0 14 $ry1
#    $can coords c 17 $cy0 23 $cy1
#    $can coords i 26 $iy0 32 $iy1
    $can coords r 49 $ry0 55 $ry1
    $can coords c 58 $cy0 64 $cy1
    $can coords i 67 $iy0 73 $iy1
  }
}


proc UISetOptions {autobudget autogoto autobulldoze disasters sound animation messages notices} {
  global AutoBudget AutoGoto AutoBulldoze Disasters Sound
  set AutoBudget $autobudget
  set AutoGoto $autogoto
  set AutoBulldoze $autobulldoze
  set Disasters $disasters
  set Sound $sound
  set DoAnimation $animation
  set DoMessages $messages
  set DoNotices $notices
}


proc UIDidToolRes {win x y} {
  UIMakeSoundOn $win edit O "-speed 140"
}


proc UIDidToolCom {win x y} {
  UIMakeSoundOn $win edit A "-speed 140"
}


proc UIDidToolInd {win x y} {
  UIMakeSoundOn $win edit E "-speed 140"
}


proc UIDidToolFire {win x y} {
  UIMakeSoundOn $win edit O "-speed 130"
}


proc UIDidToolQry {win x y} {
  UIMakeSoundOn $win edit E "-speed 200"
}


proc UIDidToolPol {win x y} {
  UIMakeSoundOn $win edit E "-speed 130"
}


proc UIDidToolWire {win x y} {
  UIMakeSoundOn $win edit O "-speed 120"
}


proc UIDidToolDozr {win x y} {
  UIMakeSoundOn $win edit Rumble
}


proc UIDidToolRail {win x y} {
  UIMakeSoundOn $win edit O "-speed 100"
}


proc UIDidToolRoad {win x y} {
  UIMakeSoundOn $win edit E "-speed 100"
}


proc UIDidToolChlk {win x y} {
}


proc UIDidToolEraser {win x y} {
}


proc UIDidToolStad {win x y} {
  UIMakeSoundOn $win edit O "-speed 90"
}


proc UIDidToolPark {win x y} {
  UIMakeSoundOn $win edit A "-speed 130"
}


proc UIDidToolSeap {win x y} {
  UIMakeSoundOn $win edit E "-speed 90"
}


proc UIDidToolCoal {win x y} {
  UIMakeSoundOn $win edit O "-speed 75"
}


proc UIDidToolNuc {win x y} {
  UIMakeSoundOn $win edit E "-speed 75"
}


proc UIDidToolAirp {win x y} {
  UIMakeSoundOn $win edit A "-speed 50"
}


proc UISetToolState {w state} {
  global EditorPallets EditorPalletImages ToolInfo
  set win [WindowLink $w.top]
  #echo "UISETTOOLSTATE w $w win $win $state"
  ExclusivePallet $state $win $EditorPallets ic $EditorPalletImages \
	raised sunken {NoFunction}
	{NoFunction}
  set c1 [WindowLink $w.cost1]
  if {"$c1" != ""} {
    set info [lindex $ToolInfo $state]
    set cost1 [lindex $info 1]
    set cost2 [lindex $info 2]
    $c1 configure -text "$cost1"
    [WindowLink $w.cost2] configure -text "$cost2"
  }
}


proc UIShowZoneStatus {zone density value crime pollution growth x y} {
  global QueryX QueryY
  set QueryX [expr "8 + 16 * $x"]
  set QueryY [expr "8 + 16 * $y"]
  UIShowPicture 9 [list $zone $density $value $crime $pollution $growth]
}


########################################################################
# Map Window Handlers


proc ShowMapOf {head} {
  global MapWindows
  set found 0
  foreach win $MapWindows {
    if {"[WindowLink $win.head]" == "$head"} {
      global MapPanelWidth MapPanelHeight
      #place configure $win -x 0 -y 0 -width $MapPanelWidth -height $MapPanelHeight
      #[WindowLink $win.parent] configure -width $MapPanelWidth -height $MapPanelHeight
      set parent [WindowLink $win.parent]
      pack append [WindowLink $head.w2] \
	  $win {top frame nw expand fill}
      set found 1
    }
  }
  if {$found == 0} {
    NewMapOf $head
  } else {
    update idletasks
    sim UpdateMaps
  }
}


proc NewMapOf {head} {
  set win [MakeWindow.map $head [winfo screen $head]]
  global MapPanelWidth MapPanelHeight
  #place configure $win -x 0 -y 0 -width $MapPanelWidth -height $MapPanelHeight
  #[WindowLink $win.parent] configure -width $MapPanelWidth -height $MapPanelHeight
  set parent [WindowLink $win.parent]
  #pack append [WindowLink $head.col1]\
  #  $parent {top frame nw fillx}
  pack append [WindowLink $head.w2] \
      $win {top frame nw expand fill}
  sim UpdateMaps
}


proc ShowMaps {} {
  global HeadWindows
  foreach win $HeadWindows {
    ShowMapOf $win
  }
}


proc WithdrawMaps {} {
  global MapWindows
  foreach win $MapWindows {
    #place forget $win
    pack unpack $win
  }
}


proc InitMaps {} {
  global MapWindows
  foreach win $MapWindows {
    InitMap $win
  }
}


proc InitMap {win} {
  SetMapState $win 0
}


proc EnableMaps {} {
  global MapWindows
  foreach win $MapWindows {
    EnableMap $win
  }
}


proc EnableMap {win} {
  [WindowLink $win.view] ShowEditors 1
  [WindowLink $win.zones] config -state normal
  [WindowLink $win.overlays] config -state normal
}


proc DisableMaps {} {
  global MapWindows
  foreach win $MapWindows {
    DisableMap $win
  }
}


proc DisableMap {win} {
  [WindowLink $win.view] ShowEditors 0
  [WindowLink $win.zones] config -state disabled
  [WindowLink $win.overlays] config -state disabled
}


proc SetMapState {win state} {
  set m [WindowLink $win.view]
  $m MapState $state
  if {$state == 14} then {
    ShowFrobOf [WindowLink $win.head]
  }
}


proc MapPanDown {w x y} {
  $w PanStart $x $y
}


proc MapPanDrag {w x y} {
  $w PanTo $x $y
}


proc MapPanUp {w x y} {
  $w PanTo $x $y
  sim UpdateMaps
  sim UpdateEditors
}


proc UISetMapState {w state} {
  global MapTitles
  #set win [winfo toplevel $w]
  set win [WindowLink $w.win]
  set m [WindowLink $win.view]
  set title [lindex $MapTitles $state]

  # TODO: set map text field header to title
  #wm title $win "$title"
  #wm iconname $win "$title"

  global [set var MapState.$win]
  set $var $state

  case $state { \
    {6 8 9 10 11 12 13} {
      [WindowLink $win.legend] config -bitmap "@images/legendmm.xpm"
    } \
    {7} {
      [WindowLink $win.legend] config -bitmap "@images/legendpm.xpm"
    } \
    {0 1 2 3 4 5 14} {
      [WindowLink $win.legend] config -bitmap "@images/legendn.xpm"
    }
  }
}


########################################################################
# Graph Window Handlers


proc ToggleGraphOf {head} {
  global State
  if {"$State" != "play"} {
    return
  }

  set mapped 0
  set win [WindowLink $head.graph]
  if {$win != ""} {
    set mapped [winfo ismapped $win]
  }

  if {$mapped} {
    WithdrawGraphOf $head
  } else {
    ShowGraphOf $head
  }

}


proc WithdrawGraphOf {head} {
  set win [WindowLink $head.graph]
  if {"$win" != ""} {
    pack unpack $win
  }
}


proc ShowGraphOf {head} {
  set win [WindowLink $head.graph]
  if {$win == {}} {
    set win [MakeWindow.graph $head [winfo screen $head]]
  }
  #wm deiconify $win
  #wm raise $win
  set parent [WindowLink $win.parent]
  pack append $parent\
    $win {top frame nw fillx}
  update idletasks
  sim UpdateGraphs
}


proc WithdrawGraphs {} {
  global GraphWindows
  foreach win $GraphWindows {
    #wm withdraw $win
    pack unpack $win
  }
}


proc InitGraphs {} {
  global GraphWindows
  foreach win $GraphWindows {
    InitGraph $win
  }
}


proc InitGraph {win} {
  UISetGraphState $win 1 1 1 1 1 1 0
}


proc UISetGraphState {win t0 t1 t2 t3 t4 t5 range} {
  set g [WindowLink $win.graphview]
  GraphPalletMask $win [expr "$t0 + ($t1<<1) + ($t2<<2) + ($t3<<3) + ($t4<<4) + ($t5<<5)"]
  GraphYearPallet $win $range
}


########################################################################
# Splash Window Handlers


proc ShowSplashOf {head} {
  set win [WindowLink $head.splash]
  if {$win == {}} {
    set win [MakeWindow.splash $head [winfo screen $head]]
  }
  set splashscreen [WindowLink $win.splashscreen]
  set success 0
  catch {$splashscreen config -bitmap "@images/splashscreen.xpm"; set success 1}
  if {$success} {
    wm deiconify $win
    global SplashScreenDelay
    after $SplashScreenDelay "UIPickScenarioMode"
  } else {
    UIPickScenarioMode
  }
}


proc WithdrawSplashOf {head} {
  set win WindowLink $head.splash]
  wm withdraw $win
}


proc ShowSplashes {} {
  global HeadWindows
  foreach win $HeadWindows {
    ShowSplashOf $win
  }
}


proc WithdrawSplashes {} {
  global SplashWindows
  foreach win $SplashWindows {
    wm withdraw $win
    set splashscreen [WindowLink $win.splashscreen]
    $splashscreen config -bitmap ""
  }
}


proc InitSplashes {} {
}


proc InitSplash {win} {
}


proc DeleteSplashWindow {win} {
  # TODO
}


########################################################################
# Scenario Window Handlers


proc ShowScenarioOf {head} {
  set win [WindowLink $head.scenario]
  if {$win == {}} {
    set win [MakeWindow.scenario $head [winfo screen $head]]
  }
  # TODO: load background bitmap
  wm deiconify $win
}


proc WithdrawScenarioOf {head} {
  set win WindowLink $head.scenario]
  wm withdraw $win
}


proc ShowScenarios {} {
  global HeadWindows
  foreach win $HeadWindows {
    ShowScenarioOf $win
  }
}


proc WithdrawScenarios {} {
  global ScenarioWindows
  foreach win $ScenarioWindows {
    wm withdraw $win
    # TODO: flush background bitmap
  }
}


proc InitScenarios {} {
  global MapHistory MapHistoryNum
  if {$MapHistoryNum < 1} {
    set prev disabled
  } else {
    set prev normal
  }
  if {$MapHistoryNum == ([llength $MapHistory] - 1)} {
    set next disabled
  } else {
    set next normal
  }
  global ScenarioWindows
  foreach win $ScenarioWindows {
    # TODO
    #[WindowLink $win.previous] config -state $prev
    #[WindowLink $win.next] config -state $next
  }
}


proc InitScenario {win} {
  global MapHistory MapHistoryNum
  if {$MapHistoryNum < 1} {
    set prev disabled
  } else {
    set prev normal
  }

  if {$MapHistoryNum == ([llength $MapHistory] - 1)} {
    set next disabled
  } else {
    set next normal
  }

  # TODO
  #[WindowLink $win.previous] config -state $prev
  #[WindowLink $win.next] config -state $next

  LinkWindow $win.scenarioTarget -1
  LinkWindow $win.scenarioTargetDown -1

  global ScenarioButtons
  set i 0
  set len [llength $ScenarioButtons]
  while {$i < $len} {
    set data [lindex $ScenarioButtons $i]
    set type [lindex $data 0]
    set id [lindex $data 1]
    #echo "DATA $data"
    #echo "ID $id"

    LinkWindow $win.$id.over 0
    LinkWindow $win.$id.enabled 1
    LinkWindow $win.$id.checked 0
    #echo "SETTING WIN $win ID $id"

    set i [expr "$i + 1"]
  }

  UpdateLevelSelection $win
  UpdateLeftRightEnabled $win
}


proc UpdateLevelSelection {win} {
  #echo UpdateLevelSelection
  global GameLevel
  if {$GameLevel == 0} then {
    LinkWindow $win.easy.checked 1
    LinkWindow $win.medium.checked 0
    LinkWindow $win.hard.checked 0
  }
  if {$GameLevel == 1} then {
    LinkWindow $win.easy.checked 0
    LinkWindow $win.medium.checked 1
    LinkWindow $win.hard.checked 0
  }
  if {$GameLevel == 2} then {
    LinkWindow $win.easy.checked 0
    LinkWindow $win.medium.checked 0
    LinkWindow $win.hard.checked 1
  }

  UpdateScenarioButtonID $win easy
  UpdateScenarioButtonID $win medium
  UpdateScenarioButtonID $win hard

}


proc UpdateLeftRightEnabled {win} {
  #echo UpdateLeftRightEnabled

  global MapHistory MapHistoryNum
  if {$MapHistoryNum < 1} {
    LinkWindow $win.left.enabled 0
  } else {
    LinkWindow $win.left.enabled 1
  }

  if {$MapHistoryNum == ([llength $MapHistory] - 1)} {
    LinkWindow $win.right.enabled 0
  } else {
    LinkWindow $win.right.enabled 1
  }

  UpdateScenarioButtonID $win left
  UpdateScenarioButtonID $win right
}


proc UpdateScenarioButtonID {win id} {

  global ScenarioButtons
  set i 0
  set len [llength $ScenarioButtons]
  while {$i < $len} {
    set data [lindex $ScenarioButtons $i]
    set id 
    if {$id == [lindex $data 1]} then {
      UpdateScenarioButton $win $data
      break
    }

    set i [expr "$i + 1"]
  }
}


proc UpdateScenarioButton {win data} {
  set type [lindex $data 0]
  set id [lindex $data 1]
  set over [WindowLink $win.$id.over]
  set enabled [WindowLink $win.$id.enabled]
  set checked [WindowLink $win.$id.checked]
  #echo "WIN $win TYPE $type ID $id OVER $over ENABLED $enabled CHECKED $checked"
  if {$enabled} {
    if {$checked} {
      if {$over} {
        set bm [lindex $data 13]
      } else {
        set bm [lindex $data 12]
      }
    } else {
      if {$over} {
        set bm [lindex $data 10]
      } else {
        set bm [lindex $data 9]
      }
    }
  } else {
    set bm [lindex $data 11]
  }

  [WindowLink $win.canvas] itemconfig $id -bitmap $bm
}


proc DoEnterCityName {win} {
}


proc InitVotesForUseThisMap {} {
  # TODO: Disabled for new scenario window.
  return

  global VotesForUseThisMap ScenarioWindows
  set VotesForUseThisMap {}
  foreach win $ScenarioWindows {
    [WindowLink $win.vote] config -relief raised
  }
  UpdateVotesForUseThisMap
}


proc UpdateVotesForUseThisMap {} {
  global ScenarioWindows
  UpdateVotesFor UseThisMap $ScenarioWindows
}


proc UIUseThisMap {} {
  global CityName GameLevel Scenario
  WithdrawAll
  # special handling for scenarios?
  if {$GameLevel != -1} {
    sim GameLevel $GameLevel
  }
  sim CityName $CityName
  UINewGame
  UIPlayGame
  if {$Scenario != -1} {
    UIShowPicture $Scenario
  }
}


proc HandleScenarioDown {win x y} {
  #echo HandleScenarioDown $win $x $y
  HandleScenarioMove $win $x $y
  set w [WindowLink $win.w]
  set target [WindowLink $w.scenarioTarget]
  LinkWindow $w.scenarioTargetDown $target
}


proc HandleScenarioUp {win x y} {
  #echo HandleScenarioUp $win $x $y
  HandleScenarioMove $win $x $y
  global ScenarioButtons
  set w [WindowLink $win.w]
  set target [WindowLink $w.scenarioTarget]
  set targetDown [WindowLink $w.scenarioTargetDown]
  if {($target != -1) &&
      ($target == $targetDown)} {
    set data [lindex $ScenarioButtons $target]
    set type [lindex $data 0]
    set id [lindex $data 1]
    set callback [lindex $data 2]
    set param [lindex $data 3]
    set var [lindex $data 4]
    set xx [lindex $data 5]
    set yy [lindex $data 6]
    set ww [lindex $data 7]
    set hh [lindex $data 8]
    set normal [lindex $data 9]
    set over [lindex $data 10]
    set disabled [lindex $data 11]
    if {$type == "button"} {
      #echo callback $callback w $w param $param
      eval "$callback $w {$param}"
    } else {
      if {$type == "checkbox"} {
        #echo checkbox callback $callback w $w param $param
	eval "$callback $w {$param}"
      }
    }
  }
}


proc HandleScenarioMove {win x y} {
  #echo HandleScenarioMove $win $x $y
  global ScenarioButtons
  set w [WindowLink $win.w]
  set target [WindowLink $w.scenarioTarget]
  set found -1
  set i 0
  set len [llength $ScenarioButtons]
  while {$i < $len} {
    set data [lindex $ScenarioButtons $i]
    set type [lindex $data 0]
    set id [lindex $data 1]
    set callback [lindex $data 2]
    set param [lindex $data 3]
    set var [lindex $data 4]
    set xx [lindex $data 5]
    set yy [lindex $data 6]
    set ww [lindex $data 7]
    set hh [lindex $data 8]

    set enabled [WindowLink $w.$id.enabled]
    set checked [WindowLink $w.$id.checked]

    #echo "ID $id ENABLED $enabled CHECKED $checked w $w id $id"

    if {($enabled != 0) &&
        ($x >= $xx) &&
	($x < ($xx + $ww)) &
	($y >= $yy) &&
	($y < ($yy + $hh))} {
      set found $i
      break
    }

    set i [expr "$i + 1"]
  }

  if {$found != $target} {

    if {$found == -1} {
      if {$target != -1} { 

	#echo OUT $w $found $xx $yy $normal
	LinkWindow $w.scenarioTarget -1

        set targetdata [lindex $ScenarioButtons $target]
	set targetid [lindex $targetdata 1]
	LinkWindow $w.$targetid.over 0
        UpdateScenarioButton $w $targetdata

      }
    } else {

      #echo IN $w $found $xx $yy $over
      LinkWindow $w.scenarioTarget $found
      LinkWindow $w.$id.over 1

      if {$target != -1} {
        set targetdata [lindex $ScenarioButtons $target]
	set targetid [lindex $targetdata 1]
	LinkWindow $w.$targetid.over 0
	UpdateScenarioButton $w $targetdata
      }

      UpdateScenarioButton $w $data

    }
  }
}

proc DoLoad {win param} {
  #echo DOLOAD $win $param
  UILoadCity $win
}


proc DoGenerate {win param} {
  #echo DOGENERATE $win $param
  UIGenerateNewCity
}


proc DoQuit {win param} {
  #echo DOQUIT $win $param
  UIQuit $win
}


proc DoAbout {win param} {
  #echo DOABOUT $win $param
  MakeHistory "DoLoadCity cities/about.cty"
}


proc DoMap {win param} {
  #echo DOMAP $win $param
}


proc DoLevel {win param} {
  #echo DOLEVEL $win $param
  DoSetGameLevel $param
}


proc DoLeft {win param} {
  #echo DOLEFT $win $param
  PrevHistory
}


proc DoRight {win param} {
  #echo DORIGHT $win $param
 NextHistory
}


proc DoPlay {win param} {
  #echo DOPLAY $win $param
  UIUseThisMap
}


proc DoPickScenario {win param} {
  #echo DOPICKSCENARIO $win $param
  UILoadScenario $param
}


########################################################################
# Undo/Redo Facility


proc InitHistory {} {
  global MapHistory
  global MapHistoryNum
  set MapHistory {}
  set MapHistoryNum -1
}


proc MakeHistory {cmd} {
  global MapHistory
  set len [llength $MapHistory]
  if {($len == 0) ||
      ($cmd != [lindex $MapHistory [expr $len-1]])} {
    lappend MapHistory $cmd
  } else {
    incr len -1
  }
  GotoHistory $len
}


proc GotoHistory {i} {
  global MapHistory
  global MapHistoryNum
  InitVotesForUseThisMap
  if {$i != $MapHistoryNum} {
    set MapHistoryNum $i
    set cmd [lindex $MapHistory $i]
    eval $cmd
  }
  if {$MapHistoryNum == 0} {
    set prev disabled
  } else {
    set prev normal
  }
  if {$MapHistoryNum == ([llength $MapHistory] - 1)} {
    set next disabled
  } else {
    set next normal
  }

  global ScenarioWindows
  foreach win $ScenarioWindows {
    UpdateLeftRightEnabled $win
  }

}


proc NextHistory {} {
  global MapHistory
  global MapHistoryNum
  set len [llength $MapHistory]
  set i [expr "$MapHistoryNum + 1"]
  if {$i < $len} {
    GotoHistory $i
  }
}


proc PrevHistory {} {
  global MapHistory
  global MapHistoryNum
  set i [expr "$MapHistoryNum - 1"]
  if {$i >= 0} {
    GotoHistory $i
  }
}


########################################################################
# Ask Window Handlers

proc ShowAskOf {head} {
  set win [WindowLink $head.ask]
  if {$win == {}} {
    set win [MakeWindow.ask $head [winfo screen $head]]
  }
  wm deiconify $win
  wm raise $win
  return $win
}


proc ShowAsks {} {
  global HeadWindows
  foreach win $HeadWindows {
    ShowAskOf $win
  }
}


proc WithdrawAsks {} {
  global AskWindows
  foreach win $AskWindows {
    wm withdraw $win
  }
}


proc WithdrawAskOf {win} {
  set ask [WindowLink $win.ask]
  if {"$ask" != ""} {
    wm withdraw $ask
  }
}


proc AskQuestion {color title text left middle right} {
  global HeadWindows
  foreach win $HeadWindows {
    AskQuestionOn $win $color $title $text $left $middle $right
  }
}


proc AskQuestionOn {head color title text left middle right} {
  ShowAskOf $head

  set win [WindowLink $head.ask]
  set t [WindowLink $win.title]
  $t configure -text $title
  $t configure -background $color

  set t [WindowLink $win.text]
  $t configure -state normal
  $t delete 0.0 end
  $t insert end "${text}\n"
  $t configure -state disabled

  set bf [WindowLink $win.frame]
  set l [WindowLink $win.left]
  set m [WindowLink $win.middle]
  set r [WindowLink $win.vote]
  set rf [WindowLink $win.voteframe]

  if {$left != ""} {
    $l config \
	-text [lindex $left 0] \
	-command [format [lindex $left 2] $head]
    SetHelp $l [lindex $left 1]
    pack append $bf $l {left frame center}
  } else {
    pack unpack $l
  }

  if {$middle != ""} {
    $m config \
	-text [lindex $middle 0] \
	-command [format [lindex $middle 2] $head]
    SetHelp $m [lindex $middle 1]
    pack append $bf $m {left frame center expand}
  } else {
    pack unpack $m
  }

  if {$right != ""} {
    set notify [format [lindex $right 2] $head]
    set preview [format [lindex $right 3] $head]
    set cmd [list DoVote $win Ask $notify $preview]
    $r config \
	-text [lindex $right 0] \
	-command $cmd
    SetHelp $r [lindex $right 1]
    pack append $bf $rf {right frame center}
  } else {
    pack unpack $rf
  }

  InitVotesForAsk
}


proc BindVotingButton {win but name} {
  set w [WindowLink $win.top]

  bind $but <Any-Enter> "VoteButtonEnter $win $but"
  bind $but <Any-Leave> "VoteButtonLeave $win $but"
  bind $but <1> "VoteButtonDown $win $but $name"
  bind $but <ButtonRelease-1> "VoteButtonUp $win $but $name"
  bind $but <2> "VoteButtonDown $win $but $name"
  bind $but <ButtonRelease-2> "VoteButtonUp $win $but $name"
  bind $but <3> "VoteButtonDown $win $but $name"
  bind $but <ButtonRelease-3> "VoteButtonUp $win $but $name"
}


proc VoteButtonEnter {win but} {
  global tk_priv
  set screen [winfo screen $but]
  if {[lindex [$but config -state] 4] != "disabled"} {
    $but config -state active
    set tk_priv(window@$screen) $but
  } else {
    set tk_priv(window@$screen) ""
  }
}


proc VoteButtonLeave {win but} {
  global tk_priv
  if {[lindex [$but config -state] 4] != "disabled"} {
    $but config -state normal
  }
  set screen [winfo screen $but]
  set tk_priv(window@$screen) ""
}


proc VoteButtonDown {win but name} {
  global tk_priv
  set screen [winfo screen $but]
  set rel [lindex [$but config -relief] 4]
  set tk_priv(relief@$screen) $rel
  if {[lindex [$but config -state] 4] != "disabled"} {
    set head [WindowLink $win.head]
    if {[IsVotingFor $head $name]} {
      $but config -relief raised
    } else {
      $but config -relief sunken
    }
  }
}


proc VoteButtonUp {win but name} {
  global tk_priv
  set screen [winfo screen $but]
  $but config -relief $tk_priv(relief@$screen)
  if {($but == $tk_priv(window@$screen))
	&& ([lindex [$but config -state] 4] != "disabled")} {
    uplevel #0 [list $but invoke]
    set head [WindowLink $win.head]
    if {[IsVotingFor $head $name]} {
      $but config -relief sunken
    } else {
      $but config -relief raised
    }
  }
}


proc PressVoteButton {win but name} {
  global tk_priv
  uplevel #0 [list $but invoke]
  set head [WindowLink $win.head]
  if {[IsVotingFor $head $name]} {
    $but config -relief sunken
  } else {
    $but config -relief raised
  }
}


proc IsVotingFor {win name} {
  global VotesFor$name
  set votes [eval ident "\$\{VotesFor$name\}"]
  if {[lsearch $votes $win] == -1} {
    return 0
  } else {
    return 1
  }
}


proc DoVote {win name notify preview} {
  global VotesFor$name
  set votes [eval ident "\$\{VotesFor$name\}"]

  set win [WindowLink $win.head]
  set i [lsearch $votes $win]
  if {$i == -1} {
    lappend VotesFor$name $win
  } else {
    set VotesFor$name [lreplace $votes $i $i]
  }
  UpdateVotesFor$name
  set votes [eval ident "\$\{VotesFor$name\}"]
  if {[llength $votes] >= [NeededVotes]} {
    eval "$notify"
  } else {
    eval "$preview"
  }
}


proc UpdateVotesFor {name wins} {
  global VotesFor$name
  set votes [eval llength "\$\{VotesFor$name\}"]
  set needed [NeededVotes]

  foreach win $wins {
    set head [WindowLink $win.head]
    if {[IsVotingFor $head $name]} {
      set border [expr "($needed - $votes) * 1"]
      set pad [expr "6 - $border"]
      [WindowLink $win.vote] config -padx $pad -pady $pad
      [WindowLink $win.voteframe] config -borderwidth $border
    } else {
      set border [expr "($needed - $votes - 1) * 1"]
      set pad [expr "6 - $border"]
      [WindowLink $win.vote] config -padx $pad -pady $pad
      [WindowLink $win.voteframe] config -borderwidth $border
    }
  }
}


proc InitVotesForAsk {} {
  global VotesForAsk AskWindows
  set VotesForAsk {}
  foreach win $AskWindows {
    [WindowLink $win.vote] config -relief raised
  }
  sim PendingTool -1
  UpdateVotesForAsk
}


proc UpdateVotesForAsk {} {
  global AskWindows
  UpdateVotesFor Ask $AskWindows
}


########################################################################
# Player Window Handlers

proc ShowPlayerOf {head} {
  set win [WindowLink $head.player]
  if {$win == {}} {
    set win [MakeWindow.player $head [winfo screen $head]]
  }
  wm deiconify $win
  wm raise $win
  return $win
}


proc ShowPlayers {} {
  global HeadWindows
  foreach win $HeadWindows {
    ShowPlayerOf $win
  }
}


proc WithdrawPlayers {} {
  global PlayerWindows
  foreach win $PlayerWindows {
    wm withdraw $win
  }
}


proc UpdatePlayers {} {
  # TODO: Disabled for new scenario window.
  return

  global HeadWindows PlayerWindows

  set players ""
  foreach win $HeadWindows {
    set server [winfo screen $win]
#    if {[string first : $server] == 0} {
#      set server "[exec hostname]:0"
#    }
    lappend players $server
  }

  sim Players [llength $players]

  foreach win $PlayerWindows {
    set list [WindowLink $win.players]
    $list delete 0 end
    eval "$list insert 0 $players"
  }

  UpdateVotesForUseThisMap
  UpdateVotesForAsk
  UpdateVotesForBudget
  UpdateTaxRateSliders
}


proc UIShowPlayer {win} {
  ShowPlayerOf $win
}


proc DoNewPlayer {win} {
  set field [WindowLink $win.display]
  set dpy [$field get]
  if {"$dpy" != ""} {
    $field delete 0 end
    sim Flush
    update idletasks
    if {[AddPlayer $dpy] != ""} {
      wm withdraw $win
    }
  }
}

########################################################################
# Notice Window Handlers


proc ShowNoticeOf {head} {
  set win [WindowLink $head.notice]
  if {$win == {}} {
    set win [MakeWindow.notice $head [winfo screen $head]]
  }
  global NoticePanelWidth NoticePanelHeight
  #place configure $win -x 0 -y 0 -width $NoticePanelWidth -height $NoticePanelHeight
  pack append [WindowLink $head.w3] \
      $win {bottom frame sw expand fill}
  return $win
}


proc ShowNotices {} {
  global HeadWindows
  foreach win $HeadWindows {
    ShowNoticeOf $win
  }
}


proc WithdrawNotices {} {
  global NoticeWindows
  foreach win $NoticeWindows {
    #place forget $win
    pack unpack $win
  }
}


proc ReShowPictureOn {{head ""}} {
  global ShowingPicture ShowingParms
  UIShowPictureOn $head $ShowingPicture $ShowingParms
}


proc UIShowPicture {id {parms ""}} {
  UIShowPictureOn "" $id $parms
}


proc UIShowPictureOn {where id {parms ""}} {
  global DoNotices Messages ShowingPicture ShowingParms
  if {$DoNotices == 0} {
      return
  }

  set ShowingPicture $id
  set ShowingParms $parms
  set msg $Messages($id)
  set color [lindex $msg 0]
  set title [lindex $msg 1]
  set body [lindex $msg 2]
  if {$parms != ""} {
    set cmd "format {$body} $parms"
    set body [uplevel #0 $cmd]
  }
  set props [lindex $msg 3]
  if {"$where" == ""} {
    global HeadWindows
    set where $HeadWindows
  }
  foreach head $where {
    NoticeMessageOn $head "$title" $color "$body" Medium $props
  }
}


proc NoticeMessageOn {head title color text font props} {
  ShowNoticeOf $head
  set win [WindowLink $head.notice]

  set t [WindowLink $win.title]
  $t configure -text $title -background $color

  set t [WindowLink $win.text]
  $t configure -state normal -font [Font $head $font]
  $t delete 0.0 end
  $t insert end "${text}\n"
  $t configure -state disabled

  set left ""
  catch {set left [keylget props left]}
  set l [WindowLink $win.left]
  if {$left != ""} {
    $l config -bitmap $left
    place $l -in $t -anchor sw -relx .05 -rely .95
  } else {
    place forget $l
  }

  set middle ""
  catch {set middle [keylget props middle]}
  set m [WindowLink $win.middle]
  if {$middle != ""} {
    $m config -bitmap $middle
    place $m -in $t -anchor s -relx .5 -rely .95
  } else {
    place forget $m
  }

  set right ""
  catch {set right [keylget props right]}
  set r [WindowLink $win.right]
  if {$right != ""} {
    $r config -bitmap $right
    place $r -in $t -anchor se -relx .95 -rely .95
  } else {
    place forget $r
  }

  set view ""
  catch {set view [keylget props view]}
  set vf [WindowLink $win.viewframe]
  global v
  set v [WindowLink $win.view]
  set bg [WindowLink $win.background]
  if {$view != ""} {
    uplevel #0 "$view"
    pack unpack $t
    pack append $bg $vf {left frame center fill}
    pack append $bg $t {right frame center fill expand}
  } else {
    pack unpack $vf
  }
}


proc UIPopUpMessage {msg} {
  DoSendMessage $msg
}


proc ComeToMe {view} {
  set win [winfo toplevel $view]

  set xy [$view Pan]
  set x [expr "[lindex $xy 0] >>4"]
  set y [expr "[lindex $xy 1] >>4"]

  ComeTo $win $x $y
}


proc ComeTo {win x y} {
  global EditorWindows
  set head [WindowLink $win.head]
  set myeds {}
  set myautoeds {}
  foreach ed $EditorWindows {
    if {"[WindowLink $ed.head]" == "$head"} {
      lappend myeds $ed
      set view [WindowLink $ed.view]
      if {[$view AutoGoto]} {
        lappend myautoeds $ed
      }
    }
  }
  if {[llength $myautoeds]} {
    UIAutoGotoOn $x $y $myautoeds
  } else {
    if {[llength $myeds]} {
      UIAutoGotoOn $x $y $myeds
    }
  }
}


proc FollowView {view id} {
  $view Follow $id

  set skips 999999
  if {[sim DoAnimation]} {
    set head [WindowLink [winfo toplevel $view].head]
    global EditorWindows
    foreach win $EditorWindows {
      if {"[WindowLink $win.head]" == "$head"} {
	set s [[WindowLink $win.view] Skip]
	set skips [min $skips $s]
      }
    }

    if {$skips == 999999} {
      set skips 0
    }
  }

  $view Skip $skips
  $view Update
}


proc PanView {view x y} {
  FollowView $view ""
  $view Pan $x $y
}


########################################################################
# Help Window Handlers


proc ShowHelpOf {head} {
  set win [WindowLink $head.help]
  if {$win == {}} {
    set win [MakeWindow.help $head [winfo screen $head]]
  }
  wm deiconify $win
  wm raise $win
  return $win
}


proc ShowHelps {} {
  global HeadWindows
  foreach win $HeadWindows {
    ShowHelpOf $win
  }
}


proc WithdrawHelps {} {
  global HelpWindows
  foreach win $HelpWindows {
    wm withdraw $win
  }
}


########################################################################
# Frob Window Handlers


proc ShowFrobOf {head} {
  set win [WindowLink $head.frob]
  if {$win == {}} {
    set win [MakeWindow.frob $head [winfo screen $head]]
  }
  wm deiconify $win
  wm raise $win

  return $win
}


proc ShowFrobs {} {
  global HeadWindows
  foreach win $HeadWindows {
    ShowFrobOf $win
  }
}


proc WithdrawFrobs {} {
  global FrobWindows
  foreach win $FrobWindows {
    wm withdraw $win
  }
}


proc UISetDynamic {i min max} {
  sim DynamicData [expr "$i*2"] $min
  sim DynamicData [expr "$i*2+1"] $max
}


########################################################################
# Help Handler


proc LoadHelp {} {
  global ResourceDir

  source $ResourceDir/help.tcl
}

proc HandleHelp {win x y rootx rooty} {
  global HelpLoaded HelpWidgets Messages

  if {$HelpLoaded == 0} {
    LoadHelp
    set HelpLoaded 1
  }

  set orig $win
  set head [WindowLink [winfo toplevel $win].head]
  set id ""
  while {1} {
    catch {set id $HelpWidgets($win)}
    if {$id != ""} {
      break
    }
    set list [split $win .]
    set len [expr "[llength $list] - 2"]
    set list [lrange $list 0 $len]
    if {[llength $list] <= 1} {
      set id Window
      break
    }
    set win [join $list .]
  }
  if [info exists Messages($id)] {
    UIShowHelpOn $head $id
  } else {
    UIShowHelpOn $head Window
  }
}


proc UIShowHelpOn {win id {parms ""}} {
  global Messages ShowingPicture ShowingParms ResourceDir
  set head [WindowLink $win.head]
  set msg $Messages($id)
  set color [lindex $msg 0]
  set title [lindex $msg 1]
  set body [lindex $msg 2]
  if {$parms != ""} {
    set cmd "format {$body} $parms"
    set body [uplevel #0 $cmd]
  }
  set props [lindex $msg 3]

  ShowHelpOf $head
  set win [WindowLink $head.help]

  set t [WindowLink $win.title]
  $t configure -text $title -background $color

  set t [WindowLink $win.text]
  $t configure -state normal -font [Font $head Large]
  $t delete 0.0 end
  $t insert end "${body}\n"
  FormatHTML $ResourceDir/doc/$id.html $t
  $t configure -state disabled
}


proc FormatHTML {filename text} {
  set f ""
  catch {
    set f [open $filename]
    set line ""
    set header ""
    set title ""
    gets $f header
    gets $f title

    while {[gets $f line] >= 0} {
      if {"$line" == "</body>"} {
	break
      }
      if {([string index $line 0] == "#") || ([string length $line] == 0)} {
	continue
      }
      $text insert end "$line\n"
    }
  }
  if {$f != ""} {
    close $f
  }
}


proc SetHelp {win id} {
  global HelpWidgets
  set HelpWidgets($win) $id
}


proc Help {id title {msg {}} {props {}}} {
  Message $id #7f7fff $title $msg $props
}

########################################################################
# Pie Menu Handlers


# Set up the bindings to pop up $pie when the right button is clicked in $win
proc InitPie {win pie} {
  bind $win <Motion> {}
  bind $win <3> "PieMenuDown $win $pie $pie Initial %X %Y"
  bind $win <B3-Motion> {}
  bind $win <B3-ButtonRelease> {}
}


# Set up the bindings to continue tracking $pie
# Get this: we keep the tracking machine state in the bindings!
proc ActivatePie {win root pie state} {
  bind $win <Motion> "PieMenuMotion $win $root $pie $state %X %Y"
  bind $win <3> "PieMenuDown $win $root $pie $state %X %Y"
  bind $win <B3-Motion> "PieMenuMotion $win $root $pie $state %X %Y"
  bind $win <B3-ButtonRelease> "PieMenuUp $win $root $pie $state %X %Y"
}


# Cancel and reset a pie menu
proc CancelPie {win} {
  set binding [bind $win <3>]
  set root [lindex $binding 2]
  set pie [lindex $binding 3]
  set state [lindex $binding 4]
  if {"$state" != "Initial"} {
    catch {$root ungrab $win}
    $pie unpost
    $pie activate none
    UIMakeSoundOn $win fancy Oop
  }
  InitPie $win $root
}


# Handle pie menu button down
proc PieMenuDown {win root pie state x y} {
  case $state {
    Initial {
      ActivatePie $win $root $pie FirstDown
      update idletasks
      catch {$root grab $win}
      $pie activate none
      $pie post $x $y
      PreviewPieMenu $win $pie $x $y
      update idletasks
    }
    ClickedUp {
      TrackPieMenu $pie $x $y
      ActivatePie $win $root $pie SecondDown
    }
    SelectedUp {
      $pie activate none
      #$pie post $x $y
      $pie defer
      PreviewPieMenu $win $pie $x $y
      ActivatePie $win $root $pie SecondDown
    }
    FirstDown { # error
      CancelPie $win
    }
    SecondDown { # error
      CancelPie $win
    }
  }
}


# Handle pie menu button motion
proc PieMenuMotion {win root pie state x y} {
  case $state {
    FirstDown {
      TrackPieMenu $pie $x $y
      $pie defer
    }
    ClickedUp {
      $pie activate none
      #$pie post $x $y
    }
    SecondDown {
      TrackPieMenu $pie $x $y
      $pie defer
    }
    SelectedUp {
      $pie activate none
      #$pie post $x $y
    }
    Initial { # error
      CancelPie $win
    }
  }
}


# Handle pie menu button up
proc PieMenuUp {win root pie state x y} {
  case $state {
    FirstDown {
      TrackPieMenu $pie $x $y
      set active [$pie index active]
      if {$active == "none"} {
	$pie show
        catch {$root grab $win}
        ActivatePie $win $root $pie ClickedUp
      } else {
        set label [lindex [$pie entryconfig $active -label] 4]
        set submenu [lindex [$pie entryconfig $active -piemenu] 4]
	UIMakeSoundOn $win mode $label
	if {$submenu == {}} {
	  set reward [$pie pending]
	  catch {$root ungrab $win}
	  $pie unpost
          $pie activate none
          if {$reward} {
	    sim Funds [expr "[sim Funds] + 5"]
	    UIMakeSoundOn $win fancy Aaah
	  }
	  eval [lindex [$pie entryconfig $active -command] 4]
          InitPie $win $root	
	} else {
	  $pie unpost
          $pie activate none
          $submenu activate none
          $submenu post $x $y
	  PreviewPieMenu $win $submenu $x $y
          catch {$root grab $win}
	  ActivatePie $win $root $submenu SelectedUp
	}
      }
    }
    SecondDown {
      TrackPieMenu $pie $x $y
      set active [$pie index active]
      if {$active == "none"} {
	CancelPie $win
      } else {
        set label [lindex [$pie entryconfig $active -label] 4]
        set submenu [lindex [$pie entryconfig $active -piemenu] 4]
	UIMakeSoundOn $win mode $label
	if {$submenu == {}} {
	  set reward [$pie pending]
	  catch {$root ungrab $win}
	  $pie unpost
          $pie activate none
          if {$reward} {
	    sim Funds [expr "[sim Funds] + 5"]
	    UIMakeSoundOn $win fancy Aaah
	  }
	  eval [lindex [$pie entryconfig $active -command] 4]
	  InitPie $win $root
	} else {
	  $pie unpost
          $pie activate none
          $submenu activate none
          $submenu post $x $y
	  PreviewPieMenu $win $submenu $x $y
          catch {$root grab $win}
	  ActivatePie $win $root $submenu SelectedUp
	}
      }
    }
    Initial { # error
      CancelPie $win
    }
    ClickedUp { # error
      CancelPie $win
    }
    SelectedUp { # error
      CancelPie $win
    }
  }
}


# Track the selected item
proc TrackPieMenu {pie x y} {
  $pie activate @$x,$y
}


proc PreviewPieMenu {win pie x y} {
  set preview [lindex [$pie config -preview] 4]
  if {"$preview" != ""} {
    set rx [winfo rootx $win]
    set ry [winfo rooty $win]
    set x [expr "$x - $rx"]
    set y [expr "$y - $ry"]
    eval [format $preview $x $y]
  }
}


########################################################################
# Pallet Handlers


proc ExclusivePallet {state parent children prefix images inactive active cmd} {
  #echo "ExclusivePallet state $state parent $parent children $children prefix $prefix images $images inactive $inactive active $active cmd $cmd"
  set i 0
  foreach child $children {
    set name [lindex $images $i]
    if {$i == $state} then {
      $parent.$child config \
	  -bitmap "@images/${prefix}${name}hi.xpm" \
	  -relief $active
    } else {
      $parent.$child config \
	  -bitmap "@images/${prefix}${name}.xpm" \
	  -relief $inactive
    }
    incr i
  }
  eval [concat $cmd $state]
}


proc NonExclusivePallet {mask parent children prefix images
			 inactive active cmd} {
  set i 0
  foreach child $children {
    set name [lindex $images $i]
    if {$mask & (1<<$i)} then {
      $parent.$child config \
	  -bitmap "@images/${prefix}${name}hi.xpm" \
	  -relief $active
    } else {
      $parent.$child config \
	  -bitmap "@images/${prefix}${name}.xpm" \
	  -relief $inactive
    }
    incr i
  }
  eval [concat $cmd $mask]
}


proc EditorPallet {win state} {
  global EditorPalletSounds
  UIMakeSoundOn $win mode [lindex $EditorPalletSounds $state]
  EditorSetTool $win $state
}


proc EditorSetTool {win state} {
  global EditorPallets
  global EditorPalletImages
  ExclusivePallet $state $win $EditorPallets ic $EditorPalletImages \
	flat raised "$win.centerframe.view ToolState"
}


proc GraphPallet {win state} {
  set mask [[WindowLink $win.graphview] Mask]
  set mask [expr "$mask ^ (1<<$state)"]
  GraphPalletMask $win $mask
}


proc GraphPalletMask {win mask} {
  global GraphPallets
  global GraphPalletImages
  NonExclusivePallet $mask $win $GraphPallets gr $GraphPalletImages \
	flat flat "SetGraphState $win"
}


proc GraphYearPallet {win state} {
  global GraphYearPallets
  global GraphYearPalletImages
  ExclusivePallet $state $win $GraphYearPallets gr $GraphYearPalletImages \
	flat flat "SetGraphYearState $win"
}


proc SetGraphYearState {win state} {
  set graph [WindowLink $win.graphview]
  if {$state == 0} {
    $graph Range 10
  } else {
    $graph Range 120
  }
}


proc SetGraphState {win mask} {
  global GraphPallets
  set graph [WindowLink $win.graphview]
  $graph Mask $mask
}


########################################################################
# Button Handlers

proc sim_butEnter {w} {
  global tk_priv
  set screen [winfo screen $w]
  set tk_priv(window@$screen) $w
}


proc sim_butLeave {w} {
  global tk_priv
  set screen [winfo screen $w]
  set tk_priv(window@$screen) ""
}


proc sim_butDown {w} {
  global tk_priv
  set screen [winfo screen $w]
  set pict [lindex [$w config -bitmap] 4]
  set tk_priv(relief@$screen) $pict
  $w config -bitmap [lindex [split $pict .] 0]hi.xpm
  update idletasks
}


proc sim_butUp {w} {
  global tk_priv
  set screen [winfo screen $w]
  $w config -bitmap $tk_priv(relief@$screen)
  update idletasks
  if {$w == $tk_priv(window@$screen)} {
    uplevel #0 [list $w invoke]
  }
}


proc BindSimButton {w} {
  bind $w <Any-Enter> {sim_butEnter %W}
  bind $w <Any-Leave> {sim_butLeave %W}
  bind $w <1> {sim_butDown %W}
  bind $w <ButtonRelease-1> {sim_butUp %W}
  bind $w <2> {sim_butDown %W}
  bind $w <ButtonRelease-2> {sim_butUp %W}
  bind $w <3> {sim_butDown %W}
  bind $w <ButtonRelease-3> {sim_butUp %W}
}


########################################################################
# Internal Callbacks


proc UIStartMicropolis {homedir resourcedir hostname} {
  global HomeDir ResourceDir HostName HeadWindows
  set HomeDir $homedir
  set ResourceDir $resourcedir
  set HostName $hostname
  sim InitGame
  sim GameStarted
  update

  foreach display [sim Displays] {
    if {"[AddPlayer $display]" == ""} {
      echo Couldn't add a player on $display ...
    }
  }

  if {"$HeadWindows" == ""} {
    echo Micropolis is exiting because it couldn't connect to any players.
    sim ReallyQuit
  }
}


proc UISelectCity {win} {
  AskQuestion [Color $win #ff0000 #ffffff] "Choose Another City" \
    "Do you want to abandon this city and choose another one?" \
    "{Keep playing.} SelectCity.No {RejectPlan}" \
    "" \
    "{Another city!} SelectCity.Yes {UIPickScenarioMode}"
}


proc UIQuit {head} {
  if {[sim Players] == 1} {
    set l "{Keep playing.} Quit.No {RejectPlan}"
    set m ""
    set r "{I quit!} Quit.IQuit {DoReallyQuit %s}"
  } else {
    set l "{Keep playing.} Quit.No {RejectPlan}"
    set m "{I quit!} Quit.IResign {DoIResign %s}"
    set r "{Everyone quit!} Quit.AllQuit {DoReallyQuit %s}"
  }
  AskQuestion [Color $head #ff0000 #ffffff] "Quit Playing Micropolis" \
    "Do you want to quit playing Micropolis?" \
    $l $m $r
}


proc DoIResign {head} {
  global VotesForAsk
  set display [winfo screen $head]
  CrushHead $head
  DecRefDisplay $display
  UISetMessage "The player on X11 Display $display has resigned."
  UpdatePlayers
  if {([sim Players] == 0) ||
      ([llength $VotesForAsk] >= [sim Players])} {
    sim ReallyQuit
  }
}


proc DoReallyQuit {head} {
  echo QuitMicropolis
  sim ReallyQuit
}


proc UISplashMode {} {
  global State
  set State splash
  sim Pause
  WithdrawAll
  InitSplashes
  ShowSplashes
}


proc UIPickScenarioMode {} {
  global State
  global CityLibDir
  set State scenario
  sim Pause
  WithdrawAll
  InitHistory
  UIGenerateCityNow
  InitScenarios
  InitVotesForUseThisMap
  ShowScenarios
}


proc ForcePickScenarioMode {} {
  global State
  if {"$State" != "scenario"} {
    UIPickScenarioMode
  }
}


proc UIGenerateCityNow {} {
  global CityName GameLevel
  sim CityName NowHere
  sim GameLevel 0
  UIGenerateNewCity
}


proc UIGenerateNewCity {} {
  global CityName GameLevel
  if {$GameLevel == -1} {
    set GameLevel 0
  }
  MakeHistory "DoNewCity NowHere $GameLevel [sim Rand] [sim TreeLevel] [sim LakeLevel] [sim CurveLevel] [sim CreateIsland]"
}


proc DoNewCity {name level {r ""} {tl -1} {ll -1} {cl -1} {ci -1}} {
  global Scenario
  set Scenario -1
  sim TreeLevel $tl
  sim LakeLevel $ll
  sim CurveLevel $cl
  sim CreateIsland $ci
  if {"$r" == ""} {
    sim GenerateNewCity
  } else {
    sim GenerateSomeCity $r
  }
  sim CityName $name
  sim GameLevel $level
  UIShowPicture 48
}


proc UIDidGenerateNewCity {} {
  sim Update
}


proc IncRefDisplay {display} {
  global DisplayRegistry
  if ![info exists DisplayRegistry($display)] {
    set DisplayRegistry($display) 0
  }
  incr DisplayRegistry($display)
}


proc DecRefDisplay {display} {
  global DisplayRegistry
  incr DisplayRegistry($display) -1
  if {$DisplayRegistry($display) <= 0} {
    CloseDisplay $display
  }
}


proc CloseDisplay {display} {
}


proc DoStopMicropolis {} {
  KillSoundServers
  destroy .
}


proc AddPlayer {display} {
  set i [string first : $display]
  if {$i == 0} {
  } else {
    if {$i == -1} {
      set display "$display:0"
    }
  }

  echo Adding a player on $display ...

  set head [MakeWindow.head $display]

  if {"$head" != ""} {
    set display [winfo screen $head]
    IncRefDisplay $display
    PrepHead $head
    UISetMessage "Added a player on X11 Display \"$display\"."
    UpdatePlayers
  } else {
    UISetMessage "Couldn't add a player on X11 Display \"$display\"!"
  }
  return $head
}


proc FireBomb {} {
  sim FireBomb
}

proc DropFireBombs {} {
  FireBomb
  after 300 FireBomb
  after 600 FireBomb
  after 900 FireBomb
}


proc UIMakeMonster {} {
  sim MakeMonster [sim Rand 120] [sim Rand 100]
}


proc melt {} {
  sim HeatSteps 1
  sim HeatFlow -7
  sim HeatRule 0
}


proc eco {} {
  sim HeatSteps 1
  sim HeatFlow 19
  sim HeatRule 1
}


proc oops {} {
  sim HeatSteps 0
}


proc TogglePause {} {
  global State

  if {"$State" != "play" || [sim Speed]} {
    sim Speed 0
  } else {
    sim Speed 3
  }
  MakeRunningSound
}


proc SetSpeedTime {time} {
  sim Speed $time
  MakeRunningSound
}


proc MakeRunningSound {} {
  global State

  if {"$State" == "play" && [sim Speed]} {
    UIMakeSound edit Boing "-speed 1[sim Speed]0"
  } else {
    UIMakeSound edit Boing "-speed 90"
  }
}


proc SayDemands {} {
  global DemandRes DemandCom DemandInd
  set slope 3
  set r [expr "100 + ($DemandRes * $slope)"]
  set c [expr "100 + ($DemandCom * $slope)"]
  set i [expr "100 + ($DemandInd * $slope)"]
  after 020 "UIMakeSound edit O \"-speed $r\""
  after 220 "UIMakeSound edit A \"-speed $c\""
  after 440 "UIMakeSound edit E \"-speed $i\""
}


proc UISaveCity {win} {
  global SaveCityWin
  set SaveCityWin $win
  sim SaveCity
}


proc UISaveCityAs {{win ""}} {
  global SaveCityWin
  if {"$win" == ""} {set win $SaveCityWin}
  set SaveCityWin $win

  global CityDir
  set file [ShowFileOf $win]
  DoFileDialog $file "Choose a File to Save the City" $CityDir "*.cty" "" \
	"UIDoReallySaveCity" ""
}


proc UIDoReallySaveCity {name path} {
  global CityDir
  if {![string match *.cty $name]} {
    set name $name.cty
  }
  set CityDir $path
  sim SaveCityAs $path/$name
}


proc UIDidSaveCity {} {
  DoSendMessage "Saved the city in \"[sim CityFileName]\"." status
}


proc UIDidntSaveCity {msg} {
  DoSendMessage $msg alert
  UIMakeSound warning Sorry "-speed 85"
}


proc UILoadScenario {scen} {
  MakeHistory "DoScenario $scen"
}


proc DoScenario {scen} {
  global Scenario
  set Scenario $scen
  sim LoadScenario $scen
  UIShowPicture $scen
}


proc UIDidLoadScenario {} {
}


proc UIStartScenario {id} {
  global Scenario
  set Scenario $id
  UILoadScenario $id
  UIPlayGame
  UIShowPicture $id
}


proc UIPlayNewCity {} {
  UIGenerateNewCity
  UIPlayGame
}


proc UIStartLoad {} {
  UIPlayGame
}


proc UIReallyStartGame {} {
  #UIPickScenarioMode
  UISplashMode
}


proc UIPlayGame {} {
  global State
  global Priority
  set State play
  sim Resume
  sim Speed 3
  sim AutoGoto 1
  SetPriority $Priority
  InitHeads
  InitAllHeadMenus
  ShowInitial
  sim NeedRest 10
}


proc UISetSpeed {speed} {
  global Time State
  set Time $speed
  if {"$State" == "play"} {
    UISetMessage [lindex {
      {Time pauses.}
      {Time flows slow.}
      {Time flows medium.}
      {Time flows fast.}
    } $speed]
  }
  UIUpdateRunning
}


proc UIUpdateRunning {} {
  global HeadWindows Time State

	if {($State == "play") && $Time} {
    set pict "@images/micropolisg.xpm"
  } else {
    set pict "@images/micropoliss.xpm"
  }

  foreach win $HeadWindows {
    set can [WindowLink $win.demand]
    $can itemconfig micropolis -bitmap $pict
  }
}


proc DoSetGameLevel {level} {
  sim GameLevel $level
}


proc UISetGameLevel {level} {
  global GameLevel
  set GameLevel $level
  global ScenarioWindows
  foreach win $ScenarioWindows {
    UpdateLevelSelection $win
  }
}


proc UISetCityName {name} {
  global EditorWindows
  global CityName
  set CityName $name
  set title "Micropolis Editor on $name"
  foreach win $EditorWindows {
    # TODO: set editor window and icon title
    #wm title $win "$title"
    #wm iconname $win "$title"
  }
}


proc UILoadCity {win} {
  # if changed, r-u-sure?
  global CityDir
  set file [ShowFileOf $win]
  DoFileDialog $file "Choose a City to Load" $CityDir "*.cty" "" \
	"UIDoLoadCity" ""
}


proc UIDoLoadCity {name path} {
  global CityDir
  set CityDir $path
  if {![string match *.cty $name]} {
    set name $name.cty
  }
  MakeHistory "DoLoadCity $path/$name"
}


proc DoLoadCity {filename} {
  sim LoadCity $filename
}

proc UIDidLoadCity {} {
  global State GameLevel Scenario
  set Scenario -1
  set GameLevel -1
  if {$State == "play"} {
    UIPlayGame
  } else {
    UIShowPicture 49 [sim CityFileName]
  }
}


proc UIDidntLoadCity {msg} {
  DoSendMessage $msg alert
  UIMakeSound warning Sorry "-speed 85"
  UIShowPicture 49 [sim CityFileName]
  sim Fill 0
  sim UpdateMaps
}


proc UINewGame {} {
  global OldBudget
  set OldBudget 0
  sim InitGame
  sim EraseOverlay
  InitEditors
  InitMaps
  InitGraphs
  update
  sim UpdateMaps
}


proc UIDidPan {w x y} {
  if {[$w ToolMode] == 1} {
    EditorToolDrag $w $x $y
  }
  update idletasks
}


proc UIDidStopPan {win} {
  UIMakeSoundOn $win fancy Skid "-volume 25"
  $win TweakCursor
}


proc UIEarthQuake {} {
}


proc UIAutoGoto {x y {except {}}} {
  global EditorWindows
  set didOne 0
  set x [expr "$x * 16 + 8"]
  set y [expr "$y * 16 + 8"]
  foreach win $EditorWindows {
    if {"$win" != "$except"} {
      set view [WindowLink $win.view]
      if {[$view AutoGoto]} {
        set didOne 1
        $view AutoGoal $x $y
      }
    }
  }
  if {$didOne} {
    sim UpdateMaps
  }
}


proc UIAutoGotoOn {x y eds} {
  set x [expr "$x * 16 + 8"]
  set y [expr "$y * 16 + 8"]
  foreach win $eds {
    [WindowLink $win.view] AutoGoal $x $y
  }
  sim UpdateMaps
}


proc DoLeaveGame {head} {
}


proc UILoseGame {} {
  UIPickScenarioMode
  UIShowPicture 200
}


proc UIWinGame {} {
  UIShowPicture 100
}


proc DoPendTool {view tool x y} {
  global HeadWindows ToolInfo VotesForAsk

  set win [WindowLink $view.top]
  set head [WindowLink $win.head]

  if {($tool == [sim PendingTool]) &&
      ($x == [sim PendingX]) &&
      ($y == [sim PendingY])} {
    if {[lsearch $VotesForAsk $head] != -1} {
      # you can only vote once
      UIMakeSound edit Oop
    } else {
      UIMakeSound edit Boing
      set ask [WindowLink $head.ask]
      PressVoteButton $ask [WindowLink $ask.vote] Ask
    }
  } else {
    UIAutoGoto $x $y $win

    UIMakeSound edit Boing
    set info [lindex $ToolInfo $tool]
    set a [lindex $info 0]
    set name [lindex $info 1]
    set cost [lindex $info 2]
    set title "Build $a $name"
    set question "Do you support the plan to build $a $name for $cost?"
    AskQuestion [Color $win #00ff00 #ffffff] "$title" \
	"$question" \
	"{Veto plan.} Plan.Veto
	   {RejectPlan}" \
	"{Goto plan.} Plan.Goto
	   {ComeTo %s $x $y}" \
	"{Support plan!} Plan.Support
	   {SupportPlan $view %s $tool $x $y}
	   {PreviewSupportPlan $view %s $tool $x $y}"
    set VotesForAsk $head
    set ask [WindowLink $head.ask]
    [WindowLink $ask.vote] config -relief sunken

    UpdateVotesForAsk

    sim PendingTool $tool
    sim PendingX $x
    sim PendingY $y
    sim Votes [llength $VotesForAsk]
  }
}


proc RejectPlan {} {
  sim PendingTool -1
  if {[sim Players] > 1} {
    UIMakeSound edit Sorry
  }
  WithdrawAsks
}


proc NeededVotes {} {
  global FreeVotes
  set players [sim Players]
  set needed [expr "$players - $FreeVotes"]
  return [max 0 $needed]
}


proc SupportPlan {view h tool x y} {
  global VotesForAsk
  sim Votes [llength $VotesForAsk]
  sim PendingTool -1
  sim OverRide 1
  $view DoTool $tool $x $y
  sim OverRide 0
  WithdrawAsks
  UIMakeSound edit Aaah
}


proc PreviewSupportPlan {view h tool x y} {
  global VotesForAsk
  sim Votes [llength $VotesForAsk]
}


proc SugarStartUp {uri} {
  global SugarURI
  set SugarURI $uri

  # TODO: Load the city file at the given URI, or configure according to URI parameters. 
}


proc SugarNickName {nick} {
  global SugarNickName
  set SugarNickName $nick
}


proc SugarActivate {} {
  global SugarActivated
  set SugarActivated 1
}


proc SugarDeactivate {} {
  global SugarActivated
  set SugarActivated 0
}


proc SugarShare {} {
  global SugarShared
  set SugarShared 1
}


proc SugarBuddyAdd {key nick color address} {
  global SugarBuddies
  set buddy [list $key $nick $color $address]
  lappend SugarBuddies $buddy
}


proc SugarBuddyDel {key nick color address} {
  global SugarBuddies
  set buddy [list $key $nick $color $address]
  set i [lsearch $wins $win]
  if {$i != -1} {
    set SugarBuddies [lreplace $wins $i $i]
  }
}


proc SugarQuit {} {
  sim ReallyQuit
}


########################################################################
