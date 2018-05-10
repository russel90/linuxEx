#include <stdlib.h>
#include <stdio.h>

int main(void)
{
	printf("Running ps with system\n");

	// backgroup job 실행
	system("ps -ax&");
	printf("Done.\n");

	exit(0);
}
