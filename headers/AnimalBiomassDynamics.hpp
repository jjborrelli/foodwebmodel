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
	friend class FishBiomassDynamics;
public:
	AnimalBiomassDynamics();
	virtual ~AnimalBiomassDynamics();
	/* Initialize all the animal structures before simulation*/
	void initializeSimulationStructures();
	void takeAnimalDynamicsStep();
protected:
	std::ostringstream lineBuffer, animalBiomassBuffer;
#ifdef INDIVIDUAL_BASED_ANIMALS
	std::ostringstream animalTraceBuffer;
	AnimalType animalType;
#endif
	/* Animal count summing. The simulation halts below a given number*/
	animalCountType floating_animal_count_summing, animals_dead_by_starvation, natural_dead_individuals;
	/* Migration of animal biomass per hour*/
		int zooplanktonBiomassCenterDifferencePerDepth[HOURS_PER_DAY];
#ifdef INDIVIDUAL_BASED_ANIMALS
#ifdef ANIMAL_COHORT_MAP
	map<pair<int,int>,AnimalCohort> *floatingAnimals, *bottomAnimals;
#else
	vector<AnimalCohort> *floatingAnimals, *bottomAnimals;
#ifdef CONGLOMERATE_ALL_COHORTS
	map<pair<int,int>, AnimalCohort> animalMigrationBuffer;
#else
	map<pair<int,int>, AnimalCohort> floatingReallocatedCohorts, bottomReallocatedCohorts;
#endif
#endif
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
	        bool removeCohort= cohort.numberOfIndividuals<=0||cohort.bodyBiomass<=0.0f||cohort.stage==AnimalStage::Mature;
//	        if(removeCohort){
//	        	cout<<"Removed cohort"<<endl;
//	        }
	        return removeCohort;
	    }
	};
	struct removeOldIndividuals : public std::unary_function<const AnimalCohort&, bool>
		{
		    bool operator()(const AnimalCohort& cohort) const
		    {
		        bool removeCohort= cohort.numberOfIndividuals<=0||cohort.bodyBiomass<=0.0f;
//		        if(removeCohort){
//		        	cout<<"Removed cohort"<<endl;
//		        }
		        return removeCohort;
		    }
		};
	biomassType migratedFloatingAnimalBodyBiomass[MAX_DEPTH_INDEX][MAX_COLUMN_INDEX], migratedFloatingAnimalGonadBiomass[MAX_DEPTH_INDEX][MAX_COLUMN_INDEX];
	animalCountType migratedFloatingAnimalCount[MAX_DEPTH_INDEX][MAX_COLUMN_INDEX];
	biomassType migratedFloatingAnimalStarvationBiomass[MAX_DEPTH_INDEX][MAX_COLUMN_INDEX];
	biomassType migratedFloatingAnimalPreviousConsumption[MAX_DEPTH_INDEX][MAX_COLUMN_INDEX];
	int migratedFloatingMigrationConstant[MAX_DEPTH_INDEX][MAX_COLUMN_INDEX];
	int migratedFloatingAnimalCohortID[MAX_DEPTH_INDEX][MAX_COLUMN_INDEX];

#ifdef CREATE_NEW_COHORTS
	vector<EggCohort> floatingEggs;//, bottomEggs;
#ifdef MATURE_JUVENILES
	vector<AnimalCohort> floatingJuveniles;//, bottomJuveniles;
#endif
#endif
#else
	/* Animal biomass in micrograms*/
	biomassType *floatingAnimalBiomass[MAX_DEPTH_INDEX], *bottomAnimalBiomass;
	/* Animal individual count. Transformed to biomass using the rule: (count*grazer weight in micrograms)*/
	animalCountType *floatingAnimalCount[MAX_DEPTH_INDEX], *bottomAnimalCount;
