#
# arrayprocs.tcl --
#
# Extended Tcl array procedures.
# 
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
# $Id: arrayprocs.tcl,v 2.0 1992/10/16 04:51:54 markd Rel $
#------------------------------------------------------------------------------
#

#@package: TclX-ArrayProcedures for_array_keys

proc for_array_keys {varName arrayName codeFragment} {
    upvar $varName enumVar $arrayName enumArray

    if ![info exists enumArray] {
	error "\"$arrayName\" isn't an array"
    }

    set searchId [array startsearch enumArray]
    while {[array anymore enumArray $searchId]} {
	set enumVar [array nextelement enumArray $searchId]
	uplevel $codeFragment
    }
    array donesearch enumArray $searchId
}
