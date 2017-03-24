/*
 * main.cpp
 *
 *  Created on: 14 mar. 2017
 *      Author: manu_
 */

#include "../headers/FoodWebModel.hpp"
#include "../headers/ReadNetCDFFile.hpp"
#include <string>
#include <cstdlib>
using namespace std;
using namespace FoodWebModel;


int main(int argc, char **argv) {
	FoodWebModel::FoodWebModel foodWebModel;
	string bathymetryFile = argv[1];
	string outputFileName = argv[2];
	int simulationCycles=atoi(argv[3]);
	ReadNetCDFFile fileReader;
	fileReader.readFile(bathymetryFile);
	//foodWebModel.simulate(simulationCycles, outputFileName.c_str());
}




