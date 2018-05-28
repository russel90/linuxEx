#include <signal.h>
#include "tlpi_hdr.h"

static void sigHandler(int sig)
{
    static int count = 0;
    {
        count++;
        printf("Caught SIgint (%d)\n", count);
        return; // resume execution at point of interruption
    }

    // Must be SIGQUIT - print a message and termination the process
    printf("Caught SIGQUIT - that's all forks!\n");
    exit(0);
}

int main(int argc, char *argv[])
{
    // Establish same handler for SIGINT

    if (signal(SIGINT, sigHandler) == SIG_ERR)
        errExit(-1);

    if(signal(SIGQUIT, sigHandler) == SIG_ERR)
        errExit(-1);

        // Loop forever, waiting for signals
        for(;;) 
        
        // block until a signal is caught
        pause();

}