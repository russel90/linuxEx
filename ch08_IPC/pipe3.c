#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>

int main(void)
{
	int data_processed;
	int file_pipes[2];

	const char some_data[] = "123";
	char buffer[BUFSIZ + 1];
	int status;


	pid_t fork_results;
	memset(buffer, '\0', sizeof(buffer));

	if(pipe(file_pipes)== -1){
		exit(EXIT_FAILURE);
	}

	fork_results = fork();
	switch(fork_results){
		case -1:
				fprintf(stderr,"Fork failure");
				exit(EXIT_FAILURE);
		case 0:
				sleep(5);
				sprintf(buffer,"%d", file_pipes[0]);
				printf("%s\n",buffer);
				execl("pipe4","pipe4", buffer, (char *)0);
				exit(EXIT_FAILURE);
		defaults:
				data_processed = write(file_pipes[1], some_data, strlen(some_data));
				printf("============= Parent Process ====================\n");
				printf("%d - wrote %d bytes\n", getpid(), data_processed);
				printf("============= Parent Process ====================\n");
				
				// wait(&status);
	}
	exit(EXIT_SUCCESS);
}
