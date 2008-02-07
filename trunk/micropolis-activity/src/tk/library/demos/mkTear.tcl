# mkTear w
#
# Create a top-level window that displays a help message on tear-off
# menus.
#
# Arguments:
#    w -	Name to use for new top-level window.

proc mkTear {{w .t1}} {
    catch {destroy $w}
    toplevel $w
    dpos $w
    wm title $w "Information On Tear-Off Menus"
    wm iconname $w "Info"
    message $w.msg -font -Adobe-times-medium-r-normal--*-180* -aspect 250 \
	    -text "To tear off a menu, press mouse button 2 over the menubutton for the menu, then drag the menu with button 2 held down.  You can reposition a torn-off menu by pressing button 2 on it and dragging again.  To unpost the menu, click mouse button 1 over the menu's menubutton.  Click the \"OK\" button when you're finished with this window."
    button $w.ok -text OK -command "destroy $w"

    pack append $w $w.msg {top fill} $w.ok {bottom fill}
}
