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
#include "TypeDefinitions.hpp"
#include "ModelConstants.hpp"
#include "ReadProcessedData.hpp"



namespace FoodWebModel {



	class FoodWebModel {


		/*Class attributes*/
	protected:
		ReadProcessedData readProcessedData;
		int current_hour;
		physicalType temperature[MAX_DEPTH_INDEX][MAX_COLUMN_INDEX], initial_temperature[MAX_DEPTH_INDEX][MAX_COLUMN_INDEX];
		physicalType depthVector[MAX_COLUMN_INDEX], temperature_range[MAX_DEPTH_INDEX], indexToDepth[MAX_DEPTH_INDEX], hourlyLightAtSurface[HOURS_PER_DAY];
		int maxDepthIndex[MAX_COLUMN_INDEX];

		/*Phytoplankton biomass, periphyton biomass and temperature*/
		biomassType phytoBiomass[MAX_DEPTH_INDEX][MAX_COLUMN_INDEX], periBiomass[MAX_COLUMN_INDEX], priorPhytoBiomass[MAX_DEPTH_INDEX][MAX_COLUMN_INDEX], priorPeriBiomass[MAX_COLUMN_INDEX], phytoDifferential[MAX_DEPTH_INDEX][MAX_COLUMN_INDEX], periDifferential[MAX_COLUMN_INDEX], localBiomass[MAX_DEPTH_INDEX][MAX_COLUMN_INDEX], verticalMigratedPhytoBiomass[MAX_DEPTH_INDEX][MAX_COLUMN_INDEX], verticalMigratedPeriBiomass[MAX_COLUMN_INDEX], sloughPhytoBiomass[MAX_DEPTH_INDEX][MAX_COLUMN_INDEX], phytoBiomassDifferential[MAX_DEPTH_INDEX][MAX_COLUMN_INDEX], periBiomassDifferential[MAX_COLUMN_INDEX];

		/* Grazer biomass*/
		biomassType zooplanktonBiomass[MAX_DEPTH_INDEX][MAX_COLUMN_INDEX], bottomFeederBiomass[MAX_COLUMN_INDEX];
		biomassType priorZooplanktonBiomass[MAX_DEPTH_INDEX][MAX_COLUMN_INDEX], priorBottomFeederBiomass[MAX_COLUMN_INDEX];
		biomassType used_grazing;
		/* Grazer count*/
		zooplanktonCountType zooplanktonCount[MAX_DEPTH_INDEX][MAX_COLUMN_INDEX], bottomFeederCount[MAX_COLUMN_INDEX], priorZooplanktonCount[MAX_DEPTH_INDEX][MAX_COLUMN_INDEX], priorBottomFeederCount[MAX_COLUMN_INDEX];
		/*A vector to reference the calculated biomass*/

		double fractionInEuphoticZone, ZEuphotic, ZMean, ZMax, P;

		/*Max possible column index (X axis)*/
		//int  maxColumn;

		/*Buffer line to write simulation results*/
		std::ostringstream lineBuffer, algaeBuffer, sloughBuffer, grazerBuffer;
		string commaString = string(", ");
		/*Class methods*/
	private:

		/* Physical attributes*/
		physicalType locale_photo_period,light_at_depth, depthInMeters, turbidity_at_depth, light_at_top, resource_limitation_exponent, light_difference, normalized_light_difference, chemical_at_depth_exponent, light_normalizer, light_allowance, light_at_depth_exponent, temperature_angular_frequency, temperature_sine, nutrient_limitation, chemical_concentration;

		/* Algae attributes*/
		biomassType biomass_to_depth, high_temperature_mortality, resource_limitation_stress, weighted_resource_limitation_stress, sedimentation_rate;

		/* Algae biomass components*/

		biomassType photosynthesis_value, algae_respiration_value, algae_excretion_value, algae_sinking_value, algae_slough_value, algae_natural_mortality;
		/* Zooplankton attributes*/
		biomassType grazing_per_individual, locale_grazing, locale_defecation, base_zooplankton_respiration, salinity_corrected_zooplankton_respiration, basal_respiration, active_respiration_exponent, active_respiration_factor, active_respiration, metabolic_respiration, grazer_excretion_loss, animal_base_mortality, animal_temperature_mortality, animal_temp_independent_mortality, salinity_effect, salinity_mortality, locale_grazing_salt_adjusted, animal_mortality;
		physicalType salinity_exponent, stroganov_adjustment;

	public:
		FoodWebModel(const std::string& depthRoute, const std::string& depthScaleRoute, const std::string& initialTemperatureRoute, const std::string& temperatureRangeRoute, const string& initialAlgaeBiomassRoute, const string& initialZooplanktonCountRoute, const string& lightAtSurfaceRoute);
		int simulate(int cycles,  const std::string& outputAlgaeFileName, const std::string& outputSloughFileName, const std::string& outputGrazerFileName);
		void initializeParameters();

	protected:

		/* Main functions*/
		void step();
		void initializePointers();
		void printSimulationMode();

		/* Physical descriptors*/
		void lightAtDepth(int depthIndex, int columnIndex);
		void calculateTemperature(int depthIndex, int columnIndex);
		void lightAllowance(int depthIndex, int columnIndex);
		void photoPeriod();
		void calculateLightAtTop();
		void phosphorusConcentrationAtDepth(int depthIndex, int columnIndex);
		void saltConcentrationAtDepth(int depthIndex, int columnIndex);
		void chemicalConcentrationAtDepth(int depthIndex, int columnIndex, physicalType concentrationAtBottom);
		void calculateNutrientLimitation();
		void calculatePhysicalLakeDescriptors();
		void setBathymetricParameters();

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
		void algaeSinking(int depthIndex, int columnIndex);
		void algaeSlough(int columnIndex);

		/* Grazers biomass*/
		void updateZooplanktonBiomass();
		biomassType grazerBiomassDifferential(int depthIndex, int columnIndex, bool bottomFeeder);
		void foodConsumptionRate(int depthIndex, int columnIndex, bool bottomFeeder);
		void defecation();
		void animalRespiration(biomassType zooBiomass, physicalType localeTemperature);
		biomassType basalRespiration(biomassType zooBiomass);
		biomassType activeRespiration(biomassType zooBiomass, physicalType localeTemperature);
		biomassType metabolicFoodConsumption();
		void animalExcretion(biomassType localeRespiration);
		void animalMortality(biomassType localeRespiration, physicalType localeTemperature, physicalType localeSalinityConcentration);
		void animalBaseMortality(physicalType localeTemperature, biomassType localeBiomass);
		void animalTemperatureMortality(physicalType localeTemperature, biomassType localeBiomass);
		void salinityEffect();
		void salinityMortality(biomassType localeBiomass);
		void stroganovApproximation(physicalType localeTemperature);

	};
}



#endif /* FOODWEBMODEL_HPP_ */
