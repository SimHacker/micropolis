#
# setfuncs --
#
# Perform set functions on lists.  Also has a procedure for removing duplicate
# list entries.
#------------------------------------------------------------------------------
# Copyright 1992 Karl Lehenbauer and Mark Diekhans.
#
# Permission to use, copy, modify, and distribute this software and its
# documentation for any purpose and without fee is hereby granted, provided
# that the above copyright notice appear in all copies.  Karl Lehenbauer and
# Mark Diekhans make no representations about the suitability of this
# software for any purpose.  It is provided "as is" without express or
# implied warranty.
#------------------------------------------------------------------------------
# $Id: setfuncs.tcl,v 2.0 1992/10/16 04:52:10 markd Rel $
#------------------------------------------------------------------------------
#

#@package: TclX-set_functions union intersect intersect3 lrmdups

#
# return the logical union of two lists, removing any duplicates
#
proc union {lista listb} {
    set full_list [lsort [concat $lista $listb]]
    set check_element [lindex $full_list 0]
    set outlist $check_element
    foreach element [lrange $full_list 1 end] {
	if {$check_element == $element} continue
	lappend outlist $element
	set check_element $element
    }
    return $outlist
}

#
# sort a list, returning the sorted version minus any duplicates
#
proc lrmdups {list} {
    set list [lsort $list]
    set result [lvarpop list]
    lappend last $result
    foreach element $list {
	if {$last != $element} {
	    lappend result $element
	    set last $element
	}
    }
    return $result
}

#
# intersect3 - perform the intersecting of two lists, returning a list
# containing three lists.  The first list is everything in the first
# list that wasn't in the second, the second list contains the intersection
# of the two lists, the third list contains everything in the second list
# that wasn't in the first.
#

proc intersect3 {list1 list2} {
    set list1Result ""
    set list2Result ""
    set intersectList ""

    set list1 [lrmdups $list1]
    set list2 [lrmdups $list2]

    while {1} {
        if [lempty $list1] {
            if ![lempty $list2] {
                set list2Result [concat $list2Result $list2]
            }
            break
        }
        if [lempty $list2] {
	    set list1Result [concat $list1Result $list1]
            break
        }
        set compareResult [string compare [lindex $list1 0] [lindex $list2 0]]

        if {$compareResult < 0} {
            lappend list1Result [lvarpop list1]
            continue
        }
        if {$compareResult > 0} {
            lappend list2Result [lvarpop list2]
            continue
        }
        lappend intersectList [lvarpop list1]
        lvarpop list2
    }
    return [list $list1Result $intersectList $list2Result]
}

#
# intersect - perform an intersection of two lists, returning a list
# containing every element that was present in both lists
#
proc intersect {list1 list2} {
    set intersectList ""

    set list1 [lsort $list1]
    set list2 [lsort $list2]

    while {1} {
        if {[lempty $list1] || [lempty $list2]} break

        set compareResult [string compare [lindex $list1 0] [lindex $list2 0]]

        if {$compareResult < 0} {
            lvarpop list1
            continue
        }

        if {$compareResult > 0} {
            lvarpop list2
            continue
        }

        lappend intersectList [lvarpop list1]
        lvarpop list2
    }
    return $intersectList
}


