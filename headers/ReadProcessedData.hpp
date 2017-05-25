/*
 * ReadProcessedData.hpp
 *
 *  Created on: 28 mar. 2017
 *      Author: manu_
 */

#ifndef READPROCESSEDDATA_HPP_
#define READPROCESSEDDATA_HPP_
#include <string>
#include <vector>
#include "TypeDefinitions.hpp"
#include "ModelConstants.hpp"

using namespace std;

namespace FoodWebModel{

	class ReadProcessedData{
		friend class FoodWebModel;
	protected:
		biomassType **initial_algae_biomass;
		zooplanktonCountType **initial_grazer_count;
		physicalType **initial_temperature;
		biomassType baseBiomassDifferential[MAX_DEPTH_INDEX];
		physicalType depth[MAX_COLUMN_INDEX], temperature_range[MAX_DEPTH_INDEX], depth_scale[MAX_DEPTH_INDEX], hourlyLightAtSurface[HOURS_PER_DAY], *phosphorusConcentrationAtBottom;
		unsigned int simulationCycles;
	public:
		ReadProcessedData();
		~ReadProcessedData();
		void readModelData(const SimulationArguments& simArguments);
	protected:
		template<typename T>
		void readValues(const string& route, T* readArray);
		void readDepth(const string& depthFileRoute);
		void readInitialTemperature(const string& initialTemperatureRoute);
		void readTemperatureRange(const string& temperatureRangeRoute);
		void readDepthScale(const string& depthScaleRoute);
		void readInitialAlgaeBiomass(const string& initialAlgaeBiomassRoute);
		void readBaseAlgaeBiomassDifferential(const string& biomassDifferentialRoute);
		void readInitialZooplanktonCount(const string& biomassRoute);
		void readPhosphorusConcentrationAtBottom(const string& phosphorusConcentrationAtBottomRoute);
		template<typename T>
		void readDataMatrix(const string& fileRoute, T** dataMatrix);
		void readLightAtSurface(const string& lightRoute);
		//int readTemperatureAtSurface(string temperatureAtSurfaceFileRoute);

	};
}


#endif /* READPROCESSEDDATA_HPP_ */
