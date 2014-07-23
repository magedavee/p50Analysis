// Unrestricted Use - Property of AECL
//
// EventAction.hh
// GEANT4 - geant4.9.3.p01
//
// Header File for Custom Event Controller and Processing
//  Contains class functions/variables
//
// --------------------------------------------------------
//  Version 1.01 - 2011/04/29 - A. Ho
//  Edited for clarity 2014/07 M. P. Mendenhall
// --------------------------------------------------------

#ifndef EventAction_H
/// Assure header file is only loaded once
#define EventAction_H

#include "G4UserEventAction.hh"

#include "G4ios.hh"
#include <iostream>
#include <fstream>

#include <map>
#include "globals.hh"

class G4Event;
class RunAction;
class PrimaryGeneratorAction;

class EventAction : public G4UserEventAction {
public:
    /// Constructor
    EventAction() { }	
    
    /// Called to start an event (particle launch)
    void BeginOfEventAction(const G4Event*);
    /// Called to finish an event and contains event-specific calculations and instructions
    void EndOfEventAction(const G4Event*);

private:

    G4int InnerHCIDEDep;                ///< ID number for hit collections
};

#endif
