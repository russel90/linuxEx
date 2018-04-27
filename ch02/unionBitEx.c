#include <stdio.h>

struct Flgas{
	union{ // 익명 공동체
		struct{ // 익명 구조체	
			unsigned short a : 3; // a는 3비트 크기
			unsigned short b : 2; // b는 2비트 크기
			unsigned short c : 7; //
			unsigned short d : 4; //
		};
	};

	unsigned short e; 
};

int main(){
	struct Flgas f1 = {0, }; // 모든 멤버를 0으로 초기화

	f1.a = 4;
	f1.b = 2;
	f1.c = 80;
	f1.d = 15;

	f1.a = 4;
	f1.b = 2;
	f1.c = 80;
	f1.d = 15;

	printf("%u\n", f1.e);
	
	return 0;
}
