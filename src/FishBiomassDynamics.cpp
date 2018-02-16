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


void FishBiomassDynamics::foodConsumptionRate(int depthIndex, int columnIndex, bool bottom, animalCountType animalCount, biomassType foodBiomassInMicrograms, biomassType individualWeight, double consumedProportion)
{
}
FishBiomassDynamics::~FishBiomassDynamics() {
	// TODO Auto-generated destructor stub
}

void FishBiomassDynamics::calculateBiomass(){
	for(int columnIndex=0; columnIndex<MAX_COLUMN_INDEX; columnIndex++){
		bottomFoodBiomass[columnIndex]=0.0f;
		for(int depthIndex=0; depthIndex<=this->maxDepthIndex[columnIndex]; depthIndex++){
			floatingFoodBiomass[depthIndex][columnIndex]=0.0f;
		}
	}
	biomassType totalConsumedBiomass=0.0f;
	for (std::vector<AnimalCohort>::iterator it=this->floatingAnimals->begin(); it!=this->floatingAnimals->end(); ++it){
		floatingFoodBiomass[it->x][it->y]+=it->bodyBiomass;
	}
	for (std::vector<AnimalCohort>::iterator it=this->bottomAnimals->begin(); it!=this->bottomAnimals->end(); ++it){
		bottomFoodBiomass[it->y]+=it->bodyBiomass;
	}
}

void FishBiomassDynamics::migrateCohortUsingRandomWalk(AnimalCohort& cohort){

}

void FishBiomassDynamics::predateCohort(AnimalCohort& cohort){}



/* If the migration index is greater than 0, migrate adult and juvenile cohorts*/

void FishBiomassDynamics::calculateMigrationValues(){
	generateMigrationIndexes();
}
}


