#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

// external variable in initialized data
int glob = 6;
char buf[] = "a write to stdout\n";

int main(void)
{
	// automatic variable on the stack
	int var;
	pid_t pid;
	var = 88;


	if(write(STDOUT_FILENO, buf, sizeof(buf) - 1) != sizeof(buf) - 1)
		perror("write error");

	// We don't flush stdout
	printf("before fork\n");

	if((pid = fork()) == 0){
		// child
		glob++;
		var++;
	}else{
		// parent
		sleep(2);
	}

	printf("[pid = %d,\tglob = %d, var = %d]\n", getpid(), glob, var);

	exit(0);
}
