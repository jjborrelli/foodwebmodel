/*
 * FoodWebModel.hpp
 *
 *  Created on: 13 mar. 2017
 *      Author: manu_
 */


#ifndef FOODWEBMODEL_HPP_
#define FOODWEBMODEL_HPP_

/* Define mode constants*/


#include <math.h>
#include <string>
#include <sstream>
#include <fstream>
#include <stdio.h>
#include "TypeDefinitions.hpp"
#include "ModelConstants.hpp"
#include "ReadProcessedData.hpp"
#include "AnimalBiomassDynamics.hpp"



namespace FoodWebModel {



	class FoodWebModel {

	private:
#ifdef INDIVIDUAL_BASED_ANIMALS
		cohortIDType cohortID;
#endif
		/*Class attributes*/
	protected:
		ReadProcessedData readProcessedData;
		unsigned int current_hour, ZMaxIndex, simulation_cycles, limiting_factor[MAX_DEPTH_INDEX][MAX_COLUMN_INDEX];
		physicalType light_allowance_matrix[MAX_DEPTH_INDEX][MAX_COLUMN_INDEX], nutrient_limitation_matrix[MAX_DEPTH_INDEX][MAX_COLUMN_INDEX], limitation_product_matrix[MAX_DEPTH_INDEX][MAX_COLUMN_INDEX], temperature_limitation_matrix[MAX_DEPTH_INDEX][MAX_COLUMN_INDEX];
		biomassType photosynthesis_peri_matrix[MAX_COLUMN_INDEX], photosynthesis_phyto_matrix[MAX_DEPTH_INDEX][MAX_COLUMN_INDEX];
		biomassType respiration_peri_matrix[MAX_COLUMN_INDEX], respiration_phyto_matrix[MAX_DEPTH_INDEX][MAX_COLUMN_INDEX];
		biomassType excretion_peri_matrix[MAX_COLUMN_INDEX], excretion_phyto_matrix[MAX_DEPTH_INDEX][MAX_COLUMN_INDEX];
		biomassType phytoMortalityMatrix[MAX_DEPTH_INDEX][MAX_COLUMN_INDEX], periMortalityMatrix[MAX_COLUMN_INDEX];
		physicalType temperature[MAX_DEPTH_INDEX][MAX_COLUMN_INDEX], initial_temperature[MAX_DEPTH_INDEX][MAX_COLUMN_INDEX], distance_to_focus[MAX_DEPTH_INDEX][MAX_COLUMN_INDEX], phosphorus_concentration[MAX_DEPTH_INDEX][MAX_COLUMN_INDEX], nitrogen_concentration[MAX_DEPTH_INDEX][MAX_COLUMN_INDEX], salinity_effect_matrix[MAX_DEPTH_INDEX][MAX_COLUMN_INDEX];
		physicalType depthVector[MAX_COLUMN_INDEX], temperature_range[MAX_DEPTH_INDEX], indexToDepth[MAX_DEPTH_INDEX], hourlyLightAtSurface[HOURS_PER_DAY], *phosphorus_concentration_at_bottom_in_hour, *nitrogen_concentration_at_bottom_in_hour, *yearly_light_at_surface, temperature_depth_proportion[MAX_DEPTH_INDEX], temperature_at_day[HOURS_PER_YEAR];
		unsigned int maxDepthIndex[MAX_COLUMN_INDEX];

		/*Register lake intensity for zooplankton migration*/
		physicalType lakeLightAtDepth[MAX_DEPTH_INDEX][MAX_COLUMN_INDEX], previousLakeLightAtDepth[MAX_DEPTH_INDEX][MAX_COLUMN_INDEX];
		/*Phytoplankton biomass concentration in micrograms/l, periphyton biomass micrograms/l and temperature in Celsius degrees*/
		biomassType phytoBiomass[MAX_DEPTH_INDEX][MAX_COLUMN_INDEX], periBiomass[MAX_COLUMN_INDEX], phytoDifferential[MAX_DEPTH_INDEX][MAX_COLUMN_INDEX], periDifferential[MAX_COLUMN_INDEX], localBiomass[MAX_DEPTH_INDEX][MAX_COLUMN_INDEX], verticalGainedPhytoBiomass[MAX_DEPTH_INDEX][MAX_COLUMN_INDEX], verticalGainedPeriBiomass[MAX_COLUMN_INDEX], verticalLostPhytoBiomass[MAX_DEPTH_INDEX][MAX_COLUMN_INDEX],  verticalLostPeriBiomass[MAX_COLUMN_INDEX], phytoBiomassDifferential[MAX_DEPTH_INDEX][MAX_COLUMN_INDEX], periBiomassDifferential[MAX_COLUMN_INDEX], baseAlgaeBiomassDifferential[MAX_DEPTH_INDEX];

#ifdef ADD_DEAD_BIOMASS_NUTRIENTS
		biomassType previousDeadFloatingBiomass[MAX_DEPTH_INDEX][MAX_COLUMN_INDEX], deadFloatingBiomass[MAX_DEPTH_INDEX][MAX_COLUMN_INDEX], previousDeadBottomBiomass[MAX_COLUMN_INDEX], deadBottomBiomass[MAX_COLUMN_INDEX];
#endif

