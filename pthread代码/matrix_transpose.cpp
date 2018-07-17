#include <stdio.h>
#include <math.h>
#include <pthread.h>
#include <ctime>
#include <iostream>
using namespace std;
#define N 10000
int A[N][N], B[N][N];
pthread_mutex_t gLock;
int NUMTHREADS;// == 4
int each_size ;

void *Transpose(void *pArg) {
	int myNum = *((int *)pArg);
	int start = myNum * each_size;
	int end = (myNum + 1) * each_size;
	// use every NUMTHREADS-th ste
	for (int i = start; i < end; i += 1) {
		for (int j = 0; j < N; j++) {
			B[i][j] = A[j][i];
			// cout << i << " " << j << endl;
		}
	}
	return 0;
}

int main(int argc, char **argv) {
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

	cout << "enter number of thread ( require that N can be divided by number of threads" << endl;
	cin >> NUMTHREADS;
	each_size = N / NUMTHREADS;
	clock_t start = clock();
	pthread_t tHandles[NUMTHREADS];
	int tNum[NUMTHREADS];
	pthread_mutex_init(&gLock, NULL);
	for ( i = 0; i < NUMTHREADS; ++i ) {
		tNum[i] = i;
		pthread_create(&tHandles[i],           // Returned thread handle
		               NULL,                 // Thread Attributes
		               Transpose,                   // Thread function
		               (void*) &tNum[i]);        // Data for Transpose()
	}
	for ( i = 0; i < NUMTHREADS; ++i ) {
		pthread_join(tHandles[i], NULL);
	}
	pthread_mutex_destroy(&gLock);
	clock_t end = clock();
	cout << "time consumed: " << end - start << "s with " << NUMTHREADS << " threads" << endl;
	return 0;
}