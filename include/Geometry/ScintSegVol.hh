#ifndef SCINTSEGVOL_HH
#define SCINTSEGVOL_HH

#include "Builder.hh"
#include <G4VPhysicalVolume.hh>

/// Base class for a (segmented) scintillator-containing volume
class ScintSegVol: public Builder {
public:
    /// Constructor
    ScintSegVol(const string& n): Builder(n), scint_log(NULL), scint_phys(NULL) { }

    /// get segment number at position in scintillator local coordinates
    virtual int getSegmentNum(const G4ThreeVector&) const { return 1; }
    
    /// assign SD to scintillator volumes
    virtual void setScintSD(G4VSensitiveDetector* SD) { scint_log->SetSensitiveDetector(SD); }
    
    G4LogicalVolume* scint_log;         ///< liquid scintillator logical volume for setting sensitive detectors
    G4VPhysicalVolume* scint_phys;      ///< (optional) liquid scintillator physical volume for storing local coordinates
};

#endif
