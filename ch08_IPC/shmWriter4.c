/**
Witer.c�� Reader.c�� �����ϰ�
	�θ� ���μ���
	�ڽ� ���μ��� ����
	�ڽ� ���μ��� Reader.c�� ����

	�θ� ���μ������� Reader.c�� ����
	�θ� ���μ��������� ���� �޸𸮸� �����ϰ� �����޸𸮿� 1 ~ 1024�� ���� ���������� ä���.

	�ڽ� ���μ���
	�ڽ� ���μ������� ���� �޸𸮸� �� ä���ٴ� ��ȣ(Signal)�� �����Ѵ�.
	(��Ʈ) �θ�� �ڽİ��� ���μ��� ID�� Ȯ���ϴ� ���.
	���� ���� ������� ���� 
	�ڽ� ���μ��������� ä���� �޸� �����͸� �о� ���Ͽ� �����Ѵ�.
*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include "shim.h"

static void sigHandler(int signo)
{
	if(signo==SIGUSR1)
		printf("SIGUSR1\n");
}

int main(void)
{

	int status;
	pid_t parentPID, childPID;
	
	key_t keyval;
	void *shared_Mem=(void *)0;
	int shmid;
	int *shmaddr;
	int i;
	char buffer[20];

	parentPID = getpid();
	if(parentPID == -1)
	{
		fprintf(stderr, "pid failed\n");
		exit(EXIT_FAILURE);
	}

	// generate shared memory
	keyval = ftok(STRING, PROJ_ID);
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

	// ���� �޸� ����
	printf("Memory attached at )x%p\n", shared_Mem);
	shmaddr = (int *)shared_Mem;

	//fork 
	// #include <stdio.h>
	parentPID = getpid();

	childPID = fork();
	switch(childPID){
		case -1:
			fprintf(stderr, "shmget failed\n");
			exit(EXIT_FAILURE);

		case 0:
			// childPID = getpid();
			printf("===============================\n");
			printf("Chil Procee: Call Child Process\n");
			printf("Chil Procee: Child PID = [%d]/[%d]\n", getpid(), childPID);
			
			// Register Signal
			printf("Registered Child Signal\n");
			signal(SIGUSR1, sigHandler);

			sprintf(buffer,"%d",keyval);
			printf("Chil Procee: Called Execl\n");
			execl("./shmReader4", "./shmReader4", buffer, NULL);
			fprintf(stderr, "execl failed\n");
			exit(EXIT_FAILURE);

		default:
			printf("===============================\n");
			printf("Parent Procee: Call Parent Process\n");
			printf("Parent Procee: Parent PID = [%d]/[%d]\n", getpid(), parentPID);

			// Register Signal
			printf("Parent Procee: Registered Parent Signal\n");
			signal(SIGUSR1, sigHandler);

			printf("Parent Procee: Generated Signal to read shared memoery\n");
			for(i = 1; i < SHMSIZE; i++){
				*(shmaddr +i) = i + 1;
				// printf("shmaddr: %p, data: %d\n", shmaddr + i, *(shmaddr +i));
			}

			printf("Parent Procee: Paused until Reader Program executed\n");
			pause();

			// wait
			printf("Parent Procee: Called wait\n", childPID);
			childPID = wait(&status);
			printf("Parent Procee: Terminated Child Process. childPID = [%d]\n", childPID);

			if(childPID == -1){
				fprintf(stderr, "wait failed\n");
				exit(EXIT_FAILURE);	
			 }
			
			 //detache shared memeory
			if(shmdt(shared_Mem) == -1){
				fprintf(stderr, "shmdt failed\n");
				exit(EXIT_FAILURE);
			}

			//close shared memeory
			// kill(childPID, SIGUSR1);
			if(shmctl(shmid, IPC_RMID, 0) == -1){
				fprintf(stderr, "shmctl(IPC_RMID) failed\n");
				exit(EXIT_FAILURE);
			}

	}

	exit(EXIT_SUCCESS);
}	
