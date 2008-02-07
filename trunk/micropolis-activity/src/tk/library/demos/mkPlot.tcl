# mkPlot w
#
# Create a top-level window containing a canvas displaying a simple
# graph with data points that can be moved interactively.
#
# Arguments:
#    w -	Name to use for new top-level window.

proc mkPlot {{w .plot}} {
    catch {destroy $w}
    toplevel $w
    dpos $w
    wm title $w "Plot Demonstration"
    wm iconname $w "Plot"
    set c $w.c

    frame $w.frame1 -relief raised -bd 2
    canvas $c -relief raised -width 450 -height 300
    button $w.ok -text "OK" -command "destroy $w"
    pack append $w $w.frame1 {top fill} $w.c {expand fill} \
	    $w.ok {bottom pady 10 frame center}
    message $w.frame1.m -font -Adobe-Times-Medium-R-Normal-*-180-* -aspect 300 \
	    -text "This window displays a canvas widget containing a simple 2-dimensional plot.  You can doctor the data by dragging any of the points with mouse button 1."
    pack append $w.frame1 $w.frame1.m {frame center}

    set font -Adobe-helvetica-medium-r-*-180-*

    $c create line 100 250 400 250 -width 2
    $c create line 100 250 100 50 -width 2
    $c create text 225 20 -text "A Simple Plot" -font $font -fill brown
    
    for {set i 0} {$i <= 10} {incr i} {
	set x [expr {100 + ($i*30)}]
	$c create line $x 250 $x 245 -width 2
	$c create text $x 254 -text [expr 10*$i] -anchor n -font $font
    }
    for {set i 0} {$i <= 5} {incr i} {
	set y [expr {250 - ($i*40)}]
	$c create line 100 $y 105 $y -width 2
	$c create text 96 $y -text [expr $i*50].0 -anchor e -font $font
    }
    
    foreach point {{12 56} {20 94} {33 98} {32 120} {61 180}
	    {75 160} {98 223}} {
	set x [expr {100 + (3*[lindex $point 0])}]
	set y [expr {250 - (4*[lindex $point 1])/5}]
	set item [$c create oval [expr $x-6] [expr $y-6] \
		[expr $x+6] [expr $y+6] -width 1 -outline black \
		-fill SkyBlue2]
	$c addtag point withtag $item
    }

    $c bind point <Any-Enter> "$c itemconfig current -fill red"
    $c bind point <Any-Leave> "$c itemconfig current -fill SkyBlue2"
    $c bind point <1> "plotDown $c %x %y"
    $c bind point <ButtonRelease-1> "$c dtag selected"
    bind $c <B1-Motion> "plotMove $c %x %y"
}

set plot(lastX) 0
set plot(lastY) 0

proc plotDown {w x y} {
    global plot
    $w dtag selected
    $w addtag selected withtag current
    $w raise current
    set plot(lastX) $x
    set plot(lastY) $y
}

proc plotMove {w x y} {
    global plot
    $w move selected [expr $x-$plot(lastX)] [expr $y-$plot(lastY)]
    set plot(lastX) $x
    set plot(lastY) $y
}
