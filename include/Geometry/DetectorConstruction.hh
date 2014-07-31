#ifndef DetectorConstruction_H
/// Assure header is only loaded once
#define DetectorConstruction_H

#include "BuildingBuilder.hh"
#include "ScintSD.hh"

#include <G4VUserDetectorConstruction.hh>
#include <G4LogicalVolume.hh>

class DetectorConstruction: public G4VUserDetectorConstruction {
public:
    /// Constructor
    DetectorConstruction() { }
    /// Destructor
    virtual ~DetectorConstruction() { }

    /// Assembles geometry of the simulation, returns world volume
    G4VPhysicalVolume* Construct();
        
    /// Get reference to active scintillator volume
    G4LogicalVolume* getScintLog() const { return myBuilding.myDetUnit.myDet.myTank.scint_log; }
    
    BuildingBuilder myBuilding; ///< building containing detector

private:
    
    // Sensitive Detectors
    ScintSD* myScintSD;         ///< sensitive detector for scintillator volume
};

#endif
