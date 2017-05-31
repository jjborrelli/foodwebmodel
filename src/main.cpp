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

	/* Initialize and run the simulator*/
	FoodWebModel::FoodWebModel foodWebModel(*simArguments);
	foodWebModel.initializeParameters();
	foodWebModel.simulate(*simArguments);
	return 0;
}




