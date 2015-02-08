#ARCH := $(firstword $(shell uname -m))	# x86_64, i386, etc..
#SYS := $(firstword $(shell uname -s))	# Linux, Darwin, etc...

# which platform? linux, win32 or osx
PLATFORM=$(shell ./zig-platform)

EXE = zig

ifeq ($(PLATFORM),linux)
  CXXFLAGS = -O2 $(shell pkg-config --cflags sdl2 SDL2_mixer libpng zlib gl)
  LIBS = $(shell pkg-config --libs sdl2 SDL2_mixer libpng zlib gl)
endif
ifeq ($(PLATFORM),osx)
  CXXFLAGS = -O2 $(shell pkg-config --cflags sdl2 SDL2_mixer libpng zlib gl)
  LIBS = $(shell pkg-config --libs sdl2 SDL2_mixer libpng zlib gl)
endif
ifeq ($(PLATFORM),win32)
  EXE = zig.exe
  CXXFLAGS = -O2 $(shell pkg-config --cflags sdl2 SDL2_mixer libpng zlib)
  LIBS = $(shell pkg-config --libs sdl2 SDL2_mixer libpng zlib)
  LIBS += -lopengl32
  CXX=g++
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

