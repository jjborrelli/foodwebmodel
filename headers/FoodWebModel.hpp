/*
 * FoodWebModel.hpp
 *
 *  Created on: 13 mar. 2017
 *      Author: manu_
 */

#include <math.h>
#ifndef FOODWEBMODEL_HPP_
#define FOODWEBMODEL_HPP_

/*
 * EXTINCTION constant (Wetzel, 1975, AquaTox Documentation, page 73)
 * Since we do not consider any feedback from the food water to the geophysical model, we assume that Light Extinction = Water Extinction
 */
#define EXTINCTION 0.02

/*
 * We consider that virtually all phytoplankton has the light attenuation coefficient of diatom algae
 */
#define DIATOM_ATTENUATION 0.14
#define ATTENUATION_COEFFICIENT DIATOM_ATTENUATION


/*
 * The Secchi disk is an indication of water turbidity (AquaTox Documentation, page 74).
 */
#define TURBIDITY 1.2/EXTINCTION


namespace FoodWebModel {


	class FoodWebModel {
		FoodWebModel();


		/*Class attributes*/
	protected:
		double* depthVector;
		double** phytoBiomass;
		double incidentIntensity;
		double fractionInEuphoticZone, ZEuphotic, ZMean, ZMax, P;
		/*Class methods*/
	public:
		int simulate(int cycles);
		FoodWebModel();
	protected:
		double biomassDifferential(int depthIndex, int column, bool periPhyton);
		double lightIntensity(int depthIndex, int column);
		double sumPhytoBiomassToDepth(int depthIndex, int column);
		double photoSynthesis(int depthIndex, int column, bool periPhyton);
		double productionLimit(int depthIndex, int column, bool periPhyton);
		void setBathymetricParameters();

	};
}



#endif /* FOODWEBMODEL_HPP_ */
