#include "FluxCounterBuilder.hh"
#include "FluxCounterSD.hh"
#include "MaterialsHelper.hh"

#include <cassert>

#include <G4SDManager.hh>
#include <G4UnitsTable.hh>
#include <G4Box.hh>

FluxCounterBuilder::FluxCounterBuilder(): Builder("FluxCounter"), main_vis(G4Color(1.,0.,0.)) {
}

void FluxCounterBuilder::construct() {
    // construct volume
    assert(dim[0]*dim[1]*dim[2]);
    G4Box* flux_box = new G4Box("flux_box", dim[0]/2., dim[1]/2., dim[2]/2.);
    main_log = new G4LogicalVolume(flux_box, MaterialsHelper::M().Vacuum, "Flux_main_log");
    main_log->SetVisAttributes(&main_vis);
    
    // assign sensitive detector to volume
    FluxCounterSD* mySD = new FluxCounterSD("FluxSD");
    G4SDManager::GetSDMpointer()->AddNewDetector(mySD);
    main_log->SetSensitiveDetector(mySD);
}
