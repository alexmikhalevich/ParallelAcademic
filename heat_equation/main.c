#include <mpi.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define tau 1

size_t plate_width = 0;
size_t plate_height = 0;
double u_0 = 0.0;
double u_l = 0.0;
double u_r = 0.0;
int process_num = 0, rank = 0;
MPI_Status status;
const double mult = 1.177749;

struct s_plate {
	double** plate_chunk;
	size_t size;
};

struct s_plate* init_plate(size_t width) {
	struct s_plate* res = (struct s_plate*)malloc(sizeof(struct s_plate));
	double** plate_chunk = (double**)malloc(width * sizeof(double*));
	for(size_t i = 0; i < width; ++i) 
		plate_chunk[i] = (double*)malloc(plate_height * sizeof(double));
	res->plate_chunk = plate_chunk;
	res->size = width;
	return res;
}

void delete_plate(struct s_plate* plate) {
	for(size_t i = 0; i < plate->size; ++i) free(plate->plate_chunk[i]);
	free(plate->plate_chunk);
	free(plate);
}

void substep(struct s_plate* last, struct s_plate* output) {
	output = init_plate(last->size);
	double* boundary_data = (double*)malloc(plate_height * sizeof(double));
	for(size_t i = 1; i < last->size - 1; ++i) 
		for(size_t j = 0; j < plate_height; ++j) 
			output->plate_chunk[i][j] = last->plate_chunk[i][j] + mult * tau * 
				(last->plate_chunk[i + 1][j] - 2 * last->plate_chunk[i][j]  + last->plate_chunk[i - 1][j]);
	if(rank % 2 == 0) {
		if(rank != 0) MPI_Send(last->plate_chunk[0], plate_height, MPI_DOUBLE, rank - 1, 1, MPI_COMM_WORLD);
		if(rank != process_num - 1) MPI_Send(last->plate_chunk[last->size - 1], plate_height, MPI_DOUBLE, 
				                     rank + 1, 1, MPI_COMM_WORLD);
		if(rank != 0) {
			MPI_Recv(boundary_data, plate_height, MPI_DOUBLE, rank - 1, 1, MPI_COMM_WORLD, &status);
			for(size_t j = 0; j < plate_height; ++j) 
				output->plate_chunk[0][j] = last->plate_chunk[0][j] + mult * tau * 
					(last->plate_chunk[1][j] - 2 * last->plate_chunk[0][j]  + boundary_data[j]);
		}
		if(rank != process_num - 1) {
			size_t l = last->size - 1;
			MPI_Recv(boundary_data, plate_height, MPI_DOUBLE, rank + 1, 1, MPI_COMM_WORLD, &status);
			for(size_t j = 0; j < plate_height; ++j) 
				output->plate_chunk[l][j] = last->plate_chunk[l][j] + mult * tau * 
					(boundary_data[j] - 2 * last->plate_chunk[l][j]  + last->plate_chunk[l - 1][j]);
		}
	}
	else {
		MPI_Recv(boundary_data, plate_height, MPI_DOUBLE, rank - 1, 1, MPI_COMM_WORLD, &status);
		for(size_t j = 0; j < plate_height; ++j) 
			output->plate_chunk[0][j] = last->plate_chunk[0][j] + mult * tau * 
				(last->plate_chunk[1][j] - 2 * last->plate_chunk[0][j]  + boundary_data[j]);
		if(rank != process_num - 1) {
			size_t l = last->size - 1;
			MPI_Recv(boundary_data, plate_height, MPI_DOUBLE, rank + 1, 1, MPI_COMM_WORLD, &status);
			for(size_t j = 0; j < plate_height; ++j) 
				output->plate_chunk[l][j] = last->plate_chunk[l][j] + mult * tau * 
					(boundary_data[j] - 2 * last->plate_chunk[l][j]  + last->plate_chunk[l - 1][j]);
		}
		MPI_Send(last->plate_chunk[0], plate_height, MPI_DOUBLE, rank - 1, 1, MPI_COMM_WORLD);
		if(rank != process_num - 1) MPI_Send(last->plate_chunk[last->size - 1], plate_height, MPI_DOUBLE, 
				                     rank + 1, 1, MPI_COMM_WORLD);
	}
	free(boundary_data);
}

void step(struct s_plate* plate) {
	struct s_plate* v_plate;
	substep(plate, v_plate);
	if(rank == 0)
		for(size_t j = 0; j < plate_width; ++j) plate->plate_chunk[0][j] = u_l;
	if(rank == process_num - 1)
		for(size_t j = 0; j < plate_width; ++j) plate->plate_chunk[plate->size - 1][j] = u_r;
	for(size_t i = 0; i < plate->size; ++i) {
		for(size_t j = 0; j < plate_height; ++j) {
			double tmp_a = ((j == plate->size - 1) ? 0.0 : v_plate->plate_chunk[i][j + 1]);
			double tmp_b = ((j == 0) ? 0.0 : v_plate->plate_chunk[i][j - 1]);
			plate->plate_chunk[i][j] = v_plate->plate_chunk[i][j] + mult * tau * 
				(tmp_a - 2 * v_plate->plate_chunk[i][j] + tmp_b); 
		}
	}
	delete_plate(v_plate);
}

void get_temp_distribution(struct s_plate* plate, size_t time) {
	for(size_t i = 0; i < plate->size; ++i) 
		for(size_t j = 0; j < plate_height; ++j) plate->plate_chunk[i][j] = u_0;
	for(size_t t = 0; t < time; ++t) step(plate);
}

int main(int argc, char** argv) {
	size_t time = 0;
	if(argc < 11) {
		printf("Usage: ./a.out -h <plate height> -w <plate width> -u0 <initial temperature>");
		printf("-ul <left boundary temperature> -ur <right boundary temperature>\n");
	}
	for(size_t i = 1; i < argc; ++i) {
		if(argv[i] == "-h") plate_height = atoi(argv[++i]);
		if(argv[i] == "-w") plate_width = atoi(argv[++i]);
		if(argv[i] == "-u0") u_0 = atof(argv[++i]);
		if(argv[i] == "-ul") u_l = atof(argv[++i]);
		if(argv[i] == "-ur") u_r = atof(argv[++i]);
	}
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &process_num);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	//struct s_plate* plate_chunk;
	size_t chunk = 0;
	if(rank != process_num - 1) chunk = plate_width / process_num;
	else chunk = plate_width / process_num + plate_width - (plate_width / process_num) * process_num;
	struct s_plate* plate = init_plate(chunk);
	get_temp_distribution(plate, time);
	delete_plate(plate);
	MPI_Finalize();
	return 0;
}