#endif

	biomassType initial_animal_weight[MAX_CLASS_INDEX];
	animalCountType initial_animal_count[MAX_DEPTH_INDEX][MAX_COLUMN_INDEX];
	/* Safety of cells according to predation risk*/
	biomassType predation_index;
	biomassType predatorSafety[MAX_DEPTH_INDEX][MAX_COLUMN_INDEX];

	/* Pointers connecting to the physical model*/
	unsigned int *maxDepthIndex, *current_hour;
	/* Number of cells in the system*/
	unsigned int cell_counter;
	cohortIDType *cohortID;
	physicalType *salinity_effect_matrix[MAX_DEPTH_INDEX];



	/* Best depth index per column*/

	int optimalDepthIndexes[MAX_COLUMN_INDEX];

	/* Fitness value per cell for each depth and column*/
	biomassType localeFitnessValue[MAX_DEPTH_INDEX][MAX_COLUMN_INDEX];
	biomassType normalizedFloatingFoodBiomass[MAX_DEPTH_INDEX][MAX_COLUMN_INDEX];

	/* Zooplankton parameter weights*/
	biomassType animal_base_mortality_proportion;

	/*Parameters of daphnia migration*/
	int maximum_distance_daphnia_swum_in_rows_per_hour, vertical_migration_buffer_size;
	biomassType filtering_rate_per_daphnia, filtering_rate_per_individual_in_cell_volume;
	biomassType filtering_length_coefficient, filtering_length_exponent,
	filtering_coefficient, filtering_exponent;
	biomassType basal_respiration_weight, k_value_respiration, consumption_temperature_factor;

	/* Animal constants*/
	physicalType food_conversion_factor;

	/* Threshold below which the concentration is considered starvation (in ug/l)*/
	biomassType food_starvation_threshold, egg_allocation_threshold, starvation_factor, dead_animals_per_lost_biomass_and_concentration;

	unsigned int layer_center_index;

	/* Maximum number of hours that the animal can survive with food below starvation levels*/
	unsigned int max_hours_without_food;

	/* Parameters for reproduction*/
	unsigned int maximum_age_in_hours, incubation_hours, ovipositing_period, maturation_hours;
	/* Threshold over which small cohorts agglomerate into the big cohort that is at the same coordinates*/
	unsigned int agglomeration_cohort_threshold;


	/*Parameters for animal carrying capacity*/
	biomassType animal_carrying_capacity_coefficient, animal_carrying_capacity_intercept, animal_carrying_capacity_amplitude, animal_carrying_capacity_constant, animal_carrying_capacity, maximum_found_animal_biomass;

	/*Parameters for physical pointers*/
	physicalType *temperature[MAX_DEPTH_INDEX], *lakeLightAtDepth[MAX_DEPTH_INDEX], *previousLakeLightAtDepth[MAX_DEPTH_INDEX], *indexToDepth;

	/* Animal attributes*/
	biomassType used_consumption, consumption_per_individual, locale_consumption, consumed_biomass, locale_defecation, base_animal_respiration, salinity_corrected_animal_respiration, basal_animal_respiration, active_respiration_exponent, active_respiration_factor, active_respiration, metabolic_respiration, animal_excretion_loss, animal_base_mortality, animal_temperature_mortality, animal_temp_independent_mortality, salinity_mortality, locale_consumption_salt_adjusted, animal_mortality, animal_predatory_pressure, low_oxigen_animal_mortality;
	double migration_consumption;

	/* Animal physical attributes*/
	physicalType stroganov_adjustment, ZMax;

	/* Grazer individual count. Transformed to biomass using the rule: (count*grazer weight in micrograms)*/
	biomassType foodPreferenceScore[MAX_DEPTH_INDEX][MAX_COLUMN_INDEX];

	/* Gonad biomass allocation */
	double reproduction_proportion_investment_amplitude, reproduction_proportion_investment_coefficient, reproduction_proportion_investment_intercept, reproduction_proportion_investment_constant, reproduction_proportion_investment_multiplier, dead_animal_proportion, maximum_gonad_weight_allocation;


	/* Gonad biomass metrics */
	biomassType reproduction_investment_subtraction, reproduction_investment_exponent, reproduction_investment_power;

	biomassType locale_algae_biomass_before_eating, locale_algae_biomass_after_eating;


	/* Food biomass and differential in micrograms*/
	biomassType *floatingFoodBiomass[MAX_DEPTH_INDEX], *bottomFoodBiomass;
	biomassType *floatingFoodBiomassDifferential[MAX_DEPTH_INDEX], *bottomFoodBiomassDifferential;
#ifdef ADD_DEAD_BIOMASS_NUTRIENTS
	/* Dead biomass */
	biomassType *deadFloatingBiomass[MAX_DEPTH_INDEX], *deadBottomBiomass;
#endif


	/*Summing of optimal values for normalization*/
	physicalType averagePredatorSafety, averageFood;

	/*Migration score weight*/
	physicalType light_migration_weight;

	/* Generator for random numbers*/
	std::default_random_engine* animalRandomGenerator;

	/* Predated indiviuals in the current step. An array is used instead of an attribute in the Cohort structure because we are interested in the individuals predated in the current step*/
	animalCountType predatedIndividuals[MAX_DEPTH_INDEX][MAX_COLUMN_INDEX];

	/* Starvation parameters affecting migration*/
	double ind_food_starvation_threshold;
	int starvation_max_hours;

	/* Maximum number of animals allowed per cohort*/
	animalCountType cohort_splitting_limit;

	/* Migration based on random walk*/
	int max_vertical_migration, max_horizontal_migration;
