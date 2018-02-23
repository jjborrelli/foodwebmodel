/*
 * FishBiomassDynamics.h
 *
 *  Created on: 4 ene. 2018
 *      Author: manu
 */

#ifndef FISHBIOMASSDYNAMICS_H_
#define FISHBIOMASSDYNAMICS_H_

#include "AnimalBiomassDynamics.hpp"

namespace FoodWebModel {


class FishBiomassDynamics: public AnimalBiomassDynamics{
	friend class FoodWebModel;
public:
	FishBiomassDynamics();
	virtual ~FishBiomassDynamics();

protected:
	biomassType predationFactor;
	AnimalCohort *preyPointers[MAX_DEPTH_INDEX][MAX_COLUMN_INDEX], *preyPointer, *currentCohort;
	biomassType gonadBiomassProportion[MAX_DEPTH_INDEX][MAX_COLUMN_INDEX];
	vector<AnimalCohort> *preys;
	unsigned int maximum_planktivore_depth;
	biomassType planktivore_saturation_constant, planktivore_consumption_weight;
	void calculatePreyBiomass();
	virtual void foodConsumptionRate(int depthIndex, int columnIndex, bool bottom, animalCountType animalCount, biomassType foodBiomassInMicrograms, biomassType individualWeight, double consumedProportion);
	/* Inherited functions*/
	virtual void predateCohort(AnimalCohort& cohort);
	virtual void calculateMigrationValues();
	virtual void updateCohortBiomass(AnimalCohort& cohort);
	virtual biomassType getFoodBiomass(bool bottom, int depthIndex,
			int columnIndex);
	virtual biomassType getFoodBiomassDifferential(bool bottom, int depthIndex, int columnIndex);
	virtual void migrateCohortUsingRandomWalk(AnimalCohort& cohort);
	virtual void matureJuveniles(vector<AnimalCohort>& juveniles, vector<AnimalCohort> *adultAnimals);
	virtual void matureFloatingEggs();
	virtual void updateStarvation(biomassType biomassAfterEating, AnimalCohort& cohort);
private:
	virtual	void calculateGrazerCarryingCapacityMortality(const animalCountType animalCount, const biomassType animalWeight, const biomassType animalBiomass);

};
}

#endif /* FISHBIOMASSDYNAMICS_H_ */
