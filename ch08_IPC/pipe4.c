#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[])
{
	int data_processed;
	char buffer[BUFSIZ + 1];
	int file_descriptor;
	
	memset(buffer, '\0', sizeof(buffer));

	scanf(argv[1], "%d", &file_descriptor);
	data_processed = read(file_descriptor, buffer, BUFSIZ);

	printf("%d - read %d bytes: %s\n", getpid(), data_processed, buffer);
	printf("%d - read %d bytes: %s\n", getpid(), file_descriptor, buffer);
	exit(EXIT_SUCCESS);
}
