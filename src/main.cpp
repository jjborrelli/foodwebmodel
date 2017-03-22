/*
 * main.cpp
 *
 *  Created on: 14 mar. 2017
 *      Author: manu_
 */

#include "../headers/FoodWebModel.hpp"
#include <string>
#include <cstdlib>
using namespace std;
using namespace FoodWebModel;


int main(int argc, char **argv) {
	FoodWebModel::FoodWebModel foodWebModel;
	string bathymetryFile = argv[1];
	string outputFileName = argv[2];
	int simulationCycles=atoi(argv[3]);
	ReadGDALFile fileReader;
	fileReader.readGDALFile(bathymetryFile);
	//foodWebModel.simulate(simulationCycles, outputFileName.c_str());
}




