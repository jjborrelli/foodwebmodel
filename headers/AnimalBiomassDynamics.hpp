/*
 * AnimalBiomassDynamics.hpp
 *
 *  Created on: Jul 6, 2017
 *      Author: manu_
 */

#ifndef ANIMALBIOMASSDYNAMICS_HPP_
#define ANIMALBIOMASSDYNAMICS_HPP_
#include <sstream>
#include "TypeDefinitions.hpp"
#include "ModelConstants.hpp"
#include "SimulationModes.hpp"

using namespace std;

namespace FoodWebModel {

class AnimalBiomassDynamics {
	friend class FoodWebModel;
public:
	AnimalBiomassDynamics();
	virtual ~AnimalBiomassDynamics();
protected:
	std::ostringstream *lineBuffer, *animalBiomassBuffer;
	/* Animal count summing. The simulation halts below a given number*/
	zooplanktonCountType floating_animal_count_summing;
	/* Animal biomass in micrograms*/
	biomassType **floatingAnimalBiomass, *bottomAnimalBiomass;
	/* Food biomass in micrograms*/
	biomassType **floatingFoodBiomass, *bottomFoodBiomass;
	/* Animal individual count. Transformed to biomass using the rule: (count*grazer weight in micrograms)*/
	zooplanktonCountType **floatingAnimalCount, *bottomAnimalCount;
	/* Pointers connecting to the physical model*/
	unsigned int *maxDepthIndex, *current_hour, *salt_concentration, *salinity_effect;
	physicalType **temperature;

	/* Zooplankton attributes*/
	biomassType used_consumption, consumption_per_individual, locale_consumption, locale_defecation, base_animal_respiration, salinity_corrected_animal_respiration, basal_animal_respiration, active_respiration_exponent, active_respiration_factor, active_respiration, metabolic_respiration, animal_excretion_loss, animal_base_mortality, animal_temperature_mortality, animal_temp_independent_mortality, salinity_mortality, locale_consumption_salt_adjusted, animal_mortality, animal_predatory_pressure, low_oxigen_animal_mortality;

#ifdef CHECK_ASSERTIONS
		std::ostringstream *assertionViolationBuffer;
#endif
	void updateAnimalBiomass();
	biomassType animalBiomassDifferential(int depthIndex, int columnIndex, bool bottom, physicalType foodConversionFactor);
	void foodConsumptionRate(int depthIndex, int columnIndex, bool bottomFeeder, biomassType algaeBiomassInMicrograms);
	void defecation();
	void animalRespiration(biomassType zooBiomass, physicalType localeTemperature);
	biomassType basalRespiration(biomassType zooBiomass);
	biomassType activeRespiration(biomassType zooBiomass, physicalType localeTemperature);
	biomassType metabolicFoodConsumption();
	void animalExcretion(biomassType localeRespiration);
	void animalMortality(biomassType localeRespiration, physicalType localeTemperature, physicalType localeSalinityConcentration);
	void animalBaseMortality(physicalType localeTemperature, biomassType localeBiomass);
	void animalTemperatureMortality(physicalType localeTemperature, biomassType localeBiomass);
	void salinityMortality(biomassType localeBiomass);
	void calculateLowOxigenMortality(biomassType inputBiomass);
	void stroganovApproximation(physicalType localeTemperature);
	void calculatePredationPressure(zooplanktonCountType zooplanktonLocaleCount);
	void calculateGrazerCarryingCapacityMortality(biomassType inputBiomass);
};

} /* namespace FoodWebModel */

#endif /* ANIMALBIOMASSDYNAMICS_HPP_ */
