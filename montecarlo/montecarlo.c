#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <math.h>
#include <semaphore.h>

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
	sem_wait(&sem);
	if(all_dots_num >= MAX_DOTS) {
		sem_post(&sem);
		return NULL;
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
	return NULL;
}

double run(int threads_num) {
	struct timespec begin, end;
	double elapsed;
	clock_gettime(CLOCK_REALTIME, &begin);

	pthread_t threads[threads_num];
	for(int i = 0; i < threads_num; ++i) {
		pthread_create(&threads[i], NULL, check_dot, NULL);
	}
	for(int i = 0; i < threads_num; ++i) {
		pthread_join(&threads[i], NULL);
	}

	clock_gettime(CLOCK_REALTIME, &end);
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
	FILE* fd = fopen(argv[1], 'w');
	double worst_time = run(1);
	double result = 3.14 * sum / good_dots_num;
	reset();
	fprintf(fd, "Result: %f", result); 
	for(int i = 2; i <= MAX_THREADS; ++i) {
		double time = run(i);
		double accel = worst_time/time;
		fprintf("%d:%f", i, accel);
		reset();
	}
	fclose(fd);
	sem_destroy(&sem);
	return 0;
}
