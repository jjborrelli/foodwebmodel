/*
 * main.cpp
 *
 *  Created on: 14 mar. 2017
 *      Author: manu_
 */

//#include "../headers/FoodWebModel.hpp"
#include <string>
#include <cstdlib>
#include <iostream>
using namespace std;
//using namespace FoodWebModel;


int main(int argc, char **argv) {
	//FoodWebModel::FoodWebModel foodWebModel;
	string bathymetryFile = string(argv[1]);
	string outputFileName = string(argv[2]);
	int simulationCycles=atoi(argv[3]);
	int loops = 10;
	for (int i = 0; i < loops; i++) {
		std::cout << "loop number" << i << endl;
	}
	std::cout << "All done" << endl;
	return 0;
	//foodWebModel.simulate(simulationCycles, outputFileName.c_str());
}




