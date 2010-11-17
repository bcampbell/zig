RAWWAVES = sine.raw triangle.raw square.raw sawtooth.raw

all:    $(RAWWAVES)

$(RAWWAVES): makewaves
	./makewaves

makewaves: makewaves.c
	gcc makewaves.c -o makewaves -lm

clean:
	rm -f *.o $(RAWWAVES) makewaves

