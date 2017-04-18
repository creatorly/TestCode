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
	pid_t pid;
	int pipe_fd[2];
	char buf_r[100];
	char *p_wbuf;
	int r_num;

	memset(buf_r, 0, sizeof(buf_r));

	if(pipe(pipe_fd) < 0)
	{
		printf("pipe create error\n");
		return -1;
	}

	if((pid = fork()) < 0)
	{
		printf("fork create error\n");
		return -1;
	}

	//child process
	else if(pid == 0)
	{
		printf("\n");
		close(pipe_fd[1]);
		sleep(2);

		if((r_num = read(pipe_fd[0], buf_r, 100)) > 0)
			printf("%d numbers child read from the pipe is %s\n", r_num, buf_r);

		close(pipe_fd[0]);
		exit(0);
	}
	//create parent process
	else if(pid > 0)
	{
		close(pipe_fd[0]);
		if(write(pipe_fd[1], "Hello", 5) != -1)
			printf("parent write1 Hello\n");
		if(write(pipe_fd[1], "Pipe", 5) != -1)
			printf("parent write2 Pipe\n");

		close(pipe_fd[1]);
		waitpid(pid, NULL, 0);
		exit(0);
	}
	return 0;
}
