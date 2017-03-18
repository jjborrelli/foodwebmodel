-include definitions.mk



$(OBJS): 
	@echo 'Building target: $@'
	@echo 'Invoking: GCC C++ Linker'
	g++ $(SRC) -o bin/$(OBJS) $^
	@echo 'Finished building target: $@'
	
.PHONY: debug
debug:
	@echo 'Debugging target: $@'
	g++ -g $(SRC) -o bin/$(OBJS) $^
	@echo 'Finished building target: $@ for debugging'