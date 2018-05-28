#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static void sig_quit(int);

int main(void)
{
	sigset_t newmask, oldmask, pendmask;
	
	if(signal(SIGQUIT, sig_quit) == SIG_ERR)
		perror("can't catch SIGQUIT");
	
	// Block SIGQUIT and save current signal mask.
	sigemptyset(&newmask);
	sigaddset(&newmask, SIGQUIT);
	
	if(sigprocmask(SIG_BLOCK, &newmask, &oldmask) < 0)
		perror("SIG_BLOCK error");
	
	// SIGQUIT here will remain pending
	sleep(5);
	
	if(sigpending(&pendmask)< 0)
		perror("sigpending error");
	
	if(sigismember(&pendmask, SIGQUIT))
		printf("\nSIGQUIT pending\n");

	// Reset signal mask which unblock SIGQUIT
	if(sigprocmask(SIG_SETMASK, &oldmask, NULL) < 0);
		perror("SIG_SETMASK error");
	printf("SIGQUIT unblocked\n");
	
	// SIGQUIT here will termiate with core file
	sleep(5);
	exit(0);
}

static void sig_quit(int signo)
{
	
	printf("Caught SIGQUIT\n");
	if(signal(SIGQUIT, SIG_DFL) == SIG_ERR)
		perror("can't reset SIGQUIT");
}
