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
		biomassType **initial_biomass;
		physicalType **initial_temperature;
		physicalType depth[MAX_COLUMN_INDEX], temperature_range[MAX_DEPTH_INDEX], depth_scale[MAX_DEPTH_INDEX];
	public:
		ReadProcessedData();
		~ReadProcessedData();
		void readGeophysicalData(const string &depthRoute, const string &depthScaleRoute, const string &initialTemperatureRoute, const string &temperatureRangeRoute, const string& initialBiomassRoute);
	protected:
		vector<physicalType> readValues(const string route);
		void readDepth(const string depthFileRoute);
		void readInitialTemperature(const string& initialTemperatureRoute);
		void readTemperatureRange(const string& temperatureRangeRoute);
		void readDepthScale(const string& depthScaleRoute);
		void readInitialBiomass(const string& biomassRoute);
		void readDataMatrix(const string& fileRoute, double** dataMatrix);
		//int readTemperatureAtSurface(string temperatureAtSurfaceFileRoute);

	};
}


#endif /* READPROCESSEDDATA_HPP_ */
