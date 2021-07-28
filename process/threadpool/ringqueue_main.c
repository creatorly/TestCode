
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ringqueue.h"

int main(int argc, char* argv[])
{
    stRingQueue_t my_ringq;
    int in_data;
    int out_data;

    printf("start ringqueue test\r\n");

    ringqueue_init(&my_ringq, 5);

    in_data = 1;
    if(ringqueue_push(&my_ringq, in_data)) {
        printf("push 1 ok\r\n");
    }
    else {
        printf("push 1 fail\r\n");
    }

    in_data = 2;
    if(ringqueue_push(&my_ringq, in_data)) {
        printf("push 2 ok\r\n");
    }
    else {
        printf("push 2 fail\r\n");
    }

    if(ringqueue_pop(&my_ringq, &out_data)) {
        printf("pop ok\r\n");
        printf("out_data:%d\r\n", out_data);
    }
    else {
        printf("pop fail\r\n");
    }
    
    in_data = 3;
    if(ringqueue_push(&my_ringq, in_data)) {
        printf("push 3 ok\r\n");
    }
    else {
        printf("push 3 fail\r\n");
    }

    in_data = 4;
    if(ringqueue_push(&my_ringq, in_data)) {
        printf("push 4 ok\r\n");
    }
    else {
        printf("push 4 fail\r\n");
    }

    in_data = 5;
    if(ringqueue_push(&my_ringq, in_data)) {
        printf("push 5 ok\r\n");
    }
    else {
        printf("push 5 fail\r\n");
    }

    in_data = 6;
    if(ringqueue_push(&my_ringq, in_data)) {
        printf("push 6 ok\r\n");
    }
    else {
        printf("push 6 fail\r\n");
    }

    in_data = 7;
    if(ringqueue_push(&my_ringq, in_data)) {
        printf("push 7 ok\r\n");
    }
    else {
        printf("push 7 fail\r\n");
    }

    if(ringqueue_pop(&my_ringq, &out_data)) {
        printf("pop ok\r\n");
        printf("out_data:%d\r\n", out_data);
    }
    else {
        printf("pop fail\r\n");
    }

    if(ringqueue_pop(&my_ringq, &out_data)) {
        printf("pop ok\r\n");
        printf("out_data:%d\r\n", out_data);
    }
    else {
        printf("pop fail\r\n");
    }

    if(ringqueue_pop(&my_ringq, &out_data)) {
        printf("pop ok\r\n");
        printf("out_data:%d\r\n", out_data);
    }
    else {
        printf("pop fail\r\n");
    }

    if(ringqueue_pop(&my_ringq, &out_data)) {
        printf("pop ok\r\n");
        printf("out_data:%d\r\n", out_data);
    }
    else {
        printf("pop fail\r\n");
    }

    if(ringqueue_pop(&my_ringq, &out_data)) {
        printf("pop ok\r\n");
        printf("out_data:%d\r\n", out_data);
    }
    else {
        printf("pop fail\r\n");
    }

    in_data = 8;
    if(ringqueue_push(&my_ringq, in_data)) {
        printf("push 8 ok\r\n");
    }
    else {
        printf("push 8 fail\r\n");
    }

    ringqueue_clean(&my_ringq);
    printf("end ringqueue test\r\n");

    return 1;
}