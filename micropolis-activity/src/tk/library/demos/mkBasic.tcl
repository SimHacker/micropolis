# mkBasic w
#
# Create a top-level window that displays a basic text widget.
#
# Arguments:
#    w -	Name to use for new top-level window.

proc mkBasic {{w .basic}} {
    catch {destroy $w}
    toplevel $w
    dpos $w
    wm title $w "Text Demonstration - Basic Facilities"
    wm iconname $w "Text Basics"
    button $w.ok -text OK -command "destroy $w"
    text $w.t -relief raised -bd 2 -yscrollcommand "$w.s set" -setgrid true
    scrollbar $w.s -relief flat -command "$w.t yview"
    pack append $w $w.ok {bottom fillx} $w.s {right filly} $w.t {expand fill}
    $w.t insert 0.0 {\
This window is a text widget.  It displays one or more lines of text
and allows you to edit the text.  Here is a summary of the things you
can do to a text widget:

1. Scrolling. Use the scrollbar to adjust the view in the text window.

2. Scanning. Press mouse button 2 in the text window and drag up or down.
This will drag the text at high speed to allow you to scan its contents.

3. Insert text. Press mouse button 1 to set the insertion cursor, then
type text.  What you type will be added to the widget.  You can backspace
over what you've typed using either the backspace key, the delete key,
or Control+h.

4. Select. Press mouse button 1 and drag to select a range of characters.
Once you've released the button, you can adjust the selection by pressing
button 1 with the shift key down.  This will reset the end of the
selection nearest the mouse cursor and you can drag that end of the
selection by dragging the mouse before releasing the mouse button.
You can double-click to select whole words, or triple-click to select
whole lines.

5. Delete. To delete text, select the characters you'd like to delete
and type Control+d.

6. Copy the selection. To copy the selection either from this window
or from any other window or application, select what you want, click
button 1 to set the insertion cursor, then type Control+v to copy the
selection to the point of the insertion cursor.

7. Resize the window.  This widget has been configured with the "setGrid"
option on, so that if you resize the window it will always resize to an
even number of characters high and wide.  Also, if you make the window
narrow you can see that long lines automatically wrap around onto
additional lines so that all the information is always visible.

When you're finished with this demonstration, press the "OK" button
below.}
    $w.t mark set insert 0.0
    bind $w <Any-Enter> "focus $w.t"
}
