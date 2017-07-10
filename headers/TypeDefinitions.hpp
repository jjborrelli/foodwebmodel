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
	outputPredatorRoute,
	outputPhysicalRoute,
	outputParameterRoute,
	outputAssertionViolationRoute,
	initialAlgaeBiomassRoute,
	initialZooplanktonCountRoute,
	lightAtSurfaceRoute,
	biomassBaseDifferentialRoute,
	phosphorusConcentrationAtBottomRoute,
	zooplanktonBiomassDepthCenterRoute;
	unsigned int simulationCycles;

	biomassType algae_biomass_differential_production_scale,
	algal_carrying_capacity_coefficient,
	algal_carrying_capacity_intercept,
	maximum_found_algal_biomass, algal_respiration_at_20_degrees,
	exponential_temperature_algal_respiration_coefficient,
	intrinsic_algae_mortality_rate, maximum_algae_resources_death;

	biomassType	grazer_base_mortality_proportion, grazer_filtering_rate_per_individual,
	grazer_basal_respiration_weight,
	grazer_k_value_respiration,
	grazer_carrying_capacity_coefficient,
	grazer_carrying_capacity_intercept,
	grazer_maximum_found_biomass;

	biomassType	predator_base_mortality_proportion, predator_filtering_rate_per_individual,
	predator_basal_respiration_weight,
	predator_k_value_respiration,
	predator_carrying_capacity_coefficient,
	predator_carrying_capacity_intercept,
	predator_maximum_found_biomass;

	physicalType light_steepness, diatom_attenuation_coefficient, limitation_scale_weight,
	phosphorus_half_saturation,
	light_allowance_weight;
} SimulationArguments;

#endif /* TYPEDEFINITIONS_HPP_ */
