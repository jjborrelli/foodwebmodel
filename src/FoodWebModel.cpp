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

	for(currentHour=0; currentHour<cycles; currentHour++){
		/* Register standard biomass and slough to file at the given hour frequency*/
		if(currentHour%TIME_MESSAGE_RESOLUTION==0)
			cout<<"Simulation hour: "<<currentHour<<endl;
		step();
		if(currentHour%TIME_OUTPUT_RESOLUTION==0){
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
		periBiomassDifferential[columnIndex] = biomassDifferential(maxDepthIndex[columnIndex], columnIndex, true);
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
				phytoBiomassDifferential[depthIndex][columnIndex] = biomassDifferential(depthIndex, columnIndex, false);
				phytoBiomassDifferential[depthIndex][columnIndex]+=verticalMigratedPhytoBiomass[depthIndex][columnIndex];
				phytoBiomass[depthIndex][columnIndex]=max((double)0.0f, phytoBiomass[depthIndex][columnIndex]+phytoBiomassDifferential[depthIndex][columnIndex]);
				lineBuffer<<commaString<<verticalMigratedPhytoBiomass[depthIndex][columnIndex]<<commaString<<currentHour<<"\n";
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
		lineBuffer<<commaString<<verticalMigratedPeriBiomass[columnIndex]<<commaString<<currentHour<<"\n";
		/*If biomass must be registered, register standard and slough periphyton biomass*/
		if(registerPeriBiomass){
			stepBuffer<<lineBuffer.str();
			sloughBuffer<<this->maxDepthIndex[columnIndex]<<commaString<<columnIndex<<commaString<<1<<commaString<<currentHour<<commaString<<0.0f<<commaString<<periBiomassDifferential[columnIndex]<<commaString<<periBiomass[columnIndex]<<"\n";//Depth, Column, Type, Time, Washup
		}
	}
	/*Add slough biomass*/
	for(int columnIndex=0; columnIndex<MAX_COLUMN_INDEX; columnIndex++){
		for(int depthIndex=0; depthIndex<MAX_DEPTH_INDEX; depthIndex++){
			phytoBiomass[depthIndex][columnIndex]=max((double)0.0f,phytoBiomass[depthIndex][columnIndex]+sloughPhytoBiomass[depthIndex][columnIndex]);
			phytoBiomassDifferential[depthIndex][columnIndex]+=sloughPhytoBiomass[depthIndex][columnIndex];
			/*If biomass must be registered and the bottom of the lake has not been reached yet, register slough biomass*/
			if(registerPhytoBiomass[depthIndex][columnIndex]&&depthVector[columnIndex]>=indexToDepth[depthIndex]){
				sloughBuffer<<depthIndex<<commaString<<columnIndex<<commaString<<0<<commaString<<currentHour<<commaString<<sloughPhytoBiomass[depthIndex][columnIndex]<<commaString<<phytoBiomassDifferential[depthIndex][columnIndex]<<commaString<<phytoBiomass[depthIndex][columnIndex]<<"\n";//Depth, Column, Type, Time, Washup, BiomassDifferential, Biomass

			}
		}
	}

}

/*
 * Differential of biomass for photoplankton and periphyton at each time step
 */

