/*
 * FoodWebModel.hpp
 *
 *  Created on: 13 mar. 2017
 *      Author: manu_
 */


#ifndef FOODWEBMODEL_HPP_
#define FOODWEBMODEL_HPP_

#include <math.h>
#include <string>
#include "TypeDefinitions.hpp"
#include "ModelConstants.hpp"
#include "ReadProcessedData.hpp"



namespace FoodWebModel {



	class FoodWebModel {


		/*Class attributes*/
	protected:
		ReadProcessedData readProcessedData;
		int currentHour;
		physicalType temperature[MAX_DEPTH_INDEX][MAX_COLUMN_INDEX], temperature_initial[MAX_DEPTH_INDEX][MAX_COLUMN_INDEX];
		physicalType depthVector[MAX_COLUMN_INDEX], temperature_range[MAX_DEPTH_INDEX], indexToDepth[MAX_DEPTH_INDEX];

		/*Phytoplankton biomass, periphyton biomass and temperature*/
		biomassType phytoBiomass[MAX_DEPTH_INDEX][MAX_COLUMN_INDEX], periBiomass[MAX_DEPTH_INDEX][MAX_COLUMN_INDEX], priorPhytoBiomass[MAX_DEPTH_INDEX][MAX_COLUMN_INDEX], priorPeriBiomass[MAX_DEPTH_INDEX][MAX_COLUMN_INDEX], localBiomass[MAX_DEPTH_INDEX][MAX_COLUMN_INDEX];

		/*A vector to reference the calculated biomass*/

		double fractionInEuphoticZone, ZEuphotic, ZMean, ZMax, P;

		/*Max possible column index (X axis)*/
		//int  maxColumn;

		/*Buffer line to write simulation results*/
		std::string lineBuffer;
		/*Class methods*/
	public:
		FoodWebModel(std::string& depthRoute, std::string& depthScaleRoute, std::string& initialTemperatureRoute, std::string& temperatureRangeRoute);
		int simulate(int cycles,  std::string& outputFileName);
		void initializeParameters();

	protected:
		void step();
		biomassType biomassDifferential(int depthIndex, int column, bool periPhyton);
		physicalType lightAtDepth(int depthIndex, int column);
		biomassType sumPhytoBiomassToDepth(int depthIndex, int column);
		biomassType photoSynthesis(biomassType localPointBiomass, physicalType localeLightAllowance, bool periPhyton);
		physicalType productionLimit(physicalType localeLightAllowance, bool periPhyton);
		void setBathymetricParameters();
		biomassType respiration(biomassType localPointBiomass, physicalType localTemperature);
		biomassType excretion(biomassType localePhotoSynthesis, physicalType localeLightAllowance);
		biomassType naturalMortality(physicalType localTemperature, physicalType localeLightAllowance, biomassType localPointBiomass);
		biomassType highTemperatureMortality(physicalType localeTemperature);
		biomassType resourceLimitationStress(physicalType localeLightAllowance);
		biomassType sinking(int depthIndex, int columnIndex);
		biomassType slough(int depthIndex, int columnIndex);
		physicalType calculateTemperature(int depthIndex, int columnIndex, int timeStep);
		physicalType lightAllowance(int depthIndex, int columnIndex);
		physicalType photoPeriod();
		void calculatePhysicalLakeDescriptors();
		void initializePointers();

	};
}



#endif /* FOODWEBMODEL_HPP_ */
