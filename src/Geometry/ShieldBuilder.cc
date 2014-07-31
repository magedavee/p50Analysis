#include "ShieldBuilder.hh"
#include "MaterialsHelper.hh"

#include <G4SystemOfUnits.hh>
#include <G4Box.hh>
#include <G4PVPlacement.hh>

ShieldBuilder::ShieldBuilder() {
}

void ShieldBuilder::construct() {
    
    layers.push_back(ShieldLayerSpec(5*cm, MaterialsHelper::Air, G4Colour(0.,0.,1.)));
    layers.push_back(ShieldLayerSpec(10*cm, MaterialsHelper::LiPoly, G4Colour(1.,0.,0.)));
    layers.push_back(ShieldLayerSpec(3*cm, MaterialsHelper::nat_Pb, G4Colour(0.,1.,0.)));
    layers.push_back(ShieldLayerSpec(47*cm, MaterialsHelper::BPoly, G4Colour(0.,1.,0.)));
    
    myDet.construct();
    main_log = myDet.main_log;
    dim = myDet.getDimensions();
    
    // add each layer
    uint nlayers = 0;
    for(std::vector<ShieldLayerSpec>::iterator it = layers.begin(); it != layers.end(); it++) {
        if(!it->mat) continue;
        nlayers++;
        
        dim += G4ThreeVector(2*it->side_thick, 2*it->side_thick, it->top_thick + it->bottom_thick);
        
        G4Box* shield_box = new G4Box("shield_box", dim[0]/2., dim[1]/2., dim[2]/2.);
        G4LogicalVolume* s_log = new G4LogicalVolume(shield_box, it->mat, "Shield_layer_log_"+to_str(nlayers));
        s_log->SetVisAttributes(&it->vis);
        
        new G4PVPlacement(NULL, G4ThreeVector(0,0,(it->bottom_thick-it->top_thick)/2.),
                          main_log, "Shield_layer_phys_"+to_str(nlayers), s_log, false, 0, true);
        
        main_log = s_log;
    }
    
}
