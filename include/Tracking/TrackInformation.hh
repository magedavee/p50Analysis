// Unrestricted Use - Property of AECL
//
// TrackInformation.hh
// GEANT4 - geant4.9.3.p01
//
// Header File for Custom Track Information Storage
//	Contains class functions/variables
//
// --------------------------------------------------------
//	Version 1.01 - 2011/04/29 - A. Ho
// --------------------------------------------------------

#ifndef TrackInformation_H		// Only carries out if object is undefined
#define TrackInformation_H 1		// Defines object

#include "G4VUserTrackInformation.hh"	// Specifies base class or parent class

#include <iostream>			// Specifies the classes which allow reading/writing to standard input/output
#include <fstream>

#include <map>				// Specifies classes defining all global parameters and variable types
#include "globals.hh"

/* -------- Class Definition --------- */

class TrackInformation : public G4VUserTrackInformation	// This class inherits functions from G4VUserTrackInformation
{
  public:	// Constructors and Destructors

    TrackInformation();				// Constructor
    virtual ~TrackInformation();			// Destructor

  public: 	// Accessible Methods

    void SetTrackStatusFlag(G4bool flag) { status = flag; };
    G4bool GetTrackStatus() { return status; };

  private:

    G4bool status;
};

/* ----------------------------------- */

#endif					// End of the if clause

// EOF
