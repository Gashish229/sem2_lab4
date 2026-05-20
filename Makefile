CC = gcc
CFLAGS = -Wall -std=c99

all: cmdcalc

cmdcalc: cmdcalc.o stack.o
	$(CC) $(CFLAGS) -o cmdcalc cmdcalc.o stack.o

cmdcalc.o: cmdcalc.c stack.h
	$(CC) $(CFLAGS) -c cmdcalc.c

stack.o: stack.c stack.h
	$(CC) $(CFLAGS) -c stack.c

clean:
	rm -f *.o cmdcalc
