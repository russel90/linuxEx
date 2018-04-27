/** 
* 팰린드로 알로리즘 구현하기
* 팰린드롬이란, 앞에서부터 읽었을 때와, 뒤에서부터 읽었을 때가 같은 문자열이다.
* 어떤 위치에 있는 문자를 삭제
* 어떤 위치에 있는 문자를 교환
* 서로 다른 문자를 교환
* 문자열의 어떤 위치에 어떤 문자를 삽입 (시작과 끝도 가능)
* 모든 문자열이 팰린드롬이 아니기 때문에 다음과 같은 4가지 연산으로 보통 문자열을 팰린드롬으로 만든다.
* 1~3번 연산은 마음껏 사용할 수 있지만, 마지막 연산은 많아야 한 번 사용할 수 있다.
* 문자열이 주어졌을 때, 팰린드롬으로 만들기 위해 필요한 연산의 최솟값을 출력하는 프로그램을 작성하시오.
* 
* [힌트]
* 1. 최대 50개 문자 입력 받는 함수: ASCII 영문  소문자 97 ~ 122 테이블 이용
* 2. 문자열 검사 함수
*   입력 문자열 입력이 잘못 됐다는 메시지 출력
* 3. 팰린드롬인지 확인하는 함수
*    팰린드롬이 맞으면 맞다 출력
* 4. 팬린드롬이 아니면 팬린드롬을 만드는 함수 생성
* reference: https://www.acmicpc.net/problem/1053
*/

#include <stdio.h>
#include <string.h>
#define MAX_STRING_SIZE 50
#define ASCII_a 97
#define ASCII_z 122
#define DEBUG 1

int checkInputString(char *p);
int ispalindrome(char *p);
int cratepalindrome(char *p);
 
char InputString[MAX_STRING_SIZE];

int main(void){

    printf("Enter Input String : ");
    scanf("%s", InputString);

#ifdef DEBUG
    // getch();
    printf("Input String: [%s]\n", InputString);
	printf("String lengths: [%d]\n",strlen(InputString));
#endif

    if(checkInputString(InputString)){
        if(ispalindrome(InputString)){
            printf("your string is Palinderome\n");
        }else{
            if(cratepalindrome(InputString)){
                printf("Crated Palindrome: [%s]", InputString);
            }else{
                printf("Creating Palindrome is failed.\n");  
            }
        }
    }else{
        printf("Please Check Input String: %s\n", InputString);
    }
    
    return 0;
}

/** 
 * 입력받은 문자열중 소문자 이외의 문자가 있는지 확인하고,
 * 없으면 1, 아니면 0을 리턴하는 함수
*/

int checkInputString(char *p){
    int strlength = strlen(p);
    int cnt;
    char ch;

    for(cnt = 0; cnt < strlength; cnt++){
        ch = *(p + cnt);
        if(ch < ASCII_a || ch > ASCII_z) return 0;
    }
    
    return 1;
}

/**
 * 문자열의 길이를 구해 첫번째 요소와 마지막 요소의 값이 같은지 확인하고 같지 않으면 0, 
 * 같으면 다음 요소를 순차적으로 비교하는 로직 생성
 * 
*/
int ispalindrome(char *p){
    int strlength = strlen(p);
    int cnt;
    int cmpcnt;

    // strlength의 반만 비교하면 됨.
    cmpcnt = strlength >> 1;
 
#ifdef DEBUG
    printf("string length : [%d]\n", strlength);
	printf("String compare count: [%d]\n",cmpcnt);
#endif     

    for(cnt=0; cnt < cmpcnt; cnt++){
        if(*(p+cnt) != *(p+strlength-cnt -1)) return 0;  
    }
    return 1;
}


//
int cratepalindrome(char *p){

    return 0;
}

