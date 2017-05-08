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


int FoodWebModel::FoodWebModel::simulate(int cycles,  const std::string& outputFileName, const std::string outputSloughRoute){
	/*CSV file to write the output. Useful for calibration*/
	printSimulationMode();
	cout<<"Simulation started."<<endl;
	ofstream outputFile, outputSloughFile;
	outputFile.open(outputFileName.c_str());
	/*Slough will be registered in a different file*/
	outputSloughFile.open(outputSloughRoute.c_str());
	/*Report successful open files*/
	cout<<"File "<<outputFileName<<" open for output."<<endl;
	cout<<"File "<<outputSloughRoute<<" open for slough register."<<endl;
	/*Write file headers*/
	outputFile<<"Depth, Column, LightAllowance, Turbidity, PhotoPeriod, LightAtDepthExponent, LightAtDepth, Temperature, TemperatureAngularFrequency, TemperatureSine, DepthInMeters, NutrientConcentration, NutrientLimitation, LimitationProduct, NutrientAtDepthExponent, LightAtTop, LightDifference, NormalizedLightDifference, SigmoidLightDifference, ResourceLimitationExponent, BiomassToDepth, PhotoSynthesys, Respiration, Excretion, NaturalMortality, Sedimentation, WeightedSedimentation, Slough, TempMortality, ResourceLimStress, WeightedResourceLimStress, Type, PriorBiomass, VerticalMigration, Time\n";
	outputSloughFile<<"Depth, Column, Type, Time, Washup, BiomassDifferential, Biomass\n";

	for(current_hour=0; current_hour<cycles; current_hour++){
		/* Register standard biomass and slough to file at the given hour frequency*/
		if(current_hour%TIME_MESSAGE_RESOLUTION==0)
			cout<<"Simulation hour: "<<current_hour<<endl;
		step();
		if(current_hour%TIME_OUTPUT_RESOLUTION==0){
			outputFile<<stepBuffer.str();
			outputSloughFile<<sloughBuffer.str();
		}
	}
	outputFile.close();
	outputSloughFile.close();
	cout<<"Simulation finished."<<endl;
	return 0;
}

