# mkPuzzle w
#
# Create a top-level window containing a 15-puzzle game.
#
# Arguments:
#    w -	Name to use for new top-level window.

proc mkPuzzle {{w .p1}} {
    catch {destroy $w}
    toplevel $w
    dpos $w
    wm title $w "15-Puzzle Demonstration"
    wm iconname $w "15-Puzzle"
    message $w.msg -font -Adobe-times-medium-r-normal--*-180* -aspect 300 \
	    -text "A 15-puzzle appears below as a collection of buttons.  Click on any of the pieces next to the space, and that piece will slide over the space.  Continue this until the pieces are arranged in numerical order from upper-left to lower-right.  Click the \"OK\" button when you've finished playing."
    set order {3 1 6 2 5 7 15 13 4 11 8 9 14 10 12}
    global xpos ypos
    frame $w.frame -geometry 120x120 -borderwidth 2 -relief sunken \
	-bg Bisque3

    for {set i 0} {$i < 15} {set i [expr $i+1]} {
	set num [lindex $order $i]
	set xpos($num) [expr ($i%4)*.25]
	set ypos($num) [expr ($i/4)*.25]
	button $w.frame.$num -relief raised -text $num \
		-command "puzzle.switch $w $num"
	place $w.frame.$num -relx $xpos($num) -rely $ypos($num) \
	    -relwidth .25 -relheight .25
    }
    set xpos(space) .75
    set ypos(space) .75

    button $w.ok -text OK -command "destroy $w"

    pack append $w $w.msg {top fill} $w.frame {top expand padx 10 pady 10} \
	    $w.ok {bottom fill}
}


# Procedure invoked by buttons in the puzzle to resize the puzzle entries:

proc puzzle.switch {w num} {
    global xpos ypos
    if {(($ypos($num) >= ($ypos(space) - .01))
	    && ($ypos($num) <= ($ypos(space) + .01))
	    && ($xpos($num) >= ($xpos(space) - .26))
	    && ($xpos($num) <= ($xpos(space) + .26)))
	    || (($xpos($num) >= ($xpos(space) - .01))
	    && ($xpos($num) <= ($xpos(space) + .01))
	    && ($ypos($num) >= ($ypos(space) - .26))
	    && ($ypos($num) <= ($ypos(space) + .26)))} {
	set tmp $xpos(space)
	set xpos(space) $xpos($num)
	set xpos($num) $tmp
	set tmp $ypos(space)
	set ypos(space) $ypos($num)
	set ypos($num) $tmp
	place $w.frame.$num -relx $xpos($num) -rely $ypos($num)
    }
}
