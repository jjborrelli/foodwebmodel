/*
 * FoodWebModel.cpp

 *
 *  Created on: 13 mar. 2017
 *      Author: manu_
 */

#include "../headers/FoodWebModel.hpp"
#include <string>
#include <iostream>
#include <random>

using namespace std;

string operator+(string arg1, int arg2){
	ostringstream  bufferString;
	bufferString<<arg2;
	return arg1+bufferString.str();
}

void FoodWebModel::FoodWebModel::copyPointersToAnimalDynamics() {
#ifdef CHECK_ASSERTIONS
	grazerDynamics.assertionViolationBuffer = &assertionViolationBuffer;
#endif
	/* Set grazer count and biomass pointers to the grazer dynamics objects*/
#ifdef INDIVIDUAL_BASED_ANIMALS
	grazerDynamics.floatingAnimals=&this->zooplankton;
	grazerDynamics.bottomAnimals=&this->bottomGrazers;

#else
	grazerDynamics.bottomAnimalBiomass = bottomFeederBiomass;
	grazerDynamics.bottomAnimalCount = bottomFeederCount;
	for (unsigned int depthIndex = 0; depthIndex < MAX_DEPTH_INDEX;
				depthIndex++) {
			grazerDynamics.floatingAnimalBiomass[depthIndex] =
					zooplanktonBiomass[depthIndex];
			grazerDynamics.floatingAnimalCount[depthIndex] =
					zooplanktonCount[depthIndex];
	}
#endif
	grazerDynamics.bottomFoodBiomass = periBiomass;
	grazerDynamics.bottomFoodBiomassDifferential = periBiomassDifferential;
	grazerDynamics.maxDepthIndex = maxDepthIndex;
	for(int i=0; i<HOURS_PER_DAY; i++){
		grazerDynamics.zooplanktonBiomassCenterDifferencePerDepth[i]=zooplanktonBiomassCenterDifferencePerDepth[i];

	}
#ifdef ADD_DEAD_BIOMASS_NUTRIENTS
	grazerDynamics.deadBottomBiomass=deadBottomBiomass;
#endif
	grazerDynamics.current_hour = &current_hour;
	grazerDynamics.indexToDepth=indexToDepth;
	for (unsigned int depthIndex = 0; depthIndex < MAX_DEPTH_INDEX;
			depthIndex++) {
		grazerDynamics.floatingFoodBiomass[depthIndex] =
				phytoBiomass[depthIndex];
		grazerDynamics.floatingFoodBiomassDifferential[depthIndex] = phytoBiomassDifferential[depthIndex];

		grazerDynamics.salinity_effect_matrix[depthIndex] =
				salinity_effect_matrix[depthIndex];
		grazerDynamics.lakeLightAtDepth[depthIndex]=lakeLightAtDepth[depthIndex];
		grazerDynamics.previousLakeLightAtDepth[depthIndex]=previousLakeLightAtDepth[depthIndex];
		grazerDynamics.temperature[depthIndex]=temperature[depthIndex];
#ifdef ADD_DEAD_BIOMASS_NUTRIENTS
		grazerDynamics.deadFloatingBiomass[depthIndex]=deadFloatingBiomass[depthIndex];
#endif
	}
//	predatorDynamics.assertionViolationBuffer = &assertionViolationBuffer;
//	predatorDynamics.bottomAnimalBiomass = bottomPredatorBiomass;
//	predatorDynamics.bottomAnimalCount = bottomPredatorCount;
//	predatorDynamics.bottomFoodBiomass = bottomFeederBiomass;
//	predatorDynamics.maxDepthIndex = maxDepthIndex;
//	predatorDynamics.current_hour = &current_hour;
//	for (unsigned int depthIndex = 0; depthIndex < MAX_DEPTH_INDEX;
//			depthIndex++) {
//		predatorDynamics.floatingAnimalBiomass[depthIndex] =
//				floatingPredatorBiomass[depthIndex];
//		predatorDynamics.floatingAnimalCount[depthIndex] =
//				floatingPredatorCount[depthIndex];
//		predatorDynamics.floatingFoodBiomass[depthIndex] =
//				zooplanktonBiomass[depthIndex];
//		predatorDynamics.salinity_effect_matrix[depthIndex] =
//				salinity_effect_matrix[depthIndex];
//		predatorDynamics.lakeLightAtDepth[depthIndex]=lakeLightAtDepth[depthIndex];
//		predatorDynamics.previousLakeLightAtDepth[depthIndex]=previousLakeLightAtDepth[depthIndex];
//		predatorDynamics.temperature[depthIndex]=temperature[depthIndex];
//	}
}

int FoodWebModel::FoodWebModel::simulate(const SimulationArguments& simArguments){
	/*Read numeric parameters*/
	printSimulationMode();

	/*CSV file to write the output. Useful for calibration*/
	openSimulationFiles(simArguments);

	/*Write file headers*/
#ifdef EXTENDED_OUTPUT
	outputInitialAlgaeFile<<"Depth, Column, Time, AlgaeType, DepthInMeters, LightAllowance, AlgaeTurbidity, LightAtDepth, LimitationProduct, LightDifference, NormalizedLightDifference, ResourceLimitationExponent, AlgaeBiomassToDepth, PhotoSynthesys, AlgaeRespiration, AlgaeExcretion, AlgaeNaturalMortality, AlgaeSedimentation, AlgaeWeightedSedimentation, AlgaeSlough, AlgaeTempMortality, AlgaeResourceLimStress, AlgaeWeightedResourceLimStress, AlgaeNaturalMortalityFactor, AlgalNaturalMortalityRate, AlgaeVerticalMigration"<<endl;
	outputAlgaeFile<<"Depth, Column, Time, AlgaeType, DepthInMeters, AlgaeWashup, AlgaeBiomassDifferential, AlgaeBiomass, LimitingFactor"<<endl;
	"Depth, Column, Time, AlgaeType, GrazerGrazingPerIndividual, GrazerGrazingPerIndividualPerAlgaeBiomass, GrazerUsedGrazingPerAlgaeBiomass, GrazerStroganovTemperatureAdjustment, Grazing, GrazingSaltAdjusted, GrazerDefecation, GrazerBasalRespiration, GrazerActiveRespiratonExponent, GrazerActiveRespirationFactor, GrazerActiveRespiration, GrazerMetabolicRespiration, GrazerNonCorrectedRespiration, GrazerCorrectedRespiration, GrazerExcretion, GrazerTempMortality, GrazerNonTempMortality, GrazerBaseMortality, GrazerSalinityMortality, LowOxigenGrazerMortality, GrazerMortality, PredatoryPressure, GrazerCarryingCapacity, AlgaeBiomassBeforeGrazing, AlgaeBiomassAfterGrazing, ReproductionInvestmentSubstraction, ReproductionInvestmentExponent, ReproductionInvestmentPower, ReproductionInvestment, GrazerBiomassDifferential, GrazerCount, GrazerBiomass";
	outputPredatorFile<<"Depth, Column, Time, AlgaeType, PredationPerIndividual, PredationPerIndividualPerZooplanktonBiomass, UsedPredationPerZooplanktonBiomass, PredatorStroganovTemperatureAdjustment, Predation, PredationSaltAdjusted, PredatorDefecation, PredatorBasalRespiration, PredatorActiveRespiratonExponent, PredatorActiveRespirationFactor, PredatorActiveRespiration, PredatorMetabolicRespiration, PredatorNonCorrectedRespiration, PredatorCorrectedRespiration, PredatorExcretion, PredatorTempMortality, PredatorNonTempMortality, PredatorBaseMortality, PredatorSalinityMortality, LowOxigenPredatorMortality, GrazerMortality, PredatoryPressure, GrazerCarryingCapacity, GrazerBiomassBeforePredation, GrazerBiomassAfterPredation, PredatorBiomassDifferential, PredatorCount, PredatorBiomass"<<endl;
#else
	outputAlgaeFile<<"Depth, Column, Time, AlgaeType, DepthInMeters, VerticalLostBiomass, VerticalGainedBiomass, AlgaeBiomassDifferential, AlgaeBiomass, LimitingFactor, LightAllowance, LightAtDepth, NutrientLimitation, LimitationProduct, Temperature, TemperatureLimitation, PhotoSynthesis, Respiration, Excretion"<<endl;
	outputGrazerFile<<"Depth, Column, Time, AlgaeType, GrazerBiomassDifferential, GrazerCount, AnimalsDeadByStarvation, NaturalDeadAnimals, GrazerBiomass";
	outputPredatorFile<<"Depth, Column, Time, AlgaeType, PredatorBiomassDifferential, PredatorBiomass, PredatorCount"<<endl;
#endif
#ifdef INDIVIDUAL_BASED_ANIMALS
	outputGrazerFile<<", GonadBiomass, FoodBeforeEating, FoodAfterEating, Grazing, CarryingCapacity, ReproductionInvestment, StroganovAdjustment, DepthInMeters, LightAtDepth, LightAtSurface, FoodBiomass, FoodBiomassAtSurface, PlanktivoreBiomassAtSurface, PreviousFitnessValue, CurrentFitnessValue, FitnessDifference, HoursInStarvation, PredatedBiomass, PredationSafety, PredationSafetyAtSurface, PredationFitness, Stage, CohortID"<<endl;
	grazerTraceFile<<"Depth, Column, Time, AlgaeType, Stage, DepthInMeters, LightAtDepth, LightAtSurface, FoodBiomass, FoodBiomassAtSurface, PlanktivoreBiomassAtSurface, LastMigration, GrazerCount, GrazerBiomass, PreviousFitnessValue, CurrentFitnessValue, FitnessDifference, HoursInStarvation, PredatedBiomass, PredationSafety, PredationSafetyAtSurface, PredationFitness, MigrationConstant, CohortID"<<endl;
	predatorTraceFile<<"Depth, Column, Time, AlgaeType, PredatorStage, PredatorIndividuals, PredatorBodyBiomass, PredatorGonadBiomass, PredatorCohortID"<<endl;
#endif
	outputPhysicalFile<<"Depth, Column, Time, AlgaeType, Temperature, TemperatureAngularFrequency, TemperatureSine, SaltAtDepthExponent, SaltConcentration, SaltEffect, SaltExponent, PhosphorusAtDepthExponent, PhosphorusConcentration, PhosphorusConcentrationAtBottom, LightAtTop"<<endl;
	writeSimulatedParameters(simArguments.outputParameterRoute);
	// Report start of the simulation
	cout<<"Simulation started for "<<simulation_cycles<<" cycles."<<endl;
	/*Clear lake light the day before*/
	for(int depthIndex=0; depthIndex<MAX_DEPTH_INDEX; depthIndex++){
		for(int columnIndex=0; columnIndex<MAX_COLUMN_INDEX; columnIndex++){
			this->previousLakeLightAtDepth[depthIndex][columnIndex]=0.0f;
		}
	}
	copyPointersToAnimalDynamics();
	grazerDynamics.initializeSimulationStructures();
	for(current_hour=0; current_hour<simulation_cycles; current_hour++){
		/* Register standard biomass and slough to file at the given hour frequency*/
		if(current_hour%TIME_MESSAGE_RESOLUTION==0)
			cout<<"Simulation hour: "<<current_hour<<endl;
		step();
#ifdef INDIVIDUAL_BASED_ANIMALS
		if(grazerDynamics.tracedCohort.numberOfIndividuals!=0){
			grazerTraceFile<<grazerDynamics.animalTraceBuffer.str();
		}

#endif
		if(current_hour%TIME_OUTPUT_RESOLUTION==0){
#ifdef EXTENDED_OUTPUT
			outputInitialAlgaeFile<<initialAlgaeBuffer.str();
#endif
			outputAlgaeFile<<algaeBuffer.str();
			outputGrazerFile<<grazerDynamics.animalBiomassBuffer.str();
			outputPredatorFile<<predatorDynamics.animalBiomassBuffer.str();

#ifdef EXTENDED_OUTPUT
			predatorTraceFile<<predatorDynamics.animalTraceBuffer.str();
#endif
		}

#ifdef CHECK_ASSERTIONS
		outputAssertionViolationFile<<assertionViolationBuffer.str();
		assertionViolationBuffer.str("");
#endif
#ifdef CHECK_GRAZER_LOWER_LIMIT
		if(this->zooplankton_count_summing<=MINIMUM_VIABLE_GRAZER_POPULATION){
			cout<<"Simulation halted because the grazer summing: "<<this->zooplankton_count_summing<<" is lower than the minimum viable grazer population: "<<MINIMUM_VIABLE_GRAZER_POPULATION<<"."<<endl;
			break;
		}
#endif
	}
	closeSimulationFiles();
	cout<<"Simulation finished."<<endl;
	return 0;
}

void FoodWebModel::FoodWebModel::setGeneralParameters(
		const SimulationArguments& simArguments) {
	/*Read numeric parameters*/
	this->algae_biomass_differential_production_scale =
			simArguments.algae_biomass_differential_production_scale;
	this->simulation_cycles = simArguments.simulationCycles;
	this->phosphorus_half_saturation = simArguments.phosphorus_half_saturation;
	this->phosphorus_functional_factor =
			simArguments.phosphorus_functional_factor;
	this->phosphorus_functional_constant_response_1 =
			simArguments.phosphorus_functional_constant_response_1;
	this->phosphorus_functional_constant_response_2 =
			simArguments.phosphorus_functional_constant_response_2;
	this->phosphorus_functional_step_1 =
			simArguments.phosphorus_functional_step_1;
	this->nitrogen_half_saturation = simArguments.nitrogen_half_saturation;
	this->nitrogen_functional_constant_response =
			simArguments.nitrogen_functional_constant_response;
	this->nitrogen_functional_step = simArguments.nitrogen_functional_step;
	this->nitrogen_phosphorus_lower_bound =
			simArguments.nitrogen_phosphorus_lower_bound;
	this->nitrogen_phosphorus_upper_bound =
			simArguments.nitrogen_phosphorus_upper_bound;
	this->temperature_optimal = simArguments.temperature_optimal;
	this->temperature_steepness = simArguments.temperature_steepness;
	this->temperature_suppression_steepness =
			simArguments.temperature_suppression_steepness;
	this->temperature_max = simArguments.temperature_max;
	this->light_allowance_weight = simArguments.light_allowance_weight;
	this->light_allowance_proportion = simArguments.light_allowance_proportion;
	this->nutrient_derivative = simArguments.nutrient_derivative;
	this->nutrient_growth = simArguments.nutrient_growth;
	this->algal_respiration_at_20_degrees =
			simArguments.algal_respiration_at_20_degrees;
	this->exponential_temperature_algal_respiration_coefficient =
			simArguments.exponential_temperature_algal_respiration_coefficient;
	this->intrinsic_algae_mortality_rate =
			simArguments.intrinsic_algae_mortality_rate;
	this->intrinsic_settling_rate = simArguments.intrinsic_settling_rate;
	this->algal_fraction_sloughed = simArguments.algal_fraction_sloughed;
	this->maximum_algae_resources_death =
			simArguments.maximum_algae_resources_death;
	this->light_steepness = simArguments.light_steepness;
	this->light_lower_quantile = simArguments.light_lower_quantile;
	this->light_upper_quantile = simArguments.light_upper_quantile;
	this->light_steepness_factor = simArguments.light_steepness_factor;
	this->diatom_attenuation_coefficient =
			simArguments.diatom_attenuation_coefficient;
	this->limitation_scale_weight = simArguments.limitation_scale_weight;
	this->algal_carrying_capacity_coefficient =
			simArguments.algal_carrying_capacity_coefficient;
	this->algal_carrying_capacity_intercept =
			simArguments.algal_carrying_capacity_intercept;
	this->maximum_found_algal_biomass =
			simArguments.maximum_found_algal_biomass;
	this->algal_mortality_scale = simArguments.algal_mortality_scale;
	this->phosphorous_weight = simArguments.phosphorous_weight;
	this->decaying_phosphorus_factor = simArguments.decaying_phosphorus_factor;
	this->retained_phosphorus_factor = simArguments.retained_phosphorus_factor;
	this->wash_up_dead_biomass_proportion =
			simArguments.wash_up_dead_biomass_proportion;
	this->wash_down_dead_biomass_proportion =
			simArguments.wash_down_dead_biomass_proportion;
	this->algae_biomass_conservation_factor =
			simArguments.algae_biomass_conservation_factor;
	this->grazer_layer_center_index = simArguments.grazer_layer_center_index;
}

void FoodWebModel::FoodWebModel::setFileParameters(
		const SimulationArguments& simArguments) {
	/*Read numeric parameters*/
	setGeneralParameters(simArguments);
#ifdef ADD_DEAD_BIOMASS_NUTRIENTS
	this->reabsorbed_algal_nutrients_proportion=simArguments.reabsorbed_algal_nutrients_proportion;
#endif
	/*Initialize the generator of random numbers*/
	this->random_seed=simArguments.random_seed;
	this->randomNumberGenerator=new default_random_engine(this->random_seed);
	srand(this->random_seed);
	initializeGrazerAttributes(simArguments);
}


