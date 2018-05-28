/*
 *	convert sigprocmask2 to sigaction fucntion
 *		1. check sigaction function:
 * 		int sigaction(int sig, const struct sigaction *act, struct sigaction *oldact);
 * 	2. struct sigaction
 * 			struct sigaction{
 * 				void (* sa_handler)(int); // handler address
 * 				sigset_t sa_mask;					//  핸들러 실행 동안 블록 된 시그널
 * 				int 		sa_flags;					// Handler  실행을 제어하는 플래그
 * 				void (* sa_restorer)(void); //  응용 프로그램 사용을 위한 것은 아님
 * 			} 
 * 
 * 	3. References: https://github.com/venture21/linuxEx/blob/master/sigprocmask2.c
 * */

#include<signal.h>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>

static void sig_quit(int signo){
	printf("caught SIGQUIT\n");
	if(signal(SIGQUIT,SIG_DFL)==SIG_ERR)
			perror("can't reset SIGQUIT");
}

static void sigHandler(int signo){
	printf("signal no : %d\n",signo);
}
int main(void){
	
	struct sigaction	newAct, oldAct;
	sigset_t newmask,oldmask,pendmask;
	
	newAct.sa_handler = sigHandler;	
	sigemptyset(&newAct.sa_mask);
	newAct.sa_flags = 0;
		
	if(sigaction(SIGQUIT, &newAct, &oldAct) == -1)
		perror("can't catch SIGQUIT");
	if(sigaction(SIGINT, &newAct, NULL) == -1)
		perror("can't catch SIGINT");

/*
	if(signal(SIGQUIT, sig_quit)==SIG_ERR)
			perror("can't catch SIGQUIT");
	if(signal(SIGINT,sigHandler)==SIG_ERR)
			perror("can't catch SIGQUIT");
*/

	sigemptyset(&newmask);
	sigaddset(&newmask,SIGQUIT);
	if(sigprocmask(SIG_BLOCK,&newmask,&oldmask)<0)
			perror("SIG_BLOCK error");
	printf("SIGQUIT is blocked\n");
	sleep(10);

	if(sigpending(&pendmask)<0)
			perror("sigpending error");
	
	if(sigismember(&pendmask,SIGQUIT))
			printf("\nSIGQUIT pending\n");

	if(sigprocmask(SIG_SETMASK, &oldAct.sa_mask, NULL)<0)
			perror("SIG_SETMASK error");
	printf("SIGQUIT unblocked\n");

	sleep(5);
	exit(0);
}

