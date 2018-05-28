/**
����) shmWrite.c�� shmReader.c�� �����Ͽ� �Ʒ��� ���� ������ ���۽�Ű�ÿ�.

     Step 1. shmWriter.c����... 
             �޼��� ť�� �����ϰ� shmWrite���μ����� pid���� shmReader ���μ����� ����
              
     Step 2. shmReader.c���� 
             Ȯ���� ���� : shmWriter�� pid�� Ȯ��
             �޼��� ť�� shmReader���μ����� pid���� shmWriter ���μ����� ����

     Step 3. shmWriter.c����
             Ȯ���� ���� : shmReader�� pidȮ��.
     
     Step 4. shmWriter.c, shmReader.c ��������
             signal()�Լ��� ȣ���Ͽ� �ڵ鷯�� ����Ѵ�. (SIGUSR1�� Ȱ��)

     Step 5. shmWriter.c����
             shmget()
             shmat()
             �����޸𸮿� �����;��� 
             kill()�Լ��� ���� shmReader���μ����� SIGUSR1 �ñ׳��� �����Ѵ�. (�����Ͱ� �غ��)

     STEP 6. shmReader.c���� 
             shmget()
             shmat()
             �����޸𸮿��� ������ �б�
             kill()�Լ��� ���� shmWriter���μ����� SIGUSR1 �ñ׳��� �����Ѵ�. (�����͸� �� ����) 
             shmdt()

     STEP 7. shmWriter.c����
             shmdt()
             shmctl()�� ���� ���� �޸� ��������

*/


#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <signal.h>
#include "shim.h"

#define SHMSIZE 100

static void sigHandler(int signo)
{
	if(signo==SIGUSR1)
		printf("SIGUSR1\n");
	//else if(signo==SIGUSR2)
	//	printf("SIGUSR2\n");
}

int main(void)
{
	void *shared_Mem=(void *)0;
	struct pidSt pidSt1;
	pid_t pid, targetPID;
	long int msg_to_receive = 0;
	
	int msqid;
	key_t keyval;

	int shmid;
	int *shmaddr;
	int i;

	// create Queue
	// first we set up the message queue.
	keyval = ftok(STRING, PROJ_ID);
	msqid = msgget(keyval, IPC_CREAT | 0666);
	if(msqid == -1){
		fprintf(stderr, "msgget failed with error: %d\n", errno);
		exit(EXIT_FAILURE);
	}
	
	printf("msqId: %d\n", msqid);

	// STEP 4. �ñ׳� �ڵ鷯 ���
	signal(SIGUSR1, sigHandler);
	// signal(SIGUSR2, sigHandler);

	// STEP 3. shmReader2�� pid���� Ȯ��
    if ((msgrcv(msqid, (void *)&pidSt1, sizeof(struct pidSt), msg_to_receive, 0)) == -1)
	{
		fprintf(stderr, "msgrcv failed with error: %d\n", errno);
		exit(EXIT_FAILURE);
	}

	targetPID = pidSt1.pid;
	printf("target pid=%d\n", targetPID);

	pidSt1.msg_type = 1;
	pidSt1.pid = getpid();

	printf("Reader PID = %d\n", pidSt1.pid);

	if(pidSt1.pid == -1){
		fprintf(stderr, "pid creation Failed\n");
		exit(EXIT_FAILURE);
	}

	// Message Sending 
	if(msgsnd(msqid, (void *)&pidSt1, sizeof(pidSt1), 0) == -1){
		fprintf(stderr, "Message Sending Failed\n");
		exit(EXIT_FAILURE);
	}

	kill(targetPID, SIGUSR2);
	pause();

	// close message queue
	//if (msgctl(msqid, IPC_RMID, 0) == -1)
	//{
	//	fprintf(stderr, "msgctl(IPC_RMID) failed\n");
	//	exit(EXIT_FAILURE);
	//}

	// 1. shmget
	shmid = shmget((key_t)1234, sizeof(int)*SHMSIZE, IPC_CREAT | 0666);
	if(shmid == -1)
	{
		fprintf(stderr, "shmget failed\n");
		exit(EXIT_FAILURE);
	}

	// 2. shmat
	shared_Mem = shmat(shmid, (void *)0, 0);
	if(shared_Mem == (void *)-1)
	{
		fprintf(stderr, "shmat failed\n");
		exit(EXIT_FAILURE);
	}

	// 3. memory access
	printf("Memoryb attached at )x%p\n", shared_Mem);
	shmaddr = (int *)shared_Mem;

	for(i = 0; i < SHMSIZE; i++){
		// *(shmaddr +i) = i + 1;
		printf("shmaddr: %p, data: %d\n", shmaddr + i, *(shmaddr +i));
	}

	kill(targetPID, SIGUSR1);

	//4. chmdt
	if(shmdt(shared_Mem) == -1){
		fprintf(stderr, "shmdt failed\n");
		exit(EXIT_FAILURE);
	}
	exit(EXIT_SUCCESS);
}	
