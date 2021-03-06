/*
 * XMLReader.cpp
 *
 *  Created on: May 11, 2017
 *      Author: manu_
 */

#include <fstream>
#include <iostream>
#include <vector>
#include "../headers/ParameterReader.hpp"
#include "../headers/AuxFunctions.hpp"

using namespace std;

FoodWebModel::ParameterReader::ParameterReader() {
	// TODO Auto-generated constructor stub

}

FoodWebModel::ParameterReader::~ParameterReader() {
	// TODO Auto-generated destructor stub
}


/* Read the simulation parameters from a text file*/
void FoodWebModel::ParameterReader::readSimulationParametersFromFile(const std::string& parameterFileName){

	/* Open the parameter file*/
	ifstream parametersFile;
	string file_line, file_content;
	parametersFile.open(parameterFileName);
	unsigned int lineCounter=0;
	if(parametersFile.is_open()){
		/*Read while there are lines left in the file*/
		std::cout<<"Reading parameters from file "<<parameterFileName<<"."<<endl;
		while(std::getline(parametersFile, file_line, '\n')){
			//			std::cout<<"Line read: "<<file_line<<"."<<endl;
			vector<string> parameterRead = generalSplit(file_line, std::string(";"));
			setParameter(parameterRead[0], parameterRead[1]);
			std::cout<<"Parameter pair with name: "<<parameterRead[0]<<" and value: "<<parameterRead[1]<<" read."<<endl;
			lineCounter++;
		}
		parametersFile.close();
		std::cout<<"Closed parameter file "<<parameterFileName<<"."<<endl;
	}

}

