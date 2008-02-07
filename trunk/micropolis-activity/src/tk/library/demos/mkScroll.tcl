# mkScroll w
#
# Create a top-level window containing a simple canvas that can
# be scrolled in two dimensions.
#
# Arguments:
#    w -	Name to use for new top-level window.

proc mkScroll {{w .cscroll}} {
    catch {destroy $w}
    toplevel $w
    dpos $w
    wm title $w "Scrollable Canvas Demonstration"
    wm iconname $w "Canvas"
    wm minsize $w 100 100
    set c $w.frame2.c

    frame $w.frame1 -relief raised -bd 2
    frame $w.frame2 -relief raised -bd 2
    button $w.ok -text "OK" -command "destroy $w"
    pack append $w $w.frame1 {top fill} $w.ok {bottom pady 10 frame center} \
	    $w.frame2 {top expand fill}
    message $w.frame1.m -font -Adobe-Times-Medium-R-Normal-*-180-* -aspect 300 \
	    -text "This window displays a canvas widget that can be scrolled either using the scrollbars or by dragging with button 2 in the canvas.  If you click button 1 on one of the rectangles, its indices will be printed on stdout."
    pack append $w.frame1 $w.frame1.m {frame center}

    canvas $c -scrollregion {-10c -10c 50c 20c}
    scrollbar $w.frame2.vscroll  -relief sunken -command "$c yview"
    scrollbar $w.frame2.hscroll -orient horiz -relief sunken -command "$c xview"
    pack append $w.frame2 $w.frame2.hscroll {bottom fillx} \
	    $w.frame2.vscroll {right filly} $c {expand fill}
    $c config -xscroll "$w.frame2.hscroll set" -yscroll "$w.frame2.vscroll set"

    set bg [lindex [$c config -bg] 4]
    for {set i 0} {$i < 20} {incr i} {
	set x [expr {-10 + 3*$i}]
	for {set j 0; set y -10} {$j < 10} {incr j; incr y 3} {
	    $c create rect ${x}c ${y}c [expr $x+2]c [expr $y+2]c \
		    -outline black -fill $bg -tags rect
	    $c create text [expr $x+1]c [expr $y+1]c -text "$i,$j" \
		-anchor center -tags text
	}
    }

    $c bind all <Any-Enter> "scrollEnter $c"
    $c bind all <Any-Leave> "scrollLeave $c"
    $c bind all <1> "scrollButton $c"
    bind $c <2> "$c scan mark %x %y"
    bind $c <B2-Motion> "$c scan dragto %x %y"
}

proc scrollEnter canvas {
    global oldFill
    set id [$canvas find withtag current]
    if {[lsearch [$canvas gettags current] text] >= 0} {
	set id [expr $id-1]
    }
    set oldFill [lindex [$canvas itemconfig $id -fill] 4]
    if {[winfo screendepth $canvas] > 4} {
	$canvas itemconfigure $id -fill SeaGreen1
    } else {
	$canvas itemconfigure $id -fill black
	$canvas itemconfigure [expr $id+1] -fill white
    }
}

proc scrollLeave canvas {
    global oldFill
    set id [$canvas find withtag current]
    if {[lsearch [$canvas gettags current] text] >= 0} {
	set id [expr $id-1]
    }
    $canvas itemconfigure $id -fill $oldFill
    $canvas itemconfigure [expr $id+1] -fill black
}

proc scrollButton canvas {
    global oldFill
    set id [$canvas find withtag current]
    if {[lsearch [$canvas gettags current] text] < 0} {
	set id [expr $id+1]
    }
    puts stdout "You buttoned at [lindex [$canvas itemconf $id -text] 4]"
}
