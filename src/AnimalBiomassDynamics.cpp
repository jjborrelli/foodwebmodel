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

void AnimalBiomassDynamics::reportAssertionError(int depthIndex, int columnIndex, biomassType biomass, biomassType previousBiomass, biomassType differential, bool isBottom) {
	if (isnan(biomass)||isinf(biomass)) {
		(*assertionViolationBuffer) << "NanInfAnimal; Biomass: " << "Biomass: "
				<< biomass << ", Depth: "<<depthIndex<<", Column: "
				<< columnIndex << ", Time: " << (*current_hour) << ", IsBottom: "<<(isBottom?1:0) << endl;
	}
	if (biomass < 0.0f) {
		(*assertionViolationBuffer) << "NegativeAnimal; Biomass: "
				<< biomass << ", Depth: "<<depthIndex<<", Column: "
				<< columnIndex << ", Time: " << (*current_hour) << ", IsBottom: "<<(isBottom?1:0) << endl;
	}
	if (biomass
			!= previousBiomass + differential) {
		(*assertionViolationBuffer) << "UpdateAnimal; Biomass: "
				<< biomass << ", PreviousBiomass: "
				<< previousBiomass << ", BiomassDiff: "
				<< differential << ", BiomassError: "
				<< biomass
						- (previousBiomass
								+ differential)
				<< ", Depth: "<<depthIndex<<", Column: " << columnIndex << ", Time: " << (*current_hour)
				<< ", IsBottom: "<<(isBottom?1:0) << endl;
	}
}

/* Calculation of grazer biomass (AquaTox Documentation, page 100, equation 90)*/

