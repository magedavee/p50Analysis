#ifndef STACKINGACTION_HH
#define STACKINGACTION_HH

#include <G4UserStackingAction.hh>

/// User "stacking action," currently used to optionally stop processing after throwing primaries
class StackingAction: public G4UserStackingAction {
public:
    /// Constructor
    StackingAction(): noTracks(false) { }
    
    /// Set options for processing track
    virtual G4ClassificationOfNewTrack ClassifyNewTrack(const G4Track* aTrack);
    
    bool noTracks;      ///< whether to stop all tracks (to record primaries only)
};

#endif
