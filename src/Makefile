# -- Config --

exe=zig
srcs=$(wildcard *.cpp)
srcs += retromat/retromat.cpp retromat/sidstyle.cpp retromat/gen.cpp
objs := $(srcs:.cpp=.o)
deps := $(objs:.o=.d)


CXX = g++
CPPFLAGS = -MMD -MP
CXXFLAGS = -Wall -std=c++0x 
CXXFLAGS += -ggdb
LDFLAGS = -ggdb
#  CXXFLAGS += -O2 

ifeq ($(shell uname), Linux)
CXXFLAGS += $(shell pkg-config --cflags sdl2 SDL2_mixer libpng zlib gl)
LDLIBS += $(shell pkg-config --libs sdl2 SDL2_mixer libpng zlib gl)
endif

ifeq ($(shell uname), Darwin)
  objs += osx_glue.o
  CXXFLAGS += $(shell pkg-config --cflags libpng)
  CXXFLAGS += -F ~/Library/Frameworks
  # need to explicitly add include paths to allow SDL-prefered includes
  # (ie include <SDL2_mixer.h> instead of <SDL2_mixer/SDL2_mixer.h>
  CXXFLAGS += -I ~/Library/Frameworks/SDL2.framework/Headers
  CXXFLAGS += -I ~/Library/Frameworks/SDL2_mixer.framework/Headers
  LDLIBS += $(shell pkg-config --libs libpng)
  LDLIBS += -lz -framework OpenGL -framework Foundation -framework SDL2 -framework SDL2_mixer
  # Make sure SDL2 & SDL2_mixer will be found when bundled
  LDLIBS += -Xlinker -rpath -Xlinker @executable_path/../Frameworks
endif

ifeq ($(PLATFORM),win32)
  exe = zig.exe
  CXXFLAGS += $(shell pkg-config --cflags sdl2 SDL2_mixer libpng)
  LDLIBS += $(shell pkg-config --libs sdl2 SDL2_mixer libpng)
  CXXFLAGS += -Dmain=SDL_main
  #LIBS += -lmingw32 -lSDL2main -lSDL2 -lSDL2_mixer -lopengl32 -lpng16 -lz -mwindows
endif

ifeq ($(PLATFORM),emscripten)
  exe = zig.html
  CXX = em++
  CXXFLAGS = --embed-file data -s DISABLE_EXCEPTION_CATCHING=0 -s LEGACY_GL_EMULATION=1 -s USE_SDL=2 -s USE_ZLIB=1 -s USE_LIBPNG=1 -I /home/ben/proj/zig/dependencies/SDL2_mixer/installdir/include/SDL2
  LIBS = --emrun
endif

# -- Rules --

$(exe): $(objs)
	$(CXX) $(LDFLAGS) -o $(exe) $(objs) $(LDLIBS)

%.o: %.mm
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

.PHONY: clean
clean: ; $(RM) $(exe) $(objs) $(deps)

-include $(deps)

