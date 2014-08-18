#########################################################
# Makefile for PROSPECT-G4 miscellaneous analysis scripts
#########################################################

# apply implicit rules only for listed file types
.SUFFIXES:
.SUFFIXES: .c .cc .cpp .o

CXXFLAGS = -O3 --std=c++11 -fPIC `root-config --cflags` -pedantic -Wall -Wextra \
	-I. -I../../include/Output/ 
LDFLAGS =  -L. -L/home/mpmendenhall/Applications/PG4/lib/ -lEventLib `root-config --libs` -lXMLIO

#
# things to build
#

Utils = FileKeeper.o GoldhagenSpectrum.o strutils.o XMLInfo.o
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
