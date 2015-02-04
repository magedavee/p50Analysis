/// \file ScintSegVol.hh \brief Base class for segmented scintillator volumes.
#ifndef SCINTSEGVOL_HH
#define SCINTSEGVOL_HH

#include "Builder.hh"
#include <G4VPhysicalVolume.hh>
#include <G4VSolid.hh>
#include <G4Material.hh>
#include <G4VisAttributes.hh>

/// Base class for a (segmented) scintillator-containing volume
class ScintSegVol: public Builder {
public:
    /// Constructor
    ScintSegVol(const string& n): Builder(n) { }

    /// get segment number at position in scintillator local coordinates
    virtual int getSegmentNum(const G4ThreeVector&) const { return 0; }
    /// get segment center in scintillator local coordinates
    virtual G4ThreeVector getSegCenter(int) const { return G4ThreeVector(); }
    /// get physical volume defining local coordinates
    virtual G4VPhysicalVolume* getCoordVolume() const { return scint_phys; }
    
    /// assign SD to scintillator volumes
    virtual void setScintSD(G4VSensitiveDetector* SD) { scint_log->SetSensitiveDetector(SD); }
    
    G4LogicalVolume* scint_log = NULL;          ///< liquid scintillator logical volume for setting sensitive detectors
    G4VPhysicalVolume* scint_phys = NULL;       ///< (optional) liquid scintillator physical volume for storing local coordinates
    
protected:
    /// XML output contents
    virtual void fillNode(TXMLEngine& E);
};


/// Single-segment chunk of material (specify solid, material in subclass)
class ScintChunk: public ScintSegVol {
public:
    /// Constructor
    ScintChunk(const string& cnm, int n): ScintSegVol(cnm), num(n) { }
    /// get segment number at position in scintillator local coordinates
    virtual int getSegmentNum(const G4ThreeVector&) const { return num; }
    /// Construct geometry
    virtual void construct();
    
    G4VSolid* mySolid = NULL;           ///< solid shape of chunk
    G4Material* myMat = NULL;           ///< material for chunk
    G4VisAttributes* myVis = NULL;      ///< visualization options for chunk
    const int num;                      ///< segment ID number
};

#endif
