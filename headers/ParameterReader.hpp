/*
 * XMLReader.h
 *
 *  Created on: May 11, 2017
 *      Author: manu_
 */

#ifndef PARAMETERREADER_HPP_
#define PARAMETERREADER_HPP_

#include <string>
#include "TypeDefinitions.hpp"
#include "SimulationModes.hpp"

using namespace std;


namespace FoodWebModel{


class ParameterReader {
public:
	SimulationArguments simArguments;
	ParameterReader();
	~ParameterReader();
//	void readXMLFile(const string&);
	void setSimulationParameters();
	void readSimulationParametersFromFile(const string&);
	void readSimulationParametersFromLine(const std::string& parameterLine);
	void setParameter(const std::string& parameterName, const std::string& parameterValue);
};
}
#endif /* PARAMETERREADER_HPP_ */
