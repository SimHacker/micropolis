# mkCanvText w
#
# Create a top-level window containing a canvas displaying a text
# string and allowing the string to be edited and re-anchored.
#
# Arguments:
#    w -	Name to use for new top-level window.

proc mkCanvText {{w .ctext}} {
    catch {destroy $w}
    toplevel $w
    dpos $w
    wm title $w "Canvas Text Demonstration"
    wm iconname $w "Text"
    set c $w.c

    frame $w.frame1 -relief raised -bd 2
    canvas $c -relief raised -width 500 -height 400
    button $w.ok -text "OK" -command "destroy $w"
    pack append $w $w.frame1 {top fill} $w.c {expand fill} \
	    $w.ok {bottom pady 10 frame center}
    message $w.frame1.m -font -Adobe-Times-Medium-R-Normal-*-180-* -aspect 300 \
	    -text "This window displays a string of text to demonstrate the text facilities of canvas widgets.  You can point, click, and type.  You can also select and then delete with Control-d.  You can copy the selection with Control-v.  You can click in the boxes to adjust the position of the text relative to its positioning point or change its justification."
    pack append $w.frame1 $w.frame1.m {frame center}

    set font -Adobe-helvetica-medium-r-*-240-*

    $c create rectangle 245 195 255 205 -outline black -fill red

    # First, create the text item and give it bindings so it can be edited.
    
    $c addtag text withtag [$c create text 250 200 -text "This is just a string of text to demonstrate the text facilities of canvas widgets. You can point, click, and type.  You can also select and then delete with Control-d." -width 440 -anchor n -font $font -justify left]
    $c bind text <1> "textB1Press $c %x %y"
    $c bind text <B1-Motion> "textB1Move $c %x %y"
    $c bind text <Shift-1> "$c select adjust current @%x,%y"
    $c bind text <Shift-B1-Motion> "textB1Move $c %x %y"
    $c bind text <KeyPress> "$c insert text cursor %A"
    $c bind text <Shift-KeyPress> "$c insert text cursor %A"
    $c bind text <Control-h> "textBs $c"
    $c bind text <Delete> "textBs $c"
    $c bind text <Control-d> "$c dchars text sel.first sel.last"
    $c bind text <Control-v> "$c insert text cursor \[selection get\]"

    # Next, create some items that allow the text's anchor position
    # to be edited.

    set x 50
    set y 50
    set color LightSkyBlue1
    mkTextConfig $c $x $y -anchor se $color
    mkTextConfig $c [expr $x+30] [expr $y] -anchor s $color
    mkTextConfig $c [expr $x+60] [expr $y] -anchor sw $color
    mkTextConfig $c [expr $x] [expr $y+30] -anchor e $color
    mkTextConfig $c [expr $x+30] [expr $y+30] -anchor center $color
    mkTextConfig $c [expr $x+60] [expr $y+30] -anchor w $color
    mkTextConfig $c [expr $x] [expr $y+60] -anchor ne $color
    mkTextConfig $c [expr $x+30] [expr $y+60] -anchor n $color
    mkTextConfig $c [expr $x+60] [expr $y+60] -anchor nw $color
    set item [$c create rect [expr $x+40] [expr $y+40] [expr $x+50] [expr $y+50] \
	    -outline black -fill red]
    $c bind $item <1> "$c itemconf text -anchor center"
    $c create text [expr $x+45] [expr $y-5] -text {Text Position} -anchor s \
	    -font -Adobe-times-medium-r-normal--*-240-* -fill brown

    # Lastly, create some items that allow the text's justification to be
    # changed.
    
    set x 350
    set y 50
    set color SeaGreen2
    mkTextConfig $c $x $y -justify left $color
    mkTextConfig $c [expr $x+30] [expr $y] -justify center $color
    mkTextConfig $c [expr $x+60] [expr $y] -justify right $color
    $c create text [expr $x+45] [expr $y-5] -text {Justification} -anchor s \
	    -font -Adobe-times-medium-r-normal--*-240-* -fill brown

    $c bind config <Enter> "textEnter $c"
    $c bind config <Leave> "$c itemconf current -fill \$textConfigFill"
}

proc mkTextConfig {w x y option value color} {
    set item [$w create rect [expr $x] [expr $y] [expr $x+30] [expr $y+30] \
	    -outline black -fill $color -width 1]
    $w bind $item <1> "$w itemconf text $option $value"
    $w addtag config withtag $item
}

set textConfigFill {}

proc textEnter {w} {
    global textConfigFill
    set textConfigFill [lindex [$w itemconfig current -fill] 4]
    $w itemconfig current -fill black
}

proc textB1Press {w x y} {
    $w cursor current @$x,$y
    $w focus current
    focus $w
    $w select from current @$x,$y
}

proc textB1Move {w x y} {
    $w select to current @$x,$y
}

proc textBs {w} {
    set char [expr {[$w index text cursor] - 1}]
    if {$char >= 0} {$w dchar text $char}
}
