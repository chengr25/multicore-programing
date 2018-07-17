#include <omp.h>
#include <stdio.h>
#include <time.h>
#include <iostream>
#define N 10000

using namespace std;
int A[N][N], B[N][N];


int main() {
	int num_thread;
	cout << "enter the thread number(enter 0 to quit)" << endl;
	cin >> num_thread;
	FILE *fp;
	int i, j;
	fp = fopen( "a.txt", "r");
	for (i = 0; i < 1000; i++)
		for (j = 0; j < 1000; j++)
			fscanf(fp, "%d", &A[i][j]);
	fclose(fp);
	fp = fopen( "b.txt", "r");
	for (i = 0; i < 1000; i++)
		for (j = 0; j < 1000; j++)
			fscanf(fp, "%d", &B[i][j]);
	fclose(fp);
	while (num_thread) {
		clock_t start = clock();
		omp_set_dynamic(0);
		omp_set_num_threads(num_thread);
		#pragma omp parallel
		#pragma omp for
		for (i = 0; i < N; i++) {
			for (j = 0; j < N; j++) {
				B[i][j] = A[j][i];
			}
		}
		clock_t end  = clock();
		cout << "time consumed: " << end - start << "ms" << endl;
		cout << "enter the thread number(enter 0 to quit)" << endl;
		cin >> num_thread;
	}
}