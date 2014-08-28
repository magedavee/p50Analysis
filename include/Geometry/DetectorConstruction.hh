#ifndef DetectorConstruction_H
/// Assure header is only loaded once
#define DetectorConstruction_H

#include "BuildingBuilder.hh"
#include "ScintCellBuilder.hh"
#include "ScintSD.hh"
#include "XMLProvider.hh"

#include <G4VUserDetectorConstruction.hh>
#include <G4LogicalVolume.hh>
#include <G4VPhysicalVolume.hh>

class DetectorConstruction: public G4VUserDetectorConstruction, public XMLProvider {
public:
    /// Constructor
    DetectorConstruction();
    /// Destructor
    virtual ~DetectorConstruction() { }

    /// Assembles geometry of the simulation, returns world volume
    G4VPhysicalVolume* Construct();
        
    /// Get reference to active scintillator volume
    G4LogicalVolume* getScintLog() const { return myBuilding.myDetUnit.myDet.myTank.scint_log; }
    
    BuildingBuilder myBuilding;         ///< building containing detector
    ScintCellBuilder myTestCell;        ///< test scintillator cell
    G4VPhysicalVolume* theWorld;        ///< world volume
    G4VPhysicalVolume* ptclSrc;         ///< event generator source volume
    G4VPhysicalVolume* building_phys;   ///< physical building placement
    double worldPad;                    ///< "padding" width of vacuum around "building"
    
private:
    
    G4ThreeVector dim;          ///< overall world volume dimensions
    ScintSD* myScintSD;         ///< sensitive detector for scintillator volume
    
    /// XML output contents
    virtual void fillNode(TXMLEngine& E);
};

#endif
