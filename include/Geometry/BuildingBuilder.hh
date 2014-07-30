#ifndef BUILDINGBUILDER_HH
/// Assure this header is only loaded once
#define BUILDINGBUILDER_HH

#include "DetVolBuilder.hh"

#include <G4LogicalVolume.hh>
#include <G4ThreeVector.hh>
#include <G4VisAttributes.hh>

/// Builder for building containing detector
class BuildingBuilder {
public:
    /// Constructor
    BuildingBuilder();
    
    /// Construct geometry
    void construct();
    
    G4LogicalVolume* main_log;  ///< main mother volume: concrete building
    G4LogicalVolume* air_log;   ///< air space inside building
    
    G4ThreeVector dim;          ///< outer dimensions
    double wall_thick;          ///< thickness of building walls
    
    DetVolBuilder myDet;        ///< detector assembly
    
private:
    
    G4VisAttributes wall_vis;  ///< visualization settings for shell
};

#endif
