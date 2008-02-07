# mkRadio w
#
# Create a top-level window that displays a bunch of radio buttons.
#
# Arguments:
#    w -	Name to use for new top-level window.

proc mkRadio {{w .r1}} {
    catch {destroy $w}
    toplevel $w
    dpos $w
    wm title $w "Radiobutton Demonstration"
    wm iconname $w "Radiobuttons"
    message $w.msg -font -Adobe-times-medium-r-normal--*-180* -aspect 300 \
	    -text "Two groups of radiobuttons are displayed below.  If you click on a button then the button will become selected exclusively among all the buttons in its group.  A Tcl variable is associated with each group to indicate which of the group's buttons is selected.  Click the \"See Variables\" button to see the current values of the variables.  Click the \"OK\" button when you've seen enough."
    frame $w.frame -borderwidth 10
    pack append $w.frame \
	[frame $w.frame.left] {left expand} \
	[frame $w.frame.right] {right expand}
    pack append $w.frame.left \
	[radiobutton $w.frame.left.b1 -text "Point Size 10" -variable size \
	    -relief flat -value 10] {top pady 4 frame w} \
	[radiobutton $w.frame.left.b2 -text "Point Size 12" -variable size \
	    -relief flat -value 12] {top pady 4 frame w} \
	[radiobutton $w.frame.left.b3 -text "Point Size 18" -variable size \
	    -relief flat -value 18] {top pady 4 frame w} \
	[radiobutton $w.frame.left.b4 -text "Point Size 24" -variable size \
	    -relief flat -value 24] {top pady 4 frame w}
    pack append $w.frame.right \
	[radiobutton $w.frame.right.b1 -text "Red" -variable color \
	    -relief flat -value red] {top pady 4 frame w} \
	[radiobutton $w.frame.right.b2 -text "Green" -variable color \
	    -relief flat -value green] {top pady 4 frame w} \
	[radiobutton $w.frame.right.b3 -text "Blue" -variable color \
	    -relief flat -value blue] {top pady 4 frame w} \
	[radiobutton $w.frame.right.b4 -text "Yellow" -variable color \
	    -relief flat -value yellow] {top pady 4 frame w} \
	[radiobutton $w.frame.right.b5 -text "Orange" -variable color \
	    -relief flat -value orange] {top pady 4 frame w} \
	[radiobutton $w.frame.right.b6 -text "Purple" -variable color \
	    -relief flat -value purple] {top pady 4 frame w}
    frame $w.frame2
    pack append $w.frame2 \
	[button $w.frame2.ok -text OK -command "destroy $w"] \
	    {left expand fill} \
	[button $w.frame2.vars -text "See Variables" \
	    -command "showVars $w.dialog size color"] \
	    {left expand fill}
    button $w.ok -text OK -command "destroy $w"

    pack append $w $w.msg {top fill} $w.frame {top expand fill} \
	    $w.frame2 {bottom fill}
}
