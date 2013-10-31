CC = gcc
CFLAGS = -g -O2
EXE= #.exe

all: mprfgen

mprfgen$(EXE): mprfgen.o
	$(CC) mprfgen.o -o mprfgen$(EXE)

mprfgen.o: mprfgen.c
	$(CC) $(CFLAGS) -c mprfgen.c

tidy:
	rm -f *.o

clean:
	rm -f *.o mprfgen$(EXE)

