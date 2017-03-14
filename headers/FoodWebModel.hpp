/*
 * FoodWebModel.hpp
 *
 *  Created on: 13 mar. 2017
 *      Author: manu_
 */

#include <math.h>
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
 * The respiration rate at 20 degrees is to be defined
 */
#define RESP20 1.0f

/*
 * The exponential temperature coefficient (AquaTox Documentation, page 84, equation 63)
 */
#define EXPONENTIAL_TEMPREATURE_COEFFICIENT 1.045

/*
 * The coefficient of proportionality between excretion and photosynthesis is to be defined
 */
#define PROPORTION_EXCRETION_PHOTOSYNTHESIS 1.0f

/*
 * The intrinsic phytoplankton mortality rate and maximum tolerable temperature values are yet to be defined
 */
#define INTRINSIC_MORTALITY_RATE 1.0f
#define MAXIMUM_TOLERABLE_TEMPERATURE 1.0f

/*
 * Maximum biomass loss due to resource limitation is yet to be defined
 */
#define MAXIMUM_RESOURCE_LIMITATION_LOSS 1.0f

typedef double biomassType;
typedef double physicalType;

namespace FoodWebModel {


	class FoodWebModel {


		/*Class attributes*/
	protected:
		physicalType* depthVector, **temperature;

		/*Phytoplankton biomass, periphyton biomass and temperature*/
		biomassType **phytoBiomass, **periBiomass, **localBiomass;

		/*A vector to reference the calculated biomass*/

		physicalType incidentIntensity;
		double fractionInEuphoticZone, ZEuphotic, ZMean, ZMax, P;
		/*Class methods*/
	public:
		int simulate(int cycles);
		FoodWebModel();
	protected:
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
	};
}



#endif /* FOODWEBMODEL_HPP_ */
