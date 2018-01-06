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
public:
	FishBiomassDynamics();
	virtual ~FishBiomassDynamics();

protected:
	biomassType predationFactor;
	vector<AnimalCohort> *juvenileGrazers, *adultGrazers;
	void calculateBiomass();
	virtual void migrateCohortUsingRandomWalk(AnimalCohort& cohort);
	virtual void foodConsumptionRate(int depthIndex, int columnIndex, bool bottom, animalCountType animalCount, biomassType foodBiomassInMicrograms, biomassType individualWeight, double consumedProportion);
	/* Inherited functions*/
	virtual biomassType getFoodBiomassDifferential(bool bottom, int columnIndex,	int depthIndex){
		return bottom?this->bottomFoodBiomassDifferential[columnIndex]:this->floatingFoodBiomassDifferential[columnIndex][depthIndex];
	}
	virtual void predateCohort(AnimalCohort& cohort);
	virtual void calculatePredatorBiomass();


};
}

#endif /* FISHBIOMASSDYNAMICS_H_ */
