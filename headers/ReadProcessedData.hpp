/*
 * ReadProcessedData.hpp
 *
 *  Created on: 28 mar. 2017
 *      Author: manu_
 */

#ifndef READPROCESSEDDATA_HPP_
#define READPROCESSEDDATA_HPP_
#include <string>
#include "typeDefinitions.hpp"
#include "FoodWebModel.hpp"

using namespace std;

namespace FoodWebModel{
	friend class FoodWebModel;
	class ReadProcessedData{
	protected:
		physicalType *depth, *temperaturAtSurface;
		int lakeSize;
	public:
		void readGeophysicalData(string depthFileRoute, string temperatureAtSurfaceRoute);
	protected:
		vector<physicalType> readValues(string route);
		int readDepth(string depthFileRoute);
		int readTemperatureAtSurface(string temperatureAtSurfaceFileRoute);

	};
}


#endif /* READPROCESSEDDATA_HPP_ */
