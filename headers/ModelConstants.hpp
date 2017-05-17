/*
 * ModelConstants.hpp
 *
 *  Created on: 3 abr. 2017
 *      Author: manu_
 */

#ifndef MODELCONSTANTS_HPP_
#define MODELCONSTANTS_HPP_

/* Simulation modes*/
/* If running in Windows OS, use debug mode*/
#if defined(_WIN64) || defined(WIN64) || defined(_WIN32) || defined(WIN32) || defined(__CYGWIN__)
	#define DEBUG_MODE
#endif
//#define HOMOGENEOUS_DEPTH
#define EXPONENTIAL_LIGHT
//#define ADJUST_SALINITY_GRAZERS
//#define IBM_MODEL_TEMPERATURE
//#define STABLE_CHLOROPHYLL
//#define USE_PHOTOPERIOD
#define USE_LITERATURE_AND_DATA_CONSTANTS

#define ADDITIVE_TURBIDITY

/* End simulation modes*/
/*
 * EXTINCTION constant (Wetzel, 1975, AquaTox Documentation, page 73)
 * Since we do not consider any feedback from the food water to the geophysical model, we assume that Light Extinction = Water Extinction
 */
static const double EXTINCTION=0.02;

/*
 * We consider that virtually all phytoplankton has the light attenuation coefficient of diatom algae
 */
static const double  DIATOM_ATTENUATION=0.14f;
#ifdef USE_LITERATURE_AND_DATA_CONSTANTS
static const double  ATTENUATION_COEFFICIENT=DIATOM_ATTENUATION;
#else
static const double  ATTENUATION_COEFFICIENT=0.000102f;
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
static const double TURBIDITY=5.426461f;
//static const double TURBIDITY=5.426461f/5.0f;
#else
static const double TURBIDITY=5.426461f/300.0f;
#endif
//static const double TURBIDITY=5.426461f/300.0f;
//static const double TURBIDITY=0.0f;


/* An alternative for water turbidity based on the average from Vince's processed data*/

//static const double TURBIDITY=14.54771f;


/*
 * The respiration rate at 20 degrees (AquaTox Documentation, page 85, figure 61)
 */
static const double RESP20=0.3f;
//static const double RESP20=0.1f;
//static const double RESP20=0.01f;

/*
 * The exponential temperature coefficient (AquaTox Documentation, page 84, equation 63)
 */
static const double EXPONENTIAL_TEMPERATURE_COEFFICIENT=1.045;

/*
 * The coefficient of proportionality between excretion and photosynthesis (AquaTox Documentation, page 86, figure 62)
 */
static const double PROPORTION_EXCRETION_PHOTOSYNTHESIS=0.04f;

/*
 * The intrinsic phytoplankton mortality rate (AquaTox Documentation, page 87, figure 63)
 */

static const double INTRINSIC_MORTALITY_RATE=0.019f;
//static const double INTRINSIC_MORTALITY_RATE=0.0019f;

/*
 * The maximum tolerable temperature (AquaTox Documentation, page 87, figure 64)
 */

static const double MAXIMUM_TOLERABLE_TEMPERATURE=38.0f;

/*
 * Maximum biomass loss due to resource limitation is yet to be defined
 */
static const double MAXIMUM_RESOURCE_LIMITATION_LOSS=1.0f;

/*
 * Intrinsic settling rate (AquaTox Documentation, page 88, figure 65)
 */
static const double  INTRINSIC_SETTLING_RATE=0.101f;

/*
 * Fraction of periphyton sloughed (AquaTox Documentation, page 92, figure 75), yet to be defined
 */
static const double FRACTION_SLOUGHED=0.1f;

/* Set an arbitrary index for max depth index. Depth will be normalized according to this max index*/

static const int MAX_DEPTH_INDEX = 254;

/* Set an arbitrary index for max columns. Depth will be normalized according to this max index*/

static const int MAX_COLUMN_INDEX = 41;

/* Set fraction of the euphotic zone*/

static const double Z_EUPHOTIC = 1.0f/(MAX_COLUMN_INDEX*1.0f);

/* Parameters for estimation of water temperature */

static const double TEMPERATURE_AMPLITUDE = 0.0174533;

static const double TEMPERATURE_DAY_FACTOR = 0.987;

static const double TEMPERATURE_PHASE_SHIFT = 90;

static const double TEMPERATURE_DELAY = 30;

static const double Math_E = 2.718282;

static const double Math_PI = 3.141593;

static const double AVERAGE_INCIDENT_LIGHT_INTENSITY = 291.8653;

static const int HOURS_PER_DAY = 24;

/* Nutrient derivative taken as the average difference between consecutive points*/
//static const double NUTRIENT_DERIVATIVE = 0.02527915f;
static const double NUTRIENT_DERIVATIVE = 0.02527915f*1.3f;

/* Phosphorus concentration at bottom taken from lake data*/
static const double PHOSPHORUS_CONCENTRATION_AT_BOTTOM = 2.507143f;

/* Salt concentration at the bottom (taken from lake data)*/
static const double SALT_CONCENTRATION_AT_BOTTOM = 15.31714f/1e3f;

/* Phosphorous linear limit threshold (obtained from Larry's data) */

