# mkListbox w
#
# Create a top-level window that displays a listbox with the names of the
# 50 states.
#
# Arguments:
#    w -	Name to use for new top-level window.

proc mkListbox {{w .l1}} {
    catch {destroy $w}
    toplevel $w
    dpos $w
    wm title $w "Listbox Demonstration (50 states)"
    wm iconname $w "Listbox"
    message $w.msg -font -Adobe-times-medium-r-normal--*-180* -aspect 300 \
	    -text "A listbox containing the 50 states is displayed below, along with a scrollbar.  You can scan the list either using the scrollbar or by dragging in the listbox window with button 2 pressed.  Click the \"OK\" button when you've seen enough."
    frame $w.frame -borderwidth 10
    pack append $w.frame \
	[scrollbar $w.frame.scroll -relief sunken \
	    -command "$w.frame.list yview"] {right expand filly frame w} \
	[listbox $w.frame.list -yscroll "$w.frame.scroll set" -relief sunken] \
	    {left expand filly frame e}
    $w.frame.list insert 0 Alabama Alaska Arizona Arkansas California \
	Colorado Connecticut Delaware Florida Georgia Hawaii Idaho Illinois \
	Indiana Iowa Kansas Kentucky Louisiana Maine Maryland \
        Massachusetts Michigan Minnesota Mississippi Missouri \
        Montana Nebraska Nevada "New Hampshire" "New Jersey" "New Mexico" \
	"New York" "North Carolina" "North Dakota" \
        Ohio Oklahoma Oregon Pennsylvania "Rhode Island" \
        "South Carolina" "South Dakota" \
        Tennessee Texas Utah Vermont Virginia Washington \
        "West Virginia" Wisconsin Wyoming
    button $w.ok -text OK -command "destroy $w"

    pack append $w $w.msg {top fill} $w.frame {top expand fill} \
	$w.ok {bottom fill}
}