void FoodWebModel::FoodWebModel::initializeGrazerAttributes(const SimulationArguments& simArguments){
	grazerDynamics.animal_base_mortality_proportion=
			simArguments.grazer_base_mortality_proportion;
	grazerDynamics.maximum_distance_daphnia_swum_in_rows_per_hour=MAXIMUM_DISTANCE_DAPHNIA_SWUM_IN_METERS_PER_HOUR * MAX_COLUMN_INDEX/ this->ZMax;
	simArguments.grazer_base_mortality_proportion;

	grazerDynamics.vertical_migration_buffer_size = 2
			* grazerDynamics.maximum_distance_daphnia_swum_in_rows_per_hour + 1;
	grazerDynamics.filtering_rate_per_daphnia = simArguments.grazer_filtering_rate_per_individual;
	grazerDynamics.filtering_rate_per_individual_in_cell_volume=grazerDynamics.filtering_rate_per_daphnia*(MILLILITER_TO_VOLUME_PER_CELL);
	grazerDynamics.consumption_temperature_factor=simArguments.grazer_consumption_temperature_factor;
	grazerDynamics.filtering_length_coefficient = simArguments.grazer_filtering_length_coefficient;
	grazerDynamics.filtering_length_exponent = simArguments.grazer_filtering_length_exponent;
	grazerDynamics.filtering_coefficient = simArguments.grazer_filtering_coefficient;
	grazerDynamics.filtering_exponent = simArguments.grazer_filtering_exponent;
	grazerDynamics.basal_respiration_weight = simArguments.grazer_basal_respiration_weight;
	grazerDynamics.k_value_respiration = simArguments.grazer_k_value_respiration;
	grazerDynamics.animal_carrying_capacity_coefficient = simArguments.grazer_carrying_capacity_coefficient;
	grazerDynamics.animal_carrying_capacity_intercept = simArguments.grazer_carrying_capacity_intercept;
	grazerDynamics.animal_carrying_capacity_amplitude = simArguments.grazer_carrying_capacity_amplitude;
	grazerDynamics.animal_carrying_capacity_constant = simArguments.grazer_carrying_capacity_constant;
	grazerDynamics.migration_consumption = simArguments.grazer_migration_consumption;
	grazerDynamics.dead_animals_per_lost_biomass_and_concentration = simArguments.grazer_dead_animals_per_lost_biomass_and_concentration;
	grazerDynamics.maximum_found_animal_biomass=simArguments.grazer_maximum_found_biomass;
	grazerDynamics.food_conversion_factor=CELL_VOLUME_IN_LITER;
	grazerDynamics.food_starvation_threshold=simArguments.grazer_food_starvation_threshold;
	grazerDynamics.max_hours_without_food=simArguments.grazer_max_hours_without_food;
	grazerDynamics.maximum_age_in_hours=simArguments.grazer_maximum_age_in_hours;
	grazerDynamics.incubation_hours=simArguments.grazer_incubation_hours;
	grazerDynamics.egg_allocation_threshold=simArguments.grazer_egg_allocation_threshold;
	grazerDynamics.ovipositing_period=simArguments.grazer_ovipositing_period;
	grazerDynamics.maturation_hours=simArguments.grazer_maturation_hours;
	grazerDynamics.agglomeration_cohort_threshold=simArguments.grazer_agglomeration_cohort_threshold;
	grazerDynamics.reproduction_proportion_investment_amplitude=simArguments.grazer_reproduction_proportion_investment_amplitude;
	grazerDynamics.reproduction_proportion_investment_coefficient=simArguments.grazer_reproduction_proportion_investment_coefficient;
	grazerDynamics.reproduction_proportion_investment_intercept=simArguments.grazer_reproduction_proportion_investment_intercept;
	grazerDynamics.reproduction_proportion_investment_intercept=simArguments.grazer_reproduction_proportion_investment_intercept;
	grazerDynamics.reproduction_proportion_investment_constant=simArguments.grazer_reproduction_proportion_investment_constant;
	grazerDynamics.reproduction_proportion_investment_multiplier=simArguments.grazer_reproduction_proportion_investment_multiplier;
	grazerDynamics.starvation_factor=simArguments.grazer_starvation_factor;
	grazerDynamics.dead_animal_proportion=simArguments.grazer_dead_animal_proportion;
	grazerDynamics.critical_depth=simArguments.grazer_critical_depth;
	grazerDynamics.minimum_tolerable_light=simArguments.grazer_minimum_tolerable_light;
	grazerDynamics.critical_light_intensity=simArguments.grazer_critical_light_intensity;
	grazerDynamics.ind_food_starvation_threshold=simArguments.grazer_ind_food_starvation_threshold;
	grazerDynamics.starvation_max_hours=simArguments.grazer_starvation_max_hours;
	grazerDynamics.light_optimal_value=simArguments.grazer_light_optimal_value;
	grazerDynamics.velocity_downward_pull=simArguments.grazer_velocity_downward_pull;
	grazerDynamics.layer_center_index=simArguments.grazer_layer_center_index;
	grazerDynamics.light_migration_weight=simArguments.grazer_light_migration_weight;
	grazerDynamics.kairomones_level_day=simArguments.kairomones_level_day;
	grazerDynamics.kairomones_level_night=simArguments.kairomones_level_night;
	grazerDynamics.kairomones_thermocline=simArguments.kairomones_thermocline;
	grazerDynamics.planktivore_biomass_center_day=simArguments.planktivore_biomass_center_day;
	grazerDynamics.planktivore_biomass_center_night=simArguments.planktivore_biomass_center_night;
	grazerDynamics.planktivore_biomass_width=simArguments.planktivore_biomass_width;
	grazerDynamics.predation_index=simArguments.grazer_predation_index;
	grazerDynamics.max_vertical_migration=simArguments.max_vertical_migration;
	grazerDynamics.max_horizontal_migration=simArguments.max_horizontal_migration;
	grazerDynamics.max_search_steps = simArguments.grazer_max_search_steps;
	grazerDynamics.random_walk_probability_weight= simArguments.grazer_random_walk_probability_weight;
	grazerDynamics.minimum_predation_safety=simArguments.grazer_minimum_predation_safety;
	grazerDynamics.maximum_light_tolerated=simArguments.grazer_maximum_light_tolerated;
	grazerDynamics.light_safety_weight=simArguments.grazer_light_safety_weight;
	grazerDynamics.light_safety_threshold=simArguments.grazer_light_safety_threshold;
	grazerDynamics.cohort_splitting_limit=simArguments.grazer_cohort_splitting_limit;
#ifdef ADD_DEAD_BIOMASS_NUTRIENTS
	grazerDynamics.reabsorbed_animal_nutrients_proportion=simArguments.grazer_reabsorbed_animal_nutrients_proportion;
#endif
	grazerDynamics.animalRandomGenerator=this->randomNumberGenerator;

	grazerDynamics.cohortID=&(this->cohortID);

}

