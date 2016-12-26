#include <cstdlib>
#include <cstring>
#include <iostream>
#include <fstream>
#include "cfield.h"

#ifdef MULTITHREAD
#define MAX_THREADS 12
#endif

int main(int argc, char** argv) {
	size_t time = 0;
	size_t etalon = 0;
	if(argc < 3) {
		std::cout << "Missing arguments!" << std::endl;
		return -1;
	}
	for(int i = 1; i < argc; ++i) {
		if(strcmp(argv[i], "--time") == 0 || strcmp(argv[i], "-t") == 0)
			time = atoi(argv[++i]);
	}
	std::ofstream out_time("out.dat");
	std::ofstream out_eff("eff.dat");

#ifdef MULTITHREAD
	for(int i = 1; i < MAX_THREADS; ++i) {
		omp_set_num_threads(i);
		double begin, end;
		begin = omp_get_wtime();
#endif
		CField* field = new CField("state.dat");
		while(time != 0) {
			field->step();
			--time;
		}
		field->write_state("life.dat");
		delete field;
#ifdef MULTITHREAD
		end = omp_get_wtime();
		if(i == 1) etalon = end - begin;
		std::cout << "Num of threads: " << i << "; time: " << end - begin << std::endl;
		out_time << i << ":" << (end - begin) / etalon << std::endl;
		out_eff << i << ":" << (end - begin) / (etalon * i) << std::endl;
	}
#endif
	return 0;
}
