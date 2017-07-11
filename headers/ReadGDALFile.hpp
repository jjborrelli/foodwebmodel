/*
 * ReadGDALFile.hpp
 *
 *  Created on: 24 mar. 2017
 *      Author: manu_
 */

#ifndef READGDALFILE_HPP_
#define READGDALFILE_HPP_

#ifdef _WIN32
	#include "C:/OSGeo4W64/include/gdal_priv.h"
#else
	#include <gdal_priv.h>
#endif

namespace FoodWebModel{
	class ReadGDALFile{
	protected:
		GDALDataset  *poDataset;
	public:
		void readFile(std::string filename);
	};
}

#endif /* READGDALFILE_HPP_ */
