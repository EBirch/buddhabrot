#include <iostream>
#include <mpi.h>
#include <vector>
#include <random>
#include <complex>
#include <algorithm>
#include <fstream>
#include <future>

std::vector<double> getPoints(std::complex<double> point, int maxIter);
void printSet(std::pair<std::vector<std::vector<int>>, int> pointList, int depth, std::string path);
std::pair<std::vector<std::vector<int>>, int> translateCoords(std::vector<double> &pointList, int xRes, int yRes);

int main(int argc, char **argv){
	int processors, rank;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &processors);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	int xRes = 500;
	int yRes = 500;
	int maxPoints = 100000;
	int maxIter = 1000;
	int depth = 256;

	std::mt19937 random(rank);
	std::uniform_real_distribution<> realGen(-2.5, 1);
	std::uniform_real_distribution<> complexGen(-1, 1);
	std::vector<double> pointList;
	std::vector<std::future<bool>> futures;
	std::mutex m, l;
	int count = 0;

	for(int i = 0; i < 8; ++i){
		futures.push_back(std::async(std::launch::async, 
			[&]()->bool{
				while(true){
					{
						std::lock_guard<std::mutex> guard(m);
						++count;
					}
					if(count >= maxPoints){
						return true;
					}
					auto tempList = getPoints(std::complex<double>(realGen(random), complexGen(random)), maxIter);
					{
						std::lock_guard<std::mutex> guard(l);
						if(tempList.size()){
							pointList.insert(pointList.end(), tempList.begin(), tempList.end());
						}
					}
				}
			}));
	}

	for(auto &fut : futures){
		fut.get();
	}	

	std::cout<<"All threads done on process "<<rank<<std::endl;

	if(rank == 0){
		for(int i = 1; i < processors; ++i){
			std::cout<<"Getting stuff from "<<i<<std::endl;
			int size = 0;
			MPI_Recv(&size, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			std::cout<<"Got size "<<size<<" from "<<i<<std::endl;
			std::vector<double> temp(size);
			MPI_Recv(&temp[0], size, MPI_DOUBLE, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			pointList.insert(pointList.end(), temp.begin(), temp.end());
			std::cout<<"Got points from "<<i<<std::endl;
		}
	}
	else{
		int size = pointList.size();
		std::cout<<rank<<" sending size "<<size<<" to 0"<<std::endl;
		MPI_Send(&size, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
		std::cout<<rank<<" sent size to 0\n";
		MPI_Send(&pointList[0], size, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
		std::cout<<rank<<" sent points to 0\n";
	}

	std::cout<<"Done MPI send/recv\n";
	
	if(rank == 0){
		printSet(translateCoords(pointList, xRes, yRes), depth, "/home/A01514050/buddhabrot/buddhabrot.ppm");
	}

	MPI_Finalize();
}

std::vector<double> getPoints(std::complex<double> point, int maxIter){
	std::complex<double> temp(point);
	std::vector<double> pointsList;
	int iter = 0;
	while(temp.real() * temp.real() + temp.imag() * temp.imag() < 4 && iter++ < maxIter){
		pointsList.push_back(temp.real());
		pointsList.push_back(temp.imag());
		temp = temp * temp + point;
	}
	return (iter == maxIter) ? std::vector<double>() : pointsList;
}

void printSet(std::pair<std::vector<std::vector<int>>, int> pointList, int depth, std::string path){
	std::fstream file(path.data(), std::ios::out);
	file<<"P3\n"<<pointList.first.size()<<" "<<pointList.first[0].size()<<std::endl<<depth<<std::endl;
	double scale = double(255 * 2.5) / pointList.second;
	for(auto pointRow : pointList.first){
		for(auto val : pointRow){
			file<<0<<" "<<(val * scale > 255 ? 255 : int(val * scale))<<" "<<(val * scale > 255 ? 255 : int(val * scale))<<" ";
		}
		file<<std::endl;
	}
}

std::pair<std::vector<std::vector<int>>, int> translateCoords(std::vector<double> &pointList, int xRes, int yRes){
	std::vector<std::vector<int>> newCoords(yRes, std::vector<int>(xRes, 0));
	int maxHit = 0;
	for(int i = 0; i < pointList.size() - 1; i += 2){
		maxHit = std::max(maxHit, ++newCoords[(pointList[i + 1] + 2) * yRes / 4][(pointList[i] + 2) * xRes / 4]);
	}
	return std::make_pair(newCoords, maxHit);
}
