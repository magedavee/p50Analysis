// Unrestricted Use - Property of AECL
//
// EventAction.hh
// GEANT4 - geant4.9.3.p01
//
// Header File for Custom Event Controller and Processing
//	Contains class functions/variables
//
// --------------------------------------------------------
//	Version 1.01 - 2011/04/29 - A. Ho
// --------------------------------------------------------

#ifndef EventAction_H			// Only carries out if object is not defined
#define EventAction_H 1			// Defines object

#include "G4UserEventAction.hh"		// Specifies base class or parent class

#include "G4ios.hh"			// Specifies the classes which allow reading/writing to standard input/output
#include <iostream>
#include <fstream>

#include <map>				// Specifies classes defining all global parameters and variable types
#include "globals.hh"

class G4Event;				// Structures necessary for class definition
class EventMessenger;
class RunAction;
class PrimaryGeneratorAction;

/* -------- Class Definition --------- */

class EventAction : public G4UserEventAction	// This class inherits functions from G4UserEventAction
{
  friend class EventMessenger;

  public:	// Constructors and Destructors

    EventAction();			// Constructor
    virtual ~EventAction();		// Destructor

  public:	// Accessible Methods

    void BeginOfEventAction(const G4Event*);	// Called to start an event (particle launch)
    void EndOfEventAction(const G4Event*);	// Called to finish an event and contains event-specific calculations and instructions

    void SetPhotonTravelTimeTable(G4bool);				// Modify various outputs
    void SetPrintPerNEvents(G4int);

  protected:	// Internal Methods

    void SetPhotonTravelTimeBinWidth(G4double bin) { tBin = bin; };	// Change photon travel time output table
    void SetPhotonTravelTimeLowerRef(G4double ref) { tRef = ref; };
    void SetPhotonTravelTimeBinCount(G4int num) { tNum = num; };

  private:	// Member Data

    EventMessenger* event_messenger;		// Messenger class

    G4int InnerHCIDNHit, PMTHCIDPhotHit, VetoHCIDPhotHit, InnerHCIDEDep,VetoHCIDEDep, InnerHCIDPHit;
    std::map<G4double,G4int>* EventTime;	// Table to store photon travel time

    G4double tBin, tRef;
    G4int tNum;

    G4int fPrintPerEvent;			// Event print modifier
    G4bool fFirstNeutronRawPrint;		// First print modifiers
    G4bool fFirstLSPhotonRawPrint;
};

/* ----------------------------------- */

#endif					// End of if clause

// EOF
