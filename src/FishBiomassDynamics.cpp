/*
 * FishBiomassDynamics.cpp
 *
 *  Created on: 4 ene. 2018
 *      Author: manu
 */

#include "../headers/FishBiomassDynamics.hpp"

namespace FoodWebModel {


FishBiomassDynamics::FishBiomassDynamics(){
}


FishBiomassDynamics::~FishBiomassDynamics() {
	// TODO Auto-generated destructor stub
}

void FishBiomassDynamics::calculatePreyBiomass(){
//
//	/*Clear prey biomass matrix*/
//	for(int columnIndex=0; columnIndex<MAX_COLUMN_INDEX; columnIndex++){
////		bottomFoodBiomass[columnIndex]=0.0f;
//		for(int depthIndex=0; depthIndex<=this->maxDepthIndex[columnIndex]; depthIndex++){
//			floatingFoodBiomass[depthIndex][columnIndex]=0.0f;
//		}
//	}

	/*Sum all biomass from prey*/
	biomassType totalConsumedBiomass=0.0f;
	for (std::vector<AnimalCohort>::iterator it=this->preys->begin(); it!=this->preys->end(); ++it){
//		biomassType localBiomass=  it->bodyBiomass+it->gonadBiomass;
//		floatingFoodBiomass[it->x][it->y]+=localBiomass;
//		gonadBiomassProportion[it->x][it->y]=it->gonadBiomass/localBiomass;
		preyPointers[it->x][it->y]=&(*it);
	}
//	for (std::vector<AnimalCohort>::iterator it=this->bottomAnimals->begin(); it!=this->bottomAnimals->end(); ++it){
//		bottomFoodBiomass[it->y]+=it->bodyBiomass;
//	}
}


void FishBiomassDynamics::predateCohort(AnimalCohort& cohort){}



/* If the migration index is greater than 0, migrate adult and juvenile cohorts*/

void FishBiomassDynamics::calculateMigrationValues(){
	calculatePreyBiomass();
	generateMigrationIndexes();
}


/* Food consumption (AquaTox Documentation, page 105, equation 98)*/
void FishBiomassDynamics::foodConsumptionRate(int depthIndex, int columnIndex, bool bottom, animalCountType animalCount, biomassType foodBiomassInMicrograms, biomassType individualWeight, double consumedProportion){


	if(preyPointer==NULL){
		used_consumption=consumption_per_individual=0.0f;
	} else{
		/*The number of individuals consumed per planktivore is modulated by the temperature and maximized by the number of individuals in the cohort*/
		consumption_per_individual = min<biomassType>(this->planktivore_consumption_weight*preyPointer->numberOfIndividuals/(preyPointer->numberOfIndividuals+this->planktivore_saturation_constant)*stroganov_adjustment,(biomassType)preyPointer->numberOfIndividuals/animalCount);


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
	/*Get the biomass of consumed cohort*/
	biomassType consumed_biomass=used_consumption*this->initial_grazer_weight[AnimalStage::Mature];
	biomassType localeGonadBiomassProportion=preyPointer->gonadBiomass/(preyPointer->gonadBiomass+preyPointer->bodyBiomass);

	/*Update biomass of consumed cohort*/

	preyPointer->numberOfIndividuals-=used_consumption;
	preyPointer->bodyBiomass-=consumed_biomass*(1-localeGonadBiomassProportion);
	preyPointer->gonadBiomass-=consumed_biomass*localeGonadBiomassProportion;
	}
}
/* Inherited functions*/

biomassType FishBiomassDynamics::getFoodBiomass(bool bottom, int depthIndex,
		int columnIndex){
		return preyPointer==NULL?0.0f:(preyPointer->bodyBiomass+preyPointer->gonadBiomass);
}

biomassType FishBiomassDynamics::getFoodBiomassDifferential(bool bottom, int depthIndex, int columnIndex){
	/* The biomass differential is defined as the difference between consecutive consumed biomass*/
	return used_consumption - currentCohort->previousConsumption;
}

void FishBiomassDynamics::updateCohortBiomass(AnimalCohort& cohort){
	/* Get pointer to consumed cohort*/

		preyPointer = preyPointers[cohort.x][cohort.y];
		currentCohort=&cohort;
		if(preyPointer!=NULL){
				;
			}
		AnimalBiomassDynamics::updateCohortBiomass(cohort);

}


/* Migrate juvenile cohorts assuming a stochastic approach. Cohorts might move to a suboptimal cell to escape from local maxima*/
void FishBiomassDynamics::migrateCohortUsingRandomWalk(AnimalCohort& cohort){
	generateMigrationIndexes();
	double searchStepCounter=1;
	int localeVerticalCoordinate=cohort.x, localeHorizontalCoordinate=cohort.y;
	cohort.previousFitnessValue =cohort.currentFitnessValue= 0.0f;
	for (std::vector<pair<int,int>>::iterator migrationIndexPair = hourlyMigrationIndexPairs->begin();
			migrationIndexPair != hourlyMigrationIndexPairs->end()&&searchStepCounter<=this->max_search_steps; ++migrationIndexPair) {
		/* Retrieve migration indexes*/
		int verticalIndex=migrationIndexPair->first, horizontalIndex=migrationIndexPair->second;
		/* Calculate the destination coordinates as the current coordinates plus the migration indexes*/
		int destinationVertical = localeVerticalCoordinate+verticalIndex, destinationHorizontal = localeHorizontalCoordinate+horizontalIndex;
		if(destinationHorizontal>=0&&destinationHorizontal<=MAX_COLUMN_INDEX){
			if(destinationVertical>=0&&destinationVertical<=MAX_DEPTH_INDEX&&destinationVertical<=this->maximum_planktivore_depth){
				if(maxDepthIndex[destinationHorizontal]>=destinationVertical){
					/*If the cell exists, attempt movement*/
					/*Otherwise, move it with a certain probability proportional to the fitness difference*/
					double randomNumber = ((double) rand() / (RAND_MAX));
					if(randomNumber<=0.5f){
						cohort.x=destinationVertical;
						cohort.y=destinationHorizontal;
						cohort.currentPredatorSafety=0.0f;
						AnimalCohort* destinationCohort = this->preyPointers[destinationVertical][destinationHorizontal];
						biomassType destinationFoodBodyBiomass=destinationCohort->bodyBiomass,
								destinationFoodGonadBiomass=destinationCohort->gonadBiomass;
						biomassType destinationFoodTotalBiomass=destinationFoodBodyBiomass+destinationFoodGonadBiomass;
						cohort.currentFoodBiomass=destinationFoodTotalBiomass;
						cohort.currentFitnessValue=0.0f;
					}
				}

				/* Update the number of search steps per group*/
				searchStepCounter++;
			}
		}
	}
	//(*it)*=0.9f;
	/*Migrate traced cohort as a special case*/
}
}


