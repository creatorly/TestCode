
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "threadpool.h"

int main(int argc, char* argv[])
{

    printf("start threadpool test\r\n");
    /* 线程池初始化，其管理者线程及工作线程都会启动 */
    threadpool_t *thp = threadpool_create(10, 100, 100);
    printf("threadpool init ... ... \n");

    /* 接收到任务后添加 */
    //threadpool_add_task(thp, do_work, (void *)p);

    /* 销毁 */
    threadpool_destroy(thp);
    
    printf("end threadpool test\r\n");

    return 1;
}