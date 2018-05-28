/*
* 1. Buffer Size 만큼 Shared Memory 만큼 할당하고
* 2. 파일을 오픈하여, Buffer Size 만큼 읽어 들이고
* 3. Flag가 0 이면, Buffer에 복사 함
* 4. Flag 값을 1로 변경
* 5. Reader는 Flag 값이 1이면 file을 Open하고 
* 6. Buffer Size 만큼 Shared Memory에서 읽어와서
* 7. File에 Copy 함* 
*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include "shim.h"

int main(int argc, char *argv[])
{
	int status;
	int i;
	// pid_t parentPID, childPID;

	int fd;
	ssize_t write_byte;
	
	key_t key_flag, key_data ;
	void *shared_flag=(void *)0;
	void *shared_data=(void *)0;
	int shmid_flag, shmid_data;
	int *shmaddr_flag; 
	char *shmaddr_data;

	char buffer[SHMSIZE], keyflag[MAX_ID_SIZE], keydata[MAX_ID_SIZE];
	char *sourcefile, *targetfile;

	printf("argc = %d\n",argc);

	if(argc != 3 || strcmp(argv[1], "--help") == 0){
		fprintf(stderr, "Usage: %s Source Target \n",*argv);
		exit(EXIT_FAILURE);	
	}

	sourcefile = argv[1];
	targetfile = argv[2];

	printf("Parent Process: Source = [%s]\tTarget = [%s]\n", sourcefile, targetfile);

	// generate flag shared memory
	key_flag = ftok(STRING, PROJ_ID);
	shmid_flag = shmget(key_flag, sizeof(int)*SHM_FLAG_SIZE, IPC_CREAT | 0666);
	if(shmid_flag == -1)
	{
		fprintf(stderr, "shmget failed\n");
		exit(EXIT_FAILURE);
	}

	// attached flag shared memeory
	shared_flag = shmat(shmid_flag, (void *)0, 0);
	if(shared_flag == (void *)-1)
	{
		fprintf(stderr, "shmat failed\n");
		exit(EXIT_FAILURE);
	}

	shmaddr_flag = (int *)shared_flag;
	printf("Flag Memory attached at )x%p\n", shmaddr_flag);

	// generate Data shared memory
	key_data = ftok(DATA_PATH_ID, DATA_PROJ_ID);
	shmid_data = shmget(key_data, sizeof(char)*SHMSIZE, IPC_CREAT | 0666);
	if(shmid_data == -1)
	{
		fprintf(stderr, "Data shmget failed\n");
		exit(EXIT_FAILURE);
	}

	// attached data shared memeory
	shared_data = shmat(shmid_data, (void *)0, 0);
	if(shared_data == (void *)-1)
	{
		fprintf(stderr, "shmat failed\n");
		exit(EXIT_FAILURE);
	}

	shmaddr_data = (char *)shared_data;
	printf("Data Memory attached at )x%p\n", shmaddr_data);

	// childPID = fork();
	switch(fork()){
		case -1:
			fprintf(stderr, "shmget failed\n");
			exit(EXIT_FAILURE);

		case 0:
			// childPID = getpid();
			printf("===============================\n");
			printf("Chil Procee: Call Child Process\n");

		    sprintf(keyflag, "%d", key_flag);
			sprintf(keydata, "%d", key_data);
			printf("Chil Procee: Called Execl\n");
			printf("Child Process: Key_flag = [%d]\tKey_data = [%d]\n", key_flag, key_data);
			execl("./shmFileReader", "./shmFileReader", keyflag, keydata, sourcefile ,NULL);
			fprintf(stderr, "execl failed\n");
			exit(EXIT_FAILURE);

		default:
			// File Read
			fd = open(targetfile,  O_RDWR | O_CREAT | O_TRUNC , \
						S_IRWXU | S_IWGRP | S_IRGRP | S_IRUSR | S_IRGRP | S_IROTH );

			if(fd == -1){
				fprintf(stderr, "Parent Process: shmat failed\n");
				exit(EXIT_FAILURE);	
			}
			
			while(1){
				// 1 - Read Privilege, 0 - Write - Privilege
				while(*(shmaddr_flag) != WRITE){
					sleep(0);
				}

				memset(buffer, 0, sizeof(buffer));
				strcpy(buffer, shmaddr_data);
				

				if(*(shmaddr_flag + 1) == 0){
					*(shmaddr_flag) = READ;
					printf("Break - EOF\n");
					break;
				}

				write_byte = write(fd, buffer, *(shmaddr_flag+1));
				printf("Parent Process: Size = %d\nBuffer = %s\n", write_byte, buffer);	
				
				*(shmaddr_flag) = READ;

				if(write_byte == -1){
					fprintf(stderr, "Parent Process: write file failed\n");
					exit(EXIT_FAILURE);	
				}
			}

			if(close(fd) == -1){
				fprintf(stderr, "Parent Process: file close failed\n");
				exit(EXIT_FAILURE);		
			}
			
			 //detache shared memeory
			printf("Parent Process: flags shmdt()\n");	
			if(shmdt(shared_flag) == -1){
				fprintf(stderr, "Parent Process: shmdt failed\n");
				exit(EXIT_FAILURE);
			}

			//detache shared memeory
			printf("Parent Process: Data shmdt()\n");
			if(shmdt(shared_data) == -1){
				fprintf(stderr, "Parent Process: shmdt failed\n");
				exit(EXIT_FAILURE);
			}

			// childPID = wait(&status);
			printf("Parent Process: wait()\n");	
			if(wait(&status) == -1){
				fprintf(stderr, "Parent Process: wait failed\n");
				exit(EXIT_FAILURE);	
			 }

			//close shared memeory
			printf("Parent Process: flags shmctl()\n");	
			if(shmctl(shmid_flag, IPC_RMID, 0) == -1){
				fprintf(stderr, "Parent Process: shmctl(IPC_RMID) failed\n");
				exit(EXIT_FAILURE);
			}

			//close shared memeory
			printf("Parent Process: Data shmctl()\n");
			if(shmctl(shmid_data, IPC_RMID, 0) == -1){
				fprintf(stderr, "Parent Process: shmctl(IPC_RMID) failed\n");
				exit(EXIT_FAILURE);
			}
	}
	exit(EXIT_SUCCESS);
}