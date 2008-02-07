# mkArrow w
#
# Create a top-level window containing a canvas demonstration that
# allows the user to experiment with arrow shapes.
#
# Arguments:
#    w -	Name to use for new top-level window.

# This file implements a canvas widget that displays a large line with
# an arrowhead and allows the shape of the arrowhead to be edited
# interactively.  The only procedure that should be invoked from outside
# the file is the first one, which creates the canvas.

proc mkArrow {{w .arrow}} {
    global tk_library
    upvar #0 demo_arrowInfo v
    catch {destroy $w}
    toplevel $w
    dpos $w
    wm title $w "Arrowhead Editor Demonstration"
    wm iconname $w "Arrow"
    set c $w.c

    frame $w.frame1 -relief raised -bd 2
    canvas $c -width 500 -height 350 -relief raised
    button $w.ok -text "OK" -command "destroy $w"
    pack append $w $w.frame1 {top fill} $w.ok {bottom pady 10 frame center} \
	    $c {expand fill}
    message $w.frame1.m -font -Adobe-Times-Medium-R-Normal-*-180-* -aspect 300 \
	    -text "This widget allows you to experiment with different widths and arrowhead shapes for lines in canvases.  To change the line width or the shape of the arrowhead, drag any of the three boxes attached to the oversized arrow.  The arrows on the right give examples at normal scale.  The text at the bottom shows the configuration options as you'd enter them for a line."
    pack append $w.frame1 $w.frame1.m {frame center}


    set v(a) 8
    set v(b) 10
    set v(c) 3
    set v(width) 2
    set v(motionProc) arrowMoveNull
    set v(x1) 40
    set v(x2) 350
    set v(y) 150
    set v(smallTips) {5 5 2}
    set v(count) 0
    if {[winfo screendepth $c] > 4} {
	set v(bigLineStyle) "-fill SkyBlue1"
	set v(boxStyle) "-fill {} -outline black -width 1"
	set v(activeStyle) "-fill red -outline black -width 1"
    } else {
	set v(bigLineStyle) "-fill black -stipple @$tk_library/demos/bitmaps/grey.25"
	set v(boxStyle) "-fill {} -outline black -width 1"
	set v(activeStyle) "-fill black -outline black -width 1"
    }
    arrowSetup $c
    $c bind box <Enter> "$c itemconfigure current $v(activeStyle)"
    $c bind box <Leave> "$c itemconfigure current $v(boxStyle)"
    $c bind box1 <1> {set demo_arrowInfo(motionProc) arrowMove1}
    $c bind box2 <1> {set demo_arrowInfo(motionProc) arrowMove2}
    $c bind box3 <1> {set demo_arrowInfo(motionProc) arrowMove3}
    $c bind box <B1-Motion> "\$demo_arrowInfo(motionProc) $c %x %y"
    bind $c <Any-ButtonRelease-1> "arrowSetup $c"
}

# The procedure below completely regenerates all the text and graphics
# in the canvas window.  It's called when the canvas is initially created,
# and also whenever any of the parameters of the arrow head are changed
# interactively.  The argument is the name of the canvas widget to be
# regenerated, and also the name of a global variable containing the
# parameters for the display.

