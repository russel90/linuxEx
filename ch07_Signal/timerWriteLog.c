#include<stdio.h>
#include<signal.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#include<fcntl.h>
#include<sys/stat.h>
#include<time.h>

int fd;

void sigHandler(int sig){
	int byteCount;
	char buffer[255];
	time_t current_time;
	struct tm *tm;
	time(&current_time);


	printf("take a signal [%d]",sig);
	//sprintf(buffer,"%s //%d \n",ctime(&current_time),sig);
	tm=localtime(&current_time);
	sprintf(buffer,"time = %s \n sig=[%d]\n",asctime(tm),sig);
	byteCount=write(fd,buffer,strlen(buffer));
	printf("byteCount=	[%d]\n",byteCount);
}

int main(void){
	
	int a=0;
	
	signal(SIGUSR1,sigHandler);
	signal(SIGQUIT,sigHandler);
	signal(SIGINT,sigHandler);
	
	fd=open("./log.txt",O_RDWR|O_CREAT|O_TRUNC,\
					 S_IRWXU|S_IWGRP|S_IRUSR|S_IRGRP|S_IROTH);

	while(a<10){
			pause();
			a++;
	}
	
	close(fd);
	pause();
	
	return 0;
}
