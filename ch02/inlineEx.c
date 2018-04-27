#include <stdio.h>

extern int square1(int x){
    return x * x;
}

double add(int x, int y){
    return (square1(x) + square1(y));
}

int main(void){
    int i, j; 

    i = 10, j = 10;
    printf("%f\n", add(i, j));
}