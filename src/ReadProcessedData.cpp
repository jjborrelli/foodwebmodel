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
		  while ( getline (dataFile,readLine) )
		     {
			  /* The temperature range is the fourth column*/
			  this->temperature_range[depth_index++] = split(readLine)[3];

		     }
		  dataFile.close();
		  cout<<"Temperature range read."<<endl;
	  } else{
		  /* If the file could not be opened, report an error*/
		  cerr << "File "<< temperatureRangeRoute<<" could not be opened";
	  }

}

vector<physicalType> FoodWebModel::ReadProcessedData::readValues(const string dataRoute){
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


void FoodWebModel::ReadProcessedData::readDepth(const string depthRoute){

	/* Read depth as a vector*/
	cout<<"Reading lake depth from file: "<<depthRoute<<endl;
	vector<physicalType> depthVector = readValues(depthRoute);

	/* Copy values to the depth array*/
	for(int i=0; i<MAX_COLUMN_INDEX; i++)
		depth[i]=depthVector[i];
	cout<<"Lake depth read."<<endl;
}

void FoodWebModel::ReadProcessedData::readGeophysicalData(const string &depthRoute, const string &depthScaleRoute, const string &initialTemperatureRoute, const string &temperatureRangeRoute, const string &initialBiomassRoute){
	/*Read depth and temperature data*/
	readDepthScale(depthScaleRoute);
	readDepth(depthRoute);
	readInitialTemperature(initialTemperatureRoute);
	readTemperatureRange(temperatureRangeRoute);
	readInitialBiomass(initialBiomassRoute);
	//this->lakeSize = readTemperatureAtSurface(temperatureRoute);
}
void FoodWebModel::ReadProcessedData::readDepthScale(const string& depthScaleRoute){
	/* Read depth as a vector*/
	cout<<"Reading lake index/depth in meters scale from file: "<<depthScaleRoute<<endl;
	vector<physicalType> depthVector = readValues(depthScaleRoute);



	/* Copy values to the depth array*/
	for(int i=0; i<MAX_DEPTH_INDEX; i++)
		depth_scale[i]=depthVector[i];
	cout<<"Index/depth in meters scale read."<<endl;
}

void FoodWebModel::ReadProcessedData::readDataMatrix(const string& fileRoute, double** dataMatrix){
	ifstream dataFile;
		string readLine;
		dataFile.open(fileRoute.c_str());
		/* Read until there are no more lines*/
		  if (dataFile.is_open())
		  {
			  int depth_index=0;
			  while ( getline (dataFile,readLine) )
			     {
				  /* For each line, split the string and fill in the initial temperature values*/
				  vector<double> initial_temperature_at_depth = split(readLine);
				  for(int colIndex=0; colIndex<MAX_COLUMN_INDEX; colIndex++){
					  dataMatrix[depth_index][colIndex]=initial_temperature_at_depth[colIndex];
				  }
				  depth_index++;
			     }
			  dataFile.close();
		  } else{
			  /* If the file could not be opened, report an error*/
			  cerr << "File "<< fileRoute<<" could not be opened";
		  }
}

void FoodWebModel::ReadProcessedData::readInitialBiomass(const string& biomassRoute){
	/* Read depth as a vector*/
	cout<<"Reading initial biomass from file: "<<biomassRoute<<endl;
	readDataMatrix(biomassRoute, initial_biomass);
	cout<<"Initial biomass read."<<endl;
}


/* Constructor and destructor to allocate biomass and temperature*/
FoodWebModel::ReadProcessedData::ReadProcessedData(){
	initial_biomass =new biomassType*[MAX_DEPTH_INDEX];
	initial_temperature = new physicalType*[MAX_DEPTH_INDEX];
	for(int i=0;i<MAX_DEPTH_INDEX; i++){
		initial_biomass[i] = new biomassType[MAX_COLUMN_INDEX];
		initial_temperature[i] = new physicalType[MAX_COLUMN_INDEX];
	}
}
FoodWebModel::ReadProcessedData::~ReadProcessedData(){
	for(int i=0;i<MAX_DEPTH_INDEX; i++){
		delete initial_biomass[i];
		delete initial_temperature[i];
	}
	delete initial_biomass;
	delete initial_temperature;
}
