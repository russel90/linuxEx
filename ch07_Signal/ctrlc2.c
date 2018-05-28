
#include <signal.h>
#include <stdio.h>
#include <unistd.h>

void ouch(int sig)
{
		printf("ouch! - I got signal %d\n", sig);
}

int main(void)
{
	struct sigaction act;
	act.sa_handler = ouch;
	sigemptyset(&act.sa_mask);
	
	act.sa_flags = 0;
	// 그 이전 sigaction 값을 백업 하지 않겠다.
	sigaction(SIGINT, &act, 0);
	
	while(1){
			printf("Hello World!\n");
			sleep(1);
	}

	return 0;
}
