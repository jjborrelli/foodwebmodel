/*
 * main.cpp
 *
 *  Created on: 14 mar. 2017
 *      Author: manu_
 */

#include "../headers/FoodWebModel.hpp"
#include <string>
#include <cstdlib>
#include <iostream>
using namespace std;
using namespace FoodWebModel;


int main(int argc, char **argv) {
	/* Check correct number of parameters*/
	if(argc<8){
		cerr<<"Not enough input parameters to run the simulation: "<<argc<<endl;
	}
	string depthRoute = string(argv[1]);
	string depthScaleRoute = string(argv[2]);
	string initialTemperatureRoute = string(argv[3]);
	string temperatureRangeRoute=string(argv[4]);
	string outputRoute=string(argv[5]);
	string initialBiomassRoute=string(argv[6]);
	int simulationCycles=atoi(argv[7]);

	/* Initialize and run the simulator*/
	FoodWebModel::FoodWebModel foodWebModel(depthRoute, depthScaleRoute, initialTemperatureRoute, temperatureRangeRoute, initialBiomassRoute);
	foodWebModel.initializeParameters();
	foodWebModel.simulate(simulationCycles, outputRoute);
	return 0;
}




