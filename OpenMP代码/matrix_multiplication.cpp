// ZhenLiu.cpp : 定义控制台应用程序的入口点。

#include <omp.h>
#include <stdio.h>
#include <time.h>
#define NN 2000

int A[NN][NN], B[NN][NN];
long long C[NN][NN];

void solve(int n, int num_thread) {
	int i, j, t, k, paralleltime, serialtime;
	clock_t startTime, endTime;
	long long sum;
	omp_set_dynamic(0);
	omp_set_num_threads(num_thread);
	//--------------对矩阵A和矩阵B进行初始化-------------
	FILE *fp;

	fp = fopen( "a.txt", "r"); //打开文件
	for (i = 0; i < 1000; i++) //读数据
		for (j = 0; j < 1000; j++)
			fscanf(fp, "%d", &A[i][j]);
	fclose(fp);//关闭文件

	fp = fopen( "b.txt", "r");

	for (i = 0; i < 1000; i++)
		for (j = 0; j < 1000; j++)
			fscanf(fp, "%d", &B[i][j]);
	fclose(fp);

	//----------------矩阵乘法并行算法------------------
	startTime = clock();
	sum = 0;
	#pragma omp parallel shared(A,B,C) private(i,j,k)
	{
		#pragma omp for schedule(dynamic)
		for (i = 0; i < n; i++) {
			for (j = 0; j < n; j++) {
				C[i][j] = 0;
				for (k = 0; k < n; k++) {
					C[i][j] += A[i][k] * B[k][j];
				}
			}
		}
	}

	for (i = 0; i < n; i++)
		for (j = 0; j < n; j++)
			sum += C[i][j];

	endTime = clock();
	paralleltime = endTime - startTime;
	printf("sum=%lld      parallel time=%dms\n", sum, paralleltime);
//--------------------------------------------------------
//--------------------------------------------------------
	startTime = clock();
	sum = 0;
	for (i = 0; i < n; i++) {
		for (j = 0; j < n; j++) {
			C[i][j] = 0;
			for (k = 0; k < n; k++) {
				C[i][j] += A[i][k] * B[k][j];
			}
		}
	}
	for (i = 0; i < n; i++)
		for (j = 0; j < n; j++)
			sum += C[i][j];
	endTime = clock();
	serialtime = endTime - startTime;
	printf("sum=%lld       serial time=%dms\n", sum, serialtime);
	printf("speed up: %lf\n", (double)serialtime / (double)paralleltime);
	//----------------------------------------------------
}


int main() {
	int n, num_thread;
	printf("enter size of matrix and number of thread\n");
	while (scanf("%d%d", &n, &num_thread) != EOF) {
		solve(n, num_thread);
	}
	return 0;

}