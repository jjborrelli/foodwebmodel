/*
 * ReadProcessedData.cpp
 *
 *  Created on: 28 mar. 2017
 *      Author: manu_
 */


#include "../headers/ReadProcessedData.hpp"
#include "../headers/AuxFunctions.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>

/*
 * Read the file encoding for the lake depth
 */

/* An auxiliary function to split a string*/



void FoodWebModel::ReadProcessedData::readInitialTemperature(const string& initialTemperatureRoute){
	cout<<"Reading initial temperature from file: "<<initialTemperatureRoute<<endl;
	readDataMatrix(initialTemperatureRoute, initial_temperature);
	cout<<"Initial temperature read."<<endl;



}

void FoodWebModel::ReadProcessedData::readTemperatureRange(const string& temperatureRangeRoute){
	ifstream dataFile;
	string readLine;
	cout<<"Reading temperature range from file: "<<temperatureRangeRoute<<endl;
	/* Store the file content in a vector*/
	dataFile.open(temperatureRangeRoute.c_str());
	/* Read until there are no more lines*/
	  if (dataFile.is_open())
	  {
		  int depth_index=0;
		  while ( getline (dataFile,readLine,'\n') )
		     {
			  /* The temperature range is the fourth column*/
			  this->temperature_range[depth_index++] = atof(split(readLine)[3].c_str());
		     }
		  dataFile.close();
		  cout<<"Temperature range read."<<endl;
	  } else{
		  /* If the file could not be opened, report an error*/
		  cerr << "File "<< temperatureRangeRoute<<" could not be opened."<<endl;
	  }

}

template<typename T>
void FoodWebModel::ReadProcessedData::readValues(const string& dataRoute, T* readArray, unsigned int readLimit, bool plotReadParameter){
	ifstream dataFile;
	string readLine;
	/* Store the file content in a vector*/
	vector<physicalType> localeVector;
	dataFile.open(dataRoute.c_str());
	int arrayIndex=0;
	/* Read until there are no more lines*/
	  if (dataFile.is_open())
	  {
     cout<<"File "<<dataRoute<<" successfully open."<<endl;
		  while ( getline (dataFile,readLine,'\n')&&(readLimit<0||arrayIndex<readLimit) )
		     {
			  if(plotReadParameter){
				  cout<<"Reading parameter index: "<<arrayIndex<<"."<<endl;
			  }
			  readArray[arrayIndex++]=std::stod(readLine.c_str());
		     }
		  dataFile.close();
	  } else{
		  /* If the file could not be opened, report an error*/
		  cerr << "File "<< dataRoute<<" could not be opened."<<endl;
	  }

}

//int FoodWebModel::ReadProcessedData::readTemperatureAtSurface(string temperatureRoute){
//	/* Read temperature as a vector*/
//	vector<physicalType> temperatureAtSurfaceVector = readValues(temperatureRoute);
//	int vectorSize=temperatureAtSurfaceVector.size();
//
//	/* Copy values to the temperature array*/
//	temperaturAtSurface = new physicalType[vectorSize];
//	for(int i=0; i<vectorSize; i++)
//		temperaturAtSurface[i]=temperatureAtSurfaceVector[i];
//	return vectorSize;
//}


void FoodWebModel::ReadProcessedData::readDepth(const string& depthRoute){

	/* Read depth as a vector*/
	cout<<"Reading lake depth from file: "<<depthRoute<<endl;
	readValues<physicalType>(depthRoute, depth, MAX_COLUMN_INDEX);
}

void FoodWebModel::ReadProcessedData::readLightAtSurface(const string& lightRoute){

	/* Read light as surface as a vector*/
	cout<<"Reading hourly light at surface from file: "<<lightRoute<<endl;
	readValues<physicalType>(lightRoute, hourlyLightAtSurface, MAX_COLUMN_INDEX);
}

void FoodWebModel::ReadProcessedData::readModelData(const SimulationArguments& simArguments){
	/*Read depth and temperature data*/
	this->simulationCycles=simArguments.simulationCycles;
	readDepthScale(simArguments.depthScaleRoute);
	readDepth(simArguments.depthRoute);
	readInitialTemperature(simArguments.initialTemperatureRoute);
	readTemperatureRange(simArguments.temperatureRangeRoute);
	readInitialAlgaeBiomass(simArguments.initialAlgaeBiomassRoute);
	readInitialZooplanktonCount(simArguments.initialZooplanktonCountRoute);
	readBaseAlgaeBiomassDifferential(simArguments.biomassBaseDifferentialRoute);
	readLightAtSurface(simArguments.lightAtSurfaceRoute);
	readPhosphorusConcentrationAtBottom(simArguments.phosphorusConcentrationAtBottomRoute);
	readZooplanktonBiomassCenterDifferencePerDepth(simArguments.zooplanktonBiomassDepthCenterRoute);
	//this->lakeSize = readTemperatureAtSurface(temperatureRoute);
}
void FoodWebModel::ReadProcessedData::readDepthScale(const string& depthScaleRoute){
	/* Read depth as a vector*/
	cout<<"Reading lake index/depth in meters scale from file: "<<depthScaleRoute<<endl;
	readValues<physicalType>(depthScaleRoute, depth_scale, MAX_DEPTH_INDEX);
}

