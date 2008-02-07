# mkHScale w
#
# Create a top-level window that displays a horizontal scale.
#
# Arguments:
#    w -	Name to use for new top-level window.

proc mkHScale {{w .scale2}} {
    catch {destroy $w}
    toplevel $w
    dpos $w
    wm title $w "Horizontal Scale Demonstration"
    wm iconname $w "Scale"
    message $w.msg -font -Adobe-times-medium-r-normal--*-180* -aspect 300 \
	    -text "A bar and a horizontal scale are displayed below.  If you click or drag mouse button 1 in the scale, you can change the width of the bar.  Click the \"OK\" button when you're finished."
    frame $w.frame -borderwidth 10
    pack append $w.frame \
	[frame $w.frame.top -borderwidth 15] {top expand frame sw} \
	[scale $w.frame.scale -orient horizontal -length 280 -from 0 -to 250 \
	    -command "setWidth $w.frame.top.inner" -tickinterval 50 \
	    -bg Bisque1] {bottom expand frame nw}
    pack append $w.frame.top \
	[frame $w.frame.top.inner -geometry 20x40 -relief raised \
	    -borderwidth 2 -bg SteelBlue1] {expand frame sw}
    $w.frame.scale set 20
    button $w.ok -text OK -command "destroy $w"

    pack append $w $w.msg {top fill} $w.frame {top expand fill} \
	$w.ok {bottom fill}
}

proc setWidth {w width} {
    $w config -geometry ${width}x40
}
