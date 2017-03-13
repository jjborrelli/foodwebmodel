-include definitions.mk

$(OBJS) :
	g++ $(SRC) -o $@

DebugTest: $(OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: GCC C++ Linker'
	g++ $(SRC) -o "DebugTest" bin/$^
	@echo 'Finished building target: $@'
	@echo ' '