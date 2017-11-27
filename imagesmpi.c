#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/wait.h>


int main(int argc, char const *argv[])
{
	int fd[2];
	pid_t pid;
	pipe(fd);
	pid = fork();

	switch(pid){
		case 0:{

		}
		case -1:{
			perror("fork failed");
			exit(-1);
		}
	}
	return 0;
}