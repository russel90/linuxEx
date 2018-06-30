#include <stdio.h>
#include <signal.h>
#include <time.h>
#include <errno.h>

#include <stdlib.h>
#include <unistd.h>

#include "tlpi_hdr.h"

struct timespec request;

/* Retrieve current value of CLOCK_REALTIME clock */

int main()
{
    int s;

    if(clock_gettime(CLOCK_REALTIME, &request) == -1)
    errExit("clock_gettime");

    /* sleep for 20 seconds from now */
    request.tv_sec += 20;

    s = clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &request, NULL);
    if(s == EINTR)
        printf("Interrupted by signal handler\n");
    else
        errExitEN(s, "clock_nanosleep");

    return 0;

 }   