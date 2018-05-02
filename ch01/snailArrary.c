/**
* Program ID: test2.c
* Description: n X n Matrix 초기화
* writer: Yongwan Kim
* Date: 2018. 04. 26
* Version: 0.01
* History
* 2018.04.26 Initalized
*/

#include <stdio.h>
#include <stdlib.h>

int **make_arr(int **arr, int size);
void setNum_arr(int **arr, int size);
void print_arr(int **arr, int size);
void free_arr(int **arr, int size);


int main(void)
{
	int **arr = {0};
	int input = 1;

	printf("크기 입력 : ");
	scanf("%d", &input);
	
	arr = make_arr(arr, input);
	setNum_arr(arr, input);
	print_arr(arr, input);
	free_arr(arr, input);
	
	return 0;
}

int **make_arr(int **arr, int size)
{
	int i, j;
	
	arr = (int **)malloc(sizeof(int*)*size);
	for(i = 0; i< size; i++){
		arr[i] = (int *)malloc(size*sizeof(int));
		for(j = 0; j < size; j++){
			arr[i][j] = 0;
		}
	}

	return arr;
}

void setNum_arr(int **arr, int size)
{
	int i, x, y, sw, data;

	x = 0;
	y = 0;
	sw = 1;
	data = 1;

	while(size)
	{	
		for(i = 0; i< size; i++){
			arr[y][x] = data++;
			x += sw;
		}
		
		x -= sw;
		y += sw;
		size --;

		for(i = 0; i < size; i++){
			arr[y][x] = data++;
			y += sw;
		}

		y -= sw;
		sw *= -1;
		x += sw;
	}
}

void print_arr(int **arr, int size)
{
	int i, j;

	for(i = 0; i< size; i++){
		for(j = 0; j< size; j++){
			printf("[%3d] ", arr[i][j]);
		}
		puts("");
	}	
}

void free_arr(int **arr, int size){
	int i;
	
	for(i = 0; i < size; i++){
		free(arr[i]);
	}
	
	free(arr);
}
