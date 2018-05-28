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
	
	key_t keyValue;
	void *shared=(void *)0;
	int shmid;
	struct mesData *shmaddr; 

	char buffer[SHMSIZE], keyflag[MAX_ID_SIZE];
	char *sourcefile, *targetfile;
	
	// struct mesData msd;

	printf("argc = %d\n",argc);

	if(argc != 3 || strcmp(argv[1], "--help") == 0){
		fprintf(stderr, "Usage: %s Source Target \n",*argv);
		exit(EXIT_FAILURE);	
	}

	sourcefile = argv[1];
	targetfile = argv[2];

	printf("Parent Process: Source = [%s]\tTarget = [%s]\n", sourcefile, targetfile);

	// generate flag shared memory
	keyValue = ftok(STRING, PROJ_ID);
	shmid = shmget(keyValue, sizeof(struct mesData), 0666 | IPC_CREAT);
	if(shmid == -1)
	{
		fprintf(stderr, "shmget failed\n");
		exit(EXIT_FAILURE);
	}

	// attached flag shared memeory
	shared = shmat(shmid, (void *)0, 0);
	if(shared == (void *)-1)
	{
		fprintf(stderr, "shmat failed\n");
		exit(EXIT_FAILURE);
	}

	shmaddr = (struct mesData *)shared;
	// msd = shmaddr;
	
	printf("Shared Memory attached at )x%p\n", shmaddr);
	// printf("Struct Memory attached at )x%p\n", msd);

	switch(fork()){
		case -1:
			fprintf(stderr, "shmget failed\n");
			exit(EXIT_FAILURE);

		case 0:
			// childPID = getpid();
			printf("===============================\n");
			printf("Chil Procee: Call Child Process\n");

		    sprintf(keyflag, "%d", keyValue);

			printf("Chil Procee: Called Execl\n");
			printf("Child Process: keyValue = [%d]\n", keyValue);
			execl("./shmFileReader", "./shmFileReader", keyValue, sourcefile ,NULL);
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
				// while(*(shmaddr) != WRITE){
				// 	sleep(0);
				// }

				while(shmaddr->flags != WRITE){
					sleep(0);
				}

				memset(buffer, 0, sizeof(buffer));
				strcpy(buffer, shmaddr->file_stream);
				

				if(shmaddr->read_byte == 0){
					shmaddr->flags = READ;
					printf("Break - EOF\n");
					break;
				}

				write_byte = write(fd, buffer, shmaddr->read_byte);
				printf("Parent Process: Size = %d\nBuffer = %s\n", write_byte, buffer);	
				
				shmaddr->flags = READ;

				if(write_byte == -1){
					fprintf(stderr, "Parent Process: write file failed\n");
					exit(EXIT_FAILURE);	
				}
			}

			if(close(fd) == -1){
				fprintf(stderr, "Parent Process: file close failed\n");
				exit(EXIT_FAILURE);		
			}

			// childPID = wait(&status);
			printf("Parent Process: wait()\n");	
			if(wait(&status) == -1){
				fprintf(stderr, "Parent Process: wait failed\n");
				exit(EXIT_FAILURE);	
			 }

			 //detache shared memeory
			printf("Parent Process: flags shmdt()\n");	
			if(shmdt(shared) == -1){
				fprintf(stderr, "Parent Process: shmdt failed\n");
				exit(EXIT_FAILURE);
			}

			//close shared memeory
			printf("Parent Process: flags shmctl()\n");	
			if(shmctl(shmid, IPC_RMID, 0) == -1){
				fprintf(stderr, "Parent Process: shmctl(IPC_RMID) failed\n");
				exit(EXIT_FAILURE);
			}

	}
	exit(EXIT_SUCCESS);
}