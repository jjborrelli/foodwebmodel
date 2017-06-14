/*
 * XMLReader.cpp
 *
 *  Created on: May 11, 2017
 *      Author: manu_
 */

#include <fstream>
#include <iostream>
#include <vector>
#include "../headers/ParameterReader.hpp"


using namespace std;
/* A function to split strings according to a given delimiter*/
vector<string> generalSplit(const string& str, const string& delim)
{
    vector<string> tokens;
    int prev = 0, pos = 0;
    do
    {
        pos = str.find(delim, prev);
        if (pos == string::npos) pos = str.length();
        string token = str.substr(prev, pos-prev);
        if (!token.empty()) tokens.push_back(token);
        prev = pos + delim.length();
    }
    while (pos < str.length() && prev < str.length());
    return tokens;
}

FoodWebModel::ParameterReader::ParameterReader() {
	// TODO Auto-generated constructor stub

}

FoodWebModel::ParameterReader::~ParameterReader() {
	// TODO Auto-generated destructor stub
}


/* Read the simulation parameters from a text file*/
void FoodWebModel::ParameterReader::readSimulationParametersFromFile(const std::string& parameterFileName){

	/* Open the parameter file*/
	ifstream parametersFile;
	string file_buffer, file_content;
	parametersFile.open(parameterFileName);
	if(parametersFile.is_open()){
		/*Read while there are lines left in the file*/
		std::cout<<"Reading parameters from file "<<parameterFileName<<"."<<endl;
		while(!parametersFile.eof()){
			parametersFile >> file_buffer;
			/*Split the line and set the value associated to the name*/
			vector<string> parameterRead = generalSplit(file_buffer, std::string(";"));
			setParameter(parameterRead[0], parameterRead[1]);

		}
		parametersFile.close();
		std::cout<<"Closed parameter file "<<parameterFileName<<"."<<endl;
	}

}

/*Set pair name-value*/
void FoodWebModel::ParameterReader::setParameter(const std::string& parameterName, const std::string& parameterValue){
	if(!parameterName.compare("DepthRoute"))
		simArguments.depthRoute= parameterValue;
	if(!parameterName.compare("DepthScaleRoute"))
		simArguments.depthScaleRoute= parameterValue;
	if(!parameterName.compare("InitialTemperatureRoute"))
		simArguments.initialTemperatureRoute= parameterValue;
	if(!parameterName.compare("TemperatureRangeRoute"))
		simArguments.temperatureRangeRoute= parameterValue;
	if(!parameterName.compare("OutputAlgaeRoute"))
		simArguments.outputAlgaeRoute= parameterValue;
	if(!parameterName.compare("OutputSloughRoute"))
		simArguments.outputSloughRoute= parameterValue;
	if(!parameterName.compare("OutputGrazerRoute"))
		simArguments.outputGrazerRoute= parameterValue;
	if(!parameterName.compare("OutputParameterRoute"))
		simArguments.outputParameterRoute= parameterValue;
	if(!parameterName.compare("OutputAssertionViolationRoute"))
		simArguments.ouputAssertionViolationRoute=  parameterValue;
	if(!parameterName.compare("InitialAlgaeBiomassRoute"))
		simArguments.initialAlgaeBiomassRoute= parameterValue;
	if(!parameterName.compare("InitialZooplanktonCountRoute"))
		simArguments.initialZooplanktonCountRoute= parameterValue;
	if(!parameterName.compare("LightAtSurfaceRoute"))
		simArguments.lightAtSurfaceRoute= parameterValue;
	if(!parameterName.compare("BiomassBaseDifferential"))
		simArguments.biomassBaseDifferential= parameterValue;
	if(!parameterName.compare("SimulationCycles"))
		simArguments.simulationCycles= atoi(parameterValue.c_str());
	if(!parameterName.compare("PhosphorusConcentrationAtBottom"))
		simArguments.phosphorusConcentrationAtBottom= parameterValue;
	if(!parameterName.compare("ZooplanktonBiomassDepthCenter"))
		simArguments.zooplanktonBiomassDepthCenter= parameterValue;
	if(!parameterName.compare("AlgaeBiomassDifferentialScale"))
		simArguments.algae_biomass_differential_production_scale=  atof(parameterValue.c_str());
	if(!parameterName.compare("AnimalBaseMortality"))
		simArguments.animal_base_mortality_proportion=  atof(parameterValue.c_str());
	if(!parameterName.compare("FilteringRatePerDaphnia"))
		simArguments.filtering_rate_per_daphnia=  atof(parameterValue.c_str());
}

/*Parse additional arguments as a string*/
void FoodWebModel::ParameterReader::readSimulationParametersFromLine(const std::string& parameterLine){
	cout<<"Parsing additional parameters."<<endl;
	/*Parameters are separated by ';'*/
	vector<string> parameterRead = generalSplit(parameterLine, std::string(";"));
	for (int parameterIndex = 0; parameterIndex < parameterRead.size(); ++parameterIndex) {
		/*Parameter name and value are separated by '='*/
		vector<string> parameterAndValue = generalSplit(parameterRead[parameterIndex], std::string("="));
		setParameter(parameterAndValue[0], parameterAndValue[1]);
		cout<<"Parameter "<<parameterAndValue[0]<<" read with value "<<parameterAndValue[1]<<"."<<endl;
	}
}
//
//void FoodWebModel::XMLReader::readXMLFile(const std::string& XMLFileName){
//	xml_document<> doc;
//
//	/* Append the file content for further parsing*/
//	ifstream parametersFile;
//	string file_buffer, file_content;
//	parametersFile.open(XMLFileName);
//	if(parametersFile.is_open()){
//		while(!parametersFile.eof()){
//			parametersFile >> file_buffer;
//			if(!parametersFile.eof()){
//				file_content.append(file_buffer);
//			}
//
//		}
//		try{
//			document.parse<0>((char*)file_content.c_str());
//		}
//			catch(parse_error e){
//			std::cerr<<e.what()<<endl;
//		}
//
//		setSimulationParameters();
//		parametersFile.close();
//	}
//
//}
//
//void FoodWebModel::XMLReader::setSimulationParameters(){
//	xml_node<> *simulationArguments = document.first_node("arguments");
//			simArguments.depthRoute= simulationArguments->first_node("DepthRoute")->value();
//			simArguments.depthScaleRoute= simulationArguments->first_node("DepthScaleRoute")->value();
//			simArguments.initialTemperatureRoute= simulationArguments->first_node("InitialTemperatureRoute")->value();
//			simArguments.temperatureRangeRoute= simulationArguments->first_node("TemperatureRangeRoute")->value();
//			simArguments.outputAlgaeRoute= simulationArguments->first_node("OutputAlgaeRoute")->value();
//			simArguments.outputSloughRoute= simulationArguments->first_node("OutputSloughRoute")->value();
//			simArguments.outputGrazerRoute= simulationArguments->first_node("OutputGrazerRoute")->value();
//			simArguments.initialAlgaeBiomassRoute= simulationArguments->first_node("InitialAlgaeBiomassRoute")->value();
//			simArguments.initialZooplanktonCountRoute= simulationArguments->first_node("InitialZooplanktonCountRoute")->value();
//			simArguments.lightAtSurfaceRoute= simulationArguments->first_node("LightAtSurfaceRoute")->value();
//			simArguments.simulationCycles= atoi(simulationArguments->first_node("SimulationCycles")->value());
//}
