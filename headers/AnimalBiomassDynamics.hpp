/*
 * AnimalBiomassDynamics.hpp
 *
 *  Created on: Jul 6, 2017
 *      Author: manu_
 */

#ifndef ANIMALBIOMASSDYNAMICS_HPP_
#define ANIMALBIOMASSDYNAMICS_HPP_
#include <sstream>
#include <math.h>
#include <vector>
#include "TypeDefinitions.hpp"
#include "ModelConstants.hpp"
#include "SimulationModes.hpp"

using namespace std;

namespace FoodWebModel {

class AnimalBiomassDynamics {
	friend class FoodWebModel;
public:
	AnimalBiomassDynamics();
	virtual ~AnimalBiomassDynamics();
protected:
	std::ostringstream lineBuffer, animalBiomassBuffer;
	/* Animal count summing. The simulation halts below a given number*/
	animalCountType floating_animal_count_summing;
#ifdef INDIVIDUAL_BASED_ANIMALS
	vector<AnimalCohort> *floatingAnimals, *bottomAnimals;
#else
	/* Animal biomass in micrograms*/
	biomassType *floatingAnimalBiomass[MAX_DEPTH_INDEX], *bottomAnimalBiomass;
	/* Animal individual count. Transformed to biomass using the rule: (count*grazer weight in micrograms)*/
	animalCountType *floatingAnimalCount[MAX_DEPTH_INDEX], *bottomAnimalCount;
#endif

	biomassType initial_grazer_weight[MAX_CLASS_INDEX];
	/* Food biomass and differential in micrograms*/
	biomassType *floatingFoodBiomass[MAX_DEPTH_INDEX], *bottomFoodBiomass;
	biomassType *floatingFoodBiomassDifferential[MAX_DEPTH_INDEX], *bottomFoodBiomassDifferential;

	/* Pointers connecting to the physical model*/
	unsigned int *maxDepthIndex, *current_hour;
	physicalType *salinity_effect_matrix[MAX_DEPTH_INDEX];

	/* Zooplankton parameter weights*/
	biomassType animal_base_mortality_proportion;

	/*Parameters of daphnia migration*/
	int maximum_distance_daphnia_swum_in_rows_per_hour, vertical_migration_buffer_size;
	biomassType filtering_rate_per_daphnia, filtering_rate_per_daphnia_in_cell_volume;
	biomassType basal_respiration_weight, k_value_respiration;

	/* Animal constants*/
	physicalType food_conversion_factor;
	biomassType food_starvation_threshold;
	unsigned int max_hours_without_food;

	/*Parameters for grazer carrying capacity*/
	biomassType animal_carrying_capacity_coefficient, animal_carrying_capacity_intercept, animal_carrying_capacity, maximum_found_animal_biomass;

	/*Parameters for physical pointers*/
	physicalType *temperature[MAX_DEPTH_INDEX], *lakeLightAtDepth[MAX_DEPTH_INDEX], *previousLakeLightAtDepth[MAX_DEPTH_INDEX];

	/* Animal attributes*/
	biomassType used_consumption, consumption_per_individual, locale_consumption, locale_defecation, base_animal_respiration, salinity_corrected_animal_respiration, basal_animal_respiration, active_respiration_exponent, active_respiration_factor, active_respiration, metabolic_respiration, animal_excretion_loss, animal_base_mortality, animal_temperature_mortality, animal_temp_independent_mortality, salinity_mortality, locale_consumption_salt_adjusted, animal_mortality, animal_predatory_pressure, low_oxigen_animal_mortality;

	/* Animal physical attributes*/
	physicalType stroganov_adjustment;

	/* Animal migration attributes*/
	biomassType floatingAnimalBiomassCenterDifferencePerDepth[HOURS_PER_DAY];
	biomassType verticalMigrationAnimalBiomassBuffer[MAX_DEPTH_INDEX][MAX_COLUMN_INDEX];
	/* Grazer individual count. Transformed to biomass using the rule: (count*grazer weight in micrograms)*/
	biomassType foodPreferenceScore[MAX_DEPTH_INDEX][MAX_COLUMN_INDEX];



#ifdef CHECK_ASSERTIONS
		std::ostringstream *assertionViolationBuffer;
#endif

	void updateAnimalBiomass();
#ifdef INDIVIDUAL_BASED_ANIMALS
	void updateCohortBiomass(AnimalCohort& animal);
#endif
	biomassType animalBiomassDifferential(int depthIndex, int columnIndex, bool bottom, animalCountType animalCount, biomassType animalBiomass);
	void foodConsumptionRate(int depthIndex, int columnIndex, bool bottomFeeder, animalCountType animalCount, biomassType algaeBiomassInMicrograms);
	void defecation();
	void animalRespiration(biomassType zooBiomass, physicalType localeTemperature, physicalType localeSalinityEffect);
	biomassType basalRespiration(biomassType zooBiomass);
	biomassType activeRespiration(biomassType zooBiomass, physicalType localeTemperature);
	biomassType metabolicFoodConsumption();
	void animalExcretion(biomassType localeRespiration);
	void animalMortality(biomassType localeRespiration, physicalType localeTemperature, physicalType localeSalinityConcentration);
	void animalBaseMortality(physicalType localeTemperature, biomassType localeBiomass);
	void animalTemperatureMortality(physicalType localeTemperature, biomassType localeBiomass);
#ifdef INDIVIDUAL_BASED_ANIMALS
	void animalStarvationMortality(AnimalCohort& animal, biomassType foodBiomass);
	biomassType getFoodBiomass(AnimalCohort& animal);
#endif
	void salinityMortality(biomassType localeBiomass);
	void calculateLowOxigenMortality(biomassType inputBiomass);
	void stroganovApproximation(physicalType localeTemperature);
	void calculatePredationPressure(animalCountType zooplanktonLocaleCount);
	void calculateGrazerCarryingCapacityMortality(biomassType inputBiomass);

	/*Vertical animal migration*/
//	void verticalMigrateAnimalsNoPreference();
//	void verticalMigrateAnimalsPreference();
	void calculateLocalPreferenceScore();
	void reportAssertionError(int depthIndex, int columnIndex, biomassType biomass, biomassType previousBiomass, biomassType differential, bool isBottom);
	biomassType getFoodBiomass(bool bottom, int columnIndex, int depthIndex);
};

} /* namespace FoodWebModel */

#endif /* ANIMALBIOMASSDYNAMICS_HPP_ */
