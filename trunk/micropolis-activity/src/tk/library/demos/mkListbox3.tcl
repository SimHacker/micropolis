# mkListbox3 w
#
# Create a top-level window containing a listbox with a bunch of well-known
# sayings.  The listbox can be scrolled or scanned in two dimensions.
#
# Arguments:
#    w -	Name to use for new top-level window.

proc mkListbox3 {{w .l3}} {
    catch {destroy $w}
    toplevel $w
    dpos $w
    wm title $w "Listbox Demonstration (well-known sayings)"
    wm iconname $w "Listbox"
    message $w.msg -font -Adobe-times-medium-r-normal--*-180* -aspect 300 \
	    -text "The listbox below contains a collection of well-known sayings.  You can scan the list using either of the scrollbars or by dragging in the listbox window with button 2 pressed.  Click the \"OK\" button when you're done."
    frame $w.frame -borderwidth 10
    pack append $w.frame \
	[scrollbar $w.frame.yscroll -relief sunken \
	    -command "$w.frame.list yview"] {right filly} \
	[scrollbar $w.frame.xscroll -relief sunken -orient horizontal \
	    -command "$w.frame.list xview"] {bottom fillx} \
	[listbox $w.frame.list -yscroll "$w.frame.yscroll set" \
		-xscroll "$w.frame.xscroll set" \
		-geometry 20x10 -relief sunken] {top}
    $w.frame.list insert 0 "Waste not, want not" "Early to bed and early to rise makes a man healthy, wealthy, and wise" "Ask not what your country can do for you, ask what you can do for your country" "I shall return" "NOT" "A picture is worth a thousand words" "User interfaces are hard to build" "Thou shalt not steal" "A penny for your thoughts" "Fool me once, shame on you;  fool me twice, shame on me" "Every cloud has a silver lining" "Where there's smoke there's fire" "It takes one to know one" "Curiosity killed the cat" "Take this job and shove it" "Up a creek without a paddle" "I'm mad as hell and I'm not going to take it any more" "An apple a day keeps the doctor away" "Don't look a gift horse in the mouth"
    button $w.ok -text OK -command "destroy $w"

    pack append $w $w.msg {top fill} $w.frame {top} \
	$w.ok {bottom fill}
}
