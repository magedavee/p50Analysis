#include "ScatterSlabBuilder.hh"
#include "FluxCounterSD.hh"

#include <cassert>

#include <G4PVPlacement.hh>
#include <G4SystemOfUnits.hh>
#include <G4UnitsTable.hh>
#include <G4SDManager.hh>
#include <G4Box.hh>
#include <G4VisAttributes.hh>

ScatterSlabBuilder::ScatterSlabBuilder(): Builder("ScatterSlab"),
mat(MaterialsHelper::M().Vacuum), thick(1*mm), width(1*m),
slabDir("/geom/slab/"),
thickCmd("/geom/slab/thickness",this),
matCmd("/geom/slab/material",this) {
    
    thickCmd.SetGuidance("Set scattering slab thickness.");
    thickCmd.AvailableForStates(G4State_PreInit);
    
    matCmd.SetGuidance("Set scattering slab material.");
    matCmd.AvailableForStates(G4State_PreInit);
    
    // funny materials for testing purposes
    G4Material* solidCl = new G4Material("solidCl", 1*g/cm3, 1, kStateSolid, MaterialsHelper::M().room_T);
    solidCl->AddMaterial(MaterialsHelper::M().nat_Cl, 1.0);
}

void ScatterSlabBuilder::SetNewValue(G4UIcommand* command, G4String newValue) {
    if(command == &thickCmd) thick = thickCmd.GetNewDoubleValue(newValue);
    else if(command == &matCmd) mat = G4Material::GetMaterial(newValue);
    else G4cout << "Unknown command!" << G4endl;
}

void ScatterSlabBuilder::construct() {
    assert(mat);
    if(!mat) mat = MaterialsHelper::M().Vacuum;
        
    double fluxthick = 1*mm;
    dim = G4ThreeVector(width, width, 2*(thick+fluxthick));
    
    G4Box* slab_vol_box = new G4Box("slab_vol_box", dim[0]/2., dim[1]/2., dim[2]/2.);
    main_log = new G4LogicalVolume(slab_vol_box, MaterialsHelper::M().Vacuum, "Slab_main_log");
    
    G4Box* slab_box = new G4Box("slab_box", dim[0]/2., dim[1]/2., thick/2.);
    slab_log = new G4LogicalVolume(slab_box, mat, "Slab_log");
    slab_log->SetVisAttributes(new G4VisAttributes(G4Color(0,0,1)));
    new G4PVPlacement(NULL, G4ThreeVector(0,0,thick/2.), slab_log, "Slab_phys", main_log, false, 0, false);
    
    G4Box* flux_box = new G4Box("slab_flux_box", dim[0]/2., dim[1]/2., fluxthick/2.);
    slab_flux_log = new G4LogicalVolume(flux_box, mat, "Slab_flux_log");
    slab_flux_log->SetVisAttributes(new G4VisAttributes(G4Color(1,0,0)));
    new G4PVPlacement(NULL, G4ThreeVector(0,0,thick+fluxthick/2.), slab_flux_log, "Slab_fluxout_phys", main_log, false, 0, false);
    new G4PVPlacement(NULL, G4ThreeVector(0,0,-thick-fluxthick/2.), slab_flux_log, "Slab_fluxback_phys", main_log, false, 0, false);
    
    FluxCounterSD* mySD = new FluxCounterSD("SlabFluxSD");
    G4SDManager::GetSDMpointer()->AddNewDetector(mySD);
    slab_flux_log->SetSensitiveDetector(mySD);
}

void ScatterSlabBuilder::fillNode(TXMLEngine& E) {
    addAttr(E, "dim", G4BestUnit(dim,"Length"));
    addAttr(E, "thick", G4BestUnit(thick,"Length"));
    if(mat) addAttr(E, "mat", mat->GetName());
}
