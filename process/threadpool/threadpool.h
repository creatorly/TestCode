#ifndef __THREADPOOL_H_
#define __THREADPOOL_H_

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <signal.h>
#include <assert.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#define DEFAULT_TIME 1            /*默认时间10s*/
#define MIN_WAIT_TASK_NUM 10       /*当任务数超过了它，就该添加新线程了*/
#define DEFAULT_THREAD_NUM 10      /*每次创建或销毁的线程个数*/
#define true 1
#define false 0


/*任务*/
typedef struct threadpool_task_t{
   void *(*function)(void *);
   void *arg;
}threadpool_task_t;

/*线程池管理*/
typedef struct threadpool_t{
   pthread_mutex_t lock;                 /* 锁住整个结构体 */
   pthread_mutex_t thread_counter;       /* 用于使用忙线程数时的锁 */
   pthread_cond_t  queue_not_full;       /* 条件变量，任务队列不为满 */
   pthread_cond_t  queue_not_empty;      /* 任务队列不为空 */

   pthread_t *threads;                   /* 存放线程的tid,实际上就是管理了线 数组 */
   pthread_t admin_tid;                  /* 管理者线程tid */
   threadpool_task_t *task_queue;        /* 任务队列 */

   /*线程池信息*/
   int min_thr_num;                      /* 线程池中最小线程数 */
   int max_thr_num;                      /* 线程池中最大线程数 */
   int live_thr_num;                     /* 线程池中存活的线程数 */
   int busy_thr_num;                     /* 忙线程，正在工作的线程 */
   int wait_exit_thr_num;                /* 需要销毁的线程数 */

   /*任务队列信息*/
   int queue_front;                      /* 队头 */
   int queue_rear;                       /* 队尾 */
   int queue_size; 
 
   /* 存在的任务数 */
   int queue_max_size;                   /* 队列能容纳的最大任务数 */

   /*状态*/
   int shutdown;                         /* true为关闭 */
}threadpool_t;

/*创建线程池*/
threadpool_t *threadpool_create(int min_thr_num, int max_thr_num, int queue_max_size);
/*销毁线程池*/
int threadpool_destroy(threadpool_t *pool);
/*管理线程*/
void *admin_thread(void *threadpool);
/*线程是否存在*/
//int is_thread_alive(pthread_t tid);
/*工作线程*/
void *threadpool_thread(void *threadpool);
/*向线程池的任务队列中添加一个任务*/
int threadpool_add_task(threadpool_t *pool, void *(*function)(void *arg), void *arg);

#endif
