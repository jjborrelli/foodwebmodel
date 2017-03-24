/*
 * ReadNetCDFFile.hpp
 *
 *  Created on: 24 mar. 2017
 *      Author: manu_
 */

#ifndef READNETCDFFILE_HPP_
#define READNETCDFFILE_HPP_
#include <netcdf>

#include <string>

namespace FoodWebModel{
	class ReadNetCDFFile{
	protected:
		double* readDepth;
	public:
		void readFile(std::string filename);
	};
}



#endif /* READNETCDFFILE_HPP_ */
