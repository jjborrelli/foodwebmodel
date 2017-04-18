/*
 * FoodWebModel.cpp

 *
 *  Created on: 13 mar. 2017
 *      Author: manu_
 */

#include "../headers/FoodWebModel.hpp"
#include <string>
#include <iostream>
#include <fstream>

using namespace std;

string operator+(string arg1, int arg2){
	ostringstream  bufferString;
	bufferString<<arg2;
	return arg1+bufferString.str();
}


int FoodWebModel::FoodWebModel::simulate(int cycles,  std::string& outputFileName){
	/*CSV file to write the output. Useful for calibration*/
	ofstream outputFile;
	outputFile.open(outputFileName.c_str());
	outputFile<<"Depth, Column, LightAllowance, Turbidity, PhotoPeriod, LightAtDepthExponent, LightAtDepth, Temperature, DepthInMeters, NutrientConcentration, NutrientLimitation, NutrientAtDepthExponent, LightAtTop, LightDifference, NormalizedLightDifference, SigmoidLightDifference, ResourceLimitationExponent, BiomassToDepth, PhotoSynthesys, Respiration, Excretion, NaturalMortality, Sedimentation, WeightedSedimentation, Slough, TempMortality, ResourceLimStress, WeightedResourceLimStress, Type, PriorBiomass, BiomassDifferential, Biomass, Time\n";
	for(currentHour=0; currentHour<cycles; currentHour++){
		step();
		outputFile<<stepBuffer.str();
	}
	outputFile.close();
	return 0;
}

void FoodWebModel::FoodWebModel::step(){
	stepBuffer.str("");
	/*Calculate phytoplankton and periphyton biomass on the current step*/
	for(int depthIndex=0; depthIndex<MAX_DEPTH_INDEX; depthIndex++){
		for(int columnIndex=0; columnIndex<MAX_COLUMN_INDEX; columnIndex++){
			priorPhytoBiomass[depthIndex][columnIndex] = phytoBiomass[depthIndex][columnIndex];
			priorPeriBiomass[depthIndex][columnIndex] = periBiomass[depthIndex][columnIndex];
			biomassType phytoBiomassDifferential = biomassDifferential(depthIndex, columnIndex, false);
			phytoBiomass[depthIndex][columnIndex]+=phytoBiomassDifferential;
			lineBuffer<<commaString<<phytoBiomassDifferential<<commaString<<phytoBiomass[depthIndex][columnIndex]<<commaString<<currentHour<<"\n";
			stepBuffer<<lineBuffer.str();
			biomassType periBiomassDifferential = biomassDifferential(depthIndex, columnIndex, true);
			periBiomass[depthIndex][columnIndex]+= periBiomassDifferential;
			lineBuffer<<commaString<<periBiomassDifferential<<commaString<<periBiomass[depthIndex][columnIndex]<<commaString<<currentHour<<"\n";
			stepBuffer<<lineBuffer.str();


		}
	}

}

/*
 * Differential of biomass for photoplankton and periphyton at each time step
 */

