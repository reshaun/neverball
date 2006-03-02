
#------------------------------------------------------------------------------

# Maybe you need one of these.  Maybe you don't.

#X11_PATH= -L/usr/X11/lib
#X11_PATH= -L/usr/X11R6/lib

OGL_LIBS= -lGL -lm
#OGL_LIBS= -lm                                                # Think Different

#------------------------------------------------------------------------------
# Configuration constants
#------------------------------------------------------------------------------

CFLAGS= -Wall -O3 -ansi -pedantic $(shell sdl-config --cflags)
#CFLAGS= -Wall -g -O1 -ansi -pedantic $(shell sdl-config --cflags)
#CFLAGS= -Wall -pg -ansi $(shell sdl-config --cflags)

SDL_LIBS= $(shell sdl-config --libs)
FT2_LIBS= $(shell freetype-config --libs)

MAPC_TARG= mapc
MAPC_EXEC = ./$(MAPC_TARG)
BALL_TARG= neverball
PUTT_TARG= neverputt

LOCALEDIR= locale
LOCALEDOM= neverball

POTFILE= po/neverball.pot

#-------------------------------------------------------------------------------

MAPC_OBJS= \
	share/vec3.o   \
	share/image.o  \
	share/solid.o  \
	share/binary.o \
	share/base_config.o \
	share/mapc.o
BALL_OBJS= \
	share/i18n.o    \
	share/st_lang.o \
	share/st_resol.o \
	share/vec3.o    \
	share/image.o   \
	share/solid.o   \
	share/part.o    \
	share/back.o    \
	share/geom.o    \
	share/gui.o     \
	share/base_config.o  \
	share/config.o  \
	share/binary.o  \
	share/state.o   \
	share/audio.o   \
	ball/hud.o      \
	ball/game.o     \
	ball/level.o    \
	ball/levels.o   \
	ball/set.o      \
	ball/demo.o     \
	ball/util.o     \
	ball/st_conf.o  \
	ball/st_demo.o  \
	ball/st_save.o  \
	ball/st_fail.o  \
	ball/st_goal.o  \
	ball/st_done.o  \
	ball/st_level.o \
	ball/st_over.o  \
	ball/st_play.o  \
	ball/st_set.o   \
	ball/st_start.o \
	ball/st_title.o \
	ball/st_name.o  \
	ball/st_shared.o  \
	ball/main.o
PUTT_OBJS= \
	share/i18n.o    \
	share/st_lang.o \
	share/st_resol.o \
	share/vec3.o   \
	share/image.o  \
	share/solid.o  \
	share/part.o   \
	share/geom.o   \
	share/back.o   \
	share/base_config.o  \
	share/config.o \
	share/binary.o \
	share/audio.o  \
	share/state.o  \
	share/gui.o    \
	putt/hud.o     \
	putt/game.o    \
	putt/hole.o    \
	putt/course.o  \
	putt/st_all.o  \
	putt/st_conf.o \
	putt/main.o

BALL_DEPS= $(BALL_OBJS:.o=.d)
PUTT_DEPS= $(PUTT_OBJS:.o=.d)
MAPC_DEPS= $(MAPC_OBJS:.o=.d)

LIBS= $(X11_PATH) $(SDL_LIBS) -lSDL_image -lSDL_ttf -lSDL_mixer $(FT2_LIBS) $(OGL_LIBS)

MESSAGEPART= /LC_MESSAGES/$(LOCALEDOM).mo
MESSAGES= $(LINGUAS:%=$(LOCALEDIR)/%$(MESSAGEPART))

MAPS= $(shell find data/ -name '*.map')
SOLS= $(MAPS:%.map=%.sol)

POS= $(shell echo po/*.po)
LINGUAS= $(POS:po/%.po=%)

#------------------------------------------------------------------------------
# Implicit rules
#------------------------------------------------------------------------------

%.d : %.c
	$(CC) $(CFLAGS) -Ishare -MM -MF $@ $<

%.o : %.c
	$(CC) $(CFLAGS) -Ishare -o $@ -c $<

%.sol : %.map $(MAPC_TARG)
	$(MAPC_EXEC) $< data

$(LOCALEDIR)/%$(MESSAGEPART) : po/%.po
	mkdir -p `dirname $@`
	msgfmt -c -v -o $@ $<

#------------------------------------------------------------------------------
# Main rules
#------------------------------------------------------------------------------

all : $(BALL_TARG) $(PUTT_TARG) $(MAPC_TARG) sols locales

$(BALL_TARG) : $(BALL_OBJS)
	$(CC) $(CFLAGS) -o $(BALL_TARG) $(BALL_OBJS) $(LIBS)

$(PUTT_TARG) : $(PUTT_OBJS)
	$(CC) $(CFLAGS) -o $(PUTT_TARG) $(PUTT_OBJS) $(LIBS)

$(MAPC_TARG) : $(MAPC_OBJS)
	$(CC) $(CFLAGS) -o $(MAPC_TARG) $(MAPC_OBJS) $(LIBS)

sols : $(SOLS)

locales : $(MESSAGES)

clean-src :
	rm -f $(BALL_TARG) $(BALL_OBJS) $(BALL_DEPS)
	rm -f $(PUTT_TARG) $(PUTT_OBJS) $(PUTT_DEPS)
	rm -f $(MAPC_TARG) $(MAPC_OBJS) $(MAPC_DEPS)

clean : clean-src
	rm -f $(SOLS)
	rm -rf $(LOCALEDIR)

test : all
	./neverball

#------------------------------------------------------------------------------
# PO update rules
#------------------------------------------------------------------------------

po/%.po : $(POTFILE)
	msgmerge -U $@ $<
	touch $@
	
po-update-extract :
	bash extractpo.sh $(POTFILE) $(LOCALEDOM)

po-update-merge : $(POS)

po-update : po-update-extract po-update-merge

#------------------------------------------------------------------------------
