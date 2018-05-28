/* The sender program is very similar to msg1.c. In the main set up, delete 
   the msg_to_receive declaration and replace it with buffer[BUFSIZ], remove
   the message queue delete and make the following changes to the running loop.
   We now have a call to msgsnd to send the entered text to the queue.
  */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <sys/stat.h>
#include <sys/time.h>

#define MAX_NAME_SIZE 10
#define MAX_AGE_SIZE 3
#define MAX_ID_SIZE 5

struct person_info{
	long msg_type;
	char name[MAX_NAME_SIZE];
	unsigned char age[MAX_AGE_SIZE];
	unsigned char id[MAX_ID_SIZE];
};

int writeLog(struct person_info *person, char *filename)
{
	int numWrite, fd;
	char tm_buffer[BUFSIZ], buffer[BUFSIZ];

	time_t UTCtime;
	struct tm *tm;
	struct timeval UTCtime_u;
	
	fd = open(filename, O_RDWR | O_CREAT | O_APPEND, \
		S_IRWXU | S_IWGRP | S_IRUSR | S_IRGRP | S_IROTH);

	if (fd == -1) {
		printf("file open error!!\n");
		return -1;
	}

	gettimeofday(&UTCtime_u, NULL);
	
	time(&UTCtime);
	tm=localtime(&UTCtime);

	strftime(tm_buffer, sizeof(tm_buffer), "%Y-%m-%d %H:%M:%S", tm);

	printf("%s %.3ld %s %s %s\n", tm_buffer, (long )(UTCtime_u.tv_usec / 1000), person->id, person->name, person->age);
	sprintf(buffer,"%s %.3ld %s %s %s\n", tm_buffer, (long )(UTCtime_u.tv_usec / 1000), person->id, person->name, person->age);
	numWrite = write(fd, buffer, strlen(buffer));
	if (numWrite == -1) {
		printf("file write error!!\n");
		return -1;
	}

	close(fd);

	return 0;

}

int main(void)
{
	int running = 1;
	int msgid;

	struct person_info person;
	long int msg_to_receive = 0;

	char buffer[BUFSIZ];

	msgid = msgget((key_t)1234, IPC_CREAT | 0666);

	if(msgid == -1){
		fprintf(stderr, "msgget failed with error: %d\n", errno);
		exit(EXIT_FAILURE);
	}

	printf("MSG_ID: %d\n", msgid);
	while(running){
		if(msgrcv(msgid, (void *)&person, sizeof(person), msg_to_receive, 0) == -1){
			fprintf(stderr, "msgrcv failed with error: %d\n", errno);
			exit(EXIT_FAILURE);
		}

		writeLog(&person, "Log.txt");

		if(person.msg_type == 2){
			running = 0;
		}		
	}

	exit(EXIT_SUCCESS);
}