/* A method to print the simulation mode in function of the defined flags*/
void FoodWebModel::FoodWebModel::printSimulationMode(){
#ifdef DEBUG_MODE
	cout<<"Running in debug mode."<<endl;
#else
	cout<<"Running in production mode."<<endl;
#endif
#ifdef SIGMOID_CONCENTRATION
	cout<<"Considering sigmoid concentration"<<endl;
#else
	cout<<"Considering non-sigmoid concentration"<<endl;
#endif
#ifdef HOMOGENEOUS_DEPTH
	cout<<"Considering homogeneous depth"<<endl;
#else
	cout<<"Considering non-homogeneous depth"<<endl;
#endif
#ifdef RADIATED_CHEMICAL
	cout<<"Modeling chemical concentration as radiated"<<endl;
#else
	cout<<"Modeling chemical concentration as non-radiated with depth distance to column bottom"<<endl;
#endif
#ifdef STABLE_CHLOROPHYLL
	cout<<"Running with static biomass differential."<<endl;
#else
	cout<<"Running with dynamic biomass differential."<<endl;
#endif
#ifdef USE_PHOTOPERIOD
	cout<<"Using sinusoidal photoperiod."<<endl;
#elif defined(USE_YEARLY_LIGHT_SURFACE)
	cout<<"Using yearly table-based photoperiod."<<endl;
#else
	cout<<"Using table-based photoperiod."<<endl;
#endif
#ifdef USE_LITERATURE_AND_DATA_CONSTANTS
	cout<<"Using literature constants."<<endl;
#else
	cout<<"Using model-tuned constants."<<endl;
#endif
#ifdef ADDITIVE_TURBIDITY
	cout<<"Using additive turbidity."<<endl;
#else
	cout<<"Using multiplicative turbidity."<<endl;
#endif
#ifdef ADD_CONSTANT_BIOMASS_DIFFERENTIAL
	cout<<"Using constant base biomass differential."<<endl;
#elif defined(ADD_VARIABLE_BIOMASS_DIFFERENTIAL)
	cout<<"Using depth-dependent base biomass differential."<<endl;
#else
	cout<<"Using no base biomass differential."<<endl;
#endif
#ifdef LIMITATION_MINIMUM
	cout<<"Calculating resource limitation as minimum."<<endl;
#elif defined(WEIGHTED_RESOURCE_LIMITATION)
	cout<<"Calculating resource limitation as weighted average."<<endl;
#else
	cout<<"Calculating resource limitation as product."<<endl;
#endif
#ifdef TIME_VARIABLE_PHOSPHORUS_CONCENTRATION_AT_BOTTOM
	cout<<"Using time-variable phosphorous concentration at bottom."<<endl;
#else
	cout<<"Using constant phosphorous concentration at bottom."<<endl;
#endif
#ifdef GRAZING_EFFECT_ON_ALGAE_BIOMASS
	cout<<"Grazing removed from algae biomass."<<endl;
#else
	cout<<"Grazing kept in algae biomass."<<endl;
#endif
#ifdef MIGRATE_ZOOPLANKTON_AT_HOUR
	cout<<"Migrating zooplankton vertically at each hour of the day."<<endl;
#ifdef ZOOPLANKTON_ACCUMULATION
	cout<<"Accumulating zooplankton count at extreme depth values."<<endl;
#else
	cout<<"Discarding zooplankton excess from vertical migration."<<endl;
#endif
#elif defined(LOCALE_SEARCH_ZOOPLANKTON_MIGRATION)
	cout<<"Using locale search zooplankton migration."<<endl;
#else
	cout<<"Maintaining zooplankton at each depth at each hour of the day."<<endl;
#endif
#ifdef SATURATION_GRAZING
	cout<<"Using saturation grazing."<<endl;
#else
	cout<<"Using linear grazing."<<endl;
#endif
#ifdef ADD_GRAZER_PREDATORY_PRESSURE
	cout<<"Using sigmoidal predatory pressure on grazers."<<endl;
#else
	cout<<"Using no predatory pressure on grazers."<<endl;
#endif
#ifdef CHECK_GRAZER_LOWER_LIMIT
	cout<<"Checking grazer lower limit."<<endl;
#endif
#ifdef CHECK_ASSERTIONS
	cout<<"Checking assertions."<<endl;
#else
	cout<<"Not checking assertions."<<endl;
#endif
#ifdef GRAZER_CARRYING_CAPACITY_MORTALITY
	cout<<"Using carrying-capacity dependent grazer mortality."<<endl;
#else
	cout<<"Using constant grazer mortality."<<endl;
#endif
#ifdef ALGAL_CARRYING_CAPACITY_MORTALITY
	cout<<"Using carrying-capacity dependent algal mortality."<<endl;
#else
	cout<<"Using constant algal mortality."<<endl;
#endif
#ifdef PROPORTIONAL_LIGHT
	cout<<"Using proportional light limitation."<<endl;
#elif defined(LINEAR_LIGHT)
	cout<<"Using linear light limitation."<<endl;
#elif defined(AQUATOX_LIGHT_ALLOWANCE)
	cout<<"Using AquaTox light limitation."<<endl;
#else
	cout<<"Using sigmoid light limitation."<<endl;
#endif
#ifdef NUTRIENT_LIMITATION_GLM
	cout<<"Using GLM nutrient limitation."<<endl;
#elif defined(NUTRIENT_LIMITATION_QUOTIENT)
	cout<<"Using quotient-based nutrient limitation."<<endl;
#elif defined(SEVERAL_LEVELS_LIMITATION_QUOTIENT)
	cout<<"Using several levels nutrient limitation."<<endl;
#else
	cout<<"Using simple linear nutrient limitation."<<endl;
#endif
#ifdef INDIVIDUAL_BASED_ANIMALS
	cout<<"Using cohort-based approach for animal dynamics."<<endl;
#ifdef ANIMAL_STARVATION_HOURS_WITHOUT_FOOD
	cout<<"Modeling animal starvation as hours without food."<<endl;
#elif defined(ANIMAL_STARVATION_PROPORTION_LOST_BIOMASS)
#ifdef ACCUMULATIVE_HOUR_STARVATION
	cout<<"Modeling animal starvation as proportion of lost biomass."<<endl;
#else
	cout<<"Modeling animal starvation accumulative lost biomass."<<endl;
#endif
#else
	cout<<"Not modeling animal starvation."<<endl;
#endif
#ifdef GRAZING_DEPENDING_ON_WEIGHT
	cout<<"Modeling grazing dependent on weight."<<endl;
#else
	cout<<"Modeling grazing independent on weight."<<endl;
#endif
#ifdef REPORT_COHORT_INFO
	cout<<"Reporting cohort info."<<endl;
#else
	cout<<"Not reporting cohort info."<<endl;
#endif
#ifdef COUNT_EGGS
	cout<<"Counting eggs."<<endl;
#else
	cout<<"Not counting eggs."<<endl;
#endif
#ifdef EXPONENTIAL_GONAD_ALLOCATION
	cout<<"Using exponential gonad allocation."<<endl;
#elif defined(SIGMOIDAL_GONAD_ALLOCATION)
	cout<<"Using sigmoidal gonad allocation."<<endl;
#else
	cout<<"Using exponential gonad allocation."<<endl;
#endif
#else
	cout<<"Using differential equations for animal dynamics."<<endl;
#endif
#ifdef PRINT_ANIMAL_SUMMATORY
	cout<<"Printing animal summatory."<<endl;
#else
	cout<<"Not printing animal summatory."<<endl;
#endif
#if defined(INDIVIDUAL_BASED_ANIMALS) && defined(CREATE_NEW_COHORTS) && defined(MATURE_JUVENILES)
	cout<<"Maturing juveniles."<<endl;
#else
	cout<<"Not maturing juveniles."<<endl;
#endif
#ifdef ADD_DEAD_BIOMASS_NUTRIENTS
	cout<<"Reusing dead biomass."<<endl;
#else
	cout<<"Not reusing dead biomass."<<endl;
#endif
#ifdef LAKE_DATA_TEMPERATURE
	cout<<"Using temperature from lake data."<<endl;
#else
	cout<<"Using temperature from sinusoidal model."<<endl;
#endif
#ifdef TEMPERATURE_MEDIATED_GRAZING
	cout<<"Using temperature mediated grazing."<<endl;
#else
	cout<<"Not using temperature mediated grazing."<<endl;
#endif
#ifdef STARVATION_MORTALITY
	cout<<"Using starvation mortality."<<endl;
#else
	cout<<"Not using starvation mortality."<<endl;
#endif
#ifdef LIGHT_BASED_MIGRATION_VARIABLE_FREQUENCY
	cout<<"Using light-based migration with variable frequency."<<endl;
#elif defined(LIGHT_BASED_MIGRATION_FIXED_FREQUENCY)
	cout<<"Using light-based migration with fixed frequency."<<endl;
#elif defined(OPTIMAL_VALUE_MIGRATION)
	cout<<"Using optimal migration index."<<endl;
#else
	cout<<"Not using light-based migration."<<endl;
#endif
#ifdef CONSUME_DURING_MIGRATION
	cout<<"Consuming biomass during migration."<<endl;
#else
	cout<<"Not consuming biomass during migration."<<endl;
#endif
#ifdef REGISTER_ALL_COHORTS
	cout<<"Registering all cohorts."<<endl;
#else
	cout<<"Registering traced cohorts."<<endl;
#endif
#ifdef ANIMAL_COHORT_MAP
	cout<<"Using animal cohort map."<<endl;
#else
#ifdef CONGLOMERATE_ALL_COHORTS
	cout<<"Using cohort conglomerate."<<endl;
#ifdef SPLIT_COHORTS
	cout<<"Splitting cohorts."<<endl;
#else
	cout<<"Not splitting cohorts."<<endl;
#endif
#else
	cout<<"Using animal cohort vector."<<endl;
#endif
#endif
#ifdef RANDOM_WALK_MIGRATION
	cout<<"Using random walk migration."<<endl;
#ifdef DAPHNIA_CIRCADIAN_CYCLE
	cout<<"Using daphnia circadian cycle"<<endl;
#else
	cout<<"Not using daphnia circadian cycle"<<endl;
#endif
#else
	cout<<"Not using random walk migration."<<endl;
#endif
#ifdef LINEAR_MIGRATION_COMBINATION
	cout<<"Using linear migration combination"<<endl;
#else
	cout<<"Using rule-based migration combination"<<endl;
#endif
#ifdef MINIMUM_PREDATION_SAFETY
	cout<<"Using minimum predation safety threshold"<<endl;
#else
	cout<<"Not using minimum predation safety threshold"<<endl;
#endif
#ifdef THRESHOLD_LIGHT_SAFETY
	cout<<"Using threshold light safety"<<endl;
#else
	cout<<"Not using threshold light safety"<<endl;
#endif
	cout<<"Using phosphorous weight "<<this->phosphorous_weight<<"."<<endl;
	cout<<"Using decaying phosphorous factor "<<this->decaying_phosphorus_factor<<"."<<endl;
	cout<<"Using retained phosphorous factor "<<this->retained_phosphorus_factor<<"."<<endl;
	cout<<"Using nutrient derivative "<<this->nutrient_derivative<<"."<<endl;
	cout<<"Using nutrient growth "<<this->nutrient_growth<<"."<<endl;
	cout<<"Using maximum light data "<<MAX_LIGHT_DATA<<"."<<endl;
	cout<<"Using grazer feeding saturation adjustment weight "<<FEEDING_SATURATION_ADJUSTMENT<<"."<<endl;
	cout<<"Using grazer water filtering per individual "<<grazerDynamics.filtering_rate_per_individual_in_cell_volume<<" liters/hour."<<endl;
	cout<<"Using grazer consumption temperature factor "<<grazerDynamics.consumption_temperature_factor<<"."<<endl;
	cout<<"Using grazer filtering length coefficient "<<grazerDynamics.filtering_length_coefficient<<"."<<endl;
	cout<<"Using grazer filtering length exponent "<<grazerDynamics.filtering_length_exponent<<" ."<<endl;
	cout<<"Using grazer filtering coefficient "<<grazerDynamics.filtering_coefficient<<"."<<endl;
	cout<<"Using grazer filtering exponent "<<grazerDynamics.filtering_exponent<<"."<<endl;
	cout<<"Using algae biomass differential weight "<<this->algae_biomass_differential_production_scale<<"."<<endl;
	cout<<"Using base grazer mortality factor "<<grazerDynamics.animal_base_mortality_proportion<<"."<<endl;
	cout<<"Using basal respiration weight "<<grazerDynamics.basal_respiration_weight<<"."<<endl;
	cout<<"Using respiration K value "<<grazerDynamics.k_value_respiration<<"."<<endl;
	cout<<"Using grazer layer center index "<<this->grazer_layer_center_index<<"."<<endl;
	cout<<"Using grazer carrying capacity coefficient "<<grazerDynamics.animal_carrying_capacity_coefficient<<"."<<endl;
	cout<<"Using grazer carrying capacity intercept "<<grazerDynamics.animal_carrying_capacity_intercept<<"."<<endl;
	cout<<"Using grazer carrying capacity amplitude "<<grazerDynamics.animal_carrying_capacity_amplitude<<"."<<endl;
	cout<<"Using grazer carrying capacity constant "<<grazerDynamics.animal_carrying_capacity_constant<<"."<<endl;
	cout<<"Using grazer migration consumption "<<grazerDynamics.migration_consumption<<"."<<endl;
	cout<<"Using grazer dead animals per lost biomass and concentration "<<grazerDynamics.dead_animals_per_lost_biomass_and_concentration<<"."<<endl;
	cout<<"Using grazer max hours without food "<<grazerDynamics.max_hours_without_food<<"."<<endl;
	cout<<"Using grazer food starvation threshold "<<grazerDynamics.food_starvation_threshold<<"."<<endl;
	cout<<"Using grazer maximum age in hours "<<grazerDynamics.maximum_age_in_hours<<"."<<endl;
	cout<<"Using grazer incubation hours "<<grazerDynamics.incubation_hours<<"."<<endl;
	cout<<"Using egg allocation threshold "<<grazerDynamics.egg_allocation_threshold<<"."<<endl;
	cout<<"Using maximum found grazer biomass "<<grazerDynamics.maximum_found_animal_biomass<<"."<<endl;
	cout<<"Using grazer ovipositing frequency "<<grazerDynamics.ovipositing_period<<"."<<endl;
	cout<<"Using grazer maturation hours "<<grazerDynamics.maturation_hours<<"."<<endl;
	cout<<"Using grazer agglomeration cohort threshold "<<grazerDynamics.agglomeration_cohort_threshold<<"."<<endl;
	cout<<"Using grazer reproduction proportion investment amplitude "<<grazerDynamics.reproduction_proportion_investment_amplitude<<"."<<endl;
	cout<<"Using grazer reproduction proportion investment coefficient "<<grazerDynamics.reproduction_proportion_investment_coefficient<<"."<<endl;
	cout<<"Using grazer reproduction proportion investment intercept "<<grazerDynamics.reproduction_proportion_investment_intercept<<"."<<endl;
	cout<<"Using grazer reproduction proportion investment constant "<<grazerDynamics.reproduction_proportion_investment_constant<<"."<<endl;
	cout<<"Using grazer reproduction proportion investment multiplier "<<grazerDynamics.reproduction_proportion_investment_multiplier<<"."<<endl;
	cout<<"Using grazer light migration weight "<<grazerDynamics.light_migration_weight<<"."<<endl;
	cout<<"Using grazer starvation factor "<<grazerDynamics.starvation_factor<<"."<<endl;
	cout<<"Using grazer dead animal proportion "<<grazerDynamics.dead_animal_proportion<<"."<<endl;
	cout<<"Using grazer critical depth "<<grazerDynamics.critical_depth<<"."<<endl;
	cout<<"Using grazer critical light intensity "<<grazerDynamics.critical_light_intensity<<"."<<endl;
	cout<<"Using grazer individual food starvation threshold "<<grazerDynamics.ind_food_starvation_threshold<<"."<<endl;
	cout<<"Using grazer starvation max hours "<<grazerDynamics.starvation_max_hours<<"."<<endl;
	cout<<"Using grazer critical light intensity "<<grazerDynamics.critical_light_intensity<<"."<<endl;
	cout<<"Using grazer light optimal value "<<grazerDynamics.light_optimal_value<<"."<<endl;
	cout<<"Using grazer velocity downward pull "<<grazerDynamics.velocity_downward_pull<<"."<<endl;
	cout<<"Using kairomones level day "<<grazerDynamics.kairomones_level_day<<"."<<endl;
	cout<<"Using kairomones level night "<<grazerDynamics.kairomones_level_night<<"."<<endl;
	cout<<"Using kairomones thermocline value "<<grazerDynamics.planktivore_biomass_center_day<<"."<<endl;
	cout<<"Using planktivore biomass center day "<<grazerDynamics.planktivore_biomass_center_night<<"."<<endl;
	cout<<"Using planktivore biomass center night "<<grazerDynamics.planktivore_biomass_width<<"."<<endl;
	cout<<"Using planktivore biomass width "<<grazerDynamics.kairomones_thermocline<<"."<<endl;
	cout<<"Using planktivore predation index "<<grazerDynamics.predation_index<<"."<<endl;
	cout<<"Using grazer maximum vertical migration value "<<grazerDynamics.max_vertical_migration<<"."<<endl;
	cout<<"Using grazer maximum horizontal migration value "<<grazerDynamics.max_horizontal_migration<<"."<<endl;
	cout<<"Using maximum migration search steps "<<grazerDynamics.max_search_steps<<"."<<endl;
	cout<<"Using random walk probability weight "<<grazerDynamics.random_walk_probability_weight<<"."<<endl;
	cout<<"Using maximum predation safety "<<grazerDynamics.minimum_predation_safety<<"."<<endl;
	cout<<"Using maximum light tolerated "<<grazerDynamics.maximum_light_tolerated<<"."<<endl;
	cout<<"Using grazer light safety weight "<<grazerDynamics.light_safety_weight<<"."<<endl;
	cout<<"Using grazer light safety threshold "<<grazerDynamics.light_safety_threshold<<"."<<endl;
	cout<<"Using cohort splitting limit "<<grazerDynamics.cohort_splitting_limit<<"."<<endl;
	cout<<"Using minimum tolerable light for daphnia "<<grazerDynamics.minimum_tolerable_light<<"."<<endl;
#ifdef ADD_DEAD_BIOMASS_NUTRIENTS
	cout<<"Using grazer reabsorbed nutrients proportion "<<grazerDynamics.reabsorbed_animal_nutrients_proportion<<"."<<endl;
#endif
	cout<<"Using random seed "<<this->random_seed<<"."<<endl;
	cout<<"Using phosphorus half saturation "<<this->phosphorus_half_saturation<<"."<<endl;
	cout<<"Using phosphorus functional factor "<<this->phosphorus_functional_factor<<"."<<endl;
	cout<<"Using phosphorus functional constant response 1 "<<this->phosphorus_functional_constant_response_1<<"."<<endl;
	cout<<"Using phosphorus functional constant response 2 "<<this->phosphorus_functional_constant_response_2<<"."<<endl;
	cout<<"Using phosphorus functional step 1 "<<this->phosphorus_functional_step_1<<"."<<endl;
	cout<<"Using nitrogen half saturation "<<this->nitrogen_half_saturation<<"."<<endl;
	cout<<"Using nitrogen functional constant response "<<this->nitrogen_functional_constant_response<<"."<<endl;
	cout<<"Using nitrogen functional step "<<this->nitrogen_functional_step<<"."<<endl;
	cout<<"Using nitrogen lower factor "<<this->nitrogen_phosphorus_lower_bound<<"."<<endl;
	cout<<"Using nitrogen upper factor "<<this->nitrogen_phosphorus_upper_bound<<"."<<endl;
	cout<<"Using temperature optimal "<<this->temperature_optimal<<"."<<endl;
	cout<<"Using temperature steepness "<<this->temperature_steepness<<"."<<endl;
	cout<<"Using temperature suppression steepness "<<this->temperature_suppression_steepness<<"."<<endl;
	cout<<"Using maximum temperature supported "<<this->temperature_max<<"."<<endl;
	cout<<"Using light allowance weight "<<this->light_allowance_weight<<"."<<endl;
	cout<<"Using light allowance proortion "<<this->light_allowance_proportion<<"."<<endl;
	cout<<"Using base algal respiration at 20 degrees "<<this->algal_respiration_at_20_degrees<<"."<<endl;
	cout<<"Using algal respiration exponential base coefficient "<<this->exponential_temperature_algal_respiration_coefficient<<"."<<endl;
	cout<<"Using intrinsic algae mortality rate "<<this->intrinsic_algae_mortality_rate<<"."<<endl;
	cout<<"Using maximum algae dead to stress due to lack of resources "<<this->maximum_algae_resources_death<<"."<<endl;
	cout<<"Using light steepness "<<this->light_steepness<<"."<<endl;
	cout<<"Using light lower quantile "<<this->light_lower_quantile<<"."<<endl;
	cout<<"Using light upper quantile "<<this->light_upper_quantile<<"."<<endl;
	cout<<"Using light steepness factor"<<this->light_steepness_factor<<"."<<endl;
	cout<<"Using diatom attenuation coefficient "<<this->diatom_attenuation_coefficient<<"."<<endl;
	cout<<"Using limitation product scale weight "<<this->limitation_scale_weight<<"."<<endl;
	cout<<"Using algal carrying capacity coefficient "<<this->algal_carrying_capacity_coefficient<<"."<<endl;
	cout<<"Using algal carrying capacity intercept "<<this->algal_carrying_capacity_intercept<<"."<<endl;
	cout<<"Using maximum found algal biomass "<<this->maximum_found_algal_biomass<<"."<<endl;
#ifdef ADD_DEAD_BIOMASS_NUTRIENTS
	cout<<"Using reabsorbed algal nutrients proportion "<<this->reabsorbed_algal_nutrients_proportion<<"."<<endl;
#endif
	cout<<"Using algal mortality scale "<<this->algal_mortality_scale<<"."<<endl;
	cout<<"Using proportion of wash-up for dead biomass "<<this->wash_up_dead_biomass_proportion<<"."<<endl;
	cout<<"Using proportion of wash-down for dead biomass "<<this->wash_down_dead_biomass_proportion<<"."<<endl;
	cout<<"Using algal biomass conservation factor "<<this->algae_biomass_conservation_factor<<"."<<endl;
	cout<<"Using algal intrinsic settling rate "<<this->intrinsic_settling_rate<<"."<<endl;
	cout<<"Using algal fraction sloughed "<<this->algal_fraction_sloughed<<"."<<endl;

}


