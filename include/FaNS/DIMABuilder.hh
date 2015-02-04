/// \file DIMABuilder.hh \brief Geometry for ``DIMA'' 16-channel scintillator detector.
#ifndef DIMADETECTORCONSTRUCTION_HH
#define DIMADETECTORCONSTRUCTION_HH

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
    /// get segment center in scintillator local coordinates
    virtual G4ThreeVector getSegCenter(int n) const;
    /// get physical volume defining local coordinates
    virtual G4VPhysicalVolume* getCoordVolume() const { return main_phys; }
    
    const int ngrid = 4;        ///< number of segments in each row/column
    double l_seg;               ///< scintillator tube length
    double r_seg;               ///< scintillator tube outer radius
    double t_seg;               ///< scintillator tube wall thickness
    double t_guide;             ///< thickness of light guide endcaps
    double t_rack;              ///< thickness of tube holder rack
    static double spacing;      ///< center-to-center array spacing
};

/// Builder for box/supports around DIMA array
class DIMABoxBuilder: public ShellLayerBuilder {
public:
    /// Constructor
    DIMABoxBuilder(): ShellLayerBuilder("DIMABox")  { expand_to_contents = false; myContents = &myArray; }
    
    /// Construct geometry
    virtual void _construct();
    
    DIMAArrayBuilder myArray;   ///< scintillator tube array
};

#endif
