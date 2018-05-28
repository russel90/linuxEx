/*
* 1. Buffer Size ��ŭ Shared Memory ��ŭ �Ҵ��ϰ�
* 2. ������ �����Ͽ�, Buffer Size ��ŭ �о� ���̰�
* 3. Flag�� 0 �̸�, Buffer�� ���� ��
* 4. Flag ���� 1�� ����
* 5. Reader�� Flag ���� 1�̸� file�� Open�ϰ� 
* 6. Buffer Size ��ŭ Shared Memory���� �о�ͼ�
* 7. File�� Copy ��
* 
*/
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>

#include <sys/shm.h>
#include <sys/types.h>

#include "shim.h"

int main(int argc, char *argv[])
{
	key_t keyValue;
	int shmid;
	struct mesData *shmaddr;

	void *shared=(void *)0;

	FILE *stream;
	char *sourcefile;
	int fd;
	ssize_t read_byte; 

	int i;
	char buffer[SHMSIZE], keyflag[MAX_ID_SIZE];

	printf("Reader Process: Program Executed\n");
	// �ɼ� �迭�� ��ҵ��� �ϳ��� ���
	
	printf("Reader Process: ===============================\n");
	printf("Reader Process: execl called. Reader PID = [%d]\n", getpid());
	printf("Reader Process: ===============================\n");
	printf("argc : %d\n", argc);
	for (i = 0; i < argc; i++){
		printf("Reader Process: argv[%d] = %s\n", i, argv[i]);
	}
	printf("Reader Process: ===============================\n");
	

	if(argc != 3 || strcmp(argv[1], "--help") == 0){
		fprintf(stderr, "Usage: %s keyval Source Target \n",*argv);
		exit(EXIT_FAILURE);	
	}
	
	keyValue = atoi(argv[1]);
	sourcefile = argv[2];

	printf("Reader Process: keyValue = [%d]\tSource = [%s]\n", keyValue, sourcefile);
	
	shmid = shmget(keyValue, sizeof(int)*SHM_FLAG_SIZE, IPC_CREAT | 0666);
	if(shmid == -1)
	{
		fprintf(stderr, "Reader Process: shmget failed\n");
		exit(EXIT_FAILURE);
	}

	// attached flag shared memeory
	shared= shmat(shmid, (void *)0, 0);
	if(shared == (void *)-1)
	{
		fprintf(stderr, "Reader Process: shmat failed\n");
		exit(EXIT_FAILURE);
	}

	shmaddr = (struct mesData *)shared;
	printf("Reader Process: Flag Memory attached at: %p\n", shmaddr);
	
	// File Read
    fd = open(sourcefile, O_RDONLY);
	if(fd == -1){
		fprintf(stderr, "Reader Process: shmat failed\n");
		exit(EXIT_FAILURE);	
	}
	
	while(1){
		
		// 1 - Read Privilege, 0 - Write - Privilege
		while(*(shmaddr_flag) != READ){
			sleep(0);
		}

		memset(buffer, 0, sizeof(buffer));
		read_byte = read(fd, buffer, sizeof(buffer));
		shmaddr->flags = WRITE;
		shmaddr->read_byte = read_byte;
		
		if(read_byte == 0){
			break;
		}
		strcpy(shmaddr->file_stream, buffer);
	}

	if(close(fd) == -1){
		fprintf(stderr, "Reader Process: file close failed\n");
		exit(EXIT_FAILURE);		
	}

	// wating until file write.
	while(shmaddr->flags != READ){
		sleep(0);
	}

	//detache shared memeory
	printf("Reader Process: flag shmdt()\n");
	if(shmdt(shared) == -1){
		fprintf(stderr, "shmdt failed\n");
		exit(EXIT_FAILURE);
	}

	// printf("Reader Process: Exit()\n");	
	return 0;
}