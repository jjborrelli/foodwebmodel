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

using namespace std;


namespace FoodWebModel{


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
