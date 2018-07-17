#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<iostream>
#include<ctime>
#include <cstdint>
using namespace std;

int thread_count;
int size, local_size;
int *a, *b, *c;
FILE *fp;

int* transpose_matrix(int *m, int size);

void* Init();

void* pthread_mult(void* rank);

int main(int argc, char* argv[]) {
	int i, j;
	long thread;
	float time_use = 0;
	clock_t  start = clock();
	size = 1000;
	thread_count = strtol(argv[1], NULL, 10);
	pthread_t thread_handles[thread_count];
	local_size = size / thread_count;
	Init();

	for (thread = 0; thread < thread_count; thread++)
		pthread_create(&thread_handles[thread], NULL, pthread_mult, (void*) thread);
	for (thread = 0; thread < thread_count; thread++)
		pthread_join(thread_handles[thread], NULL);
	fp = fopen("c.txt", "w"); //打开文件
	for (i = 0; i < 1000; i++) { //写数据
		for (j = 0; j < 1000; j++)
			fprintf(fp, "%d ", c[i * size + j]);
		fputc('\n', fp);
	}
	fclose(fp);//关闭文件

	clock_t  end = clock();
	cout << "time consumed:" << end - start << endl;

	free(thread_handles);
	free(a);
	free(b);
	free(c);
	return 0;
}

int* transpose_matrix(int *m, int size) {
	int i, j;
	for (i = 0; i < size; i++) {
		for (j = i + 1; j < size; j++) {
			int temp = m[i * size + j];
			m[i * size + j] = m[j * size + i];
			m[j * size + i] = temp;
		}
	}
	return m;
}

void* Init() {

	int i, j;

	a = (int*)malloc(sizeof(int) * size * size);
	b = (int*)malloc(sizeof(int) * size * size);
	c = (int*)malloc(sizeof(int) * size * size);

	//从文件中读入矩阵
	fp = fopen("a.txt", "r"); //打开文件
	for (i = 0; i < 1000; i++) //读数据
		for (j = 0; j < 1000; j++)
			fscanf(fp, "%d", &a[i * size + j]);
	fclose(fp);//关闭文件

	fp = fopen("b.txt", "r");

	for (i = 0; i < 1000; i++)
		for (j = 0; j < 1000; j++)
			fscanf(fp, "%d", &b[i * size + j]);
	fclose(fp);

	b = transpose_matrix(b, size);
}

void* pthread_mult(void* rank) {
	intptr_t my_rank = (intptr_t) rank;
	int i, j, k, temp;
	int my_first_row = my_rank * local_size;
	int my_last_row = (my_rank + 1) * local_size - 1;

	for (i = my_first_row; i <= my_last_row; i++) {
		for (j = 0; j < size; j++) {
			temp = 0;
			for (k = 0; k < size; k++)
				temp += a[i * size + k] * b[j * size + k];
			c[i * size + j] = temp;
		}

	}
}