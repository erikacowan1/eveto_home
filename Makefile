CC      = g++
CFLAGS  = -Wall -Werror $(shell root-config --cflags)
LDFLAGS = $(shell root-config --libs) $(shell root-config --glibs)

all: cbc_eveto

cbc_eveto: cbc_eveto.o cbc_eveto_main.o cbc_eveto_read_omicron.o cbc_eveto_read_cbc_triggers.o
	$(CC) -o $@ $^ $(LDFLAGS)

cbc_eveto.o: cbc_eveto.cpp cbc_eveto.h
	$(CC) -c $(CFLAGS) $<

cbc_eveto_main.o: cbc_eveto_main.cpp cbc_eveto.h
	$(CC) -c $(CFLAGS) $<

cbc_eveto_read_omicron.o: cbc_eveto_read_omicron.cpp cbc_eveto.h
	$(CC) -c $(CFLAGS) $<

cbc_eveto_read_cbc_triggers.o: cbc_eveto_read_cbc_triggers.cpp cbc_eveto.h
	$(CC) -c $(CFLAGS) $<

calc_dumb_sig.o: calc_dumb_sig.cpp cbc_eveto.h

.PHONY: clean

clean:
	rm -f *.o cbc_eveto cbc_eveto.root
