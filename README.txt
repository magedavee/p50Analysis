PROSPECT-G4
===========

Geant4-based Monte Carlo simulation for PROSPECT detector

Dependencies
============
cmake
Geant4 (http://geant4.cern.ch); known to compile with version 4.10.0-p2
ROOT (http://root.cern.ch); known to work with version 5.34/18
CRY cosmic ray shower event generator (http://cnr07.llnl.gov/simulation/main.html), version 1.7

For visualization, it's also useful to have some of Geant4's optional graphics dependencies installed
(see Geant4's install guides), e.g. OpenGL and Motif developer libraries.


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
You'll need to tell your computer to look in ./lib/ when searching for libraries; you can do this with:

export LD_LIBRARY_PATH=<path to install directory>/lib/:$LD_LIBRARY_PATH


Running
=======

to run:
bin/PROSPECT-G4 <macro file>

see the mac/ directory for example macros.

The macros named "*_Template.mac" have placeholder values like "%(run_num)i" intended to be
filled in by a launcher script (e.g. LaunchSims.py); you could also make a copy to
fill in by hand.


Output
======

Simulation output is a ROOT TTree with event data, and an XML metadata file (same name as ROOT output,
but with added ".xml" suffix) containing simulation setup information.

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

Running "make <progname>" will compile <progname>.cc linked with the analysis
utilities and other necessary libraries (including the Event.hh classes).
For example, "make MuVeto" will compile MuVeto.cc to an executable named
"MuVeto," which takes a path to a directory full of PROSPECT-G4 output
.root and .xml files as its argument. You can create your own analysis
scripts along similar lines to MuVeto.cc, and compile them without needing
to modify the Makefile.

