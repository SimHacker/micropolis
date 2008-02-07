# mkItems w
#
# Create a top-level window containing a canvas that displays the
# various item types and allows them to be selected and moved.  This
# demo can be used to test out the point-hit and rectangle-hit code
# for items.
#
# Arguments:
#    w -	Name to use for new top-level window.

proc mkItems {{w .citems}} {
    global c tk_library
    catch {destroy $w}
    toplevel $w
    dpos $w
    wm title $w "Canvas Item Demonstration"
    wm iconname $w "Items"
    wm minsize $w 100 100
    set c $w.frame2.c

    frame $w.frame1 -relief raised -bd 2
    frame $w.frame2 -relief raised -bd 2
    button $w.ok -text "OK" -command "destroy $w"
    pack append $w $w.frame1 {top fill} $w.frame2 {top fill expand} \
	    $w.ok {bottom pady 10 frame center}
    message $w.frame1.m -font -Adobe-Times-Medium-R-Normal-*-180-* -aspect 300 \
	    -text "This window contains a canvas widget with examples of the various kinds of items supported by canvases.  The following operations are supported:\n  Button-1 drag:\tmoves item under pointer.\n  Button-2 drag:\trepositions view.\n  Button-3 drag:\tstrokes out area.\n  Ctrl+f:\t\tprints items under area."
    pack append $w.frame1 $w.frame1.m {frame center}

    canvas $c -scrollregion {0c 0c 30c 24c} -width 15c -height 10c
    scrollbar $w.frame2.vscroll  -relief sunken -command "$c yview"
    scrollbar $w.frame2.hscroll -orient horiz -relief sunken -command "$c xview"
    pack append $w.frame2 $w.frame2.hscroll {bottom fillx} \
	    $w.frame2.vscroll {right filly} $c {expand fill}
    $c config -xscroll "$w.frame2.hscroll set" -yscroll "$w.frame2.vscroll set"

    # Display a 3x3 rectangular grid.

    $c create rect 0c 0c 30c 24c -width 2
    $c create line 0c 8c 30c 8c -width 2
    $c create line 0c 16c 30c 16c -width 2
    $c create line 10c 0c 10c 24c -width 2
    $c create line 20c 0c 20c 24c -width 2

    set font1 -Adobe-Helvetica-Medium-R-Normal-*-120-*
    set font2 -Adobe-Helvetica-Bold-R-Normal-*-240-*
    if {[winfo screendepth $c] > 4} {
	set blue DeepSkyBlue3
	set red red
	set bisque bisque3
	set green SeaGreen3
    } else {
	set blue black
	set red black
	set bisque black
	set green black
    }

    # Set up demos within each of the areas of the grid.

    $c create text 5c .2c -text Lines -anchor n
    $c create line 1c 1c 3c 1c 1c 4c 3c 4c -width 2m -fill $blue \
	    -cap butt -join miter -tags item
    $c create line 4.67c 1c 4.67c 4c -arrow last -tags item
    $c create line 6.33c 1c 6.33c 4c -arrow both -tags item
    $c create line 5c 6c 9c 6c 9c 1c 8c 1c 8c 4.8c 8.8c 4.8c 8.8c 1.2c \
	    8.2c 1.2c 8.2c 4.6c 8.6c 4.6c 8.6c 1.4c 8.4c 1.4c 8.4c 4.4c \
	    -width 3 -fill $red -tags item
    $c create line 1c 5c 7c 5c 7c 7c 9c 7c -width .5c \
	    -stipple @$tk_library/demos/bitmaps/grey.25 \
	    -arrow both -arrowshape {15 15 7} -tags item
    $c create line 1c 7c 1.75c 5.8c 2.5c 7c 3.25c 5.8c 4c 7c -width .5c \
	    -cap round -join round -tags item

    $c create text 15c .2c -text "Curves (smoothed lines)" -anchor n
    $c create line 11c 4c 11.5c 1c 13.5c 1c 14c 4c -smooth on \
	    -fill $blue -tags item
    $c create line 15.5c 1c 19.5c 1.5c 15.5c 4.5c 19.5c 4c -smooth on \
	    -arrow both -width 3 -tags item
    $c create line 12c 6c 13.5c 4.5c 16.5c 7.5c 18c 6c \
	    16.5c 4.5c 13.5c 7.5c 12c 6c -smooth on -width 3m -cap round \
	    -stipple @$tk_library/demos/bitmaps/grey.25 -fill $red -tags item

    $c create text 25c .2c -text Polygons -anchor n
    $c create polygon 21c 1.0c 22.5c 1.75c 24c 1.0c 23.25c 2.5c \
	    24c 4.0c 22.5c 3.25c 21c 4.0c 21.75c 2.5c -fill $green -tags item
    $c create polygon 25c 4c 25c 4c 25c 1c 26c 1c 27c 4c 28c 1c \
	    29c 1c 29c 4c 29c 4c -fill $red -smooth on -tags item
    $c create polygon 22c 4.5c 25c 4.5c 25c 6.75c 28c 6.75c \
	    28c 5.25c 24c 5.25c 24c 6.0c 26c 6c 26c 7.5c 22c 7.5c \
	    -stipple @$tk_library/demos/bitmaps/grey.25 -tags item

    $c create text 5c 8.2c -text Rectangles -anchor n
    $c create rectangle 1c 9.5c 4c 12.5c -outline $red -width 3m -tags item
    $c create rectangle 0.5c 13.5c 4.5c 15.5c -fill $green -tags item
    $c create rectangle 6c 10c 9c 15c -outline {} \
	    -stipple @$tk_library/demos/bitmaps/grey.25 -fill $blue -tags item

    $c create text 15c 8.2c -text Ovals -anchor n
    $c create oval 11c 9.5c 14c 12.5c -outline $red -width 3m -tags item
    $c create oval 10.5c 13.5c 14.5c 15.5c -fill $green -tags item
    $c create oval 16c 10c 19c 15c -outline {} \
	    -stipple @$tk_library/demos/bitmaps/grey.25 -fill $blue -tags item

    $c create text 25c 8.2c -text Text -anchor n
    $c create rectangle 22.4c 8.9c 22.6c 9.1c
    $c create text 22.5c 9c -anchor n -font $font1 -width 4c \
	    -text "A short string of text, word-wrapped, justified left, and anchored north (at the top).  The rectangles show the anchor points for each piece of text." -tags item
    $c create rectangle 25.4c 10.9c 25.6c 11.1c
    $c create text 25.5c 11c -anchor w -font $font1 -fill $blue \
	    -text "Several lines,\n each centered\nindividually,\nand all anchored\nat the left edge." \
	    -justify center -tags item
    $c create rectangle 24.9c 13.9c 25.1c 14.1c
    $c create text 25c 14c -font $font2 -anchor c -fill $red \
	    -stipple @$tk_library/demos/bitmaps/grey.5 \
	    -text "Stippled characters" -tags item

    $c create text 5c 16.2c -text Arcs -anchor n
    $c create arc 0.5c 17c 7c 20c -fill $green -outline black \
	    -start 45 -extent 270 -style pieslice -tags item
    $c create arc 6.5c 17c 9.5c 20c -width 4m -style arc \
	    -fill $blue -start -135 -extent 270 \
	    -stipple @$tk_library/demos/bitmaps/grey.25 -tags item
    $c create arc 0.5c 20c 9.5c 24c -width 4m -style pieslice \
	    -fill {} -outline $red -start 225 -extent -90 -tags item
    $c create arc 5.5c 20.5c 9.5c 23.5c -width 4m -style chord \
	    -fill $blue -outline {} -start 45 -extent 270  -tags item

    $c create text 15c 16.2c -text Bitmaps -anchor n
    $c create bitmap 13c 20c -bitmap @$tk_library/demos/bitmaps/face -tags item
    $c create bitmap 17c 18.5c \
	    -bitmap @$tk_library/demos/bitmaps/noletters -tags item
    $c create bitmap 17c 21.5c \
	    -bitmap @$tk_library/demos/bitmaps/letters -tags item

    $c create text 25c 16.2c -text Windows -anchor n
    button $c.button -text "Press Me" -command "butPress $c $red"
    $c create window 21c 18c -window $c.button -anchor nw -tags item
    entry $c.entry -width 20 -relief sunken
    $c.entry insert end "Edit this text"
    $c create window 21c 21c -window $c.entry -anchor nw -tags item
    scale $c.scale -from 0 -to 100 -length 6c -sliderlength .4c \
	    -width .5c -tickinterval 0
    $c create window 28.5c 17.5c -window $c.scale -anchor n -tags item
    $c create text 21c 17.9c -text Button: -anchor sw
    $c create text 21c 20.9c -text Entry: -anchor sw
    $c create text 28.5c 17.4c -text Scale: -anchor s

    # Set up event bindings for canvas:

    $c bind item <Any-Enter> "itemEnter $c"
    $c bind item <Any-Leave> "itemLeave $c"
    bind $c <2> "$c scan mark %x %y"
    bind $c <B2-Motion> "$c scan dragto %x %y"
    bind $c <3> "itemMark $c %x %y"
    bind $c <B3-Motion> "itemStroke $c %x %y"
    bind $c <Control-f> "itemsUnderArea $c"
    bind $c <1> "itemStartDrag $c %x %y"
    bind $c <B1-Motion> "itemDrag $c %x %y"
    bind $w <Any-Enter> "focus $c"
}

