#include <iostream>
#include <mpi.h>
#include <vector>
#include <random>
#include <complex>

std::vector<std::complex<double>> getPoints(std::complex<double> point, int maxIter);

int main(int argc, char **argv){
	int numprocessors, rank, namelen;
	char processor_name[MPI_MAX_PROCESSOR_NAME];

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &numprocessors);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Get_processor_name(processor_name, &namelen);

	int maxPoints = 100;
	int maxIter = 10;

	std::mt19937 random(rank);
	std::uniform_real_distribution<> realGen(-2.5, 1);
	std::uniform_real_distribution<> complexGen(-1, 1);
	std::vector<std::complex<double>> pointList;

	for(int i = 0 ; i < maxPoints ; ++i){
		auto tempList = getPoints(std::complex<double>(realGen(random), complexGen(random)), maxIter);
		if(tempList.size()){
			pointList.insert(pointList.end(), tempList.begin(), tempList.end());
		}
	}

	std::cout<<"End size: "<<pointList.size()<<std::endl;

	MPI_Finalize();
}

std::vector<std::complex<double>> getPoints(std::complex<double> point, int maxIter){
	std::complex<double> temp(point);
	std::vector<std::complex<double>> pointsList;
	int iter = 0;
	while(temp.real() * temp.real() + temp.imag() * temp.imag() < 4 && iter++ < maxIter){
		pointsList.push_back(temp);
		temp = temp * temp + point;
	}
	return (iter == maxIter) ? std::vector<std::complex<double>>() : pointsList;
}