void AnimalBiomassDynamics::updateAnimalBiomass(){
	animalBiomassBuffer.str("");
	/* Clear zooplankton count summing. This will be compared with threshold upper number as a halting condition*/
	this->floating_animal_count_summing=0;
	/*Matrix to store the decision of biomass must be saved. It will be read when registering slough to output slough file*/
	bool registerZooplanktonBiomass[MAX_DEPTH_INDEX][MAX_COLUMN_INDEX];
	/* Clear vertical migrated phyto biomass*/
#ifndef INDIVIDUAL_BASED_ANIMALS
	for(int columnIndex=0; columnIndex<MAX_COLUMN_INDEX; columnIndex++){
		bottomAnimalBiomass[columnIndex]=0.0f;
		for(int depthIndex=0; depthIndex<MAX_DEPTH_INDEX; depthIndex++){
			floatingAnimalBiomass[depthIndex][columnIndex]=0.0f;
		}
	}
#endif
	/*Migrate verically zooplankton according to current time */
#ifdef MIGRATE_ZOOPLANKTON_AT_HOUR
	verticalMigrateAnimalCount();
#elif defined(LOCALE_SEARCH_ZOOPLANKTON_MIGRATION)
	verticalMigrateAnimalAlgae();
#endif

#ifdef INDIVIDUAL_BASED_ANIMALS
	for (int animalIndex = 0; animalIndex < bottomAnimals->size(); ++animalIndex) {
<<<<<<< HEAD
		AnimalCohort* animal=&((*bottomAnimals)[animalIndex]);
=======
		Animal* animal=&(bottomAnimals[animalIndex]);
>>>>>>> 8195f30dc2d51fc07f9ebfb14a307f90b5923092
		unsigned int depthIndex=animal->x, columnIndex=animal->y;
		lineBuffer.str("");
		lineBuffer.clear();
		bool isBottomAnimal=true;
		bool registerBiomass=columnIndex%COLUMN_OUTPUT_RESOLUTION==0;
<<<<<<< HEAD
		biomassType initialAnimalBiomass = animal->totalBiomass;
		animalCountType animalCount=animal->numberOfIndividuals;
		biomassType biomassDifferential = animalBiomassDifferential(depthIndex, columnIndex, isBottomAnimal, animalCount, initialAnimalBiomass);
		animal->totalBiomass+=biomassDifferential;
	#ifdef CHECK_ASSERTIONS
		reportAssertionError(maxDepthIndex[columnIndex], columnIndex, animal->totalBiomass, initialAnimalBiomass,
				biomassDifferential, true);
	#endif
		animal->numberOfIndividuals=ceil(animal->totalBiomass/initial_grazer_weight[animal->stage]);
=======
		biomassType initialAnimalBiomass = animal->totalWeight;
		animalCountType animalCount=animal->numberOfIndividuals;
		biomassType biomassDifferential = animalBiomassDifferential(depthIndex, columnIndex, isBottomAnimal, animalCount, initialAnimalBiomass);
		animal->totalWeight+=biomassDifferential;
	#ifdef CHECK_ASSERTIONS
		reportAssertionError(maxDepthIndex[columnIndex], columnIndex, animal->totalWeight, initialAnimalBiomass,
				bottomAnimalBiomassDifferential, true);
	#endif
		animal->numberOfIndividuals=ceil(animal->totalWeight/DAPHNIA_WEIGHT_IN_MICROGRAMS);
>>>>>>> 8195f30dc2d51fc07f9ebfb14a307f90b5923092
		animal->numberOfIndividuals=max<animalCountType>((animalCountType)0.0f, animal->numberOfIndividuals);
		this->floating_animal_count_summing+=animal->numberOfIndividuals;
		/*If biomass must be registered, register standard phytoplankton biomass*/
		if(registerZooplanktonBiomass[depthIndex][columnIndex]){
<<<<<<< HEAD
			animalBiomassBuffer<<lineBuffer.str()<<commaString<<animal->numberOfIndividuals<<commaString<<animal->totalBiomass<<endl;
=======
			animalBiomassBuffer<<lineBuffer.str()<<commaString<<floatingAnimalCount[depthIndex][columnIndex]<<commaString<<floatingAnimalBiomass[depthIndex][columnIndex]<<endl;
>>>>>>> 8195f30dc2d51fc07f9ebfb14a307f90b5923092
		}
	}
#else
	/*Calculate phytoplankton and periphyton biomass on the current step*/
	for(int columnIndex=0; columnIndex<MAX_COLUMN_INDEX; columnIndex++){
		lineBuffer.str("");
		lineBuffer.clear();
		bool registerBottomAnimalBiomass=columnIndex%COLUMN_OUTPUT_RESOLUTION==0;
		/*Transform grazer count to biomass*/
		bottomAnimalBiomass[columnIndex]=((biomassType)bottomAnimalCount[columnIndex])*DAPHNIA_WEIGHT_IN_MICROGRAMS;
		biomassType previousBottomAnimalBiomass = bottomAnimalBiomass[columnIndex];
		/*Update biomass and output new biomass*/
<<<<<<< HEAD
		biomassType bottomAnimalBiomassDifferential = animalBiomassDifferential(maxDepthIndex[columnIndex], columnIndex, true, bottomAnimalCount[columnIndex], previousBottomAnimalBiomass);
=======
		biomassType bottomAnimalBiomassDifferential = animalBiomassDifferential(maxDepthIndex[columnIndex], columnIndex, true, previousBottomAnimalBiomass, bottomAnimalCount[columnIndex]);
>>>>>>> 8195f30dc2d51fc07f9ebfb14a307f90b5923092
		bottomAnimalBiomass[columnIndex]+=bottomAnimalBiomassDifferential;
#ifdef CHECK_ASSERTIONS
		reportAssertionError(maxDepthIndex[columnIndex], columnIndex, bottomAnimalBiomass[columnIndex], previousBottomAnimalBiomass,
				bottomAnimalBiomassDifferential, true);
#endif
		/* From biomass to discrete count*/
		bottomAnimalCount[columnIndex]=ceil(bottomAnimalBiomass[columnIndex]/DAPHNIA_WEIGHT_IN_MICROGRAMS);
		bottomAnimalCount[columnIndex]=max<animalCountType>((animalCountType)0.0f, bottomAnimalCount[columnIndex]);
		/*If biomass must be registered, register standard and slough periphyton biomass*/
		if(registerBottomAnimalBiomass){
			animalBiomassBuffer<<lineBuffer.str()<<commaString<<bottomAnimalCount[columnIndex]<<commaString<<bottomAnimalBiomass[columnIndex]<<endl;
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
<<<<<<< HEAD
				biomassType floatingAnimalBiomassDifferential = animalBiomassDifferential(depthIndex, columnIndex, false, floatingAnimalCount[depthIndex][columnIndex], previousFloatingAnimalBiomass);
=======
				biomassType floatingAnimalBiomassDifferential = animalBiomassDifferential(depthIndex, columnIndex, false, previousFloatingAnimalBiomass, floatingAnimalCount[depthIndex][columnIndex]);
>>>>>>> 8195f30dc2d51fc07f9ebfb14a307f90b5923092
				floatingAnimalBiomass[depthIndex][columnIndex]+=floatingAnimalBiomassDifferential;
#ifdef CHECK_ASSERTIONS
				reportAssertionError(depthIndex, columnIndex, floatingAnimalBiomass[depthIndex][columnIndex], previousFloatingAnimalBiomass,
						floatingAnimalBiomassDifferential, false);
#endif
				/* From biomass to discrete count*/
				floatingAnimalCount[depthIndex][columnIndex]=ceil(floatingAnimalBiomass[depthIndex][columnIndex]/DAPHNIA_WEIGHT_IN_MICROGRAMS);
				floatingAnimalCount[depthIndex][columnIndex]=max<animalCountType>((animalCountType)0.0f, floatingAnimalCount[depthIndex][columnIndex]);
				this->floating_animal_count_summing+=floatingAnimalCount[depthIndex][columnIndex];
				/*If biomass must be registered, register standard phytoplankton biomass*/
				if(registerZooplanktonBiomass[depthIndex][columnIndex]){
					animalBiomassBuffer<<lineBuffer.str()<<commaString<<floatingAnimalCount[depthIndex][columnIndex]<<commaString<<floatingAnimalBiomass[depthIndex][columnIndex]<<endl;
				}

			}
		}
	}
