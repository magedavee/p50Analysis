#ifndef DIMADETECTORCONSTRUCTION_HH
#define DIMADETECTORCONSTRUCTION_HH

#include "DetectorConstruction.hh"
#include "ScintSegVol.hh"

/// Builder for ``DIMA'' fast neutron spectrometer array
class DIMAArrayBuilder: public ScintSegVol {
public:
    /// Constructor
    DIMAArrayBuilder();
    
    /// Construct geometry
    virtual void construct();
    /// identify hit segment number
    int getSegmentNum(const G4ThreeVector& pos) const;
    
    const int ngrid = 4;        ///< number of segments in each row/column
    double l_seg;               ///< scintillator tube length
    double r_seg;               ///< scintillator tube outer radius
    double t_seg;               ///< scintillator tube wall thickness
    double t_guide;             ///< thickness of light guide endcaps
    static double spacing;      ///< center-to-center array spacing
};

/// Builder for box/supports around DIMA array
class DIMABoxBuilder: public ShellLayerBuilder {
public:
    /// Constructor
    DIMABoxBuilder(): ShellLayerBuilder("DIMABox")  { }
    
    /// Construct geometry
    virtual void construct();
    
    DIMAArrayBuilder myArray;
};

/// Drop-in replacement for DetectorConstruction, for ``Russian'' spectrometer model
class DIMADetectorConstruction: public DetectorConstruction  {
public:
    /// Constructor
    DIMADetectorConstruction() { }
    
    /// Assembles geometry of the simulation, returns world volume
    virtual G4VPhysicalVolume* Construct();
    
    DIMABoxBuilder myBox;
};

#endif
