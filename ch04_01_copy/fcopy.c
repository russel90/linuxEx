#include <stdio.h>
#define BUFSIZE 1024

int main(int argc, char **argv)
{
    int byteCount;
    FILE *in, *out;
    char buf[BUFSIZE];

    // 실행시 Paramter의 갯수가 정상인지 확인
    if(argc != 3)
    {
        // write(2, "Usage: fcopy file1 file2\n", 25); 와  동일
        fprintf(stderr, "Usage: fcopy file1 file2\n");
        return -1;
    }

    // 원본파일을 열지 못한 경우
    if((in=fopen(argv[1],"r")) == NULL)
    {
        perror(argv[1]);
        return -1;
    }

    // 대상파일을 열지 못한 경우
    if((in=fopen(argv[2],"w")) == NULL)
    {
        perror(argv[2]);
        return -1;
    }

    while((byteCount=fread( buf, sizeof(char), BUFSIZE, in))>0)
        fwrite( buf, sizeof(char), byteCount, out);

    fclose(in);
    fclose(out);
}