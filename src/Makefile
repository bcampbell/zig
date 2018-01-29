# -- Config --

exe=zig
srcs=$(wildcard *.cpp)
srcs += retromat/retromat.cpp retromat/sidstyle.cpp retromat/gen.cpp
objs := $(srcs:.cpp=.o)
deps := $(objs:.o=.d)


CXX = g++
CPPFLAGS = -MMD -MP
CXXFLAGS = -Wall -std=c++0x 


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
  CPPFLAGS += -I ~/Library/Frameworks/SDL2.framework/Headers
  CPPFLAGS += -I ~/Library/Frameworks/SDL2_mixer.framework/Headers
  LDFLAGS += -F ~/Library/Frameworks
  LDLIBS += $(shell pkg-config --libs libpng)
  LDLIBS += -lz -framework OpenGL -framework Foundation -framework SDL2 -framework SDL2_mixer
  # Make sure SDL2 & SDL2_mixer will be found when bundled
  LDLIBS += -Xlinker -rpath -Xlinker @executable_path/../Frameworks
endif

#ifeq ($(shell uname),MINGW64_NT-10.0)
#ifeq ($(shell uname),MINGW32_NT-10.0)
# NOTE: uname -o fails on osx
ifeq ($(shell uname -o), Msys)
  exe = zig.exe
  CXXFLAGS += $(shell pkg-config --cflags sdl2 SDL2_mixer libpng)
  LDLIBS += $(shell pkg-config --static --libs sdl2 SDL2_mixer libpng)
  LDLIBS += -lopengl32
  # use static linking under windows
  LDFLAGS += -static -static-libgcc -static-libstdc++
  objs += zig.res
endif

ifeq ($(PLATFORM),emscripten)
  exe = zig.html
  CXX = em++
  CXXFLAGS = --embed-file data -s DISABLE_EXCEPTION_CATCHING=0 -s LEGACY_GL_EMULATION=1 -s USE_SDL=2 -s USE_ZLIB=1 -s USE_LIBPNG=1 -I /home/ben/proj/zig/dependencies/SDL2_mixer/installdir/include/SDL2
  LIBS = --emrun
endif

# -- Rules --

# release:
.PHONY: release
release: CXXFLAGS += -O2 
release: $(exe)

# debug:
.PHONY: debug
debug: CXXFLAGS += -ggdb
debug: CPPFLAGS += -DENABLE_CHEATS
debug: LDFLAGS = -ggdb
debug: $(exe)


$(exe): $(objs)
	$(CXX) $(LDFLAGS) -o $(exe) $(objs) $(LDLIBS)

# -- mac stuff --
%.o: %.mm
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

# -- windows resource --
%.res: %.rc
	windres $< -O coff -o $@

zig.res: zig.ico

zig.ico: icon/icon.png
	/mingw32/bin/convert $< -define icon:auto-resize=16,32,48,256 -compress zip $@



.PHONY: clean
clean: ; $(RM) $(exe) $(objs) $(deps)

-include $(deps)