void FoodWebModel::FoodWebModel::writeSimulatedParameters(const string& parameterSimulationRoute){
	ofstream parameterFileStream;
	parameterFileStream.open(parameterSimulationRoute.c_str());
	if (parameterFileStream.is_open()){
		cout<<"File "<<parameterSimulationRoute<<" open for simulation parameter register."<<endl;
		parameterFileStream<<"PhosphorousWeight;"<<this->phosphorous_weight<<endl;
		parameterFileStream<<"NutrientDerivative;"<<this->nutrient_derivative<<endl;
		parameterFileStream<<"NutrientGrowth;"<<this->nutrient_growth<<endl;
		parameterFileStream<<"DecayingPhosphorusFactor;"<<this->decaying_phosphorus_factor<<endl;
		parameterFileStream<<"RetainedPhosphorusFactor;"<<this->retained_phosphorus_factor<<endl;
		parameterFileStream<<"AlgaeBiomassDifferentialScale;"<<this->algae_biomass_differential_production_scale<<endl;
		parameterFileStream<<"AnimalBaseMortality;"<<grazerDynamics.animal_base_mortality_proportion<<endl;
		parameterFileStream<<"SimulationCycles;"<<this->simulation_cycles<<endl;
		parameterFileStream<<"FilteringRatePerDaphnia;"<<grazerDynamics.filtering_rate_per_daphnia<<endl;
		parameterFileStream<<"GrazerConsumptionTemperatureFactor;"<<grazerDynamics.consumption_temperature_factor<<endl;
		parameterFileStream<<"GrazerFilteringLengthCoefficient;"<<grazerDynamics.filtering_length_coefficient<<endl;
		parameterFileStream<<"GrazerFilteringLengthExponent;"<<grazerDynamics.filtering_length_exponent<<endl;
		parameterFileStream<<"GrazerFilteringCoefficient;"<<grazerDynamics.filtering_coefficient<<endl;
		parameterFileStream<<"GrazerFilteringExponent;"<<grazerDynamics.filtering_exponent<<endl;
		parameterFileStream<<"BasalRespirationWeight;"<<grazerDynamics.basal_respiration_weight<<endl;
		parameterFileStream<<"KValueRespiration;"<<grazerDynamics.k_value_respiration<<endl;
		parameterFileStream<<"GrazerLayerCenterIndex;"<<grazer_layer_center_index<<endl;
		parameterFileStream<<"GrazerCarryingCapacityCoefficient;"<<grazerDynamics.animal_carrying_capacity_coefficient<<endl;
		parameterFileStream<<"GrazerCarryingCapacityIntercept;"<<grazerDynamics.animal_carrying_capacity_intercept<<endl;
		parameterFileStream<<"GrazerCarryingCapacityAmplitude;"<<grazerDynamics.animal_carrying_capacity_amplitude<<endl;
		parameterFileStream<<"GrazerCarryingCapacityConstant;"<<grazerDynamics.animal_carrying_capacity_constant<<endl;
		parameterFileStream<<"GrazerMigrationConsumption;"<<grazerDynamics.migration_consumption<<endl;
		parameterFileStream<<"GrazerDeadAnimalsPerLostBiomassAndConcentration"<<grazerDynamics.dead_animals_per_lost_biomass_and_concentration<<endl;
		parameterFileStream<<"GrazerMaxHoursWithoutFood;"<<grazerDynamics.max_hours_without_food<<endl;
		parameterFileStream<<"GrazerFoodStarvationThreshold;"<<grazerDynamics.food_starvation_threshold<<endl;
		parameterFileStream<<"GrazerMaximumFoundBiomass;"<<grazerDynamics.maximum_found_animal_biomass<<endl;
		parameterFileStream<<"GrazerMaximumAgeInHours;"<<grazerDynamics.maximum_age_in_hours<<endl;
		parameterFileStream<<"GrazerIncubationHours;"<<grazerDynamics.incubation_hours<<endl;
		parameterFileStream<<"GrazerEggAllocationThreshold;"<<grazerDynamics.egg_allocation_threshold<<endl;
		parameterFileStream<<"GrazerOvipositingFrequency;"<<grazerDynamics.ovipositing_period<<endl;
		parameterFileStream<<"GrazerMaturationHours;"<<grazerDynamics.maturation_hours<<endl;
		parameterFileStream<<"GrazerAgglomerationCohortThreshold;"<<grazerDynamics.agglomeration_cohort_threshold<<endl;
		parameterFileStream<<"GrazerReproductionProportionInvestmentAmplitude;"<<grazerDynamics.reproduction_proportion_investment_amplitude<<endl;
		parameterFileStream<<"GrazerReproductionProportionInvestmentCoefficient;"<<grazerDynamics.reproduction_proportion_investment_coefficient<<endl;
		parameterFileStream<<"GrazerReproductionProportionInvestmentIntercept;"<<grazerDynamics.reproduction_proportion_investment_intercept<<endl;
		parameterFileStream<<"GrazerReproductionProportionInvestmentConstant;"<<grazerDynamics.reproduction_proportion_investment_constant<<endl;
		parameterFileStream<<"GrazerReproductionProportionInvestmentMultiplier;"<<grazerDynamics.reproduction_proportion_investment_multiplier<<endl;
		parameterFileStream<<"GrazerStarvationFactor;"<<grazerDynamics.starvation_factor<<endl;
		parameterFileStream<<"GrazerDeadAnimalProportion;"<<grazerDynamics.dead_animal_proportion<<endl;
		parameterFileStream<<"GrazerVelocityDownwardPull;"<<grazerDynamics.velocity_downward_pull<<endl;
		parameterFileStream<<"GrazerCriticalLightIntensity;"<<grazerDynamics.critical_light_intensity<<endl;
		parameterFileStream<<"GrazingStarvationThreshold;"<<grazerDynamics.ind_food_starvation_threshold<<endl;
		parameterFileStream<<"GrazingStarvationMaxHours;"<<grazerDynamics.starvation_max_hours<<endl;
		parameterFileStream<<"GrazerLightOptimalValue;"<<grazerDynamics.light_optimal_value<<endl;
		parameterFileStream<<"GrazerMigrationLightWeight;"<<grazerDynamics.light_migration_weight<<endl;
		parameterFileStream<<"KairomonesLevelDay;"<<grazerDynamics.kairomones_level_day<<endl;
		parameterFileStream<<"KairomonesLevelNight;"<<grazerDynamics.kairomones_level_night<<endl;
		parameterFileStream<<"KairomonesThermocline;"<<grazerDynamics.kairomones_thermocline<<endl;
		parameterFileStream<<"PlanktivoreBiomassCenterDay;"<<grazerDynamics.planktivore_biomass_center_day<<endl;
		parameterFileStream<<"PlanktivoreBiomassCenterNight;"<<grazerDynamics.planktivore_biomass_center_night<<endl;
		parameterFileStream<<"PlanktivoreBiomassWidth;"<<grazerDynamics.planktivore_biomass_width<<endl;
		parameterFileStream<<"GrazerPredationIndex;"<<grazerDynamics.predation_index<<endl;
		parameterFileStream<<"MaxVerticalMigration;"<<grazerDynamics.max_vertical_migration<<endl;
		parameterFileStream<<"MaxHorizontalMigration;"<<grazerDynamics.max_horizontal_migration<<endl;
		parameterFileStream<<"MaxSearchSteps;"<<grazerDynamics.max_search_steps<<endl;
		parameterFileStream<<"RandomWalkProbabilityWeight;"<<grazerDynamics.random_walk_probability_weight<<endl;
		parameterFileStream<<"GrazerMinimumPredationSafety;"<<grazerDynamics.minimum_predation_safety<<endl;
		parameterFileStream<<"GrazerMaximumLightTolerated;"<<grazerDynamics.maximum_light_tolerated<<endl;
		parameterFileStream<<"GrazerLightSafetyWeight;"<<grazerDynamics.light_safety_weight<<endl;
		parameterFileStream<<"GrazerLightSafetyThreshold;"<<grazerDynamics.light_safety_threshold<<endl;
		parameterFileStream<<"GrazerCohortSplittingLimit;"<<grazerDynamics.cohort_splitting_limit<<endl;
		parameterFileStream<<"GrazerCriticalDepth;"<<grazerDynamics.critical_depth<<endl;
		parameterFileStream<<"GrazerMinimumTolerableLight;"<<grazerDynamics.minimum_tolerable_light<<endl;
#ifdef ADD_DEAD_BIOMASS_NUTRIENTS
		parameterFileStream<<"GrazerReabsorbedDeadNutrientsProportion;"<<grazerDynamics.reabsorbed_animal_nutrients_proportion<<endl;
#endif
		parameterFileStream<<"GrazerRandomSeed;"<<this->random_seed<<endl;
		parameterFileStream<<"AlgalCarryingCapacityCoefficient;"<<this->algal_carrying_capacity_coefficient<<endl;
		parameterFileStream<<"AlgalCarryingCapacityIntercept;"<<this->algal_carrying_capacity_intercept<<endl;
		parameterFileStream<<"AlgalMaximumFoundBiomass;"<<this->maximum_found_algal_biomass<<endl;
#ifdef ADD_DEAD_BIOMASS_NUTRIENTS
		parameterFileStream<<"AlgalReabsorbedDeadNutrientsProportion;"<<this->reabsorbed_algal_nutrients_proportion<<endl;
#endif
		parameterFileStream<<"AlgalMortalityScale;"<<this->algal_mortality_scale<<endl;
		parameterFileStream<<"AlgalIntrinsicSettlingRate;"<<this->intrinsic_settling_rate<<endl;
		parameterFileStream<<"AlgalFractionSloughed"<<this->algal_fraction_sloughed<<endl;
		parameterFileStream<<"PhosphorusHalfSaturation;"<<this->phosphorus_half_saturation<<endl;
		parameterFileStream<<"PhosphorusFunctionalFactor;"<<this->phosphorus_functional_factor<<endl;
		parameterFileStream<<"PhosphorusFunctionalConstantResponse1;"<<this->phosphorus_functional_constant_response_1<<endl;
		parameterFileStream<<"PhosphorusFunctionalConstantResponse2;"<<this->phosphorus_functional_constant_response_2<<endl;
		parameterFileStream<<"PhosphorusFunctionalStep1;"<<this->phosphorus_functional_step_1<<endl;
		parameterFileStream<<"NitrogenHalfSaturation;"<<this->nitrogen_half_saturation<<endl;
		parameterFileStream<<"NitrogenFunctionalConstantResponse;"<<this->nitrogen_functional_constant_response<<endl;
		parameterFileStream<<"NitrogenFunctionalStep;"<<this->nitrogen_functional_step<<endl;
		parameterFileStream<<"NitrogenPhosphorusLowerFactor;"<<this->nitrogen_phosphorus_lower_bound<<endl;
		parameterFileStream<<"NitrogenPhosphorusUpperFactor;"<<this->nitrogen_phosphorus_upper_bound<<endl;
		parameterFileStream<<"TemperatureOptimal;"<<this->temperature_optimal<<endl;
		parameterFileStream<<"TemperatureSteepness;"<<this->temperature_steepness<<endl;
		parameterFileStream<<"TemperatureSuppressionSteepness;"<<this->temperature_suppression_steepness<<endl;
		parameterFileStream<<"TemperatureMax;"<<this->temperature_max<<endl;
		parameterFileStream<<"LightAllowanceWeight;"<<this->light_allowance_weight<<endl;
		parameterFileStream<<"LightAllowanceProportion;"<<this->light_allowance_proportion<<endl;
		parameterFileStream<<"Respiration20Degrees;"<<this->algal_respiration_at_20_degrees<<endl;
		parameterFileStream<<"AlgalRespirationExponentialTemperatureCoefficient;"<<this->exponential_temperature_algal_respiration_coefficient<<endl;
		parameterFileStream<<"MaximumAlgaeDeathResources;"<<this->maximum_algae_resources_death<<endl;
		parameterFileStream<<"LightSteepness;"<<this->light_steepness<<endl;
		parameterFileStream<<"IntrinsicAlgaeMortalityRate;"<<this->intrinsic_algae_mortality_rate<<endl;
		parameterFileStream<<"LimitationScaleWeight;"<<this->limitation_scale_weight<<endl;
		parameterFileStream<<"WashUpDeadBiomass;"<<this->wash_up_dead_biomass_proportion<<endl;
		parameterFileStream<<"WashDownDeadBiomass;"<<this->wash_down_dead_biomass_proportion<<endl;
		parameterFileStream<<"AlgaeBiomassConservationFactor;"<<this->wash_down_dead_biomass_proportion<<endl;
		parameterFileStream.close();
	} else {
		cerr<<"File "<<parameterSimulationRoute<<" could not be opened for simulation parameter register."<<endl;
	}
}


void FoodWebModel::FoodWebModel::openSimulationFiles(const SimulationArguments& simArguments){
	outputInitialAlgaeFile.open(simArguments.outputAlgaeRoute.c_str());
	outputGrazerFile.open(simArguments.outputGrazerRoute.c_str());
#ifdef INDIVIDUAL_BASED_ANIMALS
	grazerTraceFile.open(simArguments.outputGrazerTraceRoute.c_str());
	predatorTraceFile.open(simArguments.outputPredatorTraceRoute.c_str());
#endif
	outputAlgaeFile.open(simArguments.outputSloughRoute.c_str());
	outputPredatorFile.open(simArguments.outputPredatorRoute.c_str());
	outputPhysicalFile.open(simArguments.outputPhysicalRoute.c_str());
#ifdef CHECK_ASSERTIONS
	outputAssertionViolationFile.open(simArguments.outputAssertionViolationRoute.c_str());
#endif
	/*Report successfully or not successfully open files*/
	if (outputInitialAlgaeFile.is_open()&&!outputInitialAlgaeFile.fail()){
		cout<<"File "<<simArguments.outputAlgaeRoute<<" open for algae biomass output."<<endl;
	} else{
		cout<<"File "<<simArguments.outputAlgaeRoute<<" could not be opened for algae biomass output."<<endl;
	}
	if (outputAlgaeFile.is_open()){
		cout<<"File "<<simArguments.outputSloughRoute<<" open for slough register."<<endl;
	} else{
		cout<<"File "<<simArguments.outputSloughRoute<<" could not be opened for slough register."<<endl;
	}
	if (outputGrazerFile.is_open()){
		cout<<"File "<<simArguments.outputGrazerRoute<<" open for grazer register."<<endl;
	} else{
		cout<<"File "<<simArguments.outputGrazerRoute<<" could not be opened for grazer register."<<endl;
	}
	if (outputPredatorFile.is_open()){
		cout<<"File "<<simArguments.outputPredatorRoute<<" open for predator register."<<endl;
	} else{
		cout<<"File "<<simArguments.outputPredatorRoute<<" could not be opened for predator register."<<endl;
	}
#ifdef INDIVIDUAL_BASED_ANIMALS
	if (grazerTraceFile.is_open()){
			cout<<"File "<<simArguments.outputGrazerTraceRoute<<" open for grazer trace register."<<endl;
		} else{
			cout<<"File "<<simArguments.outputGrazerTraceRoute<<" could not be opened for grazer trace register."<<endl;
		}
	if (predatorTraceFile.is_open()){
			cout<<"File "<<simArguments.outputPredatorTraceRoute<<" open for predator trace register."<<endl;
		} else{
			cout<<"File "<<simArguments.outputPredatorTraceRoute<<" could not be opened for predator trace register."<<endl;
		}
#endif
	if (outputPhysicalFile.is_open()){
		cout<<"File "<<simArguments.outputPhysicalRoute<<" open for physical variables."<<endl;
	} else{
		cout<<"File "<<simArguments.outputPhysicalRoute<<" could not be opened for physical variables."<<endl;
	}
#ifdef CHECK_ASSERTIONS
	if (outputAssertionViolationFile.is_open()){
		cout<<"File "<<simArguments.outputAssertionViolationRoute<<" open for violated assertions register."<<endl;
	} else{
		cout<<"File "<<simArguments.outputAssertionViolationRoute<<" could not be opened for violated assertions register."<<endl;
	}

#endif

}

void FoodWebModel::FoodWebModel::closeSimulationFiles(){
	outputInitialAlgaeFile.close();
	outputAlgaeFile.close();
	outputGrazerFile.close();
	outputPredatorFile.close();
#ifdef INDIVIDUAL_BASED_ANIMALS
	grazerTraceFile.close();
	predatorTraceFile.close();
#endif
	outputPhysicalFile.close();
#ifdef CHECK_ASSERTIONS
	outputAssertionViolationFile.close();
#endif
}
void FoodWebModel::FoodWebModel::step(){
	updateRegisterVariables();
	updatePhysicalState();
#ifdef ADD_DEAD_BIOMASS_NUTRIENTS
	updateDeadBiomass();
#endif
	updateAlgaeBiomass();
	updateAlgaeVerticalMigration();
	grazerDynamics.takeAnimalDynamicsStep();
//	predatorDynamics.updateAnimalBiomass();
}

void FoodWebModel::FoodWebModel::updateRegisterVariables(){
	for(int columnIndex=0; columnIndex<MAX_COLUMN_INDEX; columnIndex++){
		for(int depthIndex=0; depthIndex<MAX_DEPTH_INDEX; depthIndex++){
			registerBiomass[depthIndex][columnIndex]=depthIndex<=maxDepthIndex[columnIndex]&&
					current_hour%TIME_OUTPUT_RESOLUTION==0;
		}
	}

}


void FoodWebModel::FoodWebModel::updatePhysicalState(){
	photoPeriod();
	calculateLightAtTop();
	//useSeasonalLight();

	for(int depthIndex=0; depthIndex<MAX_DEPTH_INDEX; depthIndex++){
		for(int columnIndex=0; columnIndex<MAX_COLUMN_INDEX; columnIndex++){
			if(depthIndex<=maxDepthIndex[columnIndex]){
				saltConcentrationAtDepth(depthIndex, columnIndex);
				physicalType salinity_at_depth_exponent=this->chemical_at_depth_exponent, salinity_concentration = this->chemical_concentration;
				salinityEffect(depthIndex, columnIndex, salinity_concentration);
				phosphorusConcentrationAtDepth(depthIndex, columnIndex);
				nitrogenConcentrationAtDepth(depthIndex, columnIndex);
				physicalType phosphorus_at_depth_exponent=this->chemical_at_depth_exponent;
				calculateTemperature(depthIndex, columnIndex);
#ifdef EXTENDED_OUTPUT
				if(registerBiomass[depthIndex][columnIndex]){
					outputPhysicalFile<<depthIndex<<commaString<<columnIndex<<commaString<<current_hour<<commaString<<0<<commaString<<temperature[depthIndex][columnIndex]<<commaString<<temperature_angular_frequency<<commaString<<temperature_sine<<commaString<<salinity_at_depth_exponent<<commaString<<salinity_concentration<<commaString<<salinity_effect_matrix[depthIndex][columnIndex]<<commaString<<salinity_exponent<<commaString<<phosphorus_at_depth_exponent<<commaString<<phosphorus_concentration[depthIndex][columnIndex]<<commaString<<current_phosphorus_concentration_at_bottom<<commaString<<light_at_top<<endl;
				}
#endif
			}
		}

	}
	for(int columnIndex=0; columnIndex<MAX_COLUMN_INDEX; columnIndex++){
		saltConcentrationAtDepth(maxDepthIndex[columnIndex], columnIndex);
		physicalType salinity_at_depth_exponent=this->chemical_at_depth_exponent, salinity_concentration = this->chemical_concentration;
		salinityEffect(maxDepthIndex[columnIndex], columnIndex, salinity_concentration);
		phosphorusConcentrationAtDepth(maxDepthIndex[columnIndex], columnIndex);
		physicalType phosphorus_at_depth_exponent=this->chemical_at_depth_exponent;

		calculateTemperature(maxDepthIndex[columnIndex], columnIndex);
		if(registerBiomass[0][columnIndex]){
			outputPhysicalFile<<maxDepthIndex[columnIndex]<<commaString<<columnIndex<<commaString<<current_hour<<commaString<<1<<commaString<<temperature[maxDepthIndex[columnIndex]][columnIndex]<<commaString<<temperature_angular_frequency<<commaString<<temperature_sine<<commaString<<salinity_at_depth_exponent<<commaString<<salinity_concentration<<commaString<<salinity_effect_matrix[maxDepthIndex[columnIndex]][columnIndex]<<commaString<<salinity_exponent<<commaString<<phosphorus_at_depth_exponent<<commaString<<phosphorus_concentration[maxDepthIndex[columnIndex]][columnIndex]<<commaString<<current_phosphorus_concentration_at_bottom<<commaString<<light_at_top;
		}

	}
}

