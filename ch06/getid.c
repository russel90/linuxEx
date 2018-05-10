#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>

int main(void)
{

	pid_t pid;
	
	sleep(5);

	printf("\n Process Id	= %d\n", pid = getpid());
	printf("Parent Process Id= %d\n", getppid());
	printf("Real User Id 	= %d\n", getuid());
	printf("Effective User Id = %d\n", geteuid());
	printf("Real Group Id	= %d\n", getgid());
	printf("Effective Group Id	=%d\n", getegid());

	return 0;
}
