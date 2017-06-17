/*
 * ModelConstants.hpp
 *
 *  Created on: 3 abr. 2017
 *      Author: manu_
 */

#ifndef MODELCONSTANTS_HPP_
#define MODELCONSTANTS_HPP_
#include "TypeDefinitions.hpp"

//#if defined(_WIN64) || defined(WIN64) || defined(_WIN32) || defined(WIN32) || defined(__CYGWIN__)
//	#define DEBUG_MODE
//#endif

#include "SimulationModes.hpp"
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

static const biomassType CONSTANT_ALGAE_BIOMASS_DIFFERENTIAL=0.00085f;
static const biomassType LOW_DEPTH_CONSTANT_ALGAE_BIOMASS_DIFFERENTIAL=CONSTANT_ALGAE_BIOMASS_DIFFERENTIAL;///5.0f;
static const biomassType VARIABLE_ALGAE_BIOMASS_DIFFERENTIAL_WEIGHT=0.0002f;
//static const unsigned int GRAZERS_LIMIT=5;
static const unsigned int GRAZERS_LIMIT=99;
//static const unsigned int GRAZERS_LIMIT=22;

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

static const biomassType INTRINSIC_ALGAE_MORTALITY_RATE=0.019f;
//static const double INTRINSIC_ALGAE_MORTALITY_RATE=0.0019f;

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

static const physicalType LIGHT_STEEPNESS = 0.0005f;

static const biomassType PHOTOSYNTHESIS_FACTOR = 8.0f;

static const unsigned int HOURS_PER_DAY = 24;

/* Nutrient derivative taken as the average difference between consecutive points*/
//static const physicalType NUTRIENT_DERIVATIVE = 0.02527915f;
//static const double NUTRIENT_DERIVATIVE = 0.02527915f*1.3f;
static const double NUTRIENT_DERIVATIVE = 0.02527915f*2.4f;

/* Phosphorus concentration at bottom taken from lake data*/
static const physicalType PHOSPHORUS_CONCENTRATION_AT_BOTTOM = 2.507143f;

/* Salt concentration at the bottom (taken from lake data)*/
static const physicalType SALT_CONCENTRATION_AT_BOTTOM = 15.31714f/1e3f;

/* Phosphorous linear limit threshold (obtained from Larry's data) */

static const physicalType PHOSPHORUS_LINEAR_THRESHOLD = 2.5f;

//static const physicalType PHOSPHORUS_LINEAR_COEFFICIENT =  2.018f;
static const physicalType PHOSPHORUS_LINEAR_COEFFICIENT =  2.664f;
//static const double PHOSPHORUS_LINEAR_COEFFICIENT =  1.518f;

//static const double PHOSPHORUS_INTERCEPT = -2.618f;
static const physicalType PHOSPHORUS_INTERCEPT = -2.118f;

static const biomassType PHOSPHORUS_GROWTH_LIMIT = 2.5f;

/* Added a weight factor to control resource limitation weight*/

//static const double RESOURCE_LIMITATION_WEIGHT = 0.08f;
//static const double RESOURCE_LIMITATION_WEIGHT = 0.5f;
static const biomassType RESOURCE_LIMITATION_WEIGHT = 1.0f;
/* Added a weight factor to control sinking as a function of depth*/

//static const biomassType SINKING_DEPTH_WEIGHT = 1.0f;
static const biomassType SINKING_DEPTH_WEIGHT = 0.3f;

/* Added factor to increase convergence of biomass*/

static const physicalType ALGAE_BIOMASS_DIFFERENTIAL_BURNIN_SCALE=1.0f;
//static const physicalType ALGAE_BIOMASS_DIFFERENTIAL_PRODUCTION_SCALE=1.0f;
//static const physicalType ALGAE_BIOMASS_DIFFERENTIAL_PRODUCTION_SCALE=0.0006594164f; //Using data without grazers removal

static const unsigned int BURNIN_MAX_CYCLE=1;

