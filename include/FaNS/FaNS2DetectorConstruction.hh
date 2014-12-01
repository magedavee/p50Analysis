#ifndef FANS2DETECTORCONSTRUCTION_HH
#define FANS2DETECTORCONSTRUCTION_HH

#include "DetectorConstruction.hh"
#include "ScintSegVol.hh"

/// Builder for FaNS2 scintillator array
class FaNS2ScintArrayBuilder: public ScintSegVol {
public:
    /// Constructor
    FaNS2ScintArrayBuilder();
    
    /// Construct geometry
    virtual void construct();
    /// identify hit segment number
    int getSegmentNum(const G4ThreeVector& pos) const;
    
    const int ngrid = 4;                ///< number of segments in each row/column
    double l_bar;                       ///< scintillator bar length
    double w_bar;                       ///< scintillator bar width
    double theta;                       ///< scintillator bar rotation angle
    static double array_spacing;        ///< center-to-center array spacing
    
protected:
    const double cos_pw;
    const double sin_pw;
};

/// Builder for FaNS2 3He tubes array
class FaNS23HeArrayBuilder: public ScintSegVol {
public:
    /// Constructor
    FaNS23HeArrayBuilder();
    
    /// Construct geometry
    virtual void construct();
    /// identify hit segment number
    int getSegmentNum(const G4ThreeVector& pos) const;
    
    const int ngrid = 5;        ///< number of 3He tubes in each row/column
    double l_tube;              ///< tube (outer) length
    double r_tube;              ///< tube (outer) radius
    double t_tube;              ///< tube wall thickness
};

/// Builder for FaNS2 muon veto paddles
class FaNS2MuVetoBuilder: public ScintSegVol {
public:
    /// Constructor
    FaNS2MuVetoBuilder();
    
    /// Construct geometry
    virtual void construct();
    /// identify hit segment number
    int getSegmentNum(const G4ThreeVector&) const { return 55; }
    
    double w_slab;              ///< width of veto slab
    double t_slab;              ///< thicknes of veto slab
};

/// Drop-in replacement for DetectorConstruction, for FaNS2 detector model
class FaNS2DetectorConstruction: public DetectorConstruction  {
public:
    /// Constructor
    FaNS2DetectorConstruction() { }
    
    /// Assembles geometry of the simulation, returns world volume
    virtual G4VPhysicalVolume* Construct();
    
    FaNS2ScintArrayBuilder myScintArray;
    FaNS23HeArrayBuilder my3HeArray;
    FaNS2MuVetoBuilder myVeto;
};

#endif
