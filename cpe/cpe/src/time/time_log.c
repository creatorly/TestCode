#include <stdio.h>
#include <string.h>
#include <time.h>

#include "time_log.h"

int main(void){
	printf("the current time : %s|test format.\n",current_time());
	return 0;
}

char* current_time(){
    time_t ltime;
    time(&ltime);

    char *result;
    result = ctime(&ltime);
    result[strlen(result)-1] = 0; // 去掉换行符
    printf("get current time : %s\n", result);
    return result;
}
