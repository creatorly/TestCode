#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <math.h>
#include <signal.h>

void my_func(int sign_no)
{
	printf("I have get SIGBUS %d\n",sign_no);
}

int main(int argc, char *argv[])
{
	signal(SIGBUS, my_func);
	while(1)
	{
		printf("Hello World!\n");
		sleep(1);
	}
	return 0;
}
