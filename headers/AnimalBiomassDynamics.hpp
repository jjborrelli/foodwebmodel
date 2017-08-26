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
#include <map>
#include <random>
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
#ifdef INDIVIDUAL_BASED_ANIMALS
	std::ostringstream animalTraceBuffer;
#endif
	/* Animal count summing. The simulation halts below a given number*/
	animalCountType floating_animal_count_summing;
#ifdef INDIVIDUAL_BASED_ANIMALS
	map<pair<int,int>,AnimalCohort> *floatingAnimals, *bottomAnimals;

	/* Attributes for internal usage for cohort modeling*/
	double reproduction_proportion_investment;
	struct removeOldEggs : public std::unary_function<const EggCohort&, bool>
	{
	    bool operator()(const EggCohort& cohort) const
	    {
	        return cohort.hasHatched;
	    }
	};

	struct removeStarvedOrMaturedJuveniles : public std::unary_function<const AnimalCohort&, bool>
	{
	    bool operator()(const AnimalCohort& cohort) const
	    {
	        return cohort.numberOfIndividuals<=0||cohort.bodyBiomass<=0.0f||cohort.stage==AnimalStage::Mature;
	    }
	};


#ifdef CREATE_NEW_COHORTS
	vector<EggCohort> floatingEggs, bottomEggs;
#ifdef MATURE_JUVENILES
	vector<AnimalCohort> floatingJuveniles, bottomJuveniles;
#endif
#endif
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
	cohortIDType *cohortID;
	physicalType *salinity_effect_matrix[MAX_DEPTH_INDEX];

	/* Zooplankton parameter weights*/
	biomassType animal_base_mortality_proportion;

	/*Parameters of daphnia migration*/
	int maximum_distance_daphnia_swum_in_rows_per_hour, vertical_migration_buffer_size;
	biomassType filtering_rate_per_daphnia, filtering_rate_per_individual_in_cell_volume;
	biomassType filtering_length_coefficient, filtering_length_exponent,
	filtering_coefficient, filtering_exponent;
	biomassType basal_respiration_weight, k_value_respiration;

	/* Animal constants*/
	physicalType food_conversion_factor;

	/* Threshold below which the concentration is considered starvation (in ug/l)*/
	biomassType food_starvation_threshold, egg_allocation_threshold, starvation_factor, dead_animals_per_lost_biomass_and_concentration;

	/* Maximum number of hours that the animal can survive with food below starvation levels*/
	unsigned int max_hours_without_food;

	/* Parameters for reproduction*/
	unsigned int maximum_age_in_hours, incubation_hours, ovipositing_period, maturation_hours;


	/*Parameters for animal carrying capacity*/
	biomassType animal_carrying_capacity_coefficient, animal_carrying_capacity_intercept, animal_carrying_capacity_amplitude, animal_carrying_capacity_constant, animal_carrying_capacity, maximum_found_animal_biomass;

	/*Parameters for physical pointers*/
	physicalType *temperature[MAX_DEPTH_INDEX], *lakeLightAtDepth[MAX_DEPTH_INDEX], *previousLakeLightAtDepth[MAX_DEPTH_INDEX];

	/* Animal attributes*/
	biomassType used_consumption, consumption_per_individual, locale_consumption, consumed_biomass, locale_defecation, base_animal_respiration, salinity_corrected_animal_respiration, basal_animal_respiration, active_respiration_exponent, active_respiration_factor, active_respiration, metabolic_respiration, animal_excretion_loss, animal_base_mortality, animal_temperature_mortality, animal_temp_independent_mortality, salinity_mortality, locale_consumption_salt_adjusted, animal_mortality, animal_predatory_pressure, low_oxigen_animal_mortality;

	/* Animal physical attributes*/
	physicalType stroganov_adjustment;

	/* Animal migration attributes*/
	biomassType floatingAnimalBiomassCenterDifferencePerDepth[HOURS_PER_DAY];
	biomassType verticalMigrationAnimalBiomassBuffer[MAX_DEPTH_INDEX][MAX_COLUMN_INDEX];
	/* Grazer individual count. Transformed to biomass using the rule: (count*grazer weight in micrograms)*/
	biomassType foodPreferenceScore[MAX_DEPTH_INDEX][MAX_COLUMN_INDEX];

	/* Gonad biomass allocation */
	double reproduction_proportion_investment_amplitude, reproduction_proportion_investment_coefficient, reproduction_proportion_investment_intercept, reproduction_proportion_investment_constant, reproduction_proportion_investment_multiplier, dead_animal_proportion;

	/* Gonad biomass metrics */
	biomassType reproduction_investment_subtraction, reproduction_investment_exponent, reproduction_investment_power;
#ifdef ADD_DEAD_BIOMASS_NUTRIENTS
	/* Dead biomass */
	biomassType *deadFloatingBiomass[MAX_DEPTH_INDEX], *deadBottomBiomass;
	physicalType reabsorbed_animal_nutrients_proportion;
#endif
	std::default_random_engine* randomGenerator;
	unsigned int random_seed;
#ifdef CHECK_ASSERTIONS
		std::ostringstream *assertionViolationBuffer;
#endif

	void updateAnimalBiomass();
#ifdef INDIVIDUAL_BASED_ANIMALS
	void updateCohortBiomass(AnimalCohort& cohort);
//	void removeDeadCohorts(vector<AnimalCohort> *animalCohorts);
//	void removeDeadAnimals();
	void updateCohortBiomassForAnimals(std::map<pair<int,int>,AnimalCohort> *animals);
#endif
#if defined(INDIVIDUAL_BASED_ANIMALS)&&defined(CREATE_NEW_COHORTS)
	biomassType animalBiomassDifferential(int depthIndex, int columnIndex, bool bottom, animalCountType animalCount, biomassType animalBiomass, AnimalStage stage);
#else
	biomassType animalBiomassDifferential(int depthIndex, int columnIndex, bool bottom, animalCountType animalCount, biomassType animalBiomass);
#endif
	void foodConsumptionRate(int depthIndex, int columnIndex, bool bottomFeeder, animalCountType animalCount, biomassType algaeBiomassInMicrograms, biomassType individualWeight);
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
#ifdef ANIMAL_STARVATION_HOURS_WITHOUT_FOOD
	void animalStarvationMortality(AnimalCohort& cohort, biomassType foodBiomass);
#elif defined(ANIMAL_STARVATION_PROPORTION_LOST_BIOMASS)
	void animalStarvationMortality(AnimalCohort& cohort);
#endif
#ifdef ANIMAL_AGING
	void animalAging(AnimalCohort& cohort);
#endif
#ifdef CREATE_NEW_COHORTS
	void calculateReproductionProportionInvestment(biomassType foodBiomass);
	biomassType createNewCohort(AnimalCohort& parentCohort, biomassType eggBiomass);
#ifdef MATURE_JUVENILES
	void matureEggs(vector<EggCohort>& eggs, vector<AnimalCohort>& adultAnimals);
	void matureJuveniles(vector<AnimalCohort>& juveniles, map<pair<int,int>,AnimalCohort> *adultAnimals);
#else
	void matureEggs(vector<EggCohort>& eggs, map<pair<int,int>,AnimalCohort> *adultAnimals);
#endif
#endif
	biomassType getFoodBiomass(AnimalCohort& cohort);
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
