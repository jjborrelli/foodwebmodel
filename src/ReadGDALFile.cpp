#include "../headers/FoodWebModel.hpp"

using namespace std;

void FoodWebModel::ReadGDALFile::readGDALFile(string filename){
	GDALAllRegister();

	 poDataset = (GDALDataset *) GDALOpen(filename.c_str(), GA_ReadOnly );
	    if( poDataset == NULL )
	    {
	    	int raster_x_size = GDALGetRasterXSize(poDataset);
	    }
}
