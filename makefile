-include definitions.mk



$(OBJS): 
	@echo 'Building target: $@'
	@echo 'Invoking: GCC C++ Linker'
	g++ -g $(SRC) $(COMPILATION_OPTIONS) -o bin/$(OBJS) $^
	@echo 'Finished building target: $@'
	
.PHONY: debug all

debug:
	@echo 'Debugging target: $@'
	@echo 'Invoking: GCC C++ Linker'
	g++ $(SRC) $(COMPILATION_OPTIONS) -o bin/$(OBJS) $^
	@echo 'Finished building target: $@ for debugging'
	
all:
	make debug