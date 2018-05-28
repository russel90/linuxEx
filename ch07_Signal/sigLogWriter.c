/*
[Results]
1. SIGUSR1	--> open(log.txt) 
2. SIGINT		--> wirte log (format YYYY-MM--DD HH:MM:SS SIGINT)
3. SIGINT		-->	wirte log (format YYYY-MM--DD HH:MM:SS SIGINT)
4. SIGQUIT  --> wirte log (format YYYY-MM--DD HH:MM:SS SIGQUIT)
5.	SIGUSR2	--> close(log.txt)

[Functions]
1. sigGenerator: 
-실행시 pid 파일(text1.txt)에 Write
-text2.txt에서 sigLogWriter의 target PID를 읽어 들임
-target PID No를 이용하여 SIGUSR1을 Signal 발생
-SIGUSR1을 받을 경우 File Open Error 출력후 sigLogWriter Kill 후 exit(0)
-SIGUSR2를 받을 경우 target PID에 SIGINT을 발생 시키고 Waiting
-SIGUSR2를 5번 받으면 성공 메세지 및 file을 close하고 target PID kill하고 exit(0)

2. sigLogWriter: 실행시 pid 파일(text1.txt)에 Write 쓰고, SIGUSR1 GENERATE
- 실행시 pid 파일(text2.txt)에 Write
-Text1.txt에서 sigGenerator의 target PID를 읽어 들임
- 파일 생성이 실패하면 target PID에 SIGUSR1을 발생 시킴
- 파일 생성이 성공하면, Target PID에 SIGUSR2를 발생 시킴
-SIGINT를 전달 받으면 LogWrite하고 SIGUSR1을 발생 시킴
*/


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>

int writePID(pid_t);
pid_t readTargetPID();
int writeLog(int signo);


