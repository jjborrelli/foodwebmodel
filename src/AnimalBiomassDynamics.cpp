/*
 * AnimalBiomassDynamics.cpp
 *
 *  Created on: Jul 6, 2017
 *      Author: manu_
 */
#include <algorithm> // for remove_if
#include <functional> // for unary_function
#include <iterator>
#include <random> // for shuffle
#include "../headers/AnimalBiomassDynamics.hpp"

using namespace std;

std::ostream& operator<<(std::ostream& os, const AnimalCohort& cohort){
	os<<"(Animal cohort. ID: "<<cohort.cohortID<<", x: "<<cohort.x<<", y: "<<cohort.y<<", number of indviduals: "<<cohort.numberOfIndividuals<<", body biomass: "<<cohort.bodyBiomass<<", gonad biomass: "<<cohort.gonadBiomass;
	os<<", stage: "<<cohort.stage;
	os<<", is bottom animal: "<<(cohort.isBottomAnimal?1:0)<<")";
	return os;

}


std::ostream& operator<<(std::ostream& os, const EggCohort& cohort){
	os<<"(Egg cohort. ID: "<<cohort.cohortID<<", x: "<<cohort.x<<", y: "<<cohort.y<<", age: "<<cohort.ageInHours<<", number of eggs: "<<cohort.numberOfEggs<<", biomass: "<<cohort.biomass;
	os<<", is bottom egg: "<<(cohort.isBottomAnimal?1:0)<<")";
	return os;
}
void operator+=(AnimalCohort& cohort1, const AnimalCohort& cohort2){
	animalCountType totalAnimals = cohort1.numberOfIndividuals + cohort2.numberOfIndividuals;
	float animalProportion1 = (float)cohort1.numberOfIndividuals/(float)totalAnimals, animalProportion2=(float)cohort2.numberOfIndividuals/(float)totalAnimals;
	cohort1.ageInHours=(cohort1.ageInHours*animalProportion1+cohort2.ageInHours*animalProportion2);
	cohort1.bodyBiomass+=cohort2.bodyBiomass;
	cohort1.numberOfIndividuals+=cohort2.numberOfIndividuals;
	cohort1.gonadBiomass+=cohort2.gonadBiomass;
	cohort1.latestMigrationIndex=cohort2.latestMigrationIndex;
	cohort1.migrationConstant=cohort2.migrationConstant;
	cohort1.hoursInStarvation=max<int>(cohort1.hoursInStarvation,cohort2.hoursInStarvation);
}

void operator-=(AnimalCohort& cohort1, const AnimalCohort& cohort2){
	animalCountType totalAnimals = cohort1.numberOfIndividuals + cohort2.numberOfIndividuals;
	float animalProportion1 = (float)cohort1.numberOfIndividuals/(float)totalAnimals, animalProportion2=(float)cohort2.numberOfIndividuals/(float)totalAnimals;
	cohort1.ageInHours=(cohort1.ageInHours*animalProportion1+cohort2.ageInHours*animalProportion2);
	cohort1.bodyBiomass -= cohort2.bodyBiomass;
	cohort1.numberOfIndividuals -= cohort2.numberOfIndividuals;
	cohort1.gonadBiomass -= cohort2.gonadBiomass;
	cohort1.hoursInStarvation=max(cohort1.hoursInStarvation,cohort2.hoursInStarvation);


}


void operator+=(EggCohort& cohort1, const EggCohort& cohort2){
	cohort1.biomass += cohort2.biomass;
	cohort1.numberOfEggs += cohort2.numberOfEggs;
}

void operator+=(AnimalCohort& cohort1, const EggCohort& cohort2){
	cohort1.bodyBiomass += cohort2.biomass;
	cohort1.numberOfIndividuals += cohort2.numberOfEggs;
	 if(cohort1.numberOfIndividuals<0){
		 cout<<"Error. Negative number of individuals."<<endl;
	 }

}

void operator*=(AnimalCohort& cohort1, const double number){
	cohort1.bodyBiomass*=number;
	cohort1.gonadBiomass*=number;
	cohort1.numberOfIndividuals*=number;
	 if(cohort1.numberOfIndividuals<0){
		 cout<<"Error. Negative product of individuals."<<endl;
	 }

}