#endif
}

//void AnimalBiomassDynamics::verticalMigrateAnimalsNoPreference(){
//
//
//
//
//
//	/* Migrate biomass with respect to the previous hour*/
//
//	/* First, calculate depth movement with respect to previous hour*/
//	int depth_dependent_hour_shift =(*current_hour)==0?INITIAL_ZOOPLANKTON_SHIFT:floatingAnimalBiomassCenterDifferencePerDepth[(*current_hour)%HOURS_PER_DAY];
//
//	/*If there exists a movement in zooplankton across depth*/
//	if(depth_dependent_hour_shift!=0){
//		/* Temporary store zooplankton count in a buffer*/
//		for(int depthIndex=0; depthIndex<MAX_DEPTH_INDEX; depthIndex++){
//			for(int columnIndex=0; columnIndex<MAX_COLUMN_INDEX; columnIndex++){
//				verticalMigrationAnimalBiomassBuffer[depthIndex][columnIndex] =floatingAnimalCount[depthIndex][columnIndex];
//				floatingAnimalCount[depthIndex][columnIndex] = 0.0f;
//			}
//		}
//		int initial_depth_index=depth_dependent_hour_shift>0?0:-depth_dependent_hour_shift,
//				final_depth_index=depth_dependent_hour_shift>0?MAX_DEPTH_INDEX-depth_dependent_hour_shift:MAX_DEPTH_INDEX;
//
//		for(int depthIndex=initial_depth_index; depthIndex<final_depth_index; depthIndex++){
//
//			/*Adjust depth movement per depth. If it is within depth range, update zooplankton count*/
//			int depth_adjustment=depthIndex+depth_dependent_hour_shift;
//			for(int columnIndex=0; columnIndex<MAX_COLUMN_INDEX; columnIndex++){
//				/* Move zooplankton individuals to the adjusted depth*/
//				if(depth_adjustment<=maxDepthIndex[columnIndex]&&depthIndex<=maxDepthIndex[columnIndex]){
//					floatingAnimalCount[depth_adjustment][columnIndex] =verticalMigrationAnimalBiomassBuffer[depthIndex][columnIndex];
//				}
//			}
//		}
//
//		/* Accumulate unmoved biomass to extreme shallow or deep index*/
//#ifdef ZOOPLANKTON_ACCUMULATION
//		initial_depth_index=depth_dependent_hour_shift>0?MAX_DEPTH_INDEX-depth_dependent_hour_shift:0,
//			final_depth_index=depth_dependent_hour_shift>0?MAX_DEPTH_INDEX:-depth_dependent_hour_shift;
//
//		for(int depthIndex=initial_depth_index; depthIndex<final_depth_index; depthIndex++){
//			for(int columnIndex=0; columnIndex<MAX_COLUMN_INDEX; columnIndex++){
//				unsigned int receivingIndex=depth_dependent_hour_shift>0?maxDepthIndex[columnIndex]:0;
//				floatingAnimalCount[receivingIndex][columnIndex]+=verticalMigrationZooplanktonBiomassBuffer[depthIndex][columnIndex];
//
//			}
//		}
//#endif
//	}
////	for(int depthIndex=0; depthIndex<MAX_DEPTH_INDEX; depthIndex++){
////
////		/*Adjust depth movement per depth. If it is within depth range, update zooplankton count*/
////		int depth_adjustment=depthIndex+depth_dependent_hour_shift;
////
////		for(int columnIndex=0; columnIndex<MAX_COLUMN_INDEX; columnIndex++){
////			if(depth_adjustment<0||depth_adjustment>=MAX_DEPTH_INDEX||depth_adjustment>maxDepthIndex[columnIndex]){
////				/* Move zooplankton individuals to the adjusted depth*/
////				unsigned int receivingIndex=depth_dependent_hour_shift>0?maxDepthIndex[columnIndex]:0;
////#ifdef ZOOPLANKTON_ACCUMULATION
////				bool zooplanktionAccumulationCondition = depthIndex>maxDepthIndex[columnIndex]||;
////#else
////				bool zooplanktionAccumulationCondition = depthIndex<=maxDepthIndex[columnIndex];
////#endif
////				if(zooplanktionAccumulationCondition){
////					zooplanktonCount[receivingIndex][columnIndex]+=verticalMigrationZooplanktonBiomassBuffer[depthIndex][columnIndex];
////				}
////			}
////		}
////	}
//}
//
//
//
//
///* Distribute preference score and daphnia across neighboring cells. Initially, it will be distributed in the same column.*/
//void AnimalBiomassDynamics::verticalMigrateAnimalsPreference(){
//	calculateLocalPreferenceScore();
//	biomassType *neighboringPreferences = new biomassType[this->vertical_migration_buffer_size];
//	for (int depthIndex = 0; depthIndex < MAX_DEPTH_INDEX; depthIndex++) {
//		int candidateLowerRowIndex = depthIndex-this->maximum_distance_daphnia_swum_in_rows_per_hour;
//		int candidateUpperRowIndex = depthIndex+this->maximum_distance_daphnia_swum_in_rows_per_hour;
//		int localeLowerRowIndex=candidateLowerRowIndex<0?0:candidateLowerRowIndex;
//		for (int columnIndex = 0; columnIndex < MAX_COLUMN_INDEX;columnIndex++) {
//			/* Normalize daphnia preference in neighboring cells from the same column. First, sum preference at neighboring cells*/
//			biomassType neighboringPreferencesSum=0.0f;
//			int localeUpperRowIndex=candidateUpperRowIndex>maxDepthIndex[columnIndex]?maxDepthIndex[columnIndex]:candidateUpperRowIndex;
//			int usedMigrationBufferSize=localeUpperRowIndex-localeLowerRowIndex;
//			for (int localeDepthIndex = localeLowerRowIndex; localeDepthIndex < localeUpperRowIndex; localeDepthIndex++) {
//				neighboringPreferencesSum+=this->foodPreferenceScore[localeDepthIndex][columnIndex];
//				neighboringPreferences[localeDepthIndex-localeLowerRowIndex] = foodPreferenceScore[localeDepthIndex][columnIndex];
//			}
//			/*Find shift towards 0-based*/
//			biomassType minimumLocalePreference=neighboringPreferences[0];
//			for (int localeDepthIndex = 0; localeDepthIndex < usedMigrationBufferSize; localeDepthIndex++) {
//				minimumLocalePreference=min(neighboringPreferences[localeDepthIndex],minimumLocalePreference);
//			}
//			for (int localeDepthIndex = 0; localeDepthIndex < usedMigrationBufferSize; localeDepthIndex++) {
//				neighboringPreferences[localeDepthIndex]-=minimumLocalePreference;
//			}
//			/* Then, normalize each cell by this summing*/
//			for (int localeDepthIndex = 0; localeDepthIndex < usedMigrationBufferSize; localeDepthIndex++) {
//				neighboringPreferences[localeDepthIndex]/=neighboringPreferencesSum;
//			}
//			/* Finally, distribute grazers using this distribution*/
//			biomassType bufferedZooplanktonCount=floatingAnimalCount[depthIndex][columnIndex];
//			floatingAnimalCount[depthIndex][columnIndex]=0.0f;
//			for (int localeDepthIndex = localeLowerRowIndex; localeDepthIndex < localeUpperRowIndex; localeDepthIndex++) {
//				this->floatingAnimalCount[localeDepthIndex][columnIndex]+=bufferedZooplanktonCount*neighboringPreferences[localeDepthIndex-localeLowerRowIndex];
//			}
//		}
//	}
//}

