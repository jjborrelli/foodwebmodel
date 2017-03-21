OBJS := FoodWebModel
SRC := src/*
HEADERS := headers/*
ifeq ($(OS),Windows_NT)
GDAL_INCLUDE := -IC:/OSGeo4W64/include
GDAL_LIBRARIES := -LC:/OSGeo4W64/lib
else
GDAL_INCLUDE := `gdal-config --cflags`
GDAL_LIBRARIES := `gdal-config --libs`
endif