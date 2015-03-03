CC      = g++
CFLAGS  = -Wall -Werror $(shell root-config --cflags)
LDFLAGS = $(shell root-config --libs) $(shell root-config --glibs)

all: eveto

eveto: eveto.o
	$(CC) -o $@ $^ $(LDFLAGS)

eveto.o: eveto.C eveto.h
	$(CC) -c $(CFLAGS) $<

.PHONY: clean cleanest

clean:
	rm *.o

cleanest: clean
	rm eveto
