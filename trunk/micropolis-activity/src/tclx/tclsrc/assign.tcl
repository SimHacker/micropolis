#
# asgnfields.tcl --
#
# Procedure to assign list fields to variables.
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
# $Id: asgnfields.tcl,v 2.0 1992/10/16 04:51:55 markd Rel $
#------------------------------------------------------------------------------
#

#@package: TclX-assign_fields assign_fields

proc assign_fields {list args} {
    foreach varName $args {
        set value [lvarpop list]
        uplevel "set $varName [list $value]"
    }
}
