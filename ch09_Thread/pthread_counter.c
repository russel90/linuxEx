#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

static int count = 0;
static pthread_mutex_t countlock = PTHREAD_MUTEX_INITIALIZER;

void *t_increment(void *data){
	int error;
	
	while(1){
		if(error = pthread_mutex_lock(&countlock)){
			return (void *) error;
		}
		count++;

		if(error = pthread_mutex_unlock(&countlock)){
			return (void *) error;
		}
		usleep(1000);
	}

	return (void *) error; 
}

void *t_decrement(void *data)
{
	int error;
	while(1){
		if(error = pthread_mutex_lock(&countlock)){
			return (void *) error;
		}

		count--;

		if(error = pthread_mutex_unlock(&countlock)){
			return (void *) error;
		}

		usleep(1000);
	} 
	return (void *) error;
}

void *t_getcount(void *data)
{
	int error;

	if(error = pthread_mutex_lock(&countlock))
		return (void *)error;

	if(error = pthread_mutex_unlock(&countlock))
		return (void *) error;
	
	while(1){
		usleep(1000);
		printf("get counter = %d\n", count);
	}

	return (void *) error;
}

int main(void)
{

	pthread_t p_thread[3];
	int err;
	int status;
	int a =0;

	printf("Before pthread_create()\t pid = %d, count = %d\n", getpid(), count);

	if((err = pthread_create(&p_thread[0], NULL, t_increment, (void *)&a)) < 0){
		perror("thread increment create error: ");
		exit(1);
	}

	if((err = pthread_create(&p_thread[1], NULL, t_decrement, (void *)&a)) < 0){

		perror("thread decrement create error: ");
		exit(1);
	}

	if((err = pthread_create(&p_thread[2], NULL, t_getcount, (void *)&a)) < 0){
		perror("thread getcount create error: ");
		exit(1);
	}


	err = pthread_detach(p_thread[0]);
	err = pthread_detach(p_thread[1]);
	err = pthread_detach(p_thread[2]);

	pause();
}
