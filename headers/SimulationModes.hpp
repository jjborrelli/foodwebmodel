/*
 * SimulationModes.hpp
 *
 *  Created on: May 31, 2017
 *      Author: manu_
 */

#ifndef SIMULATIONMODES_HPP_
#define SIMULATIONMODES_HPP_



/* Simulation modes*/
/* If running in Windows OS, use debug mode*/

#define CONSTANT_CONCENTRATION

//#define SIGMOID_CONCENTRATION

#define HOMOGENEOUS_DEPTH


//#define RADIATED_CHEMICAL
//#define PROPORTIONAL_LIGHT
//#define ADJUST_SALINITY_GRAZERS
//#define IBM_MODEL_TEMPERATURE
//#define STABLE_CHLOROPHYLL
//#define USE_PHOTOPERIOD

#define USE_YEARLY_LIGHT_SURFACE
//#define LINEAR_LIGHT
#define AQUATOX_LIGHT_ALLOWANCE

//#define LIMITATION_MINIMUM

//#define WEIGHTED_RESOURCE_LIMITATION

#define USE_LITERATURE_AND_DATA_CONSTANTS

//#define ADDITIVE_TURBIDITY

#define ADD_CONSTANT_BIOMASS_DIFFERENTIAL
//#define ADD_VARIABLE_BIOMASS_DIFFERENTIAL

#define GRAZING_EFFECT_ON_ALGAE_BIOMASS

//#define NUTRIENT_LIMITATION_GLM

//#define NUTRIENT_LIMITATION_QUOTIENT

#define SEVERAL_LEVELS_LIMITATION_QUOTIENT

#define TIME_VARIABLE_CHEMICAL_CONCENTRATION_AT_BOTTOM

//#define MIGRATE_ZOOPLANKTON_AT_HOUR

//#define LOCALE_SEARCH_ZOOPLANKTON_MIGRATION

//#define ADD_GRAZER_PREDATORY_PRESSURE

//#define CHECK_ASSERTIONS

//#define GRAZER_CARRYING_CAPACITY_MORTALITY

//#define ALGAL_CARRYING_CAPACITY_MORTALITY

//#define EXTENDED_OUTPUT

#define INDIVIDUAL_BASED_ANIMALS

//#define ANIMAL_STARVATION_HOURS_WITHOUT_FOOD

#define ANIMAL_STARVATION_PROPORTION_LOST_BIOMASS

#define ACCUMULATIVE_HOUR_STARVATION

//#define ANIMAL_AGING
//#define CHECK_GRAZER_LOWER_LIMIT

#define CREATE_NEW_COHORTS

#define GRAZING_DEPENDING_ON_WEIGHT

//#define ANIMAL_COHORT_MAP

//#define REPORT_COHORT_INFO

//#define REGISTER_COHORT_ID

//#define COUNT_EGGS

//#define EXPONENTIAL_GONAD_ALLOCATION

//#define SIGMOIDAL_GONAD_ALLOCATION

//#define DIFFERENTIAL_ALLOCATION


#define MATURE_JUVENILES

//#define ADD_DEAD_BIOMASS_NUTRIENTS

#define LAKE_DATA_TEMPERATURE

//#define TEMPERATURE_MEDIATED_GRAZING
//#define CHECK_LOST_BIOMASS_ADDITION

//#define PRINT_ANIMAL_SUMMATORY

//#define SATURATION_GRAZING
//#define ZOOPLANKTON_ACCUMULATION
/* End simulation modes*/

#define STARVATION_MORTALITY

//#define LIGHT_BASED_MIGRATION_VARIABLE_FREQUENCY

//#define LIGHT_BASED_MIGRATION_FIXED_FREQUENCY

#define RANDOM_WALK_MIGRATION

#define LINEAR_MIGRATION_COMBINATION

//#define DAPHNIA_CIRCADIAN_CYCLE

#define MINIMUM_PREDATION_SAFETY

//#define THRESHOLD_LIGHT_SAFETY

//#define OPTIMAL_VALUE_MIGRATION

//#define CONSUME_DURING_MIGRATION

//#define REGISTER_ALL_COHORTS

#define CONGLOMERATE_ALL_COHORTS

#define SPLIT_COHORTS

#endif /* SIMULATIONMODES_HPP_ */
