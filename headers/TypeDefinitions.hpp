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
	outputParameterRoute,
	ouputAssertionViolationRoute,
	initialAlgaeBiomassRoute,
	initialZooplanktonCountRoute,
	lightAtSurfaceRoute,
	biomassBaseDifferential,
	phosphorusConcentrationAtBottom,
	zooplanktonBiomassDepthCenter;
	unsigned int simulationCycles;
	biomassType algae_biomass_differential_production_scale, animal_base_mortality_proportion, filtering_rate_per_daphnia;
} SimulationArguments;

#endif /* TYPEDEFINITIONS_HPP_ */
