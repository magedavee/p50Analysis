#########################################################
# Makefile for PROSPECT-G4 miscellaneous analysis scripts
#########################################################

# apply implicit rules only for listed file types
.SUFFIXES:
.SUFFIXES: .c .cc .cpp .o

CXXFLAGS = -O3 --std=c++11 -fPIC `root-config --cflags` -pedantic -Wall -Wextra \
	-I. -ICommon/ -I../../include/Output/ 
LDFLAGS =  -L. -L$(PG4_LIB_DIR) -lEventLib `root-config --libs` -lMathMore -lXMLIO

#
# things to build
#

VPATH = Common/:Private/:../../src/Output/
objects = AnaUtils.o FileKeeper.o GoldhagenSpectrum.o strutils.o TimeStructure.o XMLInfo.o
_examples = PlotFlux PlotIBD PlotPos PlotPrimaries TestCell

# optional HDF5-dependent components
ifeq ($(WITH_HDF5),true)
	LDFLAGS += -L$(HDF5_DIR)/bin/ -lhdf5 -lhdf5_hl
	CXXFLAGS += -DWITH_HDF5
	objects += HDF5Reader.o
	_examples += DetectorResponse
endif

all: $(objects)
examples: $(objects) $(_examples)

# generic rule for everything else .cc
% : %.cc $(objects)
	$(CXX) $(CXXFLAGS) $(objects) $< $(LDFLAGS) -o $@

#
# cleanup
#
.PHONY: clean
clean:
	-rm -f *.o
	-rm -f $(_examples)
