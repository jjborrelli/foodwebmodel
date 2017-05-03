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

		/*A vector to reference the calculated biomass*/

		double fractionInEuphoticZone, ZEuphotic, ZMean, ZMax, P;

		/*Max possible column index (X axis)*/
		//int  maxColumn;

		/*Buffer line to write simulation results*/
		std::ostringstream lineBuffer, stepBuffer, sloughBuffer;
		string commaString = string(", ");
		/*Class methods*/
	private:

		/* Physical attributes*/
		physicalType locale_photo_period,light_at_depth, depthInMeters, turbidity_at_depth, light_at_top, resource_limitation_exponent, light_difference, normalized_light_difference, nutrient_at_depth_exponent, light_normalizer, light_allowance, light_at_depth_exponent, temperature_angular_frequency, temperature_sine, nutrient_limitation, nutrient_concentration;

		/* Algae attributes*/
		biomassType biomass_to_depth, high_temperature_mortality, resource_limitation_stress, weighted_resource_limitation_stress, sedimentation_rate;

		/* Algae biomass components*/

		biomassType photosynthesis_value, algae_respiration_value, algae_excretion_value, algae_sinking_value, algae_slough_value, algae_natural_mortality;
		/* Zooplankton attributes*/
		biomassType locale_grazing, locale_defecation, base_zooplankton_respiration, salinity_corrected_zooplankton_respiration, basal_respiration, active_respiration_exponent, active_respiration_factor, active_respiration, metabolic_respiration, grazer_excretion_loss, animal_base_mortality, animal_temperature_mortality, animal_temp_independent_mortality, salinity_effect, salinity_exponent, salinity_mortality;
	public:
		FoodWebModel(const std::string& depthRoute, const std::string& depthScaleRoute, const std::string& initialTemperatureRoute, const std::string& temperatureRangeRoute, const string& initialBiomassRoute, const string& lightAtSurfaceRoute);
		int simulate(int cycles,  const std::string& outputFileName, const std::string outputSloughRoute);
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
		void nutrientConcentrationAtDepth(int depthIndex, int columnIndex);
		void calculateNutrientLimitation();
		void calculatePhysicalLakeDescriptors();
		void setBathymetricParameters();

		/* Algae biomass*/
		biomassType biomassDifferential(int depthIndex, int columnIndex, bool periPhyton);
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

		biomassType foodConsumptionRate(biomassType zooBiomass, biomassType phytoBiomass);
		biomassType defecation(biomassType grazing);
		biomassType animalRespiration(biomassType zooBiomass, biomassType consumptionBiomass, biomassType productionBiomass, physicalType localeTemperature);
		biomassType basalRespiration(biomassType zooBiomass);
		biomassType activeRespiration(biomassType zooBiomass, physicalType localeTemperature);
		biomassType metabolicFoodConsumption(biomassType productionBiomass, biomassType consumptionBiomass);
		biomassType animalExcretion(biomassType localeRespiration);
		biomassType animalMortality(biomassType localeRespiration, physicalType localeTemperature, physicalType localeSalinityConcentration);
		biomassType animalBaseMortality(physicalType localeTemperature, biomassType localeBiomass);
		biomassType animalTemperatureMortality(physicalType localeTemperature);
		physicalType salinityEffect(physicalType salinityConcentration);
		biomassType salinityMortality(biomassType localeBiomass, physicalType localeSalinityConcentration);

	};
}



#endif /* FOODWEBMODEL_HPP_ */
