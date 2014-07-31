PROSPECT-G4
===========

Geant4-based Monte Carlo simulation for PROSPECT detector

Dependencies
============
cmake
Geant4 (http://geant4.cern.ch); known to compile with version 4.10.0-p2
ROOT (http://root.cern.ch); known to work with version 5.34/18
CRY cosmic ray shower event generator (http://cnr07.llnl.gov/simulation/main.html), version 1.7


Building and running
====================
Make sure Geant4 and ROOT environment variables are properly set by "geant4.sh" and "thisroot.sh" setup scripts

You'll need to set the following two environment variables for CRY:
export CRYHOME=<directory where you've installed CRY>/cry_v1.7/
export CRYDATA=${CRYHOME}/data

To build, working in directory where you want the products built (separate from project directory):

cmake <path to project source directory>
make -j<number of processors on your computer>

executable is placed in ./bin/ directory, and a library for ROOT to read output objects is in ./lib/

To run:
bin/PROSPECT-G4 <macro file>

see the mac/ directory for example macros.
