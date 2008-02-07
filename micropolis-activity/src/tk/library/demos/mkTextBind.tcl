# mkTextBind w
#
# Create a top-level window that illustrates how you can bind
# Tcl commands to regions of text in a text widget.
#
# Arguments:
#    w -	Name to use for new top-level window.

proc mkTextBind {{w .bindings}} {
    catch {destroy $w}
    toplevel $w
    dpos $w
    wm title $w "Text Demonstration - Tag Bindings"
    wm iconname $w "Text Bindings"
    button $w.ok -text OK -command "destroy $w"
    text $w.t -relief raised -bd 2 -yscrollcommand "$w.s set" -setgrid true \
	    -width 60 -height 28 \
	    -font "-Adobe-Helvetica-Bold-R-Normal-*-120-*"
    scrollbar $w.s -relief flat -command "$w.t yview"
    pack append $w $w.ok {bottom fillx} $w.s {right filly} $w.t {expand fill}

    # Set up display styles

    if {[winfo screendepth $w] > 4} {
	set bold "-foreground red"
	set normal "-foreground {}"
    } else {
	set bold "-foreground white -background black"
	set normal "-foreground {} -background {}"
    }
    $w.t insert 0.0 {\
The same tag mechanism that controls display styles in text
widgets can also be used to associate Tcl commands with regions
of text, so that mouse or keyboard actions on the text cause
particular Tcl commands to be invoked.  For example, in the
text below the descriptions of the canvas demonstrations have
been tagged.  When you move the mouse over a demo description
the description lights up, and when you press button 3 over a
description then that particular demonstration is invoked.

This demo package contains a number of demonstrations of Tk's
canvas widgets.  Here are brief descriptions of some of the
demonstrations that are available:

}
    insertWithTags $w.t \
{1. Samples of all the different types of items that can be
created in canvas widgets.} d1
    insertWithTags $w.t \n\n
    insertWithTags $w.t \
{2. A simple two-dimensional plot that allows you to adjust
the positions of the data points.} d2
    insertWithTags $w.t \n\n
    insertWithTags $w.t \
{3. Anchoring and justification modes for text items.} d3
    insertWithTags $w.t \n\n
    insertWithTags $w.t \
{4. An editor for arrow-head shapes for line items.} d4
    insertWithTags $w.t \n\n
    insertWithTags $w.t \
{5. A ruler with facilities for editing tab stops.} d5
    insertWithTags $w.t \n\n
    insertWithTags $w.t \
{6. A grid that demonstrates how canvases can be scrolled.} d6

    foreach tag {d1 d2 d3 d4 d5 d6} {
	$w.t tag bind $tag <Any-Enter> "$w.t tag configure $tag $bold"
	$w.t tag bind $tag <Any-Leave> "$w.t tag configure $tag $normal"
    }
    $w.t tag bind d1 <3> mkItems
    $w.t tag bind d2 <3> mkPlot
    $w.t tag bind d3 <3> mkCanvText
    $w.t tag bind d4 <3> mkArrow
    $w.t tag bind d5 <3> mkRuler
    $w.t tag bind d6 <3> mkScroll

    $w.t mark set insert 0.0
    bind $w <Any-Enter> "focus $w.t"
}

# The procedure below inserts text into a given text widget and
# applies one or more tags to that text.  The arguments are:
#
# w		Window in which to insert
# text		Text to insert (it's inserted at the "insert" mark)
# args		One or more tags to apply to text.  If this is empty
#		then all tags are removed from the text.

proc insertWithTags {w text args} {
    set start [$w index insert]
    $w insert insert $text
    foreach tag [$w tag names $start] {
	$w tag remove $tag $start insert
    }
    foreach i $args {
	$w tag add $i $start insert
    }
}
