/**
* Program ID: test1.c
* Description: 구조체 Padding에 따른 성능 차이를 비교하기 위한 Program
* writer: Yongwan Kim
* Date: 2018. 04. 26
* Version: 0.01
* History
* 2018.04.26 Initalized
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_STRUCT_SIZE 255 
#define DEBUG

typedef struct _score{
	unsigned char name[8];
	unsigned char kor;
	unsigned char eng;
	unsigned char math;
	unsigned char hist;
	short total;
	float average;
} SCORE;

// 성적 Data 생성을 위한 함수
int createRandomData(SCORE *p);

// 총점과 평균을 구하는 함수
int calcuateScore(SCORE *p);

// Print Score Function
void printScore(SCORE *p);

SCORE *p;

int main(void){

	p = (SCORE *)malloc(sizeof(SCORE)*MAX_STRUCT_SIZE);

	createRandomData(p);
	calcuateScore(p);
	printScore(p);
	free(p);
	return 0;
}

int createRandomData(SCORE *p){
	int cnt;
	char name[8]="aaa";
	name[3]=0;

	srand(time(NULL));

	for(cnt = 0; cnt < MAX_STRUCT_SIZE; cnt++){
        // cnt = cnt + 0x30;
		// strcat(name, cnt+0x30);	
		strcpy(p[cnt].name, name);
		p[cnt].kor = rand() % 100;
		p[cnt].eng = rand() % 100;
		p[cnt].math = rand() % 100;
		p[cnt].hist = rand() % 100;
	}
	
	return 1;
}

int calcuateScore(SCORE *p){
	int cnt;
	int sum, average;
	
	for (cnt = 0; cnt < MAX_STRUCT_SIZE; cnt++){
		sum = p[cnt].kor + p[cnt].eng + p[cnt].math +p[cnt].hist;
		average = sum / 4;

		p[cnt].total = sum;
		p[cnt].average = average;	
	}

	return 1;
}

void printScore(SCORE *p){
	int cnt;

	printf("[name]\t[kor]\t[eng]\t[math]\t[hist]\t[total]\t[average]\n");
	for(cnt = 0; cnt < MAX_STRUCT_SIZE; cnt++){
		printf("%s\t%3d\t%3d\t%3d\t%3d\t%3d\t%3.2f\n",p[cnt].name, p[cnt].kor, p[cnt].eng, p[cnt].math, p[cnt].hist, p[cnt].total, p[cnt].average);
	}
}
