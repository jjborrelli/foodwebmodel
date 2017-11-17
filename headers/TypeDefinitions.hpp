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
typedef long long int animalCountType;
typedef long int cohortIDType;

#include <string>
#include <iostream>
#include "SimulationModes.hpp"
//#include "ModelConstants.hpp"

using namespace std;

typedef struct {
	std::string depthRoute,
	depthScaleRoute,
	initialTemperatureRoute,
	temperatureRangeRoute,
	temperatureDepthProportionRoute,
	temperatureAtTimeRoute,
	outputAlgaeRoute,
	outputSloughRoute,
	outputGrazerRoute,
	outputGrazerTraceRoute,
	outputPredatorRoute,
	outputPredatorTraceRoute,
	outputPhysicalRoute,
	outputParameterRoute,
	outputAssertionViolationRoute,
	initialAlgaeBiomassRoute,
	initialZooplanktonCountRoute,
	initialZooplanktonDistributionRoute,
	initialZooplanktonWeightRoute,
	lightAtSurfaceRoute,
	lightAtSurfaceYearRoute,
	biomassBaseDifferentialRoute,
	phosphorusConcentrationAtBottomRoute,
	nitrogenConcentrationAtBottomRoute,
	zooplanktonBiomassDepthCenterRoute;

	unsigned int simulationCycles;

	physicalType phosphorous_weight, decaying_phosphorus_factor, retained_phosphorus_factor, light_allowance_proportion, nutrient_derivative, nutrient_growth;
	physicalType light_lower_quantile, light_upper_quantile, light_steepness_factor;

	biomassType algae_biomass_differential_production_scale,
	algal_carrying_capacity_coefficient,
	algal_carrying_capacity_intercept,
	maximum_found_algal_biomass, algal_respiration_at_20_degrees,
	exponential_temperature_algal_respiration_coefficient,
	intrinsic_algae_mortality_rate, maximum_algae_resources_death,
	reabsorbed_algal_nutrients_proportion,
	algal_mortality_scale,
	intrinsic_settling_rate,
	algal_fraction_sloughed;

/*Movement of dead biomass*/
	biomassType wash_up_dead_biomass_proportion, wash_down_dead_biomass_proportion, algae_biomass_conservation_factor;

	biomassType	grazer_base_mortality_proportion, grazer_filtering_rate_per_individual,
	grazer_filtering_length_coefficient, grazer_filtering_length_exponent,
	grazer_filtering_coefficient, grazer_filtering_exponent,
	grazer_basal_respiration_weight,
	grazer_k_value_respiration,
	grazer_carrying_capacity_coefficient,
	grazer_carrying_capacity_intercept,
	grazer_carrying_capacity_amplitude,
	grazer_carrying_capacity_constant,
	grazer_dead_animals_per_lost_biomass_and_concentration,
	grazer_maximum_found_biomass,
	grazer_food_starvation_threshold,
	grazer_egg_allocation_threshold,
	grazer_starvation_factor,
	grazer_dead_animal_proportion,
	grazer_reabsorbed_animal_nutrients_proportion,
	grazer_consumption_temperature_factor,
	grazer_velocity_downward_pull,
	grazer_critical_depth,
	grazer_critical_light_intensity;
	double grazer_migration_consumption;

	physicalType grazer_light_optimal_value, grazer_light_migration_weight;

	double grazer_reproduction_proportion_investment_amplitude,
	grazer_reproduction_proportion_investment_coefficient,
	grazer_reproduction_proportion_investment_intercept,
	grazer_reproduction_proportion_investment_constant,
	grazer_reproduction_proportion_investment_multiplier;

	unsigned int grazer_max_hours_without_food, grazer_maximum_age_in_hours,
	grazer_random_seed, grazer_incubation_hours, grazer_ovipositing_period, grazer_maturation_hours, grazer_agglomeration_cohort_threshold;

	int grazer_layer_center_index;

	biomassType	predator_base_mortality_proportion, predator_filtering_rate_per_individual,
	predator_basal_respiration_weight,
	predator_k_value_respiration,
	predator_carrying_capacity_coefficient,
	predator_carrying_capacity_intercept,
	predator_maximum_found_biomass;

	physicalType light_steepness, diatom_attenuation_coefficient, limitation_scale_weight,
	phosphorus_half_saturation,
	phosphorus_functional_factor,
	phosphorus_functional_constant_response_1,
	phosphorus_functional_constant_response_2,
	phosphorus_functional_step_1,
	nitrogen_half_saturation,
	nitrogen_functional_constant_response,
	nitrogen_functional_step,
	nitrogen_phosphorus_lower_bound,
	nitrogen_phosphorus_upper_bound,
	light_allowance_weight;

	physicalType temperature_optimal, temperature_steepness, temperature_suppression_steepness, temperature_max;

	unsigned int predator_random_seed;
} SimulationArguments;

/* Types for individual-based dynamics of animals*/
#ifdef INDIVIDUAL_BASED_ANIMALS
typedef enum {Egg=0, Juvenile=1, Mature=2} AnimalStage;
typedef enum {None=0, Starvation=1, Senescence=2, Other=3} causeOfDeath;

typedef struct {
	mutable int x, y, ageInHours, latestMigrationIndex;
	/* The relative difference between the depth of the center of mass and the depth of the cohort */
	int migrationConstant;
	mutable AnimalStage stage;
//	causeOfDeath death;
	/* If the animal is a bottom feeder, moves upwards and has matured in the current cycle*/
	bool isBottomAnimal, upDirection, justMatured;
	mutable animalCountType numberOfIndividuals;
	mutable biomassType bodyBiomass, gonadBiomass, starvationBiomass;
	cohortIDType cohortID;
} AnimalCohort;

typedef struct {
	 int x, y;
	 mutable int ageInHours;
	 /* The relative difference between the depth of the center of mass and the depth of the cohort */
	 int migrationConstant;
	cohortIDType cohortID;
	bool isBottomAnimal;
	mutable int hasHatched;
	animalCountType numberOfEggs;
	biomassType biomass;
} EggCohort;

std::ostream& operator<<(std::ostream& os, const AnimalCohort& cohort);
std::ostream& operator<<(std::ostream& os, const EggCohort& cohort);
void operator+=(AnimalCohort& cohort1, const AnimalCohort& cohort2);
void operator-=(AnimalCohort& cohort1, const AnimalCohort& cohort2);
void operator+=(EggCohort& cohort1, const EggCohort& cohort2);
void operator+=(AnimalCohort& cohort1, const EggCohort& cohort2);
void operator*=(AnimalCohort& cohort1, const double number);

#endif

#endif /* TYPEDEFINITIONS_HPP_ */
