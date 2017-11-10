/*
 * ReadProcessedData.hpp
 *
 *  Created on: 28 mar. 2017
 *      Author: manu_
 */

#ifndef READPROCESSEDDATA_HPP_
#define READPROCESSEDDATA_HPP_
#include <string>
#include <vector>
#include "TypeDefinitions.hpp"
#include "ModelConstants.hpp"

using namespace std;

namespace FoodWebModel{

	class ReadProcessedData{
		friend class FoodWebModel;
	protected:
		biomassType **initial_algae_biomass;
		animalCountType **initial_grazer_count;

		/* Grazer weight and distribution taken from [1] M. F. Baudouin and O. Ravera, “Weight, size and chemical composition of some freshwater zooplankters: Daphnia Hyalina (Leydig),” Limnol. Oceanogr., vol. 17, pp. 645–649, 1972.*/
		double initial_grazer_distribution[MAX_CLASS_INDEX];
		biomassType initial_grazer_weight[MAX_CLASS_INDEX];
		/* Algae and animal biomass differential*/
		biomassType baseBiomassDifferential[MAX_DEPTH_INDEX], zooplanktonBiomassCenterDifferencePerDepth[HOURS_PER_DAY];

		/* Physical data*/
		physicalType **initial_temperature;
		physicalType depth[MAX_COLUMN_INDEX], temperature_range[MAX_DEPTH_INDEX], depth_scale[MAX_DEPTH_INDEX], hourlyLightAtSurface[HOURS_PER_DAY], *phosphorusConcentrationAtBottom, *nitrogenConcentrationAtBottom, *yearlylightAtSurface;
		physicalType temperature_depth_proportion[MAX_DEPTH_INDEX], temperature_at_day[HOURS_PER_YEAR];
		unsigned int simulationCycles;
	public:
		ReadProcessedData();
		~ReadProcessedData();
		void readModelData(const SimulationArguments& simArguments);
	protected:
		template<typename T>
		void readValues(const string& route, T* readArray, unsigned int readLimit, bool plotReadParameter = false);
		void readDepth(const string& depthFileRoute);
		void readInitialTemperature(const string& initialTemperatureRoute);
		void readTemperatureRange(const string& temperatureRangeRoute);
		void readTemperatureDepthProportion(const string& temperatureDepthProportionRoute);
		void readTemperatureAtDay(const string& temperatureAtDayRoute);
		void readDepthScale(const string& depthScaleRoute);
		void readInitialAlgaeBiomass(const string& initialAlgaeBiomassRoute);
		void readBaseAlgaeBiomassDifferential(const string& biomassDifferentialRoute);
		void readInitialZooplanktonCount(const string& biomassRoute);
		void readInitialZooplanktonDistribution(const string& grazerDistributionRoute);
		void readInitialZooplanktonWeight(const string& grazerWeightRoute);
		void readPhosphorusConcentrationAtBottom(const string& phosphorusConcentrationAtBottomRoute);
		void readNitrogenConcentrationAtBottom(const string& nitrogenConcentrationAtBottomRoute);
		void readZooplanktonBiomassCenterDifferencePerDepth(const string& zooplanktonBiomassCenterDifferencePerDepthRoute);
		template<typename T>
		void readDataMatrix(const string& fileRoute, T** dataMatrix);
		void readLightAtSurface(const string& lightRoute);
		void readYearLightIntensity(const string& lightAtSurfaceYearRoute);
		//int readTemperatureAtSurface(string temperatureAtSurfaceFileRoute);

	};
}


#endif /* READPROCESSEDDATA_HPP_ */
