CC=gcc
CFLAGS=-Wall -std=c11

all:
	$(CC) $(CFLAGS) main.c bitstruct.c matrix2d.c contqueue.c -o main -lm

clean:
	rm -f main *.bin
