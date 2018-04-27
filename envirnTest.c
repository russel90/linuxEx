/**
* 환경 목록 설정 및 환경 가져오기 예
* 
* Program ID: environTest.c
* Description: 환경 목록 설정 및 환경 가져오기 예
* writer: Yongwan Kim
* Date: 2018. 04. 27
* Version: 0.01
* History
* 2018.04.26 Initalized
*/


#include <stdio.h>
#include <stdlib.h>

extern char **environ;

int main(void){
	setenv("MYDIR","/home/russel90",0);
	setenv("KERNEL_SRC","/usr/src/linux",0);

	printf("MYDIR = %s\n", getenv("MYDIR"));
	printf("KERNEL_SRC = %s\n", getenv("KERNEL_SRC"));

	// 현재 설정된 환경 변수 값을 모두 출력할 경우

	while(*environ) 
		printf("%s\n", *environ++);

}
