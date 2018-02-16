/*
 * GrazerBiomassDynamics.cpp
 *
 *  Created on: 5 ene. 2018
 *      Author: manu
 */

#include "../headers/GrazerBiomassDynamics.hpp"

namespace FoodWebModel {


GrazerBiomassDynamics::GrazerBiomassDynamics() {
	// TODO Auto-generated destructor stub
}

GrazerBiomassDynamics::~GrazerBiomassDynamics() {
	// TODO Auto-generated destructor stub
}



void GrazerBiomassDynamics::calculateKairomonesConcetration(){
	/* Kairomone levels depend on day and night cycles*/
	physicalType surfaceKairomones = this->dayTime?this->kairomones_level_day:this->kairomones_level_night;
	for(int columnIndex=0; columnIndex<MAX_COLUMN_INDEX; ++columnIndex){
		for(int depthIndex=0; depthIndex<=maxDepthIndex[columnIndex]; ++depthIndex){
			physicalType localeDepth=this->indexToDepth[depthIndex];
			/* Assume a sigmoid diffusion function for kairomones, like any other chemical compound (e.g. inorganic chemicals)*/
			this->kairomoneConcentration[depthIndex][columnIndex]=surfaceKairomones*(1-1/(1+exp(-(localeDepth-this->kairomones_thermocline))));
		}
	}

}



/* Food consumption (AquaTox Documentation, page 105, equation 98)*/
void GrazerBiomassDynamics::foodConsumptionRate(int depthIndex, int columnIndex, bool bottom, animalCountType animalCount, biomassType foodBiomassInMicrograms, biomassType individualWeight, double consumedProportion){
	physicalType localeTemperature = temperature[depthIndex][columnIndex];
#ifdef GRAZING_DEPENDING_ON_WEIGHT
	/* Grazing constant taken from [1] C. W. Burns, �Relation between filtering rate, temperature, and body size in four species of Daphnia,� Limnol. Oceanogr., vol. 14, no. 5, pp. 693�700, Sep. 1969.*/
	biomassType bodyLength = pow((individualWeight*MICROGRAM_TO_GRAM)/this->filtering_length_coefficient, this->filtering_length_exponent);
	biomassType usedFiltering = this->filtering_coefficient*pow(bodyLength, this->filtering_exponent)*MILLILITER_TO_VOLUME_PER_CELL;
#else
	biomassType usedFiltering = this->filtering_rate_per_individual_in_cell_volume;
#endif
#ifdef TEMPERATURE_MEDIATED_GRAZING
	usedFiltering*=localeTemperature*this->consumption_temperature_factor;
#endif
//	usedFiltering=0.0f;
	consumption_per_individual = usedFiltering*foodBiomassInMicrograms*stroganov_adjustment;
#ifdef SATURATION_GRAZING
	consumption_per_individual = min<biomassType>(FEEDING_SATURATION,MAXIMUM_GRAZING_ABSORBED);
//	grazing_per_individual = min<biomassType>(FEEDING_SATURATION,grazing_per_individual);
#endif
	locale_consumption= consumption_per_individual*animalCount;
	locale_consumption_salt_adjusted=locale_consumption*salinity_effect_matrix[depthIndex][columnIndex];
	/* Grazing can be adjusted according to water salinity*/
#ifdef ADJUST_SALINITY_GRAZERS
	used_consumption=locale_consumption_salt_adjusted;
#else
	used_consumption=locale_consumption;
#endif
	 used_consumption=min<biomassType>(foodBiomassInMicrograms, used_consumption);
	 used_consumption*=consumedProportion;
	 biomassType updatedAlgaeBiomassInMicrograms = foodBiomassInMicrograms - used_consumption;
	 if(used_consumption<0){
		 cout<<"Error. Negative used consumption."<<endl;
	 }
	 biomassType updatedConcentration = updatedAlgaeBiomassInMicrograms/CELL_VOLUME_IN_LITER;
#ifdef GRAZING_EFFECT_ON_ALGAE_BIOMASS
	if(bottom){
		bottomFoodBiomass[columnIndex]= updatedConcentration;
	} else{
		floatingFoodBiomass[depthIndex][columnIndex] = updatedConcentration;
	}
#endif
}


void GrazerBiomassDynamics::calculatePredatorBiomass(){
	/* Select biomass center according to light*/
	physicalType planktivoreBiomassFactor = this->dayTime?this->kairomones_level_day:this->kairomones_level_night;
	int biomassCenter = this->dayTime?this->planktivore_biomass_center_day:planktivore_biomass_center_night;
	for(int columnIndex=0; columnIndex<MAX_COLUMN_INDEX; ++columnIndex){
		/* Planktivore migration is limited by lake depth */
		int localeDepth=maxDepthIndex[columnIndex];
		int localePlanktivoreBiomassCenter = min<int>(localeDepth, biomassCenter);
		/* Planktivores are present in a depth-limited band*/
		int initialPlanktivoreBiomassDepth = max<int>(0, localePlanktivoreBiomassCenter-this->planktivore_biomass_width),
				finalPlanktivoreBiomassDepth = min<int>(localeDepth, localePlanktivoreBiomassCenter+this->planktivore_biomass_width);
		for(int depthIndex=0; depthIndex<=localeDepth; ++depthIndex){
			if(depthIndex>=initialPlanktivoreBiomassDepth&&depthIndex<=finalPlanktivoreBiomassDepth){
				/* If the depth is within this band, use lighttime dependent biomass*/
				this->predatorBiomass[depthIndex][columnIndex]=planktivoreBiomassFactor;
			}
			else{
				/*Otherwise, clear fish biomass*/
				this->predatorBiomass[depthIndex][columnIndex]=0.0f;
			}

		}
	}

}

void GrazerBiomassDynamics::findNormalizingFactors(){
	averagePredatorSafety=averageFood=0.0f;
#ifndef THRESHOLD_LIGHT_SAFETY
	averageLightSafety=0.0f;
#endif

	/* A counter for the number of cells in the system*/
	for(int columnIndex=0; columnIndex<MAX_COLUMN_INDEX; ++columnIndex){
		for(int depthIndex=0; depthIndex<=maxDepthIndex[columnIndex]; ++depthIndex){
			/*Calculate predator and light safety*/
			this->predatorSafety[depthIndex][columnIndex]=calculatePredatorSafety(depthIndex, columnIndex);
#ifndef THRESHOLD_LIGHT_SAFETY
			this->lightSafety[depthIndex][columnIndex]=this->light_safety_weight*(1-1/(1+exp(-this->lakeLightAtDepth[depthIndex][columnIndex] + this->maximum_light_tolerated)));
			averageLightSafety+=this->lightSafety[depthIndex][columnIndex];
#endif
			/*Light safety is 1 for safe light and 0 for burning light*/
//			this->lightSafety[depthIndex][columnIndex]=this->lakeLightAtDepth[depthIndex][columnIndex]>this->maximum_light_tolerated?0.0f:1.0f;
			/*Average predator, light and food safety across al cells */
			averagePredatorSafety+=predatorSafety[depthIndex][columnIndex];
			averageFood+=getFoodBiomass(false, depthIndex, columnIndex);

		}
	}
	/* Calculate the average of fitness for each metrics*/
	averagePredatorSafety/=cell_counter;
	averageFood/=cell_counter;
#ifndef THRESHOLD_LIGHT_SAFETY
	averageLightSafety/=cell_counter;
#endif
	/*Normalize fitness values using the summing at the current time*/
	for(int columnIndex=0; columnIndex<MAX_COLUMN_INDEX; ++columnIndex){
			for(int depthIndex=0; depthIndex<=maxDepthIndex[columnIndex]; ++depthIndex){
				/*Sum the inverse of the distance to optimal light*/
				this->predatorSafety[depthIndex][columnIndex]/=this->averagePredatorSafety;
				this->normalizedFloatingFoodBiomass[depthIndex][columnIndex]=this->floatingFoodBiomass[depthIndex][columnIndex]/this->averageFood;
#ifndef THRESHOLD_LIGHT_SAFETY
				this->lightSafety[depthIndex][columnIndex]/=this->averageLightSafety;
#endif
			}
		}
//#ifdef MINIMUM_PREDATION_SAFETY
//	sumOptimalPredatorSafetyValues=1.0f;
//#endif
}


void GrazerBiomassDynamics::migrateJuvenileCohortDepthDependent(AnimalCohort& cohort){
	if(cohort.numberOfIndividuals>0&&cohort.bodyBiomass>0.0f){
				/* Migrate non-empty cohorts */
	#ifdef LIGHT_BASED_MIGRATION_VARIABLE_FREQUENCY
				int migratedDepth = migrateCohortsDepthDependent(cohort);
	#elif defined(LIGHT_BASED_MIGRATION_FIXED_FREQUENCY)
				int migratedDepth = migrateCohortsFixedFrequency(cohort);
	#elif defined(OPTIMAL_VALUE_MIGRATION)
				if(abs(cohort.migrationConstant)>2){
					cout<<"Error on migration constant: "<<cohort.migrationConstant<<"."<<endl;
				}
		int migratedDepth = cohort.migrationConstant + optimalDepthIndexes[cohort.y]-cohort.x;
	#else
				int migratedDepth = 0;
	#endif
				if(migratedDepth!=0){
					/* Consume algae during migration */
					int initialDepth=cohort.x;
					cohort.x+=migratedDepth;
					cohort.x=max<int>(0, min<int>(maxDepthIndex[cohort.y], cohort.x));
					int finalDepth=cohort.x;
#ifdef CONSUME_DURING_MIGRATION
					consumeDuringMigration(initialDepth, finalDepth, cohort);
#endif
#ifdef LIGHT_BASED_MIGRATION_FIXED_FREQUENCY
					if(cohort.x != 0 && migratedDepth != -this->velocity_downward_pull && lakeLightAtDepth[cohort.x][cohort.y]<this->critical_light_intensity){
						/*If the cohort is not at the surface (cohort.x!=0), and the migration speed is not maximum upwards, and it is too dark for the cohort, report an error*/
						cout<<"Juvenile lake light at coordinates: "<<cohort.x<<", "<<cohort.y<<" is "<<critical_light_intensity<<"."<<endl;
					}
#endif
				}

			}
}
void GrazerBiomassDynamics::migrateAdultCohortsDepthDependent(std::map<pair<int,int>,AnimalCohort>* animals){
	for (std::map<pair<int,int>,AnimalCohort>::iterator it = animals->begin();
				it != animals->end(); ++it) {
		migrateAdultCohort(it->second);
		/*If the cohort has migrated, remove it from its previous location*/

	}
}


/*There exist 3 types of migration:
 * Adults: The number of cohorts is constant. During migration, if a destination cohort exists, it is updated with the immigrant individuals. If not, a new cohort is created.
 * Juveniles. The number of cohorts changes over time. New cohorts are created (recruited from eggs) and removed (promoted to adults). During migration, the cohort coordinates are updated indivdually.
 * Traced adults. They are adults whose cohort needs to be traced individually. Therefore, they are traced as juveniles.*/
void GrazerBiomassDynamics::migrateAdultCohort(AnimalCohort& cohort){
	if(cohort.numberOfIndividuals>0&&cohort.bodyBiomass>0.0f){
		/* Migrate non-empty cohorts */
#ifdef LIGHT_BASED_MIGRATION_VARIABLE_FREQUENCY
		int migratedDepth = migrateCohortsDepthDependent(cohort);
#elif defined(LIGHT_BASED_MIGRATION_FIXED_FREQUENCY)
		int migratedDepth = migrateCohortsFixedFrequency(cohort);
		/*Update the migration table*/
#elif defined(OPTIMAL_VALUE_MIGRATION)
		if(abs(cohort.migrationConstant)>2){
			cout<<"Error on migration constant: "<<cohort.migrationConstant<<"."<<endl;
		}
		int migratedDepth = cohort.migrationConstant + optimalDepthIndexes[cohort.y]-cohort.x;
		cohort.latestMigrationIndex = migratedDepth;
#else
		int migratedDepth = 0;
#endif
		if(migratedDepth!=0){
			/* The destination depth is bounded between the surface and the maximum depth in the column*/
			int destinationDepth=max(0,min((int)maxDepthIndex[cohort.y], migratedDepth+cohort.x));
			migratedDepth=destinationDepth-cohort.x;
			if(migratedDepth!=0){
				/*After limiting to upper and lower depth boundaries, the updated migration depth can be 0*/
#ifdef CONSUME_DURING_MIGRATION
				consumeDuringMigration(cohort.x, destinationDepth, cohort);
#endif
				updateMigrationTable(cohort, migratedDepth);
			}
		}
	}
}

/* Migrate juvenile cohorts assuming a stochastic approach. Cohorts might move to a suboptimal cell to escape from local maxima*/
void GrazerBiomassDynamics::migrateCohortUsingRandomWalk(AnimalCohort& cohort){
	generateMigrationIndexes();
	double searchStepCounter=1;
	int localeVerticalCoordinate=cohort.x, localeHorizontalCoordinate=cohort.y;
	//	if(currentCellTooDark){
	//		cout<<"Cell too dark."<<endl;
	//	}
#ifdef	LINEAR_MIGRATION_COMBINATION
	biomassType originFitnessValue = localeFitnessValue[localeVerticalCoordinate][localeHorizontalCoordinate];
#else
	/* If the model is running in rule-based mode, only expose to migration when starvation occurs. Otherwise, follow the standard circadian rhythm*/
	biomassType originFitnessValue = predatorSafety[localeVerticalCoordinate][localeHorizontalCoordinate];
	bool starvationMode = cohort.hoursInStarvation>this->starvation_max_hours;
	if(starvationMode){
		originFitnessValue = localeFitnessValue[localeVerticalCoordinate][localeHorizontalCoordinate];
	}
#endif
	/* Check if the group is in a cell that is too predated. Therefore, daphnia will migrate out of it*/
	biomassType originPredationSafety=cohort.currentPredatorSafety=this->predatorSafety[localeVerticalCoordinate][localeHorizontalCoordinate],
			originFoodBiomass=cohort.currentFoodBiomass=this->normalizedFloatingFoodBiomass[localeVerticalCoordinate][localeHorizontalCoordinate];
#ifdef THRESHOLD_LIGHT_SAFETY
	physicalType originLight= this->lakeLightAtDepth[localeVerticalCoordinate][localeHorizontalCoordinate];
#else
	physicalType originLightSafety= this->lightSafety[localeVerticalCoordinate][localeHorizontalCoordinate];
#endif
	bool currentCellTooPredated= originPredationSafety<this->minimum_predation_safety,
			currentCellTooLuminous=this->lakeLightAtDepth[localeVerticalCoordinate][localeHorizontalCoordinate]>this->maximum_light_tolerated;
	cohort.predatorFitness=currentCellTooPredated?predatorFitnessType::Predator:predatorFitnessType::Food;
	/* If the cell is too predated, the fitness value is predator safety. Otherwise, the fitness value is food*/
	originFitnessValue = currentCellTooPredated?this->predatorSafety[localeVerticalCoordinate][localeHorizontalCoordinate]:
			this->normalizedFloatingFoodBiomass[localeVerticalCoordinate][localeHorizontalCoordinate];
#ifndef THRESHOLD_LIGHT_SAFETY
	originFitnessValue+=originLightSafety;
#endif
	cohort.previousFitnessValue =cohort.currentFitnessValue= originFitnessValue;
	for (std::vector<pair<int,int>>::iterator migrationIndexPair = hourlyMigrationIndexPairs->begin();
			migrationIndexPair != hourlyMigrationIndexPairs->end()&&searchStepCounter<=this->max_search_steps; ++migrationIndexPair) {
		/* Retrieve migration indexes*/
		int verticalIndex=migrationIndexPair->first, horizontalIndex=migrationIndexPair->second;
		/* Calculate the destination coordinates as the current coordinates plus the migration indexes*/
		int destinationVertical = localeVerticalCoordinate+verticalIndex, destinationHorizontal = localeHorizontalCoordinate+horizontalIndex;
		if(destinationHorizontal>=0&&destinationHorizontal<=MAX_COLUMN_INDEX){
			if(destinationVertical>=0&&destinationVertical<=MAX_DEPTH_INDEX){
				if(maxDepthIndex[destinationHorizontal]>=destinationVertical){
					/*If the cell exists, attempt movement*/

					/* Calculate destination predation safety and food*/
					biomassType destinationPredationSafety = this->predatorSafety[destinationVertical][destinationHorizontal],
											destinationFoodBiomass=this->normalizedFloatingFoodBiomass[destinationVertical][destinationHorizontal];
					/* Retrieve destination light*/

#ifdef THRESHOLD_LIGHT_SAFETY
					physicalType destinationLight = this->lakeLightAtDepth[destinationVertical][destinationHorizontal];
					bool destinationLightSafe=(destinationLight<=this->maximum_light_tolerated),
							destinationLightLower=(destinationLight<originLight);
#else
					physicalType destinationLightSafety= this->lightSafety[localeVerticalCoordinate][localeHorizontalCoordinate];
					bool destinationLightSafe=(destinationLightSafety*averageLightSafety>this->light_safety_threshold),
							destinationLightLower=(destinationLightSafety>originLightSafety);
#endif
					if(((!destinationLightSafe)&&lakeLightAtDepth[destinationVertical][destinationHorizontal]<(this->maximum_light_tolerated-2))||
							(destinationLightSafe&&lakeLightAtDepth[destinationVertical][destinationHorizontal]>(this->maximum_light_tolerated))){
						cout<<"Light threshold limit incorrect."<<endl;
					}
//					if((!destinationLightSafe)){
//						cout<<"Zero destination light safety."<<endl;
//					}

					/* Test if the predation level and destination light are acceptable*/
					bool destinationSafe=(destinationPredationSafety>=this->minimum_predation_safety)&&destinationLightSafe,
					/* If not, test if the current cell is too predated and the destination cell is less predated*/
					destinationBetterPredation=(currentCellTooPredated&&destinationLightSafe&&(destinationPredationSafety>originPredationSafety)),
					/* If not, test if the current cell is too luminous and the destination cell is less luminous*/
					destinationBetterLight=(currentCellTooLuminous&&destinationLightLower);
					/* Test that any of the conditions that make the destination cell a good candidate holds*/
					bool destinationCellReachable=destinationSafe||destinationBetterPredation||destinationBetterLight;
					if(destinationCellReachable){
//						if(!destinationLightSafe){
//							cout<<"Destination with better light."<<endl;
//						}
						/*If the cell is safe enough, or the current cell is unsafe and the destination is safer*/
#ifdef	LINEAR_MIGRATION_COMBINATION
						biomassType destinationFitnessValue = localeFitnessValue[destinationVertical][destinationHorizontal];
#else
						/* If the model is running in rule-based mode, only expose to migration when starvation occurs. Otherwise, follow the standard circadian rhythm*/
						biomassType destinationFitnessValue = this->predatorSafety[destinationVertical][destinationHorizontal];
						if(starvationMode){
							destinationFitnessValue = this->localeFitnessValue[destinationVertical][destinationHorizontal];
						}
#endif
						/* If the cell is too predated, the fitness value is predator safety. Otherwise, the fitness value is food*/
						destinationFitnessValue = currentCellTooPredated?this->predatorSafety[destinationVertical][destinationHorizontal]:
								this->normalizedFloatingFoodBiomass[destinationVertical][destinationHorizontal];
#ifndef THRESHOLD_LIGHT_SAFETY
//						if(destinationFitnessValue>destinationLightSafety){
//							cout<<"Light not dominant."<<endl;
//						}
						destinationFitnessValue+=destinationLightSafety;
#endif
						bool destinationSafetyGreater=destinationPredationSafety>originPredationSafety,
								destinationFoodGreater=destinationFoodBiomass>originFoodBiomass;
						/* Marker to check if the cohort moves or not*/
						bool groupMigrated=false;
						if(((!currentCellTooPredated)&&destinationFoodGreater)||(currentCellTooPredated&&destinationSafetyGreater)){
							/*If the current cell is too predated, maximize safety. Otherwise, maximize food */
							groupMigrated=true;

						} else{
							/*Otherwise, move it with a certain probability proportional to the fitness difference*/
							double randomNumber = ((double) rand() / (RAND_MAX));
							double acceptanceProbabilityExponent = (destinationFitnessValue-originFitnessValue)/(this->random_walk_probability_weight);
							double movementProbability = exp(acceptanceProbabilityExponent);
							if(randomNumber<=movementProbability){
								groupMigrated=true;
							}
						}
						if(groupMigrated){
							/* If the group migrates, update the values*/
							cohort.x=destinationVertical;
							cohort.y=destinationHorizontal;
							cohort.currentPredatorSafety=destinationPredationSafety,
									cohort.currentFoodBiomass=destinationFoodBiomass,
									cohort.currentFitnessValue=destinationFitnessValue;
							/*Update predation and food values*/
							originPredationSafety=destinationPredationSafety;
							originFoodBiomass=destinationFoodBiomass;

							cohort.predatorFitness=currentCellTooPredated?predatorFitnessType::Predator:predatorFitnessType::Food;
							/*Update cell status*/
							currentCellTooPredated= originPredationSafety<this->minimum_predation_safety;
#ifdef THRESHOLD_LIGHT_SAFETY
							originLight=destinationLight;
							currentCellTooLuminous=localeLight>this->maximum_light_tolerated;
#else
							originLightSafety=destinationLightSafety;
							currentCellTooLuminous=originLightSafety>this->light_safety_threshold;
#endif

						}
						/* Update the number of search steps per group*/
						searchStepCounter++;
					}
				}
			}
		}
	}
	//(*it)*=0.9f;
	/*Migrate traced cohort as a special case*/
}


int GrazerBiomassDynamics::migrateCohortsFixedFrequency(AnimalCohort& cohort){
	int depthChange=0;

	/*We assume constant upward and downward migration velocity*/

	int indexMoved = this->velocity_downward_pull;
	physicalType cohortLightIntensity = lakeLightAtDepth[cohort.x][cohort.y];
	if((cohortLightIntensity>=this->critical_light_intensity)&&(((*this->current_hour%HOURS_PER_DAY>=HOURS_PER_DAY/4)&&(*this->current_hour%HOURS_PER_DAY<HOURS_PER_DAY/2))||((*this->current_hour%HOURS_PER_DAY>=HOURS_PER_DAY*3/4)&&(*this->current_hour%HOURS_PER_DAY<HOURS_PER_DAY)))){
		/*If the current time is not a flank (moving upward or downward) and the light is not too dark, the cohort remains at the same depth*/
		indexMoved = 0;
	}

	/*The clock whose flanks trigger upward and downward migration has a period of 24 hours*/
	if(*this->current_hour%HOURS_PER_DAY==0){
		/* Each 24*t hours, the migration becomes downwards */
		cohort.upDirection=false;
	} else {
		if (*this->current_hour%HOURS_PER_DAY==HOURS_PER_DAY/2){
			/* Each 24*t+12 hours, the migration becomes upwards */
			cohort.upDirection=true;
		}
	}

	if(cohort.upDirection){
		int destinationDepth=cohort.x;
		/* If the cohort is moving upwards and the migration does not overflow the surface, the cohort migrates upwards*/
		destinationDepth=max<int>(0,cohort.x-indexMoved);
		depthChange = -indexMoved;
//		else{
//			cout<<"Upward migration not completed at origin depth: "<<cohort.x<<" and destination depth "<<destinationDepth<<"."<<endl;
//		}
	} else{
		/* Migration downwards is limited by the depth of the lake*/
		int movementDownwards= min<int>(cohort.x+indexMoved,maxDepthIndex[cohort.y]);
		/* The destination depth is the final depth minus the current depth*/
		movementDownwards-=cohort.x;
			physicalType destinationLightIntensity  = lakeLightAtDepth[cohort.x+movementDownwards][cohort.y];
			if(destinationLightIntensity>=this->critical_light_intensity){
				/* If the cohort is moving downwards and the migration does not reach too dark areas, the cohort migrates downwards*/
				depthChange = movementDownwards;
			} else{
				/* If the cohort is moving downwards and the migration reaches too dark areas, downward migration stops at the deepest area that it is not too dark*/
				int i=cohort.x+movementDownwards-1;
				for(;lakeLightAtDepth[i][cohort.y]<this->critical_light_intensity&&i-cohort.x>= -(this->velocity_downward_pull-1) ;i--);
				depthChange = i-cohort.x;

			}
//			else{
////				cout<<"Downward migration not completed at origin depth: "<<cohort.x<<", destination depth "<<destinationDepth<<" and destination light intensity "<<destinationLightIntensity<<"."<<endl;
////				if(traceCohort&&cohort.cohortID==this->tracedCohortID){
////					cout<<"Traced cohort moved."<<endl;
////				}
//			}


	}
	if(cohort.x + depthChange != 0 && !cohort.upDirection&&(depthChange!= -this->velocity_downward_pull &&lakeLightAtDepth[cohort.x + depthChange][cohort.y]<this->critical_light_intensity)){
		/*If the cohort is not at the surface (cohort.x!=0), and the migration speed is not maximum upwards, and it is too dark for the cohort, report an error*/
			cout<<"Adult lake light at coordinates: "<<cohort.x+depthChange<<", "<<cohort.y<<" is "<<critical_light_intensity<<"."<<endl;
		}
	cohort.latestMigrationIndex = depthChange;
	return depthChange;

}

int GrazerBiomassDynamics::migrateCohortsDepthDependent(AnimalCohort& cohort){

	physicalType downwardPull = 0.0f, downwardPullDepthCosine=0.0f, downwardPullLightCosine =0.0f;
	physicalType cohortDepth=  indexToDepth[cohort.x], cohortLightIntensity = lakeLightAtDepth[cohort.x][cohort.y];

	/*Get absolute value of relative light change (RLC)*/
	physicalType relativeLightChange = lakeLightAtDepth[cohort.x][cohort.y] - previousLakeLightAtDepth[cohort.x][cohort.y];
	relativeLightChange=relativeLightChange>0?relativeLightChange:-relativeLightChange;

	/*Adjust downward pull according to depth and light intensity*/
	if(cohortDepth<this->critical_depth){

		/* The cohort has reached the surface, so it will start moving downwards*/
		cohort.upDirection=false;
	}
	else {
		if(cohortLightIntensity<this->critical_light_intensity){
			/* The cohort has reached the lowest light level, so it will start moving upwards*/
			cohort.upDirection=true;
		}
	}
	if(cohort.upDirection){
		downwardPullLightCosine = cos(Math_PI*cohortLightIntensity/(2*this->critical_light_intensity));
		if(downwardPullLightCosine<0){
			cout<<"Unexpected upward migration: "<<downwardPullLightCosine<<"."<<endl;
		}
		downwardPull -= downwardPullLightCosine*this->velocity_downward_pull;
	} else{
		downwardPullDepthCosine =cos(Math_PI*cohortDepth/(2*this->critical_depth));
		if(downwardPullLightCosine<0){
			cout<<"Unexpected downward migration: "<<downwardPullDepthCosine<<"."<<endl;
		}
		downwardPull += downwardPullDepthCosine*this->velocity_downward_pull;
	}


	/* Convert from meters to depth intensity*/
	int downwardPullIndex = (downwardPull/ZMax)*MAX_DEPTH_INDEX;
//	if(downwardPullIndex!=0){
//		cout<<"Downward pull index modified."<<endl;
//	}
//	if(downwardPullIndex<0){
//		cout<<"Downward pull index negative: "<<downwardPullIndex<<", depth index: "<<cohort.x<<", light at top: "<<lakeLightAtDepth[0][cohort.y]<<"."<<endl;
//	}
	return downwardPullIndex;

}


/* Predation of cohorts based on predator biomass*/
void GrazerBiomassDynamics::predateCohort(AnimalCohort& cohort){
	if(predatorBiomass[cohort.x][cohort.y]>0.0f){
				/* If there are plantivores in the region, consume grazer biomass*/
				/* Calculate predated biomass*/
				biomassType predationFactor=predatorBiomass[cohort.x][cohort.y]*this->predation_index;
				biomassType grazerPredatedBiomass=predationFactor*cohort.bodyBiomass;
				animalCountType localePredatedIndividuals = grazerPredatedBiomass/this->initial_grazer_weight[cohort.stage];
				this->predatedIndividuals[cohort.x][cohort.y]+= localePredatedIndividuals;
				/* Remove predated individuals*/
				cohort.numberOfIndividuals=max<animalCountType>(0,cohort.numberOfIndividuals-localePredatedIndividuals);
				cohort.bodyBiomass=max<animalCountType>(0.0f,cohort.bodyBiomass-grazerPredatedBiomass);
			}
}


physicalType GrazerBiomassDynamics::calculatePredatorSafety(int depthIndex, int columnIndex){
	/* Distance to optimal light is used for fitness*/
	physicalType localeLakeLightAtDepth = lakeLightAtDepth[depthIndex][columnIndex];
//	return 1.0f/fabs((localeLakeLightAtDepth-light_optimal_value)+1);
	/* To model risk of predation, light is multiplied by planktivore biomass*/
	biomassType localePredatorBiomass = this->predatorBiomass[depthIndex][columnIndex];
	biomassType calculatedLightPropensity = 1.0f/((localeLakeLightAtDepth*localePredatorBiomass)+1.0f);
//	if(calculatedLightPropensity!=1.0f){
//		cout<<"Light propensity greater than 0."<<endl;
//	}
	return calculatedLightPropensity;
}


/* If the migration index is greater than 0, migrate adult and juvenile cohorts*/

void GrazerBiomassDynamics::calculateMigrationValues(){
	calculatePredatorBiomass();
	generateMigrationIndexes();
	findNormalizingFactors();
	findOptimalDepthIndexes();
}


void GrazerBiomassDynamics::findOptimalDepthIndexes(){
	for (int columnIndex = 0; columnIndex < MAX_COLUMN_INDEX; ++columnIndex) {
		findOptimalDepthIndex(columnIndex);
	}
//	for (int columnIndex = 0; columnIndex <= MAX_COLUMN_INDEX; ++columnIndex) {
//		if(maxDepthIndex[columnIndex]!=optimalDepthIndex[columnIndex]){
//			cout<<"Difference found."<<endl;
//		}
//	}
}
void GrazerBiomassDynamics::findOptimalDepthIndex(unsigned int columnIndex){

	int optimalDepthIndex=0;
	/*Calculate the value to optimize as the weighted sum of the inverse of the distance to optimal light normalized and the amount of food normalized*/
	physicalType predatorSafetyValueToOptimize=(light_migration_weight)*predatorSafety[0][columnIndex];
	physicalType foodValueToOptimize =(1-light_migration_weight)*normalizedFloatingFoodBiomass[0][columnIndex];
	physicalType valueToOptimize = predatorSafetyValueToOptimize+foodValueToOptimize;
	for (int depthIndex = 1; depthIndex <= maxDepthIndex[columnIndex]; ++depthIndex) {
		/*Inverse of the distance to optimal light normalized*/
		physicalType localePredationSafety = light_migration_weight*predatorSafety[depthIndex][columnIndex];
		/*Amount of food normalized*/
		physicalType localeFood=(1-light_migration_weight)*normalizedFloatingFoodBiomass[depthIndex][columnIndex];
#ifdef MINIMUM_PREDATION_SAFETY
		/* If we are assuming that the predation safety must be above a minimum, compare it with */
		physicalType localeComposedValue = localePredationSafety;
		if(localePredationSafety>this->minimum_predation_safety){
			localeComposedValue=localeFood;
		}
#else
		physicalType localeComposedValue = localePredationSafety+localeFood;
#endif
		/* Register the locale fitness value*/
		localeFitnessValue[depthIndex][columnIndex] = localeComposedValue;
		if(localeComposedValue>valueToOptimize){
			/* Update of the new value is greater than the previous*/
			valueToOptimize = localeComposedValue;
			optimalDepthIndex=depthIndex;
//			if(tracedCohort.numberOfIndividuals!=0){
//				cout<<"Better food detected."<<endl;
//			}
		}
	}
	optimalDepthIndexes[columnIndex] = optimalDepthIndex;
//	if(tracedCohort.numberOfIndividuals>0&&columnIndex==tracedCohort.y&&optimalDepthIndex>2){
//		cout<<"Index for traced cohort is "<<optimalDepthIndex<<"."<<endl;
//	}

}



} /* namespace FoodWebModel */
