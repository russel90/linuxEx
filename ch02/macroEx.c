#include <stdio.h>
#define MUL2(x, y) (x * 6)
#define MUL1(x, y) ((x)*(y))

void main(){
    int a = 1, b = 2, c = 3, d = 5, k = 0;

    k = MUL2(a+b, c+d);
    printf("%d\n", k);

    k = MUL1(a+b, c+d);
    printf("%d\n", k);
}