/* Calculate neighboring preference score for daphnia migration. Inspired by ([1] B.-P. Han and M. Stra�kraba, �Modeling patterns of zooplankton diel vertical migration,� J. Plankton Res., vol. 20, no. 8, pp. 1463�1487, 1998., Eqn. 15)*/
void AnimalBiomassDynamics::calculateLocalPreferenceScore() {
	for (int depthIndex = 0; depthIndex < MAX_DEPTH_INDEX; depthIndex++) {
		for (int columnIndex = 0; columnIndex < MAX_COLUMN_INDEX;
				columnIndex++) {
			/*Daphnia prefer descents in light (to avoid predators) and increases in algae*/
			this->foodPreferenceScore[depthIndex][columnIndex] =
					((previousLakeLightAtDepth[depthIndex][columnIndex] - lakeLightAtDepth[depthIndex][columnIndex])
							/ lakeLightAtDepth[depthIndex][columnIndex])
							* (floatingFoodBiomassDifferential[depthIndex][columnIndex]/floatingFoodBiomass[depthIndex][columnIndex]);
		}
	}
}

biomassType AnimalBiomassDynamics::animalBiomassDifferential(int depthIndex, int columnIndex, bool bottom, animalCountType animalCount, biomassType animalBiomass){
	physicalType localeTemperature = temperature[depthIndex][columnIndex];

		/* Get zooplankton count and biomass*/
		biomassType localeAlgaeBiomassBeforeEating=bottom?this->bottomFoodBiomass[columnIndex]:this->floatingFoodBiomass[depthIndex][columnIndex];
		biomassType localeAlgaeBiomassInMicrograms = localeAlgaeBiomassBeforeEating*this->food_conversion_factor;

		stroganovApproximation(localeTemperature);
		foodConsumptionRate(depthIndex,columnIndex,bottom, animalCount, localeAlgaeBiomassInMicrograms);
		biomassType localeAlgaeBiomassAfterEating = bottom?this->bottomFoodBiomass[columnIndex]:this->floatingFoodBiomass[depthIndex][columnIndex];

		defecation();
		animalRespiration(animalBiomass, localeTemperature, salinity_effect_matrix[depthIndex][columnIndex]);
		animalExcretion(salinity_corrected_animal_respiration);
		animalMortality(animalBiomass, localeTemperature, salinity_effect_matrix[depthIndex][columnIndex]);
		calculatePredationPressure(animalCount);

		biomassType localeBiomassDifferential=used_consumption-locale_defecation-salinity_corrected_animal_respiration-animal_excretion_loss-animal_mortality-animal_predatory_pressure;

		/* Plot grazer biomass differential*/
		lineBuffer.str("");
		lineBuffer.clear();
		lineBuffer<<depthIndex;
		lineBuffer<<commaString<<columnIndex;
		lineBuffer<<commaString<<(*current_hour);
		lineBuffer<<commaString<<bottom?1:0;
		lineBuffer<<commaString<<consumption_per_individual;
		lineBuffer<<commaString<<consumption_per_individual/localeAlgaeBiomassBeforeEating;
		lineBuffer<<commaString<<used_consumption/localeAlgaeBiomassBeforeEating;
		lineBuffer<<commaString<<stroganov_adjustment;
		lineBuffer<<commaString<<locale_consumption;
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
		lineBuffer<<commaString<<animal_carrying_capacity;
		lineBuffer<<commaString<<localeBiomassDifferential;
		lineBuffer<<commaString<<localeAlgaeBiomassBeforeEating;
		lineBuffer<<commaString<<localeAlgaeBiomassAfterEating;
		return localeBiomassDifferential;
}



