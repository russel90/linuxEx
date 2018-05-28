#include <stdio.h>
#include <signal.h>
#include <stdlib.h>

// set nonzero by signal handler
volatile sig_atomic_t quitflag;

// one signal handler for SIGINT and SIGQUIT
static void sig_int(int signo)
{
		if(signo == SIGINT)
			printf("\n interrupt \n");
		else if(signo == SIGQUIT)
			// set flag for main loop
			quitflag = 1;
}

int main(void)
{
	sigset_t newmask, oldmask, zeromask;
	
	if(signal(SIGINT, sig_int) == SIG_ERR)
		perror("signal(SIGINT) error");
	if(signal(SIGQUIT, sig_int) == SIG_ERR)
		perror("signal(SIQUIT) error");

	sigemptyset(&zeromask);
	sigemptyset(&newmask);
	sigemptyset(&oldmask);
	
	// Block SIGQUIT and save current signal mask
	if(sigprocmask(SIG_BLOCK, &newmask, &oldmask) < 0)
		perror("SIG_BLOCK error");
		
	while(quitflag == 0)
		sigsuspend(&zeromask);
		
	// SIGQUIT has been caught and is now blocked; do whatever;
	quitflag = 0;
	
	// Reset signal mask which unblock SIGQUIT
	if(sigprocmask(SIG_SETMASK, &oldmask, NULL) < 0)
		perror("SIG_SETMASK error");
		
	exit(0);
	
	}
