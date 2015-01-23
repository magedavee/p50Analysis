/// \file EventAction.hh \brief Geant4 event action user class.
#ifndef EventAction_H
/// Assure header file is only loaded once
#define EventAction_H

#include <TStopwatch.h>
#include <G4UserEventAction.hh>

/// Geant4 event action user class
class EventAction : public G4UserEventAction {
public:
    /// Constructor
    EventAction() { }
    
    /// Called to start an event (particle launch)
    void BeginOfEventAction(const G4Event*);
    /// Called to finish an event and contains event-specific calculations and instructions
    void EndOfEventAction(const G4Event*);
    /// get computation time spent so far
    double getCPUTime();
    /// decide whether to keep event four output
    bool keepEvent() const;
    
protected:
    TStopwatch timer;   ///< event computation time timer
};

#endif
