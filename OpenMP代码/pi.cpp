#include <stdio.h>
#include <omp.h>
#include <iostream>
#include <ctime>

using namespace std;
#define NUM_STEPS 100000000

int main (int argc, char **argv) {
	int n_threads;
	cin >> n_threads;
	while (n_threads) {
		omp_set_dynamic(0);     // Explicitly disable dynamic teams
		omp_set_num_threads(n_threads);
		cout << "using" << n_threads << " threads now" << endl;
		long int i, num_steps = NUM_STEPS;
		double x, step, sum, pi;
		sum = 0.0;
		step = 1.0 / (double) num_steps;
		clock_t begin = clock();
		#pragma omp parallel private(i,x)
		{
			#pragma omp for reduction(+:sum)
			for (i = 0; i < num_steps; ++i) {
				x = (i + 0.5) * step;
				sum += 4.0 / (1.0 + x * x);

			}
		}
		clock_t end = clock();
		/* All threads join master thread and terminate */
		pi = step * sum;
		printf("PI value = %.24f\n", pi);
		double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
		printf("%fs consumed\n", elapsed_secs);
		cin >> n_threads;
	}
}