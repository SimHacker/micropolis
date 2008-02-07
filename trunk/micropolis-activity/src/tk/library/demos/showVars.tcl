# showVars w var var var ...
#
# Create a top-level window that displays a bunch of global variable values
# and keeps the display up-to-date even when the variables change value
#
# Arguments:
#    w -	Name to use for new top-level window.
#    var -	Name of variable to monitor.

proc showVars {w args} {
    catch {destroy $w}
    toplevel $w
    wm title $w "Variable values"
    label $w.title -text "Variable values:" -width 20 -anchor center \
	    -font -Adobe-helvetica-medium-r-normal--*-180*
    pack append $w $w.title {top fillx}
    foreach i $args {
	frame $w.$i
	label $w.$i.name -text "$i: "
	label $w.$i.value -textvar $i
	pack append $w.$i $w.$i.name left $w.$i.value left
	pack append $w $w.$i {top frame w}
    }
    button $w.ok -text OK -command "destroy $w"
    pack append $w $w.ok {bottom pady 2}
}
