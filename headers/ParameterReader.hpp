/*
 * XMLReader.h
 *
 *  Created on: May 11, 2017
 *      Author: manu_
 */

#ifndef PARAMETERREADER_HPP_
#define PARAMETERREADER_HPP_

#include <string>

using namespace std;


namespace FoodWebModel{
typedef struct {
	string depthRoute,
	depthScaleRoute,
	initialTemperatureRoute,
	temperatureRangeRoute,
	outputAlgaeRoute,
	outputSloughRoute,
	outputGrazerRoute,
	initialAlgaeBiomassRoute,
	initialZooplanktonCountRoute,
	lightAtSurfaceRoute;
	int simulationCycles;
} SimulationArguments;

class ParameterReader {
public:
	SimulationArguments simArguments;
	ParameterReader();
	~ParameterReader();
//	void readXMLFile(const string&);
	void setSimulationParameters();
	void readSimulationParameters(const string&);
	void setParameter(const std::string& parameterName, const std::string& parameterValue);
};
}
#endif /* PARAMETERREADER_HPP_ */
