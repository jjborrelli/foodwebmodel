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

#define HOMOGENEOUS_DEPTH
//#define RADIATED_CHEMICAL
#define EXPONENTIAL_LIGHT
//#define ADJUST_SALINITY_GRAZERS
//#define IBM_MODEL_TEMPERATURE
//#define STABLE_CHLOROPHYLL
//#define USE_PHOTOPERIOD

#define LIMITATION_MINIMUM

#define USE_LITERATURE_AND_DATA_CONSTANTS

//#define ADDITIVE_TURBIDITY

#define ADD_CONSTANT_BIOMASS_DIFFERENTIAL
//#define ADD_VARIABLE_BIOMASS_DIFFERENTIAL

#define GRAZING_EFFECT_ON_ALGAE_BIOMASS


#define TIME_VARIABLE_PHOSPHORUS_CONCENTRATION_AT_BOTTOM

#define MIGRATE_ZOOPLANKTON_AT_HOUR

//#define ZOOPLANKTON_ACCUMULATION
/* End simulation modes*/


#endif /* SIMULATIONMODES_HPP_ */
