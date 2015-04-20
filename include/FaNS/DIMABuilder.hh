/// \file DIMABuilder.hh \brief Geometry for ``DIMA'' 16-channel scintillator detector.
#ifndef DIMADETECTORCONSTRUCTION_HH
#define DIMADETECTORCONSTRUCTION_HH

#include "ScintSegVol.hh"

#include <G4UImessenger.hh>
#include <G4UIdirectory.hh>
#include <G4UIcmdWithABool.hh>

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
class DIMABoxBuilder: public ShellLayerBuilder, public G4UImessenger {
public:
    /// Constructor
    DIMABoxBuilder();
    
    /// Construct geometry
    virtual void _construct();
    
    /// Respond to UI commands
    void SetNewValue(G4UIcommand* command, G4String newValue);
    
    DIMAArrayBuilder myArray;   ///< scintillator tube array
    /// get physical volume defining local coordinates
    virtual G4VPhysicalVolume* getCoordVolume() const { return myArray.getCoordVolume(); }
    
    bool withLeadBrick = false; ///< whether to place lead brick on top for Cf252 gamma blocker (done externally)
    G4ThreeVector brickDim;     ///< dimensions of blocker brick
    G4LogicalVolume* brick_log; ///< logical volume for blocker brick, awaiting possible placement
    
protected:
    /// XML output contents
    virtual void fillNode(TXMLEngine& E);
    
    G4UIdirectory dimaDir;      ///< UI directory for DIMA commands
    G4UIcmdWithABool brickCmd;  ///< UI command for whether to stack gamma blocker brick on top
};

#endif
