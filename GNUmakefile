name :=  ShortBaseline_v1_01
G4TARGET := $(name)
G4EXLIB := true

#export G4LIB_BUILD_STATIC=1
#export G4LIB_BUILD_SHARED=
EXTRALIBS += -L$(CRYHOME)/lib -lCRY
CPPFLAGS  += -I$(CRYHOME)/src

.PHONY: all
all: dict.cxx libEventLib.so lib bin

dict.cxx :
	rootcint -f dict.cxx -c include/Event.hh include/EventLinkDef.h
 
libEventLib.so: dict.cxx src/Event.cc
	g++ -shared -o$@ -fPIC `root-config --ldflags` $(CPPFLAGS) -I$(ROOTSYS)/include $^

myclean :
	rm -rf dict.* $(G4TMP)/$(G4SYSTEM)/$(name)
	rm -rf *.so

include $(G4INSTALL)/config/architecture.gmk

G4NOHIST := true
CPPFLAGS += $(shell root-config --cflags)
# Here comes dict.cxx
LDFLAGS  += $(shell root-config --libs) dict.cxx

include $(G4INSTALL)/config/binmake.gmk
