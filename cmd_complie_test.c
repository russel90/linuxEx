/**
* Program ID: cmd_compile_test.c
* Description: 전달 받은 c program file을 컴파일하여 현재 working directory의 dist directory에 컴파일한 결과를 위치 시킨다
* 			   bash에소 export 명령을 통해 환경변수를 설정할 수 있음 
* writer: Yongwan Kim
* Date: 2018. 04. 27
* Version: 0.01
* History
* 2018.04.27 Initalized
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

extern char **environ;

int main(int argc, char *argv[]){
	char distDIR[100];
	char currentDI
	char buff[1014];

	printf("FILEDIR = %s\n", getenv("FILEDIR"));

	printf("argc=%d\n", argc);
	printf("argv[0]=%s\n", argv[0]);
	printf("argv[1]=%s\n", argv[1]);

	sprintf(distDIR,"%s",getenv("DIST_DIR"));
	printf("strDir = %s\n", distDIR);

	// 작업 디렉토리 구하기
	getcwd( buff, 1024);                   
	printf( "작업 디렉토리: %s\n", buff);

	return 0;
}

