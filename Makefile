CC      = g++
CFLAGS  = -Wall -Werror $(shell root-config --cflags)
LDFLAGS = $(shell root-config --libs) $(shell root-config --glibs)

all: cbc_eveto

cbc_eveto: cbc_eveto.o eveto_main.o eveto_read_omicron.o
	$(CC) -o $@ $^ $(LDFLAGS)

cbc_eveto.o: cbc_eveto.cpp cbc_eveto.h
	$(CC) -c $(CFLAGS) $<

eveto_main.o: eveto_main.cpp cbc_eveto.h
	$(CC) -c $(CFLAGS) $<

eveto_read_omicron.o: eveto_read_omicron.cpp cbc_eveto.h
	$(CC) -c $(CFLAGS) $<

.PHONY: clean

clean:
	rm -f *.o cbc_eveto cbc_eveto.root
