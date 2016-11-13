#include <cstdlib>
#include <iostream>
#include "cfield.h"

int main(int argc, char** argv) {
	size_t time = 0;
	if(argc < 3) {
		std::cout << "Missing arguments!" << std::endl;
		return -1;
	}
	for(int i = 1; i < argc; ++i) {
		if(argv[i] == "--time" || argv[i] == "-t") time = atoi(argv[++i]);
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
