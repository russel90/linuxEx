#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>

#include <sys/shm.h>
#include <sys/types.h>

#include "shim.h"

int writeLog(char *buffer, char *filename)
{
	int numWrite, fd;
	
	fd = open(filename, O_RDWR | O_CREAT | O_APPEND, \
		S_IRWXU | S_IWGRP | S_IRUSR | S_IRGRP | S_IROTH);

	if (fd == -1) {
		printf("file open error!!\n");
		return -1;
	}

	numWrite = write(fd, buffer, strlen(buffer));
	if (numWrite == -1) {
		printf("file write error!!\n");
		return -1;
	}

	close(fd);

	return 0;
}

int main(int argc, char *argv[])
{
	key_t keyval;
	int shmid;
	int *shmaddr;
	void *shared_Mem=(void *)0;

	int i;
	char buffer[BUFSIZ];

	printf("Reader Process: Program Executed\n");
	// 옵션 배열의 요소들을 하나씩 출력
	
	printf("Reader Process: ===============================\n");
	printf("Reader Process: execl called. Reader PID = [%d]\n", getpid());
	printf("Reader Process: ===============================\n");
	printf("argc : %d\n", argc);
	for (i = 0; i < argc; i++){
		printf("Reader Process: argv[%d] = %s\n", i, argv[i]);
	}
	printf("Reader Process: ===============================\n");

	// generate shared memory
	keyval = atoi(argv[1]);
	printf("Reader Process: Shared Memory KeyVal = [%d]", keyval);
	shmid = shmget(keyval, sizeof(int)*SHMSIZE, IPC_CREAT | 0666);
	if(shmid == -1)
	{
		fprintf(stderr, "shmget failed\n");
		exit(EXIT_FAILURE);
	}

	// attached shared memeory
	shared_Mem = shmat(shmid, (void *)0, 0);
	if(shared_Mem == (void *)-1)
	{
		fprintf(stderr, "shmat failed\n");
		exit(EXIT_FAILURE);
	}

	printf("Reader Process: Generate Kill Signal to Parant Process. getppid = [%d]\n", getppid());
	kill(getppid(), SIGUSR1);

	// 공유 메모리 설정
	printf("Reader Process: Memory attached at )x%p\n", shared_Mem);
	shmaddr = (int *)shared_Mem;

	for(i = 1; i < SHMSIZE; i++){
		sprintf(buffer,"%d %d\n", i, *(shmaddr +i));
		writeLog(buffer, "log.txt");
	}

	 //detache shared memeory
	if(shmdt(shared_Mem) == -1){
		fprintf(stderr, "shmdt failed\n");
		exit(EXIT_FAILURE);
	}
	
	printf("Reader Process: Memory detached.\n");

	return 0;
}