#include <cstdlib>
#include <cstring>
#include <iostream>
#include "cfield.h"

int main(int argc, char** argv) {
	size_t time = 0;
	if(argc < 3) {
		std::cout << "Missing arguments!" << std::endl;
		return -1;
	}
	for(int i = 1; i < argc; ++i) {
		if(strcmp(argv[i], "--time") == 0 || strcmp(argv[i], "-t") == 0) time = atoi(argv[++i]);
	}
	CField* field = new CField("state.dat");
	while(time != 0) {
		field->step();
		--time;
	}
	field->write_state("life.dat");
	delete field;

	return 0;
}