/* Time and spatial resolution constants for simulation*/
#ifndef DEBUG_MODE
static const unsigned int TIME_MESSAGE_RESOLUTION=10, TIME_OUTPUT_RESOLUTION=10, DEPTH_OUTPUT_RESOLUTION=2, COLUMN_OUTPUT_RESOLUTION=2;
#else
static const unsigned int TIME_MESSAGE_RESOLUTION=1, TIME_OUTPUT_RESOLUTION=1, DEPTH_OUTPUT_RESOLUTION=1, COLUMN_OUTPUT_RESOLUTION=1;
#endif
/* Set an hour where the differential is considered stable*/

static const unsigned int STABLE_STATE_HOUR=10;

/* Individual Daphnia dry weight. Taken from (Baudouin and Ravera, 1972, Weight, size and chemical composition of some freshwater zooplankters: Daphnia hyalina (Leydig)) (in milligrams) */

static const physicalType MICROGRAM_TO_MILLIGRAM=1.0f/1000.0f, M3_TO_LITER=1000.0f, VOLUME_PER_CELL_IN_M3=3.55579f, LITER_TO_M3=1/M3_TO_LITER, MILLILITER_TO_LITER=0.001f, LITER_TO_CELL_VOLUME=LITER_TO_M3/VOLUME_PER_CELL_IN_M3, MILLILITER_TO_VOLUME_PER_CELL=MILLILITER_TO_LITER*LITER_TO_CELL_VOLUME, CELL_VOLUME_IN_LITER=1/LITER_TO_CELL_VOLUME;

static const biomassType DAPHNIA_WEIGHT_IN_MICROGRAMS=47.41f;

static const biomassType DAPHNIA_WEIGHT_IN_MILLIGRAMS=DAPHNIA_WEIGHT_IN_MICROGRAMS*MICROGRAM_TO_MILLIGRAM;


/*Initial zooplankton vertical shift*/

static const unsigned int INITIAL_ZOOPLANKTON_SHIFT=22;


/*Maximum distance in meters for daphnia swum per hour (B.-P. Han and M. Straškraba, “Modeling patterns of zooplankton diel vertical migration,” J. Plankton Res., vol. 20, no. 8, pp. 1463–1487, 1998.)*/


static const biomassType MAXIMUM_DISTANCE_DAPHNIA_SWUM_IN_METERS_PER_HOUR=3.5f;

/*Minimum viable grazer population*/

static const zooplanktonCountType MINIMUM_VIABLE_GRAZER_POPULATION=100000000;

static const biomassType INITIAL_PREDATORY_PRESSURE = 1000000.0f;

/* Threshold beyond no more food is grazed. Taken from (Luecke et al., 1992, Feeding and assimilation rates of Daphnia pulex-fed Aphanixomenon flos-aquae, page 410, table 20-1) in grams algae/grams grazer/day.*/
//static const biomassType FEEDING_SATURATION_ADJUSTMENT = 100.0f;
//static const biomassType FEEDING_SATURATION_ADJUSTMENT = 15.0f;
//static const biomassType FEEDING_SATURATION_ADJUSTMENT = 5.0f;
static const biomassType FEEDING_SATURATION_ADJUSTMENT = 1.0f;
/* Saturation concentration taken from (N. P. Holm, G. G. Ganf, and J. Shapiro, “Feeding and assimilation rates of Daphnia pulex fed Aphanizomenon flos-aquae1,” Limnol. Oceanogr., vol. 28, no. 4, pp. 677–687, Jul. 1983.) */
/* The concentration is adjusted assuming that 20000 phyotplankton cells = 0.86 ug/ml, so 15000 cells = 0.645 ug/ml = 645 ug/l*/
static const biomassType MAXIMUM_GRAZING_ABSORBED = 645.0f;
static const biomassType FEEDING_SATURATION=FEEDING_SATURATION_ADJUSTMENT*0.4f*DAPHNIA_WEIGHT_IN_MILLIGRAMS/((double)HOURS_PER_DAY);
static const biomassType MAXIMUM_GRAZING_PROPORTION=1.0f;

/* The average grazing proportion has been taken from (Holm et al., 1983, Feeding and assimilation rates of Daphnia pulex-fed Aphanixomenon flos-aquae, page 410, table 20-1 and average grazing rate for Daphnia longispina from the control data from (Lair, 1991, page 4, table 1) (info in milliliters))
 *  in grams algae/grams grazer/day.*/

