#include <stdio.h>
int max(int *p);

void main(){
	int a[] = {1,2,3,4,5,6,7,8,9,10};
	int m;

	m = max(a);

	printf("%d\n",m);

}

int max(int *p){
	int i, tmp = -10000;

	for(i=0; i< 10; i++) tmp = p[i];

	return tmp;

}
