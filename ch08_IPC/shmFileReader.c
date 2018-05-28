/*
* 1. Buffer Size 만큼 Shared Memory 만큼 할당하고
* 2. 파일을 오픈하여, Buffer Size 만큼 읽어 들이고
* 3. Flag가 0 이면, Buffer에 복사 함
* 4. Flag 값을 1로 변경
* 5. Reader는 Flag 값이 1이면 file을 Open하고 
* 6. Buffer Size 만큼 Shared Memory에서 읽어와서
* 7. File에 Copy 함
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
	key_t key_flag, key_data;
	int shmid_flag, shmid_data;
	int *shmaddr_flag;
	char *shmaddr_data ;
	void *shared_Mem=(void *)0;

	void *shared_flag=(void *)0;
	void *shared_data=(void *)0;

	FILE *stream;
	char *sourcefile;
	int fd;
	ssize_t read_byte; 

	int i;
	char buffer[SHMSIZE], keyflag[MAX_ID_SIZE], keydata[MAX_ID_SIZE];

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
	

	if(argc != 4 || strcmp(argv[1], "--help") == 0){
		fprintf(stderr, "Usage: %s Source Target \n",*argv);
		exit(EXIT_FAILURE);	
	}
	
	key_flag = atoi(argv[1]);
	key_data = atoi(argv[2]);
	sourcefile = argv[3];

	printf("Reader Process: Key_flag = [%d]\tKey_data = [%d]\tSource = [%s]\n", key_flag, key_data, sourcefile);
	
	shmid_flag = shmget(key_flag, sizeof(int)*SHM_FLAG_SIZE, IPC_CREAT | 0666);
	if(shmid_flag == -1)
	{
		fprintf(stderr, "Reader Process: shmget failed\n");
		exit(EXIT_FAILURE);
	}

	// attached flag shared memeory
	shared_flag= shmat(shmid_flag, (void *)0, 0);
	if(shared_flag == (void *)-1)
	{
		fprintf(stderr, "Reader Process: shmat failed\n");
		exit(EXIT_FAILURE);
	}

	shmaddr_flag = (int *)shared_flag;
	printf("Reader Process: Flag Memory attached at: %p\n", shmaddr_flag);

	// generate Data shared memory
	// key_data = ftok(DATA_PATH_ID, DATA_PROJ_ID);

	shmid_data = shmget(key_data, sizeof(char)*SHMSIZE, IPC_CREAT | 0666);
	if(shmid_data == -1)
	{
		fprintf(stderr, "Reader Process: Data shmget failed\n");
		exit(EXIT_FAILURE);
	}

	// attached data shared memeory
	shared_data = shmat(shmid_data, (void *)0, 0);
	if(shared_data == (void *)-1)
	{
		fprintf(stderr, "Reader Process: shmat failed\n");
		exit(EXIT_FAILURE);
	}

	// Set shared data address 
	shmaddr_data = (char *)shared_data;
	printf("Reader Process: Data Memory attached at )x%p\n", shmaddr_data);
	
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
		*(shmaddr_flag) = WRITE;
		*(shmaddr_flag + 1) = read_byte;

		if(read_byte == 0){
			break;
		}
		
		// printf("Parent Process: Size = %d\nBuffer = %s\n", read_byte, buffer);	
		strcpy(shmaddr_data, buffer);
	}

	if(close(fd) == -1){
		fprintf(stderr, "Reader Process: file close failed\n");
		exit(EXIT_FAILURE);		
	}

	// wating until file write.
	while(*(shmaddr_flag) != READ){
		sleep(0);
	}

	//detache shared memeory
	printf("Reader Process: flag shmdt()\n");
	if(shmdt(shared_flag) == -1){
		fprintf(stderr, "shmdt failed\n");
		exit(EXIT_FAILURE);
	}

	//detache shared memeory
	printf("Reader Process: Data shmdt()\n");
	if(shmdt(shared_data) == -1){
		fprintf(stderr, "shmdt failed\n");
		exit(EXIT_FAILURE);
	}
	
	// printf("Reader Process: Exit()\n");	
	return 0;
}