CC=gcc
CFLAGS=-Wall
P1_OBJECTS = p1.o gfunctions.o  sh_mem.c
P2_OBJECTS = p2.o gfunctions.o  sh_mem.c
ENC1_OBJECTS = enc1.o gfunctions.o sh_mem.c 
CHAN_OBJECTS = chan.o gfunctions.o sh_mem.c
ENC2_OBJECTS = enc2.o gfunctions.o sh_mem.c
PLUS= -g -Og -lcrypto

all: p1 p2 enc1 chan enc2

p1		: $(P1_OBJECTS)
	$(CC) $(CFLAGS) $(P1_OBJECTS) $(PLUS) -o p1

p2		: $(P2_OBJECTS)
	$(CC) $(CFLAGS) $(P2_OBJECTS) $(PLUS) -o p2

enc1	: $(ENC1_OBJECTS)
	$(CC) $(CFLAGS) $(ENC1_OBJECTS) $(PLUS) -o enc1

chan	: $(CHAN_OBJECTS)
	$(CC) $(CFLAGS) $(CHAN_OBJECTS) $(PLUS) -o chan

enc2	: $(ENC2_OBJECTS)
	$(CC) $(CFLAGS) $(ENC2_OBJECTS) $(PLUS) -o enc2	

p1.o   		: p1.c
	$(CC) $(CFLAGS) -c p1.c -o p1.o

p2.o   		: p2.c
	$(CC) $(CFLAGS) -c p2.c -o p2.o

enc1.o   		: enc1.c
	$(CC) $(CFLAGS) -c enc1.c -o enc1.o

chan.o   		: chan.c
	$(CC) $(CFLAGS) -c chan.c -o chan.o

enc2.o   		: enc2.c
	$(CC) $(CFLAGS) -c enc2.c -o enc2.o

#addons functions

sh_mem.o   		: sh_mem.c
	$(CC) $(CFLAGS) -c sh_mem.c -o sh_mem.o

gfunctions.o   		: gfunctions.c
	$(CC) $(CFLAGS) -c gfunctions.c -o gfunctions.o
.PHONY: clean

clean:
	rm -f p1 p2 enc1 chan enc2 p1.o p2.o enc1.o chan.o enc2.o gfunctions.o sh_mem.o
