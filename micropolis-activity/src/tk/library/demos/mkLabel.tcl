# mkLabel w
#
# Create a top-level window that displays a bunch of labels.
#
# Arguments:
#    w -	Name to use for new top-level window.

proc mkLabel {{w .l1}} {
    global tk_library
    catch {destroy $w}
    toplevel $w
    dpos $w
    wm title $w "Label Demonstration"
    wm iconname $w "Labels"
    message $w.msg -font -Adobe-times-medium-r-normal--*-180* -aspect 300 \
	    -text "Five labels are displayed below: three textual ones on the left, and a bitmap label and a text label on the right.  Labels are pretty boring because you can't do anything with them.  Click the \"OK\" button when you've seen enough."
    frame $w.frame -borderwidth 10
    pack append $w.frame \
	[frame $w.frame.right -borderwidth 10] {right} \
	[label $w.frame.l1 -text "First label"] {top frame w pady 4 expand} \
	[label $w.frame.l2 -text "Second label, raised just for fun" \
	    -relief raised] {top frame w pady 4 expand} \
	[label $w.frame.l3 -text "Third label, sunken" -relief sunken ] \
	    {top frame w pady 4 expand}
    pack append $w.frame.right \
	[label $w.frame.right.bitmap -bitmap @$tk_library/demos/bitmaps/face \
	    -borderwidth 2 -relief sunken] top \
	[label $w.frame.right.caption -text "Tcl/Tk Proprietor"] bottom
    button $w.ok -text OK -command "destroy $w"

    pack append $w $w.msg {top frame c} $w.frame {top expand fill} \
	    $w.ok {bottom fill}
}
