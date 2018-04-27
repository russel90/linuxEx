#include <stdio.h>

struct Test1{
	char a;
	int b;
	char c;
	int d;
};

int main(){
	struct Test1 test1;
	
	printf("%d\n", sizeof(test1));
	printf("%p\n", &test1.a);
	printf("%p\n", &test1.b);
	printf("%p\n", &test1.c);
	printf("%p\n", &test1.d);
}
