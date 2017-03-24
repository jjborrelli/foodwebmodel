/*
 * ReadGDALFile.hpp
 *
 *  Created on: 24 mar. 2017
 *      Author: manu_
 */

#ifndef READGDALFILE_HPP_
#define READGDALFILE_HPP_

#include "../headers/ReadNetCDFFile.hpp"



#include <iostream>
#ifdef _WIN32
	#include "C:/Users/manu_/Downloads/netcdf/netcdf-cxx4-master/cxx4/ncFile.h"
#else
	#include <ncFile.h>
#endif



#define ERR(e) {cout<< "Error: "<< nc_strerror(e)<<"\n"; exit(2);}

using namespace std;
using namespace netCDF;
using namespace netCDF::exceptions;

/* Handle errors by printing an error message and exiting with a
 * non-zero status. */


void FoodWebModel::ReadNetCDFFile::readFile(string filename){

	int fileHandler, varHandler;
	int returnedFileValue=0;

		/*Open the NetCDF file*/
	netCDF::NcFile dataFile;


}

#endif /* READGDALFILE_HPP_ */
