#include <mpi.h>
#include <stdlib.h>
#include <stdio.h>

int sum(int* data, int size) {
	return ((*data + *(data + size - 1)) * size) / 2;
}

int main(int argc, char** argv) {
	int N = 0, process_num = 0, rank = 0;
	int* array;
	MPI_Status status;

	N = atoi(argv[1]);

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &process_num);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	int chunk = N / (process_num - 1);
	if(rank == 0) {
		array = (int*)malloc(N * sizeof(int));
		int results[process_num - 1];
		for(int i = 0; i < N; ++i) array[i] = i;
		printf("\n");
		for(int i = 0; i < process_num - 2; ++i) MPI_Send(array + i * chunk, chunk, MPI_INT, i + 1, 1, MPI_COMM_WORLD);
		MPI_Send(array + (process_num - 2) * chunk, N - (process_num - 2) * chunk, MPI_INT, process_num - 1, 1, MPI_COMM_WORLD);
		int control_sum = sum(array, N);
		for(int i = 0; i < process_num - 1; ++i)
			MPI_Recv(&results[i], 1, MPI_INT, i + 1, 1, MPI_COMM_WORLD, &status);
		for(int i = 1; i < process_num - 1; ++i) results[0] += results[i];
		printf("Control sum: %d, sum from processes: %d\n", control_sum, results[0]); 
		free(array);
	}
	if(rank != 0) {
		int recv_size = 0;
		if(rank != process_num - 1) recv_size = chunk;
		else recv_size = N - (process_num - 2) * chunk;
		array = (int*)malloc(recv_size * sizeof(int));
		MPI_Recv(array, recv_size, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);
		int res = sum(array, recv_size);
		printf("Result from process %d: %d\n", rank, res);
		MPI_Send(&res, 1, MPI_INT, 0, 1, MPI_COMM_WORLD);
		free(array);
	}
	MPI_Finalize();

	return 0;
}
