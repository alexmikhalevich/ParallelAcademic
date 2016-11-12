#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <string.h>
#include <stdbool.h>

#define EPS 0.0001
#define MAX_THREADS 12

int chunks = 0;

double func(double x) {
	return 1 / (1 + x * x);
}

int main(int argc, char** argv)  {
	bool reduction =  false;
	if(argc < 3) {
		printf("You should specify the number of chunks");
		return 0;
	}
	for(int i = 1; i < argc; ++i) {
		if(strcmp(argv[i], "--chunks") == 0) chunks = atoi(argv[++i]);
		if(strcmp(argv[i], "--reduction") == 0) reduction = true;
	}

	double step = 1 / (double)chunks;
	double begin, end;
	for(int i = 1; i < MAX_THREADS; ++i) {
		begin = omp_get_wtime(); 
		double res = 0.0;
		if(reduction) {
			#pragma omp parallel reduction (+:res)
			{
				#pragma omp for
				for(unsigned int i = 1; i < chunks; ++i) res += func(step * i);
			}
		}
		else {
			#pragma omp parallel
			{
				#pragma omp for 
				for(unsigned int i =  0; i < chunks; ++i) {
					#pragma omp critical 
					{
						res += func(step * i);
					}
				}
			}
		}
		res += (func(0) + func(1)) / 2;
		res *= 4 * step;
		end = omp_get_wtime();
		printf("Num of threads: %d, result: %f, time: %f\n", i, res, end - begin);
	}

	return 0;
}

