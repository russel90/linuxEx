#include <stdio.h>
#include <unistd.h>

int global = 10;

int main(void)
{
	int i = 20;
	pid_t pid;
	int status;

	if((pid = fork()) == 0){
		// child process
		global = global + 10;
		i = i + 10;
	}else{
		// fork() returns a pid != 0
		// parent process 
		global += 100;
		i += 100;
	}

	printf("[global = %d];\t[i = %d]\n",global, i);
	
	return 0;

}
