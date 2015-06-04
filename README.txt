PROSPECT-G4
===========

Geant4-based Monte Carlo simulation for PROSPECT detector

Dependencies
============
cmake (http://www.cmake.org/), sufficiently new version for other dependencies (>= 3 recommended)

Geant4 (http://geant4.cern.ch); known to compile with versions 4.10.00, 4.10.01
    cmake flags: -DGEANT4_INSTALL_DATA=ON -DGEANT4_BUILD_CXXSTD=c++11

For visualization, it's also useful to have some of Geant4's optional graphics dependencies installed
(see Geant4's install guides), e.g. OpenGL and Motif developer libraries.
    Geant4 cmake flags: -DGEANT4_USE_QT=ON -DGEANT4_USE_XM=ON -DGEANT4_USE_OPENGL_X11=ON -DCMAKE_INSTALL_PREFIX=<install dir>
    
ROOT (http://root.cern.ch); known to work with versions 5.34/18, v6-02-1, v6-03-1

CRY cosmic ray shower event generator (http://cnr07.llnl.gov/simulation/main.html), version 1.7
compile with -fPIC ("export CXXFLAGS=-fPIC")

Doxygen (www.doxygen.org), optional for generating comments-based documentation

HDF5 (http://www.hdfgroup.org/HDF5/) including "high level" libraries, optional for HDF5 output
cmake-based installers provided at https://www.hdfgroup.org/HDF5/release/cmakebuild.html

Building
========
Make sure Geant4 and ROOT environment variables are properly set by "geant4.sh" and "thisroot.sh" setup scripts

You'll need to set the following two environment variables for CRY:
export CRYHOME=<directory where you've installed CRY>/cry_v1.7/
export CRYDATA=${CRYHOME}/data

To build, working in directory where you want the products built (separate from project directory):

cmake <path to project source directory>
make -j<number of processors on your computer>

executable is placed in ./bin/ directory, and a library for ROOT to read output objects is in ./lib/
You'll need to tell your computer to look in ./lib/ when searching for libraries, by:

export PG4_LIB_DIR=<path to install directory>/lib/
export LD_LIBRARY_PATH=${PG4_LIB_DIR}:${LD_LIBRARY_PATH}

optional: 'make doc' to generate Doxygen documentation

For locally-installed HDF5 builds, use cmake with the `-DHDF5_BUILD_HL_LIB=ON` option
    to generate the "high level" library libhdf5_hl.a;
    set environment variables to point to your HDF5 headers and build:

export HDF5_DIR=<HDF5 install directory>/share/cmake/

Running
=======

to run:
bin/PROSPECT-G4 <macro file>

see the mac/ directory for example macros.

The macros named "*_Template.mac" have placeholder values like "%(run_num)i" intended to be
filled in by a launcher script (e.g. LaunchSims.py); you could also make a copy to
fill in by hand.

Some additional environment variables may be needed for specific features, e.g.:
export PG4_AUX=<path to source directory>/Aux/

Output
======

Simulation output is a ROOT TTree or HDF5 tables with event data,
and an XML metadata file (same name as ROOT/HDF5 output, but with added ".xml" suffix)
containing simulation setup information.

The branches of the TTree (named "PG4") vary depending on the systems activated in the analysis;
for example, a muon veto ionization branch named "VetoIoni," which is generated when the muon veto
is enabled. Contents of the branches are from the classes given in Output/Event.hh .
Several of these objects contain variable-length TClonesArray lists of sub-objects;
for example, event primaries are stored in branch "Prim" as a ParticleEvent object,
which contains a list of ParticleVertex describing each primary particle.

Output branches include:

Prim (class ParticleEvent): event primary particles

ScIoni (class IoniClusterEvent): ionizing energy deposition events (separated by particle type
    and scintillator segment) in the liquid scintillator
    
ScN (class NCaptEvent): neutron captures in the scintillator

VetoIoni (class IoniClusterEvent): ionizing energy deposition events (separated by particle
    type) in the optional muon veto layer
    

Analysis
========

Example analysis scripts and a Makefile are contained in the mac/Analysis/ directory.
The Makefile, when "make" is run without arguments, will compile a few analysis utility
'.o' files containing functions/classes useful for many analysis tasks.
These shared utilities are in mac/Analysis/Common.

Running "make <progname>" will compile <progname>.cc linked with the analysis
utilities and other necessary libraries (including the Event.hh classes).
For example, "make MuVeto" will compile MuVeto.cc to an executable named
"MuVeto," which takes a path to a directory full of PROSPECT-G4 output
.root and .xml files as its argument. You can create your own analysis
scripts along similar lines to MuVeto.cc, and compile them without needing
to modify the Makefile.

For access to shared analysis utilities from PROSPECT2x_Analysis in the makefile,
set the environment variable:
    export MPM_P2X_ANALYSIS=<... path to ...>/PROSPECT2x_Analysis/cpp/
    