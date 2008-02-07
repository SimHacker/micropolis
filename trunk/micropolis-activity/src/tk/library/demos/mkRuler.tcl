# mkRuler w
#
# Create a canvas demonstration consisting of a ruler.
#
# Arguments:
#    w -	Name to use for new top-level window.
# This file implements a canvas widget that displays a ruler with tab stops
# that can be set individually.  The only procedure that should be invoked
# from outside the file is the first one, which creates the canvas.

proc mkRuler {{w .ruler}} {
    global tk_library
    upvar #0 demo_rulerInfo v
    catch {destroy $w}
    toplevel $w
    dpos $w
    wm title $w "Ruler Demonstration"
    wm iconname $w "Ruler"
    set c $w.c

    frame $w.frame1 -relief raised -bd 2
    canvas $c -width 14.8c -height 2.5c -relief raised
    button $w.ok -text "OK" -command "destroy $w"
    pack append $w $w.frame1 {top fill} $w.ok {bottom pady 10 frame center} \
	    $c {expand fill}
    message $w.frame1.m -font -Adobe-Times-Medium-R-Normal-*-180-* -aspect 300 \
	    -text "This canvas widget shows a mock-up of a ruler.  You can create tab stops by dragging them out of the well to the right of the ruler.  You can also drag existing tab stops.  If you drag a tab stop far enough up or down so that it turns dim, it will be deleted when you release the mouse button."
    pack append $w.frame1 $w.frame1.m {frame center}

    set v(grid) .25c
    set v(left) [winfo fpixels $c 1c]
    set v(right) [winfo fpixels $c 13c]
    set v(top) [winfo fpixels $c 1c]
    set v(bottom) [winfo fpixels $c 1.5c]
    set v(size) [winfo fpixels $c .2c]
    set v(normalStyle) "-fill black"
    if {[winfo screendepth $c] > 4} {
	set v(activeStyle) "-fill red -stipple {}"
	set v(deleteStyle) "-stipple @$tk_library/demos/bitmaps/grey.25 \
		-fill red"
    } else {
	set v(activeStyle) "-fill black -stipple {}"
	set v(deleteStyle) "-stipple @$tk_library/demos/bitmaps/grey.25 \
		-fill black"
    }

    $c create line 1c 0.5c 1c 1c 13c 1c 13c 0.5c -width 1
    for {set i 0} {$i < 12} {incr i} {
	set x [expr $i+1]
	$c create line ${x}c 1c ${x}c 0.6c -width 1
	$c create line $x.25c 1c $x.25c 0.8c -width 1
	$c create line $x.5c 1c $x.5c 0.7c -width 1
	$c create line $x.75c 1c $x.75c 0.8c -width 1
	$c create text $x.15c .75c -text $i -anchor sw
    }
    $c addtag well withtag [$c create rect 13.2c 1c 13.8c 0.5c \
	    -outline black -fill [lindex [$c config -bg] 4]]
    $c addtag well withtag [rulerMkTab $c [winfo pixels $c 13.5c] \
	    [winfo pixels $c .65c]]

    $c bind well <1> "rulerNewTab $c %x %y"
    $c bind tab <1> "demo_selectTab $c %x %y"
    bind $c <B1-Motion> "rulerMoveTab $c %x %y"
    bind $c <Any-ButtonRelease-1> "rulerReleaseTab $c"
}

proc rulerMkTab {c x y} {
    upvar #0 demo_rulerInfo v
    $c create polygon $x $y [expr $x+$v(size)] [expr $y+$v(size)] \
	    [expr $x-$v(size)] [expr $y+$v(size)]
}

proc rulerNewTab {c x y} {
    upvar #0 demo_rulerInfo v
    $c addtag active withtag [rulerMkTab $c $x $y]
    $c addtag tab withtag active
    set v(x) $x
    set v(y) $y
    rulerMoveTab $c $x $y
}

proc rulerMoveTab {c x y} {
    upvar #0 demo_rulerInfo v
    if {[$c find withtag active] == ""} {
	return
    }
    set cx [$c canvasx $x $v(grid)]
    set cy [$c canvasy $y]
    if {$cx < $v(left)} {
	set cx $v(left)
    }
    if {$cx > $v(right)} {
	set cx $v(right)
    }
    if {($cy >= $v(top)) && ($cy <= $v(bottom))} {
	set cy [expr $v(top)+2]
	eval "$c itemconf active $v(activeStyle)"
    } else {
	set cy [expr $cy-$v(size)-2]
	eval "$c itemconf active $v(deleteStyle)"
    }
    $c move active [expr $cx-$v(x)] [expr $cy-$v(y)]
    set v(x) $cx
    set v(y) $cy
}

proc demo_selectTab {c x y} {
    upvar #0 demo_rulerInfo v
    set v(x) [$c canvasx $x $v(grid)]
    set v(y) [expr $v(top)+2]
    $c addtag active withtag current
    eval "$c itemconf active $v(activeStyle)"
    $c raise active
}

proc rulerReleaseTab c {
    upvar #0 demo_rulerInfo v
    if {$v(y) != [expr $v(top)+2]} {
	$c delete active
    } else {
	eval "$c itemconf active $v(normalStyle)"
	$c dtag active
    }
}
