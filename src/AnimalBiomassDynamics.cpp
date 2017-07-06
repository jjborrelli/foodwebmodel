/*
 * AnimalBiomassDynamics.cpp
 *
 *  Created on: Jul 6, 2017
 *      Author: manu_
 */

#include "../headers/AnimalBiomassDynamics.hpp"

using namespace std;

namespace FoodWebModel {

AnimalBiomassDynamics::AnimalBiomassDynamics() {
	// TODO Auto-generated constructor stub

}

AnimalBiomassDynamics::~AnimalBiomassDynamics() {
	// TODO Auto-generated destructor stub
}


/* Calculation of grazer biomass (AquaTox Documentation, page 100, equation 90)*/

void AnimalBiomassDynamics::updateAnimalBiomass(){
	animalBiomassBuffer.str("");
	/* Clear zooplankton count summing. This will be compared with threshold upper number as a halting condition*/
	this->floating_animal_count_summing=0;
	/*Matrix to store the decision of biomass must be saved. It will be read when registering slough to output slough file*/
	bool registerZooplanktonBiomass[MAX_DEPTH_INDEX][MAX_COLUMN_INDEX];
	/* Clear vertical migrated phyto biomass*/
	for(int columnIndex=0; columnIndex<MAX_COLUMN_INDEX; columnIndex++){
		bottomAnimalBiomass[columnIndex]=0.0f;
		for(int depthIndex=0; depthIndex<MAX_DEPTH_INDEX; depthIndex++){
			floatingAnimalBiomass[depthIndex][columnIndex]=0.0f;
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
		bool registerBottomAnimalBiomass=columnIndex%COLUMN_OUTPUT_RESOLUTION==0;
		/*Transform grazer count to biomass*/
		bottomAnimalBiomass[columnIndex]=((biomassType)bottomAnimalCount[columnIndex])*DAPHNIA_WEIGHT_IN_MICROGRAMS;
		biomassType previousBottomAnimalBiomass = bottomAnimalBiomass[columnIndex];
		/*Update biomass and output new biomass*/
		biomassType bottomAnimalBiomassDifferential = animalBiomassDifferential(maxDepthIndex[columnIndex], columnIndex, true);
		bottomAnimalBiomass[columnIndex]+=bottomAnimalBiomassDifferential;
#ifdef CHECK_ASSERTIONS
		if(isnan(bottomAnimalBiomass[columnIndex])||isinf(bottomAnimalBiomass[columnIndex])){
			assertionViolationBuffer<<"NanInfBottomFeeder; BottomFeeder: "<<bottomAnimalBiomass[columnIndex]<<", Column: "<<columnIndex<<", Time: "<<(*current_hour)<<endl;
		}
		if(bottomAnimalBiomass[columnIndex]<0.0f){
			assertionViolationBuffer<<"NegativeBottomFeeder; BottomFeeder: "<<bottomAnimalBiomass[columnIndex]<<", Column: "<<columnIndex<<", Time: "<<(*current_hour)<<endl;
		}
		if(bottomAnimalBiomass[columnIndex]!=previousBottomAnimalBiomass+bottomAnimalBiomassDifferential){
			assertionViolationBuffer<<"UpdateBottomFeeder; BottomFeeder: "<<bottomAnimalBiomass[columnIndex]<<", PreviousBottomFeeder: "<<previousBottomAnimalBiomass<<", BottomFeederDiff: "<<bottomAnimalBiomassDifferential<<", BottomFeederError: "<<bottomAnimalBiomass[columnIndex]-(previousBottomAnimalBiomass+bottomAnimalBiomassDifferential)<<", Column: "<<columnIndex<<", Time: "<<(*current_hour)<<endl;
		}
#endif
		/* From biomass to discrete count*/
		bottomAnimalCount[columnIndex]=ceil(bottomAnimalBiomass[columnIndex]/DAPHNIA_WEIGHT_IN_MICROGRAMS);
		bottomAnimalCount[columnIndex]=max<zooplanktonCountType>((zooplanktonCountType)0.0f, bottomAnimalCount[columnIndex]);
		/*If biomass must be registered, register standard and slough periphyton biomass*/
		if(registerBottomAnimalBiomass){
			animalBiomassBuffer<<lineBuffer.str()<<commaString<<bottomAnimalCount[columnIndex]<<endl;
		}
		for(int depthIndex=0; depthIndex<MAX_DEPTH_INDEX; depthIndex++){
			lineBuffer.str("");
			lineBuffer.clear();
			/* Do not register biomass for empty cells. Since this is the bottom of the lake, do not register biomass below it*/
			if(depthIndex>maxDepthIndex[columnIndex]){
				floatingAnimalBiomass[depthIndex][columnIndex]=0.0f;
				registerZooplanktonBiomass[depthIndex][columnIndex]=false;
				break;
			} else{
				/*Set if biomass must be registered*/
				registerZooplanktonBiomass[depthIndex][columnIndex]=depthIndex%DEPTH_OUTPUT_RESOLUTION==0&&columnIndex%COLUMN_OUTPUT_RESOLUTION==0;
				/*Transform zooplankton count to biomass*/
				floatingAnimalBiomass[depthIndex][columnIndex]=((biomassType)floatingAnimalCount[depthIndex][columnIndex])*DAPHNIA_WEIGHT_IN_MICROGRAMS;
				biomassType previousFloatingAnimalBiomass = floatingAnimalBiomass[depthIndex][columnIndex];
				/*Update biomass and output new biomass*/
				biomassType floatingAnimalBiomassDifferential = animalBiomassDifferential(depthIndex, columnIndex, false, CELL_VOLUME_IN_LITER);
				floatingAnimalBiomass[depthIndex][columnIndex]+=floatingAnimalBiomassDifferential;
#ifdef CHECK_ASSERTIONS
				if(isnan(floatingAnimalBiomass[depthIndex][columnIndex])||isinf(floatingAnimalBiomass[depthIndex][columnIndex])){
					assertionViolationBuffer<<"NanInfZooplankton; Zooplankton: "<<floatingAnimalBiomass[depthIndex][columnIndex]<<", Depth: "<<depthIndex<<", Column: "<<columnIndex<<", Time"<<(*current_hour)<<endl;
				}
				if(floatingAnimalBiomass[depthIndex][columnIndex]<0.0f){
					assertionViolationBuffer<<"NegativeZooplankton; Zooplankton: "<<floatingAnimalBiomass[depthIndex][columnIndex]<<", Depth: "<<depthIndex<<", Column: "<<columnIndex<<", Time"<<(*current_hour)<<endl;
				}
				if(floatingAnimalBiomass[depthIndex][columnIndex]!=previousFloatingAnimalBiomass+floatingAnimalBiomassDifferential){
					assertionViolationBuffer<<"UpdateZooplankton; Zooplankton: "<<floatingAnimalBiomass[depthIndex][columnIndex]<<", PreviousZooplanktonBiomass: "<<previousFloatingAnimalBiomass<<", ZooplanktonDiff: "<<floatingAnimalBiomassDifferential<<", ZooplanktonError: "<<floatingAnimalBiomass[depthIndex][columnIndex]-(previousFloatingAnimalBiomass+floatingAnimalBiomassDifferential)<<", Depth: "<<depthIndex<<", Column: "<<columnIndex<<", Time"<<(*current_hour)<<endl;
				}
#endif
				/* From biomass to discrete count*/
				floatingAnimalCount[depthIndex][columnIndex]=ceil(floatingAnimalBiomass[depthIndex][columnIndex]/DAPHNIA_WEIGHT_IN_MICROGRAMS);
				floatingAnimalCount[depthIndex][columnIndex]=max<zooplanktonCountType>((zooplanktonCountType)0.0f, floatingAnimalCount[depthIndex][columnIndex]);
				this->floating_animal_count_summing+=floatingAnimalCount[depthIndex][columnIndex];
				/*If biomass must be registered, register standard phytoplankton biomass*/
				if(registerZooplanktonBiomass[depthIndex][columnIndex]){
					animalBiomassBuffer<<lineBuffer.str()<<commaString<<floatingAnimalCount[depthIndex][columnIndex]<<endl;
				}

			}
		}
	}
}


biomassType AnimalBiomassDynamics::animalBiomassDifferential(int depthIndex, int columnIndex, bool bottom, physicalType foodConversionFactor){
	physicalType localeTemperature = temperature[depthIndex][columnIndex];

	/* Get zooplankton count and biomass*/
	zooplanktonCountType localeAnimalCount = bottom?bottomAnimalCount[columnIndex]:floatingAnimalCount[depthIndex][columnIndex];
	biomassType localeAnimalBiomass = bottom?bottomAnimalBiomass[columnIndex]:floatingAnimalBiomass[depthIndex][columnIndex];
	biomassType localeFoodBiomassConcentration = bottom?this->bottomFoodBiomass[columnIndex]:this->floatingFoodBiomass[depthIndex][columnIndex];
	biomassType localeFoodBiomassInMicrograms = localeFoodBiomassConcentration*foodConversionFactor;

	stroganovApproximation(localeTemperature);
	foodConsumptionRate(depthIndex,columnIndex,bottom, localeFoodBiomassInMicrograms);
	biomassType localeConsumedBiomassAfterGrazing = bottom?this->bottomFoodBiomass[columnIndex]:this->floatingFoodBiomass[depthIndex][columnIndex];

	defecation();
	animalRespiration(localeAnimalBiomass, localeTemperature);
	animalExcretion(salinity_corrected_animal_respiration);
	animalMortality(localeAnimalBiomass, localeTemperature, salinity_corrected_animal_respiration);
	calculatePredationPressure(localeAnimalCount);

	biomassType localeBiomassDifferential=used_consumption-locale_defecation-salinity_corrected_animal_respiration-animal_excretion_loss-animal_mortality-animal_predatory_pressure;

	/* Plot grazer biomass differential*/
	lineBuffer.str("");
	lineBuffer.clear();
	lineBuffer<<depthIndex;
	lineBuffer<<commaString<<columnIndex;
	lineBuffer<<commaString<<current_hour;
	lineBuffer<<commaString<<bottom?1:0;
	lineBuffer<<commaString<<localeTemperature;
	lineBuffer<<commaString<<grazing_per_individual;
	lineBuffer<<commaString<<grazing_per_individual/localeFoodBiomassConcentration;
	lineBuffer<<commaString<<used_consumption/localeFoodBiomassConcentration;
	lineBuffer<<commaString<<stroganov_adjustment;
	lineBuffer<<commaString<<chemical_at_depth_exponent;
	lineBuffer<<commaString<<(*salt_concentration);
	lineBuffer<<commaString<<(*salinity_effect);
	lineBuffer<<commaString<<salinity_exponent;
	lineBuffer<<commaString<<locale_grazing;
	lineBuffer<<commaString<<locale_consumption_salt_adjusted;
	lineBuffer<<commaString<<locale_defecation;
	lineBuffer<<commaString<<basal_animal_respiration;
	lineBuffer<<commaString<<active_respiration_exponent;
	lineBuffer<<commaString<<active_respiration_factor;
	lineBuffer<<commaString<<active_respiration;
	lineBuffer<<commaString<<metabolic_respiration;
	lineBuffer<<commaString<<base_animal_respiration;
	lineBuffer<<commaString<<salinity_corrected_animal_respiration;
	lineBuffer<<commaString<<animal_excretion_loss;
	lineBuffer<<commaString<<animal_temperature_mortality;
	lineBuffer<<commaString<<animal_temp_independent_mortality;
	lineBuffer<<commaString<<animal_base_mortality;
	lineBuffer<<commaString<<salinity_mortality;
	lineBuffer<<commaString<<low_oxigen_animal_mortality;
	lineBuffer<<commaString<<animal_mortality;
	lineBuffer<<commaString<<animal_predatory_pressure;
	lineBuffer<<commaString<<grazer_carrying_capacity;
	lineBuffer<<commaString<<localeBiomassDifferential;
	lineBuffer<<commaString<<localeAnimalBiomass;
	lineBuffer<<commaString<<localeFoodBiomassConcentration;
	lineBuffer<<commaString<<localeConsumedBiomassAfterGrazing;
	return localeBiomassDifferential;
}



/* Food consumption (AquaTox Documentation, page 105, equation 98)*/
void AnimalBiomassDynamics::foodConsumptionRate(int depthIndex, int columnIndex, bool bottom, biomassType foodBiomassInMicrograms){
	zooplanktonCountType localeFloatingAnimalCount=bottom?bottomAnimalCount[columnIndex]:floatingAnimalCount[depthIndex][columnIndex];

	grazing_per_individual = this->filtering_rate_per_daphnia_in_cell_volume*foodBiomassInMicrograms*stroganov_adjustment;
#ifdef SATURATION_GRAZING
	grazing_per_individual = min<biomassType>(FEEDING_SATURATION,MAXIMUM_GRAZING_ABSORBED);
//	grazing_per_individual = min<biomassType>(FEEDING_SATURATION,grazing_per_individual);
#endif
	locale_grazing= grazing_per_individual*localeFloatingAnimalCount;
	locale_consumption_salt_adjusted=locale_grazing*salinity_effect;
	/* Grazing can be adjusted according to water salinity*/
#ifdef ADJUST_SALINITY_GRAZERS
	used_consumption=locale_consumption_salt_adjusted;
#else
	used_consumption=locale_grazing;
#endif
	 used_consumption=min<biomassType>(foodBiomassInMicrograms, used_consumption);
	 biomassType updatedAlgaeBiomassInMicrograms = foodBiomassInMicrograms - used_consumption;
	 biomassType updatedConcentration = updatedAlgaeBiomassInMicrograms/CELL_VOLUME_IN_LITER;
#ifdef GRAZING_EFFECT_ON_ALGAE_BIOMASS
	if(bottom){
		bottomFoodBiomass[columnIndex]= updatedConcentration;
	} else{
		floatingFoodBiomass[depthIndex][columnIndex] = updatedConcentration;
	}
#endif
}

/* Food consumption (AquaTox Documentation, page 105, equation 97)*/
void AnimalBiomassDynamics::defecation(){
	locale_defecation = DEFECATION_COEFFICIENT*used_consumption;
}

/* Zooplankton respiration (AquaTox Documentation, page 106, equation 100)*/
void AnimalBiomassDynamics::animalRespiration(biomassType zooBiomass, physicalType localeTemperature){
	base_animal_respiration = RESPIRATION_ADJUSTMENT*(basalRespiration(zooBiomass) + metabolicFoodConsumption());
	salinity_corrected_animal_respiration = base_animal_respiration*salinity_effect;
}


/* Basal respiration (AquaTox Documentation, page 106, equation 101)*/
biomassType AnimalBiomassDynamics::basalRespiration(biomassType zooBiomass){
	basal_animal_respiration =zooBiomass*BASAL_RESPIRATION_RATE*this->basal_respiration_weight*stroganov_adjustment;
	return basal_animal_respiration;

}

/* An approximation of Stroganov function between 0 and 15 degrees (AquaTox Documentation, page 84, figure 59)*/
void AnimalBiomassDynamics::stroganovApproximation(physicalType localeTemperature){
	stroganov_adjustment= (0.1f+0.011f*localeTemperature)*STROGANOV_ADJUSTMENT;
}

/* Active respiration (AquaTox Documentation, page 107, equation 104)*/
biomassType AnimalBiomassDynamics::activeRespiration(biomassType zooBiomass, physicalType localeTemperature){
	active_respiration_exponent = RATE_RESPIRATION_CHANGE_10DEG*localeTemperature;
	active_respiration_factor=exp(active_respiration_exponent);
	active_respiration = zooBiomass*active_respiration_factor;
	return active_respiration;
}


/* Specific dynamic action respiration (AquaTox Documentation, page 109, equation 110)*/
biomassType AnimalBiomassDynamics::metabolicFoodConsumption(){
	metabolic_respiration= this->k_value_respiration*(used_consumption-locale_defecation);
	return metabolic_respiration;
}

/* Grazer excretion biomass loss (AquaTox Documentation, page 109, equation 111)*/
void AnimalBiomassDynamics::animalExcretion(biomassType localeRespiration){
	animal_excretion_loss= EXCRETION_RESPIRATION_PROPORTION*localeRespiration;
}


/* Grazer mortality (AquaTox Documentation, page 110, equation 112)*/
void AnimalBiomassDynamics::animalMortality(biomassType localeBiomass, physicalType localeTemperature, physicalType localeSalinityConcentration){
	animalBaseMortality(localeTemperature, localeBiomass);
	salinityMortality(localeBiomass);
	calculateLowOxigenMortality(localeBiomass);
	animal_mortality = animal_base_mortality+salinity_mortality+low_oxigen_animal_mortality;
}

/* Grazer base mortality (AquaTox Documentation, page 110, equation 113)*/
void AnimalBiomassDynamics::animalBaseMortality(physicalType
		localeTemperature, biomassType localeBiomass){
	animalTemperatureMortality(localeTemperature, localeBiomass);
	calculateGrazerCarryingCapacityMortality(localeBiomass);
#ifdef GRAZER_CARRYING_CAPACITY_MORTALITY
	animal_temp_independent_mortality = this->grazer_carrying_capacity*localeBiomass;
#else
	animal_temp_independent_mortality = this->animal_base_mortality_proportion*localeBiomass;
#endif
	animal_base_mortality= animal_temperature_mortality+animal_temp_independent_mortality;
}

/* Grazer base mortality (AquaTox Documentation, page 110, equation 114)*/
void AnimalBiomassDynamics::animalTemperatureMortality(physicalType localeTemperature, biomassType localeBiomass){
	if(localeTemperature<=MAXIMUM_TOLERABLE_TEMPERATURE){
		animal_temperature_mortality = 0.0f;
	} else{
		animal_temperature_mortality = localeBiomass*exp(MAXIMUM_TOLERABLE_TEMPERATURE-localeTemperature)/2.0f;
	}

}

void AnimalBiomassDynamics::calculateGrazerCarryingCapacityMortality(biomassType inputBiomass){
	grazer_carrying_capacity =  1/(1+exp(-inputBiomass/(this->maximum_found_grazer_biomass*this->grazer_carrying_capacity_coefficient)+this->grazer_carrying_capacity_intercept));
}


/* Salinity mortality (AquaTox Documentation, page 110, equation 112)*/
void AnimalBiomassDynamics::salinityMortality(biomassType localeBiomass){
		/* Salinity mortality can be present or not*/
#ifdef ADJUST_SALINITY_GRAZERS
	salinity_mortality=localeBiomass*(1-salinity_effect);
#else
	salinity_mortality = 0.0f;
#endif
}

/* The levels of oxygen concentration found in the lake do not reflect significant D. pulex mortality according to: (L. J. Weider and W. Lampert, “Differential response of Daphnia genotypes to oxygen stress: respiration rates, hemoglobin content and low-oxygen tolerance,” Oecologia, vol. 65, no. 4, pp. 487–491, Mar. 1985.)*/
void AnimalBiomassDynamics::calculateLowOxigenMortality(biomassType inputBiomass){
	low_oxigen_animal_mortality=0.0f;
}

/* Include predation pressure to control grazer values*/
void AnimalBiomassDynamics::calculatePredationPressure(zooplanktonCountType zooplanktonLocaleCount){
#ifdef ADD_GRAZER_PREDATORY_PRESSURE
	/* Use a sigmoid function to model predatory pressure*/
	animal_predatory_pressure = 1/(1+exp(-(biomassType)zooplanktonLocaleCount+INITIAL_PREDATORY_PRESSURE));
#else
	animal_predatory_pressure = 0.0f;
#endif
}



} /* namespace FoodWebModel */
