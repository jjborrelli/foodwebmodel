-include definitions.mk



$(OBJS): 
	@echo 'Building target: $@'
	@echo 'Invoking: GCC C++ Linker'
	g++ $(SRC) -o bin/"FoodWebModel" $^
	@echo 'Finished building target: $@'
	