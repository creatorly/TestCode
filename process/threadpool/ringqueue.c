/*
环形队列广泛用于网络数据收发，和不同程序间数据交换（比如内核与应用程序大量交换数据,从硬件接收大量数据）均使用了环形队列。
为了方便读写，还要用数组下标来指明队列的读写位置。head/tail.其中head指向可以读的位置，tail指向可以写的位置。
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ringqueue.h"

/* 当tail + 1就追上head时，队列为满时 */
static int ringqueue_is_full(stRingQueue_t *ring_queue)
{
    return (ring_queue->head == ((ring_queue->tail + 1) % ring_queue->max_size)) ? 1:0;
}

/* 当head追上tail时，队列为空 */
static int ringqueue_is_empty(stRingQueue_t *ring_queue)
{
    return (ring_queue->tail == ring_queue->head) ? 1:0;
}

int ringqueue_init(stRingQueue_t *ring_queue, int max_size)
{
    ring_queue->msg = (int*) malloc(max_size * sizeof(int));
    if(ring_queue != NULL) {
        ring_queue->max_size = max_size;
        ring_queue->head = 0;
        ring_queue->tail = 0;
        print_ringq(ring_queue);
    }

    return RINGQUEUE_TRUE;
}

int ringqueue_clean(stRingQueue_t *ring_queue)
{
    if(ring_queue->msg != NULL){
        free(ring_queue->msg);
        ring_queue->max_size = 0;
        ring_queue->head = 0;
        ring_queue->tail = 0;
        print_ringq(ring_queue);
    }

    return RINGQUEUE_TRUE;
}

int ringqueue_push(stRingQueue_t *ring_queue, int data)
{
    if(ringqueue_is_full(ring_queue)) {
        printf("ringqueue_is_full\r\n");
        return RINGQUEUE_FALSE;
    }

    ring_queue->msg[ring_queue->tail] = data;
    ring_queue->tail = (ring_queue->tail + 1) % ring_queue->max_size;

    print_ringq(ring_queue);
    return RINGQUEUE_TRUE;
}

int ringqueue_pop(stRingQueue_t *ring_queue, int *data)
{
    if(ringqueue_is_empty(ring_queue)) {
        return RINGQUEUE_FALSE;
    }

    *data = ring_queue->msg[ring_queue->head];
    ring_queue->head = (ring_queue->head + 1) % ring_queue->max_size;

    print_ringq(ring_queue);
    return RINGQUEUE_TRUE;
}