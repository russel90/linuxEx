#include <stdio.h> 
#include <string.h> 

void insert(char *m ,char *s, int n); 


int main() { 
    char s1[20]="abcdefghhhhhhhh"; 
    char *s2="0123"; 
    insert(s1, s2, 12); 
    printf("%s", s1); 
    
    return 0; 
} 

void insert (char *m, char *s, int n) { 
    memmove(m+n, m + n - strlen(s), strlen(m)); 
    memmove(m+n, s, strlen(s)); 
}
