/*
 * FoodWebModel.cpp

 *
 *  Created on: 13 mar. 2017
 *      Author: manu_
 */

#include "../headers/FoodWebModel.hpp"
#include <string>
#include <iostream>

using namespace std;

string operator+(string arg1, int arg2){
	ostringstream  bufferString;
	bufferString<<arg2;
	return arg1+bufferString.str();
}

void FoodWebModel::FoodWebModel::setFileParameters(
		const SimulationArguments& simArguments) {
	/*Read numeric parameters*/
	this->algae_biomass_differential_production_scale =
			simArguments.algae_biomass_differential_production_scale;
	this->animal_base_mortality_proportion =
			simArguments.animal_base_mortality_proportion;
	this->simulation_cycles = simArguments.simulationCycles;
	this->maximum_distance_daphnia_swum_in_rows_per_hour =
			MAXIMUM_DISTANCE_DAPHNIA_SWUM_IN_METERS_PER_HOUR * MAX_COLUMN_INDEX
					/ this->ZMax;
	this->vertical_migration_buffer_size = 2
			* maximum_distance_daphnia_swum_in_rows_per_hour + 1;
	this->filtering_rate_per_daphnia = simArguments.filtering_rate_per_daphnia;
	this->filtering_rate_per_daphnia_in_cell_volume=this->filtering_rate_per_daphnia*(MILLILITER_TO_VOLUME_PER_CELL);
	this->basal_respiration_weight = simArguments.basal_respiration_weight;
	this->k_value_respiration = simArguments.k_value_respiration;
	this->grazer_carrying_capacity_coefficient = simArguments.grazer_carrying_capacity_coefficient;
	this->grazer_carrying_capacity_intercept = simArguments.grazer_carrying_capacity_intercept;
	this->phosphorus_half_saturation = simArguments.phosphorus_half_saturation;
	this->light_allowance_weight = simArguments.light_allowance_weight;
	this->algal_respiration_at_20_degrees = simArguments.algal_respiration_at_20_degrees;
	this->exponential_temperature_algal_respiration_coefficient = simArguments.exponential_temperature_algal_respiration_coefficient;
	this->intrinsic_algae_mortality_rate = simArguments.intrinsic_algae_mortality_rate;
	this->maximum_algae_resources_death = simArguments.maximum_algae_resources_death;
	this->light_steepness = simArguments.light_steepness;
	this->diatom_attenuation_coefficient = simArguments.diatom_attenuation_coefficient;
	this->limitation_scale_weight = simArguments.limitation_scale_weight;

}

int FoodWebModel::FoodWebModel::simulate(const SimulationArguments& simArguments){
	/*Read numeric parameters*/
	setFileParameters(simArguments);
	printSimulationMode();
	writeSimulatedParameters(simArguments.outputParameterRoute);
	/*CSV file to write the output. Useful for calibration*/
	openSimulationFiles(simArguments);

	/*Write file headers*/
	outputAlgaeFile<<"Depth, Column, Time, AlgaeType, LightAllowance, AlgaeTurbidity, PhotoPeriod, LightAtDepthExponent, LightAtDepth, Temperature, TemperatureAngularFrequency, TemperatureSine, DepthInMeters, PhosphorusConcentration, PhosphorusConcentrationAtBottom, PhosphorusLimitation, LimitationProduct, PhosphorusAtDepthExponent, LightAtTop, LightDifference, NormalizedLightDifference, SigmoidLightDifference, ResourceLimitationExponent, AlgaeBiomassToDepth, PhotoSynthesys, AlgaeRespiration, AlgaeExcretion, AlgaeNaturalMortality, AlgaeSedimentation, AlgaeWeightedSedimentation, AlgaeSlough, AlgaeTempMortality, AlgaeResourceLimStress, AlgaeWeightedResourceLimStress, AlgaeNaturalMortalityFactor, AlgaeVerticalMigration"<<endl;
	outputSloughFile<<"Depth, Column, Time, AlgaeType, DepthInMeters, AlgaeWashup, AlgaeBiomassDifferential, AlgaeBiomass"<<endl;
	outputGrazerFile<<"Depth, Column, Time, AlgaeType, Temperature, GrazerGrazingPerIndividual, GrazerGrazingPerIndividualPerAlgaeBiomass, GrazerUsedGrazingPerAlgaeBiomass, GrazerStroganovTemperatureAdjustment, SaltAtDepthExponent, SaltConcentration, SaltEffect, SaltExponent, Grazing, GrazingSaltAdjusted, GrazerDefecation, GrazerBasalRespiration, GrazerActiveRespiratonExponent, GrazerActiveRespirationFactor, GrazerActiveRespiration, GrazerMetabolicRespiration, GrazerNonCorrectedRespiration, GrazerCorrectedRespiration, GrazerExcretion, GrazerTempMortality, GrazerNonTempMortality, GrazerBaseMortality, SalinityMortality, GrazerMortality, PredatoryPressure, GrazerCarryingCapacity, GrazerBiomassDifferential, GrazerBiomass, AlgaeBiomassBeforeGrazing, AlgaeBiomassAfterGrazing, GrazerCount"<<endl;
	// Report start of the simulation
	cout<<"Simulation started for "<<simulation_cycles<<" cycles."<<endl;
	/*Clear lake light the day before*/
	for(int depthIndex=0; depthIndex<MAX_DEPTH_INDEX; depthIndex++){
		for(int columnIndex=0; columnIndex<MAX_COLUMN_INDEX; columnIndex++){
			this->previousLakeLightAtDepth[depthIndex][columnIndex]=0.0f;
		}
	}
	for(current_hour=0; current_hour<simulation_cycles; current_hour++){
		/* Register standard biomass and slough to file at the given hour frequency*/
		if(current_hour%TIME_MESSAGE_RESOLUTION==0)
			cout<<"Simulation hour: "<<current_hour<<endl;
		step();
		if(current_hour%TIME_OUTPUT_RESOLUTION==0){
			outputAlgaeFile<<algaeBuffer.str();
			outputSloughFile<<sloughBuffer.str();
			outputGrazerFile<<grazerBuffer.str();
		}
#ifdef CHECK_ASSERTIONS
		outputAssertionViolationFile<<assertionViolationBuffer.str();
		assertionViolationBuffer.str("");
#endif
#ifdef CHECK_GRAZER_LOWER_LIMIT
		if(this->zooplankton_count_summing<=MINIMUM_VIABLE_GRAZER_POPULATION){
			cout<<"Simulation halted because the grazer summing: "<<this->zooplankton_count_summing<<" is lower than the minimum viable grazer population: "<<MINIMUM_VIABLE_GRAZER_POPULATION<<"."<<endl;
			break;
		}
#endif
	}
	closeSimulationFiles();
	cout<<"Simulation finished."<<endl;
	return 0;
}


void FoodWebModel::FoodWebModel::openSimulationFiles(const SimulationArguments& simArguments){

	outputAlgaeFile.open(simArguments.outputAlgaeRoute.c_str());
	outputGrazerFile.open(simArguments.outputGrazerRoute.c_str());
	outputSloughFile.open(simArguments.outputSloughRoute.c_str());
#ifdef CHECK_ASSERTIONS
	outputAssertionViolationFile.open(simArguments.outputAssertionViolationRoute.c_str());
#endif
	/*Report successfully or not successfully open files*/
	if (outputAlgaeFile.is_open()){
		cout<<"File "<<simArguments.outputAlgaeRoute<<" open for algae biomass output."<<endl;
	} else{
		cout<<"File "<<simArguments.outputAlgaeRoute<<" could not be opened for algae biomass output."<<endl;
	}
	if (outputSloughFile.is_open()){
		cout<<"File "<<simArguments.outputSloughRoute<<" open for slough register."<<endl;
	} else{
		cout<<"File "<<simArguments.outputSloughRoute<<" could not be opened for slough register."<<endl;
	}
	if (outputGrazerFile.is_open()){
		cout<<"File "<<simArguments.outputGrazerRoute<<" open for grazer register."<<endl;
	} else{
		cout<<"File "<<simArguments.outputGrazerRoute<<" could not be opened for grazer register."<<endl;
	}

#ifdef CHECK_ASSERTIONS
	if (outputAssertionViolationFile.is_open()){
		cout<<"File "<<simArguments.outputAssertionViolationRoute<<" open for violated assertions register."<<endl;
	} else{
		cout<<"File "<<simArguments.outputAssertionViolationRoute<<" could not be opened for violated assertions register."<<endl;
	}

#endif

}

void FoodWebModel::FoodWebModel::closeSimulationFiles(){
	outputAlgaeFile.close();
	outputSloughFile.close();
	outputGrazerFile.close();
#ifdef CHECK_ASSERTIONS
	outputAssertionViolationFile.close();
#endif
}
void FoodWebModel::FoodWebModel::step(){
	updateAlgaeBiomass();
	updateZooplanktonBiomass();
}

