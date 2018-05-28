#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include "tlpi_hdr.h"

int main(int argc, char *argv[])
{
    int fd, flags;
    char template[] = "/tmp/testXXXXXX";

    // stdout의 버퍼링을 막는다.
    setbuf(stdout, NULL);

    fd = mkstemp(template);
    if(fd == -1)
        errExit("mkstemp");

    printf("File offset before fork(): %lld\n", (long long)lseek(fd, 0, SEEK_CUR));

    flags = fcntl(fd, F_GETFL);

    if(flags == -1)
        errExit("fcntl - F_GETFL");
    printf("O_APPEND flag before fork() is: %s\n", (flags &O_APPEND) ? "on" : "off");

    switch(fork()){
        case -1:
            // fork 생성 오류
            errExit("fork");

        case 0:
            // 자식: 파일 오프셋과 상태 플래그를 변경
            if(lseek(fd, 1000, SEEK_SET) == -1)
                errExit("lseek");

            // 현재 플래그 읽기
            flags = fcntl(fd, F_GETFL);
            if(flags == -1)
                errExit("fcntl - F_GETFL");

            // O_APPEND 속성 설정
            flags |= O_APPEND;

            if(fcntl(fd, F_SETFL, flags) == -1)
                errExit("fcntl - F_SETFL");

            _exit(EXIT_SUCCESS);

        default:
            // 부모: 자식이 만든 변경을 볼 수 있는지 확인
            
            // 자식의 종료를 기다린다.
            if(wait(NULL) == -1)
                errExit("wait");

            printf("Child has exited\n");
            printf("File offset in parent: %lld\n", (long long)lseek(fd,0,SEEK_CUR));

            flags = fcntl(fd, F_GETFL);
            if(flags == -1)
                errExit("fcntl - F_GETFL");
            printf("O_APPEND flag in parent is: %s\n", (flags &O_APPEND) ? "on" : "off");

            exit(EXIT_SUCCESS);
    }
}
