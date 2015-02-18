#ARCH := $(firstword $(shell uname -m))	# x86_64, i386, etc..
#SYS := $(firstword $(shell uname -s))	# Linux, Darwin, etc...

# which platform? linux, win32 or osx
PLATFORM=$(shell ./zig-platform)

EXE = zig

SRCS=$(wildcard *.cpp)
SRCS += retromat/retromat.cpp retromat/sidstyle.cpp

OBJS = $(patsubst %.cpp,%.o,$(SRCS) )

ifeq ($(PLATFORM),linux)
  CXXFLAGS = -std=c++0x -O2 $(shell pkg-config --cflags sdl2 SDL2_mixer libpng zlib gl)
  LIBS = $(shell pkg-config --libs sdl2 SDL2_mixer libpng zlib gl)
endif
ifeq ($(PLATFORM),osx)
  CXXFLAGS = -O2 $(shell pkg-config --cflags sdl2 SDL2_mixer libpng)
  LIBS = $(shell pkg-config --libs sdl2 SDL2_mixer libpng)
  LIBS += -lz -framework OpenGL -framework Foundation
  OBJS += osx_glue.o
endif
ifeq ($(PLATFORM),win32)
  EXE = zig.exe
  CXXFLAGS = -O2 $(shell pkg-config --cflags sdl2 SDL2_mixer libpng zlib)
  LIBS = $(shell pkg-config --libs sdl2 SDL2_mixer libpng zlib)
  LIBS += -lopengl32
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

.depend: $(SRCS)
	$(CXX) $(CXXFLAGS) -MM $^ >.depend

include .depend