void FoodWebModel::FoodWebModel::step(){
	stepBuffer.str("");
	sloughBuffer.str("");
	/* Clear vertical migrated phyto biomass*/
	for(int columnIndex=0; columnIndex<MAX_COLUMN_INDEX; columnIndex++){
		verticalMigratedPeriBiomass[columnIndex]=periBiomassDifferential[columnIndex]=0.0f;
		for(int depthIndex=0; depthIndex<MAX_DEPTH_INDEX; depthIndex++){
			verticalMigratedPhytoBiomass[depthIndex][columnIndex]=sloughPhytoBiomass[depthIndex][columnIndex]=phytoBiomassDifferential[depthIndex][columnIndex]=0.0f;
		}
	}
	/*Matrix to store the decision of biomass must be saved. It will be read when registering slough to output slough file*/
	bool registerPhytoBiomass[MAX_DEPTH_INDEX][MAX_COLUMN_INDEX];
	/*Clear vertical migration biomass*/
	/*Calculate phytoplankton and periphyton biomass on the current step*/
	for(int columnIndex=0; columnIndex<MAX_COLUMN_INDEX; columnIndex++){
		lineBuffer.str("");
		lineBuffer.clear();
		bool registerPeriBiomass=columnIndex%COLUMN_OUTPUT_RESOLUTION==0;
		/*Register previous biomass*/
		priorPeriBiomass[columnIndex] = periBiomass[columnIndex];
		/*Update biomass and output new biomass*/
		periBiomassDifferential[columnIndex] = algaeBiomassDifferential(maxDepthIndex[columnIndex], columnIndex, true);
		/* Save periphyton biomass for later registering in file*/
		std::ostringstream copyBuffer;
		copyBuffer.str("");
		copyBuffer.clear();
		copyBuffer<<lineBuffer.str();
		for(int depthIndex=0; depthIndex<MAX_DEPTH_INDEX; depthIndex++){
			lineBuffer.str("");
			lineBuffer.clear();
			/* Do not register biomass for empty cells. Since this is the bottom of the lake, do not register biomass below it*/
			if(depthVector[columnIndex]<indexToDepth[depthIndex]){
				phytoBiomass[depthIndex][columnIndex]=priorPhytoBiomass[depthIndex][columnIndex]=0.0f;
				registerPhytoBiomass[depthIndex][columnIndex]=false;
				break;
			} else{
				/*Set if biomass must be registered*/
				registerPhytoBiomass[depthIndex][columnIndex]=depthIndex%DEPTH_OUTPUT_RESOLUTION==0&&columnIndex%COLUMN_OUTPUT_RESOLUTION==0;
				/*Register previous biomass*/
				priorPhytoBiomass[depthIndex][columnIndex] = phytoBiomass[depthIndex][columnIndex];
				/*Update biomass and output new biomass*/
				phytoBiomassDifferential[depthIndex][columnIndex] = algaeBiomassDifferential(depthIndex, columnIndex, false);
				phytoBiomassDifferential[depthIndex][columnIndex]+=verticalMigratedPhytoBiomass[depthIndex][columnIndex];
				phytoBiomass[depthIndex][columnIndex]=max((double)0.0f, phytoBiomass[depthIndex][columnIndex]+phytoBiomassDifferential[depthIndex][columnIndex]);
				lineBuffer<<commaString<<verticalMigratedPhytoBiomass[depthIndex][columnIndex]<<commaString<<current_hour<<"\n";
				/*If biomass must be registered, register standard phytoplankton biomass*/
				if(registerPhytoBiomass[depthIndex][columnIndex]){
					stepBuffer<<lineBuffer.str();
				}

			}
		}
		/* Retrieve line registering periphyton biomass*/
		lineBuffer.str("");
		lineBuffer.clear();
		lineBuffer<<copyBuffer.str();
		/* Update vertical migration biomass*/
		periBiomassDifferential[columnIndex]+=verticalMigratedPeriBiomass[columnIndex];
		periBiomass[columnIndex]=max((double)0.0f, periBiomass[columnIndex]+periBiomassDifferential[columnIndex]);
		lineBuffer<<commaString<<verticalMigratedPeriBiomass[columnIndex]<<commaString<<current_hour<<"\n";
		/*If biomass must be registered, register standard and slough periphyton biomass*/
		if(registerPeriBiomass){
			stepBuffer<<lineBuffer.str();
			sloughBuffer<<this->maxDepthIndex[columnIndex]<<commaString<<columnIndex<<commaString<<1<<commaString<<current_hour<<commaString<<0.0f<<commaString<<periBiomassDifferential[columnIndex]<<commaString<<periBiomass[columnIndex]<<"\n";//Depth, Column, Type, Time, Washup
		}
	}
	/*Add slough biomass*/
	for(int columnIndex=0; columnIndex<MAX_COLUMN_INDEX; columnIndex++){
		for(int depthIndex=0; depthIndex<MAX_DEPTH_INDEX; depthIndex++){
			phytoBiomass[depthIndex][columnIndex]=max((double)0.0f,phytoBiomass[depthIndex][columnIndex]+sloughPhytoBiomass[depthIndex][columnIndex]);
			phytoBiomassDifferential[depthIndex][columnIndex]+=sloughPhytoBiomass[depthIndex][columnIndex];
			/*If biomass must be registered and the bottom of the lake has not been reached yet, register slough biomass*/
			if(registerPhytoBiomass[depthIndex][columnIndex]&&depthVector[columnIndex]>=indexToDepth[depthIndex]){
				sloughBuffer<<depthIndex<<commaString<<columnIndex<<commaString<<0<<commaString<<current_hour<<commaString<<sloughPhytoBiomass[depthIndex][columnIndex]<<commaString<<phytoBiomassDifferential[depthIndex][columnIndex]<<commaString<<phytoBiomass[depthIndex][columnIndex]<<"\n";//Depth, Column, Type, Time, Washup, BiomassDifferential, Biomass

			}
		}
	}

}

/*
 * Differential of biomass for photoplankton and periphyton at each time step
 */

