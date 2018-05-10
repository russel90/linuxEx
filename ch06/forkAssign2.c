/**
 * 문제2: 
 * 1 ~ 10,000까지 더하는 프로그램을 작성하시오 (vfork() 함수 이용)
 * 
 * vfork() 함수를 사용하여 부모 프로세스에서는 1 ~ 5,000
 * 자식 프로세스에서는 5001 ~ 10,000을 더하는 프로그램을 작성하시오.
 * 부모: 1 ~5,000까지 더한 값 출력 sum1, 자식: 5001 ~ 1,000k까지 더한 값 출력 sum2
 * sum = sum1 + sum2
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
    int parentcnt, parentinitValue;
    int childcnt, childinitValue;
    int parentsum, childsum;
    int exit_code, status;

    printf("vfork program starting\n");
    pid = vfork();

    switch(pid){
        // 자식 프로세스 생성이 안된 경우
        case -1:
                perror("vfork failed");
                exit (-1);    
        
        // 자식 프로세스인 경우
        case 0:
                childinitValue = 5001;
                exit_code = 37;
                childsum = calSum(childinitValue);
                printf("Child results: %d\n", childsum);
       		
		        exit(0);
 
        // 부모 프로세스인 경우
        // default:
        //         parentinitValue = 1;
        //         exit_code = 0;

        //         parentsum = calSum(parentinitValue);
        //         printf("Parent results: %d\n", parentsum);
        //         printf("Total Sum = %d\n", parentsum + childsum);        
                
        //         break;
    }

    // 부모 프로세스인 경우
    parentinitValue = 1;
    exit_code = 0;

    parentsum = calSum(parentinitValue);
    printf("Parent results: %d\n", parentsum);
    printf("Total Sum = %d\n", parentsum + childsum);

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
