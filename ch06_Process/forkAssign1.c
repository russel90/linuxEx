/**
 * 문제1: 
 * 1 ~ 10,000까지 더하는 프로그램을 작성하시오
 * 
 * fork() 함수를 사용하여 부모 프로세스에서는 1 ~ 5,000
 * 자식 프로세스에서는 5001 ~ 10,000을 더하는 프로그램을 작성하시오.
 * 부모: 1 ~5,000까지 더한 값 출력, 자식: 5001 ~ 1,000k까지 더한 값 출력
 * 
 * (단, 자식 프로세스가 종료 되었을 때 자식 프로세스가 정상적으로 종료 되었는지 시그널에 의해 종료 되었는지 확인하는 구문을 넣으시오)
 */

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int calSum(int initalValue);
void checkForkStatus(int *status);

int main(void)
{
    pid_t pid, childPid;
    int initValue, sum = 0;
    int exit_code, status;

    printf("fork program starting\n");
    pid = fork();

    switch(pid){
        // 자식 프로세스 생성이 안된 경우
        case -1:
                perror("fork failed");
                exit (-1);    
        // 자식 프로세스인 경우
        case 0:
                initValue = 5001;
                exit_code = 37;
                sum = calSum(initValue);
                printf("Child results: %d\n", sum);
                break;
        
        //부모 프로세스인 경우
        default:
                initValue = 1;
                exit_code = 0;
                sum = calSum(initValue);
                printf("Parent results: %d\n", sum);

                childPid = wait(&status);
                printf("Child has finished: PID = %d\n", childPid);

                checkForkStatus(&status);
		                        
                break;
    }

    exit(0);
}

int calSum(int initalValue)
{
    int cnt = 5000;
    int sum = 0;

    for(cnt = 0; cnt < 5000; cnt++)
        sum += initalValue + cnt;

    return sum;
}

void checkForkStatus(int *status)
{
		// 정상 종료인 경우
		if (WIFEXITED(status)){
            printf("normal termination, exit status = %d\n", WEXITSTATUS(status));
        } 
		// 시그널에 의해서 종료된 경우
		else if (WIFSIGNALED(status))
		    printf("abnormal termination, signal number = %d%s\n",WTERMSIG(status),
					WCOREDUMP(status) ? "(core file generated)" : "");
		else if (WIFSTOPPED(status))
		    printf("child stopped, signal number = %d\n", WSTOPSIG(status));
}