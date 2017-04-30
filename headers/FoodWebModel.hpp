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
		int currentHour;
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
		physicalType localePhotoPeriod,localeLightAtDepth, depthInMeters, turbidity_at_depth, light_at_top, resource_limitation_exponent, light_difference, normalized_light_difference, nutrient_at_depth_exponent, light_normalizer, sigmoid_light_difference, light_at_depth_exponent, temperature_angular_frequency, temperature_sine;
		biomassType biomass_to_depth, high_temperature_mortality, resource_limitation_stress, weighted_resource_limitation_stress, sedimentation_rate;
	public:
		FoodWebModel(const std::string& depthRoute, const std::string& depthScaleRoute, const std::string& initialTemperatureRoute, const std::string& temperatureRangeRoute, const string& initialBiomassRoute, const string& lightAtSurfaceRoute);
		int simulate(int cycles,  const std::string& outputFileName, const std::string outputSloughRoute);
		void initializeParameters();

	protected:
		void step();
		biomassType biomassDifferential(int depthIndex, int columnIndex, bool periPhyton);
		physicalType lightAtDepth(int depthIndex, int columnIndex);
		biomassType sumPhytoBiomassToDepth(int depthIndex, int columnIndex);
		biomassType photoSynthesis(biomassType localPointBiomass, physicalType localeLimitationProduct, bool periPhyton);
		physicalType productionLimit(physicalType localeLightAllowance, bool periPhyton);
		void setBathymetricParameters();
		biomassType respiration(biomassType localPointBiomass, physicalType localTemperature);
		biomassType excretion(biomassType localePhotoSynthesis, physicalType localeLightAllowance);
		biomassType naturalMortality(physicalType localTemperature, physicalType localeLimitationProduct, biomassType localPointBiomass);
		biomassType highTemperatureMortality(physicalType localeTemperature);
		biomassType resourceLimitationStress(physicalType llocaleLimitationProduct);
		biomassType sinking(int depthIndex, int columnIndex);
		biomassType slough(int columnIndex);
		physicalType calculateTemperature(int depthIndex, int columnIndex);
		physicalType lightAllowance(int depthIndex, int columnIndex);
		physicalType photoPeriod();
		physicalType getLightAtTop();
		physicalType nutrientConcentrationAtDepth(int depthIndex, int columnIndex);
		biomassType calculateNutrientLimitation(physicalType localeNutrientConcentration);
		void calculatePhysicalLakeDescriptors();
		void initializePointers();
		void printSimulationMode();

	};
}



#endif /* FOODWEBMODEL_HPP_ */