biomassType FoodWebModel::FoodWebModel::biomassDifferential(int depthIndex, int column, bool periPhyton){

	/*Calculate temporal and spatially local values that will be used to calculate biomass differential*/
	biomassType localPointBiomass=periPhyton?priorPeriBiomass[depthIndex][column]:priorPhytoBiomass[depthIndex][column];
	physicalType localeLightAllowance = lightAllowance(depthIndex, column);
	biomassType localePhotoSynthesis=photoSynthesis(localPointBiomass, localeLightAllowance, periPhyton);
	biomassType localSedimentation = sinking(depthIndex, column);
    biomassType localSlough = slough(depthIndex, column);
	physicalType localeTemperature =calculateTemperature(depthIndex, column);
	biomassType localeRespiraton = -respiration(localPointBiomass, localeTemperature);
	biomassType localeExcretion =-excretion(localePhotoSynthesis, localeLightAllowance);
	physicalType localeNutrientConcentration=nutrientConcentrationAtDepth(depthIndex, column);
	biomassType localeNutrientLimitation= calculateNutrientLimitation(localeNutrientConcentration);
	biomassType localeNaturalMortality = -naturalMortality(localeTemperature, localeLightAllowance, localeNutrientLimitation, localPointBiomass);
	/*Formula of biomass differential (AquaTox Documentation, page 67, equations 33 and 34)*/
	biomassType totalProductivity =  localePhotoSynthesis+localeRespiraton+localeExcretion
			+localeNaturalMortality;
	totalProductivity=totalProductivity*BIOMASS_DIFFERENTIAL_SCALE;
	/* Hydrodynamics rules still need to be encoded */
	if(periPhyton){
		localSlough=-localSlough;

	} else {
		localSlough=localSlough/3.0f;
		localSedimentation=-localSedimentation;
	}
	totalProductivity+=localSlough+localSedimentation;
	/* Create line buffer to write results*/
	lineBuffer.str("");
	lineBuffer.clear();
	lineBuffer<<depthIndex;
	lineBuffer<<commaString<<column;
	lineBuffer<<commaString<<localeLightAllowance;
	lineBuffer<<commaString<<turbidity_at_depth;
	lineBuffer<<commaString<<localePhotoPeriod;
	lineBuffer<<commaString<<light_at_depth_exponent;
	lineBuffer<<commaString<<localeLightAtDepth;
	lineBuffer<<commaString<<localeTemperature;
	lineBuffer<<commaString<<depthInMeters;
	lineBuffer<<commaString<<localeNutrientConcentration;
	lineBuffer<<commaString<<localeNutrientLimitation;
	lineBuffer<<commaString<<nutrient_at_depth_exponent;
	lineBuffer<<commaString<<light_at_top;
	lineBuffer<<commaString<<light_difference;
	lineBuffer<<commaString<<normalized_light_difference;
	lineBuffer<<commaString<<sigmoid_light_difference;
	lineBuffer<<commaString<<resource_limitation_exponent;
	lineBuffer<<commaString<<biomass_to_depth;
	lineBuffer<<commaString<<localePhotoSynthesis;
	lineBuffer<<commaString<<localeRespiraton;
	lineBuffer<<commaString<<localeExcretion;
	lineBuffer<<commaString<<localeNaturalMortality;
	lineBuffer<<commaString<<sedimentation_rate;
	lineBuffer<<commaString<<localSedimentation;
	lineBuffer<<commaString<<localSlough;
	lineBuffer<<commaString<<high_temperature_mortality;
	lineBuffer<<commaString<<resource_limitation_stress;
	lineBuffer<<commaString<<weighted_resource_limitation_stress;
	lineBuffer<<commaString<<periPhyton?1:0;
	lineBuffer<<commaString<<localPointBiomass;
	return totalProductivity;
}


/*
 * Light intensity is calculated using the simple model from Ryabov, 2012, Phytoplankton competition in deep biomass maximum, Theoretical Ecology, equation 3
 */
