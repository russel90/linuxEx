#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>

#define MAX_NAME_SIZE 10
#define MAX_AGE_SIZE 3
#define MAX_ID_SIZE 5

struct person_info{
	long msg_type;
	char name[MAX_NAME_SIZE];
	unsigned char age[MAX_AGE_SIZE];
	unsigned char id[MAX_ID_SIZE];
};

int main(void)
{
	int running = 1;
	int msgid;

	struct person_info person;
	long int msg_to_receive = 0;

	char buffer[BUFSIZ];
	char yesNo;

	// first we set up the message queue.

	msgid = msgget((key_t)1234, IPC_CREAT | 0666);

	if(msgid == -1){
		fprintf(stderr, "msgget failed with error: %d\n", errno);
		exit(EXIT_FAILURE);
	}
	
	printf("msgId: %d\n", msgid);

	while(running){
		person.msg_type = 1;

		printf("\n========================\n");	
		printf("ID: ");
		scanf("%s", person.id);

		printf("Person name: ");
		scanf("%s", person.name);

		printf("Age: ");
		scanf("%s", person.age);
		getchar();
		printf("\n========================");	

		printf("\nSedning more information (Y/N)?");
		scanf("%c", &yesNo);
		if(yesNo == 'N' || yesNo == 'n'){
			running = 0;
			person.msg_type =2;
		}

		// Message Sending 
		if(msgsnd(msgid, (void *)&person, sizeof(person), 0) == -1){
			fprintf(stderr, "Message Sending Failed\n");
			exit(EXIT_FAILURE);
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