biomassType FoodWebModel::FoodWebModel::algaeBiomassDifferential(int depthIndex, int columnIndex, bool periPhyton){

	/*Calculate temporal and spatially local values that will be used to calculate biomass differential*/
	biomassType localPointBiomass=periPhyton?priorPeriBiomass[columnIndex]:priorPhytoBiomass[depthIndex][columnIndex];

	/* Calculate nutrient limitations*/
	lightAllowance(depthIndex, columnIndex);
	nutrientConcentrationAtDepth(depthIndex, columnIndex);
	calculateNutrientLimitation();
	physicalType localeLimitationProduct = light_allowance*nutrient_limitation;
	/* Calculate biomass differential components*/
	photoSynthesis(localPointBiomass, localeLimitationProduct, periPhyton);
	algaeSinking(depthIndex, columnIndex);
    algaeSlough(columnIndex);
	calculateTemperature(depthIndex, columnIndex);
	physicalType localeTemperature =temperature[depthIndex][columnIndex];
	algaeRespiration(localPointBiomass, localeTemperature);
	algaeExcretion();
	/*Formula of biomass differential (AquaTox Documentation, page 67, equations 33 and 34)*/
	algaeNaturalMortality(localeTemperature, localeLimitationProduct, localPointBiomass);
	biomassType totalProductivity =  photosynthesis_value+algae_respiration_value+algae_excretion_value
			+algae_natural_mortality;
	totalProductivity=totalProductivity*BIOMASS_DIFFERENTIAL_SCALE;

	/* Register differential*/
	#ifndef STABLE_CHLOROPHYLL
		if(periPhyton){
			periDifferential[columnIndex]=totalProductivity/localPointBiomass;
		} else {
			phytoDifferential[depthIndex][columnIndex]=totalProductivity/localPointBiomass;
		}
	#else
		if(current_hour<=STABLE_STATE_HOUR){
			if(periPhyton){
				periDifferential[columnIndex]=totalProductivity/localPointBiomass;
			} else {
				phytoDifferential[depthIndex][columnIndex]=totalProductivity/localPointBiomass;
			}
		}
	#endif
	/* Hydrodynamics rules still need to be encoded */
	if(periPhyton){
		verticalMigratedPhytoBiomass[depthIndex][columnIndex]+=algae_slough_value/3.0f;
		algae_slough_value=-algae_slough_value;
		algae_sinking_value=0.0f;

	} else {
		if(depthIndex==maxDepthIndex[columnIndex]){
			verticalMigratedPeriBiomass[columnIndex]+=algae_sinking_value;
		} else{
			verticalMigratedPhytoBiomass[depthIndex+1][columnIndex]+=algae_sinking_value;
		}
		algae_sinking_value=-algae_sinking_value;
		/*Slough cannot be added on the spot (it affects the previous row), so it is now registered and added at the end of the step*/
		if(depthIndex>0){
			sloughPhytoBiomass[depthIndex-1][columnIndex]=algae_slough_value;
			algae_slough_value=-algae_slough_value;
		}
	}
	totalProductivity+=algae_slough_value+algae_sinking_value;
	/* Create line buffer to write results*/
	lineBuffer.str("");
	lineBuffer.clear();
	lineBuffer<<depthIndex;
	lineBuffer<<commaString<<columnIndex;
	lineBuffer<<commaString<<light_allowance;
	lineBuffer<<commaString<<turbidity_at_depth;
	lineBuffer<<commaString<<locale_photo_period;
	lineBuffer<<commaString<<light_at_depth_exponent;
	lineBuffer<<commaString<<light_at_depth;
	lineBuffer<<commaString<<localeTemperature;
	lineBuffer<<commaString<<temperature_angular_frequency;
	lineBuffer<<commaString<<temperature_sine;
	lineBuffer<<commaString<<depthInMeters;
	lineBuffer<<commaString<<nutrient_concentration;
	lineBuffer<<commaString<<nutrient_limitation;
	lineBuffer<<commaString<<localeLimitationProduct;
	lineBuffer<<commaString<<nutrient_at_depth_exponent;
	lineBuffer<<commaString<<light_at_top;
	lineBuffer<<commaString<<light_difference;
	lineBuffer<<commaString<<normalized_light_difference;
	lineBuffer<<commaString<<light_allowance;
	lineBuffer<<commaString<<resource_limitation_exponent;
	lineBuffer<<commaString<<biomass_to_depth;
	lineBuffer<<commaString<<photosynthesis_value;
	lineBuffer<<commaString<<algae_respiration_value;
	lineBuffer<<commaString<<algae_excretion_value;
	lineBuffer<<commaString<<algae_natural_mortality;
	lineBuffer<<commaString<<sedimentation_rate;
	lineBuffer<<commaString<<algae_sinking_value;
	lineBuffer<<commaString<<algae_slough_value;
	lineBuffer<<commaString<<high_temperature_mortality;
	lineBuffer<<commaString<<resource_limitation_stress;
	lineBuffer<<commaString<<weighted_resource_limitation_stress;
	lineBuffer<<commaString<<periPhyton?1:0;
	lineBuffer<<commaString<<localPointBiomass;
	#ifdef STABLE_CHLOROPHYLL
		if(current_hour>=STABLE_STATE_HOUR){
			if(periPhyton){
				totalProductivity= periDifferential[columnIndex]*priorPeriBiomass[columnIndex];
			} else{
				totalProductivity= phytoDifferential[depthIndex][columnIndex]*priorPhytoBiomass[depthIndex][columnIndex];

			}
		}
	#endif
	return totalProductivity;
}