biomassType FoodWebModel::FoodWebModel::biomassDifferential(int depthIndex, int columnIndex, bool periPhyton){

	/*Calculate temporal and spatially local values that will be used to calculate biomass differential*/
	biomassType localPointBiomass=periPhyton?priorPeriBiomass[columnIndex]:priorPhytoBiomass[depthIndex][columnIndex];

	/* Calculate nutrient limitations*/
	physicalType localeLightAllowance = lightAllowance(depthIndex, columnIndex);
	physicalType localeNutrientConcentration=nutrientConcentrationAtDepth(depthIndex, columnIndex);
	physicalType localeNutrientLimitation= calculateNutrientLimitation(localeNutrientConcentration);
	physicalType localeLimitationProduct = localeLightAllowance*localeNutrientLimitation;
	/* Calculate biomass differential components*/
	biomassType localePhotoSynthesis=photoSynthesis(localPointBiomass, localeLimitationProduct, periPhyton);
	biomassType localSedimentation = sinking(depthIndex, columnIndex);
    biomassType localSlough = slough(columnIndex);
	physicalType localeTemperature =calculateTemperature(depthIndex, columnIndex);
	biomassType localeRespiraton = -respiration(localPointBiomass, localeTemperature);
	biomassType localeExcretion =-excretion(localePhotoSynthesis, localeLightAllowance);
	biomassType localeNaturalMortality = -naturalMortality(localeTemperature, localeLimitationProduct, localPointBiomass);
	/*Formula of biomass differential (AquaTox Documentation, page 67, equations 33 and 34)*/
	biomassType totalProductivity =  localePhotoSynthesis+localeRespiraton+localeExcretion
			+localeNaturalMortality;
	totalProductivity=totalProductivity*BIOMASS_DIFFERENTIAL_SCALE;

	/* Register differential*/
	#ifndef STABLE_CHLOROPHYLL
		if(periPhyton){
			periDifferential[columnIndex]=totalProductivity/localPointBiomass;
		} else {
			phytoDifferential[depthIndex][columnIndex]=totalProductivity/localPointBiomass;
		}
	#else
		if(currentHour<=STABLE_STATE_HOUR){
			if(periPhyton){
				periDifferential[columnIndex]=totalProductivity/localPointBiomass;
			} else {
				phytoDifferential[depthIndex][columnIndex]=totalProductivity/localPointBiomass;
			}
		}
	#endif
	/* Hydrodynamics rules still need to be encoded */
	if(periPhyton){
		localSlough=-localSlough;
		verticalMigratedPhytoBiomass[depthIndex][columnIndex]+=localSlough/3.0f;
		localSedimentation=0.0f;

	} else {
		if(depthIndex==maxDepthIndex[columnIndex]){
			verticalMigratedPeriBiomass[columnIndex]+=localSedimentation;
		} else{
			verticalMigratedPhytoBiomass[depthIndex+1][columnIndex]+=localSedimentation;
		}
		localSedimentation=-localSedimentation;
		/*Slough cannot be added on the spot (it affects the previous row), so it is now registered and added at the end of the step*/
		if(depthIndex>0){
			sloughPhytoBiomass[depthIndex-1][columnIndex]=localSlough;
		}
		localSlough=-localSlough;
	}
	totalProductivity+=localSlough+localSedimentation;
	/* Create line buffer to write results*/
	lineBuffer.str("");
	lineBuffer.clear();
	lineBuffer<<depthIndex;
	lineBuffer<<commaString<<columnIndex;
	lineBuffer<<commaString<<localeLightAllowance;
	lineBuffer<<commaString<<turbidity_at_depth;
	lineBuffer<<commaString<<localePhotoPeriod;
	lineBuffer<<commaString<<light_at_depth_exponent;
	lineBuffer<<commaString<<localeLightAtDepth;
	lineBuffer<<commaString<<localeTemperature;
	lineBuffer<<commaString<<temperature_angular_frequency;
	lineBuffer<<commaString<<temperature_sine;
	lineBuffer<<commaString<<depthInMeters;
	lineBuffer<<commaString<<localeNutrientConcentration;
	lineBuffer<<commaString<<localeNutrientLimitation;
	lineBuffer<<commaString<<localeLimitationProduct;
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
	#ifdef STABLE_CHLOROPHYLL
		if(currentHour>=STABLE_STATE_HOUR){
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
physicalType FoodWebModel::FoodWebModel::lightAtDepth(int depthIndex, int columnIndex){
	/*
	 * Transform depth from an integer index to a real value in ms
	 */
	depthInMeters= indexToDepth[depthIndex];
	biomass_to_depth = ATTENUATION_COEFFICIENT*sumPhytoBiomassToDepth(depthIndex, columnIndex);
	turbidity_at_depth=TURBIDITY*depthInMeters;
	light_at_depth_exponent = -(turbidity_at_depth+biomass_to_depth);
	return light_at_top*exp(light_at_depth_exponent);
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
biomassType FoodWebModel::FoodWebModel::photoSynthesis(biomassType localPointBiomass, physicalType localeLimitationProduct, bool periPhyton){

	return productionLimit(localeLimitationProduct, periPhyton)*localPointBiomass;
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
		/* Initialize periphyton biomass*/
		this->periDifferential[i] = 0;
		this->priorPeriBiomass[i]=this->periBiomass[i]=readProcessedData.initial_biomass[maxDepthIndex[i]][i];
		for(int j=0; j<MAX_DEPTH_INDEX; j++){
			this->initial_temperature[j][i] = readProcessedData.initial_temperature[j][i];
			if(depthVector[i]<indexToDepth[j]){
				/* If the cell depth is greater than the lake depth, biomass is 0 (out of the lake)*/
				this->phytoBiomass[j][i]=this->priorPhytoBiomass[j][i]=0.0f;
				break;
			} else{
				/* If we are modeling the bottom of the lake (the depth is exactly lake depth or it is the closest possible to the lake depth) then all initial algae biomass is periphyton*/
				if(maxDepthIndex[i]==j){
					this->priorPhytoBiomass[j][i]=this->phytoBiomass[j][i]=0.0f;
				} else{
					/* If we are modeling any biomass that it is not at the bottom, then all initial biomass is phytoplankton*/
					this->priorPhytoBiomass[j][i]=this->phytoBiomass[j][i]=readProcessedData.initial_biomass[j][i];

				}
			}
			this->temperature[j][i] = this->initial_temperature[j][i];
			this->phytoDifferential[j][i]=0.0f;
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
biomassType FoodWebModel::FoodWebModel::naturalMortality(physicalType localeTemperature, physicalType localeLimitationProduct, biomassType localPointBiomass){
	return (INTRINSIC_MORTALITY_RATE+highTemperatureMortality(localeTemperature)+resourceLimitationStress(localeLimitationProduct))*localPointBiomass;
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
biomassType FoodWebModel::FoodWebModel::resourceLimitationStress(physicalType localeLimitationProduct){
	resource_limitation_exponent = -MAXIMUM_RESOURCE_LIMITATION_LOSS*(1-localeLimitationProduct);
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
biomassType FoodWebModel::FoodWebModel::slough( int columnIndex){
	return periBiomass[columnIndex]*FRACTION_SLOUGHED;
}

physicalType FoodWebModel::FoodWebModel::calculateTemperature(int depthIndex, int columnIndex){
	int dayOfYear = (currentHour/24)%365;
	physicalType localeTemperature=temperature[depthIndex][columnIndex];
	temperature_angular_frequency = TEMPERATURE_AMPLITUDE*(TEMPERATURE_DAY_FACTOR*(dayOfYear+TEMPERATURE_PHASE_SHIFT)-TEMPERATURE_DELAY);
	temperature_sine = sin(temperature_angular_frequency);
	physicalType updatedTemperature = localeTemperature+(-1.0*(temperature_range[depthIndex]))*temperature_sine;
	temperature[depthIndex][columnIndex] = updatedTemperature;
	return updatedTemperature;
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
	light_at_top=getLightAtTop();
	localeLightAtDepth = lightAtDepth(depthIndex, columnIndex);
#ifndef HOMOGENEOUS_DEPTH
	light_normalizer = biomass_to_depth*depthVector[columnIndex];
#else
	light_normalizer = biomass_to_depth*ZMax;
#endif
#ifndef EXPONENTIAL_LIGHT
	light_difference=Math_E*(localeLightAtDepth-light_at_top);
	sigmoid_light_difference=1/(1+exp(-1.0f*light_difference));
	//sigmoid_light_difference=1;

#else
	/* Use a simplistic model of light decreasing exponentially with depth*/
	light_difference=localeLightAtDepth-light_at_top;
	sigmoid_light_difference = localeLightAtDepth/light_at_top;
#endif
	return sigmoid_light_difference;
	/* Uncomment this line to make the model equivalent to AquaTox*/
	//normalized_light_difference =light_difference/(light_normalizer+light_difference);
	//return localePhotoPeriod*normalized_light_difference;
}

/*
 * Daily photo-period modeling light limitation
 * Adapted from (AquaTox Documentation, page 58, equation 26)
 * */
physicalType FoodWebModel::FoodWebModel::photoPeriod(){
	physicalType hour_fraction = ((physicalType)(currentHour%HOURS_PER_DAY))/(double)HOURS_PER_DAY;
	return max<double>(0.0f, cos(2.0f*Math_PI*hour_fraction))*0.5f +0.5f;
}

/*
 * Hourly light at top
 * */
physicalType FoodWebModel::FoodWebModel::getLightAtTop(){
#ifdef USE_PHOTOPERIOD
	return AVERAGE_INCIDENT_LIGHT_INTENSITY*localePhotoPeriod;
#else
	return this->hourlyLightAtSurface[currentHour%HOURS_PER_DAY];
#endif
}


/*Nutrient concentration at a given depth*/

physicalType FoodWebModel::FoodWebModel::nutrientConcentrationAtDepth(int depthIndex, int columnIndex){
	physicalType localeNutrientAtBottom = NUTRIENT_CONCENTRATION_AT_BOTTOM;
#ifndef HOMOGENEOUS_DEPTH
	nutrient_at_depth_exponent = (double)(NUTRIENT_DERIVATIVE*(this->indexToDepth[depthIndex]-depthVector[columnIndex]));
#else
	nutrient_at_depth_exponent = (double)(NUTRIENT_DERIVATIVE*(this->indexToDepth[depthIndex]-ZMax));
#endif
	physicalType nutrientAtDepth=localeNutrientAtBottom*exp(nutrient_at_depth_exponent);
	return nutrientAtDepth;
}

/* Nutrient biomass growth limitation based on nutrient concentration */

biomassType FoodWebModel::FoodWebModel::calculateNutrientLimitation(physicalType localeNutrientConcentration){
	biomassType returnedValue=0.0f;
	if(localeNutrientConcentration>=PHOSPHORUS_LINEAR_THRESHOLD)
		returnedValue=1.0f;
	else
		returnedValue=min((double)1.0f,(double)max((double)0.0f,(localeNutrientConcentration*PHOSPHORUS_LINEAR_COEFFICIENT+PHOSPHORUS_INTERCEPT)/PHOSPHORUS_GROWTH_LIMIT));
	//returnedValue=1.0f;
	return returnedValue;
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
biomassType FoodWebModel::FoodWebModel::animalMortality(biomassType localeBiomass, physicalType localeTemperature){
	animal_base_mortality= animalBaseMortality(localeTemperature, localeBiomass);
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
biomassType FoodWebModel::FoodWebModel::salinityEffect(physicalType salinityConcentration){
	salinity_effect=1;
	salinity_exponent=0.0f;
	biomassType salinityBase=1.0f;
	if(salinityConcentration<MIN_SALINITY){
		salinity_exponent=salinityConcentration-MIN_SALINITY;
		salinity_effect=SALINITY_COEFFICIENT_LOW*exp(salinity_exponent);
	}
	if(salinityConcentration<=MAX_SALINITY){
		salinity_exponent=MAX_SALINITY-salinityConcentration;
		salinity_effect=SALINITY_COEFFICIENT_HIGH*exp(salinity_exponent);
	}
	return salinity_effect;

}
