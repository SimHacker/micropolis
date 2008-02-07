PREFIX=/usr/local
DATADIR=$(PREFIX)/share/micropolis
LIBEXECDIR=$(PREFIX)/libexec
BINDIR=$(PREFIX)/bin
DOCDIR=$(PREFIX)/share/doc/micropolis
PIXMAPDIR=$(PREFIX)/share/pixmaps
APPLICATIONSDIR=$(PREFIX)/share/applications

INSTALL=install

DIRS=	$(DESTDIR)/$(DATADIR)/res/sounds $(DESTDIR)/$(DATADIR)/res/dejavu-lgc \
	$(DESTDIR)/$(DATADIR)/images $(DESTDIR)/$(DATADIR)/cities \
	$(DESTDIR)/$(LIBEXECDIR) $(DESTDIR)/$(BINDIR) $(DESTDIR)/$(DOCDIR) \
	$(DESTDIR)/$(PIXMAPDIR) $(DESTDIR)/$(APPLICATIONSDIR)

RES=	res/buildidx.tcl res/button.tcl res/entry.tcl res/help.tcl res/hexa.112 \
	res/hexa.232 res/hexa.384 res/hexa.385 res/hexa.386 res/hexa.387 \
	res/hexa.388 res/hexa.456 res/hexa.544 res/hexa.563 res/hexa.999 \
	res/init.tcl res/listbox.tcl res/menu.tcl res/micropolis.tcl \
	res/mkindex.tcl res/parray.tcl res/snro.111 res/snro.222 res/snro.333 \
	res/snro.444 res/snro.555 res/snro.666 res/snro.777 res/snro.888 \
	res/sound.tcl res/stri.202 res/stri.219 res/stri.301 res/stri.356 \
	res/tcl.tdx res/tcl.tlb res/tclindex res/tclinit.tcl res/text.tcl \
	res/tk.tcl res/tk.tdx res/tk.tlb res/tkerror.tcl res/wask.tcl \
	res/wbudget.tcl res/weditor.tcl res/weval.tcl res/wfile.tcl \
	res/wfrob.tcl res/wgraph.tcl res/whead.tcl res/whelp.tcl res/wish.tcl \
	res/wishx.tcl res/wmap.tcl res/wnotice.tcl res/wplayer.tcl \
	res/wscen.tcl res/wsplash.tcl

all: res/sim

res/sim: src/sim/sim
	cp src/sim/sim $@
	strip $@

src/sim/sim: tcl tk tclx sim
	@#

tcl:
	cd src/tcl && $(MAKE) MAKEFLAGS=

tk:
	cd src/tk && $(MAKE) MAKEFLAGS=

tclx:
	cd src/tclx && $(MAKE) MAKEFLAGS=

sim:
	cd src/sim && $(MAKE) MAKEFLAGS=

clean: 
	cd src/sim && $(MAKE) MAKEFLAGS= $@
	cd src/tcl && $(MAKE) MAKEFLAGS= $@
	cd src/tk && $(MAKE) MAKEFLAGS= $@
	cd src/tclx && $(MAKE) MAKEFLAGS= $@
	rm -f res/sim

install: res/sim install-dirs install-bin install-res install-images \
	install-cities install-doc install-desktop

install-dirs:
	$(INSTALL) -d $(DIRS)

install-bin:
	$(INSTALL) -m 0755 res/sim $(DESTDIR)/$(LIBEXECDIR)/sim
	$(INSTALL) -m 0755 res/sounds/player $(DESTDIR)/$(DATADIR)/res/sounds/player
	echo "SIMHOME=$(DATADIR); export SIMHOME" >$(DESTDIR)/$(BINDIR)/micropolis
	echo "echo \"Starting Micropolis in \$${SIMHOME} ... \"" >>$(DESTDIR)/$(BINDIR)/micropolis
	echo "cd $(DATADIR) && $(LIBEXECDIR)/sim \$$*" >>$(DESTDIR)/$(BINDIR)/micropolis
	chmod 755 $(DESTDIR)/$(BINDIR)/micropolis

install-res: install-res-sounds install-res-dejavu-lgc
	for file in $(RES); do \
		install -m 0644 $$file $(DESTDIR)/$(DATADIR)/$$file; \
	done

install-res-sounds:
	find res/sounds -type f -name \*.wav -exec $(INSTALL) -m 0644 {} $(DESTDIR)/$(DATADIR)/res/sounds/ \;

install-res-dejavu-lgc:
	find res/dejavu-lgc -type f -exec $(INSTALL) -m 0644 {} $(DESTDIR)/$(DATADIR)/res/dejavu-lgc/ \;

install-images:
	find images -type f -exec $(INSTALL) -m 0644 {} $(DESTDIR)/$(DATADIR)/images/ \;

install-cities:
	find cities -type f -exec $(INSTALL) -m 0644 {} $(DESTDIR)/$(DATADIR)/cities/ \;

install-doc:
	find manual -type f -exec $(INSTALL) -m 0644 {} $(DESTDIR)/$(DOCDIR)/ \;

install-desktop:
	$(INSTALL) -m 0644 Micropolis.desktop $(DESTDIR)/$(APPLICATIONSDIR)/micropolis.desktop
	$(INSTALL) -m 0644 Micropolis.png $(DESTDIR)/$(PIXMAPDIR)/micropolis.png

uninstall:
	rm -f $(DESTDIR)/$(BINDIR)/micropolis
	rm -f $(DESTDIR)/$(LIBEXECDIR)/sim
	-rmdir $(DESTDIR)/$(LIBEXECDIR)
	rm -f $(DESTDIR)/$(DATADIR)/res/sounds/player
	rm -f $(DESTDIR)/$(DATADIR)/res/sounds/*.wav
	-rmdir $(DESTDIR)/$(DATADIR)/res/sounds
	rm -f $(DESTDIR)/$(DATADIR)/res/dejavu-lgc/*ttf \
		$(DESTDIR)/$(DATADIR)/res/dejavu-lgc/fonts.alias \
		$(DESTDIR)/$(DATADIR)/res/dejavu-lgc/fonts.dir \
		$(DESTDIR)/$(DATADIR)/res/dejavu-lgc/fonts.scale
	-rmdir $(DESTDIR)/$(DATADIR)/res/dejavu-lgc
	for file in $(RES); do \
		rm $(DESTDIR)/$(DATADIR)/$$file; \
	done
	-rmdir $(DESTDIR)/$(DATADIR)/res
	rm -f $(DESTDIR)/$(DATADIR)/images/*.xpm
	-rmdir $(DESTDIR)/$(DATADIR)/images
	rm -f $(DESTDIR)/$(DATADIR)/cities/*.cty
	-rmdir $(DESTDIR)/$(DATADIR)/cities
	-rmdir $(DESTDIR)/$(DATADIR)
	rm -f $(DESTDIR)/$(DOCDIR)/*.html $(DESTDIR)/$(DOCDIR)/README
	-rmdir $(DESTDIR)/$(DOCDIR)
	rm -f $(DESTDIR)/$(APPLICATIONSDIR)/micropolis.desktop
	rm -f $(DESTDIR)/$(PIXMAPDIR)/micropolis.png

.PHONY: all clean install install-dirs install-bin install-res \
	install-res-sounds install-res-dejavu-lgc install-images \
	install-cities install-doc install-desktop uninstall tcl tk tclx sim