/* Lenght assumed to be about 22mm from (H. J. Dumont, I. Van De Velde, and S. Dumont, “The Dry Weight Estimate of Biomass in a Selection of Cladocera, Copepoda and Rotifera from the Plankton, Periphyton and Benthos of Continental Waters,” vol. 19, pp. 75–97, 1975.)*/

//static const biomassType
//WATER_FILTERING_RATE_PER_INDIVIDUAL_HOUR_MILLILITERS=0.6439372f,
//		WATER_FILTERING_RATE_PER_INDIVIDUAL_HOUR=WATER_FILTERING_RATE_PER_INDIVIDUAL_HOUR_MILLILITERS/MILLILITERS_TO_M3,
//WATER_FILTERING_RATE_PER_INDIVIDUAL_HOUR=2609.333f/MILLILITERS_TO_M3,
//WATER_FILTERING_RATE_PER_INDIVIDUAL_HOUR=6262.399f/MILLILITERS_TO_M3, // Stable algae biomass
//WATER_FILTERING_RATE_PER_INDIVIDUAL_HOUR=365.4528f/MILLILITERS_TO_M3,
//		DEFECATION_COEFFICIENT=0.3f;
//		DEFECATION_COEFFICIENT=0.1f;
//		DEFECATION_COEFFICIENT=0.03f;
static const biomassType DEFECATION_COEFFICIENT=0.01f;

/* Basal respiration rate. It needs to be adjusted*/

/* Calculated using the formula from (https://www.geol.umd.edu/~jmerck/bsci393/lab5/allometry.html) and the data from (H. J. Dumont, I. Van De Velde, and S. Dumont, “The Dry Weight Estimate of Biomass in a Selection of Cladocera, Copepoda and Rotifera from the Plankton, Periphyton and Benthos of Continental Waters,” vol. 19, pp. 75–97, 1975.)*/
static const biomassType BASAL_RESPIRATION_RATE=0.1125f,
		/*Weight factor (AquaTox Documentation, page 107, equation 103)*/
		BASAL_RESPIRATION_WEIGHT=1.5f;

/* Rate of respiration change for each 10 degrees Celsius (AquaTox Documentation, page 107, equation 105)*/
static const biomassType RATE_RESPIRATION_CHANGE_10DEG=-1.0f;


/* Proportion of energy lost to respiration activity*/


/* Animal base mortality*/
//static const biomassType ANIMAL_BASE_MORTALITY = 0.025f;
//static const biomassType ANIMAL_BASE_MORTALITY = 0.00005f;
//static const biomassType ANIMAL_BASE_MORTALITY = 0.0f;


/* Salinity thresholds*/

static const biomassType MIN_SALINITY = 0.0f, MAX_SALINITY=2.245f, SALINITY_COEFFICIENT_LOW=1.0f, SALINITY_COEFFICIENT_HIGH=1.0f;

/* Excretion percentage of respiration (AquaTox Documentation, page 110)*/
static const biomassType EXCRETION_RESPIRATION_PROPORTION=0.17f;
//static const biomassType EXCRETION_RESPIRATION_PROPORTION=0.017f;

#ifdef USE_LITERATURE_AND_DATA_CONSTANTS
static const biomassType STROGANOV_ADJUSTMENT = 1.0f;
#else
static const biomassType STROGANOV_ADJUSTMENT = 2.0f;
#endif

//static const biomassType RESPIRATION_ADJUSTMENT = 0.2f;
//static const biomassType RESPIRATION_ADJUSTMENT = 0.02f;
static const biomassType RESPIRATION_ADJUSTMENT = 1.0f;

/* Temperature sigmoid constants*/

static const biomassType TEMPERATURE_SLOPE_AMPLITUDE = 4.0f;

static const biomassType TEMPERATURE_SLOPE_PHASE = -7.0f;

static const biomassType TEMPERATURE_UPPER_LIMIT=25, TEMPERATURE_LOWER_LIMIT=2;

static const biomassType TEMPERATURE_ADDITIVE_COMPONENT=TEMPERATURE_UPPER_LIMIT-TEMPERATURE_LOWER_LIMIT;
#endif /* MODELCONSTANTS_HPP_ */
