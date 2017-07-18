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


		/*Class attributes*/
	protected:
		ReadProcessedData readProcessedData;
		unsigned int current_hour, ZMaxIndex, simulation_cycles;
		physicalType temperature[MAX_DEPTH_INDEX][MAX_COLUMN_INDEX], initial_temperature[MAX_DEPTH_INDEX][MAX_COLUMN_INDEX], distance_to_focus[MAX_DEPTH_INDEX][MAX_COLUMN_INDEX], phosphorus_concentration[MAX_DEPTH_INDEX][MAX_COLUMN_INDEX], salinity_effect_matrix[MAX_DEPTH_INDEX][MAX_COLUMN_INDEX];
		physicalType depthVector[MAX_COLUMN_INDEX], temperature_range[MAX_DEPTH_INDEX], indexToDepth[MAX_DEPTH_INDEX], hourlyLightAtSurface[HOURS_PER_DAY], *phosphorus_concentration_at_bottom_in_hour;
		unsigned int maxDepthIndex[MAX_COLUMN_INDEX];

		/*Register lake intensity for zooplankton migration*/
		physicalType lakeLightAtDepth[MAX_DEPTH_INDEX][MAX_COLUMN_INDEX], previousLakeLightAtDepth[MAX_DEPTH_INDEX][MAX_COLUMN_INDEX];
		/*Phytoplankton biomass concentration in micrograms/l, periphyton biomass micrograms/l and temperature in Celsius degrees*/
		biomassType phytoBiomass[MAX_DEPTH_INDEX][MAX_COLUMN_INDEX], periBiomass[MAX_COLUMN_INDEX], phytoDifferential[MAX_DEPTH_INDEX][MAX_COLUMN_INDEX], periDifferential[MAX_COLUMN_INDEX], localBiomass[MAX_DEPTH_INDEX][MAX_COLUMN_INDEX], verticalMigratedPhytoBiomass[MAX_DEPTH_INDEX][MAX_COLUMN_INDEX], verticalMigratedPeriBiomass[MAX_COLUMN_INDEX], sloughPhytoBiomass[MAX_DEPTH_INDEX][MAX_COLUMN_INDEX], phytoBiomassDifferential[MAX_DEPTH_INDEX][MAX_COLUMN_INDEX], periBiomassDifferential[MAX_COLUMN_INDEX], baseAlgaeBiomassDifferential[MAX_DEPTH_INDEX];

		/*Nutrient parameters*/
		biomassType phosphorus_half_saturation, limitation_scale_weight;

		/*Light parameters*/
		biomassType light_allowance_weight, light_steepness, diatom_attenuation_coefficient, salinity_exponent;

		/*Algal respiration and mortality parameters*/
		biomassType algal_respiration_at_20_degrees, exponential_temperature_algal_respiration_coefficient, algae_natural_mortality_factor;

		/*Algae differential growth*/
		biomassType algae_biomass_differential_production_scale;
		/* Grazer biomass in micrograms*/
#ifdef INDIVIDUAL_BASED_ANIMALS
		vector<AnimalCohort> zooplankton, bottomGrazers;
		vector<AnimalCohort> floatingPredator, bottomPredator;
#else
		biomassType zooplanktonBiomass[MAX_DEPTH_INDEX][MAX_COLUMN_INDEX], bottomFeederBiomass[MAX_COLUMN_INDEX];
		biomassType zooplanktonBiomassCenterDifferencePerDepth[HOURS_PER_DAY];
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



		/* Physical attributes*/
		physicalType locale_photo_period,light_at_depth, depthInMeters, turbidity_at_depth, light_at_top, resource_limitation_exponent, light_difference, normalized_light_difference, chemical_at_depth_exponent, light_normalizer, light_allowance, light_at_depth_exponent, temperature_angular_frequency, temperature_sine, nutrient_limitation, chemical_concentration, current_phosphorus_concentration_at_bottom;

		/* Algae attributes*/
		biomassType algae_biomass_to_depth, high_temperature_mortality, resource_limitation_stress, weighted_resource_limitation_stress, sedimentation_rate, algae_biomass_differential_scale;

		/* Algal mortality attributes*/
		biomassType intrinsic_algae_mortality_rate, maximum_algae_resources_death, used_algal_mortality_rate, algal_carrying_capacity_coefficient, algal_carrying_capacity_intercept, maximum_found_algal_biomass;
		/* Algae biomass components*/

		biomassType photosynthesis_value, algae_respiration_value, algae_excretion_value, algae_sinking_value, algae_slough_value, algae_natural_mortality;



		/* Buffer line to write simulation results*/
		std::ostringstream lineBuffer, algaeBuffer, sloughBuffer;

		/* Matrix for biomass registration */
		bool registerBiomass[MAX_DEPTH_INDEX][MAX_COLUMN_INDEX];

#ifdef CHECK_ASSERTIONS
		std::ostringstream assertionViolationBuffer;
#endif


		/*Output files*/
		std::ofstream outputAlgaeFile, outputSloughFile, outputGrazerFile, outputPredatorFile, outputPhysicalFile;
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
		void addAnimalCohorts(unsigned int i, unsigned int j, animalCountType count, vector<AnimalCohort>& animals, bool isBottomAnimal);
		void addAnimalCohort(unsigned int i, unsigned int j, animalCountType count, vector<AnimalCohort>& animals, animalStage developmentStage, bool isBottomAnimal);
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
		void saltConcentrationAtDepth(int depthIndex, int columnIndex);
		void salinityEffect(unsigned int depthIndex, unsigned int columnIndex, physicalType saltConcentration);

		/*Photosynthesis variable state*/
		void chemicalConcentrationAtDepth(int depthIndex, int columnIndex, physicalType concentrationAtBottom);
		void calculateNutrientLimitation(biomassType localPointBiomass, physicalType phosphorusConcentration);



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
		void algaeNaturalMortality(physicalType localTemperature, physicalType localeLimitationProduct, biomassType localPointBiomass);
		void algaeHighTemperatureMortality(physicalType localeTemperature);
		void resourceLimitationStress(physicalType localeLimitationProduct);
		void calculateAlgalCarryingCapacityMortality(biomassType localPointBiomass);
		void algaeSinking(int depthIndex, int columnIndex);
		void algaeSlough(int columnIndex);
	void copyPointersToAnimalDynamics();
};
}



#endif /* FOODWEBMODEL_HPP_ */
