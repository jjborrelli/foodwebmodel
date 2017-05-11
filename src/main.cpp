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

#include "../headers/ParameterReader.hpp"
using namespace std;
using namespace FoodWebModel;


int main(int argc, char **argv) {

	/* Read file containing the parameters*/
	FoodWebModel::ParameterReader reader;
	reader.readSimulationParameters(argv[1]);
	SimulationArguments *simArguments =&reader.simArguments;
	/* Set parameters values*/

	string depthRoute = simArguments->depthRoute;
	string depthScaleRoute = simArguments->depthScaleRoute;
	string initialTemperatureRoute = simArguments->initialTemperatureRoute;
	string temperatureRangeRoute= simArguments->temperatureRangeRoute;
	string outputAlgaeRoute = simArguments->outputAlgaeRoute;
	string outputSloughRoute = simArguments->outputSloughRoute;
	string outputGrazerRoute = simArguments->outputGrazerRoute;
	string initialAlgaeBiomassRoute = simArguments->initialAlgaeBiomassRoute;
	string initialZooplanktonCountRoute = simArguments->initialZooplanktonCountRoute;
	string lightAtSurfaceRoute = simArguments->lightAtSurfaceRoute;
	int simulationCycles =  simArguments->simulationCycles;

	/* Initialize and run the simulator*/
	FoodWebModel::FoodWebModel foodWebModel(depthRoute, depthScaleRoute, initialTemperatureRoute, temperatureRangeRoute, initialAlgaeBiomassRoute, initialZooplanktonCountRoute, lightAtSurfaceRoute);
	foodWebModel.initializeParameters();
	foodWebModel.simulate(simulationCycles, outputAlgaeRoute, outputSloughRoute, outputGrazerRoute);
	return 0;
}




