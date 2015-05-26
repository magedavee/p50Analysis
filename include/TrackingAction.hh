/// \file TrackingAction.hh \brief Geant4 user tracking action
#ifndef TRACKINGACTION_HH
#define TRACKINGACTION_HH

#include <G4UserTrackingAction.hh>

/// User action called for each track; used to identify neutron production
class TrackingAction: public G4UserTrackingAction {
public:
    /// Constructor
    TrackingAction() {}
    /// start-of-track routines (identify neutrons)
    virtual void PreUserTrackingAction(const G4Track* aTrack);
    /// end-of-track routines
	virtual void PostUserTrackingAction(const G4Track*);
};

 #endif
