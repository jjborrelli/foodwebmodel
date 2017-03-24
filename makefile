-include definitions.mk



$(OBJS): 
	@echo 'Building target: $@'
	@echo 'Invoking: GCC C++ Linker'
	g++ $(SRC) $(GDAL_INCLUDE) $(NETCDF_INCLUDE) $(GDAL_LIBRARIES) $(NETCDF_LIBRARIES) -o bin/$(OBJS) $^
	@echo 'Finished building target: $@'
	
.PHONY: debug
debug:
	@echo 'Debugging target: $@'
	@echo 'Invoking: GCC C++ Linker'
	g++ -g $(SRC) ${GDAL_INCLUDE} $(NETCDF_INCLUDE) $(GDAL_LIBRARIES) $(NETCDF_LIBRARIES) -o bin/$(OBJS) $^
	@echo 'Finished building target: $@ for debugging'