/*Set pair name-value*/
void FoodWebModel::ParameterReader::setParameter(const std::string& parameterName, const std::string& parameterValue){
	if(!parameterName.compare("DepthRoute")){
		simArguments.depthRoute= parameterValue;
		return;
	}
	if(!parameterName.compare("DepthScaleRoute")){
		simArguments.depthScaleRoute= parameterValue;
		return;
	}
	if(!parameterName.compare("InitialTemperatureRoute")){
		simArguments.initialTemperatureRoute= parameterValue;
		return;
	}
	if(!parameterName.compare("TemperatureRangeRoute")){
		simArguments.temperatureRangeRoute= parameterValue;
		return;
	}
	if(!parameterName.compare("TemperatureDepthProportionRoute")){
		simArguments.temperatureDepthProportionRoute= parameterValue;
		return;
	}
	if(!parameterName.compare("TemperatureAtTimeRoute")){
		simArguments.temperatureAtTimeRoute= parameterValue;
		return;
	}
	if(!parameterName.compare("OutputAlgaeRoute")){
		simArguments.outputAlgaeRoute= parameterValue;
		return;
	}
	if(!parameterName.compare("OutputSloughRoute")){
		simArguments.outputSloughRoute= parameterValue;
		return;
	}
	if(!parameterName.compare("OutputGrazerRoute")){
		simArguments.outputGrazerRoute= parameterValue;
		return;
	}
	if(!parameterName.compare("OutputGrazerTraceRoute")){
		simArguments.outputGrazerTraceRoute= parameterValue;
		return;
	}
	if(!parameterName.compare("OutputPredatorTraceRoute")){
		simArguments.outputPredatorTraceRoute= parameterValue;
		return;
	}
	if(!parameterName.compare("OutputPredatorRoute")){
		simArguments.outputPredatorRoute= parameterValue;
		return;
	}
	if(!parameterName.compare("OutputPhysicalRoute")){
		simArguments.outputPhysicalRoute= parameterValue;
		return;
	}
	if(!parameterName.compare("OutputParameterRoute")){
		simArguments.outputParameterRoute= parameterValue;
		return;
	}
	if(!parameterName.compare("OutputAssertionViolationRoute")){
		simArguments.outputAssertionViolationRoute= parameterValue;
		return;
	}
	if(!parameterName.compare("InitialAlgaeBiomassRoute")){
		simArguments.initialAlgaeBiomassRoute= parameterValue;
		return;
	}
	if(!parameterName.compare("InitialZooplanktonCountRoute")){
		simArguments.initialZooplanktonCountRoute= parameterValue;
		return;
	}
	if(!parameterName.compare("InitialPlanktovireCountRoute")){
		simArguments.initialPlanktivoreCountRoute= parameterValue;
		return;
	}
	if(!parameterName.compare("InitialZooplanktonDistributionRoute")){
		simArguments.initialZooplanktonDistributionRoute= parameterValue;
		return;
	}
	if(!parameterName.compare("InitialZooplanktonWeightRoute")){
		simArguments.initialZooplanktonWeightRoute= parameterValue;
		return;
	}
	if(!parameterName.compare("InitialPlanktivoreWeightRoute")){
		simArguments.initialPlanktivoreWeightRoute= parameterValue;
		return;
	}
	if(!parameterName.compare("LightAtSurfaceRoute")){
		simArguments.lightAtSurfaceRoute= parameterValue;
		return;
	}
	if(!parameterName.compare("LightAtSurfaceYearRoute")){
		simArguments.lightAtSurfaceYearRoute= parameterValue;
		return;
	}
	if(!parameterName.compare("BiomassBaseDifferentialRoute")){
		simArguments.biomassBaseDifferentialRoute= parameterValue;
		return;
	}

	if(!parameterName.compare("SimulationCycles")){
		simArguments.simulationCycles= atoi(parameterValue.c_str());
		return;
	}

	if(!parameterName.compare("PhosphorusConcentrationAtBottomRoute")){
		simArguments.phosphorusConcentrationAtBottomRoute= parameterValue;
		return;
	}
	if(!parameterName.compare("NitrogenConcentrationAtBottomRoute")){
		simArguments.nitrogenConcentrationAtBottomRoute= parameterValue;
		return;
	}
	if(!parameterName.compare("ZooplanktonBiomassDepthCenterRoute")){
		simArguments.zooplanktonBiomassDepthCenterRoute= parameterValue;
		return;
	}
	if(!parameterName.compare("AlgaeBiomassDifferentialScale")){
		simArguments.algae_biomass_differential_production_scale= atof(parameterValue.c_str());
		return;
	}
	if(!parameterName.compare("PhosphorusHalfSaturation")){
		simArguments.phosphorus_half_saturation = atof(parameterValue.c_str());
		return;
	}
	if(!parameterName.compare("LightAllowanceWeight")){
		simArguments.light_allowance_weight = atof(parameterValue.c_str());
		return;
	}
	if(!parameterName.compare("LightAllowanceProportion")){
		simArguments.light_allowance_proportion = atof(parameterValue.c_str());
		return;
	}
	if(!parameterName.compare("PhosphorusWeight")){
		simArguments.phosphorous_weight = atof(parameterValue.c_str());
		return;
	}
	if(!parameterName.compare("NutrientDerivative")){
		simArguments.nutrient_derivative= atof(parameterValue.c_str());
		return;
	}
	if(!parameterName.compare("NutrientGrowth")){
		simArguments.nutrient_growth= atof(parameterValue.c_str());
		return;
	}
	if(!parameterName.compare("DecayingPhosphorusFactor")){
		simArguments.decaying_phosphorus_factor=atof(parameterValue.c_str());
		return;
	}
	if(!parameterName.compare("PhosphorusFunctionalFactor")){
		simArguments.phosphorus_functional_factor=atof(parameterValue.c_str());
		return;
	}
	if(!parameterName.compare("PhosphorusFunctionalConstantResponse1")){
		simArguments.phosphorus_functional_constant_response_1=atof(parameterValue.c_str());
		return;
	}
	if(!parameterName.compare("PhosphorusFunctionalConstantResponse2")){
		simArguments.phosphorus_functional_constant_response_2=atof(parameterValue.c_str());
		return;
	}
	if(!parameterName.compare("PhosphorusFunctionalStep1")){
		simArguments.phosphorus_functional_step_1=atof(parameterValue.c_str());
		return;
	}
	if(!parameterName.compare("NitrogenHalfSaturation")){
		simArguments.nitrogen_half_saturation=atof(parameterValue.c_str());
		return;
	}
	if(!parameterName.compare("NitrogenFunctionalConstantResponse")){
		simArguments.nitrogen_functional_constant_response=atof(parameterValue.c_str());
		return;
	}
	if(!parameterName.compare("NitrogenFunctionalStep")){
		simArguments.nitrogen_functional_step=atof(parameterValue.c_str());
		return;
	}
	if(!parameterName.compare("NitrogenPhosphorusLowerFactor")){
		simArguments.nitrogen_phosphorus_lower_bound=atof(parameterValue.c_str());
		return;
	}
	if(!parameterName.compare("NitrogenPhosphorusUpperFactor")){
		simArguments.nitrogen_phosphorus_upper_bound=atof(parameterValue.c_str());
		return;
	}
	if(!parameterName.compare("TemperatureOptimal")){
		simArguments.temperature_optimal=atof(parameterValue.c_str());
		return;
	}
	if(!parameterName.compare("TemperatureSteepness")){
		simArguments.temperature_steepness=atof(parameterValue.c_str());
		return;
	}
	if(!parameterName.compare("TemperatureSuppressionSteepness")){
		simArguments.temperature_suppression_steepness=atof(parameterValue.c_str());
		return;
	}
	if(!parameterName.compare("TemperatureMax")){
			simArguments.temperature_max=atof(parameterValue.c_str());
			return;
		}
	if(!parameterName.compare("WashUpDeadBiomass")){
		simArguments.wash_up_dead_biomass_proportion=atof(parameterValue.c_str());
		return;
	}
	if(!parameterName.compare("WashDownDeadBiomass")){
		simArguments.wash_down_dead_biomass_proportion=atof(parameterValue.c_str());
		return;
	}
	if(!parameterName.compare("AlgaeBiomassConservationFactor")){
			simArguments.algae_biomass_conservation_factor=atof(parameterValue.c_str());
			return;
	}
	if(!parameterName.compare("RetainedPhosphorusFactor")){
		simArguments.retained_phosphorus_factor=atof(parameterValue.c_str());
		return;
	}
	if(!parameterName.compare("Respiration20Degrees")){
		simArguments.algal_respiration_at_20_degrees = atof(parameterValue.c_str());
		return;
	}
	if(!parameterName.compare("AlgalRespirationExponentialTemperatureCoefficient")){
		simArguments.exponential_temperature_algal_respiration_coefficient = atof(parameterValue.c_str());
		return;
	}
	if(!parameterName.compare("IntrinsicAlgaeMortalityRate")){
		simArguments.intrinsic_algae_mortality_rate = atof(parameterValue.c_str());
		return;
	}
	if(!parameterName.compare("MaximumAlgaeDeathResources")){
		simArguments.maximum_algae_resources_death = atof(parameterValue.c_str());
		return;
	}
	if(!parameterName.compare("LightSteepness")){
		simArguments.light_steepness = atof(parameterValue.c_str());
		return;
	}
	if(!parameterName.compare("LightLowerQuantile")){
		simArguments.light_lower_quantile = atof(parameterValue.c_str());
		return;
	}
	if(!parameterName.compare("LightUpperQuantile")){
		simArguments.light_upper_quantile = atof(parameterValue.c_str());
		return;
	}
	if(!parameterName.compare("LightSteepnessFactor")){
		simArguments.light_steepness_factor = atof(parameterValue.c_str());
		return;
	}
	if(!parameterName.compare("DiatomAttenuationCoefficient")){
		simArguments.diatom_attenuation_coefficient = atof(parameterValue.c_str());
		return;
	}
	if(!parameterName.compare("LimitationScaleWeight")){
		simArguments.limitation_scale_weight = atof(parameterValue.c_str());
		return;
	}
	if(!parameterName.compare("AlgalCarryingCapacityCoefficient")){
		simArguments.algal_carrying_capacity_coefficient= atof(parameterValue.c_str());
		return;
	}
	if(!parameterName.compare("AlgalCarryingCapacityIntercept")){
		simArguments.algal_carrying_capacity_intercept = atof(parameterValue.c_str());
		return;
	}
	if(!parameterName.compare("AlgalMaximumFoundBiomass")){
		simArguments.maximum_found_algal_biomass = atof(parameterValue.c_str());
		return;
	}
	if(!parameterName.compare("AlgalReabsorbedDeadNutrientsProportion")){
		simArguments.reabsorbed_algal_nutrients_proportion = atof(parameterValue.c_str());
		return;
	}
	if(!parameterName.compare("AlgalMortalityScale")){
		simArguments.algal_mortality_scale = atof(parameterValue.c_str());
		return;
	}
	if(!parameterName.compare("AlgalIntrinsicSettlingRate")){
		simArguments.intrinsic_settling_rate = atof(parameterValue.c_str());
		return;
	}
	if(!parameterName.compare("AlgalFractionSloughed")){
		simArguments.algal_fraction_sloughed = atof(parameterValue.c_str());
		return;
	}
	if(!parameterName.compare("GrazerBaseMortality")){
		simArguments.grazer_base_mortality_proportion= atof(parameterValue.c_str());
		return;
	}
	if(!parameterName.compare("GrazerFilteringRate")){
		simArguments.grazer_filtering_rate_per_individual= atof(parameterValue.c_str());
		return;
	}
	if(!parameterName.compare("GrazerFilteringLengthCoefficient")){
		simArguments.grazer_filtering_length_coefficient= atof(parameterValue.c_str());
		return;
	}
	if(!parameterName.compare("GrazerFilteringLengthExponent")){
		simArguments.grazer_filtering_length_exponent= atof(parameterValue.c_str());
		return;
	}
	if(!parameterName.compare("GrazerFilteringCoefficient")){
		simArguments.grazer_filtering_coefficient= atof(parameterValue.c_str());
		return;
	}
	if(!parameterName.compare("GrazerFilteringExponent")){
		simArguments.grazer_filtering_exponent= atof(parameterValue.c_str());
		return;
	}
	if(!parameterName.compare("GrazerConsumptionTemperatureFactor")){
		simArguments.grazer_consumption_temperature_factor= atof(parameterValue.c_str());
		return;
	}
	if(!parameterName.compare("GrazerBasalRespirationWeight")){
		simArguments.grazer_basal_respiration_weight= atof(parameterValue.c_str());
		return;
	}
	if(!parameterName.compare("GrazerKValueRespiration")){
		simArguments.grazer_k_value_respiration= atof(parameterValue.c_str());
		return;
	}
	if(!parameterName.compare("GrazerCarryingCapacityCoefficient")){
		simArguments.grazer_carrying_capacity_coefficient= atof(parameterValue.c_str());
		return;
	}
	if(!parameterName.compare("GrazerCarryingCapacityIntercept")){
		simArguments.grazer_carrying_capacity_intercept = atof(parameterValue.c_str());
		return;
	}
	if(!parameterName.compare("GrazerLayerCenterIndex")){
		simArguments.grazer_layer_center_index = atoi(parameterValue.c_str());
		return;
	}
	if(!parameterName.compare("GrazerCarryingCapacityAmplitude")){
			simArguments.grazer_carrying_capacity_amplitude= atof(parameterValue.c_str());
			return;
		}
	if(!parameterName.compare("GrazerCarryingCapacityConstant")){
			simArguments.grazer_carrying_capacity_constant= atof(parameterValue.c_str());
			return;
		}
	if(!parameterName.compare("GrazerMaximumFoundBiomass")){
		simArguments.grazer_maximum_found_biomass = atof(parameterValue.c_str());
		return;
	}
	if(!parameterName.compare("GrazerMaxHoursWithoutFood")){
		simArguments.grazer_max_hours_without_food= atoi(parameterValue.c_str());
		return;
	}
	if(!parameterName.compare("PlanktivoreMaxHoursWithoutFood")){
			simArguments.planktivore_max_hours_without_food= atoi(parameterValue.c_str());
			return;
		}
	if(!parameterName.compare("PlanktivoreStarvationFactor")){
			simArguments.planktivore_starvation_factor= atof(parameterValue.c_str());
			return;
		}
	if(!parameterName.compare("GrazerDeadAnimalsPerLostBiomassAndConcentration")){
		simArguments.grazer_dead_animals_per_lost_biomass_and_concentration= atof(parameterValue.c_str());
		return;
	}
	if(!parameterName.compare("GrazerFoodStarvationThreshold")){
		simArguments.grazer_food_starvation_threshold= atof(parameterValue.c_str());
		return;
	}
	if(!parameterName.compare("GrazerMaximumAgeInHours")){
		simArguments.grazer_maximum_age_in_hours= atoi(parameterValue.c_str());
		return;
	}
	if(!parameterName.compare("GrazerMaxGonadAllocation")){
		simArguments.grazer_maximum_gonad_weight_allocation= atof(parameterValue.c_str());
		return;
	}
	if(!parameterName.compare("GrazerIncubationHours")){
		simArguments.grazer_incubation_hours= atoi(parameterValue.c_str());
		return;
	}
	if(!parameterName.compare("GrazerEggAllocationThreshold")){
		simArguments.grazer_egg_allocation_threshold= atof(parameterValue.c_str());
		return;
	}
	if(!parameterName.compare("GrazerOvipositingPeriod")){
		simArguments.grazer_ovipositing_period= atoi(parameterValue.c_str());
		return;
	}
	if(!parameterName.compare("GrazerMaturationHours")){
		simArguments.grazer_maturation_hours= atoi(parameterValue.c_str());
		return;
	}
	if(!parameterName.compare("GrazerAgglomerationCohortThreshold")){
		simArguments.grazer_agglomeration_cohort_threshold= atoi(parameterValue.c_str());
		return;
	}
	if(!parameterName.compare("GrazerReproductionProportionInvestmentAmplitude")){
		simArguments.grazer_reproduction_proportion_investment_amplitude= atof(parameterValue.c_str());
		return;
	}
	if(!parameterName.compare("GrazerReproductionProportionInvestmentCoefficient")){
		simArguments.grazer_reproduction_proportion_investment_coefficient= atof(parameterValue.c_str());
		return;
	}
	if(!parameterName.compare("GrazerReproductionProportionInvestmentIntercept")){
		simArguments.grazer_reproduction_proportion_investment_intercept= atof(parameterValue.c_str());
		return;
	}
	if(!parameterName.compare("GrazerReproductionProportionInvestmentConstant")){
		simArguments.grazer_reproduction_proportion_investment_constant= atof(parameterValue.c_str());
		return;
	}
	if(!parameterName.compare("GrazerReproductionProportionInvestmentMultiplier")){
		simArguments.grazer_reproduction_proportion_investment_multiplier= atof(parameterValue.c_str());
		return;
	}
	if(!parameterName.compare("GrazerStarvationFactor")){
		simArguments.grazer_starvation_factor= atof(parameterValue.c_str());
		return;
	}
	if(!parameterName.compare("GrazerDeadAnimalProportion")){
		simArguments.grazer_dead_animal_proportion= atof(parameterValue.c_str());
		return;
	}
	if(!parameterName.compare("GrazerReabsorbedDeadNutrientsProportion")){
		simArguments.grazer_reabsorbed_animal_nutrients_proportion = atof(parameterValue.c_str());
		return;
	}
	if(!parameterName.compare("GrazerRandomSeed")){
		simArguments.random_seed= atoi(parameterValue.c_str());
		return;
	}
	if(!parameterName.compare("PopulationSeed")){
		simArguments.population_seed= atoi(parameterValue.c_str());
		return;
	}
	if(!parameterName.compare("InitialAlgalCoefficientOfVariation")){
		simArguments.initial_algae_coefficient_variation= atof(parameterValue.c_str());
		return;
	}
	if(!parameterName.compare("GrazerVelocityDownwardPull")){
		simArguments.grazer_velocity_downward_pull= atof(parameterValue.c_str());
		return;
	}
	if(!parameterName.compare("GrazerCriticalLightIntensity")){
		simArguments.grazer_critical_light_intensity= atof(parameterValue.c_str());
		return;
	}
	if(!parameterName.compare("GrazerStarvationThreshold")){
		simArguments.grazer_ind_food_starvation_threshold= atof(parameterValue.c_str());
		return;
	}
	if(!parameterName.compare("GrazingStarvationMaxHours")){
		simArguments.grazer_starvation_max_hours= atoi(parameterValue.c_str());
		return;
	}
	if(!parameterName.compare("GrazerCriticalDepth")){
		simArguments.grazer_critical_depth= atof(parameterValue.c_str());
		return;
	}
	if(!parameterName.compare("GrazerLightOptimalValue")){
		simArguments.grazer_light_optimal_value= atof(parameterValue.c_str());
		return;
	}
	if(!parameterName.compare("GrazerMigrationLightWeight")){
		simArguments.grazer_light_migration_weight= atof(parameterValue.c_str());
		return;
	}
	if(!parameterName.compare("GrazerMigrationConsumption")){
		simArguments.grazer_migration_consumption= atof(parameterValue.c_str());
		return;
	}
	if(!parameterName.compare("KairomonesLevelDay")){
		simArguments.kairomones_level_day= atof(parameterValue.c_str());
		return;
	}
	if(!parameterName.compare("KairomonesLevelNight")){
		simArguments.kairomones_level_night= atof(parameterValue.c_str());
		return;
	}
	if(!parameterName.compare("KairomoneThermocline")){
		simArguments.kairomones_thermocline= atof(parameterValue.c_str());
		return;
	}
	if(!parameterName.compare("PlanktivoreBiomassCenterDay")){
		simArguments.planktivore_biomass_center_day= atof(parameterValue.c_str());
		return;
	}
	if(!parameterName.compare("PlanktivoreBiomassCenterNight")){
		simArguments.planktivore_biomass_center_night= atof(parameterValue.c_str());
		return;
	}
	if(!parameterName.compare("PlanktivoreBiomassWidth")){
		simArguments.planktivore_biomass_width= atof(parameterValue.c_str());
		return;
	}
	if(!parameterName.compare("MaxVerticalMigration")){
		simArguments.max_vertical_migration= atoi(parameterValue.c_str());
		return;
	}
	if(!parameterName.compare("MaxHorizontalMigration")){
		simArguments.max_horizontal_migration= atoi(parameterValue.c_str());
		return;
	}
	if(!parameterName.compare("MaxSearchSteps")){
		simArguments.grazer_max_search_steps= atoi(parameterValue.c_str());
		return;
	}
	if(!parameterName.compare("GrazerMinimumTolerableLight")){
		simArguments.grazer_minimum_tolerable_light= atof(parameterValue.c_str());
		return;
	}
	if(!parameterName.compare("GrazerPredationIndex")){
		simArguments.grazer_predation_index= atof(parameterValue.c_str());
		return;
	}
	if(!parameterName.compare("RandomWalkProbabilityWeight")){
		simArguments.grazer_random_walk_probability_weight= atof(parameterValue.c_str());
		return;
	}
	if(!parameterName.compare("GrazerMinimumPredationSafety")){
		simArguments.grazer_minimum_predation_safety=atof(parameterValue.c_str());
		return;
	}
	if(!parameterName.compare("GrazerMaximumLightTolerated")){
			simArguments.grazer_maximum_light_tolerated=atof(parameterValue.c_str());
			return;
		}
	if(!parameterName.compare("GrazerLightSafetyWeight")){
				simArguments.grazer_light_safety_weight=atof(parameterValue.c_str());
				return;
			}
	if(!parameterName.compare("GrazerLightSafetyThreshold")){
		simArguments.grazer_light_safety_threshold=atof(parameterValue.c_str());
		return;
	}

	if(!parameterName.compare("GrazerCohortSplittingLimit")){
		simArguments.grazer_cohort_splitting_limit=atoi(parameterValue.c_str());
		return;
	}
	if(!parameterName.compare("PredatorBaseMortality")){
		simArguments.predator_base_mortality_proportion= atof(parameterValue.c_str());
		return;
	}
	if(!parameterName.compare("PredatorFilteringRate")){
		simArguments.predator_filtering_rate_per_individual= atof(parameterValue.c_str());
		return;
	}
	if(!parameterName.compare("PredatorBasalRespirationWeight")){
		simArguments.predator_basal_respiration_weight= atof(parameterValue.c_str());
		return;
	}
	if(!parameterName.compare("PredatorKValueRespiration")){
		simArguments.predator_k_value_respiration= atof(parameterValue.c_str());
		return;
	}
	if(!parameterName.compare("PlanktivoreCarryingCapacityCoefficient")){
		simArguments.planktivore_carrying_capacity_coefficient= atof(parameterValue.c_str());
		return;
	}
	if(!parameterName.compare("PlanktivoreCarryingCapacityIntercept")){
		simArguments.planktivore_carrying_capacity_intercept = atof(parameterValue.c_str());
		return;
	}
	if(!parameterName.compare("PredatorMaximumFoundBiomass")){
		simArguments.predator_maximum_found_biomass = atof(parameterValue.c_str());
		return;
	}
	if(!parameterName.compare("PredatorRandomSeed")){
		simArguments.predator_random_seed= atoi(parameterValue.c_str());
		return;
	}
	if(!parameterName.compare("PlanktivoreMaximumDepth")){
		simArguments.maximum_planktivore_depth= atoi(parameterValue.c_str());
		return;
	}
	if(!parameterName.compare("PlanktivoreSaturationConstant")){
		simArguments.planktivore_saturation_constant= atof(parameterValue.c_str());
		return;
	}
	if(!parameterName.compare("PlanktivoreConsumptionWeight")){
		simArguments.planktivore_consumption_weight= atof(parameterValue.c_str());
		return;
	}
	if(!parameterName.compare("PlanktivoreIncubationHours")){
		simArguments.planktivore_incubation_hours= atoi(parameterValue.c_str());
		return;
	}

	if(!parameterName.compare("GrazerPredationPerceivedBiomass")){
		simArguments.grazer_predation_perceived_biomass= atof(parameterValue.c_str());
		return;
	}


	cout<<"Parameter: "<<parameterName<<" with value: "<<parameterValue<<" does not correspond to any set parameter."<<endl;
}

