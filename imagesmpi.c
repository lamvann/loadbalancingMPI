/*
	
	This is an affirmation of originality.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char ** argv) {
	int fd[2];
	pid_t pid;
	pipe(fd);
	pid = fork();

	switch(pid){
		case 0: { /*	Child 1		*/		
			execlp("mkdir","mkdir","results", (int*)0);	
			break;
		}
		case -1:{
			perror("fork failed");
			exit(-1);
		}
		default: { /*	Parent	*/
			
			break;
		}
	}
	return 0;
}