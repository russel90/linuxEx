#define _BSD_SOURCE
#include <stdlib.h>
#include "tlpi_hdr.h"

static void atexitFunc1(void)
{
    printf("atext function 1 called\n");
}

static void atexitFunc2(void)
{
    printf("atext function 2 called\n");
}

static void onExitFunc(int exitStatus, void *arg)
{
    printf("OnExitFunc called: status = %d, arg = %ld\n", exitStatus, (long)arg);
}

int main(void)
{
    // 종료 핸들러1 등록
    if(on_exit(onExitFunc,(void *)10) !=0 )
        fatal("on_exit 1");
    
    // 종료 핸들러2 등록
    if(atexit(atexitFunc1) != 0)
        fatal("atext 1");
    
    // 종료 핸들러3 등록
    if(atexit(atexitFunc2) != 0)
        fatal("atext 1");

    // 종료 핸들러4 등록    
    if(on_exit(onExitFunc,(void *)20) !=0 )
        fatal("on_exit 1");
    
    exit(0);
}