/**
문제) shmWrite.c와 shmReader.c를 수정하여 아래와 같은 구조로 동작시키시오.

     Step 1. shmWriter.c에서... 
             메세지 큐를 생성하고 shmWrite프로세스의 pid값을 shmReader 프로세스로 전달
              
     Step 2. shmReader.c에서 
             확인할 내용 : shmWriter의 pid를 확인
             메세지 큐에 shmReader프로세스의 pid값을 shmWriter 프로세스로 전달

     Step 3. shmWriter.c에서
             확인할 내용 : shmReader의 pid확인.
     
     Step 4. shmWriter.c, shmReader.c 각각에서
             signal()함수를 호출하여 핸들러를 등록한다. (SIGUSR1을 활용)

     Step 5. shmWriter.c에서
             shmget()
             shmat()
             공유메모리에 데이터쓰기 
             kill()함수를 통해 shmReader프로세스로 SIGUSR1 시그널을 전송한다. (데이터가 준비됨)

     STEP 6. shmReader.c에서 
             shmget()
             shmat()
             공유메모리에서 데이터 읽기
             kill()함수를 통해 shmWriter프로세스로 SIGUSR1 시그널을 전송한다. (데이터를 다 읽음) 
             shmdt()

     STEP 7. shmWriter.c에서
             shmdt()
             shmctl()을 통해 공유 메모리 공간삭제

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

	// STEP 4. 시그널 핸들러 등록
	signal(SIGUSR1, sigHandler);
	// signal(SIGUSR2, sigHandler);

	// STEP 3. shmReader2의 pid값을 확인
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
