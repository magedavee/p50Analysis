#########################################################
# Makefile for PROSPECT-G4 miscellaneous analysis scripts
#########################################################

# apply implicit rules only for listed file types
.SUFFIXES:
.SUFFIXES: .c .cc .cpp .o

CXXFLAGS = -O3 --std=c++11 -fPIC `root-config --cflags` -pedantic -Wall -Wextra \
	-I. -ICommon/ -I../../include/Output/ -I${MPM_P2X_ANALYSIS}
LDFLAGS =  -L. -L$(PG4_LIB_DIR) -lEventLib `root-config --libs` -lMathMore -lgsl -lXMLIO

#
# things to build
#

VPATH = ${MPM_P2X_ANALYSIS}:Common/:Private/:../../src/Output/
objects = AnaUtils.o FileKeeper.o GoldhagenSpectrum.o strutils.o XMLInfo.o
_examples = PlotFlux PlotPos PlotPrimaries TestCell

# optional HDF5-dependent components
ifeq ($(WITH_HDF5),true)
	LDFLAGS += -L$(HDF5_LIBDIR) -lhdf5 -lhdf5_hl
	CXXFLAGS += -DWITH_HDF5 -I$(HDF5_SRCDIR)
	objects += HDF5Reader.o TimeStructure.o IoniTiming.o DetectorResponse.o
	_examples += CalcDetectorResponse SelectH5Events
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
