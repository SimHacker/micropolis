XINCLUDE = /usr/include/X11
TCLHOME = ../tcl
TKHOME = ../tk
TCLXHOME = ../tclx
TCLLIBRARY = /usr/local/lib/tcl
TKLIBRARY = /usr/local/lib/tk

CC = gcc

OPTFLAGS = -O3
#OPTFLAGS = -g

#DEFINES = -DIS_LINUX -DIS_INTEL -DCAM -DNET
DEFINES = -DIS_LINUX -DIS_INTEL

CFLAGS = $(OPTFLAGS) $(DEFINES)

#LDFLAGS = -Bstatic

INSTALL = install -s

INCLUDES = \
	-Iheaders \
	-I$(XPMHOME) \
	-I$(XINCLUDE) \
	-I$(TCLHOME) \
	-I$(TCLXHOME)/src \
	-I$(TKHOME)

CPPFLAGS = $(INCLUDES)

LIBS =	$(TCLXHOME)/libtk.a \
	$(TCLXHOME)/libtcl.a \
	-lm \
        -lX11 \
        -lXext \
	-lXpm

SRCS = \
	sim.c \
	s_alloc.c \
	s_disast.c \
	s_eval.c \
	s_gen.c \
	s_init.c \
	s_msg.c \
	s_power.c \
	s_scan.c \
	s_sim.c \
	s_traf.c \
	s_zone.c \
	s_fileio.c \
	g_setup.c \
	g_ani.c \
	g_bigmap.c \
	g_map.c \
	g_smmaps.c \
	w_stubs.c \
	w_sound.c \
	w_resrc.c \
	w_keys.c \
	w_util.c \
	w_con.c \
	w_eval.c \
	w_budget.c \
	w_graph.c \
	w_date.c \
	w_update.c \
	w_tool.c \
	w_x.c \
	w_tk.c \
	w_sim.c \
	w_editor.c \
	w_map.c \
	w_sprite.c \
	w_piem.c \
	w_inter.c \
	w_cam.c \
	g_cam.c \
	w_net.c \
	rand.c \
	random.c

HEADERS = \
	headers/mac.h \
	headers/macros.h \
	headers/sim.h \
	headers/view.h \
	headers/animtab.h

OBJS = $(SRCS:.c=.o)
ALLOBJS = $(OBJS)

all: sim

lint:
	alint $(INCLUDES) $(SRCS) > LINT

sim: $(ALLOBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(INCLUDES) $(ALLOBJS) $(LIBS) -o sim

clean:
	rm -f $(OBJS) \
		sim a.out core gmon.out *~ *.BAK Headers/*~

etags:
	etags $(SRCS) $(HEADERS)