void FoodWebModel::FoodWebModel::updateAlgaeBiomass(){
// All algae biomass must be a well-formed number greater than 0
#ifdef CHECK_ASSERTIONS
	for(int columnIndex=0; columnIndex<MAX_COLUMN_INDEX; columnIndex++){
		if(isnan(periBiomass[columnIndex])||isinf(periBiomass[columnIndex])){
			assertionViolationBuffer<<"NanInfPeri; Peri: "<<periBiomass[columnIndex]<<", Column: "<<columnIndex<<", Time: "<<current_hour<<endl;
		}
		if(periBiomass[columnIndex]<0.0f){
			assertionViolationBuffer<<"NegativePeri; Peri: "<<periBiomass[columnIndex]<<", Column: "<<columnIndex<<", Time: "<<current_hour<<endl;
		}
		for(int depthIndex=0; depthIndex<MAX_DEPTH_INDEX; depthIndex++){
			if(isnan(phytoBiomass[depthIndex][columnIndex])||isinf(phytoBiomass[depthIndex][columnIndex])){
				assertionViolationBuffer<<"NanInfPhyto: "<<phytoBiomass[depthIndex][columnIndex]<<", Depth: "<<depthIndex<<", Column: "<<columnIndex<<", Time: "<<current_hour<<endl;
			}
			if(phytoBiomass[depthIndex][columnIndex]<0.0f){
				assertionViolationBuffer<<"NegativePhyto: "<<phytoBiomass[depthIndex][columnIndex]<<", Depth: "<<depthIndex<<", Column: "<<columnIndex<<", Time: "<<current_hour<<endl;
			}
		}
	}
#endif
	/*Use different algae biomass differential weights for burn-in and production*/
//	algae_biomass_differential_scale=current_hour<=BURNIN_MAX_CYCLE?ALGAE_BIOMASS_DIFFERENTIAL_BURNIN_SCALE:this->algae_biomass_differential_production_scale;
	algae_biomass_differential_scale=algae_biomass_differential_production_scale;
	algaeBuffer.str("");
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
		/*Update biomass and output new biomass*/
		biomassType previousPeriBiomass=periBiomass[columnIndex];
		periBiomassDifferential[columnIndex] = algaeBiomassDifferential(maxDepthIndex[columnIndex], columnIndex, true);
#ifdef CHECK_ASSERTIONS
		if(isnan(periBiomassDifferential[columnIndex])||isinf(periBiomassDifferential[columnIndex])){
			assertionViolationBuffer<<"NanInfPeriDiff; Peri: "<<periBiomassDifferential[columnIndex]<<", Column: "<<columnIndex<<", Time: "<<current_hour<<endl;
		}
#endif
		/* Save periphyton biomass for later registering in file*/
		std::ostringstream copyBuffer;
		copyBuffer.str("");
		copyBuffer.clear();
		copyBuffer<<lineBuffer.str();
		for(int depthIndex=0; depthIndex<MAX_DEPTH_INDEX; depthIndex++){
			lineBuffer.str("");
			lineBuffer.clear();
			/* Do not register biomass for empty cells. Since this is the bottom of the lake, do not register biomass below it*/
			if(depthIndex>maxDepthIndex[columnIndex]){
				phytoBiomass[depthIndex][columnIndex]=0.0f;
				registerPhytoBiomass[depthIndex][columnIndex]=false;
				break;
			} else{
				/*Set if biomass must be registered*/
				registerPhytoBiomass[depthIndex][columnIndex]=depthIndex%DEPTH_OUTPUT_RESOLUTION==0&&columnIndex%COLUMN_OUTPUT_RESOLUTION==0;
				/*Update biomass and output new biomass*/
				biomassType previousPhytoBiomass=phytoBiomass[depthIndex][columnIndex];
				phytoBiomassDifferential[depthIndex][columnIndex] = algaeBiomassDifferential(depthIndex, columnIndex, false);
				phytoBiomassDifferential[depthIndex][columnIndex]+=verticalMigratedPhytoBiomass[depthIndex][columnIndex];
				phytoBiomassDifferential[depthIndex][columnIndex]*=algae_biomass_differential_scale;
				phytoBiomass[depthIndex][columnIndex]=max((double)0.0f, phytoBiomass[depthIndex][columnIndex]+phytoBiomassDifferential[depthIndex][columnIndex]);
#ifdef CHECK_ASSERTIONS
				if(isnan(phytoBiomass[depthIndex][columnIndex])||isinf(phytoBiomass[depthIndex][columnIndex])){
					assertionViolationBuffer<<"NanInfPhyto: "<<phytoBiomass[depthIndex][columnIndex]<<", Depth: "<<depthIndex<<", Column: "<<columnIndex<<", Time: "<<current_hour<<endl;
				}
				if(phytoBiomass[depthIndex][columnIndex]<0.0f){
					assertionViolationBuffer<<"NegativePhyto: "<<phytoBiomass[depthIndex][columnIndex]<<", Depth: "<<depthIndex<<", Column: "<<columnIndex<<", Time: "<<current_hour<<endl;
				}
				if(phytoBiomass[depthIndex][columnIndex]!=0.0f&&(phytoBiomass[depthIndex][columnIndex]!=previousPhytoBiomass+phytoBiomassDifferential[depthIndex][columnIndex])){
					assertionViolationBuffer<<"UpdatePhyto; Phyto: "<<phytoBiomass[depthIndex][columnIndex]<<", PreviousPhyto: "<<previousPhytoBiomass<<", PhytoDiff: "<<phytoBiomassDifferential[depthIndex][columnIndex]<<", PhytoError: "<<phytoBiomass[depthIndex][columnIndex]-(previousPhytoBiomass+phytoBiomassDifferential[depthIndex][columnIndex])<<", Depth: "<<depthIndex<<", Column: "<<columnIndex<<", Time: "<<current_hour<<endl;
				}
#endif

				lineBuffer<<commaString<<verticalMigratedPhytoBiomass[depthIndex][columnIndex]<<endl;
				/*If biomass must be registered, register standard phytoplankton biomass*/
				if(registerPhytoBiomass[depthIndex][columnIndex]){
					algaeBuffer<<lineBuffer.str();
					//cout<<"Biomass: "<<phytoBiomass[depthIndex][columnIndex]<<", Differential: "<<phytoBiomassDifferential[depthIndex][columnIndex]<<"."<<endl;
				}

			}
		}
		/* Retrieve line registering periphyton biomass*/
		lineBuffer.str("");
		lineBuffer.clear();
		lineBuffer<<copyBuffer.str();
		/* Update vertical migration biomass*/
		periBiomassDifferential[columnIndex]+=verticalMigratedPeriBiomass[columnIndex];
		periBiomassDifferential[columnIndex]*=algae_biomass_differential_scale;
		periBiomass[columnIndex]=max((double)0.0f, periBiomass[columnIndex]+periBiomassDifferential[columnIndex]);
#ifdef CHECK_ASSERTIONS
		if(isnan(periBiomass[columnIndex])||isinf(periBiomass[columnIndex])){
			assertionViolationBuffer<<"NanInfPeri; Peri: "<<periBiomass[columnIndex]<<", Column: "<<columnIndex<<", Time: "<<current_hour<<endl;
		}
		if(periBiomass[columnIndex]<0.0f){
			assertionViolationBuffer<<"NegativePeri; Peri: "<<periBiomass[columnIndex]<<", Column: "<<columnIndex<<", Time: "<<current_hour<<endl;
		}
		if(periBiomass[columnIndex]!=0.0f&&(periBiomass[columnIndex]!=previousPeriBiomass+periBiomassDifferential[columnIndex])){
			assertionViolationBuffer<<"UpdatePeri; Peri: "<<periBiomass[columnIndex]<<", PreviousPeri: "<<previousPeriBiomass<<", PeriDiff: "<<periBiomassDifferential[columnIndex]<<", PeriError: "<<periBiomass[columnIndex]-(previousPeriBiomass+periBiomassDifferential[columnIndex])<<", Column: "<<columnIndex<<", Time: "<<current_hour<<endl;
		}
#endif
		lineBuffer<<commaString<<verticalMigratedPeriBiomass[columnIndex]<<endl;
		/*If biomass must be registered, register standard and slough periphyton biomass*/
		if(registerPeriBiomass){
			algaeBuffer<<lineBuffer.str();
			sloughBuffer<<this->maxDepthIndex[columnIndex]<<commaString<<columnIndex<<commaString<<current_hour<<commaString<<1<<commaString<<depthVector[columnIndex]<<commaString<<0.0f<<commaString<<periBiomassDifferential[columnIndex]<<commaString<<periBiomass[columnIndex]<<endl;//Depth, Column, Type, Time, Washup
		}
	}
	/*Add slough biomass*/
	for(int columnIndex=0; columnIndex<MAX_COLUMN_INDEX; columnIndex++){
		for(int depthIndex=0; depthIndex<MAX_DEPTH_INDEX; depthIndex++){
			phytoBiomass[depthIndex][columnIndex]=max((double)0.0f,phytoBiomass[depthIndex][columnIndex]+sloughPhytoBiomass[depthIndex][columnIndex]);
			phytoBiomassDifferential[depthIndex][columnIndex]+=sloughPhytoBiomass[depthIndex][columnIndex];
#ifdef CHECK_ASSERTIONS
				if(isnan(phytoBiomass[depthIndex][columnIndex])||isinf(phytoBiomass[depthIndex][columnIndex])){
					assertionViolationBuffer<<"NanInfPhyto: "<<phytoBiomass[depthIndex][columnIndex]<<", Depth: "<<depthIndex<<", Column: "<<columnIndex<<", Time: "<<current_hour<<endl;
				}
				if(phytoBiomass[depthIndex][columnIndex]<0.0f){
					assertionViolationBuffer<<"NegativePhyto: "<<phytoBiomass[depthIndex][columnIndex]<<", Depth: "<<depthIndex<<", Column: "<<columnIndex<<", Time: "<<current_hour<<endl;
				}

#endif

			/*If biomass must be registered and the bottom of the lake has not been reached yet, register slough biomass*/
			if(registerPhytoBiomass[depthIndex][columnIndex]&&depthIndex<=maxDepthIndex[columnIndex]){
				sloughBuffer<<depthIndex<<commaString<<columnIndex<<commaString<<current_hour<<commaString<<0<<commaString<<indexToDepth[depthIndex]<<commaString<<sloughPhytoBiomass[depthIndex][columnIndex]<<commaString<<phytoBiomassDifferential[depthIndex][columnIndex]<<commaString<<phytoBiomass[depthIndex][columnIndex]<<endl;//Depth, Column, Type, Time, Washup, BiomassDifferential, Biomass

			}
		}
	}

}