/*
 * Light intensity is calculated using the simple model from Ryabov, 2012, Phytoplankton competition in deep biomass maximum, Theoretical Ecology, equation 3
 */
void FoodWebModel::FoodWebModel::lightAtDepth(int depthIndex, int columnIndex){
	/*
	 * Transform depth from an integer index to a real value in ms
	 */
	depthInMeters= indexToDepth[depthIndex];
	biomass_to_depth = ATTENUATION_COEFFICIENT*sumPhytoBiomassToDepth(depthIndex, columnIndex);
	turbidity_at_depth=TURBIDITY*depthInMeters;
	light_at_depth_exponent = -(turbidity_at_depth+biomass_to_depth);
	light_at_depth =  light_at_top*exp(light_at_depth_exponent);
}


/*
 * Summing of phytoplankton biomass up to the given depth
 */
biomassType FoodWebModel::FoodWebModel::sumPhytoBiomassToDepth(int depthIndex, int columnIndex){
	biomassType sumPhytoBiomass=0.0l;
	for(int i=0; i<depthIndex; i++){
		sumPhytoBiomass+=priorPhytoBiomass[i][columnIndex];
	}
	return sumPhytoBiomass;
}


/*
 * Photosynthesis to calculate biomass differential
 */
void FoodWebModel::FoodWebModel::photoSynthesis(biomassType localPointBiomass, physicalType localeLimitationProduct, bool periPhyton){

	photosynthesis_value =  productionLimit(localeLimitationProduct, periPhyton)*localPointBiomass;
}

physicalType FoodWebModel::FoodWebModel::productionLimit(physicalType localeLimitationProduct, bool periPhyton){
	/*The main limiting effects are temperature, nutrient concentration, toxicant and light. Since we will not have data
	 * on nutrient concentration and the temperature model is complex, we will use only light intensity as a limiting effect*/
	/*
	 * We will also use the fraction of littoral zone for periphyton
	 */
	physicalType productionLimit= localeLimitationProduct*1.0f;
	if(periPhyton){
		productionLimit*=fractionInEuphoticZone;
	}
	return productionLimit;
}