namespace FoodWebModel {

AnimalBiomassDynamics::AnimalBiomassDynamics():animalRandomGenerator(NULL) {
	tracedCohort.numberOfIndividuals=0;
	// TODO Auto-generated constructor stub

}

AnimalBiomassDynamics::~AnimalBiomassDynamics() {
	delete animalRandomGenerator;
}

void AnimalBiomassDynamics::initializeSimulationStructures(){
	for (int horizontalIndex = -this->max_horizontal_migration; horizontalIndex <=this->max_horizontal_migration; ++horizontalIndex) {
		for (int verticalIndex = -this->max_vertical_migration; verticalIndex <=this->max_vertical_migration; ++verticalIndex) {
			/* The migration pairs will be shuffled each simulation step*/
#ifdef DAPHNIA_CIRCADIAN_CYCLE
			pair<int,int> migrationPair=std::make_pair(verticalIndex, horizontalIndex);
			if(verticalIndex>=0){
				this->daytimeMigrationIndexPairs.push_back(migrationPair);
			}
			if(verticalIndex<=0){
				this->nighttimeMigrationIndexPairs.push_back(migrationPair);

			}
#else
			this->migrationIndexPairs.push_back(std::make_pair(verticalIndex, horizontalIndex));
#endif
		}
	}


}


void AnimalBiomassDynamics::takeAnimalDynamicsStep(){
	this->dayTime = (*current_hour%HOURS_PER_DAY)<(HOURS_PER_HALF_DAY);
	updateAnimalBiomass();
	migrateAnimalCohorts();
#ifndef ANIMAL_COHORT_MAP
	removeEmptyCohorts();
	reallocateSmallCohorts();
#endif
}

void AnimalBiomassDynamics::reportAssertionError(int depthIndex, int columnIndex, biomassType biomass, biomassType previousBiomass, biomassType differential, bool isBottom) {
	#ifdef CHECK_ASSERTIONS
  unsigned int isBottomAsInt=(isBottom?1:0);
	if (isnan((float)biomass)||isinf((float)biomass)) {
		(*assertionViolationBuffer) << "NanInfAnimal; Biomass: " << "Biomass: "
				<< biomass << ", Depth: "<<depthIndex<<", Column: "
				<< columnIndex << ", Hour: " << (*current_hour) << ", IsBottom: "<<isBottomAsInt << endl;
	}
	if (biomass < 0.0f) {
		(*assertionViolationBuffer) << "NegativeAnimal; Biomass: "
				<< biomass << ", Depth: "<<depthIndex<<", Column: "
				<< columnIndex << ", Hour: " << (*current_hour) << ", IsBottom: "<<isBottomAsInt << endl;
	}
 #endif
	/*if (biomass
			!= previousBiomass + differential) {
		(*assertionViolationBuffer) << "UpdateAnimal; Biomass: "
				<< biomass << ", PreviousBiomass: "
				<< previousBiomass << ", BiomassDiff: "
				<< differential << ", BiomassError: "
				<< biomass
						- (previousBiomass
								+ differential)
				<< ", Depth: "<<depthIndex<<", Column: " << columnIndex << ", Hour: " << (*current_hour)
				<< ", IsBottom: "<<isBottomAsInt << endl;
	}*/
}

#ifdef ANIMAL_COHORT_MAP
void AnimalBiomassDynamics::updateCohortBiomassForAnimals(std::map<pair<int,int>,AnimalCohort> *animals) {//
#else
void AnimalBiomassDynamics::updateCohortBiomassForAnimals(std::vector<AnimalCohort> *animals) {//
#ifndef CONGLOMERATE_ALL_COHORTS
	floatingReallocatedCohorts.clear();
	bottomReallocatedCohorts.clear();
#endif
#endif
	int iteratorCounter=0;
#ifdef ANIMAL_COHORT_MAP
	for (std::map<pair<int,int>,AnimalCohort>::iterator it = animals->begin();
			it != animals->end();++it) {
		AnimalCohort* cohort=&(it->second);
#else
		for (std::vector<AnimalCohort>::iterator it = animals->begin();
				it != animals->end();++it) {
			AnimalCohort* cohort=&*it;
#endif
		if(cohort->stage==AnimalStage::Egg){
			cout<<"Error. Egg cohort updated."<<endl;
		}
		iteratorCounter++;
		if(cohort->numberOfIndividuals>0&&cohort->bodyBiomass>0){
			/* Update biomass of non-empty cohorts */
			updateCohortBiomass(*cohort);
		}

	}


//	std::vector<pair<int,int>> emptyCohorts;
//	for (std::map<pair<int,int>,AnimalCohort>::iterator it = animals->begin();
//				it != animals->end();++it) {
//		/* Remove cohort if the number of animals or its biomass is 0*/
//		if(it->second.numberOfIndividuals<=0||it->second.bodyBiomass<=0.0f){
//			/*We cannot remove elements while iterating map entries, so we store them for later deletion*/
//			emptyCohorts.push_back(it->first);
//		}
//	}
	/*Remove empty cohorts*/
//	iteratorCounter=0;
//	for (std::vector<pair<int,int>>::iterator it = emptyCohorts.begin();
//					it != emptyCohorts.end();++it) {
//		iteratorCounter++;
////		animals->erase(*it);
//	}
}

/* Calculation of grazer biomass (AquaTox Documentation, page 100, equation 90)*/

void AnimalBiomassDynamics::updateAnimalBiomass(){
/* Clear string buffers */
	animalBiomassBuffer.str("");
	/* Clear zooplankton count summing. This will be compared with threshold upper number as a halting condition*/
	this->floating_animal_count_summing=0;
	/*Matrix to store the decision of biomass must be saved. It will be read when registering slough to output slough file*/
	bool registerZooplanktonBiomass[MAX_DEPTH_INDEX][MAX_COLUMN_INDEX];
	/* Clear vertical migrated phyto biomass*/
#ifndef INDIVIDUAL_BASED_ANIMALS
	for(int columnIndex=0; columnIndex<MAX_COLUMN_INDEX; columnIndex++){
		bottomAnimalBiomass[columnIndex]=0.0f;
		for(int depthIndex=0; depthIndex<MAX_DEPTH_INDEX; depthIndex++){
			floatingAnimalBiomass[depthIndex][columnIndex]=0.0f;
		}
	}
#endif
	/*Migrate verically zooplankton according to current time */

#ifdef INDIVIDUAL_BASED_ANIMALS
	/* Update biomass in bottom and floating grazer cohorts*/



	updateCohortBiomassForAnimals(bottomAnimals);
	updateCohortBiomassForAnimals(floatingAnimals);
	if(tracedCohort.numberOfIndividuals!=0){
		/*Update biomass for traced cohort*/
		if(*current_hour==5){
			cout<<"Traced cohort empty."<<endl;
		}
		updateCohortBiomass(tracedCohort);
		if(tracedCohort.numberOfIndividuals==0){
			cout<<"Traced cohort empty."<<endl;
		}
	}
	/* Increase egg age*/

#ifdef CREATE_NEW_COHORTS
#ifdef MATURE_JUVENILES
	matureEggs(bottomEggs, bottomJuveniles);
	matureEggs(floatingEggs, floatingJuveniles);
	matureJuveniles(bottomJuveniles, bottomAnimals);
	matureJuveniles(floatingJuveniles, floatingAnimals);
#else
	matureEggs(bottomEggs, bottomAnimals);
	matureEggs(floatingEggs, floatingAnimals);
#endif
#endif
#ifdef PRINT_ANIMAL_SUMMATORY
	cout<<"Summatory of individuals. Original: "<<this->floating_animal_count_summing<<", Normalized: "<<this->floating_animal_count_summing/(41*254)<<"."<<endl;
	cout<<"Floating adults: "<<this->floatingAnimals->size()<<", floating juveniles: "<<this->floatingJuveniles.size()<<", floating eggs: "<<floatingEggs.size()<<"."<<endl;
	cout<<"Combined animals size: "<<this->floatingAnimals->size()+this->floatingJuveniles.size()+floatingEggs.size()<<"."<<endl;
#endif
	/* Remove all dead animals*/
//	removeDeadAnimals();

#else
	/*Calculate phytoplankton and periphyton biomass on the current step*/
	for(int columnIndex=0; columnIndex<MAX_COLUMN_INDEX; columnIndex++){
		lineBuffer.str("");
		lineBuffer.clear();
		bool registerBottomAnimalBiomass=*current_time%TIME_OUTPUT_RESOLUTION==0;
		/*Transform grazer count to biomass*/
		bottomAnimalBiomass[columnIndex]=((biomassType)bottomAnimalCount[columnIndex])*DAPHNIA_WEIGHT_IN_MICROGRAMS;
		biomassType previousBottomAnimalBiomass = bottomAnimalBiomass[columnIndex];
		/*Update biomass and output new biomass*/
		biomassType bottomAnimalBiomassDifferential = animalBiomassDifferential(maxDepthIndex[columnIndex], columnIndex, true, bottomAnimalCount[columnIndex], previousBottomAnimalBiomass);
		if(bottomAnimalBiomassDifferential>0.0f){
			biomassType mortalityBiomass=previousBottomAnimalBiomass;
			calculateGrazerCarryingCapacityMortality(mortalityBiomass);
			bottomAnimalBiomassDifferential-=animal_carrying_capacity*bottomAnimalBiomassDifferential;
		}

		bottomAnimalBiomass[columnIndex]+=bottomAnimalBiomassDifferential;
#ifdef CHECK_ASSERTIONS
		reportAssertionError(maxDepthIndex[columnIndex], columnIndex, bottomAnimalBiomass[columnIndex], previousBottomAnimalBiomass,
				bottomAnimalBiomassDifferential, true);
#endif
		/* From biomass to discrete count*/
		bottomAnimalCount[columnIndex]=ceil(bottomAnimalBiomass[columnIndex]/DAPHNIA_WEIGHT_IN_MICROGRAMS);
		bottomAnimalCount[columnIndex]=max<animalCountType>((animalCountType)0.0f, bottomAnimalCount[columnIndex]);
		/*If biomass must be registered, register standard and slough periphyton biomass*/
		if(registerBottomAnimalBiomass&&bottomAnimalBiomass[columnIndex]>0){
			animalBiomassBuffer<<lineBuffer.str()<<commaString<<bottomAnimalCount[columnIndex]<<commaString<<bottomAnimalBiomass[columnIndex]<<endl;
		}
		for(int depthIndex=0; depthIndex<MAX_DEPTH_INDEX; depthIndex++){
			lineBuffer.str("");
			lineBuffer.clear();
			/* Do not register biomass for empty cells. Since this is the bottom of the lake, do not register biomass below it*/
			if(depthIndex>maxDepthIndex[columnIndex]){
				floatingAnimalBiomass[depthIndex][columnIndex]=0.0f;
				registerZooplanktonBiomass[depthIndex][columnIndex]=false;
				break;
			} else{
				/*Set if biomass must be registered*/
				registerZooplanktonBiomass[depthIndex][columnIndex]=depthIndex%DEPTH_OUTPUT_RESOLUTION==0&&columnIndex%COLUMN_OUTPUT_RESOLUTION==0;
				/*Transform zooplankton count to biomass*/
				floatingAnimalBiomass[depthIndex][columnIndex]=((biomassType)floatingAnimalCount[depthIndex][columnIndex])*DAPHNIA_WEIGHT_IN_MICROGRAMS;
				biomassType previousFloatingAnimalBiomass = floatingAnimalBiomass[depthIndex][columnIndex];
				/*Update biomass and output new biomass*/
				biomassType floatingAnimalBiomassDifferential = animalBiomassDifferential(depthIndex, columnIndex, false, floatingAnimalCount[depthIndex][columnIndex], previousFloatingAnimalBiomass);
				if(floatingAnimalBiomassDifferential>0.0f){
					biomassType mortalityBiomass=animalBiomass;
					calculateGrazerCarryingCapacityMortality(mortalityBiomass);
					floatingAnimalBiomassDifferential-=animal_carrying_capacity*floatingAnimalBiomassDifferential;
				}

				floatingAnimalBiomass[depthIndex][columnIndex]+=floatingAnimalBiomassDifferential;
#ifdef CHECK_ASSERTIONS
				reportAssertionError(depthIndex, columnIndex, floatingAnimalBiomass[depthIndex][columnIndex], previousFloatingAnimalBiomass,
						floatingAnimalBiomassDifferential, false);
#endif
				/* From biomass to discrete count*/
				floatingAnimalCount[depthIndex][columnIndex]=ceil(floatingAnimalBiomass[depthIndex][columnIndex]/DAPHNIA_WEIGHT_IN_MICROGRAMS);
				floatingAnimalCount[depthIndex][columnIndex]=max<animalCountType>((animalCountType)0.0f, floatingAnimalCount[depthIndex][columnIndex]);
				this->floating_animal_count_summing+=floatingAnimalCount[depthIndex][columnIndex];
				/*If biomass must be registered, register standard phytoplankton biomass*/
				if(registerZooplanktonBiomass[depthIndex][columnIndex]&&floatingAnimalBiomass[depthIndex][columnIndex]>0){
					animalBiomassBuffer<<lineBuffer.str()<<commaString<<floatingAnimalCount[depthIndex][columnIndex]<<commaString<<floatingAnimalBiomass[depthIndex][columnIndex]<<endl;
				}

			}
		}
	}
#endif
}

#ifdef INDIVIDUAL_BASED_ANIMALS

/* Function for updating biomass in grazer cohorts*/
void AnimalBiomassDynamics::updateCohortBiomass(AnimalCohort& cohort){
	int depthIndex=cohort.x, columnIndex=cohort.y;

	/*Assign cohort ID if it does not exist*/
	if(cohort.cohortID==-1){
		cout<<"Error: negative cohort."<<endl;
//		cohort.cohortID=(*this->cohortID)++;
	}

	/* Check if biomass must be registered*/
	bool registerBiomass=*current_hour%TIME_OUTPUT_RESOLUTION==0;
	lineBuffer.str("");
	lineBuffer.clear();
	biomassType initialFoodBiomass= getFoodBiomass(cohort.isBottomAnimal, cohort.y, cohort.x);
	biomassType initialAnimalBiomass = cohort.bodyBiomass;
	animalCountType animalCount=cohort.numberOfIndividuals;
#if defined (INDIVIDUAL_BASED_ANIMALS)&& defined (CREATE_NEW_COHORTS)
	if(*current_hour>=20*24){
//		cout<<"Potential negative biomass."<<endl;
	}
	biomassType biomassDifferential = animalBiomassDifferential(depthIndex, columnIndex, cohort.isBottomAnimal, animalCount, initialAnimalBiomass, cohort.stage);
#else
	biomassType biomassDifferential = animalBiomassDifferential(depthIndex, columnIndex, cohort.isBottomAnimal, animalCount, initialAnimalBiomass);
#endif
	if(biomassDifferential>0.0f){
#if defined(INDIVIDUAL_BASED_ANIMALS)&&defined(CREATE_NEW_COHORTS)
		biomassType mortalityBiomass=min<biomassType>(animalCount*this->initial_grazer_weight[cohort.stage], initialAnimalBiomass);
#else
		biomassType mortalityBiomass=previousBottomAnimalBiomass;
#endif
		calculateGrazerCarryingCapacityMortality(mortalityBiomass);
		biomassDifferential-=animal_carrying_capacity*biomassDifferential;
	}
#ifdef CREATE_NEW_COHORTS
	/* If biomass differential is negative, do not invest in eggs*/
	if(cohort.stage==AnimalStage::Egg){
		cout<<"Error. Egg biomass uodated."<<endl;
	}

	//if(cohort.stage!=AnimalStage::Juvenile&&biomassDifferential>0.0f){
		if(biomassDifferential>0.0f){

		biomassType biomassDifferential=cohort.isBottomAnimal?this->bottomFoodBiomassDifferential[cohort.y]:this->floatingFoodBiomassDifferential[cohort.x][cohort.y];
		calculateReproductionProportionInvestment(initialFoodBiomass, biomassDifferential);
	} else{
		reproduction_proportion_investment=0.0f;
	}
	if(cohort.x>maxDepthIndex[cohort.y]){
		cout<<"Lower overflow."<<endl;
	}
#else
	reproduction_proportion_investment=0.0f;
#endif
	/* Amount of biomass invested in body and gonad weight*/
//	reproduction_proportion_investment=0.0f;
	biomassType bodyBiomassInvestment=biomassDifferential*(1-reproduction_proportion_investment);
 biomassType bodyLostBiomass=consumed_biomass*(1-reproduction_proportion_investment);
	biomassType gonadBiomassInvestment=biomassDifferential*reproduction_proportion_investment;
	cohort.gonadBiomass+=gonadBiomassInvestment;
#ifdef ANIMAL_STARVATION_HOURS_WITHOUT_FOOD
	animalStarvationMortality(cohort, getFoodBiomass(cohort));
#elif defined(ANIMAL_STARVATION_PROPORTION_LOST_BIOMASS)
	animalStarvationMortality(cohort);
#endif
#ifdef CREATE_NEW_COHORTS
	/* If there exists gonad investment, create new eggs*/
	if(((*(this->current_hour)%this->ovipositing_period)==0)||(cohort.ageInHours==this->maximum_age_in_hours)){
		if(cohort.gonadBiomass>0.0f){
			/* If the biomass is enough to generate at least one egg, create it.*/
			if(cohort.gonadBiomass>=(biomassType)initial_grazer_weight[AnimalStage::Egg]){
	#ifdef REPORT_COHORT_INFO
				cout<<"Hour: "<<(*(this->current_hour))<<". Creating new cohort with ID: "<<(cohort.cohortID)<<", biomass: "<<cohort.gonadBiomass<<", x: "<<cohort.x<<", y: "<<cohort.y<<"."<<endl;
	#endif
				biomassType newCohortBiomass = createNewCohort(cohort, cohort.gonadBiomass, cohort.isBottomAnimal?&(this->bottomJuveniles):&(this->floatingJuveniles));
	#ifdef REPORT_COHORT_INFO
				cout<<"Cohort created."<<endl;
	#endif
				/* Store biomass remnant as body investment in the parent cohort*/
				cohort.gonadBiomass-=newCohortBiomass;
			}
		}
	}
#endif
#ifdef ANIMAL_AGING
	animalAging(cohort);
#endif
	cohort.bodyBiomass+=bodyBiomassInvestment;
//	cohort.numberOfIndividuals=cohort.bodyBiomass/(biomassType)initial_grazer_weight[cohort.stage];
	/*Remove dead animals in the cohort*/
	if(animal_mortality>0){
		biomassType biomassAfterEating = getFoodBiomass(cohort);
		if(biomassAfterEating==0.0f){
			/* If there is no food, all individuals die*/
			natural_dead_individuals=cohort.numberOfIndividuals;
//			cout<<"Zero individuals."<<endl;
//			deadIndividuals=animal_mortality/this->initial_grazer_weight[cohort.stage];
		} else{
			/* Otherwise, only a number die*/
			biomassType starvationProportion = (cohort.numberOfIndividuals/biomassAfterEating);
			starvationProportion*=this->starvation_factor;
			natural_dead_individuals=animal_mortality/this->initial_grazer_weight[cohort.stage]*starvationProportion;
//			if(cohort.numberOfIndividuals<=natural_dead_individuals){
//				cout<<"Zero individuals."<<endl;
//			}
		}
//		deadIndividuals=animal_mortality/this->initial_grazer_weight[cohort.stage];
		cohort.numberOfIndividuals=max<animalCountType>(0,cohort.numberOfIndividuals-natural_dead_individuals);
		cohort.bodyBiomass=max<animalCountType>(0.0f,cohort.bodyBiomass-natural_dead_individuals*this->initial_grazer_weight[cohort.stage]);
//		if(natural_dead_individuals>0){
//			cout<<"Naturally dead individuals greater than 0."<<endl;
//		}
		if(cohort.numberOfIndividuals<0){
			cout<<"Error. Negative dying individuals."<<endl;
		}
	}

	/*Remove starved animals in the cohort*/
	/*if(consumed_biomass>0){
		biomassType biomassDeficit=consumed_biomass;
		animalCountType starvedIndividuals=this->starvation_factor*biomassDeficit/this->initial_grazer_weight[cohort.stage];
   cout<<"Starved individuals: "<<starvedIndividuals<<", starvation factor: "<<this->starvation_factor<<endl;
		cohort.numberOfIndividuals=max<animalCountType>(0,cohort.numberOfIndividuals-starvedIndividuals);
	}*/
#ifdef CHECK_ASSERTIONS
	reportAssertionError(maxDepthIndex[columnIndex], columnIndex, cohort.bodyBiomass+cohort.gonadBiomass, initialAnimalBiomass,
			biomassDifferential, cohort.isBottomAnimal);
#endif
	/* Update number of individuals based on cohort biomass*/
#ifndef CREATE_NEW_COHORTS
	cohort.numberOfIndividuals=ceil(cohort.bodyBiomass/initial_grazer_weight[cohort.stage]);
	cohort.numberOfIndividuals=max<animalCountType>(0, cohort.numberOfIndividuals);
	if(cohort->numberOfIndividuals==0){
				cout<<"Zero individuals."<<endl;
			}
#endif
  if(bodyLostBiomass>0.0f){

    //cohort.numberOfIndividuals=ceil(cohort.bodyBiomass/initial_grazer_weight[cohort.stage]);
	  // Include removal of starved grazers multiplied by a weight
//	cohort.numberOfIndividuals-=ceil(this->dead_animal_proportion*bodyLostBiomass/initial_grazer_weight[cohort.stage]);
//	cohort.numberOfIndividuals=max<animalCountType>(0,cohort.numberOfIndividuals);
//	if(cohort.numberOfIndividuals<0){
//		cout<<"Error. Negative lost individuals."<<endl;
//	}
  }
    if(!cohort.isBottomAnimal){
    	this->floating_animal_count_summing+=cohort.numberOfIndividuals;
    }
	/* If the number of individuals or total biomass in the cohort is 0, consider it dead of starvation */
//	if((cohort.bodyBiomass<=0||cohort.numberOfIndividuals<=0)&&cohort.death==None){
//		cohort.death=causeOfDeath::Starvation;
//	}
	/*If biomass must be registered, register standard phytoplankton biomass*/
	if(registerBiomass&&cohort.numberOfIndividuals>0){
//		if(cohort.cohortID==this->tracedCohortID){
//			cout<<"Traced individual."<<endl;
//		}
//		if(natural_dead_individuals>0){
//			cout<<"Natural dead individuals greater than 0."<<endl;
//		}
		biomassType fitnessDifference = cohort.currentFitnessValue-cohort.previousFitnessValue;
		animalBiomassBuffer<<lineBuffer.str()<<commaString<<cohort.numberOfIndividuals<<commaString<<animals_dead_by_starvation<<commaString<<natural_dead_individuals<<commaString<<cohort.bodyBiomass<<commaString<<cohort.gonadBiomass<<commaString<<locale_algae_biomass_before_eating<<commaString<<locale_algae_biomass_after_eating<<commaString<<used_consumption<<commaString<<animal_carrying_capacity<<commaString<<reproduction_proportion_investment<<commaString<<this->stroganov_adjustment<<commaString<<indexToDepth[cohort.x]<<commaString<<lakeLightAtDepth[cohort.x][cohort.y]<<commaString<<cohort.previousFitnessValue<<commaString<<cohort.currentFitnessValue<<commaString<<fitnessDifference<<commaString<<cohort.hoursInStarvation<<commaString<<cohort.stage<<commaString<<cohort.cohortID<<endl;
//#ifdef LIGHT_BASED_MIGRATION_FIXED_FREQUENCY
//		if(lakeLightAtDepth[cohort.x][cohort.y]<this->critical_light_intensity){
					//cout<<"Juvenile lake light at coordinates: "<<cohort.x<<", "<<cohort.y<<" is "<<critical_light_intensity<<"."<<endl;
//				}
//#endif
	}

	/* If the cohort is dead, register its death*/
//	if(cohort.death!=None){
//		unsigned int isBottomAnimal=cohort.isBottomAnimal?1:0;
//		this->animalDeadBuffer<<cohort.x<<commaString<<cohort.y<<commaString<<(*current_hour)<<commaString<<isBottomAnimal<<commaString<<cohort.ageInHours<<commaString<<cohort.hoursWithoutFood<<commaString<<cohort.stage<<commaString<<cohort.numberOfIndividuals<<commaString<<cohort.bodyBiomass<<endl;
//	}
#ifndef ANIMAL_COHORT_MAP
#ifndef CONGLOMERATE_ALL_COHORTS
	if(cohort.numberOfIndividuals>0&&cohort.numberOfIndividuals<=this->agglomeration_cohort_threshold&&cohort.stage==AnimalStage::Mature){
//		If the number of individuals is lower than 3 and the cohort is mature, integrate into an existing cohort
		pair<int,int> cohortCoordinates=pair<int,int>(cohort.x, cohort.y);
		if(cohort.isBottomAnimal){
			if(bottomReallocatedCohorts.find(cohortCoordinates)==bottomReallocatedCohorts.end()){
				/*If the bottom cohort does not exist, create it*/
				bottomReallocatedCohorts[cohortCoordinates]=cohort;
			} else{
				/*Otherwise, add to the existing cohort*/
				bottomReallocatedCohorts[cohortCoordinates]+=cohort;
			}
		} else{
			if(floatingReallocatedCohorts.find(cohortCoordinates)==floatingReallocatedCohorts.end()){
				floatingReallocatedCohorts[cohortCoordinates]=cohort;
			} else{
				floatingReallocatedCohorts[cohortCoordinates]+=cohort;
			}
		}
		/* Clear cohort for removal */
		cohort.bodyBiomass=cohort.gonadBiomass=0.0f;
		cohort.numberOfIndividuals=0;
	}
#endif

#endif
}
#endif
//void AnimalBiomassDynamics::verticalMigrateAnimalsNoPreference(){
//
//
//
//
//
//	/* Migrate biomass with respect to the previous hour*/
//
//	/* First, calculate depth movement with respect to previous hour*/
//	int depth_dependent_hour_shift =(*current_hour)==0?INITIAL_ZOOPLANKTON_SHIFT:floatingAnimalBiomassCenterDifferencePerDepth[(*current_hour)%HOURS_PER_DAY];
//
//	/*If there exists a movement in zooplankton across depth*/
//	if(depth_dependent_hour_shift!=0){
//		/* Temporary store zooplankton count in a buffer*/
//		for(int depthIndex=0; depthIndex<MAX_DEPTH_INDEX; depthIndex++){
//			for(int columnIndex=0; columnIndex<MAX_COLUMN_INDEX; columnIndex++){
//				verticalMigrationAnimalBiomassBuffer[depthIndex][columnIndex] =floatingAnimalCount[depthIndex][columnIndex];
//				floatingAnimalCount[depthIndex][columnIndex] = 0.0f;
//			}
//		}
//		int initial_depth_index=depth_dependent_hour_shift>0?0:-depth_dependent_hour_shift,
//				final_depth_index=depth_dependent_hour_shift>0?MAX_DEPTH_INDEX-depth_dependent_hour_shift:MAX_DEPTH_INDEX;
//
//		for(int depthIndex=initial_depth_index; depthIndex<final_depth_index; depthIndex++){
//
//			/*Adjust depth movement per depth. If it is within depth range, update zooplankton count*/
//			int depth_adjustment=depthIndex+depth_dependent_hour_shift;
//			for(int columnIndex=0; columnIndex<MAX_COLUMN_INDEX; columnIndex++){
//				/* Move zooplankton individuals to the adjusted depth*/
//				if(depth_adjustment<=maxDepthIndex[columnIndex]&&depthIndex<=maxDepthIndex[columnIndex]){
//					floatingAnimalCount[depth_adjustment][columnIndex] =verticalMigrationAnimalBiomassBuffer[depthIndex][columnIndex];
//				}
//			}
//		}
//
//		/* Accumulate unmoved biomass to extreme shallow or deep index*/
//#ifdef ZOOPLANKTON_ACCUMULATION
//		initial_depth_index=depth_dependent_hour_shift>0?MAX_DEPTH_INDEX-depth_dependent_hour_shift:0,
//			final_depth_index=depth_dependent_hour_shift>0?MAX_DEPTH_INDEX:-depth_dependent_hour_shift;
//
//		for(int depthIndex=initial_depth_index; depthIndex<final_depth_index; depthIndex++){
//			for(int columnIndex=0; columnIndex<MAX_COLUMN_INDEX; columnIndex++){
//				unsigned int receivingIndex=depth_dependent_hour_shift>0?maxDepthIndex[columnIndex]:0;
//				floatingAnimalCount[receivingIndex][columnIndex]+=verticalMigrationZooplanktonBiomassBuffer[depthIndex][columnIndex];
//
//			}
//		}
//#endif
//	}
////	for(int depthIndex=0; depthIndex<MAX_DEPTH_INDEX; depthIndex++){
////
////		/*Adjust depth movement per depth. If it is within depth range, update zooplankton count*/
////		int depth_adjustment=depthIndex+depth_dependent_hour_shift;
////
////		for(int columnIndex=0; columnIndex<MAX_COLUMN_INDEX; columnIndex++){
////			if(depth_adjustment<0||depth_adjustment>=MAX_DEPTH_INDEX||depth_adjustment>maxDepthIndex[columnIndex]){
////				/* Move zooplankton individuals to the adjusted depth*/
////				unsigned int receivingIndex=depth_dependent_hour_shift>0?maxDepthIndex[columnIndex]:0;
////#ifdef ZOOPLANKTON_ACCUMULATION
////				bool zooplanktionAccumulationCondition = depthIndex>maxDepthIndex[columnIndex]||;
////#else
////				bool zooplanktionAccumulationCondition = depthIndex<=maxDepthIndex[columnIndex];
////#endif
////				if(zooplanktionAccumulationCondition){
////					zooplanktonCount[receivingIndex][columnIndex]+=verticalMigrationZooplanktonBiomassBuffer[depthIndex][columnIndex];
////				}
////			}
////		}
////	}
//}
//
//
//
//
///* Distribute preference score and daphnia across neighboring cells. Initially, it will be distributed in the same column.*/
//void AnimalBiomassDynamics::verticalMigrateAnimalsPreference(){
//	calculateLocalPreferenceScore();
//	biomassType *neighboringPreferences = new biomassType[this->vertical_migration_buffer_size];
//	for (int depthIndex = 0; depthIndex < MAX_DEPTH_INDEX; depthIndex++) {
//		int candidateLowerRowIndex = depthIndex-this->maximum_distance_daphnia_swum_in_rows_per_hour;
//		int candidateUpperRowIndex = depthIndex+this->maximum_distance_daphnia_swum_in_rows_per_hour;
//		int localeLowerRowIndex=candidateLowerRowIndex<0?0:candidateLowerRowIndex;
//		for (int columnIndex = 0; columnIndex < MAX_COLUMN_INDEX;columnIndex++) {
//			/* Normalize daphnia preference in neighboring cells from the same column. First, sum preference at neighboring cells*/
//			biomassType neighboringPreferencesSum=0.0f;
//			int localeUpperRowIndex=candidateUpperRowIndex>maxDepthIndex[columnIndex]?maxDepthIndex[columnIndex]:candidateUpperRowIndex;
//			int usedMigrationBufferSize=localeUpperRowIndex-localeLowerRowIndex;
//			for (int localeDepthIndex = localeLowerRowIndex; localeDepthIndex < localeUpperRowIndex; localeDepthIndex++) {
//				neighboringPreferencesSum+=this->foodPreferenceScore[localeDepthIndex][columnIndex];
//				neighboringPreferences[localeDepthIndex-localeLowerRowIndex] = foodPreferenceScore[localeDepthIndex][columnIndex];
//			}
//			/*Find shift towards 0-based*/
//			biomassType minimumLocalePreference=neighboringPreferences[0];
//			for (int localeDepthIndex = 0; localeDepthIndex < usedMigrationBufferSize; localeDepthIndex++) {
//				minimumLocalePreference=min(neighboringPreferences[localeDepthIndex],minimumLocalePreference);
//			}
//			for (int localeDepthIndex = 0; localeDepthIndex < usedMigrationBufferSize; localeDepthIndex++) {
//				neighboringPreferences[localeDepthIndex]-=minimumLocalePreference;
//			}
//			/* Then, normalize each cell by this summing*/
//			for (int localeDepthIndex = 0; localeDepthIndex < usedMigrationBufferSize; localeDepthIndex++) {
//				neighboringPreferences[localeDepthIndex]/=neighboringPreferencesSum;
//			}
//			/* Finally, distribute grazers using this distribution*/
//			biomassType bufferedZooplanktonCount=floatingAnimalCount[depthIndex][columnIndex];
//			floatingAnimalCount[depthIndex][columnIndex]=0.0f;
//			for (int localeDepthIndex = localeLowerRowIndex; localeDepthIndex < localeUpperRowIndex; localeDepthIndex++) {
//				this->floatingAnimalCount[localeDepthIndex][columnIndex]+=bufferedZooplanktonCount*neighboringPreferences[localeDepthIndex-localeLowerRowIndex];
//			}
//		}
//	}
//}

/* Calculate neighboring preference score for daphnia migration. Inspired by ([1] B.-P. Han and M. Stra�kraba, �Modeling patterns of zooplankton diel vertical migration,� J. Plankton Res., vol. 20, no. 8, pp. 1463�1487, 1998., Eqn. 15)*/
void AnimalBiomassDynamics::calculateLocalPreferenceScore() {
	for (int depthIndex = 0; depthIndex < MAX_DEPTH_INDEX; depthIndex++) {
		for (int columnIndex = 0; columnIndex < MAX_COLUMN_INDEX;
				columnIndex++) {
			/*Daphnia prefer descents in light (to avoid predators) and increases in algae*/
			this->foodPreferenceScore[depthIndex][columnIndex] =
					((previousLakeLightAtDepth[depthIndex][columnIndex] - lakeLightAtDepth[depthIndex][columnIndex])
							/ lakeLightAtDepth[depthIndex][columnIndex])
							* (floatingFoodBiomassDifferential[depthIndex][columnIndex]/floatingFoodBiomass[depthIndex][columnIndex]);
		}
	}
}

/* Get food biomass for the modeled species */
biomassType AnimalBiomassDynamics::getFoodBiomass(bool bottom, int columnIndex,
		int depthIndex) {
	return bottom ?
			this->bottomFoodBiomass[columnIndex] :
			this->floatingFoodBiomass[depthIndex][columnIndex];
}
#if defined(INDIVIDUAL_BASED_ANIMALS)&&defined(CREATE_NEW_COHORTS)
biomassType AnimalBiomassDynamics::animalBiomassDifferential(int depthIndex, int columnIndex, bool bottom, animalCountType animalCount, biomassType animalBiomass, AnimalStage stage){
#else
biomassType AnimalBiomassDynamics::animalBiomassDifferential(int depthIndex, int columnIndex, bool bottom, animalCountType animalCount, biomassType animalBiomass){
#endif
	physicalType localeTemperature = temperature[depthIndex][columnIndex];

		/* Get zooplankton count and biomass*/
	locale_algae_biomass_before_eating = getFoodBiomass(bottom, columnIndex, depthIndex);
	biomassType localeFoodBiomassInMicrograms = locale_algae_biomass_before_eating*this->food_conversion_factor;
	stroganovApproximation(localeTemperature);
#ifdef INDIVIDUAL_BASED_ANIMALS
	biomassType usedWeight= this->initial_grazer_weight[stage];
#else
	biomassType usedWeight= 15;
#endif
	foodConsumptionRate(depthIndex,columnIndex,bottom, animalCount, localeFoodBiomassInMicrograms, usedWeight, 1-migration_consumption);
	locale_algae_biomass_after_eating =  getFoodBiomass(bottom,	columnIndex, depthIndex);
	defecation();
	animalRespiration(animalBiomass, localeTemperature, salinity_effect_matrix[depthIndex][columnIndex]);
	animalExcretion(salinity_corrected_animal_respiration);
	/* Calculate mortality based on animal biomass or animal count*/
#if defined(INDIVIDUAL_BASED_ANIMALS)&&defined(CREATE_NEW_COHORTS)

	biomassType mortalityBiomass=min<biomassType>(animalCount*this->initial_grazer_weight[stage], animalBiomass);
#else
	biomassType mortalityBiomass=animalBiomass;
#endif
	animalMortality(mortalityBiomass, localeTemperature, salinity_effect_matrix[depthIndex][columnIndex]);
#ifdef STARVATION_MORTALITY
//	animal_mortality/=min<biomassType>(1.0f,locale_algae_biomass_before_eating/142.0f);
//	animal_mortality=min<biomassType>(animal_mortality, animalBiomass);
#endif
#ifdef ADD_DEAD_BIOMASS_NUTRIENTS
	biomassType reabsorbedAlgalNutrients=reabsorbed_animal_nutrients_proportion*animal_mortality;
#ifdef CHECK_LOST_BIOMASS_ADDITION
	if(deadFloatingBiomass[depthIndex][columnIndex]<0.0f){
		cout<<"Lower than 0 dead biomass: "<<deadFloatingBiomass[depthIndex][columnIndex]<<"."<<endl;
	}
#endif
	/* Add dead animal biomass*/
	if(bottom){
		deadBottomBiomass[columnIndex]+=reabsorbedAlgalNutrients;
	} else{
		deadFloatingBiomass[depthIndex][columnIndex]+=reabsorbedAlgalNutrients;
	}
#ifdef CHECK_LOST_BIOMASS_ADDITION
	if(deadFloatingBiomass[depthIndex][columnIndex]<0.0f){
		cout<<"Lower than 0 dead biomass: "<<deadFloatingBiomass[depthIndex][columnIndex]<<"."<<endl;
	}
#endif
#endif

	consumed_biomass=locale_defecation+salinity_corrected_animal_respiration+animal_excretion_loss;
	consumed_biomass=0.0f;
	biomassType localeBiomassDifferential=used_consumption-consumed_biomass-animal_mortality-animal_predatory_pressure;
	calculatePredationPressure(animalCount);
		if(*current_hour>=20*24&&bottom){
//			cout<<"Condition found."<<endl;
		}
	/* Plot grazer biomass differential*/
	lineBuffer.str("");
	lineBuffer.clear();
	lineBuffer<<depthIndex;
	lineBuffer<<commaString<<columnIndex;
	lineBuffer<<commaString<<(*current_hour);
	lineBuffer<<commaString<<bottom?1:0;
#ifdef EXTENDED_OUTPUT
	lineBuffer<<commaString<<consumption_per_individual;
	lineBuffer<<commaString<<consumption_per_individual/localeFoodBiomassBeforeEating;
	lineBuffer<<commaString<<used_consumption/localeFoodBiomassBeforeEating;
	lineBuffer<<commaString<<stroganov_adjustment;
	lineBuffer<<commaString<<locale_consumption;
	lineBuffer<<commaString<<locale_consumption_salt_adjusted;
	lineBuffer<<commaString<<locale_defecation;
	lineBuffer<<commaString<<basal_animal_respiration;
	lineBuffer<<commaString<<active_respiration_exponent;
	lineBuffer<<commaString<<active_respiration_factor;
	lineBuffer<<commaString<<active_respiration;
	lineBuffer<<commaString<<metabolic_respiration;
	lineBuffer<<commaString<<base_animal_respiration;
	lineBuffer<<commaString<<salinity_corrected_animal_respiration;
	lineBuffer<<commaString<<animal_excretion_loss;
	lineBuffer<<commaString<<animal_temperature_mortality;
	lineBuffer<<commaString<<animal_temp_independent_mortality;
	lineBuffer<<commaString<<animal_base_mortality;
	lineBuffer<<commaString<<salinity_mortality;
	lineBuffer<<commaString<<low_oxigen_animal_mortality;
	lineBuffer<<commaString<<animal_mortality;
	lineBuffer<<commaString<<animal_predatory_pressure;
	lineBuffer<<commaString<<animal_carrying_capacity;
	lineBuffer<<commaString<<locale_algae_biomass_before_eating;
	lineBuffer<<commaString<<locale_algae_biomass_after_eating;
	lineBuffer<<commaString<<reproduction_investment_subtraction;
	lineBuffer<<commaString<<reproduction_investment_exponent;
	lineBuffer<<commaString<<reproduction_investment_power;
	lineBuffer<<commaString<<reproduction_proportion_investment;
#endif
	lineBuffer<<commaString<<localeBiomassDifferential;
	return localeBiomassDifferential;
}



/* Food consumption (AquaTox Documentation, page 105, equation 98)*/
void AnimalBiomassDynamics::foodConsumptionRate(int depthIndex, int columnIndex, bool bottom, animalCountType animalCount, biomassType foodBiomassInMicrograms, biomassType individualWeight, double consumedProportion){
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

/* Food consumption (AquaTox Documentation, page 105, equation 97)*/
void AnimalBiomassDynamics::defecation(){
	locale_defecation = DEFECATION_COEFFICIENT*used_consumption;
}

/* Zooplankton respiration (AquaTox Documentation, page 106, equation 100)*/
void AnimalBiomassDynamics::animalRespiration(biomassType zooBiomass, physicalType localeTemperature, physicalType localeSalinityEffect){
	base_animal_respiration = RESPIRATION_ADJUSTMENT*(basalRespiration(zooBiomass) + metabolicFoodConsumption());
	salinity_corrected_animal_respiration = base_animal_respiration*localeSalinityEffect;
}


/* Basal respiration (AquaTox Documentation, page 106, equation 101)*/
biomassType AnimalBiomassDynamics::basalRespiration(biomassType zooBiomass){
	basal_animal_respiration =zooBiomass*BASAL_RESPIRATION_RATE*this->basal_respiration_weight*stroganov_adjustment;
	return basal_animal_respiration;

}

/* An approximation of Stroganov function between 0 and 15 degrees (AquaTox Documentation, page 84, figure 59)*/
void AnimalBiomassDynamics::stroganovApproximation(physicalType localeTemperature){
	stroganov_adjustment= (0.1f+0.011f*localeTemperature)*STROGANOV_ADJUSTMENT;
}

/* Active respiration (AquaTox Documentation, page 107, equation 104)*/
biomassType AnimalBiomassDynamics::activeRespiration(biomassType zooBiomass, physicalType localeTemperature){
	active_respiration_exponent = RATE_RESPIRATION_CHANGE_10DEG*localeTemperature;
	active_respiration_factor=exp(active_respiration_exponent);
	active_respiration = zooBiomass*active_respiration_factor;
	return active_respiration;
}


/* Specific dynamic action respiration (AquaTox Documentation, page 109, equation 110)*/
biomassType AnimalBiomassDynamics::metabolicFoodConsumption(){
	metabolic_respiration= this->k_value_respiration*(used_consumption-locale_defecation);
	return metabolic_respiration;
}

/* Grazer excretion biomass loss (AquaTox Documentation, page 109, equation 111)*/
void AnimalBiomassDynamics::animalExcretion(biomassType localeRespiration){
	animal_excretion_loss= EXCRETION_RESPIRATION_PROPORTION*localeRespiration;
}


/* Grazer mortality (AquaTox Documentation, page 110, equation 112)*/
void AnimalBiomassDynamics::animalMortality(biomassType localeBiomass, physicalType localeTemperature, physicalType localeSalinityConcentration){
	animalBaseMortality(localeTemperature, localeBiomass);
	salinityMortality(localeBiomass);
	calculateLowOxigenMortality(localeBiomass);
	animal_mortality = animal_base_mortality+salinity_mortality+low_oxigen_animal_mortality;


}

/* Grazer base mortality (AquaTox Documentation, page 110, equation 113)*/
void AnimalBiomassDynamics::animalBaseMortality(physicalType
		localeTemperature, biomassType localeBiomass){
	animalTemperatureMortality(localeTemperature, localeBiomass);
	//
#ifdef GRAZER_CARRYING_CAPACITY_MORTALITY
	calculateGrazerCarryingCapacityMortality(localeBiomass);
	animal_temp_independent_mortality = this->animal_carrying_capacity*localeBiomass;
#else
	animal_temp_independent_mortality = this->animal_base_mortality_proportion*localeBiomass;
#endif
	animal_base_mortality= animal_temperature_mortality+animal_temp_independent_mortality;
}

/* Grazer base mortality (AquaTox Documentation, page 110, equation 114)*/
void AnimalBiomassDynamics::animalTemperatureMortality(physicalType localeTemperature, biomassType localeBiomass){
	if(localeTemperature<=MAXIMUM_TOLERABLE_TEMPERATURE){
		animal_temperature_mortality = 0.0f;
	} else{
		animal_temperature_mortality = localeBiomass*exp(MAXIMUM_TOLERABLE_TEMPERATURE-localeTemperature)/2.0f;
	}

}

void AnimalBiomassDynamics::calculateGrazerCarryingCapacityMortality(biomassType inputBiomass){
//	biomassType animalBiomassProduct = (this->maximum_found_animal_biomass*this->animal_carrying_capacity_coefficient);
//	biomassType carryingCapacityExponent = -inputBiomass/animalBiomassProduct+this->animal_carrying_capacity_intercept;
	biomassType animalBiomassProduct = inputBiomass*this->animal_carrying_capacity_coefficient;
	biomassType carryingCapacityExponent = -animalBiomassProduct+this->animal_carrying_capacity_intercept;
	biomassType carryingCapacityExponentiation = (1/(1+exp(carryingCapacityExponent)));
	animal_carrying_capacity =  max<biomassType>(0.0f,min<biomassType>(1.0f,this->animal_carrying_capacity_amplitude*(carryingCapacityExponentiation+this->animal_carrying_capacity_constant)));
//	animal_carrying_capacity=0.0f;
}



/* Salinity mortality (AquaTox Documentation, page 110, equation 112)*/
void AnimalBiomassDynamics::salinityMortality(biomassType localeBiomass){
		/* Salinity mortality can be present or not*/
#ifdef ADJUST_SALINITY_GRAZERS
	salinity_mortality=localeBiomass*(1-salinity_effect_matrix);
#else
	salinity_mortality = 0.0f;
#endif
}

/* The levels of oxygen concentration found in the lake do not reflect significant D. pulex mortality according to: (L. J. Weider and W. Lampert, �Differential response of Daphnia genotypes to oxygen stress: respiration rates, hemoglobin content and low-oxygen tolerance,� Oecologia, vol. 65, no. 4, pp. 487�491, Mar. 1985.)*/
void AnimalBiomassDynamics::calculateLowOxigenMortality(biomassType inputBiomass){
	low_oxigen_animal_mortality=0.0f;
}

/* Include predation pressure to control grazer values*/
void AnimalBiomassDynamics::calculatePredationPressure(animalCountType zooplanktonLocaleCount){
#ifdef ADD_GRAZER_PREDATORY_PRESSURE
	/* Use a sigmoid function to model predatory pressure*/
	animal_predatory_pressure = 1/(1+exp(-(biomassType)zooplanktonLocaleCount+INITIAL_PREDATORY_PRESSURE));
#else
	animal_predatory_pressure = 0.0f;
#endif
}

#ifdef INDIVIDUAL_BASED_ANIMALS

#ifdef ANIMAL_STARVATION_HOURS_WITHOUT_FOOD
/* Starvation mortality ([1] Z. M. Gliwicz and C. Guisande, “Family planning inDaphnia: resistance to starvation in offspring born to mothers grown at different food levels,” Oecologia, vol. 91, no. 4, pp. 463–467, Oct. 1992., page 465, Fig. 1)*/
void AnimalBiomassDynamics::animalStarvationMortality(AnimalCohort& cohort, biomassType foodBiomass){
	if(foodBiomass<this->food_starvation_threshold){
			/* If starving, increment the number of hours without food*/
			cohort.hoursInStarvation++;
		if(cohort.hoursInStarvation>=this->max_hours_without_food){
			/* If starving above the maximum, kill the cohort*/
			cohort.numberOfIndividuals=0;
			cohort.death=Starvation;
		}
	} else{
		/* IF there is enough food, reset the number of hours without food*/
		cohort.hoursInStarvation=0;
	}
	if(cohort.hoursInStarvation>this->starvation_max_hours){
		cout<<"Starvation."<<endl;
	}
}
#elif defined(ANIMAL_STARVATION_PROPORTION_LOST_BIOMASS)

/* Starvation mortality ([1] Z. M. Gliwicz and C. Guisande, “Family planning inDaphnia: resistance to starvation in offspring born to mothers grown at different food levels,” Oecologia, vol. 91, no. 4, pp. 463–467, Oct. 1992., page 465, Fig. 1)*/
void AnimalBiomassDynamics::animalStarvationMortality(AnimalCohort& cohort){
#ifdef ACCUMULATIVE_HOUR_STARVATION
	biomassType biomassPerIndividual = cohort.bodyBiomass/cohort.numberOfIndividuals;
	cohort.starvationBiomass +=consumed_biomass/biomassPerIndividual*this->dead_animals_per_lost_biomass_and_concentration;
	animals_dead_by_starvation= cohort.starvationBiomass/this->initial_grazer_weight[cohort.stage];

	if(animals_dead_by_starvation>0){
		cohort.hoursInStarvation++;
		cohort.starvationBiomass=0.0f;
		cohort.numberOfIndividuals-=animals_dead_by_starvation;
		cohort.numberOfIndividuals=max<animalCountType>(cohort.numberOfIndividuals, 0);
//			if(animalsDeadByStarvation>0){
//				cout<<"The actual number of animals dead by starvation is "<<animalsDeadByStarvation<<" for "<<cohort<<endl;
//			}

	} else{
		cohort.hoursInStarvation=0;
	}
//	if(cohort.hoursInStarvation>this->starvation_max_hours){
//		cout<<"Starvation."<<endl;
//	}
#else
	animalCountType maxAnimalsDeadByStarvation = consumed_biomass/this->initial_grazer_weight[cohort.stage];
	biomassType biomassPerIndividual = cohort.bodyBiomass/cohort.numberOfIndividuals;
	animals_dead_by_starvation=0;
	if(maxAnimalsDeadByStarvation>0){
		animals_dead_by_starvation = (maxAnimalsDeadByStarvation/biomassPerIndividual)*this->dead_animals_per_lost_biomass_and_concentration;
		cohort.numberOfIndividuals-=animals_dead_by_starvation;
//		if(actualAnimalsDeadByStarvation>0){
//			cout<<"The actual number of animals dead by starvation is "<<actualAnimalsDeadByStarvation<<" for "<<cohort<<endl;
//		}
	}
#endif
}
#endif

/* Get food biomass depending of the animal cohort*/
biomassType  AnimalBiomassDynamics::getFoodBiomass(AnimalCohort& cohort){
	if(cohort.isBottomAnimal)
		return bottomFoodBiomass[cohort.y];
	else
		return floatingFoodBiomass[cohort.x][cohort.y];

}

#ifdef ANIMAL_AGING

/* Animal aging*/
void AnimalBiomassDynamics::animalAging(AnimalCohort& cohort){
	if(cohort.ageInHours++>this->maximum_age_in_hours){
		cohort.bodyBiomass=cohort.gonadBiomass=0.0f;
		cohort.numberOfIndividuals=0;
#ifdef REPORT_COHORT_INFO
		cout<<"Hour: "<<(*this->current_hour)<<". Senescence reached for cohort: "<<cohort<<"."<<endl;
#endif
	}
}

#endif



/* Remove dead animals*/


/* Iterate over animal collections and remove those that are dead*/
//void AnimalBiomassDynamics::removeDeadCohorts(set<AnimalCohort> *animalCohorts){
//	animalCohorts->erase(std::remove_if(animalCohorts->begin(), animalCohorts->end(), removeDeadCohort()), animalCohorts->end());
//}
//
//void AnimalBiomassDynamics::removeDeadAnimals(){
//	removeDeadCohorts(bottomAnimals);
//	removeDeadCohorts(floatingAnimals);
//
//}

#ifdef CREATE_NEW_COHORTS

void AnimalBiomassDynamics::calculateReproductionProportionInvestment(biomassType foodBiomass, biomassType foodBiomassDifferential){
	/* Proportion of investment to eggs taken from ([1] M. Lynch, “The Life History Consequences of Resource Depression in Daphnia Pulex,” Ecology, vol. 70, no. 1, pp. 246–256, Feb. 1989.)*/
#ifdef EXPONENTIAL_GONAD_ALLOCATION
	reproduction_investment_subtraction=foodBiomass*MILLILITER_TO_LITER - this->reproduction_proportion_investment_intercept;
	reproduction_investment_exponent=-this->reproduction_proportion_investment_coefficient*(reproduction_investment_subtraction);
	reproduction_investment_power=exp((biomassType)reproduction_investment_exponent);
	reproduction_proportion_investment = this->reproduction_proportion_investment_amplitude*(1 - reproduction_investment_power);
#elif defined(SIGMOIDAL_GONAD_ALLOCATION)
	reproduction_investment_subtraction=-reproduction_proportion_investment_coefficient*(foodBiomass*MILLILITER_TO_LITER);
	reproduction_investment_exponent=reproduction_investment_subtraction+this->reproduction_proportion_investment_amplitude;
	reproduction_investment_power=exp((biomassType)reproduction_investment_exponent);
	reproduction_proportion_investment=max<biomassType>(0.0f,min<biomassType>(1.0f,this->reproduction_proportion_investment_multiplier*this->reproduction_proportion_investment_intercept/(this->reproduction_proportion_investment_intercept+reproduction_investment_power))+this->reproduction_proportion_investment_constant);
#elif defined(DIFFERENTIAL_ALLOCATION)
	reproduction_proportion_investment=foodBiomassDifferential/0.012f;
	#else
	reproduction_investment_subtraction=0;
	reproduction_investment_exponent=reproduction_proportion_investment_coefficient*(foodBiomass*MILLILITER_TO_LITER);
	reproduction_investment_power=reproduction_investment_exponent+this->reproduction_proportion_investment_amplitude;
	reproduction_proportion_investment=max<biomassType>(0,min<biomassType>(0.4f,reproduction_investment_power));
#endif

}

biomassType AnimalBiomassDynamics::createNewCohort(AnimalCohort& parentCohort, biomassType initialBiomass, vector<AnimalCohort> *juveniles){
	/* The attributes from the child cohort are inherited from the parent cohort*/
	EggCohort eggCohort;
	eggCohort.x=parentCohort.x;
	eggCohort.y=parentCohort.y;
	if(eggCohort.x<0||eggCohort.y<0){
		cout<<"Error. New cohort with negative eggs."<<endl;
	}
	eggCohort.isBottomAnimal=parentCohort.isBottomAnimal;
	/* The migration constant is not used in the egg stage, but it needs to be inherited form the parent*/
	eggCohort.migrationConstant = parentCohort.migrationConstant;
	/*The others are attributes for newborn cohorts*/
	eggCohort.ageInHours=0;
	eggCohort.numberOfEggs=(animalCountType)(initialBiomass/initial_grazer_weight[AnimalStage::Egg]);
	eggCohort.biomass=(biomassType)eggCohort.numberOfEggs*initial_grazer_weight[AnimalStage::Egg];
	//eggCohort.cohortID=(*this->cohortID)++;
	eggCohort.cohortID=parentCohort.cohortID;
	eggCohort.hasHatched=false;
	/*Add to the correct map of eggs*/
	if(eggCohort.isBottomAnimal){
		bottomEggs.push_back(eggCohort);
	} else {
		floatingEggs.push_back(eggCohort);
	}
//	AnimalCohort juvenileCohort=parentCohort;
//	pair<int,int> cohortCoordinates(parentCohort.x, parentCohort.y);
//	juvenileCohort.bodyBiomass=initialBiomass;
//	juvenileCohort.stage=AnimalStage::Juvenile;
//	juvenileCohort.gonadBiomass=0.0f;
//	juvenileCohort.numberOfIndividuals=(animalCountType)(initialBiomass/initial_grazer_weight[AnimalStage::Juvenile]);
//	juvenileCohort.cohortID=(*this->cohortID)++;
//	juvenileCohort.upDirection=false;
//	juveniles->push_back(juvenileCohort);
	return eggCohort.biomass;
}
#ifdef MATURE_JUVENILES
void AnimalBiomassDynamics::matureEggs(vector<EggCohort>& eggs, vector<AnimalCohort>& juveniles){
#else
void AnimalBiomassDynamics::matureEggs(vector<EggCohort>& eggs, map<pair<int,int>,AnimalCohort> *juveniles){
#endif
	for (std::vector<EggCohort>::iterator it=eggs.begin(); it!=eggs.end(); ++it){
		/* Increase egg age and check if the egg has hatched*/
		it->hasHatched=++it->ageInHours>incubation_hours;
		const EggCohort& eggCohort=*it;
		/* If eggs are counted, report number of eggs found*/
#ifdef COUNT_EGGS
		lineBuffer.str("");
		lineBuffer.clear();
		lineBuffer<<eggCohort.x;
		lineBuffer<<commaString<<eggCohort.y;
		lineBuffer<<commaString<<(*current_hour);
		lineBuffer<<commaString<<eggCohort.isBottomAnimal?1:0;
#ifdef EXTENDED_OUTPUT
		lineBuffer<<commaString<<0;
		lineBuffer<<commaString<<0;
		lineBuffer<<commaString<<0;
		lineBuffer<<commaString<<0;
		lineBuffer<<commaString<<0;
		lineBuffer<<commaString<<0;
		lineBuffer<<commaString<<0;
		lineBuffer<<commaString<<0;
		lineBuffer<<commaString<<0;
		lineBuffer<<commaString<<0;
		lineBuffer<<commaString<<0;
		lineBuffer<<commaString<<0;
		lineBuffer<<commaString<<0;
		lineBuffer<<commaString<<0;
		lineBuffer<<commaString<<0;
		lineBuffer<<commaString<<0;
		lineBuffer<<commaString<<0;
		lineBuffer<<commaString<<0;
		lineBuffer<<commaString<<0;
		lineBuffer<<commaString<<0;
		lineBuffer<<commaString<<0;
		lineBuffer<<commaString<<0;
		lineBuffer<<commaString<<0;
		lineBuffer<<commaString<<0;
		lineBuffer<<commaString<<0;
#endif
		lineBuffer<<commaString<<0<<commaString<<eggCohort.numberOfEggs<<commaString<<0<<commaString<<0<<commaString<<eggCohort.biomass<<commaString<<0<<commaString<<0<<commaString<<0<<commaString<<0<<commaString<<0<<commaString<<0<<commaString<<0<<commaString<<this->lakeLightAtDepth[eggCohort.x][eggCohort.y]<<commaString<<0<<commaString<<0<<commaString<<0<<commaString<<AnimalStage::Egg<<commaString<<eggCohort.cohortID<<endl;
		animalBiomassBuffer<<lineBuffer.str();
#endif
		/*If the egg has hatched, add to the adult cohort*/
	    if(eggCohort.hasHatched){
#ifdef REPORT_COHORT_INFO
	    	cout<<"Hour: "<<*(this->current_hour)<<". Maturing egg cohort: "<<eggCohort<<"."<<endl;
#endif

#ifndef MATURE_JUVENILES
	    	pair<int,int> cohortCoordinates(eggCohort.x, eggCohort.y);
	    	if ( juveniles->find(cohortCoordinates) == juveniles->end() ) {
#endif
	    		AnimalCohort animalCohort;
	    		animalCohort.x=eggCohort.x;
	    		animalCohort.y=eggCohort.y;
	    		animalCohort.bodyBiomass=eggCohort.biomass;
	    		animalCohort.numberOfIndividuals=eggCohort.numberOfEggs;
	    		animalCohort.cohortID=eggCohort.cohortID;
	    		animalCohort.justMatured=true;
	    		animalCohort.isBottomAnimal=eggCohort.isBottomAnimal;
	    		animalCohort.gonadBiomass=animalCohort.starvationBiomass=0.0f;
	    		animalCohort.latestMigrationIndex=0;
	    		/* The migration constant is not used in the juvenile stage*/
	    		animalCohort.migrationConstant = eggCohort.migrationConstant;
	    		animalCohort.ageInHours=animalCohort.hoursInStarvation=0;
	    		animalCohort.previousFitnessValue=animalCohort.currentFitnessValue=0.0f;
	    		animalCohort.upDirection = false;
#ifdef MATURE_JUVENILES
	    		animalCohort.stage=AnimalStage::Juvenile;
	    		juveniles.push_back(animalCohort);
#else
	    		animalCohort.stage=AnimalStage::Mature;
	    		/* If the adult cohort exists, increase biomass and number of eggs*/
	    		(*juveniles)[cohortCoordinates]=animalCohort;
	    	} else{
	    		/* Otherwise, add number of individuals and biomass to the existing cohort*/
	    		(*juveniles)[cohortCoordinates]+=animalCohort;
	    	}
//	    	animalCohort.death=causeOfDeath::None;
//	    	animalCohort.hoursWithoutFood=animalCohort.ageInHours=0;
	    	/*Register newly hatched individuals as adults*/
#endif


#ifdef REPORT_COHORT_INFO
	    	cout<<"Egg cohort matured."<<endl;
#endif
	    }

	}
	/* Remove eggs that have hatched*/
	int size_before_erase=eggs.size();
	eggs.erase(std::remove_if(eggs.begin(), eggs.end(), removeOldEggs()),
			eggs.end());
	int size_after_erase=eggs.size();

}

#ifdef MATURE_JUVENILES

/* Mature juveniles and update their biomass */
#ifdef ANIMAL_COHORT_MAP
void AnimalBiomassDynamics::matureJuveniles(vector<AnimalCohort>& juveniles, map<pair<int,int>,AnimalCohort> *adultAnimals){
#else
void AnimalBiomassDynamics::matureJuveniles(vector<AnimalCohort>& juveniles, vector<AnimalCohort> *adultAnimals){
#endif
	for (std::vector<AnimalCohort>::iterator it = juveniles.begin();
			it != juveniles.end(); ++it) {
		updateCohortBiomass(*it);
		/* Remove cohort if the number of animals or its biomass is 0*/
		/*Mature age*/
		if(++it->ageInHours>=this->maturation_hours){
			/* Create a copy of the cohort to store in adult animals. After erasing it from the juveniles collection, the original cohort will be deleted*/
			it->stage=AnimalStage::Mature;
			AnimalCohort cohortCopy;
			/*Update the fields of the copy*/
			cohortCopy.ageInHours=it->ageInHours;
			cohortCopy.bodyBiomass=it->bodyBiomass;
			cohortCopy.cohortID=it->cohortID;
			cohortCopy.gonadBiomass=it->gonadBiomass;
			cohortCopy.isBottomAnimal=it->isBottomAnimal;
			cohortCopy.numberOfIndividuals=it->numberOfIndividuals;
			cohortCopy.stage=it->stage;
			cohortCopy.x=it->x;
			cohortCopy.y=it->y;
			cohortCopy.starvationBiomass=it->starvationBiomass;
			cohortCopy.latestMigrationIndex=it->latestMigrationIndex;
			cohortCopy.migrationConstant=it->migrationConstant;
			cohortCopy.hoursInStarvation=it->hoursInStarvation;
			pair<int,int> cohortCoordinates(cohortCopy.x, cohortCopy.y);
//			if(indexToDepth[maxDepthIndex[cohortCopy.y]]>=TRACED_COHORT_DEPTH&&tracedCohort.numberOfIndividuals==0){
//				/*Create traced cohort*/
//				tracedCohort=cohortCopy;
//				this->tracedCohortID=tracedCohort.cohortID;
//				cout<<"Traced cohort: "<<this->tracedCohortID<<"."<<endl;
//			} else{
				/*Add cohort to the collection of adult cohorts*/
#ifdef ANIMAL_COHORT_MAP
				if ( adultAnimals->find(cohortCoordinates) == adultAnimals->end() ) {
					cohortCopy.upDirection=false;
					(*adultAnimals)[cohortCoordinates]=cohortCopy;
				} else{
					(*adultAnimals)[cohortCoordinates]+=cohortCopy;
				}
#else
				adultAnimals->push_back(cohortCopy);
#endif
//			}
		}


	}
	/*Remove starved or mature juveniles*/
	juveniles.erase(std::remove_if(juveniles.begin(), juveniles.end(), removeStarvedOrMaturedJuveniles()), juveniles.end());
}
#endif

#endif

/* If the migration index is greater than 0, migrate adult and juvenile cohorts*/

void AnimalBiomassDynamics::migrateAnimalCohorts(){
	int migrationStep = zooplanktonBiomassCenterDifferencePerDepth[*current_hour%HOURS_PER_DAY];
//	calculateKairomonesConcetration();
	calculatePlanktivoreBiomass();
	generateMigrationIndexes();
	findNormalizingFactors();
	findOptimalDepthIndexes();
#ifdef RANDOM_WALK_MIGRATION
	migrateCohortsUsingRandomWalk();
#else
#ifdef ANIMAL_COHORT_MAP
	migrateAdultCohorts(floatingAnimals,migrationStep);
#else
#if defined(LIGHT_BASED_MIGRATION_VARIABLE_FREQUENCY) || defined(LIGHT_BASED_MIGRATION_FIXED_FREQUENCY) || defined(OPTIMAL_VALUE_MIGRATION)
	migrateJuvenileCohortsDepthDependent(*floatingAnimals);
#else
	migrateJuvenileCohortsStructurally(floatingAnimals,migrationStep);
#endif
#endif
#if defined(LIGHT_BASED_MIGRATION_VARIABLE_FREQUENCY) || defined(LIGHT_BASED_MIGRATION_FIXED_FREQUENCY) || defined(OPTIMAL_VALUE_MIGRATION)
	migrateJuvenileCohortsDepthDependent(floatingJuveniles);
#else
	migrateJuvenileCohortsStructurally(floatingJuveniles,migrationStep);
#endif
#endif
#ifdef INDIVIDUAL_BASED_ANIMALS
	/* Register the migration movement*/
	registerMigration();
#endif
}

/* Migrate the center of mass of the adult cohorts*/
void AnimalBiomassDynamics::migrateAdultCohorts(std::map<pair<int,int>,AnimalCohort> *animals, int migrationStep){
#if defined(LIGHT_BASED_MIGRATION_VARIABLE_FREQUENCY) || defined(LIGHT_BASED_MIGRATION_FIXED_FREQUENCY) || defined(OPTIMAL_VALUE_MIGRATION)
	/*Light-dependent migration occurs even when the migration step set is 0*/
	clearMigrationParameters();
	migrateAdultCohortsDepthDependent(animals);
	updateMigratedCohorts(animals);

#else
	if(migrationStep!=0){
		clearMigrationParameters();
		migrateAdultsCohortsStructurally(animals, migrationStep);
		updateMigratedCohorts(animals);
	}
#endif


}

void AnimalBiomassDynamics::migrateAdultsCohortsStructurally(std::map<pair<int,int>,AnimalCohort> *animals, int migrationStep){
	//	AnimalCohort& cohortAlias=(*animals)[pair<int,int>(11,0)];
	if(migrationStep!=0){
		for (std::map<pair<int,int>,AnimalCohort>::iterator it = animals->begin();
				it != animals->end(); ++it) {
			AnimalCohort& cohort = it->second;
			if(cohort.numberOfIndividuals>0&&cohort.bodyBiomass>0.0f){
				/* Migrate non-empty cohorts */
				bool migratedBiomass = updateMigrationTable(cohort, migrationStep);
			}
			/*If the cohort has migrated, remove it from its previous location*/

		}
	}
}

/* Set migration parameters to 0*/
void AnimalBiomassDynamics::clearMigrationParameters(){

			for(int depthIndex=0; depthIndex<MAX_DEPTH_INDEX; depthIndex++){
					for(int columnIndex=0; columnIndex<MAX_COLUMN_INDEX; columnIndex++){
						this->migratedFloatingAnimalStarvationBiomass[depthIndex][columnIndex]=this->migratedFloatingAnimalBodyBiomass[depthIndex][columnIndex] = this->migratedFloatingAnimalGonadBiomass[depthIndex][columnIndex] =0.0f;
						this->migratedFloatingAnimalCount[depthIndex][columnIndex]=0;
					}
				}
}

/*Update cohort coordinates using the migration arrays*/
void AnimalBiomassDynamics::updateMigratedCohorts(std::map<pair<int,int>,AnimalCohort> *animals){
	for(int columnIndex=0; columnIndex<MAX_COLUMN_INDEX; columnIndex++){
		/* Migrate only above the max depth*/
		for(int depthIndex=0; depthIndex<=maxDepthIndex[columnIndex]; depthIndex++){
			/* If there exists migration, change animal metrics*/
			if(this->migratedFloatingAnimalCount[depthIndex][columnIndex]!=0){
				pair<int,int> migratedCoordinates(depthIndex, columnIndex);
//				if(tracedCohort.numberOfIndividuals!=0&&depthIndex==tracedCohort.x&&columnIndex==tracedCohort.y){
//					/* If the migrated cohort is the traced cohort, update it using the migration tables*/
//					//if(migratedFloatingAnimalBodyBiomass[depthIndex][columnIndex]>0){
//						tracedCohort.bodyBiomass+=migratedFloatingAnimalBodyBiomass[depthIndex][columnIndex];
//						tracedCohort.gonadBiomass+=migratedFloatingAnimalGonadBiomass[depthIndex][columnIndex];
//						tracedCohort.numberOfIndividuals+=migratedFloatingAnimalCount[depthIndex][columnIndex];
//						if(tracedCohort.numberOfIndividuals<0){
//							cout<<"Error. Traced individuals is lower than 0."<<endl;
//						}
//					//}
//
//				} else{
					/* Otherwise, update the existing cohort (if exists) or create a new cohort*/
					if(animals->find(migratedCoordinates)==animals->end()){
						/* If the coordinate is not occupied, create a new cohort*/
						if(migratedFloatingAnimalCount[depthIndex][columnIndex]<0||migratedFloatingAnimalBodyBiomass[depthIndex][columnIndex]<0.0f||migratedFloatingAnimalGonadBiomass[depthIndex][columnIndex]<0.0f||(*animals)[migratedCoordinates].starvationBiomass<0.0f){
							cout<<"Error. Negative migrated individuals."<<endl;
						}
						AnimalCohort createdCohort;
						createdCohort.x=depthIndex;
						createdCohort.y=columnIndex;
						createdCohort.bodyBiomass=migratedFloatingAnimalBodyBiomass[depthIndex][columnIndex];
						createdCohort.gonadBiomass=migratedFloatingAnimalGonadBiomass[depthIndex][columnIndex];
						createdCohort.numberOfIndividuals=migratedFloatingAnimalCount[depthIndex][columnIndex];
						createdCohort.migrationConstant=migratedFloatingMigrationConstant[depthIndex][columnIndex];
						createdCohort.stage=AnimalStage::Mature;
						createdCohort.starvationBiomass=migratedFloatingAnimalStarvationBiomass[depthIndex][columnIndex];
						createdCohort.isBottomAnimal=false;
						createdCohort.ageInHours=0;
						createdCohort.latestMigrationIndex= createdCohort.hoursInStarvation = 0;
						createdCohort.cohortID=migratedFloatingAnimalCohortID[depthIndex][columnIndex];;
						(*animals)[migratedCoordinates]=createdCohort;
						if((*animals)[migratedCoordinates].stage!=AnimalStage::Mature){
							cout<<"Error. Non-mature migration update."<<endl;
						}
					} else{
						/* If the coordinate is occupied, update the existing cohort with the immigrant individuals. Update number of individuals and biomass*/
						migrateExistingAdultCohort((*animals)[migratedCoordinates], depthIndex, columnIndex);
						//						if((*animals)[migratedCoordinates].numberOfIndividuals==0||(*animals)[migratedCoordinates].bodyBiomass==0.0f){
						//							AnimalCohort migratedCohort = (*animals)[migratedCoordinates];
						//							/*Remove empty cohorts*/
						////							animals->erase(migratedCoordinates);
						//						}
					}
//				}


			}

		}
	}
}

void AnimalBiomassDynamics::migrateExistingAdultCohort(AnimalCohort& cohort, int depthIndex, int columnIndex){
	/* If the coordinate is occupied, update the values*/
	cohort.numberOfIndividuals+=migratedFloatingAnimalCount[depthIndex][columnIndex];
	cohort.bodyBiomass+=migratedFloatingAnimalBodyBiomass[depthIndex][columnIndex];
	cohort.gonadBiomass+=migratedFloatingAnimalGonadBiomass[depthIndex][columnIndex];
	cohort.migrationConstant=migratedFloatingMigrationConstant[depthIndex][columnIndex];
	cohort.starvationBiomass+=migratedFloatingAnimalStarvationBiomass[depthIndex][columnIndex];
	cohort.cohortID=migratedFloatingAnimalCohortID[depthIndex][columnIndex];
	if(cohort.numberOfIndividuals<0||cohort.bodyBiomass<0.0f||cohort.gonadBiomass<0.0f||cohort.starvationBiomass<0.0f){
		cout<<"Error. Negative biomass."<<endl;
	}
	if(cohort.stage!=AnimalStage::Mature){
		cout<<"Error. Non-mature stage."<<endl;
	}
#ifdef LIGHT_BASED_MIGRATION_FIXED_FREQUENCY
//	if(lakeLightAtDepth[depthIndex][columnIndex]<this->critical_light_intensity){
//		cout<<"Adult lake light at coordinates: "<<cohort.x<<", "<<cohort.y<<" is "<<critical_light_intensity<<"."<<endl;
//	}
#endif
}
void AnimalBiomassDynamics::migrateJuvenileCohortsStructurally(vector<AnimalCohort>& juveniles, int migrationStep){
	if(migrationStep!=0){
		for (std::vector<AnimalCohort>::iterator it = juveniles.begin();
				it != juveniles.end(); ++it) {
			migrateJuvenileCohortStructurally(*it, migrationStep);
			//(*it)*=0.9f;
		}
		/*Migrate traced cohort as a special case*/
		if(tracedCohort.numberOfIndividuals!=0){
			migrateJuvenileCohortStructurally(tracedCohort, migrationStep);
		}
	}
}

void AnimalBiomassDynamics::migrateCohortsUsingRandomWalk(){
	migrateCohortsUsingRandomWalk(*floatingAnimals);
	migrateCohortsUsingRandomWalk(floatingJuveniles);
	if(tracedCohort.numberOfIndividuals!=0){
		migrateCohortUsingRandomWalk(tracedCohort);
	}
}

void AnimalBiomassDynamics::migrateCohortsUsingRandomWalk(vector<AnimalCohort>& cohorts){
	for (std::vector<AnimalCohort>::iterator it = cohorts.begin();
			it != cohorts.end(); ++it) {
		migrateCohortUsingRandomWalk(*it);
		//(*it)*=0.9f;
	}
			/*Migrate traced cohort as a special case*/

}

bool AnimalBiomassDynamics::updateMigrationTable(AnimalCohort& cohort, int migrationStep){
	/*Migrate the center of biomass of the animal cohorts*/
	int destinationX =cohort.x+migrationStep;
	/* Register the latest migration step*/
	cohort.latestMigrationIndex = migrationStep;
	destinationX=max<int>(0,min<int>(maxDepthIndex[cohort.y], destinationX));
	/*Check the limits of the new depth of the cohort*/
	bool verticalMigrationOccurred = destinationX>=0&&destinationX<MAX_DEPTH_INDEX&&destinationX<=maxDepthIndex[cohort.y]&&destinationX!=cohort.x;
	if(verticalMigrationOccurred){

		int originalX=cohort.x, originalY=cohort.y;
		int destinationY=cohort.y;
//		if(destinationX != 0 && migrationStep != -this->velocity_downward_pull &&lakeLightAtDepth[destinationX][destinationY]<this->critical_light_intensity){
//			/*If the cohort is not at the surface (destination!=0), and the migration speed is not maximum upwards, and it is too dark for the cohort, report an error*/
//			cout<<"Adult lake light at coordinates: "<<destinationX<<", "<<destinationY<<" is "<<critical_light_intensity<<"."<<endl;
//		}
//		if(destinationX== 150){
//			cout<<"Lake light "<<lakeLightAtDepth[destinationX][destinationY]<<" at coordinates: ("<<destinationX<<", "<<destinationY<<")."<<endl;
//		}
		/*Increment the biological values of the destination cohort with the migrated cohort*/
		this->migratedFloatingAnimalBodyBiomass[destinationX][destinationY]+=cohort.bodyBiomass;
		this->migratedFloatingAnimalGonadBiomass[destinationX][destinationY]+=cohort.gonadBiomass;
		this->migratedFloatingAnimalCount[destinationX][destinationY]+=cohort.numberOfIndividuals;
		this->migratedFloatingAnimalStarvationBiomass[destinationX][destinationY]+=cohort.starvationBiomass;
		this->migratedFloatingMigrationConstant[destinationX][destinationY]=cohort.migrationConstant;
		this->migratedFloatingAnimalCohortID[destinationX][destinationY]=cohort.cohortID;
		/*Decrement the biological values of the origin cohort with the migrated cohort*/

		this->migratedFloatingAnimalBodyBiomass[originalX][originalY]-=cohort.bodyBiomass;
		this->migratedFloatingAnimalGonadBiomass[originalX][originalY]-=cohort.gonadBiomass;
		this->migratedFloatingAnimalCount[originalX][originalY]-=cohort.numberOfIndividuals;
		this->migratedFloatingAnimalStarvationBiomass[originalX][originalY]-=cohort.starvationBiomass;
	}
	return verticalMigrationOccurred;
}

void AnimalBiomassDynamics::migrateJuvenileCohortStructurally(AnimalCohort& cohort, int migrationStep){
	/*Migrate the center of biomass of the animal cohorts*/
	int destinationX =cohort.x+migrationStep;
	destinationX=max<int>(0,min<int>(maxDepthIndex[cohort.y], destinationX));
	/*Check the limits of the new depth of the cohort*/
	if(destinationX>=0&&destinationX<MAX_DEPTH_INDEX&&destinationX<=maxDepthIndex[cohort.y]&&destinationX!=cohort.x){
		cohort.x=destinationX;
		if(cohort.cohortID!=this->tracedCohortID&&cohort.stage!=AnimalStage::Juvenile){
			cout<<"Error. The stage is not juvenile."<<endl;
		}
	}
}

void AnimalBiomassDynamics::migrateJuvenileCohortsDepthDependent(vector<AnimalCohort>& animals){

	for (std::vector<AnimalCohort>::iterator it = animals.begin();
				it != animals.end(); ++it) {
		migrateJuvenileCohortDepthDependent(it);
		/*If the cohort has migrated, remove it from its previous location*/

	}
	if(tracedCohort.numberOfIndividuals!=0){
		/* Migrate traced cohort as a special case*/
		migrateJuvenileCohortDepthDependent(tracedCohort);
	}
}

/*It is necessary to have repeated methods for the iterator and the cohort itself because, when migrating juveniles, the cohort is given as an iterator. However, when migrated traced cohorts, the cohort is given as an instance of AnimalCohort. It is not possible to cast between Iterator to AnimalCohort */

void AnimalBiomassDynamics::consumeDuringMigration(int initialDepth, int finalDepth,
		AnimalCohort& it) {
	if(this->migration_consumption>0.0f&&initialDepth!=finalDepth){
		/* If initialDepth=finalDepth, there is no migration*/
		if (initialDepth > finalDepth) {
			/* Initial depth< final depth */
			int tempVar = finalDepth;
			finalDepth = initialDepth;
			initialDepth = tempVar;
		}
		finalDepth--;
		initialDepth++;
		/*Consume for each depth proportionally. Normalized by the number of visited depths*/
		biomassType consumedProportion = (1.0f/(biomassType) (finalDepth - initialDepth + 1))*migration_consumption;
		for (int depthIndex = initialDepth; depthIndex <= finalDepth; ++depthIndex) {
			biomassType availableFood=it.isBottomAnimal?bottomFoodBiomass[it.y]:floatingFoodBiomass[depthIndex][it.y];
			foodConsumptionRate(depthIndex, it.y, it.isBottomAnimal,
					it.numberOfIndividuals, availableFood,
					this->initial_grazer_weight[it.stage], consumedProportion);
		}
	}

}

void AnimalBiomassDynamics::consumeDuringMigration(int initialDepth, int finalDepth,
		std::vector<AnimalCohort>::iterator it) {
	if(this->migration_consumption>0.0f&&initialDepth!=finalDepth){
		/* If initialDepth=finalDepth, there is no migration*/
		if (initialDepth > finalDepth) {
			/* Initial depth< final depth */
			int tempVar = finalDepth;
			finalDepth = initialDepth;
			initialDepth = tempVar;
		}
		finalDepth--;
		initialDepth++;
		/*Consume for each depth proportionally. Normalized by the number of visited depths*/
		biomassType consumedProportion = (1.0f/(biomassType) (finalDepth - initialDepth + 1))*migration_consumption;
		for (int depthIndex = initialDepth; depthIndex <= finalDepth; ++depthIndex) {
			biomassType availableFood=it->isBottomAnimal?bottomFoodBiomass[it->y]:floatingFoodBiomass[depthIndex][it->y];
			foodConsumptionRate(depthIndex, it->y, it->isBottomAnimal,
					it->numberOfIndividuals, availableFood,
					this->initial_grazer_weight[it->stage], consumedProportion);
		}
	}

}


/* Migrate juvenile cohorts assuming a stochastic approach. Cohorts might move to a suboptimal cell to escape from local maxima*/
void AnimalBiomassDynamics::migrateCohortUsingRandomWalk(AnimalCohort& cohort){
	double searchStepCounter=1;
	int localeVerticalCoordinate=cohort.x, localeHorizontalCoordinate=cohort.y;
	/* Check if the group is in a cell that is too dark. Therefore, daphnia will migrate out of it*/
	physicalType localeLightLevel=lakeLightAtDepth[localeVerticalCoordinate][localeHorizontalCoordinate];
	bool currentCellTooDark= localeLightLevel<this->minimum_tolerable_light;
//	if(currentCellTooDark){
//		cout<<"Cell too dark."<<endl;
//	}
#ifdef	LINEAR_MIGRATION_COMBINATION
	biomassType originFitnessValue = localeFitnessValue[localeVerticalCoordinate][localeHorizontalCoordinate];
#else
	/* If the model is running in rule-based mode, only expose to migration when starvation occurs. Otherwise, follow the standard circadian rhythm*/
	biomassType originFitnessValue = predatorPropensity[localeVerticalCoordinate][localeHorizontalCoordinate];
	bool starvationMode = cohort.hoursInStarvation>this->starvation_max_hours;
	if(starvationMode){
		originFitnessValue = localeFitnessValue[localeVerticalCoordinate][localeHorizontalCoordinate];
	}
#endif
	cohort.previousFitnessValue =cohort.currentFitnessValue= originFitnessValue;
	for (std::vector<pair<int,int>>::iterator migrationIndexPair = hourlyMigrationIndexPairs->begin();
			migrationIndexPair != hourlyMigrationIndexPairs->end()&&searchStepCounter<=this->max_search_steps; ++migrationIndexPair) {
		/* Retrieve migration indexes*/
		int verticalIndex=migrationIndexPair->first, horizontalIndex=migrationIndexPair->second;
			/* Calculate the destination coordinates as the current coordinates plus the migration indexes*/
			int destinationVertical = localeVerticalCoordinate+verticalIndex;
			int destinationHorizontal = localeHorizontalCoordinate+horizontalIndex;
			if(destinationHorizontal>=0&&destinationHorizontal<=MAX_COLUMN_INDEX){
				if(destinationVertical>=0&&destinationVertical<=MAX_DEPTH_INDEX){
					physicalType destinationLightLevel = lakeLightAtDepth[destinationVertical][destinationHorizontal];
					/*If the cell exists and has enough light, attempt movement. If the current cell is too dark and the destination cell has more light, attempt movement*/
					if(maxDepthIndex[destinationHorizontal]>=destinationVertical&&(destinationLightLevel>=this->minimum_tolerable_light||(currentCellTooDark&&(destinationLightLevel>localeLightLevel)))){
						/*If the cell is reachable*/
#ifdef	LINEAR_MIGRATION_COMBINATION
						biomassType destinationFitnessValue = localeFitnessValue[destinationVertical][destinationHorizontal];
#else
						/* If the model is running in rule-based mode, only expose to migration when starvation occurs. Otherwise, follow the standard circadian rhythm*/
						biomassType destinationFitnessValue = predatorPropensity[destinationVertical][destinationHorizontal];
						if(starvationMode){
							destinationFitnessValue = localeFitnessValue[destinationVertical][destinationHorizontal];
						}
#endif
						/* If the group needs to migrate because of the darkness, it may accept a suboptimal destination cell*/
						if(destinationFitnessValue>originFitnessValue||currentCellTooDark){
							/*If the destination fitness value is greater than the previous fitness value, move the group*/
							cohort.x=destinationVertical;
							cohort.y=destinationHorizontal;
							cohort.currentFitnessValue=destinationFitnessValue;
							/*Update light values*/
							localeLightLevel=destinationLightLevel;
							currentCellTooDark= localeLightLevel<this->minimum_tolerable_light;
						} else{
							/*Otherwise, move it with a certain probability proportional to the fitness difference*/

							double randomNumber = ((double) rand() / (RAND_MAX));
							double acceptanceProbabilityExponent = (destinationFitnessValue-originFitnessValue)/(this->random_walk_probability_weight);
							double movementProbability = exp(acceptanceProbabilityExponent);
							if(randomNumber<=movementProbability){
								cohort.x=destinationVertical;
								cohort.y=destinationHorizontal;
								cohort.currentFitnessValue=destinationFitnessValue;
								/*Update light values*/
								localeLightLevel=destinationLightLevel;
								currentCellTooDark= localeLightLevel<this->minimum_tolerable_light;
							}
						}
						/* Update the number of search steps per group*/
						searchStepCounter++;
					}
				}
			}
	}
	//(*it)*=0.9f;
	/*Migrate traced cohort as a special case*/
}


/*It is necessary to have repeated methods for the iterator and the cohort itself because, when migrating juveniles, the cohort is given as an iterator. However, when migrated traced cohorts, the cohort is given as an instance of AnimalCohort. It is not possible to cast between Iterator to AnimalCohort */
void AnimalBiomassDynamics::migrateJuvenileCohortDepthDependent(std::vector<AnimalCohort>::iterator it){
	if(it->numberOfIndividuals>0&&it->bodyBiomass>0.0f){
		/* Migrate non-empty cohorts */
#if defined(LIGHT_BASED_MIGRATION_VARIABLE_FREQUENCY)
		int migratedDepth = migrateCohortsDepthDependent(*it);
#elif defined(LIGHT_BASED_MIGRATION_FIXED_FREQUENCY)
		int migratedDepth = migrateCohortsFixedFrequency(*it);
#elif defined(OPTIMAL_VALUE_MIGRATION)
		if(abs(it->migrationConstant)>2){
			cout<<"Error on migration constant: "<<it->migrationConstant<<"."<<endl;
		}
//		if(optimalDepthIndexes[it->y]>3){
//			cout<<"Deep index."<<endl;
//		}
		int migratedDepth = it->migrationConstant + optimalDepthIndexes[it->y]-it->x;
#else
		int migratedDepth = 0;
#endif
		if(migratedDepth!=0){
			/* Consume algae during migration */
			int initialDepth=it->x;
			it->x+=migratedDepth;
			it->x=max<int>(0, min<int>(maxDepthIndex[it->y], it->x));
			int finalDepth=it->x;
//			if(finalDepth!=0){
//				cout<<"Traced cohort moved."<<endl;
//			}
#ifdef CONSUME_DURING_MIGRATION
			consumeDuringMigration(initialDepth, finalDepth, it);
#endif
#ifdef LIGHT_BASED_MIGRATION_FIXED_FREQUENCY
			if(it->x != 0 && migratedDepth != -this->velocity_downward_pull && lakeLightAtDepth[it->x][it->y]<this->critical_light_intensity){
				cout<<"Juvenile lake light at coordinates: "<<it->x<<", "<<it->y<<" is "<<critical_light_intensity<<"."<<endl;
			}
#endif
		}

	}
}


void AnimalBiomassDynamics::migrateJuvenileCohortDepthDependent(AnimalCohort& cohort){
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
void AnimalBiomassDynamics::migrateAdultCohortsDepthDependent(std::map<pair<int,int>,AnimalCohort>* animals){
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
void AnimalBiomassDynamics::migrateAdultCohort(AnimalCohort& cohort){
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
int AnimalBiomassDynamics::migrateCohortsDepthDependent(AnimalCohort& cohort){

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


int AnimalBiomassDynamics::migrateCohortsFixedFrequency(AnimalCohort& cohort){
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


void AnimalBiomassDynamics::findOptimalDepthIndexes(){
	for (int columnIndex = 0; columnIndex <= MAX_COLUMN_INDEX; ++columnIndex) {
		findOptimalDepthIndex(columnIndex);
	}
//	for (int columnIndex = 0; columnIndex <= MAX_COLUMN_INDEX; ++columnIndex) {
//		if(maxDepthIndex[columnIndex]!=optimalDepthIndex[columnIndex]){
//			cout<<"Difference found."<<endl;
//		}
//	}
}
void AnimalBiomassDynamics::findOptimalDepthIndex(unsigned int columnIndex){

	int optimalDepthIndex=0;
	/*Calculate the value to optimize as the weighted sum of the inverse of the distance to optimal light normalized and the amount of food normalized*/
	physicalType lightValueToOptimize=(light_migration_weight)*predatorPropensity[0][columnIndex]/sumOptimalLightValues;
	physicalType foodValueToOptimize =(1-light_migration_weight)*floatingFoodBiomass[0][columnIndex]/sumOptimalFoodValues;
	physicalType valueToOptimize = lightValueToOptimize+foodValueToOptimize;
	for (int depthIndex = 1; depthIndex <= maxDepthIndex[columnIndex]; ++depthIndex) {
		/*Inverse of the distance to optimal light normalized*/
		physicalType localeLight = light_migration_weight*predatorPropensity[depthIndex][columnIndex]/sumOptimalLightValues;
		/*Amount of food normalized*/
		physicalType localeFood=(1-light_migration_weight)*floatingFoodBiomass[depthIndex][columnIndex]/sumOptimalFoodValues;
		physicalType localeComposedValue = localeLight+localeFood;
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

#endif


/* Set the maximum allocation biomass to 0.5 and set that the grazers left behind do not eat nor reproduce*/
#ifdef INDIVIDUAL_BASED_ANIMALS
void AnimalBiomassDynamics::registerMigration(){
	animalTraceBuffer.str("");
#ifdef REGISTER_ALL_COHORTS
#ifdef ANIMAL_COHORT_MAP
	/*Register migration for floating adults*/
	for (std::map<pair<int,int>,AnimalCohort>::iterator it = floatingAnimals->begin();
						it != floatingAnimals->end(); ++it) {
			AnimalCohort& iteratedCohort = it->second;
			animalTraceBuffer<<iteratedCohort.x<<commaString<<iteratedCohort.y<<commaString<<(*current_hour)<<commaString<<(iteratedCohort.isBottomAnimal?1:0)<<commaString<<iteratedCohort.stage<<commaString<<lakeLightAtDepth[iteratedCohort.x][iteratedCohort.y]<<commaString<<iteratedCohort.latestMigrationIndex<<commaString<<iteratedCohort.numberOfIndividuals<<commaString<<iteratedCohort.bodyBiomass<<commaString<<tracedCohort.migrationConstant<<commaString<<iteratedCohort.cohortID<<endl;
		}
#else
	for (std::vector<AnimalCohort>::iterator it = floatingAnimals->begin();
						it != floatingAnimals->end(); ++it) {
			AnimalCohort& iteratedCohort = it;
			animalTraceBuffer<<iteratedCohort.x<<commaString<<iteratedCohort.y<<commaString<<(*current_hour)<<commaString<<(iteratedCohort.isBottomAnimal?1:0)<<commaString<<iteratedCohort.stage<<commaString<<lakeLightAtDepth[iteratedCohort.x][iteratedCohort.y]<<commaString<<iteratedCohort.latestMigrationIndex<<commaString<<iteratedCohort.numberOfIndividuals<<commaString<<iteratedCohort.bodyBiomass<<commaString<<tracedCohort.migrationConstant<<commaString<<iteratedCohort.cohortID<<endl;
		}
#endif
	/*Register migration for floating juveniles*/
	for (std::vector<AnimalCohort>::iterator it = floatingJuveniles.begin();
						it != floatingJuveniles.end(); ++it) {
			AnimalCohort& iteratedCohort = *it;
			animalTraceBuffer<<iteratedCohort.x<<commaString<<iteratedCohort.y<<commaString<<(*current_hour)<<commaString<<(iteratedCohort.isBottomAnimal?1:0)<<commaString<<iteratedCohort.stage<<commaString<<lakeLightAtDepth[iteratedCohort.x][iteratedCohort.y]<<commaString<<iteratedCohort.latestMigrationIndex<<commaString<<iteratedCohort.numberOfIndividuals<<commaString<<iteratedCohort.bodyBiomass<<commaString<<tracedCohort.migrationConstant<<commaString<<iteratedCohort.cohortID<<endl;
		}
#endif
	/*Register migration for traced cohort*/
//	if(*current_hour>220){
//		cout<<"Hour limit reached."<<endl;
//	}
	if(tracedCohort.numberOfIndividuals!=0){
		biomassType fitnessDifference = tracedCohort.currentFitnessValue-tracedCohort.previousFitnessValue;
		animalTraceBuffer<<tracedCohort.x<<commaString<<tracedCohort.y<<commaString<<(*current_hour)<<commaString<<(tracedCohort.isBottomAnimal?1:0)<<commaString<<tracedCohort.stage<<commaString<<this->indexToDepth[tracedCohort.x]<<commaString<<lakeLightAtDepth[tracedCohort.x][tracedCohort.y]<<commaString<<tracedCohort.latestMigrationIndex<<commaString<<tracedCohort.numberOfIndividuals<<commaString<<tracedCohort.bodyBiomass<<commaString<<tracedCohort.previousFitnessValue<<commaString<<tracedCohort.currentFitnessValue<<commaString<<fitnessDifference<<commaString<<tracedCohort.hoursInStarvation<<commaString<<tracedCohort.migrationConstant<<commaString<<tracedCohort.cohortID<<endl;
//		if(tracedCohort.x!=0){
//			cout<<"Non-zero depth detected."<<endl;
//		}
	}

}

/*Calculate the summing of all light and food values*/


void AnimalBiomassDynamics::findNormalizingFactors(){
	sumOptimalLightValues=sumOptimalFoodValues=0.0f;
	for(int columnIndex=0; columnIndex<MAX_COLUMN_INDEX; ++columnIndex){
		for(int depthIndex=0; depthIndex<=maxDepthIndex[columnIndex]; ++depthIndex){
			/*Sum the inverse of the distance to optimal light*/
			predatorPropensity[depthIndex][columnIndex]=calculatePredationPropensity(depthIndex, columnIndex);
			sumOptimalLightValues+=predatorPropensity[depthIndex][columnIndex];
			/*Sum the value of food*/
			sumOptimalFoodValues+=floatingFoodBiomass[depthIndex][columnIndex];
		}
	}
}

physicalType AnimalBiomassDynamics::calculatePredationPropensity(int depthIndex, int columnIndex){
	/* Distance to optimal light is used for fitness*/
	physicalType localeLakeLightAtDepth = lakeLightAtDepth[depthIndex][columnIndex];
//	return 1.0f/fabs((localeLakeLightAtDepth-light_optimal_value)+1);
	/* To model risk of predation, light is multiplied by planktivore biomass*/
	biomassType locakePlanktivoreBiomass = this->planktivoreBiomass[depthIndex][columnIndex];
	biomassType calculatedLightPropensity = 1.0f/((localeLakeLightAtDepth*locakePlanktivoreBiomass)+1.0f);
//	if(calculatedLightPropensity!=1.0f){
//		cout<<"Light propensity greater than 0."<<endl;
//	}
	return calculatedLightPropensity;
}

void AnimalBiomassDynamics::removeEmptyCohorts(){
	/* Remove mature individuals that have either starved or grown too old*/
	floatingAnimals->erase(std::remove_if(floatingAnimals->begin(), floatingAnimals->end(), removeOldIndividuals()), floatingAnimals->end());
	bottomAnimals->erase(std::remove_if(bottomAnimals->begin(), bottomAnimals->end(), removeOldIndividuals()), bottomAnimals->end());
}


#ifndef ANIMAL_COHORT_MAP

void AnimalBiomassDynamics::reallocateSmallCohorts(){
#ifdef CONGLOMERATE_ALL_COHORTS
	agglomerateCohorts(floatingAnimals);
	agglomerateCohorts(bottomAnimals);
#else
	reallocateSmallCohorts(floatingAnimals, floatingReallocatedCohorts);
	reallocateSmallCohorts(bottomAnimals, bottomReallocatedCohorts);
#endif
}

#ifdef CONGLOMERATE_ALL_COHORTS
void AnimalBiomassDynamics::agglomerateCohorts(vector<AnimalCohort> *animals){
	animalMigrationBuffer.clear();
	bool foundCohort[MAX_DEPTH_INDEX][MAX_COLUMN_INDEX];
	for(int depthIndex=0;depthIndex<MAX_DEPTH_INDEX; depthIndex++){
		for(int columnIndex=0;columnIndex<MAX_COLUMN_INDEX; columnIndex++){
			foundCohort[depthIndex][columnIndex]=false;
		}
	}
	int maxID=0;
	for(vector<AnimalCohort>::iterator it=animals->begin(); it!=animals->end(); ++it){
		/* Agglomerate cohorts into bigger elements*/
		pair<int,int> cohortCoordinates = pair<int,int>(it->x,it->y);

		if(animalMigrationBuffer.find(cohortCoordinates)==animalMigrationBuffer.end()){
			/* If the cohort is not in the map, create*/
			animalMigrationBuffer[cohortCoordinates]=*it;
			maxID=max<int>(maxID, it->cohortID);
		} else{
			/* Otherwise, add it*/
			AnimalCohort& localeCohort =animalMigrationBuffer[cohortCoordinates];
			animalCountType totalAnimals = it->numberOfIndividuals + localeCohort.numberOfIndividuals;
			float animalProportion1 = (float)it->numberOfIndividuals/(float)totalAnimals, animalProportion2=(float)localeCohort.numberOfIndividuals/(float)totalAnimals;
			animalMigrationBuffer[cohortCoordinates].ageInHours=(it->ageInHours*animalProportion1+localeCohort.ageInHours*animalProportion2);
			animalMigrationBuffer[cohortCoordinates].bodyBiomass+=it->bodyBiomass;
			animalMigrationBuffer[cohortCoordinates].numberOfIndividuals+=it->numberOfIndividuals;
			animalMigrationBuffer[cohortCoordinates].gonadBiomass+=it->gonadBiomass;
			animalMigrationBuffer[cohortCoordinates].hoursInStarvation=max(it->hoursInStarvation, localeCohort.hoursInStarvation);

			/* Use a consistent rule for cohort ID (the minimum)*/
			if(!it->justMatured){
				animalMigrationBuffer[cohortCoordinates].cohortID=min<int>(animalMigrationBuffer[cohortCoordinates].cohortID, it->cohortID);
//				if(foundCohort[it->x][it->y]){
//					cout<<"Duplicated cohort."<<endl;
//				}
				foundCohort[it->x][it->y]=true;
			}

		}
		animalMigrationBuffer[cohortCoordinates].justMatured=false;
	}
	animals->clear();
	for(map<pair<int,int>,AnimalCohort>::iterator it=animalMigrationBuffer.begin(); it!=animalMigrationBuffer.end(); ++it){
		/* Put the cohorts back in the vector*/
		animals->push_back(it->second);
	}
//	cout<<"Number of animals: "<<animals->size()<<"."<<endl;
}
#else
void AnimalBiomassDynamics::reallocateSmallCohorts(vector<AnimalCohort> *animals, map<pair<int,int>,AnimalCohort>& reallocatedCohorts){

	/*Iterate over the large cohorts*/
	std::shuffle(animals->begin(), animals->end(), *animalRandomGenerator);
	for(vector<AnimalCohort>::iterator it=animals->begin(); it!=animals->end(); ++it){
		pair<int,int> cohortCoordinates = pair<int,int>(it->x,it->y);
		if(reallocatedCohorts.find(cohortCoordinates)!=reallocatedCohorts.end()){
			/* If there exists a small cohort with the same coordinates, add it to the large cohort*/
			AnimalCohort& foundCohort = reallocatedCohorts[cohortCoordinates];
			animalCountType totalAnimals = it->numberOfIndividuals + foundCohort.numberOfIndividuals;
			float animalProportion1 = (float)it->numberOfIndividuals/(float)totalAnimals, animalProportion2=(float)foundCohort.numberOfIndividuals/(float)totalAnimals;
			it->ageInHours=(it->ageInHours*animalProportion1+foundCohort.ageInHours*animalProportion2);
			it->bodyBiomass+=foundCohort.bodyBiomass;
			it->numberOfIndividuals+=foundCohort.numberOfIndividuals;
			it->gonadBiomass+=foundCohort.gonadBiomass;
			it->hoursInStarvation+=max(it->hoursInStarvation,foundCohort.hoursInStarvation);
			/* Erase reallocated cohort*/
			reallocatedCohorts.erase(cohortCoordinates);
		}
	}
	if(reallocatedCohorts.size()>0){
		/*Preserve small cohorts that do not have a big cohort at the same coordinates*/
		for(map<pair<int,int>,AnimalCohort>::iterator it=reallocatedCohorts.begin(); it!=reallocatedCohorts.end(); ++it){
			animals->push_back(it->second);
		}
//		cout<<"Not all cohorts reallocated."<<endl;
	}
	/* Clear the reallocated cohorts*/
	reallocatedCohorts.clear();
}
#endif
#endif

void AnimalBiomassDynamics::calculateKairomonesConcetration(){
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

/* Generate the vertical and horizontal migration indexes*/
void AnimalBiomassDynamics::generateMigrationIndexes(){

	/* Generate the list of index pairs for the current cycle*/
#ifdef DAPHNIA_CIRCADIAN_CYCLE
	if(this->dayTime){
		hourlyMigrationIndexPairs=&daytimeMigrationIndexPairs;
	} else{
		hourlyMigrationIndexPairs=&nighttimeMigrationIndexPairs;
	}

#else
	hourlyMigrationIndexPairs=&migrationIndexPairs;
#endif
	std::shuffle(hourlyMigrationIndexPairs->begin(), hourlyMigrationIndexPairs->end(), *animalRandomGenerator);
}

void AnimalBiomassDynamics::calculatePlanktivoreBiomass(){
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
				this->planktivoreBiomass[depthIndex][columnIndex]=planktivoreBiomassFactor;
			}
			else{
				/*Otherwise, clear fish biomass*/
				this->planktivoreBiomass[depthIndex][columnIndex]=0.0f;
			}

		}
	}

}

#endif
} /* namespace FoodWebModel */
