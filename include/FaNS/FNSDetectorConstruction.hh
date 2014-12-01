#ifndef FNSDETECTORCONSTRUCTION_HH
#define FNSDETECTORCONSTRUCTION_HH

#include "DetectorConstruction.hh"
#include "ScintSegVol.hh"

/// Builder for ``Russian'' fast neutron spectrometer array
class FNSArrayBuilder: public ScintSegVol {
public:
    /// Constructor
    FNSArrayBuilder();
    
    /// Construct geometry
    virtual void construct();
    /// identify hit segment number
    int getSegmentNum(const G4ThreeVector& pos) const;
    
    const int ngrid = 4;        ///< number of segments in each row/column
    double l_seg;               ///< scintillator tube length
    double r_seg;               ///< scintillator tube outer radius
    double t_seg;               ///< scintillator tube wall thickness
    static double spacing;      ///< center-to-center array spacing
};

/// Drop-in replacement for DetectorConstruction, for ``Russian'' spectrometer model
class FNSDetectorConstruction: public DetectorConstruction  {
public:
    /// Constructor
    FNSDetectorConstruction() { }
    
    /// Assembles geometry of the simulation, returns world volume
    virtual G4VPhysicalVolume* Construct();
    
    FNSArrayBuilder myArray;
};

#endif
