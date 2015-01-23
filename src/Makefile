ARCH := $(firstword $(shell uname -m))	# x86_64, i386, etc..
SYS := $(firstword $(shell uname -s))	# Linux, Darwin, etc...

ifneq (,$(findstring MINGW,$(SYS)))
  PLATFORM=Windows
else
  PLATFORM=$(SYS)
endif


ifeq ($(PLATFORM),Windows)
  EXE = zig.exe
  CXXFLAGS = -O2 $(shell pkg-config --cflags sdl2 SDL2_mixer libpng zlib)
  LIBS = $(shell pkg-config --libs sdl2 SDL2_mixer libpng zlib)
  LIBS += -lopengl32
  CXX=g++
else
  EXE = zig
  CXXFLAGS = -O2 $(shell pkg-config --cflags sdl2 SDL2_mixer libpng zlib gl)
  LIBS = $(shell pkg-config --libs sdl2 SDL2_mixer libpng zlib gl)
endif


SRCS=$(wildcard *.cpp)
SRCS += retromat/retromat.cpp retromat/sidstyle.cpp

OBJS = $(patsubst %.cpp,%.o,$(SRCS) )

$(EXE): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(EXE) $(OBJS) $(LIBS)


.PHONY: clean
clean:
	rm -f $(OBJS) $(EXE)

.PHONY: depend
depend: .depend

.depend: $(SRCS)
	$(CXX) $(CFLAGS) -MM $^ >.depend

include .depend

