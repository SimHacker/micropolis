#
# Eight Queens in Tcl
#
# by Karl Lehenbauer, 23 Nov 1990
#
#-----------------------------------------------------------------------------
# $Id: 8queens.tcl,v 2.0 1992/10/16 04:51:36 markd Rel $
#-----------------------------------------------------------------------------
#
proc none_attacking {rank position} {
    global queens
    loop i 1 $rank {
	set j $queens([expr {$rank - $i}])
	if {($j==$position-$i) || ($j==$position) || ($j==$position+$i)} {
	    return 0
	}
    }
    return 1
}

proc solution {} {
    global queens
    echo $queens(1) $queens(2) $queens(3) $queens(4) $queens(5) $queens(6) $queens(7) $queens(8)
}

proc x8queens {rank} {
    global queens
    set queens($rank) 1
    loop i 1 9 {
	if [none_attacking $rank $i] {
	    set queens($rank) $i
	    if {$rank != 8} {
		x8queens [expr $rank+1]
	    } else {
		solution
		return
	    }
	}
    }
}

proc 8queens {} {
    x8queens 1
}

global interactiveSession
if !$interactiveSession 8queens