#ifdef DAPHNIA_CIRCADIAN_CYCLE
	vector<pair<int,int>> daytimeMigrationIndexPairs, nighttimeMigrationIndexPairs;
#else
	vector<pair<int,int>> migrationIndexPairs;
#endif
	vector<pair<int,int>>* hourlyMigrationIndexPairs;
	bool dayTime;

	/*Input parameters for migration*/
	unsigned int max_search_steps;
	double random_walk_probability_weight;
	biomassType minimum_predation_safety;

	/*Parameters relative to light*/
#ifndef THRESHOLD_LIGHT_SAFETY
	biomassType lightSafety[MAX_DEPTH_INDEX][MAX_COLUMN_INDEX], averageLightSafety;
#endif

	physicalType maximum_light_tolerated, light_safety_weight, light_safety_threshold;

	/* Minimum tolerable light for Daphnia*/
	physicalType minimum_tolerable_light;

	/*Special traced adult cohort and flag to set that it has been created*/
	AnimalCohort tracedCohort;
	int tracedCohortID;


	/*Planktivore biomass at each depth and column */
//	biomassType predatorBiomass[MAX_DEPTH_INDEX][MAX_COLUMN_INDEX];

#ifdef CHECK_ASSERTIONS
		std::ostringstream *assertionViolationBuffer;
#endif
		/* virtual methods*/
	virtual biomassType getFoodBiomass(bool bottom, int depthIndex, int columnIndex);
	virtual biomassType getFoodBiomass(const AnimalCohort& cohort);
	virtual void calculateMigrationValues()=0;
	virtual void migrateCohortUsingRandomWalk(AnimalCohort& cohort)=0;
private:
	void updateAnimalBiomass();
#ifdef INDIVIDUAL_BASED_ANIMALS
	virtual void updateCohortBiomass(AnimalCohort& cohort);
//	void removeDeadCohorts(vector<AnimalCohort> *animalCohorts);
//	void removeDeadAnimals();
#ifdef ANIMAL_COHORT_MAP
	void updateCohortBiomassForAnimals(std::map<pair<int,int>,AnimalCohort> *animals);
#else
	void updateCohortBiomassForAnimals(std::vector<AnimalCohort> *animals);
#endif
	void registerMigration();
#endif
#if defined(INDIVIDUAL_BASED_ANIMALS)&&defined(CREATE_NEW_COHORTS)
	biomassType animalBiomassDifferential(int depthIndex, int columnIndex, bool bottom, animalCountType animalCount, biomassType animalBiomass, AnimalStage stage);
#else
	biomassType animalBiomassDifferential(int depthIndex, int columnIndex, bool bottom, animalCountType animalCount, biomassType animalBiomass);
#endif

	/* Methods for updating the biomass in animals*/
	virtual void foodConsumptionRate(int depthIndex, int columnIndex, bool bottomFeeder, animalCountType animalCount, biomassType algaeBiomassInMicrograms, biomassType individualWeight, double consumedProportion=1.0f)=0;
	virtual biomassType getFoodBiomassDifferential(bool bottom, int depthIndex, int columnIndex);
	void defecation();
	void animalRespiration(biomassType zooBiomass, physicalType localeTemperature, physicalType localeSalinityEffect);
	biomassType basalRespiration(biomassType zooBiomass);
	biomassType activeRespiration(biomassType zooBiomass, physicalType localeTemperature);
	biomassType metabolicFoodConsumption();
	void animalExcretion(biomassType localeRespiration);
	void animalMortality(biomassType localeRespiration, physicalType localeTemperature, physicalType localeSalinityConcentration);
	void animalBaseMortality(physicalType localeTemperature, biomassType localeBiomass);
	void animalTemperatureMortality(physicalType localeTemperature, biomassType localeBiomass);

	virtual void predateCohort(AnimalCohort& cohort)=0;

