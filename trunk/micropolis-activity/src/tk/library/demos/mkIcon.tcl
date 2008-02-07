# mkIcon w
#
# Create a top-level window that displays a bunch of iconic
# buttons.
#
# Arguments:
#    w -	Name to use for new top-level window.

proc mkIcon {{w .icon}} {
    global tk_library
    catch {destroy $w}
    toplevel $w
    dpos $w
    wm title $w "Iconic Button Demonstration"
    wm iconname $w "Icons"
    message $w.msg -font -Adobe-times-medium-r-normal--*-180* -aspect 300 \
	    -text "This window shows three buttons that display bitmaps instead of text.  On the left is a regular button, which changes its bitmap when you click on it.  On the right are two radio buttons.  Click the \"OK\" button when you're done."
    frame $w.frame -borderwidth 10
    pack append $w.frame \
	[button $w.frame.b1 -bitmap @$tk_library/demos/bitmaps/flagdown  \
		-command "iconCmd $w.frame.b1"] {left expand} \
	[frame $w.frame.right] {left expand}
    radiobutton $w.frame.right.b2 -bitmap @$tk_library/demos/bitmaps/letters \
	    -variable letters
    radiobutton $w.frame.right.b3 -bitmap @$tk_library/demos/bitmaps/noletters \
	    -variable letters
    pack append $w.frame.right $w.frame.right.b2 {top expand} \
	    $w.frame.right.b3 {top expand}
    button $w.ok -text OK -command "destroy $w"

    pack append $w $w.msg {top frame c} $w.frame {top expand fill} \
	    $w.ok {bottom fill}
}

proc iconCmd {w} {
    global tk_library
    set bitmap [lindex [$w config -bitmap] 4]
    if {$bitmap == "@$tk_library/demos/bitmaps/flagdown"} {
	$w config -bitmap @$tk_library/demos/bitmaps/flagup
    } else {
	$w config -bitmap @$tk_library/demos/bitmaps/flagdown
    }
}
