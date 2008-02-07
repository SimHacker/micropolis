# mkDialog w msgArgs list list ...
#
# Create a dialog box with a message and any number of buttons at
# the bottom.
#
# Arguments:
#    w -	Name to use for new top-level window.
#    msgArgs -	List of arguments to use when creating the message of the
#		dialog box (e.g. text, justifcation, etc.)
#    list -	A two-element list that describes one of the buttons that
#		will appear at the bottom of the dialog.  The first element
#		gives the text to be displayed in the button and the second
#		gives the command to be invoked when the button is invoked.

proc mkDialog {w msgArgs args} {
    catch {destroy $w}
    toplevel $w -class Dialog
    wm title $w "Dialog box"
    wm iconname $w "Dialog"

    # Create two frames in the main window. The top frame will hold the
    # message and the bottom one will hold the buttons.  Arrange them
    # one above the other, with any extra vertical space split between
    # them.

    frame $w.top -relief raised -border 1
    frame $w.bot -relief raised -border 1
    pack append $w $w.top {top fill expand} $w.bot {top fill expand}
    
    # Create the message widget and arrange for it to be centered in the
    # top frame.
    
    eval message $w.top.msg -justify center \
	    -font -Adobe-times-medium-r-normal--*-180* $msgArgs
    pack append $w.top $w.top.msg {top expand padx 5 pady 5}
    
    # Create as many buttons as needed and arrange them from left to right
    # in the bottom frame.  Embed the left button in an additional sunken
    # frame to indicate that it is the default button, and arrange for that
    # button to be invoked as the default action for clicks and returns in
    # the dialog.

    if {[llength $args] > 0} {
	set arg [lindex $args 0]
	frame $w.bot.0 -relief sunken -border 1
	pack append $w.bot $w.bot.0 {left expand padx 20 pady 20}
	button $w.bot.0.button -text [lindex $arg 0] \
		-command "[lindex $arg 1]; destroy $w"
	pack append $w.bot.0 $w.bot.0.button {expand padx 12 pady 12}
	bind $w <Return> "[lindex $arg 1]; destroy $w"
	focus $w

	set i 1
	foreach arg [lrange $args 1 end] {
	    button $w.bot.$i -text [lindex $arg 0] \
		    -command "[lindex $arg 1]; destroy $w"
	    pack append $w.bot $w.bot.$i {left expand padx 20}
	    set i [expr $i+1]
	}
    }
    bind $w <Any-Enter> [list focus $w]
    focus $w
}
