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
//      Edited 201407 M. P. Mendenhall
// --------------------------------------------------------

#ifndef TrackingAction_H
/// Assure header is loaded only once
#define TrackingAction_H

#include "G4UserTrackingAction.hh"

#include <iostream>
#include <fstream>

#include <map>
#include "globals.hh"

class TrackingAction : public G4UserTrackingAction {
public:
    /// Constructor
    TrackingAction() { }
    /// called at start of every track
    void PreUserTrackingAction(const G4Track*);
    /// called at end of every track
    void PostUserTrackingAction(const G4Track*);
};

#endif
