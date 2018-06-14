#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>

struct timeval StartTime, EndTime, ProcTime;

void timer_handler (int signum)
{
    static int count = 0;
    

    gettimeofday(&EndTime, NULL);

    if((EndTime.tv_usec-StartTime.tv_usec) <0){
        ProcTime.tv_sec = EndTime.tv_sec - StartTime.tv_sec - 1;
        ProcTime.tv_sec = 1000000 + EndTime.tv_usec - StartTime.tv_usec;
    }else{
        ProcTime.tv_sec = EndTime.tv_sec - StartTime.tv_sec;
        ProcTime.tv_sec = EndTime.tv_usec - StartTime.tv_usec;
    }

    printf("timer expired %d timers\t", ++count);
    printf("Copy Time: %ld.%ldusec\n", ProcTime.tv_sec, ProcTime.tv_usec);

    StartTime = EndTime;
}

int main ()
{
        struct sigaction sa;
        struct itimerval timer;

        /* Install timer_handler as the signal handler for SIGVTALRM. */
        memset (&sa, 0, sizeof (sa));
        sa.sa_handler = &timer_handler;
        sigaction (SIGVTALRM, &sa, NULL);

        /* Configure the timer to expire after 250 msec... */
        timer.it_value.tv_sec = 0;
        timer.it_value.tv_usec = 250000;

        /* ... and every 250 msec after that. */
        timer.it_interval.tv_sec = 0;
        timer.it_interval.tv_usec = 250000;

        /* Start a virtual timer. It counts down whenever this process is executing. */
        setitimer (ITIMER_VIRTUAL, &timer, NULL);

        gettimeofday(&StartTime, NULL);

        /* Do busy work.  */
        while (1);
}
