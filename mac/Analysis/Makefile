#########################################################
# Makefile for PROSPECT-G4 miscellaneous analysis scripts
#########################################################

# apply implicit rules only for listed file types
.SUFFIXES:
.SUFFIXES: .c .cc .cpp .o

CXXFLAGS = -O3 --std=c++11 -fPIC `root-config --cflags` -pedantic -Wall -Wextra \
	-I. -ICommon/ -I../../include/Output/ 
LDFLAGS =  -L. -L$(PG4_LIB_DIR) -lEventLib `root-config --libs` -lXMLIO

#
# things to build
#

VPATH = Common/

Utils = AnaUtils.o FileKeeper.o GoldhagenSpectrum.o PathUtils.o strutils.o XMLInfo.o
objects = $(Utils)

all: $(objects)

# generic rule for everything else .cc
% : %.cc $(objects)
	$(CXX) $(CXXFLAGS) $(objects) $< $(LDFLAGS) -o $@

#
# cleanup
#
.PHONY: clean
clean:
	-rm -f *.o
	-rm -f MuonCal MuVeto nScatter PlotFlux PlotPos PlotPrimaries PlotXS ProtonSlab TestCell
