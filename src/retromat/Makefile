EXE = retromat

#CXXFLAGS = -O2
CXXFLAGS = -ggdb

CXXFLAGS += -I/usr/local/include/stk -D__LINUX_ALSA__
LIBS += -L/usr/local/lib -lstk

SRCS += main.cpp retromat.cpp sidstyle.cpp gen.cpp

#STKDIR = /home/ben/crap/stk-4.5.0
#CXXFLAGS += -I$(STKDIR)/include
#STK_SRCS = Stk.cpp OnePole.cpp ADSR.cpp FileLoop.cpp FileWvIn.cpp FileRead.cpp
#SRCS += $(addprefix $(STKDIR)/src/,$(STK_SRCS))

OBJS = $(patsubst %.cpp,%.o,$(SRCS) )
#OBJS += /usr/local/lib/libstk.so

$(EXE): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(EXE) $(OBJS) $(LIBS)

sidstyle.o: sidstyle.h

retromat.o: retromat.h sidstyle.h

main.o: retromat.h sidstyle.h





clean:
	rm $(OBJS) $(EXE)

