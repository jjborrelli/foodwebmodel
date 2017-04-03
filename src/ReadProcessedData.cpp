/*
 * ReadProcessedData.cpp
 *
 *  Created on: 28 mar. 2017
 *      Author: manu_
 */


#include "../headers/ReadProcessedData.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <vector>

/*
 * Read the file encoding for the lake depth
 */

/* An auxiliary function to split a string*/

vector<double> split(string &str){
	    string buf; // Have a buffer string
	    stringstream ss(str); // Insert the string into a stream

	    vector<double> tokens; // Create vector to hold our words

	    while (ss >> buf)
	        tokens.push_back(atof(buf.c_str()));
	 return tokens;
}


void FoodWebModel::ReadProcessedData::readInitialTemperature(string& initialTemperatureRoute){
	ifstream dataFile;
	string readLine;
	dataFile.open(initialTemperatureRoute.c_str());
	/* Read until there are no more lines*/
	  if (dataFile.is_open())
	  {
		  int depth_index=0;
		  while ( getline (dataFile,readLine) )
		     {
			  /* For each line, split the string and fill in the initial temperature values*/
			  vector<double> initial_temperature_at_depth = split(readLine);
			  for(int colIndex=0; colIndex<initial_temperature_at_depth.size(); colIndex++){
				  temperature_initial[depth_index++][colIndex]=initial_temperature_at_depth[colIndex];
			  }
		     }
		  dataFile.close();
	  } else{
		  /* If the file could not be opened, report an error*/
		  cerr << "File "<< initialTemperatureRoute<<" could not be opened";
	  }

}

void FoodWebModel::ReadProcessedData::readTemperatureRange(string& temperatureRangeRoute){
	ifstream dataFile;
	string readLine;
	/* Store the file content in a vector*/
	dataFile.open(temperatureRangeRoute.c_str());
	/* Read until there are no more lines*/
	  if (dataFile.is_open())
	  {
		  int depth_index=0;
		  while ( getline (dataFile,readLine) )
		     {
			  /* The temperature range is the fourth column*/
			  this->temperature_range[depth_index] = split(readLine)[3];

		     }
		  dataFile.close();
	  } else{
		  /* If the file could not be opened, report an error*/
		  cerr << "File "<< temperatureRangeRoute<<" could not be opened";
	  }

}

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


int FoodWebModel::ReadProcessedData::readDepth(string depthRoute){

	/* Read depth as a vector*/
	vector<physicalType> depthVector = readValues(depthRoute);
	int vectorSize=depthVector.size();

	/* Copy values to the depth array*/
	for(int i=0; i<vectorSize; i++)
		depth[i]=depthVector[i];
	return vectorSize;
}

void FoodWebModel::ReadProcessedData::readGeophysicalData(string &depthRoute, string &initialTemperatureRoute, string &temperatureRangeRoute){
	/*Read depth and temperature data*/
	readDepth(depthRoute);
	readInitialTemperature(initialTemperatureRoute);
	readTemperatureRange(temperatureRangeRoute);
	//this->lakeSize = readTemperatureAtSurface(temperatureRoute);
}
