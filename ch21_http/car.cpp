#include <iostream>
#include <string>

using namespace std;
class Car {
	public:
	// �����������
	int speed; // �ӵ�
	int gear; // ���
	string color; // ����
	// ����Լ�����
	void speedUp() { // �ӵ���������Լ�
		speed += 10;
	}
	void speedDown() { // �ӵ����Ҹ���Լ�
		speed -= 10;
	}
};
 

int main()
{
	Car myCar; // ��ü ����
	myCar.speed = 100; // ������� ����
	myCar.gear = 3;
	myCar.color = "red";
	myCar.speedUp(); // ����Լ� ȣ��
	myCar.speedDown();
	return 0;
}