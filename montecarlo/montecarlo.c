#define _POSIX_C_SOURCE 200112L

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <math.h>
#include <semaphore.h>
#include <errno.h>

#define MAX_THREADS 12
#define MAX_DOTS 1000000000

double sum = 0.0;
sem_t sem;

void reset() {
	sum = 0.0;
}

void* check_dot(void* _iterations) {
	int* iterations = (int*)_iterations;
	for(int i = 0; i < *iterations; ++i) {
		double x = (double)(rand() % 314) / 100;
		double y = (double)(rand() % 100) / 100;
		if(y <= sin(x)) {
			sem_wait(&sem);
			sum += x * y;
			sem_post(&sem);
		}
	}
	return NULL;
}

double run(int threads_num) {
	struct timespec begin, end;
	double elapsed;
	pthread_t threads[threads_num];
	int iters = MAX_DOTS / threads_num;
	for(int i = 0; i < threads_num; ++i) {
		pthread_create(&threads[i], NULL, &check_dot, (void*)&iters);
	}
	if(clock_gettime(CLOCK_REALTIME, &begin) == -1) {
		perror("Unable to get time");
		exit(-1);
	}
	for(int i = 0; i < threads_num; ++i) {
		pthread_join(threads[i], NULL);
	}
	if(clock_gettime(CLOCK_REALTIME, &end) == -1) {
		perror("Unable to get time");
		exit(-1);
	}
	elapsed = end.tv_sec - begin.tv_sec;
	elapsed += (end.tv_nsec - begin.tv_nsec) / 1000000000.0;
	return elapsed;
}

int main(int argc, char** argv) {
	if(argc < 2) {
		fprintf(stderr, "You should provide the name of the output file.\n");
		exit(-1);
	}
	sem_init(&sem, 0, 1);
	FILE* fd = fopen(argv[1], "w");
	if(!fd) {
		perror("Unable to open file");
		exit(-1);
	}
	srand(time(NULL));
	double worst_time = run(1);
	double result = (3.14 / MAX_DOTS) * sum;
	reset();
	fprintf(fd, "Result: %f\n", result); 
	for(int i = 2; i <= MAX_THREADS; ++i) {
		double time = run(i);
		double accel = time / worst_time;
		fprintf(fd, "%d:%f\n", i, accel);
		reset();
	}
	fclose(fd);
	sem_destroy(&sem);
	return 0;
}
