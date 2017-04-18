#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <math.h>

int main(int argc, char *argv[])
{
	if(argc < 2)
	{
		perror("you haven't input the filename, please try again!\n");
		exit(EXIT_FAILURE);
	}

	if(execl("./file_create", "file_creat", argv[1],NULL) < 0)
		perror("execl error!\n");
	
	return 0;
}
