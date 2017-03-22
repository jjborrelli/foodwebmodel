#include "../headers/FoodWebModel.hpp"
#include <iostream>
using namespace std;

void FoodWebModel::ReadGDALFile::readGDALFile(string filename){
	GDALAllRegister();

	/*Read raster data*/
	 poDataset = (GDALDataset *) GDALOpen(filename.c_str(), GA_ReadOnly );
	    if( poDataset != NULL )
	    {
	    	/*Get raster dimensions*/
	    	int raster_x_size = GDALGetRasterXSize(poDataset), raster_y_size = GDALGetRasterYSize(poDataset);
	    	int numberOfBands = poDataset->GetRasterCount() ;

	    	/*Fetch the raster band (presumably 1) and read its data*/
	    	GDALRasterBand* poBand = poDataset->GetRasterBand( 1 );
	    	int band_x_size = poBand->GetXSize(), band_y_size = poBand->GetYSize();

	    	/*Get block size*/
	    	int nXBlockSize, nYBlockSize;
	    	poBand->GetBlockSize( &nXBlockSize, &nYBlockSize );
	    	int nXBlocks = (poBand->GetXSize() + nXBlockSize - 1) / nXBlockSize;
	    	int nYBlocks = (poBand->GetYSize() + nYBlockSize - 1) / nYBlockSize;

	    	 GByte *pabyData = (GByte *) CPLMalloc(nXBlockSize * nYBlockSize);
	    	     for( int iYBlock = 0; iYBlock < nYBlocks; iYBlock++ )
	    	     {
	    	         for( int iXBlock = 0; iXBlock < nXBlocks; iXBlock++ )
	    	         {
	    	             int        nXValid, nYValid;
	    	             CPLErr error_block_signal = poBand->ReadBlock( iXBlock, iYBlock, pabyData );
	    	             // Compute the portion of the block that is valid
	    	             // for partial edge blocks.

	    	         }
	    	     }
	    	float* pafScanline = (float *) CPLMalloc(sizeof(float)*band_x_size*band_y_size);
	    	CPLErr error_band_signal = poBand->RasterIO( GF_Read, 0, 0, band_x_size, band_y_size,
	    	                  pafScanline, band_x_size, band_y_size, GDT_Float32,
	    	                  0, 0 );

	    	cout << "X: "<<raster_x_size<<", Y: "<<raster_y_size<<", number: "<<numberOfBands;

	    }
}
