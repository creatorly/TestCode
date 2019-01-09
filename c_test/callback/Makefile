CC = gcc

all:$(subst .c,.o,$(wildcard *.c))
 
%.o:%.c
	$(CC) -o $@ $<

clean:
	rm -rf *.o
