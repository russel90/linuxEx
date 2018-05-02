/**
* Program ID: main.c
* Description: 
* 자연수 N이 주어지면, 자연수를 유지하면서 N을 2로 몇 번까지 나눌 수 있는지를 생각해 볼 수 있다. 
* 즉, N의 모든 약수 중 2의 거듭제곱 꼴이면서 가장 큰 약수를 생각하는 것이다. 
* 예를 들어 15의 경우는 2로 한 번도 나눌 수 없으므로 2^0 = 1이 해당되고, 40의 경우는 2로 세 번까지 나눌 수 있으므로 2^3 = 8이 해당된다. 
* 이러한 약수를 함수값으로 가지는 함수 f(x)를 정의하자. 즉, f(15) = 1이고, f(40) = 8이다.
*
* 두 자연수 A, B(A≤B)가 주어지면, A 이상 B 이하의 모든 자연수에 대해서, 그 자연수의 모든 약수 중 2의 거듭제곱 꼴이면서 가장 큰 약수들의 총 합을 구하는 프로그램을 작성하시오. 즉 아래와 같은 수식의 값을 구해야 한다.
* writer: Yongwan Kim
* Date: 2018. 05. 02
* Version: 0.01
* History
* 2018.05.02 Initalized
*/

#include <stdio.h>

int isDivided(int num);

int main(void){
	int numA, numB;
    int cnt;
    int results;   
	do{
		printf("Pleased Enter two Number(A, B) (A<=B):");
		scanf("%d %d",&numA, &numB);
		cnt = numA;
		results = 0;
		while(cnt <= numB){
			results += isDivided(cnt);
			cnt++;
		}
		printf("%d\n",results);
	} while(1);

	return 0;
}

int isDivided(int num){
	const int bitComp = 0x1;
	
	if(num & bitComp) 
		return 1;
	else
		return isDivided(num>>1)<<1;
		// return 2 * isDivided(num / 2);
}
