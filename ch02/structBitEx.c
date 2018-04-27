#include <stdio.h>

typedef struct flags{
	unsigned int a : 1;
	unsigned int b : 3;
	unsigned int c : 7;
} Flags;

int main(){
	Flags f1;
	
	f1.a = 1;
	f1.b = 15;
	f1.c = 255;


	printf("%d\n", f1.a);
	printf("%d\n", f1.b);
	printf("%d\n", f1.c);

	printf("%d\n", sizeof(f1));

	return 0;
}
