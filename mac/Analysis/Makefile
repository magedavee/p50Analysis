#########################################################
# Makefile for PROSPECT-G4 miscellaneous analysis scripts
#########################################################

# apply implicit rules only for listed file types
.SUFFIXES:
.SUFFIXES: .c .cc .cpp .o

# check for necessary environment variables
ifndef MPM_P2X_ANALYSIS
$(error MPM_P2X_ANALYSIS is not set)
endif
ifndef PG4_LIB_DIR
$(error PG4_LIB_DIR is not set)
endif


CXXFLAGS = -O3 --std=c++11 -fPIC `root-config --cflags` -pedantic -Wall -Wextra \
	-I. -ICommon/ -I../../include/Output/ -I${MPM_P2X_ANALYSIS}
LDFLAGS =  -L. -L$(PG4_LIB_DIR) -lEventLib `root-config --libs` -lMathMore -lgsl -lXMLIO

#
# things to build
#

VPATH = ${MPM_P2X_ANALYSIS}:Common/:Private/:../../src/Output/
objects = AnaUtils.o FileKeeper.o GoldhagenSpectrum.o strutils.o PathUtils.o XMLInfo.o
_examples = PlotFlux PlotPos PlotPrimaries TestCell

# optional HDF5-dependent components
ifeq ($(WITH_HDF5),true)
ifndef HDF5_INSTALL
$(error HDF5_INSTALL is not set)
endif
	LDFLAGS += -L$(HDF5_INSTALL)/lib/ -lhdf5 -lhdf5_hl
	CXXFLAGS += -DWITH_HDF5 -I$(HDF5_INSTALL)/include/
	objects += PG4_HDF5_Reader.o TimeStructure.o IoniTiming.o
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