template<typename T>
void FoodWebModel::ReadProcessedData::readDataMatrix(const string& fileRoute, T** dataMatrix){
	ifstream dataFile;
		string readLine;
		dataFile.open(fileRoute.c_str());
		/* Read until there are no more lines*/
		  if (dataFile.is_open())
		  {
			  int depth_index=0;
			  while ( getline (dataFile,readLine,'\n') )
			     {
				  /* For each line, split the string and fill in the parsed parameters*/

//				  cout<<"Reading line "<<depth_index<<" with content"<<readLine<<"."<<endl;
				  vector<string> vector_data = split(readLine);
				  for(int colIndex=0; colIndex<MAX_COLUMN_INDEX; colIndex++){
					  dataMatrix[depth_index][colIndex]=atof(vector_data[colIndex].c_str());
				  }
				  depth_index++;
			     }
			  dataFile.close();
		  } else{
			  /* If the file could not be opened, report an error*/
			  cerr << "File "<< fileRoute<<" could not be opened."<<endl;
		  }
}

void FoodWebModel::ReadProcessedData::readInitialAlgaeBiomass(const string& initialAlgaeBiomassRoute){
	cout<<"Reading initial algae biomass from file: "<<initialAlgaeBiomassRoute<<endl;
	readDataMatrix<biomassType>(initialAlgaeBiomassRoute, initial_algae_biomass);
	cout<<"Initial algae biomass read."<<endl;
}


/* Constructor and destructor to allocate biomass and temperature*/
FoodWebModel::ReadProcessedData::ReadProcessedData(){
	initial_algae_biomass =new biomassType*[MAX_DEPTH_INDEX];
	initial_temperature = new physicalType*[MAX_DEPTH_INDEX];
	initial_grazer_count = new zooplanktonCountType*[MAX_DEPTH_INDEX];
	for(int i=0;i<MAX_DEPTH_INDEX; i++){
		initial_algae_biomass[i] = new biomassType[MAX_COLUMN_INDEX];
		initial_temperature[i] = new physicalType[MAX_COLUMN_INDEX];
		initial_grazer_count[i]= new zooplanktonCountType[MAX_COLUMN_INDEX];
	}
}
FoodWebModel::ReadProcessedData::~ReadProcessedData(){
	for(int i=0;i<MAX_DEPTH_INDEX; i++){
		delete initial_algae_biomass[i];
		delete initial_temperature[i];
		delete initial_grazer_count[i];
	}
	delete initial_algae_biomass;
	delete initial_temperature;
	delete initial_grazer_count;
}


void FoodWebModel::ReadProcessedData::readInitialZooplanktonCount(const string& grazerCountRoute){
	cout<<"Reading initial grazer count from file: "<<grazerCountRoute<<endl;
	readDataMatrix<zooplanktonCountType>(grazerCountRoute, initial_grazer_count);
	cout<<"Initial grazer count read."<<endl;
}

void FoodWebModel::ReadProcessedData::readBaseAlgaeBiomassDifferential(const string& biomassDifferentialRoute){
	cout<<"Reading base algae biomass differential from file: "<<biomassDifferentialRoute<<endl;
	readValues<physicalType>(biomassDifferentialRoute, baseBiomassDifferential, MAX_DEPTH_INDEX);
	cout<<"Base algae biomass read."<<endl;
}

void FoodWebModel::ReadProcessedData::readPhosphorusConcentrationAtBottom(const string& phosphorusConcentrationAtBottomRoute){
	cout<<"Reading phosphorus concentration at bottom from file: "<<phosphorusConcentrationAtBottomRoute<<endl;
	phosphorusConcentrationAtBottom = new physicalType[this->simulationCycles];
	cout<<"Allocated phosphorus concentration at bottom with size "<<this->simulationCycles<<endl;
	readValues<physicalType>(phosphorusConcentrationAtBottomRoute, phosphorusConcentrationAtBottom, this->simulationCycles);
	cout<<"Phosphorus concentration at bottom read."<<endl;
}


void FoodWebModel::ReadProcessedData::readZooplanktonBiomassCenterDifferencePerDepth(const string& zooplanktonBiomassCenterDifferencePerDepthRoute){
	cout<<"Reading zooplankton biomass center difference per depth at hour from file: "<<zooplanktonBiomassCenterDifferencePerDepthRoute<<endl;
	readValues<physicalType>(zooplanktonBiomassCenterDifferencePerDepthRoute, zooplanktonBiomassCenterDifferencePerDepth, HOURS_PER_DAY);
	cout<<"Zooplankton biomass center difference per depth at hour read."<<endl;
}