# Utility procedures for highlighting the item under the pointer:

proc itemEnter {c} {
    global restoreCmd

    if {[winfo screendepth $c] <= 4} {
	set restoreCmd {}
	return
    }
    set type [$c type current]
    if {$type == "window"} {
	set restoreCmd {}
	return
    }
    if {$type == "bitmap"} {
	set bg [lindex [$c itemconf current -background] 4]
	set restoreCmd [list $c itemconfig current -background $bg]
	$c itemconfig current -background SteelBlue2
	return
    }
    set fill [lindex [$c itemconfig current -fill] 4]
    if {(($type == "rectangle") || ($type == "oval") || ($type == "arc"))
	    && ($fill == "")} {
	set outline [lindex [$c itemconfig current -outline] 4]
	set restoreCmd "$c itemconfig current -outline $outline"
	$c itemconfig current -outline SteelBlue2
    } else {
	set restoreCmd "$c itemconfig current -fill $fill"
	$c itemconfig current -fill SteelBlue2
    }
}

proc itemLeave {c} {
    global restoreCmd

    eval $restoreCmd
}

# Utility procedures for stroking out a rectangle and printing what's
# underneath the rectangle's area.

proc itemMark {c x y} {
    global areaX1 areaY1
    set areaX1 [$c canvasx $x]
    set areaY1 [$c canvasy $y]
    $c delete area
}

