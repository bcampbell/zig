#ARCH := $(firstword $(shell uname -m))	# x86_64, i386, etc..
#SYS := $(firstword $(shell uname -s))	# Linux, Darwin, etc...

# which platform? linux, win32 or osx
PLATFORM=$(shell ./zig-platform)

EXE = zig

SRCS=$(wildcard *.cpp)
SRCS += retromat/retromat.cpp retromat/sidstyle.cpp retromat/gen.cpp

OBJS = $(patsubst %.cpp,%.o,$(SRCS) )

ifeq ($(PLATFORM),linux)
  CXXFLAGS = -Wall -std=c++0x -ggdb $(shell pkg-config --cflags sdl2 SDL2_mixer libpng zlib gl)
  LIBS = $(shell pkg-config --libs sdl2 SDL2_mixer libpng zlib gl)
endif
ifeq ($(PLATFORM),osx)
  CXXFLAGS = -Wall -O2 $(shell pkg-config --cflags libpng)
  CXXFLAGS += -F ~/Library/Frameworks
  # need to explicitly add include paths to allow SDL-prefered includes
  # (ie include <SDL2_mixer.h> instead of <SDL2_mixer/SDL2_mixer.h>
  CXXFLAGS += -I ~/Library/Frameworks/SDL2.framework/Headers
  CXXFLAGS += -I ~/Library/Frameworks/SDL2_mixer.framework/Headers

  LIBS = $(shell pkg-config --libs libpng)
  LIBS += -lz -framework OpenGL -framework Foundation -framework SDL2 -framework SDL2_mixer
  # Make sure SDL2 & SDL2_mixer will be found when bundled
  LIBS += -Xlinker -rpath -Xlinker @executable_path/../Frameworks
  OBJS += osx_glue.o
endif
ifeq ($(PLATFORM),win32)
  EXE = zig.exe
  CXXFLAGS = -Wall -std=c++0x -O2 -Dmain=SDL_main -I/mingw32/include/libpng16
  CXXFLAGS += -I/c/site/SDL2_mixer-2.0.0/i686-w64-mingw32/include/SDL2 -I/c/site/SDL2-2.0.3/i686-w64-mingw32/include/SDL2
  LIBS = -L/c/site/SDL2_mixer-2.0.0/i686-w64-mingw32/lib -L/c/site/SDL2-2.0.3/i686-w64-mingw32/lib
  LIBS += -lmingw32 -lSDL2main -lSDL2 -lSDL2_mixer -lopengl32 -lpng16 -lz -mwindows
  CXX=g++
endif


$(EXE): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(EXE) $(OBJS) $(LIBS)

ifeq ($(PLATFORM),osx)
%.o: %.mm
	$(CXX) -c $(CXXFLAGS) $< -o $@ 
endif

.PHONY: clean
clean:
	rm -f $(OBJS) $(EXE)

.PHONY: depend
depend: .depend

.depend:
	$(CXX) $(CXXFLAGS) -MM $^ >.depend

include .depend