/* Initialize vectors of parameters based on read data*/
void FoodWebModel::FoodWebModel::initializeParameters(){
	/* Copy the data to arrays inside the class */
	/* Copy depth vector */
	for(int i=0; i<MAX_COLUMN_INDEX; i++){
		depthVector[i]=readProcessedData.depth[i];
	}
	/*Copy arrays that depend on maximum depth*/
	for(int i=0; i<MAX_DEPTH_INDEX; i++){
		this->temperature_range[i]=readProcessedData.temperature_range[i];
		this->indexToDepth[i]=readProcessedData.depth_scale[i];

	}
	/*Copy cyclic light at surface*/
	for(int i=0; i<HOURS_PER_DAY; i++){
		this->hourlyLightAtSurface[i]=readProcessedData.hourlyLightAtSurface[i];
	}
	/* Calculate maximum depth index*/
	for(int j=0; j<MAX_DEPTH_INDEX; j++){
		for(int i=0; i<MAX_COLUMN_INDEX; i++){
			if((depthVector[i]==indexToDepth[j])||(depthVector[i]<indexToDepth[j+1]&&depthVector[i]>=indexToDepth[j])){
				this->maxDepthIndex[i]=j;
			}

		}
	}
	/* Copy initial biomass vector*/
	for(int i=0; i<MAX_COLUMN_INDEX; i++){
		/* Initialize periphyton and bottom feeder biomass*/
		this->periDifferential[i] = 0;
		this->priorPeriBiomass[i]=this->periBiomass[i]=readProcessedData.initial_algae_biomass[maxDepthIndex[i]][i];
		this->priorBottomFeederBiomass[i]=this->bottomFeederBiomass[i]=readProcessedData.initial_grazer_biomass[maxDepthIndex[i]][i];

		for(int j=0; j<MAX_DEPTH_INDEX; j++){
			this->temperature[j][i] = this->initial_temperature[j][i] = readProcessedData.initial_temperature[j][i];
			this->phytoDifferential[j][i]=0.0f;
			if(depthVector[i]<indexToDepth[j]||maxDepthIndex[i]==j){
				/* If the cell depth is greater than the lake depth, biomass is 0 (out of the lake)*/
				this->phytoBiomass[j][i]=this->priorPhytoBiomass[j][i]=0.0f;
				this->priorZooplanktonBiomass[j][i]=this->zooplanktonBiomass[j][i]=0.0f;

				break;
			} else {
				/* If we are modeling any biomass that it is not at the bottom, then all initial biomass is phytoplankton*/
				this->priorPhytoBiomass[j][i]=this->phytoBiomass[j][i]=readProcessedData.initial_algae_biomass[j][i];
				this->priorZooplanktonBiomass[j][i]=this->zooplanktonBiomass[j][i]=readProcessedData.initial_grazer_biomass[j][i];


			}
		}
		//initialTemperatureAtSurface[i] = readProcessedData.temperaturAtSurface[i];
	}
	setBathymetricParameters();
}

