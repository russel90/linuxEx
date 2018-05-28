#include<stdio.h>
#include<signal.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#include<fcntl.h>
#include<sys/stat.h>
#include<time.h>

void MixArray(int n,int*Array){
	int numMix1=0,numMix2=0;
	int i=0,temp=0;;

	for(i=0;i<1000;i++){
		numMix1 = rand()%n;
		numMix2 = rand()%n;

		
		temp=Array[numMix1];
		Array[numMix1]=Array[numMix2];
		Array[numMix2]=temp;
	}
}
int main(int argc,char* argv[]){
	pid_t pid;
	int fd,byteCount;
	int a=0;
	int value[3] ={SIGINT,SIGUSR1,SIGUSR2};
	pid=atoi(argv[1]);
	
	printf("%d",pid);
	srand(time(NULL));
	
	while(a<10){
		sleep(1);
		MixArray(3,value);
		kill(pid,value[0]);
		printf("%d, %d \n",value[0],pid);
		a++;
	}
	
	return 0;
}
