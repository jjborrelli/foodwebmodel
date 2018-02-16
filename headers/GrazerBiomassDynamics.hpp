/*
 * GrazerBiomassDynamics.hpp
 *
 *  Created on: 5 ene. 2018
 *      Author: manu
 */

#ifndef GRAZERBIOMASSDYNAMICS_HPP_
#define GRAZERBIOMASSDYNAMICS_HPP_
#include <random>
#include "AnimalBiomassDynamics.hpp"
#include "TypeDefinitions.hpp"
#include "ModelConstants.hpp"
#include "SimulationModes.hpp"

namespace FoodWebModel {

class GrazerBiomassDynamics: public AnimalBiomassDynamics{
	friend class FoodWebModel;
public:
	GrazerBiomassDynamics();
	virtual ~GrazerBiomassDynamics();
protected:
	physicalType reabsorbed_animal_nutrients_proportion, critical_depth, critical_light_intensity, velocity_downward_pull, light_optimal_value;



	/* Animal migration attributes*/
	biomassType floatingAnimalBiomassCenterDifferencePerDepth[HOURS_PER_DAY];
	biomassType verticalMigrationAnimalBiomassBuffer[MAX_DEPTH_INDEX][MAX_COLUMN_INDEX];




	/* Kairomone parameters*/
	biomassType kairomones_level_day, kairomones_level_night, kairomones_thermocline;

	/* Kairomone concentration at depth. This will be eventually in the instance modeling planktivore dynamics*/
	biomassType kairomoneConcentration[MAX_DEPTH_INDEX][MAX_COLUMN_INDEX];

	/* Distribution of planktivore biomass across depths*/
	biomassType planktivore_biomass_center_day, planktivore_biomass_center_night, planktivore_biomass_width;



	/*Functions related to migration (depredated)*/
	int migrateCohortsDepthDependent(AnimalCohort& cohort);
	int migrateCohortsFixedFrequency(AnimalCohort& cohort);
	void calculateKairomonesConcetration();
	void migrateJuvenileCohortDepthDependent(AnimalCohort& cohort);
	void migrateAdultCohortsDepthDependent(std::map<pair<int,int>,AnimalCohort>* animals);
	void migrateAdultCohort(AnimalCohort& cohort);




	virtual void predateCohort(AnimalCohort& cohort);
	virtual void foodConsumptionRate(int depthIndex, int columnIndex, bool bottom, animalCountType animalCount, biomassType foodBiomassInMicrograms, biomassType individualWeight, double consumedProportion);
	virtual void calculatePredatorBiomass();

private:
	physicalType calculatePredatorSafety(int initialDepth, int finalDepth);
	void findNormalizingFactors();
	void migrateCohortUsingRandomWalk(AnimalCohort& cohort);
	void calculateMigrationValues();

	/* Calculating the optimal migration index for animals*/
	void findOptimalDepthIndexes();
	void findOptimalDepthIndex(unsigned int column);



};

} /* namespace FoodWebModel */

#endif /* GRAZERBIOMASSDYNAMICS_HPP_ */