FoodWebModel::FoodWebModel::FoodWebModel(const string& depthRoute, const string& depthScaleRoute, const string& initialTemperatureRoute, const string& temperatureRangeRoute, const string& initialBiomassRoute, const string& lightAtSurfaceRoute){
/* Read the geophysical parameters from the lake, including depth and temperature at water surface
 *
 * */



	/*
	 * Read the data from the files
	 */
	cout<<"Reading parameters."<<endl;
	readProcessedData.readGeophysicalData(depthRoute, depthScaleRoute, initialTemperatureRoute, temperatureRangeRoute, initialBiomassRoute, lightAtSurfaceRoute);
	cout<<"Parameters read."<<endl;


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
void FoodWebModel::FoodWebModel::algaeRespiration(biomassType localPointBiomass, physicalType localTemperature){
	algae_respiration_value = -RESP20*pow(EXPONENTIAL_TEMPERATURE_COEFFICIENT, localTemperature-20)*localPointBiomass;
}

/*
 * Biomass lost to excretion (AquaTox Documentation, page 85, equation 64)
 */
void FoodWebModel::FoodWebModel::algaeExcretion(){
	algae_excretion_value =  -PROPORTION_EXCRETION_PHOTOSYNTHESIS*(1-light_allowance)*photosynthesis_value;
}


/*
 * Biomass lost to natural mortality (AquaTox Documentation, page 86, equation 66)
 */
void FoodWebModel::FoodWebModel::algaeNaturalMortality(physicalType localeTemperature, physicalType localeLimitationProduct, biomassType localPointBiomass){
	algaeHighTemperatureMortality(localeTemperature);
	resourceLimitationStress(localeLimitationProduct);
	algae_natural_mortality = -(INTRINSIC_MORTALITY_RATE+high_temperature_mortality+weighted_resource_limitation_stress)*localPointBiomass;
}

/*
 * Biomass lost to high temperature (AquaTox Documentation, page 86, equation 67)
 */
void FoodWebModel::FoodWebModel::algaeHighTemperatureMortality(physicalType localeTemperature){
	high_temperature_mortality = exp(localeTemperature-MAXIMUM_TOLERABLE_TEMPERATURE)/2.0f;

}

/*
 * Biomass lost to stress related to resource limitation (AquaTox Documentation, page 86, equation 68)
 */
void FoodWebModel::FoodWebModel::resourceLimitationStress(physicalType localeLimitationProduct){
	resource_limitation_exponent = -MAXIMUM_RESOURCE_LIMITATION_LOSS*(1-localeLimitationProduct);
	resource_limitation_stress= 1.0f-exp(resource_limitation_exponent);
	weighted_resource_limitation_stress = RESOURCE_LIMITATION_WEIGHT*resource_limitation_stress;
}

/*
 * Biomass moved from phytoplankton to periphyton by sinking (AquaTox Documentation, page 87, equation 69)
 */
void FoodWebModel::FoodWebModel::algaeSinking(int depthIndex, int columnIndex){
	/*  Can the ration mean discharge/discharge be omitted? Does this mean the amount of biomass that accumulates due to sinking?*/
	sedimentation_rate=(INTRINSIC_SETTLING_RATE/indexToDepth[depthIndex])*phytoBiomass[depthIndex][columnIndex];
	algae_sinking_value= sedimentation_rate*SINKING_DEPTH_WEIGHT;
}

/*
 * Biomass washed from periphyton to phytoplankton by slough (AquaTox Documentation, page 92, equation 75)
 */
void FoodWebModel::FoodWebModel::algaeSlough( int columnIndex){
	algae_slough_value = periBiomass[columnIndex]*FRACTION_SLOUGHED;
}

void FoodWebModel::FoodWebModel::calculateTemperature(int depthIndex, int columnIndex){
	int dayOfYear = (current_hour/24)%365;
	physicalType localeTemperature=temperature[depthIndex][columnIndex];
	temperature_angular_frequency = TEMPERATURE_AMPLITUDE*(TEMPERATURE_DAY_FACTOR*(dayOfYear+TEMPERATURE_PHASE_SHIFT)-TEMPERATURE_DELAY);
	temperature_sine = sin(temperature_angular_frequency);
	physicalType updatedTemperature = localeTemperature+(-1.0*(temperature_range[depthIndex]))*temperature_sine;
	temperature[depthIndex][columnIndex] = updatedTemperature;
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

/* Calculation of algae biomass*/

/*
 * Amount of light that is allowed into the biomass depth
 * Adapted from (AquaTox Documentation, page 70, equation 39)
 * */
void FoodWebModel::FoodWebModel::lightAllowance(int depthIndex, int columnIndex){
	photoPeriod();
	calculateLightAtTop();
	lightAtDepth(depthIndex, columnIndex);
#ifndef HOMOGENEOUS_DEPTH
	light_normalizer = biomass_to_depth*depthVector[columnIndex];
#else
	light_normalizer = biomass_to_depth*ZMax;
#endif
#ifndef EXPONENTIAL_LIGHT
	light_difference=Math_E*(localeLightAtDepth-light_at_top);
	light_allowance=1/(1+exp(-1.0f*light_difference));
	//sigmoid_light_difference=1;

#else
	/* Use a simplistic model of light decreasing exponentially with depth*/
	light_difference=light_at_depth-light_at_top;
	light_allowance = light_at_depth/light_at_top;
#endif
	/* Uncomment this line to make the model equivalent to AquaTox*/
	//normalized_light_difference =light_difference/(light_normalizer+light_difference);
	//return localePhotoPeriod*normalized_light_difference;
}

/*
 * Daily photo-period modeling light limitation
 * Adapted from (AquaTox Documentation, page 58, equation 26)
 * */
void FoodWebModel::FoodWebModel::photoPeriod(){
	physicalType hour_fraction = ((physicalType)(current_hour%HOURS_PER_DAY))/(double)HOURS_PER_DAY;
	locale_photo_period= max<double>(0.0f, cos(2.0f*Math_PI*hour_fraction))*0.5f +0.5f;
}

/*
 * Hourly light at top
 * */
void FoodWebModel::FoodWebModel::calculateLightAtTop(){
#ifdef USE_PHOTOPERIOD
	light_at_top= AVERAGE_INCIDENT_LIGHT_INTENSITY*localePhotoPeriod;
#else
	light_at_top= this->hourlyLightAtSurface[current_hour%HOURS_PER_DAY];
#endif
}


/*Nutrient concentration at a given depth*/

void FoodWebModel::FoodWebModel::nutrientConcentrationAtDepth(int depthIndex, int columnIndex){
	physicalType localeNutrientAtBottom = NUTRIENT_CONCENTRATION_AT_BOTTOM;
#ifndef HOMOGENEOUS_DEPTH
	nutrient_at_depth_exponent = (double)(NUTRIENT_DERIVATIVE*(this->indexToDepth[depthIndex]-depthVector[columnIndex]));
#else
	nutrient_at_depth_exponent = (double)(NUTRIENT_DERIVATIVE*(this->indexToDepth[depthIndex]-ZMax));
#endif
	nutrient_concentration =localeNutrientAtBottom*exp(nutrient_at_depth_exponent);
}

/* Nutrient biomass growth limitation based on nutrient concentration */

void FoodWebModel::FoodWebModel::calculateNutrientLimitation(){
	nutrient_limitation=0.0f;
	if(nutrient_concentration>=PHOSPHORUS_LINEAR_THRESHOLD)
		nutrient_limitation=1.0f;
	else
		nutrient_limitation=min((double)1.0f,(double)max((double)0.0f,(nutrient_concentration*PHOSPHORUS_LINEAR_COEFFICIENT+PHOSPHORUS_INTERCEPT)/PHOSPHORUS_GROWTH_LIMIT));
	//returnedValue=1.0f;
}

/* A method to print the simulation mode in function of the defined flags*/
void FoodWebModel::FoodWebModel::printSimulationMode(){
#ifdef DEBUG_MODE
	cout<<"Running in debug mode."<<endl;
#else
	cout<<"Running in production mode."<<endl;
#endif
#ifdef HOMOGENEOUS_DEPTH
	cout<<"Considering homogeneous depth"<<endl;
#else
	cout<<"Considering non-homogeneous depth"<<endl;
#endif
#ifdef STABLE_CHLOROPHYLL
	cout<<"Running with static biomass differential."<<endl;
#else
	cout<<"Running with dynamic biomass differential."<<endl;
#endif
#ifdef USE_PHOTOPERIOD
	cout<<"Using sinusoidal photoperiod."<<endl;
#else
	cout<<"Using table-based photoperiod."<<endl;
#endif
}

/* Calculation of grazer biomass*/

biomassType FoodWebModel::FoodWebModel::grazerBiomassDifferential(int depthIndex, int columnIndex, bool periPython){
	return 0.0f;
}


/* Food consumption (AquaTox Documentation, page 105, equation 98)*/
biomassType FoodWebModel::FoodWebModel::foodConsumptionRate(biomassType zooBiomass, biomassType phytoBiomass){
	if(phytoBiomass/zooBiomass>FEEDING_SATURATION){
		locale_grazing= MAXIMUM_GRAZING_PROPORTION;
	} else{
		locale_grazing= min<double>(phytoBiomass, zooBiomass*GRAZING_PROPORTION);
	}
	return locale_grazing;
}

/* Food consumption (AquaTox Documentation, page 105, equation 97)*/
biomassType FoodWebModel::FoodWebModel::defecation(biomassType grazing){
	locale_defecation = DEFECATION_COEFFICIENT*grazing;
	return locale_defecation;
}

/* Zooplankton respiration (AquaTox Documentation, page 106, equation 100)*/
biomassType FoodWebModel::FoodWebModel::animalRespiration(biomassType zooBiomass, biomassType consumptionBiomass, biomassType productionBiomass, physicalType localeTemperature){
	base_zooplankton_respiration = (basalRespiration(zooBiomass) + activeRespiration(zooBiomass, localeTemperature) + metabolicFoodConsumption(productionBiomass, consumptionBiomass));
	salinity_corrected_zooplankton_respiration = base_zooplankton_respiration*salinity_effect;
	return salinity_corrected_zooplankton_respiration;
}


/* Basal respiration (AquaTox Documentation, page 106, equation 101)*/
biomassType FoodWebModel::FoodWebModel::basalRespiration(biomassType zooBiomass){
	basal_respiration =zooBiomass*BASAL_RESPIRATION_RATE;
	return basal_respiration;

}

/* Active respiration (AquaTox Documentation, page 107, equation 104)*/
biomassType FoodWebModel::FoodWebModel::activeRespiration(biomassType zooBiomass, physicalType localeTemperature){
	active_respiration_exponent = RATE_RESPIRATION_CHANGE_10DEG*localeTemperature;
	active_respiration_factor=exp(active_respiration_exponent);
	active_respiration = zooBiomass*active_respiration_factor;
	return active_respiration;
}


/* Specific dynamic action respiration (AquaTox Documentation, page 109, equation 110)*/
biomassType FoodWebModel::FoodWebModel::metabolicFoodConsumption(biomassType productionBiomass, biomassType consumptionBiomass){
	metabolic_respiration= K_RESP*(productionBiomass-consumptionBiomass);
	return metabolic_respiration;
}

/* Grazer excretion biomass loss (AquaTox Documentation, page 109, equation 111)*/
biomassType FoodWebModel::FoodWebModel::animalExcretion(biomassType localeRespiration){
	grazer_excretion_loss= RESPIRATION_TO_EXCRETION*localeRespiration;
	return grazer_excretion_loss;
}


/* Grazer mortality (AquaTox Documentation, page 110, equation 112)*/
biomassType FoodWebModel::FoodWebModel::animalMortality(biomassType localeBiomass, physicalType localeTemperature, physicalType localeSalinityConcentration){
	animal_base_mortality= animalBaseMortality(localeTemperature, localeBiomass);
	salinity_mortality=salinityMortality(localeBiomass, localeSalinityConcentration);
	return animal_base_mortality;
}

/* Grazer base mortality (AquaTox Documentation, page 110, equation 113)*/
biomassType FoodWebModel::FoodWebModel::animalBaseMortality(physicalType localeTemperature, biomassType localeBiomass){
	animal_temperature_mortality = animalTemperatureMortality(localeTemperature)*localeBiomass;
	animal_temp_independent_mortality = ANIMAL_BASE_MORTALITY*localeBiomass;
	return animal_temperature_mortality+animal_temp_independent_mortality;
}

/* Grazer base mortality (AquaTox Documentation, page 110, equation 114)*/
biomassType FoodWebModel::FoodWebModel::animalTemperatureMortality(physicalType localeTemperature){
	if(localeTemperature<=MAXIMUM_TOLERABLE_TEMPERATURE){
		return 0;
	} else{
		return exp(MAXIMUM_TOLERABLE_TEMPERATURE-localeTemperature)/2.0f;
	}

}

/* Salinity effect on respiration and mortality (AquaTox Documentation, page 295, equation 440)*/
physicalType FoodWebModel::FoodWebModel::salinityEffect(physicalType salinityConcentration){
	salinity_effect=1;
	salinity_exponent=0.0f;
	biomassType salinityBase=1.0f;
	if(salinityConcentration<MIN_SALINITY){
		salinity_exponent=salinityConcentration-MIN_SALINITY;
		salinity_effect=SALINITY_COEFFICIENT_LOW*exp(salinity_exponent);
	}
	if(salinityConcentration>MAX_SALINITY){
		salinity_exponent=MAX_SALINITY-salinityConcentration;
		salinity_effect=SALINITY_COEFFICIENT_HIGH*exp(salinity_exponent);
	}
	return salinity_effect;

}

/* Salinity mortality (AquaTox Documentation, page 110, equation 112)*/
biomassType FoodWebModel::FoodWebModel::salinityMortality(biomassType localeBiomass, physicalType salinityConcentration){
	salinity_mortality=localeBiomass*(1-salinityConcentration);
	return salinity_mortality;

}

/* As a first approximation, let us assume that we only have Cladocerans (Daphnia) in the system, since they are the main grazers.
 * Assume that the grazing rate of Cladocerans is constant, independent of the abundance of algae biomass in the system
 * Play with the number of Cladocerans in the system, increasing them directly proportional to temperature.
 * Model grazers as an integer number (we are modeling the number of cladocerans, not their biomass) and grazers' biomass as an array of real numbers.
 * We can assume that grazers are evenly distributed across all depth values.
 * */
