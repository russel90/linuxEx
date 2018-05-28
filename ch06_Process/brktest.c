#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int main(void)
{
	char *ptr;
	
	printf("Before end data seg addr : %10p\n", sbrk(0));
	ptr = malloc(0x1000);
	printf("after end data seg addr : %10p\n", sbrk(0));

	if(sbrk(0x1000) == (void *)-1)
		printf("sbrk error\n");
	printf("call sbrk(0x1000) : %10p\n", sbrk(0));

	if(brk(sbrk(0)+0x1000) != 0)
		printf("brk error\n");
	
	printf("call brk : %10p\n", sbrk(0));

	free(ptr);

	return 0;
}

