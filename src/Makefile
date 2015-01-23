#uname_O := $(shell sh -c 'uname -o 2>/dev/null || echo not')
ARCH := $(firstword $(shell uname -m))	# x86_64, i386, etc..
SYS := $(firstword $(shell uname -s))	# Linux, Darwin, etc...

#include ${SYS}.${ARCH}.mk

#EXE = zig.exe
EXE = zig

CXXFLAGS = -O2 $(shell pkg-config --cflags sdl2 SDL2_mixer libpng zlib gl)
LIBS = $(shell pkg-config --libs sdl2 SDL2_mixer libpng zlib gl)
#LIBS += -lopengl32

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
	$(CC) $(CFLAGS) -MM $^ >.depend

include .depend

