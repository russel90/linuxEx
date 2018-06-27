#include <iostream>
#include <string>

using namespace std;
class Car {
	public:
	// 멤버변수선언
	int speed; // 속도
	int gear; // 기어
	string color; // 색상
	// 멤버함수선언
	void speedUp() { // 속도증가멤버함수
		speed += 10;
	}
	void speedDown() { // 속도감소멤버함수
		speed -= 10;
	}
};
 

int main()
{
	Car myCar; // 객체 생성
	myCar.speed = 100; // 멤버변수 접근
	myCar.gear = 3;
	myCar.color = "red";
	myCar.speedUp(); // 멤버함수 호출
	myCar.speedDown();
	return 0;
}