/* Food consumption (AquaTox Documentation, page 105, equation 98)*/
void AnimalBiomassDynamics::foodConsumptionRate(int depthIndex, int columnIndex, bool bottom, animalCountType animalCount, biomassType foodBiomassInMicrograms){

	consumption_per_individual = this->filtering_rate_per_daphnia_in_cell_volume*foodBiomassInMicrograms*stroganov_adjustment;
#ifdef SATURATION_GRAZING
	consumption_per_individual = min<biomassType>(FEEDING_SATURATION,MAXIMUM_GRAZING_ABSORBED);
//	grazing_per_individual = min<biomassType>(FEEDING_SATURATION,grazing_per_individual);
#endif
	locale_consumption= consumption_per_individual*animalCount;
	locale_consumption_salt_adjusted=locale_consumption*salinity_effect_matrix[depthIndex][columnIndex];
	/* Grazing can be adjusted according to water salinity*/
#ifdef ADJUST_SALINITY_GRAZERS
	used_consumption=locale_consumption_salt_adjusted;
#else
	used_consumption=locale_consumption;
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
void AnimalBiomassDynamics::animalRespiration(biomassType zooBiomass, physicalType localeTemperature, physicalType localeSalinityEffect){
	base_animal_respiration = RESPIRATION_ADJUSTMENT*(basalRespiration(zooBiomass) + metabolicFoodConsumption());
	salinity_corrected_animal_respiration = base_animal_respiration*localeSalinityEffect;
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
	animal_temp_independent_mortality = this->animal_carrying_capacity*localeBiomass;
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
	animal_carrying_capacity =  1/(1+exp(-inputBiomass/(this->maximum_found_animal_biomass*this->animal_carrying_capacity_coefficient)+this->animal_carrying_capacity_intercept));
}


/* Salinity mortality (AquaTox Documentation, page 110, equation 112)*/
void AnimalBiomassDynamics::salinityMortality(biomassType localeBiomass){
		/* Salinity mortality can be present or not*/
#ifdef ADJUST_SALINITY_GRAZERS
	salinity_mortality=localeBiomass*(1-salinity_effect_matrix);
#else
	salinity_mortality = 0.0f;
#endif
}

/* The levels of oxygen concentration found in the lake do not reflect significant D. pulex mortality according to: (L. J. Weider and W. Lampert, �Differential response of Daphnia genotypes to oxygen stress: respiration rates, hemoglobin content and low-oxygen tolerance,� Oecologia, vol. 65, no. 4, pp. 487�491, Mar. 1985.)*/
void AnimalBiomassDynamics::calculateLowOxigenMortality(biomassType inputBiomass){
	low_oxigen_animal_mortality=0.0f;
}

/* Include predation pressure to control grazer values*/
void AnimalBiomassDynamics::calculatePredationPressure(animalCountType zooplanktonLocaleCount){
#ifdef ADD_GRAZER_PREDATORY_PRESSURE
	/* Use a sigmoid function to model predatory pressure*/
	animal_predatory_pressure = 1/(1+exp(-(biomassType)zooplanktonLocaleCount+INITIAL_PREDATORY_PRESSURE));
#else
	animal_predatory_pressure = 0.0f;
#endif
}



} /* namespace FoodWebModel */
