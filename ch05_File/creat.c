#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#define BUFFERSIZE 100

void insert (char *m, char *s, int n);

int main(void){
    int fd, len;
    char buff[BUFFERSIZE], *rbuff="eggs ";
    int rcount, wcount, pos ;

    fd = open("./test.txt", O_RDWR | O_CREAT | O_TRUNC, \
                S_IRWXU | S_IWGRP | S_IRGRP | S_IROTH);


    if(fd == -1){
        printf("file open error!!\n");
        return -1;
    }

    // 버퍼에 테스트 문자열 만들기
    sprintf(buff, "Do not count the before they hatch");

    // test.txt 파일에 문자열 쓰기
    wcount = write(fd, buff, strlen(buff));

    close(fd);

    fd = open("./test.txt", O_RDWR | O_CREAT | O_TRUNC, \
                S_IRWXU | S_IWGRP | S_IRGRP | S_IROTH);

    // 파일 내용 읽어 오기
    if(fd == -1){
        printf("file open error!!\n");
        return -1;
    }    

    rcount = read(fd, buff, strlen(buff));

    len = strlen("Do not count ");

    // 문자열 길이를 기준으로 rbuff 값을 붙임
    insert(buff, rbuff, len);
    printf("%s\n", buff);

    wcount = write(fd, buff, strlen(buff));

    printf("File Descriptor: %d\n",fd);
    close(fd);

    return 0;
}

void insert (char *m, char *s, int n) { 
    memmove(m+n, m + n - strlen(s), strlen(m)); 
    memmove(m+n, s, strlen(s)); 
}