proc arrowSetup c {
    upvar #0 demo_arrowInfo v
    $c delete all

    # Create the arrow and outline.

    eval "$c create line $v(x1) $v(y) $v(x2) $v(y) -width [expr 10*$v(width)] \
	    -arrowshape {[expr 10*$v(a)] [expr 10*$v(b)] [expr 10*$v(c)]} \
	    -arrow last $v(bigLineStyle)"
    set xtip [expr $v(x2)-10*$v(b)]
    set deltaY [expr 10*$v(c)+5*$v(width)]
    $c create line $v(x2) $v(y) $xtip [expr $v(y)+$deltaY] \
	    [expr $v(x2)-10*$v(a)] $v(y) $xtip [expr $v(y)-$deltaY] \
	    $v(x2) $v(y) -width 2 -capstyle round -joinstyle round

    # Create the boxes for reshaping the line and arrowhead.

    eval "$c create rect [expr $v(x2)-10*$v(a)-5] [expr $v(y)-5] \
	    [expr $v(x2)-10*$v(a)+5] [expr $v(y)+5] $v(boxStyle) \
	    -tags {box1 box}"
    eval "$c create rect [expr $xtip-5] [expr $v(y)-$deltaY-5] \
	    [expr $xtip+5] [expr $v(y)-$deltaY+5] $v(boxStyle) \
	    -tags {box2 box}"
    eval "$c create rect [expr $v(x1)-5] [expr $v(y)-5*$v(width)-5] \
	    [expr $v(x1)+5] [expr $v(y)-5*$v(width)+5] $v(boxStyle) \
	    -tags {box3 box}"

    # Create three arrows in actual size with the same parameters

    $c create line [expr $v(x2)+50] 0 [expr $v(x2)+50] 1000 \
	    -width 2
    set tmp [expr $v(x2)+100]
    $c create line $tmp [expr $v(y)-125] $tmp [expr $v(y)-75] \
	    -width $v(width) \
	    -arrow both -arrowshape "$v(a) $v(b) $v(c)"
    $c create line [expr $tmp-25] $v(y) [expr $tmp+25] $v(y) \
	    -width $v(width) \
	    -arrow both -arrowshape "$v(a) $v(b) $v(c)"
    $c create line [expr $tmp-25] [expr $v(y)+75] [expr $tmp+25] \
	    [expr $v(y)+125] -width $v(width) \
	    -arrow both -arrowshape "$v(a) $v(b) $v(c)"

    # Create a bunch of other arrows and text items showing the
    # current dimensions.

    set tmp [expr $v(x2)+10]
    $c create line $tmp [expr $v(y)-5*$v(width)] \
	    $tmp [expr $v(y)-$deltaY] \
	    -arrow both -arrowshape $v(smallTips)
    $c create text [expr $v(x2)+15] [expr $v(y)-$deltaY+5*$v(c)] \
	    -text $v(c) -anchor w
    set tmp [expr $v(x1)-10]
    $c create line $tmp [expr $v(y)-5*$v(width)] \
	    $tmp [expr $v(y)+5*$v(width)] \
	    -arrow both -arrowshape $v(smallTips)
    $c create text [expr $v(x1)-15] $v(y) -text $v(width) -anchor e
    set tmp [expr $v(y)+5*$v(width)+10*$v(c)+10]
    $c create line [expr $v(x2)-10*$v(a)] $tmp $v(x2) $tmp \
	    -arrow both -arrowshape $v(smallTips)
    $c create text [expr $v(x2)-5*$v(a)] [expr $tmp+5] \
	    -text $v(a) -anchor n
    set tmp [expr $tmp+25]
    $c create line [expr $v(x2)-10*$v(b)] $tmp $v(x2) $tmp \
	    -arrow both -arrowshape $v(smallTips)
    $c create text [expr $v(x2)-5*$v(b)] [expr $tmp+5] \
	    -text $v(b) -anchor n

    $c create text $v(x1) 310 -text "-width  $v(width)" \
	    -anchor w -font -Adobe-Helvetica-Medium-R-Normal-*-180-*
    $c create text $v(x1) 330 -text "-arrowshape  {$v(a)  $v(b)  $v(c)}" \
	    -anchor w -font -Adobe-Helvetica-Medium-R-Normal-*-180-*

    incr v(count)
}

# The procedures below are called in response to mouse motion for one
# of the three items used to change the line width and arrowhead shape.
# Each procedure updates one or more of the controlling parameters
# for the line and arrowhead, and recreates the display if that is
# needed.  The arguments are the name of the canvas widget, and the
# x and y positions of the mouse within the widget.

proc arrowMove1 {c x y} {
    upvar #0 demo_arrowInfo v
    set newA [expr ($v(x2)+5-[$c canvasx $x])/10]
    if {$newA < 1} {
	set newA 1
    }
    if {$newA > 25} {
	set newA 25
    }
    if {$newA != $v(a)} {
	$c move box1 [expr 10*($v(a)-$newA)] 0
	set v(a) $newA
    }
}

proc arrowMove2 {c x y} {
    upvar #0 demo_arrowInfo v
    set newB [expr ($v(x2)+5-[$c canvasx $x])/10]
    if {$newB < 1} {
	set newB 1
    }
    if {$newB > 25} {
	set newB 25
    }
    set newC [expr ($v(y)+5-[$c canvasy $y]-5*$v(width))/10]
    if {$newC < 1} {
	set newC 1
    }
    if {$newC > 20} {
	set newC 20
    }
    if {($newB != $v(b)) || ($newC != $v(c))} {
	$c move box2 [expr 10*($v(b)-$newB)] [expr 10*($v(c)-$newC)]
	set v(b) $newB
	set v(c) $newC
    }
}

proc arrowMove3 {c x y} {
    upvar #0 demo_arrowInfo v
    set newWidth [expr ($v(y)+5-[$c canvasy $y])/5]
    if {$newWidth < 1} {
	set newWidth 1
    }
    if {$newWidth > 20} {
	set newWidth 20
    }
    if {$newWidth != $v(width)} {
	$c move box3 0 [expr 5*($v(width)-$newWidth)]
	set v(width) $newWidth
    }
}
