-include definitions.mk



$(OBJS): 
	@echo 'Building target: $@'
	@echo 'Invoking: GCC C++ Linker'
	g++ -g $(SRC) $(HEADERS) $(COMPILATION_OPTIONS) -o bin/$(OBJS)
	@echo 'Finished building target: $@'
	
.PHONY: debug all run

debug:
	@echo 'Compiling to debug target: $@'
	@echo 'Invoking: GCC C++ Linker'
	g++ -g $(SRC) $(HEADERS) $(COMPILATION_OPTIONS) -o bin/$(OBJS)
	@echo 'Finished building target: $@ for debugging'
	
all:
	make debug
	
run:
	@echo 'Compiling to run target: $@'
	@echo 'Invoking: GCC C++ Linker'
	g++ $(SRC) $(HEADERS) $(COMPILATION_OPTIONS) -o bin/$(OBJS)
	@echo 'Finished building target: $@ for running'