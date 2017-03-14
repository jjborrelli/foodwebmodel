/*
 * FoodWebModel.cpp

 *
 *  Created on: 13 mar. 2017
 *      Author: manu_
 */

using FoodWebModel::FoodWebModel;

#include "../headers/FoodWebModel.hpp"

int FoodWebModel::simulate(int cycles){
	return 0;
}

/*
 * Differential of biomass for photoplankton and periphyton at each time step
 */

double FoodWebModel::biomassDifferential(int depthIndex, int column, bool periPhyton){
	localBiomass=periPhyton?periBiomass:phytoBiomass;

	/*Calculate temporal and spatially local values that will be used to calculate biomass differential*/
	double localPointBiomass=localBiomass[depthIndex][column];
	double localeLightLimitation = lightLimitation(depthIndex, column);
	double localePhotoSynthesis=photoSynthesis(localPointBiomass, localeLightLimitation, periPhyton);
	double localeTemperature =temperature[depthIndex][column];

	/*Formula of biomass differential (AquaTox Documentation, page 67, equations 33 and 34)*/
	return localePhotoSynthesis-respiration(localPointBiomass, localeTemperature)
			-excretion(localePhotoSynthesis, localeLightLimitation)
			-naturalMortality(localeTemperature, localeLightLimitation, localPointBiomass);
}


/*
 * Light intensity is calculated using the simple model from Ryabov, 2012, Phytoplankton competition in deep biomass maximum, Theoretical Ecology, equation 3
 */
double FoodWebModel::lightLimitation(int depthIndex, int column){
	/*
	 * Transform depth from an integer index to a real value in ms
	 */
	double depthInMeters= depthVector[depthIndex];
	return incidentIntensity*exp(-(TURBIDITY*depthInMeters+ATTENUATION_COEFFICIENT*sumPhytoBiomassToDepth(depthIndex, column)));
}


/*
 * Summing of phytoplankton biomass up to the given depth
 */
double FoodWebModel::sumPhytoBiomassToDepth(int depthIndex, int column){
	double sumPhytoBiomass=0.0l;
	for(int i=0; i<depthIndex; i++){
		sumPhytoBiomass+=phytoBiomass[i][column];
	}
	return sumPhytoBiomass;
}


/*
 * Photosynthesis to calculate biomass differential
 */
double FoodWebModel::photoSynthesis(double localPointBiomass, double localeLightLimitation, bool periPhyton){

	return productionLimit(localeLightLimitation, periPhyton)*localPointBiomass;
}

double FoodWebModel::productionLimit(double localeLightLimitation, bool periPhyton){
	/*The main limiting effects are temperature, nutrient concentration, toxicant and light. Since we will not have data
	 * on nutrient concentration and the temperature model is complex, we will use only light intensity as a limiting effect*/
	/*
	 * We will also use the fraction of littoral zone for periphyton
	 */
	double productionLimit= localeLightLimitation;
	if(periPhyton){
		productionLimit*=fractionInEuphoticZone;
	}
	return productionLimit;
}

FoodWebModel::FoodWebModel(){
	setBathymetricParameters();
}

/*
 * Calculate system-specific bathymetric parameters
 */
void FoodWebModel::setBathymetricParameters(){
	/*
	 *  (AquaTox Documentation, page 45, equation 8)
	 */
	this->P =6.0l*ZMean/ZMax-3.0l;

	/*
	 *  (AquaTox Documentation, page 46, equation 9)
	 */
	this->fractionInEuphoticZone=(1-P)*ZEuphotic/ZMax + P*pow(ZEuphotic/ZMax, 2);
}

/*
 * Biomass lost to respiration
 */
double FoodWebModel::respiration(double localPointBiomass, double localTemperature){
	return RESP20*pow(EXPONENTIAL_TEMPREATURE_COEFFICIENT, localTemperature-20)*localPointBiomass;
}

/*
 * Biomass lost to excretion (AquaTox Documentation, page 85, equation 64)
 */
double FoodWebModel::excretion(double localePhotoSynthesis, double localeLightLimitation){
	return PROPORTION_EXCRETION_PHOTOSYNTHESIS*(1-localeLightLimitation)*localePhotoSynthesis;
}


/*
 * Biomass lost to natural mortality (AquaTox Documentation, page 86, equation 66)
 */
double FoodWebModel::naturalMortality(double localeTemperature, double localeLightLimitation, double localPointBiomass){
	return (INTRINSIC_MORTALITY_RATE+highTemperatureMortality(localeTemperature))*localPointBiomass;
}

/*
 * Biomass lost to high temperature (AquaTox Documentation, page 86, equation 67)
 */
double FoodWebModel::highTemperatureMortality(double localeTemperature){

	return exp(localeTemperature-MAXIMUM_TOLERABLE_TEMPERATURE)/2.0f;

}

/*
 * Biomass lost to stress related to resource limitation (AquaTox Documentation, page 86, equation 68)
 */
double FoodWebModel::resourceLimitationStress(double localeLightLimitation){
	return 1.0f-exp(-MAXIMUM_RESOURCE_LIMITATION_LOSS*(1-localeLightLimitation));

}