		/*Vertical movement of dead biomass*/
		biomassType wash_up_dead_biomass_proportion, wash_down_dead_biomass_proportion, algae_biomass_conservation_factor;

		/*Nutrient parameters*/
		biomassType phosphorus_half_saturation, limitation_scale_weight, decaying_phosphorus_factor, retained_phosphorus_factor, reabsorbed_algal_nutrients_proportion, nutrient_derivative, nutrient_growth;

		/*Staircase phosphorus functional response parameters*/
		physicalType
		phosphorus_functional_factor,
		phosphorus_functional_constant_response_1,
		phosphorus_functional_constant_response_2,
		phosphorus_functional_step_1,
		nitrogen_half_saturation,
		nitrogen_functional_constant_response,
		nitrogen_functional_step,
		nitrogen_phosphorus_lower_bound,
		nitrogen_phosphorus_upper_bound;

		/*Light parameters*/
		biomassType light_allowance_weight, light_steepness, diatom_attenuation_coefficient, salinity_exponent, light_allowance_proportion, light_lower_quantile, light_upper_quantile, light_steepness_factor;

		/*Algal respiration and mortality parameters*/
		biomassType algal_respiration_at_20_degrees, exponential_temperature_algal_respiration_coefficient, algae_natural_mortality_factor, algal_mortality_scale;

		/*Algae differential growth*/
		biomassType algae_biomass_differential_production_scale;
		/* Grazer biomass in micrograms*/
		biomassType zooplanktonBiomassCenterDifferencePerDepth[HOURS_PER_DAY];

#ifdef INDIVIDUAL_BASED_ANIMALS
		map<pair<int,int>,AnimalCohort> zooplankton, bottomGrazers;
		map<pair<int,int>,AnimalCohort> floatingPredator, bottomPredator;
#else
		biomassType zooplanktonBiomass[MAX_DEPTH_INDEX][MAX_COLUMN_INDEX], bottomFeederBiomass[MAX_COLUMN_INDEX];
		biomassType verticalMigrationZooplanktonBiomassBuffer[MAX_DEPTH_INDEX][MAX_COLUMN_INDEX];


		/* Grazer individual count. Transformed to biomass using the rule: (count*grazer weight in micrograms)*/
		animalCountType zooplanktonCount[MAX_DEPTH_INDEX][MAX_COLUMN_INDEX], bottomFeederCount[MAX_COLUMN_INDEX];
		biomassType grazerPreferenceScore[MAX_DEPTH_INDEX][MAX_COLUMN_INDEX];





		/* Predator biomass in micrograms*/
		biomassType floatingPredatorBiomass[MAX_DEPTH_INDEX][MAX_COLUMN_INDEX], bottomPredatorBiomass[MAX_COLUMN_INDEX];
		biomassType floatingPredatorBiomassCenterDifferencePerDepth[HOURS_PER_DAY];
		biomassType verticalMigrationfloatingPredatorBiomassBuffer[MAX_DEPTH_INDEX][MAX_COLUMN_INDEX];


		/* Predator individual count. Transformed to biomass using the rule: (count*grazer weight in micrograms)*/
		animalCountType floatingPredatorCount[MAX_DEPTH_INDEX][MAX_COLUMN_INDEX], bottomPredatorCount[MAX_COLUMN_INDEX];
		biomassType preadtorPreferenceScore[MAX_DEPTH_INDEX][MAX_COLUMN_INDEX];
#endif
		/*Grazer count summing. The simulation halts below a given number*/
		animalCountType zooplankton_count_summing;

		/*Grazer count summing. The simulation halts below a given number*/
		animalCountType floating_predator_count_summing;

		/*A vector to reference the calculated biomass*/

		physicalType fractionInEuphoticZone, ZEuphotic, ZMean, ZMax, P;

		/*Max possible column index (X axis)*/
		//int  maxColumn;

		/* Physical single-value parameters*/
		physicalType phosphorous_weight;

		/* Physical attributes*/
		physicalType locale_photo_period,light_at_depth, depthInMeters, turbidity_at_depth, light_at_top, resource_limitation_exponent, light_difference, normalized_light_difference, chemical_at_depth_exponent, light_normalizer, light_allowance, light_at_depth_exponent, temperature_angular_frequency, temperature_sine, nutrient_limitation, chemical_concentration, current_phosphorus_concentration_at_bottom, current_nitrogen_concentration_at_bottom, temperature_limitation;

		/* Algae attributes*/
		biomassType algae_biomass_to_depth, high_temperature_mortality, resource_limitation_stress, weighted_resource_limitation_stress, sedimentation_rate, algae_biomass_differential_scale;

		/* Algal mortality attributes*/
		biomassType intrinsic_algae_mortality_rate, maximum_algae_resources_death, used_algal_mortality_rate, algal_carrying_capacity_coefficient, algal_carrying_capacity_intercept, maximum_found_algal_biomass, intrinsic_settling_rate, algal_fraction_sloughed;
		/* Algae biomass components*/

