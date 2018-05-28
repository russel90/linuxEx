#include <signal.h>
#include <stdio.h>
#include <unistd.h>

//SIGINT에 대응하는 핸틀러
void sigHandler(int sig)
{
	static int counter = 0;
	printf("OUCH! - I got signal %d\n", sig);
	
	counter++;
	if(counter == 5)
		(void) signal(SIGINT, SIG_DFL);
}

int main(void)
{
	if(signal(SIGINT, sigHandler) == SIG_ERR)
				printf("Error Occured\n");

	while(1){
		printf("Hello World!\n");
		sleep(1);
	}

	return 0;
}
