# mkStyles w
#
# Create a top-level window with a text widget that demonstrates the
# various display styles that are available in texts.
#
# Arguments:
#    w -	Name to use for new top-level window.

proc mkStyles {{w .styles}} {
    catch {destroy $w}
    toplevel $w
    dpos $w
    wm title $w "Text Demonstration - Display Styles"
    wm iconname $w "Text Styles"
    button $w.ok -text OK -command "destroy $w"
    text $w.t -relief raised -bd 2 -yscrollcommand "$w.s set" -setgrid true \
	    -width 70 -height 28
    scrollbar $w.s -relief flat -command "$w.t yview"
    pack append $w $w.ok {bottom fillx} $w.s {right filly} $w.t {expand fill}

    # Set up display styles

    $w.t tag configure bold -font -Adobe-Courier-Bold-O-Normal-*-120-*
    $w.t tag configure big -font -Adobe-Courier-Bold-R-Normal-*-140-*
    $w.t tag configure verybig -font -Adobe-Helvetica-Bold-R-Normal-*-240-*
    if {[winfo screendepth $w] > 4} {
	$w.t tag configure color1 -background #eed5b7
	$w.t tag configure color2 -foreground red
	$w.t tag configure raised -background #eed5b7 -relief raised \
		-borderwidth 1
	$w.t tag configure sunken -background #eed5b7 -relief sunken \
		-borderwidth 1
    } else {
	$w.t tag configure color1 -background black -foreground white
	$w.t tag configure color2 -background black -foreground white
	$w.t tag configure raised -background white -relief raised \
		-borderwidth 1
	$w.t tag configure sunken -background white -relief sunken \
		-borderwidth 1
    }
    $w.t tag configure bgstipple -background black -borderwidth 0 \
	    -bgstipple gray25
    $w.t tag configure fgstipple -fgstipple gray50
    $w.t tag configure underline -underline on

    $w.t insert 0.0 {\
Text widgets like this one allow you to display information in a
variety of styles.  Display styles are controlled using a mechanism
called }
    insertWithTags $w.t tags bold
    insertWithTags $w.t {. Tags are just textual names that you can apply to one
or more ranges of characters within a text widget.  You can configure
tags with various display styles.  If you do this, then the tagged
characters will be displayed with the styles you chose.  The
available display styles are:
}
    insertWithTags $w.t {
1. Font.} big
    insertWithTags $w.t {  You can choose any X font, }
    insertWithTags $w.t large verybig
    insertWithTags $w.t { or }
    insertWithTags $w.t {small.
}
    insertWithTags $w.t {
2. Color.} big
    insertWithTags $w.t {  You can change either the }
    insertWithTags $w.t background color1
    insertWithTags $w.t { or }
    insertWithTags $w.t foreground color2
    insertWithTags $w.t {
color, or }
    insertWithTags $w.t both color1 color2
    insertWithTags $w.t {.
}
    insertWithTags $w.t {
3. Stippling.} big
    insertWithTags $w.t {  You can cause either the }
    insertWithTags $w.t background bgstipple
    insertWithTags $w.t { or }
    insertWithTags $w.t foreground fgstipple
    insertWithTags $w.t {
information to be drawn with a stipple fill instead of a solid fill.
}
    insertWithTags $w.t {
4. Underlining.} big
    insertWithTags $w.t {  You can }
    insertWithTags $w.t underline underline
    insertWithTags $w.t { ranges of text.
}
    insertWithTags $w.t {
5. 3-D effects.} big
    insertWithTags $w.t {  You can arrange for the background to be drawn
with a border that makes characters appear either }
    insertWithTags $w.t raised raised
    insertWithTags $w.t { or }
    insertWithTags $w.t sunken sunken
    insertWithTags $w.t {.
}
    insertWithTags $w.t {
6. Yet to come.} big
    insertWithTags $w.t {  More display effects will be coming soon, such
as the ability to change line justification and perhaps line spacing.}

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
