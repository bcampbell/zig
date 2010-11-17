# Makefile for Zig
#
# optimised build:	RELEASE=1
# crippled demo:	CRIPPLED=1
# win32:			WIN32=1
# OSX:				OSX=1
# cheats:			CHEATS=1

SRCS = agent.cpp \
agentmanager.cpp \
arena.cpp \
boss.cpp \
background.cpp \
beam.cpp \
boids.cpp \
bonuses.cpp \
bullets.cpp \
colours.cpp \
controller.cpp \
completionscreen.cpp \
crippleclock.cpp \
display.cpp \
drawing.cpp \
dudes.cpp \
edgepuller.cpp \
effects.cpp \
fatzapper.cpp \
gameover.cpp \
highscorescreen.cpp \
highscores.cpp \
humanoid.cpp \
image.cpp \
level.cpp \
leveldef.cpp \
loadpng.cpp \
mempool.cpp \
menusupport.cpp \
optionsmenu.cpp \
optionsscreen.cpp \
pausemenu.cpp \
perlinnoise.cpp \
player.cpp \
proceduraltextures.cpp \
vec2.cpp \
resources.cpp \
scene.cpp \
snow.cpp \
snowblower.cpp \
soundmgr.cpp \
spiker.cpp \
texture.cpp \
titlescreen.cpp \
util.cpp \
weapons.cpp \
wobbly.cpp \
zig.cpp \
zigconfig.cpp \
retromat/retromat.cpp \
retromat/sidstyle.cpp


OBJS = $(patsubst %.cpp,%.o,$(SRCS) )

CC := g++

ifdef CRIPPLED
	EXEBASE = zigdemo
else
	EXEBASE = zig
endif


ifdef WIN32
	# windows
	EXE = $(EXEBASE).exe
	CFLAGS = -Wall -I/usr/local/include -I/usr/local/include/SDL -DWIN32 -Dmain=SDL_main
	LIBS = -L/usr/local/lib -lpng -lz -lopengl32 -lglu32 -lmingw32 -lSDLmain -lSDL -lSDL_mixer -mwindows
	# resource file for icon
	OBJS += zig.opc
	STKPATH = /home/ben/working/stk-4.2.0
else
ifdef OSX
	# OSX
	EXE = $(EXEBASE)
#	CFLAGS = `sdl-config --cflags` -Wall
#	LIBS = `sdl-config --libs` -lpng -lz -lSDL_mixer
	CFLAGS = -I/Library/Frameworks/SDL.framework/Headers \
		-I/Library/Frameworks/SDL_mixer.framework/Headers \
		`libpng-config --cflags`

	LIBS = -framework SDL \
		-framework SDL_mixer \
		-framework Cocoa \
		-framework OpenGL \
		`libpng-config  --libs`

	OBJS += osx/SDLMain.o

	STKPATH = /Users/ben/working/stk-4.2.0
else
	# Linux
	EXE = $(EXEBASE)
	CFLAGS = `sdl-config --cflags` -Wall -I/usr/include/stk
	LIBS = `sdl-config --libs` -lpng -lz -lGL -lGLU -lSDL_mixer -lstk
#	STKPATH = /home/ben/proj/stk-4.2.1
endif
endif


ifdef CRIPPLED
	CFLAGS += -DCRIPPLED
endif

ifdef CHEATS
	CFLAGS += -DENABLE_CHEATS
endif

/* LIBS += -lstk */

ifdef PROFILE
	CFLAGS += -pg
endif

ifdef RELEASE
	CFLAGS += -O4 -DNDEBUG
else
	CFLAGS += -ggdb
endif



all: $(EXE)

$(EXE): $(OBJS)
	$(CC) $(CFLAGS) -o $(EXE) $(OBJS) $(LIBS)
ifdef RELEASE
	strip $(EXE)
endif


%.o : %.cpp
	$(CC) $(CFLAGS) -o $@ -c $<

ifdef WIN32
zig.opc: zig.rc zig.ico
	windres -i zig.rc zig.opc
endif

ifdef OSX
SDLMain.o: osx/SDLMain.m osx/SDLMain.h
	$(CC) $(CFLAGS) -c -o osx/SDLMain.o osx/SDLMain.m
endif

clean:
	rm -f $(OBJS)
	rm -f $(EXE)
	rm -f .depend
	rm -f tags

tags:
	ctags *.cpp *.h


depend:
	$(CC) -MM $(CFLAGS) $(SRCS) > .depend

ifeq ($(wildcard .depend),.depend)
include .depend
endif

