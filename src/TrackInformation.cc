// Unrestricted Use - Property of AECL
//
// TrackInformation.cc
// GEANT4 - geant4.9.3.p01
//
// Class File for Custom Track Information Storage
//	Contains definitions for functions in header file
//
// --------------------------------------------------------
//	Version 1.01 - 2011/04/29 - A. Ho
// --------------------------------------------------------

#include "TrackInformation.hh"		// Specifies the file which contains the class structure

#include "OpticalTrajectory.hh"		// Specifies user-defined classes which are called upon in this class
#include "TrackingAction.hh"

#include "G4ios.hh"			// Specifies classes which allow reading/writing into standard input/output

#include "globals.hh"			// Specifies class defining all global constants and variable types

	// ****** Constructor ****** //
TrackInformation::TrackInformation()
{
  status = false;	// Determines whether to draw track or not - optical photons
}

	// ****** Destructor ****** //
TrackInformation::~TrackInformation()
{;}

// EOF
