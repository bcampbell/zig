uname_O := $(shell sh -c 'uname -o 2>/dev/null || echo not')

#EXE = zig.exe
EXE = zig

CXXFLAGS = -O2 $(shell pkg-config --cflags sdl2 SDL2_mixer libpng zlib gl)
CXXFLAGS += -I/usr/local/include/stk
LIBS = $(shell pkg-config --libs sdl2 SDL2_mixer libpng zlib gl)
#LIBS += -lopengl32
LIBS += -L/usr/local/lib -lstk

SRCS=$(wildcard *.cpp)
SRCS += retromat/retromat.cpp retromat/sidstyle.cpp

OBJS = $(patsubst %.cpp,%.o,$(SRCS) )
#OBJS += /usr/local/lib/libstk.so

$(EXE): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(EXE) $(OBJS) $(LIBS)



clean:
	rm $(OBJS) $(EXE)