#ifdef INDIVIDUAL_BASED_ANIMALS

	/* Migration methods */

	void migrateAnimalCohorts();
	void migrateAdultCohorts(std::map<pair<int,int>,AnimalCohort> *animals, int migrateStep);
	void updateMigratedCohorts(std::map<pair<int,int>,AnimalCohort> *animals);

	void clearMigrationParameters();

	/* Different methods for adult migration (map-based)*/
	void migrateAdultsCohortsStructurally(std::map<pair<int,int>,AnimalCohort> *animals, int migrationStep);
	void migrateAdultCohortsDepthDependent(std::map<pair<int,int>,AnimalCohort> *animals);
	int migrateCohortsDepthDependent(AnimalCohort& cohort);
	int migrateCohortsFixedFrequency(AnimalCohort& cohort);
	int migrateCohortsOptimizedDepth(AnimalCohort& cohort);
	void migrateAdultCohort(AnimalCohort& cohort);
	void migrateExistingAdultCohort(AnimalCohort& cohort, int depthIndex, int columnIndex);
	void migrateCohortsUsingRandomWalk();
	void migrateCohortsUsingRandomWalk(vector<AnimalCohort>& cohorts);

	/* Different methods for juvenile migration (vector-based)*/
	void migrateJuvenileCohortsStructurally(vector<AnimalCohort>& juveniles, int migrateStep);
	void migrateJuvenileCohortStructurally(AnimalCohort& cohort, int migrateStep);
	void migrateJuvenileCohortsDepthDependent(vector<AnimalCohort>& animals);
	void migrateJuvenileCohortDepthDependent(AnimalCohort& cohort);
	void migrateJuvenileCohortDepthDependent(std::vector<AnimalCohort>::iterator it);


	/*If animals consume during migration, use these methods*/
	void consumeDuringMigration(int initialDepth, int finalDepth, AnimalCohort& it);
	void consumeDuringMigration(int initialDepth, int finalDepth, std::vector<AnimalCohort>::iterator it);






	/*Maturation and reproduction methods*/
#ifdef ANIMAL_STARVATION_HOURS_WITHOUT_FOOD
	void animalStarvationMortality(AnimalCohort& cohort, biomassType foodBiomass);
#elif defined(ANIMAL_STARVATION_PROPORTION_LOST_BIOMASS)
	void animalStarvationMortality(AnimalCohort& cohort);
#endif
#ifdef ANIMAL_AGING
	void animalAging(AnimalCohort& cohort);
#endif
#ifdef CREATE_NEW_COHORTS
	void calculateReproductionProportionInvestment(biomassType foodBiomass, biomassType foodBiomassDifferential);
	biomassType createNewCohort(AnimalCohort& parentCohort, biomassType eggBiomass, vector<AnimalCohort>* juveniles);
#ifdef MATURE_JUVENILES
	void matureEggs(vector<EggCohort>& eggs, vector<AnimalCohort>& adultAnimals);
#ifdef ANIMAL_COHORT_MAP
	virtual void matureJuveniles(vector<AnimalCohort>& juveniles, map<pair<int,int>,AnimalCohort> *adultAnimals);
#else
	virtual void matureJuveniles(vector<AnimalCohort>& juveniles, vector<AnimalCohort> *adultAnimals);
	virtual void matureFloatingEggs();
	void reallocateSmallCohorts();
#ifdef CONGLOMERATE_ALL_COHORTS
	void agglomerateCohorts(vector<AnimalCohort> *animals);
#else
	void reallocateSmallCohorts(vector<AnimalCohort> *animals, map<pair<int,int>,AnimalCohort>& reallocatedCohorts);
#endif
#endif
	void removeEmptyCohorts();
#else
	void matureEggs(set<EggCohort>& eggs, map<pair<int,int>,AnimalCohort> *adultAnimals);
#endif
#endif
#endif

	/* External factors affecting animal metabolism*/
	void salinityMortality(biomassType localeBiomass);
	void calculateLowOxigenMortality(biomassType inputBiomass);
	void stroganovApproximation(physicalType localeTemperature);
	void calculatePredationPressure(animalCountType zooplanktonLocaleCount);
virtual	void calculateGrazerCarryingCapacityMortality(const animalCountType animalCount, const biomassType animalWeight, const biomassType animalBiomass);

	/*Vertical animal migration*/
//	void verticalMigrateAnimalsNoPreference();
//	void verticalMigrateAnimalsPreference();
	void calculateLocalPreferenceScore();
	void reportAssertionError(int depthIndex, int columnIndex, biomassType biomass, biomassType previousBiomass, biomassType differential, bool isBottom);
protected:
	bool updateMigrationTable(AnimalCohort& cohort, int migrateStep);
	void generateMigrationIndexes();
	virtual void updateStarvation(biomassType biomassAfterEating, AnimalCohort& cohort);
};

} /* namespace FoodWebModel */

#endif /* ANIMALBIOMASSDYNAMICS_HPP_ */
