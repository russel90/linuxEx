#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

struct my_msg_st{
	long int my_msg_type;
	char some_text[BUFSIZ];
};

int main(void)
{
	int running = 1;
	int msgid;

	struct my_msg_st some_data;
	long int msg_to_receive = 0;

	// first we set up the message queue.

	msgid = msgget((key_t)1234, IPC_CREAT | 0666);

	if(msgid == -1){
		fprintf(stderr, "msgget failed with error: %d\n", errno);
		exit(EXIT_FAILURE);
	}
	
	printf("msgId: %d\n", msgid);
	while(running){
		if(msgrcv(msgid, (void *)&some_data, BUFSIZ, msg_to_receive, 0) == -1){
			fprintf(stderr, "msgrcv failed with error: %d\n", errno);
			exit(EXIT_FAILURE);
		}

		printf("You wrote: %s\n", some_data.some_text);
		if(strncmp(some_data.some_text, "end", 3) == 0){
			running = 0;
		}
	}

	// Then the messages are retrieved from the queue, until an end message is encounteered.
	// Lastly the message queue is deleted.

	if(msgctl(msgid, IPC_RMID, 0) == -1){
		fprintf(stderr, "msgctl(IPC_RMID) failed\n");
		exit(EXIT_FAILURE);
	}

	exit(EXIT_SUCCESS);
}

