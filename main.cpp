#include <iostream>
#include <mpi.h>
#include <vector>
#include <random>
#include <complex>

int main(int argc, char **argv){
	int numprocessors, rank, namelen;
	char processor_name[MPI_MAX_PROCESSOR_NAME];

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &numprocessors);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Get_processor_name(processor_name, &namelen);

	// srand(rank);
	std::mt19937 random(rank);
	std::uniform_real_distribution<> realGen(-2.5, 1);
	std::uniform_real_distribution<> complexGen(-1, 1);

	std::complex<double> point(realGen(random), complexGen(random));

	if ( rank == 0 ){
		std::cout << "Processor name: " << processor_name << "\n";
		std::cout << "master (" << rank << "/" << numprocessors << ")\n";
	} 
	else {
		std::cout << "slave  (" << rank << "/" << numprocessors << ")\n";
	}
	MPI_Finalize();
	return 0;
}
