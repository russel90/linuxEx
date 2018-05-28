/*************************************************************************\
*                  Copyright (C) Michael Kerrisk, 2017.                   *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU General Public License as published by the   *
* Free Software Foundation, either version 3 or (at your option) any      *
* later version. This program is distributed without any warranty.  See   *
* the file COPYING.gpl-v3 for details.                                    *
\*************************************************************************/

/* Listing 4-1 */

#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/time.h>
#include "tlpi_hdr.h"

#ifndef BUF_SIZE        /* Allow "cc -D" to override definition */
#define BUF_SIZE 1024
#endif

main(int argc, char *argv[])
{
	int inputFd, outputFd, openFlags;
    mode_t filePerms;
    ssize_t numRead;
	char buf[BUF_SIZE];

    struct timeval StartTime, EndTime, ProcTime;

    if (argc != 3 || strcmp(argv[1], "--help") == 0)
        usageErr("%s old-file new-file\n", argv[0]);

    /* Open input and output files */

    inputFd = open(argv[1], O_RDONLY);
    if (inputFd == -1)
        errExit("opening file %s", argv[1]);

    gettimeofday(&StartTime, NULL);

    openFlags = O_CREAT | O_WRONLY | O_TRUNC;
    filePerms = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP |
                S_IROTH | S_IWOTH;      /* rw-rw-rw- */
    outputFd = open(argv[2], openFlags, filePerms);
    if (outputFd == -1)
        errExit("opening file %s", argv[2]);

    /* Transfer data until we encounter end of input or an error */

    while ((numRead = read(inputFd, buf, BUF_SIZE)) > 0)
        if (write(outputFd, buf, numRead) != numRead)
            fatal("couldn't write whole buffer");
    if (numRead == -1)
        errExit("read");

    if (close(inputFd) == -1)
        errExit("close input");
    if (close(outputFd) == -1)
        errExit("close output");
	
    gettimeofday(&EndTime, NULL);

    if((EndTime.tv_usec-StartTime.tv_usec) <0){
        ProcTime.tv_sec = EndTime.tv_sec - StartTime.tv_sec - 1;
        ProcTime.tv_sec = 1000000 + EndTime.tv_usec - StartTime.tv_usec;
    }else{
        ProcTime.tv_sec = EndTime.tv_sec - StartTime.tv_sec;
        ProcTime.tv_sec = EndTime.tv_usec - StartTime.tv_usec;
    }

    printf("Copy Time: %ld.%ldusec\n", ProcTime.tv_sec, ProcTime.tv_usec);
    
    // exit(EXIT_SUCCESS);
}