		biomassType photosynthesis_value, algae_respiration_value, algae_excretion_value, algae_sinking_value, algae_slough_value, algae_natural_mortality;

		unsigned int grazer_layer_center_index;

		/* Buffer line to write simulation results*/
		std::ostringstream lineBuffer, initialAlgaeBuffer, algaeBuffer;

		/* Matrix for biomass registration */
		bool registerBiomass[MAX_DEPTH_INDEX][MAX_COLUMN_INDEX];

		/* Temperature limitation parameters */
		physicalType temperature_optimal, temperature_steepness, temperature_suppression_steepness;

#ifdef CHECK_ASSERTIONS
		std::ostringstream assertionViolationBuffer;
#endif


		/*Output files*/
		std::ofstream outputInitialAlgaeFile, outputAlgaeFile, outputGrazerFile, outputPredatorFile, outputPhysicalFile;
#ifdef INDIVIDUAL_BASED_ANIMALS
		std::ofstream grazerTraceFile, predatorTraceFile;
#endif
		#ifdef CHECK_ASSERTIONS
		std::ofstream outputAssertionViolationFile;
		#endif

		AnimalBiomassDynamics grazerDynamics, predatorDynamics;

		/*Class methods*/

	public:
		FoodWebModel(const SimulationArguments& simArguments);
		int simulate(const SimulationArguments& simArguments);
		void initializeParameters();

	protected:

		/* Main functions*/
		void step();
		void updateRegisterVariables();
		void initializePointers();
#ifdef INDIVIDUAL_BASED_ANIMALS
		void addAnimalCohorts(unsigned int depthIndex, unsigned int columnIndex, animalCountType count, map<pair<int,int>,AnimalCohort>& animals, bool isBottomAnimal);
		void addAnimalCohort(unsigned int depthIndex, unsigned int columnIndex, animalCountType count, map<pair<int,int>,AnimalCohort>& animals, AnimalStage developmentStage, bool isBottomAnimal);
#endif
#ifdef ADD_DEAD_BIOMASS_NUTRIENTS
		void updateDeadBiomass();
#endif
		void printSimulationMode();
		void writeSimulatedParameters(const string& parameterSimulationRoute);
		void setFileParameters(const SimulationArguments& simArguments);
		void initializeGrazerAttributes(const SimulationArguments& simArguments);
		void openSimulationFiles(const SimulationArguments& simArguments);
		void closeSimulationFiles();

		/* Physical descriptors*/
		void calculatePhysicalLakeDescriptors();
		void setBathymetricParameters();
		void calculateDistanceToFocus();

		/* Physical model state*/
		void updatePhysicalState();
		void calculateTemperature(int depthIndex, int columnIndex);
		void photoPeriod();
		void calculateLightAtTop();
		void phosphorusConcentrationAtDepth(int depthIndex, int columnIndex);
		void nitrogenConcentrationAtDepth(int depthIndex, int columnIndex);
		void calculateTemperatureLimitation(int depthIndex, int columnIndex);
		void saltConcentrationAtDepth(int depthIndex, int columnIndex);
		void salinityEffect(unsigned int depthIndex, unsigned int columnIndex, physicalType saltConcentration);

		/*Photosynthesis variable state*/
		void chemicalConcentrationAtDepth(int depthIndex, int columnIndex, physicalType concentrationAtBottom);
		physicalType calculateNutrientLimitation(biomassType localPointBiomass, physicalType phosphorusConcentration, physicalType functionalStep, physicalType constant1, physicalType constant2);
		void updateAlgaeVerticalMigration();


		/*Light variables*/
		void lightAtDepth(int depthIndex, int columnIndex);
		void lightAllowance(biomassType localeAlgaeBiomass);
		void normalizeLight(int depthIndex);
		/* Algae biomass*/
		void updateAlgaeBiomass();
		biomassType algaeBiomassDifferential(int depthIndex, int columnIndex, bool periPhyton);
		biomassType sumPhytoBiomassToDepth(int depthIndex, int columnIndex);
		void photoSynthesis(biomassType localPointBiomass, physicalType localeLimitationProduct, bool periPhyton);
		physicalType productionLimit(physicalType localeLimitationProduct, bool periPhyton);
		void algaeRespiration(biomassType localPointBiomass, physicalType localTemperature);
		void algaeExcretion();
		biomassType algaeNaturalMortality(physicalType localTemperature, physicalType localeLimitationProduct, biomassType localPointBiomass);
		void algaeHighTemperatureMortality(physicalType localeTemperature);
		void resourceLimitationStress(physicalType localeLimitationProduct);
		void calculateAlgalCarryingCapacityMortality(biomassType localPointBiomass);
		void algaeSinking(int depthIndex, int columnIndex);
		void algaeSlough(int columnIndex);
	void copyPointersToAnimalDynamics();
};
}



#endif /* FOODWEBMODEL_HPP_ */
