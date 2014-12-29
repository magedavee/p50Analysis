#ifndef PR2SHIELDBUILDER_HH
#define PR2SHIELDBUILDER_HH

#include "Builder.hh"
#include "ScintSegVol.hh"

/// Builder for PROSPECT-2 shielding cave
class PR2ShieldBuilder: public ShellLayerBuilder {
public:
    /// constructor
    PR2ShieldBuilder(): ShellLayerBuilder("PROSPECT2") { }
    
    /// Construct geometry
    void construct();
    
    G4LogicalVolume* cave_log;  ///< internal cave volume

protected:
    /// XML output contents
    virtual void fillNode(TXMLEngine& E);
};

/// Builder for PROSPECT-2 muon veto
class PR2MuVetoBuilder: public ScintSegVol {
public:
    /// Constructor
    PR2MuVetoBuilder(): ScintSegVol("PR2_Veto") { }
    
    /// Construct geometry
    void construct();
    
    /// get segment number at position in scintillator local coordinates
    virtual int getSegmentNum(const G4ThreeVector& x) const;
};

#endif
