CC = gcc
source=$(wildcard *.c)
objects=$(source:.c=.o)

all:$(objects)

%.o:%.c
	$(CC) -o $@ $<

clean:
	rm *.o
