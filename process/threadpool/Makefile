CC = gcc
RINGQUEUE         = ringqueue
THREADPOOL        = threadpool

CC_LDFLAGS = $(LDFLAGS) -L. -lm -lrt -lpthread
CC_CFLAGS  = $(CFLAGS) -fPIC -Wall -Werror -std=gnu99 -Os

all: $(RINGQUEUE) $(THREADPOOL)

$(RINGQUEUE) : ringqueue_main.o ringqueue.o
	${CC} $^ -o $@ ${CC_LDFLAGS}

ringqueue_main.o : ringqueue_main.c
	${CC} -c $< -o $@ ${CC_CFLAGS}

ringqueue.o : ringqueue.c ringqueue.h
	${CC} -c $< -o $@ ${CC_CFLAGS}


$(THREADPOOL) : threadpool_main.o threadpool.o
	${CC} $^ -o $@ ${CC_LDFLAGS}

threadpool_main.o : threadpool_main.c
	${CC} -c $< -o $@ ${CC_CFLAGS}

threadpool.o : threadpool.c threadpool.h
	${CC} -c $< -o $@ ${CC_CFLAGS}

.PHONY : clean
clean:
	-rm -f *.o  $(RINGQUEUE) $(THREADPOOL)
