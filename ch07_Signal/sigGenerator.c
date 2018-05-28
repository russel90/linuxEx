/*
* [Results]
* 	1. SIGUSR1	--> open(log.txt) 
* 	2. SIGINT		--> wirte log (format YYYY-MM--DD HH:MM:SS SIGINT)
* 	3. SIGINT		-->	wirte log (format YYYY-MM--DD HH:MM:SS SIGINT)
* 	4. SIGQUIT  --> wirte log (format YYYY-MM--DD HH:MM:SS SIGQUIT)
* 
* 
* 
* 
*     5.	SIGUSR2	--> close(log.txt)
* 
* [Functions]
* 	1. sigGenerator: 
* 		- 실행시 pid 파일(text1.txt)에 Write
* 		-text2.txt에서 sigLogWriter의 target PID를 읽어 들임
*   -target PID No를 이용하여 SIGUSR1을 Signal 발생
* 		-SIGUSR1을 받을 경우 File Open Error 출력후 sigLogWriter Kill 후 exit(0)
* 		-SIGUSR2를 받을 경우 target PID에 SIGINT을 발생 시키고 Waiting
*   -SIGUSR2를 5번 받으면 성공 메세지 및 file을 close하고 target PID kill하고 exit(0)
* 
* 	2. sigLogWriter: 실행시 pid 파일(text1.txt)에 Write 쓰고, SIGUSR1 GENERATE
* 		- 실행시 pid 파일(text2.txt)에 Write
* 		-Text1.txt에서 sigGenerator의 target PID를 읽어 들임
* 		- 파일 생성이 실패하면 target PID에 SIGUSR1을 발생 시킴
* 		- 파일 생성이 성공하면, Target PID에 SIGUSR2를 발생 시킴
*   -SIGINT를 전달 받으면 LogWrite하고 SIGUSR1을 발생 시킴
*  
* */


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <time.h>

#define BUF_MAX_SIZE 100

int setPID(char *filename, pid_t pid);
int logWirter(char *filename, int sig);
pid_t getTargetPID(char *filename);

int setPID(char *filename, pid_t pid)
{
	int fd;
	ssize_t numWrite;
	char buffer[BUF_MAX_SIZE];

	fd = open(filename, O_RDWR | O_CREAT | O_TRUNC, \
		S_IRWXU | S_IWGRP | S_IRUSR | S_IRGRP | S_IROTH);

	if (fd == -1) {
		printf("file open error!!\n");
		return -1;
	}

	sprintf(buffer, "%d", pid);
	numWrite = write(fd, buffer, strlen(buffer));
	if (numWrite == -1) {
		printf("file write error!!\n");
		return -1;
	}

	close(fd);

	return 0;

}

int logWirter(char *filename, int sig)
{
	int byteCount;
	char buffer[BUF_MAX_SIZE];
	struct tm *loc;
	time_t t;
	int fd;

	// 새로운 또는 기존 파일을 쓰기용으로 연다. 
	// 언제나 파일의 맨뒤에 추가해서 쓴다
	fd = open( filename, O_WRONLY| O_CREAT| O_APPEND, S_IRUSR| S_IWUSR);

	if (fd == -1) {
		printf("file open error!!\n");
		return -1;
	}

	// printf("take a signal [%d]",sig);
	// ctime(&t);
	t = time(&t);
	loc = localtime(&t);

	strftime(buffer, BUF_MAX_SIZE, "%Y-%m-%d %T", loc);
	sprintf(buffer, "%s sigNo= [%d]\n", buffer, sig);
	
	byteCount=write(fd,buffer,strlen(buffer));

	close(fd);
}

pid_t getTargetPID(char *filename)
{
	int fd;
	ssize_t numRead;
	char buffer[BUF_MAX_SIZE];

    // Option 정리 read only / no truncate   
    fd= open(filename, O_RDONLY, S_IRWXU| S_IWGRP| S_IRUSR| S_IRGRP| S_IROTH);

    if(fd  == -1){
		  printf("file open error!!\n");
		  return -1;
    }      

    numRead = read(fd, buffer,strlen(buffer));
    if(numRead  == -1){
      printf("file read error!!\n");
      return -1;
    } 

    close(fd);

	return atoi(buffer);
}

int main(void)
{
	char fnPID[BUF_MAX_SIZE]="Text1.txt";
	char fnTargetPID[BUF_MAX_SIZE]="Text1.txt";
	char fnLog[BUF_MAX_SIZE]="log.txt";
	pid_t pid, targetPID;

	pid = getpid();
	printf("sigGen1: sigGen1 pid: %d\n", pid);

	if (setPID(fnPID, pid) == -1)	{
		printf("setPID Error\n");
	}

	targetPID = getTargetPID(fnTargetPID);
	if ( targetPID == -1)	{
		printf("getPID Error\n");
	}

	printf("Target PID: %d\n", targetPID);

	logWirter(fnLog, 7);
	logWirter(fnLog, 9);

	return 0;
}
