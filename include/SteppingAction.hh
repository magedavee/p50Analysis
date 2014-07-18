// Unrestricted Use - Property of AECL
//
// SteppingAction.hh
// GEANT4 - geant4.9.3.p01
//
// Header File for Custom Step Controller and Processing
//      Contains class functions/variables
//
// --------------------------------------------------------
//      Version 1.01 - 2011/04/29 - A. Ho
//      Edited for clarity 2014/07 M. P. Mendenhall
// --------------------------------------------------------

#ifndef SteppingAction_H
/// Assure header is only loaded once
#define SteppingAction_H

#include "G4UserSteppingAction.hh"

#include <iostream>
#include <fstream>

#include <map>
#include "globals.hh"

class SteppingAction : public G4UserSteppingAction {
  public:
    /// constructor
    SteppingAction() {}

    /// Processing to be done at every step - stores track lengths in this simulation
    void UserSteppingAction(const G4Step*);
};

#endif
