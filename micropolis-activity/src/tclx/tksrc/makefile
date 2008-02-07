#
# Makefile --
#
# Makefile for to build Extended wish.
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
# $Id: Makefile,v 2.0 1992/10/16 04:54:30 markd Rel $
#------------------------------------------------------------------------------
#

include ../config.mk
include ../config/$(TCL_CONFIG_FILE)
SHELL=/bin/sh

#------------------------------------------------------------------------------

CFLAGS= $(OPTIMIZE_FLAG) $(XCFLAGS) -I$(TCL_UCB_DIR) $(MEM_DEBUG_FLAGS)

#------------------------------------------------------------------------------

all: ../wish

../wish: ../libtk.a ../libtcl.a
	$(RANLIB_CMD) ../libtk.a
	$(CC) $(CFLAGS) ../tkucbsrc/main.o ../libtk.a ../libtcl.a \
             $(XLDFLAGS) $(XPM_LIBS) $(TCL_TK_LIBS) -o ../wish
	if $(DO_STRIPPING) ; then \
	    strip ../wish; \
	    $(MCS_CMD) ../wish; fi

#------------------------------------------------------------------------------

clean:
	touch junk~
	-rm -f *~ ../wish