/*
 * Differential of biomass for photoplankton and periphyton at each time step
 */

biomassType FoodWebModel::FoodWebModel::algaeBiomassDifferential(int depthIndex, int columnIndex, bool periPhyton){

	/*Calculate temporal and spatially local values that will be used to calculate biomass differential*/
	biomassType localPointBiomass=periPhyton?periBiomass[columnIndex]:phytoBiomass[depthIndex][columnIndex];

	/* Calculate nutrient limitations*/
	photoPeriod();
	calculateLightAtTop();
	lightAtDepth(depthIndex, columnIndex);
	normalizeLight(columnIndex);
	lightAllowance(localPointBiomass);
	phosphorusConcentrationAtDepth(depthIndex, columnIndex);
	calculateNutrientLimitation(localPointBiomass);
#ifdef LIMITATION_MINIMUM
	physicalType localeLimitationProduct = min<physicalType>(light_allowance,nutrient_limitation);
#else
	physicalType localeLimitationProduct = light_allowance*nutrient_limitation;
#endif
	/* Calculate biomass differential components*/
	localeLimitationProduct*=this->limitation_scale_weight;
	photoSynthesis(localPointBiomass, localeLimitationProduct, periPhyton);
	algaeSinking(depthIndex, columnIndex);
    algaeSlough(columnIndex);
    /**
     * Temperature at surface 25o
     * Temperature at bottom 2o
     * And thermocline is about 8.25m
     * Model as a reversed sigmoid
     *
     */
	calculateTemperature(depthIndex, columnIndex);
	physicalType localeTemperature =temperature[depthIndex][columnIndex];
	algaeRespiration(localPointBiomass, localeTemperature);
	algaeExcretion();
	/*Formula of biomass differential (AquaTox Documentation, page 67, equations 33 and 34)*/
	algaeNaturalMortality(localeTemperature, localeLimitationProduct, localPointBiomass);
	/*Add algae constant differential if grazers are present*/
	biomassType constantBiomassDifferential=0.0f;
#ifdef ADD_CONSTANT_BIOMASS_DIFFERENTIAL
	if(depthIndex>=GRAZERS_LIMIT){
		constantBiomassDifferential = CONSTANT_ALGAE_BIOMASS_DIFFERENTIAL;
	}
#elif defined(ADD_VARIABLE_BIOMASS_DIFFERENTIAL)
	constantBiomassDifferential = this->baseAlgaeBiomassDifferential[depthIndex]*VARIABLE_ALGAE_BIOMASS_DIFFERENTIAL_WEIGHT;
#endif
	biomassType localeConstantBiomassDifferential = localPointBiomass>0.0f?constantBiomassDifferential:0.0f;
	biomassType totalProductivity =  localeConstantBiomassDifferential + photosynthesis_value+algae_respiration_value+algae_excretion_value
			+algae_natural_mortality;
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
	/* Create line buffer to write algae biomass*/
	lineBuffer.str("");
	lineBuffer.clear();
	lineBuffer<<depthIndex;
	lineBuffer<<commaString<<columnIndex;
	lineBuffer<<commaString<<current_hour;
	lineBuffer<<commaString<<periPhyton?1:0;
	lineBuffer<<commaString<<light_allowance;
	lineBuffer<<commaString<<turbidity_at_depth;
	lineBuffer<<commaString<<locale_photo_period;
	lineBuffer<<commaString<<light_at_depth_exponent;
	lineBuffer<<commaString<<light_at_depth;
	lineBuffer<<commaString<<localeTemperature;
	lineBuffer<<commaString<<temperature_angular_frequency;
	lineBuffer<<commaString<<temperature_sine;
	lineBuffer<<commaString<<depthInMeters;
	lineBuffer<<commaString<<chemical_concentration;
	lineBuffer<<commaString<<current_phosphorus_concentration_at_bottom;
	lineBuffer<<commaString<<nutrient_limitation;
	lineBuffer<<commaString<<localeLimitationProduct;
	lineBuffer<<commaString<<chemical_at_depth_exponent;
	lineBuffer<<commaString<<light_at_top;
	lineBuffer<<commaString<<light_difference;
	lineBuffer<<commaString<<normalized_light_difference;
	lineBuffer<<commaString<<light_allowance;
	lineBuffer<<commaString<<resource_limitation_exponent;
	lineBuffer<<commaString<<algae_biomass_to_depth;
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
	lineBuffer<<commaString<<algae_natural_mortality_factor;
	#ifdef STABLE_CHLOROPHYLL
		if(current_hour>=STABLE_STATE_HOUR){
			if(periPhyton){
				totalProductivity= periDifferential[columnIndex]*periBiomass[columnIndex];

			} else{
				totalProductivity= phytoDifferential[depthIndex][columnIndex]*phytoBiomass[depthIndex][columnIndex];
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
	algae_biomass_to_depth = this->diatom_attenuation_coefficient*sumPhytoBiomassToDepth(depthIndex, columnIndex)*MICROGRAM_TO_GRAM*M3_TO_LITER;
	turbidity_at_depth=TURBIDITY*depthInMeters;
	light_at_depth_exponent = -(turbidity_at_depth+algae_biomass_to_depth);
#ifdef ADDITIVE_TURBIDITY
	light_at_depth =  light_at_top*ALGAE_ATTENUATION_WEIGHT*(ALGAE_ATTENUATION_PROPORTION*exp(-algae_biomass_to_depth)+TURBIDITY_PROPORTION*exp(-turbidity_at_depth));

#else
	light_at_depth =  light_at_top*exp(this->light_steepness*light_at_depth_exponent);

#endif
	previousLakeLightAtDepth[depthIndex][columnIndex] = lakeLightAtDepth[depthIndex][columnIndex];
	lakeLightAtDepth[depthIndex][columnIndex] = light_at_depth;
}


/*
 * Summing of phytoplankton biomass up to the given depth
 */
biomassType FoodWebModel::FoodWebModel::sumPhytoBiomassToDepth(int depthIndex, int columnIndex){
	biomassType sumPhytoBiomass=0.0l;
	for(int i=0; i<depthIndex; i++){
		sumPhytoBiomass+=phytoBiomass[i][columnIndex];
	}
#ifdef CHECK_ASSERTIONS
	if(isnan(sumPhytoBiomass)||isinf(sumPhytoBiomass)){
		assertionViolationBuffer<<"NanInfPhytoSum; PhytoSum: "<<sumPhytoBiomass<<", Depth: "<<depthIndex<<", Column: "<<columnIndex<<", Time: "<<current_hour<<endl;
	}
	if(sumPhytoBiomass<0.0f){
		assertionViolationBuffer<<"NegativePhytoSum; PhytoSum: "<<sumPhytoBiomass<<", Depth: "<<depthIndex<<", Column: "<<columnIndex<<", Time: "<<current_hour<<endl;
	}
#endif
	return sumPhytoBiomass;
}


/*
 * Photosynthesis to calculate biomass differential
 */
void FoodWebModel::FoodWebModel::photoSynthesis(biomassType localPointBiomass, physicalType localeLimitationProduct, bool periPhyton){

	photosynthesis_value =  max<biomassType>(0.0002f,productionLimit(localeLimitationProduct, periPhyton)*localPointBiomass*PHOTOSYNTHESIS_FACTOR);
}

physicalType FoodWebModel::FoodWebModel::productionLimit(physicalType localeLimitationProduct, bool periPhyton){
	/*The main limiting effects are temperature, nutrient concentration, toxicant and light. Since we will not have data
	 * on nutrient concentration and the temperature model is complex, we will use only light intensity as a limiting effect*/
	/*
	 * We will also use the fraction of littoral zone for periphyton
	 */
	physicalType productionLimit= localeLimitationProduct;
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

	/* Store phosphorus concentration at bottom*/
	phosphorus_concentration_at_bottom_in_hour= new physicalType[readProcessedData.simulationCycles];
	for(int i=0; i<readProcessedData.simulationCycles; i++){
		phosphorus_concentration_at_bottom_in_hour[i]=readProcessedData.phosphorusConcentrationAtBottom[i];
	}
	/*Copy arrays that depend on maximum depth*/
	for(int i=0; i<MAX_DEPTH_INDEX; i++){
		this->temperature_range[i]=readProcessedData.temperature_range[i];
		this->indexToDepth[i]=readProcessedData.depth_scale[i];
		this->baseAlgaeBiomassDifferential[i]=readProcessedData.baseBiomassDifferential[i];

	}
	/*Copy cyclic light at surface*/
	for(int i=0; i<HOURS_PER_DAY; i++){
		this->hourlyLightAtSurface[i]=readProcessedData.hourlyLightAtSurface[i];
	}
	/*Copy zooplankton biomass center per daily hour*/
	for(int i=0; i<HOURS_PER_DAY; i++){
		this->zooplanktonBiomassCenterDifferencePerDepth[i]=readProcessedData.zooplanktonBiomassCenterDifferencePerDepth[i];
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
		this->periBiomass[i]=readProcessedData.initial_algae_biomass[maxDepthIndex[i]][i];
		this->bottomFeederCount[i]=readProcessedData.initial_grazer_count[maxDepthIndex[i]][i];

		for(int j=0; j<MAX_DEPTH_INDEX; j++){
			this->temperature[j][i] = this->initial_temperature[j][i] = readProcessedData.initial_temperature[j][i];
			this->phytoDifferential[j][i]=0.0f;
			if(j==maxDepthIndex[i]){
				/* If the cell depth is greater than the lake depth, biomass is 0 (out of the lake)*/
				this->phytoBiomass[j][i]=0.0f;
				this->zooplanktonCount[j][i]=0;

				break;
			} else {
				/* If we are modeling any biomass that it is not at the bottom, then all initial biomass is phytoplankton*/
				this->phytoBiomass[j][i]=readProcessedData.initial_algae_biomass[j][i];
				this->zooplanktonCount[j][i]=readProcessedData.initial_grazer_count[j][i];
			}
		}
	}
	setBathymetricParameters();
	calculateDistanceToFocus();
}

FoodWebModel::FoodWebModel::FoodWebModel(const SimulationArguments& simArguments){
/* Read the geophysical parameters from the lake, including depth and temperature at water surface
 *
 * */
	cout<<"Reading parameters."<<endl;
	readProcessedData.readModelData(simArguments);
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
	algae_respiration_value = -min(photosynthesis_value*MAX_RESPIRATION_PHOTOSYNTHESIS_RATE,this->algal_respiration_at_20_degrees*pow(this->exponential_temperature_algal_respiration_coefficient, localTemperature-20)*localPointBiomass);
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
	algae_natural_mortality_factor = this->intrinsic_algae_mortality_rate+high_temperature_mortality+weighted_resource_limitation_stress;
	algae_natural_mortality = -algae_natural_mortality_factor*localPointBiomass;
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
	resource_limitation_exponent = -this->maximum_algae_resources_death*(1-localeLimitationProduct);
	resource_limitation_stress= 1.0f-exp(resource_limitation_exponent);
	weighted_resource_limitation_stress = resource_limitation_stress;
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
	/* Temperature can be modeled as a sigmoidal function sloping at the thermocline or as a sinusoidal function*/
#ifdef IBM_MODEL_TEMPERATURE
	physicalType updatedTemperature = localeTemperature+(-1.0*(temperature_range[depthIndex]))*temperature_sine;
#else
	physicalType updatedTemperature = (1-1/(1+exp(-TEMPERATURE_SLOPE_AMPLITUDE*(indexToDepth[depthIndex]-ZMax/2)+TEMPERATURE_SLOPE_PHASE)))*TEMPERATURE_ADDITIVE_COMPONENT+TEMPERATURE_LOWER_LIMIT;
#endif
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
//		this->temperature[i] = new physicalType[MAX_COLUMN_INDEX];
//	}

}

void FoodWebModel::FoodWebModel::calculatePhysicalLakeDescriptors(){

	/* Calculate maximum and mean depth*/
	this->ZMax=this->ZMean=0;
	this->ZMaxIndex=-1;
	for(int i=0; i<MAX_COLUMN_INDEX; i++){
		if(this->ZMax<this->depthVector[i]){
			ZMax = this->depthVector[i];
			ZMaxIndex=i;
		}
		this->ZMean+=this->depthVector[i];
	}
	this->ZMean/=(physicalType)MAX_COLUMN_INDEX;

}

/* Calculation of algae biomass*/

void FoodWebModel::FoodWebModel::normalizeLight(int columnIndex){
#ifndef HOMOGENEOUS_DEPTH
	light_normalizer = algae_biomass_to_depth*depthVector[columnIndex];
#else
	light_normalizer = algae_biomass_to_depth*ZMax;
#endif
}

/*
 * Amount of light that is allowed into the biomass depth
 * Adapted from (AquaTox Documentation, page 70, equation 39)
 * */
void FoodWebModel::FoodWebModel::lightAllowance(biomassType localeAlgaeBiomass){

light_difference=light_at_depth-light_at_top;
#ifdef PROPORTIONAL_LIGHT
	/* Use a simplistic model of light decreasing exponentially with depth*/
	light_allowance = light_at_depth/light_at_top;
#elif defined(LINEAR_LIGHT)
	light_allowance = (physicalType)LIGHT_ALLOWANCE_INTERCEPT+ALGAE_BIOMASS_COEFFICIENT_LIGHT_ALLOWANCE*localeAlgaeBiomass+RADIATION_COEFFICIENT_LIGHT_ALLOWANCE*light_at_depth;
#elif defined(AQUATOX_LIGHT_ALLOWANCE)
	light_allowance = min((double)1.0f,(physicalType)this->light_allowance_weight*light_difference/(this->light_at_depth_exponent*ZMax));
#else
	light_difference=Math_E*light_difference;
	light_allowance=1/(1+exp(-1.0f*light_difference));
	//sigmoid_light_difference=1;
#endif
	light_allowance=min((double)1.0f,max(light_allowance, (double)0.0f));
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


/*Phosphorous concentration at a given depth*/

void FoodWebModel::FoodWebModel::phosphorusConcentrationAtDepth(int depthIndex, int columnIndex){
	current_phosphorus_concentration_at_bottom = PHOSPHORUS_CONCENTRATION_AT_BOTTOM;
	#ifdef TIME_VARIABLE_PHOSPHORUS_CONCENTRATION_AT_BOTTOM
	current_phosphorus_concentration_at_bottom = phosphorus_concentration_at_bottom_in_hour[current_hour];
	#endif
	chemicalConcentrationAtDepth(depthIndex, columnIndex, current_phosphorus_concentration_at_bottom);

}

/*Salt concentration at a given depth*/

void FoodWebModel::FoodWebModel::saltConcentrationAtDepth(int depthIndex, int columnIndex){
	chemicalConcentrationAtDepth(depthIndex, columnIndex, SALT_CONCENTRATION_AT_BOTTOM);
}



/*Nutrient concentration at a given depth*/

void FoodWebModel::FoodWebModel::chemicalConcentrationAtDepth(int depthIndex, int columnIndex, physicalType concentrationAtBottom){
	physicalType localeNutrientAtBottom = concentrationAtBottom;
#ifdef HOMOGENEOUS_DEPTH
	chemical_at_depth_exponent = (double)(NUTRIENT_DERIVATIVE*(this->indexToDepth[depthIndex]-ZMax));
#elif defined(RADIATED_CHEMICAL)
	chemical_at_depth_exponent = (double)(NUTRIENT_DERIVATIVE*(-this->distance_to_focus[depthIndex][columnIndex]));
#else
	chemical_at_depth_exponent = (double)(NUTRIENT_DERIVATIVE*(this->indexToDepth[depthIndex]-depthVector[columnIndex]));
#endif
	chemical_concentration =localeNutrientAtBottom*exp(chemical_at_depth_exponent);
}

/* Nutrient biomass growth limitation based on nutrient concentration */

void FoodWebModel::FoodWebModel::calculateNutrientLimitation(biomassType localPointBiomass){
	nutrient_limitation=0.0f;
	if(chemical_concentration>=PHOSPHORUS_LINEAR_THRESHOLD)
		nutrient_limitation=1.0f;
	else{
#ifdef NUTRIENT_LIMITATION_GLM
		nutrient_limitation=max((double)0.0f,(chemical_concentration*PHOSPHORUS_LIMITATION_LINEAR_COEFFICIENT_TFP+localPointBiomass*PHOSPHORUS_LIMITATION_LINEAR_COEFFICIENT_ALGAE_BIOMASS+PHOSPHORUS_INTERCEPT_GLM));
#elif defined(NUTRIENT_LIMITATION_QUOTIENT)
		biomassType chemical_concentration_in_grams=chemical_concentration*MICROGRAM_TO_GRAM*M3_TO_LITER;
		nutrient_limitation=chemical_concentration_in_grams/(chemical_concentration_in_grams+this->phosphorus_half_saturation);
#else
		nutrient_limitation=min((double)1.0f,(double)max((double)0.0f,(chemical_concentration*PHOSPHORUS_LINEAR_COEFFICIENT+PHOSPHORUS_INTERCEPT)/PHOSPHORUS_GROWTH_LIMIT));

#endif
	}
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
#ifdef RADIATED_CHEMICAL
	cout<<"Modeling chemical concentration as radiated"<<endl;
#else
	cout<<"Modeling chemical concentration as non-radiated with depth distance to column bottom"<<endl;
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
#ifdef USE_LITERATURE_AND_DATA_CONSTANTS
	cout<<"Using literature constants."<<endl;
#else
	cout<<"Using model-tuned constants."<<endl;
#endif
#ifdef ADDITIVE_TURBIDITY
	cout<<"Using additive turbidity."<<endl;
#else
	cout<<"Using multiplicative turbidity."<<endl;
#endif
#ifdef ADD_CONSTANT_BIOMASS_DIFFERENTIAL
	cout<<"Using constant base biomass differential."<<endl;
#elif defined(ADD_VARIABLE_BIOMASS_DIFFERENTIAL)
	cout<<"Using depth-dependent base biomass differential."<<endl;
#else
	cout<<"Using no base biomass differential."<<endl;
#endif
#ifdef LIMITATION_MINIMUM
	cout<<"Calculating resource limitation as minimum."<<endl;
#else
	cout<<"Calculating resource limitation as product."<<endl;
#endif
#ifdef TIME_VARIABLE_PHOSPHORUS_CONCENTRATION_AT_BOTTOM
	cout<<"Using time-variable phosphorous concentration at bottom."<<endl;
#else
	cout<<"Using constant phosphorous concentration at bottom."<<endl;
#endif
#ifdef GRAZING_EFFECT_ON_ALGAE_BIOMASS
	cout<<"Grazing removed from algae biomass."<<endl;
#else
	cout<<"Grazing kept in algae biomass."<<endl;
#endif
#ifdef MIGRATE_ZOOPLANKTON_AT_HOUR
	cout<<"Migrating zooplankton vertically at each hour of the day."<<endl;
#ifdef ZOOPLANKTON_ACCUMULATION
	cout<<"Accumulating zooplankton count at extreme depth values."<<endl;
#else
	cout<<"Discarding zooplankton excess from vertical migration."<<endl;
#endif
#elif defined(LOCALE_SEARCH_ZOOPLANKTON_MIGRATION)
	cout<<"Using locale search zooplankton migration."<<endl;
#else
	cout<<"Maintaining zooplankton at each depth at each hour of the day."<<endl;
#endif
#ifdef SATURATION_GRAZING
	cout<<"Using saturation grazing."<<endl;
#else
	cout<<"Using linear grazing."<<endl;
#endif
#ifdef ADD_GRAZER_PREDATORY_PRESSURE
	cout<<"Using sigmoidal predatory pressure on grazers."<<endl;
#else
	cout<<"Using no predatory pressure on grazers."<<endl;
#endif
#ifdef CHECK_GRAZER_LOWER_LIMIT
	cout<<"Checking grazer lower limit."<<endl;
#endif
#ifdef CHECK_ASSERTIONS
	cout<<"Checking assertions."<<endl;
#else
	cout<<"Not checking assertions."<<endl;
#endif
#ifdef CARRYING_CAPACITY_MORTALITY
	cout<<"Using carryng-capacity dependent grazer mortality."<<endl;
#else
	cout<<"Using constant grazer mortality."<<endl;
#endif
#ifdef PROPORTIONAL_LIGHT
	cout<<"Using proportional light limitation."<<endl;
#elif defined(LINEAR_LIGHT)
	cout<<"Using linear light limitation."<<endl;
#elif defined(AQUATOX_LIGHT_ALLOWANCE)
	cout<<"Using AquaTox light limitation."<<endl;
#else
	cout<<"Using sigmoid light limitation."<<endl;
#endif
#ifdef NUTRIENT_LIMITATION_GLM
	cout<<"Using GLM nutrient limitation."<<endl;
#elif defined(NUTRIENT_LIMITATION_QUOTIENT)
	cout<<"Using quotient-based nutrient limitation."<<endl;
#else
	cout<<"Using simple linear nutrient limitation."<<endl;
#endif
	cout<<"Using grazer feeding saturation adjustment weight "<<FEEDING_SATURATION_ADJUSTMENT<<"."<<endl;
	cout<<"Using grazer water filtering per individual "<<this->filtering_rate_per_daphnia_in_cell_volume<<" liters/hour."<<endl;
	cout<<"Using algae biomass differential weight "<<this->algae_biomass_differential_production_scale<<"."<<endl;
	cout<<"Using base grazer mortality factor "<<this->animal_base_mortality_proportion<<"."<<endl;
	cout<<"Using basal respiration weight "<<this->basal_respiration_weight<<"."<<endl;
	cout<<"Using respiration K value "<<this->k_value_respiration<<"."<<endl;
	cout<<"Using grazer carrying capacity coefficient "<<this->grazer_carrying_capacity_coefficient<<"."<<endl;
	cout<<"Using grazer carrying capacity intercept "<<this->grazer_carrying_capacity_intercept<<"."<<endl;
	cout<<"Using phosphorus half saturation "<<this->phosphorus_half_saturation<<"."<<endl;
	cout<<"Using light allowance weight "<<this->light_allowance_weight<<"."<<endl;
	cout<<"Using base algal respiration at 20 degrees "<<this->algal_respiration_at_20_degrees<<"."<<endl;
	cout<<"Using algal respiration exponential base coefficient "<<this->exponential_temperature_algal_respiration_coefficient<<"."<<endl;
	cout<<"Using intrinsic algae mortality rate "<<this->intrinsic_algae_mortality_rate<<"."<<endl;
	cout<<"Using maximum algae dead to stress due to lack of resources "<<this->maximum_algae_resources_death<<"."<<endl;
	cout<<"Using light steepness "<<this->light_steepness<<"."<<endl;
	cout<<"Using diatom attenuation coefficient "<<this->diatom_attenuation_coefficient<<"."<<endl;
	cout<<"Using limitation product scale weight "<<this->limitation_scale_weight<<"."<<endl;

}

/* Calculation of grazer biomass (AquaTox Documentation, page 100, equation 90)*/

void FoodWebModel::FoodWebModel::updateZooplanktonBiomass(){
	grazerBuffer.str("");
	/* Clear zooplankton count summing. This will be compared with threshold upper number as a halting condition*/
	this->zooplankton_count_summing=0;
	/*Matrix to store the decision of biomass must be saved. It will be read when registering slough to output slough file*/
	bool registerZooplanktonBiomass[MAX_DEPTH_INDEX][MAX_COLUMN_INDEX];
	/* Clear vertical migrated phyto biomass*/
	for(int columnIndex=0; columnIndex<MAX_COLUMN_INDEX; columnIndex++){
		bottomFeederBiomass[columnIndex]=0.0f;
		for(int depthIndex=0; depthIndex<MAX_DEPTH_INDEX; depthIndex++){
			zooplanktonBiomass[depthIndex][columnIndex]=0.0f;
		}
	}
	/*Migrate verically zooplankton according to current time */
#ifdef MIGRATE_ZOOPLANKTON_AT_HOUR
	verticalMigrateZooplanktonCount();
#elif defined(LOCALE_SEARCH_ZOOPLANKTON_MIGRATION)
	verticalMigrateZooplanktonAlgae();
#endif


	/*Calculate phytoplankton and periphyton biomass on the current step*/
	for(int columnIndex=0; columnIndex<MAX_COLUMN_INDEX; columnIndex++){
		lineBuffer.str("");
		lineBuffer.clear();
		bool registerBottomFeederBiomass=columnIndex%COLUMN_OUTPUT_RESOLUTION==0;
		/*Transform grazer count to biomass*/
		bottomFeederBiomass[columnIndex]=((biomassType)bottomFeederCount[columnIndex])*DAPHNIA_WEIGHT_IN_MICROGRAMS;
		biomassType previousBottomFeederBiomass = bottomFeederBiomass[columnIndex];
		/*Update biomass and output new biomass*/
		biomassType periBiomassBeforeDifferential=periBiomass[columnIndex];
		biomassType bottomFeederBiomassDifferential = grazerBiomassDifferential(maxDepthIndex[columnIndex], columnIndex, true);
		bottomFeederBiomass[columnIndex]+=bottomFeederBiomassDifferential;
#ifdef CHECK_ASSERTIONS
		if(isnan(bottomFeederBiomass[columnIndex])||isinf(bottomFeederBiomass[columnIndex])){
			assertionViolationBuffer<<"NanInfBottomFeeder; BottomFeeder: "<<bottomFeederBiomass[columnIndex]<<", Column: "<<columnIndex<<", Time: "<<current_hour<<endl;
		}
		if(bottomFeederBiomass[columnIndex]<0.0f){
			assertionViolationBuffer<<"NegativeBottomFeeder; BottomFeeder: "<<bottomFeederBiomass[columnIndex]<<", Column: "<<columnIndex<<", Time: "<<current_hour<<endl;
		}
		if(bottomFeederBiomass[columnIndex]!=previousBottomFeederBiomass+bottomFeederBiomassDifferential){
			assertionViolationBuffer<<"UpdateBottomFeeder; BottomFeeder: "<<bottomFeederBiomass[columnIndex]<<", PreviousBottomFeeder: "<<previousBottomFeederBiomass<<", BottomFeederDiff: "<<bottomFeederBiomassDifferential<<", BottomFeederError: "<<bottomFeederBiomass[columnIndex]-(previousBottomFeederBiomass+bottomFeederBiomassDifferential)<<", Column: "<<columnIndex<<", Time: "<<current_hour<<endl;
		}
#endif
		/* From biomass to discrete count*/
		bottomFeederCount[columnIndex]=ceil(bottomFeederBiomass[columnIndex]/DAPHNIA_WEIGHT_IN_MICROGRAMS);
		bottomFeederCount[columnIndex]=max<zooplanktonCountType>((zooplanktonCountType)0.0f, bottomFeederCount[columnIndex]);
		/*If biomass must be registered, register standard and slough periphyton biomass*/
		if(registerBottomFeederBiomass){
			grazerBuffer<<lineBuffer.str()<<commaString<<bottomFeederCount[columnIndex]<<endl;
		}
		for(int depthIndex=0; depthIndex<MAX_DEPTH_INDEX; depthIndex++){
			lineBuffer.str("");
			lineBuffer.clear();
			/* Do not register biomass for empty cells. Since this is the bottom of the lake, do not register biomass below it*/
			if(depthIndex>maxDepthIndex[columnIndex]){
				zooplanktonBiomass[depthIndex][columnIndex]=0.0f;
				registerZooplanktonBiomass[depthIndex][columnIndex]=false;
				break;
			} else{
				/*Set if biomass must be registered*/
				registerZooplanktonBiomass[depthIndex][columnIndex]=depthIndex%DEPTH_OUTPUT_RESOLUTION==0&&columnIndex%COLUMN_OUTPUT_RESOLUTION==0;
				/*Transform zooplankton count to biomass*/
				zooplanktonBiomass[depthIndex][columnIndex]=((biomassType)zooplanktonCount[depthIndex][columnIndex])*DAPHNIA_WEIGHT_IN_MICROGRAMS;
				biomassType previousZooplanktonBiomass = zooplanktonBiomass[depthIndex][columnIndex];
				/*Update biomass and output new biomass*/
				biomassType zooplanktonBiomassDifferential = grazerBiomassDifferential(depthIndex, columnIndex, false);
				zooplanktonBiomass[depthIndex][columnIndex]+=zooplanktonBiomassDifferential;
#ifdef CHECK_ASSERTIONS
				if(isnan(zooplanktonBiomass[depthIndex][columnIndex])||isinf(zooplanktonBiomass[depthIndex][columnIndex])){
					assertionViolationBuffer<<"NanInfZooplankton; Zooplankton: "<<zooplanktonBiomass[depthIndex][columnIndex]<<", Depth: "<<depthIndex<<", Column: "<<columnIndex<<", Time"<<current_hour<<endl;
				}
				if(zooplanktonBiomass[depthIndex][columnIndex]<0.0f){
					assertionViolationBuffer<<"NegativeZooplankton; Zooplankton: "<<zooplanktonBiomass[depthIndex][columnIndex]<<", Depth: "<<depthIndex<<", Column: "<<columnIndex<<", Time"<<current_hour<<endl;
				}
				if(zooplanktonBiomass[depthIndex][columnIndex]!=previousZooplanktonBiomass+zooplanktonBiomassDifferential){
					assertionViolationBuffer<<"UpdateZooplankton; Zooplankton: "<<zooplanktonBiomass[depthIndex][columnIndex]<<", PreviousZooplanktonBiomass: "<<previousZooplanktonBiomass<<", ZooplanktonDiff: "<<zooplanktonBiomassDifferential<<", ZooplanktonError: "<<zooplanktonBiomass[depthIndex][columnIndex]-(previousZooplanktonBiomass+zooplanktonBiomassDifferential)<<", Depth: "<<depthIndex<<", Column: "<<columnIndex<<", Time"<<current_hour<<endl;
				}
#endif
				/* From biomass to discrete count*/
				zooplanktonCount[depthIndex][columnIndex]=ceil(zooplanktonBiomass[depthIndex][columnIndex]/DAPHNIA_WEIGHT_IN_MICROGRAMS);
				zooplanktonCount[depthIndex][columnIndex]=max<zooplanktonCountType>((zooplanktonCountType)0.0f, zooplanktonCount[depthIndex][columnIndex]);
				this->zooplankton_count_summing+=zooplanktonCount[depthIndex][columnIndex];
				/*If biomass must be registered, register standard phytoplankton biomass*/
				if(registerZooplanktonBiomass[depthIndex][columnIndex]){
					grazerBuffer<<lineBuffer.str()<<commaString<<zooplanktonCount[depthIndex][columnIndex]<<endl;
				}

			}
		}
	}
}


void FoodWebModel::FoodWebModel::verticalMigrateZooplanktonCount(){





	/* Migrate biomass with respect to the previous hour*/

	/* First, calculate depth movement with respect to previous hour*/
	int depth_dependent_hour_shift =current_hour==0?INITIAL_ZOOPLANKTON_SHIFT:zooplanktonBiomassCenterDifferencePerDepth[current_hour%HOURS_PER_DAY];

	/*If there exists a movement in zooplankton across depth*/
	if(depth_dependent_hour_shift!=0){
		/* Temporary store zooplankton count in a buffer*/
		for(int depthIndex=0; depthIndex<MAX_DEPTH_INDEX; depthIndex++){
			for(int columnIndex=0; columnIndex<MAX_COLUMN_INDEX; columnIndex++){
				verticalMigrationZooplanktonBiomassBuffer[depthIndex][columnIndex] =zooplanktonCount[depthIndex][columnIndex];
				zooplanktonCount[depthIndex][columnIndex] = 0.0f;
			}
		}
		int initial_depth_index=depth_dependent_hour_shift>0?0:-depth_dependent_hour_shift,
				final_depth_index=depth_dependent_hour_shift>0?MAX_DEPTH_INDEX-depth_dependent_hour_shift:MAX_DEPTH_INDEX;

		for(int depthIndex=initial_depth_index; depthIndex<final_depth_index; depthIndex++){

			/*Adjust depth movement per depth. If it is within depth range, update zooplankton count*/
			int depth_adjustment=depthIndex+depth_dependent_hour_shift;
			for(int columnIndex=0; columnIndex<MAX_COLUMN_INDEX; columnIndex++){
				/* Move zooplankton individuals to the adjusted depth*/
				if(depth_adjustment<=maxDepthIndex[columnIndex]&&depthIndex<=maxDepthIndex[columnIndex]){
					zooplanktonCount[depth_adjustment][columnIndex] =verticalMigrationZooplanktonBiomassBuffer[depthIndex][columnIndex];
				}
			}
		}

		/* Accumulate unmoved biomass to extreme shallow or deep index*/
#ifdef ZOOPLANKTON_ACCUMULATION
		initial_depth_index=depth_dependent_hour_shift>0?MAX_DEPTH_INDEX-depth_dependent_hour_shift:0,
			final_depth_index=depth_dependent_hour_shift>0?MAX_DEPTH_INDEX:-depth_dependent_hour_shift;

		for(int depthIndex=initial_depth_index; depthIndex<final_depth_index; depthIndex++){
			for(int columnIndex=0; columnIndex<MAX_COLUMN_INDEX; columnIndex++){
				unsigned int receivingIndex=depth_dependent_hour_shift>0?maxDepthIndex[columnIndex]:0;
				zooplanktonCount[receivingIndex][columnIndex]+=verticalMigrationZooplanktonBiomassBuffer[depthIndex][columnIndex];

			}
		}
#endif
	}
//	for(int depthIndex=0; depthIndex<MAX_DEPTH_INDEX; depthIndex++){
//
//		/*Adjust depth movement per depth. If it is within depth range, update zooplankton count*/
//		int depth_adjustment=depthIndex+depth_dependent_hour_shift;
//
//		for(int columnIndex=0; columnIndex<MAX_COLUMN_INDEX; columnIndex++){
//			if(depth_adjustment<0||depth_adjustment>=MAX_DEPTH_INDEX||depth_adjustment>maxDepthIndex[columnIndex]){
//				/* Move zooplankton individuals to the adjusted depth*/
//				unsigned int receivingIndex=depth_dependent_hour_shift>0?maxDepthIndex[columnIndex]:0;
//#ifdef ZOOPLANKTON_ACCUMULATION
//				bool zooplanktionAccumulationCondition = depthIndex>maxDepthIndex[columnIndex]||;
//#else
//				bool zooplanktionAccumulationCondition = depthIndex<=maxDepthIndex[columnIndex];
//#endif
//				if(zooplanktionAccumulationCondition){
//					zooplanktonCount[receivingIndex][columnIndex]+=verticalMigrationZooplanktonBiomassBuffer[depthIndex][columnIndex];
//				}
//			}
//		}
//	}
}


/* Calculate neighboring preference score for daphnia migration. Inspired by ([1] B.-P. Han and M. Straškraba, “Modeling patterns of zooplankton diel vertical migration,” J. Plankton Res., vol. 20, no. 8, pp. 1463–1487, 1998., Eqn. 15)*/
void FoodWebModel::FoodWebModel::calculateLocalPreferenceScore() {
	for (int depthIndex = 0; depthIndex < MAX_DEPTH_INDEX; depthIndex++) {
		for (int columnIndex = 0; columnIndex < MAX_COLUMN_INDEX;
				columnIndex++) {
			/*Daphnia prefer descents in light (to avoid predators) and increases in algae*/
			this->grazerPreferenceScore[depthIndex][columnIndex] =
					((previousLakeLightAtDepth[depthIndex][columnIndex] - lakeLightAtDepth[depthIndex][columnIndex])
							/ lakeLightAtDepth[depthIndex][columnIndex])
							* (phytoBiomassDifferential[depthIndex][columnIndex]/phytoBiomass[depthIndex][columnIndex]);
		}
	}
}

/* Distribute preference score and daphnia across neighboring cells. Initially, it will be distributed in the same column.*/
void FoodWebModel::FoodWebModel::verticalMigrateZooplanktonAlgae(){
	calculateLocalPreferenceScore();
	biomassType *neighboringPreferences = new biomassType[this->vertical_migration_buffer_size];
	for (int depthIndex = 0; depthIndex < MAX_DEPTH_INDEX; depthIndex++) {
		int candidateLowerRowIndex = depthIndex-this->maximum_distance_daphnia_swum_in_rows_per_hour;
		int candidateUpperRowIndex = depthIndex+this->maximum_distance_daphnia_swum_in_rows_per_hour;
		int localeLowerRowIndex=candidateLowerRowIndex<0?0:candidateLowerRowIndex;
		for (int columnIndex = 0; columnIndex < MAX_COLUMN_INDEX;columnIndex++) {
			/* Normalize daphnia preference in neighboring cells from the same column. First, sum preference at neighboring cells*/
			biomassType neighboringPreferencesSum=0.0f;
			int localeUpperRowIndex=candidateUpperRowIndex>maxDepthIndex[columnIndex]?maxDepthIndex[columnIndex]:candidateUpperRowIndex;
			int usedMigrationBufferSize=localeUpperRowIndex-localeLowerRowIndex;
			for (int localeDepthIndex = localeLowerRowIndex; localeDepthIndex < localeUpperRowIndex; localeDepthIndex++) {
				neighboringPreferencesSum+=this->grazerPreferenceScore[localeDepthIndex][columnIndex];
				neighboringPreferences[localeDepthIndex-localeLowerRowIndex] = grazerPreferenceScore[localeDepthIndex][columnIndex];
			}
			/*Find shift towards 0-based*/
			biomassType minimumLocalePreference=neighboringPreferences[0];
			for (int localeDepthIndex = 0; localeDepthIndex < usedMigrationBufferSize; localeDepthIndex++) {
				minimumLocalePreference=min(neighboringPreferences[localeDepthIndex],minimumLocalePreference);
			}
			for (int localeDepthIndex = 0; localeDepthIndex < usedMigrationBufferSize; localeDepthIndex++) {
				neighboringPreferences[localeDepthIndex]-=minimumLocalePreference;
			}
			/* Then, normalize each cell by this summing*/
			for (int localeDepthIndex = 0; localeDepthIndex < usedMigrationBufferSize; localeDepthIndex++) {
				neighboringPreferences[localeDepthIndex]/=neighboringPreferencesSum;
			}
			/* Finally, distribute grazers using this distribution*/
			biomassType bufferedZooplanktonCount=zooplanktonCount[depthIndex][columnIndex];
			zooplanktonCount[depthIndex][columnIndex]=0.0f;
			for (int localeDepthIndex = localeLowerRowIndex; localeDepthIndex < localeUpperRowIndex; localeDepthIndex++) {
				this->zooplanktonCount[localeDepthIndex][columnIndex]+=bufferedZooplanktonCount*neighboringPreferences[localeDepthIndex-localeLowerRowIndex];
			}
		}
	}
}


biomassType FoodWebModel::FoodWebModel::grazerBiomassDifferential(int depthIndex, int columnIndex, bool bottomFeeder){
	physicalType localeTemperature = temperature[depthIndex][columnIndex];

	/* Get zooplankton count and biomass*/
	zooplanktonCountType localeZooplanktonCount = bottomFeeder?bottomFeederCount[columnIndex]:zooplanktonCount[depthIndex][columnIndex];
	biomassType localeZooplanktonBiomass = bottomFeeder?bottomFeederBiomass[columnIndex]:zooplanktonBiomass[depthIndex][columnIndex];
	biomassType localeAlgaeBiomassConcentration = bottomFeeder?this->periBiomass[columnIndex]:this->phytoBiomass[depthIndex][columnIndex];
	biomassType localeAlgaeBiomassInMicrograms = localeAlgaeBiomassConcentration*CELL_VOLUME_IN_LITER;

	stroganovApproximation(localeTemperature);
	saltConcentrationAtDepth(depthIndex, columnIndex);
	salinityEffect();
	foodConsumptionRate(depthIndex,columnIndex,bottomFeeder, localeAlgaeBiomassInMicrograms);
	biomassType localeAlgaeBiomassAfterGrazing = bottomFeeder?this->periBiomass[columnIndex]:this->phytoBiomass[depthIndex][columnIndex];

	defecation();
	animalRespiration(localeZooplanktonBiomass, localeTemperature);
	animalExcretion(salinity_corrected_zooplankton_respiration);
	animalMortality(localeZooplanktonBiomass, localeTemperature, salinity_corrected_zooplankton_respiration);
	calculatePredationPressure(localeZooplanktonCount);
	biomassType localeBiomassDifferential=used_grazing-locale_defecation-salinity_corrected_zooplankton_respiration-grazer_excretion_loss-animal_mortality-grazer_predatory_pressure;

	/* Plot grazer biomass differential*/
	lineBuffer.str("");
	lineBuffer.clear();
	lineBuffer<<depthIndex;
	lineBuffer<<commaString<<columnIndex;
	lineBuffer<<commaString<<current_hour;
	lineBuffer<<commaString<<bottomFeeder?1:0;
	lineBuffer<<commaString<<localeTemperature;
	lineBuffer<<commaString<<grazing_per_individual;
	lineBuffer<<commaString<<grazing_per_individual/localeAlgaeBiomassConcentration;
	lineBuffer<<commaString<<used_grazing/localeAlgaeBiomassConcentration;
	lineBuffer<<commaString<<stroganov_adjustment;
	lineBuffer<<commaString<<chemical_at_depth_exponent;
	lineBuffer<<commaString<<chemical_concentration;
	lineBuffer<<commaString<<salinity_effect;
	lineBuffer<<commaString<<salinity_exponent;
	lineBuffer<<commaString<<locale_grazing;
	lineBuffer<<commaString<<locale_grazing_salt_adjusted;
	lineBuffer<<commaString<<locale_defecation;
	lineBuffer<<commaString<<basal_respiration;
	lineBuffer<<commaString<<active_respiration_exponent;
	lineBuffer<<commaString<<active_respiration_factor;
	lineBuffer<<commaString<<active_respiration;
	lineBuffer<<commaString<<metabolic_respiration;
	lineBuffer<<commaString<<base_zooplankton_respiration;
	lineBuffer<<commaString<<salinity_corrected_zooplankton_respiration;
	lineBuffer<<commaString<<grazer_excretion_loss;
	lineBuffer<<commaString<<animal_temperature_mortality;
	lineBuffer<<commaString<<animal_temp_independent_mortality;
	lineBuffer<<commaString<<animal_base_mortality;
	lineBuffer<<commaString<<salinity_mortality;
	lineBuffer<<commaString<<animal_mortality;
	lineBuffer<<commaString<<grazer_predatory_pressure;
	lineBuffer<<commaString<<grazer_carrying_capacity;
	lineBuffer<<commaString<<localeBiomassDifferential;
	lineBuffer<<commaString<<localeZooplanktonBiomass;
	lineBuffer<<commaString<<localeAlgaeBiomassConcentration;
	lineBuffer<<commaString<<localeAlgaeBiomassAfterGrazing;
	return localeBiomassDifferential;
}


/* Food consumption (AquaTox Documentation, page 105, equation 98)*/
void FoodWebModel::FoodWebModel::foodConsumptionRate(int depthIndex, int columnIndex, bool bottomFeeder, biomassType algaeBiomassInMicrograms){
	zooplanktonCountType localeZooplanktonCount=bottomFeeder?bottomFeederCount[columnIndex]:zooplanktonCount[depthIndex][columnIndex];

	grazing_per_individual = this->filtering_rate_per_daphnia_in_cell_volume*algaeBiomassInMicrograms*stroganov_adjustment;
#ifdef SATURATION_GRAZING
	grazing_per_individual = min<biomassType>(FEEDING_SATURATION,MAXIMUM_GRAZING_ABSORBED);
//	grazing_per_individual = min<biomassType>(FEEDING_SATURATION,grazing_per_individual);
#endif
	locale_grazing= grazing_per_individual*localeZooplanktonCount;
	locale_grazing_salt_adjusted=locale_grazing*salinity_effect;
	/* Grazing can be adjusted according to water salinity*/
#ifdef ADJUST_SALINITY_GRAZERS
	used_grazing=locale_grazing_salt_adjusted;
#else
	used_grazing=locale_grazing;
#endif
	 used_grazing=min<biomassType>(algaeBiomassInMicrograms, used_grazing);
	 biomassType updatedAlgaeBiomassInMicrograms = algaeBiomassInMicrograms - used_grazing;
	 biomassType updatedConcentration = updatedAlgaeBiomassInMicrograms/CELL_VOLUME_IN_LITER;
#ifdef GRAZING_EFFECT_ON_ALGAE_BIOMASS
	if(bottomFeeder){
		periBiomass[columnIndex]= updatedConcentration;
	} else{
		phytoBiomass[depthIndex][columnIndex] = updatedConcentration;
	}
#endif
}

/* Food consumption (AquaTox Documentation, page 105, equation 97)*/
void FoodWebModel::FoodWebModel::defecation(){
	locale_defecation = DEFECATION_COEFFICIENT*used_grazing;
}

/* Zooplankton respiration (AquaTox Documentation, page 106, equation 100)*/
void FoodWebModel::FoodWebModel::animalRespiration(biomassType zooBiomass, physicalType localeTemperature){
	base_zooplankton_respiration = RESPIRATION_ADJUSTMENT*(basalRespiration(zooBiomass) + metabolicFoodConsumption());
	salinity_corrected_zooplankton_respiration = base_zooplankton_respiration*salinity_effect;
}


/* Basal respiration (AquaTox Documentation, page 106, equation 101)*/
biomassType FoodWebModel::FoodWebModel::basalRespiration(biomassType zooBiomass){
	basal_respiration =zooBiomass*BASAL_RESPIRATION_RATE*this->basal_respiration_weight*stroganov_adjustment;
	return basal_respiration;

}

/* An approximation of Stroganov function between 0 and 15 degrees (AquaTox Documentation, page 84, figure 59)*/
void FoodWebModel::FoodWebModel::stroganovApproximation(physicalType localeTemperature){
	stroganov_adjustment= (0.1f+0.011f*localeTemperature)*STROGANOV_ADJUSTMENT;
}

/* Active respiration (AquaTox Documentation, page 107, equation 104)*/
biomassType FoodWebModel::FoodWebModel::activeRespiration(biomassType zooBiomass, physicalType localeTemperature){
	active_respiration_exponent = RATE_RESPIRATION_CHANGE_10DEG*localeTemperature;
	active_respiration_factor=exp(active_respiration_exponent);
	active_respiration = zooBiomass*active_respiration_factor;
	return active_respiration;
}


/* Specific dynamic action respiration (AquaTox Documentation, page 109, equation 110)*/
biomassType FoodWebModel::FoodWebModel::metabolicFoodConsumption(){
	metabolic_respiration= this->k_value_respiration*(used_grazing-locale_defecation);
	return metabolic_respiration;
}

/* Grazer excretion biomass loss (AquaTox Documentation, page 109, equation 111)*/
void FoodWebModel::FoodWebModel::animalExcretion(biomassType localeRespiration){
	grazer_excretion_loss= EXCRETION_RESPIRATION_PROPORTION*localeRespiration;
}


/* Grazer mortality (AquaTox Documentation, page 110, equation 112)*/
void FoodWebModel::FoodWebModel::animalMortality(biomassType localeBiomass, physicalType localeTemperature, physicalType localeSalinityConcentration){
	animalBaseMortality(localeTemperature, localeBiomass);
	salinityMortality(localeBiomass);
	animal_mortality = animal_base_mortality+salinity_mortality;
}

/* Grazer base mortality (AquaTox Documentation, page 110, equation 113)*/
void FoodWebModel::FoodWebModel::animalBaseMortality(physicalType
		localeTemperature, biomassType localeBiomass){
	animalTemperatureMortality(localeTemperature, localeBiomass);
	calculateCarryingCapacity(localeBiomass);
#ifdef CARRYING_CAPACITY_MORTALITY
	animal_temp_independent_mortality = this->grazer_carrying_capacity*localeBiomass;
#else
	animal_temp_independent_mortality = this->animal_base_mortality_proportion*localeBiomass;
#endif
	animal_base_mortality= animal_temperature_mortality+animal_temp_independent_mortality;
}

/* Grazer base mortality (AquaTox Documentation, page 110, equation 114)*/
void FoodWebModel::FoodWebModel::animalTemperatureMortality(physicalType localeTemperature, biomassType localeBiomass){
	if(localeTemperature<=MAXIMUM_TOLERABLE_TEMPERATURE){
		animal_temperature_mortality = 0.0f;
	} else{
		animal_temperature_mortality = localeBiomass*exp(MAXIMUM_TOLERABLE_TEMPERATURE-localeTemperature)/2.0f;
	}

}

void FoodWebModel::FoodWebModel::calculateCarryingCapacity(biomassType inputBiomass){
	grazer_carrying_capacity =  1/(1+exp(-inputBiomass/(MAXIMUM_FOUND_GRAZER_BIOMASS*this->grazer_carrying_capacity_coefficient)+this->grazer_carrying_capacity_intercept));
}

/* Salinity effect on respiration and mortality (AquaTox Documentation, page 295, equation 440)*/
void FoodWebModel::FoodWebModel::salinityEffect(){
	salinity_effect=1;
	salinity_exponent=0.0f;
	biomassType salinityBase=1.0f;
	if(chemical_concentration<MIN_SALINITY){
		salinity_exponent=chemical_concentration-MIN_SALINITY;
		salinity_effect=SALINITY_COEFFICIENT_LOW*exp(salinity_exponent);
	}
	if(chemical_concentration>MAX_SALINITY){
		salinity_exponent=MAX_SALINITY-chemical_concentration;
		salinity_effect=SALINITY_COEFFICIENT_HIGH*exp(salinity_exponent);
	}

}

/* Salinity mortality (AquaTox Documentation, page 110, equation 112)*/
void FoodWebModel::FoodWebModel::salinityMortality(biomassType localeBiomass){
		/* Salinity mortality can be present or not*/
#ifdef ADJUST_SALINITY_GRAZERS
	salinity_mortality=localeBiomass*(1-salinity_effect);
#else
	salinity_mortality = 0.0f;
#endif
}


/* Include predation pressure to control grazer values*/
void FoodWebModel::FoodWebModel::calculatePredationPressure(zooplanktonCountType zooplanktonLocaleCount){
#ifdef ADD_GRAZER_PREDATORY_PRESSURE
	/* Use a sigmoid function to model predatory pressure*/
	grazer_predatory_pressure = 1/(1+exp(-(biomassType)zooplanktonLocaleCount+INITIAL_PREDATORY_PRESSURE));
#else
	grazer_predatory_pressure = 0.0f;
#endif
}




void FoodWebModel::FoodWebModel::calculateDistanceToFocus(){
	for(int depthIndex=0; depthIndex<MAX_DEPTH_INDEX; depthIndex++){
		for(int columnIndex=0; columnIndex<MAX_COLUMN_INDEX; columnIndex++){
			distance_to_focus[depthIndex][columnIndex] =sqrt(pow(ZMax-indexToDepth[depthIndex],2)+pow((ZMaxIndex-columnIndex)*COLUMN_TO_METER,2));
		}
	}

}

void FoodWebModel::FoodWebModel::writeSimulatedParameters(const string& parameterSimulationRoute){
	ofstream parameterFileStream;
	parameterFileStream.open(parameterSimulationRoute.c_str());
	if (parameterFileStream.is_open()){
		cout<<"File "<<parameterSimulationRoute<<" open for simulation parameter register."<<endl;
		parameterFileStream<<"AlgaeBiomassDifferentialScale;"<<this->algae_biomass_differential_production_scale<<endl;
		parameterFileStream<<"AnimalBaseMortality;"<<this->animal_base_mortality_proportion<<endl;
		parameterFileStream<<"SimulationCycles;"<<this->simulation_cycles<<endl;
		parameterFileStream<<"FilteringRatePerDaphnia;"<<this->filtering_rate_per_daphnia<<endl;
		parameterFileStream<<"BasalRespirationWeight;"<<this->basal_respiration_weight<<endl;
		parameterFileStream<<"KValueRespiration;"<<this->k_value_respiration<<endl;
		parameterFileStream<<"GrazerCarryingCapacityCoefficient;"<<this->grazer_carrying_capacity_coefficient<<endl;
		parameterFileStream<<"GrazerCarryingCapacityIntercept;"<<this->grazer_carrying_capacity_intercept<<endl;
		parameterFileStream<<"PhosphorusHalfSaturation;"<<this->phosphorus_half_saturation<<endl;
		parameterFileStream<<"LightAllowanceWeight;"<<this->light_allowance_weight<<endl;
		parameterFileStream<<"Respiration20Degrees;"<<this->algal_respiration_at_20_degrees<<endl;
		parameterFileStream<<"AlgalRespirationExponentialTemperatureCoefficient;"<<this->exponential_temperature_algal_respiration_coefficient<<endl;
		parameterFileStream<<"MaximumAlgaeDeathResources;"<<this->maximum_algae_resources_death<<endl;
		parameterFileStream<<"LightSteepness;"<<this->light_steepness<<endl;
		parameterFileStream<<"IntrinsicAlgaeMortalityRate;"<<this->intrinsic_algae_mortality_rate<<endl;
		parameterFileStream<<"LimitationScaleWeight;"<<this->limitation_scale_weight<<endl;
		parameterFileStream.close();
	} else {
		cerr<<"File "<<parameterSimulationRoute<<" could not be opened for simulation parameter register."<<endl;
	}
}

/* As a first approximation, let us assume that we only have Cladocerans (Daphnia) in the system, since they are the main grazers.
 * Assume that the grazing rate of Cladocerans is constant, independent of the abundance of algae biomass in the system
 * Play with the number of Cladocerans in the system, increasing them directly proportional to temperature.
 * Model grazers as an integer number (we are modeling the number of cladocerans, not their biomass) and grazers' biomass as an array of real numbers.
 * We can assume that grazers are evenly distributed across all depth values.
 * */
