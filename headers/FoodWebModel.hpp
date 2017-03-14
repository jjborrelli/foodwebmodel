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
#define RESP20

/*
 * The exponential temperature coefficient (AquaTox Documentation, page 84, equation 63)
 */
#define EXPONENTIAL_TEMPREATURE_COEFFICIENT 1.045

/*
 * The coefficient of proportionality between excretion and photosynthesis is to be defined
 */
#define PROPORTION_EXCRETION_PHOTOSYNTHESIS

/*
 * The intrinsic phytoplankton mortality rate and maximum tolerable temperature values are yet to be defined
 */
#define INTRINSIC_MORTALITY_RATE
#define MAXIMUM_TOLERABLE_TEMPERATURE 1.0f

/*
 * Maximum biomass loss due to resource limitation is yet to be defined
 */
#define MAXIMUM_RESOURCE_LIMITATION_LOSS 1.0f


namespace FoodWebModel {


	class FoodWebModel {
		FoodWebModel();


		/*Class attributes*/
	protected:
		double* depthVector;

		/*Phytoplankton biomass, periphyton biomass and temperature*/
		double **phytoBiomass, **periBiomass, **temperature;

		/*A vector to reference the calculated biomass*/
		double **localBiomass;

		double incidentIntensity;
		double fractionInEuphoticZone, ZEuphotic, ZMean, ZMax, P;
		/*Class methods*/
	public:
		int simulate(int cycles);
		FoodWebModel();
	protected:
		double biomassDifferential(int depthIndex, int column, bool periPhyton);
		double lightLimitation(int depthIndex, int column);
		double sumPhytoBiomassToDepth(int depthIndex, int column);
		double photoSynthesis(double localPointBiomass, double localeLightLimitation, bool periPhyton);
		double productionLimit(double localeLightLimitation, bool periPhyton);
		void setBathymetricParameters();
		double respiration(double localPointBiomass, double localTemperature);
		double excretion(double localePhotoSynthesis, double localeLightLimitation);
		double naturalMortality(double localTemperature, double localeLightLimitation, double localPointBiomass);
		double highTemperatureMortality(double localeTemperature);
		double resourceLimitationStress(double localeLightLimitation);
	};
}



#endif /* FOODWEBMODEL_HPP_ */
