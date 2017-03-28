/*
 * FoodWebModel.hpp
 *
 *  Created on: 13 mar. 2017
 *      Author: manu_
 */


#ifndef FOODWEBMODEL_HPP_
#define FOODWEBMODEL_HPP_

#include <math.h>
#include <string>
#include "TypeDefinitions.hpp"
#include "ReadProcessedData.hpp"


/*
 * EXTINCTION constant (Wetzel, 1975, AquaTox Documentation, page 73)
 * Since we do not consider any feedback from the food water to the geophysical model, we assume that Light Extinction = Water Extinction
 */
static const double EXTINCTION=0.02;

/*
 * We consider that virtually all phytoplankton has the light attenuation coefficient of diatom algae
 */
static const double  DIATOM_ATTENUATION=0.14;
static const double  ATTENUATION_COEFFICIENT=DIATOM_ATTENUATION;


/*
 * The Secchi disk is an indication of water turbidity (AquaTox Documentation, page 74).
 */
static const double TURBIDITY=1.2f/EXTINCTION;

/*
 * The respiration rate at 20 degrees (AquaTox Documentation, page 85, figure 61)
 */
static const double RESP20=0.3f;

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
static const double FRACTION_SLOUGHED=0.5f;

/* Set an arbitrary index for max depth index. Depth will be normalized according to this max index*/

static const int MAX_DEPTH_INDEX = 100;

namespace FoodWebModel {



	class FoodWebModel {


		/*Class attributes*/
	protected:
		physicalType indexToDepth[MAX_DEPTH_INDEX];
		physicalType *temperatureAtSurface, *initialTemperatureAtSurface, **temperature, *depthVector;

		/*Phytoplankton biomass, periphyton biomass and temperature*/
		biomassType **phytoBiomass, **periBiomass, **priorPhytoBiomass, **priorPeriBiomass, **localBiomass;

		/*A vector to reference the calculated biomass*/

		physicalType incidentLightIntensity;
		double fractionInEuphoticZone, ZEuphotic, ZMean, ZMax, P;

		/*Max possible column index (X axis)*/
		int  maxColumn;

		/*Buffer line to write simulation results*/
		std::string lineBuffer;
		/*Class methods*/
	public:
		FoodWebModel(std::string depthRoute, std::string temperatureRoute);
		int simulate(int cycles,  const char* outputFileName);


	protected:
		void step();
		biomassType biomassDifferential(int depthIndex, int column, bool periPhyton);
		physicalType lightLimitation(int depthIndex, int column);
		biomassType sumPhytoBiomassToDepth(int depthIndex, int column);
		biomassType photoSynthesis(biomassType localPointBiomass, physicalType localeLightLimitation, bool periPhyton);
		physicalType productionLimit(physicalType localeLightLimitation, bool periPhyton);
		void setBathymetricParameters();
		biomassType respiration(biomassType localPointBiomass, physicalType localTemperature);
		biomassType excretion(biomassType localePhotoSynthesis, physicalType localeLightLimitation);
		biomassType naturalMortality(physicalType localTemperature, physicalType localeLightLimitation, biomassType localPointBiomass);
		biomassType highTemperatureMortality(physicalType localeTemperature);
		biomassType resourceLimitationStress(physicalType localeLightLimitation);
		biomassType sinking(int depthIndex, int columnIndex);
		biomassType slough(int depthIndex, int columnIndex);
		void calculatePhysicalLakeDescriptors();
		void initializePointers();
	};
}



#endif /* FOODWEBMODEL_HPP_ */
