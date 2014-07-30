#include "BuildingBuilder.hh"
#include "MaterialsHelper.hh"

#include <G4SystemOfUnits.hh>
#include <G4Box.hh>
#include <G4RotationMatrix.hh>
#include <G4PVPlacement.hh>

BuildingBuilder::BuildingBuilder(): main_log(NULL),
dim(6.*m, 6.*m, 3.*m), wall_thick(0.75*m), wall_vis(G4Colour(0.3, 0.4, 0.4)) { }

void BuildingBuilder::construct() {
    
    myDet.construct();
    
    G4Box* wall_box = new G4Box("wall_box", dim[0]/2., dim[1]/2., dim[2]/2.);
    main_log = new G4LogicalVolume(wall_box, MaterialsHelper::Concrete, "Building_main_log");
    main_log->SetVisAttributes(&wall_vis);
    
    G4Box* air_box = new G4Box("air_box", dim[0]/2.-wall_thick, dim[1]/2.-wall_thick, dim[2]/2.-wall_thick);
    air_log = new G4LogicalVolume(air_box, MaterialsHelper::Air, "Building_air_log");
    air_log->SetVisAttributes(&wall_vis);
    new G4PVPlacement(NULL, G4ThreeVector(), air_log, "Building_air_phys", main_log, false, 0, false);
    
    G4RotationMatrix* detRot = new G4RotationMatrix();
    detRot->rotateX(90*deg);
    new G4PVPlacement(detRot, G4ThreeVector(0.,0.,0.), myDet.main_log, "Det_phys", air_log, false, 0, true);    
}
