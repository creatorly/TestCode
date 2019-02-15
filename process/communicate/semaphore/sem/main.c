#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include "ut_sem.h"

#define BUFFER_SIZE 2

typedef struct cons
{
    int buffer[BUFFER_SIZE];
	int readpos;
	int writepos;
	SEM_ID notempty;
	SEM_ID notfull;
}CONS, *PCONS;

CONS stCons;

static void init(void);
static void put(int data);
static int get(void);
static void *producer(void *data);
static void *consumer(void *data);

static void init(void)
{
    PCONS pCons;

	pCons = (PCONS)&stCons;

	pCons->readpos = 0;
	pCons->writepos = 0;
    pCons->notempty = semBCreate(SEM_Q_FIFO, SEM_EMPTY);
	pCons->notfull = semBCreate(SEM_Q_FIFO, SEM_EMPTY);
	printf("init OK\n");
}

static void put(int data)
{
   PCONS pCons;

   pCons = (PCONS)&stCons;

   while((pCons->writepos + 1) % BUFFER_SIZE == pCons->readpos)
   {
       printf("produces wait for not full\n");
       semTake(pCons->notfull, 5000);
   }

   pCons->buffer[pCons->writepos] = data;
   pCons->writepos++;
   if(pCons->writepos >= BUFFER_SIZE)
        pCons->writepos = 0;

   semGive(pCons->notempty);
}

static int get(void)
{
    int data;
    PCONS pCons;

    pCons = (PCONS)&stCons;

    while(pCons->writepos == pCons->readpos)
    {
        printf("consumer wait for not empty\n");
        semTake(pCons->notempty, 5000);
    }

    data = pCons->buffer[pCons->readpos];
    pCons->readpos++;
    if(pCons->readpos >= BUFFER_SIZE)
         pCons->readpos = 0;

    semGive(pCons->notfull);

	return data;
}

static void *producer(void *data)
{
    int n;;

	for(n=1;n<=5;n++)
	{
	    printf("producer sleep 1 sencond...\n");
		sleep(1);
	    printf("put the %d producer\n", n);
		put(n);
	}

	for(n=6;n<=10;n++)
	{
	    printf("producer sleep 3 sencond...\n");
		sleep(3);
	    printf("put the %d producer\n", n);
		put(n);
	}
	put(-1);
	printf("producer stopped!\n");
	return NULL;
}

static void *consumer(void *data)
{
    int d = 0;

	while(1)
	{
	    printf("consumer sleep 2 sencond...\n");
		sleep(2);
		d = get();
		printf("get the %d product\n", d);
		if(d == -1)
		    break;
	}
	printf("consumer stopped!\n");
	return NULL;
}

int main(int argc, char* argv[])
{
    pthread_t th_a, th_b;
    void *ret;

    init();
	pthread_create(&th_a, NULL, producer, 0);
	pthread_create(&th_b, NULL, consumer, 0);
	/* Wait until producer and consumer finish */
	pthread_join(th_a, &ret);
	pthread_join(th_b, &ret);

    return 0;
}