void FoodWebModel::FoodWebModel::updateAlgaeBiomass(){
// All algae biomass must be a well-formed number greater than 0
#ifdef CHECK_ASSERTIONS
	for(int columnIndex=0; columnIndex<MAX_COLUMN_INDEX; columnIndex++){
		if(isnan((float)periBiomass[columnIndex])||isinf((float)periBiomass[columnIndex])){
			assertionViolationBuffer<<"NanInfPeri; Peri: "<<periBiomass[columnIndex]<<", Column: "<<columnIndex<<", Time: "<<current_hour<<endl;
		}
		if(periBiomass[columnIndex]<0.0f){
			assertionViolationBuffer<<"NegativePeri; Peri: "<<periBiomass[columnIndex]<<", Column: "<<columnIndex<<", Time: "<<current_hour<<endl;
		}
		for(int depthIndex=maxDepthIndex[columnIndex]; depthIndex>=0; depthIndex--){
			if(isnan((float)phytoBiomass[depthIndex][columnIndex])||isinf((float)phytoBiomass[depthIndex][columnIndex])){
				assertionViolationBuffer<<"NanInfPhyto: "<<phytoBiomass[depthIndex][columnIndex]<<", Depth: "<<depthIndex<<", Column: "<<columnIndex<<", Time: "<<current_hour<<endl;
			}
			if(phytoBiomass[depthIndex][columnIndex]<0.0f){
				assertionViolationBuffer<<"NegativePhyto: "<<phytoBiomass[depthIndex][columnIndex]<<", Depth: "<<depthIndex<<", Column: "<<columnIndex<<", Time: "<<current_hour<<endl;
			}
		}
	}
#endif
	/*Use different algae biomass differential weights for burn-in and production*/
//	algae_biomass_differential_scale=current_hour<=BURNIN_MAX_CYCLE?ALGAE_BIOMASS_DIFFERENTIAL_BURNIN_SCALE:this->algae_biomass_differential_production_scale;
	algae_biomass_differential_scale=algae_biomass_differential_production_scale;
	initialAlgaeBuffer.str("");
	algaeBuffer.str("");
	/* Clear vertical migrated phyto biomass*/
	for(int columnIndex=0; columnIndex<MAX_COLUMN_INDEX; columnIndex++){
		verticalGainedPeriBiomass[columnIndex]=periBiomassDifferential[columnIndex]=0.0f;
		for(int depthIndex=0; depthIndex<MAX_DEPTH_INDEX; depthIndex++){
			verticalGainedPhytoBiomass[depthIndex][columnIndex]=verticalLostPhytoBiomass[depthIndex][columnIndex]=phytoBiomassDifferential[depthIndex][columnIndex]=0.0f;
		}
	}

	/*Clear vertical migration biomass*/
	/*Calculate phytoplankton and periphyton biomass on the current step*/
	for(int columnIndex=0; columnIndex<MAX_COLUMN_INDEX; columnIndex++){
		lineBuffer.str("");
		lineBuffer.clear();
		/*Register previous biomass*/
		/*Update biomass and output new biomass*/
		biomassType previousPeriBiomass=periBiomass[columnIndex];
		periBiomass[columnIndex]*=this->algae_biomass_conservation_factor;
		periBiomassDifferential[columnIndex] = algaeBiomassDifferential(maxDepthIndex[columnIndex], columnIndex, true);
		periBiomassDifferential[columnIndex]+=algae_natural_mortality*this->algal_mortality_scale/(this->limitation_product_matrix[maxDepthIndex[columnIndex]][columnIndex]/light_at_top);

#ifdef CHECK_ASSERTIONS
		if(isnan((float)periBiomassDifferential[columnIndex])||isinf((float)periBiomassDifferential[columnIndex])){
			assertionViolationBuffer<<"NanInfPeriDiff; Peri: "<<periBiomassDifferential[columnIndex]<<", Column: "<<columnIndex<<", Time: "<<current_hour<<endl;
		}
#endif
		/* Save periphyton biomass for later registering in file*/
		std::ostringstream copyBuffer;
		copyBuffer.str("");
		copyBuffer.clear();
		copyBuffer<<lineBuffer.str();
		for(int depthIndex=0; depthIndex<MAX_DEPTH_INDEX; depthIndex++){
			lineBuffer.str("");
			lineBuffer.clear();
			/* Do not register biomass for empty cells. Since this is the bottom of the lake, do not register biomass below it*/
			if(depthIndex>maxDepthIndex[columnIndex]){
				phytoBiomass[depthIndex][columnIndex]=0.0f;
				break;
			} else{
				/*Set if biomass must be registered*/
				/*Update biomass and output new biomass*/

				biomassType previousPhytoBiomass=phytoBiomass[depthIndex][columnIndex];
				phytoBiomass[depthIndex][columnIndex]*=this->algae_biomass_conservation_factor;
				phytoBiomassDifferential[depthIndex][columnIndex] = algaeBiomassDifferential(depthIndex, columnIndex, false);
				phytoBiomassDifferential[depthIndex][columnIndex]+=algae_natural_mortality*this->algal_mortality_scale/(this->limitation_product_matrix[depthIndex][columnIndex]/light_at_top);
//				phytoBiomass[depthIndex][columnIndex]=min((double)this->maximum_found_algal_biomass,phytoBiomass[depthIndex][columnIndex]);
				//phytoBiomass[depthIndex][columnIndex]-=phytoBiomass[depthIndex][columnIndex]/this->maximum_found_algal_biomass;

				/*Apply carrying capacity*/
				//calculateAlgalCarryingCapacityMortality(phytoBiomass[depthIndex][columnIndex]);
				//phytoBiomass[depthIndex][columnIndex]=this->maximum_found_algal_biomass*used_algal_mortality_rate;
				#ifdef CHECK_ASSERTIONS
				if(isnan((float)phytoBiomass[depthIndex][columnIndex])||isinf((float)phytoBiomass[depthIndex][columnIndex])){
					assertionViolationBuffer<<"NanInfPhyto: "<<phytoBiomass[depthIndex][columnIndex]<<", Depth: "<<depthIndex<<", Column: "<<columnIndex<<", Time: "<<current_hour<<endl;
				}
				if(phytoBiomass[depthIndex][columnIndex]<0.0f){
					assertionViolationBuffer<<"NegativePhyto: "<<phytoBiomass[depthIndex][columnIndex]<<", Depth: "<<depthIndex<<", Column: "<<columnIndex<<", Time: "<<current_hour<<endl;
				}
				if(phytoBiomass[depthIndex][columnIndex]!=0.0f&&(phytoBiomass[depthIndex][columnIndex]!=previousPhytoBiomass+phytoBiomassDifferential[depthIndex][columnIndex])){
					assertionViolationBuffer<<"UpdatePhyto; Phyto: "<<phytoBiomass[depthIndex][columnIndex]<<", PreviousPhyto: "<<previousPhytoBiomass<<", PhytoDiff: "<<phytoBiomassDifferential[depthIndex][columnIndex]<<", PhytoError: "<<phytoBiomass[depthIndex][columnIndex]-(previousPhytoBiomass+phytoBiomassDifferential[depthIndex][columnIndex])<<", Depth: "<<depthIndex<<", Column: "<<columnIndex<<", Time: "<<current_hour<<endl;
				}
#endif

				lineBuffer<<commaString<<verticalGainedPhytoBiomass[depthIndex][columnIndex]<<endl;
				/*If biomass must be registered, register standard phytoplankton biomass*/
				if(registerBiomass[depthIndex][columnIndex]){
					initialAlgaeBuffer<<lineBuffer.str();
					//cout<<"Biomass: "<<phytoBiomass[depthIndex][columnIndex]<<", Differential: "<<phytoBiomassDifferential[depthIndex][columnIndex]<<"."<<endl;
				}

			}
		/* Retrieve line registering periphyton biomass*/
		}
		lineBuffer.str("");
		lineBuffer.clear();
		lineBuffer<<copyBuffer.str();
		//periBiomass[columnIndex]-=periBiomass[columnIndex]/this->maximum_found_algal_biomass;
		//periBiomass[columnIndex]=min((double)this->maximum_found_algal_biomass,periBiomass[columnIndex]);

		/*Apply carrying capacity*/
		//calculateAlgalCarryingCapacityMortality(periBiomass[columnIndex]);
		//periBiomass[columnIndex]=this->maximum_found_algal_biomass*used_algal_mortality_rate;
#ifdef CHECK_ASSERTIONS
		if(isnan((float)periBiomass[columnIndex])||isinf((float)periBiomass[columnIndex])){
			assertionViolationBuffer<<"NanInfPeri; Peri: "<<periBiomass[columnIndex]<<", Column: "<<columnIndex<<", Time: "<<current_hour<<endl;
		}
		if(periBiomass[columnIndex]<0.0f){
			assertionViolationBuffer<<"NegativePeri; Peri: "<<periBiomass[columnIndex]<<", Column: "<<columnIndex<<", Time: "<<current_hour<<endl;
		}
		if(periBiomass[columnIndex]!=0.0f&&(periBiomass[columnIndex]!=previousPeriBiomass+periBiomassDifferential[columnIndex])){
			assertionViolationBuffer<<"UpdatePeri; Peri: "<<periBiomass[columnIndex]<<", PreviousPeri: "<<previousPeriBiomass<<", PeriDiff: "<<periBiomassDifferential[columnIndex]<<", PeriError: "<<periBiomass[columnIndex]-(previousPeriBiomass+periBiomassDifferential[columnIndex])<<", Column: "<<columnIndex<<", Time: "<<current_hour<<endl;
		}
#endif
		lineBuffer<<commaString<<verticalGainedPeriBiomass[columnIndex]<<endl;
		/*If biomass must be registered, register standard and slough periphyton biomass*/

	}
	/*Add slough biomass*/
#ifdef CHECK_ASSERTIONS
	for(int columnIndex=0; columnIndex<MAX_COLUMN_INDEX; columnIndex++){
		for(int depthIndex=0; depthIndex<MAX_DEPTH_INDEX; depthIndex++){

				if(isnan((float)phytoBiomass[depthIndex][columnIndex])||isinf((float)phytoBiomass[depthIndex][columnIndex])){
					assertionViolationBuffer<<"NanInfPhyto: "<<phytoBiomass[depthIndex][columnIndex]<<", Depth: "<<depthIndex<<", Column: "<<columnIndex<<", Time: "<<current_hour<<endl;
				}
				if(phytoBiomass[depthIndex][columnIndex]<0.0f){
					assertionViolationBuffer<<"NegativePhyto: "<<phytoBiomass[depthIndex][columnIndex]<<", Depth: "<<depthIndex<<", Column: "<<columnIndex<<", Time: "<<current_hour<<endl;
				}




		}
	}
#endif

}

/*
 * Differential of biomass for photoplankton and periphyton at each time step
 */

biomassType FoodWebModel::FoodWebModel::algaeBiomassDifferential(int depthIndex, int columnIndex, bool periPhyton){

	/*Calculate temporal and spatially local values that will be used to calculate biomass differential*/
	biomassType localPointBiomass=periPhyton?periBiomass[columnIndex]:phytoBiomass[depthIndex][columnIndex];

	/* Calculate nutrient limitations*/

	lightAtDepth(depthIndex, columnIndex);
	normalizeLight(columnIndex);
	lightAllowance(localPointBiomass);
	physicalType limitationRatio = (phosphorus_concentration[depthIndex][columnIndex]/nitrogen_concentration[depthIndex][columnIndex]);
	physicalType phosphorusLimitation = calculateNutrientLimitation(localPointBiomass, phosphorus_concentration[depthIndex][columnIndex], this->phosphorus_functional_step_1, this->phosphorus_half_saturation, this->phosphorus_functional_constant_response_2);
	physicalType nitrogenLimitation = calculateNutrientLimitation(localPointBiomass, nitrogen_concentration[depthIndex][columnIndex], this->nitrogen_functional_step, this->nitrogen_half_saturation, this->nitrogen_functional_constant_response);
	calculateTemperatureLimitation(depthIndex, columnIndex);
	/* Calculate the limiting factor ([1] R. W. Sterner, “On the Phosphorus Limitation Paradigm for Lakes,” Internat. Rev. Hydrobiol, vol. 93, pp. 4–5, 2008.)*/
	if(limitationRatio > this->nitrogen_phosphorus_upper_bound){
		nutrient_limitation = phosphorusLimitation;
	} else{
		if(limitationRatio < this->nitrogen_phosphorus_lower_bound){
			nutrient_limitation = nitrogenLimitation;
		} else{
			nutrient_limitation=min(phosphorusLimitation, nitrogenLimitation);
		}
	}
#ifdef ADD_DEAD_BIOMASS_NUTRIENTS
	/* Add nutrients from biomass dead in previous hours*/
	biomassType previousDeadBiomass=periPhyton?previousDeadBottomBiomass[columnIndex]:previousDeadFloatingBiomass[depthIndex][columnIndex];
	physicalType phosphorusFactorAddition=this->decaying_phosphorus_factor*previousDeadBiomass;

	/* Remove nutrients from dead biomass that have been absorbed*/
	if(periPhyton){
		previousDeadBottomBiomass[columnIndex]-=phosphorusFactorAddition;
		previousDeadBottomBiomass[columnIndex]=max<biomassType>(previousDeadBottomBiomass[columnIndex],0.0f);
	} else{
		previousDeadFloatingBiomass[depthIndex][columnIndex]-=phosphorusFactorAddition;
		previousDeadFloatingBiomass[depthIndex][columnIndex]=max<biomassType>(previousDeadFloatingBiomass[depthIndex][columnIndex],0.0f);

	}
	if(phosphorusFactorAddition<0){
		cout<<"Phosphorus addition: "<<phosphorusFactorAddition<<" is lower than 0."<<endl;
	}
	nutrient_limitation+=phosphorusFactorAddition;
#endif
	light_allowance_matrix[depthIndex][columnIndex]=light_allowance;
	nutrient_limitation_matrix[depthIndex][columnIndex]=nutrient_limitation;
#ifdef LIMITATION_MINIMUM
	physicalType localeLimitationProduct = light_at_top*min<physicalType>(light_allowance*(light_allowance_proportion),nutrient_limitation*(1-light_allowance_proportion));

	limiting_factor[depthIndex][columnIndex]=localeLimitationProduct==light_at_top*light_allowance*(light_allowance_proportion)?1:0;
#elif defined(WEIGHTED_RESOURCE_LIMITATION)
	physicalType localeLimitationProduct = light_allowance*light_allowance_proportion+nutrient_limitation*(1-light_allowance_proportion);
	limiting_factor[depthIndex][columnIndex]=light_allowance<nutrient_limitation?1:0;
#else
	physicalType localeLimitationProduct = light_at_top*light_allowance*nutrient_limitation*temperature_limitation;
	limiting_factor[depthIndex][columnIndex]=light_allowance<nutrient_limitation?1:0;
#endif

	/* Calculate biomass differential components*/
	biomassType unweightedLocaleLimitationProduct=localeLimitationProduct;
	localeLimitationProduct*=this->limitation_scale_weight;
	limitation_product_matrix[depthIndex][columnIndex]=localeLimitationProduct;
#ifdef CHECK_LOST_BIOMASS_ADDITION
	if(unweightedLocaleLimitationProduct>1.0f){
		cout<<"Unweighted locale limitation product: "<<unweightedLocaleLimitationProduct<<" is greater than 0."<<endl;
	}
#endif

	photoSynthesis(localPointBiomass, localeLimitationProduct, periPhyton);


    /**
     * Temperature at surface 25o
     * Temperature at bottom 2o
     * And thermocline is about 8.25m
     * Model as a reversed sigmoid
     *
     */

	physicalType localeTemperature =temperature[depthIndex][columnIndex];
	algaeRespiration(localPointBiomass, localeTemperature);


	algaeExcretion();
	/*Register photosynthesis*/
	if(periPhyton){
		photosynthesis_peri_matrix[columnIndex]=photosynthesis_value;

	} else{
		photosynthesis_phyto_matrix[depthIndex][columnIndex]=photosynthesis_value;

	}

	/*Register respiration*/
	if(periPhyton){
		respiration_peri_matrix[columnIndex]=-algae_respiration_value;

	} else{
		respiration_phyto_matrix[depthIndex][columnIndex]=-algae_respiration_value;

	}

	/*Register excretion*/
	if(periPhyton){
		excretion_peri_matrix[columnIndex]=-algae_excretion_value;

	} else{
		excretion_phyto_matrix[depthIndex][columnIndex]=-algae_excretion_value;

	}

	/*Formula of biomass differential (AquaTox Documentation, page 67, equations 33 and 34)*/
	biomassType local_algal_mortality_factor = algaeNaturalMortality(localeTemperature, unweightedLocaleLimitationProduct, localPointBiomass);
	local_algal_mortality_factor*=this->algae_biomass_differential_scale;
	if(periPhyton){
		this->periMortalityMatrix[columnIndex] = local_algal_mortality_factor;
	} else{
		this->phytoMortalityMatrix[depthIndex][columnIndex] = local_algal_mortality_factor;
	}
	#ifdef ADD_DEAD_BIOMASS_NUTRIENTS
	/* Add dead nutrients*/
	biomassType reabsorbedNutrients = this->reabsorbed_algal_nutrients_proportion*-algae_natural_mortality;
	if(deadFloatingBiomass[depthIndex][columnIndex]<0.0f){
		cout<<"Lower than 0 dead biomass: "<<deadFloatingBiomass[depthIndex][columnIndex]<<"."<<endl;
	}
	if(periPhyton){
		deadBottomBiomass[columnIndex]+= reabsorbedNutrients;
	} else{
		deadFloatingBiomass[depthIndex][columnIndex]+= reabsorbedNutrients;
	}
	if(deadFloatingBiomass[depthIndex][columnIndex]<0.0f){
		cout<<"Lower than 0 dead biomass: "<<deadFloatingBiomass[depthIndex][columnIndex]<<"."<<endl;
	}
#endif
	/*Add algae constant differential if grazers are present*/
	biomassType constantBiomassDifferential=0.0f;
#ifdef ADD_CONSTANT_BIOMASS_DIFFERENTIAL
	if(depthIndex>=GRAZERS_LIMIT){
		constantBiomassDifferential = CONSTANT_ALGAE_BIOMASS_DIFFERENTIAL;
	}
#elif defined(ADD_VARIABLE_BIOMASS_DIFFERENTIAL)
	constantBiomassDifferential = this->baseAlgaeBiomassDifferential[depthIndex]*VARIABLE_ALGAE_BIOMASS_DIFFERENTIAL_WEIGHT;
#endif
	biomassType localeConstantBiomassDifferential = localPointBiomass>0.0f?constantBiomassDifferential:0.0f;
	algae_respiration_value=algae_excretion_value=0.0f;
	biomassType totalAlgaeBiomassDifferential =  localeConstantBiomassDifferential + photosynthesis_value+algae_respiration_value+algae_excretion_value;
//			+algae_natural_mortality*10.0f;
	/* Register differential*/
//	#ifndef STABLE_CHLOROPHYLL
//		if(periPhyton){
//			periDifferential[columnIndex]=totalAlgaeBiomassDifferential/localPointBiomass;
//		} else {
//			phytoDifferential[depthIndex][columnIndex]=totalAlgaeBiomassDifferential/localPointBiomass;
//		}
//	#else
//		if(current_hour<=STABLE_STATE_HOUR){
//			if(periPhyton){
//				periDifferential[columnIndex]=totalProductivity/localPointBiomass;
//			} else {
//				phytoDifferential[depthIndex][columnIndex]=totalProductivity/localPointBiomass;
//			}
//		}
//	#endif
//
//	/* Create line buffer to write algae biomass*/
//	lineBuffer.str("");
//	lineBuffer.clear();
//	lineBuffer<<depthIndex;
//	lineBuffer<<commaString<<columnIndex;
//	lineBuffer<<commaString<<current_hour;
//	lineBuffer<<commaString<<periPhyton?1:0;
//	lineBuffer<<commaString<<depthInMeters;
//	lineBuffer<<commaString<<light_allowance;
//	lineBuffer<<commaString<<turbidity_at_depth;
//	lineBuffer<<commaString<<light_at_depth;
//	lineBuffer<<commaString<<localeLimitationProduct;
//	lineBuffer<<commaString<<light_difference;
//	lineBuffer<<commaString<<normalized_light_difference;
//	lineBuffer<<commaString<<resource_limitation_exponent;
//	lineBuffer<<commaString<<algae_biomass_to_depth;
//	lineBuffer<<commaString<<photosynthesis_value;
//	lineBuffer<<commaString<<algae_respiration_value;
//	lineBuffer<<commaString<<algae_excretion_value;
//	lineBuffer<<commaString<<algae_natural_mortality;
//	lineBuffer<<commaString<<sedimentation_rate;
//	lineBuffer<<commaString<<algae_sinking_value;
//	lineBuffer<<commaString<<algae_slough_value;
//	lineBuffer<<commaString<<high_temperature_mortality;
//	lineBuffer<<commaString<<resource_limitation_stress;
//	lineBuffer<<commaString<<weighted_resource_limitation_stress;
//	lineBuffer<<commaString<<algae_natural_mortality_factor;
//	lineBuffer<<commaString<<used_algal_mortality_rate;
//	#ifdef STABLE_CHLOROPHYLL
//		if(current_hour>=STABLE_STATE_HOUR){
//			if(periPhyton){
//				totalProductivity= periDifferential[columnIndex]*periBiomass[columnIndex];
//
//			} else{
//				totalProductivity= phytoDifferential[depthIndex][columnIndex]*phytoBiomass[depthIndex][columnIndex];
//			}
//		}
//	#endif
	return totalAlgaeBiomassDifferential;
}


