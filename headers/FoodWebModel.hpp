/*
 * FoodWebModel.hpp
 *
 *  Created on: 13 mar. 2017
 *      Author: manu_
 */

#include <math.h>
#include <string>
#ifdef _WIN32
	#include "C:/OSGeo4W64/include/gdal_priv.h"
#else
	#include <gdal_priv.h>
#endif

#ifndef FOODWEBMODEL_HPP_
#define FOODWEBMODEL_HPP_

/*
 * EXTINCTION constant (Wetzel, 1975, AquaTox Documentation, page 73)
 * Since we do not consider any feedback from the food water to the geophysical model, we assume that Light Extinction = Water Extinction
 */
#define EXTINCTION 0.02

/*
 * We consider that virtually all phytoplankton has the light attenuation coefficient of diatom algae
 */
#define DIATOM_ATTENUATION 0.14
#define ATTENUATION_COEFFICIENT DIATOM_ATTENUATION


/*
 * The Secchi disk is an indication of water turbidity (AquaTox Documentation, page 74).
 */
#define TURBIDITY 1.2/EXTINCTION

/*
 * The respiration rate at 20 degrees (AquaTox Documentation, page 85, figure 61)
 */
#define RESP20 0.3f

/*
 * The exponential temperature coefficient (AquaTox Documentation, page 84, equation 63)
 */
#define EXPONENTIAL_TEMPERATURE_COEFFICIENT 1.045

/*
 * The coefficient of proportionality between excretion and photosynthesis (AquaTox Documentation, page 86, figure 62)
 */
#define PROPORTION_EXCRETION_PHOTOSYNTHESIS 0.04f

/*
 * The intrinsic phytoplankton mortality rate (AquaTox Documentation, page 87, figure 63)
 */

#define INTRINSIC_MORTALITY_RATE 0.019f
/*
 * The maximum tolerable temperature (AquaTox Documentation, page 87, figure 64)
 */

#define MAXIMUM_TOLERABLE_TEMPERATURE 38.0f

/*
 * Maximum biomass loss due to resource limitation is yet to be defined
 */
#define MAXIMUM_RESOURCE_LIMITATION_LOSS 1.0f

/*
 * Intrinsic settling rate (AquaTox Documentation, page 88, figure 65)
 */
#define INTRINSIC_SETTLING_RATE 0.101f

/*
 * Fraction of periphyton sloughed (AquaTox Documentation, page 92, figure 75), yet to be defined
 */
#define FRACTION_SLOUGHED 0.5f

typedef double biomassType;
typedef double physicalType;

namespace FoodWebModel {

	class ReadGDALFile{
	protected:
		GDALDataset  *poDataset;
	public:
		void readGDALFile(std::string filename);
	};

	class FoodWebModel {


		/*Class attributes*/
	protected:
		physicalType* depthVector, **temperature;

		/*Phytoplankton biomass, periphyton biomass and temperature*/
		biomassType **phytoBiomass, **periBiomass, **priorPhytoBiomass, **priorPeriBiomass, **localBiomass;

		/*A vector to reference the calculated biomass*/

		physicalType incidentLightIntensity;
		double fractionInEuphoticZone, ZEuphotic, ZMean, ZMax, P;

		/*Max possible index*/
		int maxDepthIndex, maxColumn;

		/*Buffer line to write simulation results*/
		std::string lineBuffer;
		/*Class methods*/
	public:
		FoodWebModel();
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
	};
}



#endif /* FOODWEBMODEL_HPP_ */
