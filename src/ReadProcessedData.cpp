/*
 * ReadProcessedData.cpp
 *
 *  Created on: 28 mar. 2017
 *      Author: manu_
 */


#include "../headers/ReadProcessedData.hpp"
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <vector>

/*
 * Read the file encoding for the lake depth
 */

vector<physicalType> FoodWebModel::ReadProcessedData::readValues(string dataRoute){
	ifstream dataFile;
	string readLine;
	/* Store the file content in a vector*/
	vector<physicalType> localeVector;
	dataFile.open(dataRoute.c_str());
	/* Read until there are no more lines*/
	  if (dataFile.is_open())
	  {
		  while ( getline (dataFile,readLine) )
		     {
			  localeVector.push_back(std::stod(readLine.c_str()));
		     }
		  dataFile.close();
	  } else{
		  /* If the file could not be opened, report an error*/
		  cerr << "File "<< dataRoute<<" could not be opened";
	  }
	 return localeVector;

}

int FoodWebModel::ReadProcessedData::readTemperatureAtSurface(string temperatureRoute){
	/* Read temperature as a vector*/
	vector<physicalType> temperatureAtSurfaceVector = readValues(temperatureRoute);
	int vectorSize=temperatureAtSurfaceVector.size();

	/* Copy values to the temperature array*/
	temperaturAtSurface = new physicalType[vectorSize];
	for(int i=0; i<vectorSize; i++)
		temperaturAtSurface[i]=temperatureAtSurfaceVector[i];
	return vectorSize;
}


int FoodWebModel::ReadProcessedData::readDepth(string depthRoute){

	/* Read depth as a vector*/
	vector<physicalType> depthVector = readValues(depthRoute);
	int vectorSize=depthVector.size();

	/* Copy values to the depth array*/
	depth = new physicalType[vectorSize];
	for(int i=0; i<vectorSize; i++)
		depth[i]=depthVector[i];
	return vectorSize;
}

void FoodWebModel::ReadProcessedData::readGeophysicalData(string depthRoute, string temperatureRoute){
	/*Read depth and temperature data*/
	readDepth(depthRoute);
	this->lakeSize = readTemperatureAtSurface(temperatureRoute);
}
