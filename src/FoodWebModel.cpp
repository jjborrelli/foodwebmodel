/*
 * FoodWebModel.cpp

 *
 *  Created on: 13 mar. 2017
 *      Author: manu_
 */

#include "../headers/FoodWebModel.hpp"
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
using namespace std;

string operator+(string arg1, int arg2){
	ostringstream  bufferString;
	bufferString<<arg2;
	return arg1+bufferString.str();
}


int FoodWebModel::FoodWebModel::simulate(int cycles,  const char* outputFileName){
	/*CSV file to write the output. Useful for calibration*/
	ofstream outputFile;
	outputFile.open(outputFileName);
	outputFile<<"Depth, Column, Productivity, PhotoSynthesys, Respiration, Excretion, NaturalMortality, Sedimentation, Slough, Type\n";
	for(int cycle=0; cycle<cycles; cycle++){
		step();
		outputFile<<lineBuffer<<"\n";
	}
	outputFile.close();
	return 0;
}

void FoodWebModel::FoodWebModel::step(){

	/*Calculate phytoplankton and periphyton biomass on the current step*/
	for(int depthIndex=0; depthIndex<MAX_DEPTH_INDEX; depthIndex++){
		for(int columnIndex=0; columnIndex<maxColumn; columnIndex++){
			priorPhytoBiomass[depthIndex][columnIndex] = phytoBiomass[depthIndex][columnIndex];
			priorPeriBiomass[depthIndex][columnIndex] = periBiomass[depthIndex][columnIndex];
			phytoBiomass[depthIndex][columnIndex] = biomassDifferential(depthIndex, columnIndex, false);
			periBiomass[depthIndex][columnIndex] = biomassDifferential(depthIndex, columnIndex, true);

		}
	}

}

/*
 * Differential of biomass for photoplankton and periphyton at each time step
 */

biomassType FoodWebModel::FoodWebModel::biomassDifferential(int depthIndex, int column, bool periPhyton){
	localBiomass=periPhyton?priorPeriBiomass:priorPhytoBiomass;

	/*Calculate temporal and spatially local values that will be used to calculate biomass differential*/
	biomassType localPointBiomass=localBiomass[depthIndex][column];
	physicalType localeLightLimitation = lightLimitation(depthIndex, column);
	biomassType localePhotoSynthesis=photoSynthesis(localPointBiomass, localeLightLimitation, periPhyton);
	biomassType localSedimentation = sinking(depthIndex, localPointBiomass);
    biomassType localSlough = slough(depthIndex, column);
	physicalType localeTemperature =temperature[depthIndex][column];
	biomassType localeRespiraton = respiration(localPointBiomass, localeTemperature);
	biomassType localeExcretion =excretion(localePhotoSynthesis, localeLightLimitation);
	biomassType localeNaturalMortality = naturalMortality(localeTemperature, localeLightLimitation, localPointBiomass);
	/*Formula of biomass differential (AquaTox Documentation, page 67, equations 33 and 34)*/
	biomassType commonProductivity =  localePhotoSynthesis-localeRespiraton-localeExcretion
			-localeNaturalMortality;

	/* Hydrodynamics rules still need to be encoded */
	if(periPhyton){
		commonProductivity+=localSedimentation-localSlough;
	} else {
		commonProductivity+=-localSedimentation+localSlough/3;
	}
	string commaString = string(", ");
	/* Create line buffer to write results*/
	lineBuffer="";
	lineBuffer+=depthIndex;
	lineBuffer+=commaString+column;
	lineBuffer+=commaString+commonProductivity;
	lineBuffer+=commaString+localePhotoSynthesis;
	lineBuffer+=commaString+localeRespiraton;
	lineBuffer+=commaString+localeExcretion;
	lineBuffer+=commaString+localeNaturalMortality;
	lineBuffer+=commaString+localSedimentation;
	lineBuffer+=commaString+localSlough;
	lineBuffer+=commaString+string(periPhyton?"peri":"phyto");
	return commonProductivity;
}


/*
 * Light intensity is calculated using the simple model from Ryabov, 2012, Phytoplankton competition in deep biomass maximum, Theoretical Ecology, equation 3
 */
physicalType FoodWebModel::FoodWebModel::lightLimitation(int depthIndex, int column){
	/*
	 * Transform depth from an integer index to a real value in ms
	 */
	physicalType depthInMeters= depthVector[depthIndex];
	return incidentLightIntensity*exp(-(TURBIDITY*depthInMeters+ATTENUATION_COEFFICIENT*sumPhytoBiomassToDepth(depthIndex, column)));
}


/*
 * Summing of phytoplankton biomass up to the given depth
 */
biomassType FoodWebModel::FoodWebModel::sumPhytoBiomassToDepth(int depthIndex, int column){
	biomassType sumPhytoBiomass=0.0l;
	for(int i=0; i<depthIndex; i++){
		sumPhytoBiomass+=priorPhytoBiomass[i][column];
	}
	return sumPhytoBiomass;
}


/*
 * Photosynthesis to calculate biomass differential
 */
biomassType FoodWebModel::FoodWebModel::photoSynthesis(biomassType localPointBiomass, physicalType localeLightLimitation, bool periPhyton){

	return productionLimit(localeLightLimitation, periPhyton)*localPointBiomass;
}

physicalType FoodWebModel::FoodWebModel::productionLimit(physicalType localeLightLimitation, bool periPhyton){
	/*The main limiting effects are temperature, nutrient concentration, toxicant and light. Since we will not have data
	 * on nutrient concentration and the temperature model is complex, we will use only light intensity as a limiting effect*/
	/*
	 * We will also use the fraction of littoral zone for periphyton
	 */
	physicalType productionLimit= localeLightLimitation;
	if(periPhyton){
		productionLimit*=fractionInEuphoticZone;
	}
	return productionLimit;
}

