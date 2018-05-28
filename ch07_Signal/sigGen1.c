/**
 *  FileName: sigGen1.c 
 *  Descirption:
 *  1. 실행시 pid_no를 sigGen1.txt 파일 씀
 *  2. sigGen2.txt를 읽어서 sigGen2의 pid_no를 읽어옴.
 *  3. sigHandler 생성하고 Kill signal을 받을 때까지 pause 함
 *  4. kill signal을 받으면 counter 값을 증가시키고, sigGen1의 kill signal 전송
 *  5. counter 값이 5이면 프로그램 종료
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>

#define BUF_MAX_SIZE 10

pid_t t_pid;

sigHandler(int sig)
{
    static unsigned int counter = 0;
    printf("sig : %d, counter : %d\n",sig, counter);  

    kill(t_pid, SIGINT); 
    if(counter == 4) exit(0);
    counter++;
}

int main(int argc, char *argv[])
{
    pid_t pid;
    int s, fd;
    ssize_t numRead, numWrite;
    char buffer[BUF_MAX_SIZE];
        
    pid = getpid();
    sprintf(buffer, "%d", pid);
    printf("sigGen1: sigGen1 pid: %d\n", pid);

    //sigGen1.txt 파일에 SigGen1 PID 출력
    fd= open("./sigGen1.txt",O_RDWR|O_CREAT|O_TRUNC,\
                    S_IRWXU|S_IWGRP|S_IRUSR|S_IRGRP|S_IROTH);

    if(fd  == -1){
		  printf("file open error!!\n");
		  return -1;
    }                
    
    numWrite = write(fd,buffer,strlen(buffer));
    if(numWrite  == -1){
      printf("file write error!!\n");
      return -1;
    } 

    close(fd);

    //sigGen2.txt 파일에서 sigGen2 pid_no 읽어옴
    //sigGen1.txt 파일에 SigGen1 PID 출력
    // Option 정리 read only / no truncate   
    fd= open("./sigGen2.txt", O_RDONLY, S_IRWXU| S_IWGRP| S_IRUSR| S_IRGRP| S_IROTH);

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

    // signal Handler 등록
    signal(SIGINT, sigHandler);

    t_pid = atoi(buffer);
    printf("sigGen1: target pid: %d\n", t_pid);
    
    kill(t_pid, SIGINT);

    for(;;)
      pause();

    return 0;
}