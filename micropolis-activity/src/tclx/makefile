#
# Makefile --
#
# Makefile for Extended Tcl.  This requires Tcl 6.1 or Tcl 6.2 from Berkeley,
# which should be compiled before running this makefile.  Its location is
# configured below.
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
# $Id: Makefile,v 2.0 1992/10/16 04:54:45 markd Rel $
#------------------------------------------------------------------------------
#

include config.mk
include config/$(TCL_CONFIG_FILE)
SHELL=/bin/sh

#------------------------------------------------------------------------------

# List of files that are touched by secondary makefiles when something changes.

MADE.FILES=ucbsrc/made.tmp ossupp/made.tmp src/made.tmp
TKMADE.FILES=tkucbsrc/made.tmp

CFLAGS= $(OPTIMIZE_FLAG) $(XCFLAGS) -I$(TCL_UCB_DIR) $(MEM_DEBUG_FLAGS) \
        $(SYS_DEP_FLAGS)

#------------------------------------------------------------------------------

all: tcl tcldef runtcl $(TCL_TK_SHELL)


#------------------------------------------------------------------------------
# Compile the Extended Tcl library and link the Tcl shell.
#

tcl: TCLX_MAKES

TCLX_MAKES: libtcl.a
	cd ucbsrc;    $(MAKE) -$(MAKEFLAGS) all
	cd ossupp;    $(MAKE) -$(MAKEFLAGS) all
	cd tclsrc;    $(MAKE) -$(MAKEFLAGS) all
	cd src;       $(MAKE) -$(MAKEFLAGS) all


# Copy the UCB libtcl.a file from where it was built.  Force the other
# Makefiles to add their .o files to the library by nuking their made.tmp file.

libtcl.a: $(TCL_UCB_DIR)/libtcl.a
	cp $(TCL_UCB_DIR)/libtcl.a .
	rm -f  $(MADE.FILES)

#------------------------------------------------------------------------------
# Generate a libtk.a with extensions and a wish shell with Extended Tcl
# commands.
#

wish: TKX_MAKES runwish

TKX_MAKES: libtk.a
	cd tkucbsrc; $(MAKE) -$(MAKEFLAGS) all
	cd tksrc;    $(MAKE) -$(MAKEFLAGS) all


# Copy the UCB libtk.a file.  Force the other Makefiles to add their
# .o files to the library by nuking their made.tmp file.

libtk.a: $(TCL_TK_DIR)/libtk.a
	cp $(TCL_TK_DIR)/libtk.a libtk.a
	rm -f $(TKMADE.FILES)


#------------------------------------------------------------------------------
# Generate a temporary TCLDEFAULT file so Tcl can be run in this directory.
# Also generate a script to point the TCLDEFAULT environment variable
# at this file for testing Tcl before its installed.

tcldef:
	@echo "	-       Generating temporary TCLDEFAULT file.     -"
	@echo "	- Use runtcl script to test Tcl before installing -"
	@echo "	- Use runwish script to test Tk before installing -"
	@echo '# Temporary TCLDEFAULT file for debugging'  >tcldef
	@echo "set TCLPATH `pwd`/tcllib"                  >>tcldef
	@echo "set TCLINIT `pwd`/tcllib/tclinit.tcl"      >>tcldef

runtcl:
	@echo ':'                                              >runtcl
	@echo '# script for testing Tcl before installation'  >>runtcl
	@echo "TCLDEFAULT=`pwd`/tcldef"                       >>runtcl
	@echo "export TCLDEFAULT"                             >>runtcl
	@echo "if [ \$$# = 0 ]"                               >>runtcl
	@echo "then"                                          >>runtcl
	@echo "    exec `pwd`/tcl"                            >>runtcl
	@echo "else"                                          >>runtcl
	@echo "    exec `pwd`/tcl \"\$$@\""                   >>runtcl
	@echo "fi"                                            >>runtcl
	chmod a+rx runtcl

runwish:
	@echo ':'                                              >runwish
	@echo '# script for testing wish before installation' >>runwish
	@echo "TCLDEFAULT=`pwd`/tcldef"                       >>runwish
	@echo "export TCLDEFAULT"                             >>runwish
	@echo "if [ \$$# = 0 ]"                               >>runwish
	@echo "then"                                          >>runwish
	@echo "    exec `pwd`/wish"                           >>runwish
	@echo "else"                                          >>runwish
	@echo "    exec `pwd`/wish \"\$$@\""                  >>runwish
	@echo "fi"                                            >>runwish
	chmod a+rx runwish

#------------------------------------------------------------------------------
#
#  just test to see if the C++ include file compiles and links

tclplus:
	cd src;$(MAKE) -$(MAKEFLAGS) tclplus

#------------------------------------------------------------------------------
#
# Run the UCB and Extended Tcl tests.

test: ucbtests extdtests

ucbtests: all
	@echo ""
	@echo "**************************************************"
	@echo "* Ignore failures in tests:  expr-2.2 & expr-2.6 *"
	@echo "**************************************************"
	@echo ""
	./runtcl -c "cd $(TCL_UCB_DIR)/tests;source all"

extdtests: all
	./runtcl -c "cd tests;source all"	

#------------------------------------------------------------------------------
# Install Tcl.

install: all
	./runtcl tclsrc/install.tcl


#------------------------------------------------------------------------------
# Clean up the mess we made.

clean:
	cd ucbsrc;    $(MAKE) -$(MAKEFLAGS) clean
	cd ossupp;    $(MAKE) -$(MAKEFLAGS) clean
	cd src;       $(MAKE) -$(MAKEFLAGS) clean
	cd tclsrc;    $(MAKE) -$(MAKEFLAGS) clean
	cd tkucbsrc;  $(MAKE) -$(MAKEFLAGS) clean
	cd tksrc;     $(MAKE) -$(MAKEFLAGS) clean
	touch junk~ config/junk~
	-rm -f *~ config/*~ libtcl.a tcldef libtk.a runtcl runwish
