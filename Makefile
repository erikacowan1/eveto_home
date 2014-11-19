all: eveto

eveto.o: eveto.C
	g++ -Wall -c `root-config --cflags` eveto.C

eveto: eveto.o
	g++ -o eveto `root-config --libs` `root-config --glibs` eveto.o
