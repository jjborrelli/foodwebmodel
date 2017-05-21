/*
 * ModelConstants.hpp
 *
 *  Created on: 3 abr. 2017
 *      Author: manu_
 */

#ifndef MODELCONSTANTS_HPP_
#define MODELCONSTANTS_HPP_
#include "TypeDefinitions.hpp"
/* Simulation modes*/
/* If running in Windows OS, use debug mode*/
#if defined(_WIN64) || defined(WIN64) || defined(_WIN32) || defined(WIN32) || defined(__CYGWIN__)
	#define DEBUG_MODE
#endif
//#define HOMOGENEOUS_DEPTH
#define RADIATED_CHEMICAL
#define EXPONENTIAL_LIGHT
//#define ADJUST_SALINITY_GRAZERS
//#define IBM_MODEL_TEMPERATURE
//#define STABLE_CHLOROPHYLL
//#define USE_PHOTOPERIOD
#define USE_LITERATURE_AND_DATA_CONSTANTS

#define ADDITIVE_TURBIDITY

//#define ADD_CONSTANT_BIOMASS_DIFFERENTIAL
#define ADD_VARIABLE_BIOMASS_DIFFERENTIAL
/* End simulation modes*/
/*
 * EXTINCTION constant (Wetzel, 1975, AquaTox Documentation, page 73)
 * Since we do not consider any feedback from the food water to the geophysical model, we assume that Light Extinction = Water Extinction
 */
static const physicalType EXTINCTION=0.02;

/*
 * We consider that virtually all phytoplankton has the light attenuation coefficient of diatom algae
 */
static const physicalType  DIATOM_ATTENUATION=0.14f;
#ifdef USE_LITERATURE_AND_DATA_CONSTANTS
static const physicalType  ATTENUATION_COEFFICIENT=DIATOM_ATTENUATION;
#else
static const physicalType  ATTENUATION_COEFFICIENT=0.000102f;
#endif
//static const double  ATTENUATION_COEFFICIENT=0.14f;
//static const double  ATTENUATION_COEFFICIENT=0.0f;

/*
 * The Secchi disk is an indication of water turbidity (AquaTox Documentation, page 74).
 */
//static const double TURBIDITY=1.2f/EXTINCTION;
/**
 * Alternative value of turbidity based on lake data
 */

/* Turbidity adjusted*/
//static const double TURBIDITY=14.54771f;
//static const double TURBIDITY=14.54771f;

/* Outliers removed*/
#ifdef USE_LITERATURE_AND_DATA_CONSTANTS
static const physicalType TURBIDITY=5.426461f;
//static const double TURBIDITY=5.426461f/5.0f;
static const physicalType TURBIDITY_PROPORTION = 0.5f;
static const biomassType ALGAE_ATTENUATION_PROPORTION = 1-TURBIDITY_PROPORTION;
static const biomassType ALGAE_ATTENUATION_WEIGHT=1.0f;
#else
static const physicalType TURBIDITY=5.426461f/300.0f;
#endif

//static const double TURBIDITY=5.426461f/300.0f;
//static const double TURBIDITY=0.0f;


/* An alternative for water turbidity based on the average from Vince's processed data*/

//static const double TURBIDITY=14.54771f;

static const biomassType CONSTANT_BIOMASS_DIFFERENTIAL=0.3f;
//static const unsigned int GRAZERS_LIMIT=5;
static const unsigned int GRAZERS_LIMIT=99;

/*
 * The respiration rate at 20 degrees (AquaTox Documentation, page 85, figure 61)
 */
static const biomassType RESP20=0.3f;
//static const double RESP20=0.1f;
//static const double RESP20=0.01f;

/*
 * The exponential temperature coefficient (AquaTox Documentation, page 84, equation 63)
 */
static const physicalType EXPONENTIAL_TEMPERATURE_COEFFICIENT=1.045;

/*
 * The coefficient of proportionality between excretion and photosynthesis (AquaTox Documentation, page 86, figure 62)
 */
static const biomassType PROPORTION_EXCRETION_PHOTOSYNTHESIS=0.04f;

/*
 * The intrinsic phytoplankton mortality rate (AquaTox Documentation, page 87, figure 63)
 */

