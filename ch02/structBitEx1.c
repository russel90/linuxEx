#include <stdio.h>

typedef struct flags1{
	unsigned int a : 1;
	unsigned int b : 3;
	unsigned int c : 7;
} Flags1;

typedef struct flags2{
	unsigned int a : 1;
	unsigned int b : 3;
	unsigned int c : 7;
	unsigned int d : 7;
	unsigned int f : 7;
} Flags2;

int main(){
	Flags1 f1;
	Flags2 f2; 
	
	f1.a = 1;
	f1.b = 15;
	f1.c = 255;


	printf("%d\n", f1.a);
	printf("%d\n", f1.b);
	printf("%d\n", f1.c);

	printf("%d\n", sizeof(f1));

	f2.a = 1;
	f2.b = 15;
	f2.c = 255;
	f2.d = 255;

	printf("%d\n", f2.a);
	printf("%d\n", f2.b);
	printf("%d\n", f2.c);
	printf("%d\n", f2.d);

	printf("%d\n", sizeof(f2));

	return 0;
}