/*Parse additional arguments as a string*/
void FoodWebModel::ParameterReader::readSimulationParametersFromLine(const std::string& parameterLine){
	cout<<"Parsing additional parameters."<<endl;
	/*Parameters are separated by ';'*/
	vector<string> parameterRead = generalSplit(parameterLine, std::string(";"));
	for (int parameterIndex = 0; parameterIndex < parameterRead.size(); ++parameterIndex) {
		/*Parameter name and value are separated by '='*/
		vector<string> parameterAndValue = generalSplit(parameterRead[parameterIndex], std::string("="));
		setParameter(parameterAndValue[0], parameterAndValue[1]);
		cout<<"Parameter "<<parameterAndValue[0]<<" read with value "<<parameterAndValue[1]<<"."<<endl;
	}
}
//
//void FoodWebModel::XMLReader::readXMLFile(const std::string& XMLFileName){
//	xml_document<> doc;
//
//	/* Append the file content for further parsing*/
//	ifstream parametersFile;
//	string file_buffer, file_content;
//	parametersFile.open(XMLFileName);
//	if(parametersFile.is_open()){
//		while(!parametersFile.eof()){
//			parametersFile >> file_buffer;
//			if(!parametersFile.eof()){
//				file_content.append(file_buffer);
//			}
//
//		}
//		try{
//			document.parse<0>((char*)file_content.c_str());
//		}
//			catch(parse_error e){
//			std::cerr<<e.what()<<endl;
//		}
//
//		setSimulationParameters();
//		parametersFile.close();
//	}
//
//}
//
//void FoodWebModel::XMLReader::setSimulationParameters(){
//	xml_node<> *simulationArguments = document.first_node("arguments");
//			simArguments.depthRoute= simulationArguments->first_node("DepthRoute")->value();
//			simArguments.depthScaleRoute= simulationArguments->first_node("DepthScaleRoute")->value();
//			simArguments.initialTemperatureRoute= simulationArguments->first_node("InitialTemperatureRoute")->value();
//			simArguments.temperatureRangeRoute= simulationArguments->first_node("TemperatureRangeRoute")->value();
//			simArguments.outputAlgaeRoute= simulationArguments->first_node("OutputAlgaeRoute")->value();
//			simArguments.outputSloughRoute= simulationArguments->first_node("OutputSloughRoute")->value();
//			simArguments.outputGrazerRoute= simulationArguments->first_node("OutputGrazerRoute")->value();
//			simArguments.initialAlgaeBiomassRoute= simulationArguments->first_node("InitialAlgaeBiomassRoute")->value();
//			simArguments.initialZooplanktonCountRoute= simulationArguments->first_node("InitialZooplanktonCountRoute")->value();
//			simArguments.lightAtSurfaceRoute= simulationArguments->first_node("LightAtSurfaceRoute")->value();
//			simArguments.simulationCycles= atoi(simulationArguments->first_node("SimulationCycles")->value());
//}
