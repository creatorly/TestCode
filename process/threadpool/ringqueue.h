#ifndef _RINGQUEUE_H
#define _RINGQUEUE_H

#define RINGQUEUE_TRUE  1
#define RINGQUEUE_FALSE 0

typedef struct stRingQueue {
    int head; /* 头部，出队列方向*/
    int tail; /* 尾部，入队列方向*/
    int max_size ; /* 队列总尺寸 */
    int *msg; /* 队列消息 */
}stRingQueue_t;

#define print_ringq(q) printf("ring head %d,tail %d,max_size %d\n", q->head,q->tail,q->max_size);


int ringqueue_init(stRingQueue_t *ring_queue, int max_size);
int ringqueue_clean(stRingQueue_t *ring_queue);
int ringqueue_push(stRingQueue_t *ring_queue, int data);
int ringqueue_pop(stRingQueue_t *ring_queue, int *data);

#endif