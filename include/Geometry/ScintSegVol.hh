#ifndef SCINTSEGVOL_HH
#define SCINTSEGVOL_HH

#include "Builder.hh"

/// Base class for a (segmented) scintillator-containing volume
class ScintSegVol: public Builder {
public:
    /// Constructor
    ScintSegVol(const std::string& n): Builder(n), scint_log(NULL) { }

    /// get segment number at position in scintillator local coordinates
    virtual int getSegmentNum(const G4ThreeVector&) const { return 1; }
    
    G4LogicalVolume* scint_log; ///< liquid scintillator volume
};

#endif
