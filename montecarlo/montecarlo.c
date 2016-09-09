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

int good_dots_num = 0;
int all_dots_num = 0;
double sum = 0.0;
sem_t sem;

void reset() {
	good_dots_num = 0;
	all_dots_num = 0;
	sum = 0.0;
}

void* check_dot() {
	while(true) {
		sem_wait(&sem);
		if(all_dots_num >= MAX_DOTS) {
			sem_post(&sem);
			break;
		}
		sem_post(&sem);
	
		srand(time(NULL));
		double x = (rand() % 314) / 100;
		double y = (rand() % 100) / 100;
		if(y <= sin(x)) {
			sem_wait(&sem);
			++good_dots_num;
			++all_dots_num;
			sum += x * y;
			sem_post(&sem);
		}
		else {
			sem_wait(&sem);	
			++all_dots_num;
			sem_post(&sem);
		}
	}
	return NULL;
}

double run(int threads_num) {
	struct timespec begin, end;
	double elapsed;
	if(clock_gettime(CLOCK_REALTIME, &begin) == -1) {
		perror("Unable to get time");
		exit(-1);
	}
	pthread_t threads[threads_num];
	for(int i = 0; i < threads_num; ++i) {
		pthread_create(&threads[i], NULL, &check_dot, NULL);
	}
	for(int i = 0; i < threads_num; ++i) {
		pthread_join(threads[i], NULL);
	}
	if(clock_gettime(CLOCK_REALTIME, &end) == -1) {
		perror("Unable to get time");
		exit(-1);
	}
	printf("threads = %d\ngood_dots_num = %d, all_dots_num = %d, sum = %f\n\n", threads_num, good_dots_num, all_dots_num, sum);
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
	double worst_time = run(1);
	double result = 3.14 * sum / good_dots_num;
	reset();
	fprintf(fd, "Result: %f\n", result); 
	for(int i = 2; i <= MAX_THREADS; ++i) {
		double time = run(i);
		double accel = worst_time/time;
		fprintf(fd, "%d:%f\n", i, accel);
		reset();
	}
	fclose(fd);
	sem_destroy(&sem);
	return 0;
}
