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
class EventMessenger;
class RunAction;
class PrimaryGeneratorAction;


class EventAction : public G4UserEventAction {
    
    friend class EventMessenger;

public:
    /// Donstructor
    EventAction();
    /// Destructor
    virtual ~EventAction();	
    
    /// Called to start an event (particle launch)
    void BeginOfEventAction(const G4Event*);
    /// Called to finish an event and contains event-specific calculations and instructions
    void EndOfEventAction(const G4Event*);

    void SetPhotonTravelTimeTable(G4bool);
    void SetPrintPerNEvents(G4int);

protected:
    
    // Change photon travel time output table
    void SetPhotonTravelTimeBinWidth(G4double bin) { tBin = bin; };	
    void SetPhotonTravelTimeLowerRef(G4double ref) { tRef = ref; };
    void SetPhotonTravelTimeBinCount(G4int num) { tNum = num; };

private:

    EventMessenger* event_messenger;        ///< Messenger class

    G4int InnerHCIDNHit, PMTHCIDPhotHit, VetoHCIDPhotHit, InnerHCIDEDep,VetoHCIDEDep, InnerHCIDPHit;
    std::map<G4double,G4int>* EventTime;    ///< Table to store photon travel time

    G4double tBin;  ///< Photon travel time histogram bin width
    G4double tRef;  ///< Photon travel time histogram lowest bin start
    G4int tNum;     ///< Photon travel time histogram number of bins

    G4int fPrintPerEvent;                   ///< Event print modifier
    G4bool fFirstNeutronRawPrint;           ///< First print modifiers
    G4bool fFirstLSPhotonRawPrint;
};

#endif
