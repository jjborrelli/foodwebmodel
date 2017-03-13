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

double FoodWebModel::biomassDifferential(int depthIndex, int column, bool periPhyton){
	return photoSynthesis(depthIndex, column, periPhyton);
}


/*
 * Light intensity is calculated using the simple model from Ryabov, 2012, Phytoplankton competition in deep biomass maximum, Theoretical Ecology, equation 3
 */
double FoodWebModel::lightIntensity(int depthIndex, int column){
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

double FoodWebModel::photoSynthesis(int depthIndex, int column, bool periPhyton){
	return productionLimit(depthIndex, column, periPhyton)*phytoBiomass[depthIndex][column];
}

double FoodWebModel::productionLimit(int depthIndex, int column, bool periPhyton){
	/*The main limiting effects are temperature, nutrient concentration, toxicant and light. Since we will not have data
	 * on nutrient concentration and the temperature model is complex, we will use only light intensity as a limiting effect*/
	/*
	 * We will also use the fraction of littoral zone for periphyton
	 */
	double productionLimit= lightIntensity(depthIndex, column);
	if(periPhyton){
		productionLimit*=fractionInEuphoticZone;
	}
	return productionLimit;
}

FoodWebModel::FoodWebModel(){
	setBathymetricParameters();
}

void FoodWebModel::setBathymetricParameters(){
	this->P =6.0l*ZMean/ZMax-3.0l;
	this->fractionInEuphoticZone=(1-P)*ZEuphotic/ZMax + P*pow(ZEuphotic/ZMax, 2);
}