/*
 * Light intensity is calculated using the simple model from Ryabov, 2012, Phytoplankton competition in deep biomass maximum, Theoretical Ecology, equation 3
 */
void FoodWebModel::FoodWebModel::lightAtDepth(int depthIndex, int columnIndex){
	/*
	 * Transform depth from an integer index to a real value in ms
	 */
	depthInMeters= indexToDepth[depthIndex];
	algae_biomass_to_depth = this->diatom_attenuation_coefficient*sumPhytoBiomassToDepth(depthIndex, columnIndex)*MICROGRAM_TO_GRAM*M3_TO_LITER;
	turbidity_at_depth=TURBIDITY*depthInMeters;
	light_at_depth_exponent = -(turbidity_at_depth+algae_biomass_to_depth);
#ifdef ADDITIVE_TURBIDITY
	light_at_depth =  light_at_top*ALGAE_ATTENUATION_WEIGHT*(ALGAE_ATTENUATION_PROPORTION*exp(-algae_biomass_to_depth)+TURBIDITY_PROPORTION*exp(-turbidity_at_depth));

#else
	/* Adjust light steepness according to light at top*/
	physicalType quantileProduct=1.0f;
	if(light_at_top>=light_upper_quantile){
		quantileProduct=1.0f/light_steepness_factor;
	}
	if(light_at_top<=light_lower_quantile){
		quantileProduct=light_steepness_factor;
	}
	light_at_depth =  light_at_top*exp(quantileProduct*this->light_steepness*light_at_depth_exponent);

#endif
	previousLakeLightAtDepth[depthIndex][columnIndex] = lakeLightAtDepth[depthIndex][columnIndex];
	lakeLightAtDepth[depthIndex][columnIndex] = light_at_depth;
}


/*
 * Summing of phytoplankton biomass up to the given depth
 */
biomassType FoodWebModel::FoodWebModel::sumPhytoBiomassToDepth(int depthIndex, int columnIndex){
	biomassType sumPhytoBiomass=0.0l;
	for(int i=0; i<depthIndex; i++){
		sumPhytoBiomass+=phytoBiomass[i][columnIndex];
	}
#ifdef CHECK_ASSERTIONS
	if(isnan((float)sumPhytoBiomass)||isinf((float)sumPhytoBiomass)){
		assertionViolationBuffer<<"NanInfPhytoSum; PhytoSum: "<<sumPhytoBiomass<<", Depth: "<<depthIndex<<", Column: "<<columnIndex<<", Time: "<<current_hour<<endl;
	}
	if(sumPhytoBiomass<0.0f){
		assertionViolationBuffer<<"NegativePhytoSum; PhytoSum: "<<sumPhytoBiomass<<", Depth: "<<depthIndex<<", Column: "<<columnIndex<<", Time: "<<current_hour<<endl;
	}
#endif
	return sumPhytoBiomass;
}


/*
 * Photosynthesis to calculate biomass differential
 */
void FoodWebModel::FoodWebModel::photoSynthesis(biomassType localPointBiomass, physicalType localeLimitationProduct, bool periPhyton){

	photosynthesis_value =  max<biomassType>(0.0002f,productionLimit(localeLimitationProduct, periPhyton)*localPointBiomass*PHOTOSYNTHESIS_FACTOR);
}

physicalType FoodWebModel::FoodWebModel::productionLimit(physicalType localeLimitationProduct, bool periPhyton){
	/*The main limiting effects are temperature, nutrient concentration, toxicant and light. Since we will not have data
	 * on nutrient concentration and the temperature model is complex, we will use only light intensity as a limiting effect*/
	/*
	 * We will also use the fraction of littoral zone for periphyton
	 */
	physicalType productionLimit= localeLimitationProduct;
	if(periPhyton){
		productionLimit*=fractionInEuphoticZone;
	}
	return productionLimit;
}


/* Initialize vectors of parameters based on read data*/
void FoodWebModel::FoodWebModel::initializeParameters(){
	/* Copy the data to arrays inside the class */
	/* Copy age class expected weight*/
	for(int i=0; i<MAX_CLASS_INDEX; i++){
		grazerDynamics.initial_grazer_weight[i]=readProcessedData.initial_grazer_weight[i];
	}

	/* Copy depth vector */
	for(int i=0; i<MAX_COLUMN_INDEX; i++){
		depthVector[i]=readProcessedData.depth[i];
	}

	/* Store phosphorus concentration at bottom*/
	phosphorus_concentration_at_bottom_in_hour= new physicalType[readProcessedData.simulationCycles];
	nitrogen_concentration_at_bottom_in_hour= new physicalType[readProcessedData.simulationCycles];
	yearly_light_at_surface= new physicalType[readProcessedData.simulationCycles];
	for(int i=0; i<readProcessedData.simulationCycles; i++){
		phosphorus_concentration_at_bottom_in_hour[i]=readProcessedData.phosphorusConcentrationAtBottom[i];
		nitrogen_concentration_at_bottom_in_hour[i]=readProcessedData.nitrogenConcentrationAtBottom[i];
		yearly_light_at_surface[i] = readProcessedData.yearlylightAtSurface[i];
	}
	/*Copy arrays that depend on maximum depth*/
	for(int i=0; i<MAX_DEPTH_INDEX; i++){
		this->temperature_range[i]=readProcessedData.temperature_range[i];
		this->temperature_depth_proportion[i]=readProcessedData.temperature_depth_proportion[i];
		this->indexToDepth[i]=readProcessedData.depth_scale[i];
		this->baseAlgaeBiomassDifferential[i]=readProcessedData.baseBiomassDifferential[i];

	}
	/*Copy cyclic light at surface*/
	for(int i=0; i<HOURS_PER_DAY; i++){
		this->hourlyLightAtSurface[i]=readProcessedData.hourlyLightAtSurface[i];
	}
	for(int i=0; i<HOURS_PER_YEAR; i++){
		this->temperature_at_day[i]=readProcessedData.temperature_at_day[i];
	}
	/*Copy zooplankton biomass center per daily hour*/
	for(int i=0; i<HOURS_PER_DAY; i++){
		this->zooplanktonBiomassCenterDifferencePerDepth[i]=readProcessedData.zooplanktonBiomassCenterDifferencePerDepth[i];
	}
	/* Calculate maximum depth index*/
	for(int j=0; j<MAX_DEPTH_INDEX; j++){
		for(int i=0; i<MAX_COLUMN_INDEX; i++){
			if((depthVector[i]==indexToDepth[j])||(depthVector[i]<indexToDepth[j+1]&&depthVector[i]>=indexToDepth[j])){
				this->maxDepthIndex[i]=j;
			}

		}
	}
#ifdef INDIVIDUAL_BASED_ANIMALS
	/* IMPORTANT: clear the cohort ID at the creation of juveniles*/
	this->cohortID=0;
#endif
	/* Copy initial biomass vector*/
	for(int i=0; i<MAX_COLUMN_INDEX; i++){
		/* Initialize periphyton and bottom feeder biomass*/
		this->periDifferential[i] = 0;
		this->periBiomass[i]=readProcessedData.initial_algae_biomass[maxDepthIndex[i]][i];
#ifdef ADD_DEAD_BIOMASS_NUTRIENTS
		this->previousDeadBottomBiomass[i]=this->deadBottomBiomass[i]=0.0f;
#endif
		/* Copy bottom grazer biomass per cell*/
#ifdef  INDIVIDUAL_BASED_ANIMALS
		/*Initialize cohort IDs*/

		addAnimalCohorts(maxDepthIndex[i],i,readProcessedData.initial_grazer_count[maxDepthIndex[i]][i], bottomGrazers, true);
#else
		this->bottomFeederCount[i]=readProcessedData.initial_grazer_count[maxDepthIndex[i]][i];
#endif
		for(int j=0; j<MAX_DEPTH_INDEX; j++){
			this->temperature[j][i] = this->initial_temperature[j][i] = readProcessedData.initial_temperature[j][i];
			this->phytoDifferential[j][i]=0.0f;
			if(j>=maxDepthIndex[i]){
				/* If the cell depth is greater than the lake depth, biomass is 0 (out of the lake)*/
				this->phytoBiomass[j][i]=0.0f;
#ifndef INDIVIDUAL_BASED_ANIMALS
				this->zooplanktonCount[j][i]=0;
#endif

				break;
			} else {
				/* If we are modeling any biomass that it is not at the bottom, then all initial biomass is phytoplankton*/
					this->phytoBiomass[j][i]=readProcessedData.initial_algae_biomass[j][i];
#ifdef ADD_DEAD_BIOMASS_NUTRIENTS
					this->previousDeadFloatingBiomass[j][i]=this->deadFloatingBiomass[j][i]=0.0f;
#endif
					/* Copy floating grazer biomass per cell*/
	#ifdef INDIVIDUAL_BASED_ANIMALS
					if(readProcessedData.initial_grazer_count[j][i]>0.0f){
						addAnimalCohorts(j,i,readProcessedData.initial_grazer_count[j][i], zooplankton, false);
					}
	#else
					this->zooplanktonCount[j][i]=readProcessedData.initial_grazer_count[j][i];
	#endif
			}
		}
	}
	setBathymetricParameters();
	calculateDistanceToFocus();
}

/* Functions for adding individual-based animal cohorts*/
#ifdef INDIVIDUAL_BASED_ANIMALS

