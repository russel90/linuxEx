/**
* 문제 1. 1 ~ 200,000까지 더하는 프로그램을 작성하시오.
* 프로그램 실행시 arg 값을 넘겨 받아 실행하기
* argv[1] = 1인 경우, one thread
* argv[2] = 2인 경우, two thread
* 스레드 함수에 인자값을 여러개 넘기려면 구조체를 만들어서 넘김
*/

#include <time.h>
#include <sys/time.h>
#include <stdlib.h>

#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include <sys/times.h>
#include <time.h>

#define MAX_THREAD 10

struct data{
	pthread_t id;
	long startNum;
	long endNum;
	long sum;
	int status;
};

void *t_function(void *data)
{
	int id;
	int i = 0;
	pthread_t t_id;

	long sum = 0;
	struct data *t_data;

	t_data = (struct data *)data;
	
	t_id = pthread_self();
	// printf("pid = %d, t_id = %lu, id = %d\n", getpid(), t_id, t_data->id);

	for(i = t_data->startNum; i <= t_data->endNum; i++)
	{
		sum += i;		
	}

	t_data->sum = sum;
	// printf("Seq id: [%d]\tStart Num: [%d]\tEnd Num: [%d]\tSub Total: [%ld]\n", (pthread_t) t_data->id, t_data->startNum, t_data->endNum, t_data->sum);

	return (void *)sum;
}

static void displayProcessTimes(const char *msg)
{
    struct tms t;
    clock_t clockTime;
    static long clockTicks = 0;

    if (msg != NULL)
        printf("%s", msg);

    if (clockTicks == 0) {      /* Fetch clock ticks on first call */
        clockTicks = sysconf(_SC_CLK_TCK);
		if (clockTicks == -1){
			perror("displayProcessTimes: sysconf error");
			exit(EXIT_FAILURE);	
		}
    }

    clockTime = clock();
	if (clockTime == -1) {
		perror("displayProcessTimes: clock error");
		exit(EXIT_FAILURE);	
	}

    printf("        clock() returns: %ld clocks-per-sec (%.2f secs)\n",
            (long) clockTime, (double) clockTime / CLOCKS_PER_SEC);

	if (times(&t) == -1){
		perror("displayProcessTimes: times error");
		exit(EXIT_FAILURE);	
	}

    printf("        times() yields: user CPU=%.2f; system CPU: %.2f\n",
            (double) t.tms_utime / clockTicks,
            (double) t.tms_stime / clockTicks);
}

int main(int argc, char *argv[])
{
	int i;
	int err;
	int status;

	int threadNum;
	long startNum, endNum;
	long interval;
	long sum = 0;
	
	struct data rData[MAX_THREAD];
	pthread_t p_thread[MAX_THREAD];	

	if(argc != 4 || strcmp(argv[1], "--help") == 0){
		fprintf(stderr, "Usage: %s threadNum startNum EndNum \n",*argv);
		exit(EXIT_FAILURE);	
	}

	printf("CLOCKS_PER_SEC=%ld  sysconf(_SC_CLK_TCK)=%ld\n\n",
            (long) CLOCKS_PER_SEC, sysconf(_SC_CLK_TCK));

    displayProcessTimes("At program start:\n");

	threadNum = atoi(argv[1]);
	startNum = atoi(argv[2]);
	endNum = atoi(argv[3]);	

	interval =  endNum/threadNum;
    
	for (i = 0; i < threadNum; i++){
		rData[i].id = i;	
		rData[i].startNum = interval * i + 1;
		
		if(i != (threadNum-1)){
			rData[i].endNum = rData[i].startNum + interval -1;
		}else{
			rData[i].endNum = endNum;
		}			

		if((err = pthread_create(&p_thread[i], NULL, t_function, (void *)&rData[i])) < 0){
			perror("thread create error: ");
			exit(1);
		}
	}

	for (i = 0; i < threadNum; i++){
		pthread_join(p_thread[i], (void **)&(rData[i].status));
		printf("Thread Join: %d\n", rData[i].status);
	}

	// Thread 종료후 결과 값 확인 필요 /
	// Thread 종료 전 결과 값 확인 시, Garbiage 값 출력
	printf("================================================================================================================\n");
	for(i = 0; i < threadNum; i++){
		// sum = sum + rData[i].sum;
		sum += rData[i].sum;
		printf("Seq No.: [%d] Start Num: [%10ld]\tEnd Num: [%10ld]\tSum: [%20ld]\n", i, rData[i].startNum, rData[i].endNum, rData[i].sum);
	}

	printf("================================================================================================================\n");
	printf("Total Sum = %20ld\n", sum);
	printf("================================================================================================================\n");

	displayProcessTimes("After getppid() loop:\n");

	return 0;
}
