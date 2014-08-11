#ifndef EventAction_H
/// Assure header file is only loaded once
#define EventAction_H

#include <G4UserEventAction.hh>

class EventAction : public G4UserEventAction {
public:
    /// Constructor
    EventAction() { }	
    
    /// Called to start an event (particle launch)
    void BeginOfEventAction(const G4Event*);
    /// Called to finish an event and contains event-specific calculations and instructions
    void EndOfEventAction(const G4Event*);
};

#endif
