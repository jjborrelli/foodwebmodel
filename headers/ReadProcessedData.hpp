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
		physicalType depth[MAX_COLUMN_INDEX], temperature_initial[MAX_DEPTH_INDEX][MAX_COLUMN_INDEX], temperature_range[MAX_DEPTH_INDEX], depth_scale[MAX_COLUMN_INDEX];
	public:
		void readGeophysicalData(string &depthRoute, string &depthScaleRoute, string &initialTemperatureRoute, string &temperatureRangeRoute);
	protected:
		vector<physicalType> readValues(string route);
		void readDepth(string depthFileRoute);
		void readInitialTemperature(string& initialTemperatureRoute);
		void readTemperatureRange(string& temperatureRangeRoute);
		void readDepthScale(string& depthScaleRoute);
		//int readTemperatureAtSurface(string temperatureAtSurfaceFileRoute);

	};
}


#endif /* READPROCESSEDDATA_HPP_ */
