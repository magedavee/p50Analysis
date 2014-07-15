// Unrestricted Use - Property of AECL
//
// SteppingVerbose.hh
// GEANT4- geant4.9.3.p01
//
// Header File for Custom Step Status Display
// 	Contains class functions/variables
//
// --------------------------------------------------------
//	Version 1.01 - 2011/04/29 - A. Ho
// --------------------------------------------------------

#ifndef SteppingVerbose_H		// Only carries out if object is undefined
#define SteppingVerbose_H 1		// Defines object

#include "G4SteppingVerbose.hh"		// Specifies base class or parent class

/* -------- Class Definition --------- */

class SteppingVerbose : public G4SteppingVerbose	// This class inherits functions from G4SteppingVerbose
{
  public:	// Constructors and Destructors

    SteppingVerbose();			// Constructor
    virtual ~SteppingVerbose();		// Destructor

  public:	// Accessible Methods

    void StepInfo();			// Obtains and displays information about the step if requested by stepping verbose level
    void TrackingStarted();		// Obtains and displays information about the tracking if requested by stepping verbose level
};

/* ----------------------------------- */

#endif					// End of the if clause

// EOF