physicalType FoodWebModel::FoodWebModel::lightAtDepth(int depthIndex, int column){
	/*
	 * Transform depth from an integer index to a real value in ms
	 */
	depthInMeters= indexToDepth[depthIndex];
	biomass_to_depth = sumPhytoBiomassToDepth(depthIndex, column);
	turbidity_at_depth=TURBIDITY*depthInMeters;
	light_at_depth_exponent = -(turbidity_at_depth+ATTENUATION_COEFFICIENT*biomass_to_depth);
	return AVERAGE_INCIDENT_LIGHT_INTENSITY*exp(light_at_depth_exponent);
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
biomassType FoodWebModel::FoodWebModel::photoSynthesis(biomassType localPointBiomass, physicalType localeLightAllowance, bool periPhyton){

	return productionLimit(localeLightAllowance, periPhyton)*localPointBiomass;
}

physicalType FoodWebModel::FoodWebModel::productionLimit(physicalType localeLightAllowance, bool periPhyton){
	/*The main limiting effects are temperature, nutrient concentration, toxicant and light. Since we will not have data
	 * on nutrient concentration and the temperature model is complex, we will use only light intensity as a limiting effect*/
	/*
	 * We will also use the fraction of littoral zone for periphyton
	 */
	physicalType productionLimit= localeLightAllowance;
	if(periPhyton){
		productionLimit*=fractionInEuphoticZone;
	}
	return productionLimit;
}


/* Initialize vectors of parameters based on read data*/
void FoodWebModel::FoodWebModel::initializeParameters(){
	/* Copy the data to arrays inside the class */
	for(int i=0; i<MAX_COLUMN_INDEX; i++){

		depthVector[i]=readProcessedData.depth[i];
		for(int j=0; j<MAX_DEPTH_INDEX; j++){
			this->initial_temperature[j][i] = readProcessedData.initial_temperature[j][i];
			this->priorPeriBiomass[j][i]=this->priorPhytoBiomass[j][i] = this->periBiomass[j][i]=this->phytoBiomass[j][i]=readProcessedData.initial_biomass[j][i];

		}
		//initialTemperatureAtSurface[i] = readProcessedData.temperaturAtSurface[i];
	}

	/*Copy arrays that depend on maximum depth*/
	for(int i=0; i<MAX_DEPTH_INDEX; i++){
		this->temperature_range[i]=readProcessedData.temperature_range[i];
		this->indexToDepth[i]=readProcessedData.depth_scale[i];

	}

	setBathymetricParameters();
}

FoodWebModel::FoodWebModel::FoodWebModel(string& depthRoute, string& depthScaleRoute, string& initialTemperatureRoute, string& temperatureRangeRoute, string& initialBiomassRoute){
/* Read the geophysical parameters from the lake, including depth and temperature at water surface
 *
 * */



	/*
	 * Read the data from the files
	 */
	readProcessedData.readGeophysicalData(depthRoute, depthScaleRoute, initialTemperatureRoute, temperatureRangeRoute, initialBiomassRoute);



}

/*
 * Calculate system-specific bathymetric parameters
 */
void FoodWebModel::FoodWebModel::setBathymetricParameters(){
	calculatePhysicalLakeDescriptors();
	/*
	 *  (AquaTox Documentation, page 45, equation 8)
	 */
	this->P =6.0l*ZMean/ZMax-3.0l;

	/*
	 *  (AquaTox Documentation, page 46, equation 9)
	 */
	this->fractionInEuphoticZone=(1-P)*Z_EUPHOTIC/ZMax + P*pow(Z_EUPHOTIC/ZMax, 2);
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
biomassType FoodWebModel::FoodWebModel::excretion(biomassType localePhotoSynthesis, physicalType localeLightAllowance){
	return PROPORTION_EXCRETION_PHOTOSYNTHESIS*(1-localeLightAllowance)*localePhotoSynthesis;
}


/*
 * Biomass lost to natural mortality (AquaTox Documentation, page 86, equation 66)
 */
biomassType FoodWebModel::FoodWebModel::naturalMortality(physicalType localeTemperature, physicalType localeLightAllowance, physicalType localeNutrientLimitation, biomassType localPointBiomass){
	return (INTRINSIC_MORTALITY_RATE+highTemperatureMortality(localeTemperature)+resourceLimitationStress(localeLightAllowance, localeNutrientLimitation))*localPointBiomass;
}

/*
 * Biomass lost to high temperature (AquaTox Documentation, page 86, equation 67)
 */
biomassType FoodWebModel::FoodWebModel::highTemperatureMortality(physicalType localeTemperature){
	high_temperature_mortality = exp(localeTemperature-MAXIMUM_TOLERABLE_TEMPERATURE)/2.0f;
	return high_temperature_mortality;

}

/*
 * Biomass lost to stress related to resource limitation (AquaTox Documentation, page 86, equation 68)
 */
biomassType FoodWebModel::FoodWebModel::resourceLimitationStress(physicalType localeLightAllowance, physicalType localeNutrientLimitation){
	resource_limitation_exponent = -MAXIMUM_RESOURCE_LIMITATION_LOSS*(1-localeLightAllowance*localeNutrientLimitation);
	resource_limitation_stress= 1.0f-exp(resource_limitation_exponent);
	weighted_resource_limitation_stress = RESOURCE_LIMITATION_WEIGHT*resource_limitation_stress;
	return weighted_resource_limitation_stress;

}

/*
 * Biomass moved from phytoplankton to periphyton by sinking (AquaTox Documentation, page 87, equation 69)
 */
biomassType FoodWebModel::FoodWebModel::sinking(int depthIndex, int columnIndex){
	/*  Can the ration mean discharge/discharge be omitted? Does this mean the amount of biomass that accumulates due to sinking?*/
	sedimentation_rate=(INTRINSIC_SETTLING_RATE/indexToDepth[depthIndex])*phytoBiomass[depthIndex][columnIndex];
	return sedimentation_rate*SINKING_DEPTH_WEIGHT;
}

/*
 * Biomass washed from periphyton to phytoplankton by slough (AquaTox Documentation, page 92, equation 75)
 */
biomassType FoodWebModel::FoodWebModel::slough(int depthIndex, int columnIndex){
	return periBiomass[depthIndex][columnIndex]*FRACTION_SLOUGHED;
}

physicalType FoodWebModel::FoodWebModel::calculateTemperature(int depthIndex, int columnIndex){
	int dayOfYear = (currentHour/24)%365;
	return temperature[depthIndex][columnIndex]+(-1.0*(temperature_range[depthIndex]))*sin(TEMPERATURE_AMPLITUDE*(TEMPERATURE_DAY_FACTOR*(dayOfYear+TEMPERATURE_PHASE_SHIFT)-TEMPERATURE_DELAY));
}


/*
 *
 */
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

//	for (int i = 0; i < MAX_COLUMN_INDEX; ++i) {
//		this->localBiomass[i] = new biomassType[MAX_COLUMN_INDEX];
//		this->periBiomass[i] = new biomassType[MAX_COLUMN_INDEX];
//		this->phytoBiomass[i] = new biomassType[MAX_COLUMN_INDEX];
//		this->priorPeriBiomass[i] = new biomassType[MAX_COLUMN_INDEX];
//		this->priorPhytoBiomass[i] = new biomassType[MAX_COLUMN_INDEX];
//		this->temperature[i] = new physicalType[MAX_COLUMN_INDEX];
//	}

}

void FoodWebModel::FoodWebModel::calculatePhysicalLakeDescriptors(){

	/* Calculate maximum and mean depth*/
	this->ZMax=this->ZMean=0;
	for(int i=0; i<MAX_COLUMN_INDEX; i++){
		ZMax = max<double>(ZMax, this->depthVector[i]);
		this->ZMean+=this->depthVector[i];
	}
	this->ZMean/=(physicalType)MAX_COLUMN_INDEX;

}

/*
 * Amount of light that is allowed into the biomass depth
 * Adapted from (AquaTox Documentation, page 70, equation 39)
 * */
physicalType FoodWebModel::FoodWebModel::lightAllowance(int depthIndex, int columnIndex){
	localePhotoPeriod = photoPeriod();
	localeLightAtDepth = lightAtDepth(depthIndex, columnIndex);
	light_at_top=AVERAGE_INCIDENT_LIGHT_INTENSITY*localePhotoPeriod;
	light_difference=Math_E*(localeLightAtDepth-light_at_top);
	light_normalizer = biomass_to_depth*depthVector[columnIndex];
	sigmoid_light_difference=1/(1+exp(-1.0f*light_difference+LIGHT_OFFSET));
	//sigmoid_light_difference=1;
	return localePhotoPeriod*sigmoid_light_difference;
	/* Uncomment this line to make the model equivalent to AquaTox*/
	//normalized_light_difference =light_difference/(light_normalizer+light_difference);
	//return localePhotoPeriod*normalized_light_difference;
}

/*
 * Daily photo-period modeling light limitation
 * Adapted from (AquaTox Documentation, page 58, equation 26)
 * */
physicalType FoodWebModel::FoodWebModel::photoPeriod(){
	physicalType hour_fraction = ((physicalType)(currentHour%(int)HOURS_PER_DAY))/HOURS_PER_DAY;
	return max<double>(0.0f, cos(2.0f*Math_PI*hour_fraction))*0.5f +0.5f;
}

/*Nutrient concentration at a given depth*/

physicalType FoodWebModel::FoodWebModel::nutrientConcentrationAtDepth(int depthIndex, int columnIndex){
	physicalType localeNutrientAtBottom = NUTRIENT_CONCENTRATION_AT_BOTTOM;
	nutrient_at_depth_exponent = (double)(NUTRIENT_DERIVATIVE*(this->indexToDepth[depthIndex]-depthVector[columnIndex]));
	physicalType nutrientAtDepth=localeNutrientAtBottom*exp(nutrient_at_depth_exponent);
	return nutrientAtDepth;
}

/* Nutrient biomass growth limitation based on nutrient concentration */

biomassType FoodWebModel::FoodWebModel::calculateNutrientLimitation(physicalType localeNutrientConcentration){
	biomassType returnedValue=0.0f;
	if(localeNutrientConcentration>=PHOSPHORUS_LINEAR_THRESHOLD)
		returnedValue=1.0f;
	else
		returnedValue=min((double)1.0f,(double)max((double)0.5f,(localeNutrientConcentration*PHOSPHORUS_LINEAR_COEFFICIENT+PHOSPHORUS_INTERCEPT)/PHOSPHORUS_GROWTH_LIMIT));
	//returnedValue=1.0f;
	return returnedValue;
}
