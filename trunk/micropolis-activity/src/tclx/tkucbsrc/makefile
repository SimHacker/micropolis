#
# Makefile --
#
# Makefile for building special version of tk source to support Extended Tcl.
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
# $Id: Makefile,v 2.0 1992/10/16 04:54:52 markd Rel $
#------------------------------------------------------------------------------
#

include ../config.mk
include ../config/$(TCL_CONFIG_FILE)
SHELL=/bin/sh

#------------------------------------------------------------------------------

CFLAGS= $(OPTIMIZE_FLAG) $(XCFLAGS) -I../src -I../$(TCL_TK_DIR) \
        -I../$(TCL_UCB_DIR) -I$(XHOME)/include \
	$(MEM_DEBUG_FLAGS) $(SYS_DEP_FLAGS) \
        -DTK_VERSION=\"2.2/\"

#------------------------------------------------------------------------------
#
# Targets to build:

LIBOBJS   = tkevent.o
OBJS      = main.o $(LIBOBJS)
WISHX_TCL = ../tcllib/wishx.tcl
TK_TCL    = ../tcllib/tk.tcl
TK_TLIB   = ../tcllib/tk.tlib

#------------------------------------------------------------------------------
#
# Original UCB Sources to build them from:

MAIN_SRC  = ../$(TCL_TK_DIR)/main.c
EVENT_SRC = ../$(TCL_TK_DIR)/tkevent.c
WISHX_SRC = ../$(TCL_TK_DIR)/library/wish.tcl
TK_TCL_SRC= ../$(TCL_TK_DIR)/library/tk.tcl

#------------------------------------------------------------------------------

#all: made.tmp $(WISHX_TCL) $(TK_TLIB) $(TK_TCL)
all: made.tmp $(WISHX_TCL) $(TK_TCL)

made.tmp: $(OBJS)
	-$(AR) d ../libtk.a $(LIBOBJS)
	$(AR) cr ../libtk.a $(LIBOBJS)
	touch made.tmp

main.c: $(MAIN_SRC) main.sed
	rm -f main.c
	sed -f main.sed <$(MAIN_SRC) >main.c

tkevent.c: $(EVENT_SRC) tkevent.sed
	rm -f tkevent.c
	sed -f tkevent.sed <$(EVENT_SRC) >tkevent.c

$(WISHX_TCL): $(WISHX_SRC) wishx.sed
	rm -f $(WISHX_TCL)
	sed -f wishx.sed <$(WISHX_SRC) >$(WISHX_TCL)

$(TK_TCL): $(TK_TCL_SRC)
	rm -f $(TK_TCL)
	cp $(TK_TCL_SRC) $(TK_TCL)

$(TK_TLIB): ../$(TCL_TK_DIR)/library/tclindex
	TCLDEFAULT=../tcldef;export TCLDEFAULT ;\
	../tcl "-c convert_lib ../$(TCL_TK_DIR)/library/tclindex $(TK_TLIB)"

#------------------------------------------------------------------------------

clean:
	touch junk~
	rm -f *~ $(OBJS) made.tmp
	rm -f tkevent.c main.c
	rm -f $(WISHX_TCL) $(TK_TCL) $(TK_TLIB) ../tcllib/tk.tdx