proc itemStroke {c x y} {
    global areaX1 areaY1 areaX2 areaY2
    set x [$c canvasx $x]
    set y [$c canvasy $y]
    if {($areaX1 != $x) && ($areaY1 != $y)} {
	$c delete area
	$c addtag area withtag [$c create rect $areaX1 $areaY1 $x $y \
		-outline black]
	set areaX2 $x
	set areaY2 $y
    }
}

proc itemsUnderArea {c} {
    global areaX1 areaY1 areaX2 areaY2
    set area [$c find withtag area]
    set items ""
    foreach i [$c find enclosed $areaX1 $areaY1 $areaX2 $areaY2] {
	if {[lsearch [$c gettags $i] item] != -1} {
	    lappend items $i
	}
    }
    puts stdout "Items enclosed by area: $items"
    set items ""
    foreach i [$c find overlapping $areaX1 $areaY1 $areaX2 $areaY2] {
	if {[lsearch [$c gettags $i] item] != -1} {
	    lappend items $i
	}
    }
    puts stdout "Items overlapping area: $items"
}

set areaX1 0
set areaY1 0
set areaX2 0
set areaY2 0

# Utility procedures to support dragging of items.

proc itemStartDrag {c x y} {
    global lastX lastY
    set lastX [$c canvasx $x]
    set lastY [$c canvasy $y]
}

proc itemDrag {c x y} {
    global lastX lastY
    set x [$c canvasx $x]
    set y [$c canvasy $y]
    $c move current [expr $x-$lastX] [expr $y-$lastY]
    set lastX $x
    set lastY $y
}

# Procedure that's invoked when the button embedded in the canvas
# is invoked.

proc butPress {w color} {
    set i [$w create text 25c 18.1c -text "Ouch!!" -fill $color -anchor n]
    after 500 "$w delete $i"
}
