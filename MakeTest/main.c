#include <stdio.h>

//print.c
extern void print(char* str);

//input.c
extern char* input();

int main(void)
{
	char* str = input();
	print(str);
	print("\n");
	
	return 0;
}