static const biomassType INTRINSIC_MORTALITY_RATE=0.019f;
//static const double INTRINSIC_MORTALITY_RATE=0.0019f;

/*
 * The maximum tolerable temperature (AquaTox Documentation, page 87, figure 64)
 */

static const biomassType MAXIMUM_TOLERABLE_TEMPERATURE=38.0f;

/*
 * Maximum biomass loss due to resource limitation is yet to be defined
 */
static const biomassType MAXIMUM_RESOURCE_LIMITATION_LOSS=1.0f;

/*
 * Intrinsic settling rate (AquaTox Documentation, page 88, figure 65)
 */
static const biomassType  INTRINSIC_SETTLING_RATE=0.101f;

/*
 * Fraction of periphyton sloughed (AquaTox Documentation, page 92, figure 75), yet to be defined
 */
static const biomassType FRACTION_SLOUGHED=0.1f;

/* Set an arbitrary index for max depth index. Depth will be normalized according to this max index*/

static const unsigned int MAX_DEPTH_INDEX = 254;

/* Set an arbitrary index for max columns. Depth will be normalized according to this max index*/

static const unsigned int MAX_COLUMN_INDEX = 41;

static const physicalType MODEL_WIDTH = 1610.0f;

/* Column to meter*/

static const physicalType COLUMN_TO_METER = MODEL_WIDTH/((physicalType)MAX_COLUMN_INDEX);

/* Set fraction of the euphotic zone*/

static const physicalType Z_EUPHOTIC = 1.0f/(MAX_COLUMN_INDEX*1.0f);

/* Parameters for estimation of water temperature */

static const physicalType TEMPERATURE_AMPLITUDE = 0.0174533f;

static const physicalType TEMPERATURE_DAY_FACTOR = 0.987f;

static const physicalType TEMPERATURE_PHASE_SHIFT = 90.0f;

static const physicalType TEMPERATURE_DELAY = 30.0f;

static const physicalType Math_E = 2.718282f;

static const physicalType Math_PI = 3.141593f;

static const physicalType AVERAGE_INCIDENT_LIGHT_INTENSITY = 291.8653f;

static const unsigned int HOURS_PER_DAY = 24;

/* Nutrient derivative taken as the average difference between consecutive points*/
//static const physicalType NUTRIENT_DERIVATIVE = 0.02527915f;
//static const double NUTRIENT_DERIVATIVE = 0.02527915f*1.3f;
static const double NUTRIENT_DERIVATIVE = 0.02527915f*3.0f;

/* Phosphorus concentration at bottom taken from lake data*/
static const physicalType PHOSPHORUS_CONCENTRATION_AT_BOTTOM = 2.507143f;

/* Salt concentration at the bottom (taken from lake data)*/
static const physicalType SALT_CONCENTRATION_AT_BOTTOM = 15.31714f/1e3f;

/* Phosphorous linear limit threshold (obtained from Larry's data) */

static const physicalType PHOSPHORUS_LINEAR_THRESHOLD = 2.5f;

static const physicalType PHOSPHORUS_LINEAR_COEFFICIENT =  2.018f;
//static const double PHOSPHORUS_LINEAR_COEFFICIENT =  1.518f;

//static const double PHOSPHORUS_INTERCEPT = -2.618f;
static const physicalType PHOSPHORUS_INTERCEPT = -1.918f;

static const biomassType PHOSPHORUS_GROWTH_LIMIT = 2.5f;

/* Added a weight factor to control resource limitation weight*/

//static const double RESOURCE_LIMITATION_WEIGHT = 0.08f;
//static const double RESOURCE_LIMITATION_WEIGHT = 0.5f;
static const biomassType RESOURCE_LIMITATION_WEIGHT = 1.0f;
/* Added a weight factor to control sinking as a function of depth*/

static const biomassType SINKING_DEPTH_WEIGHT = 1.0f;

/* Added factor to increase convergence of biomass*/

//static const double BIOMASS_DIFFERENTIAL_SCALE=1.0f;
//static const double BIOMASS_DIFFERENTIAL_SCALE=0.1f;
//static const double BIOMASS_DIFFERENTIAL_SCALE=0.05f;
static const physicalType BIOMASS_DIFFERENTIAL_SCALE=0.8f;

