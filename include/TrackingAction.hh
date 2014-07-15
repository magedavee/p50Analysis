// Unrestricted Use - Property of AECL
//
// TrackingAction.hh
// GEANT4 - geant4.9.3.p01
//
// Header File for Custom Track Controller and Processing
//	Contains class functions/variables
//
// --------------------------------------------------------
//	Version 1.01  - 2011/04/29 - A. Ho
// --------------------------------------------------------

#ifndef TrackingAction_H		// Only carries out if object is undefined
#define TrackingAction_H 1		// Defines object

#include "G4UserTrackingAction.hh"	// Specifies base class or parent class

#include <iostream>			// Specifies the classes which allow reading/writing to standard input/output
#include <fstream>

#include <map>				// Specifies classes defining all global parameters and variable types
#include "globals.hh"

/* -------- Class Definition --------- */

class TrackingAction : public G4UserTrackingAction	// This class inherits functions from G4UserTrackingAction
{
  public:	// Constructors and Destructors

    TrackingAction();				// Constructor
    virtual ~TrackingAction();			// Destructor

  public: 	// Accessible Methods

    void PreUserTrackingAction(const G4Track*);		// Function called at start of every track
    void PostUserTrackingAction(const G4Track*);	// Function called at end of every track
};

/* ----------------------------------- */

#endif					// End of the if clause

// EOF