static const double PHOSPHORUS_LINEAR_THRESHOLD = 2.5f;

static const double PHOSPHORUS_LINEAR_COEFFICIENT =  2.018f;
//static const double PHOSPHORUS_LINEAR_COEFFICIENT =  1.518f;

//static const double PHOSPHORUS_INTERCEPT = -2.618f;
static const double PHOSPHORUS_INTERCEPT = -1.918f;

static const double PHOSPHORUS_GROWTH_LIMIT = 2.5;

/* Added a weight factor to control resource limitation weight*/

//static const double RESOURCE_LIMITATION_WEIGHT = 0.08f;
//static const double RESOURCE_LIMITATION_WEIGHT = 0.5f;
static const double RESOURCE_LIMITATION_WEIGHT = 1.0f;
/* Added a weight factor to control sinking as a function of depth*/

static const double SINKING_DEPTH_WEIGHT = 1.0f;

/* Added factor to increase convergence of biomass*/

//static const double BIOMASS_DIFFERENTIAL_SCALE=1.0f;
//static const double BIOMASS_DIFFERENTIAL_SCALE=0.1f;
//static const double BIOMASS_DIFFERENTIAL_SCALE=0.05f;
static const double BIOMASS_DIFFERENTIAL_SCALE=0.8f;

/* Time and spatial resolution constants for simulation*/
#ifndef DEBUG_MODE
static const int TIME_MESSAGE_RESOLUTION=10, TIME_OUTPUT_RESOLUTION=10, DEPTH_OUTPUT_RESOLUTION=2, COLUMN_OUTPUT_RESOLUTION=2;
#else
static const int TIME_MESSAGE_RESOLUTION=1, TIME_OUTPUT_RESOLUTION=1, DEPTH_OUTPUT_RESOLUTION=1, COLUMN_OUTPUT_RESOLUTION=1;
#endif
/* Set an hour where the differential is considered stable*/

static const int STABLE_STATE_HOUR=10;

/* Individual Daphnia dry weight. Taken from (Baudouin and Ravera, 1972, Weight, size and chemical composition of some freshwater zooplankters: Daphnia Hyalina (Leydig)) (in milligrams) */

static const double MILLIGRAM_TO_GRAM=1.0f/1000.0f;

static const double DAPHNIA_WEIGHT=22.00f;

static const double DAPHNIA_WEIGHT_IN_GRAMS=DAPHNIA_WEIGHT*MILLIGRAM_TO_GRAM;


/* Threshold beyond no more food is grazed. Taken from (Luecke et al. , 1992, Impacts of Variation in Planktivorous Fish on Abundance of Daphnids: A Simulation Model of the Lake Mendota Food Web, page 410, table 20-1) in grams algae/grams grazer/day.*/
static const double FEEDING_SATURATION=0.4f*DAPHNIA_WEIGHT_IN_GRAMS/((double)HOURS_PER_DAY);

static const double MAXIMUM_GRAZING_PROPORTION=1.0f;


static const double MILLILITERS_TO_M3=1000000.0f;
/*The average grazing proportion has been taken from the average grazing rate for Daphnia longispina from the control data from (Lair, 1991, page 4, table 1) (info in milliliters) */

static const double WATER_FILTERING_RATE_PER_INDIVIDUAL_HOUR=260.9333f/MILLILITERS_TO_M3, DEFECATION_COEFFICIENT=0.3f;

/* Basal respiration rate. It needs to be adjusted*/

static const double BASAL_RESPIRATION_RATE=0.2f;

/* Rate of respiration change for each 10 degrees Celsius (AquaTox Documentation, page 107, equation 105)*/
static const double RATE_RESPIRATION_CHANGE_10DEG=-1.0f;


/* Proportion of energy lost to respiration activity*/
//static const double K_RESP=1.0f;
static const double K_RESP=0.05f;


/* Animal base mortality*/
static const double ANIMAL_BASE_MORTALITY = 0.05f;

/* Salinity thresholds*/

static const double MIN_SALINITY = 0.0f, MAX_SALINITY=2.245f, SALINITY_COEFFICIENT_LOW=1.0f, SALINITY_COEFFICIENT_HIGH=1.0f;

/* Excretion percentage of respiration (AquaTox Documentation, page 110)*/
static const double EXCRETION_RESPIRATION_PROPORTION=0.17f;

#ifdef USE_LITERATURE_AND_DATA_CONSTANTS
static const double STROGANOV_ADJUSTMENT = 1.0f;
#else
static const double STROGANOV_ADJUSTMENT = 2.0f;
#endif

static const double RESPIRATION_ADJUSTMENT = 0.2f;

/* Temperature sigmoid constants*/

static const double TEMPERATURE_SLOPE_AMPLITUDE = 4.0f;

static const double TEMPERATURE_SLOPE_PHASE = -7.0f;

static const double TEMPERATURE_UPPER_LIMIT=25, TEMPERATURE_LOWER_LIMIT=2;

static const double TEMPERATURE_ADDITIVE_COMPONENT=TEMPERATURE_UPPER_LIMIT-TEMPERATURE_LOWER_LIMIT;
#endif /* MODELCONSTANTS_HPP_ */
