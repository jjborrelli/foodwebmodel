/*
 * TypeDefinitions.hpp
 *
 *  Created on: 28 mar. 2017
 *      Author: manu_
 */

#ifndef TYPEDEFINITIONS_HPP_
#define TYPEDEFINITIONS_HPP_



typedef double biomassType;
typedef double physicalType;
typedef unsigned int zooplanktonCountType;

#include <string>

using namespace std;

typedef struct {
	string depthRoute,
	depthScaleRoute,
	initialTemperatureRoute,
	temperatureRangeRoute,
	outputAlgaeRoute,
	outputSloughRoute,
	outputGrazerRoute,
	initialAlgaeBiomassRoute,
	initialZooplanktonCountRoute,
	lightAtSurfaceRoute,
	biomassBaseDifferential,
	phosphorusConcentrationAtBottom,
	zooplanktonBiomassDepthCenter;
	unsigned int simulationCycles;
} SimulationArguments;

#endif /* TYPEDEFINITIONS_HPP_ */
