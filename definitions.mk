OBJS := FoodWebModel
COMPS := FoodWebModel ReadNetCDFFile
SRC := $(foreach d, $(COMPS), src/${d}.cpp)
HEADERS := SRC := $(foreach d, $(COMPS), headers/${d}.hpp)
ifeq ($(OS),Windows_NT)
GDAL_INCLUDE := -IC:/OSGeo4W64/include
GDAL_LIBRARIES := -LC:/OSGeo4W64/lib
NETCDF_INCLUDE := -I"C:/Users/manu_/Downloads/netcdf/netcdf-cxx4-master/cxx4"
NETCDF_LIBRARIES := -L"C:/Users/manu_/Downloads/netcdf/netcdf-cxx4-master/cxx4"
else
GDAL_INCLUDE := `gdal-config --cflags`
GDAL_LIBRARIES := `gdal-config --libs`
NETCDF_INCLUDE := -I"/usr/local/share/netcdf/include"
NETCDF_LIBRARIES := -L"/usr/local/share/netcdf/lib" -L"/usr/local/share/netcdf-c/netcdf-4.4.1.1/liblib" -lnetcdf_c++4 -lnetcdf
endif