/* Time and spatial resolution constants for simulation*/
#ifndef DEBUG_MODE
static const unsigned int TIME_MESSAGE_RESOLUTION=10, TIME_OUTPUT_RESOLUTION=10, DEPTH_OUTPUT_RESOLUTION=2, COLUMN_OUTPUT_RESOLUTION=2;
#else
static const unsigned int TIME_MESSAGE_RESOLUTION=1, TIME_OUTPUT_RESOLUTION=1, DEPTH_OUTPUT_RESOLUTION=1, COLUMN_OUTPUT_RESOLUTION=1;
#endif
/* Set an hour where the differential is considered stable*/

static const unsigned int STABLE_STATE_HOUR=10;

/* Individual Daphnia dry weight. Taken from (Baudouin and Ravera, 1972, Weight, size and chemical composition of some freshwater zooplankters: Daphnia Hyalina (Leydig)) (in milligrams) */

static const biomassType MILLIGRAM_TO_GRAM=1.0f/1000.0f;

static const biomassType DAPHNIA_WEIGHT=22.00f;

static const biomassType DAPHNIA_WEIGHT_IN_GRAMS=DAPHNIA_WEIGHT*MILLIGRAM_TO_GRAM;


/* Threshold beyond no more food is grazed. Taken from (Luecke et al. , 1992, Impacts of Variation in Planktivorous Fish on Abundance of Daphnids: A Simulation Model of the Lake Mendota Food Web, page 410, table 20-1) in grams algae/grams grazer/day.*/
static const biomassType FEEDING_SATURATION=0.4f*DAPHNIA_WEIGHT_IN_GRAMS/((double)HOURS_PER_DAY);

static const biomassType MAXIMUM_GRAZING_PROPORTION=1.0f;


static const physicalType MILLILITERS_TO_M3=1000000.0f;
/*The average grazing proportion has been taken from the average grazing rate for Daphnia longispina from the control data from (Lair, 1991, page 4, table 1) (info in milliliters) */

static const biomassType WATER_FILTERING_RATE_PER_INDIVIDUAL_HOUR=260.9333f/MILLILITERS_TO_M3, DEFECATION_COEFFICIENT=0.3f;

/* Basal respiration rate. It needs to be adjusted*/

static const biomassType BASAL_RESPIRATION_RATE=0.2f;

/* Rate of respiration change for each 10 degrees Celsius (AquaTox Documentation, page 107, equation 105)*/
static const biomassType RATE_RESPIRATION_CHANGE_10DEG=-1.0f;


/* Proportion of energy lost to respiration activity*/
//static const double K_RESP=1.0f;
static const biomassType K_RESP=0.05f;


/* Animal base mortality*/
static const biomassType ANIMAL_BASE_MORTALITY = 0.05f;

/* Salinity thresholds*/

static const biomassType MIN_SALINITY = 0.0f, MAX_SALINITY=2.245f, SALINITY_COEFFICIENT_LOW=1.0f, SALINITY_COEFFICIENT_HIGH=1.0f;

/* Excretion percentage of respiration (AquaTox Documentation, page 110)*/
static const biomassType EXCRETION_RESPIRATION_PROPORTION=0.17f;

#ifdef USE_LITERATURE_AND_DATA_CONSTANTS
static const biomassType STROGANOV_ADJUSTMENT = 1.0f;
#else
static const biomassType STROGANOV_ADJUSTMENT = 2.0f;
#endif

static const biomassType RESPIRATION_ADJUSTMENT = 0.2f;

/* Temperature sigmoid constants*/

static const biomassType TEMPERATURE_SLOPE_AMPLITUDE = 4.0f;

static const biomassType TEMPERATURE_SLOPE_PHASE = -7.0f;

static const biomassType TEMPERATURE_UPPER_LIMIT=25, TEMPERATURE_LOWER_LIMIT=2;

static const biomassType TEMPERATURE_ADDITIVE_COMPONENT=TEMPERATURE_UPPER_LIMIT-TEMPERATURE_LOWER_LIMIT;
#endif /* MODELCONSTANTS_HPP_ */
