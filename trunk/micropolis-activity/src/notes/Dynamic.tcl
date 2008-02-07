option add *Interval.activeForeground	#d0d0d0
option add *Interval.sliderForeground	#b0b0b0
option add *Interval.activeForeground	#d0d0d0
option add *Interval.sliderForeground	#b0b0b0

########################################################################
# Dynamic Map Handlers

proc ShowDynamicPanel {win} {
  if {! [winfo ismapped $win.bottomframe]} {
    pack append $win $win.bottomframe	{bottom frame center fill expand}
    set x [winfo rootx $win]
    set y [winfo rooty $win]
    set width [winfo width $win]
    set height 670
    wm geometry $win ${width}x${height}
  }
}


proc HideDynamicPanel {win} {
  if {[winfo ismapped $win.bottomframe]} {
    pack unpack $win.bottomframe
    set x [winfo rootx $win]
    set y [winfo rooty $win]
    set width [winfo width $win]
    set height 332
    wm geometry $win ${width}x${height}
  }
}


proc InitDynamic {} {
  global MapWindows
  sim ResetDynamic

  for {set i 0} {$i < 8} {incr i} {
    foreach win $MapWindows {
      [WindowLink $win.interval]$i reset
    }
  }

}


proc SetDynamicData {win index min max} {
  set MinIndex [expr "$index * 2"]
  set MaxIndex [expr "$MinIndex + 1"]

  sim DynamicData $MinIndex $min
  sim DynamicData $MaxIndex $max

  set int [WindowLink $win.interval]$index
  set from [lindex [$int config -from] 4]
  set to [lindex [$int config -to] 4]
  set min [lindex [$int config -min] 4]
  set max [lindex [$int config -max] 4]
  set range [expr "${to}-${from}"]
  set offset 80
  set scale 100
  set low [expr "(($min-$from)*$scale/$range) + $offset"]
  set high [expr "(($max-$from)*$scale/$range) + $offset"]
  set sound 29
  set title Slider$index
  set vol 70
  set chan fancy

  UIDoSound "\"sound replay $title -channel $chan -volume $vol ; sound replay $sound -channel $chan -volume $vol -speed $low ; sound play $sound -channel $chan -volume $vol -speed $high\""

  global MapWindows
  foreach win $MapWindows {
    [WindowLink $win.interval]$index set $min $max
  }
}





global MapTitles
set MapTitles {
  {Micropolis Map}
  {Residential Zone Map}
  {Commercial Zone Map}
  {Industrial Zone Map}
  {Power Grid Map}
  {Transportation Map}
  {Population Density Map}
  {Rate of Growth Map}
  {Traffic Map}
  {Pollution Map}
  {Crime Map}
  {Land Value Map}
  {Fire Station Map}
  {Police Station Map}
  {Dynamic Query Map}
}




proc UISetMapState {w state} {
  global MapTitles
  set win [winfo toplevel $w]
  set m [WindowLink $win.view]
  set title [lindex $MapTitles $state]
  wm title $win "$title"
  wm iconname $win "$title"
  global [set var MapState.$win]
  set $var $state

  case $state { \
    {6 8 9 10 11 12 13} {
      [WindowLink $win.legend] config -bitmap "@Images/LegendMaxMin.xpm"
      HideDynamicPanel $win
    } \
    {7} {
      [WindowLink $win.legend] config -bitmap "@Images/LegendPlusMinus.xpm"
      HideDynamicPanel $win
    } \
    {0 1 2 3 4 5 14} {
      [WindowLink $win.legend] config -bitmap "@Images/LegendNone.xpm"
      if {$state == 14} {
	ShowDynamicPanel $win
      } else {
        HideDynamicPanel $win
      }
    }
  }
}


