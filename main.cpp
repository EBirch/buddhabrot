#include <iostream>
#include <mpi.h>
#include <vector>
#include <random>
#include <complex>
#include <algorithm>
#include <fstream>

std::vector<std::complex<double>> getPoints(std::complex<double> point, int maxIter);
void printSet(std::pair<std::vector<std::vector<int>>, int> pointList, int depth);
std::pair<std::vector<std::vector<int>>, int> translateCoords(std::vector<std::complex<double>> &pointList, int xRes, int yRes);

int main(int argc, char **argv){
	int numprocessors, rank, namelen;
	char processor_name[MPI_MAX_PROCESSOR_NAME];

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &numprocessors);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Get_processor_name(processor_name, &namelen);

	int xRes = 500;
	int yRes = 500;
	int maxPoints = 100000;
	int maxIter = 1000;
	int depth = 256;

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
	printSet(translateCoords(pointList, xRes, yRes), depth);

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

void printSet(std::pair<std::vector<std::vector<int>>, int> pointList, int depth){
	std::fstream file("buddhabrot.ppm", std::ios::out);
	file<<"P3\n"<<pointList.first.size()<<" "<<pointList.first[0].size()<<std::endl<<depth<<std::endl;
	for(auto pointRow : pointList.first){
		for(auto val : pointRow){
			val = (val * (depth - 1)) / pointList.second;
			file<<std::min(val, depth - 1)<<" "<<std::min(val*val*4/255, depth - 1)<<" "<<std::min(val*val*6/255, depth - 1)<<" ";
			file<<std::endl;
		}
	}
}

std::pair<std::vector<std::vector<int>>, int> translateCoords(std::vector<std::complex<double>> &pointList, int xRes, int yRes){
	std::vector<std::vector<int>> newCoords(yRes, std::vector<int>(xRes, 0));
	int maxHit = 0;
	for(auto &point : pointList){
		maxHit = std::max(maxHit, ++newCoords[(point.imag() + 2) * yRes / 4][(point.real() + 2) * xRes / 4]);
	}
	return std::make_pair(newCoords, maxHit);
}
