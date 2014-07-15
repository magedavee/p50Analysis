// Unrestricted Use - Property of AECL
//
// SteppingAction.hh
// GEANT4 - geant4.9.3.p01
//
// Header File for Custom Step Controller and Processing
//	Contains class functions/variables
//
// --------------------------------------------------------
//	Version 1.01 - 2011/04/29 - A. Ho
// --------------------------------------------------------

#ifndef SteppingAction_H		// Only carries out if object is undefined
#define SteppingAction_H 1		// Defines object

#include "G4UserSteppingAction.hh"	// Specifies base class or parent class

#include <iostream>			// Specifies the classes which allow reading/writing to standard input/output
#include <fstream>

#include <map>				// Specifies classes defining all global parameters and variable types
#include "globals.hh"

/* -------- Class Definition --------- */

class SteppingAction : public G4UserSteppingAction	// This class inherits functions from G4UserSteppingAction
{
  public:	// Constructors and Destructors

    SteppingAction();				// Constructor
    virtual ~SteppingAction();			// Destructor

  public: 	// Accessible Methods

    void UserSteppingAction(const G4Step*);		// Processing to be done at every step - stores track lengths in this simulation
};

/* ----------------------------------- */

#endif					// End of the if clause

// EOF