#ifdef ANIMAL_COHORT_MAP
void FoodWebModel::FoodWebModel::addAnimalCohorts(unsigned int depthIndex, unsigned int columnIndex, animalCountType count, map<pair<int,int>,AnimalCohort>& animals, bool isBottomAnimal){
#else
	void FoodWebModel::FoodWebModel::addAnimalCohorts(unsigned int depthIndex, unsigned int columnIndex, animalCountType count, vector<AnimalCohort>& animals, bool isBottomAnimal){
#endif
	if(count>0){
//		addAnimalCohort(depthIndex,columnIndex,count, animals, AnimalStage::Juvenile, isBottomAnimal);
		addAnimalCohort(depthIndex,columnIndex,count, animals, AnimalStage::Mature, isBottomAnimal);
	}
}
#ifdef ANIMAL_COHORT_MAP
void FoodWebModel::FoodWebModel::addAnimalCohort(unsigned int depthIndex, unsigned int columnIndex, animalCountType count, map<pair<int,int>,AnimalCohort>& animals, AnimalStage developmentStage, bool isBottomAnimal){
#else
	void FoodWebModel::FoodWebModel::addAnimalCohort(unsigned int depthIndex, unsigned int columnIndex, animalCountType count, vector<AnimalCohort>& animals, AnimalStage developmentStage, bool isBottomAnimal){
#endif
	if(count>0&&readProcessedData.initial_grazer_distribution[developmentStage]>0.0f){
		AnimalCohort newCohort;
		//pair<int,int> cohortCoordinates(depthIndex, columnIndex);
		newCohort.x=depthIndex;
		newCohort.y=columnIndex;
		newCohort.stage=developmentStage;
		newCohort.isBottomAnimal=isBottomAnimal;
//		newCohort.ageInHours=newCohort.hoursWithoutFood=0;
//		newCohort.death=None;
		newCohort.cohortID=this->cohortID++;
		newCohort.justMatured=false;
		newCohort.latestMigrationIndex=newCohort.hoursInStarvation=0;
		newCohort.migrationConstant=-(this->grazer_layer_center_index-depthIndex);
		newCohort.currentFitnessValue=newCohort.currentFoodBiomass=newCohort.currentPredatorSafety=0.0f;
		newCohort.predatorFitness=predatorFitnessType::Unassigned;
		if(abs<int>((int)newCohort.migrationConstant)>2){
			cout<<"Far away elements."<<endl;
		}
//		if(abs((int)newCohort.migrationConstant)==0){
//					cout<<"Centered elements."<<endl;
//				}
		newCohort.numberOfIndividuals=count*readProcessedData.initial_grazer_distribution[developmentStage];
		newCohort.bodyBiomass=newCohort.numberOfIndividuals*readProcessedData.initial_grazer_weight[developmentStage];
		newCohort.gonadBiomass=newCohort.starvationBiomass=0.0f;
		newCohort.previousFitnessValue=newCohort.currentFitnessValue=0.0f;
		newCohort.upDirection=false;
		if(indexToDepth[maxDepthIndex[newCohort.y]]>=TRACED_COHORT_DEPTH&&newCohort.migrationConstant==0){
			cout<<"Optimal grazer ID is "<<newCohort.cohortID<<"."<<endl;
		}
		if(indexToDepth[maxDepthIndex[newCohort.y]]>=TRACED_COHORT_DEPTH&&grazerDynamics.tracedCohort.numberOfIndividuals==0){
			/*Create traced cohort*/
			grazerDynamics.tracedCohort=newCohort;
			cout<<"Traced cohort: "<<grazerDynamics.tracedCohortID<<"."<<endl;
		} else{
#ifdef ANIMAL_COHORT_MAP
			if ( animals.find(pair<int,int>(depthIndex, columnIndex)) == animals.end() ) {
				/* If the cohort exists, increase biomass and number of eggs*/
				animals[pair<int,int>(depthIndex, columnIndex)]=newCohort;
			} else{
				animals[pair<int,int>(depthIndex, columnIndex)]+=newCohort;
			}
#else
			animals.push_back(newCohort);
#endif
		}

	}
}

#endif

FoodWebModel::FoodWebModel::FoodWebModel(const SimulationArguments& simArguments){
/* Read the geophysical parameters from the lake, including depth and temperature at water surface
 *
 * */
	cout<<"Reading parameters."<<endl;
	readProcessedData.readModelData(simArguments);
	cout<<"Parameters read."<<endl;
	cout<<"Setting parameters from file."<<endl;
	setFileParameters(simArguments);
	cout<<"Parameters from file set."<<endl;

}

/*
 * Calculate system-specific bathymetric parameters
 */
void FoodWebModel::FoodWebModel::setBathymetricParameters(){
	calculatePhysicalLakeDescriptors();
	grazerDynamics.ZMax=this->ZMax;
	/*
	 *  (AquaTox Documentation, page 45, equation 8)
	 */
	this->P =6.0l*ZMean/ZMax-3.0l;

	/*
	 *  (AquaTox Documentation, page 46, equation 9)
	 */
	this->fractionInEuphoticZone=(1-P)*Z_EUPHOTIC/ZMax + P*pow(Z_EUPHOTIC/ZMax, 2);
}

/*
 * Biomass lost to respiration (AquaTox Documentation, page 84, equation 63)
 */
void FoodWebModel::FoodWebModel::algaeRespiration(biomassType localPointBiomass, physicalType localTemperature){
	algae_respiration_value = -min(photosynthesis_value*MAX_RESPIRATION_PHOTOSYNTHESIS_RATE,this->algal_respiration_at_20_degrees*pow(this->exponential_temperature_algal_respiration_coefficient, localTemperature-20)*localPointBiomass);
}

/*
 * Biomass lost to excretion (AquaTox Documentation, page 85, equation 64)
 */
void FoodWebModel::FoodWebModel::algaeExcretion(){
	algae_excretion_value =  -PROPORTION_EXCRETION_PHOTOSYNTHESIS*(1-light_allowance)*photosynthesis_value;
}


/*
 * Biomass lost to natural mortality (AquaTox Documentation, page 86, equation 66)
 */
biomassType FoodWebModel::FoodWebModel::algaeNaturalMortality(physicalType localeTemperature, physicalType localeLimitationProduct, biomassType localPointBiomass){
	algaeHighTemperatureMortality(localeTemperature);
	resourceLimitationStress(localeLimitationProduct);
#ifdef ALGAL_CARRYING_CAPACITY_MORTALITY
	//calculateAlgalCarryingCapacityMortality(localPointBiomass);
#else
	this->used_algal_mortality_rate=this->intrinsic_algae_mortality_rate;
#endif
//	algae_natural_mortality_factor = min(this->used_algal_mortality_rate+high_temperature_mortality+weighted_resource_limitation_stress,(double)1.0f);
//	algae_natural_mortality_factor = min(used_algal_mortality_rate+this->high_temperature_mortality+weighted_resource_limitation_stress,(double)1.0f);
	algae_natural_mortality_factor = min(used_algal_mortality_rate+this->high_temperature_mortality,(double)1.0f);
	algae_natural_mortality = -algae_natural_mortality_factor*localPointBiomass;
	/*Return the calculated mortality factor*/
	return algae_natural_mortality_factor;

}

/*
 * Biomass lost to high temperature (AquaTox Documentation, page 86, equation 67)
 */
void FoodWebModel::FoodWebModel::algaeHighTemperatureMortality(physicalType localeTemperature){
	high_temperature_mortality = exp(localeTemperature-MAXIMUM_TOLERABLE_TEMPERATURE)/2.0f;

}

/*
 * Biomass lost to stress related to resource limitation (AquaTox Documentation, page 86, equation 68)
 */
void FoodWebModel::FoodWebModel::resourceLimitationStress(physicalType localeLimitationProduct){
#ifdef CHECK_LOST_BIOMASS_ADDITION
	if(localeLimitationProduct>1.0f){
		cout<<"Locale limitation product: "<<localeLimitationProduct<<" is higher than 1.";
	}
#endif
	resource_limitation_exponent = -this->maximum_algae_resources_death*(1-localeLimitationProduct);
	resource_limitation_stress= 1.0f-exp(resource_limitation_exponent);
	weighted_resource_limitation_stress = resource_limitation_stress;
}

/*
 * Carrying capacity algal mortality. Introduce to control base mortality as a function of algal biomass
 */
void FoodWebModel::FoodWebModel::calculateAlgalCarryingCapacityMortality(biomassType localPointBiomass){
	 this->used_algal_mortality_rate = 1/(1+exp(-localPointBiomass*this->algal_carrying_capacity_coefficient+this->algal_carrying_capacity_intercept));
//	 this->used_algal_mortality_rate = 0.0f;
}

/*
 * Biomass moved from phytoplankton to periphyton by sinking (AquaTox Documentation, page 87, equation 69)
 */
void FoodWebModel::FoodWebModel::algaeSinking(int depthIndex, int columnIndex){

	sedimentation_rate=(this->intrinsic_settling_rate/indexToDepth[depthIndex]);
	algae_sinking_value= sedimentation_rate*phytoBiomass[depthIndex][columnIndex];

	verticalLostPhytoBiomass[depthIndex][columnIndex]+=algae_sinking_value;

	if(depthIndex>=maxDepthIndex[columnIndex]){
		/* Sink algae biomass to periPhyton if the bottom of the lake is reached*/

		verticalGainedPeriBiomass[columnIndex]+=algae_sinking_value;
	} else{
		verticalGainedPhytoBiomass[depthIndex+1][columnIndex]+=algae_sinking_value;
	}
}

/*
 * Biomass washed from periphyton to phytoplankton by slough (AquaTox Documentation, page 92, equation 75)
 */
void FoodWebModel::FoodWebModel::algaeSlough(int columnIndex){
	algae_slough_value = periBiomass[columnIndex]*this->algal_fraction_sloughed;
	verticalLostPeriBiomass[columnIndex]+=algae_slough_value;
	/*The sloughed biomass becomes part of the phytoplankton*/
	verticalGainedPhytoBiomass[maxDepthIndex[columnIndex]][columnIndex] += algae_slough_value;
}

void FoodWebModel::FoodWebModel::calculateTemperature(int depthIndex, int columnIndex){
#ifdef LAKE_DATA_TEMPERATURE
	physicalType updatedTemperature=this->temperature_at_day[current_hour%HOURS_PER_YEAR]*this->temperature_depth_proportion[depthIndex];
#else
	int dayOfYear = (current_hour/24)%365;
	physicalType localeTemperature=temperature[depthIndex][columnIndex];
	temperature_angular_frequency = TEMPERATURE_AMPLITUDE*(TEMPERATURE_DAY_FACTOR*(dayOfYear+TEMPERATURE_PHASE_SHIFT)-TEMPERATURE_DELAY);
	temperature_sine = sin(temperature_angular_frequency);
	/* Temperature can be modeled as a sigmoidal function sloping at the thermocline or as a sinusoidal function*/
#ifdef IBM_MODEL_TEMPERATURE
	physicalType updatedTemperature = localeTemperature+(-1.0*(temperature_range[depthIndex]))*temperature_sine;
#else
	physicalType updatedTemperature = (1-1/(1+exp(-TEMPERATURE_SLOPE_AMPLITUDE*(indexToDepth[depthIndex]-ZMax/2)+TEMPERATURE_SLOPE_PHASE)))*TEMPERATURE_ADDITIVE_COMPONENT+TEMPERATURE_LOWER_LIMIT;
#endif
#endif
	temperature[depthIndex][columnIndex] = updatedTemperature;
}


/*
 *
 */
/*Can floating be omitted from the model? I could not find the equation modeling it.*/

/*Are we going to introduce hydrodynamics in the model? If so, we need to model the equivalent to the following equations (AquaTox Documentation, page 67, equation 33)
* 1. Washout
* 2. Washin
* 3. TurbDiff
* 4. Diffusion
*/

/*
 * Since our model will be spatially-explicit, can we replace these hydrodynamics equations with local neighborhood rules?
 * For instance, a fraction of phytoplankton biomass is drawn out and drawn in from the neighboring cells.
 * */


void FoodWebModel::FoodWebModel::initializePointers(){

//	for (int i = 0; i < MAX_COLUMN_INDEX; ++i) {
//		this->localBiomass[i] = new biomassType[MAX_COLUMN_INDEX];
//		this->periBiomass[i] = new biomassType[MAX_COLUMN_INDEX];
//		this->phytoBiomass[i] = new biomassType[MAX_COLUMN_INDEX];
//		this->temperature[i] = new physicalType[MAX_COLUMN_INDEX];
//	}

}

void FoodWebModel::FoodWebModel::calculatePhysicalLakeDescriptors(){

	/* Calculate maximum and mean depth*/
	this->ZMax=this->ZMean=0;
	this->ZMaxIndex=-1;
	for(int i=0; i<MAX_COLUMN_INDEX; i++){
		if(this->ZMax<this->depthVector[i]){
			ZMax = this->depthVector[i];
			ZMaxIndex=i;
		}
		this->ZMean+=this->depthVector[i];
	}
	this->ZMean/=(physicalType)MAX_COLUMN_INDEX;

}

/* Calculation of algae biomass*/

void FoodWebModel::FoodWebModel::normalizeLight(int columnIndex){
#ifndef HOMOGENEOUS_DEPTH
	light_normalizer = algae_biomass_to_depth*depthVector[columnIndex];
#else
	light_normalizer = algae_biomass_to_depth*ZMax;
#endif
}

/*
 * Amount of light that is allowed into the biomass depth
 * Adapted from (AquaTox Documentation, page 70, equation 39)
 * */
void FoodWebModel::FoodWebModel::lightAllowance(biomassType localeAlgaeBiomass){

light_difference=light_at_depth-light_at_top;
#ifdef PROPORTIONAL_LIGHT
	/* Use a simplistic model of light decreasing exponentially with depth*/
	light_allowance = light_at_depth/light_at_top;
#elif defined(LINEAR_LIGHT)
	light_allowance = (physicalType)LIGHT_ALLOWANCE_INTERCEPT+ALGAE_BIOMASS_COEFFICIENT_LIGHT_ALLOWANCE*localeAlgaeBiomass+RADIATION_COEFFICIENT_LIGHT_ALLOWANCE*light_at_depth;
#elif defined(AQUATOX_LIGHT_ALLOWANCE)
	light_allowance = min((double)1.0f,(physicalType)this->light_allowance_weight*light_difference/(this->light_at_depth_exponent*ZMax));
#else
	light_difference=Math_E*light_difference;
	light_allowance=1/(1+exp(-1.0f*light_difference));
	//sigmoid_light_difference=1;
#endif
	light_allowance=min((double)1.0f,max(light_allowance, (double)0.0f));
	/* Uncomment this line to make the model equivalent to AquaTox*/
	//normalized_light_difference =light_difference/(light_normalizer+light_difference);
	//return localePhotoPeriod*normalized_light_difference;
}

/*
 * Daily photo-period modeling light limitation
 * Adapted from (AquaTox Documentation, page 58, equation 26)
 * */
void FoodWebModel::FoodWebModel::photoPeriod(){
	physicalType hour_fraction = ((physicalType)(current_hour%HOURS_PER_DAY))/(double)HOURS_PER_DAY;
	locale_photo_period= max<double>(0.0f, cos(2.0f*Math_PI*hour_fraction))*0.5f +0.5f;
}

/*
 * Hourly light at top
 * */
void FoodWebModel::FoodWebModel::calculateLightAtTop(){
#ifdef USE_PHOTOPERIOD
	light_at_top= AVERAGE_INCIDENT_LIGHT_INTENSITY*localePhotoPeriod;
#elif defined(USE_YEARLY_LIGHT_SURFACE)
	light_at_top= this->yearly_light_at_surface[current_hour%MAX_LIGHT_DATA];
#else
	light_at_top= this->hourlyLightAtSurface[current_hour%HOURS_PER_DAY];
#endif
}


/*Phosphorus concentration at a given depth*/

void FoodWebModel::FoodWebModel::phosphorusConcentrationAtDepth(int depthIndex, int columnIndex){

#ifdef TIME_VARIABLE_PHOSPHORUS_CONCENTRATION_AT_BOTTOM
	current_phosphorus_concentration_at_bottom = this->phosphorous_weight * phosphorus_concentration_at_bottom_in_hour[current_hour];
#else
	current_phosphorus_concentration_at_bottom = PHOSPHORUS_CONCENTRATION_AT_BOTTOM;
#endif
	chemicalConcentrationAtDepth(depthIndex, columnIndex, current_phosphorus_concentration_at_bottom);
	/* Save phosphorus concentration for later use*/
	phosphorus_concentration[depthIndex][columnIndex] = chemical_concentration;
}

void FoodWebModel::FoodWebModel::nitrogenConcentrationAtDepth(int depthIndex, int columnIndex){

	current_nitrogen_concentration_at_bottom = nitrogen_concentration_at_bottom_in_hour[current_hour];
	chemicalConcentrationAtDepth(depthIndex, columnIndex, current_nitrogen_concentration_at_bottom);
	/* Save phosphorus concentration for later use*/
	nitrogen_concentration[depthIndex][columnIndex] = chemical_concentration;
}

/*Salt concentration at a given depth*/

void FoodWebModel::FoodWebModel::saltConcentrationAtDepth(int depthIndex, int columnIndex){
	chemicalConcentrationAtDepth(depthIndex, columnIndex, SALT_CONCENTRATION_AT_BOTTOM);
}



/*Nutrient concentration at a given depth*/

void FoodWebModel::FoodWebModel::chemicalConcentrationAtDepth(int depthIndex, int columnIndex, physicalType concentrationAtBottom){
	physicalType localeNutrientAtBottom = concentrationAtBottom;
#ifdef SIGMOID_CONCENTRATION
	/* Using depth-dependent sigmoid function from ([1] A. Herbland, D. Delmas, P. Laborde, B. Sautour, and F. Artigas, “Phytoplankton spring bloom of the Gironde plume waters in the Bay of Biscay: early phosphorus limitation and food-web consequences,” Oceanol. Acta, vol. 21, no. 2, pp. 279–291, Mar. 1998.)*/
	physicalType depthScale = (1.0f+this->nutrient_growth/(1.0f+exp(-this->indexToDepth[depthIndex]+this->nutrient_derivative)));
	chemical_concentration = localeNutrientAtBottom*depthScale;
#else
#ifdef HOMOGENEOUS_DEPTH
	chemical_at_depth_exponent = (double)(this->nutrient_derivative*(this->indexToDepth[depthIndex]-ZMax));
#elif defined(RADIATED_CHEMICAL)
	chemical_at_depth_exponent = (double)(this->nutrient_derivative*(-this->distance_to_focus[depthIndex][columnIndex]));
#else
	chemical_at_depth_exponent = (double)(this->nutrient_derivative*(this->indexToDepth[depthIndex]-depthVector[columnIndex]));
#endif
	chemical_concentration =localeNutrientAtBottom*exp(chemical_at_depth_exponent);
#endif


}

/* Nutrient biomass growth limitation based on nutrient concentration */

//void FoodWebModel::FoodWebModel::calculateNutrientLimitation(biomassType localPointBiomass, physicalType phosphorusConcentration){
//	nutrient_limitation=0.0f;
////	if(phosphorusConcentration>=PHOSPHORUS_LINEAR_THRESHOLD)
////		nutrient_limitation=1.0f;
////	else{
//#ifdef NUTRIENT_LIMITATION_GLM
//		nutrient_limitation=max((double)0.0f,(phosphorusConcentration*PHOSPHORUS_LIMITATION_LINEAR_COEFFICIENT_TFP+localPointBiomass*PHOSPHORUS_LIMITATION_LINEAR_COEFFICIENT_ALGAE_BIOMASS+PHOSPHORUS_INTERCEPT_GLM));
//#elif defined(NUTRIENT_LIMITATION_QUOTIENT)
//		biomassType chemical_concentration_in_grams=phosphorusConcentration*MICROGRAM_TO_GRAM*M3_TO_LITER;
//		nutrient_limitation=chemical_concentration_in_grams/(chemical_concentration_in_grams+this->phosphorus_half_saturation);
//#elif defined(SEVERAL_LEVELS_LIMITATION_QUOTIENT)
//		nutrient_limitation = 1;///(1+this->phosphorus_half_saturation*exp(-phosphorusConcentration*this->phosphorus_functional_factor+this->phosphorus_functional_constant_response_1));
//		  if(phosphorusConcentration>this->phosphorus_functional_step_1){
//			  /* Formula taken from [1] H. Xu, H. W. Paerl, B. Qin, G. Zhu, and G. Gaoa, “Nitrogen and phosphorus inputs control phytoplankton growth in eutrophic Lake Taihu, China,” Limnol. Oceanogr., vol. 55, no. 1, pp. 420–432, Jan. 2010.*/
//			  nutrient_limitation = this->phosphorus_half_saturation*phosphorusConcentration/(phosphorusConcentration+this->phosphorus_functional_constant_response_2)/HOURS_PER_DAY+1.0f;
////			  nutrient_limitation = 1/(1+this->phosphorus_half_saturation*exp(-phosphorusConcentration*this->phosphorus_functional_factor+this->phosphorus_functional_constant_response_2))+1;
//		  }
//#else
////		nutrient_limitation=min((double)1.0f,(double)max((double)0.0f,(phosphorusConcentration*PHOSPHORUS_LINEAR_COEFFICIENT+PHOSPHORUS_INTERCEPT)/PHOSPHORUS_GROWTH_LIMIT));
//		nutrient_limitation=(double)max((double)0.0f,(phosphorusConcentration*PHOSPHORUS_LINEAR_COEFFICIENT+PHOSPHORUS_INTERCEPT)/PHOSPHORUS_GROWTH_LIMIT);
//
//#endif
////		nutrient_limitation=1.0f;
//
////	}
//	//returnedValue=1.0f;
//}


physicalType FoodWebModel::FoodWebModel::calculateNutrientLimitation(biomassType localPointBiomass, physicalType phosphorusConcentration, physicalType functionalStep, physicalType constant1, physicalType constant2){
	nutrient_limitation=0.0f;
//	if(phosphorusConcentration>=PHOSPHORUS_LINEAR_THRESHOLD)
//		nutrient_limitation=1.0f;
//	else{
#ifdef NUTRIENT_LIMITATION_GLM
		nutrient_limitation=max((double)0.0f,(phosphorusConcentration*PHOSPHORUS_LIMITATION_LINEAR_COEFFICIENT_TFP+localPointBiomass*PHOSPHORUS_LIMITATION_LINEAR_COEFFICIENT_ALGAE_BIOMASS+PHOSPHORUS_INTERCEPT_GLM));
#elif defined(NUTRIENT_LIMITATION_QUOTIENT)
		biomassType chemical_concentration_in_grams=phosphorusConcentration*MICROGRAM_TO_GRAM*M3_TO_LITER;
		nutrient_limitation=chemical_concentration_in_grams/(chemical_concentration_in_grams+this->phosphorus_half_saturation);
#elif defined(SEVERAL_LEVELS_LIMITATION_QUOTIENT)
		nutrient_limitation = 1.0f;///(1+this->phosphorus_half_saturation*exp(-phosphorusConcentration*this->phosphorus_functional_factor+this->phosphorus_functional_constant_response_1));
		  if(phosphorusConcentration>functionalStep){
			  /* Formula taken from [1] H. Xu, H. W. Paerl, B. Qin, G. Zhu, and G. Gaoa, “Nitrogen and phosphorus inputs control phytoplankton growth in eutrophic Lake Taihu, China,” Limnol. Oceanogr., vol. 55, no. 1, pp. 420–432, Jan. 2010.*/
//			  nutrient_limitation = constant1*phosphorusConcentration/(phosphorusConcentration+constant2)/HOURS_PER_DAY+1.0f;
			  nutrient_limitation = constant1*phosphorusConcentration/(phosphorusConcentration+constant2)/HOURS_PER_DAY;
//			  nutrient_limitation = 1/(1+this->phosphorus_half_saturation*exp(-phosphorusConcentration*this->phosphorus_functional_factor+this->phosphorus_functional_constant_response_2))+1;
		  }
#else
//		nutrient_limitation=min((double)1.0f,(double)max((double)0.0f,(phosphorusConcentration*PHOSPHORUS_LINEAR_COEFFICIENT+PHOSPHORUS_INTERCEPT)/PHOSPHORUS_GROWTH_LIMIT));
		nutrient_limitation=(double)max((double)0.0f,(phosphorusConcentration*PHOSPHORUS_LINEAR_COEFFICIENT+PHOSPHORUS_INTERCEPT)/PHOSPHORUS_GROWTH_LIMIT);

#endif
//		nutrient_limitation=1.0f;

//	}
		return nutrient_limitation;
	//returnedValue=1.0f;
}


/* To model the effect of temperature on algal growth, a Gaussian bell inspired by (AquaTox Documentation, page 82, equation 59 and page 84, figure 60) is modeled*/
void FoodWebModel::FoodWebModel::calculateTemperatureLimitation(int depthIndex, int columnIndex){
	physicalType localeTemperature = -temperature[depthIndex][columnIndex];
	physicalType temperatureBell = exp(-(pow((this->temperature_optimal+localeTemperature),2.0f)/this->temperature_steepness));
	physicalType highTemperatureSuppression = 1-1/(1+exp(localeTemperature/this->temperature_suppression_steepness+(this->temperature_max)));
	temperature_limitation = temperatureBell*highTemperatureSuppression;
	temperature_limitation_matrix[depthIndex][columnIndex] = temperature_limitation;
}

/* Salinity effect on respiration and mortality (AquaTox Documentation, page 295, equation 440)*/
void FoodWebModel::FoodWebModel::salinityEffect(unsigned int depthIndex, unsigned int columnIndex, physicalType saltConcentration){
	physicalType salinity_effect=1;
	salinity_exponent=0.0f;
	biomassType salinityBase=1.0f;
	if(saltConcentration<MIN_SALINITY){
		salinity_exponent=saltConcentration-MIN_SALINITY;
		salinity_effect=SALINITY_COEFFICIENT_LOW*exp(salinity_exponent);
	}
	if(chemical_concentration>MAX_SALINITY){
		salinity_exponent=MAX_SALINITY-chemical_concentration;
		salinity_effect=SALINITY_COEFFICIENT_HIGH*exp(salinity_exponent);
	}
	salinity_effect_matrix[depthIndex][columnIndex] = salinity_effect;

}



void FoodWebModel::FoodWebModel::calculateDistanceToFocus(){
	for(int depthIndex=0; depthIndex<MAX_DEPTH_INDEX; depthIndex++){
		for(int columnIndex=0; columnIndex<MAX_COLUMN_INDEX; columnIndex++){
			distance_to_focus[depthIndex][columnIndex] =sqrt(pow(ZMax-indexToDepth[depthIndex],2)+pow((ZMaxIndex-columnIndex)*COLUMN_TO_METER,2));
		}
	}

}

#ifdef ADD_DEAD_BIOMASS_NUTRIENTS
void FoodWebModel::FoodWebModel::updateDeadBiomass(){
	for(int columnIndex=0; columnIndex<MAX_COLUMN_INDEX; columnIndex++){
		for(int depthIndex=0; depthIndex<=maxDepthIndex[columnIndex]; depthIndex++){
#ifdef CHECK_LOST_BIOMASS_ADDITION
			if(deadFloatingBiomass[depthIndex][columnIndex]<0.0f){
				cout<<"Lower than 0 dead biomass: "<<deadFloatingBiomass[depthIndex][columnIndex]<<"."<<endl;
			}
#endif
			biomassType washupDownPhytoplanktonBiomass = this->wash_down_dead_biomass_proportion*deadFloatingBiomass[depthIndex][columnIndex];
			biomassType movedBiomass=washupDownPhytoplanktonBiomass;
			if(depthIndex>0){
				/* If not the most superficial water layer, move up washed-up biomass */
				biomassType washupDeadPhytoplanktonBiomass = this->wash_up_dead_biomass_proportion*deadFloatingBiomass[depthIndex][columnIndex];
				movedBiomass+=washupDeadPhytoplanktonBiomass;
				deadFloatingBiomass[depthIndex-1][columnIndex]+= washupDeadPhytoplanktonBiomass;
			}
			if(depthIndex<maxDepthIndex[columnIndex]){
				/* If not the deepest water layer, move down washed-up biomass */
				deadFloatingBiomass[depthIndex+1][columnIndex]+= washupDownPhytoplanktonBiomass;
			} else{
				/*Otherwise, incorporate to periphyton*/
				deadBottomBiomass[columnIndex]+= washupDownPhytoplanktonBiomass;

			}
#ifdef CHECK_LOST_BIOMASS_ADDITION
			if(deadFloatingBiomass[depthIndex][columnIndex]<0.0f){
				cout<<"Lower than 0 dead biomass: "<<deadFloatingBiomass[depthIndex][columnIndex]<<"."<<endl;
			}
#endif
			/* Remove moved biomass*/
			deadFloatingBiomass[depthIndex][columnIndex]-=movedBiomass;
			/*Phytoplankton after degradation*/
			previousDeadFloatingBiomass[depthIndex][columnIndex] =deadFloatingBiomass[depthIndex][columnIndex];

			deadFloatingBiomass[depthIndex][columnIndex]*=this->retained_phosphorus_factor;
#ifdef CHECK_LOST_BIOMASS_ADDITION
			if(deadFloatingBiomass[depthIndex][columnIndex]<0.0f){
				cout<<"Lower than 0 dead biomass: "<<deadFloatingBiomass[depthIndex][columnIndex]<<"."<<endl;
			}
#endif

		}
		/* Washup of periphyton*/
		biomassType washupDeadPeriphytonBiomass = this->wash_up_dead_biomass_proportion*deadBottomBiomass[columnIndex];
		deadFloatingBiomass[maxDepthIndex[columnIndex]][columnIndex]+= washupDeadPeriphytonBiomass;
		deadBottomBiomass[columnIndex]-= washupDeadPeriphytonBiomass;

		/*Periphyton after degradation*/
		previousDeadBottomBiomass[columnIndex] =deadBottomBiomass[columnIndex];
		deadBottomBiomass[columnIndex]*=this->retained_phosphorus_factor;


	}

}

#endif
void FoodWebModel::FoodWebModel::updateAlgaeVerticalMigration(){

	/*Clear vertical migration values*/
	for(int columnIndex=0; columnIndex<MAX_COLUMN_INDEX; columnIndex++){
		verticalGainedPeriBiomass[columnIndex]=0.0f;
		for(int depthIndex=0; depthIndex<=maxDepthIndex[columnIndex]; depthIndex++){
			verticalGainedPhytoBiomass[depthIndex][columnIndex]=0.0f;
		}
	}
	/*Calculate vertical migration values*/
	for(int columnIndex=0; columnIndex<MAX_COLUMN_INDEX; columnIndex++){
		algaeSlough(columnIndex);
		for(int depthIndex=0; depthIndex<=maxDepthIndex[columnIndex]; depthIndex++){
			algaeSinking(depthIndex, columnIndex);
		}
	}
	for(int columnIndex=0; columnIndex<MAX_COLUMN_INDEX; columnIndex++){
		periBiomassDifferential[columnIndex]+=verticalGainedPeriBiomass[columnIndex]-verticalLostPeriBiomass[columnIndex];
		periBiomassDifferential[columnIndex]*=algae_biomass_differential_scale;
		if(periBiomassDifferential[columnIndex]>0.0f){
			calculateAlgalCarryingCapacityMortality(periBiomass[columnIndex]);
			periBiomassDifferential[columnIndex]-=this->used_algal_mortality_rate*periBiomassDifferential[columnIndex];
		}
		periBiomass[columnIndex]+=periBiomassDifferential[columnIndex];
		periBiomass[columnIndex]=max<biomassType>(periBiomass[columnIndex],0.0f);
		if(registerBiomass[0][columnIndex]){
			algaeBuffer<<this->maxDepthIndex[columnIndex]<<commaString<<columnIndex<<commaString<<current_hour<<commaString<<1<<commaString<<depthVector[columnIndex];//Depth, Column, Type, Time, Washup
			algaeBuffer<<commaString<<verticalLostPeriBiomass[columnIndex]<<commaString<<verticalGainedPeriBiomass[columnIndex];
			algaeBuffer<<commaString<<periBiomassDifferential[columnIndex]<<commaString<<periBiomass[columnIndex];
			algaeBuffer<<commaString<<limiting_factor[maxDepthIndex[columnIndex]][columnIndex]<<commaString<<light_allowance_matrix[maxDepthIndex[columnIndex]][columnIndex]<<commaString<<lakeLightAtDepth[maxDepthIndex[columnIndex]][columnIndex]<<commaString<<nutrient_limitation_matrix[maxDepthIndex[columnIndex]][columnIndex]<<commaString<<limitation_product_matrix[maxDepthIndex[columnIndex]][columnIndex];
			algaeBuffer<<commaString<<temperature[maxDepthIndex[columnIndex]][columnIndex]<<commaString<<temperature_limitation_matrix[maxDepthIndex[columnIndex]][columnIndex];
			algaeBuffer<<commaString<<photosynthesis_peri_matrix[columnIndex]<<commaString<<respiration_peri_matrix[columnIndex]<<commaString<<excretion_peri_matrix[columnIndex]<<endl;
		}
		for(int depthIndex=0; depthIndex<=maxDepthIndex[columnIndex]; depthIndex++){
			phytoBiomassDifferential[depthIndex][columnIndex]+=verticalGainedPhytoBiomass[depthIndex][columnIndex]-verticalLostPhytoBiomass[depthIndex][columnIndex];
			phytoBiomassDifferential[depthIndex][columnIndex]*=algae_biomass_differential_scale;
			if(phytoBiomassDifferential[depthIndex][columnIndex]>0.0f){
				calculateAlgalCarryingCapacityMortality(phytoBiomass[depthIndex][columnIndex]);
				phytoBiomassDifferential[depthIndex][columnIndex]-=this->used_algal_mortality_rate*phytoBiomassDifferential[depthIndex][columnIndex];
			}
			phytoBiomass[depthIndex][columnIndex]+=phytoBiomassDifferential[depthIndex][columnIndex];
			phytoBiomass[depthIndex][columnIndex]=max<biomassType>(phytoBiomass[depthIndex][columnIndex],0.0f);
			if(registerBiomass[depthIndex][columnIndex]){
				algaeBuffer<<depthIndex<<commaString<<columnIndex<<commaString<<current_hour<<commaString<<0<<commaString<<indexToDepth[depthIndex];
				algaeBuffer<<commaString<<verticalLostPhytoBiomass[depthIndex][columnIndex]<<commaString<<verticalGainedPhytoBiomass[depthIndex][columnIndex];
				algaeBuffer<<commaString<<phytoBiomassDifferential[depthIndex][columnIndex]<<commaString<<phytoBiomass[depthIndex][columnIndex];
				algaeBuffer<<commaString<<this->limiting_factor[depthIndex][columnIndex]<<commaString<<light_allowance_matrix[depthIndex][columnIndex]<<commaString<<lakeLightAtDepth[depthIndex][columnIndex]<<commaString<<nutrient_limitation_matrix[depthIndex][columnIndex]<<commaString<<limitation_product_matrix[depthIndex][columnIndex];
				algaeBuffer<<commaString<<temperature[depthIndex][columnIndex]<<commaString<<temperature_limitation_matrix[depthIndex][columnIndex];
				algaeBuffer<<commaString<<photosynthesis_phyto_matrix[depthIndex][columnIndex]<<commaString<<respiration_phyto_matrix[depthIndex][columnIndex]<<commaString<<excretion_phyto_matrix[depthIndex][columnIndex]<<endl;

			}
		}
	}
}
//	if(registerBiomass[0][columnIndex]){
//				initialAlgaeBuffer<<lineBuffer.str();
//				algaeBuffer<<this->maxDepthIndex[columnIndex]<<commaString<<columnIndex<<commaString<<current_hour<<commaString<<1<<commaString<<depthVector[columnIndex];//Depth, Column, Type, Time, Washup
//	#ifdef EXTENDED_OUTPUT
//				algaeBuffer<<commaString<<0.0f;
//	#endif
//				algaeBuffer<<commaString<<periBiomassDifferential[columnIndex]<<commaString<<periBiomass[columnIndex]<<commaString<<limiting_factor[maxDepthIndex[columnIndex]][columnIndex]<<commaString<<light_allowance_matrix[maxDepthIndex[columnIndex]][columnIndex]<<commaString<<nutrient_limitation_matrix[maxDepthIndex[columnIndex]][columnIndex]<<commaString<<limitation_product_matrix[maxDepthIndex[columnIndex]][columnIndex]<<endl;
//			}


/* As a first approximation, let us assume that we only have Cladocerans (Daphnia) in the system, since they are the main grazers.
 * Assume that the grazing rate of Cladocerans is constant, independent of the abundance of algae biomass in the system
 * Play with the number of Cladocerans in the system, increasing them directly proportional to temperature.
 * Model grazers as an integer number (we are modeling the number of cladocerans, not their biomass) and grazers' biomass as an array of real numbers.
 * We can assume that grazers are evenly distributed across all depth values.
 * */
