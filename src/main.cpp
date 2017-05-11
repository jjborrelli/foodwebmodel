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
	string outputAlgaeRoute=string(argv[5]);
	string outputSloughRoute=string(argv[6]);
	string outputGrazerRoute=string(argv[7]);
	string initialAlgaeBiomassRoute=string(argv[8]);
	string initialZooplanktonCountRoute=string(argv[9]);
	string lightAtSurfaceRoute=string(argv[10]);
	int simulationCycles=atoi(argv[11]);

	/* Initialize and run the simulator*/
	FoodWebModel::FoodWebModel foodWebModel(depthRoute, depthScaleRoute, initialTemperatureRoute, temperatureRangeRoute, initialAlgaeBiomassRoute, initialZooplanktonCountRoute, lightAtSurfaceRoute);
	foodWebModel.initializeParameters();
	foodWebModel.simulate(simulationCycles, outputAlgaeRoute, outputSloughRoute, outputGrazerRoute);
	return 0;
}




