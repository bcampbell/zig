
CC = g++

PROGRAM = retromat

OBJECTS = sidstyle.o retromat.o main.o
#STKPATH = /home/ben/working/stk-4.1.3
#STKPATH = /home/ben/working/stk-4.2.0

CFLAGS = -O4 -D__LINUX_OSS__ -D__LITTLE_ENDIAN__ -I/usr/include/stk

LIBS = -lstk -lpthread


all:    $(PROGRAM)


.SUFFIXES:	.o .cpp

.cpp.o :
	$(CC) -c $(CFLAGS) -o $@ $<

$(PROGRAM):	$(RAWWAVES) $(OBJECTS)
	$(CC) -o $(PROGRAM) $(OBJECTS) $(LIBS)

clean:
	rm -f *.o $(PROGRAM)

