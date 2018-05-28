#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main(void)
{
	FILE *read_fp;
	char buffer[BUFSIZ + 1];
	int chars_read;

	memset(buffer, '\0', sizeof(buffer));
	read_fp = popen("cat popen*.c | wc -l","r");
	
	if(read_fp == NULL){
		pclose(read_fp);
		exit(EXIT_SUCCESS);
	}

	while(chars_read > 0){
		buffer[chars_read -1] = '\0';
		printf("Reading: -\n %s \n", buffer);
		chars_read = fread(buffer, sizeof(char), BUFSIZ, read_fp);
	}
	pclose(read_fp);
	exit(EXIT_SUCCESS);
}
