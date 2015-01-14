/// \file DetectorConstruction.hh \brief Geant4 detector construction user class.
#ifndef DETECTORCONSTRUCTION_HH
/// Assure header is only loaded once
#define DETECTORCONSTRUCTION_HH

#include "Builder.hh"
#include "BuildingBuilder.hh"
#include "PR2ShieldBuilder.hh"
#include "PR20CellBuilder.hh"
#include "PR20ShieldBuilder.hh"
#include "ShieldBuilder.hh"
#include "ScintCellBuilder.hh"
#include "ScatterSlabBuilder.hh"
#include "SphereShellBuilder.hh"
#include "ScintSD.hh"

#include <G4VUserDetectorConstruction.hh>
#include <G4LogicalVolume.hh>
#include <G4VPhysicalVolume.hh>
#include <G4UImessenger.hh>
#include <G4UIdirectory.hh>
#include <G4UIcmdWithAString.hh>

/// Geant4 detector construction user class
class DetectorConstruction: public G4VUserDetectorConstruction, public ShellLayerBuilder,  public G4UImessenger {
public:
    /// Constructor
    DetectorConstruction();
    
    /// Assembles geometry of the simulation, returns world volume
    virtual G4VPhysicalVolume* Construct();
    /// world layer shell construction
    virtual void _construct();
    
    /// Respond to UI commands
    void SetNewValue(G4UIcommand* command, G4String newValue);
    
    /// Get reference to active scintillator volume (or equivalent)
    ScintSegVol* getScint();
    
    /// available simulation geometry modes
    enum buildMode {
        PROSPECT,       ///< PROSPECT detector
        PROSPECT2,      ///< PROSPECT-2 small prototype
        PROSPECT20,     ///< PROSPECT-20 single cell prototype
        P20INNER,       ///< PROSPECT-20 with inner shield only
        TEST_CELL,      ///< scintillator testing cell
        SLAB,           ///< material slab
        SPHERE          ///< concentric spheres
    } mode;             ///< what kind of system to construct
    string modeName = "PROSPECT";       ///< name of active mode
    
    BuildingBuilder myBuilding;         ///< building containing detector
    ShieldBuilder myPRShield;           ///< PROSPECT shielded detector
    PR2ShieldBuilder myPR2Shield;       ///< PROSPECT-2 shielding cave
    vector<PR2MuVetoBuilder*> myPR2Veto;        ///< PROSPECT-2/20 muon vetos
    ScintCellBuilder myTestCell;        ///< test scintillator-filled cell
    PR20CellBuilder myPR20Cell;         ///< PROSPECT-20 cell
    PR20ShieldBuilder myPR20Shield;     ///< PROSPECT-20 shield
    ScatterSlabBuilder mySlab;          ///< toy slab geometry
    SphereShellBuilder mySphere;        ///< toy sphere geometry
    
    G4VPhysicalVolume* theWorld = NULL; ///< world volume
    G4VPhysicalVolume* ptclSrc = NULL;  ///< optional event generator source volume
    G4VPhysicalVolume* ptclTrg = NULL;  ///< optional event generator target volume
    ShellLayerSpec worldShell;          ///< world outer vacuum layer dimensions
    
protected:
    
    ScintSD* myScintSD = NULL;          ///< sensitive detector for scintillator volume
    G4UIdirectory geomDir;              ///< UI directory for construction geometry commands
    G4UIcmdWithAString modeCmd;         ///< UI command for setting construction mode
    
    /// XML output contents
    virtual void fillNode(TXMLEngine& E);
};

#endif