FoodWebModel::FoodWebModel::FoodWebModel(string depthRoute, string temperatureRoute){
/* Read the geophysical parameters from the lake, including depth and temperature at water surface
 *
 * */
	setBathymetricParameters();
	ReadProcessedData readProcessedData;

	/*
	 * Read the data from the files
	 */
	readProcessedData.readGeophysicalData(depthRoute, temperatureRoute);

	/* Copy the data to arrays inside the class */
	this->depthVector=new physicalType[readProcessedData.lakeSize];
	this->initialTemperatureAtSurface=new physicalType[readProcessedData.lakeSize];
	for(int i=0; i<readProcessedData.lakeSize; i++){
		depthVector[i]=readProcessedData.depth[i];
		initialTemperatureAtSurface[i] = readProcessedData.temperaturAtSurface[i];
	}
	maxColumn=readProcessedData.lakeSize;
}

/*
 * Calculate system-specific bathymetric parameters
 */
void FoodWebModel::FoodWebModel::setBathymetricParameters(){
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
 * Biomass lost to respiration (AquaTox Documentation, page 84, equation 63)
 */
biomassType FoodWebModel::FoodWebModel::respiration(biomassType localPointBiomass, physicalType localTemperature){
	return RESP20*pow(EXPONENTIAL_TEMPERATURE_COEFFICIENT, localTemperature-20)*localPointBiomass;
}

/*
 * Biomass lost to excretion (AquaTox Documentation, page 85, equation 64)
 */
biomassType FoodWebModel::FoodWebModel::excretion(biomassType localePhotoSynthesis, physicalType localeLightLimitation){
	return PROPORTION_EXCRETION_PHOTOSYNTHESIS*(1-localeLightLimitation)*localePhotoSynthesis;
}


/*
 * Biomass lost to natural mortality (AquaTox Documentation, page 86, equation 66)
 */
biomassType FoodWebModel::FoodWebModel::naturalMortality(physicalType localeTemperature, physicalType localeLightLimitation, biomassType localPointBiomass){
	return (INTRINSIC_MORTALITY_RATE+highTemperatureMortality(localeTemperature))*localPointBiomass;
}

/*
 * Biomass lost to high temperature (AquaTox Documentation, page 86, equation 67)
 */
biomassType FoodWebModel::FoodWebModel::highTemperatureMortality(physicalType localeTemperature){

	return exp(localeTemperature-MAXIMUM_TOLERABLE_TEMPERATURE)/2.0f;

}

/*
 * Biomass lost to stress related to resource limitation (AquaTox Documentation, page 86, equation 68)
 */
biomassType FoodWebModel::FoodWebModel::resourceLimitationStress(physicalType localeLightLimitation){
	return 1.0f-exp(-MAXIMUM_RESOURCE_LIMITATION_LOSS*(1-localeLightLimitation));

}

/*
 * Biomass moved from phytoplankton to periphyton by sinking (AquaTox Documentation, page 87, equation 69)
 */
biomassType FoodWebModel::FoodWebModel::sinking(int depthIndex, int columnIndex){
	/*  Can the ration mean discharge/discharge be omitted? Does this mean the amount of biomass that accumulates due to sinking?*/
	return INTRINSIC_SETTLING_RATE/depthVector[depthIndex]*phytoBiomass[depthIndex][columnIndex];
}

/*
 * Biomass washed from periphyton to phytoplankton by slough (AquaTox Documentation, page 92, equation 75)
 */
biomassType FoodWebModel::FoodWebModel::slough(int depthIndex, int columnIndex){
	return periBiomass[depthIndex][columnIndex]*FRACTION_SLOUGHED;
}
/*Can floating be omitted from the model? I could not find the equation modeling it.*/

/*Are we going to introduce hydrodynamics in the model? If so, we need to model the equivalent to the following equations (AquaTox Documentation, page 67, equation 33)
* 1. Washout
* 2. Washin
* 3. TurbDiff
* 4. Diffusion
*/

/*
 * Since our model will be spatially-explicit, can we replace these hydrodynamics equations with local neighborhood rules?
 * For instance, a fraction of phytoplankton biomass is drawn out and drawn in from the neighboring cells.
 * */


void FoodWebModel::FoodWebModel::initializePointers(){
	this->localBiomass = new biomassType[maxColumn][MAX_DEPTH_INDEX];
	this->periBiomass = new biomassType[maxColumn][MAX_DEPTH_INDEX];
	this->phytoBiomass = new biomassType[maxColumn][MAX_DEPTH_INDEX];
	this->priorPeriBiomass = new biomassType[maxColumn][MAX_DEPTH_INDEX];
	this->priorPhytoBiomass = new biomassType[maxColumn][MAX_DEPTH_INDEX];
	this->temperature = new physicalType[maxColumn][MAX_DEPTH_INDEX];
	this->temperatureAtSurface = new physicalType[maxColumn][MAX_DEPTH_INDEX];

	/* Initialize temperature at surface */
	for(int i=0; i<this->maxColumn; i++)
		this->temperatureAtSurface[i] = this->initialTemperatureAtSurface[i];
}

void FoodWebModel::FoodWebModel::calculatePhysicalLakeDescriptors(){

	/* Calculate maximum and mean depth*/
	this->ZMax=this->ZMean=0;
	for(int i=0; i<maxColumn; i++){
		ZMax = max(ZMax, depthVector[i]);
		this->ZMean+=depthVector[i]
	}
	this->ZMean/=(physicalType)maxColumn;

}
