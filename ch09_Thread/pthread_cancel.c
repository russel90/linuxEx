#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void *theThread(void *parm)
{
	int oldstate;
	printf("Thread: Entered\n");
	pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &oldstate);

	while(1){
		printf("Thread: Looping or long running request\n");
		pthread_testcancel();

		sleep(1);		
	}
	
	return NULL;
}


int main(int arg, char **argv)
{
	pthread_t thread;
	int rc = 0; 
	void *status;

	printf("Create/start a thread\n");

	rc = pthread_create(&thread, NULL, theThread, NULL);
	printf("Wait a bit until we 'realize ' the thread needs ot be canceled\n");
	sleep(3);
	rc = pthread_cancel(thread);

	printf("Wait for the thread to complete, and relases its resources\n");
	rc = pthread_join(thread, &status);

	printf("Thread status indicates it was canceled\n");
	if(status != PTHREAD_CANCELED){
		printf("Unexpected thread status\n");
	}

	return 0;
}