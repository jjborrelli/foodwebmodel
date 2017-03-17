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


int main(int argc, char **argv) {
	FoodWebModel::FoodWebModel foodWebModel;
	string outputFileName = argv[1];
	int simulationCycles=atoi(argv[2]);
	foodWebModel.simulate(simulationCycles, outputFileName.c_str());
}




